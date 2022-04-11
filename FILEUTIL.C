/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: fileutil.c
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <ctype.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "typedefs.h"

#include "fileutil.h"

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
   Function    - GetNextLine
   Description - Gets the next line of the file. Skip comments ie '#' in 
                 column zero.
                 Also skip leading white space. (And thus blank lines.)
   Returns     - Either EOF or the number of characters retrieved.
   ======================================================================== */
LONG GetNextLine(FILE *fp, char *cpNewLine, LONG len)
{
	LONG result = 0;
	do
	{
		LONG i;
		
		cpNewLine[0] = 0; // Initialize it incase the read fails.
		if( NULL == fgets(cpNewLine, len, fp))
		{
			result = EOF;
			break;
		}
		
		// Advance past the white space.
		for (i = 0; isspace(cpNewLine[i]) && cpNewLine[i] != 0; i++ )
		{
		}
		
		if (cpNewLine[i] == 0)
		{
			// End of line found.
			cpNewLine[0] = 0;
		}
		else
		if (i > 0) // Leading white space found.
		{
			char *cpShiftedLine;
			char *cpBeginText;
			
			cpShiftedLine = cpNewLine;
			cpBeginText = cpNewLine + i;
			// Cheap inplace string shift.
			do
			{
				*cpShiftedLine = *cpBeginText;
				cpBeginText++;
				cpShiftedLine++;
			}
			while(*cpBeginText);
			
			*cpShiftedLine = 0;
		}
	
	} while (cpNewLine[0] == '#' || 
	         cpNewLine[0] == 0 || 
	         cpNewLine[0] ==  '\n');
	
	if (result != EOF)
	{
		LONG LastCharIndex;
		
		result = strlen(cpNewLine);
		
		
		// Trim off the trailing <LF> <CR> and white space
		for (LastCharIndex = result - 1;
		     LastCharIndex > - 1 && 
		     (cpNewLine[LastCharIndex] == '\n' ||
		      cpNewLine[LastCharIndex] == '\r' ||
		      isspace(cpNewLine[LastCharIndex]));
			 LastCharIndex--, result--)
		{
			cpNewLine[LastCharIndex] = 0;
		}
	}
	
	return result;
}

