/* ========================================================================
   Copyright (c) 1990,1995	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: BLOODREQ.c  -
   Author:   Gary Powell

   ========================================================================

   Contains the following general functions:


   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#include "system.h"

#include "bloodreq.hxx"
#include "dice.h"

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
   Typedefs
   ------------------------------------------------------------------------ */
// Note: I may want to percolate this information back to the user.
typedef enum {
	CHECK_BLOODLINE_ROLL_CANT_KEEP	= 0,
	CHECK_BLOODLINE_ROLL_EXACT_MATCH,
	CHECK_BLOODLINE_ROLL_WILL_FIT
} CHECK_BLOODLINE_ROLL_RESULT;

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

class BLOOD_ABILITY_REQ {
public:
	SBYTE	StrengthReq[BLOODLINE_STRENGTH_INFO::GREAT + 1];
	SBYTE	DerivationRew[BLOODLINE_DERIVATION_INFO::MAX_DERIVATION];
};

typedef BLOOD_ABILITY_REQ *PTR_BLOOD_ABILITY_REQ;

// Data from Birthright Rules Handbook p.22 Table 13
BLOOD_ABILITY_REQ BloodAbilityReqTable[] = {
//     T Mi Ma  G    An R Br Ba  M  V Az
	{{ 0, 1, 0, 0},{ 0, 1, 1, 1, 0, 0, 1}},		// Alertness
	{{ 0, 0, 1, 0},{ 0, 0, 1, 0, 0, 1, 1}},		// Alter Appearance
	{{ 0, 1, 1, 1},{ 1, 1, 1, 1, 1, 1, 1}},		// Animal Affinity
	{{ 0, 0, 1, 0},{ 1, 0, 0, 0, 0, 0, 1}},		// Battlewise
	{{ 0, 1, 0, 0},{ 0, 0, 1, 0, 1, 1, 0}},		// Blood History
	{{ 0, 1, 0, 0},{ 1, 1, 1, 1, 1, 1, 1}},		// Bloodmark
	{{ 0, 0, 1, 0},{ 0, 0, 1, 1, 0, 1, 0}},		// Character Reading
	{{ 0, 1, 1, 1},{ 1, 0, 0, 0, 0, 0, 0}},		// Courage
	{{ 0, 1, 0, 0},{ 1, 1, 1, 1, 1, 1, 0}},		// Detect Lie
	{{ 0, 1, 0, 0},{ 0, 1, 1, 1, 1, 1, 1}},		// Detect Illusion
	{{ 0, 1, 0, 0},{ 0, 1, 0, 0, 1, 0, 0}},		// Direction Sense
	{{ 0, 0, 1, 1},{ 1, 1, 0, 1, 1, 1, 1}},		// Devine Aura
	{{ 0, 0, 1, 0},{ 1, 0, 0, 1, 1, 0, 0}},		// Devine Wrath
	{{ 0, 0, 0, 1},{ 1, 1, 0, 1, 1, 0, 0}},		// Elemental Control
	{{ 0, 1, 1, 0},{ 1, 1, 1, 1, 1, 1, 1}},		// Enhanced Sense
	{{ 0, 0, 1, 0},{ 0, 0, 0, 0, 0, 0, 1}},		// Fear
	{{ 0, 1, 1, 1},{ 1, 1, 0, 1, 0, 0, 0}},		// Healing
	{{ 0, 1, 0, 0},{ 1, 1, 1, 1, 0, 1, 1}},		// Heightened Ability
	{{ 0, 1, 0, 0},{ 1, 1, 0, 0, 0, 0, 1}},		// Iron Will
	{{ 0, 0, 1, 0},{ 0, 0, 1, 0, 0, 0, 1}},		// Persuasion
	{{ 0, 1, 0, 0},{ 0, 1, 0, 0, 0, 0, 1}},		// Poison Sense
	{{ 0, 0, 1, 0},{ 1, 1, 1, 1, 1, 1, 0}},		// Protection from Evil
	{{ 0, 0, 0, 1},{ 1, 1, 0, 0, 0, 0, 1}},		// Regeneration
	{{ 0, 1, 1, 1},{ 1, 1, 1, 1, 1, 1, 1}},		// Resistance
	{{ 0, 0, 0, 1},{ 0, 0, 1, 0, 0, 0, 1}},		// Shadow Form
	{{ 0, 0, 0, 1},{ 0, 0, 0, 0, 0, 0, 1}},		// Touch Of Decay
	{{ 0, 0, 0, 1},{ 0, 0, 1, 1, 1, 1, 1}},		// Travel
	{{ 0, 1, 0, 0},{ 1, 1, 1, 1, 1, 1, 1}}		// Unreadable Thoughts
};

class BLOOD_ABILITY_GAINED {
public:
	typedef enum {
		NONE,
		MINOR,
		MAJOR,
		GREAT,
		ENHANCED_TO_MAJOR,
		ENHANCED_TO_GREAT
	} TYPE;

	SHORT	sHighLimit;
	TYPE	BloodAbilityGain;
};

typedef BLOOD_ABILITY_GAINED *PTR_BLOOD_ABILITY_GAINED;

#define MAX_BLOOD_ABLITY_GAIN	6 	// 6 is enough space for the largest subArray.

typedef struct _BLOOD_ABILITY_ACQUISITION {
	SBYTE						sBloodStrengthHighValue;
	SBYTE                       sNumberOfGainedValues;
	BLOOD_ABILITY_GAINED		BloodAbilityGain[MAX_BLOOD_ABLITY_GAIN];
} BLOOD_ABILITY_ACQUISITION, *PTR_BLOOD_ABILITY_ACQUISITION;

// Table 12 from the Birthright Rulebook p.22
BLOOD_ABILITY_ACQUISITION BloodAbilityAcquisitionTable [] = {
	{11, 2, {{ 91, BLOOD_ABILITY_GAINED::NONE},
			 {100, BLOOD_ABILITY_GAINED::MINOR}}},

	{19, 4, {{ 16, BLOOD_ABILITY_GAINED::NONE},
			 { 81, BLOOD_ABILITY_GAINED::MINOR},
			 { 96, BLOOD_ABILITY_GAINED::ENHANCED_TO_MAJOR},
			 {100, BLOOD_ABILITY_GAINED::MAJOR}}},

	{29, 6, {{ 11, BLOOD_ABILITY_GAINED::NONE},
			 { 36, BLOOD_ABILITY_GAINED::MINOR},
			 { 46, BLOOD_ABILITY_GAINED::ENHANCED_TO_MAJOR},
			 { 97, BLOOD_ABILITY_GAINED::MAJOR},
			 { 99, BLOOD_ABILITY_GAINED::ENHANCED_TO_GREAT},
			 {100, BLOOD_ABILITY_GAINED::GREAT}}},

	{36, 4, {{ 51, BLOOD_ABILITY_GAINED::NONE},
			 { 81, BLOOD_ABILITY_GAINED::MINOR},
			 { 91, BLOOD_ABILITY_GAINED::ENHANCED_TO_MAJOR},
			 {100, BLOOD_ABILITY_GAINED::MAJOR}}},

	{51, 6, {{  6, BLOOD_ABILITY_GAINED::NONE},
			 { 16, BLOOD_ABILITY_GAINED::MINOR},
			 { 27, BLOOD_ABILITY_GAINED::ENHANCED_TO_MAJOR},
			 { 41, BLOOD_ABILITY_GAINED::MAJOR},
			 { 51, BLOOD_ABILITY_GAINED::ENHANCED_TO_GREAT},
			 {100, BLOOD_ABILITY_GAINED::GREAT}}},

	{81, 5, {{ 11, BLOOD_ABILITY_GAINED::NONE},
			 { 26, BLOOD_ABILITY_GAINED::ENHANCED_TO_MAJOR},
			 { 41, BLOOD_ABILITY_GAINED::MAJOR},
			 { 71, BLOOD_ABILITY_GAINED::ENHANCED_TO_GREAT},
			 {100, BLOOD_ABILITY_GAINED::GREAT}}},

	{100, 5,{{ 51, BLOOD_ABILITY_GAINED::ENHANCED_TO_GREAT},
			 {100, BLOOD_ABILITY_GAINED::GREAT}}}
};

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
static CHECK_BLOODLINE_ROLL_RESULT CheckBloodlineAbilityRoll(
	BLOOD_ABILITY_INFO::BLOOD_ABILITY/* BloodAbilityRolled */,
	PTR_PLAYER_BLOOD_INFO			/* pPlayerBloodInfo */,
	PTR_BLOODLINE_ACQUISITION_LIST	/* pBloodlineAcquisitionAvail */,
	PTR_BLOOD_ABILITIES_LIST		/* pBloodAbilitiesRefused */);

static void RemoveBloodlineAcquisitionItem(
	PTR_BLOODLINE_ACQUISITION_LIST * /* pTempBloodlineAcquisitionList */,
	PTR_BLOODLINE_ACQUISITION_LIST   /* pBloodlineAcquisitionAvail */);

static void AddBloodlineAbilityAcquisition(
	PTR_BLOODLINE_ACQUISITION_LIST * /* ppBloodAcquisitionList */,
	BLOOD_ABILITY_GAINED::TYPE 		 /* NewStrength */);

static void ModifyBloodlineAbilityAcquisition(
	PTR_BLOODLINE_ACQUISITION_LIST * /* pBloodAcquistionList */,
	BLOOD_ABILITY_GAINED::TYPE 		 /* OldStrength */,
	BLOOD_ABILITY_GAINED::TYPE 		 /* NewStrength */);

#if 0
// [d2-11-97 JPC] Comment out the functions that don't compile.
// Gary said they need work on their linked list stuff.
/* =======================================================================
   Function    - CheckBloodlineAbilityRoll
   Description - Given a Blood Ablity, check to see if
                 1) There is an available slot for it.
				 2) That the character's deviation allows for it.
				 3) That the character hasn't already refused it.
				 4) That the character hasn't already choosen it.
   Returns    CHECK_BLOODLINE_ROLL_RESULT.
   ======================================================================== */
static CHECK_BLOODLINE_ROLL_RESULT CheckBloodlineAbilityRoll(
	BLOOD_ABILITY_INFO::BLOOD_ABILITY BloodAbilityRolled,
	PTR_PLAYER_BLOOD_INFO pPlayerBloodInfo,
	PTR_BLOODLINE_ACQUISITION_LIST pBloodlineAcquisitionAvail,
	PTR_BLOOD_ABILITIES_LIST pBloodAbilitiesRefused)
{
	CHECK_BLOODLINE_ROLL_RESULT sResult = CHECK_BLOODLINE_ROLL_CANT_KEEP;
	PTR_BLOOD_ABILITIES_LIST pTempBloodAbilityList;

	// Check to see if the player has already refused it.
	for (pTempBloodAbilityList = pBloodAbilitiesRefused;
	     pTempBloodAbilityList != 0;
		 pTempBloodAbilityList =
		 pTempBloodAbilityList->next)
	{
		if (pTempBloodAbilityList->BloodAbility == BloodAbilityRolled)
		{
			// I don't like fns with multiple returns but it makes this one
			// lots cleaner and isn't that the point of the rule?
			return sResult;
		}
	}
	

	// Check that the player has the required derivation type
	if (1 == BloodAbilityReqTable[BloodAbilityRolled].DerivationRew[pPlayerBloodInfo->Type])
	{
		PTR_BLOODLINE_ACQUISITION_LIST pTempBloodlineAcquisitionList;

		for (pTempBloodAbilityList = pPlayerBloodInfo->pBloodAblityList;
		     pTempBloodAbilityList != 0;
			 pTempBloodAbilityList = pTempBloodAbilityList->next)
		{
			// Test whether the player has already choosen it.
			if (pTempBloodAbilityList->BloodAbility == BloodAbilityRolled)
			{
				return sResult;
			}
		}

		// Test whether the player has an open slot for it.
		for (pTempBloodlineAcquisitionList = pBloodlineAcquisitionAvail;
		     pTempBloodlineAcquisitionList != 0;
			 pTempBloodlineAcquisitionList = pTempBloodlineAcquisitionList->next)
		{
			// Exact fit first.
			if (1 == BloodAbilityReqTable[BloodAbilityRolled].StrengthReq[pTempBloodlineAcquisitionList->Strength])
			{
				sResult = CHECK_BLOODLINE_ROLL_EXACT_MATCH;
				break;
			}
			// Anything can fit in a Great slot
			else if (pTempBloodlineAcquisitionList->Strength == BLOODLINE_STRENGTH_GREAT)
			{
				sResult = CHECK_BLOODLINE_ROLL_WILL_FIT;
				break;
			}
			// Minor Abilities will fit in a Major slot
			else if (pTempBloodlineAcquisitionList->Strength == BLOODLINE_STRENGTH_MAJOR &&
				     (1 == BloodAbilityReqTable[BloodAbilityRolled].StrengthReq[BLOODLINE_STRENGTH_MINOR]))
			{
				sResult = CHECK_BLOODLINE_ROLL_WILL_FIT;
				break;
			}

		}
	}
	return sResult;
}
/* =======================================================================
   Function    - RejectBloodlineAbility
   Description - Reject the Blood Ability for the current player. Put this
                 Blood Ability on the list of rejected abilities.

				 Note: At the moment I'm not checking that the user didn't just
				       reject the only possible ability left to them. That
					   probably should be done in the Roll fn.
   Returns     - void
   ======================================================================== */
void RejectBloodlineAbility(
	BLOOD_ABILITY_INFO::BLOOD_ABILITY BloodAbility,
	PTR_BLOOD_ABILITIES_LIST *ppBloodAbilitiesRefused)
{
	PTR_BLOOD_ABILITIES_LIST pTempBloodAblitiesRefusedList;
	SHORT				hdl;

	pTempBloodAblitiesRefusedList = *ppBloodAbilitiesRefused;
	
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// GEH fix the structure type to keep the handle, not the pointer
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//*ppBloodAbilitiesRefused = malloc (sizeof(BLOOD_ABILITIES_LIST));
	hdl = NewLockedBlock(sizeof(BLOOD_ABILITIES_LIST));
	*ppBloodAbilitiesRefused = (PTR_BLOOD_ABILITIES_LIST) BLKPTR(hdl);

	(*ppBloodAbilitiesRefused)->next = pTempBloodAblitiesRefusedList;
	(*ppBloodAbilitiesRefused)->BloodAbility = BloodAbility;
}

/* =======================================================================
   Function    - AddBloodlineAbility
   Description - Just add the Blood Ability to the current player info.
                 To be used for Non Player Characters and testing of Blood
				 Abilites.
   Returns     - void
   ======================================================================== */
void AddBloodlineAbility(
	BLOOD_ABILITY_INFO::BLOOD_ABILITY BloodAbility,
	PTR_PLAYER_BLOOD_INFO pPlayerBloodInfo)
{
	SHORT	hdl = NewLockedBlock(sizeof(BLOOD_ABILITIES_LIST));
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// GEH fix the structure type to keep the handle, not the pointer
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//PTR_BLOOD_ABILITIES_LIST pNewBloodAbility = malloc (sizeof(BLOOD_ABILITIES_LIST));
	PTR_BLOOD_ABILITIES_LIST pNewBloodAbility = (PTR_BLOOD_ABILITIES_LIST) BLKPTR(hdl);

	if (pNewBloodAbility)
	{
		// Because its easy I'm sticking it on the front of the list.
		pNewBloodAbility->next = pPlayerBloodInfo->pBloodAblityList;
		pPlayerBloodInfo->pBloodAblityList = pNewBloodAbility;
	}
}

/* =======================================================================
   Function    - RemoveBloodlineAcquisitionItem
   Description - Remove this item from the list, and reconnect.
   Returns     - void
   ======================================================================== */
static void RemoveBloodlineAcquisitionItem(
   PTR_BLOODLINE_ACQUISITION_LIST *pTempBloodlineAcquisitionList,
   PTR_BLOODLINE_ACQUISITION_LIST pBloodlineAcquisitionAvail
)
{
	if (pBloodlineAcquisitionAvail == *pTempBloodlineAcquisitionList) // First item
	{
		*pTempBloodlineAcquisitionList = pBloodlineAcquisitionAvail->next;
		(*pTempBloodlineAcquisitionList)->prev = 0;
	}
	else // Middle item removed
	{
		pBloodlineAcquisitionAvail->prev->next = pBloodlineAcquisitionAvail->next;

		// if not the Last item on the list
        if (pBloodlineAcquisitionAvail->next)
		{
			pBloodlineAcquisitionAvail->next->prev = pBloodlineAcquisitionAvail->prev;
		}
	}
	
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// GEH fix the structure type to keep the handle, not the pointer
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//free (pBloodlineAcquisitionAvail);
	ClrLock ((SHORT) pBloodlineAcquisitionAvail);
	SetPurge ((SHORT) pBloodlineAcquisitionAvail);

}

/* =======================================================================
   Function    - KeepBloodlineAbilityRoll
   Description - Keep a  Bloodline Ability.
                 Put it in the best possible slot.
   Returns     - void
   ======================================================================== */
void KeepBloodlineAbilityRoll(
	BLOOD_ABILITY_INFO::BLOOD_ABILITY BloodAbilityRolled,
	PTR_PLAYER_BLOOD_INFO pPlayerBloodInfo,
	PTR_BLOODLINE_ACQUISITION_LIST *pBloodlineAcquisitionAvail)
{
	PTR_BLOODLINE_ACQUISITION_LIST pTempBloodlineAcquisitionList;
	SHORT sSlotRemoved = 0;

	// First try to find the exact fit slot.
	for (pTempBloodlineAcquisitionList = *pBloodlineAcquisitionAvail;
	     pTempBloodlineAcquisitionList;
		 pTempBloodlineAcquisitionList = pTempBloodlineAcquisitionList->next)
	{
		// Exact match found.
		if (1 == BloodAbilityReqTable[BloodAbilityRolled].StrengthReq[pTempBloodlineAcquisitionList->Strength])
		{
			RemoveBloodlineAcquisitionItem(pBloodlineAcquisitionAvail,pTempBloodlineAcquisitionList);
			sSlotRemoved = 1;
			break;
		}
	}

	// If we didn't match it up try putting a Minor Ability in a Major slot
	if (sSlotRemoved == 0 && 1 == BloodAbilityReqTable[BloodAbilityRolled].StrengthReq[BLOODLINE_STRENGTH_MINOR])
	{
		for (pTempBloodlineAcquisitionList = *pBloodlineAcquisitionAvail;
			pTempBloodlineAcquisitionList;
			pTempBloodlineAcquisitionList = pTempBloodlineAcquisitionList->next)
		{	
			// Try putting a Minor in a Major slot
			if (pTempBloodlineAcquisitionList->Strength == BLOODLINE_STRENGTH_MAJOR)
			{
				RemoveBloodlineAcquisitionItem(pBloodlineAcquisitionAvail,pTempBloodlineAcquisitionList);
				sSlotRemoved = 1;
				break;
			}
		}
	}

	// If that didn't work try putting a Minor Ability in a Great slot or a Major in a Great
	if (sSlotRemoved == 0)
	{
		for (pTempBloodlineAcquisitionList = *pBloodlineAcquisitionAvail;
			pTempBloodlineAcquisitionList;
			pTempBloodlineAcquisitionList = pTempBloodlineAcquisitionList->next)
		{	
			// Find the Great slot
			if (pTempBloodlineAcquisitionList->Strength == BLOODLINE_STRENGTH_GREAT)
			{
				RemoveBloodlineAcquisitionItem(pBloodlineAcquisitionAvail,pTempBloodlineAcquisitionList);
				sSlotRemoved = 1;
				break;
			}
		}
	}

	// Don't add unless allowed to.
	if (sSlotRemoved)
	{
		AddBloodlineAbility(BloodAbilityRolled, pPlayerBloodInfo);
	}
}

/* =======================================================================
   Function    - RollBloodlineAbility
   Description - Roll a  Bloodline Ability.
                 Taken from Birthright Rulebook p. 22
				 If there is no choice, add it to the user's list of Abilities.
   Returns     - Bloodline Ability,
                 We are done if the pBloodlineAcquisitionAvail is Zero
				 Otherwise Querry the user and Accept or reject it, then
				 call this routine again.
   ======================================================================== */
BLOOD_ABILITY_INFO::BLOOD_ABILITY RollBloodlineAbility(
	PTR_PLAYER_BLOOD_INFO			pPlayerBloodInfo,
	PTR_BLOODLINE_ACQUISITION_LIST	*ppBloodlineAcquisitionAvail,
	PTR_BLOOD_ABILITIES_LIST		pBloodAbilitiesRefused)
{
	DICE dice = {1, 100, 0};
	CHECK_BLOODLINE_ROLL_RESULT CheckResult = CHECK_BLOODLINE_ROLL_CANT_KEEP;
	BLOOD_ABILITY BloodAbilityResult;

	while (*ppBloodlineAcquisitionAvail || CheckResult != CHECK_BLOODLINE_ROLL_WILL_FIT)
	{
		do
		{
			SHORT sRollResult = RollDice(&dice);

			if (sRollResult == 00)
			{
				BloodAbilityResult = BLOOD_ABILITY_UNREADABLE_THOUGHTS;
			}
			else if (sRollResult < 4)
			{
				BloodAbilityResult = BLOOD_ABILITY_ALERTNESS;
			}
			else if (sRollResult < 6)
			{
				BloodAbilityResult = BLOOD_ABILITY_ALTER_APPEARANCE;
			}
			else if (sRollResult < 15)
			{
				BloodAbilityResult = BLOOD_ABILITY_ANIMAL_AFFINITY;
			}
			else if (sRollResult < 20)
			{
				BloodAbilityResult = BLOOD_ABILITY_BATTLEWISE;
			}
			else if (sRollResult < 23)
			{
				BloodAbilityResult = BLOOD_ABILITY_BLOOD_HISTORY;
			}
			else if (sRollResult < 28)
			{
				BloodAbilityResult = BLOOD_ABILITY_BLOODMARK;
			}
			else if (sRollResult < 30)
			{
				BloodAbilityResult = BLOOD_ABILITY_CHARACTER_READING;
			}
			else if (sRollResult < 35)
			{
				BloodAbilityResult = BLOOD_ABILITY_COURAGE;
			}
			else if (sRollResult < 37)
			{
				BloodAbilityResult = BLOOD_ABILITY_DETECT_LIE;
			}
			else if (sRollResult < 39)
			{
				BloodAbilityResult = BLOOD_ABILITY_DETECT_ILLUSION;
			}
			else if (sRollResult < 41)
			{
				BloodAbilityResult = BLOOD_ABILITY_DIRECTION_SENSE;
			}
			else if (sRollResult < 44)
			{
				BloodAbilityResult = BLOOD_ABILITY_DIVINE_AURA;
			}
			else if (sRollResult < 48)
			{
				BloodAbilityResult = BLOOD_ABILITY_DEVINE_WRATH;
			}
			else if (sRollResult < 49)
			{
				BloodAbilityResult = BLOOD_ABILITY_ELEMENTAL_CONTROL;
			}
			else if (sRollResult < 59)
			{
				BloodAbilityResult = BLOOD_ABILITY_ENHANCED_SENSE;
			}
			else if (sRollResult < 60)
			{
				BloodAbilityResult = BLOOD_ABILITY_FEAR;
			}
			else if (sRollResult < 66)
			{
				BloodAbilityResult = BLOOD_ABILITY_HEALING;
			}
			else if (sRollResult < 76)
			{
				BloodAbilityResult = BLOOD_ABILITY_HEIGHTENED_ABILITY;
			}
			else if (sRollResult < 80)
			{
				BloodAbilityResult = BLOOD_ABILITY_IRON_WILL;
			}
			else if (sRollResult < 82)
			{
				BloodAbilityResult = BLOOD_ABILITY_PERSUASION;
			}
			else if (sRollResult < 84)
			{
				BloodAbilityResult = BLOOD_ABILITY_POISON_SENSE;
			}
			else if (sRollResult < 86)
			{
				BloodAbilityResult = BLOOD_ABILITY_PROTECTION_FROM_EVIL;
			}
			else if (sRollResult < 87)
			{
				BloodAbilityResult = BLOOD_ABILITY_REGENERATION;
			}
			else if (sRollResult < 92)
			{
				BloodAbilityResult = BLOOD_ABILITY_RESISTANCE;
			}
			else if (sRollResult < 93)
			{
				BloodAbilityResult = BLOOD_ABILITY_SHADOW_FORM;
			}
			else if (sRollResult < 94)
			{
				BloodAbilityResult = BLOOD_ABILITY_TOUCH_OF_DECAY;
			}
			else if (sRollResult < 100)
			{
				BloodAbilityResult = BLOOD_ABILITY_TRAVEL;
			}

			CheckResult = CheckBloodlineAbilityRoll(BloodAbilityResult,
													pPlayerBloodInfo,
													*ppBloodlineAcquisitionAvail,
													pBloodAbilitiesRefused);
		}
		while (CheckResult == CHECK_BLOODLINE_ROLL_CANT_KEEP);

		// The user has no choice, we have to keep it so we will!
		if (CheckResult == CHECK_BLOODLINE_ROLL_EXACT_MATCH)
		{
			KeepBloodlineAbilityRoll(BloodAbilityResult,
				                     pPlayerBloodInfo,
									 ppBloodlineAcquisitionAvail);
		}
	}

	return BloodAbilityResult;
}

/* =======================================================================
   Function    - AddBloodlineAbilityAcquisition
   Description - Add a new Acquisition list item.
   Returns     - void
   ======================================================================== */
static void AddBloodlineAbilityAcquisition(
	PTR_BLOODLINE_ACQUISITION_LIST *ppBloodAcquisitionList,
	BLOOD_ABILITY_GAINED::TYPE NewStrength)
{
		PTR_BLOODLINE_ACQUISITION_LIST pPreviousHeadList;

		pPreviousHeadList = *ppBloodAcquisitionList;
		*ppBloodAcquisitionList = (PTR_BLOODLINE_ACQUISITION_LIST) malloc (sizeof(BLOODLINE_ACQUISITION_LIST));
		
		// GWP switch to the memory manager.

		if (*ppBloodAcquisitionList)
		{
			(*ppBloodAcquisitionList)->prev = 0;
			(*ppBloodAcquisitionList)->next = pPreviousHeadList;

			(*ppBloodAcquisitionList)->Strength = (BLOODLINE_STRENGTH_TYPE) NewStrength;
		}
}

/* =======================================================================
   Function    - ModifyBloodlineAbilityAcquisition
   Description - Modify an existing Ability or add a new Acquisition list item.
                 if none are found.
   Returns     - void
   ======================================================================== */
static void ModifyBloodlineAbilityAcquisition(
	PTR_BLOODLINE_ACQUISITION_LIST *pBloodAcquistionList,
	BLOOD_ABILITY_GAINED::TYPE OldStrength,
	BLOOD_ABILITY_GAINED::TYPE NewStrength)
{
	PTR_BLOODLINE_ACQUISITION_LIST pTempBloodAcquisitionList;

	for (pTempBloodAcquisitionList = *pBloodAcquistionList;
		 pTempBloodAcquisitionList;
		 pTempBloodAcquisitionList = pTempBloodAcquisitionList->next)
	{
		if (pTempBloodAcquisitionList->Strength == OldStrength)
		{
			pTempBloodAcquisitionList->Strength = (BLOODLINE_STRENGTH_INFO::TYPE) NewStrength;
			return;		// I think its clearer to just return here than setting yet another flag.
		}
	}

	// If you get here you must not have found an acceptable match. So give the guy
	// one of the new abilities.
	AddBloodlineAbilityAcquisition(pBloodAcquistionList,NewStrength);
}

/* =======================================================================
   Function    - RollBloodlineAbilityAcquisition
   Description - Generate a list of Bloodline Abilities for a given player.
                 Taken from Birthright Rulebook p. 20
				 This routine generates available slots to be filled later
				 with Blood Abilities.
   Returns     - Bloodline Ability list. (or 0 if none were allocated.)
   ======================================================================== */
PTR_BLOODLINE_ACQUISITION_LIST RollBloodlineAbilityAcquisition(
	SBYTE	Strength)
{
	SHORT i;
	PTR_BLOODLINE_ACQUISITION_LIST pBloodAcquistionList = 0;

	for (i = 0;
	     i < (sizeof(BloodAbilityAcquisitionTable)/sizeof(BLOOD_ABILITY_ACQUISITION));
		 i++)
	{
		if (Strength < BloodAbilityAcquisitionTable[i].sBloodStrengthHighValue)
		{
			const DICE dice = { 1, 100, 0};
			SHORT sRollResult = RollDice(&dice);
            SHORT j;

			for (j = BloodAbilityAcquisitionTable[i].sNumberOfGainedValues;
			     j >= 0;
				 j--)
			{
				if (sRollResult < BloodAbilityAcquisitionTable[i].BloodAbilityGain[j].sHighLimit)
				{
					switch (BloodAbilityAcquisitionTable[i].BloodAbilityGain[j].BloodAbilityGain)
					{
					case BLOOD_ABILITY_GAINED::MINOR:
						// Add a Minor Ability
						AddBloodlineAbilityAcquisition(&pBloodAcquistionList,BLOOD_ABILITY_GAINED_MINOR);
						break;
					case BLOOD_ABILITY_GAINED::MAJOR:
						// Add a Major Ability
						AddBloodlineAbilityAcquisition(&pBloodAcquistionList,BLOOD_ABILITY_GAINED_MAJOR);
						break;
					case BLOOD_ABILITY_GAINED::GREAT:
						// Add a Great Ability
						AddBloodlineAbilityAcquisition(&pBloodAcquistionList,BLOOD_ABILITY_GAINED_GREAT);
						break;
					case BLOOD_ABILITY_GAINED::ENHANCED_TO_MAJOR:
						// Upgrade a Minor Ability to a Major one or add a Major Ability
						ModifyBloodlineAbilityAcquisition(&pBloodAcquistionList, BLOOD_ABILITY_GAINED_MINOR, BLOOD_ABILITY_GAINED_MAJOR);
						break;
					case BLOOD_ABILITY_GAINED::ENHANCED_TO_GREAT:
						// Upgrade a Major Ability to a Great one or add a Great Ability
						ModifyBloodlineAbilityAcquisition(&pBloodAcquistionList, BLOOD_ABILITY_GAINED_MAJOR, BLOOD_ABILITY_GAINED_GREAT);
						break;
					}
					break;
				}
			}
		}
	}
	return pBloodAcquistionList;
}
#endif

/* =======================================================================
   Function    - RollBloodlineDerivation
   Description - Generate a Bloodline Derivation value for a given player.
                 Taken from Birthright Rulebook p. 21
				 Note: Players may select attribute this vs rolling for it.
   Returns     - Bloodline Derivation
   ======================================================================== */
BLOODLINE_DERIVATION_INFO::TYPE RollBloodlineDerivation()
{
	const DICE	dice = {1, 100, 0};
	const SHORT	sRollResult = RollDice(&dice);
	BLOODLINE_DERIVATION_INFO::TYPE BloodDerivation;

	if (sRollResult == 0 || sRollResult > 90)
	{
		BloodDerivation = BLOODLINE_DERIVATION_INFO::AZRAI;
	}
	else if (sRollResult < 21 )
	{
		BloodDerivation = BLOODLINE_DERIVATION_INFO::ANDUIRAS;
	}
	else if (sRollResult < 36)
	{
		BloodDerivation = BLOODLINE_DERIVATION_INFO::REYNIR;
	}
	else if (sRollResult < 53)	
	{
		BloodDerivation = BLOODLINE_DERIVATION_INFO::BRENNA;
	}
	else if (sRollResult < 71)	
	{
		BloodDerivation = BLOODLINE_DERIVATION_INFO::BASAIA;
	}
	else if (sRollResult < 81)	
	{
		BloodDerivation = BLOODLINE_DERIVATION_INFO::MASELA;
	}
	else if (sRollResult < 91)
	{
		BloodDerivation = BLOODLINE_DERIVATION_INFO::VORYNN;
	}

	return BloodDerivation;
}

/* =======================================================================
   Function    - RollBloodlineStrength
   Description - Generate a Bloodline Strength value for a given player.
                 Taken from Birthright Rulebook p. 20
   Returns     - Bloodline Strength Type
   ======================================================================== */
BLOODLINE_STRENGTH_INFO::TYPE	RollBloodlineStrength(SHORT *psStrength)
{
	DICE dice = {1, 100, 0};
	BLOODLINE_STRENGTH_INFO::TYPE BloodStrengthType;

	*psStrength = RollDice(&dice);

	if (*psStrength == 0 || *psStrength > 95)
	{
		BloodStrengthType = BLOODLINE_STRENGTH_INFO::GREAT;
	}
	else if ( *psStrength < 26)
	{
		BloodStrengthType = BLOODLINE_STRENGTH_INFO::TAINTED;
	}
	else if ( *psStrength < 66)
	{
		BloodStrengthType = BLOODLINE_STRENGTH_INFO::MINOR;
	}
	else if ( *psStrength < 96)
	{
		BloodStrengthType = BLOODLINE_STRENGTH_INFO::MAJOR;
	}

	return BloodStrengthType;
}

/* =======================================================================
   Function    - RollBloodlineNumericalStrength
   Description - Generate numerical (as opposed to descriptive) stength.
                 Taken from Michael Ormsby's notes on Modifying Birthright I
					  into Birthright II.
   Returns     - Numerical strength.
	Notes			- Call this AFTER calling RollBloodlineStrength.
					  [d5-12-97 JPC]
   ======================================================================== */
LONG RollBloodlineNumericalStrength (BLOODLINE_STRENGTH_INFO::TYPE BloodStrengthType)
{
	DICE dice = {4, 6, 0};

	switch (BloodStrengthType)
	{
		case BLOODLINE_STRENGTH_INFO::TAINTED:
			dice.sbNumberOfDice = 4;
			break;
		case BLOODLINE_STRENGTH_INFO::MINOR:
			dice.sbNumberOfDice = 5;
			break;
		case BLOODLINE_STRENGTH_INFO::MAJOR:
		case BLOODLINE_STRENGTH_INFO::GREAT:
			dice.sbNumberOfDice = 8;
			break;
	}

	return RollDice (&dice);
}



