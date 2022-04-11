
#include "system.h"

#include "playstat.hxx"
#include "avatar.hxx"
#include "itemtype.hxx"
#include "tmsgbox.hxx"
#include "dice.h"
#include "strenum.h"
#include "scnmgr.hxx"

#include "battleui.hxx"
#include "gamemap.hxx"
#include "strenum.h"   


//Relics of Power!
LONG ItemEffects::ChaliceOfDead(LONG Combo,LONG Arg,LONG Action)
{Arg=Combo;Combo=Action;Action=Arg;return 0;}


LONG ItemEffects::TomePrince(LONG Combo,LONG Arg,LONG Action)
{
	SHORT Itemtype;
	SHORT hPlayerStats;
 	
 	SPLIT_COMBO(Combo,Itemtype,hPlayerStats);
	DumbAutoLockPtr<PLAYER_STATS> pAvatarStats(hPlayerStats);
	ItemType& Item=ItemTypes[Itemtype]; //A reference to the item

	switch(Action)
	{
		case ACTIVATE:
			Item.mfMessage(STR_ITEM_ALWAYS_IN_EFFECT);
			return DONT_DROP;
		case DEACTIVATE:
			break;
		case PICKUP:
			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::CONTEST_MOD, +5);
			break;
		case DROP:
			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::CONTEST_MOD, -5);
			break;
		case EQUIP:
			return ALWAYS_IN_EFFECT;
		case UNEQUIP:
			break;
		case SERVICE:
			break;
		default:
			return NOT_SUPPORTED;
	}
	
	return OK; //everything went ok	
}


LONG ItemEffects::Corgandal(LONG Combo,LONG Arg,LONG Action)
{
	SHORT Itemtype;
	SHORT hPlayerStats;
 	
 	SPLIT_COMBO(Combo,Itemtype,hPlayerStats);
	DumbAutoLockPtr<PLAYER_STATS> pAvatarStats(hPlayerStats);
	ItemType& Item=ItemTypes[Itemtype]; //A reference to the item

	switch(Action)
	{
		case ACTIVATE:
			Item.mfMessage(STR_ITEM_ALWAYS_IN_EFFECT);
			return DONT_DROP;
		case DEACTIVATE:
			break;
		case PICKUP:
			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::TRANSPORT_MOD, +1);
			break;
		case DROP:
			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::TRANSPORT_MOD, -1);
			break;
		case EQUIP:
			return ALWAYS_IN_EFFECT;
		case UNEQUIP:
			break;
		case SERVICE:
			break;
		default:
			return NOT_SUPPORTED;
	}
	
	return OK; //everything went ok	
}


LONG ItemEffects::Danica(LONG Combo,LONG Arg,LONG Action)
{
	SHORT Itemtype;
	SHORT hPlayerStats;
 	
 	SPLIT_COMBO(Combo,Itemtype,hPlayerStats);
	DumbAutoLockPtr<PLAYER_STATS> pAvatarStats(hPlayerStats);
	ItemType& Item=ItemTypes[Itemtype]; //A reference to the item

	switch(Action)
	{
		case ACTIVATE:
			Item.mfMessage(STR_ITEM_ALWAYS_IN_EFFECT);
			return DONT_DROP;
		case DEACTIVATE:
			break;
		case PICKUP:
			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::SEEHIDDEN_MOD, +1);
			break;
		case DROP:
			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::SEEHIDDEN_MOD, -1);
			break;
		case EQUIP:
			return ALWAYS_IN_EFFECT;
		case UNEQUIP:
			break;
		case SERVICE:
			break;
		default:
			return NOT_SUPPORTED;
	}
	
	return OK; //everything went ok	
}

LONG ItemEffects::HammerOfThunder(LONG Combo,LONG Arg,LONG Action)
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
		{
			DumbAutoLockPtr<PLAYER_STATS> pAvatarStats(hPlayerStats);

			DICE d={1,10,5};
			LONG damage=RollDice(&d);
		
			if (pAvatarStats->hAvatar != fERROR)
			{
				DumbAutoLockPtr<CAvatar> pAvatar(pAvatarStats->hAvatar);
				pAvatar->mfSetDamageDealt(damage);
				pAvatar->mfCreateFireBall( GetNewId(), LIGHTNING_1 ,damage);
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
		{
			DumbAutoLockPtr<PLAYER_STATS> pAvatarStats(hPlayerStats);

			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::WEAPONHIT_MOD,+3);
			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::WEAPONDAMAGE_MOD,+5);
		}
			break;
		case UNEQUIP:
		{
			DumbAutoLockPtr<PLAYER_STATS> pAvatarStats(hPlayerStats);

			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::WEAPONHIT_MOD,-3);
			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::WEAPONDAMAGE_MOD,-5);
		}
		
			break;
		case SERVICE:
			break;
		default:
			return NOT_SUPPORTED;
	}
	
	return OK; //everything went ok	
}
LONG ItemEffects::SwordOfRoele(LONG Combo,LONG Arg,LONG Action)
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

			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::WEAPONHIT_MOD,+2);
			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::WEAPONDAMAGE_MOD,+2);
			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::BTL_MORALE_MOD,+1);
			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::BTL_ATTACK_MOD,+1);
			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::BTL_DEFEND_MOD,+1);
		
		}	
			break;
		case DROP:
		{
			DumbAutoLockPtr<PLAYER_STATS> pAvatarStats(hPlayerStats);

			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::WEAPONHIT_MOD,-2);
			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::WEAPONDAMAGE_MOD,-2);
			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::BTL_MORALE_MOD,-1);
			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::BTL_ATTACK_MOD,-1);
			pAvatarStats->StatsMod.mfModifyStats(STATSMOD::BTL_DEFEND_MOD,-1);
		}
			break;
		case EQUIP:
			return ALWAYS_IN_EFFECT;
		case UNEQUIP:
			break;
		case SERVICE:
			break;
		default:
			return NOT_SUPPORTED;
	}

	return OK; //everything went ok	
}


/* -----------------------------------------------------------------
   
   ----------------------------------------------------------------- */

LONG ItemEffects::CandleOfInvocation(LONG Combo,LONG Arg,LONG Action)
{

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
		switch(Action)
		{
			case ACTIVATE:
			{
				DumbAutoLockPtr<PLAYER_STATS> pPlayerStats(hPlayerStats);
				
				// only priests get this spell
				if(pPlayerStats->mfIsType(PLAYER_CLASS_INFO::PRIEST))
				{
					Item.mfMessage(STR_INVOKE_CANDLE);
					pPlayerStats->StatsMod.mfModifyStats(STATSMOD::LEVEL_MOD, +2);
				}
				else
					Item.mfMessage(STR_ACTIVATE_LIGHT);
				mfAddActiveItem(ItemEffects::CandleOfInvocation,Combo,Arg,Item.mfDuration(hPlayerStats));
				gfTestMagicLightSwitch=TRUE;
				
			}
				break;
			case DEACTIVATE:
			{
				DumbAutoLockPtr<PLAYER_STATS> pPlayerStats(hPlayerStats);
				
				if(pPlayerStats->mfIsType(PLAYER_CLASS_INFO::PRIEST))
				{
					Item.mfMessage(STR_REVOKE_CANDLE);
					pPlayerStats->StatsMod.mfModifyStats(STATSMOD::LEVEL_MOD, -2);
				}
				else
					Item.mfMessage(STR_DEACTIVATE_LIGHT);
				mfRemoveActiveItem(ItemEffects::CandleOfInvocation,Combo,Arg);
				gfTestMagicLightSwitch=FALSE;
			}
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
	else	// adventure level
	{
		return NOT_SUPPORTED;
	}
	return OK; //everything went ok	
}



/* -----------------------------------------------------------------
   
   ----------------------------------------------------------------- */

LONG ItemEffects::AmuletInspiration(LONG Combo,LONG Arg,LONG Action)
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
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BTL_MORALE_MOD, +1);
				mfAddActiveItem(ItemEffects::AmuletInspiration,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
				break;
			case DROP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BTL_MORALE_MOD, -1);
				mfRemoveActiveItem(ItemEffects::AmuletInspiration,Combo,Arg);
				}
				break;
			case EQUIP:
				return ALWAYS_IN_EFFECT;
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

LONG ItemEffects::BannerOfRoele(LONG Combo,LONG Arg,LONG Action)
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
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BTL_ROUTE_MOD, +1);
				mfAddActiveItem(ItemEffects::BannerOfRoele,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
				break;
			case DROP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BTL_ROUTE_MOD, -1);
				mfRemoveActiveItem(ItemEffects::BannerOfRoele,Combo,Arg);
				}
				break;
			case EQUIP:
				return ALWAYS_IN_EFFECT;
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

LONG ItemEffects::CrownOfCommand(LONG Combo,LONG Arg,LONG Action)
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
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::FREEACTION_MOD, +1);
				mfAddActiveItem(ItemEffects::CrownOfCommand,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
				break;
			case DROP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::FREEACTION_MOD, -1);
				mfRemoveActiveItem(ItemEffects::CrownOfCommand,Combo,Arg);
				}
				break;
			case EQUIP:
				return ALWAYS_IN_EFFECT;
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

LONG ItemEffects::SceptreCuiraeecn(LONG Combo,LONG Arg,LONG Action)
{

	ITEMTYPE		IType;
	SHORT			hPlayerStats;
	
 	SPLIT_COMBO(Combo,IType,hPlayerStats);
 	
	ItemType&		Item=ItemTypes[IType];

	
	if(fDomainTurn)
	{
		return NOT_SUPPORTED;
	}
	else
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
				//Item.mfMessage(STR_ITEM_ALWAYS_IN_EFFECT);
				DumbAutoLockPtr<PLAYER_STATS> pAvatarStats(hPlayerStats);

				DICE const d={12,8,0};	// damage 12->96
				LONG const damage=RollDice(&d);
			
				if (pAvatarStats->hAvatar != fERROR)
				{
					DumbAutoLockPtr<CAvatar> pAvatar(pAvatarStats->hAvatar);
					pAvatar->mfSetDamageDealt(damage);
					pAvatar->mfCreateFireBall( GetNewId(), LIGHTNING_1 ,damage);
				}
						
			}
				return DECREMENT_CHARGE;
			    
			case DEACTIVATE:
				break;
			case PICKUP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BTL_ATTACK_MOD, +1);
				mfAddActiveItem(ItemEffects::SceptreCuiraeecn,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
				break;
			case DROP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BTL_ATTACK_MOD, -1);
				mfRemoveActiveItem(ItemEffects::SceptreCuiraeecn,Combo,Arg);
				}
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
LONG ItemEffects::BarazadsTools(LONG Combo,LONG Arg,LONG Action)
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
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BUILDCOST_MOD, -25);
				mfAddActiveItem(ItemEffects::BarazadsTools,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
				break;
			case DROP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BUILDCOST_MOD, +25);
				mfRemoveActiveItem(ItemEffects::BarazadsTools,Combo,Arg);
				}
				break;
			case EQUIP:
				return ALWAYS_IN_EFFECT;
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


LONG ItemEffects::FaelesRing(LONG Combo,LONG Arg,LONG Action)
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
				break;
			case DROP:
				break;
			case EQUIP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::FREEMAGIC_MOD, +1);
				mfAddActiveItem(ItemEffects::FaelesRing,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
				break;
			case UNEQUIP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::FREEMAGIC_MOD, -1);
				mfRemoveActiveItem(ItemEffects::FaelesRing,Combo,Arg);
				}
				break;
			case SERVICE:
				break;
			default:
				return NOT_SUPPORTED;
		}
	}
	return OK; //everything went ok	
}

LONG ItemEffects::GavelonStaff(LONG Combo,LONG Arg,LONG Action)
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
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::PROSPERITY_MOD, +1);
				mfAddActiveItem(ItemEffects::GavelonStaff,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
				break;
			case DROP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::PROSPERITY_MOD, -1);
				mfRemoveActiveItem(ItemEffects::GavelonStaff,Combo,Arg);
				}
				break;
			case EQUIP:
				return ALWAYS_IN_EFFECT;
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

LONG ItemEffects::DierdriensRing(LONG Combo,LONG Arg,LONG Action)
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
				break;
			case DROP:
				break;
			case EQUIP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::FREEFORGE_MOD, +1);
				mfAddActiveItem(ItemEffects::DierdriensRing,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
				break;
			case UNEQUIP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::FREEFORGE_MOD, -1);
				mfRemoveActiveItem(ItemEffects::DierdriensRing,Combo,Arg);
				}
				break;
			case SERVICE:
				break;
			default:
				return NOT_SUPPORTED;
		}
	}
	return OK; //everything went ok	
}


LONG ItemEffects::BrennasFavor(LONG Combo,LONG Arg,LONG Action)
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
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BLOODLINE_MOD, +5);
				mfAddActiveItem(ItemEffects::BrennasFavor,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
				break;
			case DROP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BLOODLINE_MOD, -5);
				mfRemoveActiveItem(ItemEffects::BrennasFavor,Combo,Arg);
				}
				break;
			case EQUIP:
				return ALWAYS_IN_EFFECT;
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

LONG ItemEffects::StateCrownAnnuire(LONG Combo,LONG Arg,LONG Action)
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
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BLOODLINE_MOD, +14);
				mfAddActiveItem(ItemEffects::StateCrownAnnuire,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
				break;
			case DROP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BLOODLINE_MOD, -14);
				mfRemoveActiveItem(ItemEffects::StateCrownAnnuire,Combo,Arg);
				}
				break;
			case EQUIP:
				return ALWAYS_IN_EFFECT;
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

LONG ItemEffects::EmperorsCrown(LONG Combo,LONG Arg,LONG Action)
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
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BLOODLINE_MOD, +6);
				mfAddActiveItem(ItemEffects::EmperorsCrown,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
				break;
			case DROP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BLOODLINE_MOD, -6);
				mfRemoveActiveItem(ItemEffects::EmperorsCrown,Combo,Arg);
				}
				break;
			case EQUIP:
				return ALWAYS_IN_EFFECT;
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

LONG ItemEffects::FaridsCoffer(LONG Combo,LONG Arg,LONG Action)
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
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::MONEYMULT_MOD, +20);
				mfAddActiveItem(ItemEffects::FaridsCoffer,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
				break;
			case DROP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::MONEYMULT_MOD, -20);
				mfRemoveActiveItem(ItemEffects::FaridsCoffer,Combo,Arg);
				}
				break;
			case EQUIP:
				return ALWAYS_IN_EFFECT;
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

LONG ItemEffects::RingOfWizardry(LONG Combo,LONG Arg,LONG Action)
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
				break;
			case DROP:
				break;
			case EQUIP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::SPELLMULT_MOD, +1);
				mfAddActiveItem(ItemEffects::RingOfWizardry,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
				break;
			case UNEQUIP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::SPELLMULT_MOD, -1);
				mfRemoveActiveItem(ItemEffects::RingOfWizardry,Combo,Arg);
				/* should probably wipe this guy's spells as punishment */
				}
				break;
			case SERVICE:
				break;
			default:
				return NOT_SUPPORTED;
		}
	}
	return OK; //everything went ok	
}

LONG ItemEffects::RobesOfMasses(LONG Combo,LONG Arg,LONG Action)
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
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::AGITATE_MOD, +5);
				mfAddActiveItem(ItemEffects::RobesOfMasses,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
				break;
			case DROP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::AGITATE_MOD, -5);
				mfRemoveActiveItem(ItemEffects::RobesOfMasses,Combo,Arg);
				}
				break;
			case EQUIP:
				return ALWAYS_IN_EFFECT;
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

LONG ItemEffects::RegaliaEmpire(LONG Combo,LONG Arg,LONG Action)
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
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::INVESTITURE_MOD, +5);
				mfAddActiveItem(ItemEffects::RegaliaEmpire,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
				break;
			case DROP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::INVESTITURE_MOD, -5);
				mfRemoveActiveItem(ItemEffects::RegaliaEmpire,Combo,Arg);
				}
				break;
			case EQUIP:
				return ALWAYS_IN_EFFECT;
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

LONG ItemEffects::SielGemLarge(LONG Combo,LONG Arg,LONG Action)
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
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BLOODLINE_MOD, +2);

				while (mfGetActiveItem(ItemEffects::SielGemLarge,Combo,Arg))
					Arg++;

				mfAddActiveItem(ItemEffects::SielGemLarge,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
				break;
			case DROP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BLOODLINE_MOD, -2);
				mfRemoveActiveItem(ItemEffects::SielGemLarge,Combo,Arg);
				}
				break;
			case EQUIP:
				return ALWAYS_IN_EFFECT;
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

LONG ItemEffects::SielGemSmall(LONG Combo,LONG Arg,LONG Action)
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
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BLOODLINE_MOD, +1);
				mfAddActiveItem(ItemEffects::SielGemSmall,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
				break;
			case DROP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::BLOODLINE_MOD, -1);
				mfRemoveActiveItem(ItemEffects::SielGemSmall,Combo,Arg);
				}
				break;
			case EQUIP:
				return ALWAYS_IN_EFFECT;
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


LONG ItemEffects::NappolansTome(LONG Combo,LONG Arg,LONG Action)
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
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::MOVECOST_MOD, -1);
				mfAddActiveItem(ItemEffects::NappolansTome,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
				break;
			case DROP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::MOVECOST_MOD, +1);
				mfRemoveActiveItem(ItemEffects::NappolansTome,Combo,Arg);
				}
				break;
			case EQUIP:
				return ALWAYS_IN_EFFECT;
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

LONG ItemEffects::VaubenelsBook(LONG Combo,LONG Arg,LONG Action)
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
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::FORTCOST_MOD, -25);
				mfAddActiveItem(ItemEffects::VaubenelsBook,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
				break;
			case DROP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::FORTCOST_MOD, +25);
				mfRemoveActiveItem(ItemEffects::VaubenelsBook,Combo,Arg);
				}
				break;
			case EQUIP:
				return ALWAYS_IN_EFFECT;
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

LONG ItemEffects::TorcSplendor(LONG Combo,LONG Arg,LONG Action)
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
				
				pPlayerStats->StatsMod.mfSetStats(STATSMOD::CHARISMA_SET, 18);
				mfAddActiveItem(ItemEffects::TorcSplendor,Combo,Arg,Item.mfDuration(hPlayerStats));
				}
				break;
			case DROP:
				{
				PLAYER_STATS* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfSetStats(STATSMOD::CHARISMA_SET, 0);
				mfRemoveActiveItem(ItemEffects::TorcSplendor,Combo,Arg);
				}
				break;
			case EQUIP:
				return ALWAYS_IN_EFFECT;
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
