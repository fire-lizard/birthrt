/* ========================================================================
   Copyright (c) 1997	Synergistic Software
   All Rights Reserved
   ========================================================================
	Character editor for Birthright: Inventory module.
   Filename: CHAREDIN.CPP
   Author:	 John Conley
   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#ifdef _WINDOWS								// Windows is required for this module

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Windows.h>
#include <windowsx.h>

#include "SYSTEM.H"
#include "ENGINE.H"

#include "ITEMTYPE.HXX"

#include "CHARED.HXX"
#include "CHAREDSY.H"						// symbols for CHARED dialogs
#include "AVA.HXX"

/* ------------------------------------------------------------------------
	External data
   ------------------------------------------------------------------------ */
extern HINSTANCE hInstApp;
extern HPALETTE hpalApp;				// in WINSYS\SYSGRAPH.C
extern HWND hwndApp;
extern AVA_FILE		gAvaData;

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define _MAX_ITEM_LEN			80
#define _MAX_SELECTIONS			40
#define BUFLEN 					120

#define RELIC_CODE 999

/* ------------------------------------------------------------------------
	Macros
   ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------
	Structures
   ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------
   Global and Module-Level Variables
   ------------------------------------------------------------------------ */
static int gmaxMagicItems;
static int gmaxBloodAb;
static int gmaxRealmSpells;
static int gmaxSpellLevel;

// ITEMS array.
// giItems is a simple list of indexes to all items, EXCEPT relics of
// power, possessed by this character.
static int giItems[_MAX_SELECTIONS * 4];
static int giItemIndex;

// RELICS array.
// giRelics is a simple list of indexes to relics of power possessed by
// this character.  As of 5-21-97, this list cannot be changed by the user.
static int giRelics[_MAX_SELECTIONS];
static int giRelicIndex;

// Table of FORBIDDEN ITEMS: these are items (including spells) we don't
// support, so they aren't allowed in the list boxes.
static ITEMTYPE gaForbiddenItems[] = {
	ITEM_AMULET_OF_WATER_WALKING,
	ITEM_SPELL_WATER_WALK
};
static int gcForbiddenItems = sizeof (gaForbiddenItems) / sizeof (ITEMTYPE);

// Table of SPELL MAXIMUMS.
static LONG gWizardMaxSpellLevel[] = {
// spell level		wizard level
	0,					// 0
	1,					// 1
	1,					// 2
	2,					// 3
	2,					// 4
	3,					// 5
	3,					// 6
	4,					// 7
	4,					// 8
	5,					// 9
	5,					// 10
	5,					// 11
	6,					// 12
	6,					// 13
	7,					// 14
	7,					// 15
	8,					// 16
	8,					// 17
	9,					// 18
	9,					// 19
	9					// 20
};

static LONG gBardMaxSpellLevel[] = {
// spell level		bard level
	0,					// 0
	0,					// 1
	1,					// 2
	1,					// 3
	2,					// 4
	2,					// 5
	2,					// 6
	3,					// 7
	3,					// 8
	3,					// 9
	4,					// 10
	4,					// 11
	4,					// 12
	5,					// 13
	5,					// 14
	5,					// 15
	6,					// 16
	6,					// 17
	6,					// 18
	6,					// 19
	6					// 20
};


// More tables of SPELL MAXIMUMS.
// Copied from SPELLPRO.CPP.
#define MAX_WIZARD_SPELL_LEVEL	9
// Note: Add one so that Spell level is direct access to the array.
typedef char WIZARD_SPELL_PROGRESSION[MAX_WIZARD_SPELL_LEVEL + 1];

// Number of spells that can be known by spell level and wizard level.
// AD&D Handbook p.43 Table 21
static WIZARD_SPELL_PROGRESSION WizardSpellProgressionTable[] = {
//   0  1  2  3  4  5  6  7  8  9 (Spell Level)
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	//  Wizard Level 0 - so that direct access by Wizard level is done.
	{ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},	//  1
	{ 0, 2, 0, 0, 0, 0, 0, 0, 0, 0},	//  2
	{ 0, 2, 1, 0, 0, 0, 0, 0, 0, 0},	//  3
	{ 0, 3, 2, 0, 0, 0, 0, 0, 0, 0},	//  4
	{ 0, 4, 2, 1, 0, 0, 0, 0, 0, 0},	//  5
	{ 0, 4, 2, 2, 0, 0, 0, 0, 0, 0},	//  6
	{ 0, 4, 3, 2, 1, 0, 0, 0, 0, 0},	//  7
	{ 0, 4, 3, 3, 2, 0, 0, 0, 0, 0},	//  8
	{ 0, 4, 3, 3, 2, 1, 0, 0, 0, 0},	//  9
	{ 0, 4, 4, 3, 2, 2, 0, 0, 0, 0},	// 10
	{ 0, 4, 4, 4, 3, 3, 0, 0, 0, 0},	// 11
	{ 0, 4, 4, 4, 4, 4, 1, 0, 0, 0},	// 12
	{ 0, 5, 5, 5, 4, 4, 2, 0, 0, 0},	// 13
	{ 0, 5, 5, 5, 4, 4, 2, 1, 0, 0},	// 14
	{ 0, 5, 5, 5, 5, 5, 2, 1, 0, 0},	// 15
	{ 0, 5, 5, 5, 5, 5, 3, 2, 1, 0},	// 16
	{ 0, 5, 5, 5, 5, 5, 3, 3, 2, 0},	// 17
	{ 0, 5, 5, 5, 5, 5, 3, 3, 2, 1},	// 18
	{ 0, 5, 5, 5, 5, 5, 3, 3, 3, 1},	// 19
	{ 0, 5, 5, 5, 5, 5, 4, 3, 3, 2}		// 20
};

#define MAX_BARD_SPELL_LEVEL	6
// Note: Add one so that Spell level is direct access to the array.
typedef char BARD_SPELL_PROGRESSION[MAX_BARD_SPELL_LEVEL + 1];

// Number of Wizard spells that can be known by spell level and bard level.
// AD&D Handbook p.59 Table 32
static BARD_SPELL_PROGRESSION BardSpellProgressionTable[] = {
//   0  1  2  3  4  5  6 (Spell Level)
	{ 0, 0, 0, 0, 0, 0, 0},	//  Bard Level 0 - so that direct access by Wizard level is done.
	{ 0, 0, 0, 0, 0, 0, 0},	//  1
	{ 0, 1, 0, 0, 0, 0, 0},	//  2
	{ 0, 2, 0, 0, 0, 0, 0},	//  3
	{ 0, 2, 1, 0, 0, 0, 0},	//  4
	{ 0, 3, 1, 0, 0, 0, 0},	//  5
	{ 0, 3, 2, 0, 0, 0, 0},	//  6
	{ 0, 3, 2, 1, 0, 0, 0},	//  7
	{ 0, 3, 3, 1, 0, 0, 0},	//  8
	{ 0, 3, 3, 2, 0, 0, 0},	//  9
	{ 0, 3, 3, 2, 1, 0, 0},	// 10
	{ 0, 3, 3, 3, 1, 0, 0},	// 11
	{ 0, 3, 3, 3, 2, 0, 0},	// 12
	{ 0, 3, 3, 3, 2, 1, 0},	// 13
	{ 0, 3, 3, 3, 3, 1, 0},	// 14
	{ 0, 3, 3, 3, 3, 2, 0},	// 15
	{ 0, 4, 3, 3, 3, 2, 1},	// 16
	{ 0, 4, 4, 3, 3, 3, 1},	// 17
	{ 0, 4, 4, 4, 3, 3, 2},	// 18
	{ 0, 4, 4, 4, 4, 3, 2},	// 19
	{ 0, 4, 4, 4, 4, 4, 3}	// 20
};

// gcSpellsThisLevel is how many spells we currently have for each level.
static int gcSpellsThisLevel[MAX_WIZARD_SPELL_LEVEL + 1];

// gcMaxSpells is how many spells we CAN have for each level.
static int gcMaxSpells[MAX_WIZARD_SPELL_LEVEL + 1];

// gcSpellsInData is how many spells we have implemented for each level.
// (This is sometimes less than gcMaxSpells.)
static int gcSpellsInData[MAX_WIZARD_SPELL_LEVEL + 1];

/* ========================================================================
   Function    - Sorry
   Description - Put a message in a message box and wait for a key.
   Returns     - Nothing
	Note:			- Does not abort.
   ======================================================================== */
static void Sorry (const char *szFormat, ...)
{
	char			szMessage[200];
	va_list		argp;
	
	va_start (argp, szFormat);
	vsprintf (szMessage, szFormat, argp);
	MessageBeep (MB_ICONEXCLAMATION);
	MessageBox (NULL, szMessage, "Sorry", MB_OK | MB_TASKMODAL);
}


// ---------------------------------------------------------------------------
static void PurgeDuplicateStrings (HWND hwnd)
{
	int			i;
	int			cItems;
	char			szItem[_MAX_ITEM_LEN + 1];
	char			szOldItem[_MAX_ITEM_LEN + 1];

	cItems = ListBox_GetCount (hwnd);
	szOldItem[0] = 0;
	for (i = 0; i < cItems;)
	{
		ListBox_GetText (hwnd, i, szItem);
		if (strcmp (szItem, szOldItem) == 0)
		{
			cItems = ListBox_DeleteString (hwnd, i);
		}
		else
		{
			strcpy (szOldItem, szItem);
			i++;
		}
	}
}


// ---------------------------------------------------------------------------
static BOOL IsForbidden (int itemType)
{
	int			i;

	for (i = 0; i < gcForbiddenItems; i++)
	{
		if (itemType == gaForbiddenItems[i])
			return TRUE;
	}
	return FALSE;
}


// ---------------------------------------------------------------------------
static void PutStringsInInventoryListBoxes (HWND hwnd)
{
// Put items to choose from in appropriate list boxes.

	ItemType		item;
	int			i;
	// HWND			hwndCtlMundane;
	HWND			hwndCtlMagicItem;
	HWND			hwndCtlSpell;
	HWND			hwndCtlRealmSpell;
	HWND			hwndCtlBloodAb;

	// hwndCtlMundane    = GetDlgItem (hwnd, IDL_SELECT_ITEMS);
	hwndCtlMagicItem  = GetDlgItem (hwnd, IDL_SELECT_MAGIC_ITEMS);
	hwndCtlBloodAb    = GetDlgItem (hwnd, IDL_SELECT_BLOOD_ABILITIES);
	hwndCtlSpell      = GetDlgItem (hwnd, IDL_SELECT_SPELLS);
	hwndCtlRealmSpell = GetDlgItem (hwnd, IDL_SELECT_REALM_SPELLS);

	// Start the for loop at 1 because item 0 is nothing.
	for (i = 1; i < MAX_ITEM_TYPES; i++)
	{
		item = ItemTypes[i];
		switch (item.mfCategory ())
		{
			// Why fool with stuff like bowls?
			// case IC_MUNDANE:
			// 	ListBox_InsertString (hwndCtlMundane, -1, item.mfGetName ());
			// 	break;

			case IC_MAGITEM:
				// The ITEM_DIERDRIENS_RING test excludes RELICS.
				if (i < ITEM_DIERDRIENS_RING && !IsForbidden (i))
					ListBox_AddString (hwndCtlMagicItem, item.mfGetName ());
				break;

			case IC_SPELL:
				if (gAvaData.mfIsWizardOrBard () && i < ITEM_SPELL_CURE_LIGHT &&
					!IsForbidden (i))
				{
					SPELL_INFO::TYPE spellIndex;
					SPELL_INFO::SPELL_ITEM spell;
					char szSpellName[128];

					spellIndex = (SPELL_INFO::TYPE) item.mfGetExtendedData ();
               spell = SPELL_INFO::mfGetSpell (spellIndex);
					if (spell.Level <= gmaxSpellLevel)
					{
						// ListBox_AddString (hwndCtlSpell, item.mfGetName ());
						// ListBox_InsertString (hwndCtlSpell, -1, item.mfGetName ());
						sprintf (szSpellName, "(%d) %s", spell.Level, item.mfGetName ());
						ListBox_AddString (hwndCtlSpell, szSpellName);
					}
				}
				break;

			case IC_REALM_SPELL:
				if (gAvaData.mfIsWizardOrBard () && i < ITEM_SPELL_BLESS_LAND &&
					!IsForbidden (i))
				{
					ListBox_AddString (hwndCtlRealmSpell, item.mfGetName ());
				}
				break;

			case IC_BLOODAB:
				// Blood abilities.  Omit the abilities past ITEM_TRAVEL,
				// because they never got implemented.
				if (GetBloodlineGrade () == (BLOODLINE_STRENGTH_INFO::TYPE) TAINTED)
				{
					ListBox_AddString (hwndCtlBloodAb, "None available");
				}
				else if ((i <= ITEM_TRAVEL && IsBloodabAvail (ITEMTYPE(i))) &&
					!IsForbidden (i))
				{
					ListBox_AddString (hwndCtlBloodAb, item.mfGetName ());
				}
				break;
		}
	}

	// Now purge the duplicate strings.
	PurgeDuplicateStrings (hwndCtlMagicItem );
	PurgeDuplicateStrings (hwndCtlSpell     );
	PurgeDuplicateStrings (hwndCtlRealmSpell);
	PurgeDuplicateStrings (hwndCtlBloodAb   );
}


// ---------------------------------------------------------------------------
static void PostCurrentInventory (HWND hwnd)
{
// Put the items from the "giItems" array into the appropriate list boxes.
// This is what the character has in his inventory.

	ItemType		item;
	int			i;
	int			itemIndex;
	HWND			hwndCtlMagicItem;
	HWND			hwndCtlSpell;
	HWND			hwndCtlRealmSpell;
	HWND			hwndCtlBloodAb;

	hwndCtlMagicItem  = GetDlgItem (hwnd, IDL_LIST_MAGIC_ITEMS);
	hwndCtlBloodAb    = GetDlgItem (hwnd, IDL_LIST_BLOOD_ABILITIES);
	hwndCtlSpell      = GetDlgItem (hwnd, IDL_LIST_SPELLS);
	hwndCtlRealmSpell = GetDlgItem (hwnd, IDL_LIST_REALM_SPELLS);

	for (i = 0; i < giItemIndex; i++)
	{
		itemIndex = giItems[i];
		item = ItemTypes[itemIndex];
		switch (item.mfCategory ())
		{
			case IC_MAGITEM:
				ListBox_AddString (hwndCtlMagicItem, item.mfGetName ());
				break;

			case IC_SPELL:
				if (gAvaData.mfIsWizardOrBard ())
				{
					SPELL_INFO::TYPE spellIndex;
					SPELL_INFO::SPELL_ITEM spell;
					char szSpellName[128];

					spellIndex = (SPELL_INFO::TYPE) item.mfGetExtendedData ();
               spell = SPELL_INFO::mfGetSpell (spellIndex);
					// ListBox_AddString (hwndCtlSpell, item.mfGetName ());
					sprintf (szSpellName, "(%d) %s", spell.Level, item.mfGetName ());
					gcSpellsThisLevel[spell.Level]++;
					ListBox_AddString (hwndCtlSpell, szSpellName);
				}
				break;

			case IC_REALM_SPELL:
				if (gAvaData.mfIsWizardOrBard ())
					ListBox_AddString (hwndCtlRealmSpell, item.mfGetName ());
				break;

			case IC_BLOODAB:
				if ((IsBloodabAvail (ITEMTYPE(itemIndex))) && !IsForbidden (itemIndex))
					ListBox_AddString (hwndCtlBloodAb, item.mfGetName ());
				break;
		}
	}
}


// ---------------------------------------------------------------------------
static int GetItemIndex (char * szItem, int iStart)
{
// Helper of PostOneWindow.
// Given an item name, return the index into ItemTypes of the item.
// Use brute-force lookup.  It is fast enough for this application.
// The szItem string came from the ItemTypes array in the first place,
// so there should be an exact match.
// This routine returns the index of the FIRST matching item that it finds.
// It matches strings, because that's all we know from the dialog boxes.
// The "iStart" parameter is needed because some items in different
// categories have identical strings.  If we always started at the beginning,
// then, for example, if you sent this routine the BLOOD ABILITY string
// "Protection from evil," the routine would return index 184, which is
// the SPELL "Protection from evil."

	int			i;

	for (i = iStart; i < MAX_ITEM_TYPES; i++)
	{
		if (strcmp (szItem, ItemTypes[i].mfGetName ()) == 0)
			return i;
	}

#if defined (_DEBUG)
	fatal_error ("Index not found in GetItemIndex.");
#endif

	return 0;									// error
}


// ---------------------------------------------------------------------------
static void PostOneWindow (HWND hwnd, int windowID, int iStart, BOOL fIndexed)
{
// Helper of PostSelectedInventory.
// iStart is the first item in the array of game items to start with.
// fIndexed, if TRUE, means the item string is preceded by a number in
// parentheses; this is for spell levels.

	int			i;
	int			index;
	int			cItems;
	HWND			hwndCtl;
	char			szItem[_MAX_ITEM_LEN + 1];
	char *		pszItem;

	hwndCtl = GetDlgItem (hwnd, windowID);
	cItems = ListBox_GetCount (hwndCtl);
	for (i = 0; i < cItems; i++)
	{
		ListBox_GetText (hwndCtl, i, szItem);
		if (fIndexed)
		{
			// For spells, skip past the (n) level info at the beginning of
			// the string.  (If the info is not found, return nothing).
			pszItem = strchr (szItem, ')');
			if (pszItem != NULL)
			{
				pszItem += 2;
				index = GetItemIndex (pszItem, iStart);
			}
			else
			{
				index = 0;
			}
		}
		else
		{
			// Everything but spells.
		   index = GetItemIndex (szItem, iStart);
		}

		if (index > 0)
		{
			giItems[giItemIndex] = index;
			giItemIndex++;
		}
	}
}


// ---------------------------------------------------------------------------
static void PostSelectedInventory (HWND hwnd)
{
// Post the items from the "SELECTED" list boxes to the giItems array.

	giItemIndex = 0;

	PostOneWindow (hwnd, IDL_LIST_MAGIC_ITEMS, ITEM_AMULET_OF_FEATHER_FALL, FALSE);
	PostOneWindow (hwnd, IDL_LIST_BLOOD_ABILITIES, ITEM_BATTLEWISE, FALSE);
	if (gAvaData.mfIsWizardOrBard ())
	{
		PostOneWindow (hwnd, IDL_LIST_SPELLS, ITEM_SPELL_DETECT_MAGIC, TRUE);
		PostOneWindow (hwnd, IDL_LIST_REALM_SPELLS, ITEM_SPELL_ALCHEMY, FALSE);
	}
}


// ---------------------------------------------------------------------------
static void CalculateMaxSpellsSub (LONG charClass, LONG charLevel)
{
// Helper of CalculateMaxSpells.

	LONG			spellLevel;
	SHORT			cSpells;

	if (charLevel > 20)
	{
#if defined (_DEBUG)
		fatal_error ("charLevel (%d) out of bounds in CalculateMaxSpellsSub", charLevel);
#endif
		charLevel = 20;						// in release version, soldier on
	}

	if (charClass == CLASS_WIZARD)
	{
		for (spellLevel = 0; spellLevel <= MAX_WIZARD_SPELL_LEVEL; spellLevel++)
		{
			if (gcMaxSpells[spellLevel] < WizardSpellProgressionTable[charLevel][spellLevel])
				gcMaxSpells[spellLevel] = WizardSpellProgressionTable[charLevel][spellLevel];
		}
	}
	else if (charClass == CLASS_BARD)
	{
		for (spellLevel = 0; spellLevel <= MAX_BARD_SPELL_LEVEL; spellLevel++)
		{
			if (gcMaxSpells[spellLevel] < BardSpellProgressionTable[charLevel][spellLevel])
				gcMaxSpells[spellLevel] = BardSpellProgressionTable[charLevel][spellLevel];
		}
	}
}


static void CalculateMaxSpells ()
{
// Update the global table gcMaxSpells.  If we have a weird character
// wizard/bard, note that a higher level bard can sometimes have more spells
// of a given level than a lower-level wizard, but at the same time, the
// wizard can have more spells of a different level than the bard.
// For example, a level 13 bard can have three spells of level 1 and one spell
// of level 5; a level 8 wizard can have FOUR spells of level 1 but NO spells
// of level 5.  So if the character is a level 13 bard and a level 8 wizard,
// he can have four spells of level 1 (the wizard's max) and one spell of
// level 5 (the bard's max).
//
// The table gcMaxSpells combines all the maximum levels for convenience
// when picking the spells.

	int							itemIndex;
	SPELL_INFO::TYPE			spellIndex;
	SPELL_INFO::SPELL_ITEM	spell;

	memset (&gcMaxSpells[1], 0, MAX_WIZARD_SPELL_LEVEL * sizeof (gcMaxSpells[0]));
	if (gAvaData.mfIsWizardOrBard ())
	{
		CalculateMaxSpellsSub (gAvaData.m_class1, gAvaData.m_level1);
		CalculateMaxSpellsSub (gAvaData.m_class2, gAvaData.m_level2);
	}

	// Now calculate gcSpellsInData from data in SPELL.CPP.
	memset (&gcSpellsInData[1], 0, MAX_WIZARD_SPELL_LEVEL * sizeof (gcSpellsInData[0]));
	for (itemIndex = ITEM_SPELL_DETECT_MAGIC; itemIndex <= ITEM_SPELL_TIME_STOP; itemIndex++)
	{
		spellIndex = (SPELL_INFO::TYPE) ItemTypes[itemIndex].mfGetExtendedData ();
		spell = SPELL_INFO::mfGetSpell (spellIndex);
		gcSpellsInData[spell.Level]++;
	}
}



// ---------------------------------------------------------------------------
static BOOL OnInitDialog (HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	POINT			pt;
	RECT			desktopRect;
	RECT			dialogRect;
	HWND			hwndRealm;
	HWND			hwndChar;
	LONG			levelW;
	LONG			levelB;
	BOOL			fEnable;

	// Expand dialog to fill client area of main window.
	GetClientRect (GetDesktopWindow (), &desktopRect);
	GetClientRect (hwnd, &dialogRect);

	// Put dialog in center of client area of desktop.
	// The app rect will normally be bigger than the dialog rect, but
	// check it to make sure.
	if (desktopRect.right > dialogRect.right)
		pt.x = (desktopRect.right - dialogRect.right) / 2;
	else
		pt.x = 0;

	if (desktopRect.bottom > dialogRect.bottom)
		pt.y = (desktopRect.bottom - dialogRect.bottom) / 2;
	else
		pt.y = 0;
	
	ClientToScreen (GetDesktopWindow (), &pt);
	SetWindowPos (hwnd, NULL, pt.x, pt.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	levelW = gAvaData.mfGetWizardLevel ();
	levelB = gAvaData.mfGetBardLevel ();
	gmaxSpellLevel = MAX (gWizardMaxSpellLevel[levelW], gBardMaxSpellLevel[levelB]);
	
	PutStringsInInventoryListBoxes (hwnd);

	hwndRealm = GetDlgItem (hwnd, IDS_INV_REALM);
	SetWindowText (hwndRealm, gAvaData.mfGetRealmName ());

	hwndChar = GetDlgItem (hwnd, IDS_INV_CHAR);
	SetWindowText (hwndChar, gAvaData.mfGetName ());
	
	gmaxMagicItems = 6;
	// switch (GetBloodlineGrade ())
	// {
	// 	case (BLOODLINE_STRENGTH_INFO::TYPE) TAINTED:
	// 		gmaxBloodAb = 0;
	// 		break;
	// 	case (BLOODLINE_STRENGTH_INFO::TYPE) MINOR:
	// 		gmaxBloodAb = 1;
	// 		break;
	// 	case (BLOODLINE_STRENGTH_INFO::TYPE) MAJOR:
	// 		gmaxBloodAb = 2;
	// 		break;
	// 	case (BLOODLINE_STRENGTH_INFO::TYPE) GREAT:
	// 		gmaxBloodAb = 3;
	// 		break;
	// }
	//GEH 9/97
	if (GetBloodlineGrade() == (BLOODLINE_STRENGTH_INFO::TYPE) TAINTED)
			gmaxBloodAb = 0;
	else
	// GEH limit to 6 to fit box and to limit power
	// if (gAvaData.mfGetBloodlineStrength() >= 80 )
	// 		gmaxBloodAb = 7;
	// else
	if (gAvaData.mfGetBloodlineStrength() >= 51 )
			gmaxBloodAb = 6;
	else
	if (gAvaData.mfGetBloodlineStrength() >= 36 )
			gmaxBloodAb = 5;
	else
	if (gAvaData.mfGetBloodlineStrength() >= 29 )
			gmaxBloodAb = 4;
	else
	if (gAvaData.mfGetBloodlineStrength() >= 20 )
			gmaxBloodAb = 3;
	else
	if (gAvaData.mfGetBloodlineStrength() >= 11 )
			gmaxBloodAb = 2;
	else
			gmaxBloodAb = 1;

	memset (&gcSpellsThisLevel[1], 0, MAX_WIZARD_SPELL_LEVEL * sizeof (gcSpellsThisLevel[0]));
	CalculateMaxSpells ();
	gmaxRealmSpells = 5;

	PostCurrentInventory (hwnd);

	fEnable = gAvaData.mfIsWizardOrBard ();
	EnableWindow (GetDlgItem (hwnd, IDS_SELECT_SPELLS_1      ), fEnable);
	EnableWindow (GetDlgItem (hwnd, IDS_SELECT_SPELLS_2      ), fEnable);
	EnableWindow (GetDlgItem (hwnd, IDS_SELECT_REALM_SPELLS_1), fEnable);
	EnableWindow (GetDlgItem (hwnd, IDS_SELECT_REALM_SPELLS_2), fEnable);

	return TRUE;
}


// ---------------------------------------------------------------------------
static void DoRandom (HWND hwndSelections, HWND hwndList, int maxItemsToSelect)
{
// Select cItems from hwndSelections, post to hwndList.
// Helper routine of SelectRandomItems.

	BOOL			fDuplicate;
	int			cItems;
	int			iSelected;
	int			iRandom;
	int			i;
	int			cSelected[_MAX_SELECTIONS];
	int			cItemsToSelect;
	char			szItem[_MAX_ITEM_LEN + 1];

	ListBox_ResetContent (hwndList);
	
	cItems = ListBox_GetCount (hwndSelections);

	cItemsToSelect = MIN (cItems, maxItemsToSelect);

	for (iSelected = 0; iSelected < cItemsToSelect; iSelected++)
	{
		do
		{
			fDuplicate = FALSE;
			iRandom = random (cItems);
			for (i = 0; i < iSelected; i++)
			{
				if (cSelected[i] == iRandom)
				{
					fDuplicate = TRUE;
					break;
				}
			}
		} while (fDuplicate);
		ListBox_GetText (hwndSelections, iRandom, szItem);
		ListBox_AddString (hwndList, szItem);
		cSelected[iSelected] = iRandom;
	}
}


// ---------------------------------------------------------------------------
static void DoRandomSpells (HWND hwndSelections, HWND hwndList)
{
// Select cItems from hwndSelections, post to hwndList.
// Helper routine of SelectRandomItems.

	BOOL			fDuplicate;
	BOOL			fFoundOne;
	int			cItems;
	int			iSelected;
	int			iRandom;
	int			i;
	int			iLevel;
	int			cSelected[_MAX_SELECTIONS];
	int			cItemsToSelect;
	int			maxItemsToSelect;
	char			szItem[_MAX_ITEM_LEN + 1];

	ListBox_ResetContent (hwndList);
	memset (&gcSpellsThisLevel[1], 0, MAX_WIZARD_SPELL_LEVEL * sizeof (gcSpellsThisLevel[0]));
	
	cItems = ListBox_GetCount (hwndSelections);

	maxItemsToSelect = 0;
	for (iLevel = 1; iLevel <= MAX_WIZARD_SPELL_LEVEL; iLevel++)
	{
		maxItemsToSelect += MIN (gcSpellsInData[iLevel], gcMaxSpells[iLevel]);
	}
	cItemsToSelect = MIN (cItems, maxItemsToSelect);

	for (iSelected = 0; iSelected < cItemsToSelect; iSelected++)
	{
		do
		{
			fFoundOne = FALSE;
			do
			{
				fDuplicate = FALSE;
				iRandom = random (cItems);
				for (i = 0; i < iSelected; i++)
				{
					if (cSelected[i] == iRandom)
					{
						fDuplicate = TRUE;
						break;
					}
				}
			} while (fDuplicate);
			ListBox_GetText (hwndSelections, iRandom, szItem);
			iLevel = atoi (&szItem[1]);
			if (gcSpellsThisLevel[iLevel] < gcMaxSpells[iLevel])
			{
				gcSpellsThisLevel[iLevel]++;
				ListBox_AddString (hwndList, szItem);
				cSelected[iSelected] = iRandom;
				fFoundOne = TRUE;
			}
		} while (!fFoundOne);
	}
}


// ---------------------------------------------------------------------------
static void SelectRandomItems (HWND hwnd)
{
// You get here by clicking on the ROLL button.
// Randomly select max items for each of the 4 categories.

	DoRandom (GetDlgItem (hwnd, IDL_SELECT_MAGIC_ITEMS),     GetDlgItem (hwnd, IDL_LIST_MAGIC_ITEMS),     gmaxMagicItems);
	DoRandom (GetDlgItem (hwnd, IDL_SELECT_BLOOD_ABILITIES), GetDlgItem (hwnd, IDL_LIST_BLOOD_ABILITIES), gmaxBloodAb);
	if (gAvaData.mfIsWizardOrBard ())
	{
		DoRandomSpells (GetDlgItem (hwnd, IDL_SELECT_SPELLS), GetDlgItem (hwnd, IDL_LIST_SPELLS));
		DoRandom (GetDlgItem (hwnd, IDL_SELECT_REALM_SPELLS), GetDlgItem (hwnd, IDL_LIST_REALM_SPELLS),    gmaxRealmSpells);
	}
}


// ---------------------------------------------------------------------------
// WM_COMMAND dispatcher and helpers.

static void SelectSpell (HWND hwnd, HWND hwndCtl)
{
	int			index;
	int			cItems;
	int			retval;
	HWND			hwndList;
	LONG 			itemIndex;
	LONG 			iLevel;
	SPELL_INFO::TYPE spellIndex;
	SPELL_INFO::SPELL_ITEM spell;
	char * 		pszItem;
	char			szItem[_MAX_ITEM_LEN + 1];

	// Post item from available list to selected list.

	index = ListBox_GetCurSel (hwndCtl);
	ListBox_GetText (hwndCtl, index, szItem);
	pszItem = strchr (szItem, ')');
	if (pszItem != NULL)
	{
		iLevel = atoi (&szItem[1]);
		if (gcSpellsThisLevel[iLevel] >= gcMaxSpells[iLevel])
		{
			Sorry ("You cannot select more than %d %s of level %d.\n"
					 "To deselect a spell, click on it in the SELECTED box.",
					 gcMaxSpells[iLevel],
					 (gcMaxSpells[iLevel] == 1)?"spell":"spells",
					 iLevel);
		}
		else
		{
			pszItem += 2;
			itemIndex = GetItemIndex (pszItem, ITEM_SPELL_DETECT_MAGIC);
			spellIndex = (SPELL_INFO::TYPE) ItemTypes[itemIndex].mfGetExtendedData ();
			spell = SPELL_INFO::mfGetSpell (spellIndex);
	
			// Check whether the string is already there.
			hwndList = GetDlgItem (hwnd, IDL_LIST_SPELLS);
			retval = ListBox_SelectString (hwndList, -1, szItem);
			if (retval == LB_ERR)
			{
				ListBox_AddString (hwndList, szItem);
				gcSpellsThisLevel[iLevel]++;
			}
			else
			{
				Sorry ("You cannot select the same spell twice.");
			}
		}
	}
}


static void OnCommand (HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
// Process commands sent from the controls in the dialog.
// hwnd       = HWND of parent
// id         = child window control ID
// hwndCtl    = HWND for child window
// codeNotify = notification code

	int			index;
	int			cItems;
	int			retval;
	HWND			hwndList;
	char			szItem[_MAX_ITEM_LEN + 1];

	switch (id)
	{
		case IDOK:
			PostSelectedInventory (hwnd);
			EndDialog (hwnd, IDOK);
			break;
	
		case IDCANCEL:
			EndDialog (hwnd, IDCANCEL);
			break;
	
		case IDB_INV_ROLL:
			SelectRandomItems (hwnd);
			break;

		case IDL_SELECT_MAGIC_ITEMS:
			if (codeNotify == LBN_SELCHANGE)
			{
				// Post item from available list to selected list.

				hwndList = GetDlgItem (hwnd, IDL_LIST_MAGIC_ITEMS);
				cItems = ListBox_GetCount (hwndList);
				if (cItems >= gmaxMagicItems)
				{
					Sorry ("You cannot select more than %d magical items.\n"
							 "To deselect an item, click on it in the SELECTED box.",
							 gmaxMagicItems);
				}
				else
				{
					index = ListBox_GetCurSel (hwndCtl);
					ListBox_GetText (hwndCtl, index, szItem);
					ListBox_AddString (hwndList, szItem);
				}
			}
			break;

		case IDL_SELECT_BLOOD_ABILITIES:
			if (codeNotify == LBN_SELCHANGE)
			{
				// Post item from available list to selected list.

				hwndList = GetDlgItem (hwnd, IDL_LIST_BLOOD_ABILITIES);
				cItems = ListBox_GetCount (hwndList);
				if (cItems >= gmaxBloodAb)
				{
					if (gmaxBloodAb == 0)
						Sorry ("Tainted bloodlines cannot have any blood abilities");
					else
						Sorry ("You cannot select more than %d blood abilities.\n"
							 "To deselect an ability, click on it in the SELECTED box.",
							 gmaxBloodAb);
				}
				else
				{
					index = ListBox_GetCurSel (hwndCtl);
					ListBox_GetText (hwndCtl, index, szItem);
					// Check whether the string is already there.
					retval = ListBox_SelectString (hwndList, -1, szItem);
					if (retval == LB_ERR)
					{
						ListBox_AddString (hwndList, szItem);
					}
					else
					{
						Sorry ("You cannot select the same blood ability twice.");
					}
				}
			}
			break;

		case IDL_SELECT_SPELLS:
			if (codeNotify == LBN_SELCHANGE)
			{
				SelectSpell (hwnd, hwndCtl);
			}
			break;

		case IDL_SELECT_REALM_SPELLS:
			if (codeNotify == LBN_SELCHANGE)
			{
				// Post item from available list to selected list.

				hwndList = GetDlgItem (hwnd, IDL_LIST_REALM_SPELLS);
				cItems = ListBox_GetCount (hwndList);
				if (cItems >= gmaxRealmSpells)
				{
					Sorry ("You cannot select more than %d realm spells.\n"
							 "To deselect a realm spell, click on it in the SELECTED box.",
							 gmaxRealmSpells);
				}
				else
				{
					index = ListBox_GetCurSel (hwndCtl);
					ListBox_GetText (hwndCtl, index, szItem);
					// Check whether the string is already there.
					retval = ListBox_SelectString (hwndList, -1, szItem);
					if (retval == LB_ERR)
					{
						ListBox_AddString (hwndList, szItem);
					}
					else
					{
						Sorry ("You cannot select the same realm spell twice.");
					}
				}
			}
			break;

		case IDL_LIST_MAGIC_ITEMS:
		case IDL_LIST_BLOOD_ABILITIES:
		case IDL_LIST_REALM_SPELLS:
			if (codeNotify == LBN_SELCHANGE)
			{
				// Drop item from selected list.
				index = ListBox_GetCurSel (hwndCtl);
				ListBox_DeleteString (hwndCtl, index);
			}
			break;

		case IDL_LIST_SPELLS:
			if (codeNotify == LBN_SELCHANGE)
			{
				// Drop item from selected list.
				int iLevel;

				index = ListBox_GetCurSel (hwndCtl);
				ListBox_GetText (hwndCtl, index, szItem);
				iLevel = atoi (&szItem[1]);
				gcSpellsThisLevel[iLevel]--;
				ListBox_DeleteString (hwndCtl, index);
			}
			break;

		default:
			break;
	}
}


// ---------------------------------------------------------------------------
BOOL CALLBACK SelectInventoryWndProc (
   HWND     hwnd,                      // handle to dialog box
   UINT     uMsg,                      // message
   WPARAM   wParam,                    // first message parameter
   LPARAM   lParam)                    // second message parameter
{
	switch (uMsg)
	{
		HANDLE_MSG (hwnd, WM_INITDIALOG, OnInitDialog);
		HANDLE_MSG (hwnd, WM_COMMAND,    OnCommand);
		default:
			return FALSE;
	}
}


// ---------------------------------------------------------------------------
int SelectInventory (HWND hwnd)
{
	int			result;

	result = DialogBox ( hInstApp, "DIALOG_CHARINV", hwnd,
		(DLGPROC) SelectInventoryWndProc);

	if (result == -1)
	{
		MessageBox (NULL, "Unable to create DIALOG_CHARINV", "Error", MB_OK);
	}
	return result;
}



// ---------------------------------------------------------------------------
void ReadINVTextData (FILE * fp)
{
// GetNextLine doesn't work in this case--the format of the .INV file
// allows C++-style comments, including end-of-line comments.
// Code boldly copied and modified from INVNTORY.CPP: ObjectList::ReadINV.

	char 			buffer[BUFLEN];
	LONG 			i, count;
	SHORT			entry;
	BOOL			fReadingRelics = FALSE;

	giItemIndex = 0;
	giRelicIndex = 0;

	while (fgets (buffer, BUFLEN, fp) != NULL)
	{
		count = strlen (buffer);
	
		for (i = 0; i < count; i++)
		{
			if (buffer[i] == '/' && buffer[i+1] == '/')  /* comment */
				i = count;   /* skip to the end */
			else if (buffer[i] >= '0' && buffer[i] <= '9')
				break;
		}
		
		if (i < count)
		{
			sscanf(buffer+i, "%d", &entry);
			if (entry > 0  && entry < MAX_ITEM_TYPES)
			{
				if (!fReadingRelics)
				{
					// Standard items.
					giItems[giItemIndex] = entry;
					giItemIndex++;
				}
				else
				{
					// Relics of power.
					giRelics[giRelicIndex] = entry;
					giRelicIndex++;
				}
			}
			else if (entry == RELIC_CODE)
			{
				// Relics of power follow this entry.
				// Remember them but do not let the user change them.
				fReadingRelics = TRUE;
			}
		}
	}
}


// ---------------------------------------------------------------------------
BOOL WriteINVFile (char * szInvFileName)
{
// Write SCENES\IDxxxx.INV.
// Return TRUE if it worked, FALSE if not.

	FILE *		fp;
	int			i;

	// Use fopen rather than FileOpen when we want to write a new file,
	// which we're probably doing here.  (We read from CD but want to
	// write to the local hard disk.)
	// fp = FileOpen (szInvFileName, "wt");
	fp = fopen (szInvFileName, "wt");
	if (fp != NULL)
	{
		for (i = 0; i < giItemIndex; i++)
			fprintf (fp, "%d\n", giItems[i]);
		if (giRelicIndex > 0)
		{
			fprintf (fp, "%d", RELIC_CODE);
			for (i = 0; i < giRelicIndex; i++)
				fprintf (fp, "%d\n", giRelics[i]);
		}
		// FileClose (fp);
		fclose (fp);
		return TRUE;
	}
	return FALSE;
}


#endif

