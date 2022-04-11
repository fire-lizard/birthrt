// ===========================================================================
// STATUS.H
// Adapted from Status Bar code by Michael Branham for VirtuHome.
// [d5-07-96  8:46 JPC]
// Copyright 1996 by NW Synergistic Software, Inc.  All rights reserved.
// ===========================================================================
#if defined (_STATUS)
#ifndef _STATUS_H_
#define _STATUS_H_

#include <windows.h>

#if defined (__cplusplus)
extern "C" {
#endif

#define ID_STATUSBAR       160
#define ID_SB_MESSAGEFIELD ID_STATUSBAR+1
#define ID_SB_X            ID_STATUSBAR+2
#define ID_SB_Y            ID_STATUSBAR+3
#define ID_SB_Z            ID_STATUSBAR+4
#define ID_SB_A            ID_STATUSBAR+5
#define ID_SB_P            ID_STATUSBAR+6
#define ID_SB_R            ID_STATUSBAR+7
#define ID_SB_S            ID_STATUSBAR+8

extern HWND ghwndStatus;
extern HWND ghwndSBMessageField;
extern HWND ghwndSBx;
extern HWND ghwndSBy;
extern HWND ghwndSBz;
extern HWND ghwndSBa;
extern HWND ghwndSBp;
extern HWND ghwndSBr;
extern HWND ghwndSBs;

BOOL InitStatusBar (HANDLE hInstance);
BOOL CreateStatusBar (HWND hwnd, HANDLE hInst, int iId);
BOOL AdjustStatusBar (HWND hwnd);
HWND AddStatusField (HANDLE hInst, int iId, int iMin, int iMax, BOOL bNewGroup);
BOOL DestroyStatusBar (void);

#if defined (__cplusplus)
}
#endif
#endif
#endif

