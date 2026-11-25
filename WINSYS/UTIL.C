/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: UTIL.C      -Low Level Utitilies
   Author: Chris Phillips
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:
   get_free_mem          -gets the largest free block of memory
   safe_malloc           -a "safer" malloc
   fatal_error           -aborts the game, writes and displays error message
   get_time              -returns the time
   pause                 -pauses for a specified length of time

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <dos.h>
#include <conio.h>
// JPC: VC4 does not have i86.h--do not use. #include <i86.h>
#include <Windows.h>
#include "../SYSTEM.H"
#include "../MACHINE.H"
#include "../MACHINT.H"
#include "../ENGINE.H"
#include "DDRAWPRO.H"

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
   Function    - get_free_mem
   Description - gets the largest block of free memory
   Returns     - the size of the block
   ======================================================================== */
ULONG get_free_mem()
{
	return((ULONG)(8 * 1024 * 1024));
}

/* ========================================================================
   Function    - safe_malloc
   Description - mallocs and aborts if malloc returns NULL
   Returns     - the pointer to the new block of memory
   ======================================================================== */
char *safe_malloc(LONG size)
{
char * x;
	x= (char *) malloc(size);

	if(x==NULL)
		fatal_error("Cant Malloc memory in safe_malloc");
	return(x);
}

/* ========================================================================
   Function    - fatal_error
   Description - aborts the game, displaying and writing to disk an error msg
   Returns     - void
   ======================================================================== */
void fatal_error(const char *format, ...)
{
	char texbuffer[200];
	FILE *f;
	va_list argp;
	
	va_start(argp, format);
	vsnprintf(texbuffer, sizeof(texbuffer), format, argp);
	va_end(argp);
	close_graph();
	remove_keyint();
	printf("Fatal Error:%s\n",texbuffer);

	// The game folder can be read-only (e.g. installed under Program
	// Files). fprintf on the failed fopen used to abort the process in the
	// C runtime, so the error was never shown.
	f=fopen("Fatal.err","wt");
	if (f != NULL)
	{
		fprintf(f,"Fatal Error:%s\n",texbuffer);
		fclose(f);
	}

	// Full-screen DirectDraw would hide the box, so drop to normal mode
	// first, as the help menu does before WinHelp.
	if ( sDrawMode == iDDRAW )
		DDSetCooperative ( FALSE );
	MessageBox (NULL, texbuffer, "FATAL ERROR", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
	
#if defined(_DEBUG)
	// if debuggable, crash into the debugger
	{
		volatile SHORT	i = 0;
		i /= 0;
	}
#endif

	// this exit is a catch all
	exit(0);
}

/* ========================================================================
   Function    - get_time
   Description - gets the time
   Returns     - the time
   ======================================================================== */
LONG get_time()
{
	// 64-bit: GetTickCount wraps to 0 after 49.7 days of uptime, and every
	// "wait until tick N" in the game would stall across that jump.
	return (LONG)(GetTickCount64()/55);
}

/* ========================================================================
   Function    - pause
   Description - pauses the game
   Returns     - void
   ======================================================================== */
void pause(LONG t)
{
LONG end;

	//I belive a "tick" is 1/18 of a second

	end=get_time()+t;
	while(get_time()<end);

}
