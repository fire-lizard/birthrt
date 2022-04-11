/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: ARMOR.HXX - Collection of Armor for characters
   Author: 	 Gary Powell
   ======================================================================== */
#ifndef _ARMOR_HXX
#define _ARMOR_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYEPDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_PLAYRACE_HXX)
#include "playrace.hxx"
#endif
//#include "gamestr.h"

#if !defined(_WEAPON_HXX)
#include "weapon.hxx"
#endif

#if !defined(_STRENUM_H)
#include "strenum.h"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
// HUNGARIAN NOTE: All values are of type LONG unless specified.  So if
// is doesn't start with a capital letter, the lower case should be a
// prefix.  'u' means ULONG.


class ARMOR_INFO {
public:
	typedef enum {
		NO_ARMOR = 0,
		FIRST_ARMOR = 0,
		BANDED_MAIL,
		BREASTPLATE,
		BRIGANDINE,
		BRONZE_PLATE,
		FIELD_PLATE,
		FULL_PLATE,
		HALF_PLATE,
		HIDE,
		IMPROVED_MAIL,
		PLATE_MAIL,
		RING_MAIL,
		SCALE_MAIL,
		SPLINT_MAIL,
		LEATHER,
		PADDED,
		STUDDED_LEATHER,
		CHAIN_MAIL,
	
		// Insert New Armor above this line!
		ARMOR_MAX_INDEX
	} TYPE;

	class ARMOR_ITEM {
	public:
		friend class ARMOR_INFO;
		ARMOR_ITEM(GAME_STRING fName, SBYTE fbArmorClass, SBYTE fbModifiers[WEAPON_INFO::MAX_WEAPON_STYLES], SBYTE fbRacesAvailable[RACE_INFO::RACE_MAX_NUMBER])
		{
			this->fName = fName;
			this->fbArmorClass = fbArmorClass;
			for (int index = 0; index < WEAPON_INFO::MAX_WEAPON_STYLES; index++)
			{
				this->fbModifiers[index] = fbModifiers[index];
			}
			for (int index = 0; index < RACE_INFO::RACE_MAX_NUMBER; index++)
			{
				this->fbRacesAvailable[index] = fbRacesAvailable[index];
			}
		}
	private:
		GAME_STRING fName;
		SBYTE	fbArmorClass;
		SBYTE	fbModifiers[WEAPON_INFO::MAX_WEAPON_STYLES];	// Use the Weapon Type to index
		SBYTE	fbRacesAvailable[RACE_INFO::RACE_MAX_NUMBER]; 		// Use Player Race Type to index
	};
	
	static char const * const mfGetName(const TYPE);
	static const SBYTE mfGetArmorClass(const TYPE);
	static const SBYTE mfGetWeaponAdjust(const TYPE, const WEAPON_INFO::STYLE);
	static const BOOL mfIsArmorAllowed(const TYPE, const RACE_INFO::TYPE);
	static const TYPE mfFindType(char *);
	
private:
	static const ARMOR_ITEM fpArmorInfoTable[];
};
typedef ARMOR_INFO *PTR_ARMOR_INFO;

class SHIELD_INFO {
public:
	typedef enum {
		NO_SHIELD = 0,
		FIRST_SHIELD = 0,
		BODY,
		BUCKLER,
		
		// That's all the shields I know about.
		SHIELD_MAX_INDEX
	} TYPE;

	class SHIELD_ITEM {
	public:
		SHIELD_ITEM(char *fcpName, SBYTE fbMeleeAttackModifier, SBYTE fbMissleAttackModifier, SBYTE fbRacesAvailable[RACE_INFO::RACE_MAX_NUMBER])
		{
			this->fcpName = fcpName;
			this->fbMeleeAttackModifier = fbMeleeAttackModifier;
			this->fbMissleAttackModifier = fbMissleAttackModifier;
			for (int index = 0; index < RACE_INFO::RACE_MAX_NUMBER; index++)
			{
				this->fbRacesAvailable[index] = fbRacesAvailable[index];
			}
		}
		friend class SHIELD_INFO;
	private:
		char *	fcpName;
		SBYTE	fbMeleeAttackModifier;
		SBYTE	fbMissleAttackModifier;
		SBYTE	fbRacesAvailable[RACE_INFO::RACE_MAX_NUMBER];	// Use Player Race Type to index
	};
	
	static char *mfGetName(const TYPE);
	static const SBYTE mfGetMeleeModifer(const TYPE);
	static const SBYTE mfGetMissleModifer(const TYPE);
	static const BOOL mfIsShieldAllowed(const TYPE, const RACE_INFO::TYPE);
	static const TYPE mfFindType(char *);
	
private:
	static const SHIELD_ITEM fpShieldInfoTable[];
};
typedef SHIELD_INFO *PTR_SHIELD_INFO;

// Get the name of this armor
inline char const * const ARMOR_INFO::mfGetName(
const ARMOR_INFO::TYPE ArmorType)
{
	return STRMGR_GetStr(fpArmorInfoTable[ArmorType].fName);
}

// Get the armor class value for an armor.
inline const SBYTE ARMOR_INFO::mfGetArmorClass(
	const ARMOR_INFO::TYPE ArmorType)
{
	return fpArmorInfoTable[ArmorType].fbArmorClass;
}


// Test whether a player of this race can use this kind of armor.
inline const BOOL ARMOR_INFO::mfIsArmorAllowed(
	const ARMOR_INFO::TYPE ArmorType, 
	const RACE_INFO::TYPE PlayerRace)
{
	return fpArmorInfoTable[ArmorType].fbRacesAvailable[PlayerRace];
}

// Get the shield description name.
inline char *SHIELD_INFO::mfGetName(const SHIELD_INFO::TYPE ShieldType)
{
	return fpShieldInfoTable[ShieldType].fcpName;
}

// Get the Melee modifier for characters using this shield
inline const SBYTE SHIELD_INFO::mfGetMeleeModifer(const SHIELD_INFO::TYPE ShieldType)
{
	return fpShieldInfoTable[ShieldType].fbMeleeAttackModifier;
}

// Get the missle attack modifer for characters using this shield
inline const SBYTE SHIELD_INFO::mfGetMissleModifer(const SHIELD_INFO::TYPE ShieldType)
{
	return fpShieldInfoTable[ShieldType].fbMissleAttackModifier;
}

// Test whether this character can use this shield
inline const BOOL SHIELD_INFO::mfIsShieldAllowed(
	const SHIELD_INFO::TYPE ShieldType, 
	const RACE_INFO::TYPE PlayerRace)
{
	return fpShieldInfoTable[ShieldType].fbRacesAvailable[PlayerRace];
}

#endif // _ARMOR_HXX

