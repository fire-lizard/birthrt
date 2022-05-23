/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: scnmap.cpp  -
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

#include "SYSTEM.H"
#include "ENGINE.H"
#include "MACHINE.H"

#include "SCENE.HXX"
#include "SCNMAP.HXX"

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
   Function    - mfReadMap
   Description - Read the Map section of the scene file.
   Returns     - 
   ======================================================================== */
LONG MAP::mfReadMap(FILE *fp)
{
	char cpBuffer[40];
	LONG Result;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (Result != EOF)
	{
		LONG i;
		LONG count = atol(cpBuffer);
		
	    for (i = 0; i < count; i++)
	    {
			char cpKeyword[20];
			char cpValue[20];
			
			Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
			
			if (Result != EOF && 2 == sscanf(cpBuffer,"%s %s", cpKeyword, cpValue))
			{
				if (strcmp(cpKeyword, "ZoomFactor") == 0)
				{
					mfSetZoomFactor(cpValue);
				}
				else
				if (strcmp(cpKeyword, "MapState") == 0)
				{
					mfSetMapState(cpValue);
				}
			}
		}
	}
	
	return Result;
}
