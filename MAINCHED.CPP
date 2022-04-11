/* =================================================================
	Copyright (c) 1990,1995   Synergistic Software
	All Rights Reserved
	========================================================================
	Filename: MAINCHED.CPP, adapted from MAIN.CPP by John Conley for
									the Birthright I Character Editor.
	Author: Chris Phillips & Wes Cumberland
	
	========================================================================
	Contains the following internal functions:
	machine_pre_frame     -does preframe things, such as check the mouse
	machine_post_frame    -does postframe things, such as updating the screen

	Contains the following general functions:
	GameMain                -the main loop

	========================================================================*/
/* ------------------------------------------------------------------------
	Includes
	------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#ifndef _WINDOWS
#include <bios.h>
#else
#include <windows.h>
#endif

#include "chared.hxx"
#include "system.h"
#include "engine.h"
#include "machine.h"
#include "machint.h"
#include "main.hxx"
#include "charsel.hxx"
#if 0
#include "game.h"
#include "gamekey.hxx"
#include "realm.hxx"            // GWP HACK addition
#include "scnmgr.hxx"
#include "itemtype.hxx"
#include "invngui.hxx"
#include "menu.h"
#include "panel.h"
#include "gmenuenm.h"
#include "flicplay.hxx"
#include <dos.h>
#include "light.h"                                                      // [d7-03-96 JPC]
#include "strmgr.h"
#include "strenum.h"
#include "playstat.hxx"
#include "things.h"
#include "multiui.hxx"
#include "margin.hxx"
#include "context.hxx"
#include "units.hxx"
#endif

#ifdef _WINDOWS
// #include "winsys\DDrawPro.h"
// #include "winsys\mulplay.hxx"       //---- Multiplay
#if defined (_CHARED)
#include "itemtype.hxx"
#include "chared.hxx"					// CHARED
#endif
#endif


#if defined (_EDIT) || defined (_WADBUILDERS)
extern "C" {
void TextureLightTest (LONG arg1, LONG arg2);
void EditToggleSpeed (LONG arg1, LONG arg2);
void EditDebugFunction (LONG arg1, LONG arg2);
void SysForceCursor (void);
}
#endif

/* ------------------------------------------------------------------------
	Defines and Compile Flags
	------------------------------------------------------------------------ */
#define SCRN_DELAY 0x2FFFFFF
//#define SCRN_DELAY 0x2FFF

// key to the animation showing code
#define _LAN	01
#define DKBROWN					128
#define MAX_ACTIVATION_DISTANCE	100		//HACK alert, already #defined
										//in sectors.c
#define NUMBER_OF_SCREENS	11

/* ------------------------------------------------------------------------
	Macros
	------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
	Prototypes
	------------------------------------------------------------------------ */
void DumpScreen (LONG, LONG);
void toggle_ShowWadSize (LONG, LONG);
void ShowWadSize (void);
void InitDomainTurn(LONG);
void DomainTurnUI (void);
static void machine_pre_frame();
static void machine_post_frame();

void PlayCredits(LONG,LONG);
void PlayFinale(LONG,LONG);
void PlayGorgon(LONG arg,LONG);
void CheckTheCDMusic(void);

#ifdef _DEMO
void PlayBattleFlc(void);
void PlayAdvFlc(void);
void PlayMapIntro(LONG,LONG);
void StartMapIntro(LONG,LONG);
#endif
void PlayIntro(void);
void AddFlicSound(LONG whichFrame, LONG whichSound);
void ClearFlicSound(void);
void NewIntro (void);

extern "C" {
void SetRedrawMainMapLevel (void);
void AskQuitSysProc(LONG MenuCombo, LONG button);
void ScaleBitmap_fine (SHORT x, SHORT y, SHORT iBitm, SHORT bx, SHORT by, SHORT w, SHORT h, LONG scale);
void inc_lens (LONG, LONG);
void dec_lens (LONG, LONG);

}

extern void ReadOptionSettings(void);


/* ------------------------------------------------------------------------
	Global Variables
	------------------------------------------------------------------------ */
extern "C" {
extern SHORT sMenusUp;
extern PTR GDIscreen;
extern LONG tot_thing_spans;
extern BOOL fDrawing;
extern LONG	cbWad_Uses;
extern BOOL fQuitting;

extern BOOL JustLoadWad;
extern BOOL fAutoRestoreCursor;

extern LONG	margin_left;
extern LONG	margin_right;
extern LONG	margin_top;
extern LONG	margin_bottom;

extern COLORSPEC CurPal[256];

//GEH this will go away later...
char InstallPath[256];

// GEH this should go into system.h
extern BOOL		fMinMemory ;

#if defined(_WINDOWS)
extern HWND	hwndApp;
extern SHORT sDrawType;   // 5 SEP 96 CAM
#endif

// for critical errors due to CD removed
BOOL fGraphInitialized = FALSE;
BOOL fIsFadedOut = FALSE;
}

extern SHORT fFadedOut;
extern LONG cheated;

SHORT	gUnitStackSize = 15;

extern BOOL	fChatLineOn;

SHORT InstallationType;
char CDDrive[4];

LONG fDialogUp;
BOOL bShowWadSize=FALSE;
BOOL bDumpScreen=FALSE;
//BOOL fRender = TRUE;
BOOL fPosition = FALSE;
void print_point(LONG,LONG);
void ActivateSegment(LONG, LONG);
void CoverInitDomain(LONG, LONG);

extern BOOL fDomainTurn;
extern BOOL fClimb;
extern LONG	DifficultyLevel;


extern LONG FlcDelay;

extern BOOL		fRedrawAll;

extern "C" {
/* ========================================================================
	Function    - GameMain
	Description - called once at start of game
	Returns     - void
	======================================================================== */
void GameMain(void)
{
	// Initialize Memory Manager, check for less than full needed memory
	//GEH const LONG MemAvailable = InitMemManag(MIN_MEMORY_TO_RUN, 3072L, 14000, FALSE);
	const LONG MemAvailable = InitMemManag(4098L, 3072L, 14000, FALSE);

	init_pal("nova_l");
	init_shade_table("nova_l");
   ScanResFiles ();							// [d5-21-97 JPC] moved here from
													// init_game.
	CharedDoEditorDialog ();
	fQuitting = TRUE;
}

/* ========================================================================
	Function    - CPPQuitSys
	Description - called once at quit time
	Returns     - void
	======================================================================== */
void CPPQuitSys(LONG, LONG)
{

   quit_sys (0);                     // skip confiming dialog
}

} // extern "C"

/* ========================================================================

	======================================================================== */
