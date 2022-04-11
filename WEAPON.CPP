/* ========================================================================
   Copyright (c) 1990,1995	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: WEAPON.cpp  -
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <string.h>

//#include "gamestr.h"
#include "weapon.hxx"
#include "strenum.h"   


/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
// TODO: (fire lizard) uncomment
const WEAPON_INFO::WEAPON_ITEM WEAPON_INFO::fpWeaponTableInfo[100] = {};/*{
	// Page 18 Table 9 Birthright Rulebook.
	// The Race limitations are from Table 7 p. 17 Birthright Rulebook.
	//                                                            			  speed, sm damage,  Lg damage,  D  E HE  H An  B  K  R  V Go  Gn
	{STR_NULL,   			  			WEAPON_INFO::NO_SIZE,	WEAPON_INFO::NO_STYLE,	  0, {0, 0, 0}, {0,  0, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
	{STR_WEAPON_CLAYMORE,		  		WEAPON_INFO::MEDIUM,	WEAPON_INFO::SLASHING,	  7, {2, 4, 0}, {2,  8, 0}, {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}},
	{STR_WEAPON_CROSSBOW_LT_WEIGHT,		WEAPON_INFO::MEDIUM,	WEAPON_INFO::PIERCING,	  7, {1, 6, 1}, {1,  8, 1}, {1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0}},
	{STR_WEAPON_CROSSBOW_HV_WEIGHT,		WEAPON_INFO::LARGE,		WEAPON_INFO::PIERCING,	 10, {1, 8, 1}, {1, 10, 1}, {1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0}},
	{STR_WEAPON_CUTLASS,				WEAPON_INFO::MEDIUM,	WEAPON_INFO::SLASHING,	  5, {1, 6, 0}, {1,  8, 0}, {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0}},
	{STR_WEAPON_MAIN_GAUCHE,			WEAPON_INFO::SMALL,		WEAPON_INFO::PIERCING,	  2, {1, 4, 0}, {1,  3, 0}, {0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0}},	// This is a dagger weapon.
	{STR_WEAPON_RAPIER,					WEAPON_INFO::MEDIUM,	WEAPON_INFO::PIERCING,	  4, {1, 6, 1}, {1,  8, 1}, {0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0}},
	{STR_WEAPON_SABRE,			  		WEAPON_INFO::MEDIUM,	WEAPON_INFO::SLASHING,	  5, {1, 6, 1}, {1,  8, 1}, {0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0}},
	{STR_WEAPON_MACE_HORSEMANS,			WEAPON_INFO::MEDIUM,	WEAPON_INFO::BLUDGEONING, 6, {1, 6, 0}, {1,  4, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
	{STR_WEAPON_MACE_FOOTMANS,			WEAPON_INFO::MEDIUM,	WEAPON_INFO::BLUDGEONING, 7, {1, 6, 1}, {1,  6, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
	{STR_WEAPON_MORNINGSTAR,			WEAPON_INFO::MEDIUM,	WEAPON_INFO::BLUDGEONING, 7, {2, 4, 0}, {1,  6, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
	{STR_WEAPON_BROAD_SWORD,			WEAPON_INFO::MEDIUM,	WEAPON_INFO::SLASHING,	  5, {2, 4, 0}, {1,  6, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
	{STR_WEAPON_SHORT_SWORD,			WEAPON_INFO::SMALL,		WEAPON_INFO::SLASHING,	  5, {1, 6, 0}, {1,  8, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
	{STR_WEAPON_DAGGER,		  			WEAPON_INFO::SMALL,		WEAPON_INFO::SLASHING,	  2, {1, 4, 0}, {1,  3, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
	{STR_WEAPON_LANCE,		  			WEAPON_INFO::MEDIUM,	WEAPON_INFO::PIERCING,	  7, {1, 6, 1}, {2,  6, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
	{STR_WEAPON_BATTLE_AXE,	  			WEAPON_INFO::SMALL,		WEAPON_INFO::SLASHING,	  7, {1, 8, 0}, {1,  8, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
	{STR_WEAPON_HALBERD,	  			WEAPON_INFO::LARGE,		WEAPON_INFO::SLASHING,	  9, {1,10, 0}, {2,  6, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
	{STR_WEAPON_SLEDGE_HAMMER,			WEAPON_INFO::MEDIUM,	WEAPON_INFO::BLUDGEONING, 8, {1, 6, 1}, {1,  4, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
	{STR_WEAPON_CLUB_GREAT,				WEAPON_INFO::LARGE,		WEAPON_INFO::BLUDGEONING, 9, {2, 4, 0}, {1,  6, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
	{STR_WEAPON_BOW,			  		WEAPON_INFO::SMALL,		WEAPON_INFO::PIERCING,	  6, {1, 6, 0}, {1,  6, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
	{STR_WEAPON_ONE_HAND_SPEAR,  		WEAPON_INFO::MEDIUM,	WEAPON_INFO::PIERCING,	  0, {1, 6, 0}, {1,  8, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
	{STR_WEAPON_TWO_HAND_SPEAR,  		WEAPON_INFO::LARGE,		WEAPON_INFO::PIERCING,	  0, {1, 6, 1}, {2,  6, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
	{STR_WEAPON_TWO_HAND_SWORD,  		WEAPON_INFO::LARGE,		WEAPON_INFO::SLASHING,	 10, {1,10, 0}, {3,  6, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
	{STR_WEAPON_HORSEMANS_FLAIL,  		WEAPON_INFO::MEDIUM,	WEAPON_INFO::BLUDGEONING, 6, {1, 4, 1}, {1,  4, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
	{STR_WEAPON_TWO_HAND_BSTD_SWORD,	WEAPON_INFO::MEDIUM,	WEAPON_INFO::SLASHING,	  6, {2, 4, 0}, {2,  8, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
	{STR_WEAPON_CLUB_WAR,  		  		WEAPON_INFO::MEDIUM,	WEAPON_INFO::BLUDGEONING, 7, {1, 6, 1}, {1,  4, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
	{STR_WEAPON_QUARTER_STAFF,	  		WEAPON_INFO::LARGE,		WEAPON_INFO::BLUDGEONING, 4, {1, 6, 0}, {1,  6, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}
};*/

/* ========================================================================
   Function    - mfFindType
   Description - Attach the avatar to the menu buttons & show the menu.
   Returns     - 
   ======================================================================== */
const WEAPON_INFO::TYPE WEAPON_INFO::mfFindType(char * cpWeaponName)
{
	TYPE Result = NO_WEAPON;
	LONG i;
	
	for (i = FIRST_WEAPON; i < WEAPON_MAX_INDEX; i++)
	{
		if (0 == strncmp(WEAPON_INFO::mfGetName((TYPE)i), cpWeaponName, strlen(WEAPON_INFO::mfGetName((TYPE) i))))
		{
			Result = (TYPE) i;
			break;
		}
	}
	
	return Result;
}

