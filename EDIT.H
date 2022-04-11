// ===========================================================================
// EDIT.H
// [d4-23-96 JPC]
// ===========================================================================

#ifndef _EDIT_H_
#define _EDIT_H_

#if defined (__cplusplus)
extern "C" {
#endif

#define WALL_TEXTURE (0)
#define TOP_TEXTURE (1)
#define BOTTOM_TEXTURE (2)

#define INVALID_SEGMENT (-1)
#define INVALID_SECTOR  (-1)
#define BRIGHTER        1
#define DIMMER          2
#define FOREVER         while (1)

// Defines for types of objects that can be added to the database.
#define typeROOM    (1)
#define typeWALL    (2)
#define typeFLOOR   (3)
#define typeCEILING (4)

// Global handle of modeless edit dialog.
extern HWND ghwndEditWallDlg;
extern HWND ghwndEditWallErrorDlg;

// Global for finding which segment we clicked on.
extern BOOL gfFindSeg;

// Prototypes.
void EditWall (int x, int y, int iKeys);
void EditCheckCoordinates (LONG x, LONG yStart, LONG yEnd, LONG iSeg, LONG iWhichTexture, LONG iSide, LONG type);
void EditMenu (HINSTANCE hInstance, HWND hwnd);
void EditChangeLighting (int iWhich);
BOOL EditHandleChar (int iKey);
void EditHandleKeydown (int iKey, HWND hwnd);
void EditChangePeg (HWND hwndDlg, BOOL fPegged);
void EditOpeningMessage (void);

#if defined (__cplusplus)
}
#endif

#endif

