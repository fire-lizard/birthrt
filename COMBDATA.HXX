/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _COMBDATA_HXX
#define _COMBDATA_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_SYSTEM_H)
#include "system.h"
#endif


#if !defined(_COMBOPTS_HXX)
#include "combopts.hxx"
#endif

#if !defined(_GAME_H)
#include "game.h"
#endif

#if !defined(_GAMECAMR_HXX)
#include "gamecamr.hxx"
#endif

#if !defined(_SCNMGR_HXX)
#include "scnmgr.hxx"
#endif


/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define DEAD_TIMER_WAIT	100
#define COMBAT_TIMES_OUT	100
#define ABSOLUTE_RPG_WAIT	400
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Class Definition
   ------------------------------------------------------------------------ */

// Class to hold previous scene state information.
// GWP I may move this to the scene class so that its available to other
// GWP aspects of the game.
class COMBAT_SCENE_DATA
{
public:
	class RETREAT_DATA {
	public:
		inline void mfInitTimer();
		inline void mfInitVals();
		inline void mfUpdateRetreatTimer();
		inline BOOL const mfIsRetreating() const;
		inline void mfDeAllocateData();
		
		void mfStartRetreat(SHORT const hLeadAvatar,
		                    LONG * const pLastAngle);
		
		BOOL const mfGetNextPt(PLAYER *const pPlayer);
		
	private:
		LONG	fTimer;
		SBYTE	fRetreatFlag;
		SHORT	fhRetreatingPositions;
		SHORT	fCurrentPosition;
		SHORT	fMaxRetreatPts;
	};
	
		   void mfInitSceneVals();
	inline void mfStartDeadTimer();
	inline BOOL const mfTestDeadTimedOut();
	inline BOOL const mfTestDamageTimedOut();
	inline void mfResetDamageTimer();
	inline void mfInitMenuVals();
	inline void mfRestoreMenuVals();
	inline void mfResetMenuVals();
	inline void mfRestoreSceneVals();
		   void mfInitCombatVals();
		   void mfRestoreCombatVals();
	inline void mfInitCombatTimer();
	inline BOOL const mfTestCombatTimedOut();
	inline void mfInitRPGTimer();
	inline BOOL const mfTestRPGTimedOut();
	
	// While the scene AI is paused, the timers must continue to update.
		   void mfPauseUpdateData();
	inline void mfStartRetreat(SHORT const hLeadAvatar);
	inline void mfDeAllocateData();
	
	inline BOOL mfCanGoToHighResArt() const;
	
	
	RETREAT_DATA				fRetreatData;
	
private:


	// No constructor calls.
	COMBAT_SCENE_DATA();
	~COMBAT_SCENE_DATA();
	// No copies, no assignments.
	COMBAT_SCENE_DATA(COMBAT_SCENE_DATA const &);
	COMBAT_SCENE_DATA const operator=(COMBAT_SCENE_DATA const &);
	
	// Menu data
	BOOL						fOldAutoResState;
	BOOL						fOldHighResState;
	LONG						fOldMarginRight;
	LONG						fOldMarginLeft;
	LONG						fOldMarginTop;
	LONG						fOldMarginBottom;
	
	// Previous scene states.
	LONG						fOldPlayerAngle;
	GAME_CAMERA::CAMERA_STATE	fOldCameraState;
	BOOL						fOldEnvSoundState;
	SHORT						fucWhichTrack;
	
	// Combat timers data.
	LONG						fgTickPrev;
	LONG						fCombatTimer;
	LONG						fDeadTimer;
	LONG						fDamageTimer;
	LONG						fRPGTimer;
	LONG						fLiftTaskIndex;
	LONG						fFloorTaskIndex;
	LONG						fCeilingTaskIndex;
		
	PFVLLL						OldInventoryPickupFunc;
	KEYSTRUCT					OldDoorKey;
};
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Inline functions   
   ------------------------------------------------------------------------ */
   
inline void COMBAT_SCENE_DATA::RETREAT_DATA::mfInitTimer()
{
	fTimer = 0;
	fRetreatFlag = FALSE;
}
		
inline void COMBAT_SCENE_DATA::RETREAT_DATA::mfInitVals()
{
	mfInitTimer();
	fhRetreatingPositions = fERROR;
	fCurrentPosition = 0;
	fMaxRetreatPts = 0;
}
		
inline void COMBAT_SCENE_DATA::RETREAT_DATA::mfUpdateRetreatTimer()
{
	fTimer++;
	if (fTimer > 10)
	{
		mfInitTimer();
	}
}
		
inline BOOL const COMBAT_SCENE_DATA::RETREAT_DATA::mfIsRetreating() const
{ 
	return ((fRetreatFlag == TRUE) ? TRUE : FALSE); 
}
		
inline void COMBAT_SCENE_DATA::RETREAT_DATA::mfDeAllocateData()
{
	if (fhRetreatingPositions != fERROR)
	{
		DisposBlock(fhRetreatingPositions);
		fhRetreatingPositions = fERROR;
	}
}
		
inline void COMBAT_SCENE_DATA::mfStartDeadTimer()
{
	fDeadTimer = SCENE_MGR::gTick + DEAD_TIMER_WAIT;
	fgTickPrev = SCENE_MGR::gTick;
}
	
inline BOOL const COMBAT_SCENE_DATA::mfTestDeadTimedOut()
{
	fgTickPrev = SCENE_MGR::gTick;
	return (fDeadTimer < SCENE_MGR::gTick) ? TRUE : FALSE;
}
	
inline	BOOL const COMBAT_SCENE_DATA::mfTestDamageTimedOut()
{
	fgTickPrev = SCENE_MGR::gTick;
	return (fDamageTimer < SCENE_MGR::gTick) ? TRUE : FALSE;
}
	
inline void COMBAT_SCENE_DATA::mfResetDamageTimer()
{
	// GWP Put the slow down number here.
	fDamageTimer = SCENE_MGR::gTick + COMBAT_ARCADE_SPEED::mfGetDelayValue();
}
	
inline void COMBAT_SCENE_DATA::mfInitMenuVals()
{
	get_margin_size(&fOldMarginRight,
	                &fOldMarginLeft,
	                &fOldMarginTop,
	                &fOldMarginBottom);
}

inline void COMBAT_SCENE_DATA::mfRestoreMenuVals()
{
	// reset the margin size
	set_margin_size(fOldMarginRight,
	                fOldMarginLeft,
	                fOldMarginTop,
	                fOldMarginBottom);
}
	
inline void COMBAT_SCENE_DATA::mfResetMenuVals()
{
	SHORT Top;
	if(fChatLineOn && fOldMarginTop < CHAT_DISPLAY_HEIGHT)
		fOldMarginTop = CHAT_DISPLAY_HEIGHT;
		
	// reset the margin size
	set_margin_size(fOldMarginRight,
	                fOldMarginLeft,
	                fOldMarginTop,
	                fOldMarginBottom);
}
	
inline void COMBAT_SCENE_DATA::mfRestoreSceneVals()
{
	// Point you back in the direction you were headed.
	player.a = fOldPlayerAngle;
	PlayEnvironmentalSounds = fOldEnvSoundState;
	SCENE_MGR::fgCamera.mfSetCameraState(fOldCameraState);
}
	
inline void COMBAT_SCENE_DATA::mfInitCombatTimer()
{
	fCombatTimer = SCENE_MGR::gTick + COMBAT_TIMES_OUT;	// 10 seconds till time out.
	fgTickPrev = SCENE_MGR::gTick;
}

inline BOOL const COMBAT_SCENE_DATA::mfTestCombatTimedOut()
{
	fgTickPrev = SCENE_MGR::gTick;
	return (fCombatTimer < SCENE_MGR::gTick) ? TRUE : FALSE;
}

inline void COMBAT_SCENE_DATA::mfInitRPGTimer()
{
	fRPGTimer = SCENE_MGR::gTick + ABSOLUTE_RPG_WAIT;	// 10 seconds till time out.
	fgTickPrev = SCENE_MGR::gTick;
}

inline BOOL const COMBAT_SCENE_DATA::mfTestRPGTimedOut()
{
	fgTickPrev = SCENE_MGR::gTick;
	return (fRPGTimer < SCENE_MGR::gTick) ? TRUE : FALSE;
}

inline void COMBAT_SCENE_DATA::mfStartRetreat(SHORT const hLeadAvatar)
{
	if (fRetreatData.mfIsRetreating() == FALSE)
	{
		fRetreatData.mfStartRetreat(hLeadAvatar, &fOldPlayerAngle);
	}
}

inline void COMBAT_SCENE_DATA::mfDeAllocateData()
{
	fRetreatData.mfDeAllocateData();
}

#endif // _COMBDATA_HXX
