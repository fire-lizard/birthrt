/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: combatui.cpp  - Control for the combat mode
   Author:   Gary Powell

   ========================================================================

   Contains the following general functions:


   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#ifdef _WINDOWS
#include <Windows.h>
#endif

#include "MACHINE.H"

#include "AVATAR.HXX"
#include "COMBATUI.HXX"
#include "COMBCAMR.HXX"
#include "COMBCNTL.HXX"
#include "COMBDATA.HXX"
#include "COMBOPTS.HXX"
#include "COMBTARG.HXX"
#include "CONTEXT.HXX"
#include "GAME.H"
#include "GAMEMAP.HXX"
#include "GAMEKEY.HXX"
#include "GMENUENM.H"
#include "INVNGUI.HXX"
#include "HANDLE.HXX"
#include "MAIN.HXX"
#include "MARGIN.HXX"
#include "PANEL.H"
#include "REALM.HXX"
#include "SCENE.HXX"
#include "SCNMGR.HXX"
#include "STRMGR.H"
#include "strenum.h"
#include "ADVPREP.HXX"
#include "REGENTS.HXX"

#ifdef _WINDOWS
#include "WINSYS\MULPLAY.HXX"
#include "WINSYS\MONO_C.H"
#endif


extern LONG CountRegions (void);

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */


#define FRAMES_TO_WAIT	10
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Classes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

BOOL RegentKilled = FALSE;
extern LONG new_treasury;
extern LONG adv_dif;

/* ------------------------------------------------------------------------
   Local Variables
   ------------------------------------------------------------------------ */

static BOOL gCombatMenuIsUp = FALSE;
static BOOL gCombatMenuRequested = TRUE;

static LONG gCountDown = 0;
static BOOL gCountDownMode = FALSE;

static BOOL gRPGTurnGo = TRUE;

static SHORT gblankBarIndex = fERROR;
static LONG GoldCoins = 0;
static LONG MundaneValue = 0;


/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
extern void IncActionTurn(void);
extern BOOL ExpCheck(PLAYER_STATS * pPS);
extern void GainExp(PLAYER_STATS * pPS, const LONG ExpAmount,
					const BOOL CanGainLevelNow);
extern LONG GetCastleName( SHORT PlacesIndex );
void static TestAndPutLiveOneInFront();


static void ShowCombatUIMenu(SCENE & /* rScene */);
static void HideCombatUIMenu(SCENE & /* rScene */);

static void AdventureTripDamageTimer(SCENE & /* rScene */ );
static void PaintAdventureWin (LONG /* MenuCombo */, LONG );
static void PaintAdventureLose (LONG /* MenuCombo */, LONG );
static void AdventureLooseProc(LONG /* MenuCombo */, LONG /* button */);
static void AdventureWinProc(LONG /* MenuCombo */, LONG /* button */);

void 			AdvExit (LONG, LONG button);
void 			QueryAdvExit (LONG, LONG);

//WRC [24.3.97] why should this be static? this is a useful function!
//static void ForceHires ();
void ForceHires ();

extern void SetRelicFound( SHORT PlacesIndex );

static void CombatEndHelpProc(LONG , LONG );

BOOL GetCombatMenuUp()
{
    return gCombatMenuRequested;
}
void SetCombatMenuUp(BOOL gCMR)
{
    gCombatMenuRequested = gCMR;
}

/* ========================================================================
   Function    - ForceHires
   Description - Put game into high-res and do a render in high-res
   				  so menus look correct.
   Returns     - nothing
	Written by  - JPC, 11-12-96
   ======================================================================== */
void ForceHires ()
{	
	set_hires (0, 0);
	if (fRender)
	{
		render_view (FALSE);// update the background screen
								  // in high res (otherwise we get
								  // a 1/4 size screen in the upper
								  // left of the video screen when
								  // the menu comes up
	}
}

/* ========================================================================
   Function    - AdventureAllocateData
   Description - Allocate some scene specific data space for the combat and
   				 adventure AI.
   Returns     -
   ======================================================================== */
BOOL const AdventureAllocateData(SCENE & rScene)
{	
	BOOL Result = FALSE;
	rScene.fhData = NewBlock(sizeof(COMBAT_SCENE_DATA));
	if (rScene.fhData != fERROR)
	{
		DumbAutoLockPtr<COMBAT_SCENE_DATA> const pcsdData(rScene.fhData);
		pcsdData->mfInitSceneVals();
		Result = TRUE;
	
	}
	
	if (COMBAT_MODE::mfIsModeRPG())
	{
		gRPGTurnGo = FALSE;
	}
	else
	{
		gRPGTurnGo = TRUE;
	}
	
	GoldCoins = 0;
	MundaneValue = 0;
	
	return Result;
}

/* ========================================================================
   Function    - AdventureTripDamageTimer
   Description - Allow Damage to Occur.
   Returns     -
   ======================================================================== */
static void AdventureTripDamageTimer(SCENE & rScene)
{
	if (rScene.fhData != fERROR)
	{
		DumbAutoLockPtr<COMBAT_SCENE_DATA> const pcsdData(rScene.fhData);
		if (COMBAT_MODE::mfIsModeRPG() == TRUE)
		{
			SCENE_AI::STATE OldSceneState;
			SCENE_AI::SCENE_TYPE OldSceneType;
			rScene.mfGetSceneAIState(&OldSceneState,
							         &OldSceneType);
			
			if (OldSceneType == SCENE_AI::COMBAT_SCENE)
			{
				// Only the Done key can let damage occur.
				return;
			}
		}
	
		if (pcsdData->mfTestDamageTimedOut() == TRUE)
		{
			LONG i;
			pcsdData->mfResetDamageTimer();
			for (i = 0; i < MAX_AVATARS; i++)
			{
				if (rScene.Avatars[i] != fERROR)
				{
					CAvatar * const pAvatar = (CAvatar * const ) BLKPTR(rScene.Avatars[i]);
					if (pAvatar->Status == CAvatar::AI_ATTACK ||
					    pAvatar->Status == CAvatar::AI_CASTSPELL)
					{
						pAvatar->mfSetDamageFlag();
					}
				}
			}
		}
	}
}

/* ========================================================================
   Function    - AdventureDeallocateData
   Description - Release the scene specific AI data.
   Returns     -
   ======================================================================== */
void AdventureDeallocateData(SCENE & rScene)
{	
	if (rScene.fhData != fERROR)
	{
		DumbAutoLockPtr<COMBAT_SCENE_DATA> const pcsdData(rScene.fhData);
		
		HideCombatUIMenu(rScene);
		
		pcsdData->mfDeAllocateData();
		
		DisposBlock(rScene.fhData);
		rScene.fhData = fERROR;
	}
}

/* ========================================================================
   Function    - CountUpTheGoldCoins
   Description - If anyone is alive, count up the gold coins.
   Returns     -
   ======================================================================== */
static ULONG CountUpTheGoldCoins( LONG * MundaneSoldFor)
{
	ADVENTURER_TEAM_ITOR AdvItor;
	SHORT numAlive = 0;
	ULONG Result = 0;
	
	*MundaneSoldFor = 0;
	
	for (AdvItor = ADVENTURER::begin();
	     AdvItor != ADVENTURER::end();
	     AdvItor++)
	{
		CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(*AdvItor);
		if (!pAvatar->mfAmIImmoblized())
		{
			numAlive++;
		}
		if (pAvatar->hPlayerStats != fERROR)
		{
			PLAYER_STATS * const pPlayerStats = (PLAYER_STATS * const) BLKPTR(pAvatar->hPlayerStats);
			Result += pPlayerStats->mfGetGold();
			pPlayerStats->mfRemoveAllGold();
			*MundaneSoldFor += pPlayerStats->Inventory.mfSellMundane();
		}
	}
	
	if (numAlive == 0)
	{
		Result = 0;
		*MundaneSoldFor = 0;
	}
	else
	{
		Result += *MundaneSoldFor;
	}
	
	return Result;
}


/* ========================================================================
   Function    - AdventureEndMulti.
   Description - send overrides event starter 
   Returns     -
   ======================================================================== */
void AdventureEndMulti( int send )
{

#ifdef _WINDOWS

	if ( IsMultiPlayer() )
	{

		// someone found the relic of power OVERRIDE

		if ( send )
		{
			AMultiPlayer.BroadcastAdventOver();
		}
		else
		{
			//---- Person who started adventure then everyone is out ?

			if ( EventStarter )
			{
				AMultiPlayer.BroadcastAdventOver();
			}
		}
	}

#endif

}

/* ========================================================================
   Function    - AdventureEndDialog.
   Description - look over the inventory of the adventure team and determin
                 victory or defeat.
   Returns     -
   ======================================================================== */
void AdventureEndDialog( int send )
{
	THINGTYPE const QuestItem = (THINGTYPE) GetQuestThing();
	ADVENTURER_TEAM_ITOR AdvItor;
	SHORT numAlive = 0;
	BOOL foundQuestItem = ADVENTURER::mfDoesAdventureTeamHave(QuestItem);

#if defined (_WINDOWS)
// [d11-12-96 JPC] Prevent menu from being expanded on screen.
	if (!fHighRes)
		ForceHires ();
#endif

	RegentKilled = FALSE;
	
	for (AdvItor = ADVENTURER::begin();
	     AdvItor != ADVENTURER::end();
	     AdvItor++)
	{
		CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(*AdvItor);
		if (pAvatar->mfAmIImmoblized())
		{
			SHORT regentNum;
			
		    if (pAvatar->hPlayerStats == fERROR)
		    {
#if defined(_DEBUG)
		        fatal_error("AdvEnd: someone cleared playerstats");
#endif
				continue;
		    }
		    for (regentNum=0; regentNum < REGENT_COUNT; regentNum++)
		        if (regents[regentNum].mfGetunit() > 0 &&
		        	playerstats[regentNum] == pAvatar->hPlayerStats)
		            break;
		
		    if (regentNum == realm[HomeRealm].mfGetRegent())
		        RegentKilled = TRUE;
		}
		else
		{
			numAlive++;
		}
	}	
	
	if (numAlive > 0)
	{
	    LONG amount = CountUpTheGoldCoins(&MundaneValue);
	    
	    GoldCoins = amount;
	    
		for (AdvItor = ADVENTURER::begin();
		     AdvItor != ADVENTURER::end();
		     AdvItor++)
		{
			CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(*AdvItor);
			if (pAvatar->mfAmIImmoblized())
				continue;
				
			SHORT hPlayerStats = pAvatar->hPlayerStats;
			if (hPlayerStats == fERROR)
				continue;
					
			DumbAutoLockPtr<PLAYER_STATS> pPS(hPlayerStats);
				
			// double xp for rogues
			if (pPS->mfIsType(PLAYER_CLASS_INFO::BARD) ||
				pPS->mfIsType(PLAYER_CLASS_INFO::THIEF) )
			{	
				GainExp(pPS, amount * 2 / numAlive, TRUE);
			}
			else	
			{
				GainExp(pPS, amount / numAlive, TRUE);
			}
		}
	    
	    realm[HomeRealm].mfAddTreasury(amount/200);
	    new_treasury += amount/200;
	}

	
	if ((numAlive > 0) && !RegentKilled && foundQuestItem)
	{
		//---- Found the relic of power.
		AdventureEndMulti(send);
		
		LONG amount = 10000 * adv_dif;
		
		for (AdvItor = ADVENTURER::begin();
		     AdvItor != ADVENTURER::end();
		     AdvItor++)
		{
			CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(*AdvItor);
			if (pAvatar->mfAmIImmoblized())
				continue;
		
			SHORT hPlayerStats = pAvatar->hPlayerStats;
			if (hPlayerStats == fERROR)
				continue;
					
			DumbAutoLockPtr<PLAYER_STATS> pPS(hPlayerStats);
			GainExp(pPS, amount / numAlive, TRUE);
		}
		VictoryScreen(PaintAdventureWin, AdventureWinProc);
	}
 	else
	{
		AdventureEndMulti(send);
		DefeatScreen(PaintAdventureLose, AdventureLooseProc);
	}

#ifdef _WINDOWS

	//---- If we are in a multiplayer game and killed then exit mulitplayer

	if ( IsMultiPlayer() &&
		 RegentKilled 	    )
	{
		AMultiPlayer.Finalize(1);
	}


#endif


}


/* ========================================================================
   Function    - AdventureOverMulti
   Description - If multiplayer then do cleanup on adventure end
   Returns     -
   ======================================================================== */
void AdventureOverMulti(void)
{

#ifdef _WINDOWS

	if ( IsMultiPlayer() )
	{
		//---- If we started adventure then increment action

		if ( EventStarter )
		{
			IncActionTurn();

			if (fLTAction == DURING_LTACTION)	// return to normal action type
			{
				ActiveRegent = realm[HomeRealm].mfGetRegent();
				fLTAction = AFTER_LTACTION;
			}
			
// Thanks
//			action_turn++;

		}

		AMultiPlayer.InitEvent(0);

	}

#endif

}



/* ========================================================================
   Function    - AdventureLooseProc
   Description - When the lose menu Done button is hit, this is called.
   Returns     -
   ======================================================================== */
static void AdventureLooseProc(LONG MenuCombo, LONG )
{
#if defined (_WINDOWS)
// [d11-12-96 JPC] Prevent menu from being expanded on screen.
	if (!fHighRes)
		ForceHires ();
#endif

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
	
	AdvPrepReturn(0,0);
	//ReturnFromAdventure(0,0);
	ADVENTURER::mfDoToAllAdventurers(&CAvatar::mfIfAliveRestoreHealth);
	SCENE_MGR::mfReleaseSceneToMap(0,0);

	AdventureOverMulti();

}


/* ========================================================================
   Function    - AdventureWinProc
   Description - When the win menu Done button is hit, this is called.
   Returns     -
   ======================================================================== */
static void AdventureWinProc(LONG MenuCombo, LONG )
{
	LONG iPlaceName;
	LONG tmpReaction;
#if defined (_WINDOWS)
// [d11-12-96 JPC] Prevent menu from being expanded on screen.
	if (!fHighRes)
		ForceHires ();
#endif

	// note that the relic was found so that it will not be
	// placed in the scene next time
	SetRelicFound(SCENE_MGR::PlacesIndex);
	
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

	// handle the realm reaction
	iPlaceName = SCENE_MGR::PlacesIndex; // GetCastleName(SCENE_MGR::PlacesIndex);
	switch(iPlaceName)
	{				  
	case 5:			  //braeme
		tmpReaction = 	RealmReaction[REALM::MEDOERE][SCENE_MGR::Visitors] +15;
		SetGameData(MP_REALMREACTION, REALM::MEDOERE, SCENE_MGR::Visitors, (tmpReaction>150)?150:tmpReaction, TRUE);
		SetGameData(MP_REALMREACTION, SCENE_MGR::Visitors, REALM::MEDOERE, (tmpReaction>150)?150:tmpReaction, TRUE);
		break;

	case 16:		  // castle haes
		tmpReaction = RealmReaction[REALM::TUORNEN][SCENE_MGR::Visitors] + 25;
		SetGameData(MP_REALMREACTION, REALM::TUORNEN, SCENE_MGR::Visitors, (tmpReaction>150)?150:tmpReaction, TRUE);
		SetGameData(MP_REALMREACTION, SCENE_MGR::Visitors, REALM::TUORNEN, (tmpReaction>150)?150:tmpReaction, TRUE);
		break;
 
	case 56:		  //gaelin's keep
		tmpReaction = RealmReaction[REALM::IMPREGNABLE_HEART_OF_HAELYN][SCENE_MGR::Visitors] + 25; 
		SetGameData(MP_REALMREACTION, REALM::IMPREGNABLE_HEART_OF_HAELYN, SCENE_MGR::Visitors, (tmpReaction>150)?150:tmpReaction, TRUE);
 
		tmpReaction = RealmReaction[REALM::FIVEPEAKS][SCENE_MGR::Visitors] + 10; 
		SetGameData(MP_REALMREACTION, REALM::FIVEPEAKS, SCENE_MGR::Visitors, (tmpReaction>150)?150:tmpReaction, TRUE);
		SetGameData(MP_REALMREACTION, SCENE_MGR::Visitors, REALM::FIVEPEAKS, (tmpReaction>150)?150:tmpReaction, TRUE);
		break;

	case 11:		  //tower ruannoch
		tmpReaction = RealmReaction[REALM::AVANIL][SCENE_MGR::Visitors] + 15;
		SetGameData(MP_REALMREACTION, REALM::AVANIL, SCENE_MGR::Visitors, (tmpReaction>150)?150:tmpReaction, TRUE);
		SetGameData(MP_REALMREACTION, SCENE_MGR::Visitors, REALM::AVANIL, (tmpReaction>150)?150:tmpReaction, TRUE);
		
		tmpReaction = RealmReaction[REALM::TUORNEN][SCENE_MGR::Visitors] + 15;
		SetGameData(MP_REALMREACTION, REALM::TUORNEN, SCENE_MGR::Visitors, (tmpReaction>150)?150:tmpReaction, TRUE);
		SetGameData(MP_REALMREACTION, SCENE_MGR::Visitors, REALM::TUORNEN, (tmpReaction>150)?150:tmpReaction, TRUE);

		tmpReaction = RealmReaction[REALM::BOERUINE][SCENE_MGR::Visitors] + 15;
		SetGameData(MP_REALMREACTION, REALM::BOERUINE, SCENE_MGR::Visitors, (tmpReaction>150)?150:tmpReaction, TRUE);
		SetGameData(MP_REALMREACTION, SCENE_MGR::Visitors, REALM::BOERUINE, (tmpReaction>150)?150:tmpReaction, TRUE);
		break;

	case 29:	 	 // Ruorven
		tmpReaction = RealmReaction[REALM::COERANYS][SCENE_MGR::Visitors] + 20;
		SetGameData(MP_REALMREACTION, REALM::COERANYS, SCENE_MGR::Visitors, (tmpReaction>150)?150:tmpReaction, TRUE);
		SetGameData(MP_REALMREACTION, SCENE_MGR::Visitors, REALM::COERANYS, (tmpReaction>150)?150:tmpReaction, TRUE);
		break;

	case 63:		 //Spirit's End
		tmpReaction = RealmReaction[REALM::OSOERDE][SCENE_MGR::Visitors] + 35;
		SetGameData(MP_REALMREACTION, REALM::OSOERDE, SCENE_MGR::Visitors, (tmpReaction>150)?150:tmpReaction, TRUE);
		SetGameData(MP_REALMREACTION, SCENE_MGR::Visitors, REALM::OSOERDE, (tmpReaction>150)?150:tmpReaction, TRUE);
		break;

	case 61:		 //Elfwash Keep
		tmpReaction = RealmReaction[REALM::BOERUINE][SCENE_MGR::Visitors] + 10;
		SetGameData(MP_REALMREACTION, REALM::BOERUINE, SCENE_MGR::Visitors, (tmpReaction>150)?150:tmpReaction, TRUE);
		SetGameData(MP_REALMREACTION, SCENE_MGR::Visitors, REALM::BOERUINE, (tmpReaction>150)?150:tmpReaction, TRUE);
		break;

	case 12:		 // Stormpoint
		if(!ISVASSAL(REALM::TAEGHAS, SCENE_MGR::Visitors) && !ISVASSAL(SCENE_MGR::Visitors, REALM::TAEGHAS)) 
		{
			tmpReaction = RealmReaction[REALM::TAEGHAS][SCENE_MGR::Visitors] - 35;
			SetGameData(MP_REALMREACTION, REALM::TAEGHAS, SCENE_MGR::Visitors, (tmpReaction<0)?0:tmpReaction, TRUE);
			SetGameData(MP_REALMREACTION, SCENE_MGR::Visitors, REALM::TAEGHAS, (tmpReaction<0)?0:tmpReaction, TRUE);
		}
		if(!ISVASSAL(REALM::AVANIL, SCENE_MGR::Visitors) && !ISVASSAL(SCENE_MGR::Visitors, REALM::AVANIL)) 
		{
			tmpReaction = RealmReaction[REALM::AVANIL][SCENE_MGR::Visitors] - 35;
			SetGameData(MP_REALMREACTION, REALM::AVANIL, SCENE_MGR::Visitors, (tmpReaction<0)?0:tmpReaction, TRUE);
			SetGameData(MP_REALMREACTION, SCENE_MGR::Visitors, REALM::AVANIL, (tmpReaction<0)?0:tmpReaction, TRUE);
	   	}
		break;
		
	case 7:			 // Spiderfell
		tmpReaction = RealmReaction[REALM::GHOERE][SCENE_MGR::Visitors] + 10;
		SetGameData(MP_REALMREACTION, REALM::GHOERE, SCENE_MGR::Visitors, (tmpReaction>150)?150:tmpReaction, TRUE);
		SetGameData(MP_REALMREACTION, SCENE_MGR::Visitors, REALM::GHOERE, (tmpReaction>150)?150:tmpReaction, TRUE);
		
		tmpReaction = RealmReaction[REALM::ENDIER][SCENE_MGR::Visitors] + 10;
		SetGameData(MP_REALMREACTION, REALM::ENDIER, SCENE_MGR::Visitors, (tmpReaction>150)?150:tmpReaction, TRUE);
		SetGameData(MP_REALMREACTION, SCENE_MGR::Visitors, REALM::ENDIER, (tmpReaction>150)?150:tmpReaction, TRUE);

		tmpReaction = RealmReaction[REALM::DIEMED][SCENE_MGR::Visitors] + 10;
		SetGameData(MP_REALMREACTION, REALM::DIEMED, SCENE_MGR::Visitors, (tmpReaction>150)?150:tmpReaction, TRUE);
		SetGameData(MP_REALMREACTION, SCENE_MGR::Visitors, REALM::DIEMED, (tmpReaction>150)?150:tmpReaction, TRUE);

		tmpReaction = RealmReaction[REALM::MEDOERE][SCENE_MGR::Visitors] + 10;
		SetGameData(MP_REALMREACTION, REALM::MEDOERE, SCENE_MGR::Visitors, (tmpReaction>150)?150:tmpReaction, TRUE);
		SetGameData(MP_REALMREACTION, SCENE_MGR::Visitors, REALM::MEDOERE, (tmpReaction>150)?150:tmpReaction, TRUE);

		tmpReaction = RealmReaction[REALM::ROESONE][SCENE_MGR::Visitors] + 10;
		SetGameData(MP_REALMREACTION, REALM::ROESONE, SCENE_MGR::Visitors, (tmpReaction>150)?150:tmpReaction, TRUE);
		SetGameData(MP_REALMREACTION, SCENE_MGR::Visitors, REALM::ROESONE, (tmpReaction>150)?150:tmpReaction, TRUE);

		break;

	default:
		break;
	}

    AdvPrepReturn(0,0);
    //ReturnFromAdventure(0,0);
	ADVENTURER::mfDoToAllAdventurers(&CAvatar::mfIfAliveRestoreHealth);
	SCENE_MGR::mfReleaseSceneToMap(0,0);

	AdventureOverMulti();

}



/* ========================================================================
   Function    - PaintAdventureLose
   Description - paint the text on the help menu
   Returns     -
   ======================================================================== */
static void PaintAdventureLose (LONG MenuCombo, LONG )
{	
	LONG	X,Y;
	LONG	mx,my;
	LONG	MenuId, ButtonId;
	CHAR	color = 1;
	CHAR	cpBuffer[256];
	CHAR	cpFormatBuffer[256];
	CHAR	cpBuffer2[128];
	CHAR	cpRegentKilled[500];
	CHAR	cpSurvivedBuffer[500];
	CHAR	cpDiedBuffer[500];
	ADVENTURER_TEAM_ITOR AdvItor;
	SHORT	numAlive = 0;
	SHORT	numDead = 0;
	BOOL	fDied = FALSE;
	BOOL	fSurvived = FALSE;
	
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	// get position of lose screen
	if( GetButtonPosition( MenuId, 0, &mx, &my ) == fERROR)
		return;
		
	X = mx + 60;
	Y = my + 20;
	
 	// paint dialog title
	strncpy(cpBuffer,STRMGR_GetStr(STR_DEFEAT), sizeof(cpBuffer));
	gprint_text(X,Y,cpBuffer, 31);
	
	X = mx + 27;
	Y = my + 100;
	
	
	// build some text that says why you lost
	
	
	init_gfont(FONT_SANS_12PT);
	strncpy(cpFormatBuffer, STRMGR_GetStr(COMBAT_STR_END_MSG_1), sizeof(cpFormatBuffer));
	strncpy(cpBuffer2, STRMGR_GetStr(COMBAT_STR_END_MSG_3), sizeof(cpBuffer2));
	sprintf(cpBuffer, cpFormatBuffer, cpBuffer2 );
	
	gprint_text(X,Y,cpBuffer ,color);
	
	Y += 20;
	
	// clear string buffers and add formatting
	strcpy(cpSurvivedBuffer, "^W300");
	strcpy(cpDiedBuffer, "^W300");
	
	for (AdvItor = ADVENTURER::begin();
	     AdvItor != ADVENTURER::end();
	     AdvItor++)
	{
		CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(*AdvItor);
		if (pAvatar->hPlayerStats != fERROR)
		{
			DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(pAvatar->hPlayerStats);
			
			if (pAvatar->mfAmIImmoblized())
			{
				fDied = TRUE;
				
				// add this name to the survived list
				strcat(cpDiedBuffer, pPlayerStats->mfGetName());
				strcat(cpDiedBuffer, ", ");
				
				// regents don't really die, they get resurrected
				if (RegentKilled && IsRegent(pAvatar->UnitIndex))
				{
					strcpy(cpFormatBuffer, STRMGR_GetStr(COMBAT_STR_END_MSG_22));
					sprintf(cpRegentKilled, cpFormatBuffer, pPlayerStats->mfGetName());
					numAlive++;
				}
				else
				{
					numDead++;
				}
			}
			else
			{
				fSurvived = TRUE;
				numAlive++;
				// add this name to the survived list
				strcat(cpSurvivedBuffer, pPlayerStats->mfGetName());
				strcat(cpSurvivedBuffer, ", ");
			}
		}
	}
	
		
	if ( fSurvived )
	{
		SHORT	sIndex = strlen(cpSurvivedBuffer);
		cpSurvivedBuffer[sIndex - 1] = 0;
		cpSurvivedBuffer[sIndex - 2] = ' ';
		if ( numAlive == 1 )
			strcat(cpSurvivedBuffer, STRMGR_GetStr(COMBAT_STR_END_MSG_20));
		else
			strcat(cpSurvivedBuffer, STRMGR_GetStr(COMBAT_STR_END_MSG_20_2));
		gprint_text(X,Y,cpSurvivedBuffer ,color);
		Y += 45;
	}
	
	if ( fDied )
	{
		SHORT	sIndex = strlen(cpDiedBuffer);
		cpDiedBuffer[sIndex - 1] = 0;
		cpDiedBuffer[sIndex - 2] = ' ';
		if ( numDead == 1 )
			strcat(cpDiedBuffer, STRMGR_GetStr(COMBAT_STR_END_MSG_21));
		else
			strcat(cpDiedBuffer, STRMGR_GetStr(COMBAT_STR_END_MSG_21_2));
		gprint_text(X,Y,cpDiedBuffer ,color);
		Y += 30;
	}
	
	// some visual space
	Y += 5;
	
	if ( RegentKilled )
	{
		gprint_text(X,Y,cpRegentKilled ,color);
		Y += 45;
	}
	
	if (numAlive > 0)
	{
		strncpy(cpBuffer, STRMGR_GetStr(COMBAT_STR_END_MSG_7), sizeof(cpFormatBuffer));
		gprint_text(X,Y,cpBuffer ,color);
		Y += 15;
		
		strncpy(cpBuffer, STRMGR_GetStr(COMBAT_STR_END_MSG_8), sizeof(cpFormatBuffer));
		gprint_text(X,Y,cpBuffer ,color);
		Y += 15;
		
	
	   if (GoldCoins > 0)
	   {
		   sprintf(cpBuffer, STRMGR_GetStr(COMBAT_STR_END_MSG_14), MundaneValue);
		   gprint_text(X,Y, cpBuffer, color);
		   Y += 15;
		   
		   strncpy(cpFormatBuffer, STRMGR_GetStr(COMBAT_STR_END_MSG_11), sizeof(cpFormatBuffer));
		   sprintf(cpBuffer, cpFormatBuffer, GoldCoins, ((double)(GoldCoins)/(double)(2000.0)));
		   gprint_text(X,Y,cpBuffer ,color);
		   Y += 15;
		
		   strncpy(cpBuffer, STRMGR_GetStr(COMBAT_STR_END_MSG_12), sizeof(cpFormatBuffer));
		   gprint_text(X,Y,cpBuffer ,color);
		   Y += 15;
	   }
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
	
	strcpy(cpBuffer,STRMGR_GetStr(STR_OK));
	print_text_centered(X,Y,cpBuffer,WHITE);
}
/* ========================================================================
   Function    - PaintAdventureWin
   Description - paint the text on the help menu
   Returns     -
   ======================================================================== */
static void PaintAdventureWin (LONG MenuCombo, LONG )
{	
	LONG	X,Y;
	LONG	mx,my;
	LONG	MenuId, ButtonId;
	CHAR	color = 1;
	CHAR	cpBuffer[256];
	CHAR	cpFormatBuffer[256];
	CHAR	cpBuffer2[128];
	ADVENTURER_TEAM_ITOR AdvItor;
	BOOL	numAlive = 0;
	
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	// get position of win screen
	if( GetButtonPosition( MenuId, 0, &mx, &my ) == fERROR)
		return;
		
	X = mx + 60;
	Y = my + 20;

	// paint dialog title
	strncpy(cpBuffer, STRMGR_GetStr(STR_VICTORY), sizeof(cpBuffer));
	gprint_text(X,Y,cpBuffer, 31);
	
	X = mx + 27;
	Y = my + 100;
	
	// build some text that says why you won
	
	init_gfont(FONT_SANS_12PT);
	strncpy(cpFormatBuffer, STRMGR_GetStr(COMBAT_STR_END_MSG_1), sizeof(cpFormatBuffer));
	strncpy(cpBuffer2, STRMGR_GetStr(COMBAT_STR_END_MSG_2), sizeof(cpBuffer2));
	sprintf(cpBuffer, cpFormatBuffer, cpBuffer2 );
	
	gprint_text(X,Y,cpBuffer ,color);
	

	Y += 50;
	for (AdvItor = ADVENTURER::begin();
	     AdvItor != ADVENTURER::end();
	     AdvItor++)
	{
		CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(*AdvItor);
		if (pAvatar->hPlayerStats != fERROR)
		{
			DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(pAvatar->hPlayerStats);
			
			if (pAvatar->mfAmIImmoblized())
			{
				strncpy(cpFormatBuffer, STRMGR_GetStr(COMBAT_STR_END_MSG_5), sizeof(cpFormatBuffer));
				sprintf(cpBuffer, cpFormatBuffer, pPlayerStats->mfGetName());
				gprint_text(X,Y,cpBuffer ,color);
				Y += 15;
				
				strncpy(cpBuffer, STRMGR_GetStr(COMBAT_STR_END_MSG_6), sizeof(cpFormatBuffer));
				gprint_text(X,Y,cpBuffer ,color);
				Y += 15;
			}
			else
			{
				numAlive++;
				
				strncpy(cpFormatBuffer, STRMGR_GetStr(COMBAT_STR_END_MSG_4), sizeof(cpFormatBuffer));
				sprintf(cpBuffer, cpFormatBuffer, pPlayerStats->mfGetName());
				gprint_text(X,Y,cpBuffer ,color);
				Y += 15;
			}
		}
	}
	
		
	if (numAlive > 0)
	{
		strncpy(cpBuffer, STRMGR_GetStr(COMBAT_STR_END_MSG_7), sizeof(cpFormatBuffer));
		gprint_text(X,Y,cpBuffer ,color);
		Y += 15;
		
		strncpy(cpBuffer, STRMGR_GetStr(COMBAT_STR_END_MSG_8), sizeof(cpFormatBuffer));
		gprint_text(X,Y,cpBuffer ,color);
		Y += 15;
		
	
	   if (GoldCoins > 0)
	   {
		   sprintf(cpBuffer, STRMGR_GetStr(COMBAT_STR_END_MSG_14), MundaneValue);
		   gprint_text(X,Y, cpBuffer, color);
		   Y += 15;
		   
		   strncpy(cpFormatBuffer, STRMGR_GetStr(COMBAT_STR_END_MSG_11), sizeof(cpFormatBuffer));
		   sprintf(cpBuffer, cpFormatBuffer, GoldCoins, ((double)(GoldCoins)/(double)(2000.0)));
		   gprint_text(X,Y,cpBuffer ,color);
		   Y += 15;
		
		   strncpy(cpBuffer, STRMGR_GetStr(COMBAT_STR_END_MSG_12), sizeof(cpFormatBuffer));
		   gprint_text(X,Y,cpBuffer ,color);
		   Y += 15;
	   }
	}
	
	//gprint_text(X,Y,cpBuffer ,color);
	Y += 15;
	
	// paint the control buttons
		
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
	
	strcpy(cpBuffer,STRMGR_GetStr(STR_OK));
	print_text_centered(X,Y,cpBuffer,WHITE);
}
/* ========================================================================
   Function    - PlayAdventureUI
   Description - Do the adventure specific UI stuff.
   Returns     -
   ======================================================================== */
void PlayAdventureUI(SCENE &rScene)
{
	if (COMBAT_TARGETTING::mfIsModeTargetting() == TRUE )
	{
		// if target selected, set it and flip the previous mode back on
		if(COMBAT_TARGETTING::mfGetTargetIndex() != (LONG)fERROR)
		{
			CAvatar *pTargetAvatar;
			BOOL TargetFound = FALSE;
			
			// get the avatar who cast the spell
			CAvatar * const pShootingAvatar = (CAvatar *) BLKPTR (COMBAT_TARGETTING::mfWhoseShooting());
			
			LONG i;
			// check to see if this thing index is an avatar
			for(i = 0; i < MAX_AVATARS; i++ )
			{
				// This is a sparsly filled array.
				if (rScene.Avatars[i] != fERROR)
				{
					pTargetAvatar = (CAvatar *) BLKPTR(rScene.Avatars[i]);
					if(pTargetAvatar->ThingIndex == COMBAT_TARGETTING::mfGetTargetIndex())
					{
						TargetFound = TRUE;
						break; // found him
					}
				}
			}
			
			// look at my target
			if (pShootingAvatar->ThingIndex != COMBAT_TARGETTING::mfGetTargetIndex())
			{
				pShootingAvatar->FaceTo( mythings[COMBAT_TARGETTING::mfGetTargetIndex()].x,
									   	   mythings[COMBAT_TARGETTING::mfGetTargetIndex()].y );
			}
			
			// if no target found or I'm my own target, fail to get an enemy.
			if (TargetFound == FALSE
				|| pShootingAvatar->ThingIndex == COMBAT_TARGETTING::mfGetTargetIndex()
				|| pTargetAvatar->GetAIFuncIndex() == CAvatar::AI_FUNC_FIREBALL
				)
			{
				pShootingAvatar->mfSethEnemy(fERROR);
			}
			else
			{
				// set the spell caster's hEnemy to be this object
				pShootingAvatar->mfSethEnemy(rScene.Avatars[i]);
			}
			
			
			pShootingAvatar->fDamageFlag = 1;
			
			// clear target
			COMBAT_TARGETTING::mfInitVals();
			
			// Shoot right away. (No delay)
			if (pShootingAvatar->hPlayerStats != fERROR)
			{
				PLAYER_STATS * const pPlayerStats = (PLAYER_STATS * const) BLKPTR(pShootingAvatar->hPlayerStats);
				pPlayerStats->Inventory.mfActivateSpellBox(pShootingAvatar->fFollowPlayer.fCurrentSpellBox);
			}
		}
	}
	
	if (ADVENTURER::mfIsAdventureTeam(&CAvatar::mfAmIBeingAttacked))
	{
		if (rScene.fhData != fERROR)
		{
			DumbAutoLockPtr<COMBAT_SCENE_DATA> const pcsdData(rScene.fhData);
			if ( TRUE == pcsdData->fRetreatData.mfIsRetreating())
			{	
				pcsdData->fRetreatData.mfUpdateRetreatTimer();
			}
			else
			{
				rScene.mfSetSceneType (SCENE_AI::COMBAT_SCENE);
				rScene.mfSetSceneState(SCENE_AI::INIT);
				rScene.mfCallSceneAI();
			}
		}
	}
	else
	{
		// Make sure there is a live avatar at the head of the Adventurers.
		TestAndPutLiveOneInFront();
	}
	
	AdventureTripDamageTimer(rScene);
	
	if (gCombatMenuIsUp == TRUE)
	{
		CONTROL_PANELS::mfCheckAllPanels();
		
		if (RECHECK_OPTIONS::mfHaveOptionsChanged())
		{
			RECHECK_OPTIONS::mfUpDateOptions();
		}
	}
}


/* ========================================================================
   Function    - AdventureShowStatus
   Description - Call the generic show status fn.
   Returns     -
   ======================================================================== */
void AdventureShowStatus(LONG , LONG)
{
	SHORT const hPlayerAvatar = CAvatar::mfGetPlayerAvatarHandle();
	
	if (hPlayerAvatar != fERROR)
	{
		CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(hPlayerAvatar);
		SetStatusRealm(pAvatar->Realm.HomeRealm);
#if defined (_WINDOWS)
// [d11-12-96 JPC] Prevent menu from being expanded on screen.
		if (!fHighRes)
			ForceHires ();
#endif

		ShowStatus(0, pAvatar->hPlayerStats);
	}
}

/* ========================================================================
   Function    - AdventureShowInventory
   Description - Call the generic show inventory fn.
   Returns     -
   ======================================================================== */
void AdventureShowInventory(LONG, LONG)
{
	SHORT const hPlayerAvatar = CAvatar::mfGetPlayerAvatarHandle();
	
	if (hPlayerAvatar != fERROR)
	{
		CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(hPlayerAvatar);
#if defined (_WINDOWS)
// [d11-12-96 JPC] Prevent menu from being expanded on screen.
		if (!fHighRes)
			ForceHires ();
#endif

		StaticInventoryGUI::mfDisplay(0, pAvatar->hPlayerStats);
	}
}

/* ========================================================================
   Function    - CombatShowStatusKey
   Description - Call the generic show status fn.
   Returns     -
   ======================================================================== */
void CombatShowStatusKey(LONG  menuid, LONG BottomMenuFlag)
{
	if (mouse_button == 2)
	{
		// Display help dialog on right click
		//CombatHelpDialog(0, COMBAT_STR_HELP_STATUS);
		mouse_click = 0;
		SystemHelp(COMBAT_STR_HELP_STATUS_TITLE, COMBAT_STR_HELP_STATUS, H_Character_Status, 0, 0);
	}
	else if ((mouse_click == 1 && mouse_button == 1) || mouse_click == 0)
	{
		mouse_click = 0;
		if (COMBAT_MODE::mfIsModeRPG() == TRUE && gRPGTurnGo == TRUE)
		{
			return;
		}
		
		
		// click the button
		SetButtonHilight(D_COMBAT_RPGBOTTOM_BAR, BUTNO_RPG_STATUS, TRUE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		SetButtonHilight(D_COMBAT_RPGBOTTOM_BAR, BUTNO_RPG_STATUS, FALSE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		
		if (!BottomMenuFlag)	// signifies this was called by the bottom menu.
		{
			CONTROL_PANELS::mfMatchMenuToCurrent(HIGH_LONG(menuid));
		}
		
		if (CONTROL_PANELS::mfCurrentInUse())
		{
			CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(CONTROL_PANELS::mfGetCurrAvatarHdl());
			SetStatusRealm(pAvatar->Realm.HomeRealm);
	#if defined (_WINDOWS)
	// [d11-12-96 JPC] Prevent menu from being expanded on screen.
			if (!fHighRes)
				ForceHires ();
	#endif
	
			ShowStatus(0, pAvatar->hPlayerStats);
		}
	}
}

/* ========================================================================
   Function    - CombatShowInventoryKey
   Description - Call the generic show inventory fn.
   Returns     -
   ======================================================================== */
void CombatShowInventoryKey(LONG , LONG )
{
	if (mouse_button == 2)
	{
		// Display help dialog on right click
		//CombatHelpDialog(0, COMBAT_STR_HELP_INVENTORY);
		mouse_click = 0;
		SystemHelp(COMBAT_STR_HELP_INVENTORY_TITLE, COMBAT_STR_HELP_INVENTORY, H_Character_Inventory, 0, 0);
	}
	else if ((mouse_click == 1 && mouse_button == 1) || mouse_click == 0)
	{
		
		mouse_click = 0;
		if (COMBAT_MODE::mfIsModeRPG() == TRUE && gRPGTurnGo == TRUE)
		{
			return;
		}
		
		// click the button
		SetButtonHilight(D_COMBAT_RPGBOTTOM_BAR, BUTNO_RPG_INVENTORY, TRUE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		SetButtonHilight(D_COMBAT_RPGBOTTOM_BAR, BUTNO_RPG_INVENTORY, FALSE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		
		if (CONTROL_PANELS::mfCurrentInUse())
		{
			CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(CONTROL_PANELS::mfGetCurrAvatarHdl());
#if defined (_WINDOWS)
	// [d11-12-96 JPC] Prevent menu from being expanded on screen.
			if (!fHighRes)
				ForceHires ();
#endif
	
			StaticInventoryGUI::mfDisplay(0, pAvatar->hPlayerStats);
		}
	}
}

/* ========================================================================
   Function    - InitAdventureUI
   Description - Set up the window size based on the user choices.
   Returns     -
   ======================================================================== */
void InitAdventureUI(SCENE & rScene)
{
	if (MARGIN_CONTROL::mfIsScreenReduced() == TRUE)
	{
		gCombatMenuRequested = TRUE;
		ShowCombatUIMenu(rScene);
	}
	else
	if (gCombatMenuRequested == TRUE)
	{
		ShowCombatUIMenu(rScene);
	}
}
/* ========================================================================
   Function    - ShowCombatUIMenu
   Description - Display the combatui menus.
   Returns     -
   ======================================================================== */
static void ShowCombatUIMenu(SCENE & rScene)
{
	if (gCombatMenuIsUp == FALSE)
	{
		DumbAutoLockPtr<COMBAT_SCENE_DATA> const pcsdData(rScene.fhData);
		LONG MenuXPos = 0;
		LONG MenuYPos = 0;
		LONG i;
		LONG NumberOfFollowers = 0;
		
#if defined (_WINDOWS)
// [d11-12-96 JPC] Prevent menu from being expanded on screen.
		if (!fHighRes)
			ForceHires ();
#endif
		gCombatMenuIsUp = TRUE;
		
		pcsdData->mfInitMenuVals();
		
		/* -----------------------------------------------------------------
		   put up the control panel menu
		   ----------------------------------------------------------------- */
		printf("Region count before Combat Menu: %ld\n", CountRegions());
		push_regions();
		// Add back the critcial keys & regions.
		AddGameKeys();
		
		HideMenu(D_GAMEBUTTON);
		ShowMenu(D_GAMEBUTTON);
		
		// Menu buttons that work without being in combat mode.
		// GWP HACK Keys
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_RETREAT), CombatRetreatKey, 0, 0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_NEW_LEADER), CombatNewLeaderKey, 0, 0);
		
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::EXPAND_VIEWING_AREA), IncreaseAdventureScreen, 0, 0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::SHRINK_VIEWING_AREA), DecreaseAdventureScreen, 0, 0);
		
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_TOGGLEMODE), ToggleCombatMode, 0, 0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_RETURN_TO_MAP), CombatReturnToMap, 0, 0);
		
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_SHOW_STATUS), CombatShowStatusKey, 0, 0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_SHOW_INVENTORY), CombatShowInventoryKey,0 ,0);
		
		// Combat menu control keys
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_HIGH_ATTACK), CombatHighAttackKey, 0, 0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_LOW_ATTACK), CombatLowAttackKey, 0, 0);
		
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_FALLBACK), CombatFallbackKey, 0, 0);
		RECHECK_OPTIONS::mfUpDateOptions();
		
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_SPELL1), CombatSpellKey,0,0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_SPELL2), CombatSpellKey,1,1);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_SPELL3), CombatSpellKey,2,2);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_SPELL4), CombatSpellKey,3,3);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_SPELL5), CombatSpellKey,4,4);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_SPELL6), CombatSpellKey,5,5);
		
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_CYCLE_ACTIVE), CombatCycleActive, 0, 0);
		
		// Combat camera position keys
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_CAMERA_FIRSTPERSON), CombatFirstPersonView, 0, 0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_CAMERA_BEHINDFRIEND), CombatAutoViewBehindFriend, 0, 0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_CAMERA_BEHINDFOE), CombatAutoViewBehindFoe, 0, 0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_CAMERA_RIGHTFRIEND), CombatAutoViewFriendRight, 0, 0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_CAMERA_LEFTFRIEND), CombatAutoViewFriendLeft, 0, 0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::CAMERA_FOLLOW_BEHIND), CombatBehindPersonView, 0, 0);
	
		// For the team of adventurers, connect them to the menus.
		NumberOfFollowers = ADVENTURER::mfNumberOfAdventurers();
		
		CONTROL_PANELS::mfInitAllPanels();
		
		// Show a plain background
		// Also only draw it if necessary.
		if (NumberOfFollowers == 1 ||
			NumberOfFollowers == 3)
		{
			ShowPanel(D_COMBAT_BLANK1_BAR);
		 	gblankBarIndex = D_COMBAT_BLANK1_BAR; // for hiding blank bar menu later
	
		}
		else
		{
			gblankBarIndex = fERROR;
		}
		
		ShowPanel(D_COMBAT_RPGBOTTOM_BAR);
		
		// shrink the render size to make room for the control panel
		if (NumberOfFollowers < 3)
		{
			GetButtonPosition(D_COMBAT_CONTROL3, 0, &MenuXPos, &MenuYPos);
		}
		else
		{
			GetButtonPosition(D_COMBAT_CONTROL1, 0, &MenuXPos, &MenuYPos);
		}
		
		if(fChatLineOn)
		{
			MoveChatDisplayWindow(0,0);
			set_margin_size (0, 0, CHAT_DISPLAY_HEIGHT, MAX_VIEW_HEIGHT - MenuYPos);
		}
		else
		{
			set_margin_size (0, 0, 0, MAX_VIEW_HEIGHT - MenuYPos);
		}
		ClearRemapTable();
	}
}

/* ========================================================================
   Function    - InitCombatUI
   Description - Called at the beginning of entering "Single" or group
                 combat mode.
   Returns     -
   ======================================================================== */
void InitCombatUI(SCENE & rScene)
{
	SHORT hLeader = fERROR;
	DumbAutoLockPtr<COMBAT_SCENE_DATA> const pcsdData(rScene.fhData);
	LONG i;
	
	pcsdData->mfInitCombatVals();
	
	if (gCombatMenuIsUp == FALSE)
	{
		// It's not already up.
		ShowCombatUIMenu(rScene);
	}
	
	CONTROL_PANELS::mfSetBeginCombatAll();
	
	PlayEnvironmentalSounds = FALSE;
	
	if (COMBAT_MODE::mfIsModeRPG())
	{
		gRPGTurnGo = FALSE;
	}
	else
	{
		gRPGTurnGo = TRUE;
	}
	
	gccCamera.mfInitializeCamera();
	
	gCountDownMode = TRUE;
	gCountDown = FRAMES_TO_WAIT;	// Frames
}
/* ========================================================================
   Function    - HideCombatUIMenu
   Description -  Take down the combat menus.
   Returns     -
   ======================================================================== */
static void HideCombatUIMenu(SCENE & rScene)
{
	if (gCombatMenuIsUp == TRUE)
	{
		DumbAutoLockPtr<COMBAT_SCENE_DATA> const pcsdData(rScene.fhData);
		gCombatMenuIsUp = FALSE;
		
		pcsdData->mfResetMenuVals();
		
		CONTROL_PANELS::mfReleaseAll();
		
		HidePanel(D_COMBAT_RPGBOTTOM_BAR);
		
	 	if (gblankBarIndex != fERROR)
	 	{
	 		HidePanel(gblankBarIndex);
	 		gblankBarIndex = fERROR;
	 	}
		
		pop_regions();
		//HideMenu(D_GAMEBUTTON);
		//ShowMenu(D_GAMEBUTTON);
		printf("Region count after Combat Menu: %ld\n", CountRegions());
	}
}

/* ========================================================================
   Function    - ReleaseCombatUI
   Description -  Restore things to the way they were.
   Returns     -
   ======================================================================== */
void ReleaseCombatUI(SCENE &rScene)
{
	LONG i;
	
	for (i = 0; i < MAX_AVATARS; i++)
	{
		if (rScene.Avatars[i] != fERROR)
		{
			//GEH SetBlockAttr(rScene.Avatars[i], LOCKED, LOCKED);
			CAvatar * const pAvatar = (CAvatar * const) BLKPTR(rScene.Avatars[i]);
			
			// Clear the Pause mode (if set)
			pAvatar->DoAI(CAvatar::AI_END_PAUSE);
			
			// Clear the taunts.
			pAvatar->attrib.IssuedTaunt = FALSE;
			
			// Just incase he died.
			//GEH if (rScene.Avatars[i] != fERROR)
			//GEH 	ClrLock(rScene.Avatars[i]);
		}
	}
	
	CONTROL_PANELS::mfSetEndAllCombat();
	
	if (gCombatMenuRequested == FALSE)
	{
		// Don't leave the menu up when leaving combat mode unless the
		// screen is reduced.
		HideCombatUIMenu(rScene);
	}
	
	// Even though we're not using this camera right now, set it to be 
	// in the state we would be using. This keeps the fMedAnim flag from
	// being set incorrectly.
	gccCamera.mfSetState(COMBAT_CAMERA::FOLLOW_AVATAR);
	if (rScene.fhData != fERROR)
	{
		DumbAutoLockPtr<COMBAT_SCENE_DATA> const pcsdData(rScene.fhData);
		pcsdData->mfRestoreCombatVals();
	}
	
	// Reset the accelleration vectors.
	StopPlayer();
	
	// Stop removing the draw from the things engine.
	ClearDontDrawCloser();
}

/* ========================================================================
   Function    - PlayCombatUI
   Description - Run the Combat UI and move the camera.
   Returns     -
   ======================================================================== */
void PlayCombatUI(SCENE &rScene)
{
	LONG i;
	
	// if in targetting mode
	if (COMBAT_TARGETTING::mfIsModeTargetting() == TRUE )
	{
		// slide the scene timers during paused time
		if (rScene.fhData != fERROR)
		{
			DumbAutoLockPtr<COMBAT_SCENE_DATA> const pcsdData(rScene.fhData);
			pcsdData->mfPauseUpdateData();
		}
		
		// drive the camera
		gccCamera.mfUpdatePosition();
		
		// if target selected, set it and flip the previous mode back on
		if(COMBAT_TARGETTING::mfGetTargetIndex() != (LONG)fERROR)
		{
			CAvatar * pTargetAvatar;
			BOOL TargetFound = FALSE;
			
			// check to see if this thing index is an avatar
			for(LONG i = 0; i < MAX_AVATARS; i++ )
			{
				// This is a sparsly filled array.
				if (rScene.Avatars[i] != fERROR)
				{
					pTargetAvatar = (CAvatar *) BLKPTR(rScene.Avatars[i]);
					if(pTargetAvatar->ThingIndex == COMBAT_TARGETTING::mfGetTargetIndex())
					{
						TargetFound = TRUE;
						break; // found him
					}
				}
			}
			
			if (rScene.fhData != fERROR)
			{
				DumbAutoLockPtr<COMBAT_SCENE_DATA> const pcsdData(rScene.fhData);
				pcsdData->mfInitCombatTimer();
			}
			
			// get the avatar who cast the spell
			CAvatar * const pShootingAvatar = (CAvatar *) BLKPTR (COMBAT_TARGETTING::mfWhoseShooting());
				
			pShootingAvatar->mfSetAttackMode(FIGHT_SEQUENCE::ATM_MAGIC);
			pShootingAvatar->mfSetAttackSequence(FIGHT_SEQUENCE::ATM_MAGIC);
							
			// look at my target
			if (pShootingAvatar->ThingIndex != COMBAT_TARGETTING::mfGetTargetIndex())
			{
				pShootingAvatar->FaceTo( mythings[COMBAT_TARGETTING::mfGetTargetIndex()].x,
									   	   mythings[COMBAT_TARGETTING::mfGetTargetIndex()].y );
			}
			
			// if no target found or I'm my own target, fail to get an enemy.
			if (TargetFound == FALSE
				|| pShootingAvatar->ThingIndex == COMBAT_TARGETTING::mfGetTargetIndex()
				|| pTargetAvatar->GetAIFuncIndex() == CAvatar::AI_FUNC_FIREBALL
				)
			{
				pShootingAvatar->mfSethEnemy(fERROR);
			}
			else
			{
				// set the spell caster's hEnemy to be this object
				pShootingAvatar->mfSethEnemy(rScene.Avatars[i]);
				
			}
			
			pShootingAvatar->fDamageFlag = 1;
			
			// clear target
			COMBAT_TARGETTING::mfInitVals();
		}
		else
		{
			// While we are waiting for you to select a target keep the ai's running.
			// Note: Most of them are in pause mode.
			for(LONG i = 0; i < MAX_AVATARS; i++ )
			{
				// This is a sparsly filled array.
				if (rScene.Avatars[i] != fERROR)
				{
					//GEH SetBlockAttr(rScene.Avatars[i], LOCKED, LOCKED);
					CAvatar * const pAvatar = (CAvatar * const ) BLKPTR(rScene.Avatars[i]);
					pAvatar->DoAI();
					// Hey he could have died!
					//GEH if (rScene.Avatars[i] != fERROR)
					//GEH {
					//GEH 	ClrLock(rScene.Avatars[i]);
					//GEH }
				}
			}
			
			CONTROL_PANELS::mfCheckAllPanels();
		}
		
	}
	else	// run the avatar AI's
	{
		if (RECHECK_OPTIONS::mfHaveOptionsChanged())
		{
			RECHECK_OPTIONS::mfUpDateOptions();
		}
		
		for(LONG i = 0; i < MAX_AVATARS; i++ )
		{
			// This is a sparsly filled array.
			if (rScene.Avatars[i] != fERROR)
			{
				//GEH SetBlockAttr(rScene.Avatars[i], LOCKED, LOCKED);
				CAvatar * const pAvatar = (CAvatar * const ) BLKPTR(rScene.Avatars[i]);
				pAvatar->DoAI();
				// Hey he could have died!
				//GEH if (rScene.Avatars[i] != fERROR)
				//GEH {
				//GEH 	ClrLock(rScene.Avatars[i]);
				//GEH }
			}
		}
		
		if (rScene.fhData == fERROR)
			return;	// Don't crash but you're toast.
			
		//GEH SetBlockAttr(rScene.fhData, LOCKED, LOCKED);
		DumbAutoLockPtr<COMBAT_SCENE_DATA> const pcsdData(rScene.fhData);
		
		CONTROL_PANELS::mfCheckAllPanels();
		
		if (pcsdData->fRetreatData.mfIsRetreating() == TRUE)
		{
			if ( TRUE == pcsdData->fRetreatData.mfGetNextPt(&player))
			{
				ADVENTURER::mfUpdateAdventureTeam(&player);
				gccCamera.mfUpdatePosition();
			}
			else
			{
				rScene.mfSetSceneState(SCENE_AI::RELEASE);
			}
		}
		else
		{
			gccCamera.mfUpdatePosition();
			
			BOOL const CombatFlag = (ADVENTURER::mfIsAdventureTeam(&CAvatar::mfAmIBeingAttacked) ||
									 ADVENTURER::mfIsAdventureTeam(&CAvatar::mfAmICastingASpell) ||
									 ADVENTURER::mfIsAdventureTeam(&CAvatar::mfAmIChasingAnEnemy));
			
			// test whether we are still fighting anybody.
			if (CombatFlag)
			{
				pcsdData->mfInitCombatTimer();
			}
			else
			{
				// If no combat occurs, drop out of combat mode.
				if (pcsdData->mfTestCombatTimedOut() == TRUE)
				{
					rScene.mfSetSceneState(SCENE_AI::RELEASE);
				}
			}
			
			if (COMBAT_MODE::mfIsModeRPG() == TRUE )
			{
				if (gRPGTurnGo == TRUE)
				{
					BOOL SomeOneNotDone = FALSE;
					for (i = 0; i < MAX_AVATARS; i++)
					{
						if (rScene.Avatars[i] != fERROR)
						{
							CAvatar * const pAvatar = (CAvatar * const ) BLKPTR(rScene.Avatars[i]);
							if (!pAvatar->mfIsAllDamageDone())
							{
								SomeOneNotDone = TRUE;
								break;
							}
						}
					}
					
					if (SomeOneNotDone == FALSE || pcsdData->mfTestRPGTimedOut())
					{
						// Pause all the AI's.
						for (i = 0; i < MAX_AVATARS; i++)
						{
							if (rScene.Avatars[i] != fERROR)
							{
								//GEH SetBlockAttr(rScene.Avatars[i], LOCKED, LOCKED);
								CAvatar * const pAvatar = (CAvatar * const) BLKPTR(rScene.Avatars[i]);
								pAvatar->DoAI(CAvatar::AI_BEGIN_PAUSE);
								pAvatar->mfClearTurnToAttack();
								
								// Just incase he died.
								//GEH if (rScene.Avatars[i] != fERROR)
								//GEH 	ClrLock(rScene.Avatars[i]);
								
							}
						}
						
						// Reset the Adventure team back to Defend mode for the next turn.
						CONTROL_PANELS::mfSetAllDefend();
						gRPGTurnGo = FALSE;
					}
				}
			}
			else
			{
				// Arcade Mode
				
				if (gCountDownMode)		// Waiting for camera to get into position.
				{
					gCountDown--;
					if (gCountDown < 0)
					{
						LONG i;
						
						gCountDownMode = FALSE;
						for (i = 0; i < MAX_AVATARS; i++)
						{
							if (rScene.Avatars[i] != fERROR)
							{
								//GEH SetBlockAttr(rScene.Avatars[i], LOCKED, LOCKED);
								CAvatar *const pAvatar = (CAvatar * const) BLKPTR(rScene.Avatars[i]);
								pAvatar->DoAI(CAvatar::AI_END_PAUSE);
								pAvatar->mfDecideWhoseTurnToAttack();
								
								// Just incase he died.
								//GEH if (rScene.Avatars[i] != fERROR)
								//GEH 	ClrLock(rScene.Avatars[i]);
								
							}
						}
					}
				}
			}
		}
		//GEH ClrLock(rScene.fhData);
		
		// Let the DamageTimers be reset if necessary.
		AdventureTripDamageTimer(rScene);
	}
}
/* ========================================================================
   Function    - PauseCombatUI
   Description - Run the camera, and update the timers so that when we
   				 restart things will continue as before.
   Returns     -
   ======================================================================== */
void PauseCombatUI(SCENE & rScene )
{
	gccCamera.mfUpdatePosition();
	if (rScene.fhData != fERROR)
	{
		DumbAutoLockPtr<COMBAT_SCENE_DATA> const pcsdData(rScene.fhData);
		pcsdData->mfPauseUpdateData();
	}
}

/* ========================================================================
   Function    - DecreaseAdventureScreen
   Description - Reduce the viewing area.
   Returns     -
   ======================================================================== */
void DecreaseAdventureScreen(LONG, LONG)
{
	if (gCombatMenuRequested == FALSE)
	{
		gCombatMenuRequested = TRUE;
		
		if (gCombatMenuIsUp == FALSE)
		{
			SCENE * const pCurrentScene = (SCENE * const) BLKPTR(SCENE_MGR::hCurrentScene);
			ShowCombatUIMenu(*pCurrentScene);
		}
		else
		{
			MARGIN_CONTROL::mfReduceScreenSize(0, 0);
		}
	}
	else
	{
		MARGIN_CONTROL::mfReduceScreenSize(0, 0);
	}
}
/* ========================================================================
   Function    - IncreaseAdventureScreen
   Description - Increase the viewing area.
   Returns     -
   ======================================================================== */
void IncreaseAdventureScreen(LONG, LONG)
{
	if (FALSE == MARGIN_CONTROL::mfIncreaseScreenSize(0, 0))
	{
		gCombatMenuRequested = FALSE;
		
		if (SCENE_MGR::hCurrentScene != fERROR)
		{
			SCENE_AI::STATE OldSceneState;
			SCENE_AI::SCENE_TYPE OldSceneType;
			
			SCENE * const pCurrentScene = (SCENE * const) BLKPTR(SCENE_MGR::hCurrentScene);
			pCurrentScene->mfGetSceneAIState(&OldSceneState,
							                 &OldSceneType);
			
			if (OldSceneType != SCENE_AI::COMBAT_SCENE)
			{
				HideCombatUIMenu(*pCurrentScene);
			}
		}
	}
}
/* ========================================================================
   Function    - ToggleCombatMode
   Description - Switch the Scene AI to combat mode.
   Returns     -
   ======================================================================== */
void ToggleCombatMode(LONG, LONG)
{
	if (SCENE_MGR::hCurrentScene != fERROR)
	{
		SCENE_AI::STATE OldSceneState;
		SCENE_AI::SCENE_TYPE OldSceneType;
		
		SCENE * const pCurrentScene = (SCENE * const) BLKPTR(SCENE_MGR::hCurrentScene);
		pCurrentScene->mfGetSceneAIState(&OldSceneState,
						                 &OldSceneType);
		
		if (OldSceneType == SCENE_AI::COMBAT_SCENE)
		{
			pCurrentScene->mfSetSceneState(SCENE_AI::RELEASE);
		}
		else
		{
			pCurrentScene->mfSetSceneType (SCENE_AI::COMBAT_SCENE);
			pCurrentScene->mfSetSceneState(SCENE_AI::INIT);
		}
		pCurrentScene->mfCallSceneAI();
	}
}

/* ========================================================================
   Function    - CombatDefendKey
   Description - Put the Avatar attached to the current active menu in
   				 Defend mode.
   Returns     -
   ======================================================================== */
void CombatDefendKey(LONG menuid, LONG )
{
	if (mouse_button == 2)
	{
		// Display help dialog on right click
		//CombatHelpDialog(0, COMBAT_STR_HELP_DEFEND);
		mouse_click = 0;
		SystemHelp(COMBAT_STR_HELP_DEFEND_TITLE, COMBAT_STR_HELP_DEFEND, H_Attack_Defend_Mode,0,0);
	}
	else if ((mouse_click == 1 && mouse_button == 1) || mouse_click == 0)
	{
		mouse_click = 0;
		if (COMBAT_MODE::mfIsModeRPG() == TRUE && gRPGTurnGo == TRUE)
		{
			return;
		}
	    
	    if (COMBAT_TARGETTING::mfIsModeTargetting() == TRUE )			// can't target two things.
	    {
	    	COMBAT_TARGETTING::mfStopTargeting();
	    }
		
		CONTROL_PANELS::mfMatchMenuToCurrent(HIGH_LONG(menuid));
		
		if (CONTROL_PANELS::mfCurrentInUse())
		{
			CAvatar *const pAvatar = (CAvatar *const ) BLKPTR (CONTROL_PANELS::mfGetCurrAvatarHdl());
			if (!pAvatar->mfAmIImmoblized())
			{
				SCENE_AI::STATE OldSceneState;
				SCENE_AI::SCENE_TYPE OldSceneType;
				
				SCENE * const pCurrentScene = (SCENE * const) BLKPTR(SCENE_MGR::hCurrentScene);
				pCurrentScene->mfGetSceneAIState(&OldSceneState,
								                 &OldSceneType);
				
				pAvatar->mfSetAttackMode(FIGHT_SEQUENCE::ATM_DEFEND);
				pAvatar->mfSetAttackSequence(FIGHT_SEQUENCE::ATM_DEFEND);
				if (OldSceneType == SCENE_AI::COMBAT_SCENE)
				{
					if (pCurrentScene->fhData != fERROR)
					{
						DumbAutoLockPtr<COMBAT_SCENE_DATA> const pcsdData(pCurrentScene->fhData);
						pcsdData->mfInitCombatTimer();
					}
				}
				else
				{
					pCurrentScene->mfSetSceneType (SCENE_AI::COMBAT_SCENE);
					pCurrentScene->mfSetSceneState(SCENE_AI::INIT);
					pCurrentScene->mfCallSceneAI();
					CONTROL_PANELS::mfCurrentSpeakAcknowledgement();
				}
			}
		}
	}
}

/* ========================================================================
   Function    - CombatFallbackKey
   Description - Put the Avatar attached to the current active menu in
   				 FallBack mode.
   Returns     -
   ======================================================================== */
void CombatFallbackKey(LONG menuid, LONG )
{
	if (mouse_button == 2)
	{
		// Display help dialog on right click
		//CombatHelpDialog(0, COMBAT_STR_HELP_FALLBACK);
		mouse_click = 0;
		SystemHelp(COMBAT_STR_HELP_FALLBACK_TITLE, COMBAT_STR_HELP_FALLBACK, -1, 0,0);
	}
	else if ((mouse_click == 1 && mouse_button == 1) || mouse_click == 0)
	{
		mouse_click = 0;
		// Only enabled for RPG mode.
		if (COMBAT_MODE::mfIsModeRPG() == FALSE)
			return;
			
		if (gRPGTurnGo == TRUE)
		{
			return;
		}
		
	    if (COMBAT_TARGETTING::mfIsModeTargetting() == TRUE )			// can't target two things.
	    {
	    	COMBAT_TARGETTING::mfStopTargeting();
	    }
		
		CONTROL_PANELS::mfMatchMenuToCurrent(HIGH_LONG(menuid));
		
		if (CONTROL_PANELS::mfCurrentInUse())
		{
			CAvatar * const pAvatar = (CAvatar * const) BLKPTR (CONTROL_PANELS::mfGetCurrAvatarHdl());
			if (!pAvatar->mfAmIImmoblized())
			{
				if (SCENE_MGR::hCurrentScene != fERROR)
				{
					SCENE_AI::STATE OldSceneState;
					SCENE_AI::SCENE_TYPE OldSceneType;
					
					SCENE * const pCurrentScene = (SCENE * const) BLKPTR(SCENE_MGR::hCurrentScene);
					pCurrentScene->mfGetSceneAIState(&OldSceneState,
									                 &OldSceneType);
					
					if (OldSceneType == SCENE_AI::COMBAT_SCENE)
					{
						if (pCurrentScene->fhData != fERROR)
						{
							DumbAutoLockPtr<COMBAT_SCENE_DATA> const pcsdData(pCurrentScene->fhData);
							pcsdData->mfInitCombatTimer();
						}
						
						if (COMBAT_MODE::mfIsModeRPG() == TRUE )
						{
							pAvatar->fPrev.Status = CAvatar::AI_FALLBACK;
						}
						else
						{
							pAvatar->Status = CAvatar::AI_FALLBACK;
						}
					}
					else
					{
						pAvatar->Status = CAvatar::AI_FALLBACK;
						pCurrentScene->mfSetSceneType (SCENE_AI::COMBAT_SCENE);
						pCurrentScene->mfSetSceneState(SCENE_AI::INIT);
						pCurrentScene->mfCallSceneAI();
						CONTROL_PANELS::mfCurrentSpeakAcknowledgement();
					}
				}
			}
		}
	}
}

/* ========================================================================
   Function    - CombatCastSpellBox
   Description - Call back from inventory menu to cast a spell.
   Returns     - 
   ======================================================================== */
void CombatCastSpellBox(LONG hAvatar , LONG SpellBoxNumber )
{
	CONTROL_PANELS::mfMatchAvatarHdlToCurrent(hAvatar);
	CombatSpellKey(0, SpellBoxNumber);
}

/* ========================================================================
   Function    - CombatSpellKey
   Description - Activate the spell at this button for the avatar connected
   				 to the current active menu.
   Returns     -
   ======================================================================== */
void CombatSpellKey(
	LONG menuid,
	LONG SpellNumber
	)
{
	if (mouse_button == 2)
	{
		// Display help dialog on right click
		//CombatHelpDialog(0, COMBAT_STR_HELP_SPELLBOX);
		mouse_click = 0;
		SystemHelp(COMBAT_STR_HELP_SPELLBOX_TITLE, COMBAT_STR_HELP_SPELLBOX, H_Adventure_Hot_Boxes, 0, 0);
	}
	else // if ((mouse_click == 1 && mouse_button == 1) || mouse_click == 0)
	{
		mouse_click = 0;
		if ((COMBAT_MODE::mfIsModeRPG() == TRUE && gRPGTurnGo == TRUE) )
		{
			return;
		}
	    
	    if (COMBAT_TARGETTING::mfIsModeTargetting() == TRUE )			// can't target two things.
	    {
	    	COMBAT_TARGETTING::mfStopTargeting();
	    }
		
		CONTROL_PANELS::mfMatchMenuToCurrent(HIGH_LONG(menuid));
		
		if (CONTROL_PANELS::mfCurrentInUse())
		{
			// GWP At some future point connect the spell number to the Avatar.
			CAvatar *const pAvatar = (CAvatar *const ) BLKPTR (CONTROL_PANELS::mfGetCurrAvatarHdl());
			
			// GWP At some future time validate that there is a spell waiting for us.
			if (!pAvatar->mfAmIImmoblized())
			{
				if (pAvatar->hPlayerStats != fERROR)
				{
					DumbAutoLockPtr<PLAYER_STATS > const pPlayerStats(pAvatar->hPlayerStats);
					InvenIndex const sSpell = pPlayerStats->Inventory.mfGetSpellBox(SpellNumber);
					pAvatar->mfSetDamageDealt(-1);
					
					if (sSpell > -1)
					{
						SCENE_AI::STATE OldSceneState;
						SCENE_AI::SCENE_TYPE OldSceneType;
						
						SCENE * const pCurrentScene = (SCENE * const) BLKPTR(SCENE_MGR::hCurrentScene);
						pCurrentScene->mfGetSceneAIState(&OldSceneState,
										                 &OldSceneType);
						
						ITEMTYPE const SpellType = ObjectList::mfData(sSpell).mfType();
						// if I am required to wave my hands, switch modes
						if (ItemTypes[SpellType].mfRequires(HANDWAVE) ||
							ItemTypes[SpellType].mfRequires(TARGET))
						{
							COMBAT_TARGETTING::mfSetModeTargetting();
							
							// Must be set before the AI is switched so that we
							// won't try to move the camera.
							pAvatar->fFollowPlayer.fCurrentSpellBox = SpellNumber;
							
							if (OldSceneType == SCENE_AI::COMBAT_SCENE)
							{
								if (pCurrentScene->fhData != fERROR)
								{
									DumbAutoLockPtr<COMBAT_SCENE_DATA> const pcsdData(pCurrentScene->fhData);
									pcsdData->mfInitCombatTimer();
								}
							}
							else
							{
								pCurrentScene->mfSetSceneType (SCENE_AI::COMBAT_SCENE);
								pCurrentScene->mfSetSceneState(SCENE_AI::INIT);
								pCurrentScene->mfCallSceneAI();
								CONTROL_PANELS::mfCurrentSpeakAcknowledgement();
							}
							
							// if this spell requires targetting, switch to
							// to targetting mode and pause game
							if (ItemTypes[SpellType].mfRequires(TARGET))
							{
								COMBAT_TARGETTING::mfSetupTargetting(pAvatar->hThis);
							}
						}
						else
						{
							// if this spell requires targetting, switch to
							// to targetting mode.
							if (ItemTypes[SpellType].mfRequires(TARGET))
							{
								pAvatar->fFollowPlayer.fCurrentSpellBox = SpellNumber;
								COMBAT_TARGETTING::mfSetupTargetting(pAvatar->hThis);
							}
							else
							{
								if (OldSceneType == SCENE_AI::COMBAT_SCENE &&
								    COMBAT_MODE::mfIsModeRPG() == TRUE)
								{
									pAvatar->fFollowPlayer.fCurrentSpellBox = SpellNumber;
									pAvatar->mfSetAttackMode(FIGHT_SEQUENCE::ATM_MAGIC);
									pAvatar->mfSetAttackSequence(FIGHT_SEQUENCE::ATM_MAGIC);
								}
								else
								{
									pPlayerStats->Inventory.mfActivateSpellBox(SpellNumber);
								}
							}
						}
					}
				}
			}
		}
	}
}


/* ========================================================================
   Function    - CombatSwitchActive
   Description - Switch the active menu to this one.
   Returns     -
   ======================================================================== */
void CombatSwitchActive(LONG menuid, LONG )
{
	CONTROL_PANELS::mfMatchMenuToCurrent(HIGH_LONG(menuid));
	
	// Highlight the active menu.
	if (CONTROL_PANELS::mfCurrentInUse())
	{
		CAvatar *const pAvatar = (CAvatar *const ) BLKPTR (CONTROL_PANELS::mfGetCurrAvatarHdl());
		// Left button
		if (mouse_button == 1)
		{
			mouse_click = 0;
			if (!pAvatar->mfAmIImmoblized())
			{
				gccCamera.mfSetNewLeader(CONTROL_PANELS::mfGetCurrAvatarHdl());
				
				CONTROL_PANELS::mfCurrentSpeakAcknowledgement();
			}
		}
		else if (mouse_button == 2)	// Right button put up inventory
		{
			mouse_click = 0;
			mouse_button = 0;
			StaticInventoryGUI::mfDisplay(0, pAvatar->hPlayerStats);
		}
	}
}

/* ========================================================================
   Function    - CombatCycleActive
   Description - Cycle to the next available active menu.
   Returns     -
   ======================================================================== */
void CombatCycleActive(LONG, LONG)
{
	CONTROL_PANELS::mfCycleToNextPanel();
	fUpdatePanels = TRUE;
	
	CAvatar * const pAvatar = (CAvatar * const ) BLKPTR (CONTROL_PANELS::mfGetCurrAvatarHdl());
	if (!pAvatar->mfAmIImmoblized())
	{
		gccCamera.mfSetNewLeader(CONTROL_PANELS::mfGetCurrAvatarHdl());
	}
}


/* ========================================================================
   Function    - CombatReturnToMap
   Description - Cleanup and return to the map.
   Returns     -
   ======================================================================== */
void CombatReturnToMap(LONG , LONG)
{
	if (mouse_button == 2)
	{
		// Display help dialog on right click
		//CombatHelpDialog(0, COMBAT_STR_HELP_EXIT);
		mouse_click = 0;
		SystemHelp(COMBAT_STR_HELP_EXIT_TITLE, COMBAT_STR_HELP_EXIT,H_Exit_Adventure_Button, 0, 0);
	}
	else if ((mouse_click == 1 && mouse_button == 1) || mouse_click == 0)
	{
		mouse_click = 0;
#ifdef _WINDOWS
		
		//---- Dont allow non adventure starter to leave the game
	
		if ( IsMultiPlayer() &&
			 !EventStarter      )
		{
			return;
		}
	
#endif
		
		// click the button
		SetButtonHilight(D_COMBAT_RPGBOTTOM_BAR, BUTNO_RPG_LEAVE, TRUE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		SetButtonHilight(D_COMBAT_RPGBOTTOM_BAR, BUTNO_RPG_LEAVE, FALSE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		
	    if (COMBAT_TARGETTING::mfIsModeTargetting() == TRUE )			// can't target two things.
	    {
	    	COMBAT_TARGETTING::mfStopTargeting();
	    }
		
		QueryAdvExit(1000, 1000);
	}
}

/*	======================================================================== */

void AdvExit (LONG MenuCombo, LONG button)
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
	
	SetButtonLabel  (MenuId, QUESTION_TEXT, -1, BLACK );
	fUpdateDataArea=TRUE;
	if(button == QUESTION_BTN1)
	{
		if (SCENE_MGR::hCurrentScene != fERROR)
		{
			SCENE_AI::STATE OldSceneState;
			SCENE_AI::SCENE_TYPE OldSceneType;
		
			SCENE * const pCurrentScene = (SCENE * const) BLKPTR(SCENE_MGR::hCurrentScene);
			pCurrentScene->mfGetSceneAIState(&OldSceneState,
						                 &OldSceneType);
		
	
			if (OldSceneType == SCENE_AI::COMBAT_SCENE)
			{
				pCurrentScene->mfSetSceneState(SCENE_AI::RELEASE);
				pCurrentScene->mfCallSceneAI();
			}
		}
	
		AdventureEndDialog(0);

	}
}

/*	======================================================================== */
void QueryAdvExit (LONG, LONG)
{

#if defined (_WINDOWS)
	//  Prevent menu from being expanded on screen.
	if (!fHighRes)
		ForceHires ();
#endif

	// set the question text
	SetButtonLabel  (D_QUESTION2, QUESTION_TEXT, STR_MENU_BUG_OUT, BLACK );
	SetButtonProc   (D_QUESTION2, 0, 0, 0, NO_KEY);
	
	// turn on the first button as YES
	SetButtonLabel  (D_QUESTION2, QUESTION_BTN1, STR_YES, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION2, QUESTION_BTN1, AdvExit, QUESTION_BTN1, D_KEY_YES );
	
	// turn on the last button as CANCEL
	SetButtonLabel  (D_QUESTION2, QUESTION_BTN2, STR_CANCEL, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION2, QUESTION_BTN2, AdvExit, QUESTION_BTN2, D_KEY_CANCEL );
	
	ShowMenu(D_QUESTION2);
}
/*	======================================================================== */


/* ========================================================================
   Function    - CombatRetreatKey
   Description - Try to get us out of combat by retreating.
   Returns     -
   ======================================================================== */
void CombatRetreatKey(LONG , LONG )
{
	if (mouse_button == 2)
	{
		// Display help dialog on right click
		//CombatHelpDialog(0, COMBAT_STR_HELP_RETREAT);
		mouse_click = 0;
		SystemHelp(COMBAT_STR_HELP_RETREAT_TITLE, COMBAT_STR_HELP_RETREAT, H_Party_Retreat,0, 0);
	}
	else if ((mouse_click == 1 && mouse_button == 1) || mouse_click == 0)
	{
		mouse_click = 0;
		if (COMBAT_MODE::mfIsModeRPG() == TRUE && gRPGTurnGo == TRUE)
		{
			return;
		}
	    
	    if (COMBAT_TARGETTING::mfIsModeTargetting() == TRUE )			// can't target two things.
	    {
	    	COMBAT_TARGETTING::mfStopTargeting();
	    }
		
		// click the button
		SetButtonHilight(D_COMBAT_RPGBOTTOM_BAR, BUTNO_RPG_RETREAT, TRUE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		SetButtonHilight(D_COMBAT_RPGBOTTOM_BAR, BUTNO_RPG_RETREAT, FALSE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		
		if (SCENE_MGR::hCurrentScene != fERROR)
		{
			LONG i;
			
			PTR_SCENE const pCurrentScene = (PTR_SCENE const ) BLKPTR(SCENE_MGR::hCurrentScene);
			
			SCENE_AI::STATE OldSceneState;
			SCENE_AI::SCENE_TYPE OldSceneType;
			pCurrentScene->mfGetSceneAIState(&OldSceneState,
							                 &OldSceneType);
			
			if (OldSceneType != SCENE_AI::COMBAT_SCENE)
			{
				pCurrentScene->mfSetSceneType (SCENE_AI::COMBAT_SCENE);
				pCurrentScene->mfSetSceneState(SCENE_AI::INIT);
				pCurrentScene->mfCallSceneAI();
			}
			
			if (CONTROL_PANELS::mfCurrentInUse())
			{
				SetCameraMotionVars(&camera,
				                    TARGET_FACTOR,
				                    NO_CHANGE);
				CAvatar const *const pAvatar = (CAvatar const *const ) BLKPTR(CONTROL_PANELS::mfGetCurrAvatarHdl());
				SetCameraTarget(&camera,
							    pAvatar->mfX(),
							    pAvatar->mfY(),
							    pAvatar->mfZ() + PLAYER_HEIGHT,
							    ((pAvatar->mfAngle() - 128) & 0xFF),
							    0,									// Avatars have no pitch.
							    TRUE
							    );
			}
			gccCamera.mfSetState(COMBAT_CAMERA::FACING_BACKWARDS);
			
			if (COMBAT_MODE::mfIsModeRPG() == TRUE)
			{
				for (i = 0; i < MAX_AVATARS; i++)
				{
					if (pCurrentScene->Avatars[i] != fERROR)
					{
						//GEH SetBlockAttr(pCurrentScene->Avatars[i], LOCKED, LOCKED);
						CAvatar * const pAvatar = (CAvatar * const) BLKPTR(pCurrentScene->Avatars[i]);
						pAvatar->DoAI(CAvatar::AI_END_PAUSE);
						
						// Just incase he died.
						//GEH if (pCurrentScene->Avatars[i] != fERROR)
						//GEH 	ClrLock(pCurrentScene->Avatars[i]);
					}
				}
			}
			
			CONTROL_PANELS::mfSetEndAllCombat();
			
			if (pCurrentScene->fhData != fERROR)
			{
				// Lock the block as the Start Retreat allocates and
				// moves memory.
				SetBlockAttr(pCurrentScene->fhData, LOCKED, LOCKED);
				
				DumbAutoLockPtr<COMBAT_SCENE_DATA> const pcsdData(pCurrentScene->fhData);
				pcsdData->mfStartRetreat(CAvatar::mfGetPlayerAvatarHandle());
				
				ClrLock(pCurrentScene->fhData);
			}
		}
	}
}

/* ========================================================================
   Function    - CombatMenuPaint
   Description - Paint the combat menu for a given avatar.
   Returns     -
   ======================================================================== */
void CombatMenuPaint(LONG MenuId, LONG )
{
	LONG const MenuIndex = HIGH_LONG(MenuId);
	
	CONTROL_PANELS::mfPaintThisMenu(MenuIndex);
}


/* ========================================================================
   Function    - CombatHighAttack
   Description - For this Avatar set mode to the High Attack
   				 (Arcade Mode only)
   Returns     -
   ======================================================================== */
void CombatHighAttackKey(LONG menuid, LONG SwingFast)
{
	if (mouse_button == 2)
	{
		// Display help dialog on right click
		//CombatHelpDialog(0, COMBAT_STR_HELP_ATTACK1);
		mouse_click = 0;
		SystemHelp(COMBAT_STR_HELP_ATTACK_TITLE, COMBAT_STR_HELP_ATTACK1,H_Attack_Defend_Mode, 0,0);
	}
	else if ((mouse_click == 1 && mouse_button == 1) || mouse_click == 0)
	{
		mouse_click = 0;
		if (COMBAT_MODE::mfIsModeRPG() == TRUE && gRPGTurnGo == TRUE)
		{
			return;
		}
		
	    if (COMBAT_TARGETTING::mfIsModeTargetting() == TRUE )			// can't target two things.
	    {
	    	COMBAT_TARGETTING::mfStopTargeting();
	    }
		
		CONTROL_PANELS::mfMatchMenuToCurrent(HIGH_LONG(menuid));
		
		if (CONTROL_PANELS::mfCurrentInUse())
		{			
			CAvatar *const pAvatar = (CAvatar *const ) BLKPTR (CONTROL_PANELS::mfGetCurrAvatarHdl());
			if (!pAvatar->mfAmIImmoblized())
			{
				if (SCENE_MGR::hCurrentScene != fERROR)
				{
					SCENE_AI::STATE OldSceneState;
					SCENE_AI::SCENE_TYPE OldSceneType;
					
					SCENE * const pCurrentScene = (SCENE * const) BLKPTR(SCENE_MGR::hCurrentScene);
					pCurrentScene->mfGetSceneAIState(&OldSceneState,
									                 &OldSceneType);
					
					if (SwingFast ||
					    async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_QUICKATTACK_MODIFIER)) ||
					    async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_QUICKATTACK_MODIFIER2)))
					{
						pAvatar->mfSetAttackMode(FIGHT_SEQUENCE::ATM_QUICK_HIGH);
					}
					else
					{
						pAvatar->mfSetAttackMode(FIGHT_SEQUENCE::ATM_HIGH);
					}
					pAvatar->mfDecideWhoseTurnToAttack();
					if (pAvatar->mfIsMyTurnToAttack())
					{
						pAvatar->mfSetAttackSequence(pAvatar->mfGetAttackMode());
					}
					else
					{
						pAvatar->mfSetAttackSequence(FIGHT_SEQUENCE::ATM_DEFEND);
					}
					
					if (OldSceneType == SCENE_AI::COMBAT_SCENE)
					{
						if (pCurrentScene->fhData != fERROR)
						{
							DumbAutoLockPtr<COMBAT_SCENE_DATA> const pcsdData(pCurrentScene->fhData);
							pcsdData->mfInitCombatTimer();
						}
					}
					else
					{
						pCurrentScene->mfSetSceneType (SCENE_AI::COMBAT_SCENE);
						pCurrentScene->mfSetSceneState(SCENE_AI::INIT);
						pCurrentScene->mfCallSceneAI();
						CONTROL_PANELS::mfCurrentSpeakAcknowledgement();
					}
					
				}
			}
		}
	}
}

/* ========================================================================
   Function    - CombatLowAttack
   Description - For this Avatar set mode to the Low Attack
   				 (Arcade mode only.)
   Returns     -
   ======================================================================== */
void CombatLowAttackKey(LONG menuid, LONG SwingFast)
{
	if (mouse_button == 2)
	{
		// Display help dialog on right click
		// CombatHelpDialog(0, COMBAT_STR_HELP_ATTACK2);
		mouse_click = 0;
		SystemHelp(COMBAT_STR_HELP_ATTACK_TITLE, COMBAT_STR_HELP_ATTACK2, H_Attack_Defend_Mode, 0, 0);
	}
	else if ((mouse_click == 1 && mouse_button == 1) || mouse_click == 0)
	{
		mouse_click = 0;
		if (COMBAT_MODE::mfIsModeRPG() == TRUE && gRPGTurnGo == TRUE)
		{
			return;
		}
		
	    if (COMBAT_TARGETTING::mfIsModeTargetting() == TRUE )			// can't target two things.
	    {
	    	COMBAT_TARGETTING::mfStopTargeting();
	    }
		
		CONTROL_PANELS::mfMatchMenuToCurrent(HIGH_LONG(menuid));
		
		if (CONTROL_PANELS::mfCurrentInUse())
		{			
			CAvatar *const pAvatar = (CAvatar *const ) BLKPTR (CONTROL_PANELS::mfGetCurrAvatarHdl());
			
			if (!pAvatar->mfAmIImmoblized() &&
			    GAME_TTYPE::mfHasAttack2Animation(pAvatar->mfType()))
			{
				if (SCENE_MGR::hCurrentScene != fERROR)
				{
					SCENE_AI::STATE OldSceneState;
					SCENE_AI::SCENE_TYPE OldSceneType;
					
					SCENE * const pCurrentScene = (SCENE * const) BLKPTR(SCENE_MGR::hCurrentScene);
					pCurrentScene->mfGetSceneAIState(&OldSceneState,
									                 &OldSceneType);
					
					if (SwingFast ||
					    async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_QUICKATTACK_MODIFIER)) ||
					    async_key_status(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_QUICKATTACK_MODIFIER2)))
					{
						pAvatar->mfSetAttackMode(FIGHT_SEQUENCE::ATM_QUICK_LOW);
					}
					else
					{
						pAvatar->mfSetAttackMode(FIGHT_SEQUENCE::ATM_LOW);
					}
					pAvatar->mfDecideWhoseTurnToAttack();
					if (pAvatar->mfIsMyTurnToAttack())
					{
						pAvatar->mfSetAttackSequence(pAvatar->mfGetAttackMode());
					}
					else
					{
						pAvatar->mfSetAttackSequence(FIGHT_SEQUENCE::ATM_DEFEND);
					}
					
					if (OldSceneType == SCENE_AI::COMBAT_SCENE)
					{
						if (pCurrentScene->fhData != fERROR)
						{
							DumbAutoLockPtr<COMBAT_SCENE_DATA> const pcsdData(pCurrentScene->fhData);
							pcsdData->mfInitCombatTimer();
						}
					}
					else
					{
						pCurrentScene->mfSetSceneType (SCENE_AI::COMBAT_SCENE);
						pCurrentScene->mfSetSceneState(SCENE_AI::INIT);
						pCurrentScene->mfCallSceneAI();
						CONTROL_PANELS::mfCurrentSpeakAcknowledgement();
					}
				}
			}
		}
	}
}

/* ========================================================================
   Function    - CombatTurnDoneKey
   Description - Let the AI run a Turn. (RPG Mode only)
   Returns     -
   ======================================================================== */
void CombatTurnDoneKey(LONG /* unused */, LONG /* unused */ )
{
	// Only enabled if RPG mode is on.
	if (COMBAT_MODE::mfIsModeRPG() == FALSE)
		return;
		
	if (mouse_button == 2)
	{
		// Display help dialog on right click
		//CombatHelpDialog(0, COMBAT_STR_HELP_DONE);
		mouse_click = 0;
		SystemHelp(COMBAT_STR_HELP_DONE_TITLE, COMBAT_STR_HELP_DONE, H_Adventure_Done_Button,0,0);
	}
	else if ((mouse_click == 1 && mouse_button == 1) || mouse_click == 0)
	{
		mouse_click = 0;
		// click the button
		SetButtonHilight(D_COMBAT_RPGBOTTOM_BAR, BUTNO_RPG_DONE, TRUE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		SetButtonHilight(D_COMBAT_RPGBOTTOM_BAR, BUTNO_RPG_DONE, FALSE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		
		if ( gRPGTurnGo == TRUE)
	    {
			return;
		}
		
	    if (COMBAT_TARGETTING::mfIsModeTargetting() == TRUE )			// can't target two things.
	    {
	    	COMBAT_TARGETTING::mfStopTargeting();
	    }
		
		SCENE_AI::STATE SceneState;
		SCENE_AI::SCENE_TYPE SceneType;
		SCENE * const pCurrentScene = (SCENE * const) BLKPTR(SCENE_MGR::hCurrentScene);
		pCurrentScene->mfGetSceneAIState(&SceneState,
						                 &SceneType);
		
		if (SceneType != SCENE_AI::COMBAT_SCENE)
		{
			return;
		}
					
			
		LONG i;
		
		for (i = 0; i < MAX_AVATARS; i++)
		{
			if (pCurrentScene->Avatars[i] != fERROR)
			{
				//GEH SetBlockAttr(pCurrentScene->Avatars[i], LOCKED, LOCKED);
				CAvatar * const pAvatar = (CAvatar * const) BLKPTR(pCurrentScene->Avatars[i]);
				pAvatar->DoAI(CAvatar::AI_END_PAUSE);
				
				if (pAvatar->Status == CAvatar::AI_ATTACK ||
				    pAvatar->Status == CAvatar::AI_CASTSPELL)
				{
					pAvatar->mfSetDamageFlag();
				}
				
				// Just incase he died.
				//GEH if (pCurrentScene->Avatars[i] != fERROR)
				//GEH 	ClrLock(pCurrentScene->Avatars[i]);
				
			}
		}
		
		gRPGTurnGo = TRUE;
		
		if (pCurrentScene->fhData != fERROR)
		{
			DumbAutoLockPtr<COMBAT_SCENE_DATA> const pcsdData(pCurrentScene->fhData);
			pcsdData->mfInitRPGTimer();
		}
	}
}

/* ========================================================================
   Function    - CombatNewLeaderKey
   Description - Put a new avatar in the lead of the group.
   				 Shouldn't be able to put dead guys in front, or 
   				 swap places during actual combat.
   Returns     -
   ======================================================================== */
void CombatNewLeaderKey(LONG , LONG /* unused */)
{
	if (mouse_button == 2)
	{
		// Display help dialog on right click
		//CombatHelpDialog(0, COMBAT_STR_HELP_LEADER);
		mouse_click = 0;
		SystemHelp(COMBAT_STR_HELP_LEADER_TITLE, COMBAT_STR_HELP_LEADER, H_Change_Leader,0,0);
	}
	else if ((mouse_click == 1 && mouse_button == 1) || mouse_click == 0)
	{
		mouse_click = 0;
		// click the button
		SetButtonHilight(D_COMBAT_RPGBOTTOM_BAR, BUTNO_RPG_LEADER, TRUE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		SetButtonHilight(D_COMBAT_RPGBOTTOM_BAR, BUTNO_RPG_LEADER, FALSE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		
		SCENE_AI::STATE SceneState;
		SCENE_AI::SCENE_TYPE SceneType;
		SCENE * const pCurrentScene = (SCENE * const) BLKPTR(SCENE_MGR::hCurrentScene);
		pCurrentScene->mfGetSceneAIState(&SceneState,
						                 &SceneType);
		
		if ( !CONTROL_PANELS::mfIsCurrentLeader() && 
			  CONTROL_PANELS::mfCurrentInUse() &&
			  SceneType != SCENE_AI::COMBAT_SCENE
			  )
		{
			CAvatar *const pAvatar = (CAvatar *const ) BLKPTR (CONTROL_PANELS::mfGetCurrAvatarHdl());
			if (!pAvatar->mfAmIImmoblized())
			{
				SHORT const hOldLeader = CAvatar::mfGetPlayerAvatarHandle();
				
				if (hOldLeader != fERROR)
				{
					CAvatar * const pOldLeader = (CAvatar * const) BLKPTR(hOldLeader);
					pAvatar->fFollowPlayer.mfSwapPlaces(pOldLeader->fFollowPlayer);
				}
				
				pAvatar->mfSetMeToThePlayer();
				
				// Now swap the menus.
				CONTROL_PANELS::mfSetCurrentToLeader();
			}
		}
	}
}

/* ========================================================================
   Function    - TestAndPutLiveOneInFront
   Description - Search the list of panels and put a live one in front.
   				 Move the "player" to that new position.
   				 Swap the Adventurer's positions in the bread crumb trail.
   Returns     - 
   ======================================================================== */

void static TestAndPutLiveOneInFront()
{
	ADVENTURER_TEAM_ITOR AdvItor;
	
	AdvItor = ADVENTURER::begin();
	
	CAvatar * const pFirstAvatar = (CAvatar * const) BLKPTR(*AdvItor);
	
	if (pFirstAvatar->mfAmIImmoblized())
	{
		for (AdvItor++;
		     AdvItor != ADVENTURER::end();
		     AdvItor++)
		{
			CAvatar * const pNewLeaderCanidate = (CAvatar * const) BLKPTR(*AdvItor);
			if (!pNewLeaderCanidate->mfAmIImmoblized())
			{
				if (gCombatMenuIsUp == TRUE)
				{
					CONTROL_PANELS::mfMatchAvatarHdlToCurrent(*AdvItor);
					CONTROL_PANELS::mfSetCurrentToLeader();
				}
				pNewLeaderCanidate->mfSetMeToThePlayer();
				ADVENTURER::mfSetWhoGetsObjectsPickedUp(*AdvItor);
				
				// Move the "player" back to the new leader.
				SetPlayer(pNewLeaderCanidate->mfX(),
				          pNewLeaderCanidate->mfY(),
				          pNewLeaderCanidate->mfZ(),
				          pNewLeaderCanidate->mfAngle(),
				          0);	// Avatars have no pitch.
				
				// Re-adjust the bread crumb trail.
				pFirstAvatar->fFollowPlayer.mfSwapPlaces(pNewLeaderCanidate->fFollowPlayer);
				break;
			}
		}
	}
}


#if (0)
/* ========================================================================
   Function    - CombatEndHelpProc
   Description -
   Returns     -
   ======================================================================== */
static void CombatEndHelpProc(LONG MenuCombo, LONG )
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
}

/* ========================================================================
   Function    - CombatHelpDialog
   Description -
   Returns     -
   ======================================================================== */
static void CombatHelpDialog(LONG, LONG StrMgrText)
{
	// set the proc to help paint
	SetButtonProc  (D_QUESTION2, 0, CombatHelpPaint, StrMgrText, 0 );
	
	// set the question text off
	SetButtonLabel  (D_QUESTION2, QUESTION_TEXT, -1, BLACK );
	
	// turn off the first button
	SetButtonLabel  (D_QUESTION2, QUESTION_BTN1, -1, BTN_LABEL_COLOR );
	SetButtonHilight(D_QUESTION2, QUESTION_BTN1, FALSE );
	
	// turn on the second button as CANCEL
	SetButtonLabel  (D_QUESTION2, QUESTION_BTN2, STR_CANCEL, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION2, QUESTION_BTN2, CombatEndHelpProc, 0, D_KEY_CANCEL );
	
	ShowMenu(D_QUESTION2);
}

/* ========================================================================
   Function    - CombatHelpPaint
   Description - paint the help text
   Returns     -
   ======================================================================== */
static void CombatHelpPaint (LONG MenuCombo, LONG StrMgrText)
{
	LONG	X,Y,W,H;
	LONG	MenuId, ButtonId;
	CHAR	buffer[256];
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	if( GetButtonPosition( MenuId, 1, &X, &Y ) == fERROR)
		return;
		
	if( GetButtonSize( MenuId, 1, &W, &H ) == fERROR)
		return;
		
 	// paint dialog title
	sprintf(buffer, "^F03^W%03d%s",W,STRMGR_GetStr(StrMgrText));
	gprint_text(X,Y,buffer, BLACK);
	

}

#endif // UNUSED
/* ========================================================================
   Function    - CombatToggleMapKey
   Description - Cycle the adventure map.
   Returns     - 
   ======================================================================== */

void CombatToggleMapKey(LONG , LONG )
{
	if (mouse_button == 2)
	{
		// Display help dialog on right click
		//CombatHelpDialog(0, COMBAT_STR_HELP_MAP);
		mouse_click = 0;
		SystemHelp(COMBAT_STR_HELP_MAP_TITLE, COMBAT_STR_HELP_MAP, H_Adventure_Map, 0, 0);
	}
	else if ((mouse_click == 1 && mouse_button == 1) || mouse_click == 0)
	{
		mouse_click = 0;
		// click the button
		SetButtonHilight(D_COMBAT_RPGBOTTOM_BAR, BUTNO_RPG_MAP, TRUE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		SetButtonHilight(D_COMBAT_RPGBOTTOM_BAR, BUTNO_RPG_MAP, FALSE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		
		SetMapDraw(0,TOGGLE);
	}
}

/* ========================================================================
   Function    - CombatToggleViewKey
   Description - Cycle between following behind the leader and being the leader
   Returns     - 
   ======================================================================== */

void CombatToggleViewKey(LONG , LONG )
{
	if (mouse_button == 2)
	{
		// Display help dialog on right click
		//CombatHelpDialog(0, COMBAT_STR_HELP_VIEW);
		mouse_click = 0;
		SystemHelp(COMBAT_STR_HELP_VIEW_TITLE, COMBAT_STR_HELP_VIEW, H_Change_View, 0, 0);
	}
	else if ((mouse_click == 1 && mouse_button == 1) || mouse_click == 0)
	{
		mouse_click = 0;
		// click the button
		SetButtonHilight(D_COMBAT_RPGBOTTOM_BAR, BUTNO_RPG_VIEW, TRUE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		SetButtonHilight(D_COMBAT_RPGBOTTOM_BAR, BUTNO_RPG_VIEW, FALSE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		
		SCENE_MGR::mfToggleFollowCamera(0,0);
	}
}

#if defined (_EDIT)
// C hooks to call IncreaseAdventureScreen and DecreaseAdventureScreen.
extern "C" {
CCallIncreaseAdventureScreen (LONG, LONG)
{
	IncreaseAdventureScreen (0, 0);
}

CCallDecreaseAdventureScreen (LONG, LONG)
{
	DecreaseAdventureScreen (0, 0);
}
}


#endif
/* ========================================================================
   Function    - AdventureFixUnitsArray
   Description - Remove dead units and send them back to thier provinces.
   Returns     - 
   ======================================================================== */

typedef struct {
	LONG	iUnit;
	PROVINCE	currProv;
} ADVENTURE_UNIT_STACK;

void AdventureFixUnitsArray(LONG iUnitIndex)
{
	ADVENTURE_UNIT_STACK	AdventureUnitStack[4] = 
	                                       { {-1, NO_PROVINCE},
	                                         {-1, NO_PROVINCE},
	                                         {-1, NO_PROVINCE},
	                                         {-1, NO_PROVINCE}};
	LONG	Counter = 0;
	LONG	i;
	LONG	thisUnit;
	
	CAvatar *pAvatar;
	BOOL	fSend = FALSE;
	
	#if defined(_WINDOWS)
	fSend = IsMultiPlayer();
	#endif
	
	// check the home team
	do
	{
		if(units[iUnitIndex].iHandle != fERROR)
		{
			pAvatar = (CAvatar *) BLKPTR(units[iUnitIndex].iHandle);
			
			thisUnit = iUnitIndex;
			PROVINCE const thisProv = (PROVINCE)units[iUnitIndex].province;
			
			// THIS IS HACK WHICH RELIES ON THE FACT THAT PROVINCE DATA ISN'T
			// PERCOLATED DOWN THE UNIT STACK! (ICK!)
			// Province will be changed after calling MakeUnitSingleStack.
			PROVINCE NextUnitsProv = NO_PROVINCE;
			BOOL ResetNextUnitProv = FALSE;
			
			if (units[iUnitIndex].NextUnit != fERROR)
			{
				NextUnitsProv = (PROVINCE) units[units[iUnitIndex].NextUnit].province;
				ResetNextUnitProv = TRUE;
			}
			
			
			if (SCENE_MGR::mfIsSceneRequested())
			{
				// if I'm dead remove me
				if ( !IsRegent(pAvatar->UnitIndex) &&
						pAvatar->mfAmIImmoblized())
				{
					iUnitIndex = DeleteUnit(thisUnit, fSend, FALSE);
					
					if (ResetNextUnitProv)
					{
						// Keep your original province data.
						SetGameData(MP_UNITS, MPUNITS_PROVINCE, iUnitIndex, NextUnitsProv, fSend);
					}
				}
				else
				{
					iUnitIndex = units[iUnitIndex].NextUnit;
				}
			}
			else
			{
				// First seperate out this unit from the stack
				iUnitIndex = MakeUnitSingleStack(thisUnit, fSend);
				if (ResetNextUnitProv && iUnitIndex != fERROR)
				{
					SetGameData(MP_UNITS, MPUNITS_PROVINCE, iUnitIndex, NextUnitsProv, fSend);
				}
				
				// if I'm dead remove me
				if ( !IsRegent(pAvatar->UnitIndex) &&
						pAvatar->mfAmIImmoblized() )
				{
					DeleteUnit(thisUnit, fSend, FALSE);
				}
				else
				{
					// Save these guys to join them up later.
					AdventureUnitStack[Counter].iUnit = thisUnit;
					AdventureUnitStack[Counter].currProv = thisProv;
					++Counter;
				}
			}
		}
		
	} while (iUnitIndex != fERROR);
	
	for (i = 0; i < Counter; i++)
	{
		thisUnit = AdventureUnitStack[i].iUnit;
		if (thisUnit > 0)
		{
			// set the province so we won't join ourselves later.
			units[thisUnit].province = NO_PROVINCE;
		}
	}
	// Now that we've broken them up into individual stacks, rejoin them.
	for (i = 0; i < Counter; i++)
	{
		thisUnit = AdventureUnitStack[i].iUnit;
		if (thisUnit > 0)
		{
			// Reset the province back to what it was.
			JoinUnitToOthersInProvince(thisUnit, AdventureUnitStack[i].currProv, fSend);
		}
	}
}
