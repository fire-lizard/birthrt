/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: WISDOM.HXX - Collection of Abilities for characters
   Author: 	 Gary Powell
   ======================================================================== */
#ifndef _WISDOM_HXX
#define _WISDOM_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_SPELL_HXX)
#include "spell.hxx"
#endif


/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
// HUNGARIAN NOTE: All values are of type long unless specified.  So if
// is doesn't start with a capital letter, the lower case should be a
// prefix.  'u' means Ulong.

class WISDOM {
public:
	class WISDOM_ITEM {
	public:
		friend class WISDOM;
		WISDOM_ITEM(SBYTE sbMagicalDefenseAdjust, SBYTE sbBonusSpells[SPELL_INFO::MAX_PR_LEVELS + 1], SBYTE sbChanceOfSpellFailure)
		{
			this->sbMagicalDefenseAdjust = sbMagicalDefenseAdjust;
			this->sbChanceOfSpellFailure = sbChanceOfSpellFailure;
			for (int index = 0; index < SPELL_INFO::MAX_PR_LEVELS + 1; index++)
			{
				this->sbBonusSpells[index] = sbBonusSpells[index];
			}
		}
	private:
		SBYTE	sbMagicalDefenseAdjust;
		SBYTE	sbBonusSpells[SPELL_INFO::MAX_PR_LEVELS+1];	// Only Priests can have these
		                                        	// Note you can directly index these by level
		SBYTE	sbChanceOfSpellFailure;				// Stored as a Percentage
		// SBYTE	sSpellImmunity;					// Stored with the spell.
	};

	static const SBYTE mfGetMagicalDefenseAdjust(const SBYTE /* WisdomValue */);
	static const SBYTE mfGetChanceOfSpellFailure(const SBYTE /* WisdomValue */);
	static const SBYTE mfGetPriestBonus(const SBYTE /* WisdomValue */,
	                                    SPELL_INFO::LEVEL /* spellLevel */);

private:
	static const WISDOM_ITEM pWisdomTable[];
};
typedef WISDOM *PTR_WISDOM;

inline const SBYTE WISDOM::mfGetMagicalDefenseAdjust(const SBYTE WisdomValue )
{
	return pWisdomTable[WisdomValue].sbMagicalDefenseAdjust;
}

inline const SBYTE WISDOM::mfGetChanceOfSpellFailure(const SBYTE WisdomValue)
{
	return pWisdomTable[WisdomValue].sbChanceOfSpellFailure;
}
	
inline const SBYTE WISDOM::mfGetPriestBonus(const SBYTE WisdomValue,
                                    SPELL_INFO::LEVEL spellLevel)
{
	if (spellLevel > SPELL_INFO::LEVEL_7)
	{
		spellLevel = SPELL_INFO::LEVEL_7;
	}
	return pWisdomTable[WisdomValue].sbBonusSpells[spellLevel];
}
#endif // _WISDOM_HXX
