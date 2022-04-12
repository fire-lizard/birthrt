/* =®RM250¯=======================================================================
   Copyright (c) 1990,1995	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: DICE.c  - Handle Dice rolling.
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:
	RollDice - Rolls a AD&D Die, ie 2d7+2
   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdlib.h>
#include "SYSTEM.H"
#include "DICE.H"

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
void RandomLogPrefix ( char * szString );

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

/* =======================================================================
   Function    - RollDice
   Description - Roll the appropriate number of dice the correct number of
                 times. (Specified by the DICE structure)
   Returns     - Result of the roll.
   ======================================================================== */
#if defined (_WINDOWS) && defined(_DEBUG)
SHORT RollDice_( DICE const * const pDiceInfo,char const * const FileName ,LONG LineNum )
#else
SHORT RollDice( DICE const * const pDiceInfo)
#endif
{
	SHORT sResult = 0;
	LONG i;

	for (i = 0; i < pDiceInfo->sbNumberOfDice; i++)
	{
		SHORT sRoll;

		// Generate a random number between 1 and the Number of sides on the dice.
#if defined (_WINDOWS) && defined(_DEBUG)
//		char temp[100];
//		sprintf(temp, "%12.12s%@%d", FileName, LineNum);
//		RandomLogPrefix(temp);
#endif

		sRoll = random(pDiceInfo->sbNumberOfSides)+1;

		sResult += sRoll;
	}

	sResult += pDiceInfo->sbModifier;
	// printf("Roll %dd%d+%d = %d\n",pDiceInfo->sbNumberOfDice,pDiceInfo->sbNumberOfSides,pDiceInfo->sModifier,sResult);
	return sResult;
}

/* =======================================================================
   Function    - AveDice
   Description - Return the average of the specified dice plus the modifier
   Returns     - Result of the roll.
   ======================================================================== */
SHORT AveDice( DICE const * const pDiceInfo)
{
	SHORT sResult = 0;
	LONG i;

	for (i = 0; i < pDiceInfo->sbNumberOfDice; i++)
	{
		SHORT sRoll;
		// Generate a random number between 1 and the Number of sides on the dice.
		sRoll = ((pDiceInfo->sbNumberOfSides+1)*32)/2;

		sResult += sRoll;
	}
	sResult = sResult/32;

	sResult += pDiceInfo->sbModifier;
	// printf("Ave %dd%d+%d = %d\n",pDiceInfo->sbNumberOfDice,pDiceInfo->sbNumberOfSides,pDiceInfo->sModifier,sResult);
	return sResult;
}

/* ======================================================================= */


