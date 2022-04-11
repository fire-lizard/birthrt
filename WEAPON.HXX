
/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _WEAPON_HXX
#define _WEAPON_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_DICE_H)
#include "dice.h"
#endif

//#include "gamestr.h"
#if !defined(_PLAYRACE_HXX)
#include "playrace.hxx"
#endif

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

class WEAPON_INFO {
public:
	typedef enum {
		NO_SIZE = 0,
		SMALL,
		MEDIUM,
		LARGE,
		HUGE			// Huge weapons are for Ogres and Giants only.
	} SIZE;
	
	// Note: Weapon Style is used to find up the Armor Modifiers
	typedef enum {
		NO_STYLE	= 0,
		SLASHING    = 0x1,
		PIERCING    = 0x2,
		BLUDGEONING = 0x4,
		// This is it, there are no more. (See armor.hxx if you need to add more.)
		MAX_WEAPON_STYLES = 3,
	} STYLE;
	
	// Weapon handedness has more to do with how a character
	// wields the weapon than the weapon design. ie Small characters
	// must use medium size weapons with two hands.
	typedef enum {
		RIGHT_HANDED,
		LEFT_HANDED,
		TWO_HANDED
	} HANDEDNESS;	// Unused for now.

	typedef enum {
		NO_WEAPON = 0,
		FIRST_WEAPON = 1,
		CLAYMORE = 1,
		CROSSBOW_LT,
		CROSSBOW_HVY,
		CUTLASS,
		MAIN_GAUCHE,
		RAPIER,
		SABRE,
		MACE_HORSEMANS,
		MACE_FOOTMANS,
		MORNINGSTAR,
		BROAD_SWORD,
		SHORT_SWORD,
		DAGGER,
		LANCE,
		BATTLE_AXE,
		HALBERD,
		SLEDGE_HAMMER,
		CLUB_GREAT,
		BOW,
		ONE_HAND_SPEAR,
		TWO_HAND_SPEAR,
		TWO_HAND_SWORD,
		HORSEMANS_FLAIL,
		TWO_HAND_BASTARD_SWORD,
		CLUB_WAR,
		QUARTER_STAFF,
	
		// Add any new weapons above this line.
		// As the indexs are stored in the .ava files.
		WEAPON_MAX_INDEX
	} TYPE;
	
	class WEAPON_ITEM {
	public:
		friend class WEAPON_INFO;
	private:
		GAME_STRING   fName;
		SIZE	   fSize;
		STYLE	   fStyle;
		SBYTE	   fbSpeed;
		DICE	   fSmallCreatureDamageDice; // And Medium Creatures too.
		DICE	   fLargeCreatureDamageDice;
		SBYTE	   fsRacesAvailable[RACE_INFO::RACE_MAX_NUMBER];	// Use Player Race Type to index
	};
	
	static char const * const mfGetName(const TYPE);
	static const STYLE mfGetStyle(const TYPE);
	static SIZE const mfGetSize(TYPE const);
	static const DICE *mfGetLargeCreatureDamage(const TYPE);
	static const DICE *mfGetSmallCreatureDamage(const TYPE);
	static const BOOL mfIsWeaponAvailable(const TYPE, const RACE_INFO::TYPE);
	
	static const TYPE mfFindType(char *);

protected:
private:

	static const WEAPON_ITEM fpWeaponTableInfo[];
};
typedef  WEAPON_INFO *PTR_WEAPON_INFO;


/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

// Get the description name for this weapon.
inline char const * const WEAPON_INFO::mfGetName(const WEAPON_INFO::TYPE WeaponType)
{
	return STRMGR_GetStr(fpWeaponTableInfo[WeaponType].fName);
}

// Get the weapon style for this weapon.
inline const WEAPON_INFO::STYLE WEAPON_INFO::mfGetStyle(const WEAPON_INFO::TYPE WeaponType)
{
	return fpWeaponTableInfo[WeaponType].fStyle;
}

// Get the weapon size for this weapon.
inline WEAPON_INFO::SIZE const WEAPON_INFO::mfGetSize(WEAPON_INFO::TYPE const WeaponType)
{
	return fpWeaponTableInfo[WeaponType].fSize;
}

// Get the damage dice for this weapon
inline const DICE *WEAPON_INFO::mfGetLargeCreatureDamage(const WEAPON_INFO::TYPE WeaponType)
{
	return &fpWeaponTableInfo[WeaponType].fLargeCreatureDamageDice;
}

// Get the damage dice for this weapon
inline const DICE *WEAPON_INFO::mfGetSmallCreatureDamage(const WEAPON_INFO::TYPE WeaponType)
{
	return &fpWeaponTableInfo[WeaponType].fSmallCreatureDamageDice;
}

// Test whether this charactar can have this weapon
inline const BOOL WEAPON_INFO::mfIsWeaponAvailable(
	const WEAPON_INFO::TYPE WeaponType, 
	const RACE_INFO::TYPE PlayerRace)
{
	return fpWeaponTableInfo[WeaponType].fsRacesAvailable[PlayerRace];
}
#endif // _WEAPON_HXX
