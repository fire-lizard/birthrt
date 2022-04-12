/* ========================================================================
   Copyright (c) 1990,1995	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: spellpro.cpp  - Spell Progression Tables
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "SYSTEM.H"
#include "SPELLPRO.HXX"
#include "PLAYSTAT.HXX"

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

#define MAX_WIZARD_SPELL_LEVEL	9
// Note: Add one so that Spell level is direct access to the array.
typedef char WIZARD_SPELL_PROGRESSION[MAX_WIZARD_SPELL_LEVEL + 1]; 

// Number of spells that can be known by spell level and wizard level.
// AD&D Handbook p.43 Table 21
WIZARD_SPELL_PROGRESSION WizardSpellProgressionTable[] = {
//    0  1  2  3  4  5  6  7  8  9 (Spell Level)
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	//  Wizard Level 0 - so that direct access by Wizard level is done.
	{ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},	//  1
	{ 0, 2, 0, 0, 0, 0, 0, 0, 0, 0},	//  2
	{ 0, 2, 1, 0, 0, 0, 0, 0, 0, 0},	//  3
	{ 0, 3, 2, 0, 0, 0, 0, 0, 0, 0},	//  4
	{ 0, 4, 2, 1, 0, 0, 0, 0, 0, 0},	//  5
	{ 0, 4, 2, 2, 0, 0, 0, 0, 0, 0},	//  6
	{ 0, 4, 3, 2, 1, 0, 0, 0, 0, 0},	//  7
	{ 0, 4, 3, 3, 2, 0, 0, 0, 0, 0},	//  8
	{ 0, 4, 3, 3, 2, 1, 0, 0, 0, 0},	//  9
	{ 0, 4, 4, 3, 2, 2, 0, 0, 0, 0},	// 10
	{ 0, 4, 4, 4, 3, 3, 0, 0, 0, 0},	// 11
	{ 0, 4, 4, 4, 4, 4, 1, 0, 0, 0},	// 12
	{ 0, 5, 5, 5, 4, 4, 2, 0, 0, 0},	// 13
	{ 0, 5, 5, 5, 4, 4, 2, 1, 0, 0},	// 14
	{ 0, 5, 5, 5, 5, 5, 2, 1, 0, 0},	// 15
	{ 0, 5, 5, 5, 5, 5, 3, 2, 1, 0},	// 16
	{ 0, 5, 5, 5, 5, 5, 3, 3, 2, 0},	// 17
	{ 0, 5, 5, 5, 5, 5, 3, 3, 2, 1},	// 18
	{ 0, 5, 5, 5, 5, 5, 3, 3, 3, 1},	// 19
	{ 0, 5, 5, 5, 5, 5, 4, 3, 3, 2}		// 20
};

#define MAX_PRIEST_SPELL_LEVEL	7
// Note: Add one so that Spell level is direct access to the array.
typedef char PRIEST_SPELL_PROGRESSION[MAX_PRIEST_SPELL_LEVEL + 1]; 

// Number of spells that can be known by spell level and priest level.
// AD&D Handbook p.47 Table 24
PRIEST_SPELL_PROGRESSION PriestSpellProgressionTable[] = {
//    0  1  2  3  4  5  6  7 (Spell Level)
	{ 0, 0, 0, 0, 0, 0, 0, 0},	//  Priest Level 0 - so that direct access by Wizard level is done.
	{ 0, 1, 0, 0, 0, 0, 0, 0},	//  1
	{ 0, 2, 0, 0, 0, 0, 0, 0},	//  2
	{ 0, 2, 1, 0, 0, 0, 0, 0},	//  3
	{ 0, 3, 2, 0, 0, 0, 0, 0},	//  4
	{ 0, 3, 3, 1, 0, 0, 0, 0},	//  5
	{ 0, 3, 3, 2, 0, 0, 0, 0},	//  6
	{ 0, 3, 3, 2, 1, 0, 0, 0},	//  7
	{ 0, 3, 3, 3, 2, 0, 0, 0},	//  8
	{ 0, 4, 4, 3, 2, 1, 0, 0},	//  9
	{ 0, 4, 4, 3, 3, 2, 0, 0},	// 10
	{ 0, 5, 4, 4, 3, 2, 1, 0},	// 11
	{ 0, 6, 5, 5, 3, 2, 2, 0},	// 12
	{ 0, 6, 6, 6, 4, 2, 2, 0},	// 13
	{ 0, 6, 6, 6, 5, 3, 2, 1},	// 14
	{ 0, 6, 6, 6, 6, 4, 2, 1},	// 15
	{ 0, 7, 7, 7, 6, 4, 3, 1},	// 16
	{ 0, 7, 7, 7, 7, 5, 3, 2},	// 17
	{ 0, 8, 8, 8, 8, 6, 4, 2},	// 18
	{ 0, 9, 9, 8, 8, 6, 4, 2},	// 19
	{ 0, 9, 9, 9, 8, 7, 5, 2}	// 20
};

#define MAX_PALADIN_SPELL_LEVEL		4
#define MAX_PALADIN_CASTING_LEVEL	20
// Note: Spell level - 1 provides direct access to the array.
//       This was done so that the spell data could be in one 4 byte datum
typedef struct _PALADIN_SPELL_PROGRESSION {
	char CastingLevel;
	char SpellLevel[MAX_PALADIN_SPELL_LEVEL];
} PALADIN_SPELL_PROGRESSION; 

// Number of priest spells that can be known by spell level and paladin level.
// AD&D Handbook p.40 Table 17
PALADIN_SPELL_PROGRESSION PaladinSpellProgressionTable[] = {
//        1  2  3  4 (Spell Level)
	{1, { 1, 0, 0, 0}},	//  Paladin Level 9 - WARNING direct access not possible by level.
	{2, { 2, 0, 0, 0}},	//  10
	{3, { 2, 1, 0, 0}},	//  11
	{4, { 2, 2, 0, 0}},	//  12
	{5, { 2, 2, 1, 0}},	//  13
	{6, { 3, 2, 1, 0}},	//  14
	{7, { 3, 2, 1, 1}},	//  15
	{8, { 3, 3, 2, 1}},	//  16
	{9, { 3, 3, 3, 1}},	//  17
	{9, { 3, 3, 3, 1}},	//  18
	{9, { 3, 3, 3, 2}},	//  19
	{9, { 3, 3, 3, 3}}	//  20
};

#define MAX_RANGER_SPELL_LEVEL	3
#define MAX_RANGER_CASTING_LEVEL 16
// Note: Add one so that Spell level is direct access to the array.
typedef struct _RANGER_SPELL_PROGRESSION {
	char CastingLevel;
	char SpellLevel[MAX_RANGER_SPELL_LEVEL + 1];
} RANGER_SPELL_PROGRESSION;
 
// Number of priest spells that can be known by spell level and ranger level.
// AD&D Handbook p.41 Table 18
// Also these can only be plant or animal spheres No bonus for wisdom
RANGER_SPELL_PROGRESSION RangerSpellProgressionTable[] = {
//        0  1  2  3 (Spell Level)
	{1, { 0, 1, 0, 0}},	//  Ranger Level 8 - WARNING direct access not possible by level.
	{2, { 0, 2, 0, 0}},	//  9
	{3, { 0, 2, 1, 0}},	// 10
	{4, { 0, 2, 2, 0}},	// 11
	{5, { 0, 2, 2, 1}},	// 12
	{6, { 0, 3, 2, 1}},	// 13
	{7, { 0, 3, 2, 2}},	// 14
	{8, { 0, 3, 3, 2}},	// 15
	{9, { 0, 3, 3, 3}}	// 16
};

#define MAX_BARD_SPELL_LEVEL	6
// Note: Add one so that Spell level is direct access to the array.
typedef char BARD_SPELL_PROGRESSION[MAX_BARD_SPELL_LEVEL + 1]; 

// Number of Wizard spells that can be known by spell level and bard level.
// AD&D Handbook p.59 Table 32
BARD_SPELL_PROGRESSION BardSpellProgressionTable[] = {
//    0  1  2  3  4  5  6 (Spell Level)
	{ 0, 0, 0, 0, 0, 0, 0},	//  Bard Level 0 - so that direct access by Wizard level is done.
	{ 0, 0, 0, 0, 0, 0, 0},	//  1
	{ 0, 1, 0, 0, 0, 0, 0},	//  2
	{ 0, 2, 0, 0, 0, 0, 0},	//  3
	{ 0, 2, 1, 0, 0, 0, 0},	//  4
	{ 0, 3, 1, 0, 0, 0, 0},	//  5
	{ 0, 3, 2, 0, 0, 0, 0},	//  6
	{ 0, 3, 2, 1, 0, 0, 0},	//  7
	{ 0, 3, 3, 1, 0, 0, 0},	//  8
	{ 0, 3, 3, 2, 0, 0, 0},	//  9
	{ 0, 3, 3, 2, 1, 0, 0},	// 10
	{ 0, 3, 3, 3, 1, 0, 0},	// 11
	{ 0, 3, 3, 3, 2, 0, 0},	// 12
	{ 0, 3, 3, 3, 2, 1, 0},	// 13
	{ 0, 3, 3, 3, 3, 1, 0},	// 14
	{ 0, 3, 3, 3, 3, 2, 0},	// 15
	{ 0, 4, 3, 3, 3, 2, 1},	// 16
	{ 0, 4, 4, 3, 3, 3, 1},	// 17
	{ 0, 4, 4, 4, 3, 3, 2},	// 18
	{ 0, 4, 4, 4, 4, 3, 2},	// 19
	{ 0, 4, 4, 4, 4, 4, 3}	// 20
};

#if 0
/* =======================================================================
   Function    - CountPlayerSpells
   Description - Count the number of spells a player has in the spell book at
                 a given level.
   Returns     - the number of spells
   ======================================================================== */
static char CountPlayerSpells(
	PTR_SPELL_LIST pSpellList,
	char bSpellLevel)
{
	char bNumberOfSpells = 0;

	while (pSpellList)
	{
		if (pSpellList->pSpell->bLevel == bSpellLevel)
		{
			bNumberOfSpells++;
		}
		pSpellList = pSpellList->next;
	}

	return bNumberOfSpells;
}

/* =======================================================================
   Function    - CheckPlayerSpellLimits
   Description - Look up the High value for a given player Race. If no limitations
                 don't change the input value.
   Returns     - SPELL_PROGRESSION_CHECK
   ======================================================================== */
SPELL_PROGRESSION_CHECK CheckPlayerSpellLimits(
		PLAYER_STATS *pPlayerStats,
		PTR_SPELL pSpell)
{
	SPELL_PROGRESSION_CHECK spcResult = SPELL_PROGRESSION_NOT_ALLOWED;
	SHORT sMaxNumberAtThisLevel = 0;
	PTR_PLAYER_CLASS_LIST	pPlayerClassList = &pPlayerStats->PlayerClassList;

	do
	{
		SHORT sThisClassMax = 0;

		if (pSpell->isa == SPELL::TYPE_PRIEST)
		{
			switch (pPlayerClassList->ClassType)
			{
			case PLAYER_CLASS_INFO::PALADIN:
				if (pPlayerClassList->sLevel > 8 && 
					pSpell->bLevel <= MAX_PALADIN_SPELL_LEVEL)
				{
					sThisClassMax = PaladinSpellProgressionTable[pPlayerClassList->sLevel - 9].SpellLevel[pSpell->bLevel - 1];
				}
				break;
			case PLAYER_CLASS_INFO::RANGER:
				if (pSpell->mfCheckSphereType(SPELL::SPHERE_PLANT) ||
					pSpell->mfCheckSphereType(SPELL::SPHERE_ANIMAL))
				{
					if (pPlayerClassList->sLevel > 7 && 
						pSpell->bLevel <= MAX_RANGER_SPELL_LEVEL)
					{
						sThisClassMax = RangerSpellProgressionTable[pPlayerClassList->sLevel - 8].SpellLevel[pSpell->bLevel];
					}
				}
				break;
			case PLAYER_CLASS_INFO::PRIEST:
				if (pSpell->bLevel <= MAX_PRIEST_SPELL_LEVEL)
				{
					sThisClassMax = PriestSpellProgressionTable[pPlayerClassList->sLevel][pSpell->bLevel];
				}
				break;
			case PLAYER_CLASS_INFO::BARD:
				if (pSpell->bLevel <= MAX_BARD_SPELL_LEVEL)
				{
					sThisClassMax = BardSpellProgressionTable[pPlayerClassList->sLevel][pSpell->bLevel];
				}
				break;
			}
		}
		else
		{
			switch (pPlayerClassList->ClassType)
			{
			case PLAYER_CLASS_INFO::WIZARD:
				if (pSpell->bLevel <= MAX_WIZARD_SPELL_LEVEL)
				{
					sThisClassMax = WizardSpellProgressionTable[pPlayerClassList->sLevel][pSpell->bLevel];
				}
				break;
			case PLAYER_CLASS_INFO::MAGICIAN:
				if (pSpell->bLevel <= MAX_WIZARD_SPELL_LEVEL)
				{
					sThisClassMax = WizardSpellProgressionTable[pPlayerClassList->sLevel][pSpell->bLevel];
				}

				// Other schools of magic limited to spells of levels 1 & 2
				// Birthright rulebook p.13
				if((!pSpell->School == SPELL::SCHOOL_ILLUSION ||
				   pSpell->School == SPELL::SCHOOL_LESSER_DIVINATION ||
				   pSpell->School == SPELL::SCHOOL_GREATER_DIVINATION))
				{
					if (pSpell->bLevel > 2)
					{
						sThisClassMax = 0;
					}
				}
				break;
			}
		}

		// You are allowed the best number of spells if you are a multiclassed character
		if (sThisClassMax > sMaxNumberAtThisLevel)
		{
			sMaxNumberAtThisLevel = sThisClassMax;
		}

		pPlayerClassList = pPlayerClassList->next;
	}
	while (pPlayerClassList);

	if (sMaxNumberAtThisLevel > 0) // No point in counting if none are allowed.
	{
		if (sMaxNumberAtThisLevel >= CountPlayerSpells(pPlayerStats->pSpellBook, pSpell->bLevel))
		{
			spcResult = SPELL_PROGRESSION_OK;
		}
		else
		{
			spcResult = SPELL_PROGRESSION_LEVEL_FULL;
		}
	}

	return spcResult;
}
#endif

SHORT num_spells_of(SPELL_INFO::CASTERTYPE spell_type,
                    SPELL_INFO::LEVEL spell_level,
		            PLAYER_CLASS_INFO::TYPE	char_class,
		            SBYTE char_level)
{
    if (char_level > 20)
        char_level = 20;  /* maximum casting level in basic AD&D */
        
    if (spell_type == SPELL_INFO::PRIEST)
    {
		if (char_class == PLAYER_CLASS_INFO::PRIEST)
		    return PriestSpellProgressionTable[char_level][spell_level];
			
        if (char_class == PLAYER_CLASS_INFO::PALADIN  &&
            char_level >= 9 &&
            spell_level <= MAX_PALADIN_SPELL_LEVEL)
			return PaladinSpellProgressionTable[char_level - 9].SpellLevel[spell_level - 1];
			
        if (char_class == PLAYER_CLASS_INFO::RANGER  &&
            char_level >= 8 &&
            spell_level <= MAX_RANGER_SPELL_LEVEL)
            {
                if (char_level > MAX_RANGER_CASTING_LEVEL)
                    char_level = MAX_RANGER_CASTING_LEVEL;
			    return RangerSpellProgressionTable[char_level - 8].SpellLevel[spell_level];
			}
    }
    else if (spell_type == SPELL_INFO::WIZARD)
    {
        if (char_class == PLAYER_CLASS_INFO::BARD &&
            spell_level <= MAX_BARD_SPELL_LEVEL)
            return BardSpellProgressionTable[char_level][spell_level];
		
		if (char_class == PLAYER_CLASS_INFO::WIZARD ||
		    char_class == PLAYER_CLASS_INFO::MAGICIAN)
		{
//            printf("A Wizard of level %d gets %d spells of level %d\n",
//                char_level,
//                WizardSpellProgressionTable[char_level][spell_level],
//                spell_level);
            return WizardSpellProgressionTable[char_level][spell_level];
        }
	}		
	
	return 0;
}
