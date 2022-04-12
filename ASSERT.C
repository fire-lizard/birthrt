/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: ASSERT.C
   Author: 	 Greg Hightower
   ========================================================================
   Contains the following general functions:
   
   assert()
   
   Contains the following internal functions:
   
   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdarg.h>
#include "SYSTEM.H"
#include "MACHINE.H"
#include "MACHINT.H"

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
	Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
void fatal_exit(const char *format, ...);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

#if !defined(_RELEASE)

/* ========================================================================
   Function    - _assert
   Description - do bool expression check, fatal_err on FALSE value
   					SEE THE MACRO assert in SYSTEM.H
   Returns     - void
   ======================================================================== */
void _assert(BOOL fTest, char *FileName, SHORT LineNum)
{
	if(!fTest)
	{
		fatal_exit("ASSERT FAILED! In %s on line %d\n",FileName,LineNum);
	}
}

/* ========================================================================
   Function    - fatal_error
   Description - aborts the game, displaying and writing to disk an error msg
   Returns     - void
   ======================================================================== */
void fatal_exit(const char *format, ...)
{
	char texbuffer[200];
	FILE *f;
	va_list argp;
	
	f=fopen("Fatal.err","wt");
	va_start(argp, format);
	vsprintf(texbuffer,format,argp);
	close_graph();
	remove_keyint();
	printf("Fatal Error:%s\n",texbuffer);
	fprintf(f,"Fatal Error:%s\n",texbuffer);
	fclose(f);

	printf("Exiting...\n");
	
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

#endif // !defined(_RELEASE)

