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
#include "WINSYS\STATUS.H"
#include "Windows.h"
#include <io.h>
#include <fcntl.h>
#include <stdarg.h>
//GEH not availible in Watcom #include <crtdbg.h>        // JPC

// GWP The macro defines in debug.h toast the compile.
//#include "debug.h"
#include "SYSTEM.H"
#include "MACHINE.H"

//---- global only used by this module

static int iDebug = FALSE;

extern HWND ghwndDebug;

//----------------------------------------------------------------------------
//  InitDebug()
//
//  Description:
//
//		Create the debug console.
//
//  Arguments: None
//
//
//  Return :
//
//
//----------------------------------------------------------------------------
void InitDebug ( void );
void InitDebug ( void )
{

//    int hCrt;
//    FILE *hf;
//    int i;

   //---- Allow printf etc to work

     AllocConsole();

//   hCrt = _open_osfhandle( (long) GetStdHandle(STD_OUTPUT_HANDLE),
//                           _O_TEXT );

//   hf = _fdopen( hCrt, "w" );

//   *stdout = *hf;

//   i = setvbuf( stdout, NULL, _IONBF, 0 );

   iDebug = TRUE;


}   //---- InitDebug ()



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

// ---------------------------------------------------------------------------
#if 1
// GWP PLEASE DON'T OVERWRITE THIS FN! [ssprintf]
// JPC I can't find a function called ssprintf (as opposed to sprintf),
// but I'll rename this one to SS_printf (SS for Synergistic Software)
// to avoid confusion.
void SS_printf (const char *format, ... );
void SS_printf (const char *format, ... )
{
// [d4-23-96 JPC]
// Synergistic Software printf.
// In Windows, prints to the debug output device.
// ---------------------------------------------------------------------------

   char 			szTemp[256];
	va_list 		argp;

	va_start (argp, format);
	vsprintf(szTemp, format, argp);
	va_end (argp);
   OutputDebugString (szTemp);
   OutputDebugString ("\r");           // [d6-05-96 JPC]
}
#endif

// ---------------------------------------------------------------------------
void ErrorMessage (const char *format, ...);
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


// ---------------------------------------------------------------------------
void SS_Assert (char * szSourceFile, int iLine)
{
// [d6-17-96 JPC]
// Put up a message box with the assert in it, then abort.
// This is for Watcom, which does not seem to have a nice assert system for
// Windows--it seems DOS-specific (but maybe I just haven't found it yet).
// ---------------------------------------------------------------------------

	int			answer;
	char			szError[128];

	sprintf (szError, "Assert failed in %s, line %d\n\nEnd the program?", szSourceFile, iLine);
	answer = MessageBox (NULL, szError, "Assertion failure!", MB_YESNO);
	if (answer == IDYES)
		fatal_error (szError);
}

// ---------------------------------------------------------------------------
#else                                  // not _WINDOWS
// ---------------------------------------------------------------------------
void ErrorMessage (const char *format, ...);
void ErrorMessage (const char *format, ...)
{
// [d5-14-96 JPC]
// Print an informational message, not a fatal error.
// ---------------------------------------------------------------------------

   char 		szTemp[256];
	va_list	argp;

	va_start (argp, format);
	vsprintf (szTemp, format, argp);
	va_end (argp);
   printf (szTemp);
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#endif   // _WINDOWS
// Debug.c
