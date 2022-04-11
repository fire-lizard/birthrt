/* ========================================================================
   Copyright (c) 1990,1997	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _MENUPROT_H
#define _MENUPROT_H

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* menu defines */
#define MAX_MENUS			100
#define MAX_ACTIVE_MENUS	10

/* menu flags */
#define D_NO_PUSH		0x00000001
#define D_HILIGHTED		0x00000002
#define D_KEEP_CR		0x00000004
#define D_FLOATER		0x00000008
#define D_LABEL_OFF		0x00000010
#define D_TEXT_LEFT		0x00010000
#define D_ZOOM			0x00020000
#define D_SPIN			0x00040000
#define D_SCROLL		0x00080000
#define D_DONT_SHADE	0x00100000
#define D_UPDATE		0x00200000
#define D_FREEZE		0x00400000
#define D_INVISIBLE		0x00800000
#define D_SANS_5		0x01000000
#define D_SANS_6		0x02000000
#define D_SANS_8		0x04000000
#define D_SANS_16		0x08000000
#define D_TITL_8		0x10000000
#define D_TITL_10		0x20000000
#define D_TITL_16		0x40000000

/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
/* menu button types */
typedef enum {
	BUTTON_REGION = 0,
	BUTTON_SHADEBEVEL,
	BUTTON_COLORBEVEL,
	BUTTON_ENTRY,
	BUTTON_LISTBOX,
	BUTTON_BITMAP
} BUTTON_TYPE;

/* menu buttons */
typedef struct _BUTTON {
	LONG			Id;				// button id
	BUTTON_TYPE		btType;			// button type
	LONG			Flags;			// button control flags
	LONG			Color;			// color of button
	int				iLabel;			// text label for button
	LONG			LabelColor;		// color of label text
	CSTRPTR			pArt;			// name of art for this button
	LONG			HilightLabelColor;	// color of label text for hilight art.
	CSTRPTR			pHilight;		// name of art for hilight state
	LONG			X;				// x,y,w,y of button region
	LONG			Y;
	LONG			W;
	LONG			H;
	LONG			Key;			// hot key short cut
	PFVLL			pfFunction;		// function if region activated
	LONG			Arg;			// user arg passed to function
	int				idToolTip;		// Tooltip for the region.
} BUTTON;

typedef struct _MENU {
	BUTTON	*Buttons;
	LONG	MenuButtonCount;
} MENU, *PMENU;

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

extern POINT         zoomStart;           //menu.c

#if defined (__cplusplus)
}
#endif

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

/* menu.c */
void InitMenuSys(PMENU UserMenus, LONG Count);
void ReleaseMenu(LONG Index);
void ShowSubMenu(LONG, LONG index);
void QuitSystem(LONG, LONG);
void ShowMenu(LONG Index);
void HideSubMenu(LONG, LONG index);
void HideSubMenuWithClick(LONG MenuCombo, LONG index);
void HideMenu(LONG Index);
void RunMenus(void);
void IncrementVal( LONG unused, LONG pVal );
void DecrementVal( LONG unused, LONG pVal );
void ToggleVal(LONG unused,  LONG pVal );
LONG SetButtonValue ( LONG MenuIndex, LONG ButtonId, LONG Val);
LONG GetButtonValue ( LONG MenuIndex, LONG ButtonId);
LONG SetButtonProc ( LONG MenuIndex, LONG ButtonId, PFVLL pFunc, LONG Val, LONG Key );
LONG ChangeButtonProc ( LONG MenuIndex, LONG ButtonId, PFVLL pFunc, LONG Val, LONG Key );
LONG SetButtonLabel ( LONG MenuIndex, LONG ButtonId, int iLabel, LONG Color );
LONG SetButtonLabelColor ( LONG MenuIndex, LONG ButtonId, LONG Color );
LONG GetButtonLabelColor(LONG MenuIndex, LONG ButtonId, LONG *pLabelColor);
LONG SetButtonPosition ( LONG MenuIndex, LONG ButtonId, LONG X, LONG Y );
LONG GetButtonPosition ( LONG MenuIndex, LONG ButtonId, LONG *X, LONG *Y );
LONG SetButtonSize ( LONG MenuIndex, LONG ButtonId, LONG W, LONG H );
LONG GetButtonSize ( LONG MenuIndex, LONG ButtonId, LONG *W, LONG *H );
LONG GetButtonFlags ( LONG MenuIndex, LONG ButtonId );
LONG SetButtonFlag ( LONG MenuIndex, LONG ButtonId, LONG Flag );
LONG SetButtonType ( LONG MenuIndex, LONG ButtonId, LONG Type );
LONG ClearButtonFlag ( LONG MenuIndex, LONG ButtonId, LONG Flag );
LONG GetButtonLabel(LONG MenuIndex, LONG ButtonID, CSTRPTR *pLabel);
LONG GetButtonArt(LONG MenuIndex, LONG ButtonId, CSTRPTR *pArt);
LONG SetButtonArt ( LONG MenuIndex, LONG ButtonId, CSTRPTR pArtPath);
LONG SetButtonHilightArt ( LONG MenuIndex, LONG ButtonId, CSTRPTR pArtPath);
LONG SetButtonHilight ( LONG MenuIndex, LONG ButtonId, BOOL Hilight );
LONG SetUpdate(LONG MenuIndex, LONG ButtonId);
LONG CleanUpdate(LONG MenuIndex, LONG ButtonId);
LONG SetButtonToolTip(LONG MenuIndex, LONG ButtonId, LONG NewToolTip);
LONG ChangeButtonToolTip(LONG MenuIndex, LONG ButtonId, LONG NewToolTip);
LONG GetButtonHLabelColor(LONG MenuIndex, LONG ButtonId, LONG *pLabelColor);
void PaintActiveMenu(LONG AMenu);
BOOL IsMenuActive(LONG MenuId);
PFVLL GetButtonProc(LONG MenuIndex, LONG ButtonId);
LONG GetButtonArg(LONG MenuIndex, LONG ButtonId);
BOOL IsButtonHilighted ( LONG MenuIndex, LONG ButtonId);

#if defined (__cplusplus)
}
#endif
#endif // _MENUPROT_H
