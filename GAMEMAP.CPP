/* ========================================================================
   Copyright (c) 1990,1996		Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: GAMEMAP.cpp  -
   Author:	Alan Clark

   ========================================================================
   Contains the following general functions:

   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#ifdef _WINDOWS
#include <windows.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <direct.h>

#include "SYSTEM.H"

#include "ACTIONS.HXX"
#include "ACTNMENU.HXX"
#include "CHARSEL.HXX"
#include "REGENTS.HXX"
#include "SOUND.HXX"
#include "SNDVOX.HXX"
#include "PROVTAX.H"
#include "SCNMGR.HXX"
#include "GAMEKEY.HXX"
#include "GAMEMAP.HXX"
#include "GAME.H"
#include "LIGHT.H"
#include "MENU.H"
#include "PANEL.H"
#include "REQUEST.H"
#include "FLICPLAY.HXX"
#include "REPORT.HXX"
#include "STRMGR.H"
#include "strenum.h"
#include "MULTIMAP.HXX"
#include "PLAYSTAT.HXX"
#include "MAPAI.HXX"
#include "LOADSAVE.HXX"
#include "VECTOR.HXX"
#include "MULTIUI.HXX"
#include "BATTLEUI.HXX"
#include "BATTLE.HXX"
#include "MAIN.HXX"
#include "REALM.HXX"
#include "TUTOR.HXX"
#include "PLACES.HXX"
#include "HANDLE.HXX"

#ifdef _WINDOWS
#include "WINSYS\MULPLAY.HXX"
#include "WINSYS\MONO_C.H"
#endif

extern BOOL fLogComment;

/* -----------------------------------------------------------------------
	Defines and Compile Flags
	------------------------------------------------------------------------ */
//#define  LOGCOMMENTSLEEP	Sleep(1);
#define  LOGCOMMENTSLEEP	{;}

#define MAX_STACK_DEPTH					15

#define PARCHMENT_SOUND			0

#define TWOMAPS_RATIO			((fLgMap==LARGE)?(4):(2))
#define MAP_WIDTH					1920
#define MAP_HEIGHT				2234

#define LARGE						0
#define SMALL						1
#define ENHANCEDRES				0
#define MESSAGE_DELAY			80
#define MAX_MAP_XY_SPEED		61
#define NORM_SCALE				(1<<8)
#define MAX_SCALE					max_scale
#define DMAX_SCALE				((FULL_SCALE*1920)/448);
#define AMAX_SCALE				((FULL_SCALE*1920)/496);
#define MIN_SCALE					(1<<7)

#define MAP_ICON_X_ADJ			0
#define MAP_ICON_Y_ADJ			0
#define UNIT_ICON_X_ADJ			MAP_ICON_X_ADJ+10
#define UNIT_ICON_Y_ADJ			MAP_ICON_Y_ADJ-25

#define BLACK						1
#define WHITE						31
#define DKBLUE						60
#define BLUE						64
#define LTBLUE						79
#define DKPURPLE					90
#define LTPURPLE					103
#define DKRED						112
#define MDRED						120
#define LTRED						127
#define DKBROWN					128
#define MDBROWN					136
#define LTBROWN					140
#define ORANGE						142
#define MDYELLOW					168
#define LTYELLOW					175
#define DKGREEN					184
#define MDGREEN					188
#define LTGREEN					192
#define BRIGHTGREEN				199
#define MDTAN						213
#define LTTAN						223

#undef BLUE_GRAD
#define BLUE_GRAD					0
#undef RED_GRAD
#define RED_GRAD					3
#undef BLACK_GRAD
#define BLACK_GRAD				5
#undef PURPLE_GRAD
#define PURPLE_GRAD				9
#undef GREEN_GRAD
#define GREEN_GRAD				18

#define MIN_MODE					0
#define ACTN_MODE					1
#define MAINT_MODE				2
#define TAXLVL_MODE				3
#define OCCUPATION_MODE			4
#define PAYRL_MODE				5
#define SELRLM_MODE				6
#define ADVPARTY_MODE			7
#define INIT_REPORT_MODE		8
#define REPORT_MODE				9
#define REPORT_DONE_MODE		10
#define REPEAT_REPORT_DONE_MODE	11
#define MAX_MODE					99

#define DATA_AREA_ACTN			144
#define DATA_AREA_MIN			192
#define DATA_AREA_MAX			352

#define regFIRST_REGION_STATE	1000
#define regDOMAIN_TURN_MODE	1001
#define regACTION_MODE			1002
#define regMINIMIZED				1003
#define regMAXIMIZED				1004
#define regTAX_ROLLS				1005
#define regSWITCH_TO_ACTIONS	1006
#define regADJUST_TAXES			1007
#define regSHOW_HOLDINGS		1008


// MOVED TO INCLUDE FILE !!!! gamemap.hxx
//#define BEFORE_LTACTION			0
//#define DURING_LTACTION			1
//#define AFTER_LTACTION			2

#define WAITINGFOR				1
#define PLAYERSTURN				2
#define REALM_IS					3
#define UNITS_MOVE				4
#define FIGHTING					5
#define HASWON						6

#define ITT_2MIN		(2*60*18)
#define ITT_4MIN		(4*60*18)
#define ITT_6MIN		(6*60*18)
#define ITT_NONE		(72*60*60*18)
#define IX_TT_NONE	3

/* ------------------------------------------------------------------------
	Macros
	------------------------------------------------------------------------ */
#define ACTIVATE_REGION_(a,b)		activate_region((a),(region_state[a-regFIRST_REGION_STATE]=(b)))
#define RESTORE_REGION_STATE(a) activate_region((a),region_state[a-regFIRST_REGION_STATE])

/* ------------------------------------------------------------------------
	Prototypes
	------------------------------------------------------------------------ */
extern void SysHideCursor (void);
extern void SysShowCursor (void);
extern void SysForceCursor (void);
extern SHORT LastRegionX (void);
extern SHORT LastRegionY (void);
extern LONG CountRegions (void);
extern void dashedline(LONG x0, LONG y0, LONG x1, LONG y1, LONG color1 ,LONG color2);
extern BOOL RegentDeathTest (void);

extern void RandomLogComment ( char * szString );
extern void LogFlush ( void );

#if defined(_JUNEDEMO)
void ShowAdScreen(SHORT);
#endif

void NPCBattleResults(BOOL);
SHORT GetLastUnit( SHORT FirstUnit );
BOOL CheckWithdrawPath (SHORT UnitIndex);
void DoSlideShow(LONG iWhich);
void PlayFinale(LONG,LONG);
void SelectAdvParty(void);
void DisplayAdvParty(void);
void DisplayRealmSelection(void);
void PlayIntro(void);
void PlayBattleFlc(void);
void PlayAdvFlc(void);
void InitAutoDemo(void);
void CheckAutoDemo(void);
extern LONG FlcDelay;
LONG CheckUnitArrival(LONG,BOOL);
void SelectSubUnit(LONG,LONG);
void MoveSubUnitUpList(LONG,LONG);
void WithdrawLosingUnit(BOOL);
void InitBattleCard( SHORT UnitIndex );
void SetStatusRealm(REALM::REALM_TYPE realm);
void ShowStatus(LONG tID, LONG hPossibleStats);
void DoNPCAdmin(void);
void DoAction(LONG iAction, LONG);
void IncActionTurn(void);
BIRTHRT_SND CalcVoxSndObj(VOX_TYPE Voice, VOX_BLOCK Block);
void DefeatScreen(PFVLL NewPaintProc, PFVLL NewDoneProc);
LONG UnitsHere (PROVINCE prov, REALM::REALM_TYPE realm);
void SelectAdvSite(void);
void AdjustHoldingLevel(PROVINCE prov, BOOL fSend);
BOOL BattlePrep(void);
SHORT QueryCommandBattle(BOOL);
void ShowSyncWaitWindow( LONG sync, LONG);
void DisplayNPCResults(void);
void PaintNPCResults(LONG, LONG);
void StartMapIntro (LONG, LONG);
void PlayGorgon(LONG arg,LONG);
void CopyReactionTable(void);
void DeleteDomainRegions(void);
void AddDomainRegions(void);
void SwitchToActionMode (LONG, LONG);
BOOL npc_move(PROVINCE);
void Clear_fProv(void);
BOOL BorderPermitted (REALM::REALM_TYPE source_realm, REALM::REALM_TYPE dest_realm, PROVINCE dest_prov);
void ResetToNonTutorialAdventures(void);		// in practice.cpp
void CheckAndFixPlaces (PROVINCE prov);
void AbortDomainAction (void);
void DoneOthersAction (LONG, LONG);
void RollOthersAction (LONG, LONG);
void UpdateHoldingFlag (void);

/* ------------------------------------------------------------------------
	Global Variables
	------------------------------------------------------------------------ */
SHORT	sProvPoints = 0;
SHORT	sVassalPoints = 0;
SHORT	sFullAllyPoints = 0;
SHORT	sPermissivePoints = 0;
SHORT	sOtherPoints = 0;

BOOL				fDomainTurn = FALSE;
BOOL				fVeryFirstTime = TRUE;
BOOL				fLoadedNewGame = FALSE;
BOOL				fWonGame = FALSE;
SHORT				gWinningPoints = 300;
SHORT				iProvSelect = -1;
static SHORT	NavigateSoundTag = fERROR;
BOOL				fRedrawAll = TRUE;
BOOL				fMapAltered = TRUE;
BOOL				fRedrawProcessPanel = TRUE;
SHORT				iLgMap = fERROR;
SHORT				iProvMap = fERROR;
SHORT				iDTurnBk = fERROR;
static SHORT	iDTurnIc = fERROR;
static SHORT	iDTurnI2 = fERROR;
static SHORT	iDShldIc = fERROR;
static SHORT	iDActnIc = fERROR;
static SHORT	iDActnI2 = fERROR;
static SHORT	iDDActIc = fERROR;
static SHORT	iDeprsIc = fERROR;
SHORT				iMCtrlIc = fERROR, iMCtr2Ic = fERROR;
SHORT				iDCtrlIc = fERROR, iDCtr2Ic = fERROR;
static SHORT	iDCtstIc = fERROR;
static SHORT	iDOcupIc = fERROR;
SHORT				iDRollIc = fERROR;
SHORT				iDRollFIc = fERROR;
SHORT				iDMapBx = fERROR;
static SHORT	iDLawL1Ic = fERROR, iDLawL2Ic = fERROR, iDLawL3Ic = fERROR;
static SHORT	iMap48Ic = fERROR, iMap48I2 = fERROR;
SHORT				iCheck[2] = {fERROR, fERROR};
static BOOL		fRedrawRoads;
static LONG		iplace_coord = 0;
LONG				desired_scale = 0;
LONG				max_scale = DMAX_SCALE;
LONG				map_scale = MAX_SCALE;
LONG				map_x = 1100;
LONG				map_y = 1664;
LONG				req_x = 0;
LONG				req_y = 0;
SHORT				iUnitInfo = 0;
SHORT				iPlaceInfo = 0;
SHORT				iProvInfo = 0;
LONG				fLgMap = LARGE;
LONG				l_map = DATA_AREA_MIN;
LONG				req_l_map = DATA_AREA_MIN;
LONG				dturn_mode = START_NEW_OR_LOAD;
LONG				report_done_dturn_mode = 0;
BOOL				fStartOfActionMode = FALSE;
BOOL				fFreezeDomainUI = FALSE;
BOOL				fInfoOnlyDomainUI = FALSE;
LONG				iHotSeatMultiplayerIndex = 1;

extern BOOL AdvisorReportOn;

DEFINE_VECTOR_DATA(BOOL,dtrn_ic_active) = {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE};
DEFINE_VECTOR_CLASS(BOOL,dtrn_ic_active);

DEFINE_VECTOR_DATA(BOOL,dact_ic_active) ={0,	TRUE,TRUE,		// Adventure, Agitate
												TRUE,TRUE,		// Build, Contest
												TRUE,TRUE,		// CreateHold, Declare War
												TRUE,TRUE,		// Diplomacy, Espionage
												TRUE,TRUE,		// LeyLine, Fortify
												TRUE,TRUE,		// Hold Action, Investiture
												TRUE,TRUE,		// Lieutenant, Lt. Action
												TRUE,TRUE,		// Muster, Realm Spell
												TRUE,TRUE};		// Rule, Trade Route
DEFINE_VECTOR_CLASS(BOOL, dact_ic_active);

LONG				max_actions = 3;
LONG				action_turn[LAND_REALM_COUNT] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
SHORT				fRoundOver = FALSE;
SHORT				fFinalRoll = FALSE;
BOOL				fAfterTaxRolls = FALSE;
BOOL				fUpdateDataArea = FALSE;
BOOL				fDoActionIcon = FALSE;
static SHORT	fSelectDomains = FALSE;
static SHORT	fDepressIcon = FALSE;
static SHORT	xDepressIcon = 0;
static SHORT	yDepressIcon = 0;
int				reqSound = FALSE;
static LONG		mode_save = MAINT_MODE;
DECL_VECTOR_DATA(SHORT,region_state,10);
SHORT				fFadedOut = FALSE;
SHORT	fCheckUnitArrival = FALSE;
static SHORT	sUnitsLostWinner;	// units lost during NPC battles
static SHORT	sUnitsLostLoser;
static BOOL		fHomeRetreatPath;		// units have a place to run
static BOOL		fAwayRetreatPath;

SHORT	fJoinDialog = FALSE;
static SHORT	xJoinDialog = 0;
static SHORT	yJoinDialog = 0;
SHORT	iJoinDialog_to = 0;
static BOOL		fWasABattle = FALSE;
BOOL				fHeldAction = FALSE;
SHORT				fLTAction = BEFORE_LTACTION;

REALM::REALM_TYPE	HomeRealm = REALM::ROESONE;
REALM::REALM_TYPE	CurrentRealm = REALM::REALM_OUT_OF_RANGE;
SHORT				ActiveRegent;
SHORT				CurrentScore = 0;
LONG				date = 0;
LONG				prov_castle, prov_law, prov_temple, prov_guild, prov_source, prov_units;
LONG				prov_tax, prov_trade, prov_reg;
LONG				our_law_claims, their_law_claims, fOurLawClaims, fTheirLawClaims;
LONG				domain_power;
LONG				realm_reg;
LONG				realm_tax;
LONG				pillage;
//static LONG		realm_source;
LONG				new_treasury = 0;
LONG				trade_routes = 0;
static LONG		law_holding_claims = 0;
LONG				payroll = 0;
LONG				payregency = 0;
BOOL				fPayrollReduced = FALSE;
LONG				domain_maint = 0;
SHORT				cProvHolds = 0;
SHORT				cProvinces = 0;
SHORT				cCastles = 0;
SHORT				cOccupiedProv = 0;
//static LONG		domain_turn_counter = 0;
static SHORT	fDontAdvance = FALSE;
SHORT				fUnitsAllOn = 1;
SHORT				fHoldingsAllOn = 1;
SHORT				iSelectedUnit = -1;
LONG				cntUnits = 302;		// guess, need not be exact
LONG				cntHolds = 900;		// guess, need not be exact
LONG				iGorgonPlot = 0;
LONG				iGorgonAlly = 0;
LONG				fControlMode = 0;		// basic game
BOOL				fBattlePrep = FALSE;

#ifdef _DEMO
	SHORT			fTutorialMessage=TRUE;
#endif
DEFINE_MATRIX_DATA_S(UBYTE,Tribute,REALM::REALM_COUNT,REALM::REALM_COUNT);
DEFINE_MATRIX_CLASS_S(UBYTE,Tribute,REALM::REALM_COUNT,REALM::REALM_COUNT);

DEFINE_VECTOR_DATA_S(UBYTE,RealmSceneFlags,REALM::REALM_COUNT);
DEFINE_VECTOR_CLASS(UBYTE,RealmSceneFlags);

DEFINE_VECTOR_DATA_S(SHORT,province_tax,PROVINCE_COUNT);
DEFINE_VECTOR_CLASS(SHORT,province_tax);

DEFINE_VECTOR_DATA(UBYTE,coRlmMagic) = {LTBLUE,MDTAN-6,LTYELLOW,MDRED,WHITE};
DEFINE_VECTOR_CLASS(UBYTE,coRlmMagic);

DEFINE_VECTOR_DATA(UBYTE,ActionColor) = {0,DKGREEN,DKBLUE,DKRED,DKPURPLE};
DEFINE_VECTOR_CLASS(UBYTE, ActionColor);

//DEFINE_VECTOR_DATA(CSTRPTR,szOrdinalCap) = {"Zeroth","First","Second","Third","Fourth","Fifth","Sixth","Seventh","Eighth","Ninth"};
//DEFINE_VECTOR_CLASS(CSTRPTR, szOrdinalCap);

//DEFINE_VECTOR_DATA(CSTRPTR,szOrdinalNum) = {"0th","1st","2nd","3rd","4th","5th","6th","7th","8th","9th"};
//DEFINE_VECTOR_CLASS(CSTRPTR,szOrdinalNum);

//DEFINE_VECTOR_DATA(CSTRPTR,szRace) = {"Dwarf","Elf","Half-Elf","Halfling","Anuirean","Brecht","Khinasi","Rjurik","Vos","Goblin","Gnoll","Awnshegh"};
//DEFINE_VECTOR_CLASS(CSTRPTR, szRace);

//DEFINE_VECTOR_DATA(CSTRPTR,szClass) = {"","Fighter","Paladin","Ranger","Priest","Wizard","Thief","Bard"};
//DEFINE_VECTOR_CLASS(CSTRPTR, szClass);

//DEFINE_VECTOR_DATA (CSTRPTR,szBL_grade) = {"Tainted","Minor","Major","Great","True"};
//DEFINE_VECTOR_CLASS(CSTRPTR, szBL_grade);
//
//DEFINE_VECTOR_DATA (CSTRPTR,szBL_deriv) = {"Anduiras","Reynir","Brenna","Basaia","Masela","Vorynn","Azrai"};
//DEFINE_VECTOR_CLASS(CSTRPTR, szBL_deriv);
//
//DEFINE_VECTOR_DATA (CSTRPTR,szMode) = {"","","Maintenance","Taxation","Occupation","Payroll"};
//DEFINE_VECTOR_CLASS(CSTRPTR,szMode);
//
//DEFINE_VECTOR_DATA (CSTRPTR,TaxLevel) = {"Light","Moderate","Severe"};
//DEFINE_VECTOR_CLASS(CSTRPTR, TaxLevel);
//
//DEFINE_VECTOR_DATA (CSTRPTR,Loyalty) = {"Rebel","Poor","Average","High"};
//DEFINE_VECTOR_CLASS(CSTRPTR, Loyalty);
//
//DEFINE_VECTOR_DATA (CSTRPTR,Terrain) = {"","Forest","Plains","Mountains","Mountain Range","Swamp","Hills","River","Bridge","Road"};
//DEFINE_VECTOR_CLASS(CSTRPTR, Terrain);


DEFINE_VECTOR_DATA (GAME_STRING,gsOrdinalCap) = {STR_NTH_WORD_0,STR_NTH_WORD_1,STR_NTH_WORD_2,STR_NTH_WORD_3,STR_NTH_WORD_4,STR_NTH_WORD_5,STR_NTH_WORD_6,STR_NTH_WORD_7,STR_NTH_WORD_8,STR_NTH_WORD_9};
DEFINE_VECTOR_CLASS(GAME_STRING,gsOrdinalCap);

DEFINE_VECTOR_DATA (GAME_STRING,gsOrdinalNum) = {STR_NTH_NUM_0,STR_NTH_NUM_1,STR_NTH_NUM_2,STR_NTH_NUM_3,STR_NTH_NUM_4,STR_NTH_NUM_5,STR_NTH_NUM_6,STR_NTH_NUM_7,STR_NTH_NUM_8,STR_NTH_NUM_9};
DEFINE_VECTOR_CLASS(GAME_STRING,gsOrdinalNum);

DEFINE_VECTOR_DATA (GAME_STRING,gsRace) = {STR_RACE_DWARF,STR_RACE_ELF,STR_RACE_HALF_ELF,STR_RACE_HALFLING,STR_RACE_ANUIREAN,STR_RACE_BRECHT,STR_RACE_KHINASI,STR_RACE_RJURIK,STR_RACE_VOS,STR_RACE_GOBLIN,STR_RACE_GNOLLS,STR_RACE_AWN};
DEFINE_VECTOR_CLASS(GAME_STRING,gsRace);

DEFINE_VECTOR_DATA (GAME_STRING,gsClass) = {STR_NULL,STR_CLASS_FIGHTER,STR_CLASS_PALADIN,STR_CLASS_RANGER,STR_CLASS_CLERIC,STR_CLASS_WIZARD,STR_CLASS_THIEF,STR_CLASS_BARD,};
DEFINE_VECTOR_CLASS(GAME_STRING,gsClass);

DEFINE_VECTOR_DATA (GAME_STRING,gsBL_grade)= {STR_BA_STRENGTH_TAINTED,STR_BA_STRENGTH_MINOR,STR_BA_STRENGTH_MAJOR,STR_BA_STRENGTH_GREAT,STR_BA_STRENGTH_TRUE};
DEFINE_VECTOR_CLASS(GAME_STRING,gsBL_grade);

DEFINE_VECTOR_DATA (GAME_STRING,gsBL_deriv)= {STR_BA_DERIV_AND,STR_BA_DERIV_REY,STR_BA_DERIV_BRE,STR_BA_DERIV_BAS,STR_BA_DERIV_MAS,STR_BA_DERIV_VOR,STR_BA_DERIV_AZR};
DEFINE_VECTOR_CLASS(GAME_STRING,gsBL_deriv);

DEFINE_VECTOR_DATA (GAME_STRING,gsMode) 	= {STR_NULL,STR_NULL,STR_RLM_MODE_MAIN,STR_RLM_MODE_TAX,STR_RLM_MODE_OCC,STR_RLM_MODE_PAY};
DEFINE_VECTOR_CLASS(GAME_STRING,gsMode);

DEFINE_VECTOR_DATA (GAME_STRING,gsTaxLevel) 	= {STR_SEVERITY_LIGHT,STR_SEVERITY_MOD,STR_SEVERITY_SEV};
DEFINE_VECTOR_CLASS(GAME_STRING,gsTaxLevel);

DEFINE_VECTOR_DATA (GAME_STRING,gsLoyalty) 	= {STR_LOYAL_REBEL,STR_LOYAL_POOR,STR_LOYAL_AVG,STR_LOYAL_HIGH};
DEFINE_VECTOR_CLASS(GAME_STRING,gsLoyalty);

DEFINE_VECTOR_DATA (GAME_STRING,gsTerrain) 	= {STR_NULL,STR_LAND_TYPE_FOR,STR_LAND_TYPE_PLA,STR_LAND_TYPE_MNT,STR_LAND_TYPE_MNTR,STR_LAND_TYPE_SWMP,STR_LAND_TYPE_HILL,STR_LAND_TYPE_RIV,STR_LAND_TYPE_BRDG,STR_LAND_TYPE_ROAD};
DEFINE_VECTOR_CLASS(GAME_STRING,gsTerrain);

DEFINE_VECTOR_DATA (GAME_STRING,gsCourt) = {STR_COURT_LVL_NONE,STR_COURT_LVL_MINIMAL,STR_COURT_LVL_QUAINT,STR_COURT_LVL_AVERAGE,STR_COURT_LVL_OPULENT};

DEFINE_VECTOR_CLASS(GAME_STRING,gsCourt);


//CSTRPTR szRealmStatus[]={"","AT WAR","NEUTRAL","PERMISSIVE ALLIANCE","FULL ALLIANCE","VASSAL"};

GAME_STRING gsRealmStatus[]={STR_NULL,STR_REALM_STAT_AT_WAR,STR_REALM_STAT_NEUTRAL,STR_REALM_STAT_PERMISSIVE_ALLIANCE,STR_REALM_STAT_FULL_ALLIANCE,STR_REALM_STAT_VASSAL};

DEFINE_VECTOR_DATA(SHORT,LoyaltyAdj) = {+1,0,-1};
DEFINE_VECTOR_CLASS(SHORT, LoyaltyAdj);

DEFINE_VECTOR_DATA(SHORT,NoLawLoyaltyAdj) = {+1,-1,-2};
DEFINE_VECTOR_CLASS(SHORT, NoLawLoyaltyAdj);


DEFINE_VECTOR_DATA(UBYTE,LoyaltyColor) = {DKRED,ORANGE,DKGREEN,DKBLUE};
DEFINE_VECTOR_CLASS(UBYTE, LoyaltyColor);

DEFINE_VECTOR_DATA(UBYTE,MaintCost)={
//							  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49
							  0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,10,10,10,10,10,10,10,10,10,
//							 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99
							 10,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,
//							 00
							 30};
DEFINE_VECTOR_CLASS(UBYTE, MaintCost);


DEFINE_VECTOR_DATA(SHORT,CourtCost) = {0,1,3,6,9};
DEFINE_VECTOR_CLASS(SHORT, CourtCost);


DEFINE_VECTOR_DATA(SHORT,TerrainCost)={0,	2,	 1,	4,		255,	3,	 2,	4,	0,	 0 };
DEFINE_VECTOR_CLASS(SHORT, TerrainCost);

//	places in provs	 1	2	3	4	5	6	7	8	9  10
DEFINE_VECTOR_DATA(SHORT,place_x) = {-10,-30, 30,-35, 35,  0,-20, 20,-10, 10};
DEFINE_VECTOR_CLASS(SHORT, place_x);

DEFINE_VECTOR_DATA(SHORT,place_y) = {-25,-15,-15, 12, 12, 27, 24, 24, 12, 12};
DEFINE_VECTOR_CLASS(SHORT, place_y);

SHORT StatusColor[]={0,MDRED,DKBROWN,MDGREEN,BLUE,DKPURPLE};
SHORT DkStatusColor[]={0,104+4,DKBROWN,176+4,56+4,80+4};

DEFINE_VECTOR_DATA(BOOL,show_place_icon) = {
TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,		// units 0-9
TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,		// units 10-19
TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,		// units 20-29
TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,		// units 30-39
TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,		// units 40-49
TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,		// units 50-59
TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,		// units 60-69
TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,		// units 70-79
TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,		// units 80-89
TRUE,TRUE,TRUE,													// units 90-92
TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,								// village, seaport1&2, port, wiztower, ruins
FALSE,FALSE,FALSE,FALSE,										// law
FALSE,FALSE,FALSE,FALSE,										// temple
FALSE,FALSE,FALSE,FALSE,										// guild
FALSE,FALSE,FALSE,FALSE,										// source
TRUE,TRUE,TRUE,TRUE,												// castle
TRUE,TRUE,TRUE														// specials
};
DEFINE_VECTOR_CLASS(BOOL, show_place_icon);

/* This is the icons names.  Use the MAP_ICON enum as the index */
/* into this table.  Remember to prepend the ICON_DIR and append ICON_EXT */
DEFINE_VECTOR_DATA_S(CSTRPTR,GameIcons,MAP_ICON_COUNT) = {
/*0 NO_MAP_ICON		  */		"",
/*1 A_INF_UNIT_ICON	*/		"A_INF",
/*2 A_EIN_UNIT_ICON	*/		"A_EIN",
/*3 A_BOW_UNIT_ICON	*/		"A_BOW",
/*4 A_IRR_UNIT_ICON	*/		"A_IRR",
/*5 A_PIK_UNIT_ICON	*/		"A_PIK",
/*6 A_LVY_UNIT_ICON	*/		"M_INF",
/*7 A_SCT_UNIT_ICON	*/		"M_OFF",
/*8 A_KNT_UNIT_ICON	*/		"A_KNT",
/*9 A_CAV_UNIT_ICON	*/		"A_CAV",
/*10 M_INF_UNIT_ICON	*/		"M_INF",
/*1 M_BOW_UNIT_ICON	*/		"A_BOW",
/*2 M_IRR_UNIT_ICON	*/		"M_PIK",
/*3 M_PIK_UNIT_ICON	*/		"M_PIK",
/*4 M_CAV_UNIT_ICON	*/		"A_CAV",
/*5 E_INF_UNIT_ICON	*/		"E_INF",
/*6 E_BOW_UNIT_ICON	*/		"E_BOW",
/*7 E_CAV_UNIT_ICON	*/		"A_KNT",
/*8 D_INF_UNIT_ICON	*/		"D_INF",
/*9 D_BOW_UNIT_ICON	*/		"D_BOW",
/*20 G_INF_UNIT_ICON	*/		"G_INF",
/*1 G_BOW_UNIT_ICON	*/		"G_BOW",
/*2 G_CAV_UNIT_ICON	*/		"G_CAV",
/*3 N_INF_UNIT_ICON	*/		"N_IRR",
/*4 N_IRR_UNIT_ICON	*/		"N_IRR",
/*5 C_HPY_UNIT_ICON	*/		"C_HPY",
/*6 C_SKL_UNIT_ICON	*/		"C_SKL",
/*7 C_SPD_UNIT_ICON	*/		"C_SPD",
/*8 C_HEL_UNIT_ICON	*/		"C_HEL",
/*9 C_WYV_UNIT_ICON	*/		"C_WYV",
/*30 REG_LORDM1_UNIT_ICON */	 "FITRM1",
/*1 REG_LORDM2_UNIT_ICON */	 "FITRM2",
/*2 REG_LORDM3_UNIT_ICON */	 "FITRM3",
/*3 REG_LORDM4_UNIT_ICON */	 "FITRM4",
/*4 REG_LORDM5_UNIT_ICON */	 "FITRM5",
/*5 REG_LORDM6_UNIT_ICON */	 "FITRM6",
/*6 REG_LORDM7_UNIT_ICON */	 "FITRM7",
/*7 REG_WIZDM1_UNIT_ICON */	 "WZRDM1",
/*8 REG_WIZDM2_UNIT_ICON */	 "WZRDM2",
/*9 REG_WIZDM3_UNIT_ICON */	 "WZRDM3",
/*40 REG_WIZDM4_UNIT_ICON */	 "WZRDM4",
/*1 REG_GILDM1_UNIT_ICON */	 "GLDRM1",
/*2 REG_GILDM2_UNIT_ICON */	 "GLDRM2",
/*3 REG_PRSTM1_UNIT_ICON */	 "PRESM1",
/*4 REG_PRSTM2_UNIT_ICON */	 "PRESM2",
/*5 REG_LORDF1_UNIT_ICON */	 "FITRF1",
/*6 REG_LORDF2_UNIT_ICON */	 "FITRF2",
/*7 REG_LORDF3_UNIT_ICON */	 "FITRF3",
/*8 REG_WIZDF1_UNIT_ICON */	 "WZRDF1",
/*9 REG_WIZDF2_UNIT_ICON */	 "WZRDF2",
/*50 REG_GILDF1_UNIT_ICON */	 "GLDRF1",
/*1 REG_PRSTF1_UNIT_ICON */	 "PRESF1",
/*2 REG_ELORD1_UNIT_ICON */  "ELVNM1",
/*3 REG_ELORD2_UNIT_ICON */  "ELVNM2",
/*4 REG_ELADY1_UNIT_ICON */  "ELVNF1",
/*5 REG_DLORD1_UNIT_ICON */  "DWVNM1",
/*6 REG_DLORD2_UNIT_ICON */  "DWVNM2",
/*7 REG_DLORD3_UNIT_ICON */  "DWVNM3",
/*8 REG_GLORD1_UNIT_ICON */  "GBLNM1",
/*9 REG_GLADY1_UNIT_ICON */  "GBLNF1",
/*60 GORGON_UNIT_ICON		*/	"AWNSH3",
/*1 SPIDERKING_UNIT_ICON */	"AWNSH1",
/*2 RHUOBHE_UNIT_ICON		*/	"AWNSH2",
/*3 LET_LORDM1_UNIT_ICON */	 "FITRM1",
/*4 LET_LORDM2_UNIT_ICON */	 "FITRM2",
/*5 LET_LORDM3_UNIT_ICON */	 "FITRM3",
/*6 LET_LORDM4_UNIT_ICON */	 "FITRM4",
/*7 LET_LORDM5_UNIT_ICON */	 "FITRM5",
/*8 LET_LORDM6_UNIT_ICON */	 "FITRM6",
/*9 LET_LORDM7_UNIT_ICON */	 "FITRM7",
/*70 LET_WIZDM1_UNIT_ICON */	 "WZRDM1",
/*1 LET_WIZDM2_UNIT_ICON */	 "WZRDM2",
/*2 LET_WIZDM3_UNIT_ICON */	 "WZRDM3",
/*3 LET_WIZDM4_UNIT_ICON */	 "WZRDM4",
/*4 LET_GILDM1_UNIT_ICON */	 "GLDRM1",
/*5 LET_GILDM2_UNIT_ICON */	 "GLDRM2",
/*6 LET_PRSTM1_UNIT_ICON */	 "PRESM1",
/*7 LET_PRSTM2_UNIT_ICON */	 "PRESM2",
/*8 LET_LORDF1_UNIT_ICON */	 "FITRF1",
/*9 LET_LORDF2_UNIT_ICON */	 "FITRF2",
/*80 LET_LORDF3_UNIT_ICON */	 "FITRF3",
/*1 LET_WIZDF1_UNIT_ICON */	 "WZRDF1",
/*2 LET_WIZDF2_UNIT_ICON */	 "WZRDF2",
/*3 LET_GILDF1_UNIT_ICON */	 "GLDRF1",
/*4 LET_PRSTF1_UNIT_ICON */	 "PRESF1",
/*5 LET_ELORD1_UNIT_ICON */  "ELVNM1",
/*6 LET_ELORD2_UNIT_ICON */  "ELVNM2",
/*7 LET_ELADY1_UNIT_ICON */  "ELVNF1",
/*8 LET_DLORD1_UNIT_ICON */  "DWVNM1",
/*9 LET_DLORD2_UNIT_ICON */  "DWVNM2",
/*90 LET_DLORD3_UNIT_ICON */  "DWVNM3",
/*1 LET_GLORD1_UNIT_ICON */  "GBLNM1",
/*2 LET_GLADY1_UNIT_ICON */  "GBLNF1",
/*3 VILLAGE_ICON	 */		"VILLAGE",
/*4 SEAPORT_ICON	 */		"SEAPORT",
/*5 SEAPORT2_ICON		 */	"SEAPORT2",
/*6 PORT_ICON			*/		"PORT",
/*7 WIZTOWER_ICON		*/		"WIZTOWER",
/*8 RUINS_ICON			*/		"RUINS",
/*9 LAW1_ICON			*/		"LAW1",
/*100 LAW2_ICON			*/		"LAW2",
/*101 LAW3_ICON			*/		"LAW3",
/*102 LAW4_ICON			*/		"LAW4",
/*103 TEMPLE1_ICON	 */		"TEMPLE1",
/*104 TEMPLE2_ICON	 */		"TEMPLE2",
/*105 TEMPLE3_ICON	 */		"TEMPLE3",
/*106 TEMPLE4_ICON	 */		"TEMPLE4",
/*107 GUILD1_ICON		  */		"GUILD1",
/*108 GUILD2_ICON		  */		"GUILD2",
/*109 GUILD3_ICON		  */		"GUILD3",
/*110 GUILD4_ICON		  */		"GUILD4",
/*111 SOURCE1_ICON	 */		"SOURCE1",
/*112 SOURCE2_ICON	 */		"SOURCE2",
/*113 SOURCE3_ICON	 */		"SOURCE3",
/*114 SOURCE4_ICON	 */		"SOURCE4",
/*115 CASTLE1_ICON	 */		"CASTLE1",
/*116 CASTLE2_ICON	 */		"CASTLE2",
/*117 CASTLE3_ICON	 */		"CASTLE3",
/*118 CASTLE4_ICON	 */		"CASTLE4",
/*119 SPECIAL1_ICON	 */		"SPIDERFL",
/*120 SPECIAL2_ICON	 */		"RHUANOCH",
/*121 SPECIAL3_ICON	 */		"FELLPORT",
};
DEFINE_VECTOR_CLASS(CSTRPTR, GameIcons);


///* This is the troop abbriviation.  Use the MAP_ICON enum as the index */
//DEFINE_VECTOR_DATA_S(CSTRPTR,IconAbbriv,MAP_ICON_COUNT) = {
///* NO_MAP_ICON		  */		"",
///* A_INF_UNIT_ICON	*/		"Infantry",
///* A_EIN_UNIT_ICON	*/		"Elt.Inf.",
///* A_BOW_UNIT_ICON	*/		"Archer",
///* A_IRR_UNIT_ICON	*/		"Irreg.",
///* A_PIK_UNIT_ICON	*/		"Pikemen",
///* A_LVY_UNIT_ICON	*/		"Levy",
///* A_SCT_UNIT_ICON	*/		"Scout",
///* A_KNT_UNIT_ICON	*/		"Knights",
///* A_CAV_UNIT_ICON	*/		"Lt.Cav.",
///* M_INF_UNIT_ICON	*/		"MercInf",
///* M_BOW_UNIT_ICON	*/		"MercBow",
///* M_IRR_UNIT_ICON	*/		"MercIrr",
///* M_PIK_UNIT_ICON	*/		"MercPike",
///* M_CAV_UNIT_ICON	*/		"MercCav",
///* E_INF_UNIT_ICON	*/		"ElfInf",
///* E_BOW_UNIT_ICON	*/		"ElfBow",
///* E_CAV_UNIT_ICON	*/		"ElfCav",
///* D_INF_UNIT_ICON	*/		"DwrfInf",
///* D_BOW_UNIT_ICON	*/		"DwrfBow",
///* G_INF_UNIT_ICON	*/		"Goblin",
///* G_BOW_UNIT_ICON	*/		"Gob.Bow",
///* G_CAV_UNIT_ICON	*/		"Gob.Cav.",
///* N_INF_UNIT_ICON	*/		"GnollInf",
///* N_IRR_UNIT_ICON	*/		"GnollIrr",
///* C_HPY_UNIT_ICON	*/		"Harpy",
///* C_SKL_UNIT_ICON	*/		"Skeleton",
///* C_SPD_UNIT_ICON	*/		"Spider",
///* C_HEL_UNIT_ICON	*/		"HellHnd",
///* C_WYV_UNIT_ICON	*/		"Wyvern",
//
///* REG_LORDM1_UNIT_ICON */	 "Regent",
///* REG_LORDM2_UNIT_ICON */	 "Regent",
///* REG_LORDM3_UNIT_ICON */	 "Regent",
///* REG_LORDM4_UNIT_ICON */	 "Regent",
///* REG_LORDM5_UNIT_ICON */	 "Regent",
///* REG_LORDM6_UNIT_ICON */	 "Regent",
///* REG_LORDM7_UNIT_ICON */	 "Regent",
///* REG_WIZDM1_UNIT_ICON */	 "Regent",
///* REG_WIZDM2_UNIT_ICON */	 "Regent",
///* REG_WIZDM3_UNIT_ICON */	 "Regent",
///* REG_WIZDM4_UNIT_ICON */	 "Regent",
///* REG_GILDM1_UNIT_ICON */	 "Regent",
///* REG_GILDM2_UNIT_ICON */	 "Regent",
///* REG_PRSTM1_UNIT_ICON */	 "Regent",
///* REG_PRSTM2_UNIT_ICON */	 "Regent",
///* REG_LORDF1_UNIT_ICON */	 "Regent",
///* REG_LORDF2_UNIT_ICON */	 "Regent",
///* REG_LORDF3_UNIT_ICON */	 "Regent",
///* REG_WIZDF1_UNIT_ICON */	 "Regent",
///* REG_WIZDF2_UNIT_ICON */	 "Regent",
///* REG_GILDF1_UNIT_ICON */	 "Regent",
///* REG_PRSTF1_UNIT_ICON */	 "Regent",
///* REG_ELORD1_UNIT_ICON */  "Regent",
///* REG_ELORD2_UNIT_ICON */  "Regent",
///* REG_ELADY1_UNIT_ICON */  "Regent",
///* REG_DLORD1_UNIT_ICON */  "Regent",
///* REG_DLORD2_UNIT_ICON */  "Regent",
///* REG_DLORD3_UNIT_ICON */  "Regent",
///* REG_GLORD1_UNIT_ICON */  "Regent",
///* REG_GLADY1_UNIT_ICON */  "Regent",
///* GORGON_UNIT_ICON		*/  "Gorgon",
///* SPIDERKING_UNIT_ICON */  "Spider",
///* RHUOBHE_UNIT_ICON		*/  "Rhuobhe",
///* LET_LORDM1_UNIT_ICON */	 "Lieut.",
///* LET_LORDM2_UNIT_ICON */	 "Lieut.",
///* LET_LORDM3_UNIT_ICON */	 "Lieut.",
///* LET_LORDM4_UNIT_ICON */	 "Lieut.",
///* LET_LORDM5_UNIT_ICON */	 "Lieut.",
///* LET_LORDM6_UNIT_ICON */	 "Lieut.",
///* LET_LORDM7_UNIT_ICON */	 "Lieut.",
///* LET_WIZDM1_UNIT_ICON */	 "Wizard",
///* LET_WIZDM2_UNIT_ICON */	 "Wizard",
///* LET_WIZDM3_UNIT_ICON */	 "Wizard",
///* LET_WIZDM4_UNIT_ICON */	 "Wizard",
///* LET_GILDM1_UNIT_ICON */	 "Lieut.",
///* LET_GILDM2_UNIT_ICON */	 "Lieut.",
///* LET_PRSTM1_UNIT_ICON */	 "Priest",
///* LET_PRSTM2_UNIT_ICON */	 "Priest",
///* LET_LORDF1_UNIT_ICON */	 "Lieut.",
///* LET_LORDF2_UNIT_ICON */	 "Lieut.",
///* LET_LORDF3_UNIT_ICON */	 "Lieut.",
///* LET_WIZDF1_UNIT_ICON */	 "Wizard",
///* LET_WIZDF2_UNIT_ICON */	 "Wizard",
///* LET_GILDF1_UNIT_ICON */	 "Lieut.",
///* LET_PRSTF1_UNIT_ICON */	 "Priest",
///* LET_ELORD1_UNIT_ICON */  "Lieut.",
///* LET_ELORD2_UNIT_ICON */  "Lieut.",
///* LET_ELADY1_UNIT_ICON */  "Wizard",
///* LET_DLORD1_UNIT_ICON */  "Lieut.",
///* LET_DLORD2_UNIT_ICON */  "Lieut.",
///* LET_DLORD3_UNIT_ICON */  "Lieut.",
///* LET_GLORD1_UNIT_ICON */  "Lieut.",
///* LET_GLADY1_UNIT_ICON */  "Lieut.",
//};
//DEFINE_VECTOR_CLASS(CSTRPTR, IconAbbriv);
//
///* This is the troop title.  Use the MAP_ICON enum as the index */
//DEFINE_VECTOR_DATA_S(CSTRPTR,UnitTitle,MAP_ICON_COUNT) = {
///* NO_MAP_ICON		  */		"",
///* A_INF_UNIT_ICON	*/		"Infantry",
///* A_EIN_UNIT_ICON	*/		"Elite Infantry",
///* A_BOW_UNIT_ICON	*/		"Archers",
///* A_IRR_UNIT_ICON	*/		"Irregulars",
///* A_PIK_UNIT_ICON	*/		"Pikemen",
///* A_LVY_UNIT_ICON	*/		"Levies",
///* A_SCT_UNIT_ICON	*/		"Scouts",
///* A_KNT_UNIT_ICON	*/		"Knights",
///* A_CAV_UNIT_ICON	*/		"Cavalry",
///* M_INF_UNIT_ICON	*/		"Merc. Infantry",
///* M_BOW_UNIT_ICON	*/		"Merc. Archers",
///* M_IRR_UNIT_ICON	*/		"Merc. Irregulars",
///* M_PIK_UNIT_ICON	*/		"Merc. Pike",
///* M_CAV_UNIT_ICON	*/		"Merc. Cavalry",
///* E_INF_UNIT_ICON	*/		"Elven Infantry",
///* E_BOW_UNIT_ICON	*/		"Elven Archers",
///* E_CAV_UNIT_ICON	*/		"Elven Cavalry",
///* D_INF_UNIT_ICON	*/		"Dwarf Infantry",
///* D_BOW_UNIT_ICON	*/		"Dwarf Bowmen",
///* G_INF_UNIT_ICON	*/		"Goblin Infantry",
///* G_BOW_UNIT_ICON	*/		"Goblin Archers",
///* G_CAV_UNIT_ICON	*/		"Goblin Cavalry",
///* N_INF_UNIT_ICON	*/		"Gnoll Infantry",
///* N_IRR_UNIT_ICON	*/		"Gnoll Marauders",
///* C_HPY_UNIT_ICON	*/		"Harpies",
///* C_SKL_UNIT_ICON	*/		"Skeletons",
///* C_SPD_UNIT_ICON	*/		"Spiders",
///* C_HEL_UNIT_ICON	*/		"Hellhounds",
///* C_WYV_UNIT_ICON	*/		"Wyvern",
//
///* REG_LORDM1_UNIT_ICON */	 "Regent",
///* REG_LORDM2_UNIT_ICON */	 "Regent",
///* REG_LORDM3_UNIT_ICON */	 "Regent",
///* REG_LORDM4_UNIT_ICON */	 "Regent",
///* REG_LORDM5_UNIT_ICON */	 "Regent",
///* REG_LORDM6_UNIT_ICON */	 "Regent",
///* REG_LORDM7_UNIT_ICON */	 "Regent",
///* REG_WIZDM1_UNIT_ICON */	 "Regent",
///* REG_WIZDM2_UNIT_ICON */	 "Regent",
///* REG_WIZDM3_UNIT_ICON */	 "Regent",
///* REG_WIZDM4_UNIT_ICON */	 "Regent",
///* REG_GILDM1_UNIT_ICON */	 "Regent",
///* REG_GILDM2_UNIT_ICON */	 "Regent",
///* REG_PRSTM1_UNIT_ICON */	 "Regent",
///* REG_PRSTM2_UNIT_ICON */	 "Regent",
///* REG_LORDF1_UNIT_ICON */	 "Regent",
///* REG_LORDF2_UNIT_ICON */	 "Regent",
///* REG_LORDF3_UNIT_ICON */	 "Regent",
///* REG_WIZDF1_UNIT_ICON */	 "Regent",
///* REG_WIZDF2_UNIT_ICON */	 "Regent",
///* REG_GILDF1_UNIT_ICON */	 "Regent",
///* REG_PRSTF1_UNIT_ICON */	 "Regent",
///* REG_ELORD1_UNIT_ICON */  "Regent",
///* REG_ELORD2_UNIT_ICON */  "Regent",
///* REG_ELADY1_UNIT_ICON */  "Regent",
///* REG_DLORD1_UNIT_ICON */  "Regent",
///* REG_DLORD2_UNIT_ICON */  "Regent",
///* REG_DLORD3_UNIT_ICON */  "Regent",
///* REG_GLORD1_UNIT_ICON */  "Regent",
///* REG_GLADY1_UNIT_ICON */  "Regent",
///* GORGON_UNIT_ICON		*/  "The Gorgon",
///* SPIDERKING_UNIT_ICON */  "The Spider",
///* RHUOBHE_UNIT_ICON		*/  "Rhuobhe Manslayer",
///* LET_LORDM1_UNIT_ICON */	 "Lieutenant",
///* LET_LORDM2_UNIT_ICON */	 "Lieutenant",
///* LET_LORDM3_UNIT_ICON */	 "Lieutenant",
///* LET_LORDM4_UNIT_ICON */	 "Lieutenant",
///* LET_LORDM5_UNIT_ICON */	 "Lieutenant",
///* LET_LORDM6_UNIT_ICON */	 "Lieutenant",
///* LET_LORDM7_UNIT_ICON */	 "Lieutenant",
///* LET_WIZDM1_UNIT_ICON */	 "Wizard",
///* LET_WIZDM2_UNIT_ICON */	 "Wizard",
///* LET_WIZDM3_UNIT_ICON */	 "Wizard",
///* LET_WIZDM4_UNIT_ICON */	 "Wizard",
///* LET_GILDM1_UNIT_ICON */	 "Lieut.",
///* LET_GILDM2_UNIT_ICON */	 "Lieut.",
///* LET_PRSTM1_UNIT_ICON */	 "Priest",
///* LET_PRSTM2_UNIT_ICON */	 "Priest",
///* LET_LORDF1_UNIT_ICON */	 "Lieutenant",
///* LET_LORDF2_UNIT_ICON */	 "Lieutenant",
///* LET_LORDF3_UNIT_ICON */	 "Lieutenant",
///* LET_WIZDF1_UNIT_ICON */	 "Wizard",
///* LET_WIZDF2_UNIT_ICON */	 "Wizard",
///* LET_GILDF1_UNIT_ICON */	 "Lieutenant",
///* LET_PRSTF1_UNIT_ICON */	 "Priest",
///* LET_ELORD1_UNIT_ICON */  "Lieutenant",
///* LET_ELORD2_UNIT_ICON */  "Lieutenant",
///* LET_ELADY1_UNIT_ICON */  "Wizard",
///* LET_DLORD1_UNIT_ICON */  "Lieutenant",
///* LET_DLORD2_UNIT_ICON */  "Lieutenant",
///* LET_DLORD3_UNIT_ICON */  "Lieutenant",
///* LET_GLORD1_UNIT_ICON */  "Lieutenant",
///* LET_GLADY1_UNIT_ICON */  "Lieutenant",
//};
//DEFINE_VECTOR_CLASS(CSTRPTR,UnitTitle);

/* This is the troop abbriviation.  Use the MAP_ICON enum as the index */
DEFINE_VECTOR_DATA_S(GAME_STRING,gsIconAbbriv,MAP_ICON_COUNT) = {
/* NO_MAP_ICON		  */		STR_NULL,
/* A_INF_UNIT_ICON	*/		STR_UNIT_ABBRV_INF,
/* A_EIN_UNIT_ICON	*/		STR_UNIT_ABBRV_EINF,
/* A_BOW_UNIT_ICON	*/		STR_UNIT_ABBRV_ARCH,
/* A_IRR_UNIT_ICON	*/		STR_UNIT_ABBRV_IRR,
/* A_PIK_UNIT_ICON	*/		STR_UNIT_ABBRV_PIKE,
/* A_LVY_UNIT_ICON	*/		STR_UNIT_ABBRV_LEVY,
/* A_SCT_UNIT_ICON	*/		STR_UNIT_ABBRV_SCT,
/* A_KNT_UNIT_ICON	*/		STR_UNIT_ABBRV_KNIG,
/* A_CAV_UNIT_ICON	*/		STR_UNIT_ABBRV_LCAV,
/* M_INF_UNIT_ICON	*/		STR_UNIT_ABBRV_MINF,
/* M_BOW_UNIT_ICON	*/		STR_UNIT_ABBRV_MBOW,
/* M_IRR_UNIT_ICON	*/		STR_UNIT_ABBRV_MIRR,
/* M_PIK_UNIT_ICON	*/		STR_UNIT_ABBRV_MPIK,
/* M_CAV_UNIT_ICON	*/		STR_UNIT_ABBRV_MCAV,
/* E_INF_UNIT_ICON	*/		STR_UNIT_ABBRV_ELINF,
/* E_BOW_UNIT_ICON	*/		STR_UNIT_ABBRV_ELBOW,
/* E_CAV_UNIT_ICON	*/		STR_UNIT_ABBRV_EKCAV,
/* D_INF_UNIT_ICON	*/		STR_UNIT_ABBRV_DINF,
/* D_BOW_UNIT_ICON	*/		STR_UNIT_ABBRV_DBOW,
/* G_INF_UNIT_ICON	*/		STR_UNIT_ABBRV_GOB,
/* G_BOW_UNIT_ICON	*/		STR_UNIT_ABBRV_GBOW,
/* G_CAV_UNIT_ICON	*/		STR_UNIT_ABBRV_GCAV,
/* N_INF_UNIT_ICON	*/		STR_UNIT_ABBRV_GNINF,
/* N_IRR_UNIT_ICON	*/		STR_UNIT_ABBRV_GNIRR,
/* C_HPY_UNIT_ICON	*/		STR_UNIT_ABBRV_HARPY,
/* C_SKL_UNIT_ICON	*/		STR_UNIT_ABBRV_SKEL,
/* C_SPD_UNIT_ICON	*/		STR_UNIT_ABBRV_SPID,
/* C_HEL_UNIT_ICON	*/		STR_UNIT_ABBRV_HHND,
/* C_WYV_UNIT_ICON	*/		STR_UNIT_ABBRV_WYV,

/* REG_LORDM1_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_LORDM2_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_LORDM3_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_LORDM4_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_LORDM5_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_LORDM6_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_LORDM7_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_WIZDM1_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_WIZDM2_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_WIZDM3_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_WIZDM4_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_GILDM1_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_GILDM2_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_PRSTM1_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_PRSTM2_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_LORDF1_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_LORDF2_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_LORDF3_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_WIZDF1_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_WIZDF2_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_GILDF1_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_PRSTF1_UNIT_ICON */	STR_UNIT_ABBRV_REGENT,
/* REG_ELORD1_UNIT_ICON */ STR_UNIT_ABBRV_REGENT,
/* REG_ELORD2_UNIT_ICON */ STR_UNIT_ABBRV_REGENT,
/* REG_ELADY1_UNIT_ICON */ STR_UNIT_ABBRV_REGENT,
/* REG_DLORD1_UNIT_ICON */ STR_UNIT_ABBRV_REGENT,
/* REG_DLORD2_UNIT_ICON */ STR_UNIT_ABBRV_REGENT,
/* REG_DLORD3_UNIT_ICON */ STR_UNIT_ABBRV_REGENT,
/* REG_GLORD1_UNIT_ICON */ STR_UNIT_ABBRV_REGENT,
/* REG_GLADY1_UNIT_ICON */ STR_UNIT_ABBRV_REGENT,
/* GORGON_UNIT_ICON		*/ STR_UNIT_ABBRV_GORGON,
/* SPIDERKING_UNIT_ICON */ STR_UNIT_ABBRV_SPIDER,
/* RHUOBHE_UNIT_ICON		*/ STR_UNIT_ABBRV_RHUOBHE,
/* LET_LORDM1_UNIT_ICON */	STR_UNIT_ABBRV_LIEUT,
/* LET_LORDM2_UNIT_ICON */	STR_UNIT_ABBRV_LIEUT,
/* LET_LORDM3_UNIT_ICON */	STR_UNIT_ABBRV_LIEUT,
/* LET_LORDM4_UNIT_ICON */	STR_UNIT_ABBRV_LIEUT,
/* LET_LORDM5_UNIT_ICON */	STR_UNIT_ABBRV_LIEUT,
/* LET_LORDM6_UNIT_ICON */	STR_UNIT_ABBRV_LIEUT,
/* LET_LORDM7_UNIT_ICON */	STR_UNIT_ABBRV_LIEUT,
/* LET_WIZDM1_UNIT_ICON */	STR_UNIT_ABBRV_WIZ,
/* LET_WIZDM2_UNIT_ICON */	STR_UNIT_ABBRV_WIZ,
/* LET_WIZDM3_UNIT_ICON */	STR_UNIT_ABBRV_WIZ,
/* LET_WIZDM4_UNIT_ICON */	STR_UNIT_ABBRV_WIZ,
/* LET_GILDM1_UNIT_ICON */	STR_UNIT_ABBRV_LIEUT,
/* LET_GILDM2_UNIT_ICON */	STR_UNIT_ABBRV_LIEUT,
/* LET_PRSTM1_UNIT_ICON */	STR_UNIT_ABBRV_PRIEST,
/* LET_PRSTM2_UNIT_ICON */	STR_UNIT_ABBRV_PRIEST,
/* LET_LORDF1_UNIT_ICON */	STR_UNIT_ABBRV_LIEUT,
/* LET_LORDF2_UNIT_ICON */	STR_UNIT_ABBRV_LIEUT,
/* LET_LORDF3_UNIT_ICON */	STR_UNIT_ABBRV_LIEUT,
/* LET_WIZDF1_UNIT_ICON */	STR_UNIT_ABBRV_WIZ,
/* LET_WIZDF2_UNIT_ICON */	STR_UNIT_ABBRV_WIZ,
/* LET_GILDF1_UNIT_ICON */	STR_UNIT_ABBRV_LIEUT,
/* LET_PRSTF1_UNIT_ICON */	STR_UNIT_ABBRV_PRIEST,
/* LET_ELORD1_UNIT_ICON */ STR_UNIT_ABBRV_LIEUT,
/* LET_ELORD2_UNIT_ICON */ STR_UNIT_ABBRV_LIEUT,
/* LET_ELADY1_UNIT_ICON */ STR_UNIT_ABBRV_WIZ,
/* LET_DLORD1_UNIT_ICON */ STR_UNIT_ABBRV_LIEUT,
/* LET_DLORD2_UNIT_ICON */ STR_UNIT_ABBRV_LIEUT,
/* LET_DLORD3_UNIT_ICON */ STR_UNIT_ABBRV_LIEUT,
/* LET_GLORD1_UNIT_ICON */ STR_UNIT_ABBRV_LIEUT,
/* LET_GLADY1_UNIT_ICON */ STR_UNIT_ABBRV_LIEUT,
};
DEFINE_VECTOR_CLASS(GAME_STRING, gsIconAbbriv);

/* This is the troop title.  Use the MAP_ICON enum as the index */
DEFINE_VECTOR_DATA_S(GAME_STRING,gsUnitTitle,MAP_ICON_COUNT) = {
/* NO_MAP_ICON		  */		STR_NULL,
/* A_INF_UNIT_ICON	*/		STR_UNIT_NAME_INF,
/* A_EIN_UNIT_ICON	*/		STR_UNIT_NAME_EINF,
/* A_BOW_UNIT_ICON	*/		STR_UNIT_NAME_ARCH,
/* A_IRR_UNIT_ICON	*/		STR_UNIT_NAME_IRR,
/* A_PIK_UNIT_ICON	*/		STR_UNIT_NAME_PIKE,
/* A_LVY_UNIT_ICON	*/		STR_UNIT_NAME_LEVY,
/* A_SCT_UNIT_ICON	*/		STR_UNIT_NAME_SCT,
/* A_KNT_UNIT_ICON	*/		STR_UNIT_NAME_KNIG,
/* A_CAV_UNIT_ICON	*/		STR_UNIT_NAME_LCAV,
/* M_INF_UNIT_ICON	*/		STR_UNIT_NAME_MINF,
/* M_BOW_UNIT_ICON	*/		STR_UNIT_NAME_MBOW,
/* M_IRR_UNIT_ICON	*/		STR_UNIT_NAME_MIRR,
/* M_PIK_UNIT_ICON	*/		STR_UNIT_NAME_MPIK,
/* M_CAV_UNIT_ICON	*/		STR_UNIT_NAME_MCAV,
/* E_INF_UNIT_ICON	*/		STR_UNIT_NAME_ELINF,
/* E_BOW_UNIT_ICON	*/		STR_UNIT_NAME_ELBOW,
/* E_CAV_UNIT_ICON	*/		STR_UNIT_NAME_EKCAV,
/* D_INF_UNIT_ICON	*/		STR_UNIT_NAME_DINF,
/* D_BOW_UNIT_ICON	*/		STR_UNIT_NAME_DBOW,
/* G_INF_UNIT_ICON	*/		STR_UNIT_NAME_GOB,
/* G_BOW_UNIT_ICON	*/		STR_UNIT_NAME_GBOW,
/* G_CAV_UNIT_ICON	*/		STR_UNIT_NAME_GCAV,
/* N_INF_UNIT_ICON	*/		STR_UNIT_NAME_GNINF,
/* N_IRR_UNIT_ICON	*/		STR_UNIT_NAME_GNIRR,
/* C_HPY_UNIT_ICON	*/		STR_UNIT_NAME_HARPY,
/* C_SKL_UNIT_ICON	*/		STR_UNIT_NAME_SKEL,
/* C_SPD_UNIT_ICON	*/		STR_UNIT_NAME_SPID,
/* C_HEL_UNIT_ICON	*/		STR_UNIT_NAME_HHND,
/* C_WYV_UNIT_ICON	*/		STR_UNIT_NAME_WYV,

/* REG_LORDM1_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_LORDM2_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_LORDM3_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_LORDM4_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_LORDM5_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_LORDM6_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_LORDM7_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_WIZDM1_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_WIZDM2_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_WIZDM3_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_WIZDM4_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_GILDM1_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_GILDM2_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_PRSTM1_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_PRSTM2_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_LORDF1_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_LORDF2_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_LORDF3_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_WIZDF1_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_WIZDF2_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_GILDF1_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_PRSTF1_UNIT_ICON */	STR_UNIT_NAME_REGENT,
/* REG_ELORD1_UNIT_ICON */ STR_UNIT_NAME_REGENT,
/* REG_ELORD2_UNIT_ICON */ STR_UNIT_NAME_REGENT,
/* REG_ELADY1_UNIT_ICON */ STR_UNIT_NAME_REGENT,
/* REG_DLORD1_UNIT_ICON */ STR_UNIT_NAME_REGENT,
/* REG_DLORD2_UNIT_ICON */ STR_UNIT_NAME_REGENT,
/* REG_DLORD3_UNIT_ICON */ STR_UNIT_NAME_REGENT,
/* REG_GLORD1_UNIT_ICON */ STR_UNIT_NAME_REGENT,
/* REG_GLADY1_UNIT_ICON */ STR_UNIT_NAME_REGENT,
/* GORGON_UNIT_ICON		*/ STR_UNIT_NAME_GORGON,
/* SPIDERKING_UNIT_ICON */ STR_UNIT_NAME_SPIDER,
/* RHUOBHE_UNIT_ICON		*/ STR_UNIT_NAME_RHUOBHE,
/* LET_LORDM1_UNIT_ICON */	STR_UNIT_NAME_LIEUT,
/* LET_LORDM2_UNIT_ICON */	STR_UNIT_NAME_LIEUT,
/* LET_LORDM3_UNIT_ICON */	STR_UNIT_NAME_LIEUT,
/* LET_LORDM4_UNIT_ICON */	STR_UNIT_NAME_LIEUT,
/* LET_LORDM5_UNIT_ICON */	STR_UNIT_NAME_LIEUT,
/* LET_LORDM6_UNIT_ICON */	STR_UNIT_NAME_LIEUT,
/* LET_LORDM7_UNIT_ICON */	STR_UNIT_NAME_LIEUT,
/* LET_WIZDM1_UNIT_ICON */	STR_UNIT_NAME_WIZ,
/* LET_WIZDM2_UNIT_ICON */	STR_UNIT_NAME_WIZ,
/* LET_WIZDM3_UNIT_ICON */	STR_UNIT_NAME_WIZ,
/* LET_WIZDM4_UNIT_ICON */	STR_UNIT_NAME_WIZ,
/* LET_GILDM1_UNIT_ICON */	STR_UNIT_NAME_LIEUT,
/* LET_GILDM2_UNIT_ICON */	STR_UNIT_NAME_LIEUT,
/* LET_PRSTM1_UNIT_ICON */	STR_UNIT_NAME_PRIEST,
/* LET_PRSTM2_UNIT_ICON */	STR_UNIT_NAME_PRIEST,
/* LET_LORDF1_UNIT_ICON */	STR_UNIT_NAME_LIEUT,
/* LET_LORDF2_UNIT_ICON */	STR_UNIT_NAME_LIEUT,
/* LET_LORDF3_UNIT_ICON */	STR_UNIT_NAME_LIEUT,
/* LET_WIZDF1_UNIT_ICON */	STR_UNIT_NAME_WIZ,
/* LET_WIZDF2_UNIT_ICON */	STR_UNIT_NAME_WIZ,
/* LET_GILDF1_UNIT_ICON */	STR_UNIT_NAME_LIEUT,
/* LET_PRSTF1_UNIT_ICON */	STR_UNIT_NAME_PRIEST,
/* LET_ELORD1_UNIT_ICON */ STR_UNIT_NAME_LIEUT,
/* LET_ELORD2_UNIT_ICON */ STR_UNIT_NAME_LIEUT,
/* LET_ELADY1_UNIT_ICON */ STR_UNIT_NAME_WIZ,
/* LET_DLORD1_UNIT_ICON */ STR_UNIT_NAME_LIEUT,
/* LET_DLORD2_UNIT_ICON */ STR_UNIT_NAME_LIEUT,
/* LET_DLORD3_UNIT_ICON */ STR_UNIT_NAME_LIEUT,
/* LET_GLORD1_UNIT_ICON */ STR_UNIT_NAME_LIEUT,
/* LET_GLADY1_UNIT_ICON */ STR_UNIT_NAME_LIEUT,
};
DEFINE_VECTOR_CLASS(GAME_STRING,gsUnitTitle);



VOX_TYPE GameIconVOX[MAP_ICON_COUNT] = {
/* NO_MAP_ICON		  */			VOX_HUMAN_MALE_YOUNG_1,
/* A_INF_UNIT_ICON	*/			VOX_HUMAN_MALE_YOUNG_1,		// Infantry
/* A_EIN_UNIT_ICON	*/			VOX_HUMAN_MALE_YOUNG_1,		// Elite Infantry
/* A_BOW_UNIT_ICON	*/			VOX_HUMAN_MALE_YOUNG_2,		// Archer
/* A_IRR_UNIT_ICON	*/			VOX_HUMAN_MALE_YOUNG_3,		// Irregular
/* A_PIK_UNIT_ICON	*/			VOX_HUMAN_MALE_YOUNG_1,		// Pikeman
/* A_LVY_UNIT_ICON	*/			VOX_HUMAN_MALE_YOUNG_3,		// Irregular
/* A_SCT_UNIT_ICON	*/			VOX_HUMAN_MALE_YOUNG_2,		// Irregular
/* A_KNT_UNIT_ICON	*/			VOX_HUMAN_MALE_YOUNG_1,		// Knight/Heavy Cavalry
/* A_CAV_UNIT_ICON	*/			VOX_HUMAN_MALE_YOUNG_2,		// Light Cavalry
/* M_INF_UNIT_ICON	*/			VOX_HUMAN_MALE_MATURE_1,	// Mercenary Infantry
/* M_BOW_UNIT_ICON	*/			VOX_HUMAN_MALE_MATURE_1,	// Mercenary Infantry
/* M_IRR_UNIT_ICON	*/			VOX_HUMAN_MALE_MATURE_1,	// Mercenary Infantry
/* M_PIK_UNIT_ICON	*/			VOX_HUMAN_MALE_MATURE_1,	// Mercenary Pikeman
/* M_CAV_UNIT_ICON	*/			VOX_HUMAN_MALE_MATURE_2,	// Mercenary Officer
/* E_INF_UNIT_ICON	*/			VOX_HUMAN_MALE_YOUNG_2,	 	// Elf Infantry
/* E_BOW_UNIT_ICON	*/			VOX_HUMAN_MALE_YOUNG_3,	 	// Elf Archer
/* E_CAV_UNIT_ICON	*/			VOX_HUMAN_MALE_MATURE_2,	// Elf Officer 2
/* D_INF_UNIT_ICON	*/			VOX_DWARF_MALE_1,	      // Dwarf Infantry
/* D_BOW_UNIT_ICON	*/			VOX_DWARF_MALE_1,	      // Dwarf Archer
/* G_INF_UNIT_ICON	*/			VOX_GOBLIN_MALE_1,	  	// Goblin Infantry
/* G_BOW_UNIT_ICON	*/			VOX_GOBLIN_MALE_1,	  	// Goblin Archer
/* G_CAV_UNIT_ICON	*/			VOX_GOBLIN_MALE_1,	  	// Goblin Cavalry
/* N_INF_UNIT_ICON	*/			VOX_GNOLL_1,	      	// Gnoll Irregular
/* N_IRR_UNIT_ICON	*/			VOX_GNOLL_1,	      	// Gnoll Irregular
/* C_HPY_UNIT_ICON	*/			VOX_NO_ENTRY,
/* C_SKL_UNIT_ICON	*/			VOX_SKELETON_1,	     	// Troop Skeleton
/* C_SPD_UNIT_ICON	*/			VOX_NO_ENTRY,
/* C_HEL_UNIT_ICON	*/			VOX_NO_ENTRY,
/* C_WYV_UNIT_ICON	*/			VOX_NO_ENTRY,

/* REG_LORDM1_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_1,
/* REG_LORDM2_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_1,
/* REG_LORDM3_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_2,
/* REG_LORDM4_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_2,
/* REG_LORDM5_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_2,
/* REG_LORDM6_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_2,
/* REG_LORDM7_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_2,
/* REG_WIZDM1_UNIT_ICON */		VOX_HUMAN_MALE_OLD_1,
/* REG_WIZDM2_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_1,
/* REG_WIZDM3_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_2,
/* REG_WIZDM4_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_2,
/* REG_GILDM1_UNIT_ICON */		VOX_HUMAN_MALE_YOUNG_1,
/* REG_GILDM2_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_1,
/* REG_PRSTM1_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_1,
/* REG_PRSTM2_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_2,
/* REG_LORDF1_UNIT_ICON */		VOX_HUMAN_FEMALE_YOUNG_1,
/* REG_LORDF2_UNIT_ICON */		VOX_HUMAN_FEMALE_MATURE_1,
/* REG_LORDF3_UNIT_ICON */		VOX_HUMAN_FEMALE_MATURE_2,
/* REG_WIZDF1_UNIT_ICON */		VOX_HUMAN_FEMALE_YOUNG_1,
/* REG_WIZDF2_UNIT_ICON */		VOX_HUMAN_FEMALE_YOUNG_2,
/* REG_GILDF1_UNIT_ICON */  	VOX_HUMAN_FEMALE_YOUNG_2,
/* REG_PRSTF1_UNIT_ICON */		VOX_HUMAN_FEMALE_MATURE_2,
/* REG_ELORD1_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_2,
/* REG_ELORD2_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_2,
/* REG_ELADY1_UNIT_ICON	*/		VOX_HUMAN_FEMALE_MATURE_2,
/* REG_DLORD1_UNIT_ICON */		VOX_DWARF_MALE_1,
/* REG_DLORD2_UNIT_ICON */		VOX_DWARF_MALE_1,
/* REG_DLORD3_UNIT_ICON */		VOX_DWARF_MALE_1,
/* REG_GLORD1_UNIT_ICON */		VOX_GOBLIN_MALE_1,
/* REG_GLADY1_UNIT_ICON */		VOX_GOBLIN_MALE_1,
/* GORGON_UNIT_ICON		*/		VOX_GORGON_1,	      	// Troop Gorgon
/* SPIDERKING_UNIT_ICON */		VOX_SPIDER_KING_1,     	// Spider King
/* RHUOBHE_UNIT_ICON		*/		VOX_RHUOBHE_1,	      	// Rhuobhe Manslayer
/* LET_LORDM1_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_1,
/* LET_LORDM2_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_1,
/* LET_LORDM3_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_2,
/* LET_LORDM4_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_2,
/* LET_LORDM5_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_2,
/* LET_LORDM6_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_2,
/* LET_LORDM7_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_2,
/* LET_WIZDM1_UNIT_ICON */		VOX_HUMAN_MALE_OLD_1,
/* LET_WIZDM2_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_1,
/* LET_WIZDM3_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_2,
/* LET_WIZDM4_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_2,
/* LET_GILDM1_UNIT_ICON */		VOX_HUMAN_MALE_YOUNG_1,
/* LET_GILDM2_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_1,
/* LET_PRSTM1_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_1,
/* LET_PRSTM2_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_2,
/* LET_LORDF1_UNIT_ICON */		VOX_HUMAN_FEMALE_YOUNG_1,
/* LET_LORDF2_UNIT_ICON */		VOX_HUMAN_FEMALE_MATURE_1,
/* LET_LORDF3_UNIT_ICON */		VOX_HUMAN_FEMALE_MATURE_2,
/* LET_WIZDF1_UNIT_ICON */		VOX_HUMAN_FEMALE_YOUNG_1,
/* LET_WIZDF2_UNIT_ICON */		VOX_HUMAN_FEMALE_YOUNG_2,
/* LET_GILDF1_UNIT_ICON */  	VOX_HUMAN_FEMALE_YOUNG_2,
/* LET_PRSTF1_UNIT_ICON */		VOX_HUMAN_FEMALE_MATURE_2,
/* LET_ELORD1_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_2,
/* LET_ELORD2_UNIT_ICON */		VOX_HUMAN_MALE_MATURE_2,
/* LET_ELADY1_UNIT_ICON	*/		VOX_HUMAN_FEMALE_MATURE_2,
/* LET_DLORD1_UNIT_ICON */		VOX_DWARF_MALE_1,
/* LET_DLORD2_UNIT_ICON */		VOX_DWARF_MALE_1,
/* LET_DLORD3_UNIT_ICON */		VOX_DWARF_MALE_1,
/* LET_GLORD1_UNIT_ICON */		VOX_GOBLIN_MALE_1,
/* LET_GLADY1_UNIT_ICON */		VOX_GOBLIN_MALE_1,
};

DEFINE_VECTOR_DATA(UBYTE,UnitDisplaySize) = 
//		A A A A A A A A A M M M M M E E E D D G G G N N
//		i e b i p l s k c i b i p c i b c i b i b c i i h s s h w
//		n i o r i v c n a n o r i a n o a n o n o a n r p k p e y
//		f n w r k y t t v f w r k v f w v f w f w v f r y l d l v
   {0,1,1,1,1,1,1,1,2,2,1,1,1,1,2,1,1,2,1,1,1,1,2,2,2,0,1,2,0,0,
	 1,1,1,1,1,1,1, 1,1,1,1,1,1, 2,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 2,2,2,
	 1,1,1,1,1,1,1, 1,1,1,1,1,1, 2,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 0};

DEFINE_VECTOR_CLASS(UBYTE, UnitDisplaySize);

DEFINE_VECTOR_DATA(SHORT,UnitMaintCost) = 
//		A A A A A A A A A M M M M M E E E D D G G G N N
//		i e b i p l s k c i b i p c i b c i b i b c i i h s s h w
//		n i o r i v c n a n o r i a n o a n o n o a n r p k p e y
//		f n w r k y t t v f w r k v f w v f w f w v f r y l d l v
  	{0,1,2,1,1,1,1,1,2,2,2,2,1,2,2,1,1,2,2,2,1,1,2,1,1,0,0,0,0,0,
	 0,0,0,0,0,0,0, 0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,
	 0,0,0,0,0,0,0, 0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0};

DEFINE_VECTOR_CLASS(SHORT, UnitMaintCost);


// Watcom botches the vector class for this. I don't know why. GWP.
#define NUM_BATTLE_SPELLS 36
ITEMTYPE battle_spells[NUM_BATTLE_SPELLS] = {
// Destroy results
	ITEM_SPELL_HOLY_WORD,
	ITEM_SPELL_FIRE_STORM,
	ITEM_SPELL_METEOR_SWARM,
	ITEM_SPELL_POWER_KILL,
	ITEM_SPELL_IMPROVED_FIREBAL,
	ITEM_SPELL_DEATH,
	ITEM_SPELL_CHAIN_LIGHTNING,
	ITEM_SPELL_DISINTIGRATE,
	ITEM_SPELL_TURN_UNDEAD,

// Rout results
	ITEM_SPELL_FLAME_STRIKE,
	ITEM_SPELL_CONE_OF_COLD,
	ITEM_SPELL_ICE_STORM,
	ITEM_SPELL_FIREBALL,
	ITEM_SPELL_LIGHTNING_BOLT,
	ITEM_SPELL_POISON_ARROW,
	ITEM_SPELL_MAGIC_MISSILE,

// No attack results
	ITEM_SPELL_POWER_STUN,
	ITEM_SPELL_POWER_BLIND,
	ITEM_SPELL_TIME_STOP,			
	
// Magic items useful in battle.
	ITEM_WAND_OF_FIRE_3,
	ITEM_WAND_OF_FIRE_2,
	ITEM_WAND_OF_FIRE_1,
	ITEM_WAND_OF_FROST_3,
	ITEM_WAND_OF_FROST_2,
	ITEM_WAND_OF_FROST_1,
	ITEM_WAND_OF_LIGHTNING_3,
	ITEM_WAND_OF_LIGHTNING_2,
	ITEM_WAND_OF_LIGHTNING_1,
	ITEM_NECKLACE_OF_MISSILES_8,
	ITEM_NECKLACE_OF_MISSILES_7,
	ITEM_NECKLACE_OF_MISSILES_6,
	ITEM_NECKLACE_OF_MISSILES_5,
	ITEM_NECKLACE_OF_MISSILES_4,
	ITEM_NECKLACE_OF_MISSILES_3,
	ITEM_NECKLACE_OF_MISSILES_2,
	ITEM_NECKLACE_OF_MISSILES_1,
};

BOOL			fTutorialSelected = FALSE;
BOOL			fTutorialFirstBattle = FALSE;
LONG			TutorialActionNumber = 0;
LONG			TutorialScreenNumber = 6;

LONG			WaitingCount = 0;
LONG			WaitingWho = 0;
LONG			WaitingWhen = 0;
LONG			WaitingBar216ths = 0;
LONG			lTimeLimit[] = {ITT_2MIN, ITT_4MIN, ITT_6MIN, ITT_NONE};

extern MENU		Menus[];
extern BOOL		char_selected;	/* from game.cpp */
extern BOOL		fSmallMap;
extern BOOL 	fMainMenuUp;

extern PROVINCE	TargetProvince;
extern USHORT	TargetUnit;
extern LONG		prev_action_turn[];

extern LONG 	fAllowDoneButton;

extern BOOL 	fPreparingSpells;	/* from invngui.cpp */

extern BOOL 	fPractice; 			/* from game.cpp */

extern BOOL		fSyncPending;
extern LONG		BattleSync;
extern int		iTTimeLimit;

extern SHORT   ChatDisplaySize;

extern BOOL		fAutoRestoreCursor;
extern SHORT	sMenusUp;
extern BOOL		fMenuClosed;
extern UBYTE	antia_table[];
extern COLORSPEC CurPal[256];
extern BOOL		fNewRes;
extern LONG		print_global[];
extern SHORT gRemapColors[cREMAP_GROUPS][2];
extern LONG		GameSpecificGlobal_type;
extern LONG		GameSpecificGlobal_x;
extern LONG		GameSpecificGlobal_y;
extern LONG		GameSpecificGlobal_w;
extern LONG		GameSpecificGlobal_h;
extern BOOL		GameSpecificArray1[20];
extern BOOL		GameSpecificArray2[20];
extern SHORT	hSaveBkgnd;
extern SHORT	cntSaveBkgnd;
#ifdef _WINDOWS
extern BOOL fStartSIGS;
#endif

static	SHORT	sCommandBattle = 0;
BOOL	fCommandBattleLoop;

static	BOOL	fPlayerAcknowledgedDeath=FALSE;

void ChangeControlMode (LONG, LONG);

/* ========================================================================
	Function	-
	Description -
	Returns	- void
	======================================================================== */
void LoadControlSet (void)
{
	if (fControlMode & 0x100)
		fControlMode &= 0xFF;

//printf("LoadControlSet - fControlMode: %d\n",fControlMode);

	if (iDActnIc!=fERROR) SetPurge(iDActnIc); iDActnIc=fERROR;
	if (iDActnI2!=fERROR) SetPurge(iDActnI2); iDActnI2=fERROR;

	switch (fControlMode)
	{
		case 0:
			iDActnIc = GetResourceStd("UI\\DACT3_IC.PCX", FALSE);
			iDActnI2 = GetResourceStd("UI\\DACT4_IC.PCX", FALSE);
			break;
		case 1:
			iDActnIc = GetResourceStd("UI\\DACT5_IC.PCX", FALSE);
			iDActnI2 = GetResourceStd("UI\\DACT6_IC.PCX", FALSE);
			break;
		case 2:
			iDActnIc = GetResourceStd("UI\\DACTN_IC.PCX", FALSE);
			iDActnI2 = GetResourceStd("UI\\DACT2_IC.PCX", FALSE);
			break;
	}
	#if fUSE_RES_FILES
	SetPurge(iDActnIc);
	SetPurge(iDActnI2);
	#endif
}

void ActivateControlSet (void)
{
	if (fControlMode & 0x100)
		fControlMode &= 0xFF;

	//printf("ActivateControlSet - fControlMode: %d\n",fControlMode);

	del_region(DoAction,0);
	del_region(ChangeControlMode,0);

	switch (fControlMode)
	{
		case 0:
			add_region(  9,120,48,46,0,DoAction, 1,0,regACTION_MODE, STR_GM_TT_ACTION1);
			add_region(  9,176,48,46,0,DoAction, 6,0,regACTION_MODE, STR_GM_TT_ACTION6);
			add_region(  9,232,48,46,0,DoAction, 7,0,regACTION_MODE, STR_GM_TT_ACTION7);
			add_region(  9,288,48,46,0,DoAction,11,0,regACTION_MODE, STR_GM_TT_ACTION11);
			add_region(  9,344,48,46,0,DoAction,13,0,regACTION_MODE, STR_GM_TT_ACTION13);
			add_region(  9,400,48,46,0,DoAction,15,0,regACTION_MODE, STR_GM_TT_ACTION15);
			//add_region(0,0,20,20,KEY_F10,ChangeControlMode,1,0,regACTION_MODE, STR_GM_TT_ACTION16);
			break;

		case 1:
			add_region(  9,64+0*41,48,41,0,DoAction, 1,0,regACTION_MODE, STR_GM_TT_ACTION1);
			add_region(  9,64+1*41,48,41,0,DoAction, 3,0,regACTION_MODE, STR_GM_TT_ACTION3);
			add_region(  9,64+2*41,48,41,0,DoAction, 5,0,regACTION_MODE, STR_GM_TT_ACTION5);
			add_region(  9,64+3*41,48,41,0,DoAction, 6,0,regACTION_MODE, STR_GM_TT_ACTION6);
			add_region(  9,64+4*41,48,41,0,DoAction, 7,0,regACTION_MODE, STR_GM_TT_ACTION7);
			add_region(  9,64+5*41,48,41,0,DoAction,11,0,regACTION_MODE, STR_GM_TT_ACTION11);
			add_region(  9,64+6*41,48,41,0,DoAction,13,0,regACTION_MODE, STR_GM_TT_ACTION13);
			add_region(  9,64+7*41,48,41,0,DoAction,15,0,regACTION_MODE, STR_GM_TT_ACTION15);
			add_region(  9,64+8*41,48,41,0,DoAction,17,0,regACTION_MODE, STR_GM_TT_ACTION17);
			add_region(  9,64+9*41,48,41,0,DoAction,18,0,regACTION_MODE, STR_GM_TT_ACTION18);
			//add_region(0,0,20,20,KEY_F10,ChangeControlMode,1,0,regACTION_MODE, STR_GM_TT_ACTION16);
			break;

		case 2:
			add_region(  9, 56+8,48,46,0,DoAction, 1,0,regACTION_MODE, STR_GM_TT_ACTION1);
			add_region( 57, 56+8,48,46,0,DoAction, 2,0,regACTION_MODE, STR_GM_TT_ACTION2);
			add_region(  9,102+8,48,46,0,DoAction, 3,0,regACTION_MODE, STR_GM_TT_ACTION3);
			add_region( 57,102+8,48,46,0,DoAction, 4,0,regACTION_MODE, STR_GM_TT_ACTION4);
			add_region(  9,148+8,48,46,0,DoAction, 5,0,regACTION_MODE, STR_GM_TT_ACTION5);
			add_region( 57,148+8,48,46,0,DoAction, 6,0,regACTION_MODE, STR_GM_TT_ACTION6);
			add_region(  9,194+8,48,46,0,DoAction, 7,0,regACTION_MODE, STR_GM_TT_ACTION7);
			add_region( 57,194+8,48,46,0,DoAction, 8,0,regACTION_MODE, STR_GM_TT_ACTION8);
			add_region(  9,240+8,48,46,0,DoAction, 9,0,regACTION_MODE, STR_GM_TT_ACTION9);
			add_region( 57,240+8,48,46,0,DoAction,10,0,regACTION_MODE, STR_GM_TT_ACTION10);
			add_region(  9,286+8,48,46,0,DoAction,11,0,regACTION_MODE, STR_GM_TT_ACTION11);
			add_region( 57,286+8,48,46,0,DoAction,12,0,regACTION_MODE, STR_GM_TT_ACTION12);
			add_region(  9,332+8,48,46,0,DoAction,13,0,regACTION_MODE, STR_GM_TT_ACTION13);
			add_region( 57,332+8,48,46,0,DoAction,14,0,regACTION_MODE, STR_GM_TT_ACTION14);
			add_region(  9,378+8,48,46,0,DoAction,15,0,regACTION_MODE, STR_GM_TT_ACTION15);
			add_region( 57,378+8,48,46,0,DoAction,16,0,regACTION_MODE, STR_GM_TT_ACTION16);
			add_region(  9,424+8,48,46,0,DoAction,17,0,regACTION_MODE, STR_GM_TT_ACTION17);
			add_region( 57,424+8,48,46,0,DoAction,18,0,regACTION_MODE, STR_GM_TT_ACTION18);
			//add_region( 0,0,20,20,KEY_F10,ChangeControlMode,-2,0,regACTION_MODE, STR_GM_TT_ACTION16);
			break;
	}
}

void ChangeControlMode (LONG dir, LONG)
{
	if (fControlMode & 0x100)
		fControlMode &= 0xFF;

	if ((fControlMode > 0 && dir==-1) || (fControlMode==2 && dir==-2) || (fControlMode < 2 && dir==1))
	{
		fControlMode += dir;
		LoadControlSet();
		ActivateControlSet();
		fRedrawAll = TRUE;
	}
}

/* ========================================================================
	Function	-
	Description -
	Returns	- void
	======================================================================== */
static LONG		ShowForTimer = 0;
static SHORT * ShowForControl;

void ShowFor (LONG timer, SHORT * control)
{
	ShowForTimer = timer + get_time();
	ShowForControl = control;
}

void TestShowFor (void)
{
//printf("ShowForTimer: %ld  time: %ld  Control: %d\n",ShowForTimer, get_time(), (SHORT)(*ShowForControl));

	if (ShowForTimer && ShowForTimer < get_time() && *ShowForControl)
	{
		*ShowForControl = FALSE;
		ShowForTimer = FALSE;
		fRedrawAll = TRUE;
	}
}

void DepressIcon (void)
{
	xDepressIcon = LastRegionX();
	yDepressIcon = LastRegionY();
	if (xDepressIcon && yDepressIcon)
	{
		fDepressIcon = TRUE;
		fUpdateDataArea = TRUE;
		ShowFor(1, &fDepressIcon);
	}
}

/* ========================================================================
	Function	- 
	Description -
	Returns	- void
	======================================================================== */
void DisplayProcess (LONG mode, LONG color, LONG index1, LONG index2)
{
	LONG	wBar = (WaitingBar216ths > 216) ? 216: WaitingBar216ths;
	char	n[50];
	static LONG mode_ = 0;
	static LONG color_ = 0;
	static LONG index1_ = 0;
	static LONG index2_ = 0;
	static LONG time = 0;

	if ((mode==REALM_IS || mode==FIGHTING) && index2 <= 0)
		return;

	if (	(mode==UNITS_MOVE && time > get_time())
//		|| ((mode==WAITINGFOR || mode==PLAYERSTURN || mode==REALM_IS) && index1==0)
		|| ((mode==PLAYERSTURN || mode==REALM_IS) && index1==0)
		|| (mode==FIGHTING && (index1==0 || index2==0))  )
	{
		mode = mode_;
		color = color_;
		index1 = index1_;
		index2 = index2_;
	}
	else
	{
		time = get_time() + 32;
		mode_ = mode;
		color_ = color;
		index1_ = index1;
		index2_ = index2;
	}

	SysHideCursor();

	if (WaitingWhen && iTTimeLimit != IX_TT_NONE)
		SetRemapTable(GREEN_GRAD);
	else
	{
		SetRemapTable(color);		// non-timer settings
		wBar = 0;
	}

	// draw full bar or time left
	DrawBitmap (422, 409, iDMapBx, 22, 0, 216-wBar, 21);

	if (wBar && iTTimeLimit != IX_TT_NONE)
	{
		SetRemapTable(RED_GRAD);
		DrawBitmap( 422+216-wBar, 409, iDMapBx, 22+216-wBar, 0, wBar, 21 );
	}
	ClearRemapTable();


	switch (mode)
	{
		char RealmName[80];
		char RealmName2[80];
		
		case WAITINGFOR:
			if (index1)
			{
				strcpy(RealmName, realm[index1].mfGetName());
				sprintf(n, STRMGR_GetStr(STR_GM_WAITING_FOR_FMT), RealmName);
			}
			else
			{
				if (index2 > 1 || WaitingWho < 1 || WaitingWho > LAND_REALM_COUNT || !realm[WaitingWho].mfIsPlayerCtrl() )
					sprintf(n, STRMGR_GetStr(STR_PLAYER_WAIT) );
				else
				{
					strcpy(RealmName, realm[WaitingWho].mfGetName());
					sprintf(n, STRMGR_GetStr(STR_GM_WAITING_FOR_FMT), RealmName);
				}
			}
			break;
		case PLAYERSTURN:
			strcpy(RealmName, realm[index1].mfGetName());
			sprintf(n, STRMGR_GetStr(STR_GM_SOMEONES_TURN_FMT), RealmName);
			break;
		case REALM_IS:
			strcpy(RealmName, realm[index1].mfGetName());
			strcpy(RealmName2, szAIAction[index2].mfGetStr());
			sprintf(n, STRMGR_GetStr(STR_GM_PS_IS_PS_FMT), RealmName, RealmName2);
			break;
		case UNITS_MOVE:
			sprintf(n, STRMGR_GetStr(STR_GM_TROOPS_MARCHING), 0, 0);
			break;
		case FIGHTING:
			strcpy(RealmName, realm[units[index1].Realm].mfGetName());
			strcpy(RealmName2, realm[units[index2].Realm].mfGetName());
			sprintf(n, STRMGR_GetStr(STR_GM_PS_IS_FIGHTING_PS_FMT), RealmName, RealmName2);
			break;
		case HASWON:
			strcpy(RealmName, realm[units[index1].Realm].mfGetName());
			sprintf(n, STRMGR_GetStr(STR_GM_PS_WON_BATTLE_FMT), RealmName);
			break;
	}

	// choose correct font
	init_gfont(FONT_SANS_12PT);
	if (gtext_width(n) > 178)
	{
		init_gfont(FONT_SANS_8PT);
		if (gtext_width(n) > 178)
			init_gfont(21);
	}
	print_textf(422+108, 420, WHITE, "^c%s",n);

	SysShowCursor();
}

/* ========================================================================
	Function	- 
	Description -
	Returns	- 
	======================================================================== */
static void PaintDeathNotification(LONG MenuCombo, LONG RealmThatBitTheDust)
{
	LONG mx,my,X,Y;
	char buffer[200];
	char Realmbuf[40];
	SHORT MenuId,ButtonId;

	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	if( GetButtonPosition( MenuId, 1, &X, &Y ) == fERROR)
		return;
	if( GetButtonSize( MenuId, 1, &mx, &my ) == fERROR)
		return;

	sprintf(Realmbuf,realm[RealmThatBitTheDust].mfGetName());

	sprintf(buffer,STRMGR_GetStr(STR_FMT_DEATHNOTIFY),Realmbuf);

	Y += my/2-gtext_height(buffer)/2;		//centered vertically
	
	// reload because gtext_height modifies our string
	sprintf(buffer,STRMGR_GetStr(STR_FMT_DEATHNOTIFY),Realmbuf);

	init_gfont(FONT_TITL_10PT);
	print_textf(X,Y,BLACK,buffer);

	// get position of Previous button
	if( GetButtonPosition( MenuId, 4, &X, &Y ) == fERROR)
		return;

	// get size of Prev Button
	if( GetButtonSize( MenuId, 4, &mx, &my ) == fERROR)
		return;

	X = X+mx/2;
	Y = Y+my/2-3;  //-3 to give a little room between the button and the border

	//gotta manually paint our own OK button
	SHORT hOK=GetResourceStd("UI\\Okaybtn.pcx",FALSE);
	if (hOK!=fERROR)
	{
		BITMPTR pOK=NULL;
		SetPurge(hOK);
		pOK=(BITMPTR)BLKPTR(hOK);

		DrawBitmap(X-pOK->w/2,Y-pOK->h/2,hOK,0,0,999,999);
	}

	strcpy(buffer,STRMGR_GetStr(STR_CS_STR_OK));
	print_text_centered(X,Y,buffer,WHITE);

//	OutlineAllRegions();
}

/* ========================================================================
   Function    - NotifyPlayerOfRealmDeath
   Description - Puts up a dialogue box saying that a realm has been defeated
   Returns     - void
   ======================================================================== */
static void NotifyPlayerOfRealmDeath(LONG RealmIdx)
{
	AddReport(IMMEDIATE_DEATHNOTIFY, RealmIdx, 0, 0);
	
//GEH 	PFVLL OldProc = GetButtonProc(D_MOREINFO,0);
//GEH 	LONG OldArg = GetButtonValue(D_MOREINFO,0);
//GEH 	LONG MenuCombo = BUILD_LONG((SHORT)D_MOREINFO,(SHORT)0);
//GEH 
//GEH 	if (RealmIdx == HomeRealm && IsMenuActive(D_DACTN))
//GEH 	{
//GEH 		HideMenu(D_DACTN);
//GEH 		RunMenus();
//GEH 	}
//GEH 
//GEH 	SetButtonProc(D_MOREINFO, 0, PaintDeathNotification, RealmIdx, NO_KEY);
//GEH 	SetButtonProc(D_MOREINFO, 4, HideSubMenu, D_MOREINFO, 0);
//GEH 	ShowMenu(D_MOREINFO);
//GEH 
//GEH 	SysForceCursor();
//GEH 	while(IsMenuActive(D_MOREINFO))
//GEH 		MenuLoop();
//GEH 
//GEH 	SetButtonProc(D_MOREINFO, 0, OldProc, OldArg, NO_KEY);
//GEH 	SetButtonProc(D_MOREINFO, 4, NULL, 0, 0);
}

#if 0
/* ========================================================================
   Function    - CheckRealmStatus
   Description - check for lose game due to no provinces
						fSend means tell remote players about this change 
   Returns     - 
   ======================================================================== */
void CheckRealmStatus (LONG investor, LONG victim, BOOL fSend )
{
	LONG	i, j, k, m, l, r, dx, ts;
	LONG	axProv[LAND_REALM_COUNT];
	LONG	ayProv[LAND_REALM_COUNT];
	LONG	xProv[LAND_REALM_COUNT];
	LONG	rProv[LAND_REALM_COUNT];

	sMyProvinces = 0;
	for (i=0; i<LAND_REALM_COUNT; ++i)
	{
		sProvinces[i] = 0;
		axProv[i] = 0;
		ayProv[i] = 0;
		rProv[i] = 0;
		xProv[i] = 99999;
	}

	// count provinces
	for (i=1; i<PROVINCE_COUNT; ++i)
	{
		r = province[i].Realm;
		if (r && r < LAND_REALM_COUNT)	// don't count those outside of LAND_REALM_COUNT
		{
			++sProvinces[r];
			if ((LONG)province[i].x < xProv[r]) xProv[r] = province[i].x;
			if ((LONG)province[i].x > rProv[r]) rProv[r] = province[i].x;
			axProv[r] += province[i].x;
			ayProv[r] += province[i].y;
		}
	}
	sMyProvinces = sProvinces[HomeRealm];

	for (i=1; i<LAND_REALM_COUNT; ++i)
	{
		// adjust name
		if (sProvinces[i] && (i==investor || i==victim))
		{
			realm[i].mfSetNameX(axProv[i]/sProvinces[i]);
			realm[i].mfSetNameY(ayProv[i]/sProvinces[i]);
			dx = rProv[i] - xProv[i];
			ts = ((LONG)realm[i].mfGetTitleSize() * dx) / ((LONG)realm[i].mfGetRealmWidth()*2L);
			//printf("%s - x:%d y:%d\n",realm[i].mfGetName(),realm[i].mfGetNameX(),realm[i].mfGetNameY());
			//printf("%s - dx:%d rw:%d ttlsz:%d ts:%d\n",realm[i].mfGetName(),dx,realm[i].mfGetRealmWidth(),realm[i].mfGetTitleSize(),ts);

			if (ts < 50) ts = 50;
			if (ts > 100) ts = 100;

			realm[i].mfSetTitleSize(ts);
		}


		// defeated realms
		if (sProvinces[i] == 0 && i != REALM::MONSTERS && i != REALM::MIERES && realm[i].mfExists())
		{
			realm[i].mfSetExists(FALSE);

			if (i != REALM::FIVEPEAKS && i != REALM::ANUIRE)
				NotifyPlayerOfRealmDeath(i);

			// The fact that DeleteUnit reorders the list doens't matter here,
			// because we are not following the list chains. So any troops which would
			// have been disrupted because they were earlier in the chain have already been
			// deleted.
			for (j=1; j<MAX_UNITS; ++j)			// kill units
				if (units[j].Realm == i)
					DeleteUnit(j, fSend );	

			for (l=0; l<PROVINCE_COUNT; ++l)		// kill holdings
			{
				CheckAndFixPlaces((PROVINCE)l);
				j = k = province[l].FirstPlace;
				while (k != 0 && j<MAX_PLACES)
				{
					k = places[j].NextPlace;
					/* if the holding which is about to delete is the first one in the province
					 * DeleteHolding() will change the province's FirstPlace as well as each places' NextPlace.
					 */
					m = province[l].FirstPlace + k;
		
					if (places[j].Realm == i)
					{
						if (places[j].Icon == SEAPORT_ICON
								|| places[j].Icon == SEAPORT2_ICON
								|| places[j].Icon == PORT_ICON
								|| places[j].Icon >= CASTLE1_ICON
								|| places[j].Icon == RUINS_ICON)
						{
//							places[j].Realm = province[l].Realm;
							SetGameData( MP_PLACES, MPPLACE_REALM, j, province[l].Realm, fSend );
						}
						else
						{
							DeleteHolding((PROVINCE)l, j, fSend );
						}
					}
					j = m;
				}
#ifdef _DEBUG
				if (j >= MAX_PLACES)
				{
					fatal_error("CheckRealmStatus -- illegal place");
				}
#endif
			}

		}
	}

	if (sMyProvinces == 0 && HomeRealm != REALM::MIERES)
	{
		// put up defeat screen, you have lost all your provinces, and
		// are not fit to be regent.
		DefeatScreen(PaintProvinceLose, ProvinceLooseProc);
		return;
	}

}


#else
/* ========================================================================
   Function    - AdjustNames
   Description - Support functions for CheckRealmStatus
   Returns     - 
   ======================================================================== */
#define X_CLUMP_BOUNDS		180
#define Y_CLUMP_BOUNDS		160

void AdjustName (LONG _realm)
{
	LONG	i, j, dx, dy, ts, c, bestprov, f, fAnyProvs = FALSE;
	LONG	cNearProvs[PROVINCE_COUNT];
	LONG	xProv, rProv, yProv, bProv, xSum, ySum, xCtr, yCtr, pCnt;
	char	n[50];

	if (!realm[_realm].mfExists())		// don't do non existant realms
		return;

	// count province clumps
	for (i=1; i<PROVINCE_COUNT; i++)
	{
		cNearProvs[i] = 0;

		if (province[i].Realm == _realm)
		{
			cNearProvs[i] = 1;
			fAnyProvs = TRUE;

			for (j=1; j<PROVINCE_COUNT; j++)
			{
				if (province[j].Realm == _realm && j != i)
				{
					dx = ABS( (LONG)province[i].x - (LONG)province[j].x );
					dy = ABS( (LONG)province[i].y - (LONG)province[j].y );

					if (dx < X_CLUMP_BOUNDS && dy < Y_CLUMP_BOUNDS)
						cNearProvs[i]++;
				}
			}
		}
	}

	if (!fAnyProvs)	// don't do realms with no provinces
		return;

	// find largest clump
	c = 0;
	for (i=1; i<PROVINCE_COUNT; i++)
		if (cNearProvs[i] > c)
		{
			c = cNearProvs[i];
			bestprov = i;
		}
	xSum = xProv = rProv = province[bestprov].x;
	ySum = yProv = bProv = province[bestprov].y;

	// find bounds of the clump
RestartFindBounds:
	f = FALSE;
	pCnt = 1;
	xSum = province[bestprov].x;
	ySum = province[bestprov].y;

	for (i=1; i<PROVINCE_COUNT; i++)
	{
		if (province[i].Realm == _realm
			&& province[i].x > (xProv - X_CLUMP_BOUNDS)
			&& province[i].x < (rProv + X_CLUMP_BOUNDS)
			&& province[i].y > (yProv - Y_CLUMP_BOUNDS)
			&& province[i].y < (bProv + Y_CLUMP_BOUNDS) )
		{
			if (province[i].x < xProv) f = xProv = province[i].x;
			if (province[i].x > rProv) f = rProv = province[i].x;
			if (province[i].y < yProv) f = yProv = province[i].y;
			if (province[i].y > bProv) f = bProv = province[i].y;
			pCnt++;
			xSum += province[i].x;
			ySum += province[i].y;
			if (f) goto RestartFindBounds;
		}
	}

	// adjust names
	//realm[_realm].mfSetNameX( (xProv+rProv)/2 );
	//realm[_realm].mfSetNameY( (yProv+bProv)/2 );
	realm[_realm].mfSetNameX(xSum/pCnt);
	realm[_realm].mfSetNameY(ySum/pCnt);

	dx = rProv - xProv;
	//ts = ((LONG)realm[_realm].mfGetTitleSize() * dx) / ((LONG)realm[_realm].mfGetRealmWidth()*2L);

	if (_realm == REALM::ANUIRE && pCnt == 1)
		ts = 10;
	else if (pCnt == 1)
		ts = 50;
	else
	{
		init_gfont(19);
		sprintf(n,"%s",(_realm==REALM::GORGONSCROWN)?STRMGR_GetStr(STR_GM_GORGONS_CROWN):realm[_realm].mfGetName());
		strupr(n);
		ts = (dx * 60L) / (LONG)gtext_width(n);	// use 60% of the avail width
		ts += 20;	// 10% boost

		if (ts < 50) ts = 50;
		if (ts > 100) ts = 100;
	}

	realm[_realm].mfSetTitleSize(ts);
}

/* ========================================================================
   Function    - CheckRealmStatus
   Description - check for lose game due to no provinces
						fSend means tell remote players about this change 
   Returns     - 
   ======================================================================== */
void CheckRealmStatus (LONG investor, LONG victim, BOOL fSend )
{
	LONG	i, j, k, m, l, r;

	// clear province count
	for (i=0; i<LAND_REALM_COUNT; ++i)
		sProvinces[i] = 0;

	// count provinces
	for (i=1; i<PROVINCE_COUNT; ++i)
	{
		r = province[i].Realm;
		if (r && r < LAND_REALM_COUNT)	// don't count those outside of LAND_REALM_COUNT
			++sProvinces[r];
	}
	sMyProvinces = sProvinces[HomeRealm];

	// adjust names
	if (sProvinces[investor]) AdjustName(investor);
	if (sProvinces[victim]) AdjustName(victim);

	for (i=1; i<LAND_REALM_COUNT; ++i)
	{
		// defeated realms
		if (sProvinces[i] == 0 && i != REALM::MONSTERS && i != REALM::MIERES && realm[i].mfExists())
		{
			realm[i].mfSetExists(FALSE);

			if (i != REALM::FIVEPEAKS && i != REALM::ANUIRE)
				NotifyPlayerOfRealmDeath(i);

			LONG rlm;
			for (rlm = 0; rlm < REALM::REALM_COUNT; ++rlm)
			{
				SetGameData(MP_TRIBUTE, rlm, i, 0, TRUE);
				SetGameData(MP_TRIBUTE, i, rlm, 0, TRUE);
			}

			// The fact that DeleteUnit reorders the list doens't matter here,
			// because we are not following the list chains. So any troops which would
			// have been disrupted because they were earlier in the chain have already been
			// deleted.
			for (j=1; j<MAX_UNITS; ++j)			// kill units
				if (units[j].Realm == i)
					DeleteUnit(j, fSend );	

			for (l=0; l<PROVINCE_COUNT; ++l)		// kill holdings
			{
				CheckAndFixPlaces((PROVINCE)l);
				j = k = province[l].FirstPlace;
				while (k != 0 && j<MAX_PLACES)
				{
					k = places[j].NextPlace;
					/* if the holding which is about to delete is the first one in the province
					 * DeleteHolding() will change the province's FirstPlace as well as each places' NextPlace.
					 */
					m = province[l].FirstPlace + k;
		
					if (places[j].Realm == i)
					{
						if (places[j].Icon == SEAPORT_ICON
								|| places[j].Icon == SEAPORT2_ICON
								|| places[j].Icon == PORT_ICON
								|| places[j].Icon >= CASTLE1_ICON
								|| places[j].Icon == RUINS_ICON)
						{
//							places[j].Realm = province[l].Realm;
							SetGameData( MP_PLACES, MPPLACE_REALM, j, province[l].Realm, fSend );
						}
						else
						{
							DeleteHolding((PROVINCE)l, j, fSend );
						}
					}
					j = m;
				}
#ifdef _DEBUG
				if (j >= MAX_PLACES)
				{
					fatal_error("CheckRealmStatus -- illegal place");
				}
#endif
			}

		}
	}

	if (sMyProvinces == 0 && HomeRealm != REALM::MIERES)
	{
		// put up defeat screen, you have lost all your provinces, and
		// are not fit to be regent.
		DefeatScreen(PaintProvinceLose, ProvinceLooseProc);
		return;
	}

}
#endif


/* ========================================================================
	Function	-
	Description -
	Returns	- void
	======================================================================== */
void DisplayInfo (CSTRPTR szFrame, BOOL fCentered, int iformat, ...)
{
	char		texbuffer[200];
	va_list argp;
	SHORT		iBkgnd, w, h, x;
	char		*format;

	if ((iBkgnd = GetResourceStd(szFrame, FALSE)) == fERROR)
		return;

	format = STRMGR_GetStr(iformat);

	w = ((BITMPTR)BLKPTR(iBkgnd))->w;
	h = ((BITMPTR)BLKPTR(iBkgnd))->h;
	x = (SHORT)((((640-l_map)/2)+l_map) - (w/2));

	SysHideCursor();
	DrawBitmap(x, (SHORT)(240-(h/2)), iBkgnd, 0, 0, w, h);
	va_start(argp, iformat);
	vsprintf(texbuffer,format,argp);
	if (fCentered)
		print_textf(x+(w/2), 240, DKBROWN, texbuffer);
	else
		print_textf(x+10, 240-(h/2)+4, DKBROWN, texbuffer);
	SysShowCursor();

	SetPurge(iBkgnd);
	va_end(argp);
}

/*	======================================================================== */
static LONG HoldingIcon[] = {VILLAGE_ICON,SEAPORT_ICON,SEAPORT2_ICON,PORT_ICON,
										RUINS_ICON,WIZTOWER_ICON,
										LAW1_ICON,GUILD1_ICON,TEMPLE1_ICON,SOURCE1_ICON,
										CASTLE1_ICON,CASTLE2_ICON,CASTLE3_ICON,CASTLE4_ICON};
//static CSTRPTR szHolding[] = {"Village", "Seaport", "Seaport", "River Port",
//										"Ruins", "Wizard's Tower",
//										"Law", "Guild", "Temple", "Source",
//										"Castle", "Citadel", "Stronghold", "Fortress"};

static GAME_STRING gsHolding[] = {STR_HOLDING_VILLAGE,STR_HOLDING_SEAPORT,
											 STR_HOLDING_SEAPORT,STR_HOLDING_RIVPORT,
											 STR_HOLDING_RUINS,STR_HOLDING_WIZTOW,
											 STR_HOLDING_LAW,STR_HOLDING_GUILD,
											 STR_HOLDING_TEMPLE,STR_HOLDING_SOURCE,
											 STR_HOLDING_CASTLE,STR_HOLDING_CIT,
											 STR_HOLDING_STRONG,STR_HOLDING_FORT};

static int sndHolding[] =	 {0, SND_SEAPORT_ICON1, SND_SEAPORT_ICON1, SND_SEAPORT_ICON1,
										SND_RUINS_ICON1, SND_WIZTOWER_ICON1,
										SND_LAW_ICON1, SND_GUILD_ICON1, SND_TEMPLE_ICON1, SND_SOURCE_ICON1,
										SND_CASTLE_ICON1, SND_RUINS_ICON1, SND_SOURCE_ICON1, SND_RUINS_ICON1};
LONG HoldType (LONG index)
{
	LONG	i;
	for(i=0; i<14; ++i)
		if(places[index].Icon == HoldingIcon[i])
			return i;
	return 0;
}

void DisplayInfoStr (CSTRPTR szFrame, BOOL fCentered, char* format, ...)
{
	char		texbuffer[200];
	va_list argp;
	SHORT		x1, x2;
	SHORT		iBkgnd, w, h, x;

	if ((iBkgnd = GetResourceStd(szFrame, FALSE)) == fERROR)
		return;

	w = ((BITMPTR)BLKPTR(iBkgnd))->w;
	h = ((BITMPTR)BLKPTR(iBkgnd))->h;
	x = (SHORT)((((640-l_map)/2)+l_map) - (w/2));

	SysHideCursor();
	DrawBitmap(x, (SHORT)(240-(h/2)), iBkgnd, 0, 0, w, h);
	va_start(argp, format);
	vsprintf(texbuffer,format,argp);
	if (fCentered)
		print_textf(x+(w/2), 240, DKBROWN, texbuffer);
	else
		print_textf(x+10, 240-(h/2)+4, DKBROWN, texbuffer);
	SysShowCursor();

	va_end(argp);
}

/* ========================================================================
	Function	- Draw Borders
	Description -
	Returns	- void
	======================================================================== */
void DrawBorders (LONG x, LONG y, LONG map_x, LONG map_y, LONG w, LONG h, LONG /* scale */)
{
	ULONG		i;
	LONG		xx, yy, ww;
	BITMPTR	p, pMap;
	PTR		bptr;
	PTR		dptr;
	ULONG		dptr_buffer_width;
	ULONG		coBorder;
	UBYTE		pix1, pix2, pix3, pix, prov;
	UBYTE		ff[2][480];
	BOOL		f;

	if (iLgMap == fERROR || iProvMap == fERROR)
		return;

	// heal problems with the map
	province[NO_PROVINCE].Realm = REALM::NO_COUNTRY;
	province[NO_PROVINCE2].Realm = REALM::MIERES;

	fMapAltered = TRUE;

	dptr_buffer_width = ((BITMPTR)BLKPTR(iLgMap))->w;

	//DumbAutoLockPtr<BITMPTR> const pMap(iLgMap);
	//dptr = (PTR)pMap + sizeof(BITMHDR);
	//DumbAutoLockPtr<BITMPTR> const p(iProvMap);
	//bptr = (PTR)p + sizeof(BITMHDR);

	p = (BITMPTR) BLKPTR(iProvMap);	// dummy to get iProvMap into memory
	ww = p->w;								// dummy to get iProvMap into memory
	SetQuickLock(iLgMap);
	SetQuickLock(iProvMap);

	dptr = ((PTR)BLKPTR(iLgMap)) + sizeof(BITMHDR);
	p = (BITMPTR) BLKPTR(iProvMap);
	bptr = (PTR)p + sizeof(BITMHDR);

	ww = p->w;

	for (yy=0; yy<2; ++yy)			  // clear history
		for (xx=0; xx<480; ++xx)
			ff[yy][xx] = FALSE;

	for (yy=(map_y/TWOMAPS_RATIO)+1; yy < (map_y+h-2)/TWOMAPS_RATIO; ++yy)
	{
		if ((yy&0x07)==0)
			run_timers();  // cdb 11/27	// ABC don't put in inner loops

		bptr = (PTR)p + sizeof(BITMHDR) + (map_x / TWOMAPS_RATIO) + (yy * ww);
		prov = *bptr;

		if (prov >= PROVINCE_COUNT) prov = NO_PROVINCE2;
		pix1 = (UBYTE)(province[prov].Realm);

		f = FALSE;
		for (xx=(map_x/TWOMAPS_RATIO)+1; xx < (map_x+w)/TWOMAPS_RATIO; ++xx)
		{
			pix2 = pix1;

			prov = *(bptr-ww);
			if (prov >= PROVINCE_COUNT) prov = NO_PROVINCE2;
			pix3 = (UBYTE)(province[prov].Realm);					// one up

			prov = *bptr++;
			if (prov >= PROVINCE_COUNT) prov = NO_PROVINCE2;
			pix1 = (UBYTE)(province[prov].Realm);					// one to the right

			if (pix1!=REALM::NO_COUNTRY &&
					(	 (pix1 != pix3 && pix3!=REALM::NO_COUNTRY) ||
						(pix1 != pix2 && !f&& pix2!=REALM::NO_COUNTRY) ) )		// draw dot/line ?
			{
				coBorder = (pix1==HomeRealm||pix2==HomeRealm||pix3==HomeRealm) ? MDRED : BLACK;

				// draw base dot
				i = x+(xx*TWOMAPS_RATIO)-map_x-6 + ((y+(yy*TWOMAPS_RATIO)-map_y)*dptr_buffer_width);

				if (fLgMap == SMALL)
					i = i + 2 -(dptr_buffer_width*2);

				dptr[i] = (UBYTE) coBorder;
				dptr[i+1] = (UBYTE) coBorder;
				dptr[i+dptr_buffer_width] = (UBYTE) coBorder;
				dptr[i+dptr_buffer_width+1] = (UBYTE) coBorder;

				if (fLgMap != SMALL)
				{
					dptr[i-1] = (UBYTE) coBorder;
					dptr[i+dptr_buffer_width-1] = (UBYTE) coBorder;
					dptr[i-dptr_buffer_width] = (UBYTE) coBorder;
					dptr[i+dptr_buffer_width*2] = (UBYTE) coBorder;
					dptr[i-dptr_buffer_width+1] = (UBYTE) coBorder;
					dptr[i+dptr_buffer_width*2+1] = (UBYTE) coBorder;
					dptr[i+2] = (UBYTE) coBorder;
					dptr[i+dptr_buffer_width+2] = (UBYTE) coBorder;
				}

//				if (f)										  //		- line
				if (f && fLgMap != SMALL)
				{
					dptr[i-dptr_buffer_width-1] = (UBYTE) coBorder;
					dptr[i+dptr_buffer_width*2-1] = (UBYTE) coBorder;
					dptr[i-dptr_buffer_width-2] = (UBYTE) coBorder;
					dptr[i+dptr_buffer_width*2-2] = (UBYTE) coBorder;
				}

				pix=ff[(yy-1)&1][xx];				//		| line
//				if (pix)
				if (pix && fLgMap != SMALL)
				{
					dptr[i-dptr_buffer_width-1] = (UBYTE)coBorder;
					dptr[i-dptr_buffer_width*2-1] = (UBYTE)coBorder;
					dptr[i-dptr_buffer_width+2] = (UBYTE)coBorder;
					dptr[i-dptr_buffer_width*2+2] = (UBYTE)coBorder;
				}

				if (!pix)
				{
					pix=ff[(yy-1)&1][xx-1];
					if (pix && !f)	  //		\ line
					{
						if (fLgMap == SMALL)
						{
							dptr[i-dptr_buffer_width] = (UBYTE)coBorder;
							dptr[i-1] = (UBYTE)coBorder;
						}
						else
						{
							dptr[i-dptr_buffer_width*3-1] = (UBYTE)coBorder;
							dptr[i-dptr_buffer_width*2-2] = (UBYTE)coBorder;
							dptr[i-dptr_buffer_width*2-1] = (UBYTE)coBorder;
							dptr[i-dptr_buffer_width*2] = (UBYTE)coBorder;
							dptr[i-dptr_buffer_width-3] = (UBYTE)coBorder;
							dptr[i-dptr_buffer_width-2] = (UBYTE)coBorder;
							dptr[i-dptr_buffer_width-1] = (UBYTE)coBorder;
							dptr[i-2] = (UBYTE)coBorder;
						}
					}

					pix=ff[(yy-1)&1][xx+1];
					if (pix)				//		/ line
					{
						if (fLgMap == SMALL)
						{
							dptr[i-dptr_buffer_width+1] = (UBYTE)coBorder;
							dptr[i+2] = (UBYTE)coBorder;
						}
						else
						{
							dptr[i-dptr_buffer_width*3+2] = (UBYTE)coBorder;
							dptr[i-dptr_buffer_width*2+3] = (UBYTE)coBorder;
							dptr[i-dptr_buffer_width*2+2] = (UBYTE)coBorder;
							dptr[i-dptr_buffer_width*2+1] = (UBYTE)coBorder;
							dptr[i-dptr_buffer_width+4] = (UBYTE)coBorder;
							dptr[i-dptr_buffer_width+3] = (UBYTE)coBorder;
							dptr[i-dptr_buffer_width+2] = (UBYTE)coBorder;
							dptr[i+3] = (UBYTE)coBorder;
						}
					}
				}

				ff[yy&1][xx] = pix1;
				f = TRUE;
			}

			else			// don't draw dot/line
			{
				ff[yy&1][xx] = FALSE;
				f = FALSE;
			}

		}
	}

	ClrLock(iLgMap);
	ClrLock(iProvMap);
}

/* ========================================================================
	Function	-
	Description -
	Returns	- void
	======================================================================== */
void MapLine (LONG x0, LONG y0, LONG x1, LONG y1, LONG co1, LONG co2, BOOL fWander)
{
	LONG		d,x,y,ax,ay,sx,sy,dx,dy,diag,p;
	LONG		q = 0;
	LONG		r = 0;
	LONG		s = 0;
	PTR		dptr;
	ULONG		bw;
	ULONG		anta_ix = (co2*256);
	UBYTE		pix;

	if (iLgMap == fERROR)
		return;

	fMapAltered = TRUE;

	dptr = ((PTR)BLKPTR(iLgMap)) + sizeof(BITMHDR);
	bw = ((BITMPTR)BLKPTR(iLgMap))->w;

	if (fLgMap==SMALL)
	{
		x0 /= 2;
		y0 /= 2;
		x1 /= 2;
		y1 /= 2;
	}

	dx = x1 - x0;
	dy = y1 - y0;

	ax = ABS (dx) << 1;
	ay = ABS (dy) << 1;

	sx = SGN (dx);
	sy = SGN (dy);

	x = x0;
	y = y0;

	if (ax>ay)			/* X dominant */
	{
		d = ay - (ax >> 1);
		diag = (d>=0);

		while (TRUE)
		{
//			if ((y+q)>=3 && (y+q)<478 && x<640 && x>=lmap)
			{
				p = x + ((y+q) * bw);

				if (diag)
					dptr[p-bw-bw-bw] = antia_table[(pix*256)+antia_table[anta_ix+(pix=dptr[p-bw-bw-bw])]];
				else
					dptr[p-bw-bw] = antia_table[(pix*256)+antia_table[anta_ix+(pix=dptr[p-bw-bw])]];
				dptr[p+bw+bw] = antia_table[(pix*256)+antia_table[anta_ix+(pix=dptr[p+bw+bw])]];

				if (diag)
					dptr[p-bw-bw] = antia_table[anta_ix+dptr[p-bw-bw]];
				else
					dptr[p-bw] = antia_table[anta_ix+dptr[p-bw]];
				dptr[p+bw] = antia_table[anta_ix+dptr[p+bw]];

				if (diag)
					dptr[p-bw] = (UBYTE) co1;
				dptr[p] = (UBYTE) co1;
			}

			if (x == x1)
				break;
			if (d>=0)
			{
				y += sy;
				d -= ax;
			}
			x += sx;
			d += ay;

			if (fWander)
			{
				q = r / 4;
				r = (s==r)?r:(r>s)?r-1:r+1;
				s += random(11)-5;
				s += (ABS(s)<ABS(x1-x))?0:((s>0)?-2:2);
			}
		}
	}
	else			/* Y dominant */
	{
		d = ax - (ay >> 1);
		diag = (d>=0);

		while (TRUE)
		{
//			if (y>=0 && y<480 && (x+q)<638 && (x+q)>=(lmap+2))
			{
				p = x + q + (y * bw);
				dptr[p-2-diag] = antia_table[(pix*256)+antia_table[anta_ix+(pix=dptr[p-2-diag])]];
				dptr[p+2] = antia_table[(pix*256)+antia_table[anta_ix+(pix=dptr[p+2])]];
				dptr[p-1-diag] = antia_table[anta_ix+dptr[p-1-diag]];
				dptr[p+1] = antia_table[anta_ix+dptr[p+1]];
				if (diag)
					dptr[p-1] = (UBYTE) co1;
				dptr[p] = (UBYTE) co1;
			}

			if (y == y1)
				break;
			if (d>=0)
			{
				x += sx;
				d -= ay;
			}
			y += sy;
			d += ax;

			if (fWander)
			{
				q = r / 4;
				r = (s==r)?r:(r>s)?r-1:r+1;
				s += random(11)-5;
				s += (ABS(s)<ABS(y1-y))?0:((s>0)?-2:2);
			}
		}
	}
}

/* ========================================================================
	Function	-
	Description -
	Returns	- void
	======================================================================== */
void DrawRoads (PROVINCE prov)
{
	LONG	i, j;

	run_timers();  // cdb 11/27	// ABC don't put in inner loops

	fMapAltered = TRUE;

	srand(prov);

	for (i=0; i<7; ++i)
	{
		j = province[prov].Border[i].Province;
		if ((province[prov].Border[i].MoveCost == PLAINS_ROAD || province[prov].Border[i].MoveCost == RIVER_ROAD)
				&& (province[prov].x < province[j].x
						|| (province[prov].x==province[j].x && province[prov].y < province[j].y)))
			MapLine(province[prov].x,province[prov].y,province[j].x,province[j].y,MDBROWN,MDBROWN,TRUE);
	}
}

void DrawRoads_ (PROVINCE prov, LONG i)
{
	LONG	j;

	run_timers();  // cdb 11/27	// ABC don't put in inner loops

	fMapAltered = TRUE;

	srand(prov);

	j = province[prov].Border[i].Province;
	if ((province[prov].Border[i].MoveCost == PLAINS_ROAD || province[prov].Border[i].MoveCost == RIVER_ROAD)
			&& (province[prov].x < province[j].x
					|| (province[prov].x==province[j].x && province[prov].y < province[j].y)))
		MapLine(province[prov].x,province[prov].y,province[j].x,province[j].y,MDBROWN,MDBROWN,TRUE);

}

/* ========================================================================
	Function	-
	Description -
	Returns	- void
	======================================================================== */
#if 0
void DrawLeyLines (void)
{
	LONG		i;

	for (i=0; i<MAX_PLACES; ++i)
	{
		// draw lines from Source to iRoute1,2,3
		if (places[i].Icon == SOURCE1_ICON
				&& (places[i].iRoute1+places[i].iRoute2+places[i].iRoute3) != 0
				&& places[i].Realm != REALM::NO_COUNTRY)
		{
			if (places[i].iRoute1)
				MapLine(places[i].x, places[i].y, places[places[i].iRoute1].x, places[places[i].iRoute1].y, WHITE, LTBLUE, FALSE);
			if (places[i].iRoute2)
				MapLine(places[i].x, places[i].y, places[places[i].iRoute2].x, places[places[i].iRoute2].y, WHITE, LTBLUE, FALSE);
			if (places[i].iRoute3)
				MapLine(places[i].x, places[i].y, places[places[i].iRoute3].x, places[places[i].iRoute3].y, WHITE, LTBLUE, FALSE);
		}
	}

}
#endif

/* ========================================================================
	Function	-
	Description -
	Returns	- void
	======================================================================== */
void SetRedrawMainMapLevel (void)
{
	fRedrawAll = TRUE;
}

/* ========================================================================
	Function	- HoldingToProv
	Description - determine what province a holding is in
	Returns	- province index
	======================================================================== */
PROVINCE HoldingToProv (LONG h)
{
	LONG i,j,k;
	PROVINCE p;

	// h = HOLDING INDEX;

	for (p=NO_PROVINCE, i=1; i<PROVINCE_COUNT && p == NO_PROVINCE; ++i)
	{
		CheckAndFixPlaces((PROVINCE)i);
		j = k = province[i].FirstPlace;
		while (k && p == NO_PROVINCE)
		{
			if (j == h)
				p = (PROVINCE)i;
			k = places[j].NextPlace;
	  		j = province[i].FirstPlace + k;
		}
	}

	// p = PROVINCE HOLDING IS IN
	return p;
}

/* ========================================================================
	Function	- ProvinceInfo
	Description - scan for places in province
	Returns	- void
	======================================================================== */
LONG ProvinceInfo (PROVINCE prov, MAP_ICON holding_type)
{
	LONG		f, i, j, k, t, icon, level, roll, route, mod;
	LONG		rv = -1;
	BOOL		fRegentOK = TRUE;

	fOurLawClaims = fTheirLawClaims = -1;
	prov_castle = prov_law = prov_temple = prov_guild = prov_source = -1;
	our_law_claims = their_law_claims = prov_units = prov_tax = prov_trade = prov_reg = 0;

	// regency only if invested ruler
	if (province[prov].Realm==HomeRealm && !province[prov].OccupRealm && !province[prov].Contested)
		prov_reg = province[prov].CivLevel;

	if (realm[HomeRealm].mfGetRegent() >= CHARACTER_COUNT)  // bad data
		fRegentOK = FALSE;

	if (fRegentOK)
		if (playerstats[realm[HomeRealm].mfGetRegent()] == fERROR)
			fRegentOK = FALSE;

	// count the number of units we have in this province
	for (j=1; j<MAX_UNITS; ++j)
	{
		// scan through units for leaders
		if ( units[j].Joined==0
			&& units[j].Realm == HomeRealm
			&& units[j].province == prov)
		{
			++prov_units;
			k = j;
			while (k != -1)		// count followers
			{
				++prov_units;
				k = units[k].NextUnit;
			}
		}
	}

	// check for LAW CLAIMS
	CheckAndFixPlaces((PROVINCE)prov);
	j = k = province[prov].FirstPlace;
	while (k != 0)
	{
		if (places[j].Icon == LAW1_ICON)
		{
			if (places[j].Realm == HomeRealm)
				fOurLawClaims = places[j].Level;
			else
				fTheirLawClaims = places[j].Level;
		}
		k = places[j].NextPlace;
		j = k + province[prov].FirstPlace;
	}

	// Our law claim on someone else's holding or province GIVES money
	if (fOurLawClaims > -1)
	{
		f = FALSE;
		if (province[prov].Realm != HomeRealm)
		{
			our_law_claims += province[prov].CivLevel;
			f = TRUE;
		}
		CheckAndFixPlaces((PROVINCE)prov);
		j = k = province[prov].FirstPlace;
		while (k != 0)
		{
			if ((places[j].Icon == TEMPLE1_ICON || places[j].Icon == GUILD1_ICON)
				&& places[j].Realm != HomeRealm)
			{
				f = TRUE;
				if (fOurLawClaims < places[j].Level)
					our_law_claims += places[j].Level/2;
				if (fOurLawClaims == places[j].Level)
					our_law_claims += (places[j].Level*3)/4;
				else
					our_law_claims += places[j].Level;
			}
			k = places[j].NextPlace;
			j = k + province[prov].FirstPlace;
		}
		if (!f) fOurLawClaims = -1;	// nothing to claim against
	}

	// Someone else's law claim TAKES money
	if (fTheirLawClaims > -1 && province[prov].Realm == HomeRealm)
		their_law_claims += province[prov].CivLevel;

	CheckAndFixPlaces((PROVINCE)prov);
	j = k = province[prov].FirstPlace;
	while (k != 0)
	{
		//	printf("%d - realm:%d icon:%d level:%d next:%d\n",j,places[j].Realm,places[j].Icon,places[j].Level,places[j].NextPlace);
		icon = places[j].Icon;
		level = places[j].Level;

		if (places[j].Realm == HomeRealm)
		{
			if (icon == holding_type) rv = j;		 // return index to holding
			if (icon==CASTLE1_ICON || icon==CASTLE2_ICON
				|| icon==CASTLE3_ICON || icon==CASTLE4_ICON)
			{
				prov_castle = level;
			}
			if (icon == LAW1_ICON)
			{
				prov_law = level;
				if (fRegentOK)
				{
					DumbAutoLockPtr< PLAYER_STATS > const pPlayerStats(playerstats[realm[HomeRealm].mfGetRegent()]);
					if (pPlayerStats->mfIsType(PLAYER_CLASS_INFO::FIGHTER))
						prov_reg+=level;
					else if (pPlayerStats->mfIsType(PLAYER_CLASS_INFO::PRIEST) ||
						pPlayerStats->mfIsType(PLAYER_CLASS_INFO::THIEF))
					 	prov_reg+=((level+1)/2);
				}
			}
			if (icon == TEMPLE1_ICON)
			{
				prov_temple = level;
				if (fRegentOK)
				{
					DumbAutoLockPtr< PLAYER_STATS > const pPlayerStats(playerstats[realm[HomeRealm].mfGetRegent()]);
					if (pPlayerStats->mfIsType(PLAYER_CLASS_INFO::PRIEST) ||
						pPlayerStats->mfIsType(PLAYER_CLASS_INFO::PALADIN))
						prov_reg+=level;
				}
			}
			if (icon == SOURCE1_ICON)
			{
				prov_source = level;
				if (fRegentOK)
				{
					DumbAutoLockPtr< PLAYER_STATS > const pPlayerStats(playerstats[realm[HomeRealm].mfGetRegent()]);
					if (pPlayerStats->mfIsType(PLAYER_CLASS_INFO::WIZARD))
						 prov_reg+=level;
				}
			}

			// Guild levels and Trade Route income
			if (icon == GUILD1_ICON)
			{
				prov_guild = level;
				if (fRegentOK)
				{
					DumbAutoLockPtr< PLAYER_STATS > const pPlayerStats(playerstats[realm[HomeRealm].mfGetRegent()]);
					if (pPlayerStats->mfIsType(PLAYER_CLASS_INFO::THIEF) ||
						pPlayerStats->mfIsType(PLAYER_CLASS_INFO::RANGER))
						prov_reg+=level;
					else if (pPlayerStats->mfIsType(PLAYER_CLASS_INFO::BARD))
						prov_reg+=((level+1)/2);
				}
				for (i=0; i<3; ++i)		// scan through trade routes
				{
					route = (i==0)?places[j].iRoute1:(i==1)?places[j].iRoute2:places[j].iRoute3;
					if (route)
					{
						t = ((province[prov].CivLevel + province[route].CivLevel) / 2);
						prov_trade += t;
						if (fRegentOK)
						{
							DumbAutoLockPtr< PLAYER_STATS > const pPlayerStats(playerstats[realm[HomeRealm].mfGetRegent()]);
							if (pPlayerStats->mfIsType(PLAYER_CLASS_INFO::THIEF))
								prov_reg += t;
						}
						//printf("Trade Route - place:%d  %s to %s adds %d\n",j,province[prov].name,province[route].name,(province[prov].CivLevel + province[route].CivLevel) / 2);
					}
				}
			}

			// Sea and River Trade
			if (icon == SEAPORT_ICON || icon == PORT_ICON)
			{
				prov_trade += province[prov].CivLevel;
				if (fRegentOK)
				{
					DumbAutoLockPtr< PLAYER_STATS > const pPlayerStats(playerstats[realm[HomeRealm].mfGetRegent()]);
					if (pPlayerStats->mfIsType(PLAYER_CLASS_INFO::THIEF))
						prov_reg+=province[prov].CivLevel;
				}
			}

			// Guild and Temple taxes
			if (icon == TEMPLE1_ICON || icon == GUILD1_ICON)
			{
				if (fFinalRoll >= cProvinces)
				{
					if (province_tax[cProvinces] < 0)
					{
						if(province[prov].Blessed > 0)
							roll = RollGuildTempleTaxes((SHORT)province[prov].CivLevel,(SHORT)level, mod=1);
						else if(province[prov].Blighted > 0)
							roll = RollGuildTempleTaxes((SHORT)province[prov].CivLevel,(SHORT)level, mod=-1);
						else 
							roll = RollGuildTempleTaxes((SHORT)province[prov].CivLevel,(SHORT)level, mod=0);
						prov_tax += roll;

						LOGCOMMENTSLEEP
						if ( fLogComment )
						{
							char temp[110];
							sprintf ( temp, "PROVINCE INFO - %s:%s  FINAL gld & tmpl - civ:%d hld:%d mod:%d roll:%d prov_tax:%d",
								realm[HomeRealm].mfGetName(),
								province[prov].name,
								province[prov].CivLevel,
								level,
								mod,
								roll,
								prov_tax);
							RandomLogComment ( temp );
						}

						//printf("doing rolls fFinalRoll:%d  GuildTempleTax:%ld  sum:%ld\n",fFinalRoll,roll,prov_tax);
					}
				}
				else
				{
					prov_tax += AveGuildTempleTaxes((SHORT)province[prov].CivLevel,(SHORT)level);
					#if 0
					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						char temp[150];
						sprintf ( temp, "PROVINCE INFO - %s:%s  AVE guild and temple:prov_tax:%06d",realm[HomeRealm].mfGetName(),province[prov].mfGetName(),prov_tax);
						RandomLogComment ( temp );
					}
					#endif
				}

				// Someone else's law claim TAKES money
				if (fTheirLawClaims > -1)
				{
					if (fTheirLawClaims < places[j].Level)
						their_law_claims += places[j].Level/2;
					if (fTheirLawClaims == places[j].Level)
						their_law_claims += (places[j].Level*3)/4;
					else
						their_law_claims += places[j].Level;
				}
			}
		}
		k = places[j].NextPlace;
		j = k + province[prov].FirstPlace;
	}

	// province tax
	if (	(	(province[prov].Realm==HomeRealm && !province[prov].OccupRealm)
				|| province[prov].OccupRealm == HomeRealm)
			&& !province[prov].Contested)
	{
		if (fFinalRoll >= cProvinces)
		{
			if (province_tax[cProvinces] < 0)
			{
				if (province[prov].Blessed > 0)
					roll = RollProvinceTaxes((SHORT)province[prov].CivLevel, (SHORT)province[prov].TaxLevel, mod=1);
				else if(province[prov].Blighted > 0)
					roll = RollProvinceTaxes((SHORT)province[prov].CivLevel, (SHORT)province[prov].TaxLevel, mod=-1);
				else
					roll = RollProvinceTaxes((SHORT)province[prov].CivLevel, (SHORT)province[prov].TaxLevel, mod=0);
				prov_tax += roll;

				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					char temp[150];
					sprintf ( temp, "PROVINCE INFO - %s:%s  FINAL province - civlvl:%d taxlvl:%d mod:%d roll:%d prov_tax:%d",realm[HomeRealm].mfGetName(),province[prov].name,province[prov].CivLevel,province[prov].TaxLevel,mod,roll,prov_tax);
					RandomLogComment ( temp );
				}

				province_tax[cProvinces] = (SHORT)prov_tax;
				//printf("Doing tax rolls - fFinalRoll:%d  ProvinceTax:(%d@%d) %ld  sum:%ld\n",fFinalRoll,province[prov].CivLevel,province[prov].TaxLevel,roll,prov_tax);
			}
			else
			{
				prov_tax = province_tax[cProvinces];
				#if 0
				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					char temp[150];
					sprintf ( temp, "PROVINCE INFO - %s:%s  NON-ROLLED province:prov_tax:%06d",realm[HomeRealm].mfGetName(),province[prov].name,prov_tax);
					RandomLogComment ( temp );
				}
				#endif
			}

		}
		else
		{
			province_tax[cProvinces] = -1;
			prov_tax += AveProvinceTaxes((SHORT)province[prov].CivLevel, (SHORT)province[prov].TaxLevel);
				#if 0
				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					char temp[150];
					sprintf ( temp, "PROVINCE INFO - %s:%s  AVE province:prov_tax:%06d",realm[HomeRealm].mfGetName(),province[prov].name,prov_tax);
					RandomLogComment ( temp );
				}
				#endif
		}
	}
	else		// case where we are not ruler but have holdings in prov
	{
		if (fFinalRoll >= cProvinces)
		{
			if (province_tax[cProvinces] < 0)
				province_tax[cProvinces] = (SHORT)prov_tax;
			else
			{
				prov_tax = province_tax[cProvinces];
				#if 0
				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					char temp[150];
					sprintf ( temp, "PROVINCE INFO - %s:%s  NON-ROLLED province:prov_tax:%06d",realm[HomeRealm].mfGetName(),province[prov].name,prov_tax);
					RandomLogComment ( temp );
				}
				#endif
			}

		}
		else
			province_tax[cProvinces] = -1;
	}

	our_law_claims = (our_law_claims + 5) / 10;
	their_law_claims = (their_law_claims + 5) / 10;

// printf("civ:%d tax:%d\n",province[prov].CivLevel,prov_tax);

	return rv;
}

/* ========================================================================
	Function	- Fortify Deamon
 
   This is done locally on each machine !!!! DONT PASS TO OTHERS !!!!

	Description -
						Level = iRoute2 / 8
						iRoute1 = desired level
						iRoute2 = already spent
						iRoute3 = spend this turn
	Returns	-
	======================================================================== */
void UpdatingFortify (void)
{
	LONG	i, cnt=0;

	//printf("UPDATING FORTIFY -");

	#ifdef _WINDOWS
	if ( !IsMultiPlayer() || AMultiPlayer.IsMaster() )
	#endif
	{
		for (i=0; i<MAX_PLACES; ++i)
		{
			// castle in work, CHECK TO MAKE SURE IT'S A CASTLE!!!!
			if (places[i].Icon == CASTLE1_ICON
					&& places[i].iRoute1 != 0
					&& places[i].Realm != REALM::NO_COUNTRY)
			{
				++cnt;

				// we might need less gold that was rolled
				if ((places[i].iRoute2+places[i].iRoute3) > (places[i].iRoute1*8))
				{
					places[i].iRoute3 = (places[i].iRoute1 * 8) - places[i].iRoute2;
					SetGameData(MP_PLACES, MPPLACE_IROUTE3, i, places[i].iRoute3, TRUE );
				}
				// not enough gold
				if (places[i].iRoute3 > realm[places[i].Realm].mfGetTreasury())
				{
					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						char temp[100];
						sprintf ( temp, "UpdatingFortify - not enough gold  castle:%d  %d/%d/%d\n", i,places[i].iRoute1*8,places[i].iRoute2,places[i].iRoute3);
						RandomLogComment ( temp );
					}

					continue;	// Go to the next place. (Might be another's realm.)
				}

				// deduct cost from treasury
				realm[places[i].Realm].mfAddTreasury( -places[i].iRoute3);
				SetGameData(MP_REALM, MPRLM_TREASURY, places[i].Realm, realm[places[i].Realm].mfGetTreasury(), TRUE);

				// add to total amount spent
				places[i].iRoute2 += places[i].iRoute3;
				SetGameData(MP_PLACES, MPPLACE_IROUTE2, i, places[i].iRoute2, TRUE );

				// increase one level
				if (places[i].iRoute2 >= ((places[i].Level*8)+8))
				{
					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						char temp[100];
						sprintf ( temp, "UpdatingFortify - level increased  castle:%d  %d/%d/%d",i,places[i].iRoute1*8,places[i].iRoute2,places[i].iRoute3);
						RandomLogComment ( temp );
					}

					++places[i].Level;
					SetGameData(MP_PLACES, MPPLACE_LEVEL, i, places[i].Level, TRUE );
					SetGameData(MP_PLACES, MPPLACE_IICONBITM, i, places[i].Icon+(((places[i].Level<8)?places[i].Level:7)/2), TRUE );

					// stop fortifying
					if (places[i].Level >= places[i].iRoute1)
					{
						places[i].iRoute1 = places[i].iRoute2 = places[i].iRoute3 = 0;
						SetGameData(MP_PLACES, MPPLACE_IROUTE1, i, 0, TRUE );
						SetGameData(MP_PLACES, MPPLACE_IROUTE2, i, 0, TRUE );
						SetGameData(MP_PLACES, MPPLACE_IROUTE3, i, 0, TRUE );

						#ifdef _WINDOWS
						if ( !IsMultiPlayer() )
						#endif
						{
							if(places[i].Realm == HomeRealm)
							{
								LONG m = 1;
								while(m < PROVINCE_COUNT)
								{
									if(province[m].Realm != HomeRealm)
									{	
										++m;
										continue;
									}
									CheckAndFixPlaces((PROVINCE)m);
									LONG n = province[m].FirstPlace;
									LONG j = n;
									while (n != 0)
									{
										if(j == i)
										{
											AddReport(IMMEDIATE_FORTIFY_COMPLETE, m, 0, 0);
											break;
										}
										n = places[j].NextPlace;
										j = province[m].FirstPlace + n;
									}
									if (n!= 0)
										break;
									++m;
								}
							}
						}

					}
				}
				else
				{
					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						char temp[100];
						sprintf ( temp, "UpdatingFortify - no increase yet  castle:%d  %d/%d/%d",i,places[i].iRoute1*8,places[i].iRoute2,places[i].iRoute3);
						RandomLogComment ( temp );
					}
				}

			}
		}
	}

}

/* ========================================================================
	Function	- various button service functions
	Description -
	Returns	-
	======================================================================== */
void DisplayHolding (LONG type, LONG)
{
	// GWP reqSound = SND_UI_BUTTON_CLICK;
	// The region code does the click.
	reqSound = FALSE;
	
	fRedrawAll = TRUE;

	// make holding buttons radio buttons
	//if (type != LAW1_ICON) 		show_place_icon[LAW1_ICON] = FALSE;
	//if (type != GUILD1_ICON) 	show_place_icon[GUILD1_ICON] = FALSE;
	//if (type != TEMPLE1_ICON)	show_place_icon[TEMPLE1_ICON] = FALSE;
	//if (type != SOURCE1_ICON)	show_place_icon[SOURCE1_ICON] = FALSE;

	show_place_icon[type] = !show_place_icon[type];

	//	if (type == GUILD1_ICON)
	//		show_place_icon[PORT_ICON] = !show_place_icon[PORT_ICON];

}

/*	======================================================================== */
void ZoomOut (LONG /* type */, LONG)
{
	if (map_scale!=MAX_SCALE)		 // clicked on zoom icon
	{
		desired_scale = MAX_SCALE;
		reqSound = SND_UI_MAP_ZOOM_OUT;
		iSelectedUnit = -1;
	}
}

/*	======================================================================== */
void DataAreaMode (LONG type, LONG fNotDepress)
{
	fDepressIcon = FALSE;					// make sure button is NOT depressed

	if (!fNotDepress)							 // if requested, depress it
		DepressIcon();

	if(mouse_button == 2)
	{
		if(type == TAXLVL_MODE)
			DoAction(19, 0);
		if(type == OCCUPATION_MODE)
			DoAction(25, 0);
		if(type == PAYRL_MODE)
			DoAction(21, 0);
		if(type == MAINT_MODE)
			DoAction(20, 0);
		return;
	}

	if (type == MAX_MODE)
		dturn_mode = mode_save;			 // restore old mode
	else
	{
		dturn_mode = type;
		if (type != MIN_MODE)				// save current mode
			mode_save = type;
	}

	fUpdateDataArea=TRUE;

	if (dturn_mode == ACTN_MODE)
	{
		max_scale = AMAX_SCALE;
		req_l_map = DATA_AREA_ACTN;
		if (map_scale > MAX_SCALE)
			desired_scale = MAX_SCALE;
		return;
	}

	if (l_map != DATA_AREA_MAX)
	{
		req_l_map = DATA_AREA_MAX;
#if PARCHMENT_SOUND
		if (!fNotDepress)
			reqSound = SND_UI_STATUS_REPORT;
#endif
//		ACTIVATE_REGION_(regMINIMIZED, FALSE);
//		ACTIVATE_REGION_(regMAXIMIZED, TRUE);
	}

	if (dturn_mode == MIN_MODE)
	{
		req_l_map = DATA_AREA_MIN;
#if PARCHMENT_SOUND
		if (!fNotDepress)
			reqSound = SND_UI_STATUS_REPORT;
#endif
//		ACTIVATE_REGION_(regMINIMIZED, TRUE);
//		ACTIVATE_REGION_(regMAXIMIZED, FALSE);
	}
}

/*	======================================================================== */
void TempDataAreaMode (LONG flag_addr, LONG value)
{
	static LONG save_mode_save;
	static LONG save_dturn_mode;
	SHORT * pFlag = (SHORT *)flag_addr;

	//printf("TempDataAreaMode - Flag:%d  value:%d\n",*pFlag,value);

	if(mouse_button == 2)
		return;

	fDepressIcon = FALSE;				// make sure button is NOT depressed

	*pFlag = !(*pFlag);
	fRedrawAll = TRUE;
	// GWP reqSound = SND_UI_BUTTON_CLICK;
	// The region code does the click.
	reqSound = FALSE;

	if (*pFlag)
	{
		if (value)
			*pFlag = (SHORT) value;
		activate_region(regDOMAIN_TURN_MODE	, FALSE);
		activate_region(regACTION_MODE	  , FALSE);
//		activate_region(regMINIMIZED			, FALSE);
//		activate_region(regMAXIMIZED			, FALSE);
		activate_region(regTAX_ROLLS			, FALSE);
		activate_region(regSWITCH_TO_ACTIONS, FALSE);
		activate_region(regADJUST_TAXES	 , FALSE);
		save_mode_save = mode_save;
		save_dturn_mode = dturn_mode;
//		DataAreaMode(MIN_MODE, 1);
		DataAreaMode(MAX_MODE, 1);
	}
	else
	{
		RESTORE_REGION_STATE(regDOMAIN_TURN_MODE	);
		RESTORE_REGION_STATE(regACTION_MODE			);
//		RESTORE_REGION_STATE(regMINIMIZED			 );
//		RESTORE_REGION_STATE(regMAXIMIZED			 );
		RESTORE_REGION_STATE(regTAX_ROLLS			 );
		RESTORE_REGION_STATE(regSWITCH_TO_ACTIONS	 );
		RESTORE_REGION_STATE(regADJUST_TAXES		);
		DataAreaMode(save_dturn_mode, 1);
		mode_save = save_mode_save;
	}
}

/*	======================================================================== */
void TempDataAreaModeSet (LONG flag_addr, LONG value)
{
	SHORT * pFlag = (SHORT *)flag_addr;

	*pFlag = FALSE;					 // set to FALSE then toggle to get TRUE
	TempDataAreaMode (flag_addr,value);
}

/*	======================================================================== */
void TempDataAreaModeOff (LONG flag_addr, LONG)
{
	SHORT * pFlag = (SHORT *)flag_addr;

	*pFlag = TRUE;						  // set to TRUE then toggle to get FALSE
	TempDataAreaMode (flag_addr,0);
}

/*	======================================================================== */
void ToggleShowUnits (LONG i, LONG fAll)
{
	LONG j;
	if (i>0)		// do one
	{
		realm[i].mfSetShowUnits(!realm[i].mfShowUnits());
	}
	else			// do many
	{
		for (j=-i; j<(LAND_REALM_COUNT-i); ++j)
			realm[j].mfSetShowUnits(fAll);

		if (i==0)		 // first column
		{
			fUnitsAllOn = fUnitsAllOn & 0xFE;
			if (fAll)
				fUnitsAllOn += 1;
		}
		else				// second column
		{
			fUnitsAllOn = fUnitsAllOn & 0xFD;
			if (fAll)
				fUnitsAllOn += 2;
		}
	}

	// GWP reqSound = SND_UI_BUTTON_CLICK;
	// The region code does the click.
	reqSound = FALSE;
	fRedrawAll = TRUE;
}

/*	======================================================================== */
void ToggleShowHoldings (LONG i, LONG fAll)
{
	LONG j, start, end;

	if (i>0)			// do one
	{
		realm[i].mfSetShowHoldings(!realm[i].mfShowHoldings());
	}

	else				// do many
	{
		start = -i;
		end = (start>0) ? REALM::REALM_COUNT : LAND_REALM_COUNT;
		for (j = start; j < end; ++j)
			realm[j].mfSetShowHoldings(fAll);

		if (i==0)		 // first column
		{
			fHoldingsAllOn = fHoldingsAllOn & 0xFE;
			if (fAll)
				fHoldingsAllOn += 1;
		}
		else				// second column
		{
			fHoldingsAllOn = fHoldingsAllOn & 0xFD;
			if (fAll)
				fHoldingsAllOn += 2;
		}
	}

	// GWP reqSound = SND_UI_BUTTON_CLICK;
	// The region code does the click.
	reqSound = FALSE;
	fRedrawAll = TRUE;
}

/*	======================================================================== */
void DoTaxRolls (LONG /* type */, LONG)
{
#if 0
	// save the current state of the game
	{
		FILE *fp;
		char *cur_sav = "tutorial.sav";
 		if (Exists(cur_sav))
			remove (cur_sav);
		fp = fopen(cur_sav, "wb");
		SaveGameFiles(fp);
		fclose(fp);
	}
#endif

	if(mouse_button == 2)
	{
		DoAction(23, 0);
		return;
	}

	if (fControlMode != 0)
		DepressIcon();

	fDoActionIcon = TRUE;
	fUpdateDataArea = TRUE;
	fDontAdvance = FALSE;
	DataAreaMode(MAINT_MODE, 0);
	dtrn_ic_active[0] = FALSE;		// adjust taxes lowlight
	ACTIVATE_REGION_(regTAX_ROLLS				, FALSE);	 // tax rolls OFF
	//ACTIVATE_REGION_(regSWITCH_TO_ACTIONS	, TRUE);		// switch to actions ON
	ACTIVATE_REGION_(regADJUST_TAXES			, FALSE);	 // adjust taxes OFF

	del_region(SwitchToActionMode,0);
	add_region(105,426,48,46,0,SwitchToActionMode,0,0,regSWITCH_TO_ACTIONS, STR_GM_TT_SELECT_ACTION);
	ACTIVATE_REGION_(regSWITCH_TO_ACTIONS	, TRUE);		// switch to actions ON
}

/*	========================================================================
   Function    - CancelGoBankrupt
   Description - 
   Returns     -
   ======================================================================== */
void CancelGoBankrupt (LONG, LONG)
{
	HideRequest(D_MOREINFO);
	RunRequests();
}

/*	========================================================================
   Function    - OkGoBankrupt
   Description - 
   Returns     -
   ======================================================================== */
void OkGoBankrupt (LONG, LONG)
{
	LONG	i, j;

	for (i=0; i<LAND_REALM_COUNT && new_treasury < 0; ++i)
	{
		if ( Tribute[HomeRealm][i] )
		{
			new_treasury += (Tribute[HomeRealm][i] * 10);

			SetGameData(MP_TRIBUTE, HomeRealm, i, 0, TRUE);
			j = RealmReaction[i][HomeRealm] - (Tribute[HomeRealm][i] * 5);
			if (j < 0) j = 0;

			if (RealmStatus[i][HomeRealm] == STATUS_PERMISSIVE_ALLIANCE && j < 40)
			{
				j = 45;
				RealmStatus[i][HomeRealm] = STATUS_NEUTRAL;
				RealmStatus[HomeRealm][i] = STATUS_NEUTRAL;
				AddReport(IMMEDIATE_ALLIANCE_DEG, i, 0, 0);
			}
			if (RealmStatus[i][HomeRealm] == STATUS_FULL_ALLIANCE && j < 40)
			{
				j = 45;
				RealmStatus[i][HomeRealm] = STATUS_PERMISSIVE_ALLIANCE;
				RealmStatus[HomeRealm][i] = STATUS_PERMISSIVE_ALLIANCE;
				AddReport(IMMEDIATE_ALLIANCE_DEG, i, 0, 0);
			}
			if (RealmStatus[i][HomeRealm] == STATUS_VASSAL && j < 40)
			{
				j = 45;
				RealmStatus[i][HomeRealm] = STATUS_NEUTRAL;
				RealmStatus[HomeRealm][i] = STATUS_NEUTRAL;
				AddReport(IMMEDIATE_VASSAL_LEAVE, i, 0, 0);
			}

			SetGameData(MP_REALMREACTION, i, HomeRealm, j, TRUE);
		}
	}

	HideRequest(D_MOREINFO);
	RunRequests();
}

/* ========================================================================
   Function    - PaintGoBankrupt
   Description - 
   Returns     -
   ======================================================================== */
void PaintGoBankrupt (LONG MenuCombo, LONG)
{
	LONG	X,Y;
	LONG	W,H;
	SHORT MenuId,ButtonId;
	LONG	startY;

	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	del_region(OkGoBankrupt, 0);
	del_region(CancelGoBankrupt, 0);

	// get position of text area
	if( GetButtonPosition( MenuId, 0, &X, &Y ) == fERROR)
		return;

	// get size of menu
	if( GetButtonSize( MenuId, 0, &W, &H ) == fERROR)
		return;

	init_gfont(FONT_SANS_8PT);

	startY = Y;
	Y += 20;
	X += 20;

	print_textf(X,Y, DKBROWN, STRMGR_GetStr(STR_GMMAP_NO_GOLD) );

	// draw buttons
	init_gfont(FONT_SANS_10PT);
	Y = startY + H - 35;

	SHORT iButton = GetResourceStd ("UI\\REGBTN2A.PCX", FALSE);

	DrawBitmap (X+90-15, (SHORT)Y, iButton, 0, 0, 999, 999);
	print_textf(X+90+34-15, Y+14, WHITE, "^F02^c%s", STRMGR_GetStr(STR_BACK));
	add_region(X+90, Y, 68, 28, 0, CancelGoBankrupt, 0, 0, 0, -1);

	DrawBitmap (X+184, (SHORT)Y, iButton, 0, 0, 999, 999);
	if (payroll == 0)
	{
		print_textf(X+184+34, Y+14, WHITE, "^F02^c%s", STRMGR_GetStr(STR_ACCEPT));
		add_region(X+184, Y, 68, 28, 0, OkGoBankrupt, 0, 0, 0, -1);
	}
	else
		print_textf(X+184+34, Y+14, GREY, "^F02^c%s", STRMGR_GetStr(STR_ACCEPT));

	if (iButton != fERROR)
		SetPurge(iButton);
}

/* ========================================================================
   Function    - SwitchToActionMode
   Description - 
   Returns     -
   ======================================================================== */
void SwitchToActionMode (LONG, LONG)
{
	LONG			i, j, k, l, p;
	static BOOL	fDMdone = FALSE;

	if(mouse_button == 2)
	{
		DoAction(24, 0);
		return;
	}

	// don't go on until all rolls done or in Basic control mode
	if (fFinalRoll < cProvinces && fControlMode != 0)
		return;

	if (new_treasury < 0)
	{
		reqSound = SND_UI_NOT_ENOUGH_GOLD;

		// do the dialog box
		SetButtonProc (D_MOREINFO, 0, PaintGoBankrupt, 0, 0);
		SetButtonProc (D_MOREINFO, 4, NULL, 0, 0);
		ShowRequest(D_MOREINFO, 0);
		return;
	}

	if (realm_reg < 0)			// either is invalid
	{
		reqSound = SND_UI_NOT_ENOUGH_REGENCY;
		return;
	}

	del_region(SwitchToActionMode,0);

#ifdef _WINDOWS
	//----  SYNC POINT #1 
	if ( IsMultiPlayer() )
	{
		StartSync( iMPS_SYNC1 );
		CheckSync( iMPS_SYNC1, TRUE, TRUE );
	}
#endif

	// reduce provinces and holdings here
	for (i=1; i<PROVINCE_COUNT; ++i)
	{
		if (province[i].ReduceHolds)
		{
			CheckAndFixPlaces((PROVINCE)i);
			j = k = province[i].FirstPlace;
			while (k != 0)
			{
				if (places[j].Realm != HomeRealm
					&& !AREALLIED(HomeRealm,places[j].Realm)
					&& (places[j].Icon == LAW1_ICON
						|| places[j].Icon == GUILD1_ICON
						|| places[j].Icon == TEMPLE1_ICON) )
				{
					//places[j].Level = 0;
					SHORT holdingtype = -1;
					switch(places[j].Icon)
					{
						case LAW1_ICON:		holdingtype = 0; break;
						case GUILD1_ICON:	holdingtype = 1; break;
						case TEMPLE1_ICON:	holdingtype = 2; break;
						default: break;
					}
					if (holdingtype >= 0)
						SendReportTo(places[j].Realm, IMMEDIATE_REDUCEHOLDING, province[i].Realm, holdingtype, i);
					SetGameData( MP_PLACES, MPPLACE_LEVEL, j, 0, TRUE );
					SetScrewed((REALM::REALM_TYPE)places[j].Realm, HomeRealm, TRUE);
				}
				k = places[j].NextPlace;
				j = k + province[i].FirstPlace;
			}
		}

		if (province[i].Pillaged && province[i].CivLevel > 0)
		{
			//province[i].CivLevel--;
			SetGameData( MP_PROVINCE, MPPROV_CIVLEVEL, i, (province[i].CivLevel-1), FALSE);
		}

		if (province[i].ReduceHolds || province[i].Pillaged)
			AdjustHoldingLevel((PROVINCE)i, FALSE);

		province[i].ReduceHolds = FALSE;			// don't send
		province[i].Pillaged = FALSE;				// don't send
	}

	// ------------------------------------------
	// we want to prevent the player from overflowing the 16-bit treasury.
	// ------------------------------------------
	//while (realm[HomeRealm].mfGetTreasury() >= 50000)
	while (new_treasury >= 50000)
	{
		SHORT x = random(100);
		LONG amount;
		REPORT_SUBJECTS lossevent;
		if (date%4 == 3)
		{
			SHORT realm;
			
			lossevent = IMMEDIATE_PARTY;
			amount = random(4)+1;				// 1d4 kgb
			for (realm=1; realm < LAND_REALM_COUNT; ++realm)
			{
				if (realm == HomeRealm)
					continue;
					
				if (realm == REALM::GORGONSCROWN ||
					realm == REALM::RHUOBHE ||
					realm == REALM::SPIDERFELL ||
					realm == REALM::FIVEPEAKS ||
					realm == REALM::CHIMAERON ||
					realm == REALM::MURKILAD ||
					realm == REALM::MARKAZOR ||
					realm == REALM::THURAZOR ||
					realm == REALM::MONSTERS)
					continue;
					
				// some people might have appreciated the party
				// (but probably not the exceptions listed above)
				for (SHORT j=random(amount+2)-1; j > 0; --j)
					SetHelped((REALM::REALM_TYPE)realm, HomeRealm, TRUE);
			}
		}
		else if (x < 60)
		{
			lossevent = IMMEDIATE_THEFT;
			amount = 1;							// 1 kgb
		}
		else
		{
			lossevent = IMMEDIATE_EMBEZZLE;
			amount = 2;							// 2 kgb
		}
		
		AddReport(lossevent, amount, 0, 0);
		//realm[HomeRealm].mfAddTreasury( -( amount * 10000));
		new_treasury -= amount * 10000;
	}

	// ------------------------------------------
	// finalize this turn's starting resources
	// ------------------------------------------
	if (fControlMode != 0)
	{
		realm[HomeRealm].mfSetTreasury(new_treasury);
		realm[HomeRealm].mfSetRegency(realm_reg);
	}
	SetGameData(MP_REALM, MPRLM_TREASURY, HomeRealm, realm[HomeRealm].mfGetTreasury(), TRUE);
	SetGameData(MP_REALM, MPRLM_REGENCY, HomeRealm, realm[HomeRealm].mfGetRegency(), TRUE);

	// disband unpaid units
	for (i=1; i<MAX_UNITS; ++i)
	{
		if (units[i].Realm == HomeRealm && (UnitMaintCost[units[i].Icon] || units[i].Salary)
			&& units[i].Use_Regency == FALSE && units[i].Paid == FALSE)
		{
			// test for mercs here
			if (units[i].Icon >= M_INF_UNIT_ICON && units[i].Icon <= M_CAV_UNIT_ICON)
			{
				// merc leader
				if (units[i].Joined == FALSE)
				{
					k = units[i].NextUnit;

					// if we HAVE followers
					if (k != -1)
					{
						units[k].province = units[i].province;
						units[k].Realm = units[i].Realm;
						units[k].Joined = 0;
						#ifdef _WINDOWS
						if (IsMultiPlayer())
							AMultiPlayer.UnitMessage(k, (UBYTE *)&units[k], "MAKE LEADER");
						#endif
						CheckLeader(k, TRUE );					// put best leader on top
					}

					units[i].Realm = REALM::MONSTERS;
					units[i].NextUnit = -1;
					#ifdef _WINDOWS
					if (IsMultiPlayer())
						AMultiPlayer.UnitMessage(i, (UBYTE *)&units[i], "DISBAND TO BANDITS");
					#endif
				}

				else		// unit is a follower
				{
					l = i;
					do
					{
						for (k=1; k<MAX_UNITS; ++k)		// scan for previous unit
						{
							if (units[k].NextUnit == l)
							{
								l = k;
								break;
							}
						}
						if (k >= MAX_UNITS)
							goto NoLeader;
					} while (units[l].Joined);
					p = units[l].province;		// leader's province

					for (k=1; k<MAX_UNITS; ++k)		// scan for previous unit
					{
						if (units[k].NextUnit == i)
						{
							units[k].NextUnit = units[i].NextUnit;
							SetGameData(MP_UNITS, MPUNITS_NEXTUNIT, k, units[i].NextUnit, TRUE );

							units[i].province = p;		// leader's province
							units[i].Joined = 0;
							units[i].Realm = REALM::MONSTERS;
							units[i].NextUnit = -1;
							#ifdef _WINDOWS
							if (IsMultiPlayer())
								AMultiPlayer.UnitMessage(i, (UBYTE *)&units[i], "DISBAND TO BANDITS2");
							#endif
							break;
						}
					}
				}	// merc leader vs. follower
			}

			else	// not a merc
			{
NoLeader:
				DeleteUnit(i, TRUE );				//---- Send to remote players
			}

// found to be annoying
//			AddReport(SUBJ_DISBAND_UNIT, i, units[i].province, 0);
		}
	}

	// hot seat multiplayer
#ifdef _WINDOWS
	if (!IsMultiPlayer())
#endif
	{
		// DM controlled
		if (fDMdone == FALSE)		// do a complete pass for DM controlled first
		{
			i = HomeRealm;
			while (iHotSeatMultiplayerIndex < LAND_REALM_COUNT)
			{
				if (realm[iHotSeatMultiplayerIndex].mfIsDMCtrl()
					&& iHotSeatMultiplayerIndex != HomeRealm)
				{
					HomeRealm = (REALM::REALM_TYPE)iHotSeatMultiplayerIndex;
					ActiveRegent = realm[HomeRealm].mfGetRegent();
					if (i != HomeRealm)
					{
						UpdateHoldingFlag();
						ReconstructMap();
					}
					dturn_mode = REPEAT_REPORT_DONE_MODE;
					dtrn_ic_active[0] = TRUE;		 // adjust taxes hilight
					fFinalRoll = FALSE;
					fDoActionIcon = FALSE;
					ACTIVATE_REGION_(regTAX_ROLLS			, TRUE);		// tax rolls ON
					ACTIVATE_REGION_(regSWITCH_TO_ACTIONS,FALSE);	// switch to actions OFF
					ACTIVATE_REGION_(regADJUST_TAXES		, TRUE);		// adjust taxes ON
					del_region(SwitchToActionMode,0);
					fUpdateDataArea = TRUE;
					fDontAdvance = TRUE;
					iHotSeatMultiplayerIndex++;
					return;
				}
				iHotSeatMultiplayerIndex++;
			}
			iHotSeatMultiplayerIndex = 1;
			fDMdone = TRUE;	// done DM controlled realms
		}

		// next do all player controlled
		i = HomeRealm;
		while (iHotSeatMultiplayerIndex < LAND_REALM_COUNT)
		{
			if (realm[iHotSeatMultiplayerIndex].mfIsPlayerCtrl()
				&& !realm[iHotSeatMultiplayerIndex].mfIsDMCtrl()
				&& iHotSeatMultiplayerIndex != HomeRealm )
			{
				HomeRealm = (REALM::REALM_TYPE)iHotSeatMultiplayerIndex;
				ActiveRegent = realm[HomeRealm].mfGetRegent();
				if (i != HomeRealm)
				{
					UpdateHoldingFlag();
					ReconstructMap();
				}
				dturn_mode = REPEAT_REPORT_DONE_MODE;
				dtrn_ic_active[0] = TRUE;		 // adjust taxes hilight
				fFinalRoll = FALSE;
				fDoActionIcon = FALSE;
				ACTIVATE_REGION_(regTAX_ROLLS			, TRUE);		// tax rolls ON
				ACTIVATE_REGION_(regSWITCH_TO_ACTIONS,FALSE);	// switch to actions OFF
				ACTIVATE_REGION_(regADJUST_TAXES		, TRUE);		// adjust taxes ON
				del_region(SwitchToActionMode,0);
				fUpdateDataArea = TRUE;
				fDontAdvance = TRUE;
				iHotSeatMultiplayerIndex++;
				return;
			}
			iHotSeatMultiplayerIndex++;
		}
		fDMdone = FALSE;		// reset for DM controlled pass next time

		i = HomeRealm;
		iHotSeatMultiplayerIndex = 1;
		// first, try for a DM controlled
		while (!realm[iHotSeatMultiplayerIndex].mfIsDMCtrl() && iHotSeatMultiplayerIndex < LAND_REALM_COUNT)
			iHotSeatMultiplayerIndex++;
		// if not successful, try for player controlled
		if (iHotSeatMultiplayerIndex == LAND_REALM_COUNT)
		{
			iHotSeatMultiplayerIndex = 1;
			while (!realm[iHotSeatMultiplayerIndex].mfIsPlayerCtrl() && iHotSeatMultiplayerIndex < LAND_REALM_COUNT)
				iHotSeatMultiplayerIndex++;
		}
		// lastly, set for AUTOPLAY
		if (iHotSeatMultiplayerIndex == LAND_REALM_COUNT)
			iHotSeatMultiplayerIndex = REALM::MIERES;
		HomeRealm = (REALM::REALM_TYPE)iHotSeatMultiplayerIndex;
		ActiveRegent = realm[HomeRealm].mfGetRegent();
		if (i != HomeRealm)
		{
			UpdateHoldingFlag();
			ReconstructMap();
		}
	}

	// put up action icons, switch to action mode
	fFinalRoll = FALSE;
	fDoActionIcon = FALSE;
	dturn_mode = ACTN_MODE;
	max_scale = AMAX_SCALE;
	req_l_map = DATA_AREA_ACTN;
#if PARCHMENT_SOUND
	reqSound = SND_UI_STATUS_REPORT;
#endif
	fLTAction = BEFORE_LTACTION;
	if (map_scale > MAX_SCALE)
		desired_scale = MAX_SCALE;
	dtrn_ic_active[0] = TRUE;		 // adjust taxes hilight
	fStartOfActionMode = TRUE;

	// make a copy of the reaction table to show change in
	// reports after the domain turn
	CopyReactionTable();

	ACTIVATE_REGION_(regDOMAIN_TURN_MODE, FALSE);	// domain turn mode OFF
	ACTIVATE_REGION_(regACTION_MODE	 , TRUE);		// action mode ON
//	ACTIVATE_REGION_(regMINIMIZED			, FALSE);	 // minimized OFF
//	ACTIVATE_REGION_(regMAXIMIZED			, FALSE);	 // maximized OFF
	ACTIVATE_REGION_(regTAX_ROLLS			, FALSE);		// tax rolls OFF
	ACTIVATE_REGION_(regSWITCH_TO_ACTIONS,FALSE);	// switch to actions OFF
	ACTIVATE_REGION_(regADJUST_TAXES		, FALSE);		// adjust taxes OFF

#ifdef _WINDOWS
	//---- Yes even more multiplayer ( SYNC POINT #0 )
	if ( IsMultiPlayer() )
	{
		//ShowSyncWaitWindow( iMPS_SYNC0, 0 );
		StartSync( iMPS_SYNC0 );
		CheckSync( iMPS_SYNC0, TRUE, TRUE );
	}
#endif

}

/*	======================================================================== */
void DoReport (LONG /* type */, LONG)
{
	DepressIcon();

	if(date < 2)
	{
		AddReport(SUBJ_INITIAL, 0, 0, 0);
		ClearOldReports();
		iNewSite = iOldSite;
	}

	report_done_dturn_mode = REPEAT_REPORT_DONE_MODE;
	GiveReport(1);
}

/*	======================================================================== */
void AdjTaxLevel (LONG prov, LONG level)
{
	if (province[prov].TaxLevel != level)
	{
		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
			char temp[100];
			sprintf ( temp, "gamemap.cpp AdjTaxLevel - prov:%ld  level:%ld",
				prov, level);
			RandomLogComment ( temp );
		}

		SetGameData(MP_PROVINCE, MPPROV_TAXLEVEL, prov, level, TRUE );
		fUpdateDataArea=TRUE;
	}
}

/*	======================================================================== */
void AdjCastleSupport (LONG prov, LONG)
{
	province[prov].CastleSptd = !province[prov].CastleSptd;
	fUpdateDataArea=TRUE;
}

/*	======================================================================== */
void AdjLawClaims (LONG prov, LONG)
{
	province[prov].LawClaims = !province[prov].LawClaims;
	fUpdateDataArea=TRUE;
}

/*	======================================================================== */
void AdjPillaged (LONG prov, LONG)
{
	province[prov].Pillaged = !province[prov].Pillaged;
	fUpdateDataArea=TRUE;
}

/*	======================================================================== */
void AdjOccupyOwn (LONG prov, LONG)
{
	if (province[prov].OccupRealm==HomeRealm)
	{
		province[prov].OccupRealm = 0;
		cOccupiedProv--;
	}
	else
	{
		province[prov].OccupRealm = HomeRealm;
		++cOccupiedProv;
	}

	SetGameData(MP_PROVINCE, MPPROV_OCCUPREALM, prov, province[prov].OccupRealm, TRUE);
	fUpdateDataArea=TRUE;
}

/*	======================================================================== */
void AdjReduceHolds (LONG prov, LONG)
{
	province[prov].ReduceHolds = !province[prov].ReduceHolds;
	fUpdateDataArea=TRUE;
}

/*	======================================================================== */

//void PayOccupiedProv (LONG prov, LONG)
//{
//	province[prov].CastleSptd = !province[prov].CastleSptd;
//	fUpdateDataArea=TRUE;
//}

/*	======================================================================== */
void AddBloodlineStrength (LONG MenuCombo, LONG button)
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
	
	SetButtonLabel  (MenuId, QUESTION_TEXT, -1, BLACK );
	if(button == QUESTION_BTN1)
	{
		SHORT regentidx = realm[HomeRealm].mfGetRegent();
		
		realm[HomeRealm].mfAddRegency(- (regents[realm[HomeRealm].mfGetRegent()].mfGetBL_strength() + 1));
		SetGameData(MP_REALM, MPRLM_REGENCY, HomeRealm, realm[HomeRealm].mfGetRegency(), TRUE);
		regents[regentidx].mfIncrBL_strength();
		SetGameData(MP_REGENT, MPREG_BL_STRENGTH, regentidx, regents[regentidx].mfGetBL_strength(), TRUE);
		fUpdateDataArea=TRUE;
	}
}

/*	======================================================================== */
void QueryBloodlineStrength (LONG, LONG)
{
	// test to see if we are about to get too high
	if (regents[realm[HomeRealm].mfGetRegent()].mfGetBL_strength() >= 99)
		return;
	
	if (realm[HomeRealm].mfGetRegency() <= regents[realm[HomeRealm].mfGetRegent()].mfGetBL_strength() + 1)
	    return;

	SetButtonProc   (D_QUESTION2, 0, NULL, 0, NO_KEY );
	
	// set the question text
	print_global[0] = regents[realm[HomeRealm].mfGetRegent()].mfGetBL_strength() + 1;
	SetButtonLabel  (D_QUESTION2, QUESTION_TEXT, STR_GM_BLOODLINE_ADD, BLACK );
	
	// turn on the first button as YES
	SetButtonLabel  (D_QUESTION2, QUESTION_BTN1, STR_YES1, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION2, QUESTION_BTN1, AddBloodlineStrength, QUESTION_BTN1, D_KEY_YES );
	
	// turn on the last button as CANCEL
	SetButtonLabel  (D_QUESTION2, QUESTION_BTN2, STR_CANCEL1, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION2, QUESTION_BTN2, AddBloodlineStrength, QUESTION_BTN2, D_KEY_CANCEL );
	
	ShowMenu(D_QUESTION2);
}
/*	======================================================================== */
void AdjCourt(LONG dir, LONG)
{
    realm[HomeRealm].mfAddCourt(dir);
	SetGameData(MP_REALM, MPRLM_COURT, HomeRealm, realm[HomeRealm].mfGetCourt(), TRUE);
	fUpdateDataArea=TRUE;
}

/*	======================================================================== */
void AdjUnitPay (LONG unit, LONG type)
{
	if (type == 0)		// troops pay gold
	{
		units[unit].Paid = !units[unit].Paid;
		if (units[unit].Paid)
			units[unit].Use_Regency = 0;
	}
	if (type == 1)		// troops use regency
	{
		units[unit].Use_Regency = !units[unit].Use_Regency;
		if (units[unit].Use_Regency)
			units[unit].Paid = 0;
	}

	if (type == 2)		// non-landed regents pay gold
	{
		units[unit].Paid = 1;
		units[unit].Use_Regency = 0;
	}
	if (type == 3)		// non-landed regents use regency
	{
		units[unit].Paid = 0;
		units[unit].Use_Regency = 1;
	}

	fUpdateDataArea=TRUE;
}

/*	======================================================================== */
void UpdateActionHeader (SHORT iAction)
{
	BOOL	f = (fControlMode == 0);

	SysHideCursor();
	//iDTurnBk = GetResourceStd("UI\\DTURN_BK.PCX", FALSE);

	DrawBitmap (0, 28, iDTurnBk, (SHORT)(DATA_AREA_MAX-l_map), 28, (SHORT)l_map, (f)?77:35);
	SetRemapTable(HomeRealm);
	DrawBitmap (5, 28, iDShldIc, 0, 27, 48, 21);
	ClearRemapTable();

	crease(8, 26, 97, 1);			// seperator
	if (f)
	{
		crease(8, 85, 97, 1);			// seperator
		crease(8, 105, 97, 1);			// seperator
//		print_textf(7, 32, DKBROWN, STRMGR_GetStr(STR_GM_FMTD_TURN));
//		print_textf(8, 33, DKBROWN, STRMGR_GetStr(STR_GM_FMTD_TURN_FMT),date+1);
		print_textf(8, 45, DKBROWN, STRMGR_GetStr(STR_GM_REGENCY),realm[HomeRealm].mfGetRegency());
		print_textf(8, 57, DKBROWN, STRMGR_GetStr(STR_GM_TREASURY),realm[HomeRealm].mfGetTreasury());
	}
	else
	{
		print_textf(7, 31, DKBROWN, STRMGR_GetStr(STR_GM_FMTD_REGENCY));
		print_textf(8, 32, DKBROWN, STRMGR_GetStr(STR_GM_FMTD_REGENCY_FMT),realm[HomeRealm].mfGetRegency());
		print_textf(8, 43, DKBROWN, STRMGR_GetStr(STR_GM_TREASURY),realm[HomeRealm].mfGetTreasury());
	}

	if (fLTAction == DURING_LTACTION)
		print_textf(55, (f)?97:58, DKPURPLE, STRMGR_GetStr(STR_GM_C_LACTION));
	else if (iAction == -1)
	{
		char buffer[100];
//		strcpy(buffer,STRMGR_GetStr(gsOrdinalNum[global_action_turn]));
//		print_textf(55, (f)?97:58, ActionColor[global_action_turn], STRMGR_GetStr(STR_GM_SELECT_ACTION),buffer);

		sprintf(buffer, STRMGR_GetStr(STR_GM_SELECT_ACTION), date+1, global_action_turn);
		print_textf(55, (f)?97:58, ActionColor[global_action_turn], buffer);
	}
	else if (iAction == BUILD_ROAD || iAction == LIEUTENANT_ACTION || iAction == MUSTER)		 // muster, lieutenant action, build
		print_textf(55, (f)?97:58, DKPURPLE, STRMGR_GetStr(STR_GM_FREE_ACTION));
	else
	{
		char buffer[100];
		strcpy(buffer,STRMGR_GetStr(gsOrdinalCap[global_action_turn]));
		print_textf(55, (f)?97:58, ActionColor[global_action_turn], STRMGR_GetStr(STR_GM_ACTION2),buffer);
	}

	SysShowCursor();
}

/* ========================================================================
	Function	- DoAction
	Description - process a domain action icon selection
	Returns	- void
	======================================================================== */
void DoAction (LONG iAction, LONG)
{
	// see if right click
	if(mouse_button == 2)
	{
		char buffer[80];
		
		strcpy(buffer, Action[iAction].mfGetTitle());
		
		SystemHelp(
			buffer,
			Action[iAction].mfGetVerbose(),
			Action[iAction].mfGetMoreInfo(),
			NULL, 0);
		// stay here until we clear the message box
		if (sMenusUp > 0)
		{
			SHORT sHelpMenuCount = sMenusUp;
			while( sHelpMenuCount <= sMenusUp )
			{
				MenuLoop();
			}
		}
	}

	else if (HomeRealm == CurrentRealm
//			&&	!fInfoOnlyDomainUI			[abc] 9/3
			&& dact_ic_active[iAction])
	{
		DepressIcon();
		UpdateActionHeader((SHORT)iAction);

		if (fTutorialSelected)
		{
			++TutorialScreenNumber;
			fUpdateDataArea=TRUE;
		}

		// handle adventure action differently
		if (iAction == ADVENTURE)
		{
		#if defined(_JUNEDEMO)
			// show teaser screen here	
			ShowAdScreen(0);
			IncActionTurn();
		#else
			#ifdef _WINDOWS
			//---- if this is multiplayer then we need to determine if everyone wants to go
			if ( IsMultiPlayer() )
			{
			   	ShowVoteWaitWindow(0,0);  //---- Make it an election for multi play	
			}
			else
			#endif
			{
				SelectAdvParty();
				//if (fLTAction == DURING_LTACTION)	// return to normal action type
				//{
				//	ActiveRegent = realm[HomeRealm].mfGetRegent();
				//	fLTAction = AFTER_LTACTION;
				//}
			}
		#endif
		}
		else if (iAction == HOLD_ACTION)
		{
			ShowMenu(D_HOLDACTION);
		}
		// all other actions bring up a menu, paint code is in ACTNMENU.CPP
		else
		{
			Menus[D_DACTN].Buttons[0].Arg = iAction;	// set argument to menu
			ShowMenu(D_DACTN);								// do menu
		}

	}

	// handle disallowed actions
	else if (!dact_ic_active[iAction])
		reqSound = SND_UI_NOT_PERMITTED;

}

/* ========================================================================
	Function	-
	Description -
	Returns	- void
	======================================================================== */
char * UnitName (SHORT iUnit)
{
	SHORT				i;
	static SHORT	t = 0;
	static char		n[2][30];

	t = !t;
	sprintf(n[t], "%s", STRMGR_GetStr(gsUnitTitle[units[iUnit].Icon]));
	if (units[iUnit].Icon >= FIRST_CHARACTER_TYPE)
		for (i=1; i<CHARACTER_COUNT; ++i)
			if (regents[i].mfGetunit() == iUnit)
			{
				sprintf(n[t], "%s", regents[i].mfGetname());
				break;
			}

	return n[t];
}

/* ========================================================================
	Function	-
	Description - roll dice for fortifying
					fortify deamon:  roll dice for all in-work castles here
					Level = iRoute2/8
					iRoute1 = desired level
					iRoute2 = already spent
					iRoute3 = spend this turn <-- result of die roll goes here
	Returns	- void
	======================================================================== */
void RollDiceForFortify()
{
	LONG i;

	for (i=0; i<MAX_PLACES; ++i)
	{
		if (places[i].Icon == CASTLE1_ICON			// castle in work
				&& places[i].iRoute1 != 0
				&& places[i].Realm != REALM::NO_COUNTRY)
		{
			if ( ForeignUnitsHere((PROVINCE)HoldingToProv(i), (REALM::REALM_TYPE)places[i].Realm) )
				places[i].iRoute3 = 0;
			else
				places[i].iRoute3 = random(6)+1;
		}
	}
}

/* ========================================================================
	Function	- UpdateHoldingFlag
	Description - Mark any prov that has a holding of ours in it.
					Reuses CastleSptd.
	Returns	- void
	======================================================================== */
void UpdateHoldingFlag (void)
{
	LONG					i, j, k, l, p, q;

	for (i=1; i< PROVINCE_COUNT; ++i)
	{
		if (province[i].Realm != HomeRealm)
		{
			province[i].CastleSptd = FALSE;

			CheckAndFixPlaces((PROVINCE)i);
			j = k = province[i].FirstPlace;
			while (k != 0 && !province[i].CastleSptd)
			{
				// set flag for provs we have a holding in
				if (places[j].Realm == HomeRealm)
				{
					province[i].CastleSptd = TRUE;
					//printf("HOLDS - province: %s\n",province[i].name);
				}

				k = places[j].NextPlace;
				j = province[i].FirstPlace + k;
			}
		}
	}
}

/* ========================================================================
	Function	- 
	Description -
	Returns	- void
	======================================================================== */
void DrawShield (LONG x, LONG y, REALM::REALM_TYPE rlm)
{
	SetRemapTable (rlm%LAND_REALM_COUNT);
	DrawBitmap ((SHORT)x, (SHORT)y, (rlm < LAND_REALM_COUNT)?iDCtrlIc:iDCtr2Ic, 0, 0, 999, 999);
	ClearRemapTable();
}

/* ========================================================================
	Function	- CastleBesieged
	Description -
	Returns	- void
	======================================================================== */
BOOL CastleBesieged (PROVINCE iProv, LONG iPlace)
{
	LONG	i;
	REALM::REALM_TYPE	rlm = (REALM::REALM_TYPE)places[iPlace].Realm;
	LONG	cUnits = 0;
	LONG	rv = FALSE;

	if (	places[iPlace].Icon == CASTLE1_ICON
		||	places[iPlace].Icon == CASTLE2_ICON
		||	places[iPlace].Icon == CASTLE3_ICON
		||	places[iPlace].Icon == CASTLE4_ICON )
	{
		for (i=0; i<LAND_REALM_COUNT; ++i)
			if (!AREALLIED((REALM::REALM_TYPE)i,rlm))
				cUnits += UnitsHere(iProv, (REALM::REALM_TYPE)i);

		rv = (cUnits && cUnits >= places[iPlace].Level);

//		if (rv)
//		{
//			 printf("CASTLE BESIEGED - %s in %s by %d units\n",province[iProv].name,realm[province[iProv].Realm].mfGetName(),cUnits);
//			places[iPlace].Besieged = TRUE;	// not used by anything so why set?
//		}

	}

	return rv;
}

/* ========================================================================
	Function	- CheckEnemyCastle
	Description - test for other people's castles
	Returns	- 
	======================================================================== */
BOOL CheckEnemyCastle (LONG iProv, LONG rlm)
{
	LONG	j, k;

	j = k = province[iProv].FirstPlace;
	while (k != 0)
	{
		if (	(	places[j].Icon == CASTLE1_ICON	// normal castle
				||	places[j].Icon == CASTLE2_ICON	// awnsheigh1 castle
				||	places[j].Icon == CASTLE3_ICON	// awnsheigh2 castle
				||	places[j].Icon == CASTLE4_ICON	// awnsheigh3 castle
				)
			//&& places[j].Realm != rlm )		// abc 8/15 
			&& !AREALLIED(places[j].Realm, rlm) )
		{
			return TRUE;
		}

		k = places[j].NextPlace;
		j = province[iProv].FirstPlace + k;
	}

	return FALSE;
}

/* ========================================================================
	Function	- DegradeCastles
	Description -
	Returns	- void
	======================================================================== */
void DegradeCastles (void)
{
	LONG	i, j, k;

	for (i=0; i< PROVINCE_COUNT; ++i)
		CheckAndFixPlaces((PROVINCE)i);

	// our castles
	for (i=0; i< PROVINCE_COUNT; ++i)
	{
		if (province[i].Realm == HomeRealm && !province[i].CastleSptd)
		{
			j = k = province[i].FirstPlace;
			while (k != 0)
			{
				k = places[j].NextPlace;
				// if the holding which is about to delete is the first one in the province
				// DeleteHolding() will change the province's FirstPlace as well as each places' NextPlace.
				LONG const m = province[i].FirstPlace + k;
				
				if ( places[j].Icon == CASTLE1_ICON
					&& places[j].Realm == HomeRealm
					&& places[j].iRoute1 == 0			// not currently fortifying it
					&& !CastleBesieged((PROVINCE)i,j) )
				{
					if (places[j].Level > 0)
						places[j].Level--;

					//printf("CASTLE DEGRADE - %s in %s, new level %d\n",province[i].name,realm[province[i].Realm].mfGetName(),places[j].Level);
					SetGameData(MP_PLACES, MPPLACE_LEVEL, j, places[j].Level, TRUE);
					SetGameData(MP_PLACES, MPPLACE_IICONBITM, j, places[j].Icon+(((places[j].Level<8)?places[j].Level:7)/2), TRUE);

					// kill zero level castle
					if (places[j].Level < 1)
						DeleteHolding((PROVINCE)i, j, TRUE );
				}
				j = m;
			}
		}
	}

	// other people's castles
	for (i=0; i< PROVINCE_COUNT; ++i)
	{
		j = k = province[i].FirstPlace;
		while (k != 0)
		{
			k = places[j].NextPlace;
			// if the holding which is about to delete is the first one in the province
			// DeleteHolding() will change the province's FirstPlace as well as each places' NextPlace.
			LONG const m = province[i].FirstPlace + k;
			
			if (	(	places[j].Icon == CASTLE1_ICON	// normal castle
					||	places[j].Icon == CASTLE2_ICON	// awnsheigh1 castle
					||	places[j].Icon == CASTLE3_ICON	// awnsheigh2 castle
					||	places[j].Icon == CASTLE4_ICON	// awnsheigh3 castle
					)
				//&& places[j].iRoute1 == 0			// not currently fortifying it
				&& CastleBesieged((PROVINCE)i,j) )
			{
				if( places[j].Level > 0)
					places[j].Level--;

				if (places[j].iRoute1 != 0 )			// currently fortifying it
				{
					if (places[j].iRoute2 > 8)
						places[j].iRoute2 -= 8;			// take away a level worth of spent gold
					else
						places[j].iRoute2 = 0;
				}

				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					char temp[100];
					sprintf ( temp, "CASTLE DEGRADE - %s in %s, new level:%d iRoute2:%ld\n",province[i].name,realm[province[i].Realm].mfGetName(),places[j].Level,places[j].iRoute2);
					RandomLogComment ( temp );
				}

				SetGameData(MP_PLACES, MPPLACE_LEVEL, j, places[j].Level, FALSE);
				if (places[j].Icon == CASTLE1_ICON)
				{
					SetGameData(MP_PLACES, MPPLACE_IICONBITM, j, places[j].Icon+(((places[j].Level<8)?places[j].Level:7)/2), FALSE);
				}

				// kill zero level castle
				if (places[j].Level < 1)
					DeleteHolding((PROVINCE)i, j, FALSE );
			}
			j = m;
		}
	}


}

/* ========================================================================
	Function	- DoRandomEvents
	Description -
	Returns	- void
	======================================================================== */
static void DoRandomEvent (LONG select, PROVINCE prov)
{
	LONG	i, k, l, r;
	REALM::REALM_TYPE	m_rlm = REALM::MONSTERS;
	BOOL	fSend = FALSE;

	if (prov >= GORGE && prov <= TORAIN && realm[REALM::FIVEPEAKS].mfExists())
		m_rlm = REALM::FIVEPEAKS;
	else if (prov >= ABATTOIR && prov <= ZAPTIG && realm[REALM::GORGONSCROWN].mfExists())
		m_rlm = REALM::GORGONSCROWN;
	else if (prov == SPIDERFELL && realm[REALM::SPIDERFELL].mfExists())
		m_rlm = REALM::SPIDERFELL;
	else if (prov == RHUOBHE && realm[REALM::RHUOBHE].mfExists())
		m_rlm = REALM::RHUOBHE;

	switch (select)
	{
		case 0:		printf("Lots of gold given to Gorgon\n");
			if ( !realm[REALM::GORGONSCROWN].mfIsPlayerCtrl() )
			{
				realm[REALM::GORGONSCROWN].mfAddTreasury((200*date)+1000);
				SetGameData(MP_REALM, MPRLM_TREASURY, REALM::GORGONSCROWN, realm[REALM::GORGONSCROWN].mfGetTreasury(), fSend );
				realm[REALM::MARKAZOR].mfAddTreasury((150*date)+500);
				SetGameData(MP_REALM, MPRLM_TREASURY, REALM::MARKAZOR, realm[REALM::MARKAZOR].mfGetTreasury(), fSend );
				realm[REALM::MURKILAD].mfAddTreasury((70*date)+300);
				SetGameData(MP_REALM, MPRLM_TREASURY, REALM::MURKILAD, realm[REALM::MURKILAD].mfGetTreasury(), fSend );
				realm[REALM::GORGONSCROWN].mfAddSurplusIncome(1000);
//				SetGameData(MP_REALM, MPRLM_SURPLUSINCOME, REALM::GORGONSCROWN, realm[REALM::GORGONSCROWN].mfGetSurplusIncome(), fSend );
				realm[REALM::MARKAZOR].mfAddSurplusIncome(500);
//				SetGameData(MP_REALM, MPRLM_SURPLUSINCOME, REALM::MARKAZOR, realm[REALM::MARKAZOR].mfGetSurplusIncome(), fSend );
				realm[REALM::MURKILAD].mfAddSurplusIncome(300);
//				SetGameData(MP_REALM, MPRLM_SURPLUSINCOME, REALM::MURKILAD, realm[REALM::MURKILAD].mfGetSurplusIncome(), fSend );
			}
			break;
		case 1:		printf("Gold given to Gorgon\n");
			if ( !realm[REALM::GORGONSCROWN].mfIsPlayerCtrl() )
			{
				realm[REALM::GORGONSCROWN].mfAddTreasury(1000);
				SetGameData(MP_REALM, MPRLM_TREASURY, REALM::GORGONSCROWN, realm[REALM::GORGONSCROWN].mfGetTreasury(), fSend );
				realm[REALM::MARKAZOR].mfAddTreasury(500);
				SetGameData(MP_REALM, MPRLM_TREASURY, REALM::MARKAZOR, realm[REALM::MARKAZOR].mfGetTreasury(), fSend );
				realm[REALM::MURKILAD].mfAddTreasury (300);
				SetGameData(MP_REALM, MPRLM_TREASURY, REALM::MURKILAD, realm[REALM::MURKILAD].mfGetTreasury(), fSend );
			}
			break;
		case 2:		printf("Gold given to Rhuobhe\n");
			if ( !realm[REALM::RHUOBHE].mfIsPlayerCtrl() )
			{
				realm[REALM::RHUOBHE].mfAddTreasury(2000);
				SetGameData(MP_REALM, MPRLM_TREASURY, REALM::RHUOBHE, realm[REALM::RHUOBHE].mfGetTreasury(), fSend );
			}
			break;
		case 3:		printf("Gold given to the Spider\n");
			if ( !realm[REALM::SPIDERFELL].mfIsPlayerCtrl() )
			{
				realm[REALM::SPIDERFELL].mfAddTreasury(2000);
				SetGameData(MP_REALM, MPRLM_TREASURY, REALM::SPIDERFELL, realm[REALM::SPIDERFELL].mfGetTreasury(), fSend );
			}
			break;
		case 4:		printf("Magical Event: Blessed in %s in %s\n",province[prov].name,realm[province[prov].Realm].mfGetName());
			province[prov].Blessed = 3;
			SetGameData(MP_PROVINCE, MPPROV_BLESSED, prov, 3, fSend );
			AddReportTo(province[prov].Realm, IMMEDIATE_BLESS_LAND, prov, 0, 0);
			break;
		case 5:		printf("Magical Event: Blighted in %s in %s\n",province[prov].name,realm[province[prov].Realm].mfGetName());
			province[prov].Blighted = 3;
			SetGameData(MP_PROVINCE, MPPROV_BLIGHTED, prov, 3, fSend );
			AddReportTo(province[prov].Realm, IMMEDIATE_BLIGHT, prov, 0, 0);
			break;
		case 6:		printf("Magical Event: Warded in %s in %s\n",province[prov].name,realm[province[prov].Realm].mfGetName());
			province[prov].Warded = 3;
			SetGameData(MP_PROVINCE, MPPROV_WARDED, prov, 3, fSend );
			SetGameData(MP_PROVINCE, MPPROV_DISPELLVL, prov, REALM::MONSTERS, fSend );
			SetGameData(MP_PROVINCE, MPPROV_DISPEL, prov, 0, fSend );
			AddReportTo(province[prov].Realm, IMMEDIATE_WARDING, prov, 0, 0);
			break;
		case 7:		printf("Magical Event: Honest in %s in %s\n",province[prov].name,realm[province[prov].Realm].mfGetName());
			province[prov].Honest = 3;
			SetGameData(MP_PROVINCE, MPPROV_HONEST, prov, 3, fSend );
			AddReportTo(province[prov].Realm, IMMEDIATE_HONEST, prov, 0, 0);
			break;
		case 8:		printf("Magical Event: Dispel in %s in %s\n",province[prov].name,realm[province[prov].Realm].mfGetName());
			province[prov].Dispel = 3;
			province[prov].DispelLvl = random(16+4);
			SetGameData(MP_PROVINCE, MPPROV_DISPEL, prov, 3, fSend );
			SetGameData(MP_PROVINCE, MPPROV_DISPELLVL, prov, province[prov].DispelLvl, fSend );
			SetGameData(MP_PROVINCE, MPPROV_WARDED, prov, 0, fSend );
			AddReportTo(province[prov].Realm, IMMEDIATE_DISPEL_REALM_MAGIC, prov, 0, 0);
			break;
		case 9:
			if (cntUnits_() < maxNPCUnits())
			{
				printf("Monsters: Goblin Inf in %s in %s\n",province[prov].name,realm[province[prov].Realm].mfGetName());
				k = CreateUnit (prov, G_INF_UNIT_ICON, 0, m_rlm, fSend );
				if (k!=fERROR)
				{
					if (province[prov].Realm == HomeRealm)
		 				AddReport(IMMEDIATE_NPC_MOVE_TROOP, m_rlm, prov, 0);
					// Don't send the Spotted flag!
					//SetGameData(MP_UNITS, MPUNITS_NOTSPOTTED, k, FALSE, fSend );
					units[k].NotSpotted = FALSE;
				}
			}
			break;
		case 10:
			if (cntUnits_() < maxNPCUnits())
			{
				printf("Monsters: Goblin Cav in %s in %s\n",province[prov].name,realm[province[prov].Realm].mfGetName());
				k = CreateUnit (prov, G_CAV_UNIT_ICON, 0, m_rlm, fSend );
				if (k!=fERROR)
				{
					if (province[prov].Realm == HomeRealm)
		 				AddReport(IMMEDIATE_NPC_MOVE_TROOP, m_rlm, prov, 0);
					// Don't send the Spotted flag!
					//SetGameData(MP_UNITS, MPUNITS_NOTSPOTTED, k, FALSE, fSend );
					units[k].NotSpotted = FALSE;
				}
			}
			break;
		case 11:
			if (cntUnits_() < maxNPCUnits())
			{
				printf("Monsters: Gnoll Irr in %s in %s\n",province[prov].name,realm[province[prov].Realm].mfGetName());
				k = CreateUnit (prov, N_IRR_UNIT_ICON, 0, m_rlm, fSend );
				if (k!=fERROR)
				{
					if (province[prov].Realm == HomeRealm)
		 				AddReport(IMMEDIATE_NPC_MOVE_TROOP, m_rlm, prov, 0);
					// Don't send the Spotted flag!
					//SetGameData(MP_UNITS, MPUNITS_NOTSPOTTED, k, FALSE, fSend );
					units[k].NotSpotted = FALSE;
				}
			}
			break;
		case 12:
			if (cntUnits_() < maxNPCUnits())
			{
				printf("Monsters: Skeletons in %s in %s\n",province[prov].name,realm[province[prov].Realm].mfGetName());
				k = CreateUnit (prov, C_SKL_UNIT_ICON, 0, m_rlm, fSend );
				if (k!=fERROR)
				{
					if (province[prov].Realm == HomeRealm)
		 				AddReport(IMMEDIATE_NPC_MOVE_TROOP, m_rlm, prov, 0);
					// Don't send the Spotted flag!
					//SetGameData(MP_UNITS, MPUNITS_NOTSPOTTED, k, FALSE, fSend );
					units[k].NotSpotted = FALSE;
				}
			}
			break;
		case 13:
			if (cntUnits_() < maxNPCUnits())
			{
				printf("Monsters: Spiders in %s in %s\n",province[prov].name,realm[province[prov].Realm].mfGetName());
				k = CreateUnit (prov, C_SPD_UNIT_ICON, 0, m_rlm, fSend );
				if (k!=fERROR)
				{
					if (province[prov].Realm == HomeRealm)
		 				AddReport(IMMEDIATE_NPC_MOVE_TROOP, m_rlm, prov, 0);
					// Don't send the Spotted flag!
					//SetGameData(MP_UNITS, MPUNITS_NOTSPOTTED, k, FALSE, fSend );
					units[k].NotSpotted = FALSE;
				}
			}
			break;
		case 14:
			if (cntUnits_() < maxNPCUnits())
			{
				printf("Bandits: Merc Irr in %s in %s\n",province[prov].name,realm[province[prov].Realm].mfGetName());
				k = CreateUnit (prov, M_IRR_UNIT_ICON, 0, m_rlm, fSend );
				if (k!=fERROR)
				{
					if (province[prov].Realm == HomeRealm)
		 				AddReport(IMMEDIATE_NPC_MOVE_TROOP, m_rlm, prov, 0);
					// Don't send the Spotted flag!
					//SetGameData(MP_UNITS, MPUNITS_NOTSPOTTED, k, FALSE, fSend );
					units[k].NotSpotted = FALSE;
				}
			}
			break;
		case 15:
			if (cntUnits_() < maxNPCUnits())
			{
				printf("Bandits: Merc Inf in %s in %s\n",province[prov].name,realm[province[prov].Realm].mfGetName());
				k = CreateUnit (prov, M_INF_UNIT_ICON, 0, m_rlm, fSend );
				if (k!=fERROR)
				{
					if (province[prov].Realm == HomeRealm)
		 				AddReport(IMMEDIATE_NPC_MOVE_TROOP, m_rlm, prov, 0);
					// Don't send the Spotted flag!
					//SetGameData(MP_UNITS, MPUNITS_NOTSPOTTED, k, FALSE, fSend );
					units[k].NotSpotted = FALSE;
				}
			}
			break;
		case 16:
			if (cntUnits_() < maxNPCUnits())
			{
				printf("Bandits: Merc Cav in %s in %s\n",province[prov].name,realm[province[prov].Realm].mfGetName());
				k = CreateUnit (prov, M_CAV_UNIT_ICON, 0, m_rlm, fSend );
				if (k!=fERROR)
				{
					if (province[prov].Realm == HomeRealm)
		 				AddReport(IMMEDIATE_NPC_MOVE_TROOP, m_rlm, prov, 0);
					// Don't send the Spotted flag!
					//SetGameData(MP_UNITS, MPUNITS_NOTSPOTTED, k, FALSE, fSend );
					units[k].NotSpotted = FALSE;
				}
			}
			break;
		case 17:				// rebellion
			r = province[prov].Realm;
			l = province[prov].Loyalty;
			for (k=1; k<PROVINCE_COUNT; ++k)
				if (province[k].Realm==r && province[k].Loyalty && province[k].Loyalty < l)
				{
					prov = (PROVINCE)k;
					l = province[k].Loyalty;
				}
			province[prov].Loyalty = 0;
			SetGameData(MP_PROVINCE, MPPROV_LOYALTY, prov, 0, fSend );
			printf("Rebellion in %s in %s\n",province[prov].name,realm[r].mfGetName());
			AddReportTo(province[prov].Realm, IMMEDIATE_REBELLION, prov, 0, 0);
			break;
	}
}

/* ========================================================================
	Function	- 
	Description -
	Returns	- void
	======================================================================== */
void RandomEvents (void)
{
	LONG	i, j, k, l, r, maxScore, rm;
	BOOL	fSend = FALSE;

	if (fTutorialSelected)			// boost the player's gold
	{
		realm[REALM::DHOESONE].mfAddTreasury (210);

		// let us get the Eyeless One easier
		if (RealmReaction[REALM::WIZARD_EYELESS_ONE][REALM::DHOESONE] < 95)
			RealmReaction[REALM::WIZARD_EYELESS_ONE][REALM::DHOESONE] = 95;

		if (date==5)					// in turn 6, put a blight in Nolien
		{
			province[NOLIEN].Blighted = 2;
			province[RIVERFORD].Loyalty = 3;
		}
	}
	else if (fControlMode < 2)		// in basic and advanced control modes
	{
		if (fControlMode == 0)		// boost the player's gold in basic mode
			realm[HomeRealm].mfAddTreasury (110);
	}

	// ------------------------------------------
	// effect of treasury modifying items
	// note: this is a real stupid place to have this and should be moved
	// to somewhere else after the patch1 code is out GEH
	// ------------------------------------------
	if (playerstats[realm[HomeRealm].mfGetRegent()] != fERROR)
	{
		DumbAutoLockPtr< PLAYER_STATS > const pPS(playerstats[realm[HomeRealm].mfGetRegent()]);
		// GEH 8/18/97
		// if money multiplier is set, as in the Coffer of the Realm,
		// add 5% plus 1d20 to the treasury
		if ( pPS->StatsMod.mfGetMod(STATSMOD::MONEYMULT_MOD) )
		{
			new_treasury = ((105 * new_treasury ) / 100) + ((random(20) + 1) * 10);
			realm[HomeRealm].mfSetTreasury(((105 * realm[HomeRealm].mfGetTreasury() ) / 100) + (random(20) + 1) * 10);
		}
	}
	
	if (date < 2 || fTutorialSelected)
		return;		// no random events the first turn

	//----- Change the seed
#ifdef _WINDOWS
	AMultiPlayer.NextSeed(__FILE__,__LINE__);
#endif

	// select some random events
	// IMPORTANT: player MUST always be the LAST event
	for (j=0, i=random(5)+2; i>=0; i--)
	{
		j = 0;

		// randomly directed events
#ifdef _WINDOWS
		if (i || IsMultiPlayer())
#else
		if (i)
#endif
			j = random(PROVINCE_COUNT-1)+1;

		else					// 1 event directed at the player except in multiplayer
		{
			fSend = TRUE;
			for (l=0,k=1; k<PROVINCE_COUNT; ++k)	// count our provinces
				if (province[k].Realm == HomeRealm)
					++l;
			k = random(MAX(l, 3));			// if only 1 or 2 provs the player may not get an event
			if (k >= l)
				continue;
			for (j=1; j<PROVINCE_COUNT; ++j)		// scan for it
				if (province[j].Realm == HomeRealm && k<=0)
					break;
				else
					k--;
		}

		// only allow first nine random event types early in the game
		rm = (	(game_difficulty == DIF_EASY && date < 4)
				|| (game_difficulty == DIF_MEDIUM && date < 3)
				|| (game_difficulty == DIF_HARD && date < 2)	) ? 9 : 18;
		r = random(rm);

		DoRandomEvent(r,(PROVINCE)j);
	}


	//----- Change the seed JUST IN CASE WE ARE OUT OF SYNC ALREADY OTHERWISE 
	//      A couple of people will get the plot and the others will not 

#ifdef _WINDOWS
	AMultiPlayer.NextSeed(__FILE__,__LINE__);
#endif

	// ------------------------------------------
	// Gorgon's Plot
	// ------------------------------------------

	// first Gorgon flic
	if (date >= 3 && iGorgonPlot==0
		&& !realm[REALM::GORGONSCROWN].mfIsPlayerCtrl() )
	{
		PlayGorgon(1,0);
		iGorgonAlly = 0;
		++iGorgonPlot;
	}

	// second Gorgon flic and choose ally
	else if (iGorgonPlot==1 && date >= 8 && random(4)==0
		&& !realm[REALM::GORGONSCROWN].mfIsPlayerCtrl() )
	{
		PlayGorgon(2,0);
		iGorgonAlly = 0;
		maxScore = 0;
		for (i=1; i<LAND_REALM_COUNT; ++i)
		{
			if (realm[i].mfExists() && !realm[i].mfIsPlayerCtrl()
				//&& (sScores[i] + (sScores[i]*TestAdjacent(HomeRealm,(REALM::REALM_TYPE)i)*15)/10 + (sScores[i]*AREALLIED(HomeRealm,i)*15)/10 ) > maxScore
				&& i!=REALM::ANUIRE
				&& i!=REALM::CHIMAERON && i!=REALM::GORGONSCROWN
				&& i!=REALM::MARKAZOR  && i!=REALM::MONSTERS
				&& i!=REALM::MURKILAD  && i!=REALM::RHUOBHE
				&& i!=REALM::SPIDERFELL )
			{
				BOOL TestScore;
	#ifdef _WINDOWS
				if ( IsMultiPlayer() )
				{
					// Random the score by +/- 20%
					TestScore = ((sScores[i] * 8) + random(sScores[i] * 4))/10 > maxScore;
				}
				else
				{
					TestScore = (sScores[i] + (sScores[i]*TestAdjacent(HomeRealm,(REALM::REALM_TYPE)i)*15)/10 + (sScores[i]*AREALLIED(HomeRealm,i)*15)/10 ) > maxScore;
				}
	#else
				TestScore = (sScores[i] + (sScores[i]*TestAdjacent(HomeRealm,(REALM::REALM_TYPE)i)*15)/10 + (sScores[i]*AREALLIED(HomeRealm,i)*15)/10 ) > maxScore;
	#endif
				if (TestScore)
				{
					iGorgonAlly = i;
					maxScore = sScores[i];
				}
			}
		}
		++iGorgonPlot;
	}

	// third Gorgon flic and ally is exposed
	else if (iGorgonPlot==2 && date >= 16 && random(4)==0
		&& !realm[REALM::GORGONSCROWN].mfIsPlayerCtrl() )
	{
		PlayGorgon(3,0);
		realm[REALM::GORGONSCROWN].mfAddTreasury(400);
		SetGameData(MP_REALM, MPRLM_TREASURY, REALM::GORGONSCROWN, realm[REALM::GORGONSCROWN].mfGetTreasury(), FALSE );
		realm[REALM::MARKAZOR].mfAddTreasury(200);
		SetGameData(MP_REALM, MPRLM_TREASURY, REALM::MARKAZOR, realm[REALM::MARKAZOR].mfGetTreasury(), FALSE );
		realm[REALM::MURKILAD].mfAddTreasury(100);
		SetGameData(MP_REALM, MPRLM_TREASURY, REALM::MURKILAD, realm[REALM::MURKILAD].mfGetTreasury(), FALSE );
		for (i=1; i<PROVINCE_COUNT; ++i)		// xfer provs
			if (province[i].Realm == iGorgonAlly)
				province[i].Realm = REALM::GORGONSCROWN;
		for (i=1; i<MAX_UNITS; ++i)			// xfer units
			if (units[i].Realm == iGorgonAlly)
				units[i].Realm = REALM::GORGONSCROWN;
		for (i=0; i<MAX_PLACES; ++i)			// xfer holdings
			if (places[i].Realm == iGorgonAlly)
				places[i].Realm = REALM::GORGONSCROWN;
		++iGorgonPlot;
	}
//printf("Gorgon's Plot is:%d, Gorgon's Ally is :%s\n", iGorgonPlot, (iGorgonAlly)?realm[iGorgonAlly].mfGetName():"pending");

	// ------------------------------------------
	// just for laughs, lets give the Gorgon's ally lots of money
	// ------------------------------------------
	if (iGorgonPlot==2 && iGorgonAlly)
	{
		realm[iGorgonAlly].mfAddTreasury(100);
		SetGameData(MP_REALM, MPRLM_TREASURY, iGorgonAlly, realm[iGorgonAlly].mfGetTreasury(), FALSE );
		realm[iGorgonAlly].mfAddRegency(50);
		SetGameData(MP_REALM, MPRLM_REGENCY, iGorgonAlly, realm[iGorgonAlly].mfGetRegency(), FALSE );
	}

	// ------------------------------------------
	// just for the hell of it, lets give the Gorgon 5-10/5-15/5-20 units per turn
	// ------------------------------------------
	if (date > 3
		&& !realm[REALM::GORGONSCROWN].mfIsPlayerCtrl() )
	{
		PROVINCE	prov = (PROVINCE)(random(17)+ABATTOIR);
		rm = (game_difficulty == DIF_EASY)?5:((game_difficulty == DIF_MEDIUM)?10:15);

		if (random(3))
			for (i=random(rm)+5; i>=0; i--)
				DoRandomEvent(12, prov);				// skeletons
		else
			for (i=random(rm)+5; i>=0; i--)
				DoRandomEvent(random(8)+9, prov);	// random monster

		// give 5-10/5-15/5-20 skeletons directly to the gorgon where ever he is
		prov = (PROVINCE) units[regents[THE_GORGON].mfGetunit()].province;
		for (i=random(rm)+5; i>=0; i--)
			DoRandomEvent(12, prov);					// skeletons
	}

	// ------------------------------------------
	// just for chuckles, give FivePeaks, Spiderfell, and Rhuobhe some troops
	// ------------------------------------------
	if (date > 4)
	{
		rm = (game_difficulty == DIF_EASY)?3:((game_difficulty == DIF_MEDIUM)?4:5);

		// Five Peaks, 0-4 stacks of 4-6 monsters
		if ( !realm[REALM::FIVEPEAKS].mfIsPlayerCtrl() )
		{
			j = random(rm);
			for (; j>=0; j--)
			{
				PROVINCE	prov = (PROVINCE)(random(7)+GORGE);
				for (i=random(3)+4; i>=0; i--)
					DoRandomEvent(random(8)+9, prov);
			}
		}

		// Spiderfell, 4-6 monsters
		if ( !realm[REALM::SPIDERFELL].mfIsPlayerCtrl() )
		{
			for (i=random(3)+4; i>=0; i--)
			{
				if (random(3))
					DoRandomEvent(13, SPIDERFELL);
				else
					DoRandomEvent(random(4)+9, SPIDERFELL);
			}
		}

		// Rhuobhe, 4-6 monsters
		if ( !realm[REALM::RHUOBHE].mfIsPlayerCtrl() )
		{
			for (i=random(3)+4; i>=0; i--)
				DoRandomEvent(random(4)+9, RHUOBHE);
		}
	}

	// ------------------------------------------
	// just for fun, lets put a perminent ward on SIDEATH
	// ------------------------------------------
	province[SIDEATH].Warded = 3;
	province[SIDEATH].DispelLvl = REALM::MONSTERS;

}

/* ========================================================================
	Function	- cntUnits_ 
	Description - Count the number of valid troops.
	Returns	- 
	======================================================================== */
LONG cntUnits_ (void)
{
	LONG	i;

	for (cntUnits=1, i=0; i < MAX_UNITS; ++i)
		if (units[i].Realm != REALM::NO_COUNTRY)
			++cntUnits;

	return cntUnits;
}

/* ========================================================================
   Function    - maxNPCUnits
   Description - The number of troops that NPC's can muster.
   Returns     - 
   ======================================================================== */
LONG maxNPCUnits()
{
	LONG Result = NPC_MAX_UNITS;
	#if defined(_WINDOWS)
		if ( IsMultiPlayer() )
		{
			Result = MAX_UNITS - (PER_PLAYER_RESERVED_UNITS * AMultiPlayer.HowManyPlayers());
		}
	#endif
	return Result;
}


void UpdateUnitAndPlaceCount (void)
{
	LONG	i;

	cntUnits_();

	for (cntHolds=0, i=0; i < MAX_PLACES; ++i)
		if (places[i].Realm != REALM::NO_COUNTRY)
			++cntHolds;
}

/* ========================================================================
	Function	- UpdateDataArea
	Description -
	Returns	- void
	======================================================================== */
//CSTRPTR szDomainMode[] = {"MIN_MODE","ACTN_MODE","MAINT_MODE","TAXLVL_MODE","OCCUPATION_MODE","PAYRL_MODE","SELRLM_MODE","ADVPARTY_MODE","INIT_REPORT_MODE","REPORT_MODE","REPORT_DONE_MODE"};
BOOL DoPageProvince(LONG *startCount, LONG provCount, LONG countToPrint, LONG counter, LONG x, LONG y);
void NextPrev (LONG val, LONG);
static LONG		startCount = 1;
static LONG		startCount1 = 1;
static LONG		startCount2 = 1;

void ResetStartCount (void)
{
	startCount = 1;
	startCount1 = 1;
	startCount2 = 1;
}

BOOL UpdateDataArea (void)
{
	LONG					i, j, k, l, p, q, x, xs, tribute, law_claims;
	//LONG				  castle_index;
	LONG					y = 36;			 // starting y position
	REALM_STRUCT_PTR	pRealm = &realm[HomeRealm];
	REGENT_PTR			pRegent = &regents[ActiveRegent];
	SHORT					iUnit;
	char					n[25];
	static LONG			wait = 0;
	static BOOL			fCanDoPushRegions = TRUE;
	LONG 		h1;
	LONG		provCount = 0;
	LONG		counter = 0;
	LONG		lPrint = FALSE;
	LONG		lPageDone = FALSE;

	LONG		provCount1 = 0;
	LONG		counter1 = 0;
	LONG		lPrint1 = FALSE;
	LONG		lPageDone1 = FALSE;

	LONG		provCount2 = 0;
	LONG		counter2 = 0;
	LONG		lPrint2 = FALSE;
	LONG		lPageDone2 = FALSE;

//printf("UpdateDataArea - dturn_mode:%s (%d)  global_action_turn:%d\n",(dturn_mode>10)?"MAX_MODE":szDomainMode[dturn_mode],dturn_mode,global_action_turn);

	if (HomeRealm == 0 || ActiveRegent == 0)  // problems!
	{
		DrawBitmap (0, 0, iDTurnBk,(SHORT)(DATA_AREA_MAX-l_map), 0, (SHORT)l_map, 480);
		return FALSE;
	}

	PTR_PLAYER_STATS pPS = NULL;


#ifdef _DEMO
	InitAutoDemo();
#endif
	del_region(NextPrev,0);
	del_region(AdjTaxLevel,0);
	del_region(AdjCastleSupport,0);
	del_region(AdjUnitPay,0);
	del_region(AdjCourt,0);
	del_region(QueryBloodlineStrength,0);
	del_region(ToggleShowUnits,0);
	del_region(ToggleShowHoldings,0);
	del_region(AdjLawClaims,0);
	del_region(AdjPillaged,0);
	del_region(AdjOccupyOwn,0);
	del_region(AdjReduceHolds,0);
//	del_region(PayOccupiedProv,0);

	// GEH for the chat panel
	fUpdatePanels = TRUE;
	
	//--------------------------------------------------
	// SELECT A REALM
	//--------------------------------------------------
	// display select regent display
	if (dturn_mode == SELRLM_MODE)
	{
		DisplayRealmSelection();
		return FALSE;
	}

	//--------------------------------------------------
	// GO ON ADVENTURE
	//--------------------------------------------------
	// display adventure party display
	if (dturn_mode == ADVPARTY_MODE)
	{
		DisplayAdvParty();
		return FALSE;
	}

	//--------------------------------------------------
	// AFTER DOMAIN ACTIONS loop to councilor's screen
	//--------------------------------------------------
	// return to min mode and do report
	if (dturn_mode == ACTN_MODE && fRoundOver
//		&& !fInfoOnlyDomainUI	[abc] 9/3
		)
	{
		usedFreeAction = usedFreeForge = usedFreeMagic = 0;
		usedFreeAgitate = usedFreeSpy = 0;
		payregency = 0;
		fAfterTaxRolls = FALSE;
		fHeldAction = FALSE;
		pillage = 0;

		//--------------------------------------------------
		// realm affecting magic
		//--------------------------------------------------
		for(i=1; i<PROVINCE_COUNT; ++i)
		{
			run_timers();

			if (playerstats[realm[province[i].Realm].mfGetRegent()] != fERROR)
				pPS = (PTR_PLAYER_STATS)BLKPTR(playerstats[realm[province[i].Realm].mfGetRegent()]);

			if(province[i].Blessed > 0)
				--province[i].Blessed;

			// items which bless
			if (pPS != NULL &&
				pPS->StatsMod.mfGetMod(STATSMOD::PROSPERITY_MOD) > 0 &&
				//province[i].Realm == HomeRealm &&
				province[i].Blessed == 0)
			{
				++province[i].Blessed;
				//SetGameData(MP_PROVINCE, MPPROV_BLESSED, i, province[i].Blessed, TRUE );
				SetGameData(MP_PROVINCE, MPPROV_BLESSED, i, province[i].Blessed, FALSE );
			}

			if(province[i].Blighted > 0)
				--province[i].Blighted;

			// items which blight
			if (pPS != NULL &&
				pPS->StatsMod.mfGetMod(STATSMOD::PROSPERITY_MOD) < 0 &&
				//province[i].Realm == HomeRealm &&
				province[i].Blighted == 0)
			{
				++province[i].Blighted;
				//SetGameData(MP_PROVINCE, MPPROV_BLIGHTED, i, province[i].Blighted, TRUE );
				SetGameData(MP_PROVINCE, MPPROV_BLIGHTED, i, province[i].Blighted, FALSE );
			}

			if(province[i].Warded >0 )
				--province[i].Warded;

			if(province[i].Honest > 0)
				--province[i].Honest;

			if(province[i].Dispel > 0)
				--province[i].Dispel;

			province[i].LevyMustered = 0;
			province[i].Contested = FALSE;
			province[i].OccupRealm = 0;
			fProv[i] = FALSE;					// clear in prep for next test, below
		}

		//--------------------------------------------------
		// check for occupied provinces
		//--------------------------------------------------
		for (i=1; i<MAX_UNITS; ++i)					// scan through units
		{
			if (units[i].Joined == FALSE  					// if leader
				&& units[i].Realm != REALM::NO_COUNTRY)	// if active
			{
				p = units[i].province;

				// castle owned by other causes prov to be contested
				if ( CheckEnemyCastle(p, units[i].Realm) )
				{
					province[p].Contested = TRUE;		// CONTESTED, not occupied
					province[p].OccupRealm = 0;
					printf("OCCUPY - castle causes contest in prov:%s\n",province[p].name);
				}

				// if non-ally, 2nd unit causes prov to be contested
				else if (fProv[p] && !AREALLIED(fProv[p], units[i].Realm))
				{
					province[p].Contested = TRUE;		// CONTESTED, not occupied
					province[p].OccupRealm = 0;
					printf("OCCUPY - 2nd unit causes contest in prov:%s\n",province[p].name);
				}

				// otherwise, unit occupies
				else if (!AREALLIED(province[p].Realm, units[i].Realm))	// don't occupy friends
				{
					province[p].OccupRealm = units[i].Realm;		// occupy the prov
					printf("OCCUPY - %s occupies prov:%s\n",realm[units[i].Realm].mfGetName(),province[p].name);
				}

				fProv[p] = units[i].Realm;
			}
		}
		run_timers();  // cdb 11/27	// ABC don't put in inner loops

		//--------------------------------------------------
		// update loyalty
		//--------------------------------------------------
		REALM::REALM_TYPE tempHome = HomeRealm;

		if ( fControlMode == 0 )
		{
			// Do no loyalty adjustment in Basic control mode. Screws things up.
		}
		else

		#ifdef _WINDOWS
		if ( IsMultiPlayer() )
		{
			for (HomeRealm = REALM::FIRST_REALM; HomeRealm < LAND_REALM_COUNT; HomeRealm = (REALM::REALM_TYPE)(HomeRealm + 1))
			{
				for (i=0; i< PROVINCE_COUNT; ++i)
				{
					if (province[i].Realm == HomeRealm || province[i].OccupRealm == HomeRealm)
					{
						ProvinceInfo((PROVINCE)i, CASTLE1_ICON);
						l = (province[i].Contested) ? 0 : LoyaltyAdj[province[i].TaxLevel];

						// if province occupied
						if (province[i].OccupRealm)
						{
							l--;		// lose a loyalty level
						}

						// !!!!!!!!!! add other loyalty mods here

						j = (province[i].OccupRealm==HomeRealm) ? prov_units : prov_law;

						if (j >= (LONG)province[i].CivLevel)	  // all law available
						{
							if (l<0) ++l; if (l<0) ++l;
						}
						else if ((j*2)>=(LONG)province[i].CivLevel) // half of all law
						{
							if (l<0) ++l;
						}
						else											// less than half of law
						{
							l = NoLawLoyaltyAdj[province[i].TaxLevel] - (province[i].OccupRealm!=0);
						}
						l += (LONG)province[i].Loyalty;
						if (l < 0) l = 0;							 // peg at zero
						if (l > 3) l = 3;							 // peg at max
						if (l==0 && province[i].Loyalty)		// report on rebellion
						{
							province[i].Contested = TRUE;
							SetGameData(MP_PROVINCE, MPPROV_CONTESTED, i, TRUE, FALSE );
						}

						province[i].Loyalty = l;				// update province structure
						SetGameData(MP_PROVINCE, MPPROV_LOYALTY, i, l, FALSE );
					}
				}
			}
			HomeRealm = tempHome;
		}

		else
		#endif
		{
			for (i=0; i< PROVINCE_COUNT; ++i)
			{
				if (province[i].Realm == HomeRealm || province[i].OccupRealm == HomeRealm)
				{
					ProvinceInfo((PROVINCE)i, CASTLE1_ICON);
					l = (province[i].Contested) ? 0 : LoyaltyAdj[province[i].TaxLevel];

					// if province occupied
					if (province[i].OccupRealm)
					{
						if (province[i].OccupRealm == HomeRealm)
							AddReport(SUBJ_SELF_OCCUP,i,0,0);
						else
							AddReport(SUBJ_FOREIGN_OCCUP,province[i].OccupRealm,i,0);
						--l;		// lose a loyalty level
					}

					// !!!!!!!!!! add other loyalty mods here

					j = (province[i].OccupRealm==HomeRealm) ? prov_units : prov_law;

					if (j >= (LONG)province[i].CivLevel)	  // all law available
					{
						if (l<0) ++l; if (l<0) ++l;
					}
					else if ((j*2)>=(LONG)province[i].CivLevel) // half of all law
					{
						if (l<0) ++l;
					}
					else											// less than half of law
					{
						l = NoLawLoyaltyAdj[province[i].TaxLevel] - (province[i].OccupRealm!=0);
					}
					l += (LONG)province[i].Loyalty;
					if (l < 0) l = 0;							 // peg at zero
					if (l > 3) l = 3;							 // peg at max
					if (l==0 && province[i].Loyalty)		// report on rebellion
					{
						if (province[i].TaxLevel>=2)
							AddReport(SUBJ_REBEL_SEVERE,i,0,0);
						else
							AddReport(SUBJ_REBELLION,i,0,0);

						province[i].Contested = TRUE;
						SetGameData(MP_PROVINCE, MPPROV_CONTESTED, i, TRUE, FALSE );
					}

					if (l && province[i].Loyalty==0)
						AddReport(SUBJ_REBELLION_ENDS,i,0,0);
					province[i].Loyalty = l;				// update province structure
					SetGameData(MP_PROVINCE, MPPROV_LOYALTY, i, l, FALSE );
				}

				if (province[i].Realm == HomeRealm && province[i].OccupRealm == HomeRealm)
					AddReport(SUBJ_SELF_OCCUP,i,0,0);
			}
		}

		run_timers();  // cdb 11/27	// ABC don't put in inner loops

		//--------------------------------------------------
		// count all places and units
		//--------------------------------------------------
		UpdateUnitAndPlaceCount();
		//printf("TOTAL UNITS:%d - TOTAL PLACES:%d\n",cntUnits_(),cntHolds);

		// revert pay to gold
		for (i=1; i<MAX_UNITS; ++i)
			if (units[i].Realm == HomeRealm)			// scan through units
			{
				units[i].Use_Regency = FALSE;
				units[i].Paid = TRUE;
			}

		//--------------------------------------------------
		// degrade castles if unsupported or besieged
		//--------------------------------------------------
		DegradeCastles();

		UpdateHoldingFlag();								// mark new holdings

		fDoActionIcon = FALSE;
		fDepressIcon = FALSE;

		// -----------------------------------------------------------------
		//	Decide if we have enought points to win the game
		// -----------------------------------------------------------------
		// now if this is greater then 300, you win!
		CurrentScore = CalculatePoints(HomeRealm);

		if (!fWonGame && CurrentScore > gWinningPoints)
		{
			fWonGame = TRUE;
			PlayFinale(0,0);
		}

#ifdef _WINDOWS
		AMultiPlayer.NextSeed(__FILE__,__LINE__);
#endif

		//--------------------------------------------------
		// Alter reactions based on last turn's activities
		//--------------------------------------------------
		for (i=0; i<LAND_REALM_COUNT; ++i)
		{
			for (j=0; j<LAND_REALM_COUNT; ++j)
			{
				if (i==j)		// we LIKE ourselves
				{
					RealmReaction[i][j] = 99;
					RealmStatus[i][j] = STATUS_FULL_ALLIANCE;
					continue;
				}

				k = RealmStatus[i][j] & 0x0F;
				l = RealmReaction[i][j];

				// if screwed we get unfriendlier
				if (RealmStatus[i][j] & 0x80)
					l -= (random(5)+1);
				else if (i!=REALM::MONSTERS && i!=REALM::GORGONSCROWN && i!=REALM::MURKILAD
						&& i!=REALM::MARKAZOR && i!=REALM::FIVEPEAKS && i!=REALM::SPIDERFELL
						&& j!=REALM::MONSTERS && j!=REALM::GORGONSCROWN && j!=REALM::MURKILAD
						&& j!=REALM::MARKAZOR && j!=REALM::FIVEPEAKS && j!=REALM::SPIDERFELL
						&& i!=REALM::RHUOBHE && j!=REALM::RHUOBHE
						&& random(50)==0)
					l += (random(5)+1);		// if not screwed we might get friendlier

				// if more than 75% of the winning points everyone hates them
//				if (((sScores[i] * 100) / gWinningPoints) > 75)
//					l -= random(10);

				if (k == STATUS_PERMISSIVE_ALLIANCE && l < 35)
				{
					RealmStatus[i][j] = STATUS_NEUTRAL;
					RealmStatus[j][i] = STATUS_NEUTRAL;
					if (j==HomeRealm)
						AddReport(IMMEDIATE_ALLIANCE_DEG, i, 0, 0);
				}
				if (k == STATUS_FULL_ALLIANCE && l < 45)
				{
					RealmStatus[i][j] = STATUS_PERMISSIVE_ALLIANCE;
					RealmStatus[j][i] = STATUS_PERMISSIVE_ALLIANCE;
					if (j==HomeRealm)
						AddReport(IMMEDIATE_ALLIANCE_DEG, i, 0, 0);
				}
				if (k == STATUS_VASSAL)
				{
					if (Tribute[j][i] && l >= 40 && l < 65)
						l = 65 + (Tribute[j][i]*3);
					if (l < 40)
					{
						//Tribute[j][i] = 0;
						//Tribute[i][j] = 0;
						SetGameData(MP_TRIBUTE,j, i, 0, TRUE);
						SetGameData(MP_TRIBUTE,i, j, 0, TRUE);
						RealmStatus[j][i] = STATUS_NEUTRAL;
						RealmStatus[i][j] = STATUS_NEUTRAL;
						if (j==HomeRealm)
							AddReport(IMMEDIATE_VASSAL_LEAVE, i, 0, 0);
					}
				}
				if (l < 0) l = 0;
				RealmReaction[i][j] = l;
			}
		}

		run_timers();  // cdb 11/27	// ABC don't put in inner loops

		//--------------------------------------------------
		// Check for adding new adventure sites
		//--------------------------------------------------
#ifdef _WINDOWS
		if (!IsMultiPlayer() && random(100)<25 && date>1)		// 75% chance of a new adventure site
			SelectAdvSite();
#else
		if (random(100)<25 && date>1)		// 75% chance of a new adventure site
			SelectAdvSite();
#endif

#if defined(_JUNEDEMO)
		if (date > 8)
		{
			ShowAdScreen(4);
			QuitSys(0,0);
		}
#endif

		TutorialActionNumber = 0;
		TutorialScreenNumber = 0;

		dturn_mode = INIT_REPORT_MODE;
		fRedrawAll = TRUE;
		return FALSE;
	}

	//--------------------------------------------------
	// INITIAL REPORT (Councilor's Screen)
	//--------------------------------------------------
	if (dturn_mode == INIT_REPORT_MODE
//			&& !fInfoOnlyDomainUI	[abc] 9/3
		)
	{
		dturn_mode = REPORT_MODE;
		++date;
		ClearOldReports();
		GiveReport(0);
		report_done_dturn_mode = REPORT_DONE_MODE;
		return FALSE;
	}

	if (dturn_mode == REPORT_MODE
//			&& !fInfoOnlyDomainUI	[abc] 9/3
		)
		return FALSE;

	//--------------------------------------------------
	// AFTER INITIAL REPORT (Councilor's Screen)
	//--------------------------------------------------
	// reset to admin phase
	if (dturn_mode == REPORT_DONE_MODE
//			&& !fInfoOnlyDomainUI	[abc] 9/3
		)
	{
		if (fLoadedNewGame)				// after initial report, restart domain game
		{
			fLoadedNewGame = FALSE;
			HideMenu(D_GAMEBUTTON);
			ShowMenu(D_GAMEBUTTON);
			DeleteDomainRegions();
			AddDomainRegions();

			for (i=1; i<MAX_UNITS; ++i)						// purge unit icons
			{
				if (units[i].iIconBitm > 0)
				{
					SetPurge(units[i].iIconBitm);
					units[i].iIconBitm = fERROR;
				}
			}

			for (i=1; i<MAX_UNITS; ++i)
				if (units[i].Icon != NO_MAP_ICON)
				{
					sprintf(n,"UI\\%s.PCX",GameIcons[units[i].Icon]);
					units[i].iIconBitm = GetResourceStd(n, FALSE);
#if fUSE_RES_FILES
					SetPurge(units[i].iIconBitm);
#endif
				}
				else
					units[i].iIconBitm = fERROR;
		}

		// set our castles as suported
		for (i=1; i< PROVINCE_COUNT; ++i)
			if (province[i].Realm == HomeRealm)
				province[i].CastleSptd = TRUE;

		RandomEvents();
		DoNPCAdmin();
		ResetStartCount();

		// reset to first player realm
		#ifdef _WINDOWS
		if (!IsMultiPlayer())
		{
			i = HomeRealm;
			iHotSeatMultiplayerIndex = 1;
			// first, try for a DM controlled
			while (!realm[iHotSeatMultiplayerIndex].mfIsDMCtrl() && iHotSeatMultiplayerIndex < LAND_REALM_COUNT)
				iHotSeatMultiplayerIndex++;
			// if not successful, try for player controlled
			if (iHotSeatMultiplayerIndex == LAND_REALM_COUNT)
			{
				iHotSeatMultiplayerIndex = 1;
				while (!realm[iHotSeatMultiplayerIndex].mfIsPlayerCtrl() && iHotSeatMultiplayerIndex < LAND_REALM_COUNT)
					iHotSeatMultiplayerIndex++;
			}
			// lastly, set for AUTOPLAY
			if (iHotSeatMultiplayerIndex == LAND_REALM_COUNT)
				iHotSeatMultiplayerIndex = REALM::MIERES;
			HomeRealm = (REALM::REALM_TYPE)iHotSeatMultiplayerIndex;
			ActiveRegent = realm[HomeRealm].mfGetRegent();
			if (i != HomeRealm)
			{
				UpdateHoldingFlag();
				ReconstructMap();
			}
		}
		#else
		i = HomeRealm;
		iHotSeatMultiplayerIndex = 1;
		// first, try for a DM controlled
		while (!realm[iHotSeatMultiplayerIndex].mfIsDMCtrl() && iHotSeatMultiplayerIndex < LAND_REALM_COUNT)
			iHotSeatMultiplayerIndex++;
		// if not successful, try for player controlled
		if (iHotSeatMultiplayerIndex == LAND_REALM_COUNT)
		{
			iHotSeatMultiplayerIndex = 1;
			while (!realm[iHotSeatMultiplayerIndex].mfIsPlayerCtrl() && iHotSeatMultiplayerIndex < LAND_REALM_COUNT)
				iHotSeatMultiplayerIndex++;
		}
		// lastly, set for AUTOPLAY
		if (iHotSeatMultiplayerIndex == LAND_REALM_COUNT)
			iHotSeatMultiplayerIndex = REALM::MIERES;
		HomeRealm = iHotSeatMultiplayerIndex;
		ActiveRegent = realm[HomeRealm].mfGetRegent();
		if (i != HomeRealm)
		{
			UpdateHoldingFlag();
			ReconstructMap();
		}
		#endif

	// fortify deamon:  roll dice for all in-work castles here
	//	Level = iRoute2/8
	//	iRoute1 = desired level
	//	iRoute2 = already spent
	//	iRoute3 = spend this turn <-- result of die roll goes here
		RollDiceForFortify();

		//--------------------------------------------------
		// save the current state of the game
		//--------------------------------------------------
		{
			FILE *fp;
			char cur_sav[25] = "savegame\\";
			strcat(cur_sav, STRMGR_GetStr(STR_SAVEGAME));
 	 		if (Exists(cur_sav))
				remove (cur_sav);

    		mkdir("SaveGame");			// in case it's not there
			fp = fopen(cur_sav, "wb");
			SaveGameFiles(fp);
			fclose(fp);
		}

		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
			char temp[150];
			sprintf ( temp, "=============================================");
			RandomLogComment ( temp );
			sprintf ( temp, "===          START OF TURN %02d           ===",date);
			RandomLogComment ( temp );
			sprintf ( temp, "=============================================");
			RandomLogComment ( temp );
		}
	}

	//--------------------------------------------------
	// AFTER COUNCILOR'S SCREEN either initial or button
	//--------------------------------------------------
	if ((dturn_mode == REPORT_DONE_MODE || dturn_mode == REPEAT_REPORT_DONE_MODE)
//			&& !fInfoOnlyDomainUI	[abc] 9/3
		)
	{
		TutorialActionNumber = 1;
		TutorialScreenNumber = 0;
		for (i=0; i<LAND_REALM_COUNT; i++)
		{
			action_turn[i] = 1;
			prev_action_turn[i] = 1;
		}
		iCurrentRealmIndex = 0;
		fRoundOver = FALSE;
		dturn_mode = MIN_MODE;
		max_scale = DMAX_SCALE;
		req_l_map = DATA_AREA_MIN;
		l_map = DATA_AREA_MIN;
//		reqSound = SND_UI_STATUS_REPORT;
		ACTIVATE_REGION_(regDOMAIN_TURN_MODE	, TRUE);		// domain turn mode ON
		ACTIVATE_REGION_(regACTION_MODE			, FALSE);	// action mode OFF
		ACTIVATE_REGION_(regTAX_ROLLS				, TRUE);		// tax rolls ON
		ACTIVATE_REGION_(regSWITCH_TO_ACTIONS	, FALSE);	// switch to actions OFF
		ACTIVATE_REGION_(regADJUST_TAXES			, TRUE);		// adjust taxes ON
		activate_region(regSHOW_HOLDINGS			, TRUE);
		//DataAreaMode (MIN_MODE, 1);

		fFadedOut = -1;
		fRedrawAll = TRUE;

		return FALSE;
	}

	//--------------------------------------------------
	// DOMAIN ACTIONS
	//--------------------------------------------------
	// display domain action buttons, etc.
	if (dturn_mode == ACTN_MODE)
	{
		if (fStartOfActionMode			// done once at start of action mode
//				&& !fInfoOnlyDomainUI	[abc] 9/3
			)
		{
			fStartOfActionMode = FALSE;

			// auto-investiture in basic and advanced control modes
			if (fControlMode < 2 && !fTutorialSelected)
			{
				#ifdef _WINDOWS
				if (IsMultiPlayer())
				{
					// can't xmit data until all have reached this point, put up wait box
					StartSync( iMPS_SYNC7 );
					CheckSync( iMPS_SYNC7, TRUE, TRUE );
				}
				#endif

				for (i=1; i<PROVINCE_COUNT; ++i)
					if (province[i].OccupRealm == HomeRealm)		// we occupy the prov
					{
						j = province[i].Realm;
						SetGameData(MP_PROVINCE, MPPROV_REALM, i, HomeRealm, TRUE );
						SetGameData(MP_PROVINCE, MPPROV_OCCUPREALM, i, 0, TRUE );
						//ReconstructMap();
						//CheckRealmStatus(HomeRealm, j, TRUE );
						SetGameData(MP_PROVINCE, MPPROV_INVESTED, HomeRealm, j, TRUE );
					}
			}

			UpdatingFortify();			// call the fortify (castle building) deamon here

			TutorialActionNumber = 2;
			TutorialScreenNumber = 0;

			//--------------------------------------------------
			// turn off AT WAR status
			//--------------------------------------------------
			for (i=0; i<LAND_REALM_COUNT; ++i)
				for (j=0; j<LAND_REALM_COUNT; ++j)
					if ((RealmStatus[i][j] & 0x0F) == STATUS_AT_WAR)
						RealmStatus[i][j] = STATUS_NEUTRAL;

			// ------------------------------------------
			// Tell the other machines our financial status
			// ------------------------------------------
   	   SetGameData( MP_REALM, MPRLM_INCOME, HomeRealm,
				realm[HomeRealm].mfGetIncome(), TRUE );

     	   SetGameData( MP_REALM, MPRLM_TREASURY, HomeRealm,
				realm[HomeRealm].mfGetTreasury(), TRUE );

			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
				char temp[100];
				sprintf ( temp, "PLAYER FINANCIAL INFO - %s  treasury:%ld  income:%ld",
					realm[HomeRealm].mfGetName(),
					realm[HomeRealm].mfGetTreasury(),
					realm[HomeRealm].mfGetIncome() );
				RandomLogComment ( temp );
			}

			if (fControlMode & 0x100)		// control mode has changed, set correct stuff
			{
				fControlMode &= 0xFF;
				LoadControlSet();
			}
			ActivateControlSet();		// make sure stuff is set right

			// reset all the necessary vars to start the turn
			CurrentRealm = REALM::REALM_OUT_OF_RANGE;
			for (i=0; i<LAND_REALM_COUNT; i++)
			{
				action_turn[i] = 1;
				prev_action_turn[i] = 1;
			}
			iCurrentRealmIndex = 0;
		}

		// effects on actions of regent's class
		i = regents[ActiveRegent].mfGetClass1(); j = regents[ActiveRegent].mfGetClass2();
		dact_ic_active[FORGE_LEY_LINE] = (i==WIZARD || j==WIZARD || i==BARD || j==BARD);
		dact_ic_active[REALM_SPELL]= (i==WIZARD || j==WIZARD || i==PRIEST || j==PRIEST || i==BARD || j==BARD);
		dact_ic_active[BUILD_ROAD]=dact_ic_active[DECLARE_WAR]=dact_ic_active[HOLD_ACTION]=dact_ic_active[INVESTITURE]=dact_ic_active[LIEUTANANT]=dact_ic_active[LIEUTENANT_ACTION]=dact_ic_active[MUSTER]=(ActiveRegent==realm[HomeRealm].mfGetRegent());
		dact_ic_active[LIEUTENANT_ACTION] = (fLTAction == BEFORE_LTACTION);
		dact_ic_active[DIPLOMACY] = (realm[HomeRealm].mfGetCourt() != 0);
		dact_ic_active[DECLARE_WAR] = (action_turn[HomeRealm]>=3)?FALSE:dact_ic_active[DECLARE_WAR];

		if (cntUnits_() > (MAX_UNITS-10))
		{
			dact_ic_active[MUSTER] = FALSE;
			dact_ic_active[LIEUTANANT] = FALSE;
		}

		dact_ic_active[CREATE_HOLDING] = (cntHolds < (MAX_PLACES-10));

	#ifdef _WINDOWS
		//--- DEACTIVATE ADVENTURES IN MULTIPLAYER ---
//		if (IsMultiPlayer())
//			dact_ic_active[ADVENTURE] = FALSE;
		dact_ic_active[ADVENTURE] = !(IsMultiPlayer());
	#endif
	
		if (fTutorialSelected)
		{
			for (j=0; j<19; ++j)
				dact_ic_active[j] = GameSpecificArray2[j];
		}

		//iDTurnBk = GetResourceStd("UI\\DTURN_BK.PCX", FALSE);
		DrawBitmap (0, 0, iDTurnBk, (SHORT)(DATA_AREA_MAX-l_map), 0, (SHORT)l_map, 480);

		if (fControlMode & 0x100)		// control mode has changed, set correct stuff
		{
			fControlMode &= 0xFF;
			LoadControlSet();
			ActivateControlSet();
		}

		if (fControlMode == 0)
		{
			//print_textf(9,  56+8, DKBROWN, "^F00^W096");
			DrawBitmap (9, (SHORT)(64+(1*56)), (dact_ic_active[1])?iDActnIc:iDActnI2, 0, (SHORT)(1*56), 48, 46);
			DrawBitmap (9, (SHORT)(64+(2*56)), (dact_ic_active[6])?iDActnIc:iDActnI2, 0, (SHORT)(2*56), 48, 46);
			DrawBitmap (9, (SHORT)(64+(3*56)), (dact_ic_active[7])?iDActnIc:iDActnI2, 0, (SHORT)(3*56), 48, 46);
			DrawBitmap (9, (SHORT)(64+(4*56)), (dact_ic_active[11])?iDActnIc:iDActnI2, 0, (SHORT)(4*56), 48, 46);
			DrawBitmap (9, (SHORT)(64+(5*56)), (dact_ic_active[13])?iDActnIc:iDActnI2, 0, (SHORT)(5*56), 48, 46);
			DrawBitmap (9, (SHORT)(64+(6*56)), (dact_ic_active[15])?iDActnIc:iDActnI2, 0, (SHORT)(6*56), 48, 46);
			init_gfont(21);
			print_textf(58, 64+(1*56)+22-10, DKBROWN, STRMGR_GetStr(STR_GM_NACTIONS_ADV));
			print_textf(58, 64+(2*56)+22-10, DKBROWN, STRMGR_GetStr(STR_GM_NACTIONS_WAR));
			print_textf(58, 64+(3*56)+22-5 , DKBROWN, STRMGR_GetStr(STR_GM_NACTIONS_DIP));
			print_textf(58, 64+(4*56)+22-10, DKBROWN, STRMGR_GetStr(STR_GM_NACTIONS_HLD));
			print_textf(58, 64+(5*56)+22-10, DKBROWN, STRMGR_GetStr(STR_GM_NACTIONS_HIR));
			print_textf(58, 64+(6*56)+22-10, DKBROWN, STRMGR_GetStr(STR_GM_NACTIONS_MST));
		}
		if (fControlMode == 1)
		{
			DrawBitmap (9, (SHORT)(64+(0*41)), (dact_ic_active[1])?iDActnIc:iDActnI2, 0, (SHORT)(0*41), 48, 41);
			DrawBitmap (9, (SHORT)(64+(1*41)), (dact_ic_active[3])?iDActnIc:iDActnI2, 0, (SHORT)(1*41), 48, 41);
			DrawBitmap (9, (SHORT)(64+(2*41)), (dact_ic_active[5])?iDActnIc:iDActnI2, 0, (SHORT)(2*41), 48, 41);
			DrawBitmap (9, (SHORT)(64+(3*41)), (dact_ic_active[6])?iDActnIc:iDActnI2, 0, (SHORT)(3*41), 48, 41);
			DrawBitmap (9, (SHORT)(64+(4*41)), (dact_ic_active[7])?iDActnIc:iDActnI2, 0, (SHORT)(4*41), 48, 41);
			DrawBitmap (9, (SHORT)(64+(5*41)), (dact_ic_active[11])?iDActnIc:iDActnI2, 0, (SHORT)(5*41), 48, 41);
			DrawBitmap (9, (SHORT)(64+(6*41)), (dact_ic_active[13])?iDActnIc:iDActnI2, 0, (SHORT)(6*41), 48, 41);
			DrawBitmap (9, (SHORT)(64+(7*41)), (dact_ic_active[15])?iDActnIc:iDActnI2, 0, (SHORT)(7*41), 48, 41);
			DrawBitmap (9, (SHORT)(64+(8*41)), (dact_ic_active[17])?iDActnIc:iDActnI2, 0, (SHORT)(8*41), 48, 41);
			DrawBitmap (9, (SHORT)(64+(9*41)), (dact_ic_active[18])?iDActnIc:iDActnI2, 0, (SHORT)(9*41), 48, 41);
			init_gfont(21);
			print_textf(58, 64+(0*41)+20-10, DKBROWN, STRMGR_GetStr(STR_GM_NACTIONS_ADV));
			print_textf(58, 64+(1*41)+20-10, DKBROWN, STRMGR_GetStr(STR_GM_NACTIONS_RD));
			print_textf(58, 64+(2*41)+20-10, DKBROWN, STRMGR_GetStr(STR_GM_NACTIONS_CRE));
			print_textf(58, 64+(3*41)+20-10, DKBROWN, STRMGR_GetStr(STR_GM_NACTIONS_WAR));
			print_textf(58, 64+(4*41)+20-5 , DKBROWN, STRMGR_GetStr(STR_GM_NACTIONS_DIP));
			print_textf(58, 64+(5*41)+20-10, DKBROWN, STRMGR_GetStr(STR_GM_NACTIONS_HLD));
			print_textf(58, 64+(6*41)+20-10, DKBROWN, STRMGR_GetStr(STR_GM_NACTIONS_HIR));
			print_textf(58, 64+(7*41)+20-10, DKBROWN, STRMGR_GetStr(STR_GM_NACTIONS_MST));
			print_textf(58, 64+(8*41)+20-10, DKBROWN, STRMGR_GetStr(STR_GM_NACTIONS_RUL));
			print_textf(58, 64+(9*41)+20-15, DKBROWN, STRMGR_GetStr(STR_GM_NACTIONS_CTR));
		}
		if (fControlMode == 2)
		{
			for (j=0; j<9; ++j)
				for (i=0; i<2; ++i)
				{
					if (dact_ic_active[i+(j*2)+1])
						DrawBitmap ((SHORT)(9+(i*48)), (SHORT)(64+(j*46)), iDActnIc, (SHORT)(i*48), (SHORT)(j*46), 48, 46);
					else
						DrawBitmap ((SHORT)(9+(i*48)), (SHORT)(64+(j*46)), iDActnI2, (SHORT)(i*48), (SHORT)(j*46), 48, 46);
				}
		}

		// print realm name
		SetRemapTable(HomeRealm);
		DrawBitmap (5, 1, iDShldIc, 0, 0, 999, 999);
		ClearRemapTable();

		init_gfont(FONT_TITL_20PT);
		xs = gtext_width(realm[HomeRealm].mfGetName());
		xs = (xs > 94)? (9400 / xs) : 100;
		print_textf(8, 1, DKBROWN, "^X%03d^F08^N%s", xs, realm[HomeRealm].mfGetName());

		if (realm[HomeRealm].mfIsDMCtrl())
			print_textf(395, 5, DKBROWN, "^ftimes30.pcx^xc^c122^t1DM CONTROLLED");

		UpdateActionHeader(-1);

		if (l_map == DATA_AREA_ACTN
				&& !sMenusUp && !fFadedOut			// don't draw if menu is up
//				&& !fInfoOnlyDomainUI	[abc] 9/3
			)
		{
			if (DoNPCAction())
				 return TRUE;		// TRUE means a battle was started

			// sometimes, DoNPCAction also raises menus...
			if (!sMenusUp)
				CheckForImmediateReports();
		}
	}

	//--------------------------------------------------
	// HOLDINGS AND UNITS TO BE DISPLAYED
	//--------------------------------------------------
	// display domain list
	else if (fSelectDomains)
	{
		//iDTurnBk = GetResourceStd("UI\\DTURN_BK.PCX", FALSE);
		DrawBitmap (0, 0, iDTurnBk,(SHORT)(DATA_AREA_MAX-l_map), 0, (SHORT)l_map, 480);

		if (l_map==DATA_AREA_MAX)
		{
			// print title
			init_gfont(FONT_TIMS_10PT);
			gprint_text(10, 5, STRMGR_GetStr(STR_GM_UNIT_HOLDING), DKBROWN);

			// first column
			y=36;
//			gprint_text(12, y, STRMGR_GetStr(STR_GM_REALM), DKBROWN);
			gprint_text(12, y, STRMGR_GetStr(STR_F00_REALMS), DKBROWN);
			gprint_text(131, y+2, STRMGR_GetStr(STR_GM_UNITS), DKBROWN);
			gprint_text(152, y+2, STRMGR_GetStr(STR_GM_HOLDINGS), DKBROWN);
			y+=12;
			crease(10, y-4, 144, 1);			// seperator

			init_gfont(FONT_SANS_8PT);
			for (i=1; i<LAND_REALM_COUNT; ++i)
			{
				if (!realm[i].mfExists() || i==REALM::MONSTERS)
					continue;
				DrawShield(8,y,(REALM::REALM_TYPE)i);
				gprint_text(22, y, realm[i].mfGetName(), StatusColor[(RealmStatus[HomeRealm][i])&0x0F]);
				DrawBitmap(117, (SHORT)y, iCheck[realm[i].mfShowUnits()], 0, 0, 20, 20);
				add_region(117, y, 30,12,0,ToggleShowUnits,i,0,0, STR_NULL);
				DrawBitmap(136, (SHORT)y, iCheck[realm[i].mfShowHoldings()], 0, 0, 20, 20);
				add_region(136, y, 30,12,0,ToggleShowHoldings,i,0,0, STR_NULL);
				y+=12;
			}

			// second column
			y=36; x=156;
//			gprint_text(x+12, y, STRMGR_GetStr(STR_GM_REALM), DKBROWN);
			gprint_text(x+12, y, STRMGR_GetStr(STR_F00_OTHER_DOMAINS), DKBROWN);
//			gprint_text(x+138, y+2, STRMGR_GetStr(STR_GM_UNITS), DKBROWN);
			gprint_text(x+159, y+2, STRMGR_GetStr(STR_GM_HOLDINGS), DKBROWN);
			y+=12;
			crease(x+10, y-4, 150, 1);			// seperator

			for (i=LAND_REALM_COUNT; i<REALM::REALM_COUNT; ++i)
			{
				if (!realm[i].mfExists() || i==REALM::MONSTERS)
					continue;
				DrawShield(x+6,y,(REALM::REALM_TYPE)i);
				init_gfont(FONT_SANS_8PT);
				xs = gtext_width(realm[i].mfGetName());
				print_textf(x+20, y, DKBROWN, "^F%02d%s", (xs>112)?21:2, realm[i].mfGetName());
				DrawBitmap(x+144, (SHORT)y, iCheck[realm[i].mfShowHoldings()], 0, 0, 20, 20);
				add_region(x+144, y, 30,12,0,ToggleShowHoldings,i,0,0, STR_NULL);
				y+=12;
			}

			// whole column controls
			y = 436;
			crease(10, y-4, 156+150, 1);			// seperator
			gprint_text(10, y, STRMGR_GetStr(STR_GM_ALL_ON), DKBROWN);

			DrawBitmap(117, (SHORT)y, iCheck[fUnitsAllOn&1], 0, 0, 20, 20);
			add_region(117, y, 30,12,0,ToggleShowUnits,0,1,0, STR_NULL);
			DrawBitmap(136, (SHORT)y, iCheck[fHoldingsAllOn&1], 0, 0, 20, 20);
			add_region(136, y, 30,12,0,ToggleShowHoldings,0,1,0, STR_NULL);

			DrawBitmap(x+144, (SHORT)y, iCheck[(fHoldingsAllOn&2)==2], 0, 0, 20, 20);
			add_region(x+144, y, 30,12,0,ToggleShowHoldings,-(LAND_REALM_COUNT),1,0, STR_NULL);
			y+=16;

			crease(10, y-4, 156+150, 1);			// seperator
			gprint_text(10, y, STRMGR_GetStr(STR_GM_ALL_OFF), DKBROWN);

			DrawBitmap(117, (SHORT)y, iCheck[(fUnitsAllOn&1)==0], 0, 0, 20, 20);
			add_region(117, y, 30,12,0,ToggleShowUnits,0,0,0, STR_NULL);
			DrawBitmap(136, (SHORT)y, iCheck[(fHoldingsAllOn&1)==0], 0, 0, 20, 20);
			add_region(136, y, 30,12,0,ToggleShowHoldings,0,0,0, STR_NULL);

			DrawBitmap(x+144, (SHORT)y, iCheck[(fHoldingsAllOn&2)==0], 0, 0, 20, 20);
			add_region(x+144, y, 30,12,0,ToggleShowHoldings,-(LAND_REALM_COUNT),0,0, STR_NULL);
			y+=16;
			crease(10, y-4, 156+150, 1);			// seperator
			print_textf(10, y, DKBROWN, STRMGR_GetStr(STR_LEGEND_AGAIN));
			ClearRemapTable();
		}
	}

	//--------------------------------------------------
	// PLAYER'S ADMIN SCREEN
	//--------------------------------------------------
	else
	{
		if (fControlMode == 0x100)		// control mode changed to Basic Tutorial
		{
			fControlMode &= 0xFF;		// set correct stuff
			LoadControlSet();
			ActivateControlSet();
		}

		// automatically do tax roll if in Basic controls mode
		//printf("auto-tax  - fControlMode:%d fDoActionIcon:%d\n",fControlMode,fDoActionIcon);
		if (	(fControlMode == 0 && fDoActionIcon == FALSE
//					&& !fInfoOnlyDomainUI	[abc] 9/3
				)
				|| (iHotSeatMultiplayerIndex == REALM::MIERES)
				|| (HomeRealm == REALM::MIERES)
			)
		{
			//DoTaxRolls(0,0);
			DrawBitmap (0, 0, iDTurnBk, (SHORT)(DATA_AREA_MAX-l_map), 0, (SHORT)l_map, 480);
			ScreenCopy( 0, 0, 0, (SHORT)l_map, 480, SC_DEFAULT_RES);
			SwitchToActionMode(0,0);
			fRedrawAll = TRUE;
			return FALSE;
		}

		// make sure we are activated
//!!!!!!!!!!!!!!!!!!!!!!!!!!! abc 6-3-97
		activate_region(regSHOW_HOLDINGS			, TRUE);
		ACTIVATE_REGION_(regDOMAIN_TURN_MODE	, TRUE);		// domain turn mode ON
		ACTIVATE_REGION_(regACTION_MODE			, FALSE);	// action mode OFF
		ACTIVATE_REGION_(regTAX_ROLLS				, dtrn_ic_active[0]);
		ACTIVATE_REGION_(regSWITCH_TO_ACTIONS	, !dtrn_ic_active[0]);
		ACTIVATE_REGION_(regADJUST_TAXES			, dtrn_ic_active[0]);
//!!!!!!!!!!!!!!!!!!!!!!!!!!! abc 6-3-97

	#ifdef _WINDOWS
		if (!IsMultiPlayer())
	#endif
			CurrentRealm = HomeRealm;		// true while in admin phase

		DrawBitmap (0, 0, iDTurnBk, (SHORT)(DATA_AREA_MAX-l_map), 0, (SHORT)l_map, 480);
		if (l_map>=DATA_AREA_MIN)
		{
			if (fTutorialSelected)
			{
				for (j=0; j<6; ++j)
					dtrn_ic_active[j] = GameSpecificArray1[j];
				ACTIVATE_REGION_(regADJUST_TAXES, dtrn_ic_active[0]);
				ACTIVATE_REGION_(regDOMAIN_TURN_MODE, (dtrn_ic_active[1]||dtrn_ic_active[2]||dtrn_ic_active[3]||dtrn_ic_active[4]));
				ACTIVATE_REGION_(regTAX_ROLLS, dtrn_ic_active[5] && !fDoActionIcon);
				ACTIVATE_REGION_(regSWITCH_TO_ACTIONS, dtrn_ic_active[5] && fDoActionIcon);
			}

			for (j=0; j<2; ++j)
				for (i=0; i<3; ++i)
					if (i==2 && j==1 && fDoActionIcon && fFinalRoll >= cProvinces)
						DrawBitmap (9+96-2, 380+46-2, iDDActIc, (dtrn_ic_active[5])?0:48, 0, 48, 46);
					else if (dtrn_ic_active[i+(j*3)])
						DrawBitmap ((SHORT)(9+(i*48)), (SHORT)(380+(j*46)), iDTurnIc, (SHORT)(i*48), (SHORT)(j*46), 48, 46);
					else
						DrawBitmap ((SHORT)(9+(i*48)), (SHORT)(380+(j*46)), iDTurnI2, (SHORT)(i*48), (SHORT)(j*46), 48, 46);

			// print realm name
			SetRemapTable(HomeRealm);
			DrawBitmap (7, 4, iDShldIc, 0, 0, 999, 999);
			ClearRemapTable();

			init_gfont(FONT_TITL_20PT);
			xs = gtext_width(realm[HomeRealm].mfGetName());
			xs = (xs > (l_map-55))? (((l_map-55)*100) / xs) : 100;
			print_textf(10, 5, DKBROWN, "^X%03d^F08^N%s", xs, realm[HomeRealm].mfGetName());

			if (realm[HomeRealm].mfIsDMCtrl())
				print_textf(395, 5, DKBROWN, "^ftimes30.pcx^xc^c122^t1DM CONTROLLED");

			// print regent name
			print_textf(10, y-3, 60, "^F02^l142%s", pRegent->mfGetname());
			y += 12;
		}

		// print brief realm info
		if (l_map==DATA_AREA_MIN)
		{
//			print_textf(50, y, DKBROWN, STRMGR_GetStr(STR_GM_BRIEF1), pRealm->regency-payregency);
			print_textf(50, y, DKBROWN, STRMGR_GetStr(STR_GM_REGENCY_FMT), pRealm->mfGetRegency()-payregency);
			y += 12;
//			print_textf(50, y, DKBROWN, STRMGR_GetStr(STR_GM_BRIEF3),new_treasury);
			print_textf(50, y, DKBROWN, STRMGR_GetStr(STR_GM_TREASURY_FMT), pRealm->mfGetTreasury());
			y += 2;
			crease(10, y+10, 144, 1);			 // seperator
		}
		y += 26;

		//--------------------------------------------------
		// TAX mode and MAINT mode, print provinces
		//--------------------------------------------------
		if (dturn_mode == MIN_MODE || dturn_mode == TAXLVL_MODE || dturn_mode == MAINT_MODE)
		{
			if (l_map==DATA_AREA_MAX)
				y += (dturn_mode == TAXLVL_MODE) ? 30 : 26;

			if (l_map>=DATA_AREA_MIN)
			{
				gprint_text(10, y-10, STRMGR_GetStr(STR_GM_PROVINCE), DKBROWN);
				if (dturn_mode != MAINT_MODE)
					gprint_text(118, y-10, STRMGR_GetStr(STR_GM_LEVEL), DKBROWN);
			}

			if (l_map==DATA_AREA_MAX)
			{
				if (dturn_mode == TAXLVL_MODE)
				{
					line(146, y-24, 208, y-24, DKBROWN);
					gprint_text(177, y-27, STRMGR_GetStr(STR_GM_TAX_LEVEL), DKBROWN);

//					gprint_text(170, y-7,  STRMGR_GetStr(STR_GM_LIGHT), DKBROWN);
//					gprint_text(190, y-7,  STRMGR_GetStr(STR_GM_NORMAL), DKBROWN);
//					gprint_text(210, y-7,  STRMGR_GetStr(STR_GM_SEVERE), DKBROWN);
					gprint_text(170, y-7,  STRMGR_GetStr(STR_GM_AALIGHT), DKBROWN);
					gprint_text(190, y-7,  STRMGR_GetStr(STR_GM_AANORMAL), DKBROWN);
					gprint_text(210, y-7,  STRMGR_GetStr(STR_GM_AASEVERE), DKBROWN);

					gprint_text(232, y-14, STRMGR_GetStr(STR_GM_LOYALTY), DKBROWN);
					gprint_text(232, y-6,  STRMGR_GetStr(STR_GM_CHANGE), DKBROWN);
					gprint_text(268, y-14, STRMGR_GetStr(STR_GM_C_LAW),DKBROWN);
					gprint_text(268, y-6,  STRMGR_GetStr(STR_GM_C_CLAIMS),DKBROWN);
					gprint_text(289, y-10, STRMGR_GetStr(STR_GM_TAX), DKBROWN);
				}
				if (dturn_mode == MAINT_MODE)
				{
					gprint_text(138,	y-8, STRMGR_GetStr(STR_GM_LLEVEL), DKBROWN);
					gprint_text(305-140, y-8, STRMGR_GetStr(STR_GM_LOCCUPIED), DKBROWN);
					gprint_text(305-120, y-8, STRMGR_GetStr(STR_GM_LCASTLE), DKBROWN);
					gprint_text(305-100, y-8, STRMGR_GetStr(STR_GM_LLAW), DKBROWN);
					gprint_text(305-80,  y-8, STRMGR_GetStr(STR_GM_LTEMPLE), DKBROWN);
					gprint_text(305-60,  y-8, STRMGR_GetStr(STR_GM_LGUILD), DKBROWN);
					gprint_text(305-40,  y-8, STRMGR_GetStr(STR_GM_LSOURCE), DKBROWN);
					gprint_text(305-20,  y-8, STRMGR_GetStr(STR_GM_LREGENCY), DKBROWN);
					gprint_text(305,	y-8, STRMGR_GetStr(STR_GM_LTAX), DKBROWN);
				}
			}

			init_gfont(FONT_SANS_8PT);
			cOccupiedProv = 0;
			cCastles = 0;
			cProvHolds = 0;
			if (!fAfterTaxRolls) realm_tax = 0;		// locked after roll
			cProvinces = 0;
			trade_routes = 0;
			domain_power = 0;

			//--------------------------------------------------
			// loop through provs three times to sort list
			// by control class and level
			// Note: province[i].CastleSptd is used temp. as
			//			'have-holding-in-this-prov' flag
			//--------------------------------------------------
			for (h1=0; h1<2; ++h1)
			{
				for (q=0; q<3; ++q)
				{
					for (i=1; i<PROVINCE_COUNT; ++i)
					{
						if (  (q==0 && province[i].Realm==HomeRealm )
							|| (q==1 && province[i].Realm!=HomeRealm && province[i].OccupRealm==HomeRealm)
							|| (q==2 && province[i].Realm!=HomeRealm && province[i].OccupRealm!=HomeRealm && province[i].CastleSptd==TRUE) )
						{

							if (h1)
							{
								++counter;
								if(counter < startCount || lPageDone)
									lPrint = FALSE;
								else
									lPrint = TRUE;

								if (province[i].Realm==HomeRealm) ++cProvHolds;
								++cProvinces;
								if (l_map>=DATA_AREA_MIN)
								{
									if(lPrint)
									{
										// display province status symbol
										// province controller
										DrawShield(6,y,(REALM::REALM_TYPE)province[i].Realm);
		
										if (province[i].Contested)				// province contested
											DrawBitmap (6, (SHORT)y, iDCtstIc, 0, 0, 999, 999);
										else if (province[i].OccupRealm)		// occupied province
										{
											SetRemapTable(province[i].OccupRealm%LAND_REALM_COUNT);
											DrawBitmap (7, (SHORT)y, iDOcupIc, 0, 0, 999, 999);
										}
										print_textf(20, y, (province[i].Realm==HomeRealm || province[i].OccupRealm==HomeRealm)?LoyaltyColor[province[i].Loyalty]:DKBROWN, (q==2)?"^T2%s":"%s", province[i].name);
										print_textf(120, y, DKBROWN, (q==2)?"^T2%d/%d":"%d/%d", province[i].CivLevel-province[i].Pillaged,province[i].MagLevel);
									}
								}
		
								//!!!!!!!!!!!!  castle_index = ProvinceInfo((PROVINCE)i, CASTLE1_ICON);
								ProvinceInfo((PROVINCE)i, CASTLE1_ICON);
		
								if (prov_castle>0 && province[i].CastleSptd) ++cCastles;
								if (prov_law>0)  ++cProvHolds;
								if (prov_temple>0) ++cProvHolds;
								if (prov_guild>0)	++cProvHolds;
								if (province[i].OccupRealm==HomeRealm) ++cOccupiedProv;

								if (l_map==DATA_AREA_MAX)
								{
									if (dturn_mode == TAXLVL_MODE)
									{
										// if contested or occupied by someone else
										if (province[i].Contested
											|| (province[i].Realm != HomeRealm
												&& province[i].OccupRealm != HomeRealm)
											|| province[i].Pillaged)
										{
											if(lPrint)
											{
												print_textf(158, y, (province[i].Pillaged)?DKRED:DKBROWN, "-");
												print_textf(178, y, (province[i].Pillaged)?DKRED:DKBROWN, "-");
												print_textf(198, y, (province[i].Pillaged)?DKRED:DKBROWN, "-");
											}
											if ((province[i].Contested || province[i].Pillaged)
													&& province[i].TaxLevel != 0)
											{
												LOGCOMMENTSLEEP
												if ( fLogComment )
												{
													char temp[100];
													sprintf ( temp, "gamemap.cpp Contest or Pillage Auto Set - prov:%ld  TaxLevel set to 0",i);
													RandomLogComment ( temp );
												}
												SetGameData(MP_PROVINCE, MPPROV_TAXLEVEL, i, 0, TRUE );		 // minimum taxes collected
											}
										}
										else		// NOT contested
										{
											if(lPrint)
											{
												DrawBitmap (158-5, (SHORT)y, iCheck[province[i].TaxLevel==0], 0, 0, 20, 20);
												DrawBitmap (178-5, (SHORT)y, iCheck[province[i].TaxLevel==1], 0, 0, 20, 20);
												DrawBitmap (198-5, (SHORT)y, iCheck[province[i].TaxLevel==2], 0, 0, 20, 20);
												add_region(158-13, y-2, 30,12,0,AdjTaxLevel,i,0,0, STR_NULL);
												add_region(178-13, y-2, 30,12,0,AdjTaxLevel,i,1,0, STR_NULL);
												add_region(198-13, y-2, 30,12,0,AdjTaxLevel,i,2,0, STR_NULL);
											}
										}

										// if we don't control it we don't care about loyalty
										if ((province[i].OccupRealm && province[i].OccupRealm == HomeRealm)
											|| province[i].Realm==HomeRealm)
										{
											l = (province[i].Contested) ? 0 : LoyaltyAdj[province[i].TaxLevel];
											l -= (province[i].OccupRealm != 0);

											j = (province[i].OccupRealm==HomeRealm) ? prov_units : prov_law;

											if (j >= (LONG)province[i].CivLevel)	  // all law available
											{
												if(lPrint)
													DrawBitmap(254-34,(SHORT)y, iDLawL3Ic, 0, 0, 20, 20);
												if (l<0) ++l; if (l<0) ++l;
											}
											else if ((j*2)>=(LONG)province[i].CivLevel)	// half of all law
											{
												if(lPrint)
													DrawBitmap(254-34,(SHORT)y, iDLawL2Ic, 0, 0, 20, 20);
												if (l<0) ++l;
											}
											else											// less than half of law
											{
												if(lPrint)
													DrawBitmap(254-34,(SHORT)y, iDLawL1Ic, 0, 0, 20, 20);
												l = NoLawLoyaltyAdj[province[i].TaxLevel] - (province[i].OccupRealm!=0);
											}

											if(lPrint)
												print_textf(278-34, y, DKBROWN, "^L%d", l);
										}
										else
										{
											if(lPrint)
												print_textf(272-34, y, DKBROWN, "-");
										}

										if(lPrint)
										{
											//--------------------------------------------------
											// Law Claims
											//--------------------------------------------------
											law_claims = (our_law_claims * province[i].LawClaims) - their_law_claims;
											if (province[i].Pillaged)
												print_textf(268, y+5, DKRED, "^c-");
											else if (fOurLawClaims > -1)
											{
												DrawBitmap (268+1, (SHORT)y, iCheck[province[i].LawClaims], 0, 0, 20, 20);
												print_textf(268-1, y, (fTheirLawClaims>-1&&!province[i].LawClaims)?DKRED:DKBROWN, "^L%d", law_claims);
												add_region(268-5, y-2, 30,12,0,AdjLawClaims,i,0,0, STR_NULL);
											}
											else if (fTheirLawClaims > -1 && law_claims != 0)
												print_textf(268, y+5, DKRED, "^c%d", law_claims);
											else
												print_textf(268, y+5, DKBROWN, "^c-");

											if (province[i].LawClaims) prov_tax+=our_law_claims;
											prov_tax-=their_law_claims; if (prov_tax<0) prov_tax=0;

											// print province tax total
											if (province[i].Pillaged)
												print_textf(300, y, DKRED, "^LP");
											else
												print_textf(300, y, DKBROWN, (fFinalRoll)?"^L%d":"^T2^L%d", prov_tax);
										}
									}
									if (dturn_mode == MAINT_MODE)
									{
										// occupied province
										if (province[i].OccupRealm==HomeRealm)
										{
											if(lPrint)
											{
												//DrawBitmap (150, y, iCheck[province[i].OccupdSptd], 0, 0, 20, 20);
												DrawBitmap (150, (SHORT)y, iCheck[TRUE], 0, 0, 20, 20);
												//add_region(154-5, y, 30, 12, 0, PayOccupiedProv, i, 0, 0, STR_NULL);
											}
										}
										else
										{
											if(lPrint)
												print_textf(158, y, DKBROWN, "^L-");
										}
		
										if (prov_castle>0)
										{
											if(lPrint)
											{
												DrawBitmap (176, (SHORT)y, iCheck[province[i].CastleSptd], 0, 0, 20, 20);
												add_region(176-5, y, 30, 12, 0, AdjCastleSupport, i, 0, 0, STR_NULL);
												print_textf(174, y, (province[i].CastleSptd)?DKBROWN:MDRED, "^L%d", prov_castle - (province[i].CastleSptd==0));
											}
										}
										else
										{
											if(lPrint)
												print_textf(176, y, DKBROWN, "^L-");
										}

										if(lPrint)
										{
											if (prov_law>=0)	  print_textf(300-100,y, DKBROWN, "^L%d", prov_law);	else print_textf(300-100,y, DKBROWN, "^L-");
											if (prov_temple>=0) print_textf(300-80, y, DKBROWN, "^L%d", prov_temple); else print_textf(300-80, y, DKBROWN, "^L-");
											if (prov_guild>=0)  print_textf(300-60, y, DKBROWN, "^L%d", prov_guild);  else print_textf(300-60, y, DKBROWN, "^L-");
											if (prov_source>=0) print_textf(300-40, y, DKBROWN, "^L%d", prov_source); else print_textf(300-40, y, DKBROWN, "^L-");
											print_textf(300-20, y, DKBROWN, "^L%d", prov_reg);
											print_textf(300, y, DKBROWN, (fFinalRoll>=cProvinces)?"^L%d":"^T2^L%d", prov_tax);
//											if (fDoActionIcon && fFinalRoll==cProvinces-1)
//												DrawBitmap(320, (SHORT)(y+5-24), iDRollIc, 0, 0, 999, 999);
										}
									}
		
								}
		
								// display realm spell symbols
								if (l_map==DATA_AREA_MAX || l_map==DATA_AREA_MIN)
								{
									l = 99;
									if (province[i].Dispel)			 l = 0;
									else if (province[i].Warded)	 l = 3;
									else if (province[i].Blessed)	 l = 2;
									else if (province[i].Blighted) l = 1;
									else if (province[i].Honest)	 l = 4;
									if (l!=99)
									{
										if(lPrint)
										{
											print_textf((dturn_mode==MIN_MODE)?145:306, y, DKBROWN, "%c", l+1);
											print_textf((dturn_mode==MIN_MODE)?144:305, y-1, coRlmMagic[l], "%c", l+1);
										}
									}
								}

								if (!province[i].Pillaged)
								{
									trade_routes += prov_trade;
									domain_power += prov_reg;
									if (!fAfterTaxRolls)			// locked after roll
										realm_tax += prov_tax;
								}

								if(lPrint)
									y += 12;		// next line

								// print prev, next buttons if provinces are more that 21
								if(lPrint)
								{
									if(DoPageProvince(&startCount, provCount, 21, counter, 10, y))
									{
										lPrint = FALSE;
										lPageDone = TRUE;
									}
								}
							}
							else
								++provCount;

						}		// if q
					}		// for i
				}		// for q
			}		// for h1

			if (l_map==DATA_AREA_MAX)
				crease(280, y, 25, 1);
			y += 4;	 // small whitespace
		}

		if (l_map==DATA_AREA_MAX)		 // print tax collected
		{
			print_textf(190, y+3, DKBROWN, (fFinalRoll)?STRMGR_GetStr(STR_GM_TAX_COLLECTED):STRMGR_GetStr(STR_GM_TAX_COLLECTED1), realm_tax);
			y += 12;
		}

		//--------------------------------------------------
		// Allow occupation of own provinces and make choices
		// for provinces we have occupied
		//--------------------------------------------------
		if (dturn_mode == OCCUPATION_MODE && l_map==DATA_AREA_MAX)
		{
			y+= 26;
			pillage = 0;

			ProvinceInfo((PROVINCE)i, CASTLE1_ICON);
			gprint_text(10, y-10, STRMGR_GetStr(STR_GM_PROVINCE), DKBROWN);
			gprint_text(118, y-10, STRMGR_GetStr(STR_GM_LEVEL), DKBROWN);

			gprint_text(180, y-14, STRMGR_GetStr(STR_GM_C_OCCUPY ), DKBROWN);
			gprint_text(180, y-6,  STRMGR_GetStr(STR_GM_C_OWN), DKBROWN);
			gprint_text(220, y-20, STRMGR_GetStr(STR_GM_C_REDUCE), DKBROWN);
			gprint_text(220, y-13, STRMGR_GetStr(STR_GM_C_FOREIGN), DKBROWN);
			gprint_text(220, y-6,  STRMGR_GetStr(STR_GM_C_HOLDINGS), DKBROWN);
			gprint_text(260, y-14, STRMGR_GetStr(STR_GM_C_PILLAGE), DKBROWN);
			gprint_text(260, y-6,  STRMGR_GetStr(STR_GM_C_PROV), DKBROWN);
			gprint_text(285, y-10, STRMGR_GetStr(STR_GM_LOOT), DKBROWN);

			init_gfont(FONT_SANS_8PT);
			for (h1=0; h1<2; ++h1)
			{
				for (i=1; i<PROVINCE_COUNT; ++i)
				{
					if (province[i].Realm==HomeRealm || province[i].OccupRealm==HomeRealm)
					{
						if (h1)
						{
							++counter2;
							if(counter2 < startCount2 || lPageDone2)
								lPrint2 = FALSE;
							else
								lPrint2 = TRUE;

							j = province[i].TaxLevel;
							province[i].TaxLevel = 2;		// severe
							ProvinceInfo((PROVINCE)i, CASTLE1_ICON);
							province[i].TaxLevel = j;

							if (lPrint2)
							{
								// display province status symbol
								DrawShield(6,y,(REALM::REALM_TYPE)province[i].Realm);
								if (province[i].OccupRealm==HomeRealm)
								{
									SetRemapTable(HomeRealm);
									DrawBitmap (7, (SHORT)y, iDOcupIc, 0, 0, 999, 999);
								}
								print_textf(20, y, LoyaltyColor[province[i].Loyalty], "%s", province[i].name);
								print_textf(120, y, DKBROWN, "%d/%d", province[i].CivLevel-province[i].Pillaged,province[i].MagLevel);

								// draw OCCUPY OWN checkbox
								if (province[i].Realm==HomeRealm && (province[i].OccupRealm==0 || province[i].OccupRealm==HomeRealm))
								{
									DrawBitmap (180-4, (SHORT)y, iCheck[province[i].OccupRealm==HomeRealm], 0, 0, 20, 20);
									add_region (180-10, y-2, 30,12,0,AdjOccupyOwn,i,0,0, STR_NULL);
								}
								else
									print_textf(180, y+5, DKBROWN, "^c-");

								// draw REDUCE FOREIGN HOLDINGS checkbox
								if (province[i].OccupRealm==HomeRealm)
								{
									DrawBitmap (220-4, (SHORT)y, iCheck[province[i].ReduceHolds], 0, 0, 20, 20);
									add_region (220-10, y-2, 30,12,0,AdjReduceHolds,i,0,0, STR_NULL);
								}
								else
								{
									province[i].ReduceHolds = FALSE;
									print_textf(220, y+5, DKBROWN, "^c-");
								}

								// draw PILLAGE PROV. checkbox
								if (province[i].OccupRealm==HomeRealm)
								{
									DrawBitmap (260-4, (SHORT)y, iCheck[province[i].Pillaged], 0, 0, 20, 20);
									add_region (260-10, y-2, 30,12,0,AdjPillaged,i,0,0, STR_NULL);
								}
								else
								{
									province[i].Pillaged = FALSE;
									print_textf(260, y+5, DKBROWN, "^c-");
								}

								// print LOOT generated
								if (province[i].Pillaged)
								{
//!!!!!!!!!!!!!!!!!!!!!!!!! add check for number of units here
									print_textf(300, y, DKBROWN, "^L%d", prov_tax);
									pillage += prov_tax;
								}
								else
									print_textf(300, y, DKBROWN, "^L-");

								y += 12;		// next line

								// print prev, next buttons if provinces are more that 21
								if(DoPageProvince(&startCount2, provCount2, 21, counter2, 10, y))
								{
									lPrint2 = FALSE;
									lPageDone2 = TRUE;
								}
							}
						}
						else
							++provCount2;

					}		// if occupied
				}		// for i
			}		// for h1
		}

		if (l_map==DATA_AREA_MAX)
		{
			if (dturn_mode == MAINT_MODE)
				print_textf(190, y+3, DKBROWN, STRMGR_GetStr(STR_GM_OTHER_INCOME_FMT), pillage+trade_routes);
			else
			{
				print_textf(190, y+3, DKBROWN, STRMGR_GetStr(STR_GM_PILLAGE_FMT), pillage);
				y += 12;
				print_textf(190, y+3, DKBROWN, STRMGR_GetStr(STR_GM_TRADE_ROUTES), trade_routes);
			}
			y += 12;
		}

		//--------------------------------------------------
		// PAYROLL mode, print units
		//--------------------------------------------------
		if (dturn_mode == PAYRL_MODE && l_map==DATA_AREA_MAX)
		{
			y += 20;
			gprint_text(10, y-10, STRMGR_GetStr(STR_GM_PR_UNIT), DKBROWN);
			gprint_text(124, y-10, STRMGR_GetStr(STR_GM_PR_PROVINCE), DKBROWN);
			gprint_text(225, y-13, STRMGR_GetStr(STR_GM_PR_PAY), DKBROWN);
			gprint_text(225, y-6, STRMGR_GetStr(STR_GM_PR_GOLD_BARS), DKBROWN);
			gprint_text(262, y-13, STRMGR_GetStr(STR_GM_PR_PAY), DKBROWN);
			gprint_text(262, y-6, STRMGR_GetStr(STR_GM_PR_REGENCY), DKBROWN);
			gprint_text(297, y-13,STRMGR_GetStr(STR_GM_PR_MAINT), DKBROWN);
			gprint_text(297, y-6, STRMGR_GetStr(STR_GM_PR_COST), DKBROWN);

			init_gfont(FONT_SANS_8PT);
		}

		payroll = payregency = 0;
		fPayrollReduced = TRUE;
		for (h1=0; h1<2; ++h1)
		{
			for (i=1; i<MAX_UNITS; ++i)
			{
				// scan through units for leaders
				if (units[i].Realm == HomeRealm && units[i].Joined == 0)
				{
					if (h1)
					{
						++counter1;
						if(counter1 < startCount1 || lPageDone1)
							lPrint1 = FALSE;
						else
							lPrint1 = TRUE;
					}
					else
					{
						lPrint1 = FALSE;
						++provCount1;
					}

					iUnit = (SHORT)i;
					j = 0;
					while (iUnit != -1)		// list followers
					{
						if (dturn_mode == PAYRL_MODE && l_map==DATA_AREA_MAX)
						{
							if(lPrint1)
								gprint_text(j+10, y, UnitName(iUnit), (units[iUnit].Paid||units[iUnit].Use_Regency)?DKBROWN:MDRED);

							if (j)	  // draw L shaped 'follower' symbol
							{
								if(!h1)
									++provCount1;
								else
								{
									++counter1;
									if(counter1 < startCount1 || lPageDone1)
										lPrint1 = FALSE;
									else
										lPrint1 = TRUE;

									if(lPrint1)
									{
										line(13, y, 13, y+5, DKBROWN);
										line(13, y+5, 15, y+5, DKBROWN);
									}
								}
							}
							else
							{
								if(lPrint1)
									gprint_text(124, y, province[units[iUnit].province].name, LoyaltyColor[province[units[iUnit].province].Loyalty]);
							}
	
							if (UnitMaintCost[units[iUnit].Icon])
							{
								if(lPrint1)
								{
									print_textf(220, y+6, DKBROWN, (units[iUnit].Paid)?"^c%d":"^T2^c%d", UnitMaintCost[units[iUnit].Icon]);
									DrawBitmap (228, (SHORT)y, iCheck[units[iUnit].Paid], 0, 0, 20, 20);
									add_region(228-5, y, 30, 12, 0, AdjUnitPay, iUnit, 0, 0, STR_NULL);
	
									print_textf(260, y, DKBROWN, (units[iUnit].Use_Regency)?"^L%d":"^T2^L%d", UnitMaintCost[units[iUnit].Icon]*5);
									DrawBitmap (265, (SHORT)y, iCheck[units[iUnit].Use_Regency], 0, 0, 20, 20);
									add_region(265-5, y, 30, 12, 0, AdjUnitPay, iUnit, 1, 0, STR_NULL);
	
									if (units[iUnit].Paid)
										print_textf(300, y, DKBROWN, "^L%d", UnitMaintCost[units[iUnit].Icon]);
									else
										print_textf(300, y, (units[iUnit].Use_Regency)?DKBROWN:MDRED, "^L-");
								}
							}
							else if (units[iUnit].Salary)
							{
								if(lPrint1)
								{
									print_textf(220, y+6, DKBROWN, (units[iUnit].Paid)?"^c%d":"^T2^c%d", units[iUnit].Salary);
									DrawBitmap (228, (SHORT)y, iCheck[units[iUnit].Paid], 0, 0, 20, 20);
									add_region(228-5, y, 30, 12, 0, AdjUnitPay, iUnit, 2, 0, STR_NULL);
	
									print_textf(260, y, DKBROWN, (units[iUnit].Use_Regency)?"^L%d":"^T2^L%d", units[iUnit].Salary*5);
									DrawBitmap (265, (SHORT)y, iCheck[units[iUnit].Use_Regency], 0, 0, 20, 20);
									add_region(265-5, y, 30, 12, 0, AdjUnitPay, iUnit, 3, 0, STR_NULL);
	
									if (units[iUnit].Paid)
										print_textf(300, y, DKBROWN, "^L%d", units[iUnit].Salary);
									else
										print_textf(300, y, (units[iUnit].Use_Regency)?DKBROWN:MDRED, "^L-");
								}
							}
							else		// cost nothing
							{
								units[iUnit].Paid = TRUE;
								if(lPrint1)
								{
									print_textf(220, y+6, DKBROWN, "^c-");
									print_textf(260, y, DKBROWN, "^L-");
									print_textf(300, y, DKBROWN, "^L-");
								}
							}

							if(lPrint1)
								y += 12;		// next line

							// print prev, next buttons if provinces are more that 18
							if(lPrint1)
							{
								if(DoPageProvince(&startCount1, provCount1, 18, counter1, 10, y))
								{
									lPrint1 = FALSE;
									lPageDone1 = TRUE;
								}
							}
						}

						if (h1)
						{
							if (units[iUnit].Paid)
							{
								payroll += UnitMaintCost[units[iUnit].Icon] + units[iUnit].Salary;
								if ( ((UnitMaintCost[units[iUnit].Icon]*5)+(units[iUnit].Salary*5)) <= realm_reg )
									fPayrollReduced = FALSE;
							}
							if (units[iUnit].Use_Regency)
								payregency += (UnitMaintCost[units[iUnit].Icon] * 5) + (units[iUnit].Salary * 5);
						}
						iUnit = units[iUnit].NextUnit;
						j = 7;
					}
				}
			}
		}

		if (dturn_mode == PAYRL_MODE && l_map==DATA_AREA_MAX)
		{
			crease(280, y, 25, 1);
			y += 4;	 // small whitespace
		}
		if (l_map==DATA_AREA_MAX)		 // print payroll total
		{	 print_textf(190, y+3, DKBROWN, STRMGR_GetStr(STR_GM_PAYROLL),	payroll); y += 12;}

		//--------------------------------------------------
		// MAINT mode, print maintenance cost calculation
		//--------------------------------------------------
		for (tribute=0,i=1; i<REALM::REALM_COUNT; ++i)
		{
			tribute -= (LONG)Tribute[HomeRealm][i];
			tribute += (LONG)Tribute[i][HomeRealm];
		}
		domain_maint = ((cProvHolds>100)?(cProvHolds/3):MaintCost[cProvHolds]) + cCastles + cOccupiedProv + CourtCost[realm[HomeRealm].mfGetCourt()] - tribute;

		if (l_map==DATA_AREA_MAX)
		{
			if (dturn_mode == MAINT_MODE)
			{
				CHAR	temp1[80];
				CHAR	temp2[80];
				line(158-4, y-40, 158-4, y-40+58, DKBROWN);	// � occupied
				line(158-4, y+18, 182,	y+18,	DKBROWN);
				line(177-4, y-40, 177-4, y-40+46, DKBROWN);	// � castles
				line(177-4, y+6,  182,	y+6,	DKBROWN);
				print_textf(190, y+3, DKBROWN, STRMGR_GetStr(STR_GM_MAINT_CASTLES), cCastles);
				y += 12;
				print_textf(190, y+3, DKBROWN, STRMGR_GetStr(STR_GM_MAINT_OCCU), cOccupiedProv);
				y += 12;
				print_textf(190, y+3, DKBROWN, STRMGR_GetStr(STR_GM_MAINT_PROV), (cProvHolds>100)?(cProvHolds/3):MaintCost[cProvHolds]);
				y += 12;
				print_textf(190, y+3, DKBROWN, STRMGR_GetStr(STR_GM_TRIBUTE_FMT), tribute);
				y += 14;
				strcpy(temp1, STRMGR_GetStr(STR_GM_MAINT_COURT));
				strcpy(temp2, STRMGR_GetStr(gsCourt[realm[HomeRealm].mfGetCourt()]));
				print_textf(190, y+3, DKBROWN, 
					temp1, 
					temp2, 
					CourtCost[realm[HomeRealm].mfGetCourt()]);
			    DrawLittleButton(265, y, 10, 4, FALSE);
				if (realm[HomeRealm].mfGetCourt() < 4)
				{
				    print_text_centered(271, y+2, "+", DKBROWN);
				    add_region(265, y-1, 12, 6, 0, AdjCourt, +1, 0, 0, STR_NULL);
				}
				else
				    color_rect(265, y, 10, 4, LTTAN-4);

			    DrawLittleButton(265, y+7, 10, 4, FALSE);
				if (realm[HomeRealm].mfGetCourt() > 0)
				{
				    print_text_centered(271, y+10, "-", DKBROWN);
				    add_region(265, y+6, 12, 6, 0, AdjCourt, -1, 0, 0, STR_NULL);
				}
				else
				    color_rect(265, y+7, 10, 4, LTTAN-4);
				y += 14;
			}
			else
			{
				print_textf(190, y+3, DKBROWN, STRMGR_GetStr(STR_GM_MAINT_MAINT),	 domain_maint);
				y += 12;
			}
		}

		//--------------------------------------------------
		// calc final treasury
		//--------------------------------------------------
		pRealm->mfSetIncome(realm_tax + law_holding_claims + trade_routes + pillage - payroll - domain_maint);

		new_treasury = (LONG)pRealm->mfGetTreasury() + ((LONG)pRealm->mfGetIncome() * 10L);

		//--------------------------------------------------
		// print realm info
		//--------------------------------------------------
		if (l_map==DATA_AREA_MAX)
		{
		    SHORT bl_extra = 0;
		    SHORT hPlayerStats = playerstats[ActiveRegent];
		    if (hPlayerStats != fERROR)
		    {
		        PTR_PLAYER_STATS pPS = (PTR_PLAYER_STATS)BLKPTR(playerstats[ActiveRegent]);
		        bl_extra = 5 * pPS->StatsMod.mfGetMod(STATSMOD::BLOODLINE_MOD);
		    }
			realm_reg = MIN(domain_power,pRegent->mfGetBL_strength()+bl_extra) + pRealm->mfGetRegency() - payregency;
			print_textf(190, 6+3,  DKBROWN, STRMGR_GetStr(STR_GM_RINFO_UNUSED), pRealm->mfGetRegency()-payregency);
			print_textf(190, 18+3, DKBROWN, (domain_power<pRegent->mfGetBL_strength()+bl_extra)?STRMGR_GetStr(STR_GM_RINFO_DOMAIN):STRMGR_GetStr(STR_GM_RINFO_DOMAIN1), domain_power);
			if (!bl_extra)
			    print_textf(190, 30+3, DKBROWN, (domain_power<pRegent->mfGetBL_strength())?STRMGR_GetStr(STR_GM_RINFO_BLOOD):STRMGR_GetStr(STR_GM_RINFO_BLOOD1), pRegent->mfGetBL_strength());
			else
			    print_textf(190, 30+3, DKBROWN, (domain_power<pRegent->mfGetBL_strength()+bl_extra)?STRMGR_GetStr(STR_GM_RINFO_BLOOD2):STRMGR_GetStr(STR_GM_RINFO_BLOOD3),
					pRegent->mfGetBL_strength(), pRegent->mfGetBL_strength() + bl_extra);

			if ((realm[HomeRealm].mfGetRegency() > (regents[realm[HomeRealm].mfGetRegent()].mfGetBL_strength() + 1))
				&& (realm_reg >= (regents[realm[HomeRealm].mfGetRegent()].mfGetBL_strength() + 1))
				&& (regents[realm[HomeRealm].mfGetRegent()].mfGetBL_strength() < 99) )
			{
			    DrawLittleButton(305, 32, 8, 6, FALSE);
			    print_textf(307, 29, DKBROWN, "+");
				add_region(305, 31, 8, 8, 0, QueryBloodlineStrength, 0, 0, 0, STR_NULL);
			}

			crease(190, 42, 115, 1);
			print_textf(190, 45+3, DKBROWN,STRMGR_GetStr(STR_GM_RINFO_NEW), realm_reg);
			crease(10, 58, 295, 1);
			print_textf(190, 62+3, DKBROWN, STRMGR_GetStr(STR_GM_RINFO_TREASURY), pRealm->mfGetTreasury());
			print_textf(10, 62, DKBROWN, "^F11%s", STRMGR_GetStr(gsMode[dturn_mode]));	// print mode title
		}

		//--------------------------------------------------
		// After tax rolls are done
		//--------------------------------------------------
		if (fDoActionIcon && fFinalRoll >= cProvinces
//				&& !fInfoOnlyDomainUI		[abc] 9/3
			)
		{
			// treasury and regency must be positive to go on
			//if (new_treasury < 0 || realm_reg < 0)			// either is invalid
			if (realm_reg < 0)		// regency must be positive to go on
				dtrn_ic_active[5] = FALSE;
			else														// if both ok
				dtrn_ic_active[5] = TRUE;

			fAfterTaxRolls = TRUE;
		}
		else
			fAfterTaxRolls = FALSE;

		if (l_map==DATA_AREA_MAX)
		{
			crease(190, y, 115, 1);
			print_textf(190, y+6, DKBROWN, (fFinalRoll>=cProvinces)?STRMGR_GetStr(STR_GM_TR_NEW):STRMGR_GetStr(STR_GM_TR_NEW1), new_treasury);
			crease(190, y+16, 115, 1);
		}

		// roll taxes
//		if (!fDontAdvance && fDoActionIcon && fFinalRoll <= cProvinces && l_map==DATA_AREA_MAX)
		if (fDoActionIcon && fFinalRoll <= cProvinces && l_map==DATA_AREA_MAX
//				&& !fInfoOnlyDomainUI	[abc] 9/3
			)
		{
			++fFinalRoll;
			fRedrawAll = TRUE;
//			fDontAdvance = TRUE;
//			ShowFor(3,&fDontAdvance);
			if (fFinalRoll == 1)
			{
//				reqSound = SND_UI_DICE1;
				if (fCanDoPushRegions)
				{
					push_regions();
					fCanDoPushRegions = !fCanDoPushRegions;
				}
			}
			if (fFinalRoll == cProvinces)
			{
				fDepressIcon = FALSE;
				if (!fCanDoPushRegions)
				{
					pop_regions();
					fCanDoPushRegions = !fCanDoPushRegions;
				}

				if (fControlMode == 0)
				{
					SwitchToActionMode(0,0);
				}
			}
		}

	}
	ClearRemapTable();
	return FALSE;
}

/* ========================================================================
	Function	- 
	Description -
	Returns	-
	======================================================================== */
void DeleteDomainRegions (void)
{
printf("DeleteDomainRegions\n");

		del_region(AdjTaxLevel,0);
		del_region(AdjCastleSupport,0);
		del_region(AdjLawClaims,0);
		del_region(AdjPillaged,0);
		del_region(AdjOccupyOwn,0);
		del_region(AdjReduceHolds,0);
		del_region(AdjUnitPay,0);
		del_region(AdjCourt,0);
//		del_region(PayOccupiedProv,0);
		del_region(QueryBloodlineStrength,0);
		del_region(TempDataAreaMode,0);
		del_region(DisplayHolding,0);
		del_region(ZoomOut,0);
		del_region(DataAreaMode,0);
		del_region(DoReport,0);
		del_region(DoTaxRolls,0);
		del_region(SwitchToActionMode,0);
		del_region(ToggleShowUnits,0);
		del_region(ToggleShowHoldings,0);
		del_region(DoAction,0);
		del_region(SelectSubUnit,0);
		del_region(MoveSubUnitUpList,0);
		del_region(JoinUnits,0);
		del_region(StartMapIntro,0);
}

void AddDomainRegions (void)
{
	printf("AddDomainRegions\n");

		del_region(TempDataAreaMode,0);
		del_region(DisplayHolding,0);
		del_region(ZoomOut,0);
		del_region(DataAreaMode,0);
		del_region(DoReport,0);
		del_region(DoTaxRolls,0);
		del_region(SwitchToActionMode,0);



//		add_region(  0,0,DATA_AREA_MIN,376,0,DataAreaMode,MAX_MODE,0,regMINIMIZED, STR_NULL);
//		add_region(DATA_AREA_MAX-32,0,32,376,0,DataAreaMode,MIN_MODE,0,regMAXIMIZED, STR_NULL);
		if(fRealmSelected || fTutorialSelected )
		{	
			add_region(422,432,32,48,0,TempDataAreaMode,(LONG)&fSelectDomains,1,regSHOW_HOLDINGS, STR_GM_TT_SELECT_DOMAIN);
			add_region(456,432,32,48,0,DisplayHolding,LAW1_ICON,0,regSHOW_HOLDINGS, STR_GM_TT_DISPLAY_LAW);
			add_region(490,432,32,48,0,DisplayHolding,GUILD1_ICON,0,regSHOW_HOLDINGS, STR_GM_TT_DISPLAY_GUILD);
			add_region(524,432,32,48,0,DisplayHolding,TEMPLE1_ICON,0,regSHOW_HOLDINGS, STR_GM_TT_DISPLAY_TEMPLE);
			add_region(558,432,32,48,0,DisplayHolding,SOURCE1_ICON,0,regSHOW_HOLDINGS, STR_GM_TT_DISPLAY_SOURCE);
			add_region(592,432,48,48,0,ZoomOut,0,0,regSHOW_HOLDINGS, STR_GM_TT_ZOOM_OUT);
			
			add_region(  9,380,48,46,0,DataAreaMode,TAXLVL_MODE,0,regADJUST_TAXES, STR_GM_TT_TAX);
			add_region( 57,380,48,46,0,DataAreaMode,OCCUPATION_MODE,0,regDOMAIN_TURN_MODE, STR_GM_TT_LAW);
			add_region(105,380,48,46,0,DataAreaMode,PAYRL_MODE,0,regDOMAIN_TURN_MODE, STR_GM_TT_PAY);
			add_region(  9,426,48,46,0,DataAreaMode,MAINT_MODE,0,regDOMAIN_TURN_MODE, STR_GM_TT_MAINT);
			add_region( 57,426,48,46,0,DoReport,0,0,regDOMAIN_TURN_MODE, STR_GM_TT_REPORT);
			add_region(105,426,48,46,0,DoTaxRolls,0,0,regTAX_ROLLS, STR_GM_TT_DOTAX);
			add_region(105,426,48,46,0,SwitchToActionMode,0,0,regSWITCH_TO_ACTIONS, STR_GM_TT_SELECT_ACTION);
		}
}

/* ========================================================================
	Function	- 
	Description -
	Returns	-
	======================================================================== */
void ReleaseDomainUI (LONG Fake)
{
	LONG	i;

	if (iLgMap!=fERROR) DisposBlock(iLgMap);		iLgMap=fERROR;
	if (iProvMap!=fERROR) SetPurge(iProvMap);		iProvMap=fERROR;
	if (iDTurnBk!=fERROR) SetPurge(iDTurnBk);		iDTurnBk=fERROR;
	if (iDTurnIc!=fERROR) SetPurge(iDTurnIc);		iDTurnIc=fERROR;
	if (iDTurnI2!=fERROR) SetPurge(iDTurnI2);		iDTurnI2=fERROR;
	if (iDShldIc!=fERROR) SetPurge(iDShldIc);		iDShldIc=fERROR;
	if (iDActnIc!=fERROR) SetPurge(iDActnIc);		iDActnIc=fERROR;
	if (iDActnI2!=fERROR) SetPurge(iDActnI2);		iDActnI2=fERROR;
	if (iDDActIc!=fERROR) SetPurge(iDDActIc);		iDDActIc=fERROR;
	if (iDeprsIc!=fERROR) SetPurge(iDeprsIc);		iDeprsIc=fERROR;
	if (iMCtrlIc!=fERROR) SetPurge(iMCtrlIc);		iMCtrlIc=fERROR;
	if (iMCtr2Ic!=fERROR) SetPurge(iMCtr2Ic);		iMCtr2Ic=fERROR;
	if (iDCtrlIc!=fERROR) SetPurge(iDCtrlIc);		iDCtrlIc=fERROR;
	if (iDCtr2Ic!=fERROR) SetPurge(iDCtr2Ic);		iDCtr2Ic=fERROR;
	if (iDCtstIc!=fERROR) SetPurge(iDCtstIc);		iDCtstIc=fERROR;
	if (iDOcupIc!=fERROR) SetPurge(iDOcupIc);		iDOcupIc=fERROR;
	if (iDRollIc!=fERROR) SetPurge(iDRollIc);		iDRollIc=fERROR;
	if (iDRollFIc!=fERROR) SetPurge(iDRollFIc);	iDRollFIc=fERROR;
	if (iDMapBx !=fERROR) SetPurge(iDMapBx );		iDMapBx =fERROR;
	if (iDLawL1Ic!=fERROR) SetPurge(iDLawL1Ic);	iDLawL1Ic=fERROR;
	if (iDLawL2Ic!=fERROR) SetPurge(iDLawL2Ic);	iDLawL2Ic=fERROR;
	if (iDLawL3Ic!=fERROR) SetPurge(iDLawL3Ic);	iDLawL3Ic=fERROR;
	if (iMap48Ic!=fERROR) SetPurge(iMap48Ic);		iMap48Ic=fERROR;
	if (iMap48I2!=fERROR) SetPurge(iMap48I2);		iMap48I2=fERROR;
	if (iCheck[0]!=fERROR) SetPurge(iCheck[0]);	iCheck[0]=fERROR;
	if (iCheck[1]!=fERROR) SetPurge(iCheck[1]);	iCheck[1]=fERROR;

	for (i=1; i<MAX_UNITS; ++i)						// purge unit icons
	{
		if (units[i].iIconBitm != fERROR)
		{
			SetPurge(units[i].iIconBitm);
			units[i].iIconBitm = fERROR;
		}
	}

	for (i=0; i<MAX_PLACES; ++i)						// purge place icons
	{
		if (places[i].iIconBitm != fERROR)
		{
			SetPurge(places[i].iIconBitm);
			places[i].iIconBitm = fERROR;
		}
	}

}

void ReleaseDomainTurn (LONG Fake)
{
	//printf("ReleaseDomainTurn\n");

	ReleaseDomainUI(0);
	DeleteDomainRegions();

	fDomainTurn = FALSE;
	//fAutoRestoreCursor = FALSE;
	HideMenu(D_GAMEBUTTON);
	//printf("Freemem leaving map level: %ld\n", ReportFreeMem(TRUE));
	//printf("Region count leaving map level: %ld\n", CountRegions());

	//add_timer(1,draw_cursor);
	fRender = TRUE;
	fFreeze = FALSE;
	fPause = FALSE;
}

/* ========================================================================
   Function    - WriteRealmStartData
	Description - like the name says
	Returns     - 
	======================================================================== */
extern BOOL fStartMultiGame;
void WriteRealmStartData ( void )
{
	FILE *fp;
	BOOL OldfStartMPG = fStartMultiGame ;
	
#ifdef _WINDOWS	
#ifndef _JUNEDEMO	
	// GEH 8/25/97
	// read multiplayer player realm start data
	fStartMultiGame = TRUE;
	
	StatReadAll();
	if (Exists("mpstart.dat"))
		remove ("mpstart.dat");
		
	fp = fopen("mpstart.dat", "wb");
	if (fp == NULL)
	{
		perror("mpstart.dat");
		fatal_error("failed writing initial conditions file");
	}
	SaveGameFiles(fp);
	fclose(fp);
#endif
#endif

	// read single player realm start data
	fStartMultiGame = FALSE;
	StatReadAll();
	
	//GEH 8/25/97 above this is new.......
	
	if (Exists("rlmstart.dat"))
		remove ("rlmstart.dat");
	fp = fopen("rlmstart.dat", "wb");
	if (fp == NULL)
	{
		perror("rlmstart.dat");
		fatal_error("failed writing initial conditions file");
	}
	SaveGameFiles(fp);
	fclose(fp);


	fStartMultiGame = OldfStartMPG;
	
}

/* ========================================================================
	Function	- InitDomainTurn
	Description -
	Returns	-
	======================================================================== */
void InitDomainTurn (LONG Fake)
{
	LONG				i, j, k, cnt;
	LONG				fLowMemory_ = fLowMemory;
	LONG				fRestrictAni_ = fRestrictAni;
	LONG				fLowResTextures_ = fLowResTextures;
	LONG				fLowResAni_ = fLowResAni;
	LONG				fMedResTextures_ = fMedResTextures;
	LONG				fMedResAni_ = fMedResAni;
	char				n[40];
	static BOOL		FirstRun	= TRUE;
	//static ULONG	old_free_mem = 0;

#if defined (_EDIT)
	return;								// [d8-09-96 JPC] can't get to map
#endif									// from editor.

	if (hSaveBkgnd != fERROR)		// throw away hSaveBkgnd (Menu.c)
	{
		DisposBlock(hSaveBkgnd);
		hSaveBkgnd = fERROR;
		cntSaveBkgnd = 0;
	}

	set_margin_size(0,0,0,0);		// restore default margin
	SetMapDraw(0,0);					// Map? We don't need no stinking adventure map
	fSelectDomains = FALSE;

	// ========================================================
	// starting map level
	// ========================================================
	if (!fDomainTurn)
	{
		if (master_game_type == GAME_ADVENTURE)
		{
			fDomainTurn = TRUE;
			RunAdvOnly();
			return;
		}
		if (master_game_type == GAME_BATTLE)
		{
			fDomainTurn = TRUE;
			RunBattlesOnly();
			return;
		}

		//printf("InitDomainTurn\n");

		SysForceCursor();
		fRender = FALSE;
		fLowMemory = FALSE;				// temporarily load as if we don't have low memory
		fRestrictAni = FALSE;
		fLowResTextures = FALSE;
		fLowResAni = FALSE;
		fMedResTextures = FALSE;
		fMedResAni = FALSE;
		if (fControlMode & 0x100)
			fControlMode &= 0xFF;

		//remove_timer(draw_cursor);

		// --- check on memory condition
		//printf("Freemem before map level: %ld\n", ReportFreeMem(TRUE));
		//if (ReportFreeMem(TRUE) != old_free_mem)
		//	PrintMemList();
		//old_free_mem = ReportFreeMem(TRUE);
		//printf("Region count before map level: %ld\n", CountRegions());

		// ===============================
		// load map graphic if necessary
		// ===============================
		//printf("Start loading map graphics\n");
		if (fMapAltered || iLgMap == fERROR)
		{
			ReleaseDomainUI(0);
printf("GAMEMAP.CPP - InitDomainTurn: load large map\n");

#ifdef _WINDOWS
			// Always small map in multiplayer otherwise we go out of sync
			if ( IsMultiPlayer() )
			{
				iLgMap = GetResourceStd("MAP\\SMMAP.PCX", FALSE);
				fLgMap = SMALL;
			}
			else
#endif
			{
				if (fSmallMap)		// low memory requires a smaller map
				{
					iLgMap = GetResourceStd ("MAP\\SMMAP.PCX", FALSE);
					fLgMap = SMALL;
				}
				else					// bigger Large Map
				{
					iLgMap = GetResourceStd ("MAP\\LGMAP.PCX", FALSE);
					fLgMap = LARGE;
				}
			}
			SetModifyResource(iLgMap);
			fMapAltered = FALSE;
			fRedrawRoads = TRUE;
		}

		iProvMap = GetResourceStd("MAP\\PROVMAP.PCX", FALSE);
		iDTurnBk = GetResourceStd("UI\\DTURN_BK.PCX", FALSE);
		iDTurnIc = GetResourceStd("UI\\DTURN_IC.PCX", FALSE);
		iDTurnI2 = GetResourceStd("UI\\DTRN2_IC.PCX", FALSE);
		iDShldIc = GetResourceStd("UI\\DSHLD_IC.PCX", FALSE);
		LoadControlSet();
		iDDActIc = GetResourceStd("UI\\DDACT_IC.PCX", FALSE);
		iDeprsIc = GetResourceStd("UI\\DEPRS_IC.PCX", FALSE);
		iMCtrlIc = GetResourceStd("UI\\MCTRL_IC.PCX", FALSE);
		iMCtr2Ic = GetResourceStd("UI\\MCTR2_IC.PCX", FALSE);
		iDCtrlIc = GetResourceStd("UI\\DCTRL_IC.PCX", FALSE);
		iDCtr2Ic = GetResourceStd("UI\\DCTR2_IC.PCX", FALSE);
		iDCtstIc = GetResourceStd("UI\\DCTST_IC.PCX", FALSE);
		iDOcupIc = GetResourceStd("UI\\DOCUP_IC.PCX", FALSE);
		iDRollIc = GetResourceStd("UI\\DROLL_IC.PCX", FALSE);
		iDRollFIc= GetResourceStd("UI\\DROLL_F.PCX", FALSE);
		iDMapBx  = GetResourceStd("UI\\DMAP_BX.PCX",  FALSE);
		iDLawL1Ic= GetResourceStd("UI\\DLAW1_IC.PCX", FALSE);
		iDLawL2Ic= GetResourceStd("UI\\DLAW2_IC.PCX", FALSE);
		iDLawL3Ic= GetResourceStd("UI\\DLAW3_IC.PCX", FALSE);
		iMap48Ic = GetResourceStd("UI\\DMAP_IC.PCX", FALSE);
		iMap48I2 = GetResourceStd("UI\\DMAP2_IC.PCX", FALSE);
		iCheck[0]= GetResourceStd("UI\\DCHECK1.PCX", FALSE);
		iCheck[1]= GetResourceStd("UI\\DCHECK2.PCX", FALSE);
#if fUSE_RES_FILES
		SetPurge(iProvMap);
		SetPurge(iDTurnBk);
		SetPurge(iDTurnIc);
		SetPurge(iDTurnI2);
		SetPurge(iDShldIc);
		SetPurge(iDDActIc);
		SetPurge(iDeprsIc);
		SetPurge(iMCtrlIc);
		SetPurge(iMCtr2Ic);
		SetPurge(iDCtrlIc);
		SetPurge(iDCtr2Ic);
		SetPurge(iDCtstIc);
		SetPurge(iDOcupIc);
		SetPurge(iDRollFIc);
		SetPurge(iDMapBx);
		SetPurge(iDLawL1Ic);
		SetPurge(iDLawL2Ic);
		SetPurge(iDLawL3Ic);
		SetPurge(iMap48Ic);
		SetPurge(iMap48I2);
		SetPurge(iCheck[0]);
		SetPurge(iCheck[1]);
#endif

		//printf("Start loading province data\n");

		for (j=1; j<PROVINCE_COUNT; ++j)
		{
			//printf("%s\n",province[j].name);

			// find out how many places are here
			cnt = 0;
			CheckAndFixPlaces((PROVINCE)j);
			i = k = province[j].FirstPlace;
			while (k != 0)
			{
				k = places[i].NextPlace;
				i = k + province[j].FirstPlace;
				++cnt;
			}

			// auto set FirstPlace
			if (fVeryFirstTime)
				if ((j+1) < PROVINCE_COUNT)
					province[j+1].FirstPlace = province[j].FirstPlace + cnt;

			// scan through the places in the province and load the art
			iplace_coord = random(9-(cnt%10));

			province[j].CastleSptd = FALSE;
			CheckAndFixPlaces((PROVINCE)j);
			i = k = province[j].FirstPlace;
			while (k != 0)
			{
				// flag reused for: we have influence in this province
				if (places[i].Realm == HomeRealm)
					province[j].CastleSptd = TRUE;

				if (places[i].Icon != NO_MAP_ICON)
				{
					// awnsheigh strongholds
					if (places[i].Icon==CASTLE2_ICON || places[i].Icon==CASTLE3_ICON || places[i].Icon==CASTLE4_ICON)
					{
						sprintf(n,"UI\\%s.PCX",GameIcons[places[i].Icon+3]);
					}
					else if (places[i].Icon==VILLAGE_ICON	||
					         places[i].Icon==SEAPORT_ICON	||
					         places[i].Icon==SEAPORT2_ICON	||
					         places[i].Icon==PORT_ICON		||
							 places[i].Icon==WIZTOWER_ICON	||
					         places[i].Icon==RUINS_ICON)
					{
						// Icon stays the same for all levels.
						sprintf(n,"UI\\%s.PCX",GameIcons[places[i].Icon]);
					}
					else
					{
						sprintf(n,"UI\\%s.PCX",GameIcons[places[i].Icon+(((places[i].Level<8)?places[i].Level:7)/2)]);
					}

					places[i].iIconBitm = GetResourceStd(n, FALSE);
					#if fUSE_RES_FILES
					SetPurge(places[i].iIconBitm);
					#endif
					if (places[i].x == 0)
					{
						places[i].x = (SHORT)(province[j].x + place_x[iplace_coord]);
						places[i].y = (SHORT)(province[j].y + place_y[iplace_coord] - 10);
						iplace_coord = (iplace_coord+1) % 10;
					}
				}
				else
					places[i].iIconBitm = fERROR;

				k = places[i].NextPlace;
				i = k + province[j].FirstPlace;
			}
		}

		// draw the borders onto the map itself, faster that way
		if (iLgMap != fERROR)
			DrawBorders (0, 0, 0, 0, ((BITMPTR)BLKPTR(iLgMap))->w, ((BITMPTR)BLKPTR(iLgMap))->h, FULL_SCALE);

		for (i=1; i<MAX_UNITS; ++i)
			if (units[i].Icon != NO_MAP_ICON)
			{
				sprintf(n,"UI\\%s.PCX",GameIcons[units[i].Icon]);
				units[i].iIconBitm = GetResourceStd(n, FALSE);
				#if fUSE_RES_FILES
				SetPurge(units[i].iIconBitm);
				#endif
			}
			else
				units[i].iIconBitm = fERROR;

		// check common error conditions
		if (iLgMap==fERROR)
			fatal_error("ERROR: gamemap.cpp - unable to load iLgMap\n");
		if (iProvMap==fERROR)
			fatal_error("ERROR: gamemap.cpp - unable to load iProvMap\n");
		if (iDTurnBk==fERROR)
			fatal_error("ERROR: gamemap.cpp - unable to load iDTurnBk\n");
		if (iDTurnIc==fERROR)
			fatal_error("ERROR: gamemap.cpp - unable to load iDTurnIc\n");

		AddDomainRegions();
		if(fRealmSelected || fTutorialSelected )
			ActivateControlSet();

		if (dturn_mode == START_NEW_OR_LOAD)
		{
			ACTIVATE_REGION_(regDOMAIN_TURN_MODE	, TRUE);		// domain turn mode ON
			ACTIVATE_REGION_(regACTION_MODE			, FALSE);	// action mode OFF
			ACTIVATE_REGION_(regTAX_ROLLS				, TRUE);		// tax rolls ON
			ACTIVATE_REGION_(regSWITCH_TO_ACTIONS	, FALSE);	// switch to actions OFF
			ACTIVATE_REGION_(regADJUST_TAXES			, TRUE);		// adjust taxes ON
			DataAreaMode(MIN_MODE, 1);
			
		}
		else
		{
			fFinalRoll = FALSE;
			fDoActionIcon = FALSE;
			dturn_mode = ACTN_MODE;
			max_scale = AMAX_SCALE;
			req_l_map = DATA_AREA_ACTN;
#if PARCHMENT_SOUND
			reqSound = SND_UI_STATUS_REPORT;
#endif
			if (map_scale > MAX_SCALE)
				desired_scale = MAX_SCALE;
			dtrn_ic_active[0] = TRUE;		 // adjust taxes hilight
			ACTIVATE_REGION_(regDOMAIN_TURN_MODE, FALSE);	// domain turn mode OFF
			ACTIVATE_REGION_(regACTION_MODE	 , TRUE);		// action mode ON
			ACTIVATE_REGION_(regTAX_ROLLS			, TRUE);		// tax rolls ON
			ACTIVATE_REGION_(regSWITCH_TO_ACTIONS,FALSE);	// switch to actions OFF
			ACTIVATE_REGION_(regADJUST_TAXES		, FALSE);		// adjust taxes OFF
		}
		activate_region(regSHOW_HOLDINGS,TRUE);

		sMenusUp = 0;
		fFadedOut = -1;
		fRedrawAll = TRUE;
		fDomainTurn = TRUE;
		fUpdateDataArea = TRUE;
		iSelectedUnit = -1;
		iProvInfo = 0;				// no info up
		iUnitInfo = 0;
		iPlaceInfo = 0;

		if (dturn_mode != ACTN_MODE)	// this should fix map breakup
			max_scale = DMAX_SCALE;

		HideMenu(D_GAMEBUTTON);
		ShowMenu(D_GAMEBUTTON);

		// handle returning from a battle
		WithdrawLosingUnit(TRUE);
	#ifdef _WINDOWS
		if (IsMultiPlayer() && fWasABattle
//				&& !fInfoOnlyDomainUI	[abc] 9/3
			)
		{
			//fSyncPending = TRUE;
			//ShowSyncWaitWindow( (iMPS_SYNC_INITIATIVE + BattleSync), 0 );
			//while (fSyncPending)
			//{
			//	MenuLoop();
			//	#ifdef _DEBUG
			//	LogFlush();
			//	#endif
			//}
			//AMultiPlayer.ResetSync( iMPS_SYNC_INITIATIVE + BattleSync );
			StartSync( iMPS_SYNC_INITIATIVE + BattleSync );
			CheckSync( (iMPS_SYNC_INITIATIVE + BattleSync), TRUE, TRUE );

			BattleSync = (BattleSync + 1) & 3;
		}
	#endif

		fLowMemory = fLowMemory_;				// DON'T REMOVE THIS
		fRestrictAni = fRestrictAni_;			// DON'T REMOVE THIS
		fLowResTextures = fLowResTextures_;	// DON'T REMOVE THIS
		fLowResAni = fLowResAni_;				// DON'T REMOVE THIS
		fMedResTextures = fMedResTextures_;	// DON'T REMOVE THIS
		fMedResAni = fMedResAni_;				// DON'T REMOVE THIS

		// --- MULTIPLAYER
		//CheckRealmStatus(0, 0, FALSE );			// update after load game
	}

	// ========================================================
	// exiting map level
	// ========================================================
	else
		ReleaseDomainTurn(0);

	//ResumeSuspendedMusic();
	//PlayTrack(REDBOOK_REALM);
}

/* ========================================================================
	Function	- SelectSubUnit
	Description -
	Returns	-
	======================================================================== */
void SelectSubUnit (LONG iUnit, LONG)
{
	LONG	l = iUnit;
	LONG	k;

	del_region(SelectSubUnit,0);
	del_region(MoveSubUnitUpList,0);

	if (mouse_button==2)			// want info
	{
		iSelectedUnit = -1;		// deselect unit
		iUnitInfo = (SHORT)iUnit;
		fRedrawAll = TRUE;
		return;
	}

	if (units[l].Realm != HomeRealm)		// can't left-mouse-click on foreign unit
	{
		reqSound = SND_UI_NOT_YOUR_TROOPS1;
		iSelectedUnit = -1;		// deselect unit
		fRedrawAll = TRUE;
		return;
	}

	iSelectedUnit = (SHORT)iUnit;

FINDLDR:
	if (units[l].Joined)				// if joined, find leader
		for (k=1; k<MAX_UNITS; ++k)
			if (units[k].NextUnit == l)
			{
				l = k;
				goto FINDLDR;
			}

	units[iUnit].province = units[l].province;
	SetGameData(MP_UNITS, MPUNITS_PROVINCE, iUnit, units[iUnit].province, TRUE);

	reqSound = CalcVoxSndObj(GameIconVOX[units[iUnit].Icon], VOX_CASTLE_INQUIRY);
	fRedrawAll = TRUE;
}
/* ======================================================================== */
void HandleMore (LONG, LONG)
{
	fRedrawAll = TRUE;
	++TutorialScreenNumber;
}
/* ========================================================================
	Function	-
	Description -
	Returns	-
	======================================================================== */
void RealmNames (LONG clipped_x, LONG clipped_y)
{
	LONG	j, temp, trans;
	LONG	x_icon, y_icon, w_icon, h_icon, l_icon, t_icon, icon_scale;
	char	n[50];

	origin_x = (SHORT)l_map;

//	icon_scale = map_scale;
	icon_scale = (((map_scale-FULL_SCALE)*8)/10)+FULL_SCALE;
	for (j=0; j<REALM::REALM_COUNT; ++j)
	{
		if (realm[j].mfExists()
			&& realm[j].mfGetNameX()
			&& (dturn_mode!=SELRLM_MODE
				|| ( realm[j].mfIsPlayable()
#ifdef _WINDOWS
					&& lRealmSelectedByPlayer[j] == -1
#endif
					)
				)
			&& j!=REALM::MONSTERS)
		{
			sprintf(n,"%s",(j==REALM::GORGONSCROWN)?STRMGR_GetStr(STR_GM_GORGONS_CROWN):realm[j].mfGetName());
			if (j==REALM::MIERES) sprintf(n,"%s",STRMGR_GetStr(STR_RLM_NAME_MIERES));
			strupr(n);
			x_icon = (((realm[j].mfGetNameX()-clipped_x) * FULL_SCALE) / map_scale);
			y_icon = (((realm[j].mfGetNameY()-clipped_y) * FULL_SCALE) / map_scale);
			temp = (FULL_SCALE*realm[j].mfGetTitleSize())/icon_scale;

			trans = 1;
			if (map_scale > (FULL_SCALE+128)) trans = 0;
			if (map_scale < (FULL_SCALE+64)) trans = 2;
			if (realm[j].mfGetTitleTrans()) ++trans;

			if (map_scale == MAX_SCALE)
				print_textf((SHORT)(x_icon+l_map), (SHORT)y_icon, DKBROWN, "^s%02d^F%02d^S%03d^c%s", (realm[j].mfGetTitleSpread())?10:6, 19, temp, n);
			else
				print_textf((SHORT)(x_icon+l_map), (SHORT)y_icon, DKBROWN, "^T%d^s%02d^F%02d^S%03d^c%s", trans, (realm[j].mfGetTitleSpread())?12:8, 12, temp, n);

		}
	}

	origin_x = 0;
}

/* ========================================================================
	Function	- 
	Description - 
	Returns	-
	======================================================================== */
BOOL CheckForCastleNeutralized (PROVINCE iProvince, LONG iMovingUnit)
{
	LONG	i, j, k, l, cUnits = 0, ccUnits;

	// check for foreign castles
	CheckAndFixPlaces(iProvince);
	j = k = province[iProvince].FirstPlace;
	while (k != 0)
	{
		//if (places[j].Icon == CASTLE1_ICON)
		//	printf("CHECK CASTLE NEUTRALIZED - %s sdisp:%c allied(%s,%s):%c\n",
		//		province[iProvince].name, (places[j].fSiteDisplayed)?'Y':'N',
		//		realm[places[j].Realm].mfGetName(), realm[units[iMovingUnit].Realm].mfGetName(),
		//		(AREALLIED(places[j].Realm, units[iMovingUnit].Realm))?'Y':'N');

		if (	(	places[j].Icon == CASTLE1_ICON
				||	places[j].Icon == CASTLE2_ICON
				||	places[j].Icon == CASTLE3_ICON
				||	places[j].Icon == CASTLE4_ICON )
			&& (places[j].fSiteDisplayed || places[j].Realm != REALM::MONSTERS)
			&& !AREALLIED(places[j].Realm, units[iMovingUnit].Realm) )
		{
			for (i=0; i<LAND_REALM_COUNT; ++i)
				if (!AREALLIED(i,places[j].Realm))
					cUnits += UnitsHere(iProvince, (REALM::REALM_TYPE)i);

			//printf("CHECK CASTLE NEUTRALIZED - %s cUnits:%d level:%d ",
			//province[iProvince].name,cUnits,places[j].Level); ccUnits=cUnits;

			cUnits--;
			l = iMovingUnit;
			while (l != -1)		// list followers
			{
				cUnits--;
				l = units[l].NextUnit;
			}

			//printf("stack:%d\n",ccUnits-cUnits);

			return (cUnits >= places[j].Level);
		}

		k = places[j].NextPlace;
		j = province[iProvince].FirstPlace + k;
	}
	return TRUE;
}

/* ========================================================================
	Function	- 
	Description - 
	Returns	-
	======================================================================== */
BOOL CheckUnit2ndMoveBlocked (LONG iMovingUnit, PROVINCE iProvince, PROVINCE iNewProv)
{
	REALM::REALM_TYPE	iRealm = (REALM::REALM_TYPE) units[iMovingUnit].Realm;
	LONG	i, j, k, cn;

	if (units[iMovingUnit].MovePart!=0)	// in case we are on the move
		return 1;

	//if (iRealm == REALM::CARIELE)
	//	printf("Unit:%d in %s, to %s - ",iMovingUnit, province[iProvince].name, province[iNewProv].name);

	// check for foreign units
	for (j=1; j<MAX_UNITS; ++j)
	{
		if (	j != iMovingUnit									// not the unit passed arg
				&& units[j].Joined==FALSE						// is leader
				&& realm[units[j].Realm].mfExists()			// realm exists
				&& UnitFinalProvince(j) == iProvince		// in this prov when done moving
				&& !AREALLIED(iRealm, units[j].Realm) )	// not ally of iMovingUnit's realm
		{
			//if (iRealm == REALM::CARIELE)
			//	printf("blocked by %d [%s]\n",j,realm[units[j].Realm].mfGetName());
			return 2;
		}
	}

	//if (iRealm == REALM::CARIELE)
	//	printf("no blocks\n");

	// check for foreign castles
	cn = CheckForCastleNeutralized(iProvince,iMovingUnit);

	//printf("CASTLE - cn:%d iNewProv:%s (%s,%d)\n",
	//cn, province[iNewProv].name, realm[province[iNewProv].Realm].name,
	//province[iNewProv].Realm==HomeRealm);

	if (!cn && province[iNewProv].Realm != iRealm
			&& province[iNewProv].OccupRealm != iRealm)
		return 3;

	return FALSE;
}

/* ========================================================================
	Function	- 
	Description - 
	Returns	-
	======================================================================== */
BOOL CheckNotWarded (LONG iMovingUnit, PROVINCE iNewProv, PROVINCE iOldProv)
{
	//return (	(province[iNewProv].Warded == 0 || province[iNewProv].DispelLvl == units[iMovingUnit].Realm)
	//		&& (province[iOldProv].Warded == 0 || province[iOldProv].DispelLvl == units[iMovingUnit].Realm));
	
	// GWP You have to check that the warded flag is set, otherwise the DispelLvl
	//     doesn't apply to the warding information.
	return !(	(province[iNewProv].Warded > 0 && province[iNewProv].DispelLvl != units[iMovingUnit].Realm)
			|| (province[iOldProv].Warded > 0 && province[iOldProv].DispelLvl != units[iMovingUnit].Realm));
}

/* ========================================================================
	Function	- 
	Description - 
	Returns	-
	======================================================================== */
void NewTutorial (BOOL fInReport)
{
	LONG	j, l, x, y;
	GAME_STRING gs=STR_NULL;

	if (fTutorialSelected && (TutorialActionNumber<5 || fInReport))
	{
		del_region(HandleMore,0);
		l = (date*5)+TutorialActionNumber;
//printf("date:%d  TutorialActionNumber:%d  TutorText[%d]\n",date,TutorialActionNumber,l);


		if (TutorialHelpMgr::HelpExists(l,TutorialScreenNumber))
		{
			gs = TutorialHelpMgr::GetHelp(l,TutorialScreenNumber);
			init_gfont(21);
			SysHideCursor();
			print_textf(0, 0, DKBROWN, STRMGR_GetStr(gs));
			if (GameSpecificGlobal_type == 5 || GameSpecificGlobal_type == 6)
			{
#if 0
				x = GameSpecificGlobal_x + (GameSpecificGlobal_w/2) - 32;
				y = GameSpecificGlobal_y + GameSpecificGlobal_h - 17 - 4;
				color_edged_rect(x,y,64,17,18);

				print_textf(x+32,y+10, MDRED,(GameSpecificGlobal_type == 5)?);
				add_region(x,y,64,17,(GameSpecificGlobal_type == 5)?D_KEY_MORE:D_KEY_DONE1,HandleMore,0,0,0,STR_NULL);
#else
				x = GameSpecificGlobal_x + (GameSpecificGlobal_w/2) - 26;
				y = GameSpecificGlobal_y + GameSpecificGlobal_h - 16 - 4;
				SHORT	iButton = fERROR;
				iButton = GetResourceStd("UI\\MARBBTNA.PCX", FALSE);
				DrawBitmap(x, y, iButton, 0, 0, 99, 99);
				SetPurge(iButton);
				iButton = fERROR;
				print_textf(x+26,y+9, MDRED,(GameSpecificGlobal_type == 5)?STRMGR_GetStr(STR_GM_TUT_BUTS_MORE):STRMGR_GetStr(STR_GM_TUT_BUTS_DONE));
				add_region(x,y,52,16,(GameSpecificGlobal_type == 5)?D_KEY_MORE:D_KEY_DONE1,HandleMore,0,0,0,STR_NULL);
#endif
			}

			// code for end the tutorial
			if (GameSpecificGlobal_type == 40)
			{
				fTutorialSelected = FALSE;
				fTutorialFirstBattle = FALSE;
				fRealmSelected = TRUE;
				for (j=0; j<6; ++j)						// reactivate everything
					dtrn_ic_active[j] = TRUE;
				for (j=0; j<19; ++j)
					dact_ic_active[j] = TRUE;
				ResetToNonTutorialAdventures();
			}
			SysShowCursor();
		}
	}

}

/* ========================================================================
	Function	- 
	Description - 
	Returns	-
	======================================================================== */
void MoveZoomMap (LONG xx, LONG yy)
{
	req_x = xx;
	req_y = yy;

//	if (map_scale != FULL_SCALE)
	if (desired_scale != FULL_SCALE)
	{
		map_x = xx;
		map_y = yy;
		req_x = req_y = 0;
//		reqSound = SND_UI_MAP_ZOOM_IN;
	}
//	else
//		reqSound = SND_UI_MAP_NAVIGATE_LOOP;

	desired_scale = FULL_SCALE;

}

/* ========================================================================
	Function	- 
	Description - 
	Returns	-
	======================================================================== */
LONG	lCurrentStartMapDraw = 0;
LONG	lTotalTimeInMapDraw = 0;
LONG	cntMapDraws = 0;

void FrameTimerStart (void)
{
	lCurrentStartMapDraw = get_time();
	++cntMapDraws;
}

void FrameTimerRestart (void)
{
	lCurrentStartMapDraw = get_time();
}

void FrameTimerEnd (void)
{
	lTotalTimeInMapDraw += (get_time() - lCurrentStartMapDraw);
}

LONG AverageTicksPerFrame (void)
{
	return (lTotalTimeInMapDraw / cntMapDraws);
}

LONG AdjustedTimeLimit (void)
{
	LONG	adj = AverageTicksPerFrame() - 3;
	if (adj < 0) adj = 0;
	adj = adj * (lTimeLimit[iTTimeLimit] / 10);	// +10% per tick-per-frame slower
	return (lTimeLimit[iTTimeLimit] + adj);
}

LONG	lSuspendedTimeLimitAdjustment = 0;
void SuspendTimeLimit (void)
{
	if (lSuspendedTimeLimitAdjustment == 0)
		lSuspendedTimeLimitAdjustment = get_time();
}

void ResumeTimeLimit (void)
{
	if (lSuspendedTimeLimitAdjustment)
		WaitingWhen += get_time() - lSuspendedTimeLimitAdjustment;
	lSuspendedTimeLimitAdjustment = 0;
}

/* ========================================================================
	Function	- 
	Description - 
	Returns	-
	======================================================================== */
BOOL		fDataAreaChanged = FALSE;

void CalcTimerBar (void)
{
	LONG	i;

	i = ((get_time() - WaitingWhen) * 216) / AdjustedTimeLimit();

	if (WaitingBar216ths != i)
	{
		WaitingBar216ths = i;
		fRedrawProcessPanel = TRUE;
		fDataAreaChanged = TRUE;
	}
}

void RedrawProcessPanel (void)
{
	if (fRedrawProcessPanel == TRUE && dturn_mode == ACTN_MODE && CurrentRealm != REALM::REALM_OUT_OF_RANGE)
	{
		#ifdef _WINDOWS
		if ( IsMultiPlayer() && realm[CurrentRealm].mfIsPlayerCtrl() && iCurrentRealmIndex > 1 )
			{ ; }		// in this case, don't print
		else
		#endif

		if (CurrentRealm == HomeRealm)
			DisplayProcess(PLAYERSTURN, PURPLE_GRAD, CurrentRealm, 0);
		else if (CurrentRealm == 0)
			DisplayProcess(WAITINGFOR, PURPLE_GRAD, CurrentRealm, WaitingCount);
		else if (realm[CurrentRealm].mfIsPlayerCtrl())
			DisplayProcess(WAITINGFOR, PURPLE_GRAD, CurrentRealm, 0);
		else if (iCurrentRealmIndex < LAND_REALM_COUNT-1)
			DisplayProcess(REALM_IS, BLUE_GRAD, CurrentRealm, npc_ai[CurrentRealm].CurrentAction);
		else
			DisplayProcess(UNITS_MOVE, GREEN_GRAD, 0, 0);

		fRedrawProcessPanel = FALSE;
	}

}

/* ========================================================================
	Function	- DomainTurnUI
	Description - Paint routine to paint the UI panels
	Returns	-
	======================================================================== */
void DomainTurnUI (void)
{
	LONG				force_x = 0;
	LONG				force_y = 0;
	LONG				i, j, k, l, temp, x, y, w, h, f, prov, dir;
	LONG				scaled_l_max, scaled_t_max;
	LONG				scaled_win_size_x, scaled_win_size_y;
	LONG				x_icon, y_icon, w_icon, h_icon, l_icon, t_icon, icon_scale;
	BITMPTR		 	p;
	PTR				bptr;
	CSTRPTR			sz;
	char				n[50];
	static int		SoundTag = fERROR;
	static LONG		dl_map = 0;
	static LONG		dx = 0;
	static LONG		dy = 0;
	static LONG		clipped_x = 0;
	static LONG		clipped_y = 0;
	static LONG		vx = 0;
	static LONG		vy = 0;
	static LONG		old_clipped_x = 9999;
	static LONG		old_clipped_y = 9999;
	static LONG		old_scale = 999;
	static BOOL		old_sMenusUp = FALSE;
	static SHORT	NavigatingMap = FALSE;
	static BOOL		fMapPosnChanged = FALSE;
	static LONG		unit_scan = 0;
	static SHORT	hPlayerStats;
#if ENHANCEDRES
	static BOOL		fEnhancedRes = FALSE;
#endif
	PTR_PLAYER_STATS pPS = NULL;
	BOOL	fRegentSeesHidden;
	BOOL	fDidTutorialScreen = FALSE;

	if ( RegentKilled )		// -- If the regent died in battle, resurrect
	{
		SHORT	sTreasure;
		SHORT	sRegency;
		SHORT	sBlood;
		SHORT regentidx = realm[HomeRealm].mfGetRegent();
		
		if ( (hPlayerStats = playerstats[realm[HomeRealm].mfGetRegent()]) != fERROR)
		{
			DumbAutoLockPtr<PLAYER_STATS> const pPS(hPlayerStats);
			pPS->mfRestoreFullHitPts();
		}
		// remove some gold and half of regency for the resurrection
		sTreasure = realm[HomeRealm].mfGetTreasury();
		sRegency = realm[HomeRealm].mfGetRegency();
		sTreasure -= 100;	// 10.0 gold bars
		if ( sTreasure < 0 )
			sTreasure = 0;
		sRegency /= 2;
		SetGameData(MP_REALM, MPRLM_TREASURY, HomeRealm, sTreasure, TRUE);
		SetGameData(MP_REALM, MPRLM_REGENCY, HomeRealm, sRegency, TRUE);
		
		// finally, reduce the regents blood line strength by one
		sBlood = regents[regentidx].mfGetBL_strength() - 1;
		if ( sBlood < 0 )
			sBlood = 0;
		SetGameData(MP_REGENT, MPREG_BL_STRENGTH, regentidx, sBlood, TRUE);
		
		fUpdateDataArea = TRUE;
		RegentKilled = FALSE;
	}

	// if "Begin New Game" selected a different type of game
	if (master_game_type == GAME_ADVENTURE || master_game_type == GAME_BATTLE)
	{
		if (!fPractice)
		{
			if (master_game_type == GAME_ADVENTURE)
			{
				RunAdvOnly();
				fInfoOnlyDomainUI = FALSE;
				return;
			}
			if (master_game_type == GAME_BATTLE)
			{
				RunBattlesOnly();
				fInfoOnlyDomainUI = FALSE;
				return;
			}
		}
		fInfoOnlyDomainUI = FALSE;
		return;
	}

	// check for multiplayer messages that require special game handling
//	if (!fInfoOnlyDomainUI)		[abc] 9/3
		HandleMultiPlayerEvents();		// calls HaveEvents

	// ========================================================
	// handle multiplayer timer
	// ========================================================
	#ifdef _WINDOWS
	if ( IsMultiPlayer() && iTTimeLimit != IX_TT_NONE &&  WaitingWhen != 0
			&& CurrentRealm == HomeRealm && lSuspendedTimeLimitAdjustment==0
//			&& !fInfoOnlyDomainUI	[abc] 9/3
		)
	{
		i = (LONG)get_time() - (LONG)WaitingWhen - AdjustedTimeLimit();

		if (i > 0 && i < (30*18) )		// run this when time is up
		{
			fAllowDoneButton = FALSE;		// don't let done happen TWICE

			if ( IsMenuActive(D_HOLDACTION) )
			{
				DoneHoldAction(0,0);
				RunMenus();
			}
			if ( IsRequestUp(D_OTHERDIPLOMACY) )		// diplomacy request up?
			{
				NeverOthersDiplomacy(0,0);
				RunRequests();
			}
			if ( IsRequestUp(D_MOREINFO) )			// oppose request up?
			{
				DoneOthersAction(0,0);
				RollOthersAction(0,0);
				RunRequests();
			}
			if ( IsMenuActive(D_QUESTION1) )		// deal menu up ?
			{
				HideMenu(D_QUESTION1);
				RunMenus();
			}
			AbortDomainAction();
		}
		CalcTimerBar();
	}
	#endif


	// ========================================================
	// refresh screen
	// ========================================================
	if (fMenuClosed && fFreezeDomainUI == FALSE)
	{
		fMenuClosed = FALSE;
		fRedrawAll = TRUE;
	}

	else if (sMenusUp)		// don't draw if menu is up
	{
		old_sMenusUp = TRUE;
		if (fFreezeDomainUI)
		{
			fInfoOnlyDomainUI = FALSE;
			return;
		}
		if (!vx && !vy &&
				((	!async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_MOVE_LEFT))
				&&	!async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_MOVE_RIGHT))
				&&	!async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_MOVE_UP))
				&&	!async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_MOVE_DOWN))
				&&	!async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_MOVE_UPLEFT))
				&&	!async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_MOVE_UPRIGHT))
				&&	!async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_MOVE_DOWNLEFT))
				&&	!async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_MOVE_DOWNRIGHT))
				) || !char_selected)
			)
		{
			NewTutorial(FALSE);
			RedrawProcessPanel();
			fInfoOnlyDomainUI = FALSE;
			return;
		}

		if (hSaveBkgnd != fERROR)		// throw away hSaveBkgnd (Menu.c)
		{
			DisposBlock(hSaveBkgnd);
			hSaveBkgnd = fERROR;
			cntSaveBkgnd = 0;
		}
	}
	
	else if (old_sMenusUp)		  // redraw if menu just cleared
	{
		fRedrawAll = TRUE;
		old_sMenusUp = FALSE;
	}
	if (AdvisorReportOn)
	{
		fInfoOnlyDomainUI = FALSE;
		return;
	}
	
	#ifdef _WINDOWS
	if (IsMultiPlayer())
		ShowChatDisplayWindow(0,0);
	#endif
	
	// detect fRegentSeesHidden value
	fRegentSeesHidden = FALSE;
	if (playerstats[realm[HomeRealm].mfGetRegent()] != fERROR)
	{
		pPS = (PTR_PLAYER_STATS)BLKPTR(playerstats[realm[HomeRealm].mfGetRegent()]);
		if (pPS != NULL)
			fRegentSeesHidden = (pPS->StatsMod.mfGetMod(STATSMOD::SEEHIDDEN_MOD) > 0);
	}
	
	// ========================================================
	// check for mouse button click
	// ========================================================
	if (mouse_click)
	{
#ifdef _DEMO
		InitAutoDemo();
#endif
		mouse_click = 0;

		// mouse_button ==1 is left ==2 is right
//		printf("mouse button: %ld\n",mouse_button);

		if(cursor_x > l_map)		// if on map
		{
			// check for clicking on map
			if (dturn_mode==SELRLM_MODE || cursor_x < 400 || cursor_y < 432)
			{
				if (iPlaceInfo || iProvInfo || iUnitInfo)	 // redraw both areas
					fUpdateDataArea=TRUE;
				iPlaceInfo = 0;		// turn off info boxs
				iProvInfo = 0;
				iUnitInfo = 0;

				// set province index
				p = (BITMPTR) BLKPTR(iProvMap);
				bptr = (PTR)p + sizeof(BITMHDR);

				//	printf("cl_x:%d crs_x:%d l_map:%d cl_y:%d crs_y:%d\n",clipped_x,cursor_x,l_map,clipped_y,cursor_y);

				iProvSelect = bptr[
						(((((cursor_x-l_map)*map_scale)/FULL_SCALE)+clipped_x)/4)
					+(	(((((cursor_y)*map_scale)/FULL_SCALE)+clipped_y)/4) *p->w) ];

				// ========================================================
				// check for selected a realm (realm selection)
				// ========================================================
				if (dturn_mode == SELRLM_MODE)
				{
//					printf("Province:%ld Realm:%ld\n",iProvSelect,province[iProvSelect].Realm);
					if (iProvSelect >= 0 && iProvSelect < PROVINCE_COUNT
//						&& HomeRealm != province[iProvSelect].Realm
						&& realm[province[iProvSelect].Realm].mfIsPlayable()
#ifdef _WINDOWS
						&& lRealmSelectedByPlayer[province[iProvSelect].Realm] == -1
#endif
						)
					{
						HomeRealm = (REALM::REALM_TYPE)province[iProvSelect].Realm;
						fRealmSelected = TRUE;
						if (iLgMap != fERROR)
							DrawBorders(0, 0, 0, 0, ((BITMPTR)BLKPTR(iLgMap))->w, ((BITMPTR)BLKPTR(iLgMap))->h, FULL_SCALE);
						fRedrawAll = TRUE;
						reqSound = SND_UI_BUTTON_CLICK;
					}
					else
						reqSound = SND_UI_NOT_PERMITTED;
					goto DoSpecial;
				}

				// ========================================================
				// can't select if not zoomed in
				// ========================================================
				if (map_scale <= FULL_SCALE)
				{

					// ========================================================
					// check for clicked on a place
					// ========================================================
					// icon_scale = (map_scale>FULL_SCALE) ? (((map_scale-FULL_SCALE)*7)/10)+FULL_SCALE : FULL_SCALE;
					// if (icon_scale < FULL_SCALE) icon_scale = FULL_SCALE;
					icon_scale = FULL_SCALE;
					for (j=0; j<MAX_PLACES; ++j)
					{
						//if (show_place_icon[places[j].Icon] && places[j].x && (i=places[j].iIconBitm) != fERROR)
						if (	(
//						        (places[j].scene!=NULL && places[j].fSiteDisplayed) ||
						        (show_place_icon[places[j].Icon] && realm[places[j].Realm].mfShowHoldings())
						        )
							&& (i=places[j].iIconBitm) != fERROR
							&& realm[places[j].Realm].mfExists()
							&& places[j].x)
						{
							w_icon = (((BITMPTR)BLKPTR(i))->w * FULL_SCALE) / icon_scale;
							h_icon = (((BITMPTR)BLKPTR(i))->h * FULL_SCALE) / icon_scale;
							x_icon = (((places[j].x+MAP_ICON_X_ADJ-clipped_x) * FULL_SCALE) / map_scale) - (w_icon/2) + l_map;
							y_icon = (((places[j].y+MAP_ICON_Y_ADJ-clipped_y) * FULL_SCALE) / map_scale) - (h_icon/2);
							if (cursor_x<x_icon || cursor_y<y_icon || cursor_x>(x_icon+w_icon-1) || cursor_y>(y_icon+h_icon-1) )
								continue;
#ifdef _DEMO
							if (mouse_button==1)
							{
								InitDomainTurn(0);
								PlayAdvFlc();
								clear_key_status(0);
								InitDomainTurn(0);
								clear_key_status(0);
								fInfoOnlyDomainUI = FALSE;
								return;
							}
#endif
							if (mouse_button==2)
							{
								iPlaceInfo = (SHORT)j;	  // turn on place info box
								ShowFor(MESSAGE_DELAY, &iPlaceInfo);
								// printf("place info: %d\n",iPlaceInfo);
								reqSound = sndHolding[HoldType(j)];
								fRedrawAll = TRUE;
								goto DoSpecial;
							}
						}
					}

					// ========================================================
					// check for clicked on a unit
					// ========================================================
					//icon_scale = (map_scale>FULL_SCALE) ? ((map_scale-FULL_SCALE)/2)+FULL_SCALE : FULL_SCALE;
					icon_scale = FULL_SCALE;
					//unit_scan++;		// NEXT unit
//printf("unit_scan: %d\n",unit_scan);
					temp = l = FALSE;
					for (k=0; k<MAX_UNITS; ++k)
					{
						++unit_scan;						// sequentially select each unit in a stack
						if (unit_scan >= MAX_UNITS)
						{
							unit_scan = 1;
							l = (iSelectedUnit > 0);	// gone over the top flag
						}
						j = unit_scan;

						//if (units[j].province && units[j].iIconBitm!=fERROR && units[j].Joined==FALSE)

						if (units[j].province && units[j].iIconBitm!=fERROR
							&& units[j].Joined==FALSE
							&& realm[units[j].Realm].mfShowUnits()
							&&	(fRegentSeesHidden || units[j].NotSpotted==FALSE || units[j].Realm==HomeRealm))

						{
							i = units[j].iIconBitm;
							//w_icon = (((BITMPTR)BLKPTR(i))->w * FULL_SCALE) / icon_scale;
							//h_icon = (((BITMPTR)BLKPTR(i))->h * FULL_SCALE) / icon_scale;
							w_icon = ((BITMPTR)BLKPTR(i))->w;
							h_icon = ((BITMPTR)BLKPTR(i))->h;
							x_icon = (((province[units[j].province].x+UNIT_ICON_X_ADJ-clipped_x) * FULL_SCALE) / map_scale) - (w_icon/2) + l_map;
							y_icon = (((province[units[j].province].y+UNIT_ICON_Y_ADJ-clipped_y) * FULL_SCALE) / map_scale) - (h_icon);
							if (cursor_x<x_icon || cursor_y<y_icon || cursor_x>(x_icon+w_icon-1) || cursor_y>(y_icon+h_icon-1) )
								continue;
							temp = TRUE;			// temp used as a flag to indicate a unit selection attempt was made
							if (mouse_button==1)
							{
#ifdef _DEMO
								TurnOffAllSounds();
								InitDomainTurn(0);
								PlayBattleFlc();
								clear_key_status(0);
								InitDomainTurn(0);
								clear_key_status(0);
								fInfoOnlyDomainUI = FALSE;
								return;
#endif
								//if (units[j].Realm == HomeRealm) // check for allowed selection
								{
									// select unit to move
									if (l)
									{
										iSelectedUnit = -1;		// deselect unit
										unit_scan = 0;
									}
									else if (iSelectedUnit == j)
										iSelectedUnit = -1;		// deselect unit
									else
									{
										iSelectedUnit = (SHORT)j;
										// speak if our troops or Awnshegh
										if (units[j].Realm == HomeRealm || (units[j].Icon >= GORGON_UNIT_ICON && units[j].Icon <= RHUOBHE_UNIT_ICON) )
											reqSound = CalcVoxSndObj(GameIconVOX[units[j].Icon], VOX_CASTLE_INQUIRY);
									}

									fRedrawAll = TRUE;
									goto DoSpecial;
								}
							}

							if (mouse_button==2)
							{
//								iSelectedUnit = (SHORT)j;
								// turn on unit info box
//								if (iSelectedUnit > 0)
//									iUnitInfo = iSelectedUnit;
//								else

								iSelectedUnit = -1;		// deselect unit
								iUnitInfo = (SHORT)j;

								//ShowFor(MESSAGE_DELAY, &iUnitInfo);
								//printf("clicked on unit info:%d  id:%d\n",iUnitInfo,units[iUnitInfo].id);
								fRedrawAll = TRUE;
								goto DoSpecial;
							}
						}
					}

					if (temp)		 // if SOME icon was selected but not a valid one
					{
						if (iSelectedUnit == -1)
						{
							reqSound = SND_UI_NOT_YOUR_TROOPS1;
							goto DoSpecial;
						}
					}

				}	 // end of  if (map_scale <= FULL_SCALE)


				// ========================================================
				// select a place on the map for map move or info or unit move
				// keywords: select unit move unit
				// ========================================================
				// select destination for unit 
               // only allow this during a players turn in MP mode

#ifdef _WINDOWS
				if ( (!IsMultiPlayer() || CurrentRealm==HomeRealm)
					&& mouse_button==1 && iSelectedUnit != -1
					&& units[iSelectedUnit].Realm == HomeRealm)
#else
				// check for unit selected
				if (mouse_button==1 && iSelectedUnit!=-1
					&& units[iSelectedUnit].Realm == HomeRealm)
#endif
				{
					//printf("Move request from %s to %s - status %s\n",realm[HomeRealm].mfName(),realm[province[iProvSelect].Realm].mfGetName(),szRealmStatus[((RealmStatus[HomeRealm][province[iProvSelect].Realm]])&0x0F));

					if (iProvSelect <= 0 || iProvSelect >= PROVINCE_COUNT)
					{
						reqSound = SND_UI_NOT_PERMITTED;
						goto DoSpecial;
					}

					// get current province from leader
					l = iSelectedUnit;
FINDLEADER:
					if (units[l].Joined)				// if joined, find leader
						for (k=1; k<MAX_UNITS; ++k)
							if (units[k].NextUnit == l)
							{
								l = k;
								goto FINDLEADER;
							}

					// check for clicked on current province
					if (units[l].province == iProvSelect)
					{
						iSelectedUnit = -1;		// deselect unit
						fRedrawAll = TRUE;
						goto DoSpecial;
					}

					if (!BorderPermitted(HomeRealm,(REALM::REALM_TYPE)province[iProvSelect].Realm,(PROVINCE)iProvSelect))
					{
						reqSound = SND_UI_YOU_CANNOT_CROSS_THAT_BORDER1;
						goto DoSpecial;
					}

					// check for adjacent destination through impassable terrain
					for (i=0; i<7; ++i)		// scan through borders
						if (province[units[l].province].Border[i].Province == iProvSelect)
							if (TerrainCost[province[units[l].province].Border[i].MoveCost] >= 10)
							{
								reqSound = SND_UI_IMPASSABLE;
								goto DoSpecial;
							}

					TargetUnit = iSelectedUnit;
					TargetProvince = (PROVINCE) iProvSelect;
					Clear_fProv();
					if (npc_move((PROVINCE) units[l].province))
					{
						reqSound = CalcVoxSndObj(GameIconVOX[units[iSelectedUnit].Icon], VOX_CASTLE_ACKNOWLEDGE);

						if (units[iSelectedUnit].Joined)	// split unit out
						{
							for (k=1; k<MAX_UNITS; ++k)
								if (units[k].NextUnit == iSelectedUnit)
								{
//									units[k].NextUnit = -1;
									SetGameData(MP_UNITS, MPUNITS_NEXTUNIT, k, -1, TRUE);
									break;
								}
//							units[iSelectedUnit].province = units[l].province;
							SetGameData(MP_UNITS, MPUNITS_PROVINCE, iSelectedUnit, units[l].province, TRUE);
//							units[iSelectedUnit].Joined = FALSE;
							SetGameData(MP_UNITS, MPUNITS_JOINED, iSelectedUnit, FALSE, TRUE);
						}

						iSelectedUnit = -1;
						fRedrawAll = TRUE;
					}
					else
					{
						l = CheckUnit2ndMoveBlocked(iSelectedUnit,(PROVINCE)units[l].province,(PROVINCE)iProvSelect);
						if (l==2)
							reqSound = SND_UI_TROOPS_ARE_ENGAGED1;
						else if (l==3)
							reqSound = SND_UI_CASTLE_BESIEGED1;
						//else
						//	reqSound = SND_UI_NOT_ADJACENT;
					}
					//printf("\n");

					goto DoSpecial;
				}

				if (mouse_button==1 && iSelectedUnit!=-1
					&& units[iSelectedUnit].Realm != HomeRealm)
				{
					reqSound = SND_UI_NOT_YOUR_TROOPS1;
					goto DoSpecial;
				}

				// ========================================================
				// move map instead
				// ========================================================
				if (mouse_button==1 || map_scale != FULL_SCALE)
				{
					req_x = clipped_x + (((cursor_x-l_map) * map_scale) / FULL_SCALE);
					req_y = clipped_y + ((cursor_y * map_scale) / FULL_SCALE);
//					printf("map coord: %ld,%ld\n",req_x,req_y);
					if (map_scale != FULL_SCALE)
					{
						map_x=req_x; map_y=req_y; req_x=req_y=0;
						reqSound = SND_UI_MAP_ZOOM_IN;
					}
					else
						reqSound = SND_UI_MAP_NAVIGATE_LOOP;
				}

				// ========================================================
				// get info on prov
				// ========================================================
				if (mouse_button==2 && map_scale == FULL_SCALE && iPlaceInfo==0
					&& iUnitInfo==0 && iProvSelect>0 && iProvSelect<PROVINCE_COUNT)
				{
					iProvInfo = iProvSelect;
					ShowFor(MESSAGE_DELAY, &iProvInfo);
					fRedrawAll = TRUE;
				}

				desired_scale = FULL_SCALE;

			}
		}

DoSpecial:
		mouse_click = 0;
	}

	// ========================================================
	// make map look good for regent select
	// ========================================================
	if (dturn_mode == SELRLM_MODE)
	{
		map_x = req_x = 950;
		map_y = req_y = 0;
		desired_scale = map_scale = max_scale = ((FULL_SCALE*1920)/470);
	}

	// ========================================================
	// handle move keys
	// ========================================================
	if (char_selected)
	{
		if (async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_MOVE_LEFT)) ||		  // scroll left
				async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_MOVE_UPLEFT)) ||
				async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_MOVE_UPRIGHT)) )
			force_x = -MAX_MAP_XY_SPEED;
		if (async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_MOVE_RIGHT)) ||		 // scroll right
				async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_MOVE_UPRIGHT)) ||
				async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_MOVE_DOWNRIGHT)) )
			force_x = MAX_MAP_XY_SPEED;
		if (async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_MOVE_UP)) ||				// scroll up
				async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_MOVE_UPLEFT)) ||
				async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_MOVE_UPRIGHT)) )
			force_y = -MAX_MAP_XY_SPEED;
		if (async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_MOVE_DOWN)) ||		  // scroll down
				async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_MOVE_DOWNLEFT)) ||
				async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_MOVE_DOWNRIGHT)) )
			force_y = MAX_MAP_XY_SPEED;
		if (async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_SCALE_DOWN)) )	  // scale down
			map_scale = (map_scale * 11) / 10;
		if (async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::MAP_SCALE_UP)) )		// scale up
			map_scale = (map_scale * 10) / 11;
	}
	if (desired_scale && desired_scale < map_scale) // autoset scale
		{map_scale = (map_scale * 10) / 11; if (desired_scale > map_scale) {map_scale = desired_scale; desired_scale = 0;}}
	if (desired_scale && desired_scale > map_scale)
		{map_scale = (map_scale * 11) / 10; if (desired_scale < map_scale) {map_scale = desired_scale; desired_scale = 0;}}

	if ((req_x==map_x && req_y==map_y) || force_x || force_y)
	{
		req_x=0; req_y=0;
		if (SoundTag > 0)
			StopASound(SND_UI_MAP_NAVIGATE_LOOP,SoundTag);
		if (reqSound==SND_UI_MAP_NAVIGATE_LOOP)
			reqSound = SoundTag = FALSE;
	}
	if (req_x) {temp=req_x-map_x; temp=(temp&&ABS(temp)>2)?temp/3:SGN(temp); dx+=SGN(temp)*3; if(ABS(dx)>MAX_MAP_XY_SPEED) dx=MAX_MAP_XY_SPEED*SGN(temp); if(ABS(dx)>ABS(temp)) dx=temp; map_x+=dx;}
	if (req_y) {temp=req_y-map_y; temp=(temp&&ABS(temp)>2)?temp/3:SGN(temp); dy+=SGN(temp)*3; if(ABS(dy)>MAX_MAP_XY_SPEED) dy=MAX_MAP_XY_SPEED*SGN(temp); if(ABS(dy)>ABS(temp)) dy=temp; map_y+=dy;}

	// check for map reposition when data area changes size
	if (req_l_map!=l_map)
	{
		temp=req_l_map-l_map;
		temp=(temp&&ABS(temp)>2)?temp/3:SGN(temp);
//		dl_map+=SGN(temp)*3;
		dl_map+=SGN(temp)*8;
//		if (ABS(dl_map)>MAX_MAP_XY_SPEED)
//			dl_map=MAX_MAP_XY_SPEED*SGN(temp);
		if (ABS(dl_map)>(MAX_MAP_XY_SPEED*3))
			dl_map=MAX_MAP_XY_SPEED*3*SGN(temp);
		if (ABS(dl_map)>ABS(temp))
			dl_map=temp;
		l_map+=dl_map;
	}
	else
		dl_map=0;

	// ========================================================
	// do acceleration / deceleration
	// ========================================================
	if (ABS(vx) > ABS(force_x))					// decel in the x
	{
		temp = (vx - force_x) / 3;
		if(temp == 0)
			vx = 0;
		else
			vx -= temp;
	}
	else
	if (ABS(vx) < ABS(force_x))					// accel in the x
	{
		vx += (force_x - vx) / 3;
		if (ABS(vx) > ABS(force_x))
			vx = force_x;
	}
	if (ABS(vy) > ABS(force_y))					// decel in the y
	{
		temp = (vy - force_y) / 3;
		if(temp == 0)
			vy = 0;
		else
			vy -= temp;
	}
	else
	if (ABS(vy) < ABS(force_y))					// accel in the y
	{
		vy += (force_y - vy) / 3;
		if (ABS(vy) > ABS(force_y))
			vy = force_y;
	}

	// add velocity
	map_x += vx;
	map_y += vy;

	// limit scale
	if (map_scale>MAX_SCALE) map_scale = MAX_SCALE;
	if (map_scale<MIN_SCALE) map_scale = MIN_SCALE;
	scaled_win_size_x = (((640-l_map) * map_scale) / FULL_SCALE);
	scaled_win_size_y = ((480 * map_scale) / FULL_SCALE);
	scaled_l_max = MAP_WIDTH - scaled_win_size_x;
	scaled_t_max = MAP_HEIGHT - scaled_win_size_y;

// I have no idea what this did
//	if (scaled_l_max < 0)
//	{
//		l_map = req_l_map = 640 - ((MAP_WIDTH * FULL_SCALE) / map_scale);
//		dl_map = 1;
//		scaled_l_max = 0;
//	}

	clipped_x = map_x - (scaled_win_size_x/2);
	clipped_y = map_y - (scaled_win_size_y/2);
	i = force_x;	// temp store forces
	j = force_y;

	// clip movement
	if (clipped_x < 0) {clipped_x = 0; vx = 0; force_x = 0;}
	if (clipped_y < 0) {clipped_y = 0; vy = 0; force_y = 0;}
	if (clipped_x > scaled_l_max) {clipped_x = scaled_l_max; vx = 0; force_x = 0;}
	if (clipped_y > scaled_t_max) {clipped_y = scaled_t_max; vy = 0; force_y = 0;}

	// movement resets auto-return-after-scaling
	if ((i || j) && map_scale<MAX_SCALE) {map_x = clipped_x+(scaled_win_size_x/2); map_y = clipped_y+(scaled_win_size_y/2);}

	if (map_scale > FULL_SCALE)		 // deselect selected unit
		iSelectedUnit = -1;

	// timer on message boxes
	TestShowFor();

	// ========================================================
	// check HERE for update conditions,
	//	later code may set update for NEXT pass
	// ========================================================
	fMapPosnChanged = (old_scale!=map_scale || old_clipped_x!=clipped_x || old_clipped_y!=clipped_y || dl_map);
	fDataAreaChanged = (dl_map || fRedrawAll || fUpdateDataArea);

	// ========================================================
	// draw map
	// ========================================================
	// GEH update the chat panel location
//	SetButtonPosition(D_CHAT_DISPLAY_WINDOW, 0, l_map, 0);
   SetChatPosition ( l_map );

#if ENHANCEDRES
	if (fMapPosnChanged || fEnhancedRes == FALSE || fRedrawAll)
	{
		FrameTimerStart();
		fUpdatePanels = TRUE;		// GEH for the chat panel
		fRedrawAll = FALSE;
		SysHideCursor();
		if (fMapPosnChanged)			// if map changed
		{
			ScaleBitmap ((SHORT)l_map, 0, iLgMap, (SHORT)((fLgMap==LARGE)?clipped_x:clipped_x/2), (SHORT)((fLgMap==LARGE)?clipped_y:clipped_y/2), (SHORT)(640-l_map), (SHORT)480, (SHORT)((fLgMap==LARGE)?map_scale:map_scale/2) );
			if (map_scale >= (FULL_SCALE<<1)) fEnhancedRes = FALSE;
		}
		else
		{
			ScaleBitmap ((SHORT)l_map, 0, iLgMap, (SHORT)clipped_x, (SHORT)clipped_y, (SHORT)(640-l_map), (SHORT)480, (SHORT)-map_scale);
			fEnhancedRes = TRUE;
		}
#else
	if (fMapPosnChanged || fRedrawAll)
	{
		FrameTimerStart();
		fRedrawAll = FALSE;
		SysHideCursor();
//		ScaleBitmap ((SHORT)l_map, 0, iLgMap, (SHORT)clipped_x, (SHORT)clipped_y, (SHORT)(640-l_map), (SHORT)480, (SHORT)map_scale);
		ScaleBitmap ((SHORT)l_map, 0, iLgMap, (SHORT)((fLgMap==LARGE)?clipped_x:clipped_x/2), (SHORT)((fLgMap==LARGE)?clipped_y:clipped_y/2), (SHORT)(640-l_map), (SHORT)480, (SHORT)((fLgMap==LARGE)?map_scale:map_scale/2) );
#endif

		// ========================================================
		// draw (print) realm magic effects
		// ========================================================
		if (map_scale <= FULL_SCALE)
		{
			origin_x = (SHORT)l_map;
			for (j=0; j<PROVINCE_COUNT; ++j)
			{
				if (province[j].Dispel)				temp = 0;
				else if (province[j].Warded)		temp = 3;
				else if (province[j].Blessed)		temp = 2;
				else if (province[j].Blighted)	temp = 1;
				else if (province[j].Honest)		temp = 4;
				else										temp = 99;

				if (temp <= 4 || province[j].Loyalty == REBEL)
				{
					x_icon = (((province[j].x+MAP_ICON_X_ADJ-clipped_x) * FULL_SCALE) / map_scale);
					y_icon = (((province[j].y+MAP_ICON_Y_ADJ-clipped_y) * FULL_SCALE) / map_scale);
					if (temp <= 4)
						print_textf((SHORT)(x_icon+l_map), (SHORT)y_icon, coRlmMagic[temp], "^F20^c%c", ('A'+temp));
					if (province[j].Loyalty == REBEL)
						print_textf((SHORT)(x_icon+l_map), (SHORT)y_icon, RED, "^F20^c%c", 'F');
				}
			}

			// display 'at war' icons for all at war realms
			for (j=1; j<LAND_REALM_COUNT; ++j)
				for (l=j+1; l<LAND_REALM_COUNT; ++l)
					// 0x40 is REALM_ADJACIENT bit
					if (  ((RealmStatus[j][l])&0x4F)==STATUS_AT_WAR_ADJ
						|| ((RealmStatus[l][j])&0x4F)==STATUS_AT_WAR_ADJ )
					{
						x_icon = (((((realm[j].mfGetNameX()+realm[l].mfGetNameX())/2)+MAP_ICON_X_ADJ-clipped_x) * FULL_SCALE) / map_scale);
						y_icon = (((((realm[j].mfGetNameY()+realm[l].mfGetNameY())/2)+MAP_ICON_Y_ADJ-clipped_y) * FULL_SCALE) / map_scale);
						print_textf((SHORT)(x_icon+l_map), (SHORT)y_icon, RED, "^F20^c%c", 'G');
					}

			origin_x = 0;
		}

		// ========================================================
		// print realm names
		// ========================================================
		if (map_scale <= FULL_SCALE)
			RealmNames (clipped_x, clipped_y);

		// ========================================================
		// draw places
		// ========================================================
		// [MDB]
		if (dturn_mode != SELRLM_MODE)
		{
			//icon_scale = (map_scale>FULL_SCALE) ? (((map_scale-FULL_SCALE)*7)/10)+FULL_SCALE : FULL_SCALE;
			//if (icon_scale < FULL_SCALE) icon_scale = FULL_SCALE;
			icon_scale = (((map_scale - (FULL_SCALE*8/10)) * 7) / 10) + FULL_SCALE;
			if (icon_scale < FULL_SCALE) icon_scale = FULL_SCALE;

			// draw ley lines
			if (show_place_icon[SOURCE1_ICON])
			{
				temp = origin_x;
				origin_x = l_map;;
				for (j=0; j<MAX_PLACES; ++j)
				{
					if (places[j].Icon == SOURCE1_ICON
						&& (places[j].iRoute1+places[j].iRoute2+places[j].iRoute3) != 0
						&& realm[places[j].Realm].mfShowHoldings()
						&& places[j].iIconBitm != fERROR
						&& places[j].x)
					{
						for (l=0; l<3; ++l)
						{
							k = (l==0)?places[j].iRoute1: ((l==1)?places[j].iRoute2:places[j].iRoute3);
							if (k)
							{
								x_icon = (((places[j].x+MAP_ICON_X_ADJ-clipped_x) * FULL_SCALE) / map_scale)+l_map;
								y_icon = (((places[j].y+MAP_ICON_Y_ADJ-clipped_y) * FULL_SCALE) / map_scale);
								w_icon = (((places[k].x+MAP_ICON_X_ADJ-clipped_x) * FULL_SCALE) / map_scale)+l_map;
								h_icon = (((places[k].y+MAP_ICON_Y_ADJ-clipped_y) * FULL_SCALE) / map_scale);
								line(x_icon, y_icon, w_icon, h_icon, WHITE);
							}
						}
					}
				}
				origin_x = temp;
			}

			// draw trade routes
			if (show_place_icon[GUILD1_ICON])
			{
				temp = origin_x;
				origin_x = l_map;;
				for (j=0; j<MAX_PLACES; ++j)
				{
					if (places[j].Icon == GUILD1_ICON
						&& (places[j].iRoute1+places[j].iRoute2+places[j].iRoute3) != 0
						&& realm[places[j].Realm].mfShowHoldings()
						&& places[j].iIconBitm != fERROR
						&& places[j].x)
					{
						for (l=0; l<3; ++l)
						{
							k = (l==0)?places[j].iRoute1: ((l==1)?places[j].iRoute2:places[j].iRoute3);
							if (k && k < PROVINCE_COUNT)
							{
								x_icon = (((places[j].x+MAP_ICON_X_ADJ-clipped_x) * FULL_SCALE) / map_scale)+l_map;
								y_icon = (((places[j].y+MAP_ICON_Y_ADJ-clipped_y) * FULL_SCALE) / map_scale);
								w_icon = (((province[k].x+MAP_ICON_X_ADJ-clipped_x) * FULL_SCALE) / map_scale)+l_map;
								h_icon = (((province[k].y+MAP_ICON_Y_ADJ-clipped_y) * FULL_SCALE) / map_scale);
//								line(x_icon, y_icon, w_icon, h_icon, LTYELLOW);
								dashedline(x_icon, y_icon, w_icon, h_icon, (gRemapColors[places[j].Realm][0])+18, (gRemapColors[places[j].Realm][1])+18);
								dashedline(x_icon+1, y_icon, w_icon+1, h_icon, (gRemapColors[places[j].Realm][0])+18, (gRemapColors[places[j].Realm][1])+18);
								dashedline(x_icon, y_icon+1, w_icon, h_icon+1, (gRemapColors[places[j].Realm][0])+18, (gRemapColors[places[j].Realm][1])+18);
							}
						}
					}
				}
				origin_x = temp;
			}

			// draw icons
			for (j=0; j<MAX_PLACES; ++j)
			{
				if (	(
//					(places[j].scene!=NULL && places[j].fSiteDisplayed) ||
					(show_place_icon[places[j].Icon] && realm[places[j].Realm].mfShowHoldings())
					)
					&& places[j].iIconBitm != fERROR
					&& realm[places[j].Realm].mfExists()
					&& (map_scale < (MAX_SCALE/2) || places[j].Icon >= CASTLE1_ICON)
					&& places[j].x )
				{
					i = places[j].iIconBitm;
//					if (icon_scale < FULL_SCALE) icon_scale = FULL_SCALE;
					w_icon = (((BITMPTR)BLKPTR(i))->w * FULL_SCALE) / icon_scale;
					h_icon = (((BITMPTR)BLKPTR(i))->h * FULL_SCALE) / icon_scale;
					x_icon = (((places[j].x+MAP_ICON_X_ADJ-clipped_x) * FULL_SCALE) / map_scale) - (w_icon/2);
					y_icon = (((places[j].y+MAP_ICON_Y_ADJ-clipped_y) * FULL_SCALE) / map_scale) - (h_icon/2);
					if (x_icon < 0) {w_icon += x_icon; l_icon = -x_icon; x_icon = 0;} else l_icon = 0;
					if (y_icon < 0) {h_icon += y_icon; t_icon = -y_icon; y_icon = 0;} else t_icon = 0;
					if (w_icon > 0 && h_icon > 0)
						ScaleBitmap ((SHORT)(x_icon+l_map), (SHORT)y_icon, (SHORT)i, (SHORT)l_icon, (SHORT)t_icon, (SHORT)w_icon, (SHORT)h_icon, (SHORT)icon_scale);

					// province controller
					//if (icon_scale == FULL_SCALE
					if (map_scale <= FULL_SCALE
						&& places[j].Icon >= LAW1_ICON
						&& places[j].Icon <= CASTLE4_ICON
						&& w_icon > 0 && h_icon > 0 )
					{
						SetRemapTable(places[j].Realm%LAND_REALM_COUNT);
						DrawBitmap ((SHORT)(x_icon+l_map), (SHORT)(y_icon+h_icon-10), ((places[j].Realm<LAND_REALM_COUNT)?iMCtrlIc:iMCtr2Ic), 0, 0, 999, 999);
						ClearRemapTable();
						if (places[j].Contested)
							DrawBitmap ((SHORT)(x_icon+l_map), (SHORT)(y_icon+h_icon-10), iDCtstIc, 0, 0, 999, 999);
					}
				}
			}
		}

		// ========================================================
		// print province names
		// ========================================================
		if (map_scale <= (FULL_SCALE+119))
		{
			origin_x = (SHORT)l_map;
			for (j=0; j<PROVINCE_COUNT; ++j)
			{
				if (province[j].x)
				{
					temp = (province[j].Terrain==PLAINS||province[j].Terrain==HILLS) ? 3 : 2;
					if (map_scale <= FULL_SCALE+54) temp--;
					if (map_scale <= FULL_SCALE) temp--;

					sprintf(n,"%s",province[j].name);
					x_icon = (((province[j].x+MAP_ICON_X_ADJ-clipped_x) * FULL_SCALE) / map_scale);
					y_icon = (((province[j].y+MAP_ICON_Y_ADJ-clipped_y-10) * FULL_SCALE) / map_scale);

					print_textf((SHORT)(x_icon+l_map), (SHORT)y_icon, DKBROWN, "^s00^F%02d^T%d^c%s", 17, temp, n);
					print_textf((SHORT)(x_icon+l_map), (SHORT)y_icon+12, DKBROWN, "^s00^T%d^c%d/%d", temp, province[j].CivLevel-province[j].Pillaged,province[j].MagLevel);
				}
			}
			origin_x = 0;
		}


		// ========================================================
		// move and draw units
		// ========================================================
		if (dturn_mode != SELRLM_MODE)		// [MDB]
		{
			temp = (map_scale>FULL_SCALE) ? ((map_scale-FULL_SCALE)/3)+FULL_SCALE : FULL_SCALE;
			for (j=0,k=1; j<=MAX_UNITS && k; ++j)
			{
//				icon_scale = temp;
				icon_scale = FULL_SCALE;

				if (j == iSelectedUnit)	 //	don't draw in order...
					continue;
				if (j == MAX_UNITS)				 // ... instead draw selected unit last
				{
					if (iSelectedUnit > 0)
					{
						k = FALSE;
						j = iSelectedUnit;
						if (units[iSelectedUnit].Realm==HomeRealm)
							icon_scale = (FULL_SCALE*4) / 5;		// scale up selected unit
					}
					else
						continue;
				}

				if (units[j].province && (i=units[j].iIconBitm)!=fERROR
						&& (units[j].Joined == 0 || j==iSelectedUnit)
						&& realm[units[j].Realm].mfExists() )
				{
					// get from province coords
					x_icon = province[units[j].province].x;
					y_icon = province[units[j].province].y;
					dir = -1;

					// adjust unit position, use .MovePart to calc how far along
					if (units[j].DestProvince != NO_PROVINCE
//							&& !fInfoOnlyDomainUI	[abc] 9/3
						)
					{
						PROVINCE fromprov = (PROVINCE)units[j].province;
						PROVINCE destprov = (PROVINCE)units[j].DestProvince;
						REALM::REALM_TYPE invaders = (REALM::REALM_TYPE)units[j].Realm;
						REALM::REALM_TYPE defenders = (REALM::REALM_TYPE)province[destprov].Realm;
						
						// check for hostile move
						if ((units[j].MovePart == 8)
							&& ISATWAR(invaders, defenders)
							&& (province[fromprov].Realm != province[destprov].Realm))
						{
							if (defenders == HomeRealm)	// Invading us
							{
								AddReport(IMMEDIATE_NPC_MOVE_TROOP, invaders, destprov, 0);
							}
							else if (AREALLIED(defenders, HomeRealm))
							{
								BOOL atwar = FALSE;
								LONG i;
								for (i = 1; i < LAND_REALM_COUNT; i++)
								{
									if (ISATWAR(HomeRealm, i) || ISATWAR(i, HomeRealm))
									{
										atwar = TRUE;
										break;
									}
								}
					
								if (atwar)
								{
									// do nothing
								}
								else if (ISVASSAL(defenders, HomeRealm))
									AddReport(IMMEDIATE_NPC_MOVE_TROOP_ALLY, invaders, 3, defenders);
								else if (AREFULLALLIES(defenders, HomeRealm))
									AddReport(IMMEDIATE_NPC_MOVE_TROOP_ALLY, invaders, 2, defenders);
								else
									AddReport(IMMEDIATE_NPC_MOVE_TROOP_ALLY, invaders, 1, defenders);
							}
						}
						
						if (units[j].MovePart == 15)		// at end of move
						{
							units[j].MovePart = 0;
							units[j].province = prov = units[j].DestProvince;
							units[j].DestProvince = NO_PROVINCE;
							x_icon = province[units[j].province].x;
							y_icon = province[units[j].province].y;

// DONT TRANSMIT TO OTHER PLAYERS - DONT TRANSMIT TO OTHER PLAYERS
// ONLY CHANGE LOCAL - ONLY CHANGE LOCAL - ONLY CHANGE LOCAL ARRAYS
							// check for spotting units
							if (units[j].Icon == A_SCT_UNIT_ICON)
							{
								PROVINCE	thisProv = (PROVINCE)prov;
								PROVINCE	nextProv;
								LONG		k;
								
								if(CanISeeAnyoneInThisProv(HomeRealm, thisProv))
								{
									MarkUnitsSpotted(nextProv);
								}
								
								for(k=0;k<7;k++)
								{
									nextProv = (PROVINCE)province[thisProv].Border[k].Province;
									if(CanISeeAnyoneInThisProv(HomeRealm, nextProv))
									{
										MarkUnitsSpotted(nextProv);
									}
								}
							}
							else
							{
								f = CanISeeAnyoneInThisProv(HomeRealm, (PROVINCE)prov);
								if (f)
									MarkUnitsSpotted((PROVINCE)prov);
							}
// ONLY CHANGE LOCAL - ONLY CHANGE LOCAL - ONLY CHANGE LOCAL ARRAYS
// DONT TRANSMIT TO OTHER PLAYERS - DONT TRANSMIT TO OTHER PLAYERS

							fCheckUnitArrival = (SHORT)j;
						}
						else
						{
							++units[j].MovePart;		// move unit along path
							fRedrawAll = TRUE;		// keep redrawing map to allow icons to update
							dir = ((AngleFromPoint(x_icon, y_icon, province[units[j].DestProvince].x, province[units[j].DestProvince].y, RESOLUTION_4)+16) / 32) & 0x07;
							x_icon = (((LONG)units[j].MovePart * ((LONG)province[units[j].DestProvince].x - x_icon)) / 16) + x_icon;
							y_icon = (((LONG)units[j].MovePart * ((LONG)province[units[j].DestProvince].y - y_icon)) / 16) + y_icon;
						}
					}
					
					// now, check to see if unit should be drawn
					if (realm[units[j].Realm].mfShowUnits() &&
						  (fRegentSeesHidden ||
							HomeRealm == REALM::MIERES ||		// auto-play
							units[j].NotSpotted==FALSE ||
							units[j].Realm==HomeRealm))
					{
						SetRemapTable(units[j].Realm%LAND_REALM_COUNT);
						l = UnitDisplaySize[units[j].Icon];	// icon size enum

						if (temp != FULL_SCALE)				// use shield instead of figure
							{i = iMCtrlIc; l = 0;}

						if (l == 1 && dir>=0)				// small size
							{w = w_icon = 40; h = h_icon = 52;}
						else if (l == 2 && dir>=0)				// large size
							{w = w_icon = 90; h = h_icon = 80;}
						else										// auto detect w and h
						{
							w = w_icon = ((BITMPTR)BLKPTR(i))->w;
							h = h_icon = ((BITMPTR)BLKPTR(i))->h;
						}
						if (l == 2) y_icon += 3;			// move horses, etc. down a bit

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
						w_icon = (w_icon * FULL_SCALE) / icon_scale;
						h_icon = (h_icon * FULL_SCALE) / icon_scale;
						x_icon = (((x_icon+UNIT_ICON_X_ADJ-clipped_x) * FULL_SCALE) / map_scale) - (w_icon/2);
						y_icon = (((y_icon+UNIT_ICON_Y_ADJ-clipped_y) * FULL_SCALE) / map_scale) - (h_icon);
						if (l && dir>=0 && dir<=4)
							if (x_icon < 0) {w_icon += x_icon; x_icon = 0; l_icon = 0;} else l_icon = 0;
						else			// static and left to right (not mirrored)
							if (x_icon < 0) {w_icon += x_icon; l_icon = -x_icon; x_icon = 0;} else l_icon = 0;

						if ((x_icon+w_icon) > (640-l_map))
						{
							if (l && dir>=0 && dir<=4)
								l_icon += ((x_icon+w_icon) - (640-l_map));
							w_icon = (640-l_map) - x_icon;
						}

						//if (y_icon < 0) {h_icon += y_icon; t_icon = -y_icon; y_icon = 0;} else t_icon = 0;
						if (w_icon > 0 && h_icon > 0)
						{
							if (l && dir>=0)	// animated icon
							{
								sprintf(n,"UI\\%s_M.PCX",GameIcons[units[j].Icon]);
								i = GetResourceStd(n, FALSE);
								l_icon += ((units[j].MovePart & 0x03) * w);
								//l_icon = ((units[j].MovePart & 0x03) * w);
								if (dir > 4)
									dir = 8 - dir;
								else
									w_icon = -w_icon;
								//t_icon += (dir * h);
								t_icon = (dir * h);
								DrawBitmap ((SHORT)(x_icon+l_map), (SHORT)y_icon, (SHORT)i, (SHORT)l_icon, (SHORT)t_icon, (SHORT)w_icon, (SHORT)h_icon);
								SetPurge(i);
							}
							else
							{
								//if (units[j].Icon==A_KNT_UNIT_ICON)
								//	printf("DOMAINTURNUI:drawunits - scaled:%c dest:%d,%d size:%d,%d drawn:%d,%d,%d,%d \n",
								//		(icon_scale==FULL_SCALE)?'N':'Y',x_icon,y_icon, w,h, l_icon,t_icon,w_icon,h_icon);

								if (icon_scale==FULL_SCALE)
									//DrawBitmap ((SHORT)(x_icon+l_map), (SHORT)y_icon, (SHORT)i, (SHORT)l_icon, (SHORT)t_icon, (SHORT)w_icon, (SHORT)h_icon);
									DrawBitmap ((SHORT)(x_icon+l_map), (SHORT)y_icon, (SHORT)i, (SHORT)l_icon, 0, w_icon, h_icon);
								else
									//ScaleBitmap ((SHORT)(x_icon+l_map), (SHORT)y_icon, (SHORT)i, (SHORT)l_icon, (SHORT)t_icon, (SHORT)w_icon, (SHORT)h_icon, (SHORT)icon_scale);
									ScaleBitmap ((SHORT)(x_icon+l_map), (SHORT)y_icon, (SHORT)i, (SHORT)l_icon, 0, w_icon, h_icon, (SHORT)icon_scale);
							}
						}
					}

					// if we arrived in a province, cycle through all units there
					if (fCheckUnitArrival && fJoinDialog==FALSE
//							&& !fInfoOnlyDomainUI	[abc] 9/3
						)
					{
						if (CheckUnitArrival(fCheckUnitArrival,TRUE) & fSCENEBATTLE)
						{
							ClearRemapTable();
							fInfoOnlyDomainUI = FALSE;
							return;
						}
					}

					if (j == fJoinDialog)
					{
						xJoinDialog = x_icon;
						yJoinDialog = y_icon;
					}

					// ========================================================
					//	text box with contents of army: name of leader and followers
					// ========================================================
					del_region(SelectSubUnit,0);
					del_region(MoveSubUnitUpList,0);

					if (j == iSelectedUnit) // && units[j].Realm == HomeRealm)
					{
						CheckUnitStack(iSelectedUnit, TRUE);

						x = x_icon+l_map+w_icon-12;
						y = y_icon+25;
						BOOL	fHomeRealm = (units[j].Realm == HomeRealm);
						BOOL	fSubUnit = (units[j].Joined);

						init_gfont(FONT_SANS_5PT);
						//w = gtext_width(realm[units[j].Realm].mfGetName())+10;
						w = gtext_width(realm[units[j].Realm].mfGetName())+20;
						h = 12;

						l = j;
						while (l > 0)				// calc size of box
						{
							sz = UnitName((SHORT)l);
							//w = MAX(w, gtext_width(sz)+4+(6*(h>3)));
							w = MAX(w, gtext_width(sz)+20);
							h += 8;
							l = units[l].NextUnit;
						}
						if ( h > ((MAX_STACK_DEPTH*8) + 12))
							h = (MAX_STACK_DEPTH*8) + 12;

						// handle edges of screen
						if (x+w > 635)		x = 635 - w;
						if (x < l_map+5)	x = l_map + 5;
						if (y+h > 475)		y = 475 - h;
						if (y < 5)			y = 5;


						//color_rect(x, y, w, h, 223);		// lt tan
						//color_box(x, y, w, h, 223-8);		// md tan
						DrawLittleButton(x, y, w, h, 0);
						l = j;

						print_textf(x+3, y+2, BLUE, realm[units[j].Realm].mfGetName());
						y += 7;
						crease(x+3, y+2, w-4, 1);			// seperator
						y += 2;

						// print the units
						h = 0;
						while (l > 0 && h < MAX_STACK_DEPTH)
						{
							sz = UnitName((SHORT)l);
							if (l==j)
								print_textf(x+3, y+2, DKBROWN, ((units[l].Blessed)?"*%s":"%s"), sz);
							else
							{
								// draw L shaped follower symbol
								line(x+4, y+2, x+4, y+5, DKBROWN+3);
								line(x+4, y+5, x+7, y+5, DKBROWN+2);
								print_textf(x+9, y+2, DKBROWN, ((units[l].Blessed)?"*%s":"%s"), sz);
							}
							add_region(x,y+1,w,7,0,SelectSubUnit,l,0,0,STR_NULL);

							// draw little box with arrow head [v]
							if (j != l		// not on first line
								&& fHomeRealm && !fSubUnit)
							{
								color_rect(x+w-10, y+2, 9, 1, LTTAN-5);
								color_rect(x+w-10, y+3, 1, 5, LTTAN-5);
								color_rect(x+w-10, y+8, 9, 1, LTTAN-14);
								color_rect(x+w-2 , y+3, 1, 5, LTTAN-14);
								color_rect(x+w-9 , y+3, 7, 5, LTTAN-1);
								line(x+w-8, y+6, x+w-6, y+4, DKBROWN);
								line(x+w-5, y+5, x+w-4, y+6, DKBROWN);
								add_region(x+w-10, y+3, 9 ,7, 0, MoveSubUnitUpList,l,j,0,STR_NULL);
							}
							l = units[l].NextUnit;
							y += 8;
							++h;
						}
					}
				}
			}

			// ========================================================
			//	Join units dialog
			// ========================================================
#if 0
			if (fJoinDialog)
			{
				// New Tutorial Display, when join dialog is up
				if (fTutorialSelected && !fFadedOut)
				{
					NewTutorial(FALSE);
					fDidTutorialScreen = TRUE;
				}

				del_region(JoinUnits,0);
				push_regions();
				init_gfont(FONT_SANS_5PT);

				if (CalcUnitValues(fJoinDialog) > CalcUnitValues(iJoinDialog_to))
					sprintf(n, STRMGR_GetStr(STR_GM_JOIN), UnitName(iJoinDialog_to), UnitName(fJoinDialog));
				else
					sprintf(n, STRMGR_GetStr(STR_GM_JOIN), UnitName(fJoinDialog), UnitName(iJoinDialog_to));

				x = xJoinDialog + l_map + 20;
				y = yJoinDialog + 25;
				w = gtext_width(n) + 4;
				h = 24;

				// handle edges of screen
				if (x+w > 635)		x = 635 - w;
				if (x < l_map+5)	x = l_map + 5;
				if (y+h > 475)		y = 475 - h;
				if (y < 5)			y = 5;

				color_rect(x, y, w, h, 223);		// lt tan
				color_box(x, y, w, h, 223-8);		// md tan
				y+=2;
				print_textf(x+3, y, DKBROWN, n);
				y+=10;
				x = x + (w/3) - 10;
				color_rect(x, y, 20, 10, 223-4);		// lt tan
				color_box(x, y, 20, 10, 223-12);		// md tan
//				print_textf(x+11, y+6, DKBROWN, STRMGR_GetStr(STR_GM_YES));
				print_textf(x+11, y+6, DKBROWN, STRMGR_GetStr(STR_GM_FMTD_YES));
				add_region(x,y,20,10,KEY_Y,JoinUnits,fJoinDialog,iJoinDialog_to,0,STR_NULL);
				x = x + (w/3);
				color_rect(x, y, 20, 10, 223-4);		// lt tan
				color_box(x, y, 20, 10, 223-12);		// md tan
//				print_textf(x+11, y+6, DKBROWN, STRMGR_GetStr(STR_GM_NO));
				print_textf(x+11, y+6, DKBROWN, STRMGR_GetStr(STR_GM_FMTD_NO));
				add_region(x,y,20,10,KEY_N,JoinUnits,0,0,0,STR_NULL);
				fCommandBattleLoop = TRUE;
			}
#endif

		}
		ClearRemapTable();

		// ========================================================
		// print realm names
		// ========================================================
		if (map_scale > FULL_SCALE)
			RealmNames (clipped_x, clipped_y);

		// ========================================================
		// draw map control icons
		// ========================================================
		if (dturn_mode != SELRLM_MODE)
		{
			DrawBitmap (422,432, (fSelectDomains)?iMap48Ic:iMap48I2,		 22, 0, 32, 48);
			DrawBitmap (456,432, (show_place_icon[LAW1_ICON	])?iMap48Ic:iMap48I2,  56, 0, 32, 48);
			DrawBitmap (490,432, (show_place_icon[GUILD1_ICON ])?iMap48Ic:iMap48I2,  90, 0, 32, 48);
			DrawBitmap (524,432, (show_place_icon[TEMPLE1_ICON])?iMap48Ic:iMap48I2, 124, 0, 32, 48);
			DrawBitmap (558,432, (show_place_icon[SOURCE1_ICON])?iMap48Ic:iMap48I2, 158, 0, 32, 48);
			DrawBitmap (592,432, (map_scale != MAX_SCALE)?iMap48Ic:iMap48I2,	192, 0, 48, 48);
		}
		else		// put up REALM SELECTION title box
		{
			//print prompt
			SHORT hRealmSelectBox=GetResourceStd("UI\\RELMSLCT.PCX",FALSE);
			BITMPTR pRealmSelectBox=(BITMPTR)BLKPTR(hRealmSelectBox);
			LONG tlxBox=216+16;
			LONG tlyBox=0;
			LONG cxText=tlxBox+(pRealmSelectBox->w)/2;
			LONG cyText=20;

			DrawBitmap(tlxBox,tlyBox,hRealmSelectBox,0,0,999,999);
			SetPurge(hRealmSelectBox);
			hRealmSelectBox = fERROR;

			//print_text_centered(cxText,cyText,STRMGR_GetStr(STR_REALM_SELECTION),BLACK);
			#if defined (_GERMANVER)
			print_textf(cxText-170,cyText-10,BLACK,STRMGR_GetStr(STR_GM_FMTD_REALM_SEL));
			print_textf(cxText-123,cyText+20,BLACK,STRMGR_GetStr(STR_GM_CLICK_FOR_CHOICES));
			print_textf(cxText-123,cyText+32,BLACK,STRMGR_GetStr(STR_GM_TO_SELECT_REGENT));
			#else
			print_textf(cxText-140,cyText-10,BLACK,STRMGR_GetStr(STR_GM_FMTD_REALM_SEL));
			print_textf(cxText-90,cyText+20,BLACK,STRMGR_GetStr(STR_GM_CLICK_FOR_CHOICES));
			print_textf(cxText-90,cyText+32,BLACK,STRMGR_GetStr(STR_GM_TO_SELECT_REGENT));
			#endif
		}

		// ========================================================
		// put up info box
		// ========================================================
		if (iUnitInfo)
		{
			//DisplayInfo("UI\\DINF3_BK.PCX",0,STR_GM_MOVE,STRMGR_GetStr(gsUnitTitle[units[iUnitInfo].Icon]),realm[units[iUnitInfo].Realm].mfGetName(),2);
			l = units[iUnitInfo].id;
			if (l >= 1000 && l < 3000)
			{
				//printf("ShowStatus - unit:%d  id:%d\n",iUnitInfo,l);
				SetStatusRealm( (REALM::REALM_TYPE) units[iUnitInfo].Realm );
				if(units[iUnitInfo].Realm != HomeRealm)
					ShowStatus(-l, 0);				// avoid to show the invntory menu for people who is not in my realm
		  		else
					ShowStatus(l, 0);
			}
			else
			{
				//printf("InitBattleCard - unit:%d  id:%d\n",iUnitInfo,l);
				InitBattleCard(iUnitInfo);
			}
			iUnitInfo = 0;
		}
		else if (iPlaceInfo)
		{
			//STR_GM_LOCATION		^F03%s\n^F00REALM: ^F02%s\n^F00ADVENTURE LOCATION: ^F02%s
			//DisplayInfo("UI\\DINF3_BK.PCX",0,STR_GM_LOCATION,
			//	(places[iPlaceInfo].scene!=NULL)?places[iPlaceInfo].scene:STRMGR_GetStr(STR_GM_NO2));

			if (HoldType(iPlaceInfo) < 5)		// don't print holding's level
			{
				char buffer1[50];
				char buffer2[50];
				char buffer3[50];
				char buffer4[50];
				char buffer5[50];
				char buffer6[50];
				char buffer7[50];
				strcpy(buffer1,(places[iPlaceInfo].iName)? STRMGR_GetStr(STR_GM_TYPE_REALM_FMT1): STRMGR_GetStr(STR_GM_TYPE_REALM_FMT2));
				strcpy(buffer2,STRMGR_GetStr(gsHolding[HoldType(iPlaceInfo)]));
				strcpy(buffer3,
//								(places[iPlaceInfo].scene!=NULL)?STRMGR_GetStr(STR_GM_ADVENTURE_LOC):
								"");
				strcpy(buffer4,(places[iPlaceInfo].Icon==CASTLE1_ICON&&places[iPlaceInfo].iRoute1)?STRMGR_GetStr(STR_GM_UNDER_CONSTRUCTION):"");
				strcpy(buffer5,(places[iPlaceInfo].Contested)?STRMGR_GetStr(STR_GM_CONTESTED):"\0");
				strcpy(buffer6,realm[places[iPlaceInfo].Realm].mfGetName());

print_textf(0,0,BLACK,buffer6);

				strcpy(buffer7,(places[iPlaceInfo].iName)? place_names[places[iPlaceInfo].iName] : "." );

				DisplayInfoStr("UI\\DINF3_BK.PCX",0,
					buffer1,
					buffer7,
					buffer2,
					buffer6,
					buffer3,
					buffer4,
					buffer5);
			}
			else		// print the holding's level as well
			{
				char buffer0[50];
				char buffer1[50];
				char buffer2[50];
				char buffer3[50];
				char buffer4[50];
				char buffer5[50];
				strcpy(buffer1,(places[iPlaceInfo].iName)? STRMGR_GetStr(STR_GM_TYPE_REALM_FMT3): STRMGR_GetStr(STR_GM_TYPE_REALM_FMT4));
				strcpy(buffer0,(places[iPlaceInfo].iName)? place_names[places[iPlaceInfo].iName] : ".");
				strcpy(buffer2,STRMGR_GetStr(gsHolding[HoldType(iPlaceInfo)]));
				strcpy(buffer4,(places[iPlaceInfo].Icon==CASTLE1_ICON&&places[iPlaceInfo].iRoute1)?STRMGR_GetStr(STR_GM_UNDER_CONSTRUCTION):"");
				strcpy(buffer5,(places[iPlaceInfo].Contested)?STRMGR_GetStr(STR_GM_CONTESTED):"");

				strcpy(buffer3,realm[places[iPlaceInfo].Realm].mfGetName());

				DisplayInfoStr("UI\\DINF3_BK.PCX",0,
					buffer1,
					buffer0,
					buffer2,
					places[iPlaceInfo].Level,
					buffer3,
					buffer4,
					buffer5, "");
			}
		}
		else if (iProvInfo)
		{
//			STR_GM_TERRAIN		^F03%s %d/%d\n^F00REALM: ^F02%s\n^F00TERRAIN: ^F02%s
//			DisplayInfo("UI\\DINF3_BK.PCX",0,STR_GM_TERRAIN,
			char buffer[100];
			char cpProvinceSpellName[100];
			LONG provSpellCount = 0;
			strcpy(buffer,STRMGR_GetStr(STR_GM_REALM_TERR_FMT));
			cpProvinceSpellName[0] = 0;
			if (province[iProvInfo].Loyalty == REBEL)
			{
				++provSpellCount;
				strcat(cpProvinceSpellName,STRMGR_GetStr(STR_MAP_INFO_REBELLION));
			}
			if (province[iProvInfo].Warded > 0)
			{
				if (provSpellCount > 0)
					strcat(cpProvinceSpellName, ", ");
				provSpellCount += 2;
				strcat(cpProvinceSpellName,STRMGR_GetStr(STR_MAP_INFO_WARDED));
				strcat(cpProvinceSpellName," (");
				strcat(cpProvinceSpellName,realm[(REALM::REALM_TYPE)province[iProvInfo].DispelLvl].mfGetName() );
				strcat(cpProvinceSpellName,")");
			}
			if (province[iProvInfo].Blessed > 0 && provSpellCount < 3)
			{
				if (provSpellCount > 0)
					strcat(cpProvinceSpellName, ", ");
				++provSpellCount;
				strcat(cpProvinceSpellName,STRMGR_GetStr(STR_MAP_INFO_BLESSED));
			}
			if (province[iProvInfo].Blighted > 0 && provSpellCount < 3)
			{
				if (provSpellCount > 0)
					strcat(cpProvinceSpellName, ", ");
				++provSpellCount;
				strcat(cpProvinceSpellName,STRMGR_GetStr(STR_MAP_INFO_BLIGHTED));
			}
			if (province[iProvInfo].Dispel > 0 && provSpellCount < 3)
			{
				if (provSpellCount > 0)
					strcat(cpProvinceSpellName, ", ");
				++provSpellCount;
				strcat(cpProvinceSpellName,STRMGR_GetStr(STR_MAP_INFO_DISPELL));
			}
			if (province[iProvInfo].Honest > 0 && provSpellCount < 2)
			{
				// This is a long spell name, only two spells can be shown at one time.
				++provSpellCount;
				strcat(cpProvinceSpellName,STRMGR_GetStr(STR_MAP_INFO_HONEST_DEALING));
			}
			char buffer6[50];
			char buffer7[50];
			
			strcpy(buffer6,realm[province[iProvInfo].Realm].mfGetName());
			strcpy(buffer7,STRMGR_GetStr(gsTerrain[province[iProvInfo].Terrain]));
			
			DisplayInfoStr("UI\\DINF3_BK.PCX",0,
				buffer,
				province[iProvInfo].name,
				province[iProvInfo].CivLevel-province[iProvInfo].Pillaged,
				province[iProvInfo].MagLevel,
				buffer6,
				buffer7,
				cpProvinceSpellName);
		}

		fRedrawProcessPanel = TRUE;
		SysShowCursor();

		FrameTimerEnd();
	}		// end of if RedrawAll, etc.

	// ========================================================
	// update process panel
	// ========================================================
	RedrawProcessPanel();

	// ========================================================
	// if data area changed
	// ========================================================
	// fDataAreaChanged includes fUpdateDataArea
	if (fDataAreaChanged && !fCommandBattleLoop)
	{
		fUpdateDataArea = FALSE;
		SysHideCursor();

		FrameTimerRestart();
		BOOL fUDA = UpdateDataArea();
		FrameTimerEnd();

		if (fUDA)		// if TRUE then a battle started
		{
			SysShowCursor();
			fInfoOnlyDomainUI = FALSE;
			return;
		}
		SysShowCursor();
	}

	// ========================================================
	// draw depressed border for icon
	// ========================================================
	if (fDepressIcon)
		DrawBitmap (xDepressIcon, yDepressIcon, iDeprsIc, 0, 0, 48, 48);

	old_clipped_x = clipped_x;
	old_clipped_y = clipped_y;
	old_scale = map_scale;

#ifdef _DEMO
	if (fTutorialMessage)
	{
		print_textf(321, 241, DKBROWN, STRMGR_GetStr(STR_GM_SPACE));
		print_textf(320, 240, WHITE, STRMGR_GetStr(STR_GM_SPACE));
	}
#endif

	// ========================================================
	// New Tutorial Display
	// ========================================================
	if (!fDidTutorialScreen && fTutorialSelected && (fMapPosnChanged || fDataAreaChanged) && !fFadedOut)
		NewTutorial(FALSE);




			// ========================================================
			//	Join units dialog
			// ========================================================
			if (fJoinDialog)
			{
				// New Tutorial Display, when join dialog is up
				//if (fTutorialSelected && !fFadedOut)
				//{
				//	NewTutorial(FALSE);
				//	fDidTutorialScreen = TRUE;
				//}

				del_region(JoinUnits,0);
				push_regions();
				init_gfont(FONT_SANS_5PT);

				if (CalcUnitValues(fJoinDialog) > CalcUnitValues(iJoinDialog_to))
					sprintf(n, STRMGR_GetStr(STR_GM_JOIN), UnitName(iJoinDialog_to), UnitName(fJoinDialog));
				else
					sprintf(n, STRMGR_GetStr(STR_GM_JOIN), UnitName(fJoinDialog), UnitName(iJoinDialog_to));

				x = xJoinDialog + l_map + 20;
				y = yJoinDialog + 25;
				w = gtext_width(n) + 4;
				h = 24;

				// handle edges of screen
				if (x+w > 635)		x = 635 - w;
				if (x < l_map+5)	x = l_map + 5;
				if (y+h > 475)		y = 475 - h;
				if (y < 5)			y = 5;

				// handle chat box interference
				#ifdef _WINDOWS
				if (fChatLineOn)
				{
					if (ChatDisplaySize)
					{
						if (y < 128) y = 128;
					}
					else
					{
						if (y < 64) y = 64;
					}
				}
				#endif

				color_rect(x, y, w, h, 223);		// lt tan
				color_box(x, y, w, h, 223-8);		// md tan
				y+=2;
				print_textf(x+3, y, DKBROWN, n);
				y+=10;
				x = x + (w/3) - 10;
				color_rect(x, y, 20, 10, 223-4);		// lt tan
				color_box(x, y, 20, 10, 223-12);		// md tan
//				print_textf(x+11, y+6, DKBROWN, STRMGR_GetStr(STR_GM_YES));
				print_textf(x+11, y+6, DKBROWN, STRMGR_GetStr(STR_GM_FMTD_YES));
				add_region(x,y,20,10,KEY_Y,JoinUnits,fJoinDialog,iJoinDialog_to,0,STR_NULL);
				x = x + (w/3);
				color_rect(x, y, 20, 10, 223-4);		// lt tan
				color_box(x, y, 20, 10, 223-12);		// md tan
//				print_textf(x+11, y+6, DKBROWN, STRMGR_GetStr(STR_GM_NO));
				print_textf(x+11, y+6, DKBROWN, STRMGR_GetStr(STR_GM_FMTD_NO));
				add_region(x,y,20,10,KEY_N,JoinUnits,0,0,0,STR_NULL);
				fCommandBattleLoop = TRUE;
			}




	// ========================================================
	// play a requested sound
	// ========================================================
	if (reqSound)
	{
		if (NavigatingMap && NavigateSoundTag > 0)
			StopASound(SND_UI_MAP_NAVIGATE_LOOP, NavigateSoundTag);
		NavigatingMap = FALSE;

		SoundTag = AddSndObj((BIRTHRT_SND)reqSound,0,VOLUME_NINETY);
		if (reqSound == SND_UI_MAP_NAVIGATE_LOOP)
		{
			NavigateSoundTag = (SHORT)SoundTag;
			NavigatingMap = TRUE;
		}
		reqSound = FALSE;
	}

	// ========================================================
	// fade in if faded out
	// ========================================================
	if (fFadedOut > 0)
	{
		clear_key_status(0);
		fFadedOut--;
		if (fFadedOut == 0)
		{
			if (fIsFadedOut)
			{
				//GEH this is a problem in WINDOWS
				#ifndef _WINDOWS
				FadeIn(100);
				#endif
			}

			// ========================================================
			// if the regent was killed then restart the game
			// ========================================================
			if (fVeryFirstTime || RegentKilled)
			{
				RegentKilled = FALSE;
				char_selected = FALSE;
				TutorialActionNumber = 0;
				TutorialScreenNumber = 6;
				if (fVeryFirstTime)		// don't write dead regent out to disk
				{
					WriteRealmStartData();
				}
				fVeryFirstTime = FALSE;
				CopyReactionTable();

#ifdef _WINDOWS
				if ( fStartSIGS == TRUE ) 
				{
					fStartSIGS = FALSE;
					StartInSIGS();
				}
				else
#endif                       
					GAMEToggleMainMenu(TRUE, 0);

			}

			// ========================================================
			// if new game loaded
			// put up the initial councelor report
			// ========================================================
			//if (fLoadedNewGame)
			//{
			//	fLoadedNewGame = FALSE;
			//	AddReport(SUBJ_INITIAL, 0, 0, 0);
			//	dturn_mode = INIT_REPORT_MODE;
			//}

		}
		fRedrawAll = TRUE;
	}

	// ========================================================
	// fade out after transition
	// ========================================================
	if (fFadedOut < 0)
	{
		FadeOut(100);
		clear_screen();
		update_screen();
		init_pal("nova_l");
		init_shade_table("nova_l");
		CurPal[254].bRed = CurPal[254].bGreen = CurPal[254].bBlue = 0;
//		set_pal((char *)&CurPal[0]);
		FadeOut(0);
		fFadedOut = 2;								// fade in after 2nd pass
		fRedrawAll = TRUE;
	}

	if (fRedrawRoads)
	{
		fRedrawRoads = FALSE;
		for (j=1; j<PROVINCE_COUNT; ++j)		// draw roads into the map
			DrawRoads((PROVINCE)j);
		fMapAltered = FALSE;
	}

	// ========================================================
	// Handle multiplayer events
	// ========================================================
	// MOVED TO TOP  ABC 6/9/97
	//HandleMultiPlayerEvents();		// calls HaveEvents

#ifdef _DEMO
	CheckAutoDemo();
#endif

	// handle join dialog
	if (fCommandBattleLoop)
	{
		while (fCommandBattleLoop)
			MenuLoop();
		pop_regions();
		clear_key_status(0);
	}

	fInfoOnlyDomainUI = FALSE;
}


/*	========================================================================
	DEMO SUPPORT CODE
	========================================================================*/
#ifdef _DEMO
static CSTRPTR szMapIntroScrn[] = {"intro\\mapscrn0.pcx","intro\\mapscrn1.pcx","intro\\mapscrn2.pcx",
"intro\\mapscrn3.pcx","intro\\mapscrn4.pcx","intro\\mapscrn5.pcx"};
#endif

void PlayMapIntro (
#if defined (_DEMO)
	LONG iStart, LONG iEnd
#else
	LONG , LONG
#endif
	)
{
#ifdef _DEMO
	SHORT	bitm, i;
	LONG	now;

	// load new palette
	FadeOut(100);
	clear_screen();
	update_screen();
	init_pal("nova_l");
	init_shade_table("nova_l");
	set_hires(0, 0);
	CurPal[254].bRed = CurPal[254].bGreen = CurPal[254].bBlue = 0;
	set_pal((char *)&CurPal[0]);
	FadeIn(100);

	// play intro flic
	for (i=iStart; i<=iEnd; ++i)
	{
		bitm = GetResourceStd (szMapIntroScrn[i], FALSE);
		if (bitm != fERROR)
		{
			POINT start,bck;
			start.x = 320;
			start.y = 240;
			bck.x = 320;
			bck.y = 240;
			ZoomBitmap (0, 0, bitm, 0, 0, 640, 480, fERROR, start, bck, 640, 480);
			//DrawBitmap (0, 0, bitm, 0, 0, 999, 999);
			update_screen();
			DisposBlock(bitm);
		}

		now = get_time()+(18*10);			 // 20 seconds delay

		while(get_time()<now)
		{
			run_timers();
			//read_mouse();
			update_buttons();
			if (mouse_button)
				break;
			if(key_status(KEY_ESCAPE))
				goto End;
			if(key_status(KEY_SPACEBAR) || key_status(KEY_RETURN))
				break;
		}
	}

End:
	clear_key_status(0);
	FadeOut(100);
	clear_screen();
	update_screen();
#endif
}

/*	========================================================================*/
static LONG		AutoDelay;
static SHORT	fWhich = 0;

void InitAutoDemo (void)
{
	AutoDelay = get_time()+(18*20);	 // 20 seconds delay
}

/*	========================================================================*/
void StartMapIntro (LONG, LONG)
{
	fWhich = 2;
	AutoDelay = get_time();

}
/*	========================================================================*/
void CheckAutoDemo (void)
{

	if (AutoDelay < get_time())
	{
		InitDomainTurn(0);
//		remove_timer(draw_cursor);

		if (fWhich==0)				  // play battle
		{
			PlayBattleFlc();
			++fWhich;
		}
		else if (fWhich==1)			// play intro
		{
			PlayIntro();
			++fWhich;
		}
		else if (fWhich==2)			// play tutorial
		{
			del_region(StartMapIntro,0);
			PlayMapIntro(0,5);
			++fWhich;
			add_key(GAME_KEYS::mfGetKey(GAME_KEYS::SKIP_TO_NEXT_FRAME),StartMapIntro,0,5);
		}
		else if (fWhich==3)			// play adventure
		{
			PlayAdvFlc();
			fWhich = 0;
		}

//		add_timer(1,draw_cursor);
		clear_key_status(0);
		InitDomainTurn(0);
		clear_key_status(0);

		InitAutoDemo();
	}
}


/* ========================================================================
   Function    - MultiBattle()
   Description - Check and initialize multiplayer battle
   Returns     -
   ======================================================================== */
#if 0
#if defined (_WINDOWS)
BOOL MultiBattle(LONG iMovingUnit, LONG j )
{

	CHAR	cTerrain;
	CHAR	strTerrain[6];

	SCENE_MGR::Aggressor = (REALM::REALM_TYPE)units[iMovingUnit].Realm;
	SCENE_MGR::Victor = 0; // clear the victor index

	if (units[j].Realm == HomeRealm)
	{
		SCENE_MGR::HomeIndex = (SHORT)j;			// who is the home unit (w/camera)
		SCENE_MGR::AwayIndex = (SHORT)iMovingUnit;	// who is the away unit
	}
	else
	{
		SCENE_MGR::HomeIndex = (SHORT)iMovingUnit;	// who is the home unit (w/camera)
		SCENE_MGR::AwayIndex = (SHORT)j;			// who is the away unit
	}

	SCENE_MGR::SceneType = SCENE_AI::BATTLE_SCENE;  // type for next scene ai

	// Got to protect ourselves

	if (    SCENE_MGR::HomeIndex == -1
		 ||	SCENE_MGR::AwayIndex == -1
		 ||	SCENE_MGR::Aggressor == -1
	   )
	{
//		WaitSync ( iMPS_SYNC7 );
		return FALSE;
	}


	// GEH my doings

	switch( province[units[j].province].Terrain )
	{
		case FOREST:
			cTerrain = 'w';	// woods
			break;

		case MOUNTN:
		case HVY_MNT:
			cTerrain = 'm';	// mountains
			break;

		case SWAMP:
			cTerrain = 's';	// swamp
			break;

		case HILLS:
		case PLAINS:
		case NO_TERRAIN:
		default:
			cTerrain = 'p';	// plains
	}

	sprintf(strTerrain, "batl%c", cTerrain);

	ClearRemapTable();

	BattlePrep();

	fShowProgressBar = TRUE;
	SCENE_MGR::mfRequestNewScene( strTerrain , TYPE_PLAYERSTART1, TRUE); // load first scene

	InitDomainTurn(0);		// turn off domain turn

	fWasABattle = TRUE;

//	WaitSync ( iMPS_SYNC7 );

	return TRUE;

}
#endif

#endif

/* ========================================================================
	Function	- CheckUnitArrival
	Description -
	Returns	- void
	======================================================================== */
#define NPCBATTLE_ACTION 199

LONG CheckUnitArrival (LONG iMovingUnit, BOOL fJoinAllowed)
{
	LONG	iProvince = units[iMovingUnit].province;
	LONG	i, j, AwayRealm;
	LONG	rv = 0;
	BOOL	f = TRUE;
	SHORT	result;
	char	n[50];

#ifdef _DEMO
		TurnOffAllSounds();
		InitDomainTurn(0);
		PlayBattleFlc();
		clear_key_status(0);
		InitDomainTurn(0);
		clear_key_status(0);
		return 0;
#endif

	if ( units[iMovingUnit].DestProvince != 0 )
	{
		// -- GEH test for illegal movement case
		// -- if he has somewhere to go and is just standing around, 
		// -- kick him in the butt to get him moving
		if ( units[iMovingUnit].MovePart == 0 )
			units[iMovingUnit].MovePart = 1; 
		
		return 0;
	}

//printf("CheckUnitArrival - unit:%d  iJoinDialog_to:%d\n",iMovingUnit,iJoinDialog_to);

	//---------------------------
	// check visibility
	// if a non-visible unit moves into a province we own, it should
	// become visible.
	//---------------------------
	if (units[iMovingUnit].NotSpotted && province[iProvince].Realm == HomeRealm)
	{
		units[iMovingUnit].NotSpotted = FALSE;
		// Mark all the followers spotted too.
		for (LONG p = units[iMovingUnit].NextUnit;
		     p > -1;
		     p = units[p].NextUnit)
		{
			units[p].NotSpotted = FALSE;
		}
		// Spotted flag is not transmitted.
		//SetGameData(MP_UNITS, MPUNITS_NOTSPOTTED, iMovingUnit, FALSE, FALSE );
	}
	
	//---------------------------
	// check for join
	// don't ask if not at end of movement
	// Or if we are already a full stack.
	//---------------------------
	if (units[iMovingUnit].DestPath1==0 && fJoinAllowed && gUnitStackSize > CountUnitStack(iMovingUnit))
	{
		for (j=iJoinDialog_to+1; j<MAX_UNITS; ++j)		// scan for units here
		{
			if (j!=iMovingUnit && units[j].DestProvince==0
					&& units[j].province == iProvince
					&& units[j].Joined==FALSE
//					&& AREFULLALLIES(units[iMovingUnit].Realm, units[j].Realm)
					&& units[iMovingUnit].Realm == units[j].Realm
					&& gUnitStackSize > CountUnitStack(j)
				)
			{
				// only put up dialog if player's units
				if (units[iMovingUnit].Realm==HomeRealm)
				{
					fJoinDialog = (SHORT)iMovingUnit;
					iJoinDialog_to = (SHORT)j;
					reqSound = CalcVoxSndObj(GameIconVOX[units[iMovingUnit].Icon], VOX_CASTLE_INQUIRY);
					return 0;
				}
				// if NPC realm, auto join
				else if (realm[units[iMovingUnit].Realm].mfIsPlayerCtrl() == FALSE)
				{
					CheckUnitStack(JoinUnits_sub(j, iMovingUnit, FALSE), FALSE);
					fCheckUnitArrival = FALSE;
					return 0;
				}
			}
		}
	}
	fJoinDialog = FALSE;

	//---------------------------
	// check for battle
	//---------------------------
	if (!fJoinAllowed
//			&& !fInfoOnlyDomainUI	[abc] 9/3
		)
	{
		for (j=1; j<MAX_UNITS; ++j)		// scan for units here
		{
			if (units[iMovingUnit].province != iProvince)	// leader killed
				break;

			// -- if we are in this province
			// -- and we are not ourself
			// -- and our realm still exists
			// -- and I'm not moving
			// -- and he's not moving
			// -- and we're not allies
			// -- then fight
			if (units[j].province == iProvince && units[j].Joined==FALSE
				&& j != iMovingUnit
				&& realm[units[j].Realm].mfExists()
				&& units[j].DestProvince == 0 && units[j].MovePart == 0
				&& units[iMovingUnit].DestProvince == 0 && units[iMovingUnit].MovePart == 0
				&& !AREALLIED(units[iMovingUnit].Realm, units[j].Realm) )
			{
				iUnitInfo = 0;			// no info up
				iPlaceInfo = 0;
				iProvInfo = 0;


			// integrated multi player tests
#ifdef _WINDOWS
				//---- Seed random number generator
				AMultiPlayer.NextSeed(__FILE__,__LINE__);
				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					char temp[150];
					char MovingUnitRealmName[50];
					
					strcpy(MovingUnitRealmName, realm[units[iMovingUnit].Realm].mfGetName() );
					sprintf ( temp, "CheckUnitArrival - mov:%s vs j:%s in prov:%s",
						MovingUnitRealmName,
						realm[units[j].Realm].mfGetName(),
						province[iProvince].name);
					RandomLogComment ( temp );
				}

				// ----------------------------------------------
				// If one player is player controlled but neither is US
				// ----------------------------------------------
				if (IsMultiPlayer()
					&& units[iMovingUnit].Realm != HomeRealm
					&&	units[j].Realm != HomeRealm
					&& ( realm[units[j].Realm].mfIsPlayerCtrl() ||
				 		realm[units[iMovingUnit].Realm].mfIsPlayerCtrl() ) )
				{
					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						char temp[150];
						char MovingUnitRealmName[50];
						strcpy(MovingUnitRealmName,realm[units[iMovingUnit].Realm].mfGetName());
						sprintf ( temp, "CheckUnitArrival - neither is us: Sync %ld - mov:%s vs j:%s in prov:%s",
							(iMPS_SYNC_INITIATIVE + BattleSync),
							MovingUnitRealmName,
							realm[units[j].Realm].mfGetName(),
							province[iProvince].name);
						RandomLogComment ( temp );
					}

					WaitSync( iMPS_SYNC6 );		// before the player controlled battle

					//--- put up info box and wait
					//fSyncPending = TRUE;
					//ShowSyncWaitWindow( (iMPS_SYNC_INITIATIVE + BattleSync), 0 );
					//while (fSyncPending)
					//{
					//	MenuLoop();
					//	#ifdef _DEBUG
					//	LogFlush();
					//	#endif
					//}
					//AMultiPlayer.ResetSync( iMPS_SYNC_INITIATIVE + BattleSync );
					StartSync( iMPS_SYNC_INITIATIVE + BattleSync );
					CheckSync( (iMPS_SYNC_INITIATIVE + BattleSync), TRUE, TRUE );

					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						char temp[150];
						sprintf ( temp, "CheckUnitArrival - neither is us: done waiting at Sync %ld",
							(iMPS_SYNC_INITIATIVE + BattleSync));
						RandomLogComment ( temp );
					}
					
					BattleSync = (BattleSync + 1) & 3;
					rv = fAUTOBATTLE;
				}
				else
#endif

				// ----------------------------------------------
				// if WE are one of the participants
				// ----------------------------------------------
				if ( units[iMovingUnit].Realm == HomeRealm || 
 				     units[j].Realm == HomeRealm   )
				{
					CHAR	cTerrain;
					CHAR	strTerrain[6];

					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						char temp[150];
						char MovingUnitRealmName[50];
						
						strcpy(MovingUnitRealmName, realm[units[j].Realm].mfGetName());
						sprintf ( temp, "CheckUnitArrival - we are one: mov:%s vs j:%s in prov:%s",
							MovingUnitRealmName,
							realm[units[j].Realm].mfGetName(),
							province[iProvince].name);
						RandomLogComment ( temp );
					}

					SCENE_MGR::Aggressor = (REALM::REALM_TYPE)units[iMovingUnit].Realm;
					SCENE_MGR::Victor = 0; // clear the victor index
					if (units[j].Realm == HomeRealm)
					{
						SCENE_MGR::HomeIndex = (SHORT)j;	// who is the home unit (w/camera)
						SCENE_MGR::AwayIndex = (SHORT)iMovingUnit;	// who is the away unit
						AwayRealm = units[iMovingUnit].Realm;
					}
					else
					{
						SCENE_MGR::HomeIndex = (SHORT)iMovingUnit;	// who is the home unit (w/camera)
						SCENE_MGR::AwayIndex = (SHORT)j;	// who is the away unit
						AwayRealm = units[j].Realm;
					}
					SCENE_MGR::SceneType = SCENE_AI::BATTLE_SCENE;  // type for next scene ai

					// Got to protect ourselves
					if (	SCENE_MGR::HomeIndex == -1
						||	SCENE_MGR::AwayIndex == -1
						||	SCENE_MGR::Aggressor == -1
						)
					{
						LOGCOMMENTSLEEP
						if ( fLogComment )
						{
							char temp[150];
							sprintf ( temp, "CheckUnitArrival - we are one, something was -1");
							RandomLogComment ( temp );
						}
						continue;
					}

					// GEH my doings
					switch(province[units[j].province].Terrain)
					{
					case FOREST:
						cTerrain = 'w';	// woods
						break;

					case MOUNTN:
					case HVY_MNT:
						cTerrain = 'm';	// mountains
						break;

					case SWAMP:
						cTerrain = 's';	// swamp
						break;

					case HILLS:
					case PLAINS:
					case NO_TERRAIN:
					default:
						cTerrain = 'p';	// plains
					}

					sprintf(strTerrain, "batl%c", cTerrain);

					//GEH 11/24/96
					ClearRemapTable();

					#ifdef _WINDOWS
					if (IsMultiPlayer())
						WaitSync( iMPS_SYNC6 );		// before the player controlled battle
					#endif

					// allow YES on the Query menu?
					// only if not multiplayer, or if multiplayer with only 
					// one opponent

					#ifdef _WINDOWS
					f = (!IsMultiPlayer() 
						|| (HowManyPlayers() == 1 && realm[AwayRealm].mfIsPlayerCtrl()));
						#if defined(_JUNEDEMO)
						f = FALSE;
						#endif	
					#else
					f = TRUE;
					#endif

					// ask if you want to command this battle in person
					// or let it just play out
					RealmSceneFlags[AwayRealm] = 99;
					result = QueryCommandBattle(f);

					// If both players are player controlled, wait until both respond
					#ifdef _WINDOWS
					if (IsMultiPlayer() && realm[AwayRealm].mfIsPlayerCtrl())
					{
						LOGCOMMENTSLEEP
						if ( fLogComment )
						{
							char temp[150];
							sprintf ( temp, "CheckUnitArrival - we are one: send RealmSceneFlags sync");
							RandomLogComment ( temp );
						}
						
						SetGameData(MP_REALM, MPRLM_BATTLEQUERY, HomeRealm, result, TRUE);
						
						LOGCOMMENTSLEEP
						if ( fLogComment )
						{
							char temp[150];
							sprintf ( temp, "CheckUnitArrival - we are one: start waiting at RealmSceneFlags delay");
							RandomLogComment ( temp );
						}
						
						StartWait(99);
						while (RealmSceneFlags[AwayRealm] == 99)	// wait for opponent
						{
							AMultiPlayer.LookForMessages();
							Sleep(0);

							#ifdef _DEBUG
							if ( GetAsyncKeyState( VK_F10 ) & 0x8000 )
							{
								AMultiPlayer.Finalize();
								break;
							}
							#endif
						}
						EndWait(99);
						
						LOGCOMMENTSLEEP
						if ( fLogComment )
						{
							char temp[150];
							sprintf ( temp, "CheckUnitArrival - we are one: done waiting at RealmSceneFlags delay");
							RandomLogComment ( temp );
						}

						// if we didn't retreat and opponent said RETREAT, we win
						if (result != 2 && RealmSceneFlags[AwayRealm] == 2)
							result = 3;		// automatic win

						// if we say YES and opponent said NO, then we say no
						else if (result == 1 && RealmSceneFlags[AwayRealm] == 0)
							result = 0;		// auto-battle
					}
					#endif

					if (result == 1) // Yes, fight
					{
						BattlePrep();
						SCENE_MGR::mfRequestNewScene( strTerrain , TYPE_PLAYERSTART1, TRUE); // load first scene
						InitDomainTurn(0);		// turn off domain turn
						fWasABattle = TRUE;
						return fSCENEBATTLE;
					}
					else
					{
						if (result == 0) // let the computer fight
						{
							DisplayProcess(FIGHTING, RED_GRAD, SCENE_MGR::HomeIndex, SCENE_MGR::AwayIndex);

							#ifdef _WINDOWS
							// battle is done only on the aggressor's machine, results are broadcast
							if (IsMultiPlayer() && realm[AwayRealm].mfIsPlayerCtrl())
							{
								if (SCENE_MGR::Aggressor == HomeRealm)
								{
									NPCBattleResults(TRUE);

									#ifdef _WINDOWS
									LOGCOMMENTSLEEP
									if ( fLogComment )
   								{
										char temp[100];
										sprintf ( temp, "CheckUnitArrival: SendEvent");
										RandomLogComment ( temp );
									}
									#endif
									
									AMultiPlayer.SendEvent (NPCBATTLE_ACTION,
										SCENE_MGR::Loser,
										SCENE_MGR::Victor,
										sUnitsLostLoser,
										sUnitsLostWinner,
										AwayRealm);
									AMultiPlayer.InitEvent(0);
								}
								else
								{
									// wait for results
									while (TRUE)
									{
										if ( AMultiPlayer.HaveEvent() == NPCBATTLE_ACTION )
										{
											SCENE_MGR::Loser = EventTable.data1;
											SCENE_MGR::Victor = EventTable.data2;
											sUnitsLostLoser = EventTable.data3;
											sUnitsLostWinner = EventTable.data4;
											AMultiPlayer.InitEvent(0);
											break;
										}

										// get messages and give time up to the system
										AMultiPlayer.LookForMessages();   //---- Update not specific messages
                	       		WindowsMessages();
										Sleep(0);

										#ifdef _DEBUG
										if ( GetAsyncKeyState( VK_F10 ) & 0x8000 )
										{
											AMultiPlayer.Finalize();
											if ( fLogComment )
											{
												char temp[150];
												sprintf ( temp, "Finalizing NPCBattle - VK_F10=%ld",
													(LONG)(GetAsyncKeyState( VK_F10 ) & 0x8000) );
												RandomLogComment ( temp );
											}
										}
										#endif

										if ( !realm[AwayRealm].mfIsPlayerCtrl() )
										{
											#if 0
											LOGCOMMENTSLEEP
											if ( fLogComment )
											{
												char temp[150];
												sprintf ( temp, "Finalizing NPCBattle - PlayerCtrl[%ld]=%ld\n",
													(LONG)AwayRealm,
													(LONG)(!realm[AwayRealm].mfIsPlayerCtrl()));
												RandomLogComment ( temp );
											}
											#endif

											AMultiPlayer.Finalize();
											SCENE_MGR::Loser = SCENE_MGR::HomeIndex;
											SCENE_MGR::Victor = SCENE_MGR::AwayIndex;
											sUnitsLostLoser = 0;
											sUnitsLostWinner = 0;
											break;
										}
									}
									RegentDeathTest();
								}
								SCENE_MGR::VictorRealm = units[SCENE_MGR::Victor].Realm;
								SCENE_MGR::LoserRealm = (SCENE_MGR::VictorRealm == HomeRealm) ? AwayRealm : HomeRealm;
							}
							else
							#endif
								NPCBattleResults(TRUE);
						}
						else 
						if (result == 2) // result 2 just displays the lose
						{
							SCENE_MGR::Loser = SCENE_MGR::HomeIndex;
							SCENE_MGR::Victor = SCENE_MGR::AwayIndex;
							SCENE_MGR::LoserRealm = units[SCENE_MGR::HomeIndex].Realm;
							SCENE_MGR::VictorRealm = units[SCENE_MGR::AwayIndex].Realm;
							sUnitsLostLoser = 0;
							sUnitsLostWinner = 0;
						}
						else			// 3 means do nothing because we win
						{
							SCENE_MGR::Loser = SCENE_MGR::AwayIndex;
							SCENE_MGR::Victor = SCENE_MGR::HomeIndex;
							SCENE_MGR::LoserRealm = units[SCENE_MGR::AwayIndex].Realm;
							SCENE_MGR::VictorRealm = units[SCENE_MGR::HomeIndex].Realm;
							sUnitsLostLoser = 0;
							sUnitsLostWinner = 0;
						}

						DisplayNPCResults();
						fWasABattle = TRUE;

						LOGCOMMENTSLEEP
						if ( fLogComment )
						{
							char temp[150];
							char VictorRealmName[50];
							
							strcpy(VictorRealmName, realm[units[SCENE_MGR::Victor].Realm].mfGetName());
							sprintf ( temp, "CheckUnitArrival - we are one: winner:%s loser:%s in prov:%s",
								VictorRealmName,
								realm[units[SCENE_MGR::Loser].Realm].mfGetName(),
								province[iProvince].name);
							RandomLogComment ( temp );
						}

						#ifdef _WINDOWS
						// in multiplay, units are withdrawn only on the 
						// looser's machine, the results are broadcast
						if ( IsMultiPlayer() )
						{
							if ( (!realm[AwayRealm].mfIsPlayerCtrl()) || 
									SCENE_MGR::LoserRealm == HomeRealm)
								WithdrawLosingUnit(TRUE);
						}
						else
						#endif
							WithdrawLosingUnit(TRUE);

						#ifdef _WINDOWS
						if (IsMultiPlayer())
						{
							LOGCOMMENTSLEEP
							if ( fLogComment )
							{
								char temp[150];
								sprintf ( temp, "CheckUnitArrival - we are one: Sync %ld",
									(iMPS_SYNC_INITIATIVE + BattleSync));
								RandomLogComment ( temp );
							}

							//fSyncPending = TRUE;
							//ShowSyncWaitWindow( (iMPS_SYNC_INITIATIVE + BattleSync), 0 );
							//while (fSyncPending)
							//{
							//	MenuLoop();
							//	#ifdef _DEBUG
							//	LogFlush();
							//	#endif
							//}
							//AMultiPlayer.ResetSync( iMPS_SYNC_INITIATIVE + BattleSync );
							StartSync( iMPS_SYNC_INITIATIVE + BattleSync );
							CheckSync( (iMPS_SYNC_INITIATIVE + BattleSync), TRUE, TRUE );

							LOGCOMMENTSLEEP
							if ( fLogComment )
							{
								char temp[150];
								sprintf ( temp, "CheckUnitArrival - we are one: done waiting at Sync %ld",
									(iMPS_SYNC_INITIATIVE + BattleSync));
								RandomLogComment ( temp );
							}
							
							BattleSync = (BattleSync + 1) & 3;

							RealmSceneFlags[HomeRealm] = 99;
							RealmSceneFlags[AwayRealm] = 99;

						}
						#endif
					}
					rv = fAUTOBATTLE;

					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						char temp[100];
						sprintf ( temp, "After PC vs NPC Battle - cntUnits %ld", cntUnits_() );
						RandomLogComment ( temp );
					}
				}

				// ----------------------------------------------
				// otherwise, just run out a NPC battle to see who won
				// ----------------------------------------------
				else
				{
					if (units[j].Realm == HomeRealm)
					{
						SCENE_MGR::HomeIndex = (SHORT)j;			// who is the home
						SCENE_MGR::AwayIndex = (SHORT)iMovingUnit;	// who is the away
					}
					else
					{
						SCENE_MGR::HomeIndex = (SHORT)iMovingUnit;	// who is the home
						SCENE_MGR::AwayIndex = (SHORT)j;			// who is the away
					}

					// Got to protect ourselves
					if (	SCENE_MGR::HomeIndex == -1
						||	SCENE_MGR::AwayIndex == -1
						||	SCENE_MGR::Aggressor == -1
						)
					{
						LOGCOMMENTSLEEP
						if ( fLogComment )
						{
							char temp[150];
							sprintf ( temp, "CheckUnitArrival - NPC only, something was -1");
							RandomLogComment ( temp );
						}
						continue;
					}

//					printf("NPCBattle was fought between %d and %d",
//						SCENE_MGR::HomeIndex, SCENE_MGR::AwayIndex);
					DisplayProcess(FIGHTING, RED_GRAD, SCENE_MGR::HomeIndex, SCENE_MGR::AwayIndex);

					NPCBattleResults(FALSE);

//					printf(" - %d won!\n", SCENE_MGR::Victor);
//					DisplayNPCBattle(-1, SCENE_MGR::Victor);

					fWasABattle = TRUE;

					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						char temp[150];
						char MovingUnitRealmName[50];
						
						strcpy(MovingUnitRealmName, realm[units[iMovingUnit].Realm].mfGetName());
						sprintf ( temp, "CheckUnitArrival - NPC only - mov:%s vs j:%s in prov:%s",
							MovingUnitRealmName,
							realm[units[j].Realm].mfGetName(),
							province[iProvince].name);
						RandomLogComment ( temp );
					}
					WithdrawLosingUnit(FALSE);
					rv = fAUTOBATTLE + fNPCBATTLE;

					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						char temp[100];
						sprintf ( temp, "After NPC vs NPC Battle - cntUnits %ld", cntUnits_() );
						RandomLogComment ( temp );
					}
				}

			}
		}
	}		// if !fJoinAllowed

	// ----------------------------------------------
	// continue moving if indicated
	// ----------------------------------------------
	if (units[iMovingUnit].DestPath1)
	{
		//printf("Continuing move\n");
		units[iMovingUnit].DestProvince = province[units[iMovingUnit].province].Border[(units[iMovingUnit].DestPath1-1)&0x7].Province;
		units[iMovingUnit].DestPath1 = units[iMovingUnit].DestPath2;
		units[iMovingUnit].DestPath2 = units[iMovingUnit].DestPath3;
		units[iMovingUnit].DestPath3 = 0;
		fRedrawAll = TRUE;
	}

	fCheckUnitArrival = FALSE;
	iJoinDialog_to = 0;
	return rv;
}

/* ========================================================================
	Function	- WithdrawLosingUnit
	Description -
	Returns	- void
	======================================================================== */
void WithdrawLosingUnit (BOOL fSend)
{
	LONG		i, j, k, f;
	LONG		WinningUnit;
	LONG		WinningRealm;
	LONG		LosingUnit;
	LONG		LosingRealm;
	LONG		WithdrawToProv;
	LONG		prov;
	LONG		border_cost = 0;
	BOOL		fFoundEnemy;
	BOOL		fTerrainPassable;
	char 		buffer[100];
//	BOOL		fPermittedBorder[8];

	if (fWasABattle == FALSE)
		return;

	fTutorialFirstBattle = FALSE;

	fWasABattle = FALSE;
	WinningUnit = SCENE_MGR::Victor;
	LosingUnit = SCENE_MGR::Loser;

	//printf("WasABattle:TRUE	Victor:%d (%s)\n",WinningUnit,realm[units[WinningUnit].Realm].mfGetName());

	// there was a winner and there was loser left to move
	if (WinningUnit != 0 && LosingUnit != 0)
	{
		WinningRealm = units[WinningUnit].Realm;

		// discover losing realm
		LosingRealm = units[LosingUnit].Realm;
		prov = units[LosingUnit].province;

		// find adjacient province that belongs to me 
		// and is not warded and is passable
		// and doesn't have any non-friendly troops in it
		for (i=0; i<7; ++i)
		{
			j = province[prov].Border[i].Province;
			border_cost = TerrainCost[province[prov].Border[i].MoveCost];
			fTerrainPassable = (border_cost < 100);

			if (province[j].Realm == LosingRealm
				&& fTerrainPassable
				&& CheckNotWarded(LosingUnit, (PROVINCE)j, (PROVINCE)prov) )
			{
				fFoundEnemy = FALSE;
				for (k=1; k<MAX_UNITS; ++k)
				{
					if (units[k].Joined == 0			// is leader
						&& realm[units[k].Realm].mfExists()
						&& !AREALLIED(units[k].Realm, LosingRealm)
						&& UnitFinalProvince(k) == j )
					{
						fFoundEnemy = TRUE;
						break;
					}
				}
				if (!fFoundEnemy)
				{
					WithdrawToProv = j;
					goto FoundOne;
				}
			}
		}

		// couldn't find the above, so look for a friendly place
		// that is not warded and is passable
		// and doesn't have any non-friendly troops in it
		for (i=0; i<7; ++i)
		{
			j = province[prov].Border[i].Province;
			border_cost = TerrainCost[province[prov].Border[i].MoveCost];
			fTerrainPassable = (border_cost < 100);

			if (AREALLIED(LosingRealm, province[j].Realm)
				&& fTerrainPassable
				&& CheckNotWarded(LosingUnit, (PROVINCE)j, (PROVINCE)prov) )
			{
				fFoundEnemy = FALSE;
				for (k=1; k<MAX_UNITS; ++k)
				{
					if (units[k].Joined == 0			// is leader
						&& realm[units[k].Realm].mfExists()
						&& UnitFinalProvince(k) == j
						&& !AREALLIED(units[k].Realm,LosingRealm) )
					{
						fFoundEnemy = TRUE;
						break;
					}
				}
				if (!fFoundEnemy)
				{
					WithdrawToProv = j;
					goto FoundOne;
				}
			}
		}

		// Next, find any place I'm at war with
		// that is not warded and is passable
		// and doesn't have any non-friendly troops in it
		for (i=0; i<7; ++i)
		{
			j = province[prov].Border[i].Province;
			border_cost = TerrainCost[province[prov].Border[i].MoveCost];
			fTerrainPassable = (border_cost < 100);

			if (ISATWAR(LosingRealm, province[j].Realm)
				&& fTerrainPassable
				&& CheckNotWarded(LosingUnit, (PROVINCE)j, (PROVINCE)prov) )
			{
				fFoundEnemy = FALSE;
				for (k=1; k<MAX_UNITS; ++k)
				{
					if (units[k].Joined == 0			// is leader
						&& realm[units[k].Realm].mfExists()
						&& UnitFinalProvince(k) == j
						&& !AREALLIED(units[k].Realm,LosingRealm) )
					{
						fFoundEnemy = TRUE;
						break;
					}
				}
				if (!fFoundEnemy)
				{
					WithdrawToProv = j;
					goto FoundOne;
				}
			}
		}

		// Finally, find any place
		// that is not warded and is passable
		// and doesn't have any non-friendly troops in it
		for (i=0; i<7; ++i)
		{
			j = province[prov].Border[i].Province;
			border_cost = TerrainCost[province[prov].Border[i].MoveCost];
			fTerrainPassable = (border_cost < 100);

			if (fTerrainPassable && CheckNotWarded(LosingUnit, (PROVINCE)j, (PROVINCE)prov) )
			{
				fFoundEnemy = FALSE;
				for (k=1; k<MAX_UNITS; ++k)
				{
					if (units[k].Joined == 0			// is leader
						&& realm[units[k].Realm].mfExists()
						&& UnitFinalProvince(k) == j
						&& !AREALLIED(units[k].Realm,LosingRealm) )
					{
						fFoundEnemy = TRUE;
						break;
					}
				}
				if (!fFoundEnemy)
				{
					WithdrawToProv = j;
					goto FoundOne;
				}
			}
		}

		// only for the gorgon, teleport back to home
		if (units[LosingUnit].Icon == GORGON_UNIT_ICON)
		{
			SHORT i;
			for (i=1; i<PROVINCE_COUNT; ++i)
			{
				if (i == units[LosingUnit].province)
					continue;

				if (province[i].Realm == REALM::GORGONSCROWN)
				{
					fFoundEnemy = FALSE;
					for (k=1; k<MAX_UNITS; ++k)
					{
						if (units[k].Joined == 0			// is leader
							&& realm[units[k].Realm].mfExists()
							&& UnitFinalProvince(k) == i
							&& !AREALLIED(units[k].Realm,REALM::GORGONSCROWN) )
						{
							fFoundEnemy = TRUE;
							break;
						}
					}
					if (fFoundEnemy)
						continue;

					WithdrawToProv = i;
					// actually teleport him there directly
					SetGameData(MP_UNITS, MPUNITS_PROVINCE, LosingUnit, WithdrawToProv, fSend);

					goto FoundOne;
				}
			}
			
			// or else just kill him, but tell the user he vanished
			// into thin are never to be seen again "I'll be back!"
			SystemHelp(" ",STRMGR_GetStr(STR_GM_GORGON_BE_BACK), -1, NULL, 0);
			// stay here until we clear the message box
			if (sMenusUp > 0)
			{
				SHORT sHelpMenuCount = sMenusUp;
				while( sHelpMenuCount <= sMenusUp )
				{
					MenuLoop();
				}
			}
		}

		// destroy losing army
		if (units[WinningUnit].Realm == HomeRealm || 
			units[LosingUnit].Realm == HomeRealm)
		{
			char buf1[80];
			char cpLosingRealmName[80];
			
			
			strcpy(buf1,STRMGR_GetStr(STR_NO_PROVINCE_DELUNIT));
			strcpy(cpLosingRealmName,realm[units[LosingUnit].Realm].mfGetName());
			sprintf(buffer,
				buf1,
				//"With no safe haven to retreat to, the army %s %s are slaughtered.", 
				STRMGR_GetStr((((REALM::REALM_TYPE)SCENE_MGR::VictorRealm==REALM::MONSTERS)?STR_NPC_OF:STR_NPC_FROM)),
				cpLosingRealmName);
				
			SystemHelp(" ", buffer, -1, NULL, 0);
			// stay here until we clear the message box
			if (sMenusUp > 0)
			{
				SHORT sHelpMenuCount = sMenusUp;
				while( sHelpMenuCount <= sMenusUp )
				{
					MenuLoop();
				}
			}
		}
			
		//printf("	Destroying LosingUnits:\n");
		while (LosingUnit != -1)
		{
			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
				char temp[150];
				sprintf ( temp, "WithdrawLosingUnit - deleting unit:%ld of %s",
					LosingUnit,
					realm[units[LosingUnit].Realm].mfGetName());
				RandomLogComment ( temp );
			}

			// DeleteUnit returns the new next unit in the chain.
			LosingUnit = DeleteUnit(LosingUnit, fSend );
			//printf("	Destroy %d\n",i);
		}
		fCheckUnitArrival = FALSE;
		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
			char temp[150];
			sprintf ( temp, "WithdrawLosingUnit - Didn't FoundOne ");
			RandomLogComment ( temp );
		}
		return;

FoundOne:

		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
			char temp[150];
			sprintf ( temp, "WithdrawLosingUnit - FoundOne - %s to prov %s",
				realm[units[LosingUnit].Realm].mfGetName(),
				province[WithdrawToProv].name);
			RandomLogComment ( temp );
		}

		//printf("	Withdrawing LosingUnit to %s\n",province[WithdrawToProv].name);
		//units[LosingUnit].DestProvince = WithdrawToProv;
		SetGameData(MP_UNITS, MPUNITS_DESTPROVINCE, LosingUnit, WithdrawToProv, fSend);
		//units[LosingUnit].MovePart	= 1;
		SetGameData(MP_UNITS, MPUNITS_MOVEPART, LosingUnit, 1, fSend);
	}

	fCheckUnitArrival = FALSE;
}

/* ========================================================================
	Function	- CheckWithdrawPath
	Description - check to see if there is a place to run
	Returns	- BOOL TRUE if found a way to run
	======================================================================== */
BOOL CheckWithdrawPath (SHORT UnitIndex)
{
	LONG		i, j, k, f;
	LONG		UnitRealm;
	LONG		prov;
	BOOL		fFoundEnemy;

	// discover losing realm
	UnitRealm = units[UnitIndex].Realm;
	prov = units[UnitIndex].province;

	// -----------------------------------------------------------------
	// -- find adjacient province that belongs to me 
	// -- and doesn't have any non-friendly troops
	// -----------------------------------------------------------------

	for (i=0; i<7; ++i)
	{
		j = province[prov].Border[i].Province;
		if (province[j].Realm == UnitRealm)
		{
			fFoundEnemy = FALSE;
			for (k=1; k<MAX_UNITS; ++k)
			{
				if (units[k].Joined == 0			// is leader
					&& realm[units[k].Realm].mfExists()
					&& units[k].province == j
					&& !AREALLIED(units[k].Realm,UnitRealm) )
				{
					fFoundEnemy = TRUE;
					break;
				}
			}
			
			if (!fFoundEnemy)
			{
				return TRUE;
			}
		}
	}

	// -----------------------------------------------------------------
	// -- couldn't find the above, so look for a friendly place
	// -- to go that doesn't have the enemy in it
	// -----------------------------------------------------------------

	for (i=0; i<7; ++i)
	{
		j = province[prov].Border[i].Province;
		if (AREALLIED(UnitRealm, province[j].Realm))
		{
			fFoundEnemy = FALSE;
			for (k=1; k<MAX_UNITS; ++k)
			{
				if (units[k].Joined == 0			// is leader
					&& realm[units[k].Realm].mfExists()
					&& units[k].province == j
					&& !AREALLIED(units[k].Realm,UnitRealm) )
				{
					fFoundEnemy = TRUE;
					break;
				}
			}
			if (!fFoundEnemy)
			{
				return TRUE;
			}
		}
	}

	// -----------------------------------------------------------------
	// -- Finally, find any place I'm at war with
	// -- to go that doesn't have the enemy in it
	// -----------------------------------------------------------------
	
	for (i=0; i<7; ++i)
	{
		j = province[prov].Border[i].Province;
		if (ISATWAR(UnitRealm, province[j].Realm))
		{
			fFoundEnemy = FALSE;
			for (k=1; k<MAX_UNITS; ++k)
			{
				if (units[k].Joined == 0			// is leader
					&& realm[units[k].Realm].mfExists()
					&& units[k].province == j
					&& !AREALLIED(units[k].Realm,UnitRealm) )
				{
					fFoundEnemy = TRUE;
					break;
				}
			}
			if (!fFoundEnemy)
			{
				return TRUE;
			}
		}
	}

	// -----------------------------------------------------------------
	// -- If you get here, there was no place to run to
	// -----------------------------------------------------------------

	return FALSE;

}

/* ========================================================================
   Function    - CalculatePoints
   Description - calculate the points needed to win the game
   Returns     - SHORT points
   ======================================================================== */
SHORT CalculatePoints (REALM::REALM_TYPE CheckRealm)
{
	SHORT accumPoints = 0;
	SHORT j;

	// You can't check the realm's existance here as the 5 globals are set in
	// CalculateProvPoints....Fix this later.  -GWP-
	accumPoints = CalculateProvPoints(CheckRealm);
	accumPoints += CalculateRelicPoints(CheckRealm);

	return accumPoints;
}

/* ========================================================================
   Function    - CalculateProvPoints
   Description - calculate the province points 
   Returns     - SHORT points
   ======================================================================== */
SHORT CalculateProvPoints (REALM::REALM_TYPE CheckRealm)
{
	SHORT accumPoints = 0;
	SHORT j;

	// clear global points for specific types
	sProvPoints = 0;
	sVassalPoints = 0;
	sFullAllyPoints = 0;
	sPermissivePoints = 0;
	sOtherPoints = 0;
	
	if (!realm[CheckRealm].mfExists())
		return 0;
		
	// points for provinces
	for (j=1; j<PROVINCE_COUNT; ++j)
	{
		REALM::REALM_TYPE ThisProvRealm = (REALM::REALM_TYPE)province[j].Realm;
		
		// my province - 3 points
		if (ThisProvRealm == CheckRealm)
		{
			accumPoints += 3;
			sProvPoints += 3;
		}
		
		// my vassals provinces - 3 points
		if (ISVASSAL(ThisProvRealm,CheckRealm) && ThisProvRealm != CheckRealm)
		{
			accumPoints += 3;
			sVassalPoints += 3;
		}
		
		// full aliance provinces - 2 points
		else
		{
			if (AREFULLALLIES(ThisProvRealm,CheckRealm) && ThisProvRealm != CheckRealm)
			{
				accumPoints += 2;
				sFullAllyPoints += 2;
			}

		
			// permissive alliances provinces - 1 point
			else
			{
				if (AREALLIED(ThisProvRealm,CheckRealm) && ThisProvRealm != CheckRealm)
				{
					accumPoints += 1;
					sPermissivePoints += 1;
				}
			}
		}
		
		// Spiderfell provinces - 10 points, add 7, 3 added above
		if (ThisProvRealm == CheckRealm && j == SPIDERFELL )
		{
			// don't allow the bad guys to get the bonus
			if(CheckRealm != REALM::SPIDERFELL &&
				CheckRealm != REALM::RHUOBHE &&
				CheckRealm != REALM::GORGONSCROWN)
			{
				accumPoints += 7;
				sOtherPoints += 7;
			}
		}
		
		// Manslayer provinces - 10 points, add 7, 3 added above
		if (ThisProvRealm == CheckRealm && j == RHUOBHE )
		{
			// don't allow the bad guys to get the bonus
			if(CheckRealm != REALM::SPIDERFELL &&
				CheckRealm != REALM::RHUOBHE &&
				CheckRealm != REALM::GORGONSCROWN)
			{
				accumPoints += 7;
				sOtherPoints += 7;
			}
		}
		
		// Gorgons Crown provinces - 10 points, add 7, 3 added above
		if ( ThisProvRealm == CheckRealm
			&&
			(	ABATTOIR  == j ||
				ANATHAR  == j ||
				ELFSEYES  == j ||
				JOGH_WARREN  == j ||
				KAL_SAITHARAK  == j ||
				METTLE  == j ||
				MOTILE  == j ||
				MUTIANS_POINT == j ||
				OROGS_HEAD  == j ||
				PELT  == j ||
				PLUMBAGO  == j ||
				SAGES_FEN  == j ||
				SERES_HOLD  == j ||
				SIDEATH == j ||
				STONES_END  == j ||
				SUNDER_FALLS  == j ||
				ZAPTIG == j )
			)
		{
			// don't allow the bad guys to get the bonus
			if(CheckRealm != REALM::SPIDERFELL &&
				CheckRealm != REALM::RHUOBHE &&
				CheckRealm != REALM::GORGONSCROWN)
			{
				accumPoints += 7;
				sOtherPoints += 7;
			}
		}
		
		// Awnsheghlien allies provinces - 5 points
		if ( ThisProvRealm == CheckRealm
			&&
			(	// markazor
				BRUSHFIRE  == j ||
				DWARFS_HOLD == j ||
				ELFSDEMISE == j ||
				PERILTREES == j ||
				RIVERSPRING == j ||
				SHATTERED_HILLS == j ||
				SUTREN_HILLS == j ||

				// thurazor
				BLOODBAY == j ||
				CRUSHING_HILLS == j ||
				DOOMS_PEAK == j ||
				FALLING_TIMBER == j ||
				MERGARROTE == j ||
				STORMS_RELEASE == j ||

				// chimaeron
				BARNIERE == j ||
				CAREINE == j ||
				HAMEIN == j ||
				LYSSAN == j ||
				MHOWE == j ||
				RUORKHE == j ||
				SALVIENE == j ||

				//mur-kilad
				CRUSHING_ROCK == j ||
				FALLEN_ROCK == j )

			)
		{
			// don't allow the bad guys to get the bonus
			if(CheckRealm != REALM::SPIDERFELL &&
				CheckRealm != REALM::RHUOBHE &&
				CheckRealm != REALM::GORGONSCROWN &&
				CheckRealm != REALM::MARKAZOR &&
				CheckRealm != REALM::THURAZOR &&
				CheckRealm != REALM::CHIMAERON &&
				CheckRealm != REALM::MURKILAD)
			{
				accumPoints += 5;
				sOtherPoints += 5;
			}
		}

#if 0
		if (AREALLIED(CheckRealm,REALM::SPIDERFELL) ||
			AREALLIED(CheckRealm,REALM::RHUOBHE) ||
			AREALLIED(CheckRealm,REALM::GORGONSCROWN) )
		{
			// only allow this for yourself
			if( CheckRealm == HomeRealm)
			{
				accumPoints += 5;
				sOtherPoints += 5;
			}
		}
#endif
	}
	
	return accumPoints;
}		

/* ========================================================================
   Function    - CalculateRelicPoints
   Description - calculate the relic points 
   Returns     - SHORT points
   ======================================================================== */
SHORT CalculateRelicPoints(REALM::REALM_TYPE CheckRealm)
{
	SHORT accumPoints = 0;
	SHORT ThisRegent;
	BOOL  bWeHaveTheGorgonsBanner = FALSE;
	BOOL  bWeHaveSwordOfRoele = FALSE;
	BOOL  bWeHaveRegaliaOfEmpire = FALSE;
	BOOL  bWeHaveCrownOfAnuire = FALSE;
	BOOL  bWeHaveBannerOfRoele = FALSE;
	
	if (!realm[CheckRealm].mfExists())
		return 0;
		
	for (ThisRegent=1; ThisRegent < CHARACTER_COUNT; ++ThisRegent)
	{
		if (regents[ThisRegent].mfGetRealm()  != CheckRealm
		    || regents[ThisRegent].mfGetunit() <= 0)
		{
			// either not our realm or dead.
			continue;
		}
		
		// for our regents and lieutenents.
		SHORT const ThisID = regents[ThisRegent].mfGetid();
		SHORT const hPlayerStats = LoadStats(ThisID, fERROR);
		if (hPlayerStats != fERROR)
		{
			DumbAutoLockPtr<PLAYER_STATS> const pPS(hPlayerStats);
			
			// NOTE: the following are 10 points less because you get 
			// an additional 10 points for each of them as relics of power
			
			//points for Sword of Roele
			if(pPS->Inventory.mfDoIHaveItem(SWORD_OF_ROELE) && !bWeHaveSwordOfRoele)
			{
				accumPoints += 50;
				bWeHaveSwordOfRoele = TRUE;
			}
			
			//points for Regalia of Empire
			if(pPS->Inventory.mfDoIHaveItem(REGALIA_OF_EMPIRE) && !bWeHaveRegaliaOfEmpire)
			{
				accumPoints += 20;
				bWeHaveRegaliaOfEmpire = TRUE;
			}
			
			//points for State Crown of Anuire
			if(pPS->Inventory.mfDoIHaveItem(STATE_CROWN_OF_ANUIRE) && !bWeHaveCrownOfAnuire)
			{
				accumPoints += 20;
				bWeHaveCrownOfAnuire = TRUE;
			}
			
			//points for Banner of Roele
			if(pPS->Inventory.mfDoIHaveItem(BANNER_OF_ROELE) && !bWeHaveBannerOfRoele)
			{
				accumPoints += 10;
				bWeHaveBannerOfRoele = TRUE;
			}
			
			//BIG POINTS FOR Gorgons Banner
			if(pPS->Inventory.mfDoIHaveItem(GORGONS_BANNER) && !bWeHaveTheGorgonsBanner)
			{
				accumPoints += 100;
				bWeHaveTheGorgonsBanner = TRUE;
			}
		
			// finally scan my inventory for relics o power
			InventoryItor itor(pPS->Inventory);
			LONG i;
			
			for (i = 0, itor.mfAdvance(IC_MAGITEM);
				 itor != pPS->Inventory.end(); 
				 ++i, itor.mfAdvanceToNext(IC_MAGITEM))
			{
				if(GAME_TTYPE::mfIsRelicOfPower(ItemTypes[(*itor)->mfType()].mfGameType()))
					accumPoints += 10;
			}
		
		}
	}
	
	return accumPoints;
}		

/* ========================================================================
   Function    - QueryCommandBattle
   Description - Do you want to command this battle?
   Returns     - 0-No 1-YES 2-RETREAT
   ========================================================================  */
SHORT QueryCommandBattle (BOOL fAddYes)
{
	ShowMenu(D_BTLCOMMAND);
	
	if (fAddYes)
	{
		SetButtonLabelColor(D_BTLCOMMAND, 333, BTN_LABEL_COLOR);
		activate_region (333, TRUE);
	}
	else
	{
		SetButtonLabelColor(D_BTLCOMMAND, 333, GREY);
		activate_region (333, FALSE);
	}

	fCommandBattleLoop = TRUE;
	SysForceCursor();
	while(fCommandBattleLoop)
		MenuLoop();

	return sCommandBattle;
}

/* ========================================================================
   Function    - SetCommandBattlePaint
   Description - paint routine for the Going To Battle...
   Returns     - 
   ======================================================================== */
void SetCommandBattlePaint(LONG, LONG)
{
	CHAR	temp[80];
	CHAR	buffer[80];
	LONG	mx,my;
	LONG	X,Y;
	LONG	mw,mh;
	LONG	halfw;
	SHORT	UnitCount = 0;
	SHORT	UnitIndex = 0;
	SHORT	scale;
	
	/* -----------------------------------------------------------------
	   print dialog title
	   ----------------------------------------------------------------- */

	if( GetButtonPosition( D_BTLCOMMAND, 0, &mx, &my ) == fERROR)
		return;
	
	if( GetButtonSize( D_BTLCOMMAND, 0, &mw, &mh ) == fERROR)
		return;
	
	X = mx + (mw / 2);
	Y = my + 25;
	
	init_gfont( FONT_SANS_16PT );
	
	strcpy( buffer, STRMGR_GetStr( BTL_COMMAND_BATTLE1 )); 
	print_text_centered( X, Y, buffer, DKBROWN );
	Y += 20;
	
	strcpy( buffer, STRMGR_GetStr( BTL_COMMAND_BATTLE2 )); 
	print_text_centered( X, Y, buffer, DKBROWN );
	Y += 20;
	
	strcpy( temp, STRMGR_GetStr( BTL_COMMAND_BATTLE3 )); 
	sprintf(
		buffer, 
		temp, 
		realm[units[SCENE_MGR::AwayIndex].Realm].mfGetName(),
		province[units[SCENE_MGR::AwayIndex].province].name
		);
	
	// calculate scale
	scale = ((mw-40)*100/gtext_width(buffer));
	if(scale > 100)
	{
		print_text_centered( X, Y, buffer, DKBROWN);
	}
	else
	{
		print_textf( mx+20, Y, DKBROWN, "^X%03d%s", scale, buffer);
	}
	
	//print_text_centered( mx, my, buffer, DKBROWN);
				
	/* -----------------------------------------------------------------
	   print unit list for in the text box
	   ----------------------------------------------------------------- */

	if( GetButtonPosition( D_BTLCOMMAND, 1, &mx, &my ) == fERROR)
		return;
	
	if( GetButtonSize( D_BTLCOMMAND, 1, &mw, &mh ) == fERROR)
		return;
	
	halfw = mw/2;
	
	init_gfont( FONT_SANS_8PT );
	
	/* -----------------------------------------------------------------
	   Home Unit List
	   ----------------------------------------------------------------- */

	X = mx;
	Y = my;
	
	UnitIndex = SCENE_MGR::HomeIndex;
	strcpy( temp, STRMGR_GetStr( BTL_COMMAND_BATTLE4 )); 
	sprintf( buffer,temp, realm[units[UnitIndex].Realm].mfGetName() );
	gprint_text( X, Y, buffer, BLUE );
	
	Y += 15;
	crease( X, Y, halfw-40, 1 );			// seperator
	
	while( UnitIndex != -1 )
	{
		// no more then gUnitStackSize units may ever go to a single battle
		if( ++UnitCount > gUnitStackSize )
			break;
			
		Y += 12;
		sprintf( buffer, "%s", STRMGR_GetStr(gsUnitTitle[units[UnitIndex].Icon]) );
		gprint_text( X, Y, buffer, BLUE );
	
		UnitIndex = (SHORT) units[UnitIndex].NextUnit;
	}
	
	X = mx + halfw;
	Y = my;
	
	UnitIndex = SCENE_MGR::AwayIndex;
	strcpy( temp, STRMGR_GetStr( BTL_COMMAND_BATTLE4 )); 
	sprintf( buffer, temp, realm[units[UnitIndex].Realm].mfGetName() );
	gprint_text( X, Y, buffer, RED-5 );
	
	Y += 15;
	crease( X, Y, halfw-40, 1 );			// seperator
	
	UnitCount = 0;
	init_gfont( FONT_SANS_8PT );
	while( UnitIndex != -1 )
	{
		// no more then gUnitStackSize units may ever go to a single battle
		if( ++UnitCount > gUnitStackSize )
			break;
			
		Y += 10;
		sprintf( buffer, "%s", STRMGR_GetStr(gsUnitTitle[units[UnitIndex].Icon]) );
		gprint_text( X, Y, buffer, RED-5 );
	
		UnitIndex = (SHORT) units[UnitIndex].NextUnit;
	}
	
}

/* ========================================================================
   Function    - SetCommandBattleProc
   Description - 
   Returns     - 
   ======================================================================== */
void SetCommandBattleProc(LONG, LONG button)
{
	HideMenu(D_BTLCOMMAND);
	RunMenus();
	
	if(button == QUESTION_BTN1)	// Yes, command this battle
	{
		sCommandBattle = 1;

#if defined(_NETJUNEDEMO)
		ShowAdScreen(3);
		sCommandBattle = 0;
#endif	
	}
	else
	if(button == QUESTION_BTN2)	// Ahhhhhh, run away!
	{
		sCommandBattle = 2;
	}
	else
	if(button == QUESTION_BTN3)	// Na, let the computer decide
	{
		sCommandBattle = 0;
	}

	// turn off menu loop
	fCommandBattleLoop = FALSE;
}

/* ========================================================================
   Function    - PrepSpellBoxes
	Description - 
	Returns     - 
	======================================================================== */
void PrepSpellBoxes ( SHORT UnitIndex )
{
	SHORT LoopCount = MAX_UNITS;	// can't look at more then this many people
	SHORT SpellLoopCount = 20;	// can't look at more then this many people
	do
	{
		if (units[UnitIndex].id >= 1000 && units[UnitIndex].id < 3000)
		{
			SHORT const hPlayerStats = LoadStats(units[UnitIndex].id, fERROR);
			DumbHandlePtr<PLAYER_STATS> const dhPlayerStats(hPlayerStats);
			
			// -- first clear out all memorized spells
			dhPlayerStats->Inventory.ClearSpells(hPlayerStats);
			
			// -- scan magic list and memorize and load hotboxes for
			// -- as many spells as we can
			for (LONG jkl = 0; jkl < NUM_BATTLE_SPELLS; ++jkl )
			{
				// -- must be a mage to use anything other then
				// -- magic missiles
				
				if ( jkl < ITEM_NECKLACE_OF_MISSILES_8 &&
					!dhPlayerStats->mfIsMagicUser() )
					continue;
					
				while ( 
					dhPlayerStats->Inventory.PrepBattleSpell( 
						battle_spells[jkl], 
						hPlayerStats )
					)
				{
					/* keep adding until false or looked too many times */
					if (!--SpellLoopCount)
						break;
				}	
				SpellLoopCount = 20;
			}
		}
		UnitIndex = (SHORT) units[UnitIndex].NextUnit;
	} while (--LoopCount > 0 && UnitIndex != fERROR);
}	

/* ========================================================================
   Function    - BattlePrep
   Description - Who has magic
   Returns     - TRUE YES or FALSE NO
   ========================================================================  */
BOOL BattlePrep(void)
{
	SHORT	UnitIndex;
	BOOL	RetVal = FALSE;

	// check the home team
	UnitIndex = SCENE_MGR::HomeIndex;
	fBattlePrep = TRUE;
	
	PrepSpellBoxes(SCENE_MGR::HomeIndex);
	
	// check the away team
	#ifdef _WINDOWS
	// prep non-playercontrolled opponents
	if ( !(IsMultiPlayer() && realm[SCENE_MGR::AwayIndex].mfIsPlayerCtrl()) )
	#endif
		PrepSpellBoxes(SCENE_MGR::AwayIndex);

	fBattlePrep = FALSE;	

	return RetVal;
}

/* ========================================================================
   Function    - BattlePrepProc
   Description - handle the done button on the prep dialog
   Returns     - 
   ======================================================================== */
void BattlePrepProc(LONG MenuCombo, LONG)
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
	// turn off menu loop
	fCommandBattleLoop = FALSE;
	fPreparingSpells=FALSE;
}

/* ========================================================================
   Function    - DisplayNPCResults
   Description - tell the results of a NPC battle
   Returns     - 
   ======================================================================== */
void DisplayNPCResults(void)
{
	BOOL	fContinueLoop = TRUE;

	// set the proc to null
	SetButtonProc  (D_QUESTION1, 0, PaintNPCResults, 0, 0 );

	// set the question text
	SetButtonLabel  (D_QUESTION1, QUESTION_TEXT, -1, BLACK );

	// turn off the middle button
	SetButtonLabel  (D_QUESTION1, QUESTION_BTN1, STR_OK, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION1, QUESTION_BTN1, ToggleVal, (LONG)&fContinueLoop, D_KEY_OK );

	ShowMenu(D_QUESTION1);

	// handle join dialog
	SysForceCursor();
	if (fContinueLoop)
	{
		while (fContinueLoop)
			MenuLoop();
		clear_key_status(0);
	}

	HideMenu(D_QUESTION1);
}

/* ========================================================================
   Function    - PaintNPCResults
   Description - paint routine for NPC results
   Returns     - 
   ======================================================================== */
void PaintNPCResults(LONG, LONG)
{
	CHAR	format[200];
	CHAR	buffer[200];
	LONG	mx, my;
	CHAR	plural;

	if(sUnitsLostWinner == 1)
		plural = ' ';
	else
		plural = 's';

	if (sUnitsLostWinner == 0 && sUnitsLostLoser == 0)
	{
		if (SCENE_MGR::VictorRealm == HomeRealm) // you won, they retreated
		{
			char cpLosingRealmName[80];
			
			strcpy(cpLosingRealmName, realm[SCENE_MGR::LoserRealm].mfGetName());
			strcpy(format,STRMGR_GetStr(STR_NPC_WE_RETREAT));
			// strcpy(format,"^W270The cowards %s %s ran from the field of battle.  They are not worthy of our hate.");
			sprintf(buffer,format, 
				STRMGR_GetStr((((REALM::REALM_TYPE)SCENE_MGR::LoserRealm==REALM::MONSTERS)?STR_NPC_OF:STR_NPC_FROM)),
				cpLosingRealmName);
		}
		else
		{
			strcpy(format,STRMGR_GetStr(STR_NPC_THEY_RETREAT));
			//strcpy(format,"^W270Wisely we retreat from this battle with %s.  We will regroup in the next province.");
			sprintf(buffer,format, realm[SCENE_MGR::VictorRealm].mfGetName());
		}
	}
	else
	if (SCENE_MGR::VictorRealm == HomeRealm) // you won
	{
		char cpLosingRealmName[80];
		
		strcpy(cpLosingRealmName, realm[SCENE_MGR::LoserRealm].mfGetName());
		strcpy(format,STRMGR_GetStr(STR_NPC_WON));
		sprintf(buffer,format,
			STRMGR_GetStr((((REALM::REALM_TYPE)SCENE_MGR::LoserRealm==REALM::MONSTERS)?STR_NPC_OF:STR_NPC_FROM)),
			cpLosingRealmName,
			sUnitsLostWinner,
//			plural,
			sUnitsLostLoser);
	}
	else
	{
		char cpVictorRealmName[80];
		
		strcpy(cpVictorRealmName,realm[SCENE_MGR::VictorRealm].mfGetName());
		strcpy(format,STRMGR_GetStr(STR_NPC_LOST));
		sprintf(buffer,format,
			STRMGR_GetStr((((REALM::REALM_TYPE)SCENE_MGR::VictorRealm==REALM::MONSTERS)?STR_NPC_OF:STR_NPC_FROM)),
			cpVictorRealmName,
			sUnitsLostLoser,
//			plural,
			sUnitsLostWinner);
	}

	// get position of lose screen
	if( GetButtonPosition( D_QUESTION1, 1, &mx, &my ) == fERROR)
		return;

	mx += 20;
	my += 20;

 	// paint dialog title
	gprint_text(mx,my, buffer, BLACK);

}


/* ========================================================================
   Function    - GetMagicUserPower
   Description - Calculates magic power of the given unit.
   Returns     - Magic Power.
	Note			- We added this because NPC combat against magic users
					  was not giving enough weight to magic users.  For example,
					  a Baruk-Azhik army could easily defeat a Sielwode army led
					  by Isaelie, which should not happen.
	[d2-25-97 JPC]
   ======================================================================== */
static LONG GetMagicUserPower (SHORT UnitIndex)
{
	if (IsRegentOrLieutenant (UnitIndex))
	{
		SHORT	hPlayerStats;

		hPlayerStats = LoadStats(units[UnitIndex].id, fERROR);
		if (hPlayerStats != fERROR)
		{
			DumbHandlePtr<PLAYER_STATS const> const dhPlayerStats(hPlayerStats);
         PLAYER_CLASS_INFO::TYPE type;
			LONG level;

			if (dhPlayerStats->mfIsMagicUser (&type, &level))
			{
				switch (type)
				{
					case PLAYER_CLASS_INFO::PRIEST:
					case PLAYER_CLASS_INFO::BARD:
#if defined (_DEBUG)
						printf ("Magic power = %d\n", level);
#endif
						return level;
					case PLAYER_CLASS_INFO::WIZARD:
						return (3 * level) / 2;
				}
			}
		}
	}
	return 0;
}

/* ========================================================================
   Function    - UnitPower
	Description - calculate the power of a unit
	Returns     - power number
	======================================================================== */
SHORT UnitPower( SHORT UnitIndex )
{
	
	// -----------------------------------------------------------------
	// -- Your get points equal to your melee, charge
	// -- added together plus 2 * your missile rating, 1.5 your defence,
	// -- your magic level, pluse 6 points for being royalty and 3 points
	// -- for being blessed
	// -----------------------------------------------------------------

	return ((SHORT)( GetMagicUserPower (UnitIndex) 
		+ (IsRegentOrLieutenant(UnitIndex) ? 6 : 0 )
		// GEH later...   + (units[UnitIndex].Blessed ? 3 : 0 )
		+ (GetBattleMissile(units[UnitIndex].Icon) * 2)
		+ GetBattleCharge(units[UnitIndex].Icon) 
		+ GetBattleMelee(units[UnitIndex].Icon) 
		+ ((3 * GetBattleDefense(units[UnitIndex].Icon)) / 2) ) );
}

/* ========================================================================
   Function    - GetLastUnit
	Description - find the last unit in a stack
	Returns     - unit index
	======================================================================== */
SHORT GetLastUnit( SHORT FirstUnit )
{
	SHORT lastUnit=FirstUnit,k;
	
	while (units[lastUnit].NextUnit != fERROR )
	{
		k = units[lastUnit].NextUnit;
		
		// end of list, Im the last guy
		if ( k == fERROR )
			break;
			
		// next guy in the list is a stack head, Im last guy of this stack
		if ( units[k].Joined == 0)
			break;
		
		lastUnit = units[lastUnit].NextUnit;
	}
	
	return lastUnit;
}

/* ========================================================================
   Function    - NPCBattleResults
   Description - fight a battle between NPC's
   Returns     - side-effect in SCENE_MGR::Victor and SCENE_MGR::Loser
   ======================================================================== */
void NPCBattleResults (BOOL fSend)
{
	SHORT	UnitIndex;
	SHORT	HomePower = 0;
	SHORT	AwayPower = 0;
	SHORT	HomeUnit = 0;
	SHORT	AwayUnit = 0;
	SHORT	HomeCount = 0;
	SHORT	AwayCount = 0;
	SHORT	HomeIndex = 0;
	SHORT	AwayIndex = 0;
	SHORT	HomeUnitPower, AwayUnitPower;
	SHORT HomeUnitIndex, AwayUnitIndex, Winner;
	SHORT	HomeOrigCount = 0;
	SHORT	AwayOrigCount = 0;
	SHORT HomeRealm = units[SCENE_MGR::HomeIndex].Realm;
	SHORT AwayRealm = units[SCENE_MGR::AwayIndex].Realm;
	BOOL  HomeLarger = FALSE;
	
	// printf ("NPCBattleResults\n");

	// exp points for being brave enough to fight
	BattleExpPoints( SCENE_MGR::HomeIndex, BTL_EXP_ENGAGE, 0, FALSE );
	BattleExpPoints( SCENE_MGR::AwayIndex, BTL_EXP_ENGAGE, 0, FALSE );
	
	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		char temp[150];
		sprintf ( temp, "NPCBattleResults - List of %s Home Units:",
			realm[units[SCENE_MGR::HomeIndex].Realm].mfGetName());
		RandomLogComment ( temp );
	}
	
	for (UnitIndex = SCENE_MGR::HomeIndex;
		 UnitIndex != fERROR;
		 UnitIndex = (SHORT) units[UnitIndex].NextUnit)
	{
		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
			char temp[150];
			sprintf ( temp, "       Realm:%s  Index:%03d Unit:%s",
				realm[units[UnitIndex].Realm].mfGetName(),
				UnitIndex,
				UnitName(UnitIndex)
				);
			RandomLogComment ( temp );
		}
	
		++HomeOrigCount;
	}
	
	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		char temp[150];
		sprintf ( temp, "NPCBattleResults - List of %s Away Units:",
			realm[units[SCENE_MGR::AwayIndex].Realm].mfGetName());
		RandomLogComment ( temp );
	}
	
	for (UnitIndex = SCENE_MGR::AwayIndex;
		 UnitIndex != fERROR;
		 UnitIndex = (SHORT) units[UnitIndex].NextUnit)
	{
		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
			char temp[150];
			sprintf ( temp, "       Realm:%s  Index:%03d Unit:%s",
				realm[units[UnitIndex].Realm].mfGetName(),
				UnitIndex,
				UnitName(UnitIndex)
				);
			RandomLogComment ( temp );
		}
	
		++AwayOrigCount;
	}
	
	// -----------------------------------------------------------------
	// -- Fight Battle
	// -----------------------------------------------------------------

	// -- Get last unit in list
	HomeUnitIndex = GetLastUnit(SCENE_MGR::HomeIndex);
	AwayUnitIndex = GetLastUnit(SCENE_MGR::AwayIndex);
	// -- Get power of these units
	HomeUnitPower = UnitPower(HomeUnitIndex);
	HomeUnitPower += random(HomeUnitPower/2);
	AwayUnitPower = UnitPower(AwayUnitIndex);
	AwayUnitPower += random(AwayUnitPower/2);
	
	if ( HomeUnitPower > AwayUnitPower )
		HomeLarger = TRUE;
		
	// -- check if they have a place to run to or are they cornered?
	fHomeRetreatPath = CheckWithdrawPath (HomeUnitIndex);
	fAwayRetreatPath = CheckWithdrawPath (AwayUnitIndex);
		
	while (TRUE)
	{
		// -- If there are no more units, declair winner
		if (AwayUnitIndex == fERROR)
		{
			goto HomeWins;
		}
		else
		if (HomeUnitIndex == fERROR)
		{
			goto AwayWins;
		}
		
		// -----------------------------------------------------------------
		// -- calc the total power of the unit lists
		// -----------------------------------------------------------------

		HomePower = 0;
		for (UnitIndex = SCENE_MGR::HomeIndex;
			 UnitIndex != fERROR;
			 UnitIndex = (SHORT) units[UnitIndex].NextUnit)
		{
			HomePower += UnitPower(UnitIndex);
		}
		
		AwayPower = 0;
		for (UnitIndex = SCENE_MGR::AwayIndex;
			 UnitIndex != fERROR;
			 UnitIndex = (SHORT) units[UnitIndex].NextUnit)
		{
			AwayPower += UnitPower(UnitIndex);
		}
	
		// -----------------------------------------------------------------
		// -- If one side is great enough in power over the other
		// -- then must have them retreat, except in the case
		// -- where they have no where to run, them fight to the death
		// -----------------------------------------------------------------
		if ( fHomeRetreatPath && (HomePower * 3) < AwayPower )
		{
				goto AwayWins;
		}
		else
		if ( fAwayRetreatPath && (AwayPower * 3) < HomePower )
		{
				goto HomeWins;
		}
		
		// -----------------------------------------------------------------
		// -- Fight battle, kill loser, setup next unit in loser list
		// -- note that the power of the winning unit is reduced due
		// -- to battle casulties
		// -----------------------------------------------------------------
		if (HomeUnitPower > AwayUnitPower)
		{
			// -- check for special case units
			if (units[AwayUnitIndex].Icon == GORGON_UNIT_ICON 
				|| units[AwayUnitIndex].Icon == RHUOBHE_UNIT_ICON )
			{
				goto HomeWins; // -- You win, but they don't die
			}
			
			// -- check for special case units before killing
			if ( IsRegent(AwayUnitIndex) )
			{
				// on a random chance, a regent dies
				if(!random(3))
					goto HomeWins;
			}
			
			// -- check for top of stack before killing
			if ( units[AwayUnitIndex].Joined == 0)
			{
				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					char temp[150];
					sprintf ( temp, "   Deleting TOS Away Realm:%s  Index:%03d Unit:%s",
						realm[units[AwayUnitIndex].Realm].mfGetName(),
						AwayUnitIndex,
						UnitName(AwayUnitIndex)
						);
					RandomLogComment ( temp );
				}
	
				// -- delete top of stack and leave
				DeleteUnit(AwayUnitIndex, fSend);
				++AwayCount;
				goto HomeWins;
			}
			else
			{
				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					char temp[150];
					sprintf ( temp, "   Deleting Away Realm:%s  Index:%03d Unit:%s",
						realm[units[AwayUnitIndex].Realm].mfGetName(),
						AwayUnitIndex,
						UnitName(AwayUnitIndex)
						);
					RandomLogComment ( temp );
				}
	
				// -- otherwise, delete loser and get next dude
				DeleteUnit(AwayUnitIndex, fSend);
				++AwayCount;
				
				// -- decrease winning unit power by a factor
				HomeUnitPower -= AwayUnitPower/2;
				
				// -- get next last unit
				AwayUnitIndex = GetLastUnit(SCENE_MGR::AwayIndex);
				AwayUnitPower = UnitPower(AwayUnitIndex);
				AwayUnitPower += random(AwayUnitPower/2);
				
			}
		}
		else
		if (AwayUnitPower > HomeUnitPower)
		{
			// -- check for special case units
			if (units[HomeUnitIndex].Icon == GORGON_UNIT_ICON 
				|| units[HomeUnitIndex].Icon == RHUOBHE_UNIT_ICON )
			{
				goto AwayWins; // -- You win, but they don't die
			}
			
			// -- check for special case units before killing
			if ( IsRegent(HomeUnitIndex) )
			{
				// -- on a random chance, the regent dies
				if(!random(3))
					goto AwayWins;
			}
			
			// -- check for top of stack before killing
			if ( units[HomeUnitIndex].Joined == 0)
			{
				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					char temp[150];
					sprintf ( temp, "   Deleting TOS Home Realm:%s  Index:%03d Unit:%s",
						realm[units[HomeUnitIndex].Realm].mfGetName(),
						HomeUnitIndex,
						UnitName(HomeUnitIndex)
						);
					RandomLogComment ( temp );
				}
	
				// -- delete top of stack and leave
				DeleteUnit(HomeUnitIndex, fSend);
				++HomeCount;
				goto AwayWins;
			}
			else
			{
				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					char temp[150];
					sprintf ( temp, "   Deleting Home Realm:%s  Index:%03d Unit:%s",
						realm[units[HomeUnitIndex].Realm].mfGetName(),
						HomeUnitIndex,
						UnitName(HomeUnitIndex)
						);
					RandomLogComment ( temp );
				}
	
				// -- otherwise, delete loser and get next dude
				DeleteUnit(HomeUnitIndex, fSend);
				++HomeCount;
				
				// -- decrease winning unit power by a factor
				AwayUnitPower -= HomeUnitPower/2;
				
				// -- get next last unit
				HomeUnitIndex = GetLastUnit(SCENE_MGR::HomeIndex);
				HomeUnitPower = UnitPower(HomeUnitIndex);
				HomeUnitPower += random(HomeUnitPower/2);
			}
		}
		else // -- tie
		{
			// -- adjust in favor of the biggest army
			HomeUnitPower /= (AwayPower/HomePower)+1;
			AwayUnitPower /= (HomePower/AwayPower)+1;
			// -- final check
			if (HomeUnitPower == AwayUnitPower)
			{
				// -- we always win!
				AwayUnitPower = 0;
				HomeUnitPower = 1;
			}
		}
	}
	
	/* -----------------------------------------------------------------
	   Decide winner
	   ----------------------------------------------------------------- */
HomeWins:
	SCENE_MGR::Victor = SCENE_MGR::HomeIndex;
	SCENE_MGR::Loser = SCENE_MGR::AwayIndex;
	SCENE_MGR::VictorRealm = HomeRealm;
	SCENE_MGR::LoserRealm  = AwayRealm;
	sUnitsLostWinner = HomeCount;
	sUnitsLostLoser = AwayCount;
	
	// -- if either side wiped out, signal by setting handle to 0
	if (( HomeOrigCount - HomeCount) == 0 )
		SCENE_MGR::Victor = 0;
	if (( AwayOrigCount - AwayCount) == 0 )
		SCENE_MGR::Loser = 0;
		
	BattleExpPoints( SCENE_MGR::HomeIndex, BTL_EXP_WIN, 0, FALSE );
	if ( !HomeLarger )
		BattleExpPoints( SCENE_MGR::HomeIndex, BTL_EXP_WIN_GREATER_STRENGTH, 0, FALSE );
	
	return;

AwayWins:	
	SCENE_MGR::Victor = SCENE_MGR::AwayIndex;
	SCENE_MGR::Loser = SCENE_MGR::HomeIndex;
	SCENE_MGR::VictorRealm = AwayRealm;
	SCENE_MGR::LoserRealm  = HomeRealm;
	sUnitsLostWinner = AwayCount;
	sUnitsLostLoser = HomeCount;
	
	// -- if either side wiped out, signal by setting handle to 0
	if (( AwayOrigCount - AwayCount) == 0 )
		SCENE_MGR::Victor = 0;
	if (( HomeOrigCount - HomeCount) == 0 )
		SCENE_MGR::Loser = 0;
		
	BattleExpPoints( SCENE_MGR::AwayIndex, BTL_EXP_WIN, 0, FALSE );
	if ( HomeLarger )
		BattleExpPoints( SCENE_MGR::AwayIndex, BTL_EXP_WIN_GREATER_STRENGTH, 0, FALSE );
	
	return;
}

/* ========================================================================
   Function    - 
	Description - 
	Returns     - 
	======================================================================== */
#if defined(_JUNEDEMO)

void ShowAdScreen(SHORT whichScreen)
{
	SHORT bitm = fERROR;
	LONG	now;
	
	switch (whichScreen)
	{
		case 0:
			bitm = GetResourceStd("graphics\\noadven.pcx", FALSE);
			if (bitm == fERROR)
				return;
			DrawBitmap (0, 0, bitm, 0, 0, 640, 480);
			gprint_text(155,335, STRMGR_GetStr(STR_DEMO_NOADV), BLACK);
			break;
		
		case 1:
		case 2:
			bitm = GetResourceStd("graphics\\keepplay.pcx", FALSE);
			if (bitm == fERROR)
				return;
			DrawBitmap (0, 0, bitm, 0, 0, 640, 480);
			#define CYAN 75
			#define GOLD 252
			gprint_text(200, 26, STRMGR_GetStr(STR_DEMO_KEEPPLAY1), GOLD);
			gprint_text(200,152, STRMGR_GetStr(STR_DEMO_KEEPPLAY2), WHITE);
			gprint_text(200,202, STRMGR_GetStr(STR_DEMO_KEEPPLAY3), GOLD);
			gprint_text(200,226, STRMGR_GetStr(STR_DEMO_KEEPPLAY4), GOLD);
			gprint_text(200,247, STRMGR_GetStr(STR_DEMO_KEEPPLAY5), GOLD);
			gprint_text(200,280, STRMGR_GetStr(STR_DEMO_KEEPPLAY6), CYAN);
			gprint_text(200,303, STRMGR_GetStr(STR_DEMO_KEEPPLAY7), CYAN);
			gprint_text(200,427, STRMGR_GetStr(STR_DEMO_KEEPPLAY8), CYAN);
			gprint_text(200,449, STRMGR_GetStr(STR_DEMO_KEEPPLAY9), CYAN);
			break;
			
		case 3:
			bitm = GetResourceStd("graphics\\nobattle.pcx", FALSE);
			if (bitm == fERROR)
				return;
			DrawBitmap (0, 0, bitm, 0, 0, 640, 480);
			gprint_text(150,293, STRMGR_GetStr(STR_DEMO_NOBATTLE), BLACK);
			break;
		
		case 4:
			bitm = GetResourceStd("graphics\\keepplay.pcx", FALSE);
			if (bitm == fERROR)
				return;
			DrawBitmap (0, 0, bitm, 0, 0, 640, 480);
			init_gfont( FONT_SANS_8PT );
			gprint_text(56,460, "The Sword of Roele must be given to Caliedhe Dosiere.", WHITE);
			break;
			
		default:
			break;	
	}
		
	update_screen();
	
	now = get_time() + (20*182/10);
	
#ifdef _WINDOWS
	ClearMessageQueue();
#endif	

	fAnyKeyChanged = FALSE;
	clear_key_status(0);
	
	while (mouse_button > 0)
	{
		mouse_button = 0;
		update_buttons();
	}
	
	while (get_time() < now)
	{
		run_timers();
	
	#ifdef _WINDOWS
		ClearMessageQueue();
	#endif	
	
		update_buttons();
		if (mouse_button)
		{
			mouse_button = 0;
			break;
		}
		if (fAnyKeyChanged)
		{
			fAnyKeyChanged = FALSE;
			clear_key_status(0);
			break;
		}
	}
}

#endif
/*	========================================================================*/
