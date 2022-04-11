/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _GAMECAMR_HXX
#define _GAMECAMR_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_CAMERA_H)
#include "camera.h"
#endif

#if !defined(_PLAYER_HXX)
#include "player.hxx"
#endif

#if !defined(_SYSTEM_H)
#include "system.h"
#endif

#if !defined(_ENGINT_H)
#include "engint.h"
#endif

#if !defined(_PLAYER_HXX)
#include "player.hxx"
#endif

#if !defined(_SOUND_HXX)
#include "sound.hxx"
#endif


/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define EYE_HEIGHT				4
#define FOLLOWING_DISTANCE		60
#define FOLLOWING_CAMERA_WIDTH	12
#define FOLLOWING_CAMERA_ABOVE	10
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
// This class controls the mode the camera is in, whether it follows the
// player or wanders freely.
class GAME_CAMERA
{
public:
	typedef enum {
		FOLLOW_PLAYER,
		WANDER_FREELY,
		PROGRAM_CAMERA_CONTROL,
		FOLLOW_BEHIND_PLAYER
	} CAMERA_STATE;
	
	GAME_CAMERA();
	~GAME_CAMERA() {};
	
	void mfInitVals();
	CAMERA_STATE const mfSetCameraState(CAMERA_STATE const);
	void mfUpdateCamera();
	CAMERA_STATE mfGetCurrentState();
protected:
private:
	CAMERA_STATE	fCameraState;
};
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   inline functions
   ------------------------------------------------------------------------ */
// Data initialization routine.
inline void GAME_CAMERA::mfInitVals()
{
	InitializeCamera(&camera, NO_CHANGE, NO_CHANGE, NO_CHANGE,
	                 NO_CHANGE, NO_CHANGE);
	fCameraState = FOLLOW_PLAYER;
}

// Constructor.
inline GAME_CAMERA::GAME_CAMERA()
{
	mfInitVals();
}

// Change the camera state
inline GAME_CAMERA::CAMERA_STATE const GAME_CAMERA::mfSetCameraState(
	GAME_CAMERA::CAMERA_STATE const newState)
{
	CAMERA_STATE const PrevState = fCameraState;
	fCameraState = newState;
	return PrevState;
}

// Called by the AI to move the camera around.
inline void GAME_CAMERA::mfUpdateCamera()
{
	LONG LightValue;
	
	switch(fCameraState)
	{
	case FOLLOW_PLAYER:
		// [d2-21-97 JPC] Added the z code for crushers.
		// Note that player.ceiling is not always correct!
		SHORT	z;
		SHORT	ceiling;

		z = player.z + (player.h - EYE_HEIGHT);
		ceiling = sectors[player.currentSector].ch;
		if (z > ceiling - EYE_HEIGHT)
			z = ceiling - EYE_HEIGHT;

		SetCameraPosition(
			&camera,
			PLAYER_INT_VAL(player.x),
			PLAYER_INT_VAL(player.y),
			z,
			player.a,
			player.p
			);
		
		break;
	
	case WANDER_FREELY:
		MoveCamera(&camera, CheckMoveKeys());
		break;
	
	case PROGRAM_CAMERA_CONTROL:
		MoveCamera(&camera, 0);
		break;
		
	case FOLLOW_BEHIND_PLAYER:
		{
		FIXED_VECTOR_3D ptBehind;
		LONG BumpDistance = 0;
		PLAYER TempPlayer;
		
		TempPlayer.x = player.x;
		TempPlayer.y = player.y;
		TempPlayer.z = player.z;
		TempPlayer.p = player.p;
		TempPlayer.a = player.a;
		TempPlayer.Flying = FALSE;
		TempPlayer.w = FOLLOWING_CAMERA_WIDTH;
		TempPlayer.h = FOLLOWING_CAMERA_WIDTH;
		TempPlayer.bump = iNOTHING;
		TempPlayer.ThingIndex = fERROR;
		
		ptBehind.dx = 0; // To left slightly(-player.w) << PLAYER_FIXEDPT;
		ptBehind.dy = (-(FOLLOWING_CAMERA_WIDTH + FOLLOWING_DISTANCE)) << PLAYER_FIXEDPT;
		ptBehind.dz = player.h + FOLLOWING_CAMERA_ABOVE;
		Rotate((POINT *)&ptBehind, player.a);
		
		CheckMoveSimple(&TempPlayer, &ptBehind, 0, &BumpDistance);
		
		// Note: Bump distance is a squared value. Rather than get the
		//       sqrt, I square the comparison value.
		if ((TempPlayer.bump == iWALL  ||
		     TempPlayer.bump == iFLOOR ||
		     TempPlayer.bump == iCEILING) && BumpDistance < ((FOLLOWING_DISTANCE + FOLLOWING_CAMERA_WIDTH) * (FOLLOWING_DISTANCE + FOLLOWING_CAMERA_WIDTH)))
		{
			SetCameraPosition(
				&camera,
				PLAYER_INT_VAL(player.x),
				PLAYER_INT_VAL(player.y),
				(player.z+(player.h - EYE_HEIGHT)),
				player.a,
				player.p
				);
		}
		else
		{
			// GWP Bob and weave follow camera.
			// GWP It gets stuck by walls and doesn't keep up.
			// GWP SetCameraRotateTarget(
			// GWP 	&camera,
			// GWP 	PLAYER_INT_VAL(player.x),
			// GWP 	PLAYER_INT_VAL(player.y),
			// GWP 	player.z + ptBehind.dz,
			// GWP 	player.p - 5,
			// GWP 	player.a + 128,
			// GWP 	player.w,
			// GWP 	100,
			// GWP 	TRUE
			// GWP 	);
			// GWP MoveCamera(&camera, 0);
				
			SetCameraPosition(
				&camera,
				(PLAYER_INT_VAL(player.x + ptBehind.dx)),
				(PLAYER_INT_VAL(player.y + ptBehind.dy)),
				(player.z + ptBehind.dz),
				player.a,
				(player.p)
				);
			
		}
		}
		break;
	}
	
	if (fCameraState == FOLLOW_PLAYER &&
		player.ThingIndex != fERROR)
	{
	   	LightValue = sector_to_light(mythings[player.ThingIndex].sect);
	}
	else
	{
		LightValue = sector_to_light(point_to_sector(CAMERA_INT_VAL(camera.x),
		                                             CAMERA_INT_VAL(camera.y)));
	}
	
	PlayEnvironmentalSounds = (LightValue == 0) ? FALSE : TRUE;
}

// Called by the self running demo to remember the initial state.
// And combatui
inline GAME_CAMERA::CAMERA_STATE GAME_CAMERA::mfGetCurrentState()
{
	return fCameraState;
}
#endif // _GAMECAMR_HXX

