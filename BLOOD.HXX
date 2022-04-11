/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: BLOOD.HXX - Collection of Bloodline Abilities for characters
   Author: 	 Gary Powell
   ======================================================================== */
#ifndef _BLOOD_HXX
#define _BLOOD_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_DICE_H)
#include "dice.h"
#endif

//#include "gamestr.h"
#if !defined(_STRENUM_H)
#include "strenum.h"
#endif
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */

// Note: Order is important because this array is autofilled.
//       If you change this structure be sure to change RAbility.c

class BLOODLINE_STRENGTH_INFO {
public:
	typedef enum {
		TAINTED	= 0,
		MINOR,
		MAJOR,
		GREAT,
	
		// This is it, don't add more!
		MAX_BLOOD_STRENGTH
	} TYPE;
	
	class BLOODLINE_STRENGTH_DATA {
	public:
		GAME_STRING	Name;
		DICE	Score;				// Unknown usefulness of this value.
	};
	
	static char const * const mfGetName(const TYPE);
	static const DICE *mfGetScore(const TYPE);

private:
	static BLOODLINE_STRENGTH_DATA fpBloodlineStrengthInfo[];
};
typedef BLOODLINE_STRENGTH_INFO *PTR_BLOODLINE_STRENGTH_INFO;

// Note: This enum type is used as an index into several arrays
//       which track Bloodline Derivation information. Don't re-order it!
// This structure is to hold information about the bloodline
// derivations needed while the game is in progress.
class BLOODLINE_DERIVATION_INFO {
public:
	typedef enum {
		ANDUIRAS = 0,
		REYNIR,
		BRENNA,
		BASAIA,
		MASELA,
		VORYNN,
		AZRAI,
	
		// This is it, don't add more!
		MAX_DERIVATION
	} TYPE;

	static char const * const mfGetName(const TYPE);
	
private:
	static GAME_STRING fpBloodlineDerivationInfo[];
};

typedef BLOODLINE_DERIVATION_INFO *PTR_BLOODLINE_DERIVATION_INFO;

// Note: This enum type is used as an index into several arrays
//       which track Blood Ability information. Don't re-order it!

// This structure is to hold information about the Blood Abilities
// derivations needed while the game is in progress.

class BLOOD_ABILITY_INFO {
public:
	typedef enum {
		ALERTNESS = 0,
		ALTER_APPEARANCE,
		ANIMAL_AFFINITY,
		BATTLEWISE,
		BLOOD_HISTORY,
		BLOODMARK,
		CHARACTER_READING,
		COURAGE,
		DETECT_LIE,
		DETECT_ILLUSION,
		DIRECTION_SENSE,
		DIVINE_AURA,
		DEVINE_WRATH,
		ELEMENTAL_CONTROL,
		ENHANCED_SENSE,
		FEAR,
		HEALING_MINOR,
		HEALING_MAJOR,
		HEALING_GREAT,
		HEIGHTENED_ABILITY,
		IRON_WILL,
		PERSUASION,
		POISON_SENSE,
		PROTECTION_FROM_EVIL,
		REGENERATION,
		RESISTANCE,
		SHADOW_FORM,
		TOUCH_OF_DECAY,
		TRAVEL,
		UNREADABLE_THOUGHTS,
	
		// Don't add to this list, that's all there are.
		MAX_ABILITIES
	} BLOOD_ABILITY;
	
	static char const * const mfGetName(const BLOOD_ABILITY);
	
private:
	static GAME_STRING	fpBloodAbilityInfo[];
};

typedef BLOOD_ABILITY_INFO *PTR_BLOOD_ABILITY_INFO;

// Every blooded player gets a list of abilities, this is
// how its stored.
typedef SHORT HDL_BLOOD_ABILITY;

class BLOOD_ABILITIES_LIST {
public:
	HDL_BLOOD_ABILITY					next;
	BLOOD_ABILITY_INFO::BLOOD_ABILITY	BloodAbility;
};

typedef BLOOD_ABILITIES_LIST *PTR_BLOOD_ABILITIES_LIST;

// This structure is used in the game to track
// the Bloodline information.
class PLAYER_BLOOD_INFO {
public:
	BLOODLINE_STRENGTH_INFO::TYPE	Type;
	BLOODLINE_DERIVATION_INFO::TYPE	DerivationType;
	SHORT							sStrengthValue;
};

typedef PLAYER_BLOOD_INFO *PTR_PLAYER_BLOOD_INFO;

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

// Get the name of this bloodline strength.
inline char const * const BLOODLINE_STRENGTH_INFO::mfGetName(
	const BLOODLINE_STRENGTH_INFO::TYPE type)
{
	return STRMGR_GetStr(fpBloodlineStrengthInfo[type].Name);
}

// Get the score dice for this bloodline strength.
inline const DICE *BLOODLINE_STRENGTH_INFO::mfGetScore(
	const BLOODLINE_STRENGTH_INFO::TYPE type)
{
	return &fpBloodlineStrengthInfo[type].Score;
}

// Get the name of this bloodline derivation.
inline char const * const BLOODLINE_DERIVATION_INFO::mfGetName(
	const BLOODLINE_DERIVATION_INFO::TYPE type)
{
	return STRMGR_GetStr(fpBloodlineDerivationInfo[type]);
}

// Get the name of this blood ability.
inline char const * const BLOOD_ABILITY_INFO::mfGetName(
	const BLOOD_ABILITY_INFO::BLOOD_ABILITY bloodAbility)
{
	return STRMGR_GetStr(fpBloodAbilityInfo[bloodAbility]);
}


#endif // _BLOOD_H
