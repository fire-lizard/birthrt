/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: FILENAME.c  -
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#if defined (_WINDOWS)
#include <windows.h>
#endif

#include "TYPEDEFS.H"
#include "ENGINE.H"
#include "SYSTEM.H"

#include "MENU.H"
#include "STRMGR.H"
#include "strenum.h"
#include "VERSION.HXX"

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
static void AboutPaint(LONG menuCombo, LONG);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

//GEH Product Version Numbers:

//                   (ReleaseVersion).(ProductType)(PatchLevel)
//
//                   ReleaseVersion  1.xx = Birthright, TheGorgonsAlliance
//
//                   ProductType     x.1x = Product
//                   ProductType     x.2x = Download Demo
//                   ProductType     x.3x = Multiplayer Demo

#if defined(_MULTIPLAY_ONLY)

// Multiplayer game
float	ProductVersionNumber = 1.33; // updated  geh 9/12/97
// NOTE:multiplay game is same AI as second patch/production master
float	RealmAIVersionNumber = 1.28; // updated  geh 9/12/97

#else

// Shipping version
// float	ProductVersionNumber = 1.10;
// float	RealmAIVersionNumber = 1.25; // updated  geh 7/09/97

// First Patch
//float	ProductVersionNumber = 1.12; // updated  geh 8/27/97
//float	RealmAIVersionNumber = 1.27; // updated  geh 8/27/97

// Second Patch and Second Production Master
//float	ProductVersionNumber = 1.13; // updated  geh 9/05/97
//float	RealmAIVersionNumber = 1.28; // updated  geh 9/05/97

// Third Patch 
//float	ProductVersionNumber = 1.14; // updated  geh 9/05/97
//float	RealmAIVersionNumber = 1.28; // unchanged

// Fourth Patch 
float	ProductVersionNumber = 1.15; // updated  geh 12/09/97
float	RealmAIVersionNumber = 1.29; // updated  geh 12/09/97

#endif // _MULTIPLAY_ONLY


/* ------------------------------------------------------------------------
   Local Variables
   ------------------------------------------------------------------------ */

/* ========================================================================
   Function    - About
   Description - change the camera position mode
   Returns     - 
   ======================================================================== */
void About(LONG /* unused */ , LONG /* unused */ )
{
	SetButtonProc   (D_QUESTION1, 0, AboutPaint, 0, NO_KEY );
	// set the question text
	SetButtonLabel  (D_QUESTION1, QUESTION_TEXT, STR_NULL, BLACK );
	
	SetButtonLabel  (D_QUESTION1, QUESTION_BTN1, STR_CANCEL1, WHITE );
	SetButtonProc   (D_QUESTION1, QUESTION_BTN1, HideSubMenuWithClick, D_QUESTION1, KEY_ESCAPE );
	SetButtonHilight(D_QUESTION1, QUESTION_BTN1, TRUE );
	
	ShowMenu(D_QUESTION1);
}
/* ========================================================================
   Function    - AboutPaint
   Description - Paint the version text.
   Returns     - 
   ======================================================================== */
static void AboutPaint(LONG MenuCombo, LONG /* unused */ )
{
	LONG X;
	LONG Y;
	LONG mx;
	LONG my;
	LONG MenuId;
	LONG ButtonId;
	char ProgramCompileDate[100];
	
	sprintf(ProgramCompileDate, "Verion %3.2f\nRealm AI Ver. %3.2f\nFrom: %s %s",
		ProductVersionNumber,
		RealmAIVersionNumber,
		__TIME__,
		__DATE__ 
		);
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	if( GetButtonPosition( MenuId, QUESTION_TEXT, &mx, &my ) == fERROR)
		return;

	if( GetButtonSize( MenuId, QUESTION_TEXT, &X, &Y ) == fERROR)
		return;

	// get center of text screen
	X = mx + (X/2);
	Y = my + (Y/3);
	
	init_gfont(FONT_SANS_12PT);
	
	print_text_centered(X,Y, ProgramCompileDate, BLACK);
	
	return;
}

