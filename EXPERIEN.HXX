/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: EXPERIEN.H - Collection of Experience Minumums for characters
   Author: 	 Gary Powell
   ======================================================================== */
#ifndef _EXPERIEN_HXX
#define _EXPERIEN_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_PLAYSTAT_HXX)
#include "playclas.hxx"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
// HUNGARIAN NOTE: All values are of type long unless specified.  So if
// is doesn't start with a capital letter, the lower case should be a
// prefix.  'u' means Ulong.

#define MAX_EXPERIENCE_LEVELS	20

// Note: Access to this part of the table is by Class Level.
class CLASS_EXPERIENCE_TABLE 
{
public:
	LONG const mfGetExperiencePts(SBYTE const /* sbLevel */ ) const;
	
	LONG	ExperiencePts[MAX_EXPERIENCE_LEVELS + 1];
protected:
private:
};

typedef CLASS_EXPERIENCE_TABLE *PTR_CLASS_EXPERIENCE_TABLE;

inline LONG const CLASS_EXPERIENCE_TABLE::mfGetExperiencePts(
	SBYTE const sbLevel ) const
{
	return ExperiencePts[sbLevel];
}


// Note: Access to this table is by PLAYER_CLASS_TYPE
class ALL_CLASS_EXPERIENCE_TABLE 
{
public:
	static LONG const mfGetMinimumExperience(PLAYER_CLASS_INFO::TYPE const /* type */,
	                                         SBYTE const /* sbLevel */);
	static BOOL const mfCanIAdvanceALevel(const PLAYER_CLASS_INFO::TYPE,
	                                      const SBYTE /* sbCurrentLevel */,
	                                      const LONG /* CurrentExpPts */);
private:
	static CLASS_EXPERIENCE_TABLE const * const pAllClassExperienceTable[PLAYER_CLASS_INFO::CLASS_MAX_NUMBER];
};

typedef ALL_CLASS_EXPERIENCE_TABLE *PTR_ALL_CLASS_EXPERIENCE_TABLE;

// For a given player class type, and level get the minimum experience pts.
inline LONG const ALL_CLASS_EXPERIENCE_TABLE::mfGetMinimumExperience(
	PLAYER_CLASS_INFO::TYPE const type,
	SBYTE const sbLevel)
{
	return 1 + pAllClassExperienceTable[type]->mfGetExperiencePts(sbLevel);
}

// Given a type, the current level and current experience points test
// whether you have enough pts to go to the next level.
inline BOOL const ALL_CLASS_EXPERIENCE_TABLE::mfCanIAdvanceALevel(
	PLAYER_CLASS_INFO::TYPE const type,
    SBYTE const sbCurrentLevel,
    LONG const CurrentExpPts)
{
	return ( sbCurrentLevel < MAX_EXPERIENCE_LEVELS && 
	         pAllClassExperienceTable[type]->mfGetExperiencePts(sbCurrentLevel + 1) < CurrentExpPts) ? TRUE : FALSE;
}

#endif // _EXPERIEN_HXX
