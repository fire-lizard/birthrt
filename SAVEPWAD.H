// SAVEPWAD.H
// 5-22-96 JPC

#ifndef _SAVEPWAD_H_
#define _SAVEPWAD_H_

#define GFN_OPEN  0
#define GFN_SAVE  1

// TODO: (fire lizard) uncomment
//BOOL MakePwad(char* PwadName);
BOOL GetFileName (HWND hwnd, LPCSTR szDirName, LPCSTR szFilter, LPCSTR i_szExt,
   LPSTR szReturnName, LPSTR szReturnTitle, DWORD flags, int openOrSave);

#endif

