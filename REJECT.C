/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: REJECT.C   -Handles Reject Tables in Nova
   Author: Greg Hightower
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#if !defined(_RELEASE)
#include <stdio.h>
#endif

#include "system.h"
#include "engine.h"
#include "engint.h"

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

/* ========================================================================
   Function    - reject
   Description - get the reject value between two sectors
   Returns     - TRUE if you can see me
   ======================================================================== */
BOOL reject(LONG MySector, LONG HisSector)
{
	LONG	Index;
	//LONG	i;
	ULONG	ubValue, ubCount, ubMask;
	
	// the reject table is an bit-stream array of tot_sector X tot_sector 
	// bits.  To index into it, do the array math and then shift dow
	// by UBYTE (8) bits.
	// Now, use the lower 3 bits to mask off the bit you want
	
	Index = (MySector * tot_sectors) + HisSector;
	
	ubValue = rejects[(Index>>3)];
	
	ubCount = Index & 0x00000007;
	//ubMask = 0x01;
	//ubMask <<= ubCount;
	ubMask = 0x01 << ubCount;

	// in the reject table, a 1 means can't see and a 0 means can
	return( !(ubValue & ubMask) );
}

