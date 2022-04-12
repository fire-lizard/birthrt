/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: STATISTI.C  -Handles runtime stats
   Author: Chris Phillips
   ========================================================================
   Contains the following internal functions: 

   Contains the following general functions:
   init_statistics       -initializes the statistical variables
   close_statistics      -prints statistics
   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <conio.h>
#include "../SYSTEM.H"
#include "../MACHINE.H"
#include "../MACHINT.H"
#include "../ENGINE.H"

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
LONG before_memory;
LONG before_time;

/* =======================================================================
   Function    - init_statistics
   Description - initializes variables to keep track of stats
   Returns     - void
   ======================================================================== */
void init_statistics()
{
	before_time=get_time();
	before_memory=get_free_mem();
}

/* =======================================================================
   Function    - close_statistics
   Description - reads before stats, generates after stats & prints them all
   Returns     - void
   ======================================================================== */
void close_statistics()
{
extern LONG frames;
LONG tot_time,tt;
ULONG after_memory;
	tt=(LONG)get_time();
	tot_time=(LONG)((LONG)tt-(LONG)before_time);
	after_memory=(ULONG)get_free_mem();

#if 0
	printf("STATISTICS------------------------------------------------------\n");
	printf("TIMER:%ld  FRAMES:%ld   TICK PER:%ld   FPS:%f\n",tot_time,frames,(LONG)((LONG)tot_time)/((LONG)frames),(float)frames/((float)tot_time/18));
	printf("CAMERA: X:%d Y:%d Z:%d PH:%d A:%d\n",camera.x,camera.y,camera.z,player.hei,camera.a);
	printf("MEMORY(Includes ZONE): Before:%u After:%u Used:%u\n",before_memory,after_memory,before_memory-after_memory);

printf("----------------------------------------------------------------\n");
#endif
}
