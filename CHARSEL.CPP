/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: CharSel.cpp
   Author:   Michael Branham

   ========================================================================
   Contains the following general functions:

   void DomainNewGame (LONG, LONG)
   void DisplayRealmSelection(void)
   void SelectRegent(LONG, LONG)
   void EndSelectDomain(void)
   void BeginRule(LONG, LONG)
   void SelectAdvParty(void)

   ®RM250¯======================================================================= */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#ifdef _WINDOWS
#include <windows.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <io.h>
#include <stdlib.h>

#include "typedefs.h"

#include "system.h"
#include "charsel.hxx"
#include "game.h"
#include "gamemap.hxx"
#include "gametype.hxx"
#include "handle.hxx"
#include "invngui.hxx"
#include "loadsave.hxx"
#include "mapai.hxx"
#include "multiui.hxx"
#include "multimap.hxx"
#include "regents.hxx"
#include "menu.h"
#include "panel.h"
#include "request.h"
#include "playstat.hxx"
#include "realm.hxx"
#include "report.hxx"
#include "sound.hxx"
#include "strenum.h"
#include "strmgr.h"
#include "units.hxx"
#include "context.hxx"
#include "provtax.h"
#include "places.hxx"

#ifdef _WINDOWS
#include "winsys\mulplay.hxx"
#include "winsys\mono_c.h"
#include "regrelic.hxx"					// [d5-30-97 JPC] lieutenant relics
#endif

/* ------------------------------------------------------------------------
   Defines
   ------------------------------------------------------------------------ */
#define REPORT_MODE				9
#define REPORT_DONE_MODE		10

#define BEFORE_LTACTION			0
#define DURING_LTACTION         1
#define AFTER_LTACTION          2
#define BLACK       1
#define DKBLUE      60
#define BLUE        64
#define DKPURPLE    90
#define LTPURPLE    103
#define DKRED       112
#define MDRED       120
#define DKBROWN     128
#define MDBROWN     136
#define LTBROWN     140
#define ORANGE      142
#define MDYELLOW    168
#define LTYELLOW    175
#define DKGREEN     184
#define MDGREEN     188
#define LTGREEN     192
#define LTTAN       223
#define WHITE       31

#define ADV_NONE        0
#define ADV_GETPARTY    1
#define ADV_GETSITE     2
#define ADV_SITE_DESCR  3
#define ADV_SITE_GO     4

#define ADVENTURER_LIST_SIZE 40

#define DATA_AREA_ACTN  144
#define DATA_AREA_MIN   192
#define DATA_AREA_MAX   352
#define DATA_AREA_MID   216

#define ACTIVATE_REGION_(a,b)		activate_region((a),(region_state[a-regFIRST_REGION_STATE]=(b)))
#define RESTORE_REGION_STATE(a) activate_region((a),region_state[a-regFIRST_REGION_STATE])

DEFINE_VECTOR_DATA(CSTRPTR, szRealmTextFile) = {
	/* no country	   */	"",
	/* Aerenwe		   */	"aerenwe",
	/* Alamie		   */	"alamie",
	/* Anuire		   */	"",
	/* Avanil		   */	"",
	/* Baruk-Azhik	   */	"barukazh",
	/* Boeruine		   */	"",
	/* Brosengae	   */	"brosenga",
	/* Cariele		   */	"cariele",
	/* Chimaeron	   */	"",
	/* Coeranys		   */	"coeranys",
	/* Dhoesone		   */	"dhoesone",
	/* Diemed		   */	"diemed",
	/* Elinie		   */	"elinie",
	/* Endier		   */	"endier",
	/* Five Peaks	   */	"",
	/* Ghoere		   */	"",
	/* Gorgon's Crown  */	"",
	/* Ilien		   */	"ilien",
	/* Markazor		   */	"",
	/* Medoere		   */	"medoere",
	/* Mhoried		   */	"",
	/* Mieres		   */	"",
	/* Monsters		   */	"",
	/* Mur-Kilad	   */	"",
	/* Osoerde		   */	"",
	/* Rhuobhe		   */	"",
	/* Roesone		   */	"roesone",
	/* Sielwode		   */	"sielwode",
	/* Spiderfell	   */	"",
	/* Taeghas		   */	"taeghas",
	/* Talinie		   */	"talinie",
	/* Thurazor		   */	"",
	/* Tuarhievel	   */	"tuarhiev",
	/* Tuornen		   */	"tuornen",
};
DEFINE_VECTOR_CLASS(CSTRPTR, szRealmTextFile);

DEFINE_STATIC_VECTOR_DATA(CSTRPTR, szRegentTextFile) = {
	/* dummy               */	"",
	/* Liliene Swordwraith  */	"swordwra",
	/* Cole Alwier		   */	"alwier",
	/* Carilon Alam		   */	"calam",
	/* Dierdren Alam	   */	"dalam",
	/* Caliedhe Dosiere	   */	"",
	/* unknown			   */	"",
	/* Darien Avan		   */	"",
	/* Dheraene Bhailie	   */	"",
	/* Grimm Graybeard	   */	"graybear",
	/* Diirk Watershold	   */	"watersho",
	/* Aeric Boeruine	   */	"",
	/* Innes			   */	"",
	/* Eriene Mierelen	   */	"mierelen",
	/* Hyde Termonie	   */	"termonie",
	/* Entier Gladanil	   */	"gladanil",
	/* Mheallie Bireon	   */	"bireon",
	/* Danita Chimaera	   */	"",
	/* Denerik			   */	"",
	/* Eluvie Cariele	   */	"ecariele",
	/* Medhlorie Haensen   */	"haensen",
	/* Fhiele Dhoesone	   */	"fdhoeson",
	/* Clumine Dhoesone	   */	"cdhoeson",
	/* Heirl Diem		   */	"hdiem",
	/* Lasica Diem		   */	"ldiem",
	/* Assan ibn Daouta	   */	"adaouta",
	/* Kalilah bint Daouda */	"kdaouda",
	/* Guilder Kalien	   */	"kalien",
	/* Caine			   */	"caine",
	/* The Eyeless One	   */	"",
	/* The Wizard		   */	"",
	/* Gavin Tael		   */	"",
	/* The Sword Mage	   */	"",
	/* The Gorgon		   */	"",
	/* Kiras Earthcore	   */	"",
	/* Rogr Aglondier	   */	"raglondi",
	/* Alliene Aglondier   */	"aaglondi",
	/* Razzik Fanggrabber  */	"",
	/* unknown			   */	"",
	/* Suris Enlien		   */	"enlien",
	/* Kotrin Skirvin	   */	"skirvin",
	/* Daeric Mhoried	   */	"",
	/* Michael Mhoried	   */	"",
	/* 					   */	"",
	/* 					   */	"",
	/* 					   */	"",
	/* 					   */	"",
	/* Godar Thurinson	   */	"",
	/* unknown			   */	"",
	/* Jaison Raenech	   */	"",
	/* Terence Gryphon	   */	"",
	/* Rhuobhe the Elf	   */	"",
	/* Nhoun			   */	"",
	/* Marlae Roesone	   */	"mroesone",
	/* Michael Agnelie	   */	"agnelie",
	/* Queen Iselie		   */	"isaelie",
	/* Corwin Rhysdiordan  */       "rhysdior",
	/* The Spider		   */	"",
	/* unknown			   */	"",
	/* Harald Khorien	   */	"khorien",
	/* Trevor Onwen		   */	"onwen",
	/* Thuriene Donalls	   */	"donalls",
	/* Torele Anviras	   */	"anviras",
	/* Tie'skar			   */	"",
	/* Kral Two-Toes	   */	"",
	/* Fhileraene		   */	"fhilerae",
	/* Llytha Damaan	   */	"damaan",
	/* Laela Flaertes	   */	"flaertes",
	/* Braedonnal Tuare	   */	"tuare",
};
DEFINE_VECTOR_CLASS(CSTRPTR, szRegentTextFile);

DEFINE_VECTOR_DATA_S(ADV_SITE,advsite,MAX_ADV_SITES) = {
// sorted by level, kinda
{"endie_mw", 1, AVAILABLE, REALM::ALAMIE,54},
{"black_pw", 1, AVAILABLE, REALM::BOERUINE,59},
{"barro_mw", 2, AVAILABLE, REALM::FIVEPEAKS,50},
{"braem_mw", 1, AVAILABLE, REALM::MEDOERE,5},
{"bord_kg",  1, AVAILABLE, REALM::RHUOBHE,61},
{"nowel_kw", 1, AVAILABLE, REALM::TALINIE,9},
{"lofto_mw", 2, AVAILABLE, REALM::ALAMIE,17},
{"bindi_mw", 2, AVAILABLE, REALM::BROSENGAE,10},
{"caerl_kw", 2, AVAILABLE, REALM::CARIELE,25},
{"rourv_mw", 2, AVAILABLE, REALM::COERANYS,29},
{"ilien_mw", 2, AVAILABLE, REALM::ILIEN,4},
{"winte_xx", 2, AVAILABLE, REALM::ROESONE,60},
{"storm_kw", 2, AVAILABLE, REALM::TAEGHAS,12},
{"doom_mw",  2, AVAILABLE, REALM::THURAZOR,52},
{"haes_kw",  1, AVAILABLE, REALM::TUORNEN,16},
{"calri_kw", 3, AVAILABLE, REALM::AERENWE,2},
{"aerel_mw", 3, AVAILABLE, REALM::DIEMED,3},
{"ansie_tw", 2, AVAILABLE, REALM::ELINIE,20},
{"falcomw",  3, AVAILABLE, REALM::ENDIER,21},
{"shadc_mw", 3, AVAILABLE, REALM::GHOERE,58},
{"moons_kw", 3, AVAILABLE, REALM::MHORIED,56},
{"sonne_kw", 3, AVAILABLE, REALM::THURAZOR,62},
{"fellp_kw", 4, AVAILABLE, REALM::GORGONSCROWN,23},
{"shadg_tw", 4, AVAILABLE, REALM::OSOERDE,63},
{"rhoub_kw", 4, AVAILABLE, REALM::RHUOBHE,11},
{"spidfell", 4, AVAILABLE, REALM::SPIDERFELL,7},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
{"        ", 1, AVAILABLE, REALM::MONSTERS,-1},
};
DEFINE_VECTOR_CLASS(ADV_SITE, advsite);


typedef struct _ButtonInfo
{
	POINT Pos;
	GAME_STRING Text;
	UBYTE HiLiteTextColor;
	UBYTE TextColor;
	POINT TextOffs;
	SHORT hButtonArt;
	SHORT hDepressedButtonArt;
} ButtonInfo;
static ButtonInfo Buttons[2][2];

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
static char SourcePath[256];
SHORT localActiveRegent;
static SHORT iRegent1 = fERROR;
static SHORT iRegent2 = fERROR;
static SHORT iOldRealm = fERROR;
static SHORT iRegn1 = fERROR;
static SHORT iRegn2 = fERROR;
static SHORT iRegn3 = fERROR;
static LONG  itRegent1ID, itRegent2ID;
static LONG  itUnit1, itUnit2;
static LONG  iThingType1, iThingType2;
static ALIGNMENT_INFO::TYPE  iAlignment1, iAlignment2;
static LONG  iWhichRegent;
static char  inParty[ADVENTURER_LIST_SIZE] = {0};
static LONG  numGoing;
static SHORT iLocalCheck[2];
static LONG  iPartyLead;
static LONG  iRegentProvince;
static LONG  iAdvMode;
static LONG  iAdvCountdown;
LONG  iSiteChosen;

DEFINE_VECTOR_DATA_S(LONG,whoIndex,ADVENTURER_LIST_SIZE);
DEFINE_VECTOR_CLASS(LONG,whoIndex);

BOOL  fRealmSelected = FALSE;
// GWP Unused BOOL  fCouncelor = FALSE;
static LONG  iRegent1ID;
static LONG  iRegent2ID;
static LONG  iUnit1;
static LONG  iUnit2;
LONG  iDomainCharInfo = FALSE;
LONG  adv_dif = 0;

DEFINE_VECTOR_DATA_S(LONG,lRealmSelectedByPlayer,LAND_REALM_COUNT);
DEFINE_VECTOR_CLASS(LONG,lRealmSelectedByPlayer);

int   iNewSite;

extern LONG		iOldSite;
extern LONG		TutorialActionNumber;
extern LONG		TutorialScreenNumber;
extern BOOL		fTutorialSelected;
extern SHORT	fFadedOut;
extern SHORT	iSelectedUnit;
extern SHORT	iUnitInfo;
extern SHORT	iPlaceInfo;
extern SHORT	iProvInfo;
extern LONG		report_done_dturn_mode;

/* ------------------------------------------------------------------------
   Extern Variables
   ------------------------------------------------------------------------ */

extern BOOL char_selected;		// from game.cpp
extern BOOL fPreparingSpells;	// from invngui.cpp
extern SHORT fRoundOver;		// from gamemap.cpp
extern SHORT sitecount[];		// from gameopt.cpp

/* ------------------------------------------------------------------------
   Internal Prototypes
   ------------------------------------------------------------------------ */
static void ClearAdvSites(void);
void SetAllAdvSite(void);
void IncActionTurn(void);

/* ------------------------------------------------------------------------
   Extern Prototypes
   ------------------------------------------------------------------------ */
void ReconstructMap(void);
void DoSlideShow(LONG iWhich);
void crease (LONG x, LONG y, LONG w, LONG h);
void UpdateDataArea(void);
void DataAreaMode (LONG type, LONG dummy);
void DisplayInfo (CSTRPTR szFrame, BOOL fCentered, int iformat, ...);
void InitDomainTurn (LONG);
void DeleteDomainRegions(void);
void AddDomainRegions(void);
void DomainTurnUI (void);
void DrawBorders (LONG x, LONG y, LONG map_x, LONG map_y, LONG w, LONG h, LONG /* scale */);
void UpdateHoldingFlag (void);
void AdventurePrep(void);

//void DoNPCAdmin(void);


void SetRedrawMainMapLevel (void);

void WriteOptionSettings(void);

/* ------------------------------------------------------------------------
   Code
   ------------------------------------------------------------------------ */


/* ========================================================================
   Function	- MultiSetAdvSite
   Description - Set adventure site in multi player
   Returns	 -
   ======================================================================== */
void MultiSetAdvSite(void)
{
	int i;
	int j;

	ClearAdvSites();

	for(i=0; i<MAX_SITES; i++)
	{
		for(j=1; j<LAND_REALM_COUNT; j++)
			if(lRealmSelectedByPlayer[j] != -1)
				if(advsite[i].realm == j)
					advsite[i].available = DISALLOWED;
	}

}


void MultiResetRealmSelect(void)
{
	FILE *fp;
	LONG	i;

#ifdef _WINDOWS
	if ( IsMultiPlayer() )
		fp = fopen("mpstart.dat", "rb");
	else
#endif
		fp = fopen("rlmstart.dat", "rb");
	
	LoadGameFiles(fp);
	fclose(fp);
	date = 0;
	
	fRealmSelected = FALSE;
	iOldRealm = fERROR;
	iProvSelect = 0;
	HomeRealm = REALM::NO_COUNTRY;
	char_selected = FALSE;
	
	ReconstructMap();
	SetRedrawMainMapLevel();
	
	iCurrentRealmIndex = 0;
	max_actions = 3;
	for (i=0; i<LAND_REALM_COUNT; i++)
		action_turn[i] = 1;
	global_action_turn = 1;
	fHeldAction = FALSE;
	fLTAction = BEFORE_LTACTION;
	fRoundOver = FALSE;

	fFinalRoll = FALSE;
	fDoActionIcon = FALSE;
	sMenusUp = 0;
	fFadedOut = -1;
	fUpdateDataArea = TRUE;
	iSelectedUnit = -1;
	iProvInfo = 0;				// no info up
	iUnitInfo = 0;
	iPlaceInfo = 0;
}

/* ========================================================================
   Function	- SelectARealm
   Description - Sets HomeRealm to the realm selected
   Returns	 -
   ======================================================================== */
void SelectARealm(LONG arg, LONG)
{
	HomeRealm = (REALM::REALM_TYPE)arg;
	fRealmSelected = TRUE;
	DrawBorders(0, 0, 0, 0, ((BITMPTR)BLKPTR(iLgMap))->w, ((BITMPTR)BLKPTR(iLgMap))->h, FULL_SCALE);
	SetRedrawMainMapLevel();
}




void NewGameConfirmed(LONG,LONG lMultiFlag);

/* ========================================================================
   Function    - NewGameYNProc
   Description - Procedure for new game dialog box
   Returns     - void
   ======================================================================== */
static LONG lMultiFlagGlobal=0;//so NewGameYNProc can pass the right flag to NewGameConfirmed

void NewGameYNProc(LONG MenuCombo,LONG arg)
{
	LONG	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	SetButtonHilight(MenuId, ButtonId, TRUE);
	RunMenus();
	update_screen();
	TickDelay(4);
	SetButtonHilight(MenuId, ButtonId, FALSE);
	RunMenus();
	update_screen();
	TickDelay(4);
	HideMenu(MenuId);
	RunMenus();

	if (arg==QUESTION_BTN1)
	{
#ifdef _WINDOWS
		if ( IsMultiPlayer() )
		{
			NewGameConfirmed( 0,lMultiFlagGlobal );
		}
		else
#endif
		   OpenGameSelect(0,lMultiFlagGlobal);
	}
}	



/* ========================================================================
   Function    - DomainNewGame
   Description - Setup for a new game
   Returns     -
   ======================================================================== */
void DomainNewGame (LONG, LONG lMultiFlag )
{
	if (!fDomainTurn &&
		dturn_mode != CHARSEL_MODE &&
		master_game_type != GAME_ADVENTURE &&
		master_game_type != GAME_BATTLE )
		return;
		
	if ( mouse_button == 2 )
	{
		SystemHelp( STR_MAIN1_HELP_NEW_TITLE,
					STR_MAIN1_HELP_NEW_TEXT, H_Begin_New_Game, 0, 0 );

	}
	else //---- We need help
	{

printf("DomainNewGame - char_selected:%d\n",char_selected);

		if (!char_selected)
		{
#ifdef _WINDOWS
			if ( IsMultiPlayer() )
			{
				NewGameConfirmed( 0,lMultiFlag );
			}
			else
			{
				OpenGameSelect(0,lMultiFlag);
			}
#else
			OpenGameSelect(0,lMultiFlag);
#endif
			return;
		}

		lMultiFlagGlobal=lMultiFlag;

		// set the proc to null
		SetButtonProc  (D_QUESTION2, 0, NULL, 0, 0 );

		// set the question text
		SetButtonLabel  (D_QUESTION2, QUESTION_TEXT, STR_MAIN1_YN_QUESTION_NEW, BLACK );

		// turn on the first button as YES
		SetButtonLabel  (D_QUESTION2, QUESTION_BTN1, STR_YES, BTN_LABEL_COLOR );
		SetButtonProc   (D_QUESTION2, QUESTION_BTN1, NewGameYNProc, QUESTION_BTN1, D_KEY_YES );

		// turn on the third button as CANCEL
		SetButtonLabel  (D_QUESTION2, QUESTION_BTN2, STR_CANCEL, BTN_LABEL_COLOR );
		SetButtonProc   (D_QUESTION2, QUESTION_BTN2, NewGameYNProc, QUESTION_BTN3, D_KEY_CANCEL );
	
		ShowMenu(D_QUESTION2);
	}
}



void NewGameConfirmed2(BOOL fMulti)
{
	int i;

	// initialize the screen for the realm/character selection
	HomeRealm = REALM::NO_COUNTRY;
	fRealmSelected = FALSE;
	iProvSelect = 0;
	iOldRealm = fERROR;

	for ( i = 0; i < LAND_REALM_COUNT; i++ )
		realm[i].mfSetPlayerCtrl(FALSE);

	ReconstructMap();
	
	// make the menu go away
//	fPause = FALSE;
//	fRender = TRUE;
//	HideMenu(D_MAINMENU);

	if ( !fMulti )
	{
		GAMEToggleMainMenu(FALSE,0);

		// wait for menu to minimize
		RunMenus();

	}

	// make sure data area is the correct size
	if (l_map != DATA_AREA_MID)
	{
		//AddSndObj((BIRTHRT_SND)SND_UI_STATUS_REPORT,0,VOLUME_NINETY);
		req_l_map = DATA_AREA_MID;
	}

	// clear any old menu data
	ResetMenus();
	ResetPanels();
	ResetRequests();
	
	// clear all adventure sites from being viewable

	if ( !fMulti )
	{
		ClearAdvSites();
	}

	// set the new mode
	dturn_mode = CHARSEL_MODE;
	SetButtonFlag(D_GAMEBUTTON, 0, D_INVISIBLE);
	
//	activate_region(regDOMAIN_TURN_MODE,	FALSE);		// domain turn mode OFF
//	activate_region(regACTION_MODE,			FALSE);		// action mode OFF
//	activate_region(regMINIMIZED,			FALSE);		// minimized OFF
//	activate_region(regMAXIMIZED,			FALSE);		// maximized OFF
//	activate_region(regTAX_ROLLS,			FALSE);		// tax rolls OFF
//	activate_region(regSWITCH_TO_ACTIONS,	FALSE);		// switch to actions OFF
//	activate_region(regADJUST_TAXES,		FALSE);		// adjust taxes OFF
//	activate_region(1008, FALSE);						// map display buttons OFF

	// clear the borders
//	DrawBorders(0, 0, 0, 0, ((BITMPTR)BLKPTR(iLgMap))->w, ((BITMPTR)BLKPTR(iLgMap))->h, FULL_SCALE);

	iRegn1 = GetResourceStd ("UI\\REGBTN1A.PCX", FALSE);
	iRegn2 = GetResourceStd ("UI\\REGAREA1.PCX", FALSE);
	iRegn3 = GetResourceStd ("UI\\REGAREA2.PCX", FALSE);

}


void NewGameConfirmed(LONG,LONG lMultiFlag)
{
	LONG i;
//printf("DomainNewGame: ");
//for (i=0; i<10; i++) printf("%d ",region_state[i]);
//printf("\n");



#ifdef _WINDOWS

	//---- If we are not calling this from multiplayer code then finalize current game

	if ( lMultiFlag != kFromMulti &&
		 IsMultiPlayer()        )
	{
	   ShowMultiQuit(iMPQ_BEGINNEW, 0);
	   return;
	
//	   AMultiPlayer.Finalize();

	}


#endif



	if (!fDomainTurn)  // New Game only allowed at map level
	    return;
	
	FILE *fp;
#ifdef _WINDOWS
	if ( IsMultiPlayer() )
		fp = fopen("mpstart.dat", "rb");
	else
#endif
		fp = fopen("rlmstart.dat", "rb");
	
	LoadGameFiles(fp);
	fclose(fp);
	date = 0;

	NewGameConfirmed2( FALSE );

}



DEFINE_MATRIX_DATA_S(GAME_STRING,TypesDiffs,LAND_REALM_COUNT,2) =
{
/* NO_COUNTRY	*/	{STR_NULL,				STR_NULL},
/* AERENWE		*/	{STR_REALMPLAYTYPE_TRA,	STR_REALMPLAYTYPE_MED},
/* ALAMIE		*/	{STR_REALMPLAYTYPE_MIL,	STR_REALMPLAYTYPE_HRD},
/* ANUIRE		*/	{STR_NULL,				STR_NULL},
/* AVANIL		*/	{STR_NULL,				STR_NULL},
/* BARUKAZHIK	*/	{STR_REALMPLAYTYPE_TRA,	STR_REALMPLAYTYPE_EAS},
/* BOERUINE		*/	{STR_NULL,				STR_NULL},
/* BROSENGAE	*/	{STR_REALMPLAYTYPE_TRA,	STR_REALMPLAYTYPE_MED},
/* CARIELE		*/	{STR_REALMPLAYTYPE_TRA,	STR_REALMPLAYTYPE_MED},
/* CHIMAERON	*/	{STR_NULL,				STR_NULL},
/* COERANYS		*/	{STR_REALMPLAYTYPE_TEM,	STR_REALMPLAYTYPE_EAS},
/* DHOESONE		*/	{STR_REALMPLAYTYPE_MAG,	STR_REALMPLAYTYPE_EAS},
/* DIEMED		*/	{STR_REALMPLAYTYPE_MIL,	STR_REALMPLAYTYPE_HRD},
/* ELINIE		*/	{STR_REALMPLAYTYPE_TRA,	STR_REALMPLAYTYPE_MED},
/* ENDIER		*/	{STR_REALMPLAYTYPE_TRA,	STR_REALMPLAYTYPE_EAS},
/* FIVEPEAKS	*/	{STR_NULL,				STR_NULL},
/* GHOERE		*/	{STR_NULL,				STR_NULL},
/* GORGONSCROWN	*/	{STR_NULL,				STR_NULL},
/* ILIEN		*/	{STR_REALMPLAYTYPE_MAG,	STR_REALMPLAYTYPE_HRD},
/* MARKAZOR		*/	{STR_NULL,				STR_NULL},
/* MEDOERE		*/	{STR_REALMPLAYTYPE_TEM,	STR_REALMPLAYTYPE_MED},
/* MHORIED		*/	{STR_NULL,				STR_NULL},
/* MIERES		*/	{STR_NULL,				STR_NULL},
/* MONSTERS		*/	{STR_NULL,				STR_NULL},
/* MURKILAD		*/	{STR_NULL,				STR_NULL},
/* OSOERDE		*/	{STR_NULL,				STR_NULL},
/* RHUOBHE		*/	{STR_NULL,				STR_NULL},
/* ROESONE		*/	{STR_REALMPLAYTYPE_MIL,	STR_REALMPLAYTYPE_HRD},
/* SIELWODE		*/	{STR_REALMPLAYTYPE_MAG,	STR_REALMPLAYTYPE_HRD},
/* SPIDERFELL	*/	{STR_NULL,				STR_NULL},
/* TAEGHAS		*/	{STR_REALMPLAYTYPE_MAG,	STR_REALMPLAYTYPE_MED},
/* TALINIE		*/	{STR_REALMPLAYTYPE_MAG,	STR_REALMPLAYTYPE_EAS},
/* THURAZOR		*/	{STR_NULL,				STR_NULL},
/* TUARHIEVEL	*/	{STR_REALMPLAYTYPE_MAG,	STR_REALMPLAYTYPE_HRD},
/* TUORNEN		*/	{STR_REALMPLAYTYPE_MIL,	STR_REALMPLAYTYPE_HRD},
};
DEFINE_MATRIX_CLASS_S(GAME_STRING, TypesDiffs, LAND_REALM_COUNT, 2);

GAME_STRING RealmInfoBlurbs[LAND_REALM_COUNT] =
{
STR_NULL,
STR_REALM_DATA_AERENWE,
STR_REALM_DATA_ALAMIE,
STR_REALM_DATA_ANUIRE,
STR_REALM_DATA_AVANIL,
STR_REALM_DATA_BARUKAZHIK,
STR_REALM_DATA_BOERUINE,
STR_REALM_DATA_BROSENGAE,
STR_REALM_DATA_CARIELE,
STR_REALM_DATA_CHIMAERON,
STR_REALM_DATA_COERANYS,
STR_REALM_DATA_DHOESONE,
STR_REALM_DATA_DIEMED,
STR_REALM_DATA_ELINIE,
STR_REALM_DATA_ENDIER,
STR_REALM_DATA_FIVEPEAKS,
STR_REALM_DATA_GHOERE,
STR_REALM_DATA_GORGONSCROWN,
STR_REALM_DATA_ILIEN,
STR_REALM_DATA_MARKAZOR,
STR_REALM_DATA_MEDOERE,
STR_REALM_DATA_MHORIED,
STR_REALM_DATA_MIERES,
STR_REALM_DATA_MONSTERS,
STR_REALM_DATA_MURKILAD,
STR_REALM_DATA_OSOERDE,
STR_REALM_DATA_RHUOBHE,
STR_REALM_DATA_ROESONE,
STR_REALM_DATA_SIELWODE,
STR_REALM_DATA_SPIDERFELL,
STR_REALM_DATA_TAEGHAS,
STR_REALM_DATA_TALINIE,
STR_REALM_DATA_THURAZOR,
STR_REALM_DATA_TUARHIEVEL,
STR_REALM_DATA_TUORNEN,
};

#define REG_SHOW_STATUS 1
#define REG_BEGIN_RULE 2
#define REG_MORE_INFO 3


//This func shows the info on a regent and sets some parameters dealing with
//that regent. You have to pass in tons of output params so the function knows
//where to put the data, and because of all the silly globals in this file.

//x and y are the upper left coords of the box that this function will show
//the info in. The info the function prints takes up a box that is about
//240x140 pixels.

//Note! This function adds regions but never gets rid of them.
// The caller wipes all the regions dealing with the offending function
//every frame.

static void ShowRegentInfo(ULONG x,ULONG y,LONG RegentIdx,LONG iWhichRegent,LONG* piThingType,ALIGNMENT_INFO::TYPE* piAlignment,SHORT* piRegent,LONG* pitUnit,LONG* pitRegentID,ButtonInfo* pCharButton,ButtonInfo* pBeginRuleButton)
{
	char buff[80];
	SHORT hStat=fERROR;
	SHORT iRegent=fERROR;
	LONG iThingType=0;
	ALIGNMENT_INFO::TYPE iAlignment=ALIGNMENT_INFO::NON_ALIGNED;
	LONG itUnit=0;
	LONG itRegentID=0;

	REGENT_PTR			pRegent = &regents[RegentIdx];

	SHORT hPortraitBack = GetResourceStd ("UI\\REGAREA1.PCX", FALSE);
	

#if !defined(_JUNEDEMO)	
	add_region(x, y+15, 72, 92,  0, SelectRegent, iWhichRegent, REG_MORE_INFO, 1009, STR_CS_SELECT_REGENT);
#endif	
	add_region(x, y+112, 84, 28,   0, SelectRegent, iWhichRegent, REG_SHOW_STATUS,1009, -1);
	add_region(x+85,y+112, 84, 28,0, SelectRegent, iWhichRegent, REG_BEGIN_RULE, 1009, -1);


	// get the pictures for the potential regents
	itUnit = regents[RegentIdx].mfGetunit();
	itRegentID = units[itUnit].id;

	hStat = LoadStats(itRegentID, fERROR);
	if(hStat != fERROR)
	{
		PLAYER_STATS* pStat = (PLAYER_STATS *) BLKPTR(hStat);
		sprintf(buff,"UI\\PORTS_L\\LG%s.pcx", GAME_TTYPE::mfGetArtFileName(pStat->mfGetType()));

		iThingType = pStat->mfGetType();	
		iAlignment = pStat->mfGetAlignment();	

		iRegent = GetResourceStd (buff, FALSE);
	}

	// print name and data
	print_textf(x, y, DKBROWN, "^F05^C060%s",pRegent->mfGetname());
	print_textf(x+82, y+14, DKRED, STRMGR_GetStr(STR_CS_RACE));
	print_textf(x+82, y+24, DKBROWN, "^F02%s", STRMGR_GetStr(gsRace[pRegent->mfGetRace()]));
	if (pRegent->mfGetClass2() != NO_CLASS && pRegent->mfGetLevel2() > 0)
	{
		//strmgr can't get two strings in one func call
		char buffer[100];
		strcpy (buffer,STRMGR_GetStr(gsClass[pRegent->mfGetClass2()]));

	    print_textf(x+82, y+34, DKBROWN, "^F02%s/%s",STRMGR_GetStr(gsClass[pRegent->mfGetClass1()]), buffer);
	    print_textf(x+82, y+44, DKBROWN, STRMGR_GetStr(STR_CS_LEVEL2),pRegent->mfGetLevel1(), pRegent->mfGetLevel2());
	}
	else
	{
	    print_textf(x+82, y+34, DKBROWN, "^F02%s", STRMGR_GetStr(gsClass[pRegent->mfGetClass1()]));
	    print_textf(x+82, y+44, DKBROWN, STRMGR_GetStr(STR_CS_LEVEL1), pRegent->mfGetLevel1());
	}
	print_textf(x+82, y+54, DKBROWN, "^F02%s", ALIGNMENT_INFO::mfGetName(iAlignment));
	print_textf(x+82, y+69, DKRED, STRMGR_GetStr(STR_CS_BLOOD));
	print_textf(x+82, y+79, DKBROWN, "^F02%s", STRMGR_GetStr(gsBL_grade[pRegent->mfGetBL_grade()]));
	print_textf(x+82, y+89, DKBROWN, "^F02%s", STRMGR_GetStr(gsBL_deriv[pRegent->mfGetBL_deriv()]));
	print_textf(x+82, y+99, DKBROWN, STRMGR_GetStr(STR_CS_STRENGTH), pRegent->mfGetBL_strength());


	// draw Portrait
	DrawBitmap (x,y+15, hPortraitBack, 0, 0, 72, 92);
	SetPurge(hPortraitBack);
	hPortraitBack = fERROR;

	if(iRegent != fERROR)
	{
		SetRemapTable((USHORT)HomeRealm);
		DrawBitmap (x+4,y+19, iRegent, 0, 16, 64, 84);
		ClearRemapTable();
	}

	
	if (pCharButton)
	{
		//pCharButton->Bkgnd=SaveBitmap(x-3,y+109,90,34);
		pCharButton->Pos.x=x;
		pCharButton->Pos.y=y+112;
		pCharButton->Text=STR_3PT_CHARACTER;
		pCharButton->TextColor=BTN_LABEL_COLOR;
		pCharButton->HiLiteTextColor=BTN_LABEL_HILITE_COLOR;
		pCharButton->TextOffs.x=10;
		pCharButton->TextOffs.y=8;
		// GWP HACK HACK HACK HACK (following test.)
		if ( y == 196)
		{
			pCharButton->hButtonArt = GetResourceStd("UI\\REGBTN3A.PCX", FALSE);
			pCharButton->hDepressedButtonArt = GetResourceStd("UI\\REGBTN3B.PCX", FALSE);
		}
		else
		{
			pCharButton->hButtonArt = GetResourceStd("UI\\REGBTN5A.PCX", FALSE);
			pCharButton->hDepressedButtonArt = GetResourceStd("UI\\REGBTN5B.PCX", FALSE);
		}
	}
		
	// draw the "Character" button
	DrawBitmap (x, y+112, pCharButton->hButtonArt, 0, 0, 84, 28);
	print_textf(x+11, y+119, BTN_LABEL_COLOR, STRMGR_GetStr(STR_3PT_CHARACTER));

	
	LONG BeginRuleColor=BTN_LABEL_COLOR;
#ifdef _WINDOWS
	if(IsMultiPlayer() && lRealmSelectedByPlayer[pRegent->mfGetRealm()] != -1)
		BeginRuleColor=GREY;
#endif

	if (pBeginRuleButton)
	{
		//pBeginRuleButton->Bkgnd=SaveBitmap(x+84,y+109,90,34);
		pBeginRuleButton->Pos.x=x+87;
		pBeginRuleButton->Pos.y=y+112;
		pBeginRuleButton->Text=STR_3PT_BEGIN_RULE;
		pBeginRuleButton->TextColor=BeginRuleColor;
		pBeginRuleButton->HiLiteTextColor=BTN_LABEL_HILITE_COLOR;
		pBeginRuleButton->TextOffs.x=10;
		pBeginRuleButton->TextOffs.y=8;
		// GWP HACK HACK HACK HACK (following test.)
		if ( y == 196)
		{
			pBeginRuleButton->hButtonArt = GetResourceStd("UI\\REGBTN4A.PCX", FALSE);
			pBeginRuleButton->hDepressedButtonArt = GetResourceStd("UI\\REGBTN4B.PCX", FALSE);
		}
		else
		{
			pBeginRuleButton->hButtonArt = GetResourceStd("UI\\REGBTN6A.PCX", FALSE);
			pBeginRuleButton->hDepressedButtonArt = GetResourceStd("UI\\REGBTN6B.PCX", FALSE);
		}
	}

	// draw the "BeginRule" button
	DrawBitmap (x+87, y+112, pBeginRuleButton->hButtonArt, 0, 0, 84, 28);

	print_textf(x+97, y+119, BeginRuleColor,STRMGR_GetStr(STR_3PT_BEGIN_RULE));

	//assign to the output params.
	*piRegent=iRegent;
	*piThingType=iThingType;
	*piAlignment=iAlignment;
	*pitUnit=itUnit;
	*pitRegentID=itRegentID;

}

/* ========================================================================
   Function	- DisplayRealmSelection
   Description - Callback function from GameMap.cpp to update the screen
   Returns	 -
   ======================================================================== */
extern LONG		req_x;
extern LONG		req_y;
extern LONG		map_x;
extern LONG		map_y;
extern LONG		desired_scale;
extern LONG		max_scale;
extern LONG		map_scale;






void DisplayRealmSelection(void)
{
	PLAYER_STATS		*pStat1, *pStat2;
	SHORT					hStat;
	char					buff[80];
	int					i, y = 40;

	// draw left panel
	//iDTurnBk = GetResourceStd("UI\\DTURN_BK.PCX", FALSE);
	DrawBitmap (0, 0, iDTurnBk, (SHORT)(DATA_AREA_MAX-l_map), 0, (SHORT)l_map, 480);

	// make sure data area is the proper size
	if(l_map != DATA_AREA_MID)
	{	
		req_l_map = DATA_AREA_MID;
		return;
	}

	// delete the old regions
	del_region(SelectRegent, 0);
	del_region(SelectARealm, 0);

	activate_region(regSHOW_HOLDINGS,FALSE);

	if(!fRealmSelected)
	{
		fRealmSelected=TRUE;
		SelectARealm(REALM::DHOESONE,0);
	}

	if (HomeRealm>0)
	{
		REALM_STRUCT_PTR	pRealm   = &realm[HomeRealm];
		unsigned int		Regent_  = pRealm->mfGetRegent();
		REGENT_PTR			pRegent1 = &regents[(HomeRealm * 2) - 1];
		REGENT_PTR			pRegent2 = &regents[(HomeRealm * 2)];

		if(iOldRealm != HomeRealm)
		{
			// remove the old bitmaps
			if (iRegent1!=fERROR)
			{
				SetPurge(iRegent1);
				iRegent1 = fERROR;
			}
			if (iRegent2!=fERROR)
			{
				SetPurge(iRegent2);
				iRegent2 = fERROR;
			}
			iOldRealm = HomeRealm;
		}

		// print realm name
		init_gfont(FONT_TITL_20PT);
		{
			char buf[80];
			sprintf(buf, "^N%s", realm[HomeRealm].mfGetName());
			gprint_text(10, 5, buf, DKBROWN);
		}
		crease(10, 35, 144, 1);						// seperator
	
		//print fields
		init_gfont(FONT_SANS_8PT);
		gprint_text(10, 40, STRMGR_GetStr(STR_REALM_DATA_FIELDS), DKBROWN);
							
		//print realm data
		init_gfont(FONT_SANS_8PT);
#if 0		
		gprint_text(100, 40, STRMGR_GetStr(RealmInfoBlurbs[HomeRealm]), DKBROWN);
#else
		//-------------------------------------------------------------
		// count stuff
		//
		// This is ridiculous.  This should be centralized.
		//-------------------------------------------------------------
		
		SHORT my_provinces=0, my_income=0;
		SHORT my_law = 0, my_guild=0, my_temple=0, my_source=0;
		SHORT my_castle=0, my_troops=0, my_allies=0;
		
		for (i = 1; i <= PROVINCE_COUNT; i++)
		{
			SHORT civlevel = province[i].CivLevel;
			SHORT taxlevel = province[i].TaxLevel;
			BOOL province_mine = (province[i].Realm == HomeRealm);
			
			if (province_mine)
			{
				my_income += AveProvinceTaxes(civlevel, taxlevel);
				my_provinces++;
			}
			
			SHORT place = province[i].FirstPlace;
			while (place > 0)
			{
				if (places[place].Realm == HomeRealm)
				{
					SHORT placelevel = places[place].Level;
					
					switch (places[place].Icon)
					{
					case LAW1_ICON:
						my_law++;
						break;
					case GUILD1_ICON:
						if (province_mine)
							my_income += AveGuildTempleTaxes(civlevel,placelevel);
						my_guild++;
						break;
					case TEMPLE1_ICON:
						if (province_mine)
							my_income += AveGuildTempleTaxes(civlevel,placelevel);
						my_temple++;
						break;
					case SOURCE1_ICON:
						my_source++;
						break;
					case CASTLE1_ICON:
						my_castle++;
						break;
					}
				}
				
				if (places[place].NextPlace == 0)
					break;
				
				place = province[i].FirstPlace + places[place].NextPlace;
			}
		}
		
		for (i = 1; i < MAX_UNITS; i++)
		{
			if (units[i].Realm == HomeRealm)
				my_troops++;
		}
		
		for (i = 1; i < LAND_REALM_COUNT; i++)
		{
			if (realm[i].mfExists() && (RealmStatus[HomeRealm][i]&0x0F) >= STATUS_PERMISSIVE_ALLIANCE)
				my_allies++;
		}
		
		char realminfobuf[120];
		SHORT count;
		
		strcpy(realminfobuf, STRMGR_GetStr(RealmInfoBlurbs[HomeRealm]));
		
		// look for second newline
		for (i = 0, count = 0; i < strlen(realminfobuf) && count < 2; i++)
		{
			if (realminfobuf[i] == '\n')
				count++;
		}
		
		sprintf(realminfobuf+i, "%d \n%d \n%d \n%d \n%d \n%d \n%d \n%d \n%d",
			my_provinces, my_income, my_law, my_guild, my_temple, my_source, my_castle,
			my_troops, my_allies);
		gprint_text(100, 40, realminfobuf, DKBROWN);
		
#endif
		
		crease(10, 195, 144, 1);					// seperator

		//WRC the following function has lots of output parameters only
		//because this this module is so reliant on global variables.

		LONG idxRegent1=(HomeRealm * 2) - 1;
		LONG idxRegent2=(HomeRealm * 2);

		memset(&Buttons[0][0], -1, 4 * sizeof(ButtonInfo));
		
		if (regents[idxRegent1].mfGetunit() > 0)
		{
			ShowRegentInfo(5,196,idxRegent1,1,&iThingType1,&iAlignment1,&iRegent1,&itUnit1,&itRegent1ID,&Buttons[0][0],&Buttons[0][1]);
		}
		if (regents[idxRegent2].mfGetunit() > 0)
		{
			crease(10, 338, 144, 1);					// seperator
			ShowRegentInfo(5,340,idxRegent2,2,&iThingType2,&iAlignment2,&iRegent2,&itUnit2,&itRegent2ID,&Buttons[1][0],&Buttons[1][1]);
		}
	}
}

/* ========================================================================
   Function    - DoneMoreInfo
   Description -
   Returns     -
   ======================================================================== */
void DoneMoreInfo (LONG, LONG)
{
	del_region(DoneMoreInfo, 0);

	//SetButtonProc (D_MOREINFO, 0, PaintMoreInfo, 0, 0);
	//SetButtonProc (D_MOREINFO, 4, HideSubMenuWithClick, D_MOREINFO, 0);
	HideMenu(D_MOREINFO);

	RunMenus();
}



void ButtonClickSound(BOOL fDelay)
{
	// The region code already does a click!
	//AddSndObj( SND_UI_BUTTON_CLICK, NULL, VOLUME_NINETY);
	if(!fDelay)
		return;
	
	//delay so the button click will play
	LONG endtime=get_time()+10;
	while(get_time()<endtime)
	{}
}

#define BUTTON_CHARACTER  0
#define BUTTON_BEGIN_RULE 1

void DepressRegentInfoButtons(LONG iWhichRegent,LONG iWhichButton)
{
	ButtonInfo & const Butt=Buttons[iWhichRegent][iWhichButton];

	SysHideCursor();
	DrawBitmap(Butt.Pos.x,Butt.Pos.y,Butt.hDepressedButtonArt,0,0,84,28);
	print_textf(Butt.Pos.x+Butt.TextOffs.x-3,Butt.Pos.y+Butt.TextOffs.y-2, Butt.HiLiteTextColor, STRMGR_GetStr(Butt.Text));
	SysShowCursor();
	update_screen();
	TickDelay(4);
	
	SysHideCursor();
	DrawBitmap(Butt.Pos.x,Butt.Pos.y,Butt.hButtonArt,0,0,84,28);
	print_textf(Butt.Pos.x+Butt.TextOffs.x,Butt.Pos.y+Butt.TextOffs.y, Butt.TextColor, STRMGR_GetStr(Butt.Text));
	SysShowCursor();
	update_screen();
}
		
	

/* ========================================================================
   Function	- SelectRegent
   Description - Callback from region for regents picture
   Returns	 -
   ======================================================================== */
void SelectRegent(LONG arg1, LONG arg2)
{
	iWhichRegent = arg1;

	switch(arg2)
	{
		case REG_MORE_INFO:
		{
			ButtonClickSound(FALSE);

			// show character discription
			iDomainCharInfo = TRUE;
			SetButtonProc (D_MOREINFO, 0, PaintMoreInfo, 0, 0);
			SetButtonProc (D_MOREINFO, 4, NULL, 0, 0);
			ShowMenu(D_MOREINFO);
		}
		break;

		case REG_SHOW_STATUS:
		{
			//FALSE means don't delay, DepressRegentInfoButtons delays for us.
 			ButtonClickSound(FALSE);
			DepressRegentInfoButtons(iWhichRegent-1,BUTTON_CHARACTER);
			
			switch(arg1)
			{
				case 1:
				{
					// show character stats
					iDomainCharInfo = iThingType1;
					SetStatusRealm((REALM::REALM_TYPE) HomeRealm);
					ShowStatus(itRegent1ID, 0);
					iRegent1ID = itRegent1ID;
					iRegent2ID = itRegent2ID;
					iUnit1 = itUnit1;
					iUnit2 = itUnit2;
				}
					break;
				case 2:
				{
					// show character stats
					iDomainCharInfo = iThingType2;
					SetStatusRealm((REALM::REALM_TYPE) HomeRealm);
					ShowStatus(itRegent2ID, 0);
					iRegent1ID = itRegent2ID;
					iRegent2ID = itRegent1ID;
					iUnit1 = itUnit2;
					iUnit2 = itUnit1;
				}
					break;

				default:
					return;
			}
		}
			break;

		case REG_BEGIN_RULE:
		{
#ifdef _WINDOWS
			if(IsMultiPlayer() && lRealmSelectedByPlayer[HomeRealm] != -1)
			return;
#endif

			//FALSE means don't delay, DepressRegentInfoButtons delays for us.
 			ButtonClickSound(FALSE);
			DepressRegentInfoButtons(iWhichRegent-1,BUTTON_BEGIN_RULE);
			
			SysHideCursor();

			BeginRule(0,0);

			SysShowCursor();


		}
			break;

		default:
			return;
	}
}

/* ========================================================================
   Function	- BeginRule
   Description - leave char selector to the map level
   Returns	 -
   ======================================================================== */

void BeginRule(LONG, LONG)
{
	int i, j;

	TurnOffAllSounds();

	ScenarioInfo.IsScenario = 0;		// turn off IsScenario flag


#ifdef _WINDOWS

	if( IsMultiPlayer() )
	{
		if ( !DoMultiRealmStuff(HomeRealm, iWhichRegent) )
			return;
	}
	else
#endif
	{

		ClearAdvSites();
		for(i=0; i<MAX_SITES; i++)
			if(advsite[i].realm == HomeRealm)
				advsite[i].available = DISALLOWED;
	}

	ClearButtonFlag(D_GAMEBUTTON, 0, D_INVISIBLE);
	
	// HideCharacter(0,0); // GWP HideMenu(D_BEGIN_RULE);

	//---- wait for menu to minimize

	// GWP RunMenus();


#ifdef _WINDOWS

	if ( IsMultiPlayer() )
	{

		//---- Everyone has selected a realm

		if ( MultiAllRealmsSelected() )
		{					

			MultiSetAdvSite();

			BeginRule2(0,0);

		}
		else //---- Put up wait menu "which does MultiSetAdvSite() + BeginRule2(0,0)"
		{

			SetupWaitRealm ();   //---- set waiting realms

			ShowMenu(D_MULTIWAIT);  //---- Put up waiting realms menu
		}

	}
	else
#endif
	{
		BeginRule2(0,0);
	}

	ResumeSuspendedMusic();
}


/* ========================================================================
   Function	- BeginRule2
   Description -
   Returns	 -
   ======================================================================== */
void BeginRule2(LONG, LONG)
{

	// maybe we should ACTUALLY SET THE REGENT, ya think?
	realm[HomeRealm].mfSetRegent((HomeRealm * 2) + iWhichRegent - 2);
	ActiveRegent = realm[HomeRealm].mfGetRegent();
	realm[HomeRealm].mfSetPlayerCtrl(TRUE);

	//---- DO NOT PUT CODE ABOVE THIS
	//---- If you have any questions about this come ask me dlj

	#ifdef _WINDOWS
		AMultiPlayer.StartSeed();		
	#endif


	iDomainCharInfo = FALSE;
	del_region(SelectRegent, 0);

	// remove the bitmaps from memory
	if (iRegent1!=fERROR)
	{
		SetPurge(iRegent1);
		iRegent1 = fERROR;
	}
	if (iRegent2!=fERROR)
	{
		SetPurge(iRegent2);
		iRegent2 = fERROR;
	}
	if (iRegn1!=fERROR)
	{
		SetPurge(iRegn1);
		iRegn1 = fERROR;
	}
	if (iRegn2!=fERROR)
	{
		SetPurge(iRegn2);
		iRegn2 = fERROR;
	}
	if (iRegn3!=fERROR)
	{
		SetPurge(iRegn3);
		iRegn3 = fERROR;
	}
	
	for (LONG i = 0; i < 2; i++)
	{
		for (LONG j = 0; j < 2 ; j++)
		{
			if (Buttons[i][j].hButtonArt != fERROR)
			{
				SetPurge (Buttons[i][j].hButtonArt);
				Buttons[i][j].hButtonArt = fERROR;
			}
			
			if (Buttons[i][j].hDepressedButtonArt != fERROR)
			{
				SetPurge (Buttons[i][j].hDepressedButtonArt);
				Buttons[i][j].hDepressedButtonArt = fERROR;
			}
			
		}
	}

	// [d5-30-97 JPC] Assign relics to lieutenants in the multiplayer game.
#if defined (_WINDOWS)
	if (IsMultiPlayer () && date == 0)
	{
		REGRELIC_STRUCT::ClearRelics ();
		REGRELIC_STRUCT::AssignRelics ();
	}
#endif

   // fix up the units array with the new regent information
	TransferRegentIconsToUnitsArray();
	
	//Goes through provinces links the units in them (including regents)

	LinkUnits(FALSE); // Don't send to others

	iCurrentRealmIndex = 0;
	max_actions = 3;
	for (LONG i=0; i<LAND_REALM_COUNT; i++)
		action_turn[i] = 1;
	global_action_turn = 1;
	fHeldAction = FALSE;
	fLTAction = BEFORE_LTACTION;
	fRoundOver = FALSE;

	fFinalRoll = FALSE;
	fDoActionIcon = FALSE;
	sMenusUp = 0;
	fFadedOut = -1;
	fRedrawAll = TRUE;
	fDomainTurn = TRUE;
	fUpdateDataArea = TRUE;
	iSelectedUnit = -1;
	iProvInfo = 0;				// no info up
	iUnitInfo = 0;
	iPlaceInfo = 0;
	
#ifdef _WINDOWS
	if (!IsMultiPlayer ())
		date = 0;
#endif

	UpdateHoldingFlag();  // set new holdings
	char_selected = TRUE;

	SelectAdvSite();
	//SetAdvSite("tutor");

//	dturn_mode = START_NEW_OR_LOAD;
//	if (fDomainTurn)
//		InitDomainTurn(0);
//	dturn_mode = START_NEW_OR_LOAD;
//	InitDomainTurn(0);

   //---- Set what type of controls a available for multiplayer games
   SetMPControlMode();

	ResumeSuspendedMusic();
	ucWhichTrack = REDBOOK_REALM;
	PlayTrack(REDBOOK_REALM);

	ClearOldReports();
	AddReport(SUBJ_INITIAL, 0, 0, 0);
	dturn_mode = REPORT_MODE;
	GiveReport(0);
	report_done_dturn_mode = REPORT_DONE_MODE;
	fLoadedNewGame = TRUE;


}

/* ========================================================================
   Function	- PaintMoreInfo
   Description - paint the text on the help menu
   Returns	 -
   ======================================================================== */
void PaintMoreInfo (LONG MenuCombo, LONG )
{
	LONG	X,Y;
	LONG	W,H;
	LONG	MenuId, ButtonId;
	CHAR	color = 228;
	CHAR	buffer[10];
	char	textbuf[4096];
	char	buff[80];
	FILE	*in;
	char path[256];
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	LONG	startY;

	del_region(DoneMoreInfo, 0);

	// get position of text area
	if( GetButtonPosition( MenuId, 0, &X, &Y ) == fERROR)
		return;

	// get size of menu
	if( GetButtonSize( MenuId, 0, &W, &H ) == fERROR)
		return;

	X += 20;
	startY = Y;

	if(iDomainCharInfo == TRUE)
	{
		if(iWhichRegent == 1)
			sprintf(buff,"%stext\\%s.txt", InstallPath, szRegentTextFile[(HomeRealm * 2) - 1]);
		else
			sprintf(buff,"%stext\\%s.txt", InstallPath, szRegentTextFile[(HomeRealm * 2)]);
	}
	else
		sprintf(buff,"%stext\\%s.txt", InstallPath, szRealmTextFile[HomeRealm]);

	// clear the buffer
	memset(&textbuf[0], 0, sizeof(textbuf));

	in = FileOpen(buff, "r");
	if(in != NULL)
	{
		fread(textbuf, sizeof(textbuf), 1, in);
		FileClose(in);
		gprint_text(X, Y, textbuf, color);
	}

	// paint the control buttons
	init_gfont(FONT_TITL_10PT);
	Y = startY+H-32;

	add_region(X+138, Y, 68, 28, 0, DoneMoreInfo, 0, 0, 0, -1);

	SHORT iButton = GetResourceStd ("UI\\REGBTN2A.PCX", FALSE);
	DrawBitmap (X+138, Y, iButton, 0, 0, 999, 999);
	if (iButton != fERROR)
		SetPurge(iButton);

	strcpy(buffer,STRMGR_GetStr(STR_CS_STR_OK));
	print_text_centered(X+138+34,Y+14,buffer,BTN_LABEL_COLOR);
}

/* ========================================================================
   Function	- SelectAdvParty
   Description - Selects the adventure party
   Returns	 -
   ======================================================================== */
void SelectAdvParty(void)
{
	int i;

	iAdvMode = ADV_GETPARTY;
	localActiveRegent = ActiveRegent;

	dturn_mode = ADVPARTY_MODE;
	
	activate_region(regDOMAIN_TURN_MODE	 , FALSE);			   // domain turn mode OFF
	activate_region(regACTION_MODE		  , FALSE);			   // action mode OFF
	activate_region(regMINIMIZED			, FALSE);			   // minimized OFF
	activate_region(regMAXIMIZED			, FALSE);			   // maximized OFF
	activate_region(regTAX_ROLLS			, FALSE);			   // tax rolls OFF
	activate_region(regSWITCH_TO_ACTIONS, FALSE);		   // switch to actions OFF
	activate_region(regADJUST_TAXES		 , FALSE);			   // adjust taxes OFF

	AdventurePrep();
}

/* ========================================================================
   Function	- EndAdvParty
   Description - Sends party off on the adventure
   Returns	 -
   ======================================================================== */
void EndAdvParty(LONG i, LONG j)
{
	// clean up regions, whatever they are
	del_region(EndAdvParty,0);
	del_region(SelectSite,0);
	del_region(ToggleShowParty,0);
	del_region(ShowPartyStatus,0);
	
	if(i == 1)			  // done with select party
	{

#ifdef _WINDOWS				 //---- non eventor

		if ( IsMultiPlayer() )
		{
			//---- Means we are non-eventor

			if ( !EventStarter )
			{
				SelectSite( EventResults.data2, 77777 );
				return;
			}			

		}
#endif

		iAdvMode = ADV_GETSITE;
		SetRedrawMainMapLevel();
	}
	else if(i == 2 || i == 3)	   // cancel or goto adventure site
	{
#ifdef _WINDOWS

		//---- MULTI dont allow cancel in multiplayer
		
		if ( IsMultiPlayer() && i == 2 )
		{
			return;			
		}

#endif


		iAdvMode = ADV_NONE;

		// clean up
		if (iLocalCheck[0]!=fERROR)
		{
			SetPurge(iLocalCheck[0]);
			iLocalCheck[0] = fERROR;
		}
		if (iLocalCheck[1]!=fERROR)
		{
			SetPurge(iLocalCheck[1]);
			iLocalCheck[1] = fERROR;
		}
		if (iRegn1!=fERROR)
		{
			SetPurge(iRegn1);
			iRegn1 = fERROR;
		}
		
		if (iRegn2!=fERROR)
		{
			SetPurge(iRegn2);
			iRegn2 = fERROR;
		}

		if(i == 2)	  // cancel
		{
			//	  restore the state of the map level
			RESTORE_REGION_STATE(regDOMAIN_TURN_MODE		);
			RESTORE_REGION_STATE(regACTION_MODE					 );
			RESTORE_REGION_STATE(regMINIMIZED					   );
			RESTORE_REGION_STATE(regMAXIMIZED					   );
			RESTORE_REGION_STATE(regTAX_ROLLS					   );
			RESTORE_REGION_STATE(regSWITCH_TO_ACTIONS	   );
			RESTORE_REGION_STATE(regADJUST_TAXES			);
			//	  RESTORE_REGION_STATE(1008);

			//action_turn--;
			//AddSndObj((BIRTHRT_SND)SND_UI_STATUS_REPORT,0,VOLUME_NINETY);
			dturn_mode = ACTN_MODE;
			if (fLTAction == DURING_LTACTION)
			{
				ActiveRegent = realm[HomeRealm].mfGetRegent();
  				fLTAction = BEFORE_LTACTION;
			}

			DataAreaMode (ACTN_MODE, 0);
		}
		else if(i == 3) // goto adventure site
		{
			// action_turn++;		[abc 8/18] removed to fix adv. as free lt. action
			if (fLTAction == DURING_LTACTION)
			{
				ActiveRegent = realm[HomeRealm].mfGetRegent();
  				fLTAction = AFTER_LTACTION;
			}
			// [abc 8/18] added to fix adv. as free lt. action
			else
				action_turn[HomeRealm]++;

#ifdef _GEH_DEMO
			// show tutorial
			DoSlideShow(2);
			//	  restore the state of the map level
			RESTORE_REGION_STATE(regDOMAIN_TURN_MODE);
			RESTORE_REGION_STATE(regACTION_MODE);
			RESTORE_REGION_STATE(regMINIMIZED);
			RESTORE_REGION_STATE(regMAXIMIZED);
			RESTORE_REGION_STATE(regTAX_ROLLS);
			RESTORE_REGION_STATE(regSWITCH_TO_ACTIONS);
			RESTORE_REGION_STATE(regADJUST_TAXES);
			//	  RESTORE_REGION_STATE(1008);

			action_turn[HomeRealm]--;
			//AddSndObj((BIRTHRT_SND)SND_UI_STATUS_REPORT,0,VOLUME_NINETY);
			dturn_mode = ACTN_MODE;
			DataAreaMode (ACTN_MODE, 0);
#else
			// first, clean up the linked lists
			{
				LONG	who, me;	  // loop index, unit# of selection
				LONG	unit;		 // index for unit search loop
				LONG	count = 0;	// how many people in party
				LONG	lastGuy = -1; // lockstep link
				
				for (who=0; who<ADVENTURER_LIST_SIZE; who++)
				{
					BOOL did_break = FALSE;
					me = whoIndex[who];
					if (me == -1)  /* end of list */
						break;
						
					if (inParty[who])  // he's going; take him out of map
					{
						if (units[me].Joined)
						{
						    for (unit = 1; unit < MAX_UNITS; unit++)
						    {
						       if (units[unit].NextUnit == me)
						       {
							       units[unit].NextUnit = units[me].NextUnit;
							       did_break = TRUE;
							       break;
						       }
						    }
						}
						else
						{
						    if (units[me].NextUnit > 0)
							{
								units[units[me].NextUnit].Joined = FALSE;
								units[units[me].NextUnit].province = units[me].province;
							}
						}
						units[me].NextUnit = -1;
						
						if (lastGuy < 0)
						{
							iPartyLead = me;
							/* make sure I don't get lost coming home */
							units[me].DestProvince = units[me].province;
							units[me].Joined = FALSE;
						}
						else
						{
							units[lastGuy].NextUnit = me;
							/* we all follow the leader home */
							units[me].DestProvince = units[iPartyLead].DestProvince;
							units[me].Joined = TRUE;
						}
						
						/* pull me out of the province I'm in */
						units[me].province = NO_PROVINCE;

						lastGuy = me;
						++count;
					}
					if (count >= 4)  // That's all we can carry!
						break;
					}
			}	
	
			fPreparingSpells = FALSE;
			// let the scene manager know who's going, and where
		
			LONG index;
			SCENE_MGR::Aggressor = REALM::MONSTERS;
			
			// find the realm in the advsite array instead of places,
			// because this guarantees the _landed_ realm
			for (index = 0; index < MAX_SITES; index++)
			{
			    if (!strnicmp(advsite[index].name, places[j].scene, 8))
			    {
			        SCENE_MGR::Aggressor = (REALM::REALM_TYPE) advsite[index].realm;
			        adv_dif = advsite[index].difficulty;
			        break;
			    }
			}
			if (index >= MAX_SITES)		// couldn't find it?
			{
				adv_dif = advsite[index].difficulty;
			}

			SCENE_MGR::Visitors = (REALM::REALM_TYPE) units[iPartyLead].Realm;
			SCENE_MGR::HomeIndex =  iPartyLead;	// who is the home unit (w/camera)
			SCENE_MGR::AwayIndex = -1;			// who is the away unit
			SCENE_MGR::PlacesIndex = j;			// where we're going
			SCENE_MGR::SceneType = SCENE_AI::ADVENTURE_SCENE;	   // type for next scene ai
			SCENE_MGR::mfRequestNewScene( places[j].scene, TYPE_PLAYERSTART1, FALSE); // load first scene
			
//			TurnOffAllSounds();
			InitDomainTurn(0);
			// dturn_mode = ACTN_MODE;
			// DataAreaMode (ACTN_MODE, 0);
#endif // _GEH_DEMO
		}
	}
	else if (i == 4)
	{
			iAdvMode = ADV_SITE_DESCR;
			SetRedrawMainMapLevel();
	}
	else if (i == 5)
	{
			iAdvMode = ADV_GETPARTY;
			SetRedrawMainMapLevel();
	}
}

void ReturnFromAdventure(LONG, LONG)
{
	LONG	who, me;	  // loop index, unit# of selection

    for (who=0; who<ADVENTURER_LIST_SIZE; who++)
    {
		me = whoIndex[who];
		if (me == -1)  /* end of list */
			break;

		if (inParty[who] && units[me].province == NO_PROVINCE)
		{
		    units[me].province = units[me].DestProvince;
		    units[me].DestProvince = NO_PROVINCE;
		}
	}

//	IncActionTurn();
	TutorialActionNumber++;
	TutorialScreenNumber = 0;

}

/* ========================================================================
   Function	- ToggleShowParty
   Description - Region function to toggle party member on/off
   Returns	 -
   ======================================================================== */
void ToggleShowParty(LONG i, LONG)
{
//	  AddSndObj((BIRTHRT_SND)SND_UI_NOT_PERMITTED,0,VOLUME_NINETY);
    SHORT activeRegentUnit = regents[localActiveRegent].mfGetunit();
    SHORT realRegentUnit = regents[realm[HomeRealm].mfGetRegent()].mfGetunit();

		if (whoIndex[i] == activeRegentUnit)           // always going
		    inParty[i] = TRUE;
		else if ((activeRegentUnit != realRegentUnit)
		         && (whoIndex[i] == realRegentUnit))   // not allowed to go
		    inParty[i] = FALSE;
		else
		{
			if (numGoing < 4 || inParty[i])
				inParty[i] = !inParty[i];	
		}
		
		SetRedrawMainMapLevel();
}

/* ========================================================================
   Function	- ShowPartyStatus
   Description - Region function to show the status screen for a member
   Returns	 -
   ======================================================================== */
void ShowPartyStatus(LONG i, LONG)
{
	if(mouse_button == 1)
	{
			// show character stats
			fPreparingSpells=TRUE;
			SetStatusRealm((REALM::REALM_TYPE) units[i].Realm);
			//ShowStatus(units[i].id, 0);
			LONG const hPlayerStats = LoadStats(units[i].id, fERROR);
			StaticInventoryGUI::mfDisplay(0, hPlayerStats);
	}
}

/* ========================================================================
   Function	- SelectSite
   Description - Region function to select the site for an adventure
   Returns	 -
   ======================================================================== */
void SelectSite(LONG i, LONG multi )
{
	iSiteChosen = i;

//	if(mouse_button == 1)

	if ( mouse_button == 1 || multi == 77777 )
	{

#ifdef _WINDOWS

		//---- If we are multi playing and we requested adventure then send results to others

		if (IsMultiPlayer())
		{
			if ( EventStarter )
			{

				AMultiPlayer.ResultsEvent( iMPE_ADVENTURE,
							        	   1, iSiteChosen, 0, 0, 0 );
			}

		}

#endif 				


		// clean up region
		del_region(SelectSite,0);
		del_region(EndAdvParty,0);
			
		// goto adventure site
		//EndAdvParty(3, i);
		iAdvMode = ADV_SITE_GO;
		iAdvCountdown = 2;
		SetRedrawMainMapLevel();
	}
	else if (mouse_button ==2)
	{
		del_region(SelectSite,0);
		del_region(EndAdvParty,0);
		
		// temporarily display site info
		EndAdvParty(4, i);
	}
}

/* ========================================================================
   Function	- DisplayAdvParty
   Description - Callback function from GameMap.cpp to update the screen
   Returns	 -
   ======================================================================== */
void DisplayAdvParty(void)
{
	unsigned int i, k, y;
	int count = 0;

	// draw left panel
	//iDTurnBk = GetResourceStd("UI\\DTURN_BK.PCX", FALSE);
	DrawBitmap (0, 0, iDTurnBk, (SHORT)(DATA_AREA_MAX-l_map), 0, (SHORT)l_map, 480);

	// make sure data area is the proper size
	if (iAdvMode == ADV_SITE_DESCR  ||  iAdvMode == ADV_SITE_GO)
	{
		if (l_map != DATA_AREA_MAX)
		{
			if (req_l_map != DATA_AREA_MAX)
			{
				if(iAdvMode == ADV_SITE_GO)
				{
					TurnOffAllSounds();
					SuspendMusic();
				}

				//AddSndObj((BIRTHRT_SND)SND_UI_STATUS_REPORT,0,VOLUME_NINETY);
				req_l_map = DATA_AREA_MAX;
			}
			SetRedrawMainMapLevel();
			return;
		}
	}
	else if(l_map != DATA_AREA_MIN)
	{
		if (req_l_map != DATA_AREA_MIN)
		{
			//AddSndObj((BIRTHRT_SND)SND_UI_STATUS_REPORT,0,VOLUME_NINETY);
			req_l_map = DATA_AREA_MIN;
		}
		SetRedrawMainMapLevel();
		return;
	}

	if (iAdvMode == ADV_GETPARTY)
	{
		SHORT activeRegentUnit = regents[localActiveRegent].mfGetunit();
		SHORT realRegentUnit = regents[realm[HomeRealm].mfGetRegent()].mfGetunit();

		// each time through, delete our own regions and add them again
		del_region(EndAdvParty,0);
		del_region(ToggleShowParty,0);
		del_region(ShowPartyStatus,0);
		// print title
		init_gfont(FONT_TITL_20PT);
		{
			char buf[80];
			sprintf(buf, "^N%s", STRMGR_GetStr(STR_CS_ADV));
			gprint_text(10, 5, buf, DKBROWN);
		}
		gprint_text(10, 36, STRMGR_GetStr(STR_CS_CHOOSE), DKBROWN);
		y=66;

		crease(10, y, 144, 1);				  // separator
		y += 8;


		// show the possible party, regent first, then active
		// regent if this is a lieutenant action

		init_gfont(FONT_SANS_8PT);
		count = 0;
		numGoing = 0;

		whoIndex[count++] = realRegentUnit;

		if (activeRegentUnit != realRegentUnit)
			whoIndex[count++] = activeRegentUnit;

		// next, find all other eligible characters

		for(i=0; i<MAX_UNITS; i++)
		{
			if (realm[units[i].Realm].mfExists() 
				&& units[i].Realm == HomeRealm 
				&& units[i].id >= 1000 
				&& units[i].id < 3000 
				&& i != activeRegentUnit 
				&& i != realRegentUnit )
			{
				whoIndex[count++] = i;

				if (count >= ADVENTURER_LIST_SIZE) // as many as will fit
				break;
			}
		}

		if (count < ADVENTURER_LIST_SIZE)
			whoIndex[count] = -1;			// clear invalid entry

		for (i=0; i < count; i++)
		{
			// find this character's name
			for (k=0; k < CHARACTER_COUNT; k++)
			{
				if (regents[k].mfGetunit() != whoIndex[i])
					continue;

				char namebuf[80];
				if (whoIndex[i] == activeRegentUnit)
				{
					numGoing++;
					// this person always goes
					sprintf(namebuf, "%s", regents[k].mfGetname());
					inParty[i] = TRUE;
				}
				else if (whoIndex[i] == realRegentUnit)
				{
					// regent can't go -- grey out name
					sprintf(namebuf, "^T2%s", regents[k].mfGetname());
					inParty[i] = FALSE;
				}
				else
				{
					sprintf(namebuf, "%s", regents[k].mfGetname());
					if (inParty[i])
						numGoing++;

					// checkbox region
					add_region(137, y+2, 30,12,0,ToggleShowParty,i,0,0, -1);
				}

				// equip character region - anyone can give items away
				add_region(5, y, 130,12,0,ShowPartyStatus,whoIndex[i],0,0, -1);

				gprint_text(5, y, namebuf, DKBROWN);
				DrawBitmap(137, y+2, iLocalCheck[inParty[i]], 0, 0, 20, 20);
				break;
			}
			y += 12;
		}

		// add done button
		y = 449;
		add_region(13, y, 68,28,0,EndAdvParty,1,0,0, -1);
		DrawBitmap (13, y, iRegn1, 0, 0, 68, 28);
		print_textf(23, y+7, BTN_LABEL_COLOR, STRMGR_GetStr(STR_CS_DONE));

		// add cancel button if not in multiplayer

#ifdef _WINDOWS			
		if ( !IsMultiPlayer() )
		{
#endif

			add_region(81, y, 68,28,0,EndAdvParty,2,0,0, -1);
			DrawBitmap (81, y, iRegn1, 0, 0, 68, 28);
			print_textf(91, y+7, BTN_LABEL_COLOR, STRMGR_GetStr(STR_CS_CANCEL));

#ifdef _WINDOWS
		}
#endif
	}
	else if (iAdvMode == ADV_GETSITE)	
	{
		// delete the check box and name regions
		del_region(EndAdvParty,0);
		del_region(SelectSite,0);

		// print title
		init_gfont(FONT_TITL_20PT);
		{
			char buf[80];
			sprintf(buf, "^N%s", STRMGR_GetStr(STR_CS_ADV));
			gprint_text(10, 5, buf, DKBROWN);
		}
		gprint_text(10, 36, STRMGR_GetStr(STR_CS_SELECT), DKBROWN);
		y=66;

		crease(10, y, 144, 1);				  // seperator
		y += 8;

		init_gfont(FONT_SANS_8PT);
		// add the names of the active adventure sites to the screen
		for(i=1; i<MAX_PLACES; i++)
		{
			if(places[i].scene != NULL && places[i].fSiteDisplayed)
			{
				int iAvailable;

				// is this site available
				for(int j=0; j<MAX_SITES; j++)
				{
					if(advsite[j].iPlaces == i)
					{
						iAvailable = advsite[j].available;
						break;
					}
				}

				// add the name and region
				if(iAvailable == SELECTABLE)
				{
					gprint_text(5, y, place_names[places[i].iName], DKBROWN);
					add_region(5, y, 130,12,0,SelectSite,i,0,0, -1);
					count++;
					y += 12;
				}
			}
		}

		// put up string if no sites displayed
		if(count == 0)
		{
			gprint_text(5, y, "^W130No new adventure sites are available at this time.", DKBROWN);
		}

		// add cancel button if not in multi player

#ifdef _WINDOWS
		if ( !IsMultiPlayer() )
		{
#endif
			y = 450;
			add_region(13, y, 68,28,0,EndAdvParty,2,0,0, -1);
			DrawBitmap (13, y, iRegn1, 0, 0, 68, 28);
			print_textf(23, y+7, BTN_LABEL_COLOR, STRMGR_GetStr(STR_CS_CANCEL));

#ifdef _WINDOWS
		}
#endif

		// add back button
		y = 450;
		add_region(81, y, 68,28,KEY_B,EndAdvParty,5,0,0, -1);
		DrawBitmap (81, y, iRegn1, 0, 0, 68, 28);
		print_textf(91, y+7, BTN_LABEL_COLOR, STRMGR_GetStr(STR_BACK));
	}
	else if (iAdvMode == ADV_SITE_DESCR  ||  iAdvMode == ADV_SITE_GO)
	{
		FILE *in;
		char textbuf[4096];
		char buff[127];
		int i;
		
		del_region(SelectSite,0);
		del_region(EndAdvParty, 0);


		for(i=0; i<MAX_SITES; i++)
			if(stricmp((char *)advsite[i].name, (char *)places[iSiteChosen].scene) == 0)
				break;

#if defined(_DEBUG)
		if (i == MAX_SITES)			// couldn't find a match ??
		{
			fatal_error("Discrepency between advsite and places");
		}
#endif

		// print title
		init_gfont(FONT_TITL_20PT);
		{
			char buf[80];
			sprintf(buf, "^N%s", place_names[places[iSiteChosen].iName]);
			gprint_text(10, 5, buf, DKBROWN);
		}

		// print info
#if 0
		sprintf(buff,"%sTEXT\\%s.adv", InstallPath, places[iSiteChosen].scene);
#else
		sprintf(buff,"%sTEXT\\%s.txt", InstallPath, places[iSiteChosen].scene);
#endif

//		printf("Trying to open %s (site %d)\n", buff, places[iSiteChosen].iName);

		init_gfont(FONT_SANS_8PT);

		// clear the buffer
		memset(&textbuf[0], 0, sizeof(textbuf));

		in = FileOpen(buff, "r");
		if(in != NULL)
		{
			fread(textbuf, sizeof(textbuf), 1, in);
			FileClose(in);
			gprint_text(10, 40, textbuf, DKBROWN);


			 	
			if (advsite[i].realm!=REALM::FIVEPEAKS)	
				if (((RealmStatus[HomeRealm][advsite[i].realm])&0x0F) > STATUS_NEUTRAL)
				{
					y = 60 + gtext_height(textbuf);
					gprint_text(10, y, STRMGR_GetStr(STR_CS_WARN_ALLIED), DKBROWN);
				}
		}
			
		if (iAdvMode == ADV_SITE_DESCR)
		{
			// done button
			y = 450;
			add_region(13, y, 68,28,0,EndAdvParty,1,0,0, -1);
			DrawBitmap (13, y, iRegn1, 0, 0, 68, 28);
			print_textf(23, y+7, BTN_LABEL_COLOR, STRMGR_GetStr(STR_CS_DONE));
		}	
		else
		{
			if (iAdvCountdown > 0)
			{
				int num = iAdvCountdown--;
				if (num == 1)
					EndAdvParty(3, iSiteChosen);
			}
			SetRedrawMainMapLevel();
		}
	}
}

/* ========================================================================
   Function	- SelectAdvSite
   Description - Picks a random adventure site
   Returns	 -
   ======================================================================== */
void SelectAdvSite(void)
{
	int i, j = 0;
	int k;
	int count;
	int level;
	int rndsite;
	int numRuns = 9999;

	iNewSite = -1;	// assume no new site available

	// if this a single-player game, or we are mastering a multi-player game
	// then select a new site else leave, the master will do it

	switch(adventure_difficulty)
	{
		case DIF_MEDIUM:
			level = (date/5) + 1;
			break;

		case DIF_HARD:
			level = (date/2) + 1;
			break;

		case DIF_EASY:
		default:
			level = (date/10) + 1;
			break;
	}

	// first get the count of available sites
	for(count=0,i=0; i<MAX_SITES; i++)
	{
		if(advsite[i].available == AVAILABLE && advsite[i].difficulty <= level)
		{
			count++;

			// find the site with the lowest number of times used
			if(sitecount[i] < numRuns)
				numRuns = sitecount[i];
		}
	}

	// are there any sites available for adventure
	if(count == 0)
		return;		// no so leave

	// get the count of sites with the lowest number of runs
	for(count=0,i=0; i<MAX_SITES; i++)
	{
		if(sitecount[i] == numRuns && advsite[i].available == AVAILABLE && advsite[i].difficulty <= level)
			count++;
	}

	// select a random count, we will then index that many into the struct
	// and select that site that matches the difficulty
	rndsite = random(count);

	// find that site
	for(i=0; i<MAX_SITES; i++)
	{
		if(sitecount[i] == numRuns && advsite[i].available == AVAILABLE && advsite[i].difficulty <= level)
		{
			if(j == rndsite)
			{
				iNewSite = i;
				break;
			}
			j++;
		}
	}

	if(iNewSite >= 0)
	{
		iOldSite = iNewSite;
//		SetAdvSite(advsite[iNewSite].name);
		SetAdvSite(iNewSite);
		sitecount[iNewSite]++;
		WriteOptionSettings();
	}
}

/* ========================================================================
   Function	- ClearAdvSites
   Description - Clears the list of adventures sites that you can visit
                 used for start new game
   Returns	 -
   ======================================================================== */
static void ClearAdvSites(void)
{
	int i;

	for(i=1; i<MAX_PLACES; i++)
		if(places[i].scene != NULL)
			places[i].fSiteDisplayed = 0;

	for(i=0; i<MAX_SITES; i++)
		advsite[i].available = AVAILABLE;
}

/* ========================================================================
   Function	- SetAdvSite
   Description - Sets an adventure site as visitable
   Returns	 -
   ======================================================================== */
void SetAdvSite(SHORT iSite)
{
	// don't talk about user sites
	if ( iSite < MAX_SITES )
		SetGameData(MP_ADVSITE, MPADV_AVAILABLE, iSite, SELECTABLE, FALSE);
}

/* ========================================================================
   Function	- SetAllAdvSite
   Description - Sets all adventure site as visitable
   Returns	 -
   ======================================================================== */
void SetAllAdvSite(void)
{
	int i;

	for(i=1; i<MAX_PLACES; i++)
		if(places[i].scene != NULL)
			places[i].fSiteDisplayed = 1;

	for(i=0; i<MAX_SITES; i++)
		advsite[i].available = SELECTABLE;
}

/* ========================================================================
   Function    - PaintBattleMagicPrep
   Description - paint the prep dialog name list
   					used out of gamemap.cpp
   Returns     -
   ======================================================================== */
void PaintBattleMagicPrep(LONG, LONG)
{
	unsigned int i, k, y;
	int 	count = 0;
	SHORT	UnitIndex;
	LONG	mx,my,mw,mh;

// each time through, delete our own regions and add them again
	del_region(ShowPartyStatus,0);

	if( GetButtonPosition( D_BATTLE_PREP, 0, &mx, &my ) == fERROR)
		return;
	
	if( GetButtonSize( D_BATTLE_PREP, 0, &mw, &mh ) == fERROR)
		return;
	
	// print title
	init_gfont(FONT_TITL_20PT);
	{
		char buf[80];
		sprintf(buf, "^N%s", STRMGR_GetStr(STR_CS_BATTLE));
		gprint_text(mx+10, my+5, buf, DKBROWN);
	}
	gprint_text(mx+10, my+36, STRMGR_GetStr(STR_CS_BCHOOSE), DKBROWN);
	
	y=my+66;

	crease(mx+10, y, mw-28, 1);				  // separator
	y += 8;
	
	// show the possible magic users in the unit stack
	
	init_gfont(FONT_SANS_8PT);
	count = 0;
	
	// next, find all other eligible characters
	// check the home team
	UnitIndex = SCENE_MGR::HomeIndex;
	do
	{
	    // a player type
	    // GWP if ((units[UnitIndex].id >= 1000 && units[UnitIndex].id < 3000)
		// GWP 	&& GetBattleMagic( units[UnitIndex].Icon )
		// GWP 	)
	    // GWP {
		// GWP     whoIndex[count++] = UnitIndex;
	    // GWP }
	    if (units[UnitIndex].id >= 1000 && units[UnitIndex].id < 3000)
	    {
		    SHORT const hPlayerStats = LoadStats(units[UnitIndex].id, fERROR);
		    DumbHandlePtr<PLAYER_STATS const> const dhPlayerStats(hPlayerStats);
		
		    if (dhPlayerStats->mfIsMagicUser())
		    {
				whoIndex[count++] = UnitIndex;
		    }
	    }
	
		UnitIndex = (SHORT) units[UnitIndex].NextUnit;
	} while (UnitIndex != fERROR);
	
	whoIndex[count] = -1;	// clear this entry, just in case
	
	// now print the results and add regions for activation
	for (i=0; i < count; i++)
	{
	    // find this character's name
	    for (k=0; k < CHARACTER_COUNT; k++)
	    {
			char namebuf[80];
			if(regents[k].mfGetunit() == whoIndex[i])
			{
				sprintf(namebuf, "%s", regents[k].mfGetname());
				
				// equip character region - anyone can give items away
				add_region(mx+10, y, 130,12,0,ShowPartyStatus,whoIndex[i],0,0, -1);
				
				gprint_text(mx+10, y, namebuf, DKBROWN);
				break;
			}
	    }
	    y += 12;
	}
}

/* ========================================================================
   Function    - SetupAdvsitePlaces
   Description - match up the advsite array with the places array
   Returns     -
   ======================================================================== */
void SetupAdvsitePlaces(void)
{
}
