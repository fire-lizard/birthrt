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
#include "ability.hxx"
#include "dice.h"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define RACEABILITY_MIN_VALUES		0
#define RACEABILITY_MAX_VALUES		1
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
static void LimitAdjustment(SBYTE *const spValue);
static void GetRaceAbilities(
	const RACE_INFO::TYPE	/* PlayerRace */, 
	const ABILITY::TYPE 	/* AbilityType */,
	SBYTE * const 			/* psHighValue */,
	SBYTE * const	      	/* psLowValue */);

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
	//const ABILITY		&fGnome;
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

// Table of mandatory adjustments if you are one of these races.
static const ABILITY	RaceAdjustment[] = {
//     S   D  Cn   I   W  Ch
	ABILITY(  0, -2,  2,  0,  0,  0),		// Dwarf
	ABILITY( -1,  1, -1,  1,  0,  0),		// Elf
	ABILITY(  0,  1, -1,  0,  0,  0),		// HalfElf (No Change)
	ABILITY( -2,  1,  0,  0,  1,  0),		// Halfling
	ABILITY(  0, -1,  0,  0,  1,  0),		// Anuirean
	ABILITY(  0,  1,  0,  0, -1,  0),		// Brecht
	ABILITY(  0,  0, -1,  1,  0,  0),		// Khinasi
	ABILITY(  0,  0,  1,  0,  0, -1),		// Rjurik
	ABILITY(  1,  0,  0, -1,  0,  0)		// Vos
};

#define MAX_ADJUSTMENT_VALUE	20
#define MIN_ADJUSTMENT_VALUE	3

static const SBYTE GetExceptionalStrength(
	const SHORT RollValue);


/* =======================================================================
   Function    - static LimitAdjustment
   Description - Limit the Adjustment to reasonable values
   Returns     - void
   ======================================================================== */
static void LimitAdjustment(SBYTE * const spValue)
{
	if (*spValue > MAX_ADJUSTMENT_VALUE)
	{
		*spValue = MAX_ADJUSTMENT_VALUE;
	}
	else if (*spValue < MIN_ADJUSTMENT_VALUE)
	{
		*spValue = MIN_ADJUSTMENT_VALUE;
	}
}

/* =======================================================================
   Function    - mfAdjustAbilty
   Description - Looks up the Ability Adjustment based on Race and adjust the
                 numbers accordingly
   Returns     - void
   ======================================================================== */
inline void ABILITY::mfAdjustAbilities(
	ABILITY const *pAdjustment,
	const ABILITY::TYPE  AbilityType)
{
	fAbilities[AbilityType] += pAdjustment->fAbilities[AbilityType];
	LimitAdjustment(&fAbilities[AbilityType]);
}

/* =======================================================================
   Function    - mfAdjustAllAbilties
   Description - Looks up the Ability Adjustment based on Race and call the
                 proper adjustment routine.
   Returns     - void
   ======================================================================== */
void ABILITY::mfAdjustAllAbilities (
	const RACE_INFO::TYPE Race)
{
	if (Race != RACE_INFO::NOT_SET)
	{
		ABILITY const *paAdjustment = &RaceAdjustment[Race];

		mfAdjustAbilities(paAdjustment, STRENGTH);
		mfAdjustAbilities(paAdjustment, DEXTERITY);
		mfAdjustAbilities(paAdjustment, CONSTITUTION);
		mfAdjustAbilities(paAdjustment, INTELLIGENCE);
		mfAdjustAbilities(paAdjustment, WISDOM);
		mfAdjustAbilities(paAdjustment, CHARISMA);
	}
}

/* =======================================================================
   Function    - mfChangeRaceAbilities
   Description - Looks up the Ability Adjustment based on Race and change the
                 numbers accordingly
   Returns     - void
   ======================================================================== */
void ABILITY::mfChangeRaceAbilities(
	const RACE_INFO::TYPE OldRace, 
	const RACE_INFO::TYPE NewRace)
{
	if (OldRace != RACE_INFO::NOT_SET)
	{
		ABILITY const *paOldAdjustment = &RaceAdjustment[OldRace];

		fAbilities[STRENGTH]		-= paOldAdjustment->fAbilities[STRENGTH];
		fAbilities[DEXTERITY]	   	-= paOldAdjustment->fAbilities[DEXTERITY];
		fAbilities[CONSTITUTION]	-= paOldAdjustment->fAbilities[CONSTITUTION];
		fAbilities[INTELLIGENCE]	-= paOldAdjustment->fAbilities[INTELLIGENCE];
	   	fAbilities[WISDOM]	   		-= paOldAdjustment->fAbilities[WISDOM];
		fAbilities[CHARISMA]		-= paOldAdjustment->fAbilities[CHARISMA];
	}
	mfAdjustAllAbilities(NewRace);	
}

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

/* =======================================================================
   Function    - RollAllAbilities
   Description - Generate a valid Charisma value for a given player Race.
   Returns     - Charisma Value
   ======================================================================== */
void ABILITY::RollAllAbilities(
	const RACE_INFO::TYPE PlayerRace	// PlayerRace can be RACE_NOT_SET
	)
{
	fAbilities[STRENGTH]		= RollStrength(PlayerRace);
	fAbilities[DEXTERITY]		= RollDexterity(PlayerRace);
	fAbilities[CONSTITUTION]	= RollConstitution(PlayerRace);
	fAbilities[INTELLIGENCE]	= RollIntelligence(PlayerRace);
	fAbilities[WISDOM]	  	 	= RollWisdom(PlayerRace);
	fAbilities[CHARISMA]		= RollCharisma(PlayerRace);
}
