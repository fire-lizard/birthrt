/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _PLAYER_HXX
#define _PLAYER_HXX
/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_WADTHING_H)
#include "wadthing.h"
#endif


/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* 24.8 fixed point adjust value for the PLAYER structure and it's routines */
#define PLAYER_FIXEDPT	8
#define PLAYER_INT_VAL(x)	((x) >> PLAYER_FIXEDPT)
#define PLAYER_FLOAT_VAL(x)	(x)

/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */

typedef struct{
	LONG			x,y;		// using 24.8 fixed point for xy
	LONG			z;			// height above floor
	LONG			a,p;		// angle, pitch (NO ROLL)
	LONG			w,h;		// width, height
	BOOL			Flying;		// Whether you are flying or not.
	BOOL			Crouching;	// Whether you are crouching or not.
	LONG			ThingIndex;	// index in the mythings array.
	LONG        	ceiling;
	LONG			floor;
	UBYTE			WalkThruWall;	 //Whether control key is held down.
	WadThingType	bump;		// Last Wad thing bumped. (reset every render.)
	LONG			BumpIndex;	// if an object bumped, its thing index is here.
	LONG			currentSector;				// [d11-13-96 JPC] for sound effects
	LONG			fallHeight;					// [d11-22-96 JPC]
	LONG			startFallZ;					// [d4-25-97 JPC] for correct caluculation
													// of fall height (jumping over holes bug)
} PLAYER;

// A CAMERA or a PLAYER should have the same first 6 values.
// and therefore be able to be cast to a POSITION.
typedef struct{
	LONG		x,y;		// using 24.8 fixed point for xy
	LONG		z;			// height above floor
	LONG		a,p;		// angle, pitch
} POSITION;

typedef struct{
	LONG	dx, dy;
	LONG	a;
	LONG	vx, vy;
} TELEPORT_DATA;
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

#if defined (__cplusplus)
extern "C" {
#endif

// auto low res for fast movement
extern BOOL	fAIAutoRes;
#if !defined(_WINDOWS)
extern BOOL	fAIMoving;
#endif

extern LONG	PlayerSpeed;
extern LONG	PlayerJumpVelocity;
extern LONG	PlayerFallRate;
extern LONG	PlayerTermVelocity;

extern PLAYER	player;
extern BOOL	fFreeze; 					// whether to allow player movement
extern BOOL fClimb;

extern LONG	fExitLevel;					// an exit tag line has been crossed

extern LONG	PlayerLoresSpeed;
extern LONG	PlayerHiresSpeed ;

extern TELEPORT_DATA teleport_data;
	
	/* For use by C routines. */
void SetPlayer (LONG x, LONG y, LONG z, LONG a, LONG p);
#define SetPlayerXYA(x,y,a) SetPlayer((x),(y),NO_CHANGE,(a),NO_CHANGE)

void TeleportPlayer (ULONG );
void PlayerArrival (LONG * );
void InitPlayer (LONG , LONG , LONG );
#define init_player(x,y,a) InitPlayer((x),(y),(a))

#if defined (__cplusplus)
}
#endif



#if defined (__cplusplus)

// C++ linkage only fns.

void StopPlayer();
LONG CheckMoveKeys(void);
BOOL SwoopToTarget ( POSITION *pStart, POSITION *pTarget);
LONG MovePlayer( LONG KeyState );
LONG MoveToward(LONG X,LONG Y,LONG TargetX,LONG TargetY,LONG Rate,LONG *NewX,LONG *NewY);
void SetPlayerTarget( LONG	X, LONG	Y, LONG	H, LONG	A, LONG	P );
LONG SetPlayerSpeed(LONG Speed);
LONG SetPlayerSize(LONG Width, LONG Height);
LONG SetPlayerJump(LONG RiseVelocity, LONG FallRate, LONG TerminalVelocity);
void PlayerFreeze(LONG NewFlag);
void PlayerFlying(LONG, LONG NewFlag);
void PlayerClimbing(LONG, LONG NewFlag);
void ConvertScreenToFloor(LONG cursor_x, LONG cursor_y, LONG *sx, LONG *sy);

#endif

/* ------------------------------------------------------------------------
   inline functions
   ------------------------------------------------------------------------ */

#endif
