/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _BATTLEUI_HXX
#define _BATTLEUI_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#if !defined(_SYSTEM_H)
#include "system.h"
#endif

#if !defined(_AVATAR_HXX)
#include "avatar.hxx"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define GRID_MAX_ROWS		3
#define GRID_MAX_COLS		5

#define FIELDGRID11_Y	-1152
#define GRID11_Y		-960		// y is from my reserves forward
#define FIELDGRID12_Y	-768
#define GRID12_Y		-576
#define FIELDGRID21_Y	-384
#define GRID21_Y		-192
#define FIELDGRID22_Y	0
#define GRID22_Y	 	192
#define FIELDGRID31_Y	384
#define GRID31_Y	 	576
#define FIELDGRID32_Y	768
#define GRID32_Y	 	960
#define FIELDGRID41_Y	1152

#define FIELDGRID1_X	-1920		// x is left to right
#define	GRID1_X			-1536
#define FIELDGRID2_X	-1152
#define GRID2_X			-768
#define FIELDGRID3_X	-384
#define GRID3_X		 	0
#define FIELDGRID4_X	384
#define GRID4_X		 	768
#define FIELDGRID5_X	1152
#define GRID5_X		 	1536
#define FIELDGRID6_X	1920

#define GRID_WIDTH	768

#define MY_RESERVES_Y		-2500
#define ENEMY_RESERVES_Y	 2500

// panel graphics defines
#define BTLUI_PANEL_HEIGHT	164

#define MAX_RESERVES 		20
#define RESV_TITLE_X		7
#define RESV_TITLE_Y		5
#define RESV_TITLE_W		323
#define RESV_TITLE_H		20

#define BTLUI_SELTROOP_X	8
#define BTLUI_SELTROOP_Y	6

#define	BTLUI_TEXT_CX		160

#define BTLUI_GRID_W		55
#define BTLUI_GRID_H		45
#define BTLUI_GRID_HALF		22
#define BTLUI_GRID_X1		338
#define BTLUI_GRID_X2		398
#define BTLUI_GRID_X3		458
#define BTLUI_GRID_X4		518
#define BTLUI_GRID_X5		578
#define BTLUI_GRID_Y1		5
#define BTLUI_GRID_Y2		49
#define BTLUI_GRID_Y3		93

#define BTLUI_STATUS_AWAY_PICTURE_X	8
#define BTLUI_STATUS_AWAY_PICTURE_Y	33
#define BTLUI_STATUS_AWAY_PICTURE_W	66
#define BTLUI_STATUS_AWAY_PICTURE_H	100
#define BTLUI_STATUS_AWAY_NAME_X 	9
#define BTLUI_STATUS_AWAY_NAME_Y 	8
#define BTLUI_STATUS_AWAY_REALM_X 	81
#define BTLUI_STATUS_AWAY_REALM_CX 	168
#define BTLUI_STATUS_AWAY_REALM_Y 	24
#define BTLUI_STATUS_AWAY_REALM_CY 	32
#define BTLUI_STATUS_AWAY_FIELD_X	81
#define BTLUI_STATUS_AWAY_FIELD_Y 	38
#define BTLUI_STATUS_AWAY_RESV_X 	81
#define BTLUI_STATUS_AWAY_RESV_Y 	52

#define BTLUI_STATUS_HOME_PICTURE_X	265
#define BTLUI_STATUS_HOME_PICTURE_Y	7
#define BTLUI_STATUS_HOME_PICTURE_W	66
#define BTLUI_STATUS_HOME_PICTURE_H	100
#define BTLUI_STATUS_HOME_NAME_X 	81
#define BTLUI_STATUS_HOME_NAME_RX 	328
#define BTLUI_STATUS_HOME_NAME_Y 	116
#define BTLUI_STATUS_HOME_REALM_X 	81
#define BTLUI_STATUS_HOME_REALM_CX 	168
#define BTLUI_STATUS_HOME_REALM_Y 	74
#define BTLUI_STATUS_HOME_REALM_CY 	82
#define BTLUI_STATUS_HOME_FIELD_X	81
#define BTLUI_STATUS_HOME_FIELD_Y 	88
#define BTLUI_STATUS_HOME_RESV_X 	81
#define BTLUI_STATUS_HOME_RESV_Y	102

#define BTLUI_UNITINFO_PICTURE_X	8
#define BTLUI_UNITINFO_PICTURE_Y	8
#define BTLUI_UNITINFO_PICTURE_W	100
#define BTLUI_UNITINFO_PICTURE_H	110

#define BTLUI_UNITINFO_NAME_X		BTLUI_UNITINFO_PICTURE_W+8
#define BTLUI_UNITINFO_NAME_Y		8
#define BTLUI_UNITINFO_NAME_W		100

#define BTLUI_UNITINFO_BAR_X		BTLUI_UNITINFO_PICTURE_W+16
#define BTLUI_UNITINFO_BAR_Y		30
#define BTLUI_UNITINFO_BAR_W		100
#define BTLUI_UNITINFO_BAR_H		12

#define BTLUI_UNITINFO_HEALTH_X	BTLUI_UNITINFO_PICTURE_W+16
#define BTLUI_UNITINFO_HEALTH_Y	50

#define BTLUI_UNITINFO_STATUS_X	BTLUI_UNITINFO_PICTURE_W+16
#define BTLUI_UNITINFO_STATUS_Y	65

#define BTLUI_UNITINFO_INFO_X1	BTLUI_UNITINFO_PICTURE_W+16
#define BTLUI_UNITINFO_INFO_Y1	80
#define BTLUI_UNITINFO_INFO_H		15
#define BTLUI_UNITINFO_INFO_X2	BTLUI_UNITINFO_PICTURE_W+100+16
#define BTLUI_UNITINFO_INFO_Y2	80
 
#define BTL_RESV_ID					100
#define BTL_STAT_ID					101
#define BTL_CHAT_ID					102
#define BTL_EXIT_ID					103
#define BTL_VIEW_ID					104
#define BTL_FOLW_ID					105
#define BTL_SHOOT_ID					106
#define BTL_MAGIC_ID					107
#define BTL_CANCEL_ID				108
#define BTL_FALLBACK_ID				109
#define BTL_LIGHT_ID					110
#define BTL_DONE_ID					111

#define BTLUI_HOME_HILIGHT			"UI\\HGRID0H.PCX"
#define BTLUI_AWAY_HILIGHT			"UI\\AGRID0H.PCX"
#define BTLUI_BOTH_HILIGHT			"UI\\BGRID0H.PCX"
#define BTLUI_EXCEPT_HILIGHT		"UI\\XGRID0.PCX"
#define BTLUI_ANIM_HILIGHT			"UI\\LIGHT.PCX"

#define BTLUI_BATLW0				"UI\\BATLW0.PCX"
#define BTLUI_BATLW1				"UI\\BATLW1.PCX"
#define BTLUI_BATLW2				"UI\\BATLW2.PCX"
#define BTLUI_BATLW3				"UI\\BATLW3.PCX"

#define BTLUI_BATLP0				"UI\\BATLW0.PCX"
#define BTLUI_BATLP1				"UI\\BATLW1.PCX"
#define BTLUI_BATLP2				"UI\\BATLW2.PCX"
#define BTLUI_BATLP3				"UI\\BATLW3.PCX"

#define BTLUI_BATLM0				"UI\\BATLM0.PCX"
#define BTLUI_BATLM1				"UI\\BATLM1.PCX"
#define BTLUI_BATLM2				"UI\\BATLM2.PCX"
#define BTLUI_BATLM3				"UI\\BATLM3.PCX"

#define BTLUI_BATLS0				"UI\\BATLS0.PCX"
#define BTLUI_BATLS1				"UI\\BATLS1.PCX"
#define BTLUI_BATLS2				"UI\\BATLS2.PCX"
#define BTLUI_BATLS3				"UI\\BATLS3.PCX"

#define BTLUI_BATLPNL1				"UI\\BATLPNL1.PCX"
#define BTLUI_BATLPNL2				"UI\\BATLPNL2.PCX"

#define BTLUI_PATH_HELP_POINTS_LEFT		"UI\\BTLHLPL.PCX"
#define BTLUI_PATH_HELP_POINTS_RIGHT	"UI\\BTLHLPR.PCX"

#define BTLUI_HELP_LEFT_X			320
#define BTLUI_HELP_LEFT_Y			320
#define BTLUI_HELP_RIGHT_X			16
#define BTLUI_HELP_RIGHT_Y			320
#define BTLUI_HELP_W				320
#define BTLUI_HELP_H				148
#define BTLUI_HELP_ADJ				34

#define BTLUI_HELP_POINTS_LEFT		0
#define BTLUI_HELP_POINTS_RIGHT		1


const DWORD kNoMultiId = 777777777;			//---- No player here in dwPlayers[]


/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
typedef enum {
	CAMERA_BEHIND = 0,
	CAMERA_LEFT,
	CAMERA_RIGHT,
	CAMERA_AHEAD,
	CAMERA_ZOOM_BEHIND,
	CAMERA_ZOOM_LEFT,
	CAMERA_ZOOM_RIGHT,
	CAMERA_ZOOM_AHEAD,
}BTLUI_CAMERA_MOVEMENT;

enum {
	BTLUI_MODE_RESERVES = 0,
	BTLUI_MODE_STATUS,
	BTLUI_MODE_UNITINFO,
	BTLUI_MODE_MISSILE,
	BTLUI_MODE_MOVE,
	BTLUI_MODE_MAGIC,
	BTLUI_MODE_FALLBACK,
};

enum {
	BTLUI_CAMERA_FIXED = 0,
	BTLUI_CAMERA_FOLLOW,
};

enum {
	TEST_FIELD = 0,
	TEST_RESERVES,
};

enum {
	BTLUI_EXCEPT_NORMAL = 0,	// all's cool
	BTLUI_EXCEPT_IMPASSIBLE,	// can't get through this way
};
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
typedef SHORT (*PFGRIDTEST)(LONG,LONG,LONG,LONG,BOOL*);

typedef struct _GRID {
	SHORT	Home;
	SHORT	Away;
} GRID, *PGRID;

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

void BattleNextTurn (LONG, LONG);

void InitBattleUI(SCENE &rScene);
void ReleaseBattleUI(SCENE &rScene);

void ChooseBattleField( LONG MenuCombo, LONG WhichField );
CHAR BattleChooseField( CHAR TerrainType );

void BattlePanelMode( LONG MenuCombo, LONG WhichMode );

void BattleChatMode( LONG MenuCombo, LONG WhichMode );
void BattleReservesMode( LONG, LONG );
void BattleStatusMode( LONG, LONG );
void BattleRetreat(LONG, LONG);
void BattleRetreatProc(LONG MenuCombo, LONG button);
void BattleLoose(LONG, LONG);
void BattleLooseProc(LONG MenuCombo, LONG button);
void BattleWin(LONG, LONG);
void BattleWinProc(LONG MenuCombo, LONG button);
void CheckDefeatOfGorgon( void );

void BattleSelMissile(LONG MenuCombo, LONG );
void BattleSelMagic(LONG MenuCombo, LONG );

void BattleControlGrid(LONG,LONG location);
void BattleMoveCamera();

void BattleControlPaint(LONG,LONG MenuId);
void BattleStatusPaint(LONG);
void BattleUnitInfoPaint(LONG MenuCombo);
void BattleSelMissilePaint(LONG MenuCombo);
void BattleFallBackPaint(LONG MenuCombo);
void BattleSelTroopsPaint(LONG,LONG MenuId);
void BattleMissilePaint(LONG MenuCombo);
void BattleSelUnitPaint(LONG MenuCombo);
void BattleMovePaint(LONG MenuCombo);

void BattleCursorPaint();
void BattleReservesGrid(LONG,LONG MenuId);
SHORT BattlePutInReserves(BOOL HomeTeam, SHORT hAvatar);
void BattleIconPaint(SHORT hAvatar, SHORT x, SHORT y);

SHORT BattleLookAround(
	LONG		MenuId, 
	SHORT		Row, 
	SHORT		Column, 
	SHORT		Movement,
	SHORT		FindEnemy,
	PFGRIDTEST	pfGridFunc
);

void BattleMissile(LONG , LONG MenuCombo);
void BattleMagic(LONG , LONG MenuCombo);
void BattleSelMissile(LONG MenuCombo, LONG );
BOOL BattleMagicReady(CAvatar *pAvatar);

// PFGRIDTEST type functions
SHORT HilightHomeMovement(LONG MenuId,LONG R,LONG C,LONG, BOOL *T);
SHORT HilightMissile( LONG MenuId, LONG R, LONG C, LONG, BOOL *T );
void HilightUnit(void);
SHORT CheckEnemy( LONG MenuId, LONG R, LONG C, LONG, BOOL *T );
void HilightMissileRange(void);
void HilightMyTroops(void);

BOOL CheckMissileRangeSub(SHORT R, SHORT C);

void HilightMissileShooter(void);
void HilightMagicShooter(void);
SHORT HilightShooter( LONG MenuId, LONG R, LONG C, LONG, BOOL *T );

void BattleCancel(LONG, LONG);
void BattleFallBack(LONG, LONG);

void BattleCancelMissile(LONG MenuCombo, LONG);
void BattleCancelMove(LONG MenuCombo, LONG);
void BattleCancelSelUnit(LONG, LONG);

void BattleCameraHighView(LONG, LONG);
void BattleCameraFollow(LONG, LONG);

void BattleHealthTest (
	SHORT TestType, 
	SHORT Away, 
	SHORT *UnitCount, 
	SHORT *UnitAlive, 
	SHORT *UnitTotal
);

void PaintChooseBattleField(LONG, LONG);

void PaintBattleLose (LONG MenuCombo, LONG );
void PaintBattleWin (LONG MenuCombo, LONG );
SHORT FixUnitsArray( SHORT UnitIndex );

void BtlRollDice(SHORT X, SHORT Y);
void PaintBattleDice(LONG, LONG);

void BattleHelpDialog(LONG, LONG StrMgrText);
void BattleHelpPaint (LONG MenuCombo, LONG StrMgrText);

#define	WW_HOME	TRUE
#define WW_AWAY	FALSE
SHORT	GetWhosWhere(SHORT Row, SHORT Col, BOOL Home);
void	SetWhosWhere(SHORT Row, SHORT Col, BOOL Home, SHORT hAvatar);

/* -----------------------------------------------------------------
   Globals
   ----------------------------------------------------------------- */
extern SHORT	sBattleMode;
extern SHORT	sOldBattleMode;
extern SHORT	hUnitInfo;
extern DECL_MATRIX_CLASS_S(SHORT,BtlExceptions,GRID_MAX_ROWS,GRID_MAX_COLS);
extern DECL_MATRIX_CLASS_S(SHORT,Reserves,2,MAX_RESERVES);
extern DECL_VECTOR_CLASS_S(SHORT,BtlGridX,GRID_MAX_COLS);		// icon grid coords
extern DECL_VECTOR_CLASS_S(SHORT,BtlGridY,GRID_MAX_ROWS);		// icon grid coords
extern int		BtlSoundTag;
extern LONG		BtlActionRate;		// ticks per battle action round
extern LONG		DifficultyLevel;	// 1 is most difficult
extern LONG		SpeedLevel;			// 1 is fastest
extern BOOL		fBtlTurnBased;		// turn based battles
extern BOOL 	fBattleNextTurn;	// countdown to end of turn

extern SHORT	gAwayUnits;

extern BOOL		fBtlMagicType;		// type of magic set in Inventory Menu
extern BOOL		fBtlMagicResult;	// battle card result set in Inventory Menu

extern BOOL		fBattleStarted;		// is a battle officially under way

extern BOOL		fBattleCombat;		// tell the world if were on the field

#endif
