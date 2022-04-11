/* ========================================================================
   Copyright (c) 1990,1996   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: InvnGUI.cpp -Handles the interface between player and inventory
   Author: Wes Cumberland
   ========================================================================
   Contains the following private functions:
   InventoryGUI::
    mfPaintSpellBoxes  -Paints the spell boxes
    mfPaintListBoxLine -Paints one of the lines of the listbox
    mfPaintArmsBoxes   -Paints the arms boxes
    mfPaintMiscInfoBox -Prints whatever needs to be in the MiscInfo Box

   Contains the following general functions:
   InventoryGUI::
    mfAddToActiveInventory -Adds the specified item to one of the active boxes
    mfPaint                -Paints the menu
    mfInit                 -Initializes the instance

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "system.h"
#include "invngui.hxx"

#include "avatar.hxx"
#include "charsel.hxx"
#include "context.hxx"
#include "handle.hxx"
#include "game.h"
#include "gamemap.hxx"
#include "listbox.hxx"
#include "itemeqp.hxx"
#include "itemtype.hxx"
#include "invntory.hxx"
#include "loadsave.hxx"
#include "panel.h"		// Hack here for the stupid global update varible
#include "regents.hxx"
#include "spell.hxx"

/* ------------------------------------------------------------------------
   Defines
   ------------------------------------------------------------------------ */
#ifdef _VC4
#define static
#endif

#define LIST_SIZE 30	// from advprep.cpp

extern DECL_VECTOR_CLASS(SHORT,Adventures);

extern void BuildAdventureList();
extern SHORT localActiveRegent;

/* ------------------------------------------------------------------------
   Static data
   ------------------------------------------------------------------------ */
InventoryGUI StaticInventoryGUI::GUI;

//SPELL_INFO::BATTLE_TYPE BattleSpellType; //global

#ifndef _WINDOWS
void InvokeDebugger(LONG,LONG);
#endif 

BOOL  fPreparingSpells  = FALSE;
static BOOL  fPickingRecipient = FALSE;
static SHORT hRecipientStats   = fERROR;
static ITEMTYPE itemGiven = ITEM_NO_THING;
static SHORT givenFrom = fERROR;
static SHORT numCharges = 0;
static SHORT numRecipients;
extern BOOL fBattlePrep;




/* ========================================================================
   Function    - mfAddToSpellBoxes
   Description - 
   Returns     - 
   ======================================================================== */

void InventoryGUI::mfAddToSpellBoxes(LONG,LONG b)
{
	if (fPickingRecipient)
	{
	    return;
	}

	DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(AvatarStatsHdl);

	ObjectList& AvatarInventory=pPlayerStats->mfGetInventory();
	AvatarInventory.mfAddToSpellBoxes((DispStart+b),CurrCategory);
	HighlightedLine = DispStart+b;
}

/* ========================================================================
   Function    - mfClearSpellBox
   Description - Fn attached to the spell box to both remove and place
                 an item in the spellbox
   Returns     - 
   ======================================================================== */

void InventoryGUI::mfClearSpellBox(LONG,LONG b)
{
	if (mouse_button==2)
	{
		SystemHelp(STR_INVN_BUTN_HOTBOXES,STR_INVN_HELP_HOTBOX1+b,H_Hot_Boxes,NULL,0);
		return;
	}

	if (fPickingRecipient)
	{
	    return;
	}
	
	DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(AvatarStatsHdl);
	ObjectList& AvatarInventory=pPlayerStats->mfGetInventory();
	
	
	if (HighlightedLine != -1)
	{
		InventoryItor itor(AvatarInventory);
		LONG i;
		LONG EmptyIndex;
		
		BOOL showEmpty = (fDomainTurn || fPreparingSpells || CurrCategory != IC_SPELL);
		if (showEmpty)
			itor.mfAdvance(CurrCategory);
		else
			itor.mfAdvanceNonEmpty(CurrCategory);
		
		for (i = 0; i < HighlightedLine; ++i)
		{
			if (itor == AvatarInventory.end())
			{
				return;
			}

			if (showEmpty)	
				itor.mfAdvanceToNext(CurrCategory);
			else	
				itor.mfAdvanceToNextNonEmpty(CurrCategory);
		}
		if (itor == AvatarInventory.end())
		{
			return;
		}
		
		// or something that isn't "boxable"
		if (EquipTypes[(*itor)->mfType()]!=NoEquip)
		{
			return;
		}
		
		// you can't spellbox something you haven't got
		// But you may if you are on the way to an adventure.
		if ((*itor)->mfHowMany() < 1 
		    && !ItemTypes[(*itor)->mfType()].mfRequires(CHARGES))
		{
			return;
		}
		// Could become charged on the way to battle or adventure.
		if (!fDomainTurn)
		{
			// If no charges, can't spellbox it.
			if (ItemTypes[(*itor)->mfType()].mfRequires(CHARGES) &&
			    (*itor)->mfGetCharges() <= 0)
			{
				return;
			}
		}
		
		// clear self if necessary
		if (AvatarInventory.SpellBoxes[b] == itor.mfGetCurrentInvIndex())
		{
			// unequip this item and remove from hot box
			LONG Arg=BUILD_LONG((*itor)->mfType(),AvatarStatsHdl);
			LONG Result=ItemTypes[(*itor)->mfType()].mfEffect(Arg,UNEQUIP);
			AvatarInventory.SpellBoxes[b] = fERROR;
			return;
		}
		
		// only one spellbox per "thing"
		for (EmptyIndex=0;EmptyIndex<MAX_SPELL_BOXES;++EmptyIndex)
		{
			if (AvatarInventory.SpellBoxes[EmptyIndex] == itor.mfGetCurrentInvIndex())
				return;
		}
		
		LONG Arg=BUILD_LONG((*itor)->mfType(),AvatarStatsHdl);
		LONG Result=ItemTypes[(*itor)->mfType()].mfEffect(Arg,EQUIP);

		if (Result==CANT_COMPLETE)
		{
			char buffer[80];
			strcpy(buffer,STRMGR_GetStr(STR_INVENTORY));

			TitledMessageBox::Display(buffer,STRMGR_GetStr(STR_ITEM_CANT_EQUIP));
		}
		else if (Result==ALWAYS_IN_EFFECT)
		{
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
			AvatarInventory.SpellBoxes[b] = itor.mfGetCurrentInvIndex();
		}
	}
	else if (AvatarInventory.SpellBoxes[b] != fERROR)
		AvatarInventory.mfClearSpellBox(b);
}

void InventoryGUI::mfUseSpellBox(LONG,LONG b)
{
	if (fPickingRecipient)
	{
	    return;
	}

	DumbAutoLockPtr<PLAYER_STATS > const pPlayerStats(AvatarStatsHdl);
	ObjectList& AvatarInventory=pPlayerStats->mfGetInventory();


//	doesn't work.
//	if (mouse_button==2)
//	{
//		GAME_STRING Name=GAME_TTYPE::mfGetSTRMgrId(ItemTypes[ObjectList::AllItems[AvatarInventory.SpellBoxes[b]].mfType()].mfGameType());
//		InfoBox::Show(Name);
//		return;
//	}	

	AvatarInventory.mfActivateSpellBox(b);
}

void InventoryGUI::mfScroll(LONG a,LONG d)
{
	HlMgr.HighlightButton(a,1);

	if (fPickingRecipient)
	{
	    if ((RecipStart+d) < numRecipients-5)
	        RecipStart += (short) d;
	    if (RecipStart < 0)
	        RecipStart = 0;
	    return;
	}
	
	DumbAutoLockPtr<PLAYER_STATS > const pPlayerStats(AvatarStatsHdl);
	ObjectList & const AvatarInventory=pPlayerStats->mfGetInventory();
	
 	if (fDomainTurn || fPreparingSpells || CurrCategory != IC_SPELL)
 	{
 		if (((InvenIndex)(DispStart+d))<AvatarInventory.mfLength(CurrCategory)-5)
			DispStart+=(SHORT)d;
	}
	else
	{
 		if (((InvenIndex)(DispStart+d))<AvatarInventory.mfNonEmptyLength(CurrCategory)-5)
			DispStart+=(SHORT)d;
 	}

	if (DispStart<0)
		DispStart=0;
}


void InventoryGUI::mfSetHighlightedLine(LONG,LONG d)
{
	if (fPickingRecipient)
	{
	    if (HighlightedRecipient == (SHORT)RecipStart+d ||
	        HighlightedRecipient >= numRecipients)
	        HighlightedRecipient = -1;
	    else
	        HighlightedRecipient = (SHORT)RecipStart+d;
	    return;
	}
	if (HighlightedLine==(SHORT)DispStart+d)
		HighlightedLine=-1;
	else
		HighlightedLine=(SHORT)(DispStart+d);
}



void InventoryGUI::mfPaintDudeNameText(LONG MenuId) const
{
	LONG NameX,NameY,NameW,NameH;
	const char* Label;
	char NameBuf[100];

	PLAYER_STATS* pss=(PLAYER_STATS*)BLKPTR(AvatarStatsHdl);

   // [d4-09-97 JPC] modified the following because PLAYER_NAME
	// class no longer exists.
	Label=pss->mfGetName();					

	GetButtonPosition(MenuId,INV_BUT_DUDENAME,&NameX,&NameY);
	GetButtonSize(MenuId,INV_BUT_DUDENAME,&NameW,&NameH);

	sprintf(NameBuf,"^F03%s",Label);
	
	print_text_centered(NameX+NameW/2-1,NameY+NameH/2-3,NameBuf,173);
}


void InventoryGUI::mfPaintCategoryButtonText(LONG MenuId) const
{
	LONG ButX,ButY,ButW,ButH;
	char* Label;
	
	if (fPickingRecipient)
	{
	    return;
	}
	
	for (LONG i=0;i<=INV_BUT_BLOODABLT-INV_BUT_MUNDANE;++i)
	{
		LONG LabelColor;
		
		GetButtonPosition(MenuId,INV_BUT_MUNDANE+i,&ButX,&ButY);
		GetButtonSize(MenuId,INV_BUT_MUNDANE+i,&ButW,&ButH);

		GetButtonLabel(MenuId,INV_BUT_MUNDANE+i,&Label);
		GetButtonLabelColor(MenuId,INV_BUT_MUNDANE+i,&LabelColor);

		if (HlMgr.Highlighted(INV_BUT_MUNDANE+i))
			print_text_centered(ButX+ButW/2-1,ButY+ButH/2-1,STRMGR_GetStr(STR_AVA_INV_ITEMS+i),31);
		else
			print_text_centered(ButX+ButW/2,ButY+ButH/2,STRMGR_GetStr(STR_AVA_INV_ITEMS+i),LabelColor);
	}
}
		
void InventoryGUI::mfPaintActionButtonText(LONG MenuId) const
{
	LONG ButX,ButY,ButW,ButH;
	const char* Label;
	LONG GetMe;
	LONG button;
	
    if (fPickingRecipient)
    {
        return;
    }

    for (button = INV_BUT_USE; button <= INV_BUT_GIVE; ++button)
    {
		GetButtonPosition(MenuId, button, &ButX, &ButY);
		GetButtonSize    (MenuId, button, &ButW, &ButH);
		
	    BOOL greyout = ( CurrCategory == IC_REALM_SPELL
	    	|| (CurrCategory == IC_BLOODAB && button != INV_BUT_USE) );

		if (HighlightedLine==-1)
			greyout=TRUE;
	
		if (fBattleCombat)
			greyout=TRUE;
	
	    switch(button)
	    {
	    case INV_BUT_USE:
	        	
	        GetMe = STR_AVA_INV_USE;
	        if (CurrCategory==IC_SPELL || CurrCategory == IC_REALM_SPELL)
	            GetMe = STR_AVA_INV_CAST;
	
	        if (fPreparingSpells)
	        	greyout = TRUE;
	        	
	        break;
				
	    case INV_BUT_DROP:
	        GetMe = STR_AVA_INV_DROP;
	        if (CurrCategory == IC_SPELL)
	        	GetMe = STR_AVA_INV_FORGET;
	
	        if (fPreparingSpells != (CurrCategory == IC_SPELL))  // XOR
	        	greyout = TRUE;
	
	        break;
	
	    case INV_BUT_GIVE:
			GetMe = STR_AVA_INV_GIVE;
	        if (CurrCategory == IC_SPELL)
	        {
				GetMe = STR_AVA_INV_MEMORIZE;
				if (!fPreparingSpells)
					greyout = TRUE;
			}
			else if (fDomainTurn && master_game_type == GAME_ADVENTURE)
			{
				greyout = TRUE;
			}
			else if (HighlightedLine != -1)		// ack, this is ugly...
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(AvatarStatsHdl);
				ObjectList & const AvatarInventory=pPlayerStats->mfGetInventory();
				InventoryItor itor(AvatarInventory);
				itor.mfAdvanceTo(HighlightedLine,CurrCategory);
				if (itor == AvatarInventory.end()
					|| (*itor) == NULL
					|| (*itor)->mfWorn())
					greyout = TRUE;
			}
	
	        break;
	    }
	    	
	    if (greyout)	
		    print_text_centered(ButX+ButW/2,ButY+ButH/2,STRMGR_GetStr(GetMe),GREY);
	    else if (HlMgr.Highlighted(button))
		    print_text_centered(ButX+ButW/2-1,ButY+ButH/2-1,STRMGR_GetStr(GetMe),31);
	    else
		    print_text_centered(ButX+ButW/2,ButY+ButH/2,STRMGR_GetStr(GetMe),221);
	}
}
	
/* ========================================================================
   Function    - InventoryGUI::mfPaintSpellBoxes
   Description - Paints the spell boxes
   Returns     - void
   ======================================================================== */
void InventoryGUI::mfPaintSpellBoxes(ObjectList const & const Ol,LONG MenuId) const
{
	LONG i=0;
	
	LONG SpellBoxX;
	LONG SpellBoxY;
	LONG SpellBoxW;
	LONG SpellBoxH;

    if (fPickingRecipient)
    {
        return;
    }

	for (i=0;i<MAX_SPELL_BOXES;++i)
	{
		if(GetButtonPosition(MenuId,INV_BUT_SPELLBOX1+i,&SpellBoxX,&SpellBoxY)==fERROR)
			return;
		if(GetButtonSize(MenuId,INV_BUT_SPELLBOX1+i,&SpellBoxW,&SpellBoxH)==fERROR)
			return;
		if (Ol.SpellBoxes[i]>-1)
		{
			ITEMTYPE const SpellType = ObjectList::mfData(Ol.SpellBoxes[i]).mfType();
			THINGTYPE SpellThing = ItemTypes[SpellType].mfGameType();
			SHORT hBitmap = ItemTypes[SpellType].mfGetIconOff();
			DrawBitmap((SHORT)SpellBoxX,
						(SHORT)SpellBoxY,
						hBitmap,
						0,0,
						(SHORT)SpellBoxW,
						(SHORT)SpellBoxH);
			SetPurge(hBitmap);
			ChangeButtonToolTip(MenuId, INV_BUT_SPELLBOX1+i, GAME_TTYPE::mfGetSTRMgrId(SpellThing));
		}
		else
		{
			print_textf(SpellBoxX,SpellBoxY,31,"^F02%d", i+1);
			ChangeButtonToolTip(MenuId, INV_BUT_SPELLBOX1+i, fERROR);
		}
	}
	paint_tooltips();
}



void InventoryGUI::mfPaintListBoxIcon(LONG Line,ObjectList const & const Ol,LONG MenuId) const
{
    return;
}

/* ========================================================================
   Function    - InventoryGUI::mfPaintListBoxLine
   Description - Paints all the lines of the listbox

   Returns     - void
   ======================================================================== */
void InventoryGUI::mfPaintListBoxLine(LONG Lines,ObjectList & const Ol,LONG MenuId) const
{
	LONG ListX=0;
	LONG ListY=0;
	LONG TextX=0;
	LONG TextY=0;
	LONG TextW=0;
	LONG TextH=0;
	LONG NumX=0;
	LONG NumY=0;
	LONG NumW=0;
	LONG NumH=0;
	
	char NameBuf[80];
	char NumBuf[20];
	LONG Line, i;
	SHORT hIcon;
	
	BOOL showEmpty = (fDomainTurn || fPreparingSpells || CurrCategory != IC_SPELL);
	InventoryItor itor(Ol);
	ADVENTURER_TEAM_ITOR AdvItor;
	
	// find first item of current category
	
	if (!fPickingRecipient)
	{
		if (showEmpty)
			itor.mfAdvance(CurrCategory);
		else
			itor.mfAdvanceNonEmpty(CurrCategory);
		
		for (i = 0; i < DispStart; ++i)
		{
			if (itor == Ol.end())
				return;
	
			if (showEmpty)	
				itor.mfAdvanceToNext(CurrCategory);
			else	
				itor.mfAdvanceToNextNonEmpty(CurrCategory);
		}
		if (itor == Ol.end())
			return;
	}
	else			/* special case code for Give */
	{
		if (fDomainTurn || fPreparingSpells)
		{
			for (i=0; i < LIST_SIZE; ++i)
			{
				if (Adventures[i] == -1)
				{
					numRecipients = i;
					break;
				}
			}
		}
		else
		{
			numRecipients = 0;
			for (AdvItor = ADVENTURER::begin();
				AdvItor != ADVENTURER::end();
				AdvItor++)
			{
				++numRecipients;
			}
			if (numRecipients < RecipStart)
				return;
				
			AdvItor = ADVENTURER::begin();
			for (i=0; i < RecipStart; ++i)
				AdvItor++;
		}
	}

	
	
	// paint the list boxes
		
	for (Line = 0; Line < Lines; ++Line)
	{
		GetButtonPosition(MenuId,INV_BUT_LISTBOX,&ListX,&ListY);
		GetButtonPosition(MenuId,INV_BUT_LBNAME1+Line,&TextX,&TextY);	
		GetButtonPosition(MenuId,INV_BUT_LBNUMBER1+Line,&NumX,&NumY);
		GetButtonSize(MenuId,INV_BUT_LBNAME1+Line,&TextW,&TextH);	
		GetButtonSize(MenuId,INV_BUT_LBNUMBER1+Line,&NumW,&NumH);

		LONG CurrTextX;
		LONG CurrTextY;
		LONG TextMarginX=4;  //margins!
		LONG TextMarginY=4;

	
	
		if (!fPickingRecipient)
		{
			if (itor == Ol.end())
				return;
				
		
			sprintf(NumBuf,"^F03%li",(*itor)->mfHowMany());
			
			if (fPreparingSpells && CurrCategory == IC_SPELL)
			{
				SPELL_INFO::TYPE spellindex = (SPELL_INFO::TYPE) ItemTypes[(*itor)->mfType()].mfGetExtendedData();
				SPELL_INFO::SPELL_ITEM spell = SPELL_INFO::mfGetSpell(spellindex);
#ifdef _FOREIGNVER
	#ifdef _GERMANVER
				sprintf(NameBuf,"^F03%s (%c%d)",
					GAME_TTYPE::mfGetDescription(ItemTypes[(*itor)->mfType()].mfGameType()),
					(spell.Type == SPELL_INFO::PRIEST)? 'P':'Z',
					spell.Level);
	#else
	// french
				sprintf(NameBuf,"^F03%s (%c%d)",
					GAME_TTYPE::mfGetDescription(ItemTypes[(*itor)->mfType()].mfGameType()),
					(spell.Type == SPELL_INFO::PRIEST)? 'P':'M',
					spell.Level);
	#endif
#else
				sprintf(NameBuf,"^F03%s (%c%d)",
					GAME_TTYPE::mfGetDescription(ItemTypes[(*itor)->mfType()].mfGameType()),
					(spell.Type == SPELL_INFO::PRIEST)? 'P':'W',
					spell.Level);
#endif
				//auto-font-reduce until it fits!
				while (gtext_width(NameBuf)>TextW && NameBuf[3]!='0')
				{
					NameBuf[3]--;
				}
			}
			else
			{
				int font=3;
				LONG Charges=0;
				char WornStr[3]={"\0"};
				char ChargesStr[15]={"\0"};

				if (ItemTypes[(*itor)->mfType()].mfRequires(CHARGES))
				{
					Charges=(*itor)->mfGetCharges();
					if (Charges >= 255)
					{
						Charges = 0;	// Infinite, don't show a number.
					}
				}
				else
				{
					Charges=ItemChargeHandler::mfCharges((*itor)->mfType());
				}
				
				if (Charges>0)
					sprintf(ChargesStr," (%i)",Charges);
				
				if ((*itor)->mfWorn())
					strcpy(WornStr,"\* ");

				//auto-font-reduce until it fits!
				do
				{
					sprintf(NameBuf,"^F%02i%s%s%s",font--,WornStr,GAME_TTYPE::mfGetDescription(ItemTypes[(*itor)->mfType()].mfGameType()),ChargesStr);
				} while (gtext_width(NameBuf)>TextW && font!=0);
			}			

			LONG NameHeight=gtext_height(NameBuf);
			LONG NameY=TextY+TextH/2-NameHeight/2;
			LONG Color=175;

	
			// draw the text and number
			if (HighlightedLine==DispStart+Line)
				Color=31;


  			gprint_text(TextX+TextMarginX,NameY+2,NameBuf,Color);
  			if (CurrCategory!=IC_BLOODAB && CurrCategory!=IC_REALM_SPELL)
  				print_text_centered(NumX+NumW/2+1,NumY+NumH/2+1,NumBuf,Color);

	
			//draw the icon
			
			if (CurrCategory!=IC_SPELL || !fPreparingSpells)
			{
				LONG IconX,IconY,IconW,IconH;
				SHORT hIcon=fERROR;
					
				GetButtonPosition(MenuId,INV_BUT_LBICON1+Line,&IconX,&IconY);
				GetButtonSize(MenuId,INV_BUT_LBICON1+Line,&IconW,&IconH);
	
				if (HighlightedLine == DispStart+Line)
					hIcon=ItemTypes[(*itor)->mfType()].mfGetIconOn();
				else
					hIcon=ItemTypes[(*itor)->mfType()].mfGetIconOff();
			
				DrawBitmap((SHORT)IconX+3,(SHORT)IconY+2,hIcon,0,0,(SHORT)IconW,(SHORT)IconH);
				SetPurge(hIcon);
			}
			else
			{
				LONG PlusX,PlusY,PlusW,PlusH;
				LONG MinusX,MinusY,MinusW,MinusH;

				GetButtonPosition(MenuId,INV_BUT_LBPLUS1+Line,&PlusX,&PlusY);
				GetButtonSize(MenuId,INV_BUT_LBPLUS1+Line,&PlusW,&PlusH);
				GetButtonPosition(MenuId,INV_BUT_LBMINUS1+Line,&MinusX,&MinusY);
				GetButtonSize(MenuId,INV_BUT_LBMINUS1+Line,&MinusW,&MinusH);

				SHORT hPlusBoxIcon;
				SHORT hMinusBoxIcon;
			
				if (GetButtonFlags(MenuId,INV_BUT_LBPLUS1+Line)&D_HILIGHTED)
					hPlusBoxIcon=GetResourceStd("UI\\INVNICON\\I_ADSUBB.PCX",FALSE);
				else
					hPlusBoxIcon=GetResourceStd("UI\\INVNICON\\I_ADSUBA.PCX",FALSE);
				
				if (GetButtonFlags(MenuId,INV_BUT_LBMINUS1+Line)&D_HILIGHTED)
					hMinusBoxIcon=GetResourceStd("UI\\INVNICON\\I_ADSUBB.PCX",FALSE);
				else
					hMinusBoxIcon=GetResourceStd("UI\\INVNICON\\I_ADSUBA.PCX",FALSE);
			
				if (hPlusBoxIcon > 0)
				{
					DrawBitmap((SHORT)PlusX+2,(SHORT)PlusY+2,hPlusBoxIcon,0,0,(SHORT)PlusW,(SHORT)PlusH);
					SetPurge(hPlusBoxIcon);
				}
			
				if (hMinusBoxIcon > 0)
				{
					DrawBitmap((SHORT)MinusX+2,(SHORT)MinusY+2,hMinusBoxIcon,0,0,(SHORT)MinusW,(SHORT)MinusH);
					SetPurge(hMinusBoxIcon);
				}

				print_text_centered(PlusX+PlusW/2+1,PlusY+PlusH/2+1,"^F03+",175);
				print_text_centered(MinusX+MinusW/2+1,MinusY+MinusH/2+1,"^F03-",175);
			}
			if (showEmpty)	
				itor.mfAdvanceToNext(CurrCategory);
			else	
				itor.mfAdvanceToNextNonEmpty(CurrCategory);
		}
		else		// "give" code
		{
			if (RecipStart+Line >= numRecipients)
			    return;
			
		    if (fPreparingSpells || fDomainTurn)
			{
				SHORT who = Adventures[RecipStart+Line];
				if (who == -1)
					return;
	
				if (RecipStart+Line == HighlightedRecipient)
					hRecipientStats = playerstats[who];
				
				// [d4-09-97 JPC] Modified the following to use the access
				// function instead of direct access to the .name member,
				// which is going away.
				strcpy(NameBuf, regents[who].mfGetname());
				
//				for (SHORT k=0; k<CHARACTER_COUNT; ++k)
//				{
//					if(regents[k].unit == who)
//					{
//						if (RecipStart+Line == HighlightedRecipient)
//							hRecipientStats = playerstats[k];
//						
//						strcpy(NameBuf, regents[k].name);
//						break;
//					}
//				}
			}
	    	else
	    	{
				CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(*AdvItor);
				AdvItor++;	/* advance to next */
		
		    	if (RecipStart+Line == HighlightedRecipient)
		    		hRecipientStats = pAvatar->hPlayerStats;
			
				SHORT who = pAvatar->hPlayerStats;
				for(SHORT k=0; k<CHARACTER_COUNT; ++k)
				{
					if (playerstats[k] == who)
					{
						strcpy(NameBuf, regents[k].mfGetname());
						break;
					}
				}
	    	}
	    	if (HighlightedRecipient == -1)
	    	    hRecipientStats = fERROR;
	
	    	if (HighlightedRecipient == RecipStart+Line)
			    gprint_text(TextX+TextMarginX,TextY+TextMarginY,NameBuf,31);
	    	else
			    gprint_text(TextX+TextMarginX,TextY+TextMarginY,NameBuf,175);
		}
	}
}


/* ========================================================================
   Function    - InventoryGUI::mfPaintMiscInfoBox
   Description - Paints the Miscellaneous Info box
   Returns     - void
   ======================================================================== */
void InventoryGUI::mfPaintMiscInfoBox(ObjectList & const Ol,LONG MenuId) const
{
	char NameBuf[300];
	LONG TextMarginX=4;  //margins!
	LONG TextMarginY=4;
	LONG MiscTextBoxX;
	LONG MiscTextBoxY;
	LONG MiscTextBoxW;
	LONG MiscTextBoxH;
	
	
	DumbAutoLockPtr<PLAYER_STATS > const pPS(AvatarStatsHdl);
	
	if (fPickingRecipient)
	{
		GetButtonPosition(MenuId,INV_BUT_INFO,&MiscTextBoxX,&MiscTextBoxY);
		
		// GEH 10/97 sprintf(NameBuf,"^W230^F03%s", "Select character to receive item.");
		sprintf(NameBuf,"^W230^F03%s", STRMGR_GetStr(STR_INV_GIVE_ITEM) );
		gprint_text(MiscTextBoxX+TextMarginX,MiscTextBoxY+TextMarginY,NameBuf,31);
	

		return;
	}

	GetButtonPosition(MenuId,INV_BUT_INFO,&MiscTextBoxX,&MiscTextBoxY);
	GetButtonSize(MenuId,INV_BUT_INFO,&MiscTextBoxW,&MiscTextBoxH);
	
	if (HighlightedLine!=-1)
	{
		InventoryItor itor(Ol);
		LONG i;
		
		BOOL showEmpty = (fDomainTurn || fPreparingSpells || CurrCategory != IC_SPELL);
		if (showEmpty)
			itor.mfAdvance(CurrCategory);
		else
			itor.mfAdvanceNonEmpty(CurrCategory);
		
		for (i = 0; i < HighlightedLine; ++i)
		{
			if (itor == Ol.end())
			{
				return;
			}

			if (showEmpty)	
				itor.mfAdvanceToNext(CurrCategory);
			else	
				itor.mfAdvanceToNextNonEmpty(CurrCategory);
		}
		if (itor == Ol.end())
		{
			return;
		}
		
		//auto-font-reduce until it fits!
		LONG font=3;
		do
		{
			sprintf(NameBuf,"^W230^F%02i%s",font--,STRMGR_GetStr(ItemTypes[(*itor)->mfType()].mfInfo()));
		} while (gtext_height(NameBuf)>(MiscTextBoxH-TextMarginY) && font!=0);

		
		gprint_text(MiscTextBoxX+TextMarginX,MiscTextBoxY+TextMarginY,NameBuf,31);
	}
	else if (fPreparingSpells && CurrCategory == IC_SPELL)
	{
    	NameBuf[0] = '\0';		// truncate buffer
    	
    	if (pPS->mfCanMemorize(SPELL_INFO::PRIEST, (SPELL_INFO::LEVEL)1) > 0)
    	{
	    	strcat(NameBuf, STRMGR_GetStr(STR_AVA_INV_MEM_PR));
	    	for (SHORT level = 1; level <= 7; ++level)
	    	{
	    		char buf[10];
	    		if (pPS->mfCanMemorize(SPELL_INFO::PRIEST, (SPELL_INFO::LEVEL) level) < 1)
	    			break;
	    		
	    		if (level != 1)	
	    			strcat(NameBuf, "/");
	    			
	    		SHORT remaining =
	    			pPS->mfCanMemorize(SPELL_INFO::PRIEST, (SPELL_INFO::LEVEL) level) -
	    			pPS->mfMemorized(SPELL_INFO::PRIEST, (SPELL_INFO::LEVEL) level);
	    			
	    		if (remaining < 0)
	    			remaining = 0;	
	    			
	    		sprintf(buf, "%d", remaining);
	    		strcat(NameBuf, buf);	
	    	}
	    	strcat(NameBuf, "\n");
	    }
    	if (pPS->mfCanMemorize(SPELL_INFO::WIZARD, (SPELL_INFO::LEVEL)1) > 0)
    	{
	    	strcat(NameBuf, STRMGR_GetStr(STR_AVA_INV_MEM_WZ));
	    	for (SHORT level = 1; level <= 9; ++level)
	    	{
	    		char buf[10];
	    		if (pPS->mfCanMemorize(SPELL_INFO::WIZARD, (SPELL_INFO::LEVEL) level) < 1)
	    			break;
	    		
	    		if (level != 1)	
	    			strcat(NameBuf, "/");
	    			
	    		SHORT remaining =
	    			pPS->mfCanMemorize(SPELL_INFO::WIZARD, (SPELL_INFO::LEVEL) level) -
	    			pPS->mfMemorized(SPELL_INFO::WIZARD, (SPELL_INFO::LEVEL) level);
	    			
	    		if (remaining < 0)
	    			remaining = 0;	
	    			
	    		sprintf(buf, "%d", remaining);
	    		strcat(NameBuf, buf);	
	    	}
	    	strcat(NameBuf, "\n");
	    }
		gprint_text(MiscTextBoxX+TextMarginX,MiscTextBoxY+TextMarginY,NameBuf,31);
	}
			
}
	

/* ========================================================================
   Function    - InventoryGUI::mfPaint
   Description - Paints the inventory for an avatar
   Returns     - void
   ======================================================================== */
void InventoryGUI::mfPaint(LONG MenuCombo,LONG)
{

	DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(AvatarStatsHdl);
	ObjectList & const AvatarInventory=pPlayerStats->mfGetInventory();
	
	LONG MenuId,ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	init_gfont(FONT_SANS_8PT);
	LONG OrigColor;



	
	if (fPickingRecipient)
	{
	    GetButtonLabelColor(MenuId, INV_BUT_TITLE, &OrigColor);
	    SetButtonLabel(MenuId, INV_BUT_TITLE, STR_AVA_INV_TITLEGIVE, OrigColor);
	    GetButtonLabelColor(MenuId, INV_BUT_STATUS, &OrigColor);
	    SetButtonLabel(MenuId, INV_BUT_STATUS, STR_NULL, OrigColor);
	}
	else if (fPreparingSpells)
	{
	    GetButtonLabelColor(MenuId, INV_BUT_TITLE, &OrigColor);
	    SetButtonLabel(MenuId, INV_BUT_TITLE, STR_AVA_INV_PREPARE_SPELLS, OrigColor);
	    GetButtonLabelColor(MenuId, INV_BUT_STATUS, &OrigColor);
	    SetButtonLabel(MenuId, INV_BUT_STATUS, STR_AVA_INV_STATUS, OrigColor);
	}
	else
	{
	    GetButtonLabelColor(MenuId, INV_BUT_TITLE, &OrigColor);
	    SetButtonLabel(MenuId, INV_BUT_TITLE,  STR_AVA_INV_INV, OrigColor);
	    GetButtonLabelColor(MenuId, INV_BUT_STATUS, &OrigColor);
	    SetButtonLabel(MenuId, INV_BUT_STATUS, STR_AVA_INV_STATUS, OrigColor);
	}
	
	mfPaintListBoxLine(6, AvatarInventory, MenuId);
	mfPaintMiscInfoBox(AvatarInventory,MenuId);
	mfPaintSpellBoxes(AvatarInventory,MenuId);
	mfPaintCategoryButtonText(MenuId);
	mfPaintActionButtonText(MenuId);
	mfPaintDudeNameText(MenuId);

	HlMgr.Service();						// service button highlights
	
	if (DrawRedBoxes)
		OutlineAllRegions();
}


/* ========================================================================
   Function    - InventoryGUI::mfInit
   Description - Initializes the instance
   Returns     - void
   ======================================================================== */
void InventoryGUI::mfInit(SHORT a)
{	
	//printf("setting avastatshdl to %li\n",a);
	AvatarStatsHdl=a;
	HighlightedLine=-1;
	DispStart=0;

	DrawRedBoxes=FALSE;
	mfChangeCurrCat(0,CurrCategory);
}


void InventoryGUI::mfAddToAvailSpells(LONG MC ,LONG d)
{
	if (fPickingRecipient)
	    return;
	
	DumbAutoLockPtr<PLAYER_STATS > const pPS(AvatarStatsHdl);
	ObjectList& Inv=pPS->mfGetInventory();
	
	if (CurrCategory==IC_SPELL && fPreparingSpells)
	{	
		HlMgr.HighlightButton(MC,1);
		InventoryItor itor(Inv);
		
		itor.mfAdvanceTo(DispStart+d,CurrCategory);
		
		if (itor == Inv.end())
		{
			return;
		}
			
		ITEMTYPE const spell_itemtype = (*itor)->mfType();
		if (pPS->mfCanIMemorizeThisSpell(spell_itemtype) == TRUE)
		{
			(*itor)->mfGainOne();
			if (fBattlePrep && Inv.mfIsBattleSpell(DispStart+d))
			{
				SHORT oHlL = HighlightedLine;
				HighlightedLine = DispStart+d;
				LONG box;
				for (box=0; box < 7; ++box)
				{
					if (Inv.SpellBoxes[box] == fERROR)
					{
						mfClearSpellBox(0,box);
						break;
					}
				}
				HighlightedLine = oHlL;
			}
		}
	}
	else
		mfAddToSpellBoxes(0,d);

}


void InventoryGUI::mfTakeFromAvailSpells(LONG MC,LONG d)
{	
	if (fPickingRecipient)
	    return;

	DumbAutoLockPtr<PLAYER_STATS > const pPlayerStats(AvatarStatsHdl);
	ObjectList& AvatarInventory=pPlayerStats->mfGetInventory();
	
	if (CurrCategory==IC_SPELL && fPreparingSpells)
	{

		HlMgr.HighlightButton(MC,1);
		// GWP AvatarInventory.mfAdvanceTo(DispStart+d,CurrCategory);
        // GWP if (AvatarInventory.mfData().mfHowMany() > 0)
		// GWP     AvatarInventory.mfData().mfLoseOne();
		InventoryItor itor(AvatarInventory);
		itor.mfAdvanceTo(DispStart+d, CurrCategory);
		if (itor != AvatarInventory.end() &&
		    (*itor)->mfHowMany() > 0)
		{
			(*itor)->mfLoseOne();
			
			// remove me from spellboxes if empty
			
			if ((*itor)->mfHowMany() <= 0)
			{
			    SHORT i;
			    for (i=0; i < MAX_SPELL_BOXES; ++i)
				if (AvatarInventory.SpellBoxes[i] == itor.mfGetCurrentInvIndex())
				{
					AvatarInventory.SpellBoxes[i] = -1;
				}
			}
		}
	}
	else
		mfAddToSpellBoxes(0,d);

}

void InventoryGUI::mfPurge(LONG,LONG)
{
	DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(AvatarStatsHdl);
	ObjectList& AvatarInventory=pPlayerStats->mfGetInventory();
	
	AvatarInventory.mfDeinit();
}

void InventoryGUI::mfUseHighlightedObject(LONG a,LONG)
{
	if (mouse_button==2)
	{
		SystemHelp(STR_INVN_BUTN_USE_BUTTON,STR_INVN_HELP_USE_BUTTON,H_Use_Button,NULL,0);
		return;
	}

	if (fPickingRecipient || fPreparingSpells)
	    return;
	
	DumbAutoLockPtr<PLAYER_STATS > const pPlayerStats(AvatarStatsHdl);
	
	if (pPlayerStats->mfGetCurHitPoints() <= 0) // dead men can't activate
	{
		// Don't clear the lock here if handle was locked before we
		// called this function.  (Two clears on same block causes fatal error
		// in debugger.)
		return;
	}
	
	ObjectList& AvatarInventory=pPlayerStats->mfGetInventory();
	if (HighlightedLine==-1)
	{
		return;
	}
		
	if (CurrCategory == IC_REALM_SPELL)
	{
		return;
	}

	//printf("highlighted use button %li\n",a&0xFFFF);
	HlMgr.HighlightButton(a&0xFFFF,1);
	
	// GWP AvatarInventory.mfAdvanceTo((SHORT)HighlightedLine,CurrCategory);
	
	InventoryItor itor(AvatarInventory);
	
	if (CurrCategory != IC_SPELL)
		itor.mfAdvanceTo((SHORT)HighlightedLine,CurrCategory);
	else
		itor.mfAdvanceToNonEmpty((SHORT)HighlightedLine,CurrCategory);
		
	if (itor == AvatarInventory.end())
	{
		return;
	}

	HlMgr.Clear();
	HideSubMenu(a,D_AVATAR_INVENTORY);
	RunMenus();
	
	// GWP AvatarInventory.mfActivateCurrent();
	itor.mfActivateCurrent();
}

static LONG CtxtHlpCats[]=	
{	
	H_Items_Button,
	H_Magic_Items_Button,
	H_Spells_Button,
	H_Realm_Spells,
	H_Blood_Abilities_Button,
};
									

void InventoryGUI::mfChangeCurrCat(LONG MenuCombo,LONG c)
{

	if (mouse_button==2)
	{
		if (c==IC_SPELL && fPreparingSpells)
			SystemHelp(STR_INVN_BUTN_SPELLS_BUTTON,STR_INVN_HELP_SPELLS_BUTTON_PREP,H_Spells_Button,NULL,0);
						
		SystemHelp(STR_INVN_BUTN_ITEMS_BUTTON+c,STR_INVN_HELP_ITEMS_BUTTON+c,CtxtHlpCats[c],NULL,0);
		return;
	}

	if (fPickingRecipient)
		return;
	
	LONG MenuId,ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	for (LONG i = INV_BUT_MUNDANE; i <= INV_BUT_BLOODABLT; ++i)
		HlMgr.SubdueButton(i);

	
	LONG newArg=BUILD_LONG(MenuId, INV_BUT_MUNDANE+c);
	

	HlMgr.HighlightButton(newArg,-1);
	
	CurrCategory=(ItemCategory)c;
	DispStart=0;
	HighlightedLine=-1;
}




void StaticInventoryGUI::mfDropItem(LONG a,LONG)
{

	if (mouse_button==2)
	{
		SystemHelp(STR_INVN_BUTN_DROP_BUTTON,STR_INVN_HELP_DROP_BUTTON,H_Drop_Button,NULL,0);
		return;
	}


	if (fPickingRecipient)
		return;

	SHORT AvatarStatsHdl=GUI.GetStatsHdl();
	
	DumbAutoLockPtr<PLAYER_STATS > const pPlayerStats(AvatarStatsHdl);
	ObjectList& AvatarInventory=pPlayerStats->mfGetInventory();
	
	InventoryItor itor(AvatarInventory);
	itor.mfAdvanceTo(GUI.HighlightedLine,GUI.CurrCategory);
	
	if (itor == AvatarInventory.end())
	{
		return;
	}
	
	
	if (fPreparingSpells && GUI.CurrCategory == IC_SPELL)
	{
		GUI.HlMgr.HighlightButton(a,1);
		if (itor != AvatarInventory.end() &&
		    (*itor)->mfHowMany() > 0)
		{
			(*itor)->mfLoseOne();
			if (fBattlePrep && AvatarInventory.mfIsBattleSpell(GUI.HighlightedLine))
			{
				LONG box;
				for (box=0; box < 7; ++box)
				{
					if (AvatarInventory.SpellBoxes[box] == fERROR)
					{
						GUI.mfClearSpellBox(0,box);
						break;
					}
				}
			}
		}
		return;
	}
	
	if (GUI.CurrCategory == IC_BLOODAB || GUI.CurrCategory == IC_SPELL ||
		GUI.CurrCategory == IC_REALM_SPELL)		// can't drop these things
	{
		return;
	}
	
	GUI.HlMgr.HighlightButton(a,1);
	
	if (!fDomainTurn)
	{
		CAvatar const * const pAv=(CAvatar const * const)BLKPTR(CAvatar::mfGetPlayerAvatarHandle());
		
		// GWP Insert bump and rotation code here!
		itor.mfDropItem(pAv->mfX(),pAv->mfY());
	}	
	else
	{
		/* we're not going to allow dropping outside of adventuring */
		// itor.mfDropItem();
	}

}
	

void StaticInventoryGUI::mfUseHighlightedObject(LONG a,LONG b)
{
	GUI.mfUseHighlightedObject(a,b);
}

void StaticInventoryGUI::mfGiveItem(LONG a,LONG)
{
	if (mouse_button==2)
	{
		SystemHelp(STR_INVN_BUTN_GIVE_BUTTON,STR_INVN_HELP_GIVE_BUTTON,H_Give_Button,NULL,0);
		return;
	}


	SHORT AvatarStatsHdl=GUI.GetStatsHdl();
	
	DumbAutoLockPtr<PLAYER_STATS > const pPS(AvatarStatsHdl);
	ObjectList& AvatarInventory=pPS->mfGetInventory();
	
	if (GUI.HighlightedLine == -1 || fPickingRecipient)
	{
		return;
	}
	
	if (fDomainTurn && GUI.CurrCategory == IC_SPELL && !fPreparingSpells)	/* in realm turn, not prepping */
	{
		return;
	}
	
	if (fDomainTurn && GUI.CurrCategory != IC_SPELL && master_game_type == GAME_ADVENTURE)
	{
		return;
	}

	GUI.HlMgr.HighlightButton(a,1);

	InventoryItor itor(AvatarInventory);
	itor.mfAdvanceTo(GUI.HighlightedLine,GUI.CurrCategory);
	
	if (itor == AvatarInventory.end())
	{
		return;
	}
	if (GUI.CurrCategory == IC_SPELL)
	{
		if (fPreparingSpells)
		{
			ITEMTYPE const spell_itemtype = (*itor)->mfType();
			if (pPS->mfCanIMemorizeThisSpell(spell_itemtype) == TRUE)
			{
				(*itor)->mfGainOne();
				if (fBattlePrep && AvatarInventory.mfIsBattleSpell(GUI.HighlightedLine))
				{
					LONG box;
					for (box=0; box < 7; ++box)
					{
						if (AvatarInventory.SpellBoxes[box] == fERROR)
						{
							GUI.mfClearSpellBox(0,box);
							break;
						}
					}
				}
			}
		}
	}
	else if (GUI.CurrCategory != IC_BLOODAB && GUI.CurrCategory != IC_REALM_SPELL)
	{
		if ((*itor) == NULL || (*itor)->mfWorn())	// can't give items being worn
			return;
			
		// pick the person to give to
		itemGiven = (*itor)->mfType();
		givenFrom = AvatarStatsHdl;
		numCharges = (*itor)->mfGetCharges();
		itor.mfDropItem();
		fPickingRecipient = TRUE;
		GUI.RecipStart = 0;
		GUI.HighlightedRecipient = -1;
		
		if (!fPreparingSpells)	// perhaps no list yet -- build it
		{
			localActiveRegent = realm[HomeRealm].mfGetRegent();
			BuildAdventureList();
		}
			
	}
}



void StaticInventoryGUI::mfGoStatus(LONG a,LONG Combo)
{
	if (fPickingRecipient)
	{
		return;
	}
	
	StatusFromInventory(a, Combo);
}


void StaticInventoryGUI::mfDone(LONG a,LONG Combo)
{
	if (fPickingRecipient)
	{
		if (hRecipientStats == fERROR)
			hRecipientStats = givenFrom;
			
		if (hRecipientStats != fERROR && itemGiven != ITEM_NO_THING)
		{
			DumbAutoLockPtr<PLAYER_STATS > pRS(hRecipientStats);
			AddItemStatus ais=pRS->mfGetInventory().mfAddItem(itemGiven);
			
			if (!(ais&ADD_OK))
			{
				// Failed to go, put it back.
				TitledMessageBox::Display(ItemTypes[itemGiven].mfGetName(),STRMGR_GetStr(STR_ITEM_RECIP_TOO_MANY));
				pRS = givenFrom;
				ais = pRS->mfGetInventory().mfAddItem(itemGiven);
			}
			if (numCharges > 0 && (ais & ADD_OK))
			{
				InventoryItor itor(pRS->mfGetInventory());
				if (itor.mfFind(itemGiven))
				{
					(*itor)->mfSetCharges(numCharges);
				}
			}
		}
		GUI.HighlightedLine = -1;
		
		// Now reset the vars back to no information.
		fPickingRecipient = FALSE;
		numCharges = 0;
		givenFrom = fERROR;
		itemGiven = ITEM_NO_THING;
		hRecipientStats = fERROR;
	}
	else
	{
		SetButtonHilight(D_AVATAR_INVENTORY, INV_BUT_DONE, TRUE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		SetButtonHilight(D_AVATAR_INVENTORY, INV_BUT_DONE, FALSE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		
		fPreparingSpells = FALSE;
		HideSubMenu(a, D_AVATAR_INVENTORY);
		RunMenus();
		// HACK HACK so that panels with new spell keys will refresh.
		fUpdatePanels = TRUE;
	}
}


//high word of second arg is whether or not we're coming from the status menu
//low word is handle to display (0 for global handle)

void StaticInventoryGUI::mfDisplay(LONG a,LONG Combo)
{
	
	SHORT FromStatus,ShowThisHandle;
	SPLIT_LONG(Combo,FromStatus,ShowThisHandle);
	
	if (ShowThisHandle!=fERROR)
  	{
		GUI.mfInit(ShowThisHandle);
		//printf("displaying Handle %li\n",ShowThisHandle);


		LONG Arg=BUILD_LONG(D_AVATAR_INVENTORY,INV_BUT_MUNDANE+GUI.CurrCategory);
	
		DumbAutoLockPtr<PLAYER_STATS> pPS(ShowThisHandle);
		if ((fBattleCombat || fPreparingSpells) &&
			pPS->mfIsMagicUser())
		{
			GUI.mfChangeCurrCat(Arg,IC_SPELL);
		}
		
		if (GUI.CurrCategory<IC_MUNDANE || GUI.CurrCategory>IC_BLOODAB)
			GUI.mfChangeCurrCat(Arg,IC_MAGITEM);
		
		GUI.mfDisplay(a,FromStatus);
	}
	else
		printf("couldn't show invntory because handle was fERROR\n");
}

/* ========================================================================
   Function    - StaticInventoryGUI::mfAddItem
   Description - Adds an Item to the Inventory on the screen
   Returns     - void
   ======================================================================== */
void StaticInventoryGUI::mfAddItem(LONG,LONG Itype)
{
	SHORT AvatarStatsHdl=GUI.GetStatsHdl();
	DumbAutoLockPtr<PLAYER_STATS > const pPlayerStats(AvatarStatsHdl);
	ObjectList& AvatarInventory=pPlayerStats->mfGetInventory();
	
	printf("adding %s to player's inventory\n",GAME_TTYPE::mfGetDescription(ItemTypes[Itype].mfGameType()));
	
	AvatarInventory.mfAddItem((ITEMTYPE)Itype);
}

