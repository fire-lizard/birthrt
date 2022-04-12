/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: random.c  -
   Author:   David L Jackson

   ========================================================================
   Contains the following general functions:

   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>		// for FILE*.
#include <stdlib.h>		// for rand.
#include <string.h>		// for rand.

#include "typedefs.h"

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
#ifdef _WINDOWS
BOOL IsMultiPlayer ( void );
#endif

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
BOOL	fLogComment = FALSE;

/* ------------------------------------------------------------------------
   External Variables
   ------------------------------------------------------------------------ */
static FILE *pFile = NULL; 
static int  fWrite = 0;
static char temp[1500];

long numrands = 0;

unsigned long lastrand = 0;

void RandomLogOpen ( void )
{
	pFile = fopen ( "rand.log", "a"); 
}

void RandomLogClose ( void )
{
	if ( pFile )
	{
		fclose ( pFile );
	}
}

void RandomLogWrite ( unsigned long therand, char * szFileName, int iLineNumber )
{
	if ( fWrite )
	{
		numrands++;
		RandomLogOpen();
		if ( pFile )
		{
			fprintf ( pFile,
						//	"%sRand %6.6lu %12.12s line %4.4d\n",
							"%sR%lu %12.12s@%d\n",
							temp,
							therand, 
							szFileName,
							iLineNumber );
			sprintf (temp, "");
		}
		RandomLogClose();
	}
}

void RandomLogComment ( char * szString )
{
	RandomLogOpen();
	if ( pFile )
	{
		fWrite = 1;   //---- Start logging

		fprintf ( pFile, 
				   "%s%s %s\n",
#ifdef _WINDOWS
					(IsMultiPlayer())?"^":"v",
#else
					" ",
#endif
					temp,
				   szString );
		sprintf (temp, "");
	}
	RandomLogClose();
}

/* ========================================================================
   Function    - RandomLogPrefix
   Description - add a strin to the running prefix string: temp
   Returns     - 
   ======================================================================== */
void RandomLogPrefix ( char * szString )
{
	if ( pFile && strlen(temp) < 1400)
		//sprintf(temp, "%s%s ",temp, szString);
		strcat(temp, szString);
}


/* ========================================================================
   Function    - LogFlush
   Description - flush the log from the buffer to the file before we crash
   Returns     - 
   ======================================================================== */
void LogFlush ( void )
{
	RandomLogOpen();
	fflush ( pFile );
	RandomLogClose();
}


/* ======================================================================== */

unsigned long lograndom ( char * szFileName, int iLineNumber )
{
	lastrand = rand();

	// -- This logging is too abusive
	// -- GEH RandomLogWrite ( lastrand, szFileName, iLineNumber );

	return lastrand;

}

void InitRandomLog(void)
{
	pFile = fopen ( "rand.log", "w"); 
	if ( pFile )
	{
		lastrand = 0;
		fclose ( pFile );
	}
}


/* ======================================================================== */
