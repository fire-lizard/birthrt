/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _DDRAWPRO_H
#define _DDRAWPRO_H

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include "../SYSTEM.H"

#if defined(_WIN32)
#include <ddraw.h>
#endif
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

/* The POSIX build has no DirectDraw at all; only the three entry points
   the portable core actually calls are declared, and POSIX/STUBS.CPP
   answers them with "not available". The rest take DirectDraw surface
   types that do not exist off Windows. */
void    DDCleanup               ( void );
BOOL    DDInitialize            ( HWND hwndMain, BOOL fGDI );
HRESULT DDSetCooperative		  ( BOOL fExclusive );

#if defined(_WIN32)
void    DDDrawBuffer            ( PCHAR pchBuf );
void    DDEraseScreen           ( void );
HRESULT DDFlipScreen            ( LPDIRECTDRAWSURFACE lpDDSFlip );
HRESULT DDFlipSurface           ( void );
HDC	  DDGetDC					  ( BOOL fBack );
DWORD	  DDGetSurfaceWidth		  ( void );
HRESULT DDLockSurface           ( PCHAR *ppchBuf, long *pViewWidth );
BOOL	  DDRealizePalette		  ( void );
BOOL	  DDSetPaletteEntries	  ( void );
void	  DDReleaseDC				  ( HDC hdc );
BOOL    DDRestoreSurfaces       ( void );
HRESULT DDSetDisplayMode        ( BOOL fHigh );
HRESULT DDUnlockSurface         ( PCHAR pchBuf );
#endif

#if defined (__cplusplus)
}
#endif
#endif // _DDRAWPRO_H
