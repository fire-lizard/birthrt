/* ========================================================================
   Copyright (c) 1990,1997	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: playhit.cpp  - class to manipulate player stats for combat.
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "SYSTEM.H"
#include "PLAYHIT.HXX"

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
extern void GainExp(PLAYER_STATS * pPS, const LONG ExpAmount,
				const BOOL CanGainLevelNow);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Local Variables
   ------------------------------------------------------------------------ */

/* ========================================================================
   Function    - mfInflictDamage
   Description - After you have succeeded in your attack roll, now roll 
   				 for damage.
   Returns     - Damage value
   ======================================================================== */
LONG const PLAYER_HIT::mfInflictDamageOnTarget( THINGTYPE const type)
{
	LONG Damage;
	
	if (fpTargetStats->HitPts <= 0)
	{
		// Nothing happens to dead avatars.
		return 0;
	}
	
	if (fpTargetStats->StatsMod.mfGetMod(STATSMOD::DAMAGE_RESIST_MOD))
	{
		// The enemy has damage resistance.
		return 0;
	}
	
	// If you have damage dice use them.
	if (fpHeroStats->DamageDice[0].sbNumberOfDice > 0)
	{
		DICE TempDice1 = fpHeroStats->DamageDice[0];
		DICE TempDice2 = fpHeroStats->DamageDice[1];
		
		TempDice1.sbModifier += fpHeroStats->StatsMod.mfGetMod(STATSMOD::DAMAGEROLL_MOD);
		TempDice2.sbModifier += fpHeroStats->StatsMod.mfGetMod(STATSMOD::DAMAGEROLL_MOD);
		
		Damage = fpHeroStats->mfRollDice(&TempDice1) + fpHeroStats->mfRollDice(&TempDice2)
				 + fpHeroStats->StatsMod.mfGetMod(STATSMOD::WEAPONDAMAGE_MOD)
				 ;
	}
	else
	{
		// Otherwise the damage you do depends on the weapon you hold.
		DICE const * const pDamageDice = 
			(GAME_TTYPE::mfIsLargeAvatar(type)) ?
				WEAPON_INFO::mfGetLargeCreatureDamage(fpHeroStats->mfGetFirstWeapon())
				:
				WEAPON_INFO::mfGetSmallCreatureDamage(fpHeroStats->mfGetFirstWeapon()) ;
			
		DICE TempDice = *pDamageDice;
		TempDice.sbModifier += fpHeroStats->StatsMod.mfGetMod(STATSMOD::DAMAGEROLL_MOD);
		
		Damage = fpHeroStats->mfRollDice(&TempDice) 
					+ STRENGTH::mfDamageAdjust(fpHeroStats->mfGetStr())
					+ fpHeroStats->StatsMod.mfGetMod(STATSMOD::WEAPONDAMAGE_MOD)
					;
	}
	
	if (Damage > 0)
	{
		fpTargetStats->HitPts -= Damage;
	
		if (fpTargetStats->HitPts <= 0)
		{
			fpTargetStats->HitPts = 0;
			GainExp(fpHeroStats, fpTargetStats->XP, TRUE);
		}
	}
	else
	{
		Damage = 0;
	}
	
	return Damage;
}

/* ========================================================================
   Function    - mfInflictMagicDamage
   Description - Call this to try and damage your enemy.
   Returns     - Damage value
   ======================================================================== */
LONG const PLAYER_HIT::mfInflictMagicDamageOnTarget(
	THINGTYPE const NewKillType, 
	LONG const Damage)
{
	if (fpTargetStats->HitPts <= 0)
	{
		// Nothing happens to dead avatars.
		return 0;
	}
	
	if (fpTargetStats->StatsMod.mfGetMod(STATSMOD::DAMAGE_RESIST_MOD))
	{
		// The enemy has damage resistance.
		return 0;
	}
	
	LONG result = Damage;
	SHORT const Bonus = (SHORT)fpTargetStats->StatsMod.mfGetMod(STATSMOD::MAGIC_RESIST_MOD);
	
	// first general magic resistance
	if( ThingIsMagic(NewKillType) && Bonus)
	{
		// check percentage
		if(random(100) < Bonus)
			result = 0;
	}
	else
	if( ThingIsHeatSource (NewKillType)
		&& fpTargetStats->StatsMod.mfGetMod(STATSMOD::FIRE_RESIST_MOD)
		)	
		result = 0;
	else
	if( ThingIsCold (NewKillType)
		&& fpTargetStats->StatsMod.mfGetMod(STATSMOD::COLD_RESIST_MOD)
		)	
		result = 0;
	else
	if( ThingIsEvil (NewKillType)
		&& fpTargetStats->StatsMod.mfGetMod(STATSMOD::EVIL_RESIST_MOD)
		)	
		result = 0;
	
	fpTargetStats->HitPts -= result;
	if (fpTargetStats->HitPts < 0)
	{
		fpTargetStats->HitPts = 0;
		GainExp(fpHeroStats, fpTargetStats->XP, TRUE);
	}
	
	return result;
}

/* ========================================================================
   Function    - mfDamageRange
   Description - Calculate the minium and max values of damage you can do.
   Returns     - 
   ======================================================================== */

void PLAYER_HIT::mfDamageRange( THINGTYPE const Enemytype,
					   LONG * const minDamage,
	                   LONG * const maxDamage) const
{
	if (fpTargetStats->StatsMod.mfGetMod(STATSMOD::DAMAGE_RESIST_MOD))
	{
		// The enemy has damage resistance.
		*minDamage = 0;
		*maxDamage = 0;
		return;
	}
	
	// If you have damage dice use them.
	if (fpHeroStats->DamageDice[0].sbNumberOfDice > 0)
	{
		DICE TempDice1 = fpHeroStats->DamageDice[0];
		DICE TempDice2 = fpHeroStats->DamageDice[1];
		
		TempDice1.sbModifier += fpHeroStats->StatsMod.mfGetMod(STATSMOD::DAMAGEROLL_MOD);
		TempDice2.sbModifier += fpHeroStats->StatsMod.mfGetMod(STATSMOD::DAMAGEROLL_MOD);
		
		*minDamage = TempDice1.sbNumberOfDice + TempDice1.sbModifier
		         + TempDice2.sbNumberOfDice + TempDice2.sbModifier
				 + fpHeroStats->StatsMod.mfGetMod(STATSMOD::WEAPONDAMAGE_MOD)
				 ;
		
		*maxDamage = (TempDice1.sbNumberOfDice * TempDice1.sbNumberOfSides) + TempDice1.sbModifier
		         + (TempDice2.sbNumberOfDice* TempDice2.sbNumberOfSides)  + TempDice2.sbModifier
				 + fpHeroStats->StatsMod.mfGetMod(STATSMOD::WEAPONDAMAGE_MOD)
				 ;
	}
	else
	{
		// Otherwise the damage you do depends on the weapon you hold.
		DICE const * const pDamageDice = 
			(GAME_TTYPE::mfIsLargeAvatar(Enemytype)) ?
				WEAPON_INFO::mfGetLargeCreatureDamage(fpHeroStats->mfGetFirstWeapon())
				:
				WEAPON_INFO::mfGetSmallCreatureDamage(fpHeroStats->mfGetFirstWeapon()) ;
			
		DICE TempDice = *pDamageDice;
		TempDice.sbModifier += fpHeroStats->StatsMod.mfGetMod(STATSMOD::DAMAGEROLL_MOD);
		
		*minDamage = TempDice.sbNumberOfDice + TempDice.sbModifier
					+ STRENGTH::mfDamageAdjust(fpHeroStats->mfGetStr())
					+ fpHeroStats->StatsMod.mfGetMod(STATSMOD::WEAPONDAMAGE_MOD)
					;
		*maxDamage = (TempDice.sbNumberOfDice*TempDice.sbNumberOfSides) + TempDice.sbModifier
					+ STRENGTH::mfDamageAdjust(fpHeroStats->mfGetStr())
					+ fpHeroStats->StatsMod.mfGetMod(STATSMOD::WEAPONDAMAGE_MOD)
					;
	}
	
	if (*minDamage < 0)
	{
		*minDamage = 0;
	}
	
	if (*maxDamage < 0)
	{
		*maxDamage = 0;
	}
}
