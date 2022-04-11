/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: _BLOODREQ.HXX - Requirements for Blood Abilities.
   Author: 	 Gary Powell
   ======================================================================== */
#ifndef _BLOODREQ_HXX
#define _BLOODREQ_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_BLOOD_HXX)
#include "blood.hxx"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
// HUNGARIAN NOTE: All values are of type long unless specified.  So if
// is doesn't start with a capital letter, the lower case should be a
// prefix.  'u' means Ulong.

// Note: This is a temporary structure used to hold the Bloodline information
//       unitl the player has choosen which abilites  to fill his list with.
class BLOODLINE_ACQUISITION_LIST {
public:
	// GWP These need to be handles not pointers.
	BLOODLINE_ACQUISITION_LIST *next;
	BLOODLINE_ACQUISITION_LIST *prev;
	BLOODLINE_STRENGTH_INFO::TYPE	   Strength;
};
typedef BLOODLINE_ACQUISITION_LIST *PTR_BLOODLINE_ACQUISITION_LIST;


void KeepBloodlineAbilityRoll(
	BLOOD_ABILITY_INFO::BLOOD_ABILITY BloodAbilityRolled,
	PTR_PLAYER_BLOOD_INFO pPlayerBloodInfo,
	PTR_BLOODLINE_ACQUISITION_LIST *pBloodlineAcquisitionAvail);

void AddBloodlineAbility(
	BLOOD_ABILITY_INFO::BLOOD_ABILITY BloodAbility,
	PTR_PLAYER_BLOOD_INFO pPlayerBloodInfo);

BLOOD_ABILITY_INFO::BLOOD_ABILITY RollBloodlineAbility(
	PTR_PLAYER_BLOOD_INFO			pPlayerBloodInfo,
	PTR_BLOODLINE_ACQUISITION_LIST	*ppBloodlineAcquisitionAvail,
	PTR_BLOOD_ABILITIES_LIST		pBloodAbilitiesRefused);

PTR_BLOODLINE_ACQUISITION_LIST RollBloodlineAbilityAcquisition(
	SBYTE	Strength);

BLOODLINE_STRENGTH_INFO::TYPE	RollBloodlineStrength(SHORT *psStrength);

BLOODLINE_DERIVATION_INFO::TYPE RollBloodlineDerivation();

LONG RollBloodlineNumericalStrength (BLOODLINE_STRENGTH_INFO::TYPE BloodStrengthType);

#endif // _BLOODREQ_HXX
