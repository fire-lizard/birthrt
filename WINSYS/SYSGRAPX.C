/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: SYSGRAPH.C  -Handles low level graphics routines
   Author: Chris Phillips
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:
   set_svga_page        -???
   CalcPage             -Calculate the size of a page on the SVGA card
   set_hires            -Sets the system to run in high resolution
   set_lowres           -sets the system to run in Low Resolution
   init_graph           -initializes the graphics system
   init_pal             -initializes the palettes (loads the pal.pal file)
   load_pal             -loads a palette and sets it active
   close_graph          -closes the graphics system
   set_pal              -sets the active palette to another one
   update_screen        -updates the screen (copies the display buffer)
   clear_screen         -makes the screen black

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <windows.h>
#include "../resource.h"
#include "../system.h"
#include "../machine.h"
#include "../machint.h"
#include "../engine.h"
#include "../player.hxx"
#include "ddrawpro.h"

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
#define WAD_PATH			"graphics\\"
extern LONG PlayerSpeed;
extern LONG	PlayerLoresSpeed;
extern LONG	PlayerHiresSpeed;
extern SHORT	sMenusUp;					// in MENU.C

typedef struct header
{
  BITMAPINFOHEADER  Header;
  RGBQUAD           aColors[256];
} header;

typedef struct pal
{
  WORD Version;
  WORD NumberOfEntries;
  PALETTEENTRY aEntries[256];
} pal;

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
void assert_pal();

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
//static BOOL fFadedOut = FALSE;

//GEH PTR	screen;
PTR	GDIscreen;
long	disp_color;
COLORSPEC CurPal[256];
HBITMAP hOldMonoBitmap;
HDC		gViewDC = NULL;
HPALETTE hpalApp = NULL;
HPALETTE hpalOrg = NULL;

BOOL fHighRes;
SHORT	iDispBuffer = fERROR;
SHORT	sDrawType = iBLT;	  // 5 SEP 96 CAM

void *pBuffer;
header	BufferHeader;

pal LogicalPalette =
{
  0x300,
  256
};

extern HDC gdcScreen;
extern HWND hwndApp;
extern BOOL fHighRes;
extern HMENU hMenuInit;

// globals for screen window width and height from machine.c
extern LONG WindowWidth, WindowHeight;

// Local functions.......

/* ========================================================================
   Function    - ClearSystemPalette
   Description - Clear out the System palette for our use
   Returns     - void
   ======================================================================== */
void ClearSystemPalette(void)
{
    //*** A dummy palette setup

    struct
    {
        WORD Version;
        WORD NumberOfEntries;
        PALETTEENTRY aEntries[256];
    } Palette =
                {
                    0x300,
                    256
                };

  HPALETTE ScreenPalette = 0;
  HDC ScreenDC;
  int Counter;
  UINT nMapped = 0;
  BOOL bOK = FALSE;
  int  nOK = 0;

  //*** Reset everything in the system palette to black
  for(Counter = 0; Counter < 256; Counter++)
  {
		run_timers();
	    Palette.aEntries[Counter].peRed = 0;
		Palette.aEntries[Counter].peGreen = 0;
		Palette.aEntries[Counter].peBlue = 0;
		Palette.aEntries[Counter].peFlags = PC_NOCOLLAPSE;
  }

  //*** Create, select, realize, deselect, and delete the palette

  ScreenDC = GetDC(NULL);

  ScreenPalette = CreatePalette((LOGPALETTE *)&Palette);

  if (ScreenPalette)
  {
    ScreenPalette = SelectPalette(ScreenDC,ScreenPalette,FALSE);
    nMapped = RealizePalette(ScreenDC);
	// if ( sDrawMode == iDDRAW )
	// {
	// 	DDRealizePalette();
	// }
    ScreenPalette = SelectPalette(ScreenDC,ScreenPalette,FALSE);
    bOK = DeleteObject(ScreenPalette);
  }

  nOK = ReleaseDC(NULL, ScreenDC);

  return;
}

/* ========================================================================
   Function    - set_hires
   Description - sets the system to run in High Resolution mode
   Returns     - void
   ======================================================================== */
#pragma off (unreferenced)
void set_hires(LONG unused, LONG UnusedAlso)
{
}

/* ========================================================================
   Function    - set_lowres
   Description - sets the system to run in Low Resolution mode
   Returns     - void
   ======================================================================== */
#pragma off (unreferenced)
void set_lowres(LONG unused, LONG UnusedAlso)
{
}

/* ========================================================================
   Function    - init_graph
   Description - initializes the graphics system
   Returns     - void
   ======================================================================== */
void init_graph (LONG init_mode)
{
}

/* ========================================================================
   Function    - assert_pal
   Description - sets the specified palette to be active
   Returns     - void
   ======================================================================== */
void assert_pal (void)
{
	if(gdcScreen != NULL)
	{
		SelectPalette( gdcScreen, hpalApp, FALSE );
		RealizePalette( gdcScreen );
		// if ( sDrawMode == iDDRAW )
		// {
		// 	DDRealizePalette();
		// }
	}
}

/* ========================================================================
   Function    - set_pal
   Description - sets the specified palette to be active
   Returns     - void
   ======================================================================== */
void set_pal (char * pal)
{
	int Counter;

//	for(Counter = 10;Counter < 246;Counter++)
	for (Counter = 0; Counter < 256; Counter++)
	{
		// copy from the original color table to the WinGBitmap's
		// color table and the logical palette
		run_timers();
		BufferHeader.aColors[Counter].rgbRed =
		  LogicalPalette.aEntries[Counter].peRed = pal[(Counter*3)];
		BufferHeader.aColors[Counter].rgbGreen =
		  LogicalPalette.aEntries[Counter].peGreen = pal[(Counter*3)+1];
		BufferHeader.aColors[Counter].rgbBlue =
		  LogicalPalette.aEntries[Counter].peBlue = pal[(Counter*3)+2];
		BufferHeader.aColors[Counter].rgbReserved = 0;

		LogicalPalette.aEntries[Counter].peFlags = PC_NOCOLLAPSE;
	}

        // force palette positions 0 and 255
        BufferHeader.aColors[0].rgbRed =
          LogicalPalette.aEntries[0].peRed = 0;

        BufferHeader.aColors[0].rgbGreen =
          LogicalPalette.aEntries[0].peGreen = 0;

        BufferHeader.aColors[0].rgbBlue =
          LogicalPalette.aEntries[0].peBlue = 0;

        BufferHeader.aColors[255].rgbRed =
          LogicalPalette.aEntries[255].peRed = 255;

        BufferHeader.aColors[255].rgbGreen =
          LogicalPalette.aEntries[255].peGreen = 255;

        BufferHeader.aColors[255].rgbBlue =
          LogicalPalette.aEntries[255].peBlue = 255;

	// create the new palette
	if(hpalApp == NULL)
	{
#ifndef _DEBUG
//		SetSystemPaletteUse(gdcScreen, SYSPAL_NOSTATIC);
//		SetSystemPaletteUse(gViewDC, SYSPAL_NOSTATIC);
#endif

		// create a new System palette
		hpalApp = CreatePalette((LOGPALETTE far *)&LogicalPalette);
		// put in the system
		hpalOrg = SelectPalette( gdcScreen, hpalApp, FALSE );
		// put it in my off screen DIB
		SetDIBColorTable( gViewDC, 0, 255, &BufferHeader.aColors[0]);

		// if ( sDrawMode == iDDRAW )
		// {
		// 	DDRealizePalette();
		// }
	}
	//  replace the current palette
	else
	{
		HDC hdcTmp;
#ifndef _DEBUG
//		SetSystemPaletteUse(gdcScreen, SYSPAL_NOSTATIC);
//		SetSystemPaletteUse(gViewDC, SYSPAL_NOSTATIC);
#endif

		// set the System Palette
		SelectPalette( gdcScreen, hpalOrg, FALSE );
		UnrealizeObject( hpalApp );
		SetPaletteEntries( hpalApp, 0, 255, &LogicalPalette.aEntries[0]);
		hdcTmp = GetDC( hwndApp );
		hpalOrg = SelectPalette( hdcTmp, hpalApp, FALSE );
		RealizePalette( hdcTmp );
		// if ( sDrawMode == iDDRAW )
		// {
		// 	DDRealizePalette();
		// }
		ReleaseDC( hwndApp, hdcTmp );
		
		// new set the off-screen DIB buffer palette
		SetDIBColorTable( gViewDC, 0, 255, &BufferHeader.aColors[0]);
	}

	assert_pal();
}

/* ========================================================================
   Function    - load_pal
   Description - loads a palette and sets to the active one
   Returns     - void
   ======================================================================== */
void load_pal(char *n)
{
	FILE *f;
	//int Counter;
	//HDC Screen;

	f=FileOpen(n,"rb");
	if(f==NULL)
		fatal_error("Palette not found: %s",n);

	// for .col files burn the first 8 bytes
	fread(&CurPal[0],sizeof(char),8,f);
	fread(&CurPal[0],sizeof(char),768,f);
	FileClose(f);


	set_pal((char *)&CurPal[0]);
}

/* ========================================================================
   Function    - init_pal
   Description - initializes the palette
   Returns     - void
   ======================================================================== */
void init_pal(char *PalName)
{
	char	pathbuffer[80];
	// Clear the System Palette so that blting runs at full speed.
	ClearSystemPalette();

	// get the file name of the pal to load
	sprintf(pathbuffer, "%s%s.col", WAD_PATH, PalName);
	load_pal(pathbuffer);
}

/* ====================================================================
	FadeOut			- Fade to black
	==================================================================== */
void FadeOut (USHORT steps)
{
}

/* ====================================================================
	FadeIn			- Fade from black to CurPal
	==================================================================== */
void FadeIn (USHORT steps)
{
}

/* ========================================================================
   Function    - close_graph
   Description - closes the graphics system
   Returns     - void
   ======================================================================== */
void close_graph()
{
    HBITMAP hbm;
	HDC ScreenDC;

	ScreenDC = GetDC(NULL);
#ifndef _DEBUG
//	SetSystemPaletteUse(ScreenDC, SYSPAL_STATIC);
#endif
	ReleaseDC(NULL, ScreenDC);

    if(gdcScreen)
    {
        hbm = (HBITMAP)SelectObject(gdcScreen, hOldMonoBitmap);

        DeleteObject(hbm);
        DeleteObject(gViewDC);
    }
}

/* ========================================================================
   Function    - update_screen
   Description - copies the display buffer into video ram
   Returns     - void
   ======================================================================== */
void update_screen(void)
{
}

/* ========================================================================
   Function    - clear_screen
   Description - clears the screen
   Returns     - void
   ======================================================================== */
/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
	//This could be a generic function, but some machines might
	//have a fast clear screen... Under Nova it's not a terribly time
	//critical routine.. i.e it's not called during a typical render.

void clear_screen()
{
}

/* ========================================================================
   Function    - clear_screen_to
   Description - like clear screen, but lets you specify the clear color.
						Purpose: solve the "mouse droppings" problem at the
						edge of the advisors screen and the adventure screen
						by clearing screen to 1 instead of 0.
						[d3-14-97 JPC]
   Returns     - void
   ======================================================================== */
void clear_screen_to(int c)
{
}


/* ========================================================================
   Function    - clear_display
   Description - clears the screen
   Returns     - void
   ======================================================================== */
/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
	//This could be a generic function, but some machines might
	//have a fast clear screen... Under Nova it's not a terribly time
	//critical routine.. i.e it's not called during a typical render.

void clear_display()
{
}

/* ========================================================================
   Function    - ScreenCopy
   Description - copy bitmap data directly to the video card
   Returns     -
   ======================================================================== */
void ScreenCopy (
	SHORT	iSrcBitm,
	SHORT	xSrc,
	SHORT	ySrc,
	SHORT	width,
	SHORT	height,
	SHORT resolution							// [d10-26-96 JPC] new
	)
{

}




void DirectDrawSaveScreen( void )
{
} // DirectDrawSaveScreen
