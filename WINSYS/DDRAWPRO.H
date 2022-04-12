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

#include <ddraw.h>
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

void    DDCleanup               ( void );
void    DDDrawBuffer            ( PCHAR pchBuf );
void    DDEraseScreen           ( void );
HRESULT DDFlipScreen            ( LPDIRECTDRAWSURFACE lpDDSFlip );
HRESULT DDFlipSurface           ( void );
HDC	  DDGetDC					  ( BOOL fBack );
DWORD	  DDGetSurfaceWidth		  ( void );
BOOL    DDInitialize            ( HWND hwndMain, BOOL fGDI );
HRESULT DDLockSurface           ( PCHAR *ppchBuf, long *pViewWidth );
BOOL	  DDRealizePalette		  ( void );
BOOL	  DDSetPaletteEntries	  ( void );
void	  DDReleaseDC				  ( HDC hdc );
BOOL    DDRestoreSurfaces       ( void );
HRESULT DDSetCooperative		  ( BOOL fExclusive );
HRESULT DDSetDisplayMode        ( BOOL fHigh );
HRESULT DDUnlockSurface         ( PCHAR pchBuf );

#if defined (__cplusplus)
}
#endif
#endif // _DDRAWPRO_H
