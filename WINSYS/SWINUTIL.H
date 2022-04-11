//-----------------------------------------------------------------------------
//
//        Copyright 1995/1996 by NW Synergistic Software, Inc.
//
//        SWinUtil.h - Misc. window utilities.
//
//------------------------------------------------------------------------------

#if defined (__cplusplus)
extern "C" {
#endif


void	CenterInWindow	( HWND hwnd, HWND hwndBasis, BOOL fRelative );
void	CenterWindow	( HWND hwnd );

BOOL	DoesCopyProtectFail( PSZ pszDir, PSZ pszVolName );

void	MakeQualifiedFilename( PSZ pszName, PSZ pszDir, PSZ pszSubDir, 
															PSZ pszFileName, PSZ pszExt );

int	SMessageBox		( HWND hwnd, HINSTANCE hInst, USHORT usIdText,	
													USHORT usIdCaption, UINT uType);
#if defined (__cplusplus)
}

#endif