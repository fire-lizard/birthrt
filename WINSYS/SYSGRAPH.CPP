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
#include <Windows.h>
#include "../RESOURCE.H"
#include "../SYSTEM.H"
#include "../MACHINE.H"
#include "../MACHINT.H"
#include "../ENGINE.H"
#include "../PLAYER.HXX"
#include "../MENU.H"
#include "DDRAWPRO.H"


/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
#define WAD_PATH			"graphics\\"

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
static BOOL fFadedOut = FALSE;

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

void *pBuffer = NULL;
header	BufferHeader;

// Scale2x upscale stage: at window sizes above 640x480 the frame is
// edge-smoothed into this 2x DIB before the final stretch, so the
// paletted art scales cleanly instead of blocky nearest-neighbor.
// NULL means fall back to the direct stretch.
static HDC		gScaleDC = NULL;
static PUCHAR	gScaleBuf = NULL;
static HBITMAP	hOldScaleBitmap = NULL;

pal LogicalPalette =
{
  0x300,
  256
};

extern HDC gdcScreen;
extern HWND hwndApp;
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

		// Palette.aEntries[Counter].peFlags = PC_RESERVED;

		Palette.aEntries[Counter].peFlags = PC_NOCOLLAPSE;
	}
	
	//*** Create, select, realize, deselect, and delete the palette
	
	ScreenDC = GetDC(NULL);
	
	ScreenPalette = CreatePalette((LOGPALETTE *)&Palette);

	if (ScreenPalette)
	{
		ScreenPalette = SelectPalette(ScreenDC,ScreenPalette,FALSE);
		nMapped = RealizePalette(ScreenDC);

		if ( sDrawMode == iDDRAW )
		{
			memcpy( &LogicalPalette, &Palette, sizeof ( LogicalPalette));
			DDRealizePalette();
		}

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
	LONG NewMapCX;
	LONG NewMapCY;
	
#pragma on (unreferenced)

	if (fHighRes == FALSE)
	{
		// The buffer is always 640x480; the window client may be larger and
		// update_screen stretch-blits to it. Never derive buffer size from
		// the client rect (a 16:9 window would yield a 640x360 buffer).
		WindowWidth = MAX_VIEW_WIDTH;
		WindowHeight = MAX_VIEW_HEIGHT;

		fHighRes = TRUE;
		set_screen_size(WindowWidth,WindowHeight);
	
		assert_pal();
		set_mouse_max();
	
		ZoomMapAbsolute(0,MapZoomFactor()/2);
	
		NewMapCX=MapCenterX()/2;
		NewMapCY=MapCenterY()*2;
		SetMapCenter(NewMapCX,NewMapCY);
	
		PlayerSpeed = PlayerHiresSpeed;
	
		// [d10-27-96 JPC] Move earlier: fHighRes = TRUE;
	}
}

/* ========================================================================
   Function    - set_lowres
   Description - sets the system to run in Low Resolution mode
   Returns     - void
   ======================================================================== */
#pragma off (unreferenced)
void set_lowres(LONG unused, LONG UnusedAlso)
{
	LONG NewMapCX;
	LONG NewMapCY;
#pragma on (unreferenced)

	// [d11-12-96 JPC] If any menu is up, do not allow low res.
	// (At this point, quick and sleazy fixes are acceptable.)
	if (sMenusUp != 0)
		return;

	if (fHighRes == TRUE)
	{
		// [d10-26-96 JPC] Restore the old way of doing this instead of using
		// constants.
		WindowWidth /= 2;
		WindowHeight /= 2;

		fHighRes = FALSE;
		set_screen_size(WindowWidth,WindowHeight);
	
		assert_pal();
		set_mouse_max();
	
		ZoomMapAbsolute(0,MapZoomFactor()*2);
	
		NewMapCX=MapCenterX()*2;
		NewMapCY=MapCenterY()/2;
		SetMapCenter(NewMapCX,NewMapCY);
	
		PlayerSpeed = (SHORT)PlayerLoresSpeed;
	
		// [d10-27-96 JPC] Move earlier: fHighRes = FALSE;
	}
}

/* ========================================================================
   Function    - init_graph
   Description - initializes the graphics system
   Returns     - void
   ======================================================================== */
void init_graph (LONG init_mode)
{
	int dx, dy;
	HDC Screen;
	HBITMAP hbm;

	if(init_mode)
	{
		fHighRes = FALSE;	// Guarentee the resolution will change.
		set_hires(0L, 0L);
	}
	else
	{
		fHighRes = TRUE;	// Guarentee the resolution will change.
		set_lowres(0L, 0L);
	}

	init_pal("default");

	/* setup defaults */
	dx = 640;
	dy = 480;

	//  Create a new HDC and 8-bit Bitmap
   BufferHeader.Header.biSize = sizeof(BITMAPINFOHEADER);
   BufferHeader.Header.biPlanes = 1;
   BufferHeader.Header.biBitCount = 8;
   BufferHeader.Header.biCompression = BI_RGB;
   BufferHeader.Header.biSizeImage = 0;
   BufferHeader.Header.biClrUsed = 0;
   BufferHeader.Header.biClrImportant = 0;

	BufferHeader.Header.biWidth = dx;
	BufferHeader.Header.biHeight = dy;

	// force bitmap to top-down
	BufferHeader.Header.biHeight = -(abs(BufferHeader.Header.biHeight));

	// create the offscreen buffer
	BufferHeader.Header.biWidth = MAX_VIEW_WIDTH;
	BufferHeader.Header.biHeight = MAX_VIEW_HEIGHT + 1;

	// force bitmap to top-down
	BufferHeader.Header.biHeight = -(abs(BufferHeader.Header.biHeight));

	//  get the dc for the screen
	Screen = GetDC(0);

	gViewDC = CreateCompatibleDC(Screen);
	SetMapMode(gViewDC, MM_TEXT);

	hbm = CreateDIBSection(	gViewDC, (BITMAPINFO far *)&BufferHeader, 
									DIB_RGB_COLORS, &pBuffer, NULL, 0);

	screen = (PUCHAR) pBuffer;
	GDIscreen = (PUCHAR) pBuffer; // save a copy
	
	iDispBuffer = NewExternalBlock(screen);

	//  Store the old hbitmap to select back in before deleting
	hOldMonoBitmap = (HBITMAP)SelectObject(gViewDC, hbm);

	SelectPalette( gViewDC, hpalApp, FALSE );
	RealizePalette( gViewDC );

	// clear this offscreen buffer
	clear_screen();

	// Create the 2x Scale2x intermediate buffer. On failure we silently
	// keep gScaleDC NULL and present with the direct stretch instead.
	if (gScaleDC == NULL)
	{
		BufferHeader.Header.biWidth = MAX_VIEW_WIDTH * 2;
		BufferHeader.Header.biHeight = -(MAX_VIEW_HEIGHT * 2 + 1);

		gScaleDC = CreateCompatibleDC(Screen);
		if (gScaleDC != NULL)
		{
			void *pScale = NULL;
			HBITMAP hbmScale;

			SetMapMode(gScaleDC, MM_TEXT);
			hbmScale = CreateDIBSection(gScaleDC, (BITMAPINFO far *)&BufferHeader,
											DIB_RGB_COLORS, &pScale, NULL, 0);
			if (hbmScale != NULL)
			{
				gScaleBuf = (PUCHAR) pScale;
				hOldScaleBitmap = (HBITMAP)SelectObject(gScaleDC, hbmScale);
				SelectPalette( gScaleDC, hpalApp, FALSE );
				RealizePalette( gScaleDC );
			}
			else
			{
				DeleteDC(gScaleDC);
				gScaleDC = NULL;
			}
		}

		// restore the header for the main buffer's users (set_pal et al.)
		BufferHeader.Header.biWidth = MAX_VIEW_WIDTH;
		BufferHeader.Header.biHeight = -(MAX_VIEW_HEIGHT + 1);
	}

	// dump the temp hdc
	ReleaseDC(0,Screen);

	init_shade_table("default");
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

		if ( sDrawMode == iDDRAW )
		{
			DDRealizePalette();
		}
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

		// create a new System palette
		hpalApp = CreatePalette((LOGPALETTE far *)&LogicalPalette);

		// put in the system
		hpalOrg = SelectPalette( gdcScreen, hpalApp, FALSE );

		// put it in my off screen DIB
		SetDIBColorTable( gViewDC, 0, 255, &BufferHeader.aColors[0]);
		if (gScaleDC != NULL)
			SetDIBColorTable( gScaleDC, 0, 255, &BufferHeader.aColors[0]);

		assert_pal();
	
	}
	//  replace the current palette
	else
	{
		HDC hdcTmp;

		// set the System Palette
		SelectPalette( gdcScreen, hpalOrg, FALSE );
		UnrealizeObject( hpalApp );

		SetPaletteEntries( hpalApp, 0, 255, &LogicalPalette.aEntries[0]);

		hdcTmp = GetDC( hwndApp );
		hpalOrg = SelectPalette( hdcTmp, hpalApp, FALSE );
		RealizePalette( hdcTmp );

		if ( sDrawMode == iDDRAW )
		{
			DDSetPaletteEntries();
		}

		ReleaseDC( hwndApp, hdcTmp );
		
		// new set the off-screen DIB buffer palette
		SetDIBColorTable( gViewDC, 0, 255, &BufferHeader.aColors[0]);
		if (gScaleDC != NULL)
			SetDIBColorTable( gScaleDC, 0, 255, &BufferHeader.aColors[0]);
	}

}

/* ========================================================================
   Function    - load_pal
   Description - loads a palette and sets to the active one
   Returns     - void
   ======================================================================== */
void load_pal(char *n)
{
	FILE *f;

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
	COLORSPEC	TempPal[256];
	SHORT			i, j;

	if(fFadedOut == TRUE)
		return;
	
	fFadedOut = TRUE;
	fIsFadedOut = TRUE;
		
	steps /= 32;

	if (steps)											/* if steps is not zero */
	{
		/* step through intensity levels */
		for (i=steps; i>=0; i--)
		{
			for (j=0; j<256; j++)
			{
				TempPal[j].bRed	= (CurPal[j].bRed * i) / steps;
				TempPal[j].bGreen	= (CurPal[j].bGreen * i) / steps;
				TempPal[j].bBlue	= (CurPal[j].bBlue * i) / steps;
				run_timers();
			}
			set_pal((char *)TempPal);
		}
	}
	else
	{
		memset((PTR)TempPal, 0, 768);
		set_pal((char *)TempPal);
	}

}

/* ====================================================================
	FadeIn			- Fade from black to CurPal
	==================================================================== */
void FadeIn (USHORT steps)
{
	COLORSPEC	TempPal[256];
	USHORT		i, j;

	if(fFadedOut == FALSE)
		return;
		
	fFadedOut = FALSE;
	fIsFadedOut = FALSE;
	
	steps /= 32;

 if ( sDrawMode == iDDRAW )
		steps = 0;

	if (steps)											/* if steps is not zero */
	{
		for (i=0; i<=steps; i++)
		{
			for (j=0; j<256; j++)
			{
				TempPal[j].bRed	= (CurPal[j].bRed * i) / steps;
				TempPal[j].bGreen	= (CurPal[j].bGreen * i) / steps;
				TempPal[j].bBlue	= (CurPal[j].bBlue * i) / steps;
				run_timers();
			}
			set_pal((char *)TempPal);
		}
	}
	else
		set_pal((char *)CurPal);

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
	ReleaseDC(NULL, ScreenDC);

    if(gdcScreen)
    {
        hbm = (HBITMAP)SelectObject(gdcScreen, hOldMonoBitmap);

        DeleteObject(hbm);
        DeleteObject(gViewDC);
    }

	if (gScaleDC)
	{
		HDC dc = gScaleDC;

		// disarm the present path first, then free
		gScaleBuf = NULL;	// PresentGDIFrame falls back to direct stretch
		gScaleDC = NULL;
		hbm = (HBITMAP)SelectObject(dc, hOldScaleBitmap);
		DeleteObject(hbm);
		DeleteDC(dc);
	}
}

/* ========================================================================
   Function    - Scale2xFrame
   Description - AdvMAME2x/Scale2x: expand the game buffer 2x into
                 gScaleBuf, smoothing staircase edges while keeping the
                 paletted art crisp. Pure byte ops, no blending, so the
                 result stays within the current 256-color palette.
   ======================================================================== */
static void Scale2xFrame(LONG srcw, LONG srch)
{
	LONG x, y;

	for (y = 0; y < srch; y++)
	{
		const UCHAR *row  = (const UCHAR *)GDIscreen + y * MAX_VIEW_WIDTH;
		const UCHAR *up   = (y > 0)        ? row - MAX_VIEW_WIDTH : row;
		const UCHAR *down = (y < srch - 1) ? row + MAX_VIEW_WIDTH : row;
		UCHAR *out0 = gScaleBuf + (2*y) * (MAX_VIEW_WIDTH*2);
		UCHAR *out1 = out0 + (MAX_VIEW_WIDTH*2);

		for (x = 0; x < srcw; x++)
		{
			UCHAR P = row[x];
			UCHAR A = up[x];							/* above */
			UCHAR D = down[x];							/* below */
			UCHAR C = (x > 0)        ? row[x-1] : P;	/* left  */
			UCHAR B = (x < srcw - 1) ? row[x+1] : P;	/* right */
			UCHAR e0 = P, e1 = P, e2 = P, e3 = P;

			if (A != D && C != B)
			{
				if (C == A) e0 = A;
				if (A == B) e1 = B;
				if (C == D) e2 = C;
				if (B == D) e3 = D;
			}

			out0[2*x]   = e0;
			out0[2*x+1] = e1;
			out1[2*x]   = e2;
			out1[2*x+1] = e3;
		}
	}
}

/* ========================================================================
   Function    - PresentGDIFrame
   Description - Stretch the game buffer to the window client area.
                 Above 640x480 the frame goes through Scale2x first and
                 the remaining non-integer stretch uses HALFTONE
                 ("sharp bilinear"). This is the ONLY routine that may
                 paint the client area in GDI mode - partial updates
                 (ScreenCopy) clip and call it, so every painter produces
                 identical pixels and nothing shimmers.
   ======================================================================== */
// ponytail: Scale2x runs over the full frame even for clipped partial
// updates; scale only the dirty rect if profiling ever demands it.
static void PresentGDIFrame(void)
{
	RECT rc;

	GetClientRect(hwndApp, &rc);

	if (gScaleBuf != NULL && (rc.right > WindowWidth || rc.bottom > WindowHeight))
	{
		int OldMode;

		GdiFlush();		// DIB-section contract: flush GDI before reading bits
		Scale2xFrame(WindowWidth, WindowHeight);

		OldMode = SetStretchBltMode(gdcScreen, HALFTONE);
		SetBrushOrgEx(gdcScreen, 0, 0, NULL);
		StretchBlt( gdcScreen, 0, 0, rc.right, rc.bottom,
					gScaleDC, 0, 0, WindowWidth*2, WindowHeight*2, SRCCOPY);
		SetStretchBltMode(gdcScreen, OldMode);
	}
	else
	{
		StretchBlt( gdcScreen, 0, 0, rc.right, rc.bottom,
					gViewDC, 0, 0, WindowWidth, WindowHeight, SRCCOPY);
	}
}

/* ========================================================================
   Function    - update_screen
   Description - copies the display buffer into video ram
   Returns     - void
   ======================================================================== */
void update_screen(void)
{
	switch ( sDrawMode )
	{
	case iGDI:
		PresentGDIFrame();
		break;

	case iDDRAW:

		switch ( sDrawType )
		{
		case iDIRECTTOCARD:
			{
			HRESULT ddrval = DDUnlockSurface( (PCHAR)screen );
			if ( ddrval != DD_OK )
				fatal_error( "Unable to unlock surface: error = %d", ddrval );

			ddrval = DDFlipSurface();
			if ( ddrval != DD_OK )
				fatal_error( "Unable to flip surface: error = %d", ddrval );

			 // -- Make sure the DirectDraw thread has a moment to do stuff
			Sleep(0);
			}
			break;

		case iBLT:
		{
			HDC hdcDD = DDGetDC( TRUE );


			if ( hdcDD != NULL )
			{
   			DDRealizePalette();

            if ( fHighRes == TRUE )
            {
   				StretchBlt( hdcDD, 0, 0, 640, 480,
	   							gViewDC, 0, 0, WindowWidth, WindowHeight, SRCCOPY );
            }
            else
            {
   				StretchBlt( hdcDD, 0, 0, render_width*2, render_height*2,
	   							gViewDC, 0, 0, render_width, render_height, SRCCOPY );
            }


				DDReleaseDC( hdcDD );

				DDFlipSurface();

			}

		}
			break;

		case iCOPYTOCARD:
			DDDrawBuffer((PCHAR)screen );
			DDFlipSurface();
			break;

		default:
			break;
		}

		break;

	default:
		break;
	}
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
	if ( (sDrawMode == iGDI) || (sDrawType != iDIRECTTOCARD) )
		memset(screen, 0, MAX_VIEW_WIDTH * MAX_VIEW_HEIGHT);
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
	if ( (sDrawMode == iGDI) || (sDrawType != iDIRECTTOCARD) )
		memset(screen, c, MAX_VIEW_WIDTH * MAX_VIEW_HEIGHT);
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
	if ( (sDrawMode == iGDI) || (sDrawType != iDIRECTTOCARD) )
		memset(screen, 0, MAX_VIEW_WIDTH * MAX_VIEW_HEIGHT);
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

#if 1

	if ( sDrawMode == iGDI)
	{
		// Partial updates must produce bit-identical pixels to update_screen's
		// full-frame present, or areas repainted every frame (e.g. the map
		// ticker) shimmer against it at scaled window sizes. So clip to the
		// mapped target rect and redo the same full-frame present.
		RECT rc;
		LONG x0, y0, x1, y1;

		GetClientRect(hwndApp, &rc);

		// Inflate by 2 source pixels and round the far edge up: Scale2x
		// output depends on +/-1 source neighbors and HALFTONE spreads
		// about a pixel more, so a tight rect leaves stale halos around
		// per-frame partial repaints.
		x0 = (xSrc - 2) * rc.right  / WindowWidth;
		y0 = (ySrc - 2) * rc.bottom / WindowHeight;
		x1 = ((xSrc + width  + 2) * rc.right  + WindowWidth  - 1) / WindowWidth;
		y1 = ((ySrc + height + 2) * rc.bottom + WindowHeight - 1) / WindowHeight;

		IntersectClipRect(gdcScreen, x0, y0, x1, y1);
		PresentGDIFrame();
		SelectClipRgn(gdcScreen, NULL);
		GdiFlush();

   }


#endif
}




void DirectDrawSaveScreen( void )
{
	 // -- Get a valid background to save from!
	if ( sDrawMode == iDDRAW )
	{
		HDC hdcDD = NULL;

		if ( sDrawType == iDIRECTTOCARD )
		{
			DDUnlockSurface( (PCHAR) screen );
			screen = GDIscreen;

			 // -- copy previous screen to the current screen
			hdcDD = DDGetDC( FALSE );
			if ( hdcDD != NULL )
			{
				StretchBlt( gViewDC, 0, 0, WindowWidth, WindowHeight,
									hdcDD, 0, 0, 640, 480, SRCCOPY );
				DDReleaseDC( hdcDD );
			}

		}
	}
} // DirectDrawSaveScreen
