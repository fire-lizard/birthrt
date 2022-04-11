
/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _PLAYCLAS_HXX
#define _PLAYCLAS_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_PLAYRACE_HXX)
#include "playrace.hxx"
#endif

#if !defined(_FILEUTIL_H)
#include "fileutil.h"
#endif

//#include "gamestr.h"
#if !defined(_ABILTIY_HXX)
#include "ability.hxx"
#endif

#if !defined(_DICE_H)
#include "dice.h"
#endif

#if !defined(_STRENUM_H)
#include "strenum.h"   
#endif


/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define UNRESTRICTED		25
#define MAX_PLAYER_CLASSTYPES	2
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
// Note: The PLAYER_CLASS_TYPES are used as indices into other tables.
//       Be careful if you modify either their order or number.


class CLASS_HIT_DICE_TABLE {
public:
	DICE	HitDice;
	SBYTE	Thac0;
};

typedef CLASS_HIT_DICE_TABLE *PTR_CLASS_HIT_DICE_TABLE;

class PLAYER_CLASS_INFO {
public:
	typedef enum {
		NOT_SET			= -1,
		FIGHTER			=  0,
		PALADIN,
		RANGER,
		PRIEST,
		WIZARD,
		MAGICIAN,
		THIEF,
		BARD,
	
		// Note: Don't add any more class types. This is it!
		CLASS_MAX_NUMBER
	} TYPE;
	
	class PLAYER_CLASS_ITEM {
	public:
		friend class PLAYER_CLASS_INFO;
	private:
		GAME_STRING  ClassName;
		CLASS_HIT_DICE_TABLE  const *HitDiceTable;
	};
	
	// As players gain experiece points their class limits need
	// to be checked to see that they stay within the race bounds.
	static void mfGetPlayerClassLimits( PLAYER_CLASS_INFO::TYPE /* PlayerClass */, 
										RACE_INFO::TYPE /* PlayerRace */, 
										SBYTE * /* psbHighValue */);
	
	static char const * const mfGetName(const TYPE);
	static DICE const * mfGetDice(const TYPE, const SBYTE /* sbLevel */ );
	static const SBYTE mfGetThac0(const TYPE, const SBYTE /* sbLevel */ );
	
private:
	static PLAYER_CLASS_ITEM const pPlayerClassInfoTable[];
};

typedef PLAYER_CLASS_INFO *PTR_PLAYER_CLASS_INFO;

// This structure is for use by the player status structure.
class PLAYER_CLASS_LIST {
public:
	class PLAYER_CLASS_DATA
	{
	public:
		PLAYER_CLASS_INFO::TYPE		Type;
		SBYTE						sbLevel;
		LONG						ExperiencePts;
	};

	PLAYER_CLASS_DATA Class[MAX_PLAYER_CLASSTYPES];		// Max number of classes per character.
	
	void mfInitData();
	void mfAddExperiencePts(const LONG /* AdditionalPts */);
	const LONG mfRollHitPts() const;
	const SBYTE mfGetBestThac0() const;
	const LONG mfReadTextData(FILE * const fp);
	void mfWriteTextData(FILE * const fp);
	
	const PLAYER_CLASS_INFO::TYPE mfGetType(const LONG /* ClassNo */ ) const;
	char const * const mfGetName(const LONG /* ClassNo */ ) const;
	const SBYTE mfGetLevel(const LONG /* ClassNo */ ) const;
	const LONG mfGetExpPts(const LONG /* ClassNo */ ) const;
	
	BOOL const mfIsType(PLAYER_CLASS_INFO::TYPE const /* ClassType */ ) const;
};

typedef PLAYER_CLASS_LIST *PTR_PLAYER_CLASS_LIST;
	
typedef enum {
	CLASS_VALID,
	CLASS_INVALID
} CLASS_TEST_TYPE;

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
CLASS_TEST_TYPE TestPlayerClassLimits( PLAYER_CLASS_INFO::TYPE,
									   RACE_INFO::TYPE,
									   PTR_ABILITY,
									   SBYTE /* sbLevel */);
			

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
// Global table for looking up Class information

// Get the name of this class type.
inline char const * const PLAYER_CLASS_INFO::mfGetName(const TYPE type)
{
	GAME_STRING const strIndex = ( (type == PLAYER_CLASS_INFO::NOT_SET) ? 
											STR_NULL :
											pPlayerClassInfoTable[type].ClassName);
	return ( STRMGR_GetStr(strIndex));
}

// Get the hit dice for a player of this type and level.
inline DICE const * PLAYER_CLASS_INFO::mfGetDice(const TYPE type, const SBYTE sbLevel )
{
	return ((type == PLAYER_CLASS_INFO::NOT_SET) ? 
			&(pPlayerClassInfoTable[FIGHTER].HitDiceTable[0].HitDice) :
			&(pPlayerClassInfoTable[type].HitDiceTable[sbLevel].HitDice));
}

// Get the Thac0 for a player of this type and level.
inline const SBYTE PLAYER_CLASS_INFO::mfGetThac0(const TYPE type, const SBYTE sbLevel )
{
	return ((type == PLAYER_CLASS_INFO::NOT_SET) ? 20 : 
			pPlayerClassInfoTable[type].HitDiceTable[sbLevel].Thac0);
}

// Initialize the data members.
inline void PLAYER_CLASS_LIST::mfInitData()
{
	LONG i;
	for (i = 0; i < MAX_PLAYER_CLASSTYPES; i++)
	{
		Class[i].Type = PLAYER_CLASS_INFO::NOT_SET;
		Class[i].sbLevel = 0;
		Class[i].ExperiencePts = 0;
	}
}
	
// Divide the experience points among all the classes that the character is.
inline void PLAYER_CLASS_LIST::mfAddExperiencePts(const LONG AdditionalPts )
{
	LONG i;
	LONG NumberOfValidTypes;
	
	for (NumberOfValidTypes = 0; 
	     NumberOfValidTypes < MAX_PLAYER_CLASSTYPES && Class[NumberOfValidTypes].Type != PLAYER_CLASS_INFO::NOT_SET;
	     NumberOfValidTypes++)
	{ };
	
	for (i = 0;
	     i < NumberOfValidTypes;
	     i++)
	{
		Class[i].ExperiencePts += AdditionalPts/NumberOfValidTypes;
	}
}

// Roll up the hit points for a player of this level.
inline const LONG PLAYER_CLASS_LIST::mfRollHitPts() const
{
	LONG Result = 0;
	
	// Average the roll for dual class players
	if (Class[0].Type != PLAYER_CLASS_INFO::NOT_SET)
	{
		Result = RollDice(PLAYER_CLASS_INFO::mfGetDice(Class[0].Type, Class[0].sbLevel));
		if (Class[0].Type != PLAYER_CLASS_INFO::NOT_SET)
		{
			Result += RollDice(PLAYER_CLASS_INFO::mfGetDice(Class[0].Type, Class[0].sbLevel));
			Result /= 2;	// Average in the 
		}
	}
	return Result;
}

// Write the text data to the .ava file.
inline void PLAYER_CLASS_LIST::mfWriteTextData(FILE * const fp)
{
	fprintf(fp, "%ld %hd %ld", (LONG)Class[0].Type, (SHORT)Class[0].sbLevel, Class[0].ExperiencePts);
	fprintf(fp, "%ld %hd %ld", (LONG)Class[1].Type, (SHORT)Class[1].sbLevel, Class[1].ExperiencePts);
}

// Get your best Thac0 for dual players. I know this isn't true AD&D but
// adding the difficulty of letting the gamer choose is out for now.
inline const SBYTE PLAYER_CLASS_LIST::mfGetBestThac0() const
{
	SBYTE Thac0 = 20;	// 20 is the worst you can be.
	if (Class[0].Type != PLAYER_CLASS_INFO::NOT_SET)
	{
		Thac0 = PLAYER_CLASS_INFO::mfGetThac0(Class[0].Type, Class[0].sbLevel);
		if (Class[1].Type != PLAYER_CLASS_INFO::NOT_SET)
		{
			const SBYTE Thac02 = PLAYER_CLASS_INFO::mfGetThac0(Class[1].Type, Class[1].sbLevel);
			if (Thac02 < Thac0)
			{
				Thac0 = Thac02;
			}
		}
	}
	
	return Thac0;
}

inline const PLAYER_CLASS_INFO::TYPE PLAYER_CLASS_LIST::mfGetType(const LONG ClassNo) const
{
	return Class[ClassNo].Type;
}

inline char const * const PLAYER_CLASS_LIST::mfGetName(const LONG ClassNo ) const
{
	return PLAYER_CLASS_INFO::mfGetName(Class[ClassNo].Type);
}

inline const SBYTE PLAYER_CLASS_LIST::mfGetLevel(const LONG ClassNo) const
{
	return (Class[ClassNo].Type != PLAYER_CLASS_INFO::NOT_SET) ? Class[ClassNo].sbLevel : (SBYTE)0;
}

inline const LONG PLAYER_CLASS_LIST::mfGetExpPts(const LONG ClassNo) const
{
	return (Class[ClassNo].Type != PLAYER_CLASS_INFO::NOT_SET) ? Class[ClassNo].ExperiencePts : 0;
}

inline BOOL const PLAYER_CLASS_LIST::mfIsType(PLAYER_CLASS_INFO::TYPE const ClassType ) const
{
	return (Class[0].Type == ClassType || Class[1].Type == ClassType);
}

#endif // _PLAYCLAS_HXX
