/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: BLOCKM.C   -handles blockmap related stuff
   Author: Chris Phillips
   ========================================================================
   Contains the following internal functions: 

   Contains the following general functions:
   get_blockm       -finds a block according to coordinates passed
   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include "SYSTEM.H"
#include "ENGINE.H"
#include "ENGINT.H"

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

/* =======================================================================
   Function    - get_blockm
   Description - determines the block specified coordinates are in
   Returns     - returns the block the coordinates are in
   ======================================================================== */   
SHORT* get_blockm (LONG px1,LONG py1)
{
	long bx,by,bo;
	SHORT *bm;

	bx=(px1-blockm_header.xo)/(128*EXPAND_FACTOR);
	by=(py1-blockm_header.yo)/(128*EXPAND_FACTOR);
	if(by<0 || by>=blockm_header.rows)
		return(FALSE);
	if(bx<0 || bx>=blockm_header.cols)
		return(FALSE);
	bo=(blockm_header.cols*by)+bx;
	bm=&blockm[blockm_offs[bo]];
	return(bm);
}

