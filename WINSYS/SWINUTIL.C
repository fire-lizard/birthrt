//-----------------------------------------------------------------------------
//
//        Copyright 1995/1996 by NW Synergistic Software, Inc.
//
//        SWinUtil.c - Provides interface for playing wave files.
//
//------------------------------------------------------------------------------

#include <windows.h>

#include "SWinUtil.h"




//--------------------------------------------------------------------------;
//
//  void CenterInWindow( hwnd )
//
//  Description:
//		Center the given window in the basis window.
//
//  Arguments:
//		hwnd		- window handle to center
//		hwndBasis	- window to center on
//		fRelative	- if true, window coords are relative to the basis hwnd
//
//  Return (void):
//
//--------------------------------------------------------------------------;
void CenterInWindow( HWND hwnd, HWND hwndBasis, BOOL fRelative )
{
	USHORT usX, usY, BW, BH;
	RECT rcWin, rcBasis;

	GetWindowRect( hwnd, &rcWin );
	if ( fRelative )
		GetClientRect( hwndBasis, &rcBasis );
	else
		GetWindowRect( hwndBasis, &rcBasis );

	 // -- compute basis parameters
	BW = (USHORT) (rcBasis.right - rcBasis.left);
	BH = (USHORT) (rcBasis.bottom - rcBasis.top);

	usX = (BW - (rcWin.right - rcWin.left)) / 2;
	usY = (BH - (rcWin.bottom - rcWin.top)) / 2;
	if ( !fRelative )
	{
		usX += (USHORT) rcBasis.left;
		usY += (USHORT) rcBasis.top;
	}

	MoveWindow( hwnd, usX, usY, (rcWin.right - rcWin.left), 
					(rcWin.bottom - rcWin.top), TRUE );

} // CenterInWindow




//--------------------------------------------------------------------------;
//
//  void CenterWindow( hwnd )
//
//  Description:
//		Center the given window on the screen.
//
//  Arguments:
//		hwnd		- window handle
//
//  Return (void):
//
//--------------------------------------------------------------------------;
void CenterWindow( HWND hwnd )
{
	int fx = GetSystemMetrics(SM_CXFULLSCREEN);
	int fy = GetSystemMetrics(SM_CYFULLSCREEN);
	USHORT usX, usY;
	RECT rcWin;

	GetWindowRect( hwnd, &rcWin );

	usX = (fx - (rcWin.right - rcWin.left)) / 2;
	usY = (fy - (rcWin.bottom - rcWin.top)) / 2;

	MoveWindow( hwnd, usX, usY, (rcWin.right - rcWin.left), 
					(rcWin.bottom - rcWin.top), TRUE );

} // CenterWindow




//--------------------------------------------------------------------------;
//
//  BOOL DoesCopyProtectionFail( ... )
//
//  Description:
//		Check for a valid CD in the drive.
//
//  Arguments:
//		pszDir		- directory to check
//		pszVolName	- correct volume name
//
//  Return (void):
//
//--------------------------------------------------------------------------;
BOOL DoesCopyProtectFail( PSZ pszDir, PSZ pszVolName )
{
	char szVolName[25] = "";
	char szFileType[25] = "";
	DWORD dwTemp;
	BOOL bRetVal = FALSE;	// assume no failure

	// for now allow a bypass
//	if(fSkipCheck)
//		return(FALSE);

	if(GetDriveType(pszDir) != DRIVE_CDROM)
		bRetVal |= TRUE;

	GetVolumeInformation(pszDir, (LPTSTR)&szVolName, sizeof(szVolName),
						NULL, &dwTemp, &dwTemp,
						(LPTSTR)&szFileType, sizeof(szFileType));

	CharUpperBuff(szVolName, 25);

	if(strcmp(szVolName, pszVolName) != 0)
		bRetVal |= TRUE;

	return(bRetVal);
} // DoesCopyProtectionFail




//--------------------------------------------------------------------------
//
//  void MakeQualifiedFilename
//
//  Description:
//		Make a fully qualified filename from the given parts.
//
//  Arguments:
//		pszName		- resultant filename
//		pszDir		- directory	(can be NULL)
//		pszSubDir	- sub-directory	(can be NULL)
//		pszFileName	- filename	(can be NULL)
//		pszExt		- file extension	(can be NULL)
//
//  Return (void):
//
//--------------------------------------------------------------------------
void MakeQualifiedFilename( PSZ pszName, PSZ pszDir, PSZ pszSubDir, 
												PSZ pszFileName, PSZ pszExt )
{
	 // -- Clear the result first
	pszName[0] = 0;

	 // -- make the initial directory
	if ( strlen(pszDir) != 0 )
	{
		strcpy ( pszName, pszDir );
		if ( pszName[strlen(pszName) - 1] != '\\' )
			strcat ( pszName, "\\" );
	}


    //---- add the subdirectory 
	if ( strlen(pszSubDir) != 0 )
	{
		strcat ( pszName, pszSubDir );
		if ( pszName[strlen(pszName) - 1] != '\\' )
			strcat ( pszName, "\\" );
	}
    

    //---- add the filename and extension
	if ( strlen(pszFileName) != 0 )
		strcat ( pszName, pszFileName );

	if ( strlen(pszExt) != 0 )
	strcat ( pszName, pszExt );
} // MakeQualifiedFilename




//--------------------------------------------------------------------------
//
//  int SMessageBox( ... )
//
//  Description:
//		Display the requested message box retrieving the text from the
//			resource specified.
//
//  Arguments:
//		hwnd			- window handle
//		hInst			- instance of resource
//		usIdText		- id of text to display
//		usIdCaption	- id of caption to display
//		uType			- type flags
//
//  Return (void):
//
//--------------------------------------------------------------------------
int SMessageBox( HWND hwnd, HINSTANCE hInst, USHORT usIdText,	
													USHORT usIdCaption, UINT uType)
{
static CHAR szText[MAX_PATH] = "";
static CHAR szCaption[MAX_PATH] = "";

	 // -- Load the text and caption strings
	LoadString( hInst, usIdText, (PCHAR) szText, sizeof(szText) );
	LoadString( hInst, usIdCaption, (PCHAR) szCaption, sizeof(szCaption) );

	return ( MessageBox( hwnd, szText, szCaption, uType ) );
} // SMesssageBox


// WinUtil.c