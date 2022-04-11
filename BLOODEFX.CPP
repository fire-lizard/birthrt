#include "itemtype.hxx"
#include "blood.hxx"


LONG ItemEffects::BAHealing(LONG Combo,LONG AbilityStrength,LONG Action)
{
	return OK; //everything went ok	
}


//blood abilities
// 10% more success in diplomacy
LONG ItemEffects::BABloodMark(LONG Combo,LONG Arg,LONG Action)	
{return 0;}
// 15% more success in diplomacy
LONG ItemEffects::BADivineAura(LONG Combo,LONG Arg,LONG Action)
{return 0;}

//this one is easy to implement... it doesn't do anything.
LONG ItemEffects::BAIronWill(LONG Combo,LONG Arg,LONG Action)
{return 0;}


LONG ItemEffects::BAProtectionEvil(LONG Combo,LONG Arg,LONG Action)
{return 0;}

LONG ItemEffects::BARegeneration(LONG Combo,LONG Arg,LONG Action)
{
	return OK; //everything went ok	
}


LONG ItemEffects::BATravel(LONG Combo,LONG Arg,LONG Action)
{
	return 0;
}

LONG ItemEffects::BAResistance(LONG Combo,LONG Arg,LONG Action)
{
	return OK; //everything went ok	
}



//essentially, MapHighlightSecretDoors
LONG ItemEffects::BADetectIllusion(LONG Combo,LONG Arg,LONG Action)
{
	return 0;
}



LONG ItemEffects::BABattleWise(LONG Combo,LONG Arg,LONG Action)
{
	return OK; //everything went ok	
}
/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */

LONG ItemEffects::BACourage(LONG Combo,LONG Arg,LONG Action)
{
	return OK; //everything went ok	
}
/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */

LONG ItemEffects::BADevineWrath(LONG Combo,LONG Arg,LONG Action)
{
	return OK; //everything went ok	
}
/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */

LONG ItemEffects::BAHeightendAbility(LONG Combo,LONG Arg,LONG Action)
{
	return OK; //everything went ok	
}

/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */
LONG ItemEffects::BAEnhancedSense(LONG Combo,LONG Arg,LONG Action)
{
	return OK; //everything went ok	
}
/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */
LONG ItemEffects::BAFear(LONG Combo,LONG Arg,LONG Action)
{
	return OK; //everything went ok	
}

