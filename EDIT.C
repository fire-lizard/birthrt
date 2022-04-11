// ===========================================================================
// EDIT.C
//    Copyright (c) 1996   Synergistic Software
//    All Rights Reserved
//    ========================================================================
//    Filename: EDIT.C--editing functions for WAD files.
//              Windows 95 version.
//    Author:   John Conley
//
//    ========================================================================
//    Contains the following general functions:
//       EditWall
//       EditCheckCoordinates
//       EditMenu
// ===========================================================================

#if defined(_EDIT)
#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include <direct.h>					// for getcwd

#include "system.h"
#include "edit.h"
#include "engine.h"
#include "engint.h"
#include "debug.h"      // [d5-14-96 JPC]
#include "fileutil.h"   // [d5-15-96 JPC]
#include "resource.h"   // [d5-15-96 JPC]
#include "savepwad.h"   // [d5-24-96 JPC]
#include "light.h"

// From COMBATUI.CPP:
// Turns out it doesn't work because the scene has not been properly
// initialized.
CCallIncreaseAdventureScreen (LONG, LONG);
CCallDecreaseAdventureScreen (LONG, LONG);

// From PLAYER.CPP (but these are plain C):
void SetPlayer (LONG x, LONG y, LONG z, LONG a, LONG p);
#define SetPlayerXYA(x,y,a) SetPlayer((x),(y),NO_CHANGE,(a),NO_CHANGE)

// From WINSYS\KEYINT.C.  Used for window subclassing.
extern volatile unsigned char key_stat[256];
extern volatile BOOL fAnyKeyChanged;

// From MACHINE.C.  The main window and the app instance.
extern HWND hwndApp;
extern HINSTANCE hInstApp;

#define EDIT_TEXTURE    1
#define EDIT_LIGHTING   2
#define TOTAL_CONTROLS  10
#define HIGH_PEGGED		8
#define LOW_PEGGED		16

#define EM_CASTLE01	 	0
#define EM_CASTLE02		1
#define EM_UNDERGROUND	2

extern TEXTURE textures[MAX_TEXTURES];

// Global for finding which segment we clicked on.
BOOL        gfFindSeg;                 // used in RENDER.C functions

// Global handles used by message loop in MACHINE.C.
HWND        ghwndEditWallDlg;          // hwnd of the edit dialog
HWND        ghwndEditWallErrorDlg;     // hwnd of the edit errors dialog

// Global for full brightness (hooked up to F7 key, used in TEXTURES.C).
BOOL			gfFullBrightness;

// Module-level variables.
#ifdef _DEBUG
static char gszSourceFile[] = __FILE__;
#endif

static int     giMouseX;               // where mouse was when user clicked
static int     giMouseY;               // where mouse was when user clicked
static int     giSeg;                  // line segment in which click occurred
static int     giSector;               // sector in which click occurred
                                       // (we need to distinguish
                                       // sectors (floors and ceilings) from
                                       // segs (walls))
static int     giWhichTexture;         // which texture clicked on:
                                       //    UPPER, LOWER, or MIDDLE
static int  	giNewSector;   			// [d8-14-96 JPC] allow user to enter the sector number directly
// static int  	giNewLinedef;  			// [d8-14-96 JPC] allow user to enter the linedef number directly
static int  	giInputNumber;  			// [d9-09-96 JPC] allow user to enter a number
static int     giInitialValue;			// [d9-09-96 JPC] initial value of number user will edit
//static char    gcWhich[5] = "-LMU";    // for reporting which texture was clicked on
static int     giSaveTexture;          // old index to textures[] for undo
static char    gszCaption[128];			// for setting up the generic input dialog
static char    gszCurrentPrompt[128];  // for setting up the generic input dialog
static char		gszNewPrompt[128];      // for setting up the generic input dialog

static SECTOR  gSaveSector;            // old value for sector for undo
static SIDEDEF	gSaveSidedef;				// also for undo
static LINEDEF gSaveLinedef;           // also for undo
static short   giSide;                 // 0 or 1
static int     giSelectType;           // what user clicked on: typeWALL,
                                       //    typeFLOOR, or typeCEILING
static int     giSidedef;              // index to sidedefs[]
static int     giLinedef;              // index to linedefs[]
static LONG    giTexture;              // index to current wall, floor, or ceiling texture
static BOOL    gfChanged;              // if TRUE, user has edited something
static UBYTE   giCopyTexture = 255;    // which texture was grabbed with Ctrl-click
static HBRUSH  ghbrushBkgrnd;				// dialog box background color
static HBRUSH  ghbrushOld;				   // old dialog box background color
static BOOL		gfErrorDialogAlreadyShown = FALSE; // set TRUE after showing the dialog once
																 // or init to TRUE to suppress the error list
static BOOL    gfShowTransparentWarning = TRUE;  // put up message box re transparent textures

static int		gMotif = EM_CASTLE01;	// [d9-25-96 JPC] which motif to use--affects
													// which textures go in the list box.
static int		giFirstMotifTexture = 0;// [d9-25-96 JPC] keep track of the first
													// motif texture loaded so we can purge all
													// motif textures as needed.

// [d9-20-96 JPC] Add the following to make the light delta report for
// linedefs a little more clear.
static char 	*gszLightDelta[8] = {
	"BRIGHTEST",
	"BRIGHTER 2",
	"BRIGHTER 1",
	"BRIGHT",
	"NO CHANGE",
	"DIM",
	"DIMMER",
	"DIMMEST" };

// Prototypes for static functions.
static void EditChangeTexture (int iTexture, BOOL fNothing);
// static short GetLightDelta (int iSeg, short iSide);
static short GetLightDelta (int iLinedef, short iSide);
static BOOL UpdateDialogText (HWND hwndDlg, int controlID, LPSTR szText);
static BOOL UpdateDialogTextNumber (HWND hwndDlg, int controlID, int value);
static void UpdateBrightnessReport (void);
static void UpdatePeggingReport (void);
static void UpdatePositionReport (void);

// Subclassing variables:
static struct STRUCT_SUBCLASS {
   int      id;
   FARPROC  lpfn;
} gSubclass[TOTAL_CONTROLS];
static int gcSubclassedControls;

#ifdef _DEBUG
// Debugging variables:
// [d6-03-96 JPC] The F4 and F5 keys jump to these coordinates.
// These are globals rather than in-line constants so I can
// change them in a debugging session.


#if 01
// ANSIE_TW.WAD
LONG     gxF4 = 95;
LONG     gyF4 = -2044;
LONG     gaF4 = 64;
#endif

#if 0
// BATLW0.WAD, Transparent textures.
LONG     gxF4 = 2650;
LONG     gyF4 = 300;
LONG     gaF4 = 64;
#endif

LONG     gxF5 = -1205;
LONG     gyF5 = -1500;
LONG     gaF5 = 0;

LONG     gxF6 = -1205;
LONG     gyF6 = -1500;
LONG     gaF6 = 0;

#endif


// ---------------------------------------------------------------------------
static void EditEnableAll (BOOL fEnable)
{
// Enable or disable the entire dialog box.
// [d8-06-96 JPC] Change: always leave to LOAD button enabled.
// ---------------------------------------------------------------------------

   int         i;

   for (i = IDB_SAVE; i <= IDB_SAVEAS; i++)
      EnableWindow (GetDlgItem (ghwndEditWallDlg, i), fEnable);
   for (i = IDC_RADIO_SECTOR; i <= IDC_POSITION_UNPEGGED; i++)
      EnableWindow (GetDlgItem (ghwndEditWallDlg, i), fEnable);
}


// ---------------------------------------------------------------------------
static void EditUpdateDialog (void)
{
// Update various fields of the edit dialog based on the state of various
// module-level variables.
// ---------------------------------------------------------------------------

	int			i;

   if (giSeg != INVALID_SEGMENT || giSector != INVALID_SECTOR)
   {
      EditEnableAll (TRUE);
      if (giSelectType == typeWALL)
      {
         CheckRadioButton (ghwndEditWallDlg, IDC_RADIO_SECTOR,
				IDC_RADIO_LINE, IDC_RADIO_LINE);

			UpdateDialogTextNumber (ghwndEditWallDlg, IDC_EDIT_LINE, giLinedef);
			UpdateDialogTextNumber (ghwndEditWallDlg, IDC_EDIT_SECTOR, giSector);
			UpdateDialogTextNumber (ghwndEditWallDlg, IDC_EDIT_SPECIAL, linedefs[giLinedef].special);
			UpdateDialogTextNumber (ghwndEditWallDlg, IDC_EDIT_TAG, linedefs[giLinedef].tag);

			CheckRadioButton (ghwndEditWallDlg, IDC_RADIO_SIDE1,
				IDC_RADIO_SIDE2, IDC_RADIO_SIDE1 + giSide);

			CheckRadioButton (ghwndEditWallDlg, IDC_RADIO_UPPER,
				IDC_RADIO_LOWER, IDC_RADIO_UPPER + UPPER_TEXTURE - giWhichTexture);

			if (giWhichTexture == MIDDLE_TEXTURE)
         {
				// Middle textures are always UNPEGGED.
				CheckRadioButton (ghwndEditWallDlg, IDC_POSITION_PEGGED_HIGH,
					IDC_POSITION_UNPEGGED, IDC_POSITION_UNPEGGED);
            EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_POSITION_PEGGED_HIGH), FALSE);
            EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_POSITION_PEGGED_LOW ), FALSE);
            EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_POSITION_UNPEGGED   ), FALSE);
				giTexture = sidedefs[giSidedef].n3[0];
         }
         else if (giWhichTexture == LOWER_TEXTURE)
         {
				// Lower textures can be pegged low or unpegged.
				if (linedefs[giLinedef].flags & LOW_PEGGED)
				{
					CheckRadioButton (ghwndEditWallDlg, IDC_POSITION_PEGGED_HIGH,
						IDC_POSITION_UNPEGGED, IDC_POSITION_PEGGED_LOW);
				}
				else
				{
					CheckRadioButton (ghwndEditWallDlg, IDC_POSITION_PEGGED_HIGH,
						IDC_POSITION_UNPEGGED, IDC_POSITION_UNPEGGED);
				}
            EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_POSITION_PEGGED_HIGH), FALSE);
            EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_POSITION_PEGGED_LOW ), TRUE);
            EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_POSITION_UNPEGGED   ), TRUE);
				giTexture = sidedefs[giSidedef].n2[0];
         }
         else if (giWhichTexture == UPPER_TEXTURE)
			{
				// Upper textures can be pegged high or unpegged.
				if (linedefs[giLinedef].flags & HIGH_PEGGED)
				{
					CheckRadioButton (ghwndEditWallDlg, IDC_POSITION_PEGGED_HIGH,
						IDC_POSITION_UNPEGGED, IDC_POSITION_PEGGED_HIGH);
				}
				else
				{
					CheckRadioButton (ghwndEditWallDlg, IDC_POSITION_PEGGED_HIGH,
						IDC_POSITION_UNPEGGED, IDC_POSITION_UNPEGGED);
				}
            EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_POSITION_PEGGED_HIGH), TRUE);
            EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_POSITION_PEGGED_LOW ), FALSE);
            EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_POSITION_UNPEGGED   ), TRUE);
				giTexture = sidedefs[giSidedef].n1[0];
			}
			else
			{
				ASSERT (FALSE);
			}
			UpdatePositionReport ();
      }
      else if (giSelectType == typeFLOOR || giSelectType == typeCEILING)
      {
			CheckRadioButton (ghwndEditWallDlg, IDC_RADIO_SECTOR,
				IDC_RADIO_LINE, IDC_RADIO_SECTOR);

			UpdateDialogText (ghwndEditWallDlg, IDC_EDIT_LINE, "");
			UpdateDialogTextNumber (ghwndEditWallDlg, IDC_EDIT_SECTOR, giSector);
			UpdateDialogTextNumber (ghwndEditWallDlg, IDC_EDIT_SPECIAL, sectors[giSector].special);
			UpdateDialogTextNumber (ghwndEditWallDlg, IDC_EDIT_TAG, sectors[giSector].tag);

			for (i = IDC_RADIO_SIDE1; i <= IDC_RADIO_LOWER; i++)
			 	EnableWindow (GetDlgItem (ghwndEditWallDlg, i), FALSE);

			for (i = IDC_POSITION_PEGGED_HIGH; i <= IDC_POSITION_UNPEGGED; i++)
			 	EnableWindow (GetDlgItem (ghwndEditWallDlg, i), FALSE);

			if (giSelectType == typeFLOOR)
				giTexture = sectors[giSector].f_flat;
			else
				giTexture = sectors[giSector].c_flat;
		}
      UpdateDialogText (ghwndEditWallDlg, IDC_TEXTURE_VAL, textures[giTexture].name);
      UpdatePositionReport ();
      UpdateBrightnessReport ();
   }
   else
   {
      // Dim out everything.
      EditEnableAll (FALSE);
		EnableWindow (GetDlgItem (ghwndEditWallDlg, IDB_UNDO), FALSE);
   }
}


// ---------------------------------------------------------------------------
void EditWall (int x, int y, int iKeys)
{
// [d4-23-96 JPC]
// Called from MACHINE: MainWndProc, case WM_LBUTTONUP.
// Detect where users clicked, update dialog box strings accordingly.
// The detection occurs in RENDER.C in the following functions:
//    draw_wall
//    draw_lower_wall
//    draw_upper_wall
// Parameters:
//    x and y: coordinates of cursor when user clicked
//    iKeys:   key state--hook up Ctrl and Shift keys
//             for quick editing.  Ctrl-Click will copy
//             a wall type, and Shift-Click will paste it.
// ---------------------------------------------------------------------------

	char			szType[16];

	if (ghwndEditWallDlg == NULL)
		return;

   // Set up certain globals so that deep in RENDER.C, we can figure out
   // which seg (segment of a linedef) "contains" the point specified
   // by parameters x and y.  ("Contains" is in quotes because it isn't
   // the segment that contains the point; rather, it's the wall rectangle
   // (as specified by the segment) that contains the point.)
   giMouseX = x;
   giMouseY = y;
   gfFindSeg = TRUE;                   // flag that we want to find which
                                       // segment contains the point
   giSeg = INVALID_SEGMENT;            // will be set in render_view
   giSector = INVALID_SECTOR;          // will be set in render_view
   giSelectType = 0;                   // will be set in render_view
   render_view (FALSE);
   if (giSeg != INVALID_SEGMENT || giSector != INVALID_SECTOR)
   {
      if (giSelectType == typeWALL)
      {
         memcpy (&gSaveLinedef, &linedefs[giLinedef], sizeof (LINEDEF));
			memcpy (&gSaveSidedef, &sidedefs[giSidedef], sizeof (SIDEDEF));
			giTexture = seg_to_texture_num (giSeg, giSide, giWhichTexture);
         StatusLine ("linedef %d, side %d, sidedef %d, texture %d (%s: %d by %d)",
            giLinedef, giSide, giSidedef, giTexture, textures[giTexture].name,
				textures[giTexture].w, textures[giTexture].h);
			if (gfShowTransparentWarning && textures[giTexture].type == TRANSP_TEX)
			{
				int answer = MessageBox (NULL,
					"You clicked on a TRANSPARENT texture.\n"
					"Note that even if it looked as if you clicked on\n"
					"something beyond that texture, you actually\n"
					"clicked on the transparent texture.\n"
					"\n"
					"Press Cancel to suppress this message in the future.",
					"WARNING!", MB_OKCANCEL);
				if (answer == IDCANCEL)
					gfShowTransparentWarning = FALSE;
			}
      }
      else
		{
			if (giSelectType == typeFLOOR)
			{
				giTexture = sectors[giSector].f_flat;
				strcpy (szType, "floor");
			}
			else if (giSelectType == typeCEILING)
			{
				giTexture = sectors[giSector].c_flat;
				strcpy (szType, "ceiling");
			}
			StatusLine ("sector %d %s [%d], texture %d (%s: %d by %d)",
				giSector, szType, sectors[giSector].special, giTexture,
				textures[giTexture].name, textures[giTexture].w, textures[giTexture].h);
			memcpy (&gSaveSector, &sectors[giSector], sizeof (SECTOR));  // for UNDO
      }
      giSaveTexture = giTexture;        // for UNDO
   }
   gfFindSeg = FALSE;

   // Shortcuts for editing textures:
   if (iKeys & MK_CONTROL)
   {
      // Ctrl-click remembers the current texture.
      giCopyTexture = (UBYTE) giTexture;
   }
   else if ((iKeys & MK_SHIFT) && giCopyTexture != 255)
   {
      // Shift-click pastes the last sector we copied (with
      // ctrl-click) to the current wall or floor or ceiling.
      EditChangeTexture (giCopyTexture, FALSE);
   }
   EditUpdateDialog ();
   EnableWindow (GetDlgItem (ghwndEditWallDlg, IDB_UNDO), FALSE);
}


// ---------------------------------------------------------------------------
void EditCheckCoordinates (LONG x, LONG yStart, LONG yEnd, LONG iSeg,         // )
   LONG iWhichTexture, LONG iSide, LONG type)
{
// Called from various draw routines in RENDER.C to check whether the
// mouse click was in the rectangle drawn for the given segment.
//
// How it fits in:
// When the user clicks on something, the mouse up event (WM_LBUTTONUP)
// is handled in MACHINE: MainWndProc, which calls EditWall, above.
// EditWall sets a special flag ("gfFindSeg") and calls render_view.
// When gfFindSeg is set, render_view's various subroutines will call this
// routine to identify whether you clicked on a wall, a floor, or a ceiling.
// ---------------------------------------------------------------------------


   if (x == giMouseX)
   {
      if (giMouseY > yStart && giMouseY < yEnd)
      {
         if (type == typeWALL)
         {
            giLinedef = segs[iSeg].lptr;
            if (iSide == FRONT)
               giSidedef = linedefs[giLinedef].psdb;
            else
               giSidedef = linedefs[giLinedef].psdt;
            giSeg = iSeg;
            giWhichTexture = iWhichTexture;
            giSide = (short) iSide;
				giSector = sidedefs[giSidedef].sec_ptr;
         }
         else if (type == typeFLOOR || type == typeCEILING)
         {
            giSidedef = 0;
            giSector = iSeg;           // in this case, iSeg is really a sector!
            giWhichTexture = 0;
            giSide = 0;
         }
         giSelectType = type;
         gfFindSeg = FALSE;
      }
   }
}


// ---------------------------------------------------------------------------
BOOL StringToTextureIndex (LPSTR szTexture, int * o_iTexture, BOOL * o_fNothing)
{
// Given string szTexture from the list box, return index to texture
// in textures array.
// Return TRUE if found, FALSE if not.
// The name in the list box has the format TEXTURENAME: description.
// We're only interested in the TEXTURENAME.
// [d9-10-96 JPC] Added o_fNothing to flag the case where the user chooses
// texture 0 = nothing as distinct from texture 0 = some real texture.
// (It was a probably a mistake to set up the ambiguity in the first place,
// but we're stuck with it.)
// When we save the PWAD, we need to know the difference between the two
// 0 cases.  See SAVEPWAD: MakePwad.  The way we tell is by looking at the
// second character in the n1, n2, n3, f_flat, or c_flat string.  If that
// character is also 0, then it's a true null texture.
// ---------------------------------------------------------------------------

   int         i;
   int         iLen;
   char        szTextureName[9];

   for (i = 0; i < min (9, (int)strlen (szTexture)); i++)
   {
      // Copy up to and including the colon.
      if ((szTextureName[i] = szTexture[i]) == ':')
         break;
   }

   // Now turn the colon into a 0 to terminate the string.
   szTextureName[i] = 0;
   iLen = i;

	// [d9-10-96 JPC] Allow user to specify NO TEXTURE (" NOTHING").
	if (strnicmp (" NOTHING", szTextureName, 8) == 0)
	{
      *o_iTexture = 0;
		*o_fNothing = TRUE;
		return TRUE;
	}

   for (i = 0; i < last_texture; i++)
   {
      if (strnicmp (textures[i].name, szTextureName, iLen) == 0)
      {
         *o_iTexture = i;
			*o_fNothing = FALSE;
         return TRUE;
      }
   }

   return FALSE;
}


// ---------------------------------------------------------------------------
BOOL CALLBACK EditErrorDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
// Very simple dialog handler--only needs to destroy the dialog window.
// ---------------------------------------------------------------------------

   HWND        hwndListbox;
	int			nTabs = 50;

   switch (uMsg)
   {
      case WM_INITDIALOG:
         hwndListbox = GetDlgItem (hwndDlg, IDC_LIST1);
         ListBox_ResetContent (hwndListbox);
      	ListBox_SetTabStops (hwndListbox, 1, &nTabs);
// 			DeleteObject ((HBRUSH)
// 				SetClassLong (hwndDlg, GCL_HBRBACKGROUND, (LONG)
// 					CreateSolidBrush (RGB (191, 165, 150))));
         return TRUE;

      case WM_CLOSE:
// 			DeleteObject ((HBRUSH)
// 				SetClassLong (hwndDlg, GCL_HBRBACKGROUND,
// 					(LONG) GetStockObject (WHITE_BRUSH)));
         DestroyWindow (hwndDlg);
         ghwndEditWallErrorDlg = NULL;
         return TRUE;
   }

   return FALSE;
}


// ---------------------------------------------------------------------------
HWND EditCreateErrorDialog ()
{
// [d6-06-96 JPC] Helper of EditTextureDlgLoadStrings.
// If we can't load a specified texture, report that to the user.
// ---------------------------------------------------------------------------

   return CreateDialog (hInstApp, "EditWallErrorDlg", hwndApp, EditErrorDlgProc);
}


// ---------------------------------------------------------------------------
void EditAddErrorString (HWND hwndError, LPSTR szTexture)
{
// Add szTexture to the list box in the error dialog.
// ---------------------------------------------------------------------------

   HWND        hwndListbox;

   if (hwndError == NULL)
      return;                          // premature return

   hwndListbox = GetDlgItem (hwndError, IDC_LIST1);
   if (hwndListbox != NULL)
		ListBox_AddString (hwndListbox, szTexture);
}


// ---------------------------------------------------------------------------
BOOL EditLoadTexture (LPSTR szTextureDescription)
{
// Load the texture specified in the szTextureDescription string.
// [d6-06-96 JPC] Made the routine return TRUE or FALSE so that we could
// report missing textures to the user.
// ---------------------------------------------------------------------------

   int         i;
   ULONG       status;
   char        szTexture[9];           // pulled from szTextureDescription

   for (i = 0; i < 8; i++)
   {
      szTexture[i] = szTextureDescription[i];
      if (szTexture[i] == ':')
         break;
   }
   szTexture[i] = 0;
   get_texture (szTexture, &status);
   if (status == GT_GENERIC_TEXTURE)
      return FALSE;
   return TRUE;
}


// ---------------------------------------------------------------------------
BOOL EditTextureDlgLoadStrings (HWND hwndDlg)
{
// Load text strings for the Edit Wall Texture dialog from resources.
// ALSO load the specified textures!
// Do this when we first open the dialog box and when we load a new WAD.
// ---------------------------------------------------------------------------

   HWND        hwndListbox;
	HCURSOR		hCurs;
   BOOL        fCreatedErrorDialog;
	char *		pColon;
	int			nTabs = 50;
	int			iString;
	int			iTexture;
   char        szBuffer[128];
	extern int	gcTextures;					// a texture counter for debugging

	if (giFirstMotifTexture > 0)
	{
		// Release memory for existing textures to free up space.
		for (iTexture = giFirstMotifTexture; iTexture < last_texture; iTexture++)
		{
			if (textures[iTexture].t != 0)
			{
				if (DisposBlock (textures[iTexture].t) == fERROR)
				{
					// This error does not seem to do any harm.
					// The "SKY1" texture causes the error, but not always.
					// No other texture seems to cause any problems.
#if defined (_JPC)
					wsprintf (szBuffer, "Error disposing of texture %d", iTexture);
					MessageBox (NULL, szBuffer, "Error", MB_OK);
#endif
				}
			}
		}
		last_texture = giFirstMotifTexture;
		gcTextures = last_texture;
	}

	giFirstMotifTexture = last_texture;
	hwndListbox = GetDlgItem (hwndDlg, IDC_LIST1);
   ASSERT (hwndListbox != NULL);
   ListBox_ResetContent (hwndListbox);
	ListBox_SetTabStops (hwndListbox, 1, &nTabs);
	ListBox_SetHorizontalExtent (hwndListbox, 320);	// required to make WH_SCROLL work
   fCreatedErrorDialog = FALSE;
	hCurs = SetCursor ((HCURSOR)LoadCursor (NULL, IDC_WAIT));
	ListBox_AddString (hwndListbox, " NOTHING: blank");
   for (iString = 1000 + 1000 * gMotif; ; iString++)
   {
		if (LoadString (hInstApp, iString, szBuffer, 127) == 0)
			break;
		if (strcmp (szBuffer, "LASTXXXX") == 0)
			break;
		pColon = strchr (szBuffer, ':');
		if (pColon != NULL)
			*(pColon + 1) = '\t';			// change space to tab for TABSTOPS
		ListBox_AddString (hwndListbox, szBuffer);
      if (!EditLoadTexture (szBuffer) && !gfErrorDialogAlreadyShown)
      {
         if (!fCreatedErrorDialog)
            ghwndEditWallErrorDlg = EditCreateErrorDialog ();
         fCreatedErrorDialog = TRUE;
         EditAddErrorString (ghwndEditWallErrorDlg, szBuffer);
      }
   }

	SetCursor (hCurs);
#if 01
   if (fCreatedErrorDialog)
   {
      MessageBox (NULL, "One or more textures could not be found\n"
         "and will be displayed as GENERIC textures.\n"
			"Possible causes are:\n"
			"-  Textures aren't ready yet\n"
			"-  Textures are not in the GRAPHICS subdirectory."
			, "Error", MB_OK);
		gfErrorDialogAlreadyShown = TRUE;
   }
#endif
   return TRUE;
}


// ---------------------------------------------------------------------------
void EditChangeOffset (SHORT xChange, SHORT yChange)
{
// Change the texture offsets.
// Only meaningful for walls.  Just beep if user tries to change
// offsets of floors or ceilings.
// Experiment: also use to change floor and ceiling height.
// ---------------------------------------------------------------------------

   if (giSelectType == typeWALL)
   {
      sidedefs[giSidedef].xoff += xChange;
      sidedefs[giSidedef].yoff += yChange;
      UpdatePositionReport ();
		EnableWindow (GetDlgItem (ghwndEditWallDlg, IDB_UNDO), TRUE);
      gfChanged = TRUE;
   }
   else
   {
		// [d6-14-96 JPC] Move floor and ceiling up and down.
		sectors[giSector].fh += xChange;
		sectors[giSector].ch += yChange;
      UpdatePositionReport ();
		EnableWindow (GetDlgItem (ghwndEditWallDlg, IDB_UNDO), TRUE);
      gfChanged = TRUE;
		ChangeThingZ (giSector);
   }
}


// ---------------------------------------------------------------------------
LRESULT CALLBACK EditSubclassFunction (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
// Subclass function for the controls in the EditWall dialog.
// This lets the + and - keys and the arrow keys work while
// the dialog has the input focus.  Of course, this means the
// user CAN'T use the keyboard for manipulating the dialog,
// but that's the breaks.  The mouse works fine.
// ---------------------------------------------------------------------------

   FARPROC     lpfn;
   int         nID;
   int         i;
   int         iKey;

   switch (uMsg)
   {
      case WM_CHAR:
         if (EditHandleChar (wParam))
				return TRUE;
			else
				break;

      case WM_KEYDOWN:
         EditHandleKeydown (wParam, hwnd);
			return TRUE;

		case WM_KEYUP:
			iKey = (int) wParam;
         key_stat[iKey&0xFFL] &= 0x3F; // clear permit and keydown
         fAnyKeyChanged = TRUE;        // keystats is dirty
			return TRUE;
   }

	nID = GetWindowLong (hwnd, GWL_ID);
   lpfn = NULL;
   for (i = 0; i < gcSubclassedControls; i++)
   {
      if (gSubclass[i].id == nID)
      {
         lpfn = gSubclass[i].lpfn;
			// TRACE ("EditSubclassFunction: Found record %d\n", i);
         break;
      }
   }
   ASSERT (lpfn != NULL);
   return CallWindowProc (lpfn, hwnd, uMsg, wParam, lParam);
}							


// ---------------------------------------------------------------------------
void DoSubclass (HWND hwndDlg, int iSubclassItem, int nID)
{
// Helper routine of EditWallDlgProc.
// Keeps track of the controls we've subclassed.
// ---------------------------------------------------------------------------

   HWND        hwndControl;

   ASSERT (iSubclassItem < TOTAL_CONTROLS);

   gSubclass[iSubclassItem].id = nID;
   hwndControl = GetDlgItem (hwndDlg, nID);
	ASSERT (hwndControl != NULL);
   gSubclass[iSubclassItem].lpfn = (FARPROC) GetWindowLong (hwndControl, GWL_WNDPROC);
   SetWindowLong (hwndControl, GWL_WNDPROC, (LONG) EditSubclassFunction);
}


// ---------------------------------------------------------------------------
static void EditChangeTexture (int i_iTexture, BOOL fNothing)
{
// [d5-23-96 JPC]
// Here is where we actually change the texture.  This modifies the WAD data.
// ---------------------------------------------------------------------------

   if (giSelectType == typeWALL)
   {
      switch (giWhichTexture)
      {
         case LOWER_TEXTURE:
				sidedefs[giSidedef].n2 = i_iTexture;
				if (fNothing)		// flag that this really is nothing, not texture 0
					sidedefs[giSidedef].n2_noTexture = 1;	// SAVEPWAD will use this to make
																// sure the texture is saved as "-"
				else
					sidedefs[giSidedef].n2_noTexture = 0;   // any non-zero number will do
				break;
         case MIDDLE_TEXTURE:
            sidedefs[giSidedef].n3 = i_iTexture;
				if (fNothing)		// flag that this really is nothing, not texture 0
					sidedefs[giSidedef].n3_noTexture = 1;	// SAVEPWAD will use this to make
																// sure the texture is saved as "-"
				else
					sidedefs[giSidedef].n3_noTexture = 0;   // any non-zero number will do
            break;
         case UPPER_TEXTURE:
            sidedefs[giSidedef].n1 = i_iTexture;
				if (fNothing)		// flag that this really is nothing, not texture 0
					sidedefs[giSidedef].n1_noTexture = 1;	// SAVEPWAD will use this to make
																// sure the texture is saved as "-"
				else
					sidedefs[giSidedef].n1_noTexture = 0;   // any non-zero number will do
            break;
      }
   }
   else if (giSelectType == typeFLOOR)
   {
      sectors[giSector].f_flat = i_iTexture;
		if (fNothing)		// flag that this really is nothing, not texture 0
			sectors[giSector].f_flat_noTexture = 1;	// SAVEPWAD will use this to make
											 	// sure the texture is saved as "-"
		else
			sectors[giSector].f_flat_noTexture = 0;
   }
   else if (giSelectType == typeCEILING)
   {
      sectors[giSector].c_flat = i_iTexture;
		if (fNothing)		// flag that this really is nothing, not texture 0
			sectors[giSector].c_flat_noTexture = 1;	// SAVEPWAD will use this to make
														// sure the texture is saved as "-"
		else
			sectors[giSector].c_flat_noTexture = 0; // any non-zero number will do
   }
#ifdef _DEBUG
   else
   {
      // This should not happen, so assert if it does.
      ASSERT (FALSE);
   }
#endif
	giTexture = i_iTexture;
   UpdateDialogText (ghwndEditWallDlg, IDC_TEXTURE_VAL, textures[giTexture].name);
   gfChanged = TRUE;
   EnableWindow (GetDlgItem (ghwndEditWallDlg, IDB_UNDO), TRUE);
   render_view (TRUE);
}


// ---------------------------------------------------------------------------
void EditSavePWAD (void)
{
// [d5-24-96 JPC] Helper of EditWallDlgProc.
// ---------------------------------------------------------------------------

   if (MakePwad (pwad_name))
	{
      StatusLine ("Saved %s", pwad_name);
		gfChanged = FALSE;
   }
	else
      ErrorMessage ("Could not save %s", pwad_name);
}


// ---------------------------------------------------------------------------
int AskSaveWad (HWND hwnd)
{
// [d7-23-96 JPC] Add CANCEL option UNLESS fQuitting is TRUE, which means
// the user is shutting down the whole app.
// fQuitting is set in WINSYS\MACHINE.C: MainWndProc(), case WM_CLOSE.
// ---------------------------------------------------------------------------

	int			mboxAnswer;
	UINT			fuStyle;
	extern BOOL	fQuitting;

	if (fQuitting)
		fuStyle = MB_YESNO;
	else
		fuStyle = MB_YESNOCANCEL;
			
	mboxAnswer = MessageBox (hwnd, "Do you want to save your changes?", "EditWAD", fuStyle);
   if (mboxAnswer == IDYES)
   {
      EditSavePWAD ();
   }
	return mboxAnswer;
}


// ---------------------------------------------------------------------------
BOOL CALLBACK EditWallWait (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
// [d7-18-96 JPC]
// Wait dialog.
// This is just a text box that says to wait while the editor loads textures.
// The wait dialog does not accept any input, so just return FALSE.
// ---------------------------------------------------------------------------

	return FALSE;
}


// ---------------------------------------------------------------------------
BOOL CALLBACK EditSectorDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
// [d8-14-96 JPC]
// Callback function for entering a sector number.
// ---------------------------------------------------------------------------

#define MAX_STR_LEN	128

	HWND			hwndCtl;
	int			cchMax;
	WORD			wID;
	char			sz[16];

	switch (uMsg)
	{
      case WM_INITDIALOG:
			giNewSector = -1;
			wsprintf (sz, "%d", giSector);
         UpdateDialogTextNumber (hwndDlg, IDC_SECTOR_VAL, giSector);
			return TRUE;

		case WM_COMMAND:
         wID = LOWORD (wParam);
         switch (wID)
         {
				case IDOK:
					EndDialog (hwndDlg, 0);
					return TRUE;

				case IDCANCEL:
					giNewSector = -1;
					EndDialog (hwndDlg, 1);
					return TRUE;

				case IDC_EDIT_SECTOR:
					if (HIWORD (wParam) == EN_CHANGE)
					{
						hwndCtl = (HWND) (lParam);
						ASSERT (hwndCtl != NULL);
						cchMax = Edit_GetTextLength (hwndCtl) + 1;
						Edit_GetText (hwndCtl, sz, cchMax);
						giNewSector = atoi (sz);
					}
               return TRUE;
			}
	}
   return FALSE;
}


// ---------------------------------------------------------------------------
BOOL CALLBACK EditGenericDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
// [d8-14-96 JPC]
// Callback function for entering a linedef number.
// [d9-09-96 JPC] Made it more generic; can be called for various items now.
// ---------------------------------------------------------------------------

#define MAX_STR_LEN	128

	HWND			hwndCtl;
	int			cchMax;
	WORD			wID;
	char			sz[16];

	switch (uMsg)
	{
      case WM_INITDIALOG:
			giInputNumber = -1;
			wsprintf (sz, "%d", /* giLinedef */ giInitialValue);
			SetWindowText (hwndDlg, gszCaption);
         UpdateDialogText (hwndDlg, IDC_LINEDEF_VAL, sz);
         UpdateDialogText (hwndDlg, IDC_CURRENT_PROMPT, gszCurrentPrompt);
         UpdateDialogText (hwndDlg, IDC_NEW_PROMPT, gszNewPrompt);
			return TRUE;

		case WM_COMMAND:
         wID = LOWORD (wParam);
         switch (wID)
         {
				case IDOK:
					EndDialog (hwndDlg, 0);
					return TRUE;

				case IDCANCEL:
					giInputNumber = -1;
					EndDialog (hwndDlg, 1);
					return TRUE;

				case IDC_EDIT_LINE:
					if (HIWORD (wParam) == EN_CHANGE)
					{
						hwndCtl = (HWND) (lParam);
						ASSERT (hwndCtl != NULL);
						cchMax = Edit_GetTextLength (hwndCtl) + 1;
						Edit_GetText (hwndCtl, sz, cchMax);
						giInputNumber = atoi (sz);
					}
               return TRUE;
			}
	}
   return FALSE;
}


// ---------------------------------------------------------------------------
BOOL CALLBACK EditSelectMotifDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
// [d9-25-96 JPC]
// Callback function for selecting motif.  Do this before loading the main
// dialog.
// ---------------------------------------------------------------------------

	WORD			wID;

   switch (uMsg)
   {
		case WM_INITDIALOG:
			CheckRadioButton (hwndDlg, 100, 102, 100 + gMotif);
			return TRUE;

		case WM_COMMAND:
         wID = LOWORD (wParam);
         switch (wID)
         {
            case IDOK:
					if (Button_GetCheck (GetDlgItem (hwndDlg, 100)))
						gMotif = EM_CASTLE01;
					else if (Button_GetCheck (GetDlgItem (hwndDlg, 101)))
						gMotif = EM_CASTLE02;
					else
						gMotif = EM_UNDERGROUND;
					EndDialog (hwndDlg, 0);
					return TRUE;
			}
	}
	return FALSE;
}


// ---------------------------------------------------------------------------
BOOL CALLBACK EditWallDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
// Callback function for the edit wall modeless dialog.
// User can set a delta for the default lighting of + or - 7.
// Note that DECREASING the light number BRIGHTENS the wall.
// User can also edit wall texture.
// ---------------------------------------------------------------------------

#define MAX_STR_LEN	128

	HWND			hwndWaitDlg;
	WORD        wID;
	int			mboxAnswer;

   switch (uMsg)
   {
      case WM_INITDIALOG:
			hwndWaitDlg = CreateDialog (hInstApp, "EDITWALLLOADINGDLG", hwndDlg, EditWallWait);
			if (ghbrushBkgrnd == NULL)
				ghbrushBkgrnd = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
			ASSERT (ghbrushBkgrnd != NULL);
			// ghbrushOld = (HBRUSH) SetClassLong (hwndDlg, GCL_HBRBACKGROUND, (LONG) ghbrushBkgrnd);
			// ASSERT (ghbrushOld != NULL);

         if (!EditTextureDlgLoadStrings (hwndDlg))   // fill in the list box from disk text file
         {
            PostMessage (hwndDlg, WM_CLOSE, 0, 0);
            return TRUE;
         }

			// Subclass certain controls so that the + and - keys and the
         // arrow keys will still work.
         DoSubclass (hwndDlg, 0, IDB_LOAD);
         DoSubclass (hwndDlg, 1, IDB_SAVE);
         DoSubclass (hwndDlg, 2, IDB_SAVEAS);
         DoSubclass (hwndDlg, 3, IDB_UNDO);
			// [d7-23-96 JPC] Don't subclass the list box; if you do, then
			// pressing alphabetic keys when the list box has the focus
			// will sometimes cause an exception in the Windows Kernel.
         // DoSubclass (hwndDlg, 4, IDC_LIST1);
         // gcSubclassedControls = 5;
         gcSubclassedControls = 4;
         gfChanged = FALSE;
			if (hwndWaitDlg != NULL)
				DestroyWindow (hwndWaitDlg);
         return TRUE;

      case WM_CLOSE:
			mboxAnswer = IDNO;
         if (gfChanged)
            mboxAnswer = AskSaveWad (hwndDlg);
			if (mboxAnswer != IDCANCEL)
			{	
				if (ghbrushBkgrnd != NULL)
				{
					SetClassLong (hwndDlg, GCL_HBRBACKGROUND, (LONG) ghbrushOld);
					DeleteObject (ghbrushBkgrnd);
					ghbrushBkgrnd = NULL;
				}
				DestroyWindow (hwndDlg);
				ghwndEditWallDlg = NULL;
         }
			return TRUE;

#if 0
// [d8-19-96 JPC]
// A failed attempt to get WATCOM to use the correct background color
// for my dialog box.  The child controls also have white instead of
// color backgrounds.  (There seems to be a bug in WATCOM's RC utility.)
// Eventual solution: use VC4, not WATCOM, for the editor.
 		case WM_ERASEBKGND:
 			GetClientRect (hwndDlg, &rect);
 			FillRect ((HDC) wParam, &rect, ghbrushBkgrnd);
 			return TRUE;
#endif

		case WM_COMMAND:
         wID = LOWORD (wParam);
         switch (wID)
         {
            case IDOK:
               DestroyWindow (hwndDlg);
               ghwndEditWallDlg = NULL;
               return TRUE;

            case IDCANCEL:
               DestroyWindow (hwndDlg);
               ghwndEditWallDlg = NULL;
               return TRUE;

            case IDB_LOAD:
            {
               char szFilename[128];
               char szTitle[128];

					mboxAnswer = IDNO;
					if (gfChanged)
                  mboxAnswer = AskSaveWad (hwndDlg);
					if (mboxAnswer != IDCANCEL)
					{
						if (GetFileName (hwndDlg, NULL, "PWAD Files (*.WAD)\0*.WAD", "WAD",
								szFilename, szTitle, OFN_PATHMUSTEXIST, GFN_OPEN))
						{
							load_new_wad (szFilename, TYPE_PLAYERSTART1);
							init_doors ();		// [d8-06-96 JPC]
							giFirstMotifTexture = 0;
							EditTextureDlgLoadStrings (hwndDlg);
							SetFocus (hwndApp);
							giSeg = INVALID_SEGMENT;
							giSector = INVALID_SECTOR;
							EditUpdateDialog ();
							gfChanged = FALSE;
						}
						else
						{
							StatusLine ("Load operation cancelled by user");
						}
               }
					return TRUE;
            }

            case IDB_SAVE:
               EditSavePWAD ();
               SetFocus (hwndApp);
               return TRUE;

            case IDB_SAVEAS:
            {
               char szFilename[128];
               char szTitle[128];

               if (GetFileName (hwndDlg, NULL, "PWAD Files (*.WAD)\0*.WAD", "WAD",
                     szFilename, szTitle, OFN_PATHMUSTEXIST, GFN_SAVE))
               {
                  strcpy (pwad_name, szFilename);
                  EditSavePWAD ();
                  SetFocus (hwndApp);
               }
               else
               {
                  StatusLine ("Save operation cancelled by user");
               }
               return TRUE;
            }

            case IDB_UNDO:
               giTexture = giSaveTexture;

               if (giSelectType == typeWALL)
					{
                  memcpy (&linedefs[giLinedef], &gSaveLinedef, sizeof (LINEDEF));
						memcpy (&sidedefs[giSidedef], &gSaveSidedef, sizeof (SIDEDEF));
               }
					else if (giSelectType == typeFLOOR || giSelectType == typeCEILING)
					{
                  memcpy (&sectors[giSector], &gSaveSector, sizeof (SECTOR));
               }
					render_view (TRUE);
					UpdateDialogText (ghwndEditWallDlg, IDC_TEXTURE_VAL, textures[giTexture].name);
               UpdateBrightnessReport ();
					UpdatePeggingReport ();
					UpdatePositionReport ();
               SetFocus (hwndApp);
               EnableWindow (GetDlgItem (ghwndEditWallDlg, IDB_UNDO), FALSE);
               return TRUE;

				case IDC_RADIO_SECTOR:
					if (giSelectType != typeFLOOR && giSelectType != typeCEILING)
						giSelectType = typeFLOOR;
					EditUpdateDialog ();
					memcpy (&gSaveSector, &sectors[giSector], sizeof (SECTOR));  // for UNDO
					giSaveTexture = giTexture;        // for UNDO
					SetFocus (hwndApp);
               return TRUE;

				case IDC_RADIO_LINE:
					giSelectType = typeWALL;
					giSide = 0;
					giWhichTexture = MIDDLE_TEXTURE;
					giSidedef = linedefs[giLinedef].psdb;
					giSector = sidedefs[giSidedef].sec_ptr;
					EditUpdateDialog ();
					memcpy (&gSaveLinedef, &linedefs[giLinedef], sizeof (LINEDEF));	// for undo
					memcpy (&gSaveSidedef, &sidedefs[giSidedef], sizeof (SIDEDEF));	// for undo
					giSaveTexture = giTexture;        // for UNDO
					SetFocus (hwndApp);
               return TRUE;

				case IDC_RADIO_SIDE1:
               giSidedef = linedefs[giLinedef].psdb;
					giSide = 0;
					EditUpdateDialog ();
					SetFocus (hwndApp);
               return TRUE;

				case IDC_RADIO_SIDE2:
					if (linedefs[giLinedef].psdt == -1)
					{
						MessageBox (NULL, "This line has no side 2", "Notice", MB_OK);
						CheckRadioButton (ghwndEditWallDlg, IDC_RADIO_SIDE1,
							IDC_RADIO_SIDE2, IDC_RADIO_SIDE1);
					}
					else
					{
	               giSidedef = linedefs[giLinedef].psdt;
						giSide = 1;
						EditUpdateDialog ();
					}
					SetFocus (hwndApp);
               return TRUE;

				case IDC_RADIO_UPPER:
					giWhichTexture = UPPER_TEXTURE;
					EditUpdateDialog ();
					SetFocus (hwndApp);
               return TRUE;

				case IDC_RADIO_MIDDLE:
					giWhichTexture = MIDDLE_TEXTURE;
					EditUpdateDialog ();
					SetFocus (hwndApp);
               return TRUE;

				case IDC_RADIO_LOWER:
					giWhichTexture = LOWER_TEXTURE;
					EditUpdateDialog ();
					SetFocus (hwndApp);
               return TRUE;

				case IDC_EDIT_SECTOR:
					DialogBox (hInstApp, "EDITWALLSECTORDLG", hwndDlg, EditSectorDlgProc);
					if (giNewSector != -1)
					{
						giSector = giNewSector;
						if (giSelectType != typeFLOOR && giSelectType != typeCEILING)
							giSelectType = typeFLOOR;
						giSidedef = 0;
						giWhichTexture = 0;
						giSide = 0;
						EditUpdateDialog ();
						memcpy (&gSaveSector, &sectors[giSector], sizeof (SECTOR));  // for UNDO
						giSaveTexture = giTexture;        // for UNDO
					}
					SetFocus (hwndApp);
					return TRUE;

				case IDC_EDIT_LINE:
					if (giSelectType != typeWALL)
					{
						MessageBox (NULL, "You cannot edit a line number when editing a sector",
							"Note", MB_OK);
						return TRUE;
					}
					giInitialValue = giLinedef;
					strcpy (gszCaption, "Edit Wall Set Linedef Value");
					strcpy (gszCurrentPrompt, "Current linedef value:");
					strcpy (gszNewPrompt, "Enter new linedef value:");
					DialogBox (hInstApp, "EDITWALLLINEDEFDLG", hwndDlg, EditGenericDlgProc);
					if (giInputNumber != -1)
					{
						giLinedef = giInputNumber;
						giSelectType = typeWALL;
						giSide = 0;
						giWhichTexture = MIDDLE_TEXTURE;
						giSidedef = linedefs[giLinedef].psdb;
						giSector = sidedefs[giSidedef].sec_ptr;
						EditUpdateDialog ();
						memcpy (&gSaveLinedef, &linedefs[giLinedef], sizeof (LINEDEF));	// for undo
						memcpy (&gSaveSidedef, &sidedefs[giSidedef], sizeof (SIDEDEF));	// for undo
						giSaveTexture = giTexture;        // for UNDO
					}
					SetFocus (hwndApp);
					return TRUE;


				case IDC_EDIT_SPECIAL:
					if (giSelectType == typeWALL)
						giInitialValue = linedefs[giLinedef].special;
					else
						giInitialValue = sectors[giSector].special;
					strcpy (gszCaption, "Edit Wall Set Special Value");
					strcpy (gszCurrentPrompt, "Current special value:");
					strcpy (gszNewPrompt, "Enter new special value:");
					DialogBox (hInstApp, "EDITWALLLINEDEFDLG", hwndDlg, EditGenericDlgProc);
					if (giInputNumber != -1)
					{
						if (giSelectType == typeWALL)
							linedefs[giLinedef].special = giInputNumber;
						else
							sectors[giSector].special = giInputNumber;
						EditUpdateDialog ();
						EnableWindow (GetDlgItem (ghwndEditWallDlg, IDB_UNDO), TRUE);
						gfChanged = TRUE;
					}
					SetFocus (hwndApp);
               return TRUE;

				case IDC_EDIT_TAG:
					if (giSelectType == typeWALL)
						giInitialValue = linedefs[giLinedef].tag;
					else
						giInitialValue = sectors[giSector].tag;
					strcpy (gszCaption, "Edit Wall Set Tag Value");
					strcpy (gszCurrentPrompt, "Current tag value:");
					strcpy (gszNewPrompt, "Enter new tag value:");
					DialogBox (hInstApp, "EDITWALLLINEDEFDLG", hwndDlg, EditGenericDlgProc);
					if (giInputNumber != -1)
					{
						if (giSelectType == typeWALL)
							linedefs[giLinedef].tag = giInputNumber;
						else
							sectors[giSector].tag = giInputNumber;
						EditUpdateDialog ();
						EnableWindow (GetDlgItem (ghwndEditWallDlg, IDB_UNDO), TRUE);
						gfChanged = TRUE;
					}
					SetFocus (hwndApp);
               return TRUE;

				case IDC_LIST1:
					switch (HIWORD(wParam))
					{
                  HWND  hwndListbox;
                  int   iCurSel;
                  int   iTexture;
						BOOL  fNothing;
                  char  szTexture[128];

						// Select a new item in the listbox.
						case LBN_SELCHANGE:
                     hwndListbox = GetDlgItem (hwndDlg, IDC_LIST1);
							iCurSel = SendMessage (hwndListbox, LB_GETCURSEL, 0, 0);
							SendMessage (hwndListbox, LB_GETTEXT, (WPARAM) iCurSel, (LPARAM) szTexture);
                     if (StringToTextureIndex (szTexture, &iTexture, &fNothing))
                     {
                        StatusLine ("Found texture %d", iTexture);
                        EditChangeTexture (iTexture, fNothing);
                     }
                     else
                     {
                        StatusLine ("Could not find that texture in the WAD");
                     }
							break;
                  default:
                     return FALSE;
					}
               return TRUE;

				case IDC_SECTOR_LIGHT_VAL:
					giInitialValue = sectors[giSector].light;
					strcpy (gszCaption, "Edit Wall Set Sector Lighting");
					strcpy (gszCurrentPrompt, "Current light value:");
					strcpy (gszNewPrompt, "Enter new light value:");
					DialogBox (hInstApp, "EDITWALLLINEDEFDLG", hwndDlg, EditGenericDlgProc);
					if (giInputNumber >= 0 && giInputNumber <= 255)
					{
						sectors[giSector].light = giInputNumber;
						UpdateBrightnessReport ();
						EnableWindow (GetDlgItem (ghwndEditWallDlg, IDB_UNDO), TRUE);
						gfChanged = TRUE;
					}
					else
					{
						MessageBox (NULL, "Please enter a number between 0 and 255",
							"Range Error", MB_OK);
					}
               SetFocus (hwndApp);
					return TRUE;


				case IDC_POSITION_X_VAL:
					giInitialValue = sidedefs[giSidedef].xoff;
					strcpy (gszCaption, "Edit Wall Set X Offset");
					strcpy (gszCurrentPrompt, "Current x offset:");
					strcpy (gszNewPrompt, "Enter new x offset:");
					DialogBox (hInstApp, "EDITWALLLINEDEFDLG", hwndDlg, EditGenericDlgProc);
					if (giInputNumber != -1)
					{
						sidedefs[giSidedef].xoff = giInputNumber;
						EditUpdateDialog ();
						EnableWindow (GetDlgItem (ghwndEditWallDlg, IDB_UNDO), TRUE);
						gfChanged = TRUE;
					}
               SetFocus (hwndApp);
					return TRUE;

				case IDC_POSITION_Y_VAL:
					giInitialValue = sidedefs[giSidedef].yoff;
					strcpy (gszCaption, "Edit Wall Set Y Offset");
					strcpy (gszCurrentPrompt, "Current Y offset:");
					strcpy (gszNewPrompt, "Enter new y offset:");
					DialogBox (hInstApp, "EDITWALLLINEDEFDLG", hwndDlg, EditGenericDlgProc);
					if (giInputNumber != -1)
					{
						sidedefs[giSidedef].yoff = giInputNumber;
						EditUpdateDialog ();
						EnableWindow (GetDlgItem (ghwndEditWallDlg, IDB_UNDO), TRUE);
						gfChanged = TRUE;
					}
               SetFocus (hwndApp);
					return TRUE;

				case IDC_FLOOR_HEIGHT_VAL:
					giInitialValue = sectors[giSector].fh;
					strcpy (gszCaption, "Edit Wall Set Floor Height");
					strcpy (gszCurrentPrompt, "Current floor height:");
					strcpy (gszNewPrompt, "Enter new floor height:");
					DialogBox (hInstApp, "EDITWALLLINEDEFDLG", hwndDlg, EditGenericDlgProc);
					if (giInputNumber != -1)
					{
						sectors[giSector].fh = giInputNumber;
						EditUpdateDialog ();
						EnableWindow (GetDlgItem (ghwndEditWallDlg, IDB_UNDO), TRUE);
						gfChanged = TRUE;
					}
               SetFocus (hwndApp);
					return TRUE;

				case IDC_CEILING_HEIGHT_VAL:
					giInitialValue = sectors[giSector].ch;
					strcpy (gszCaption, "Edit Wall Set Ceiling Height");
					strcpy (gszCurrentPrompt, "Current ceiling height:");
					strcpy (gszNewPrompt, "Enter new ceiling height:");
					DialogBox (hInstApp, "EDITWALLLINEDEFDLG", hwndDlg, EditGenericDlgProc);
					if (giInputNumber != -1)
					{
						sectors[giSector].ch = giInputNumber;
						EditUpdateDialog ();
						EnableWindow (GetDlgItem (ghwndEditWallDlg, IDB_UNDO), TRUE);
						gfChanged = TRUE;
					}
               SetFocus (hwndApp);
					return TRUE;

				case IDC_POSITION_PEGGED_HIGH:
				case IDC_POSITION_PEGGED_LOW:
               EditChangePeg (hwndDlg, TRUE);
               SetFocus (hwndApp);
               return TRUE;

				case IDC_POSITION_UNPEGGED:
               EditChangePeg (hwndDlg, FALSE);
               SetFocus (hwndApp);
               return TRUE;
         } // end switch (wID)
         break;

		// Didn't fix the problem of losing focus after moving dialog: case WM_MOVE:
		// case WM_LBUTTONUP:					// second attempt--did odd things
      //    SetFocus (hwndApp);
      //    break;

		case WM_CHAR:
         EditHandleChar (wParam);
         break;

      case WM_KEYDOWN:
         EditHandleKeydown (wParam, hwndDlg);
         break;
   } // end switch (uMsg)
   return FALSE;
}


// ---------------------------------------------------------------------------
void EditMenu (HINSTANCE hInstance, HWND hwnd)
{
// Bring up the edit menu modeless dialog.
// ---------------------------------------------------------------------------

	extern BOOL gfWadLoaded;				// defined in LEVEL.C

	if (!gfWadLoaded)							// [d8-08-96 JPC]
		return;									// no editing at the map level!

   if (ghwndEditWallDlg == NULL)
   {
#ifdef _JPC
		TRACE ("Starting EditMenu\n");
#endif
		DialogBox (hInstApp, "EDITWALLSELECTMOTIF", hwnd, EditSelectMotifDlgProc);
		ghwndEditWallDlg = CreateDialog (hInstance, "EditWallDlg", hwnd,
         EditWallDlgProc);
      ASSERT (ghwndEditWallDlg != NULL);
		if (ghwndEditWallDlg == NULL)
			return;
      // The following should not be necessary if we use WS_VISIBLE in the
      // RC file:
      // ShowWindow (ghwndEditWallDlg, SW_SHOW);
		// Dim out certain recalcitrant controls:
		EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_LIST1), FALSE);
		EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_POSITION_PEGGED_HIGH), FALSE);
		EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_POSITION_PEGGED_LOW ), FALSE);
		EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_POSITION_UNPEGGED   ), FALSE);
   }
}


// ---------------------------------------------------------------------------
void EditChangeLighting (int iWhich)
{
// Pressing + and - gets here from local functions and MACHINE: EditMenu.
// For linedefs:
//    Change upper 6 bits of flags, 3 for sidedef 1, 3 for sidedef 2.
//    Values 1 -  3 = increase SectLight (which makes things dimmer)
//    Values 4 -  7 = decrease SectLight (which makes things brighter)
//    4 -> -4
//    5 -> -3
//    6 -> -2
//    7 -> -1
// The above was true as of 6-18-96.  Check LIGHT.H to see the exact number
// of bits used to save the light deltas.
// The light deltas are implemented in RENDER: SetLightDelta.
// For sectors, allow the value to range from 0 to 255.
// [d8-14-96 JPC] Remove giSeg; user can now set the giLinedef directly,
// and so giSeg is not necessarily accurate anymore.
// ---------------------------------------------------------------------------

   int         iLinedef;
   short       iSectorLight;
   short       iLightDelta;

   if (!IsWindow (ghwndEditWallDlg))
      return;

   if (giSelectType == typeWALL)
   {
      // iLinedef = segs[giSeg].lptr;
      // ASSERT (iLinedef == giLinedef);
      // iLightDelta = GetLightDelta (giSeg, giSide);
		iLinedef = giLinedef;
      iLightDelta = GetLightDelta (iLinedef, giSide);
      switch (iWhich)
      {
         case BRIGHTER:
            if (iLightDelta > LIGHT_MIN_NEGATIVE)
            {
               iLightDelta--;
               gfChanged = TRUE;
               EnableWindow (GetDlgItem (ghwndEditWallDlg, IDB_UNDO), TRUE);
            }
            else
               MessageBeep ((WORD)-1);
            break;

         case DIMMER:
            if (iLightDelta < LIGHT_MAX_POSITIVE)
            {
               iLightDelta++;
               gfChanged = TRUE;
               EnableWindow (GetDlgItem (ghwndEditWallDlg, IDB_UNDO), TRUE);
            }
            else
               MessageBeep ((WORD)-1);
            break;

         default:
            ASSERT (FALSE);
      }
      if (iLightDelta < 0)
         iLightDelta += LIGHT_ADD_FACTOR;
      if (giSide == 0)
      {
			linedefs[iLinedef].flags &= LIGHT_SIDE_1_MASK;
         linedefs[iLinedef].flags |= iLightDelta << SIDE_1_LIGHT_SHIFT;
      }
      else
      {
         linedefs[iLinedef].flags &= LIGHT_SIDE_2_MASK;
         linedefs[iLinedef].flags |= iLightDelta << SIDE_2_LIGHT_SHIFT;
      }
   }
   else if (giSelectType == typeFLOOR || giSelectType == typeCEILING)
   {
      // For sectors, step by 8 because we divide by 8 to get our
      // light value.  See sector_to_light ().
      iSectorLight = sectors[giSector].light;
      switch (iWhich)
      {
         case BRIGHTER:
            iSectorLight += 8;
            if (iSectorLight > 255)
            {
               MessageBeep ((WORD)-1);
               iSectorLight = 255;
            }
            break;

         case DIMMER:
            iSectorLight -= 8;
				// [d6-26-96 JPC] Keep everything as multiples of 8.
				if (iSectorLight == 247)
					iSectorLight = 248;
            if (iSectorLight < 0)
            {
               MessageBeep ((WORD)-1);
               iSectorLight = 0;
            }
            break;

         default:
            ASSERT (FALSE);
      }
      if (sectors[giSector].light != iSectorLight)
      {
         gfChanged = TRUE;
         EnableWindow (GetDlgItem (ghwndEditWallDlg, IDB_UNDO), TRUE);
         sectors[giSector].light = iSectorLight;
      }
   }
#ifdef _DEBUG
   else
   {
      // This should not happen, so assert if it does.
      ASSERT (FALSE);
   }
#endif

   if (ghwndEditWallDlg != NULL)
   {
      UpdateBrightnessReport ();
   }
}


// ---------------------------------------------------------------------------
static short GetLightDelta (int iLinedef, short iSide)
{
// Return light delta value for specified segment and side.
// ---------------------------------------------------------------------------

   short       iLightDelta;

   if (iSide == 0)
      iLightDelta = (linedefs[iLinedef].flags) >> SIDE_1_LIGHT_SHIFT;
   else
      iLightDelta = (linedefs[iLinedef].flags) >> SIDE_2_LIGHT_SHIFT;
   iLightDelta &= LIGHT_VALUE_MASK;

   // We treat the high bit as a sign bit:
   if (iLightDelta > LIGHT_MAX_POSITIVE)
      iLightDelta -= LIGHT_ADD_FACTOR;

   return iLightDelta;
}


// ---------------------------------------------------------------------------
static BOOL UpdateDialogText (HWND hwndDlg, int controlID, LPSTR szText)
{
// Print a string in the "controlID" text window of a dialog box.
// ---------------------------------------------------------------------------

   HWND hwndControl;

   if (!IsWindow (hwndDlg))
      return FALSE;

   hwndControl = GetDlgItem (hwndDlg, controlID);
   if (hwndControl != NULL)
   {
      SetWindowText (hwndControl, szText);
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}


// ---------------------------------------------------------------------------
static BOOL UpdateDialogTextNumber (HWND hwndDlg, int controlID, int number)
{
// Convert "number" to a string and print the string in the "controlID"
// text window of a dialog box.
// ---------------------------------------------------------------------------

   HWND 			hwndControl;
	char			szText[16];

   if (!IsWindow (hwndDlg))
      return FALSE;

	itoa (number, szText, 10);

	hwndControl = GetDlgItem (hwndDlg, controlID);
   if (hwndControl != NULL)
   {
      SetWindowText (hwndControl, szText);
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}


// ---------------------------------------------------------------------------
static void UpdateBrightnessReport (void)
{
// Show current brighter/dimmer level.
// ---------------------------------------------------------------------------

   short       iLightDeltaText;
   short       iSectorLight;

   if (!IsWindow (ghwndEditWallDlg))
      return;

   EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_LIGHTBOX),          TRUE);
   EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_SECTOR_LIGHT_VAL),  TRUE);
   if (giSelectType == typeWALL)
   {
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_LINEDEF_LIGHT),     TRUE);
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_LINEDEF_LIGHT_VAL), TRUE);
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_SECTOR_LIGHT),      FALSE);
      iLightDeltaText = GetLightDelta (giLinedef, giSide) + 4;
      UpdateDialogText (ghwndEditWallDlg, IDC_LINEDEF_LIGHT_VAL, gszLightDelta[iLightDeltaText]);
      iSectorLight = sectors[giSector].light;
      UpdateDialogTextNumber (ghwndEditWallDlg, IDC_SECTOR_LIGHT_VAL, iSectorLight);
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_SECTOR_LIGHT_VAL),  FALSE);
   }
   else if (giSelectType == typeFLOOR || giSelectType == typeCEILING)
   {
      UpdateDialogText (ghwndEditWallDlg, IDC_LINEDEF_LIGHT_VAL, "");
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_LINEDEF_LIGHT),     FALSE);
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_LINEDEF_LIGHT_VAL), FALSE);
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_SECTOR_LIGHT),      TRUE);
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_SECTOR_LIGHT_VAL),  TRUE);
      iSectorLight = sectors[giSector].light;
      UpdateDialogTextNumber (ghwndEditWallDlg, IDC_SECTOR_LIGHT_VAL, iSectorLight);
   }
#ifdef _DEBUG
   else
   {
      // This should not happen, so assert if it does.
      ASSERT (FALSE);
   }
#endif
}


// ---------------------------------------------------------------------------
static void UpdatePeggingReport (void)
{
   if (giSelectType == typeWALL)
	{
		if (giWhichTexture == MIDDLE_TEXTURE)
		{
			if (linedefs[giLinedef].flags & LOW_PEGGED)
			{
				CheckRadioButton (ghwndEditWallDlg, IDC_POSITION_PEGGED_HIGH,
					IDC_POSITION_UNPEGGED, IDC_POSITION_PEGGED_LOW);
			}
			else
			{
				CheckRadioButton (ghwndEditWallDlg, IDC_POSITION_PEGGED_HIGH,
					IDC_POSITION_UNPEGGED, IDC_POSITION_UNPEGGED);
			}
		}
	}
}


// ---------------------------------------------------------------------------
static void UpdatePositionReport (void)
{
// Show current brighter/dimmer level.
// ---------------------------------------------------------------------------

   if (!IsWindow (ghwndEditWallDlg))
      return;

   if (giSelectType == typeWALL)
	{
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_POSITION_X        ), TRUE);
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_POSITION_Y        ), TRUE);
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_POSITION_X_VAL    ), TRUE);
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_POSITION_Y_VAL    ), TRUE);
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_FLOOR_HEIGHT      ), FALSE);
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_CEILING_HEIGHT    ), FALSE);
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_FLOOR_HEIGHT_VAL  ), FALSE);
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_CEILING_HEIGHT_VAL), FALSE);
		UpdateDialogTextNumber (ghwndEditWallDlg, IDC_POSITION_X_VAL, sidedefs[giSidedef].xoff);
		UpdateDialogTextNumber (ghwndEditWallDlg, IDC_POSITION_Y_VAL, sidedefs[giSidedef].yoff);
	}
   else if (giSelectType == typeFLOOR || giSelectType == typeCEILING)
	{
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_POSITION_X        ), FALSE);
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_POSITION_Y        ), FALSE);
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_POSITION_X_VAL    ), FALSE);
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_POSITION_Y_VAL    ), FALSE);
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_FLOOR_HEIGHT      ), TRUE);
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_CEILING_HEIGHT    ), TRUE);
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_FLOOR_HEIGHT_VAL  ), TRUE);
      EnableWindow (GetDlgItem (ghwndEditWallDlg, IDC_CEILING_HEIGHT_VAL), TRUE);
		UpdateDialogTextNumber (ghwndEditWallDlg, IDC_FLOOR_HEIGHT_VAL  , sectors[giSector].fh);
		UpdateDialogTextNumber (ghwndEditWallDlg, IDC_CEILING_HEIGHT_VAL, sectors[giSector].ch);
	}
}


// ---------------------------------------------------------------------------
BOOL EditHandleChar (int iKey)
{
// Called from window proc (MainWndProc) in MACHINE.C as well as internally.
// Which caller depends on which window has the keyboard input focus.
// [d7-23-96 JPC] Changed return value to BOOL so caller can call default
// procedure if we don't process the key.
// ---------------------------------------------------------------------------

   if (!IsWindow (ghwndEditWallDlg))
      return FALSE;

   if (iKey == '+')
   {
      EditChangeLighting (BRIGHTER);
		return TRUE;
   }
   else if (iKey == '-')
   {
      EditChangeLighting (DIMMER);
		return TRUE;
   }
	return FALSE;
}


// ---------------------------------------------------------------------------
void EditHandleKeydown (int iKey, HWND hwnd)
{
// Called from window proc (MainWndProc) in MACHINE.C as well as internally.
// Which caller depends on which window has the keyboard input focus.
// RELEASE NOTE: Do NOT leave these things in when preparing a
// release for the WAD builders.
// This is NOT handled by the _DEBUG/NDEBUG scheme, or the _RELEASE scheme,
// so be careful.
// ---------------------------------------------------------------------------

	// POINT			p;
	// POINT			origin;
	// LONG			i;
	// ULONG			a;

	switch (iKey)
	{
		case VK_F3:
			EditMenu (hInstApp, hwnd);
			return;
#ifdef _DEBUG
		case VK_F4:
			// Handy way to jump to a specific location.
			gxF6 = player.x >> PLAYER_FIXEDPT;
			gyF6 = player.y >> PLAYER_FIXEDPT;
			gaF6 = player.a;
			SetPlayerXYA (gxF4, gyF4, gaF4);
			return;
		case VK_F5:
			// Handy way to jump to a specific location.
			gxF6 = player.x >> PLAYER_FIXEDPT;
			gyF6 = player.y >> PLAYER_FIXEDPT;
			gaF6 = player.a;
			SetPlayerXYA (gxF5, gyF5, gaF5);
			return;
		case VK_F6:
			// F6 restores previous location after you do F4 or F5.
			SetPlayerXYA (gxF6, gyF6, gaF6);
			return;
#endif
	}

   if (IsWindow (ghwndEditWallDlg))
	{
		switch (iKey)
		{
			case VK_HOME:
				EditChangeOffset (1, 0);
				break;
			case VK_END:
				EditChangeOffset (-1, 0);
				break;
			case VK_PRIOR:             // PgUp
				EditChangeOffset (0, 1);
				break;
			case VK_NEXT:              // PgDn
				EditChangeOffset (0, -1);
				break;
			default:
				iKey &= 0xFFL;
				if (!(key_stat[iKey] & 0x80))	// if not prohibit
				{
					fAnyKeyChanged = TRUE;     // keystats is dirty
					key_stat[iKey] = 0x81;     // set key and prohibit
				}
				key_stat[iKey] |= 0x40;       // key down
		}
	}
	else
	{
		iKey &= 0xFFL;
		if (!(key_stat[iKey] & 0x80))	// if not prohibit
		{
			fAnyKeyChanged = TRUE;     // keystats is dirty
			key_stat[iKey] = 0x81;     // set key and prohibit
		}
		key_stat[iKey] |= 0x40;       // key down
	}
}


// ---------------------------------------------------------------------------
void EditChangePeg (HWND hwndDlg, BOOL fPegged)
{
// Change this linedef's pegging.
//
// Comment for OLD code:
// o	WARNING! CHANGE FROM STANDARD MEANING!
// o	In DOOM, if a texture is PEGGED, then the relevant bit is CLEAR;
// o	if UNPEGGED, the bit is SET.  Upper and lower textures can be
// o	unpegged independently.
// o	Birthright is different!  In Birthright, we use the "unpegged" bits
// o	as follows:
// o	   bit 3 set: peg upper and lower textures to the ceiling
// o	   bit 4 set: peg upper and lower textures to the floor
// o	   both bits clear: don't peg textures; base them from the ground (zero)
// o	Bits 3 and 4 CANNOT BOTH BE SET in Birthright!  (Although DCK and DEU
// o	and so on allow them to be...)
// End comment from OLD code.
//
// [d6-19-96 JPC] We're going back to where high pegging affects only the
// upper texture and low pegging only the lower texture.
// We've still transposed the DOOM meanings, though.
// [d8-14-96 JPC] Remove giSeg; user can now set the giLinedef directly,
// and so giSeg is not necessarily accurate anymore.
// ---------------------------------------------------------------------------

   int         iLinedef;
	SHORT			oldValue;
	SHORT			newValue;

   if (!IsWindow (ghwndEditWallDlg))
      return;

   if (giSelectType != typeWALL)
		return;

	// iLinedef = segs[giSeg].lptr;
	// ASSERT (iLinedef == giLinedef);
	iLinedef = giLinedef;
	switch (giWhichTexture)
	{
		case LOWER_TEXTURE:
			oldValue = linedefs[iLinedef].flags & LOW_PEGGED;
			if (fPegged)
				linedefs[iLinedef].flags |= LOW_PEGGED;
			else
				linedefs[iLinedef].flags &= (SHORT) ~LOW_PEGGED;
			newValue = linedefs[iLinedef].flags & LOW_PEGGED;
			break;
		case UPPER_TEXTURE:
			oldValue = linedefs[iLinedef].flags & HIGH_PEGGED;
			if (fPegged)
				linedefs[iLinedef].flags |= HIGH_PEGGED;
			else
				linedefs[iLinedef].flags &= (SHORT) ~HIGH_PEGGED;
			newValue = linedefs[iLinedef].flags & HIGH_PEGGED;
			break;
	}

#if 0
// This was the code where pegging affected BOTH textures.
		// By default, clear both bits.
      linedefs[iLinedef].flags &= ~(HIGH_PEGGED | LOW_PEGGED);
		// Check the radio buttons and set bits accordingly.
		if (Button_GetCheck (GetDlgItem (hwndDlg, IDC_POSITION_PEGGED_HIGH)))
         linedefs[iLinedef].flags |= HIGH_PEGGED;
		else if (Button_GetCheck (GetDlgItem (hwndDlg, IDC_POSITION_PEGGED_LOW)))
         linedefs[iLinedef].flags |= LOW_PEGGED;
#endif

	if (oldValue != newValue)
	{
		EnableWindow (GetDlgItem (ghwndEditWallDlg, IDB_UNDO), TRUE);
		gfChanged = TRUE;
	}
}


// ---------------------------------------------------------------------------
void EditToggleSpeed (LONG arg1, LONG arg2)
{
// [d7-23-96 JPC]
// Toggle maxSpeed for extra-fine movement control.
// Hook up to the A key in MAIN: AddGameKeys.
// (Tried F10, but that is a Windows system key.)
// (LATER: A is a control key [see PLAYER.CPP, definition of keyUp].)
// Try B.  That was battle.  C and D are also used.  E seems to work.
// ---------------------------------------------------------------------------
#if 01
// [d7-26-96 JPC] Get it to compile.
	extern int maxSpeed;						// defined in PLAYER.CPP
   // extern int maxASpeed;               // defined in PLAYER.CPP

	MessageBeep ((WORD)-1);
	if (maxSpeed == (31 << PLAYER_FIXEDPT))
	{
		maxSpeed = 1 << PLAYER_FIXEDPT;
	// 	maxASpeed = 1;
	}
	else if (maxSpeed == (1 << PLAYER_FIXEDPT))
	{
		maxSpeed = 5 << PLAYER_FIXEDPT;
	// 	maxASpeed = 3;
	}
	else
	{
		maxSpeed = 31 << PLAYER_FIXEDPT;
	// 	maxASpeed = 11;
	}
#endif

}


// ---------------------------------------------------------------------------
void EditOpeningMessage (void)
{
// [d8-09-96 JPC]
// Moved here from GAME: init_game.
// ---------------------------------------------------------------------------

	MessageBeep (MB_ICONINFORMATION);
	MessageBox (NULL, "Press F3 to edit textures.\n\n"
		"Make sure to back up everything first!\nThank you.",
		"Notice", MB_OK);
#ifdef _JPC
	MessageBox (NULL, "IMPORTANT: This version contains many strange editing keys\n"
		"for use by JPC only.  You should not even be seeing this message.\n"
		"If you are not sure you want to run this version, exit immediately!",
		"WARNING  WARNING  WARNING WARNING", MB_OK);
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void EditDebugFunction (LONG arg1, LONG arg2)
{
	switch (arg1)
	{
		case 0:									// F7 key tests
			set_margin_size (0, 0, 0, 164 /* BTLUI_PANEL_HEIGHT */);
			clear_screen ();
			update_screen ();
			// CCallIncreaseAdventureScreen (0, 0);
			break;
		case 1:									// F8 key tests
			set_margin_size (0, 0, 0, 0);
			clear_screen ();
			update_screen ();
			// CCall DecreaseAdventureScreen (0, 0);
			break;
		case 2:									// F9 key tests
			break;
	}
}


#endif

