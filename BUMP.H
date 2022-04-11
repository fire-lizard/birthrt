/* ========================================================================
   Copyright (c) 1990,1997	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _BUMP_H
#define _BUMP_H

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_WADTHING_H)
#include "wadthing.h"
#endif

#if !defined(_PLAYER_HXX)
#include "player.hxx"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* check line special types */
// JPC Changed the following from 0 and 1 to 1 and 2 because we AND the
// values with the sSpecial parameter.  sSpecial & CHECKLINE_PLAYER
// came out 0, even when sSpecial was CHECKLINE_PLAYER.
// Note that if we add new values, they need to be a power of 2.
#define CHECKLINE_PLAYER	0x1		// was 0
#define CHECKLINE_MONSTER	0x2		// was 1
#define CHECKLINE_CAMERA	0x4
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
   Global Variables
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

extern LONG gDialogFlag;

#if defined (__cplusplus)
}
#endif

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

// Note the POINT is in 28.4 FixedPoint values.
LONG CheckBump (PLAYER *Player, FIXED_VECTOR_3D *pPoint, LONG /* Rate */);
// Note the POINT is in 28.4 FixedPoint values.
WadThingType CheckMove (PLAYER *pPlayer, FIXED_VECTOR_3D *pPoint, SHORT sSpecial, LONG *BumpAngle, LONG *BumpDistance);
WadThingType CheckLongMove (PLAYER *pPlayer, LONG a, LONG distance, LONG sSpecial, LONG z, LONG *BumpDistance);
WadThingType CheckMoveSimple (
	PLAYER *pPlayer,
	FIXED_VECTOR_3D *pPoint, 	// This is the delta of the motion, not the absolute location.
	SHORT sSpecial,
	LONG *BumpDistance);	// distance to line squared;


// [d8-13-96 JPC] LinePointDistance is now also called from SECTORS: activate_seg.
LONG LinePointDistance (LONG x, LONG y,
						LONG ax, LONG ay,
						LONG bx, LONG by);

#if defined (__cplusplus)
}
#endif
#endif // _BUMP_H
