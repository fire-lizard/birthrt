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

#include "armor.hxx"
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
// TODO: (fire lizard) uncomment
const ARMOR_INFO::ARMOR_ITEM ARMOR_INFO::fpArmorInfoTable[100]={};/* = {
	{STR_ARMOR_NO_ARMOR,			   10, {0,  0,  0}, {1,1,1,1,1,1,1,1,1,1,1}},
	{STR_ARMOR_BANDED_MAIL_ARMOR, 	  	4, {2,  0,  1}, {0,0,0,0,1,1,0,0,1,0,0}},
	{STR_ARMOR_BREAST_PLATE,			6, {0,  1,  0}, {0,0,0,0,1,0,0,0,0,0,0}}, // No Data for Modifiers
	{STR_ARMOR_BRIGANDINE_ARMOR,		6, {1,  1,  0}, {0,0,0,0,1,1,0,1,1,0,0}},
	{STR_ARMOR_BRONZE_PLATE,			4, {2,  1,  0}, {0,0,0,0,0,0,0,0,0,0,0}}, // No Data for Modifiers
	{STR_ARMOR_FIELD_PLATE,		 	  	2, {3,  1,  0}, {1,0,0,0,1,1,0,0,0,0,0}},
	{STR_ARMOR_FULL_PLATE,		 	  	1, {4,  3,  0}, {1,0,0,0,1,0,0,0,0,0,0}},
	{STR_ARMOR_HALF_PLATE,		 	  	4, {2,  1,  0}, {0,0,0,0,1,1,0,0,0,0,0}}, // No Data for Modifiers
	{STR_ARMOR_HIDE_ARMOR,			  	5, {0, -2,  0}, {0,0,0,0,0,0,0,0,0,1,1}},
	{STR_ARMOR_IMPROVED_MAIL,	  		6, {0,  0,  0}, {0,0,0,0,0,0,1,1,0,0,0}}, // No Data for Modifiers
	{STR_ARMOR_PLATE_MAIL,		 	  	3, {3,  0,  0}, {1,1,0,0,1,1,0,0,1,0,0}},
	{STR_ARMOR_RING_MAIL,		 		7, {1,  1,  0}, {0,0,0,0,0,1,0,1,1,0,0}},
	{STR_ARMOR_SCALE_MAIL,		 	  	6, {0,  1,  0}, {0,0,0,0,0,0,1,0,0,0,0}},
	{STR_ARMOR_SPLINT_MAIL,		   		4, {0,  1,  2}, {0,0,0,0,0,0,1,0,0,0,0}},
	{STR_ARMOR_LEATHER_ARMOR,			8, {0, -2,  0}, {1,1,1,1,1,1,1,1,1,1,1}},
	{STR_ARMOR_PADDED_ARMOR,	 		8, {0, -2,  0}, {1,1,1,1,1,1,1,1,1,1,1}},
	{STR_ARMOR_STUDDED_LEATHER_ARMOR,	7, {2,  1,  0}, {1,1,1,1,1,1,1,1,1,1,1}},
	{STR_ARMOR_CHAIN_MAIL,				5, {2,  0, -2}, {1,1,1,1,1,1,1,1,1,1,1}}
};*/


// TODO: (fire lizard) uncomment
const SHIELD_INFO::SHIELD_ITEM SHIELD_INFO::fpShieldInfoTable[3]={};/* = {
	//	   	 Melee Attack, Missile Attack, races.
	{"No Shield",		0, 				0, {1,1,1,1,1,1,1,1,1,1,1}},
	{"Body Shield",		1, 				2, {0,1,1,0,1,1,1,1,1,1,1}},
	{"Buckler Shield",	1, 				0, {1,1,1,1,1,1,1,1,1,1,1}}
};*/

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
