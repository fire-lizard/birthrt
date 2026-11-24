/* ========================================================================
   Copyright (c) 1990,1995	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: BLOODREQ.c  -
   Author:   Gary Powell

   ========================================================================

   Contains the following general functions:

   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#include "SYSTEM.H"

#include "BLOODREQ.HXX"
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
   Typedefs
   ------------------------------------------------------------------------ */
// Note: I may want to percolate this information back to the user.
typedef enum {
	CHECK_BLOODLINE_ROLL_CANT_KEEP	= 0,
	CHECK_BLOODLINE_ROLL_EXACT_MATCH,
	CHECK_BLOODLINE_ROLL_WILL_FIT
} CHECK_BLOODLINE_ROLL_RESULT;

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

class BLOOD_ABILITY_REQ {
public:
	SBYTE	StrengthReq[BLOODLINE_STRENGTH_INFO::GREAT + 1];
	SBYTE	DerivationRew[BLOODLINE_DERIVATION_INFO::MAX_DERIVATION];
};

typedef BLOOD_ABILITY_REQ *PTR_BLOOD_ABILITY_REQ;

class BLOOD_ABILITY_GAINED {
public:
	typedef enum {
		NONE,
		MINOR,
		MAJOR,
		GREAT,
		ENHANCED_TO_MAJOR,
		ENHANCED_TO_GREAT
	} TYPE;

	SHORT	sHighLimit;
	TYPE	BloodAbilityGain;
};

typedef BLOOD_ABILITY_GAINED *PTR_BLOOD_ABILITY_GAINED;

#define MAX_BLOOD_ABLITY_GAIN	6 	// 6 is enough space for the largest subArray.

typedef struct _BLOOD_ABILITY_ACQUISITION {
	SBYTE						sBloodStrengthHighValue;
	SBYTE                       sNumberOfGainedValues;
	BLOOD_ABILITY_GAINED		BloodAbilityGain[MAX_BLOOD_ABLITY_GAIN];
} BLOOD_ABILITY_ACQUISITION, *PTR_BLOOD_ABILITY_ACQUISITION;

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

#if defined(_CHARED)			// Character Editor only (CHARED.CPP)
/* =======================================================================
   Function    - RollBloodlineDerivation
   Description - Generate a Bloodline Derivation value for a given player.
                 Taken from Birthright Rulebook p. 21
				 Note: Players may select attribute this vs rolling for it.
   Returns     - Bloodline Derivation
   ======================================================================== */
BLOODLINE_DERIVATION_INFO::TYPE RollBloodlineDerivation()
{
	const DICE	dice = {1, 100, 0};
	const SHORT	sRollResult = RollDice(&dice);
	BLOODLINE_DERIVATION_INFO::TYPE BloodDerivation;

	if (sRollResult == 0 || sRollResult > 90)
	{
		BloodDerivation = BLOODLINE_DERIVATION_INFO::AZRAI;
	}
	else if (sRollResult < 21 )
	{
		BloodDerivation = BLOODLINE_DERIVATION_INFO::ANDUIRAS;
	}
	else if (sRollResult < 36)
	{
		BloodDerivation = BLOODLINE_DERIVATION_INFO::REYNIR;
	}
	else if (sRollResult < 53)
	{
		BloodDerivation = BLOODLINE_DERIVATION_INFO::BRENNA;
	}
	else if (sRollResult < 71)
	{
		BloodDerivation = BLOODLINE_DERIVATION_INFO::BASAIA;
	}
	else if (sRollResult < 81)
	{
		BloodDerivation = BLOODLINE_DERIVATION_INFO::MASELA;
	}
	else if (sRollResult < 91)
	{
		BloodDerivation = BLOODLINE_DERIVATION_INFO::VORYNN;
	}

	return BloodDerivation;
}

/* =======================================================================
   Function    - RollBloodlineStrength
   Description - Generate a Bloodline Strength value for a given player.
                 Taken from Birthright Rulebook p. 20
   Returns     - Bloodline Strength Type
   ======================================================================== */
BLOODLINE_STRENGTH_INFO::TYPE	RollBloodlineStrength(SHORT *psStrength)
{
	DICE dice = {1, 100, 0};
	BLOODLINE_STRENGTH_INFO::TYPE BloodStrengthType;

	*psStrength = RollDice(&dice);

	if (*psStrength == 0 || *psStrength > 95)
	{
		BloodStrengthType = BLOODLINE_STRENGTH_INFO::GREAT;
	}
	else if ( *psStrength < 26)
	{
		BloodStrengthType = BLOODLINE_STRENGTH_INFO::TAINTED;
	}
	else if ( *psStrength < 66)
	{
		BloodStrengthType = BLOODLINE_STRENGTH_INFO::MINOR;
	}
	else if ( *psStrength < 96)
	{
		BloodStrengthType = BLOODLINE_STRENGTH_INFO::MAJOR;
	}

	return BloodStrengthType;
}

/* =======================================================================
   Function    - RollBloodlineNumericalStrength
   Description - Generate numerical (as opposed to descriptive) stength.
                 Taken from Michael Ormsby's notes on Modifying Birthright I
				  into Birthright II.
   Returns     - Numerical strength.
	Notes			- Call this AFTER calling RollBloodlineStrength.
				  [d5-12-97 JPC]
   ======================================================================== */
LONG RollBloodlineNumericalStrength (BLOODLINE_STRENGTH_INFO::TYPE BloodStrengthType)
{
	DICE dice = {4, 6, 0};

	switch (BloodStrengthType)
	{
		case BLOODLINE_STRENGTH_INFO::TAINTED:
			dice.sbNumberOfDice = 4;
			break;
		case BLOODLINE_STRENGTH_INFO::MINOR:
			dice.sbNumberOfDice = 5;
			break;
		case BLOODLINE_STRENGTH_INFO::MAJOR:
		case BLOODLINE_STRENGTH_INFO::GREAT:
			dice.sbNumberOfDice = 8;
			break;
	}

	return RollDice (&dice);
}
#endif	// _CHARED
