/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: PRACTICE.C
   Author: Greg Hightower
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#include <stdio.h>
#if defined(_WINDOWS)
#include <Windows.h>
#endif

#include "SYSTEM.H"
#include "ENGINE.H"
#include "MACHINE.H"

#include "ACTNMENU.HXX"
#include "ADVPREP.HXX"
#include "AVATAR.HXX"
#include "BATTLE.HXX"
#include "CHARSEL.HXX"
#include "GAME.H"
#include "GAMETYPE.HXX"
#include "REALM.HXX"
#include "SCNMGR.HXX"
#include "STRMGR.H"
#include "strenum.h"
#include "MULTIMAP.HXX"
#include "MULTIUI.HXX"
#include "SNDVOX.HXX"
#include "LOADSAVE.HXX"
#include "PLAYSTAT.HXX"
#include "INVNGUI.HXX"
#include "MAPAI.HXX"
#include "VECTOR.HXX"
#include "DESCRIBE.HXX"
#include "REGENTS.HXX"
#include "GAMEMAP.HXX"
#include "PLACES.HXX"
#include "BATTLEUI.HXX"

#ifdef _WINDOWS
#include ".\WINSYS\MULPLAY.HXX"
#endif

#include "MENU.H"
#include "PANEL.H"
#include "REQUEST.H"
#include "GMENUENM.H"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define DMAX_SCALE		((FULL_SCALE*1920)/448);

#define REPORT_DONE_MODE		10
#define START_SITE_LIST_X	20
#define START_SITE_LIST_Y	98
#define START_PARTY_LIST_X	20
#define START_PARTY_LIST_Y	273
#define INC_LIST_Y			15
#define LIST_CLASS_X		150
#define LIST_CHECK_X		160
#define LIST_CHECK_REGION_X	140
#define BA_DIFF_COL			170

#define LIST_SIZE 			8

/* ------------------------------------------------------------------------
   Structures
   ------------------------------------------------------------------------ */

typedef struct {
	SHORT	IconType;
	SHORT	Id;
} BB_UNIT_TYPES;

static BB_UNIT_TYPES BB_UnitTypes[] = {
	/*  0 - BB_A_INF_UNIT:      */ { A_INF_UNIT_ICON,			0 },
	/*  1 - BB_A_EIN_UNIT:	    */ { A_EIN_UNIT_ICON,			0 },
	/*  2 - BB_A_BOW_UNIT:	    */ { A_BOW_UNIT_ICON,			0 },
	/*  3 - BB_A_IRR_UNIT:	    */ { A_IRR_UNIT_ICON,			0 },
	/*  4 - BB_A_PIK_UNIT:	    */ { A_PIK_UNIT_ICON,			0 },
	/*  5 - BB_A_LVY_UNIT:	    */ { A_LVY_UNIT_ICON,			0 },
	/*  6 - BB_A_SCT_UNIT:	    */ { A_SCT_UNIT_ICON,			0 },
	/*  7 - BB_A_KNT_UNIT:	    */ { A_KNT_UNIT_ICON,			0 },
	/*  8 - BB_A_CAV_UNIT:	    */ { A_CAV_UNIT_ICON,			0 },
	/*  9 - BB_M_INF_UNIT:	    */ { M_INF_UNIT_ICON,			0 },
	/* 10 - BB_M_BOW_UNIT:	    */ { M_BOW_UNIT_ICON,			0 },
	/* 11 - BB_M_IRR_UNIT:	    */ { M_IRR_UNIT_ICON,			0 },
	/* 12 - BB_M_PIK_UNIT:	    */ { M_PIK_UNIT_ICON,			0 },
	/* 13 - BB_M_CAV_UNIT:	    */ { M_CAV_UNIT_ICON,			0 },
	/* 14 - BB_E_INF_UNIT:	    */ { E_INF_UNIT_ICON,			0 },
	/* 15 - BB_E_BOW_UNIT:	    */ { E_BOW_UNIT_ICON,			0 },
	/* 16 - BB_E_CAV_UNIT:	    */ { E_CAV_UNIT_ICON,			0 },
	/* 17 - BB_D_INF_UNIT:	    */ { D_INF_UNIT_ICON,			0 },
	/* 18 - BB_D_BOW_UNIT:	    */ { D_BOW_UNIT_ICON,			0 },
	/* 19 - BB_G_INF_UNIT:	    */ { G_INF_UNIT_ICON,			0 },
	/* 20 - BB_G_BOW_UNIT:	    */ { G_BOW_UNIT_ICON,			0 },
	/* 21 - BB_G_CAV_UNIT:	    */ { G_CAV_UNIT_ICON,			0 },
	/* 22 - BB_N_INF_UNIT:	    */ { N_INF_UNIT_ICON,			0 },
	/* 23 - BB_N_IRR_UNIT:	    */ { N_IRR_UNIT_ICON,			0 },
	/* 24 - BB_C_SKL_UNIT:	    */ { C_SKL_UNIT_ICON,			0 },
	/* 25 - BB_C_SPD_UNIT:	    */ { C_SPD_UNIT_ICON,			0 },
	/* 26 - Assan ibn Daouta:   */ { REG_M_PAL_UNIT_ICON,		1005 },
	/* 27 - Grimm Graybeard:    */ { REG_DWF_M_FTR_UNIT_ICON,	1017 },
	/* 28 - Heirl Diem:         */ { REG_M_FTR_UNIT_ICON, 		1023 },
	/* 29 - Isaelie:            */ { REG_ELF_F_WIZ_UNIT_ICON,	1025 },
	/* 30 - Caine:              */ { M_WIZARD_UNIT_ICON, 		1020 },
	/* 31 - Eluvie Cariele:     */ { REG_F_FTR_UNIT_ICON,		1009 },
	/* 32 - BB_GORGON_UNIT:     */ { GORGON_UNIT_ICON,			1049 },
	/* 33 - BB_SPIDER_UNIT:     */ { SPIDERKING_UNIT_ICON,		1056 },
	/* 34 - BB_RHOUBHE_UNIT:    */ { RHUOBHE_UNIT_ICON,			1054 },
	/* 35 - High mage Aelies:   */ { M_WIZARD_UNIT_ICON,		1058 },
	/* 36 - Hubaere Armiendin:  */ { M_PRIEST_UNIT_ICON,		1068 },
	/* 37 - The Wizard:         */ { F_WIZARD_UNIT_ICON,		1081 },
	/* 38 - Ruarch Rockhammer:  */ { M_PRIEST_UNIT_ICON,		1077 },
	/* 39 - Nhoun the Elf:      */ { ELF_M_FTR_UNIT_ICON,		1055 },
	/* 40 - The Eyeless One:    */ { M_WIZARD_UNIT_ICON,		1063 },
};

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
void DrawLittleButton(LONG, LONG, LONG, LONG, LONG);
extern void LoadControlSet(void);
extern void ActivateControlSet(void);

static void ClearUnitLists(void);
static void InitTroopDisplay (LONG type, LONG id, LONG realm);
static void PaintTroopList();
static void PaintTroopCard();
static void PaintBattleScenarios();
static void AddDisplayTroopsRegions();
static void AddScenariosRegions();
static void SetCurrentScenario(SHORT iValue);
static LONG AddUnit (SHORT StartIndex, LONG prov, LONG icon, LONG id, LONG realm);
static LONG DelUnit (SHORT StartIndex, SHORT KillIndex);
static void ClearRegentUnits(void);
static void PlayBtlFinale();

static void ReadBtlCurrentScenario(void);
static BOOL ReadBtlScenario(FILE *fp, BOOL);
static void InitScenarioTitles(void);

static void ReadBtlStatFile(void);
static void WriteBtlStatFile(void);

static void TogglePartyCheck(LONG i, LONG);
static void SelectAdventures(void);

static void WriteAdvScenario(void);
static void ReadAdvScenario(void);
static void SaveAdvInventories(void);
static void LoadAdvInventories(void);
static void PlayAdvFinale();

void InitDomainTurn(LONG);
void DeleteDomainRegions(void);
void AddDomainRegions(void);

BOOL BattlePrep(void);
void DescribeGame(void);
void DescribeAdventure(void);
void DescribeBattle(void);
void DescribeHistory(void);

// this is stupid that I have to do this and that
// it is not part of some include file!
void SetRedrawMainMapLevel (void);

#if defined(_JUNEDEMO)
extern void ShowAdScreen(SHORT);
#endif

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
/* this is the list of availible people for adventuring 
   We will choose from two from each list at startup
   These numbers are the indexes in the regents array for these people.
   They probably should be the Id's instead.
 */
static UBYTE FighterList[] = {
 1, 3, 19, 60, 40, 2, 103, 101, 104, 95, 
 15, 53, 102, 97, 98, 9, 65, 23, 99, 96, 
 100, 94, 25, 26, 56
};
static UBYTE SizeFighterList = sizeof(FighterList);
 
static UBYTE PriestList[] = {
 81, 89, 88, 20, 36, 14, 39, 106, 109, 110, 
 83, 61, 105, 77, 71, 84, 92, 107, 108, 82, 91, 
};
static UBYTE SizePriestList = sizeof(PriestList);
 
static UBYTE ThiefList[] = {
 10, 72, 75, 54, 86, 74, 73, 78, 85, 27, 
 79, 69, 21, 13, 16,
};
static UBYTE SizeThiefList = sizeof(ThiefList);

static UBYTE WizardList[] = {
 62, 90, 28, 55, 70, 35, 22, 59, 112, 117,
 24, 115, 116, 66, 4, 87, 111, 80, 113, 114,
 76,
};
static UBYTE SizeWizardList = sizeof(WizardList);

static BOOL	OldPartyHasAtLeastOne = TRUE;
static BOOL	PartyHasAtLeastOne = FALSE;

class CANIDATES {
public:
	inline CANIDATES(UBYTE RegentIndex, UBYTE const Dead, UBYTE InParty) :
		fAdventurerRegentIndex(RegentIndex),
		fDead(Dead),
		fInParty(InParty),
		fhPlayerStats(fERROR) {}
	
	inline ~CANIDATES() {
		fAdventurerRegentIndex = 0;
		fDead = 0;
		fInParty = 0;
		fhPlayerStats = fERROR;}
	
	inline SHORT const mfSetRegentIndex(UBYTE const RegentIndex) {
		fAdventurerRegentIndex = RegentIndex;
		fDead = FALSE;
		fInParty = 0; 
		fhPlayerStats = LoadStats(mfGetRegentId(), fERROR);
		return mfGetRegentId();}
	
	inline void mfSetOnlyRegentIndex(UBYTE const RegentIndex) {
		fAdventurerRegentIndex = RegentIndex;
		fhPlayerStats = LoadStats(mfGetRegentId(), fERROR);}
		
	inline void mfSetDead() {
		fDead = TRUE; 
		fInParty = FALSE;}
		
	inline void mfSetAlive() {
		fDead = FALSE; }

	inline BOOL const mfIsDead() const {
		return (fDead == TRUE); }

	inline void mfSetInParty(UBYTE const Flag) {
		fInParty = Flag; }
	
	inline BOOL const mfIsInParty() const {
		return (fInParty == TRUE); }

	inline SHORT const mfGetRegentId() const {
		return regents[fAdventurerRegentIndex].mfGetid(); }
		
	// For use in writing out the saved adventure only data.
	// Should be writing out the id's!
	inline UBYTE const mfGetRegentIndex() const {
		return fAdventurerRegentIndex; }
			
	inline SHORT const mfGetUnit() const {
		return regents[fAdventurerRegentIndex].mfGetunit(); }
		
	inline char const * const mfGetName() const {
		return regents[fAdventurerRegentIndex].mfGetname(); }
		
	inline MAP_ICON const mfGetIcon() const {
		return regents[fAdventurerRegentIndex].mfGeticon(); }

	inline BOOL const mfIsSingleClass() const {
		DumbHandlePtr<PLAYER_STATS const> const pPlayerStats(fhPlayerStats);
		return (pPlayerStats->mfGetClassType(0) >= 0); }
		
	inline BOOL const mfIsDualClass() const {
		DumbHandlePtr<PLAYER_STATS const> const pPlayerStats(fhPlayerStats);
		return (pPlayerStats->mfGetClassType(1) >= 0); }

	inline void mfGetDualClassInfo(char * const sClass0,
	                               char * const sLevel0,
	                               char * const sClass1,
	                               char * const sLevel1) const {
		DumbHandlePtr<PLAYER_STATS const> const pPlayerStats(fhPlayerStats);
	    strcpy(sClass0,pPlayerStats->mfGetClass(0));
	    strcpy(sClass1,pPlayerStats->mfGetClass(1));
	    sprintf(sLevel0,"%d",(int) pPlayerStats->mfGetLevel(0));
	    sprintf(sLevel1,"%d",(int) pPlayerStats->mfGetLevel(1)); }
	
	inline void mfGetSingleClassInfo(char * const sClass0,
	                               char * const sLevel0 ) const {
		DumbHandlePtr<PLAYER_STATS const> const pPlayerStats(fhPlayerStats);
	    strcpy(sClass0,pPlayerStats->mfGetClass(0));
	    sprintf(sLevel0,"%d",(int)pPlayerStats->mfGetLevel(0)); }

	inline void mfCheckStatsDead() {
		if (fhPlayerStats != fERROR)
		{
			DumbHandlePtr<PLAYER_STATS const> const pPlayerStats(fhPlayerStats);
			if (pPlayerStats->mfGetCurHitPoints() <= 0)
			{
				mfSetDead();
			}
			else
			{
				mfSetAlive();
			}
		} }
	
	inline void mfShowInventory() const {
		SetStatusRealm(mfGetRealm());
		// show character spells inventory
		fPreparingSpells=TRUE;
		StaticInventoryGUI::mfDisplay(0, fhPlayerStats); }
	
	inline void mfShowStatus() const {
		SetStatusRealm(mfGetRealm());
		// show character spells inventory
		ShowStatus(0, fhPlayerStats); }
protected:
private:
	inline REALM::REALM_TYPE const mfGetRealm() const {
		// return (REALM::REALM_TYPE)regents[fAdventurerRegentIndex].Realm; 
		// Everyone appears to be from ROESONE.
		return REALM::ROESONE;}

	UBYTE	fAdventurerRegentIndex;
	UBYTE	fDead;
	UBYTE	fInParty;
	SHORT	fhPlayerStats;
};

//static UBYTE Adventurers[LIST_SIZE] = {
static DEFINE_STATIC_VECTOR_DATA(CANIDATES,Adventurers) = { 
 CANIDATES(0, 0, 0),
 CANIDATES(0, 0, 0),
 CANIDATES(0, 0, 0),
 CANIDATES(0, 0, 0),
 CANIDATES(0, 0, 0),
 CANIDATES(0, 0, 0),
 CANIDATES(0, 0, 0),
 CANIDATES(0, 0, 0) };
DEFINE_STATIC_VECTOR_CLASS(CANIDATES,Adventurers);

class ADVENTURE_PRACTICE_SITE {
public:
	typedef enum {
					NEVER_BEEN_THERE	= 0,
					VISITED_SITE		= 1,
					FOUND_RELIC 		= 2,
				} SEEN_STATE;
				
	inline ADVENTURE_PRACTICE_SITE(UBYTE const AdvIndex,
	                               LONG SiteTitle,
	                               SEEN_STATE SeenSite) :
	       fAdvIndex(AdvIndex),
	       fSiteTitle(SiteTitle),
	       fSeenSite(SeenSite) {}
	
	inline char const * const mfGetSiteTitle() const {
		return STRMGR_GetStr(fSiteTitle); }
	
	inline char const * const mfGetSiteName() const {
		return place_names[advsite[fAdvIndex].iPlaces]; }
	
	inline char const * const mfGetSiteSceneName() const {
		return advsite[fAdvIndex].name; }
	
	inline BOOL const mfEverVisitedSite() const {
		return fSeenSite != NEVER_BEEN_THERE; }

	inline BOOL const mfWonSite() const {
		return fSeenSite == FOUND_RELIC; }
		
	inline void mfSetSiteState(SEEN_STATE const flag ) {
		fSeenSite = flag; }
		
	inline SEEN_STATE const mfGetSiteState() const {
		return fSeenSite; }
		
	inline LONG const mfGetSiteDifficulty() const {
		return advsite[fAdvIndex].difficulty; }
		
	inline SEEN_STATE const mfSeenSite() const {
		return  fSeenSite; }

	inline BOOL const mfReadTextFile(char * const TextBuffer,
	                                 LONG const sizeBuffer) const {
		char buffer[128];
		FILE	*in;
		BOOL	Result = FALSE;
		
		sprintf(buffer,"%sTEXT\\%s.txt", InstallPath, advsite[fAdvIndex].name);
		
		in = FileOpen(buffer, "r");
		if(in != NULL)
		{
			fread(TextBuffer, sizeBuffer, 1, in);
			FileClose(in);
			Result = TRUE;
		}
		return Result; }
	
	inline SHORT const mfGetBitmap() const {
		char buffer[128];
		
		sprintf(buffer,"UI\\ADVSCN\\%s.pcx", advsite[fAdvIndex].name);
		return GetResourceStd (buffer, FALSE); }

	inline SHORT const mfGetPlaceIndex() const {
		return advsite[fAdvIndex].iPlaces;
	}
		
protected:
private:
	UBYTE	const	fAdvIndex;
	LONG	const	fSiteTitle;
	SEEN_STATE		fSeenSite;
};

static DEFINE_STATIC_VECTOR_DATA(ADVENTURE_PRACTICE_SITE,SiteList) = {
	ADVENTURE_PRACTICE_SITE( 0,STR_BA_TITLE_1,ADVENTURE_PRACTICE_SITE::NEVER_BEEN_THERE),	// "Sufhanie ,The Five Peaks",	
	ADVENTURE_PRACTICE_SITE( 1,STR_BA_TITLE_2,ADVENTURE_PRACTICE_SITE::NEVER_BEEN_THERE), 	// "Rivien province of Boeruine",	 
	ADVENTURE_PRACTICE_SITE( 4,STR_BA_TITLE_3,ADVENTURE_PRACTICE_SITE::NEVER_BEEN_THERE),	// "Redoubt province of Boeruine",	 
	ADVENTURE_PRACTICE_SITE( 2,STR_BA_TITLE_4,ADVENTURE_PRACTICE_SITE::NEVER_BEEN_THERE),	// "Floodspaeth, The Five Peaks",	 
	ADVENTURE_PRACTICE_SITE(13,STR_BA_TITLE_5,ADVENTURE_PRACTICE_SITE::NEVER_BEEN_THERE),	// "Doom's Peak area in Thuazor", 
	ADVENTURE_PRACTICE_SITE( 6,STR_BA_TITLE_6,ADVENTURE_PRACTICE_SITE::NEVER_BEEN_THERE),	// "Deseirain province of Alamie", 
	ADVENTURE_PRACTICE_SITE(18,STR_BA_TITLE_7,ADVENTURE_PRACTICE_SITE::NEVER_BEEN_THERE),	// "Ansien province of Elinie", 
	ADVENTURE_PRACTICE_SITE(25,STR_BA_TITLE_8,ADVENTURE_PRACTICE_SITE::NEVER_BEEN_THERE),	// "The Spiderfell" 
};
DEFINE_STATIC_VECTOR_CLASS(ADVENTURE_PRACTICE_SITE,SiteList);

// list of sites we've been to and the result
//static UBYTE SeenSite[LIST_SIZE] = {0};

static SHORT SeenAllSites = 0;


static SHORT iTroopRealm = 0;
static SHORT iTroopType = A_INF_UNIT_ICON;
static SHORT iTroopId = -1;
static SHORT iTroopScale = FULL_SCALE;
static SHORT iTroopWidth;
static THINGTYPE iTroopThingType = INFANTRY;

static BYTE iMyListSel = 0;
static SHORT iMyListStart = -1;
static BYTE iHisListSel = 0;
static SHORT iHisListStart = -1;
static SHORT iTerrainType = 0;
static UBYTE iSite = 0;
static LONG  iPartyLead = 0;
static SHORT iLocalCheck[2];

static BOOL	OldFoundMyHead = TRUE, OldFoundHisHead = TRUE;

static BOOL	FightMode = TRUE, OldFightMode = FALSE;
static BOOL PracticeMode = FALSE;
static BOOL GoneFighting = FALSE;

static SHORT CurrentScenario = 1;
static SHORT ScenarioCount = 30;
static CHAR ScenarioTitles[30][40] = {0};
static CHAR	WonTable[50];

static SHORT	BtlSpeed, BtlDiff;

static BOOL DisplayTroops = FALSE;
static BOOL fSaveInventories = FALSE;

BOOL fPractice = FALSE;
SHORT BARelicFound = 0;

extern SHORT	sMenusUp;
extern BOOL		fPreparingSpells;	// from invngui.cpp
extern BOOL		fTutorialSelected;	// from gamemap.cpp
extern LONG		fControlMode;
extern BOOL		char_selected;		// from game.cpp
extern LONG		new_treasury;
extern LONG		TutorialActionNumber;
extern LONG		TutorialScreenNumber;
extern LONG		map_x;
extern LONG		map_y;
extern LONG		req_x;
extern LONG		req_y;
extern LONG		desired_scale;
extern LONG		max_scale;
extern LONG		map_scale;
extern LONG		DifficultyLevel;	// 1 is most difficult
extern LONG		SpeedLevel;			// 1 is fastest
extern LONG		iOldSite;
extern SHORT	fFadedOut;
extern SHORT	iSelectedUnit;
extern SHORT	iUnitInfo;
extern SHORT	iPlaceInfo;
extern SHORT	iProvInfo;
extern SHORT	oldAction;				/* actnmenu.cpp */
extern BOOL		fTutorialFirstBattle;

/* -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   ----------------------------------------------------------------- */

/* ========================================================================
   Function    - BuildArmy routines
   Description - run the build army dialog
   Returns     - 
   ======================================================================== */
LONG	BB_CurrentUnitSel = -1;
LONG	BB_WarCardIcon = -1;

void RunBattlesOnly(void)
{
	SHORT i;
	
	HideMenu(D_GAMEBUTTON);
	push_regions();
	FadeOut(100);
	clear_display();
	update_screen();
	init_pal("nova_l");
	init_shade_table("nova_l");
	
	ShowPanel(D_BUILD_ARMY);
	ShowMenu(D_GAMEBUTTON);
	
	if (GoneFighting && !PracticeMode)
	{
		// if we won this fight
		// GWP  Wrong test if (SCENE_MGR::Victor == SCENE_MGR::HomeIndex)
		if (SCENE_MGR::VictorRealm == SCENE_MGR::Aggressor)
		{
			WonTable[CurrentScenario] = 1;
			for (i=1;i<=ScenarioCount;i++)
			{
				if(WonTable[i] == 0)
					break;
			}
			// you've won!
			if (i == ScenarioCount+1)
			{
				// only if you haven't already seen this
				if (!WonTable[0])
					PlayBtlFinale();
				else
					WonTable[0] = 1;
			}
			WriteBtlStatFile();
		}
	}
	
	// set terrain sub type
	if (SCENE_MGR::TerrainSubType == -1)
		SCENE_MGR::TerrainSubType = 0;
	
	BuildArmyReload(0,0);
	
	if (DisplayTroops)
	{
		InitTroopDisplay (BB_UnitTypes[0].IconType, BB_UnitTypes[0].Id, 0);
		DisplayTroops = !DisplayTroops;
		BuildDisplayToggle(0, 0);
	}
	else
	{
		DisplayTroops = !DisplayTroops;
		BuildDisplayToggle(0, 0);
	}

	// back from fighting
	GoneFighting = FALSE;
	// not practicing yet
	PracticeMode = FALSE;
	// turn on the fight mode
	FightMode = TRUE;
	
	// reset the Done display variables
	OldFoundMyHead = TRUE; OldFoundHisHead = TRUE;OldFightMode = FALSE;
	
	// tell the world we are only practicing
	fPractice = TRUE;

	// now paint once
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	FadeIn(100);
}

void BuildArmyPaint (LONG MenuCombo, LONG )
{
	SHORT	i,j;
	LONG    X,Y,W,H;
	LONG    mx,my;
	CHAR    color = 228;
	char    textbuf[3000];
	char    buff[80];
	LONG    MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	BOOL	FoundMyHead = FALSE, FoundHisHead = FALSE;
	LONG OrigColor;

	// what is the current Scenario
// STR_BB_Scenario_TEXT	^F02Scenario ^F03%hd ^F02of 50
	// strcpy(buff, "^F02Scenario ^F03%hd ^F02of %hd");
	sprintf(buff, "%s", STRMGR_GetStr(STR_BB_SCENARIO_TEXT));
	sprintf(textbuf, buff, CurrentScenario, ScenarioCount);
	gprint_text( 486, 18, textbuf, BLACK );
	
// STR_OPT_BAT_DIF		Battle Difficulty
// STR_OPT_DIF_EASY		Easy
// STR_OPT_DIF_MEDIUM	Medium
// STR_OPT_DIF_HARD		Hard
	sprintf(buff, "^F02%s:", STRMGR_GetStr(STR_OPT_BAT_DIF));
	sprintf(textbuf, "%s %s", buff, STRMGR_GetStr(STR_OPT_DIF_HARD-BtlDiff));
	gprint_text( 486, 39, textbuf, BLACK );
	
// STR_OPT_BAT_SPD		Battle Speed
// STR_OPT_SPD_SLOW		Slow
// STR_OPT_SPD_MEDIUM	Medium
// STR_OPT_SPD_FAST		Fast
	sprintf(buff, "^F02%s:", STRMGR_GetStr(STR_OPT_BAT_SPD));
	sprintf(textbuf, "%s %s", buff, STRMGR_GetStr(STR_OPT_SPD_FAST-BtlSpeed));
	gprint_text( 486, 50, textbuf, BLACK );
	
	// first my units
	//init_gfont(FONT_SANS_12PT);
	//mx = 18;
	//my = 88;
	// check joining units
	j = BB_MYARMY1;
	for (i = 970; i < 985; i++)	// scan for units here
	{
		// find head of list that isn't me
		if (units[i].Joined == FALSE &&
			units[i].Realm != REALM::NO_COUNTRY
			)
		{
			
			FoundMyHead = TRUE;
			//sprintf( textbuf, "%s", STRMGR_GetStr(gsUnitTitle[units[i].Icon]));
			//gprint_text( mx, my, textbuf, YELLOW );
			//my += 16;
			GetButtonLabelColor(MenuId, j, &OrigColor);
			SetButtonLabel(MenuId, j, gsUnitTitle[units[i].Icon], OrigColor);
			j++;
			
			while (units[i].NextUnit != -1 )
			{
				i = units[i].NextUnit;
				//sprintf( textbuf, "%s", STRMGR_GetStr(gsUnitTitle[units[i].Icon]));
				//gprint_text( mx, my, textbuf, YELLOW );
				//my += 16;
				GetButtonLabelColor(MenuId, j, &OrigColor);
				SetButtonLabel(MenuId, j, gsUnitTitle[units[i].Icon], OrigColor);
				j++;
			}
			break;
		}
	}
	
	for (; j <= BB_MYARMY15; j++)
	{
		GetButtonLabelColor(MenuId, j, &OrigColor);
		SetButtonLabel(MenuId, j, -1, OrigColor);
	}
	
	// enemy units
	//mx = 491;
	//my = 88;
	// check joining units
	j = BB_HISARMY1;
	for (i = 985; i < 1000; i++)
	{
		// find head of list that isn't me
		if (units[i].Joined == FALSE &&
			units[i].Realm != REALM::NO_COUNTRY
			)
		{
			FoundHisHead = TRUE;
			//sprintf( textbuf, "%s", STRMGR_GetStr(gsUnitTitle[units[i].Icon]));
			//gprint_text( mx, my, textbuf, YELLOW );
			//my += 16;
			GetButtonLabelColor(MenuId, j, &OrigColor);
			SetButtonLabel(MenuId, j, gsUnitTitle[units[i].Icon], OrigColor);
			j++;
			
			while (units[i].NextUnit != -1 )
			{
				i = units[i].NextUnit;
				//sprintf( textbuf, "%s", STRMGR_GetStr(gsUnitTitle[units[i].Icon]));
				//gprint_text( mx, my, textbuf, YELLOW );
				//my += 16;
				GetButtonLabelColor(MenuId, j, &OrigColor);
				SetButtonLabel(MenuId, j, gsUnitTitle[units[i].Icon], OrigColor);
				j++;
			}
			break;
		}
	}
	
	for (; j <= BB_HISARMY15; j++)
	{
		GetButtonLabelColor(MenuId, j, &OrigColor);
		SetButtonLabel(MenuId, j, -1, OrigColor);
	}
	
	// switch done button on if units selected on both sides
	if (FoundMyHead && FoundHisHead)
	{
		if ((FoundMyHead != OldFoundMyHead) ||
			(FoundHisHead != OldFoundHisHead) ||
			(FightMode != OldFightMode) )
		{
			OldFoundMyHead = FoundMyHead;
			OldFoundHisHead = FoundHisHead;
			OldFightMode = FightMode;
			fUpdatePanels = TRUE;
			SetButtonLabelColor(MenuId, BB_PRACTICE, BTN_LABEL_COLOR);
			activate_region(BB_PRACTICE, TRUE);
			if(FightMode)
			{
				SetButtonLabelColor(MenuId, BB_FIGHT, BTN_LABEL_COLOR);
				activate_region(BB_FIGHT, TRUE);
			}
			else
			{
				SetButtonLabelColor(MenuId, BB_FIGHT, GREY);
				activate_region(BB_FIGHT, FALSE);
			}
		}
	}
	else
	{
		if ((FoundMyHead != OldFoundMyHead) ||
			(FoundHisHead != OldFoundHisHead) )
		{
			OldFoundMyHead = FoundMyHead;
			OldFoundHisHead = FoundHisHead;
			fUpdatePanels = TRUE;
			SetButtonLabelColor(MenuId, BB_PRACTICE, GREY);
			activate_region(BB_PRACTICE, FALSE);
			SetButtonLabelColor(MenuId, BB_FIGHT, GREY);
			activate_region(BB_FIGHT, FALSE);
		}
	}
		
	if (DisplayTroops)
		PaintTroopList();
	else
		PaintBattleScenarios();
}


void BuildListSel (LONG MenuCombo, LONG )
{
	SHORT	i;
	LONG    MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
 	
 	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
	if(mouse_button == 2)
	{
		SystemHelp(STR_BB_HELP_LISTSEL_TITLE, STR_BB_HELP_LISTSEL_TEXT, -1, NULL, 0);
	}
	else
	{
		if(ButtonId < BB_HISARMY1)	// my list
		{
			iMyListStart = 970;
			iMyListSel = ButtonId - BB_MYARMY1;
		}
		else						// his list
		{
			iHisListStart = 985;
			iHisListSel = ButtonId - BB_HISARMY1;
		}
	
		for(i=BB_MYARMY1;i<=BB_MYARMY15;i++)
		{
			//SetButtonType(MenuId, i, BUTTON_REGION);
			if (ButtonId >= BB_MYARMY1 && ButtonId <= BB_MYARMY15)
				SetButtonHilight(MenuId, i, FALSE);
		}
		for(i=BB_HISARMY1;i<=BB_HISARMY15;i++)
		{
			//SetButtonType(MenuId, i, BUTTON_REGION);
			if (ButtonId >= BB_HISARMY1 && ButtonId <= BB_HISARMY15)
				SetButtonHilight(MenuId, i, FALSE);
		}
		
		// SetButtonType(MenuId, ButtonId, BUTTON_COLORBEVEL);
		SetButtonHilight(MenuId, ButtonId, TRUE);
	}
	fUpdatePanels = TRUE;
	
	// Extra paint because the troop list is done in the paint routine.
	RunPanels();
	fUpdatePanels = TRUE;
}

void BuildListAdd (LONG MenuCombo, LONG )
{
	SHORT 	i;
	LONG    MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
		
	// click the button
	if (mouse_button)
	{
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
	}
	
  	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
	if(mouse_button == 2)
	{
		SystemHelp(STR_BB_HELP_LISTADD_TITLE, STR_BB_HELP_LISTADD_TEXT, -1, NULL, 0);
	}
	else
	{
		// any edits turn off the fight mode
		FightMode = FALSE;
		
		// for(i=BB_MYARMY1;i<=BB_MYARMY15;i++)
		// 	SetButtonType(MenuId, i, BUTTON_REGION);
		// for(i=BB_HISARMY1;i<=BB_HISARMY15;i++)
		// 	SetButtonType(MenuId, i, BUTTON_REGION);
		
		
		if (iTroopId != -1)
		{
			if( ButtonId == BB_MY_ADD)
			{
				AddUnit(970, GHORIED, iTroopType, iTroopId, REALM::ROESONE );
				iMyListStart = -1;
				iMyListSel = -1;
			}
			else	
			{
				AddUnit(985, BHELINE, iTroopType, iTroopId, REALM::GHOERE );
				iHisListStart = -1;
				iHisListSel = -1;
			}
		}
	
		for (i = BB_MYARMY1; i <= BB_MYARMY15; i++)
		{
			SetButtonHilight(MenuId, i, FALSE);
		}
		
		for (i = BB_HISARMY1; i <= BB_HISARMY15; i++)
		{
			SetButtonHilight(MenuId, i, FALSE);
		}
	}
		
	fUpdatePanels = TRUE;
	// Run this twice, because we use the paint routine to set the strings.
	RunPanels();
	fUpdatePanels = TRUE;
}

void BuildListRemove (LONG MenuCombo, LONG )
{
	LONG    MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	// click the button
	if (mouse_button)
	{
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
	}
	
  	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
	if(mouse_button == 2)
	{
		SystemHelp(STR_BB_HELP_LISTREM_TITLE, STR_BB_HELP_LISTREM_TEXT, -1, NULL, 0);
	}
	else
	{
		LONG 	i;
		// any edits turn off the fight mode
		FightMode = FALSE;
		
		if (ButtonId == BB_MY_REMOVE)
		{
			if(iMyListStart != -1)
				DelUnit(iMyListStart, iMyListSel);
			iMyListStart = -1;
			iMyListSel = -1;
		
			for(i=BB_MYARMY1;i<=BB_MYARMY15;i++)
			{
				SetButtonHilight(MenuId, i, FALSE);
			}
		}
		else
		{
			if(iHisListStart != -1)
				DelUnit(iHisListStart, iHisListSel);
			iHisListStart = -1;
			iHisListSel = -1;
			
			for(i=BB_HISARMY1;i<=BB_HISARMY15;i++)
			{
				SetButtonHilight(MenuId, i, FALSE);
			}
		}
	}
	fUpdatePanels = TRUE;
	
	// Run this twice, because we use the paint routine to set the strings.
	RunPanels();
	fUpdatePanels = TRUE;
}

void BuildListClear (LONG MenuCombo, LONG )
{
	SHORT 	i;
	SHORT	MenuId, ButtonId;

	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	if (mouse_button)
	{
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
	}
	
  	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
	if(mouse_button == 2)
	{
		SystemHelp(STR_BB_HELP_LISTCLR_TITLE, STR_BB_HELP_LISTCLR_TEXT, -1, NULL, 0);
	}
	else
	{
		LONG i;
		
		memset(&units[970], 0, 15 * sizeof(UNIT_TYPE));	
		memset(&units[985], 0, 15 * sizeof(UNIT_TYPE));	
		
		iMyListStart = -1;
		iMyListSel = -1;
		
		iHisListStart = -1;
		iHisListSel = -1;
		
		for (i = BB_MYARMY1; i <= BB_MYARMY15; i++)
		{
			SetButtonHilight(MenuId, i, FALSE);
		}
		
		for (i = BB_HISARMY1; i <= BB_HISARMY15; i++)
		{
			SetButtonHilight(MenuId, i, FALSE);
		}
	}
	fUpdatePanels = TRUE;
	// Extra paint because the troop list is done in the paint routine.
	RunPanels();
	fUpdatePanels = TRUE;
}

void BuildUnitSel (LONG ButtonId, LONG )
{
	SHORT 	i;

  	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
	if(mouse_button == 2)
	{
		SystemHelp(STR_BB_HELP_UNITSEL_TITLE, STR_BB_HELP_UNITSEL_TEXT, -1, NULL, 0);
	}
	else
	{
		// for(i=BB_MYARMY1;i<=BB_MYARMY15;i++)
		// 	SetButtonType(D_BUILD_ARMY, i, BUTTON_REGION);
		// for(i=BB_HISARMY1;i<=BB_HISARMY15;i++)
		// 	SetButtonType(D_BUILD_ARMY, i, BUTTON_REGION);
			
		ButtonId -= BB_A_INF_UNIT;
		
		InitTroopDisplay(BB_UnitTypes[ButtonId].IconType, BB_UnitTypes[ButtonId].Id, 0);
	}
	
	fUpdatePanels = TRUE;
	// Extra paint because the troop list is done in the paint routine.
	RunPanels();
	fUpdatePanels = TRUE;
}

void BuildTerrainSel (LONG MenuCombo, LONG WhichTerrain)
{
	SHORT	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	if (mouse_button)
	{
		SetButtonHilight(MenuId, ButtonId, TRUE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		SetButtonHilight(MenuId, ButtonId, FALSE);
	}
	
  	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
	if(mouse_button == 2)
	{
		SystemHelp(STR_BB_HELP_TERRSEL_TITLE, STR_BB_HELP_TERRSEL_TEXT, -1, NULL, 0);
	}
	else
	{
		// any edits turn off the fight mode
		FightMode = FALSE;
		
		SetButtonHilight(D_BUILD_ARMY, BB_PLAINS, FALSE);
		SetButtonHilight(D_BUILD_ARMY, BB_MOUNTAINS, FALSE);
		SetButtonHilight(D_BUILD_ARMY, BB_FOREST, FALSE);
		SetButtonHilight(D_BUILD_ARMY, BB_SWAMP, FALSE);
		
		SetButtonHilight(D_BUILD_ARMY, BB_PLAINS+WhichTerrain, TRUE);
		
		iTerrainType = (SHORT)WhichTerrain;
	}
	fUpdatePanels = TRUE;
}

void BuildArmyRestart (LONG MenuCombo, LONG )
{
	SHORT	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	if (mouse_button)
	{
		
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
	}
	
	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
	if(mouse_button == 2)
	{
		SystemHelp(STR_BB_HELP_RESTART_TITLE,STR_BB_HELP_RESTART_TEXT, -1, NULL, 0);
	}
	else
	{
		// set the proc to null
		SetButtonProc  (D_QUESTION2, 0, BuildArmyRestartPaint, 0, 0 );
		
		// set the question text
		SetButtonLabel  (D_QUESTION2, QUESTION_TEXT, -1, BLACK );
		
		// turn on the first button as YES
		SetButtonLabel  (D_QUESTION2, QUESTION_BTN1, STR_YES, BTN_LABEL_COLOR );
		SetButtonProc   (D_QUESTION2, QUESTION_BTN1, BuildArmyRestartProc, QUESTION_BTN1, D_KEY_YES );
		
		// turn on the last button as CANCEL
		SetButtonLabel  (D_QUESTION2, QUESTION_BTN2, STR_CANCEL, BTN_LABEL_COLOR );
		SetButtonProc   (D_QUESTION2, QUESTION_BTN2, BuildArmyRestartProc, QUESTION_BTN3, D_KEY_CANCEL );
		
		ShowMenu(D_QUESTION2);
	}
}

void BuildArmyRestartPaint(LONG MenuCombo, LONG )
{
	LONG	X,Y,W,H;
	LONG	MenuId, ButtonId;
	CHAR	buffer[256];
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	// get position of lose screen
	if( GetButtonPosition( MenuId, 1, &X, &Y ) == fERROR)
		return;
		
	if( GetButtonSize( MenuId, 1, &W, &H ) == fERROR)
		return;
		
 	X += W / 5;
 	Y += H / 3;
 	// paint dialog title
	sprintf(buffer, "^F03^W%03d%s",W*2/3,STRMGR_GetStr(STR_BB_RESTART_TEXT));
	gprint_text(X,Y,buffer, BLACK);
}

void BuildArmyRestartProc(LONG MenuCombo, LONG button)
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
		memset(&WonTable[0], 0, sizeof(CHAR) * 50 );
		WriteBtlStatFile();
		SetCurrentScenario(1); // reset to first Scenario
		BuildArmyReload (0, 0);
	}
}

void BuildArmyReload (LONG MenuCombo, LONG )
{
	SHORT i;
	
	SHORT	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	if (mouse_button)
	{
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
	}
	
  	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
	if(mouse_button == 2)
	{
		SystemHelp(STR_BB_HELP_RELOAD_TITLE, STR_BB_HELP_RELOAD_TEXT, -1, NULL, 0);
	}
	else
	{
		ClearUnitLists();
		
		ReadBtlStatFile();
		ReadBtlCurrentScenario();
		
		// turn on the fight mode
		FightMode = TRUE;
	}
	fUpdatePanels = TRUE;
	// Extra paint because the troop list is done in the paint routine.
	RunPanels();
	fUpdatePanels = TRUE;
}

void BuildDisplayToggle(LONG MenuCombo, LONG)
{
	SHORT	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	if (mouse_button)
	{
		SetButtonHilight(D_BUILD_ARMY, BB_DISPLAY, TRUE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		SetButtonHilight(D_BUILD_ARMY, BB_DISPLAY, FALSE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
	}
	
  	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
	if(mouse_button == 2)
	{
		SystemHelp(STR_BB_HELP_DISPTOGGLE_TITLE, STR_BB_HELP_DISPTOGGLE_TEXT, -1, NULL, 0);
	}
	else
	{
		LONG OrigColor;
		
		GetButtonLabelColor  (D_BUILD_ARMY, BB_DISPLAY, &OrigColor);
		DisplayTroops = !DisplayTroops;
		if (DisplayTroops)
		{
			SetButtonLabel  (D_BUILD_ARMY, BB_DISPLAY, STR_BB_SCENARIOS, OrigColor );
			AddDisplayTroopsRegions();
			
			// If no troop type is set, default to the first one.
			if (iTroopId == -1)
			{
				iTroopId = 0;
				iTroopType = A_INF_UNIT_ICON;
			}
		}
		else
		{
			SetButtonLabel  (D_BUILD_ARMY, BB_DISPLAY, STR_BB_TROOPS, OrigColor );
			AddScenariosRegions();
			InitScenarioTitles();
			iTroopId = -1;
			iTroopType = fERROR;
		}
	}
	fUpdatePanels = TRUE;
	// Extra paint because the troop list is done in the paint routine.
	RunPanels();
	fUpdatePanels = TRUE;
}

void BuildArmyDone (LONG MenuCombo, LONG )
{
	SHORT 	i;
	SHORT	HomeIndex, AwayIndex;
	LONG    MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	char	strTerrain[10];
	char	cTerrain;
	
	// click the button
	if (mouse_button)
	{
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
	}
	
  	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
	if(mouse_button == 2)
	{
		if (ButtonId == BB_PRACTICE)
			SystemHelp(STR_BB_HELP_PRACTICE_DONE_TITLE, STR_BB_HELP_PRACTICE_DONE_TEXT, -1, NULL, 0);
		else
			SystemHelp(STR_BB_HELP_FIGHT_DONE_TITLE, STR_BB_HELP_FIGHT_DONE_TEXT, -1, NULL, 0);
	}
	else
	{
		// off on a fight
		GoneFighting = TRUE;
		
		if (ButtonId == BB_PRACTICE)
			PracticeMode = TRUE;
		else
			PracticeMode = FALSE;
			
		// for(i=BB_MYARMY1;i<=BB_MYARMY15;i++)
		// 	SetButtonType(MenuId, i, BUTTON_REGION);
		// for(i=BB_HISARMY1;i<=BB_HISARMY15;i++)
		// 	SetButtonType(MenuId, i, BUTTON_REGION);
		
		for (i = 970; i < 985; i++)	// scan for head unit here
		{ 
			if (units[i].Joined == FALSE && units[i].Realm != REALM::NO_COUNTRY )
			{
				break;
			}
		}
		HomeIndex = i;
		
		for (i = 985; i < 1000; i++)	// scan for head unit here
		{ 
			if (units[i].Joined == FALSE && units[i].Realm != REALM::NO_COUNTRY )
			{
				break;
			}
		}
		AwayIndex = i;
		
		if (PracticeMode)
		{
			SCENE_MGR::Aggressor = (REALM::REALM_TYPE) units[AwayIndex].Realm;
			SCENE_MGR::TerrainSubType = -1;
		}
		else
		{
			SCENE_MGR::Aggressor = (REALM::REALM_TYPE) units[HomeIndex].Realm;
		}
			
		SCENE_MGR::Victor = 0; // clear the victor index
		SCENE_MGR::HomeIndex = HomeIndex;
		SCENE_MGR::AwayIndex = AwayIndex;
		SCENE_MGR::SceneType = SCENE_AI::BATTLE_SCENE;  // type for next scene ai
	
		switch(iTerrainType)
		{
		case 1:
			cTerrain = 'm';	// mountains
			break;
		case 2:
			cTerrain = 'w';	// woods
			break;
		case 3:
			cTerrain = 's';	// swamp
			break;
		default:
			cTerrain = 'p';	// plains
		}
	
		sprintf(strTerrain, "batl%c", cTerrain);
	
		ClearRemapTable();
		
		BattlePrep();
		fShowProgressBar = TRUE;
		SCENE_MGR::mfRequestNewScene( strTerrain , TYPE_PLAYERSTART1, TRUE);
		InitDomainTurn(0);		// turn off domain turn
		fPractice = FALSE;
		
		HidePanel(D_BUILD_ARMY);
		pop_regions();
	}
	fUpdatePanels = TRUE;
}
	
/* ========================================================================
   Function    - clear the units list at out end
   Description - 
   Returns     - 
   ======================================================================== */
void ClearUnitLists(void)
{
	SHORT i;
	for (i = 970; i < 1000; i++)
	{ 
		units[i].Joined = FALSE;
		units[i].Realm = REALM::NO_COUNTRY;
		units[i].NextUnit = -1;
	}
}
	
/* ========================================================================
   Function    - InitTroopDisplay
   Description - init the data for the battle card
   Returns     -
   ======================================================================== */
static void InitTroopDisplay (LONG type, LONG id, LONG realm)
{
	LONG typeID;
	SHORT				hPlayerStats;
	
	iTroopId = id;
	iTroopType = type;
	iTroopRealm = realm;
	
	if (iTroopId > 0)
		typeID = iTroopId;
	else
		typeID = UnitTypeID[iTroopType];
		

	iTroopThingType = GetThingTypeFromID(typeID);
	

	run_timers();
}

/* ========================================================================
   Function    - PaintTroopCard
   Description - paint proc for the battle card menu
   Returns     -
   ======================================================================== */
static void PaintTroopCard()
{
	SHORT	i;
	LONG	X,Y;
	LONG	mx,my;
	SHORT	tx,ty;
	LONG	MenuId, ButtonId;
	CHAR	buffer[150];
	LONG	timedif;
	SHORT	iBitm;
	
	// if not set, we have nothing to paint
	if(iTroopType == fERROR)
		return;
		
	// location of battle card art
	mx = 160;
	my = 0;
	
	/* The art */
	iBitm = GetResourceStd ("UI\\warcard.pcx", FALSE);
	DrawBitmap (mx, my, iBitm, 0, 0, 320, 192 );
	SetPurge(iBitm);
	
	/* Name     */
	init_gfont(FONT_SANS_16PT);
	sprintf( buffer, "%s", STRMGR_GetStr(gsUnitTitle[iTroopType]));
	gprint_text( mx+10, my+10, buffer, BLACK );
	
	init_gfont(FONT_SANS_12PT);
	tx = (SHORT)mx + 12;
	ty = (SHORT)my + 35;
	
	/* Move		*/
	sprintf( buffer, STRMGR_GetStr(BTLUI_UNITINFO_INFO_MOVE), GetBattleMove(iTroopType));
	gprint_text( tx, ty, buffer, BLACK );
	ty += 16;
	
	/* Defend	*/
	sprintf( buffer, STRMGR_GetStr(BTLUI_UNITINFO_INFO_DEFEND), GetBattleDefense(iTroopType));
	gprint_text( tx, ty, buffer, BLACK );
	ty += 16;
	
	/* Morale   */
	sprintf( buffer, STRMGR_GetStr(BTLUI_UNITINFO_INFO_MORALE), GetBattleMorale(iTroopType));
	gprint_text( tx, ty, buffer, BLACK );
	ty += 16;
	
	tx = (SHORT)mx + 118;
	ty = (SHORT)my + 35;
	
	/* Melee	*/
	sprintf( buffer, STRMGR_GetStr(BTLUI_UNITINFO_INFO_MELEE), GetBattleMelee(iTroopType));
	gprint_text( tx, ty, buffer, BLACK );
	ty += 16;
	
	/* Charge	*/
	sprintf( buffer, STRMGR_GetStr(BTLUI_UNITINFO_INFO_CHARGE), GetBattleCharge(iTroopType));
	gprint_text( tx, ty, buffer, BLACK );
	ty += 16;
	
	/* Missile	*/
	sprintf( buffer, STRMGR_GetStr(BTLUI_UNITINFO_INFO_MISSILE), GetBattleMissile(iTroopType));
	gprint_text( tx, ty, buffer, BLACK );

	/* Bonuses */

	sprintf( buffer, "^W200%s", STRMGR_GetStr(GetBattleBonus(iTroopType)));
	gprint_text( mx+12, my+100, buffer, BLACK );
	
	// paint picture

	sprintf(buffer,"UI\\PORTS_W\\wd%s.PCX", GAME_TTYPE::mfGetArtFileName(iTroopThingType));
	SHORT hTroopBitm = GetResourceStd(buffer, FALSE);
	if(hTroopBitm != fERROR)
	{
		SetRemapTable(iTroopRealm);
		DrawBitmap(
			(SHORT)(mx+144),(SHORT)(my + 4),
			hTroopBitm, 0, 0, 172, 176);
		ClearRemapTable();
		SetPurge(hTroopBitm);
		hTroopBitm = fERROR;
	}
}

static void AddDisplayTroopsRegions(void)
{
	SHORT y;
	
	// del both possible region types
	del_region(BuildUnitSel,0);
	del_region(SetScenario,0);
	
	// add regions list
	y = 204 + 15;
	add_region( 170, y, 80, 10, 0, BuildUnitSel, BB_A_INF_UNIT,   0, BB_A_INF_UNIT, -1);
	y += 10;
	add_region( 170, y, 80, 10, 0, BuildUnitSel, BB_A_EIN_UNIT,   0, BB_A_EIN_UNIT, -1);
	y += 10;
	add_region( 170, y, 80, 10, 0, BuildUnitSel, BB_A_BOW_UNIT,   0, BB_A_BOW_UNIT, -1);
	y += 10;
	add_region( 170, y, 80, 10, 0, BuildUnitSel, BB_A_IRR_UNIT,   0, BB_A_IRR_UNIT, -1);
	y += 10;
	add_region( 170, y, 80, 10, 0, BuildUnitSel, BB_A_PIK_UNIT,   0, BB_A_PIK_UNIT, -1);
	y += 10;
	add_region( 170, y, 80, 10, 0, BuildUnitSel, BB_A_LVY_UNIT,   0, BB_A_LVY_UNIT, -1);
	y += 10;
	add_region( 170, y, 80, 10, 0, BuildUnitSel, BB_A_SCT_UNIT,   0, BB_A_SCT_UNIT, -1);
	y += 10;
	add_region( 170, y, 80, 10, 0, BuildUnitSel, BB_A_KNT_UNIT,   0, BB_A_KNT_UNIT, -1);
	y += 10;
	add_region( 170, y, 80, 10, 0, BuildUnitSel, BB_A_CAV_UNIT,   0, BB_A_CAV_UNIT, -1);
	y += 15;
	y += 15;
	add_region( 170, y, 80, 10, 0, BuildUnitSel, BB_E_INF_UNIT,   0, BB_E_INF_UNIT, -1);
	y += 10;
	add_region( 170, y, 80, 10, 0, BuildUnitSel, BB_E_BOW_UNIT,   0, BB_E_BOW_UNIT, -1);
	y += 10;
	add_region( 170, y, 80, 10, 0, BuildUnitSel, BB_E_CAV_UNIT,   0, BB_E_CAV_UNIT, -1);
	y += 15;
	y += 15;
	add_region( 170, y, 80, 10, 0, BuildUnitSel, BB_D_INF_UNIT,   0, BB_D_INF_UNIT, -1);
	y += 10;
	add_region( 170, y, 80, 10, 0, BuildUnitSel, BB_D_BOW_UNIT,   0, BB_D_BOW_UNIT, -1);
	
	y = 204 + 15;
	add_region( 270, y, 80, 10, 0, BuildUnitSel, BB_M_INF_UNIT,   0, BB_M_INF_UNIT, -1);
	y += 10;
	add_region( 270, y, 80, 10, 0, BuildUnitSel, BB_M_BOW_UNIT,   0, BB_M_BOW_UNIT, -1);
	y += 10;
	add_region( 270, y, 80, 10, 0, BuildUnitSel, BB_M_IRR_UNIT,   0, BB_M_IRR_UNIT, -1);
	y += 10;
	add_region( 270, y, 80, 10, 0, BuildUnitSel, BB_M_PIK_UNIT,   0, BB_M_PIK_UNIT, -1);
	y += 10;
	add_region( 270, y, 80, 10, 0, BuildUnitSel, BB_M_CAV_UNIT,   0, BB_M_CAV_UNIT, -1);
	y += 15;
	y += 15;
	add_region( 270, y, 80, 10, 0, BuildUnitSel, BB_G_INF_UNIT,   0, BB_G_INF_UNIT, -1);
	y += 10;
	add_region( 270, y, 80, 10, 0, BuildUnitSel, BB_G_BOW_UNIT,   0, BB_G_BOW_UNIT, -1);
	y += 10;
	add_region( 270, y, 80, 10, 0, BuildUnitSel, BB_G_CAV_UNIT,   0, BB_G_CAV_UNIT, -1);
	y += 15;
	y += 15;
	add_region( 270, y, 80, 10, 0, BuildUnitSel, BB_N_INF_UNIT,   0, BB_N_INF_UNIT, -1);
	y += 10;
	add_region( 270, y, 80, 10, 0, BuildUnitSel, BB_N_IRR_UNIT,   0, BB_N_IRR_UNIT, -1);
	y += 15;
	y += 15;
	add_region( 270, y, 80, 10, 0, BuildUnitSel, BB_C_SKL_UNIT,   0, BB_C_SKL_UNIT, -1);
	y += 10;
	add_region( 270, y, 80, 10, 0, BuildUnitSel, BB_C_SPD_UNIT,   0, BB_C_SPD_UNIT, -1);
	
	y = 204 + 15;
	add_region( 372, y, 80, 10, 0, BuildUnitSel, BB_REGENT1_UNIT, 0, BB_REGENT1_UNIT, -1);
	y += 10;
	add_region( 372, y, 80, 10, 0, BuildUnitSel, BB_REGENT2_UNIT, 0, BB_REGENT2_UNIT, -1);
	y += 10;
	add_region( 372, y, 80, 10, 0, BuildUnitSel, BB_REGENT3_UNIT, 0, BB_REGENT3_UNIT, -1);
	y += 10;
	add_region( 372, y, 80, 10, 0, BuildUnitSel, BB_REGENT4_UNIT, 0, BB_REGENT4_UNIT, -1);
	y += 10;
	add_region( 372, y, 80, 10, 0, BuildUnitSel, BB_REGENT5_UNIT, 0, BB_REGENT5_UNIT, -1);
	y += 10;
	add_region( 372, y, 80, 10, 0, BuildUnitSel, BB_REGENT6_UNIT, 0, BB_REGENT6_UNIT, -1);
	y += 10;
	add_region( 372, y, 80, 10, 0, BuildUnitSel, BB_GORGON_UNIT,  0, BB_GORGON_UNIT, -1);
	y += 10;
	add_region( 372, y, 80, 10, 0, BuildUnitSel, BB_SPIDER_UNIT,  0, BB_SPIDER_UNIT, -1);
	y += 10;
	add_region( 372, y, 80, 10, 0, BuildUnitSel, BB_RHOUBHE_UNIT, 0, BB_RHOUBHE_UNIT, -1);
	y += 15;
	y += 15;
	add_region( 372, y, 80, 10, 0, BuildUnitSel, BB_LIEUTENANT1_UNIT, 0, BB_LIEUTENANT1_UNIT, -1);
	y += 10;
	add_region( 372, y, 80, 10, 0, BuildUnitSel, BB_LIEUTENANT2_UNIT, 0, BB_LIEUTENANT2_UNIT, -1);
	y += 10;
	add_region( 372, y, 80, 10, 0, BuildUnitSel, BB_LIEUTENANT3_UNIT, 0, BB_LIEUTENANT3_UNIT, -1);
	y += 10;
	add_region( 372, y, 80, 10, 0, BuildUnitSel, BB_LIEUTENANT4_UNIT, 0, BB_LIEUTENANT4_UNIT, -1);
	y += 10;
	add_region( 372, y, 80, 10, 0, BuildUnitSel, BB_LIEUTENANT5_UNIT, 0, BB_LIEUTENANT5_UNIT, -1);
	y += 10;
	add_region( 372, y, 80, 10, 0, BuildUnitSel, BB_LIEUTENANT6_UNIT, 0, BB_LIEUTENANT6_UNIT, -1);
}

static void PaintTroopList()
{
	CHAR	buffer[80];
	SHORT	y;
		
	PaintTroopCard();
	
	y = 204;
	init_gfont(FONT_SANS_8PT);
	strcpy(buffer,STRMGR_GetStr(STR_BB_ANUIRIEN));
	gprint_text(168,y, buffer, 170);
	init_gfont(FONT_SANS_6PT);
	y += 15;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_INFANTRY));
	gprint_text(170,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_ELITE_INFANTRY));
	gprint_text(170,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_ARCHER));
	gprint_text(170,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_IRREGULAR));
	gprint_text(170,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_PIKEMAN));
	gprint_text(170,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_LEVY));
	gprint_text(170,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_SCOUT));
	gprint_text(170,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_HEAVY_CAVALRY));
	gprint_text(170,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_LIGHT_CAVALRY));
	gprint_text(170,y, buffer, 173);
	y += 15;
	init_gfont(FONT_SANS_8PT);
	strcpy(buffer,STRMGR_GetStr(STR_BB_ELVEN));
	gprint_text(168,y, buffer, 170);
	init_gfont(FONT_SANS_6PT);
	y += 15;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_ELF_INFANTRY));
	gprint_text(170,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_ELF_ARCHER));
	gprint_text(170,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_ELF_CAVALRY));
	gprint_text(170,y, buffer, 173);
	y += 15;
	init_gfont(FONT_SANS_8PT);
	strcpy(buffer,STRMGR_GetStr(STR_BB_DWARVEN));
	gprint_text(168,y, buffer, 170);
	init_gfont(FONT_SANS_6PT);
	y += 15;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_DWARF_INFANTRY));
	gprint_text(170,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_DWARF_ARCHER));
	gprint_text(170,y, buffer, 173);
	
	y = 204;
	init_gfont(FONT_SANS_8PT);
	strcpy(buffer,STRMGR_GetStr(STR_BB_MERC));
	gprint_text(268,y, buffer, 170);
	init_gfont(FONT_SANS_6PT);
	y += 15;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_MERC_INFANTRY));
	gprint_text(270,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_MERC_ARCHER));
	gprint_text(270,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_MERC_IRREGULAR));
	gprint_text(270,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_MERC_PIKEMAN));
	gprint_text(270,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_MERC_CAVALRY));
	gprint_text(270,y, buffer, 173);
	y += 15;
	init_gfont(FONT_SANS_8PT);
	strcpy(buffer,STRMGR_GetStr(STR_BB_GOBLIN));
	gprint_text(268,y,buffer, 170);
	init_gfont(FONT_SANS_6PT);
	y += 15;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_GOBLIN_INFANTRY));
	gprint_text(270,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_GOBLIN_ARCHER));
	gprint_text(270,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_GOBLIN_CAVALRY));
	gprint_text(270,y, buffer, 173);
	y += 15;
	init_gfont(FONT_SANS_8PT);
	strcpy(buffer,STRMGR_GetStr(STR_BB_GNOLL));
	gprint_text(268,y,buffer, 170);
	init_gfont(FONT_SANS_6PT);
	y += 15;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_GNOLL_1));
	gprint_text(270,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_GNOLL_IRREGULAR));
	gprint_text(270,y, buffer, 173);
	y += 15;
	init_gfont(FONT_SANS_8PT);
	strcpy(buffer,STRMGR_GetStr(STR_BB_MONSTERS));
	gprint_text(268,y,buffer, 170);
	init_gfont(FONT_SANS_6PT);
	y += 15;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_SKELETON));
	gprint_text(270,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_GIANT_SPIDER));
	gprint_text(270,y, buffer, 173);
	
	y = 204;
	init_gfont(FONT_SANS_8PT);
	strcpy(buffer,STRMGR_GetStr(STR_BB_REGENTS));
	gprint_text(370,y,buffer, 170);
	init_gfont(FONT_SANS_6PT);
	y += 15;
	strcpy(buffer,STRMGR_GetStr(STR_REGENT_NAME_ASSAN_IBN_DAOUTA));
	gprint_text(372,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_REGENT_NAME_GRIMM_GRAYBEARD));
	gprint_text(372,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_REGENT_NAME_HEIRL_DIEM));
	gprint_text(372,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_REGENT_NAME_ISAELIE));
	gprint_text(372,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_REGENT_NAME_CAINE));
	gprint_text(372,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_REGENT_NAME_ELUVIE_CARIELE));
	gprint_text(372,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_REGENT_NAME_GORGON));
	gprint_text(372,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_AVATAR_SPIDER_KING));
	gprint_text(372,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_REGENT_NAME_RHUOBHE));
	gprint_text(372,y, buffer, 173);
	y += 15;
	init_gfont(FONT_SANS_8PT);
	strcpy(buffer,STRMGR_GetStr(STR_BB_LIEUT));
	gprint_text(370,y,buffer, 170);
	init_gfont(FONT_SANS_6PT);
	y += 15;
	strcpy(buffer,STRMGR_GetStr(STR_LIEUT_NAME_AELIES));
	gprint_text(372,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_LIEUT_NAME_HUBAERE_ARMIENDIN));
	gprint_text(372,y,buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_LIEUT_NAME_WIZARD_THE));
	gprint_text(372,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_LIEUT_NAME_RUARCH_ROCKHAMMER));
	gprint_text(372,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_REGENT_NAME_NHOUN));
	gprint_text(372,y, buffer, 173);
	y += 10;
	strcpy(buffer,STRMGR_GetStr(STR_LIEUT_NAME_EYELESS_ONE));
	gprint_text(372,y, buffer, 173);
	
}

static void AddScenariosRegions(void)
{
	SHORT y,i;
	// del both possible region types
	del_region(BuildUnitSel,0);
	del_region(SetScenario,0);
	y = 204 + 15;
	
	for (i=1;i<=ScenarioCount/2;i++)
	{
		add_region( 170, y, 130, 10, 0, SetScenario, i, 0, 0, -1);
		y += 12;
	}
	y = 204 + 15;
	for (;i<=ScenarioCount;i++)
	{
		add_region( 340, y, 130, 10, 0, SetScenario, i, 0, 0, -1);
		y += 12;
	}
}

/* ========================================================================
   Function    - PaintBattleScenarios
   Description - paint proc for the scenarios list
   Returns     -
   ======================================================================== */
static void PaintBattleScenarios()
{
	SHORT	i;
	SHORT	y;
	LONG	mx,my;
	LONG	MenuId, ButtonId;
	CHAR	buffer[80];
	LONG	timedif;
	SHORT	iBitm;
	SHORT	color;
	
	// scene picture
	sprintf(buffer, "ui\\btlscn\\btlscn%hd.pcx", CurrentScenario);
	iBitm = GetResourceStd (buffer, FALSE);
	DrawBitmap (160, 0, iBitm, 0, 0, 320, 192 );
	SetPurge(iBitm);
		
	y = 204;
	init_gfont(FONT_SANS_8PT);
	strcpy(buffer,STRMGR_GetStr(STR_BB_SCENARIO_LABEL));
	gprint_text(270,y, buffer, 170);
	
	init_gfont(FONT_SANS_6PT);
	y += 15;
	
	// column one
	for (i=1;i<=ScenarioCount/2;i++)
	{
		if (CurrentScenario == i)
		{
			color = WHITE;
		}
		else if(WonTable[i])
		{
			color = YELLOW;
		}
		else
		{
			color = BEIGE;
		}
		
		sprintf(buffer, ScenarioTitles[i-1]);
		if(WonTable[i])
		{
			strcat(buffer, "^l145^C127");
			strcat(buffer, STRMGR_GetStr(STR_BA_WON));
			strcat(buffer, "^C000");
		}
			
		gprint_text(170,y, buffer, color);
		y += 12;
	}
	// column two
	y = 219;
	for (;i<=ScenarioCount;i++)
	{
		if (CurrentScenario == i)
		{
			color = WHITE;
		}
		else if(WonTable[i])
		{
			color = YELLOW;
		}
		else
		{
			color = BEIGE;
		}
		sprintf(buffer, ScenarioTitles[i-1]);
		
		if(WonTable[i])
		{
			strcat(buffer, "^l135^C127");
			strcat(buffer, STRMGR_GetStr(STR_BA_WON));
			strcat(buffer, "^C000");
		}
			
		gprint_text(340, y, buffer, color);
		y += 12;
	}
}


/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
static LONG AddUnit (SHORT StartIndex, LONG prov, LONG icon, LONG id, LONG realm)
{
	char	n[40];
	LONG	c=0, i, k, j;
	LONG	f = FALSE;

	j = StartIndex;
	
	// look for a slot
	while (++c <= 15 && units[j].Realm != REALM::NO_COUNTRY)
		j++;
	
	if (c == 16 || j >= MAX_UNITS)
		return fERROR;

	units[j].Realm			= realm;
	units[j].Icon			= icon;
	units[j].Moving			= FALSE;
	units[j].AvoidMove		= FALSE;
	units[j].Joined			= FALSE;
	units[j].NotSpotted		= TRUE;
	units[j].Disguised		= FALSE;
	units[j].Hastened		= FALSE;
	units[j].Blessed		= FALSE;
	units[j].Paid			= TRUE;
	units[j].Use_Regency	= FALSE;
	units[j].DestProvince	= NO_PROVINCE;
	units[j].MovePart		= 0;
	units[j].id				= (id == 0)? UnitTypeID[icon]:id;
	units[j].NextUnit		= -1;
	sprintf(n,"UI\\%s.PCX",GameIcons[units[j].Icon]);
	units[j].iIconBitm = GetResourceStd(n, FALSE);

	// setting the province tells the system that the unit exists
	units[j].province		= prov;

	// check joining units
	for (i=StartIndex; i<StartIndex+15; i++)	// scan for units here
	{
		// find head of list that isn't me
		if (i != j && 
			units[i].Joined == FALSE &&
			units[i].Realm != REALM::NO_COUNTRY
			)
		{
			k = i;
			while (units[i].NextUnit != -1 )
				i = units[i].NextUnit;
	
			units[i].NextUnit = j;	// join our new unit with the stack
			units[j].Joined = TRUE;
			CheckUnitStack(k, FALSE ); // MP send doesn't matter in this case
			break;
		}
	}
	
	if (id > 0)
	{
		// find this regent and fix the regent.unit
		for (i=0; i < CHARACTER_COUNT; i++)
		{
			if (regents[i].mfGetid() == id)
			{
				regents[i].mfSetunit(j);
				break;
			}
		}
	}
	return 	j;
}


/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
static LONG DelUnit (SHORT StartIndex, SHORT KillIndex)
{
	char	n[40];
	LONG	c=0, i, k, j;
	LONG	f = FALSE;

	for (i = StartIndex; i < StartIndex+15; i++)	// scan for head unit here
	{ 
		if (units[i].Joined == FALSE &&
			units[i].Realm != REALM::NO_COUNTRY
			)
			break;
	}
	k = i;
	
	// now scan the list KillIndex times
	j = k;
	for (i=0; i<KillIndex; i++)	// scan for units here
	{
		if (units[j].Realm != REALM::NO_COUNTRY &&
			units[j].NextUnit != -1
			)
		{
			c++;
			j = units[j].NextUnit;
		}
	}
	
	// not valid selection
	if (c != KillIndex)
		return fERROR;
		
	// not joined means this is a leader or single unit
	if (units[j].Joined == FALSE)
	{
		k = units[j].NextUnit;
		if (k != -1)							// if we HAVE followers
			units[k].Joined = FALSE;			// new leader
	}
	else		// unit is a follower
	{
		for (k=StartIndex; k<StartIndex+15; k++)		// scan for previous unit
		{
			if (units[k].NextUnit == j)
			{
				units[k].NextUnit = units[j].NextUnit;
				break;
			}
		}
	}
	
	units[j].Realm = REALM::NO_COUNTRY;		// clear this entry
	return 	j;
}

/* ========================================================================
   Function    - ReadBtlCurrentScenario
   Description - 
   Returns     - 
   ======================================================================== */
static void ReadBtlCurrentScenario(void)
{
	SHORT	i;
	FILE	*fp;
	CHAR	buffer[50];
	SHORT	fileResult;
	
	// read out current Scenario
	fp = FileOpen("btlscn.dat", "r");	
	if (fp == NULL)
		return;

	// get who many scenarios
	fileResult = GetNextLine(fp, buffer, sizeof(buffer));
	if (fileResult == EOF)
	{
		goto close_file;
	}
		
	sscanf(buffer, "%hd", &ScenarioCount);
	
	for (i=1; i<CurrentScenario; i++)
	{
		if (!ReadBtlScenario(fp,FALSE))
			goto close_file;
		ClearUnitLists();
	}
	
	ReadBtlScenario(fp,TRUE);
	BuildTerrainSel (0 , iTerrainType);

close_file:	
	FileClose(fp);
}

/* ========================================================================
   Function    - ReadBtlScenario
   Description - 
   Returns     - 
   ======================================================================== */
static BOOL ReadBtlScenario(FILE *fp, BOOL fCreateUnits)
{
	CHAR	buffer[80], buffer2[40];
	SHORT	fileResult;
	SHORT	i, iType;
	SHORT	unitlist[15];
	SHORT	count;
	SHORT	iTerrainSubType;
	
	// remove old data from the regents array
	ClearRegentUnits();
	
	// get scenario title
	fileResult = GetNextLine(fp, buffer, sizeof(buffer));
	if (fileResult == EOF)
		return FALSE;
		
	// get my list
	fileResult = GetNextLine(fp, buffer, sizeof(buffer));
	if (fileResult == EOF)
		return FALSE;
		
	count = sscanf(buffer, "%hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd", 
		&unitlist[0], &unitlist[1], &unitlist[2], &unitlist[3], &unitlist[4], 
		&unitlist[5], &unitlist[6], &unitlist[7], &unitlist[8], &unitlist[9], 
		&unitlist[10], &unitlist[11], &unitlist[12], &unitlist[13], &unitlist[14] );
		 
	if (fCreateUnits)
	{
		for (i = 0; i < count; i++ )
		{
			AddUnit(970, GHORIED, BB_UnitTypes[unitlist[i]].IconType, BB_UnitTypes[unitlist[i]].Id, REALM::ROESONE );
		}
	}
	
	// get his list
	fileResult = GetNextLine(fp, buffer, sizeof(buffer));
	if (fileResult == EOF)
		return FALSE;
	
	count = sscanf(buffer, "%hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd", 
		&unitlist[0], &unitlist[1], &unitlist[2], &unitlist[3], &unitlist[4], 
		&unitlist[5], &unitlist[6], &unitlist[7], &unitlist[8], &unitlist[9], 
		&unitlist[10], &unitlist[11], &unitlist[12], &unitlist[13], &unitlist[14] );
		 
	if (fCreateUnits)
	{
		for (i = 0; i < count; i++ )
		{
			AddUnit(985, BHELINE, BB_UnitTypes[unitlist[i]].IconType, BB_UnitTypes[unitlist[i]].Id, REALM::GHOERE );
		}
	}
	
	// get terrain type, speed and diff
	fileResult = GetNextLine(fp, buffer, sizeof(buffer));
	if (fileResult == EOF)
		return FALSE;
	sscanf(buffer, "%hd %hd %hd %hd", &iTerrainType, &iTerrainSubType, &BtlSpeed, &BtlDiff);
	SCENE_MGR::TerrainSubType = (BYTE)iTerrainSubType;
	DifficultyLevel = BtlDiff;
	SpeedLevel = BtlSpeed;
	if ( SpeedLevel == 0 )
	{
		fBtlTurnBased = TRUE;
		fBattleNextTurn = 3;
	}
	else
	{
		fBtlTurnBased = FALSE;
		fBattleNextTurn = 0;
	}

	// now modify these globals to zero based from one based
	BtlSpeed--;
	BtlDiff--;
	
	return TRUE;
}

/* ========================================================================
   Function    - InitScenarioTitles
   Description - 
   Returns     - 
   ======================================================================== */
static void InitScenarioTitles(void)
{
	FILE	*fp;
	CHAR	buffer[80];
	SHORT	fileResult;
	SHORT	i;
	
	// read out current Scenario
	fp = FileOpen("btlscn.dat", "r");	
	
	if (fp == NULL)
		return;

	// get who many scenarios
	fileResult = GetNextLine(fp, buffer, sizeof(buffer));
	if (fileResult == EOF)
		goto close_file;
		
	sscanf(buffer, "%hd", &ScenarioCount);
	
	for (i=0; i<ScenarioCount; i++)
	{
		// get scenario title
		fileResult = GetNextLine(fp, buffer, sizeof(buffer));
		if (fileResult == EOF)
			goto close_file ;
			
		strcpy(ScenarioTitles[i], buffer);
	
		// get my list
		fileResult = GetNextLine(fp, buffer, sizeof(buffer));
		if (fileResult == EOF)
			goto close_file ;
		
		// get his list
		fileResult = GetNextLine(fp, buffer, sizeof(buffer));
		if (fileResult == EOF)
			goto close_file ;
		
		// terrain and speed
		fileResult = GetNextLine(fp, buffer, sizeof(buffer));
		if (fileResult == EOF)
			goto close_file ;
		
	}

close_file:	
	FileClose(fp);
}

/* ========================================================================
   Function    - ReadBtlStatFile
   Description - 
   Returns     - 
   ======================================================================== */
static void ReadBtlStatFile(void)
{
	SHORT	i;
	FILE	*fp;
	CHAR	buffer[50];
	SHORT	fileResult;
	
	// read out current Scenario
	fp = fopen("btlsts.dat", "r");	
	
	if (fp == NULL)
		return;

	fileResult = GetNextLine(fp, buffer, sizeof(buffer));
	if (fileResult == EOF)
		 goto close_file;
		
	sscanf(buffer, "%hd", &ScenarioCount);
	
	fileResult = GetNextLine(fp, buffer, sizeof(buffer));
	if (fileResult == EOF)
		goto close_file ;
		
	sscanf(buffer, "%hd", &CurrentScenario);
	
	fileResult = GetNextLine(fp, buffer, sizeof(buffer));
	if (fileResult == EOF)
		goto close_file ;
		
	// save whether I've seen the win screen
	WonTable[0] = (buffer[0] == '0') ? 0 : 1;
	
	for (i=1; i<=ScenarioCount; i++)
		WonTable[i] = (buffer[i] == '0') ? 0 : 1;
		
close_file:
	fclose(fp);
}

/* ========================================================================
   Function    - WriteBtlStatFile
   Description - 
   Returns     - 
   ======================================================================== */
static void WriteBtlStatFile(void)
{
	SHORT	i;
	FILE	*fp;
	CHAR	buffer[50];
	
	// read out current Scenario
	fp = fopen("btlsts.dat", "w");	
	
	if (fp == NULL)
		return;

	fprintf(fp, "%hd\n", ScenarioCount);
	
	fprintf(fp, "%hd\n", CurrentScenario);
	
	for (i=0; i<=ScenarioCount; i++)
		fprintf(fp, "%c", (WonTable[i]==1) ? '1' : '0');
	
	fprintf(fp, "\n");	
	
	fclose(fp);
}

/* ========================================================================
   Function    - SetCurrentScenario
   Description - 
   Returns     - 
   ======================================================================== */
static void SetCurrentScenario(SHORT iValue)
{
	LONG	i;
	FILE	*fp;
	
	CurrentScenario = iValue;
	WriteBtlStatFile();
	
	// Need to clear these vars. when the scenario changes.
	iTroopId = -1;
	iTroopType = fERROR;
	
	iMyListStart = -1;
	iMyListSel = 0;
	
	iHisListStart = -1;
	iHisListSel = 0;
	
	// Turn off the hilights.
	for(i=BB_MYARMY1;i<=BB_MYARMY15;i++)
	{
		SetButtonHilight(D_BUILD_ARMY, i, FALSE);
	}
	for(i=BB_HISARMY1;i<=BB_HISARMY15;i++)
	{
		SetButtonHilight(D_BUILD_ARMY, i, FALSE);
	}
}

void SetScenario(LONG NewScenario, LONG)
{
	if (NewScenario > ScenarioCount)
		return;
	
	SetCurrentScenario(NewScenario);
	BuildArmyReload (0, 0);
}

/* ========================================================================
   Function    - ClearRegentUnits
   Description - clear the unit value out of each regent in the list
   Returns     - 
   ======================================================================== */
void ClearRegentUnits(void)
{
	SHORT	i;
	
	// find this regent and fix the regent.unit
	for (i=0; i < CHARACTER_COUNT; i++)
	{
		regents[i].mfSetunit(-1);
	}
}

/* ========================================================================
	Function    - PlayBtlFinale
	Description - Play the battle win sequence
	Returns     -
	======================================================================== */
void PlayBtlFinale(void)
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
	
	StopRedBook();
	KillSoundsNoFade();

	// this sound doesn't play
	PlayTrack(SND_WIN_GAME_MUSIC1);	
	
	bitm = GetResourceStd ("finale\\finale.pcx", FALSE);
 		
	if (bitm != fERROR)
	{
		DrawBitmap (0, 0, bitm, 0, 0, 640, 480);
		SetPurge(bitm);
	}
	
	sprintf(filename, "finale\\marlae.PCX",name);
	
	bitm = GetResourceStd (filename, FALSE);
 		
	if (bitm != fERROR)
	{
		DrawBitmap (280, 152, bitm, 0, 0, 72, 176);
		SetPurge(bitm);
	}
	
	init_gfont(FONT_TITL_16PT);
	strcpy(buffer,STRMGR_GetStr(STR_BB_FINALE_SCREEN1));
	print_textf(175,30,1,buffer);
	
	init_gfont(FONT_TITL_10PT);
	strcpy(buffer,STRMGR_GetStr(STR_BB_FINALE_SCREEN2));
	print_textf(175,70,1,buffer);
	
	update_screen();
		
	// delay 45 seconds
	now=get_time() + (45 * 182 / 10);
	
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
	
	return;	
}

/* -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   ----------------------------------------------------------------- */
   
/* ========================================================================
   Function    - BuildAdv routines
   Description - build an adventure to go on
   Returns     - 
   ======================================================================== */
void RunAdvOnly(void)
{
	SHORT	i,x,y,me;
	LONG	j;
	
	HideMenu(D_GAMEBUTTON);
	push_regions();
	FadeOut(100);
	clear_display();
	update_screen();
	init_pal("nova_l");
	init_shade_table("nova_l");
	
	ShowPanel(D_BUILD_ADV);
	ShowMenu(D_GAMEBUTTON);
	
	fPractice = TRUE;
	
	// flip button checking toggle
	PartyHasAtLeastOne = FALSE;
	OldPartyHasAtLeastOne = !PartyHasAtLeastOne;
		
	// This call can purge out current Status handles, so call this before
	// initializing the Adventurers array.
	if(fSaveInventories)
		SaveAdvInventories();
	else
		LoadAdvInventories();
	
	
	// activate all the sites
	SetAllAdvSite();
	
	// read in the current Scenario
	ReadAdvScenario();
	
	// if still have an uninitialized array
	for(j = 0; j < LIST_SIZE; j++)
	{
		if (Adventurers[j].mfGetRegentId() == 0)
		{
			SelectAdventures();
			// clear initial party list
			for (i=0; i<LIST_SIZE; i++)
			{
				Adventurers[i].mfSetInParty(FALSE);
			}
			break;
		}
	}
	
	// if coming back from adventure and relic found, level won
	if (BARelicFound)
	{
		BARelicFound = FALSE;
		SiteList[iSite].mfSetSiteState(ADVENTURE_PRACTICE_SITE::FOUND_RELIC);
		
		for (i=0; i<LIST_SIZE; i++)
		{
			if (!SiteList[i].mfWonSite())
				break;
		}
		if (i == LIST_SIZE)	// all sites won
		{
			if (!SeenAllSites)
				PlayAdvFinale();
			SeenAllSites = 1;
		}
		WriteAdvScenario();
	}
	else
	{
		SiteList[iSite].mfSetSiteState(ADVENTURE_PRACTICE_SITE::VISITED_SITE);
	}
	
	// scan the party list for dead bodies
	for (i=0; i<LIST_SIZE; i++)
	{
		Adventurers[i].mfCheckStatsDead();
	}
	
	ClearUnitLists();
	
	// // clear hilight bar
	// for(i=BA_LIST_01;i<=BA_LIST_16;i++)
	// 	SetButtonType(D_BUILD_ADV, i, BUTTON_REGION);
	
	// SetButtonType(D_BUILD_ADV, BA_LIST_01 + iSite, BUTTON_COLORBEVEL);
	// SetButtonType(D_BUILD_ADV, BA_LIST_09, BUTTON_COLORBEVEL);
		
	// add check regions
	del_region(TogglePartyCheck, 0);
	x = START_PARTY_LIST_X + LIST_CHECK_REGION_X;
	y = START_PARTY_LIST_Y;
	for (i = BA_LIST_09; i <= BA_LIST_16; i++)
	{
		// checkbox region
		add_region(x, y+2, 30,14,0,TogglePartyCheck,i-BA_LIST_09,0,0, -1);
		y += INC_LIST_Y;
	}
		
	// load checkmark art
	iLocalCheck[0] = GetResourceStd ("UI\\DCHECK1.PCX", FALSE);
	iLocalCheck[1] = GetResourceStd ("UI\\DCHECK2.PCX", FALSE);
		
	// now paint once
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	FadeIn(100);
}

void BuildAdvPaint (LONG MenuCombo, LONG )
{
	SHORT	i,j;
	LONG    x,y,w,h;
	CHAR    buffer[20];
	char    textbuf[3000];
	char    buff[80];
	CHAR    color = 228;
	LONG    MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	PartyHasAtLeastOne = FALSE;
	
	// current site title
	init_gfont(FONT_SANS_16PT);
	print_text_centered(420,30,(char *)SiteList[iSite].mfGetSiteTitle(), BLACK);
	
	init_gfont(FONT_SANS_8PT);
	// select an adventure site
	x = START_SITE_LIST_X;
	y = START_SITE_LIST_Y;
	
	// add the names of the active adventure sites to the screen
	for(i=0; i<LIST_SIZE; i++)
	{
		SHORT color;
		if (i == iSite)
			color = WHITE;
		else
		{
			switch(SiteList[i].mfSeenSite())
			{
			case ADVENTURE_PRACTICE_SITE::FOUND_RELIC:		// done that
				color = GREY;
				break;
			case ADVENTURE_PRACTICE_SITE::VISITED_SITE:		// been there
				color = BEIGE;
				break;
			default:
				color = YELLOW;
				break;
			}
		}
		gprint_text(x, y, (char *)SiteList[i].mfGetSiteName(), color);
		// add the name and region
		switch(SiteList[i].mfGetSiteDifficulty())
		{
		case 1:
			sprintf( buffer, "^l%d%s", BA_DIFF_COL, STRMGR_GetStr(STR_BA_EASY));
			gprint_text(x, y, buffer, color);
			break;
		case 2:
			sprintf( buffer, "^l%d%s", BA_DIFF_COL, STRMGR_GetStr(STR_BA_MED));
			gprint_text(x, y, buffer, color);
			break;
		case 3:
			sprintf( buffer, "^l%d%s", BA_DIFF_COL, STRMGR_GetStr(STR_BA_HARD));
			gprint_text(x, y, buffer, color);
			break;
		case 4:
			sprintf( buffer, "^l%d%s", BA_DIFF_COL, STRMGR_GetStr(STR_BA_VERYHARD));
			gprint_text(x, y, buffer, color);
			break;
		case 0:
		default:
			break;
		}
		y += INC_LIST_Y;
	}
	
	// select party members
	x = START_PARTY_LIST_X;
	y = START_PARTY_LIST_Y;
	
	// add the names of the active party members to the screen
	for(i=0; i<LIST_SIZE; i++)
	{
		SHORT	color;
		CHAR	sClass0[10], sClass1[10];
		CHAR	sLevel0[10], sLevel1[10];
		SHORT	me = Adventurers[i].mfGetRegentId();
		
		if (fERROR == me)
			continue;
			
		// get name
		sprintf(buff, "%s", Adventurers[i].mfGetName());
		
		// get classes
		if (Adventurers[i].mfIsDualClass())
		{
		    Adventurers[i].mfGetDualClassInfo(sClass0, sLevel0, sClass1, sLevel1);
		    
		    if(sClass0[0] == 'P')
		    	sClass0[2] = 0;
	    	else
		    	sClass0[1] = 0;
		    	
		    if(sClass1[0] == 'P')
		    	sClass1[2] = 0;
	    	else
		    	sClass1[1] = 0;
		    	
		    sprintf(buffer,"%s%s/%s%s", 
		    	sClass0, sLevel0, sClass1, sLevel1 );
		}
		else if (Adventurers[i].mfIsSingleClass())
		{
		    Adventurers[i].mfGetSingleClassInfo(sClass0, sLevel0);
		    
		    if(sClass0[0] == 'P')
		    	sClass0[2] = 0;
	    	else
		    	sClass0[1] = 0;
		    	
		    sprintf(buffer,"%s%s", 
		    	sClass0, sLevel0);
		}
		else
		{
			sprintf(buffer, "Monster");
		}
			
		sprintf(textbuf, "^F02%s ^F01(%s)", buff, buffer);
		    
		if(Adventurers[i].mfIsDead())
			color = GREY;
		else
		{
			if (Adventurers[i].mfIsInParty())
			{
				PartyHasAtLeastOne = TRUE;
				color = WHITE;
			}
			else
				color = YELLOW;
		}
			
		gprint_text(x, y, textbuf, color);
		
		// draw the check mark
		if(!Adventurers[i].mfIsDead())
		{
			DrawBitmap(x+LIST_CHECK_X, y+2, iLocalCheck[Adventurers[i].mfIsInParty()], 0, 0, 20, 20);
		}
		y += INC_LIST_Y;
	}
	
	// check to see if DONE button should be on or off
	if ( PartyHasAtLeastOne != OldPartyHasAtLeastOne)
	{
		OldPartyHasAtLeastOne = PartyHasAtLeastOne;
		fUpdatePanels = TRUE;
		if(PartyHasAtLeastOne)
		{
			SetButtonLabelColor(MenuId, BA_DONE, BTN_LABEL_COLOR);
			activate_region(BA_DONE, TRUE);
		}
		else
		{
			SetButtonLabelColor(MenuId, BA_DONE, GREY);
			activate_region(BA_DONE, FALSE);
		}
	}
	
	// clear the buffer
	memset(&textbuf[0], 0, sizeof(textbuf));
	
	if(SiteList[iSite].mfReadTextFile(textbuf, sizeof(textbuf)))
	{
		if(strlen(textbuf) < 850)
		{
			textbuf[2] = '0';	// change large font
			textbuf[3] = '3';	// 12 point
		}
		else
		{
			textbuf[6] = '0';	// change small font
			textbuf[7] = '2';	// 8 point
		}
		textbuf[6] = '4';	// change format string in local buffer
		textbuf[7] = '1';
		textbuf[8] = '5';
		
		gprint_text(204, 281, textbuf, DKBROWN);
	}
	
	SHORT const iBitm = SiteList[iSite].mfGetBitmap();
	if (iBitm != fERROR)
	{
		DrawBitmap (204, 52, iBitm, 0, 0, 420, 220 );
		SetPurge(iBitm);
	}
}

void BuildAdvDone (LONG MenuCombo, LONG )
{
	SHORT 	i,count=0;
	SHORT	HomeIndex, AwayIndex;
	LONG	me;	  // loop index, unit# of selection
	LONG	unit;		 // index for unit search loop
	LONG	lastGuy = -1; // lockstep link
	LONG    MenuId, ButtonId;
		
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
	
	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
	if(mouse_button == 2)
	{
		SystemHelp(STR_BA_HELP_DONE_TITLE, STR_BA_HELP_DONE_TITLE, -1, NULL, 0);
	}
	else
	{
		// check the inparty list and build a units array list
		for(i=0; i<LIST_SIZE; i++)
		{
			// find out party
			if (Adventurers[i].mfIsInParty())
			{
				AddUnit(970, GHORIED, 
					0, 
					Adventurers[i].mfGetRegentId(), 
					REALM::ROESONE );
				me = Adventurers[i].mfGetUnit();
				if (count == 0) // party lead
				{
					iPartyLead = me;
					units[me].Joined = FALSE;
					units[me].DestProvince = units[me].province;
				}
				else // party members
				{
					units[lastGuy].NextUnit = me;
					units[me].DestProvince = units[iPartyLead].DestProvince;
					units[me].Joined = TRUE;
				}
					
				units[me].NextUnit = -1;
				/* pull me out of the province I'm in */
				units[me].province = NO_PROVINCE;
	
				lastGuy = me;
				++count;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				SHORT const Id = Adventurers[i].mfGetRegentId();

				if (Id >= 1000 && Id < 3000)
				{
					SHORT const hPS = LoadStats(Id, fERROR);
					DumbHandlePtr<PLAYER_STATS> const pPS(hPS);
					LONG pr_spells, wiz_spells;
					LONG k;

					pr_spells = wiz_spells = 0;
					for (k=1; k<=7; ++k)
						pr_spells += pPS->mfCanMemorize(SPELL_INFO::PRIEST, SPELL_INFO::LEVEL(k))
										-  pPS->mfMemorized(SPELL_INFO::PRIEST, SPELL_INFO::LEVEL(k));

					for (k=1; k<=9; ++k)
						wiz_spells += pPS->mfCanMemorize(SPELL_INFO::WIZARD, SPELL_INFO::LEVEL(k))
										-	pPS->mfMemorized(SPELL_INFO::WIZARD, SPELL_INFO::LEVEL(k));

					if (pr_spells > 0 || wiz_spells > 0)
					{
						fPreparingSpells = TRUE;
						Adventurers[i].mfShowInventory();
						while (fPreparingSpells)
							MenuLoop();
					}
					
					// One charge per adventure per sceptre.
					InventoryItor itor(pPS->Inventory);
					while (itor.mfFind(ITEM_SCEPTRE_OF_CUIRAECEN))
					{
						(*itor)->mfSetCharges(1);
					}
					
					// Setup Blood healing abilites for adventures.
					itor = pPS->Inventory.begin();
					if (itor.mfFind(ITEM_HEALING_MINOR))
					{
						(*itor)->mfSetCharges(1);
					}
					
					itor = pPS->Inventory.begin();
					if (itor.mfFind(ITEM_HEALING_MAJOR))
					{
						(*itor)->mfSetCharges(2);
					}
					
					itor = pPS->Inventory.begin();
					if (itor.mfFind(ITEM_HEALING_GREAT))
					{
						(*itor)->mfSetCharges(3);
					}
					
					itor = pPS->Inventory.begin();
					if (itor.mfFind(ITEM_FEAR))
					{
						(*itor)->mfSetCharges(3);
					}
					
					itor = pPS->Inventory.begin();
					if (itor.mfFind(ITEM_TRAVEL))
					{
						(*itor)->mfSetCharges(1);
					}
						
					itor = pPS->Inventory.begin();
					if (itor.mfFind(ITEM_ENHANCED_SENSE))
					{
						// Anduiras'ns get 3 charges of detect evil.
						if (ANDUIRAS == regents[Adventurers[i].mfGetRegentIndex()].mfGetBL_deriv())
						{
							(*itor)->mfSetCharges(3);
						}
						else
						{
							(*itor)->mfSetCharges(255);	// infinite number. well as big as we can hold.
						}
					}
					
					itor = pPS->Inventory.begin();
					if (itor.mfFind(ITEM_PROTECTION_FROM_EVIL))
					{
						(*itor)->mfSetCharges(1);
					}
				}
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			}
		}
		
		if (count == 0)	// no one selected
		{
			SystemHelp("No Party Members Selected", "You haven't selected any party members for this adventure.  Click the box to the right of their name to include them. If everyone is greyed out, press the restart button.", -1, NULL, 0);
			return;
		}
		
		// now, write out the data from this Scenario
		WriteAdvScenario();
		fSaveInventories = TRUE;
		
		SCENE_MGR::Visitors = REALM::ROESONE;
		SCENE_MGR::Aggressor = REALM::GHOERE;
		SCENE_MGR::HomeIndex =  iPartyLead;	// who is the home unit (w/camera)
		SCENE_MGR::AwayIndex = -1;			// who is the away unit
		SCENE_MGR::PlacesIndex = SiteList[iSite].mfGetPlaceIndex();	// where we're going
		SCENE_MGR::SceneType = SCENE_AI::ADVENTURE_SCENE;	// type for next scene ai
		SCENE_MGR::mfRequestNewScene( (char *)SiteList[iSite].mfGetSiteSceneName(), TYPE_PLAYERSTART1, FALSE); // load first scene
					
		fShowProgressBar = TRUE;
		
		InitDomainTurn(0);		// turn off domain turn
		fPractice = FALSE;
		
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
		
		del_region(TogglePartyCheck, 0);
		
		HidePanel(D_BUILD_ADV);
		pop_regions();
	}
	
	fUpdatePanels = TRUE;
}
	
void BuildAdvListSel (LONG MenuCombo, LONG )
{
	SHORT	i,index;
	LONG    MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
	// GEH right button means show status
	//if(mouse_button == 2)
	//{
	//	SystemHelp(STR_BA_HELP_LISTSEL_TITLE, STR_BA_HELP_LISTSEL_TEXT, -1, NULL, 0);
	//}
	//else
	{
		index = ButtonId - BA_LIST_01;
		
		// site selection
		if(index < 8)
		{
			// for(i=BA_LIST_01;i<=BA_LIST_08;i++)
			// 	SetButtonType(MenuId, i, BUTTON_REGION);
			
			// SetButtonType(MenuId, ButtonId, BUTTON_COLORBEVEL);
			
			iSite = index;
		}	
		// party member selection
		else
		{
			index -= 8;
			// for(i=BA_LIST_09;i<=BA_LIST_16;i++)
			// 	SetButtonType(MenuId, i, BUTTON_REGION);
			
			// SetButtonType(MenuId, ButtonId, BUTTON_COLORBEVEL);
		
			if (mouse_button == 1)
			{
				Adventurers[index].mfShowInventory();
			}
			else
			{
				Adventurers[index].mfShowStatus();
			}
		}
		
	}
	fUpdatePanels = TRUE;
}

void BuildAdvRestart (LONG MenuCombo, LONG )
{
	SHORT	MenuId, ButtonId;
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
	
	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
	if(mouse_button == 2)
	{
		SystemHelp(STR_BA_HELP_RESTART_TITLE, STR_BA_HELP_RESTART_TEXT, -1, NULL, 0);
	}
	else
	{
		// set the proc to null
		SetButtonProc  (D_QUESTION2, 0, BuildAdvRestartPaint, 0, 0 );
		
		// set the question text
		SetButtonLabel  (D_QUESTION2, QUESTION_TEXT, -1, BLACK );
		
		// turn on the first button as YES
		SetButtonLabel  (D_QUESTION2, QUESTION_BTN1, STR_YES, BTN_LABEL_COLOR );
		SetButtonProc   (D_QUESTION2, QUESTION_BTN1, BuildAdvRestartProc, QUESTION_BTN1, D_KEY_YES );
		
		// turn on the last button as CANCEL
		SetButtonLabel  (D_QUESTION2, QUESTION_BTN2, STR_CANCEL, BTN_LABEL_COLOR );
		SetButtonProc   (D_QUESTION2, QUESTION_BTN2, BuildAdvRestartProc, QUESTION_BTN3, D_KEY_CANCEL );
		
		ShowMenu(D_QUESTION2);
	}
}

void BuildAdvRestartPaint(LONG MenuCombo, LONG )
{
	LONG	X,Y,W,H;
	LONG	MenuId, ButtonId;
	CHAR	buffer[256];
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	// get position of lose screen
	if( GetButtonPosition( MenuId, 1, &X, &Y ) == fERROR)
		return;
		
	if( GetButtonSize( MenuId, 1, &W, &H ) == fERROR)
		return;
		
 	X += W / 5;
 	Y += H / 3;
 	
 	// paint dialog title
	sprintf(buffer, "^F03^W%03d%s",W*2/3,STRMGR_GetStr(STR_BA_RESTART_TEXT));
	gprint_text(X,Y,buffer, BLACK);
}

void BuildAdvRestartProc(LONG MenuCombo, LONG button)
{
	FILE *fp;
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
	fUpdatePanels = TRUE;
	RunPanels();
	
	if(button == QUESTION_BTN1)
	{
		LONG j;
		
		// clear which sites we've seen
		for (j = 0; j < LIST_SIZE; j++)
		{
			SiteList[j].mfSetSiteState(ADVENTURE_PRACTICE_SITE::NEVER_BEEN_THERE);
		}
		SeenAllSites = 0;
		// clear dead list
		for (j = 0; j < LIST_SIZE; j++)
		{
			Adventurers[j].mfSetAlive();
			Adventurers[j].mfSetInParty(FALSE);
		}
		
		// reset game files
		//GEH 8/25/97
		#ifdef _WINDOWS
		if ( IsMultiPlayer() )
			fp = FileOpen("mpstart.dat", "rb");
		else
		#endif
			fp = FileOpen("rlmstart.dat", "rb");
		LoadGameFiles(fp);
		FileClose(fp);
	    
		if (Exists("advinv.dat"))
			remove("advinv.dat");
		
		master_game_type = GAME_ADVENTURE;
		fTutorialSelected = FALSE;
		
Reselect:		
		// select new party list
		SelectAdventures();
		
		for(LONG i = 0; i < LIST_SIZE; i++)
		{
		 	SHORT j;
		 	
		 	// check to see if already in the units array
		 	for(j = 0;j < MAX_UNITS; j++)
		 	{
		 		if (units[j].id == Adventurers[i].mfGetRegentId())
		 			continue;
		 	}
		 	
		 	// otherwise, create him
		 	j = CreateUnit(GHORIED, Adventurers[i].mfGetIcon(), Adventurers[i].mfGetRegentId(), REALM::ROESONE, TRUE );
			if (j == fERROR)
				goto Reselect;
			
			SetGameData(MP_REGENT, MPREG_REALM, Adventurers[i].mfGetRegentIndex(), REALM::ROESONE, TRUE );
			SetGameData(MP_REGENT, MPREG_UNIT, Adventurers[i].mfGetRegentIndex(), j, TRUE );
		}
		
		// save new information
		WriteAdvScenario();
	}
	fUpdatePanels = TRUE;
}
			

void TogglePartyCheck(LONG i, LONG)
{
	SHORT	j;
	SHORT	count = 0;
	
	for(j=0;j<LIST_SIZE;j++)
	{
		if (Adventurers[j].mfIsInParty())
			count++;
	}
	
	if(Adventurers[i].mfIsInParty())
	{
		Adventurers[i].mfSetInParty(FALSE);
	}
	else
	{
		if (count < 4 && !Adventurers[i].mfIsDead())
			Adventurers[i].mfSetInParty(TRUE);
		else
			AddSndObj((BIRTHRT_SND)SND_UI_NOT_PERMITTED,0,VOLUME_NINETY);
	}
	fUpdatePanels = TRUE;
}

void SelectAdventures(void)
{
	/* select two from each type list */
	Adventurers[0].mfSetRegentIndex(FighterList[random(SizeFighterList)]);
	while ( (Adventurers[1].mfSetRegentIndex(FighterList[random(SizeFighterList)])) == Adventurers[0].mfGetRegentId() )
	{}
	
	Adventurers[2].mfSetRegentIndex(PriestList[random(SizePriestList)]);
	while ( (Adventurers[3].mfSetRegentIndex(PriestList[random(SizePriestList)])) == Adventurers[2].mfGetRegentId() )
	{}
	
	Adventurers[4].mfSetRegentIndex(ThiefList[random(SizeThiefList)]);
	while ( (Adventurers[5].mfSetRegentIndex(ThiefList[random(SizeThiefList)])) == Adventurers[4].mfGetRegentId() )
	{}
	
	Adventurers[6].mfSetRegentIndex(WizardList[random(SizeWizardList)]);
	while ( (Adventurers[7].mfSetRegentIndex(WizardList[random(SizeWizardList)])) == Adventurers[6].mfGetRegentId() )
	{}
	
}

/* ========================================================================
   Function    - WriteAdvScenario
   Description - dump out the adventure data
   Returns     - 
   ======================================================================== */
static void WriteAdvScenario(void)
{
	SHORT	i, count;
	FILE	*fp;
	
	// open Scenario file
	fp = fopen("advscn.dat", "w");	
	
	if (fp == NULL)
		return;

	for (i = 0; i < LIST_SIZE; i++)
	{
		UBYTE const val = (UBYTE) SiteList[i].mfGetSiteState();
		count = fwrite(&val,sizeof(UBYTE),1,fp);
	}
	
	count = fwrite(&SeenAllSites,sizeof(SHORT),1,fp);
	for (i = 0; i < LIST_SIZE; i++)
	{
		UBYTE const val = Adventurers[i].mfIsDead();
		count = fwrite(&val,sizeof(UBYTE),1,fp);
	}
	for (i = 0; i < LIST_SIZE; i++)
	{
		UBYTE const val = Adventurers[i].mfIsInParty();
		count = fwrite(&val,sizeof(UBYTE),1,fp);
	}
	for (i = 0; i < LIST_SIZE; i++)
	{
		UBYTE const val = Adventurers[i].mfGetRegentIndex();
		count = fwrite(&val,sizeof(UBYTE),1,fp);
	}
	count = fwrite(&iSite, sizeof(UBYTE), 1, fp);
	
	fclose(fp);
}

/* ========================================================================
   Function    - ReadAdvScenario
   Description - suck in the adventure data
   Returns     - 
   ======================================================================== */
static void ReadAdvScenario(void)
{
	SHORT	i, count;
	FILE	*fp;
	
	// open Scenario file
	fp = fopen("advscn.dat", "r");	
	
	if (fp == NULL)
		return;

	iSite = 0;
	
	for (i = 0; i < LIST_SIZE; i++)
	{
		UBYTE val;
		count = fread(&val,sizeof(UBYTE),1,fp);
		if (!count)
		{
			for (i--; i>= 0 ;i--)
			{
				SiteList[i].mfSetSiteState(ADVENTURE_PRACTICE_SITE::NEVER_BEEN_THERE);
			}
			goto ExitFn;
		}
		SiteList[i].mfSetSiteState((ADVENTURE_PRACTICE_SITE::SEEN_STATE)val);
	}
	
	count = fread(&SeenAllSites,sizeof(SHORT),1,fp);
	for (i = 0; i < LIST_SIZE; i++)
	{
		UBYTE val;
		count = fread(&val,sizeof(UBYTE),1,fp);
		if (!count)
			goto ExitFn;
			
		if (val)
			Adventurers[i].mfSetDead();
		else
			Adventurers[i].mfSetAlive();
	}
	for (i = 0; i < LIST_SIZE; i++)
	{
		UBYTE val;
		count = fread(&val,sizeof(UBYTE),1,fp);
		if (!count)
			goto ExitFn;
			
		Adventurers[i].mfSetInParty(val);
	}
	for (i = 0; i < LIST_SIZE; i++)
	{
		UBYTE val;
		count = fread(&val,sizeof(UBYTE),1,fp);
		if (!count)
			goto ExitFn;
			
		Adventurers[i].mfSetOnlyRegentIndex(val);
	}
	count = fread(&iSite, sizeof(UBYTE), 1, fp);
	
ExitFn:
	fclose(fp);
}

/* ========================================================================
   Function    - SaveAdvInventories
   Description - dump the 8 inventories to disk
   Returns     - 
   ======================================================================== */
void SaveAdvInventories(void)
{
    // store all the game files
    FILE *fp = fopen("advinv.dat", "wb");
    if(fp != NULL)
    {
	    SaveGameFiles(fp);
    	fclose(fp);
    }

}

/* ========================================================================
   Function    - LoadAdvInventories
   Description - dump the 8 inventories to disk
   Returns     - 
   ======================================================================== */
void LoadAdvInventories(void)
{
    // store all the game files
    FILE *fp = fopen("advinv.dat", "rb");
    if(fp != NULL)
    {
	    LoadGameFiles(fp);
    	fclose(fp);
    }

}

/* ========================================================================
	Function    - PlayAdvFinale
	Description - Play the adventure win sequence
	Returns     -
	======================================================================== */
void PlayAdvFinale(void)
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
	
	StopRedBook();
	KillSoundsNoFade();

	// this sound doesn't play
	PlayTrack(SND_WIN_GAME_MUSIC1);	
	
	bitm = GetResourceStd ("finale\\finale.pcx", FALSE);
 		
	if (bitm != fERROR)
	{
		DrawBitmap (0, 0, bitm, 0, 0, 640, 480);
		SetPurge(bitm);
	}
	
	sprintf(filename, "finale\\marlae.PCX",name);
	
	bitm = GetResourceStd (filename, FALSE);
 		
	if (bitm != fERROR)
	{
		DrawBitmap (280, 152, bitm, 0, 0, 72, 176);
		SetPurge(bitm);
	}
	
	init_gfont(FONT_TITL_16PT);
	strcpy(buffer,STRMGR_GetStr(STR_BA_FINALE_SCREEN1));
	print_textf(175,30,1,buffer);
	
	init_gfont(FONT_TITL_10PT);
	strcpy(buffer,STRMGR_GetStr(STR_BA_FINALE_SCREEN2));
	print_textf(175,70,1,buffer);
	
	update_screen();
		
	// delay 45 seconds
	now=get_time() + (45 * 182 / 10);
	
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
	
	return;	
}

/* -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   ----------------------------------------------------------------- */

/* -----------------------------------------------------------------
   Select game type code follows:
   ----------------------------------------------------------------- */

static LONG lMultiFlagLocal = 0;

void OpenGameSelect(LONG, LONG lMultiFlag)
{
	lMultiFlagLocal = lMultiFlag;
	
	if (IsMenuActive(D_BTLCOMMAND))
	{
	    HideMenu(D_BTLCOMMAND);
	    RunMenus();
	}
	
#if defined(_MULTIPLAY_ONLY)
		SetButtonLabelColor(D_SELECT_GAME, 4, GREY);
		SetButtonLabelColor(D_SELECT_GAME, 5, GREY);
#endif

	ShowMenu(D_SELECT_GAME);
}

void PaintGameSelect(LONG MenuCombo, LONG)
{
	LONG    MenuId, ButtonId;
	LONG	mx, my;
	LONG	mw, mh;
	char	buffer[80];
	LONG	OrigColor;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	GetButtonPosition( MenuId, 0, &mx, &my );
	GetButtonSize( MenuId, 0, &mw, &mh );
    sprintf(buffer, "%s", STRMGR_GetStr(STR_SELECT_TITLE));
	gprint_text(mx+15, my+25, buffer, 168);
}

static SHORT tut_or_game = 0;

void OpenGameComplexity(LONG, LONG)
{
	HideMenu(D_SELECT_GAME);		// or you could do this after
	RunMenus();						// closing D_GAME_COMPLEXITY
	
	ShowMenu(D_GAME_COMPLEXITY);
}

void PaintGameComplexity(LONG MenuCombo, LONG)
{
	LONG    MenuId, ButtonId;
	LONG	mx, my;
	LONG	mw, mh;
	LONG	ox=4, oy=4;
	char	buffer[80];
	LONG	button;
	LONG	OrigColor;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	GetButtonPosition( MenuId, 0, &mx, &my );
	GetButtonSize( MenuId, 0, &mw, &mh );
	init_gfont(13);
    
	sprintf( buffer, "^N%s", STRMGR_GetStr(
		((tut_or_game) ? STR_SELECT_GAME_TITLE : STR_SELECT_TUTORIAL_TITLE)));
    
	gprint_text(mx+15, my+25, buffer, 31);
	
	for (button=2; button <= 4; button++)
	{
		LONG getme = STR_NULL;
		
		switch(button)
		{
			case 2:
				getme = (tut_or_game) ? STR_SELECT_BASIC_GAME : STR_SELECT_BASIC_TUTORIAL;
				break;
			case 3:
				getme = (tut_or_game) ? STR_SELECT_ADVANCED_GAME : STR_SELECT_ADVANCED_TUTORIAL;
				break;
			case 4:
				getme = (tut_or_game) ? STR_SELECT_EXPERT_GAME : STR_SELECT_EXPERT_TUTORIAL;
				break;
			default:
				getme = STR_NULL;
				break;
		}
		GetButtonPosition( MenuId, button, &mx, &my);
		if (IsButtonHilighted(MenuId,button))
			GetButtonHLabelColor(MenuId, button, &OrigColor);
		else
			GetButtonLabelColor(MenuId, button, &OrigColor);
			
		sprintf( buffer, "^F02%s", STRMGR_GetStr(getme));
		gprint_text( mx+ox, my+oy, buffer, OrigColor );
	}
}


void NewGameConfirmed(LONG,LONG lMultiFlag);

void ResetToNonTutorialAdventures (void)
{
	LONG	i;

	advsite[0].name = "endie_mw";
	advsite[0].realm = REALM::ALAMIE;
	advsite[0].iPlaces = 54;
}

void SelectGameType(LONG MenuCombo, LONG type)
{
	LONG	i;
	FILE *fp;
	SHORT	MenuId, ButtonId;
	// general cleanup
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
#if defined(_JUNEDEMO)
	if (type == 3)
	{
		ShowAdScreen(0);
		fRedrawAll = TRUE;
		update_screen();
		return;
	}
#endif
#if defined(_NETJUNEDEMO) || defined(_JUNEDEMO)
	if (type == 4)
	{
		ShowAdScreen(3);
		fRedrawAll = TRUE;
		update_screen();
		return;
	}
#endif
	
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
	
	
#if defined(_MULTIPLAY_ONLY)
// disable single player and adventures only
	if (type == 2 || type == 3)
	{
		SHORT   bitm;
		LONG    now;
		
		//GEH Splash Screen to tell them to buy the full product
 		bitm = GetResourceStd ("intro\\nosingle.pcx", FALSE);
		if (bitm != fERROR)
		{
			// put up the art
			DrawBitmap (0, 0, bitm, 0, 0, 999, 999);
			update_screen();
			SetPurge(bitm);
			
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
						break;
					}
					fAnyKeyChanged = FALSE;
					clear_key_status(0);
					break;
				}
			}
		}
		
		return;
	}
#endif

	HidePanel(D_BUILD_ARMY);
	HidePanel(D_BUILD_ADV);
	HidePanel(D_ADVENTURE_PREP);
	
	switch(type)
	{
		case 0:
			DescribeGame();
			break;
			
		case 1:
		case 2:
			tut_or_game = type-1;
			OpenGameComplexity(0, 0);
			break;

		case 10:
		case 11:
		case 12:
			//GEH 8/5/97
			// reload virgin copies of the .ava and .inv files
			//StatReadAll();
			//WriteRealmStartData();
			//GEH 8/25/97
			
			if (tut_or_game == 0)
			{
				if (type == 10)
	  				fp = FileOpen("tutorial.sav", "rb");
				if (type == 11)
  					fp = FileOpen("tutoradv.sav", "rb");
				if (type == 12)
  					fp = FileOpen("tutorexp.sav", "rb");

  				if (fp == NULL)
  					break;

				master_game_type = GAME_INTRO;
   				LoadGameFiles(fp);
	 			FileClose(fp);

				HideMenu(D_GAME_COMPLEXITY);
				RunMenus();
				GAMEToggleMainMenu(FALSE,0);
				RunMenus();
			
				// clear any old menu data
				ResetMenus();
				ResetPanels();
				ResetRequests();
				
				// remove all extra regions and start over
				del_all_regions();
				AddGameKeys();
			
				char_selected = TRUE;
				max_actions = 3;
				for (i=0; i<LAND_REALM_COUNT; i++)
					action_turn[i] = 1;
				oldAction = 0;
				global_action_turn = 1;
				fRoundOver = FALSE;
				fFinalRoll = FALSE;
				fUpdateDataArea = FALSE;
				fDoActionIcon = FALSE;
				fHeldAction = FALSE;
				fLTAction = BEFORE_LTACTION;
				CurrentRealm = REALM::REALM_OUT_OF_RANGE;
				new_treasury = 0;
				map_x = req_x = 0;
				map_y = req_y = 0;
				desired_scale = map_scale = max_scale = DMAX_SCALE;
				fPractice = FALSE;

				//fFinalRoll = FALSE;		// done in InitDomainTurn
				//fDoActionIcon = FALSE;
				//sMenusUp = 0;
				//fFadedOut = -1;
				//fRedrawAll = TRUE;
				//fDomainTurn = TRUE;
				//fUpdateDataArea = TRUE;
				//iSelectedUnit = -1;
				//iProvInfo = 0;
				//iUnitInfo = 0;
				//iPlaceInfo = 0;
				//date = 0;

				fTutorialSelected = TRUE;			// new flag
				if (type == 10)
					date = 0;
				if (type == 11)
					date = 3;
				if (type == 12)
					date = 5;

				TutorialActionNumber = 1;
				TutorialScreenNumber = 0;
				if (type == 10)
					fTutorialFirstBattle = TRUE;	// only in BASIC tutorial

				for (i=0; i<MAX_SITES; i++)		// turn off all sites
					advsite[i].available = AVAILABLE;
				advsite[0].name = "tutor";			// turn on site "tutor"
				advsite[0].realm = REALM::DHOESONE;
				advsite[0].iPlaces = 64;
//				SetupAdvsitePlaces();
				SetAdvSite(0);
				advsite[0].available = SELECTABLE;
				iOldSite = 0;

				// Fix the regents icons & attach the correct icon to the unit.
				TransferRegentIconsToUnitsArray();
				
		    	// release a running scene, calls InitDomainTurn
				if (fDomainTurn == FALSE)
				{
					master_game_type = GAME_NORMAL;
					dturn_mode = START_NEW_OR_LOAD;
			    	SCENE_MGR::mfReleaseSceneToMap(0,0);
				}
				else
				{
					// exit and release current domain turn
					if (iLgMap != fERROR)		// if domain turn IS active
						InitDomainTurn(0);

			  		// start a new domain turn UI
					fDomainTurn = FALSE;
					master_game_type = GAME_NORMAL;
					dturn_mode = START_NEW_OR_LOAD;
					InitDomainTurn(0);
				}

				dturn_mode = REPORT_DONE_MODE;
				SetRedrawMainMapLevel();

				fControlMode = (type-10) | 0x100;	// selected tutorial
				break;
			}
			else
			{
				ResetToNonTutorialAdventures();
				master_game_type = GAME_NORMAL;
				HideMenu(D_GAME_COMPLEXITY);
				RunMenus();
				GAMEToggleMainMenu(FALSE,0);
				RunMenus();
			
				// clear any old menu data
				ResetMenus();
				ResetPanels();
				ResetRequests();
				
				// remove all extra regions and start over
				del_all_regions();
				AddGameKeys();
			
				fPractice = FALSE;
				fTutorialSelected = FALSE;
				fTutorialFirstBattle = FALSE;
				NewGameConfirmed(0, lMultiFlagLocal);

		    	// release a running scene, calls InitDomainTurn
				if (fDomainTurn == FALSE)
				{
					master_game_type = GAME_NORMAL;
					dturn_mode = START_NEW_OR_LOAD;
			    	SCENE_MGR::mfReleaseSceneToMap(0,0);
				}
				else
				{
					// exit and release current domain turn
					if (iLgMap != fERROR)		// if domain turn IS active
						InitDomainTurn(0);

			  		// start a new domain turn UI
					fDomainTurn = FALSE;
					master_game_type = GAME_NORMAL;
					dturn_mode = START_NEW_OR_LOAD;
					InitDomainTurn(0);
				}

				dturn_mode = CHARSEL_MODE;
				SetButtonFlag(D_GAMEBUTTON, 0, D_INVISIBLE);
				SetRedrawMainMapLevel();
				fControlMode = (type-10) | 0x100;
				fFadedOut = -1;
				break;
			}
		
		case 3:
			if (!fDomainTurn)
			{
		    	// release a running scene
		    	SCENE_MGR::mfReleaseSceneToMap(0,0);
		 	}
		 	
			//GEH 8/5/97
			// reload virgin copies of the .ava and .inv files
			//StatReadAll();
			//WriteRealmStartData();
			//GEH 8/25/97
			
			// reset game files
			//GEH 8/25/97
			#ifdef _WINDOWS
			if ( IsMultiPlayer() )
				fp = FileOpen("mpstart.dat", "rb");
			else
			#endif
				fp = FileOpen("rlmstart.dat", "rb");
			LoadGameFiles(fp);
			FileClose(fp);

			ResetToNonTutorialAdventures();
			master_game_type = GAME_ADVENTURE;
			fSaveInventories = FALSE;	// begin with a load
			HideMenu(D_SELECT_GAME);
			RunMenus();
			GAMEToggleMainMenu(FALSE,0);
			RunMenus();
			
			// clear any old menu data
			ResetMenus();
			ResetPanels();
			ResetRequests();
			
			// remove all extra regions and start over
			del_all_regions();
			AddGameKeys();
			
			fTutorialSelected = FALSE;
			RunAdvOnly();
			
			DescribeAdventure();
			
			break;
			
		case 4:
			if (!fDomainTurn)
			{
				// release a running scene
				SCENE_MGR::mfReleaseSceneToMap(0,0);
			}
			
			//GEH 8/5/97
			// reload virgin copies of the .ava and .inv files
			//StatReadAll();
			//WriteRealmStartData();
			//GEH 8/25/97
			
			// reset game files
			//GEH 8/25/97
			#ifdef _WINDOWS
			if ( IsMultiPlayer() )
				fp = FileOpen("mpstart.dat", "rb");
			else
			#endif
	    		fp = FileOpen("rlmstart.dat", "rb");
			LoadGameFiles(fp);
	    	FileClose(fp);
	    	
			ResetToNonTutorialAdventures();
			master_game_type = GAME_BATTLE;
			HideMenu(D_SELECT_GAME);
			RunMenus();
			GAMEToggleMainMenu(FALSE,0);
			RunMenus();
			
			// clear any old menu data
			ResetMenus();
			ResetPanels();
			ResetRequests();
			
			// remove all extra regions and start over
			del_all_regions();
			AddGameKeys();
			
			fTutorialSelected = FALSE;
			RunBattlesOnly();
			
			RunMenus();
			DescribeBattle();
			fUpdatePanels = TRUE;
			RunPanels();
			RunMenus();
			
			break;
			
		case 5:
			DescribeHistory();
			break;
			
		default:
			break;	
	}
}
