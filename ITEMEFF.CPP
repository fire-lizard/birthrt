#include "avatar.hxx"
#include "battleui.hxx"
#include "dice.h"
#include "effsepar.hxx"
#include "invntory.hxx"
#include "parch.hxx"
#include "playstat.hxx"
#include "itemtype.hxx"
#include "itemutil.hxx"
#include "strenum.h"
#include "spell.hxx"
#include "scene.hxx"
#include "scnmgr.hxx"
#include "strenum.h"
#include "tmsgbox.hxx"


/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */

LONG ItemEffects::Generic(LONG Combo,LONG,LONG action)
{	
	ITEMTYPE IType;
	SHORT hPlayerStats;
 	SPLIT_COMBO(Combo,IType,hPlayerStats);

	ItemType& Item=ItemTypes[IType];
	
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
		switch(action)
		{
			case ACTIVATE:
			{
				char buffer1[100], buffer2[100];
				strcpy(buffer1, STRMGR_GetStr(STR_ITEM_YOU_USE_THE));
				sprintf(buffer2, "%s %s",buffer1, Item.mfGetName());
				mfMessageBox(Item.mfGetName(), buffer2);
				return DONT_DROP;
			}
				
			case PICKUP:
//		     	printf("Generic object PICKUP effect called\n");
				break;
			case EQUIP:
				printf("Generic object EQUIP effect called\n");
				break;
			case UNEQUIP:
				printf("Generic object UNEQUIP effect called\n");
				break;
			case DROP:
				printf("Generic object DROP effect called\n");
				break;
			case SERVICE:
				break;
			default:
				return NOT_SUPPORTED;
		}
	}
	return OK;
}

LONG ItemEffects::Levitate(LONG Combo,LONG Arg,LONG Action)
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
		return NOT_SUPPORTED;
	}
	else	// adventure level
	{
		switch(Action)
		{
			case ACTIVATE:
			{
				if (!mfAddActiveItem(ItemEffects::Levitate,Combo,Arg,Item.mfDuration(hPlayerStats)))
					return CANT_COMPLETE;
					
				DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(hPlayerStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbAutoLockPtr<CAvatar>	const pAvatar(pPlayerStats->hAvatar);
					mfSound(SND_LEVITATE1,1,pAvatar->ThingIndex);
				}
				
				ADVENTURER_TEAM_ITOR AdvItor;
				for (AdvItor = ADVENTURER::begin(); AdvItor != ADVENTURER::end();
		    		AdvItor++)
		    	{
					DumbAutoLockPtr<CAvatar> const pAvatar(*AdvItor);
					pAvatar->attrib.IsFlying=1;
				}
				fClimb = TRUE;
				player.Flying=TRUE;
			
				Item.mfMessage(STR_ACTIVATE_LEVITATE);
  			}
				break;
				
			case DEACTIVATE:
			{
				ADVENTURER_TEAM_ITOR AdvItor;
				for (AdvItor = ADVENTURER::begin(); AdvItor != ADVENTURER::end();
		    		AdvItor++)
		    	{
					DumbAutoLockPtr<CAvatar> const pAvatar(*AdvItor);
					pAvatar->attrib.IsFlying=0;
				}
				fClimb = FALSE;
				player.Flying=FALSE;
				Item.mfMessage(STR_DEACTIVATE_LEVITATE);
				mfRemoveActiveItem(ItemEffects::Levitate,Combo,Arg);
			}
				break;
			default:
				return NOT_SUPPORTED;
		}
	}
	
	return OK;
}

LONG ItemEffects::Flight(LONG Combo,LONG Arg,LONG action)
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
		return NOT_SUPPORTED;
	}
	else	// adventure level
	{
		switch(action)
		{
			case ACTIVATE:
			{
				if (!mfAddActiveItem(ItemEffects::Flight,Combo,Arg,Item.mfDuration(hPlayerStats)))
					return CANT_COMPLETE;
	
				DumbAutoLockPtr<PLAYER_STATS const>	const pPlayerStats(hPlayerStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbAutoLockPtr<CAvatar>	const pAvatar(pPlayerStats->hAvatar);
					mfSound(SND_GENERAL_MAGIC1,1,pAvatar->ThingIndex);
				}
				
				ADVENTURER_TEAM_ITOR AdvItor;
				for (AdvItor = ADVENTURER::begin(); AdvItor != ADVENTURER::end();
		    		AdvItor++)
		    	{
					DumbAutoLockPtr<CAvatar> const pAvatar(*AdvItor);
					pAvatar->attrib.IsFlying=1;
				}
				
				Item.mfMessage(STR_ACTIVATE_FLIGHT);
	
				// JPC if (pPlayerStats->hAvatar==CAvatar::mfGetPlayerAvatarHandle()) //this is kludgy
				player.Flying=TRUE;
					
	 				
			}
				break;
			case DEACTIVATE:
			{
				ADVENTURER_TEAM_ITOR AdvItor;
				for (AdvItor = ADVENTURER::begin(); AdvItor != ADVENTURER::end();
		    		AdvItor++)
		    	{
					DumbAutoLockPtr<CAvatar> const pAvatar(*AdvItor);
					pAvatar->attrib.IsFlying=0;
				}
	
				// JPC if (pPlayerStats->hAvatar==CAvatar::mfGetPlayerAvatarHandle()) //this is kludgy
				player.Flying=FALSE;

				Item.mfMessage(STR_DEACTIVATE_FLIGHT);
				
				mfRemoveActiveItem(ItemEffects::Flight,Combo,Arg);
			}
				break;
			default:
				return NOT_SUPPORTED;
		}
	}
	
	return OK;
}



LONG ItemEffects::RingFeatherFall(LONG Combo,LONG Arg,LONG action)
{
	const LONG FallDivider=6;
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
	{
		switch(action)
		{
			case ACTIVATE:
				break;
			case DEACTIVATE:
				break;
			case PICKUP:
				break;
			case DROP:
				break;

			case EQUIP:
			{
				DumbAutoLockPtr<PLAYER_STATS const>	const pPlayerStats(hPlayerStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbAutoLockPtr<CAvatar const> const pAvatar(pPlayerStats->hAvatar);
					
					if (pAvatar->attrib.IsFeathered)
						return CANT_COMPLETE;
	
					Item.mfMessage(STR_ACTIVATE_FEATHERFALL);
					mfSound(SND_GENERAL_MAGIC1,1,pAvatar->ThingIndex);
				}

				// [d12-03-96 JPC] Applied to whole adventure team.
				ADVENTURER_TEAM_ITOR AdvItor;
				for (AdvItor = ADVENTURER::begin(); AdvItor != ADVENTURER::end();
		    		AdvItor++)
		    	{
					DumbAutoLockPtr<CAvatar> const pAvatar(*AdvItor);
					pAvatar->attrib.IsFeathered=1;
				}
				PlayerFallRate = FALL_RATE / FallDivider;
				PlayerTermVelocity = TERMINAL_VELOCITY / FallDivider;
			}
				break;

			case UNEQUIP:
			{
				Item.mfMessage(STR_DEACTIVATE_FEATHERFALL);

				// [d12-03-96 JPC] Applied to whole adventure team.
				ADVENTURER_TEAM_ITOR AdvItor;
				for (AdvItor = ADVENTURER::begin(); AdvItor != ADVENTURER::end();
		    		AdvItor++)
		    	{
					DumbAutoLockPtr<CAvatar> const pAvatar(*AdvItor);
					pAvatar->attrib.IsFeathered=0;
				}
				PlayerFallRate = FALL_RATE;
				PlayerTermVelocity = TERMINAL_VELOCITY;
			}		  
				break;
		
			default:
				return NOT_SUPPORTED;
		}
	}
	
	return OK;
}
LONG ItemEffects::FeatherFall(LONG Combo,LONG Arg,LONG action)
{
	const LONG FallDivider=6;
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
	{
		switch(action)
		{
			case ACTIVATE:
			{

				DumbAutoLockPtr<PLAYER_STATS const>	const pPlayerStats(hPlayerStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbAutoLockPtr<CAvatar const> const pAvatar(pPlayerStats->hAvatar);
					
					if (pAvatar->attrib.IsFeathered)
						return CANT_COMPLETE;
					if (!mfAddActiveItem(ItemEffects::FeatherFall,Combo,Arg,Item.mfDuration(hPlayerStats)))
						return CANT_COMPLETE;
	
					Item.mfMessage(STR_ACTIVATE_FEATHERFALL);
					mfSound(SND_GENERAL_MAGIC1,1,pAvatar->ThingIndex);
				}

				// [d12-03-96 JPC] Applied to whole adventure team.
				ADVENTURER_TEAM_ITOR AdvItor;
				for (AdvItor = ADVENTURER::begin(); AdvItor != ADVENTURER::end();
		    		AdvItor++)
		    	{
					DumbAutoLockPtr<CAvatar> const pAvatar(*AdvItor);
					pAvatar->attrib.IsFeathered=1;
				}
				PlayerFallRate = FALL_RATE / FallDivider;
				PlayerTermVelocity = TERMINAL_VELOCITY / FallDivider;
			}
				break;
			case DEACTIVATE:
			{
				Item.mfMessage(STR_DEACTIVATE_FEATHERFALL);

				// [d12-03-96 JPC] Applied to whole adventure team.
				ADVENTURER_TEAM_ITOR AdvItor;
				for (AdvItor = ADVENTURER::begin(); AdvItor != ADVENTURER::end();
		    		AdvItor++)
		    	{
					DumbAutoLockPtr<CAvatar> const pAvatar(*AdvItor);
					pAvatar->attrib.IsFeathered=0;
				}
				PlayerFallRate = FALL_RATE;
				PlayerTermVelocity = TERMINAL_VELOCITY;
				mfRemoveActiveItem(FeatherFall,Combo,Arg);
			}
				break;
			default:
				return NOT_SUPPORTED;
		}
	}
	
	return OK;
}

LONG ItemEffects::RingInfravision(LONG Combo,LONG Arg,LONG action)
{
	ITEMTYPE		IType;
	SHORT			hPlayerStats;
	
 	SPLIT_COMBO(Combo,IType,hPlayerStats);
 	
	ItemType&		Item=ItemTypes[IType];

	ActiveItem* ai;

	if(fDomainTurn)
	{
		if (action == EQUIP)
			gfInfraVision=TRUE;
		else if (action == UNEQUIP)
			gfInfraVision=FALSE;
		else	
			return NOT_SUPPORTED;
	}
	if(fBattleCombat)
	{
		return NOT_SUPPORTED;
	}
	else	// adventure level
	{
		switch(action)
		{
			case PICKUP:
				break;
				
			case DROP:
				break;
				
			case EQUIP:
			{
				DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(hPlayerStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbAutoLockPtr<CAvatar const> const pAvatar(pPlayerStats->hAvatar);
	
					Item.mfMessage(STR_ACTIVATE_INFRAVISION);
					mfSound(SND_INFRAVISION1,1,pAvatar->ThingIndex);
				}

				gfInfraVision=TRUE;
				break;
			}
			case UNEQUIP:
			{
				Item.mfMessage(STR_DEACTIVATE_INFRAVISION);
				gfInfraVision=FALSE;
				break;
			}
			default:
				return NOT_SUPPORTED;
		}
	}
	return OK;
}
	

LONG ItemEffects::SeeInDark(LONG Combo,LONG Arg,LONG action)
{

	ITEMTYPE		Itemtype;
	SHORT			hPlayerStats;
	
 	SPLIT_COMBO(Combo,Itemtype,hPlayerStats);
 	
	ItemType&		Item=ItemTypes[Itemtype];
	
	ActiveItem* ai;

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
		switch(action)
		{
			case ACTIVATE:
			{
				if (!mfAddActiveItem(ItemEffects::SeeInDark,Combo,Arg,Item.mfDuration(hPlayerStats)))
					return CANT_COMPLETE;
					
				DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(hPlayerStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbAutoLockPtr<CAvatar const> const pAvatar(pPlayerStats->hAvatar);
	
					Item.mfMessage(STR_ACTIVATE_LIGHT);
					mfSound(SND_INFRAVISION1,1,pAvatar->ThingIndex);
				}

				gfTestMagicLightSwitch=TRUE;
				
				if (ItemTypes[Itemtype].mfCategory() == IC_MAGITEM)
				{
					ItemChargeHandler::mfDropCharge(Itemtype,hPlayerStats);
					return DONT_DROP;
				}
				break;
				
			}
			case DEACTIVATE:
			{
				Item.mfMessage(STR_DEACTIVATE_LIGHT);

				gfTestMagicLightSwitch=FALSE;
				mfRemoveActiveItem(ItemEffects::SeeInDark,Combo,Arg);
				break;
			}
			default:
				return NOT_SUPPORTED;
		}
	}
	
	return OK;

}


LONG ItemEffects::Speed(LONG Combo,LONG Arg,LONG Action)
{

	ITEMTYPE		IType;
	SHORT			hPlayerStats;
	
 	SPLIT_COMBO(Combo,IType,hPlayerStats);
 	
	ItemType&		Item=ItemTypes[IType];
	
	// if(fDomainTurn)
	// {
	// 	return NOT_SUPPORTED;
	// }
	// if(fBattleCombat)
	// {
	// 	return NOT_SUPPORTED;
	// }
	// else	// adventure level
	// ALL
	{
		switch(Action)
		{
			case ACTIVATE:
			{
				if (!mfAddActiveItem(ItemEffects::Speed,Combo,Arg,Item.mfDuration(hPlayerStats)))
					return CANT_COMPLETE;
					
				DumbAutoLockPtr<PLAYER_STATS>	const pPlayerStats(hPlayerStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbAutoLockPtr<CAvatar const> const pAvatar(pPlayerStats->hAvatar);
	
					Item.mfMessage(STR_ACTIVATE_SPEED);
	
					mfSound(SND_REDUCE1,1,pAvatar->ThingIndex); //sounds better than general magic sound
				}

				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::SPEED_MOD, +1);
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::ARMORCLASS_MOD, -2);
			}
				break;
			case DEACTIVATE:
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hPlayerStats);
				Item.mfMessage(STR_DEACTIVATE_SPEED);

				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::SPEED_MOD, -1);
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::ARMORCLASS_MOD, +2);
				mfRemoveActiveItem(ItemEffects::Speed,Combo,Arg);
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
	return OK; //everything went ok	
}
/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */
LONG ItemEffects::StrideAndSpring(LONG Combo,LONG Arg,LONG Action)
{
	static LONG OldJumpRate=PlayerJumpVelocity;
	
	SHORT Itemtype;  				//the index in ItemTypes of the Item
									//being activated
	SHORT hPlayerStats;   			//the stats of the avatar who's activating
									//the item
 	SPLIT_COMBO(Combo,Itemtype,hPlayerStats); //extract the data from the Combo

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
				DumbAutoLockPtr<PLAYER_STATS>	const pPlayerStats(hPlayerStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbAutoLockPtr<CAvatar const> const pAvatar(pPlayerStats->hAvatar);
	
					// [d12-03-96 JPC] Can't enhance jumping while it is
					// already enhanced.  This leads to permanent increase
					// in jumping ability as well as errors when the
					// first jumping ability spell or item wears off.
					if (pAvatar->attrib.IsJumpEnhanced)
						return CANT_COMPLETE;
					if (!mfAddActiveItem(ItemEffects::StrideAndSpring,Combo,Arg,Item.mfDuration(hPlayerStats)))
						return CANT_COMPLETE;
	
					Item.mfMessage(STR_ACTIVATE_SPRING);
	
					mfSound(SND_REDUCE1,1,pAvatar->ThingIndex); //sounds better than general magic sound
				}

				// [d12-03-96 JPC] Applied to whole adventure team.
				ADVENTURER_TEAM_ITOR AdvItor;
				for (AdvItor = ADVENTURER::begin(); AdvItor != ADVENTURER::end();
		    		AdvItor++)
		    	{
					DumbAutoLockPtr<CAvatar> const pAvatar(*AdvItor);
					pAvatar->attrib.IsJumpEnhanced=1;
		    		SHORT hPS = pAvatar->hPlayerStats;
		    		if (hPS < 0)
		    		    continue;
		    	
		    		DumbAutoLockPtr<PLAYER_STATS> pPS(hPS);
					pPS->StatsMod.mfModifyStats(STATSMOD::SPEED_MOD, +1);
				}

				OldJumpRate=PlayerJumpVelocity;
				SetPlayerJump(PlayerJumpVelocity*3,PlayerFallRate,PlayerTermVelocity);
			}
				break;
			case DEACTIVATE:
			{
				Item.mfMessage(STR_DEACTIVATE_SPRING);

				// [d12-03-96 JPC] Applied to whole adventure team.
				ADVENTURER_TEAM_ITOR AdvItor;
				for (AdvItor = ADVENTURER::begin(); AdvItor != ADVENTURER::end();
		    		AdvItor++)
		    	{
					DumbAutoLockPtr<CAvatar> const pAvatar(*AdvItor);
					pAvatar->attrib.IsJumpEnhanced=0;
		    		SHORT hPS = pAvatar->hPlayerStats;
		    		if (hPS < 0)
		    		    continue;
		    	
		    		DumbAutoLockPtr<PLAYER_STATS> const pPS(hPS);
					pPS->StatsMod.mfModifyStats(STATSMOD::SPEED_MOD, -1);
				}

				SetPlayerJump(OldJumpRate,PlayerFallRate,PlayerTermVelocity);
				mfRemoveActiveItem(ItemEffects::StrideAndSpring,Combo,Arg);
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
	return OK; //everything went ok	
}

LONG ItemEffects::KeyExplanation(LONG Combo,LONG Arg,LONG Action)
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
			Item.mfMessage(STR_ACTIVATE_ANY_KEY);
			return DONT_DROP;
		}
			
		case DEACTIVATE:
			break;
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

	return OK; //everything went ok	
}

/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */

LONG ItemEffects::BookOfExaltedDeeds(LONG Combo,LONG Arg,LONG Action)
{

	ITEMTYPE		IType;
	SHORT			hPlayerStats;
	
 	SPLIT_COMBO(Combo,IType,hPlayerStats);
 	
	ItemType&		Item=ItemTypes[IType];

	
	// if(fDomainTurn)
	// {
	// 	return NOT_SUPPORTED;
	// }
	// if(fBattleCombat)
	// {
	// 	return NOT_SUPPORTED;
	// }
	// else	// adventure level
	// ALL
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
					DumbAutoLockPtr<PLAYER_STATS>	const pPlayerStats(hPlayerStats);
				
					// only good priests get the benefit
					if(pPlayerStats->mfIsType(PLAYER_CLASS_INFO::PRIEST))
					{
						if (!ALIGNMENT_INFO::mfIsGood(pPlayerStats->mfGetAlignment()))
						{
							Item.mfMessage(STR_ACTIVATE_BOOK_WRONG);
							break;
						}
					
						if (!mfAddActiveItem(ItemEffects::BookOfExaltedDeeds,Combo,Arg,Item.mfDuration(hPlayerStats)))
							break;
							
						Item.mfMessage(STR_ACTIVATE_BOOK_EXHALTED);
						if (pPlayerStats->hAvatar != fERROR)
						{
							DumbAutoLockPtr<CAvatar const> const pAvatar(pPlayerStats->hAvatar);
							mfSound(SND_HOLY_WORD1,1,pAvatar->ThingIndex);
						}

						pPlayerStats->StatsMod.mfModifyStats(STATSMOD::WISDOM_MOD, +1);
						pPlayerStats->StatsMod.mfModifyStats(STATSMOD::LEVEL_MOD_PRIEST, +1);
					}
					else
					{
						Item.mfMessage(STR_ACTIVATE_BOOK_CONFUSE);
					}
				}
				break;
			case DROP:
			{
				DumbAutoLockPtr<PLAYER_STATS>	const pPlayerStats(hPlayerStats);
				if(pPlayerStats->mfIsType(PLAYER_CLASS_INFO::PRIEST))
				{
					if (!ALIGNMENT_INFO::mfIsGood(pPlayerStats->mfGetAlignment()))
						break;
						
					Item.mfMessage(STR_DEACTIVATE_BOOK_EXHALTED);
				
					pPlayerStats->StatsMod.mfModifyStats(STATSMOD::WISDOM_MOD, -1);
					pPlayerStats->StatsMod.mfModifyStats(STATSMOD::LEVEL_MOD_PRIEST, -1);
					mfRemoveActiveItem(ItemEffects::BookOfExaltedDeeds,Combo,Arg);
				}
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
/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */
LONG ItemEffects::BookOfVileDarkness(LONG Combo,LONG Arg,LONG Action)
{

	ITEMTYPE		IType;
	SHORT			hPlayerStats;
	
 	SPLIT_COMBO(Combo,IType,hPlayerStats);
 	
	ItemType&		Item=ItemTypes[IType];
	PLAYER_STATS	*pPlayerStats=(PLAYER_STATS*)BLKPTR(hPlayerStats);
	SHORT			hAvatar = pPlayerStats->hAvatar;

	
	// if(fDomainTurn)
	// {
	// 	return NOT_SUPPORTED;
	// }
	// if(fBattleCombat)
	// {
	// 	return NOT_SUPPORTED;
	// }
	// else	// adventure level
	// ALL
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
					DumbAutoLockPtr<PLAYER_STATS>	const pPlayerStats(hPlayerStats);
				
					// only priests get this spell
					//GEH ADD EVIL
					if(pPlayerStats->mfIsType(PLAYER_CLASS_INFO::PRIEST))
					{
						if (!ALIGNMENT_INFO::mfIsEvil(pPlayerStats->mfGetAlignment()))
						{
							Item.mfMessage(STR_ACTIVATE_BOOK_WRONG);
							break;
						}
					
						if (!mfAddActiveItem(ItemEffects::BookOfVileDarkness,Combo,Arg,Item.mfDuration(hPlayerStats)))
							break;
							
						Item.mfMessage(STR_ACTIVATE_BOOK_VILE);
						if (pPlayerStats->hAvatar != fERROR)
						{
							DumbAutoLockPtr<CAvatar const > const pAvatar(pPlayerStats->hAvatar);
							mfSound(SND_POWER_KILL1,1,pAvatar->ThingIndex);
						}

						pPlayerStats->StatsMod.mfModifyStats(STATSMOD::WISDOM_MOD, +1);
						pPlayerStats->StatsMod.mfModifyStats(STATSMOD::LEVEL_MOD_PRIEST, +1);
					}
					else
					{
						Item.mfMessage(STR_ACTIVATE_BOOK_CONFUSE);
					}
				}
				break;
			case DROP:
			{
				DumbAutoLockPtr<PLAYER_STATS>	const pPlayerStats(hPlayerStats);
				if(pPlayerStats->mfIsType(PLAYER_CLASS_INFO::PRIEST))
				{
					if (!ALIGNMENT_INFO::mfIsEvil(pPlayerStats->mfGetAlignment()))
						break;
					
					Item.mfMessage(STR_DEACTIVATE_BOOK_VILE);
				
					pPlayerStats->StatsMod.mfModifyStats(STATSMOD::WISDOM_MOD, -1);
					pPlayerStats->StatsMod.mfModifyStats(STATSMOD::LEVEL_MOD_PRIEST, -1);
					mfRemoveActiveItem(ItemEffects::BookOfVileDarkness,Combo,Arg);
				}
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








/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */

LONG ItemEffects::CloakProtect(LONG Combo,LONG Arg,LONG Action)
{

	SHORT			ac_mod;
	SHORT			max_ac;
	SHORT			cur_ac;
	ITEMTYPE		IType;
	SHORT			hPlayerStats;
	
 	SPLIT_COMBO(Combo,IType,hPlayerStats);
 	
	ItemType&		Item=ItemTypes[IType];

	// works in all modes
	{
		switch(Action)
		{
			case ACTIVATE:
				Item.mfMessage(STR_ACTIVATE_CLOAK_PROTECT);
				return DONT_DROP;
				
			case DEACTIVATE:
				break;
			
			case PICKUP:
			{
			}
				break;
				
			case DROP:
			{
			}
				break;
			case EQUIP:
			{
				DumbAutoLockPtr<PLAYER_STATS>	const pPlayerStats(hPlayerStats);
				LONG ProtLevel=(SHORT)(Arg & 0x0000FFFF);
				LONG ProtType=(SHORT)(Arg & 0xFFFF0000);

				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::ARMORCLASS_MOD, -ProtLevel);
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::SAVES_MOD, ProtLevel);
				
			}
				break;
			case UNEQUIP:
			{
				DumbAutoLockPtr<PLAYER_STATS>	const pPlayerStats(hPlayerStats);
				LONG ProtLevel=(SHORT)(Arg & 0x0000FFFF);
				LONG ProtType=(SHORT)(Arg & 0xFFFF0000);

				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::ARMORCLASS_MOD, ProtLevel);
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::SAVES_MOD, -ProtLevel);
				
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
/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */
LONG ItemEffects::GauntletsOfDex(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE		IType;
	SHORT			hPlayerStats;
	
 	SPLIT_COMBO(Combo,IType,hPlayerStats);
 	
	ItemType&		Item=ItemTypes[IType];
	// if(fDomainTurn)
	// {
	//	return NOT_SUPPORTED;
	// }
	// if(fBattleCombat)
	// {
	//	return NOT_SUPPORTED;
	// }
	// else	// adventure level
	// works in all modes
	{
		switch(Action)
		{
			case ACTIVATE:
				Item.mfMessage(STR_ACTIVATE_GAUNTLETS_DEX);
				return DONT_DROP;
				
			case DEACTIVATE:
				break;
			case PICKUP:
				break;
			case DROP:
				break;
			case EQUIP:
			{
				PLAYER_STATS * const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::DEXTERITY_MOD, 1);
				pPlayerStats->StatsMod.mfSetStats(STATSMOD::DEXTERITY_SET, 15);
			}
				break;
			case UNEQUIP:
			{
				PLAYER_STATS * const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				pPlayerStats->StatsMod.mfSetStats(STATSMOD::DEXTERITY_SET, 0);
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::DEXTERITY_MOD, -1);
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

/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */
LONG ItemEffects::GuantletsOfOgrePow(LONG Combo,LONG Arg,LONG Action)
{

	ITEMTYPE		IType;
	SHORT			hPlayerStats;
	
 	SPLIT_COMBO(Combo,IType,hPlayerStats);
 	
	ItemType&		Item=ItemTypes[IType];
	
	// if(fDomainTurn)
	// {
	//	return NOT_SUPPORTED;
	// }
	// if(fBattleCombat)
	// {
	//	return NOT_SUPPORTED;
	// }
	// else	// adventure level
	// works in all modes
	{
		switch(Action)
		{
			case ACTIVATE:
				Item.mfMessage(STR_ACTIVATE_GAUNTLETS_DEX);
				return DONT_DROP;
				
			case DEACTIVATE:
				break;
			case PICKUP:
				break;
			case DROP:
				break;
			case EQUIP:
			{
				PLAYER_STATS * const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				if (pPlayerStats->mfIsType(PLAYER_CLASS_INFO::FIGHTER) ||
					pPlayerStats->mfIsType(PLAYER_CLASS_INFO::PALADIN) ||
					pPlayerStats->mfIsType(PLAYER_CLASS_INFO::RANGER) ||
					pPlayerStats->mfIsType(PLAYER_CLASS_INFO::THIEF) ||
					pPlayerStats->mfIsType(PLAYER_CLASS_INFO::BARD) ||
					pPlayerStats->mfIsType(PLAYER_CLASS_INFO::PRIEST))
					pPlayerStats->StatsMod.mfSetStats(STATSMOD::STRENGTH_SET, 23);
			}
				break;
			case UNEQUIP:
			{
				PLAYER_STATS * const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				if (pPlayerStats->mfIsType(PLAYER_CLASS_INFO::FIGHTER) ||
					pPlayerStats->mfIsType(PLAYER_CLASS_INFO::PALADIN) ||
					pPlayerStats->mfIsType(PLAYER_CLASS_INFO::RANGER) ||
					pPlayerStats->mfIsType(PLAYER_CLASS_INFO::THIEF) ||
					pPlayerStats->mfIsType(PLAYER_CLASS_INFO::BARD) ||
					pPlayerStats->mfIsType(PLAYER_CLASS_INFO::PRIEST))
					pPlayerStats->StatsMod.mfSetStats(STATSMOD::STRENGTH_SET, 0);
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

struct AddSpellCombo
{
	SBYTE	fRealm;
	SBYTE	Number;
	SBYTE	LowBoundLevel;
	SBYTE	HighBoundLevel;
};


static ITEMTYPE GetRandomSpellItem(LONG lo,LONG hi)
{
	LONG SpellLevel=0;
	ITEMTYPE ItemTypeIdx;

	do
	{
		LONG Spell=random(SPELL_INFO::LAST_WIZARD_SPELL+1);
		SpellLevel=SPELL_INFO::mfGetSpell((SPELL_INFO::TYPE)Spell).Level;

		ItemTypeIdx=SpellTypeToItemTypeIdx((SPELL_INFO::TYPE)Spell);
	
	} while (SpellLevel<lo || SpellLevel>hi || ItemTypeIdx==MAX_ITEM_TYPES);

	return ItemTypeIdx;
}

static ITEMTYPE GetRandomRealmSpellItem()
{

   	LONG RSpell=random(ITEM_SPELL_WARDING-ITEM_SPELL_ALCHEMY+1);
	ITEMTYPE ItemTypeIdx=(ITEMTYPE)(ITEM_SPELL_ALCHEMY+RSpell);
	
	return ItemTypeIdx;
}



LONG ItemEffects::TomeAddRandomSpells(LONG Combo,LONG Arg,LONG Action)
{
	SHORT Itemtype;  				//the index in ItemTypes of the Item
									//being activated
	SHORT hPlayerStats;   			//the stats of the avatar who's activating
									//the item
 	SPLIT_COMBO(Combo,Itemtype,hPlayerStats); //extract the data from the Combo

	ItemType& Item=ItemTypes[Itemtype]; //A reference to the item

	AddSpellCombo ASC;
	ASC.fRealm=Arg>>24;
	ASC.Number=(Arg>>16)&0xFF;
	ASC.LowBoundLevel=(Arg>>8)&0xFF;
	ASC.HighBoundLevel=(Arg)&0xFF;


//	if(fDomainTurn)
//	{
//	return NOT_SUPPORTED;
//	}
//	if(fBattleCombat)
//	{
//	return NOT_SUPPORTED;
//	}
//	else	// adventure level
	{
		switch(Action)
		{
			case ACTIVATE:
			{
				
				DumbAutoLockPtr< PLAYER_STATS > const pPlayerStats(hPlayerStats);
				ObjectList&  const Ol=pPlayerStats->mfGetInventory();


				if (pPlayerStats->mfGetSpellCastClass(SPELL_INFO::WIZARD)==CANT_CAST_SPELLS)
				{
					Item.mfMessage(STR_PRIEST_USING_TOME_RANDOM);
					return DONT_DROP;
				}
				
				printf("pASC Num:%li  Low:%li  Hi:%li fRealm:%li\n",ASC.Number,ASC.LowBoundLevel,ASC.HighBoundLevel,ASC.fRealm);

				if (!ASC.fRealm)
				{
	

					//this tries twice, and if it gets two dupes in a row, gives up
					for (LONG i=0;i<ASC.Number;i++)
						if (Ol.mfAddItem(GetRandomSpellItem(ASC.LowBoundLevel,ASC.HighBoundLevel))&ADD_DUP)
							Ol.mfAddItem(GetRandomSpellItem(ASC.LowBoundLevel,ASC.HighBoundLevel));
				}
				else
				{
//untested
					if (Ol.mfAddItem(GetRandomRealmSpellItem())&ADD_DUP)
						Ol.mfAddItem(GetRandomRealmSpellItem());
					
				}
				
				Item.mfMessage(STR_ACTIVATE_TOME_RANDOM);

			}
			
				break;
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
LONG ItemEffects::LibrOfConjuration(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE		IType;
	SHORT			hPlayerStats;
	
 	SPLIT_COMBO(Combo,IType,hPlayerStats);
 	
	ItemType&		Item=ItemTypes[IType];
	
//	if(fDomainTurn)
//	{
//		return NOT_SUPPORTED;
//	}
//	if(fBattleCombat)
//	{
//		return NOT_SUPPORTED;
//	}
//	else	/ adventure level
	
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
				// only the swiss get this one (Neutral)
				//ho ho ho
				DumbAutoLockPtr<PLAYER_STATS>	const pPlayerStats(hPlayerStats);
				
				if(pPlayerStats->mfIsType(PLAYER_CLASS_INFO::WIZARD))
				{
					if (!ALIGNMENT_INFO::mfIsNeutral(pPlayerStats->mfGetAlignment()))
					{
						Item.mfMessage(STR_ACTIVATE_BOOK_WRONG);
						break;
					}
					
					if (!mfAddActiveItem(ItemEffects::LibrOfConjuration,Combo,Arg,Item.mfDuration(hPlayerStats)))
						break;
						
					Item.mfMessage(STR_ACTIVATE_LIBRAM_GAINFUL);
					if (pPlayerStats->hAvatar != fERROR)
					{
						DumbAutoLockPtr<CAvatar const > const pAvatar(pPlayerStats->hAvatar);
						mfSound(SND_HOLY_WORD1,1,pAvatar->ThingIndex);
					}
					pPlayerStats->StatsMod.mfModifyStats(STATSMOD::LEVEL_MOD_WIZARD, +1);
				}
				else
				{
						Item.mfMessage(STR_ACTIVATE_BOOK_CONFUSE);
				}
			}
				break;
			case DROP:
			{
				DumbAutoLockPtr<PLAYER_STATS>	const pPlayerStats(hPlayerStats);

				if(pPlayerStats->mfIsType(PLAYER_CLASS_INFO::WIZARD))
				{
					if (!ALIGNMENT_INFO::mfIsNeutral(pPlayerStats->mfGetAlignment()))
						break;
						
					Item.mfMessage(STR_DEACTIVATE_LIBRAM_GAINFUL);
					pPlayerStats->StatsMod.mfModifyStats(STATSMOD::LEVEL_MOD_WIZARD, -1);
					mfRemoveActiveItem(ItemEffects::LibrOfConjuration,Combo,Arg);
				}
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
LONG ItemEffects::LibrOfDamnation(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE		IType;
	SHORT			hPlayerStats;
	
 	SPLIT_COMBO(Combo,IType,hPlayerStats);
 	
	ItemType&		Item=ItemTypes[IType];
	
//	if(fDomainTurn)
//	{
//		return NOT_SUPPORTED;
//	}
//	if(fBattleCombat)
//	{
//		return NOT_SUPPORTED;
//	}
//	else
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
				DumbAutoLockPtr<PLAYER_STATS>	const pPlayerStats(hPlayerStats);
				
				// Bad boy, bad boy, what'cha gonna do? (Evil)
				// watcha gonna do when they fireball you?
				if(pPlayerStats->mfIsType(PLAYER_CLASS_INFO::WIZARD))
				{
					if (!ALIGNMENT_INFO::mfIsEvil(pPlayerStats->mfGetAlignment()))
					{
						Item.mfMessage(STR_ACTIVATE_BOOK_WRONG);
						break;
					}
					
					if (!mfAddActiveItem(ItemEffects::LibrOfDamnation,Combo,Arg,Item.mfDuration(hPlayerStats)))
						break;
						
					Item.mfMessage(STR_ACTIVATE_LIBRAM_INEFFABLE);
					if (pPlayerStats->hAvatar != fERROR)
					{
						DumbAutoLockPtr<CAvatar const> const pAvatar(pPlayerStats->hAvatar);
						mfSound(SND_REALM_DEATH_PLAGUE1,1,pAvatar->ThingIndex);
					}
					pPlayerStats->StatsMod.mfModifyStats(STATSMOD::LEVEL_MOD_WIZARD, +1);
				}
				else
				{
						Item.mfMessage(STR_ACTIVATE_BOOK_CONFUSE);
				}
			}
				break;
			case DROP:
			{
				DumbAutoLockPtr<PLAYER_STATS>	const pPlayerStats(hPlayerStats);
				if(pPlayerStats->mfIsType(PLAYER_CLASS_INFO::WIZARD))
				{
					if (!ALIGNMENT_INFO::mfIsEvil(pPlayerStats->mfGetAlignment()))
						break;
					
					Item.mfMessage(STR_ACTIVATE_LIBRAM_INEFFABLE);
					pPlayerStats->StatsMod.mfModifyStats(STATSMOD::LEVEL_MOD_WIZARD, -1);
					mfRemoveActiveItem(ItemEffects::LibrOfDamnation,Combo,Arg);
				}
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
LONG ItemEffects::LibrSilver(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE		IType;
	SHORT			hPlayerStats;
	
 	SPLIT_COMBO(Combo,IType,hPlayerStats);
 	
	ItemType&		Item=ItemTypes[IType];
	
//	if(fDomainTurn)
//	{
//		return NOT_SUPPORTED;
//	}
//	else if(fBattleCombat)
//	{
//		return NOT_SUPPORTED;
//	}
//	else
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
				DumbAutoLockPtr<PLAYER_STATS>	const pPlayerStats(hPlayerStats);
				
				// Goodie two shoes get this one (Good)
				if(pPlayerStats->mfIsType(PLAYER_CLASS_INFO::WIZARD))
				{
					if (!ALIGNMENT_INFO::mfIsGood(pPlayerStats->mfGetAlignment()))
					{
						Item.mfMessage(STR_ACTIVATE_BOOK_WRONG);
						break;
					}
					
					if (!mfAddActiveItem(ItemEffects::LibrSilver,Combo,Arg,Item.mfDuration(hPlayerStats)))
						break;
						
					Item.mfMessage(STR_ACTIVATE_LIBRAM_SILVER);
					if (pPlayerStats->hAvatar != fERROR)
					{
						DumbAutoLockPtr<CAvatar const> const pAvatar(pPlayerStats->hAvatar);
						mfSound(SND_GENERAL_MAGIC1,1,pAvatar->ThingIndex);
					}

					pPlayerStats->StatsMod.mfModifyStats(STATSMOD::LEVEL_MOD_WIZARD, +1);
				}
				else
				{
						Item.mfMessage(STR_ACTIVATE_BOOK_CONFUSE);
				}
			}
				break;
			case DROP:
			{
				DumbAutoLockPtr<PLAYER_STATS>	const pPlayerStats(hPlayerStats);
				
				if(pPlayerStats->mfIsType(PLAYER_CLASS_INFO::WIZARD))
				{
					if (!ALIGNMENT_INFO::mfIsGood(pPlayerStats->mfGetAlignment()))
						break;
						
					Item.mfMessage(STR_DEACTIVATE_LIBRAM_SILVER);
					pPlayerStats->StatsMod.mfModifyStats(STATSMOD::LEVEL_MOD_WIZARD, -1);
					mfRemoveActiveItem(ItemEffects::LibrSilver,Combo,Arg);
				}
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
/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */



LONG ItemEffects::Parchment(LONG Combo,LONG Arg,LONG Action)
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
		return NOT_SUPPORTED;
	}
	else	// adventure level
	{
		switch(Action)
		{
			case ACTIVATE:
				return DONT_DROP;
			
			case DEACTIVATE:
				break;
			case PICKUP:
			{
				DumbAutoLockPtr<PLAYER_STATS>	const pPlayerStats(hPlayerStats);
				GAME_STRING noteString;
				
				if (SCENE_MGR::hCurrentScene == fERROR)
				    return DONT_PICKUP;
				
				DumbAutoLockPtr<SCENE const>	const pCS(SCENE_MGR::hCurrentScene);

				noteString = pCS->mfGetNote(Arg);
				
				if (noteString < STR_NOTE_HEADER)
				{
#if defined(_DEBUG)				
					fatal_error("Illegal Note ID");
#endif				
					return DONT_PICKUP;
				}
				
				InitializeParchmentMenu(noteString);
				
				return DONT_PICKUP;
			}
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



LONG ItemEffects::GoldCoins(LONG Combo, LONG Arg, LONG Action)
{
	SHORT Itemtype;  				//the index in ItemTypes of the Item
									//being activated
	SHORT hPlayerStats;   			//the stats of the avatar who's activating
									//the item
 	SPLIT_COMBO(Combo,Itemtype,hPlayerStats); //extract the data from the Combo

	
	if(fDomainTurn)
	{
		return NOT_SUPPORTED;
	}
	
	if(fBattleCombat)
	{
		return NOT_SUPPORTED;
	}
	
	switch(Action)
	{
		case ACTIVATE:
			return NOT_SUPPORTED;
		case DEACTIVATE:
			return NOT_SUPPORTED;
		case PICKUP:
			{
				DumbAutoLockPtr<PLAYER_STATS>	const pPlayerStats(hPlayerStats);
			
				pPlayerStats->mfAddGold(Arg);	// Add in the amount of gold.
				return DONT_ADD;  //this just signifies mfAddItem to not
								  //add it to the inventory list
			}
		case DROP:
			return NOT_SUPPORTED;
		case EQUIP:
			return NOT_SUPPORTED;
		case UNEQUIP:
			return NOT_SUPPORTED;
		case SERVICE:
			return NOT_SUPPORTED;
		default:
			return NOT_SUPPORTED;
	}
}

LONG ItemEffects::RingJump(LONG Combo,LONG Arg,LONG Action)
{
	static EffectSeparator<LONG> OldJumpRates;
	
	SHORT Itemtype;  				//the index in ItemTypes of the Item
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
				break;
			case DEACTIVATE:
				break;
			case PICKUP:
				break;
			case DROP:
				break;
			case EQUIP:
			{
				DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(hAvatarStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbAutoLockPtr<CAvatar const> const pAvatar(pPlayerStats->hAvatar);
	
					// [d12-02-96 JPC] Can't enhance jumping while it is
					// already enhanced.  This leads to permanent increase
					// in jumping ability as well as errors when the
					// first jumping ability spell or item wears off.
					if (pAvatar->attrib.IsJumpEnhanced)
						return CANT_COMPLETE;
	
	
					Item.mfMessage(STR_ACTIVATE_JUMPx3);
	
					mfSound(SND_GENERAL_MAGIC1,NULL,pAvatar->ThingIndex);
				}

				OldJumpRates.mfAddEffect(hAvatarStats, PlayerJumpVelocity);
				if (Arg <= 0)
					SetPlayerJump(PlayerJumpVelocity*3,PlayerFallRate,PlayerTermVelocity);
				else
					SetPlayerJump(PlayerJumpVelocity*Arg,PlayerFallRate,PlayerTermVelocity);

				// [d12-03-96 JPC] Applied to whole adventure team.
				ADVENTURER_TEAM_ITOR AdvItor;
				for (AdvItor = ADVENTURER::begin(); AdvItor != ADVENTURER::end();
		    		AdvItor++)
		    	{
					DumbAutoLockPtr<CAvatar> const pAvatar(*AdvItor);
					pAvatar->attrib.IsJumpEnhanced = 1;
				}
			}
				break;
			case UNEQUIP:
			{

				DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(hAvatarStats);

				Item.mfMessage(STR_DEACTIVATE_JUMPx3);
				
				LONG const * pOldJumpRate=OldJumpRates.mfGetVal(hAvatarStats);

				if (pOldJumpRate)
				{
					SetPlayerJump(*pOldJumpRate,PlayerFallRate,PlayerTermVelocity);
					OldJumpRates.mfRemoveEffect(hAvatarStats);
				}
#ifdef DEBUG
				else
					fatal_error("EffectSeparator::mfGetVal returned NULL  %s(%li)\n",__FILE__,__LINE__);
#endif


				// [d12-03-96 JPC] Applied to whole adventure team.
				ADVENTURER_TEAM_ITOR AdvItor;
				for (AdvItor = ADVENTURER::begin(); AdvItor != ADVENTURER::end();
		    		AdvItor++)
		    	{
					DumbAutoLockPtr<CAvatar> const pAvatar(*AdvItor);
					pAvatar->attrib.IsJumpEnhanced = 0;
				}
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


LONG ItemEffects::RingResistance(LONG Combo,LONG Arg,LONG Action)
{
	STATSMOD::MOD_TYPE	type;
	LONG				mod;

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
				return DONT_DROP;
				
			case DEACTIVATE:
				break;
			case PICKUP:
				break;
			case DROP:
				break;
				
			case EQUIP:
			{
				PLAYER_STATS	* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				
				mod = 1;
				if(Arg&_RESIST_FIRE)
				{
					type = STATSMOD::FIRE_RESIST_MOD;
				}
				else
				if(Arg&_RESIST_COLD)
				{
					type = STATSMOD::COLD_RESIST_MOD;
				}
				else
				if(Arg&_RESIST_EVIL)
				{
					type = STATSMOD::EVIL_RESIST_MOD;
				}
				else
				if(Arg&_RESIST_MAGIC)
				{
					type = STATSMOD::MAGIC_RESIST_MOD;
					mod = (SBYTE)(Arg&0x0000FFFF);
				}
				else
				if(Arg&_RESIST_FEAR)
				{
					type = STATSMOD::FEAR_RESIST_MOD;
				}
				pPlayerStats->StatsMod.mfModifyStats(type, mod);
			}
				break;
			case UNEQUIP:
			{
				PLAYER_STATS	* const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
	
				mod = 1;
				if(Arg&_RESIST_FIRE)
				{
					type = STATSMOD::FIRE_RESIST_MOD;
				}
				else
				if(Arg&_RESIST_COLD)
				{
					type = STATSMOD::COLD_RESIST_MOD;
				}
				else
				if(Arg&_RESIST_EVIL)
				{
					type = STATSMOD::EVIL_RESIST_MOD;
				}
				else
				if(Arg&_RESIST_MAGIC)
				{
					type = STATSMOD::MAGIC_RESIST_MOD;
					mod = (SBYTE)(Arg&0x0000FFFF);
				}
				else
				if(Arg&_RESIST_FEAR)
				{
					type = STATSMOD::FEAR_RESIST_MOD;
				}
 				pPlayerStats->StatsMod.mfModifyStats(type, -mod);
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

