/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: combtarg.cpp  - combat targeting code.
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "2DMOUSE.H"
#include "MACHINE.H"

#include "COMBATUI.HXX"
#include "COMBCAMR.HXX"
#include "COMBTARG.HXX"
#include "COMBOPTS.HXX"
#include "GAMEKEY.HXX"
#include "SCENE.HXX"

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

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Local Variables
   ------------------------------------------------------------------------ */

PFVLLL	 COMBAT_TARGETTING::fPrevObjMouseCallback = 0;
PFVLLL	 COMBAT_TARGETTING::fPrevWallMouseCallback = 0;
PFVLLL	 COMBAT_TARGETTING::fPrevFloorMouseCallback = 0;
PFVL	 COMBAT_TARGETTING::fPrevRightMouseCallback = 0;
LONG	 COMBAT_TARGETTING::fTargetIndex = fERROR;
SHORT	 COMBAT_TARGETTING::fhAvatar = fERROR;
COMBAT_TARGETTING::MODE COMBAT_TARGETTING::fMode = COMBAT_TARGETTING::NOT_SET;
BOOL	COMBAT_TARGETTING::fRegionsPushed = FALSE;

/* ========================================================================
   Function    - mfSetupTargetting
   Description - Initialize the targeting state values.
   Returns     - 
   ======================================================================== */

void COMBAT_TARGETTING::mfSetupTargetting(SHORT const hAvatar)
{
	if(fRegionsPushed == FALSE)
	{
		// clear target thing index
		fTargetIndex = fERROR;
		push_regions();
		fRegionsPushed = TRUE;
		
		// Add back necessary camera keys.
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_CYCLE_ACTIVE), CombatCycleActive, 0, 0);
		
		// Add back the camera movement hot keys.
		// Combat camera position keys
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_CAMERA_FIRSTPERSON), CombatFirstPersonView, 0, 0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_CAMERA_BEHINDFRIEND), CombatAutoViewBehindFriend, 0, 0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_CAMERA_BEHINDFOE), CombatAutoViewBehindFoe, 0, 0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_CAMERA_RIGHTFRIEND), CombatAutoViewFriendRight, 0, 0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_CAMERA_LEFTFRIEND), CombatAutoViewFriendLeft, 0, 0);
		
		
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_HIGH_ATTACK), CombatHighAttackKey, 0, 0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_LOW_ATTACK), CombatLowAttackKey, 0, 0);
		
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_SPELL1), CombatSpellKey,0,0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_SPELL2), CombatSpellKey,1,1);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_SPELL3), CombatSpellKey,2,2);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_SPELL4), CombatSpellKey,3,3);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_SPELL5), CombatSpellKey,4,4);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_SPELL6), CombatSpellKey,5,5);
		
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_FALLBACK), CombatFallbackKey, 0, 0);
		RECHECK_OPTIONS::mfUpDateOptions();
		
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::EXPAND_VIEWING_AREA), IncreaseAdventureScreen, 0, 0);
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::SHRINK_VIEWING_AREA), DecreaseAdventureScreen, 0, 0);
		
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_RETURN_TO_MAP), CombatReturnToMap, 0, 0);
		
		add_key(GAME_KEYS::mfGetKey(GAME_KEYS::COMBAT_RETREAT), CombatRetreatKey, 0, 0);
		
		init_mouse("TARGET",CURSOR_HOTSPOT_CENTER);
		
		mfSetModeTargetting();
		fhAvatar = hAvatar;
		
		fPrevObjMouseCallback = SetMouseClicked(iOBJECT,TargetClick);
			
		fPrevWallMouseCallback = SetMouseClicked(iWALL,TargetClick);
			
		fPrevFloorMouseCallback = SetMouseClicked(iFLOOR,TargetClick);
		
		fPrevRightMouseCallback = SetRightButtonClicked(TargetStop);
		// Pause all the AI's.
		SCENE * const pCurrentScene = (SCENE * const) BLKPTR(SCENE_MGR::hCurrentScene);
		for (LONG i = 0; i < MAX_AVATARS; i++)
		{
			if (pCurrentScene->Avatars[i] != fERROR)
			{
				//GEH SetBlockAttr(rScene.Avatars[i], LOCKED, LOCKED);
				CAvatar * const pAvatar = (CAvatar * const) BLKPTR(pCurrentScene->Avatars[i]);
				pAvatar->DoAI(CAvatar::AI_BEGIN_PAUSE);
				
				// Just incase he died.
				//GEH if (rScene.Avatars[i] != fERROR)
				//GEH 	ClrLock(rScene.Avatars[i]);
				
			}
		}
	}
}
/* ========================================================================
   Function    - TargetStop
   Description - Stop the targeting mode.
   Returns     - 
   ======================================================================== */
void COMBAT_TARGETTING::TargetStop(LONG button)
{
	if(button == 2) // right click
	{
		mfStopTargeting();
	}
}

/* ========================================================================
   Function    - mfStopTargeting
   Description - Subroutine to stop the targeting mode.
   Returns     - 
   ======================================================================== */
void COMBAT_TARGETTING::mfStopTargeting()
{
	if (fRegionsPushed == TRUE)
	{
		SetMouseClicked(iOBJECT,fPrevObjMouseCallback);
		SetMouseClicked(iWALL,  fPrevWallMouseCallback);
		SetMouseClicked(iFLOOR, fPrevFloorMouseCallback);
		SetRightButtonClicked(fPrevRightMouseCallback);
		
		init_mouse("DAGGER",CURSOR_HOTSPOT_UL);
		// return this station to it's normal programming
		pop_regions();
		clear_key_status(0);
		fRegionsPushed = FALSE;
		mfInitVals();
		// Pause all the AI's.
		SCENE * const pCurrentScene = (SCENE * const) BLKPTR(SCENE_MGR::hCurrentScene);
		for (LONG i = 0; i < MAX_AVATARS; i++)
		{
			if (pCurrentScene->Avatars[i] != fERROR)
			{
				//GEH SetBlockAttr(rScene.Avatars[i], LOCKED, LOCKED);
				CAvatar * const pAvatar = (CAvatar * const) BLKPTR(pCurrentScene->Avatars[i]);
				pAvatar->DoAI(CAvatar::AI_END_PAUSE);
				
				// Just incase he died.
				//GEH if (rScene.Avatars[i] != fERROR)
				//GEH 	ClrLock(rScene.Avatars[i]);
				
			}
		}
	}
}

/* ========================================================================
   Function    - TargetClick
   Description - Mouse Callback routine for targetting objects
   				 Note: Don't re-init the vals here. We need to wait
   				       until the play loop extracts the target info.
   Returns     - void
   ======================================================================== */
void COMBAT_TARGETTING::TargetClick(LONG but,LONG obj,LONG WadThingtype)
{
	if (fRegionsPushed)
	{
		if(but == 2) // right click
		{
			mfStopTargeting();
		}
		else
		if(WadThingtype == iOBJECT)
		{
			fTargetIndex = obj;
			SetMouseClicked(iOBJECT,fPrevObjMouseCallback);
			SetMouseClicked(iWALL,  fPrevWallMouseCallback);
			SetMouseClicked(iFLOOR, fPrevFloorMouseCallback);
			SetRightButtonClicked(fPrevRightMouseCallback);
			
			init_mouse("DAGGER",CURSOR_HOTSPOT_UL);
			// return this station to it's normal programming
			pop_regions();
			clear_key_status(0);
			fRegionsPushed = FALSE;
			// Pause all the AI's.
			SCENE * const pCurrentScene = (SCENE * const) BLKPTR(SCENE_MGR::hCurrentScene);
			for (LONG i = 0; i < MAX_AVATARS; i++)
			{
				if (pCurrentScene->Avatars[i] != fERROR)
				{
					//GEH SetBlockAttr(rScene.Avatars[i], LOCKED, LOCKED);
					CAvatar * const pAvatar = (CAvatar * const) BLKPTR(pCurrentScene->Avatars[i]);
					pAvatar->DoAI(CAvatar::AI_END_PAUSE);
					
					// Just incase he died.
					//GEH if (rScene.Avatars[i] != fERROR)
					//GEH 	ClrLock(rScene.Avatars[i]);
					
				}
			}
		}
	}
}
