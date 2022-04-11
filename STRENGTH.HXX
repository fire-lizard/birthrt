/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: STRENGTH.H - Structure to hold Strength data
   Author: 	 Gary Powell
   ======================================================================== */
#ifndef _STRENGTH_HXX
#define _STRENGTH_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYEPDEFS_H)
#include "typedefs.h"
#endif


/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
// HUNGARIAN NOTE: All values are of type long unless specified.  So if
// is doesn't start with a capital letter, the lower case should be a
// prefix.  'u' means Ulong.



// The Strength Table can be accessed directly by Pseudo Strength Rating.
class STRENGTH {
public:
	class DOORS {
	public:
		SBYTE sbChanceToForceDoor;
		SBYTE sbChanceToOpenLockedDoor;
	};
	typedef DOORS PTR_DOORS;

	class STRENGTH_ITEM {
	public:
		friend class STRENGTH;
	private:
		SBYTE	bHitProbablity;
		SBYTE	bDamageAdjust;
		SHORT	sWeightAllowance;
		SHORT	sMaxPressure;
		SBYTE	bBend_Lift;				// Stored as a Percentage
		DOORS	dDoor;
	};

	static const SBYTE	mfBasicStrength(const SBYTE /* RawStrenghtValue */);
	static const SBYTE	mfHitAdjust(const SBYTE /* RawStrenghtValue */);
	static const SBYTE	mfDamageAdjust(const SBYTE /* RawStrenghtValue */);
	static const SBYTE	mfChanceToForceDoor(const SBYTE /* RawStrengthValue */);
	static const SBYTE	mfChanceToOpenLockedDoor(const SBYTE /* RawStrengthValue */);
	
protected:
private:
	static const STRENGTH_ITEM	pStrengthTable[];
};

typedef STRENGTH *PTR_STRENGTH;

// Use this fn if you want the Basic Strength Rating from
// a Raw Strength value.
inline const SBYTE STRENGTH::mfBasicStrength(const SBYTE x)
{
	return ((const SBYTE)((x < 19 )? x : ((x > 23) ? x - 5 : 18 )));
}

extern SBYTE strength_percentile[];

// Get the strength adjustment for hit probablity.
inline const SBYTE STRENGTH::mfHitAdjust(const SBYTE RawStrengthValue)
{
	return (pStrengthTable[RawStrengthValue].bHitProbablity);
}

// Get the strength adjustment for damage.
inline const SBYTE STRENGTH::mfDamageAdjust(const SBYTE RawStrengthValue)
{
	return (pStrengthTable[RawStrengthValue].bDamageAdjust);
}

inline const SBYTE	STRENGTH::mfChanceToForceDoor(const SBYTE RawStrengthValue )
{
	return (pStrengthTable[RawStrengthValue].dDoor.sbChanceToForceDoor);
}

inline const SBYTE	STRENGTH::mfChanceToOpenLockedDoor(const SBYTE RawStrengthValue )
{
	return (pStrengthTable[RawStrengthValue].dDoor.sbChanceToOpenLockedDoor);
}
#endif // _STRENGTH_HXX
