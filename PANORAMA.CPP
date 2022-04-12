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
#include "PANORAMA.HXX"


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
   Function    - mfReadData
   Description - Read the Panorama section of the scene file.
   Returns     - 
   ======================================================================== */
LONG PANORAMA_DATA::mfReadData(FILE *fp)
{
	LONG Result;
	LONG i;
	SHORT *phPanoramaFileNames;
	
	// Just get the memory once per scene.
	if (fhPanoramaFileNames == fERROR)
	{
		fhPanoramaFileNames = NewBlock(sizeof(SHORT) * MAX_PANORAMAS);
		if (fhPanoramaFileNames != fERROR)
		{
			phPanoramaFileNames = (SHORT *) BLKPTR(fhPanoramaFileNames);
		
			// initialize the array.
			for (i = 0; i < MAX_PANORAMAS ; i++)
			{
				phPanoramaFileNames[i] = NewBlock(sizeof(char) * MAX_FILENAME_LEN);
			}
		}
	}
	
	if (fhPanoramaFileNames != fERROR)
	{
		char cpBuffer[80];
		
		SetBlockAttr(fhPanoramaFileNames, LOCKED, LOCKED);
		phPanoramaFileNames = (SHORT *) BLKPTR(fhPanoramaFileNames);
		
		i = 0;
		
		do
		{
			Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
			if (Result != EOF)
			{
				if (phPanoramaFileNames[i] != fERROR)
				{
					char *cpFileName = (char *) BLKPTR(phPanoramaFileNames[i]);
					strncpy(cpFileName, cpBuffer, MAX_FILENAME_LEN - 1);
					cpFileName[MAX_FILENAME_LEN - 1] = 0;
					i++;
				}
				else
				{
					Result = EOF;  // Out of memory.
				}
			}
		}
		while (Result != EOF && i < MAX_PANORAMAS);
		
		ClrLock(fhPanoramaFileNames);
	}
	else
	{
		Result = EOF; // Out of memory.
	}
		
	if (Result != EOF)
	{
		hSky_filenames = fhPanoramaFileNames;
	}
	return Result;
}
