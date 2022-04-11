/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: STATSMOD.HXX - modifying values for an avatars stats
   Author: 	 Greg Hightower
   ======================================================================== */
#ifndef _STATSMOD_HXX
#define _STATSMOD_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
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
class STATSMOD {
public:
	typedef enum {
		STRENGTH_SET = 0,			// attributes sets
		DEXTERITY_SET,
		CONSTITUTION_SET,
		INTELLIGENCE_SET,
		WISDOM_SET,
		CHARISMA_SET,
		HITPOINTS_SET,
		ARMORCLASS_SET,
		
		LEVEL_SET,					// environment sets
		
		MAGIC_LEVEL_IMMUNE_SET,		//for globes of invuln.
		
		// These are the stats sets,...Don't add more!
		STATSSET_MAX_NUMBER
	}SET_TYPE;
		
	typedef enum {
		STRENGTH_MOD = 0,			// attributes mods
		DEXTERITY_MOD,
		CONSTITUTION_MOD,
		INTELLIGENCE_MOD,
		WISDOM_MOD,
		CHARISMA_MOD,
		HITPOINTS_MOD,
		ARMORCLASS_MOD,
		
		FIRE_RESIST_MOD,			// resistance mods
		COLD_RESIST_MOD,
		EVIL_RESIST_MOD,			// like turn undead
		MAGIC_RESIST_MOD,
		FEAR_RESIST_MOD,
		DAMAGE_RESIST_MOD,			// resistance to all damage
		POISON_RESIST_MOD,			
		
		LEVEL_MOD,					// character level mods
		LEVEL_MOD_WARRIOR,
		LEVEL_MOD_WIZARD,
		LEVEL_MOD_PRIEST,
		LEVEL_MOD_ROGUE,
		
		SAVES_MOD,					// environment mods
		REGENERATE_MOD,
		SPEED_MOD,
		DIEROLL_MOD,
		
		WEAPONHIT_MOD,				// weapon mods
		WEAPONDAMAGE_MOD,
		DAMAGEROLL_MOD,
		
		SPELLMULT_MOD,				// you get (1+this) times normal spells
		
// ------------------ realm specific stuff ---------------------		
		BLOODLINE_MOD,				// BLStrength += this*5 garnering regency
		PROSPERITY_MOD,				// auto Bless/Blight on your provinces
		TRANSPORT_MOD,				// cast Transport realm spell wo/source
		MONEYMULT_MOD,				// treasury *= (100+this)/100.0 per turn
		
		FREEACTION_MOD,				// base is 3 actions per domain turn
		FREEFORGE_MOD,				// free Forge Ley Lines per turn
		FREEMAGIC_MOD,				// free Realm Spell actions per turn
		
		AGITATE_MOD,				// modifier to agitate actions
		CONTEST_MOD,				// modifier to contest actions
		INVESTITURE_MOD,			// modifier to investiture actions
		
		BUILDCOST_MOD,				// percentage change in build cost
		FORTCOST_MOD, 				// percentage change in fortify cost
		MOVECOST_MOD,				// delta .1GB cost per troop move
		
		SEEHIDDEN_MOD,				// see hidden troops if > 0
		
// ------------------ battle specific stuff ---------------------		
		BTL_MORALE_MOD,				// battle field mods
		BTL_ROUTE_MOD,
		BTL_DEAD_MOD,
		BTL_ATTACK_MOD,
		BTL_DEFEND_MOD,
		
		// These are the stats mods,...Don't add more!
		STATSMOD_MAX_NUMBER
	} MOD_TYPE;
	
	inline const void  mfClearStatMods(void) ;
	
	inline const SBYTE mfModifyStats( MOD_TYPE const, SBYTE const );
	
	inline const SBYTE mfSetStats( SET_TYPE const, SBYTE const );
	
	inline const SBYTE mfGetMod( MOD_TYPE const ) const;
	
	inline const SBYTE mfGetSet( SET_TYPE const ) const;

protected:
private:
	SBYTE bStatsMods[STATSMOD_MAX_NUMBER]; 
	SBYTE bStatsSets[STATSSET_MAX_NUMBER]; 
};
typedef STATSMOD *PTR_STATSMOD;

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

/* =======================================================================
   Function    - mfClearStatMods
   Description - clears both tables
   Returns     - nothing
   ======================================================================== */
inline const void STATSMOD::mfClearStatMods(void)
{
	SHORT i;
	
	for (i=0; i < STATSMOD_MAX_NUMBER; i++)
		bStatsMods[i] = 0; 
		
	for (i=0; i < STATSSET_MAX_NUMBER; i++)
		bStatsSets[i] = 0; 
}
   
/* =======================================================================
   Function    - mfSetStatsMod
   Description - adds/subs an amount from the current modify value
   Returns     - old value
   ======================================================================== */
inline const SBYTE STATSMOD::mfModifyStats( 
	MOD_TYPE const ModType, 
	SBYTE const Amount
)
{
	SBYTE	OldVal = bStatsMods[ModType];
    bStatsMods[ModType] += Amount;         // check for under/overflow?
	return OldVal;
}

/* =======================================================================
   Function    - mfSetStatsSet
   Description - sets the value of the stats over-ride
   Returns     - the old value
   ======================================================================== */
inline const SBYTE STATSMOD::mfSetStats( 
	SET_TYPE const ModType, 
	SBYTE const Value
)
{
	SBYTE	OldVal = bStatsSets[ModType];
	bStatsSets[ModType] = Value;
	return OldVal;
}

/* =======================================================================
   Function    - mfGetMod
   Description - returns the specified stats mod
   Returns     - the value
   ======================================================================== */
inline const SBYTE STATSMOD::mfGetMod( 
	MOD_TYPE const ModType 
) const
{
	return bStatsMods[ModType];
}

/* =======================================================================
   Function    - mfGetSet
   Description - returns the specified stats set
   Returns     - the value
   ======================================================================== */
inline const SBYTE STATSMOD::mfGetSet( 
	SET_TYPE const ModType 
) const
{
	return bStatsSets[ModType];
}

#endif // _STATSMOD_HXX
