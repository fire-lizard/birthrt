/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: FILENAME.cpp  -
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#include "stdio.h"
#include "stdlib.h"

#include "playstat.hxx"
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

/* ------------------------------------------------------------------------
   Local Variables
   ------------------------------------------------------------------------ */
   
int main(int Argv, char *Argc[])
{
	LONG i;
	int Result = EXIT_SUCCESS;
	
	for (i = 1; i < Argv; i++)
	{
		LONG id;
		char BinaryAvaFile[80];
		LONG const count = sscanf(Argc[i],"scenes\\id%ld.ava", &id);
		PLAYER_STATS playerStat;
		
		if (count < 1)
		{
			printf("Makeavd: ERROR! Bad argument \"%s\"\n", Argc[i]);
			Result = EXIT_FAILURE;
			continue;
		}
		
		printf("Reading %s ....\n", Argc[i]);
		FILE * fp = fopen(Argc[i],"r");
		if (fp == 0)
		{
			printf("Makeavd: ERROR! Unable to open text data file \"%s\"\n", Argc[i]);
			Result = EXIT_FAILURE;
			continue;
		}
		
		
		if (EOF == playerStat.mfReadTextData(fp))
		{
			printf("Makeavd: ERROR! Bad data in Text file \"%s\"\n", Argc[i]);
			Result = EXIT_FAILURE;
			fclose(fp);
			continue;
		}
		
		fclose(fp);
		
		sprintf(BinaryAvaFile,"scenes\\id%ld.avd", id);
		printf("Writing %s ....\n", BinaryAvaFile);
		
		fp = fopen(BinaryAvaFile,"wb");
		if (fp == 0)
		{
			printf("Makeavd: ERROR! Unable to create Binary file \"%s\"\n", BinaryAvaFile);
			Result = EXIT_FAILURE;
			continue;
		}
		playerStat.mfWriteData(fp);
		fclose(fp);
	}
	
	return Result;
}
