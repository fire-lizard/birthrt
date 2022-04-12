//
//              Copyright 1995/1996 by NW Synergistic Software, Inc.
//
//              DDraw.C  - handles all direct draw functionality.
//


#define CINTERFACE
#include <windows.h>

#include "../system.h"
#include "../engine.h"

#include "ddrawpro.h"

extern "C" {
extern HDC		gdcScreen;
}

extern "C" HRESULT CALLBACK DDAttachedCallback( LPDIRECTDRAWSURFACE lpDDSurface, 
                                       LPDDSURFACEDESC lpDDSurfaceDesc, 
														LPVOID lpContext );

extern "C"
{
typedef struct pal
{
  WORD Version;
  WORD NumberOfEntries;
  PALETTEENTRY aEntries[256];
} pal;
}
extern pal LogicalPalette;



 // -- local direct draw items
LPDIRECTDRAW            lpDD                    = NULL;   // DirectDraw object
LPDIRECTDRAWSURFACE     lpDDSPrimary    = NULL;   // DirectDraw primary surface
LPDIRECTDRAWSURFACE     lpDDSBack1              = NULL;   // DirectDraw back surface
LPDIRECTDRAWSURFACE     lpDDSBack2              = NULL;   // DirectDraw GDI surface
LPDIRECTDRAWPALETTE     lpDDPPalette    = NULL;   // DirectDraw palette

static BOOL    fUsingGDI       = FALSE;
static HWND    hwndAppDD       = NULL;
static DWORD   dwFillColor     = 0;
static HRESULT ddrvalG         = 0;
static USHORT  usNum           = 0;
static HDC		hdcSurface		 = NULL;



extern "C" HRESULT CALLBACK DDAttachedCallback( 
	LPDIRECTDRAWSURFACE lpDDSurface, 
    LPDDSURFACEDESC /* lpDDSurfaceDesc */, 
    LPVOID /* lpContext */)
{
   if ( NULL == lpDDSBack1 )
	   lpDDSBack1 = lpDDSurface;
   else
      lpDDSBack2 = lpDDSurface;

   return ( DDENUMRET_OK );
} // DDAttachedCallback




//--------------------------------------------------------------------------
//
//  void DDCleanup
//
//  Description:
//              Clean up all DirectDraw items.
//
//  Arguments:
//              none
//
//  Return none
//
//--------------------------------------------------------------------------
extern "C" void DDCleanup( void )
{
         // -- make sure GDI surface is in front
   if ( lpDD != NULL )
   {
		IDirectDraw_RestoreDisplayMode( lpDD );
      DDSetCooperative( FALSE );
      IDirectDraw_FlipToGDISurface( lpDD );
   }

    // -- release all DD buffers with the front buffer
   if( lpDDSPrimary != NULL )
		IDirectDrawSurface_Release( lpDDSPrimary );
       
   if( lpDDSBack2 != NULL )
      IDirectDrawSurface_Release( lpDDSBack2 );
       
      
        // -- release the palette
   if( lpDDPPalette != NULL )
      IDirectDrawPalette_Release( lpDDPPalette );

        // -- release DirectDraw itself        
   if( lpDD != NULL )
      IDirectDraw_Release( lpDD );

    // -- mark the cleanup effort
   lpDDSPrimary = NULL;
   lpDDSBack1 = NULL;
   lpDDSBack2 = NULL;

   lpDDPPalette = NULL;

   lpDD = NULL;

} // DDCleanup




//--------------------------------------------------------------------------
//
//  void DDDrawBuffer
//
//  Description:
//              Draw the buffer to the DirectDraw surface and display.
//
//  Arguments:
//              pchBuf          - buffer to display
//
//  Return none
//
//--------------------------------------------------------------------------
extern "C" void DDDrawBuffer ( PCHAR pchBuf )
{
	int i;
   HRESULT ddrval;
   DDSURFACEDESC ddsd;
   RECT src;

   ddsd.dwSize = sizeof(DDSURFACEDESC);
                                         
    // -- we have an error here
   if ( NULL == lpDDSBack1 )
           return;

   DDEraseScreen();

   if ( fUsingGDI &&  
                   (lpDDSBack2 != NULL) )
   {
		
      ddrval = IDirectDrawSurface_GetSurfaceDesc( lpDDSBack2, &ddsd );
		                               
      src.left = 0;
      src.top = 0;
      src.right = ddsd.dwWidth;
      src.bottom = ddsd.dwHeight;

       // -- Blt the GDI from the back to the middle buffer
      ddrval = IDirectDrawSurface_BltFast( lpDDSBack1, 0, 0, lpDDSBack2, &src, 
                                       DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT );
   }

	 // -- Do the buffer part
   ddrval = DDLockSurface( (PCHAR *)&ddsd.lpSurface, &ddsd.lPitch );

	if ( ddrval == DD_OK )
   {
		PCHAR lpS = (PCHAR) ddsd.lpSurface;
      PCHAR lpB = pchBuf;

        // -- copy buffer to back surface
      for ( i = 0; i < MAX_VIEW_HEIGHT; i++ )
      {
		 	memcpy( lpS, lpB, MAX_VIEW_WIDTH );
         lpS += ddsd.lPitch;
         lpB += MAX_VIEW_WIDTH;
      }
	   DDUnlockSurface( (PCHAR) ddsd.lpSurface );
   }

} // DDDrawBuffer




//--------------------------------------------------------------------------
//
//  void DDEraseScreen
//
//  Description:
//              Erase the DirectDraw screen in preparation for drawing.
//
//  Arguments:
//              none
//
//  Return none
//
//--------------------------------------------------------------------------
extern "C" void DDEraseScreen ( void )
{
    DDBLTFX     ddbltfx;
    HRESULT     ddrval;

        if ( NULL == lpDDSBack1 )
                return;
        
    // Erase the background
    ddbltfx.dwSize = sizeof( ddbltfx );
    ddbltfx.dwFillColor = dwFillColor;

    while ( TRUE )
    {
      ddrval = IDirectDrawSurface_Blt( lpDDSBack1, NULL, NULL,
                               NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx );

      if( ddrval == DD_OK )
      {
			break;
      }
		if( ddrval == DDERR_SURFACELOST )
      {
			if( !DDRestoreSurfaces() )
				return;
      }
      if( ddrval != DDERR_WASSTILLDRAWING )
      {
			return;
      }
    } // while TRUE

} // DDEraseScreen




//--------------------------------------------------------------------------
//
//  void DDFlipScreen
//
//  Description:
//              Flip the DirectDraw screen.
//
//  Arguments:
//              none
//
//  Return none
//
//--------------------------------------------------------------------------
extern "C" HRESULT DDFlipScreen( LPDIRECTDRAWSURFACE lpDDSFlip )
{
   HRESULT     ddrval;

   if ( NULL == lpDDSPrimary )
		return ( DDERR_GENERIC );

    // Flip the surfaces
   while ( TRUE )
   {
		ddrval = IDirectDrawSurface_Flip( lpDDSPrimary, lpDDSFlip, DDFLIP_WAIT );

      if ( ddrval == DD_OK )
      {
			break;
      }
      if ( ddrval == DDERR_SURFACELOST )
      {
			if( !DDRestoreSurfaces() )
         {
				break;
         }
      }
	   if ( ddrval != DDERR_WASSTILLDRAWING )
      {
		   break;
      }
	} // while TRUE

	return ( ddrval );

} // DDFlipScreen




//--------------------------------------------------------------------------
//
//  void DDFlipSurface
//
//  Description:
//              Flip the DirectDraw screen.
//
//  Arguments:
//              none
//
//  Return none
//
//--------------------------------------------------------------------------
extern "C" HRESULT DDFlipSurface( void )
{
	LPDIRECTDRAWSURFACE lpDDSFlip = NULL;

   if ( fUsingGDI )
		lpDDSFlip = lpDDSBack1;

   return ( DDFlipScreen( lpDDSFlip ) );

} // DDFlipSurface




//--------------------------------------------------------------------------
//
//  BOOL DDGetDC
//
//  Description:
//		Return an HDC for the surface.
//
//  Arguments:
//		fBack		- if TRUE, get the back buffer, else the front
//
//  Return an HDC for the surface
//
//--------------------------------------------------------------------------
extern "C" HDC DDGetDC( BOOL fBack )
{
	ddrvalG = DD_OK + 1;
	if ( fBack )
	{
		if ( lpDDSBack1 != NULL )
		{
			ddrvalG = IDirectDrawSurface_GetDC( lpDDSBack1, &hdcSurface );
			if ( ddrvalG == DDERR_SURFACELOST )
			{
			   if( DDRestoreSurfaces() )
				{
					ddrvalG = IDirectDrawSurface_GetDC( lpDDSBack1, &hdcSurface );
				}
			}
		}
	}
	else
	{
		if ( lpDDSPrimary != NULL )
		{
			ddrvalG = IDirectDrawSurface_GetDC( lpDDSPrimary, &hdcSurface );
			if ( ddrvalG == DDERR_SURFACELOST )
			{
			   if( DDRestoreSurfaces() )
				{
					ddrvalG = IDirectDrawSurface_GetDC( lpDDSPrimary, &hdcSurface );
				}
			}
		}
	}

	if ( ddrvalG == DD_OK )
		return ( hdcSurface );
	else
		return ( NULL );
} // DDGetDc




//--------------------------------------------------------------------------
//
//  BOOL DDGetSurfaceWidth
//
//  Description:
//		Return the surface width of the primary surface.
//
//  Arguments:
//		none
//
//  Return surface width
//
//--------------------------------------------------------------------------
extern "C" DWORD DDGetSurfaceWidth( void )
{
	DDSURFACEDESC dds;

	IDirectDrawSurface_GetSurfaceDesc( lpDDSPrimary, &dds );

	return ( dds.dwWidth );
} // DDgetSurfaceWidth




//--------------------------------------------------------------------------
//
//  BOOL DDInitialize
//
//  Description:
//              Initialize DirectDraw.
//
//  Arguments:
//              hwndMain        - handle of game window
//              fGDI            - do we need GDI also
//
//  Return TRUE if DirectDraw was initialized, else FALSE
//
//--------------------------------------------------------------------------
extern "C" BOOL DDInitialize ( HWND hwndMain, BOOL fGDI )
{
	BOOL fOK = TRUE;

   DDSURFACEDESC   ddsd;
   DDSCAPS         ddscaps;
	int rval;
        
    // -- Save the requested state information
	fUsingGDI = fGDI;
   hwndAppDD = hwndMain;

//        // see if we are in 256 color mode
//        rval = GetDeviceCaps(gdcScreen, NUMCOLORS);
//        if(rval == -1 || rval == 32767)
//                return FALSE;

    // --
    // -- Create the Direct Draw object
    // --
   usNum = 0;
//	printf("before DirectDrawCreate( NULL, &lpDD, NULL )");
   ddrvalG = DirectDrawCreate( NULL, &lpDD, NULL );
//	printf("after DirectDrawCreate( NULL, &lpDD, NULL )");

   if( ddrvalG != DD_OK )
   {
		fOK = FALSE;
      DDCleanup();
      goto DDInitializeExit;
   }


    // --
    // -- Set the cooperative level
    // --
   usNum = 2;
	ddrvalG = DDSetCooperative( TRUE );

   if( ddrvalG != DD_OK )
   {
		fOK = FALSE;
		DDCleanup();
      goto DDInitializeExit;
   }


    // --
    // -- Set the mode to 640 by 480 by 8
    // --
   usNum = 3;
//	printf("before DDSetDisplayMode");
   ddrvalG = DDSetDisplayMode( TRUE ); 
//	printf("after DDSetDisplayMode");
   if( ddrvalG != DD_OK )
   {
      fOK = FALSE;
      DDCleanup();
      goto DDInitializeExit;
   }


    // --
    // -- Create the front and back surfaces
    // --
   memset( &ddsd, 0, sizeof( ddsd ) );
   ddsd.dwSize = sizeof( ddsd );
   ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
   ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
                          DDSCAPS_FLIP |
                          DDSCAPS_COMPLEX;
   ddsd.dwBackBufferCount = fGDI ? 2 : 1;

   usNum = 4;
//	printf("before IDirectDraw_CreateSurface");
   ddrvalG = IDirectDraw_CreateSurface( lpDD, &ddsd, &lpDDSPrimary, NULL );
//	printf("after IDirectDraw_CreateSurface");
   if( ddrvalG != DD_OK )
   {
		fOK = FALSE;
      DDCleanup();
      goto DDInitializeExit;
   }


    // --
    // -- Get a pointer to the back buffer(s)
    // --
   usNum = 5;
   if ( !fGDI )
   {
      ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
      ddrvalG = IDirectDrawSurface_GetAttachedSurface( lpDDSPrimary,
                                                                                                        &ddscaps, &lpDDSBack1 );
		if( ddrvalG != DD_OK )
      {
			fOK = FALSE;
         DDCleanup();
         goto DDInitializeExit;
      }
   }
   else 
   {
		ddrvalG = IDirectDrawSurface_EnumAttachedSurfaces( lpDDSPrimary, NULL, 
                                                                                                                                DDAttachedCallback );

//              usNum = 6;
//          ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
//              ddsd.ddsCaps.dwCaps = DDSCAPS_FLIP;
//          ddsd.dwBackBufferCount = 1;
//          ddrvalG = IDirectDraw_CreateSurface( lpDD, &ddsd, &lpDDSBack2, NULL );
      if( ddrvalG != DD_OK )
      {
	      fOK = FALSE;
         DDCleanup();
         goto DDInitializeExit;
		}

//              usNum = 7;
//              ddrvalG = IDirectDrawSurface_AddAttachedSurface( lpDDSPrimary, lpDDSBack2 );
//          if( ddrvalG != DD_OK )
//              {
//                      fOK = FALSE;
//              DDCleanup();
//                      goto DDInitializeExit;
//              }
	}


    // --
    // -- Deal with Blting the GDI
    // --
   if ( fGDI )
   {
		DDSURFACEDESC ddsdesc;
      RECT src;

      ddsdesc.dwSize = sizeof( ddsdesc );
      usNum = 10;
      ddrvalG = IDirectDrawSurface_GetSurfaceDesc( lpDDSPrimary, &ddsdesc );
                              
      src.left = 0;
      src.top = 0;
      src.right = ddsdesc.dwWidth;
      src.bottom = ddsdesc.dwHeight;

       // -- Blt the GDI from the front to the middle buffer
      usNum = 7;
      ddrvalG = IDirectDrawSurface_BltFast( lpDDSBack1, 0, 0, lpDDSPrimary, &src, 
                                                                              DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT );

       // -- Flip to make the GDI copy the backmost buffer
      DDFlipScreen( NULL );
   }

    // -- Get everything setup for sure
   usNum = 11;
   if( !DDRestoreSurfaces() )
   {
		fOK = FALSE;
      DDCleanup();
      goto DDInitializeExit;
   }


    // --
    // -- Cleanup and leave
    // --
	DDInitializeExit:

   return ( fOK );
} // DDInitialize




//--------------------------------------------------------------------------
//
//  HRESULT DDLockSurface
//
//  Description:
//              Attempt to lock the DirectDraw surface memory.
//
//  Arguments:
//              ppchBuf         - pointer to address of screen buffer
//              pViewWidth      - return the pitch of the surface memory
//
//  Return HRESULT from DirectDraw
//
//--------------------------------------------------------------------------
extern "C" HRESULT DDLockSurface( PCHAR *ppchBuf, long *pViewWidth )
{
   DDSURFACEDESC ddsdesc;
   HRESULT ddrval = DD_OK;

    // -- pre-initialize stuff
   ddsdesc.dwSize = sizeof(DDSURFACEDESC);
   (*ppchBuf) = NULL;

   if ( NULL == lpDDSBack1 )
		return ( DDERR_SURFACELOST ); 


    // -- lock the surface drawing area
	while ( TRUE )
   {
      ddrval = IDirectDrawSurface_Lock( lpDDSBack1, NULL, &ddsdesc, 
					                  DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL );

      if( ddrval == DD_OK )
      {
			break;
      }
      if( ddrval == DDERR_SURFACELOST )
      {
         if( !DDRestoreSurfaces() )
				break;
      }
      if ( ddrval != DDERR_WASSTILLDRAWING )
      {
			break;
      }

		if ( GetAsyncKeyState( VK_F4 ) & 0x8000 )
			break;
	} // while TRUE

   if ( DD_OK == ddrval )
   {
		(*ppchBuf) = (PCHAR) ddsdesc.lpSurface;
      (*pViewWidth) = ddsdesc.lPitch;
   }

	return ( ddrval );
} // DDLockSurface




//--------------------------------------------------------------------------
//
//  BOOL DDRealizePalette
//
//  Description:
//              Deal with the new palette,
//
//  Arguments:
//              none
//
//  Return none
//
//--------------------------------------------------------------------------
extern "C" BOOL DDRealizePalette( void )
{
	HRESULT ddrval;

   if ( lpDD == NULL )
       return TRUE;

   if( lpDDPPalette != NULL )
	{

		// release the previous palette
      IDirectDrawPalette_Release( lpDDPPalette );

		lpDDPPalette = NULL;
	}


    // -- Create and set the palette 
   ddrval = IDirectDraw_CreatePalette( lpDD, 
	   									DDPCAPS_8BIT | DDPCAPS_ALLOW256, 
	   									&LogicalPalette.aEntries[0],
	   									&lpDDPPalette,
	    								NULL );

   if ( ddrval != DD_OK )
       return ( FALSE );

	 // -- set the primary buffer 
	IDirectDrawSurface_SetPalette( lpDDSPrimary, lpDDPPalette );

	return ( TRUE );

} // DDRealizePalette




//--------------------------------------------------------------------------
//
//  BOOL DDSetPaletteEntries
//
//  Description:
//
//       Set palette entries.
//
//       Don't ask why the following works but it does 
// 
//  Arguments:
//              none
//
//  Return none
//
//--------------------------------------------------------------------------
extern "C" BOOL DDSetPaletteEntries( void )
{
	HRESULT ddrval;

   if ( lpDD == NULL )
       return TRUE;

   if( lpDDPPalette != NULL )
	{
		// release the primary surface palette
		IDirectDrawSurface_SetPalette( lpDDSPrimary, NULL );

		IDirectDrawPalette_SetEntries( lpDDPPalette, 0, 0, 
												 255, &LogicalPalette.aEntries[0] );

	}

   if ( ddrval != DD_OK )
       return ( FALSE );

	// -- set the back buffer to avoid the palette flash
	IDirectDrawSurface_SetPalette( lpDDSBack1, lpDDPPalette );

	return ( TRUE );

} // DDRealizePalette




//--------------------------------------------------------------------------
//
//  BOOL DDReleaseDC
//
//  Description:
//		Release the HDC for the surface.
//
//  Arguments:
//		none
//
//  Returns: none
//
//--------------------------------------------------------------------------
extern "C" void DDReleaseDC( HDC hdc )
{
	IDirectDrawSurface_ReleaseDC( lpDDSBack1, hdc );
} // DDReleaseDC




//--------------------------------------------------------------------------
//
//  BOOL DDRestoreSurfaces
//
//  Description:
//              Attempt to restore the DirectDraw surfaces.
//
//  Arguments:
//              none
//
//  Return TRUE if surfaces could be restored, else FALSE
//
//--------------------------------------------------------------------------
extern "C" BOOL DDRestoreSurfaces( void )
{
        HRESULT     ddrval;

    ddrval = IDirectDrawSurface_Restore( lpDDSPrimary );
    if ( ddrval != DD_OK )
        return ( FALSE );

	 return ( DDRealizePalette() );

    // set colorfill colors and color keys according to bitmap contents

        // -- CLR_INVALID will make palette 0's entry the transparent one
//    dwFillColor = DDColorMatch( lpScreen, CLR_INVALID );
//    
//    return ( TRUE );
} // DDRestoreSurfaces




//--------------------------------------------------------------------------
//
//  HRESULT DDSetCooperative
//
//  Description:
//              Set the cooperative level.
//
//  Arguments:
//              fExclusive	- TRUE is exclusive mode desired
//
//  Return HRESULT from DirectDraw
//
//--------------------------------------------------------------------------
HRESULT DDSetCooperative( BOOL fExclusive )
{
	HRESULT ddrval = DD_OK + 1;

//	if ( fUsingGDI )
//	{
//		ddrval = IDirectDraw_SetCooperativeLevel( lpDD, hwndAppDD,
//												DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
//	}
//	else
//	{
//		printf("before IDirectDraw_SetCooperativeLevel");

       if ( lpDD == NULL )
          return ( ddrval );

		if ( fExclusive )
		{
	      ddrval = IDirectDraw_SetCooperativeLevel( lpDD, hwndAppDD,
                       DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT );
		}
		else
		{
	      ddrval = IDirectDraw_SetCooperativeLevel( lpDD, hwndAppDD, DDSCL_NORMAL );
		}
//		printf("after IDirectDraw_SetCooperativeLevel");
//	}

	return ( ddrval );
} // DDSetCooperative




//--------------------------------------------------------------------------
//
//  HRESULT DDSetDisplayMode
//
//  Description:
//              Set the display mode.
//
//  Arguments:
//              fHigh   - if true, set high-res, else low-res
//
//  Return HRESULT from DirectDraw
//
//--------------------------------------------------------------------------
extern "C" HRESULT DDSetDisplayMode( BOOL fHigh )
{
   HRESULT hRes = DD_OK;

	if ( fHigh )
   {
		hRes =  IDirectDraw_SetDisplayMode( lpDD, 640, 480, 16 );

       if ( hRes != DD_OK )
       {
   		hRes =  IDirectDraw_SetDisplayMode( lpDD, 640, 480, 8 );
       }

		return hRes;

   }
	else
		return ( IDirectDraw_SetDisplayMode( lpDD, 320, 200, 8 ) );

} // DDSetDisplayMode




//--------------------------------------------------------------------------
//
//  HRESULT DDUlockSurface
//
//  Description:
//              Attempt to unlock the DirectDraw surface memory.
//
//  Arguments:
//              pchBuf          - address of screen buffer
//
//  Return HRESULT from DirectDraw
//
//--------------------------------------------------------------------------
extern "C" HRESULT DDUnlockSurface( PCHAR pchBuf )
{
	HRESULT ddrVal = DD_OK;

	while ( TRUE )
	{
		 // -- unlock the drawing surface
		ddrVal = IDirectDrawSurface_Unlock( lpDDSBack1, pchBuf );

		if ( ddrVal == DD_OK )
		{
			break;
		}

		if ( ddrVal == DDERR_SURFACELOST )
	   {
	      if( !DDRestoreSurfaces() )
			{
				break;
			}
	   }

		if( ddrVal != DDERR_WASSTILLDRAWING )
      {
			break;
      }
	} // while TRUE

	return ( ddrVal );
} // DDUnlockSurface

// DDraw.cpp
