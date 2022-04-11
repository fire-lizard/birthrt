/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: combcamr.hxx -
   Author:   Gary Powell

   ========================================================================

   Contains the following general functions:


   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#include "adventur.hxx"
#include "avatar.hxx"
#include "combcamr.hxx"
#include "combcntl.hxx"
#include "combdata.hxx"
#include "combopts.hxx"
#include "combtarg.hxx"
#include "gamecamr.hxx"
#include "scene.hxx"
#include "scnmgr.hxx"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */


#define LOOK_DOWN_PITCH				-180
#define DISTANCE_AWAY				150
#define YOU_DRIVE_DISTANCE_AWAY		200
#define MIN_DISTANCE_AWAY			50
#define DISTANCE_UP					100

#define CAMERA_DISTANCE_FACTOR		3 * 2 * 2
#define ROTATE_DISTANCE_FACTOR		3 * 2

#define RIGHT_OF_FRIEND_ANGLE	64
#define LEFT_OF_FRIEND_ANGLE	191
#define BEHIND_FRIEND_ANGLE		128
#define BEHIND_FOE_ANGLE		0


/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Classes
   ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */


COMBAT_CAMERA gccCamera;

/* ------------------------------------------------------------------------
   Local Variables
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

typedef LONG const (*CombatTest_pfv)(CAvatar const * const, BOOL *const);

static void CombatCameraRightOfFriend(CAvatar const * const /* pAvatar */);
static void CombatCameraLeftOfFriend(CAvatar const * const /* pAvatar */);
static void CombatCameraBehindFriend(CAvatar const * const /* pAvatar */);
static void CombatCameraBehindFoe(CAvatar const * const /* pAvatar */);
static void CombatCameraBehindAvatar(CAvatar const * const /* pAvatar */);
static void CombatCameraWanderFreely(CAvatar const * const /* pAvatar */);

static LONG const CombatTestCamera(
	CAvatar const * const /* pAvatar */,
	LONG	const /* Angle */,
	BOOL	*const /* pbCanSeeEnemy */);
	
static LONG const CombatTestCameraRightOfFriend(
	CAvatar const * const /* pAvatar */, BOOL *const
	/* pCanSeeEnemy */);
	
static LONG const CombatTestCameraLeftOfFriend(
	CAvatar const * const /* pAvatar */, BOOL *const
	/* pCanSeeEnemy */);

static LONG const CombatTestCameraBehindFriend(
	CAvatar const * const /* pAvatar */, BOOL *const
	/* pCanSeeEnemy */);

static LONG const CombatTestCameraBehindFoe(
	CAvatar const * const /* pAvatar */, BOOL *const
	/* pCanSeeEnemy */);

/* ========================================================================
   Function    - mfSetNewLeader
   Description -  Called to change leaders. Doesn't update the old leader 
   				position so the camera will pick a new relative angle.
   Returns     - 
   ======================================================================== */
void COMBAT_CAMERA::mfSetNewLeader(SHORT const hAvatar)
{
	if (hAvatar != fERROR)
	{
		fhLeader = hAvatar;
		CAvatar const * const pLeader = (CAvatar const * const) BLKPTR(fhLeader);
		SetDontDrawCloser(pLeader->ThingIndex);
		if (fCurrentState == WANDER_FREELY)
		{
			CombatCameraWanderFreely(pLeader);
		}
	}
}

/* ========================================================================
   Function    - Test4States
   Description - Test four states and pick the best fit.
                 Note: Best fit, means if the distance to the wall is
                       at least the DesiredDistance then that coordinate
                       is satisfied. If not try the next coordinate, remembering
                       the previous best coordinate.
   Returns     - the best state
   ======================================================================== */
static COMBAT_CAMERA::CAMERA_STATE const Test4States(
	CAvatar const * const				pLeader,
	LONG const							DesiredDistance,
	LONG const							MinimumDistance,
	CombatTest_pfv 						TestCamera1,
	COMBAT_CAMERA::CAMERA_STATE	const	TestState1,
	CombatTest_pfv const				TestCamera2,
	COMBAT_CAMERA::CAMERA_STATE	const	TestState2,
	CombatTest_pfv const				TestCamera3,
	COMBAT_CAMERA::CAMERA_STATE	const	TestState3,
	CombatTest_pfv const				TestCamera4,
	COMBAT_CAMERA::CAMERA_STATE	const	TestState4,
	COMBAT_CAMERA::CAMERA_STATE const	DefaultState )
{
	COMBAT_CAMERA::CAMERA_STATE Result;
	COMBAT_CAMERA::CAMERA_STATE BestState = DefaultState;
	LONG BumpDistance;
	LONG BestDistance = 0;
	BOOL CanSeeEnemy = 0;
	
	BumpDistance = (*TestCamera1)(pLeader, &CanSeeEnemy);
	if (BumpDistance >= DesiredDistance && CanSeeEnemy)
	{
		Result = TestState1;
	}
	else
	{
		BestDistance = BumpDistance;
		BestState = TestState1;
		
		BumpDistance = (*TestCamera2)(pLeader, &CanSeeEnemy);
		if (BumpDistance >= DesiredDistance && CanSeeEnemy)
		{
			Result = TestState2;
		}
		else
		{
			if (BumpDistance > BestDistance && CanSeeEnemy)
			{
				BestDistance = BumpDistance;
				BestState = TestState2;
			}
			
			BumpDistance = (*TestCamera3)(pLeader, &CanSeeEnemy);
			if (BumpDistance >= DesiredDistance && CanSeeEnemy)
			{
				Result = TestState3;
			}
			else
			{
				if (BumpDistance > BestDistance)
				{
					BestDistance = BumpDistance;
					BestState = TestState3;
				}
				
				BumpDistance = (*TestCamera4)(pLeader, &CanSeeEnemy);
				if (BumpDistance >= DesiredDistance && CanSeeEnemy)
				{
					Result = TestState4;
				}
				else
				{
					if (BumpDistance > BestDistance && CanSeeEnemy)
					{
						BestDistance = BumpDistance;
						BestState = TestState4;
					}
					
					if (BestDistance < MinimumDistance)
					{
						Result = DefaultState;
					}
					else
					{
						Result = BestState;
					}
				}
			}
		}
	}
	
	return Result;
}

/* ========================================================================
   Function    - mfTestCamera
   Description - Called at the beginning of entering "Single" or group
                 combat mode to try and guarentee that the battle will
                 be visible.
   Returns     - The camera state.
   ======================================================================== */
COMBAT_CAMERA::CAMERA_STATE COMBAT_CAMERA::mfTestCamera() const
{
	CAMERA_STATE Result = RIGHT_OF_FRIENDS; // Default
	
	if (fhLeader != fERROR)
	{
		CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(fhLeader);
		Result = Test4States(pAvatar,
		                     DISTANCE_AWAY,
							 (pAvatar->mfWidth() * CAMERA_DISTANCE_FACTOR),
							 CombatTestCameraRightOfFriend,
							 RIGHT_OF_FRIENDS,
							 CombatTestCameraBehindFriend,
							 BEHIND_FRIENDS,
							 CombatTestCameraLeftOfFriend,
							 LEFT_OF_FRIENDS,
							 CombatTestCameraBehindFoe,
							 BEHIND_FOES,
							 FOLLOW_AVATAR);
	}
	
	return Result;
}

/* ========================================================================
   Function    - mfPickNewCameraState
   Description - Update the quadrant for the Combat Camera.
   Returns     -
   ======================================================================== */
void COMBAT_CAMERA::mfPickNewCameraState(CAvatar const * const pLeader)
{
	LONG const AngleBetween = RelativeAngle(pLeader->mfAngle(),
	                                  AngleFromPoint(
	                                                 pLeader->mfX(),
	                                                 pLeader->mfY(),
	                                                 CAMERA_INT_VAL(camera.x),
	                                                 CAMERA_INT_VAL(camera.y),
	                                                 RESOLUTION_8));
	// GWP I adjusted these angles to work with the new
	//     camera cardinal points. (On the 45Degrees.)
	if (AngleBetween > 224)
	{
		fCurrentState = Test4States(pLeader,
		                            DISTANCE_AWAY,
									(pLeader->mfWidth() * CAMERA_DISTANCE_FACTOR),
									CombatTestCameraBehindFoe,
									BEHIND_FOES,
									CombatTestCameraLeftOfFriend,
									LEFT_OF_FRIENDS,
									CombatTestCameraRightOfFriend,
									RIGHT_OF_FRIENDS,
									CombatTestCameraBehindFriend,
									BEHIND_FRIENDS,
									FOLLOW_AVATAR);
	}
	else if (AngleBetween > 160)
	{
		fCurrentState = Test4States(pLeader,
		                            DISTANCE_AWAY,
									(pLeader->mfWidth() * CAMERA_DISTANCE_FACTOR),
									CombatTestCameraLeftOfFriend,
									LEFT_OF_FRIENDS,
									CombatTestCameraBehindFriend,
									BEHIND_FRIENDS,
									CombatTestCameraBehindFoe,
									BEHIND_FOES,
									CombatTestCameraRightOfFriend,
									RIGHT_OF_FRIENDS,
									FOLLOW_AVATAR);
	}
	else if (AngleBetween > 96)
	{
		fCurrentState = Test4States(pLeader,
		                            DISTANCE_AWAY,
									(pLeader->mfWidth() * CAMERA_DISTANCE_FACTOR),
									CombatTestCameraBehindFriend,
									BEHIND_FRIENDS,
									CombatTestCameraRightOfFriend,
									RIGHT_OF_FRIENDS,
									CombatTestCameraLeftOfFriend,
									LEFT_OF_FRIENDS,
									CombatTestCameraBehindFoe,
									BEHIND_FOES,
									FOLLOW_AVATAR);
	}
	else if (AngleBetween > 32 )
	{
		fCurrentState = Test4States(pLeader,
		                            DISTANCE_AWAY,
									(pLeader->mfWidth() * CAMERA_DISTANCE_FACTOR),
									CombatTestCameraRightOfFriend,
									RIGHT_OF_FRIENDS,
									CombatTestCameraBehindFoe,
									BEHIND_FOES,
									CombatTestCameraBehindFriend,
									BEHIND_FRIENDS,
									CombatTestCameraLeftOfFriend,
									LEFT_OF_FRIENDS,
									FOLLOW_AVATAR);
	}
	else
	{
		fCurrentState = Test4States(pLeader,
		                            DISTANCE_AWAY,
									(pLeader->mfWidth() * CAMERA_DISTANCE_FACTOR),
									CombatTestCameraBehindFoe,
									BEHIND_FOES,
									CombatTestCameraRightOfFriend,
									RIGHT_OF_FRIENDS,
									CombatTestCameraLeftOfFriend,
									LEFT_OF_FRIENDS,
									CombatTestCameraBehindFriend,
									BEHIND_FRIENDS,
									FOLLOW_AVATAR);
	}
}
/* ========================================================================
   Function    - mfUpdatePosition
   Description - Update the position of the Combat Camera.
   Returns     -
   ======================================================================== */
void COMBAT_CAMERA::mfUpdatePosition()
{
	CAvatar *pAvatar = 0;
	BOOL bLeaderMoved = FALSE;
	LONG const KeyState = (fWanderFreelyAllowed == TRUE) ? CheckMoveKeys() : 0;

	mfUpdateWanderFlag();
	
	if (fhLeader != fERROR)
	{
		pAvatar = (CAvatar *) BLKPTR(fhLeader);
		
		// If the user hits a key, release the auto follow mechanism.
		if (KeyState != 0)
		{
			if (fCurrentState != WANDER_FREELY)
			{
				CombatCameraWanderFreely(pAvatar);
				mfSetState(WANDER_FREELY);
			}
		}
		
		if (fCurrentState != FOLLOW_AVATAR &&
		    fCurrentState != FACING_BACKWARDS &&
		    fCurrentState != WANDER_FREELY)
		{
			bLeaderMoved = mfDidLeaderMove(pAvatar);
			if (bLeaderMoved)
			{
				mfPickNewCameraState(pAvatar);
			}
		}
	
	}
	else
	{
		// Don't crash.
		camera.TargetRequested = 0;
		mfSetState(WANDER_FREELY);
	}
	
	switch (fCurrentState)
	{
	case FOLLOW_AVATAR:
		// Note: This is the position that the camera normally is at.
		// GWP Implement later to adjust the requested position if the
		// GWP leader is in motion.
		
		if (camera.TargetRequested)
		{
			MoveCamera(&camera, 0);
		}
		else
		{
			SetCameraPosition(&camera,
							  pAvatar->mfX(),
					          pAvatar->mfY(),
					          //pAvatar->mfZ() + (pAvatar->mfHeight() - EYE_HEIGHT),
					          pAvatar->mfZ() + (player.h - EYE_HEIGHT),
					          pAvatar->mfAngle(),
					          0 // Avatars have no pitch.
							 );
		}
		break;
	case FACING_BACKWARDS:
		{
			LONG const Angle = (pAvatar->mfAngle() - 128) & 0xFF;
			SetCameraPosition(&camera,
							  pAvatar->mfX(),
					          pAvatar->mfY(),
					          //pAvatar->mfZ() + (pAvatar->mfHeight() - EYE_HEIGHT),
					          pAvatar->mfZ() + (player.h - EYE_HEIGHT),
					          Angle,
					          0 // Avatars have no pitch.
							 );
		}
		break;
	case BEHIND_FRIENDS:
		if (camera.TargetRequested && !bLeaderMoved)
		{
			MoveCamera(&camera, 0);
		}
		else
		{
			if (bLeaderMoved)
			{
				mfUpdateLeaderPosition(pAvatar);
			}
			
			CombatCameraBehindFriend(pAvatar);
			MoveCamera(&camera, 0);
		}
		break;
	case BEHIND_FOES:
		if (camera.TargetRequested && !bLeaderMoved)
		{
			MoveCamera(&camera, 0);
		}
		else
		{
			if (bLeaderMoved)
			{
				mfUpdateLeaderPosition(pAvatar);
			}
			
			CombatCameraBehindFoe(pAvatar);
			MoveCamera(&camera, 0);
		}
		break;
	case LEFT_OF_FRIENDS:
		if (camera.TargetRequested && !bLeaderMoved)
		{
			MoveCamera(&camera, 0);
		}
		else
		{
			if (bLeaderMoved)
			{
				mfUpdateLeaderPosition(pAvatar);
			}
			
			CombatCameraLeftOfFriend(pAvatar);
			MoveCamera(&camera, 0);
		}
		break;
	case RIGHT_OF_FRIENDS:
		if (camera.TargetRequested && !bLeaderMoved)
		{
			MoveCamera(&camera, 0);
		}
		else
		{
			if (bLeaderMoved)
			{
				mfUpdateLeaderPosition(pAvatar);
			}
			
			CombatCameraRightOfFriend(pAvatar);
			MoveCamera(&camera, 0);
		}
		break;
	case WANDER_FREELY:
 		{
		MoveCamera(&camera, KeyState);
		}
		break;
	case BEHIND_AVATAR:
		if (camera.TargetRequested)
		{
			MoveCamera(&camera, 0);
		}
		else
		{
			CombatCameraBehindAvatar(pAvatar);
		}
		break;
	}
}

/* ========================================================================
   Function    - CombatFirstPersonView
   Description - Snap back on the head cam
   				 This fn is attached to a hot key.
   Returns     -
   ======================================================================== */
void CombatFirstPersonView(LONG, LONG)
{
	if (SCENE_MGR::fgCamera.mfGetCurrentState() == GAME_CAMERA::PROGRAM_CAMERA_CONTROL &&
		CONTROL_PANELS::mfCurrentInUse())
	{
		CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(CONTROL_PANELS::mfGetCurrAvatarHdl());
		gccCamera.mfSetState(COMBAT_CAMERA::FOLLOW_AVATAR);
		gccCamera.mfSetNewLeader(CONTROL_PANELS::mfGetCurrAvatarHdl());
		
#if GWP_SWOOP_TO_POSITION
		SetCameraMotionVars(&camera,
		                    TARGET_FACTOR,
		                    NO_CHANGE);
		SetCameraTarget(&camera,
					    pAvatar->mfX(),
					    pAvatar->mfY(),
					    pAvatar->mfZ() + (pAvatar->mfHeight() - EYE_HEIGHT),
					    pAvatar->mfAngle(),
					    0,									// Avatars have no pitch.
					    TRUE
					    );
#else
		// Snap to location. (Avoids being stuck by a wall.)
		SetCameraPosition(&camera,
						  pAvatar->mfX(),
						  pAvatar->mfY(),
						  pAvatar->mfZ() + (pAvatar->mfHeight() - EYE_HEIGHT),
						  pAvatar->mfAngle(),
						  0
						  );
#endif
	}
	
	DumbHandlePtr<SCENE const> const pCurrentScene(SCENE_MGR::hCurrentScene);
	DumbHandlePtr<COMBAT_SCENE_DATA> const pcsdData(pCurrentScene->fhData);
	
	if (pcsdData->mfCanGoToHighResArt())
	{
		fMedResAni = FALSE;
	}
}

/* ========================================================================
   Function    - CombatAutoViewBehindFriend
   Description - View from the rear of your party
   				 This fn is attached to a hot key.
   				 If the panel has a "live" avatar, then switch the camera
   				 to them.
   Returns     -
   ======================================================================== */
void CombatAutoViewBehindFriend(LONG, LONG)
{
	if (SCENE_MGR::fgCamera.mfGetCurrentState() == GAME_CAMERA::PROGRAM_CAMERA_CONTROL &&
		CONTROL_PANELS::mfCurrentInUse())
	{
		CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(CONTROL_PANELS::mfGetCurrAvatarHdl());
		
		gccCamera.mfSetState(COMBAT_CAMERA::BEHIND_FRIENDS);
		
		gccCamera.mfSetNewLeader(CONTROL_PANELS::mfGetCurrAvatarHdl());
		
		CombatCameraBehindFriend(pAvatar);
		
	}
}
/* ========================================================================
   Function    - CombatAutoViewBehindFoe
   Description - View from the rear of their party
   				 This fn is attached to a hot key.
   				 If the panel has a "live" avatar, then switch the camera
   				 to them.
   Returns     -
   ======================================================================== */
void CombatAutoViewBehindFoe(LONG, LONG)
{
	if (SCENE_MGR::fgCamera.mfGetCurrentState() == GAME_CAMERA::PROGRAM_CAMERA_CONTROL &&
		CONTROL_PANELS::mfCurrentInUse())
	{
		CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(CONTROL_PANELS::mfGetCurrAvatarHdl());
		
		gccCamera.mfSetState(COMBAT_CAMERA::BEHIND_FOES);
		
		gccCamera.mfSetNewLeader(CONTROL_PANELS::mfGetCurrAvatarHdl());
		
		CombatCameraBehindFoe(pAvatar);
	}
}
/* ========================================================================
   Function    - CombatAutoViewFriendRight
   Description - View from the right side of the melee
   				 This fn is attached to a hot key.
   				 If the panel has a "live" avatar, then switch the camera
   				 to them.
   Returns     -
   ======================================================================== */
void CombatAutoViewFriendRight(LONG, LONG)
{
	if (SCENE_MGR::fgCamera.mfGetCurrentState() == GAME_CAMERA::PROGRAM_CAMERA_CONTROL &&
		CONTROL_PANELS::mfCurrentInUse())
	{
		CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(CONTROL_PANELS::mfGetCurrAvatarHdl());
		
		gccCamera.mfSetState(COMBAT_CAMERA::RIGHT_OF_FRIENDS);
		
		gccCamera.mfSetNewLeader(CONTROL_PANELS::mfGetCurrAvatarHdl());
		
		CombatCameraRightOfFriend(pAvatar);
	}
}

/* ========================================================================
   Function    - CombatAutoViewFriendLeft
   Description - View from the left side of the melee
   				 This fn is attached to a hot key.
   				 If the panel has a "live" avatar, then switch the camera
   				 to them.
   Returns     -
   ======================================================================== */
void CombatAutoViewFriendLeft(LONG, LONG)
{
	if (SCENE_MGR::fgCamera.mfGetCurrentState() == GAME_CAMERA::PROGRAM_CAMERA_CONTROL &&
		CONTROL_PANELS::mfCurrentInUse())
	{
		CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(CONTROL_PANELS::mfGetCurrAvatarHdl());
		
		gccCamera.mfSetState(COMBAT_CAMERA::LEFT_OF_FRIENDS);
		
		gccCamera.mfSetNewLeader(CONTROL_PANELS::mfGetCurrAvatarHdl());
		
		CombatCameraLeftOfFriend(pAvatar);
	}
}

/* ========================================================================
   Function    - CombatBehindPersonView
   Description - Same as the "V" key view during adventure.
   Returns     - 
   ======================================================================== */
void CombatBehindPersonView(LONG, LONG)
{
	if (SCENE_MGR::fgCamera.mfGetCurrentState() != GAME_CAMERA::PROGRAM_CAMERA_CONTROL)
	{
		SCENE_MGR::mfToggleFollowCamera(0,0);
	}
	else
	if (CONTROL_PANELS::mfCurrentInUse())
	{
		CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(CONTROL_PANELS::mfGetCurrAvatarHdl());
		gccCamera.mfSetState(COMBAT_CAMERA::BEHIND_AVATAR);
		gccCamera.mfSetNewLeader(CONTROL_PANELS::mfGetCurrAvatarHdl());
		
		CombatCameraBehindAvatar(pAvatar);
	}
	
}

/* ========================================================================
   Function    - CombatCameraRightOfFriend
   Description - View from the right side of the melee
   				 Set the initial state, so the camera will move to this new
   				 quadrant.
   Returns     -
   ======================================================================== */
static void CombatCameraRightOfFriend(CAvatar const * const pAvatar)
{
	SetCameraMotionVars(&camera,
	                    TARGET_FACTOR,
	                    NO_CHANGE);
	
	POINT_3D	ContactPoint;
	pAvatar->mfGetContactPoint(&ContactPoint);
	SetCameraRotateTarget(&camera,
 						  ContactPoint.x,
 						  ContactPoint.y, 
						  pAvatar->mfZ() + DISTANCE_UP,
						  LOOK_DOWN_PITCH,
						  pAvatar->mfAngle() + RIGHT_OF_FRIEND_ANGLE,
						  pAvatar->mfWidth() * ROTATE_DISTANCE_FACTOR,
						  DISTANCE_AWAY,
						  TRUE);
	if (!fHighResCombat)
		fMedResAni = TRUE;		// Stop the high res art from comming in.
}
/* ========================================================================
   Function    - CombatCameraLeftOfFriend
   Description - View from the left side of the melee
   				 Set the initial state, so the camera will move to this new
   				 quadrant.
   Returns     -
   ======================================================================== */
static void CombatCameraLeftOfFriend(CAvatar const * const pAvatar)
{
	SetCameraMotionVars(&camera,
	                    TARGET_FACTOR,
	                    NO_CHANGE);
	
	POINT_3D	ContactPoint;
	pAvatar->mfGetContactPoint(&ContactPoint);
	SetCameraRotateTarget(&camera,
 						  ContactPoint.x,
 						  ContactPoint.y, 
						  pAvatar->mfZ() + DISTANCE_UP,
						  LOOK_DOWN_PITCH,
						  pAvatar->mfAngle() + LEFT_OF_FRIEND_ANGLE,
						  pAvatar->mfWidth() * ROTATE_DISTANCE_FACTOR,
						  DISTANCE_AWAY,
						  TRUE);
	if (!fHighResCombat)
		fMedResAni = TRUE;		// Stop the high res art from comming in.
}
/* ========================================================================
   Function    - CombatCameraBehindFriend
   Description - View from the rear of your party
   				 Set the initial state, so the camera will move to this new
   				 quadrant.
   Returns     -
   ======================================================================== */
static void CombatCameraBehindFriend(CAvatar const * const pAvatar)
{
	SetCameraMotionVars(&camera,
	                    TARGET_FACTOR,
	                    NO_CHANGE);
	
	SetCameraRotateTarget(&camera,
							pAvatar->mfX(),
							pAvatar->mfY(),
							pAvatar->mfZ() + DISTANCE_UP,
							LOOK_DOWN_PITCH,
							pAvatar->mfAngle() + BEHIND_FRIEND_ANGLE,
							pAvatar->mfWidth() * ROTATE_DISTANCE_FACTOR,
							DISTANCE_AWAY,
							TRUE);
	if (!fHighResCombat)
		fMedResAni = TRUE;		// Stop the high res art from comming in.
}
/* ========================================================================
   Function    - CombatCameraBehindFoe
   Description - View from the rear of their party
   				 Set the initial state, so the camera will move to this new
   				 quadrant.
   Returns     -
   ======================================================================== */
static void CombatCameraBehindFoe(CAvatar const * const pAvatar)
{
	SetCameraMotionVars(&camera,
	                    TARGET_FACTOR,
	                    NO_CHANGE);
	
	SetCameraRotateTarget(&camera,
							pAvatar->mfX(),
							pAvatar->mfY(),
							pAvatar->mfZ() + DISTANCE_UP,
							LOOK_DOWN_PITCH,
							pAvatar->mfAngle() + BEHIND_FOE_ANGLE,
							pAvatar->mfWidth() * ROTATE_DISTANCE_FACTOR,
							DISTANCE_AWAY,
							TRUE);
	if (!fHighResCombat)
		fMedResAni = TRUE;		// Stop the high res art from comming in.
}

/* ========================================================================
   Function    - CombatCameraBehindAvatar
   Description - place the camera in the same offset as the "V" key.
   Returns     - 
   ======================================================================== */

static void CombatCameraBehindAvatar(CAvatar const * const pAvatar)
{
	FIXED_VECTOR_3D ptBehind;
	LONG Angle;
	LONG BumpDistance;
	PLAYER TempPlayer;
	
	ptBehind.dx = 0; // To left slightly(-player.w) << PLAYER_FIXEDPT;
	ptBehind.dy = (-100) << PLAYER_FIXEDPT;
	ptBehind.dz = pAvatar->mfHeight();
	Rotate((POINT *)&ptBehind, pAvatar->mfAngle());
	
	pAvatar->mfConvertPositionToPlayer(TempPlayer);
	
	CheckMove(&TempPlayer, &ptBehind, 0, &Angle, &BumpDistance);
	
	// Note: Bump distance is a squared value. Rather than get the 
	//       sqrt, I square the comparison value.
	if (TempPlayer.bump == iWALL && BumpDistance < (15 * 15))
	{
		SetCameraPosition(
			&camera,
			pAvatar->mfX(),
			pAvatar->mfY(),
			(pAvatar->mfZ()+(pAvatar->mfHeight() - EYE_HEIGHT)), 
			pAvatar->mfAngle(), 
			0	// Avatars have no pitch
			);
	}
	else
	{
		SetCameraPosition(
			&camera,
			(pAvatar->mfX() + PLAYER_INT_VAL(ptBehind.dx)),
			(pAvatar->mfY() + PLAYER_INT_VAL(ptBehind.dy)),
			(pAvatar->mfZ() + ptBehind.dz), 
			pAvatar->mfAngle(), 
			0	// avatars have no pitch.
			);
		
	}
	DumbHandlePtr<SCENE const> const pCurrentScene(SCENE_MGR::hCurrentScene);
	DumbHandlePtr<COMBAT_SCENE_DATA> const pcsdData(pCurrentScene->fhData);
	
	if (pcsdData->mfCanGoToHighResArt())
	{
		fMedResAni = FALSE;
	}
}

/* ========================================================================
   Function    - CombatCameraWanderFreely
   Description - Initialize the values for wandering freely about an avatar.
   Returns     - 
   ======================================================================== */

static void CombatCameraWanderFreely(CAvatar const * const pAvatar)
{ 
	POINT_3D	ContactPoint;
	pAvatar->mfGetContactPoint(&ContactPoint);
 	LONG const angle = AngleFromPoint2(
 									   ContactPoint.x,
 									   ContactPoint.y, 
 									   camera.x >> CAMERA_FIXEDPT, 
 									   camera.y >> CAMERA_FIXEDPT, 
 									   0);
 									   
	LONG const radius = dist(ContactPoint.x, ContactPoint.y, 
							 CAMERA_INT_VAL(camera.x), CAMERA_INT_VAL(camera.y));

	// Give the target a new current position, so that we won't move
	// unless the user pushes the keys.
	SetCameraCurrentPosition(
 							&camera,
					CAMERA_INT_VAL(camera.x), // x
					CAMERA_INT_VAL(camera.y), // y
					camera.z, // z
					angle,
					camera.p, // p
					radius	
 							);

	SetCameraRotatePointRates(
				&camera,
   					 20,    //RadiusRate
				 	  8,    //AngleRate
    				 200,   //HeightRate
    				 -800,  //PitchRate
    				 pAvatar->fContactDistance/2, //MIN_DISTANCE_AWAY,   //MinDist
 				 YOU_DRIVE_DISTANCE_AWAY  //MaxDist
				 );
	
	SetCameraRotatePointTarget(&camera,
	    		  ContactPoint.x,	//pAvatar->mfX(),
		          ContactPoint.y,	//pAvatar->mfY(),
				  pAvatar->mfZ()+(player.h - EYE_HEIGHT), //pAvatar->mfZ()+(pAvatar->mfHeight() - EYE_HEIGHT),
				  angle,
				  radius,
				  0 		//  swoop
				  );
	
	if (!fHighResCombat)
		fMedResAni = TRUE;
}

/* ========================================================================
   Function    - CombatTestCamera
   Description - Test the view from the angle. (Helper fn for the 4 other
   			     position fns. which are now only marginally necessary.)
   			     
   Returns     - Distance to bump wall And if an enemy exists, whether we
                 can see him or not from this possible position.
   ======================================================================== */
static LONG const CombatTestCamera(
	CAvatar const * const pAvatar,
	LONG	const Angle,
	BOOL	*const pbCanSeeEnemy)
{
	FIXED_VECTOR_3D	TempVector;
	PLAYER	TempPlayer;
	LONG BumpDistance;
	
	// Test Right of Friends
	TempVector.dx = DISTANCE_AWAY << PLAYER_FIXEDPT;
	TempVector.dy = 0;
	TempVector.dz = DISTANCE_UP - (pAvatar->mfHeight() - EYE_HEIGHT);
	if (TempVector.dz < 0)
		TempVector.dz = 0;
	Rotate((POINT *)&TempVector, pAvatar->mfAngle() + Angle);
	
	TempPlayer.x = pAvatar->mfX() << PLAYER_FIXEDPT;
	TempPlayer.y = pAvatar->mfY() << PLAYER_FIXEDPT;
	TempPlayer.z = pAvatar->mfZ() + (pAvatar->mfHeight() - EYE_HEIGHT);
	TempPlayer.p = 0;
	TempPlayer.a = pAvatar->mfAngle();
	TempPlayer.Flying = TRUE;
	TempPlayer.w = 16;
	TempPlayer.h = 16;
	TempPlayer.bump = iNOTHING;
	TempPlayer.ThingIndex = fERROR;
	TempPlayer.currentSector = pAvatar->mfGetSector();
	
	CheckLongMove(&TempPlayer,
	          pAvatar->mfAngle()+Angle,
			  DISTANCE_AWAY,
			  0,
			  DISTANCE_UP,
	          &BumpDistance);
	
    switch (TempPlayer.bump)
    {
    case iNOTHING:
	 case iSHALLOW_WATER:
	 case iDEEP_WATER:
	 case iLAVA:
	 case iACID:
    	BumpDistance = DISTANCE_AWAY;
    	break;
    default:
    	BumpDistance = BumpDistance;
    }
    
    if (pAvatar->hEnemy == fERROR)
    {
    	*pbCanSeeEnemy = TRUE;
    }
    else
    {
		CAvatar const * const pEnemyAvatar = (CAvatar const * const) BLKPTR(pAvatar->hEnemy);
		
		TempPlayer.x = pEnemyAvatar->mfX() << PLAYER_FIXEDPT;
		TempPlayer.y = pEnemyAvatar->mfY() << PLAYER_FIXEDPT;
		TempPlayer.z = pEnemyAvatar->mfZ() + (pEnemyAvatar->mfHeight() - EYE_HEIGHT);
		TempPlayer.p = 0;
		TempPlayer.a = pEnemyAvatar->mfAngle();
		TempPlayer.Flying = TRUE;
		TempPlayer.w = 16;
		TempPlayer.h = 16;
		TempPlayer.bump = iNOTHING;
		TempPlayer.ThingIndex = fERROR;
		TempPlayer.currentSector = pEnemyAvatar->mfGetSector();
		
		CheckLongMove(&TempPlayer,
		          pAvatar->mfAngle()+Angle,
				  DISTANCE_AWAY,
				  0,
				  DISTANCE_UP,
		          &BumpDistance);
		
	    switch (TempPlayer.bump)
	    {
	    case iNOTHING:
		 case iSHALLOW_WATER:
		 case iDEEP_WATER:
		 case iLAVA:
		 case iACID:
	    	*pbCanSeeEnemy = TRUE;
	    	break;
	    default:
	    	*pbCanSeeEnemy = FALSE;
	    	break;
	    }
    }
	
	return BumpDistance;
}

/* ========================================================================
   Function    - CombatTestCameraRightOfFriend
   Description - Test the view from the right of the avatar
   Returns     - Distance to bump wall
   ======================================================================== */
static LONG const CombatTestCameraRightOfFriend(
	CAvatar const * const pAvatar,
	BOOL * const pbCanSeeEnemy)
{
	return CombatTestCamera(pAvatar, RIGHT_OF_FRIEND_ANGLE, pbCanSeeEnemy);
}

/* ========================================================================
   Function    - CombatTestCameraLeftOfFriend
   Description - Test the view from the left of the avatar
   Returns     - Distance to bump wall
   ======================================================================== */
static LONG const CombatTestCameraLeftOfFriend(
	CAvatar const * const pAvatar,
	BOOL * const pbCanSeeEnemy)
{
	return CombatTestCamera(pAvatar, LEFT_OF_FRIEND_ANGLE, pbCanSeeEnemy);
}

/* ========================================================================
   Function    - CombatTestCameraBehindFriend
   Description - Test the view from the rear of the avatar
   Returns     - Distance to bump wall
   ======================================================================== */
static LONG const CombatTestCameraBehindFriend(
	CAvatar const * const pAvatar,
	BOOL * const pbCanSeeEnemy)
{
	return CombatTestCamera(pAvatar, BEHIND_FRIEND_ANGLE, pbCanSeeEnemy);
}

/* ========================================================================
   Function    - CombatTestCameraBehindFoe
   Description - Test the view from the front of the avatar looking back at 'em.
   Returns     - Distance to bump wall
   ======================================================================== */
static LONG const CombatTestCameraBehindFoe(
	CAvatar const * const pAvatar,
	BOOL * const pbCanSeeEnemy)
{
	return CombatTestCamera(pAvatar, BEHIND_FOE_ANGLE, pbCanSeeEnemy);
}

/* ========================================================================
   Function    - mfInitializeCamera
   Description - At begining of combat setup the camera position.
   Returns     - 
   ======================================================================== */

void COMBAT_CAMERA::mfInitializeCamera()
{
	// I'm taking over total control of the camera.
	GAME_CAMERA::CAMERA_STATE const PrevCameraState = SCENE_MGR::fgCamera.mfSetCameraState(GAME_CAMERA::PROGRAM_CAMERA_CONTROL);
	
	HDL_AVATAR hLeader = CONTROL_PANELS::mfGetCurrAvatarHdl();
	CAvatar const * const pAvatar = (CAvatar *const ) BLKPTR (hLeader);
	// Did we start combat mode by casting a spell?
	if(pAvatar->fFollowPlayer.fCurrentSpellBox != -1 ||
	   COMBAT_INITIAL_VIEWPT::mfIsModeFirstPerson())
	{
		// Try to keep the camera in the same location.
		if (PrevCameraState == GAME_CAMERA::FOLLOW_BEHIND_PLAYER)
		{
			CombatBehindPersonView(0,0);
		}
		else
		{
			CombatFirstPersonView(0,0);
		}
		gccCamera.mfInitToLeader(hLeader);
	}
	else
	{
		hLeader = ADVENTURER::mfFirstAdventurerIs(&CAvatar::mfAmIBeingAttacked);
		if (hLeader == fERROR)
		{
			// No one under attack yet. Look for spell casters.
			hLeader = ADVENTURER::mfFirstAdventurerIs(&CAvatar::mfAmICastingASpell);
			// Use the one set by the button.
			if (hLeader == fERROR)
			{
				hLeader = CONTROL_PANELS::mfGetCurrAvatarHdl();
			}
		}
		else
		{
			// Find the person under attack and set the camera to them.
			CONTROL_PANELS::mfMatchAvatarHdlToCurrent(hLeader);
		}
		
		gccCamera.mfInitToLeader(hLeader);
		gccCamera.mfInitDelayWanderFree();
	
		if (COMBAT_TARGETTING::mfIsModeTargetting() == TRUE )
		{
			// If we started up the combat by casting a targeting spell
			// keep the camera in place.
			CombatFirstPersonView(0,0);
		}
		else
		{
			switch (gccCamera.mfTestCamera())
			{
			case COMBAT_CAMERA::RIGHT_OF_FRIENDS:
				CombatAutoViewFriendRight(0,0);
				break;
			case COMBAT_CAMERA::BEHIND_FRIENDS:
				CombatAutoViewBehindFriend(0,0);
				break;
			case COMBAT_CAMERA::LEFT_OF_FRIENDS:
				CombatAutoViewFriendLeft(0,0);
				break;
			case COMBAT_CAMERA::BEHIND_FOES:
				CombatAutoViewBehindFoe(0,0);
				break;
			default:
				if (PrevCameraState == GAME_CAMERA::FOLLOW_BEHIND_PLAYER)
				{
					CombatBehindPersonView(0,0);
				}
				else
				{
					CombatFirstPersonView(0,0);
				}
				break;
			}
		}
	}
}
