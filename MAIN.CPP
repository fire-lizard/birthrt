/* =================================================================
	Copyright (c) 1990,1995   Synergistic Software
	All Rights Reserved
	========================================================================
	Filename: MAIN.C -the main loop and other things
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
#include <io.h>

#ifndef _WINDOWS
#include <bios.h>
#else
#include <Windows.h>
#endif

//included by gamemap.hxx commented out to fix the "Segment too large"
//compiler error that seems to pop up randomly

#include "SYSTEM.H"
//#include "engine.h"
//#include "machine.h"
#include "MACHINT.H"


#include "GAMEMAP.HXX"

#include "MAIN.HXX"
#include "FINDFILE.HXX"
#include "SCENE.HXX"

#include "CHARSEL.HXX"
#include "GAME.H"
#include "GAMEKEY.HXX"
#include "REGENTS.HXX"
#include "REALM.HXX"            // GWP HACK addition
#include "SCNMGR.HXX"
#include "INVNGUI.HXX"
#include "MENU.H"
#include "REQUEST.H"
#include "PANEL.H"
#include "GMENUENM.H"
#include "FLICPLAY.HXX"
#include <dos.h>
#include "LIGHT.H"                                                      // [d7-03-96 JPC]
#include "STRMGR.H"
#include "strenum.h"
#include "PLAYSTAT.HXX"
#include "THINGS.H"
#include "MULTIUI.HXX"
#include "MARGIN.HXX"
#include "CONTEXT.HXX"
#include "UNITS.HXX"
#include "PLACES.HXX"
#include "TASK.H"

#ifdef _WINDOWS
#include "WINSYS\DDRAWPRO.H"
#include "WINSYS\MULPLAY.HXX"       //---- Multiplay
extern HWND hwndApp;
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
/* ------------------------------------------------------------------------
	Macros
	------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
	Prototypes
	------------------------------------------------------------------------ */
#if defined(_JUNEDEMO)
void ShowAdScreen(SHORT);
#endif

#ifdef _CHARED
void CharedDoEditorDialog ();
#endif

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

void SetRedrawMainMapLevel(void);
extern "C" {
void AskQuitSysProc(LONG MenuCombo, LONG button);
void ScaleBitmap_fine (SHORT x, SHORT y, SHORT iBitm, SHORT bx, SHORT by, SHORT w, SHORT h, LONG scale);
void inc_lens (LONG, LONG);
void dec_lens (LONG, LONG);

}

extern void ReadOptionSettings(void);
LONG AverageTicksPerFrame (void);


/* ------------------------------------------------------------------------
	Global Variables
	------------------------------------------------------------------------ */
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

#if defined(_WINDOWS)
extern HWND	hwndApp;
extern SHORT sDrawType;   // 5 SEP 96 CAM
extern BOOL  fStartSIGS;
#endif

// for critical errors due to CD removed
BOOL fGraphInitialized = FALSE;
BOOL fIsFadedOut = FALSE;

extern SHORT fFadedOut;
extern LONG cheated;

SHORT	gUnitStackSize = 15;

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
extern BOOL fInfoOnlyDomainUI;


extern LONG FlcDelay;

extern BOOL		fRedrawAll;

//extern "C" {
//extern LONG	JoyXPos;
//extern LONG	JoyYPos;
//extern LONG	JoyZPos;
//extern BOOL	JoyButton1;
//extern BOOL	JoyButton2;
//extern BOOL	JoyButton3;
//extern BOOL	JoyButton4;
//}

/* ========================================================================

	======================================================================== */

#ifndef _WINDOWS
void InvokeDebugger(LONG,LONG)
{
	union REGS regs;
	int386(0x03,&regs,&regs);
}
#endif

void SortThings(LONG, LONG)
{
	sort_things_from_camera(tot_nodes-1);
}

void toggle_ShowWadSize (LONG, LONG)
{
	bShowWadSize = !bShowWadSize;
}

void ShowWadSize (void)
{
	init_gfont(FONT_SANS_5PT);

	shade_edged_rect(60, 15, 150, 20, 32);
   print_textf(70,20,31,"Current wad size: %ld.%02ld meg",cbWad_Uses/1048576L,((cbWad_Uses%1048576L)*100)/1048576L);
}


void PrintLocationInfo(void)
{
	LONG floor;
	LONG ceiling;
	LONG special;
	LONG tag;
	LONG sector;
	
	const LONG player_ssector = find_ssector(PLAYER_INT_VAL(player.x),PLAYER_INT_VAL(player.y));
	ssector_to_sector_info( PLAYER_INT_VAL(player.x),PLAYER_INT_VAL(player.y),
 		player_ssector, &sector, &floor, &ceiling, &special, &tag);

#if defined(_DEBUG)
	// lot's of info
	print_textf( render_width/4, 35, 31,"^F00X=%ld, Y=%ld, Z=%ld, H=%ld, A=%ld, P=%ld Sector=%hd ssect=%hd",
 		PLAYER_INT_VAL(player.x), PLAYER_INT_VAL(player.y), player.z, player.h, player.a, player.p,
		sector, player_ssector);
		
	print_textf( render_width/4, 45, 31,"^F00CX=%ld, CY=%ld, CZ=%ld, CA=%ld, CP=%ld",
		CAMERA_INT_VAL(camera.x), CAMERA_INT_VAL(camera.y), camera.z, camera.a, camera.p);
	if (MapIsActive())
	{
		print_textf(render_width/4,55, 31, "^F00Map Zoom Factor %ld", MapZoomFactor());
	}
#else
	// just x,y location
	print_textf( render_width/4, 35, 31,"^F00X=%ld, Y=%ld", 
		PLAYER_INT_VAL(player.x), PLAYER_INT_VAL(player.y) );
#endif
	
	ScreenCopy(0, render_width/4, 25, render_width, 70, SC_DEFAULT_RES);
}

// Hack to switch between Direct draw and regular windows.
// GWP It crashes in direct draw and switching back out.
// CAM Not any more!
#if defined (_WINDOWS)
extern LONG lSavedX;
extern LONG lSavedY;
void ToggleDirectDraw(LONG, LONG)
{

	if (sDrawMode == iGDI)
	{
	    //GEH Leave this until direct draw 5.0 is working
	    #ifdef _JUNEDEMO
	    return;
	    #endif

		RECT rc;

		sDrawMode = iDDRAW;        // set the mode first! 5 SEP 96 CAM

 		 // -- Save the current window position for later	CAM 21 NOV 96
		GetWindowRect( hwndApp, &rc );
		lSavedX = rc.left;
		lSavedY = rc.top;

 		DefaultMainWindowSize( 0 );	// CAM 12 FEB 97

		if ( DDInitialize( hwndApp, FALSE ) )
		{
			ShowCursor( FALSE );
			SetRedrawMainMapLevel();
			screen_buffer_width = MAX_VIEW_WIDTH;
			screen_buffer_height = MAX_VIEW_HEIGHT;
			set_screen_size(screen_buffer_width,screen_buffer_height);

		}
		else
		{
			sDrawMode = iGDI;
	 		DefaultMainWindowSize( 0 );	// CAM 12 FEB 97
		}
	}
	else
	{
		sDrawMode = iGDI;   // set the mode first! 5 SEP 96 CAM
		DDCleanup();
		SetRedrawMainMapLevel();

 		DefaultMainWindowSize( 0 );	// CAM 21 NOV 96

		 // -- Make sure the desktop redraws		CAM 13 FEB 97

		InvalidateRect( NULL, NULL, TRUE );

		ShowWindow(hwndApp, SW_SHOW);
		ShowCursor( TRUE );
	}
}



extern void DirectDrawPreFrame( void )
{
	fDrawing = TRUE;
	if ( sDrawMode == iGDI )
	{
		screen = GDIscreen;
		screen_buffer_width = 640 /*WindowWidth*/;
	}
	else
	{
			sDrawType = iBLT;
	}
} // DirectDrawPreFrame



 // -- routine in sysgraph.c
#else
extern "C" void DirectDrawPreFrame( void )
{
}
extern "C" void DirectDrawSaveScreen( void )
{
}
#endif

// GWP HACK DEMO CODE
void GoToBattleScene(LONG, LONG)
{
	SCENE_MGR::Aggressor = REALM::AERENWE;
	SCENE_MGR::HomeIndex = 235;     // who is the home unit (w/camera)
	SCENE_MGR::AwayIndex = 236;     // who is the away unit
	SCENE_MGR::SceneType = SCENE_AI::BATTLE_SCENE;  // type for next scene ai
	SCENE_MGR::mfRequestNewScene( "batlw" , TYPE_PLAYERSTART1, TRUE); // load first scene
	InitDomainTurn(0);
}

/* =======================================================================
	Function    - machine_pre_frame
	Description - does pre_frame activities, such as checking the mouse
	Returns     - void
	======================================================================== */
static void machine_pre_frame()
{
	static LONG prev_button = 0;

	if(mouse_button != prev_button)
	{
		if(mouse_button)
			mouse_click = 1;
		else
			mouse_click = 0;

		prev_button = mouse_button;
	}

//	mouse_button = 0;		[GEH 9/26]

	zoomStart.x = cursor_x;
	zoomStart.y = cursor_y;  // zoom in & out menu

	// temp system counter
	//if (SCENE_MGR::fFadeToNextScene == SCENE_MGR::FADE_OUT)
	//{
		//if (SCENE_MGR::hCurrentScene != fERROR)
		//{
		//      PTR_SCENE pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
		//      pCurrentScene->mfSetScenePallet();
		//}
		//FadeOut(500);
		//SCENE_MGR::fFadeToNextScene = SCENE_MGR::FADE_IN;
 	//}
}

/* =======================================================================
	Function    - machine_post_frame
	Description - does post frame activities such as updating the screen
	Returns     - void
	======================================================================== */
static void machine_post_frame()
{
	static LONG	lTime = 0;

	// no faster then 18.2 frames per second
	if (lTime == get_time())
	{
		while(lTime == get_time())
		{
			//draw_cursor();
		}
	}
	
	// save new time
	lTime = get_time();

#ifdef _WINDOWS
	if ( sDrawMode == iGDI )
	{

		if(!fRender)
			update_screen();
	}
	else	// direct draw
	{
 		update_screen();
	}
#else

	if(!fRender)
		update_screen();
#endif

	if (SCENE_MGR::fFadeToNextScene == SCENE_MGR::FADE_IN)
	{
		MARGIN_CONTROL::mfResetCurrentSize();
		FadeIn(500);
		SCENE_MGR::fFadeToNextScene = SCENE_MGR::NO_FADE;
	}
}


/* ========================================================================
	Function    - MainLoop
	Description - Main loop for the game, called once per frame
	Returns     -
	======================================================================== */
void MainLoop(void)
{
	
#if defined (_DEBUG)
	static UCHAR	cMemDebugTimer = 0;
#endif

#if defined (_EDIT)
	static BOOL fShowMessage = TRUE;                // [d9-16-96 JPC]
#endif

	//this will do anything machine specific you want done
	//before a frame like clearing the screen or filling
	//key_status arrays....
	machine_pre_frame();

#if defined(_DEBUG)
	if(cMemDebugTimer++ == 0)
	{
		ULONG ulMemUsed = ReportInUse(FALSE);
		printf("MEM USAGE:	%ld\n", ulMemUsed);
	}
#endif
	
	// Touch all the memory attributes moldy bits.
	ClearAllMoldyBits();
	// check status of redbook audio, if stopped, loop it
#if !defined(_DEMO)
	CheckTheCDMusic();
#endif

	ServiceSOLAudio();

	//run tasks (based off frame counter)
	run_tasks();
	run_timers();

	#ifdef _WINDOWS
	if ( IsMultiPlayer() )
		PingMyMaster();
	#endif
	
	//run through regions list executing functions based
	//from keyboard and mouse input.
	if (REGION_HANDLED_EVENT == check_regions())
	{
		//mouse_button = 0;     // event handled, clear button
		mouse_click = 0;
	}
	
	// GWP Hack to get tool tips to paint on the map.
	if (!fRender)
	{
		paint_tooltips();
	}

#if defined(_WINDOWS)
   AMultiPlayer.LookForMessages();   //---- Update not specific messages
#endif

	DirectDrawPreFrame();

	// Domain turn vs. scene
	if (fDomainTurn)
	{
		DomainTurnUI();

		// display the panels
		RunPanels();

		// display menus into view buffer
		RunMenus();
		
		// display any request boxes
		RunRequests();
	}
	

	// Render a scene
	else
	{
		if(fQuitting)
			return;

		// Calls the Scene AI which in turn calls the Avatar AI's.
		SCENE_MGR::mfDoAI();

		run_timers();

		// render the current view to the buffer
		if(fRender)
		{
			CSTRPTR pArt;
			SHORT	hBitm;
			render_view(FALSE);
			GetButtonArt(D_GAMEBUTTON,1,&pArt);
			hBitm = GetResourceStd ( pArt, FALSE);
			if (hBitm != fERROR && fHighRes)
			{
				LONG X,Y,W,H;
				GetButtonPosition(D_GAMEBUTTON, 1, &X, &Y);
				GetButtonSize(D_GAMEBUTTON, 1, &W, &H);
				DrawBitmap (X, Y, hBitm, 0, 0, W, H);
				SetPurge(hBitm);
			}
			
			// High-res and low-res are no longer different here.
			ScreenCopy(0, margin_left, margin_top, render_width, render_height, SC_DEFAULT_RES);
#if defined (_WADBUILDERS)
			SysHideCursor ();
			SysForceCursor ();				// [d4-07-97 JPC]
#endif
		}

#if defined (_EDIT)
// [d9-16-96 JPC]
		if (fShowMessage)
		{
			char szTemp[128];
			sprintf (szTemp, "WAD Builders Edition, %s, %s", __DATE__, __TIME__);
			// TitledMessageBox::Display ("Special Version", szTemp);
			MessageBox (NULL, szTemp, "Special Version", MB_OK);
			fShowMessage = FALSE;
		}
#endif
		run_timers();

		// update running sounds by distance(volume) and pan

		UpdateLoopingSoundObjects();

		// if no menus are up, see if someone was clicked on
		if (!sMenusUp)
			handle_2dmouse(); // Handle events.

		// display the panels
		RunPanels();

		// display menus into view buffer
		RunMenus();
		
		// display any request boxes
		RunRequests();
		
		// paint the tool tips on top of the menus and panels.
		if (fRender)
		{
			paint_tooltips();
		}

		run_timers();

		if (bDumpScreen)
			DumpScreen(0,0);

		if (bShowWadSize)
			ShowWadSize();

#if !defined (_EDIT)
		if (SCENE_MGR::mfIsScenePaused())
		{
			print_textf(render_width/4, 20, 31, STRMGR_GetStr(STR_PAUSE));
		}
#endif
	}       // if in domain turn else..

	// Show frames per second
//#if !defined(_RELEASE)
	UpdateFPS();
	color_rect( render_width/2, render_top+5, 60, 15, 0);
	print_textf( render_width/2, render_top + 5, 31,"^F00F/S %ld  T/F %ld", fps, AverageTicksPerFrame());

//	print_textf( render_width/2, render_top + 5, 31,"^F00X:%ld Y:%ld %ld%ld%ld%ld",
//		JoyXPos,
//		JoyYPos,
//		JoyButton1,
//		JoyButton2,
//		JoyButton3,
//		JoyButton4
//	);

	ScreenCopy(0, render_width/2, render_top+5, 60, 15, SC_DEFAULT_RES);

	if (fPosition)
		PrintLocationInfo();


//#if defined(_WINDOWS)
//	if ( sDrawMode == iDDRAW )
//		print_textf( 320, render_top + 25, 31,"DDT %s",
//			(sDrawType == iDIRECTTOCARD) ? "ToCard" : "Blt" );
//#endif

//#endif

	// GWP HACK ALERT! HACK ALERT!
	// GWP Prevents drawing to the screen after the demo has tried to Exit.
	// GWP The idea is the last thing on the screen is a PCX not a view of
	// GWP the wad.
	if (SCENE_MGR::fbRender)
	{
		// this will do anything machine specific to the frame
		// like drawing a mouse cursor and then update screen.
		machine_post_frame();
	}

#ifdef _WINDOWS
	fDrawing = FALSE;
#endif

	fInfoOnlyDomainUI = FALSE;			// set to TRUE in multiui.cpp
}

/* ========================================================================
	Function    - MenuLoop
	Description - a game loop with no render
	Returns     -
	======================================================================== */
void MenuLoop()
{
	//this will do anything machine specific you want done
	//before a frame like clearing the screen or filling
	//key_status arrays....
	machine_pre_frame();

	// check status of redbook audio, if stopped, loop it
#if !defined(_DEMO)
	CheckTheCDMusic();
#endif

//#ifndef _WINDOWS
//#ifdef AUDIO_LIB
//	ServiceSOLAudio();
//#endif
//#endif

	//run tasks (based off frame counter)
	run_tasks();
	run_timers();

	//run through regions list executing functions based
	//from keyboard and mouse input.
	if (REGION_HANDLED_EVENT == check_regions())
	{
		//mouse_button = 0;     // event handled, clear button
		mouse_click = 0;
	}

#if defined(_WINDOWS)
	AMultiPlayer.LookForMessages();   //---- Update not specific messages
#endif

	DirectDrawPreFrame();

	// if no menus are up, see if someone was clicked on
	if (!sMenusUp)
		handle_2dmouse(); // Handle events.

	// display the panels
	RunPanels();

	// display menus into view buffer
	RunMenus();

	// display any request boxes
	RunRequests();
	
	#ifdef _WINDOWS
	if ( IsMultiPlayer() )
		PingMyMaster();
	#endif
	
	fRender = FALSE;					// cause machine_post_frame to update the screen
	machine_post_frame();

#ifdef _WINDOWS
	WindowsMessages();
#endif

}

/* ========================================================================
	Function    - GameMain
	Description - called once at start of game
	Returns     - void
	======================================================================== */
void GameMain(void)
{
	FILE *fp;
	char cpBuffer[256];
	LONG fileResult, vol, i;
 	char path[256];
	char waveres[256];
	//SHORT splashbitm;

	// Initialize Memory Manager, check for less than full needed memory
	//GEH const LONG MemAvailable = InitMemManag(MIN_MEMORY_TO_RUN, 3072L, 14000, FALSE);
	const LONG MemAvailable = InitMemManag(4098L, 3072L, 14000, FALSE);

#ifndef _CHARED
{
	gUnitStackSize = 15;
	
	// Setup graph mode AND load any graphical fonts... <graph.c>
	init_graph(1);          /* init in hires */

	// Must be done after memory is initialized.
	InitializeSoundSystem();

	init_timers();  //<see timers.c>
	// service the sound system 18 times a second
	add_timer(1,ServiceSOLAudio);

	// Install mouse handler.. check_regions will look at cursor_x
	// cursor_y and mouse_button
	init_mouse("DAGGER",CURSOR_HOTSPOT_UL);
#if !defined (_WINDOWS)
	if (!mouse_present)
	{
		fatal_error("You must have a mouse to run Birthright.  No mouse was detected.");
	}
#endif	
	
	for (i=0; i<MAX_UNITS; i++)
		units[i].iIconBitm = fERROR;
	for (i=0; i<MAX_PLACES; i++)
		places[i].iIconBitm = fERROR;
	
	//-------------------
	// load options here.
	//-------------------
	ReadOptionSettings();

	//-----------------------
	// load user scenes
	//-----------------------
	FindUserSceneFiles ( SCENEFILE_PATH, SCENEFILE_EXT );
	
	//-----------------------
	// Register resources here.
	//-----------------------
	// Setup resource files.
	RegisterResExtention("AVD", LoadAVD, DisposRes, SetPurgeRes, ClrPurgeRes, HashID);
	RegisterResExtention("IDA", LoadIData, DisposeIDataBlk, SetPurgeRes, ClrPurgeRes, HashCRC);
	RegisterResExtention("ANT", LoadAnimatedTextureStub, DisposRes, SetPurgeRes, ClrPurgeRes, HashCRC);
	//RegisterResExtention("WAV", LoadWAV, DisposRes, SetPurgeRes, ClrPurgeRes, HashCRC /* HashID*/);
	
	// Get these two res files before playing the intro.
	// get the right language
	OpenResFile_("wave.res");
	OpenResFile_("INTRO.RES");
	
#if !defined (_EDIT)
//	// -- immediate DirectDraw
//	ToggleDirectDraw( 0, 0 );
//=================================
	clear_key_status(0);
	if (!JustLoadWad)
	{
		// vol = GetGlobalVolume();
		// SetGlobalVolume((vol*8)/10);

		//---- If starting with SIGS don't play intro
#if defined(_WINDOWS)
		if ( fStartSIGS == FALSE )
#endif
			PlayIntro();

		
		// SetGlobalVolume(vol);
  		// [d1-07-97 JPC] You can quit while intro is playing--check
		// for that here.
		if (fQuitting)
			return;
	}
//=================================
#endif

#ifdef _DEMO
	PlayAdvFlc();
	set_hires(0, 0);
#endif

#if 0
// Do this just to build the shade table as needed.
	init_pal("nova_l");
	init_shade_table("nova_l");
#endif

#ifndef _WINDOWS	
	// only checks for button clicks
	add_timer(1,update_buttons);
#endif

	// preload a song from CD
	ucWhichTrack = 100;

	// always auto paint the cursor
	fAutoRestoreCursor = TRUE;

	init_tasks();   //<see tasks.c>

	init_regions(); //<see regions.c>

	init_game();    //<see game.c>
	
	// load font
	init_gfont(FONT_SANS_5PT);

#ifdef _WINDOWS
#ifdef _DEBUG
	InitRandomLog();
#endif
#endif

	SetupAdvsitePlaces();
	
	/* function keys */

#if defined(_DEBUG) && !defined(_EDIT)
   //  Moved this out of the way of DD and Chat dlj 9-26-96
	add_key(KEY_F6, print_point, 0, 0);
#endif

	/* keyboard keys */
	AddGameKeys();

#ifdef _DEMO
	JustLoadWad = FALSE;
#endif

#ifdef _WINDOWS
	ClearMessageQueue();
#endif
	clear_key_status(0);

	fGraphInitialized = TRUE;

	//[WRC]Inventory stuff
	ItemEffects::InitDurationServicing();

	//---------------------------------------------
	// fix the regent id entries in the units array
	//---------------------------------------------
	for (i=1; i < CHARACTER_COUNT; i++)
	{
		if (regents[i].mfGetunit() > 0 && regents[i].mfGeticon() != NO_MAP_ICON)
			units[regents[i].mfGetunit()].id = regents[i].mfGetid();
	}

	//-------------------
	// load player stats
	//-------------------
	StatReadAll();

	clear_key_status(0);
	ResumeSuspendedMusic();

#ifdef _DEMO
	PlayTrack(REDBOOK_REALM);
	SlideShowPlaying = FALSE;
	ServiceMusicWave(TRUE);
 	PlayTrack(REDBOOK_REALM);
#endif

	// begin with the domain turn
	if (JustLoadWad)
	{
		clear_screen_to (1);
	}
	else
	{
		InitDomainTurn(1);
	}
	
	SysShowCursor();
	
#ifndef _WINDOWS
	// the Windows main loop takes care of calling our MainLoop
	while(1)
	{
		MainLoop();  // call the main loop
	}
#endif
}
#else // _CHARED
{
   ScanResFiles ();							// [d5-21-97 JPC] moved here from
													// init_game.
	init_pal("nova_l");
	CharedDoEditorDialog ();
	fQuitting = TRUE;
}
#endif
}

/* ========================================================================
	Function    - CPPQuitSys
	Description - called once at quit time
	Returns     - void
	======================================================================== */
void CPPQuitSys(LONG, LONG)
{

#ifdef _CHARED
	quit_sys(0);
#endif

#if defined (_WADBUILDERS)
   QuitSys (0, 0);                     // skip confiming dialog
#else

	if ( mouse_button == 2 )
   {
   	SystemHelp( STR_MAIN1_HELP_QUIT_TITLE,
                   STR_MAIN1_HELP_QUIT_TEXT, H_Quit, 0, 0 );
	}
	else
	{
		// set the proc to null
		SetButtonProc  (D_QUESTION3, 0, NULL, 0, 0 );
			
		// set the question text
		SetButtonLabel  (D_QUESTION3, QUESTION_TEXT, STR_ASK_QUIT_SYS, BLACK );
			
		// turn on the first button as YES
		SetButtonLabel  (D_QUESTION3, QUESTION_BTN1, STR_YES, BTN_LABEL_COLOR );
		SetButtonProc   (D_QUESTION3, QUESTION_BTN1, AskQuitSysProc, QUESTION_BTN1, D_KEY_YES );
			
		// turn on the middle button as No
		SetButtonLabel  (D_QUESTION3, QUESTION_BTN2, STR_CANCEL, BTN_LABEL_COLOR );
		SetButtonProc   (D_QUESTION3, QUESTION_BTN2, AskQuitSysProc, QUESTION_BTN2, D_KEY_CANCEL );
		
		// turn on the last button as Credits
		SetButtonLabel  (D_QUESTION3, QUESTION_BTN3, STR_CREDITS, BTN_LABEL_COLOR );
		SetButtonProc   (D_QUESTION3, QUESTION_BTN3, AskQuitSysProc, QUESTION_BTN3, D_KEY_CREDITS );
			
		ShowMenu(D_QUESTION3);
	}
#endif
}

/* ========================================================================

	======================================================================== */
void DumpScreen (LONG, LONG)
{
	// start with screen 0
	static SHORT    filenumber = 0;
	static CSTRPTR  szFileName = "scrn0000.pcx";

	// dump build buffer to a .pcx file
	PutScreen(iDispBuffer, 0, 0, 640, 480,szFileName);

	// increment screen number
	if ((++filenumber) % 10 == 0)
	{
		szFileName[6]++;
		szFileName[7] = 48;
	}
	else
	{
		szFileName[7]++;
	}
}
// GWP Hack so it will compile.
void CoverInitDomain(LONG, LONG)
{
	TurnOffAllSounds();
	InitDomainTurn(0);
}
/* =======================================================================
	Function    - ActivateSegment
	Description - Cover fn for hot key.
	Returns     - void
	======================================================================== */
void ActivateSegment(LONG, LONG val)
{
	activate_seg(val);
}
/* =======================================================================
	Function    - print_point
	Description - Dumps the current camera point to a file.
	Returns     - void
	======================================================================== */
void print_point(LONG,LONG)
{
	FILE *fp = fopen("temp.pts", "a");

	if (fp)
	{
		fprintf(fp, "MoveCamera\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\n",
			PLAYER_INT_VAL(player.x),
			PLAYER_INT_VAL(player.y),
			player.z,
			player.h,
			player.a,
			player.p);
		fclose(fp);
	}
}

/* ========================================================================
	Function    - AddGameKeys
	Description - add the regions hotkeys for the game
	Returns     -
	======================================================================== */
void AddGameKeys(void)
{
	/* -----------------------------------------------------------------
	The letter keys
	----------------------------------------------------------------- */

#if defined(_DEBUG) && !defined(_EDIT)
	add_key(KEY_T, PlayCredits,0,0);
	add_key(KEY_Y, PlayFinale,0,0);
	add_key(KEY_1, PlayGorgon,1,0);
	add_key(KEY_2, PlayGorgon,2,0);
	add_key(KEY_3, PlayGorgon,3,0);
	add_key(KEY_4, ShowChatDisplayWindow,0,0);
	add_key(KEY_5, HideChatDisplayWindow,0,0);
#endif
	
	// GWP Remove cheat keys
#if defined (_EDIT) || defined (_WADBUILDERS)
	{
		extern BOOL fVeryFirstTime;

		add_key(KEY_C, ToggleVal, (LONG)&fClimb, 0);
      add_key(KEY_G, DumpScreen, 0, 0);            // dump screen buffer to .pcx
      add_key(KEY_F8, TextureLightTest, 0, 0);     // brighten things
      fVeryFirstTime = FALSE;                      // allow main menu to come up
	}
#endif

#if _LAN && defined(_DEBUG)
	add_key(KEY_E,ShowAllAnimations, 0, 0);
#endif

#ifdef _JPC
	add_key(KEY_E,  EditToggleSpeed,  0, 0);        // [d7-23-96 JPC]
#endif

	add_key(KEY_P, ToggleVal, (LONG)&fPosition, 0);

#if defined( _DEBUG) || defined(_EDIT)
	add_key(KEY_W, toggle_ShowWadSize,0, 0);
#endif

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#if defined (_DEBUG)
	add_key(KEY_EQUAL, inc_lens ,0, 0);
	add_key(KEY_BACKSLASH, dec_lens ,0, 0);
#endif
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	/* -----------------------------------------------------------------
		The special keys
		----------------------------------------------------------------- */

	add_key(GAME_KEYS::mfGetKey(GAME_KEYS::MAIN_MENU),GAMEToggleMainMenu,(LONG)TRUE, 0);
	add_key(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_TOGGLEDISPLAY),(PFVLL)SetMapDraw,TOGGLE,TOGGLE);

	// This is the scene level map keys.
	add_key(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_ZOOMOUT),ZoomMap,-1, 0);
	add_key(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_ZOOMIN),ZoomMap,+1, 0);
	
#ifdef _DEMO
	add_key(KEY_SPACEBAR,StartMapIntro,0,5);
#else
	add_key(GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_OPENDOOR),ActivateSegment,MAX_ACTIVATION_DISTANCE*MAX_ACTIVATION_DISTANCE,MAX_ACTIVATION_DISTANCE*MAX_ACTIVATION_DISTANCE);
#endif

	/* -----------------------------------------------------------------
		The Function keys
	----------------------------------------------------------------- */

	add_key(GAME_KEYS::mfGetKey(GAME_KEYS::HELP_MENU),ShowSubMenu,D_HELP, 0);

	// Add Global Keys

	AddGlobalKeys();


#ifdef _EDIT
	add_key(KEY_F7, EditDebugFunction, 0, 0);        // [d7-01-96 JPC]
	add_key(KEY_F8, EditDebugFunction, 1, 0);        // [d7-02-96 JPC]
	add_key(KEY_F9, EditDebugFunction, 2, 0);        // [d7-01-96 JPC]
#endif

#if defined(_DEBUG) && !defined(_WINDOWS)
	add_key(KEY_N, InvokeDebugger, 0, 0);
#endif

}

/* ========================================================================
	Function    - printf cover function
	Description - removes printfs from the code in the release version
	Returns     -
	======================================================================== */

#if defined(_RELEASE)
// for the release version, we don't want any printf's
int	printf( const char *__format, ... )
{
	return 0;
}
#endif

/* ========================================================================
	Function    - PlayIntro
	Description - Play the intro sequence
	Returns     -
	======================================================================== */
void PlayIntro(void)
{
	SHORT   bitm;
	LONG    now;
	BOOL	fFlicPlayedComplete;

	FadeOut(100);
	clear_screen();
	update_screen();
	init_pal("intropal");
	init_shade_table("intropal");
	FadeIn(100);

#ifdef _GEH_DEMO	
	TurnOffAllSounds();
#else
 	// add list of sound effects
 	ClearFlicSound();
	AddFlicSound(  1,SND_INTRO_LOGO2);
	AddFlicSound( 66,SND_SWORD_SLASH1);
	AddFlicSound( 67,SND_DRUM_ROLL_LOOP);
	AddFlicSound(100,SND_AMBIENT_SCABBARD_SCRAPE1);

	FlcDelay = 2;
	fFlicPlayedComplete = PlayFlic("intro\\intrbrpl.flc", 0);

	// [d1-07-97 JPC] You can quit while intro is playing--check
	// for that here.
	if (fQuitting)
		goto EndSplash;

	KillSoundsNoFade();
	AddSndObj(SND_AMBIENT_SCABBARD_HIT1,0,-1);      // sword clank
	FlcDelay = 34;
	// if we didn't already bail and we have enough memory, play history intro
#if !defined(_JUNEDEMO)	
	FadeOut(900);
	
	if (fFlicPlayedComplete)
	{
		NewIntro();
	}

#endif
#endif

	fAnyKeyChanged = FALSE;
	mouse_button = 0;

	run_timers();
	clear_screen();
//	FadeIn(1);
	update_screen();
	init_pal("intropal");
	init_shade_table("intropal");
	FadeIn(1);

	//GEH Splash Screen to eliminate the black screen
 	bitm = GetResourceStd ("intro\\introscr.pcx", FALSE);
	if (bitm != fERROR)
	{
		DrawBitmap (0, 0, bitm, 0, 0, 999, 999);
		update_screen();
		SetPurge(bitm);
	}
	else
		clear_screen();

EndSplash:
	ClearFlicSound();
	intro = FALSE;
	//KillSoundsNoFade();
	//SuspendMusic();

}

/* ========================================================================
	Function    - DrawHintPanel
	Description -
	Returns     -
	======================================================================== */
void DrawHintPanel(CSTRPTR szFile)
{
	SHORT   iBitm;
	SHORT	x, y, w, h;
	char	buffer[1024];
	char	buff[80];
	FILE	*in;
	
	fAnyKeyChanged = FALSE;
	mouse_button = 0;
	
	// draw the hint panel
	if ((iBitm = GetResourceStd("ui\\hntpanl1.pcx", FALSE)) == fERROR)
	{
		FadeIn(100);
		update_screen();
		return;
	}

	w = ((BITMPTR)BLKPTR(iBitm))->w;
	h = ((BITMPTR)BLKPTR(iBitm))->h;
	x = (SHORT)(320-(w/2));
	y = (SHORT)(240-(h/2));

	DrawBitmap(x, y, iBitm, 0, 0, w, h);

	// load the text file
	sprintf(buff,"%sTEXT\\%s", InstallPath, szFile);
	memset(&buffer[0], 0, sizeof(buffer));	// clear the buffer
	in = FileOpen(buff, "r");
	if(in != NULL)
	{
		fread(buffer, sizeof(buffer), 1, in);
		FileClose(in);
	}

	init_gfont(FONT_TIMS_10PT);
	print_textf(x+50, y+50, DKBROWN, buffer);
	print_textf(x+(w/2), y+h-20, DKBROWN, "^c%s",STRMGR_GetStr(STR_UNITS_FOOTER));
	draw_cursor();
	update_screen();

	SetPurge(iBitm);

	FadeIn(100);

	while(TRUE)
	{
		// we have to do this to keep the sound going in DOS
		run_timers();
		
		#ifdef _WINDOWS
		ClearMessageQueue();
		#endif
		
		update_buttons();
		if(mouse_button)
		{
			mouse_button = 0;
			SetRedrawMainMapLevel();
			return;
		}
		else
		if(fAnyKeyChanged)
		{
			fAnyKeyChanged = FALSE;
			clear_key_status(0);
			SetRedrawMainMapLevel();
			return;
		}
	}
}

/* ========================================================================
	Function    - PlayGorgon
	Description - Play a gorgon flic
	Returns     -
	======================================================================== */
void PlayGorgon(LONG arg, LONG)
{
	char buf[80];
	char buf1[80];

	// start the intro music
	TurnOffAllSounds();

	FadeOut(100);
	clear_screen();
	update_screen();

	switch (arg)
	{
	case 1:
		init_pal("gorgon2");

		// add list of sound effects
		ClearFlicSound();
		AddFlicSound(1, SND_DOOR_OPEN1);
		AddFlicSound(2, SND_DUNGEON1);
		AddFlicSound(11, SND_FOOTSTEP4);
		AddFlicSound(18, SND_FOOTSTEP5);
		AddFlicSound(24, SND_FOOTSTEP6);
		AddFlicSound(29, SND_SERVANT_VOX1);
		AddFlicSound(31, SND_FOOTSTEP7);
		AddFlicSound(38, SND_FOOTSTEP8);
		AddFlicSound(44, SND_FOOTSTEP1);
		AddFlicSound(75, SND_SHOVE_CHAIR1);
		AddFlicSound(82, SND_GORGON_THROW_OBJECT1);
		AddFlicSound(94, SND_GORGON_VOX1);
		strcpy(buf, "intro\\gorgon2.flc");
		strcpy(buf1, "gorgon1.txt");
		break;

	case 2:
		init_pal("gorgon3");

		// add list of sound effects
		ClearFlicSound();
		AddFlicSound(1, SND_DOOR_OPEN1);
		AddFlicSound(2, SND_DUNGEON1);
		AddFlicSound(11, SND_FOOTSTEP4);
		AddFlicSound(18, SND_FOOTSTEP5);
		AddFlicSound(25, SND_FOOTSTEP6);
		AddFlicSound(32, SND_FOOTSTEP7);
		AddFlicSound(39, SND_FOOTSTEP1);
		AddFlicSound(48, SND_SERVANT_VOX2);
		AddFlicSound(108, SND_GORGON_VOX2);
		AddFlicSound(116, SND_SHOVE_CHAIR1);
		strcpy(buf, "intro\\gorgon3.flc");
		strcpy(buf1, "gorgon2.txt");
		break;

	case 3:
		init_pal("gorgon4");
		// [d11-11-96 JPC] Don't use shade tables with flics.

		// add list of sound effects
		ClearFlicSound();
		AddFlicSound(1, SND_DOOR_OPEN1);
		AddFlicSound(2, SND_DUNGEON1);
		AddFlicSound(11, SND_FOOTSTEP2);
		AddFlicSound(18, SND_FOOTSTEP3);
		AddFlicSound(21, SND_SERVANT_VOX3);
		AddFlicSound(24, SND_FOOTSTEP4);
		AddFlicSound(30, SND_FOOTSTEP5);
		AddFlicSound(74, SND_GORGON_VOX3);
		AddFlicSound(80, SND_SHOVE_CHAIR1);
		AddFlicSound(95, SND_SUMMON_SWORD1);
		AddFlicSound(103, SND_SERVANT1_SCREAM1);
		AddFlicSound(111, SND_GORGON_FOOTSTEP1);
		AddFlicSound(116, SND_GORGON_FOOTSTEP2);
		AddFlicSound(120, SND_GORGON_FOOTSTEP3);
		AddFlicSound(124, SND_GORGON_FOOTSTEP4);
		strcpy(buf, "intro\\gorgon4.flc");
		strcpy(buf1, "gorgon3.txt");
		break;
	}

	clear_key_status(0);
	FadeIn(100);
	update_screen();

	FlcDelay = 2;
	PlayFlic(buf, 4);

	ClearFlicSound();
	TurnOffAllSounds();

	FadeOut(100);

	clear_screen();
	update_screen();

	init_pal("nova_l");
	init_shade_table("nova_l");

	DrawHintPanel(buf1);

	ResumeSuspendedMusic();	// cdb 2/10 the music wasn't restarting after the gorgon flic.
}

/* ========================================================================
	Function    - PlayCredits
	Description - Play the credits sequence
	Returns     -
	======================================================================== */
void PlayCredits(LONG,LONG)
{
	CHAR	filename[40];
	SHORT	i;
	SHORT	bitm;
	LONG	now;
	
	StopRedBook();
	KillSoundsNoFade();
	PlayTrack(103);
	
//	for(i = 1; i <= 8; i++)  //---- Changed 6-6-97 by RC
	for(i = 1; i <= 7; i++) 
	{
		sprintf(filename, "intro\\credit%02d.pcx", i);
		
		bitm = GetResourceStd (filename, FALSE);
  		
		if (bitm != fERROR)
		{
			POINT start,bck;
			
			start.x = 320;
			start.y = 240;
			bck.x = 320;
			bck.y = 240;
			ZoomBitmap (0, 0, bitm, 0, 0, 640, 480, fERROR, start, bck, 640, 480);
			update_screen();
			SetPurge(bitm);
		}
		else
		{
			continue; // can't find this one, try the next
		}
		
		init_gfont(FONT_SANS_8PT);
		switch(i)
		{
		case 1:
			//print_textf(90,455,31,"The Birthright Art Staff in costume on the set of the Imperial Palace in Anuire.");
			print_textf(90,455,31,STRMGR_GetStr(STR_CREDIT_1));
			print_textf(  5,155,31,"Captain Alan Speed");
			print_textf( 74,179,31,"Guilder Jeremy Jones");
			print_textf(158,145,31,"Baron Darren Lamb");
			print_textf(216,338,31,"Spider King Kirt Lemons");
			print_textf( 20,245,31,"Earl Jason Robertson");
			print_textf(353,126,31,"Count Tim Jensen");
			print_textf(391,331,31,"Peter Stonecrusher Watje");
			print_textf(418, 64,31,"Patrick Gargoyle Watje");
			print_textf(526,119,31,"Mike Manslayer");
			print_textf(541,129,31,"McMillen");
			print_textf(302,222,31,"Black Knight Jerry Speed");
			print_textf(534,292,31,"Jason Goblin Jones");
			print_textf(240,110,31,"Lord Alex Bennion");
			break;
		case 2:
			//print_textf( 64,455,31,"The Birthright designer/sound/management team preparing for the evening's torture sessions.");
			print_textf( 64,455,31,STRMGR_GetStr(STR_CREDIT_2));
			print_textf( 10,321,31,"Craig Utterback the Grim");
			print_textf(  5,155,31,"High Mage Derek Clardy");
			print_textf(100,300,31,"Michael Zombie Ormsby");
			print_textf( 96,118,31,"Ann Swordwraith Clardy");
			print_textf(155, 95,31,"Linda Harpy Westerfield");
			print_textf(197,344,31,"Archbishop Chris Barker");
			print_textf(248,127,31,"Lady Kate Policani");
			print_textf(317,150,31,"Mona the Blade Branham");
			print_textf(375,174,31,"Lord Kevin Green");
			print_textf(471, 34,31,"Robert Gorgon Clardy");
			break;
		case 3:
			//print_textf(137,455,31,"The Birthright Programming staff taking a break between melees.");
			print_textf(137,455,31,STRMGR_GetStr(STR_CREDIT_3));
			print_textf(  9,198,31,"Lady Lan Zhou");
			print_textf( 63,178,31,"Lord Mike Branham");
			print_textf(137,190,31,"Assassin Gary");
			print_textf(152,200,31,"Powell");		
			print_textf(196,159,31,"Wizard Wes Cumberland");
			print_textf(269,175,31,"Alan Ogre Clark");
			print_textf(538,280,31,"Ranger Cheryl");
			print_textf(548,290,31,"Makovsky");
			print_textf(513,150,31,"Archduke David");
			print_textf(528,160,31,"Jackson");
			print_textf(383,233,31,"Greg Hightower The Tall");
			print_textf(289,130,31,"General John Conley");
			print_textf(449,256,31,"Stumpy Donald Tsang");
			print_textf(393,155,31,"Baron Evan Dean");
			print_textf(353,100,31,"Invisible Man James David Walley");
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		}
		
		update_screen();
		
		// delay 20 seconds
		now=get_time() + (20 * 182 / 10);
		
		#ifdef _WINDOWS
		ClearMessageQueue();
		#endif
						
		fAnyKeyChanged = FALSE;
		clear_key_status(0);
		mouse_button = 0;
	
		while(get_time()<now)
		{
			run_timers();
			
			#ifdef _WINDOWS
			ClearMessageQueue();
			#endif
						
			update_buttons();
			if(mouse_button)
			{
				mouse_button = 0;
				break;
			}
			else
			if(fAnyKeyChanged)
			{
				if(key_status(KEY_ESCAPE))
				{
					goto End;
				}
				fAnyKeyChanged = FALSE;
				clear_key_status(0);
				break;
			}
		}
	}
End:
	fAnyKeyChanged = FALSE;
	clear_key_status(0);
	//add_timer(1,draw_cursor);
	return;	
}

/* ========================================================================
	Function    - PlayFinale
	Description - Play the credits sequence
	Returns     -
	======================================================================== */
void PlayFinale(LONG WinningRealm,LONG)
{
	CHAR	filename[40];
	CHAR	name[10];
	CHAR	temp[200];
	CHAR	buffer[200];
	CHAR	rlm[10];
	CHAR	battle[10];
	CHAR	game[10];
	SHORT	i;
	SHORT	bitm;
	LONG	now;
	CHAR 	*Dash;
	SHORT  prevTrack = ucWhichTrack;
	
	StopRedBook();
//	PlayTrack(REDBOOK_REALM);
	KillSoundsNoFade();

	// this sound doesn't play
	PlayTrack(SND_WIN_GAME_MUSIC1);	
	
	if ( WinningRealm == 0 )
	{
		WinningRealm = HomeRealm;

#ifdef _WINDOWS

		//---- Better tell the losers ?

		if ( IsMultiPlayer() )
		{
			AMultiPlayer.SendWonGame ( HomeRealm );
		}

#endif

	}

#if defined(_JUNEDEMO)
	bitm = GetResourceStd ("graphics\\keepplay.pcx", FALSE);
#else
	bitm = GetResourceStd ("finale\\finale.pcx", FALSE);
#endif
 		
	if (bitm != fERROR)
	{
		DrawBitmap (0, 0, bitm, 0, 0, 640, 480);
		SetPurge(bitm);
	}
	
#if !defined(_JUNEDEMO)
	// use only first 8 chars up to a space
	memset(&name[0], 0, 9);
	// [d4-09-97 JPC] Modified the following to use the access
	// function instead of direct access to the obsolete .name member
	// of the REGENT class.
	strncpy(name, regents[realm[WinningRealm].mfGetRegent()].mfGetname(), 8);
	if((Dash = strchr(name, ' ')) != NULL)
	{
		Dash[0] = 0;
	}
						
	sprintf(filename, "finale\\%s.PCX",name);
	
	bitm = GetResourceStd (filename, FALSE);
	if (bitm == fERROR)
	{
		// maybe this is a player rolled character try and get art that at least matches
		// the flc art.
		SHORT const RegentId = regents[realm[WinningRealm].mfGetRegent()].mfGetid();
		SHORT const hPlayerStats = LoadStats(RegentId, fERROR);
		DumbAutoLockPtr<PLAYER_STATS const> dhPlayerStats(hPlayerStats);
		sprintf(filename, "finale\\fl%s.PCX",GAME_TTYPE::mfGetArtFileName(dhPlayerStats->mfGetType()));
		bitm = GetResourceStd (filename, FALSE);
	}
 		
	if (bitm != fERROR)
	{
		DrawBitmap (280, 152, bitm, 0, 0, 72, 176);
		SetPurge(bitm);
	}
	
	init_gfont(FONT_TITL_16PT);
	strcpy(temp,STRMGR_GetStr(STR_FINALE_SCREEN1));
	// [d4-09-97 JPC] Modified the following to use the access
	// function instead of direct access to the obsolete .name member
	// of the REGENT class.
	sprintf(buffer,temp,regents[realm[WinningRealm].mfGetRegent()].mfGetname());
	print_textf(175,30,1,buffer);
	
	init_gfont(FONT_TITL_8PT);
	
	strcpy(temp,STRMGR_GetStr(STR_FINALE_SCREEN2));
	print_textf(190,54,1,temp);
	
	strcpy(temp,STRMGR_GetStr(STR_FINALE_SCREEN51));

	if( TypesDiffs[HomeRealm][1] == STR_REALMPLAYTYPE_EAS)
		strcpy(rlm,STRMGR_GetStr(STR_FINALE_AEASY));
	else if( TypesDiffs[HomeRealm][1] == STR_REALMPLAYTYPE_MED)
		strcpy(rlm,STRMGR_GetStr(STR_FINALE_AMED));
	else if( TypesDiffs[HomeRealm][1] == STR_REALMPLAYTYPE_HRD)
		strcpy(rlm,STRMGR_GetStr(STR_FINALE_AHARD));

	sprintf(buffer,temp,rlm);
	print_textf(450,130,1,buffer);
	
	strcpy(temp,STRMGR_GetStr(STR_FINALE_SCREEN52));
	if( DifficultyLevel == 1)
		strcpy(battle,STRMGR_GetStr(STR_FINALE_HARD));
	else
	if( DifficultyLevel == 2)
		strcpy(battle,STRMGR_GetStr(STR_FINALE_MED));
	else
	if( DifficultyLevel == 3)
		strcpy(battle,STRMGR_GetStr(STR_FINALE_EASY));
	sprintf(buffer,temp,battle);
	print_textf(450,140,1,buffer);

	strcpy(temp,STRMGR_GetStr(STR_FINALE_SCREEN53));
	if( gWinningPoints	== 300 )
		strcpy(game,STRMGR_GetStr(STR_FINALE_EASY));
	else
	if( gWinningPoints	== 400 )
		strcpy(game,STRMGR_GetStr(STR_FINALE_MED));
	else
	if( gWinningPoints	== 500 )
		strcpy(game,STRMGR_GetStr(STR_FINALE_HARD));
	sprintf(buffer,temp,game);
	print_textf(450,150,1,buffer);
	
	if(cheated)
	{
		strcpy(temp, STRMGR_GetStr(STR_FINALE_CHEATED));
		sprintf(buffer, temp, cheated);
		print_textf(450,160,1,buffer);
	}
		
	strcpy(temp,STRMGR_GetStr(STR_FINALE_SCREEN61));
	print_textf(450,175,1,temp);
	strcpy(temp,STRMGR_GetStr(STR_FINALE_SCREEN62));
	print_textf(450,185,1,temp);
	strcpy(temp,STRMGR_GetStr(STR_FINALE_SCREEN63));
	print_textf(450,195,1,temp);
	
	strcpy(temp,STRMGR_GetStr(STR_FINALE_SCREEN71));
	print_textf(450,210,1,temp);
	strcpy(temp,STRMGR_GetStr(STR_FINALE_SCREEN72));
	print_textf(450,220,1,temp);
	strcpy(temp,STRMGR_GetStr(STR_FINALE_SCREEN73));
	print_textf(450,230,1,temp);
#endif //_JUNEDEMO
	
	update_screen();
		
	// delay 45 seconds
	now=get_time() + (45 * 182 / 10);
	
	#ifdef _WINDOWS
	ClearMessageQueue();
	#endif
					
	fAnyKeyChanged = FALSE;
	clear_key_status(0);
	mouse_button = 0;

	while(get_time()<now)
	{
		run_timers();
		
		#ifdef _WINDOWS
		ClearMessageQueue();
		#endif
					
		update_buttons();
		if(mouse_button)
		{
			mouse_button = 0;
			break;
		}
		else
		if(fAnyKeyChanged)
		{
			if(key_status(KEY_ESCAPE))
			{
				goto End;
			}
			fAnyKeyChanged = FALSE;
			clear_key_status(0);
			break;
		}
	}

End:
	FadeOut(100);

	clear_screen();
	update_screen();
	
	init_pal("nova_l");
	init_shade_table("nova_l");

	update_screen();
	
	fRedrawAll = TRUE;
	fAnyKeyChanged = FALSE;
	clear_key_status(0);
	
	FadeIn(100);
	PlayTrack(prevTrack);	
	
	//add_timer(1,draw_cursor);
	return;	
}

void CoverResetSound( LONG, LONG )
{
	//GEH fade out and reset sound system
	FadeToReset();
}

/* ========================================================================
	Function		- AddGlobalKeys
	Description - add the global keys ( always work )
	Returns     -
   ======================================================================== */
void AddGlobalKeys(void)
{

#if defined(_WINDOWS)
	// Direct Draw hot keys
	add_key(GAME_KEYS::mfGetKey(GAME_KEYS::TOGGLE_DIRECT_DRAW), ToggleDirectDraw, 0, 0);
//	add_key(GAME_KEYS::mfGetKey(GAME_KEYS::TOGGLE_CHAT_SELECT_WINDOW),ShowChatSelectWindow, 0, 0);
	add_key(GAME_KEYS::mfGetKey(GAME_KEYS::TOGGLE_CHAT_DISPLAY_WINDOW),ToggleChatDisplayWindow, 0, 0);
#endif

	add_key(GAME_KEYS::mfGetKey(GAME_KEYS::TOGGLE_CHAT_WINDOW),ShowChatWindow, 0, 0);

	add_key(KEY_F8, CoverResetSound, 0, 0);        // [d7-02-96 JPC]
}
/*===============================================================================
function CheckTheCDMusic  (actually, CD or .wav music)
purpose  if the music is done, start a frame counter to wait a while before starting it again.
===============================================================================*/
void CheckTheCDMusic(void)
{
	static LONG StartTime = 0;
	static SHORT delayTicks;

	if(fMusic && !CheckCDBusy() && 	(ucWhichTrack != 100) )
	{
		if( (ucWhichTrack == REDBOOK_REALM) ||	(ucWhichTrack == REDBOOK_PERSCOMBAT) )
			PlayTrack(ucWhichTrack);

		else if(StartTime == 0)
		{
			StartTime = get_time();
			delayTicks= (60 + random(120) * 18 );  // 18 ticks/second
		}
		else if( ( (get_time() ) - StartTime) > delayTicks )
		{
			PlayTrack(ucWhichTrack);
			StartTime = 0;
		}
	}
}

/* ========================================================================
   Function    - AskQuitSysProc
   Description -
   Returns     -
   ======================================================================== */
void AskQuitSysProc(LONG MenuCombo, LONG button)
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
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	
	// yes, quit
	if (button == QUESTION_BTN1)
	{
#if defined(_WINDOWS)
		AMultiPlayer.Finalize();
#endif
#if defined(_JUNEDEMO)
		ShowAdScreen(1);
#endif
		QuitSys(0,0);
	}
	else
	if (button == QUESTION_BTN3) 	// play credits, then quit
	{
#if defined(_WINDOWS)
		AMultiPlayer.Finalize();
#endif
		PlayCredits(0,0);
#if defined(_JUNEDEMO)
		ShowAdScreen(1);
#endif
		QuitSys(0,0);
	}
	// else just return
}

/* ========================================================================
   Function    - DoPan
   Description -
   Returns     -
   ======================================================================== */
static LONG			fPan = 0;
static LONG			fWindow = 0;
static SHORT		iScreen;
static LONG			wx = 0, wy = 0, ww = 640, wh = 480;
static LONG			cx = 0, cy = 0, cw = 640, ch = 480;

void DoPan (void)
{
	LONG			x, f;
	static LONG		ox = 0;

	if (fPan)
	{
		x = ((get_time()-fPan)*125) / 100;
		if (x!=ox)
		{
			ox = x;
			DrawBitmap(0, 0, iScreen, x, 0, 640, 480);
			update_screen();
			if (x>(1920-640))
				fPan = FALSE;
			run_timers();
		}
	}

	if (fWindow && fWindow != get_time())
	{
		fWindow = get_time();
		f = FALSE;
		if (wx > cx) {cx += ((wx - cx)/10)+1; f=TRUE;}
		if (wx < cx) {cx += ((wx - cx)/10)-1; f=TRUE;}
		if (wy > cy) {cy += ((wy - cy)/10)+1; f=TRUE;}
		if (wy < cy) {cy += ((wy - cy)/10)-1; f=TRUE;}
		if (ww > cw) {cw += ((ww - cw)/10)+1; f=TRUE;}
		if (ww < cw) {cw += ((ww - cw)/10)-1; f=TRUE;}
		if (wh > ch) {ch += ((wh - ch)/10)+1; f=TRUE;}
		if (wh < ch) {ch += ((wh - ch)/10)-1; f=TRUE;}
		color_rect(0, 0, 640, 480, 0);
		DrawBitmap(cx, cy, iScreen, cx, cy, cw, ch);
		update_screen();
		if (!f)
		{
			fWindow = FALSE;
			//wx = 0; wy = 0; ww = 640; wh = 480;
			//cx = 0; cy = 0; cw = 640; ch = 480;
		}
		run_timers();
	}


}

/* ========================================================================
   Function    - FadeTo
   Description -
   Returns     -
   ======================================================================== */
void FadeTo (LONG red, LONG green, LONG blue, LONG steps)
{
	UBYTE	CardPal[768];
	UBYTE	TempPal[768];
	LONG	i, j, c;

printf("FadeTo called %d/%d/%d, %d\n", red, green, blue, steps);

	if (steps==0)		// just set palette
	{
		for (i=0; i<768; i+=3)
		{
			TempPal[i] = red;
			TempPal[i+1] = green;
			TempPal[i+2] = blue;
		}
		set_pal((char *)TempPal);
		return;
	}

	else if (steps > 0)					// fade to color
	{
		for (i = 0; i < 768; i++)
		{
			LONG cpv = i;
			if (cpv >= 512)
			{
				cpv -= 512;
			}
			else if (cpv >= 256)
			{
				cpv -= 256;
			}
			CardPal[i] = cpv;
		}

		for (j=0; j>steps; j++)		/* step through intensity levels */
		{
			run_timers();
			DoPan();

			for (i=0; i<768; i+=3)
			{
				TempPal[i] = (UBYTE)((LONG)CardPal[i] + (((red - (LONG)CardPal[i]) * j) / steps));
				TempPal[i+1] = (UBYTE)((LONG)CardPal[i+1] + (((green - (LONG)CardPal[i+1]) * j) / steps));
				TempPal[i+2] = (UBYTE)((LONG)CardPal[i+2] + (((blue - (LONG)CardPal[i+2]) * j) / steps));
			}
			set_pal((char *)TempPal);
		}
		return;
	}

	else						// fade from color to CurPal
	{
		for (i=0; i<256; i++)
		{
			CardPal[i*3]   = CurPal[i].bRed;
			CardPal[(i*3)+1] = CurPal[i].bGreen;
			CardPal[(i*3)+2] = CurPal[i].bBlue;
		}

		steps = -steps;
		for (j=steps; j>=0; j--)		/* step through intensity levels */
		{
			run_timers();
			DoPan();

			for (i=0; i<768; i+=3)
			{
				TempPal[i] = (UBYTE)((LONG)CardPal[i] + (((red - (LONG)CardPal[i]) * j) / steps));
				TempPal[i+1] = (UBYTE)((LONG)CardPal[i+1] + (((green - (LONG)CardPal[i+1]) * j) / steps));
				TempPal[i+2] = (UBYTE)((LONG)CardPal[i+2] + (((blue - (LONG)CardPal[i+2]) * j) / steps));
			}
			set_pal((char *)TempPal);
		}
	}

}

/* ========================================================================
   Function    -
   Description -
   Returns     -
   ======================================================================== */
void CrossFade (SHORT iScreen, LONG steps)
{
	LONG			j,x,y,z,zz,ps,pd,st,wid;
	BITMPTR		s;
	PTR			sptr;
	static LONG	hash[256] = {-1,0};
	static LONG	hash_s[256] = {-1,0};

	if (steps <= 1)
	{
		DrawBitmap(0, 0, iScreen, 0, 0, 640, 480);
		update_screen();
		return;
	}

	if (hash[0] == -1)
	{
		for (j=x=0; x<256; x++)
		{
tryagain:
			j++;
			if (j>100000)
				break;
			z = random(256);
			for (y=0; y<x; y++)
				if (hash[y]==z)
					goto tryagain;
			hash[x]=z;
		}
		for (x=0; x<256; x++)
			hash[x] = ((hash[x]&0x00F0L)*40L) + (hash[x]&0x000FL);
	}

	s = (BITMPTR) BLKPTR(iScreen);			// get pointer to bitm header
	if (!IsPointerGood(s))
		return;
	sptr = (PTR)s + sizeof(BITMHDR);			// get pointer to data
	wid = s->w;

	for (x=0; x<256; x++)
		hash_s[x] = ((hash[x]&0xFFFFFFF0L)*(wid/640L)) + (hash[x]&0x000FL);

	if (steps < 256)
	{
		st = 256 / steps;
		for (z=0; z<256; z+=st)
		{
			for (zz=0; zz<st; zz++)
			{
				for (x=0; x<640; x+=16)
				{
					pd=hash[z+zz]+x; ps=hash_s[z+zz]+x;
					for (y=0; y<480; y+=16, pd+=(640*16), ps+=(wid*16))
						if (pd>=0 && pd<(640*480) && ps>=0 && ps<(wid*480))
							screen[pd] = sptr[ps];
				}
				run_timers();
			}
			update_screen();
		}
	}
	else
	{
		st = steps / 256;
		for (z=0; z<256; z++)
		{
			for (zz=0; zz<st; zz++)
			{
				for (x=0; x<640; x+=16)
				{
					pd=hash[z]+x; ps=hash_s[z]+x;
					for (y=0; y<480; y+=16, pd+=(640*16), ps+=(wid*16))
						if (pd>=0 && pd<(640*480) && ps>=0 && ps<(wid*480))
							screen[pd] = sptr[ps];
				}
				run_timers();
			}
			update_screen();
		}
	}
}

/* ========================================================================
   Function    -
   Description -
   Returns     -
   ======================================================================== */
#define FULL_SCALE_FINE		(FULL_SCALE*256)
#define THRQTR_SCALE_FINE	(FULL_SCALE*384)

void NewIntro (void)
{
	LONG			i = 0, j, oj, curScreen = 99, x=0, ox=999, y, z=0, oz=999;
	LONG			startTime;
	SHORT			iScreen_[12];
	SHORT			curSound = fERROR;
	int			SoundTag = fERROR;
	LONG			fZoom = 0;
	SHORT			iDark = fERROR;
	LONG			hash[256];
	SHORT			bitm;
	static char* scrname[] = {	"INTRO\\SCREEN0.PCX",
				       	"INTRO\\SCREEN1.PCX",
				       	"INTRO\\SCREEN2.PCX",
				       	"INTRO\\SCREEN3.PCX",
				       	"INTRO\\SCREEN4.PCX",
				       	"INTRO\\SCREEN5.PCX",
				       	"INTRO\\SCREEN6.PCX",
				       	"INTRO\\SCREEN7.PCX",
				       	"INTRO\\SCREEN8.PCX",
				       	"INTRO\\SCREEN9.PCX",
				       	"INTRO\\SCREENA.PCX",
					"INTRO\\SCREEN2B.PCX"};
	const LONG NumberOfScreens = sizeof(scrname)/sizeof(char *);
	static LONG	timing[] = {
 0, 0, 0,108,306,306,580,580,684,1160,1160,1160,1160,1160,1680,1680,
1760,1760,1862,1862,1974,1974,1974,2210,2210,2304,2304,2304,2516,2516,2685,2685,2685,2750,2750,3060,3060,3337,3466,3466,3530,3568,3568,3568,3586,3586,3586,3606,3606,3606,3624,3624,3624,3646,3646,3646,00,3850,0};
	static LONG	effect[] = {																																																																																																																																												
 0,30,70, 24,  1, 60,  2, 60, 40,  11,  51,  21,  40,  52,   3,  60,
  62,  41,  63,  41,  13,   4,  23,   5,  60,  14,   6,  23,  65,  41,  13,   7,  22,  66,  41,  67,  41,  14,   8,  21,  13,   8,  21,  13,   9,  21,  13,   9,  21,  13,  10,  21,  13,  10,  21,  14,00,   0,99};

  	clear_key_status(0);

#ifdef _WINDOWS
	color_rect(0, 0, 640, 480, 0);
	iDark = SaveBitmap(0, 0, 640, 480);
#endif

	for (j=0; j<NumberOfScreens; j++)
		iScreen_[j] = GetResourceStd(scrname[j], FALSE);

	KillSoundsNoFade();
	startTime = get_time();
	fPan = 0;

	while (effect[i] != 99)
	{
#ifdef _WINDOWS
		WindowsMessages();
#endif
		update_buttons();
		if (mouse_button
			|| key_status(KEY_ESCAPE)
			|| key_status(KEY_SPACEBAR)
			|| key_status(KEY_RETURN) )
		{
			break;
		}

		run_timers();
		DoPan();

		if (get_time() > (startTime+timing[i]))
		{
			switch (effect[i])
			{
				case 0:
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
					fPan = fZoom = 0;
					curScreen = effect[i];
					iScreen = iScreen_[curScreen];
					wx = 0; wy = 0; ww = 640; wh = 480;
					cx = 0; cy = 0; cw = 640; ch = 480;
					break;

#ifdef _WINDOWS
				case 12:	CrossFade(iDark, 1); break;
				case 13:	CrossFade(iDark, 4); break;
				case 14:	CrossFade(iDark, 20); break;
				case 15:	CrossFade(iDark, 75); break;
				case 16:	CrossFade(iDark, 150); break;
				case 21: CrossFade(iScreen, 1); break;
				case 22: CrossFade(iScreen, 4); break;
				case 23: CrossFade(iScreen, 20); break;
				case 24: CrossFade(iScreen, 75); break;
#else
				case 12:	FadeOut(1); break;
				case 13:	FadeOut(180); break;
				case 14:	FadeOut(1000); break;
				case 15:	FadeOut(3000); break;
				case 16:	FadeOut(5000); break;
				case 21: DrawBitmap(cx, cy, iScreen, cx, cy, cw, ch); update_screen(); FadeIn(1); break;
				case 22: DrawBitmap(cx, cy, iScreen, cx, cy, cw, ch); update_screen(); FadeIn(200); break;
				case 23: DrawBitmap(cx, cy, iScreen, cx, cy, cw, ch); update_screen(); FadeIn(1000); break;
				case 24: DrawBitmap(cx, cy, iScreen, cx, cy, cw, ch); update_screen(); FadeIn(4000); break;
#endif

				case 30: SoundTag = AddSndObj(SND_INTRO_FLC_VOX1,0,-1); break;

				case 40: fPan=get_time(); x = 0; break;
				case 41: fWindow=get_time(); x = 0; break;

				case 51:	FadeTo(255, 255, 255, -3); break;			// white flash..


#ifdef _WINDOWS
				case 52:	FadeTo(255, 255, 255, -40); break;			// ..and fade away
#else
				case 52:	FadeTo(255, 255, 255, -500); break;			// ..and fade away
#endif

#ifdef _WINDOWS
				case 60: CrossFade(iScreen,50); break;	// crossfade
#else
				case 60: CrossFade(iScreen,80); break;		// crossfade
#endif
				// default
				case 61: wx = 0; wy = 0; ww = 640; wh = 480; break;
				// zoom down to Spider
				case 62: wx = 77; wy = 160; ww = 335; wh = 236; break;
				// zoom over to Rhuobhe
				case 63: wx = 395; wy = 108; ww = 132; wh = 262; break;
				//	Raesene
				case 64: cx = 279; cy = 174; cw = 138; ch = 202; color_rect(0, 0, 640, 480, 0); break;
				//	Roele on throne
				case 65: wx = 248; wy = 98; ww = 140; wh = 245; break;
				// Michele Roele dead
				case 66: wx = 0; wy = 195; ww = 212; wh = 232; break;
				// Gorgon on crag
				case 67: wx = 280; wy = 0; ww = 325; wh = 480; break;
			
				case 70:						// init the screen
						// this is now done below
					FadeOut(100);
					run_timers();
					clear_screen();
					run_timers();
					update_screen();
					run_timers();
					init_pal("nova_l");
					init_shade_table("nova_l");
					run_timers();
					set_hires(0, 0);
					run_timers();
					set_pal((char *)&CurPal[0]);
					run_timers();
#ifdef _WINDOWS
					FadeIn(1);
#else
					FadeOut(1);
#endif
					break;

			}
			i++;
		}
	}

	StopASound(SND_INTRO_FLC_VOX1, SoundTag);

	for (j=0; j<NumberOfScreens; j++)
	{
		if (iScreen_[j] != fERROR)
			SetPurge(iScreen_[j]);
	}
	if (iDark != fERROR)
		DisposBlock(iDark);
	clear_key_status(0);
}

/* ======================================================================== */

