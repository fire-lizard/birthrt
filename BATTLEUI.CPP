/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: battleui.cpp  -
   Author:   Greg Hightower

   ========================================================================

   Contains the following general functions:


   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <ctype.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WINDOWS)
#include <windows.h>
#include "winsys\mulplay.hxx"
#endif

#include "system.h"
#include "engine.h"
#include "machint.h"
#include "machine.h"

#include "charsel.hxx"
#include "main.hxx"
#include "menu.h"
#include "margin.hxx"
#include "panel.h"
#include "game.h"
#include "gamekey.hxx"
#include "gmenuenm.h"
#include "avatar.hxx"
#include "scene.hxx"
#include "scnai.hxx"
#include "regents.hxx"
//#include "btlstr.h"
#include "battle.hxx"
#include "battleui.hxx"
#include "strmgr.h"
#include "strenum.h"
#include "sndvox.hxx"
#include "invngui.hxx"
#include "vector.hxx"
#include "context.hxx"
#include "describe.hxx"
#include "gamemap.hxx"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

extern void RandomLogComment ( char * szString );

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
//#define  LOGCOMMENTSLEEP	Sleep(1);
#define  LOGCOMMENTSLEEP	{;}

#define NPCBATTLE_ACTION 199
#define BTL_KILL_DELAY 4

enum {
	BTL_CURSOR_DAGGER,
	BTL_CURSOR_AHEAD,
	BTL_CURSOR_BEHIND,
	BTL_CURSOR_LEFT,
	BTL_CURSOR_RIGHT,
	BTL_CURSOR_MARCH,
	BTL_CURSOR_ENGAGE,
	BTL_CURSOR_MISSILE,
	BTL_CURSOR_MAGIC,
	BTL_CURSOR_CANCEL,
};

#define BTL_MAGIC_SPELL_LIST_X1	 30
#define BTL_MAGIC_SPELL_LIST_X2	180
#define BTL_MAGIC_SPELL_LIST_Y	  8

#define BTL_MAGIC_SPELL_MISSILES		6
#define BTL_MAGIC_SPELL_MOVE		7
#define BTL_MAGIC_SPELL_RESERVES	8

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
void IncreaseBattleScreen(LONG, LONG);
void DecreaseBattleScreen(LONG, LONG);

void CollapseUnits(SHORT UnitIndex);

void DumpScreen (LONG, LONG);
void toggle_ShowWadSize (LONG, LONG);
void BattleCameraPosition(LONG, LONG);
void CheckMissileRange();
void CheckMagicRange();
static void SetMagicRegions(void);
static void ClearMagicRegions(void);
static BOOL HilightTheBattleGrids( BOOL );
static BOOL BattleMissilesReady(CAvatar *pAvatar);
static void BattleMissileBox( LONG, LONG );
static void BattleHotBox( LONG, LONG );

void BattleMagicPaint(LONG MenuCombo);
void BattleMissilePaint(LONG MenuCombo);

void Init_MessQue ( void );

void SendWin ( BOOL IWin );
void SendMagic ( CAvatar *pAvatar );
void SendToReserves ( CAvatar *pAvatar );
void SendFallBack ( CAvatar *pAvatar, SHORT R, SHORT C );

void SetExceptionGrids(void);

extern "C" {
void inc_lens (LONG, LONG);
void dec_lens (LONG, LONG);
LONG GetButtonValue ( LONG MenuIndex, LONG ButtonId );

}

void SetBattleUIMouseCursor (void);

static PFVLLL	fPrevObjMouseCallback = 0;
static PFVLLL	fPrevWallMouseCallback = 0;
static PFVLLL	fPrevFloorMouseCallback = 0;
static PFVL		fPrevRightMouseCallback = 0;
static void FieldFloorClick(LONG ButtonClicked, LONG SectorClicked, LONG WhichType);
static void FieldObjectClick(LONG ButtonClicked, LONG ObjectClicked, LONG WhichType);
static BOOL ConvertFieldToGrid( LONG fx, LONG fy, LONG *gx, LONG *gy, BOOL *home);
static BOOL WhichGridHalf( LONG CX, LONG CY, SHORT *Row, SHORT *Column, BOOL *Home);

static void ChangeBattleCursor (CHAR *pArtName, BOOL HotSpot);

static SHORT HilightPossibleGrids( LONG MenuId, LONG R, LONG C, LONG Distance, BOOL *fTest );

static void BattleUpdateTime(void);
static void CheckBattleGrid();

extern BOOL RegentDeathTest (void);
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
BOOL	fBtlTurnBased = TRUE;		// turn based battles
LONG	BtlActionRate = 45;		// ticks per battle action round, about 2.5 sec
LONG	DifficultyLevel = 3;	// 1 is most difficult
LONG	SpeedLevel = 2;			// 1 is fastest

SHORT	BattleCurrentSpell = fERROR;	// current battle spell

SHORT	gAwayUnits = 0;			// used in aibtlcap.cpp

SHORT sHomeRealm;
SHORT sAwayRealm;

extern DECL_MATRIX_CLASS_S(POINT,BattleGridHome,GRID_MAX_ROWS,GRID_MAX_COLS);	// aibtlcap.cpp

extern BOOL fRender;	// main.cpp
extern BOOL fPause;		// game.cpp

extern BOOL fTutorialFirstBattle;	// from gamemap.cpp

int		BtlSoundTag = fERROR;

LONG	BattleCursor = BTL_CURSOR_DAGGER;

SHORT	sBattleMode = BTLUI_MODE_RESERVES;
SHORT	sOldBattleMode = BTLUI_MODE_RESERVES;

static CHAR		cBattleField = '0';

static	BOOL	HomeStrengthLarger = FALSE;
static	SHORT	sBattleCameraCount = 50;
static	BYTE	bBattleCameraMode = BTLUI_CAMERA_FIXED;

static SHORT	battleTroops = fERROR;
static SHORT	startLoc = -2;	// invalid

SHORT	hUnitInfo = fERROR;
SHORT	hMissile = fERROR;

static SHORT	sBattleCameraState = CAMERA_BEHIND;

BOOL		fBattleMovement = FALSE;
BOOL		fBattleReserves = FALSE;

static LONG		UnitFieldX = 0;
static LONG		UnitFieldY = 0;
static BOOL		UnitFieldCursor = FALSE;

static LONG		BattleCountDown;
static LONG		MovementCountDown;
static LONG		WinCount = 0;
static BOOL		BtlGoesWell = FALSE;
static BOOL		BtlVictoryAtHand = FALSE;
static BOOL		BtlVictory = FALSE;
static BOOL		BtlGoesIll = FALSE;
static BOOL		BtlDefeatAtHand = FALSE;
static BOOL		BtlWithdraw = FALSE;
static BOOL		BtlPrompted = FALSE;
static BOOL		BtlCowards = FALSE;

BOOL		fDoTutWelcome = TRUE;
BOOL		fDoTutReserves = TRUE;
BOOL		fDoTutMove = TRUE;
BOOL		fDoTutEngage = TRUE;
BOOL		fDoTutArchers = TRUE;

static BOOL		fBtlChat = FALSE;
extern SHORT   ChatDisplaySize;
static BOOL		LastChatDisplaySize = ChatDisplaySize;

extern "C" {
BOOL	fBattleCombat = FALSE;
}

BOOL		fBattleStarted = FALSE;

BOOL	fBtlMagicType = BTLCAP_NOSHOT;
BOOL	fBtlMagicResult = BTL_N;

static BOOL	fOldAutoRes;

//---- Other realm is remote

DWORD dwBattleMultiId = kNoMultiId;    //---- If non zero then we have an remote player

SHORT hHilightAvatar = fERROR; 
SHORT	sHilightMenuId ; 
SHORT	sHilightRow ; 
SHORT	sHilightColumn ; 
SHORT	sHilightMovement ;

/* keep the handles of which battle captain is where */
DEFINE_MATRIX_DATA_S(GRID,WhosWhere,GRID_MAX_ROWS,GRID_MAX_COLS);
DEFINE_MATRIX_CLASS_S(GRID,WhosWhere,GRID_MAX_ROWS,GRID_MAX_COLS);

DEFINE_MATRIX_DATA_S(SHORT,Reserves,2,MAX_RESERVES);
DEFINE_MATRIX_CLASS_S(SHORT,Reserves,2,MAX_RESERVES);

DEFINE_VECTOR_DATA_S(SHORT,ReservesGridX,MAX_RESERVES) = {
	6, 70, 134, 198, 262,
	6, 70, 134, 198, 262,
	6, 70, 134, 198, 262,
};
DEFINE_VECTOR_CLASS(SHORT, ReservesGridX);

DEFINE_VECTOR_DATA_S(SHORT,ReservesGridY,MAX_RESERVES) = {
	37, 37, 37, 37, 37,
	62, 62, 62, 62, 62,
	87, 87, 87, 87, 87,
};
DEFINE_VECTOR_CLASS(SHORT, ReservesGridY);

/* exceptions list to battle grids */
DEFINE_MATRIX_DATA_S(SHORT,BtlExceptions,GRID_MAX_ROWS,GRID_MAX_COLS);
DEFINE_MATRIX_CLASS_S(SHORT,BtlExceptions,GRID_MAX_ROWS,GRID_MAX_COLS);

DEFINE_VECTOR_DATA_S(SHORT,BtlGridX,GRID_MAX_COLS) =
{
	BTLUI_GRID_X1,
	BTLUI_GRID_X2,
	BTLUI_GRID_X3,
	BTLUI_GRID_X4,
	BTLUI_GRID_X5,
};
DEFINE_VECTOR_CLASS(SHORT, BtlGridX);

DEFINE_VECTOR_DATA_S(SHORT,BtlGridY,GRID_MAX_ROWS) =
{
	BTLUI_GRID_Y3,
	BTLUI_GRID_Y2,
	BTLUI_GRID_Y1,
};
DEFINE_VECTOR_CLASS(SHORT, BtlGridY);

static DEFINE_STATIC_VECTOR_DATA(int,StatusText) = {
STR_STATUS_INIT,			
STR_STATUS_RELEASE,				
STR_STATUS_BEGIN_PAUSE,			
STR_STATUS_PAUSED,				
STR_STATUS_END_PAUSE,			
STR_STATUS_MOVING,				
STR_STATUS_READFILE,			
STR_STATUS_RUNNING,				
STR_STATUS_SEARCH,				
STR_STATUS_ATTACK,				
STR_STATUS_DEFEND,				
STR_STATUS_FALLBACK,			
STR_STATUS_CASTSPELL,			
STR_STATUS_BEGIN_LISTEN,		
STR_STATUS_LISTEN,				
STR_STATUS_END_LISTEN,			
STR_STATUS_BEGIN_LISTEN_BOW,	
STR_STATUS_END_LISTEN_BOW,		
STR_STATUS_ROTATE_TO_CAMERA,	
STR_STATUS_RETURN_TO_POSITION,	
STR_STATUS_DEAD,				
STR_STATUS_FIDGET,
STR_STATUS_FALLING,
STR_STATUS_STONE,
STR_STATUS_CONSUME,
STR_STATUS_RESURRECTION,

};
DEFINE_VECTOR_CLASS(int, StatusText);

#ifndef _WINDOWS
extern "C" {
}
#endif

static BOOL BtlLowResAni;

BOOL fBattleFreezeTime = FALSE;
BOOL fBattleNextTurn = FALSE;
LONG BattleTick = 0;
BOOL fUnitsStillMoving = FALSE;

/* #################################################################
   #################################################################
   #################################################################
   
        Battle Initialize Routines
   
   #################################################################
   #################################################################
   ################################################################# */

/* ========================================================================
   Function    - InitBattleUI
   Description - Load and Lock the battle UI
   Returns     - void
   ======================================================================== */
void InitBattleUI(SCENE &rScene)
{
	SHORT i;

	//---- Clear the message que

	#ifdef _WINDOWS
	if ( IsMultiPlayer() )
	{
		#ifdef _DEBUG
		// clear the debug battle log
		FILE *fPtr;
		fPtr = fopen ("battle.log","w");
		if (fPtr != NULL)
			fclose(fPtr);
		#endif
		
		Init_MessQue();
	}
	#endif
	
	/* -----------------------------------------------------------------
	   force ultra low animations on the battle field to save memory
	   and gain speed
	   ----------------------------------------------------------------- */
	BtlLowResAni = fLowResAni;
	fLowResAni = TRUE;
	
	// reset our clock
	BattleUpdateTime();				// clear out old junk
	BattleTick = SCENE_MGR::gTick;	// clear current time
	BattleUpdateTime();				// set new current time
	
	sHomeRealm = units[SCENE_MGR::HomeIndex].Realm;
	sAwayRealm = units[SCENE_MGR::AwayIndex].Realm;
	
	// -- Store power of these units
	if ( UnitPower(SCENE_MGR::HomeIndex) > UnitPower(SCENE_MGR::AwayIndex) )
		HomeStrengthLarger = TRUE;
		
	/* -----------------------------------------------------------------
	   Put the display in hires, no auto res, and add a lower margin
	   for the control panel
	   ----------------------------------------------------------------- */
	//printf("Begin Battle Init...");
	fflush(stdout);
	
	SuspendMusic();
	ClearRemapTable();
	
	// global to tell the world that were on the field
	fBattleCombat = TRUE;
	battleTroops = fERROR;
	
	// set up the engine and panel flags and shut off auto res'ing
	fRender = TRUE;
	fUpdatePanels = TRUE;
	fOldAutoRes = fAIAutoRes;
	fAIAutoRes = FALSE;
	
	/* -----------------------------------------------------------------
	   now add back all the keys I want to support
	   ----------------------------------------------------------------- */
	AddGameKeys();
	
	//GEH add_key(KEY_F4,BattleCameraHighView, 0, 0);
	
	add_key(GAME_KEYS::mfGetKey(GAME_KEYS::EXPAND_VIEWING_AREA), IncreaseBattleScreen, 0, 0);
	add_key(GAME_KEYS::mfGetKey(GAME_KEYS::SHRINK_VIEWING_AREA), DecreaseBattleScreen, 0, 0);
		
	/* -----------------------------------------------------------------
	   Clear the battle data array
	   ----------------------------------------------------------------- */
	// GWP memset(WhosWhere, -1, sizeof(WhosWhere));
	// GWP memset(Reserves, -1, sizeof(Reserves));
	memset(WhosWhere, -1, sizeof(GRID) * GRID_MAX_ROWS * GRID_MAX_COLS);
	memset(Reserves, -1, sizeof(SHORT) * 2 * MAX_RESERVES);
			
	/* -----------------------------------------------------------------
	   set up the misc battle stuff
	   ----------------------------------------------------------------- */
	//hUnitInfo = units[SCENE_MGR::HomeIndex].iHandle;
	hUnitInfo = fERROR;
	battleTroops= fERROR;
	BattleCameraHighView(0, 0);
	
	// always start in reserves mode
	sBattleMode = -1;
	BattlePanelMode(BUILD_LONG(D_BTL_CONTROL,0),BTLUI_MODE_RESERVES);
	
	BattleCurrentSpell = fERROR;
	
	///// menu stuff
	gAwayUnits = 0;			// used in aibtlcap.cpp
	
	// turn off unit info back light
	SetButtonHilight(D_BTL_CONTROL, BTL_LIGHT_ID, FALSE);
	
	// clear modal grid hilights
	for(i=2;i<17;i++)
		SetButtonHilight(D_BTL_CONTROL, i, FALSE);
	
	fBattleStarted = FALSE;	// battle not officially started yet
	fBattleMovement = FALSE;// no one has move yet
	fBattleReserves = FALSE;// no one has move yet out of reserves
	BattleCountDown = 60;	// and must satisfy start conditions for 60 frames
	MovementCountDown = 300;// they must eventually move some troops or loose
	WinCount = 0;			// how long a win/loose condition must hold
	
	fDoTutWelcome = TRUE;	// tutorial mode stuff
	fDoTutReserves = TRUE;	// tutorial mode stuff
	fDoTutMove = TRUE;
	fDoTutEngage = TRUE;
	fDoTutArchers = TRUE;
	
	// set fade distance
	SetSoundDecay(5000, 10000);
	
	// start background sounds
	BtlSoundTag = AddSndObj(SND_BATTLE_LOOP1, 0, VOLUME_EIGHTY);
	
	// play the battle track
//	PlayTrack(REDBOOK_BATTLE);
	
	// init the flags that control voice queues
	BtlPrompted = FALSE;
	BtlCowards = FALSE;
	BtlGoesWell = FALSE;
	BtlVictoryAtHand = FALSE;
	BtlVictory = FALSE;
	BtlGoesIll = FALSE;
	BtlDefeatAtHand = FALSE;
	BtlWithdraw = FALSE;

	//---- Check if the away team is remote if so setup the player id
#ifdef _WINDOWS
	
	if ( lRealmSelectedByPlayer[ units[SCENE_MGR::AwayIndex].Realm] != -1 )
	{
		dwBattleMultiId = lRealmSelectedByPlayer[ units[SCENE_MGR::AwayIndex].Realm ];
	}
	else
#endif
	{
		dwBattleMultiId = kNoMultiId;
	}
	
 	/* -----------------------------------------------------------------
	   deal with the chat dialogs
	   ----------------------------------------------------------------- */
	FadeOut(100);
	fBtlChat = fChatLineOn;	// save this value
	LastChatDisplaySize = ChatDisplaySize;
	BattleChatMode( 0, 1);
	ChatLineClear();
	
}

/* ========================================================================
   Function    - ReleaseBattleUI
   Description - release the stuff used in a battle AI
   Returns     -
   ======================================================================== */
void ReleaseBattleUI(SCENE &rScene)
{
	// release ani back to correct resolution
	fLowResAni = BtlLowResAni;
	
	// clean up a couple values
	bBattleCameraMode = BTLUI_CAMERA_FIXED;
	hUnitInfo = fERROR;
	
	// global to tell the world that the battle's over
	fBattleCombat = FALSE;
	fRender = FALSE;
	fBattleFreezeTime = FALSE;
	fPause = TRUE;
	fAIAutoRes = fOldAutoRes;
	SetSoundDecay(100, 200);
	StopASound(SND_BATTLE_LOOP1, BtlSoundTag);
	
	HidePanel(D_BTL_CONTROL);
	HideChatDisplayWindow(0,0);
	fChatLineOn = fBtlChat;
	
}

/* #################################################################
   #################################################################
   #################################################################
   
        Battle Camera Routines
   
   #################################################################
   #################################################################
   ################################################################# */

/* ========================================================================
   Function    - BattleMoveCamera
   Description - move the camera around the battle field
   Returns     -
   ======================================================================== */
void BattleMoveCamera(void)
{
	LONG	Temp;
	BOOL	fPosition = FALSE;
	SHORT	HomeUnitCount, HomeUnitAlive, HomeUnitTotal;
	SHORT	AwayUnitCount, AwayUnitAlive, AwayUnitTotal;
	SHORT	ResvCount, ResvAlive, ResvTotal;
	SHORT	HomeTotal, AwayTotal;
	
	LONG	TargetAngle;
	LONG	TargetRadius;
	LONG	TargetHeight;
	LONG	TargetPitch;
	
	static	BOOL	fRotatedCamera = FALSE;
	static	POINT	TempPoint = {0,-1500};
	static 	BOOL fOldMoving = !fUnitsStillMoving;
		
	
	// run battle clock
	BattleUpdateTime();
	
	CheckBattleGrid();
	
	if ( ChatDisplaySize != LastChatDisplaySize)
	{
		LastChatDisplaySize = ChatDisplaySize;
		BattleChatMode( 0, 0);
	}
	
	// check if anyone is in missile or magic range
	CheckMissileRange();
	CheckMagicRange();
	
	if(bBattleCameraMode == BTLUI_CAMERA_FOLLOW)
	{
		if(hUnitInfo != fERROR)
		{
			CAvatar * const pAvatar = (CAvatar * const)BLKPTR(hUnitInfo);
			SetCameraRotatePointTarget( &camera,
				pAvatar->mfX(),			// x
				pAvatar->mfY() + 150,	// y
				0,						// z
				NO_CHANGE,				// angle
				NO_CHANGE,				// radius
				1);						// swoop
		}
	}
	else
	{
		if(camera.RTarget.a < 95 )
			camera.RTarget.a = 95;
		else
		if(camera.RTarget.a > 160 )
			camera.RTarget.a = 160;
	}
	
	MoveCamera(&camera,CheckMoveKeys());

	// -----------------------------------------------------------------
	// -- OK, enough camera crap, let's check on the real game
	// -----------------------------------------------------------------

	// -- check for turn based mode
	if ( fBtlTurnBased )
	{
		if ( fOldMoving != fUnitsStillMoving )
		{
			ClearButtonFlag (D_BTL_CONTROL, BTL_DONE_ID, D_LABEL_OFF);
			SetButtonType   (D_BTL_CONTROL, BTL_DONE_ID, BUTTON_BITMAP );
			
			if ( fUnitsStillMoving )
			{
				activate_region (BTL_DONE_ID, FALSE);
				SetButtonHilight(D_BTL_CONTROL, BTL_DONE_ID, FALSE);
			}
			else
			{
				activate_region (BTL_DONE_ID, TRUE);
				SetButtonHilight(D_BTL_CONTROL, BTL_DONE_ID, TRUE);
			}
			fUpdatePanels = TRUE;	// paint new button value
			fOldMoving = fUnitsStillMoving;
		}
			
		fUnitsStillMoving = FALSE;
	}
	else
	{
		// make the button disappear completely
		activate_region (BTL_DONE_ID, FALSE);
		SetButtonHilight(D_BTL_CONTROL, BTL_DONE_ID, FALSE);
		SetButtonFlag   (D_BTL_CONTROL, BTL_DONE_ID, D_LABEL_OFF);
		SetButtonType   (D_BTL_CONTROL, BTL_DONE_ID, BUTTON_REGION );
		fOldMoving = !fUnitsStillMoving;
	}
	
	//GEH SORRY ABOUT THIS, WILL FIX LATER
	ShowPanel(D_BTL_CONTROL);
	
	// check which cursor should be showing
	SetBattleUIMouseCursor ();
	
	// now see if the battle has been decided
	// check the home team
	BattleHealthTest((SHORT)TEST_FIELD, 0, &HomeUnitCount, &HomeUnitAlive, &HomeUnitTotal);
	HomeTotal = HomeUnitAlive;
	
	// check the away team
	BattleHealthTest((SHORT)TEST_FIELD, 1, &AwayUnitCount, &AwayUnitAlive, &AwayUnitTotal);
	AwayTotal = AwayUnitAlive;
	
	BattleHealthTest((SHORT)TEST_RESERVES, 1, &ResvCount, &ResvAlive, &ResvTotal);
	
	// if he is just completly dead, end the game
	if (AwayTotal == 0 && ResvTotal == 0)
	{
		// only the defender wins a tie
		#ifdef _WINDOWS
		if( IsMultiPlayer && units[SCENE_MGR::HomeIndex].Realm != SCENE_MGR::Aggressor )
		#endif
		// game over, man!
		BattleWin(0,0);
	}
		
	// if he is very battered compaired to me, end game
	if (ResvAlive<<2 < ResvTotal &&
		AwayUnitAlive<<2 < AwayUnitTotal )
	{
		// only the defender wins a tie
		#ifdef _WINDOWS
		if( IsMultiPlayer && units[SCENE_MGR::HomeIndex].Realm != SCENE_MGR::Aggressor )
		#endif
		// game over, man!
		BattleWin(0,0);
	}
		
	// prompt use to move out of reserves onto the field
	if(!BtlPrompted && !fBattleReserves && !(--MovementCountDown))
	{
		// do nothing yet
		if(BtlPrompted == FALSE)
		{
			BtlPrompted = TRUE;
			AddSndObj(SND_RESERVES_TO_FIELD1,SND_RESERVES_TO_FIELD_TOTAL,VOLUME_PRIORITY);
		}
	}
	
	// once the battle starts, if either side is less them 1/4 the
	// total number of men on the field for more then so many frames
	if(fBattleStarted)
	{
		if(AwayTotal == 0 && HomeTotal == 0 )
		{
			// mutual annilation
			if(++WinCount > 10 )
			{
				// only the defender wins a tie
				#ifdef _WINDOWS
				if( IsMultiPlayer && units[SCENE_MGR::HomeIndex].Realm != SCENE_MGR::Aggressor )
				{
				#endif
				
				fBattleStarted = FALSE;
				BattleWin(0,0);
				
				#ifdef _WINDOWS
				}
				#endif
			}
		}
		else
		if(AwayTotal == 0 )
		{
			// away team annilation
			if(++WinCount > 10 )
			{
				fBattleStarted = FALSE;
				BattleWin(0,0);
			}
		}
		else
		if(BtlVictoryAtHand == FALSE && AwayTotal<<3 < HomeTotal<<2)
		{
			if(++WinCount > 10 )
			{
				BtlVictoryAtHand = TRUE;
				BtlGoesWell = TRUE;
				AddSndObj(SND_VICTORY_NEAR1, SND_VICTORY_NEAR_TOTAL,VOLUME_PRIORITY);
			}
		}
		else
		if(BtlGoesWell == FALSE && AwayTotal<<1 < HomeTotal)
		{
			if(++WinCount > 10 )
			{
				BtlGoesWell = TRUE;
				AddSndObj(SND_BATTLE_GOES_WELL1,SND_BATTLE_GOES_WELL_TOTAL,VOLUME_PRIORITY);
			}
		}
		else
		if(AwayTotal<<2 < HomeTotal )
		{
			// beat them, neener, neener
			if(++WinCount > 20 )
			{
				fBattleStarted = FALSE;
				BattleWin(0,0);
			}
		}
		else
		if(BtlDefeatAtHand == FALSE && HomeTotal<<3 < AwayTotal<<2)
		{
			if(++WinCount > 10 )
			{
				BtlDefeatAtHand = TRUE;
				BtlGoesIll = TRUE;
				AddSndObj(SND_DEFEAT_IMMANENT1,SND_DEFEAT_IMMANENT_TOTAL,VOLUME_PRIORITY);
			}
		}
		else
		if(BtlGoesIll == FALSE && HomeTotal<<1 < AwayTotal)
		{
			if(++WinCount > 10 )
			{
				BtlGoesIll = TRUE;
				AddSndObj(SND_BATTLE_GOES_ILL1,SND_BATTLE_GOES_ILL_TOTAL,VOLUME_PRIORITY);
			}
		}
		else
		if(HomeTotal<<2 < AwayTotal )
		{
			if(++WinCount > 20 )
			{
				// game over, man!
				fBattleStarted = FALSE;
				BattleLoose(0,0);
			}
		}
		else
			WinCount = 0;
	}
	
	// -- check on turn duration
	if ( fBattleNextTurn )
	{
		if ( --fBattleNextTurn == 0 )
			fBattleFreezeTime = TRUE;
	}
	
	//GEH TUTORIAL
	if (fTutorialFirstBattle && 
		fDoTutWelcome &&
		++WinCount > 5)
	{
		fDoTutWelcome = FALSE;
		WinCount = 0;
		DescribeBtlWelcome();
	}
	//GEH TUTORIAL
	if (fTutorialFirstBattle && 
		!fDoTutWelcome &&
		fDoTutReserves &&
		++WinCount > 10)
	{
		fDoTutReserves = FALSE;
		WinCount = 0;
		DescribeBtlReserves();
	}
}

/* ========================================================================
   Function    - BattleCameraPosition
   Description - change the camera position mode
   Returns     -
   ======================================================================== */
void BattleCameraPosition(LONG where, LONG)
{
	sBattleCameraState = (SHORT)where;
}

/* ========================================================================
   Function    - BattleCameraHighView
   Description - move the camera to a global position
   Returns     -
   ======================================================================== */
void BattleCameraHighView(LONG, LONG)
{
	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
	if(mouse_button == 2)
	{
		SystemHelp(STR_BTLUI_HELP_COMMANDVIEW_TITLE,STR_BTLUI_HELP_COMMANDVIEW_TEXT, H_Command_View, NULL, 0);
	}
	else
	{
		// don't interupt a selection mode
		if (sBattleMode == BTLUI_MODE_MOVE ||
			sBattleMode == BTLUI_MODE_MISSILE ||
			sBattleMode == BTLUI_MODE_MAGIC
			)
			return;
			
		// set panel art
		SetButtonHilight(   D_BTL_CONTROL, BTL_VIEW_ID, TRUE);
		
		// move camera
		bBattleCameraMode = BTLUI_CAMERA_FIXED;
		
		// set up the camera rates
		SetCameraRotatePointRates( &camera,
			57, 		// RadiusRate
			8, 		// AngleRate
			250, 		// HeightRate
			-100, 	// PitchRate
			2000, 	// MinDist
			2000 		// MaxDist
			);
			
		SetCameraRotatePointTarget( &camera,
			0, 		// X
			-2000, 	// Y
			0, 		// Z
			128, 		// Angle
			2000, 	// Radius
			1);		// swoop
	
		SetCameraCurrentPosition( &camera,
			0, 		// x
			-2000, 	// y
			500, 		// z
			128, 		// angle
			-50, 		// p
			2000); 	// radius
				
		fUpdatePanels = TRUE;
	}
}

/* ========================================================================
   Function    - BattleCameraFollow
   Description - set the camera to follow a given unit
   Returns     -
   ======================================================================== */
void BattleCameraFollow(LONG, LONG)
{
	SHORT	Angle = 96;
	
	// set the count so that next frame we pick a now location
	bBattleCameraMode = BTLUI_CAMERA_FOLLOW;
	
	SetButtonHilight(   D_BTL_CONTROL, BTL_VIEW_ID, FALSE);
		
	// set up the camera rates
	SetCameraRotatePointRates( &camera,
		100,		// RADIUS_RATE
		8,			// ANGLE_RATE
		200,		// HEIGHT_RATE
		-100,		// PITCH_RATE
		1500,		// MIN_DISTANCE
		2500		// MAX_DISTANCE
		);
	
	// if already looking toward the right
	// else look toward the left
	if (camera.a < 128)
		Angle = 160;
		
	SetCameraRotatePointTarget( &camera,
		NO_CHANGE, 	// X
		NO_CHANGE, 	// Y
		NO_CHANGE, 	// Z
		Angle, 		// Angle
		2500, 		// Radius
		1);			// swoop
		
//	SetCameraCurrentPosition( &camera,
//		NO_CHANGE,	// x
//		NO_CHANGE,	// y
//		NO_CHANGE,	// z
//		Angle,		// angle
//		NO_CHANGE,	// p
//		2500);		// radius
				
}

/* #################################################################
   #################################################################
   #################################################################
   
        Battle Panel  Routines
   
   #################################################################
   #################################################################
   ################################################################# */
   
/* ========================================================================
   Function    - BattleControlGrid
   Description -
   Returns     -
   ======================================================================== */
void BattleControlGrid(LONG MenuCombo, LONG DataCombo)
{
	CHAR	buffer[CHAT_CHARS];
	SHORT	i,r,c;
	BOOL	home;
	LONG	Row, Column;
	LONG	MenuId, ButtonId;
	LONG	CalledByMouse, location;
	LONG	mx, my;
	SHORT	Health, color;
	SHORT	Movement;
	CAvatar *pAvatar;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	SPLIT_LONG(DataCombo, CalledByMouse, location);
	
	fUpdatePanels = TRUE;
	
	if (CalledByMouse)
	{
		r = location/GRID_MAX_COLS;
		c = location%GRID_MAX_COLS;
		home = CalledByMouse & 0xFF00;
	}
	else
	if (cursor_y > (480-BTLUI_PANEL_HEIGHT))
	{
		if (!WhichGridHalf(cursor_x, cursor_y, &r, &c, &home))
			return;
	}
	
	/* -----------------------------------------------------------------
	   Handle right clicks on the grid
	   ----------------------------------------------------------------- */
	if(mouse_button == 2)
	{
		switch(sBattleMode)
		{
		case BTLUI_MODE_MOVE:
		case BTLUI_MODE_MISSILE:
		case BTLUI_MODE_MAGIC:
		case BTLUI_MODE_FALLBACK:
			// right click not allowd in these modes
			AddSndObj(SND_UI_INVALID_BUTTON_CLICK ,0,VOLUME_SEVENTY);
			return;
		 	
		default:
			mouse_click = 0;
			mouse_button = 0;
			// if one of my units, select him for info display
			if(home && GetWhosWhere(r,c,WW_HOME) != fERROR)
			{
				// show battle war card
				CAvatar * const pAvatar = (CAvatar * const) BLKPTR((SHORT) GetWhosWhere(r,c,WW_HOME));
				if (IsRegentOrLieutenant(pAvatar->UnitIndex))
					StaticInventoryGUI::mfDisplay(0, pAvatar->hPlayerStats);
				else
					InitBattleCard( pAvatar->fBtlCap.UnitIndex );
			}
			else
			if(!home && GetWhosWhere(r,c,WW_AWAY) != fERROR)
			{
				// show battle war card
				CAvatar * const pAvatar = (CAvatar * const) BLKPTR((SHORT) GetWhosWhere(r,c,WW_AWAY));
				InitBattleCard( pAvatar->fBtlCap.UnitIndex );
			}
			else
			{
				// else just give help on the control grid
				SystemHelp(STR_BTLUI_HELP_CONTROL_GRID_TITLE,STR_BTLUI_HELP_CONTROL_GRID_TEXT, H_Battlefield_Grid_Interface, NULL, 0);
			}
		}
	}
	/* -----------------------------------------------------------------
	   Handle left clicks on the grid
	   ----------------------------------------------------------------- */
	else
	{
		switch(sBattleMode)
		{
			case BTLUI_MODE_MAGIC:
				if (!home)
				{
					if(battleTroops == fERROR)
						break;
					
					pAvatar = (CAvatar *)BLKPTR(battleTroops);
					
					if (BattleCurrentSpell == fERROR)
					{
						AddSndObj(SND_UI_INVALID_BUTTON_CLICK ,0,VOLUME_SEVENTY);
					}
					else
					if (BattleCurrentSpell < 6 && pAvatar->hPlayerStats != fERROR) // 0-5 magic, 6 missile, 7 move
					{
						DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(pAvatar->hPlayerStats);
						InvenIndex const sSpell = pPlayerStats->Inventory.mfGetSpellBox(BattleCurrentSpell);
					
						//GEH if (sSpell > -1)
						if (sSpell > 0)
						{
							pPlayerStats->Inventory.mfActivateSpellBox(BattleCurrentSpell);
							
							pAvatar->fBtlCap.fBtlMagicType = fBtlMagicType;
							pAvatar->fBtlCap.fBtlMagicResult = fBtlMagicResult;
							
							// -- use up this turn
							pAvatar->fBtlCap.TurnAction = FALSE;
							
							// not a selected grid
							if(!(GetButtonFlags(MenuId, ButtonId) & D_HILIGHTED))
							{
								// cant get there
								AddVoxSnd(VOX_BATTLE_DECLINE,pAvatar->ThingIndex);
								break;
							}
							
							// note: D_SANS_6 means legal enemy shot
							if(GetButtonFlags(MenuId, ButtonId) & D_SANS_6)
							{
								if(GetWhosWhere(r,c,WW_AWAY) != fERROR)
								{
									// remeber my target
									pAvatar->hEnemy = GetWhosWhere(r,c,WW_AWAY);
									
									if(pAvatar->fBtlCap.fBtlMagicType == BTLCAP_MAGIC_MISSILE)
									{
										pAvatar->fBtlCap.Shoot = BTLCAP_MISSILE;
									}
									else
									if(pAvatar->fBtlCap.fBtlMagicType == BTLCAP_MAGIC_FIRE)
									{
										pAvatar->fBtlCap.Shoot = BTLCAP_MAGIC_FIRE;
									}
									else
									if(pAvatar->fBtlCap.fBtlMagicType == BTLCAP_MAGIC_PLASMA)
									{
										pAvatar->fBtlCap.Shoot = BTLCAP_MAGIC_FIRE;
									}
									else
									if(pAvatar->fBtlCap.fBtlMagicType == BTLCAP_MAGIC_LIGHTNING)
									{
										pAvatar->fBtlCap.Shoot = BTLCAP_MAGIC_FIRE;
									}
									else
									if(pAvatar->fBtlCap.fBtlMagicType == BTLCAP_MAGIC_DESOLVE)
									{
										pAvatar->fBtlCap.Shoot = BTLCAP_MAGIC_DESOLVE;
									}
								}
								SendMagic( pAvatar );
								BattleCancelMove(0,0);
							}
							break;
						}
					}
				}
				// fall through
		
		case BTLUI_MODE_MISSILE:
		case BTLUI_MODE_FALLBACK:
		case BTLUI_MODE_MOVE:
			if(battleTroops == fERROR)
			{
				BattleCancelMove(0,0);
				break;
			}
			
			pAvatar = (CAvatar *)BLKPTR(battleTroops);
			
			// if clicked on my own square, cancel mode
			if (sBattleMode != BTLUI_MODE_MISSILE &&
				r == pAvatar->fBtlCap.Row &&
				c == pAvatar->fBtlCap.Column )
			{
				BattleCancelMove(0,0);
				break;
			}
			
			if(startLoc == -1) // from reserves
			{
				r = 0;  // only allow first row
				c = location%GRID_MAX_COLS;
				
				if (GetWhosWhere(r,c,WW_HOME) != fERROR)
				{
					// can't move to here, already occupied!
					// BattleCancelMove(0,0);
					AddSndObj(SND_UI_INVALID_BUTTON_CLICK ,0,VOLUME_SEVENTY);
					break;
				}
				
				pAvatar->fBtlCap.Row = r;
				pAvatar->fBtlCap.Column = c;
				pAvatar->fBtlCap.TargetRow = 0;
				pAvatar->fBtlCap.TargetCol = 0;
				pAvatar->fBtlCap.TargetX = BattleGridHome[r][c].x;
				pAvatar->fBtlCap.TargetY = BattleGridHome[r][c].y;
				pAvatar->fBtlCap.Rate = 40;
				pAvatar->fBtlCap.OnBtlField = TRUE;
				
				
				SetWhosWhere(r,c,WW_HOME,battleTroops);
				//hUnitInfo = (SHORT)battleTroops;
				fBattleReserves = TRUE;
				
				// change out of move troops mode
				startLoc = -2;	// mark invalid
				BattleCancelMove(0,0);
				
				if ( !pAvatar->fBtlCap.EverOnBtlField )
				{
					pAvatar->fBtlCap.EverOnBtlField = TRUE;
					BattleExpPoints( pAvatar->UnitIndex, BTL_EXP_ONFIELD, 0, FALSE );
				}
				
				// -- use up this turn
				pAvatar->fBtlCap.TurnAction = FALSE;
							
				// on out way
				AddVoxSnd(VOX_BATTLE_ACKNOWLEDGE,pAvatar->ThingIndex);
				AddSndObj( SND_BATTLE_HORN1, 0, VOLUME_EIGHTY);
			}
			else
			{
				if (startLoc < 0)	// invalid request
					BattleCancelMove(0,0);
					
				// button is hilighted if this is a legal shot
				// note: D_SANS_6 means legal enemy shot
				if (!home && 
					GetButtonFlags(MenuId, ButtonId) & D_SANS_6 &&
					BattleMissilesReady(pAvatar)
					)
				{
					r = location/GRID_MAX_COLS;
					c = location%GRID_MAX_COLS;
					if (GetWhosWhere(r,c,WW_AWAY) != fERROR)
					{
						// -- use up this turn
						pAvatar->fBtlCap.TurnAction = FALSE;
									
						// remeber my target
						pAvatar->hEnemy = GetWhosWhere(r,c,WW_AWAY);
						pAvatar->fBtlCap.MissileCount = BattleTick + BtlActionRate;
						pAvatar->fBtlCap.Shoot = BTLCAP_MISSILE;
						pAvatar->fBtlCap.Shot = BattleTick + BTL_KILL_DELAY;
						if (pAvatar->fBtlCap.HitPoints > 4)
						{
							// fake magic shot to get arrow flight
							pAvatar->fBtlCap.fBtlMagicType = BTLCAP_NOSHOT;
							pAvatar->mfCreateFireBall( GetNewId(), ARROWS, ARROW_DAMAGE);
						}
					}
					BattleCancelMove(0,0);
				}
				else
				// note: D_SANS_5 means legal move
				if(GetButtonFlags(MenuId, ButtonId) & D_SANS_5)
				{
					r = (location/5)-(startLoc/5);
					c = (location%5)-(startLoc%5);
					pAvatar->fBtlCap.TargetRow = r;
					pAvatar->fBtlCap.TargetCol = c;
					fBattleMovement = TRUE;
					
					// -- use up this turn
					pAvatar->fBtlCap.TurnAction = FALSE;
									
					// change out of move troops mode
					startLoc = -2;	// mark invalid
					BattleCancelMove(0,0);
					
					if (pAvatar->Engaged)
					{
						pAvatar->fBtlCap.HitCount = 0;
						SendFallBack ( 
							pAvatar,
							pAvatar->fBtlCap.TargetRow,
							pAvatar->fBtlCap.TargetCol);
						pAvatar->fBtlCap.mfFallBack(pAvatar->hThis);
						AddVoxSnd(VOX_BACK_TO_RESERVES,pAvatar->ThingIndex);
					}
					else
						AddVoxSnd(VOX_BATTLE_ACKNOWLEDGE,pAvatar->ThingIndex);
						
				}
				else
				// move selection to someone else
				if (GetWhosWhere(r,c,WW_HOME) != fERROR)
				{
					// change out of move troops mode
					startLoc = -2;	// mark invalid
					BattleCancelMove(0,0);
					goto TryNewMovement;
				}
				else
				// not a selected grid
				if(!(GetButtonFlags(MenuId, ButtonId) & D_HILIGHTED))
				{
					// cant get there
					AddVoxSnd(VOX_BATTLE_DECLINE,pAvatar->ThingIndex);
					break;
				}
			}
			break;
			
		// by default, we select troops for movement
TryNewMovement:		
		default:
			Row = location/GRID_MAX_COLS;
			Column = location%GRID_MAX_COLS;
			battleTroops = GetWhosWhere(Row,Column,WW_HOME);
			assert(battleTroops != 0);
			assert(battleTroops >= fERROR);
			if(battleTroops == fERROR)
				return;
				
			// if you clicked on the same troop again, don't
			// move the camera
			if (hUnitInfo != battleTroops)
			{
				hUnitInfo = battleTroops;
				BattleCameraFollow(0, 0);
			}
			
			BattlePanelMode(BUILD_LONG(D_BTL_CONTROL,0),BTLUI_MODE_UNITINFO);
				
			pAvatar = (CAvatar *)BLKPTR(battleTroops);
				
			Movement = BattleTroopInfo[pAvatar->fBtlCap.UnitIcon].Move;
			
			startLoc = location;
			
			// if he is engaged, ask if you want him to fallback one
			if(pAvatar->Engaged)
				Movement = 1;
			
			// if already moving or, can't move again
			if(pAvatar->Status == CAvatar::AI_MOVING
				|| (fBtlTurnBased && !pAvatar->fBtlCap.TurnAction ))
			{
				battleTroops = fERROR;
				AddVoxSnd(VOX_BUSY,pAvatar->ThingIndex);
				return;
			}
			// if there is somewhere to move to OR can go to the reserves
			else
			{
				// -- My apoligies for this, but it was the last minute...
				hHilightAvatar = pAvatar->hThis; 
				sHilightMenuId = MenuId; 
				sHilightRow = Row; 
				sHilightColumn = Column; 
				sHilightMovement = Movement;
				HilightTheBattleGrids(TRUE);
			}
			
			// Inquiry what he wants
			AddVoxSnd(VOX_BATTLE_INQUIRY,pAvatar->ThingIndex);
			break;
		}
	}
}
/* ========================================================================
   Function    - HilightTheBattleGrids
	Description - 
	Returns     - 
	======================================================================== */
BOOL HilightTheBattleGrids( BOOL ChangeModesAllowed )
{
	SHORT i;
	BOOL	FoundTarget = FALSE;
	
	if (hHilightAvatar == fERROR)
		return FALSE;
		
	CAvatar * const pAvatar = (CAvatar * const ) BLKPTR((SHORT) hHilightAvatar);
	
	if (pAvatar == NULL)
		return FALSE;
	
	// clear modal grid hilights
	for(i=2;i<17;i++)
	{
		ClearButtonFlag( D_BTL_CONTROL, i, D_SANS_5|D_SANS_6);
		SetButtonHilight(D_BTL_CONTROL, i, FALSE);
	}
	
	if(BattleLookAround(
		sHilightMenuId, sHilightRow, sHilightColumn, sHilightMovement,
		0,(PFGRIDTEST)HilightPossibleGrids )
		||
		sHilightRow == 0	
		)
	{
		SHORT	buttonId;
		
		if (ChangeModesAllowed)
		{
			// change to move troops mode
			sOldBattleMode = sBattleMode;
			
			// check to see if any grids show missile targets
			for (buttonId = 2; buttonId < 17; buttonId++)
			{
				if(GetButtonFlags(D_BTL_CONTROL, buttonId) & D_SANS_6)
				{
					FoundTarget = TRUE;
					break;
				}
			}
			
			if (!pAvatar->Engaged && FoundTarget && BattleMagicReady(pAvatar))
			{
				BattlePanelMode(BUILD_LONG(D_BTL_CONTROL,0),BTLUI_MODE_MAGIC);
			}
			else
			if (!pAvatar->Engaged && FoundTarget && BattleMissilesReady(pAvatar))
			{
				BattlePanelMode(BUILD_LONG(D_BTL_CONTROL,0),BTLUI_MODE_MISSILE);
			}
			else
			if (pAvatar->Engaged)
			{
				BattlePanelMode(BUILD_LONG(D_BTL_CONTROL,0),BTLUI_MODE_FALLBACK);
			}
			else
			{
				BattlePanelMode(BUILD_LONG(D_BTL_CONTROL,0),BTLUI_MODE_MOVE);
			}
		}
	}
	
	return FoundTarget;
}
/* ========================================================================
   Function    - BattleReservesGrid
   Description -
   Returns     -
   ======================================================================== */
void BattleReservesGrid(LONG MenuCombo, LONG unused)
{
	CHAR	temp[80];
	CHAR	buffer[80];
	SHORT	i,x,y,w,h;
	LONG	mx,my;
	LONG	MenuId, ButtonId;
	CAvatar *pAvatar;
	char n[256];
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	if( GetButtonPosition( MenuId, 0, &mx, &my ) == fERROR)
		return;
		
	// display info on right click
	if(mouse_button == 2)
	{
		switch(sBattleMode)
		{
		case BTLUI_MODE_STATUS:
			SystemHelp(STR_BTLUI_HELP_STATUS_TITLE,STR_BTLUI_HELP_STATUS_TEXT, H_Battle_Status, NULL, 0);
			return;
			
		case BTLUI_MODE_UNITINFO:
			SystemHelp(STR_BTLUI_HELP_UNITINFO_TITLE,STR_BTLUI_HELP_UNITINFO_TEXT, H_Unit_Types_and_Strengths, NULL, 0);
			return;
		
		case BTLUI_MODE_MAGIC:
			SystemHelp(STR_BTLUI_HELP_SEL_MAGIC_TITLE,STR_BTLUI_HELP_SEL_MAGIC_TEXT,-1, NULL, 0);
			return;
			
		case BTLUI_MODE_MISSILE:
			SystemHelp(STR_BTLUI_HELP_SEL_MISSILE_TITLE,STR_BTLUI_HELP_SEL_MISSILE_TEXT,-1, NULL, 0);
			return;
			
		case BTLUI_MODE_RESERVES:
		default:
			w = 64;	// dimension of a reserve grid
			h = 30;
			
			//GEHfor(i=0;i<MAX_RESERVES;i++)
			//GEH{
			//GEH	x = mx + ReservesGridX[i];
			//GEH	y = my + ReservesGridY[i];
			//GEH	if(cursor_x>x&&cursor_y>y&&cursor_x<(x+w)&&cursor_y<(y+h))
			//GEH		break;
			//GEH}
			
			//GEHif(i < MAX_RESERVES && Reserves[1][i] != fERROR)
			//GEH{
			//GEH	CAvatar *pAvatar = (CAvatar *) BLKPTR((SHORT) Reserves[1][i]);
			//GEH	if (pAvatar->fBtlCap.OfficerIcon >= REG_M_FTR_UNIT_ICON)
			//GEH		StaticInventoryGUI::mfDisplay(0, pAvatar->hPlayerStats);
			//GEH	else
			//GEH		InitBattleCard( pAvatar->fBtlCap.UnitIndex );
			//GEH}
			//GEHelse
			{
				SystemHelp(STR_BTLUI_HELP_RESERVES_TITLE,STR_BTLUI_HELP_RESERVES_TEXT, H_Reserves, NULL, 0);
			}
		}
	}
	else
	{
		switch(sBattleMode)
		{
		case BTLUI_MODE_UNITINFO:
		case BTLUI_MODE_STATUS:
		case BTLUI_MODE_MAGIC:
		case BTLUI_MODE_MISSILE:
		case BTLUI_MODE_MOVE:
		case BTLUI_MODE_FALLBACK:
			break;
			
		case BTLUI_MODE_RESERVES:
		default:	
			// if some troop is selected
			if(battleTroops != fERROR)
			{
				pAvatar = (CAvatar *) BLKPTR((SHORT) battleTroops);
				// if that troop started in some grid position
				if(startLoc >= 0)
				{
					SHORT r = startLoc/5;
					SHORT c = startLoc%5;
					
					// if he is not in the friendly row
					if(r != 0)
					{
						AddVoxSnd(VOX_BATTLE_DECLINE,pAvatar->ThingIndex);
						return;
					}
					
					// remove him from the grid
					SetWhosWhere(r,c,WW_HOME,fERROR);
					startLoc = -1;
				
					// can't look at him anymore
					BattleCameraHighView(0, 0);
					
					SendToReserves(pAvatar);
	
					// now place him in the reserves
					// GWP 5/18/97
					// I think this needs to be called here.
					//
					//
					SendFallBack ( 
						pAvatar,
						pAvatar->fBtlCap.TargetRow,
						pAvatar->fBtlCap.TargetCol);
					pAvatar->fBtlCap.mfFallBack(pAvatar->hThis);
					
					// GWP And this no longer needs to be called.
					// GWP BattleMoveToReservesArea(pAvatar->hThis);
					// GWP BattlePutInReserves(1, battleTroops);
					
					battleTroops = fERROR;
					// always go to reserves mode at this point
					BattlePanelMode(BUILD_LONG(D_BTL_CONTROL,0),BTLUI_MODE_RESERVES);
					AddVoxSnd(VOX_BACK_TO_RESERVES,pAvatar->ThingIndex);
				}
			}
			else
			{
				// this means we came from the reserves
				startLoc = -1;
				
				w = 64;	// dimension of a reserve grid
				h = 30;
				for(i=0;i<MAX_RESERVES;i++)
				{
					x = mx + ReservesGridX[i];
					y = my + ReservesGridY[i];
					if(cursor_x>x&&cursor_y>y&&cursor_x<(x+w)&&cursor_y<(y+h))
						break;
				}
				
				if(i==MAX_RESERVES)
					return;
				
				battleTroops = Reserves[1][i];
				assert(battleTroops != 0);
				assert(battleTroops >= fERROR);
				if(battleTroops != fERROR)
				{
					CAvatar * const pAvatar = (CAvatar * const) BLKPTR((SHORT) battleTroops);
					if(pAvatar->fBtlCap.FallBackCount)
					{
						battleTroops = fERROR; // not this guy
						
						// test morale check for next time
						if(pAvatar->fBtlCap.mfMoraleCheck(pAvatar))
							pAvatar->fBtlCap.FallBackCount = 0;
							
						AddVoxSnd(VOX_BATTLE_DECLINE,pAvatar->ThingIndex);
					}
					else
					{
						// get format string
						#ifdef BTL_CHAT
						strcpy(temp,STRMGR_GetStr(STR_CHAT_RESERVESELECT));
						// fill in data
						sprintf(buffer, temp, STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon] ));
						
						ChatLineAdd(buffer, LT_BLUE);
						#endif
						
						Reserves[1][i] = -1;
						for(i=2;i<7;i++)
						{
							// except of a impassible grids
							if(BTLUI_EXCEPT_IMPASSIBLE == BtlExceptions[0][i-2])
								continue;
								
							if(GetWhosWhere(0,i-2,WW_HOME) == -1)
							{
								SetButtonHilightArt(MenuId, i, BTLUI_HOME_HILIGHT);
								SetButtonHilight(MenuId, i, TRUE);
							}
						}
						sOldBattleMode = sBattleMode;
						BattlePanelMode(BUILD_LONG(D_BTL_CONTROL,0),BTLUI_MODE_MOVE);
						AddVoxSnd(VOX_BATTLE_INQUIRY,pAvatar->ThingIndex);
					}
				}
			}
		}
	}
}

/* #################################################################
   #################################################################
   #################################################################
   
        Paint Routines for Battle
   
   #################################################################
   #################################################################
   ################################################################# */


/* ========================================================================
   Function    - BattleControlPaint
   Description - Paint the control menu face
   Returns     -
   ======================================================================== */
void BattleControlPaint(LONG MenuCombo, LONG )
{
	SHORT	i,j;
	LONG	x,y,mx,my;
	LONG	MenuId, ButtonId;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	if( GetButtonPosition( MenuId, 0, &mx, &my ) == fERROR)
		return;
		
	// loop through the battle grid and display icons on the panel
	for(i=0;i<GRID_MAX_ROWS;i++)
	{
		// start in the first column
		// and check my row of troops
		for(j=0;j<GRID_MAX_COLS;j++)
		{
			SHORT	Who;
			
			// if someone is in this column, display them
			Who = GetWhosWhere(i,j,WW_HOME);
			if(Who != fERROR)
			{
				x = mx + (LONG)BtlGridX[j];
				y = my + (LONG)BtlGridY[i]+BTLUI_GRID_HALF;
				BattleIconPaint(Who, (SHORT)x, (SHORT)y);
			}
			Who = GetWhosWhere(i,j,WW_AWAY);
			if(Who != fERROR)
			{
				x = mx + (LONG)BtlGridX[j];
				y = my + (LONG)BtlGridY[i];
				BattleIconPaint(Who, (SHORT)x, (SHORT)y);
			}
		}
	}
	
	// paint the utility area based on the mode we are in
	switch(sBattleMode)
	{
	case BTLUI_MODE_RESERVES:
		BattleSelTroopsPaint(MenuCombo, 0);
		break;
	case BTLUI_MODE_STATUS:
		BattleStatusPaint(MenuCombo);
		break;
	case BTLUI_MODE_UNITINFO:
		BattleUnitInfoPaint(MenuCombo);
		break;
	case BTLUI_MODE_MISSILE:
		#ifdef _WINDOWS
		if (IsMultiPlayer())
			HilightTheBattleGrids(FALSE);
		#endif
		BattleMissilePaint(MenuCombo);
		#ifdef _WINDOWS
		if (IsMultiPlayer())
			fUpdatePanels = TRUE;
		#endif
		break;
	case BTLUI_MODE_MAGIC:
		#ifdef _WINDOWS
		if (IsMultiPlayer())
			HilightTheBattleGrids(FALSE);
		#endif
		BattleMagicPaint(MenuCombo);
		#ifdef _WINDOWS
		if (IsMultiPlayer())
			fUpdatePanels = TRUE;
		#endif
		break;
	case BTLUI_MODE_MOVE:
		#ifdef _WINDOWS
		if (IsMultiPlayer())
			HilightTheBattleGrids(FALSE);
		#endif
		BattleMovePaint(MenuCombo);
		#ifdef _WINDOWS
		if (IsMultiPlayer())
			fUpdatePanels = TRUE;
		#endif
		break;
	case BTLUI_MODE_FALLBACK:
		#ifdef _WINDOWS
		if (IsMultiPlayer())
			HilightTheBattleGrids(FALSE);
		#endif
		BattleFallBackPaint(MenuCombo);
		#ifdef _WINDOWS
		if (IsMultiPlayer())
			fUpdatePanels = TRUE;
		#endif
		break;
	}
	
	// now catch the exceptions
	SetExceptionGrids();
}

/* ========================================================================
   Function    - BattleIconPaint
   Description - Paint a battle grid icon
   Returns     -
   ======================================================================== */
void BattleIconPaint(SHORT hAvatar, SHORT x, SHORT y)
{
	SHORT	bar_color;
	SHORT	backg_color = GREY;
	SHORT	label_color = WHITE;
	SHORT	health;
	
	if (hAvatar == fERROR)
		return;
		
	CAvatar * const pAvatar = (CAvatar * const )BLKPTR(hAvatar);
	
	// draw his icon
	if(pAvatar->mfIsHomeTeam())
		SetRemapTable( units[SCENE_MGR::HomeIndex].Realm );
	else
		SetRemapTable( units[SCENE_MGR::AwayIndex].Realm );
	if(pAvatar->fBtlCap.hIcon != fERROR)
		DrawBitmap (x+1, y+1, pAvatar->fBtlCap.hIcon, 0, 0,  999, 999);
	ClearRemapTable();
	
	// check health percentage
	health =
		(pAvatar->fBtlCap.HitPoints * 100)
		/ pAvatar->fBtlCap.Healthy
		* 30
		/ 100;
	
	if((health*100/30) > 50)
		bar_color = GREEN;
	else
	if((health*100/30) > 25)
		bar_color = YELLOW;
	else
		bar_color = RED;
		
	// if moving, don't show his hilighted health bar
	if ( pAvatar->Status == CAvatar::AI_MOVING
			|| (fBtlTurnBased && !pAvatar->fBtlCap.TurnAction ))
	{
		bar_color -= 10;
		backg_color = BLACK;
		label_color = GREY;
	}
	else
	if (pAvatar->Status == CAvatar::AI_ATTACK)
	{
		backg_color = WHITE;
	}
	else
	if (pAvatar->mfIsHomeTeam() && 
		pAvatar->fBtlCap.OnBtlField &&
		(BattleMissilesReady(pAvatar) ||
		 BattleMagicReady(pAvatar)) )
	{
		label_color = YELLOW;
	}
	
	// show his health
	color_rect( x+19, y+8, 32, 8, backg_color);
	color_rect( x+20, y+9, health, 6, bar_color);
	
	init_gfont(FONT_SANS_5PT);
	gprint_text( x+18, y+1, STRMGR_GetStr(gsIconAbbriv[pAvatar->fBtlCap.OfficerIcon]), label_color);
}

/* ========================================================================
   Function    - BattleStatusPaint
   Description - paint the status info on the battle
   Returns     -
   ======================================================================== */
void BattleStatusPaint(LONG MenuCombo)
{
	LONG	mx,my;
	LONG	width;
	CHAR	buffer[80];
	SHORT	iBitm;
	SHORT	UnitCount, UnitAlive, UnitTotal;
	SHORT	AwayColor = RED;
	SHORT	HomeColor = LT_BLUE;
	SHORT	color;
	LONG	MenuId, ButtonId;
	CAvatar	*pAvatar;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	if( GetButtonPosition( MenuId, 0, &mx, &my ) == fERROR)
		return;
		
	/* -----------------------------------------------------------------
	   His Status
	   ----------------------------------------------------------------- */
	// his picture
	//NOTE: fix this later
	if (units[SCENE_MGR::AwayIndex].iHandle != fERROR)
	{
		pAvatar = (CAvatar *) BLKPTR(units[SCENE_MGR::AwayIndex].iHandle);
	
		sprintf(buffer, "UI\\PORTS_L\\LG%s.PCX", GAME_TTYPE::mfGetArtFileName(pAvatar->mfType()));
		iBitm = GetResourceStd (buffer, FALSE);
		SetRemapTable( units[SCENE_MGR::AwayIndex].Realm );
		DrawBitmap (BTLUI_STATUS_AWAY_PICTURE_X+mx, BTLUI_STATUS_AWAY_PICTURE_Y+my,
			iBitm, 0, 0, BTLUI_STATUS_AWAY_PICTURE_W, BTLUI_STATUS_AWAY_PICTURE_H);
		ClearRemapTable();
		
		init_gfont(FONT_TITL_10PT);
		
		// his name
		pAvatar = (CAvatar *) BLKPTR(units[SCENE_MGR::AwayIndex].iHandle);
		if (pAvatar->hPlayerStats != fERROR)
		{
			DumbAutoLockPtr<PLAYER_STATS const> pPlayerStats(pAvatar->hPlayerStats);
			sprintf(buffer, "%s", pPlayerStats->mfGetName());
			gprint_text(
				BTLUI_STATUS_AWAY_NAME_X+mx,
				BTLUI_STATUS_AWAY_NAME_Y+my,
				buffer, AwayColor );
			
		}
		#if defined(_FOREIGNVER)
		init_gfont(FONT_SANS_8PT);
		#else
		init_gfont(FONT_SANS_12PT);
		#endif
		
		// his realm
		sprintf(buffer,"%s", realm[units[SCENE_MGR::AwayIndex].Realm].mfGetName());
		print_text_centered(
			BTLUI_STATUS_AWAY_REALM_CX+mx,
			BTLUI_STATUS_AWAY_REALM_CY+my,
			buffer, AwayColor );
		
		// his units on the field
		BattleHealthTest((SHORT)TEST_FIELD, 1, &UnitCount, &UnitAlive, &UnitTotal);
		sprintf(buffer,STRMGR_GetStr(BTLUI_STATUS_FIELD), UnitCount, UnitAlive, UnitTotal);
		gprint_text( BTLUI_STATUS_AWAY_FIELD_X+mx, BTLUI_STATUS_AWAY_FIELD_Y+my,
			buffer, AwayColor );
		
		// his units in reserves
		BattleHealthTest((SHORT)TEST_RESERVES, 1, &UnitCount, &UnitAlive, &UnitTotal);
		sprintf(buffer,STRMGR_GetStr(BTLUI_STATUS_RESV), UnitCount, UnitAlive, UnitTotal);
		gprint_text( BTLUI_STATUS_AWAY_RESV_X+mx, BTLUI_STATUS_AWAY_RESV_Y+my,
			buffer, AwayColor );
		
	}
	/* -----------------------------------------------------------------
	   My status
	   ----------------------------------------------------------------- */
	// my picture
	if (units[SCENE_MGR::HomeIndex].iHandle != fERROR)
	{
		pAvatar = (CAvatar *) BLKPTR(units[SCENE_MGR::HomeIndex].iHandle);
		sprintf(buffer, "UI\\PORTS_L\\LG%s.PCX", GAME_TTYPE::mfGetArtFileName(pAvatar->mfType()));
		iBitm = GetResourceStd (buffer, FALSE);
		SetRemapTable( units[SCENE_MGR::HomeIndex].Realm );
		DrawBitmap (BTLUI_STATUS_HOME_PICTURE_X+mx, BTLUI_STATUS_HOME_PICTURE_Y+my,
			iBitm, 0, 0, BTLUI_STATUS_HOME_PICTURE_W, BTLUI_STATUS_HOME_PICTURE_H);
		ClearRemapTable();
		
		init_gfont(FONT_TITL_10PT);
		
		pAvatar = (CAvatar *) BLKPTR(units[SCENE_MGR::HomeIndex].iHandle);
		
		if (pAvatar->hPlayerStats != fERROR)
		{
			DumbAutoLockPtr<PLAYER_STATS const> pPlayerStats(pAvatar->hPlayerStats);
			
			sprintf(buffer, "%s", pPlayerStats->mfGetName());
			width = gtext_width(buffer);
			gprint_text(
				BTLUI_STATUS_HOME_NAME_RX+mx-width,
				BTLUI_STATUS_HOME_NAME_Y+my,
				buffer, HomeColor );
		}
		
		#if defined(_FOREIGNVER)
		init_gfont(FONT_SANS_8PT);
		#else
		init_gfont(FONT_SANS_12PT);
		#endif
		
		// my realm
		sprintf(buffer,"%s", realm[units[SCENE_MGR::HomeIndex].Realm].mfGetName());
		print_text_centered(
			BTLUI_STATUS_HOME_REALM_CX+mx,
			BTLUI_STATUS_HOME_REALM_CY+my,
			buffer, HomeColor );
			
		// my units on the field
		BattleHealthTest((SHORT)TEST_FIELD, 0, &UnitCount, &UnitAlive, &UnitTotal);
		sprintf(buffer,STRMGR_GetStr(BTLUI_STATUS_FIELD), UnitCount, UnitAlive, UnitTotal);
		gprint_text( BTLUI_STATUS_HOME_FIELD_X+mx, BTLUI_STATUS_HOME_FIELD_Y+my,
			buffer, HomeColor );
		
		// my units in reserves
		BattleHealthTest((SHORT)TEST_RESERVES, 0, &UnitCount, &UnitAlive, &UnitTotal);
		sprintf(buffer,STRMGR_GetStr(BTLUI_STATUS_RESV), UnitCount, UnitAlive, UnitTotal);
		gprint_text( BTLUI_STATUS_HOME_RESV_X+mx, BTLUI_STATUS_HOME_RESV_Y+my,
			buffer, HomeColor );
		
	}
}

/* ========================================================================
   Function    - BattleHealthTest
   Description - find the health of a type of troops
   Returns     -
   ======================================================================== */
void BattleHealthTest (
	SHORT TestType,
	SHORT Away,
	SHORT *UnitCount,
	SHORT *UnitAlive,
	SHORT *UnitTotal
)
{
	SHORT	i;
	SHORT	j;
	CAvatar *pAvatar;
	
	*UnitCount = 0;
	*UnitAlive = 0;
	*UnitTotal = 0;
	if( TestType == TEST_FIELD )
	{
		if(Away)
		{
			for(i=0;i<GRID_MAX_ROWS;i++)
			{
				for(j=0;j<GRID_MAX_COLS;j++)
				{
					if(GetWhosWhere(i,j,WW_AWAY) != fERROR)
					{
						*UnitCount += 1;
						pAvatar = (CAvatar *) BLKPTR(GetWhosWhere(i,j,WW_AWAY));
						*UnitAlive += pAvatar->fBtlCap.HitPoints;
						*UnitTotal += pAvatar->fBtlCap.Healthy;
					}
				}
			}
		}
		else
		{
			for(i=0;i<GRID_MAX_ROWS;i++)
			{
				for(j=0;j<GRID_MAX_COLS;j++)
				{
					if(GetWhosWhere(i,j,WW_HOME) != fERROR)
					{
						*UnitCount += 1;
						pAvatar = (CAvatar *) BLKPTR(GetWhosWhere(i,j,WW_HOME));
						*UnitAlive += pAvatar->fBtlCap.HitPoints;
						*UnitTotal += pAvatar->fBtlCap.Healthy;
					}
				}
			}
		}
	}
	else
	{
		if(Away)
		{
			for(i=0;i<MAX_RESERVES;i++)
			{
				if(Reserves[0][i] != fERROR)
				{
					*UnitCount += 1;
					pAvatar = (CAvatar *) BLKPTR(Reserves[0][i]);
					*UnitAlive += pAvatar->fBtlCap.HitPoints;
					*UnitTotal += pAvatar->fBtlCap.Healthy;
				}
			}
		}
		else
		{
			for(i=0;i<MAX_RESERVES;i++)
			{
				if(Reserves[1][i] != fERROR)
				{
					*UnitCount += 1;
					pAvatar = (CAvatar *) BLKPTR(Reserves[1][i]);
					*UnitAlive += pAvatar->fBtlCap.HitPoints;
					*UnitTotal += pAvatar->fBtlCap.Healthy;
				}
			}
		}
	}
}

/* ========================================================================
   Function    - UnitInfoPaint
   Description - paint the unit info
   Returns     -
   ======================================================================== */
void BattleUnitInfoPaint(LONG MenuCombo)
{
	LONG		mx,my;
	LONG		tx,ty;
	SHORT		i,j;
	SHORT		w,h;
	SHORT		W,H;
	SHORT		type;
	CHAR		temp[80];
	CHAR		buffer[80];
	SHORT		Health;
	SHORT		color;
	SHORT		r,c;
	LONG		MenuId, ButtonId;
	SHORT		iBitm;
	BITMPTR		pBitm;
	SHORT		scale;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	if( GetButtonPosition( MenuId, 0, &mx, &my ) == fERROR)
		return;
		
	if(hUnitInfo == fERROR)
		return;
	
	CAvatar * const pAvatar = (CAvatar * const)BLKPTR(hUnitInfo);
			
	/* -----------------------------------------------------------------
	   Paint my picture
	   ----------------------------------------------------------------- */
	
	iBitm = mythings[pAvatar->ThingIndex].iBitm;
	if(iBitm != fERROR)
	{
		pBitm = (BITMPTR) BLKPTR(iBitm);
		// note: bitmap w and h are switched, because the art is rotated
		H = h = pBitm->w;
		W = w = pBitm->h;
		if (h > 0 && w > 0)
		{
			if(w > BTLUI_UNITINFO_PICTURE_W)
			{
				h = h * BTLUI_UNITINFO_PICTURE_W / w;
				w = BTLUI_UNITINFO_PICTURE_W;
			}
			if(h > BTLUI_UNITINFO_PICTURE_H)
			{
				w = w * BTLUI_UNITINFO_PICTURE_H / h;
				h = BTLUI_UNITINFO_PICTURE_H;
			}
			
			if(abs(w-W) > abs(h-H))
			{
				scale = (H<<8)/h;
			}
			else
			{
				scale = (W<<8)/w;
			}
			
			//scale = 1<<8;
			
			SetRemapTable( units[pAvatar->UnitIndex].Realm );
			ScaleBitmap (
				BTLUI_UNITINFO_PICTURE_X+mx+(BTLUI_UNITINFO_PICTURE_W/2),
				BTLUI_UNITINFO_PICTURE_Y+my,
				iBitm, 0, 0,
				BTLUI_UNITINFO_PICTURE_W,
				-BTLUI_UNITINFO_PICTURE_H,
				scale);
			ClearRemapTable();
		}
	}
	
	/* -----------------------------------------------------------------
	   //GEH Place abbriviated troop name up
	   Place full troop name up
	   ----------------------------------------------------------------- */
	init_gfont(FONT_SANS_16PT);
	
	//sprintf(buffer,"%s", STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.UnitIcon]));
	sprintf(buffer,"%s", STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon]));
	gprint_text( BTLUI_UNITINFO_NAME_X+mx, BTLUI_UNITINFO_NAME_Y+my,
		buffer, LT_BLUE );
	
	/* -----------------------------------------------------------------
	   put up my troop health bar
	   ----------------------------------------------------------------- */

	// his health bar
	Health = (pAvatar->fBtlCap.HitPoints * 100)
		/ pAvatar->fBtlCap.Healthy
		* BTLUI_UNITINFO_BAR_W
		/ 100;
	
	// check health percentage
	if(Health > 50)
		color = GREEN;
	else
	if(Health > 25)
		color = YELLOW;
	else
		color = RED;
		
	// show his health
	color_rect(
		BTLUI_UNITINFO_BAR_X+mx,
		BTLUI_UNITINFO_BAR_Y+my,
		BTLUI_UNITINFO_BAR_W+4,
		BTLUI_UNITINFO_BAR_H,
		GREY);
	color_rect(
		BTLUI_UNITINFO_BAR_X+mx+2,
		BTLUI_UNITINFO_BAR_Y+my+2,
		Health,
		BTLUI_UNITINFO_BAR_H-4,
		color);
	
	/* -----------------------------------------------------------------
	   put up my troop health numerically
	   ----------------------------------------------------------------- */

	init_gfont(FONT_SANS_12PT);
	
	sprintf( buffer, STRMGR_GetStr(BTLUI_UNITINFO_HEALTH),
		pAvatar->fBtlCap.HitPoints, pAvatar->fBtlCap.Healthy);
	gprint_text( BTLUI_UNITINFO_HEALTH_X+mx, BTLUI_UNITINFO_HEALTH_Y+my,
		buffer, LT_BLUE );
	
	/* -----------------------------------------------------------------
	   put up my current actions and engagment
	   ----------------------------------------------------------------- */
	
	strcpy(temp, STRMGR_GetStr(BTLUI_UNITINFO_STATUS));
	sprintf( buffer, temp, STRMGR_GetStr(StatusText[pAvatar->Status]));
	gprint_text( BTLUI_UNITINFO_STATUS_X+mx, BTLUI_UNITINFO_STATUS_Y+my,
		buffer, LT_BLUE );
	
	/* -----------------------------------------------------------------
	   put up my unit type info
	   ----------------------------------------------------------------- */
	type = pAvatar->fBtlCap.OfficerIcon;
	
	tx = BTLUI_UNITINFO_INFO_X1;
	ty = BTLUI_UNITINFO_INFO_Y1;
	
	/* Move		*/
	sprintf( buffer, STRMGR_GetStr(BTLUI_UNITINFO_INFO_MOVE), GetBattleMove(type));
	gprint_text( tx+mx, ty+my, buffer, LT_BLUE );
	
	/* Defend	*/
	ty += BTLUI_UNITINFO_INFO_H;
	sprintf( buffer, STRMGR_GetStr(BTLUI_UNITINFO_INFO_DEFEND), GetBattleDefense(type));
	gprint_text( tx+mx, ty+my, buffer, LT_BLUE );
	
	/* Melee	*/
	ty += BTLUI_UNITINFO_INFO_H;
	sprintf( buffer, STRMGR_GetStr(BTLUI_UNITINFO_INFO_MELEE), GetBattleMelee(type));
	gprint_text( tx+mx, ty+my, buffer, LT_BLUE );
	
	tx = BTLUI_UNITINFO_INFO_X2;
	ty = BTLUI_UNITINFO_INFO_Y2;
	
	/* Charge	*/
	//ty += BTLUI_UNITINFO_INFO_H;
	sprintf( buffer, STRMGR_GetStr(BTLUI_UNITINFO_INFO_CHARGE), GetBattleCharge(type));
	gprint_text( tx+mx, ty+my, buffer, LT_BLUE );
	
	/* Missile	*/
	ty += BTLUI_UNITINFO_INFO_H;
	sprintf( buffer, STRMGR_GetStr(BTLUI_UNITINFO_INFO_MISSILE), GetBattleMissile(type));
	gprint_text( tx+mx, ty+my, buffer, LT_BLUE );

}		

/* ========================================================================
   Function    - BattleMagicPaint
   Description - paint the select magic questions
   Returns     -
   ======================================================================== */
void BattleMagicPaint(LONG MenuCombo)
{
	LONG	mx,my;
	LONG	MenuId, ButtonId;
	CHAR   	buffer[80];
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	if( GetButtonPosition( MenuId, 0, &mx, &my ) == fERROR)
		return;
		
	init_gfont(FONT_SANS_12PT);
	
	/* -----------------------------------------------------------------
	   spell list
	   ----------------------------------------------------------------- */
	strncpy(buffer, STRMGR_GetStr(BTLUI_MAGIC_TEXT),sizeof(buffer));
	print_text_centered(mx+BTLUI_TEXT_CX,my+15,buffer,31);
	
	ClearMagicRegions();
	SetMagicRegions();
	
	init_gfont(FONT_SANS_8PT);
	
 	if (battleTroops != fERROR)
 	{
	 	CAvatar * const pAvatar = (CAvatar * const )BLKPTR(battleTroops);
	 	SHORT	i;
	 	
	 	// add a text to select movement
	 	for (i=0;i<15;i++)
	 	{
			if (GetButtonFlags(D_BTL_CONTROL, i+2) & D_SANS_5)
			{
				strncpy(buffer, STRMGR_GetStr(BTLUI_MISSILE_TEXT1),sizeof(buffer));
				print_text_centered(mx+BTLUI_TEXT_CX,my+80,buffer,31);
	 			break;
			}
	 	}
	 	
	 	// add a text to select movement
	 	for (i=0;i<15;i++)
	 	{
			if (GetButtonFlags(D_BTL_CONTROL, i+2) & D_SANS_6)
			{
				strncpy(buffer, STRMGR_GetStr(BTLUI_MOVE_TEXT1),sizeof(buffer));
				print_text_centered(mx+BTLUI_TEXT_CX,my+90,buffer,31);
	 			break;
			}
	 	}
	 	
 	}
}

/* ========================================================================
   Function    - BattleMissilePaint
   Description - paint the select magic questions
   Returns     -
   ======================================================================== */
void BattleMissilePaint(LONG MenuCombo)
{
	LONG	mx,my;
	LONG	MenuId, ButtonId;
	CHAR   	buffer[80];
	BOOL	fMoves = FALSE;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	if( GetButtonPosition( MenuId, 0, &mx, &my ) == fERROR)
		return;
		
	init_gfont(FONT_SANS_12PT);
	
	strncpy(buffer, STRMGR_GetStr(BTLUI_MISSILE_TITLE),sizeof(buffer));
	print_text_centered(mx+BTLUI_TEXT_CX,my+15,buffer,31);
	
	/* -----------------------------------------------------------------
	   list possible actions
	   ----------------------------------------------------------------- */
 	if (battleTroops != fERROR)
 	{
	 	CAvatar * const pAvatar = (CAvatar * const)BLKPTR(battleTroops);
	 	SHORT	i;
	 	
	 	// add a text to select movement
	 	for (i=0;i<15;i++)
	 	{
			if (GetButtonFlags(D_BTL_CONTROL, i+2) & D_SANS_6)
			{
				strncpy(buffer, STRMGR_GetStr(BTLUI_MISSILE_TEXT1),sizeof(buffer));
				print_text_centered(mx+BTLUI_TEXT_CX,my+30,buffer,31);
				fMoves = TRUE;
	 			break;
			}
	 	}
	 	
	 	// add a text to select movement
	 	for (i=0;i<15;i++)
	 	{
			if (GetButtonFlags(D_BTL_CONTROL, i+2) & D_SANS_5)
			{
				strncpy(buffer, STRMGR_GetStr(BTLUI_MOVE_TEXT1),sizeof(buffer));
				print_text_centered(mx+BTLUI_TEXT_CX,my+50,buffer,31);
				fMoves = TRUE;
	 			break;
			}
	 	}
	 	
 	}
 	
 	if (!fMoves)
 	{
		strncpy(buffer, STRMGR_GetStr(BTLUI_NO_MOVES),sizeof(buffer));
		print_text_centered(mx+BTLUI_TEXT_CX,my+40,buffer,31);
 	}
}

/* ========================================================================
   Function    - BattleFallBackPaint
   Description - paint the fallback text
   Returns     -
   ======================================================================== */
void BattleFallBackPaint(LONG MenuCombo)
{
	LONG	mx,my;
	LONG	MenuId, ButtonId;
	CHAR   	buffer[80];
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	if( GetButtonPosition( MenuId, 0, &mx, &my ) == fERROR)
		return;
		
	init_gfont(FONT_SANS_12PT);
	
	/* -----------------------------------------------------------------
	   His Status
	   ----------------------------------------------------------------- */
	strncpy(buffer, STRMGR_GetStr(BTLUI_FALLBACK_TEXT1),sizeof(buffer));
	print_text_centered( mx+BTLUI_TEXT_CX,my+30, buffer, 31);
	
	strncpy(buffer, STRMGR_GetStr(BTLUI_FALLBACK_TEXT2),sizeof(buffer));
	print_text_centered( mx+BTLUI_TEXT_CX,my+50, buffer, 31);
	
}

/* ========================================================================
   Function    - BattleMovePaint
   Description - paint the select move target text
   Returns     -
   ======================================================================== */
void BattleMovePaint(LONG MenuCombo)
{
	LONG	mx,my;
	LONG	MenuId, ButtonId;
	CHAR   	buffer[80];
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	if( GetButtonPosition( MenuId, 0, &mx, &my ) == fERROR)
		return;
		
	init_gfont(FONT_SANS_12PT);
	
	strncpy(buffer, STRMGR_GetStr(BTLUI_MOVE_TITLE),sizeof(buffer));
	print_text_centered(mx+BTLUI_TEXT_CX,my+15,buffer,31);
	
	/* -----------------------------------------------------------------
	   His Status
	   ----------------------------------------------------------------- */
	if(startLoc == -1) // from reserves
	{
		strncpy(buffer, STRMGR_GetStr(BTLUI_MOVE_TEXT1),sizeof(buffer));
		print_text_centered(mx+BTLUI_TEXT_CX,my+30,buffer,31);
	}
	else
 	if (battleTroops != fERROR)
 	{
	 	CAvatar * const pAvatar = (CAvatar * const)BLKPTR(battleTroops);
	 	SHORT	i;
	 	
	 	// add a text to select movement
	 	for (i=0;i<15;i++)
	 	{
			if (GetButtonFlags(D_BTL_CONTROL, i+2) & D_SANS_5)
			{
				strncpy(buffer, STRMGR_GetStr(BTLUI_MOVE_TEXT1),sizeof(buffer));
				print_text_centered(mx+BTLUI_TEXT_CX,my+30,buffer,31);
	 			break;
			}
	 	}
	 	
 	}
}

/* ========================================================================
   Function    - BattleSelTroopsPaint
   Description - Paint the sel troops menu face
   Returns     -
   ======================================================================== */
void BattleSelTroopsPaint(LONG MenuCombo, LONG unused)
{
	SHORT	i,j;
	LONG	x,y,mx,my;
	LONG	MenuId, ButtonId;
	CHAR	buffer[80];
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	if( GetButtonPosition( MenuId, 0, &mx, &my ) == fERROR)
		return;
		
	// place up a title
	init_gfont(FONT_SANS_16PT);
	strncpy(buffer,STRMGR_GetStr(RESV_TITLE), sizeof(buffer));
	print_text_centered(
		mx+RESV_TITLE_X+RESV_TITLE_W/2,
		my+RESV_TITLE_Y+RESV_TITLE_H,
		buffer,
		31);
	
	// loop through the battle grid and display icons on the panel
	for(i=0;i<MAX_RESERVES;i++)
	{
		SHORT	Who = Reserves[1][i];
		// if someone is in this column, display them
		if(Who != fERROR)
		{
			x = mx + ReservesGridX[i];
			y = my + ReservesGridY[i];
			BattleIconPaint(Who, x, y);
		}
	}
}

/* #################################################################
   #################################################################
   #################################################################
   
        Utility Routines for Battle
   
   #################################################################
   #################################################################
   ################################################################# */

/* ========================================================================
   Function    - BattlePutInReserves
   Description - put this avatar in the next availible reserves slot
   Returns     - slot number
   ======================================================================== */
SHORT BattlePutInReserves(BOOL HomeTeam, SHORT hAvatar)
{
	SHORT i;
	for(i=0;i<MAX_RESERVES;i++)
	{
		if(Reserves[HomeTeam][i] == -1)
		{
			Reserves[HomeTeam][i] = hAvatar;
			return i;
		}
	}
	
	return -1;
}

/* ========================================================================
   Function    - BattleLookAround
   Description - check all grids within Movement distance
   Returns     - result of pfGridFunc
   ======================================================================== */
SHORT BattleLookAround(
	LONG		MenuId,
	SHORT		Row,
	SHORT		Column,
	SHORT		Movement,
	SHORT		,
	PFGRIDTEST	pfGridTest
)
{
	SHORT	i,j,NumTests,Mask;
	SHORT	R,C;
	BOOL	fRetVal = FALSE;
	BOOL	fTest;
	BOOL	fCont;
	
	//GEH forgive the HACKyness of this code, I will try to clean
	//    it up a little later on...
	
	if(Row < 0 || Row > 2)
		return FALSE;
		
	if(Column < 0 || Row > 4)
		return FALSE;
		
	/* -----------------------------------------------------------------
	   Movement of 0 is a special case to test the grid I'm in
	   ----------------------------------------------------------------- */
	if(BTLUI_EXCEPT_IMPASSIBLE == BtlExceptions[Row][Column])
		return fRetVal;
	
	(*pfGridTest)(MenuId,Row,Column,0,&fTest);
	if( fTest )
		fRetVal = TRUE;
		
	/* -----------------------------------------------------------------
	   Test up and left
	   ----------------------------------------------------------------- */
	// Movement paths tested
	NumTests = 0x0001 << Movement;
	
	for(i=0;i<NumTests;i++)
	{
		R = Row;
		C = Column;
		Mask = 0x0001;
		for(j=0;j<Movement;j++)
		{
			if((Mask & i) == 0)	// move up
			{
				R++;
				if(R >= GRID_MAX_ROWS)
					break; // this path is invalid
			}
			else				// move left
			{
				C--;
				if(C < 0)
					break; // this path is invalid
			}
			
			// end of a valid path
			if(BTLUI_EXCEPT_IMPASSIBLE == BtlExceptions[R][C])
				break;
				
			// call test proc
			fCont = (*pfGridTest)(MenuId,R,C,j+1,&fTest);
			
			if(fTest)
				fRetVal = TRUE;	// found at least one good path
				
			// if func returns false, stop checking this quad
			if(!fCont)
				break;
			
			Mask <<= 1; // shift to test next bit
		}
	}
	/* -----------------------------------------------------------------
	   Test up and right
	   ----------------------------------------------------------------- */
	// Movement paths tested
	NumTests = 0x0001 << Movement;
	
	for(i=0;i<NumTests;i++)
	{
		R = Row;
		C = Column;
		Mask = 0x0001;
		for(j=0;j<Movement;j++)
		{
			if((Mask & i) == 0)	// move up
			{
				R++;
				if(R >= GRID_MAX_ROWS)
					break; // this path is invalid
			}
			else				// move right
			{
				C++;
				if(C >= GRID_MAX_COLS)
					break; // this path is invalid
			}
			
			// end of a valid path
			if(BTLUI_EXCEPT_IMPASSIBLE == BtlExceptions[R][C])
				break;
				
			// call test proc
			fCont = (*pfGridTest)(MenuId,R,C,j+1,&fTest);
			
			if(fTest)
				fRetVal = TRUE;	// found at least one good path
				
			// if func returns false, stop checking this quad
			if(!fCont)
				break;
			
			Mask <<= 1; // shift to test next bit
		}
	}
	/* -----------------------------------------------------------------
	   Test down and left
	   ----------------------------------------------------------------- */
	// Movement paths tested
	NumTests = 0x0001 << Movement;
	
	for(i=0;i<NumTests;i++)
	{
		R = Row;
		C = Column;
		Mask = 0x0001;
		for(j=0;j<Movement;j++)
		{
			if((Mask & i) == 0)	// move down
			{
				R--;
				if(R < 0)
					break; // this path is invalid
			}
			else				// move left
			{
				C--;
				if(C < 0)
					break; // this path is invalid
			}
			
			// end of a valid path
			if(BTLUI_EXCEPT_IMPASSIBLE == BtlExceptions[R][C])
				break;
				
			// call test proc
			fCont = (*pfGridTest)(MenuId,R,C,j+1,&fTest);
			
			if(fTest)
				fRetVal = TRUE;	// found at least one good path
				
			// if func returns false, stop checking this quad
			if(!fCont)
				break;
			
			Mask <<= 1; // shift to test next bit
		}
	}
	/* -----------------------------------------------------------------
	   Test down and right
	   ----------------------------------------------------------------- */
	// Movement paths tested
	NumTests = 0x0001 << Movement;
	
	for(i=0;i<NumTests;i++)
	{
		R = Row;
		C = Column;
		Mask = 0x0001;
		for(j=0;j<Movement;j++)
		{
			if((Mask & i) == 0)	// move down
			{
				R--;
				if(R < 0)
					break; // this path is invalid
			}
			else				// move right
			{
				C++;
				if(C >= GRID_MAX_COLS)
					break; // this path is invalid
			}
			
			// end of a valid path
			if(BTLUI_EXCEPT_IMPASSIBLE == BtlExceptions[R][C])
				break;
				
			// call test proc
			fCont = (*pfGridTest)(MenuId,R,C,j+1,&fTest);
			
			if(fTest)
				fRetVal = TRUE;	// found at least one good path
				
			// if func returns false, stop checking this quad
			if(!fCont)
				break;
			
			Mask <<= 1; // shift to test next bit
		}
	}
	
	return (SHORT)fRetVal;
}

/* ========================================================================
   Function    - HilightPossibleGrids
   Description - turn on the light for a grid
   Returns     - SHORT 1 if move valid and grid turned on
   ======================================================================== */
SHORT HilightPossibleGrids(
	LONG	MenuId,		// menu id of battle panel
	LONG	R,			// row
	LONG	C,			// column
	LONG	Distance,	// how far we've moved
	BOOL	*fTest
)
{
	const SHORT	l = (R * GRID_MAX_COLS) + C;
	
	// assume false
	SHORT	retval = 0;
	*fTest = FALSE;
	
	if (battleTroops == fERROR)
		return FALSE;
	
	// except of a impassible grids
	if (BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[R][C])
	{
		CAvatar * const pAvatar = (CAvatar * const)BLKPTR(battleTroops);
	
		// if engaged, can't move forward
		if (pAvatar->Engaged && pAvatar->fBtlCap.Row < R )
		{
			*fTest = FALSE;
			return FALSE;
		}
		
		// if no one from my army is in this square, hilight
		if (GetWhosWhere(R,C,WW_HOME) == fERROR)
		{
			retval = 1;
			*fTest = TRUE;
		}
		
		// if some one from enemy side in this grid, show that
		if (!pAvatar->Engaged && (Distance <= 1) && (GetWhosWhere(R,C,WW_AWAY) != fERROR))
		{
			if (BattleMissilesReady(pAvatar) ||
				BattleMagicReady(pAvatar) )
			{
				retval |= 2;
				*fTest = TRUE;
			}
		}
		
		// set the proper hilight art
		if (retval == 1)
		{
			if (!(GetButtonFlags(MenuId, l+2) & D_HILIGHTED))
			{
				SetButtonHilightArt ( MenuId, l+2, BTLUI_HOME_HILIGHT);
				SetButtonHilight( MenuId, l+2, TRUE);
				ClearButtonFlag( MenuId, l+2, D_SANS_5|D_SANS_6);
				SetButtonFlag( MenuId, l+2, D_SANS_5); // val=1
			}
			return TRUE;	// continue on
		}
		else
		if (retval == 2)
		{
			if (!(GetButtonFlags(MenuId, l+2) & D_HILIGHTED))
			{
				SetButtonHilightArt ( MenuId, l+2, BTLUI_AWAY_HILIGHT);
				SetButtonHilight( MenuId, l+2, TRUE);
				ClearButtonFlag( MenuId, l+2, D_SANS_5|D_SANS_6);
				SetButtonFlag( MenuId, l+2, D_SANS_6); // val=2
			}
			return FALSE;	// end of valid path
		}
		else
		if (retval == 3)
		{
			if (!(GetButtonFlags(MenuId, l+2) & D_HILIGHTED))
			{
				SetButtonHilightArt ( MenuId, l+2, BTLUI_BOTH_HILIGHT);
				SetButtonHilight( MenuId, l+2, TRUE);
				ClearButtonFlag( MenuId, l+2, D_SANS_5|D_SANS_6);
				SetButtonFlag( MenuId, l+2, D_SANS_5|D_SANS_6); // val=3
			}
			return FALSE;	// end of valid path
		}
		else
			return FALSE;	// end of valid path
	}
	else
		return FALSE;	// exception halts movement along this path
}   			

/* ========================================================================
   Function    - CheckMissileRange
   Description - check to see if the missile button should be on or off
   Returns     -
   ======================================================================== */
void CheckMissileRange()
{
	SHORT	R,C;
	CHAR	temp[80];
	CHAR	buffer[80];
	
	for(R=0;R<GRID_MAX_ROWS;R++)
	{
		for(C=0;C<GRID_MAX_COLS;C++)
		{
			if(GetWhosWhere(R,C,WW_HOME) != fERROR)
			{
				CAvatar * const pAvatar = (CAvatar * const)BLKPTR(GetWhosWhere(R,C,WW_HOME));
				if( GetBattleMissile(pAvatar->fBtlCap.OfficerIcon)
					&& pAvatar->Status == CAvatar::AI_SEARCH
					&& pAvatar->fBtlCap.HasSpoken == FALSE)
				{
					// must have elapsed enough time since last shot
					if (!BattleMissilesReady(pAvatar))
						continue;
				
					if(CheckMissileRangeSub(pAvatar->fBtlCap.Row, pAvatar->fBtlCap.Column))
					{
						pAvatar->fBtlCap.HasSpoken = TRUE;
						if(pAvatar->mfIsHomeTeam())
							AddVoxSnd(VOX_ARCHERS_READY,pAvatar->ThingIndex);
						
						fUpdatePanels = TRUE;
						
						// fill in data
						#ifdef BTL_CHAT
						strcpy(temp,STRMGR_GetStr(STR_CHAT_ARCHERS_READY));
						sprintf(buffer, temp,
							STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon]),
							pAvatar->fBtlCap.Row,
							pAvatar->fBtlCap.Column
							);
						ChatLineAdd(buffer, LT_BLUE);
						#endif
						
						//GEH TUTORIAL
						if (fTutorialFirstBattle && 
							fDoTutArchers )
						{
							fDoTutArchers = FALSE;
							//SystemHelp("TUTORIAL TEST", "One of your missile units is ready. Click on them and then the red hilighted square", -1, NULL, 0);
							DescribeBtlArchers();
						}
					}
				}
			}
		}
	}
}

/* ========================================================================
   Function    - BattleMissilesReady
   Description - 
   Returns     - 
   ======================================================================== */
BOOL BattleMissilesReady(CAvatar *pAvatar)
{   	
	SHORT	buttonId;
	
	if (GetBattleMissile(pAvatar->fBtlCap.OfficerIcon) && 
		pAvatar->fBtlCap.MissileCount < BattleTick )
	{
		return TRUE;
	}
	
	return FALSE;
}

/* ========================================================================
   Function    - BattleMagicReady
   Description - 
   Returns     - 
   ======================================================================== */
BOOL BattleMagicReady(CAvatar *pAvatar)
{   	
	if (pAvatar->hPlayerStats == fERROR)
		return FALSE;
	
	DumbHandlePtr<PLAYER_STATS const> const dhPlayerStats(pAvatar->hPlayerStats);
	
	// -- let's let anyone with stuff in their hotbox give it a shot
	// -- if (dhPlayerStats->mfIsMagicUser() && 
	// -- 	pAvatar->fBtlCap.MagicCount < BattleTick)
	if ( pAvatar->fBtlCap.MagicCount < BattleTick )
	{
		SHORT i;
		// scan hot boxes for a spell
		for (i=0;i<6;i++)
		{
			InvenIndex const sSpell = dhPlayerStats->Inventory.mfGetSpellBox(i);
			if (sSpell > -1)
			{
				return TRUE;
			}
		}
	}
	
	return FALSE;
}

/* ========================================================================
   Function    - CheckMagicRange
   Description - check to see if the magic button should be on or off
   Returns     -
   ======================================================================== */
void CheckMagicRange()
{
	SHORT	R,C;
	CHAR	temp[80];
	CHAR	buffer[80];
	
	for(R=0;R<GRID_MAX_ROWS;R++)
	{
		for(C=0;C<GRID_MAX_COLS;C++)
		{
			if(GetWhosWhere(R,C,WW_HOME) == fERROR)
				continue;
			
			CAvatar * const pAvatar = (CAvatar * const )BLKPTR(GetWhosWhere(R,C,WW_HOME));
			
			if (pAvatar->hPlayerStats == fERROR)
				continue;
			
			DumbAutoLockPtr<PLAYER_STATS const> const dhPlayerStats(pAvatar->hPlayerStats);
			
			if( dhPlayerStats->mfIsMagicUser()				// GWP GetBattleMagic(pAvatar->fBtlCap.OfficerIcon)
				&& pAvatar->Status == CAvatar::AI_SEARCH
				&& pAvatar->fBtlCap.HasSpoken == FALSE)
			{
				// must have elapsed enough time since last shot
				if(pAvatar->fBtlCap.MagicCount > BattleTick)
					continue;
			
				if(CheckMissileRangeSub(pAvatar->fBtlCap.Row, pAvatar->fBtlCap.Column))
				{
					pAvatar->fBtlCap.HasSpoken = TRUE;
					// GWP if(1 == GetBattleMagic(pAvatar->fBtlCap.OfficerIcon))
					if (dhPlayerStats->mfIsType(PLAYER_CLASS_INFO::WIZARD))
						AddVoxSnd(VOX_WIZARD_READY,pAvatar->ThingIndex);
					else
						AddVoxSnd(VOX_PRIEST_READY,pAvatar->ThingIndex);
						
					// fill in data
					#ifdef BTL_CHAT
					strcpy(temp,STRMGR_GetStr(STR_CHAT_MAGIC_READY));
					sprintf(buffer, temp,
						STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon]),
						pAvatar->fBtlCap.Row,
						pAvatar->fBtlCap.Column
						);
					ChatLineAdd(buffer, LT_BLUE);
					#endif
				}
			}
		}
	}
}

/* ========================================================================
   Function    - CheckMissileRangeSub
   Description -
   Returns     -
   ======================================================================== */
BOOL CheckMissileRangeSub(SHORT R, SHORT C)
{
	BOOL fRange = FALSE;
	
	// except of a impassible grids
	if (R+1 < GRID_MAX_ROWS &&
		BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[R+1][C] &&
		GetWhosWhere(R+1,C,WW_AWAY) != fERROR )
			fRange = TRUE;
	else
	if (R-1 >= 0 &&
		BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[R-1][C] &&
		GetWhosWhere(R-1,C,WW_AWAY) != fERROR )
			fRange = TRUE;
	else
	if (C+1 < GRID_MAX_COLS &&
		BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[R][C+1] &&
		GetWhosWhere(R,C+1,WW_AWAY) != fERROR )
			fRange = TRUE;
	else
	if (C-1 > 0 &&
		BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[R][C-1] &&
		GetWhosWhere(R,C-1,WW_AWAY) != fERROR )
			fRange = TRUE;
	
	return fRange;
}
			
/* ========================================================================
   Function    - BattleUpdateTime
   Description - 
   Returns     - 
   ======================================================================== */
void BattleUpdateTime(void)
{
	static LONG prevTime = SCENE_MGR::gTick;
	LONG CurrentTime = SCENE_MGR::gTick;
	
	if (!fBattleFreezeTime)
	{
		BattleTick += CurrentTime - prevTime;
	}

	prevTime = CurrentTime;
}

/* ========================================================================
   Function    - HilightUnit
   Description - hilight the square of the hMissile unit
   Returns     -
   ======================================================================== */
void HilightUnit(void)
{
	if (hMissile == fERROR)
		return;
		
	CAvatar * const pAvatar = (CAvatar * const)BLKPTR(hMissile);
	
	LONG const l = (pAvatar->fBtlCap.Row * GRID_MAX_COLS) + pAvatar->fBtlCap.Column;
	SetButtonHilightArt ( D_BTL_CONTROL, l+2, BTLUI_HOME_HILIGHT);
	SetButtonHilight( D_BTL_CONTROL, l+2, TRUE);
	fUpdatePanels = TRUE;
}

/* ========================================================================
   Function    - DecreaseBattleScreen
   Description - Reduce the viewing area.
   Returns     -
   ======================================================================== */
void DecreaseBattleScreen(LONG, LONG)
{
	MARGIN_CONTROL::mfReduceScreenSize(0, 0);
}
/* ========================================================================
   Function    - IncreaseBattleScreen
   Description - Increase the viewing area.
   Returns     -
   ======================================================================== */
void IncreaseBattleScreen(LONG, LONG)
{
	MARGIN_CONTROL::mfIncreaseScreenSize(0, 0);
}

/* ========================================================================
   Function    - SetExceptionGrids
   Description - set hilight on the exceptions
   Returns     -
   ======================================================================== */
void SetExceptionGrids(void)
{
	SHORT i;
	
	for(i=2;i<17;i++)
	{
		const SHORT r = (i-2)/GRID_MAX_COLS;
		const SHORT c = (i-2)%GRID_MAX_COLS;
		
		if(BTLUI_EXCEPT_IMPASSIBLE == BtlExceptions[r][c])
		{
			SetButtonHilightArt(D_BTL_CONTROL, i, BTLUI_EXCEPT_HILIGHT);
			SetButtonHilight(D_BTL_CONTROL, i, TRUE);
		}
	}
}

/* ========================================================================
   Function    - GetWhosWhere
   Description - return the person in a given slot
   Returns     - SHORT handle to an avatar
   ======================================================================== */
SHORT GetWhosWhere(SHORT Row, SHORT Col, BOOL Home)
{
	SHORT retVal;

#if 01	
	assert(Row >= 0);
	assert(Row <= 2);
	assert(Col >= 0);
	assert(Col <= 4);
#endif
	
	
	if( Row >= 0 &&
		Row <= 2 &&
		Col >= 0 &&
		Col <= 4
		)
	{
		if( Home )
			retVal = WhosWhere[Row][Col].Home;
		else
			retVal = WhosWhere[Row][Col].Away;
		
	}
	else
		retVal = fERROR;

	return retVal;
}


/* ========================================================================
   Function    - FixUnitsArray
   Description - removes off those nasty dead units
   Returns     - new top of the unit chain
   ======================================================================== */
SHORT FixUnitsArray( SHORT UnitIndex )
{
	SHORT	firstIndex = 0;
	
	// check the home team
	do
	{
		if(units[UnitIndex].iHandle != fERROR)
		{
			CAvatar * const pAvatar = (CAvatar * const) BLKPTR(units[UnitIndex].iHandle);
			
			// if I'm dead and not a regent, remove me
			if (pAvatar->mfAmIImmoblized() && !IsRegent(pAvatar->UnitIndex))
			{
				// Delete unit returns the new next unit.
				UnitIndex = DeleteUnit(UnitIndex, TRUE);
			
				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					#ifdef _WINDOWS
					if (IsMultiPlayer())
					{
						char temp[200];
						sprintf ( temp, "Battles - %s Died",
							STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon]) );
						RandomLogComment ( temp );
					}
					#endif
				}
				
				continue;
			}
		
			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
				#ifdef _WINDOWS
				if (IsMultiPlayer())
				{
					char temp[200];
					sprintf ( temp, "Battles - %s Survived",
						STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon]) );
					RandomLogComment ( temp );
				}
				#endif
			}
			
			// save first index
			if(firstIndex == 0)
				firstIndex = UnitIndex;
		}
		
		UnitIndex = (SHORT) units[UnitIndex].NextUnit;
	
	} while (UnitIndex != fERROR);
	
	return firstIndex;
}

/* ========================================================================
   Function    - CollapseUnits
   Description - join together the weak into new units, then kill the left-overs
   Returns     -
   ======================================================================== */
void CollapseUnits(SHORT UnitIndex) // either home or away
{
	SHORT	NextIndex;
	SHORT	CheckIndex = 0;
	SHORT	MyType;
	
	// check the home team
	do
	{
		if(units[UnitIndex].iHandle != fERROR)
		{
			CAvatar * const pAvatar = (CAvatar * const) BLKPTR(units[UnitIndex].iHandle);
			
			// if I'm not dead or a not a regent or leu
			if (!pAvatar->mfAmIImmoblized() && 
				!IsRegentOrLieutenant(UnitIndex))
			{
				// while my hit points were below full, try to find someone
				// to join up with to get back to full strength
				MyType = pAvatar->fBtlCap.OfficerIcon;
				CheckIndex = (SHORT) units[UnitIndex].NextUnit;
				while( CheckIndex != fERROR
					&& pAvatar->fBtlCap.HitPoints < pAvatar->fBtlCap.Healthy
					)
				{
					CAvatar * const pCheckAvatar = (CAvatar * const) BLKPTR(units[CheckIndex].iHandle);
					// this is my kind of guy and he's had some damage
					// and he and I together are less then healthy
					if( pCheckAvatar->fBtlCap.OfficerIcon == MyType &&
						pCheckAvatar->Status != CAvatar::AI_DEAD &&
						pCheckAvatar->Status != CAvatar::AI_STONE &&
						pCheckAvatar->fBtlCap.HitPoints
							+ pAvatar->fBtlCap.HitPoints
							< pAvatar->fBtlCap.Healthy)
					{
						// suck the life out of this one and discard
						// the body.
						pAvatar->fBtlCap.HitPoints += pCheckAvatar->fBtlCap.HitPoints;
						pCheckAvatar->Status = CAvatar::AI_STONE;
					}
					CheckIndex = (SHORT) units[CheckIndex].NextUnit;
				}
				
				// just too weak to go on, so disband
				if( pAvatar->fBtlCap.HitPoints < (pAvatar->fBtlCap.Healthy/4) )
						pAvatar->Status = CAvatar::AI_DEAD;
			}
		}
		
		UnitIndex = (SHORT) units[UnitIndex].NextUnit;
	
	} while (UnitIndex != fERROR);
}

/* ========================================================================
   Function    - HilightShooter
   Description - turn on the light for a grid with a legal shooter
   Returns     - SHORT 1 if move valid and grid turned on
   ======================================================================== */
SHORT HilightShooter(
	LONG	MenuId,		// menu id of battle panel
	LONG	R,			// row
	LONG	C,			// column
	LONG	Magic,		// FALSE = missile TRUE = magic
	BOOL	*Success	// was this test a success
)
{
	// look at home locations
	const SHORT	l = (R * GRID_MAX_COLS) + C;
	// assume false
	*Success = FALSE;
	
	if(GetWhosWhere(R,C,WW_HOME) != fERROR)
	{
		SHORT	hTemp = hUnitInfo;
		CAvatar * const pAvatar = (CAvatar * const )BLKPTR(GetWhosWhere(R,C,WW_HOME));
		
		if (pAvatar->hPlayerStats == fERROR)
			return TRUE;
			
		DumbAutoLockPtr<PLAYER_STATS const> const dhPlayerStats(pAvatar->hPlayerStats);
		
		if( Magic )
		{
			if (!BattleMagicReady(pAvatar))
				return TRUE;
		}
		else
		{
			if (!BattleMissilesReady(pAvatar))
				return TRUE;
		}
			
			
		// only if in searching mode
		if( pAvatar->Status != CAvatar::AI_SEARCH)
			return TRUE;
			
		hUnitInfo = GetWhosWhere(R,C,WW_HOME);
		if(CheckMissileRangeSub(R,C))
		{
			SetButtonHilightArt ( MenuId, l+2, BTLUI_HOME_HILIGHT);
			SetButtonHilight( MenuId, l+2, TRUE);
			fUpdatePanels = TRUE;
			*Success = TRUE;
		}
		hUnitInfo = hTemp;
	}
	
	return TRUE;	// ok move, keep looking
}   			

/* #################################################################
   #################################################################
   #################################################################
   
        Battle Mode Routines
   
   #################################################################
   #################################################################
   ################################################################# */

/* ========================================================================
   Function    - BattlePanelMode
   Description - change the mode of the battle panel display area
   Returns     -
   ======================================================================== */
void BattlePanelMode( LONG MenuCombo, LONG WhichMode )
{
	LONG	MenuId, ButtonId;
	SHORT	i;
	CAvatar *pAvatar;
	// static storage of the current cursor type
	//static	SHORT	save_cursor = cursor_bitmap;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	fUpdatePanels = TRUE;
	
	if(WhichMode != sBattleMode)
	{
		// clear old mode hilight
		SetButtonHilight(   D_BTL_CONTROL, BTL_RESV_ID, FALSE);
		SetButtonHilight(   D_BTL_CONTROL, BTL_STAT_ID, FALSE);
		
		SetButtonHilight(D_BTL_CONTROL, BTL_LIGHT_ID, FALSE);
		
		// turn off the action buttons
		activate_region (BTL_CANCEL_ID, FALSE);
		SetButtonHilight(D_BTL_CONTROL, BTL_CANCEL_ID, FALSE);
		SetButtonFlag   (D_BTL_CONTROL, BTL_CANCEL_ID, D_LABEL_OFF);
		
		activate_region (BTL_FALLBACK_ID, FALSE);
		SetButtonHilight(D_BTL_CONTROL, BTL_FALLBACK_ID, FALSE);
		SetButtonFlag   (D_BTL_CONTROL, BTL_FALLBACK_ID, D_LABEL_OFF);
		
		// reset cursor
		//SysHideCursor();
		//cursor_bitmap = save_cursor;
		//SysShowCursor();
		
		// clear out old modes hilights
		switch(sBattleMode)
		{
		case BTLUI_MODE_MAGIC:
			ClearMagicRegions();
			// fall through
		case BTLUI_MODE_MISSILE:
		case BTLUI_MODE_FALLBACK:
		case BTLUI_MODE_MOVE:
			// clear modal grid hilights
			for(i=2;i<17;i++)
			{
				ClearButtonFlag( MenuId, i, D_SANS_5|D_SANS_6);
				SetButtonHilight(D_BTL_CONTROL, i, FALSE);
			}
			break;
		}
			
		// turn the engine back on
		fRender = TRUE;
		if ( !fBtlTurnBased )
			fBattleFreezeTime = FALSE;
		
		sBattleMode = WhichMode;
		
		// do new mode stuff
		switch(sBattleMode)
		{
		case BTLUI_MODE_RESERVES:
			SetButtonHilight(D_BTL_CONTROL, BTL_RESV_ID, TRUE);
			SetButtonArt(D_BTL_CONTROL, 0, BTLUI_BATLPNL1);
			break;
			
		case BTLUI_MODE_STATUS:
			SetButtonHilight(D_BTL_CONTROL, BTL_STAT_ID, TRUE);
			SetButtonArt(D_BTL_CONTROL, 0, BTLUI_BATLPNL2);
			break;
			
		case BTLUI_MODE_UNITINFO:	
			SetButtonHilight(D_BTL_CONTROL, BTL_LIGHT_ID, TRUE);
			SetButtonArt(D_BTL_CONTROL, 0, BTLUI_BATLPNL1);
			break;
			
		
		case BTLUI_MODE_MISSILE:
		case BTLUI_MODE_FALLBACK:
		case BTLUI_MODE_MOVE:
		case BTLUI_MODE_MAGIC:
			if (battleTroops != fERROR)
			{
				CAvatar * const pAvatar = (CAvatar * const)BLKPTR(battleTroops);
		 		// add a text to select reserves
		 		if (pAvatar->fBtlCap.Row == 0)
		 		{
					activate_region(BTL_FALLBACK_ID, TRUE);
					SetButtonHilight(D_BTL_CONTROL, BTL_FALLBACK_ID, TRUE);
					ClearButtonFlag(D_BTL_CONTROL, BTL_FALLBACK_ID, D_LABEL_OFF);
		 		}
			}
			fRender = TRUE;
			#ifdef _WINDOWS
			if (!IsMultiPlayer())
			#endif
				if ( !fBtlTurnBased )
					fBattleFreezeTime = TRUE;
			activate_region(BTL_CANCEL_ID, TRUE);
			SetButtonHilight(D_BTL_CONTROL, BTL_CANCEL_ID, TRUE);
			ClearButtonFlag(D_BTL_CONTROL, BTL_CANCEL_ID, D_LABEL_OFF);
			SetButtonArt(D_BTL_CONTROL, 0, BTLUI_BATLPNL1);
			break;
		
		}
			
		// now catch the exceptions
		SetExceptionGrids();
	}
}

/* ========================================================================
   Function    - ChooseBattleField
   Description - routine for the choose battle field dialog
   Returns     -
   ======================================================================== */
void ChooseBattleField( LONG MenuCombo, LONG WhichField )
{
	LONG	MenuId, ButtonId;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// convert button id to field index
	cBattleField = (CHAR)WhichField + '0';
	
	// close the menu
	HideMenu(D_BTL_CHOOSEFIELD);
}

/* ========================================================================
   Function    - BattleReservesMode
   Description - toggle the Reserves mode
   Returns     -
   ======================================================================== */
void BattleReservesMode( LONG, LONG )
{ 	
	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
	if(mouse_button == 2)
	{
		SystemHelp(STR_BTLUI_HELP_RESERVES_TITLE,STR_BTLUI_HELP_RESERVES_TEXT, H_Reserves, NULL, 0);
	}
	else	// change to reserves mode
	{
		sOldBattleMode = sBattleMode;
		BattlePanelMode(BUILD_LONG(D_BTL_CONTROL,0),BTLUI_MODE_RESERVES);
	}
}

/* ========================================================================
   Function    - BattleStatusMode
   Description - toggle the status mode
   Returns     -
   ======================================================================== */
void BattleStatusMode( LONG, LONG )
{
	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
	if(mouse_button == 2)
	{
		SystemHelp(STR_BTLUI_HELP_STATUS_TITLE,STR_BTLUI_HELP_STATUS_TEXT, H_Battle_Status, NULL, 0);
	}
	else	// change to reserves mode
	{
		sOldBattleMode = sBattleMode;
		BattlePanelMode(BUILD_LONG(D_BTL_CONTROL,0),BTLUI_MODE_STATUS);
	}
}

/* ========================================================================
   Function    - BattleChatMode
   Description - toggle the chat mode
   Returns     -
   ======================================================================== */
void BattleChatMode( LONG chatToggle, LONG first)
{
	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
//	if(mouse_button == 2)
//	{
//		SystemHelp(STR_BTLUI_HELP_CHAT_TITLE,STR_BTLUI_HELP_CHAT_TEXT, H_Chat, NULL, 0);
//	}
//	else
//	{
		LONG	ChatDisplayWidth,ChatDisplayHeight;
		if ( !ChatDisplaySize )
		{
			GetButtonSize(D_CHAT_DISPLAY_WINDOW, 0, &ChatDisplayWidth, &ChatDisplayHeight);
		}
		else
		{
			GetButtonSize(D_CHAT_DISPLAY_WINDOW1, 0, &ChatDisplayWidth, &ChatDisplayHeight);
		}
	
		if (chatToggle)
			fChatLineOn = !fChatLineOn;
			
		if (fChatLineOn)
		{
			SetButtonHilight(   D_BTL_CONTROL, BTL_CHAT_ID, TRUE);
			MoveChatDisplayWindow(0,0);
			ShowChatDisplayWindow(0,0);
			set_margin_size (0, 0, ChatDisplayHeight, BTLUI_PANEL_HEIGHT);
			if(!first)
				MARGIN_CONTROL::mfResetCurrentSize();
		}
		else
		{
			SetButtonHilight(   D_BTL_CONTROL, BTL_CHAT_ID, FALSE);
			HideChatDisplayWindow(0,0);
			set_margin_size (0, 0, 0, BTLUI_PANEL_HEIGHT);
			if(!first)
				MARGIN_CONTROL::mfResetCurrentSize();
		}

//	}

}

/* ========================================================================
   Function    - BattleFallBack
   Description - select a target to fallback
   Returns     -
   ======================================================================== */
void BattleFallBack(LONG, LONG )
{
	if(battleTroops != fERROR)
	{
		// tell these dudes to try to go to the reserves
		CAvatar	* const pAvatar = (CAvatar * const)BLKPTR(battleTroops);
		
		// remove him from the grid
		SetWhosWhere(pAvatar->fBtlCap.Row,pAvatar->fBtlCap.Column,WW_HOME,fERROR);
		startLoc = -1;
		
		// can't look at him anymore
		BattleCameraHighView(0, 0);
					
		SendToReserves(pAvatar);
		// now place him in the reserves
		
		// GWP 5/18/97
		// Now call mfFallBack instead of the next two calls.
		//
		SendFallBack ( 
			pAvatar,
			pAvatar->fBtlCap.TargetRow,
			pAvatar->fBtlCap.TargetCol);
		pAvatar->fBtlCap.mfFallBack(pAvatar->hThis);
		
		// GWP BattleMoveToReservesArea(pAvatar->hThis);
		// GWP BattlePutInReserves(1, battleTroops);
		//
		//
		
		battleTroops = fERROR;
		// always go to reserves mode at this point
		BattlePanelMode(BUILD_LONG(D_BTL_CONTROL,0),BTLUI_MODE_RESERVES);
		AddVoxSnd(VOX_BACK_TO_RESERVES,pAvatar->ThingIndex);
	}
	// this mode is over
	BattleCancel(0,0);
}

/* ========================================================================
   Function    - BattleCancel
   Description - cancel current mode
   Returns     -
   ======================================================================== */
void BattleCancel(LONG, LONG)
{
	SHORT i;
	switch(sBattleMode)
	{
	case BTLUI_MODE_FALLBACK:
	case BTLUI_MODE_MAGIC:
		BattleCancelMissile(0,0);
		break;
	case BTLUI_MODE_MOVE:
	case BTLUI_MODE_MISSILE:
		BattleCancelMove(0,0);
		break;
	}
	fRender = TRUE;
	if ( !fBtlTurnBased )
		fBattleFreezeTime = FALSE;
	fUpdatePanels = TRUE;
	battleTroops = fERROR;
	hMissile = fERROR;
	hHilightAvatar = fERROR;
	
	// turn off the action buttons
	activate_region(BTL_CANCEL_ID, FALSE);
	SetButtonHilight(D_BTL_CONTROL, BTL_CANCEL_ID, FALSE);
	SetButtonFlag(D_BTL_CONTROL, BTL_CANCEL_ID, D_LABEL_OFF);
		
	activate_region(BTL_FALLBACK_ID, FALSE);
	SetButtonHilight(D_BTL_CONTROL, BTL_FALLBACK_ID, FALSE);
	SetButtonFlag(D_BTL_CONTROL, BTL_FALLBACK_ID, D_LABEL_OFF);
		
	// clear modal grid hilights
	for(i=2;i<17;i++)
		SetButtonHilight(D_BTL_CONTROL, i, FALSE);
	
	SetExceptionGrids();
}

/* ========================================================================
   Function    - BattleCancelMissile
   Description - get out of missile mode
   Returns     -
   ======================================================================== */
void BattleCancelMissile(LONG, LONG)
{
	hHilightAvatar = fERROR;
	BattlePanelMode(BUILD_LONG(D_BTL_CONTROL,0),sOldBattleMode);
}

/* ========================================================================
   Function    - BattleCancelSelUnit
   Description - get out of SelUnit mode
   Returns     -
   ======================================================================== */
void BattleCancelSelUnit(LONG, LONG)
{
	BattlePanelMode(BUILD_LONG(D_BTL_CONTROL,0),sOldBattleMode);
}

/* ========================================================================
   Function    - BattleCancelMove
   Description -
   Returns     -
   ======================================================================== */
void BattleCancelMove(LONG, LONG)
{
	SHORT		i;
	
	// if from the reserves, put him back
	if(startLoc == -1)
	{
		BattlePutInReserves(1, battleTroops);
		battleTroops = fERROR;
	}
	
	hHilightAvatar = fERROR;
	startLoc = -2;	// mark invalid
	battleTroops = fERROR;
	
	BattlePanelMode(BUILD_LONG(D_BTL_CONTROL,0),sOldBattleMode);
}

/* ========================================================================
   Function    - BattleMissileBox
   Description - 
   Returns     - 
   ======================================================================== */
void BattleMissileBox( LONG val, LONG )
{
	BattleCurrentSpell = val;
	AddSndObj(SND_PICKUP_MAGIC_ITEM1, 0, VOLUME_SEVENTY);
	fUpdatePanels = TRUE;
}

/* ========================================================================
   Function    - BattleHotBox
   Description - 
   Returns     - 
   ======================================================================== */
void BattleHotBox( LONG BoxNumber, LONG )
{
	// set the magic type to whats in BoxNumber
	BattleCurrentSpell = BoxNumber;
	AddSndObj(SND_PICKUP_MAGIC_ITEM1, 0, VOLUME_SEVENTY);
	fUpdatePanels = TRUE;
}

/* ========================================================================
   Function    - SetMagicRegions
   Description - 
   Returns     - 
   ======================================================================== */
void SetMagicRegions(void)
{
	LONG	x,y,mx,my,i;
	CHAR	buffer[80], format_buffer[80];
	SHORT	color;
	LONG	const spellWidth = BTL_MAGIC_SPELL_LIST_X2 - BTL_MAGIC_SPELL_LIST_X1 - 10;
	
	if (battleTroops == fERROR)
		return;
		
	CAvatar * const pAvatar = (CAvatar * const)BLKPTR(battleTroops);
		
	GetButtonPosition(D_BTL_CONTROL, 0, &mx, &my);
	
	if (pAvatar->hPlayerStats == fERROR)
		return;
		
	DumbAutoLockPtr<PLAYER_STATS const> const dhPlayerStats(pAvatar->hPlayerStats);
	
	init_gfont(FONT_SANS_8PT);
	
	
	// first column
	x = mx + BTL_MAGIC_SPELL_LIST_X1;
	y = my + BTL_MAGIC_SPELL_LIST_Y;
	for (i=0;i<3;i++)
	{
		if (BattleCurrentSpell == i)
			color = YELLOW;
		else
			color = WHITE;
			
		InvenIndex const sSpell = dhPlayerStats->Inventory.mfGetSpellBox(i);
		
		y += 12;
		
		if (sSpell >-1)
		{
			ITEMTYPE const SpellType = ObjectList::mfData(sSpell).mfType();
			THINGTYPE SpellThing = ItemTypes[SpellType].mfGameType();
			sprintf(buffer, "%d - %s", i+1, STRMGR_GetStr(GAME_TTYPE::mfGetSTRMgrId(SpellThing)));
			gprint_text( x,y, buffer, color);
			add_region( x, y, spellWidth, 10, KEY_1 + i, BattleHotBox, i, 0, 0, -1);
		}
		else
		{
			strncpy(format_buffer, STRMGR_GetStr(BTLUI_MAGIC_EMPTY),sizeof(buffer));
			sprintf(buffer,format_buffer, i+1); 
			gprint_text( x,y, buffer, color);
		}
	}
	// second column
	x = mx + BTL_MAGIC_SPELL_LIST_X2;
	y = my + BTL_MAGIC_SPELL_LIST_Y;
	for (;i<6;i++)
	{
		if (BattleCurrentSpell == i)
			color = YELLOW;
		else
			color = WHITE;
			
		InvenIndex const sSpell = dhPlayerStats->Inventory.mfGetSpellBox(i);
		
		y += 12;
		
		if (sSpell >-1)
		{
			ITEMTYPE const SpellType = ObjectList::mfData(sSpell).mfType();
			THINGTYPE SpellThing = ItemTypes[SpellType].mfGameType();
			sprintf(buffer, "%d - %s", i+1, STRMGR_GetStr(GAME_TTYPE::mfGetSTRMgrId(SpellThing)));
			gprint_text( x,y, buffer, color);
			add_region( x, y, spellWidth, 10, KEY_1 + i, BattleHotBox, i, 0, 0, -1);
		}
		else
		{
			strncpy(format_buffer, STRMGR_GetStr(BTLUI_MAGIC_EMPTY),sizeof(buffer));
			sprintf(buffer,format_buffer, i+1); 
			gprint_text( x,y, buffer, color);
		}
	}
	
 	// add a region to select for shot missiles
	x = mx + BTL_MAGIC_SPELL_LIST_X1;
	y += 12;
	if (BattleMissilesReady(pAvatar))
	{
		if (BattleCurrentSpell == BTL_MAGIC_SPELL_MISSILES)
			color = YELLOW;
		else
			color = WHITE;
		strncpy(buffer, STRMGR_GetStr(BTLUI_MISSILE_REGION),sizeof(buffer));
		gprint_text( x,y, buffer, color);
		add_region( x, y, 80, 10, KEY_A, BattleMissileBox, BTL_MAGIC_SPELL_MISSILES, 0, 0, -1);
	}
	
}

/* ========================================================================
   Function    - ClearMagicRegions
   Description - 
   Returns     - 
   ======================================================================== */
void ClearMagicRegions(void)
{
	del_region(BattleHotBox,0);
	del_region(BattleMissileBox,0);
}


// This code is not called right now. I removed it because it was using context
// numbers that were removed from the help file. dlj 5/30/97

#if 0
/* ========================================================================
   Function    - BattleSelMagic
   Description - select a magic unit
   Returns     -
   ======================================================================== */
void BattleSelMagic(LONG MenuCombo, LONG )
{
	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
	if(mouse_button == 2)
	{
		SystemHelp(STR_BTLUI_HELP_MAGIC_TITLE,STR_BTLUI_HELP_MAGIC_TEXT, H_Magic_Button, NULL, 0);
	}
	else
	{
		if (sBattleMode != BTLUI_MODE_MOVE &&
			sBattleMode != BTLUI_MODE_MAGIC &&
			sBattleMode != BTLUI_MODE_MISSILE &&
			sBattleMode != BTLUI_MODE_FALLBACK
		)
		{
			sOldBattleMode = sBattleMode;
			BattlePanelMode(BUILD_LONG(D_BTL_CONTROL,0),BTLUI_MODE_MAGIC);
		}
	}
}

#endif

/* ========================================================================
   Function    - HilightMissile
   Description - turn on the light for a grid with a legal missile target
   Returns     - SHORT 1 if move valid and grid turned on
   ======================================================================== */
SHORT HilightMissile(
	LONG	MenuId,		// menu id of battle panel
	LONG	R,			// row
	LONG	C,			// column
	LONG	Away,		// check the away side
	BOOL	*Success	// was this test a success
)
{
	// look at enemy locations
	const SHORT	l = (R * GRID_MAX_COLS) + C;
	SHORT		hUnit;
	
	// assume false
	*Success = FALSE;
	
	// if the enemy is asking, add one to the row
	if(Away)
		hUnit = GetWhosWhere(R,C,WW_AWAY);
	else
		hUnit = GetWhosWhere(R,C,WW_HOME);
		
	// if someone from enemy army is in this square, hilight
	if(hUnit != -1)
	{
		SetButtonHilightArt ( MenuId, l+2, BTLUI_AWAY_HILIGHT);
		SetButtonHilight( MenuId, l+2, TRUE);
		fUpdatePanels = TRUE;
		*Success = TRUE;
	}
	
	return TRUE;	// ok move, keep looking
}


/* #################################################################
   #################################################################
   #################################################################
   
        Battle Cursor/Mouse Routines
   
   #################################################################
   #################################################################
   ################################################################# */

/* ========================================================================
   Function    - SetWhosWhere
   Description - put a person in a given slot
   Returns     -
   ======================================================================== */
void SetWhosWhere(SHORT Row, SHORT Col, BOOL Home, SHORT hAvatar)
{

#if 0
	assert(Row >= 0);
	assert(Row <= 2);
	assert(Col >= 0);
	assert(Col <= 4);
#endif
	
	if ( Row >= 0 &&
		Row <= 2 &&
		Col >= 0 &&
		Col <= 4
		)
	{
		if( Home )
			WhosWhere[Row][Col].Home = hAvatar;
		else
			WhosWhere[Row][Col].Away = hAvatar;
	}
}

/* ========================================================================
   Function    - ConvertFieldToGrid
   Description - 
   Returns     - 
   ======================================================================== */
static BOOL ConvertFieldToGrid( LONG fx, LONG fy, LONG *gx, LONG *gy, BOOL *home)
{
	LONG	i;
		
	// find x -> column
	if (fx >= FIELDGRID1_X && fx < FIELDGRID2_X)
		*gx = 0;
	else
	if (fx >= FIELDGRID2_X && fx < FIELDGRID3_X)
		*gx = 1;
	else
	if (fx >= FIELDGRID3_X && fx < FIELDGRID4_X)
		*gx = 2;
	else
	if (fx >= FIELDGRID4_X && fx < FIELDGRID5_X)
		*gx = 3;
	else
	if (fx >= FIELDGRID5_X && fx < FIELDGRID6_X)
		*gx = 4;
	else
	{
		*gx = -1;
		return FALSE;
	}
		
	// find y -> row and home/away
	if (fy > FIELDGRID11_Y && fy < FIELDGRID12_Y)
	{
		*gy = 0;
		*home = TRUE;
	}
	else
	if (fy > FIELDGRID12_Y && fy < FIELDGRID21_Y)
	{
		*gy = 0;
		*home = FALSE;
	}
	else
	if (fy > FIELDGRID21_Y && fy < FIELDGRID22_Y)
	{
		*gy = 1;
		*home = TRUE;
	}
	else
	if (fy > FIELDGRID22_Y && fy < FIELDGRID31_Y)
	{
		*gy = 1;
		*home = FALSE;
	}
	else
	if (fy > FIELDGRID31_Y && fy < FIELDGRID32_Y)
	{
		*gy = 2;
		*home = TRUE;
	}
	else
	if (fy > FIELDGRID32_Y && fy < FIELDGRID41_Y)
	{
		*gy = 2;
		*home = FALSE;
	}
	else
	{
		*gy = -1;
		return FALSE;
	}
	
	return TRUE;
}

/* ========================================================================
   Function    - WhichGridHalf
   Description - convert x,y on the grid panel into row, column and which half
   Returns     - 
   ======================================================================== */

static BOOL WhichGridHalf( LONG CX, LONG CY, SHORT *Row, SHORT *Column, BOOL *Home)
{
	SHORT i;
	// sorry for the mess, it's just the way the number work
	//  Bound Column
	if(CX >= BTLUI_GRID_X1 && CX < (BTLUI_GRID_X1+BTLUI_GRID_W) )
		*Column= 0;
	else
	if(CX >= BTLUI_GRID_X2 && CX < (BTLUI_GRID_X2+BTLUI_GRID_W) )
		*Column= 1;
	else
	if(CX >= BTLUI_GRID_X3 && CX < (BTLUI_GRID_X3+BTLUI_GRID_W) )
		*Column= 2;
	else
	if(CX >= BTLUI_GRID_X4 && CX < (BTLUI_GRID_X4+BTLUI_GRID_W) )
		*Column= 3;
	else
	if(CX >= BTLUI_GRID_X5 && CX <= (BTLUI_GRID_X5+BTLUI_GRID_W) )
		*Column= 4;
	else
		return FALSE;
	
	//  Bound Row
	i = 480 - BTLUI_PANEL_HEIGHT;
	if(CY >= i+BTLUI_GRID_Y1 && CY < (i+BTLUI_GRID_Y1+BTLUI_GRID_HALF) )
	{
		*Row = 2;
		*Home = FALSE;
	}
	else
	if(CY >= i+BTLUI_GRID_Y1+BTLUI_GRID_HALF && CY < (i+BTLUI_GRID_Y1+BTLUI_GRID_H) )
	{
		*Row = 2;
		*Home = TRUE;
	}
	else
	if(CY >= i+BTLUI_GRID_Y2 && CY < (i+BTLUI_GRID_Y2+BTLUI_GRID_HALF) )
	{
		*Row = 1;
		*Home = FALSE;
	}
	else
	if(CY >= i+BTLUI_GRID_Y2+BTLUI_GRID_HALF && CY < (i+BTLUI_GRID_Y2+BTLUI_GRID_H) )
	{
		*Row = 1;
		*Home = TRUE;
	}
	else
	if(CY >= i+BTLUI_GRID_Y3 && CY < (i+BTLUI_GRID_Y3+BTLUI_GRID_HALF) )
	{
		*Row = 0;
		*Home = FALSE;
	}
	else
	if(CY >= i+BTLUI_GRID_Y3+BTLUI_GRID_HALF && CY <= (i+BTLUI_GRID_Y3+BTLUI_GRID_H) )
	{
		*Row = 0;
		*Home = TRUE;
	}
	else
		return FALSE;

	return TRUE;
}

/* ========================================================================
   Function    - SetBattleUIMouseCursor
   Description - 
   Returns     - 
   ======================================================================== */
void SetBattleUIMouseCursor (void)
{
	LONG	fx, fy;
	LONG	Row, Column;
	BOOL	Home;
	SHORT	location;
	LONG	DataCombo;
	LONG	MenuCombo;
	LONG	CursorType = BTL_CURSOR_CANCEL;
	LONG	UnitType;
	
	ConvertScreenToFloor(cursor_x, cursor_y, &fx, &fy);
	if (!ConvertFieldToGrid(fx, fy, &Column, &Row, &Home))
		return;
	
	// default cursor
	CursorType = BTL_CURSOR_DAGGER;
	
	// is row/column in range
	if (Column >= 0 && Row >= 0 &&
		Column < GRID_MAX_COLS && Row < GRID_MAX_ROWS )
	{
		if (mouse_click != 0)
		{
			location = (Row * GRID_MAX_COLS) + Column;
			Home = (SHORT)((Home << 8) | 1);
			MenuCombo = BUILD_LONG((SHORT)D_BTL_CONTROL,(SHORT)(location+2));
			DataCombo = BUILD_LONG(Home,location);
			
			BattleControlGrid(MenuCombo, DataCombo);
			mouse_click = 0;
		}
		
		if (cursor_y < (480-BTLUI_PANEL_HEIGHT))
		{
			CursorType = BTL_CURSOR_DAGGER;
			
			if (battleTroops != fERROR)
			{
				CAvatar * const pAvatar = (CAvatar *const) BLKPTR(battleTroops);
				
				if (Home &&
					pAvatar->fBtlCap.Row == Row &&
					pAvatar->fBtlCap.Column == Column )
				{
					CursorType = BTL_CURSOR_CANCEL;
				}
				
				location = (Row * GRID_MAX_COLS) + Column;
				if ((GetButtonFlags(D_BTL_CONTROL, location+2) & D_HILIGHTED))
				{
					SHORT Who = GetWhosWhere(Row,Column,WW_AWAY);
					CursorType = BTL_CURSOR_MARCH;
					
					if (!Home && GetButtonFlags(D_BTL_CONTROL, location+2) & D_SANS_6)
					{
						if (Who != fERROR)
						{
							if (BattleMissilesReady(pAvatar))
								CursorType = BTL_CURSOR_MISSILE;
							
							if (BattleMagicReady(pAvatar))
								CursorType = BTL_CURSOR_MAGIC;
						}
					}
					else
					{
						if (Who != fERROR)
							CursorType = BTL_CURSOR_ENGAGE;
					}
				}
			}
		}
		else
			CursorType = BTL_CURSOR_DAGGER;
	}
		
	if (BattleCursor != CursorType)
	{
		BattleCursor = CursorType;
		switch(BattleCursor)
		{
		case BTL_CURSOR_CANCEL: 
			ChangeBattleCursor("CANCEL",CURSOR_HOTSPOT_UL);
			break;
		case BTL_CURSOR_MARCH: 
			ChangeBattleCursor("MARCH",CURSOR_HOTSPOT_UL);
			break;
		case BTL_CURSOR_ENGAGE: 
			ChangeBattleCursor("ENGAGE",CURSOR_HOTSPOT_UL);
			break;
		case BTL_CURSOR_MISSILE: 
			ChangeBattleCursor("MISSILE",CURSOR_HOTSPOT_UL);
			break;
		case BTL_CURSOR_MAGIC: 
			ChangeBattleCursor("MAGIC",CURSOR_HOTSPOT_UL);
			break;
		default:
		case BTL_CURSOR_DAGGER: 
			ChangeBattleCursor("DAGGER",CURSOR_HOTSPOT_UL);
			break;
		}
	}
}

/* ========================================================================
   Function    - ChangeBattleCursor
   Description - 
   Returns     - 
   ======================================================================== */
void ChangeBattleCursor (CHAR *pArtName, BOOL HotSpot)
{
	CHAR	pPathName[40];
#ifdef _WINDOWS
	if (sDrawMode == iDDRAW)
	{
		// set mouse to be his icon art
		SysHideCursor();
		SetPurge(cursor_bitmap);
		sprintf(pPathName, "graphics\\%s.pcx", pArtName);
		cursor_bitmap = GetResourceStd(pPathName, FALSE);
		SysShowCursor();
	}
	else
		init_mouse(pArtName, HotSpot);
#else
	// set mouse to be his icon art
	SysHideCursor();
	SetPurge(cursor_bitmap);
	sprintf(pPathName, "graphics\\%s.pcx", pArtName);
	cursor_bitmap = GetResourceStd(pPathName, FALSE);
	SysShowCursor();
#endif
}

/* #################################################################
   #################################################################
   #################################################################
   
        Choose Battle Field Dialog Routines
   
   #################################################################
   #################################################################
   ################################################################# */

/* ========================================================================
   Function    - BattleChooseField
   Description - choose the battle field to defend from
   Returns     - a character between '0' and '3' inclusive
   ======================================================================== */
CHAR BattleChooseField( CHAR TerrainType )
{
	SHORT	StartingMenuCount;
	
	switch( TerrainType )
	{
	case 'w':	// woods
		SetButtonArt   ( D_BTL_CHOOSEFIELD,  1, BTLUI_BATLW0 );
		SetButtonArt   ( D_BTL_CHOOSEFIELD,  2, BTLUI_BATLW1 );
		SetButtonArt   ( D_BTL_CHOOSEFIELD,  3, BTLUI_BATLW2 );
		SetButtonArt   ( D_BTL_CHOOSEFIELD,  4, BTLUI_BATLW3 );
		SetButtonLabel ( D_BTL_CHOOSEFIELD, 91, STR_BTLUI_WOODS_0, 1 );
		SetButtonLabel ( D_BTL_CHOOSEFIELD, 92, STR_BTLUI_WOODS_1, 1 );
		SetButtonLabel ( D_BTL_CHOOSEFIELD, 93, STR_BTLUI_WOODS_2, 1 );
		SetButtonLabel ( D_BTL_CHOOSEFIELD, 94, STR_BTLUI_WOODS_3, 1 );
		break;
	case 'p':	// plains
		SetButtonArt   ( D_BTL_CHOOSEFIELD,  1, BTLUI_BATLP0 );
		SetButtonArt   ( D_BTL_CHOOSEFIELD,  2, BTLUI_BATLP1 );
		SetButtonArt   ( D_BTL_CHOOSEFIELD,  3, BTLUI_BATLP2 );
		SetButtonArt   ( D_BTL_CHOOSEFIELD,  4, BTLUI_BATLP3 );
		SetButtonLabel ( D_BTL_CHOOSEFIELD, 91, STR_BTLUI_PLAINS_0, 1 );
		SetButtonLabel ( D_BTL_CHOOSEFIELD, 92, STR_BTLUI_PLAINS_1, 1 );
		SetButtonLabel ( D_BTL_CHOOSEFIELD, 93, STR_BTLUI_PLAINS_2, 1 );
		SetButtonLabel ( D_BTL_CHOOSEFIELD, 94, STR_BTLUI_PLAINS_3, 1 );
		break;
	case 'm':	// mountains
		SetButtonArt   ( D_BTL_CHOOSEFIELD,  1, BTLUI_BATLM0 );
		SetButtonArt   ( D_BTL_CHOOSEFIELD,  2, BTLUI_BATLM1 );
		SetButtonArt   ( D_BTL_CHOOSEFIELD,  3, BTLUI_BATLM2 );
		SetButtonArt   ( D_BTL_CHOOSEFIELD,  4, BTLUI_BATLM3 );
		SetButtonLabel ( D_BTL_CHOOSEFIELD, 91, STR_BTLUI_MNTS_0, 1 );
		SetButtonLabel ( D_BTL_CHOOSEFIELD, 92, STR_BTLUI_MNTS_1, 1 );
		SetButtonLabel ( D_BTL_CHOOSEFIELD, 93, STR_BTLUI_MNTS_2, 1 );
		SetButtonLabel ( D_BTL_CHOOSEFIELD, 94, STR_BTLUI_MNTS_3, 1 );
		break;
	case 's':	// swamp
		SetButtonArt   ( D_BTL_CHOOSEFIELD,  1, BTLUI_BATLS0 );
		SetButtonArt   ( D_BTL_CHOOSEFIELD,  2, BTLUI_BATLS1 );
		SetButtonArt   ( D_BTL_CHOOSEFIELD,  3, BTLUI_BATLS2 );
		SetButtonArt   ( D_BTL_CHOOSEFIELD,  4, BTLUI_BATLS3 );
		SetButtonLabel ( D_BTL_CHOOSEFIELD, 91, STR_BTLUI_SWAMP_0, 1 );
		SetButtonLabel ( D_BTL_CHOOSEFIELD, 92, STR_BTLUI_SWAMP_1, 1 );
		SetButtonLabel ( D_BTL_CHOOSEFIELD, 93, STR_BTLUI_SWAMP_2, 1 );
		SetButtonLabel ( D_BTL_CHOOSEFIELD, 94, STR_BTLUI_SWAMP_3, 1 );
		break;
	default:
		return '0';
	}
	
	// Show the menu and run the dialog idle loop
	ShowMenu(D_BTL_CHOOSEFIELD);
	
	// set the defauld field
	cBattleField = '0';
	
	// find how many menus are up
	StartingMenuCount = sMenusUp;
	
	// call the idle loop until the count differs
	// NOTE: we won't leave this routine until a field is selected
	while( sMenusUp == StartingMenuCount )
	{
		MenuLoop();
	}
	
	return cBattleField;
	
}

/* ========================================================================
   Function    - PaintChooseBattleField
   Description - paint the title on the top of the dialog
   Returns     -
   ======================================================================== */
void PaintChooseBattleField(LONG, LONG)
{
	CHAR	temp[80];
	CHAR	buffer[80];
	LONG	mx,my;
	LONG	mw,mh;
	SHORT	scale;
	
	if( GetButtonPosition( D_BTL_CHOOSEFIELD, 0, &mx, &my ) == fERROR)
		return;
	
	if( GetButtonSize( D_BTL_CHOOSEFIELD, 0, &mw, &mh ) == fERROR)
		return;
	
	// fill in data
	strcpy(temp,STRMGR_GetStr(BTLUI_WHOS_ATTACKING));
	if(province[units[SCENE_MGR::AwayIndex].province].Realm != HomeRealm
		&& units[SCENE_MGR::HomeIndex].Realm == HomeRealm)
		sprintf(
			buffer,
			temp,
			province[units[SCENE_MGR::AwayIndex].province].name,
			realm[HomeRealm].mfGetName()
			);
	else
		sprintf(
			buffer,
			temp,
			province[units[SCENE_MGR::AwayIndex].province].name,
			realm[units[SCENE_MGR::AwayIndex].Realm].mfGetName()
			);
	
	mx += mw / 2;
	my += 15;
	
	init_gfont(FONT_SANS_12PT);
	
	// calculate scale
	
	LONG const BufferWidth = gtext_width(buffer);
	if (BufferWidth > 0)
	{
		scale = ((mw-50)*100/BufferWidth);
		if(scale > 100)
			print_text_centered(mx,my, buffer, DKBROWN);
		else
			print_textf(mx,my, DKBROWN, "^c^X%03d%s", scale, buffer);
	}
				
	my += 14;
	
	init_gfont(FONT_SANS_8PT);
	strncpy(buffer, STRMGR_GetStr(BTLUI_CHOOSE_FIELD), sizeof(buffer));
	print_text_centered(mx,my,buffer,DKBROWN);
}

/* #################################################################
   #################################################################
   #################################################################
   
        Victory/Defeat Dialog Routines
   
   #################################################################
   #################################################################
   ################################################################# */


/* ========================================================================
   Function    - BattleLoose
   Description - Tell you that you lost the battle
   Returns     -
   ======================================================================== */
void BattleLoose(LONG, LONG)
{
	fUpdatePanels = TRUE;
	RunPanels();


	if(BtlWithdraw == FALSE)
	{
		SendWin ( FALSE );
		BtlWithdraw = TRUE;
		AddSndObj(SND_MUST_WITHDRAW1, SND_MUST_WITHDRAW_TOTAL,VOLUME_PRIORITY);
	}
	
	CollapseUnits(SCENE_MGR::HomeIndex);
	CollapseUnits(SCENE_MGR::AwayIndex);
	
	BattleExpPoints( SCENE_MGR::AwayIndex, BTL_EXP_WIN, 0, FALSE );
	if ( HomeStrengthLarger )
		BattleExpPoints( SCENE_MGR::AwayIndex, BTL_EXP_WIN_GREATER_STRENGTH, 0, FALSE );
	
	DefeatScreen(PaintBattleLose, BattleLooseProc);
}

/* ========================================================================
   Function    - BattleLooseProc
   Description -
   Returns     -
   ======================================================================== */
void BattleLooseProc(LONG MenuCombo, LONG button)
{
	SHORT i,j;
	
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
	RunMenus();
	RunPanels();
	
	// now kill off anyone from our team left on the battle field
	for(i=0;i<3;i++)
	{
		for(j=0;j<5;j++)
		{
			if(GetWhosWhere(i,j,WW_HOME) != fERROR)
			{
				CAvatar * const pAvatar = (CAvatar * const ) BLKPTR(GetWhosWhere(i,j,WW_HOME));
				// make sure non-regents are dead
				if (!IsRegent(pAvatar->UnitIndex))
					pAvatar->Status = CAvatar::AI_DEAD;
			}
		}
	}
			
	#ifdef _WINDOWS
	if (IsMultiPlayer())
	{
		// wait for results
		while (TRUE)
		{
			if ( AMultiPlayer.HaveEvent() == NPCBATTLE_ACTION )
			{
				SCENE_MGR::Loser = EventTable.data1;
				SCENE_MGR::Victor = EventTable.data2;
				AMultiPlayer.InitEvent(0);
				break;
			}

			// get messages and give time up to the system
			AMultiPlayer.LookForMessages();   //---- Update not specific messages
			WindowsMessages();
			Sleep(0);

			if ( (GetAsyncKeyState( VK_F10 ) & 0x8000)
					|| !realm[sAwayRealm].mfIsPlayerCtrl() )
			{
				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					char temp[150];
					sprintf ( temp, "Finalizing NPCBattle - VK_F10=%ld and PlayerCtrl[%ld]=%ld\n",
						(LONG)(GetAsyncKeyState( VK_F10 ) & 0x8000),
						(LONG)sAwayRealm,
						(LONG)(!realm[sAwayRealm].mfIsPlayerCtrl()));
					RandomLogComment ( temp );
				}

				AMultiPlayer.Finalize();
				SCENE_MGR::Loser = SCENE_MGR::HomeIndex;
				SCENE_MGR::Victor = SCENE_MGR::AwayIndex;
				break;
			}
		}
		RegentDeathTest();
	}
	else
	#endif
	{
		SCENE_MGR::Loser = FixUnitsArray(SCENE_MGR::HomeIndex);
		SCENE_MGR::Victor = FixUnitsArray(SCENE_MGR::AwayIndex);
	}
		
	// now fix the units array
	SCENE_MGR::LoserRealm =	units[SCENE_MGR::Loser].Realm;
	SCENE_MGR::VictorRealm = units[SCENE_MGR::Victor].Realm;
	
	SCENE_MGR::mfReleaseSceneToMap(0,0);
}

/* ========================================================================
   Function    - PaintBattleLose
   Description - paint the text on the help menu
   Returns     -
   ======================================================================== */
void PaintBattleLose (LONG MenuCombo, LONG )
{	
	SHORT	i,j;
	LONG	X,Y;
	LONG	mx,my;
	LONG	MenuId, ButtonId;
	CHAR	color = 1;
	CHAR	temp[256];
	CHAR	temp2[256];
	CHAR	buffer[256];
	SHORT	UnitIndex;
	SHORT	Count = 0;
	char RealmName[80];
	
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	// get position of lose screen
	if( GetButtonPosition( MenuId, 0, &mx, &my ) == fERROR)
		return;
		
	X = mx + 60;
	Y = my + 20;
	
 	// paint dialog title
	strncpy(buffer, STRMGR_GetStr(STR_DEFEAT), sizeof(buffer));
	gprint_text(X,Y,buffer, 31);
	
	/* -----------------------------------------------------------------
	   paint the realm we lost to
	   ----------------------------------------------------------------- */

	X = mx + 15;
	Y = my + 80;
	
	// build some text that says why you lost
	strcpy(RealmName, realm[sAwayRealm].mfGetName());
		
	sprintf(buffer, STRMGR_GetStr(BTLUI_LOOSE_TEXT), RealmName);
	
	init_gfont(FONT_SANS_12PT);
	
	gprint_text(X,Y,buffer ,color);
	
	/* -----------------------------------------------------------------
	   paint the list of my units and there status
	   ----------------------------------------------------------------- */

	init_gfont(FONT_SANS_12PT);
	Y += 50;
	
	for (UnitIndex = SCENE_MGR::HomeIndex;
		 UnitIndex != fERROR;
		 UnitIndex = (SHORT) units[UnitIndex].NextUnit)
	{
		if(++Count > gUnitStackSize)
			break;
			
		if(units[UnitIndex].iHandle == fERROR)
			continue;
			
		CAvatar * const pAvatar = (CAvatar * const) BLKPTR(units[UnitIndex].iHandle);
		
		for(i=0;i<MAX_RESERVES;i++)
		{
			if(Reserves[1][i] == pAvatar->hThis)
				break;
		}
		
		// if not found in the reserves, there are dead
		if(i == MAX_RESERVES)
		{
			// regents don't die this easly
			if (IsRegent(pAvatar->UnitIndex))
			{
				strcpy(temp,STRMGR_GetStr(BTLUI_WIN_UNIT_TEXT));
				sprintf(temp2, "%s",
					STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon])); 
				sprintf(buffer, temp,
					temp2,
					STRMGR_GetStr(BTLUI_LOOSE_RESERVES));
			}
			else
			{
				if( pAvatar->Status == CAvatar::AI_STONE )
				{
					strcpy(temp,STRMGR_GetStr(BTLUI_LOOSE_UNIT_TEXT));
					sprintf(temp2, "%s",
						STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon]));
					sprintf(buffer, temp,
						temp2,
						STRMGR_GetStr(BTLUI_COLLAPSE));
				}
				else
				{
					strcpy(temp,STRMGR_GetStr(BTLUI_LOOSE_UNIT_TEXT));
					sprintf(temp2, "%s",
						STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon]));
					sprintf(buffer, temp,
						temp2,
						STRMGR_GetStr(BTLUI_LOOSE_DEAD));
					// make sure they are dead
					pAvatar->Status = CAvatar::AI_DEAD;
				}
			}
		}
		// else they survived in the reserves
		else
		{
			if( pAvatar->Status == CAvatar::AI_STONE )
			{
				strcpy(temp,STRMGR_GetStr(BTLUI_LOOSE_UNIT_TEXT));
				sprintf(temp2, "%s",
					STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon]));
				sprintf(buffer, temp,
					temp2,
					STRMGR_GetStr(BTLUI_COLLAPSE));
			}
			else
			{
				strcpy(temp, STRMGR_GetStr(BTLUI_LOOSE_UNIT_TEXT));
				sprintf(temp2, "%s",
					STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon]));
				sprintf(buffer, temp,
					temp2,
					STRMGR_GetStr(BTLUI_LOOSE_RESERVES));
			}
		}
		
		gprint_text(X,Y,buffer ,color);
		Y += 13;
	}
	
	// paint the control buttons
	init_gfont(FONT_TITL_10PT);
		
	// get position of Done button
	if( GetButtonPosition( MenuId, 2, &mx, &my ) == fERROR)
		return;
		
	X = mx;
	Y = my;
	
	// get size of Done Button
	if( GetButtonSize( MenuId, 2, &mx, &my ) == fERROR)
		return;
		
	X += mx/2;
	Y += my/2;
	
	strcpy(buffer,STRMGR_GetStr(STR_OK));
	print_text_centered(X,Y,buffer,WHITE);
}

/* ========================================================================
   Function    - BattleWin
   Description - Tell you that you lost the battle
   Returns     -
   ======================================================================== */
void BattleWin(LONG, LONG)
{
	LONG	regentUnit;
	
	fUpdatePanels = TRUE;
	RunPanels();
	if(BtlVictory == FALSE)
	{
		BtlVictory = TRUE;
		AddSndObj(SND_VICTORY1, SND_VICTORY_TOTAL,VOLUME_PRIORITY);
		SendWin ( TRUE );
		CheckDefeatOfGorgon();
	}
	CollapseUnits(SCENE_MGR::HomeIndex);
	CollapseUnits(SCENE_MGR::AwayIndex);
	
	BattleExpPoints( SCENE_MGR::HomeIndex, BTL_EXP_WIN, 0, FALSE );
	if ( !HomeStrengthLarger )
		BattleExpPoints( SCENE_MGR::HomeIndex, BTL_EXP_WIN_GREATER_STRENGTH, 0, FALSE );
	
	VictoryScreen(PaintBattleWin, BattleWinProc);
}

/* ========================================================================
   Function    - CheckDefeatOfGorgon
	Description - check for a win over the Gorgon and get his banner
	Returns     - 
	======================================================================== */
void CheckDefeatOfGorgon( void )
{
	SHORT	regentUnit;
	SHORT	hPlayerStats;
	
	// if you defeated the Gorgon, you get a special bonus worth 100 pts
	if(units[SCENE_MGR::AwayIndex].Icon == GORGON_UNIT_ICON)
	{
		// find the regent of this country
		regentUnit = RegentUnitOfRealm((REALM::REALM_TYPE)units[SCENE_MGR::HomeIndex].Realm);
		if(regentUnit != 0)
		{
			// get the player stats for this regent and give him the banner
			hPlayerStats = LoadStats(units[regentUnit].id,fERROR);
			if (hPlayerStats != fERROR)
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlrStat(hPlayerStats);
				pPlrStat->Inventory.mfAddItem(ITEM_GORGONS_BANNER);
			}
		}
	}
}

/* ========================================================================
   Function    - BattleWinProc
   Description -
   Returns     -
   ======================================================================== */
void BattleWinProc(LONG MenuCombo, LONG button)
{
	SHORT i,j;
	
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
	
	// now kill off anyone from there team left on the battle field
	for(i=0;i<3;i++)
	{
		for(j=0;j<5;j++)
		{
			if(GetWhosWhere(i,j,WW_AWAY) != fERROR)
			{
				CAvatar * const pAvatar = (CAvatar * const) BLKPTR(GetWhosWhere(i,j,WW_AWAY));
				// make sure they are dead
				pAvatar->Status = CAvatar::AI_DEAD;
			}
		}
	}
			
	SCENE_MGR::Victor = FixUnitsArray(SCENE_MGR::HomeIndex);
	SCENE_MGR::Loser = FixUnitsArray(SCENE_MGR::AwayIndex);
	
	// now fix the units array
	SCENE_MGR::VictorRealm = units[SCENE_MGR::Victor].Realm;
	SCENE_MGR::LoserRealm =	units[SCENE_MGR::Loser].Realm;
	
#ifdef _WINDOWS
	if (IsMultiPlayer())
	{
		AMultiPlayer.SendEvent (NPCBATTLE_ACTION,
			SCENE_MGR::Loser, SCENE_MGR::Victor, 0, 0, sAwayRealm);
			
		AMultiPlayer.InitEvent(0);
	}
#endif

	SCENE_MGR::mfReleaseSceneToMap(0,0);
}

/* ========================================================================
   Function    - PaintBattleWin
   Description - paint the text on the help menu
   Returns     -
   ======================================================================== */
void PaintBattleWin (LONG MenuCombo, LONG )
{	
	SHORT	i,j;
	LONG	X,Y;
	LONG	mx,my;
	LONG	MenuId, ButtonId;
	CHAR	color = 1;
	CHAR	temp[256];
	CHAR	temp2[256];
	CHAR	buffer[256];
	SHORT	PFBM, Troops, dummy;
	SHORT	UnitIndex;
	SHORT	Count = 0;
	//CAvatar *pAvatar;
	char RealmName[80];
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	// get position of win screen
	if( GetButtonPosition( MenuId, 0, &mx, &my ) == fERROR)
		return;
		
	X = mx + 60;
	Y = my + 20;

	// paint dialog title
	strncpy(buffer, STRMGR_GetStr(STR_VICTORY), sizeof(buffer));
	gprint_text(X,Y,buffer, 31);
	
	/* -----------------------------------------------------------------
	   paint the realm we won to
	   ----------------------------------------------------------------- */

	X = mx + 15;
	Y = my + 80;
	
	UnitIndex = SCENE_MGR::AwayIndex;
	// GWP Unused pAvatar = (CAvatar *) BLKPTR(units[UnitIndex].iHandle);
	
	// calculate how many unit they lost
	PFBM = 0;
	for(i=0;i<MAX_RESERVES;i++)
	{
		if(Reserves[0][i] != fERROR)
			PFBM++;
	}
	PFBM = gAwayUnits - PFBM;
	
	strcpy(RealmName, realm[sAwayRealm].mfGetName());
	sprintf(buffer, STRMGR_GetStr(BTLUI_WIN_TEXT), RealmName, PFBM);
	
	init_gfont(FONT_TITL_16PT);
	
	gprint_text(X,Y,buffer ,color);
	
	/* -----------------------------------------------------------------
	   paint the tree of my units and there status
	   ----------------------------------------------------------------- */

	init_gfont(FONT_SANS_12PT);
	Y += 64;
	
	// convert the game avatars to members of an adventuer
	for (UnitIndex = SCENE_MGR::HomeIndex;
		 UnitIndex != fERROR;
		 UnitIndex = (SHORT) units[UnitIndex].NextUnit)
	{
		if(++Count > gUnitStackSize)
			break;
			
		if(units[UnitIndex].iHandle == fERROR)
			continue;
			
		CAvatar * const pAvatar = (CAvatar * const) BLKPTR(units[UnitIndex].iHandle);
		
		if(pAvatar->Status == CAvatar::AI_DEAD)
		{
			strcpy(temp,STRMGR_GetStr(BTLUI_WIN_UNIT_TEXT));
			sprintf(temp2, "%s",
				STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon])); 
			sprintf(buffer, temp,
				temp2,
				STRMGR_GetStr(BTLUI_WIN_DEAD));
		}
		else
		if( pAvatar->Status == CAvatar::AI_STONE )
		{
			strcpy(temp,STRMGR_GetStr(BTLUI_LOOSE_UNIT_TEXT));
			sprintf(temp2, "%s",
				STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon]));
			sprintf(buffer, temp,
				temp2,
				STRMGR_GetStr(BTLUI_COLLAPSE));
			#ifdef _FOREIGNVER
				Y += 13;
			#endif
		}
		else
		{
			strcpy(temp, STRMGR_GetStr(BTLUI_WIN_UNIT_TEXT));
			sprintf(temp2, "%s",
				STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon]));
			sprintf(buffer, temp,
				temp2,
				STRMGR_GetStr(BTLUI_WIN_SURVIVED));
		}
		
		gprint_text(X,Y,buffer ,color);
		Y += 13;
	}
	
	// paint the control buttons
	init_gfont(FONT_TITL_10PT);
		
	// get position of Done button
	if( GetButtonPosition( MenuId, 2, &mx, &my ) == fERROR)
		return;
		
	X = mx;
	Y = my;
	
	// get size of Done Button
	if( GetButtonSize( MenuId, 2, &mx, &my ) == fERROR)
		return;
		
	X += mx/2;
	Y += my/2;
	
	strcpy(buffer,STRMGR_GetStr(STR_OK));
	print_text_centered(X,Y,buffer,WHITE);
}

/* #################################################################
   #################################################################
   #################################################################
   
        Retreat Dialog Routines
   
   #################################################################
   #################################################################
   ################################################################# */

/* ========================================================================
   Function    - BattleRetreat
   Description - change the camera position mode
   Returns     -
   ======================================================================== */
void BattleRetreat(LONG, LONG)
{
	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
	if(mouse_button == 2)
	{
		//BattleHelpDialog(0,STR_BTLUI_HELP_RETREATBUTTON);
		SystemHelp(STR_BTLUI_HELP_RETREAT_TITLE,STR_BTLUI_HELP_RETREAT_TEXT, H_Retreat, NULL, 0);
	}
	else
	{
		// set the proc to null
		SetButtonProc  (D_QUESTION2, 0, NULL, 0, 0 );
		
		// set the question text
		SetButtonLabel  (D_QUESTION2, QUESTION_TEXT, BTLUI_SURRENDER_TEXT, BLACK );
		
		// turn on the first button as YES
		SetButtonLabel  (D_QUESTION2, QUESTION_BTN1, STR_YES, BTN_LABEL_COLOR );
		SetButtonProc   (D_QUESTION2, QUESTION_BTN1, BattleRetreatProc, QUESTION_BTN1, D_KEY_YES );
		
		// turn on the last button as CANCEL
		SetButtonLabel  (D_QUESTION2, QUESTION_BTN2, STR_CANCEL, BTN_LABEL_COLOR );
		SetButtonProc   (D_QUESTION2, QUESTION_BTN2, BattleRetreatProc, QUESTION_BTN2, D_KEY_CANCEL );
		
		ShowMenu(D_QUESTION2);
	}
}

/* ========================================================================
   Function    - BattleRetreatProc
   Description -
   Returns     -
   ======================================================================== */
void BattleRetreatProc(LONG MenuCombo, LONG button)
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
	
	if(button == QUESTION_BTN1)
	{
		BtlWithdraw = TRUE;
		SendWin ( FALSE );
		BattleLoose(0,0);
	}
}


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void BattleSelMissile(LONG , LONG )
{
	// bad function!
}
/* ========================================================================
   Function    - CheckBattleGrid
   Description - 
   Returns     - 
   ======================================================================== */

static void CheckBattleGrid()
{
	LONG i;
	SCENE const * const pScene = (SCENE const * const) BLKPTR(SCENE_MGR::hCurrentScene);
	
	for (i = 0; i < MAX_AVATARS; i++)
	{
		if (pScene->Avatars[i] != fERROR)
		{
			CAvatar *pAvatar = (CAvatar *) BLKPTR(pScene->Avatars[i]);
			if (pAvatar->GetAIFuncIndex() == CAvatar::AI_FUNC_BATTLE_CAPTAIN)
			{
				// If we're not dead we belong on the grid.
				if (pAvatar->hThis != battleTroops 
				    && !pAvatar->mfAmIImmoblized() 
				    && pAvatar->fBtlCap.HitPoints > 0)
				{
					LONG r;
					BOOL Found = FALSE;
					
					// Search the Grid
					for (r = 0; r < GRID_MAX_ROWS && !Found; r++)
					{
						LONG c;
						for (c = 0; c < GRID_MAX_COLS && !Found; c++)
						{
							if (pAvatar->mfIsHomeTeam() )
							{
							    if( GetWhosWhere(r,c,WW_HOME) == pAvatar->hThis)
							    {
									Found = TRUE;
								}
							}
							else
							{
							    if (GetWhosWhere(r, c, WW_AWAY) == pAvatar->hThis)
							    {
							    	Found = TRUE;
								}
							}
						}
					}
					
					// Search the Reserves
					for (LONG j = 0; j< MAX_RESERVES; j++)
					{
						if (Reserves[pAvatar->mfIsHomeTeam()][j] == pAvatar->hThis)
						{
							Found = TRUE;
						}
					}
					
					// -- This is healing code to correct any avatar who falls
					// -- out of the grid system.
					if (!Found)
					{
						//GEH fatal_error("ERROR! Lost Battle captain on the grid.\n");
						SendToReserves(pAvatar);
						BattleMoveToReservesArea(pAvatar->hThis);
						BattlePutInReserves(pAvatar->mfIsHomeTeam(), pAvatar->hThis);
						fUpdatePanels = TRUE;
					}
				}
			}
		}
	}
}

/* ========================================================================
   Function    - BattleNextTurn
	Description - In RPG mode, start next turn
	Returns     - void
	======================================================================== */
void BattleNextTurn (LONG, LONG)
{
	fBattleFreezeTime = FALSE;
	BattleTick = BattleTick + BtlActionRate;
	BattleUpdateTime();
	fBattleNextTurn = 10;
	
	// -- Tell all avatars it's the next turn
	SCENE const * const pScene = (SCENE const * const) BLKPTR(SCENE_MGR::hCurrentScene);
	
	for (LONG i = 0; i < MAX_AVATARS; i++)
	{
		if (pScene->Avatars[i] != fERROR)
		{
			CAvatar *pAvatar = (CAvatar *) BLKPTR(pScene->Avatars[i]);
			// -- use up this turn
			pAvatar->fBtlCap.TurnAction = TRUE;
		}
	}						
}

