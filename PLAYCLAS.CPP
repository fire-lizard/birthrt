/* ========================================================================
   Copyright (c) 1990,1995	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: playclas.cpp  - Player Class Information
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "TYPEDEFS.H"

#include "EXPERIEN.HXX"
#include "PLAYCLAS.HXX"
#include "strenum.h"   

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

// Note: This table came from the Birthright Rulebook p. 5 and are the upper
//       limits for a character's class for a given race.
typedef char	CLASS_LIMITS[PLAYER_CLASS_INFO::CLASS_MAX_NUMBER];	// Use the PLAYER_CLASS_TYPE to index

class PLAYER_CLASS_LIMITS {
public:
	CLASS_LIMITS	Dwarf;
	CLASS_LIMITS	Elf;
	CLASS_LIMITS	HalfElf;
	CLASS_LIMITS	Halfling;
};

typedef PLAYER_CLASS_LIMITS *PTR_PLAYER_CLASS_LIMITS;

// Note: This is Table 14 from the AD&D Handbook p.36
static const CLASS_HIT_DICE_TABLE FighterHitDiceTable[] = {
	{{0,  0,  0}, 20},	//  Level 0	(So that Level is a direct access to this table.)
	{{1, 10,  0}, 20},	//  1
	{{2, 10,  0}, 19},	//  2
	{{3, 10,  0}, 18},	//  3
	{{4, 10,  0}, 17},	//  4
	{{5, 10,  0}, 16},	//  5
	{{6, 10,  0}, 15},	//  6
	{{7, 10,  0}, 14},	//  7
	{{8, 10,  0}, 13},	//  8
	{{9, 10,  0}, 12},	//  9
	{{9, 10,  3}, 11},	// 10
	{{9, 10,  6}, 10},	// 11
	{{9, 10,  9},  9},	// 12
	{{9, 10, 12},  8},	// 13
	{{9, 10, 15},  7},	// 14
	{{9, 10, 18},  6},	// 15
	{{9, 10, 21},  5},	// 16
	{{9, 10, 24},  4},	// 17
	{{9, 10, 27},  3},	// 18
	{{9, 10, 30},  2},	// 19
	{{9, 10, 33},  1}	// 20
};	

// Note: This is Table 20 from the AD&D Handbook p.40
static const CLASS_HIT_DICE_TABLE WizardHitDiceTable[] = {
	{{ 0, 0,  0}, 20},	//  Level 0	(So that Level is a direct access to this table.)
	{{ 1, 4,  0}, 20},	//  1
	{{ 2, 4,  0}, 20},	//  2
	{{ 3, 4,  0}, 20},	//  3
	{{ 4, 4,  0}, 19},	//  4
	{{ 5, 4,  0}, 19},	//  5
	{{ 6, 4,  0}, 19},	//  6
	{{ 7, 4,  0}, 18},	//  7
	{{ 8, 4,  0}, 18},	//  8
	{{ 9, 4,  0}, 18},	//  9
	{{10, 4,  0}, 17},	// 10
	{{10, 4,  1}, 17},	// 11
	{{10, 4,  2}, 17},	// 12
	{{10, 4,  3}, 16},	// 13
	{{10, 4,  4}, 16},	// 14
	{{10, 4,  5}, 16},	// 15
	{{10, 4,  6}, 15},	// 16
	{{10, 4,  7}, 15},	// 17
	{{10, 4,  8}, 15},	// 18
	{{10, 4,  9}, 14},	// 19
	{{10, 4, 10}, 14}	// 20
};

// Note: This is Table 23 from the AD&D Handbook p.47
// Note: I'm not sure if we will have Druids or not. Until then I'm only putting
//       in the Cleric data table.
static const CLASS_HIT_DICE_TABLE ClericHitDiceTable[] = {
	{{ 0, 0,  0}, 20},	//  Level 0	(So that Level is a direct access to this table.)
	{{ 1, 8,  0}, 20},	//  1
	{{ 2, 8,  0}, 20},	//  2
	{{ 3, 8,  0}, 20},	//  3
	{{ 4, 8,  0}, 18},	//  4
	{{ 5, 8,  0}, 18},	//  5
	{{ 6, 8,  0}, 18},	//  6
	{{ 7, 8,  0}, 16},	//  7
	{{ 8, 8,  0}, 16},	//  8
	{{ 9, 8,  0}, 16},	//  9
	{{ 9, 8,  2}, 14},	// 10
	{{ 9, 8,  4}, 14},	// 11
	{{ 9, 8,  6}, 14},	// 12
	{{ 9, 8,  8}, 12},	// 13
	{{ 9, 8, 10}, 12},	// 14
	{{ 9, 8, 12}, 12},	// 15
	{{ 9, 8, 14}, 10},	// 16
	{{ 9, 8, 16}, 10},	// 17
	{{ 9, 8, 18}, 10},	// 18
	{{ 9, 8, 20},  8},	// 19
	{{ 9, 8, 22},  8}	// 20
};

// Note: This is Table 25 from the AD&D Handbook p.53
static const CLASS_HIT_DICE_TABLE RogueHitDiceTable[] = {
	{{ 0, 0,  0}, 20},	//  Level 0	(So that Level is a direct access to this table.)
	{{ 1, 6,  0}, 20},	//  1
	{{ 2, 6,  0}, 20},	//  2
	{{ 3, 6,  0}, 19},	//  3
	{{ 4, 6,  0}, 19},	//  4
	{{ 5, 6,  0}, 18},	//  5
	{{ 6, 6,  0}, 18},	//  6
	{{ 7, 6,  0}, 17},	//  7
	{{ 8, 6,  0}, 17},	//  8
	{{ 9, 6,  0}, 16},	//  9
	{{10, 6,  0}, 16},	// 10
	{{10, 6,  2}, 15},	// 11
	{{10, 6,  4}, 15},	// 12
	{{10, 6,  6}, 14},	// 13
	{{10, 6,  8}, 14},	// 14
	{{10, 6, 10}, 13},	// 15
	{{10, 6, 12}, 13},	// 16
	{{10, 6, 14}, 12},	// 17
	{{10, 6, 16}, 12},	// 18
	{{10, 6, 18}, 11},	// 19
	{{10, 6, 20}, 11}	// 20
};

PLAYER_CLASS_INFO::PLAYER_CLASS_ITEM const PLAYER_CLASS_INFO::pPlayerClassInfoTable[] = {
	{STR_CLASS_FIGHTER,	 	FighterHitDiceTable},
	{STR_CLASS_PALADIN, 	FighterHitDiceTable},
	{STR_CLASS_RANGER, 		FighterHitDiceTable},
	{STR_CLASS_CLERIC, 		ClericHitDiceTable},
	{STR_CLASS_WIZARD, 		WizardHitDiceTable},
	{STR_CLASS_MAGICIAN,	WizardHitDiceTable},	// Doesn't say but since Magicians are specialized Wizards...
	{STR_CLASS_THIEF,		RogueHitDiceTable},
	{STR_CLASS_BARD,		RogueHitDiceTable}
};	

class CLASS_ABILITY {
public:
	const ABILITY	&minAbility;
	SBYTE			RacesAllowed[RACE_INFO::RACE_MAX_NUMBER];	// Use Player Race Type to index
	
	CLASS_ABILITY (const ABILITY &,
	               SBYTE /* Dwarf */,
	               SBYTE /* Elf */,
	               SBYTE /* HalfElf */,
	               SBYTE /* Halfling */,
	               SBYTE /* Anuirean */,
	               SBYTE /* Brecht */,
	               SBYTE /* Khinasi */,
	               SBYTE /* Rjurik */,
	               SBYTE /* Vos */,
	               SBYTE /* Goblins */,
	               SBYTE /* Gnolls */);
	
	~CLASS_ABILITY() {}
};
typedef CLASS_ABILITY *PTR_CLASS_ABILITY;

// Constructor
CLASS_ABILITY::CLASS_ABILITY (const ABILITY &ability,
	               SBYTE Dwarf,
	               SBYTE Elf,
	               SBYTE HalfElf,
	               SBYTE Halfling,
	               SBYTE Anuirean,
	               SBYTE Brecht,
	               SBYTE Khinasi,
	               SBYTE Rjurik,
	               SBYTE Vos,
	               SBYTE Goblins,
	               SBYTE Gnolls)
	: minAbility(ability)
{
	RacesAllowed[RACE_INFO::DWARF]    		= Dwarf;
	RacesAllowed[RACE_INFO::ELF]    		= Elf;
	RacesAllowed[RACE_INFO::HALF_ELF] 		= HalfElf;
	RacesAllowed[RACE_INFO::HALFLING] 		= Halfling;
	RacesAllowed[RACE_INFO::HUMAN_ANUIREAN] = Anuirean;
	RacesAllowed[RACE_INFO::HUMAN_BRECHT] 	= Brecht;
	RacesAllowed[RACE_INFO::HUMAN_KHINASI] 	= Khinasi;
	RacesAllowed[RACE_INFO::HUMAN_RJURIK] 	= Rjurik;
	RacesAllowed[RACE_INFO::HUMAN_VOS] 		= Vos;
	RacesAllowed[RACE_INFO::GOBLINS] 		= Goblins;
	RacesAllowed[RACE_INFO::GNOLLS] 		= Gnolls;
}

// Note: For the momement I'm ignore'ing bloodline requirements.
//       This information came from the Birthright Rulebook pp. 10-14
static CLASS_ABILITY const ClassAbilityTable[] = {
//    					   S   D  Cn   I   W  Ch   D  E HE  H  A  B  K  R  V  G  G
	CLASS_ABILITY(ABILITY( 9,  0,  0,  0,  0,  0), 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1),	// Fighter
	CLASS_ABILITY(ABILITY(12,  0,  9,  0, 13, 17), 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0),	// Paladin
	CLASS_ABILITY(ABILITY(13, 13, 14,  0, 14,  0), 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0),	// Ranger
	CLASS_ABILITY(ABILITY( 0,  0,  0,  0,  9,  0), 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0),	// Priest
	CLASS_ABILITY(ABILITY( 0,  0,  0,  9,  0,  0), 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0),	// Wizard
	CLASS_ABILITY(ABILITY( 0, 12,  0, 12, 12,  0), 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0),	// Magician
	CLASS_ABILITY(ABILITY( 0,  9,  0,  0,  0,  0), 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1),	// Thief
	CLASS_ABILITY(ABILITY( 0, 12,  0, 12,  0, 15), 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0)		// Bard
};

/* =======================================================================
   Function    - mfReadTextData
   Description - Read the text data from the .ava file.
   Returns     - void
   ======================================================================== */
const LONG PLAYER_CLASS_LIST::mfReadTextData(FILE * const fp)
{
	LONG Result;
	char cpBuffer[80];
	LONG lTemp;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (3 == sscanf(cpBuffer, "%d %hd %d", &lTemp, &Class[0].sbLevel, &Class[0].ExperiencePts))
	{
		Class[0].Type = (PLAYER_CLASS_INFO::TYPE) lTemp;
		if (Class[0].Type != PLAYER_CLASS_INFO::NOT_SET && 
		    Class[0].ExperiencePts == 0)
		{
			Class[0].ExperiencePts = ALL_CLASS_EXPERIENCE_TABLE::mfGetMinimumExperience(Class[0].Type,
																						Class[0].sbLevel);
		}
	}
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (3 == sscanf(cpBuffer, "%d %hd %d", &lTemp, &Class[1].sbLevel, &Class[1].ExperiencePts))
	{
		Class[1].Type = (PLAYER_CLASS_INFO::TYPE) lTemp;
		if (Class[1].Type != PLAYER_CLASS_INFO::NOT_SET &&
		    Class[1].ExperiencePts == 0)
		{
			Class[1].ExperiencePts = ALL_CLASS_EXPERIENCE_TABLE::mfGetMinimumExperience(Class[1].Type,
																						Class[1].sbLevel);
		}
	}
	
	return Result;
}
