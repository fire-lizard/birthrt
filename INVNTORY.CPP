/* ========================================================================
   Copyright (c) 1990,1996   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: Invntory.cpp  -Handles the inventory of items for characters
   Author:   Wes Cumberland
   ========================================================================
   Contains the following private functions:
   ObjectList::
    mfPrevNodeIdx() -Gets the index of the node before Current
    mfMakeHead()    -Gets a blank item and makes a head for the list

   Contains the following general functions:
   (arguments only specified in overloaded cases)

   ObjectList::
    mfInitAllItems           -Initializes the AllItems table
    mfFirstBlank             -Finds the first blank in the AllItems list
    mfAddItem(ITEMTYPE,SHORT)-Adds Items to this particular list
    mfAddItem(ITEMTYPE)      -Adds an Item to this particular list
    mfDropItem()             -Drops the current Item
    mfDropItem(LONG,LONG)    -Drops the current Item into the world
    mfDropItem(ITEMTYPE)     -Drops an Item with a certain type
    mfDropItem(ITEMTYPE,LONG,LONG)-Drops an Item with a certain type into the world
    mfFind                   -Advances current to the next occurence of a type
    mfPrint                  -Debug func to print the list.
    mfFind                   -finds the next item with the specified type

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#ifdef _WINDOWS
#include <Windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "SYSTEM.H"
#include "ENGINE.H"
#include "MACHINE.H"

#include "INVNTORY.HXX"

#include "ADVENTUR.HXX"
#include "ADVPREP.HXX"
#include "AVATAR.HXX"
#include "BATTLEUI.HXX"
#include "CHARSEL.HXX"
#include "COMBATUI.HXX"
#include "GMENUENM.H"
#include "GAMETYPE.HXX"
#include "GAMEMAP.HXX"
#include "GAMESTR.H"
#include "INFOBOX.HXX"
#include "INVNGUI.HXX"
#include "ITEMEQP.HXX"
#include "ITEMTYPE.HXX"
#include "ITEMUTIL.HXX"
#include "LOADSAVE.HXX"
#include "PANEL.H"
#include "PLACES.HXX"
#include "REALM.HXX"
#include "REGENTS.HXX"
#include "SCENE.HXX"
#include "SCNMGR.HXX"
#include "SPELL.HXX"
#include "SPELLPRO.HXX"

#ifdef _WINDOWS
#include "WINSYS\MULPLAY.HXX"
#include "WINSYS\MONO_C.H"
#endif

extern BOOL fLogComment;

extern void RandomLogComment ( char * szString );
extern "C" void RandomLogPrefix ( char * szString );

/* ------------------------------------------------------------------------
   Defines
   ------------------------------------------------------------------------ */
//Visual C++ can't handle redundant static modifiers, (as in, "static" in the
//definition of the function) so we have to define them out of existence.
//I provide the redundant statics for readability and clarity.
#ifdef _VC4
#define static
#endif

#define MAX_ITEMS_IN_LIST 20
#define MAX_PICKUP_DISTANCE 300
#define SAVEDIR ".\\"

#define NECKLACE_OF_MISSILE_CHARGES	8
#define GENERAL_SHOOTING_CHARGES	3

/* ------------------------------------------------------------------------
   Static Member Data
   ------------------------------------------------------------------------ */
InventoryNode ObjectList::AllItems[MAX_INVENTORY_ITEMS];
DEFINE_VECTOR_MEMBER_CLASS(InventoryNode, ObjectList, AllItems);

BOOL ObjectList::AllItemsInitted=FALSE;
BOOL ObjectList::MouseCallbackInitted=FALSE;
UBYTE ObjectList::IDs=0;

UBYTE const InventoryNode::fValid=0x01;
UBYTE const InventoryNode::fWorn=0x02;
UBYTE const InventoryNode::fIgnorable=0x04;
UBYTE const InventoryNode::fHidden=0x08;

extern SHORT BARelicFound;			// from practice.cpp

extern void VictoryScreen(PFVLL,PFVLL);

extern BOOL fStartMultiGame;

/* ========================================================================
   Function    - DoesPlayerHaveItem
   Description - C callable func to determine if the player has an object
   Returns     - void
   ======================================================================== */
BOOL DoesPlayerHaveItem(THINGTYPE t)
{
	return ADVENTURER::mfDoesAdventureTeamHave(t);
}


void PaintQuestItemFound(LONG MenuCombo,LONG b)
{
	LONG	X,Y;
	LONG	mx,my;
	LONG	MenuId, ButtonId;
	CHAR	color = 1;

	CHAR	FormatBuf[256];
	CHAR	LongDesc[512];
	CHAR	Buf[512];
	
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	// get position of screen
	if( GetButtonPosition( MenuId, 0, &mx, &my ) == fERROR)
		return;
		
	X = mx + 60;
	Y = my + 20;

	// paint dialog title
	gprint_text(X,Y,STRMGR_GetStr(STR_SUCCESS), 31);
	
	X = mx + 27;
	Y = my + 100;
	
	init_gfont(FONT_SANS_12PT);
		
	
	strncpy(FormatBuf, STRMGR_GetStr(STR_QUESTITEM_CONGRATS), sizeof(FormatBuf));

	
	ITEMTYPE const QuestItem = ObjectList::mfGameTypetoItemType((THINGTYPE)GetQuestThing());
	ItemType const &QuestItemData = ItemTypes[QuestItem];
	strcpy(LongDesc, STRMGR_GetStr(QuestItemData.mfGetExtendedData()));

	sprintf(Buf,FormatBuf,
			    GAME_TTYPE::mfGetDescription((THINGTYPE)GetQuestThing()),
			    LongDesc);

	gprint_text(X,Y,Buf,color);
	Y += gtext_height(Buf);
	
	strncpy(FormatBuf, STRMGR_GetStr(STR_QUESTITEM_YOUCANLEAVE), sizeof(FormatBuf));
	sprintf(Buf, FormatBuf, place_names[SCENE_MGR::PlacesIndex]);
	gprint_text(X, Y, Buf, color);
}
	
void QuestItemFoundProc(LONG MenuCombo,LONG b)
{
	LONG	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	SetButtonHilight(MenuId, ButtonId, TRUE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	SetButtonHilight(MenuId, ButtonId, FALSE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	HideMenu(MenuId);
	RunMenus();
}

LONG GetLevelFiendIndex(void)
{
	POINT ReturnMe;
	
	for (LONG i=0;i<MAX_THINGS;++i)
	{
		if (mythings[i].valid)
			if ((mythings[i].OriginalType >= FIRST_FIEND &&
				mythings[i].OriginalType <= LAST_FIEND) ||
				mythings[i].OriginalType==SPIDER_KING)
					return i;
	}

	return -1;
}
	

char const * const GivingRealms[]={
{"braem_mw"},
{"calri2kw"},
{"haes_kw" },	
{"ruorv2mw"},
{"sonne2kw"},
{"moons_kw"},
{"shadg_tw"},
//{"bord_kg"}, 2-03-97 WRC
};
#define NUM_GIVING_REALMS (sizeof(GivingRealms)/sizeof(char *))


// GWP What a hack! This data should have come from the scene file.
static BOOL TheftAffectsAlliance(void)
{
	SCENE* pScene=(SCENE*)BLKPTR(SCENE_MGR::hCurrentScene);
	LONG i=0;

	for (i=0;i<NUM_GIVING_REALMS;++i)
	{
		if (!strcmp(pScene->mfGetSceneName(),GivingRealms[i]))
		{
			return FALSE;
		}
	}
	
	return TRUE;
}
		


			
/* -----------------------
    ObjectList Members
   ----------------------- */
	
BOOL const ObjectList::mfDoIHaveItem(THINGTYPE const t)
{
	ITEMTYPE Donkey=ObjectList::mfGameTypetoItemType(t);
	
	InventoryItor itor(*this);
	
	return (itor.mfFind(Donkey));
}

BOOL const ObjectList::mfCanICastSpell(ITEMTYPE const Donkey)
{
	InventoryItor itor(*this);
	BOOL Result = FALSE;
	
	if (itor.mfFind(Donkey))
	{
		if ((*itor)->mfHowMany() > 0)
			Result = TRUE;
	}
	
	return Result;
}

BOOL const ObjectList::mfCatsMatch(ItemCategory const c1,ItemCategory const c2)
{
    //seperate them to do a tiny bit of optimizing
	if (c1 == IC_ALL ||
	    c2 == IC_ALL ||
	    c1 == c2)
    {
		return TRUE;
	}

	//special cases
#if 0

	if (c1==IC_ARMS)
		if (c2==IC_WEAPON || c2==IC_ARMOR)
			return TRUE;
	if (c2==IC_ARMS)
		if (c1==IC_WEAPON || c1==IC_ARMOR)
			return TRUE;

	if (c1==IC_SPELL)
		if (c2==IC_REALM_SPELL)
			return TRUE;
	if (c2==IC_SPELL)
		if (c1==IC_REALM_SPELL)
			return TRUE;
#endif
	return FALSE;
}



/* ========================================================================
   Function    - ObjectList::mfMouseCallback()
   Description - this function is what the mouse calls when it gets a click
                 on an object
   Returns     - void
   ======================================================================== */
void ObjectList::mfMouseCallback(LONG but,LONG obj,LONG /* WadThingtype */)
{
	ITEMTYPE Itemtype;
	SHORT hPlrStat=-1;
	CAvatar* pAvatar;
	
	
	//if they just want some information, not to pick it up.
  	if (but==2)
	{
		GAME_STRING Name=GAME_TTYPE::mfGetSTRMgrId((THINGTYPE)mythings[obj].OriginalType);
		InfoBox::Show(Name);
		return;
	}




	Itemtype=mfGameTypetoItemType((THINGTYPE)mythings[obj].OriginalType);
	
	if (Itemtype==MAX_ITEM_TYPES)  //if that gametype wasn't found.
	{
//		printf("The %s is not a gettable item!\n",GAME_TTYPE::mfGetDescription((THINGTYPE)mythings[obj].type));
		
		//this might be a dead player, so we check the player list
		ADVENTURER_TEAM_ITOR AdvItor;
		
		for (AdvItor=ADVENTURER::begin();AdvItor!=ADVENTURER::end();AdvItor++)
		{
			DumbAutoLockPtr< CAvatar > const pAvatar(*AdvItor);
			
			if (pAvatar->ThingIndex==obj)
			{
				DumbAutoLockPtr< PLAYER_STATS > const pStats(pAvatar->hPlayerStats);
				if (pStats->mfGetCurHitPoints()<=0)
					StaticInventoryGUI::mfDisplay(0,pAvatar->hPlayerStats);
			}
		}
	

		return;
	}

	//WRC do this test AFTER we determine if it's a valid object!
	//otherwise it'll tell you that a tree is out of reach...
	if ( (mythings[obj].dist>>8) > MAX_PICKUP_DISTANCE)
	{
		AddSndObj(SND_UI_OUT_OF_REACH1, SND_UI_OUT_OF_REACH_TOTAL, VOLUME_FULL);
		return;
	}
	
		
	pAvatar=((CAvatar*)BLKPTR(ADVENTURER::mfWhoGetsObjectsPickedUp()));
	if (!IsPointerGood(pAvatar))
	{
		printf("BLKPTR returned NULL\n");

		return;
	}

	hPlrStat=pAvatar->hPlayerStats;
	if (hPlrStat!=fERROR)
	{
		SHORT count=1;

		DumbAutoLockPtr< PLAYER_STATS > const pPS(hPlrStat);
		AddItemStatus ais=pPS->mfGetInventory().mfAddItem(Itemtype, count, 0, TRUE);
		if (ais&ADD_OK)
		{
			if (TheftAffectsAlliance())
			{
				SET_NOT_FRIENDLY(SCENE_MGR::Aggressor, (REALM::REALM_TYPE)HomeRealm);
				// In case you have an alliance with monsters as well.
				if (AREALLIED(REALM::MONSTERS, (REALM::REALM_TYPE)HomeRealm))
				{
					SET_NOT_FRIENDLY((REALM::REALM_TYPE)HomeRealm, REALM::MONSTERS);
				}
			}
			purge_thing(obj);
			remove_thing(obj);


			#ifdef _WINDOWS

				if ( IsMultiPlayer() )
				{
					ItemType& Item=ItemTypes[Itemtype]; //A reference to the item
					char buffer[80];
					sprintf(buffer, " picked up %s",Item.mfGetName());
				
					AMultiPlayer.Chat(buffer);
				}


			#endif

			if (mythings[obj].OriginalType==GetQuestThing())
			{
				RunMenus();

#ifdef _WINDOWS

				if ( IsMultiPlayer() )
				{
					AdventureEndDialog( MULTI_EXIT );
				}
				else
#endif
				{
					VictoryScreen(PaintQuestItemFound,QuestItemFoundProc);
				}

				RunMenus();

				if (master_game_type == GAME_ADVENTURE)
				{
					BARelicFound = TRUE;
				}
				else
				{
					advsite[iAdventureSiteIndex].available=DISALLOWED;
				}
			}


			if (ItemTypes[Itemtype].mfCategory() == IC_MAGITEM)
				AddSndObj(SND_PICKUP_MAGIC_ITEM1,0,-1);
			else
				AddSndObj(SND_PICKUP_ITEM1,0,-1);
			
			if (ais&LIST_FULL)
				printf("last item just added\n");
		}
		else
		{
			//need another variable because STRMGR
			//moves mem, so first ptr is unreliable

			char buffer[80];
			strcpy(buffer,STRMGR_GetStr(STR_INVENTORY));
	
			
			if (ais&LIST_OVERFLOW)
			{
				TitledMessageBox::Display(buffer,STRMGR_GetStr(STR_ITEM_TOO_MANY));
				return;
			}
			
			if (ais&EFFECT_ONLY)
				AddSndObj(SND_PICKUP_ITEM1,0,-1);
			else	
			    TitledMessageBox::Display(buffer,STRMGR_GetStr(STR_ITEM_CANT_PICKUP));
			
			return;
		}
	}
}


/* ========================================================================
   Function    - ObjectList::mfInitAllItems()
   Description - Initializes the AllItems table
   Returns     - void
   ======================================================================== */
void ObjectList::mfInitAllItems()
{
	for(long i=0;i<MAX_INVENTORY_ITEMS;++i)
	{
		AllItems[i].mfInit(ITEM_NO_THING,0,0,fERROR,FALSE);
	}
	AllItemsInitted=TRUE;
}



/* ------------------------------------------------------------------------
   ObjectList Public Member Functions
   ------------------------------------------------------------------------ */



/* ========================================================================
   Function    - ObjectList::mfInit
   Description - Initializes an Inventory
   Returns     - void
   ======================================================================== */
void ObjectList::mfInit(SHORT a,LONG ID)
{
	OLID=IDs++;

	if (!AllItemsInitted)
	{
		//printf("initting allitems\n");
		mfInitAllItems();
	}

//	if (!MouseCallbackInitted)
//	{
//		//printf("setting MouseCallback to %p\n",ObjectList::mfMouseCallback);
//	}
//	if (!fBattleCombat)
//	{
//		SetMouseClicked(iOBJECT,ObjectList::mfMouseCallback);
//		MouseCallbackInitted=TRUE;
//	}

	hAvatarStats=a;
	mfSetAvatarID(ID);	
	

	HeadIndex=-1;
	
	
	for (LONG i=0;i<MAX_SPELL_BOXES;++i)
		SpellBoxes[i]= fERROR;

}


//Total, counting each one's quantity.
LONG const ObjectList::mfTotalItems()
{
	InventoryItor itor(*this);
	LONG i;
	
	for (i = 0, itor = begin();
	     itor != end();
	     ++itor)
	{
		if ((*itor)->mfCategory() !=IC_SPELL
			&& (*itor)->mfCategory() !=IC_BLOODAB
			&& (*itor)->mfCategory() !=IC_REALM_SPELL
			)
			i+=(*itor)->mfHowMany();
	}
	return i;
}

//Total number of different items... ignores quantity
LONG const ObjectList::mfTotalDifferentItems()
{
	InventoryItor itor(*this);
	LONG i;
	
	for (i = 0, itor = begin();
	     itor != end();
	     ++itor)
	{
		if ((*itor)->mfCategory() !=IC_SPELL
			&& (*itor)->mfCategory() !=IC_BLOODAB
			&& (*itor)->mfCategory() !=IC_REALM_SPELL
			)
			++i;
			
	}
	return i;
}

//Total number of different items... ignores any item with charges (hack for now)
LONG const ObjectList::mfCountItems()
{
	InventoryItor itor(*this);
	LONG i;
	
	for (i = 0, itor = begin();
	     itor != end();
	     ++itor)
	{
		if ((*itor)->mfCategory() !=IC_SPELL
			&& (*itor)->mfCategory() !=IC_BLOODAB
			&& (*itor)->mfCategory() !=IC_REALM_SPELL
			)
			{
				if (!ItemChargeHandler::mfCharges((*itor)->mfType()))
					++i;
			}
			
	}
	return i;
}

LONG const ObjectList::mfLength()
{
	InventoryItor itor(*this);
	LONG i;
	
	for (i = 0, itor = begin();
	     itor != end();
	     ++i, ++itor)
	{}
	
	return i;
}

LONG const ObjectList::mfLength(ItemCategory const cat)
{
	InventoryItor itor(*this);
	LONG i;
	
	for (i = 0, itor.mfAdvance(cat);
		 itor != end();
		 ++i, itor.mfAdvanceToNext(cat))
	{ }
	
	return i;
}

LONG const ObjectList::mfNonEmptyLength(ItemCategory const cat)
{
	InventoryItor itor(*this);
	LONG i;
	
	for (i = 0, itor.mfAdvanceNonEmpty(cat);
		 itor != end();
		 ++i, itor.mfAdvanceToNextNonEmpty(cat))
	{ }
	
	return i;
}


void ObjectList::mfLoad()
{
	FILE* DatFile;


	char FileName[40];
	SHORT Header;
	SHORT Data[2]; //[0]=Type [1]=Quantity
	SHORT Items;
	LONG i;
	
		
	SHORT ID=mfGetAvatarID();
	//printf("mf Loading OlID=%li (AID=%li)\n",GetID(),AvatarID);

	
	sprintf(FileName,"%sid%li.inv",SAVEDIR,ID);
	if(!Exists(FileName))
	{
		//printf("no inventory data file to load from!\n");
		return;
	}
	
	DatFile=fopen(FileName,"rb");
	if (!DatFile)
	{
		//printf("file not found\n");
		return;
	}
	
	fread(&Header,sizeof(SHORT),1,DatFile);

	if (Header!=MAX_ITEM_TYPES)
	{
		//printf("wrong version! MIT=%li FileMIT=%li cannot load\n",MAX_ITEM_TYPES,Header);
		return;
	}
	
	fread(&Items,sizeof(SHORT),1,DatFile);
	

	// SHORT len;
	// fseek(DatFile,0,SEEK_END);
	// len=ftell(DatFile);
	// fseek(DatFile,4,SEEK_SET);

		
	for (i=0;i<Items;++i)//!feof(DatFile))
	{
		fread(Data,sizeof(SHORT),2,DatFile);
		mfAddItem((ITEMTYPE)Data[0], (SHORT) Data[1], 0, FALSE);
	}
	fclose(DatFile);
}

void ObjectList::mfSaveTo(FILE *fp)
{
	SHORT iteminfo[2];
	LONG  size, count;
	
    count = mfLength();

    fwrite((char *) &count, sizeof(LONG), 1, fp);

	InventoryItor itor(*this);
	for (itor = begin(); itor != end(); ++itor)
	{
		iteminfo[0]= (*itor)->mfType();
		iteminfo[1]= (*itor)->mfHowMany();
		if ((*itor)->mfWorn())
			iteminfo[1] = -iteminfo[1];
		fwrite(iteminfo, sizeof(SHORT) * 2, 1, fp);
	}
}

void ObjectList::mfLoadFrom(FILE *fp)
{
    LONG size, count;
    LONG i;
    SHORT iteminfo[2];

    BOOL sQI = fQuietItems;
    fQuietItems = TRUE;

    fread((char *) &count, sizeof(LONG), 1, fp);
    for (i=0; i < count; ++i)  /* should really buffer this */
    {
        fread((char *) iteminfo, sizeof(SHORT) * 2, 1, fp);
		if (iteminfo[1] < 0)
		{
			mfAddItem((ITEMTYPE)iteminfo[0], (SHORT) (-iteminfo[1]), 0, FALSE);

			InventoryItor itor(*this);
			if (itor.mfFind((ITEMTYPE)iteminfo[0]))
			{
				LONG Arg=BUILD_LONG((*itor)->mfType(),hAvatarStats);
				if (ItemTypes[(*itor)->mfType()].mfEffect(Arg,EQUIP) != CANT_COMPLETE)
				{
					(*itor)->mfWear();
				}
			}
		}
		else
		{
			mfAddItem((ITEMTYPE)iteminfo[0], iteminfo[1], 0, FALSE);
		}
    }
    for (i=0; i < MAX_SPELL_BOXES; ++i)
        SpellBoxes[i] = fERROR;

    fQuietItems = sQI;
}

#if 0
/* ========================================================================
   Function    - ObjectList::mfSave
   Description - Saves data to the invntory data file.
   Returns     - void
   ======================================================================== */
void ObjectList::mfSave()
{
	FILE* DatFile;
	SHORT ID=mfGetAvatarID();
	//printf("mf Saving OlID=%li (AID=%li)\n",GetID(),AvatarID);

	
	SHORT Header=MAX_ITEM_TYPES;
	char FileName[40];
	SHORT WriteMe[2]; //[0]=Type [1]=Quantity
	SHORT Items=mfLength();

//	//printf("ID vs AvID  (%li %li)\n",mfGetAvatarID(),ID);


	sprintf(FileName,"%sid%li.inv",SAVEDIR,ID);

	DatFile=fopen(FileName,"wb");
	fwrite(&Header,sizeof(SHORT),1,DatFile);
	fwrite(&Items,sizeof(SHORT),1,DatFile);
	
	InventoryItor itor(*this);
	for (itor = begin(); itor != end(); ++itor)
	{
		WriteMe[0] = (*itor)->mfType();
		WriteMe[1] = (*itor)->mfHowMany();
		fwrite(WriteMe, sizeof(SHORT), 2, DatFile);
	}
	fclose(DatFile);
}	
#endif
	

BOOL ObjectList::mfFindInSpellBoxes(InvenIndex ii,LONG  * outIdx)
{
	for (LONG i=0;i<MAX_SPELL_BOXES;++i)
	{
		if (SpellBoxes[i]==ii)
		{
			*outIdx=i;
			return TRUE;
		}
	}

 	*outIdx=-1;
	return FALSE;
}

BOOL ObjectList::mfFindInSpellBoxes(ITEMTYPE it,LONG * outIdx)
{
	for (LONG i=0;i<MAX_SPELL_BOXES;++i)
	{
		if (SpellBoxes[i]!=fERROR)
			if (AllItems[SpellBoxes[i]].mfType()==it)
			{
				*outIdx=i;
				return TRUE;
			}
	}

 	*outIdx=-1;
	return FALSE;
}

void ObjectList::mfChangeSpellBox(LONG a, InvenIndex newI)
{
	if (a>=0 && a<MAX_SPELL_BOXES)
		SpellBoxes[a]=newI;
	
	return;
}		



/* ========================================================================
   Function    - ObjectList::mfAddToSpellBoxes
   Description - adds the specified item to the next available spellbox
   Returns     - void
   ======================================================================== */
void ObjectList::mfAddToSpellBoxes(LONG const to,ItemCategory const cat)
{
	LONG i;
	LONG EmptyIndex;
	

	InventoryItor itor(*this);
	
	if (fDomainTurn || cat != IC_SPELL)
		itor.mfAdvanceTo(to,cat);
	else	
		itor.mfAdvanceToNonEmpty(to,cat);
	
	// Make sure that there is at least one charge on it.
	if (itor == end() ||
	    (*itor)->mfHowMany() <= 0)
	{
		return;
	}
	
	// It may become charged on the way to an adventure or battle.
	if (!fDomainTurn)
	{
		if (ItemTypes[(*itor)->mfType()].mfRequires(CHARGES) &&
		    (*itor)->mfGetCharges() <= 0)
		{
			return;
		}
	}
	
	InvenIndex const CurrentInventoryIndex = itor.mfGetCurrentInvIndex();

	// Only one per type in the spell boxes.
	for (i=0;i<MAX_SPELL_BOXES;++i)
	{
		if (SpellBoxes[i] > 0 &&
		    SpellBoxes[i] == CurrentInventoryIndex)
		{
			return;
		}
	}

	//test if we need to wear it instead of hot-boxing it.
	if (EquipTypes[(*itor)->mfType()] > NoEquip)
	{
		ItemEquipType MyType=EquipTypes[(*itor)->mfType()];

		//if it's already worn, take it off!
		if ((*itor)->mfWorn())
		{
			LONG Arg=BUILD_LONG((*itor)->mfType(),hAvatarStats);
			LONG Result=ItemTypes[(*itor)->mfType()].mfEffect(Arg,UNEQUIP);

			if (Result!=CANT_COMPLETE)
			{
				(*itor)->mfRemove();
			}
			return;
		}


		if (EquipTypes[(*itor)->mfType()]!=Unlimited)		
		{
			//get how many of that type of item he's wearing
			LONG numWorn=0;
			InventoryItor Counter(*this);
			for (Counter=begin();Counter!=end();++Counter)
			{
				if (EquipTypes[(*Counter)->mfType()]==MyType)
					if((*Counter)->mfWorn())
						++numWorn;
			}
			if (numWorn>=EquipMax[MyType])
			{
				//print message box or something here
				return;
			}
		}

		LONG Arg=BUILD_LONG((*itor)->mfType(),hAvatarStats);
		LONG Result=ItemTypes[(*itor)->mfType()].mfEffect(Arg,EQUIP);
		if (Result!=CANT_COMPLETE)
		{
//			TitledMessageBox::Display("wow!","Wearing!");
		
			(*itor)->mfWear();
		}
		return;
	}
		
		
#if 0
	
	LONG Arg=BUILD_LONG((*itor)->mfType(),hAvatarStats);
	LONG Result=ItemTypes[(*itor)->mfType()].mfEffect(Arg,EQUIP);

	if (Result==CANT_COMPLETE)
	{
		//need another variable because STRMGR
		//moves mem, so first ptr is unreliable

		char buffer[80];
		strcpy(buffer,STRMGR_GetStr(STR_INVENTORY));

		TitledMessageBox::Display(buffer,STRMGR_GetStr(STR_ITEM_CANT_EQUIP));
	}
	else if (Result==ALWAYS_IN_EFFECT)
	{
		//need another variable because STRMGR
		//moves mem, so first ptr is unreliable

		char buffer[80];
		strcpy(buffer,STRMGR_GetStr(STR_INVENTORY));

		TitledMessageBox::Display(buffer,STRMGR_GetStr(STR_ITEM_ALWAYS_IN_EFFECT));
	}
	else if (Result==NOT_OK)
	{
		//do nothing
	}
	else
	{
		// Get first available slot.
		for (EmptyIndex=0;EmptyIndex<MAX_SPELL_BOXES;++EmptyIndex)
			if (SpellBoxes[EmptyIndex] < 0)
				break;
	
		if(EmptyIndex>=MAX_SPELL_BOXES)
			return;
	
		SpellBoxes[EmptyIndex]=CurrentInventoryIndex;
	}

#endif

}


void ObjectList::mfClearSpellBox(LONG const a)
{
	if (SpellBoxes[a] < 0)
	    return;
	
	ITEMTYPE const SpellType = AllItems[SpellBoxes[a]].mfType();
	LONG Arg=BUILD_LONG(SpellType,hAvatarStats);

	if (ItemTypes[SpellType].mfEffect(Arg,UNEQUIP)!=CANT_COMPLETE)
	{
		SpellBoxes[a]= fERROR;
	}
	else
	{
		//need another variable because STRMGR
		//moves mem, so first ptr is unreliable

		char buffer[80];
		strcpy(buffer,STRMGR_GetStr(STR_INVENTORY));

		TitledMessageBox::Display(buffer,STRMGR_GetStr(STR_ITEM_CANT_UNEQUIP));
	}
}



void ObjectList::mfActivateSpellBox(LONG const a)
{
	if (a<0 || a>MAX_SPELL_BOXES+1 || SpellBoxes[a] < 0)
		return;

	ITEMTYPE const SpellType = AllItems[SpellBoxes[a]].mfType();
	
	InventoryItor itor(*this);
	if (itor.mfFind(SpellType))
	{
	    if ( ItemTypes[SpellType].mfRequires(CHARGES))
	    {
		    if ((*itor)->mfGetCharges() <= 0)
		    {
		    	SpellBoxes[a] = fERROR; // shouldn't try to use it again
		    	return;
		    }
	    }
	    else
	    {
		    if ((*itor)->mfHowMany() < 1)
		    {
		    	SpellBoxes[a] = fERROR; // shouldn't try to use it again
		    	return;
		    }
	    }
	}
#if defined(_DEBUG)
	else
	{
		fatal_error("Activated spellbox without corresponding inventory");
	}
#endif


	LONG Result = mfActivateSpell(SpellType);
	if (OK == Result)
	{
		(*itor)->mfLoseOne();
		
		// If all items are used up, remove from spell box.
		if ((*itor)->mfHowMany() <=0)
		{
			for (LONG i=0; i < MAX_SPELL_BOXES; ++i)
			{
				if (a != i && SpellBoxes[i] == SpellBoxes[a])
					SpellBoxes[i] = fERROR;
			}
			SpellBoxes[a] = fERROR;
				
			ItemCategory cat = (*itor)->mfCategory();
			if (cat != IC_SPELL && cat != IC_REALM_SPELL && cat != IC_BLOODAB)
			{
				itor.mfDeleteItem();
			}
		}
	}
	else if (Result == DECREMENT_CHARGE)
	{
		(*itor)->mfDecrementCharges();
		// If all charges are used up, remove from spell box.
		if ((*itor)->mfGetCharges() <= 0)
		{
			for (LONG i=0; i < MAX_SPELL_BOXES; ++i)
			{
				if (a != i && SpellBoxes[i] == SpellBoxes[a])
					SpellBoxes[i] = fERROR;
			}
			SpellBoxes[a] = fERROR;
		}
	}
}	


InvenIndex const ObjectList::mfGetSpellBox(LONG const a) const
{
	if (a<0 || a>MAX_SPELL_BOXES)
		return fERROR;

	return SpellBoxes[a];
}


/* ========================================================================
   Function    - ObjectList::mfDoesContain(ITEMTYPE)
   Description - attempts to find an occurence of a particular item
   Returns     - whether or not it found it.
   ======================================================================== */
BOOL const ObjectList::mfDoesContain(ITEMTYPE const it)
{	
	InventoryItor itor(*this);
	
	return (itor.mfFind(it));
}

	

/* ========================================================================
   Function    - ObjectList::mfGameTypetoItemType(LONG)
   Description - Finds an ItemType associated with a GameType
   Returns     - the ItemType
   ======================================================================== */
ITEMTYPE const ObjectList::mfGameTypetoItemType(THINGTYPE const GameType)
{
	if (!GAME_TTYPE::mfCanBePickedUp(GameType))
		return MAX_ITEM_TYPES;


	LONG i;
	for (i=ITEM_NO_THING;i<MAX_ITEM_TYPES;++i)
	{
		if (GameType==ItemTypes[i].mfGameType())
			return (ITEMTYPE)i;
	}
	return (ITEMTYPE)i;

}
/* ========================================================================
   Function    - ObjectList::mfFirstBlank()
   Description - finds the first invalid entry in the AllItems table
   Returns     - the Index of that entry
   ======================================================================== */
InvenIndex ObjectList::mfFirstBlank()
{
	InvenIndex NewHead=-1;

	// GWP Loop and test for end of space.
	while (NewHead < MAX_INVENTORY_ITEMS &&
	       AllItems[++NewHead].mfisValid())
	       ;

	if (NewHead >= MAX_INVENTORY_ITEMS)
	{
#if defined (_DEBUG)
		fatal_error("Exceeded max number of items in inventory %s, %d\n",
					__FILE__,__LINE__);
#endif
		NewHead = -1;
	}
		
	return NewHead;
}

/* ========================================================================
   Function    - ObjectList::mfAddItem(ITEMTYPE)
   Description - Adds [count] of Item of a specified type to the list
   Returns     -
   ======================================================================== */
//DJ

AddItemStatus const ObjectList::mfAddItem(
	ITEMTYPE const Type, 
	SHORT const _count, 
	SHORT const _charges,
	BOOL fSend)
{
	SHORT count=(SHORT)_count;
	AddItemStatus ReturnCode=NOT_ADDED;

#ifdef _WINDOWS	
	if (IsMultiPlayer() && fSend)
	{
		LONG i;
		for (i=0; i < CHARACTER_COUNT; ++i)
		{
			if (playerstats[i] == hAvatarStats)
			{
				break;
			}
		}
		if (i < CHARACTER_COUNT) // found
		{
	#ifdef _WINDOWS
	if ( fLogComment )
   {
		char temp[100];
		sprintf ( temp, "AddItem: SendEvent");
		RandomLogComment ( temp );
	}
	#endif

			AMultiPlayer.SendEvent(iMPE_ADD_ITEM, i, (LONG) Type, (LONG) count, TRUE, HomeRealm);
			AMultiPlayer.InitEvent(0);
		}
	}
#endif
	
	if (Type <= ITEM_NO_THING ||
	    Type >= MAX_ITEM_TYPES)
    {
	    return ReturnCode;
	}
	
	if (mfCountItems() >= MAX_ITEMS_IN_LIST)
		ReturnCode|=LIST_FULL;

	if (begin() != end())
	{
		InventoryItor itor(*this);
		if( itor.mfFind(Type))
		{
			// GWP We can't let charged items nest up because there is no way
			//     to activate them one at a time.
			//     The other alternative is to boost the charge on the one
			//     we've got. However note many objects are of a n number of 
			//     charges and when you drop and pick it up, we have no idea
			//     how many charges are on it.
			if (!ItemTypes[(*itor)->mfType()].mfRequires(CHARGES))
			{
				//if one is already there
				for (LONG i=0; i < count; ++i)
				    (*itor)->mfGainOne(); //add one to quantity
	
				return ReturnCode|ADD_DUP|ADD_OK;
			}
		}
	}
	
	if (ReturnCode & LIST_FULL)
	{
		return ReturnCode|LIST_OVERFLOW;
	}

	//by calling the pick-up effect, if it does not fail, we
	//must follow up by picking up the object, otherwise, weird stuff
	//may happen.
		
	LONG Arg=BUILD_LONG(Type,hAvatarStats);
	LONG EffectResult=ItemTypes[Type].mfEffect(Arg,PICKUP);
		
	if (EffectResult==CANT_COMPLETE)
		return ReturnCode|EFFECT_FAILED;
	else if (EffectResult==DONT_ADD)
		return ReturnCode|ADD_OK|EFFECT_ONLY;
	else if (EffectResult == DONT_PICKUP)
		return ReturnCode|EFFECT_ONLY;
	
	if (begin() == end())						// empty inventory
	{
		mfMakeHead(Type, count, _charges);
	}			
	else
	{
		InventoryItor itor(*this);
		SHORT NewItem;
		
		if (ReturnCode&LIST_FULL)
			return ReturnCode|LIST_OVERFLOW;
			
		NewItem=mfFirstBlank();
		if (NewItem < 0)				// No space available.
			return ReturnCode|LIST_FULL;

		itor.mfLast();
		(*itor)->mfSetNext(NewItem);
		++itor;
		(*itor)->mfInit(Type,count,_charges,fERROR,TRUE);
	}
		
	return ReturnCode|ADD_OK;
}

/* for just adding one, the general case */

AddItemStatus const ObjectList::mfAddItem(ITEMTYPE const Type)
{
	ItemCategory const category = ItemTypes[Type].mfCategory();
	
	if (category == IC_SPELL ||
	    category == IC_REALM_SPELL ||
	    category == IC_BLOODAB)
    {
	    return mfAddItem(Type, (SHORT) 0, (SHORT) 0, TRUE); /* these don't get added as quantity */
    }

    return mfAddItem(Type, (SHORT) 1, (SHORT)0, TRUE);
}



/* ========================================================================
   Function    - ObjectList::mfPrint()
   Description - Debug function, printf's the list
   Returns     - nothing
   ======================================================================== */
void ObjectList::mfPrint()
{
   InventoryItor itor(*this);

   //printf("Inventory:\n+--------\n");
   for (itor = begin(); itor != end(); ++itor)
   {
   		ItemTypes[(*itor)->mfType()].mfPrint();
   }
   //printf("+--------\n\n");
}



#if 0
/* ========================================================================
   Function    - ObjectList::mfSetCurrentWeapon(InvenIndex)
   Description - Sets the current weapon
   Returns     - void
   ======================================================================== */
void ObjectList::mfSetCurrentWeapon(InvenIndex const i)
{
	mfReset();
	mfAdvanceTo(i,IC_ARMS);
	if (ItemTypes[mfData().mfType()].mfCategory()==IC_ARMOR)
		return;

	CurrentWeapon=mfData().mfType();
//	//printf("added %s\n",GAME_TTYPE::mfGetDescription(ItemTypes[CurrentWeapon].mfGameType()));

	
}
#endif

#if 0
/* ========================================================================
   Function    - ObjectList::mfSetCurrentArmor(InvenIndex)
   Description - Sets the current armor
   Returns     - void
   ======================================================================== */
void ObjectList::mfSetCurrentArmor(InvenIndex const i)
{
	mfReset();
	mfAdvanceTo(i,IC_ARMS);
	if (mfData().mfCategory()==IC_ARMOR)
		return;

	CurrentArmor=mfData().mfType();
//	//printf("added %s\n",GAME_TTYPE::mfGetDescription(ItemTypes[CurrentWeapon].mfGameType()));

	
}
#endif




#if 0
/* ========================================================================
   Function    - ObjectList::PrintWeaponInfo()
   Description - debug func, prints info to stdout
   Returns     - void
   ======================================================================== */
void ObjectList::PrintWeaponInfo()
{
	SBYTE w;
	
	w=mfGetCurrentWeapon();

	//printf("name: %s \nstyle: %li\n",WEAPON_INFO::mfGetName((WEAPON_INFO::TYPE)w),WEAPON_INFO::mfGetStyle((WEAPON_INFO::TYPE)w));

}
#endif

#if 0
/* ========================================================================
   Function    - ObjectList::PrintArmorInfo()
   Description - debug func, prints info to stdout
   Returns     - void
   ======================================================================== */
void ObjectList::PrintArmorInfo()
{
	SBYTE a;
	
	a=mfGetCurrentArmor();

	//printf("name: %s\nac:%li\n",ARMOR_INFO::mfGetName((ARMOR_INFO::TYPE)a),ARMOR_INFO::mfGetArmorClass((ARMOR_INFO::TYPE)a));

}
#endif
	
#define BUFLEN 120

void ObjectList::ReadINV(SHORT hPlayerStats, LONG id)
{
    FILE *fp;
    char filename[80], buffer[BUFLEN];
    LONG count;
    SHORT entry;

    BOOL sQI = fQuietItems;
    fQuietItems = TRUE;

    // load .inv file

    sprintf(filename, "scenes\\id%ld.inv", id);
    fp = FileOpen(filename, "r");
    if (fp != NULL)
    {
	    while (fgets(buffer, BUFLEN, fp) != NULL)
	    {
	        LONG i;
	        
	        count = strlen(buffer);
	
	        for (i=0; i < count; ++i)
	        {
	            if (buffer[i] == '/' && buffer[i+1] == '/')  /* comment */
	                i = count;   /* skip to the end */
	            else if (buffer[i] >= '0' && buffer[i] <= '9')
	                break;
	        }
	
	        if (i < count)
	        {
				// TODO Fix Stack around the variable 'entry' was corrupted.
	        	sscanf(buffer+i, "%d", &entry);
	            if (entry > 0  && entry < MAX_ITEM_TYPES)
	                mfAddItem((ITEMTYPE) entry);
	        }
	    }
	    FileClose(fp);
	}
	// add priest spells if appropriate
	AddPriestSpells(hPlayerStats);

	fQuietItems = sQI;
}

void ObjectList::AddPriestSpells(SHORT hPlayerStats)
{
	DumbAutoLockPtr< PLAYER_STATS > const pPS(hPlayerStats);
    PLAYER_CLASS_LIST::PLAYER_CLASS_DATA *pCD;
    SHORT level, index;

    for (index = 0; index < MAX_PLAYER_CLASSTYPES; ++index)
    {
        pCD = &(pPS->PlayerClassList.Class[index]);
        for (level = 1; level <= 7; ++level)
        {
            if (num_spells_of(SPELL_INFO::PRIEST, (SPELL_INFO::LEVEL) level,
                pCD->Type, pCD->sbLevel) == 0)
                break;
        }
        --level;
        if (level > 0)
        {
            SPELL_INFO::TYPE spell;

            for (spell  =  SPELL_INFO::FIRST_PRIEST_SPELL;
                 spell <=  SPELL_INFO::LAST_PRIEST_SPELL;
                 spell = (SPELL_INFO::TYPE)(spell + 1))
            {
                if (SPELL_INFO::mfGetSpell(spell).Level <= level)
                    mfAddItem(SpellTypeToItemTypeIdx(spell));
            }
        }

        // and their realm spells

        if (level >= 1)
        {
        	mfAddItem(ITEM_SPELL_INVESTITURE);
        	mfAddItem(ITEM_SPELL_BLESS_LAND);
        	mfAddItem(ITEM_SPELL_BLESS_ARMY);
        	mfAddItem(ITEM_SPELL_BLIGHT);
        	mfAddItem(ITEM_SPELL_DISPEL_REALM_MAGIC_PRIESTVER);
        }
        if (level >= 3)
        	mfAddItem(ITEM_SPELL_HONEST_DEALING);
    }
}
LONG ObjectList::mfSellMundane(void)
{
//	const ItemCategory cat = IC_MUNDANE;
	InventoryItor itor(*this);
	LONG gold = 0;
	
	itor = begin();
	while (itor != end())
	{
		LONG value = -1;
		switch((*itor)->mfType())
		{
			case ITEM_CANDLE_1:
			case ITEM_PARCHMENT_1:
			case ITEM_PARCHMENT_2:
			case ITEM_PARCHMENT_3:
			case ITEM_PARCHMENT_4:
			case ITEM_PARCHMENT_5:
				value = 0;
				break;
				
			case ITEM_CUP_1:
			case ITEM_BOWL_1:
			case ITEM_KNIFE_1:
			case ITEM_PLATE_1:
				value = 1;
				break;
			
			case ITEM_BUCKET_1:
			case ITEM_FLAGON_1:
			case ITEM_SPEAR_1:
				value = 2;
				break;
			
			case ITEM_HELMET_1:
			case ITEM_KEY_BLACK:
			case ITEM_KEY_BLUE:
			case ITEM_KEY_BONE:
			case ITEM_KEY_JEWELED:
			case ITEM_KEY_RED:
			case ITEM_KEY_SILVER:
			case ITEM_KEY_WHITE:
			case ITEM_KEY_SKELETON:
			case ITEM_KEY_STONE:
			case ITEM_KEY_TINY:
				value = 5;
				break;
			
			case ITEM_SWORD_1:
				value = 10;
				break;
			
			case ITEM_GEM_BLUE:
				value = 500;
				break;
				
			case ITEM_GEM_GREEN:
				value = 2000;
				break;
				
			case ITEM_CAPTIVE_1:
				value = -2;		// special case hack
				break;

			case ITEM_CAPTIVE_2:
				value = -3;		// special case hack
				break;

			default:
				break;
		}
		if (value != -1)		// found it
		{
			if (value > 0)
			{
				gold += value * (*itor)->mfHowMany();
				itor.mfDeleteItem();
			}
			else if (value == -2)
			{
				itor.mfDeleteItem();
				mfAddItem(ITEM_GAVELONS_STAFF_OF_PROSPERITY);
			}
			else if (value == -3)
			{
				itor.mfDeleteItem();
				mfAddItem(ITEM_AMULET_OF_INSPIRATION);
			}
			else
			{
				itor.mfDeleteItem();
			}
			itor = begin();
		}
		else
		{
			itor.mfAdvance();
		}
	}
	
	return gold;
}

/* ========================================================================
   Function    - ObjectList::~ObjectList()
   Description - destructor: just runs through and invalidates all it's nodes
                 so they can be reused
   Returns     - N/A
   ======================================================================== */
ObjectList::~ObjectList()
{
	MouseCallbackInitted=FALSE;
	
	InventoryItor itor(*this);
	
	for (itor = begin(); itor != end(); ++itor)
	{
		(*itor)->mfInvalidate();
	}
	HeadIndex=-1;
}


LONG const InventoryItor::mfActivateCurrent()
{
 	LONG Result = 0;
 	
 	if (fCurrentIndex != fInventory.end())
 	{
	 	ITEMTYPE const iType=mfData()->mfType();
	 	
 	    if (ItemTypes[iType].mfRequires(CHARGES)
 	        && mfData()->mfGetCharges() <= 0)
 	    	return CANT_COMPLETE;
 	    	
		if (!fDomainTurn && !fBattleCombat)	// handle targeting in adventure
		{
			DumbAutoLockPtr< PLAYER_STATS > const pPS(fInventory.hAvatarStats);
			if (pPS->hAvatar == fERROR)	// something wrong here
				return CANT_COMPLETE;
				
	 		fInventory.SpellBoxes[MAX_SPELL_BOXES] = fCurrentIndex;
			CombatCastSpellBox(pPS->hAvatar, MAX_SPELL_BOXES);
			return OK;
		}
	 	
	 	Result = fInventory.mfActivateSpell(iType);
	 	if (Result == OK)
	 	{
	 		mfData()->mfLoseOne();
	 		
	 		if (mfData()->mfHowMany() <= 0)
	 		{
				// If its also in a spell box remove it.
				LONG i;
				for (i = 0; i < MAX_SPELL_BOXES; ++i)
				{
					if (fInventory.SpellBoxes[i] == fCurrentIndex)
					{
						fInventory.SpellBoxes[i] = fERROR;
					}
				}
				
				if (mfData()->mfCategory() != IC_SPELL &&
				    mfData()->mfCategory() != IC_REALM_SPELL)
				{
					mfDeleteItem();
				}
	 		}
	 	}
	 	else if (Result == DECREMENT_CHARGE)
	 	{
	 		mfData()->mfDecrementCharges();
	 		if (mfData()->mfGetCharges() <= 0)
	 		{
				// If its also in a spell box remove it.
				LONG i;
				for (i = 0; i < MAX_SPELL_BOXES; ++i)
				{
					if (fInventory.SpellBoxes[i] == fCurrentIndex)
					{
						fInventory.SpellBoxes[i] = fERROR;
					}
				}
	 		}
	 	}
 	}
 		return Result;
}

/* ========================================================================
   Function    - mfFind(ITEMTYPE)
   Description - Advances Current to the next node with the specified type
   		 (starts looking at Current)
   Returns     - void
   ======================================================================== */
BOOL const InventoryItor::mfFind(ITEMTYPE const Type, ItemCategory const cat)
{
	if (Type==ITEM_NO_THING)
		return FALSE;

	BOOL Result = FALSE;
	
	// If this is the second call to this fn. advance one before looking again.
	if (fCurrentIndex != fInventory.begin() &&
		fCurrentIndex != fInventory.end() &&
	    mfData()->mfType() == Type)
	{
		++(*this);
	}

	for(mfAdvance(cat);
	    fCurrentIndex != fInventory.end();
		mfAdvanceToNext(cat))
	{
		if (mfData()->mfType() == Type
			&& !mfData()->mfHidden())
		{
			Result = TRUE;
			break;
		}
	}
	return Result;
}


/* ========================================================================
   Function    - mfFind(ITEMTYPE)
   Description - Advances Current to the next node with the specified type
   		 (starts looking at Current)
   Returns     - void
   ======================================================================== */
BOOL const InventoryItor::mfFind(ITEMTYPE const Type)
{	
	if (Type==ITEM_NO_THING)
		return FALSE;

	BOOL Result = FALSE;
	
	// If this is the second call to this fn. advance one before looking again.
	if (fCurrentIndex != fInventory.begin() &&
		fCurrentIndex != fInventory.end() &&
	    mfData()->mfType() == Type)
	{
		++(*this);
	}

	while (fCurrentIndex != fInventory.end())
	{
		if (mfData()->mfType() == Type
			&& !mfData()->mfHidden())
		{
			Result = TRUE;
			break;
		}
		++(*this);
	}
	
	return Result;
}	


/* ========================================================================
   Function    - mfDeleteItem
   Description - Deletes an Item from the list Call after its all used up.
   Returns     -
   ======================================================================== */
void InventoryItor::mfDeleteItem()
{
	InvenIndex NextNode=mfData()->mfNext();
	
	if (fInventory.HeadIndex == fCurrentIndex)
	{
		fInventory.HeadIndex = NextNode;
	}
	
	InventoryNode * const pPrev = mfPrevious();
	// Test for head of the list.
	if (pPrev)
	{
		pPrev->mfSetNext(NextNode);
	}
	mfData()->mfInvalidate();
	fCurrentIndex=NextNode;
}

/* ========================================================================
   Function    - mfDropItem
   Description - Deletes an Item from the list
   Returns     -
   OPTIONAL ARGUMENT... defaults to TRUE
   ======================================================================== */
void InventoryItor::mfDropItem(BOOL fSend)
{
	if (fCurrentIndex == fInventory.end())
		return;

#ifdef _WINDOWS
	if (IsMultiPlayer() && fSend)
	{
		LONG i;
		for (i=0; i < CHARACTER_COUNT; ++i)
		{
			if (playerstats[i] == fInventory.hAvatarStats)
			{
				break;
			}
		}
		if (i < CHARACTER_COUNT) // found
		{
	#ifdef _WINDOWS
	if ( fLogComment )
   {
		char temp[100];
		sprintf ( temp, "DropItem: SendEvent");
		RandomLogComment ( temp );
	}
	#endif
			AMultiPlayer.SendEvent(iMPE_DROP_ITEM, i, mfData()->mfType(), 0, TRUE, HomeRealm);
			AMultiPlayer.InitEvent(0);
		}
	}
#endif

	LONG Arg=BUILD_LONG((mfData()->mfType()),fInventory.hAvatarStats);

	
	if (mfData()->mfHowMany() > 0)
	{
		mfData()->mfLoseOne();
	}
	
	if (mfData()->mfHowMany() > 0 ||
		mfData()->mfCategory() == IC_SPELL ||
		mfData()->mfCategory() == IC_REALM_SPELL ||
		mfData()->mfCategory() == IC_BLOODAB)
	{
		return;
	}

	//first things first, if it's equipped, unequip it.
	BOOL fEquipped;
	LONG idxSpellBox;

	//see if they're wearing it.
	fEquipped=mfData()->mfWorn();
	
	//if they're not wearing it, maybe it's in the spell boxes
	if (!fEquipped)
		fEquipped=fInventory.mfFindInSpellBoxes(mfGetCurrentInvIndex(),&idxSpellBox);
	
	SHORT	Itemtype = mfData()->mfType();
	if (fEquipped)
	{
		LONG UnequipResult=ItemTypes[Itemtype].mfEffect(Arg,UNEQUIP);

		if (UnequipResult==CANT_COMPLETE)
		{	
			TitledMessageBox::SetTitle(STRMGR_GetStr(STR_INVENTORY));
			TitledMessageBox::SetMessage(STRMGR_GetStr(STR_ITEM_CANT_UNEQUIP));
			TitledMessageBox::Display();			
			
			return;
		}
		else
		{
			if (idxSpellBox>-1)
				fInventory.SpellBoxes[idxSpellBox]=fERROR;
			else
				mfData()->mfRemove();
		}
	}
			
	//now it's been unequipped, now we need to drop it.
		
	LONG DropResult=	ItemTypes[Itemtype].mfEffect(Arg,DROP);
	
	if (DropResult != CANT_COMPLETE)
	{
		mfDeleteItem();
	}
	else
	{
		//need another variable because STRMGR
		//moves mem, so first ptr is unreliable

		char buffer[80];
		strcpy(buffer,STRMGR_GetStr(STR_INVENTORY));

		TitledMessageBox::Display(buffer,STRMGR_GetStr(STR_ITEM_CANT_DROP));
	}
}

/* ========================================================================
   Function    - mfDropItem
   Description - Deletes the current item and inserts it into the world
                 at (x,y)
   Returns     -
   ======================================================================== */
void InventoryItor::mfDropItem(LONG const x, LONG const y)
{
	if (!fDomainTurn)
	{
		InventoryNode * const pCurrentNode = mfData();
		if (!pCurrentNode)
		{
			return;
		}
		
		THINGTYPE const GameType=ItemTypes[pCurrentNode->mfType()].mfGameType();
		
		LONG const ThingIndex = create_thing((LONG)GameType,x,y,point_to_floor_height(x,y));
		mythings[ThingIndex].angle = 0;
		LONG const ss = find_ssector(mythings[ThingIndex].x,
		 						     mythings[ThingIndex].y);
		add_thing(ThingIndex,ss);
	}
	mfDropItem();
}

/* ========================================================================
   Function    - mfDropItem
   Description - Deletes the current item and inserts it into the world
                 at (x,y) this one is overloaded so that the caller can
                 specify whether or not to put it on top of the person who
                 is dropping it.
   Returns     -
   ======================================================================== */
void InventoryItor::mfDropItem(LONG const x, LONG const y, BOOL fWilliamTell)
{
	//WilliamTell, the apple, on top, get it?
	
	if (!fDomainTurn)
	{
		InventoryNode * const pCurrentNode = mfData();
		if (!pCurrentNode)
			return;
		
		DumbAutoLockPtr< PLAYER_STATS > const pAvatarStats(this->fInventory.hAvatarStats);
		DumbAutoLockPtr< CAvatar > const pAvatar(pAvatarStats->hAvatar);
		
		THINGTYPE const GameType=ItemTypes[pCurrentNode->mfType()].mfGameType();
		LONG NewThingZ;

		if (fWilliamTell)
		{
			LONG Floor;
			LONG Ceiling;
			LONG Special;
			
			pAvatar->mfGetSectorInfo(&Floor, &Ceiling, &Special);
			
			NewThingZ=Floor+pAvatar->mfDeathHeight();
		}
		else
			NewThingZ=point_to_floor_height(x,y);
			
	
		LONG const ThingIndex = create_thing((LONG)GameType,x,y,NewThingZ);
		mythings[ThingIndex].angle = 0;
		LONG const ss = find_ssector(mythings[ThingIndex].x,
		 						     mythings[ThingIndex].y);
		add_thing(ThingIndex,ss);
	}
	mfDropItem();
}

void InventoryItor::mfChangeTo(ITEMTYPE it)
{
	//need to check for this in the spellboxes

	LONG SpellBoxToUpdate;


	if (mfData()->mfHowMany()>1)
	{
		fInventory.mfFindInSpellBoxes(fCurrentIndex,&SpellBoxToUpdate);
		
		mfDropItem();
		fInventory.mfAddItem(it);
	
		if (SpellBoxToUpdate!=-1)
		{
			if (!mfFind(it))
			{
				fInventory.SpellBoxes[SpellBoxToUpdate]=fERROR;
				return;
			}
			else
			{
				fInventory.SpellBoxes[SpellBoxToUpdate]=fCurrentIndex;
				return;
			}
		}
	}
	else
	{
		mfData()->mfChangeTo(it);
	}
}

void InventoryNode::mfChangeTo(ITEMTYPE it)
{
	Itemtype=it;
}

void MultiAddItem(LONG iRegentIndex, LONG iItemtype, LONG number)
{
	SHORT hPlayerStats = playerstats[iRegentIndex];
	ITEMTYPE Itemtype = (ITEMTYPE) iItemtype;
	SHORT count = number;
	
	if (hPlayerStats != fERROR)
	{
		DumbAutoLockPtr< PLAYER_STATS > const pPlayerStats(hPlayerStats);
		pPlayerStats->mfGetInventory().mfAddItem(Itemtype, count, 0, FALSE);
	}
}

void MultiDropItem(LONG iRegentIndex, LONG iItemtype)
{
	SHORT hPlayerStats = playerstats[iRegentIndex];
	ITEMTYPE Itemtype = (ITEMTYPE) iItemtype;
	
	if (hPlayerStats != fERROR)
	{
		DumbAutoLockPtr< PLAYER_STATS > const pPlayerStats(hPlayerStats);
		ObjectList& AvatarInventory=pPlayerStats->mfGetInventory();
		InventoryItor itor(AvatarInventory);
		
		if (itor.mfFind(Itemtype))
		{
			itor.mfDropItem(FALSE);
		}
	}
}

/* ========================================================================
   Function    - ClearSpells
	Description - emepty the spells list of any memorized spells
                  and clear hot boxes
	Returns     - void
	======================================================================== */
void ObjectList::ClearSpells(SHORT AvatarStatsHdl)
{
	BOOL   retVal = FALSE;	// assume the worst
	DumbAutoLockPtr<PLAYER_STATS > const pPS(AvatarStatsHdl);
	ObjectList& Inv=pPS->mfGetInventory();
	
	InventoryItor itor(Inv);
	
	// -- clear the memorized spells
	for (itor.mfAdvance(IC_SPELL);
		 itor != end();
		 itor.mfAdvanceToNext(IC_SPELL))
	{ 
		while ((*itor)->mfHowMany() > 0)
		{
			(*itor)->mfLoseOne();
		}
	}
	
	// -- clear the hot boxes
	for (SHORT i=0;i<MAX_SPELL_BOXES;++i)
		SpellBoxes[i] = -1;
}

/* ========================================================================
   Function    - PrepBattleSpells
	Description - given a spell or item, add to inventory if theres room
				  And put it in the hot box if I can.
	Returns     - TRUE still room, FALSE no more room
	======================================================================== */

BOOL ObjectList::PrepBattleSpell(
	ITEMTYPE const           Item,
	SHORT const          AvatarStatsHdl
)
{
	BOOL   retVal = FALSE;	// assume the worst
	DumbAutoLockPtr<PLAYER_STATS > const pPS(AvatarStatsHdl);
	
	InventoryItor itor(*this);
	
	itor.mfFind(Item);
	
	if (itor == end())
	{
		// -- This character doesn't have this spell
		return FALSE;
	}
		
	// -- Check to see if this character can learn any more
	// -- of this type of spell
	LONG const Index = itor.mfGetCurrentInvIndex();
	if (pPS->mfCanIMemorizeThisSpell(Item) == TRUE)
	{
		(*itor)->mfGainOne();
		retVal = TRUE;
	}
	else
	{
		retVal = FALSE;
	}
	
	// -- if I know may of this spell, check for empty hot box to put it in
	if ((*itor)->mfHowMany() > 0)
	{
		LONG EmptyIndex;
	 	// -- check for existing hot box entry	
		for (EmptyIndex=0;EmptyIndex<MAX_SPELL_BOXES;++EmptyIndex)
		{
			if (SpellBoxes[EmptyIndex] == Index)
			{
				goto Leave;
			}
		}
		
		for (EmptyIndex=0;EmptyIndex<MAX_SPELL_BOXES;++EmptyIndex)
		{
			if (SpellBoxes[EmptyIndex] < 0)
			{
				SpellBoxes[EmptyIndex]=Index;
				break;
			}
		}
	}

Leave:		
	return retVal;	// -- can this character learn any more of this spell
}

/* ========================================================================
   Function    - mfIsBattleSpell
   Description - Get my spell number ie, the third spell, and check to 
                 see if it is a battle spell or not.
   Returns     - 
   ======================================================================== */

BOOL ObjectList::mfIsBattleSpell(LONG MySpellNumber)
{
	LONG i;
	
	InventoryItor itor(*this);
	itor.mfAdvance(IC_SPELL);
	
	// Advance my inventory itor to this spell number.
	for (i = 0; i < MySpellNumber; ++i)
	{
		if (itor == end())
			return FALSE;
		itor.mfAdvanceToNext(IC_SPELL);
	}
	if (itor == end())
		return FALSE;
	
	SPELL_INFO::TYPE const spellindex = (SPELL_INFO::TYPE) ItemTypes[(*itor)->mfType()].mfGetExtendedData();
	SPELL_INFO::SPELL_ITEM const spell = SPELL_INFO::mfGetSpell(spellindex);
	return ((spell.WhereCast & SPELL_INFO::BATTLE) != 0);
}


