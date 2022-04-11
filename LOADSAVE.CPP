/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: LoadSave.cpp
   Author:   Michael Branham, Donald Tsang

   ========================================================================
   Contains the following general functions:

   ®RM250¯======================================================================= */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#ifdef _WINDOWS
#include <windows.h>
#endif

#include <io.h>
#include "system.h"
#include <stdio.h>
#include "typedefs.h"
#include <sys\types.h>
#include <direct.h>
//#include <assert.h>

#include "actnmenu.hxx"
#include "charsel.hxx"
#include "game.h"
#include "sound.hxx"
#include "menu.h"
#include "dos.h"

#include "gamemap.hxx"  /* for realm[], regents[], province[], places[] */
#include "realm.hxx"    /* for RealmReaction[][] and RealmStatus[][] */
#include "units.hxx"    /* for units[] */
#include "playstat.hxx" /* for the PLAYER_STATS class */
#include "mapai.hxx"	/* for the realm-level AIs */

#include "regrelic.hxx"
#include "loadsave.hxx"
#include "regents.hxx"
#include "multimap.hxx"
#include "strmgr.h"
#include "multiui.hxx"
#include "report.hxx"
#include "context.hxx"
#include "panel.h"
#include "request.h"
#include "places.hxx"

#ifdef _WINDOWS
#include "winsys\mulplay.hxx"
#endif

// TODO: (fire lizard) have to include it here
#define F_OK 0

/* ------------------------------------------------------------------------
   Defines
   ------------------------------------------------------------------------ */
//#define	LOGCOMMENTSLEEP	Sleep(1);
#define  LOGCOMMENTSLEEP	{;}

#define FONT_TITL_40PT  13
#define MAX_SAVED_GAMES 50
#define FILENAME_LEN     8

#define LS_SAVE 1
#define LS_LOAD 2

#define BUFSIZE 2048
#define SAVEFILE_VERSION 0x00020051
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
static char saved_games[MAX_SAVED_GAMES+1][FILENAME_LEN+1];
static SCENARIO_INFO ScenInfo[MAX_SAVED_GAMES+1];

static LONG LoadSave;  // whether we're loading or saving
static LONG num_games = 0;
static LONG highlighted_choice = 0;
static char LoadSaveName[FILENAME_LEN+2];
static SHORT first_displayed = 0;

static BOOL fMultiLoad = FALSE;
static BOOL back_to_main = TRUE;
static BOOL in_question = FALSE;

static unsigned long helpbits = 0x00000000;

LONG master_game_type = GAME_NORMAL;

SCENARIO_INFO	ScenarioInfo = {0,0,0};

static LONG localWar;
static LONG localShow;

/* ------------------------------------------------------------------------
   Extern Variables
   ------------------------------------------------------------------------ */
extern "C" LONG print_global[];
extern LONG SaveMenuSize;
extern LONG LoadMenuSize;
extern "C" int iChatKey;
extern SHORT fRoundOver;			/* gamemap.cpp */
extern LONG global_action_turn;		/* mapai.cpp */
extern SHORT oldAction;				/* actnmenu.cpp */
extern BOOL char_selected;
extern LONG cheated;				/* how many times player has cheated */

extern SHORT usedFreeAction;		/* actnmenu.cpp */
extern SHORT usedFreeForge;
extern SHORT usedFreeMagic;
extern SHORT usedFreeAgitate;
extern SHORT usedFreeSpy;

extern BOOL fPlayerNeverDeclaredWar;

#define MAINT_MODE				2
#define BEFORE_LTACTION			0

// extern LONG mode_save;		/* gamemap.cpp */
extern SHORT fFinalRoll;	/* gamemap.cpp */
extern BOOL fUpdateDataArea;/* gamemap.cpp */
extern BOOL fDoActionIcon;
extern BOOL fHeldAction;
extern SHORT fLTAction;
extern LONG new_treasury;
extern LONG iGorgonPlot;	/* gamemap.cpp */
extern LONG iGorgonAlly;	/* gamemap.cpp */
extern LONG fControlMode;	/* gamemap.cpp, new as of 03/13/97 */

extern DECL_VECTOR_CLASS(BOOL,dtrn_ic_active);
extern DECL_VECTOR_CLASS(BOOL,dact_ic_active);
extern DECL_MATRIX_CLASS_S(UBYTE,Tribute,REALM::REALM_COUNT,REALM::REALM_COUNT);

extern SHORT fUnitsAllOn;
extern SHORT fHoldingsAllOn;

extern BOOL 	fPractice;
extern SHORT	fFadedOut;
extern SHORT	iSelectedUnit;
extern SHORT	iUnitInfo;
extern SHORT	iPlaceInfo;
extern SHORT	iProvInfo;
extern BOOL		fTutorialSelected;		// from gamemap.cpp
extern BOOL		fTutorialFirstBattle;	// from gamemap.cpp
extern void		ResetToNonTutorialAdventures(void);

/* ------------------------------------------------------------------------
   Internal Prototypes
   ------------------------------------------------------------------------ */
void ShowLoadMenu(BOOL fMulti);
void ShowSaveMenu(void);
LONG GetSaveGames(char names[MAX_SAVED_GAMES+1][FILENAME_LEN+1]);
void filename_chop(char *);
/* ------------------------------------------------------------------------
   Extern Prototypes
   ------------------------------------------------------------------------ */
extern void ClearOldInvestitureList(void);
extern void	SetCurAction(SHORT);
extern void InitDomainTurn (LONG);
extern void NewGameConfirmed2(BOOL fMulti);
extern void AdjustName (LONG _realm);

extern "C" {
	void SetRedrawMainMapLevel (void);

	void RandomLogComment ( char * szString );
}

/* ------------------------------------------------------------------------
   Code
   ------------------------------------------------------------------------ */

void PushLoadSaveButton(LONG MenuCombo)
{
	LONG	MenuId, ButtonId;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
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

void SaveGameFiles(FILE *fp)
{

    REALM_STRUCT	*pRmS;
    REGENT			*pRgS;
    PROV_TYPE		*pPrS;
    PLACE_TYPE		*pPlS;

    char buffer[BUFSIZE];
    LONG ver, i, j, saved, size, count;
    LONG dummy = 0;
    LONG newIPlaces = 1;

    /*-------------------------------------------------------------------*/
    /* Save version, whoami, date, action_turn, etc. */

	run_timers();
    ver = SAVEFILE_VERSION;
    fwrite(&ver, sizeof(LONG), 1, fp);
    fwrite(&HomeRealm,			sizeof(SHORT),			1, fp);
    fwrite(&date,				sizeof(LONG),			1, fp);

	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		char temp[200];
		sprintf (temp, "Saved game on turn %d.", date);
		RandomLogComment (temp);
	}
    fwrite(&action_turn[HomeRealm],	sizeof(LONG),			1, fp);
    fwrite(&iGorgonPlot,		sizeof(LONG),			1, fp);
    fwrite(&iGorgonAlly,		sizeof(LONG),			1, fp);
    fwrite(iLieutenants,		sizeof(UBYTE),			9, fp);
    fwrite(&helpbits,			sizeof(unsigned long),	1, fp);
    fwrite(&cheated,			sizeof(LONG),			1, fp);
	fwrite(&usedFreeAction,		sizeof(SHORT),			1, fp);
	fwrite(&usedFreeForge,		sizeof(SHORT),			1, fp);
	fwrite(&usedFreeMagic,		sizeof(SHORT),			1, fp);
	fwrite(&usedFreeAgitate,	sizeof(SHORT),			1, fp);
	fwrite(&usedFreeSpy,		sizeof(SHORT),			1, fp);
	
    localWar = !(fPlayerNeverDeclaredWar);		// so 0 is "TRUE"
    localShow = BUILD_LONG(fUnitsAllOn, fHoldingsAllOn);

	fwrite(&master_game_type,	sizeof(LONG),			1, fp);
	fwrite(&localWar,				sizeof(LONG),			1, fp);
	fwrite(&localShow,			sizeof(LONG),			1, fp);
	fwrite(&iOldSite,				sizeof(LONG),			1, fp);
	fwrite(&fControlMode,		sizeof(LONG),			1, fp);
	fwrite(&newIPlaces,			sizeof(LONG),			1, fp);
	fwrite(&ScenarioInfo,		sizeof(LONG),			1, fp);
	
	fwrite(&dummy,					sizeof(LONG),			1, fp);
	fwrite(&dummy,					sizeof(LONG),			1, fp);
	fwrite(&dummy,					sizeof(LONG),			1, fp);

    /*-------------------------------------------------------------------*/
    /* Save realm[] */

	run_timers();
    size = sizeof(REALM_STRUCT);
    count = REALM::REALM_COUNT;
    fwrite(&size, sizeof(LONG), 1, fp);
    fwrite(&count, sizeof(LONG), 1, fp);
    fwrite(realm, size, count, fp);

    /*-------------------------------------------------------------------*/
    /* Save regents[] */

	run_timers();
    size = sizeof(REGENT);
    count = CHARACTER_COUNT;
    fwrite(&size, sizeof(LONG), 1, fp);
    fwrite(&count, sizeof(LONG), 1, fp);
    fwrite(regents, size, count, fp);

    /*-------------------------------------------------------------------*/
    /* Save RealmReaction[][] */

	run_timers();
    size = REALM::REALM_COUNT * sizeof(UBYTE);
    count = LAND_REALM_COUNT;
    fwrite((char *) &size, sizeof(LONG), 1, fp);
    fwrite((char *) &count, sizeof(LONG), 1, fp);
    fwrite((char *) RealmReaction, size, count, fp);

    /*-------------------------------------------------------------------*/
    /* Save RealmStatus[][] */

	run_timers();
    size = REALM::REALM_COUNT * sizeof(UBYTE);
    count = LAND_REALM_COUNT;
    fwrite(&size, sizeof(LONG), 1, fp);
    fwrite(&count, sizeof(LONG), 1, fp);
    fwrite(RealmStatus, size, count, fp);

    /*-------------------------------------------------------------------*/
    /* Save province[] */

	run_timers();
    size = sizeof(PROV_TYPE);
    count = PROVINCE_COUNT;
    fwrite(&size, sizeof(LONG), 1, fp);
    fwrite(&count, sizeof(LONG), 1, fp);
    fwrite(province, size, count, fp);

    /*-------------------------------------------------------------------*/
    /* Save places[] */

	run_timers();
    size = sizeof(PLACE_TYPE);
    count = MAX_PLACES;
    fwrite(&size, sizeof(LONG), 1, fp);
    fwrite(&count, sizeof(LONG), 1, fp);
    fwrite(places, size, count, fp);

    /*-------------------------------------------------------------------*/
    /* Save units[] */

	run_timers();
    size = sizeof(UNIT_TYPE);
    count = MAX_UNITS;
    fwrite(&size, sizeof(LONG), 1, fp);
    fwrite(&count, sizeof(LONG), 1, fp);
    fwrite(units, size, count, fp);

    /*-------------------------------------------------------------------*/
    /* Save npc_ai[] */

    run_timers();
    size = sizeof(NPC_AI);
    count = LAND_REALM_COUNT;
    fwrite(&size, sizeof(LONG), 1, fp);
    fwrite(&count, sizeof(LONG), 1, fp);
    fwrite(npc_ai, size, count, fp);

    /*-------------------------------------------------------------------*/
    /* Save RealmOrder[] */

    size = sizeof(UBYTE);
    count = LAND_REALM_COUNT;
    fwrite(&size, sizeof(LONG), 1, fp);
    fwrite(&count, sizeof(LONG), 1, fp);
    fwrite(RealmOrder, size, count, fp);
	run_timers();

    /*-------------------------------------------------------------------*/
    /* Save Tribute[][] */

    run_timers();
    size = REALM::REALM_COUNT * sizeof(UBYTE);
    count = REALM::REALM_COUNT;
    fwrite(&size, sizeof(LONG), 1, fp);
    fwrite(&count, sizeof(LONG), 1, fp);
    fwrite(Tribute, size, count, fp);

    /*-------------------------------------------------------------------*/
    /* Save advsite[] */

    run_timers();
    size = sizeof(ADV_SITE);
    count = MAX_SITES;
    fwrite(&size, sizeof(LONG), 1, fp);
    fwrite(&count, sizeof(LONG), 1, fp);
    fwrite(advsite, size, count, fp);

    /*-------------------------------------------------------------------*/
    /* Save playerstats[]	*/

    StatWriteAllTo(fp);
	run_timers();


	/*-------------------------------------------------------------------*/
	// Save scenario specific realm names
	// [abc] 10/27/97

	if (ScenarioInfo.RealmNames)
	{
		run_timers();
		fwrite(RealmNameBuffer, 22, REALM::REALM_COUNT, fp);
	}

}


void LoadGameFiles(FILE *fp)
{
    REALM_STRUCT	*pRmS;
    REGENT			*pRgS;
    PROV_TYPE		*pPrS;
    PLACE_TYPE		*pPlS;
    char buffer[BUFSIZE];
    LONG ver, i, j, loaded, tmp, size, count;
    LONG dummy, newIPlaces;
    BOOL			oldSaveFile = FALSE;

    run_timers();
    fread(&ver, sizeof(LONG), 1, fp);
    if (ver == SAVEFILE_VERSION - 1)
    {
    	oldSaveFile = TRUE;
    }
    else if (ver != SAVEFILE_VERSION)
    {
        // error display
        return;
    };

    fread(&HomeRealm, sizeof(SHORT), 1, fp);
    fread(&date,				sizeof(LONG),			1, fp);
    fread(&dummy,				sizeof(LONG),			1, fp);
	 for (i=0; i<LAND_REALM_COUNT; i++)
    	action_turn[i] = dummy;
    fread(&iGorgonPlot,			sizeof(LONG),			1, fp);
    fread(&iGorgonAlly,			sizeof(LONG),			1, fp);
    fread(iLieutenants,			sizeof(UBYTE),			9, fp);
    fread(&helpbits,			sizeof(unsigned long),	1, fp);
    fread(&cheated,				sizeof(LONG),			1, fp);
	fread(&usedFreeAction,		sizeof(SHORT),			1, fp);
	fread(&usedFreeForge,		sizeof(SHORT),			1, fp);
	fread(&usedFreeMagic,		sizeof(SHORT),			1, fp);
	fread(&usedFreeAgitate,		sizeof(SHORT),			1, fp);
	fread(&usedFreeSpy,			sizeof(SHORT),			1, fp);

	fread(&master_game_type,	sizeof(LONG),			1, fp);
	
	fread(&localWar,			sizeof(LONG),			1, fp);
    fPlayerNeverDeclaredWar = !(localWar);

	fread(&localShow,			sizeof(LONG),			1, fp);
    SPLIT_COMBO(localShow, fUnitsAllOn, fHoldingsAllOn);
	fread(&iOldSite,			sizeof(LONG),			1, fp);
#ifdef _WINDOWS
	if (IsMultiPlayer())
    {
		fread(&dummy,				sizeof(LONG),			1, fp);
		
//		if ((dummy&0xff) != 0)	// not saved as basic game
//    		++date;
    }
	else
#endif

	fread(&fControlMode,		sizeof(LONG),			1, fp);
	fread(&newIPlaces,		sizeof(LONG),			1, fp);
	fread(&ScenarioInfo,		sizeof(LONG),			1, fp);

	fread(&dummy,				sizeof(LONG),			1, fp);
	fread(&dummy,				sizeof(LONG),			1, fp);
	fread(&dummy,				sizeof(LONG),			1, fp);

    /*-------------------------------------------------------------------*/
    /* Load realm[] */

    run_timers();
    printf("Loading realm[]\n");
    fread((char *) &size, sizeof(LONG), 1, fp);
    assert(size == sizeof(REALM_STRUCT));
    fread((char *) &count, sizeof(LONG), 1, fp);
    pRmS = (REALM_STRUCT *) buffer;

    loaded = BUFSIZE / size;
    if (loaded > count)
        loaded = count;
    fread(buffer, size, loaded, fp);
    for (i=0; i < count; i++)
    {
		if (i >= loaded)
        {
            tmp = loaded;
            loaded += BUFSIZE/size;
            if (loaded > count)
                loaded = count;
            fread(buffer, size, (loaded-tmp), fp);
            pRmS = (REALM_STRUCT *) buffer;
        }
		realm[i].mfSetNameX		 	(pRmS->mfGetNameX());
		realm[i].mfSetNameY			(pRmS->mfGetNameY());
		realm[i].mfSetTitleSize		(pRmS->mfGetTitleSize());
		realm[i].mfSetRealmWidth	(pRmS->mfGetRealmWidth());
		realm[i].mfSetTitleSpread	(pRmS->mfGetTitleSpread());
		realm[i].mfSetTitleTrans	(pRmS->mfGetTitleTrans());
		realm[i].mfSetRegent			(pRmS->mfGetRegent());
		realm[i].mfSetTreasury		(pRmS->mfGetTreasury());
		realm[i].mfSetRegency		(pRmS->mfGetRegency());
		realm[i].mfSetCourt			(pRmS->mfGetCourt());
		realm[i].mfSetExists			(pRmS->mfExists());
		realm[i].mfSetShowUnits		(pRmS->mfShowUnits());
		realm[i].mfSetShowHoldings	(pRmS->mfShowHoldings());
		realm[i].mfSetPlayable		(pRmS->mfIsPlayable());
		realm[i].mfSetPlayerCtrl	(pRmS->mfIsPlayerCtrl());
		realm[i].mfSetDMCtrl			( (pRmS->mfIsDMCtrl())?1:0 );
		realm[i].mfSetSurplusIncome(pRmS->mfGetSurplusIncome());
		++pRmS;  /* advance */
		run_timers();
    }


    /*-------------------------------------------------------------------*/
    /* Load regents[] */

    run_timers();
    printf("Loading regents[]\n");
    fread((char *) &size, sizeof(LONG), 1, fp);
    assert(size == sizeof(REGENT));
    fread((char *) &count, sizeof(LONG), 1, fp);
    pRgS = (REGENT *) buffer;
    loaded = BUFSIZE / size;
    if (loaded > count)
        loaded = count;
    fread(buffer, size, loaded, fp);
    for (i=0; i < count; i++)
    {
        if (i >= loaded)
        {
            tmp = loaded;
            loaded += BUFSIZE/size;
            if (loaded > count)
                loaded = count;
            fread(buffer, size, (loaded-tmp), fp);
            pRgS = (REGENT *) buffer;
        }
        regents[i].mfSetunit(pRgS->mfGetunit());
        regents[i].mfSetRealm(pRgS->mfGetOriginalRealm());
        regents[i].mfSetLevel1(pRgS->mfGetLevel1());
        regents[i].mfSetLevel2(pRgS->mfGetLevel2());
        regents[i].mfSetBL_strength(pRgS->mfGetBL_strength());
        ++pRgS;  /* advance */
		run_timers();
    }

    /*-------------------------------------------------------------------*/
    /* Load RealmReaction[][] */

    run_timers();
    printf("Loading RealmReaction[][]\n");
    fread((char *) &size, sizeof(LONG), 1, fp);
    assert(size == REALM::REALM_COUNT * sizeof(UBYTE));
    fread((char *) &count, sizeof(LONG), 1, fp);
    assert(count == LAND_REALM_COUNT);
    fread((char *) RealmReaction, size, count, fp);


    /*-------------------------------------------------------------------*/
    /* Load RealmStatus[][] */

	run_timers();
    printf("Loading RealmStatus[][]\n");
    fread((char *) &size, sizeof(LONG), 1, fp);
    assert(size == REALM::REALM_COUNT * sizeof(UBYTE));
    fread((char *) &count, sizeof(LONG), 1, fp);
    assert(count == LAND_REALM_COUNT);
    fread((char *) RealmStatus, size, count, fp);


    /*-------------------------------------------------------------------*/
    /* Load province[] */

	run_timers();
    printf("Loading province[]\n");
    fread((char *) &size, sizeof(LONG), 1, fp);
    assert(size == sizeof(PROV_TYPE));
    fread((char *) &count, sizeof(LONG), 1, fp);
    pPrS = (PROV_TYPE *) buffer;
    loaded = BUFSIZE / size;
    if (loaded > count)
        loaded = count;
    fread(buffer, size, loaded, fp);
    for (i=0; i < count; i++)
    {
        if (i >= loaded)
        {
            tmp = loaded;
            loaded += BUFSIZE/size;
            if (loaded > count)
                loaded = count;
            fread(buffer, size, (loaded-tmp), fp);
            pPrS = (PROV_TYPE *) buffer;
        }
        for (j=0; j < 7; j++)
            province[i].Border[j] = pPrS->Border[j];

        province[i].Realm        = pPrS->Realm;
        province[i].OccupRealm   = pPrS->OccupRealm;
        province[i].CivLevel     = pPrS->CivLevel;
        province[i].CurCivLevel  = pPrS->CurCivLevel;
        province[i].MagLevel     = pPrS->MagLevel;
        province[i].TaxLevel     = pPrS->TaxLevel;
        province[i].Loyalty      = pPrS->Loyalty;
        province[i].HumanPop		= pPrS->HumanPop;
        province[i].ElfPop			= pPrS->ElfPop;
        province[i].DwarfPop		= pPrS->DwarfPop;
        province[i].GoblinPop		= pPrS->GoblinPop;
        province[i].LevyMustered = pPrS->LevyMustered;
        province[i].Contested    = pPrS->Contested;
        province[i].CastleSptd   = pPrS->CastleSptd;
        province[i].Blessed      = pPrS->Blessed;
        province[i].Blighted     = pPrS->Blighted;
        province[i].Honest       = pPrS->Honest;
        province[i].Warded       = pPrS->Warded;
        province[i].Dispel       = pPrS->Dispel;
        province[i].DispelLvl    = pPrS->DispelLvl;
        province[i].FirstPlace   = pPrS->FirstPlace;
        ++pPrS;  /* advance */
		run_timers();
    }


    /*-------------------------------------------------------------------*/
    /* Load places[] */

	run_timers();
    printf("Loading places[]\n");
    fread((char *) &size, sizeof(LONG), 1, fp);
    assert(size == sizeof(PLACE_TYPE));
    fread((char *) &count, sizeof(LONG), 1, fp);
    pPlS = (PLACE_TYPE *) buffer;
    loaded = BUFSIZE / size;
    if (loaded > count)
        loaded = count;
    fread(buffer, size, loaded, fp);
    for (i=0; i < count; i++)
    {
        if (i >= loaded)
        {
            tmp = loaded;
            loaded += BUFSIZE/size;
            if (loaded > count)
                loaded = count;
            fread(buffer, size, (loaded-tmp), fp);
            pPlS = (PLACE_TYPE *) buffer;
        }
        places[i].iRoute1   		= pPlS->iRoute1;
        places[i].iRoute2   		= pPlS->iRoute2;
        places[i].iRoute3   		= pPlS->iRoute3;
        places[i].Realm     		= pPlS->Realm;
        places[i].Level     		= pPlS->Level;
        places[i].Contested 		= pPlS->Contested;
        places[i].Besieged  		= pPlS->Besieged;
        places[i].NextPlace		    = pPlS->NextPlace;
        places[i].fSiteDisplayed	= pPlS->fSiteDisplayed;
        places[i].fRelicFound		= pPlS->fRelicFound;
        places[i].Icon				= pPlS->Icon;
        places[i].x					= pPlS->x;
        places[i].y					= pPlS->y;
//      places[i].iName				= pPlS->iName; // never nonzero

//		places[i].iIconBitm			= fERROR;  // InitDomainTurn will fill in

		if (places[i].Icon != NO_MAP_ICON)
		{
			char filename[50];
			sprintf(filename,"UI\\%s.PCX",GameIcons[places[i].Icon+(((places[i].Level<8)?places[i].Level:7)/2)]);
			places[i].iIconBitm = GetResourceStd(filename, FALSE);
		}
		else
			places[i].iIconBitm = fERROR;


        ++pPlS;  /* advance */
		run_timers();
    }

    /*-------------------------------------------------------------------*/
    /* Load units[] */

	run_timers();
    printf("Loading units[]\n");
    fread((char *) &size, sizeof(LONG), 1, fp);
	assert(size == sizeof(UNIT_TYPE));
    fread((char *) &count, sizeof(LONG), 1, fp);

    loaded = 0;
    while (loaded < count)
    {
        i = fread(((char *) units) + loaded * size, size, count-loaded, fp);
        if (i < 0)
        {
            perror("freading units array");
            exit(1);
        }
        loaded += i;
		run_timers();
    }

//	We changed the count of the Units array.  This code should make us
//	compatible with old save files...
    
    if (count < MAX_UNITS)
    {
    	for (/* count */; count < MAX_UNITS; count++)
    	{
			SetGameData(MP_UNITS, MPUNITS_PROVINCE, count, NO_PROVINCE, FALSE );
			SetGameData(MP_UNITS, MPUNITS_REALM,    count, REALM::NO_COUNTRY, FALSE );
			SetGameData(MP_UNITS, MPUNITS_ICON,     count, NO_MAP_ICON, FALSE );
			SetGameData(MP_UNITS, MPUNITS_JOINED,   count, 0, FALSE );
			SetGameData(MP_UNITS, MPUNITS_NEXTUNIT, count, -1, FALSE );
			SetGameData(MP_UNITS, MPUNITS_SALARY,   count, 0, FALSE );
    	}
		run_timers();
    }
    
    for (i=0; i < count; i++)
    {
		char iconname[50];
		if (units[i].Icon != NO_MAP_ICON)
		{
			sprintf(iconname,"UI\\%s.PCX",GameIcons[units[i].Icon]);
			units[i].iIconBitm = GetResourceStd(iconname, FALSE);
		}
		else
			units[i].iIconBitm = fERROR;
		
		if (oldSaveFile)
		{
			SetGameData(MP_UNITS, MPUNITS_SALARY, i, 0, FALSE);
		}
		run_timers();
    }


    /*-------------------------------------------------------------------*/
    /* Load npc_ai[] */

	run_timers();
    NPC_AI *pNA = (NPC_AI *) buffer;
    fread(&size, sizeof(LONG), 1, fp);
    assert(size == sizeof(NPC_AI));
    fread(&count, sizeof(LONG), 1, fp);
    assert(count == LAND_REALM_COUNT);

    fread(npc_ai, size, count, fp);
    //for (i=0; i < count; i++)
    //    npc_ai[i].CurrentMode = pNA[i].CurrentMode;

    /*-------------------------------------------------------------------*/
    /* Load RealmOrder[] */

	run_timers();
    fread(&size, sizeof(LONG), 1, fp);
    assert(size == sizeof(UBYTE));
    fread(&count, sizeof(LONG), 1, fp);
    assert(count == LAND_REALM_COUNT);
    fread(RealmOrder, size, count, fp);

// Okay, this is a huge hack.  If the Tributes array isn't here, set it
// to all 0s and read the rest of the file anyway.

	run_timers();
	fread(&size, sizeof(LONG), 1, fp);
	fread(&count, sizeof(LONG), 1, fp);
	
	if (size == sizeof(ADV_SITE) && count == MAX_SITES)  // not saved; reset
	{
		LONG i, j;
		for (i=0; i < REALM::REALM_COUNT; i++)
		{
			for (j=0; j < REALM::REALM_COUNT; j++)
			{
				Tribute[i][j] = 0;
			}
		}
	}
	else
	{
		assert(size == (REALM::REALM_COUNT * sizeof(UBYTE)));
		assert(count == REALM::REALM_COUNT);
		fread((char *)Tribute, size, count, fp);
		
		run_timers();
    	fread(&size, sizeof(LONG), 1, fp);
    	assert(size == sizeof(ADV_SITE));
    	fread(&count, sizeof(LONG), 1, fp);
    	assert(count == MAX_SITES);
    }

    /*-------------------------------------------------------------------*/
    /* Load advsite[].available */

    fread(buffer, size, count, fp);

    ADV_SITE *pAS = (ADV_SITE *) buffer;
    for (i=0; i < MAX_SITES; i++)
    {
		advsite[i].available = pAS[i].available;
		if (newIPlaces)
		{
			advsite[i].iPlaces = pAS[i].iPlaces;	// else not valid
		}
	}

	/*-------------------------------------------------------------------*/
	/* Load playerstats[] */

	StatReadAllFrom(fp);

	/*-------------------------------------------------------------------*/
	// Load scenario specific realm names
	// [abc] 10/27/97

	if (ScenarioInfo.RealmNames)
	{
		run_timers();
		fread(RealmNameBuffer, 22, REALM::REALM_COUNT, fp);
	}

	// fix up realm names
	for (i=REALM::FIRST_REALM; i < LAND_REALM_COUNT; i++)
		AdjustName(i);

    /*-------------------------------------------------------------------*/
    /* update regent info */


	if ( fMultiLoad )
	{
		int i;

		for ( i = 0; i < 8; i++ )
		{
			iLieutenants[i] = 0;
		}

	}


	ActiveRegent = realm[HomeRealm].mfGetRegent();
	realm[HomeRealm].mfSetPlayerCtrl(TRUE);
	
	for (i=0; i < 6; i++)
		dtrn_ic_active[i] = TRUE;
	for (i=1; i < 19; i++)
		dact_ic_active[i] = TRUE;

	char cur_sav[25] = "savegame\\";
	strcat(cur_sav, STRMGR_GetStr(STR_SAVEGAME));

	if (0 == access(cur_sav, F_OK))
		remove(cur_sav);
}


void ScrollGame(LONG dir, LONG)
{
    first_displayed += dir;
    if (first_displayed > num_games - 6)
        first_displayed = num_games - 6;
    if (first_displayed < 0)
        first_displayed = 0;
}

void SelectGame(LONG game, LONG)
{
    highlighted_choice = game;

    // add a space to be compatible with the chat-input routine
    sprintf(LoadSaveName, "%s ", saved_games[game]);
    if (LoadSave == LS_LOAD)
        return;
}

/* ========================================================================
   Function    - LoadGame
   Description - Loads a game in progress
   Returns     -
   ======================================================================== */
void LoadGame(LONG, LONG)
{
	if ( mouse_button == 2 ) //---- We need help
	{
		SystemHelp( STR_MAIN1_HELP_LOAD_TITLE,
					STR_MAIN1_HELP_LOAD_TEXT, H_Load_Saved_Game, 0, 0 );
	}
	else
	{
		if (!fDomainTurn)   // can only do at map level
			return;
		
#ifdef _WINDOWS
		//---- If this is a multiplayer game then shut down the game if this is done

		if ( IsMultiPlayer() )
		{
			ShowMultiQuit( iMPQ_LOADGAME, 0 );
			return;
		}
#endif
		
		LoadSave = LS_LOAD;
		in_question = FALSE;
	
		// hide main menu
	
//		GAMEToggleMainMenu(FALSE,0);
		back_to_main = TRUE;

		// wait for menu to minimize
		RunMenus();
		RunMenus();

		ShowLoadMenu(FALSE);
	}
}


/* ========================================================================
   Function    - LoadGameMulti
   Description - Loads a game in progress for MULTIPLAYER ONLY
   Returns     -
   ======================================================================== */
void LoadGameMulti(LONG, LONG)
{
		
	LoadSave = LS_LOAD;
	in_question = FALSE;
	
	// hide main menu
	
//	GAMEToggleMainMenu(FALSE,0);

	back_to_main = FALSE;

	// wait for menu to minimize
	RunMenus();
	RunMenus();

	ShowLoadMenu(TRUE);

}



void PaintCantSave(LONG, LONG)
{
	LONG menu_x, menu_y, menu_w, menu_h;
	char buffer[200];
	
	GetButtonPosition( D_QUESTION1, QUESTION_TEXT, &menu_x, &menu_y );
	GetButtonSize(D_QUESTION1, QUESTION_TEXT, &menu_w, &menu_h);
	
	strcpy(buffer, STRMGR_GetStr(STR_SAVE_NOCURGAME));
	print_textf(menu_x + 20, menu_y + menu_h/2, BLACK, buffer);
	
}

void CantSaveMsg()
{
	in_question = TRUE;
	
	// set the question text
	SetButtonLabel  (D_QUESTION1, QUESTION_TEXT, -1, 0 );
	SetButtonProc  (D_QUESTION1, 0, PaintCantSave, 0, NO_KEY );
	
	// turn on the middle button as DONE
	SetButtonLabel  (D_QUESTION1, QUESTION_BTN1, STR_OK, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION1, QUESTION_BTN1, HideSubMenuWithClick, D_QUESTION1, D_KEY_OK );
	
	ShowMenu(D_QUESTION1);
	return;
}

/* ========================================================================
   Function    - SaveGame
   Description - Saves a game in progress
   Returns     -
   ======================================================================== */
void SaveGame(LONG, LONG)
{

//#ifdef _WINDOWS
//	//---- Dont allow if this is a multiplayer game
//	if ( IsMultiPlayer() )
//	{
//		return;		
//	}
// #endif

	if ( mouse_button == 2 )
	{
		SystemHelp( STR_MAIN1_HELP_SAVE_TITLE,
					STR_MAIN1_HELP_SAVE_TEXT, H_Save_Current_Game, 0, 0 );
	}
	else //---- We need help
	{

		if (!fDomainTurn || !char_selected)
			return;
//		if (dturn_mode != MIN_MODE && dturn_mode != TAXLVL_MODE && dturn_mode != MAINT_MODE)
//			return;
		if (master_game_type == GAME_INTRO)	
			return;

		// Doesn't exist.
		char cur_sav[25] = "savegame\\";
		strcat(cur_sav, STRMGR_GetStr(STR_SAVEGAME));
		if ( 0 != access(cur_sav, F_OK))
		{
			CantSaveMsg();
			return;
		}

		LoadSave = LS_SAVE;
		in_question = FALSE;

		// turn off main menu
	
//		GAMEToggleMainMenu(FALSE,0);
		back_to_main = TRUE;

		// wait for menu to minimize
		RunMenus();
		RunMenus();

		ShowSaveMenu();
	}
}


/* ========================================================================
   Function    - ShowLoadMenu
   Description - Displays the load menu
   Returns     -
   ======================================================================== */
void ShowLoadMenu(BOOL fMulti)
{
	 //---- Are we doing this with others
	 fMultiLoad = fMulti;

    num_games = GetSaveGames(saved_games);
    highlighted_choice = -1;
    memset(LoadSaveName, '\0', FILENAME_LEN);
    LoadSaveName[0] = ' ';
    ShowMenu(D_LOAD_MENU);
}




/* ========================================================================
   Function    - ShowSaveMenu
   Description - Displays the Save menu
   Returns     -
   ======================================================================== */
void ShowSaveMenu()
{
    num_games = GetSaveGames(saved_games);
    highlighted_choice = -1;
    memset(LoadSaveName, '\0', FILENAME_LEN);
    LoadSaveName[0] = ' ';
    clear_key_status(0);
    iChatKey = 0;
    ShowMenu(D_SAVE_MENU);
}

/* ========================================================================
   Function    - HideLoadMenu
   Description - Hides the load menu
   Returns     -
   ======================================================================== */
void HideLoadMenu(LONG MenuCombo, LONG)
{
	LONG	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	del_region(ScrollGame, 0);
	del_region(SelectGame, 0);
	
	// Click is done by the button itself.
	//PushLoadSaveButton(MenuCombo);
	
	HideMenu(MenuId);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	
	//---- we are in multiplayer bring back up game type screen

	if ( fMultiLoad )
	{
		ShowWaitMultiType(0, 0);
	}
	
	if (!back_to_main)
	    GAMEToggleMainMenu(FALSE,0);

}

void CancelLoad(LONG MenuCombo, LONG)
{
	PushLoadSaveButton(MenuCombo);
#ifdef _WINDOWS
	// we canceled Multiload; just start a new game.
	if (fMultiLoad)
	{
		fMultiLoad = FALSE;
		HideLoadMenu(MenuCombo,0);
		AMultiPlayer.SetGameType( iMPGT_NEW );
		AMultiPlayer.SendGameType();
		AMultiPlayer.InitPost2();
	}	
	else
#endif	
		HideLoadMenu(MenuCombo,0);

}

void HideAndReturn(LONG MenuCombo, LONG)
{
	LONG	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	PushLoadSaveButton(MenuCombo);
	
	HideMenu(MenuId);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	
    GAMEToggleMainMenu(FALSE,0);
}

/* ========================================================================
   Function    - HideSaveMenu
   Description - Hides the Save menu
   Returns     -
   ======================================================================== */
void HideSaveMenu(LONG MenuCombo, LONG)
{
	LONG	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	del_region(ScrollGame, 0);
	del_region(SelectGame, 0);
	
	// click the button
	// The button does the push.
	// PushLoadSaveButton(MenuCombo);
	
	HideMenu(MenuId);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	
	if (!back_to_main)  // saved.  tell them.
	{
		print_global[0] = date+1;
		// set the question text
		SetButtonLabel  (D_QUESTION1, QUESTION_TEXT, STR_SAVE_DONE, BLACK );
		SetButtonProc  (D_QUESTION1, 0, NULL, 0, NO_KEY );
	
		// turn on the middle button as OK
		SetButtonLabel  (D_QUESTION1, QUESTION_BTN1, STR_OK, BTN_LABEL_COLOR );
		SetButtonProc   (D_QUESTION1, QUESTION_BTN1, HideAndReturn, 0, D_KEY_OK );
	
		ShowMenu(D_QUESTION1);
		RunMenus();
	}
}

void CancelSave(LONG MenuCombo, LONG)
{
	PushLoadSaveButton(MenuCombo);
	HideSaveMenu(MenuCombo,0);
}

void DeleteGameReal(LONG, LONG);

void OverwriteGame (LONG MenuCombo, LONG button)
{
	LONG	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	iChatKey = 0;
	
    char *p;
    if (strlen(LoadSaveName) > 0)
        p = LoadSaveName + strlen(LoadSaveName) - 1;
    else
        p = LoadSaveName;

    if (*p != ' ' && *p != '_')
        strcat(LoadSaveName, " ");
	
	// click the button
	PushLoadSaveButton(MenuCombo);
	
	HideMenu(MenuId);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	
	in_question = FALSE;
	
	SetButtonLabel  (D_QUESTION2, QUESTION_TEXT, -1, BLACK );
	if(button == QUESTION_BTN1)  // Yes, overwrite
	{
	    char tf[FILENAME_LEN + 2];
	    strcpy(tf, LoadSaveName);
	    filename_chop(tf);
	    for (SHORT i = 0; i < num_games; i++)
	    {
	        if (!stricmp(tf, saved_games[i]))
	        {
	        	highlighted_choice = i;
	        	break;
	        }
	    }
	    DeleteGameReal(MenuCombo, QUESTION_BTN1);
		
		LONG tmpcombo = BUILD_LONG(D_SAVE_MENU, 2);
	    SaveGameDo(tmpcombo, 1);
	}
	fUpdateDataArea=TRUE;
}

/*	======================================================================== */
void QueryOverwrite ()
{
	in_question = TRUE;
	
	// set the question text
	SetButtonLabel  (D_QUESTION2, QUESTION_TEXT, STR_LOADSAVE_OVERWRITE, BLACK );
	SetButtonProc  (D_QUESTION2, 0, NULL, 0, NO_KEY );
	
	// turn on the first button as YES
	SetButtonLabel  (D_QUESTION2, QUESTION_BTN1, STR_YES, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION2, QUESTION_BTN1, OverwriteGame, QUESTION_BTN1, D_KEY_YES );
	
	// turn on the last button as CANCEL
	SetButtonLabel  (D_QUESTION2, QUESTION_BTN2, STR_CANCEL, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION2, QUESTION_BTN2, OverwriteGame, QUESTION_BTN2, D_KEY_CANCEL );
	
	ShowMenu(D_QUESTION2);
	return;
}

void HideMaxReached(LONG MenuCombo, LONG)
{
	LONG	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	PushLoadSaveButton(MenuCombo);
	
	HideMenu(MenuId);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	
	SetButtonLabel  (MenuId, QUESTION_TEXT, -1, BLACK );
	in_question = FALSE;
	return;
}

void ShowMaxReached()
{
	in_question = TRUE;
	
	// set the question text
	SetButtonLabel  (D_QUESTION1, QUESTION_TEXT, STR_LOADSAVE_MAXREACHED, BLACK );
	SetButtonProc  (D_QUESTION1, 0, NULL, 0, NO_KEY );
	
	// turn off the middle button
	SetButtonLabel  (D_QUESTION1, QUESTION_BTN1, STR_BACK, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION1, QUESTION_BTN1, HideMaxReached, 0, D_KEY_BACK );
	
	ShowMenu(D_QUESTION1);
	RunMenus();
}



void filename_chop(char *filename)
{
    char *p;

    while (strlen(filename) > 0)
    {
        p = filename + strlen(filename) - 1;
        if (*p != ' ' && *p != '_')
            break;
        *p = '\0';
    }
}


// borrowed enough code here so "HideMaxReached" works for this, too.
void ShowDeleteFailed()
{
	in_question = TRUE;
	
	// set the question text
	SetButtonLabel  (D_QUESTION1, QUESTION_TEXT, STR_LOADSAVE_DELETEFAILED, BLACK );
	SetButtonProc  (D_QUESTION1, 0, NULL, 0, NO_KEY );
	
	// turn off the middle button
	SetButtonLabel  (D_QUESTION1, QUESTION_BTN1, STR_BACK, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION1, QUESTION_BTN1, HideMaxReached, 0, D_KEY_BACK );
	
	ShowMenu(D_QUESTION1);
	RunMenus();
}

BOOL CopyAFile(const char * fromname, const char * toname)
{
    char buffer[2048];
    FILE *fp, *fp2;

    fp2 = fopen(fromname, "rb");
    if (fp2 != NULL)
    	fp = fopen(toname, "wb");

    if (fp2 == NULL || fp == NULL)
    {
        if (fp2 != NULL)
        	fclose(fp2);

        return TRUE;
    }

    LONG len;

    while ((len = fread(buffer, sizeof(char), 2048, fp2)) > 0)
    {
    	fwrite(buffer, sizeof(char), len, fp);
    	run_timers();
    }
    fclose(fp);
    fclose(fp2);
    
    return FALSE;
}

/* ========================================================================
   Function    - SaveGameDo
   Description - With the name we just got, create the savegame file.
   Returns     -
   ======================================================================== */
void SaveGameDo(LONG MenuCombo, LONG forsure)
{
    char buffer[80];
    char buf1[FILENAME_LEN + 2];
	
	char cur_game[25] = "savegame\\";
	strcat(cur_game, STRMGR_GetStr(STR_SAVEGAME));

	del_region(ScrollGame, 0);
	del_region(SelectGame, 0);
	
    iChatKey = 0;
    
    mkdir("SaveGame");

    strcpy(buf1, LoadSaveName);
    filename_chop(buf1);
    sprintf(buffer, "SaveGame\\%s.SAV", buf1);
    if (!stricmp(buffer, cur_game))				// can't save as "current"
    	return;

	if (!forsure)
		PushLoadSaveButton(MenuCombo);

	// does exist.
	if (0 == access(buffer, F_OK))
	{
		if (!forsure)
		{
			QueryOverwrite();
			RunMenus();
		}
		else  // delete failed -- must be read only
		{
			ShowDeleteFailed();
		}
		return;
    }
    else if (num_games >= MAX_SAVED_GAMES)
    {
    	ShowMaxReached();
    	RunMenus();
    	return;
    }


    if (!CopyAFile(cur_game, buffer))	// save succeeded
    {
    	back_to_main = FALSE;
    }
    HideSaveMenu(MenuCombo, 0);
    return;
}


/* ========================================================================
   Function    - LoadGameDoMulti
   Description - Load transfered file
   Returns     -
   ======================================================================== */
void LoadGameDoMulti( void )
{

#ifdef _WINDOWS

	FILE *fp;

	fMultiLoad = TRUE;
	REGRELIC_STRUCT::SetAssigned();  // shouldn't give more relics


	fp = fopen( szLoadTemp, "rb");

	if (fp == NULL)
	{
		char buf[200];

		AMultiPlayer.Finalize();		

		sprintf(buf, "Couldn't open load file %s (len %d).\n", szLoadTemp,
				strlen(szLoadTemp));
		init_gfont(FONT_SANS_8PT);
		gprint_text(10, 10, buf, 31);
		return;

	}

	LoadGameFiles(fp);
	fclose(fp);

	HidePanel(D_BUILD_ARMY);
	HidePanel(D_BUILD_ADV);

	LinkUnits(FALSE); // Don't send to others 
	
	
	SHORT i;
	for (i=0; i < LAND_REALM_COUNT; i++)
	{
		SHORT RegentIdx = realm[i].mfGetRegent();
		if (regents[RegentIdx].mfGetunit() == fERROR)
			realm[i].mfSetPlayable(FALSE);
	}
	
	ClearOldInvestitureList();
	
	char_selected = FALSE;
	for (i=0; i<LAND_REALM_COUNT; i++)
   	action_turn[i] = 1;
	global_action_turn = 1;
	oldAction = 0;
	fRoundOver = FALSE;
	//    mode_save = MAINT_MODE;
	fFinalRoll = FALSE;
	fUpdateDataArea = FALSE;
	fDoActionIcon = FALSE;
	fHeldAction = FALSE;
	fLTAction = BEFORE_LTACTION;
	CurrentRealm = REALM::REALM_OUT_OF_RANGE;
	new_treasury = 0;
	fPractice = FALSE;

	dturn_mode = START_NEW_OR_LOAD;

	if (fDomainTurn)
	{
		InitDomainTurn(0);
		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
			char temp[200];
			sprintf (temp, "LoadSaveDoMulti: Leaving domain turn");
			RandomLogComment (temp);
		}
	}

	dturn_mode = START_NEW_OR_LOAD;
	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		char temp[200];
		sprintf (temp, "LoadSaveDoMulti: Entering domain turn");
		RandomLogComment (temp);
	}
	InitDomainTurn(0);  /* restore the rest of the stuff */



#endif


}



/* ========================================================================
   Function    - LoadGameDo
   Description - Now we have a name... load it!
   Returns     -
   ======================================================================== */
void LoadGameDo(LONG MenuCombo, LONG )
{
	FILE *fp;
	char buffer[127];
	char buf1[FILENAME_LEN + 2];
	LONG	i;

	del_region(ScrollGame, 0);
	del_region(SelectGame, 0);
	if (highlighted_choice == -1)
		return;

	LONG MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	LONG tmpcombo = BUILD_LONG(MenuId, 2);
	PushLoadSaveButton(tmpcombo);

	strcpy(buf1, LoadSaveName);
	filename_chop(buf1);
    sprintf(buffer, "SaveGame\\%s.SAV", buf1);


	//---- Do some multiplayer things instead

#ifdef _WINDOWS

	if ( fMultiLoad )
	{
		back_to_main = FALSE;
		del_region(ScrollGame, 0);
		del_region(SelectGame, 0);
		
		LONG	MenuId, ButtonId;
		SPLIT_LONG(MenuCombo, MenuId, ButtonId);
		
		// click the button
		PushLoadSaveButton(MenuCombo);
		
		HideMenu(MenuId);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();

 		//@@@@@ More code for failure

		remove( szLoadTemp );   			// remove the old temp file
		CopyAFile ( buffer, szLoadTemp );	// copy to our transfer name
		AMultiPlayer.SendGameType();
		AMultiPlayer.InitPost2();
	}
	else
#endif
	{
		fp = fopen(buffer, "rb");
		if (fp == NULL)
   		{
	        char buf[200];
			sprintf(buf, "Couldn't open load file %s (len %d).\n", buffer,
			strlen(buffer));
			init_gfont(FONT_SANS_8PT);
			gprint_text(10, 10, buf, 31);
			HideLoadMenu(MenuCombo,0);
			return;
		}
		
		LoadGameFiles(fp);
	 	fclose(fp);

		//---- If you change these could you check how it might effect multiplayer
		HidePanel(D_BUILD_ARMY);
		HidePanel(D_BUILD_ADV);

		LinkUnits(FALSE); // Don't send to others 
		char_selected = TRUE;
		for (i=0; i<LAND_REALM_COUNT; i++)
			action_turn[i] = 1;
		oldAction = 0;
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
		fUpdateDataArea = FALSE;
		iSelectedUnit = -1;
		iProvInfo = 0;
		iUnitInfo = 0;
		iPlaceInfo = 0;
		
		SetCurAction(0);
		CurrentRealm = REALM::REALM_OUT_OF_RANGE;
		new_treasury = 0;
		fPractice = FALSE;
		fControlMode |= 0x100;
		fRealmSelected = TRUE;

		back_to_main = FALSE;
		HideLoadMenu(MenuCombo,0);
		
		ResetToNonTutorialAdventures();
		master_game_type = GAME_NORMAL;
		
		// clear any old menu data
		ResetMenus();
		ResetPanels();
		ResetRequests();
		
		del_all_regions();
		AddGameKeys();

		fPractice = FALSE;
		fTutorialSelected = FALSE;
		fTutorialFirstBattle = FALSE;
		
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
	}
	ReconstructMap();
	SetRedrawMainMapLevel();
	fFadedOut = -1;
	
	if (ScenarioInfo.IsScenario)		// [abc] 10/6/97
		NewGameConfirmed2(FALSE);
	
	return;
}



void DeleteGameReal(LONG , LONG button)
{	
    char buffer[127];
    char buf1[FILENAME_LEN + 2];
    iChatKey = 0;
	
	in_question = FALSE;
	
	if(button != QUESTION_BTN1)  // other than "Yes, delete"
		return;
	
	if (highlighted_choice == -1)
	    return;

    strcpy(buf1, LoadSaveName);
    filename_chop(buf1);
    sprintf(buffer, "SaveGame\\%s.SAV", buf1);

    remove(buffer);
    highlighted_choice = -1;
    num_games = GetSaveGames(saved_games);
    ScrollGame(0,0);
}


void DeleteGameConfirmed(LONG MenuCombo, LONG button)
{
	LONG	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	iChatKey = 0;
	PushLoadSaveButton(MenuCombo);
	
	HideMenu(MenuId);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	
	SetButtonLabel  (MenuId, QUESTION_TEXT, -1, BLACK );
	
	DeleteGameReal(0, button);
}



void DeleteGame (LONG MenuCombo, LONG)
{
	if (highlighted_choice == -1)
		return;
		
	in_question = TRUE;
	del_region(ScrollGame, 0);
	del_region(SelectGame, 0);
	
	LONG MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	LONG tmpcombo;
    if (LoadSave == LS_SAVE)
		tmpcombo = BUILD_LONG(MenuId, 3);
	else	
		tmpcombo = BUILD_LONG(MenuId, 3);
	
	PushLoadSaveButton(tmpcombo);
	
	// set the question text
	SetButtonLabel  (D_QUESTION2, QUESTION_TEXT, STR_LOADSAVE_DELETEQ, BLACK );
	SetButtonProc  (D_QUESTION2, 0, NULL, 0, NO_KEY );
	
	// turn on the first button as YES
	SetButtonLabel  (D_QUESTION2, QUESTION_BTN1, STR_YES, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION2, QUESTION_BTN1, DeleteGameConfirmed, QUESTION_BTN1, D_KEY_YES );
	
	// turn on the last button as CANCEL
	SetButtonLabel  (D_QUESTION2, QUESTION_BTN2, STR_CANCEL, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION2, QUESTION_BTN2, DeleteGameConfirmed, QUESTION_BTN2, D_KEY_CANCEL );
	
	ShowMenu(D_QUESTION2);
	return;
}

/* ========================================================================
   Function    - GetSaveGames
   Description - Go to the right directory on the disk, and load filenames
   Returns     - Number of saved games
   ======================================================================== */
LONG GetSaveGames(char names[MAX_SAVED_GAMES+1][FILENAME_LEN+1])
{
	// TODO: (fire lizard) rewrite completely
	char *path = "SAVEGAME\\*.SAV";
    unsigned attribs = _A_NORMAL;
    //struct find_t findbuf, *bufptr = &findbuf;
    LONG status, num_files, i;
    char filename[127];
    FILE *fp;
	char dummy_[80];

    for (i=0; i <= MAX_SAVED_GAMES; i++)
        memset(names[i], '\0', FILENAME_LEN);

    /*for (status = _dos_findfirst(path, attribs, bufptr), num_files = 0;
          status == 0 && 
          ((LoadSave==LS_SAVE && num_files < MAX_SAVED_GAMES) ||
           (LoadSave==LS_LOAD && num_files <= MAX_SAVED_GAMES));
          status = _dos_findnext(bufptr))
    {
        strncpy(names[num_files], bufptr->name, FILENAME_LEN);

        for (i=0; i < strlen(names[num_files]); i++)
            if (names[num_files][i] == '.')
                names[num_files][i] = '\0';

			// find out if file is SCENARIO type
			sprintf(filename, "SAVEGAME\\%s.SAV", names[num_files]);
			fp = fopen(filename, "rb");
			if (fp != NULL)
			{
				fread(&dummy_[0], sizeof(UBYTE), 73, fp);
				fread(&ScenInfo[num_files], sizeof(LONG),	1, fp);
				fclose(fp);
			}

        // don't show "current" when saving
        if ((LoadSave == LS_SAVE) && !stricmp(names[num_files], "current"))
        	continue;
        	
        num_files++;
    }

    _dos_findclose(bufptr);*/
    return num_files;
}

/* ========================================================================
   Function    - PaintLoadSaveMenu
   Description - Each time the menu runs, we repaint
   Returns     -
   ======================================================================== */

void PaintLoadSaveMenu (LONG MenuCombo, LONG)
{
	LONG	i, num_buttons;
	LONG	MenuId, ButtonId;
	char	buffer[80];
	LONG    menu_x, menu_y;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	if( GetButtonPosition( MenuId, 0, &menu_x, &menu_y ) == fERROR)
	    return;

	del_region(ScrollGame, 0);
	del_region(SelectGame, 0);
	
	// paint the background
	init_gfont(FONT_TITL_40PT);
	if (LoadSave == LS_SAVE)
	{
	    sprintf(buffer, "^N%s", STRMGR_GetStr(STR_SAVE_TITLE));
	    num_buttons = SaveMenuSize - 1;  /* don't count button 0 */
	}
	else
	{
	    sprintf(buffer, "^N%s", STRMGR_GetStr(STR_LOAD_TITLE));
	    num_buttons = LoadMenuSize - 1;  /* don't count button 0 */
	}
	#if defined(_FOREIGNVER)
	print_textf(menu_x + 14, menu_y + 23, 0, buffer);
	#else
	print_textf(menu_x + 25, menu_y + 23, 0, buffer);
	#endif
	
	// set up the buttons
	init_gfont(FONT_SANS_8PT);
	for(i=1; i <= num_buttons; i++)
	{
	    switch(i)
	    {
	        case 2:  /* done */
	            if ((LoadSave != LS_SAVE && highlighted_choice == -1) ||
	                (strlen(LoadSaveName) < 2))
	            	SetButtonLabelColor(MenuId, i, 16);
			    else
	            	SetButtonLabelColor(MenuId, i, BTN_LABEL_COLOR);
	            break;
	
	        case 3:  /* delete */
	            if (highlighted_choice == -1)
	            	SetButtonLabelColor(MenuId, i, 16);
			    else
	            	SetButtonLabelColor(MenuId, i, BTN_LABEL_COLOR);
	            break;
	
	        default:
	            buffer[0] = '\0';
	            break;
	    }
	
	}
	if (LoadSave == LS_SAVE)
	{
	    gprint_text(menu_x + 48, menu_y + 81, STRMGR_GetStr(STR_LOADSAVE_FNAME), 31);
        gprint_text(menu_x + 110, menu_y + 81, LoadSaveName, 31);
        // grab a file name
        if (PaintInputField(MenuId, LoadSaveName, FILENAME_LEN+2, -1, FALSE, 3,0,0))
        {
		    // ignore return, but put space back at end of LoadSaveName
		    char *p;
		    if (strlen(LoadSaveName) > 0)
		        p = LoadSaveName + strlen(LoadSaveName) - 1;
		    else
		        p = LoadSaveName;
		
		    if (*p != ' ' && *p != '_')
		        strcat(LoadSaveName, " ");
        }

	    // check to see if the highlighted entry is still valid
	    if (highlighted_choice >= 0)  // we have a selection
	    {
	        char buf1[FILENAME_LEN+2], buf2[FILENAME_LEN+2];
	
	        strcpy(buf1, saved_games[highlighted_choice]);
	        filename_chop(buf1);
	        strcpy(buf2, LoadSaveName);
	        filename_chop(buf2);
	        if (strcmp(buf1, buf2))
	            highlighted_choice = -1;
	    }
	}
    else
        gprint_text(menu_x + 48, menu_y + 81, STRMGR_GetStr(STR_LOADSAVE_SELECTG), 31);

    // paint listbox
    for (i=0; i < 6; i++)
    {
        if (i + first_displayed >= num_games)
            break;

#ifdef _FOREIGNVER
	#ifdef _FRENCHVER
			sprintf(buffer, "%s %s", saved_games[i+first_displayed], (ScenInfo[i+first_displayed].IsScenario)?"Scénario":" ");
	#endif
	#ifdef _GERMANVER
			sprintf(buffer, "%s %s", saved_games[i+first_displayed], (ScenInfo[i+first_displayed].IsScenario)?"Szenario":" ");
	#endif
#else
			sprintf(buffer, "%s %s", saved_games[i+first_displayed], (ScenInfo[i+first_displayed].IsScenario)?"SCENARIO":" ");
#endif
        if ((i+first_displayed) == highlighted_choice)
            gprint_text(menu_x + 48, menu_y + 113 + i*20, buffer, 31);
        else
            gprint_text(menu_x + 48, menu_y + 113 + i*20, buffer, (ScenInfo[i+first_displayed].IsScenario)?127:173);

        if (!in_question)
        	add_region(menu_x+48, menu_y+113+i*20, 168, 20, NO_KEY,
        	    SelectGame, i+first_displayed, LoadSave, 0, -1);
    }
    if (!in_question)
    {
    	add_region(menu_x+200, menu_y+108, 24, 60, NO_KEY, ScrollGame, -1, 0, 0, -1);
    	add_region(menu_x+200, menu_y+168, 24, 60, NO_KEY, ScrollGame,  1, 0, 0, -1);
    }
}


BOOL GetHelpBit(SHORT bitno)
{
	unsigned long x;
	
	if (bitno < 0 || bitno > 31)
		return TRUE;
		
	x = (1 << bitno) & helpbits;
	return (x != 0);
}

void SetHelpBit(SHORT bitno)
{
	if (bitno < 0 || bitno > 31)
		return;
	
	helpbits |= (1 << bitno);
}

void SetAllHelpBits(void)
{
	helpbits = 0xFFFFFFFF;
}

