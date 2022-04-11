/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: MOUSE.C     -Handles low level mouse stuff
   Author: Chris Phillips
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:
   handle_mouse          -Reads the mouse, calls read_mouse
   init_mouse            -Initializes the mouse
   set_mouse_max         -sets the max screen value for the mouse
   read_mouse            -sets the position and button state variables
   set_mouse_position    -sets the mouse's position
   set_cursor            -sets the active cursor
   load_cursors          -loads the cursors
   draw_cursor           -draws a cursor
   cursor_in_box         -checks to see if the cursor is in a bounding box

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <windows.h>
#include "../system.h"
#include "../machine.h"
#include "../machint.h"
#include "../engine.h"

/* ------------------------------------------------------------------------
   Defines and Comile Flags
   ------------------------------------------------------------------------ */
#define MIN_MOUSE_X	(0)
#define MIN_MOUSE_Y	(0)
#define MAX_MOUSE_X	(VIEW_WIDTH)
#define MAX_MOUSE_Y	(VIEW_HEIGHT)
#define TOTAL_CURSORS 6

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
extern void CombineMouseMoveMessages( void );

void load_cursors();
void set_cursor(long c);
void SysShowCursor(void);
void SysHideCursor(void);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
long	old_cursor_x,old_cursor_y;
long	cursor_x,cursor_y,mouse_button;
// GWP LONG	hot_x = 0;
// GWP LONG	hot_y = 0;
char	chCursorName[32];
long	location_x,location_y;
long	mouse_click = 0;
long	mouse_present = TRUE;
//long mouse_need_seg = 0;
//long mouse_found_seg = -1;
//long mouse_found_type;
long	current_cursor;
long	need_cursor_draw=FALSE;

SHORT	cursor_bitmap;
BOOL	fAutoRestoreCursor = FALSE;

//static UBYTE	st[1024];
static BOOL		fMouseHidden = TRUE;

extern UBYTE antia_table[];
extern HWND	hwndApp;
extern LONG WindowWidth;
extern LONG WindowHeight;


/* ========================================================================
   Function    - init_mouse
   Description - initializes the mouse and then sets the maxes and mins for it
   Returns     - void
   ======================================================================== */

void init_mouse (CSTRPTR name, BOOL Centered)
{
	// GWP PTR	p;
	// GWP LONG w,h;
	char cursor_path[256];
	char path[256];

	// set windows cursor
	strcpy(chCursorName, name);
	WinCursorSet();
	
	// emulate the dos cursor
	sprintf(cursor_path, "graphics\\%s.pcx", name);
	if(InstallationType == INSTALL_SMALL)
	{
		sprintf(path,"%s%s",CDDrive,cursor_path);
	}
	else
	{
		sprintf(path,"%s",cursor_path);
	}
	cursor_bitmap = GetResourceStd(path,FALSE);
	if (cursor_bitmap == fERROR)
		fatal_error ("init_mouse: file %s not found", name);
	
	// GWP In Windows let the hot spot ride with the art.
	// GWP if(Centered)
	// GWP {
	// GWP 	p = ((PTR)BLKPTR(cursor_bitmap)) + sizeof(BITMHDR);
	// GWP 	w = ((BITMPTR)BLKPTR(cursor_bitmap))->w;
	// GWP 	h = ((BITMPTR)BLKPTR(cursor_bitmap))->h;
	// GWP
	// GWP 	hot_x = w/2;
	// GWP 	hot_y = h/2;
	// GWP }
	// GWP else
	// GWP {
	// GWP 	hot_x = 0;
	// GWP 	hot_y = 0;
	// GWP }
	
	set_mouse_max();
}

/* ========================================================================
   Function    - set_mouse_max
   Description - sets the max coords for the mouse
   Returns     - void
   ======================================================================== */
void set_mouse_max()
{
}

/* ========================================================================
   Function    - set_mouse_position
   Description - sets the position of the cursor
   Returns     - void
   ======================================================================== */
#pragma off (unreferenced)
void set_mouse_position(long x,long y)
{
}
#pragma on (unreferenced)

/* ========================================================================
   Function    - read_mouse
   Description - causes cursor_x,cursor_y and mouse_button to be set.
   Returns     - void
   ======================================================================== */
void read_mouse()
{
	RECT rc;
	POINT pt;


	GetClientRect(hwndApp, &rc);

									  // because if your shutting down these are 0 and the app traps
									  // if you execute this code. DLJ 11-25-96	
	if(rc.right && rc.bottom)
	{
		GetCursorPos(&pt);
		SetCursorPos(pt.x,pt.y);
		ScreenToClient(hwndApp, &pt);
		// GWP cursor_x = pt.x + hot_x;
		// GWP cursor_y = pt.y + hot_y;
		cursor_x = pt.x;
		cursor_y = pt.y;
	
		//GEH scale mouse to draw buffer
		cursor_x = cursor_x * WindowWidth / rc.right;
		cursor_y = cursor_y * WindowHeight / rc.bottom;
	
		//GEH MUST BE CLEARED IN MAIN LOOP: mouse_button = FALSE;
		mouse_button = FALSE;		// [GEH 9/25]
	
		if(GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		{
			mouse_button = 2;
		}
		else
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			mouse_button = 1;
		}
	}
}

/* ========================================================================
   Function    - update_buttons
   Description - causes mouse_button to be set.
   Returns     - void
   ======================================================================== */
void update_buttons()
{
	RECT rc;
	GetClientRect(hwndApp, &rc);
	
	if(rc.right && rc.bottom)
	{
		if(GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		{
			mouse_button = 2;
		}
		else
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			mouse_button = 1;
		}
	}
}

/* ========================================================================
   Function    -SysHideCursor
   Description - draws a cursor
   Returns     - void
   ======================================================================== */
void SysHideCursor (void)
{
}

/* ========================================================================
   Function    -
   Description -
   Returns     -
   ======================================================================== */

void ForceCursor (void)
{
}

/* ========================================================================
   Function    -
   Description -
   Returns     -
   ======================================================================== */

void SysShowCursor (void)
{
	if (fMouseHidden)
	{
		ForceCursor();
		fMouseHidden = FALSE;
	}
}

/* ========================================================================
   Function    - SysForceCursor
   Description - force show the cursor
   Returns     -
   ======================================================================== */
void SysForceCursor (void)
{
	ForceCursor();
	fMouseHidden = 0;
}

/* ========================================================================
   Function    - draw_cursor
   Description -
   Returns     -
   ======================================================================== */

void draw_cursor()
{
	CombineMouseMoveMessages();		// CAM 28 AUG 96
	
	WinCursorSet();					// GEH

	if (fAutoRestoreCursor)
	{
		SysHideCursor();
		SysShowCursor();
	}
	else
		ForceCursor();
}

/* ========================================================================
   Function    - cursor_in_box
   Description - checks to see if the cursor is inside a bounding box
   Returns     - returns TRUE if cursor is inside the bounding box specified
   ======================================================================== */
LONG cursor_in_box (LONG x1,LONG y1,LONG w,LONG h)
{
	return (cursor_x>x1 && cursor_y>y1 && cursor_x<x1+w && cursor_y<y1+h);
}

/* ======================================================================== */

