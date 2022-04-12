/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: REGIONS.C  -Handles regions on the screen (for mouse etc...)
   Author: Chris Phillips
   [GEH removed]dialog modifications by Michael Branham
   ========================================================================
   Contains the following internal functions: 
   
   Contains the following general functions:
   add_key              -adds a key (zero-size region) to the command list                                       
   add_region           -Adds a Region to the screen
   del_region           -Deletes a region from the screen
   del_all_regions      -Wipes the screen clear of regions
   check_regions        -runs through and checks valid regions for activation
   init_regions         -initializes regions (just calls del_all_regions)
   push_regions         -push the cur region stack pointer
   pop_regions          -pop the cur region stack pointer

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
//#include <dos.h>
#include <string.h>

#include "SYSTEM.H"
#include "MACHINE.H"
#include "ENGINE.H"

#include "REGIONS.H"
#include "PANEL.H"

#include "STRMGR.H"
#include "SOUND.HXX"

/* ------------------------------------------------------------------------
   ®RM250¯Notes
   ------------------------------------------------------------------------ */
	//REVISIT:Do we need any more functionality here?
	//                       Should val be a PTR so it can be modified by the region func.
	//                       ^ No, cast a PTR to a LONG on add_key, case LONG to PTR
	//                       in function to get data. Remember to make sure the pointer
	//                       doesn't become invalid or move before it's used.(GEH)

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
//  ABC used to be 300
#define MAX_REGIONS						500
#define MAX_STACK_DEPTH					15
#define ticsDISPLAY_TIP_TIME			50
//#define ticsWAIT_BEFORE_DISPLAY		40
#define ticsWAIT_BEFORE_DISPLAY		1

typedef struct
{
	LONG flags;
	LONG x,y;       /*rectangle for region*/
	LONG w,h;
	LONG key;       /*alternate key press if any*/
	LONG val;       /*Value to pass to function*/
	LONG val2;      /*Second value to pass to function*/
	PFVLL function;
	LONG id;        /* id for this critter */
	LONG lTipIndex; /* index into tooltip array for the tooltip string */
} REGION;

typedef enum
{
	REGION_INVALID = 0,
	REGION_INACTIVE,
	REGION_ACTIVE,
	REGION_SELECTED,
	REGION_KEYTYPE
} REGION_FLAG;

// variables for tooltips
typedef struct
{
	LONG    lId;
	int    idToolTip;
} TOOLTIP;

LONG lLastToolTip = -1;			// region id of last region cursor was over
LONG lLastTime = 0;				// timer tick for delay prior to showing tip
LONG lTipDisplayedTime = 0;	// timer tick tip was displayed, for auto erase

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
#define MAKE_RECT(a,x,y,r,b) a.x=(x);a.y=(y);a.r=(r);a.b=(b)
#define CURSOR_IN_BOX(x,y,w,h) \
	(cursor_x>(x)&&cursor_y>(y)&&cursor_x<(x)+(w)&&cursor_y<(y)+(h))
	
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
void SetRedrawMainMapLevel (void);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
static LONG CurStackIndex = 0;

static REGION *pThisRegion;
static LONG   gLastRegionIndex = 0;

static REGION regions[MAX_REGIONS];
static TOOLTIP tooltip[MAX_REGIONS];
static LONG region_stack[MAX_STACK_DEPTH] = {0};

static REGION *gpLastRegion = &regions[0];
/* =======================================================================
   Function    - add_key
   Description - adds a key (zero-size region) to the command list
   Returns     - void
   ======================================================================== */
void add_key(LONG key,PFVLL func, LONG val, LONG val2)
{
	LONG i;
	
	i = add_region(0,0,0,0,key,func,val,val2,0, -1);
	
	if(i != fERROR)
		regions[i].flags=REGION_ACTIVE;
}

/* =======================================================================
   Function    - replace_key_vals
   Description - switch a key (zero-size region) to the command list
		 if it has never been added, then just add it.
   Returns     - void
   ======================================================================== */
void replace_key_vals(LONG key, 
		 PFVLL pNewFunc,
		 LONG NewVal,
		 LONG NewVal2,
		 PTR_KEYSTRUCT pOldKeyStruct)
{
	
// Check in case we are attempting to restore a null function.
	if (pNewFunc != 0)
	{
		LONG found = FALSE;
		REGION *pThisRegion;
		
		for(pThisRegion=&regions[region_stack[CurStackIndex]];
		    pThisRegion <= gpLastRegion;
		    ++pThisRegion)
		{
			if(pThisRegion->flags != REGION_INVALID && pThisRegion->key==key)
			{
				found = TRUE;
				pOldKeyStruct->func = pThisRegion->function;
				pOldKeyStruct->val  = pThisRegion->val;
				pOldKeyStruct->val2 = pThisRegion->val2;
				
				pThisRegion->function=pNewFunc;
				pThisRegion->val=NewVal;
				pThisRegion->val2=NewVal2;
				break;
			}
		}
		
		if (found == FALSE)
		{
			add_key(key,
				pNewFunc,
				NewVal,
				NewVal2);
				
			pOldKeyStruct->func = 0;
			pOldKeyStruct->val  = 0;
			pOldKeyStruct->val2  = 0;
		}
	}
	else
	{
		del_key(key, pOldKeyStruct);
	}
	
}
/* =======================================================================
   Function    - del_key
   Description - removes a key (zero-size region) to the command list
   Returns     - void
   ======================================================================== */
void del_key(LONG key, PTR_KEYSTRUCT pOldKeyStruct)
{
	LONG found = FALSE;
	REGION *pThisRegion;

	for(pThisRegion=&regions[region_stack[CurStackIndex]];
		pThisRegion<=gpLastRegion;
		++pThisRegion)
	{
		if(pThisRegion->key==key)
		{
			found = TRUE;
			pOldKeyStruct->func = pThisRegion->function;
			pOldKeyStruct->val = pThisRegion->val;
			pOldKeyStruct->val2 = pThisRegion->val2;
			
			pThisRegion->flags = REGION_INVALID;
			break;
		}
	}
	
	if (found == FALSE)
	{
		pOldKeyStruct->func = 0;
		pOldKeyStruct->val  = 0;
		pOldKeyStruct->val2  = 0;
	}
}
/* ========================================================================
   Function    - add_tooltip
   Description - Find a vacant spot in the tool tip array and add one.
   Returns     - 
   ======================================================================== */

static void add_tooltip(LONG i, LONG idTip)
{
	if(idTip != -1)
	{
		LONG j;
		
		// find a tooltip position in the tooltip array
		for(j=0; j<MAX_REGIONS; ++j)
		{
			if(tooltip[j].lId == -1)
				break;
		}

		if(j>=MAX_REGIONS)
		{
			fatal_error("Exceded max regions adding tooltip");
		}

		// fill in the links to the tip
		tooltip[j].idToolTip = idTip;
		tooltip[j].lId = i;
		regions[i].lTipIndex = j;
	}
}
/* =======================================================================
   Function    - add_region
   Description - adds an area to the screen that activates when clicked on
   Returns     - void
   ======================================================================== */
LONG add_region(LONG x, LONG y, LONG w, LONG h, LONG key, PFVLL func, LONG val, LONG val2, LONG id, int idTip)
{
	LONG i = fERROR;

	if(func==NULL)
		fatal_error("Bad function in add_region");

	for(i=region_stack[CurStackIndex];i<MAX_REGIONS;++i)    // Find a slot in the regions array to put it.
	{
		if(regions[i].flags==REGION_INVALID)
		{
			break;
		}
	}
	if(i>=MAX_REGIONS)
	{
		fatal_error("Exceded max regions creating region");
	}

	if (i > gLastRegionIndex)
	{
		gpLastRegion = &regions[i];
		gLastRegionIndex = i;
	}
	
	regions[i].x=x;
	regions[i].y=y;
	regions[i].w=w;
	regions[i].h=h;
	regions[i].function=func;
	regions[i].key=key;
	regions[i].val=val;
	regions[i].val2=val2;
	regions[i].flags=REGION_ACTIVE;
	regions[i].id=id;
	regions[i].lTipIndex = -1;

	add_tooltip(i, idTip);

	return i;
}

/* =======================================================================
   Function    - del_region
   Description - deletes a region from the screen
   Returns     - void
   ======================================================================== */
void del_region(PFVLL func,LONG key)
{
	REGION *pThisRegion;


	if(func!=NULL)
	{
		for(pThisRegion=&regions[region_stack[CurStackIndex]];
		    pThisRegion<=gpLastRegion;
		    ++pThisRegion)
		{
			if(pThisRegion->function==func)
			{
				pThisRegion->flags=REGION_INVALID;
				if(pThisRegion->lTipIndex != -1)
				{
					tooltip[pThisRegion->lTipIndex].lId = -1;
					pThisRegion->lTipIndex = -1;
				}
				//return;
			}

		}
	}
	if(key!=0L)
	{
		for(pThisRegion=&regions[region_stack[CurStackIndex]];
		    pThisRegion<= gpLastRegion;
		    ++pThisRegion)
		{
			if(pThisRegion->key==key)
			{
				pThisRegion->flags=REGION_INVALID;
				if(pThisRegion->lTipIndex != -1)
				{
					tooltip[pThisRegion->lTipIndex].lId = -1;
					pThisRegion->lTipIndex = -1;
				}
				//return;
			}
		}
	}
}

/* =======================================================================
   Function    - del_all_regions
   Description - wipes the screen clear of regions
   Returns     - void
   ======================================================================== */
void del_all_regions(void)
{
	LONG i;

printf("del_all_regions\n");

	// clear stack 
	memset(&region_stack[0], 0, sizeof(LONG)*MAX_STACK_DEPTH);
	// clear stack pointer
	CurStackIndex = 0;
	
	// render all regions INVALID
	for(i=0;i<MAX_REGIONS;++i)
		regions[i].flags=REGION_INVALID;

	// clear all tool tips
	for(i=0;i<MAX_REGIONS;++i)
		tooltip[i].lId = -1;
		
	gpLastRegion = &regions[0];
	gLastRegionIndex = 0;
}

#if OLD_CHECK_REGIONS
/* =======================================================================
   Function    - check_regions
   Description - checks all valid regions for activiation. returns whether
		 routine has eaten the event.
   Returns     - REGION_EVENT_TYPE  
   ======================================================================== */
REGION_EVENT_TYPE check_regions(void)
{
	REGION_EVENT_TYPE EventHandled = REGION_EVENT_NOT_HANDLED;
	int i, iCursorOnTip = 0;
	int iHaveTip = 0;
	//SHORT         iBkgnd;
	SHORT w, h, x, y;
	//REGION * const pLastRegion = &regions[MAX_REGIONS - 1];
	REGION const * const pFirstRegion = &regions[region_stack[CurStackIndex]];
	char szText[50];

	// do any regions have tips
	for(i=region_stack[CurStackIndex]; i<gLastRegionIndex; ++i)
	{
		if (regions[i].lTipIndex != -1)
		{
			iHaveTip = 1;
			break;
		}
	}

	for (pThisRegion=gpLastRegion;
	     pThisRegion>=pFirstRegion;
	     pThisRegion--)
	{
		// this region is unused
		if (pThisRegion->flags == REGION_INVALID)
			continue;

		// this region is used, but inactive
		if (pThisRegion->flags == REGION_INACTIVE)
			continue;
		if (key_status(pThisRegion->key))
		{
			AddSndObj( SND_UI_BUTTON_CLICK, NULL, VOLUME_NINETY);
			(*pThisRegion->function)(pThisRegion->val, pThisRegion->val2);
			EventHandled = REGION_HANDLED_EVENT;
			break;
		}

		// if a tip is already displayed
		if(lTipDisplayedTime && get_time() > lTipDisplayedTime + ticsDISPLAY_TIP_TIME)
		{
			// erase the tip
			SetRedrawMainMapLevel();
			fUpdatePanels = TRUE;		// GWP mark the panels needing an update.

			// clear vars
			iHaveTip = 0;
			lTipDisplayedTime = 0;
			iCursorOnTip = 1;
		}

		if(iHaveTip)			// if tips exist..
		{
			if (pThisRegion->lTipIndex != -1 && 	//..and cursor is in a region..
					CURSOR_IN_BOX (
						X_RES_ADJ(pThisRegion->x),
						Y_RES_ADJ(pThisRegion->y),
						X_RES_ADJ(pThisRegion->w),
						Y_RES_ADJ(pThisRegion->h) )
				)
			{
				iCursorOnTip = 1;

				// ..and this tip is NOT already displayed
				if(lLastToolTip != pThisRegion->lTipIndex)
				{
					if (lTipDisplayedTime)			// if a tip is up right now..
					{
						SetRedrawMainMapLevel();	// ..erase that tip first..
						fUpdatePanels = TRUE;		// GWP mark the panels needing an update.
						iHaveTip = 0;					// ..and start over
						lTipDisplayedTime = 0;
						lLastToolTip = -1;
						lLastTime = 0;
						goto SKIP_TIP;
					}
					
					if(lLastTime == 0)				//..wait until time to display
						lLastTime = get_time();
					else if (get_time() > lLastTime + ticsWAIT_BEFORE_DISPLAY)
					{
						if(lTipDisplayedTime)
						{
							SetRedrawMainMapLevel();
							lTipDisplayedTime = 0;
						}
						else
						{
							// display the tooltip
							if(tooltip[pThisRegion->lTipIndex].idToolTip != -1)
							{
								sprintf(szText, "^F00%s", STRMGR_GetStr(tooltip[pThisRegion->lTipIndex].idToolTip));
								if(strcmp(szText, "^F00") != 0)
								{
									x = X_RES_ADJ(pThisRegion->x)+8;
									y = Y_RES_ADJ(pThisRegion->y)+8;
									w = gtext_width(szText) + 4;
									h = 10;
									if (x+w > 635)                          // handle edge of screen
										x = x - w + X_RES_ADJ(pThisRegion->w) - 16;
									if ((y+h) > 475)
									    y = y - h + Y_RES_ADJ(pThisRegion->h) - 16;
									SysHideCursor();
									color_rect(x, y, w, h, 223);            // lt tan
									color_box(x, y, w, h, 223-8);           // md tan
									print_textf(x + 2, y + 2, 128, szText); // dk brown
									SysShowCursor();
							
									// set vars
									lLastToolTip = pThisRegion->lTipIndex;
									lTipDisplayedTime = get_time();
								}
							}
						}
					}
				}
			}
		}

SKIP_TIP:
		if (mouse_click)
		{
			if (CURSOR_IN_BOX (
					X_RES_ADJ(pThisRegion->x),
					Y_RES_ADJ(pThisRegion->y),
					X_RES_ADJ(pThisRegion->w),
					Y_RES_ADJ(pThisRegion->h) )
				)
			{
				AddSndObj( SND_UI_BUTTON_CLICK, NULL, VOLUME_NINETY);
				(*pThisRegion->function)(pThisRegion->val, pThisRegion->val2);
				EventHandled = REGION_HANDLED_EVENT;
				break;
			}
		}
	}

	// case where cursor is not on a region
	if (!iCursorOnTip)
	{
		if (lTipDisplayedTime)			// if a tip is up right now..
		{
			SetRedrawMainMapLevel();	// ..erase that tip
			lTipDisplayedTime = 0;
		}
		lLastTime = 1;
		lLastToolTip = -1;
	}

	return EventHandled;
}
#endif // OLD_CHECK_REGIONS
/* =======================================================================
   Function    - check_regions
   Description - checks all valid regions for activiation. returns whether
		 routine has eaten the event.
   Returns     - REGION_EVENT_TYPE  
   ======================================================================== */
REGION_EVENT_TYPE check_regions(void)
{
	REGION_EVENT_TYPE EventHandled = REGION_EVENT_NOT_HANDLED;
	//REGION * const pLastRegion = &regions[MAX_REGIONS - 1];
	REGION const * const pFirstRegion = &regions[region_stack[CurStackIndex]];


	for (pThisRegion=gpLastRegion;
	     pThisRegion>=pFirstRegion;
	     pThisRegion--)
	{
		// this region is unused
		if (pThisRegion->flags == REGION_INVALID)
			continue;

		// this region is used, but inactive
		if (pThisRegion->flags == REGION_INACTIVE)
			continue;
		if (key_status(pThisRegion->key))
		{
			AddSndObj( SND_UI_BUTTON_CLICK, NULL, VOLUME_NINETY);
			(*pThisRegion->function)(pThisRegion->val, pThisRegion->val2);
			EventHandled = REGION_HANDLED_EVENT;
			break;
		}

		if (mouse_click)
		{
			if (CURSOR_IN_BOX (
					X_RES_ADJ(pThisRegion->x),
					Y_RES_ADJ(pThisRegion->y),
					X_RES_ADJ(pThisRegion->w),
					Y_RES_ADJ(pThisRegion->h) )
				)
			{
				AddSndObj( SND_UI_BUTTON_CLICK, NULL, VOLUME_NINETY);
				(*pThisRegion->function)(pThisRegion->val, pThisRegion->val2);
				EventHandled = REGION_HANDLED_EVENT;
				break;
			}
		}
	}

	return EventHandled;
}
/* =======================================================================
   Function    - paint_tooltips
   Description - checks all valid regions for tooltips.
   Returns     - 
   ======================================================================== */
void paint_tooltips(void)
{
	int i, iCursorOnTip = 0;
	int iHaveTip = 0;
	//SHORT         iBkgnd;
	SHORT w, h, x, y;
	//REGION * const pLastRegion = &regions[MAX_REGIONS - 1];
	REGION const * const pFirstRegion = &regions[region_stack[CurStackIndex]];
	char szText[50];

	// do any regions have tips
	for(i=region_stack[CurStackIndex]; i<gLastRegionIndex; ++i)
	{
		if (regions[i].lTipIndex != -1)
		{
			iHaveTip = 1;
			break;
		}
	}

	for (pThisRegion=gpLastRegion;
	     pThisRegion>=pFirstRegion;
	     pThisRegion--)
	{
		// this region is unused
		if (pThisRegion->flags == REGION_INVALID)
			continue;

		// this region is used, but inactive
		if (pThisRegion->flags == REGION_INACTIVE)
			continue;

		// if a tip is already displayed
		if(lTipDisplayedTime && get_time() > lTipDisplayedTime + ticsDISPLAY_TIP_TIME)
		{
			// erase the tip
			SetRedrawMainMapLevel();
			fUpdatePanels = TRUE;		// GWP mark the panels needing an update.

			// clear vars
			iHaveTip = 0;
			lTipDisplayedTime = 0;
			iCursorOnTip = 1;
		}

		if(iHaveTip)			// if tips exist..
		{
			if (pThisRegion->lTipIndex != -1 && 	//..and cursor is in a region..
					CURSOR_IN_BOX (
						X_RES_ADJ(pThisRegion->x),
						Y_RES_ADJ(pThisRegion->y),
						X_RES_ADJ(pThisRegion->w),
						Y_RES_ADJ(pThisRegion->h) )
				)
			{
				iCursorOnTip = 1;

				// ..and this tip is NOT already displayed
				if(lLastToolTip != pThisRegion->lTipIndex)
				{
					if (lTipDisplayedTime)			// if a tip is up right now..
					{
						SetRedrawMainMapLevel();	// ..erase that tip first..
						fUpdatePanels = TRUE;		// GWP mark the panels needing an update.
						iHaveTip = 0;					// ..and start over
						lTipDisplayedTime = 0;
						lLastToolTip = -1;
						lLastTime = 0;
						goto SKIP_TIP;
					}
					
					if(lLastTime == 0)				//..wait until time to display
						lLastTime = get_time();
					else if (get_time() > lLastTime + ticsWAIT_BEFORE_DISPLAY)
					{
						if(lTipDisplayedTime)
						{
							SetRedrawMainMapLevel();
							lTipDisplayedTime = 0;
						}
						else
						{
							// display the tooltip
							if(tooltip[pThisRegion->lTipIndex].idToolTip != -1)
							{
								sprintf(szText, "^F00%s", STRMGR_GetStr(tooltip[pThisRegion->lTipIndex].idToolTip));
								if(strcmp(szText, "^F00") != 0)
								{
									x = X_RES_ADJ(pThisRegion->x)+8;
									y = Y_RES_ADJ(pThisRegion->y)+8;
									w = gtext_width(szText) + 4;
									h = 10;
									if (x+w > 635)                          // handle edge of screen
										x = x - w + X_RES_ADJ(pThisRegion->w) - 16;
									if ((y+h) > 475)
									    y = y - h + Y_RES_ADJ(pThisRegion->h) - 16;
									SysHideCursor();
									color_rect(x, y, w, h, 223);            // lt tan
									color_box(x, y, w, h, 223-8);           // md tan
									print_textf(x + 2, y + 2, 128, szText); // dk brown
									
									// GWP We need to paint ourselves.
									if (fRender)
									{
										ScreenCopy(0, x, y, w, h, SC_HIGH_RES);
									}
									SysShowCursor();
							
									// set vars
									lLastToolTip = pThisRegion->lTipIndex;
									lTipDisplayedTime = get_time();
								}
							}
						}
					}
				}
			}
		}
	}

SKIP_TIP:
	// case where cursor is not on a region
	if (!iCursorOnTip)
	{
		if (lTipDisplayedTime)			// if a tip is up right now..
		{
			SetRedrawMainMapLevel();	// ..erase that tip
			fUpdatePanels = TRUE;
			lTipDisplayedTime = 0;
		}
		lLastTime = 1;
		lLastToolTip = -1;
	}
}
/* =======================================================================
   Function    - LastRegionX
   Description - 
   Returns     - 
   ======================================================================== */
SHORT   LastRegionX (void)
{
	return X_RES_ADJ(pThisRegion->x);
}

SHORT   LastRegionY (void)
{
	return Y_RES_ADJ(pThisRegion->y);
}
/* =======================================================================
   Function    - init_regions
   Description - wrapper for del_all_regions, with a friendlier name
   Returns     - void
   ======================================================================== */
void init_regions(void)
{
	del_all_regions();
}

/* =======================================================================
   Function    - push_regions
   Description - pushes the current region state onto the stack
   Returns     - void
   ======================================================================== */
void push_regions(void)
{
	LONG i;
	
	//GEH what about errors?
	if (CurStackIndex >= (MAX_STACK_DEPTH - 1))
	{
#if defined(_DEBUG)
		fatal_error("MAX_STACK_DEPTH reached in push_regions()");
#endif
		return;
	}
		
	for (i=gLastRegionIndex;i>=0;i--)
	{
		if (regions[i].flags != REGION_INVALID)
		{
			++CurStackIndex;
			region_stack[CurStackIndex] = i + 1;
			
			gLastRegionIndex = region_stack[CurStackIndex];
			gpLastRegion = &regions[gLastRegionIndex];
			
			break;
		}
	}
}

/* =======================================================================
   Function    - pop_regions
   Description - pop back to previous regions stack pointer
   Returns     - void
   ======================================================================== */
void pop_regions(void)
{
	REGION *pThisRegion;
	
	if (CurStackIndex <= 0)
	{
#if defined (_DEBUG)
		fatal_error("ERROR! pop_regions. Popping too many times.\n");
#endif
		return;
	}

	// inactivate all regions from current stack up
	for (pThisRegion=&regions[region_stack[CurStackIndex]];
		 pThisRegion<=gpLastRegion;
		 ++pThisRegion)
	{
		// this region is unused
		if (pThisRegion->flags == REGION_INVALID) /*not a valid region so go on*/
			continue;
		
		pThisRegion->flags=REGION_INVALID;
		if (pThisRegion->lTipIndex != -1)
		{
			tooltip[pThisRegion->lTipIndex].lId = -1;
			pThisRegion->lTipIndex = -1;
		}
	}
	
	// Reset these to the old beginings.
	gLastRegionIndex = region_stack[CurStackIndex] - 1;
	gpLastRegion = &regions[gLastRegionIndex];
	
	// move stack index back on set (until at first slot)
	if(CurStackIndex)
	{
		region_stack[CurStackIndex] = 0;
		CurStackIndex--;
	}
}

/* ========================================================================
   Function    - activate_region
   Description - turn a region on or off by id
   Returns     - 
   ======================================================================== */
void activate_region(LONG id, BOOL state_on)
{
	REGION *pThisRegion;
	REGION * const pFirstRegion = &regions[region_stack[CurStackIndex]];

	for (pThisRegion=gpLastRegion;
	     pThisRegion>=pFirstRegion;
	     pThisRegion--)
	{
		// this region is unused
		if (pThisRegion->flags == REGION_INVALID)
			continue;

		// find match
		if (pThisRegion->id == id)
		{
			if(state_on)
				pThisRegion->flags = REGION_ACTIVE;
			else
				pThisRegion->flags = REGION_INACTIVE;
				
//                      break;          //[ABC] let's do all the regions with the id
		}
	}
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     - 
   ======================================================================== */
LONG CountRegions (void)
{
	LONG	i;
	LONG	cnt = 0;

	for (i=0; i<MAX_REGIONS; ++i)
		if (regions[i].flags != REGION_INVALID)
			++cnt;

	return cnt;
}

/* ========================================================================
   Function    - OutlineAllRegions
   Description - 
   Returns     - 
   ======================================================================== */
void OutlineAllRegions(void)
{
/*      REGION *pThisRegion;
	// REGION * const pLastRegion = &regions[MAX_REGIONS - 1];
	REGION * const pFirstRegion = &regions[region_stack[CurStackIndex]];
	

	for (pThisRegion=gpLastRegion;
	     pThisRegion>=pFirstRegion;
	     pThisRegion--)
	{
		// this region is unused
		if (pThisRegion->flags == REGION_INVALID)
			continue;

		// this region is used, but inactive
		if (pThisRegion->flags == REGION_INACTIVE)
			continue;
		color_box(pThisRegion->x,pThisRegion->y,
				pThisRegion->w,pThisRegion->h,79);
		

	}
*/

	REGION *pThisRegion;

	for (pThisRegion=&regions[region_stack[CurStackIndex]];
	     pThisRegion<=gpLastRegion;
	     ++pThisRegion)
	{
		if (pThisRegion->flags==REGION_ACTIVE)
		{
			color_box(pThisRegion->x,
					  pThisRegion->y,
					  pThisRegion->w,
					  pThisRegion->h,127);
		}
		else if (pThisRegion->flags==REGION_INACTIVE)
		{
			color_box(pThisRegion->x,
					  pThisRegion->y,
					  pThisRegion->w,
					  pThisRegion->h,79);
		}
	}

}

/* ========================================================================
   Function    - change_tooltip
				return;
   Description - given a func and or key and an x,y , swap out the old tool 
   				 tip for the new one.
   				 Change them all throughout the stack because we could have
   				 pushed the same data down several times.
   Returns     - TRUE | FALSE depending on success.
   ======================================================================== */

BOOL change_tooltip(PFVLL func, LONG key, LONG x, LONG y, 
					LONG newToolTip,
					CHANGE_REGION_MODE crMode)
{
	BOOL Result = FALSE;
	LONG BeginingRegion = (crMode == CHANGE_ALL_REGIONS) ? 0 : region_stack[CurStackIndex];
	
	if(func!=NULL)
	{
		REGION *pThisRegion;
		LONG i;
		
		for(pThisRegion=&regions[BeginingRegion], i = BeginingRegion;
		    pThisRegion<=gpLastRegion;
		    ++pThisRegion, ++i)
		{
			if(pThisRegion->function==func &&
			   pThisRegion->x == x &&
			   pThisRegion->y == y)
			{
				// Remove the old tool tip.
				if(pThisRegion->lTipIndex != -1)
				{
					tooltip[pThisRegion->lTipIndex].lId = -1;
					pThisRegion->lTipIndex = -1;
				}
				
				// Add the new tip.
				add_tooltip(i, newToolTip);
				Result = TRUE;
			}

		}
	}
	if(key!=0L)
	{
		REGION *pThisRegion;
		LONG i;
		
		for(pThisRegion=&regions[BeginingRegion], i = BeginingRegion;
		    pThisRegion<= gpLastRegion;
		    ++pThisRegion, ++i)
		{
			if(pThisRegion->key==key &&
			   pThisRegion->x == x &&
			   pThisRegion->y == y)
			{
				// Remove the old tool tip.
				if(pThisRegion->lTipIndex != -1)
				{
					tooltip[pThisRegion->lTipIndex].lId = -1;
					pThisRegion->lTipIndex = -1;
				}
				
				// Add the new tip.
				add_tooltip(i, newToolTip);
				Result = TRUE;
			}
		}
	}
	return Result;
}

/* ======================================================================== */

/* ========================================================================
   Function    - change_function
				return;
   Description - given a func and or key and an x,y , swap out the old func 
   				 key and vals
   				 Change them all throughout the stack because we could have
   				 pushed the same data down several times.
   Returns     - TRUE | FALSE depending on success.
   ======================================================================== */

BOOL change_function(PFVLL func, LONG key, LONG x, LONG y, 
					PFVLL NewFunc, LONG NewKey, LONG NewVal, LONG NewVal2,
					CHANGE_REGION_MODE crMode)
{
	BOOL Result = FALSE;
	LONG BeginingRegion = (crMode == CHANGE_ALL_REGIONS) ? 0 : region_stack[CurStackIndex];
	REGION *pThisRegion;
	
	if(func!=NULL)
	{
		for(pThisRegion=&regions[BeginingRegion];
		    pThisRegion<=gpLastRegion;
		    ++pThisRegion)
		{
			if(pThisRegion->function==func &&
			   pThisRegion->x == x &&
			   pThisRegion->y == y)
			{
				pThisRegion->key = NewKey;
				pThisRegion->val = NewVal;
				pThisRegion->val2 = NewVal2;
				pThisRegion->function = NewFunc;
				Result = TRUE;
			}

		}
	}
	if(key!=0L)
	{
		for(pThisRegion=&regions[BeginingRegion];
		    pThisRegion<= gpLastRegion;
		    ++pThisRegion)
		{
			if(pThisRegion->key==key &&
			   pThisRegion->x == x &&
			   pThisRegion->y == y)
			{
				pThisRegion->key = NewKey;
				pThisRegion->val = NewVal;
				pThisRegion->val2 = NewVal2;
				pThisRegion->function = NewFunc;
				
				Result = TRUE;
			}
		}
	}
	return Result;
}

/* ========================================================================
   Function    - del_region_xy
   Description - 
   Returns     - 
   ======================================================================== */

BOOL del_region_xy(PFVLL oldFunc, LONG x, LONG y, LONG w, LONG h, CHANGE_REGION_MODE crMode)
{
	BOOL Result = FALSE;
	REGION *pThisRegion;
	LONG BeginingRegion = (crMode == CHANGE_ALL_REGIONS) ? 0 : region_stack[CurStackIndex];

	for(pThisRegion=&regions[BeginingRegion];
	    pThisRegion<= gpLastRegion;
	    ++pThisRegion)
	{
		if(pThisRegion->function == oldFunc &&
		   pThisRegion->x == x &&
		   pThisRegion->y == y &&
		   pThisRegion->w == w &&
		   pThisRegion->h == h)
		{
			pThisRegion->flags=REGION_INVALID;
			if(pThisRegion->lTipIndex != -1)
			{
				tooltip[pThisRegion->lTipIndex].lId = -1;
				pThisRegion->lTipIndex = -1;
			}
			Result = TRUE;
		}
	}
	return Result;
}
/* ======================================================================== */

