//-----------------------------------------------------------------------------
//
//		Copyright 1994/1995 by NW Synergistic Software, Inc.
//
//		Debug.C - Provides debug console routines.
//
//------------------------------------------------------------------------------
//

#include <stdio.h>

#ifdef _WINDOWS
#include "WINSYS/STATUS.H"
#include "PLATFORM.H"
#include <fcntl.h>
#include <stdarg.h>
//GEH not availible in Watcom #include <crtdbg.h>        // JPC

// GWP The macro defines in debug.h toast the compile.
#include "SYSTEM.H"
#include "MACHINE.H"

//---- global only used by this module

static int iDebug = FALSE;

extern HWND ghwndDebug;

//----------------------------------------------------------------------------
//  debugf()
//
//  Description:
//
//		printf text to the debug console.
//
//  Arguments: the same as printf
//
//             format - ptr to format string
//             vars   - parameters for format string
//
//  Return : int - same as printf; num bytes or negative if error
//
//
//----------------------------------------------------------------------------
int debugf ( const char *format, ... );
int debugf ( const char *format, ... )
{
    if ( iDebug == TRUE )
    {
        int     iReturn;
        va_list ptr;

        va_start ( ptr, format );

        iReturn = vprintf ( format, ptr );

        va_end(ptr);

        printf ( "\n" );

        return iReturn;
    }

    return(0);

}   //---- End of debugf()

#if defined(_EDIT)
// ---------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
#endif
void ErrorMessage (const char *format, ...);
#ifdef __cplusplus
extern "C"
#endif
void ErrorMessage (const char *format, ...)
{
// [d5-14-96 JPC]
// Print an informational message, not a fatal error.
// User's only choice is to press OK.
// ---------------------------------------------------------------------------

	char 			szTemp[256];
	va_list		argp;

	va_start (argp, format);
	vsprintf (szTemp, format, argp);
	va_end (argp);
	MessageBox (NULL, szTemp, "Error", MB_OK);
}
#endif

#endif   // _WINDOWS
// Debug.c
