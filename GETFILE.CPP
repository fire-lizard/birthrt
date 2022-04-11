/* ========================================================================
   Copyright (c) 1997	Synergistic Software
   All Rights Reserved
   ========================================================================
	Get a file name using the WINDOWS common dialog.
   Filename: GETFILE.CPP
   Author:	 John Conley
   ======================================================================== */

#if defined (_WINDOWS)						// Windows is required for this function
#include <windows.h>
#include "getfile.hxx"


// ---------------------------------------------------------------------------
BOOL GetFileName (HWND i_hwnd, LPCSTR i_szDirName, LPCSTR i_szFilter,
	LPCSTR i_szExt, LPSTR o_szReturnName, LPSTR o_szReturnTitle,
	DWORD i_dwflags, int openOrSave)
{
// Use the common dialog to get a file name.
// i_ prefix means input-only parameter.
// o_ prefix means output-only parameter.
// io_ prefix means input/output parameter.
//
// Example of use:
//
// 	char 		szFilename[128];
// 	char 		szTitle[128];
//
//
// 	szFilename[0] = 0;
// 	szTitle[0] = 0;
//
// 	if (GetFileName (hwnd, NULL, "BMP Files (*.BMP)\0*.BMP", "BMP",
// 			szFilename, szTitle, OFN_PATHMUSTEXIST, GFN_OPEN))
// 	{
// 		hDIB = GetDIB (szFilename);
// 		if (hDIB)
// 		{
// 			return hDIB;
// 		}
// 	}
//
// ---------------------------------------------------------------------------

   OPENFILENAME   ofn;
   BOOL           retval;


   // Set all structure members to zero.
   memset(&ofn, 0, sizeof(OPENFILENAME));
   o_szReturnName[0]  = 0;             // make sure buffer is empty
   o_szReturnTitle[0] = 0;             // ...or GetOpenFileName will fail.

   ofn.lStructSize = sizeof(OPENFILENAME);
   ofn.hwndOwner = i_hwnd;
   ofn.lpstrFilter = i_szFilter;
   ofn.nFilterIndex = 1;
   ofn.lpstrFile= o_szReturnName;
   ofn.nMaxFile = _MAX_PATH;           // not sizeof(o_szReturnName);
   ofn.lpstrFileTitle = o_szReturnTitle;
   ofn.nMaxFileTitle = _MAX_PATH;      // not sizeof(o_szReturnTitle);
   ofn.lpstrInitialDir = i_szDirName;
   ofn.Flags = /* OFN_SHOWHELP | */ OFN_HIDEREADONLY | i_dwflags;
   ofn.lpstrDefExt = i_szExt;

   if (openOrSave == GFN_OPEN)
      retval = GetOpenFileName(&ofn);
   else
      retval = GetSaveFileName(&ofn);

   return retval;
}


#endif

