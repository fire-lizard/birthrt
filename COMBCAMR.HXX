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

#if !defined(_TYEPDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_AVATAR_HXX)
#include "avatar.hxx"
#endif

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

#define WAIT_FOR_FIRST_CAMERA_LOC 10

#define TARGET_FACTOR	210

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
   
/* ------------------------------------------------------------------------
   Classes
   ------------------------------------------------------------------------ */

// Class to hold information about the camera for the combat ui.
// Decides where the camera should be in relation to the leader.
class COMBAT_CAMERA 
{
public:
	typedef enum {
		FOLLOW_AVATAR,
		FACING_BACKWARDS,
		BEHIND_FRIENDS,
		BEHIND_FOES,
		LEFT_OF_FRIENDS,
		RIGHT_OF_FRIENDS,
		WANDER_FREELY,
		BEHIND_AVATAR
	} CAMERA_STATE;

	inline COMBAT_CAMERA();
	~COMBAT_CAMERA() {}
	
	void mfInitializeCamera();
	
	// Updates the camera position.
	void mfUpdatePosition();
	
	// Call to change leaders.
	void mfSetNewLeader(SHORT const /* hAvatar */);
	
	SHORT const mfGetCurrentLeader() const 
	{	return fhLeader; }
	
	void mfSetState(CAMERA_STATE const cstate)
	{ fCurrentState = cstate; }
	
protected:
private:
	// Called at the beginning
	inline void mfInitToLeader(SHORT const /* hAvatar */);
	
	// GWP HACK To get the user to stop leaning on the now camera only
	// GWP HACK control keys. I disable the keys for a period of ticks.
	inline void mfUpdateWanderFlag();
	
	CAMERA_STATE mfTestCamera() const;
	void mfPickNewCameraState(CAvatar const * const /* pLeader */);
	
	// Test if the leader's position has changed.
	inline BOOL const mfDidLeaderMove(CAvatar const * const /* pLeader */) const;
	
	inline void mfUpdateLeaderPosition(CAvatar const * const /* pLeader */);
	
	// Called at the beginning of the combat UI.
	void mfInitDelayWanderFree()
	{
		fWanderFreelyAllowed = FALSE;
		fWanderTickCount = 0;
	}
	
	// No copies or assignments
	COMBAT_CAMERA(COMBAT_CAMERA const&);
	COMBAT_CAMERA const operator=(COMBAT_CAMERA const&);
	
	CAMERA_STATE fCurrentState;
	SHORT		 fhLeader;			// The handle of the avatar the camera is
	                                // key'ing off of.
	LONG		 fLeaderOrigAngle;
	LONG		 fLeaderOrigX;
	LONG		 fLeaderOrigY;
	
	BOOL		 fWanderFreelyAllowed;
	LONG		 fWanderTickCount;
};

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
extern COMBAT_CAMERA gccCamera;

/* ------------------------------------------------------------------------
   Local Variables
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
void CombatFirstPersonView(LONG, LONG);
void CombatBehindPersonView(LONG, LONG);
void CombatAutoViewBehindFriend(LONG, LONG);
void CombatAutoViewBehindFoe(LONG, LONG);
void CombatAutoViewFriendRight(LONG, LONG);
void CombatAutoViewFriendLeft(LONG, LONG);

// Default constructor
inline COMBAT_CAMERA::COMBAT_CAMERA() :
	fCurrentState(FOLLOW_AVATAR),	// So we don't auto remove Hi Res.
	fhLeader(fERROR),
	fLeaderOrigAngle(0),
	fLeaderOrigX(0),
	fLeaderOrigY(0),
	fWanderFreelyAllowed(FALSE)
	 { }
	
// Update the leaders current position so we can tell if he moved.
inline void COMBAT_CAMERA::mfUpdateLeaderPosition(CAvatar const * const pLeader)
{
	fLeaderOrigAngle = pLeader->mfAngle();
	fLeaderOrigX	 = pLeader->mfX();
	fLeaderOrigY	 = pLeader->mfY();
}
	
// Called at the beginning
inline void COMBAT_CAMERA::mfInitToLeader(SHORT const hAvatar)
{
	if (hAvatar != fERROR)
	{
		mfSetNewLeader(hAvatar);
		CAvatar const * const pLeader = (CAvatar const * const) BLKPTR(fhLeader);
		mfUpdateLeaderPosition(pLeader);
	}
}

// GWP HACK To get the user to stop leaning on the now camera only
// GWP HACK control keys. I disable the keys for a period of Frame counts.
inline void COMBAT_CAMERA::mfUpdateWanderFlag()
{
	if (fWanderTickCount < WAIT_FOR_FIRST_CAMERA_LOC)
	{
		fWanderTickCount++;
	}
	else
	{
		fWanderFreelyAllowed = TRUE;
	}
}
	
// Test if the leader's position has changed.
inline BOOL const COMBAT_CAMERA::mfDidLeaderMove(CAvatar const * const pLeader) const
{
	BOOL Result = FALSE;
	LONG DeltaAngle = pLeader->mfAngle() - fLeaderOrigAngle;
	LONG DeltaX = pLeader->mfX() - fLeaderOrigX;
	LONG DeltaY = pLeader->mfY() - fLeaderOrigY;
	
	if (DeltaAngle < 0)
	{
		DeltaAngle += 256;
	}
	DeltaX = ABS(DeltaX);
	DeltaY = ABS(DeltaY);
	
	// We allow a slight amount of movement before
	// we react and adjust the camera.
	if ((DeltaAngle > 5) ||
	    (DeltaX > 5 ) ||
	    (DeltaY > 5 ) )
	{
		Result = TRUE;
	}
	return Result;
}
