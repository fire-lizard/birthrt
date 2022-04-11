/* ========================================================================
   Copyright (c) 1990,1997	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: parch.cpp  -
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#include <string.h>

#include "engine.h"
#include "system.h"
#include "machine.h"
#include "menu.h"
#include "panel.h"
#include "gmenuenm.h"
#include "strenum.h"
#include "strmgr.h"

#include "parch.hxx"
/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
   
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
   
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Local Variables
   ------------------------------------------------------------------------ */
/* ========================================================================
   Function    - ParchmentDone
   Description - The close the menu procedure.
   Returns     - 
   ======================================================================== */

void ParchmentDone(LONG MenuCombo, LONG)
{
	LONG	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	SetButtonHilight(MenuId, ButtonId, TRUE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	SetButtonHilight(MenuId, ButtonId, FALSE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	HideMenu(D_PARCHMENT);
	RunMenus();
}

/* ========================================================================
   Function    - PaintParchment
   Description - The menu paint routine.
   Returns     - 
   ======================================================================== */

void PaintParchment(LONG MenuCombo, LONG stringhandle)
{
	char	buffer[712];
	LONG	menu_x, menu_y;
	LONG	X,Y;
	LONG	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	if( GetButtonPosition( MenuId, PARCHMENT_TEXT, &menu_x, &menu_y ) == fERROR)
	    return;
	
	strcpy(buffer, STRMGR_GetStr(stringhandle));
	init_gfont(7);	// Ignious 16
	gprint_text(menu_x, menu_y, buffer, 204);
}

/* ========================================================================
   Function    - InitializeParchmentMenu
   Description - Put up the question dialog with parchment background.
   Returns     - 
   ======================================================================== */

void InitializeParchmentMenu(LONG StringHandle)
{
	
	SetButtonProc  (D_PARCHMENT, 0, PaintParchment, StringHandle, NO_KEY);
	ShowMenu(D_PARCHMENT);
}
				
