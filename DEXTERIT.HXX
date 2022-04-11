/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: DEXTERIT.H structure to access values from the Dexterity Table
   Author: 	 Gary Powell
   ======================================================================== */
#ifndef _DEXTERIT_HXX
#define _DEXTERIT_HXX

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

class DEXTERITY {
public:
	class DEXTERITY_ITEM {
	public:
		friend class DEXTERITY;
	private:
		SBYTE	sbReactionAdjust;
		SBYTE	sbMissileAttackAdjust;
		SBYTE	sbDefensiveAdjust;
	};
	
	static const SBYTE mfGetReactionAdjust(const SBYTE /* sbDexterity */);
	static const SBYTE mfGetMissleAttackAdjust(const SBYTE /* sbDexterity */);
	static const SBYTE mfGetDefensiveAdjust(const SBYTE /* sbDexterity */);

private:
	static	const DEXTERITY_ITEM	pDexterityTable[];
};

typedef DEXTERITY *PTR_DEXTERITY;

// Get the adjustment for surprise adjustment
inline const SBYTE DEXTERITY::mfGetReactionAdjust(const SBYTE sbDexterity)
{
	return pDexterityTable[sbDexterity].sbReactionAdjust;
}

// Get the adjustment for using a missil weapon.
inline const SBYTE DEXTERITY::mfGetMissleAttackAdjust(const SBYTE sbDexterity)
{
	return pDexterityTable[sbDexterity].sbMissileAttackAdjust;
}

// Get the adjustment for general purpose defense against attacks.
inline const SBYTE DEXTERITY::mfGetDefensiveAdjust(const SBYTE sbDexterity)
{
	return pDexterityTable[sbDexterity].sbDefensiveAdjust;
}

#endif // _DEXTERIT_HXX
