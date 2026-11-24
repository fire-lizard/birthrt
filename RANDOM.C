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

#include "TYPEDEFS.H"

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

void RandomLogComment ( char * szString )
{
	RandomLogOpen();
	if ( pFile )
	{
		fWrite = 1;   //---- Start logging

		fprintf ( pFile, 
				   "%s%s %s\n",
					(IsMultiPlayer())?"^":"v",
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
		strcat(temp, szString);
}

/* ======================================================================== */

unsigned long lograndom ( char * szFileName, SHORT iLineNumber )
{
	lastrand = rand();

	// -- This logging is too abusive

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
