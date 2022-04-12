//FUNCS THAT NEED IMPLEMENTING FOR SCENE
//TimeStopSpell
//WaterWalk

/* ========================================================================
   Copyright (c) 1990,1996   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: SpelType.cpp -Effects for Spells
   Author: Wes Cumberland
   ======================================================================== */
#include "AVATAR.HXX"
#include "BATTLE.HXX"
#include "BATTLEUI.HXX"
#include "BLOOD.HXX"
#include "DICE.H"
#include "EFFSEPAR.HXX"
#include "ITEMTYPE.HXX"
#include "INVNTORY.HXX"
#include "PLAYER.HXX"
#include "PLAYSTAT.HXX"
#include "RESURECT.HXX"
#include "SCENE.HXX"
#include "SPELL.HXX"
#include "strenum.h"
#include "TMSGBOX.HXX"


// from aibtlcap.cpp
extern BOOL fBtlTurnUndead;

//this is for the spell Knock.  it is externed in doors.c
BOOL fSpellKnocking=FALSE;

LONG ItemEffects::Jump(LONG Combo,LONG Arg,LONG Action)
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
			{
				DumbAutoLockPtr<PLAYER_STATS const > const pPlayerStats(hAvatarStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
	
					// [d12-02-96 JPC] Can't enhance jumping while it is
					// already enhanced.  This leads to permanent increase
					// in jumping ability as well as errors when the
					// first jumping ability spell or item wears off.
					if (pAvatar->attrib.IsJumpEnhanced)
						return CANT_COMPLETE;
	
					if (!mfAddActiveItem(Jump,Combo,Arg,Item.mfDuration(hAvatarStats)))
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
					DumbHandlePtr<CAvatar> const pAvatar(*AdvItor);
					pAvatar->attrib.IsJumpEnhanced = 1;
				}
			}
				break;
			case DEACTIVATE:
			{

				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);

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
					DumbHandlePtr<CAvatar> const pAvatar(*AdvItor);
					pAvatar->attrib.IsJumpEnhanced = 0;
				}
				mfRemoveActiveItem(Jump,Combo,Arg);
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

LONG ItemEffects::Strength(LONG Combo,LONG Arg,LONG Action)
{
	SHORT Itemtype;  				//the index in ItemTypes of the Item
									//being activated
	SHORT hAvatarStats;   			//the stats of the avatar who's activating
									//the item
 	SPLIT_COMBO(Combo,Itemtype,hAvatarStats); //extract the data from the Combo

	ItemType& Item=ItemTypes[Itemtype]; //A reference to the item


	static EffectSeparator<LONG> Mods;
	
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
				if (!mfAddActiveItem(ItemEffects::Strength,Combo,Arg,Item.mfDuration(hAvatarStats)))
					return CANT_COMPLETE;
	
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
				DICE dice= {1,4,0};
				
				if (pPlayerStats->mfIsType(PLAYER_CLASS_INFO::FIGHTER) ||
					pPlayerStats->mfIsType(PLAYER_CLASS_INFO::PALADIN) ||
					pPlayerStats->mfIsType(PLAYER_CLASS_INFO::RANGER))
				{
					dice.sbNumberOfSides = 8;
				}
				else
				if (pPlayerStats->mfIsType(PLAYER_CLASS_INFO::PRIEST) ||
					pPlayerStats->mfIsType(PLAYER_CLASS_INFO::THIEF) ||
					pPlayerStats->mfIsType(PLAYER_CLASS_INFO::BARD))
				{
					dice.sbNumberOfSides = 6;
				}
				
				LONG StrMod=RollDice(&dice);

				Item.mfMessage(STR_ACTIVATE_STRENGTH);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
					mfSound(SND_STRENGTH1,NULL,pAvatar->ThingIndex);
				}

				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::STRENGTH_MOD,StrMod);
	
				Mods.mfAddEffect(hAvatarStats,StrMod);
			}
				break;
			case DEACTIVATE:
			{
				
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
				LONG StrMod=*Mods.mfGetVal(hAvatarStats);

				Item.mfMessage(STR_DEACTIVATE_STRENGTH);

				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::STRENGTH_MOD,-StrMod);
				Mods.mfRemoveEffect(hAvatarStats);
				mfRemoveActiveItem(ItemEffects::Strength,Combo,Arg);
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

LONG ItemEffects::StoneSkin(LONG Combo,LONG Arg,LONG Action)
{
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
			{
				if (!mfAddActiveItem(ItemEffects::StoneSkin,Combo,Arg,Item.mfDuration(hAvatarStats)))
					return CANT_COMPLETE;

				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);

				Item.mfMessage(STR_ACTIVATE_STONESKIN);

				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
					mfSound(SND_STONESKIN1,NULL,pAvatar->ThingIndex);
				}
				pPlayerStats->StatsMod.mfSetStats(STATSMOD::ARMORCLASS_SET,2);

			}
				break;
			case DEACTIVATE:
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);

				Item.mfMessage(STR_DEACTIVATE_STONESKIN);

				pPlayerStats->StatsMod.mfSetStats(STATSMOD::ARMORCLASS_SET,0);
				mfRemoveActiveItem(ItemEffects::StoneSkin,Combo,Arg);
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


LONG ItemEffects::BarkSkin(LONG Combo,LONG Arg,LONG Action)
{
	SHORT Itemtype;  				//the index in ItemTypes of the Item
									//being activated
	SHORT hAvatarStats;   			//the stats of the avatar who's activating
									//the item
 	SPLIT_COMBO(Combo,Itemtype,hAvatarStats); //extract the data from the Combo

	ItemType& Item=ItemTypes[Itemtype]; //A reference to the item


	static EffectSeparator<LONG> OldACs;
	
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
				if (!mfAddActiveItem(ItemEffects::BarkSkin,Combo,Arg,Item.mfDuration(hAvatarStats)))
					return CANT_COMPLETE;

				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);

				LONG 			PriestClass=CANT_CAST_SPELLS;
				LONG 			PriestLevel=0;
				LONG 			ACSet=0;

				Item.mfMessage(STR_ACTIVATE_BARKSKIN);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
					mfSound(SND_GENERAL_MAGIC1,NULL,pAvatar->ThingIndex);
				}
				
				PriestClass=pPlayerStats->mfGetSpellCastClass(SPELL_INFO::PRIEST);
				if (PriestClass==CANT_CAST_SPELLS)
					return CANT_COMPLETE;

				PriestLevel=pPlayerStats->mfGetLevel(PriestClass);
//				ACSet=6-PriestLevel/4;
//				if (ACSet<0)
//					ACSet=0;
//
//				OldACs.mfAddEffect(hAvatarStats, pPlayerStats->StatsMod.mfSetStats(STATSMOD::ARMORCLASS_SET,ACSet));

				pPlayerStats->StatsMod.mfSetStats(STATSMOD::ARMORCLASS_SET, 6);
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::ARMORCLASS_MOD, -PriestLevel/4);
			}
				break;
			case DEACTIVATE:
			{
				LONG ACSet;
				LONG 			PriestClass=CANT_CAST_SPELLS;
				LONG 			PriestLevel=0;

				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);

				Item.mfMessage(STR_DEACTIVATE_BARKSKIN);
	
				PriestClass=pPlayerStats->mfGetSpellCastClass(SPELL_INFO::PRIEST);
				PriestLevel=pPlayerStats->mfGetLevel(PriestClass);
				
//				ACSet=*OldACs.mfGetVal(hAvatarStats);
//				pPlayerStats->StatsMod.mfSetStats(STATSMOD::ARMORCLASS_SET,ACSet);
//				OldACs.mfRemoveEffect(hAvatarStats);
				
				pPlayerStats->StatsMod.mfSetStats(STATSMOD::ARMORCLASS_SET, 0);
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::ARMORCLASS_MOD, PriestLevel/4);
				
				mfRemoveActiveItem(ItemEffects::BarkSkin,Combo,Arg);
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

LONG ItemEffects::MagicalVestement(LONG Combo,LONG Arg,LONG Action)
{
	SHORT Itemtype;  				//the index in ItemTypes of the Item
									//being activated
	SHORT hAvatarStats;   			//the stats of the avatar who's activating
									//the item
 	SPLIT_COMBO(Combo,Itemtype,hAvatarStats); //extract the data from the Combo

	ItemType& Item=ItemTypes[Itemtype]; //A reference to the item



	static EffectSeparator<LONG> OldACs;
	
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
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);

				LONG PriestClass=CANT_CAST_SPELLS;
				LONG PriestLevel=0;
				LONG ACSet=0;


				PriestClass=pPlayerStats->mfGetSpellCastClass(SPELL_INFO::PRIEST);
				if (PriestClass==CANT_CAST_SPELLS)
					return CANT_COMPLETE;

				PriestLevel=pPlayerStats->mfGetLevel(PriestClass);
				
				
				if (!mfAddActiveItem(ItemEffects::MagicalVestement,Combo,Arg,Item.mfDuration(hAvatarStats)))
					return CANT_COMPLETE;

				Item.mfMessage(STR_ACTIVATE_MAGICALVEST);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
					mfSound(SND_GENERAL_MAGIC1,NULL,pAvatar->ThingIndex);
				}

				pPlayerStats->StatsMod.mfSetStats(STATSMOD::ARMORCLASS_SET, 5);
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::ARMORCLASS_MOD, -PriestLevel/3);
				
				

			}
				break;
			case DEACTIVATE:
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);

				LONG const *pACSet=0;
				LONG PriestClass=CANT_CAST_SPELLS;
				LONG PriestLevel=0;

				
				Item.mfMessage(STR_DEACTIVATE_MAGICALVEST);
				

				PriestClass=pPlayerStats->mfGetSpellCastClass(SPELL_INFO::PRIEST);
				PriestLevel=pPlayerStats->mfGetLevel(PriestClass);
				
//				pACSet=OldACs.mfGetVal(hAvatarStats);
//				if (!pACSet)
//				{
//#ifdef DEBUG
//					fatal_error("BadVal from GetVal %s(%li)\n",__FILE__,__LINE__);
//#endif
//					return CANT_COMPLETE;
//				}
//				
//				pPlayerStats->StatsMod.mfSetStats(STATSMOD::ARMORCLASS_SET,*pACSet);
//				OldACs.mfRemoveEffect(hAvatarStats);
				
				pPlayerStats->StatsMod.mfSetStats(STATSMOD::ARMORCLASS_SET, 0);
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::ARMORCLASS_MOD, PriestLevel/3);
				
				mfRemoveActiveItem(ItemEffects::MagicalVestement,Combo,Arg);
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

LONG ItemEffects::Shrink(LONG Combo,LONG Arg,LONG Action)
{
	
	ITEMTYPE		IType;
	SHORT			hAvatarStats;
	
 	SPLIT_COMBO(Combo,IType,hAvatarStats);
 	
	ItemType&		Item=ItemTypes[IType];

	EffectSeparator<LONG> OldScales;
	EffectSeparator<LONG> OldW;
	EffectSeparator<LONG> OldH;
	
	static BOOL fAlreadyShrunk=FALSE;

	

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
				if (fAlreadyShrunk)
				{
					Item.mfMessage(STR_ITEM_ALREADY_IN_EFFECT);
					return CANT_COMPLETE;
				}

				ADVENTURER_TEAM_ITOR AdvItor;
				
				if (!mfAddActiveItem(Shrink,Combo,Arg,Item.mfDuration(hAvatarStats)))
					return CANT_COMPLETE;

				for (AdvItor = ADVENTURER::begin(); AdvItor != ADVENTURER::end();
					AdvItor++)
				{
					DumbHandlePtr<CAvatar> pAvatar(*AdvItor);
					OldScales.mfAddEffect(pAvatar->hPlayerStats,pAvatar->mfChangeScale(Arg)); //just changes art
	
				}
				
				OldW.mfAddEffect(hAvatarStats,player.w);
				OldH.mfAddEffect(hAvatarStats,player.h);
				SetPlayerSize(player.w*Arg/100,player.h*Arg/100);

				fAlreadyShrunk=TRUE;

				Item.mfMessage(STR_ACTIVATE_SHRINK);
				
			}
				break;
			case DEACTIVATE:
			{
				ADVENTURER_TEAM_ITOR AdvItor;
				for (AdvItor = ADVENTURER::begin(); AdvItor != ADVENTURER::end();
					AdvItor++)
				{
					DumbHandlePtr<CAvatar> pAvatar(*AdvItor);
					LONG const* pOldScale=OldScales.mfGetVal(pAvatar->hPlayerStats);
					if (!pOldScale)
					{
						#ifdef _DEBUG		
						fatal_error("needed data not in effectseperator! %s(%li)\n",__FILE__,__LINE__);
						#else
						pAvatar->mfChangeScale(100);
						#endif
					}
					else
						pAvatar->mfChangeScale(*pOldScale);
				}
				
									
				LONG const * pW=OldW.mfGetVal(hAvatarStats);
				LONG const * pH=OldH.mfGetVal(hAvatarStats);
				LONG W;
				LONG H;
				
				
						
				if (!pW)
					W=PLAYER_WIDTH;
				else
					W=(LONG)*pW;
				if (!pH)
					H=PLAYER_HEIGHT;
				else
					H=(LONG)*pH;
				
				SetPlayerSize(W,H);

				fAlreadyShrunk=FALSE;
				
				mfRemoveActiveItem(Shrink,Combo,Arg);
				Item.mfMessage(STR_DEACTIVATE_SHRINK);
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


LONG ItemEffects::MagicMissileSpell(LONG Combo,LONG Arg,LONG Action)
{

	ITEMTYPE		IType;
	SHORT			hAvatarStats;
	
 	SPLIT_COMBO(Combo,IType,hAvatarStats);
 	
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

				fBtlMagicType = BTLCAP_MAGIC_MISSILE;
				fBtlMagicResult = BTL_R;
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
	else	// adventure level
	{
		switch(Action)
		{
			case ACTIVATE:
			{
				DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(hAvatarStats);

				DICE			dice={0,0,0};
				LONG			WizardLevel;
				
				
				LONG const WizardClass=pPlayerStats->mfGetSpellCastClass(SPELL_INFO::WIZARD);
				if (WizardClass!=CANT_CAST_SPELLS)
					WizardLevel=pPlayerStats->mfGetLevel(WizardClass);
				
				LONG numMissiles = (WizardLevel+1)/2;
				if (numMissiles > 5)
				    numMissiles = 5;
				
				dice.sbNumberOfDice=numMissiles;
				dice.sbNumberOfSides=4;
				dice.sbModifier=numMissiles;
			
				LONG const Damage=RollDice(&dice);

				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
					pAvatar->mfSetDamageDealt(Damage);
					pAvatar->mfCreateFireBall( GetNewId(), PLASMA_STREAK_1, Damage);
				}
					
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
											
LONG ItemEffects::IceStormSpell(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE		IType;
	SHORT			hAvatarStats;
	
 	SPLIT_COMBO(Combo,IType,hAvatarStats);
 	
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
				fBtlMagicType = BTLCAP_MAGIC_PLASMA;
				fBtlMagicResult = BTL_R;
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
	else	// adventure level
	{
	
		switch(Action)
		{
			case ACTIVATE:
			{
				DICE dice={3,10,0};

				DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(hAvatarStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
					
					mfSound(SND_ICE_STORM1,NULL,pAvatar->ThingIndex);
					pAvatar->mfCreateFireBall( GetNewId(), PLASMA_BALL_1, RollDice(&dice));
				}
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

LONG ItemEffects::ConeOfColdSpell(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE		IType;
	SHORT			hAvatarStats;
	
 	SPLIT_COMBO(Combo,IType,hAvatarStats);
 	
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
				fBtlMagicType = BTLCAP_MAGIC_PLASMA;
				fBtlMagicResult = BTL_R;
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
	else	// adventure level
	{
		switch(Action)
		{
			case ACTIVATE:
			{
				DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(hAvatarStats);

				DICE			dice={0,0,0};
				LONG			WizardLevel;
				
				
				LONG const WizardClass=pPlayerStats->mfGetSpellCastClass(SPELL_INFO::WIZARD);
				if (WizardClass!=CANT_CAST_SPELLS)
					WizardLevel=pPlayerStats->mfGetLevel(WizardClass);
			
				dice.sbNumberOfDice=WizardLevel;
				dice.sbNumberOfSides=4;
				dice.sbModifier=WizardLevel*2;
			
				LONG const Damage=RollDice(&dice);

				//               oops ||||
				//                    vvvv
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
					mfSound(SND_CONE_OF_CONE1,NULL,pAvatar->ThingIndex);
					pAvatar->mfSetDamageDealt(Damage);
					pAvatar->mfCreateFireBall( GetNewId(), PLASMA_BALL_1, Damage);
				}
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

LONG ItemEffects::ChainLightningSpell(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE		IType;
	SHORT			hAvatarStats;
	
 	SPLIT_COMBO(Combo,IType,hAvatarStats);
 	
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
				fBtlMagicType = BTLCAP_MAGIC_PLASMA;
				fBtlMagicResult = BTL_D;
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
	else	// adventure level
	{
		switch(Action)
		{
			case ACTIVATE:
			{
				DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(hAvatarStats);

				DICE			dice={0,0,0};
				LONG			WizardLevel = 0;
				
				LONG const WizardClass=pPlayerStats->mfGetSpellCastClass(SPELL_INFO::WIZARD);
				if (WizardClass!=CANT_CAST_SPELLS)
					WizardLevel=pPlayerStats->mfGetLevel(WizardClass);
			
				dice.sbNumberOfDice=WizardLevel;
				dice.sbNumberOfSides=6;
				dice.sbModifier=0;
			
				LONG const Damage=RollDice(&dice);
			
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
					pAvatar->mfSetDamageDealt(Damage);
					pAvatar->mfCreateFireBall( GetNewId(), CHAIN_LIGHTNING_2, Damage);
				}
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

LONG ItemEffects::DeathSpell(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE		IType;
	SHORT			hAvatarStats;
	
 	SPLIT_COMBO(Combo,IType,hAvatarStats);
 	
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
				fBtlMagicType = BTLCAP_MAGIC_DESOLVE;
				fBtlMagicResult = BTL_D;
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
	else	// adventure level
	{
		switch(Action)
		{
			case ACTIVATE:
			{
				DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(hAvatarStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
	
					if (pAvatar->hEnemy==fERROR)
						return CANT_COMPLETE;
					
					DumbHandlePtr<CAvatar>		pEnemy(pAvatar->hEnemy);
					DumbAutoLockPtr<PLAYER_STATS> const pEnemyStats(pEnemy->hPlayerStats);
	
					DICE			dice={0,0,0};
					LONG			WizardLevel = 0;
					
					
					LONG const WizardClass=pPlayerStats->mfGetSpellCastClass(SPELL_INFO::WIZARD);
					if (WizardClass!=CANT_CAST_SPELLS)
						WizardLevel=pPlayerStats->mfGetLevel(WizardClass);
				
					dice.sbNumberOfDice=WizardLevel;
					dice.sbNumberOfSides=6;
					dice.sbModifier=0;
				
					LONG const Damage=RollDice(&dice);
					mfSound(SND_CRUMBLE1,NULL,pEnemy->ThingIndex);
					pAvatar->mfSetDamageDealt(Damage);
					pAvatar->mfCreateFireBall( GetNewId(), CRUMBLE_1,1000);
				}
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

LONG ItemEffects::Blindness(LONG Combo,LONG Arg,LONG Action)
{
	SHORT Itemtype;  				//the index in ItemTypes of the Item
									//being activated
	SHORT hAvatarStats;   			//the stats of the avatar who's activating
									//the item
 	SPLIT_COMBO(Combo,Itemtype,hAvatarStats); //extract the data from the Combo

	ItemType& Item=ItemTypes[Itemtype]; //A reference to the item

	static EffectSeparator<SHORT> OldhEnemies;
	
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
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
	
					if (pAvatar->hEnemy==fERROR)
						return CANT_COMPLETE;
	
					if (!mfAddActiveItem(ItemEffects::Blindness,Combo,Arg,Item.mfDuration(hAvatarStats)))
						return CANT_COMPLETE;
	
					
					DumbHandlePtr<CAvatar>		pEnemy(pAvatar->hEnemy);
					DumbAutoLockPtr<PLAYER_STATS> const pEnemyStats(pEnemy->hPlayerStats);
	
					
					OldhEnemies.mfAddEffect(hAvatarStats,pAvatar->hEnemy);
		
					
					Item.mfMessage(STR_ACTIVATE_BLINDNESS);
					
					mfSound(SND_POWER_BLIND1,NULL,pAvatar->ThingIndex);
					//now actually blind him.
					
					pEnemyStats->StatsMod.mfModifyStats(STATSMOD::WEAPONHIT_MOD,-4);
					pEnemyStats->StatsMod.mfModifyStats(STATSMOD::ARMORCLASS_MOD,4);
					pEnemyStats->mfSetBlind();	
				}
			}
				break;
			case DEACTIVATE:
			{
				SHORT const * phEnemy= OldhEnemies.mfGetVal(hAvatarStats);
				
				if (!phEnemy)
#ifdef DEBUG
					fatal_error("mfGetVal returned NULL %s(%li)\n",__FILE__,__LINE__);
#else
					return CANT_COMPLETE;
#endif

				DumbHandlePtr<CAvatar>		pEnemy(*phEnemy);
				DumbAutoLockPtr<PLAYER_STATS> const pEnemyStats(pEnemy->hPlayerStats);
				
				Item.mfMessage(STR_DEACTIVATE_BLINDNESS);
				
				
				pEnemyStats->StatsMod.mfModifyStats(STATSMOD::WEAPONHIT_MOD,4);
				pEnemyStats->StatsMod.mfModifyStats(STATSMOD::ARMORCLASS_MOD,-4);
				pEnemyStats->mfClearBlind();

				mfRemoveActiveItem(ItemEffects::Blindness,Combo,Arg);
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

LONG ItemEffects::PowerBlindSpell(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE		IType;
	SHORT			hAvatarStats;
	
 	SPLIT_COMBO(Combo,IType,hAvatarStats);
 	
	ItemType&		Item=ItemTypes[IType];

	static EffectSeparator<SHORT> OldhEnemies;

	if(fDomainTurn)
	{
		return NOT_SUPPORTED;
	}
	if(fBattleCombat)
	{
		switch(Action)
		{
			case ACTIVATE:
				fBtlMagicType = BTLCAP_MAGIC_NO_ATTACK;
				fBtlMagicResult = BTL_N;
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
	else	// adventure level
	{
		
		switch(Action)
		{
			case ACTIVATE:
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
	
					if (pAvatar->hEnemy==fERROR)
						return CANT_COMPLETE;
	
					if (!mfAddActiveItem(ItemEffects::Blindness,Combo,Arg,Item.mfDuration(hAvatarStats)))
						return CANT_COMPLETE;
	
					
					DumbHandlePtr<CAvatar>		pEnemy(pAvatar->hEnemy);
					DumbAutoLockPtr<PLAYER_STATS> const pEnemyStats(pEnemy->hPlayerStats);
			
					OldhEnemies.mfAddEffect(hAvatarStats,pAvatar->hEnemy);
		
					
					Item.mfMessage(STR_ACTIVATE_BLINDNESS);
					
					mfSound(SND_POWER_BLIND1,NULL,pAvatar->ThingIndex);
					//now actually blind him.
					
					pEnemyStats->StatsMod.mfModifyStats(STATSMOD::WEAPONHIT_MOD,-4);
					pEnemyStats->StatsMod.mfModifyStats(STATSMOD::ARMORCLASS_MOD,4);
					pEnemyStats->mfSetBlind();
				}
			}
				break;
			case DEACTIVATE:
			{
				SHORT const * phEnemy= OldhEnemies.mfGetVal(hAvatarStats);
				
				if (!phEnemy)
#ifdef DEBUG
					fatal_error("mfGetVal returned NULL %s(%li)\n",__FILE__,__LINE__);
#else
					return CANT_COMPLETE;
#endif

				DumbHandlePtr<CAvatar>		pEnemy(*phEnemy);
				DumbAutoLockPtr<PLAYER_STATS> const pEnemyStats(pEnemy->hPlayerStats);
				
				
				Item.mfMessage(STR_DEACTIVATE_BLINDNESS);
				
				
				pEnemyStats->StatsMod.mfModifyStats(STATSMOD::WEAPONHIT_MOD,4);
				pEnemyStats->StatsMod.mfModifyStats(STATSMOD::ARMORCLASS_MOD,-4);
				pEnemyStats->mfClearBlind();

				mfRemoveActiveItem(ItemEffects::Blindness,Combo,Arg);
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

LONG ItemEffects::MeteorSwarmSpell(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE		IType;
	SHORT			hAvatarStats;
	
 	SPLIT_COMBO(Combo,IType,hAvatarStats);
 	
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
				fBtlMagicType = BTLCAP_MAGIC_FIRE;
				fBtlMagicResult = BTL_D;
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
	else	// adventure level
	{
		switch(Action)
		{
			case ACTIVATE:
			{
				DICE		const dice = {10, 4, 0};
				SHORT		i;
				DumbAutoLockPtr<PLAYER_STATS const > const pPlayerStats(hAvatarStats);
				SHORT		const hAvatar = pPlayerStats->hAvatar;
				LONG		totalDamage = 0;
				
				if (hAvatar == fERROR)
					return NOT_SUPPORTED;
				CAvatar	* const pAvatar=(CAvatar* const )BLKPTR(hAvatar);
				
				for (i=0; i < 4; i++)
				{
					LONG const damage = RollDice(&dice);
					totalDamage += damage;
					pAvatar->mfCreateFireBall( GetNewId(), FIREBALL_1, damage);
				}
				pAvatar->mfSetDamageDealt(totalDamage);
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

LONG ItemEffects::PowerKillSpell(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE		IType;
	SHORT			hAvatarStats;
	
 	SPLIT_COMBO(Combo,IType,hAvatarStats);
 	
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
				fBtlMagicType = BTLCAP_MAGIC_DESOLVE;
				fBtlMagicResult = BTL_D;
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
	else	// adventure level
	{
		switch(Action)
		{
			case ACTIVATE:
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
	
					if (pAvatar->hEnemy==fERROR)
						return CANT_COMPLETE;
					
					DumbHandlePtr<CAvatar>		pEnemy(pAvatar->hEnemy);
					DumbAutoLockPtr<PLAYER_STATS const> const pEnemyStats(pEnemy->hPlayerStats);
	
					mfSound(SND_POWER_KILL1,NULL,pAvatar->ThingIndex);
					if (pEnemyStats->mfGetCurHitPoints()<=60)
						pAvatar->mfKillAvatar(pEnemy);
				}
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

LONG ItemEffects::TimeStopSpell(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE		IType;
	SHORT			hAvatarStats;
	
 	SPLIT_COMBO(Combo,IType,hAvatarStats);
 	
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
				fBtlMagicType = BTLCAP_MAGIC_NO_ATTACK;
				fBtlMagicResult = BTL_N;
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
	else	// adventure level
	{
		//???
	}
	return OK; //everything went ok	
}

/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */

LONG ItemEffects::FireStormSpell(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE		IType;
	SHORT			hAvatarStats;
	
 	SPLIT_COMBO(Combo,IType,hAvatarStats);
 	
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
				fBtlMagicType = BTLCAP_MAGIC_NO_ATTACK;
				fBtlMagicResult = BTL_D;
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
	else	// adventure level
	{
		switch(Action)
		{
			case ACTIVATE:
			{
				DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(hAvatarStats);
				DICE			dice={0,0,0};
				LONG			PriestLevel = 0;
				
				LONG PriestClass=pPlayerStats->mfGetSpellCastClass(SPELL_INFO::PRIEST);
				if (PriestClass!=CANT_CAST_SPELLS)
					PriestLevel=pPlayerStats->mfGetLevel(PriestClass);
			
				dice.sbNumberOfDice=PriestLevel;
				dice.sbNumberOfSides=8;
				dice.sbModifier=0;
			
				LONG const Damage=RollDice(&dice);
				
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
					mfSound(SND_FIRESTORM1,NULL,pAvatar->ThingIndex);
					pAvatar->mfSetDamageDealt(Damage);
					pAvatar->mfCreateFireBall( GetNewId(), FIREBALL_1,Damage);
				}
			}		
				break;
			case DEACTIVATE:
			case PICKUP:
			case DROP:
			case EQUIP:
			case UNEQUIP:
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

LONG ItemEffects::HolyWordSpell(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE		IType;
	SHORT			hAvatarStats;
	
 	SPLIT_COMBO(Combo,IType,hAvatarStats);
 	
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
				fBtlMagicType = BTLCAP_MAGIC_DESOLVE;
				fBtlMagicResult = BTL_D;
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
	else	// adventure level
	{
		switch(Action)
		{
			case ACTIVATE:	
			{
				DumbHandlePtr<SCENE>			pCurrentScene(SCENE_MGR::hCurrentScene);
				DumbAutoLockPtr<PLAYER_STATS>	const pPlayerStats(hAvatarStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>			pAvatar(pPlayerStats->hAvatar);
					
					LONG CasterX = pAvatar->mfX();
					LONG CasterY = pAvatar->mfY();
	
					mfSound(SND_HOLY_WORD1,NULL,pAvatar->ThingIndex);
					for (LONG i=0; i < MAX_AVATARS; i++)
					{
						if (pCurrentScene->Avatars[i] == fERROR)
							continue;
							
						DumbHandlePtr<CAvatar> pAvaTesting(pCurrentScene->Avatars[i]);
						if (aprox_dist(CasterX,CasterY,pAvaTesting->mfX(),pAvaTesting->mfY()) > 300)
							continue;
							
						if (pAvaTesting->hPlayerStats == fERROR)
							continue;
	
						// GWP BOOL const AlreadyLocked = IsLocked(pAvaTesting->hPlayerStats);
						// GWP
						// GWP if (!AlreadyLocked)
						// GWP 	SetQuickLock(pAvaTesting->hPlayerStats);
						
						DumbAutoLockPtr<PLAYER_STATS> pStatsTesting(pAvaTesting->hPlayerStats);
							
						if (GAME_TTYPE::mfIsEvil((THINGTYPE)mythings[pAvaTesting->ThingIndex].type))
						{
							pAvatar->mfInflictMagicDamage(pAvaTesting,VAPORIZE_1,10000);
						}
						else if (pStatsTesting->mfGetAlignment() != pPlayerStats->mfGetAlignment())
						{
							DICE const*const TheHitDice = pStatsTesting->mfGetHitDice();
							SHORT hitdice = (TheHitDice->sbNumberOfDice) + (TheHitDice->sbModifier / 4);
							if (hitdice < 5)
							{
								pAvatar->mfInflictMagicDamage(pAvaTesting,VAPORIZE_1,10000);
							}
							else
							{
								DICE const dice = {5, 8, 0};
								LONG const damage = RollDice(&dice);
								pAvatar->mfSetDamageDealt(damage);
								pAvatar->mfInflictMagicDamage(pAvaTesting,VAPORIZE_1,damage);
							}
						}
						
						// GWP if (!AlreadyLocked)
						// GWP 	ClrLock(pAvaTesting->hPlayerStats);
					}
				}
				break;
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

LONG ItemEffects::MapShowAll(LONG Combo,LONG Arg,LONG Action)
{
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
			{
				if (!mfAddActiveItem(ItemEffects::MapShowAll,Combo,Arg,Item.mfDuration(hAvatarStats)))
					return CANT_COMPLETE;


				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
				
				Item.mfMessage(STR_ACTIVATE_MAPSHOWALL);
				
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
					mfSound(SND_GENERAL_MAGIC1,NULL,pAvatar->ThingIndex);
				}

				SetMapShowAll(0,TRUE);
			}
				break;
			case DEACTIVATE:
				
				Item.mfMessage(STR_DEACTIVATE_MAPSHOWALL);
				
				SetMapShowAll(0,FALSE);
				mfRemoveActiveItem(ItemEffects::MapShowAll,Combo,Arg);
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



LONG ItemEffects::LocateObject(LONG Combo,LONG Arg,LONG Action)
{
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
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);

				//permanently sets it.
				
				Item.mfMessage(STR_ACTIVATE_LOCATEOBJECT);
				
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
					mfSound(SND_GENERAL_MAGIC1,NULL,pAvatar->ThingIndex);
				}
				SetMapHighlightQuestObject(0,TRUE);
			}
				break;
			case DEACTIVATE:
				
				Item.mfMessage(STR_DEACTIVATE_LOCATEOBJECT);
				
				SetMapHighlightQuestObject(0,FALSE);
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

LONG ItemEffects::TurnUndead(LONG Combo,LONG Arg,LONG Action)
{
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
			{
				DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(hAvatarStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
			
					if (pAvatar->hEnemy==fERROR)
						return CANT_COMPLETE;
					
					DumbHandlePtr<CAvatar>		pEnemy(pAvatar->hEnemy);
					DumbAutoLockPtr<PLAYER_STATS > const pEnemyStats(pEnemy->hPlayerStats);
			
					DICE			DamageDice;
	
					mfSound(SND_GENERAL_MAGIC1,NULL,pAvatar->ThingIndex);
					
					LONG PriestClass=pPlayerStats->mfGetSpellCastClass(SPELL_INFO::PRIEST);
					if (PriestClass==CANT_CAST_SPELLS)
						return CANT_COMPLETE;
	
					LONG PriestLevel=pPlayerStats->mfGetLevel(PriestClass);
					
					DamageDice.sbNumberOfDice=PriestLevel;
					DamageDice.sbNumberOfSides=4;
					DamageDice.sbModifier=0;
					LONG const DamageDone=RollDice(&DamageDice);
					
	
					if(pEnemy->mfType()==ZOMBIE ||
						pEnemy->mfType() == SKELETON ||
						pEnemy->mfType() == SPECTRE)
					{
						pAvatar->mfSetDamageDealt(DamageDone);
						pAvatar->mfInflictMagicDamage(pEnemy,CRUMBLE_1,DamageDone);
					}
				}
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

LONG ItemEffects::StoneUndead(LONG Combo,LONG Arg,LONG Action)
{
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
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
			
					if (pAvatar->hEnemy==fERROR)
						return CANT_COMPLETE;
					
					DumbHandlePtr<CAvatar>		pEnemy(pAvatar->hEnemy);
					DumbAutoLockPtr<PLAYER_STATS> const pEnemyStats(pEnemy->hPlayerStats);
	
					if(ThingIsEvil(pEnemy->ThingType) &&
						!SaveVs(pEnemy->hPlayerStats,ST_SPELL,0))
					{	
						mfSound(SND_FLESH_TO_STONE1,NULL,pEnemy->ThingIndex);
						pEnemy->mfTurnToStone();
					}
				}
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


LONG ItemEffects::FleshToStone(LONG Combo,LONG Arg,LONG Action)
{
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
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
			
					if (pAvatar->hEnemy==fERROR)
						return CANT_COMPLETE;
					
					DumbHandlePtr<CAvatar>		pEnemy(pAvatar->hEnemy);
	
					if (!SaveVs(pEnemy->hPlayerStats,ST_SPELL,0))
					{
						mfSound(SND_FLESH_TO_STONE1,NULL,pEnemy->ThingIndex);
						pEnemy->mfTurnToStone();
					}
				}
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

LONG ItemEffects::ResistFire(LONG Combo,LONG Arg,LONG Action)
{
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
			{
				if (!mfAddActiveItem(ItemEffects::ResistFire,Combo,Arg,Item.mfDuration(hAvatarStats)))
					return CANT_COMPLETE;

				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
		
				
				Item.mfMessage(STR_ACTIVATE_RESISTFIRE);
				
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
					mfSound(SND_GENERAL_MAGIC1,NULL,pAvatar->ThingIndex);
				}
			  	pPlayerStats->StatsMod.mfModifyStats(STATSMOD::FIRE_RESIST_MOD,100);
			}
				break;
			case DEACTIVATE:
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);

				
				Item.mfMessage(STR_DEACTIVATE_RESISTFIRE);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::FIRE_RESIST_MOD,-100);
				mfRemoveActiveItem(ItemEffects::ResistFire,Combo,Arg);
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

LONG ItemEffects::SpritualHammer(LONG Combo,LONG Arg,LONG Action)
{
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
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
			
					if (pAvatar->hEnemy==fERROR)
						return CANT_COMPLETE;
					
					DumbHandlePtr<CAvatar>		pEnemy(pAvatar->hEnemy);
					DumbAutoLockPtr<PLAYER_STATS> const pEnemyStats(pEnemy->hPlayerStats);
	
					LONG PriestLevel=0;
					
					LONG PriestClass=pPlayerStats->mfGetSpellCastClass(SPELL_INFO::PRIEST);
					if (PriestClass != CANT_CAST_SPELLS)
						PriestLevel = pPlayerStats->mfGetLevel(PriestClass);
					
					DICE const DamageDice = {1, 6, PriestLevel/2};
			   	
					LONG const DamageDone=RollDice(&DamageDice);
					
					mfSound(SND_SPIRITUAL_HAMMER,NULL,pAvatar->ThingIndex);
					pAvatar->mfSetDamageDealt(DamageDone);
					pAvatar->mfInflictMagicDamage(pEnemy,MAGIC_MISSILE_1,DamageDone);
				}
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


LONG ItemEffects::ProtectionFromEvil(LONG Combo,LONG Arg,LONG Action)
{
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
			{
				if (!mfAddActiveItem(ItemEffects::ProtectionFromEvil,Combo,Arg,Item.mfDuration(hAvatarStats)))
					return CANT_COMPLETE;
						
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
		
				
				Item.mfMessage(STR_ACTIVATE_PROTFROMEVIL);
				
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
					mfSound(SND_GENERAL_MAGIC1,NULL,pAvatar->ThingIndex);
				}
				ADVENTURER_TEAM_ITOR AdvItor;
				
				for (AdvItor = ADVENTURER::begin();
				     AdvItor != ADVENTURER::end();
				     AdvItor++)
				{
					CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(*AdvItor);
					SHORT hStats=pAvatar->hPlayerStats;
					PLAYER_STATS* const pAvatarStats=(PLAYER_STATS* const)BLKPTR(hStats);
					
					pAvatarStats->StatsMod.mfModifyStats(STATSMOD::EVIL_RESIST_MOD,2);

				}
			}
				break;
			case DEACTIVATE:
			{
				
				Item.mfMessage(STR_DEACTIVATE_PROTFROMEVIL);
				

				ADVENTURER_TEAM_ITOR AdvItor;
				
				for (AdvItor = ADVENTURER::begin();
				     AdvItor != ADVENTURER::end();
				     AdvItor++)
				{
					CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(*AdvItor);
					SHORT hStats=pAvatar->hPlayerStats;
					PLAYER_STATS* const pAvatarStats=(PLAYER_STATS* const)BLKPTR(hStats);
					
					pAvatarStats->StatsMod.mfModifyStats(STATSMOD::EVIL_RESIST_MOD,-2);

				}
				mfRemoveActiveItem(ItemEffects::ProtectionFromEvil,Combo,Arg);
				break;
			}
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

LONG ItemEffects::Disintigrate(LONG Combo,LONG Arg,LONG Action)
{
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
	{ 		switch(Action)
		{
			case ACTIVATE:
			{
				fBtlMagicType = BTLCAP_MAGIC_DESOLVE;
				fBtlMagicResult = BTL_D;
			}
			break;
			
			default:
				return NOT_SUPPORTED;
		}
	}
	else	// adventure level
	{
		switch(Action)
		{
			case ACTIVATE:
			{			
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
			
					if (pAvatar->hEnemy==fERROR)
						return CANT_COMPLETE;
					
					DumbHandlePtr<CAvatar>		pEnemy(pAvatar->hEnemy);
					DumbAutoLockPtr<PLAYER_STATS> const pEnemyStats(pEnemy->hPlayerStats);
	
					if (!SaveVs(pEnemy->hPlayerStats,ST_SPELL,0))
					{
						pAvatar->mfCreateFireBall(GetNewId(),VAPORIZE_1,10000);
						mfSound(SND_GENERAL_MAGIC1,NULL,pAvatar->ThingIndex);
					}
				}
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

LONG ItemEffects::SpellImmunity(LONG Combo,LONG Arg,LONG Action)
{
	SHORT Itemtype;					//the index in ItemTypes of the Item
									//being activated
	SHORT hAvatarStats;				//the stats of the avatar who's activating
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
				if (!mfAddActiveItem(ItemEffects::SpellImmunity,Combo,Arg,Item.mfDuration(hAvatarStats)))
					return CANT_COMPLETE;
				
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
				
				Item.mfMessage(STR_ACTIVATE_SPELL_IMMUNE);
				
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
					mfSound(SND_GENERAL_MAGIC1,NULL,pAvatar->ThingIndex);
				}
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::MAGIC_RESIST_MOD,100);
			}
				break;
			case DEACTIVATE:
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
		
				Item.mfMessage(STR_DEACTIVATE_SPELL_IMMUNE);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::MAGIC_RESIST_MOD,-100);
				mfRemoveActiveItem(ItemEffects::SpellImmunity,Combo,Arg);
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

LONG ItemEffects::FlameStrike(LONG Combo,LONG Arg,LONG Action)
{
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
		switch(Action)
		{
			case ACTIVATE:
			{
				fBtlMagicType = BTLCAP_MAGIC_FIRE;
				fBtlMagicResult = BTL_R;
			}
				break;
			default:
				return NOT_SUPPORTED;
		}
	}
	else	// adventure level
	{
		
		switch(Action)
		{
			case ACTIVATE:
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
			
					if (pAvatar->hEnemy==fERROR)
						return CANT_COMPLETE;
					
					DumbHandlePtr<CAvatar>		pEnemy(pAvatar->hEnemy);
					DumbAutoLockPtr<PLAYER_STATS> const pEnemyStats(pEnemy->hPlayerStats);
					
					DICE			const DamageDice = {6,8,0};
					LONG			DamageDone=RollDice(&DamageDice);
	
					mfSound(SND_FIREBALL_EXPLODE1,NULL,pAvatar->ThingIndex);
					if (SaveVs(pEnemy->hPlayerStats,ST_SPELL,0))
						 DamageDone /= 2;
					pAvatar->mfSetDamageDealt(DamageDone);
					pAvatar->mfInflictMagicDamage(pEnemy,FIREBALL_1,DamageDone);
				}
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




LONG ItemEffects::Shillelagh(LONG Combo,LONG Arg,LONG Action)
{
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
			{
				if (!mfAddActiveItem(ItemEffects::Shillelagh,Combo,Arg,Item.mfDuration(hAvatarStats)))
					return CANT_COMPLETE;
				
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
			
				
				Item.mfMessage(STR_ACTIVATE_SHILLELAGH);
				
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
					mfSound(SND_GENERAL_MAGIC1,NULL,pAvatar->ThingIndex);
				}
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::WEAPONHIT_MOD,1);
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::WEAPONDAMAGE_MOD,1);
			}
				break;
			case DEACTIVATE:
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
				
				Item.mfMessage(STR_DEACTIVATE_SHILLELAGH);
				
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::WEAPONHIT_MOD,-1);
				pPlayerStats->StatsMod.mfModifyStats(STATSMOD::WEAPONDAMAGE_MOD,-1);
				mfRemoveActiveItem(ItemEffects::Shillelagh,Combo,Arg);
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



LONG ItemEffects::GlobeInvulnerability(LONG Combo,LONG Arg,LONG Action)
{
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
		LONG ResistLevel;
		switch(Action)
		{
			case ACTIVATE:
			{
				if (!mfAddActiveItem(ItemEffects::GlobeInvulnerability,Combo,Arg,Item.mfDuration(hAvatarStats)))
					return CANT_COMPLETE;

				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);

				
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
					mfSound(SND_GENERAL_MAGIC1,NULL,pAvatar->ThingIndex);
				}

				
				if(Arg==GLOBE_MAJOR)
				{
					ResistLevel=4;
					Item.mfMessage(STR_ACTIVATE_GLOBE_INVULN_4);
					
				}
				else
				{
					ResistLevel=3;
					Item.mfMessage(STR_ACTIVATE_GLOBE_INVULN_3);
					
				}

				pPlayerStats->StatsMod.mfSetStats(STATSMOD::MAGIC_LEVEL_IMMUNE_SET,ResistLevel);
			}
				break;
			case DEACTIVATE:
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
				
				Item.mfMessage(STR_DEACTIVATE_GLOBE_INVULN);
				
				pPlayerStats->StatsMod.mfSetStats(STATSMOD::MAGIC_LEVEL_IMMUNE_SET,0);
				mfRemoveActiveItem(ItemEffects::GlobeInvulnerability,Combo,Arg);
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

LONG ItemEffects::ShockingGrasp(LONG Combo,LONG Arg,LONG Action)
{
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
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
			
					if (pAvatar->hEnemy==fERROR)
						return CANT_COMPLETE;
					
					DumbHandlePtr<CAvatar>		pEnemy(pAvatar->hEnemy);
					DumbAutoLockPtr<PLAYER_STATS> const pEnemyStats(pEnemy->hPlayerStats);
					DICE						DamageDice = {1,8,0};
	
					mfSound(SND_SHOCKING_GRASP1,NULL,pAvatar->ThingIndex);
					LONG const WizardClass=pPlayerStats->mfGetSpellCastClass(SPELL_INFO::WIZARD);
					if (WizardClass==CANT_CAST_SPELLS)
						return CANT_COMPLETE;
	
					LONG const WizardLevel=pPlayerStats->mfGetLevel(WizardClass);
					DamageDice.sbModifier=WizardLevel;
					LONG const DamageDone=RollDice(&DamageDice);
					pAvatar->mfSetDamageDealt(DamageDone);
					pAvatar->mfInflictMagicDamage(pEnemy,LIGHTNING_1,DamageDone);
				}
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




LONG ItemEffects::Confusion(LONG Combo,LONG Arg,LONG Action)
{
	SHORT Itemtype;  				//the index in ItemTypes of the Item
									//being activated
	SHORT hAvatarStats;   			//the stats of the avatar who's activating
									//the item
 	SPLIT_COMBO(Combo,Itemtype,hAvatarStats); //extract the data from the Combo

	ItemType& Item=ItemTypes[Itemtype]; //A reference to the item



	static EffectSeparator<CAvatar::AIFUNC> OldAIFuncs;
	static EffectSeparator<SHORT> OldhEnemies;

	
	if(fDomainTurn)
	{
		return NOT_SUPPORTED;
	}
	if(fBattleCombat)
	{
		switch(Action)
		{
			case ACTIVATE:
				fBtlMagicType = BTLCAP_MAGIC_DESOLVE;
				fBtlMagicResult = BTL_R;
				break;
		}
	}
	else	// adventure level
	{
		
		switch(Action)
		{
			case ACTIVATE:
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
				if (pPlayerStats->hAvatar != fERROR)
				{
					DumbHandlePtr<CAvatar>		pAvatar(pPlayerStats->hAvatar);
	
					if (pAvatar->hEnemy==fERROR)
						return CANT_COMPLETE;
	
					DumbHandlePtr<CAvatar>		pEnemy(pAvatar->hEnemy);
					DumbAutoLockPtr<PLAYER_STATS> const pEnemyStats(pEnemy->hPlayerStats);
					LONG NewCombo=BUILD_LONG((SHORT)Itemtype,pEnemy->hPlayerStats);
			
	
					if (!mfAddActiveItem(ItemEffects::Confusion,NewCombo,Arg,Item.mfDuration(hAvatarStats)))
					{
						Item.mfMessage(STR_DUDE_ALREADY_CONFUSED);
						return CANT_COMPLETE;
					}
	
					CAvatar::AIFUNC TempAIFunc;
					
					Item.mfMessage(STR_ACTIVATE_CONFUSION);
					
					mfSound(SND_GENERAL_MAGIC1,NULL,pAvatar->ThingIndex);
				
					TempAIFunc=(CAvatar::AIFUNC)pEnemy->GetAIFuncIndex();
					OldAIFuncs.mfAddEffect(pEnemy->hPlayerStats,TempAIFunc);
					STUNNED_DATA::mfSwitchToStunned(pAvatar->hEnemy);
				}
			}

				break;
			case DEACTIVATE:
			{
				//in this case, the hAvatarStats are the ENEMY'S
				//stats, not the caster's...

				CAvatar::AIFUNC const *pTempAIFunc=OldAIFuncs.mfGetVal(hAvatarStats);
				OldAIFuncs.mfRemoveEffect(hAvatarStats);

				if (!pTempAIFunc)
#ifdef _DEBUG
					fatal_error("Needed data not in list!(pTempAIFunc:%p) %s(%li)\n",pTempAIFunc,__FILE__,__LINE__);
#else
					return NOT_SUPPORTED;
#endif

				DumbAutoLockPtr<PLAYER_STATS> const pEnemyStats(hAvatarStats);
				DumbHandlePtr<CAvatar>		pEnemy(pEnemyStats->hAvatar);

				
				Item.mfMessage(STR_DEACTIVATE_CONFUSION);
				
				pEnemy->SetAIFuncIndex(*pTempAIFunc);
				pEnemy->DoAI(CAvatar::AI_INIT);				
				mfRemoveActiveItem(ItemEffects::Confusion,Combo,Arg);
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




LONG ItemEffects::Knock(LONG Combo,LONG Arg,LONG Action)
{
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
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);

				fSpellKnocking=TRUE;
				activate_seg(500*500);
				fSpellKnocking=FALSE;
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
LONG ItemEffects::WaterWalk(LONG Combo,LONG Arg,LONG Action)
{Arg=Combo=Action;return 0;}


/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */
LONG ItemEffects::UndeadTurn(LONG Combo,LONG Arg,LONG Action)
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
		switch(Action)
		{
			case ACTIVATE:
				fBtlMagicType = BTLCAP_MAGIC_FIRE;
				fBtlMagicResult = BTL_D;
				fBtlTurnUndead = TRUE;
				break;
		}
	}
	else	// adventure level
	{
		switch(Action)
		{
			case ACTIVATE:
				{
				PLAYER_STATS * const pPlayerStats=(PLAYER_STATS* const)BLKPTR(hPlayerStats);
				SHORT 		const hAvatar = pPlayerStats->hAvatar;
				
				if (hAvatar == fERROR)
					return NOT_SUPPORTED;
					
				CAvatar			*pAvatar=(CAvatar*)BLKPTR((SHORT)hAvatar);
				
				SBYTE	level = 1;
				DICE	dice;
				
				if (Arg & USE_LEVEL)
				{
				   LONG PriestClass = pPlayerStats->mfGetSpellCastClass(SPELL_INFO::PRIEST);
				   if (PriestClass!=CANT_CAST_SPELLS)
				       level = pPlayerStats->mfGetLevel(PriestClass);
				}
				else
				    level = DICE_NUMDICE(Arg);
				
				
				dice.sbNumberOfSides=DICE_NUMSIDES(Arg);
				dice.sbModifier=DICE_MODIFIER(Arg);
				dice.sbNumberOfDice = level;
				
				
				LONG const damage = RollDice(&dice);
				
				// enemy must be undead (marked EVIL)
				if(pAvatar->hEnemy == fERROR)
					return NOT_SUPPORTED;
				
				CAvatar const * const pEnemy=(CAvatar const * const)BLKPTR((SHORT)pAvatar->hEnemy);
				if( ThingIsEvil (pEnemy->ThingType))
				{
					pAvatar->mfSetDamageDealt( damage);
					pAvatar->mfCreateFireBall( GetNewId(), LIGHTNING_1 , damage);
				}
				}
				break;
				
			case PICKUP:
				break;
			case EQUIP:
				break;
			case UNEQUIP:
				break;
			case DROP:
				break;
			case SERVICE:
				break;
			default:
				return NOT_SUPPORTED;
		}
	}
	return OK;
}



//Not used.
LONG ItemEffects::ImprovedFireball(LONG Combo,LONG Arg,LONG Action)
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
				fBtlMagicType = BTLCAP_MAGIC_FIRE;
				fBtlMagicResult = BTL_D;
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
	else	// adventure level
	{
		switch(Action)
		{
			case ACTIVATE:
				{
				DumbAutoLockPtr<PLAYER_STATS const > const pPlayerStats(hPlayerStats);
				SHORT 		const hAvatar = pPlayerStats->hAvatar;
				DICE		dice = {0, 6, 0};
				
				if (hAvatar == fERROR)
					return NOT_SUPPORTED;
				CAvatar	* const pAvatar=(CAvatar* const )BLKPTR(hAvatar);
				
				LONG WizClass = pPlayerStats->mfGetSpellCastClass(SPELL_INFO::WIZARD);
				if (WizClass==CANT_CAST_SPELLS)
				    return CANT_COMPLETE;
				
				dice.sbNumberOfDice = pPlayerStats->mfGetLevel(WizClass);
				LONG const damage = RollDice(&dice);
				pAvatar->mfSetDamageDealt(damage);
				pAvatar->mfCreateFireBall( GetNewId(), FIREBALL_1, damage);
				
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

LONG ItemEffects::ActInfravision(LONG Combo,LONG Arg,LONG action)
{
	ITEMTYPE		IType;
	SHORT			hPlayerStats;
	
 	SPLIT_COMBO(Combo,IType,hPlayerStats);
 	
	ItemType&		Item=ItemTypes[IType];

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
			case PICKUP:
				break;
				
			case ACTIVATE:
			{
				if (!mfAddActiveItem(ItemEffects::ActInfravision,Combo,Arg,Item.mfDuration(hPlayerStats)))
					return CANT_COMPLETE;
					
				DumbAutoLockPtr<PLAYER_STATS>	const pPlayerStats(hPlayerStats);
				DumbHandlePtr<CAvatar>	pAvatar(pPlayerStats->hAvatar);

				Item.mfMessage(STR_ACTIVATE_INFRAVISION);
				mfSound(SND_INFRAVISION1,1,pAvatar->ThingIndex);

				gfInfraVision=TRUE;
				break;
			}
			case DEACTIVATE:
			{
				Item.mfMessage(STR_DEACTIVATE_INFRAVISION);
				gfInfraVision=FALSE;
			   	mfRemoveActiveItem(ItemEffects::ActInfravision,Combo,Arg);
				break;
			}
			default:
				return NOT_SUPPORTED;
		}
	}
	
	return OK;

}


LONG ItemEffects::Resurrection(LONG Combo,LONG Arg,LONG Action)
{
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
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hAvatarStats);
				
				if (!pPlayerStats->mfIsType(PLAYER_CLASS_INFO::PRIEST))
					return CANT_COMPLETE;

				ResurrectionMenu::Display(hAvatarStats,(ITEMTYPE)Itemtype);
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

ITEMTYPE ResurrectionMenu::UsedItemType=ITEM_NO_THING;
SHORT ResurrectionMenu::hCasterStats=fERROR;
SHORT ResurrectionMenu::StatsHdls[4]={fERROR,fERROR,fERROR};

void ResurrectionMenu::Paint(LONG MenuCombo, LONG Arg)
{	
	LONG i=0;
	char NameBuf[30];		
	LONG NameX;
	LONG NameY;
	LONG NameW;
	LONG NameH;
	
	LONG MenuIdx;
	LONG ButtonID;
	SPLIT_LONG(MenuCombo,MenuIdx,ButtonID);


	ADVENTURER_TEAM_ITOR AdvItor;


	AdvItor=ADVENTURER::begin();
	i=RESURRECT_BUT_NAME1;

	while (i<=RESURRECT_BUT_NAME3 && AdvItor!=ADVENTURER::end())
	{

		DumbHandlePtr<CAvatar> pAvatar(*AdvItor);
		if (pAvatar->hPlayerStats)
		{
			DumbAutoLockPtr<PLAYER_STATS> const pAvatarStats(pAvatar->hPlayerStats);			
	
			if (pAvatar->hPlayerStats==hCasterStats || pAvatarStats->mfGetCurHitPoints()>0)
			{
			 	AdvItor++;
				continue;
			}
		
			StatsHdls[i-RESURRECT_BUT_NAME1]=pAvatar->hPlayerStats;
	
	//		sprintf(NameBuf,"%s (%li)",PLAYER_NAME::mfGetName(pAvatarStats->NameClass.fName),pAvatarStats->HitPts);
			// [d4-09-97 JPC] Changed the following because PLAYER_NAME is obsolete.
			// sprintf(NameBuf,"%s",PLAYER_NAME::mfGetName(pAvatarStats->NameClass.fName));
			sprintf(NameBuf,"%s",pAvatarStats->mfGetName());
	
			GetButtonPosition(MenuIdx,i,&NameX,&NameY);
			GetButtonSize(MenuIdx,i,&NameW,&NameH);
			
			SetButtonProc(MenuIdx,i,ResurrectionMenu::Resurrect,i-RESURRECT_BUT_NAME1,NO_KEY);
	
			print_text_centered(NameX+(NameW/2),NameY+(NameH/2)+1,NameBuf,31);
		}

		AdvItor++;
		i++;
	}
}

void ResurrectionMenu::Resurrect(LONG MenuCombo, LONG Arg)
{	
		
	if (StatsHdls[Arg]==fERROR)
		return;

	HideSubMenu(0,D_RESURRECT_MENU);
	RunMenus();
	
	DumbAutoLockPtr<PLAYER_STATS> pAvatarStats(StatsHdls[Arg]);

	pAvatarStats->mfRestoreFullHitPts();
	
	AddSndObj(SND_HEAL2,NULL,VOLUME_FULL);
	
	if (pAvatarStats->hAvatar != fERROR)
	{
		DumbHandlePtr<CAvatar> pAvatar(pAvatarStats->hAvatar);

		pAvatar->attrib.KilledByLava=FALSE; //Now they can regenerate again
					
		pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PLAYER);
		pAvatar->mfSetType(pAvatarStats->mfGetType());
		pAvatar->DoAI(CAvatar::AI_RESURRECTION);
	}


	//this section is so we only drop the item if they actually
	//resurrect somebody.
	DumbAutoLockPtr<PLAYER_STATS > const pCasterStats(hCasterStats);
	ObjectList& const Ol= pCasterStats->mfGetInventory();
	
	InventoryItor itor(Ol);
	
	itor.mfFind(UsedItemType);
	
	itor.mfDropItem();
	
}	

void ResurrectionMenu::Display(SHORT hCastingPriestStats,ITEMTYPE Type)
{
	hCasterStats=hCastingPriestStats;
	UsedItemType=Type;	
	SetButtonLabel(D_RESURRECT_MENU, RESURRECT_BUT_TITLE, STR_ITEM_UI_RESURRECTION, 31);
	SetButtonLabel(D_RESURRECT_MENU, RESURRECT_BUT_Q, STR_ITEM_UI_RESURRECTION_QUESTION, 31);
	SetButtonProc(D_RESURRECT_MENU, RESURRECT_BUT_ALL, ResurrectionMenu::Paint, 0, NO_KEY);
	ShowSubMenu(0,D_RESURRECT_MENU);
}


