/* ========================================================================
   Copyright (c) 1997	Synergistic Software
   All Rights Reserved
   ========================================================================
	Character editor for Birthright
   Filename: CHARED.CPP
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
#include <direct.h>							// for mkdir
#include <errno.h>

#include <Windows.h>
#include <windowsx.h>

#include "SYSTEM.H"
#include "ENGINE.H"
#include "FOPEN.H"

#include "AVATAR.HXX"
#include "ALIGNMNT.HXX"
#include "GAMEMAP.HXX"
#include "MAIN.HXX"
#include "SCENE.HXX"
#include "PLAYCLAS.HXX"
#include "APPEAR.HXX"
#include "BLOODREQ.HXX"
#include "CONSTITN.HXX"

#include "RESOURCE.H"						// symbols for CHARED dialogs
#include "AVA.HXX"							// load/save functions
#include "GETFILE.HXX"
#include "CHARED.HXX"
#include "CHAREDIN.HXX"						// inventory selection functions

#define W_OK 2
#define F_OK 0
#define S_IRUSR 1024
#define S_IWUSR 512

/* ------------------------------------------------------------------------
	External data
   ------------------------------------------------------------------------ */
extern HINSTANCE hInstApp;
extern HPALETTE hpalApp;				// in WINSYS\SYSGRAPH.C
extern HWND hwndApp;


/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define SIGNED             TRUE        // for SetDlgItemInt
#define UNSIGNED           FALSE       // for SetDlgItemInt
#define MAX_CLASS_TYPES		7
#define cBLOOD_ABILITIES	17
#define CHAR_MAX_ABILITIES 6
#define CA_NOTWRITABLE		0				// return value from CheckAccess
#define CA_WRITABLE			1           // return value from CheckAccess
/* ------------------------------------------------------------------------
	Macros
   ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------
	Structures
   ------------------------------------------------------------------------ */

struct BITMAPHEADER
{
  BITMAPINFOHEADER  Header;
  RGBQUAD           aColors[256];
};                                     // this is from WINSYS\SYSGRAPH.C


/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

#ifdef _CHARED
void CharedDoEditorDialog ();
#endif

static void ShowNewAppearance (HWND hwnd, int index);
static void ChangeHitPoints (HWND hwnd);
static BOOL IsWarrior (int iClass);
static int CheckAccess (char * szPathname);
static BOOL UpdateAvaData(HWND hwnd);

BOOL CALLBACK CharedWndProc (
   HWND     hwnd,                      // handle to dialog box
   UINT     uMsg,                      // message
   WPARAM   wParam,                    // first message parameter
   LPARAM   lParam);                   // second message parameter

BOOL CALLBACK SelectRegentWndProc (
   HWND     hwnd,                      // handle to dialog box
   UINT     uMsg,                      // message
   WPARAM   wParam,                    // first message parameter
   LPARAM   lParam);                   // second message parameter

BOOL DisplayRegentsOfRealm (HWND hwnd, char * szRealm);
/* ------------------------------------------------------------------------
   Global and Module-Level Variables
   ------------------------------------------------------------------------ */
AVA_FILE				gAvaData;				// main character data
static char			gszAvaFile[256];		// .AVA file name
static char			gszInvFile[256];		// .INV file name
static BOOL 		gfSetInventory;		// if TRUE, user has done the inventory dialog
static BOOL			gfSaveOK;				// if not TRUE, disable SAVE button

static PSTR 		gszNoTitle = "No title";

static int			giPortrait;
static HWND			ghwndChar;

static LONG 		giRegent;

// The following is based on information in PLAYCLAS.CPP and .HXX, but
// excludes the magician class.
static struct {
	char * szName;
	LONG	 index;
} gClassInfo [MAX_CLASS_TYPES] = {
		"Fighter", 	0,
		"Paladin", 	1,
		"Ranger",	2,
		"Priest",	3,
		"Wizard",	4,
		"Thief",		6,
		"Bard",		7
};


// gRaceGameToEditor converts from game enumeration to combo box index.
// Purpose: don't allow users to choose halfling race.
// Later: dropped goblin, gnoll, and awnshegh; these are easier because
// they are at the end of the list.  All we have to do is not add them
// to the combo box.
// index   in game		in combo box
// 	 0	  dwarf        dwarf
// 	 1	  elf          elf
// 	 2	  half-elf     half-elf
// 	 3	  halfling     anuirean
// 	 4	  anuirean     brecht
// 	 5	  brecht       khinasi
// 	 6	  khinasi      rjurik
// 	 7	  rjurik       vos
// 	 8	  vos          goblin
// 	 9	  goblin       gnoll
// 	10	  gnoll        ---
// 	11   awnshegh     ---

static int gRaceGameToEditor[] = {
	0,	
	1,	
	2,	
	3,	
	3,	
	4,	
	5,	
	6,	
	7,	
	8,	
	9,	
	10};
	
// gRaceEditorToGame converts from combo box index to game enumeration.
// Purpose: don't allow users to choose halfling race.
static int gRaceEditorToGame[] = {
	0,
	1,
	2,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11};
	
// ---------------------------------------------------------------------------
// Table of blood abilities.
//
// itemType is blood ability
// iAvail is what blood strength you need to have this ability:
// 0 = not available
// 1 = tainted
// 2 = minor
// 3 = major
// 4 = great

static struct tagBATable {
	ITEMTYPE		itemType;
	int			iAvail[BLOODLINE_DERIVATION_INFO::MAX_DERIVATION];
} gBATable[cBLOOD_ABILITIES] = {
//										A	R  B  B  M  V  A
//										n	e	r	a	a	o	z
//										d  y	e	s	s	r	r

	ITEM_BATTLEWISE,           3, 0, 0, 0, 0, 0, 3,
	ITEM_COURAGE,              2, 0, 0, 0, 0, 0, 0,
	ITEM_DEVINE_WRATH,         3, 0, 0, 0, 0, 0, 0,
	ITEM_HEIGHTENED_ABILITY,  	2, 2, 2, 2, 0, 2, 2,
	ITEM_BLOODMARK,            2, 2, 2, 2, 2, 2, 2,
	ITEM_DETECT_ILLUSION,      0, 2, 2, 2, 2, 2, 2,
	ITEM_DIVINE_AURA,          3, 3, 0, 3, 3, 3, 3,
	ITEM_ENHANCED_SENSE,      	2, 0, 2, 0, 0, 2, 2,
	ITEM_FEAR,                 0, 0, 0, 0, 0, 0, 3,
	ITEM_HEALING_MINOR,        2, 2, 0, 2, 0, 0, 0,
	ITEM_HEALING_MAJOR,        3, 3, 0, 3, 0, 0, 0,
	ITEM_HEALING_GREAT,        4, 4, 0, 4, 0, 0, 0,
	ITEM_IRON_WILL,           	2, 2, 0, 0, 0, 0, 2,
	ITEM_PROTECTION_FROM_EVIL, 3, 3, 3, 3, 3, 3, 0,
	ITEM_REGENERATION,         4, 4, 0, 0, 0, 0, 4,
	ITEM_RESISTANCE,           3, 3, 3, 3, 3, 3, 3,
	ITEM_TRAVEL,               0,	0, 4, 4, 4, 4, 4
};


int gaAbilityIDs[CHAR_MAX_ABILITIES] = {
	IDE_STRENGTH,
	IDE_INTELLIGENCE,
	IDE_DEXTERITY,
	IDE_WISDOM,
	IDE_CONSTITUTION,
	IDE_CHARISMA
};


int gaMinAbility[MAX_CLASS_TYPES][CHAR_MAX_ABILITIES] = {
// Note that we omit magicians.
// Str Int Dex Wis Con Cha
	 9,  1,  1,  1,  1,  1,		// CLASS_FIGHTER
	12,  1,  1, 13,  9, 17,    // CLASS_PALADIN
	13,  1, 13, 14, 14,  1,    // CLASS_RANGER
	 1,  1,  1,  9,  1,  1,    // CLASS_PRIEST
    1,  9,  1,  1,  1,  1,    // CLASS_WIZARD
	 1,  1,  9,  1,  1,  1,    // CLASS_THIEF
	 1, 13, 12,  1,  1, 15     // CLASS_BARD
};


// ---------------------------------------------------------------------------
HBITMAP PCXtoDIBSection (HDC hdc, PSTR szFilename, void * pBuffer)
{
// [d2-10-97 JPC] Load a PCX file and use it to create a DIBSection.
// Return handle to bitmap; return address of bitmap data in pBuffer.
// The advantage of a DIBSection-type DIB is that we can access its
// data bits directly.
// ---------------------------------------------------------------------------

	BITMPTR			pBitm;
	PTR				pBitmPixelData;
	SHORT				iBitm;
	HBITMAP			hbitmap;
	int				size;
	BITMAPHEADER	bitmapHeader;

	iBitm = GetResourceStd (szFilename, FALSE);
	if (iBitm != fERROR)
	{
		pBitm = (BITMPTR) BLKPTR (iBitm);
		if (IsPointerGood (pBitm))
		{
			pBitmPixelData = (PTR) pBitm + sizeof (BITMHDR);
			bitmapHeader.Header.biSize = sizeof(BITMAPINFOHEADER);
			bitmapHeader.Header.biPlanes = 1;
			bitmapHeader.Header.biBitCount = 8;
			bitmapHeader.Header.biCompression = BI_RGB;
			bitmapHeader.Header.biSizeImage = 0;
			bitmapHeader.Header.biClrUsed = 0;
			bitmapHeader.Header.biClrImportant = 0;

			bitmapHeader.Header.biWidth = pBitm->w;
			bitmapHeader.Header.biHeight = pBitm->h;
			
			// Force bitmap to top-down.
			bitmapHeader.Header.biHeight = -(abs(bitmapHeader.Header.biHeight));

			// Set up the palette.
			for (int Counter = 0; Counter < 256; Counter++)
			{
				bitmapHeader.aColors[Counter].rgbRed   = CurPal[Counter].bRed;
				bitmapHeader.aColors[Counter].rgbGreen = CurPal[Counter].bGreen;
				bitmapHeader.aColors[Counter].rgbBlue  = CurPal[Counter].bBlue;
				bitmapHeader.aColors[Counter].rgbReserved = 0;
			}

			hbitmap = CreateDIBSection (hdc, (BITMAPINFO far *)&bitmapHeader,
				DIB_RGB_COLORS, &pBuffer, NULL, 0);
			
			if (hbitmap != NULL)
			{
				size = pBitm->w * pBitm->h;
				// The following memcpy is all we need to do
				// to get the PCX data into the DIBSection.
				memcpy ((PTR) pBuffer, pBitmPixelData, size);
			}
		}
	}
	else
	{
		hbitmap = NULL;
	}
	
	// We can now dispose of the PCX data.
	if (iBitm != fERROR)
		SetPurge (iBitm);

	return hbitmap;
}




// ---------------------------------------------------------------------------
// Miscellaneous functions.
static void EnableClass2Windows (HWND hwnd)
{
// Enable or disable the level 2 and experience 2 windows, depending on
// whether the Class 2 description is "(Not used)."

	BOOL			fEnable;

	if (ComboBox_GetCurSel (GetDlgItem (hwnd, IDC_CLASS2)) == 0)
		fEnable = FALSE;
	else
		fEnable = TRUE;

	EnableWindow (GetDlgItem (hwnd, IDS_LEVEL2), fEnable);
	EnableWindow (GetDlgItem (hwnd, IDC_LEVEL2), fEnable);
	EnableWindow (GetDlgItem (hwnd, IDB_ROLL_LEVEL2), fEnable);
}



static int GetAVAFileName ()
{
// Bring up a dialog that lets the user select realm and regent.
// We use the regent's ID to set module-level variables, gszAvaFile
// and gszInvFile.

	int			result;

	result = DialogBox ( hInstApp, "DIALOG_SELCHAR", (HWND) NULL,
		(DLGPROC) SelectRegentWndProc);
	if (result == -1)
	{
		MessageBox (NULL, "Unable to create DIALOG_SELCHAR", "Error", MB_OK);
	}
	else if (result != IDCANCEL)
	{
		gfSaveOK = TRUE;
		if (CheckAccess (gszAvaFile) == CA_NOTWRITABLE)
			gfSaveOK = FALSE;
		else if (CheckAccess (gszInvFile) == CA_NOTWRITABLE)
			gfSaveOK = FALSE;
	}
	return result;
}


// ---------------------------------------------------------------------------
int GetClassIndex (int iClass)
{
// Given a class number from an AVA file, return index to our class structure.
// This is required so we can exclude the magician class.

	int			i;

	for (i = 0; i < MAX_CLASS_TYPES; i++)
	{
		if (gClassInfo[i].index == iClass)
		{
			return i;
		}
	}
	return 0;									// error--return something!
}


// ---------------------------------------------------------------------------
// Initialization functions

static void InitFieldsFromAVAFile (HWND hwnd)
{
	LONG			iClass;
	LONG			iLevel;
	LONG			iRace;
	PCSTR			szName;
	HWND			hwndCtl;
	HWND			hwndRealm;
	THINGTYPE	iType;

	hwndRealm = GetDlgItem (hwnd, IDS_REALM);
	SetWindowText (hwndRealm, 
		STRMGR_GetStr(STR_RLM_NAME_CHAOS + regents[giRegent].mfGetRealm()));

	szName = gAvaData.mfGetName ();
	Edit_SetText (GetDlgItem (hwnd, IDE_CHARNAME), szName);

	hwndCtl = GetDlgItem (hwnd, IDC_TITLE);
	ComboBox_SetCurSel (hwndCtl, gAvaData.mfGetTitle ());

	hwndCtl = GetDlgItem (hwnd, IDC_RACE);
	iRace = gAvaData.mfGetRace ();
	if (iRace > RACE_INFO::GNOLLS)
		iRace = RACE_INFO::GNOLLS;
	iRace = gRaceGameToEditor[iRace];
	ComboBox_SetCurSel (hwndCtl, iRace);

	iType = gAvaData.mfGetThingType ();
	giPortrait = APPEARANCE_INFO::IndexFromType (iType);

	iClass = GetClassIndex (gAvaData.mfGetClass1 ());
	hwndCtl = GetDlgItem (hwnd, IDC_CLASS1);
	ComboBox_SetCurSel (hwndCtl, iClass);

	iLevel = gAvaData.mfGetClass1Level ();
	if (iLevel < 1)
		iLevel = 1;
	if (iLevel > 20)
		iLevel = 20;
	iLevel--;									// put in combo box range
	hwndCtl = GetDlgItem (hwnd, IDC_LEVEL1);
	ComboBox_SetCurSel (hwndCtl, iLevel);

	// Class 2 values range from -1 to max instead of 0 to max.
	// -1 means there is no class 2, which is the usual case.
	// We add 1 here to translate to the combo box index range, which
	// starts at 0.
	iClass = gAvaData.mfGetClass2 ();
	if (iClass >= 0)
		iClass = GetClassIndex (iClass);
	hwndCtl = GetDlgItem (hwnd, IDC_CLASS2);
	ComboBox_SetCurSel (hwndCtl, iClass + 1);

	EnableClass2Windows (hwnd);
	iLevel = gAvaData.mfGetClass2Level ();
	if (iLevel < 1)
		iLevel = 1;
	if (iLevel > 20)
		iLevel = 20;
	iLevel--;									// put in combo box range
	hwndCtl = GetDlgItem (hwnd, IDC_LEVEL2);
	ComboBox_SetCurSel (hwndCtl, iLevel);

	SetDlgItemInt (hwnd, IDE_STRENGTH,     gAvaData.mfGetStrength (), SIGNED);
	SetDlgItemInt (hwnd, IDE_INTELLIGENCE, gAvaData.mfGetIntelligence (), SIGNED);
	SetDlgItemInt (hwnd, IDE_DEXTERITY,    gAvaData.mfGetDexterity (), SIGNED);
	SetDlgItemInt (hwnd, IDE_WISDOM,       gAvaData.mfGetWisdom (), SIGNED);
	SetDlgItemInt (hwnd, IDE_CONSTITUTION, gAvaData.mfGetConstitution (), SIGNED);
	SetDlgItemInt (hwnd, IDE_CHARISMA,     gAvaData.mfGetCharisma (), SIGNED);
	SetDlgItemInt (hwnd, IDE_HIT_POINTS,   gAvaData.mfGetHitPoints (), SIGNED);

	hwndCtl = GetDlgItem (hwnd, IDC_ALIGNMENT);
	ComboBox_SetCurSel (hwndCtl, gAvaData.mfGetAlignment ());


	hwndCtl = GetDlgItem (hwnd, IDC_ARMOR);
	ComboBox_SetCurSel (hwndCtl, gAvaData.mfGetArmorType ());

	hwndCtl = GetDlgItem (hwnd, IDC_WEAPON);
	ComboBox_SetCurSel (hwndCtl, gAvaData.mfGetWeapon ());

	hwndCtl = GetDlgItem (hwnd, IDC_ATTACKS_PER_ROUND);
	ComboBox_SetCurSel (hwndCtl, gAvaData.mfGetAttacksPerRound () - 1);

	if (gAvaData.mfGetBloodlineGrade () == -1)
	{
		//GEH gAvaData.mfSetBloodlineGrade (regents[giRegent].BL_grade);
		gAvaData.mfSetBloodlineGrade (regents[giRegent].mfGetBL_grade());
	}

	if (gAvaData.mfGetBloodlineDerivation () == -1)
	{
		//GEH gAvaData.mfSetBloodlineDerivation (regents[giRegent].BL_deriv);
		gAvaData.mfSetBloodlineDerivation (regents[giRegent].mfGetBL_deriv());
	}

	if (gAvaData.mfGetBloodlineStrength () == -1)
	{
		//GEH gAvaData.mfSetBloodlineStrength (regents[giRegent].BL_strength);
		gAvaData.mfSetBloodlineStrength (regents[giRegent].mfGetBL_strength());
	}

	hwndCtl = GetDlgItem (hwnd, IDC_BLOODLINE);
	ComboBox_SetCurSel (hwndCtl, gAvaData.mfGetBloodlineDerivation ());
	hwndCtl = GetDlgItem (hwnd, IDC_BLOOD_GRADE);
	ComboBox_SetCurSel (hwndCtl, gAvaData.mfGetBloodlineGrade ());
	SetDlgItemInt (hwnd, IDE_BLOOD_STRENGTH, gAvaData.mfGetBloodlineStrength (), SIGNED);
}




static void PutStringsInTitleComboBox (HWND hwnd)
{
	LONG			iType;
	HWND			hwndCtl;
	PCSTR			szTitle;
	char			szTypeName[128];

	hwndCtl = GetDlgItem (hwnd, IDC_TITLE);
	for (iType = 0; iType <= PLAYER_TITLE::THANE; iType++)
	{
		szTitle = PLAYER_TITLE::mfGetTitle (PLAYER_TITLE::TITLE (iType));
		if (szTitle == NULL)
			szTitle = gszNoTitle;
		lstrcpy (szTypeName, szTitle);
		ComboBox_InsertString (hwndCtl, -1, szTypeName);
	}
}



static void PutStringsInRaceComboBox (HWND hwnd)
{
// [d5-16-97 JPC] Don't allow users to choose HALFLING, GOBLIN, or GNOLL.
// Do this by excluding these races from the combo box.

	// RACE_INFO::TYPE	iType;
	int			iType;
	HWND			hwndCtl;
	char			szTypeName[128];

	hwndCtl = GetDlgItem (hwnd, IDC_RACE);
	for (iType = 0; iType <= RACE_INFO::GNOLLS; iType++)
	{
		if (iType == RACE_INFO::HALFLING)
			continue;
		lstrcpy (szTypeName, RACE_INFO::mfGetRaceName ((RACE_INFO::TYPE) iType));
		ComboBox_InsertString (hwndCtl, -1, szTypeName);
	}
}



static void PutStringsInAnyClassComboBox (HWND hwndCtl)
{
// Helper routine for both class combo boxes.  Note special handling of
// magician: you can't select this class.
// LATER: restore magician to simplify indexing.  Why can't you select
// this class if you want to?

	LONG			iType;
	for (iType = 0; iType < MAX_CLASS_TYPES; iType++)
	{
		ComboBox_InsertString (hwndCtl, -1, gClassInfo[iType].szName);
	}
}



static void PutStringsInClassComboBox (HWND hwnd)
{
	HWND			hwndCtl;

	hwndCtl = GetDlgItem (hwnd, IDC_CLASS1);
	PutStringsInAnyClassComboBox (hwndCtl);
	ComboBox_SetCurSel (hwndCtl, 0);
}



static void PutStringsInClass2ComboBox (HWND hwnd)
{
	HWND			hwndCtl;

	hwndCtl = GetDlgItem (hwnd, IDC_CLASS2);
	ComboBox_InsertString (hwndCtl, -1, "(Not used)");
	PutStringsInAnyClassComboBox (hwndCtl);
	ComboBox_SetCurSel (hwndCtl, 0);

	// Once we read in data from AVA files, the current selection may not
	// be zero.  If it is zero, disable the "Level 2" and "Experience 2"
	// controls.
	EnableClass2Windows (hwnd);
}



static void PutStringsInBloodlineComboBox (HWND hwnd)
{
	LONG			iType;
	HWND			hwndCtl;
	char			szTypeName[128];

	hwndCtl = GetDlgItem (hwnd, IDC_BLOODLINE);
	for (iType = 0; iType < BLOODLINE_DERIVATION_INFO::MAX_DERIVATION; iType++)
	{
		lstrcpy (szTypeName, BLOODLINE_DERIVATION_INFO::mfGetName (BLOODLINE_DERIVATION_INFO::TYPE (iType)));
		ComboBox_InsertString (hwndCtl, -1, szTypeName);
	}
	ComboBox_SetCurSel (hwndCtl, 0);
}



static void PutStringsInBloodStrengthComboBox (HWND hwnd)
{
	LONG			iType;
	HWND			hwndCtl;
	char			szTypeName[128];

	hwndCtl = GetDlgItem (hwnd, IDC_BLOOD_GRADE);
	for (iType = 0; iType <= (BLOODLINE_STRENGTH_INFO::GREAT); iType++)
	{
		lstrcpy (szTypeName, BLOODLINE_STRENGTH_INFO::mfGetName (BLOODLINE_STRENGTH_INFO::TYPE (iType)));
		ComboBox_InsertString (hwndCtl, -1, szTypeName);
	}
	ComboBox_SetCurSel (hwndCtl, 0);
}



static void PutStringsInAlignmentComboBox (HWND hwnd)
{
	LONG			iType;
	HWND			hwndCtl;
	char			szTypeName[128];

	hwndCtl = GetDlgItem (hwnd, IDC_ALIGNMENT);
	for (iType = 0; iType < ALIGNMENT_INFO::MAX_ALIGNMENTS; iType++)
	{
		lstrcpy (szTypeName, ALIGNMENT_INFO::mfGetName (ALIGNMENT_INFO::TYPE (iType)));
		ComboBox_InsertString (hwndCtl, -1, szTypeName);
	}
	ComboBox_SetCurSel (hwndCtl, 0);
}



static void PutStringsInArmorComboBox (HWND hwnd)
{
	LONG			iType;
	HWND			hwndCtl;
	char			szTypeName[128];

	hwndCtl = GetDlgItem (hwnd, IDC_ARMOR);
	for (iType = ARMOR_INFO::FIRST_ARMOR; iType < ARMOR_INFO::ARMOR_MAX_INDEX; iType++)
	{
		lstrcpy (szTypeName, ARMOR_INFO::mfGetName (ARMOR_INFO::TYPE (iType)));
		ComboBox_InsertString (hwndCtl, -1, szTypeName);
	}
	ComboBox_SetCurSel (hwndCtl, 0);
}



static void PutStringsInWeaponComboBox (HWND hwnd)
{
	LONG			iType;
	HWND			hwndCtl;
	char			szTypeName[128];

	hwndCtl = GetDlgItem (hwnd, IDC_WEAPON);
	ComboBox_InsertString (hwndCtl, -1, "No weapon");
	for (iType = WEAPON_INFO::FIRST_WEAPON; iType < WEAPON_INFO::WEAPON_MAX_INDEX; iType++)
	{
		lstrcpy (szTypeName, WEAPON_INFO::mfGetName (WEAPON_INFO::TYPE (iType)));
		ComboBox_InsertString (hwndCtl, -1, szTypeName);
	}
	ComboBox_SetCurSel (hwndCtl, 0);
}



static void PutStringsInLevelComboBox (HWND hwnd)
{
	LONG			i;
	HWND			hwndCtl1;
	HWND			hwndCtl2;
	char			szNum[8];

	hwndCtl1 = GetDlgItem (hwnd, IDC_LEVEL1);
	hwndCtl2 = GetDlgItem (hwnd, IDC_LEVEL2);
	for (i = 1; i <= 20; i++)
	{
		sprintf (szNum, "%d", i);
		ComboBox_InsertString (hwndCtl1, -1, szNum);
		ComboBox_InsertString (hwndCtl2, -1, szNum);
	}
}



static void PutStringsInAttacksPerRoundComboBox (HWND hwnd)
{
	LONG			iType;
	HWND			hwndCtl;
	char			szTypeName[128];

	hwndCtl = GetDlgItem (hwnd, IDC_ATTACKS_PER_ROUND);
	for (iType = 1; iType < 3; iType++)
	{
		sprintf (szTypeName, "%d", iType);
		ComboBox_InsertString (hwndCtl, -1, szTypeName);
	}
	ComboBox_SetCurSel (hwndCtl, 0);
}



static BOOL ReadAVAFile ()
{
	FILE *		fp;

	fp = FileOpen (gszAvaFile, "r");
	if (fp != NULL)
	{
		gAvaData.mfReadTextData (fp);
		FileClose (fp);
		return TRUE;
	}
	return FALSE;
}



static BOOL ReadINVFile ()
{
	FILE *		fp;

	fp = FileOpen (gszInvFile, "r");
	if (fp != NULL)
	{
		ReadINVTextData (fp);
		FileClose (fp);
		return TRUE;
	}
	return FALSE;
}



static BOOL OnInitDialog (HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	POINT			pt;
	RECT			desktopRect;
	RECT			dialogRect;

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

	PutStringsInTitleComboBox (hwnd);
	PutStringsInRaceComboBox (hwnd);
	PutStringsInClassComboBox (hwnd);
	PutStringsInClass2ComboBox (hwnd);
	PutStringsInBloodlineComboBox (hwnd);
	PutStringsInBloodStrengthComboBox (hwnd);
	PutStringsInAlignmentComboBox (hwnd);
	PutStringsInArmorComboBox (hwnd);
	PutStringsInWeaponComboBox (hwnd);
	PutStringsInLevelComboBox (hwnd);
	PutStringsInAttacksPerRoundComboBox (hwnd);

	ReadAVAFile ();
	ReadINVFile ();
	gfSetInventory = FALSE;
	InitFieldsFromAVAFile (hwnd);

	ghwndChar = hwnd;

	EnableWindow (GetDlgItem (hwnd, IDB_SAVE), gfSaveOK);

	return TRUE;
}



// ---------------------------------------------------------------------------
// Informational functions

static RACE_INFO::TYPE CharedGetRace (HWND hwnd)
{
	int			race;

	race = ComboBox_GetCurSel (GetDlgItem (hwnd, IDC_RACE));
	if (race < 0 || race >= RACE_INFO::RACE_MAX_NUMBER)
		race = 0;
	race = gRaceEditorToGame[race];
	return RACE_INFO::TYPE (race);
}



// ---------------------------------------------------------------------------
// WM_COMMAND handling Functions

static void CharedRollLevel (HWND hwnd, int iClass)
{
	int			iLevel;
	HWND			hwndCtl;

   iLevel = random (6) + 4;				// TEMPORARY!  We haven't decided
													// how to init the levels.
	if (iLevel < 1)
		iLevel = 1;
	if (iLevel > 20)
		iLevel = 20;
	iLevel--;									// put in combo box range

	if (iClass == 0)
		hwndCtl = GetDlgItem (hwnd, IDC_LEVEL1);
	else
		hwndCtl = GetDlgItem (hwnd, IDC_LEVEL2);

	ComboBox_SetCurSel (hwndCtl, iLevel);

	ChangeHitPoints (hwnd);
}



static void CharedRollStrength (HWND hwnd)
{
	SBYTE			sValue;

	sValue = RollStrength (CharedGetRace (hwnd));
	SetDlgItemInt (hwnd, IDE_STRENGTH, sValue, SIGNED);
}



static void CharedRollIntelligence (HWND hwnd)
{
	SBYTE			sValue;

	sValue = RollIntelligence (CharedGetRace (hwnd));
	SetDlgItemInt (hwnd, IDE_INTELLIGENCE, sValue, SIGNED);
}



static void CharedRollDexterity (HWND hwnd)
{
	SBYTE			sValue;

	sValue = RollDexterity (CharedGetRace (hwnd));
	SetDlgItemInt (hwnd, IDE_DEXTERITY, sValue, SIGNED);
}



static void CharedRollWisdom (HWND hwnd)
{
	SBYTE			sValue;

	sValue = RollWisdom (CharedGetRace (hwnd));
	SetDlgItemInt (hwnd, IDE_WISDOM, sValue, SIGNED);
}



static void CharedRollConstitution (HWND hwnd)
{
	SBYTE			sValue;

	sValue = RollConstitution (CharedGetRace (hwnd));
	SetDlgItemInt (hwnd, IDE_CONSTITUTION, sValue, SIGNED);
	ChangeHitPoints (hwnd);
}



static void CharedRollCharisma (HWND hwnd)
{
	SBYTE			sValue;

	sValue = RollCharisma (CharedGetRace (hwnd));
	SetDlgItemInt (hwnd, IDE_CHARISMA, sValue, SIGNED);
}



static void CharedRollRace (HWND hwnd)
{
	HWND			hwndCtl;
	int 			iRace;

	hwndCtl = GetDlgItem (hwnd, IDC_RACE);
   iRace = random (RACE_INFO::GNOLLS + 1);
	if (iRace > RACE_INFO::GNOLLS)
		iRace = RACE_INFO::GNOLLS;
	iRace = gRaceGameToEditor[iRace];
	ComboBox_SetCurSel (hwndCtl, iRace);
}



static void CharedRollBloodline (HWND hwnd)
{
	HWND			hwndCtl;
	LONG			iType;

	iType = RollBloodlineDerivation ();
	hwndCtl = GetDlgItem (hwnd, IDC_BLOODLINE);
	ComboBox_SetCurSel (hwndCtl, iType);
}



static void CharedRollBloodStrength (HWND hwnd)
{
	HWND			hwndCtl;
	LONG			iStrength;
	SHORT			dieRoll;
	BLOODLINE_STRENGTH_INFO::TYPE iType;

	iType = RollBloodlineStrength (&dieRoll);
	hwndCtl = GetDlgItem (hwnd, IDC_BLOOD_GRADE);
	ComboBox_SetCurSel (hwndCtl, iType);

	iStrength = RollBloodlineNumericalStrength (iType);
	SetDlgItemInt (hwnd, IDE_BLOOD_STRENGTH, iStrength, SIGNED);
}



static void ChangeAppearance (HWND hwnd, HWND hwndCtl, int change)
{
	static int 	lastIndex = -1;

	if (change < 0 && giPortrait > 0)
		giPortrait--;
	else if (change > 0 && giPortrait < APPEARANCE_INFO::APPEARANCE_MAX_INDEX - 1)
		giPortrait++;

	if (giPortrait != lastIndex)
	{
		lastIndex = giPortrait;
		ShowNewAppearance (hwnd, giPortrait);
	}
}



static void ChangeClass2 (HWND hwnd, HWND hwndCtl, UINT codeNotify)
{
	if (codeNotify == CBN_SELCHANGE)
	{
		EnableClass2Windows (hwnd);
	}
}



static BOOL SaveAVAFile (HWND hwnd)
{
// Returns TRUE if it worked, FALSE if not.

	// update the data structures
	

	if (!UpdateAvaData(hwnd))
		return FALSE;	
	
	// save the file
	
	// [d5-28-97 JPC] Since we want to WRITE the files, we want to use
	// fopen, not FileOpen.  (FileOpen will go to the CD if the file does
	// not exist on the hard drive.  In most cases, the file will NOT
	// already exist on the hard drive, so we would end up trying to
	// write to the CD, which just won't work.)
	FILE * fp = fopen (gszAvaFile, "wt");
	if (fp != NULL)
	{
		// Now actually write the data.
		gAvaData.mfWriteTextData (fp);
		// FileClose (fp);
		fclose (fp);
		return TRUE;
	}
	
	return FALSE;
}

static BOOL UpdateAvaData(HWND hwnd)
{
	HWND			hwndCtl;
	BOOL			fTranslated;
	int			iClass;
	int			iLevel;
	int			iRace;
	char			szName[MAX_NAME_LEN + 1];

	// Post the dialog information to the gAvaData structure.
	THINGTYPE thingType = APPEARANCE_INFO::mfGetType (APPEARANCE_INFO::TYPE(giPortrait));
	gAvaData.mfSetThingType (thingType);

	gAvaData.mfSetStrength ((int) GetDlgItemInt (hwnd, IDE_STRENGTH, &fTranslated, SIGNED));
	if (!fTranslated)
		return FALSE;

	gAvaData.mfSetDexterity ((int) GetDlgItemInt (hwnd, IDE_DEXTERITY, &fTranslated, SIGNED));
	if (!fTranslated)
		return FALSE;

	gAvaData.mfSetConstitution ((int) GetDlgItemInt (hwnd, IDE_CONSTITUTION, &fTranslated, SIGNED));
	if (!fTranslated)
		return FALSE;

	gAvaData.mfSetIntelligence ((int) GetDlgItemInt (hwnd, IDE_INTELLIGENCE, &fTranslated, SIGNED));
	if (!fTranslated)
		return FALSE;

	gAvaData.mfSetWisdom ((int) GetDlgItemInt (hwnd, IDE_WISDOM, &fTranslated, SIGNED));
	if (!fTranslated)
		return FALSE;

	gAvaData.mfSetCharisma ((int) GetDlgItemInt (hwnd, IDE_CHARISMA, &fTranslated, SIGNED));
	if (!fTranslated)
		return FALSE;

	iRace = ComboBox_GetCurSel (GetDlgItem (hwnd, IDC_RACE));
   iRace = gRaceEditorToGame[iRace];
	gAvaData.mfSetRace (iRace);

	hwndCtl = GetDlgItem (hwnd, IDE_CHARNAME);
	Edit_GetText (hwndCtl, szName, MAX_NAME_LEN);
	gAvaData.mfSetName (szName);

	gAvaData.mfSetTitle (ComboBox_GetCurSel (GetDlgItem (hwnd, IDC_TITLE)));

	gAvaData.mfSetAlignment (ComboBox_GetCurSel (GetDlgItem (hwnd, IDC_ALIGNMENT)));

	gAvaData.mfSetHitPoints ((int) GetDlgItemInt (hwnd, IDE_HIT_POINTS, &fTranslated, SIGNED));

	// fArmorClass is calculated at run time if we set it to ARMOR_INFO::NO_ARMOR.
	gAvaData.mfSetArmorClass (ARMOR_INFO::NO_ARMOR);

	gAvaData.mfSetArmorType (ComboBox_GetCurSel (GetDlgItem (hwnd, IDC_ARMOR)));

	gAvaData.mfSetWeapon (ComboBox_GetCurSel (GetDlgItem (hwnd, IDC_WEAPON)));

	// fMorale is not changed or used.

	gAvaData.mfSetTotalHitPoints (0);

	// fTHAC0 is calculated at run time if we set it to 0 in the AVA file.
	gAvaData.mfSetTHAC0 (0);

	// fDamageDice is ignored.

	// fHitDice is ignored.

	// Set fExperience to 0 so it will be properly set when we run the game.
	gAvaData.mfSetExperience (0);

	iClass = ComboBox_GetCurSel (GetDlgItem (hwnd, IDC_CLASS1));
	gAvaData.mfSetClass1 (gClassInfo[iClass].index);

	iLevel = ComboBox_GetCurSel (GetDlgItem (hwnd, IDC_LEVEL1));
	iLevel++;
	gAvaData.mfSetClass1Level (iLevel);

	// Make experience factor of class 1 zero so it will be set in
	// PLAYER_CLASS_LIST::mfReadTextData.
	gAvaData.mfSetClass1Experience (0);

	iClass = ComboBox_GetCurSel (GetDlgItem (hwnd, IDC_CLASS2));
	if (iClass == 0)
	{
		gAvaData.mfSetClass2 (-1);
		gAvaData.mfSetClass2Level (0);
	}
	else
	{
		gAvaData.mfSetClass2 (gClassInfo[iClass - 1].index);
		iLevel = ComboBox_GetCurSel (GetDlgItem (hwnd, IDC_LEVEL2));
		iLevel++;
		gAvaData.mfSetClass2Level (iLevel);
	}

	// Make experience factor of class 2 zero so it will be set in
	// PLAYER_CLASS_LIST::mfReadTextData.
	gAvaData.mfSetClass2Experience (0);

	gAvaData.mfSetAttacksPerRound (ComboBox_GetCurSel (GetDlgItem (hwnd, IDC_ATTACKS_PER_ROUND)) + 1);

	gAvaData.mfSetBloodlineDerivation (ComboBox_GetCurSel (GetDlgItem (hwnd, IDC_BLOODLINE)));

	gAvaData.mfSetBloodlineGrade (ComboBox_GetCurSel (GetDlgItem (hwnd, IDC_BLOOD_GRADE)));

	gAvaData.mfSetBloodlineStrength ((int) GetDlgItemInt (hwnd, IDE_BLOOD_STRENGTH, &fTranslated, SIGNED));

	if (!fTranslated)
		return FALSE;

	return TRUE;
}

// ---------------------------------------------------------------------------
static void SetClassFlags (HWND hwnd)
{
// Set the AVA file flags fPriest and fWizard so we can select
// appropriate inventory.
// [d5-15-97 JPC] Priest always has all priest spells, so we can ignore priests.

	int			iClass;
	HWND			hwndCtl;
	BOOL			fTranslated;

	hwndCtl = GetDlgItem (hwnd, IDC_CLASS1);
	iClass = ComboBox_GetCurSel (hwndCtl);
	gAvaData.m_class1 = iClass;
	gAvaData.m_level1 = (int) GetDlgItemInt (hwnd, IDC_LEVEL1, &fTranslated, SIGNED);

	// For class 2, we have to subtract 1 from the index in the list box
	// (entry 0 in the list box is "(Not used)").
	hwndCtl = GetDlgItem (hwnd, IDC_CLASS2);
	iClass = ComboBox_GetCurSel (hwndCtl) - 1;
	gAvaData.m_class2 = iClass;
	gAvaData.m_level2 = (int) GetDlgItemInt (hwnd, IDC_LEVEL2, &fTranslated, SIGNED);
}


// ---------------------------------------------------------------------------
static void SetAbilitiesMinimaSub (HWND hwnd, LONG charClass)
{
// Helper of SetAbilitiesMinima.

	BOOL			fTranslated;
	int			i;
	int			value;

	for (i = 0; i < CHAR_MAX_ABILITIES; i++)
	{
		value = (int) GetDlgItemInt (hwnd, gaAbilityIDs[i], &fTranslated, SIGNED);
		if (fTranslated)
		{
			if (value < gaMinAbility[charClass][i])
			{
				SetDlgItemInt (hwnd, gaAbilityIDs[i], gaMinAbility[charClass][i], SIGNED);
			}
		}
#if defined (_DEBUG)
		else
		{
			fatal_error ("GetDlgItemInt failed to translate in SetAbilitiesMinimaSub");
		}
#endif
	}
}


static void SetAbilitiesMinima (HWND hwnd)
{
// Make sure abilities are the minimum required for the given class(es).
// Call when the user rolls new abilities or changes either class.
// Do NOT call when the user edits the abilities directly.

	LONG			iClass1;
	LONG			iClass2;

	iClass1 = ComboBox_GetCurSel (GetDlgItem (hwnd, IDC_CLASS1));
	SetAbilitiesMinimaSub (hwnd, iClass1);
	iClass2 = ComboBox_GetCurSel (GetDlgItem (hwnd, IDC_CLASS2));
	if (iClass2 != 0)
	{
		// The dialog box for class 2 has "(Not used)" in entry 0; correct for
		// that by subtracting 1 from the second parameter.
		SetAbilitiesMinimaSub (hwnd, iClass2 - 1);
	}
}


// ---------------------------------------------------------------------------
// WM_COMMAND dispatcher

static void OnCommand (HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
// Process commands sent from the controls in the Character Editor Dialog.

	switch (id)
	{
		case IDOK:
		case IDCANCEL:
			if (MessageBox (NULL, "Are you sure you want to quit?",
				"Birthright Character Editor",
				MB_YESNO | MB_ICONASTERISK | MB_TASKMODAL) == IDYES)
			{
				EndDialog (hwnd, IDOK);
			}
			break;
	
		case IDB_ROLL_LEVEL1:
			CharedRollLevel (hwnd, 0);
			break;
	
		case IDB_ROLL_LEVEL2:
			CharedRollLevel (hwnd, 1);
			break;
	
		case IDB_ROLL_STATS:
			CharedRollStrength (hwnd);
			CharedRollIntelligence (hwnd);
			CharedRollDexterity (hwnd);
			CharedRollWisdom (hwnd);
			CharedRollConstitution (hwnd);
			CharedRollCharisma (hwnd);
			SetAbilitiesMinima (hwnd);
			break;
	
		case IDB_ROLL_RACE:
			CharedRollRace (hwnd);
			break;

		case IDB_ROLL_BLOODLINE:
			CharedRollBloodline (hwnd);
			CharedRollBloodStrength (hwnd);
			break;

		case IDB_ROLL_HITPOINTS:
			ChangeHitPoints (hwnd);
			break;

		case IDB_PORT_PREV:
			ChangeAppearance (hwnd, hwndCtl, -1);
			break;

		case IDB_PORT_NEXT:
			ChangeAppearance (hwnd, hwndCtl, 1);
			break;

		case IDC_CLASS1:
			if (codeNotify == CBN_SELCHANGE)
				SetAbilitiesMinima (hwnd);
			break;

		case IDC_CLASS2:
			ChangeClass2 (hwnd, hwndCtl, codeNotify);
			SetAbilitiesMinima (hwnd);
			break;

		case IDB_LOAD:
			if (GetAVAFileName () == IDOK)
			{
				if (ReadAVAFile ())
				{
					InitFieldsFromAVAFile (hwnd);
					InvalidateRect (hwnd, NULL, FALSE); // update the portrait
				}
				ReadINVFile ();
				gfSetInventory = FALSE;
				EnableWindow (GetDlgItem (hwnd, IDB_SAVE), gfSaveOK);
			}
			break;

		case IDB_SAVE:
		{
			int choice = IDYES;
			if (!gfSetInventory)
			{
				choice = MessageBox (NULL, "You have not changed the inventory.\n"
				"Do you want to save anyway?", "Warning!", MB_YESNO | MB_ICONASTERISK | MB_TASKMODAL);
			}
			if (choice == IDYES)
			{
				BOOL fSuccess = SaveAVAFile (hwnd);
				if (fSuccess)
					fSuccess = WriteINVFile (gszInvFile);
				if (fSuccess)
				{
					MessageBox (NULL, "Saved character.", "Success!", MB_OK | MB_TASKMODAL);
					CloseAllFiles ();			// keep from reading file from CD
				}									// if it's now on the hard disk
				else
				{
					MessageBox (NULL, "Save did not succeed.", "Error!", MB_OK | MB_TASKMODAL);
				}
			}
			break;
		}

		case IDB_INVENTORY:
			UpdateAvaData(hwnd);
			SetClassFlags (hwnd);
			if (SelectInventory (hwnd) == IDOK)
				gfSetInventory = TRUE;
			break;

		case IDB_RESTORE:
			{
				// flush out all old file data and reload cdrom image
				CloseAllFiles ();
				SetFileMode ( FILEOPEN_CDROM_ONLY );
				if (ReadAVAFile ())
				{
					InitFieldsFromAVAFile (hwnd);
					InvalidateRect (hwnd, NULL, FALSE); // update the portrait
				}
				else
				{
					MessageBox (NULL, "You muse have the Birthright CDROM to restore default values.  Place the Birthright CDROM in your drive and try again.", "Error!", MB_OK | MB_TASKMODAL);
					break;
				}
				ReadINVFile ();
				gfSetInventory = FALSE;
				SetFileMode ( FILEOPEN_MIXED_MODE );
				EnableWindow (GetDlgItem (hwnd, IDB_SAVE), gfSaveOK);
			}
			break;
			
		default:
			break;
	}
}



// ---------------------------------------------------------------------------
// WM_PAINT handler and support functions.

static void ShowNewAppearance (HWND hwnd, int index)
{
// OLD: Put a picture in the dialog based on the current selection in the
// IDC_APPEARANCE drop-down list box.
// [d5-07-97 JPC]
// NEW: Base appearance on the giPortrait variable, which is changed via
// buttons under the portrait.

	HWND 			hwndCtl;
	HBITMAP		hbitmap;
	HGDIOBJ		oldhbitmap;
	HDC			hdc;
	HDC			hdcMem;
	BITMAP		bm;
	void *		pBuffer;
	int			w, h;
	char			szFilename[128];
	THINGTYPE 	thingType;
	RECT			rect;

	hwndCtl = GetDlgItem (hwnd, IDP_APPEARANCE);

	if ((hdc = GetDC (hwndCtl)) == NULL)
		goto ErrExit;

	if ((hdcMem = CreateCompatibleDC (hdc)) == NULL)
		goto ErrExit;

	SelectPalette (hdc, hpalApp, FALSE);
	SelectPalette (hdcMem, hpalApp, FALSE);

   thingType = APPEARANCE_INFO::mfGetType (APPEARANCE_INFO::TYPE(index));
	sprintf (szFilename, "ui\\ports_l\\lg%s.pcx", GAME_TTYPE::mfGetArtFileName (thingType));
	if ((hbitmap = PCXtoDIBSection (hdcMem, szFilename, pBuffer)) == NULL)
		goto ErrExit;

	if ((oldhbitmap = SelectObject (hdcMem, hbitmap)) == NULL)
		goto ErrExit;

	GetObject (hbitmap, sizeof(BITMAP), (LPVOID) & bm);
	GetClientRect (hwndCtl, &rect);
	w = bm.bmWidth;
	h = bm.bmHeight;
	//	[d5-14-97 JPC] Instead, do a StretchBlt for actual size of window.
	// BitBlt (hdc, 0, 0, w, h, hdcMem, 0, 0, SRCCOPY);
	StretchBlt (hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, w, h, SRCCOPY);

ErrExit:
	if (hdc != NULL)
		ReleaseDC (hwndCtl, hdc);
	if (hdcMem != NULL)
	{
		if (oldhbitmap != NULL)
			SelectObject (hdcMem, oldhbitmap);
		DeleteDC (hdcMem);
	}
	if (hbitmap != NULL)
		DeleteObject (hbitmap);
}



static void OnPaint (HWND hwnd)
{
	PAINTSTRUCT	ps;
	HWND			hwndCtl;
	int			index;

	BeginPaint (hwnd, &ps);
	EndPaint (hwnd, &ps);
	// hwndCtl = GetDlgItem (hwnd, IDC_APPEARANCE);
	// index = ComboBox_GetCurSel (hwndCtl);
	// ShowNewAppearance (hwnd, index);
	ShowNewAppearance (hwnd, giPortrait);
}



LONG GetBloodlineGrade ()
{
// Return current bloodline grade (not the one in gAvaData).

	HWND			hwndCtl;

	hwndCtl = GetDlgItem (ghwndChar, IDC_BLOOD_GRADE);
	return ComboBox_GetCurSel (hwndCtl);
}


/* ========================================================================
   Function    - IsBloodabAvail
   Description - Determines whether a given blood ability is available
					  for this character.
   Returns     - TRUE if it is available, FALSE if not.
	Note			- Uses CURRENT selection (not gAvaData) for blood abilities.
   ======================================================================== */
BOOL IsBloodabAvail (ITEMTYPE iItem)
{
	LONG			i;
	LONG			bloodlineDerivation;
	LONG			bloodlineGrade;
	HWND			hwndCtl;

	hwndCtl = GetDlgItem (ghwndChar, IDC_BLOODLINE);
	bloodlineDerivation = ComboBox_GetCurSel (hwndCtl);
	bloodlineGrade = GetBloodlineGrade () + 1;

	for (i = 0; i < cBLOOD_ABILITIES; i++)
	{
		if (gBATable[i].itemType == iItem)
		{
			return (gBATable[i].iAvail[bloodlineDerivation] != 0 &&
	         gBATable[i].iAvail[bloodlineDerivation] <= bloodlineGrade);
		}
	}

#if defined (_DEBUG)
	fatal_error ("Ran out of records in IsBloodabAvail");
#endif
	return FALSE;								// should not get here
}

/* ========================================================================
   Function    - CheckAccess
   Description - Determines whether a file can be written to; if not,
					  warns the user and gives him the chance to make the
					  file read/write.
   Returns     - nothing
   ======================================================================== */
static int CheckAccess (char * szPathname)
{
	int		choice;
	int		retval = CA_WRITABLE;
	char 		szText[512];

	if (_access (szPathname, W_OK) != 0)
	{
		if (errno == EACCES)
		{
			retval = CA_NOTWRITABLE;
			sprintf (szText, "Warning!  The file %s cannot be written to.\n"
			"Your changes cannot be saved unless the file is made writable.\n"
			"Would you like me to make the file writable?", szPathname);
			choice = MessageBox (NULL, szText, "Warning!", MB_YESNO | MB_ICONASTERISK | MB_TASKMODAL);
			if (choice == IDYES)
			{
				if (chmod (szPathname, S_IRUSR | S_IWUSR) == -1)
				{
					MessageBox (NULL, "I couldn't make the file writable.\n"
					"Your changes will not be saved.\n"
					"Try exiting the program and fixing the file manually.",
					"Error!", MB_OK | MB_ICONASTERISK | MB_TASKMODAL);
				}
				else
				{
					retval = CA_WRITABLE;
				}
			}
			else
			{
				MessageBox (NULL, "You can edit the character but your changes cannot be saved.",
					"Warning!", MB_OK | MB_ICONASTERISK | MB_TASKMODAL);
			}
		}
		else
		{
			// errno == ENOENT
			if (_access ("scenes", F_OK) != 0)
				mkdir ("scenes");				// create scenes directory on hard disk
		}
	}
	return retval;
}


// ---------------------------------------------------------------------------
static void ChangeHitPoints (HWND hwnd)
{
// If class, level, or constitution changes, recalculate hit points.

	int			iClass1, iClass2;
	PLAYER_CLASS_INFO::TYPE	class1, class2;
	int			level1, level2;
	int			constitution;
	int			hitPoints;
	int			cClasses = 1;
	int			result;
	BOOL			fTranslated;
	BOOL			fWarrior;
	DICE 			hitDice;
   DICE const *d;

	iClass1 = ComboBox_GetCurSel (GetDlgItem (hwnd, IDC_CLASS1));
	class1 = (PLAYER_CLASS_INFO::TYPE)(gClassInfo[iClass1].index);
	level1 = GetDlgItemInt (hwnd, IDC_LEVEL1, &fTranslated, SIGNED);
	if (!fTranslated)
		return;
	if (IsWarrior (iClass1))
		fWarrior = TRUE;

	iClass2 = ComboBox_GetCurSel (GetDlgItem (hwnd, IDC_CLASS2));
	if (iClass2 > 0)
	{
		cClasses = 2;
		iClass2--;
		class2 = (PLAYER_CLASS_INFO::TYPE)(gClassInfo[iClass2].index);
		level2 = GetDlgItemInt (hwnd, IDC_LEVEL2, &fTranslated, SIGNED);
		if (!fTranslated)
			return;
		if (IsWarrior (iClass2))
			fWarrior = TRUE;
	}

	constitution = GetDlgItemInt (hwnd, IDE_CONSTITUTION, &fTranslated, SIGNED);
	if (!fTranslated)
		return;

	d = PLAYER_CLASS_INFO::mfGetDice (class1, level1);
	hitDice.sbNumberOfDice  = d->sbNumberOfDice;
	hitDice.sbNumberOfSides = d->sbNumberOfSides;
	hitDice.sbModifier      = d->sbModifier;
	result = RollDice (&hitDice);
	if (fWarrior)
		result += CONSTITUTION::mfGetWarriorHitPtBonus (constitution) *
						  hitDice.sbNumberOfDice;
	else
		result += CONSTITUTION::mfGetHitPtAdjustment (constitution) *
						  hitDice.sbNumberOfDice;

	if (cClasses > 1)
	{
		d = PLAYER_CLASS_INFO::mfGetDice (class2, level2);
		hitDice.sbNumberOfDice  = d->sbNumberOfDice;
		hitDice.sbNumberOfSides = d->sbNumberOfSides;
		hitDice.sbModifier      = d->sbModifier;
		result += RollDice (&hitDice);
		if (fWarrior)
			result += CONSTITUTION::mfGetWarriorHitPtBonus (constitution) *
				hitDice.sbNumberOfDice;
		else
			result += CONSTITUTION::mfGetHitPtAdjustment (constitution) *
				hitDice.sbNumberOfDice;
	
		result /= 2;
	}

	SetDlgItemInt (hwnd, IDE_HIT_POINTS, result, SIGNED);
}


// ---------------------------------------------------------------------------
static BOOL IsWarrior (int iClass)
{
// The iClass is an index into the CLASS LISTBOX; do NOT confuse it with
// the enums for fighters, etc.

	return (iClass == CLASS_FIGHTER || iClass == CLASS_PALADIN ||
		iClass == CLASS_RANGER);
}


// ===========================================================================
// Main entry point for Character Editor Dialog

void CharedDoEditorDialog ()
{
	int			result;

	ShowWindow (hwndApp, SW_HIDE);
	result = GetAVAFileName ();
	if (result != -1 && result != IDCANCEL)
	{
		DialogBox ( hInstApp, "DIALOG_CHARED", (HWND) NULL, (DLGPROC) CharedWndProc);
	}
}



BOOL CALLBACK CharedWndProc (
   HWND     hwnd,                      // handle to dialog box
   UINT     uMsg,                      // message
   WPARAM   wParam,                    // first message parameter
   LPARAM   lParam)                    // second message parameter
{
	switch (uMsg)
	{
		HANDLE_MSG (hwnd, WM_INITDIALOG, OnInitDialog);
		HANDLE_MSG (hwnd, WM_COMMAND,    OnCommand);
		HANDLE_MSG (hwnd, WM_PAINT,      OnPaint);
		default:
			return FALSE;
	}
}


static void PutStringsInRealmListBox (HWND hwnd)
{
	LONG			iRealm;
	HWND			hwndCtl;
	char			szRealm[AVA_FILE::_MAX_REALM + 1];

	hwndCtl = GetDlgItem (hwnd, IDL_SELECT_REALM);
   for (iRealm = 1; iRealm < REALM::REALM_COUNT; iRealm++)
	{
		if (realm[iRealm].mfIsPlayable ())
		{
			strncpy (szRealm, realm[iRealm].mfGetName (), AVA_FILE::_MAX_REALM);
			ListBox_InsertString (hwndCtl, -1, szRealm);
		}
	}
	ListBox_SetCurSel (hwndCtl, 0);
}


static BOOL SelectRegentOnInitDialog (HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	POINT			pt;
	RECT			desktopRect;
	RECT			dialogRect;

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
	
	// Add realm names to list box.
	DisplayRegentsOfRealm(hwnd, NULL);

	// Disable OK until user selects a realm.
	EnableWindow (GetDlgItem (hwnd, IDOK), FALSE);

	return TRUE;
}


int RealmNameToIndex (char * szRealm)
{
// Return index to realm, given a realm name in szRealm.
// Do this rather than store a global array of realm indexes that
// parallel the entries in the list box.
// Return -1 if not found.

	int	iRealm;

   for (iRealm = 1; iRealm < REALM::REALM_COUNT; iRealm++)
	{
		// No need to use stricmp because the strings came from
		// the "realm" array in the first place.
		if (strcmp (realm[iRealm].mfGetName (), szRealm) == 0)
		{
			return iRealm;
		}
	}
	return -1;									// not found
}


BOOL DisplayRegentsOfRealm (HWND hwnd, char * szRealm)
{
// Given a realm name, display the two regents for that realm.

 	int			iRegent;
	HWND			hwndCtl;
	char			szNameAndRealm[256];
	
	FILE		*fp;

	hwndCtl = GetDlgItem (hwnd, IDL_SELECT_CHARACTER);
	ListBox_ResetContent (hwndCtl);
	
	for (iRegent = 1; iRegent < CHARACTER_COUNT; iRegent++)
	{
		sprintf (gszAvaFile, "SCENES\\ID%d.AVA", regents[iRegent].mfGetid());
		// Not here: CheckAccess (gszAvaFile);
		ReadAVAFile ();							// just to get the name
	
		strcpy(szNameAndRealm, gAvaData.mfGetName ());
		strcat(szNameAndRealm, " - ");
		strcat(szNameAndRealm, 
			STRMGR_GetStr(STR_RLM_NAME_CHAOS + regents[iRegent].mfGetRealm()));
		ListBox_InsertString (hwndCtl, -1, szNameAndRealm );
	}

	ListBox_SetCurSel (hwndCtl, 0);

	return TRUE;
}


static void SelectRegentOnCommand (HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
// Process commands sent from the controls in the Character Editor Dialog.
// hwnd       = HWND of parent
// id         = child window control ID
// hwndCtl    = HWND for child window
// codeNotify = notification code

	int			index;

	switch (id)
	{
		case IDOK:
		{
			HWND hwndRegent;
			hwndRegent = GetDlgItem (hwnd, IDL_SELECT_CHARACTER);
			index = ListBox_GetCurSel (hwndRegent) + 1;
			sprintf (gszAvaFile, "SCENES\\ID%d.AVA", regents[index].mfGetid());
			sprintf (gszInvFile, "SCENES\\ID%d.INV", regents[index].mfGetid());
			giRegent = index;
			EndDialog (hwnd, IDOK);
			break;
		}

		case IDCANCEL:
			EndDialog (hwnd, IDCANCEL);
			break;
	
		case IDL_SELECT_CHARACTER:
			if (codeNotify == LBN_SELCHANGE)
			{
				char	szRealm[AVA_FILE::_MAX_REALM + 1];
	
				index = ListBox_GetCurSel (hwndCtl) + 1;
				if ( regents[index].mfGetRealm() == REALM::NO_COUNTRY
					|| regents[index].mfGetRealm() == REALM::MIERES
					|| regents[index].mfGetRealm() == REALM::MONSTERS )
					EnableWindow (GetDlgItem (hwnd, IDOK), FALSE);
				else
					EnableWindow (GetDlgItem (hwnd, IDOK), TRUE);
			}
			break;
	
		default:
			break;
	}
}


BOOL CALLBACK SelectRegentWndProc (
   HWND     hwnd,                      // handle to dialog box
   UINT     uMsg,                      // message
   WPARAM   wParam,                    // first message parameter
   LPARAM   lParam)                    // second message parameter
{
	switch (uMsg)
	{
		HANDLE_MSG (hwnd, WM_INITDIALOG, SelectRegentOnInitDialog);
		HANDLE_MSG (hwnd, WM_COMMAND,    SelectRegentOnCommand);
		default:
			return FALSE;
	}
}


#endif


