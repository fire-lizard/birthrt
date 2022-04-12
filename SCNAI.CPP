/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: scene.cpp  -
   Author:   Gary Powell

   ========================================================================

   Contains the following general functions:


   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#ifdef _WINDOWS
#include <windows.h>
#endif

#include <ctype.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SYSTEM.H"
#include "ENGINE.H"
//#include "machine.h"

#include "ADVENTUR.HXX"
#include "AVATAR.HXX"
#include "BATTLEUI.HXX"
#include "COMBATUI.HXX"
#include "GAMEKEY.HXX"
#include "INVNTORY.HXX"
#include "MARGIN.HXX"
#include "PLAYER.HXX"
#include "PANEL.H"
#include "SCENE.HXX"
#include "SCNAI.HXX"
#include "SLFRDEMO.HXX"
#include "TUTRMENU.HXX"
#include "LOADSAVE.HXX"
//#include "sound.hxx"
#include "UNITS.HXX"

#ifdef _WINDOWS
#include "WINSYS\MULPLAY.HXX"
#include "WINSYS\MONO_C.H"
#endif


#ifdef _WINDOWS
#ifdef OLD_SOUND
#include "winsys\sndutil.h"
#include "winsys\musiutil.h"
extern HWND hwndApp;
#endif
#endif

extern void SysForceCursor (void);

void UpdateTime(void);

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
// Wait Button defines for mfSceneWaitForKey.
#define WB_X	13
#define WB_Y	450
#define WB_W	68
#define WB_H	28

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
extern BOOL	fChatLineOn;
extern LONG lens_factor;

BOOL fFreezeTime = FALSE;
BOOL fFreezeAvatars = FALSE;

BOOL gfInCombat;								// [d12-07-96 JPC] used by the door
													// code to decide whether to function.

SCENE_AI::PFVS SCENE_AI::sfpSceneTable[SCENE_AI::MAX_SCENES][SCENE_AI::SCENE_MAX_STATES] = {
#if NOT_USED
	{ SCENE_AI::mfSceneInit,
	  SCENE_AI::mfSceneRelease,
	  SCENE_AI::mfScenePause,
	  SCENE_AI::mfBattleWaitForKey,
	  SCENE_AI::mfSceneFadeOut,
	  SCENE_AI::mfScenePlay
	},
#endif
	{ SCENE_AI::mfBattleInit,
	  SCENE_AI::mfBattleRelease,
	  SCENE_AI::mfBattlePause,
	  SCENE_AI::mfBattleWaitForKey,
	  SCENE_AI::mfBattleFadeOut,
	  SCENE_AI::mfBattlePlay
	},
	{ SCENE_AI::mfCombatInit,
	  SCENE_AI::mfCombatRelease,
	  SCENE_AI::mfCombatPause,
	  SCENE_AI::mfCombatWaitForKey,
	  SCENE_AI::mfCombatFadeOut,
	  SCENE_AI::mfCombatPlay
	},
#if defined(_SELF_RUNNING_DEMO)
	{ SCENE_AI::mfSelfRunningDemoInit,
	  SCENE_AI::mfSelfRunningDemoRelease,
	  SCENE_AI::mfScenePause,
	  SCENE_AI::mfSceneWaitForKey,
	  SCENE_AI::mfSceneFadeOut,
	  SCENE_AI::mfSelfRunningDemoPlay
	},
#endif
	{ SCENE_AI::mfAdventureInit,
	  SCENE_AI::mfAdventureRelease,
	  SCENE_AI::mfScenePause,
	  SCENE_AI::mfBattleWaitForKey,
	  SCENE_AI::mfSceneFadeOut,
	  SCENE_AI::mfAdventurePlay
	}
};

#if defined(_SELF_RUNNING_DEMO)
/* ========================================================================
   Function    - mfSelfRunningDemoInit
   Description - PFLL proceedure for initializing a self running demo scene.
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfSelfRunningDemoInit ( SCENE &rScene)
{
#ifdef _WINDOWS
#ifdef OLD_SOUND
	//mci_OpenCD();
	//mci_PlayCD(hwndApp, 2, 0, 0, 0, 0, 0, 0);
#endif
#endif
	
	LONG i;
	
	UpdateTime();
	
	rScene.fhData = SRDEMO_DATA::mfAllocate();
	if (rScene.fhData != fERROR)
	{
		PTR_SRDEMO_DATA pSrdData = (PTR_SRDEMO_DATA) BLKPTR(rScene.fhData);
		
		if (!pSrdData->mfInitVals(rScene.mfGetCommandFileName()))
		{
			pSrdData->mfDelete();
			rScene.fhData = fERROR;
			rScene.mfSetSceneType( SCENE_AI::GENERIC_SCENE);
		}
		
		// Do some commands in case there are commands in the demo
		// which must be done before the scene runs.
		if (SRDEMO_DATA::NO_MORE_COMMANDS == pSrdData->mfDoCommand())
		{
			pSrdData->mfDelete();
			rScene.fhData = fERROR;
			rScene.mfSetSceneType (SCENE_AI::GENERIC_SCENE);
		}
	}
	else
	{
		rScene.mfSetSceneType( SCENE_AI::GENERIC_SCENE);
	}
	
	// Run through all INIT AI's
	for (i=0; i<MAX_AVATARS; i++)
	{
		if (rScene.Avatars[i] != fERROR)
		{
			//GEH SetBlockAttr(rScene.Avatars[i], LOCKED, LOCKED);
			CAvatar * const pAvatar = (CAvatar * const ) BLKPTR(rScene.Avatars[i]);
			pAvatar->DoAI(CAvatar::AI_INIT);
			
			// Hey he could have died!
			//GEH if (rScene.Avatars[i] != fERROR)
			//GEH 	ClrLock(rScene.Avatars[i]);
		}
	}
	rScene.mfSetSceneState(SCENE_AI::PLAY);
}
/* ========================================================================
   Function    - mfSelfRunningDemoRelease
   Description - PFLL proceedure for leaving a self running demo scene.
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfSelfRunningDemoRelease (SCENE &rScene)
{
	// Let go of the Self Running Demo Scene data.
	if (rScene.fhData != fERROR)
	{
		PTR_SRDEMO_DATA pSrdData = (PTR_SRDEMO_DATA) BLKPTR(rScene.fhData);
		
		pSrdData->mfDelete();
		rScene.fhData = fERROR;
		
	}
	rScene.mfReleaseScene();
}
		
/* ========================================================================
   Function    - mfSelfRunningDemoPlay
   Description - PFLL proceedure for any wad
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfSelfRunningDemoPlay (SCENE &rScene)
{
	UpdateTime();
	
	if (rScene.fhData != fERROR)
	{
		PTR_SRDEMO_DATA pSrdData = (PTR_SRDEMO_DATA) BLKPTR(rScene.fhData);
		
		if (SRDEMO_DATA::NO_MORE_COMMANDS != pSrdData->mfDoCommand())
		{
		   if (SCENE_MGR::fbRender)
		   {
			   MovePlayer(0);	// Allow no player movement from keyboard while
			                    // demo is running.
			
			   SCENE_MGR::fgCamera.mfUpdateCamera();
	   	
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
				   	   //GEH     ClrLock(rScene.Avatars[i]);
				    }
				}
			}
		}
		else
		{
			pSrdData->mfDelete();
			rScene.fhData = fERROR;
			rScene.mfSetSceneType (SCENE_AI::GENERIC_SCENE);
			rScene.mfSetSceneState(SCENE_AI::PLAY);
		}
	}
	else
	{
		rScene.mfSetSceneType (SCENE_AI::GENERIC_SCENE);
		rScene.mfSetSceneState(SCENE_AI::PLAY);
	}
}
#endif // _SELF_RUNNING_DEMO


#if NOT_USED
/* ========================================================================
   Function    - mfSceneInit
   Description - General PFLL proceedure for initializing a scene in a wad.
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfSceneInit ( SCENE &rScene)
{
#ifdef _WINDOWS
#ifdef OLD_SOUND
	//mci_OpenCD();
	//mci_PlayCD(hwndApp, 2, 0, 0, 0, 0, 0, 0);
#endif
#endif
	
	LONG 	i;

	UpdateTime();
	
	/* keyboard keys */
	push_regions();
	AddGameKeys();
	
 	// set engine lens
 	lens_factor = NORMAL_LENS;
 	
 	// now add the game unit avatars to the scene
 	// now add the game unit avatars to the scene
	if(SCENE_MGR::HomeIndex != -1)
	{
		// convert the game avatars to the away team
		SHORT UnitIndex = SCENE_MGR::HomeIndex;
		
		// Initialize the Regent to be the current picker up of things
		// clicked on.
		CAvatar *pAvatar = (CAvatar *) BLKPTR(units[UnitIndex].iHandle);
		pAvatar->mfSetMeToThePlayer();
		
		// convert the game avatars to members of an adventuer
		for (;
			 UnitIndex != fERROR;
			 UnitIndex = (SHORT) units[UnitIndex].NextUnit)
		{
			pAvatar = (CAvatar *) BLKPTR(units[UnitIndex].iHandle);
			pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PLAYER);
			pAvatar->mfInitVals();
			pAvatar->fFollowPlayer.mfJoinAdventureTeam();
		}
	}
	
	// convert the game avatars to members of an adventure
	for (SHORT UnitIndex = SCENE_MGR::AwayIndex;
		 UnitIndex != fERROR;
		 UnitIndex = (SHORT) units[UnitIndex].NextUnit)
	{
		CAvatar * const pAvatar = (CAvatar * const) BLKPTR(units[UnitIndex].iHandle);
		pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PLAYER);
		pAvatar->mfInitVals();
		pAvatar->fFollowPlayer.mfJoinAdventureTeam();
	}
	
	// Run through all INIT AI's
	for (i=0; i<MAX_AVATARS; i++)
	{
		if (rScene.Avatars[i] != fERROR)
		{
			//GEH SetBlockAttr(rScene.Avatars[i], LOCKED, LOCKED);
			CAvatar * const pAvatar = (CAvatar * const) BLKPTR(rScene.Avatars[i]);
			pAvatar->DoAI(CAvatar::AI_INIT);
			
			// Hey he could have died!
			//GEH if (rScene.Avatars[i] != fERROR)
			//GEH 	ClrLock(rScene.Avatars[i]);
		}
	}
	// [d11-15-96 JPC] rScene.mfSetSceneState(SCENE_AI::PLAY);
	ResumeSuspendedMusic();	
}
/* ========================================================================
   Function    - mfSceneRelease
   Description - General PFLL proceedure for any a wad
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfSceneRelease (SCENE &rScene)
{
	rScene.mfReleaseScene();
	
	// release the control panel stuff
	HideMenu(D_GAMEBUTTON);
	
	pop_regions();
	
#ifdef _WINDOWS
#ifdef OLD_SOUND
	//StopAllWave(); // Don't stop now incase we are in the middle of a speech.
	StopMusic();
	FinalMusic();
	//mci_StopCD();
	//mci_CloseCD();
#endif
#endif
}
		
/* ========================================================================
   Function    - mfScenePlay
   Description - General PFLL proceedure for any wad
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfScenePlay (SCENE &rScene)
{
	UpdateTime();
	
	MovePlayer(CheckMoveKeys());
	SCENE_MGR::fgCamera.mfUpdateCamera();
	
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
			//GEH 	ClrLock(rScene.Avatars[i]);
		}
	}
}

#endif // _NOT_USED
/* ========================================================================
   Function    - mfScenePause
   Description - General pause scene ai proceedure for any wad
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfScenePause (SCENE &rScene)
{
#if defined(SCENE_EDITOR)
	// All the avatars have to stop moving so we can edit them!
	//if (bEditMode)
	{
#endif

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
		   //GEH     ClrLock(rScene.Avatars[i]);
	    }
	}

#if defined(SCENE_EDITOR)
	}
#endif
}

/* ========================================================================
   Function    - mfSceneWaitForKey
   Description - Put up a "Done" button and wait for a keypress
					  or mouse button click before launching an adventure WAD
   Returns     - void
	Notes       - 11-15-96 JPC
					- [d11-17-96 JPC] Revision: in DOS, don't put up a button,
					  just a prompt for a keypress or button.  (Could not
					  figure out how to move the mouse in DOS without various
					  flicker/failure to erase problems.)
					  [d3-14-97 JPC] Don't process if (master_game_type == GAME_ADVENTURE)

   ======================================================================== */
void SCENE_AI::mfSceneWaitForKey (SCENE &rScene)
{
// [d3-13-97 JPC] Remove Windows code with button to make interface
// consistent between DOS and Windows.  Still need to leave in the
// Windows-specific call to WindowsMessages.

	// [d3-14-97 JPC] If doing adventures only, don't wait for a key
	// because the adventure-only routine has its own menu and button.
	if (master_game_type == GAME_ADVENTURE)
	{
		rScene.mfSetSceneState(SCENE_AI::FADEOUT);
  		return;									// premature return
	}

	print_textf(23, 457, BLACK, STRMGR_GetStr(STR_CS_WAITKEY));
	ScreenCopy (0, 13, WB_Y, 280, 28, SC_DEFAULT_RES);
	clear_key_status(0);
	fAnyKeyChanged = FALSE;
	mouse_button = 0;
	while (1)
	{
#ifdef _WINDOWS
		WindowsMessages();
#endif
		// We have to do this to keep the sound going in DOS.
		run_timers();

		update_buttons();
		if (fAnyKeyChanged)
		{
			fAnyKeyChanged = FALSE;
			clear_key_status(0);
			break;
		}
		if(mouse_button)
		{
			break;
		}
	}
	// [d12-03-96 JPC] Debounce the mouse button to prevent activating
	// the Exit Adventure region under the DONE button.
	do
	{
	   mouse_button = 0;
#ifdef _WINDOWS
		WindowsMessages();
#endif
		update_buttons();
	} while (mouse_button);
	
	mouse_button = 0;
	mouse_click = 0;
	rScene.mfSetSceneState(SCENE_AI::FADEOUT);
}

/* ========================================================================
   Function    - mfSceneFadeOut
   Description - Fade out before starting a WAD
   Returns     - void
	Notes       - 11-15-96 JPC
   ======================================================================== */
void SCENE_AI::mfSceneFadeOut (SCENE &rScene)
{
	UpdateTime();
	
	rScene.mfSetScenePallet();
	if (SCENE_MGR::fFadeToNextScene == SCENE_MGR::FADE_OUT)
	{
		FadeOut(500);
		SCENE_MGR::fFadeToNextScene = SCENE_MGR::FADE_IN;
	}

	// Clear out the background buffer to solve a number
	// of bugs with the main map showing through.  Note that fRealmTurn
	// is FALSE at this point.
	// clear_screen ();
	clear_screen_to (1);						// [d3-13-97 JPC]
	MARGIN_CONTROL::mfResetCurrentSize();
	update_screen ();
	rScene.mfSetSceneState(SCENE_AI::PLAY);
	rScene.mfCallSceneAI();
	
	// Any Panels drawn to the screen need one good paint.
	fUpdatePanels = TRUE;
}

/* ========================================================================
   Function    - mfBattleInit
   Description - General PFLL proceedure for initializing a battle scene in a wad.
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfBattleInit ( SCENE &rScene)
{
#ifdef _WINDOWS
#ifdef OLD_SOUND
	//mci_OpenCD();
	//mci_PlayCD(hwndApp, 2, 0, 0, 0, 0, 0, 0);
#endif
#endif
	
	LONG 	i;
	
	UpdateTime();
	
 	/* keyboard keys */
	push_regions();
	AddGameKeys();

	//makesure clicking the mouse will do nothing
	SetMouseClicked(iOBJECT,NULL);
	
	// set engine lens
 	lens_factor = NORMAL_LENS*3;
 	
	// load and init the battle control panel
	InitBattleUI(rScene);
	
	// put up the main menu button
	ShowMenu(D_GAMEBUTTON);
	
 	// now add the game unit avatars to the scene
	if(SCENE_MGR::HomeIndex != -1)
	{
		// generate the game avatars battle captains
		BtlCapGenerate(SCENE_MGR::HomeIndex);
	}
	
	if(SCENE_MGR::AwayIndex != -1)
	{
		// generate the game avatars battle captains
		BtlCapGenerate(SCENE_MGR::AwayIndex);
	}
	
	// Run through all INIT AI's
	for (i=0; i<MAX_AVATARS; i++)
	{
		if (rScene.Avatars[i] != fERROR)
		{
			//GEH SetBlockAttr(rScene.Avatars[i], LOCKED, LOCKED);
			CAvatar * const pAvatar = (CAvatar * const) BLKPTR(rScene.Avatars[i]);
			pAvatar->DoAI(CAvatar::AI_INIT);
			
			// Hey he could have died!
			//GEH if (rScene.Avatars[i] != fERROR)
			//GEH 	ClrLock(rScene.Avatars[i]);
		}
	}
	
	// start the scene
	rScene.mfSetSceneState(SCENE_AI::PLAY);
	
	SysForceCursor();
}
/* ========================================================================
   Function    - mfBattleRelease
   Description - General PFLL proceedure for a battle scene
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfBattleRelease (SCENE &rScene)
{
	char path[256];	
	fChatLineOn = FALSE;
	
	// release the control panel stuff
	HideMenu(D_GAMEBUTTON);
	ReleaseBattleUI(rScene);
	
	rScene.mfReleaseScene();
	
	TurnOffAllSounds();
	//StopASound(SND_UI_MAP_NAVIGATE_LOOP,BtlSoundTag);
	//BtlSoundTag = fERROR;
	
	init_mouse("DAGGER",CURSOR_HOTSPOT_UL);
	
	pop_regions();
	
#ifdef _WINDOWS
#ifdef OLD_SOUND
	//StopAllWave(); // Don't stop now incase we are in the middle of a speech.
	StopMusic();
	FinalMusic();
	//mci_StopCD();
	//mci_CloseCD();
#endif
#endif
}
		
/* ========================================================================
   Function    - mfBattlePlay
   Description - General PFLL proceedure for battle wads
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfBattlePlay (SCENE &rScene)
{
	BattleMoveCamera();
	
	UpdateTime();
	
	if (!fFreezeAvatars)
	{
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
				//GEH 	ClrLock(rScene.Avatars[i]);
			}
		}
	}
}

/* ========================================================================
   Function    - mfBattlePause
   Description - General pause scene ai proceedure for battle scenes
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfBattlePause (SCENE &rScene)
{
	for(LONG i = 0; i < MAX_AVATARS; i++ )
	{
	    // This is a sparsly filled array.
	    if (rScene.Avatars[i] != fERROR)
	    {
		    //GEH SetBlockAttr(rScene.Avatars[i], LOCKED, LOCKED);
		    CAvatar * const pAvatar = (CAvatar * const) BLKPTR(rScene.Avatars[i]);
		    pAvatar->DoAI();
	
		   // Hey he could have died!
		   //GEH if (rScene.Avatars[i] != fERROR)
		   //GEH     ClrLock(rScene.Avatars[i]);
	    }
	}

}

/* ========================================================================
   Function    - mfBattleWaitForKey
   Description - Don't actually wait for a key in this case.
					  (we don't want to wait for a keypress when going
					  into a battle WAD)
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfBattleWaitForKey (SCENE &rScene)
{
	rScene.mfSetSceneState(SCENE_AI::FADEOUT);
	rScene.mfCallSceneAI();
}

/* ========================================================================
   Function    - mfBattleFadeOut
   Description - Fade out, then play the scene
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfBattleFadeOut (SCENE &rScene)
{
	UpdateTime();
	
	if (SCENE_MGR::fFadeToNextScene == SCENE_MGR::FADE_OUT)
	{
		FadeOut(500);
		SCENE_MGR::fFadeToNextScene = SCENE_MGR::FADE_IN;
	}

	// Clear out the background buffer to solve a number
	// of bugs with the main map showing through.  Note that fRealmTurn
	// is FALSE at this point.
	// clear_screen ();
	clear_screen_to (1);						// [d3-13-97 JPC]
	update_screen ();
	rScene.mfSetSceneState(SCENE_AI::PLAY);
	rScene.mfCallSceneAI();
}

/* ========================================================================
   Function    - mfCombatInit
   Description - General PFLL proceedure for initializing a Combat scene in a wad.
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfCombatInit ( SCENE &rScene)
{
#ifdef _WINDOWS
#ifdef OLD_SOUND
	//mci_OpenCD();
	//mci_PlayCD(hwndApp, 2, 0, 0, 0, 0, 0, 0);
#endif
#endif
	LONG i;

	UpdateTime();
	
	gfInCombat = TRUE;						// [d12-07-96 JPC] prevent doors,
													// lifts, etc. from moving.

	// load and init the Combat control panel
	InitCombatUI(rScene);
		
	rScene.mfSetSceneState(SCENE_AI::PLAY);
	// pause the Avatars to allow the camera to get in place.
	for (i = 0; i < MAX_AVATARS; i++)
	{
		if (rScene.Avatars[i] != fERROR)
		{
			//GEH SetBlockAttr(rScene.Avatars[i], LOCKED, LOCKED);
			CAvatar *pAvatar = (CAvatar *) BLKPTR(rScene.Avatars[i]);
			pAvatar->DoAI(CAvatar::AI_BEGIN_PAUSE);
			
			// Just incase he died.
			//GEH if (rScene.Avatars[i] != fERROR)
			//GEH 	ClrLock(rScene.Avatars[i]);
			
		}
	}
	SysForceCursor();
}
/* ========================================================================
   Function    - mfCombatRelease
   Description - General PFLL proceedure for a Combat scene
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfCombatRelease (SCENE &rScene)
{
	// release the control panel stuff
	ReleaseCombatUI(rScene);

	gfInCombat = FALSE;						// [d12-07-96 JPC] re-enable doors,
													// lifts, etc.

	// Go back to standard Adventure AI.
	rScene.mfSetSceneType(SCENE_AI::ADVENTURE_SCENE);
	rScene.mfSetSceneState(SCENE_AI::PLAY);
	
#ifdef _WINDOWS
#ifdef OLD_SOUND
	//StopAllWave(); // Don't stop now incase we are in the middle of a speech.
	StopMusic();
	FinalMusic();
	//mci_StopCD();
	//mci_CloseCD();
#endif
#endif
}
		
/* ========================================================================
   Function    - mfCombatPlay
   Description - General PFLL proceedure for Combat wads
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfCombatPlay (SCENE &rScene)
{
	UpdateTime();
	
	// Let the AI's run before trying to keep the camera on them.
	PlayCombatUI(rScene);
}
/* ========================================================================
   Function    - mfCombatPause
   Description - General pause scene ai proceedure for any wad
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfCombatPause (SCENE &rScene)
{
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
		    //GEH     ClrLock(rScene.Avatars[i]);
		    //GEH }
	    }
	}
	
	// Let the AI's run before trying to keep the camera on them.
	PauseCombatUI(rScene);
}

/* ========================================================================
   Function    - mfCombatWaitForKey
   Description - Does not wait for a key--function exists for consistency
					  with scene functions
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfCombatWaitForKey (SCENE &rScene)
{
	rScene.mfSetSceneState(SCENE_AI::FADEOUT);
	rScene.mfCallSceneAI();
}

/* ========================================================================
   Function    - mfCombatFadeOut
   Description - Just a stub for consistency with scene functions
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfCombatFadeOut (SCENE &rScene)
{
	UpdateTime();
	
	if (SCENE_MGR::fFadeToNextScene == SCENE_MGR::FADE_OUT)
	{
		FadeOut(500);
		SCENE_MGR::fFadeToNextScene = SCENE_MGR::FADE_IN;
	}

	// Clear out the background buffer to solve a number
	// of bugs with the main map showing through.  Note that fRealmTurn
	// is FALSE at this point.
	// clear_screen ();
	clear_screen_to (1);						// [d3-13-97 JPC]
	update_screen ();
	rScene.mfSetSceneState(SCENE_AI::PLAY);
	rScene.mfCallSceneAI();
}

/* ========================================================================
   Function    - mfAdventureInit
   Description - General PFLL proceedure for initializing a scene in a wad.
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfAdventureInit ( SCENE &rScene)
{
#ifdef _WINDOWS
#ifdef OLD_SOUND
	//mci_OpenCD();
	//mci_PlayCD(hwndApp, 2, 0, 0, 0, 0, 0, 0);
#endif
#endif
	
	UpdateTime();
	
 	// set engine lens
 	lens_factor = NORMAL_LENS;
 	
	// Allocate some space attached to the scene.
	push_regions();

	//make sure we can pick up objects
	SetMouseClicked(iOBJECT,ObjectList::mfMouseCallback);

	
	if (TRUE == AdventureAllocateData(rScene))
	{
		LONG 	i;
	
		PlayTrack(ucWhichTrack);
		
		set_margin_size(0, 0, 0, 0);
		
		SetSoundDecay(300, 1000);
	
		/* keyboard keys */
		AddGameKeys();
		
		// put up the main menu button
		ShowMenu(D_GAMEBUTTON);
		
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::EXPAND_VIEWING_AREA), IncreaseAdventureScreen, 0, 0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::SHRINK_VIEWING_AREA), DecreaseAdventureScreen, 0, 0);
		
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_TOGGLEMODE), ToggleCombatMode, 0, 0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_RETURN_TO_MAP), CombatReturnToMap, 0, 0);
		
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::CAMERA_FOLLOW_BEHIND), SCENE_MGR::mfToggleFollowCamera, 0, 0);
	
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_SHOW_STATUS), AdventureShowStatus, 0, 0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_SHOW_INVENTORY), AdventureShowInventory,0 ,0);
		
		ADVENTURER::mfInitAdventureTeam(&player);
		
	 	// now add the game unit avatars to the scene
		if(SCENE_MGR::HomeIndex != -1)
		{
			// convert the game avatars to the away team
			SHORT UnitIndex = SCENE_MGR::HomeIndex;
			
			// Initialize the Regent to be the current picker up of things
			// clicked on.
			CAvatar *pAvatar = (CAvatar *) BLKPTR(units[UnitIndex].iHandle);
			pAvatar->mfSetMeToThePlayer();
			
			do
			{
				pAvatar = (CAvatar *) BLKPTR(units[UnitIndex].iHandle);
				pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PLAYER);
				pAvatar->mfInitVals();
				pAvatar->fFollowPlayer.mfJoinAdventureTeam();
				UnitIndex = (SHORT) units[UnitIndex].NextUnit;
			} while(UnitIndex != -1);
		}
		
	   // Need a location and an AI not FOLLOW_PLAYER for the enemys.
	   //	if(SCENE_MGR::AwayIndex != -1)
	   //	{
	   //		// convert the game avatars to the away team.
	   //		SHORT UnitIndex = SCENE_MGR::AwayIndex;
	   //		do
	   //		{
	   //			CAvatar * const pAvatar = (CAvatar * const) BLKPTR(units[UnitIndex].iHandle);
	   //			pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PLAYER);
	   //			pAvatar->mfInitVals();
	   //			UnitIndex = (SHORT) units[UnitIndex].NextUnit;
	   //		} while(UnitIndex != -1);
	   //	}
		
		// Run through all INIT AI's
		for (i=0; i<MAX_AVATARS; i++)
		{
			if (rScene.Avatars[i] != fERROR)
			{
				//GEH SetBlockAttr(rScene.Avatars[i], LOCKED, LOCKED);
				CAvatar * const pAvatar = (CAvatar * const) BLKPTR(rScene.Avatars[i]);
				pAvatar->DoAI(CAvatar::AI_INIT);
				
				// Hey he could have died!
				//GEH if (rScene.Avatars[i] != fERROR)
				//GEH 	ClrLock(rScene.Avatars[i]);
			}
		}
		
		//Resize the screen with the combat menu etc.
		InitAdventureUI(rScene);
		
		rScene.mfSetSceneState(SCENE_AI::PLAY);
	}
	else
	{
		// If not enough memory, skip out.
		SCENE_MGR::mfReleaseSceneToMap(0,0);
	}
	SysForceCursor();
//	ResumeSuspendedMusic();
}
/* ========================================================================
   Function    - mfAdventureRelease
   Description - General PFLL proceedure for any a wad
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfAdventureRelease (SCENE &rScene)
{
	AdventureFixUnitsArray(SCENE_MGR::HomeIndex);
	
	// GWP We'll disconnect these player stats when we delete the avatars.
	// GWP ADVENTURER_TEAM_ITOR AdvItor;
	// GWP for (AdvItor = ADVENTURER::begin(); AdvItor != ADVENTURER::end();
	// GWP 	AdvItor++)
	// GWP {
	// GWP 	DumbHandlePtr<CAvatar> const pAvatar(*AdvItor);
	// GWP 	SHORT const hPS = pAvatar->hPlayerStats;
	// GWP 	if (hPS < 0)
	// GWP 		continue;
	// GWP 	
	// GWP 	DumbHandlePtr<PLAYER_STATS> const pPS(hPS);
	// GWP 	pPS->hAvatar = fERROR;
	// GWP }	
	
	ADVENTURER::mfReleaseAdventureTeam();
	
	AdventureDeallocateData(rScene);
	
	rScene.mfReleaseScene();
	// release the control panel stuff
	HideMenu(D_GAMEBUTTON);
	SetSoundDecay(100, 200);
	
	pop_regions();
	
#ifdef _WINDOWS
#ifdef OLD_SOUND
	//StopAllWave(); // Don't stop now incase we are in the middle of a speech.
	TurnOffAllSounds();
	StopMusic();
	FinalMusic();
	//mci_StopCD();
	//mci_CloseCD();
#endif
#endif
}
/* ========================================================================
   Function    - mfAdventurePlay
   Description - General PFLL proceedure for any wad
   Returns     - void
   ======================================================================== */
void SCENE_AI::mfAdventurePlay (SCENE &rScene)
{
	UpdateTime();
	
	MovePlayer(CheckMoveKeys());
	SCENE_MGR::fgCamera.mfUpdateCamera();
	
	if (ADVENTURER::mfIsAdventureTeam(&CAvatar::mfAmIAlive))
	{
		LONG	const OpeningDialogMsg = rScene.mfGetOpeningDialog();
		if (OpeningDialogMsg != STR_NULL)
		{
			InitializeTutorialMenu(OpeningDialogMsg, 0);
			rScene.mfClearOpeningDialog();
		}
		
		if (gDialogFlag)
		{
			GAME_STRING const dialogMsg = rScene.mfGetDialog(gDialogFlag);
			if (dialogMsg != STR_NULL)
			{
				InitializeTutorialMenu(dialogMsg, D_FREEZE);
			}
			
			gDialogFlag = 0;
		}
		else
		{
			ADVENTURER::mfUpdateAdventureTeam(&player);
			
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
		
			PlayAdventureUI(rScene);
		}
	}
	else
	{
		AdventureEndDialog(0);
	}



#ifdef _WINDOWS

	//---- Check if the adventure over flag has been set by starter

	if ( IsMultiPlayer() )
	{
		if ( iAdventOver )
		{
			AdventureEndDialog(0);
			iAdventOver = FALSE;   //---- reset just in case
		}
	}

#endif



}

/* ========================================================================
   Function    - UpdateTime
   Description -
   Returns     -
   ======================================================================== */
void UpdateTime(void)
{
	static LONG prevTime = 0;
	LONG const CurrentTime = get_time();
	
	if (!fFreezeTime)
	{
		SCENE_MGR::gTick += CurrentTime - prevTime;
		SCENE_MGR::gFrame++;
	}

	prevTime = CurrentTime;
}
