/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: CHARISMA.HXX - Structure to access Charisma Table Information
   Author: 	 Gary Powell
   ======================================================================== */
#ifndef _CHARISMA_HXX
#define _CHARISMA_HXX

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

class  CHARISMA {
public:
	class CHARISMA_ITEM {
	public:
		friend class CHARISMA;
	private:
		SBYTE	sbMaxNumOfHenchmen;
		SBYTE	sbLoyalityBase;		
		SBYTE	sbReactionAdjustment;
	};
	
	static const SBYTE mfGetMaxHenchmen(const SBYTE /* sbCharisma */ );
	static const SBYTE mfGetReactionAdjustment(const SBYTE /* sbCharisma */ );
	
private:
	static	const CHARISMA_ITEM	pCharismaTable[];
};
typedef CHARISMA *PTR_CHARISMA;

inline const SBYTE CHARISMA::mfGetMaxHenchmen(const SBYTE sbCharisma )
{
	return pCharismaTable[sbCharisma].sbMaxNumOfHenchmen;
}

inline const SBYTE CHARISMA::mfGetReactionAdjustment(const SBYTE sbCharisma )
{
	return pCharismaTable[sbCharisma].sbReactionAdjustment;
}

#endif // _CHARISMA_HXX
