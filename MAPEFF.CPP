#include "SYSTEM.H"

#include "PLAYSTAT.HXX"
#include "ITEMTYPE.HXX"
#include "TMSGBOX.HXX"
#include "strenum.h"
#include "SCNMGR.HXX"

#include "BATTLE.HXX"
#include "BATTLEUI.HXX"
#include "strenum.h"   


//MAP EFFECTS

LONG ItemEffects::MapHighlightEnemies(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE Itemtype;  				//the index in ItemTypes of the Item 
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
				if (!mfAddActiveItem(MapHighlightEnemies,Combo,Arg,Item.mfDuration(hPlayerStats)))
					return CANT_COMPLETE;
				
				mfMessageBox(Item.mfGetName(),STRMGR_GetStr(STR_ACTIVATE_MAP_HIGHLIGHT_ENEMIES));
				mfSound(SND_GENERAL_MAGIC1,1,VOLUME_FULL);
				SetMapHighlightEnemies(0,Arg);
				

				if (ItemTypes[Itemtype].mfCategory() == IC_MAGITEM)
				{
					ItemChargeHandler::mfDropCharge(Itemtype,hPlayerStats);
					return DONT_DROP;
				}
				break;
			case DEACTIVATE:
				mfMessageBox(Item.mfGetName(),STRMGR_GetStr(STR_DEACTIVATE_MAP_HIGHLIGHT_ENEMIES));
				SetMapHighlightEnemies(0,Arg);
				mfRemoveActiveItem(MapHighlightEnemies,Combo,Arg);
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

LONG ItemEffects::MapHighlightTreasure(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE Itemtype;  				//the index in ItemTypes of the Item 
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
				if(!mfAddActiveItem(ItemEffects::MapHighlightTreasure,Combo,Arg,Item.mfDuration(hPlayerStats)))
					return CANT_COMPLETE;
				mfMessageBox(Item.mfGetName(),STRMGR_GetStr(STR_ACTIVATE_MAP_HIGHLIGHT_TREASURE));
				mfSound(SND_GENERAL_MAGIC1,1,VOLUME_FULL);
				SetMapHighlightTreasure(0,Arg);
				break;
			case DEACTIVATE:
				mfMessageBox(Item.mfGetName(),STRMGR_GetStr(STR_DEACTIVATE_MAP_HIGHLIGHT_TREASURE));
				SetMapHighlightTreasure(0,Arg);
				mfRemoveActiveItem(ItemEffects::MapHighlightTreasure,Combo,Arg);
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

LONG ItemEffects::MapHighlightEvil(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE Itemtype;  				//the index in ItemTypes of the Item 
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
				if(!mfAddActiveItem(ItemEffects::MapHighlightEvil,Combo,Arg,Item.mfDuration(hPlayerStats)))
					return CANT_COMPLETE;
				mfMessageBox(Item.mfGetName(),STRMGR_GetStr(STR_ACTIVATE_MAP_HIGHLIGHT_EVIL));
				mfSound(SND_GENERAL_MAGIC1,1,VOLUME_FULL);
				SetMapHighlightEvil(0,Arg);
				break;
			case DEACTIVATE:
				mfMessageBox(Item.mfGetName(),STRMGR_GetStr(STR_DEACTIVATE_MAP_HIGHLIGHT_EVIL));
				SetMapHighlightEvil(0,Arg);
				mfRemoveActiveItem(ItemEffects::MapHighlightEvil,Combo,Arg);
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

LONG ItemEffects::MapHighlightMagic(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE Itemtype;  				//the index in ItemTypes of the Item 
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
				if(!mfAddActiveItem(ItemEffects::MapHighlightMagic,Combo,Arg,Item.mfDuration(hPlayerStats)))
					return CANT_COMPLETE;
				mfMessageBox(Item.mfGetName(),STRMGR_GetStr(STR_ACTIVATE_MAP_HIGHLIGHT_MAGIC));
				mfSound(SND_GENERAL_MAGIC1,1,VOLUME_FULL);
				SetMapHighlightMagic(0,Arg);
				
				if (ItemTypes[Itemtype].mfCategory() == IC_MAGITEM)
				{
					ItemChargeHandler::mfDropCharge(Itemtype,hPlayerStats);
					return DONT_DROP;
				}
				break;
			case DEACTIVATE:
				mfMessageBox(Item.mfGetName(),STRMGR_GetStr(STR_DEACTIVATE_MAP_HIGHLIGHT_MAGIC));
				SetMapHighlightMagic(0,Arg);
				mfRemoveActiveItem(ItemEffects::MapHighlightMagic,Combo,Arg);
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


LONG ItemEffects::MapHighlightTraps(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE Itemtype;  				//the index in ItemTypes of the Item 
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
				if(!mfAddActiveItem(ItemEffects::MapHighlightTraps,Combo,Arg,Item.mfDuration(hPlayerStats)))
					return CANT_COMPLETE;

				mfMessageBox(Item.mfGetName(),STRMGR_GetStr(STR_ACTIVATE_MAP_HIGHLIGHT_TRAPS));
				mfSound(SND_GENERAL_MAGIC1,1,VOLUME_FULL);
				SetMapHighlightTraps(0,Arg);
				break;
			case DEACTIVATE:
				mfMessageBox(Item.mfGetName(),STRMGR_GetStr(STR_DEACTIVATE_MAP_HIGHLIGHT_TRAPS));
				SetMapHighlightTraps(0,Arg);
				mfRemoveActiveItem(ItemEffects::MapHighlightTraps,Combo,Arg);
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
	
LONG ItemEffects::MapHighlightSecretDoors(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE Itemtype;  				//the index in ItemTypes of the Item 
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
				if(!mfAddActiveItem(ItemEffects::MapHighlightSecretDoors,Combo,Arg,Item.mfDuration(hPlayerStats)))
					return CANT_COMPLETE;

				mfMessageBox(Item.mfGetName(),STRMGR_GetStr(STR_ACTIVATE_MAP_HIGHLIGHT_SECRET));
				mfSound(SND_GENERAL_MAGIC1,1,VOLUME_FULL);
				SetMapHighlightSecretDoors(0,Arg);

				if (ItemTypes[Itemtype].mfCategory() == IC_MAGITEM)
				{
					ItemChargeHandler::mfDropCharge(Itemtype,hPlayerStats);
					return DONT_DROP;
				}
				break;
			case DEACTIVATE:
				mfMessageBox(Item.mfGetName(),STRMGR_GetStr(STR_DEACTIVATE_MAP_HIGHLIGHT_SECRET));
				SetMapHighlightSecretDoors(0,Arg);
				mfRemoveActiveItem(ItemEffects::MapHighlightSecretDoors,Combo,Arg);
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



LONG ItemEffects::MapTeleport(LONG Combo,LONG Arg,LONG Action)
{
	ITEMTYPE Itemtype;  				//the index in ItemTypes of the Item 
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
				//globally scoped MapTeleport...
				::MapTeleport(TELEPORT_WAIT);
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
