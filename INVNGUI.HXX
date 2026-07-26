#ifndef _INVNGUI_HXX
#define _INVNGUI_HXX

/* ========================================================================
   Copyright (c) 1990,1996   Synergistic Software
   All Rights Reserved.
   ========================================================================
   Filename: InvnGUI.hxx -Header for the Inventory GUI controller
   Author: Wes Cumberland
   ========================================================================
   Contains the following class declarations functions: 
   InventoryGUI -A class to handle the interface between the player and an
                 avatar's inventory. It is it's own class, because there is 
                 no reason why every ObjectList class needs to store GUI 
                 details.  Only one, or two of this class should ever be
                 instantiated at the same time. (This is not enforced, though)

   StaticInventoryGUI -A wrapper class to provide a quick and easy interface
                       to the actual PLAYER'S avatar's inventory. Rather than
                       making the client jump through all the hoops of getting
                       the player's inventory from a resource handle, this 
                       class does it for them.
                       
   Contains the following inline general functions:
   (arguments only specified in overloaded cases)

   StaticInventoryGUI::
    mfPaint                -Paints the player's inventory
    mfDisplay              -Shows the player's inventory menu
    mfScroll               -Scrolls the player's inventory listbox
    mfUseHighlightedObject -Uses the player's highlighted object
    mfSetHighlightedObject -Sets the player's highlighted object
    mfChangeCurrCat        -Changes the player's item category to display
    mfSetNextSpellBox      -Sets the player's next spell box
    mfClearSpellBox        -Clears one of the player's spellboxes
    mfAddToSpellBoxes      -Sets one of the player's blank spell boxes
    mfUseSpellBox          -Acvtivates one of the player's spell boxes
    mfAddToActiveInventory -Adds an item to one of the player's boxes
    mfToggleDemBoxes       -Toggles the Outline of the regions on the menu
    mfPrintWeaponInfo      -Prints weapon info about the player's current weap
    mfPrintArmorInfo       -Prints armor info about the player's current armor

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "TYPEDEFS.H"
#endif

#if !defined(_MENUPROT_H)
#include "MENUPROT.H"
#endif

#if !defined(_PLAYSTATS_HXX)
#include "PLAYSTAT.HXX"
#endif

#if !defined(_INVNTORY_HXX)
#include "INVNTORY.HXX"
#endif

#if !defined(_HANDLE_HXX)
#include "HANDLE.HXX"
#endif

#if !defined(_INVNBUTS_HXX)
#include "INVNBUTS.HXX"
#endif

#if !defined(_GMENUENM_H)
#include "GMENUENM.H"
#endif

#if !defined(_LISTBOX_HXX)
#include "LISTBOX.HXX"
#endif

#if !defined(_HMGR_HXX)
#include "HMGR.HXX"			//button highlight manager.
#endif

#if !defined(_STRENUM_H)
#include "strenum.h"
#endif

#if !defined(_STRMGR_H)
#include "STRMGR.H"
#endif

#if !defined(_INFOBOX_HXX)
#include "INFOBOX.HXX"
#endif

extern BOOL fBattleCombat;

extern BOOL fPreparingSpells;

/* ------------------------------------------------------------------------
   Classes
   ------------------------------------------------------------------------ */

class StaticInventoryGUI;
class ObjectList;

/* ========================================================================
   Class       - InventoryGUI
   Description - A class to handle the interface between the player and an
                 avatar's inventory. It is it's own class, because there is
                 no reason why every ObjectList class needs to store GUI
				 details.  Only one, or two of this class should ever be
				 instantiated at the same time. (This is not enforced, though)
   Size        - 8 bytes
   ======================================================================== */
class InventoryGUI
{
		friend class StaticInventoryGUI;
	private:
		SHORT DispStart;
		SHORT HighlightedLine;
		ItemCategory CurrCategory;
		SHORT AvatarStatsHdl;
		SHORT RecipStart;
		SHORT HighlightedRecipient;

		void mfPaintSpellBoxes(ObjectList const & ,LONG) const;
		void mfPaintListBoxLine(LONG,ObjectList & ,LONG) const;
		void mfPaintListBoxIcon(LONG,ObjectList const & ,LONG) const;
		void mfPaintMiscInfoBox(ObjectList & ,LONG) const;
		void mfPaintArmsBoxes(ObjectList&,LONG);
		void mfPaintCategoryButtonText(LONG) const;
		void mfPaintDudeNameText(LONG) const;
		void mfPaintActionButtonText(LONG) const;
		
	public:

		void mfPaint(LONG,LONG);
		void mfDisplay(LONG,LONG);
		void mfScroll(LONG,LONG);
		void mfUseHighlightedObject(LONG,LONG);
		void mfSetHighlightedLine(LONG,LONG d);
		void mfChangeCurrCat(LONG,LONG c);
		void mfAddToSpellBoxes(LONG,LONG);
		void mfClearSpellBox(LONG,LONG);
		void mfUseSpellBox(LONG,LONG);
		void mfAddToAvailSpells(LONG,LONG);
		void mfTakeFromAvailSpells(LONG,LONG);
		
		void mfPurge(LONG,LONG);
		
		InventoryGUI();
		void mfInit(SHORT);
		SHORT GetStatsHdl(){return AvatarStatsHdl;}

		BOOL DrawRedBoxes;
		HighlightMgr HlMgr;
		
};
		
/* ========================================================================
   Class       - StaticInventoryGUI
   Description - A wrapper class to provide the menus a way to interface
				 with the actual avatar's inventory.
   Size        - 0 bytes
   ======================================================================== */
class StaticInventoryGUI
{
	private:
		static InventoryGUI GUI;
	public:
		static void mfPaint(LONG,LONG);
		static void mfDone(LONG,LONG);
		static void mfGoStatus(LONG,LONG);
		static void mfDisplay(LONG,LONG);
		static void mfScroll(LONG,LONG);
		static void mfUseHighlightedObject(LONG,LONG);
		static void mfSetHighlightedLine(LONG,LONG);
		static void mfChangeCurrCat(LONG,LONG);
		static void mfClearSpellBox(LONG,LONG);
		static void mfAddToSpellBoxes(LONG,LONG);
		static void mfUseSpellBox(LONG,LONG);
		static void mfToggleDemBoxes(LONG,LONG);
		static void mfPrintWeaponInfo(LONG,LONG);
		static void mfPrintArmorInfo(LONG,LONG);
		static void mfDropItem(LONG,LONG);
		static void mfGiveItem(LONG,LONG);
		static void mf(LONG,LONG); //cool....
		static void mfAddItem(LONG,LONG); //cool....
		static void mfAddToAvailSpells(LONG,LONG);
		static void mfTakeFromAvailSpells(LONG,LONG);
		static void Hide(LONG,LONG);
		static void ShowStatus(LONG,LONG);
		static void mfLoad(LONG,LONG);
		static void mfPurge(LONG,LONG);
		
};

/* ------------------------------------------------------------------------
   Inline Functions
   ------------------------------------------------------------------------ */

inline InventoryGUI::InventoryGUI() 
:	DispStart(0),
	HighlightedLine(-1),
	CurrCategory(IC_MAGITEM),
	AvatarStatsHdl(fERROR),
	DrawRedBoxes(FALSE),
	HlMgr(D_AVATAR_INVENTORY)
{
}

inline void InventoryGUI::mfDisplay(LONG,LONG FromStatus)
{	

	if (FromStatus)
		HideMenu(D_AVATAR_ATTRIBUTES); //get rid of status
	
	if (AvatarStatsHdl==fERROR)
		fatal_error("tried to display invalid invntory\n");
	
	//Store the handle to the status in the menu.
	SetButtonValue(D_AVATAR_INVENTORY, INV_BUT_STATUS, AvatarStatsHdl);
	
	ShowMenu(D_AVATAR_INVENTORY); //show the actual inventory pcx
	
}

inline void StaticInventoryGUI::mfPaint(LONG a,LONG b)
{
	GUI.mfPaint(a,b);
}

inline void StaticInventoryGUI::mfScroll(LONG a,LONG b)
{
	GUI.mfScroll(a,b);
}

inline void StaticInventoryGUI::mfSetHighlightedLine(LONG a,LONG b)
{
	GUI.mfSetHighlightedLine(a,b);
}

inline void StaticInventoryGUI::mfChangeCurrCat(LONG a,LONG b)
{
	GUI.mfChangeCurrCat(a,b);
}
inline void StaticInventoryGUI::mfClearSpellBox(LONG a,LONG b)
{
	GUI.mfClearSpellBox(a,b);
}
inline void StaticInventoryGUI::mfAddToSpellBoxes(LONG a,LONG b)
{
	GUI.mfAddToSpellBoxes(a,b);
}
inline void StaticInventoryGUI::mfUseSpellBox(LONG a,LONG b)
{
	GUI.mfUseSpellBox(a,b);
}

inline void StaticInventoryGUI::mfToggleDemBoxes(LONG,LONG)
{
	GUI.DrawRedBoxes=!GUI.DrawRedBoxes;
}

inline void StaticInventoryGUI::mfPrintArmorInfo(LONG,LONG)
{

}

inline void StaticInventoryGUI::mfPrintWeaponInfo(LONG,LONG)
{

}

inline void StaticInventoryGUI::mfAddToAvailSpells(LONG a,LONG b)
{
	GUI.mfAddToAvailSpells(a,b);
}

inline void StaticInventoryGUI::mfTakeFromAvailSpells(LONG a,LONG b)
{								 
	GUI.mfTakeFromAvailSpells(a,b);
}

inline void StaticInventoryGUI::mfPurge(LONG a,LONG b)
{
	GUI.mfPurge(a,b);
}

inline void StaticInventoryGUI::ShowStatus(LONG a,LONG b)
{
	GUI.HlMgr.HighlightButton(a,1);
	//need a delay!	

	GUI.HlMgr.Clear();
	ShowSubMenu(a,D_AVATAR_ATTRIBUTES);

}

inline void StaticInventoryGUI::Hide(LONG a,LONG b)
{
	GUI.HlMgr.HighlightButton(a,1);
		
	//need a delay!	
	GUI.HlMgr.Clear();
	HideSubMenu(a,D_AVATAR_INVENTORY);
}

inline void StaticInventoryGUI::mfLoad(LONG,LONG)
{
	DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(GUI.GetStatsHdl());
	ObjectList& Ol=pPlayerStats->mfGetInventory();
	Ol.mfLoad();
}

#endif