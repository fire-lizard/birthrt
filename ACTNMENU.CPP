/* =======================================================================
	Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: ACTNMENU.CPP
   Authors: Alan Clark, Lan Zhou, & Michael Branham
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:

   ======================================================================= */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#ifdef _WINDOWS
#include <Windows.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include "SYSTEM.H"

#include "ACTNMENU.HXX"
#include "CHARSEL.HXX"
#include "SOUND.HXX"
#include "UNITS.HXX"
#include "REALM.HXX"
//#include "rlmspell.hxx"
#include "GAMEMAP.HXX"
#include "MAPAI.HXX"
#include "MENU.H"
#include "REQUEST.H"
#include "PLAYSTAT.HXX"
#include "REGENTS.HXX"
#include "GAME.H"
#include "ACTIONS.HXX"
#include "STRMGR.H"
#include "strenum.h"
#include "REPORT.HXX"
#include "MULTIMAP.HXX"
#include "THINGTYP.H"
#include "MULTIUI.HXX"
#include "MAIN.HXX"
#include "MENU.H"
#include "PANEL.H"
#include "PLACES.HXX"
#include "HANDLE.HXX"
#include "RLMSPELL.HXX"
#include "ITEMUTIL.HXX"

#ifdef _WINDOWS
#include "WINSYS\MULPLAY.HXX"
#endif

extern BOOL fLogComment;

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
//#define  LOGCOMMENTSLEEP	Sleep(1);
#define  LOGCOMMENTSLEEP	{;}

#define D20TOPERCENT(a)		((mundane_descriptions)?(105-((a)*5)):(a))
#define PERCENTCHAR			((mundane_descriptions)?'%':0)

#define RULE_LAW		0
#define RULE_GUILD	1
#define RULE_TEMPLE	2
#define RULE_SOURCE	3
#define RULE_PROV		4

#define DKBLUE					60
#define BLUE					64
#define DKRED					112
#define MDRED					120
#define MDGREEN				188
#define DKBROWN				128
#define MDBROWN				136
#define ORANGE					142
#define DKTAN					209
#define MDTAN					213
#define LTTAN					223
#define COPPER_GRAD			224

#define BEFORE_LTACTION			0
#define DURING_LTACTION			1
#define AFTER_LTACTION			2

#define LARGE						0
#define SMALL						1

#define REGENTNAME				160
#define NUMDIPSUBJ				11

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
void CleanupAfterActnMenu (void);
static void DeleteActnMenuRegions (void);
void DoneHoldAction(LONG, LONG);
void CancelHoldAction(LONG, LONG);
void DispSuccess (void);
void MapAIPlayerResponseWaitSync (void);
void UpdateHoldingFlag (void);
void SuspendTimeLimit (void);
void InitActionGlobals (void);
LONG cntUnits_ (void);
LONG UnitsHere (PROVINCE prov, REALM::REALM_TYPE realm);

extern void GainRealmExp(LONG RegentIndex, const LONG ExpAmount,
					const BOOL CanGainLevelNow, const BOOL fSend);

extern "C" void DrawLittleButton (LONG x, LONG y, LONG w, LONG h, LONG pushFlag);
extern void ResumeTimeLimit (void);
extern void RandomLogComment ( char * szString );
extern "C" void RandomLogPrefix ( char * szString );
extern void SetRedrawMainMapLevel (void);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
SHORT usedFreeAction = 0;	// extra actions you've used this turn
SHORT usedFreeForge = 0;	// number of LL's you've forged as free actions
SHORT usedFreeMagic = 0;	// number of Realm Spells cast as free actions
SHORT usedFreeAgitate = 0;	// if priest
SHORT usedFreeSpy = 0;		// if thief

static LONG			startProvCount = 1;
static LONG			startProvCount1 = 1;
static LONG			startProvCount2 = 1;
static SHORT		iCurAction = 0;
static BOOL			musterSkl = 0;
static BOOL 		firstRollFlag = 0;
SHORT				oldAction = 1;
static BOOL 		checkGoldReg = 0;
LONG 				fAllowCancelButton = TRUE;
LONG 				fAllowRollButton = FALSE;
LONG 				fAllowDoneButton = TRUE;
LONG 				fAllowRollButton_others = FALSE;
LONG 				fAllowDoneButton_others = TRUE;
BOOL				fPlayerNeverDeclaredWar = TRUE;
static LONG 		GoldBar = -2;
static LONG			GoldGained = 0;
static LONG 		RegencePoint = -2;
static LONG			iSelectedRealm = REALM::NO_COUNTRY;
static UBYTE 		modGoldBars = 0;
UBYTE				modRegency=0;
static BOOL   		NoFlag = FALSE;
LONG				modOpponent = 0;
static LONG			modLevel = 0;
static LONG			reportFlag = 0;
static LONG			iNewScreen = 0;
static LONG			iCancel = 1; 
static LONG			additionalamt = 0;
static LONG			startPlace = 0;
static PROVINCE 	iSelectedProvince = NO_PROVINCE;
static LONG 		iSelectedOneHolding = 99;
static LONG 		iSelectedUnit = 0;

LONG				fOtherDipMP = FALSE;  //---- Multiplayer diplomacy sync flag 
										  //---- should be true when called from mapai.cpp
										  //---- Sync point #5 should be done	

extern USHORT	TargetPlace;
extern SHORT	iProvMap;
extern BOOL		fTutorialSelected;
extern BOOL		fMenuClosed;
extern LONG		fControlMode;
extern USHORT 	StartPlace;
extern LONG		WaitingWhen;
extern LONG		OldNPCRegency;
extern LONG		OldNPCRealm;

DEFINE_STATIC_VECTOR_DATA_S(LONG,iSelection,NUMDIPSUBJ);
DEFINE_VECTOR_CLASS(LONG, iSelection);

DEFINE_STATIC_VECTOR_DATA_S(LONG,iSelectedMulUnits,20);
DEFINE_VECTOR_CLASS(LONG,iSelectedMulUnits);

DEFINE_STATIC_VECTOR_DATA_S(UBYTE,iSelectedMulProvince,16);
DEFINE_VECTOR_CLASS(UBYTE, iSelectedMulProvince);

DEFINE_MATRIX_DATA_S(UBYTE,iSelectedHolding,PROVINCE_COUNT,4);
DEFINE_MATRIX_CLASS_S(UBYTE, iSelectedHolding,PROVINCE_COUNT,4);

DEFINE_VECTOR_DATA_S(UBYTE,iLieutenants,9)={0, 0, 0, 0, 0, 0, 0, 0, 0};
DEFINE_VECTOR_CLASS(UBYTE, iLieutenants);

DEFINE_STATIC_VECTOR_DATA(LONG,HoldingType) = {RULE_LAW,RULE_GUILD,RULE_TEMPLE,RULE_SOURCE};
DEFINE_VECTOR_CLASS(LONG, HoldingType);

DEFINE_VECTOR_DATA(MAP_ICON,InfoReqType) = {LAW1_ICON,GUILD1_ICON,TEMPLE1_ICON,SOURCE1_ICON,CASTLE1_ICON,WIZTOWER_ICON, ANY_MAP_ICON};
DEFINE_VECTOR_CLASS(MAP_ICON,InfoReqType);

DEFINE_STATIC_VECTOR_DATA(GAME_STRING,gsHoldType) = {STR_ACTNMENU_CAPS_LAW,STR_ACTNMENU_CAPS_GUILD,STR_ACTNMENU_CAPS_TEMPLE,STR_ACTNMENU_CAPS_SOURCE,STR_ACTNMENU_CAPS_PROVINCE};
DEFINE_VECTOR_CLASS(GAME_STRING, gsHoldType);

DEFINE_VECTOR_DATA(GAME_STRING,gsAllHold) = {STR_ACTNMENU_LAW,STR_ACTNMENU_GUILD,STR_ACTNMENU_TEMPLE,STR_ACTNMENU_SOURCE,STR_ACTNMENU_CASTLE,STR_ACTNMENU_WIZTOWER,STR_NULL};
DEFINE_VECTOR_CLASS(GAME_STRING, gsAllHold);

DEFINE_STATIC_VECTOR_DATA(GAME_STRING,gsLoyaltyUC) = {STR_ACTNMENU_LOYALTY_REBEL,STR_ACTNMENU_LOYALTY_POOR,STR_ACTNMENU_LOYALTY_AVERAGE,STR_ACTNMENU_LOYALTY_HIGH};
DEFINE_VECTOR_CLASS(GAME_STRING, gsLoyaltyUC);

DEFINE_VECTOR_DATA(MAP_ICON,MonsterUnits)={G_INF_UNIT_ICON, G_CAV_UNIT_ICON, N_INF_UNIT_ICON, C_SKL_UNIT_ICON};
DEFINE_VECTOR_CLASS(MAP_ICON, MonsterUnits);

DEFINE_VECTOR_DATA(UBYTE,LevelUnitReqt) =  {0, 1,3,2,1,2,1,0,4,3,0,0,0,0,0, 2,3,4, 4,4, 2,2,3, 0,0, 0,0,0,0,0};
DEFINE_VECTOR_CLASS(UBYTE, LevelUnitReqt);

DEFINE_VECTOR_DATA(UBYTE,UnitMusterCost) = {0, 2,4,2,1,2,0,2,6,4,4,4,3,4,6, 4,4,8, 4,4, 1,1,4, 2,3, 4,4,4,4,8};
DEFINE_VECTOR_CLASS(UBYTE, UnitMusterCost);

static LONG 		iThisRoll;
static LONG 		iWhichRoll = 99;
static LONG			lAskedTargetWhichRoll = 99;
static LONG 		xRollLocation;
static LONG 		yRollLocation;
static UBYTE		fRollResult[128];
static UBYTE		success_roll;

DEFINE_STATIC_VECTOR_DATA(UBYTE,expertHolding)={1, 6, 4, 5, 1}; 
DEFINE_VECTOR_CLASS(UBYTE, expertHolding);

DEFINE_STATIC_VECTOR_DATA(UBYTE,expertise) = {
	/*NO_ACTION*/			0,		// no one
	/*ADVENTURE*/			0,
	/*AGITATE*/				4,		// priest
	/*BUILD_ROAD*/			0,
	/*CONTEST*/				8,		// as holding type
	/*CREATE_HOLDING*/	8,		// as holding type
	/*DECLARE_WAR*/		0,
	/*DIPLOMACY*/			9,		// any
	/*ESPIONAGE*/			6,		// thief
	/*FORGE_LEY_LINE*/	5,		// wizard
	/*FORTIFY*/				1,		// fighter
	/*HOLD_ACTION*/		0,
	/*INVESTITURE*/		0,
	/*LIEUTANANT*/			0,
	/*LT_ACTION*/			0,
	/*MUSTER*/				0,
	/*REALM_SPELL*/		10,	// wizard & priest
	/*RULE*/					8,		// as holding type
	/*TRADE_ROUTE*/		6,		// thief
};
DEFINE_VECTOR_CLASS(UBYTE, expertise);

DEFINE_VECTOR_DATA(SHORT,UnitTypeID) = {
/* NO_MAP_ICON			*/		0,
/* A_INF_UNIT_ICON	*/		INFANTRY,
/* A_EIN_UNIT_ICON	*/		ELITE_INFANTRY,
/* A_BOW_UNIT_ICON	*/		ARCHER,
/* A_IRR_UNIT_ICON	*/		IRREGULAR,
/* A_PIK_UNIT_ICON	*/		PIKEMAN,
/* A_LVY_UNIT_ICON	*/		LEVY,
/* A_SCT_UNIT_ICON	*/		SCOUT,
/* A_KNT_UNIT_ICON	*/		KNIGHT,
/* A_CAV_UNIT_ICON	*/		LIGHT_CAVALRY,
/* M_INF_UNIT_ICON	*/		MERC_INFANTRY,
/* M_BOW_UNIT_ICON	*/		MERC_ARCHER,
/* M_IRR_UNIT_ICON	*/		MERC_IRREGULAR,
/* M_PIK_UNIT_ICON	*/		MERC_PIKEMAN,
/* M_CAV_UNIT_ICON	*/		MERC_CAVALRY,
/* E_INF_UNIT_ICON	*/		ELF_INFANTRY,
/* E_BOW_UNIT_ICON	*/		ELF_ARCHER,
/* E_CAV_UNIT_ICON	*/		ELF_CAVALRY,
/* D_INF_UNIT_ICON	*/		DWARF_INFANTRY,
/* D_BOW_UNIT_ICON	*/		DWARF_ARCHER,
/* G_INF_UNIT_ICON	*/		GOBLIN_INFANTRY,
/* G_BOW_UNIT_ICON	*/		GOBLIN_ARCHER,
/* G_CAV_UNIT_ICON	*/		GOBLIN_CAVALRY,
/* N_INF_UNIT_ICON	*/		GNOLL_1,
/* N_IRR_UNIT_ICON	*/		GNOLL_IRREGULAR,
/* C_HPY_UNIT_ICON	*/		T_HARPY,
/* C_SKL_UNIT_ICON	*/		T_SKELETON,
/* C_SPD_UNIT_ICON	*/		T_GIANT_SPIDER,
/* C_HEL_UNIT_ICON	*/		T_HELL_HOUND,
/* C_WYV_UNIT_ICON	*/		T_WYVERN_1,
};
DEFINE_VECTOR_CLASS(SHORT, UnitTypeID);


extern SHORT		iDRollFIc;
extern int			reqSound;
extern BOOL			fSmallMap;
extern LONG			TutorialActionNumber;
extern LONG			TutorialScreenNumber;
extern BOOL			mundane_descriptions;

static void DoDealMenu(LONG iRealm);
#if 0
// Diplomacy structs
typedef struct
{
	unsigned int	iWhoFrom	: 6;
	unsigned int	iAlliance	: 2;
	unsigned int	iReqGold	: 5;
	unsigned int	iReqProv	: 8;
	unsigned int	iReqHold	: 10;
	unsigned int	iReqTrib	: 5;
	unsigned int	iOffProv	: 8;
	unsigned int	iOffHold	: 10;
	unsigned int	iOffTrib	: 5;
	unsigned int	iOffGold	: 5;
	unsigned int	iOffRegy	: 5;
} DIP_DATA;

typedef struct
{
	LONG	data1;
	LONG	data2;
	LONG	data3;
} DIP_DATA1;

typedef struct
{
	union
	{
		DIP_DATA  d1;
		DIP_DATA1 d2;
	};
} DIPLOMACY_DATA;
#endif
DIPLOMACY_DATA dData;


void SetCurAction(SHORT iAction)
{
	iCurAction = iAction;
	return;
}

// table of realm experience points
#define NUM_OF_EXP_CLASSES	4
static LONG RealmExpPoints[NUM_OF_EXP_TYPES][NUM_OF_EXP_CLASSES] = {
/*      Action                       		Fighter,	Priest,	Thief,	Wizard */
/* RLM_EXP_AGITATE,					  */	{	500,		1000,		1000,		1000 },
/* RLM_EXP_BUILD_ROAD,				  */	{	50,		50,		50,		50 },
/* RLM_EXP_BUILD_TRADEROUTE,		  */	{	500,		500,		3000,		500 },
/* RLM_EXP_CONTEST_GUILD,			  */	{	500,		500,		2000,		500 },
/* RLM_EXP_CONTEST_LAW,				  */	{	2000,		500,		500,		500 },
/* RLM_EXP_CONTEST_SOURCE,			  */	{	500,		500,		500,		2000 },
/* RLM_EXP_CONTEST_TEMPLE,			  */	{	500,		2000,		500,		500 },
/* RLM_EXP_CREATE_GUILD,			  */	{	500,		500,		2000,		500 },
/* RLM_EXP_CREATE_LAW,				  */	{	2000,		500,		500,		500 },
/* RLM_EXP_CREATE_SOURCE,			  */	{	500,		500,		500,		2000 },
/* RLM_EXP_CREATE_TEMPLE,			  */	{	500,		2000,		500,		500 },
/* RLM_EXP_DECLARE_WAR,				  */	{	1000,		500,		500,		500 },
/* RLM_EXP_DIPLOMACY_PERMISSIVE,	  */	{	2000,		2000,		2000,		2000 },
/* RLM_EXP_DIPLOMACY_FULL,			  */	{	4000,		4000,		4000,		4000 },
/* RLM_EXP_DIPLOMACY_VASSALAGE,	  */	{	10000,	10000,	10000,	10000 },
/* RLM_EXP_ESPIONAGE_SPY,			  */	{	500,		500,		1000,		500 },
/* RLM_EXP_ESPIONAGE_ASSASSINATE,  */	{	500,		500,		1000,		500 },
/* RLM_EXP_FORGE_LEYLINE,			  */	{	500,		500,		500,		2000 },
/* RLM_EXP_INVEST,					  */	{	5000,		5000,		5000,		5000 },
/* RLM_EXP_REALM_SPELL,				  */	{	100,		1000,		100,		1000 },
/* RLM_EXP_RULE_PROV,				  */	{	1000,		1000,		1000,		1000 },
/* RLM_EXP_RULE_GUILD,				  */	{	500,		500,		1000,		500 },
/* RLM_EXP_RULE_LAW,					  */	{	1000,		500,		500,		500 },
/* RLM_EXP_RULE_SOURCE,				  */	{	500,		500,		500,		1000 },
/* RLM_EXP_RULE_TEMPLE,				  */	{	500,		1000,		500,		500 },
/* RLM_EXP_RULE_PASS,				  */	{	0,			0,			0,			0 },
};


/* ========================================================================
   Function    - 
   Description - Vassal of another realm?
   Returns     -
   ======================================================================== */
BOOL CheckVassal ( REALM::REALM_TYPE rlm )
{
   REALM::REALM_TYPE i;

	for (i=REALM::FIRST_REALM; i < LAND_REALM_COUNT; i = (REALM::REALM_TYPE)(i + 1))
   {
       if ( ISVASSAL(rlm, i) ) 
       {
           return TRUE;
       }
   }

   return FALSE;
}



/* ========================================================================
   Function    - 
   Description - reconstruct the map
   Returns     -
   ======================================================================== */
void ReconstructMap (void)
{
	LONG	j;
	LONG	fLowMemory_ = fLowMemory;
	LONG	fRestrictAni_ = fRestrictAni;
	LONG	fLowResTextures_ = fLowResTextures;
	LONG	fLowResAni_ = fLowResAni;
	LONG	fMedResTextures_ = fMedResTextures;
	LONG	fMedResAni_ = fMedResAni;

	fLowMemory = FALSE;
	fRestrictAni = FALSE;
	fLowResTextures = FALSE;
	fLowResAni = FALSE;
	fMedResTextures = FALSE;
	fMedResAni = FALSE;

	if (iLgMap != fERROR)
	{
		DisposBlock(iLgMap);
		iLgMap = fERROR;
	}

	run_timers();  // cdb 11/27	// ABC don't put in inner loops

	//---- Always small map in multiplayer otherwise we go out of sync

#ifdef _WINDOWS
	if ( IsMultiPlayer() )
	{
		iLgMap = GetResourceStd("MAP\\SMMAP.PCX", FALSE);
		fLgMap = SMALL;
	}
	else
#endif
	{

		if (fSmallMap)
		{
			iLgMap = GetResourceStd("MAP\\SMMAP.PCX", FALSE);
			fLgMap = SMALL;
		}
		else
		{
			iLgMap = GetResourceStd("MAP\\LGMAP.PCX", FALSE);
			if (iLgMap == fERROR)
			{
				// Try for the smaller map.
				iLgMap = GetResourceStd("MAP\\SMMAP.PCX", FALSE);
				fLgMap = SMALL;
			}
			else
			{
				fLgMap = LARGE;
			}
		}
	}

	if (iProvMap == fERROR)
		iProvMap = GetResourceStd("MAP\\PROVMAP.PCX", FALSE);

	SetModifyResource(iLgMap);

	for (j=FIRST_PROVINCE; j<PROVINCE_COUNT; ++j)
		DrawRoads((PROVINCE)j);

	DrawBorders(0, 0, 0, 0, ((BITMPTR)BLKPTR(iLgMap))->w, ((BITMPTR)BLKPTR(iLgMap))->h, FULL_SCALE);

	fLowMemory = fLowMemory_;				// DON'T REMOVE THIS
	fRestrictAni = fRestrictAni_;			// DON'T REMOVE THIS
	fLowResTextures = fLowResTextures_;	// DON'T REMOVE THIS
	fLowResAni = fLowResAni_;				// DON'T REMOVE THIS
	fMedResTextures = fMedResTextures_;	// DON'T REMOVE THIS
	fMedResAni = fMedResAni_;				// DON'T REMOVE THIS
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
void NextPrev (LONG val, LONG);
BOOL DoPageProvince(LONG *startCount, LONG provCount, LONG countToPrint, LONG counter, LONG x, LONG y)
{
	BOOL rv = FALSE;

	if(*startCount < 1)
		*startCount = 1;

	if(provCount > countToPrint &&
		(((counter%countToPrint) == 0) || (counter == provCount)))
	{
		if(*startCount > 1)
		{
			DrawLittleButton(x+10, y+1, 50, 8, FALSE);
			add_region(x+10, y+1, 50, 8, D_KEY_PREV, NextPrev,-countToPrint,(LONG)startCount,0, -1);
			print_textf(x+10+25, y+6, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_PREV_PAGE));
			rv = TRUE;
		}

		if(*startCount <= (provCount - countToPrint))
		{
			DrawLittleButton(x+65, y+1, 50, 8, FALSE);
			add_region(x+65, y+1, 50, 8, D_KEY_NEXT, NextPrev,countToPrint,(LONG)startCount,0, -1);
			print_textf(x+65+25, y+6, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_NEXT_PAGE));
			rv = TRUE;
		}
	}
	return rv;
}

/* ========================================================================
   Function    - Functions to determine success
   Description - 
   Returns     -
   ======================================================================== */
#define CHECK_ONLY		 1
#define CHECK_AND_UPDATE 2
#define CHECK_QUITE  	 3

static BOOL ProcessGoldAndReg (LONG mode)
{
	LONG	j;

	// DM controlled realms get unlimited resources
	if (iCurAction != HOLD_ACTION && realm[HomeRealm].mfIsDMCtrl())
		return TRUE;

	checkGoldReg = 1;
	if (((GoldBar-GoldGained)*10) > realm[HomeRealm].mfGetTreasury())
	{
		if(mode != CHECK_QUITE)
			AddSndObj((BIRTHRT_SND)SND_UI_NOT_ENOUGH_GOLD,0,VOLUME_NINETY);
		return FALSE;
	}
	if (RegencePoint > realm[HomeRealm].mfGetRegency())
	{
		if(mode != CHECK_QUITE)
			AddSndObj((BIRTHRT_SND)SND_UI_NOT_ENOUGH_REGENCY,0,VOLUME_NINETY);
		return FALSE;
	}

	if (mode == CHECK_AND_UPDATE )
	{
		realm[HomeRealm].mfAddRegency(-RegencePoint);

//		if(((iCurAction == REALM_SPELL && iSelectedOneHolding == ITEM_SPELL_ALCHEMY) ||
//			 iCurAction == DIPLOMACY )
//			&& GoldGained > 0)

		// Handle the Diplomacy case upon success.
		if ((iCurAction == REALM_SPELL & iSelectedOneHolding == ITEM_SPELL_ALCHEMY)
			&& GoldGained > 0)
		{	
			realm[HomeRealm].mfAddTreasury(GoldGained * 10);
			GoldGained = 0;
		}

		realm[HomeRealm].mfAddTreasury(-(GoldBar * 10));

	   // YOU HAVE TO SEND THIS IN MULTIPLAYER
		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
			char temp[100];
			sprintf ( temp, "ProcessGoldAndRegency - HomeRealm:%s  regency:%04d",realm[HomeRealm].mfGetName(),realm[HomeRealm].mfGetRegency());
			RandomLogComment ( temp );
		}
		SetGameData(MP_REALM, MPRLM_TREASURY, HomeRealm, realm[HomeRealm].mfGetTreasury(), TRUE);
		SetGameData(MP_REALM, MPRLM_REGENCY, HomeRealm, realm[HomeRealm].mfGetRegency(), TRUE);
	}

	return TRUE;
}

/* ========================================================================
   Function    - Functions to determine success
   Description - 
   Returns     -
   ======================================================================== */
void CancelRoll (void)
{
	fAllowCancelButton = FALSE;
	fAllowRollButton = TRUE;
	fAllowDoneButton = FALSE;
	iWhichRoll = 99;
	lAskedTargetWhichRoll = 99;
}

/* ========================================================================
   Function    - Functions to determine success
   Description - 
   Returns     -
   ======================================================================== */
static UBYTE RollSuccess (UBYTE Chance, LONG x, LONG y, LONG lPrint)
{

	++iThisRoll;
	if (iThisRoll >= 128)
		fatal_error("ERROR - index of fRollResult exceeded 127 in ACTNMENU\n");

	if (fLTAction == DURING_LTACTION && iSelectedRealm > 0 && iSelectedRealm < REALM::REALM_COUNT)
	{
		CLASS_ENUM const class1 = regents[iSelectedRealm].mfGetClass1();
		CLASS_ENUM const class2 = regents[iSelectedRealm].mfGetClass2();
		UBYTE const expert = expertise[iCurAction];

		if (  expert!=class1
		   && expert!=class2
		   && expert!=9
		   && expert!=8
		   && (!(expert==10&&(class1==PRIEST||class1==WIZARD||class2==PRIEST||class2==WIZARD))))
		{
			Chance += 3;
		}

		if(expert == 8)
		{
			if(expertHolding[iSelectedOneHolding]!=class1
			   && expertHolding[iSelectedOneHolding]!=class2)
				Chance += 3;
		}
	}

	if (fRollResult[iThisRoll] == 1)
	{
		if(iCurAction==REALM_SPELL && iSelectedOneHolding == ITEM_SPELL_SCRY)
			return 3;

		if(lPrint)
		{
			print_textf(x+1, y+6, DKBROWN,STRMGR_GetStr(STR_ACTNMENU_F02_WON));
			print_textf(x, y+5, MDGREEN,  STRMGR_GetStr(STR_ACTNMENU_WON));
		}
		fAllowRollButton = FALSE;
		fAllowCancelButton = FALSE;
		if(iCurAction == ESPIONAGE)
			return 3;

		return 0;
	}

	if (fRollResult[iThisRoll] == 2)
	{
		if(iCurAction==REALM_SPELL && iSelectedOneHolding == ITEM_SPELL_SCRY)
			return 3;

		if(lPrint)
		{
			print_textf(x+1, y+6, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_F02_LOST));
			print_textf(x, y+5, RED,       STRMGR_GetStr(STR_ACTNMENU_LOST));
		}
		fAllowRollButton = FALSE;
		fAllowCancelButton = FALSE;
		return 0;
	}

	if (iThisRoll == iWhichRoll)				// time to do the roll
	{
		xRollLocation = x;
		yRollLocation = y;
		DispSuccess(); 
		success_roll = random(20)+1;
		if (success_roll >= Chance || fTutorialSelected)
		{
			fRollResult[iThisRoll] = 1;		// succeed
			if(iCurAction != ESPIONAGE || iSelectedOneHolding != 1)
			{
				if (iThisRoll == 0)
					AddSndObj((BIRTHRT_SND)SND_UI_DELAY_SUCCEEDED,0,VOLUME_NINETY);
				else	
					AddSndObj((BIRTHRT_SND)SND_UI_SUCCEEDED,0,VOLUME_NINETY);
			}
		}
		else
		{
			fRollResult[iThisRoll] = 2;		// fail
			if (iThisRoll == 0)
				AddSndObj((BIRTHRT_SND)SND_UI_DELAY_DID_NOT_SUCCEED,0,VOLUME_NINETY);
			else
				AddSndObj((BIRTHRT_SND)SND_UI_DID_NOT_SUCCEED,0,VOLUME_NINETY);
		}

		//printf("ROLL SUCCESS - Chance:%d  roll:%d  result:%d\n",Chance,success_roll,fRollResult[iThisRoll]);

		return fRollResult[iThisRoll];

	}

	return 0;
}

/*	======================================================================== */
static void DispSuccess (void)
{
	static LONG waittime = 0;

	// check for start conditions
	if (iWhichRoll>iThisRoll || xRollLocation==0 || yRollLocation==0)
	{
		lAskedTargetWhichRoll = 99;
		return;
	}

	// draw graphics each time
	DrawBitmap((SHORT)(xRollLocation+15-90), (SHORT)(yRollLocation+5-24), iDRollFIc, 0, 0, 999, 999);

	// do sound once
	//if(iWhichRoll==0)
	//	AddSndObj((BIRTHRT_SND)SND_UI_DICE1,0,VOLUME_NINETY);

	if(iWhichRoll==0 && firstRollFlag==0)
	{
		waittime = get_time()+27;
		AddSndObj((BIRTHRT_SND)SND_UI_DICE1,0,VOLUME_NINETY);
		firstRollFlag=1;
	}
	// wait to advance
	if (waittime>get_time())
	{ 
		lAskedTargetWhichRoll = 99;
		fAllowDoneButton = FALSE;
		return;
	}

	waittime = get_time()+18;

	++iWhichRoll;		// advance
	fAllowDoneButton = TRUE;
	lAskedTargetWhichRoll = iWhichRoll;
}

/* ========================================================================
   Function    - Various utility functions
   Description - 
   Returns     -
   ======================================================================== */
void SelectRealm (LONG realm_, LONG)
{

	if (mouse_button == 2)
 	{
		if (iCurAction == LIEUTANANT)
 			ShowStatus(-realm_, 0);
 		else if (iCurAction == LIEUTENANT_ACTION)
 			ShowStatus(regents[realm_].mfGetid(), 0);
 		else if (iCurAction == DIPLOMACY && realm[realm_].mfGetRegent() > 0)
 			ShowStatus(-regents[realm[realm_].mfGetRegent()].mfGetid(), 0);
 	}
 	else
 	{
		fAllowRollButton -= (iSelectedRealm!=0);
		// set new or toggle off
		iSelectedRealm = (iSelectedRealm == realm_) ? 0 : realm_;
		fAllowRollButton += (iSelectedRealm!=0);
		fAllowRollButton -= (iSelectedProvince!=0);

		// clear a bunch of stuff
		checkGoldReg = 0;
		iSelectedProvince = NO_PROVINCE;
		iSelectedOneHolding = 99;
		iSelectedUnit = 0;
		modRegency = modGoldBars = modLevel = 0;
 		for (LONG i=0; i<16; ++i)
 			iSelectedMulProvince[i] = 0;
	 	for (LONG i=0; i<20; ++i)
	 		iSelectedMulUnits[i] = 0;
		for (LONG i=0; i<9; ++i)
			iSelection[i] = 0;
		if (iCurAction==DIPLOMACY)
		{
			for (LONG i=0; i<PROVINCE_COUNT; ++i)
				iSelectedHolding[i][3] = 0;

			startProvCount = 1;
		}
		iNewScreen = 0;

	}
}
/*	======================================================================== */
void SelectProvince (LONG prov, LONG)
{
	LONG i, j;

	checkGoldReg = 0;
	fAllowRollButton -= (iSelectedProvince!=0);
	iSelectedProvince = (iSelectedProvince == prov) ? NO_PROVINCE : (PROVINCE)prov;
	fAllowRollButton += (iSelectedProvince!=0);
	modRegency = modGoldBars=modLevel = 0;

	if (iCurAction!=REALM_SPELL && iCurAction!=TRADE_ROUTE && iCurAction != FORGE_LEY_LINE)
	{
		
		for(i=0; i<PROVINCE_COUNT; ++i)
	   		for(j=0; j<4; ++j)
	   		{
				fAllowRollButton -= (iSelectedHolding[i][j]!=0);
				iSelectedHolding[i][j] = 0;
			}
	}
	iSelectedOneHolding = 99;		// 99 means no holding
	iSelectedUnit = 0;
 	for(i=0; i<16; ++i)
 		iSelectedMulProvince[i]=0;
 	for(i=0; i<20; ++i)
 		iSelectedMulUnits[i]=0;

}

/*	======================================================================== */
void Selection (LONG prov, LONG index)
{
	LONG i, j;
	checkGoldReg = 0;

	fAllowRollButton -= (iSelection[index]!=0);
	iSelection[index] = (iSelection[index] == prov) ? 0 : prov;
	fAllowRollButton += (iSelection[index]!=0);
	if(iSelection[index]!=0 && index != 5 && index != 8) // for Diplomacy choosing holding
		iNewScreen = 0;
	if(iSelection[index]==5 || iSelection[index]==8)
		iSelection[index-2]=0;
	if(iSelection[index]==3 || iSelection[index]==6)
		startProvCount = 1;

}


/* ======================================================================== */
void SelectMulProvince (LONG prov, LONG level)
{
  	LONG count=0, i, rate;
 	LONG baseLevel;

 	switch ((ITEMTYPE)iSelectedOneHolding)
 	{
 	 	case ITEM_SPELL_DEATH_PLAGUE:
		case ITEM_SPELL_WARDING:
 	 		baseLevel = 5;
			rate = 2;
 	 		break;
 	 	case ITEM_SPELL_DEMAGOGUE:
		case ITEM_SPELL_HONEST_DEALING:
 	 		baseLevel = 3;
			rate = 2;
 	 		break;
		case ITEM_SPELL_MASS_DESTRUCTION:
			baseLevel = 1;
			rate = 3;
			break;
		case ITEM_SPELL_BLESS_LAND:
		case ITEM_SPELL_BLIGHT:
			baseLevel = 1;
			rate = 2;
			break;
		case ITEM_SPELL_BLESS_ARMY:
			baseLevel = 1;
			rate = 1;
		default:
			baseLevel = 0;
			rate = 1;
			break;
 	}

  	if(iSelectedMulProvince[prov] == 1)
  	{
		checkGoldReg = 0;
  		fAllowRollButton --;
		additionalamt--;
  		iSelectedMulProvince[prov] = 0;
  	}

  	else
  	{
  		for (i = 0; i<16; ++i)
  		{
  			if ((ITEMTYPE)iSelectedOneHolding == ITEM_SPELL_DEMAGOGUE && i == 8)
				continue;
			if(iSelectedMulProvince[i]==1)
  				++count;

  		}
		additionalamt = count;
		
  		if (count < (level-baseLevel)/rate)
  		{
			checkGoldReg = 0;
  			
			iSelectedMulProvince[prov]=1;
  			++fAllowRollButton ;
			++additionalamt;
  		}
		
  	}
  	modRegency = modGoldBars = 0;

}

/*	======================================================================== */

/*	======================================================================== */
static void SelectHolding (LONG prov, LONG typ)
{
	// general SelectHolding code, used by RULE
	checkGoldReg = 0;
	fAllowRollButton -= (iSelectedHolding[prov][typ]!=0);
	iSelectedHolding[prov][typ] = !iSelectedHolding[prov][typ];
	fAllowRollButton += (iSelectedHolding[prov][typ]!=0);
	if (!iSelectedHolding[prov][typ])
		iNewScreen = 0;

	// specific code for DIPLOMACY only
	if (iCurAction == DIPLOMACY)
	{
		if (prov < 9)
			iSelection[prov] = 0;
		if (prov == 5 || prov == 8)
			iSelection[prov-2] = 0;
		startProvCount = 1;
	}

	modRegency = modGoldBars = 0;
}

/*	======================================================================== */
void SelectOneHolding (LONG typ, LONG)
{
	checkGoldReg = 0;
	fAllowRollButton -= (iSelectedOneHolding!=99);
	iSelectedOneHolding = (iSelectedOneHolding==typ)?99:typ;
	fAllowRollButton += (iSelectedOneHolding!=99);
	iSelectedUnit = 0;
 	for(LONG i=0; i<16; ++i)
 		iSelectedMulProvince[i]=0;
 	for(LONG i=0; i<20; ++i)
 		iSelectedMulUnits[i]=0;

	modRegency = modGoldBars = modLevel =0;
}
/*	======================================================================== */
void SelectUnit (LONG typ, LONG)
{
	checkGoldReg = 0;
	fAllowRollButton -= (iSelectedUnit!=0);
	iSelectedUnit = (iSelectedUnit==typ)?0:typ;
	fAllowRollButton += (iSelectedUnit!=0);
 	for(LONG i=0; i<16; ++i)
 		iSelectedMulProvince[i]=0;

	modRegency = modGoldBars = modLevel =0;
}
/*	======================================================================== */
void SelectMulUnits (LONG unitid, LONG index)
{
	LONG count=0, i;
  	if(iSelectedMulUnits[index] != 0)
  	{
		checkGoldReg = 0;
  		fAllowRollButton --;
  		iSelectedMulUnits[index] = 0;
  	}
  	else 
  	{
  		for(i = 0; i<20; ++i)
  		{
			if(iSelectedMulUnits[i]!= 0)
  				++count;
  		} 
  		if(count < modLevel-1)   
  		{
			checkGoldReg = 0;
  			iSelectedMulUnits[index]=unitid;
  			++fAllowRollButton ;
  		}
  	}
	count = 0;
  	for(i = 0; i<20; ++i)
  	{
		if(iSelectedMulUnits[i]!= 0)
  			++count;
  	}
	if(count == 0)
		iSelectedUnit = 0; 
  	modRegency = modGoldBars = 0;

}

/*	======================================================================== */
void IncNum (LONG lpNum, LONG)
{
	checkGoldReg = 0;
	UBYTE * pNum = (UBYTE*)lpNum;
	*pNum = *pNum + 1;
}
/*	======================================================================== */
void DecNum (LONG lpNum, LONG)
{
	checkGoldReg = 0;
	UBYTE * pNum = (UBYTE*)lpNum;
	*pNum = *pNum - 1;
}
/*	======================================================================== */
void IncNumWithSign (LONG, LONG)
{
	checkGoldReg = 0;
	modLevel += 1;
}
/*	======================================================================== */
void DecNumWithSign (LONG, LONG)
{
	checkGoldReg = 0;
	modLevel -= 1;
}
/*	======================================================================== */
void SetNum (LONG lpNum, LONG i)
{
	UBYTE * pNum = (UBYTE*)lpNum;

	checkGoldReg = 0;
	fAllowRollButton -= (*pNum!=0);
	*pNum = (*pNum == i) ? 0 : i;
	fAllowRollButton += (*pNum!=0);
}
/*	======================================================================== */
void NextPrev (LONG val, LONG lpStart)
{
	LONG *pStart = (LONG *)lpStart;
	*pStart += val;

	// make sure screen updates
	SetRedrawMainMapLevel();
}
/*	======================================================================== */
void SetupMod (LONG x, LONG y, UBYTE * pNum, LONG NumMin, LONG Base, LONG dirFactor)
{
	SHORT		v;
	SHORT		NumMax = 99;

	if (dirFactor == -1)			// spend gold of reg to mod success
	{
		NumMax = Base - 1;
		v = Base - (LONG)(*pNum);
	}
	if (dirFactor == 1)			// set level of something
	{
		NumMax = Base;
		v = (LONG)(*pNum);
	}
	if (dirFactor == 2)			// set level of something
	{
		v = (LONG)(*pNum) + NumMin;
		NumMax = Base - NumMin;
		NumMin = 0;
	}

	if (dirFactor==-1)
	{
		print_textf(x+2, y+6, DKBROWN, "^F21^c%d", D20TOPERCENT(v));
		x += 11;
	}
	else if (dirFactor)					// if zero, don't print
	{
		print_textf(x+3, y+6, DKBROWN, "^F02^c%d", v);
		x += 11;
	}

	init_gfont(FONT_SANS_5PT);

	// for plus
	if ((LONG)(*pNum) < NumMax)
	{
		DrawLittleButton(x, y-1, 10, 4, FALSE);
		add_region(x-6, y-3, 18, 8, 0, IncNum, (LONG)pNum, 0, 0, -1);
	}
	else
		color_rect(x-1, y-2, 10, 4, LTTAN-4);
	print_textf(x+3, y-2, DKBROWN, "+");

	// for minus
	if ((LONG)(*pNum) > NumMin)
	{
		DrawLittleButton(x, y+6, 10, 4, FALSE);
		add_region(x-6, y+4, 18, 8, 0, DecNum, (LONG)pNum, 0, 0, -1);
	}
	else
		color_rect(x-1, y+6, 10, 4, LTTAN-4);
	print_textf(x+4, y+5, DKBROWN, "-");
}
/* --------------------------------------------- */
#if 0
BOOL find_path(LONG cur_prov, LONG to_prov, LONG * countPt)
{
	LONG	i, dx, dy, dist;
	LONG	test_prov;
	LONG	best_prov = 0;
	LONG	best_border = 0;
	LONG	min_dist = 99999999;
	LONG	count;
	count = *countPt;
	for (i=0; i<7; ++i)
	{
		test_prov = province[cur_prov].Border[i].Province;
		dx = province[to_prov].x - province[test_prov].x;
		dy = province[to_prov].y - province[test_prov].y;
		dist = (dx * dx) + (dy * dy);
		if (dist < min_dist && TerrainCost[province[cur_prov].Border[i].MoveCost] < 100)
		{
			min_dist = dist;
			best_prov = test_prov;
			best_border = i;
		}
	}

	if (best_prov)		// found a good province
	{
		++count;			// make sure count == 0 when call this function
		if (best_prov != to_prov)
			find_path(best_prov, to_prov, &count);
		*countPt = count;
		return TRUE;
	}

	return FALSE;
}
#endif

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
// terminate the list here
void TerminatePlaceList (LONG indexPlaces)
{
	SetGameData(MP_PLACES, MPPLACE_NEXTPLACE, indexPlaces, 0, TRUE );
}

/*	======================================================================== */
BOOL fBeenHere[MAX_PLACES];

void CheckAndFixPlaces (PROVINCE prov)
{
	LONG	i, j, k;

	// clear loop detector
	memset(&fBeenHere[0], FALSE, sizeof(BOOL)*MAX_PLACES);

	// if FirstPlace is bad...
	if (province[prov].FirstPlace > MAX_PLACES)
	{
		// ...terminate the list here
		SetGameData(MP_PROVINCE, MPPROV_FIRSTPLACE, prov, 0, TRUE);
//		fatal_error("ACTNMENU::CheckAndFixPlaces - province[%s (%ld)].FirstPlace > MAX_PLACES (%ld)",province[prov].name,prov,province[prov].FirstPlace);
	}

	i = j = province[prov].FirstPlace;
	while (j != 0)
	{
		// save we've been here
		fBeenHere[i] = TRUE;

		if (places[i].NextPlace == 0)		// stop when we reach the end
			return;

		// get potential next place in the list
		k = province[prov].FirstPlace + places[i].NextPlace;

		// if NextPlace is bad...
		if ( k > MAX_PLACES )
		{
			TerminatePlaceList(i);	// ...terminate the list here
//			fatal_error("ACTNMENU::CheckAndFixPlaces - places[%ld].NextPlace bad, link (%ld) > MAX_PLACES",i,k);
		}

		// if NextPlace causes loop...
		if ( fBeenHere[k] )
		{
			TerminatePlaceList(i);	// ...terminate the list here
//			fatal_error("ACTNMENU::CheckAndFixPlaces - places[%ld].NextPlace loops back to %ld",i,k);
		}

		j = places[i].NextPlace;		// next place in province
		i = province[prov].FirstPlace + j;
	}

}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
void CheckAndFixSourceLevels (PROVINCE iProv)
{
	LONG	i, j, maxl, maxi, suml;

	CheckAndFixPlaces(iProv);

RetestSourceLevels:
	maxl=0; maxi=-1; suml=0;
	i = j = province[iProv].FirstPlace;
	while (j != 0)
	{
		if (places[i].Icon == SOURCE1_ICON)
		{
			if (places[i].Level > maxl)
				maxi = i;
			suml += places[i].Level;
		}
		j = places[i].NextPlace;
		i = province[iProv].FirstPlace + j;
	}

	if (suml > province[iProv].MagLevel && maxi > 0)
	{
		--places[maxi].Level;
		SetGameData(MP_PLACES, MPPLACE_LEVEL, maxi, places[maxi].Level, TRUE );
		SetGameData(MP_PLACES, MPPLACE_IICONBITM, maxi, places[maxi].Icon+(((places[maxi].Level<8)?places[maxi].Level:7)/2), TRUE );
		goto RetestSourceLevels;
	}

}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
void DeleteHolding (PROVINCE iProv, LONG iHold, BOOL fSend )
{
	LONG	i, j, k, l;

	CheckAndFixPlaces(iProv);

	// break ley lines if a SOURCE holding
	if (places[iHold].Icon == SOURCE1_ICON)
	{
		for (i=1; i<MAX_PLACES; ++i)
		{
			if (places[i].Icon == SOURCE1_ICON)
			{
				if (places[i].iRoute1 == iHold)
				{
					//places[i].iRoute1 = 0;
					SetGameData(MP_PLACES, MPPLACE_IROUTE1, i, 0, fSend );
				}
				else if (places[i].iRoute2 == iHold)
				{
					//places[i].iRoute2 = 0;
					SetGameData(MP_PLACES, MPPLACE_IROUTE2, i, 0, fSend );
				}
				else if (places[i].iRoute3 == iHold)
				{
					//places[i].iRoute3 = 0;
					SetGameData(MP_PLACES, MPPLACE_IROUTE3, i, 0, fSend );
				}
			}
		}
	}

	// Remove the place from the links in the places array

	// if Holding is the first place in a province, move FirstPlace
	if (province[iProv].FirstPlace == iHold)
	{
		i = places[iHold].NextPlace;				// offset we will be subtracting from everything
		j = k = iHold; 								// same as province[iProv].FirstPlace
		while (k != 0)
		{
			k = places[j].NextPlace;				// save current value, we are about to change it
			if (k != 0)
			{
				places[j].NextPlace -= i;
				SetGameData(MP_PLACES, MPPLACE_NEXTPLACE, j, places[j].NextPlace, fSend );
			}
			j = province[iProv].FirstPlace + k;
		}
		// GWP Looks like if this is the last place in your holding we need to set the
		// GWP province[].FirstPlace should be set to zero.
		if (i == 0)
		{
			province[iProv].FirstPlace = 0;
		}
		else
		{
			province[iProv].FirstPlace += i;
		}
		SetGameData(MP_PROVINCE, MPPROV_FIRSTPLACE, iProv, province[iProv].FirstPlace, fSend );
	}

	// Holding is not the first place
	else
	{
		// find previous holding
		l = i = province[iProv].FirstPlace;
		while (l != 0 && i != iHold)
		{
			j = i;								// j's .NextPlace refers to iHold

			l = places[i].NextPlace;		// next place in province
			i = province[iProv].FirstPlace + l;
		}

		if ((province[iProv].FirstPlace+places[j].NextPlace) != iHold)
			return;				// places list screwed, didn't find iHold

		places[j].NextPlace = places[iHold].NextPlace;
		SetGameData(MP_PLACES, MPPLACE_NEXTPLACE, j, places[j].NextPlace, fSend );
	}

//	places[iHold].Realm = REALM::NO_COUNTRY;
	SetGameData(MP_PLACES, MPPLACE_REALM, iHold, REALM::NO_COUNTRY, FALSE );

//	places[iHold].NextPlace = 0;
	SetGameData(MP_PLACES, MPPLACE_NEXTPLACE, iHold, 0, FALSE );
//	places[iHold].Contested	= 0;
	SetGameData(MP_PLACES, MPPLACE_CONTESTED, iHold, 0, FALSE );
//	places[iHold].iRoute1 = 0;
	SetGameData(MP_PLACES, MPPLACE_IROUTE1, iHold, 0, FALSE );
//	places[iHold].iRoute2 = 0;
	SetGameData(MP_PLACES, MPPLACE_IROUTE2, iHold, 0, FALSE );
//	places[iHold].iRoute3 = 0;
	SetGameData(MP_PLACES, MPPLACE_IROUTE3, iHold, 0, FALSE );

	//GWP
	#ifdef _WINDOWS
	if (fSend && IsMultiPlayer())
		AMultiPlayer.PlacesMessage(iHold, (UBYTE *)&places[iHold]);
	#endif
	
	cntHolds--;
}
/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
BOOL CreateHolding (LONG type, LONG route1, LONG route2, PROVINCE prov, REALM::REALM_TYPE realm, BOOL fSend )
{
	LONG	i, j, k, tmp, x;
	LONG	prevPlace = fERROR;
	BOOL	f[10] = {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE};

	CheckAndFixPlaces(prov);

	// find an open site in the province for the new place
	j = k = province[prov].FirstPlace;
	while (k != 0)
	{
		for (i=0; i<10; ++i)
		{
			if (places[j].x == (SHORT)(province[prov].x+place_x[i])
					&& places[j].y == (SHORT)(province[prov].y+place_y[i]-10) )
			{
				f[i] = FALSE;
				//	printf("Create holding - found x:%d y:%d don't use %d\n",places[j].x,places[j].y,i);
			}
		}
		prevPlace = j;
		k = places[j].NextPlace;
		j = province[prov].FirstPlace + k;
	}

	// find the place to put the new holding(the end of places array)
	//	for (tmp=0; places[tmp].Realm != REALM::NO_COUNTRY && tmp<MAX_PLACES; ++tmp){}
	RequestPlacesIndex(&tmp, fSend);

	if (tmp==MAX_PLACES)
	{
		//	fatal_error("ERROR - exceeded MAX_PLACES while creating a holding in ACTNMENU\n");
		return FALSE;
	}

	// put the new holding into link
	if (prevPlace != fERROR)
	{
		//	places[prevPlace].NextPlace = tmp - province[prov].FirstPlace;
		SetGameData(MP_PLACES, MPPLACE_NEXTPLACE, prevPlace, tmp - province[prov].FirstPlace, fSend );
	}
	else
	{
		// province[prov].FirstPlace = tmp;
		SetGameData(MP_PROVINCE, MPPROV_FIRSTPLACE, prov, tmp, fSend );
	}
	//	places[tmp].NextPlace = 0;
	SetGameData(MP_PLACES, MPPLACE_NEXTPLACE, tmp, 0, FALSE );

	// initialize the new holding
	for (i=0; i<10; ++i)
	{
		if (f[i])
		{
			x = province[prov].x + place_x[i];
//			places[tmp].y = (SHORT)(province[prov].y + place_y[i] - 10);
			SetGameData(MP_PLACES, MPPLACE_Y, tmp, (SHORT)(province[prov].y + place_y[i] - 10), FALSE );
//			printf("Create holding at x:%d y:%d\n",places[tmp].x,places[tmp].y);
			break;
		}
	}
//	places[tmp].Realm = realm;
	SetGameData(MP_PLACES, MPPLACE_REALM, tmp, realm, FALSE );
//	places[tmp].Level = 0;
	SetGameData(MP_PLACES, MPPLACE_LEVEL, tmp, 0, FALSE );
//	places[tmp].iRoute1 = route1;
	SetGameData(MP_PLACES, MPPLACE_IROUTE1, tmp, route1, FALSE );
//	places[tmp].iRoute2 = route2;
	SetGameData(MP_PLACES, MPPLACE_IROUTE2, tmp, route2, FALSE );
//	places[tmp].iRoute3 = 0;
	SetGameData(MP_PLACES, MPPLACE_IROUTE3, tmp, 0, FALSE );
//	places[tmp].iName = 0;
	SetGameData(MP_PLACES, MPPLACE_INAME, tmp, 0, FALSE );
//	places[tmp].Contested	= 0;
	SetGameData(MP_PLACES, MPPLACE_CONTESTED, tmp, 0, FALSE );
//	places[tmp].scene = NULL;
	SetGameData(MP_PLACES, MPPLACE_SCENE, tmp, NULL, FALSE );
//	places[tmp].Icon = (SHORT)InfoReqType[type];
	SetGameData(MP_PLACES, MPPLACE_ICON, tmp, (SHORT)InfoReqType[type], FALSE );
//	sprintf(n,"UI\\%s.PCX",GameIcons[places[tmp].Icon]);
//	places[tmp].iIconBitm = GetResourceStd(n, FALSE);
	SetGameData(MP_PLACES, MPPLACE_IICONBITM, tmp, places[tmp].Icon, FALSE );

// setting the x coord tells the system the holding exists
//	places[tmp].x = (SHORT)x;
	SetGameData(MP_PLACES, MPPLACE_X, tmp, (SHORT)x, FALSE );

	//GEH
	#ifdef _WINDOWS
	if (fSend && IsMultiPlayer())
		AMultiPlayer.PlacesMessage(tmp, (UBYTE *)&places[tmp]);
	#endif
	
	// GWP szHoldType overflows its bounds
	// WRC szHoldType was changed to gsHoldType, it has string enums instead of strings now
	// GWP printf("Create holding %d - link:%d prov:%s type:%s iRoute1:%d iRoute2:%d\n", tmp, prevPlace, province[prov].name, STRMGR_GetStr(gsHoldType[type]), places[tmp].iRoute1, places[tmp].iRoute2);

	CheckAndFixPlaces(prov);

	++cntHolds;
	return TRUE;
}


/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
static void SyncRegencySpentInOpposition (REALM::REALM_TYPE TargetRealm)
{
	if (HomeRealm <= 0 || HomeRealm >= REALM::REALM_COUNT
			|| TargetRealm <= 0 || TargetRealm >= REALM::REALM_COUNT)
	{
		#ifdef _DEBUG
		fatal_error("SyncRegencySpentInOpposition - HomeRealm (%ld) or TargetRealm (%ld) out of bounds (%d)", HomeRealm, TargetRealm, REALM::REALM_COUNT);
		#endif
		return;
	}
	
	if (HomeRealm != TargetRealm)
	{
		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
			char temp[100];
			sprintf ( temp, "SyncRegencySpentInOpposition - HomeRealm:%s  regency:%04d",realm[HomeRealm].mfGetName(),realm[HomeRealm].mfGetRegency());
			RandomLogComment ( temp );
		}
		SetGameData(MP_REALM, MPRLM_REGENCY, HomeRealm, realm[HomeRealm].mfGetRegency(), TRUE);

		// player's regency is xmited in  RollOthersAction
		if (!realm[TargetRealm].mfIsPlayerCtrl())
		{
			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
				char temp[100];
				sprintf ( temp, "SyncRegencySpentInOpposition - TargetRealm:%s  regency:%04d",realm[TargetRealm].mfGetName(),realm[TargetRealm].mfGetRegency());
				RandomLogComment ( temp );
			}
			SetGameData(MP_REALM, MPRLM_REGENCY, TargetRealm, realm[TargetRealm].mfGetRegency(), TRUE);
		}
	}
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
static LONG			MaxSourceLevel = 0;
static LONG			MaxPlace;

static LONG LargestSource (PROVINCE prov, LONG place, REALM::REALM_TYPE ThisRealm)
{
	LONG			j, k, i;
	LONG			route, route2;

	//printf("*");

	// find a SOURCE we own in prov
	if (prov)
	{
		for (i=0; i<MAX_PLACES; ++i)
			places[i].Besieged = 0;

		MaxSourceLevel = 0;

		CheckAndFixPlaces(prov);
		place = j = province[prov].FirstPlace;
		while (j != 0)
		{
			if (places[place].Icon==SOURCE1_ICON && places[place].Realm==ThisRealm)
				goto FollowLeyLine;

			j = places[place].NextPlace;		// next place in province
			place = province[prov].FirstPlace + j;
		}
		return 0;		// MaxSourceLevel = 0
	}

FollowLeyLine:
	places[place].Besieged = 1;

	if (places[place].Level > MaxSourceLevel)
	{
		MaxSourceLevel = places[place].Level;
		MaxPlace = place;
	}

	for (k=0; k<3; ++k)		// scan through ley lines
	{
		route = (k==0)?places[place].iRoute1:(k==1)?places[place].iRoute2:places[place].iRoute3;
		if (route && places[route].Besieged==0 && places[route].Realm==ThisRealm)
			LargestSource(NO_PROVINCE, route, HomeRealm);
	}

	for (i=0; i<MAX_PLACES; ++i)
		if (places[i].Icon==SOURCE1_ICON
				&& places[i].Besieged == 0
				&& places[i].Realm==ThisRealm
				&& (places[i].iRoute1==place || places[i].iRoute2==place || places[i].iRoute3==place)
			)
			LargestSource(NO_PROVINCE, i, HomeRealm);

	return MaxSourceLevel;
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
#define xPROVWIDTH			(135)
#define xSECONDPROV			(145)

void PaintForgeLeyLine (LONG x, LONG y, LONG h)
{
	BOOL		f = FALSE;
	LONG		i;
	LONG		j, k;
	LONG		startY;
	LONG		tmpIndex;
	LONG		route;
	LONG		success;
	LONG		result;
	LONG		place, place2;
	LONG		provCount = 0;
	LONG		counter = 0;

	GoldBar = RegencePoint = 0;
	for (i=0; i<PROVINCE_COUNT; ++i)
		iSelectedHolding[i][2] = 0;

	fAllowCancelButton = !fAllowDoneButton;
	print_textf(x, y, DKBROWN, "^F00%s", 		 STRMGR_GetStr(STR_ACTNMENU_START_PROVINCE));
	print_textf(x+xSECONDPROV, y, DKBROWN, "^F00%s", STRMGR_GetStr(STR_ACTNMENU_END_PROVINCE));
	y += 12;
	startY = y;

	del_region(NextPrev,0);

	// gather info about provinces
	for (i=FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)			// scan all provinces
	{
		CheckAndFixPlaces((PROVINCE)i);
		tmpIndex = j = province[i].FirstPlace;
		while (j != 0)							// scan for a SOURCE we own
		{
			if (places[tmpIndex].Icon == SOURCE1_ICON)
			{
				for (k=0; k<3; ++k)		// scan through ley lines
				{
					route = (k==0)?places[tmpIndex].iRoute1:(k==1)?places[tmpIndex].iRoute2:places[tmpIndex].iRoute3;
					if (route && route < PROVINCE_COUNT)
					{
						++iSelectedHolding[i][2];			// mark both ends of a route
						++iSelectedHolding[route][2];
					}
				}
				if (places[tmpIndex].Realm == HomeRealm)	// mark a SOURCE of ours
				{
					iSelectedHolding[i][0] = tmpIndex;
					iSelectedHolding[i][1] = tmpIndex/256;
//printf("Province Info:%s  holding:%d  Icon%s  Realm:%s  NextPlace:%d\n",province[i].name,tmpIndex,GameIcons[places[tmpIndex].Icon],realm[places[tmpIndex].Realm].name ,places[tmpIndex].NextPlace);
				}
			}
			j = places[tmpIndex].NextPlace;		// next place in province
			tmpIndex = province[i].FirstPlace + j;
		}
	}

	// add buttons for provinces
	provCount = 0;
	counter = 0;
	for (int h1=0; h1<2; ++h1)
	{
		for (i=FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)			// scan all provinces
		{
			// if we can build a ley line from this prov
			j = iSelectedHolding[i][0] + (iSelectedHolding[i][1]*256);
			if (j > 0)
			{
				if (h1)
				{
					++counter;
					if(counter < startProvCount)
						continue;

					init_gfont(FONT_SANS_8PT);
					DrawLittleButton(x, y, xPROVWIDTH, 10, (iSelectedRealm==i));
					add_region(x, y, xPROVWIDTH, 10, 0, SelectRealm,i,0,0, -1);
					DrawShield(x,y,(REALM::REALM_TYPE)province[i].Realm);
					gprint_text(x+14, y, province[i].name, LoyaltyColor[province[i].Loyalty]);
					y+=16;

					// print prev, next buttons if provinces are more that 13		
					if(DoPageProvince(&startProvCount, provCount, 13, counter, x, y))
						break;
				}
				else
					++provCount;
			}
		}
	}
	ClearRemapTable();

	if (startY == y)
	   gprint_text(x, y, STRMGR_GetStr(STR_ACTNMENU_CREATE_LEY_LINE_HELP), DKBROWN);

	// add buttons for second province
	y = startY;
	if (iSelectedRealm)
	{
		place  = iSelectedHolding[iSelectedRealm][0] + (iSelectedHolding[iSelectedRealm][1]*256);
		provCount = 0;
		counter = 0;
		for (int h1=0; h1<2; ++h1)
		{
			for (i=FIRST_PROVINCE; i<PROVINCE_COUNT && y<(startY+h-70); ++i)			// scan all provinces
			{
				place2 = iSelectedHolding[i][0] + (iSelectedHolding[i][1]*256);
				if (i != iSelectedRealm && place2
					&& places[place].iRoute1 != place2 && places[place2].iRoute1 != place
					&& places[place].iRoute2 != place2 && places[place2].iRoute2 != place
					&& places[place].iRoute3 != place2 && places[place2].iRoute3 != place
					&& (places[place].iRoute1==0||places[place].iRoute2==0||places[place].iRoute3==0) )
				{
					if (h1)
					{
						++counter;
						if(counter < startProvCount1)
							continue;

						DrawLittleButton(x+xSECONDPROV, y, xPROVWIDTH, 10, (iSelectedProvince==i));
						add_region(x+xSECONDPROV, y, xPROVWIDTH, 10, 0, SelectProvince,i,0,0, -1);
						DrawShield(x+xSECONDPROV, y, (REALM::REALM_TYPE)province[i].Realm);
						init_gfont(FONT_SANS_8PT);
						gprint_text(x+xSECONDPROV+14, y, province[i].name, StatusColor[(RealmStatus[HomeRealm][province[i].Realm])&0x0F]);
						y+=16;

						// print prev, next buttons if provinces are more than 13		
						if(DoPageProvince(&startProvCount1, provCount, 13, counter, x+xSECONDPROV, y))
							break;
					}
					else
						++provCount;
				}
			}
		}
		ClearRemapTable();
	}

	// print success and allow modification
	y = startY+h-84;
	if(!iSelectedProvince || !iSelectedRealm)
		fAllowRollButton = FALSE;

	if (iSelectedRealm && iSelectedProvince)
	{
		fAllowRollButton = TRUE;
		place  = iSelectedHolding[iSelectedRealm][0] + (iSelectedHolding[iSelectedRealm][1]*256);
		place2  = iSelectedHolding[iSelectedProvince][0] + (iSelectedHolding[iSelectedProvince][1]*256);
//printf("LEYLINE - start:%s place:%d (%d routes) end:%s place:%d (%d routes)\n",
//	province[iSelectedRealm].name,place,iSelectedHolding[iSelectedRealm][2],
//	province[iSelectedProvince].name,place2,iSelectedHolding[iSelectedProvince][2]);

		success = 5 - places[place].Level - modRegency + modOpponent;
		if (success < 1) success = 1;

		print_textf(x, y, DKBROWN,STRMGR_GetStr(STR_ACTNMENU_F02_SUCCESS));
		print_textf(x+66, y+6, DKBROWN, "^F02^c%d%c", D20TOPERCENT(success),PERCENTCHAR);
		if (fLTAction != DURING_LTACTION)
		{
			print_textf(x+90, y-2, DKBROWN, STRMGR_GetStr(STR_MAPAI_MODIFY_SUCCESS));
			print_textf(x+90, y+4, DKBROWN, STRMGR_GetStr(STR_MAPAI_USING_REGENCY));
			SetupMod(x+165, y, &modRegency, 0, (4-places[place].Level+modOpponent), 1);
		}

		// multi-player and AI support
		if(lAskedTargetWhichRoll == iThisRoll + 1)
		{
			success = AskAndWaitForTargetPlayer(FORGE_LEY_LINE, iSelectedRealm, 0, success, x+63+7, y);
			success = AskAndWaitForTargetPlayer(FORGE_LEY_LINE, iSelectedProvince, 0, success, x+63+7, y);
		}

		// check for success roll
		result = RollSuccess(success, x+63+7, y, TRUE);
		if (result == 1)	// success
		{
			// give experience points
			ActionExpPoints(ActiveRegent, RLM_EXP_FORGE_LEYLINE, 0 , TRUE);
			// multi-player support
			SendResult(FORGE_LEY_LINE, iSelectedRealm, 0, result);
			SendResult(FORGE_LEY_LINE, (LONG)iSelectedProvince, 0, result);
			SyncRegencySpentInOpposition((REALM::REALM_TYPE)province[iSelectedRealm].Realm);
			SyncRegencySpentInOpposition((REALM::REALM_TYPE)province[iSelectedProvince].Realm);
			if (places[place].iRoute1==0)
//				places[place].iRoute1 = place2;
				SetGameData(MP_PLACES, MPPLACE_IROUTE1, place, place2, TRUE );
			else if (places[place].iRoute2==0)
//				places[place].iRoute2 = place2;
				SetGameData(MP_PLACES, MPPLACE_IROUTE2, place, place2, TRUE );
			else
//				places[place].iRoute3 = place2;
				SetGameData(MP_PLACES, MPPLACE_IROUTE3, place, place2, TRUE );

			AddReport(SUBJ_LEY_LINE, iSelectedRealm, iSelectedProvince, 0);

//printf("WON LEYL- start:%s place:%d route1:%d end:%s place:%d route1:%d\n",
//	province[iSelectedRealm].name,place,places[place].iRoute1,
//	province[iSelectedProvince].name,place2,places[place2].iRoute1);

		}
		if (result == 2)	// failure
		{
			// multi-player support
			SendResult(FORGE_LEY_LINE, (LONG)iSelectedRealm, 0, result);
			SendResult(FORGE_LEY_LINE, (LONG)iSelectedProvince, 0, result);
			SyncRegencySpentInOpposition((REALM::REALM_TYPE)province[iSelectedRealm].Realm);
			SyncRegencySpentInOpposition((REALM::REALM_TYPE)province[iSelectedProvince].Realm);
			AddReport(SUBJ_LEYLINE_FAIL, iSelectedRealm, iSelectedProvince, 0);
		}

		k = ProvDist(iSelectedRealm, iSelectedProvince, 0, TRUE);
		GoldBar = k + 1;
		RegencePoint = modRegency + k + 1;
		if(checkGoldReg)
			fAllowRollButton = (ProcessGoldAndReg(CHECK_QUITE))?fAllowRollButton:0;
		else
			fAllowRollButton = (ProcessGoldAndReg(CHECK_ONLY))?fAllowRollButton:0;
	}

	y = startY+h-69;
	print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_TOTAL_COST_FMT), GoldBar, RegencePoint);
	print_textf(x, y+15, DKBROWN,STRMGR_GetStr(STR_ACTNMENU_SELECT_START_AND_END) );

	if (fAllowDoneButton)
		DeleteActnMenuRegions();

}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
void PaintBuildRoad (LONG x, LONG y, LONG h)
{
	BOOL		f = FALSE;
	SHORT		i, j;
	SHORT		startY;
	LONG		provCount = 0;
	LONG		counter = 0;

	fAllowRollButton = FALSE;

	print_textf(x, y, DKBROWN, "^F00%s", 		STRMGR_GetStr(STR_ACTNMENU_START_PROVINCE));
	print_textf(x+xSECONDPROV, y, DKBROWN, "^F00%s",STRMGR_GetStr(STR_ACTNMENU_END_PROVINCE));
	y += 12;
	startY = y;

	del_region(NextPrev,0);

	// add buttons for provinces
	provCount = 0;
	counter = 0;
	for (int h1=0; h1<2; ++h1)
	{
		for (i=FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)			// scan all provinces
		{
			if (province[i].Realm == HomeRealm)
			{
				if (h1)
				{
					++counter;
					if(counter < startProvCount)
						continue;

					init_gfont(FONT_SANS_8PT);
					DrawLittleButton(x, y, xPROVWIDTH, 10, (iSelectedRealm==i));
					add_region(x, y, xPROVWIDTH, 10, 0, SelectRealm,i,0,0, -1);
					DrawShield(x, y, (REALM::REALM_TYPE)province[i].Realm);
					gprint_text(x+14, y, province[i].name, LoyaltyColor[province[i].Loyalty]);
					y+=16;

					// print prev, next buttons if provinces are more than 13		
					if(DoPageProvince(&startProvCount, provCount, 13, counter, x, y))
						break;
				}
				else
					++provCount;
			}
		}
	}
	ClearRemapTable();

	// add buttons for second province
	y = startY;
	if (iSelectedRealm)
	{
		provCount = 0;
		counter = 0;
		for (int h1=0; h1<2; ++h1)
		{
			for (i=FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)			// scan all provinces
			{
				f = FALSE;
				for (j=0; j<7; ++j)		// scan through borders
					if (province[i].Border[j].Province == iSelectedRealm
						&& province[i].Border[j].MoveCost != PLAINS_ROAD
						&& province[i].Border[j].MoveCost != RIVER_ROAD
						&& province[i].Border[j].MoveCost != HVY_MNT)
						f = TRUE;
				if (AREALLIED(province[i].Realm, HomeRealm) && f)
				{
					if (h1)
					{
						++counter;
						if(counter < startProvCount1)
							continue;
	
						DrawLittleButton(x+xSECONDPROV, y, xPROVWIDTH+14, 10, (iSelectedProvince==i));
						add_region(x+xSECONDPROV, y, xPROVWIDTH+14, 10, 0, SelectProvince,i,0,0, -1);
						DrawShield(x+xSECONDPROV+2, y, (REALM::REALM_TYPE)province[i].Realm);
						init_gfont(FONT_SANS_8PT);
						if (province[i].Realm == HomeRealm)
						   gprint_text(x+xSECONDPROV+16, y, province[i].name, StatusColor[(RealmStatus[HomeRealm][province[i].Realm])&0x0F]);
						else
						   gprint_text(x+xSECONDPROV+16, y, province[i].name, DKBROWN);
						y+=16;

						// print prev, next buttons if provinces are more than 12		
						if(DoPageProvince(&startProvCount1, provCount, 12, counter, x+xSECONDPROV, y))
							break;
					}
					else
						++provCount;
				}
			}
		}
		ClearRemapTable();
	}

	if(!iSelectedRealm || !iSelectedProvince)
		fAllowRollButton = FALSE;
	y = startY+h-79;
	if (iSelectedRealm && iSelectedProvince)
	{
		CHAR temp1[80];
		CHAR temp2[80];
		CHAR temp3[80];
		fAllowRollButton = TRUE;
		if(province[iSelectedRealm].Terrain == HVY_MNT )
		{
			modGoldBars = 10;
			strcpy(temp1, STRMGR_GetStr(STR_ACTNMENU_ROAD_GB_COST_FMT));
			strcpy(temp2, STRMGR_GetStr(gsTerrain[province[iSelectedRealm].Terrain]));
			strcpy(temp3, STRMGR_GetStr(gsTerrain[province[iSelectedProvince].Terrain]));
			print_textf(x, y-12, DKBROWN,
				temp1, 
				temp2, 
				TerrainCost[province[iSelectedRealm].Terrain],
				temp3, 
				TerrainCost[province[iSelectedProvince].Terrain],
				modGoldBars);
		}
		else if(province[iSelectedProvince].Terrain	== HVY_MNT )
	    {
			modGoldBars = 10;
			strcpy(temp1, STRMGR_GetStr(STR_ACTNMENU_ROAD_GB_COST_FMT));
			strcpy(temp2, STRMGR_GetStr(gsTerrain[province[iSelectedRealm].Terrain]));
			strcpy(temp3, STRMGR_GetStr(gsTerrain[province[iSelectedProvince].Terrain]));
			print_textf(x, y-12, DKBROWN, 
				temp1, 
				temp2, 
				TerrainCost[province[iSelectedRealm].Terrain],
				temp3, 
				TerrainCost[province[iSelectedProvince].Terrain],
				modGoldBars);
		}
		else
		{
			modGoldBars = TerrainCost[province[iSelectedRealm].Terrain] + TerrainCost[province[iSelectedProvince].Terrain];
			if (playerstats[ActiveRegent] != fERROR)
			{
				DumbAutoLockPtr< PLAYER_STATS > const pPS(playerstats[ActiveRegent]);
			    modGoldBars = (modGoldBars * (100 + pPS->StatsMod.mfGetMod(STATSMOD::BUILDCOST_MOD)) + 50) / 100;
			    if (modGoldBars < 1)
			    	modGoldBars = 1;
			}
			strcpy(temp1, STRMGR_GetStr(STR_ACTNMENU_ROAD_GB_COST_FMT));
			strcpy(temp2, STRMGR_GetStr(gsTerrain[province[iSelectedRealm].Terrain]));
			strcpy(temp3, STRMGR_GetStr(gsTerrain[province[iSelectedProvince].Terrain]));
			print_textf(x, y-12, DKBROWN,
				temp1, 
				temp2, 
				TerrainCost[province[iSelectedRealm].Terrain],
				temp3, 
				TerrainCost[province[iSelectedProvince].Terrain],
				modGoldBars);
		}
	}
	GoldBar = GoldBar + modGoldBars;
	if(checkGoldReg)
		fAllowRollButton = (ProcessGoldAndReg(CHECK_QUITE))?fAllowRollButton:0;
	else
		fAllowRollButton = (ProcessGoldAndReg(CHECK_ONLY))?fAllowRollButton:0;
	GoldBar = GoldBar - modGoldBars;

	print_textf(x, y+12, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_TOTAL_GB_COST_FMT), GoldBar+modGoldBars);
	print_textf(x, y+25, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_ROAD_IS_FREE_ACTN));

}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
static BOOL RecursiveRoadFollower_sub (PROVINCE prov)
{
	LONG		j;
	BOOL		rv = FALSE;

	iSelectedHolding[prov][3] = TRUE;

	for (j=0; j<7; ++j)		// scan through borders
	{
		if ((province[prov].Border[j].MoveCost == PLAINS_ROAD
					|| province[prov].Border[j].MoveCost == RIVER_ROAD)
				&& AREALLIED(province[province[prov].Border[j].Province].Realm, CurrentRealm) )
		{
			if (iSelectedHolding[province[prov].Border[j].Province][3] == FALSE)
			{
				if (province[prov].Terrain != province[province[prov].Border[j].Province].Terrain)
					rv = TRUE;

				//LOGCOMMENTSLEEP
				//if ( fLogComment )
				//{
				//	char temp[100];
				//	sprintf ( temp, "   RecursiveRoadFollower_sub - found road from %s to %s",province[prov].name,province[province[prov].Border[j].Province].name );
				//	RandomLogComment ( temp );
				//}

				if (RecursiveRoadFollower_sub((PROVINCE)province[prov].Border[j].Province))
					rv = TRUE;
			}
		}

	}

	//LOGCOMMENTSLEEP
	//if ( fLogComment )
	//{
	//	char temp[100];
	//	if (rv)
	//		sprintf ( temp, "   RecursiveRoadFollower_sub - prov:%s returns TRUE",province[prov].name );
	//	else
	//		sprintf ( temp, "   RecursiveRoadFollower_sub - prov:%s returns FALSE",province[prov].name );
	//	RandomLogComment ( temp );
	//}

	return rv;
}



BOOL RecursiveRoadFollower (PROVINCE prov)
{
	LONG		j;

	for(j=0; j<PROVINCE_COUNT; ++j)
		iSelectedHolding[j][3] = 0;

	for (j=0; j<7; ++j)		// scan through borders
	{
		if (province[prov].Border[j].MoveCost == PLAINS_ROAD
				|| province[prov].Border[j].MoveCost == RIVER_ROAD)
		{
			//LOGCOMMENTSLEEP
			//if ( fLogComment )
			//{
			//	char temp[100];
			//	sprintf ( temp, "   RecursiveRoadFollower - found road from %s to %s",province[prov].name,province[province[prov].Border[j].Province].name );
			//	RandomLogComment ( temp );
			//}

			return RecursiveRoadFollower_sub(prov);
		}
	}

	//LOGCOMMENTSLEEP
	//if ( fLogComment )
	//{
	//	char temp[100];
	//	sprintf ( temp, "   RecursiveRoadFollower - Failed, found no road borders");
	//	RandomLogComment ( temp );
	//}
	return FALSE;
}

/* ======================================================================== */
void PaintTradeRoute (LONG x, LONG y, LONG h)
{
	BOOL			f = FALSE;
	LONG			i;
	LONG			j, k;
	LONG			startY;
	LONG			startX;
	LONG			tmpIndex;
	LONG			route;
	LONG			success;
	LONG			result;
	LONG			place;
	LONG		provCount = 0;
	LONG		counter = 0;

	GoldBar = RegencePoint = 0;
	for (i=0; i<PROVINCE_COUNT; ++i)
		iSelectedHolding[i][2] = 0;
	fAllowRollButton = FALSE;
	fAllowCancelButton = !fAllowDoneButton;

	print_textf(x, y, DKBROWN, "^F00%s", 		STRMGR_GetStr(STR_ACTNMENU_START_PROVINCE));
	print_textf(x+xSECONDPROV, y, DKBROWN, "^F00%s",STRMGR_GetStr(STR_ACTNMENU_END_PROVINCE));
	y += 12;
	startY = y;
	startX = x;

	del_region(NextPrev,0);

	// gather info about provinces
	for (i=FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)			// scan all provinces
	{
		CheckAndFixPlaces((PROVINCE)i);
		tmpIndex = j = province[i].FirstPlace;
		while (j != 0)							// scan for a GUILD we own
		{
			if (places[tmpIndex].Icon == GUILD1_ICON)		// look for GUILDS
			{
				for (k=0; k<3; ++k)		// scan through trade routes
				{
					route = (k==0)?places[tmpIndex].iRoute1:(k==1)?places[tmpIndex].iRoute2:places[tmpIndex].iRoute3;
					if (route && route < PROVINCE_COUNT)
					{
						++iSelectedHolding[i][2];			// mark both ends of a route
						++iSelectedHolding[route][2];
					}
				}
				if (places[tmpIndex].Realm == HomeRealm	// mark a GUILD of ours
					&& RecursiveRoadFollower((PROVINCE)i) )
				{
					iSelectedHolding[i][0] = tmpIndex;
					iSelectedHolding[i][1] = tmpIndex/256;
//printf("Province Info:%s  holding:%d  Icon%s  Realm:%s  NextPlace:%d\n",province[i].name,tmpIndex,GameIcons[places[tmpIndex].Icon],realm[places[tmpIndex].Realm].name ,places[tmpIndex].NextPlace);
				}
			}
			j = places[tmpIndex].NextPlace;		// next place in province
			tmpIndex = province[i].FirstPlace + j;
		}
	}

	// add buttons for provinces
	provCount = 0;
	counter = 0;
	for (int h1=0; h1<2; ++h1)
	{
		for (i=FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)			// scan all provinces
		{
			// if we can build a trade route from this prov
			j = iSelectedHolding[i][0] + (iSelectedHolding[i][1]*256);
			if (j>0 && iSelectedHolding[i][2] < ((province[i].CivLevel+2)/3) )
			{
				if (h1)
				{
					++counter;
					if(counter < startProvCount)
						continue;

//printf("Start Province:%s  guild:%d  num_of_routes:%d\n",province[i].name,*((SHORT*)(&iSelectedHolding[i][0])),iSelectedHolding[i][2]);
					init_gfont(FONT_SANS_8PT);
					DrawLittleButton(x, y, xPROVWIDTH, 10, (iSelectedRealm==i));
					add_region(x, y, xPROVWIDTH, 10, 0, SelectRealm,i,0,0, -1);
					gprint_text(x+2, y, province[i].name, LoyaltyColor[province[i].Loyalty]);
					y+=16;

					// print prev, next buttons if provinces are more than 13		
					if(DoPageProvince(&startProvCount, provCount, 13, counter, x, y))
						break;
				}
				else
					++provCount;
			}
		}
	}

	// add buttons for second province
	if (iSelectedRealm)
	{
		y = startY;

		RecursiveRoadFollower((PROVINCE)iSelectedRealm);	// find all prov reachable by road
		place = iSelectedHolding[iSelectedRealm][0] + (iSelectedHolding[iSelectedRealm][1]*256);

		provCount = 0;
		counter = 0;
		for (int h1=0; h1<2; ++h1)
		{
			for (i=FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)			// scan all provinces
			{
				if (i!=iSelectedRealm && iSelectedHolding[i][3]
						&& province[iSelectedRealm].Terrain != province[i].Terrain
						&& places[place].iRoute1 != i
						&& places[place].iRoute2 != i
						&& places[place].iRoute3 != i)
				{
					if (h1)
					{
						++counter;
						if(counter < startProvCount1)
							continue;

						DrawLittleButton(x+xSECONDPROV, y, xPROVWIDTH+14, 10, (iSelectedProvince==i));
						add_region(x+xSECONDPROV, y, xPROVWIDTH+14, 10, 0, SelectProvince,i,0,0, -1);
						DrawShield(x+xSECONDPROV+2, y, (REALM::REALM_TYPE)province[i].Realm);
						init_gfont(FONT_SANS_8PT);
						gprint_text(x+xSECONDPROV+16, y, province[i].name, StatusColor[(RealmStatus[HomeRealm][province[i].Realm])&0x0F]);
						y+=16;

						// print prev, next buttons if provinces are more than 13		
						if(DoPageProvince(&startProvCount1, provCount, 13, counter, x+xSECONDPROV, y))
							break;
					}
					else
						++provCount;
				}
			}
		}
		ClearRemapTable();
	}

	// print success and allow modification
	if(!iSelectedRealm || !iSelectedProvince)
		fAllowRollButton = FALSE;
	
	if (iSelectedRealm && iSelectedProvince)
	{
		fAllowRollButton = TRUE;
		y = startY+h-84;

		GoldBar = 1;
		RegencePoint = modRegency + 1;
		if(checkGoldReg)
			fAllowRollButton = (ProcessGoldAndReg(CHECK_QUITE))?fAllowRollButton:0;
		else
			fAllowRollButton = (ProcessGoldAndReg(CHECK_ONLY))?fAllowRollButton:0;
		
		//if(fAllowRollButton) // Need to be able to modify this.
		{
			place = iSelectedHolding[iSelectedRealm][0] + (iSelectedHolding[iSelectedRealm][1]*256);
	//printf("Start Province:%s  guild:%d  num_of_routes:%d\n",province[iSelectedRealm].name,place,iSelectedHolding[iSelectedRealm][2]);
	
			success = 10 - places[place].Level - modRegency + modOpponent;
	
			print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_F02_SUCCESS));
			print_textf(x+66, y+6, DKBROWN, "^F02^c%d%c", D20TOPERCENT(success),PERCENTCHAR);
			if (fLTAction != DURING_LTACTION)
			{
				print_textf(x+95, y-2, DKBROWN, STRMGR_GetStr(STR_MAPAI_MODIFY_SUCCESS));
				print_textf(x+95, y+4, DKBROWN, 	STRMGR_GetStr(STR_MAPAI_USING_REGENCY));
				SetupMod(x+170, y, &modRegency, 0, (9-places[place].Level+modOpponent), 1);
			}
		}
		// multi-player and AI support
		if(lAskedTargetWhichRoll == iThisRoll + 1)
		{
			success = AskAndWaitForTargetPlayer(TRADE_ROUTE, iSelectedRealm, 0, success, x+63+7, y);
			success = AskAndWaitForTargetPlayer(TRADE_ROUTE, iSelectedProvince, 0, success, x+63+7, y);
		}

		// check for success roll
		result = RollSuccess(success, x+63+7, y, TRUE);
		if (result == 1)	// success
		{
			ActionExpPoints(ActiveRegent, RLM_EXP_BUILD_TRADEROUTE, 0, TRUE );
			// multi-player support
			SendResult(TRADE_ROUTE, (LONG)iSelectedRealm, 0, result);
			SendResult(TRADE_ROUTE, (LONG)iSelectedProvince, 0, result);
			SyncRegencySpentInOpposition((REALM::REALM_TYPE)province[iSelectedRealm].Realm);
			SyncRegencySpentInOpposition((REALM::REALM_TYPE)province[iSelectedProvince].Realm);
			if (places[place].iRoute1==0)
			{
				places[place].iRoute1 = iSelectedProvince;
				SetGameData(MP_PLACES, MPPLACE_IROUTE1, place, iSelectedProvince, TRUE );
//printf("Trade Route - holding:%d iRoute1 from %s to %s\n",place,province[iSelectedRealm].name,province[iSelectedProvince].name);
			}
			else if (places[place].iRoute2==0)
			{
				places[place].iRoute2 = iSelectedProvince;
				SetGameData(MP_PLACES, MPPLACE_IROUTE2, place, iSelectedProvince, TRUE );
//printf("Trade Route - holding:%d iRoute2 from %s to %s\n",place,province[iSelectedRealm].name,province[iSelectedProvince].name);
			}
			else
			{
				places[place].iRoute3 = iSelectedProvince;
				SetGameData(MP_PLACES, MPPLACE_IROUTE3, place, iSelectedProvince, TRUE );
//printf("Trade Route - holding:%d iRoute3 from %s to %s\n",place,province[iSelectedRealm].name,province[iSelectedProvince].name);
			}
//			AddReport(IMMEDIATE_TRADE_RT, iSelectedRealm, iSelectedProvince, 0);
		}
		if (result == 2)	// failure
		{
			// multi-player support
			SendResult(TRADE_ROUTE, iSelectedRealm, 0, result);
			SendResult(TRADE_ROUTE, iSelectedProvince, 0, result);
			SyncRegencySpentInOpposition((REALM::REALM_TYPE)province[iSelectedRealm].Realm);
			SyncRegencySpentInOpposition((REALM::REALM_TYPE)province[iSelectedProvince].Realm);
//			AddReport(IMMEDIATE_TRD_RT_FAIL, iSelectedRealm, iSelectedProvince, 0);
		}
/*
		GoldBar = 1;
		RegencePoint = modRegency + 1;
		if(checkGoldReg)
			fAllowRollButton = (ProcessGoldAndReg(CHECK_QUITE))?fAllowRollButton:0;
		else
			fAllowRollButton = (ProcessGoldAndReg(CHECK_ONLY))?fAllowRollButton:0;
*/
	}
	if (startY == y && startX == x && !fAllowDoneButton && GoldBar==0 && !iSelectedRealm)
	   gprint_text(x, y,STRMGR_GetStr(STR_ACTNMENU_CANT_BUILD_ROAD) , DKBROWN);

	y = startY+h-69;
	print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_TOTAL_COST_FMT), GoldBar, RegencePoint);
	print_textf(x, y+15, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_SELECT_START_AND_END));

	if (fAllowDoneButton)
		DeleteActnMenuRegions();
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
//#define xPROVWIDTH			(125)			see above
#define xUNITTYPES			(150)
#define xUNITNUM				(270)

void PaintMuster (LONG x, LONG y, LONG h)
{
	SHORT		i, j;
	SHORT		startY;
	SHORT		units;
	UBYTE		numType;
	SHORT 	troops=0;     // number of troops you already have
	LONG		provCount = 0;
	LONG		provCount1 = 0;
	LONG		h1;
	LONG		counter = 0;
	LONG		counter1 = 0;
	LONG		done=0;
	LONG		hStat;
	modGoldBars = 0;
	fAllowRollButton = FALSE;

	print_textf(x, y, DKBROWN, "^F00%s", STRMGR_GetStr(STR_ACTNMENU_CAPS_PROVINCE));
	y += 12;
	startY = y;

	del_region(NextPrev,0);

	hStat = LoadStats(regents[ActiveRegent].mfGetid(), -1);
	if(hStat == fERROR)
		 return;
	
	DumbAutoLockPtr< PLAYER_STATS > const pStat(hStat);
	InventoryItor itor(pStat->Inventory);
	LONG IHaveTheChalice = itor.mfFind(ITEM_CHALICE_OF_THE_DEAD);

	// add buttons for provinces
	provCount = 0;
	counter = 0;
	for (h1=0; h1<2; ++h1)
	{
		for (i=FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)			// scan all provinces
		{
			if (province[i].Realm == HomeRealm)
			{
				if (h1)
				{
					++counter;
					if (counter < startProvCount)
						continue;

					init_gfont(FONT_SANS_8PT);
					DrawLittleButton(x, y, xPROVWIDTH, 10, (iSelectedProvince==i));
					add_region(x, y, xPROVWIDTH, 10, 0, SelectProvince,i,0,0, -1);
					DrawShield(x, y, (REALM::REALM_TYPE)province[i].Realm);
					gprint_text(x+14, y, province[i].name, LoyaltyColor[province[i].Loyalty]);
					y+=16;

					// print prev, next buttons if provinces are more that 14
					if (DoPageProvince(&startProvCount, provCount, 14, counter, x, y))
						break;
				}
				else
					++provCount;
			}
		}
	}
	ClearRemapTable();

	// add selectors for troop types
	y = startY;
	if (iSelectedProvince)
	{
		for (i=1; i<C_SPD_UNIT_ICON; ++i)
			troops+=iSelectedHolding[i][1];			// keep track of the number of the troops we already have.

		for (h1=0; h1<2; ++h1)
		{
			for (i=1; i<C_SPD_UNIT_ICON; ++i)
			{
			  	if (	( (province[iSelectedProvince].HumanPop && i>=A_INF_UNIT_ICON && i<=A_CAV_UNIT_ICON)
			  			||(province[iSelectedProvince].ElfPop && i>=E_INF_UNIT_ICON && i<=E_CAV_UNIT_ICON)
			  			||(province[iSelectedProvince].DwarfPop && i>=D_INF_UNIT_ICON && i<=D_BOW_UNIT_ICON)
			  			||(province[iSelectedProvince].GoblinPop && i>=G_INF_UNIT_ICON && i<=G_CAV_UNIT_ICON)
			  			||(i>=M_INF_UNIT_ICON && i<=M_CAV_UNIT_ICON) 
						|| (IHaveTheChalice && i==C_SKL_UNIT_ICON))
			  			&& (province[iSelectedProvince].CivLevel >= LevelUnitReqt[i]) 
			  			&& ((cntUnits_() + troops) < (MAX_UNITS - 10))
			  		)
				{
					if (h1)
					{
						++counter1;
						if(counter1 < startProvCount1)
							continue;
						init_gfont(FONT_SANS_8PT);
						print_textf(x+xUNITTYPES, y, DKBROWN, "%s", STRMGR_GetStr(gsUnitTitle[i]));
						add_region(x+xUNITTYPES, y-2, 120, 16, 0, InitBattleCard_alt, i, HomeRealm, 0, -1);
						if(i == C_SKL_UNIT_ICON)
							SetupMod(x+xUNITNUM, y, &iSelectedHolding[i][1], 0, 1-iSelectedHolding[i][1]-musterSkl, 1);
						else
							SetupMod(x+xUNITNUM, y, &iSelectedHolding[i][1], 0, province[iSelectedProvince].CivLevel - province[iSelectedProvince].LevyMustered-troops+iSelectedHolding[i][1], 1);

						//modGoldBars += iSelectedHolding[i][1] * UnitMusterCost[i];
						y+=16;
						//if(iSelectedHolding[i][1])
						//	done = TRUE;
						//fAllowRollButton = done;
						// print prev, next buttons if provinces are more than 13		
						if(DoPageProvince(&startProvCount1, provCount1, 13, counter1, x+xUNITTYPES, y))
							break;
					}
					else
					{
						// testing if gray the "Muster" button"
						if(iSelectedHolding[i][1])
							done = TRUE;
						if(i == C_SKL_UNIT_ICON && iSelectedHolding[i][1])
							modGoldBars += 1;
						else
							modGoldBars += iSelectedHolding[i][1] * UnitMusterCost[i];
						fAllowRollButton = done;
						++provCount1;
		
					}
				}
			}
		}
	 	GoldBar += modGoldBars;
		if(checkGoldReg)
			fAllowRollButton = (ProcessGoldAndReg(CHECK_QUITE))?fAllowRollButton:0;
		else
			fAllowRollButton = (ProcessGoldAndReg(CHECK_ONLY))?fAllowRollButton:0;
		GoldBar -= modGoldBars;

	   if( IsMenuActive(D_BTL_CARD) )
			DeleteActnMenuRegions();
	}
	if(iSelectedProvince)
	{
		y = startY+h-81;
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_MUSTER_GB_COST), province[iSelectedProvince].name, modGoldBars);
	}
	y = startY+h-67;
	print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_TOTAL_GB_COST_FMT), GoldBar+modGoldBars);
	print_textf(x, y+13, DKBROWN,STRMGR_GetStr(STR_ACTNMENU_MUSTER_IS_FREE));

}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
#define xLOYALTY		104
#define xRAISE			153
#define xLOWER			183
#define xcSUCCESS		227
#define xUSEGOLD		258
#define xUSEREG		293

void PaintAgitate (LONG x, LONG y, LONG h)
{
	LONG		i, j, q;
	LONG		startY = y;
	LONG		tmpIndex;
	LONG		max_holding;
	LONG		rulers_law;
	LONG		success;
	LONG		result;
	LONG		provCount = 0;
	LONG		counter = 0;
	LONG		lPrint = TRUE;
	LONG		lPageDone = FALSE;

	GoldBar = RegencePoint = 0;
	fAllowCancelButton = !fAllowDoneButton;

	y+= 3;
	print_textf(x, y, DKBROWN, "^F00%s", STRMGR_GetStr(STR_ACTNMENU_CAPS_PROVINCE));
	print_textf(x+xLOYALTY, y, DKBROWN,   STRMGR_GetStr(STR_ACTNMENU_CAPS_LOYALTY));
	print_textf(x+xLOWER-1, y-4, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_cCAPS_CHANGE));
	print_textf(x+xLOWER-1, y+3, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_cCAPS_LOYALTY));
	print_textf(x+xcSUCCESS, y+3, DKBROWN,STRMGR_GetStr(STR_ACTNMENU_cCAPS_SUCCESS));
	print_textf(x+3+(xUSEGOLD+xUSEREG)/2, y-5, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_cMODIFY_SUCCESS));
	line(x+xUSEGOLD-11, y-2, x+xUSEREG+17, y-2, DKBROWN);
	print_textf(x+xUSEGOLD, y+3, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_cGOLD));
	print_textf(x+xUSEREG, y+3, DKBROWN,  STRMGR_GetStr(STR_ACTNMENU_cREGENCY));
	y+= 12;

	del_region(NextPrev,0);
	for (int h1=0; h1<2; ++h1)
	{
		for(q = 0; q < 2; ++q)
		{
			for (i=FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)			// scan all provinces
			{
				j = 0;
				max_holding = -1;						// our highest level holding in prov
				rulers_law = 0;						// level of ruler's law holding
		
				CheckAndFixPlaces((PROVINCE)i);
				tmpIndex = j = province[i].FirstPlace;
				while(j != 0)
				{
		
					// our highest level holding in prov
					if (places[tmpIndex].Realm == HomeRealm && (LONG)places[tmpIndex].Level > max_holding)
						max_holding = places[tmpIndex].Level;
		
					// level of ruler's law holding
					if (places[tmpIndex].Realm == province[i].Realm && places[tmpIndex].Icon == LAW1_ICON)
						rulers_law = (province[i].Realm == HomeRealm) ? -places[tmpIndex].Level :	places[tmpIndex].Level;
		
					j = places[tmpIndex].NextPlace;	// next place in province
					tmpIndex = province[i].FirstPlace + j;
				}
		
				// if we have a holding in prov
				if ( (!q && province[i].Realm == HomeRealm && province[i].Honest == 0) || (q && max_holding > -1 && province[i].Honest == 0 && province[i].Realm != HomeRealm))
				{
					if (h1)
					{
						++counter;
						if(counter < startProvCount || lPageDone)
							lPrint = FALSE;
						else
							lPrint = TRUE;
	
						if(lPrint)
						{
							init_gfont((strlen(province[i].name)>15)?21:FONT_SANS_8PT);
							DrawShield(x-4, y, (REALM::REALM_TYPE)province[i].Realm);
							if (province[i].Realm==HomeRealm)
							   gprint_text(x+10, y, province[i].name, LoyaltyColor[province[i].Loyalty]);
							else
							   gprint_text(x+10, y, province[i].name, DKBROWN);
			
							print_textf(x+xLOYALTY, y+2, DKBROWN, "^F01^B%s", 
								STRMGR_GetStr(gsLoyaltyUC[province[i].Loyalty+(iSelectedHolding[i][0]==2)-(iSelectedHolding[i][0]==3)]));
			
							if (province[i].Loyalty < 3)
							{
								DrawLittleButton(x+xRAISE, y+1, 25, 8, (iSelectedHolding[i][0]==2 || iSelectedHolding[i][0]==4));
								add_region(x+xRAISE, y+1, 25, 8, 0, SetNum,(LONG)&iSelectedHolding[i][0],2,0, -1);
							}
							else
								color_rect(x+xLOWER, y+1, 25, 8, LTTAN-4);
							print_textf(x+xRAISE+13, y+6, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_cRAISE));
			
							if (province[i].Loyalty>0)
							{
								DrawLittleButton(x+xLOWER, y+1, 25, 8, (iSelectedHolding[i][0]==3 || iSelectedHolding[i][0]==5));
								add_region(x+xLOWER, y+1, 25, 8,0, SetNum,(LONG)&iSelectedHolding[i][0],3,0, -1);
							}
							else
								color_rect(x+xLOWER, y+1, 25, 8, LTTAN-4);
							print_textf(x+xLOWER+13, y+6, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_cLOWER));
						}
			
						// success number
						success = 10 + province[i].CivLevel - max_holding + rulers_law - iSelectedHolding[i][1] - iSelectedHolding[i][2]+modOpponent;
						if (playerstats[ActiveRegent] != fERROR)
						{
							DumbAutoLockPtr< PLAYER_STATS > const pPS(playerstats[ActiveRegent]);
						    success -= pPS->StatsMod.mfGetMod(STATSMOD::AGITATE_MOD);
						}
						if(lPrint)
							print_textf(x+xcSUCCESS, y+6, DKBROWN, (iSelectedHolding[i][0])?"^F02^c%d%c":"^F02^T2^c%d%c", D20TOPERCENT(success),PERCENTCHAR);
			
						if (iSelectedHolding[i][0])	// selected raise or lower
						{
							// Gold Bars to modify
							if(lPrint)
								SetupMod(x+244, y, &iSelectedHolding[i][1], 0, (9+province[i].CivLevel-max_holding+rulers_law-iSelectedHolding[i][2]+modOpponent), 1);
							GoldBar += iSelectedHolding[i][1] + (iSelectedHolding[i][0]!=0);
							fAllowRollButton = (GoldBar>0||RegencePoint>0)?TRUE:FALSE;
							// regency to modify
							if (fLTAction != DURING_LTACTION)
							{
								if(lPrint)
									SetupMod(x+284, y, &iSelectedHolding[i][2], 0, (9+province[i].CivLevel-max_holding+rulers_law-iSelectedHolding[i][1]+modOpponent), 1);
								RegencePoint += iSelectedHolding[i][2] + (iSelectedHolding[i][0]!=0);
							}
			
							// check for success roll
							if (iSelectedHolding[i][0])
							{
								// multi-player and AI support
								if(lAskedTargetWhichRoll == iThisRoll + 1)
									success = AskAndWaitForTargetPlayer(AGITATE, i, 0, success, x+xcSUCCESS, y);
	
								result = RollSuccess(success, x+xcSUCCESS, y, lPrint);
								if (result == 1)	// success
								{
									// reward the regent
									ActionExpPoints(ActiveRegent, RLM_EXP_AGITATE, 0, TRUE );
									// multi-player support
									SendResult(AGITATE, i, 0, result);
									SyncRegencySpentInOpposition((REALM::REALM_TYPE)province[i].Realm);
									province[i].Loyalty += (iSelectedHolding[i][0]==2) - (iSelectedHolding[i][0]==3);
									if(iSelectedHolding[i][0]==2)
										SetHelped((REALM::REALM_TYPE)province[i].Realm, HomeRealm, TRUE );
									else if(iSelectedHolding[i][0]==3)
										SetScrewed((REALM::REALM_TYPE)province[i].Realm, HomeRealm, TRUE );
									SetGameData(MP_PROVINCE, MPPROV_LOYALTY, i, province[i].Loyalty, TRUE );
	//								AddReport(IMMEDIATE_AGITATE, (iSelectedHolding[i][0]==3), i, province[i].Loyalty);
									iSelectedHolding[i][0]+=2;		// don't add to printed loyalty anymore
								}
								if (result == 2)	// failure
								{
									// multi-player support
									SendResult(AGITATE, i, 0, result);
									SyncRegencySpentInOpposition((REALM::REALM_TYPE)province[i].Realm);
	//								AddReport(IMMEDIATE_AGITATE_FAIL, i, 0, 0);
									iSelectedHolding[i][0]+=2;		// don't add to printed loyalty anymore
								}
							}
						}
						else
						{
							if(lPrint)
							{
								print_textf(x+244+15, y+6, DKBROWN, "^F02^c-");
								print_textf(x+284+15, y+6, DKBROWN, "^F02^c-");
							}
						}
	
						if(lPrint)		
							y+= 15;
			
						// print prev, next buttons if provinces are more than 15		
						if(lPrint)
						{
							if(DoPageProvince(&startProvCount, provCount, 15, counter, x, y))
							{
								lPrint = FALSE;
								lPageDone = TRUE;
							}
						}
					}
					else
						++provCount;
				}
			}
		}
	}
	if(checkGoldReg)
		fAllowRollButton = (ProcessGoldAndReg(CHECK_QUITE))?fAllowRollButton:0;
	else
		fAllowRollButton = (ProcessGoldAndReg(CHECK_ONLY))?fAllowRollButton:0;
	
	print_textf(x, startY+h-60, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_TOTAL_COST_FMT), GoldBar, RegencePoint);
	print_textf(x, startY+h-45, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_ANY_NUM_PROVS));
	if (fAllowDoneButton)
		DeleteActnMenuRegions();
}
/* ========================================================================
   Function    - 
   Description - 
   Returns     -   pass in a place index, return the province index
					if return zero, the place index is illegal.
   ======================================================================== */
static PROVINCE FindProvForPlace(LONG place)
{
 	PROVINCE i;
 	LONG j, k;
	for(i = FIRST_PROVINCE; i<PROVINCE_COUNT; i = (PROVINCE)(i + 1))
	{	
		CheckAndFixPlaces(i);
		j = k = province[i].FirstPlace;
		while(j != 0)
		{
			if(k == place)
				return (PROVINCE) i;
			j = places[k].NextPlace;
			k = province[i].FirstPlace + j;
		}
	}
	return NO_PROVINCE;
}	
/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
static void CheckHoldingConflict(LONG place1, LONG	place2, BOOL fSend)
{
	LONG j, m;
	LONG	prov=0;
	BOOL flag=FALSE;
	USHORT tmpRealm;
	//check if provinces are the same
	if(places[place1].Icon==places[place2].Icon)
	{
		for(LONG i=FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)
		{
			CheckAndFixPlaces((PROVINCE)i);
			j=province[i].FirstPlace;
			m=province[i].FirstPlace;
			while(m!=0)
			{
				if((j==place1 || j==place2) && prov!=0)
				{
			   		if(i==prov)
						flag=TRUE;
					break;
				}
				else if((j==place1 || j==place2) && prov==0)   
					prov=i;
			  	m=places[j].NextPlace;
				j=province[i].FirstPlace+m;
			}
			if(m!=0)
				break;
		}   	
	}
	if(flag)
	{
		tmpRealm = places[place1].Realm; 
		SetGameData(MP_PLACES, MPPLACE_REALM, place1, places[place2].Realm, fSend );
		SetGameData(MP_PLACES, MPPLACE_REALM, place2, tmpRealm, fSend );
	}
	else
	{
		tmpRealm = places[place1].Realm;
		CheckHolding(place1, (REALM::REALM_TYPE)places[place2].Realm, fSend);
		CheckHolding(place2, (REALM::REALM_TYPE)tmpRealm, fSend);
	}
}		

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
void CheckHolding(LONG place, REALM::REALM_TYPE realm_, BOOL fSend)
{
	LONG i;
	PROVINCE prov;
	
	prov = FindProvForPlace(place);
		
	for (i=0; i<MAX_PLACES; ++i)
	{
		if(places[i].Realm == realm_
				&& places[i].Icon == places[place].Icon
				&& FindProvForPlace(i)==prov)
		{
			if(places[place].Level > places[i].Level)
				places[i].Level = (places[i].Level > 0) ? (places[place].Level + 1) : places[place].Level;
		 	else
				places[i].Level = (places[place].Level > 0) ? (places[i].Level + 1)	: places[i].Level;
		
			DeleteHolding (prov, place, fSend);
			SetGameData(MP_PLACES, MPPLACE_LEVEL, i, places[i].Level, fSend );
			SetGameData(MP_PLACES, MPPLACE_IICONBITM, i, places[i].Icon+(((places[i].Level<8)?places[i].Level:7)/2), fSend );
			break;
		}
	}
	if(i == MAX_PLACES)
		SetGameData(MP_PLACES, MPPLACE_REALM, place, realm_, fSend );

}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
//CSTRPTR	szDiplomacySubj[] = {
///*00*/	"Seek Permissive Alliance","needs 75%c",
///*01*/	"Seek Full Alliance","needs 95%c",
///*02*/	"Seek Oath of Fealty","needs 150%c",
///*03*/	"Request Gold","-3%c per GB",
///*04*/	"Demand Territory","varies",
///*05*/	"Demand Holding","-10%c per level",
///*06*/	"Demand Tribute","-10%c per GB",
///*07*/	"Offer Territory","varies",
///*08*/	"Offer Holding","+5%c per level",
///*09*/	"Offer Tribute","+5%c per GB",
///*10*/	"Offer Gold","+2%c per GB",
///*09b*/	"Offer Salary","varies",
///*  */	"Offer Regency", "",	};

static DEFINE_STATIC_VECTOR_DATA(GAME_STRING,gsDiplomacySubj) = {
/*00*/	STR_DIPLOMACY_SEEK_PERMISSIVE,STR_DIPLOMACY_NEEDS_75,
/*01*/	STR_DIPLOMACY_SEEK_FULL,STR_DIPLOMACY_NEEDS_95,
/*02*/	STR_DIPLOMACY_SEEK_OATH_FEALTY,STR_DIPLOMACY_NEEDS_150,
/*03*/	STR_DIPLOMACY_REQ_GOLD,STR_DIPLOMACY_VARIES,
/*04*/	STR_DIPLOMACY_DEMAND_TERR,STR_DIPLOMACY_VARIES,
/*05*/	STR_DIPLOMACY_DEMAND_HOLDING,STR_DIPLOMACY_MINUS_30,
/*06*/	STR_DIPLOMACY_DEMAND_TRIBUTE,STR_DIPLOMACY_VARIES,
/*07*/	STR_DIPLOMACY_OFFER_TERR,STR_DIPLOMACY_VARIES,
/*08*/	STR_DIPLOMACY_OFFER_HOLDING,STR_DIPLOMACY_PLUS_15,
/*09*/	STR_DIPLOMACY_OFFER_TRIBUTE,STR_DIPLOMACY_VARIES,
/*10*/	STR_DIPLOMACY_OFFER_GOLD,STR_DIPLOMACY_VARIES,
/*09b*/	STR_DIPLOMACY_OFFER_SALARY,STR_DIPLOMACY_VARIES,
/*  */	STR_DIPLOMACY_OFFER_REGENCY,STR_NULL };
DEFINE_STATIC_VECTOR_CLASS(GAME_STRING, gsDiplomacySubj);


//																				    ____Demand_____ _____Offer_____
//																	    Prm Ful Vsl GBs Ter Hld Trb Ter Hld Trb GBs
static DEFINE_STATIC_VECTOR_DATA(LONG,MinReaction) =	{75, 95,150, 60, 50, 50, 50,  0,  0,  0,  0};
DEFINE_STATIC_VECTOR_CLASS(LONG, MinReaction);
static DEFINE_STATIC_VECTOR_DATA(LONG,Modifiers) =		{ 0,  0,  0, -3,-40,-10,-10, 20,  5,  5,  2};
DEFINE_STATIC_VECTOR_CLASS(LONG, Modifiers);

DEFINE_MATRIX_DATA_S(UBYTE,align,10,10)={
						/*		N  LG  LN   LE   NG   NN   NE   CG   CN   CE */
 				  /*N*/    {0,  0,  0,   0,   0,   0,   0,   0,   0,   0},
 				  /*LG*/	  {0, 15,  5,  85,  10,   0,  80,   0,  90,  70},
 				  /*LN*/	  {0,  5,  15,  5,   0,  10,   0,  90,   0,  90},
 				  /*LE*/	  {0, 85,  5,  15,  80,   0,  10,  70,  90,   0},
 				  /*NG*/	  {0, 10,  0,  80,  15,   5,  85,  10,   0,  80},
 				  /*NN*/	  {0,  0, 10,   0,   5,  15,   5,   0,  10,   0},
 				  /*NE*/	  {0, 80,  0,  10,  85,   5,  15,  80,   0,  10},
 				  /*CG*/   {0,  0, 90,  70,  10,   0,  80,  15,   5,  85},
 				  /*CN*/   {0, 90,  0,  90,   0,  10,   0,   5,  15,   5},
 				  /*CE*/   {0, 70, 90,   0,  80,   0,  10,  85,   5,  15}};
 // if the alignment is negative, add 100 to it. If the number is -5, in align 
 // array, the value is 95.
DEFINE_MATRIX_CLASS_S(UBYTE, align, 10, 10);

//																		"NONE","minimal","quaint","average","opulent"
static DEFINE_STATIC_VECTOR_DATA(LONG,CourtEffect) = {   0,		  4,       2,        0,        -3};
DEFINE_STATIC_VECTOR_CLASS(LONG, CourtEffect);

static int NonLandedOathValue[] = {				//MO AC DC MB JE
/* 35  N_IMPORTS_EXPORTS				*/		(( 3+10+ 0+10+ 0)/5),
/* 36  IMPREGNABLE_HEART_OF_HAELYN	*/		((22+ 0+15+10+50)/5),
/* 37  S_ANUIRE_SHIPPING_AND_IMPORTS*/		(( 0+10+ 5+10+ 0)/5),
/* 38  HIGH_MAGE_AELIES					*/		((20+20+30+20+40)/5),
/* 39  PORT_OF_CALL_EXCHANGE			*/		(( 0+10+ 0+10+ 0)/5),
/* 40  ORTHODOX_IMP_TEMPLE_OF_HAELYN*/		(( 8+ 0+15+10+10)/5),
/* 41  WIZARD_HERMEDHIE					*/		(( 0+20+10+ 0+30)/5),
/* 42  CELESTIAL_JEWEL_OF_SARIMIE	*/		((18+ 0+25+10+30)/5),
/* 43  HIDDEN_TEMPLE_OF_CUIRAECEN	*/		(( 7+ 0+10+10+10)/5),
/* 44  UPPER_ANUIRE_TRADERS			*/		(( 9+10+ 0+10+20)/5),
/* 45  HAELYNS_AEGIS						*/		((23+10+20+10+40)/5),
/* 46  MILITANT_ORDER_OF_CUIRAECEN	*/		((30+10+25+20+45)/5),
/* 47  POINTS_EAST_TRADING_GUILD		*/		(( 9+10+ 5+10+15)/5),
/* 48  WIZARD_SECOND_SWAMP_MAGE		*/		((16+20+20+ 0+30)/5),
/* 49  N_REFORMED_CHURCH_OF_SARIMIE	*/		((10+ 0+15+10+15)/5),
/* 50  WIZARD_EYELESS_ONE				*/		(( 9+30+25+ 0+30)/5),
/* 51  RUARCH_ROCKHAMMER				*/		((15+10+10+50+30)/5)	};

// for landed realms only
#define BASE_REACTION_FOR_OATH_FEALITY					150
#define BASE_REACTION_FOR_FULL_ALLIANCE				95
#define OATH_FEALITY_PERCENT_ADDED_PER_PROVINCE		10
#define FULL_ALLIANCE_PERCENT_ADDED_PER_PROVINCE	5

// for multi-player diplomacy
extern LONG iGlobRealm;

/* ======================================================================== */
UBYTE ReactionAndStatusColor(LONG Realm1, LONG Realm2)
{
	if (ISATWAR(Realm1,Realm2) || ISATWAR(Realm2, Realm1)) return MDRED;
	if (RealmReaction[Realm2][Realm1] < 25) return ORANGE;
	return StatusColor[(RealmStatus[Realm2][Realm1])&0x0F];
}


/* ======================================================================== 
   calculate the value of all the units in prov belongs to realm_
   ========================================================================*/ 
static LONG CalcMultUnitsValue(PROVINCE prov, REALM::REALM_TYPE realm_)
{
 	LONG j, iUnit, sum=0;
	for(j = 1; j<MAX_UNITS; ++j)
	{
	   	if (units[j].province == prov 
		   	&& units[j].Realm == realm_ 
		   	&& units[j].Joined == 0)
	   	{
		   if(units[j].Icon < FIRST_CHARACTER_TYPE )
			{
		 		for(iUnit = j; iUnit > 0; iUnit = units[iUnit].NextUnit)
			 		sum += UnitMusterCost[units[iUnit].Icon];
			}
		   else if(units[j].Icon >= FIRST_CHARACTER_TYPE )
			{
		 		for(iUnit = j; iUnit > 0; iUnit = units[iUnit].NextUnit)
		   			if(units[iUnit].Icon < FIRST_CHARACTER_TYPE) 
			 			sum += UnitMusterCost[units[iUnit].Icon];
			}
		}
	}
	return sum;	
}	

/* ======================================================================== */
static LONG	FullNeeds (REALM::REALM_TYPE _realm)
{
	LONG i, cProvs = 0;

	if (fTutorialSelected)
		return (BASE_REACTION_FOR_FULL_ALLIANCE);

	if (_realm && _realm >= LAND_REALM_COUNT)
		return (BASE_REACTION_FOR_FULL_ALLIANCE);

	for (i=FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)			// scan all provinces
		if (province[i].Realm == _realm)
			++cProvs;

	return ((cProvs * (FULL_ALLIANCE_PERCENT_ADDED_PER_PROVINCE)) + (BASE_REACTION_FOR_FULL_ALLIANCE - FULL_ALLIANCE_PERCENT_ADDED_PER_PROVINCE));
}

/* ======================================================================== */
static LONG	OathNeeds (REALM::REALM_TYPE _realm)
{
	LONG i, cProvs = 0;

	if (fTutorialSelected)
		return (BASE_REACTION_FOR_OATH_FEALITY);

	if (_realm && _realm >= LAND_REALM_COUNT)
		return ((BASE_REACTION_FOR_OATH_FEALITY) + NonLandedOathValue[_realm-LAND_REALM_COUNT]);

	for (i=FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)			// scan all provinces
		if (province[i].Realm == _realm)
			++cProvs;

	return ((cProvs * (OATH_FEALITY_PERCENT_ADDED_PER_PROVINCE)) + (BASE_REACTION_FOR_OATH_FEALITY - OATH_FEALITY_PERCENT_ADDED_PER_PROVINCE));
}

/* ======================================================================== */
void PaintDiplomacy (LONG x, LONG y, LONG h)
{
	LONG	i, xs, success, mod, f, tmpIndex, j, k, count, iUnit, index, saveY;
	LONG	result, tmp, troopValue, time, theirCurGold, tmpLevel;
	LONG	startY = y, modMod;
	LONG	startX = x;
	LONG	counter = 0;
	LONG	counter1 = 0;
	LONG	provCount = 0;
	LONG	provCount1 = 0;
	LONG	minReact = 0;
	LONG	offers = 0;
	LONG	demands = 0;
	LONG	curReact = 0;
	LONG	lPrint = TRUE;
	LONG	lPrint1 = TRUE;
	LONG	lPageDone = FALSE;
	LONG	lPageDone1 = FALSE;
	LONG	answer, alignMod;
	LONG	investor, victim, target_province;
 	PTR_PLAYER_STATS pYourStat;
 	LONG	myAlign=0; 
 	LONG	yourAlign=0;
 	LONG	relicMod=0;
 	LONG	modifyMod=0;
 	LONG	warMod=0;
	static LONG reaction=0;
	static LONG	typAlliance = 0;
	UBYTE	coReaction;


	del_region(NextPrev,0);
 	if (playerstats[ActiveRegent] != fERROR)
 	{
		DumbAutoLockPtr< PLAYER_STATS > const pPS(playerstats[ActiveRegent]);
		InventoryItor itor(pPS->Inventory);
 		myAlign = pPS->mfGetAlignment();
		
		for (itor.mfAdvance(IC_BLOODAB);
			 itor != pPS->Inventory.end();
			 itor.mfAdvanceToNext(IC_BLOODAB))
		{
			if ((*itor)->mfType() == ITEM_DIVINE_AURA)
 			{
 				if (relicMod < 15)
	 				relicMod = 15;
 			}
			else if ((*itor)->mfType() == ITEM_BLOODMARK)
 			{
 				if (relicMod < 10)
	 				relicMod = 10;
 			}
 		}
 	}
	fAllowCancelButton = !fAllowDoneButton;


	if (iSelectedRealm && iSelectedRealm >= LAND_REALM_COUNT
			&& CheckVassal((REALM::REALM_TYPE)iSelectedRealm) && !ISVASSAL(iSelectedRealm, HomeRealm) )
	{
DiplomacyRestart:
		AddSndObj((BIRTHRT_SND)SND_UI_DID_NOT_SUCCEED,0,VOLUME_NINETY);
		InitActionGlobals();
		fAllowCancelButton = TRUE;
		fAllowRollButton = FALSE;
		fAllowDoneButton = FALSE;
		iSelectedRealm = REALM::NO_COUNTRY;
	}


	if (!iSelectedRealm)
	{
		y = startY = y-6;
		GoldBar = 0;
		RegencePoint = 0;
		fAllowCancelButton = TRUE;
		fAllowRollButton = FALSE;
		startProvCount = 1;

		for(i=0; i<=10; ++i)
			iSelectedHolding[i][0]=0;

		print_textf(x, startY+h-39, DKBROWN, STRMGR_GetStr(STR_DIPLOMACY_HELP_1));
		print_textf(x, startY+h-32, DKBROWN, STRMGR_GetStr(STR_DIPLOMACY_HELP_2));
		print_textf(x, startY+h-25, DKBROWN, STRMGR_GetStr(STR_DIPLOMACY_HELP_3));
		print_textf(x, startY+h-18, DKBROWN, STRMGR_GetStr(STR_DIPLOMACY_HELP_4));

		init_gfont(FONT_SANS_8PT);
		for (i=1; i<REALM::REALM_COUNT && x < (startX+290); ++i)
		{
			if (i==HomeRealm || !realm[i].mfExists() || i==REALM::MONSTERS || i==REALM::ANUIRE
				|| i==REALM::GORGONSCROWN || i==REALM::MURKILAD || i==REALM::MARKAZOR
				|| i==REALM::FIVEPEAKS || i==REALM::SPIDERFELL
				|| realm[i].mfGetRegent() <= 0
				|| (i >= LAND_REALM_COUNT && CheckVassal((REALM::REALM_TYPE)i) )
				|| (i==REALM::RHUOBHE && HomeRealm!=REALM::TUARHIEVEL && HomeRealm!=REALM::SIELWODE) )
				continue;

			// second column
			if ((y-startY)>(14*19) || (i >= LAND_REALM_COUNT && x == startX) )
			{
				if(x == startX+145)
					  continue;
				y = startY;
				x+=145;
			}

//          if ( x < startX + 290 )
//          {

			   DrawLittleButton(x, y, (x>startX)?160:130, 10, (iSelectedRealm==i));
			   DrawShield(x+2, y, (REALM::REALM_TYPE)i);
			   xs = gtext_width(realm[i].mfGetName());
			   print_textf(x+16, y, ReactionAndStatusColor(HomeRealm,i), "^F%02d%s", (xs>95)?21:2, realm[i].mfGetName());
			   if(ISATWAR(i, HomeRealm)||ISATWAR(HomeRealm, i))
					print_textf(x+((x>startX)?145:115), y+6, DKBROWN, "^c%d%c", ((RealmReaction[i][HomeRealm] - 25) > 0)?(RealmReaction[i][HomeRealm] - 25):0,'%');
   			else
	    			print_textf(x+((x>startX)?145:115), y+6, DKBROWN, "^c%d%c", RealmReaction[i][HomeRealm],'%');
		    	add_region(x, y-1, (x>startX)?160:130, 12, 0, SelectRealm,i,0,0, -1);
			   y+=14;
//          }
		}
	}

	// realm has been selected, now select subject
	else
	{
		y = startY = y-1;
 		fAllowCancelButton = TRUE;
		if(GoldBar == 0)
			if(ISATWAR(iSelectedRealm, HomeRealm)||ISATWAR(HomeRealm, iSelectedRealm))
				reaction = ((RealmReaction[iSelectedRealm][HomeRealm]- 25) > 0)?(RealmReaction[iSelectedRealm][HomeRealm] - 25):0;
			else
				reaction = RealmReaction[iSelectedRealm][HomeRealm];

		DrawLittleButton(x, y-1, 145, 12, FALSE);
		DrawShield(x+2, y, (REALM::REALM_TYPE)iSelectedRealm);
 		
 		SHORT theirRegent = realm[iSelectedRealm].mfGetRegent();
 		
 		if (theirRegent > 0 && playerstats[theirRegent] != fERROR)
 		{
			DumbAutoLockPtr< PLAYER_STATS > const pPS(playerstats[realm[iSelectedRealm].mfGetRegent()]);
 	   		yourAlign = pPS->mfGetAlignment();
 		}

		gprint_text(x+16, y, realm[iSelectedRealm].mfGetName(), RED);
		add_region(x, y, 145, 10, 0, SelectRealm,iSelectedRealm,0,0, -1);

		if (theirRegent > 0)
		{
			char RegentName[80];
			strcpy(RegentName, regents[theirRegent].mfGetname());
			print_textf(x+REGENTNAME, y+4, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_REGENT_FMT), RegentName);
		}
		y+=18;

		if(iNewScreen==0)
		{
			crease(x, y-2, 310, 1);
			print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_CAPS_PETITIONS));
			y+=10;
			fAllowRollButton = FALSE;
			GoldBar = 1; RegencePoint = 1;
			GoldGained = 0;
			alignMod = ((align[yourAlign][myAlign]>50)?(align[yourAlign][myAlign]-100):align[yourAlign][myAlign]);
  			curReact = reaction + alignMod + relicMod;

			// zero the values when the check box is deselected
			if (!iSelectedHolding[3][3]) iSelectedHolding[3][0] = 0;
			if (!iSelectedHolding[6][3]) iSelectedHolding[6][0] = 0;
			if (!iSelectedHolding[9][3]) iSelectedHolding[9][0] = 0;
			if (!iSelectedHolding[10][3]) iSelectedHolding[10][0] = 0;

			for (i=0; i<NUMDIPSUBJ; ++i)
			{
				modifyMod = 0;
				init_gfont(FONT_SANS_8PT);
				f = ( (iSelectedRealm<LAND_REALM_COUNT && RealmStatus[HomeRealm][iSelectedRealm]==STATUS_VASSAL && i==2)		// no vassalage to liege lords
						|| (i==2&&CheckVassal((REALM::REALM_TYPE)iSelectedRealm))
						|| (iSelectedHolding[6][3]&&i==9)		// no tribute for tribute
						|| (iSelectedHolding[9][3]&&i==6)
						|| (iSelectedHolding[3][3]&&i==10)		// no gold for gold
						|| (iSelectedHolding[10][3]&&i==3)
						|| (iSelectedRealm >= LAND_REALM_COUNT && i==4)
						|| (iSelectedRealm >= LAND_REALM_COUNT && i==7)	 // don't offer territory to no land realm
			 			|| (((fControlMode & 0xFF) == 0) && (i == 9 || i == 6) )
					);
				print_textf(x+30, y, (f)?DKTAN-4:DKBROWN, STRMGR_GetStr(gsDiplomacySubj[(typAlliance==3&&i==9)?22:i*2]));
				
				// no tribute in simple games
			 	if ( (((fControlMode & 0xFF) != 0) || (i != 9 && i != 6))
				 	  && ( (cntUnits_() < (MAX_UNITS-10)) || i != 2)	)
			 	{
					DrawBitmap(x, (SHORT)y, iCheck[iSelectedHolding[i][3]], 0, 0, 20, 20);
					if (!f)
						add_region(x, y, 30,12,0,SelectHolding,i,3,0, -1);
				}

				// selections with adjustments
				if (iSelectedHolding[i][3] && (i==3 || i==6 || i==9 || i==10))
				{
					#ifdef _FOREIGNVER
						print_textf(x+200, y+3, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_SPACES_GB_FMT),(i==3)?STRMGR_GetStr(STR_ACTNMENU_REQUEST):(i==6)?STRMGR_GetStr(STR_ACTNMENU_DEMAND):STRMGR_GetStr(STR_ACTNMENU_OFFER));
					#else
						print_textf(x+220, y+3, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_SPACES_GB_FMT),(i==3)?STRMGR_GetStr(STR_ACTNMENU_REQUEST):(i==6)?STRMGR_GetStr(STR_ACTNMENU_DEMAND):STRMGR_GetStr(STR_ACTNMENU_OFFER));
					#endif
					if (i==3 || i==6)
					{
						if (i==3) theirCurGold = (realm[iSelectedRealm].mfGetTreasury()/10) - iSelectedHolding[6][0];
						if (i==6) theirCurGold = (realm[iSelectedRealm].mfGetTreasury()/10) - iSelectedHolding[3][0];
						SetupMod(x+258, y, &iSelectedHolding[i][0], 0, (MIN(theirCurGold,20)), 1);
						GoldGained += iSelectedHolding[i][0];
					}
					else			// i==9 || i==10
					{
						//GEH SetupMod(x+258, y, &iSelectedHolding[i][0], 0, 20, 1);
						SetupMod(x+258, y, &iSelectedHolding[i][0], 0, 45, 1);
						GoldBar += iSelectedHolding[i][0];
					}
				}

				// goes to a new menu which lists provinces and holdings
				else if(iSelectedHolding[i][3] && (i==4||i==5||i==7||i==8) )
				{
					if(iSelection[i] == 0)
					{
						// goes to a new menu which lists provinces and holdings
						iNewScreen = i;
					}
					else
					{
						//iSelection[4] demand territory, province index
						//iSelection[5] demand holding, province index
						//iSelection[3] demand holding, place offset
						//iSelection[7] offer territory, province index
						//iSelection[8] offer holding, province index
						//iSelection[6] offer holding, place offset

						// print the province and holdings selected
						if (i==5 || i == 8)
						{
							if(iSelection[i]== iSelection[i-2])
								tmpIndex = province[iSelection[i]].FirstPlace;
							else
								tmpIndex = province[iSelection[i]].FirstPlace + iSelection[i-2];
							for(j=0; j<6; ++j)
								if(places[tmpIndex].Icon == InfoReqType[j])
									break;
							tmpLevel = places[tmpIndex].Level;
							print_textf(x, y, DKBLUE, "^F02^l300%s - %s %d", province[iSelection[i]].name, STRMGR_GetStr(gsAllHold[j]), tmpLevel);
						}
						if (i==4 || i == 7)
						{
							print_textf(x, y, DKBLUE, "^F02^l300%s", province[iSelection[i]].name);
							if(iSelection[i] != 0)
							{
								// check wiztower, port, seaport
								CheckAndFixPlaces((PROVINCE)iSelection[i]);
								j = index = province[iSelection[i]].FirstPlace;
								tmp = 0;
								while (index != 0)
								{
									if((places[j].Realm == iSelectedRealm && i == 4) 
									  || (places[j].Realm == HomeRealm && i == 7))
									{
										if(places[j].Icon == WIZTOWER_ICON || places[j].Icon == PORT_ICON || places[j].Icon == SEAPORT_ICON )
											++tmp;
									}
									index = places[j].NextPlace;
									j = province[iSelection[i]].FirstPlace + index;
								}
								if(i==4)
									modifyMod = -tmp * 20;
								else modifyMod = tmp*20;

								// check the province level and troops
								if(i == 4)
								{
									troopValue = CalcMultUnitsValue((PROVINCE)iSelection[i], (REALM::REALM_TYPE)iSelectedRealm);
									modifyMod -= troopValue * 2 + province[iSelection[i]].CivLevel * 10;
								}
								else 
								{
									troopValue = CalcMultUnitsValue((PROVINCE)iSelection[i], HomeRealm);
									modifyMod += troopValue * 1 + province[iSelection[i]].CivLevel * 5;
								}

							}
						}
					}
				}
				else
				{
					if (i==1)		// full
						print_textf(x+220, y, (f)?DKTAN-4:DKBROWN, STRMGR_GetStr(gsDiplomacySubj[i*2+1]),FullNeeds((REALM::REALM_TYPE)iSelectedRealm),'%');
					else if (i==2)		// oath
						print_textf(x+220, y, (f)?DKTAN-4:DKBROWN, STRMGR_GetStr(gsDiplomacySubj[i*2+1]),OathNeeds((REALM::REALM_TYPE)iSelectedRealm),'%');
					else
						print_textf(x+220, y, (f)?DKTAN-4:DKBROWN, STRMGR_GetStr(gsDiplomacySubj[i*2+1]),'%');
				}

				y+=13;

				if (i==3)		// end of petitions
				{
					y+=2;
					crease(x, y-2, 310, 1);
					print_textf(x, y, DKBROWN,STRMGR_GetStr(STR_ACTNMENU_DEMANDS));
					y+=10;
				}
				if (i==6)		// end of demands
				{
					y+=2;
					crease(x, y-2, 310, 1);
					print_textf(x, y, DKBROWN,STRMGR_GetStr(STR_ACTNMENU_OFFERS));
					y+=10;
				}

				// make alliances radio buttons
				// only select one at a time
				if (typAlliance == 1 && (iSelectedHolding[1][3] || iSelectedHolding[2][3]))
					iSelectedHolding[0][3] = FALSE;
				if (typAlliance == 2 && (iSelectedHolding[0][3] || iSelectedHolding[2][3]))
					iSelectedHolding[1][3] = FALSE;
				if (typAlliance == 3 && (iSelectedHolding[0][3] || iSelectedHolding[1][3]))
					iSelectedHolding[2][3] = FALSE;
				if (iSelectedHolding[0][3]) typAlliance = 1;
				else if (iSelectedHolding[1][3]) typAlliance = 2;
				else if (iSelectedHolding[2][3]) typAlliance = 3;
				else typAlliance = 0;

				if (iSelectedHolding[i][3])
				{
					if (i==1)			// full
						minReact = MAX(FullNeeds((REALM::REALM_TYPE)iSelectedRealm), minReact);
					else if (i==2)		// oath
						minReact = MAX(OathNeeds((REALM::REALM_TYPE)iSelectedRealm), minReact);
					else					// all others
						minReact = MAX(MinReaction[i], minReact);

					if (i==3 || i==6 || i==9 || i==10)
					{
						LONG	first, second;
						//GEH mod = Modifiers[i] * iSelectedHolding[i][0];
						first = iSelectedHolding[i][0];
						second = 0;
						if ( i==9 || i==10 )
						{
							if ( first > 15 )
							{
								second = first-15;
								first  = 15;
							}
						}
						mod = (first * Modifiers[i]) + (second * (Modifiers[i]/2)) ;
					}
					else if (i==5 || i==8)
						mod = Modifiers[i] * (MAX(tmpLevel,1));
					else
						mod = Modifiers[i];
					mod += modifyMod;
					if (Modifiers[i] > 0)
					{
						offers += mod;
					}
					else
					{
						demands += mod;
					}
					curReact += mod;
					fAllowRollButton = TRUE;
				}
			}
			crease(x, y, 310, 1);

			y+=2;
			tmp = reaction + alignMod;
			if(tmp < 10)
			{
				// Keep SlectRealm region active, delete all the others. If you have a better way to do it, please change the code. lan
				del_region(SelectProvince,0);
				del_region(SelectHolding,0);
				del_region(IncNum,0);
				del_region(DecNum,0);
				del_region(IncNumWithSign,0);
				del_region(DecNumWithSign,0);
				del_region(SetNum,0);
				del_region(SelectOneHolding,0);
				del_region(SelectMulProvince, 0);
				del_region(InitBattleCard_alt, 0);
				del_region(SelectUnit, 0);
				del_region(SelectMulUnits, 0);
				del_region(Selection, 0);
//				DeleteActnMenuRegions();
//				add_region(x, y, 145, 10, 0, SelectRealm,iSelectedRealm,0,0, -1);
				
				{
					char RegentName[80];
					if (realm[iSelectedRealm].mfGetRegent() > 0)
						strcpy(RegentName, regents[realm[iSelectedRealm].mfGetRegent()].mfGetname());
					else
						strcpy(RegentName, realm[iSelectedRealm].mfGetName());	
					print_textf(x, y, DKBLUE,STRMGR_GetStr(STR_ACTNMENU_AMBASSADOR_IN_BAG_FMT) ,
						RegentName, reaction,'%',alignMod,'%');
				}
			}
			else
			{
				char ReactionBuf[40];
				char FormatBuf[512];
				char RegentName[80];
				SHORT rmod = CourtEffect[realm[HomeRealm].mfGetCourt()];
				strcpy(ReactionBuf, (rmod < 0) ?
					STRMGR_GetStr(STR_ACTN_IMPROVES) :
					STRMGR_GetStr(STR_ACTN_REDUCES)    );
				rmod = ABS(rmod);	
				
				strcpy(FormatBuf, STRMGR_GetStr(STR_ACTNMENU_MODIFIED_REACTION_FMT));
				
				if (realm[iSelectedRealm].mfGetRegent() > 0)
					strcpy(RegentName,regents[realm[iSelectedRealm].mfGetRegent()].mfGetname());
				else	
					strcpy(RegentName,realm[iSelectedRealm].mfGetName());
				print_textf(x, y, DKBLUE, FormatBuf,
					RegentName,
					reaction+relicMod,'%',alignMod,'%',offers,'%',-demands,'%',curReact,'%',
					STRMGR_GetStr(gsCourt[realm[HomeRealm].mfGetCourt()]),
					ReactionBuf,
					(mundane_descriptions ? rmod * 5 : rmod),
					PERCENTCHAR );
			}
			
			y+=54;
			// if the two realms are at war, the curreact must be at least higher than their original reaction to each other before it can roll
			if((ISATWAR(iSelectedRealm, HomeRealm)||ISATWAR(HomeRealm, iSelectedRealm))
					&& minReact< RealmReaction[iSelectedRealm][HomeRealm])
						minReact = RealmReaction[iSelectedRealm][HomeRealm];
  			if(checkGoldReg)
				fAllowRollButton = (ProcessGoldAndReg(CHECK_QUITE))?fAllowRollButton:0;
	   	else
				fAllowRollButton = (ProcessGoldAndReg(CHECK_ONLY))?fAllowRollButton:0;
	
			
			if (curReact < minReact && !fAllowDoneButton && tmp >= 10
#ifdef _WINDOWS
			   && !(IsMultiPlayer() && realm[iSelectedRealm].mfIsPlayerCtrl())
#endif
			)
			{
				print_textf(x, y, DKRED,STRMGR_GetStr(STR_ACTNMENU_NOT_CONSIDERED_FMT) ,(minReact==50)?STRMGR_GetStr(STR_ACTNMENU_LOWCASE_DEMANDS):STRMGR_GetStr(STR_ACTNMENU_LOWCASE_PETITION),minReact,'%');
				fAllowRollButton = FALSE;
			}
			else if(fAllowRollButton || fAllowDoneButton)
			{
				#ifdef _FOREIGNVER
					y+=10;
				#endif
				crease(x, y, 310, 1);
				y+=2;
				
				success = 20 - (curReact/5) - modRegency + CourtEffect[realm[HomeRealm].mfGetCourt()] + modOpponent;
				if (success > 20) success = 21;
				if (success < 1) success = 1;
				
				print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_F02_SUCCESS_FMT), D20TOPERCENT(success),PERCENTCHAR);
				print_textf(x+150, y-1, DKBROWN, STRMGR_GetStr(STR_MAPAI_MODIFY_SUCCESS));
				print_textf(x+150, y+5, DKBROWN, STRMGR_GetStr(STR_MAPAI_USING_REGENCY));
				// regency to modify
				if (fLTAction != DURING_LTACTION)
				{
					SetupMod(x+230, y, &modRegency, 0, 19-(curReact/5)+CourtEffect[realm[HomeRealm].mfGetCourt()]+modOpponent, 1);
					RegencePoint += modRegency;
				}

				// multi-player and AI support
				if(lAskedTargetWhichRoll == iThisRoll + 1)
				{

					//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					// trying for a non-landed vassal
					//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					#ifdef _WINDOWS
					if ( IsMultiPlayer() )
					{
						if (typAlliance == 3 && iSelectedRealm >= LAND_REALM_COUNT)
						{
							AMultiPlayer.LookForMessages();

							// check for already taken
							if (CheckVassal((REALM::REALM_TYPE)iSelectedRealm) && !ISVASSAL(iSelectedRealm, HomeRealm) )
								goto DiplomacyRestart;

#if 0
							AMultiPlayer.LookForMessages();

							// check lockout for 45 seconds
							time = get_time() + (45 * 18);
							while ( regents[0].mfGetunit() != 0 )
							{
								AMultiPlayer.LookForMessages();
								Sleep(1);
								if (get_time() > time)
									goto DiplomacyRestart;
							}

							// set lockout
							SetGameData(MP_REGENT, MPREG_UNIT, 0, HomeRealm, TRUE );

							AMultiPlayer.LookForMessages();

							// check lockout for 4 seconds
							time = get_time() + (4 * 18);
							while ( get_time() < time )
							{
								if ( regents[0].mfGetunit() != HomeRealm )
								{
									SetGameData(MP_REGENT, MPREG_UNIT, 0, 0, TRUE );
									goto DiplomacyRestart;
								}
								AMultiPlayer.LookForMessages();
								Sleep(1);
							}

							// check again for already taken. if so, clear lockout
							if (CheckVassal((REALM::REALM_TYPE)iSelectedRealm) && !ISVASSAL(iSelectedRealm, HomeRealm) )
							{
								SetGameData(MP_REGENT, MPREG_UNIT, 0, 0, TRUE );
								goto DiplomacyRestart;
							}
#endif
							// sieze lieutenant
							SetGameData(MP_REALMSTATUS, iSelectedRealm, HomeRealm, STATUS_VASSAL, TRUE );

							AMultiPlayer.LookForMessages();

							// check for 7 seconds
							time = get_time() + (7 * 18);
							while ( get_time() < time )
							{
								if ( !ISVASSAL(iSelectedRealm, HomeRealm) )
								{
//									SetGameData(MP_REGENT, MPREG_UNIT, 0, 0, TRUE );
									goto DiplomacyRestart;
								}
								AMultiPlayer.LookForMessages();
								Sleep(1);
							}

							// clear lockout
//							SetGameData(MP_REGENT, MPREG_UNIT, 0, 0, TRUE );

						}
					}
					#endif

					if (iSelection[3])
					{
						if(iSelection[3] == iSelection[5])
							dData.d1.iReqHold	= province[iSelection[5]].FirstPlace;
						else
							dData.d1.iReqHold = province[iSelection[5]].FirstPlace+iSelection[3];
					}
					else
					{
						dData.d1.iReqHold = 0;
					}

					// -------------------------------------------------------
					//	iSelection[6] offer holding, place offset
					//	iSelection[8] offer holding, province index
					// -------------------------------------------------------
					if (iSelection[6])
					{
						if(iSelection[6]==iSelection[8])
							dData.d1.iOffHold  =  province[iSelection[8]].FirstPlace;
						else
							dData.d1.iOffHold  =  province[iSelection[8]].FirstPlace+iSelection[6];
					}
					else
					{
						dData.d1.iOffHold = 0;
					}
					iGlobRealm = iSelectedRealm;
					dData.d1.iWhoFrom	= HomeRealm;
					dData.d1.iAlliance	= typAlliance;
					dData.d1.iReqGold	= iSelectedHolding[3][3]?iSelectedHolding[3][0]:0;
					dData.d1.iReqProv	= iSelection[4];
					dData.d1.iReqTrib	= iSelectedHolding[6][3]?iSelectedHolding[6][0]:0;
					dData.d1.iOffProv	= iSelection[7];
					dData.d1.iOffTrib	= iSelectedHolding[9][3]?iSelectedHolding[9][0]:0;
					dData.d1.iOffGold	= iSelectedHolding[10][3]?iSelectedHolding[10][0]:0;
					dData.d1.iOffRegy	= RegencePoint;

					answer = AskAndWaitForTargetPlayer(DIPLOMACY, dData.d2.data1, dData.d2.data2, dData.d2.data3, x+230, y);

					if(answer == 1)	// success
						success = 1;

					if(answer == 2)	// fail
						success = 21;

					if(answer == 3)	// redo deal
					{
						// put up question box saying so-and-so is interested,
						// but needs a better deal
						DoDealMenu(iSelectedRealm);

						iWhichRoll = 99;
						fAllowCancelButton = TRUE;
						fAllowRollButton = TRUE;
						fAllowDoneButton = FALSE;
						lAskedTargetWhichRoll = iWhichRoll;
					}
				}

				result = RollSuccess(success, x+230, y, TRUE);
				if (result == 1)	// success
				{
					LONG goldGiven    = GoldBar - 1;
					LONG goldReceived = GoldGained;
					
					GoldBar = 1;
					GoldGained = 0;
					
					SyncRegencySpentInOpposition((REALM::REALM_TYPE)iGlobRealm);

					// we have already "given" the gold in another place
					realm[HomeRealm].mfAddTreasury ((goldReceived) * 10);
					realm[iGlobRealm].mfAddTreasury((goldGiven - goldReceived) * 10);
					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						char temp[200];
						char HomeRealmName[50];
						
						strcpy(HomeRealmName, realm[HomeRealm].mfGetName());
						sprintf ( temp, "PaintDiplomacy - HomeRealm:%s  newtreasury: %04d\nTargetRealm:%s  newtreasury: %04d",
									HomeRealmName, 
									realm[HomeRealm].mfGetTreasury(),
									realm[iGlobRealm].mfGetName(),
									realm[iGlobRealm].mfGetTreasury());
						RandomLogComment ( temp );
					}
					SetGameData(MP_REALM, MPRLM_TREASURY, HomeRealm, realm[HomeRealm].mfGetTreasury(), TRUE);
					SetGameData(MP_REALM, MPRLM_TREASURY, iGlobRealm, realm[iGlobRealm].mfGetTreasury(), TRUE);

					// -------------------------------------------------------
					//	iSelection[4] demand territory, province index
					// -------------------------------------------------------
					if (iSelection[4])
					{
						for (j = 1; j < MAX_UNITS; ++j)
						{
							if (units[j].Icon < FIRST_CHARACTER_TYPE 
									&& units[j].province == iSelection[4] 
									&& units[j].Realm == province[iSelection[4]].Realm
									&& units[j].Joined == 0)
							{
								for (iUnit = j; iUnit > 0; iUnit = units[iUnit].NextUnit)
								{
									units[iUnit].Realm = HomeRealm;
									SetGameData(MP_UNITS, MPUNITS_REALM, iUnit, HomeRealm, TRUE );
							 	}
							}

							// units leader is a lieutenant.
							else if (units[j].Icon >= FIRST_CHARACTER_TYPE
									&& units[j].province == iSelection[4] 
									&& units[j].Realm == province[iSelection[4]].Realm
									&& units[j].Joined == 0)
							{
								for(iUnit = j; iUnit > 0; iUnit = units[iUnit].NextUnit)
								{
									if(units[j].Icon < FIRST_CHARACTER_TYPE)
									{
										units[iUnit].Realm = HomeRealm;
										SetGameData(MP_UNITS, MPUNITS_REALM, iUnit, HomeRealm, TRUE );
									}
							 	}
							}
						}

						investor = HomeRealm;
						victim = iSelectedRealm;
						target_province = iSelection[4];
						province[iSelection[4]].Realm = HomeRealm;
						SetGameData(MP_PROVINCE, MPPROV_REALM, iSelection[4], HomeRealm, TRUE );
					}

					// -------------------------------------------------------
					//	iSelection[7] offer territory, province index
					// -------------------------------------------------------
					if (iSelection[7])
					{
						for(j = 1; j < MAX_UNITS; ++j)
						{
							if (units[j].Icon < FIRST_CHARACTER_TYPE 
								&& units[j].province == iSelection[7] 
								&& units[j].Realm == HomeRealm 
								&& units[j].Joined == 0)
							{
								for (iUnit = j; iUnit > 0; iUnit = units[iUnit].NextUnit)
								{
									units[iUnit].Realm = iSelectedRealm;
									SetGameData(MP_UNITS, MPUNITS_REALM, iUnit, iSelectedRealm, TRUE );
							 	}
							}

							// units leader is a lieutenant.
							else if (units[j].Icon >= FIRST_CHARACTER_TYPE
									&& units[j].province == iSelection[7] 
									&& units[j].Realm == HomeRealm
									&& units[j].Joined == 0)
							{
								for(iUnit = j; iUnit > 0; iUnit = units[iUnit].NextUnit)
								{
									if(units[j].Icon < FIRST_CHARACTER_TYPE)
									{
										units[iUnit].Realm = iSelectedRealm;
										SetGameData(MP_UNITS, MPUNITS_REALM, iUnit, iSelectedRealm, TRUE );
									}
							 	}
							}
						}

						investor = iSelectedRealm;
						victim = HomeRealm;
						target_province = iSelection[7];
						province[iSelection[7]].Realm = iSelectedRealm;
						SetGameData(MP_PROVINCE, MPPROV_REALM, iSelection[7], iSelectedRealm, TRUE );
					}

					// -------------------------------------------------------
					//	iSelection[3] demand holding, place offset
					//	iSelection[5] demand holding, province index
					// -------------------------------------------------------
					if (iSelection[3])
					{
						if(iSelection[3] == iSelection[5])
						{
							//places[province[iSelection[5]].FirstPlace].Realm = HomeRealm;
							//SetGameData(MP_PLACES, MPPLACE_REALM, province[iSelection[5]].FirstPlace, HomeRealm, TRUE );
							if(iSelection[6])
								if(iSelection[6]==iSelection[8])
									CheckHoldingConflict(province[iSelection[5]].FirstPlace, province[iSelection[8]].FirstPlace, TRUE);	 
								else
									CheckHoldingConflict(province[iSelection[5]].FirstPlace, province[iSelection[8]].FirstPlace+iSelection[6], TRUE);	 
							else		  
								CheckHolding(province[iSelection[5]].FirstPlace, HomeRealm, TRUE);
						}
						else
						{
							//places[province[iSelection[5]].FirstPlace+iSelection[3]].Realm = HomeRealm;
							//SetGameData(MP_PLACES, MPPLACE_REALM, province[iSelection[5]].FirstPlace+iSelection[3], HomeRealm, TRUE );
			   				if(iSelection[6])
								if(iSelection[6]==iSelection[8])
									CheckHoldingConflict(province[iSelection[5]].FirstPlace+iSelection[3], province[iSelection[8]].FirstPlace, TRUE);	 
								else
									CheckHoldingConflict(province[iSelection[5]].FirstPlace+iSelection[3], province[iSelection[8]].FirstPlace+iSelection[6], TRUE);	 
							else		  
								CheckHolding((province[iSelection[5]].FirstPlace+iSelection[3]), HomeRealm, TRUE);
						}
					}

					// -------------------------------------------------------
					//	iSelection[6] offer holding, place offset
					//	iSelection[8] offer holding, province index
					// -------------------------------------------------------
					if (iSelection[6])
					{
						if(iSelection[6]==iSelection[8])
						{
							//places[province[iSelection[8]].FirstPlace].Realm = iSelectedRealm;
							//SetGameData(MP_PLACES, MPPLACE_REALM, province[iSelection[8]].FirstPlace, iSelectedRealm, TRUE );
							if(!(iSelection[3]))
								CheckHolding(province[iSelection[8]].FirstPlace, (REALM::REALM_TYPE)iSelectedRealm, TRUE);
						}
						else
						{
							//places[province[iSelection[8]].FirstPlace+iSelection[6]].Realm = iSelectedRealm;
							//SetGameData(MP_PLACES, MPPLACE_REALM, province[iSelection[8]].FirstPlace+iSelection[6], iSelectedRealm, TRUE );
							if(!(iSelection[3]))
								CheckHolding((province[iSelection[8]].FirstPlace+iSelection[6]), (REALM::REALM_TYPE)iSelectedRealm, TRUE);
						}
					}

					// -------------------------------------------------------
					// set alliances
					// -------------------------------------------------------
					if (typAlliance == 1)
					{
						// give experience points
						ActionExpPoints(ActiveRegent, RLM_EXP_DIPLOMACY_PERMISSIVE, 0, TRUE );
						RealmStatus[iSelectedRealm][HomeRealm] = STATUS_PERMISSIVE_ALLIANCE;
						SetGameData(MP_REALMSTATUS, iSelectedRealm, HomeRealm, STATUS_PERMISSIVE_ALLIANCE, TRUE );
						if (iSelectedRealm<LAND_REALM_COUNT)
						{
							RealmStatus[HomeRealm][iSelectedRealm] = STATUS_PERMISSIVE_ALLIANCE;
							SetGameData(MP_REALMSTATUS, HomeRealm, iSelectedRealm, STATUS_PERMISSIVE_ALLIANCE, TRUE );
						}
					}
					else if (typAlliance == 2)
					{
						// give experience points
						ActionExpPoints(ActiveRegent, RLM_EXP_DIPLOMACY_FULL, 0, TRUE );
						RealmStatus[iSelectedRealm][HomeRealm] = STATUS_FULL_ALLIANCE;
						SetGameData(MP_REALMSTATUS, iSelectedRealm, HomeRealm, STATUS_FULL_ALLIANCE, TRUE );
						if (iSelectedRealm<LAND_REALM_COUNT)
						{
							RealmStatus[HomeRealm][iSelectedRealm] = STATUS_FULL_ALLIANCE;
							SetGameData(MP_REALMSTATUS, HomeRealm, iSelectedRealm, STATUS_FULL_ALLIANCE, TRUE );
						}
					}
					else if (typAlliance == 3)
					{
						// give experience points
						ActionExpPoints(ActiveRegent, RLM_EXP_DIPLOMACY_VASSALAGE, 0, TRUE );
						RealmStatus[iSelectedRealm][HomeRealm] = STATUS_VASSAL;
						SetGameData(MP_REALMSTATUS, iSelectedRealm, HomeRealm, STATUS_VASSAL, TRUE );
						{
							// You can see your vassal's units.
							LONG iUnit;
							for (iUnit = 1; iUnit< MAX_UNITS; ++iUnit)
							{
								if (units[iUnit].province != NO_PROVINCE	// In use.
								    && units[iUnit].Realm == iSelectedRealm
								    && units[iUnit].Joined == 0)
								{
									LONG kUnit;
									for (kUnit = iUnit; kUnit > 0; kUnit = units[kUnit].NextUnit)
									{
										units[kUnit].NotSpotted = FALSE;
									}
								}
							}
						}
						
						if (iSelectedRealm<LAND_REALM_COUNT)
						{
							RealmStatus[HomeRealm][iSelectedRealm] = STATUS_FULL_ALLIANCE;
							SetGameData(MP_REALMSTATUS, HomeRealm, iSelectedRealm, STATUS_FULL_ALLIANCE, TRUE );

							#ifdef _WINDOWS
							if (!IsMultiPlayer())
							#endif
							{
								// Make vassal player controlled if not multiplayer
								realm[iSelectedRealm].mfSetPlayerCtrl(1);
								realm[iSelectedRealm].mfSetDMCtrl(0);
							}
						}
						else		// non-landed vassal becomes lieutenant instead
						{
							iSelectedProvince = (PROVINCE) units[regents[realm[HomeRealm].mfGetRegent()].mfGetunit()].province;
							for (j=69; j<CHARACTER_COUNT; ++j)			// scan all regents
						   	if (regents[j].mfGetRealm() == iSelectedRealm)
									iSelectedOneHolding = j;
						 	j = CreateUnit(iSelectedProvince, regents[iSelectedOneHolding].mfGeticon(), (LONG) regents[iSelectedOneHolding].mfGetid(), HomeRealm, TRUE );
							if (j != fERROR)
							{
								if (regents[iSelectedOneHolding].mfGetRealm() >= LAND_REALM_COUNT
								   	|| regents[iSelectedOneHolding].mfGetRealm() == HomeRealm)
								{
									for (k=0; k<MAX_PLACES; ++k)
									{
										if(places[k].Realm == regents[iSelectedOneHolding].mfGetRealm())
										{
											//places[k].Realm = HomeRealm;
											CheckHolding(k, HomeRealm, TRUE);
										}
									}
								}
								//	regents[iSelectedOneHolding].mfGetunit() = j;
								SetGameData(MP_REGENT, MPREG_UNIT, iSelectedOneHolding, j, TRUE );

								// set his maintenance
								if (iSelectedHolding[9][3])			// offer tribute
								{
									units[j].Salary = Tribute[HomeRealm][iSelectedRealm] + iSelectedHolding[9][0];
									SetGameData(MP_UNITS, MPUNITS_SALARY, j, iSelectedHolding[9][0], TRUE );
									// No more tribute
									LONG rlm;
									for (rlm = 0; rlm < REALM::REALM_COUNT; rlm++)
									{
										SetGameData(MP_TRIBUTE, rlm, iSelectedRealm, 0, TRUE);
										SetGameData(MP_TRIBUTE, iSelectedRealm, rlm, 0, TRUE);
									}
									//iSelectedHolding[9][0] = 0	// visual bug
								}

							}
						}
					}

					// set new reaction
					// if we are forming an alliance the reactions sticks
					if (typAlliance != 0 && curReact>0)
					{
						RealmReaction[iSelectedRealm][HomeRealm] = (curReact>150)?150:curReact;
						if(ISATWAR(iSelectedRealm, HomeRealm)||ISATWAR(HomeRealm, iSelectedRealm))
						{
							//RealmStatus[iSelectedRealm][HomeRealm] = STATUS_NEUTRAL;
							SetGameData(MP_REALMSTATUS, iSelectedRealm, HomeRealm, STATUS_NEUTRAL, TRUE );
						}
						SetGameData(MP_REALMREACTION, iSelectedRealm, HomeRealm, RealmReaction[iSelectedRealm][HomeRealm], TRUE);
						if(iSelectedRealm < LAND_REALM_COUNT)
						{
							if(ISATWAR(iSelectedRealm, HomeRealm)||ISATWAR(HomeRealm, iSelectedRealm))
							{
								//RealmStatus[HomeRealm][iSelectedRealm] = STATUS_NEUTRAL;
								SetGameData(MP_REALMSTATUS, HomeRealm, iSelectedRealm,STATUS_NEUTRAL, TRUE );
							}
							if(typAlliance == 3)
								RealmReaction[HomeRealm][iSelectedRealm] = (curReact>95)?95:curReact;
							else
								RealmReaction[HomeRealm][iSelectedRealm] = (curReact>150)?150:curReact;
							SetGameData(MP_REALMREACTION, HomeRealm, iSelectedRealm, RealmReaction[HomeRealm][iSelectedRealm], TRUE);
						}
					}

					// if we are screwing someone with diplomacy
					else if (curReact < RealmReaction[iSelectedRealm][HomeRealm])
					{
						RealmReaction[iSelectedRealm][HomeRealm] = (curReact>0)?curReact:0;
					}

					// Tribute or Salary
					if (iSelectedHolding[6][3])		// demand tribute
					{
						Tribute[iSelectedRealm][HomeRealm] += iSelectedHolding[6][0];
						SetGameData(MP_TRIBUTE, iSelectedRealm, HomeRealm, Tribute[iSelectedRealm][HomeRealm], TRUE);
					}
					if (iSelectedHolding[9][3]			// offer tribute
						&& (typAlliance != 3 || iSelectedRealm<LAND_REALM_COUNT) )
					{
						Tribute[HomeRealm][iSelectedRealm] += iSelectedHolding[9][0];
						SetGameData(MP_TRIBUTE, HomeRealm, iSelectedRealm, Tribute[HomeRealm][iSelectedRealm], TRUE);
					}

					// reconstruct the map
					if (iSelection[4] || iSelection[7])
					{
						TickDelay(20);
						//ReconstructMap();
						//CheckRealmStatus(investor,victim, TRUE );
						SetGameData(MP_PROVINCE, MPPROV_INVESTED, investor, victim, TRUE );

						if (realm[HomeRealm].mfExists() == FALSE)	// gave away the farm
						{
							CleanupAfterActnMenu();
							return;
						}
					}
				}

				if (result == 2)	// failure
				{
					LONG goldRefunded = GoldBar - 1;
					realm[HomeRealm].mfAddTreasury ((goldRefunded) * 10);
					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						char temp[100];
						sprintf ( temp, "PaintDiplomacy - failed HomeRealm:%s  newtreasury: %04d\n",
									realm[HomeRealm].mfGetName(), 
									realm[HomeRealm].mfGetTreasury());
						RandomLogComment ( temp );
					}
					SetGameData(MP_REALM, MPRLM_TREASURY, HomeRealm, realm[HomeRealm].mfGetTreasury(), TRUE);
					
					// multi-player support
					SyncRegencySpentInOpposition((REALM::REALM_TYPE)iGlobRealm);
				}

				if (!mundane_descriptions)
				{
					y+=14;
					print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_HELP_20_SIDED));
				}
			}
	
			print_textf(x, startY+h-45, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_TOTAL_COST_FMT), GoldBar-GoldGained, RegencePoint);
			print_textf(x, startY+h-33, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_SELECT_ACTION));
		}

		// ---------------------------------------------------
		// auxiliary menus for choosing a province or holding
		// ---------------------------------------------------
		else		// iNewScreen == TRUE
		{
			fAllowCancelButton = TRUE;
			fAllowRollButton = FALSE;
			fAllowDoneButton = FALSE;
			print_textf(x+30, y, DKBROWN, STRMGR_GetStr(gsDiplomacySubj[iNewScreen*2]));
			DrawBitmap(x, (SHORT)y, iCheck[iSelectedHolding[iNewScreen][3]], 0, 0, 20, 20);
			add_region(x, y, 30,12,0,SelectHolding,iNewScreen,3,0, -1);
			print_textf(x+220, y, DKBROWN, STRMGR_GetStr(gsDiplomacySubj[iNewScreen*2+1]), '%');
			y += 20;
			for (int h1=0; h1<2; ++h1)
			{
				for(i = FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)
				{
					// check for castle 
					CheckAndFixPlaces((PROVINCE)i);
					j = index = province[i].FirstPlace;
					tmp = 0;
					while (index != 0)
					{
						if(((places[j].Realm == iSelectedRealm && iNewScreen == 4) 
						  || (places[j].Realm == HomeRealm && iNewScreen ==	7))&& places[j].Icon == CASTLE1_ICON)
							break;
						index = places[j].NextPlace;
						j = province[i].FirstPlace + index;
					}


					if(((iNewScreen == 4 &&(province[i].Realm == iSelectedRealm) )
					  ||(iNewScreen == 7 &&(province[i].Realm == HomeRealm))) && (index == 0))
					{
						if(h1)
						{
							++counter;
							if(counter < startProvCount || lPageDone)
								lPrint = FALSE;
							else
								lPrint = TRUE;

							if(lPrint)
							{
								DrawLittleButton(x, y-1, 145, 10, FALSE);
								DrawShield(x+2, y, (REALM::REALM_TYPE)province[i].Realm);
								gprint_text(x+16, y,province[i].name, LoyaltyColor[province[i].Loyalty]);
								if(iNewScreen == 4) 
								{
									troopValue = CalcMultUnitsValue((PROVINCE)i, (REALM::REALM_TYPE)iSelectedRealm);
									modMod  = Modifiers[iNewScreen] - troopValue * 2 - province[i].CivLevel * 10;
									print_textf(x+115, y, DKBROWN, "^F02%d%%", -modMod);
								}
								else
								{
									troopValue = CalcMultUnitsValue((PROVINCE)i, HomeRealm);
									modMod  = Modifiers[iNewScreen] + troopValue * 1 + province[i].CivLevel * 5;
									print_textf(x+115, y, DKBROWN, "^F02%d%%", modMod);
								}

								add_region(x, y, 145, 10, 0, Selection,i,iNewScreen,0, -1);
								y+= 18;

								// print prev, next buttons if provinces are more than 8		
								if(DoPageProvince(&startProvCount, provCount, 8, counter, x, y))
								{
									lPrint = FALSE;
									lPageDone = TRUE;
								}
							}
						}
						else
							++provCount;
					}

					else if(iNewScreen == 5 || iNewScreen == 8)
					{
				  		// look for province where the regent has a holding/holdings in
						CheckAndFixPlaces((PROVINCE)i);
						j = index = province[i].FirstPlace;
						while (index != 0)
						{
							if((places[j].Realm == iSelectedRealm && iNewScreen == 5)
							  || (places[j].Realm == HomeRealm && iNewScreen ==	8))
							{
								break;
							}
							index = places[j].NextPlace;
					  		j = province[i].FirstPlace + index;
						}
						if(index!=0 && h1)
						{
							++counter1;
							if(counter1 < startProvCount1 || lPageDone1)
								lPrint1 = FALSE;
							else
								lPrint1 = TRUE;

							if(iSelection[iNewScreen]!=i)
							{
								if(lPrint1)
									DrawLittleButton(x, y-1, 145, 10, FALSE);
							}

							// have chosen a province, now go to holdings inside the province
							else
							{
								if(lPrint1)
								{
									saveY = y;
									DrawLittleButton(x, y-1, 145, 10, TRUE);
									x += 160;
									y = startY+38;
									CheckAndFixPlaces((PROVINCE)i);
							  		j = province[i].FirstPlace;
									tmpIndex = i;
									while (tmpIndex != 0)
									{
										if((places[j].Realm == iSelectedRealm && iNewScreen == 5)
										  || (places[j].Realm == HomeRealm && iNewScreen ==	8))
										{
											// find the type of the place
											for(count=0; count<6; ++count)
												if(places[j].Icon == InfoReqType[count])
													break;
											if(count!=6)
											{
												DrawLittleButton(x, y-1, 145, 10, FALSE);
												DrawShield(x+2, y, (REALM::REALM_TYPE)places[j].Realm);
												print_textf(x+16, y, DKBROWN, "%s %d",STRMGR_GetStr(gsAllHold[count]),places[j].Level);
												add_region(x, y, 145, 10, 0, Selection,tmpIndex,iNewScreen-2,0, -1);
												y+= 18;
											}
					
										}
										tmpIndex = places[j].NextPlace;
								  		j = province[i].FirstPlace + tmpIndex;
									}
								}
							}

							if(lPrint1)
							{
								x = startX;
								y = (iSelection[iNewScreen]!=i)?y:saveY;
								DrawShield(x+2, y, (REALM::REALM_TYPE)province[i].Realm);
								gprint_text(x+16, y, province[i].name, LoyaltyColor[province[i].Loyalty]);
								add_region(x, y, 145, 10, 0, Selection,i,iNewScreen,0, -1);
								y+= 18;	

								// print prev, next buttons if provinces are more than 8		
								if(DoPageProvince(&startProvCount1, provCount1, 8, counter1, x, y))
								{
									lPrint1 = FALSE;
									lPageDone1 = TRUE;
								}
							}

						}
						else if(index != 0)
							++provCount1;
					}
				}
			}
		}
	}
	if (fAllowDoneButton || IsMenuActive(D_AVATAR_ATTRIBUTES2))
		DeleteActnMenuRegions();


//	fAllowCancelButton = TRUE;
//	fAllowRollButton = FALSE;
//	fAllowDoneButton = (iSelectedRealm != 0);
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
void PaintEspionage (LONG x, LONG y, LONG h)
{
	LONG  i, j, level, startY, startX, success, result, a_level, a_chance, iUnit;
	static LONG	assassin_roll;
	static LONG	delay;
	static LONG	sequence;
	static char	n[20];
	static LONG	t_level;
	static LONG	rlm;
	LONG		provCount = 0;
	LONG		counter = 0;
	LONG		provCount1 = 0;
	LONG		counter1 = 0;
	BOOL	CanScry = FALSE;

	if (playerstats[ActiveRegent] != fERROR)
	{
		DumbAutoLockPtr< PLAYER_STATS > const pPS(playerstats[ActiveRegent]);
		CanScry = (pPS->StatsMod.mfGetMod(STATSMOD::SEEHIDDEN_MOD) > 0);
	}
	level = 0;
	startY = y;
	startX = x;
	GoldBar = RegencePoint = 0;
	result = 0;

	fAllowRollButton = FALSE;

	del_region(NextPrev,0);

	// allow a realm to be selected
	if (iSelectedRealm==0)
	{
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_CAPS_REALMS));
		y+=12;

		init_gfont(FONT_SANS_8PT);
		for (i=1; i<LAND_REALM_COUNT; ++i)
		{
			if (!realm[i].mfExists() || i==REALM::MONSTERS)
				continue;

			if ((y-(startY+12)) >= (16*15))			// two columns
			{
				y = startY+12;
				x+=160;
			}

			DrawLittleButton(x, y-1, 145, 10, FALSE);
			DrawShield(x+2, y, (REALM::REALM_TYPE)i);
			gprint_text(x+16, y, realm[i].mfGetName(), DKBROWN);
			add_region(x, y, 145, 10, 0, SelectRealm,i,0,0, -1);
			y+=15;
		}
	}

	// print the realm button
	if (iSelectedRealm!=0)
	{
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_CAPS_REALM));
		y += 10;
		init_gfont(FONT_SANS_8PT);
		DrawLittleButton(x, y-1, 145, 12, FALSE);
		DrawShield(x+2, y, (REALM::REALM_TYPE)iSelectedRealm);
		gprint_text(x+16, y, realm[iSelectedRealm].mfGetName(), RED);
		add_region(x, y, 145, 12, 0, SelectRealm,iSelectedRealm,0,0, -1);
		if (iSelectedProvince==0)
			print_textf(x+160, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_TREASURY_FMT),
				realm[iSelectedRealm].mfGetTreasury(), realm[iSelectedRealm].mfGetSurplusIncome(), realm[iSelectedRealm].mfGetRegency());
		y+=16;
	}

	// realm has been selected, now select province
	if (iSelectedRealm!=0 && iSelectedProvince==0)
	{
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_F00_PROVINCES));
		y+=12;

		init_gfont(FONT_SANS_8PT);
		for (int h1=0; h1<2; ++h1)
		{
			for (i=FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)
			{
				if (province[i].Realm == iSelectedRealm&& province[i].Honest == 0)
				{
					if (h1)
					{
						++counter;
						if(counter < startProvCount)
							continue;
	
						DrawLittleButton(x, y-1, 145, 10, (iSelectedProvince==i));
						if (i==iSelectedProvince)
							gprint_text(x+16, y, province[i].name, RED);
						else
							gprint_text(x+16, y, province[i].name, DKBROWN);
		
						add_region(x, y, 145, 10, 0, SelectProvince, i, 0, 0, -1);
						y += 15;

						// print prev, next buttons if provinces are more than 15		
						if(DoPageProvince(&startProvCount, provCount, 15, counter, x, y))
							break;
					}
					else
						++provCount;
				}
			}
		}
		x = startX+160;
	}

	if (iSelectedRealm!=0 && iSelectedProvince!=0)
	{
		// province
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_F00_PROVINCES));
		y += 12;
		init_gfont(FONT_SANS_8PT);
		DrawLittleButton(x, y-1, 145, 12, FALSE);
		DrawShield(x+2, y, (REALM::REALM_TYPE)province[iSelectedProvince].Realm);
		gprint_text(x+16, y, province[iSelectedProvince].name, RED);
		add_region(x, y, 145, 12, 0, SelectProvince,iSelectedProvince,0,0, -1);
		y+=16;

		// find level of guild
		CheckAndFixPlaces(iSelectedProvince);
  		j = i = province[iSelectedProvince].FirstPlace;
		while (i != 0)
		{
			if(places[j].Icon==GUILD1_ICON&&places[j].Realm == HomeRealm)
			{
				level = places[j].Level;
				break;
			}
			i = places[j].NextPlace;
	  		j = province[iSelectedProvince].FirstPlace + i;
		}
	}

	// realm and prov selected, now select type
	if (iSelectedRealm!=0 && iSelectedProvince!=0)
	{
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_TYPES));
		y += 12;
		init_gfont(FONT_SANS_8PT);

		if(iSelectedOneHolding == 0)
		   DrawLittleButton(x, y-1, 145, 12, TRUE);
		else
			DrawLittleButton(x, y-1, 145, 12, FALSE);
		print_textf(x+6, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_SPY_ON_UNITS));
		add_region(x, y, 145, 12, 0, SelectOneHolding, 0,0,0, -1);
		y += 16;

		if(iSelectedOneHolding == 1)
			DrawLittleButton(x, y-1, 145, 12, TRUE);
		else
			DrawLittleButton(x, y-1, 145, 12, FALSE);
		print_textf(x+6, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_ASSASSINATE));
		add_region(x, y, 145, 12, 0, SelectOneHolding, 1,0,0, -1);
		y+=16;

		// type is Assassinate, now select unit
		if (iSelectedOneHolding == 1 && iSelectedUnit == 0)
		{
//			x = startX+160;
//			y = startY;
			print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_F00_UNITS));
			init_gfont(FONT_SANS_8PT);
			y += 12;

			for (int h1=0; h1<2; ++h1)
			{
			 	for(i = 1; i<MAX_UNITS; ++i)
				{
					if(units[i].province == iSelectedProvince
						&& units[i].DestProvince == NO_PROVINCE	// not moving.
						&& units[i].Joined == 0
						&& units[i].Realm != HomeRealm
						&& realm[units[i].Realm].mfExists()
						&& (units[i].NotSpotted==FALSE || CanScry)
//						&& (units[i].Icon<REG_M_FTR_UNIT_ICON
//							|| units[i].Icon>REG_GOB_FTR_UNIT_ICON
//							|| !realm[units[i].Realm].mfIsPlayerCtrl())
					//	&& units[i].Icon != GORGON_UNIT_ICON
					//	&& units[i].Icon != RHUOBHE_UNIT_ICON
						)
					{
						for(iUnit = i; iUnit > 0; iUnit = units[iUnit].NextUnit)
						{
							// don't allow assassination of PC regents
							if (units[i].Icon >= REG_M_FTR_UNIT_ICON
								&& units[i].Icon <= REG_GOB_FTR_UNIT_ICON
								&& realm[units[i].Realm].mfIsPlayerCtrl())
								continue;
								
							if (h1)
							{
								++counter1;
								if(counter1 < startProvCount1)
									continue;
		
								if(iSelectedUnit == iUnit)
								 	DrawLittleButton(x, y-1, 145, 10, TRUE);
								else
								 	DrawLittleButton(x, y-1, 145, 10, FALSE);
								DrawShield(x+2, y, (REALM::REALM_TYPE)units[iUnit].Realm);
							 	print_textf(x+16, y, DKBROWN, "%s", UnitName(iUnit));
				 				add_region(x, y, 145, 12, 0, SelectUnit, iUnit,0,0, -1);
								y += 15;
	
								// print prev, next buttons if number of units are more than 10
								if(DoPageProvince(&startProvCount1, provCount1, 10, counter1, x, y))
									goto assexit;
							}
							else
								++provCount1;
						}
					}
				}

assexit:   ;
			}
		}
   }

	// display selected unit
	if (iSelectedOneHolding == 1 && iSelectedUnit != 0)
	{
		if (assassin_roll == 0)
		{
			if (units[iSelectedUnit].Icon >= FIRST_CHARACTER_TYPE)
			{
				t_level = 1;	// just in case
				for (i=1; i<CHARACTER_COUNT; ++i)
				{
					if (regents[i].mfGetunit() == iSelectedUnit)
					{
						t_level = MAX(regents[i].mfGetLevel1(), regents[i].mfGetLevel2());
						break;
					}
				}
			}
			else		
			{
				t_level = UnitMusterCost[units[iSelectedUnit].Icon]+1;
			}

			sprintf(n,"%s",UnitName(iSelectedUnit));
			rlm = units[iSelectedUnit].Realm;
		}

		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_F00_UNIT));
		y += 12;
		init_gfont(FONT_SANS_8PT);
		DrawLittleButton(x, y-1, 145, 12, FALSE);
		DrawShield(x+2, y, (REALM::REALM_TYPE)rlm);
		gprint_text(x+16, y, n, RED);
		add_region(x, y, 145, 12, 0, SelectUnit, 0,0,0, -1);
		y+=16;
	}


	// allow modification of success roll
   if (iSelectedOneHolding == 0
      || (iSelectedOneHolding==1 && iSelectedUnit != 0))
   {
		fAllowRollButton = TRUE;
		
		y += 10;
		success = 20-province[iSelectedProvince].CivLevel-level - modRegency + modOpponent;
		RegencePoint = modRegency;
		GoldBar = 1;
  		if(checkGoldReg)
			fAllowRollButton = (ProcessGoldAndReg(CHECK_QUITE))?fAllowRollButton:0;
		else
			fAllowRollButton = (ProcessGoldAndReg(CHECK_ONLY))?fAllowRollButton:0;
	
		
		if (iSelectedOneHolding == 1)
		{
			print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_LOCATE_ASSASSIN));
			y+=12;
		}
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_F02_SUCCESS));
		print_textf(x+66, y+6, DKBROWN, "^F02^c%d%c", D20TOPERCENT(success),PERCENTCHAR);
		if (fLTAction != DURING_LTACTION)
		{
			y += 15;
			print_textf(x, y-2, DKBROWN, STRMGR_GetStr(STR_MAPAI_MODIFY_SUCCESS));
			print_textf(x, y+4, DKBROWN, STRMGR_GetStr(STR_MAPAI_USING_REGENCY));
			//GEH 10/97 SetupMod(x+75, y, &modRegency, 0, 19-province[iSelectedProvince].CivLevel-level+modOpponent, 1);
			SetupMod(x+95, y, &modRegency, 0, 19-province[iSelectedProvince].CivLevel-level+modOpponent, 1);
		}

		// check for success roll
		result = RollSuccess(success, x+68+7, y-15, TRUE);

		if (result == 0)
		{
			assassin_roll = 0;
			delay = 0;
		}

		if (result == 2)	// failure
		{
//			AddReport(IMMEDIATE_SPY_FAIL, 0, 0, 0);	// "Majesty, your agents were unable to carry out your order."
		}

		if (result == 1 || result == 3)	// was successful
		{
			// spy on units
			if(iSelectedOneHolding == 0)
			{
				x = startX + 160;
				y = startY;
				print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_F00_UNITS));
				y += 15;
				init_gfont(FONT_SANS_8PT);
				j=0;

				for (int h1=0; h1<2; ++h1)
				{
					for(i = 1; i<MAX_UNITS; ++i)
					{
						if(units[i].province == iSelectedProvince 
						   && units[i].Realm != HomeRealm 
							&& realm[units[i].Realm].mfExists()
						   && units[j].DestProvince == NO_PROVINCE	// Not moving.
						   && units[i].Joined == 0)
						{
							iUnit = i;
							for(iUnit = i; iUnit > 0; iUnit = units[iUnit].NextUnit)
							{
								if (h1)
								{
									++counter1;
									if(counter1 < startProvCount1)
										continue;
		
									DrawShield(x+2, y, (REALM::REALM_TYPE)units[iUnit].Realm);
									print_textf(x+16, y, DKBROWN, "%s", UnitName(iUnit) );
									units[iUnit].NotSpotted = FALSE;
									// -- Don't transmit this data change
									//SetGameData(MP_UNITS, MPUNITS_NOTSPOTTED, iUnit, FALSE, TRUE );
									y += 18;
									++j;
	
									// print prev, next buttons if number of units are more than 10
									if(DoPageProvince(&startProvCount1, provCount1, 10, counter1, x, y))
										goto spyexit;
								}
								else
									++provCount1;
							}
						}
					}
spyexit:	;
				}
				// give experience points
				ActionExpPoints(ActiveRegent, RLM_EXP_ESPIONAGE_SPY, 0, TRUE );
//				AddReport(IMMEDIATE_SPY, j, 0, 0);
			}

			// assassinate a unit
			else
			{
				fAllowDoneButton = FALSE;
				a_level = ((success_roll - success) / 2) + 3;
				x = startX + 160;
				y = startY;
				print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_ROLL_4_ASSASSINATION));
				y += 15;
				print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_OUR_AGENT));
				print_textf(x+135, y+4, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_cLEVEL));
				y += 10;
				print_textf(x, y, DKBROWN, (success_roll&1)?STRMGR_GetStr(STR_ACTNMENU_F02_THIEF):STRMGR_GetStr(STR_ACTNMENU_F02_FIGHTER));
				print_textf(x+135, y+6, DKBROWN, "^c%d", a_level);
				y += 18;

				print_textf(x, y, DKBROWN,STRMGR_GetStr(STR_ACTNMENU_TARGET));
				print_textf(x+135, y+4, DKBROWN,STRMGR_GetStr(STR_ACTNMENU_cLEVEL));
				y += 10;
				DrawShield(x+2, y, (REALM::REALM_TYPE)rlm);
				print_textf(x+16, y, DKBROWN, "^F02%s", n);
				print_textf(x+135, y+6, DKBROWN, "^c%d", t_level);
				y += 18;

				a_chance = 10 + t_level - a_level;
				if (a_chance<1) a_chance=1;
				if (a_chance>21) a_chance=21;
				print_textf(x, y+3, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_SUCCESS_CHANCE));
				print_textf(x+135, y+5, DKBROWN, "^F02^c%d%c", D20TOPERCENT(a_chance),PERCENTCHAR );

				if (result == 1)	// success
				{
					AddSndObj((BIRTHRT_SND)SND_UI_ASSASINATION_ATTEMPT1,0,VOLUME_NINETY);
					delay = get_time();
					sequence = 0;
				}

				if (delay != 0)
				{
					j = get_time() - delay;
					if (j > 25 && sequence < 1)
						{AddSndObj((BIRTHRT_SND)SND_BIG_SWORD_HIT2,0,VOLUME_NINETY); ++sequence;}
					if (j > 38 && sequence < 2)
						{AddSndObj((BIRTHRT_SND)SND_SMALL_SWORD_HIT2,0,VOLUME_NINETY); ++sequence;}
					if (j > 45 && sequence < 3)
						{AddSndObj((BIRTHRT_SND)SND_BIG_SWORD_HIT6,0,VOLUME_NINETY); ++sequence;}
					if (j > 58 && sequence < 4)
						{AddSndObj((BIRTHRT_SND)SND_SMALL_SWORD_HIT3,0,VOLUME_NINETY); ++sequence;}
					if (j > 65 && sequence < 5)
						{AddSndObj((BIRTHRT_SND)SND_BIG_SWORD_HIT9,0,VOLUME_NINETY); ++sequence;}
					if (j > 70 && sequence < 6)
						{AddSndObj((BIRTHRT_SND)SND_WEAPON_STAB2,0,VOLUME_NINETY); ++sequence;}
					if (j > 75 && sequence < 7)
						{AddSndObj((BIRTHRT_SND)SND_MALE_SCREAM5,0,VOLUME_NINETY); ++sequence;}
					if (j > 90 && sequence < 8)
						{AddSndObj((BIRTHRT_SND)SND_BODY_FALLS_DEAD1,0,VOLUME_NINETY); ++sequence;}
					if (j > 120 && sequence < 9)
						{AddSndObj((BIRTHRT_SND)SND_UI_ASSASINATION_ATTEMPT1,0,VOLUME_NINETY); ++sequence;}
					if (j > 150 && assassin_roll==0) // do final roll for assassination
					{
						assassin_roll = random(20)+1;
						if (assassin_roll >= a_chance)
						{
							// give experience points
							ActionExpPoints(ActiveRegent, RLM_EXP_ESPIONAGE_ASSASSINATE, 0, TRUE );
							REALM::REALM_TYPE const whichRealm = (REALM::REALM_TYPE)units[iSelectedUnit].Realm;
							PROVINCE const whichProv = (PROVINCE) units[GetUnitLeader(iSelectedUnit)].province;
							
							AddSndObj((BIRTHRT_SND)SND_UI_SUCCEEDED,0,VOLUME_NINETY);
							if (whichProv > NO_PROVINCE && whichProv < NO_PROVINCE2)
								SendReportTo(whichRealm, IMMEDIATE_NPC_ASSASSIN, HomeRealm, whichProv, 0);
							SetScrewed(whichRealm, HomeRealm, TRUE );
				  			DeleteUnit(iSelectedUnit, TRUE );
// 							AddReport(IMMEDIATE_ASSASSIN, 1, 0, 0);	// assassinate succeeds
// 							AddReport(IMMEDIATE_ASSASSIN, 1, 0, 0);	// assassinate succeeds
						}
						else
						{
							AddSndObj((BIRTHRT_SND)SND_UI_DID_NOT_SUCCEED,0,VOLUME_NINETY);
// 							AddReport(IMMEDIATE_ASSASSIN_FAIL, 0, 0, 0);	// assassinate fails
						}
					}
					if (j > 180 && sequence < 10 && assassin_roll < a_chance)
					{
						++sequence;
						if (iSelectedUnit==267 || iSelectedUnit==268) // GORGON
							AddSndObj((BIRTHRT_SND)SND_GORGON_VOX6,0,VOLUME_NINETY);
						if (iSelectedUnit==285)					// RHUOBHE
							AddSndObj((BIRTHRT_SND)SND_GORGON_VOX6,0,VOLUME_NINETY);
					}
				}

				// result of final roll for assassination
				if (assassin_roll != 0)
				{
					fAllowDoneButton = TRUE;
					y+=12;
					if (assassin_roll >= a_chance)
					{
						print_textf(x+136, y+6, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_F02_WON));
						print_textf(x+135, y+5, MDGREEN, STRMGR_GetStr(STR_ACTNMENU_WON));
					}
					else
					{
						print_textf(x+136, y+6, DKBROWN,STRMGR_GetStr(STR_ACTNMENU_F02_LOST));
						print_textf(x+135, y+5, RED,STRMGR_GetStr(STR_ACTNMENU_LOST));
					}
				}
			}
		}
	}


	if (iSelectedRealm)
	{
		GoldBar = 1;
		RegencePoint = modRegency;
		x = startX+160; 
		y = startY+h-85;
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_TOTAL_GB_COST_FMT), GoldBar);
		y += 13;
		print_textf(x+63, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_REGENCY_FMT), RegencePoint);
		y += 15;
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_SELECT_REALM));
	}
	else
		print_textf(startX, startY+h-45, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_SEL_REALM_ACTIVITY));

	if (fAllowDoneButton)
		DeleteActnMenuRegions();

}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
void PaintDeclareWar (LONG x, LONG y, LONG h)
{
	SHORT	i, startY = y;

	print_textf(x, startY+h-48, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_DECLARE_WAR_HELP));

	for (i=1; i<LAND_REALM_COUNT; ++i)
	{
		if (i==HomeRealm || !realm[i].mfExists()
				|| i==REALM::MONSTERS || i==REALM::ANUIRE
				|| ISVASSAL(i,HomeRealm) )
			continue;
		if ((y-startY)==(18*15)) {y = startY; x+=160;}
		init_gfont(FONT_SANS_8PT);
		DrawLittleButton(x, y-1, 145, 12, (iSelectedRealm==i));
		DrawShield(x+2, y, (REALM::REALM_TYPE)i);

		if (((RealmStatus[i][iSelectedRealm])&0x0F) >= STATUS_FULL_ALLIANCE)
			gprint_text(x+16, y, realm[i].mfGetName(), StatusColor[1]);
		else
			gprint_text(x+16, y, realm[i].mfGetName(), StatusColor[(RealmStatus[i][HomeRealm])&0x0F]);
			gprint_text(x+16, y, realm[i].mfGetName(), ReactionAndStatusColor(HomeRealm,i));
		add_region(x, y, 145, 12, 0, SelectRealm,i,0,0, -1);
		y+=18;
	}
	ClearRemapTable();

	fAllowCancelButton = TRUE;
	fAllowRollButton = FALSE;
	fAllowDoneButton = (iSelectedRealm != 0);
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
#define xFIRSTBTN			(104)
#define dxBTN				(43)
#define wBTN				(43-6)
#define wBTNDN				(8)
#define xSECONDBTN		(xFIRSTBTN+dxBTN)
#define xMODEXMPL			270

static	LONG		tmp_provCount=0;

void PaintRule (LONG x, LONG y, LONG h)
{
	LONG	i, j, f, q, xx, tmpIndex;
	LONG 	startY, level, select, index, tmp_index, totalLevel;
	LONG	success, result;
	UBYTE	dummy = 0;
	LONG		provCount;
	LONG		flag = 0;
	LONG		counter = 0;
	LONG		lPrint = TRUE;
	LONG		lPageDone = FALSE;
	BOOL		fCostUnlocked = (fAllowCancelButton || fAllowRollButton);

	startY = y;
	provCount = tmp_provCount;
	tmp_provCount = 0;

	del_region(NextPrev,0);

//	if (!fAllowDoneButton)
	if (fCostUnlocked)
		GoldBar = RegencePoint = 0;

	fAllowCancelButton = !fAllowDoneButton;

	init_gfont(FONT_SANS_5PT);
	print_textf(x, y, DKBROWN, "%s", STRMGR_GetStr(STR_ACTNMENU_CAPS_PROVINCE));
 	print_textf(x+xFIRSTBTN+19, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_cCAPS_PROVINCE));
	print_textf(x+xFIRSTBTN+19, y+8, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_cCAPS_LEVEL));
	print_textf(x+xSECONDBTN+dxBTN+(dxBTN/2)+19, y, DKBROWN, "^c%s", STRMGR_GetStr(STR_ACTNMENU_HOLDING_LEVEL));
	line(x+xSECONDBTN, y+3, x+xSECONDBTN+(3*dxBTN)+35, y+3, DKBROWN);
	for (i = 0; i < 4; ++i)
		print_textf(x+xSECONDBTN+(i*dxBTN)+19, y+8, DKBROWN, "^c%s", STRMGR_GetStr(gsHoldType[i]));
	y += 15;

//	for (int h1=0; h1<2; ++h1)
//	{

		for (q=0; q<2; ++q)
		{
			for (i=FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)
			{
				if ( (!q && province[i].Realm == HomeRealm)
					|| (q && province[i].Realm != HomeRealm 
					       && province[i].CastleSptd) )			// HACK This means you have a holding in this province.
					ProvinceInfo((PROVINCE)i, CASTLE1_ICON);

				if ((!q && province[i].Realm == HomeRealm)
					|| (q && (prov_law>=0 || prov_temple>=0 || prov_guild>=0 || prov_source>=0) 
					      && province[i].Realm!=HomeRealm 
					      && province[i].CastleSptd) )			// HACK This means you have holding in this province.
				{
//					if (h1)
//					{
						++counter;
						if(counter < startProvCount || lPageDone || provCount == 0)
							lPrint = FALSE;
						else
							lPrint = TRUE;

						if(lPrint)
						{
							init_gfont(FONT_SANS_8PT);
							if (province[i].Realm == HomeRealm)
								gprint_text(x, y, province[i].name, LoyaltyColor[province[i].Loyalty]);
							else
								gprint_text(x, y, province[i].name, DKBROWN);
						}
			
						if (province[i].Realm == HomeRealm)
						{
							// ------------------------------------------
							// for provinces
							// ------------------------------------------
							f = (iSelectedProvince==i);

							if (lPrint)
							{
								if ( province[i].CivLevel<10	// DON'T CHANGE THIS WITHOUT TALKING TO ALAN
									|| province[i].Contested )
								{
									DrawLittleButton(x+xFIRSTBTN, y, (f)?wBTNDN:wBTN, 9, f);
									add_region(x+xFIRSTBTN, y, (f)?wBTNDN:wBTN, 9, 0, SelectProvince, i, 0, 0, -1);
								}
								print_textf(x+xFIRSTBTN+((f)?5:19), y+6, DKBROWN, "^c%d", province[i].CivLevel);
							}
							if (f)
							{
								level = province[i].CivLevel;
								//if (!fAllowDoneButton)
								if (fCostUnlocked)
								{
									GoldBar += level+1;
									if (level)
										RegencePoint += level+1;
									RegencePoint += modRegency;
									if(fCostUnlocked)
										fAllowRollButton = TRUE;
								  	if(checkGoldReg)
										fAllowRollButton = (ProcessGoldAndReg(CHECK_QUITE))?fAllowRollButton:0;
									else
										fAllowRollButton = (ProcessGoldAndReg(CHECK_ONLY))?fAllowRollButton:0;
								}

								// success
								success = 10 + level - modRegency + modOpponent;
								if (lPrint)
								{
									if (fLTAction != DURING_LTACTION)
										SetupMod(x+xFIRSTBTN+17, y, &modRegency, 0, 10+level+modOpponent, -1);
									else
									 	print_textf(x+xFIRSTBTN+17, y, DKBROWN, "^F02%d%c", D20TOPERCENT(success),PERCENTCHAR);
								}

								// check for success roll
								iSelectedOneHolding = 4; 

								// multi-player and AI support
								if(lAskedTargetWhichRoll == iThisRoll + 1)
									success = AskAndWaitForTargetPlayer(RULE, i, 0, success, x+xFIRSTBTN+18+7, y);

								result = RollSuccess(success, x+xFIRSTBTN+18+7, y, lPrint);

								if (result == 1)	// success
								{
									// multi-player support
									SendResult(RULE, i, 0, result);
									SyncRegencySpentInOpposition((REALM::REALM_TYPE)province[i].Realm);
									
									if (province[i].Contested)
									{
										province[i].Contested = FALSE;
										SetGameData(MP_PROVINCE, MPPROV_CONTESTED, i, FALSE, TRUE );
									}
									if (province[i].CivLevel<10) // DON'T CHANGE THIS WITHOUT TALKING TO ALAN
									{
										++province[i].CivLevel;
										// give experience points
										ActionExpPoints(ActiveRegent, RLM_EXP_RULE_PROV, province[i].CivLevel, TRUE );
										SetGameData(MP_PROVINCE, MPPROV_CIVLEVEL, i, province[i].CivLevel, TRUE);
										if (province[i].MagLevel > 0 && !province[i].ElfPop)
										{
											--province[i].MagLevel;
											SetGameData(MP_PROVINCE, MPPROV_MAGLEVEL, i, province[i].MagLevel, TRUE );
											CheckAndFixSourceLevels((PROVINCE)i);
										}
									}
//									AddReport(IMMEDIATE_RULE_PROV, i, province[i].CivLevel, 0);
								}
								if (result == 2)	// failure
								{
									// multi-player support
									SendResult(RULE, i, 0, result);
									SyncRegencySpentInOpposition((REALM::REALM_TYPE)province[i].Realm);
//									AddReport(IMMEDIATE_RULE_PR_FAIL, i, 0, 0);
								}
							}
						}
						else
							if(lPrint)
								print_textf(x+xFIRSTBTN+19, y+6, DKBROWN, "^c-");

						// ------------------------------------------
						// for holdings
						// ------------------------------------------
						for (j=0; j<4; ++j)
						{
							init_gfont(FONT_SANS_8PT);
							flag = 0;
							xx = x + xSECONDBTN + (j * dxBTN);
							level = (j==0)?prov_law:(j==1)?prov_guild:(j==2)?prov_temple:prov_source;
							if (level >= 0)
							{
								CheckAndFixPlaces((PROVINCE)i);
								tmp_index = index = province[i].FirstPlace;
								totalLevel = 0;
								while (index != 0)
								{
									if (places[tmp_index].Icon == InfoReqType[j])
										totalLevel+= places[tmp_index].Level;
									index = places[tmp_index].NextPlace;
									tmp_index = province[i].FirstPlace+index;
								}
							 	if ((InfoReqType[j]==SOURCE1_ICON && totalLevel < province[i].MagLevel)
									||(InfoReqType[j]!=SOURCE1_ICON && totalLevel < province[i].CivLevel))
									flag = TRUE;
								index = ProvinceInfo((PROVINCE)i, InfoReqType[j]);

								f = iSelectedHolding[i][j];
								if (flag || places[index].Contested || f)
								{
									if(lPrint)
									{
										DrawLittleButton(xx, y, (f)?wBTNDN:wBTN, 9, f);
										add_region(xx, y, (f)?wBTNDN:wBTN, 9 ,0, SelectHolding,i,HoldingType[j],0, -1);
									}
									if (f)
									{
										// selecting a holding clears the selected province (there can be only one)
										fAllowRollButton -= (iSelectedProvince!=0);
										iSelectedProvince = NO_PROVINCE;

										//if (!fAllowDoneButton)
										if (fCostUnlocked)
										{
											++GoldBar;
											if (level) RegencePoint += level+1;
											RegencePoint += iSelectedHolding[i][j]-1;
											if(fCostUnlocked)
												fAllowRollButton = TRUE;
										  	if(checkGoldReg)
												fAllowRollButton = (ProcessGoldAndReg(CHECK_QUITE))?fAllowRollButton:0;
											else
												fAllowRollButton = (ProcessGoldAndReg(CHECK_ONLY))?fAllowRollButton:0;
										}

										// success
										success = 10 + level - (iSelectedHolding[i][j]-1);
										//	printf("level:%d iSH:%d success:%d\n",level,iSelectedHolding[i][j],D20TOPERCENT(success));

										if(lPrint)
										{
											if (fLTAction != DURING_LTACTION)
												SetupMod(xx+17, y, &iSelectedHolding[i][j], 0, 11+level, -1);
											else
											 	print_textf(xx+17, y, DKBROWN, "^F02%d%c", D20TOPERCENT(success),PERCENTCHAR);
										}

										iSelectedOneHolding = j; 

										// multi-player and AI support
										if(lAskedTargetWhichRoll == iThisRoll + 1)
											success = AskAndWaitForTargetPlayer(RULE, 0, index, success, xx+18+7, y);

										// check for success roll
										result = RollSuccess(success, xx+18+7, y, lPrint);
										if (result == 1)	// success
										{
											// multi-player support
											SendResult(RULE, 0, j, result);
											SyncRegencySpentInOpposition((REALM::REALM_TYPE)places[j].Realm);
										 	if (flag)		// we can increase the level
											{
												++places[index].Level;
												SetGameData(MP_PLACES, MPPLACE_LEVEL, index, places[index].Level, TRUE );
												SetGameData(MP_PLACES, MPPLACE_IICONBITM, index, places[index].Icon+(((places[index].Level<8)?places[index].Level:7)/2), TRUE );
											}
											places[index].Contested = FALSE;
											SetGameData(MP_PLACES, MPPLACE_CONTESTED, index, FALSE, TRUE);

//											AddReport(IMMEDIATE_RULE_HOLD, j, i, places[index].Level);

											// decide which type of holding was selected and deal exp points
											switch ( places[index].Icon )
											{
											// guild
											case GUILD1_ICON:
											case GUILD2_ICON:
											case GUILD3_ICON:
											case GUILD4_ICON:
												ActionExpPoints(ActiveRegent, RLM_EXP_RULE_GUILD, places[index].Level, TRUE );
												break;
											
											// law
											case LAW1_ICON:
											case LAW2_ICON:
											case LAW3_ICON:
											case LAW4_ICON:
												ActionExpPoints(ActiveRegent, RLM_EXP_RULE_LAW, places[index].Level, TRUE );
												break;
											
											// source
											case SOURCE1_ICON:
											case SOURCE2_ICON:
											case SOURCE3_ICON:
											case SOURCE4_ICON:
												ActionExpPoints(ActiveRegent, RLM_EXP_RULE_SOURCE, places[index].Level, TRUE );
												break;
											
											// temples
											case TEMPLE1_ICON:
											case TEMPLE2_ICON:
											case TEMPLE3_ICON:
											case TEMPLE4_ICON:
												ActionExpPoints(ActiveRegent, RLM_EXP_RULE_TEMPLE, places[index].Level, TRUE );
												break;
											
											// castles and others
											case WIZTOWER_ICON:
											case CASTLE1_ICON:
											case CASTLE2_ICON:
											case CASTLE3_ICON:
											case CASTLE4_ICON:
											
											case VILLAGE_ICON:
											case SEAPORT_ICON:
											case SEAPORT2_ICON:
											case PORT_ICON:
											case RUINS_ICON:
											case SPECIAL1_ICON:
											case SPECIAL2_ICON:
											case SPECIAL3_ICON:
												break;
											}
										}
										if (result == 2)	// failure
										{
											// multi-player support
											SendResult(RULE, 0, j, result);
											SyncRegencySpentInOpposition((REALM::REALM_TYPE)places[j].Realm);
//											AddReport(IMMEDIATE_RULE_HLD_FAIL, j, i, 0);
										}
									}
								}
								if(places[index].Contested)
								{
									if(lPrint)
									{
										print_textf(xx+((f)?5:10), y+6, DKBROWN, "^F02^c%d", level);
										if (!f)
											print_textf(xx+16, y+3, DKBROWN,STRMGR_GetStr(STR_ACTNMENU_CAPS_CONT));
									}
								}
								else
									if(lPrint)
										print_textf(xx+((f)?5:19), y+6, DKBROWN, "^F02^c%d", level);
							}
							else
								if(lPrint)
									print_textf(xx+19, y+6, DKBROWN, "^F02^c-");
						}

						if(lPrint)
							y += 15;

						// print prev, next buttons if provinces are more than 15		
						if(lPrint)
						{
							if(DoPageProvince(&startProvCount, provCount, 15, counter, x, y))
							{
								lPrint = FALSE;
								lPageDone = TRUE;
							}
						}
//					}
//					else
						++tmp_provCount;
				}
			}
		}
//	}
  	if(checkGoldReg)
		fAllowRollButton = (ProcessGoldAndReg(CHECK_QUITE))?fAllowRollButton:0;
	else
		fAllowRollButton = (ProcessGoldAndReg(CHECK_ONLY))?fAllowRollButton:0;
	

	y = startY+h-67;
	print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_TOTAL_COST_FMT), GoldBar, RegencePoint);
	y += 15;
	print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_HOLDINGS_HELP));

	// show example of success and mod
	y-=8;
	print_textf(x+xMODEXMPL-14, y, DKBLUE, STRMGR_GetStr(STR_ACTNMENU_EXAMPLE));
	y+=9;
	print_textf(x+xMODEXMPL-14, y, DKBLUE, STRMGR_GetStr(STR_ACTNMENU_LEVEL_SUCCESS));
	y+=11;
	line(x+xMODEXMPL-14, y-4, x+xMODEXMPL+10, y-4, DKBLUE);
	line(x+xMODEXMPL+16, y-4, x+xMODEXMPL+49, y-4, DKBLUE);
	line(x+xMODEXMPL-10, y-4, x+xMODEXMPL-10, y+4, DKBLUE);
	line(x+xMODEXMPL-10, y+4, x+xMODEXMPL-6, y+4, DKBLUE);
	line(x+xMODEXMPL+20, y-4, x+xMODEXMPL+20, y-1, DKBLUE);
	DrawLittleButton(x+xMODEXMPL, y, wBTNDN, 9, TRUE);
	print_textf(x+xMODEXMPL+6, y+6, DKBROWN, "^F02^c3");
	SetupMod(x+xMODEXMPL+18, y, &dummy, 0, 13, -1);
	y+=14;
	print_textf(x+xMODEXMPL-14, y, DKBLUE, STRMGR_GetStr(STR_ACTNMENU_SPEND_REGENCY));
	line(x+xMODEXMPL-14, y-1, x+xMODEXMPL+47, y-1, DKBLUE);
	line(x+xMODEXMPL+44, y-1, x+xMODEXMPL+44, y-10, DKBLUE);
	line(x+xMODEXMPL+41, y-10, x+xMODEXMPL+44, y-10, DKBLUE);

	if (fAllowDoneButton)
		DeleteActnMenuRegions();
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
void PaintCreateHolding (LONG x, LONG y, LONG h)
{
	LONG  i, j, k, startY, startX, success, result;
	LONG  realm_flag = 0;
	LONG  prov_flag = 0;
	LONG  holding[] = {0, 0, 0, 0};
	LONG		provCount = 0;
	LONG		counter = 0;

	startY = y;
	startX = x;
	GoldBar = RegencePoint = 0;
	fAllowRollButton = FALSE;

	del_region(NextPrev,0);
	fAllowCancelButton = !fAllowDoneButton;
	// allow a realm to be selected
	if (iSelectedRealm==0)
	{
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_CAPS_REALM));
		y+=15;

		init_gfont(FONT_SANS_8PT);
		for (i=1; i<LAND_REALM_COUNT; ++i)
		{
			if (!realm[i].mfExists() || i==REALM::MONSTERS)
				continue;

			if ((y-(startY+15))==(16*15))			// two columns
			{
				y = startY+15;
				x+=160;
			}

			DrawLittleButton(x, y-1, 145, 10, FALSE);
			DrawShield(x+2, y, (REALM::REALM_TYPE)i);
			gprint_text(x+16, y, realm[i].mfGetName(), DKBROWN);
			add_region(x, y, 145, 10, 0, SelectRealm,i,0,0, -1);
			y+=15;
		}
	}

	if (iSelectedRealm)	// realm has been selected, now select province
	{
		DrawLittleButton(x, y-1, 145, 12, FALSE);
		DrawShield(x+2, y, (REALM::REALM_TYPE)iSelectedRealm);

		gprint_text(x+16, y, realm[iSelectedRealm].mfGetName(), RED);
		add_region(x, y, 145, 10, 0, SelectRealm,iSelectedRealm,0,0, -1);
		y+=20;

		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_CAPS_F00_PROVINCE));
		y+=10;

		init_gfont(FONT_SANS_8PT);
		for (int h1=0; h1<2; ++h1)
		{
			for (i=FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)
			{
				if (province[i].Realm == iSelectedRealm)
				{
					if (h1)
					{
						++counter;
						if(counter < startProvCount)
							continue;
	
						DrawLittleButton(x, y-1, 145, 10, (iSelectedProvince==i));
						if (i==iSelectedProvince)
							gprint_text(x+16, y, province[i].name, RED);
						else
							gprint_text(x+16, y, province[i].name, DKBROWN);
		
						add_region(x, y, 145, 10, 0, SelectProvince, i, 0, 0, -1);
						y += 15;

						// print prev, next buttons if provinces are more than 17		
						if(DoPageProvince(&startProvCount, provCount, 17, counter, x, y))
							break;
					}
					else
						++provCount;
				}
			}
		}
		x = startX+160;
	}
	// realm and province have been selected, now select holding type
	if (iSelectedRealm && iSelectedProvince)
	{
		y = startY+20;
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_HOLDING));
		y += 10;

		init_gfont(FONT_SANS_8PT);

		CheckAndFixPlaces(iSelectedProvince);
		i = k = province[iSelectedProvince].FirstPlace;
		
		assert(i < MAX_PLACES);	// GEH chasing a bug
		
		while (k != 0)
		{
			if (places[i].Realm == HomeRealm)
				for (j = 0; j<4; ++j)
					if (InfoReqType[j]==places[i].Icon)
						holding[j]=TRUE;

			k = places[i].NextPlace;
			i = province[iSelectedProvince].FirstPlace + k;
			assert(i < MAX_PLACES);	// GEH chasing a bug
		}

		// holding options
		init_gfont(FONT_SANS_8PT);
		for (i = 0; i<4; ++i)
		{
			if (holding[i]==FALSE || i==iSelectedOneHolding)
			{
				DrawLittleButton(x, y-1, 145, 10, (iSelectedOneHolding==i));
				add_region(x, y, 145, 10, 0, SelectOneHolding, i, 0, 0, -1);
			}
			gprint_text(x+16, y, STRMGR_GetStr(gsHoldType[i]), DKBROWN);
			y += 15;
		}

		if (iSelectedOneHolding != 99)
		{
			GoldBar = 1;
			RegencePoint = modRegency; 

			fAllowRollButton = TRUE;
  			if(checkGoldReg)
				fAllowRollButton = (ProcessGoldAndReg(CHECK_QUITE))?fAllowRollButton:0;
	   	else
				fAllowRollButton = (ProcessGoldAndReg(CHECK_ONLY))?fAllowRollButton:0;
	

			success = 10 + ((province[iSelectedProvince].Realm==HomeRealm)?-province[iSelectedProvince].CivLevel:province[iSelectedProvince].CivLevel) - modRegency + modOpponent;
			if(success < 1)
				success = 1;

			print_textf(x, startY+120, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_F02_SUCCESS));
			print_textf(x+66, startY+120+6, DKBROWN, "^F02^c%d%c", D20TOPERCENT(success),PERCENTCHAR);
			if (fLTAction != DURING_LTACTION)
			{
				print_textf(x+110, startY+120-16, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_F00_cMODIFY_SUCCESS));
				print_textf(x+110, startY+120-8, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_cUSING_REGENCY));
				SetupMod(x+105, startY+120, &modRegency, 0, (9 + ((province[iSelectedProvince].Realm==HomeRealm)?-province[iSelectedProvince].CivLevel:province[iSelectedProvince].CivLevel)+modOpponent), 1);
			}

			// multi-player and AI support
			if(lAskedTargetWhichRoll == iThisRoll + 1)
				success = AskAndWaitForTargetPlayer(CREATE_HOLDING, iSelectedProvince, 0, success, x+63+7, startY+120);

			// check for success roll
			result = RollSuccess(success, x+63+7, startY+120, TRUE);
			if (result == 1)	// success
			{
				// decide which type of holding was selected and deal exp points
				switch ( places[iSelectedOneHolding].Icon )
				{
				// guild
				case GUILD1_ICON:
				case GUILD2_ICON:
				case GUILD3_ICON:
				case GUILD4_ICON:
					ActionExpPoints(ActiveRegent, RLM_EXP_CREATE_GUILD, 0, TRUE );
					break;
				
				// law
				case LAW1_ICON:
				case LAW2_ICON:
				case LAW3_ICON:
				case LAW4_ICON:
					ActionExpPoints(ActiveRegent, RLM_EXP_CREATE_LAW, 0, TRUE );
					break;
				
				// source
				case SOURCE1_ICON:
				case SOURCE2_ICON:
				case SOURCE3_ICON:
				case SOURCE4_ICON:
					ActionExpPoints(ActiveRegent, RLM_EXP_CREATE_SOURCE, 0, TRUE );
					break;
				
				// temples
				case TEMPLE1_ICON:
				case TEMPLE2_ICON:
				case TEMPLE3_ICON:
				case TEMPLE4_ICON:
					ActionExpPoints(ActiveRegent, RLM_EXP_CREATE_TEMPLE, 0, TRUE );
					break;
				
				// castles and others
				case WIZTOWER_ICON:
				case CASTLE1_ICON:
				case CASTLE2_ICON:
				case CASTLE3_ICON:
				case CASTLE4_ICON:
				
				case VILLAGE_ICON:
				case SEAPORT_ICON:
				case SEAPORT2_ICON:
				case PORT_ICON:
				case RUINS_ICON:
				case SPECIAL1_ICON:
				case SPECIAL2_ICON:
				case SPECIAL3_ICON:
					break;
				}
				// multi-player support
				SendResult(CREATE_HOLDING, iSelectedProvince, 0, result);
				SyncRegencySpentInOpposition((REALM::REALM_TYPE)province[iSelectedProvince].Realm);
				CreateHolding(iSelectedOneHolding, 0, 0, iSelectedProvince, HomeRealm, TRUE );
				
				// GWP I'd like to reduce his liking of you here no matter what
				//     you do. But only set the screw bit if you violate the treaty.
				//     You can do anything to a vassal
				if (!ISVASSAL((REALM::REALM_TYPE)province[iSelectedProvince].Realm, HomeRealm))
				{
					if (AREFULLALLIES((REALM::REALM_TYPE) province[iSelectedProvince].Realm, HomeRealm))
					{
						if ( places[iSelectedOneHolding].Icon == LAW1_ICON)
							SetScrewed((REALM::REALM_TYPE)province[iSelectedProvince].Realm, HomeRealm, TRUE );
					}
					else if (AREALLIED((REALM::REALM_TYPE)province[iSelectedProvince].Realm, HomeRealm))
					{
						if ( places[iSelectedOneHolding].Icon == LAW1_ICON ||
						     places[iSelectedOneHolding].Icon == SOURCE1_ICON ||
						     places[iSelectedOneHolding].Icon == TEMPLE1_ICON)
					    {
							SetScrewed((REALM::REALM_TYPE)province[iSelectedProvince].Realm, HomeRealm, TRUE );
						}
					}
					else
					{
						SetScrewed((REALM::REALM_TYPE)province[iSelectedProvince].Realm, HomeRealm, TRUE );
					}
				}
//				AddReport(IMMEDIATE_CREATE_HOLD, iSelectedOneHolding, iSelectedProvince, iSelectedRealm);
			}
			if (result == 2)	// failure
			{
				// multi-player support
				SendResult(CREATE_HOLDING, iSelectedProvince, 0, result);
				SyncRegencySpentInOpposition((REALM::REALM_TYPE)province[iSelectedProvince].Realm);
//				AddReport(IMMEDIATE_CREATE_HLD_FAIL, iSelectedOneHolding, iSelectedProvince, iSelectedRealm);
			}
		}
	}

	if (iSelectedRealm)	// different instructions depending on context
	{
		#ifdef _FOREIGNVER
		y = startY+h-90;
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_TOTAL_GB_COST_FMT), GoldBar);
		y+=13;
		print_textf(x+63, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_REGENCY_FMT), RegencePoint);
		y+=15;
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_SELECT_REALM_B1));
		y+=30;					   
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_SELECT_REALM_B2));

		#else
		y = startY+h-80;
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_TOTAL_GB_COST_FMT), GoldBar);
		y+=13;
		print_textf(x+63, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_REGENCY_FMT), RegencePoint);
		y+=15;
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_SELECT_REALM_B1));
		y+=20;					   
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_SELECT_REALM_B2));
		#endif
	}
	else
		print_textf(startX, startY+h-45, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_SELECT_REALM_B3));

	if (fAllowDoneButton)
		DeleteActnMenuRegions();
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
#define FORT_FIRST_COL		120
#define FORT_SECND_COL		190
#define FORT_THIRD_COL		260

void PaintFortify (LONG x, LONG y, LONG h)
{
	LONG 	startY = y;
	LONG 	tmp, count, i, dLevel, newCastle;
	LONG	provCount = 0;
	LONG	MaxLevel = 0;
	LONG	counter = 0;
	LONG	lPrint = TRUE;
	LONG	lPageDone = FALSE;

	GoldBar = RegencePoint = 0;

	del_region(NextPrev,0);

	y+=3;
	print_textf(x, y, DKBROWN, "^F00%s", STRMGR_GetStr(STR_ACTNMENU_CAPS_PROVINCE));

	print_textf(x+FORT_FIRST_COL, y-6,	DKBROWN,STRMGR_GetStr(STR_ACTNMENU_EXISTING));
	print_textf(x+FORT_FIRST_COL, y,	DKBROWN,STRMGR_GetStr(STR_ACTNMENU_CASTLES));
	print_textf(x+FORT_SECND_COL, y-6,	DKBROWN,STRMGR_GetStr(STR_ACTNMENU_DESIRED));
	print_textf(x+FORT_SECND_COL, y,	DKBROWN,STRMGR_GetStr(STR_ACTNMENU_LEVEL));
	print_textf(x+FORT_THIRD_COL, y-6,	DKBROWN,STRMGR_GetStr(STR_ACTNMENU_GB_COST));
	print_textf(x+FORT_THIRD_COL, y,	DKBROWN,STRMGR_GetStr(STR_ACTNMENU_THIS_TURN));

	y+=12;
	for (int h1=0; h1<2; ++h1)
	{
		for (i=FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)
		{
			dLevel = 0;
			count = 0;
			newCastle = 0;
			MaxLevel = 0;
			if (province[i].Realm == HomeRealm)
			{
				if (h1)
				{
					++counter;
					if(counter < startProvCount || lPageDone)
						lPrint = FALSE;
					else
						lPrint = TRUE;

					if(lPrint)
					{
						init_gfont(FONT_SANS_8PT);
					    gprint_text(x, y, province[i].name, LoyaltyColor[province[i].Loyalty]);
					}

					CheckAndFixPlaces((PROVINCE)i);
					count = tmp = province[i].FirstPlace;
					while (tmp != 0)
					{
						if (places[count].Icon == InfoReqType[4])
						{
							if(lPrint)
								print_textf(x+FORT_FIRST_COL+18, y, DKBROWN, "%d", places[count].Level);
							dLevel = places[count].Level;
							if(places[count].iRoute1 > 0)
								newCastle = places[count].iRoute1;
							if(places[count].Level >= province[i].CivLevel)
								MaxLevel = TRUE;
							break;
						}
						tmp = places[count].NextPlace;
						count = province[i].FirstPlace + tmp;
					}

					if (tmp == 0 && lPrint)
						print_textf(x+FORT_FIRST_COL+18, y, DKBROWN, "-");

					if(!iSelectedHolding[i][1])
						iSelectedHolding[i][1]=random(6)+1;		 // use iSelectedHolding[i][1] to hold the gold bars got to be spent this turn
		
					iSelectedHolding[i][0]+=dLevel;

					if(lPrint)
					{		
						if (!newCastle && !MaxLevel
							&& !ForeignUnitsHere((PROVINCE)i, HomeRealm) )
						{
							SetupMod(x+FORT_SECND_COL, y, &iSelectedHolding[i][0], dLevel, province[i].CivLevel, 1); 	// use iSelectedholding[i][0] as castle holding
						}
						else
						{
							print_textf(x+FORT_SECND_COL, y, DKBROWN, "%d", newCastle);
						}
					}
		
					iSelectedHolding[i][0]-=dLevel;
					init_gfont(FONT_SANS_8PT);
					if(iSelectedHolding[i][0])
					{
						RegencePoint += 1;
						if(lPrint)
							print_textf(x+FORT_THIRD_COL+17, y, DKBROWN, "%d",iSelectedHolding[i][1]);
						GoldBar += iSelectedHolding[i][1];
					}
					else
					{
						if(lPrint)
							print_textf(x+FORT_THIRD_COL+17, y, DKBROWN, "-");
					}
					if(lPrint)
						y += 15;

					// print prev, next buttons if provinces are more than 15		
					if(lPrint)
					{
						if(DoPageProvince(&startProvCount, provCount, 15, counter, x, y))
						{
							lPrint = FALSE;
							lPageDone = TRUE;
						}
					}
				}
				else
					++provCount;
			}
		}
	}
  	if(checkGoldReg)
		fAllowRollButton = (ProcessGoldAndReg(CHECK_QUITE))?fAllowRollButton:0;
	else
		fAllowRollButton = (ProcessGoldAndReg(CHECK_ONLY))?fAllowRollButton:0;
	

	print_textf(x, startY+h-65, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_TOTAL_COST_FMT), GoldBar, RegencePoint);
	print_textf(x, startY+h-50, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_FORTIFY_HELP));
	fAllowCancelButton = TRUE;
	fAllowRollButton = FALSE;
	fAllowDoneButton = (RegencePoint != 0);

}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
void PaintContest (LONG x, LONG y, LONG h)
{
	LONG	startY = y;
	LONG	startX = x;
	LONG	provCount = 0;
	LONG	provCount1 = 0;
	LONG	counter = 0;
	LONG	counter1 = 0;
	LONG	lPageDone = FALSE;
	char	n[128];
	LONG	h1, succ;
	LONG 	max_holding = 0;

	RegencePoint = GoldBar = 0;

	del_region(NextPrev,0);

	fAllowCancelButton = !fAllowDoneButton;
	y+=3;
	print_textf(x, y, DKBROWN, "^F00%s",STRMGR_GetStr(STR_ACTNMENU_CAPS_PROVINCE));
	y+=12;
	init_gfont(FONT_SANS_8PT);

	// buttons for all provinces we have holdings in
	for (h1=0; h1<2; ++h1)
	{
		LONG q;
		for ( q = 0; q<2; ++q)
		{
			LONG i;
			for (i=FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)
			{
				LONG k;
				CheckAndFixPlaces((PROVINCE)i);
				LONG j = k = province[i].FirstPlace;
				while (k != 0)
				{
					if (places[j].Realm == HomeRealm)
						break;
					k = places[j].NextPlace;
					j = province[i].FirstPlace + k;
				}
	
				// if we have a holding in prov
				if ( (!q&&province[i].Realm == HomeRealm) || (q && k != 0 && province[i].Realm != HomeRealm))
				{
					if (h1)
					{
						LONG lPrint;
						
						++counter;
						if(counter < startProvCount || lPageDone)
							lPrint = FALSE;
						else
							lPrint = TRUE;
	
						if(lPrint)
						{
							DrawLittleButton(x, y-1, xPROVWIDTH, 10, (iSelectedProvince == i));
							DrawShield(x+2, y, (REALM::REALM_TYPE)province[i].Realm);
							gprint_text(x+16, y, province[i].name, LoyaltyColor[province[i].Loyalty]);
							add_region(x, y, 120, 15, 0, SelectProvince ,i,0,0, -1);
							y+=16;
	
							// print prev, next buttons if provinces are more than 15		
							if(DoPageProvince(&startProvCount, provCount, 15, counter, x, y))
							{
								lPrint = FALSE;
								lPageDone = TRUE;
							}
						}
					}
					else
						++provCount;
				}
			}
		}
	}

	// buttons for all holdings (which are not ours) in the province
	if (iSelectedProvince != 0)
	{
		
		y = startY + 3;
		x += 145;
		print_textf(x, y, DKBROWN, "^F00%s",STRMGR_GetStr(STR_ACTNMENU_HOLDINGS));
		y += 12;

		for (h1=0; h1<2; ++h1)
		{
			LONG tmpIndex;
			LONG i;
			
			CheckAndFixPlaces(iSelectedProvince);
			tmpIndex = i = province[iSelectedProvince].FirstPlace;
			//	i = places[tmpIndex].NextPlace;
			for (; i!=0; i = places[tmpIndex].NextPlace,
						 tmpIndex = province[iSelectedProvince].FirstPlace + i)
			{
				LONG j;
				
				for(j=0; j<4; ++j)								// find the type of it
					if (places[tmpIndex].Icon == InfoReqType[j])
						break;
		
				// if the holding is one of the law, guild, sourse, or temple,
				// and it's not my own holding, add a button.
				if (places[tmpIndex].Realm != HomeRealm && j<4)
				{
					if (h1)
					{
						++counter1;
						if(counter1 < startProvCount1)
						{
							tmpIndex = province[iSelectedProvince].FirstPlace + i;
							continue;
						}
						DrawLittleButton(x, y-1, xPROVWIDTH+35, 10, (iSelectedOneHolding == tmpIndex));
						add_region(x, y, xPROVWIDTH+35, 10, 0, SelectOneHolding, tmpIndex, 0, 0, -1);
			
						print_textf(x+5, y, DKBROWN, "^F02%s", STRMGR_GetStr(gsHoldType[j]));
						sprintf(n, realm[places[tmpIndex].Realm].mfGetName());
						if (places[tmpIndex].Realm < LAND_REALM_COUNT)
						{
							DrawShield(x+58, y, (REALM::REALM_TYPE)places[tmpIndex].Realm);
							print_textf(x+72, y, DKBROWN, "^F01%s", strupr(n));
						}
						else
							print_textf(x+60, y, DKBROWN, "^F01%s", strupr(n));
						y+=16;
						// print prev, next buttons if provinces are more than 8		
						if(DoPageProvince(&startProvCount1, provCount1, 8, counter1, x, y))
							break;
					}
					else
						++provCount1;
				}
		
				// find the highest level of the holdings I own in this province
				if (province[iSelectedProvince].Realm == HomeRealm)
					max_holding = province[iSelectedProvince].CivLevel;
				else
					if (places[tmpIndex].Realm == HomeRealm && (LONG)places[tmpIndex].Level > max_holding)
						max_holding = places[tmpIndex].Level;
		
			}
		}

	}

	if (iSelectedOneHolding != 99)		// holding has been chosen
	{
		LONG	success;
		
  		if(checkGoldReg)
			fAllowRollButton = (ProcessGoldAndReg(CHECK_QUITE))?fAllowRollButton:0;
		else
			fAllowRollButton = (ProcessGoldAndReg(CHECK_ONLY))?fAllowRollButton:0;
	
		// calculate and print the success number
		succ = 10 + places[iSelectedOneHolding].Level - max_holding + modOpponent;
		if (playerstats[ActiveRegent] != fERROR)
		{
			DumbAutoLockPtr< PLAYER_STATS > const pPS(playerstats[ActiveRegent]);
			succ -= pPS->StatsMod.mfGetMod(STATSMOD::CONTEST_MOD);
		}
	 	success = succ - modRegency;

//		if(iSelectedOneHolding == 14)
//			success = 10 + province[iSelectedProvince].CivLevel - max_holding;

		print_textf(x, y + 20, DKBROWN,STRMGR_GetStr(STR_ACTNMENU_F02_SUCCESS));

		if (fLTAction != DURING_LTACTION)
		 	SetupMod(x+66, y+20, &modRegency, 0, succ, -1);
		else
		 	print_textf(x+66, y+20, DKBROWN, "^F02%d%c", D20TOPERCENT(success),PERCENTCHAR);

		// multi-player and AI support
		if(lAskedTargetWhichRoll == iThisRoll + 1)
			success = AskAndWaitForTargetPlayer(CONTEST, 0, iSelectedOneHolding, success, x+75, y + 20);

		// check for success roll
		LONG const result = RollSuccess(success, x+75, y + 20, TRUE);

		if (result == 1)			// success
		{
			// decide which type of holding was selected and deal exp points
			switch ( places[iSelectedOneHolding].Icon )
			{
			// guild
			case GUILD1_ICON:
			case GUILD2_ICON:
			case GUILD3_ICON:
			case GUILD4_ICON:
				ActionExpPoints(ActiveRegent, RLM_EXP_CONTEST_GUILD, 0, TRUE );
				break;
			
			// law
			case LAW1_ICON:
			case LAW2_ICON:
			case LAW3_ICON:
			case LAW4_ICON:
				ActionExpPoints(ActiveRegent, RLM_EXP_CONTEST_LAW, 0, TRUE );
				break;
			
			// source
			case SOURCE1_ICON:
			case SOURCE2_ICON:
			case SOURCE3_ICON:
			case SOURCE4_ICON:
				ActionExpPoints(ActiveRegent, RLM_EXP_CONTEST_SOURCE, 0, TRUE );
				break;
			
			// temples
			case TEMPLE1_ICON:
			case TEMPLE2_ICON:
			case TEMPLE3_ICON:
			case TEMPLE4_ICON:
				ActionExpPoints(ActiveRegent, RLM_EXP_CONTEST_TEMPLE, 0, TRUE );
				break;
			
			// castles and others
			case WIZTOWER_ICON:
			case CASTLE1_ICON:
			case CASTLE2_ICON:
			case CASTLE3_ICON:
			case CASTLE4_ICON:
			
			case VILLAGE_ICON:
			case SEAPORT_ICON:
			case SEAPORT2_ICON:
			case PORT_ICON:
			case RUINS_ICON:
			case SPECIAL1_ICON:
			case SPECIAL2_ICON:
			case SPECIAL3_ICON:
				break;
			}
			
			// multi-player support
			SendResult(CONTEST, 0, iSelectedOneHolding, result);
			SyncRegencySpentInOpposition((REALM::REALM_TYPE)places[iSelectedOneHolding].Realm);
			SetScrewed((REALM::REALM_TYPE)places[iSelectedOneHolding].Realm, HomeRealm, TRUE );

			// -----------------------------------------
			// destroy the holding (place)
			// -----------------------------------------
	 		if (places[iSelectedOneHolding].Level == 0
			   || places[iSelectedOneHolding].Contested == TRUE)
			{
				DeleteHolding(iSelectedProvince,iSelectedOneHolding, TRUE );
				AddReport(IMMEDIATE_CONTEST_DESTROYED, iSelectedProvince, 0, 0); 
			}
			else
			{
				LONG j;
				
				places[iSelectedOneHolding].Contested = TRUE;
				SetGameData(MP_PLACES, MPPLACE_CONTESTED, iSelectedOneHolding, TRUE, TRUE );

				for (j=0; j<4; ++j)
					if(InfoReqType[j]==places[iSelectedOneHolding].Icon)
						break;
//				AddReport(IMMEDIATE_CONTEST, places[iSelectedOneHolding].Realm, j, iSelectedProvince);
			}
	 	}
		else if (result == 2)			// failure
		{
			// multi-player support
			SendResult(CONTEST, 0, iSelectedOneHolding, result);
			SyncRegencySpentInOpposition((REALM::REALM_TYPE)places[iSelectedOneHolding].Realm);
//		 	AddReport(IMMEDIATE_CONTEST_FAIL, iSelectedProvince, 0, 0);
		}
	}

	x = startX + 140;
	y = startY + h - 80;

	RegencePoint = modRegency + 1;
	print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_TOTAL_REG_COST_FMT), RegencePoint); 
	y+=15;
	print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_CONTEST_HELP));

	fAllowRollButton = (iSelectedOneHolding != 99 && !fAllowDoneButton);

	if (fAllowDoneButton)
		DeleteActnMenuRegions();
}


/* ========================================================================
   Function    - PaintLieutenant
   Description - Paint routine for selecting a general purpose lieutenant.
   Returns     -
   ======================================================================== */
void PaintLieutenant(LONG x, LONG y, LONG h)
{
	 LONG 				startY = y;
	 LONG 				id;
	 SHORT				hStat;
	 LONG				charisma = 0;
	 LONG				i, j;
	 LONG				numLieutenants;
	 LONG				iRegent;
	 LONG			   	Gold;
	 LONG 				alignMod[CHARACTER_COUNT];
	 LONG				myAlign;
	 GoldBar = 0;
	 init_gfont(FONT_SANS_8PT);

	 // find myself information and province I'm in
	 iRegent = realm[HomeRealm].mfGetRegent();

	 iSelectedProvince = (PROVINCE) units[regents[iRegent].mfGetunit()].province;
	 id = regents[iRegent].mfGetid();

	 hStat = LoadStats(id, -1);
	 reportFlag = 1;						// if very first time to call this function

	 if (hStat != fERROR)
	 {
		DumbAutoLockPtr< PLAYER_STATS > const pPS(hStat);
		charisma = pPS->mfGetChr();
		myAlign = pPS->mfGetAlignment(); 
	 }

	// count the number of lieutenants you already have.
	numLieutenants = 0;
	for (j = 1;j < MAX_UNITS; ++j)
	{
		if (j != regents[iRegent].mfGetunit()	// not yourself
		    && units[j].Realm == HomeRealm 		// your realm
		    && units[j].id >= 1000)				// Regent or lieutenant.
			++numLieutenants;
	}

	if ((charisma+regents[iRegent].mfGetBL_grade()) < numLieutenants)
	{
		print_textf(x, y, RED, STRMGR_GetStr(STR_ACTNMENU_CANT_HAVE_MORE_LIEUTS));
	}
	else
	{
		CHAR temp1[80];
		CHAR temp2[80];
		LONG count;
		
		print_textf(x, y, DKBROWN, 	   STRMGR_GetStr(STR_ACTNMENU_F00_AVAILABLE_LIEUTENANTS));
		print_textf(x+155, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_CLASS_AND_LEVEL));

		y += 20;
		for (count = 0; count<8; ++count)	// count is number of open slots
		{
			if (iLieutenants[count]!=0)
				break;
		}

		if (count == 8)			// if all 8 slots are open
			reportFlag = 0;		// if we don't have the max number of lts. roll for more

		count = 0;
		if (reportFlag == 0)		// all slots are empty
		{
		 	
			LONG i;
			for (i=69; i<CHARACTER_COUNT; ++i)			// scan all regents
			{
		 		LONG yourAlign;
		 		if (playerstats[i] != fERROR)
		 		{
					DumbAutoLockPtr< PLAYER_STATS > const pPS(playerstats[i]);
		 		  		yourAlign = pPS->mfGetAlignment();
		 		}
				
				alignMod[i] = ((align[yourAlign][myAlign]>50)?(align[yourAlign][myAlign]-100):align[yourAlign][myAlign]);
				alignMod[i]	= alignMod[i]/2;
							
				SHORT tmpId = regents[i].mfGetid();
				// GEH the 1999 test is a validity check
				if(  ((tmpId > 1000 && tmpId < 1999) || (tmpId >= 2000 && tmpId < 3000))
				   && regents[i].mfGetRealm() < LAND_REALM_COUNT
				   && regents[i].mfGetRealm() > 0
				   && regents[i].mfGetunit() == 0		// unit == -1 is dead
				   && count < 8
				   && (regents[i].mfGetRealm() == HomeRealm || random(100) < (20+alignMod[i]))
					)
				{
					init_gfont(FONT_SANS_8PT);
					add_region(x, y, 145, 12, 0, SelectRealm, regents[i].mfGetid(), 0, 0, -1);
					DrawLittleButton(x, y-1, 145, 12, FALSE);
					print_textf(x+16, y, DKBROWN, "%s", regents[i].mfGetname());

					iLieutenants[count] = i;
					++count;
					y+=20;
				}
			}
			reportFlag = 1;
		}

		else		// subsequent frames (after first)
		{
		 	LONG k;
		 	
		 	for (count=0; count < 8; ++count)
			{
				k = iLieutenants[count];

				if (!iCancel)			// only drop and fill at start of action
				{
					// each Lieutenant on list has 10% chance to drop off
					if (k!=0 )
					{
						if (regents[k].mfGetunit()!=0 || random(100) < 10)
						{
							k=0;
							iLieutenants[count]=0;
						}
					}

					if (k==0)    // If the spot is empty, there is a chance a new Lieutenant fill in
					{
						if (random(100)<50)    // if the spot is empty, there is 50% chance a lietenant might fill in
						{
							LONG i;
							for (i=69; i<CHARACTER_COUNT; ++i)			// scan all regents
							{
								LONG j;
								for(j = 0; j<8; ++j)
								{
									if(iLieutenants[j]==i)		// check for if this lieutenant is already on the list
										break;
								}
								if(j != 8)
									continue;

								// This is a Lieutenant who is not on the list currently
								if(regents[i].mfGetRealm() < LAND_REALM_COUNT
									&& regents[i].mfGeticon() >= REG_LORDM1_UNIT_ICON
				   					&& regents[i].mfGetunit() == 0		// unit == -1 is dead
				   					&& (regents[i].mfGetRealm() == HomeRealm || random(100) < (20+alignMod[i])))
								{
									iLieutenants[count] = i;
									k = i;
									break;
								}
							}
						}
					}
				}

				if (k==0)
					continue;	  // if still no one fill in, go to the next one

				init_gfont(FONT_SANS_8PT);
				add_region(x, y, 145, 12, 0, SelectRealm, regents[k].mfGetid(), 0, 0, -1);
				if (iSelectedRealm == regents[k].mfGetid())
				{
					DrawLittleButton(x, y-1, 145, 12, TRUE);
					iSelectedOneHolding = k;
					Gold = (regents[k].mfGetLevel1() > regents[k].mfGetLevel2())?regents[k].mfGetLevel1():regents[k].mfGetLevel2();
//					GoldBar = Gold/2;
					GoldBar = Gold;
				}
				else
					DrawLittleButton(x, y-1, 145, 12, FALSE);

				if(regents[k].mfGetClass2() != NO_CLASS)
				{
					strcpy(temp1, STRMGR_GetStr(gsClass[regents[k].mfGetClass1()]));
					strcpy(temp2, STRMGR_GetStr(gsClass[regents[k].mfGetClass2()])); 
					print_textf(x+165, y, DKBROWN, "%s %d / %s %d", 
						temp1, 
						regents[k].mfGetLevel1(), 
						temp2, 
						regents[k].mfGetLevel2());
				}
				else
				{
					print_textf(x+165, y, DKBROWN, "%s %d", STRMGR_GetStr(gsClass[regents[k].mfGetClass1()]), regents[k].mfGetLevel1());
				}

		 		print_textf(x+12, y, DKBROWN, regents[k].mfGetname());
		 		y += 20;
			}
		}
	}

	iCancel=1;
	y=startY+h-60;
	print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_TOTAL_GB_COST_FMT), GoldBar);
	print_textf(x, y+15, DKBROWN,STRMGR_GetStr(STR_ACTNMENU_CLICK_DONE_2_ACCEPT));

	fAllowCancelButton = TRUE;
	fAllowRollButton = FALSE;
	fAllowDoneButton = (iSelectedRealm != 0);

	if(checkGoldReg)
		fAllowDoneButton = (ProcessGoldAndReg(CHECK_QUITE))?fAllowDoneButton:0;
	else
		fAllowDoneButton = (ProcessGoldAndReg(CHECK_ONLY))?fAllowDoneButton:0;
	
	
	if (IsMenuActive(D_AVATAR_ATTRIBUTES) || IsMenuActive(D_AVATAR_INVENTORY))
		DeleteActnMenuRegions();
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
void PaintHoldAction(LONG MenuCombo, LONG)
{
	LONG	x, y, x1, x2;
	LONG	w, h;
	LONG	MenuId, ButtonId;
	LONG  startY;
	del_region(CancelHoldAction,0);
	del_region(DoneHoldAction,0);

	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	// get position of text area
	if( GetButtonPosition( MenuId, 0, &x, &startY ) == fERROR)
		return;

	// get size of menu
	if( GetButtonSize( MenuId, 0, &w, &h ) == fERROR)
		return;

	print_textf(x+(w/2), startY+((h-24)/2), DKBROWN, STRMGR_GetStr(STR_ACTNMENU_WANNA_DO_NOTHING));

	// draw buttons
	y = startY + h - 24;
	x1 = x + (w/3);
	x2 = x1 + (w/3);

	SHORT iButton = GetResourceStd ("UI\\RELMBTNA.PCX", FALSE);

	DrawBitmap(x1-(88/2), (SHORT)y, iButton, 0, 0, 999, 999);
	add_region(x1-(88/2), y, 88, 20, D_KEY_CANCEL, CancelHoldAction, 0, 0, 0, -1);
	print_textf(x1, y+10, WHITE, "^F02^c%s", STRMGR_GetStr(STR_CANCEL));

	DrawBitmap(x2-(88/2), (SHORT)y, iButton, 0, 0, 999, 999);
 	add_region(x2-(88/2), y, 88, 20, D_KEY_OK, DoneHoldAction, 0, 0, 0, -1);
	print_textf(x2, y+10, WHITE, "^F02^c%s", STRMGR_GetStr(STR_OK));

	if (iButton != fERROR)
		SetPurge(iButton);
}


/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
void PaintInvestiture(LONG x, LONG y, LONG h)
{
	LONG i, me, j, startX, startY, success, result, count, victim;
	CLASS_ENUM type;
	LONG	provCount = 0;
	LONG	counter = 0;

	startX = x;
	startY = y;
	me = realm[HomeRealm].mfGetRegent();
	type = regents[me].mfGetClass1();

	del_region(NextPrev,0);
	fAllowCancelButton = !fAllowDoneButton;

	RegencePoint = GoldBar = 0;

	if(type != PRIEST)
	{
		type = regents[me].mfGetClass2();
	}

	y+=3;
	print_textf(x, y, DKBROWN, "^F00%s",STRMGR_GetStr(STR_ACTNMENU_CAPS_PROVINCES));
	init_gfont(FONT_SANS_8PT);
	y += 16;
	count = 0;

	for (int h1=0; h1<2; ++h1)
	{
		for(i = FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)
		{
			if(province[i].Contested==0 && province[i].OccupRealm == HomeRealm && province[i].Realm != HomeRealm)
			{
				if (h1)
				{
					++counter;
					if(counter < startProvCount)
						continue;

					DrawLittleButton(x, y-1, xPROVWIDTH, 10, (iSelectedProvince == i));
					DrawShield(x+2, y, (REALM::REALM_TYPE)province[i].Realm);
					gprint_text(x+16, y, province[i].name, LoyaltyColor[province[i].Loyalty]);
					add_region(x, y, xPROVWIDTH, 10, 0, SelectProvince,i,0,0, -1);
					y += 16;
					++count;

					// print prev, next buttons if provinces are more than 15		
					if(DoPageProvince(&startProvCount, provCount, 15, counter, x, y))
						break;
				}
				else
					++provCount;
			}
		}
	}

	if(iSelectedProvince != 0)
	{
		x = startX + 160;
		y = startY + 40;

		// take care of the priest performing part, get GoldBar
		if(type != PRIEST)
		{
			for(j=0;j!=CHARACTER_COUNT&&type!=PRIEST;++j)
			{
				if(regents[j].mfGetRealm() == HomeRealm && 
				   regents[j].mfGetid() != regents[me].mfGetid())
				{
					type = regents[j].mfGetClass1();	
					if(type != PRIEST)
					{
						type = regents[me].mfGetClass2();
					}
				}
			}
		}

		if(type != PRIEST)
			GoldBar = 4;
		else
			GoldBar = 1;
		
		fAllowRollButton = TRUE;
  		if(checkGoldReg)
			fAllowRollButton = (ProcessGoldAndReg(CHECK_QUITE))?fAllowRollButton:0;
	   	else
			fAllowRollButton = (ProcessGoldAndReg(CHECK_ONLY))?fAllowRollButton:0;
	
		success = 10 - modRegency + modOpponent;
		if (playerstats[ActiveRegent] != fERROR)
		{
			DumbAutoLockPtr< PLAYER_STATS > const pPS(playerstats[ActiveRegent]);
			success -= pPS->StatsMod.mfGetMod(STATSMOD::INVESTITURE_MOD);

			if (success < 1)
				success=1;
		}
	
		if(success <= 1)
			success = 1;
		if(success >= 20)
			success = 20;

		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_F02_SUCCESS));
		print_textf(x+66, y+6, DKBROWN, "^F02^c%d%c", D20TOPERCENT(success),PERCENTCHAR);
		if (fLTAction != DURING_LTACTION)
		{
			print_textf(x+110, y-16, DKBROWN, 	STRMGR_GetStr(STR_ACTNMENU_F00_cMODIFY_SUCCESS));
			print_textf(x+110, y-8, DKBROWN, 	STRMGR_GetStr(STR_ACTNMENU_cUSING_REGENCY));
			SetupMod(x+105, y, &modRegency, 0, 9+modOpponent, 1);
		}
		RegencePoint = province[iSelectedProvince].CivLevel+modRegency;

		// multi-player and AI support
		if(lAskedTargetWhichRoll == iThisRoll + 1)
			success = AskAndWaitForTargetPlayer(INVESTITURE, iSelectedProvince, 0, success, x+63, y);

		// check for success roll
		result = RollSuccess(success, x+63, y, TRUE);
		if (result == 1)	// success
		{
			// give experience points
			ActionExpPoints(ActiveRegent, RLM_EXP_INVEST, 0, TRUE );
			// multi-player support
			j = province[iSelectedProvince].Realm;
			SetScrewed((REALM::REALM_TYPE)j, HomeRealm, TRUE );
			SetScrewed((REALM::REALM_TYPE)j, HomeRealm, TRUE ); // double the hate from victim

			SendResult(INVESTITURE, iSelectedProvince, 0, result);
			SyncRegencySpentInOpposition((REALM::REALM_TYPE)province[iSelectedProvince].Realm);

			victim = province[iSelectedProvince].Realm;
			//province[iSelectedProvince].Realm = HomeRealm;
			SetGameData(MP_PROVINCE, MPPROV_REALM, iSelectedProvince, HomeRealm, TRUE );

			//province[iSelectedProvince].OccupRealm = 0;
			SetGameData(MP_PROVINCE, MPPROV_OCCUPREALM, iSelectedProvince, 0, TRUE );

			//ReconstructMap();
			//CheckRealmStatus(HomeRealm, province[iSelectedProvince].Realm, TRUE );
			SetGameData(MP_PROVINCE, MPPROV_INVESTED, HomeRealm, victim, TRUE );

//			AddReport(IMMEDIATE_INVESTITURE, iSelectedProvince, 0, 0);	
		}

		if (result == 2)	// failure
		{
			// multi-player support
			SendResult(INVESTITURE, iSelectedProvince, 0, result);
			SyncRegencySpentInOpposition((REALM::REALM_TYPE)province[iSelectedProvince].Realm);
//			AddReport(IMMEDIATE_INVESTITURE_FAIL, 0, 0, 0);	
		}

	}
	
	y = startY + 19;
	if(GoldBar == 0 && count == 0 && !fAllowDoneButton)
	{
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_U_HAVENT_CONQUERED_ANYTHING));
		return;
	}

	x = startX;
	y = startY+h-45;
	print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_TOTAL_COST_FMT), GoldBar, RegencePoint);

	if (fAllowDoneButton)
		DeleteActnMenuRegions();
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
void PaintRealmSpell(LONG x, LONG y, LONG h)
{
	LONG			i, h1, j, q, level, slevel, count, tmpIndex, distance;
	LONG			MaxLevel, iUnit;
	LONG			startX = x;
	LONG			flag = 0;
	LONG			startY = y;
	LONG			id, hStat;
	LONG			item;
	SHORT			result = 0;
	SHORT			spellindex;
	SHORT			currentRegent;
	LONG		provCount = 0;
	LONG		provCount1 = 0;
	LONG		provCount2 = 0;
	LONG		counter = 0;
	LONG		counter1 = 0;
	LONG		counter2 = 0;
	LONG		baseLevel, rate, maxamt;
	LONG		disX, disY;
	CLASS_ENUM	type;
	RegencePoint = 0;
	GoldBar = 0;
	GoldGained = 0;

	del_region(NextPrev,0);

	if(fLTAction == DURING_LTACTION)
		currentRegent = ActiveRegent;
	else
		currentRegent = realm[HomeRealm].mfGetRegent();

	// check for wizard or priest
	type = regents[currentRegent].mfGetClass1();
	level = regents[currentRegent].mfGetLevel1();
	if(type != WIZARD && type != PRIEST && type != BARD/*bard*/)
	{
		type = regents[currentRegent].mfGetClass2();
		level = regents[currentRegent].mfGetLevel2();
	}
	id = regents[currentRegent].mfGetid();
	
	if(type != WIZARD && type != PRIEST && type != BARD)
		return;
		
	hStat = LoadStats(id, -1);
	if(hStat == fERROR)
		 return;
	
	DumbAutoLockPtr< PLAYER_STATS > const pStat(hStat);

	// select a realm
	if (iSelectedRealm == 0)
	{
		modLevel = 0;
		// gather info about sources and temples
		for (i=FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)			// scan all provinces
		{
			CheckAndFixPlaces((PROVINCE)i);
			tmpIndex = j = province[i].FirstPlace;
			while (j != 0)							// scan for a SOURCE we own
			{
				if (places[tmpIndex].Realm==HomeRealm
					&& (	((type == WIZARD || type == BARD)&& places[tmpIndex].Icon == SOURCE1_ICON)
						||	(type == PRIEST && places[tmpIndex].Icon == TEMPLE1_ICON)
					)	)
				{
					iSelectedHolding[province[i].Realm][0] = TRUE;	// mark realm
					// province with source or temple index
					iSelectedHolding[i][1] = tmpIndex;
					iSelectedHolding[i][2] = tmpIndex/256;
				}
				j = places[tmpIndex].NextPlace;
				tmpIndex = province[i].FirstPlace + j;
			}
		}

		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_CAPS_REALMS));
		y += 15;
		init_gfont(FONT_SANS_8PT);
		if (iSelectedHolding[HomeRealm][0])		// check for homerealm first
		{
			DrawLittleButton(x, y-1, 145, 12, FALSE);
			DrawShield(x+2, y, HomeRealm);
			gprint_text(x+16, y, realm[HomeRealm].mfGetName(), DKBROWN);
			add_region(x, y, 145, 12, 0, SelectRealm,HomeRealm,0,0, -1);
			y+=18;
		}
		for (i=1; i < LAND_REALM_COUNT; ++i)
		{

			if (!realm[i].mfExists() || i==REALM::MONSTERS || i==HomeRealm)
				continue;

			if ((y-(startY+15))>=240)			// two columns
			{
				y = startY+15;
				x+=160;
			}

			if (iSelectedHolding[i][0])		// check for marked realms
			{
				DrawLittleButton(x, y-1, 145, 12, FALSE);
				DrawShield(x+2, y, (REALM::REALM_TYPE)i);
				gprint_text(x+16, y, realm[i].mfGetName(), DKBROWN);
				add_region(x, y, 145, 12, 0, SelectRealm,i,0,0, -1);
				y+=18;
			}
		}
		if (y == startY+15 && x == startX)
		{
			if (type == WIZARD)
				gprint_text(x, y,STRMGR_GetStr(STR_ACTNMENU_COOLNESS_FOR_WIZARD) , DKBROWN);
			else if (type == BARD)
				gprint_text(x, y,STRMGR_GetStr(STR_ACTNMENU_COOLNESS_FOR_BARD) , DKBROWN);
			else
				gprint_text(x, y,STRMGR_GetStr(STR_ACTNMENU_COOLNESS_FOR_PRIEST) , DKBROWN);
		}





	}

	// print selected realm
	if (iSelectedRealm != 0)
	{
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_CAPS_REALMS));
		y += 15;
		init_gfont(FONT_SANS_8PT);
		DrawLittleButton(x, y-1, 145, 12, TRUE);
		DrawShield(x, y, (REALM::REALM_TYPE)iSelectedRealm);
		gprint_text(x+14, y, realm[iSelectedRealm].mfGetName(), DKBROWN);
		add_region(x, y, 145, 12, 0, SelectRealm,iSelectedRealm,0,0, -1);
		y+=18;
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_F00_PROVINCES));
		y += 15;
		init_gfont(FONT_SANS_8PT);
	}

	// select a province
	if (iSelectedRealm != 0 && iSelectedProvince == 0)
	{
		for (h1=0; h1<2; ++h1)
		{
			for (i=FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)
			{
				tmpIndex = iSelectedHolding[i][1] + (iSelectedHolding[i][2]*256);
				
				if (province[i].Realm==iSelectedRealm&&tmpIndex!=0)
				{
					if (h1)
					{
						++counter;
						if(counter < startProvCount)
							continue;
	
						DrawLittleButton(x, y-1, 145, 12, FALSE);
						DrawShield(x, y, (REALM::REALM_TYPE)province[i].Realm);
						init_gfont(FONT_SANS_8PT);
						if (province[i].Realm==HomeRealm)
				   		gprint_text(x+14, y, province[i].name, LoyaltyColor[province[i].Loyalty]);
						else
				   		gprint_text(x+14, y, province[i].name, DKBROWN);
						if (type==WIZARD || type == BARD)
							print_textf(x+160, y+3,DKBROWN, "^F00%s: ^F02%d", STRMGR_GetStr(STR_ACTN_SOURCE),LargestSource((PROVINCE)i, tmpIndex, HomeRealm));	// wizard
						else
							print_textf(x+160, y+3,DKBROWN, "^F00%s: ^F02%d", STRMGR_GetStr(STR_ACTN_TEMPLE),places[tmpIndex].Level);			// priest
						if (province[i].Dispel > 0)
							print_textf(x+210, y+3,DKBROWN, "^F00%s: ^F02%d", STRMGR_GetStr(STR_ACTN_DISPELL),province[i].DispelLvl);
						add_region(x, y, 145, 12, 0, SelectProvince,i,0,0, -1);
						y+=18;

						// print prev, next buttons if provinces are more than 13		
						if(DoPageProvince(&startProvCount, provCount, 13, counter, x, y))
							break;
					}
					else
						++provCount;
				}
			}
		}
	}

	// print selected province
	if (iSelectedProvince != 0)
	{
		tmpIndex = iSelectedHolding[iSelectedProvince][1] + (iSelectedHolding[iSelectedProvince][2]*256);
		DrawLittleButton(x, y-1, 145, 12, TRUE);
		DrawShield(x, y, (REALM::REALM_TYPE)province[iSelectedProvince].Realm);
		if (province[iSelectedProvince].Realm==HomeRealm)
	  		gprint_text(x+14, y, province[iSelectedProvince].name, LoyaltyColor[province[iSelectedProvince].Loyalty]);
		else
	  		gprint_text(x+14, y, province[iSelectedProvince].name, DKBROWN);
		add_region(x, y, 145, 12, 0, SelectProvince,iSelectedProvince,0,0, -1);

		if (type==WIZARD || type == BARD)	// get slevel (source or temple level)
			slevel = LargestSource(iSelectedProvince, tmpIndex, HomeRealm);
		else
			slevel = places[tmpIndex].Level;

		if (iSelectedOneHolding == 99)	// no spell selected yet
		{
			print_textf(x+160, y+3,DKBROWN, "^F00%s: ^F02%d", STRMGR_GetStr((type==WIZARD || type == BARD)? STR_ACTN_SOURCE:STR_ACTN_TEMPLE), slevel);
			if (province[iSelectedProvince].Dispel > 0)
			{
				print_textf(x+210, y+3,DKBROWN, "^F00%s: ^F02%d", STRMGR_GetStr(STR_ACTN_DISPELL), province[iSelectedProvince].DispelLvl);
				print_textf(startX+160, startY+h-71, DKBROWN, STRMGR_GetStr(STR_ACTN_DISPELL_FORMAT), province[iSelectedProvince].name, province[iSelectedProvince].DispelLvl); 
			}
		}
		y+=18;
	}

	// select spell
	if (iSelectedProvince != 0 && iSelectedOneHolding == 99)	// iSelectedOneHolding starts as 99
	{
		print_textf(x, y, DKBROWN, "^F00%s", STRMGR_GetStr(STR_ACTN_SPELLS));
		print_textf(x+125, y+4, DKBROWN, STRMGR_GetStr((type==5)?STR_ACTN_REQ_SOURCE:STR_ACTN_REQ_TEMPLE));
		y += 15;
		init_gfont(FONT_SANS_8PT);

		InventoryItor itor(pStat->Inventory);

		// GWP pStat->Inventory.mfReset(IC_REALM_SPELL);
		// GWP while(!pStat->Inventory.mfEndOfList())

		for (itor.mfAdvance(IC_REALM_SPELL);
			 itor != pStat->Inventory.end();
			 itor.mfAdvanceToNext(IC_REALM_SPELL))
		{

		 	// GWP InventoryNode& dataIndex = pStat->Inventory.mfData();
			// GWP item = dataIndex.mfType();

			item = (*itor)->mfType();
			spellindex=ItemTypes[item].mfGetExtendedData();
			if((REALM_SPELL_INFO::TYPE)spellindex == REALM_SPELL_INFO::INVESTITURE)
				continue;

			REALM_SPELL_INFO::REALM_SPELL_ITEM	const &realmspell =REALM_SPELL_INFO::mfGetRealmSpell((REALM_SPELL_INFO::TYPE) spellindex);

			if (realmspell.mfGetSource() <= slevel ||
				((REALM_SPELL_INFO::TYPE) spellindex == REALM_SPELL_INFO::TRANSPORT &&
				pStat->StatsMod.mfGetMod(STATSMOD::TRANSPORT_MOD) > 0))
			{
				if(!(province[iSelectedProvince].CivLevel == 0 && item == ITEM_SPELL_DEATH_PLAGUE)) 
			 	{
					if(iSelectedOneHolding==item)
				 		DrawLittleButton(x, y-1, 145, 12, TRUE);
				 	else
				 		DrawLittleButton(x, y-1, 145, 12, FALSE);
				 	add_region(x, y, 145, 12, 0, SelectOneHolding, (SHORT)item, 0, 0, -1);
				}
			}
			else
			{
				color_rect(x, y-1, 145, 12, LTTAN-4);
				print_textf(x+160, y-1, DKRED, STRMGR_GetStr(STR_ACTN_INSUFFICIENT_FMT),(type==WIZARD || type == BARD)?"SOURCE":"TEMPLE");
				print_textf(x+160, y+5, DKRED, STRMGR_GetStr(STR_ACTN_LEVEL_TO_CAST));
			}

			print_textf(x+2, y, DKBROWN, "^F02%s", realmspell.mfGetName());
			print_textf(x+125, y+6, DKBROWN, "^c%d", realmspell.mfGetSource());

			y+=16;

			// GWP pStat->Inventory.mfAdvance(IC_REALM_SPELL);
		}
		if(province[iSelectedProvince].Dispel>0)
			RegencePoint += province[iSelectedProvince].DispelLvl+1;
	}

	// display selected spell
	if (iSelectedOneHolding != 99)	// iSelectedOneHolding starts as 99
	{
		print_textf(x, y, DKBROWN, "^F00SPELL");
		y += 15;

//		// GWP pStat->Inventory.mfReset(IC_REALM_SPELL);
//		// GWP while(!pStat->Inventory.mfEndOfList())
//
//		InventoryItor itor(pStat->Inventory);
//		
//		for (itor.mfAdvance(IC_REALM_SPELL);
//		     itor != pStat->Inventory.end();
//		     itor.mfAdvanceToNext(IC_REALM_SPELL))
//		{
//			// GWP InventoryNode& dataIndex = pStat->Inventory.mfData();
//			// GWP item = dataIndex.mfType();
//			
//			item = (*itor)->mfType();
//
//			if((SHORT)item == iSelectedOneHolding)
//			{

		item = iSelectedOneHolding;
		spellindex=ItemTypes[item].mfGetExtendedData();
	 	REALM_SPELL_INFO::REALM_SPELL_ITEM	const &realmspell=REALM_SPELL_INFO::mfGetRealmSpell((REALM_SPELL_INFO::TYPE) spellindex);

		init_gfont(FONT_SANS_8PT);
		DrawLittleButton(x, y-1, 145, 12, TRUE);
		print_textf(x+2, y, DKBROWN, "%s", realmspell.mfGetName());
		print_textf(x+125, y, DKBROWN, "%d", realmspell.mfGetSource());
		add_region(x, y, 145, 12, 0, SelectOneHolding,iSelectedOneHolding,0,0, -1);
		y+=18;
		switch((ITEMTYPE)iSelectedOneHolding)
 	 	{
			case ITEM_SPELL_DEATH_PLAGUE:
			case ITEM_SPELL_WARDING:
	 	 		baseLevel = 5;
				rate = 2;
	 	 		break;
	 	 	case ITEM_SPELL_DEMAGOGUE:
			case ITEM_SPELL_HONEST_DEALING:
	 	 		baseLevel = 3;
				rate = 2;
	 	 		break;
			case ITEM_SPELL_MASS_DESTRUCTION:
				baseLevel = 1;
				rate = 3;
				break;
			case ITEM_SPELL_BLESS_LAND:
			case ITEM_SPELL_BLIGHT:
				baseLevel = 1;
				rate = 2;
				break;
			case ITEM_SPELL_BLESS_ARMY:
				baseLevel = 1;
				rate = 1;
			default:
				baseLevel = 0;
				rate = 1;
		}
		maxamt = (level - baseLevel) / rate;
		switch((ITEMTYPE)iSelectedOneHolding)
		{
			case ITEM_SPELL_ALCHEMY:
				x = startX + 160;
				y = startY + 33;
				print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTN_MAKE_GOLD));
				SetupMod(x+100, y, &modGoldBars, 0, 100, 1); 	// use iSelectedholding[i][0] as castle holding
				GoldGained = modGoldBars;
				y += 15;
				init_gfont(FONT_SANS_8PT);
				RegencePoint = GoldGained * 4;
				print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTN_REGENCY_COST), RegencePoint);
				y += 15;
				print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTN_GOLD_COST));
				if(GoldGained != 0)
					GoldBar = 1;

				break;

			case ITEM_SPELL_DEATH_PLAGUE:
				init_gfont(FONT_SANS_6PT);
				print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTN_SELECT_ADD_PROV));
				y += 15;
				init_gfont(FONT_SANS_8PT);
				RegencePoint = province[iSelectedProvince].CivLevel;
				GoldBar = 2;
				for (count = 0;
						count<7 && province[iSelectedProvince].Border[count].Province!=0;
							++count)
				{
					tmpIndex=province[iSelectedProvince].Border[count].Province;
					if(province[tmpIndex].CivLevel != 0)
					{
						if (iSelectedMulProvince[count])
						{
							RegencePoint += province[tmpIndex].CivLevel;
							DrawLittleButton(x, y-1, 145, 12, TRUE);
							if(province[tmpIndex].Dispel>0)
								RegencePoint += province[tmpIndex].DispelLvl+1;	
						}
						else
							DrawLittleButton(x, y-1, 145, 12, FALSE);
					}
					DrawShield(x+2, y, (REALM::REALM_TYPE)province[tmpIndex].Realm);
			   	gprint_text(x+16, y, province[tmpIndex].name, LoyaltyColor[province[tmpIndex].Loyalty]);
					if(province[tmpIndex].Dispel>0)
					{
						print_textf(x+115, y,LoyaltyColor[province[tmpIndex].Loyalty] ,  "D%d", province[tmpIndex].DispelLvl);
					}
					add_region(x, y, 145, 12, 0, SelectMulProvince,count,level,0, -1);
					y+=18;
				}
				init_gfont(FONT_SANS_8PT);
				print_textf(startX + 160, startY + h - 73, DKBROWN, STRMGR_GetStr(STR_ACTN_YOU_CAN_CHOOSE)); 
				print_textf(startX + 160, startY + h - 60, DKBROWN, STRMGR_GetStr(STR_ACTN_AN_ADDITIONAL_PROV), maxamt-additionalamt); 
				break;

			case ITEM_SPELL_DEMAGOGUE:
				init_gfont(FONT_SANS_6PT);
				print_textf(x, y, DKBROWN, "^F00 SELECT ADDITIONAL PROVINCES");
				y += 15;
				init_gfont(FONT_SANS_8PT);
				for(count = 0;
					count < 7 && province[iSelectedProvince].Border[count].Province != 0;
					++count )
				{
					tmpIndex=province[iSelectedProvince].Border[count].Province;

					if(iSelectedMulProvince[count])
					{
						DrawLittleButton(x, y-1, 145, 12, TRUE);
						print_textf(x+150, y+2, DKBROWN, "^F01^B%s", STRMGR_GetStr(gsLoyaltyUC[province[tmpIndex].Loyalty+(iSelectedHolding[count][0]==2)-(iSelectedHolding[count][0]==3)]));
						if(province[tmpIndex].Loyalty < 3)
						{
							DrawLittleButton(x+200, y+1, 25, 8, (iSelectedHolding[count][0]==2 || iSelectedHolding[count][0]==4));
							add_region(x+200, y+1, 25, 8, 0, SetNum,(LONG)&iSelectedHolding[count][0],2,0, -1);
						}
						print_textf(x+200+13, y+6, DKBROWN, "^F00^c%s", STRMGR_GetStr(STR_ACTN_RAISE));
						if(province[tmpIndex].Dispel>0)
							RegencePoint += province[tmpIndex].DispelLvl+1;	
						if(province[tmpIndex].Loyalty > 0)
						{
							DrawLittleButton(x+250, y+1, 25, 8, (iSelectedHolding[count][0]==3 || iSelectedHolding[count][0]==5));
							add_region(x+250, y+1, 25, 8,0, SetNum,(LONG)&iSelectedHolding[count][0],3,0, -1);
						}
						print_textf(x+250+13, y+6, DKBROWN, "^c%s", STRMGR_GetStr(STR_ACTN_LOWER));

					}
					else
						DrawLittleButton(x, y-1, 145, 12, FALSE);

					init_gfont(FONT_SANS_8PT);
					DrawShield(x+2, y, (REALM::REALM_TYPE)province[tmpIndex].Realm);
			   	gprint_text(x+16, y, province[tmpIndex].name, LoyaltyColor[province[tmpIndex].Loyalty]);
					if(province[tmpIndex].Dispel>0)
					{
						print_textf(x+115, y,LoyaltyColor[province[tmpIndex].Loyalty] ,  "D%d", province[tmpIndex].DispelLvl);
					}
					add_region(x, y, 145, 12, 0, SelectMulProvince,count,level,0, -1);

					y+=18;
				}

				y = startY + 48;
				print_textf(x+150, y+2, DKBROWN, "^F01^B%s", STRMGR_GetStr(gsLoyaltyUC[province[iSelectedProvince].Loyalty+(iSelectedHolding[7][0]==2)-(iSelectedHolding[7][0]==3)]));
				if(province[iSelectedProvince].Loyalty < 3)
				{
					DrawLittleButton(x+200, y+1, 25, 8, (iSelectedHolding[7][0]==2));
					add_region(x+200, y+1, 25, 8, 0, SetNum,(LONG)&iSelectedHolding[7][0],2,0, -1);
				}
				print_textf(x+200+13, y+6, DKBROWN, "^F00^c%s", STRMGR_GetStr(STR_ACTN_RAISE));
				if(province[iSelectedProvince].Loyalty > 0)
				{
					DrawLittleButton(x+250, y+1, 25, 8, (iSelectedHolding[7][0]==3));
					add_region(x+250, y+1, 25, 8,0, SetNum,(LONG)&iSelectedHolding[7][0],3,0, -1);
				}
				print_textf(x+250+13, y+6, DKBROWN, "^c%s", STRMGR_GetStr(STR_ACTN_LOWER));

				for(count = 0; count < 8; ++count)
				{
					RegencePoint += abs(((iSelectedHolding[count][0]==2)-(iSelectedHolding[count][0]==3))*5);
				}
				if(RegencePoint != 0) GoldBar +=1;
				init_gfont(FONT_SANS_8PT);
				print_textf(startX + 160, startY + h - 73, DKBROWN, STRMGR_GetStr(STR_ACTN_YOU_CAN_CHOOSE)); 
				print_textf(startX + 160, startY + h - 60, DKBROWN, STRMGR_GetStr(STR_ACTN_AN_ADDITIONAL_PROV), maxamt- additionalamt); 

				break;

			case ITEM_SPELL_LEGION_OF_DEAD:
				init_gfont(FONT_SANS_8PT);
				y += 20;
				print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTN_SUMMON_SKELETON));
				RegencePoint = 4;
				GoldBar = 1;
				break;

			case ITEM_SPELL_MASS_DESTRUCTION:
				count = 0;
				y = startY; 
				x = startX + 160;
				print_textf(x, y, DKBROWN, "^F00%s", STRMGR_GetStr(STR_ACTN_UNITS));
				y += 15;
				maxamt = (level+3-baseLevel)/rate;
	 			for (h1=0; h1<2; ++h1)
				{		
					for (i = 1; i < MAX_UNITS && count < 12; ++i)
					{
						if(units[i].province == iSelectedProvince
			 			   && units[i].Joined == 0
							&& realm[units[i].Realm].mfExists()
			 			   && units[i].Realm != HomeRealm)
						{
							for(iUnit = i; iUnit > 0; iUnit = units[iUnit].NextUnit)
							{
								if (h1)
								{
									++counter1;
									if(counter1 < startProvCount1)
										continue;
				
									if(iSelectedMulProvince[count])
									{
										DrawLittleButton(x, y-1, 145, 12, TRUE);
										GoldBar = 5;
										RegencePoint += 10;
									}
									else
										DrawLittleButton(x, y-1, 145, 12, FALSE);
				
									DrawShield(x+2, y, (REALM::REALM_TYPE)units[iUnit].Realm);
									init_gfont(FONT_SANS_8PT);
									print_textf(x+16, y, DKBROWN, "%s", UnitName(iUnit));
									add_region(x, y, 145, 12,0, SelectMulProvince, count, level+3, 0, -1);	
									++count;
									
									y += 15;
			
									// print prev, next buttons if number of units are more than 10
									if(DoPageProvince(&startProvCount1, provCount1, 8, counter1, x, y))
										goto md_exit;
								}
								else
									++provCount1;
							}
						}
					}
				}
				
md_exit: 		init_gfont(FONT_SANS_8PT);
				print_textf(startX + 160, startY + h - 73, DKBROWN, STRMGR_GetStr(STR_ACTN_YOU_CAN_CHOOSE)); 
				print_textf(startX + 160, startY + h - 60, DKBROWN, STRMGR_GetStr(STR_ACTN_ADDITIONAL_UNITS), maxamt- additionalamt); 

				break;

			case ITEM_SPELL_RAZE:
				CheckAndFixPlaces(iSelectedProvince);
				count = j = province[iSelectedProvince].FirstPlace;
				while (j != 0)
				{
					if(places[count].Icon == CASTLE1_ICON ||
						places[count].Icon == CASTLE2_ICON ||
						places[count].Icon == CASTLE3_ICON ||
						places[count].Icon == CASTLE4_ICON)
					{
						DrawShield(x+2, y, (REALM::REALM_TYPE)places[count].Realm);
						print_textf(x+16, y, DKBROWN, "^F00%s", STRMGR_GetStr(STR_ACTN_CASTLE));

						print_textf(x+100, y+6, DKBROWN, "^F02^c%d", places[count].Level+modLevel);
						init_gfont(FONT_SANS_5PT);
						x += 111;
						// for plus
						if (modLevel<0 )
						{
							DrawLittleButton(x, y-1, 10, 4, FALSE);
							add_region(x-6, y-3, 18, 8, 0, IncNumWithSign, modLevel, 0, 0, -1);
						}
						else
							color_rect(x-1, y-2, 10, 4, LTTAN-4);
						print_textf(x+3, y-2, DKBROWN, "+");

						// for minus
						if(places[count].Level > (-modLevel))
						{
							DrawLittleButton(x, y+6, 10, 4, FALSE);
							add_region(x-6, y+4, 18, 8, 0, DecNumWithSign, modLevel, 0, 0, -1);
						}
						else
							color_rect(x-1, y+6, 10, 4, LTTAN-4);
						print_textf(x+4, y+5, DKBROWN, "-");
						x = startX;
						init_gfont(FONT_SANS_8PT);

					}
					j = places[count].NextPlace;
					count = province[iSelectedProvince].FirstPlace + j;
				}
				RegencePoint = -modLevel * 10;
				GoldBar = -modLevel * 2;
				break;

			case ITEM_SPELL_SCRY:
				count = 0;
				GoldBar = 5;
				RegencePoint = 10;
				if(checkGoldReg)
	   				fAllowRollButton = (ProcessGoldAndReg(CHECK_QUITE))?fAllowRollButton:0;
				else
	   				fAllowRollButton = (ProcessGoldAndReg(CHECK_ONLY))?fAllowRollButton:0;
				GoldBar = 0;
				RegencePoint = 0;
				result = RollSuccess(1, x, y, TRUE);
				fAllowDoneButton = 0;
				if (result == 1 || result == 2)	// success
				{
					fAllowDoneButton = 1;
					for(i = 1; i< MAX_UNITS; ++i)
					{
						if (units[i].Realm != HomeRealm
							&& realm[units[i].Realm].mfExists()
							&& units[i].NotSpotted == TRUE)
						{
							disX = province[units[i].province].x-province[iSelectedProvince].x;
							disY = province[units[i].province].y-province[iSelectedProvince].y;
							distance =(LONG)sqrt(disX * disX + disY * disY);
							if(random(600) > distance)
							{
								// Only needs to be done locally
								units[i].NotSpotted = FALSE;
								if( y > (startY + h - 70) && x == startX)
								{
									y = startY; 
									x += 160;
 								}	
								if(count < 17)
								{
									iSelectedMulUnits[count]=i;
									DrawShield(x+2, y, (REALM::REALM_TYPE)units[i].Realm);
									print_textf(x+16, y, DKBROWN, "%s", UnitName(i) );
									print_textf(x+40, y+12, DKBROWN, "%s", province[units[i].province].name);
									y += 24;
								}
//								SetGameData(MP_UNITS, MPUNITS_NOTSPOTTED, i, FALSE, TRUE );
								++j;
								++count;
							}
							++j;
						}
					}
					modLevel = count;
					print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTN_YOU_HAVE_SPOTTED), count); 
					y+=12;
					print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTN_HIDDEN_UNITS));
					GoldBar = 5;
					RegencePoint = 10;

				}
				else if(result == 3)
				{
					fAllowDoneButton = 1;
					if(modLevel != 0)
					{
						for(i=0; i<modLevel&&i<17; ++i)
						{
							if( y > (startY + h - 70) && x == startX)
							{
								y = startY; 
								x += 160;
 							}	
							DrawShield(x+2, y, (REALM::REALM_TYPE)units[iSelectedMulUnits[i]].Realm);
							print_textf(x+16, y, DKBROWN, "%s", UnitName(iSelectedMulUnits[i]) );
							print_textf(x+40, y+12, DKBROWN, "%s", province[units[iSelectedMulUnits[i]].province].name);
							y += 24;
						}	
					}
					print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTN_YOU_HAVE_SPOTTED), modLevel); 
					y+=12;
					print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTN_HIDDEN_UNITS));
					GoldBar = 5;
					RegencePoint = 10;
				}
				else
				{
					print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTN_READY_TO_SPOT));
				}
				break;

			case ITEM_SPELL_STRONGHOLD:
				CheckAndFixPlaces(iSelectedProvince);
				count = j = province[iSelectedProvince].FirstPlace;
				MaxLevel = 0;
				while (j != 0)
				{
					if(places[count].Icon == WIZTOWER_ICON)
					{
						if(places[count].Level >= province[iSelectedProvince].CivLevel)
							MaxLevel=TRUE;
						print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTN_WIZARD_TOWER_LEVEL));
						y += 20;
						print_textf(x, y, DKBROWN, "%d", places[count].Level+iSelectedMulProvince[0]);
						if(iSelectedMulProvince[0])
						{
							if(!MaxLevel)
								DrawLittleButton(x+25, y-1, 50, 12, TRUE);
							print_textf(x+25, y, 50, "^F00%s", STRMGR_GetStr(STR_ACTN_RAISE));
							GoldBar = 1;
							RegencePoint = 5;
						}
						else
						{
							if(!MaxLevel)
								DrawLittleButton(x+25, y-1, 50, 12, FALSE);
						}	
						print_textf(x+25, y, DKBROWN, "^F00%s", STRMGR_GetStr(STR_ACTN_RAISE));
			   			if(!MaxLevel)
							add_region(x+25, y, 50, 12,0, SelectMulProvince, 0, 5, 0, -1); // iSelectedMulProvince[0] for improve an old tower	
						y += 18;
						break;
					}
					j = places[count].NextPlace;
					count = province[iSelectedProvince].FirstPlace + j;
				}
				if (j == 0)
				{
					print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTN_CREATE_WIZARD_TOWER));
					y += 20;
					if(iSelectedMulProvince[1])
					{
						DrawLittleButton(x, y-1, 50, 12, TRUE);
						GoldBar = 1;
						RegencePoint = 5;
					}
					else
					{
						DrawLittleButton(x, y-1, 50, 12, FALSE);
					}
					print_textf(x, y, DKBROWN, "^F00%s", STRMGR_GetStr(STR_ACTN_YES));
					add_region(x, y, 50, 12, 0, SelectMulProvince, 1, 10, 0, -1); // iSelectedMulProvince[1] for create new tower
				}
				break;

			case ITEM_SPELL_SUMMONING:
				print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTN_SUMMON_MONSTER_UNITS));
				y += 20;
				
				for(i = 0; i<=(level-1)/3; ++i)
				{	
					if(i>3)
						break;
					if(iSelectedMulProvince[i])
					{
						DrawLittleButton(x, y-1, 145, 12, TRUE);
						GoldBar += 2;
						RegencePoint += 5;
					}
					else
						DrawLittleButton(x, y-1, 145, 12, FALSE);

					switch(i)
					{
						case 0:	
							print_textf(x+2, y, DKBROWN, "^F02%s", STRMGR_GetStr(STR_UNIT_NAME_GOB));
							break;
						case 1:
							print_textf(x+2, y, DKBROWN, "^F02%s", STRMGR_GetStr(STR_UNIT_NAME_GNIRR));
							break;
						case 2:
							print_textf(x+2, y, DKBROWN, "^F02%s", STRMGR_GetStr(STR_UNIT_NAME_GNINF));
							break;
						case 3:
							print_textf(x+2, y, DKBROWN, "^F02%s", STRMGR_GetStr(STR_UNIT_NAME_SKEL));
							break;
					}
					add_region(x, y, 145, 12,0, SelectMulProvince, i, 10, 0, -1);	
					y += 18;
				}
				maxamt = (level-1)/3;
				if(maxamt > 3)
					maxamt = 3;
				init_gfont(FONT_SANS_8PT);
				print_textf(startX + 160, startY + h - 73, DKBROWN, STRMGR_GetStr(STR_ACTN_YOU_CAN_CHOOSE)); 
				print_textf(startX + 160, startY + h - 60, DKBROWN, STRMGR_GetStr(STR_ACTN_ADDITIONAL_UNITS), maxamt+1-additionalamt); 

				break;

			case ITEM_SPELL_TRANSPORT:
				LONG test_prov;
				modLevel=level;
				if(pStat->StatsMod.mfGetMod(STATSMOD::TRANSPORT_MOD) > 0 && modLevel < 15)
					modLevel = 15;
				count = 0;
				flag = 0;
				modRegency = 0;
				print_textf(x, y, DKBROWN, "^F00%s", STRMGR_GetStr(STR_ACTN_UNITS));
				init_gfont(FONT_SANS_8PT);
				y += 20;
				for (h1=0; h1<2; ++h1)
				{
					count = 0;
				 	for(i = 1; i<MAX_UNITS; ++i)
					{
						if(count >= 19) 
							continue;
						if (units[i].province == iSelectedProvince
							&& realm[units[i].Realm].mfExists()
						   && units[i].Joined == 0)
						{
							for(iUnit = i; iUnit > 0; iUnit = units[iUnit].NextUnit)
							{
								if (h1)
								{
									++counter1;
									++count;

									if(counter1 < startProvCount1 || count >= 20)
										continue;
		
									if(iSelectedMulUnits[count]!=0)	
									{
										DrawLittleButton(x, y-1, 145, 12, TRUE);
									}
									else
										DrawLittleButton(x, y-1, 145, 12, FALSE);
			
									DrawShield(x+2, y, (REALM::REALM_TYPE)units[iUnit].Realm);
					 	 			print_textf(x+16, y, DKBROWN, "%s", UnitName(iUnit));
									add_region(x, y, 145, 12, 0, SelectMulUnits,iUnit,count,0, -1);
									y+=18;
				
									// print prev, next buttons if number of units are more than 10
									if(DoPageProvince(&startProvCount1, provCount1, 8, counter1, x, y))
										goto trans_exit;
								}
								else
								{
									if(provCount1 < 19)
									{
										++provCount1;
										if( iSelectedMulUnits[provCount1]!=0)	
										{
											++flag;
											if (pStat->StatsMod.mfGetMod(STATSMOD::TRANSPORT_MOD) > 0)
												modRegency += 1;
											else if (units[iUnit].Realm != HomeRealm)
												modRegency += 8;
											else
												modRegency += 4;
										}
									}
								}
							}
						}
					}
			    }
trans_exit:   
				if (flag != 0)
				{
					y = startY + 33;
					x = startX + 160;
					print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_F00_PROVINCE));
					y += 15;
					init_gfont(FONT_SANS_8PT);

					for (h1=0; h1<2; ++h1)
				    {
						for(count = FIRST_PROVINCE; count<PROVINCE_COUNT; ++count)
						{
							if(count != iSelectedProvince)
							{
								CheckAndFixPlaces((PROVINCE)count);
								tmpIndex = j = province[count].FirstPlace;
								while (j != 0)							// scan for a SOURCE we own
								{
									if (places[tmpIndex].Icon == SOURCE1_ICON&&places[tmpIndex].Realm==HomeRealm)
										break;
									j = places[tmpIndex].NextPlace;
									tmpIndex = province[count].FirstPlace + j;
								}
								if (j != 0)
								{
									
									slevel = ProvDist(iSelectedProvince, count, 0, TRUE);
									if (slevel < level)
									{
										if (h1)
										{
											++counter2;
											if(counter2 < startProvCount2)
												continue;
			
											if(iSelectedUnit == count)
												DrawLittleButton(x, y-1, 145, 12, TRUE);
											else
												DrawLittleButton(x, y-1, 145, 12, FALSE);
											DrawShield(x+2, y, (REALM::REALM_TYPE)province[count].Realm);
							   				gprint_text(x+16, y, province[count].name, LoyaltyColor[province[count].Loyalty]);
											print_textf(x+125, y, LoyaltyColor[province[count].Loyalty], "%d", slevel);
											add_region(x, y, 145, 12, 0, SelectUnit,count,0,0, -1);
											y+=18;
											// print prev, next buttons if provinces are more that 10		
											if(DoPageProvince(&startProvCount2, provCount2, 10, counter2, x, y))
												break;
										}
										else
											++provCount2;
									}
								}
							}
						}
					}
				}
				if(iSelectedUnit != 0)
				{
					RegencePoint = modRegency;
					GoldBar = 1;
				}
				break;

			case ITEM_SPELL_WARDING:
			case ITEM_SPELL_HONEST_DEALING:
			case ITEM_SPELL_BLESS_LAND:
			case ITEM_SPELL_BLIGHT:
				print_textf(x, y, DKBROWN,STRMGR_GetStr(STR_SELECT_MORE_REALMS));
				y += 15;
				init_gfont(FONT_SANS_8PT);
				for(count = 0;
					count < 7 && province[iSelectedProvince].Border[count].Province != 0;
					++count )
				{
					tmpIndex=province[iSelectedProvince].Border[count].Province;
					
					if(iSelectedMulProvince[count])
					{
						DrawLittleButton(x, y-1, 145, 12, TRUE);
						if(province[tmpIndex].Dispel>0)
							RegencePoint += province[tmpIndex].DispelLvl+1;	
						if((ITEMTYPE)iSelectedOneHolding == ITEM_SPELL_HONEST_DEALING)
						{
							RegencePoint += province[tmpIndex].CivLevel * 3;
						}
						else if((ITEMTYPE)iSelectedOneHolding == ITEM_SPELL_WARDING)
						{
							RegencePoint += 5;
							GoldBar +=2;
						}
						else
						{
							CheckAndFixPlaces((PROVINCE)tmpIndex);
							i = j = province[tmpIndex].FirstPlace;
							while (j != 0)							// scan for a SOURCE we own
							{
								if (type == 4 && places[i].Icon == TEMPLE1_ICON&&places[i].Realm==HomeRealm)
									break;
								j = places[i].NextPlace;
								i = province[tmpIndex].FirstPlace + j;
							}
							if (j != 0)
							{
								if(province[tmpIndex].CivLevel <= places[i].Level)
									RegencePoint += 1;
								else
									RegencePoint += province[tmpIndex].CivLevel - places[i].Level;
							}
							else
								RegencePoint += province[tmpIndex].CivLevel;
						}
					}
					else
						DrawLittleButton(x, y-1, 145, 12, FALSE);

					init_gfont(FONT_SANS_8PT);
					DrawShield(x+2, y, (REALM::REALM_TYPE)province[tmpIndex].Realm);
			   	gprint_text(x+16, y, province[tmpIndex].name, LoyaltyColor[province[tmpIndex].Loyalty]);
					if(province[tmpIndex].Dispel>0)
					{
						print_textf(x+115, y,LoyaltyColor[province[tmpIndex].Loyalty] ,  "D%d", province[tmpIndex].DispelLvl);
					}
					add_region(x, y, 145, 12, 0, SelectMulProvince,count,level,0, -1);
					y+=18;
				}
				if((ITEMTYPE)iSelectedOneHolding == ITEM_SPELL_HONEST_DEALING)
					RegencePoint += province[iSelectedProvince].CivLevel * 3;
				else if((ITEMTYPE)iSelectedOneHolding == ITEM_SPELL_WARDING)
				{
					RegencePoint += 5;
					GoldBar += 2;
				}
				else
					RegencePoint += province[iSelectedProvince].CivLevel - slevel;
				if(RegencePoint != 0)
				{
					if(RegencePoint < 0)
						RegencePoint = 1;
					if((ITEMTYPE)iSelectedOneHolding == ITEM_SPELL_BLESS_LAND)
						GoldBar = 1;
					else if((ITEMTYPE)iSelectedOneHolding == ITEM_SPELL_BLIGHT 
							||(ITEMTYPE)iSelectedOneHolding == ITEM_SPELL_HONEST_DEALING)
						GoldBar = 2;
				}
				init_gfont(FONT_SANS_8PT);
				print_textf(startX + 160, startY + h - 73, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_U_CAN_CHOOSE_ANOTHER1)); 
				print_textf(startX + 160, startY + h - 60, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_U_CAN_CHOOSE_ANOTHER2), maxamt- additionalamt); 

				break;

		    case ITEM_SPELL_BLESS_ARMY:
				count = 0;
				y = startY; 
				x = startX + 160;
				print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_F00_UNITS));
				y += 15;
				for(i = 1; i < MAX_UNITS && count < 12; ++i)	
				{
					if(units[i].province == iSelectedProvince 
					   && units[i].Realm == HomeRealm
					   && units[i].Joined == 0)
					{
						for(iUnit = i; iUnit > 0 && count < 12; iUnit = units[iUnit].NextUnit)
						{

							if(iSelectedMulProvince[count])
							{
								DrawLittleButton(x, y-1, 145, 12, TRUE);
								iSelectedMulUnits[count]=iUnit;
								GoldBar = 1;
								RegencePoint += 1;
							}
							else
								DrawLittleButton(x, y-1, 145, 12, FALSE);
	
							DrawShield(x+2, y, (REALM::REALM_TYPE)units[iUnit].Realm);
							init_gfont(FONT_SANS_8PT);
							print_textf(x+16, y, DKBROWN, "%s", UnitName(iUnit));
							add_region(x, y, 145, 12,0, SelectMulProvince, count, level, 0, -1);	
							++count;
							y += 15;
						}
					}
				}
				init_gfont(FONT_SANS_8PT);
				print_textf(startX + 160, startY + h - 73, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_U_CAN_CHOOSE_ANOTHER1)); 
				print_textf(startX + 160, startY + h - 60, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_U_CAN_CHOOSE_ANOTHER3), level - additionalamt); 

				break;

			case ITEM_SPELL_DISPEL_REALM_MAGIC:
			case ITEM_SPELL_DISPEL_REALM_MAGIC_PRIESTVER:
				y += 20;
				modLevel = level/3;
				if(modLevel > 7)
					modLevel = 7;
				print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_DURATION_DOMAIN_TURN_FMT), modLevel);
				y += 20;
				print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_F02_STRENGTH));

				SetupMod(x+65, y, &modRegency, 0, 100, 1); 	// use iSelectedholding[i][0] as castle holding
				RegencePoint = modRegency;
				GoldBar = 1;
				break;


		}

//			}
//		 	++i; 
//			// GWP pStat->Inventory.mfAdvance(IC_REALM_SPELL);
//	   }

		if (province[iSelectedProvince].Dispel > 0)
   		RegencePoint += province[iSelectedProvince].DispelLvl+1;
	}
	if(checkGoldReg)
		fAllowRollButton = (ProcessGoldAndReg(CHECK_QUITE))?fAllowRollButton:0;
   	else
		fAllowRollButton = (ProcessGoldAndReg(CHECK_ONLY))?fAllowRollButton:0;

	x = startX+160;
	y = startY+h-45;
	init_gfont(FONT_SANS_8PT);

	if(iSelectedOneHolding==ITEM_SPELL_ALCHEMY)
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_GOLD_GAIN_FMT), GoldGained - GoldBar);
	else
		print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_TOTAL_GB_COST_FMT), GoldBar);
	y+=13;
	print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_TOTAL_REG_COST_FMT2), RegencePoint);
	fAllowCancelButton = TRUE;
	if((ITEMTYPE)iSelectedOneHolding != ITEM_SPELL_SCRY)
	{
		fAllowRollButton = FALSE;
		fAllowDoneButton = (iSelectedOneHolding != 99);
	}
}


/* ========================================================================
   Function    - PaintLieutenatAction
   Description - Paint the select a lieutenant from your realm to do the 
                 next action.
   Returns     -
   ======================================================================== */
void PaintLieutenantAction(LONG x, LONG y, LONG h)
{
 	LONG 		startY;
	LONG		counter, LieutenantCount;
	startY = y;
	counter = 0;
	LieutenantCount = 0;

	del_region(NextPrev,0);
	GoldBar = RegencePoint = 0;
	print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_F00_LIEUTENANTS));
	print_textf(x+150, y, DKBROWN,STRMGR_GetStr(STR_ACTNMENU_CLASS_AND_LEVEL));
	init_gfont(FONT_SANS_8PT);
	y += 20;
	for (int h1=0; h1<2; ++h1)
	{
		LONG i;
		
		for (i = 0; i<CHARACTER_COUNT; ++i)
		{
			if(regents[i].mfGetRealm() == HomeRealm 
			   && i != realm[HomeRealm].mfGetRegent() 
			   && regents[i].mfGetunit() > 0 		// -1 is dead, 0 unused.
			   )
			{
		  		if(h1)
				{
					++counter;
					if(counter < startProvCount)
						continue;

					if(iSelectedRealm == i) 
						DrawLittleButton(x, y-1, 130, 12, TRUE);
					else
						DrawLittleButton(x, y-1, 130, 12, FALSE);
					print_textf(x+2, y, DKBROWN, "%s", regents[i].mfGetname());
					if (regents[i].mfGetClass2() != NO_CLASS)
					{
						//String Manager can't be used twice in the same function call
						char buffer[100];
						strcpy(buffer, STRMGR_GetStr(gsClass[regents[i].mfGetClass2()]));
						print_textf(x+150, y, DKBROWN, "%s %d / %s %d", 
							STRMGR_GetStr(gsClass[regents[i].mfGetClass1()]), 
							regents[i].mfGetLevel1(),
							buffer, 
							regents[i].mfGetLevel2());
					}
					else if (regents[i].mfGetClass1() != NO_CLASS)
					{
						char buffer[100];
						strcpy(buffer, STRMGR_GetStr(gsClass[regents[i].mfGetClass1()]));
						print_textf(x+150, y, DKBROWN, "%s %d", buffer, regents[i].mfGetLevel1());
					}
			
					add_region(x, y, 130, 12, 0, SelectRealm,i,0,0, -1);
					y+=20;

					// print prev, next buttons if number of Lieutenants are more than 10		
					if(DoPageProvince(&startProvCount, LieutenantCount, 10, counter, x, y))
						break;

				}
				else
					++LieutenantCount;	
			}
		}
		
	}
	y = startY + h - 45;
	print_textf(x, y, DKBROWN,STRMGR_GetStr(STR_ACTNMENU_LIEUT_ACTN_HELP));
	fAllowCancelButton = TRUE;
	fAllowRollButton = FALSE;
	fAllowDoneButton = (iSelectedRealm != 0);
}



/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
static void DeleteActnMenuRegions (void)
{
	del_region(SelectRealm,0);
	del_region(SelectProvince,0);
	del_region(SelectHolding,0);
	del_region(IncNum,0);
	del_region(DecNum,0);
	del_region(IncNumWithSign,0);
	del_region(DecNumWithSign,0);
	del_region(SetNum,0);
	del_region(SelectOneHolding,0);
	del_region(SelectMulProvince, 0);
	del_region(InitBattleCard_alt, 0);
	del_region(SelectUnit, 0);
	del_region(SelectMulUnits, 0);
	del_region(Selection, 0);
}

void CleanupAfterActnMenu (void)
{
	startProvCount = 1;
	startProvCount1 = 1;
	GoldBar = -2;
	RegencePoint = -2;
	DeleteActnMenuRegions();
	fUpdateDataArea=TRUE;
	if (IsMenuActive(D_DACTN))
		HideMenu(D_DACTN);
	RunMenus();
}

void LogAction( LONG iAction )
{
   char temp[100];
   char ActionTitleString[50];
   strcpy(ActionTitleString, Action[iAction&0xFF].mfGetTitle());
	sprintf ( temp, "actnmenu.cpp - Player %s is doing %s", 
		realm[CurrentRealm].mfGetName(), 
		ActionTitleString);
   RandomLogComment ( temp );
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
static void InitActionGlobals (void)
{
	SHORT	i, j;

	OldNPCRegency = -1;
	fAllowRollButton = FALSE;
	fAllowDoneButton = FALSE;
	fAllowCancelButton = TRUE;
	firstRollFlag = 0;
	GoldBar = GoldGained = RegencePoint = 0;
	iSelectedRealm = REALM::NO_COUNTRY;
	iSelectedProvince = NO_PROVINCE;
	iSelectedOneHolding = 99;
	modGoldBars = 0;
	modRegency = 0;
	modOpponent = 0;
	reportFlag = 0;
	tmp_provCount = 0;				// don't remove it without asking Alan, please.
	iWhichRoll = 99;
	additionalamt = 0;
	startProvCount = 1;
	startProvCount1 = 1;
	startProvCount2 = 1;
	for(i=0; i<PROVINCE_COUNT; ++i)
		for(j=0; j<4; ++j)
			iSelectedHolding[i][j] = 0;
	for(i=0; i<128; ++i)
		fRollResult[i] = 0;
	for(i=0; i<16; ++i)
		iSelectedMulProvince[i]=0;
	for(i=0; i<20; ++i)
		iSelectedMulUnits[i]=0;
	for(i=0; i<9; ++i)
		iSelection[i]=0;
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
void PaintDomainAction (LONG MenuCombo, LONG iAction)
{
	SHORT	i, j;
	LONG	X,Y;
	LONG	W,H;
	LONG	MenuId, ButtonId;
	CHAR	color = 228;

	DeleteActnMenuRegions();
	iCurAction = iAction;
	iThisRoll = -1;

	// reinitialize globals to neutral values
	if(GoldBar == -2)
	{
		UpdateHoldingFlag();
		InitActionGlobals();

		#ifdef _WINDOWS
		#ifdef _DEBUG
		LogAction ( iAction );
		#endif
		#endif

	}

	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	// get position of help screen
	if( GetButtonPosition( MenuId, 1, &X, &Y ) == fERROR)
		return;

	// get size of menu
	if( GetButtonSize( MenuId, 1, &W, &H ) == fERROR)
		return;

	// ---------------------------------
	// print body of menu here
	// ---------------------------------
	print_textf(X+(W/2), Y, DKBLUE, "^F11^c%s",Action[iAction&0xFF].mfGetTitle());
	Y += 14;
	init_gfont(FONT_SANS_8PT);

	switch (iAction)
	{
		case AGITATE: 			// Agitate
			PaintAgitate(X, Y, H);
			break;
		case BUILD_ROAD:			// Build Road
			PaintBuildRoad(X,Y,H);
			break;
		case CONTEST: 			// Contest
			PaintContest(X, Y, H);
			break;
		case CREATE_HOLDING: 			// Create Holding
			PaintCreateHolding(X,Y,H);
			break;
		case DECLARE_WAR: 			// Declare War
			PaintDeclareWar(X,Y,H);
			break;
		case DIPLOMACY: 			// Diplomacy
			PaintDiplomacy(X,Y,H);
			break;
		case ESPIONAGE: 			// Espionage
			PaintEspionage(X,Y,H);
			break;
		case FORGE_LEY_LINE: 			// Forge Ley Line
			PaintForgeLeyLine(X, Y, H);
			break;
		case FORTIFY: 			// Fortify
			PaintFortify(X, Y, H);
			break;
		case INVESTITURE: 		// Investiture
			PaintInvestiture(X, Y, H);
			break;
		case LIEUTANANT:			// Lieutenant
			PaintLieutenant(X, Y, H);
			break;
		case LIEUTENANT_ACTION:			// Lieutenant Action
			PaintLieutenantAction(X, Y, H);
			break;
		case MUSTER:			// Muster Armies
			PaintMuster(X, Y, H);
			break;
		case REALM_SPELL:			// Realm Spell
			PaintRealmSpell(X, Y, H);
			break;
		case RULE:			// Rule
			PaintRule(X,Y,H);
			break;
		case TRADE_ROUTE:			// Trade Route
			PaintTradeRoute(X,Y,H);
			break;
		default:			// info about actions (1000+iAction)
			print_textf(X, Y, DKBROWN, "^F09%s",Action[iAction-1024].mfGetVerbose());
	}

	if (!IsMenuActive(D_DACTN))	// don't draw the buttons if there is no menu
		return;

	// ---------------------------------
	// paint the control buttons
	// ---------------------------------
	init_gfont(FONT_SANS_10PT);
	for(i=2;i<=4;++i)
	{
		// get position of Previous button
		if( GetButtonPosition( MenuId, i, &X, &Y ) == fERROR)
			return;

		// get size of Prev Button
		if( GetButtonSize( MenuId, i, &W, &H ) == fERROR)
			return;

		X += W/2;
		Y += H/2+1;

		if (i==2)
			print_text_centered(X,Y,STRMGR_GetStr(STR_CANCEL1),(fAllowCancelButton)?WHITE:GREY);
		else if (i==3)
		{
			if (iAction == BUILD_ROAD)  // Build Road
				print_text_centered(X,Y,STRMGR_GetStr(STR_ACTNMENU_PLAIN_BUILD),(fAllowRollButton)?WHITE:GREY);
			else if(iAction == MUSTER)  // Muster Troops
				print_text_centered(X,Y,STRMGR_GetStr(STR_ACTNMENU_PLAIN_MUSTER),(fAllowRollButton)?WHITE:GREY);
			else
				print_text_centered(X,Y,STRMGR_GetStr(STR_ROLL1),(fAllowRollButton)?WHITE:GREY);
		}
		else
			print_text_centered(X,Y,STRMGR_GetStr(STR_DONE1),(fAllowDoneButton)?WHITE:GREY);


		DispSuccess();
	}
}

/* ========================================================================
   Function    - Increment the action turn number if appropriate
   Description - 
   Returns     -
   ======================================================================== */
void IncActionTurn (void)
{
	// DM controlled realms get unlimited actions
	if (iCurAction != HOLD_ACTION && realm[HomeRealm].mfIsDMCtrl())
		return;

	if (fLTAction != DURING_LTACTION)
	{
		if (playerstats[ActiveRegent] != fERROR)
		{
			DumbAutoLockPtr< PLAYER_STATS > const pPS(playerstats[ActiveRegent]);
			if (iCurAction == FORGE_LEY_LINE && usedFreeForge < pPS->StatsMod.mfGetMod(STATSMOD::FREEFORGE_MOD))
			{
				AddReport(IMMEDIATE_USED_FREEACTION, 0, 0, 0);
				++usedFreeForge;
			}
			else if (iCurAction == REALM_SPELL && usedFreeMagic < pPS->StatsMod.mfGetMod(STATSMOD::FREEMAGIC_MOD))
			{
				AddReport(IMMEDIATE_USED_FREEACTION, 0, 0, 0);
				++usedFreeMagic;
			}
			else if (iCurAction == AGITATE && pPS->mfIsType(PLAYER_CLASS_INFO::PRIEST) && usedFreeAgitate == 0)
			{
				AddReport(IMMEDIATE_USED_FREEACTION, 0, 0, 0);
				++usedFreeAgitate;
			}
			else if (iCurAction == ESPIONAGE && pPS->mfIsType(PLAYER_CLASS_INFO::THIEF) && usedFreeSpy == 0)
			{
				AddReport(IMMEDIATE_USED_FREEACTION, 0, 0, 0);
				++usedFreeSpy;
			}
			else if (iCurAction != HOLD_ACTION && usedFreeAction < pPS->StatsMod.mfGetMod(STATSMOD::FREEACTION_MOD))	
			{
				AddReport(IMMEDIATE_USED_FREEACTION, 0, 0, 0);
				++usedFreeAction;
			}
			else
			{
//printf("Advancing to next ActionTurn - from:%d to:%d\n",action_turn[HomeRealm],action_turn[HomeRealm]+1);
				++action_turn[HomeRealm];
				musterSkl = 0;
				++TutorialActionNumber;
				TutorialScreenNumber = 0;
			}
		}
		else
		{
//printf("Advancing to next ActionTurn - from:%d to:%d\n",action_turn[HomeRealm],action_turn[HomeRealm]+1);
			++action_turn[HomeRealm];
			musterSkl = 0;
			++TutorialActionNumber;
			TutorialScreenNumber = 0;
		}


	}
	oldAction = action_turn[HomeRealm];
	if (action_turn[HomeRealm] > 3)			// never more than 4
	{
		action_turn[HomeRealm] = 4;
	}
}


/* ========================================================================
   Function    - AdjustHoldingLevel
   Description - Adjust Holding level to match the new province level
   Returns     -
   ======================================================================== */
void AdjustHoldingLevel (PROVINCE prov, BOOL fSend)
{
	LONG		count, tmpIndex, tmp, j, i;
	LONG		holding[6];
	char		n[40];

	for(j = 0; j<6; ++j)
		holding[j] = 0;

	for (count = 0; count < 3; ++count)
	{
		CheckAndFixPlaces(prov);
		tmpIndex = j = province[prov].FirstPlace;
		tmp = 0;
		i = 0;
		while ( j != 0)
		{
			if (places[tmpIndex].Icon == InfoReqType[count])
			{
				tmp += places[tmpIndex].Level;
				holding[i] = tmpIndex;	   // hold the index for decreasing the level later
				++i;
			}
			j = places[tmpIndex].NextPlace;
			tmpIndex = province[prov].FirstPlace + j;
		}
		if (tmp > province[prov].CivLevel)
		{
			for (i = 0; i<6 && holding[i]!=0; ++i)
			{
				if (places[holding[i]].Level > 0)
				{
					//places[holding[i]].Level--;   // reduce all holding with the same type by 1
					SetGameData(MP_PLACES, MPPLACE_LEVEL, holding[i], (places[holding[i]].Level-1), fSend);
				}
			}
		}
	}

	CheckAndFixPlaces(prov);
	tmpIndex = j = province[prov].FirstPlace;
	while (j != 0)
	{
		if ( places[tmpIndex].Icon == TEMPLE1_ICON ||
		    places[tmpIndex].Icon == LAW1_ICON ||
		    places[tmpIndex].Icon == GUILD1_ICON)
		{
			SetGameData(MP_PLACES, MPPLACE_IICONBITM, tmpIndex, places[tmpIndex].Icon+(((places[tmpIndex].Level<8)?places[tmpIndex].Level:7)/2), fSend);
		}
		j = places[tmpIndex].NextPlace;
		tmpIndex = province[prov].FirstPlace + j;
	}

}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
 void CancelHoldAction(LONG, LONG)
{
	del_region(CancelHoldAction,0);
	del_region(DoneHoldAction,0);
	HideMenu(D_HOLDACTION);
	RunMenus();
	fUpdateDataArea=TRUE;
 }
/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
 void DoneHoldAction(LONG, LONG)
{
	del_region(CancelHoldAction,0);
	del_region(DoneHoldAction,0);
	HideMenu(D_HOLDACTION);
	RunMenus();

	fUpdateDataArea=TRUE;
	fHeldAction = TRUE;
	iCurAction = HOLD_ACTION;
	IncActionTurn();
 }


void WarDeclared(REALM::REALM_TYPE who, REALM::REALM_TYPE onWhom, BOOL fSend)
{
	REALM::REALM_TYPE rlm;
	LONG	temp;
	LONG	num_allies;

	if (fSend || onWhom == HomeRealm)
		SendReportTo(onWhom, IMMEDIATE_NPC_DECLARE_WAR, who, 0, 0);
	
	// count the number of allies who will declare with us
	num_allies = 0;
	for (rlm = REALM::FIRST_REALM; rlm < LAND_REALM_COUNT; rlm = (REALM::REALM_TYPE)(rlm + 1))
	{
		if (!realm[rlm].mfExists() || rlm == onWhom || realm[rlm].mfIsPlayerCtrl())
			continue;

		if ( !AREFULLALLIES(rlm, who) || AREFULLALLIES(rlm, onWhom) )
			continue;

		++num_allies;
	}

	if (who == HomeRealm && num_allies >= 5)
	{
		SendReportTo(who, IMMEDIATE_ALLIES_JOIN_WAR, onWhom, 0, 0);
	}
	
	// "who" and full+ allies declare war on "onWhom"
	for (rlm = REALM::FIRST_REALM; rlm < LAND_REALM_COUNT; rlm = (REALM::REALM_TYPE)(rlm + 1))
	{
		// only realms that exist can declare war
		if (!realm[rlm].mfExists())
			continue;

		// victim won't declare war on self
		if (rlm == onWhom)
			continue;

		// choose our friends, not theirs
		if (rlm != HomeRealm && (!AREFULLALLIES(rlm, who) || AREFULLALLIES(rlm, onWhom)))
			continue;

		// how good of friends are we?
		if (ISVASSAL(rlm, who))			// declarer is rlm's liege
			temp = 4;
		else if (ISVASSAL(who, rlm))	// declarer is rlm's vassal
			temp = 3;
		else if (AREFULLALLIES(who, rlm))
			temp = 2;
		else if (AREALLIED(who, rlm))
			temp = 1;
		else
			temp = 0;	
			
		// other players don't auto-decalre-war, but notify them
		if ((realm[rlm].mfIsPlayerCtrl() && rlm != who))
		{
			// we can only have "who" whine about past declarations
			if (((RealmStatus[onWhom][who])&0x0F) == STATUS_AT_WAR &&
				((RealmStatus[rlm][onWhom])&0x0F) != STATUS_AT_WAR &&
				(fSend || rlm == HomeRealm))
			{
				if (temp > 0)
					SendReportTo(rlm, IMMEDIATE_NPC_DECLARE_WAR_ALLY, onWhom, temp, who);
			}
			continue;
		}
					
		// cancel tributes
//		Tribute[rlm][onWhom] = 0;
//		Tribute[onWhom][rlm] = 0;
		SetGameData(MP_TRIBUTE, rlm, onWhom, 0, TRUE);
		SetGameData(MP_TRIBUTE, onWhom, rlm, 0, TRUE);

		// congrats, your ally joins you... if there aren't too many,
		// we'll tell you about each one
		if (who == HomeRealm && rlm != HomeRealm && num_allies < 5)
		{
			if (temp >= 3)  // sending to declarer, so reverse the vassalage
				SendReportTo(who, IMMEDIATE_ALLY_DECLARE_WAR, (7-temp), rlm, onWhom);
			else
				SendReportTo(who, IMMEDIATE_ALLY_DECLARE_WAR, temp, rlm, onWhom);
		}
		
		if (onWhom == HomeRealm && rlm != who &&
			((RealmStatus[rlm][onWhom])&0x0F) != STATUS_AT_WAR)
		{
			SendReportTo(onWhom, IMMEDIATE_NPC_DECLARE_WAR_ALONE, rlm, 0, 0);
		}
		
		// set to AT_WAR and set them neutral to us if not at war
		LONG warStatus = (TestAdjacent(rlm, onWhom))?
			STATUS_AT_WAR_ADJ : STATUS_AT_WAR;
		SetGameData(MP_REALMSTATUS, rlm, onWhom, warStatus, fSend);
							
		// if they're at war with us already, no further political harm...
		if (((RealmStatus[onWhom][rlm])&0x0F) != STATUS_AT_WAR)
		{
			LONG tmp = RealmReaction[rlm][onWhom] - (Tribute[onWhom][rlm] * 5);
			if (tmp < 0)
				tmp = 0;
			SetGameData(MP_REALMREACTION, rlm, onWhom, tmp, fSend);
			tmp = RealmReaction[onWhom][rlm]
				- (5 + random(10) + (Tribute[rlm][onWhom] * 5));
			if (tmp < 0)
				tmp = 0;	
			SetGameData(MP_REALMREACTION, onWhom, rlm, tmp, fSend);
			SetGameData(MP_REALMSTATUS, onWhom, rlm, STATUS_NEUTRAL, fSend );
		}
	}
	
	// player-controlled realms have to declare war back.
	if (realm[onWhom].mfIsPlayerCtrl())
		return;
		
	if (who == HomeRealm &&
		((RealmStatus[onWhom][who])&0x0F) != STATUS_AT_WAR)
	{
		SendReportTo(who, IMMEDIATE_NPC_DECLARE_WAR, onWhom, 0, 0);
	}
	
	// now in the reverse direction
	for (rlm = REALM::FIRST_REALM; rlm < LAND_REALM_COUNT; rlm = (REALM::REALM_TYPE)(rlm + 1))
	{
		// only realms that exist can declare war
		if (!realm[rlm].mfExists())
			continue;

		// victim won't declare war on self
		if (rlm == who)
			continue;

		// choose our friends, not theirs
		if (!AREFULLALLIES(rlm, onWhom) || AREFULLALLIES(rlm, who))
			continue;

		// notify players rather than having them declare
		if (realm[rlm].mfIsPlayerCtrl() && (fSend || rlm == HomeRealm))
		{
			if (ISVASSAL(rlm, onWhom))		// war victim is rlm's liege
				temp = 4;
			else if (ISVASSAL(onWhom, rlm))	// war victim is rlm's vassal
				temp = 3;
			else if (AREFULLALLIES(onWhom, rlm))
				temp = 2;
			else if (AREALLIED(onWhom, rlm))
				temp = 1;
			else
				temp = 0;	
			
			if (temp > 0)
				SendReportTo(rlm, IMMEDIATE_NPC_DECLARE_WAR_ALLY, who, temp, onWhom);
		}

		// players never auto declare war back
		if (realm[rlm].mfIsPlayerCtrl())
			continue;

		if (who == HomeRealm && rlm != onWhom &&
			((RealmStatus[rlm][who])&0x0F) != STATUS_AT_WAR)
		{
			SendReportTo(who, IMMEDIATE_NPC_DECLARE_WAR_ALONE, rlm, 0, 0);
		}
		
		// set to AT_WAR and set them neutral to us if not at war
		LONG warStatus = (TestAdjacent(rlm, who))?
			STATUS_AT_WAR_ADJ : STATUS_AT_WAR;
		SetGameData(MP_REALMSTATUS, rlm, who, warStatus, fSend);
							
		LONG tmp = RealmReaction[rlm][who] - (Tribute[who][rlm] * 5);
		if (tmp < 0)
			tmp = 0;
		SetGameData(MP_REALMREACTION, rlm, who, tmp, fSend);
		
		tmp = RealmReaction[who][rlm]
			- (5 + random(10) + (Tribute[rlm][who] * 5));
		if (tmp < 0)
			tmp = 0;	
		SetGameData(MP_REALMREACTION, who, rlm, tmp, fSend);
		
		// cancel tributes
//		Tribute[rlm][who] = 0;
//		Tribute[who][rlm] = 0;
		SetGameData(MP_TRIBUTE, rlm, who, 0, TRUE);
		SetGameData(MP_TRIBUTE, who, rlm, 0, TRUE);
		
		if (((RealmStatus[who][rlm])&0x0F) != STATUS_AT_WAR)
			SetGameData(MP_REALMSTATUS, who, rlm, STATUS_NEUTRAL, fSend );
	}
	
}

/* ========================================================================
   Function    - DoneDomainAction
   Description - paint the text on the help menu
   Returns     -
   ======================================================================== */
void DoneDomainAction (LONG MenuCombo, LONG)
{
	LONG	i, j, k, index, dLevel, tmp, count, tmpIndex, iUnit;
	LONG	lAction = FALSE;
	LONG	x = 320;
	LONG	y = 240;
	LONG	f;
	SHORT	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	if (!fAllowDoneButton)
		return;
	
	WaitingWhen += 60;	// time limit will NOT run out while processing DONE
	
	// click the button
	if (mouse_button)
	{
		LONG	X, Y, X0, Y0;
		GetButtonPosition(MenuId, 0, &X0, &Y0);
		GetButtonPosition(MenuId, ButtonId, &X, &Y);
		SetButtonPosition(MenuId, ButtonId, X-X0-2, Y-Y0-3);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		SetButtonPosition(MenuId, ButtonId, X-X0, Y-Y0);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
	}
	

	iCancel = 0;

	ResumeTimeLimit();

	f = ProcessGoldAndReg(CHECK_AND_UPDATE);
	if (!f && iCurAction!=AGITATE && iCurAction!=BUILD_ROAD && iCurAction!=CONTEST
		 && iCurAction!=CREATE_HOLDING && iCurAction!=DECLARE_WAR && iCurAction!=DIPLOMACY
		 && iCurAction!=ESPIONAGE && iCurAction!=FORGE_LEY_LINE && iCurAction!=HOLD_ACTION
		 && iCurAction!=INVESTITURE && iCurAction!=RULE && iCurAction!=TRADE_ROUTE)
		return;

	switch (iCurAction)
	{
		case DIPLOMACY:
		case AGITATE:
		case CONTEST:
		case CREATE_HOLDING:
		case ESPIONAGE:
		case FORGE_LEY_LINE:
		case INVESTITURE:
		case RULE:
		case TRADE_ROUTE:
			IncActionTurn();
			break;

		case BUILD_ROAD:
			break;

		case LIEUTENANT_ACTION:
			/* FREE ACTION */
			ActiveRegent = iSelectedRealm;
			fLTAction = DURING_LTACTION;
			lAction = TRUE;
			break;

		case DECLARE_WAR:			// Declare War

//			fPlayerNeverDeclaredWar = FALSE;
			SetGameData( MP_GAMEDATA, MPGAMESEMIPHORE_DECLAREDWAR, 
						 0, FALSE, TRUE );

//	this now happens in WarDeclared
//			// the receipient of the declaration should know about it
//			SendReportTo(iSelectedRealm, IMMEDIATE_NPC_DECLARE_WAR, HomeRealm, 0, 0);
			
			// if you declare war on an ally you are SCUM and should suffer!!
			if (AREALLIED(HomeRealm, iSelectedRealm))
			{
				for (i=1; i<REALM::REALM_COUNT; ++i)
				{
					if (i != HomeRealm)
					{
						k = RealmReaction[i][HomeRealm] - random(5) - 5;
						RealmReaction[i][HomeRealm] = (k>0)?k:0;
						SetGameData(MP_REALMREACTION, i, HomeRealm, RealmReaction[i][HomeRealm], TRUE);
					}
				}
			}
			
			// give experience points
			ActionExpPoints(ActiveRegent, RLM_EXP_DECLARE_WAR, 0, TRUE);
			
			// new function to handle stuff commented out below, better
			WarDeclared(HomeRealm, (REALM::REALM_TYPE)iSelectedRealm, TRUE);
			
#if 0			
			// for you and everyone you are full allies with
			for (i=1; i<LAND_REALM_COUNT; ++i)
			{
				if (((RealmStatus[i][HomeRealm])&0x0F) >= STATUS_FULL_ALLIANCE
						&& i != iSelectedRealm && (i == HomeRealm || !realm[i].mfIsPlayerCtrl()))
				{
					// vs. them and everyone they are full allies with
					for (j=1; j<LAND_REALM_COUNT; ++j)
					{
						if ( ((RealmStatus[j][iSelectedRealm])&0x0F) >= STATUS_FULL_ALLIANCE
							&& ( ((RealmStatus[i][j])&0x0F) < STATUS_FULL_ALLIANCE || i==HomeRealm)
							&& j != HomeRealm
							)
						{
							// set to AT_WAR and set them neutral to us if not at war
							SetGameData(MP_REALMSTATUS, i, j, (TestAdjacent((REALM::REALM_TYPE)i,(REALM::REALM_TYPE)j))?STATUS_AT_WAR_ADJ:STATUS_AT_WAR, TRUE );
							if (j==iSelectedRealm && i!=HomeRealm && (ISVASSAL(i, HomeRealm) || ISVASSAL(HomeRealm, i)))
								AddReport(IMMEDIATE_ALLY_DECLARE_WAR, 3, i, j);
							else if (j == iSelectedRealm && i!=HomeRealm && AREFULLALLIES(i, HomeRealm))
						 		AddReport(IMMEDIATE_ALLY_DECLARE_WAR, 2, i, j);
							
							k = RealmReaction[i][j] - (Tribute[j][i] * 5);
							RealmReaction[i][j] = (k>0)?k:0;
							SetGameData(MP_REALMREACTION, i, j, RealmReaction[i][j], TRUE);
							k = RealmReaction[j][i] - (5 + random(10) + (Tribute[i][j] * 5));
							RealmReaction[j][i] = (k>0)?k:0;
							SetGameData(MP_REALMREACTION, j, i, RealmReaction[j][i], TRUE);
							//Tribute[i][j] = 0; Tribute[j][i] = 0;
							SetGameData(MP_TRIBUTE, i, j, 0, TRUE);
							SetGameData(MP_TRIBUTE, j, i, 0, TRUE);
							if (((RealmStatus[j][i])&0x0F) != STATUS_AT_WAR)
								SetGameData(MP_REALMSTATUS, j, i, STATUS_NEUTRAL, TRUE );
						}
					}
				}
			}
#endif
			// should test whether sounds are on
			{
				i = get_time()+5;
				while (i>get_time())
					ServiceSOLAudio();
				AddSndObj((BIRTHRT_SND)SND_BATTLE_HORN1,0,VOLUME_NINETY);
				i = get_time()+30;
				while (i>get_time())
					ServiceSOLAudio();
				AddSndObj((BIRTHRT_SND)SND_BATTLE_CHARGE1,0,VOLUME_NINETY);
				i = get_time()+40;
				while (i>get_time())
					ServiceSOLAudio();
			}
			IncActionTurn();
			break;

		case FORTIFY:        // fortify
			for(i=FIRST_PROVINCE; i<PROVINCE_COUNT; ++i)
			{
				dLevel = 0;
				if (iSelectedHolding[i][0])
				{
					CheckAndFixPlaces((PROVINCE)i);
					count = tmp = province[i].FirstPlace;
					while (tmp != 0)
					{
						if(places[count].Icon == CASTLE1_ICON)
						{
							dLevel = places[count].Level;
							break;
						}
						tmp = places[count].NextPlace;
						count = province[i].FirstPlace + tmp;
					}
					if (dLevel == 0)
					{
						CreateHolding(4, iSelectedHolding[i][0], iSelectedHolding[i][1], (PROVINCE)i, HomeRealm, TRUE );
						//printf("creating holding succeeded, province: %d, level: %d", i, iSelectedHolding[i][0]+dLevel);
					}
					else
					{
						places[count].iRoute1 = iSelectedHolding[i][0]+dLevel;
						//SetGameData(MP_PLACES, MPPLACE_IROUTE1, count, places[count].iRoute1, TRUE);
						places[count].iRoute2 = (dLevel*8) + iSelectedHolding[i][1];
						//SetGameData(MP_PLACES, MPPLACE_IROUTE2, count, places[count].iRoute2, TRUE);
 						//printf("improving an existed castle, province: %d, level: %d", i, iSelectedHolding[i][0]+dLevel);
						//GWP
						#ifdef _WINDOWS
						if (IsMultiPlayer())
							AMultiPlayer.PlacesMessage(count, (UBYTE *)&places[count]);
						#endif
					}
//					AddReport(IMMEDIATE_FORTIFY, dLevel + iSelectedHolding[i][0], i, 0);
				}
			}
			IncActionTurn();
			break;

		case LIEUTANANT:			// Create Lieutenant
			if (iSelectedRealm)
			{
				index = iSelectedOneHolding;		// lieutenant requested

				// check for already taken
				if (regents[index].mfGetunit() != 0)
				{
					AddSndObj((BIRTHRT_SND)SND_UI_DID_NOT_SUCCEED,0,VOLUME_NINETY);
					goto CantHire;
				}

				// check lockout for 10 seconds
				tmp = get_time() + (10 * 18);
				while ( regents[0].mfGetunit() != 0)
				{
					if (get_time() > tmp)
					{
						AddSndObj((BIRTHRT_SND)SND_UI_DID_NOT_SUCCEED,0,VOLUME_NINETY);
						goto CantHire;
					}
				}

				// set lockout
				SetGameData(MP_REGENT, MPREG_UNIT, 0, index, TRUE );

				// check again for already taken. if so, clear lockout
				if (regents[index].mfGetunit() != 0)
				{
					SetGameData(MP_REGENT, MPREG_UNIT, 0, 0, TRUE );
					AddSndObj((BIRTHRT_SND)SND_UI_DID_NOT_SUCCEED,0,VOLUME_NINETY);
					goto CantHire;
				}

				{
					BOOL	fFoundProv = FALSE;
					// look for a province with units and without a leader
					for (i=1; i<MAX_UNITS; ++i)		// scan for units here
					{
						if (i!=j 											// not self
							&& units[i].Joined == FALSE				// is a leader
							&& units[i].Realm == HomeRealm			// in same realm
							&& units[i].DestProvince == NO_PROVINCE	// Not moving.
							&& CountUnitStack(i) < gUnitStackSize 	// joinable stack
			    			&& units[i].id < 1000)						// Not Regent or lieutenant.
						{
							fFoundProv = TRUE;
							iSelectedProvince = (PROVINCE)units[i].province;
							break;
						}
					}
	 				if ( !fFoundProv )
	 				{
	 					// else look for a province without units
						for (i=1; i<PROVINCE_COUNT; ++i)
						{
							if ( (province[i].Realm == HomeRealm)
									&& (!province[i].Warded || (province[i].Warded && province[i].DispelLvl == HomeRealm))
								)
							{
								// empty provs
								if (UnitsHere((PROVINCE)i, HomeRealm) == 0)
								{
									iSelectedProvince = (PROVINCE)i;
								}
							}
						}
					}
				}
				
				// sieze the lieutenant
			 	j = CreateUnit(iSelectedProvince, regents[index].mfGeticon(), iSelectedRealm, HomeRealm, TRUE );
				CheckLeader (GetUnitLeader(j), TRUE );
				SetGameData(MP_REGENT, MPREG_UNIT, index, j, TRUE );

				// clear lockout
				SetGameData(MP_REGENT, MPREG_UNIT, 0, 0, TRUE );

				if (j != fERROR)
				{
					if (regents[iSelectedOneHolding].mfGetRealm() >= LAND_REALM_COUNT
					   	|| regents[iSelectedOneHolding].mfGetRealm() == HomeRealm)
						for(i=0; i<MAX_PLACES; ++i)
						{
							if(places[i].Realm == regents[iSelectedOneHolding].mfGetRealm())
							{
								//places[i].Realm = HomeRealm;
								SetGameData(MP_PLACES, MPPLACE_REALM, i, HomeRealm, TRUE );
							}
						}
//					regents[iSelectedOneHolding].mfSetunit(j);
					SetGameData(MP_REGENT, MPREG_UNIT, iSelectedOneHolding, j, TRUE );
					// HACK HACK HACK
					// Because I didn't want to change the saved games, we don't
					// update the regents realm.
//					// GWP regents[iSelectedOneHolding].mfSetRealm(HomeRealm);
					// GWP SetGameData(MP_REGENT, MPREG_REALM, iSelectedOneHolding, HomeRealm, TRUE );
//	 				AddReport(IMMEDIATE_LIEUTENANT, iSelectedOneHolding, 0, 0);	// dummy report
				}
				IncActionTurn();
			}
CantHire:
			break;

		case MUSTER:        // muster troops
			/* FREE ACTION */
//			if (iSelectedProvince)
//			{
//				j = 0;
//				for (i=1; i<C_HPY_UNIT_ICON; ++i)
//				{
//					while (iSelectedHolding[i][1])
//					{
//						CreateUnit(iSelectedProvince, i, 0, HomeRealm, TRUE );
//						++province[iSelectedProvince].LevyMustered;
//						--iSelectedHolding[i][1];
//						++j;
//					}
//				}
//// 				AddReport(IMMEDIATE_MUSTER, j, 0, 0);	// dummy report
//			}
			break;

 		case REALM_SPELL:
			// give experience points
			ActionExpPoints(
				ActiveRegent,
				RLM_EXP_REALM_SPELL, 
				RlmSpellLevel((ITEMTYPE)iSelectedOneHolding), TRUE);
			
 			switch((ITEMTYPE)iSelectedOneHolding)		 // death plague
 			{
 			case ITEM_SPELL_DEATH_PLAGUE:
				SetGameData(MP_PROVINCE, MPPROV_CIVLEVEL, iSelectedProvince, (province[iSelectedProvince].CivLevel-1), TRUE );
				//province[iSelectedProvince].CivLevel --;
				// adjust holding level
				AdjustHoldingLevel(iSelectedProvince, TRUE);
 				SetScrewed((REALM::REALM_TYPE)iSelectedRealm, HomeRealm, TRUE );
 				for(i=0; i<7; ++i)
 				{
 					if(iSelectedMulProvince[i]==TRUE)
 					{
 						//province[province[iSelectedProvince].Border[i].Province].CivLevel --;
						SetGameData(MP_PROVINCE, MPPROV_CIVLEVEL, province[iSelectedProvince].Border[i].Province, (province[province[iSelectedProvince].Border[i].Province].CivLevel-1), TRUE );
						SetScrewed((REALM::REALM_TYPE)province[province[iSelectedProvince].Border[i].Province].Realm, HomeRealm, TRUE );
						AdjustHoldingLevel((PROVINCE)province[iSelectedProvince].Border[i].Province, TRUE);
 					}
 				}
 				AddReport(SUBJ_DEATH_PLAGUE, 1, iSelectedProvince, province[iSelectedProvince].CivLevel);	
				break;

			case ITEM_SPELL_DEMAGOGUE:
				if(iSelectedHolding[7][0]==2)
					SetHelped((REALM::REALM_TYPE)iSelectedRealm, HomeRealm, TRUE );
				if(iSelectedHolding[7][0]==3)
					SetScrewed((REALM::REALM_TYPE)iSelectedRealm, HomeRealm, TRUE );
				province[iSelectedProvince].Loyalty += (iSelectedHolding[7][0]==2) - (iSelectedHolding[7][0]==3);
				SetGameData(MP_PROVINCE, MPPROV_LOYALTY, iSelectedProvince, province[iSelectedProvince].Loyalty, TRUE );
				for(i=0; i<7; ++i)
				{
					if(iSelectedMulProvince[i]==TRUE)
					{
						tmpIndex=province[iSelectedProvince].Border[i].Province;
						if(iSelectedHolding[i][0]==2)
							SetHelped((REALM::REALM_TYPE)province[tmpIndex].Realm, HomeRealm, TRUE );
						if(iSelectedHolding[i][0]==3)
							SetScrewed((REALM::REALM_TYPE)province[tmpIndex].Realm, HomeRealm, TRUE );
						province[tmpIndex].Loyalty += (iSelectedHolding[i][0]==2) - (iSelectedHolding[i][0]==3);
						SetGameData(MP_PROVINCE, MPPROV_LOYALTY, tmpIndex, province[tmpIndex].Loyalty, TRUE );
					}  
				}
 				AddReport(SUBJ_DEMAGOGUE, iSelectedHolding[7][0]==3, iSelectedProvince, province[iSelectedProvince].Loyalty);
				break;

			case ITEM_SPELL_LEGION_OF_DEAD:
				{
				//GEH added setup and loop on 8/18/97 as part of patch 1
				PLAYER_CLASS_INFO::TYPE MagicType; 
				LONG MagicLevel;
				SHORT const hPlayerStats = LoadStats(regents[ActiveRegent].mfGetid(), fERROR);
				DumbHandlePtr<PLAYER_STATS> const dhPlayerStats(hPlayerStats);
			
				// get the casters wizard level and for each level above 7,
				// generate one more skeleton.  Other code allows only WIZARDS
				// to cast this, so we assume that we only have to check
				// magiclevel
				if ( dhPlayerStats->mfIsMagicUser(&MagicType, &MagicLevel))
				{
					// if less then seven, shouldn't be here!
					if ( MagicLevel >= 7)
					{
						j = MagicLevel - 6;
						if (!AREALLIED((REALM::REALM_TYPE)iSelectedRealm, HomeRealm))
						{
							// We can put troops in our allies territories.
							SetScrewed((REALM::REALM_TYPE)iSelectedRealm, HomeRealm, TRUE );
						}
						for (i=1; i <= j; i++)
					 		CreateUnit(iSelectedProvince, C_SKL_UNIT_ICON, 0, HomeRealm, TRUE );
		 				AddReport(SUBJ_LEGION_OF_DEAD, iSelectedProvince, 0, 0);
					}
				}
				}
 				break;

			case ITEM_SPELL_MASS_DESTRUCTION:
				i = j = count = 0;

				for(i = 1; i < MAX_UNITS && count < 12; ++i)
				{
					if(units[i].province == iSelectedProvince
			 			   && units[i].Joined == 0
							&& realm[units[i].Realm].mfExists()
			 			   && units[i].Realm != HomeRealm)
					{
						for(iUnit = i; iUnit > 0;)
						{
							if(iSelectedMulProvince[count])
							{
								SetScrewed((REALM::REALM_TYPE)units[iUnit].Realm, HomeRealm, TRUE );
								if (HomeRealm != units[iUnit].Realm
								    && units[iUnit].Realm != province[iSelectedProvince].Realm
								    && AREALLIED((REALM::REALM_TYPE)province[iSelectedProvince].Realm, HomeRealm))
								{
									// Did we kill off an allies enemies
									SetHelped((REALM::REALM_TYPE)province[iSelectedProvince].Realm, HomeRealm, TRUE);
								}
								if(units[iUnit].Icon == GORGON_UNIT_ICON||units[iUnit].Icon == RHUOBHE_UNIT_ICON )
								{
									iUnit = units[iUnit].NextUnit;
								}
								else	
								{
									// DeleteUnit returns the new next unit.
									iUnit = DeleteUnit(iUnit, TRUE);
			 		  				++j;
								}
							}	
							else
							{
								iUnit = units[iUnit].NextUnit;
							}
							++count;
						}
						
					}
				}
 				AddReport(SUBJ_MASS_DESTRUCTION, j, iSelectedProvince, 0);
				break;

			case ITEM_SPELL_RAZE:
				CheckAndFixPlaces(iSelectedProvince);
				j = k = province[iSelectedProvince].FirstPlace;
				while (k != 0)
				{
					if(places[j].Icon == CASTLE1_ICON ||
						places[j].Icon == CASTLE2_ICON ||
						places[j].Icon == CASTLE3_ICON ||
						places[j].Icon == CASTLE4_ICON)
					{
						SetScrewed((REALM::REALM_TYPE)places[j].Realm, HomeRealm, TRUE );
						places[j].Level += modLevel;
						SetGameData(MP_PLACES, MPPLACE_LEVEL, j, places[j].Level, TRUE );
						if (places[j].Icon == CASTLE1_ICON)
						{
							SetGameData(MP_PLACES, MPPLACE_IICONBITM, j, places[j].Icon+(((places[j].Level<8)?places[j].Level:7)/2), TRUE );
						}
						break;
					}
					k = places[j].NextPlace;
					j = province[iSelectedProvince].FirstPlace + k;
				}

				if (places[j].Level == 0)
				{
					DeleteHolding(iSelectedProvince,j, TRUE );
	 				AddReport(SUBJ_RAZE_DESTROY, iSelectedProvince, 0, 0);
				}
				else
	 				AddReport(SUBJ_RAZE, iSelectedProvince, places[j].Level, 0);
				break;

 			case ITEM_SPELL_STRONGHOLD:
				j = TRUE;
 				if (iSelectedMulProvince[1])
 				{
					// create a WIZTOWER
					if (!AREFULLALLIES((REALM::REALM_TYPE)iSelectedRealm, HomeRealm))
					{
						SetScrewed((REALM::REALM_TYPE)iSelectedRealm, HomeRealm, TRUE );
					}
					CreateHolding(5, 0, 0, iSelectedProvince, HomeRealm, TRUE );
					j = FALSE;
 				}

				// we are raising an existing fortress or the just-created WIZTOWER
 				if (iSelectedMulProvince[0] || iSelectedMulProvince[1])
 				{
					CheckAndFixPlaces(iSelectedProvince);
					tmpIndex = i = province[iSelectedProvince].FirstPlace;
 					while (i != 0)
 					{
						if (places[tmpIndex].Icon == WIZTOWER_ICON
								 || (j && places[tmpIndex].Icon == CASTLE1_ICON))
							break;
						i = places[tmpIndex].NextPlace;
						tmpIndex = province[iSelectedProvince].FirstPlace + i;
 					}
 					if (i != 0)
					{
						++places[tmpIndex].Level;
						SetGameData(MP_PLACES, MPPLACE_LEVEL, tmpIndex, places[tmpIndex].Level, TRUE );
 					}

//	 				if (iSelectedMulProvince[1])
//		 				AddReport(SUBJ_STRONGHOLD1, iSelectedProvince, 0, 0);
//					else
//	 					AddReport(SUBJ_STRONGHOLD2, iSelectedProvince, places[tmpIndex].Level, 0);
				}
				break;

 			case ITEM_SPELL_SUMMONING:
 				j=0; 
				for(i = 0; i<4; ++i)
 				 {
 				 	if(iSelectedMulProvince[i]) 
 					{
						if (!AREALLIED((REALM::REALM_TYPE)iSelectedRealm, HomeRealm))
						{
							SetScrewed((REALM::REALM_TYPE)iSelectedRealm, HomeRealm, TRUE );
						}
						CreateUnit(iSelectedProvince, MonsterUnits[i], 0, HomeRealm, TRUE );
						++j;
					}
 				 }
 		   		AddReport(SUBJ_SUMMONING, j, iSelectedProvince, 0);	
				break;

			case ITEM_SPELL_TRANSPORT:
				for(i = 0; i<20; ++i)
				{
					if(iSelectedMulUnits[i]!=0 && iSelectedUnit != 0)
					{
						// GWP iSelectedUnit is actually a province value here.
						//     so casting is perfectly safe....uh huh.
						TransportUnit(iSelectedMulUnits[i], (PROVINCE)iSelectedUnit, TRUE);
					}
				}
 				AddReport(SUBJ_TRANSPORT, iSelectedProvince, iSelectedUnit, 0);	
				break;

			case ITEM_SPELL_BLESS_LAND:
			case ITEM_SPELL_BLIGHT:
			case ITEM_SPELL_HONEST_DEALING:
			case ITEM_SPELL_WARDING:
				for(count = 0;
					count < 7 && province[iSelectedProvince].Border[count].Province != 0;
					++count )
				{
					tmpIndex=province[iSelectedProvince].Border[count].Province;
					
					if(iSelectedMulProvince[count])
					{
						if((ITEMTYPE)iSelectedOneHolding==ITEM_SPELL_BLESS_LAND)
						{
							if(province[tmpIndex].Blighted==0)
								//province[tmpIndex].Blessed = 2;
								SetGameData(MP_PROVINCE, MPPROV_BLESSED, tmpIndex, 2, TRUE );
							else
								//province[tmpIndex].Blighted=0;
								SetGameData(MP_PROVINCE, MPPROV_BLIGHTED, tmpIndex, 0, TRUE );
							SetHelped((REALM::REALM_TYPE)province[tmpIndex].Realm, HomeRealm, TRUE );
						}
						else if((ITEMTYPE)iSelectedOneHolding == ITEM_SPELL_BLIGHT)
						{
							if(province[tmpIndex].Blessed==0)
								//province[tmpIndex].Blighted = 2;
								SetGameData(MP_PROVINCE, MPPROV_BLIGHTED, tmpIndex, 2, TRUE );
							else
								//province[tmpIndex].Blessed = 0;
								SetGameData(MP_PROVINCE, MPPROV_BLESSED, tmpIndex, 0, TRUE );
							SetScrewed((REALM::REALM_TYPE)province[tmpIndex].Realm, HomeRealm, TRUE );
						}
						else if((ITEMTYPE)iSelectedOneHolding == ITEM_SPELL_WARDING)
						{	
							//province[tmpIndex].Warded = 2;
							SetGameData(MP_PROVINCE, MPPROV_WARDED, tmpIndex, 2, TRUE );
							SetGameData(MP_PROVINCE, MPPROV_DISPELLVL, tmpIndex, HomeRealm, TRUE );
							SetGameData(MP_PROVINCE, MPPROV_DISPEL, tmpIndex, 0, TRUE );
							SetScrewed((REALM::REALM_TYPE)province[tmpIndex].Realm, HomeRealm, TRUE );
						}
						else
							//province[tmpIndex].Honest = 2;
							SetGameData(MP_PROVINCE, MPPROV_HONEST, tmpIndex, 2, TRUE );
					}
				}
				if((ITEMTYPE)iSelectedOneHolding==ITEM_SPELL_BLESS_LAND)
				{
					if(province[iSelectedProvince].Blighted==0)
						//province[iSelectedProvince].Blessed = 2;
						SetGameData(MP_PROVINCE, MPPROV_BLESSED, iSelectedProvince, 2, TRUE );
					else
						//province[iSelectedProvince].Blighted = 0;
						SetGameData(MP_PROVINCE, MPPROV_BLIGHTED, iSelectedProvince, 0, TRUE );
					SetHelped((REALM::REALM_TYPE)iSelectedRealm, HomeRealm, TRUE );

 					AddReport(SUBJ_BLESS_LAND, iSelectedProvince, 0, 0);	
					
				}
				else if((ITEMTYPE)iSelectedOneHolding == ITEM_SPELL_BLIGHT)
				{
					if(province[iSelectedProvince].Blessed == 0)
						//province[iSelectedProvince].Blighted = 2;
						SetGameData(MP_PROVINCE, MPPROV_BLIGHTED, iSelectedProvince, 2, TRUE );
					else
						//province[iSelectedProvince].Blessed = 0;
						SetGameData(MP_PROVINCE, MPPROV_BLESSED, iSelectedProvince, 0, TRUE );
					SetScrewed((REALM::REALM_TYPE)iSelectedRealm, HomeRealm, TRUE );
 					AddReport(SUBJ_BLIGHT, iSelectedProvince, 0, 0);
				}

				else if((ITEMTYPE)iSelectedOneHolding == ITEM_SPELL_WARDING)
				{
					province[iSelectedProvince].Warded = 2;
					SetGameData(MP_PROVINCE, MPPROV_WARDED, iSelectedProvince, 2, TRUE );
					SetGameData(MP_PROVINCE, MPPROV_DISPELLVL, iSelectedProvince, HomeRealm, TRUE );
					SetGameData(MP_PROVINCE, MPPROV_DISPEL, iSelectedProvince, 0, TRUE );
					SetScrewed((REALM::REALM_TYPE)iSelectedRealm, HomeRealm, TRUE );
 					AddReport(SUBJ_WARDING, iSelectedProvince, 0, 0);
				}
				else
				{
					province[iSelectedProvince].Honest = 2;
					SetGameData(MP_PROVINCE, MPPROV_HONEST, iSelectedProvince, 2, TRUE );
 					AddReport(SUBJ_HONEST, iSelectedProvince, 0, 0);
				}
				break;

			case ITEM_SPELL_BLESS_ARMY:
				j  = 0;
				for(i=0; i<12; ++i)
				{
					if(iSelectedMulUnits[i])
					{
						units[iSelectedMulUnits[i]].Blessed = TRUE;
						SetGameData(MP_UNITS, MPUNITS_BLESSED, iSelectedMulUnits[i], TRUE, TRUE );
						SetHelped((REALM::REALM_TYPE)units[iSelectedMulUnits[i]].Realm, HomeRealm, TRUE );
						++j;
					}
				}
 				AddReport(SUBJ_BLESS_ARMY, j, iSelectedProvince, 0);
				break;

			case ITEM_SPELL_DISPEL_REALM_MAGIC:
			case ITEM_SPELL_DISPEL_REALM_MAGIC_PRIESTVER:
				province[iSelectedProvince].Dispel = modLevel;
				SetGameData(MP_PROVINCE, MPPROV_DISPEL, iSelectedProvince, modLevel, TRUE );
				province[iSelectedProvince].DispelLvl = modRegency;
				SetGameData(MP_PROVINCE, MPPROV_DISPELLVL, iSelectedProvince, modRegency, TRUE );
				SetGameData(MP_PROVINCE, MPPROV_WARDED, iSelectedProvince, 0, TRUE );
 				AddReport(SUBJ_DISPEL_REALM_MAGIC, iSelectedProvince, 0, 0);
				break;
			}
			IncActionTurn();
			break;		// end of switch (iCurAction)

	}

	if (fLTAction == DURING_LTACTION && !lAction)
	{
		ActiveRegent = realm[HomeRealm].mfGetRegent();
		fLTAction = AFTER_LTACTION;
	}

	CleanupAfterActnMenu();
}

/* ========================================================================
   Function    - CancelDomainAction
   Description - 
   Returns     -
   ======================================================================== */
void CancelDomainAction (LONG MenuCombo, LONG)
{
	SHORT	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	if (!fAllowCancelButton)
		return;
	
	// click the button
	if (mouse_button)
	{
		LONG	X, Y, X0, Y0;
		GetButtonPosition(MenuId, 0, &X0, &Y0);
		GetButtonPosition(MenuId, ButtonId, &X, &Y);
		SetButtonPosition(MenuId, ButtonId, X-X0-2, Y-Y0-3);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		SetButtonPosition(MenuId, ButtonId, X-X0, Y-Y0);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();

		if (OldNPCRegency != -1 && OldNPCRealm > 0 && OldNPCRealm < REALM::REALM_COUNT)
			SetGameData(MP_REALM, MPRLM_REGENCY, OldNPCRealm, OldNPCRegency, TRUE);
	}
	

	if(iCurAction == LIEUTANANT)
		++iCancel;

	CleanupAfterActnMenu();

	if (fLTAction == DURING_LTACTION)
	{
		ActiveRegent = realm[HomeRealm].mfGetRegent();
		fLTAction = BEFORE_LTACTION;
	}

	ResumeTimeLimit();
}

/* ========================================================================
   Function    - AbortDomainAction
   Description - 
   Returns     -
   ======================================================================== */
void AbortDomainAction (void)
{
	if(iCurAction == LIEUTANANT)
		++iCancel;

	//IncActionTurn();
	++action_turn[HomeRealm];

	oldAction = action_turn[HomeRealm];
	if (action_turn[HomeRealm] > 3)			// never more than 4
		action_turn[HomeRealm] = 4;

	if ( IsMenuActive(D_DACTN) )
		CleanupAfterActnMenu();

	if (fLTAction == DURING_LTACTION)
	{
		ActiveRegent = realm[HomeRealm].mfGetRegent();
		fLTAction = BEFORE_LTACTION;
	}

	fUpdateDataArea=TRUE;
}

/* ========================================================================
   Function    - RollDomainAction
   Description - 
   Returns     -
   ======================================================================== */
void RollDomainAction (LONG MenuCombo, LONG)
{
	LONG		i;

	SHORT	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	if (!fAllowRollButton)
		return;
	
	// click the button
	if (mouse_button)
	{
		LONG	X, Y, X0, Y0;
		GetButtonPosition(MenuId, 0, &X0, &Y0);
		GetButtonPosition(MenuId, ButtonId, &X, &Y);
		SetButtonPosition(MenuId, ButtonId, X-X0-2, Y-Y0-3);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		SetButtonPosition(MenuId, ButtonId, X-X0, Y-Y0);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
	}


	if (!ProcessGoldAndReg(CHECK_ONLY))
		return;

	SuspendTimeLimit();

	if (iCurAction == MUSTER)		// Muster Troops
	{
		GoldBar += modGoldBars;
		if (!ProcessGoldAndReg(CHECK_ONLY))
		{
			GoldBar -= modGoldBars;
			return;
		}
		if (iSelectedProvince)
		{
			for (i=1; i<C_SPD_UNIT_ICON; ++i)
			{
				while (iSelectedHolding[i][1])
				{
					LONG const iUnit = CreateUnit(iSelectedProvince, (MAP_ICON)i, 0, HomeRealm, TRUE );
					if (iUnit != fERROR)
					{
						++province[iSelectedProvince].LevyMustered;
						iSelectedHolding[i][1]--;
						if(i == C_SKL_UNIT_ICON)
							musterSkl = 1;
					}
					else
					{
						// Don't charge for failed to create units.
						if(i == C_SKL_UNIT_ICON && iSelectedHolding[i][1])
							GoldBar += 1;
						else
							GoldBar += UnitMusterCost[i];
					}
				}
			}
		}
		modGoldBars = 0;
		iSelectedRealm = REALM::NO_COUNTRY;
		iSelectedProvince = NO_PROVINCE;
	}

	if (iCurAction == BUILD_ROAD)		// Build Road
	{
		GoldBar += modGoldBars;
		if (!ProcessGoldAndReg(CHECK_ONLY))
		{
			GoldBar -= modGoldBars;
			return;
		}

		// give experience points
		ActionExpPoints(ActiveRegent, RLM_EXP_BUILD_ROAD, 0, TRUE);
		
		for (i=0; i<7; ++i)		// set border one direction
			if (province[iSelectedRealm].Border[i].Province == iSelectedProvince)
				//province[iSelectedRealm].Border[i].MoveCost = PLAINS_ROAD;
				SetGameData(MP_PROVINCE, MPPROV_BORDER0+i, iSelectedRealm, PLAINS_ROAD, TRUE);

		for (i=0; i<7; ++i)		// set border other direction
			if (province[iSelectedProvince].Border[i].Province == iSelectedRealm)
				//province[iSelectedProvince].Border[i].MoveCost = PLAINS_ROAD;
				SetGameData(MP_PROVINCE, MPPROV_BORDER0+i, iSelectedProvince, PLAINS_ROAD, TRUE);

		// draw the road the same way it's done in GAMEMAP.CPP
		//if (province[iSelectedRealm].x < province[iSelectedProvince].x
		//	|| (province[iSelectedRealm].x==province[iSelectedProvince].x
		//		&& province[iSelectedRealm].y < province[iSelectedProvince].y))
		//	MapLine(province[iSelectedRealm].x,province[iSelectedRealm].y,province[iSelectedProvince].x,province[iSelectedProvince].y,MDBROWN,MDBROWN,TRUE);
		//else
		//	MapLine(province[iSelectedProvince].x,province[iSelectedProvince].y,province[iSelectedRealm].x,province[iSelectedRealm].y,MDBROWN,MDBROWN,TRUE);

//		AddReport(IMMEDIATE_BUILDROAD, iSelectedRealm, iSelectedProvince, 0);

		modGoldBars = 0;
		iSelectedRealm = REALM::NO_COUNTRY;
		iSelectedProvince = NO_PROVINCE;
	}

	fAllowCancelButton = FALSE;
	fAllowRollButton = FALSE;
	fAllowDoneButton = TRUE;
	iWhichRoll = 0;
	lAskedTargetWhichRoll = iWhichRoll;
}


/* ========================================================================
	Multi-player and computer AI Actions
   ======================================================================== */

//LONG lFromId;
//LONG lWhichRealm;
//LONG lFromRealm;
//LONG lWhichAction;
//LONG lWhichProvince;
//LONG lWhichHold;
//LONG lSuccess;
//LONG lResult;

RESPONSE_DIALOG response[8];
LONG	curResponse = 0;

LONG WaitForResult(LONG iWhichAction, LONG iWhichProvince, LONG iWhichHolding);

//CSTRPTR szWhichAction[19] = {
///* NO_ACTION		 */	"",
///* ADVENTURE		 */	"",
///* AGITATE			 */	"agitating",
///* BUILD_ROAD		 */	"building roads",
///* CONTEST			 */	"contesting",
///* CREATE_HOLDING	 */	"creating a holding",
///* DECLARE_WAR		 */	"",
///* DIPLOMACY		 */	"",
///* ESPIONAGE		 */	"",
///* FORGE_LEY_LINE	 */	"forging a ley line",
///* FORTIFY			 */	"fortifying",
///* HOLD_ACTION		 */	"",
///* INVESTITURE		 */	"",
///* LIEUTANANT		 */	"",
///* LIEUTENANT_ACTION */	"",
///* MUSTER			 */	"",
///* REALM_SPELL		 */	"",
///* RULE				 */	"ruling",
///* TRADE_ROUTE		 */	"creating a trade route",
//	};

static DEFINE_STATIC_VECTOR_DATA_S(GAME_STRING,gsWhichAction,19) = {
/* NO_ACTION		 */	STR_NULL,
/* ADVENTURE		 */	STR_NULL,
/* AGITATE			 */	STR_ACTNMENU_WHICH_ACTION_AGITATING,
/* BUILD_ROAD		 */	STR_ACTNMENU_WHICH_ACTION_BUILDING,
/* CONTEST			 */	STR_ACTNMENU_WHICH_ACTION_CONTESTING,
/* CREATE_HOLDING	 */	STR_ACTNMENU_WHICH_ACTION_CREATING,
/* DECLARE_WAR		 */	STR_NULL,
/* DIPLOMACY		 */	STR_NULL,
/* ESPIONAGE		 */	STR_NULL,
/* FORGE_LEY_LINE	 */	STR_ACTNMENU_WHICH_ACTION_FORGING,
/* FORTIFY			 */	STR_ACTNMENU_WHICH_ACTION_FORTIFYING,
/* HOLD_ACTION		 */	STR_NULL,
/* INVESTITURE		 */	STR_ACTNMENU_WHICH_ACTION_INVESTING,
/* LIEUTANANT		 */	STR_NULL,
/* LIEUTENANT_ACTION */	STR_NULL,
/* MUSTER			 */	STR_NULL,
/* REALM_SPELL		 */	STR_NULL,
/* RULE				 */	STR_ACTNMENU_WHICH_ACTION_RULING,
/* TRADE_ROUTE		 */	STR_ACTNMENU_WHICH_ACTION_TRADE_ROUTE,
	};
DEFINE_STATIC_VECTOR_CLASS(GAME_STRING, gsWhichAction);


/* ========================================================================
   Function    - DoneOthersAction
   Description - 
   Returns     -
   ======================================================================== */
void DoneOthersAction (LONG, LONG)
{
	DeleteActnMenuRegions();

	//SetButtonProc (D_MOREINFO, 0, PaintMoreInfo, 0, 0);
	//SetButtonProc (D_MOREINFO, 4, HideSubMenu, D_MOREINFO, 0);
	HideRequest(D_MOREINFO);

	RunRequests();
	fMenuClosed = TRUE;		// force DomainTurnUI redraw, sets fRedrawAll
	if (curResponse > 0)
		curResponse--;
	ResumeTimeLimit();
}

/* ========================================================================
   Function    - RollOthersAction
   Description - 
   Returns     -
   ======================================================================== */
void RollOthersAction (LONG, LONG)
{
	SendMyMods(response[curResponse].lWhichAction, response[curResponse].lWhichProvince, response[curResponse].lWhichHold, response[curResponse].modRegency);
	response[curResponse].lResult = WaitForResult(response[curResponse].lWhichAction, response[curResponse].lWhichProvince, response[curResponse].lWhichHold);

	realm[HomeRealm].mfAddRegency( -response[curResponse].modRegency );
	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		char temp[100];
		sprintf ( temp, "RollOthersAction - HomeRealm:%s  regency:%04d",realm[HomeRealm].mfGetName(),realm[HomeRealm].mfGetRegency());
		RandomLogComment ( temp );
	}
	SetGameData(MP_REALM, MPRLM_REGENCY, HomeRealm, realm[HomeRealm].mfGetRegency(), TRUE);
}

/* ========================================================================
   Function    - PaintOthersAction
   Description - 
   Returns     -
   ======================================================================== */
void PaintOthersAction (LONG MenuCombo, LONG)
{
	SHORT	i, j;
	LONG	X,Y;
	LONG	W,H;
	LONG	MenuId, ButtonId;
	LONG	result = 0;
	LONG	startY;

	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	LONG const menuResponse = MyRequestValue( D_MOREINFO );
	if (menuResponse == fERROR)
		return;

	del_region(RollOthersAction, 0);
	del_region(DoneOthersAction, 0);
	DeleteActnMenuRegions();
	iThisRoll = -1;

	fAllowRollButton_others = TRUE;
	fAllowDoneButton_others = FALSE;

	// get position of text area
	if( GetButtonPosition( MenuId, 0, &X, &Y ) == fERROR)
		return;

	// get size of menu
	if( GetButtonSize( MenuId, 0, &W, &H ) == fERROR)
		return;

	startY = Y;

	init_gfont(FONT_SANS_8PT);

	// ---------------------------------
	// print body of menu here
	// ---------------------------------

	Y += 20;
	X += 20;

	{
		char FormatString[256];
		char RealmName[80];
		LONG	m = -1;

		switch ( places[response[curResponse].lWhichHold].Icon )
		{
			case LAW1_ICON: m = 0; break;
			case GUILD1_ICON: m = 1; break;
			case TEMPLE1_ICON: m = 2; break;
			case SOURCE1_ICON: m = 3; break;
			default: break;
		}

		strcpy(RealmName, realm[response[menuResponse].lFromRealm].mfGetName());

		if (m >= 0 && (places[response[curResponse].lWhichHold].Level == 0
				   		|| places[response[curResponse].lWhichHold].Contested == TRUE) )
		{
			strcpy(FormatString, STRMGR_GetStr(STR_MAPAI_DESTROYING_HOLDING_FMT));
			print_textf(X+5,Y, DKBROWN, FormatString,
				RealmName,
				STRMGR_GetStr(gsHoldType[m]),
				province[response[menuResponse].lWhichProvince].name);
		}
		else
		{
			strcpy(FormatString, STRMGR_GetStr(STR_ACTNMENU_WHICH_ACTION_FMT));
			print_textf(X+5, Y, DKBROWN, FormatString,
				RealmName,
				(response[menuResponse].lWhichAction<0)?szAIAction[-response[menuResponse].lWhichAction].mfGetStr():STRMGR_GetStr(gsWhichAction[response[menuResponse].lWhichAction]),
				province[response[menuResponse].lWhichProvince].name);
		}
	}

	Y += 45;
	print_textf(X+5, Y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_F02_SUCCESS));
	print_textf(X+71, Y+6, DKBROWN, "^F02^c%d%c", D20TOPERCENT(response[menuResponse].lSuccess+response[menuResponse].modRegency),PERCENTCHAR);

	print_textf(X+110, Y, DKBROWN,   STRMGR_GetStr(STR_MAPAI_MODIFY_SUCCESS));
	print_textf(X+110, Y+8, DKBROWN, STRMGR_GetStr(STR_MAPAI_USING_REGENCY));
	//SetupMod(X+200, Y, &response[menuResponse].modRegency, 0, 20, 1);
	//SetupMod(X+200, Y, &response[menuResponse].modRegency, 0, 20*((response[menuResponse].lSuccess+response[menuResponse].modRegency)<20), 1);
	if (realm[HomeRealm].mfGetRegency() > response[menuResponse].modRegency
		&& response[menuResponse].lSuccess + response[menuResponse].modRegency < 20)
	{
		SetupMod(X+200, Y, &response[menuResponse].modRegency, 0, 20, 1);
	}
	else 	// not enough regency, or spinner already pegged
	{
		SetupMod(X+200, Y, &response[menuResponse].modRegency, 0, 0, 1);
	}

	RegencePoint = response[menuResponse].modRegency;

	//---- Only play the sounds once

	if (response[menuResponse].lResult == 1)	// success
	{
		AddSndObj((BIRTHRT_SND)SND_UI_SUCCEEDED,0,VOLUME_NINETY);
	   response[menuResponse].lResult = 3;
	}
	if (response[menuResponse].lResult == 2)	// failure
	{
 		AddSndObj((BIRTHRT_SND)SND_UI_DID_NOT_SUCCEED,0,VOLUME_NINETY);
	   response[menuResponse].lResult = 4;
	}

	//---- Now we can print the data 
	init_gfont(FONT_SANS_8PT);

	if ( response[menuResponse].lResult == 3 )
	{
		fAllowRollButton_others = FALSE;
		fAllowDoneButton_others = TRUE;
		print_textf(X+130, Y, RED, STRMGR_GetStr(STR_MAPAI_ACTION_SUCCEEDED));
	}

	if ( response[menuResponse].lResult == 4 )
	{
		fAllowRollButton_others = FALSE;
		fAllowDoneButton_others = TRUE;
		print_textf(X+130, Y, RED, STRMGR_GetStr(STR_MAPAI_ACTION_FAILED));
	}

	Y = startY+H-80;
	print_textf(X+5, Y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_TOTAL_REG_COST_FMT), RegencePoint);

	Y = startY+H-65;
	print_textf(X+5, Y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_OTHERS_ACTION_HELP));

	// draw buttons
	init_gfont(FONT_SANS_10PT);
	Y = startY+H-32;

	if(fAllowRollButton_others)
		add_region(X+138, Y, 68, 28, 0, RollOthersAction, 0, 0, 0, -1);

	SHORT iButton = GetResourceStd ("UI\\REGBTN2A.PCX", FALSE);

	DrawBitmap (X+138-15, (SHORT)Y, iButton, 0, 0, 999, 999);
	print_textf(X+138+34-15, Y+14, (fAllowRollButton_others)?WHITE:GREY, "^F02^c%s", STRMGR_GetStr(STR_ROLL1));

	if(fAllowDoneButton_others)
		add_region(X+218, Y, 68, 28, 0, DoneOthersAction, 0, 0, 0, -1);

	DrawBitmap (X+218, (SHORT)Y, iButton, 0, 0, 999, 999);
	print_textf(X+218+34, Y+14, (fAllowDoneButton_others)?WHITE:GREY, "^F02^c%s", STRMGR_GetStr(STR_DONE1));

	if (iButton != fERROR)
		SetPurge(iButton);
}

/* ========================================================================
   Function    - DisplayOthersAction
   Description - 
   Returns     -
   ======================================================================== */
void DisplayOthersAction ( LONG iFromId, LONG lAction, LONG lProv, LONG lHold, LONG success)
{
	if (curResponse < 8)
		++curResponse;

	response[curResponse].lFromId			= iFromId;
	response[curResponse].lFromRealm		= iFromId;
	response[curResponse].lWhichHold		= lHold;
	response[curResponse].lWhichAction	= lAction;
	response[curResponse].lWhichProvince= lProv;
	response[curResponse].lSuccess		= success;
	response[curResponse].lResult			= 0;
	response[curResponse].modRegency		= 0;

	SuspendTimeLimit();

#ifdef _WINDOWS
	if(IsMultiPlayer())
	{
		// set the realm of the sender
		for (int i = 0; i < LAND_REALM_COUNT; ++i)
		{
			if ( lRealmSelectedByPlayer[i] == iFromId )
			{
				response[curResponse].lFromRealm = i;
				break;
			}
		}
		// tell sender we heard
		SetGameData(MP_REALM, MPRLM_BATTLEQUERY, response[curResponse].lFromRealm, 0, TRUE);
	}
#endif

	// if action is Diplomacy
	if (lAction == DIPLOMACY)
	{
		if (realm[response[curResponse].lFromRealm].mfIsPlayerCtrl())	// non NPC
			fOtherDipMP = FALSE;

		ShowRequest(D_OTHERDIPLOMACY, curResponse);	// do Diplomacy style dialog
	}

	// find the realm of the target
	else
	{
		if(lProv)
		{
			response[curResponse].lWhichRealm = province[lProv].Realm;
		}
		else	// get the realm from the holding
		{
			response[curResponse].lWhichRealm = places[lHold].Realm;
			response[curResponse].lWhichProvince = FindProvForPlace(lHold);
		}

		// do the dialog box
		SetButtonProc (D_MOREINFO, 0, PaintOthersAction, 0, 0);
		SetButtonProc (D_MOREINFO, 4, NULL, 0, 0);
		ShowRequest(D_MOREINFO, curResponse);
	}

}

/* ======================================================================== */
void DoOthersAction(LONG, LONG)
{
	// test routine Baruk-Azik is agitating Ghonallison
	// chance is 9
	DisplayOthersAction(0, DIPLOMACY, 85, 0, 9);
}

/* ======================================================================== */
void NeverOthersDiplomacy(LONG, LONG)
{
	ResumeTimeLimit();

	// send failure
	// send make other offer only if against other remote !!
#ifdef _WINDOWS
	if ( !fOtherDipMP )
#endif
		SendMyMods(response[curResponse].lWhichAction, response[curResponse].lWhichProvince, response[curResponse].lWhichHold, 2);

	HideRequest(D_OTHERDIPLOMACY);
	RunRequests();
	fMenuClosed = TRUE;		// force DomainTurnUI redraw, sets fRedrawAll
	if (curResponse > 0)
		curResponse--;

	//----- Need to sync with other players that are waiting

#ifdef _WINDOWS
	if ( fOtherDipMP )
		//WaitSync( iMPS_SYNC5 );
		MapAIPlayerResponseWaitSync();

	fOtherDipMP = FALSE;
#endif

	NoFlag = FALSE;

}
/* ======================================================================== */
BOOL NPCResult(SHORT flag)
{
	LONG 	TProvince, holding, gold, offgold, trib, tmp, value, victim;

	if (flag)
	{
		if(dData.d1.iAlliance == 1)
		{
			SetGameData(MP_REALMSTATUS, HomeRealm, dData.d1.iWhoFrom, STATUS_PERMISSIVE_ALLIANCE, TRUE );
			SetGameData(MP_REALMREACTION, HomeRealm, dData.d1.iWhoFrom, 75, TRUE);
	
			SetGameData(MP_REALMREACTION, dData.d1.iWhoFrom, HomeRealm, 75, TRUE);
			SetGameData(MP_REALMSTATUS, dData.d1.iWhoFrom, HomeRealm, STATUS_PERMISSIVE_ALLIANCE, TRUE );
			if(dData.d1.iOffHold)
				//SetGameData(MP_PLACES, MPPLACE_REALM, dData.d1.iOffHold, HomeRealm, TRUE);
				CheckHolding(dData.d1.iOffHold, HomeRealm, TRUE);
			if(dData.d1.iReqHold)
				//SetGameData(MP_PLACES, MPPLACE_REALM, dData.d1.iReqHold, CurrentRealm, TRUE);
				CheckHolding(dData.d1.iReqHold, (REALM::REALM_TYPE)dData.d1.iWhoFrom, TRUE);
			Tribute[dData.d1.iWhoFrom][HomeRealm] += dData.d1.iOffTrib;
			SetGameData(MP_TRIBUTE,dData.d1.iWhoFrom, HomeRealm, Tribute[dData.d1.iWhoFrom][HomeRealm], TRUE);
	 	}
		else if(dData.d1.iAlliance == 2)
		{
			SetGameData(MP_REALMSTATUS, HomeRealm, dData.d1.iWhoFrom, STATUS_FULL_ALLIANCE, TRUE );
			RealmStatus[dData.d1.iWhoFrom][HomeRealm] = STATUS_FULL_ALLIANCE;
			SetGameData(MP_REALMSTATUS, dData.d1.iWhoFrom, HomeRealm, STATUS_FULL_ALLIANCE, TRUE );
			RealmReaction[HomeRealm][dData.d1.iWhoFrom] = 95;
			SetGameData(MP_REALMREACTION, HomeRealm, dData.d1.iWhoFrom, 95, TRUE);
			SetGameData(MP_REALMREACTION, dData.d1.iWhoFrom, HomeRealm, 95, TRUE);
			if(dData.d1.iOffHold)
				//SetGameData(MP_PLACES, MPPLACE_REALM, dData.d1.iOffHold, HomeRealm, TRUE);
				CheckHolding(dData.d1.iOffHold, HomeRealm, TRUE);
			if(dData.d1.iReqHold)
				//SetGameData(MP_PLACES, MPPLACE_REALM, dData.d1.iReqHold, CurrentRealm, TRUE);
				CheckHolding(dData.d1.iReqHold, (REALM::REALM_TYPE)dData.d1.iWhoFrom, TRUE);
			
			Tribute[dData.d1.iWhoFrom][HomeRealm] += dData.d1.iOffTrib;
			SetGameData(MP_TRIBUTE,dData.d1.iWhoFrom, HomeRealm, Tribute[dData.d1.iWhoFrom][HomeRealm], TRUE);
		}
		else if(dData.d1.iAlliance == 3)
		{
			SetGameData(MP_REALMSTATUS, HomeRealm, dData.d1.iWhoFrom, STATUS_VASSAL, TRUE);
			SetGameData(MP_REALMREACTION, HomeRealm, dData.d1.iWhoFrom, 150, TRUE);
			SetGameData(MP_REALMREACTION, dData.d1.iWhoFrom, HomeRealm, 95, TRUE);
			SetGameData(MP_REALMSTATUS, dData.d1.iWhoFrom, HomeRealm, STATUS_FULL_ALLIANCE, TRUE );

			victim = province[dData.d1.iOffProv].Realm;
			SetGameData(MP_PROVINCE, MPPROV_REALM, dData.d1.iOffProv, HomeRealm, TRUE);
			Tribute[dData.d1.iWhoFrom][HomeRealm] += dData.d1.iOffTrib;
			SetGameData(MP_TRIBUTE,dData.d1.iWhoFrom, HomeRealm, Tribute[dData.d1.iWhoFrom][HomeRealm], TRUE);
			if(dData.d1.iOffHold)
				//SetGameData(MP_PLACES, MPPLACE_REALM, dData.d1.iOffHold, HomeRealm, TRUE );
				CheckHolding(dData.d1.iOffHold, HomeRealm, TRUE);
			//ReconstructMap();
			SetGameData(MP_PROVINCE, MPPROV_INVESTED, HomeRealm, victim, TRUE );
		}
		if (dData.d1.iReqProv)
		{
			victim = province[dData.d1.iReqProv].Realm;
			SetGameData(MP_PROVINCE, MPPROV_REALM, dData.d1.iReqProv, dData.d1.iWhoFrom, TRUE );
			//SetGameData(MP_PLACES, MPPLACE_REALM, dData.d1.iOffHold, HomeRealm, TRUE );
			if(dData.d1.iOffHold)
			{
				CheckHolding(dData.d1.iOffHold, HomeRealm, TRUE);
			}
			Tribute[dData.d1.iWhoFrom][HomeRealm] += dData.d1.iOffTrib;
			//ReconstructMap();
			SetGameData(MP_PROVINCE, MPPROV_INVESTED, dData.d1.iWhoFrom, victim, TRUE );
		}
		if (dData.d1.iReqHold && !dData.d1.iAlliance)
		{
			//SetGameData(MP_PLACES, MPPLACE_REALM, dData.d1.iReqHold, dData.d1.iWhoFrom, TRUE );
			CheckHolding(dData.d1.iReqHold, (REALM::REALM_TYPE)dData.d1.iWhoFrom, TRUE);
			//SetGameData(MP_PLACES, MPPLACE_REALM, dData.d1.iOffHold, HomeRealm, TRUE );
			if(dData.d1.iOffHold)
			{
				//SetGameData(MP_PLACES, MPPLACE_REALM, dData.d1.iOffHold, HomeRealm, TRUE );
				CheckHolding(dData.d1.iOffHold, HomeRealm, TRUE);
			}
			Tribute[dData.d1.iWhoFrom][HomeRealm] += dData.d1.iOffTrib;
			SetGameData(MP_TRIBUTE, dData.d1.iWhoFrom,HomeRealm, Tribute[dData.d1.iWhoFrom][HomeRealm], TRUE);
		}
		if (dData.d1.iReqTrib)
		{
			if(dData.d1.iOffHold)
			{
				//SetGameData(MP_PLACES, MPPLACE_REALM, dData.d1.iOffHold, HomeRealm, TRUE );
				CheckHolding(dData.d1.iOffHold, HomeRealm, TRUE);
			}
			Tribute[HomeRealm][dData.d1.iWhoFrom] += dData.d1.iReqTrib;
			SetGameData(MP_TRIBUTE, HomeRealm, dData.d1.iWhoFrom,Tribute[HomeRealm][dData.d1.iWhoFrom], TRUE);
		}

		realm[HomeRealm].mfAddTreasury(dData.d1.iOffGold * 10);
		realm[HomeRealm].mfAddTreasury( -(dData.d1.iReqGold * 10));
		SetGameData(MP_REALM, MPRLM_TREASURY, HomeRealm, realm[HomeRealm].mfGetTreasury(), TRUE);

		realm[dData.d1.iWhoFrom].mfAddRegency( -dData.d1.iOffRegy);
		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
			char temp[100];
			sprintf ( temp, "NPCResult - Realm:%s  regency:%04d",realm[dData.d1.iWhoFrom].mfGetName(),realm[dData.d1.iWhoFrom].mfGetRegency());
			RandomLogComment ( temp );
		}
		SetGameData(MP_REALM, MPRLM_REGENCY, dData.d1.iWhoFrom, realm[dData.d1.iWhoFrom].mfGetRegency(), TRUE);

		realm[dData.d1.iWhoFrom].mfAddTreasury( -(dData.d1.iOffGold * 10));
		realm[dData.d1.iWhoFrom].mfAddTreasury(dData.d1.iReqGold * 10);
		SetGameData(MP_REALM, MPRLM_TREASURY, dData.d1.iWhoFrom, realm[dData.d1.iWhoFrom].mfGetTreasury(), TRUE);

		return FALSE;
	}

	else		// flag == FALSE
	{
		if (dData.d1.iAlliance == 1
			|| dData.d1.iAlliance == 2
			|| dData.d1.iAlliance == 3)
		{
			gold = 0;
			offgold = 0;
			trib = 0;
			TargetPlace = dData.d1.iOffHold;

			value = random(10);			// make deal better
			value -= dData.d1.iReqGold;
			value += dData.d1.iOffGold;
			value += dData.d1.iOffTrib * 3;

			if (dData.d1.iReqHold)
			{
				value -= HoldingValue(dData.d1.iReqHold);
				tmp = DemandHolding(HomeRealm, dData.d1.iReqHold);
				if (tmp)
				{
					dData.d1.iReqHold = StartPlace;
					value += tmp;
				}
			}
			if (dData.d1.iOffHold == 0 && value >= 0)
			{
				tmp = OfferHolding();
				if (tmp)
				{
					dData.d1.iOffHold = TargetPlace;
					value -= tmp;
				}
			}

			if (value < 0)									// make up what's left
			{
				gold = -value;
				if (!CanAfford (gold, 0, 0, HomeRealm))
					gold = realm[HomeRealm].mfGetTreasury()/10-1;
				if (gold < 0)
					gold = 0;
				value += gold;
			}

			if (value > 0)								// make up what's left
			{
				trib = value / 3;
				if (!CanAfford(0, 0, trib, (REALM::REALM_TYPE)dData.d1.iWhoFrom))
					trib = realm[(REALM::REALM_TYPE)dData.d1.iWhoFrom].mfGetIncome() - 1;
				if (trib < 0)
					trib = 0;
				value -= (trib * 3);
			}

			if (value > 0)								// make up what's left
			{
				offgold = value;
				if (!CanAfford(offgold, 0, 0, (REALM::REALM_TYPE)dData.d1.iWhoFrom))
					offgold = realm[(REALM::REALM_TYPE)dData.d1.iWhoFrom].mfGetTreasury()/10-1;
				if (offgold < 0)
					offgold = 0;
				value -= offgold;
			}

			dData.d1.iReqGold = gold;
			dData.d1.iOffGold = offgold;
			dData.d1.iOffTrib = trib;

			if (value > 5)			// can't make better
				return FALSE;
	 	}

		if (dData.d1.iReqProv && dData.d1.iAlliance == 0)
		{
			gold = random(4)+1;
			if (!CanAfford(gold+dData.d1.iOffGold, 0, 0, (REALM::REALM_TYPE)dData.d1.iWhoFrom))
				return FALSE;
			dData.d1.iOffGold += gold;
		}
		if (dData.d1.iReqHold && dData.d1.iAlliance == 0)
		{
			dData.d1.iReqHold = DemandHolding(HomeRealm, dData.d1.iOffHold);
			if (!dData.d1.iReqHold)
				return FALSE;
			dData.d1.iReqHold = StartPlace;
		}
		if (dData.d1.iReqTrib && dData.d1.iAlliance == 0)
		{
			gold = random(4)+1;
			if(!CanAfford(gold+dData.d1.iOffGold, 0, 0, (REALM::REALM_TYPE)dData.d1.iWhoFrom))
				return FALSE;
			dData.d1.iOffGold += gold;
		}

		response[curResponse].lWhichProvince = dData.d2.data1;
		response[curResponse].lWhichHold		 = dData.d2.data2;
		response[curResponse].lSuccess		 = dData.d2.data3;

		ShowRequest(D_OTHERDIPLOMACY, curResponse);

		return TRUE;
	}
}

/* ======================================================================== */
void YesOthersDiplomacy(LONG, LONG)
{
	ResumeTimeLimit();

	// send success
	// send make other offer only if against other remote !!
#ifdef _WINDOWS
	if ( !fOtherDipMP )
#endif
		SendMyMods(response[curResponse].lWhichAction, response[curResponse].lWhichProvince, response[curResponse].lWhichHold, 1);

	HideRequest(D_OTHERDIPLOMACY);
	RunRequests();
	fMenuClosed = TRUE;		// force DomainTurnUI redraw, sets fRedrawAll
	if (curResponse > 0)
		curResponse--;

	//----- Need to sync with other players that are waiting

#ifdef _WINDOWS
	if ( fOtherDipMP )
		//WaitSync( iMPS_SYNC5 );
		MapAIPlayerResponseWaitSync();

	fOtherDipMP = FALSE;
#endif

	// take any regency offered
	//realm[HomeRealm].regency += dData.d1.iOffRegy;
	
	// update tributes
	Tribute[dData.d1.iWhoFrom][HomeRealm] += dData.d1.iOffTrib;
	SetGameData(MP_TRIBUTE,dData.d1.iWhoFrom,HomeRealm, Tribute[dData.d1.iWhoFrom][HomeRealm], TRUE);
	Tribute[HomeRealm][dData.d1.iWhoFrom] += dData.d1.iReqTrib;
	SetGameData(MP_TRIBUTE,HomeRealm,dData.d1.iWhoFrom,Tribute[HomeRealm][dData.d1.iWhoFrom], TRUE);

	// NPC effect
//	if(!realm[CurrentRealm].mfIsPlayerCtrl())
//		NPCResult(TRUE);

	if (!realm[dData.d1.iWhoFrom].mfIsPlayerCtrl())
		NPCResult(TRUE);

	NoFlag = FALSE;

	// reconstruct the map
	if (dData.d1.iReqProv || dData.d1.iOffProv)
	{

		DisposBlock(iLgMap);


#ifdef _WINDOWS
		if ( IsMultiPlayer() )
		{
			iLgMap = GetResourceStd("MAP\\SMMAP.PCX", FALSE);
			fLgMap = SMALL;
		}
		else
#endif
		{
			if (fLowMemory)
			{
				fLowMemory = FALSE;
				iLgMap = GetResourceStd("MAP\\SMMAP.PCX", FALSE);
				fLgMap = SMALL;
				fLowMemory = TRUE;
			}
			else
			{
				iLgMap = GetResourceStd("MAP\\LGMAP.PCX", FALSE);
				fLgMap = LARGE;
			}
		}

		for (int j=FIRST_PROVINCE; j<PROVINCE_COUNT; ++j)
			DrawRoads((PROVINCE)j);

		DrawBorders(0, 0, 0, 0, ((BITMPTR)BLKPTR(iLgMap))->w, ((BITMPTR)BLKPTR(iLgMap))->h, FULL_SCALE);
	}
}

/* ======================================================================== */
// called when the IMPROVE button is pressed
/* ======================================================================== */
void NoOthersDiplomacy(LONG, LONG)
{
	ResumeTimeLimit();

#ifdef _WINDOWS
	// 970605 don't allow this button if diplomacy req by another player
	if ( fOtherDipMP || realm[dData.d1.iWhoFrom].mfIsPlayerCtrl())
		return;
#endif
	
	// send make other offer only if against other remote !!
#ifdef _WINDOWS
	if ( !fOtherDipMP )
#endif
		SendMyMods(response[curResponse].lWhichAction, response[curResponse].lWhichProvince, response[curResponse].lWhichHold, 3);

	HideRequest(D_OTHERDIPLOMACY);
	RunRequests();
	fMenuClosed = TRUE;		// force DomainTurnUI redraw, sets fRedrawAll
	if (curResponse > 0)
		curResponse--;

	if (NoFlag == FALSE)
	{
		++curResponse;
		NoFlag = NPCResult(FALSE);
		//NoFlag = TRUE;
	}
	else
		NoFlag = FALSE;


	//----- Need to sync with other players that are waiting
	//----- NoFlag means Diplomacy is back up 

#ifdef _WINDOWS
	if ( NoFlag == FALSE )
	{
		if ( fOtherDipMP )
			//WaitSync( iMPS_SYNC5 );
			MapAIPlayerResponseWaitSync();

		fOtherDipMP = FALSE;
	}
#endif

}

/* ======================================================================== */
void PaintOthersDiplomacy (LONG MenuCombo, LONG)
{
	LONG	k, i, tmpIndex, j;
	LONG	MenuId, ButtonId;
	LONG	x, y, w, h;

	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	LONG const menuResponse = MyRequestValue( D_OTHERDIPLOMACY );
	if (menuResponse == fERROR)
		return;

	// get position of text area
	if( GetButtonPosition( MenuId, 1, &x, &y ) == fERROR)
		return;

	// get size of menu
	if( GetButtonSize( MenuId, 1, &w, &h ) == fERROR)
		return;

	// set the data fields
	dData.d2.data1 = response[menuResponse].lWhichProvince;
	dData.d2.data2 = response[menuResponse].lWhichHold;
	dData.d2.data3 = response[menuResponse].lSuccess;

	print_textf(x+(w/2), y, DKBLUE,STRMGR_GetStr(STR_ACTNMENU_DIPLOMACY_REQUEST_FROM));
	y += 14;

	DrawLittleButton(x, y-1, 145, 12, FALSE);
	DrawShield(x+2, y, (REALM::REALM_TYPE)dData.d1.iWhoFrom);

	gprint_text(x+16, y, realm[dData.d1.iWhoFrom].mfGetName(), RED);

	if (realm[dData.d1.iWhoFrom].mfGetRegent() > 0)
	{
		char RegentName[80];
		strcpy(RegentName, regents[realm[dData.d1.iWhoFrom].mfGetRegent()].mfGetname());
		print_textf(x+REGENTNAME, y+4, DKBROWN,STRMGR_GetStr(STR_ACTNMENU_REGENT_FMT2) , RegentName);
	}
	y+=18;

	crease(x, y-2, 310, 1);
	print_textf(x, y, DKBROWN,STRMGR_GetStr(STR_ACTNMENU_CAPS_PETITIONS));
	y+=10;

	DrawBitmap(x, (SHORT)y, iCheck[dData.d1.iAlliance == 1], 0, 0, 20, 20);
	print_textf(x+30, y, DKBROWN, "^F02%s",STRMGR_GetStr(gsDiplomacySubj[0]));
	y += 13;

	DrawBitmap(x, (SHORT)y, iCheck[dData.d1.iAlliance == 2], 0, 0, 20, 20);
	print_textf(x+30, y, DKBROWN, "^F02%s",STRMGR_GetStr(gsDiplomacySubj[2]));
	y += 13;

	DrawBitmap(x, (SHORT)y, iCheck[dData.d1.iAlliance == 3], 0, 0, 20, 20);
	print_textf(x+30, y, DKBROWN, "^F02%s",STRMGR_GetStr(gsDiplomacySubj[4]));
	y += 13;

	DrawBitmap(x, (SHORT)y, iCheck[dData.d1.iReqGold > 0], 0, 0, 20, 20);
	print_textf(x+30, y, DKBROWN, "^F02%s",STRMGR_GetStr(gsDiplomacySubj[6]));
	if(dData.d1.iReqGold)
		print_textf(x+220, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_GB_FMT), dData.d1.iReqGold);
	y += 13;

	y+=2;
	crease(x, y-2, 310, 1);
	print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_DEMANDS));
	y+=10;

	DrawBitmap(x, (SHORT)y, iCheck[dData.d1.iReqProv > 0], 0, 0, 20, 20);
	print_textf(x+30, y, DKBROWN, "^F02%s",STRMGR_GetStr(gsDiplomacySubj[8]));
	if(dData.d1.iReqProv)
		print_textf(x, y, DKBLUE, "^F02^l300%s", province[dData.d1.iReqProv].name);
	y += 13;

	DrawBitmap(x, (SHORT)y, iCheck[dData.d1.iReqHold > 0], 0, 0, 20, 20);
	print_textf(x+30, y, DKBROWN, "^F02%s",STRMGR_GetStr(gsDiplomacySubj[10]));
	if(dData.d1.iReqHold)
	{
		// find the province for the holding
		tmpIndex = HoldingToProv(dData.d1.iReqHold);

		for(j=0; j<6; ++j)
			if(places[dData.d1.iReqHold].Icon == InfoReqType[j])
				break;

		print_textf(x, y, DKBLUE, "^F02^l300%s - %s %d", province[tmpIndex].name, STRMGR_GetStr(gsAllHold[j]),places[dData.d1.iReqHold].Level );
	}
	y += 13;

	DrawBitmap(x, (SHORT)y, iCheck[dData.d1.iReqTrib > 0], 0, 0, 20, 20);
	print_textf(x+30, y, DKBROWN, "^F02%s",STRMGR_GetStr(gsDiplomacySubj[12]));
	if(dData.d1.iReqTrib)
		print_textf(x+220, y, DKBROWN,STRMGR_GetStr(STR_ACTNMENU_GB_FMT), dData.d1.iReqTrib);
	y += 13;

	y+=2;
	crease(x, y-2, 310, 1);
	print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_OFFERS));
	y+=10;

	DrawBitmap(x, (SHORT)y, iCheck[dData.d1.iOffProv > 0], 0, 0, 20, 20);
	print_textf(x+30, y, DKBROWN, "^F02%s",STRMGR_GetStr(gsDiplomacySubj[14]));
	if(dData.d1.iOffProv)
		print_textf(x, y, DKBLUE, "^F02^l300%s", province[dData.d1.iOffProv].name);
	y += 13;

	DrawBitmap(x, (SHORT)y, iCheck[dData.d1.iOffHold > 0], 0, 0, 20, 20);
	print_textf(x+30, y, DKBROWN, "^F02%s",STRMGR_GetStr(gsDiplomacySubj[16]));
	if(dData.d1.iOffHold)
	{
		// find the province for the holding
		tmpIndex = HoldingToProv(dData.d1.iOffHold);

		for(j=0; j<6; ++j)
			if(places[dData.d1.iOffHold].Icon == InfoReqType[j])
				break;

		print_textf(x, y, DKBLUE, "^F02^l300%s - %s %d", province[tmpIndex].name, STRMGR_GetStr(gsAllHold[j]),places[dData.d1.iOffHold].Level);
	}
	y += 13;

	DrawBitmap(x, (SHORT)y, iCheck[dData.d1.iOffTrib > 0], 0, 0, 20, 20);
	print_textf(x+30, y, DKBROWN, "^F02%s",STRMGR_GetStr(gsDiplomacySubj[18]));
	if(dData.d1.iOffTrib)
		print_textf(x+220, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_GB_FMT), dData.d1.iOffTrib);
	y += 13;

	DrawBitmap(x, (SHORT)y, iCheck[dData.d1.iOffGold > 0], 0, 0, 20, 20);
	print_textf(x+30, y, DKBROWN, "^F02%s",STRMGR_GetStr(gsDiplomacySubj[20]));
	if(dData.d1.iOffGold)
		print_textf(x+220, y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_GB_FMT), dData.d1.iOffGold);
	y += 13;

//	DrawBitmap(x, (SHORT)y, iCheck[dData.d1.iOffRegy > 0], 0, 0, 20, 20);
//	print_textf(x+30, y, DKBROWN, "^F02%s",STRMGR_GetStr(gsDiplomacySubj[24]));
//	if(dData.d1.iOffRegy)
//		print_textf(x+220, y, DKBROWN,STRMGR_GetStr(STR_ACTNMENU_RP_FMT), dData.d1.iOffRegy);
//	y += 13;

	crease(x, y, 310, 1);

	y+=(63-13);

	{
	char RegentName[80];
	char RealmName[80];
	
	strcpy (RealmName, realm[dData.d1.iWhoFrom].mfGetName());
	if (realm[dData.d1.iWhoFrom].mfGetRegent() > 0)
		strcpy (RegentName, regents[realm[dData.d1.iWhoFrom].mfGetRegent()].mfGetname());
	else
		strcpy(RegentName, RealmName);

	print_textf(x, y, DKBROWN, STRMGR_GetStr(STR_DIPLOMACY_HELP_FMT), 
								RealmName, 
								RegentName, 
								RealmName);
	}

	init_gfont(FONT_SANS_10PT);
	for(i=2;i<=4;++i)
	{
		// get position of Previous button
		if( GetButtonPosition( MenuId, i, &x, &y ) == fERROR)
			return;

		// get size of Prev Button
		if( GetButtonSize( MenuId, i, &w, &h ) == fERROR)
			return;

		x += w/2;
		y += h/2+1;

		SHORT button_text = STR_NULL;
		LONG  color = WHITE;
		switch(i)
		{
			case 2: button_text = STR_GO_AWAY;
					break;
			
			case 3: button_text = STR_ACCEPT;
					break;
			
			case 4: button_text = STR_IMPROVE;
					if (realm[dData.d1.iWhoFrom].mfIsPlayerCtrl())
						color = GREY;
					break;
			
			default: break;
		}
		print_text_centered(x, y, STRMGR_GetStr(button_text), color);
	}
}

/* ======================================================================== */
void PaintDealMenu(LONG MenuCombo, LONG iRealm)
{
	LONG	x,y,mx,my;
	LONG	MenuId, ButtonId;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	if( GetButtonPosition( MenuId, 0, &mx, &my ) == fERROR)
		return;

	x = mx;
	y = my;

	{
	    char RegentName[80];
	    if (realm[iRealm].mfGetRegent() > 0)
		    strcpy(RegentName, regents[realm[iRealm].mfGetRegent()].mfGetname());
	    else
		    strcpy(RegentName, realm[iRealm].mfGetName());
    
	    print_textf(x+40, y+50, DKBROWN, STRMGR_GetStr(STR_DIPLOMACY_WANTS_BETTER_DEAL), RegentName);
	}
}

/* ======================================================================== */
static void DoDealMenu(LONG iRealm)
{
	// set the proc to null
	SetButtonProc  (D_QUESTION1, 0, PaintDealMenu, iRealm, 0 );
	
	// set the question text
	SetButtonLabel  (D_QUESTION1, QUESTION_TEXT, -1, BLACK );
	
	// turn on the middle button as OK
	SetButtonLabel  (D_QUESTION1, QUESTION_BTN1, STR_OK, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION1, QUESTION_BTN1, HideSubMenuWithClick, D_QUESTION1, NO_KEY );
	
	ShowMenu(D_QUESTION1);
}

/* ========================================================================
   Function    - ActionExpPoints
	Description - Doll out experience points for realm actions
	Returns     - number of points
	======================================================================== */
LONG ActionExpPoints(
	LONG RegentIndex, 
	ACTION_EXPTYPE ActionType, 
	LONG Modifier,
	BOOL fSend )
{
	LONG	Amount, A1, A2, Class1, Class2;
	
	// simple default answer
	Amount = 0;
	
	// don't advance Chaos
	if ( RegentIndex == UNKNOWN1 ||
		RegentIndex == UNKNOWN9 ||
		RegentIndex == UNKNOWN2 ||
		RegentIndex == UNKNOWN3 ||
		RegentIndex == UNKNOWN4 ||
		RegentIndex == UNKNOWN5 ||
		RegentIndex == UNKNOWN6 ||
		RegentIndex == UNKNOWN7 ||
		RegentIndex == UNKNOWN8 )
		return 0;
		
	if (playerstats[RegentIndex] != fERROR)
	{
		// collect primary/secondary class types
		Class1 = regents[RegentIndex].mfGetClass1(); 
		Class2 = regents[RegentIndex].mfGetClass2(); 
		
		// convert classes to table lookup values
		switch ( Class1 )
		{
		case FIGHTER:
		case PALADIN:
		case RANGER:
			Class1 = 0;
			break;
		case PRIEST:
			Class1 = 1;
			break;
		case THIEF:
		case BARD:
			Class1 = 2;
			break;
		case WIZARD:
			Class1 = 3;
			break;
		
		case NO_CLASS:
		default:
			// an error, just bail out
			return 0;
		}
		switch ( Class2 )
		{
		case FIGHTER:
		case PALADIN:
		case RANGER:
			Class2 = 0;
			break;
		case PRIEST:
			Class2 = 1;
			break;
		case THIEF:
		case BARD:
			Class2 = 2;
			break;
		case WIZARD:
			Class2 = 3;
			break;
		
		case NO_CLASS:
		default:
			// this character is no second class shmuck
			break;
		}
		
		// give them highest value of their two possible classes
		A1 = RealmExpPoints[ActionType][Class1];
		if ( Class2 != NO_CLASS )
		{
			A2 = RealmExpPoints[ActionType][Class2];
			Amount = MAX(A1,A2);
		}
		else
		{
			Amount = A1;
		}
			
		// check amount modifiers
		switch ( ActionType )
		{
		case RLM_EXP_REALM_SPELL:
		case RLM_EXP_RULE_PROV:
		case RLM_EXP_RULE_GUILD:
		case RLM_EXP_RULE_LAW:
		case RLM_EXP_RULE_SOURCE:
		case RLM_EXP_RULE_TEMPLE:
			Amount *= Modifier;
			break;
		}
		
		GainRealmExp(RegentIndex, Amount, TRUE, fSend);
	}

	return Amount;
}

/* ======================================================================== */


