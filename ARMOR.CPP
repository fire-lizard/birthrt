/* ========================================================================
   Copyright (c) 1990,1995	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: ARMOR.c  -
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <string.h>

#include "ARMOR.HXX"
//#include "gamestr.h"
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

// Note: The Modifiers came from the p121 Table #52 of the AD&D Handbook.
//       The Race limit information comes from Table 8 p.18 Birthright Rulebook.
//       The race limitation only exists for initial character creation. Characters
//       are allowed to buy or acquire other kinds of armor during the course of the game.
SBYTE m1[]{ 0,  0,  0 };
SBYTE r1[]{ 1,1,1,1,1,1,1,1,1,1,1 };
SBYTE m2[]{ 2,  0,  1 };
SBYTE r2[]{ 0,0,0,0,1,1,0,0,1,0,0 };
SBYTE m3[]{ 0,  1,  0 };
SBYTE r3[]{ 0,0,0,0,1,0,0,0,0,0,0 };
SBYTE m4[]{ 1,  1,  0 };
SBYTE r4[]{ 0,0,0,0,1,1,0,1,1,0,0 };
SBYTE m5[]{ 2,  1,  0 };
SBYTE r5[]{ 0,0,0,0,0,0,0,0,0,0,0 };
SBYTE m6[]{ 3,  1,  0 };
SBYTE r6[]{ 1,0,0,0,1,1,0,0,0,0,0 };
SBYTE m7[]{ 4,  3,  0 };
SBYTE r7[]{ 1,0,0,0,1,0,0,0,0,0,0 };
SBYTE m8[]{ 2,  1,  0 };
SBYTE r8[]{ 0,0,0,0,1,1,0,0,0,0,0 };
SBYTE m9[]{ 0,  -2,  0 };
SBYTE r9[]{ 0,0,0,0,0,0,0,0,0,1,1 };
SBYTE m10[]{ 0,  0,  0 };
SBYTE r10[]{ 0,0,0,0,0,0,1,1,0,0,0 };
SBYTE m11[]{ 3,  0,  0 };
SBYTE r11[]{ 1,1,0,0,1,1,0,0,1,0,0 };
SBYTE m12[]{ 1,  1,  0 };
SBYTE r12[]{ 0,0,0,0,0,1,0,1,1,0,0 };
SBYTE m13[]{ 0,  1,  0 };
SBYTE r13[]{ 0,0,0,0,0,0,1,0,0,0,0 };
SBYTE m14[]{ 0,  1,  2 };
SBYTE r14[]{ 0,0,0,0,0,0,1,0,0,0,0 };
SBYTE m15[]{ 0,  -2,  0 };
SBYTE r15[]{ 1,1,1,1,1,1,1,1,1,1,1 };
SBYTE m16[]{ 0,  -2,  0 };
SBYTE r16[]{ 1,1,1,1,1,1,1,1,1,1,1 };
SBYTE m17[]{ 2,  1,  0 };
SBYTE r17[]{ 1,1,1,1,1,1,1,1,1,1,1 };
SBYTE m18[]{ 2,  0,  -2 };
SBYTE r18[]{ 1,1,1,1,1,1,1,1,1,1,1 };
const ARMOR_INFO::ARMOR_ITEM ARMOR_INFO::fpArmorInfoTable[] = {
	{STR_ARMOR_NO_ARMOR,			   10, m1, r1},
	{STR_ARMOR_BANDED_MAIL_ARMOR, 	  	4, m2, r2},
	{STR_ARMOR_BREAST_PLATE,			6, m3, r3}, // No Data for Modifiers
	{STR_ARMOR_BRIGANDINE_ARMOR,		6, m4, r4},
	{STR_ARMOR_BRONZE_PLATE,			4, m5, r5}, // No Data for Modifiers
	{STR_ARMOR_FIELD_PLATE,		 	  	2, m6, r6},
	{STR_ARMOR_FULL_PLATE,		 	  	1, m7, r7},
	{STR_ARMOR_HALF_PLATE,		 	  	4, m8, r8}, // No Data for Modifiers
	{STR_ARMOR_HIDE_ARMOR,			  	5, m9, r9},
	{STR_ARMOR_IMPROVED_MAIL,	  		6, m10, r10}, // No Data for Modifiers
	{STR_ARMOR_PLATE_MAIL,		 	  	3, m11, r11},
	{STR_ARMOR_RING_MAIL,		 		7, m12, r12},
	{STR_ARMOR_SCALE_MAIL,		 	  	6, m13, r13},
	{STR_ARMOR_SPLINT_MAIL,		   		4, m14, r14},
	{STR_ARMOR_LEATHER_ARMOR,			8, m15, r15},
	{STR_ARMOR_PADDED_ARMOR,	 		8, m16, r16},
	{STR_ARMOR_STUDDED_LEATHER_ARMOR,	7, m17, r17},
	{STR_ARMOR_CHAIN_MAIL,				5, m18, r18}
};


SBYTE ns[]{ 1,1,1,1,1,1,1,1,1,1,1 };
SBYTE bs[]{ 0,1,1,0,1,1,1,1,1,1,1 };
SBYTE bcs[]{ 1,1,1,1,1,1,1,1,1,1,1 };
const SHIELD_INFO::SHIELD_ITEM SHIELD_INFO::fpShieldInfoTable[] = {
	//	   	 Melee Attack, Missile Attack, races.
	{"No Shield",		0, 				0, ns},
	{"Body Shield",		1, 				2, bs},
	{"Buckler Shield",	1, 				0, bcs}
};

/* ========================================================================
   Function    - mfFindType
   Description - Matchup the armor name to its type.
   Returns     - 
   ======================================================================== */
const ARMOR_INFO::TYPE ARMOR_INFO::mfFindType(char * cpArmorName)
{
	TYPE Result = NO_ARMOR;
	LONG i;
	
	for (i = FIRST_ARMOR; i < ARMOR_MAX_INDEX; i++)
	{
		if (0 == strncmp(ARMOR_INFO::mfGetName((TYPE)i), cpArmorName, strlen(ARMOR_INFO::mfGetName((TYPE) i))))
		{
			Result = (TYPE) i;
			break;
		}
	}
	
	return Result;
}

/* ========================================================================
   Function    - mfFindType
   Description - Match up the shield name to its type.
   Returns     - 
   ======================================================================== */
const SHIELD_INFO::TYPE SHIELD_INFO::mfFindType(char * cpShieldName)
{
	TYPE Result = NO_SHIELD;
	LONG i;
	
	for (i = FIRST_SHIELD; i < SHIELD_MAX_INDEX; i++)
	{
		if (0 == strncmp(SHIELD_INFO::mfGetName((TYPE)i), cpShieldName, strlen(SHIELD_INFO::mfGetName((TYPE) i))))
		{
			Result = (TYPE) i;
			break;
		}
	}
	
	return Result;
}

/* ========================================================================
   Function    - mfGetWeaponAdjust
   Description - For a given weapon style get the best adjustment.
   Returns     - A modifier for ToHit.
   ======================================================================== */

// Get the weapon adjustment for this armor.
const SBYTE ARMOR_INFO::mfGetWeaponAdjust(
	const ARMOR_INFO::TYPE ArmorType, 
	const WEAPON_INFO::STYLE WeaponStyle)
{
	SBYTE Result = 0;
	
	if (WeaponStyle & WEAPON_INFO::SLASHING)
	{
		Result = fpArmorInfoTable[ArmorType].fbModifiers[0];
		if (WeaponStyle & WEAPON_INFO::PIERCING)
		{
			if (Result > fpArmorInfoTable[ArmorType].fbModifiers[1])
			{
				Result = fpArmorInfoTable[ArmorType].fbModifiers[1];
			}
		}
		else
		if (WeaponStyle & WEAPON_INFO::BLUDGEONING)
		{
			if (Result > fpArmorInfoTable[ArmorType].fbModifiers[2])
			{
				Result = fpArmorInfoTable[ArmorType].fbModifiers[2];
			}
		}
	}
	else
	if (WeaponStyle & WEAPON_INFO::PIERCING)
	{
		Result = fpArmorInfoTable[ArmorType].fbModifiers[1];
		if (WeaponStyle & WEAPON_INFO::BLUDGEONING)
		{
			if (Result > fpArmorInfoTable[ArmorType].fbModifiers[2])
			{
				Result = fpArmorInfoTable[ArmorType].fbModifiers[2];
			}
		}
	}
	else
	if (WeaponStyle & WEAPON_INFO::BLUDGEONING)
	{
		Result = fpArmorInfoTable[ArmorType].fbModifiers[2];
	}

	return Result;
}
