/* =======================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: REQUEST.c
   Author: 	 Greg Hightower
   ========================================================================
   Contains the following general functions:

   Contains the following internal functions:

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <string.h>

#include "SYSTEM.H"
#include "SYSINT.H"
#include "ENGINE.H"
#include "MACHINE.H"
#include "SOUND.HXX"
#include "STRMGR.H"
#include "strenum.h"

#include "MENU.H"
#include "REQUEST.H"

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
	Macros
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

static LONG AddActiveRequest( LONG Index );
static void RemoveActiveRequest( LONG Index );
static void PaintRequests(void);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
// list of active menus
static LONG ActiveRequests[MAX_ACTIVE_MENUS] = {fERROR,fERROR,fERROR,fERROR,fERROR,fERROR,fERROR,fERROR,fERROR,fERROR};
static LONG RequestValue[MAX_ACTIVE_MENUS] = {fERROR,fERROR,fERROR,fERROR,fERROR,fERROR,fERROR,fERROR,fERROR,fERROR};

// pointers to the data to use (provided by the user)
extern PMENU	Menus;
extern LONG		MaxMenu;

extern SHORT	sMenusUp;

/* ========================================================================
   Function    - ResetRequests
	Description - clear old panel data
	Returns     - 
	======================================================================== */
void ResetRequests(void)
{
	LONG i;
	for(i=0;i<MAX_ACTIVE_MENUS;++i)
	{
		ActiveRequests[i] = fERROR;
	}
}

/* ========================================================================
   Function    - ShowRequest
   Description - turn a Request on
   Returns     -
   ======================================================================== */
void ShowRequest (LONG Index, LONG value)
{
	LONG i, rv;
	LONG xOff = 0;
	LONG yOff = 0;
	
	
	// simple error test
	if (!MaxMenu)
		return;
	
	// add this guys index to the list of active menu
	// NOTE: remember you can only have MAX_ACTIVE_MENUS of these
	rv = AddActiveRequest(Index);
	if (fERROR == rv)
	{
		return;
	}
	
	RequestValue[rv] = value;		// set unique value of this request
	
	++sMenusUp;			// non-zero if a menu or request is up
	push_regions();	// model action
	
	// add regions for all the sub-buttons
	// the first button of a menus describes the background
	// and we use it's X and Y as the offsets for all following
	// buttons.
	xOff = Menus[Index].Buttons[0].X;
	yOff = Menus[Index].Buttons[0].Y;
	
	for (i = 1; i < Menus[Index].MenuButtonCount; ++i )
	{
		if (Menus[Index].Buttons[i].pfFunction)
		{
			add_region(
				Menus[Index].Buttons[i].X + xOff,
				Menus[Index].Buttons[i].Y + yOff,
				Menus[Index].Buttons[i].W,
				Menus[Index].Buttons[i].H,
				Menus[Index].Buttons[i].Key,
				Menus[Index].Buttons[i].pfFunction,
				BUILD_LONG(Index, Menus[Index].Buttons[i].Id),
				Menus[Index].Buttons[i].Arg,
				Menus[Index].Buttons[i].Id,
				Menus[Index].Buttons[i].idToolTip
				);
		}
	}
}

/* ========================================================================
   Function    -  HideRequest
   Description -  Removes the active menu and resets the hot keys.
   Returns     -  void
   ======================================================================== */

void HideRequest(LONG Index)
{
	
	// simple error test
	if (!MaxMenu)
		return;
	
	RemoveActiveRequest(Index);
	
	pop_regions();		// clear model action
	--sMenusUp;			// non-zero if a menu or request is up
	if (sMenusUp < 0)
		sMenusUp = 0;
	
	//clear any keys pending that we didn't handle
	clear_key_status(0);
}


/* ========================================================================
   Function    - IsRequestUp
   Description -
   Returns     - 0 request not up 1 request up 
   ======================================================================== */
int IsRequestUp ( LONG Index )
{
   int i;

   // search the Request

	for (i=0;i<MAX_ACTIVE_MENUS;++i)
	{
		if (Index == ActiveRequests[i])
		{
			return 1;
		}
	}

   return 0;

}


/* ========================================================================
   Function    - AddActiveRequest
   Description -
   Returns     - The index of this menu (In the active list)
   ======================================================================== */

static LONG AddActiveRequest( LONG Index )
{
	LONG i;

	// only one instance of each Request allowed
//	for (i=0;i<MAX_ACTIVE_MENUS;++i)
//	{
//		if (Index == ActiveRequests[i])
//		{
//			return fERROR;
//		}
//	}


	// try to add the new Request
	for (i=0;i<MAX_ACTIVE_MENUS;++i)
	{
		if (ActiveRequests[i] == fERROR)
		{
			ActiveRequests[i] = Index;
			Menus[Index].Buttons[0].Flags |= D_UPDATE;
			break;
		}
	}
	
	if (i == MAX_ACTIVE_MENUS)
		return fERROR;
	
	return i;
}

/* ========================================================================
   Function    -  RemoveActiveRequest
   Description -  The "this" Request and remove it from the active menu array
   Returns     -  void
   ======================================================================== */

static void RemoveActiveRequest( LONG Index )
{
	LONG i;
	
	//for (i=0;i<MAX_ACTIVE_MENUS;++i)
	for (i=MAX_ACTIVE_MENUS-1; i>=0; --i)
	{
		if (i<0) break;

		if (Index == ActiveRequests[i])
		{
			LONG j;
			LONG xOff;
			LONG yOff;
			
		 	xOff = Menus[Index].Buttons[0].X;
			yOff = Menus[Index].Buttons[0].Y;
			
			for (j = 1; j < Menus[Index].MenuButtonCount; ++j )
			{
				if (Menus[Index].Buttons[j].pfFunction)
				{
					del_region_xy(
						Menus[Index].Buttons[j].pfFunction,
						Menus[Index].Buttons[j].X + xOff,
						Menus[Index].Buttons[j].Y + yOff,
						Menus[Index].Buttons[j].W,
						Menus[Index].Buttons[j].H,
						CHANGE_TOP_STACK);
				}
			}
			ActiveRequests[i] = fERROR;
			break;
		}
	}
}

/* ========================================================================
   Function    - main loop call routine
   Description -
   Returns     -
   ======================================================================== */
void RunRequests(void)
{
	PaintRequests();
}


/* ========================================================================
   Function    - Return the value of the top designated request type
   Description -
   Returns     -
   ======================================================================== */
LONG MyRequestValue ( LONG Index )
{
	LONG i;

	for (i=MAX_ACTIVE_MENUS-1; i>=0; --i)
	{
		if (i<0) break;

		if (Index == ActiveRequests[i])
		{
			return ( RequestValue[i] );
		}
	}
	// Not found.
	return fERROR;
}

/* ========================================================================
   Function    - PaintRequests
   Description -
   Returns     - void
   ======================================================================== */
static void PaintRequests(void)
{
	LONG	ARequest;
	LONG	CurRequest;
	
	SysHideCursor();
	
	for (ARequest=0;ARequest < MAX_ACTIVE_MENUS;++ARequest)
	{
		SHORT	hBitm;
		LONG	i;
		LONG	xOff;
		LONG	yOff;
		
		// if this slot not used, skip
		if( ActiveRequests[ARequest] == fERROR)
			continue;

		CurRequest = ActiveRequests[ARequest];
		xOff = 0;
		yOff = 0;

		if( Menus[CurRequest].Buttons[0].Flags & D_INVISIBLE)
			continue;
			
		for (i = 0; i < Menus[CurRequest].MenuButtonCount; ++i )
		{
			const LONG X = X_RES_ADJ(Menus[CurRequest].Buttons[i].X);
			const LONG Y = Y_RES_ADJ(Menus[CurRequest].Buttons[i].Y);
			const LONG W = X_RES_ADJ(Menus[CurRequest].Buttons[i].W);
			const LONG H = Y_RES_ADJ(Menus[CurRequest].Buttons[i].H);
				
			if(i == 0)
			{
				xOff = 0;
				yOff = 0;
			}
			else
			{
				// the first button of a Menus describes the background
				// and we use it's X and Y as the offsets for all following
				// buttons.
				xOff = X_RES_ADJ(Menus[CurRequest].Buttons[0].X);
				yOff = Y_RES_ADJ(Menus[CurRequest].Buttons[0].Y);
				
			}

			if(	Menus[CurRequest].Buttons[i].Flags & D_HILIGHTED )
			{
				if(Menus[CurRequest].Buttons[i].pHilight != 0)
				{
					hBitm = GetResourceStd (Menus[CurRequest].Buttons[i].pHilight,TRUE);
					if (hBitm != fERROR)
					{
						DrawBitm (X + xOff, Y + yOff, hBitm, 0, 0, W, H);
						SetPurge(hBitm);
					}
				}
			}
			else
			{
				switch ( Menus[CurRequest].Buttons[i].btType )
				{
				case BUTTON_SHADEBEVEL:
					shade_edged_rect( X + xOff, Y + yOff, W, H, Menus[CurRequest].Buttons[i].Color );
					break;

				case BUTTON_ENTRY:
				case BUTTON_LISTBOX:
				case BUTTON_COLORBEVEL:
					color_edged_rect( X + xOff, Y + yOff, W, H, Menus[CurRequest].Buttons[i].Color );
					break;

				case BUTTON_BITMAP:
					hBitm = GetResourceStd (Menus[CurRequest].Buttons[i].pArt,TRUE);
					if (hBitm != fERROR)
					{
						if (i == 0) // a Request background
							BltBitmap (X + xOff, Y + yOff, hBitm, 0, 0, W, H);
						else
							DrawBitm (X + xOff, Y + yOff, hBitm, 0, 0, W, H);
						SetPurge(hBitm);
					}
	
					break;
				}
		
			}

			

			// deal with the labels
			if (Menus[CurRequest].Buttons[i].iLabel != fERROR &&
				!(Menus[CurRequest].Buttons[i].Flags & D_LABEL_OFF))
			{
				if (Menus[CurRequest].Buttons[i].Flags & D_SANS_5)
					init_gfont(FONT_SANS_5PT);
				else
				if (Menus[CurRequest].Buttons[i].Flags & D_SANS_6)
					init_gfont(FONT_SANS_6PT);
				else
				if (Menus[CurRequest].Buttons[i].Flags & D_SANS_8)
					init_gfont(FONT_SANS_8PT);
				else
				if (Menus[CurRequest].Buttons[i].Flags & D_SANS_16)
					init_gfont(FONT_SANS_16PT);
				else
				if (Menus[CurRequest].Buttons[i].Flags & D_TITL_8)
					init_gfont(FONT_TITL_8PT);
				else
				if (Menus[CurRequest].Buttons[i].Flags & D_TITL_10)
					init_gfont(FONT_TITL_10PT);
				else
				if (Menus[CurRequest].Buttons[i].Flags & D_TITL_16)
					init_gfont(FONT_TITL_16PT);
				else
					init_gfont(FONT_SANS_12PT);

				if (Menus[CurRequest].Buttons[i].Flags & D_TEXT_LEFT)
				{
					LONG labelColor;
					const LONG tOff = Y_RES_ADJ(4);
					
					if(	Menus[CurRequest].Buttons[i].Flags & D_HILIGHTED )
					{
						labelColor = Menus[CurRequest].Buttons[i].HilightLabelColor;
					}
					else
					{
						labelColor = Menus[CurRequest].Buttons[i].LabelColor;
					}
					gprint_text(
						X + xOff + 4,
						Y + yOff + tOff,
						STRMGR_GetStr(Menus[CurRequest].Buttons[i].iLabel),
						labelColor
						);
				}
				else
				{
					LONG labelColor;
					
					if(	Menus[CurRequest].Buttons[i].Flags & D_HILIGHTED )
					{
						labelColor = Menus[CurRequest].Buttons[i].HilightLabelColor;
					}
					else
					{
						labelColor = Menus[CurRequest].Buttons[i].LabelColor;
					}
					print_text_centered(
						X + xOff + (W/2),
						Y + yOff + (H/2) + 1,
						STRMGR_GetStr(Menus[CurRequest].Buttons[i].iLabel),
						labelColor
						);
				}
			}

		}
		
		if( Menus[CurRequest].Buttons[0].pfFunction != NULL )
		{
			(*Menus[CurRequest].Buttons[0].pfFunction)(
				BUILD_LONG(CurRequest, Menus[CurRequest].Buttons[0].Id),
				Menus[CurRequest].Buttons[0].Arg
				);
		}
	}
	
	/* if we are rendering, we need to paint ourselves */
	//GEH if(0)
	if(fRender)
	{
		for (ARequest=0;ARequest < MAX_ACTIVE_MENUS;++ARequest)
		{
			// if this slot not used, skip
			if( ActiveRequests[ARequest] == fERROR)
				continue;

			// don't paint if invisible
			if( Menus[ActiveRequests[ARequest]].Buttons[0].Flags & D_INVISIBLE)
				continue;
				
			CurRequest = ActiveRequests[ARequest];
			ScreenCopy(0, Menus[CurRequest].Buttons[0].X, Menus[CurRequest].Buttons[0].Y, Menus[CurRequest].Buttons[0].W, Menus[CurRequest].Buttons[0].H,SC_HIGH_RES);
		}
	}
	
	SysShowCursor();
}

/* ======================================================================== */

