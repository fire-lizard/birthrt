/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: SPELLPRO.HXX - Fn Prototypes for spell progression information
   Author: 	 Gary Powell
   ======================================================================== */
#ifndef _SPELLPRO_HXX
#define _SPELLPRO_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_SPELL_HXX)
#include "spell.hxx"
#endif


class PLAYER_STATS;
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
// HUNGARIAN NOTE: All values are of type long unless specified.  So if
// is doesn't start with a capital letter, the lower case should be a
// prefix.  'u' means Ulong.

#if 0
typedef enum {
	SPELL_PROGRESSION_NOT_ALLOWED,	// You're not allowed this spell.
	SPELL_PROGRESSION_LEVEL_FULL,	// No more allowed at this level.
	SPELL_PROGRESSION_OK			// You can keep it.
} SPELL_PROGRESSION_CHECK;

SPELL_PROGRESSION_CHECK CheckPlayerSpellLimits(
		PLAYER_STATS *pPlayerStats,
		PTR_SPELL pSpell);
#endif

// returns number of spells of a certain type/level at a class/level
SHORT num_spells_of(SPELL_INFO::CASTERTYPE	spell_type,
                    SPELL_INFO::LEVEL		spell_level,
		            PLAYER_CLASS_INFO::TYPE	char_class,
		            SBYTE 					char_level);

#endif // _SPELLPRO_HXX
