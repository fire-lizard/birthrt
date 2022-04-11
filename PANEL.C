/* =======================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: Panel.c
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

#include "system.h"
#include "sysint.h"
#include "engine.h"
#include "machine.h"
#include "player.hxx"
#include "sound.hxx"
#include "strmgr.h"
#include "strenum.h"

#include "menu.h"
#include "panel.h"

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
	Macros
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

static LONG AddActivePanel( LONG Index );
static void RemoveActivePanel( LONG Index );
static void PaintPanels(void);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
BOOL	fUpdatePanels = FALSE;

// list of active menus
static LONG ActivePanels[MAX_ACTIVE_MENUS] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

// pointers to the data to use (provided by the user)
extern PMENU Menus;
extern LONG  MaxMenu;
extern SHORT	cntSaveBkgnd;

/* ========================================================================
   Function    - ResetPanels
	Description - clear old panel data
	Returns     - 
	======================================================================== */
void ResetPanels(void)
{
	SHORT i;
	for(i=0;i<MAX_ACTIVE_MENUS;i++)
	{
		ActivePanels[i] = fERROR;
	}
}

/* ========================================================================
   Function    - ShowPanel
   Description - turn a panel on
   Returns     -
   ======================================================================== */
void ShowPanel(LONG Index)
{
	LONG i;
	LONG xOff = 0;
	LONG yOff = 0;
	
	
	// simple error test
	if (!MaxMenu)
		return;
	
	// add this guys index to the list of active menu
	// NOTE: remember you can only have MAX_ACTIVE_MENUS of these
	if (fERROR == AddActivePanel(Index))
	{
		return;
	}
	
	// add regions for all the sub-buttons
	// the first button of a menus describes the background
	// and we use it's X and Y as the offsets for all following
	// buttons.
	xOff = Menus[Index].Buttons[0].X;
	yOff = Menus[Index].Buttons[0].Y;
	
	for (i = 1; i < Menus[Index].MenuButtonCount; i++ )
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
	fUpdatePanels = TRUE;
}

/* ========================================================================
   Function    -  HidePanel
   Description -  Removes the active menu and resets the hot keys.
   Returns     -  void
   ======================================================================== */

void HidePanel(LONG Index)
{
	
	// simple error test
	if (!MaxMenu)
		return;
	
	RemoveActivePanel(Index);
	
	//clear any keys pending that we didn't handle
	clear_key_status(0);
}


/* ========================================================================
   Function    - IsPanelUp
   Description -
   Returns     - 0 panel not up 1 panel up 
   ======================================================================== */
int IsPanelUp ( LONG Index )
{
   int i;


   // search the panels 

	for (i=0;i<MAX_ACTIVE_MENUS;i++)
	{
		if (Index == ActivePanels[i])
		{
			return 1;
		}
	}

   return 0;

}


/* ========================================================================
   Function    - AddActivePanel
   Description -
   Returns     - The index of this menu (In the active list)
   ======================================================================== */

static LONG AddActivePanel( LONG Index )
{
	LONG i;

	// only one instance of each panel allowed
	for (i=0;i<MAX_ACTIVE_MENUS;i++)
	{
		if (Index == ActivePanels[i])
		{
			return fERROR;
		}
	}
	
	// try to add the new panel
	for (i=0;i<MAX_ACTIVE_MENUS;i++)
	{
		if (fERROR == ActivePanels[i])
		{
			ActivePanels[i] = Index;
			Menus[Index].Buttons[0].Flags |= D_UPDATE;
			break;
		}
	}
	
	if (i == MAX_ACTIVE_MENUS)
		return fERROR;
		
	return i;
}

/* ========================================================================
   Function    -  RemoveActivePanel
   Description -  The "this" Panel and remove it from the active menu array
   Returns     -  void
   ======================================================================== */

static void RemoveActivePanel( LONG Index )
{
	LONG i;
	
	for (i=0;i<MAX_ACTIVE_MENUS;i++)
	{
		if (Index == ActivePanels[i])
		{
			LONG j;
			LONG xOff;
			LONG yOff;
			
		 	xOff = Menus[Index].Buttons[0].X;
			yOff = Menus[Index].Buttons[0].Y;
			
			for (j = 1; j < Menus[Index].MenuButtonCount; j++ )
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
			ActivePanels[i] = -1;
			break;
		}
	}
}

/* ========================================================================
   Function    - main loop call routine
   Description -
   Returns     -
   ======================================================================== */
void RunPanels(void)
{
//	if(fUpdatePanels)
	if(fUpdatePanels && cntSaveBkgnd < 1)
	{
//		printf("Running Panels...\n");
		fUpdatePanels = FALSE;
		PaintPanels();
	}
}

/* ========================================================================
   Function    - PaintPanels
   Description -
   Returns     - void
   ======================================================================== */
static void PaintPanels(void)
{
	LONG	APanel;
	LONG	CurPanel;
	
	SysHideCursor();
	
	for (APanel=0;APanel < MAX_ACTIVE_MENUS;APanel++)
	{
		SHORT	hBitm;
		LONG	i;
		LONG	xOff;
		LONG	yOff;
		
		// if this slot not used, skip
		if( ActivePanels[APanel] == -1)
			continue;

		CurPanel = ActivePanels[APanel];
		xOff = 0;
		yOff = 0;

		if( Menus[CurPanel].Buttons[0].Flags & D_INVISIBLE)
			continue;
			
		for (i = 0; i < Menus[CurPanel].MenuButtonCount; i++ )
		{
			const LONG X = X_RES_ADJ(Menus[CurPanel].Buttons[i].X);
			const LONG Y = Y_RES_ADJ(Menus[CurPanel].Buttons[i].Y);
			const LONG W = X_RES_ADJ(Menus[CurPanel].Buttons[i].W);
			const LONG H = Y_RES_ADJ(Menus[CurPanel].Buttons[i].H);
				
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
				xOff = X_RES_ADJ(Menus[CurPanel].Buttons[0].X);
				yOff = Y_RES_ADJ(Menus[CurPanel].Buttons[0].Y);
				
			}

			if(	Menus[CurPanel].Buttons[i].Flags & D_HILIGHTED )
			{
				if(Menus[CurPanel].Buttons[i].pHilight != 0)
				{
					hBitm = GetResourceStd (Menus[CurPanel].Buttons[i].pHilight,TRUE);
					if (hBitm != fERROR)
					{
						DrawBitm (X + xOff, Y + yOff, hBitm, 0, 0, W, H);
						SetPurge(hBitm);
					}
				}
			}
			else
			{
				switch ( Menus[CurPanel].Buttons[i].btType )
				{
				case BUTTON_SHADEBEVEL:
					shade_edged_rect( X + xOff, Y + yOff, W, H, Menus[CurPanel].Buttons[i].Color );
					break;

				case BUTTON_ENTRY:
				case BUTTON_LISTBOX:
				case BUTTON_COLORBEVEL:
					color_edged_rect( X + xOff, Y + yOff, W, H, Menus[CurPanel].Buttons[i].Color );
					break;

				case BUTTON_BITMAP:
					hBitm = GetResourceStd (Menus[CurPanel].Buttons[i].pArt,TRUE);
					if (hBitm != fERROR)
					{
						if (i == 0) // a panel background
							BltBitmap (X + xOff, Y + yOff, hBitm, 0, 0, W, H);
						else
							DrawBitm (X + xOff, Y + yOff, hBitm, 0, 0, W, H);
						SetPurge(hBitm);
					}
	
					break;
				}
		
			}

			

			// deal with the labels
			if (Menus[CurPanel].Buttons[i].iLabel != -1 &&
				!(Menus[CurPanel].Buttons[i].Flags & D_LABEL_OFF))
			{
				if (Menus[CurPanel].Buttons[i].Flags & D_SANS_5)
					init_gfont(FONT_SANS_5PT);
				else
				if (Menus[CurPanel].Buttons[i].Flags & D_SANS_6)
					init_gfont(FONT_SANS_6PT);
				else
				if (Menus[CurPanel].Buttons[i].Flags & D_SANS_8)
					init_gfont(FONT_SANS_8PT);
				else
				if (Menus[CurPanel].Buttons[i].Flags & D_SANS_16)
					init_gfont(FONT_SANS_16PT);
				else
				if (Menus[CurPanel].Buttons[i].Flags & D_TITL_8)
					init_gfont(FONT_TITL_8PT);
				else
				if (Menus[CurPanel].Buttons[i].Flags & D_TITL_10)
					init_gfont(FONT_TITL_10PT);
				else
				if (Menus[CurPanel].Buttons[i].Flags & D_TITL_16)
					init_gfont(FONT_TITL_16PT);
				else
					init_gfont(FONT_SANS_12PT);

				if (Menus[CurPanel].Buttons[i].Flags & D_TEXT_LEFT)
				{
					LONG labelColor;
					const LONG tOff = Y_RES_ADJ(4);
					
					if(	Menus[CurPanel].Buttons[i].Flags & D_HILIGHTED )
					{
						labelColor = Menus[CurPanel].Buttons[i].HilightLabelColor;
					}
					else
					{
						labelColor = Menus[CurPanel].Buttons[i].LabelColor;
					}
					gprint_text(
						X + xOff + 4,
						Y + yOff + tOff,
						STRMGR_GetStr(Menus[CurPanel].Buttons[i].iLabel),
						labelColor
						);
				}
				else
				{
					LONG labelColor;
					
					if(	Menus[CurPanel].Buttons[i].Flags & D_HILIGHTED )
					{
						labelColor = Menus[CurPanel].Buttons[i].HilightLabelColor;
					}
					else
					{
						labelColor = Menus[CurPanel].Buttons[i].LabelColor;
					}
					print_text_centered(
						X + xOff + (W/2),
						Y + yOff + (H/2) + 1,
						STRMGR_GetStr(Menus[CurPanel].Buttons[i].iLabel),
						labelColor
						);
				}
			}

		}
		
		if( Menus[CurPanel].Buttons[0].pfFunction != NULL )
		{
			(*Menus[CurPanel].Buttons[0].pfFunction)(
				BUILD_LONG(CurPanel, Menus[CurPanel].Buttons[0].Id),
				Menus[CurPanel].Buttons[0].Arg
				);
		}
	}
	
	/* if we are rendering, we need to paint ourselves */
	//GEH if(0)
	if(fRender)
	{
		for (APanel=0;APanel < MAX_ACTIVE_MENUS;APanel++)
		{
			// if this slot not used, skip
			if( ActivePanels[APanel] == -1)
				continue;

			// don't paint if invisible
			if( Menus[ActivePanels[APanel]].Buttons[0].Flags & D_INVISIBLE)
				continue;
				
			CurPanel = ActivePanels[APanel];
			ScreenCopy(0, Menus[CurPanel].Buttons[0].X, Menus[CurPanel].Buttons[0].Y, Menus[CurPanel].Buttons[0].W, Menus[CurPanel].Buttons[0].H,SC_HIGH_RES);
		}
	}
	
	SysShowCursor();
}
