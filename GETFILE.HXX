// ===========================================================================
//
// GETFILE.HXX
//
// ===========================================================================

#if !defined (_GETFILE_HXX_)
#define _GETFILE_HXX_

#if defined (_WINDOWS)						// Windows required
#define GFN_OPEN  0
#define GFN_SAVE  1

BOOL GetFileName (HWND i_hwnd, LPCSTR i_szDirName, LPCSTR i_szFilter,
	LPCSTR i_szExt, LPSTR o_szReturnName, LPSTR o_szReturnTitle,
	DWORD i_dwflags, int openOrSave);

#endif
#endif
