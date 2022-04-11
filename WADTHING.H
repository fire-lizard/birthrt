/* ========================================================================
   Copyright (c) 1990,1997	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _WADTHING_H
#define _WADTHING_H

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* the types of things we can click on */
// Also these things can be "bumped"
typedef enum
{
	iNOTHING = 0,
	iOBJECT,	// Click on
	iFLOOR,		// Click on
	iWALL,		// Click on (Hit face on)
#ifdef INET
	iMap,	   	// Click on
#endif
	iSLIDE_ON_WALL,	// Hit on side
	iCEILING,	// Hit your head
	iHOLE,		// Gaping hole in front of you.
	iSHALLOW_WATER,
	iDEEP_WATER,
	iLAVA,
	iACID,
	iEDGE_OF_WORLD,
	iMONSTER_BOX,
	iEXITLEVEL	// Wad-to-wad teleport
} WadThingType;
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


#if defined (__cplusplus)
}
#endif

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif


#if defined (__cplusplus)
}
#endif
#endif // _WADTHING_H
