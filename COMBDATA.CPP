/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: combdata.cpp  - Combat scene data.
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "COMBDATA.HXX"

#include "AVATAR.HXX"
#include "ADVENTUR.HXX"
#include "DOORS.H"
#include "ITEMTYPE.HXX"
#include "GAMEKEY.HXX"
#include "SCENE.HXX"
#include "TASK.H"

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

static void CombatButtonObject(
	LONG butClicked,
	LONG objClicked,
	LONG );
	
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Local Variables
   ------------------------------------------------------------------------ */
#ifdef _DEBUG
static int gDebug;
#endif
		
/* ========================================================================
   Function    - CombatButtonObject
   Description - Look at the mouse click and do the appropriate thing.
   Returns     -
   ======================================================================== */

static void CombatButtonObject(
	LONG butClicked,
	LONG objClicked,
	LONG ) //typeClicked
{
	mouse_click = 0;
	if (SCENE_MGR::hCurrentScene != fERROR)
	{
		LONG i;
		BOOL bObjFound = FALSE;
		
		SetBlockAttr(SCENE_MGR::hCurrentScene, LOCKED, LOCKED);
		PTR_SCENE pScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
		
		for (i = 0; i < MAX_AVATARS && !bObjFound; i++)
		{
			if (pScene->Avatars[i] != fERROR)
			{
				CAvatar *pAvatar = (CAvatar *) BLKPTR(pScene->Avatars[i]);
				if (pAvatar->ThingIndex == objClicked)
				{
					bObjFound = TRUE;
					// If the avatar dosen't have a button fn that's ok. It
					// may be dead.
					
					if (butClicked == 2 && pAvatar->RightButtonFn)
					{
						pAvatar->RightButtonFn(pAvatar);
					}
					else
					if (butClicked == 1 && pAvatar->LeftButtonFn)
					{
						pAvatar->LeftButtonFn(pAvatar);
					}
					
				}
			}
		}
	}
}
/* ========================================================================
   Function    - mfStartRetreat
   Description - 
   Returns     - 
   ======================================================================== */

void COMBAT_SCENE_DATA::RETREAT_DATA::mfStartRetreat(SHORT const hLeadAvatar,
                    LONG * const pLastAngle)
{
	fMaxRetreatPts = ADVENTURER::mfNumberOfTrailPts();
	fhRetreatingPositions = NewBlock(sizeof (PLAYER) * fMaxRetreatPts);
	
	if (fhRetreatingPositions != fERROR)
	{
		LONG ThingIndex;
		if (hLeadAvatar != fERROR)
		{
			CAvatar const * const pLeadAvatar = (CAvatar const * const ) BLKPTR(hLeadAvatar);
			ThingIndex = pLeadAvatar->ThingIndex;
		}
		else
		{
			ThingIndex = fERROR;
		}
		
		fTimer = 0;
		fRetreatFlag = TRUE;
		
		SetBlockAttr(fhRetreatingPositions, LOCKED, LOCKED);
		PLAYER * const pPlayerPts = (PLAYER * const) BLKPTR(fhRetreatingPositions);
		ADVENTURER::mfFillReversePlayerPts(pPlayerPts, fMaxRetreatPts, ThingIndex);
		fCurrentPosition = 0;
		*pLastAngle = pPlayerPts[fMaxRetreatPts].a;
		
		ClrLock(fhRetreatingPositions);
	}
	else
	{
		fRetreatFlag = FALSE;
		fCurrentPosition = fMaxRetreatPts;
	}
}
		
/* ========================================================================
   Function    - mfGetNextPt
   Description - 
   Returns     - 
   ======================================================================== */

BOOL const COMBAT_SCENE_DATA::RETREAT_DATA::mfGetNextPt(PLAYER *const pPlayer)
{
	BOOL Result = FALSE;
	
	if (fRetreatFlag == TRUE &&
	    fhRetreatingPositions != fERROR)
	{
		if ( fCurrentPosition < fMaxRetreatPts )
		{
			// [d11-20-96 JPC] Added TempPlayer to do bump test on retreat move.
			// If you bump something, you stop.
			PLAYER * const pPlayerPts = (PLAYER * const) BLKPTR(fhRetreatingPositions);
			PLAYER TempPlayer;
			PLAYER * pp;
			BOOL	 fFlying;

			FIXED_VECTOR_3D NewPoint;
			LONG Angle;
			LONG BumpDistance;

			// Use our current position for starting point.
			TempPlayer.x = pPlayer->x;
			TempPlayer.y = pPlayer->y;
			TempPlayer.z = pPlayer->z;
			TempPlayer.a = pPlayer->a;
			TempPlayer.p = pPlayer->p;
			TempPlayer.w = pPlayer->w;
			TempPlayer.h = pPlayer->h;
			TempPlayer.Flying = pPlayer->Flying;
			TempPlayer.Crouching = pPlayer->Crouching;
			TempPlayer.ThingIndex = fERROR;
			TempPlayer.WalkThruWall = 0;
			TempPlayer.bump = iNOTHING;
			TempPlayer.BumpIndex = fERROR;

			pp = pPlayerPts + fCurrentPosition;

			NewPoint.dx = pp->x - pPlayer->x;
			NewPoint.dy = pp->y - pPlayer->y;
			NewPoint.dz = pp->z - pPlayer->z;

			// [d11-22-96 JPC] Special test to prevent falling up.
			if (!pPlayer->Flying && NewPoint.dz > pPlayer->h / 2)
				TempPlayer.bump = iWALL;
			else
				CheckMove(&TempPlayer, &NewPoint, 0 /* CHECKLINE_PLAYER */, &Angle, &BumpDistance);

			if (TempPlayer.bump == iNOTHING)
			{
				// Move was OK.
				// [d11-21-96 JPC] Added fFlying because whether we can
				// fly or not should depend on our abilities when we
				// start the retreat and not what we had in the past.
				// (In the case I saw, we LOST the ability to fly
				// after retreating!)
				fFlying = pPlayer->Flying;
				*pPlayer = pPlayerPts[fCurrentPosition];
				pPlayer->Flying = fFlying;
				// Prevent suffering the same fall twice:
				pPlayer->fallHeight = 0;
				fCurrentPosition++;
				Result = TRUE;
#ifdef _DEBUG
				if (pPlayer->Crouching && pPlayer->h > 40)
				{
					gDebug++;			// for break point
				}
#endif
			}
			else
			{
				fRetreatFlag = FALSE;
				DisposBlock(fhRetreatingPositions);
				fhRetreatingPositions = fERROR;
                  // Q for GARY: what do we do, if anything, with fCurrentPosition?
				// fCurrentPosition = 0;
			}
		}
		else
		{
			DisposBlock(fhRetreatingPositions);
			fhRetreatingPositions = fERROR;
		}
	}
	
	return Result;
}

/* ========================================================================
   Function    - mfInitCombatVals
   Description - 
   Returns     - 
   ======================================================================== */

void COMBAT_SCENE_DATA::mfInitCombatVals()
{
	RECHECK_OPTIONS::mfInitOptions();
	
	mfInitSceneVals();

	OldInventoryPickupFunc=SetMouseClicked(iOBJECT,CombatButtonObject);
	
	replace_key_vals(GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_OPENDOOR),
		0, 0, 0, &OldDoorKey);
	
	// combat screens are best in High res. If AutoRes is on
	// go to High res. to start out.
	if (fAIAutoRes == TRUE)
	{
		set_hires(0, 0);
	}
	fAIAutoRes = FALSE;
	
	// Remember whether we can have high resolution art or not.
	fOldMedResAni = fMedResAni;
	
	// Remember which track we were playing.
	fucWhichTrack = ucWhichTrack;
	
	if (fMusic)
		PlayTrack(REDBOOK_PERSCOMBAT);
}
	
/* ========================================================================
   Function    - mfRestoreCombatVals
   Description - 
   Returns     - 
   ======================================================================== */

void COMBAT_SCENE_DATA::mfRestoreCombatVals()
{
	KEYSTRUCT	TempKeyStruct;
	
	mfRestoreSceneVals();
	
	SetMouseClicked(iOBJECT,OldInventoryPickupFunc);

	//if we're restoring the inventory, then these must be nulled out
	//otherwise the right-click-on-object-to-get-its-name stuff doesn't
	//work since that's handled by the inventory's mousecallback function

	SetLeftButtonClicked(NULL);
	SetRightButtonClicked(NULL);
		
	
	replace_key_vals(GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_OPENDOOR),
					OldDoorKey.func,
					OldDoorKey.val,
					OldDoorKey.val2,
					&TempKeyStruct);
	
// [d11-12-96 JPC] No need to return to low res; movement will do that.
#if 0
	// If we were in low res, and AutoRes is off, put us back
	// in low res.
	if (fOldHighResState == FALSE &&
	    fOldAutoResState == FALSE)
	{
		set_lowres(0, 0);
	}
#endif
	fAIAutoRes = fOldAutoResState;

	ucWhichTrack = fucWhichTrack;
	
	if (fMusic)
		PlayTrack(fucWhichTrack);
	
	fMedResAni = fOldMedResAni;
}
/* ========================================================================
   Function    - mfPauseUpdataData
   Description - While the scene AI is paused, the timers must continue to 
   				 update.
   Returns     - 
   ======================================================================== */

void COMBAT_SCENE_DATA::mfPauseUpdateData()
{
	LONG const DiffTime = SCENE_MGR::gTick - fgTickPrev;
	fCombatTimer += DiffTime;
	fDeadTimer += DiffTime;
	fDamageTimer += DiffTime;
	fRPGTimer += DiffTime;
	fgTickPrev = SCENE_MGR::gTick;
	
	// Also pause duration effects.
	ItemEffects::mfPauseDurations(DiffTime);
	
	// Pause the floors from rising, the ceiling from lowering, lifts.
	pause_task(fLiftTaskIndex);
	pause_task(fFloorTaskIndex);
	pause_task(fCeilingTaskIndex);
}

/* ========================================================================
   Function    - mfInitSceneVals
   Description - Get all the data needed at the start of the scene.
   Returns     - 
   ======================================================================== */

void COMBAT_SCENE_DATA::mfInitSceneVals()
{
	mfInitCombatTimer();
	mfInitRPGTimer();
	
	fRetreatData.mfInitVals();
	
	fDeadTimer = SCENE_MGR::gTick;
	fDamageTimer = SCENE_MGR::gTick + 200;
	
	fOldPlayerAngle = player.a;
	fOldCameraState = SCENE_MGR::fgCamera.mfGetCurrentState();
	fOldEnvSoundState = PlayEnvironmentalSounds;
	
	// fOldHighResState = fHighRes;
	fOldAutoResState = fAIAutoRes;
	
	fLiftTaskIndex = find_task(HandleLifts);
	fFloorTaskIndex = find_task(HandleFloors);
	fCeilingTaskIndex = find_task(HandleCeilings);
	
}
	
