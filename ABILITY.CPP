/* ========================================================================
   Copyright (c) 1990,1995	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: ABILITY.cpp  -
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "ABILITY.HXX"
#include "DICE.H"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#if defined(_CHARED)			// The rest of this file is used only by the Character Editor
#define RACEABILITY_MIN_VALUES		0
#define RACEABILITY_MAX_VALUES		1
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
static void GetRaceAbilities(
	const RACE_INFO::TYPE	/* PlayerRace */,
	const ABILITY::TYPE 	/* AbilityType */,
	SBYTE * const 			/* psHighValue */,
	SBYTE * const	      	/* psLowValue */);
static const SBYTE GetExceptionalStrength(
	const SHORT RollValue);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
// Note: Instead of using an array of the number of races I've limited
//       to just the races which have Birthright defined limits. Otherwise
//       we'd have all 5 Human types with no limit.

class RACEABILITYREQ {
public:
	RACEABILITYREQ(const ABILITY &/* Dwarf */,
				const ABILITY &/* Elf */,
				const ABILITY &/* HalfElf */,
				const ABILITY &/* Halfling */);
	~RACEABILITYREQ() {}
	
	const ABILITY		&fDwarf;
	const ABILITY		&fElf;
	const ABILITY		&fHalfElf;
	const ABILITY		&fHalfling;
};
typedef RACEABILITYREQ *PTR_RACEABILITYREQ;

// Constructor
inline RACEABILITYREQ::RACEABILITYREQ(const ABILITY & Dwarf,
								const ABILITY & Elf,
								const ABILITY & HalfElf,
								const ABILITY & Halfling) :
	fDwarf(Dwarf),
	fElf(Elf),
	fHalfElf(HalfElf),
	fHalfling(Halfling)
{
}

static const RACEABILITYREQ RaceAbilityReq[] = {
	RACEABILITYREQ(		// Minimum Values
	   //  S   D  Cn   I   W  Ch
		ABILITY( 13,  3, 13,  3,  3,  3),		// Dwarf
		ABILITY(  5,  6,  6,  8,  3, 10),		// Elf
		ABILITY(  4,  6,  3,  6,  3,  6),		// HalfElf
		ABILITY(  3, 10,  8,  6,  6,  3)		// Halfling
	),
	RACEABILITYREQ(		// Maximum Values
	   //  S   D  Cn   I   W  Ch
		ABILITY( 18, 16, 18, 18, 18, 17),		// Dwarf
		ABILITY( 18, 18, 18, 18, 18, 18),		// Elf
		ABILITY( 18, 18, 18, 18, 18, 18),		// HalfElf
		ABILITY( 18, 18, 18, 18, 18, 18)		// Halfling
	)
};

/* =======================================================================
   Function    - GetRaceAbilties
   Description - Looks up the Ability Min and Max based for a given TYPE
                 If not found, don't modify the input values for the range.
   Returns     - void
   ======================================================================== */
static void GetRaceAbilities(
	const RACE_INFO::TYPE PlayerRace,
	const ABILITY::TYPE AbilityType,
	SBYTE * const psHighValue,
	SBYTE * const psLowValue)
{
	switch (PlayerRace)
	{
	case RACE_INFO::DWARF:
		*psHighValue = RaceAbilityReq[RACEABILITY_MAX_VALUES].fDwarf.mfGetAbility(AbilityType);
		*psLowValue = RaceAbilityReq[RACEABILITY_MIN_VALUES].fDwarf.mfGetAbility(AbilityType);
		break;
	case RACE_INFO::ELF:
		*psHighValue = RaceAbilityReq[RACEABILITY_MAX_VALUES].fElf.mfGetAbility(AbilityType);
		*psLowValue = RaceAbilityReq[RACEABILITY_MIN_VALUES].fElf.mfGetAbility(AbilityType);
		break;
	case RACE_INFO::HALF_ELF:
		*psHighValue = RaceAbilityReq[RACEABILITY_MAX_VALUES].fHalfElf.mfGetAbility(AbilityType);
		*psLowValue = RaceAbilityReq[RACEABILITY_MIN_VALUES].fHalfElf.mfGetAbility(AbilityType);
		break;
	case RACE_INFO::HALFLING:
		*psHighValue = RaceAbilityReq[RACEABILITY_MAX_VALUES].fHalfling.mfGetAbility(AbilityType);
		*psLowValue = RaceAbilityReq[RACEABILITY_MIN_VALUES].fHalfling.mfGetAbility(AbilityType);
		break;
	}
}
/* =======================================================================
   Function    - RollAbility
   Description - Generate a valid Ability value for a given player Race.
                 Don't call this for AbilityType ABILITY_STRENGTH! (The table
				 is too weird.)
   Returns     - Ability Value
   ======================================================================== */
const SBYTE RollAbility(
	const RACE_INFO::TYPE PlayerRace,	// PlayerRace can be RACE_NOT_SET
	const ABILITY::TYPE	AbilityType,
	SBYTE sDefaultHighValue,
	SBYTE sDefaultLowValue)
{
	SBYTE	sResult = 0;
	DICE	Dice = {3, 6, 0};

	// If no race is set, we'll use the default values.
	GetRaceAbilities(PlayerRace, AbilityType, &sDefaultHighValue, &sDefaultLowValue);

	do
	{
		sResult = (SBYTE)RollDice(&Dice);
	}
	while (sResult < sDefaultLowValue && sResult > sDefaultHighValue);

	return sResult;

}

/* =======================================================================
   Function    - GetExceptionalStrength
   Description - Calculate the Strength Data for the Strength Data Table
				 Values from AD&D Player Handbook p.19
   Returns     - Increment value necessary.
   ======================================================================== */
static const SBYTE GetExceptionalStrength(const SHORT RollValue)
{
	SBYTE sAddFactorRtn = 0;

	if (RollValue == 0)
	{
		sAddFactorRtn = 5;
	}
	else if (RollValue < 51)
	{
		sAddFactorRtn = 1;
	}
	else if (RollValue < 76)
	{
		sAddFactorRtn = 2;
	}
	else if (RollValue < 91)
	{
		sAddFactorRtn = 3;
	}
	else
	{
		sAddFactorRtn = 4;
	}

	return sAddFactorRtn;
}

/* =======================================================================
   Function    - RollStrength
   Description - Generate a valid Strength value for a given player Race.
   Returns     - Strength Value
   ======================================================================== */
const SBYTE RollStrength(
	const RACE_INFO::TYPE PlayerRace)	// PlayerRace can be RACE_NOT_SET
{
	// First roll the basic range.
	SBYTE	sResult = RollAbility(PlayerRace,
		                          ABILITY::STRENGTH,
								  MAX_ROLLED_STRENGTH_RATING,
								  MIN_ROLLED_STRENGTH_RATING);
	if (sResult == 18)
	{
		DICE	Dice = {1, 100, 0};
		SHORT sSecondRoll = RollDice(&Dice);

		sResult += GetExceptionalStrength (sSecondRoll);
	}

	return sResult;
}
#endif	// _CHARED

