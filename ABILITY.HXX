/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: ABILITY.H - Collection of Abilities for characters
   Author: 	 Gary Powell
   ======================================================================== */
#ifndef _ABILITY_HXX
#define _ABILITY_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_FILEUTIL_H)
#include "fileutil.h"
#endif

#if !defined(_PLAYRACE_HXX)
#include "playrace.hxx"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define	MIN_ROLLED_INTELLIGENCE_RATING		4
#define MAX_ROLLED_INTELLIGENCE_RATING		19

#define	MIN_ROLLED_WISDOM_RATING		2
#define MAX_ROLLED_WISDOM_RATING		18

#define	MIN_ROLLED_CONSTITUTION_RATING		2
#define MAX_ROLLED_CONSTITUTION_RATING		19

#define MIN_ROLLED_STRENGTH_RATING	3
#define	MAX_ROLLED_STRENGTH_RATING	18

#define	MIN_ROLLED_CHARISMA_RATING		3
#define MAX_ROLLED_CHARISMA_RATING		18

#define	MIN_ROLLED_DEXTERITY_RATING		3
#define MAX_ROLLED_DEXTERITY_RATING		19

/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
// Note: Order is important because this array is autofilled.
//       If you change this structure be sure to change RAbility.c
// This may change to just a straight array. (I'll edit the files!)
class ABILITY {
public:
	typedef enum {
		STRENGTH = 0,
		DEXTERITY,
		CONSTITUTION,
		INTELLIGENCE,
		WISDOM,
		CHARISMA,
	
		// These are the abilities,...Don't add more!
		ABILITY_MAX_NUMBER
	} TYPE;

	ABILITY(SBYTE /* STRENGTH */,
			SBYTE /* DEXTERITY */,
			SBYTE /* CONSTITUTION */,
			SBYTE /* INTELLIGENCE */,
			SBYTE /* WISDOM */,
			SBYTE /* CHARISMA */);
	~ABILITY() {}
	
	void mfAdjustAllAbilities(const RACE_INFO::TYPE);
	void mfChangeRaceAbilities( const RACE_INFO::TYPE /* OldRace */, 
								const RACE_INFO::TYPE /* NewRace */);
	const SBYTE mfGetAbility(const TYPE) const;			
	
	SBYTE const mfGetStrength() const;			
	SBYTE const mfGetDexterity() const;			
	SBYTE const mfGetConstitution() const;			
	SBYTE const mfGetIntelligence() const;			
	SBYTE const mfGetWisdom() const;			
	SBYTE const mfGetCharisma() const;			
	
	void mfSetStrength(SBYTE const /* newValue */);			
	void mfSetDexterity(SBYTE const /* newValue */);			
	void mfSetConstitution(SBYTE const /* newValue */);			
	void mfSetIntelligence(SBYTE const /* newValue */);			
	void mfSetWisdom(SBYTE const /* newValue */);			
	void mfSetCharisma(SBYTE const /* newValue */);			
	
	void RollAllAbilities(const RACE_INFO::TYPE);	// PlayerRace can be RACE_NOT_SET
	
	LONG mfReadData(FILE *);
	void mfWriteData(FILE *) const;
	void mfInitData();
				
protected:
private:
	void mfAdjustAbilities (ABILITY const *, const TYPE);
	SBYTE fAbilities[ABILITY_MAX_NUMBER]; // Use the ABILTITY_TYPE to index in this array.
	
};
typedef ABILITY *PTR_ABILITY;

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
const SBYTE RollAbility(
	const RACE_INFO::TYPE PlayerRace,	// PlayerRace can be RACE_NOT_SET
	const ABILITY::TYPE	AbilityType,
	SBYTE sDefaultHighValue,
	SBYTE sDefaultLowValue);

const SBYTE RollConstitution( const RACE_INFO::TYPE);	// PlayerRace can be RACE_NOT_SET

const SBYTE RollWisdom( const RACE_INFO::TYPE);	 	// PlayerRace can be RACE_NOT_SET

const SBYTE RollIntelligence( const RACE_INFO::TYPE);	// PlayerRace can be RACE_NOT_SET

const SBYTE RollCharisma( const RACE_INFO::TYPE); 		// PlayerRace can be RACE_NOT_SET

const SBYTE RollDexterity( const RACE_INFO::TYPE); 	// PlayerRace can be RACE_NOT_SET

const SBYTE RollStrength(const RACE_INFO::TYPE);
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
inline ABILITY::ABILITY(
		SBYTE bStrength,
		SBYTE bDexterity,
		SBYTE bConstitution,
		SBYTE bIntelligence,
		SBYTE bWisdom,
		SBYTE bCharisma)
{
		fAbilities[STRENGTH]  		= bStrength;
		fAbilities[DEXTERITY]  		= bDexterity;
		fAbilities[CONSTITUTION]	= bConstitution;
		fAbilities[INTELLIGENCE]	= bIntelligence;
		fAbilities[WISDOM] 	  		= bWisdom;
		fAbilities[CHARISMA]		= bCharisma;
}
   
// mfGetAbility
inline const SBYTE ABILITY::mfGetAbility(const ABILITY::TYPE atype) const
{
	return fAbilities[atype];
}
	
// Read the text data from the .ava files.
inline LONG ABILITY::mfReadData(FILE *fp)
{
	LONG Result = EOF;
	LONG i;
	char cpBuffer[126];
	LONG lTemp;
	
	for(i=0;i<ABILITY::ABILITY_MAX_NUMBER;i++)		/* ABILITY	*/
	{
		Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
		sscanf(cpBuffer, "%ld", &lTemp);
		fAbilities[i] = (SBYTE) lTemp;
	}
	
	return Result;
}


// Write the text data to the .ava files.
inline void ABILITY::mfWriteData(FILE *fp) const
{
	LONG i;
	
	for (i=0;i< ABILITY_MAX_NUMBER; i++)
	{
		fprintf(fp,"%hd\n", (SHORT) fAbilities[i]);
	}
}
inline void ABILITY::mfInitData()
{
	LONG i;
	
	for (i=0;i< ABILITY_MAX_NUMBER; i++)
	{
		fAbilities[i] = 0;
	}
}

/* =======================================================================
   Function    - RollIntelligence
   Description - Generate a valid Intelligence value for a given player Race.
   Returns     - Intelligence Value
   ======================================================================== */
inline const SBYTE RollIntelligence(
	const RACE_INFO::TYPE PlayerRace)	// PlayerRace can be RACE_NOT_SET
{
	return RollAbility(PlayerRace, 
		               ABILITY::INTELLIGENCE,
					   MAX_ROLLED_INTELLIGENCE_RATING,
					   MIN_ROLLED_INTELLIGENCE_RATING);
}

/* =======================================================================
   Function    - RollWisdom
   Description - Generate a valid Wisdom value for a given player Race.
   Returns     - Wisdom Value
   ======================================================================== */
inline const SBYTE RollWisdom(
	const RACE_INFO::TYPE PlayerRace)	// PlayerRace can be RACE_NOT_SET
{
	return RollAbility(PlayerRace, 
		               ABILITY::WISDOM,
					   MAX_ROLLED_WISDOM_RATING,
					   MIN_ROLLED_WISDOM_RATING);
}

/* =======================================================================
   Function    - RollConstitution
   Description - Generate a valid Constitution value for a given player Race.
   Returns     - Constitution Value
   ======================================================================== */
inline const SBYTE RollConstitution(
	const RACE_INFO::TYPE PlayerRace)	// PlayerRace can be RACE_NOT_SET
{
	return RollAbility(PlayerRace, 
		               ABILITY::CONSTITUTION,
					   MAX_ROLLED_CONSTITUTION_RATING,
					   MIN_ROLLED_CONSTITUTION_RATING);
}

/* =======================================================================
   Function    - RollDexterity
   Description - Generate a valid Dexterity value for a given player Race.
   Returns     - Dexterity Value
   ======================================================================== */
inline const SBYTE RollDexterity(
	const RACE_INFO::TYPE PlayerRace)	// PlayerRace can be RACE_NOT_SET
{
	return RollAbility(PlayerRace, 
		               ABILITY::DEXTERITY,
					   MAX_ROLLED_DEXTERITY_RATING,
					   MIN_ROLLED_DEXTERITY_RATING);
}

/* =======================================================================
   Function    - RollCharisma
   Description - Generate a valid Charisma value for a given player Race.
   Returns     - Charisma Value
   ======================================================================== */
inline const SBYTE RollCharisma(
	const RACE_INFO::TYPE PlayerRace)	// PlayerRace can be RACE_NOT_SET
{
	return RollAbility(PlayerRace, 
		               ABILITY::CHARISMA,
					   MAX_ROLLED_CHARISMA_RATING,
					   MIN_ROLLED_CHARISMA_RATING);
}

inline SBYTE const ABILITY::mfGetStrength() const
{
	return fAbilities[ABILITY::STRENGTH];
}

inline SBYTE const ABILITY::mfGetDexterity() const
{
	return fAbilities[ABILITY::DEXTERITY];
}

inline SBYTE const ABILITY::mfGetConstitution() const
{
	return fAbilities[ABILITY::CONSTITUTION];
}

inline SBYTE const ABILITY::mfGetIntelligence() const
{
	return fAbilities[ABILITY::INTELLIGENCE];
}

inline SBYTE const ABILITY::mfGetWisdom() const
{
	return fAbilities[ABILITY::WISDOM];
}

inline SBYTE const ABILITY::mfGetCharisma() const
{
	return fAbilities[ABILITY::CHARISMA];
}

// Set a new Strength value
inline void ABILITY::mfSetStrength(SBYTE const NewValue)			
{
	fAbilities[ABILITY::STRENGTH] = NewValue;
}

// Set a new Dexterity value
inline void ABILITY::mfSetDexterity(SBYTE const NewValue)			
{
	fAbilities[ABILITY::DEXTERITY] = NewValue;
}

// Set a new Constitution value
inline void ABILITY::mfSetConstitution(SBYTE const NewValue)			
{
	fAbilities[ABILITY::CONSTITUTION] = NewValue;
}

// Set a new Intelligence value
inline void ABILITY::mfSetIntelligence(SBYTE const NewValue)			
{
	fAbilities[ABILITY::INTELLIGENCE] = NewValue;
}

// Set a new Wisdom value.
inline void ABILITY::mfSetWisdom(SBYTE const NewValue)			
{
	fAbilities[ABILITY::WISDOM] = NewValue;
}

// Set a new Charisma value
inline void ABILITY::mfSetCharisma(SBYTE const NewValue)			
{
	fAbilities[ABILITY::CHARISMA] = NewValue;
}
#endif // _ABILITY_HXX
