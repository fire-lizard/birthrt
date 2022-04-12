/* ========================================================================
   Copyright (c) 1990,1997	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: tutrmenu.cpp  - Tutorial menu code.
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <string.h>

#include "SYSTEM.H"
#include "ENGINE.H"
#include "MACHINE.H"

#include "GMENUENM.H"
#include "MENUPROT.H"
#include "PANEL.H"
#include "STRMGR.H"
#include "TUTRMENU.HXX"

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
   Function    - InitializeTutorialMenu
   Description - 
   Returns     - 
   ======================================================================== */

void InitializeTutorialMenu(
	LONG const StringHandle,
	LONG const MenuFlags
	)
{
	char	cpBuffer[1000];
	LONG	btn_width;
	LONG	btn_height;
	
	set_hires (0, 0);
	if (fRender)
	{
		render_view (FALSE);					// update the background screen
													// in high res (otherwise we get
													// a 1/4 size screen in the upper
													// left of the video screen when
													// the menu comes up
	}
	
	LONG MenuId = D_TUTORIAL_HELP_MED;
	
	GetButtonSize(D_TUTORIAL_HELP_MED,TUTORIAL_MENU_TEXT, &btn_width, &btn_height);
	sprintf(cpBuffer,"^F03^W%0.3d",btn_width);
	strncat(cpBuffer, STRMGR_GetStr(StringHandle), sizeof(cpBuffer));
	
	LONG const TextHeight = gtext_height(cpBuffer);
	
	if (TextHeight < btn_height)
	{
		MenuId = D_TUTORIAL_HELP_MED;
	}
	else
	{
		MenuId = D_TUTORIAL_HELP_WIDE;
	}
	
	SetButtonValue(MenuId, TUTORIAL_MENU_TEXT, StringHandle);
	SetButtonFlag( MenuId, 0, MenuFlags);
	ShowMenu(MenuId);
}

/* ========================================================================
   Function    - TutorialDone
   Description - 
   Returns     - 
   ======================================================================== */

void TutorialDone(LONG MenuCombo, LONG)
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
	HideMenu(MenuId);
	RunMenus();
}

/* ========================================================================
   Function    - PaintTutorial
   Description - 
   Returns     - 
   ======================================================================== */

void PaintTutorial(LONG MenuCombo, LONG )
{
	LONG	text_x, text_y;
	LONG	MenuId;
	LONG	ButtonId;
	LONG	StringHandle;
	char	cpBuffer[1000];
	LONG	btn_width;
	LONG	btn_height;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	if( GetButtonPosition( MenuId, TUTORIAL_MENU_TEXT, &text_x, &text_y ) == fERROR)
	    return;
	
	StringHandle = GetButtonValue(MenuId, TUTORIAL_MENU_TEXT);
	GetButtonSize(MenuId, TUTORIAL_MENU_TEXT, &btn_width, &btn_height);
	
	sprintf(cpBuffer,"^F03^W%0.3d",btn_width);
	strncat(cpBuffer, STRMGR_GetStr(StringHandle), sizeof(cpBuffer));
	
	LONG TextHeight = gtext_height(cpBuffer);
	if (TextHeight > btn_height)
	{
		// Try a smaller font.
		sprintf(cpBuffer,"^F02^W%0.3d",btn_width);
		strncat(cpBuffer, STRMGR_GetStr(StringHandle), sizeof(cpBuffer));
	
		TextHeight = gtext_height(cpBuffer);
		if (TextHeight > btn_height)
		{
			sprintf(cpBuffer,"^F01^W%0.3d",btn_width);
			strncat(cpBuffer, STRMGR_GetStr(StringHandle), sizeof(cpBuffer));
		}
	}
	
	gprint_text(text_x,text_y,cpBuffer, DKBROWN);
}
