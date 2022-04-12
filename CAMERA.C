/* ========================================================================
   Copyright (c) 1990,1995 Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: CAMERA.C	- handles indoor and outdoor cameras
   Author: 	Chris Phillips & Greg Hightower & Gary Powell
   ========================================================================

   Contains the following general functions:
   push_camera				-pushes the camera onto the camera stack
   pull_camera				-pulls a camera from the stack

   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>

#if defined (_STATUS)
#include "debug.h"
#include "engint.h"
#endif
#include "SYSTEM.H"
#include "ENGINE.H"
#include "MACHINE.H"

#if defined (_STATUS)
#include "winsys\status.h"
void WriteDebug (const char *format, ... );
extern BOOL gfCheckCameraMove;
#else
	#define WriteDebug 1?0:printf
#endif


/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
// UNUSED #define CAMERA_STACK_SIZE 20

#define INITIAL_TARGET_FACTOR	105
#define DEFAULT_LINEAR_RATE		20
#define DEFAULT_ANGLE_RATE     4
#define DEFAULT_PITCH_RATE     4
#define DEFAULT_HEIGHT_RATE    10
#define MAX_HEIGHT				-100
#define MAX_PITCH				200
#define MAX_A_SPEED				11   //prime
#define MAX_P_SPEED				19   //prime
#define MAX_XY_SPEED			(61 << CAMERA_FIXEDPT)   //prime
#define MAX_Z_SPEED				19   //prime

// [d12-06-96 JPC] Defined two camera widths.  CAMERA_WIDTH_NARROW is for
// when the person playing the game is moving the camera--then it's possible
// to get through doors.  CAMERA_WIDTH_WIDE is for camera auto-move,
// to prevent moving the camera into narrow places where it gets stuck.
#define CAMERA_WIDTH_NARROW	12			// (JPC) was 16
#define CAMERA_WIDTH_WIDE		50
#define CAMERA_HEIGHT			16	

#define DEFAULT_CAMERA_Z		64			// [d12-09-96 JPC]

enum {
	NO_TARGET_REQUESTED,
	SWOOP_CAMERA,
	MOVE_CAMERA_EVENLY,
	ROTATE_SWOOP_CAMERA,
	ROTATE_CAMERA_EVENLY,
	ROTATE_CAMERA_AROUND_POINT
};
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
CAMERA camera;	// used by the engine to do the main view.
// UNUSED CAMERA camera_stack[CAMERA_STACK_SIZE];
LONG camera_stack_ptr=0;

extern BOOL fAIAutoRes;

#if defined (_STATUS)
char gszTemp[80];
#endif



// [d12-09-96 JPC] Some debugging functions.

#if defined (_STATUS)
int	gDebug = 0;
/* =======================================================================
   Function    - DebugCameraMove
   Description - Checks whether proposed move is OK.
   Returns     - TRUE if move is OK
   ======================================================================== */
BOOL DebugCameraMove (CAMERA *pCamera, FIXED_VECTOR_3D NewPoint)
{
	LONG 			iOldSector;
	LONG 			iNewSector;
	LONG			x, y, z;

	x = pCamera->x >> CAMERA_FIXEDPT;
	y = pCamera->y >> CAMERA_FIXEDPT;
	iOldSector = point_to_sector (x, y);

	x = (pCamera->x + NewPoint.dx) >> CAMERA_FIXEDPT;
	y = (pCamera->y + NewPoint.dy) >> CAMERA_FIXEDPT;
	iNewSector = point_to_sector (x, y);

	if (iOldSector == 321 && iNewSector != 321)
		gDebug++;

	z = pCamera->z + NewPoint.dz;
	if (z < sectors[iNewSector].fh || z > sectors[iNewSector].ch - 16)
		return FALSE;
	return TRUE;
}


/* =======================================================================
   Function    - ShowCameraPosition
   Description - Updates status bar with camera position.
   Returns     - Nothing
   ======================================================================== */
void ShowCameraPosition (CAMERA *pCamera)
{
	// LONG 			iSector;
	LONG			x, y;

	if (!fGraphInitialized)
		return;

	x = pCamera->x >> CAMERA_FIXEDPT;
	y = pCamera->y >> CAMERA_FIXEDPT;
	// iSector = point_to_sector (x, y);

   wsprintf (gszTemp, "X %d", x);
   SetWindowText (ghwndSBx, gszTemp);
   wsprintf (gszTemp, "Y %d", y);
   SetWindowText (ghwndSBy, gszTemp);
   wsprintf (gszTemp, "Z %d", pCamera->z);
   SetWindowText (ghwndSBz, gszTemp);
   wsprintf (gszTemp, "A %d", pCamera->a);
   SetWindowText (ghwndSBa, gszTemp);
   wsprintf (gszTemp, "P %d", pCamera->p);
   SetWindowText (ghwndSBp, gszTemp);
//    wsprintf (gszTemp, "S %d",iSector);
//    SetWindowText (ghwndSBs, gszTemp);
}
#endif

/*
	NOTE: See engine.h for macro cover functions to change each
	of the camera elements individually
*/

/* =======================================================================
   Function    - InitializeCamera
   Description - Set all the vars for a new scene.
   Returns     - void
   ======================================================================== */
void InitializeCamera(
	CAMERA *pCamera,
	const LONG	X,
	const LONG	Y,
	const LONG	Z,
	const LONG	A,
	const LONG	P
)
{
	SetCameraPosition(pCamera, X, Y, Z, A, P);
	pCamera->Rate[0] = DEFAULT_LINEAR_RATE;
	pCamera->Rate[1] = DEFAULT_HEIGHT_RATE;
	pCamera->Rate[2] = DEFAULT_ANGLE_RATE;
	pCamera->Rate[3] = DEFAULT_PITCH_RATE;
	pCamera->TargetFactor = INITIAL_TARGET_FACTOR;
}

/* =======================================================================
   Function    - SetCameraPosition
   Description - sets the position of the active camera
   Returns     - void
   ======================================================================== */
void SetCameraPosition(
	CAMERA *pCamera,
	const LONG x,
	const LONG y,
	const LONG z,
	const LONG a,
	const LONG p)
{
	pCamera->TargetRequested = NO_TARGET_REQUESTED;
	if(x != NO_CHANGE)
	{
		pCamera->Current.x = pCamera->x = x<<CAMERA_FIXEDPT;
	}
	if(y != NO_CHANGE)
	{
		pCamera->Current.y = pCamera->y = y<<CAMERA_FIXEDPT;
	}
	if(z != NO_CHANGE)
	{
		pCamera->Current.z = pCamera->z = z;
	}
	if(a != NO_CHANGE)
	{
		pCamera->Current.a = pCamera->a = a % 256;
	}
	if(p != NO_CHANGE)
	{
		pCamera->Current.p = pCamera->p = p;
	}
#if defined (_STATUS)
	ShowCameraPosition (pCamera);
#endif
}

/* =======================================================================
   Function    - SetCameraCurrentPosition
   Description - sets the current position of the active camera relative to target
   Returns     - void
   ======================================================================== */
void SetCameraCurrentPosition(
	CAMERA *pCamera,
	const LONG x,
	const LONG y,
	const LONG z,
	const LONG angle,
	const LONG p,
	const LONG radius)
{
	if(x != NO_CHANGE)
	{
		pCamera->Current.x = x<<CAMERA_FIXEDPT;
	}
	if(y != NO_CHANGE)
	{
		pCamera->Current.y = y<<CAMERA_FIXEDPT;
	}
	if(z != NO_CHANGE)
	{
		pCamera->Current.z = z;
	}
	if(angle != NO_CHANGE)
	{
		pCamera->Current.a = angle % 256;	  // angle from target to camera
	}
	if(p != NO_CHANGE)
	{
		pCamera->Current.p = p;
	}
	if(radius != NO_CHANGE)
	{
		pCamera->Current.radius = radius;	 //distance between target and camera
	}
}
	


/* =======================================================================
   Function    - SetCameraTarget
   Description - Request the position of the active camera
   Returns     - void
   ======================================================================== */
void SetCameraTarget(
	CAMERA *pCamera,
	const LONG Targetx,
	const LONG Targety,
	const LONG Targetz,
	const LONG Targeta,
	const LONG Targetp,
	const BOOL swoop)
{
	pCamera->TargetRequested = (swoop == TRUE) ? SWOOP_CAMERA : MOVE_CAMERA_EVENLY;
	pCamera->TargetFactor = INITIAL_TARGET_FACTOR;
	
	if(Targetx != NO_CHANGE)
		pCamera->LTarget.x=Targetx;
	if(Targety != NO_CHANGE)
		pCamera->LTarget.y=Targety;
	if(Targetz != NO_CHANGE)
		pCamera->LTarget.z=Targetz;
	if(Targeta != NO_CHANGE)
		pCamera->LTarget.a=Targeta & 0xFF;
	if(Targetp != NO_CHANGE)
		pCamera->LTarget.p=Targetp;
}
/* =======================================================================
   Function    - SetCameraRotateTarget
   Description - For rotation of the camera about a point.
   Returns     - void
   ======================================================================== */
void SetCameraRotateTarget(
	CAMERA *pCamera,
	const LONG TargetPtX,
	const LONG TargetPtY,
	const LONG NewZ,
	const LONG NewPitch,
	const LONG Angle,  // Of polar point (a,finalDistance) translated to TargetPtX,Y
	const LONG MinRadius,
	const LONG FinalDistance,
	const BOOL swoop)
{
	pCamera->TargetRequested = (swoop == TRUE) ? ROTATE_SWOOP_CAMERA : ROTATE_CAMERA_EVENLY;
	//pCamera->TargetFactor = INITIAL_TARGET_FACTOR;
	
	pCamera->RTarget.x = TargetPtX;
	pCamera->RTarget.y = TargetPtY;
	if (NewZ == NO_CHANGE)
	{
		pCamera->RTarget.z = pCamera->z;
	}
	else
	{
		pCamera->RTarget.z = NewZ;
	}
	
	if (NewPitch == NO_CHANGE)
	{
		pCamera->RTarget.p = pCamera->p;
	}
	else
	{
		pCamera->RTarget.p = NewPitch;
	}
	
	pCamera->RTarget.a = Angle % 256;
	
	if (MinRadius == NO_CHANGE)
	{
		const LONG distance = dist(TargetPtX, TargetPtY,
									pCamera->x, pCamera->y);
		pCamera->RTarget.radius = ABS(distance);
	}
	else
	{
		pCamera->RTarget.radius = ABS(MinRadius);
	}
	
	if (FinalDistance == NO_CHANGE)
	{
		const LONG distance = dist(TargetPtX, TargetPtY,
									pCamera->x, pCamera->y);
		pCamera->RTarget.distance = ABS(distance);
	}
	else
	{
		pCamera->RTarget.distance = ABS(FinalDistance);
	}
}

/* =======================================================================
   Function    - SetCameraRotatePointTarget
   Description - For rotation of the camera about a point.
   Returns     - void
   ======================================================================== */
void SetCameraRotatePointTarget(
	CAMERA *pCamera,
	const LONG X,
	const LONG Y,
	const LONG Z,
	const LONG Angle,
	const LONG Radius,
	const BOOL swoop)
{
	pCamera->TargetRequested = ROTATE_CAMERA_AROUND_POINT;
	
	if (X != NO_CHANGE)
		pCamera->RTarget.x = X<<CAMERA_FIXEDPT;
	
	if (Y != NO_CHANGE)
		pCamera->RTarget.y = Y<<CAMERA_FIXEDPT;
	
	if (Angle != NO_CHANGE)
		pCamera->RTarget.a = Angle % 256;
	
	if (Radius != NO_CHANGE)
		pCamera->RTarget.radius = Radius;

	if (Z != NO_CHANGE)
		pCamera->RTarget.z = Z;
	
}

/* =======================================================================
   Function    - SetCameraRotatePointRates
   Description - set the rotate around point movement rates
   Returns     - void
   ======================================================================== */
void SetCameraRotatePointRates(
	CAMERA *pCamera,
	const LONG RadiusRate,
	const LONG AngleRate,
	const LONG HeightRate,
	const LONG PitchRate,
	const LONG MinDist,
	const LONG MaxDist )
{
	pCamera->Rate[0] = RadiusRate;
	pCamera->Rate[1] = HeightRate;
	pCamera->Rate[2] = AngleRate;
	pCamera->Rate[3] = PitchRate;
	pCamera->RTarget.distance = MinDist;
	pCamera->RTarget.max_distance = MaxDist;
	
}

/* =======================================================================
   Function    - SetCameraMotionVars
   Description - Change the camera motion paramers.
   Returns     - void
   ======================================================================== */
void SetCameraMotionVars(
	CAMERA *pCamera,
	const LONG	TargetFactor,
	const LONG 	LinearRate)
{
	if (TargetFactor != NO_CHANGE)
		pCamera->TargetFactor = TargetFactor;

	if (LinearRate != NO_CHANGE)
		pCamera->Rate[0] = LinearRate;
}

/* =======================================================================
   Function    - MoveCamera
   Description - If a target location has been requested move towards it.
   				 Note: No bump checking of things is done, just walls.
   Returns     - void
   ======================================================================== */
void MoveCamera(CAMERA *pCamera, const LONG KeyState)
{
#if defined (_STATUS)
		gfCheckCameraMove = TRUE;
#endif

	switch (pCamera->TargetRequested)
	{
	case SWOOP_CAMERA:
		SwoopCamera(pCamera);
		break;
	case MOVE_CAMERA_EVENLY:
		MoveCameraEvenly(pCamera);
		break;
	case ROTATE_SWOOP_CAMERA:
		RotateSwoopCamera(pCamera);		// individual combat
		break;
	case ROTATE_CAMERA_EVENLY:
		RotateCameraEvenly(pCamera);
		break;
	case ROTATE_CAMERA_AROUND_POINT:
		RotateCameraAroundPoint(pCamera, KeyState); // battle and individual combat
		break;
	case NO_TARGET_REQUESTED:
		{
		LONG force_x = 0;
		LONG force_y = 0;
		LONG force_z = 0;
		LONG force_a = 0;
		LONG force_p = 0;
		
		/* -----------------------------------------------------------------
		   Create movement force vectors
		   ----------------------------------------------------------------- */

		/* ---------------------------------------------------------------- */
		/* forward/backward */
		if (KeyState & KEY_FORWARD_BIT )
		{
			force_y += MAX_XY_SPEED;
		}
		if (KeyState & KEY_BACKWARD_BIT)
		{
			force_y -= MAX_XY_SPEED;
		}

		/* ---------------------------------------------------------------- */
		/* slide left/right */
		if (KeyState & KEY_SLIDELEFT_BIT)
		{
			force_x -= MAX_XY_SPEED;
		}
		if (KeyState & KEY_SLIDERIGHT_BIT)
		{
			force_x += MAX_XY_SPEED;
		}
		
		/* ---------------------------------------------------------------- */
		/* climb up */
		if (KeyState & KEY_UP_BIT)
		{
			force_z += MAX_Z_SPEED;
		}
		else
		/* climb down */
		if (KeyState & KEY_DOWN_BIT)
		{
			force_z -= MAX_Z_SPEED;
		}
			
		/* ---------------------------------------------------------------- */
		/* rotate left/right */
		if (KeyState & KEY_LEFT_BIT)
		{
			force_a -= MAX_A_SPEED;
		}
		if (KeyState & KEY_RIGHT_BIT)
		{
			force_a += MAX_A_SPEED;
		}
		
		/* ---------------------------------------------------------------- */
		/* Pitch */
		if (KeyState & KEY_LOOKUP_BIT)
		{
			if (pCamera->p < MAX_PITCH)
				force_p += MAX_P_SPEED;
		}
		else
		if (KeyState & KEY_LOOKDOWN_BIT)
		{
			if (pCamera->p > -MAX_PITCH)
				force_p -= MAX_P_SPEED;
		}
		
		pCamera->p += force_p;
		if (pCamera->p < -MAX_PITCH)
		{
			pCamera->p = -MAX_PITCH;
		}
		else
		if (pCamera->p > MAX_PITCH)
		{
			pCamera->p = MAX_PITCH;
		}
		
		/* -----------------------------------------------------------------
		   Now put into effect the force and accel/decel
		   ----------------------------------------------------------------- */

		if (ABS(pCamera->vx) > ABS(force_x))
		{
			// decel in the x
			LONG temp = (pCamera->vx - force_x) / 2;
			if(temp == 0)
				pCamera->vx = 0;
			else
				pCamera->vx -= temp;
		}
		else
		if (ABS(pCamera->vx) < ABS(force_x))
		{
			// accel in the x
			pCamera->vx += (force_x - pCamera->vx) / 5;
			if (ABS(pCamera->vx) > ABS(force_x))
				pCamera->vx = force_x;
		}
		
		/* ---------------------------------------------------------------- */
		if (ABS(pCamera->vy) > ABS(force_y))
		{
			// decel in the y
			LONG temp = (pCamera->vy - force_y) / 2;
			if(temp == 0)
				pCamera->vy = 0;
			else
				pCamera->vy -= temp;
		}
		else
		if (ABS(pCamera->vy) < ABS(force_y))
		{
			// accel in the y
			pCamera->vy += (force_y - pCamera->vy) / 5;
			if (ABS(pCamera->vy) > ABS(force_y))
				pCamera->vy = force_y;
		}
		
		/* ---------------------------------------------------------------- */
		if (ABS(pCamera->vz) > ABS(force_z))
		{
			// decel in the z
			LONG temp = (pCamera->vz - force_z) / 2;
			if(temp == 0)
				pCamera->vz = 0;
			else
				pCamera->vz -= temp;
		}
		else
		if (ABS(pCamera->vz) < ABS(force_z))
		{
			// accel in the z
			pCamera->vz += (force_z - pCamera->vz) / 5;
			if (ABS(pCamera->vz) > ABS(force_z))
				pCamera->vz = force_z;
		}
			
		/* ---------------------------------------------------------------- */
		// now rotate and add the new vector
		if(pCamera->vx != 0 ||
		   pCamera->vy != 0 ||
		   pCamera->vz != 0
		  )
		{
			FIXED_VECTOR_3D NewVector;
			
			//NewVector.dx = pCamera->vx << PLAYER_FIXEDPT;
			//NewVector.dy = pCamera->vy << PLAYER_FIXEDPT;
			NewVector.dx = pCamera->vx;
			NewVector.dy = pCamera->vy;
			NewVector.dz = pCamera->vz;
			Rotate((POINT *)&NewVector, pCamera->a);
			
			if(KeyState & KEY_WALKTHRUWALL_BIT)
			{
				pCamera->x += NewVector.dx;
				pCamera->y += NewVector.dy;
				pCamera->z += NewVector.dz;
			}
			else
			{
				PLAYER TempPlayer;
				LONG BumpDistance;
				
				//TempPlayer.x = pCamera->x << PLAYER_FIXEDPT;
				//TempPlayer.y = pCamera->y << PLAYER_FIXEDPT;
				TempPlayer.x = pCamera->x;
				TempPlayer.y = pCamera->y;
				TempPlayer.z = pCamera->z;
				TempPlayer.w = CAMERA_WIDTH_NARROW;
				TempPlayer.h = CAMERA_HEIGHT;
				TempPlayer.a = pCamera->a;
				TempPlayer.p = pCamera->p;
				TempPlayer.Flying = TRUE;
				TempPlayer.ThingIndex = fERROR;
				TempPlayer.bump = iNOTHING;
				TempPlayer.BumpIndex = fERROR;
				
				CheckMoveSimple(&TempPlayer, &NewVector, 0, &BumpDistance);
				// NewVector.dx << PLAYER_FIXEDPT;
				// NewVector.dy << PLAYER_FIXEDPT;
				
				switch(TempPlayer.bump)
				{
				case iNOTHING:
					pCamera->x += NewVector.dx;
					pCamera->y += NewVector.dy;
					pCamera->z += NewVector.dz;
					break;
				case iSLIDE_ON_WALL:				//lan Should this line go with iNOTHING??
				case iWALL:
					pCamera->z += NewVector.dz;
					break;
				case iFLOOR:
				case iCEILING:
					//pCamera->x += NewVector.dx;
					//pCamera->y += NewVector.dy;
					pCamera->z += NewVector.dz;
					break;
					
				}
			}
		}
		
		/* ---------------------------------------------------------------- */
		if (ABS(pCamera->va) > ABS(force_a))
		{
			// decel in the a
			LONG temp = (pCamera->va - force_a) / 2;
			if(temp == 0)
				pCamera->va = 0;
			else
				pCamera->va -= temp;
		}
		else
		if (ABS(pCamera->va) < ABS(force_a))
		{
			// accel in the a
			pCamera->va += (force_a - pCamera->va) / 5;
			if (ABS(pCamera->va) > ABS(force_a))
				pCamera->va = force_a;
		}
		pCamera->a += pCamera->va;
		pCamera->a = (pCamera->a%256);
		if (pCamera->a<0)
			pCamera->a+=256;

#if defined (_WINDOWS)
// [d11-12-96 JPC] Only do autores in Windows; it's no help in DOS and
// causes more bugs.
		if( fAIAutoRes )
		{
			if( force_x || force_y || force_z || force_a || force_p)
			{
				set_lowres(0L, 0L);
			}
			else
			{
				set_hires(0L, 0L);
			}
		}
#endif		
		}
		break;
	}
#if defined (_STATUS)
	gfCheckCameraMove = FALSE;
	ShowCameraPosition (pCamera);
#endif
}
/* =======================================================================
   Function    - SwoopCamera
   Description - Swoop the camera
   Returns     - void
   ======================================================================== */
void SwoopCamera(CAMERA *pCamera)
{
	const LONG tx = (pCamera->LTarget.x << CAMERA_FIXEDPT) - pCamera->x;
	const LONG ty = (pCamera->LTarget.y << CAMERA_FIXEDPT) - pCamera->y;
	const LONG tz = pCamera->LTarget.z - pCamera->z;
		  LONG ta = pCamera->LTarget.a - pCamera->a;
	const LONG tp = pCamera->LTarget.p - pCamera->p;
	FIXED_VECTOR_3D NewPoint;
	const LONG TargetFactor2 = pCamera->TargetFactor - 1;
	PLAYER TempPlayer;
	LONG BumpDistance;
	
	// If the camera is at the target position stop moving.
	if ((ABS(tx)) <= (3 << CAMERA_FIXEDPT) &&
	    (ABS(ty)) <= (3 << CAMERA_FIXEDPT) &&
	    tz == 0 &&
	    ta == 0 &&
	    tp == 0
	   )
    {
    	pCamera->TargetRequested = NO_TARGET_REQUESTED;
#if defined (_WINDOWS)
// [d11-12-96 JPC] Only do autores in Windows; it's no help in DOS and
// causes more bugs.
		if( fAIAutoRes )
		{
			set_hires(0L, 0L);
		}
#endif
    	return;
    }		
	
#if defined (_WINDOWS)
// [d11-12-96 JPC] Only do autores in Windows; it's no help in DOS and
// causes more bugs.
	if( fAIAutoRes )
	{
		set_lowres(0L, 0L);
	}
#endif

  	// Angles are in Byteans.
	// Roll the camera the shortest way around the circle.
	if (ta>128)
		ta=ta-256;
	else
	if (ta<-128)
		ta=ta+256;
		
	NewPoint.dz = ((tz+((tz>0)?TargetFactor2:-TargetFactor2)) / pCamera->TargetFactor);
   	NewPoint.dx = ((tx+((tx>0)?TargetFactor2 << CAMERA_FIXEDPT:-TargetFactor2 << CAMERA_FIXEDPT)) / pCamera->TargetFactor);
   	NewPoint.dy = ((ty+((ty>0)?TargetFactor2 << CAMERA_FIXEDPT:-TargetFactor2 << CAMERA_FIXEDPT)) / pCamera->TargetFactor);
	
	//NewPoint.dx <<= PLAYER_FIXEDPT;
	//NewPoint.dy <<= PLAYER_FIXEDPT;
	
	// If Camera's become fixed point be sure and fix these next two lines.
	//TempPlayer.x = camera.x << PLAYER_FIXEDPT;
	//TempPlayer.y = camera.y << PLAYER_FIXEDPT;
	TempPlayer.x = camera.x;
	TempPlayer.y = camera.y;
	TempPlayer.z = camera.z;
	TempPlayer.a = camera.a;
	TempPlayer.w = CAMERA_WIDTH_WIDE; // GWP HACK numbers for the size of a camera.
	TempPlayer.h = CAMERA_HEIGHT;
	TempPlayer.Flying = TRUE;
	TempPlayer.ThingIndex = fERROR;
	TempPlayer.bump = iNOTHING;
	TempPlayer.BumpIndex = fERROR;
   	
   	CheckMoveSimple(&TempPlayer, &NewPoint, 0, &BumpDistance);
   	//NewPoint.dx >>= PLAYER_FIXEDPT;
   	//NewPoint.dy >>= PLAYER_FIXEDPT;
   	
   	switch(TempPlayer.bump)
   	{
   	case iNOTHING:
   	case iSLIDE_ON_WALL:
#if defined (_STATUS)
			if (!DebugCameraMove (pCamera, NewPoint))
				break;
#endif
	   	pCamera->x += NewPoint.dx;
	   	pCamera->y += NewPoint.dy;
	   	pCamera->z += NewPoint.dz;
   		break;
#if 0
// [d12-09-96 JPC]
   	case iWALL:
	   	pCamera->z += NewPoint.dz;
	   	break;
   	case iCEILING:
	   	pCamera->x += NewPoint.dx;
	   	pCamera->y += NewPoint.dy;
   		if (NewPoint.dz < 0) // going down.
   		{
	   		pCamera->z += NewPoint.dz;
   		}
   		break;
   	case iFLOOR:
	   	pCamera->x += NewPoint.dx;
	   	pCamera->y += NewPoint.dy;
   		if (NewPoint.dz > 0) // going up.
   		{
	   		pCamera->z += NewPoint.dz;
   		}
   		break;
#endif
		// [d12-09-96 JPC] In these cases, do not change x and y; move z to
		// a reasonable height relative to the player.
		case iWALL:
		case iCEILING:
		case iFLOOR:
			pCamera->z = player.z + DEFAULT_CAMERA_Z;
			break;
   	}
   	
   	pCamera->a += ((ta+((ta>0)?TargetFactor2:-TargetFactor2)) / pCamera->TargetFactor);
   	pCamera->p += ((tp+((tp>0)?TargetFactor2:-TargetFactor2)) / pCamera->TargetFactor);
	
	pCamera->TargetFactor = ((pCamera->TargetFactor - 5) / 2) + 5;
	
	// Now Limit the roll angles to the range 0 -> 256
	if (pCamera->a > 256)
		pCamera->a -= 256;
	else
	if (pCamera->a < 0 )
		pCamera->a += 256;

}

/* =======================================================================
   Function    - MoveCameraEvenly
   Description - move the camera to a new point.
   Returns     - void
   ======================================================================== */
void MoveCameraEvenly(CAMERA *pCamera)
{
	const LONG tx = (pCamera->LTarget.x << CAMERA_FIXEDPT) - pCamera->x;
	const LONG ty = (pCamera->LTarget.y << CAMERA_FIXEDPT) - pCamera->y;
	const LONG tz = pCamera->LTarget.z - pCamera->z;
		  LONG ta = pCamera->LTarget.a - pCamera->a;
	const LONG tp = pCamera->LTarget.p - pCamera->p;
	LONG Difference[4];
  		
  		// Angles are in Byteans.
	// Roll the camera the shortest way around the circle.
	if (ta>128)
		ta=ta-256;
	else
	if (ta<-128)
		ta=ta+256;
		
	// MOVE_CAMERA_EVENLY
	Difference[0] = dist(CAMERA_INT_VAL(pCamera->x),
	                     CAMERA_INT_VAL(pCamera->y),
	                     pCamera->LTarget.x,
	                     pCamera->LTarget.y);	// Linear difference between the two point
  	Difference[1] = ABS(tz);	// Height difference
	Difference[2] = ABS(ta);	// Angle difference
	Difference[3] = ABS(tp);	// Pitch difference		

 		 			
	if( Difference[0] <= pCamera->Rate[0] &&
		Difference[1] <= pCamera->Rate[1] &&
		Difference[2] <= pCamera->Rate[2] &&
		Difference[3] <= pCamera->Rate[3] )
	{	
		pCamera->x = pCamera->LTarget.x << CAMERA_FIXEDPT;
		pCamera->y = pCamera->LTarget.y << CAMERA_FIXEDPT;
		pCamera->z = pCamera->LTarget.z;
		pCamera->a = pCamera->LTarget.a;
		pCamera->p = pCamera->LTarget.p;
	}
	else
	{
		LONG MaxIndex=0, i;

		while(Difference[MaxIndex] <= pCamera->Rate[MaxIndex]) MaxIndex++;
		
		for( i=MaxIndex+1; i<4; i++)
		{
			if((Difference[i] > pCamera->Rate[i]) &&
    					 ((Difference[MaxIndex]/pCamera->Rate[MaxIndex])<
			 	  (Difference[i]/pCamera->Rate[i])))
	 	 	{
		 	 	MaxIndex = i;
	 	 	}
		}

		pCamera->x += (tx * pCamera->Rate[MaxIndex] ) / Difference[MaxIndex];	
		pCamera->y += (ty * pCamera->Rate[MaxIndex] ) / Difference[MaxIndex];		 	
		pCamera->z += (tz * pCamera->Rate[MaxIndex] ) / Difference[MaxIndex];		    		
		pCamera->a += (ta * pCamera->Rate[MaxIndex] ) / Difference[MaxIndex];	
		pCamera->p += (tp * pCamera->Rate[MaxIndex] ) / Difference[MaxIndex];	
	}
	
	// Now Limit the roll angles to the range 0 -> 256
	if (pCamera->a > 256)
		pCamera->a -= 256;
	else
	if (pCamera->a < 0 )
		pCamera->a += 256;
		
	
	// If the camera is at the target position stop moving.
	if (CAMERA_INT_VAL(pCamera->x) == pCamera->LTarget.x &&
	    CAMERA_INT_VAL(pCamera->y) == pCamera->LTarget.y &&
	    pCamera->z == pCamera->LTarget.z &&
	    pCamera->a == pCamera->LTarget.a &&
	    pCamera->p == pCamera->LTarget.p
	    )
    {
    	pCamera->TargetRequested = NO_TARGET_REQUESTED;
#if defined (_WINDOWS)
// [d11-12-96 JPC] Only do autores in Windows; it's no help in DOS and
// causes more bugs.
		if( fAIAutoRes )
		{
			set_hires(0L, 0L);
		}
#endif
    }		
    else
    {
#if defined (_WINDOWS)
// [d11-12-96 JPC] Only do autores in Windows; it's no help in DOS and
// causes more bugs.
		if( fAIAutoRes )
		{
			set_lowres(0L, 0L);
		}
#endif
	}
}
/* =======================================================================
   Function    - RotateCameraEvenly
   Description - Rotate the camera Evenly about a point.
   				 
   				 GWP - This doesn't work well if at all.
   				 
   Returns     - void
   ======================================================================== */
void RotateCameraEvenly(CAMERA *pCamera)
{
	const LONG tz = pCamera->RTarget.z - pCamera->z;
	const LONG tp = pCamera->RTarget.p - pCamera->p;
	LONG MoveTota;
	FIXED_VECTOR_3D NewPoint;
	POINT MoveToPoint;
	PLAYER TempPlayer;
	LONG distanceToTarget;
	LONG MoveToPointAngle;
	LONG AngleToMovePoint;
	LONG distanceToMoveToPt;
	LONG BumpDistance;
	LONG MaxIndex=0;
	LONG i;
	LONG Difference[4];
	const LONG CurrentAngleToTarget = AngleFromPoint2(
													 pCamera->x,
													 pCamera->y,
													 pCamera->RTarget.x << CAMERA_FIXEDPT,
													 pCamera->RTarget.y << CAMERA_FIXEDPT,
													 RESOLUTION_1);
	
	// GWP Later store the MoveToPoint & Angle in the camera RTarget struct.
	// GWP when it is first specified.
	
	// Calculate the destination point.
	MoveToPoint.x = 0;
	MoveToPoint.y = pCamera->RTarget.distance << CAMERA_FIXEDPT;
	Rotate(&MoveToPoint, pCamera->RTarget.a);
	
	// Translate to target coordinates.
	MoveToPoint.x += pCamera->RTarget.x << CAMERA_FIXEDPT;
	MoveToPoint.y += pCamera->RTarget.y << CAMERA_FIXEDPT;
	
	distanceToMoveToPt = dist ( CAMERA_INT_VAL(pCamera->x),
								CAMERA_INT_VAL(pCamera->y),
					            CAMERA_INT_VAL(MoveToPoint.x),
					            CAMERA_INT_VAL(MoveToPoint.y));
	
	// Compute the final looking back angle.
	MoveToPointAngle = AngleFromPoint2(
									  MoveToPoint.x,
									  MoveToPoint.y,
									  pCamera->RTarget.x << CAMERA_FIXEDPT,
									  pCamera->RTarget.y << CAMERA_FIXEDPT,
									  RESOLUTION_1);
	
	MoveTota = MoveToPointAngle - CurrentAngleToTarget;
	// Roll the camera the shortest way around the circle.
	if (MoveTota>128)
		MoveTota=MoveTota-256;
	else
	if (MoveTota<-128)
		MoveTota=MoveTota+256;
	
	AngleToMovePoint = AngleFromPoint2(pCamera->x,
									  pCamera->y,
									  MoveToPoint.x,
									  MoveToPoint.y,
									  RESOLUTION_1);
	
   	Difference[0] = distanceToMoveToPt;
   	Difference[1] = ABS(tz);
   	Difference[2] = ABS(MoveTota);
   	Difference[3] = ABS(tp);
   	
   	if ( Difference[0] != 0 ||
   	     Difference[1] != 0 ||
   	     Difference[2] != 0 ||
   	     Difference[3] != 0)
   	{
	
		//while(Difference[MaxIndex] <= pCamera->Rate[MaxIndex]) MaxIndex++;
		
		MaxIndex = 0;
		for( i=0; i<4; i++)
		{
			if((Difference[i] > pCamera->Rate[i]) &&
	    	   ((Difference[MaxIndex]/pCamera->Rate[MaxIndex]) < (Difference[i]/pCamera->Rate[i])))
	 	 	{
		 	 	MaxIndex = i;
	 	 	}
		}
	   	
	   	NewPoint.dx = 0;
	   	NewPoint.dy = (distanceToMoveToPt * pCamera->Rate[MaxIndex] ) / Difference[MaxIndex];
	   	NewPoint.dy <<= CAMERA_FIXEDPT;
		AngleToMovePoint -= (MoveTota * pCamera->Rate[MaxIndex] ) / Difference[MaxIndex];
		Rotate((POINT *)&NewPoint, AngleToMovePoint);
		
		distanceToTarget = dist(CAMERA_INT_VAL(pCamera->x + NewPoint.dx),
								CAMERA_INT_VAL(pCamera->y + NewPoint.dy),
					            pCamera->RTarget.x,
					            pCamera->RTarget.y);
		distanceToTarget = ABS(distanceToTarget);
		
		
		if (distanceToTarget < pCamera->RTarget.radius)
		{
			POINT EdgePoint;
			POINT InsidePoint;
			LONG AngleToCamera;
			// If the cross over point is inside the circle, put us on the edge.
			
			InsidePoint.x = pCamera->x + NewPoint.dx;
			InsidePoint.y = pCamera->y + NewPoint.dy;
			AngleToCamera = AngleFromPoint2(
										   pCamera->RTarget.x << CAMERA_FIXEDPT,
										   pCamera->RTarget.y << CAMERA_FIXEDPT,
										   InsidePoint.x,
										   InsidePoint.y,
										   RESOLUTION_1);
			
			// Now compute the point on the edge.
			EdgePoint.x = 0;
			EdgePoint.y = pCamera->RTarget.radius << CAMERA_FIXEDPT;
			Rotate(&EdgePoint, AngleToCamera);
			// Translate to the target coordinates.
			EdgePoint.x += pCamera->RTarget.x << CAMERA_FIXEDPT;
			EdgePoint.y += pCamera->RTarget.y << CAMERA_FIXEDPT;
			
			// for the CheckMoveSimple call compute the change vector.
			NewPoint.dx = EdgePoint.x - pCamera->x;
			NewPoint.dy = EdgePoint.y - pCamera->y;
		}
		
		NewPoint.dz = (tz * pCamera->Rate[MaxIndex] ) / Difference[MaxIndex];
		
		TempPlayer.x = camera.x;
		TempPlayer.y = camera.y;
		TempPlayer.z = camera.z;
		TempPlayer.a = camera.a;
		TempPlayer.w = CAMERA_WIDTH_NARROW; // GWP HACK numbers for the size of a camera.
		TempPlayer.h = CAMERA_HEIGHT;
		TempPlayer.Flying = TRUE;
		TempPlayer.ThingIndex = fERROR;
		TempPlayer.bump = iNOTHING;
		TempPlayer.BumpIndex = fERROR;
	   	
	   	CheckMoveSimple(&TempPlayer, &NewPoint, 0, &BumpDistance);
	   	
	   	switch(TempPlayer.bump)
	   	{
	   	case iNOTHING:
	   	case iSLIDE_ON_WALL:
#if defined (_STATUS)
				if (!DebugCameraMove (pCamera, NewPoint))
					break;
#endif
		   	pCamera->x += NewPoint.dx;
		   	pCamera->y += NewPoint.dy;
		   	pCamera->z += NewPoint.dz;
	   		break;
#if 0
	// [d12-09-96 JPC]
			case iWALL:
		   	pCamera->z += NewPoint.dz;
		   	break;
	   	case iCEILING:
		   	pCamera->x += NewPoint.dx;
		   	pCamera->y += NewPoint.dy;
	   		if (NewPoint.dz < 0) // going down.
	   		{
		   		pCamera->z += NewPoint.dz;
	   		}
	   		break;
	   	case iFLOOR:
		   	if(NewPoint.dz < 10)
			{
				pCamera->x += NewPoint.dx;
		   		pCamera->y += NewPoint.dy;
	   			
	   			if (NewPoint.dz > 0) // going up.
	   			{
		   	  		pCamera->z += NewPoint.dz;
	   			}
			}
	   		break;
#endif
			case iWALL:
			case iCEILING:
			case iFLOOR:
				pCamera->z = player.z + DEFAULT_CAMERA_Z;
				break;
	   	}
		
	   	pCamera->p += (tp * pCamera->Rate[MaxIndex] ) / Difference[MaxIndex];
	    pCamera->a = AngleFromPoint2(
								    pCamera->x,
								    pCamera->y,
								    pCamera->RTarget.x << CAMERA_FIXEDPT,
								    pCamera->RTarget.y << CAMERA_FIXEDPT,
								    RESOLUTION_1);
	
	}
	// If the camera is at the target position stop moving.
    distanceToMoveToPt = ABS(distanceToMoveToPt);
    if (
	    distanceToMoveToPt <= 4 &&
	    pCamera->z == pCamera->RTarget.z &&
	    pCamera->p == pCamera->RTarget.p
       )
    {
    	pCamera->TargetRequested = NO_TARGET_REQUESTED;
#if defined (_WINDOWS)
// [d11-12-96 JPC] Only do autores in Windows; it's no help in DOS and
// causes more bugs.
		if( fAIAutoRes )
		{
			set_hires(0L, 0L);
		}
#endif
    }
#if defined (_WINDOWS)
// [d11-12-96 JPC] Only do autores in Windows; it's no help in DOS and
// causes more bugs.
    else
    {
		if( fAIAutoRes )
		{
			set_lowres(0L, 0L);
		}
	}
#endif
}
/* =======================================================================
   Function    - RotateSwoopCamera
   Description - Swoop the camera
   Returns     - void
   ======================================================================== */
void RotateSwoopCamera(CAMERA *pCamera)
{
	const LONG tz = pCamera->RTarget.z - pCamera->z;
	const LONG tp = pCamera->RTarget.p - pCamera->p;
	LONG MoveTota;
	FIXED_VECTOR_3D NewPoint;
	POINT MoveToPoint;
	PLAYER TempPlayer;
	const LONG TargetFactor2 = pCamera->TargetFactor - 1;
	LONG distanceToTarget;
	LONG MoveToPointAngle;
	LONG AngleToMovePoint;
	LONG distanceToMoveToPt;
	LONG BumpDistance;
   	LONG Angle;
	const LONG CurrentAngleToTarget = AngleFromPoint2(
													 CAMERA_INT_VAL(pCamera->x),
													 CAMERA_INT_VAL(pCamera->y),
													 pCamera->RTarget.x,
													 pCamera->RTarget.y,
													 RESOLUTION_1);
	
	// GWP Later store the MoveToPoint & Angle in the camera RTarget struct.
	// GWP when it is first specified.
	
	// Calculate the destination point.
	MoveToPoint.x = 0;
	MoveToPoint.y = pCamera->RTarget.distance << CAMERA_FIXEDPT;
	Rotate(&MoveToPoint, pCamera->RTarget.a);
	
	// Translate to target coordinates.
	MoveToPoint.x += pCamera->RTarget.x << CAMERA_FIXEDPT;
	MoveToPoint.y += pCamera->RTarget.y << CAMERA_FIXEDPT;
	
	// Compute the final looking back angle.
	MoveToPointAngle = AngleFromPoint2(
									  CAMERA_INT_VAL(MoveToPoint.x),
									  CAMERA_INT_VAL(MoveToPoint.y),
									  pCamera->RTarget.x,
									  pCamera->RTarget.y,
									  RESOLUTION_1);
	
	distanceToMoveToPt = dist ( CAMERA_INT_VAL(pCamera->x),
								CAMERA_INT_VAL(pCamera->y),
					            CAMERA_INT_VAL(MoveToPoint.x),
					            CAMERA_INT_VAL(MoveToPoint.y));
	
	MoveTota = MoveToPointAngle - CurrentAngleToTarget;
	// Roll the camera the shortest way around the circle.
	if (MoveTota>128)
		MoveTota=MoveTota-256;
	else
	if (MoveTota<-128)
		MoveTota=MoveTota+256;
	
	AngleToMovePoint = AngleFromPoint2(pCamera->x,
									  pCamera->y,
									  MoveToPoint.x,
									  MoveToPoint.y,
									  RESOLUTION_1);
	
   	NewPoint.dx = 0;
   	NewPoint.dy = ((distanceToMoveToPt+((distanceToMoveToPt>0)?TargetFactor2:-TargetFactor2)) / pCamera->TargetFactor);
   	NewPoint.dy <<= CAMERA_FIXEDPT;
	AngleToMovePoint -= ((MoveTota+((MoveTota>0)?TargetFactor2:-TargetFactor2)) / pCamera->TargetFactor);
	Rotate((POINT *)&NewPoint, AngleToMovePoint);
	
	distanceToTarget = dist(CAMERA_INT_VAL(pCamera->x + NewPoint.dx),
							CAMERA_INT_VAL(pCamera->y + NewPoint.dy),
				            pCamera->RTarget.x,
				            pCamera->RTarget.y);
	distanceToTarget = ABS(distanceToTarget);
	
	
	if (distanceToTarget < pCamera->RTarget.radius)
	{
		POINT EdgePoint;
		POINT InsidePoint;
		LONG AngleToCamera;
		// If the cross over point is inside the circle, put us on the edge.
		
		InsidePoint.x = pCamera->x + NewPoint.dx;
		InsidePoint.y = pCamera->y + NewPoint.dy;
		AngleToCamera = AngleFromPoint2(
									   pCamera->RTarget.x << CAMERA_FIXEDPT,
									   pCamera->RTarget.y << CAMERA_FIXEDPT,
									   InsidePoint.x,
									   InsidePoint.y,
									   RESOLUTION_1);
		
		// Now compute the point on the edge.
		EdgePoint.x = 0;
		EdgePoint.y = pCamera->RTarget.radius << CAMERA_FIXEDPT;
		Rotate(&EdgePoint, AngleToCamera);
		// Translate to the target coordinates.
		EdgePoint.x += pCamera->RTarget.x << CAMERA_FIXEDPT;
		EdgePoint.y += pCamera->RTarget.y << CAMERA_FIXEDPT;
		
		// for the CheckMoveSimple call compute the change vector.
		NewPoint.dx = EdgePoint.x - pCamera->x;
		NewPoint.dy = EdgePoint.y - pCamera->y;
	}
	
	NewPoint.dz = ((tz+((tz>0)?TargetFactor2:-TargetFactor2)) / pCamera->TargetFactor);
	
	TempPlayer.x = camera.x;
	TempPlayer.y = camera.y;
	TempPlayer.z = camera.z;
	TempPlayer.a = camera.a;
	TempPlayer.w = CAMERA_WIDTH_NARROW; // GWP HACK numbers for the size of a camera.
	TempPlayer.h = CAMERA_HEIGHT;
	TempPlayer.Flying = TRUE;
	TempPlayer.Crouching = FALSE;
	TempPlayer.ThingIndex = fERROR;
	TempPlayer.ceiling = 0xDCDCDCDC;
	TempPlayer.floor = 0xDCDCDCDC;
	TempPlayer.WalkThruWall = FALSE;
	TempPlayer.bump = iNOTHING;
	TempPlayer.BumpIndex = fERROR;
	TempPlayer.currentSector = 0;
	TempPlayer.fallHeight = 0;
   	
   	//CheckMoveSimple(&TempPlayer, &NewPoint, 0, &BumpDistance);
   	CheckMove(&TempPlayer, &NewPoint, 0, &Angle, &BumpDistance);
   	
   	switch(TempPlayer.bump)
   	{
   	case iNOTHING:
	case iACID:
	case iSHALLOW_WATER:
	case iDEEP_WATER:
	case iLAVA:
	case iHOLE:
   	case iSLIDE_ON_WALL:
#if defined (_STATUS)
			if (!DebugCameraMove (pCamera, NewPoint))
				break;
#endif
	   	pCamera->x += NewPoint.dx;
	   	pCamera->y += NewPoint.dy;
	   	pCamera->z += NewPoint.dz;
   		break;
	case iWALL:
	   	pCamera->z += NewPoint.dz;
	   	break;
   	case iCEILING:
	   	//pCamera->x += NewPoint.dx;
	   	//pCamera->y += NewPoint.dy;
   		if (NewPoint.dz < 0) // going down.
   		{
	   		pCamera->z += NewPoint.dz;
   		}
   		break;
   	case iFLOOR:
		//pCamera->x += NewPoint.dx;
	   	//pCamera->y += NewPoint.dy;
   		
   		if (NewPoint.dz > 0) // going up.
   		{
	   		pCamera->z += NewPoint.dz;
   		}
   		break;
#if 0
		// GWP 3/24/97
		case iWALL:
		case iCEILING:
		case iFLOOR:
			// GWP This is bad because it pays no attention to what the
			//     actual floor height is under the camera vs the player.
			pCamera->z = player.z + DEFAULT_CAMERA_Z;
			break;
#endif
   	}
	
   	pCamera->p += ((tp+((tp>0)?TargetFactor2:-TargetFactor2)) / pCamera->TargetFactor);
    pCamera->a = AngleFromPoint2(
							    pCamera->x >> CAMERA_FIXEDPT,
							    pCamera->y >> CAMERA_FIXEDPT,
							    pCamera->RTarget.x,
							    pCamera->RTarget.y,
							    RESOLUTION_1);

	pCamera->TargetFactor = ((pCamera->TargetFactor - 5) / 2) + 3;
	
	// If the camera is at the target position stop moving.
    distanceToMoveToPt = ABS(distanceToMoveToPt);
    if (
	    distanceToMoveToPt <= 4 &&
	    pCamera->z == pCamera->RTarget.z &&
	    pCamera->p == pCamera->RTarget.p
       )
    {
    	pCamera->TargetRequested = NO_TARGET_REQUESTED;
#if defined (_WINDOWS)
// [d11-12-96 JPC] Only do autores in Windows; it's no help in DOS and
// causes more bugs.
		if( fAIAutoRes )
		{
			set_hires(0L, 0L);
		}
#endif
    }
#if defined (_WINDOWS)
// [d11-12-96 JPC] Only do autores in Windows; it's no help in DOS and
// causes more bugs.
    else
    {
		if( fAIAutoRes )
		{
			set_lowres(0L, 0L);
		}
	}
#endif
}

/* =======================================================================
   Function    - RotateCameraAroundPoint
   Description - rotate the camera around a point,
					  have to call SetCameraCurrentPosition first to set current angle and radius
   Returns     - void
   ======================================================================== */
void RotateCameraAroundPoint(CAMERA * pCamera, LONG KeyState )
{
	LONG	Temp;
	BOOL	fPosition = FALSE;
	POINT	TempPoint;
	FIXED_VECTOR_3D NewPoint;
	PLAYER TempPlayer;
	LONG BumpDistance;
	BOOL 	CameraMoved = FALSE;
	LONG	Angle;
	
	static LONG PitchDelta = 0;
	
	/* -----------------------------------------------------------------
	   First decode key movement
	   ----------------------------------------------------------------- */
	// NOTE:
	// Rate[0]: distance rate    Rate[1]: height rate
	// Rate[2]: angle rate       Rate[3]: pitch rate
				
	/* forward/backward */
	if (KeyState & KEY_FORWARD_BIT )
	{
		pCamera->RTarget.radius -= pCamera->Rate[0];
		if (pCamera->RTarget.radius < pCamera->RTarget.distance)
			pCamera->RTarget.radius = pCamera->RTarget.distance;
		
		CameraMoved = TRUE;
	}
	if (KeyState & KEY_BACKWARD_BIT)
	{
		pCamera->RTarget.radius += pCamera->Rate[0];
		if (pCamera->RTarget.radius > pCamera->RTarget.max_distance)
			pCamera->RTarget.radius = pCamera->RTarget.max_distance;
		
		CameraMoved = TRUE;
	}

	/* ---------------------------------------------------------------- */
	/* rotate left/right */
	if (KeyState & KEY_LEFT_BIT)
	{
		pCamera->RTarget.a += pCamera->Rate[2];
		CameraMoved = TRUE;
	}
	if (KeyState & KEY_RIGHT_BIT)
	{
		pCamera->RTarget.a -= pCamera->Rate[2];
		CameraMoved = TRUE;
	}
	
	/* -----------------------------------------------------------------
	   Second, figure the movement of the camera
	   ----------------------------------------------------------------- */

	if( pCamera->RTarget.a != pCamera->Current.a )
	{
		Temp = pCamera->RTarget.a - pCamera->Current.a;
		// Roll the camera the shortest way around the circle.
		if (Temp>128)
			Temp=Temp-256;
		else
		if (Temp<-128)
			Temp=Temp+256;
			
		// halt the wobble
		if (abs(Temp) <= pCamera->Rate[2]*2)
			pCamera->Current.a = pCamera->RTarget.a;
   		else
	   		pCamera->Current.a += ((Temp>0) ? pCamera->Rate[2] : -pCamera->Rate[2] );
			
		fPosition = TRUE;
	}
	
	if( pCamera->RTarget.radius != pCamera->Current.radius )
	{
		Temp = pCamera->RTarget.radius - pCamera->Current.radius;
   		pCamera->Current.radius += ((Temp>0) ? pCamera->Rate[0] : -pCamera->Rate[0] );
		// halt the wobble
		if (abs(Temp) <= pCamera->Rate[0]*2)
			pCamera->Current.radius = pCamera->RTarget.radius;
		
		fPosition = TRUE;
	}
	
//	if( pCamera->RTarget.z != pCamera->Current.z )
//	{
//		Temp = pCamera->RTarget.z - pCamera->Current.z;
//   		pCamera->Current.z += ((Temp>0) ? pCamera->Rate[1] : -pCamera->Rate[1] );
		// halt the wobble
//		if (abs(Temp) <= pCamera->Rate[1]*2)
//			pCamera->Current.z = pCamera->RTarget.z;
//		fPosition = TRUE;
//	}
//	
//	if( pCamera->RTarget.p != pCamera->Current.p )
//	{
//		Temp = pCamera->RTarget.p - pCamera->Current.p;
//   		pCamera->Current.p += ((Temp>0) ? pCamera->Rate[3] : -pCamera->Rate[3] );
//		// halt the wobble
//		if (abs(Temp) <= pCamera->Rate[3]*2)
//			pCamera->Current.p = pCamera->RTarget.p;
//		fPosition = TRUE;
//	}
	
	// get the point relative to the target that
	// we need to move and rotate to
	TempPoint.x = 0;
	TempPoint.y = pCamera->Current.radius<<CAMERA_FIXEDPT;
	Rotate(&TempPoint, pCamera->Current.a);
		
	// move the camera
	pCamera->Current.x = pCamera->RTarget.x + TempPoint.x;
	pCamera->Current.y = pCamera->RTarget.y + TempPoint.y;
	
	/* Look up and down */
	if (KeyState & KEY_LOOKUP_BIT)
	{
		if (PitchDelta < MAX_PITCH)
			PitchDelta += MAX_P_SPEED;
		CameraMoved = TRUE;
	}
	else if (KeyState & KEY_LOOKDOWN_BIT)
	{
		if (PitchDelta > -MAX_PITCH)
			PitchDelta -= MAX_P_SPEED;
		CameraMoved = TRUE;
	}
	else
	{
		PitchDelta = (PitchDelta < -(MAX_P_SPEED+1))
							? PitchDelta + MAX_P_SPEED
							: (PitchDelta > (MAX_P_SPEED))
								? PitchDelta - MAX_P_SPEED
								: 0;
		if (PitchDelta)
			CameraMoved = TRUE;
	}
	
	// calculate the new height and pitch
	pCamera->Current.z = pCamera->RTarget.z + (pCamera->Current.radius*pCamera->Rate[1])/1000;
	pCamera->Current.p =
			(pCamera->Current.radius*pCamera->Rate[3])/1000
			+ PitchDelta;

	
	/* -----------------------------------------------------------------
	   Finally, calculate where to look and move there
	   ----------------------------------------------------------------- */

	
	TempPlayer.x = pCamera->x;
	TempPlayer.y = pCamera->y;
	TempPlayer.z = pCamera->z;
	TempPlayer.a = pCamera->a;
	// If human is driving the camera, use narrow width for camera,
	// else use wide width.
	if (KeyState != 0)
		TempPlayer.w = CAMERA_WIDTH_NARROW;
	else
		TempPlayer.w = CAMERA_WIDTH_WIDE;
	TempPlayer.h = CAMERA_HEIGHT;
	TempPlayer.Flying = TRUE;
	TempPlayer.ThingIndex = fERROR;
	TempPlayer.bump = iNOTHING;
	TempPlayer.BumpIndex = fERROR;

	NewPoint.dx = pCamera->Current.x - pCamera->x;
	NewPoint.dy = pCamera->Current.y - pCamera->y;
	NewPoint.dz = pCamera->Current.z - pCamera->z;	
	CheckMove(&TempPlayer, &NewPoint, 0, &Angle, &BumpDistance);

#ifdef _STATUS
	WriteDebug ("Camera bump in RotateCameraAroundPoint = %d", TempPlayer.bump);
	if (async_key_status (KEY_CONTROL))
		TempPlayer.bump = iNOTHING;
#endif

	switch(TempPlayer.bump)
	{
	case iNOTHING:
	case iACID:
	case iSHALLOW_WATER:
	case iDEEP_WATER:
	case iLAVA:
	case iHOLE:
	case iSLIDE_ON_WALL:
#if defined (_STATUS)
			if (!DebugCameraMove (pCamera, NewPoint))
				break;
#endif
	 	pCamera->x += NewPoint.dx;
	 	pCamera->y += NewPoint.dy;
	 	pCamera->z += NewPoint.dz;
		break;
#if 0
// [d12-09-96 JPC]
	case iWALL:
	 	pCamera->z += NewPoint.dz;
		break;
	case iCEILING:
	 	pCamera->x += NewPoint.dx;
	 	pCamera->y += NewPoint.dy;
		if(NewPoint.dz < 0) //going down
		{
			pCamera->z += NewPoint.dz;
		}
		break;
	case iFLOOR:
		if(NewPoint.dz < 10)
		{
			pCamera->x += NewPoint.dx;
	 		pCamera->y += NewPoint.dy;
			if (NewPoint.dz > 0) // going up.
			{
	 			pCamera->z += NewPoint.dz;
			}
		}	
		break;
#endif
		case iWALL:
		case iCEILING:
		case iFLOOR:
			pCamera->z = player.z + DEFAULT_CAMERA_Z;
			break;
	}
		
	Temp = AngleFromPoint2(
		pCamera->x,
		pCamera->y,
		pCamera->RTarget.x,
		pCamera->RTarget.y,
		RESOLUTION_1);
   	

	//pCamera->x = pCamera->Current.x;
	//pCamera->y = pCamera->Current.y;
	//pCamera->z = pCamera->Current.z;
	pCamera->z = pCamera->Current.z;
	pCamera->a = Temp;
	pCamera->p = pCamera->Current.p;
	
#if defined (_WINDOWS)
// [d11-12-96 JPC] Only do autores in Windows; it's no help in DOS and
// causes more bugs.
	if (fAIAutoRes)
	{
		if (CameraMoved == TRUE)
		{
			set_lowres(0,0);
		}
		else
		{
			set_hires(0,0);
		}
	}
#endif
}

#if 0	// UNUSED
/* =======================================================================
   Function    - push_camera
   Description - pushes the active camera onto the camera stack
   Returns     - void
   ======================================================================== */
void push_camera()
{
	camera_stack[camera_stack_ptr++]=camera;
	if(camera_stack_ptr>CAMERA_STACK_SIZE)
		fatal_error("Overflowed camera stack");
}

/* =======================================================================
   Function    - pull_camera
   Description - pulls a camera off the stack
   Returns     - void
   ======================================================================== */
void pull_camera()
{
	camera=camera_stack[--camera_stack_ptr];
	if(camera_stack_ptr<0)
		fatal_error("Underflowed camera stack");
}

#endif // UNUSED
