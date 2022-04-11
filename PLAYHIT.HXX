/* ========================================================================
   Copyright (c) 1990,1997	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _PLAYHIT_HXX
#define _PLAYHIT_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_PLAYSTAT_HXX)
#include "playstat.hxx"
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
/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Class Definition
   ------------------------------------------------------------------------ */
// This is a functional class to operate on player stat data in combat.
class PLAYER_HIT {
public:
	inline PLAYER_HIT( PLAYER_STATS * const pHeroStats,
			            PLAYER_STATS * const pTargetStats);
			            
	inline LONG const mfToHitTarget() const;
	inline LONG const mfMinimumToHit(LONG const /* AttackRollModifier */) const;
	inline BOOL const mfRollAttackTarget( LONG const /* AttackRollModifier */,
								SBYTE * const /* pRollResult */ ) const;
	
	LONG const mfInflictDamageOnTarget( THINGTYPE const /* type */);
   	LONG const mfInflictMagicDamageOnTarget( THINGTYPE const /* KillType */,
						LONG const /* Damage */ );
	
	void mfDamageRange( THINGTYPE const /* type */,
	                   LONG * const /* minDamage */,
	                   LONG * const /* maxDamage */) const;
protected:
private:
	PLAYER_STATS * const  fpHeroStats;
	PLAYER_STATS * const  fpTargetStats;
};

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Inline functions   
   ------------------------------------------------------------------------ */
   
// Constructor
inline PLAYER_HIT::PLAYER_HIT( PLAYER_STATS * const pHeroStats,
		            PLAYER_STATS * const pTargetStats)
		            : fpHeroStats(pHeroStats),
		              fpTargetStats(pTargetStats)
{
}

// Get the number on die roll necessary to score a hit on this character.
inline LONG const PLAYER_HIT::mfToHitTarget() const
{
	LONG const Result =  fpHeroStats->mfGetTHAC0() 
			- STRENGTH::mfHitAdjust(fpHeroStats->mfGetStr())
			- fpTargetStats->mfGetArmorClass()
			+ ARMOR_INFO::mfGetWeaponAdjust(fpTargetStats->mfGetCurrentArmor(),
											 WEAPON_INFO::mfGetStyle(fpHeroStats->mfGetFirstWeapon()))
			- fpHeroStats->StatsMod.mfGetMod(STATSMOD::WEAPONHIT_MOD)
			;
			
	return Result;
}

inline LONG const PLAYER_HIT::mfMinimumToHit(LONG const AttackRollModifier) const
{
	LONG ToHit = mfToHitTarget() - AttackRollModifier;
	if (ToHit < 2)
		ToHit = 2;
	
	return ToHit;
}

// Roll the standard AD&D attack dice.
inline BOOL const PLAYER_HIT::mfRollAttackTarget(
	LONG const AttackRollModifier,
	SBYTE * const pRollResult ) const
{
	DICE const AttackDie = {1, 20, 0};
	const SHORT AttackRoll = fpHeroStats->mfRollDice(&AttackDie);
	// 1 is always a miss,
	// 20 is always a hit.
	*pRollResult = (SBYTE) AttackRoll;
	
	return (( (AttackRoll > 1 && AttackRoll >= mfMinimumToHit(AttackRollModifier)
	         ) 
	        || AttackRoll == 20) ?
			TRUE : FALSE);
}

#endif // _PLAYHIT_HXX
