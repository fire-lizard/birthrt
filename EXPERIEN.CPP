/* ========================================================================
   Copyright (c) 1990,1995	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: Experien.c  - Experience data for each class type.
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "EXPERIEN.HXX"

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
// Note: This is Table 14 from the AD&D Handbook p.36
static const CLASS_EXPERIENCE_TABLE FighterExperienceTable[MAX_EXPERIENCE_LEVELS + 1] = {
	      0,	//  Level 0	(So that Level is a direct access to this table.)
	      0,	//  1
	   2000,	//  2
	   4000,	//  3
	   8000,	//  4
	  16000,	//  5
	  32000,	//  6
	  64000,	//  7
	 125000,	//  8
	 250000,	//  9
	 500000,	// 10
	 750000,	// 11
	1000000,	// 12
	1250000,	// 13
	1500000,	// 14
	1750000,	// 15
	2000000,	// 16
	2250000,	// 17
	2500000,	// 18
	2750000,	// 19
	3000000		// 20
};	

// Note: This is Table 14 from the AD&D Handbook p.36
static const CLASS_EXPERIENCE_TABLE RangerExperienceTable[MAX_EXPERIENCE_LEVELS + 1] = {
	      0,	//  Level 0	(So that Level is a direct access to this table.)
	      0,	//  1
	   2250,	//  2
	   4500,	//  3
	   9000,	//  4
	  18000,	//  5
	  36000,	//  6
	  75000,	//  7
	 150000,	//  8
	 300000,	//  9
	 600000,	// 10
	 900000,	// 11
	1200000,	// 12
	1500000,	// 13
	1800000,	// 14
	2100000,	// 15
	2400000,	// 16
	2700000,	// 17
	3000000,	// 18
	3300000,	// 19
	3600000		// 20
};
	
// Note: This is Table 20 from the AD&D Handbook p.40
static const CLASS_EXPERIENCE_TABLE WizardExperienceTable[MAX_EXPERIENCE_LEVELS + 1] = {
	      0,	//  Level 0	(So that Level is a direct access to this table.)
	      0,	//  1
	   2500,	//  2
	   5000,	//  3
	  10000,	//  4
	  20000,	//  5
	  40000,	//  6
	  60000,	//  7
	  90000,	//  8
	 135000,	//  9
	 250000,	// 10
	 375000,	// 11
	 750000,	// 12
	1125000,	// 13
	1500000,	// 14
	1875000,	// 15
	2250000,	// 16
	2625000,	// 17
	3000000,	// 18
	3375000,	// 19
	3750000		// 20
};

// Note: This is Table 23 from the AD&D Handbook p.47
// Note: I'm not sure if we will have Druids or not. Until then I'm only putting
//       in the Cleric data table.
static const CLASS_EXPERIENCE_TABLE ClericExperienceTable[MAX_EXPERIENCE_LEVELS + 1] = {
	      0,	//  Level 0	(So that Level is a direct access to this table.)
	      0,	//  1
	   1500,	//  2
	   3000,	//  3
	   6000,	//  4
	  13000,	//  5
	  27500,	//  6
	  55000,	//  7
	 110000,	//  8
	 225000,	//  9
	 450000,	// 10
	 675000,	// 11
	 900000,	// 12
	1125000,	// 13
	1350000,	// 14
	1575000,	// 15
	1800000,	// 16
	2025000,	// 17
	2250000,	// 18
	2475000,	// 19
	2700000		// 20
};

// Note: This is Table 25 from the AD&D Handbook p.53
static const CLASS_EXPERIENCE_TABLE RogueExperienceTable[MAX_EXPERIENCE_LEVELS + 1] = {
	      0,	//  Level 0	(So that Level is a direct access to this table.)
	      0,	//  1
	   1250,	//  2
	   2500,	//  3
	   5000,	//  4
	  10000,	//  5
	  20000,	//  6
	  40000,	//  7
	  70000,	//  8
	 110000,	//  9
	 160000,	// 10
	 220000,	// 11
	 440000,	// 12
	 660000,	// 13
	 880000,	// 14
	1100000,	// 15
	1320000,	// 16
	1540000,	// 17
	1760000,	// 18
	1980000,	// 19
	2200000		// 20
};

CLASS_EXPERIENCE_TABLE const * const ALL_CLASS_EXPERIENCE_TABLE::pAllClassExperienceTable[] = {
	FighterExperienceTable,	// Fighter
	FighterExperienceTable,	// Paladin
	RangerExperienceTable,	// Ranger
	ClericExperienceTable,	// Priest (Cleric)
	WizardExperienceTable,	// Wizard
	WizardExperienceTable,	// Magician
	RogueExperienceTable,	// Thief
	RogueExperienceTable	// Bard
};


