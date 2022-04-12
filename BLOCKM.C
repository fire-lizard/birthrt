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
   show_block_lines -draws the block-grid (?)
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
SHORT* get_blockm (long px1,long py1)
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

/* =======================================================================
   Function    - show_block_lines
   Description - draws the grid lines on the map?
   Returns     - void
   ======================================================================== */
void show_block_lines ()
{
}

/* ========================================================================
   Function    - GetBlockFromXY
   Description - returns the starting Point of a block from world coords
   Returns     - a corner of the block
   ======================================================================== */
POINT GetBlockCornerFromXY(LONG x,LONG y)
{
	POINT ReturnMe;

	ReturnMe.x=(x-blockm_header.xo)/128;
	ReturnMe.y=(y-blockm_header.yo)/128;

	ReturnMe.x*=128;
	ReturnMe.y*=128;

	ReturnMe.x+=blockm_header.xo;
	ReturnMe.y+=blockm_header.yo;


	return ReturnMe;

}

	