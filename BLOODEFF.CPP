
#include "system.h"
#include "playstat.hxx"
#include "avatar.hxx"
#include "itemtype.hxx"
#include "tmsgbox.hxx"
#include "dice.h"
#include "strenum.h"
#include "scnmgr.hxx"
#include "regen.hxx"

#include "battleui.hxx"
#include "regents.hxx"
#include "strenum.h"   


LONG ItemEffects::BAHealing(LONG Combo,LONG AbilityStrength,LONG Action)
{
	//recursion...
	//static BOOL AlreadyCalled=FALSE;
	
	SHORT Itemtype;  			//the index in ItemTypes of the Item 
									//being activated
	SHORT hAvatarStats;   			//the stats of the avatar who's activating 
									//the item
	SPLIT_COMBO(Combo,Itemtype,hAvatarStats); //extract the data from the Combo
	
	ItemType& Item=ItemTypes[Itemtype]; //A reference to the item
	
	if(fDomainTurn)
	{
		return NOT_SUPPORTED;
	}
	if(fBattleCombat)
	{
		return NOT_SUPPORTED;
	}
	else	// adventure level
	{
		switch(Action)
		{
			case ACTIVATE:
			{
				LONG Result = ItemEffects::Heal(Combo,HEAL_FULLY,ACTIVATE);
				if ( OK == Result)
				{
					Result = DECREMENT_CHARGE;
				}
				return Result;
			}
			case DEACTIVATE:
				break;
			case PICKUP:
				break;
			case DROP:
				break;
			case EQUIP:
				break;
			case UNEQUIP:
				break;
			case SERVICE:
				break;
			default:
				return NOT_SUPPORTED;
		}
	}
	return OK; //everything went ok	
}


//blood abilities
// 10% more success in diplomacy
LONG ItemEffects::BABloodMark(LONG Combo,LONG Arg,LONG Action)	
{Arg=Combo;Combo=Action;Action=Arg;return 0;}
// 15% more success in diplomacy
LONG ItemEffects::BADivineAura(LONG Combo,LONG Arg,LONG Action)
{Arg=Combo;Combo=Action;Action=Arg;return 0;}

LONG ItemEffects::BAIronWill(LONG Combo,LONG Arg,LONG Action)
{
// GWP
// The characters in BR already have this factored into their hit points.
// Character editors just add 2!
#if 0
	SHORT Itemtype;  				//the index in ItemTypes of the Item 
									//being activated
	SHORT hPlayerStats;   			//the stats of the avatar who's activating 
									//the item
 	SPLIT_COMBO(Combo,Itemtype,hPlayerStats); //extract the data from the Combo
 	
	switch(Action)
	{
		case ACTIVATE:
			{
			ItemType& Item=ItemTypes[Itemtype]; //A reference to the item

			Item.mfMessage(STR_ITEM_ALWAYS_IN_EFFECT);
			}
			return DONT_DROP;
		case DEACTIVATE:
			break;
		case PICKUP:
		{
			DumbAutoLockPtr<PLAYER_STATS> pAvatarStats(hPlayerStats);
			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::HITPOINTS_MOD,2);
		}
			break;
		case DROP:
		{
			DumbAutoLockPtr<PLAYER_STATS> pAvatarStats(hPlayerStats);
			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::HITPOINTS_MOD,-2);
		}
			break;
		case EQUIP:
			return CANT_COMPLETE;
		case UNEQUIP:
			break;
		case SERVICE:
			break;
		default:
			return NOT_SUPPORTED;
	}
	
	return OK; //everything went ok	
#endif
	return NOT_SUPPORTED;
}


// Since it affects the whole party, the adventure part is in playhit.cpp
LONG ItemEffects::BAProtectionEvil(LONG Combo,LONG Arg,LONG Action)
{
	SHORT Itemtype;  				//the index in ItemTypes of the Item 
									//being activated
	SHORT hPlayerStats;   			//the stats of the avatar who's activating 
									//the item
 	SPLIT_COMBO(Combo,Itemtype,hPlayerStats); //extract the data from the Combo
 	
	switch(Action)
	{
		case ACTIVATE:
		{
			LONG Result = ItemEffects::ProtectionFromEvil(Combo, Arg, Action);
			if (Result == OK)
			{
				Result = DECREMENT_CHARGE;
			}
			return Result;
		}
		case DEACTIVATE:
			break;
		case PICKUP:
			break;
		case DROP:
			break;
		case EQUIP:
			break;
		case UNEQUIP:
			break;
		case SERVICE:
			break;
		default:
			return NOT_SUPPORTED;
	}
	
	return OK; //everything went ok	
}

LONG ItemEffects::BARegeneration(LONG Combo,LONG Arg,LONG Action)
{
	SHORT Itemtype;  				//the index in ItemTypes of the Item 
									//being activated
	SHORT hPlayerStats;   			//the stats of the avatar who's activating 
									//the item
 	SPLIT_COMBO(Combo,Itemtype,hPlayerStats); //extract the data from the Combo

	ItemType& Item=ItemTypes[Itemtype]; //A reference to the item

	if(fDomainTurn)
	{
		switch(Action)
		{
			case PICKUP:
				Regenerator::mfRegister(hPlayerStats,60);
			    break;
			    
			case DROP:
				Regenerator::mfUnRegister(hPlayerStats,60);
				break;
		}
		return OK;
	}
	else if(fBattleCombat)
	{
		return NOT_SUPPORTED;
	}
	else	// adventure level
	{
		switch(Action)
		{
			case ACTIVATE:
				Item.mfMessage(STR_ITEM_ALWAYS_IN_EFFECT);
				return DONT_DROP;
			case DEACTIVATE:
//				{
//					PLAYER_STATS* pAvStats=NULL;
//
//					DumbAutoLockPtr<PLAYER_STATS> pAvatarStats(hPlayerStats);
//					pAvStats=pAvatarStats;
//							    
//					if (pAvatarStats->hAvatar == fERROR)
//						break;
//
//					DumbAutoLockPtr<CAvatar> pAvatar(pAvatarStats->hAvatar);
//					CAvatar* pCAv=pAvatar;
//						
//					// This power doesn't help you when you're dead.
//					if (pAvatarStats->HitPts < pAvatarStats->FullHitPts &&
//						pAvatarStats->HitPts > 0 &&
//						pAvatar->Status != CAvatar::AI_DEAD)
//						++pAvatarStats->HitPts;
//	
//					ActiveItem* pai=mfGetActiveItem(BARegeneration,Combo,Arg);
//					pai->TimeToStop = SCENE_MGR::gTick + 60*18.2;
//				}
				break;
				
			case PICKUP:
				Regenerator::mfRegister(hPlayerStats,60);
				break;
			case DROP:
				Regenerator::mfUnRegister(hPlayerStats,60);
				break;
			case EQUIP:
				return CANT_COMPLETE;
			case UNEQUIP:
				break;
			case SERVICE:
				break;
			default:
				return NOT_SUPPORTED;
		}
	}
	return OK; //everything went ok	

}


LONG ItemEffects::BATravel(LONG Combo,LONG Arg,LONG Action)
{
	LONG Result = NOT_SUPPORTED;
	
	if(fDomainTurn)
	{
		Result = ItemEffects::MapTeleport(Combo,Arg,Action);
	}
	else if (fBattleCombat)
	{
		Result = NOT_SUPPORTED;
	}
	else
	{
		switch (Action)
		{
		case ACTIVATE:
			Result = ItemEffects::MapTeleport(Combo,Arg,Action);
			if (Result == OK)
			{
				Result = DECREMENT_CHARGE;
			}
			break;
		}
	}
	
	return Result;
}

LONG ItemEffects::BAResistance(LONG Combo,LONG Arg,LONG Action)
{
	SHORT Itemtype;  				//the index in ItemTypes of the Item 
									//being activated
	SHORT hPlayerStats;   			//the stats of the avatar who's activating 
									//the item
 	SPLIT_COMBO(Combo,Itemtype,hPlayerStats); //extract the data from the Combo

	ItemType& Item=ItemTypes[Itemtype]; //A reference to the item

	switch(Action)
	{
		case ACTIVATE:
			Item.mfMessage(STR_ITEM_ALWAYS_IN_EFFECT);
			return DONT_DROP;
		case DEACTIVATE:
			break;
		case PICKUP:
		{
			DumbAutoLockPtr<PLAYER_STATS> pAvatarStats(hPlayerStats);
			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::MAGIC_RESIST_MOD,50);
		}
			break;
		case DROP:
		{
			DumbAutoLockPtr<PLAYER_STATS> pAvatarStats(hPlayerStats);
			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::MAGIC_RESIST_MOD,-50);
		}
			break;
		case EQUIP:
			return CANT_COMPLETE;
		case UNEQUIP:
			break;
		case SERVICE:
			break;
		default:
			return NOT_SUPPORTED;
	}
	
	return OK; //everything went ok	
}



//essentially, MapHighlightSecretDoors
LONG ItemEffects::BADetectIllusion(LONG Combo,LONG Arg,LONG Action)
{
	if (fDomainTurn)
	{
		return NOT_SUPPORTED;
	}
	else if (fBattleCombat)
	{
		return NOT_SUPPORTED;
	}
	else
	{
		switch(Action)
		{
		case ACTIVATE:
			MapHighlightSecretDoors(Combo,Arg,Action);
			return DONT_DROP;
  		case PICKUP:
  			break;
  		case DROP:
			break;
		case EQUIP:
			return CANT_COMPLETE;
		case UNEQUIP:
			break;
		case SERVICE:
			break;
		default:
			return NOT_SUPPORTED;
		}
	}
	
	return OK;
}



LONG ItemEffects::BABattleWise(LONG Combo,LONG Arg,LONG Action)
{

	ITEMTYPE		IType;
	SHORT			hPlayerStats;
	
 	SPLIT_COMBO(Combo,IType,hPlayerStats);
 	
	ItemType&		Item=ItemTypes[IType];

	
	//if(fDomainTurn)
	//{
	//	return NOT_SUPPORTED;
	//}
	//if(fBattleCombat)
	//{
	//	return NOT_SUPPORTED;
	//}
	//else	// adventure level
	// works in all modes
	{
		switch(Action)
		{
			case ACTIVATE:
				Item.mfMessage(STR_ITEM_ALWAYS_IN_EFFECT);
				return DONT_DROP;
			    
			case DEACTIVATE:
				break;
			case PICKUP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BTL_ATTACK_MOD,+1);
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BTL_DEFEND_MOD,+1);
				}
				break;
			case DROP:
				break;
			case EQUIP:
				return CANT_COMPLETE;
			case UNEQUIP:
				break;
			case SERVICE:
				break;
			default:
				return NOT_SUPPORTED;
		}
	}
	return OK; //everything went ok	
}
/* -----------------------------------------------------------------
   
   ----------------------------------------------------------------- */

LONG ItemEffects::BACourage(LONG Combo,LONG Arg,LONG Action)
{

	ITEMTYPE		IType;
	SHORT			hPlayerStats;
	
 	SPLIT_COMBO(Combo,IType,hPlayerStats);
 	
	ItemType&		Item=ItemTypes[IType];

	
	//if(fDomainTurn)
	//{
	//	return NOT_SUPPORTED;
	//}
	//if(fBattleCombat)
	//{
	//	return NOT_SUPPORTED;
	//}
	//else	// adventure level
	// works in all modes
	{
		switch(Action)
		{
			case ACTIVATE:
				Item.mfMessage(STR_ITEM_ALWAYS_IN_EFFECT);
				return DONT_DROP;
			    
			case DEACTIVATE:
				break;
			case PICKUP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BTL_MORALE_MOD,+1);
				}
				break;
			case DROP:
				break;
			case EQUIP:
				return CANT_COMPLETE;
			case UNEQUIP:
				break;
			case SERVICE:
				break;
			default:
				return NOT_SUPPORTED;
		}
	}
	return OK; //everything went ok	
}
/* -----------------------------------------------------------------
   
   ----------------------------------------------------------------- */

LONG ItemEffects::BADevineWrath(LONG Combo,LONG Arg,LONG Action)
{

	ITEMTYPE		IType;
	SHORT			hPlayerStats;
	
 	SPLIT_COMBO(Combo,IType,hPlayerStats);
 	
	ItemType&		Item=ItemTypes[IType];

	
	//if(fDomainTurn)
	//{
	//	return NOT_SUPPORTED;
	//}
	//if(fBattleCombat)
	//{
	//	return NOT_SUPPORTED;
	//}
	//else	// adventure level
	// works in all modes
	{
		switch(Action)
		{
			case ACTIVATE:
				Item.mfMessage(STR_ITEM_ALWAYS_IN_EFFECT);
				return DONT_DROP;
			case DEACTIVATE:
				break;
			case PICKUP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::WEAPONHIT_MOD,+1);
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::WEAPONDAMAGE_MOD,+1);
				}
				break;
			case DROP:
				break;
			case EQUIP:
				return CANT_COMPLETE;
			case UNEQUIP:
				break;
			case SERVICE:
				break;
			default:
				return NOT_SUPPORTED;
		}
	}
	return OK; //everything went ok	
}
/* -----------------------------------------------------------------
   
   ----------------------------------------------------------------- */

LONG ItemEffects::BAHeightendAbility(LONG Combo,LONG Arg,LONG Action)
{
// GWP
// These ablities are already reflected in the stats of the regents.
// That's because this fn can't work until the Blood types are in the
// PLAYER_STAT data.
#if 0
	DICE	dice;
	LONG	mod;
	LONG	RegentIndex;
	
	ITEMTYPE		IType;
	SHORT			hPlayerStats;
	
	if (CalledRecursively)
		return CANT_COMPLETE;
	
 	SPLIT_COMBO(Combo,IType,hPlayerStats);
 	
	ItemType&		Item=ItemTypes[IType];
	

	if(fDomainTurn)
	{
		return NOT_SUPPORTED;
	}
	if(fBattleCombat)
	{
		return NOT_SUPPORTED;
	}
	else	// adventure level
	// works in all modes, well not really until blood line derivation 
	// is in the player stats, not some avatar related global array
	{
		switch(Action)
		{
			case ACTIVATE:
				char buffer[120];
				strcpy(buffer, STRMGR_GetStr(STR_ACTIVATE_HEIGHTENEDAB));
				mfMessageBox(Item.mfGetName(), buffer);
				return DONT_DROP;
			case DEACTIVATE:
				break;
			case PICKUP:
				{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hPlayerStats);
				
				for(RegentIndex = 0; RegentIndex < REGENT_COUNT; ++RegentIndex)
				{
					// GWP This really sucks. There isn't a back pointer or Id in the player stats struct.
					// GWP It also doesn't work because you can't compare against
					//     regents who haven't been loaded yet. And you can't finish loading
					//     until you can compare. We need the blood type in the player stats!
					LONG const Id = regents[RegentIndex].mfGetid();
					if( Id >= 1000 
					    && Id <= 30000)
					{
					    SHORT const hTestPlayerstats = LoadStats(Id, fERROR);
						
					    if (hTestPlayerstats == hPlayerStats)
						{
							switch(	regents[RegentIndex].mfGetBL_deriv() )
							{
								case ANDUIRAS:
									if(random(2))	// strength or charisma (50/50)
									{
										if(pPlayerStats->mfGetStr() > 14)
											mod = 1;
										else
										{
											dice.sbNumberOfDice=1;
											dice.sbNumberOfSides=6;
											dice.sbModifier=0;
											mod = RollDice(&dice);
										}
										pPlayerStats->StatsMod.mfModifyStats(STATSMOD::STRENGTH_MOD,mod);
									}
									else
									{
										if(pPlayerStats->mfGetChr() > 14)
											mod = 1;
										else
										{
											dice.sbNumberOfDice=1;
											dice.sbNumberOfSides=6;
											dice.sbModifier=0;
											mod = RollDice(&dice);
										}
										pPlayerStats->StatsMod.mfModifyStats(STATSMOD::CHARISMA_MOD,mod);
									}
									break;
								case REYNIR:
										if(pPlayerStats->mfGetCon() > 14)
											mod = 1;
										else
										{
											dice.sbNumberOfDice=1;
											dice.sbNumberOfSides=6;
											dice.sbModifier=0;
											mod = RollDice(&dice);
										}
										pPlayerStats->StatsMod.mfModifyStats(STATSMOD::CONSTITUTION_MOD,mod);
									break;
								case BRENNA:
										if(pPlayerStats->mfGetDex() > 14)
											mod = 1;
										else
										{
											dice.sbNumberOfDice=1;
											dice.sbNumberOfSides=6;
											dice.sbModifier=0;
											mod = RollDice(&dice);
										}
										pPlayerStats->StatsMod.mfModifyStats(STATSMOD::DEXTERITY_MOD,mod);
									break;
								case BASAIA:
										if(pPlayerStats->mfGetInt() > 14)
											mod = 1;
										else
										{
											dice.sbNumberOfDice=1;
											dice.sbNumberOfSides=6;
											dice.sbModifier=0;
											mod = RollDice(&dice);
										}
										pPlayerStats->StatsMod.mfModifyStats(STATSMOD::INTELLIGENCE_MOD,mod);
									break;
								case VORYNN:
										if(pPlayerStats->mfGetWis() > 14)
											mod = 1;
										else
										{
											dice.sbNumberOfDice=1;
											dice.sbNumberOfSides=6;
											dice.sbModifier=0;
											mod = RollDice(&dice);
										}
										pPlayerStats->StatsMod.mfModifyStats(STATSMOD::WISDOM_MOD,mod);
									break;
								case AZRAI:
									if(random(2))	// intelligence or charisma (50/50)
									{
										if(pPlayerStats->mfGetInt() > 14)
											mod = 1;
										else
										{
											dice.sbNumberOfDice=1;
											dice.sbNumberOfSides=6;
											dice.sbModifier=0;
											mod = RollDice(&dice);
										}
										pPlayerStats->StatsMod.mfModifyStats(STATSMOD::INTELLIGENCE_MOD,mod);
									}
									else
									{
										if(pPlayerStats->mfGetChr() > 14)
											mod = 1;
										else
										{
											dice.sbNumberOfDice=1;
											dice.sbNumberOfSides=6;
											dice.sbModifier=0;
											mod = RollDice(&dice);
										}
										pPlayerStats->StatsMod.mfModifyStats(STATSMOD::CHARISMA_MOD,mod);
									}
									break;
							}
						}
					}
				}
				}
				break;
			case DROP:
				break;
			case EQUIP:
				return CANT_COMPLETE;
			case UNEQUIP:
				break;
			case SERVICE:
				break;
			default:
				return NOT_SUPPORTED;
		}
	}
	return OK; //everything went ok	
#endif
	return CANT_COMPLETE;
}

/* -----------------------------------------------------------------
   
   ----------------------------------------------------------------- */
LONG ItemEffects::BAEnhancedSense(LONG Combo,LONG Arg,LONG Action)
{
	DICE	dice;
	LONG	mod;
	LONG	RegentIndex;
	
	ITEMTYPE		IType;
	SHORT			hPlayerStats;
	
 	SPLIT_COMBO(Combo,IType,hPlayerStats);
 	
	ItemType&		Item=ItemTypes[IType];
	

	if(fDomainTurn)
	{
		switch (Action)
		{
		case ACTIVATE:
			return CANT_COMPLETE;
		}
	}
	else if(fBattleCombat)
	{
		return NOT_SUPPORTED;
	}
	else	// adventure level
	// works in all modes, well not really until blood line derivation 
	// is in the player stats, not some avatar related global array
	{
		switch(Action)
		{
			case ACTIVATE:
			{
				PLAYER_STATS * const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				SHORT 		const hAvatar = pPlayerStats->hAvatar;
				
				if (hAvatar == fERROR)
					return NOT_SUPPORTED;
					
				CAvatar	const * const pAvatar=(CAvatar const * const )BLKPTR(hAvatar);
				
				for(RegentIndex = 0; RegentIndex < REGENT_COUNT; ++RegentIndex)
				{
					if(regents[RegentIndex].mfGetid() == pAvatar->Id)
					{
						switch(	regents[RegentIndex].mfGetBL_deriv() )
						{
						case ANDUIRAS:
							{
							LONG Result = ItemEffects::MapHighlightEvil(Combo, 600, Action );
							if (Result == OK)
							{
								Result = DECREMENT_CHARGE;
							}
							return Result;
							}
						case AZRAI:
							gfTestMagicLightSwitch = TRUE;
							break;
						case BRENNA:
							// GWP How do I turn it off?
							gfInfraVision = TRUE;
							break;
						case VORYNN:
							// plus 1 to all Realm actions
							Item.mfMessage(STR_ITEM_ALWAYS_IN_EFFECT);
							break;
						}
						
						break;
					}
				}
				return DONT_DROP;
			}
			case DEACTIVATE:
				break;
			case PICKUP:
				break;
			case DROP:
				break;
			case EQUIP:
				break;
			case UNEQUIP:
				break;
			case SERVICE:
				break;
			default:
				return NOT_SUPPORTED;
		}
	}
	return OK; //everything went ok	
}
/* -----------------------------------------------------------------
   
   ----------------------------------------------------------------- */
LONG ItemEffects::BAFear(LONG Combo,LONG Arg,LONG Action)
{
	STATSMOD::MOD_TYPE	type;
	LONG				mod;

	ITEMTYPE		IType;
	SHORT			hPlayerStats;
	
 	SPLIT_COMBO(Combo,IType,hPlayerStats);
 	
	ItemType&		Item=ItemTypes[IType];

	
	if(fDomainTurn)
	{
		return NOT_SUPPORTED;
	}
	if(fBattleCombat)
	{
		return NOT_SUPPORTED;
	}
	else	// adventure level
	// works in all modes
	{
		switch(Action)
		{
			case ACTIVATE:
				{
				// GWP No real fear spell for now.
				LONG Result = ItemEffects::Confusion(Combo, 0, Action);
				if (Result == OK)
				{
					Result = DECREMENT_CHARGE;
				}
				return Result;
				}
			case DEACTIVATE:
				break;
			case PICKUP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::FEAR_RESIST_MOD,+1);
				mfAddActiveItem(ItemEffects::BAFear,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
			break;
				
			case DROP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::FEAR_RESIST_MOD,-1);
				mfRemoveActiveItem(ItemEffects::BAFear,Combo,Arg);
				}
				break;
				
			case EQUIP:
				return CANT_COMPLETE;
			case UNEQUIP:
				break;
			case SERVICE:
				break;
			default:
				return NOT_SUPPORTED;
		}
	}
	return OK; //everything went ok	
}
