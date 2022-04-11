/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: FATAL.C
   Author: 	 Greg Hightower
   ========================================================================
   Contains the following general functions:
   
   fatal_test()
   
   Contains the following internal functions:
   
   fatal_exit()
   
   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "custmsys.hpp"
#include <windows.h>

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
void close_graph(void);
void remove_keyint(void);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */


/* ========================================================================
   Function    - _fatal_test
   Description - do bool expression check, fatal_exit on FALSE value
   					NOTE: see the macro fatal_test() in custmsys.hpp
   Returns     - void
   ======================================================================== */
void _fatal_test(Boolean fTest, char * FileName, int LineNumber)
{
	if(fTest == False)
	{
		fatal_exit("SOLAUDIO FAILED in %s at line %ld\n",FileName,(long)LineNumber);
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
	
	MessageBox (NULL, texbuffer, "FATAL ERROR", MB_OK);
	
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

void close_graph(void)
{
	// must be filled in by application code
}
void remove_keyint(void)
{
	// must be filled in by application code
}
