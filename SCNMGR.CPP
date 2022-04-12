/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: SCENEMGR.cpp  -
   Author:   Gary Powell

   ========================================================================

   Contains the following general functions:


   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <string.h>

#ifdef _WINDOWS
#include <windows.h>
#endif

#include "avatar.hxx"
#include "combatui.hxx"
#include "scnmgr.hxx"
#include "battleui.hxx"
#include "playstat.hxx"
#include "scene.hxx"
#include "units.hxx"
#include "machine.h"
#include "doors.h"
#include "light.h"
#include "game.h"
#include "multimap.hxx"
#include "multiui.hxx"
#include "regents.hxx"
#include "gamemap.hxx"
#include "task.h"

#ifdef _WINDOWS
#include "winsys\mulplay.hxx"
#endif

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

void DisplayInfo (CSTRPTR szFrame, BOOL fCentered, int iformat, ...);
static BOOL LoadSceneAvatars( SCENE * const, SHORT /* UnitIndex */ );
void InitDomainTurn(LONG);
BOOL LookMessages(void);
extern BOOL gfWadLoaded;
void PurgeLevel(void);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
HDL_SCENE SCENE_MGR::hCurrentScene = -1; // Handle to current Scene
BOOL SCENE_MGR::fScenePaused = FALSE; 	// Flag to freeze the game.
ULONG SCENE_MGR::gTick = 0;
ULONG SCENE_MGR::gFrame = 0;

BOOL SCENE_MGR::fNewSceneRequested = FALSE;
LONG SCENE_MGR::fNewScenePlayerStart = TYPE_PLAYERSTART1;
char SCENE_MGR::fcpNewSceneName[20] = { "" };

GAME_CAMERA SCENE_MGR::fgCamera;
BOOL SCENE_MGR::fbRender = TRUE;
SCENE_MGR::FADE_STATE SCENE_MGR::fFadeToNextScene = SCENE_MGR::FADE_IN;

REALM::REALM_TYPE		SCENE_MGR::Aggressor = REALM::NO_COUNTRY;
REALM::REALM_TYPE		SCENE_MGR::Visitors = REALM::NO_COUNTRY;
SHORT					SCENE_MGR::HomeIndex = -1;	// who is the home unit (w/camera)
SHORT					SCENE_MGR::AwayIndex = -1;	// who is the away unit
SHORT					SCENE_MGR::Victor = 0;	// who wins
SHORT					SCENE_MGR::Loser = 0;	// who loses
SHORT					SCENE_MGR::VictorRealm = 0;// [d1-09-97 JPC] Remember realm of winner
SHORT					SCENE_MGR::LoserRealm = 0;	// [d1-09-97 JPC] Remember realm of loser
SHORT					SCENE_MGR::PlacesIndex = 0;	// Where we are
SBYTE					SCENE_MGR::TerrainSubType = -1;	// battle field sub type
SCENE_AI::SCENE_TYPE	SCENE_MGR::SceneType = SCENE_AI::NO_SCENE;	// type for next scene ai

// GEH these are gregs debug stuff
//REALM::REALM_TYPE	SCENE_MGR::Aggressor = REALM::AERENWE;
//SHORT		SCENE_MGR::HomeIndex = 0;	// who is the home unit (w/camera)
//SHORT		SCENE_MGR::AwayIndex = 1;	// who is the away unit

LONG		SCENE_MGR::MachineIds = 10000;	// current availible avatar id


extern BOOL fQuietItems;

	
/* ========================================================================
   Function    - GetNewId
   Description -
   Returns     -
   ======================================================================== */
LONG const GetNewId()
{
	return(SCENE_MGR::MachineIds++);
}

/* ========================================================================
   Function    - mfToggleScenePause
   Description - Change the game running state.
   Returns     - void
   ======================================================================== */
void SCENE_MGR::mfToggleScenePause(LONG, LONG)
{
	mfSetScenePause(((fScenePaused == TRUE) ? FALSE : TRUE));
}

/* ========================================================================
   Function    - mfToggleFollowCamera
   Description - Change Whether the camera is behind the player or is the player.
   Returns     - void
   ======================================================================== */
void SCENE_MGR::mfToggleFollowCamera(LONG, LONG)
{
	if (fgCamera.mfGetCurrentState() == GAME_CAMERA::FOLLOW_PLAYER)
	{
		fgCamera.mfSetCameraState(GAME_CAMERA::FOLLOW_BEHIND_PLAYER);
	}
	else
	{
		fgCamera.mfSetCameraState(GAME_CAMERA::FOLLOW_PLAYER);
	}
}

/* ========================================================================
   Function    - mfSetScenePause
   Description - Set the game state
   Returns     - void
   ======================================================================== */
void SCENE_MGR::mfSetScenePause(BOOL const bState)
{
	LONG i;

	// Also stops the game clock from running.
	fScenePaused = bState;

	if (SCENE_MGR::hCurrentScene != fERROR)
	{
		//GEH SetBlockAttr(SCENE_MGR::hCurrentScene, LOCKED, LOCKED);
		PTR_SCENE pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);

		// Pause the scene so that the music keeps playing but the camera movement
		// stops and the avatars freeze in place.
		// (Flames & fires too)
		
		if (fScenePaused)
		{
			pCurrentScene->mfSetSceneState(SCENE_AI::PAUSED);
			//PlayEnvironmentalSounds = FALSE;
			// For the momemt send all the AI's into a Paused state
			for (i = 0; i < MAX_AVATARS; i++)
			{
				if (pCurrentScene->Avatars[i] != fERROR)
				{
					//GEH SetBlockAttr(pCurrentScene->Avatars[i], LOCKED, LOCKED);
					CAvatar *pAvatar = (CAvatar *) BLKPTR(pCurrentScene->Avatars[i]);
					pAvatar->DoAI(CAvatar::AI_BEGIN_PAUSE);
					
					// Just incase he died.
					//GEH if (pCurrentScene->Avatars[i] != fERROR)
						//GEH ClrLock(pCurrentScene->Avatars[i]);
					
				}
			}
		}
		else
		{
			pCurrentScene->mfSetSceneState(SCENE_AI::PLAY);
			//PlayEnvironmentalSounds = TRUE;
			// For the momemt send all the AI's into a Paused state
			for (i = 0; i < MAX_AVATARS; i++)
			{
				if (pCurrentScene->Avatars[i] != fERROR)
				{
					//GEH SetBlockAttr(pCurrentScene->Avatars[i], LOCKED, LOCKED);
					CAvatar *pAvatar = (CAvatar *) BLKPTR(pCurrentScene->Avatars[i]);
					pAvatar->DoAI(CAvatar::AI_END_PAUSE);
					
					// Just incase he died.
					//GEH if (pCurrentScene->Avatars[i] != fERROR)
						//GEH ClrLock(pCurrentScene->Avatars[i]);
					
				}
			}
		}
		
		//GEh ClrLock(hCurrentScene);
	}
}
/* ========================================================================
   Function    - mfLoadScene
   Description - Load the current scene.
   Returns     - void
   ======================================================================== */
#define FILENAME_LEN	8
LONG const SCENE_MGR::mfLoadScene(
	char const * const cpNewSceneName,
	LONG   const playerStartLocation)
{
	PTR_SCENE	pCurrentScene;
	BOOL		bFirstTime = FALSE;
	CHAR		SceneName[FILENAME_LEN + 1];
	CHAR		btl_char;
	
	SuspendMusic();
	// Check for first time thru this.
	if (hCurrentScene == -1)
	{
		SCENE_MGR::hCurrentScene = NewLockedBlock(sizeof(SCENE));
		if (SCENE_MGR::hCurrentScene == fERROR)
		{
			goto Error;
		}
		else
		{
			//GEH SetBlockAttr(SCENE_MGR::hCurrentScene, LOCKED, LOCKED);
			
			bFirstTime = TRUE;
			pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
			pCurrentScene->mfInitializeData();
		}
	}
	else
	{
		//GEH SetBlockAttr(SCENE_MGR::hCurrentScene, LOCKED, LOCKED);
		
		pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
		pCurrentScene->mfSetSceneState(SCENE_AI::RELEASE);
		pCurrentScene->mfCallSceneAI();
	}
	
	// Reset any exit level conditions.
	fExitLevel = 0;
	
	// make a local copy of new scene name incase we wish to modify it
	strncpy(SceneName, cpNewSceneName, FILENAME_LEN);
	SceneName[FILENAME_LEN] = 0;
	
	// set default scene type
	pCurrentScene->mfSetSceneType( SCENE_MGR::SceneType );
	
	/* -----------------------------------------------------------------
	   Put any scene loading logic in the following section
	   ----------------------------------------------------------------- */

	// is this a request for a battle field scene, if so and
	// HomeIndex is the defender ask the user which of 4 does
	// he wish to defend from.
	if ((strncmp(SceneName, "batl", 4) == 0))
	{
		SCENE_MGR::Victor = 0;
		
		if(SCENE_MGR::HomeIndex == -1)
			goto Error;
			
		if(SCENE_MGR::AwayIndex == -1)
			goto Error;
			
		// clear the exceptions table.  Must do this here because
		// if there isn't a [Exceptions] entry in the file, it
		// won't clear the list
		memset(BtlExceptions, 0, sizeof(SHORT) * GRID_MAX_ROWS * GRID_MAX_COLS);


#ifdef _WINDOWS

		if ( IsMultiPlayer() )
		{

			// is the home team the defenders?

			if( units[SCENE_MGR::HomeIndex].Realm != SCENE_MGR::Aggressor )
			{
				// let the player decide
				btl_char = BattleChooseField(SceneName[4]);
				SceneName[5] = btl_char;
				if ( IsMulPlayerandRealm(units[SCENE_MGR::AwayIndex].Realm) )
				{
					SetGameData(MP_GAMEDATA, MPGAMESEMIPHORE_BATTLE_START, 0, (LONG)btl_char, TRUE );
					// -- WaitForSemiphore(MPGAMESEMIPHORE_BATTLE_START1);
					StartSync(iMPS_SYNC8);
					CheckSync(iMPS_SYNC8, TRUE, FALSE);
					DisplayInfo("UI\\DINFO_BK.PCX",1,STR_GM_ENTER_BATTLE);
					update_screen();
				}
			}
			else
			{

				if ( IsMulPlayerandRealm(units[SCENE_MGR::AwayIndex].Realm) )
				{
					SetGameData(MP_GAMEDATA, MPGAMESEMIPHORE_BATTLE_START1, 0, 1, TRUE );

					// put up a waiting for player message
					DisplayInfo("UI\\DINF3_BK.PCX",1,STR_GM_PLAYER_WAIT);
					update_screen();
					StartSync(iMPS_SYNC8);
					CheckSync(iMPS_SYNC8, TRUE, FALSE);
					btl_char = DoGetGameData(MP_GAMEDATA, MPGAMESEMIPHORE_BATTLE_START, 0 );
					// -- btl_char = WaitForSemiphore(MPGAMESEMIPHORE_BATTLE_START);
					DisplayInfo("UI\\DINFO_BK.PCX",1,STR_GM_ENTER_BATTLE);
					update_screen();
					SceneName[5] = btl_char;
				}
				else
				{
					// if someone hasn't already declaired which sub type
					if ((SBYTE)SCENE_MGR::TerrainSubType != -1)
					{
						SceneName[5] = SCENE_MGR::TerrainSubType + '0';
					}
					else
					{
						// let the comuter decide
						SceneName[5] = (CHAR)(random(4)+'0');
					}
				}
			}


		}
		else    //---- Let non multiplayer do what it wants
#endif
		{
			// if someone has already declaired which sub type
			if ((SBYTE)SCENE_MGR::TerrainSubType != -1)
			{
				SceneName[5] = SCENE_MGR::TerrainSubType + '0';
			}
			else
			// is the home team the defenders?
			if( units[SCENE_MGR::HomeIndex].Realm != SCENE_MGR::Aggressor )
			{
				// let the player decide
				btl_char = BattleChooseField(SceneName[4]);
				SceneName[5] = btl_char;
			}
			else
			// let the comuter decide
			{
				SceneName[5] = (CHAR)(random(4)+'0');
			}
		}


		// over-ride scene type
		pCurrentScene->mfSetSceneType( SCENE_AI::BATTLE_SCENE );
	}
	
	/* -----------------------------------------------------------------
	   End loading logic section
	   ----------------------------------------------------------------- */

#ifdef _WINDOWS
	LookMessages();
#endif

	/* -----------------------------------------------------------------
	   put up the "loading" box
	   ----------------------------------------------------------------- */
	if (fShowProgressBar)
	{
		SetLoadingProgress(0);
		ShowMenu(D_LOADING_SCENE);
		RunMenus();
	}
	
	// [d11-15-96 JPC]
	// Setup the scene ai function.  If this is the WAD loaded, then
	// wait for a keypress before going on.  If this is a WAD-to-WAD
	// teleport, go directly to the init state.
	pCurrentScene->mfSetSceneState(SCENE_AI::INIT);
	
	if (fShowProgressBar)
	{
		SetLoadingProgress(10);
		RunMenus();
	}
	// set initial engine state
	fRender = TRUE;
	fFreeze = FALSE;
	fPause = FALSE;
	
	// Reset this after running the release scene AI, so we can tell whether this
	// is a scene to scene teleport or a not when we release the scene ai.
	SCENE_MGR::fNewSceneRequested = FALSE;
	
	// load the new scene
	pCurrentScene->mfLoadScene(SceneName, playerStartLocation);
	
#ifdef _WINDOWS
	LookMessages();
#endif

	// now add the game unit avatars to the scene
	if(SCENE_MGR::HomeIndex != -1)
	{
		if(!LoadSceneAvatars( pCurrentScene, SCENE_MGR::HomeIndex))
			goto Error;
	}
	
	if(SCENE_MGR::AwayIndex != -1)
	{
		if(!LoadSceneAvatars( pCurrentScene, SCENE_MGR::AwayIndex))
			goto Error;
	}
	
	if (fShowProgressBar)
	{
		SetLoadingProgress(95);
		RunMenus();
	}
	
#ifdef _WINDOWS
	LookMessages();
#endif

	// Toss all pending key strokes
	clear_key_status(0);
	
	// Initialize the camera to the player's location.
	SetCameraPosition(
		&camera,
		PLAYER_INT_VAL(player.x),
		PLAYER_INT_VAL(player.y),
		(player.z+player.h),
		player.a,
		player.p
		);
	
	if (fShowProgressBar)
	{
		SetLoadingProgress(100);
		RunMenus();
	}
	
	// Execute the init state
	pCurrentScene->mfCallSceneAI ();

	// always clear this for the next guy.  If he doesn't specify, dont show
	fShowProgressBar = FALSE;
	
	/* -----------------------------------------------------------------
	   drop the "loading" menu
	   ----------------------------------------------------------------- */
	HideMenu(D_LOADING_SCENE);
	RunMenus();

	// Now set either the WAITFORKEY state or the PLAY state, depending
	// on whether this is the first time.
	if (bFirstTime)
		pCurrentScene->mfSetSceneState(SCENE_AI::WAITFORKEY);
	else
		pCurrentScene->mfSetSceneState(SCENE_AI::PLAY);

	// Execute the PLAY state or the WAITFORKEY state, as the case may be.
	// This saves 1 render pass (no big deal, really).
	pCurrentScene->mfCallSceneAI();

#if 0
// [d11-15-96 JPC] Add a DONE button to the adventure description.
// The following code gets moved to SCENE_AI::mfSceneFadeOut.
	// Execute the init state
	pCurrentScene->mfCallSceneAI();

	pCurrentScene->mfSetScenePallet();
	if (SCENE_MGR::fFadeToNextScene == SCENE_MGR::FADE_OUT)
	{
		FadeOut(500);
		SCENE_MGR::fFadeToNextScene = SCENE_MGR::FADE_IN;
	}

	// [d11-09-96 JPC] Clear out the background buffer to solve a number
	// of bugs with the main map showing through.  Note that fRealmTurn
	// is FALSE at this point.
	clear_screen ();
	update_screen ();
#endif

	// [d8-08-96 JPC] Move these here from MAIN: GameMain.  These tasks should
	// only run if a non-battle scene is active.
	// They are removed in LEVEL: PurgeLevel.
	if ((strncmp(SceneName, "batl", 4) != 0))
	{
		add_task(1,handle_doors,0);  	//once every frame see if any doors moving
		add_task(1,HandleLifts,0);  	//once every frame see if any lifts moving
		add_task(1,HandleFloors,0);  	//once every frame see if any floors moving
		add_task(1,HandleCeilings,0); //once every frame see if any ceilings moving
		add_task(1,TextureFrameHandler,0);  //once every frame check lighting effects
		// no longer used: add_task(10,SectorFrameHandler,0);  //once every 10 frames check sector effects
	}
	ResumeSuspendedMusic();
	//GEH ClrLock(SCENE_MGR::hCurrentScene);
	return fNOERR;
	
Error:
	return fERROR;
}

/* ========================================================================
   Function    - LoadSceneAvatar
   Description - given a unit index, load him and his followers
   Returns     -
   ======================================================================== */
static BOOL LoadSceneAvatars( SCENE * const pScene, SHORT UnitIndex )
{
	SHORT					i=0;
	const REALM::REALM_TYPE	Realm = (REALM::REALM_TYPE) units[UnitIndex].Realm;
	// CHAR					buffer[24];
	
	do
	{
		while( i < MAX_AVATARS && pScene->Avatars[i] != fERROR)
		{
			i++;
		}
		
		if (i < MAX_AVATARS)
		{
			pScene->Avatars[i] = CAvatar::NewAvatar();
			
			if (pScene->Avatars[i] != fERROR)
			{
				//GEH SetBlockAttr(pScene->Avatars[i], LOCKED, LOCKED);
				CAvatar *pAvatar = (CAvatar *) BLKPTR(pScene->Avatars[i]);
				pAvatar->UnitIndex = UnitIndex;
				pAvatar->Id = units[UnitIndex].id;
				if (fERROR != (pAvatar->hPlayerStats = LoadStats((LONG)units[UnitIndex].id,pScene->Avatars[i])))
				{
					PLAYER_STATS *pPlayerStats;
					pPlayerStats = (PLAYER_STATS*)BLKPTR(pAvatar->hPlayerStats);
					pAvatar->ThingType = pPlayerStats->mfGetType();
				}
				else
				{
					printf("WARNING! Unable to load Avatar %d\n", (LONG)units[UnitIndex].id);
					// We call this instead of DeleteAvatar because it isn't in the
					// things table yet.
					DisposBlock(pScene->Avatars[i]);
					pScene->Avatars[i] = fERROR;
					i--;
					goto Error;
				}
				
				// GEH all avatars loaded with this routine are generic
				pAvatar->mfDefineCThing(pAvatar->mfType(), 0, 0, 0, 0, 0, 0,0);
				pAvatar->attrib.SceneInstance = TRUE;
				// Real guys id's start at 1000
				if( units[UnitIndex].id >= 1000)
					pAvatar->Id = (LONG)units[UnitIndex].id;
				else
					pAvatar->Id = GetNewId();
				pAvatar->Realm.HomeRealm = Realm;
				pAvatar->mfSetOriginalRemapColor(Realm);
				
				units[UnitIndex].iHandle = pScene->Avatars[i];
				//GEH ClrLock(pScene->Avatars[i]);
			}
		}
		else
		{
			// Keep reading them from the scene file so we can read
			// the other data sections.
			printf("WARNING! Exceeded number of avatars!.");
			goto Error;
		}
		
		UnitIndex = (SHORT) units[UnitIndex].NextUnit;
	} while(UnitIndex != -1);
	
	return TRUE;
	
Error:
	return FALSE;	
}
/* ========================================================================
   Function    - mfReleaseSceneToMap
   Description - Cleanup the scene and return to the map level
   Returns     -
   ======================================================================== */
void SCENE_MGR::mfReleaseSceneToMap(LONG,LONG)
{

#if defined (_EDIT)
	return;										// [d8-09-96 JPC] can't get to map
#endif											// from editor.
	PlayEnvironmentalSounds = FALSE;
	SuspendMusic();
	if (hCurrentScene != fERROR)
	{
		//GEH SetBlockAttr(SCENE_MGR::hCurrentScene, LOCKED, LOCKED);
		PTR_SCENE pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
		
		// GWP You must deactivate before releasing the avatar memory.
		fQuietItems=TRUE;	
		ItemEffects::DeactivateAllItems();
		fQuietItems=FALSE;
			
		HideSubMenu(0,D_AVATAR_INVENTORY);
		HideSubMenu(0,D_AVATAR_ATTRIBUTES);
		
		pCurrentScene->mfSetSceneState(SCENE_AI::RELEASE);
		pCurrentScene->mfCallSceneAI();
		pCurrentScene->mfDeleteScene();
		
		ClrLock(hCurrentScene);
		
		DisposBlock(hCurrentScene);
		hCurrentScene = fERROR;

		PurgeLevel();
	}
	
	// The map must run in high res mode.
	set_hires(0,0);
	KillSoundsNoFade();

	// return to the map
	InitDomainTurn(0);
	ResumeSuspendedMusic();
}

/* ========================================================================
   Function    - mfDoAI
   Description - This gets called every render. It decides whether to
                 load a new scene, or freeze the scene in place.
   Returns     - void
   ======================================================================== */

void SCENE_MGR::mfDoAI()
{
	if(!fPause)
	{
	StartSceneAI:
		// Note: Many places in the game can request a new scene to be loaded.
		if (SCENE_MGR::fNewSceneRequested)
		{
			if(fERROR == SCENE_MGR::mfLoadScene(SCENE_MGR::fcpNewSceneName, SCENE_MGR::fNewScenePlayerStart))
				SCENE_MGR::mfReleaseSceneToMap(0,0);
			
			// Reset this after loading the scene, so we can tell whether this
			// is a scene to scene teleport or a not when we release the scene ai.
			SCENE_MGR::fNewSceneRequested = FALSE;
		}
	
		// In case the wad was loaded directly
		if (SCENE_MGR::hCurrentScene != fERROR)
		{
			//GEH SetBlockAttr(SCENE_MGR::hCurrentScene, LOCKED, LOCKED);
			PTR_SCENE pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
	
			if (fExitLevel != 0)
			{
			    // Note: Request is called instead of LoadScene so that we get a copy
			    //       of the new scene name. Because it will be tossed when the old
			    //       scene is purged.
			    char * cpNewSceneName;
			
			    if (pCurrentScene->mfGetExitScene(fExitLevel, &cpNewSceneName))
			    {
				    SCENE_MGR::mfRequestNewScene(cpNewSceneName, fExitLevel, FALSE);
				    // Major hack but put here for SPEED! (Don't wait another render frame just go!)
					fExitLevel = 0;
				    goto StartSceneAI;
			    }
			
			    // Couldn't leave. Sorry.
			    fExitLevel = 0;
				pCurrentScene->mfCallSceneAI();
				
				//GEH ClrLock(SCENE_MGR::hCurrentScene);
			
			}
			else
			{
				SCENE_AI::STATE SceneState;
				SCENE_AI::SCENE_TYPE SceneType;
			
				pCurrentScene->mfCallSceneAI();
				pCurrentScene->mfGetSceneAIState(&SceneState,
								                 &SceneType);
				// If we need to release do it now. Don't wait and get more
				// mouse clicks or Keyboard.
				if (SCENE_AI::RELEASE == SceneState)
				{
					goto StartSceneAI;
				}
				
				//GEH ClrLock(SCENE_MGR::hCurrentScene);
			}
		}
		else
		{
			if (gfWadLoaded == FALSE)
			{
				fatal_error("ERROR: scnmgr.cpp No Wad loaded.\n");
			}
			else
			{
				MovePlayer(CheckMoveKeys());
				fgCamera.mfUpdateCamera();
			}
		}
	}
	
}
/* ========================================================================
   Function    - mfRequestNewScene
   Description - Take a new scene request and save the necessary information.
                 Note: This is called instead of loading the scene directly so
                       that requests can be made by call back fns from within
                       the render loop.
   Returns     - void
   ======================================================================== */
void SCENE_MGR::mfRequestNewScene(char * cpSceneFile,
	                                    LONG   playerStartLocation,
	                                    BOOL   FadeToNextScene)
{
	// GWP HACK HACK HACK HACK
	// GWP If the regent has died, we need to end the game not go on.
	if (!fDomainTurn && !fBattleCombat)
	{
        if (regents[ActiveRegent].mfGetunit() > 0 &&
            playerstats[ActiveRegent] > 0)
        {
        	DumbHandlePtr<PLAYER_STATS const> const dhPlayerStats((SHORT)(playerstats[ActiveRegent]));
        	if ( dhPlayerStats->mfGetCurHitPoints() <= 0)
			{
				AdventureEndDialog(0);
				return;
			}
			else
			{
				if (dhPlayerStats->hAvatar != fERROR)
				{
					CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR (dhPlayerStats->hAvatar);
					if (pAvatar->mfAmIImmoblized())
					{
						AdventureEndDialog(0);
						return;
					}
				}
			}
		}
	}
	
	SCENE_MGR::fNewSceneRequested = TRUE;
	SCENE_MGR::fNewScenePlayerStart = playerStartLocation;
	strcpy(SCENE_MGR::fcpNewSceneName, cpSceneFile);
	if (FadeToNextScene)
		SCENE_MGR::fFadeToNextScene = SCENE_MGR::FADE_OUT;
}
/*	======================================================================== */
