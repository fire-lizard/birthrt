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
SBYTE m[][3] = {
{ 0,  0,  0 },
{ 2,  0,  1 },
{ 0,  1,  0 },
{ 1,  1,  0 },
{ 2,  1,  0 },
{ 3,  1,  0 },
{ 4,  3,  0 },
{ 2,  1,  0 },
{ 0,  -2,  0 },
{ 0,  0,  0 },
{ 3,  0,  0 },
{ 1,  1,  0 },
{ 0,  1,  0 },
{ 0,  1,  2 },
{ 0,  -2,  0 },
{ 0,  -2,  0 },
{ 2,  1,  0 },
{ 2,  0,  -2 }
};

SBYTE r[][11] = {
{ 1,1,1,1,1,1,1,1,1,1,1 },
{ 0,0,0,0,1,1,0,0,1,0,0 },
{ 0,0,0,0,1,0,0,0,0,0,0 },
{ 0,0,0,0,1,1,0,1,1,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0 },
{ 1,0,0,0,1,1,0,0,0,0,0 },
{ 1,0,0,0,1,0,0,0,0,0,0 },
{ 0,0,0,0,1,1,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,1,1 },
{ 0,0,0,0,0,0,1,1,0,0,0 },
{ 1,1,0,0,1,1,0,0,1,0,0 },
{ 0,0,0,0,0,1,0,1,1,0,0 },
{ 0,0,0,0,0,0,1,0,0,0,0 },
{ 0,0,0,0,0,0,1,0,0,0,0 },
{ 1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1 }
};

const ARMOR_INFO::ARMOR_ITEM ARMOR_INFO::fpArmorInfoTable[] = {
	{STR_ARMOR_NO_ARMOR,			   10, m[0], r[0]},
	{STR_ARMOR_BANDED_MAIL_ARMOR, 	  	4, m[1], r[1]},
	{STR_ARMOR_BREAST_PLATE,			6, m[2], r[2]}, // No Data for Modifiers
	{STR_ARMOR_BRIGANDINE_ARMOR,		6, m[3], r[3]},
	{STR_ARMOR_BRONZE_PLATE,			4, m[4], r[4]}, // No Data for Modifiers
	{STR_ARMOR_FIELD_PLATE,		 	  	2, m[5], r[5]},
	{STR_ARMOR_FULL_PLATE,		 	  	1, m[6], r[6]},
	{STR_ARMOR_HALF_PLATE,		 	  	4, m[7], r[7]}, // No Data for Modifiers
	{STR_ARMOR_HIDE_ARMOR,			  	5, m[8], r[8]},
	{STR_ARMOR_IMPROVED_MAIL,	  		6, m[9], r[9]}, // No Data for Modifiers
	{STR_ARMOR_PLATE_MAIL,		 	  	3, m[10], r[10]},
	{STR_ARMOR_RING_MAIL,		 		7, m[11], r[11]},
	{STR_ARMOR_SCALE_MAIL,		 	  	6, m[12], r[12]},
	{STR_ARMOR_SPLINT_MAIL,		   		4, m[13], r[13]},
	{STR_ARMOR_LEATHER_ARMOR,			8, m[14], r[14]},
	{STR_ARMOR_PADDED_ARMOR,	 		8, m[15], r[15]},
	{STR_ARMOR_STUDDED_LEATHER_ARMOR,	7, m[16], r[16]},
	{STR_ARMOR_CHAIN_MAIL,				5, m[17], r[17]}
};


SBYTE shield[][11] = {
	{ 1,1,1,1,1,1,1,1,1,1,1 },
	{ 0,1,1,0,1,1,1,1,1,1,1 },
	{ 1,1,1,1,1,1,1,1,1,1,1 } };
const SHIELD_INFO::SHIELD_ITEM SHIELD_INFO::fpShieldInfoTable[] = {
	//	   	 Melee Attack, Missile Attack, races.
	{"No Shield",		0, 				0, shield[0]},
	{"Body Shield",		1, 				2, shield[1]},
	{"Buckler Shield",	1, 				0, shield[2]}
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
