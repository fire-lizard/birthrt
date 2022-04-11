// ===========================================================================
// STATUS.C
//    Copyright 1996 by NW Synergistic Software, Inc.  All rights reserved.
//
//		Programmed by:				John Conley
//    Original code by:       Michael Branham for VirtuaHome
// ===========================================================================

#if defined (_STATUS)
#include <windows.h>
#include "system.h"

#define MAXSTATUS 		10
#define STATUS_WIDTH		800

typedef struct _tagStatus {
	 HWND    hwnd;
	 int     iMaxWidth, iMinWidth, iGiveWidth;
} Status;

Status  statusField[MAXSTATUS];

LONG APIENTRY StatusProc (HWND hwnd, UINT msg, UINT wParam, LONG lParam);
LONG APIENTRY StatusFieldProc (HWND hwnd, UINT msg, UINT wParam, LONG lParam);

HWND              ghwndStatus;
HWND              ghwndSBMessageField;
HWND              ghwndSBx;
HWND              ghwndSBy;
HWND              ghwndSBz;
HWND              ghwndSBa;
HWND              ghwndSBp;
HWND              ghwndSBr;				// [d12-06-96 JPC] return from bump code
HWND              ghwndSBs;				// [d12-09-96 JPC] current sector

static int        cntStatusField = 0;
static int        dyStatus, cxStatusBorder, cyStatusBorder,
                  cxFrame, cyFrame, dyField;
static HFONT      hfontStatus;
static TEXTMETRIC tmStatusFont;
static HBRUSH     hbrBtnFace;

BOOL InitStatusBar (HANDLE hInstance)
{
	 WNDCLASS    wndclass;

	 hbrBtnFace = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

	 wndclass.style         = CS_HREDRAW | CS_VREDRAW;
	 wndclass.lpfnWndProc   = (WNDPROC)StatusProc;
	 wndclass.cbClsExtra    = 0;
	 wndclass.cbWndExtra    = 0;
	 wndclass.hInstance     = hInstance;
	 wndclass.hIcon         = NULL;
	 wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
	 wndclass.hbrBackground = hbrBtnFace;
	 wndclass.lpszMenuName  = NULL;
	 wndclass.lpszClassName = "Status";

	 if (!RegisterClass (&wndclass))
		  return FALSE;

	 wndclass.style         = CS_HREDRAW | CS_VREDRAW;
	 wndclass.lpfnWndProc   = (WNDPROC)StatusFieldProc;
	 wndclass.cbClsExtra    = 0;
	 wndclass.cbWndExtra    = 0;
	 wndclass.hInstance     = hInstance;
	 wndclass.hIcon         = NULL;
	 wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
	 wndclass.hbrBackground = hbrBtnFace;
	 wndclass.lpszMenuName  = NULL;
	 wndclass.lpszClassName = "StatusField";

	 if (!RegisterClass (&wndclass))
		  return FALSE;

	 return TRUE;
}

BOOL CreateStatusBar (HWND hwnd, HANDLE hInst, int iId)
{
// [d5-07-96  8:50 JPC] Change: make this a non-child window.
// ---------------------------------------------------------------------------
   int         x, y, w, h;

	cxStatusBorder = GetSystemMetrics (SM_CXBORDER);
	cyStatusBorder = GetSystemMetrics (SM_CYBORDER);

   x = 0;
   y = 500;
   w = 640;
   h = 40;

   ghwndStatus = CreateWindow ("Status", "Status",
	   // WS_CHILD | WS_BORDER | WS_VISIBLE,
		WS_CAPTION | WS_POPUP | WS_BORDER | WS_VISIBLE,
		x, y, w, h,
		// hwnd, (HMENU)iId, hInst, NULL);
		NULL, NULL, hInst, NULL);

	if (!ghwndStatus)
		return FALSE;
		
	SetWindowPos(ghwndStatus, HWND_TOP, 0, 0, w, h, SWP_NOMOVE);
	return TRUE;
}

int StatusBarHeight (HWND hwnd)
{
	 RECT rect;
	 GetClientRect (ghwndStatus, &rect);
	 return (rect.bottom-rect.top);
}


// ---------------------------------------------------------------------------
BOOL AdjustStatusBar (HWND hwnd)
{
// Needs modification because it's now a popup window.
// Move to under the Birthright window (hwnd).
// ---------------------------------------------------------------------------
	RECT rect;
	GetWindowRect (hwnd, &rect);
	MoveWindow (ghwndStatus,
		0,											// top
		rect.bottom - dyStatus/2,			// left
		STATUS_WIDTH,							// width
		dyStatus,								// height
		TRUE);									// repaint flag
	return TRUE;
}

HWND AddStatusField (HANDLE hInst, int iId, int iMin, int iMax, BOOL bNewGroup)
{
	 LONG lStyle;

	 if (cntStatusField >= MAXSTATUS) return (HWND)0; // No room left in our fixed array

    if (ghwndStatus == NULL)           // [d5-07-96  8:55 JPC]
      return NULL;

	 statusField[cntStatusField].hwnd = CreateWindow ("StatusField", "",
		  WS_CHILD | WS_VISIBLE,
		  0, 0, 0, 0,
		  ghwndStatus, (HMENU)iId, hInst, NULL);

	 if (!statusField[cntStatusField].hwnd) return (HWND)0; // CreateWindow failed for some reason

	 if (iMin < 0) {
	 statusField[cntStatusField].iMinWidth = tmStatusFont.tmAveCharWidth*abs(iMin);
	 } else {
	 statusField[cntStatusField].iMinWidth = iMin;
	 }

	 if (iMax < 0) {
	 statusField[cntStatusField].iMaxWidth = tmStatusFont.tmAveCharWidth*abs(iMax);
	 } else {
	 statusField[cntStatusField].iMaxWidth = iMax;
	 }

	 if (bNewGroup) {
		  lStyle = GetWindowLong (statusField[cntStatusField].hwnd, GWL_STYLE);
		  lStyle |= WS_GROUP;
		  SetWindowLong (statusField[cntStatusField].hwnd, GWL_STYLE, lStyle);
	 }

	 return statusField[cntStatusField++].hwnd;
}

BOOL DestroyStatusBar (void)
{
	 return DeleteObject (hbrBtnFace);
}

LONG APIENTRY StatusProc (HWND hwnd, UINT msg, UINT wParam, LONG lParam)
{
	 HDC hdc;
	 PAINTSTRUCT ps;
	 int x, y, i;
	 int wAvailWidth, wFlexWidth, cntFlexWidth, wNeedWidth, cntNeedWidth;
	 RECT    rect, border;
	 HBRUSH  hBrush;
	 LOGFONT lfTmp;

	 switch (msg) {
		  case WM_CREATE:
				hfontStatus = CreateFont(14, 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0,
					 VARIABLE_PITCH | FF_SWISS, "");

								if (GetObject (hfontStatus, sizeof(LOGFONT), &lfTmp)) {
										  if ((lfTmp.lfPitchAndFamily & VARIABLE_PITCH) &&
											  (lfTmp.lfPitchAndFamily & FF_SWISS)) {
										  } else {
													 MessageBox (GetFocus(),
"Unable to get an unnamed variable pitch swiss font", "Status Bar CreateFont Error",
																MB_OK);
					 hfontStatus = CreateFont(14, 0, 0, 0, 0, 0, 0, 0,
								0, 0, 0, 0,
								VARIABLE_PITCH | FF_SWISS, "Arial");
										  }
								}


				if (!hfontStatus) {
					 MessageBox (GetFocus(), "Failed To Create Font", "StatusProc", MB_OK);
				}
				hdc = GetDC (hwnd);
				SelectObject (hdc, hfontStatus);
				GetTextMetrics (hdc, &tmStatusFont);
				cxStatusBorder = GetSystemMetrics (SM_CXBORDER);
				cyStatusBorder = GetSystemMetrics (SM_CYBORDER);
				cxFrame = 3*cxStatusBorder;
				cyFrame = 3*cyStatusBorder;
				dyField = tmStatusFont.tmHeight + (2*cyStatusBorder);
				dyStatus = dyField + (2*cyFrame) +  // original
               GetSystemMetrics (SM_CYCAPTION) + 2; // [d5-07-96 10:37 JPC]
				ReleaseDC (hwnd, hdc);
				return DefWindowProc (hwnd, msg, wParam, lParam);

		  case WM_DESTROY:
				if (hfontStatus) {
					 DeleteObject (hfontStatus);
				}
				break;

		  case WM_SIZE:
		  if (cntStatusField) {
					 GetClientRect (hwnd, &rect);
					 wAvailWidth = rect.right - rect.left - (cxStatusBorder*8);
					 wNeedWidth = 0;
					 cntNeedWidth = 0;
					 cntFlexWidth = 0;

					 /* First Pass: Dole out to fields that have a minimum need */
					 for (i=0; i<cntStatusField; i++) {
						  statusField[i].iGiveWidth = 0; // Make sure all are initialized to 0
						  if (statusField[i].iMinWidth) {
								/* (n, ?) */
                        statusField[i].iGiveWidth = statusField[i].iMinWidth;
                        wAvailWidth -= (statusField[i].iGiveWidth + cxStatusBorder*2);
                        if (GetWindowLong(statusField[i].hwnd, GWL_STYLE) & WS_GROUP) {
                            wAvailWidth -= cxStatusBorder*4;
                        }
                    } else {
                        /* They didn't specify a minimum... don't give them anything yet */
								/* (0, ?) */
                        statusField[i].iGiveWidth = 0;
                    }

                    /* For those that have a minimum, but can grow to be as large as possible...*/
                    /* (n, 0) */
                    if ((statusField[i].iMinWidth >0) && (statusField[i].iMaxWidth ==0)) {
                        ++cntFlexWidth;
                    }

						  /* For those that have a max that is greater then their min... */
						  /* Includes (0,n) and (n,>n) */
                    if (statusField[i].iMaxWidth > statusField[i].iGiveWidth) {
                        wNeedWidth += (statusField[i].iMaxWidth - statusField[i].iGiveWidth);
								++cntNeedWidth;
                    }
                }

                /* Second Pass: Dole out to fields that have a stated maximum need */
                /* This will also hit those who had no minimum, but did have a maximum */
                /* It will still not give anything to those with no min, no max */
                if ((cntNeedWidth > 0) && (wAvailWidth > 0)) {
                    if (wNeedWidth > wAvailWidth) {
                        wNeedWidth = wAvailWidth;
                    }
                    wNeedWidth = wNeedWidth / cntNeedWidth;
                    for (i=0; i<cntStatusField; i++) {
								if (statusField[i].iMaxWidth > statusField[i].iGiveWidth) {
									 statusField[i].iGiveWidth += wNeedWidth;
									 wAvailWidth -= (statusField[i].iGiveWidth + cxStatusBorder*2);
                            if (GetWindowLong(statusField[i].hwnd, GWL_STYLE) & WS_GROUP) {
                                wAvailWidth -= cxStatusBorder*4;
                            }
                        }
                    }
                }

                /* Third Pass: Dole out the remaining to fields that want all they can get */
                /* This includes those who had a minimum, but no maximum */
                if ((cntFlexWidth > 0) && (wAvailWidth > 0)) {
						  wFlexWidth = wAvailWidth / cntFlexWidth;
                    for (i=0; i<cntStatusField; i++) {
                        if (statusField[i].iMaxWidth==0) {
									 statusField[i].iGiveWidth += wFlexWidth;
                            wAvailWidth -= ((wFlexWidth - statusField[i].iMinWidth) + cxStatusBorder*2);
									 if (GetWindowLong(statusField[i].hwnd, GWL_STYLE) & WS_GROUP) {
										  wAvailWidth -= cxStatusBorder*4;
                            }
                        }
                    }
                }

                x = cxStatusBorder*4;
                y = rect.top + (2*cyStatusBorder);
                for (i=0; i<cntStatusField; i++) {
                    if (GetWindowLong (statusField[i].hwnd, GWL_STYLE) & WS_GROUP) {
                        x += (cxStatusBorder*4);
                    }
						  MoveWindow (statusField[i].hwnd, x, y, statusField[i].iGiveWidth, dyField, TRUE);
						  x += statusField[i].iGiveWidth + (cxStatusBorder*2);
					 }
				}
				break;

		  case WM_PAINT:
            hdc = BeginPaint (hwnd, &ps);
				GetClientRect (hwnd, &rect);

            hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
				border = rect;
            border.bottom = border.top + cyStatusBorder;
            FillRect (hdc, &border, hBrush);
				DeleteObject (hBrush);

            hBrush = CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
            border = rect;
            border.top = border.bottom - cyStatusBorder;
            FillRect (hdc, &border, hBrush);
            DeleteObject (hBrush);

            EndPaint (hwnd, &ps);

				return DefWindowProc (hwnd, msg, wParam, lParam);

        default:
            return DefWindowProc (hwnd, msg, wParam, lParam);
    }
    return 0L;
}



LONG APIENTRY StatusFieldProc (HWND hwnd, UINT msg, UINT wParam, LONG lParam)
{
    HDC     hdc;
	 PAINTSTRUCT ps;
    RECT    rect, border;
    HBRUSH  hBrush;
	 WORD    edge = 1;
	 HFONT   hTmp;
	 char szText[80];
	 int len;

    switch (msg) {
        case WM_PAINT:
            hdc = BeginPaint (hwnd, &ps);
            GetClientRect (hwnd, &rect);

            hBrush = CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
            border = rect;
            border.bottom = border.top + cyStatusBorder;
            FillRect (hdc, &border, hBrush);
            border = rect;
            border.right = border.left + cxStatusBorder;
            FillRect (hdc, &border, hBrush);
            DeleteObject (hBrush);

				hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
				border = rect;
            border.top = border.bottom - cyStatusBorder;
            FillRect (hdc, &border, hBrush);
				border = rect;
            border.left = border.right - cxStatusBorder;
            FillRect (hdc, &border, hBrush);
				DeleteObject (hBrush);

				if (len = GetWindowText(hwnd, szText, sizeof (szText))) {
					 hTmp = SelectObject(hdc, hfontStatus);

                SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
					 SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));

                InflateRect (&rect, -(cxStatusBorder*2), -cyStatusBorder);
                ExtTextOut(hdc, rect.left, rect.top,
                    ETO_OPAQUE | ETO_CLIPPED,
						  &rect,
						  (LPSTR)szText,
                    len, NULL);

                SelectObject (hdc, hTmp);
            }

            EndPaint (hwnd, &ps);
            break;

		  case WM_CTLCOLORSTATIC:
				{
					HDC hdc;

					hdc = (HDC) wParam;
					SetBkMode (hdc, TRANSPARENT);
					return (LRESULT) GetStockObject (LTGRAY_BRUSH);
				} break;
        case WM_SETTEXT:
            InvalidateRect (hwnd, NULL, TRUE);
            return DefWindowProc (hwnd, msg, wParam, lParam);

        default:
				return DefWindowProc (hwnd, msg, wParam, lParam);
    }
    return 0L;
}

#endif
