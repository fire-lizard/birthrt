/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: CONSTITN.H structure to access values from the Dexterity Table
   Author: 	 Gary Powell
   ======================================================================== */
#ifndef _CONSTITN_HXX
#define _CONSTITN_HXX

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

#define NO_REGENERATION						-1

class CONSTITUTION {
public:
	class CONSTITUTION_ITEM {
	public:
		friend class CONSTITUTION;
	private:
		SBYTE	sbHitPointAdjust;
		SBYTE	sbWarriorHitAdjBonus;	// Use Warrior Hit Points if a Warrior.
		SBYTE	sbSystemShock;			// Stored as a Percentage
		SBYTE	sbResurrectionSurvival;	// Stored as a Percentage
		SBYTE	sbPoisonSave;
		SBYTE	sbRegeneration;			// Number Of Turns to heal
		SBYTE   sbSavingThrowBonus;
	};
	
	static const SBYTE mfGetHitPtAdjustment( const SBYTE /* sbConsititution */ );
	static const SBYTE mfGetWarriorHitPtBonus( const SBYTE /* sbConsititution */ );
	static const SBYTE mfGetPoisonSave( const SBYTE /* sbConsititution */ );

private:
	static const CONSTITUTION_ITEM	pConstitutionTable[];
};

typedef CONSTITUTION *PTR_CONSTITUTION;

inline const SBYTE CONSTITUTION::mfGetHitPtAdjustment( const SBYTE sbConsititution )
{
	return pConstitutionTable[sbConsititution].sbHitPointAdjust;
}

inline const SBYTE CONSTITUTION::mfGetWarriorHitPtBonus( const SBYTE sbConsititution )
{
	return pConstitutionTable[sbConsititution].sbWarriorHitAdjBonus;
}

inline const SBYTE CONSTITUTION::mfGetPoisonSave( const SBYTE sbConsititution )
{
	return pConstitutionTable[sbConsititution].sbPoisonSave;
}

#endif // _CONSTITN_HXX
