/* ========================================================================
   Copyright (c) 1990,1997   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: DESCRIBE.C
   Author: Donald Tsang
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#include "SYSTEM.H"
#include "ENGINE.H"

#include "MENU.H"
#include "PANEL.H"
#include "GMENUENM.H"
#include "GAMEMAP.HXX"
#include "STRMGR.H"
#include "strenum.h"

#include "DESCRIBE.HXX"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define NUM_DESCRIPTION_SCREENS 4
#define NUM_ADVENTURE_SCREENS 2
#define NUM_BATTLE_SCREENS 3
#define NUM_QUICK_INTRO_SCREENS 2
#define NUM_BATTLE_RESERVES	1
#define NUM_BATTLE_MOVEMENT	1
#define NUM_BATTLE_ARCHERS	1
#define NUM_BATTLE_ENGAGED	1

#define MDRED						120

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
extern LONG		GameSpecificGlobal_type;
   
static SHORT screennum = 0;
static SHORT screenend = 0;

/* ========================================================================
   Function    - IncDescribe
   Description - Go to the next page of text
   Returns     - void
   ======================================================================== */
void IncDescribe(LONG, LONG)
{
	fRedrawAll = TRUE;
	screennum++;
	if (screennum >= screenend)
	{
		HideMenu(D_DESCRIBE_GAME);
		RunMenus();
		fUpdatePanels = TRUE;
	}
}

/* ========================================================================
   Function    - PaintDescribe
   Description - paint the panel, face, and text
   Returns     - 
   ======================================================================== */
void PaintDescribe(LONG, LONG)
{
	if (!IsMenuActive(D_DESCRIBE_GAME) || screennum >= screenend)
	{
		fRedrawAll = TRUE;
		fUpdatePanels = TRUE;
		return;
	}
	SysHideCursor();
	init_gfont(FONT_SANS_8PT);
	print_textf(0, 0, DKBROWN, STRMGR_GetStr(screennum));
	
	SHORT	iButton = fERROR;
	iButton = GetResourceStd("UI\\MARBBTNA.PCX", FALSE);
	DrawBitmap(294, 360, iButton, 0, 0, 99, 99);
	SetPurge(iButton);
	
	init_gfont(FONT_SANS_8PT);
	char buf[80];
	if (GameSpecificGlobal_type == 5)
		strcpy(buf, STRMGR_GetStr(STR_GM_TUT_BUTS_MORE));
	else	
		strcpy(buf, STRMGR_GetStr(STR_GM_TUT_BUTS_DONE));
	print_textf(320, 368, MDRED, buf);
	ChangeButtonProc(D_DESCRIBE_GAME, 1, IncDescribe, 0, (GameSpecificGlobal_type == 5) ? D_KEY_MORE : D_KEY_DONE1);
	SysShowCursor();
}

/* ========================================================================
   Function    - Describe setup functions
   Description - 
   Returns     - 
   ======================================================================== */
void DescribeHistory(void)
{
	screennum = STR_DESCRIPTION_1;
	screenend = screennum + NUM_DESCRIPTION_SCREENS;
	ShowMenu(D_DESCRIBE_GAME);
}

void DescribeGame(void)
{
	screennum = STR_QUICK_INTRO_1;
	screenend = screennum + NUM_QUICK_INTRO_SCREENS;
	ShowMenu(D_DESCRIBE_GAME);
}

void DescribeAdventure(void)
{
	static BOOL have_seen = FALSE;
	
	if (have_seen)
		return;
	
	have_seen = TRUE;
	screennum = STR_ADVENTURE_1;
	screenend = screennum + NUM_ADVENTURE_SCREENS;
	ShowMenu(D_DESCRIBE_GAME);
}

void DescribeBattle(void)
{
	static BOOL have_seen = FALSE;
	
	if (have_seen)
		return;
	
	have_seen = TRUE;
	screennum = STR_BATTLE_1;
	screenend = screennum + NUM_BATTLE_SCREENS;
	ShowMenu(D_DESCRIBE_GAME);
}

void DescribeBtlWelcome(void)
{
	screennum = STR_BTLTUT_WELCOME;
	screenend = screennum + 1;
	ShowMenu(D_DESCRIBE_GAME);
}

void DescribeBtlReserves(void)
{
	screennum = STR_BTLTUT_RESERVES;
	screenend = screennum + 1;
	ShowMenu(D_DESCRIBE_GAME);
}

void DescribeBtlMovement(void)
{
	screennum = STR_BTLTUT_MOVEMENT;
	screenend = screennum + 1;
	ShowMenu(D_DESCRIBE_GAME);
}

void DescribeBtlArchers(void)
{
	screennum = STR_BTLTUT_ARCHERS;
	screenend = screennum + 1;
	ShowMenu(D_DESCRIBE_GAME);
}

void DescribeBtlEngagement(void)
{
	screennum = STR_BTLTUT_ENGAGEMENT;
	screenend = screennum + 1;
	ShowMenu(D_DESCRIBE_GAME);
}
