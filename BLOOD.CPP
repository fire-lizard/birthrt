/* ========================================================================
   Copyright (c) 1990,1995	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: blood.cpp  -
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "blood.hxx"
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


BLOODLINE_STRENGTH_INFO::BLOODLINE_STRENGTH_DATA BLOODLINE_STRENGTH_INFO::fpBloodlineStrengthInfo[] = {
	{STR_BLOOD_STRENGTH_TAINTED,	{4, 4, 0}},
	{STR_BLOOD_STRENGTH_MINOR,		{5, 6, 0}},
	{STR_BLOOD_STRENGTH_MAJOR,		{8, 6, 0}},
	{STR_BLOOD_STRENGTH_GREAT,		{8, 8, 0}}
};

GAME_STRING BLOODLINE_DERIVATION_INFO::fpBloodlineDerivationInfo[] = {
	STR_BLOOD_DERIV_ANDUIRAS,
	STR_BLOOD_DERIV_REYNIR,
	STR_BLOOD_DERIV_BRENNA,
	STR_BLOOD_DERIV_BASAIA,
	STR_BLOOD_DERIV_MASELA,
	STR_BLOOD_DERIV_VORYNN,
	STR_BLOOD_DERIV_AZRAI
};

GAME_STRING BLOOD_ABILITY_INFO::fpBloodAbilityInfo[] = {
      	STR_BLOODAB_NAME_ALERTNESS,
	STR_BLOODAB_NAME_ALTER_APPEARANCE,
	STR_BLOODAB_NAME_ANIMAL_AFFINITY,
	STR_BLOODAB_NAME_BATTLEWISE,
	STR_BLOODAB_NAME_BLOOD_HISTORY,
	STR_BLOODAB_NAME_BLOODMARK,
	STR_BLOODAB_NAME_CHARACTER_READING,
	STR_BLOODAB_NAME_COURAGE,
	STR_BLOODAB_NAME_DETECT_LIE,
	STR_BLOODAB_NAME_DETECT_ILLUSION,
	STR_BLOODAB_NAME_DIRECTION_SENSE,
	STR_BLOODAB_NAME_DIVINE_AURA,
	STR_BLOODAB_NAME_DIVINE_WRATH,
	STR_BLOODAB_NAME_ELEMENTAL_CONTROL,
	STR_BLOODAB_NAME_ENHANCED_SENSE,
	STR_BLOODAB_NAME_FEAR,
	STR_BLOODAB_NAME_HEALING,
	STR_BLOODAB_NAME_HEALING,
	STR_BLOODAB_NAME_HEALING,
	STR_BLOODAB_NAME_HEIGHTENED_ABILITY,
	STR_BLOODAB_NAME_IRON_WILL,
	STR_BLOODAB_NAME_PERSUASION,
	STR_BLOODAB_NAME_POISON_SENSE,
	STR_BLOODAB_NAME_PROTECTION_FROM_EVIL,
	STR_BLOODAB_NAME_REGENERATION,
	STR_BLOODAB_NAME_RESISTANCE,
	STR_BLOODAB_NAME_SHADOW_FORM,
	STR_BLOODAB_NAME_TOUCH_OF_DECAY,
	STR_BLOODAB_NAME_TRAVEL,
};
