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

// Data from Birthright Rules Handbook p.22 Table 13
BLOOD_ABILITY_REQ BloodAbilityReqTable[] = {
//     T Mi Ma  G    An R Br Ba  M  V Az
	{{ 0, 1, 0, 0},{ 0, 1, 1, 1, 0, 0, 1}},		// Alertness
	{{ 0, 0, 1, 0},{ 0, 0, 1, 0, 0, 1, 1}},		// Alter Appearance
	{{ 0, 1, 1, 1},{ 1, 1, 1, 1, 1, 1, 1}},		// Animal Affinity
	{{ 0, 0, 1, 0},{ 1, 0, 0, 0, 0, 0, 1}},		// Battlewise
	{{ 0, 1, 0, 0},{ 0, 0, 1, 0, 1, 1, 0}},		// Blood History
	{{ 0, 1, 0, 0},{ 1, 1, 1, 1, 1, 1, 1}},		// Bloodmark
	{{ 0, 0, 1, 0},{ 0, 0, 1, 1, 0, 1, 0}},		// Character Reading
	{{ 0, 1, 1, 1},{ 1, 0, 0, 0, 0, 0, 0}},		// Courage
	{{ 0, 1, 0, 0},{ 1, 1, 1, 1, 1, 1, 0}},		// Detect Lie
	{{ 0, 1, 0, 0},{ 0, 1, 1, 1, 1, 1, 1}},		// Detect Illusion
	{{ 0, 1, 0, 0},{ 0, 1, 0, 0, 1, 0, 0}},		// Direction Sense
	{{ 0, 0, 1, 1},{ 1, 1, 0, 1, 1, 1, 1}},		// Devine Aura
	{{ 0, 0, 1, 0},{ 1, 0, 0, 1, 1, 0, 0}},		// Devine Wrath
	{{ 0, 0, 0, 1},{ 1, 1, 0, 1, 1, 0, 0}},		// Elemental Control
	{{ 0, 1, 1, 0},{ 1, 1, 1, 1, 1, 1, 1}},		// Enhanced Sense
	{{ 0, 0, 1, 0},{ 0, 0, 0, 0, 0, 0, 1}},		// Fear
	{{ 0, 1, 1, 1},{ 1, 1, 0, 1, 0, 0, 0}},		// Healing
	{{ 0, 1, 0, 0},{ 1, 1, 1, 1, 0, 1, 1}},		// Heightened Ability
	{{ 0, 1, 0, 0},{ 1, 1, 0, 0, 0, 0, 1}},		// Iron Will
	{{ 0, 0, 1, 0},{ 0, 0, 1, 0, 0, 0, 1}},		// Persuasion
	{{ 0, 1, 0, 0},{ 0, 1, 0, 0, 0, 0, 1}},		// Poison Sense
	{{ 0, 0, 1, 0},{ 1, 1, 1, 1, 1, 1, 0}},		// Protection from Evil
	{{ 0, 0, 0, 1},{ 1, 1, 0, 0, 0, 0, 1}},		// Regeneration
	{{ 0, 1, 1, 1},{ 1, 1, 1, 1, 1, 1, 1}},		// Resistance
	{{ 0, 0, 0, 1},{ 0, 0, 1, 0, 0, 0, 1}},		// Shadow Form
	{{ 0, 0, 0, 1},{ 0, 0, 0, 0, 0, 0, 1}},		// Touch Of Decay
	{{ 0, 0, 0, 1},{ 0, 0, 1, 1, 1, 1, 1}},		// Travel
	{{ 0, 1, 0, 0},{ 1, 1, 1, 1, 1, 1, 1}}		// Unreadable Thoughts
};

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

// Table 12 from the Birthright Rulebook p.22
BLOOD_ABILITY_ACQUISITION BloodAbilityAcquisitionTable [] = {
	{11, 2, {{ 91, BLOOD_ABILITY_GAINED::NONE},
			 {100, BLOOD_ABILITY_GAINED::MINOR}}},

	{19, 4, {{ 16, BLOOD_ABILITY_GAINED::NONE},
			 { 81, BLOOD_ABILITY_GAINED::MINOR},
			 { 96, BLOOD_ABILITY_GAINED::ENHANCED_TO_MAJOR},
			 {100, BLOOD_ABILITY_GAINED::MAJOR}}},

	{29, 6, {{ 11, BLOOD_ABILITY_GAINED::NONE},
			 { 36, BLOOD_ABILITY_GAINED::MINOR},
			 { 46, BLOOD_ABILITY_GAINED::ENHANCED_TO_MAJOR},
			 { 97, BLOOD_ABILITY_GAINED::MAJOR},
			 { 99, BLOOD_ABILITY_GAINED::ENHANCED_TO_GREAT},
			 {100, BLOOD_ABILITY_GAINED::GREAT}}},

	{36, 4, {{ 51, BLOOD_ABILITY_GAINED::NONE},
			 { 81, BLOOD_ABILITY_GAINED::MINOR},
			 { 91, BLOOD_ABILITY_GAINED::ENHANCED_TO_MAJOR},
			 {100, BLOOD_ABILITY_GAINED::MAJOR}}},

	{51, 6, {{  6, BLOOD_ABILITY_GAINED::NONE},
			 { 16, BLOOD_ABILITY_GAINED::MINOR},
			 { 27, BLOOD_ABILITY_GAINED::ENHANCED_TO_MAJOR},
			 { 41, BLOOD_ABILITY_GAINED::MAJOR},
			 { 51, BLOOD_ABILITY_GAINED::ENHANCED_TO_GREAT},
			 {100, BLOOD_ABILITY_GAINED::GREAT}}},

	{81, 5, {{ 11, BLOOD_ABILITY_GAINED::NONE},
			 { 26, BLOOD_ABILITY_GAINED::ENHANCED_TO_MAJOR},
			 { 41, BLOOD_ABILITY_GAINED::MAJOR},
			 { 71, BLOOD_ABILITY_GAINED::ENHANCED_TO_GREAT},
			 {100, BLOOD_ABILITY_GAINED::GREAT}}},

	{100, 5,{{ 51, BLOOD_ABILITY_GAINED::ENHANCED_TO_GREAT},
			 {100, BLOOD_ABILITY_GAINED::GREAT}}}
};

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
static CHECK_BLOODLINE_ROLL_RESULT CheckBloodlineAbilityRoll(
	BLOOD_ABILITY_INFO::BLOOD_ABILITY/* BloodAbilityRolled */,
	PTR_PLAYER_BLOOD_INFO			/* pPlayerBloodInfo */,
	PTR_BLOODLINE_ACQUISITION_LIST	/* pBloodlineAcquisitionAvail */,
	PTR_BLOOD_ABILITIES_LIST		/* pBloodAbilitiesRefused */);

static void RemoveBloodlineAcquisitionItem(
	PTR_BLOODLINE_ACQUISITION_LIST * /* pTempBloodlineAcquisitionList */,
	PTR_BLOODLINE_ACQUISITION_LIST   /* pBloodlineAcquisitionAvail */);

static void AddBloodlineAbilityAcquisition(
	PTR_BLOODLINE_ACQUISITION_LIST * /* ppBloodAcquisitionList */,
	BLOOD_ABILITY_GAINED::TYPE 		 /* NewStrength */);

static void ModifyBloodlineAbilityAcquisition(
	PTR_BLOODLINE_ACQUISITION_LIST * /* pBloodAcquistionList */,
	BLOOD_ABILITY_GAINED::TYPE 		 /* OldStrength */,
	BLOOD_ABILITY_GAINED::TYPE 		 /* NewStrength */);

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



