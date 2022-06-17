/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: scene.cpp  -
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

#include "FILEUTIL.H"
#include "SCNVER.HXX"


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
   Function    - mfReadVersion
   Description - Read the Version section of the scene file.
   Returns     - 
   ======================================================================== */
LONG SCENE_FILE_VERSION::mfReadVersion(FILE *fp)
{
	char cpBuffer[15];
	LONG Result;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (Result != EOF)
	{
		if (2 != sscanf(cpBuffer,"%hd.%hd", &fsMajor, &fsMinor))
		{
			printf("WARNING! Scene version file not set correctly, \"%s\"\n",
			       cpBuffer);
	        // Set them so everything else won't blow up.
	        fsMajor = NOT_SET;
			fsMinor = NOT_SET;
		}
	}
		
	return Result;
}

