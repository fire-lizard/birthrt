/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: INTELLI.H - Structure to access Intelligence Table data
   Author: 	 Gary Powell
   ======================================================================== */
#ifndef _INTELLI_HXX
#define _INTELLI_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif


/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
// HUNGARIAN NOTE: All values are of type long unless specified.  So if
// is doesn't start with a capital letter, the lower case should be a
// prefix.  'u' means Ulong.

#define ALL_SPELLS			100		// Just guessing here.

class INTELLIGENCE {
public:
	class INTELLIGENCE_ITEM {
	public:
		friend class INTELLIGENCE;
	private:
		SBYTE	sbNumOfLanguages;
		SBYTE	sbSpellLevel;
		SBYTE	sbChanceToLearnSpell;		// Stored as a Percentage
		SBYTE	sbMaxNumberOfSpells;
		SBYTE	sbIllusionImmunity;
	};

	static const SBYTE mfGetSpellLevel( const SBYTE /* IntellValue */ );
	static const SBYTE mfGetChanceToLearn( const SBYTE /* IntellValue */ );
	static const SBYTE mfGetMaxNumberOfSpells( const SBYTE /* IntellValue */ );
	
private:
	static const INTELLIGENCE_ITEM pIntelligenceTable[];
};
typedef INTELLIGENCE *PTR_INTELLIGENCE;

inline const SBYTE INTELLIGENCE::mfGetSpellLevel( const SBYTE IntellValue)
{
	return pIntelligenceTable[IntellValue].sbSpellLevel;
}

inline const SBYTE INTELLIGENCE::mfGetChanceToLearn(const SBYTE IntellValue)
{
	return pIntelligenceTable[IntellValue].sbChanceToLearnSpell;
}

inline const SBYTE INTELLIGENCE::mfGetMaxNumberOfSpells(const SBYTE IntellValue)
{
	return pIntelligenceTable[IntellValue].sbMaxNumberOfSpells;
}
#endif // _INTELLI_HXX
