/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename:
   Author:
   ========================================================================
   Contains the following internal functions: 

   Contains the following general functions:
   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "system.h"
#include "sysint.h"
#include "engine.h"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
extern long fDialogUp;
extern long objClicked;

struct DIALOGBOX
{
	struct rDialogRect
	{
		long left, top, right, bottom;
	} rDialogRect;
	SHORT wDialogBackColor;
	void (*pfOnHide)();
	LONG wNumItems;
	LONG fWhichHasFocus;
	LONG fActive;
	struct dialogItem
	{
		SHORT wId;
		struct rDialogItemRect
		{
			long left, top, right, bottom;
		} rDialogItemRect;
		SHORT fFlags;
		SHORT wBackColor;
		SHORT wForeColor;
		SHORT wFlashColor;
		CHAR* szCaption;
		PFVL pfLButtonProc;
		PFVL pfRButtonProc;
		LONG wFlashCount;
		LONG lLButtonArg;
		LONG lRButtonArg;
	};
	struct dialogItem dialogItems[];
} sDialog;
#define MAX_DIALOG_ITEMS 100
#define DIALOG_ITEM_VISIBLE 1
#define DIALOG_ITEM_FLASH 1
#define DIALOG_ITEM_BUTTON 1
#define DIALOG_ITEM_EDIT 1
#define DIALOG_LEFT_BUTTON 1
#define DIALOG_ITEM_EXIT_ON_SELECT 1

int iWhichDialogItem;

//extern long numFrames;
long numFrames=0;

void InitDialog(
	long lDialogXPos,	// dialog x position
	long lDialogYPos,	// dialog y position
	long lDialogWidth,	// dialog width
	long lDialogHeight,	// dialog height
	SHORT wDialogBackColor)	// back ground color of dialog
{
	int i;

	// set up the dialog rect
	sDialog.rDialogRect.left = lDialogXPos;
	sDialog.rDialogRect.top = lDialogYPos;
	sDialog.rDialogRect.right = lDialogXPos + lDialogWidth;
	sDialog.rDialogRect.bottom = lDialogYPos + lDialogHeight;

	// check for auto sizing dialog
	if(0 == lDialogWidth)
		sDialog.rDialogRect.right = 0;
	if(0 == lDialogHeight)
		sDialog.rDialogRect.bottom = 0;

	// initialize the dialog items
	for(i=0; i<MAX_DIALOG_ITEMS; i++)
		sDialog.dialogItems[i].wId = -1;

	// set the dialog background color
	// if undefined set to default smokey glass color
	if(wDialogBackColor == 0)
		wDialogBackColor = 33;

	sDialog.wDialogBackColor = wDialogBackColor;
	
	// no initial hide function
	sDialog.pfOnHide = NULL;

	// initialize item count
	sDialog.wNumItems = 0;
	iWhichDialogItem = 0;
}

void AddDialogItem(
	SHORT wId,				// id number of dialog item
	RECT *rct,				// rect for dialog item
	BOOL fFlags,			// dialog item flags (see dialog.h)
	SHORT wBackColor,		// background color for dialog item
	SHORT wForeColor,		// foreground color for dialog item
	SHORT wFlashColor,		// flash color for dialog item
	char *szString,			// text for dialog item
	PFVL	pfLButtonProc,	// left button proc address
	PFVL	pfRButtonProc)	// right button proc address
{
	sDialog.dialogItems[iWhichDialogItem].wId = wId;
	sDialog.dialogItems[iWhichDialogItem].rDialogItemRect.top = rct->top;
	sDialog.dialogItems[iWhichDialogItem].rDialogItemRect.left = rct->left;
	sDialog.dialogItems[iWhichDialogItem].rDialogItemRect.right = rct->right;
	sDialog.dialogItems[iWhichDialogItem].rDialogItemRect.bottom = rct->bottom;
	sDialog.dialogItems[iWhichDialogItem].fFlags = fFlags;
	sDialog.dialogItems[iWhichDialogItem].wBackColor = wBackColor;
	sDialog.dialogItems[iWhichDialogItem].wForeColor = wForeColor;
	sDialog.dialogItems[iWhichDialogItem].wFlashColor = wFlashColor;
	strcpy(sDialog.dialogItems[iWhichDialogItem].szCaption, szString);
	sDialog.dialogItems[iWhichDialogItem].pfLButtonProc = pfLButtonProc;
	sDialog.dialogItems[iWhichDialogItem].pfRButtonProc = pfRButtonProc;

	// point to next item
	iWhichDialogItem++;
	sDialog.wNumItems++;
}

void AddDialogSelectString(
	SHORT wId,
	LONG x,
	LONG y,
	SHORT wForeColor,
	SHORT wFlashColor,
	char *szString,
	PFVL pfLButtonProc,
	PFVL pfRButtonProc)
{
	sDialog.dialogItems[iWhichDialogItem].wId = wId;
	sDialog.dialogItems[iWhichDialogItem].rDialogItemRect.top = y;
	sDialog.dialogItems[iWhichDialogItem].rDialogItemRect.left = x;
	sDialog.dialogItems[iWhichDialogItem].rDialogItemRect.right = gtext_width(szString);
	sDialog.dialogItems[iWhichDialogItem].rDialogItemRect.bottom = FONT_HEIGHT2+1;
	sDialog.dialogItems[iWhichDialogItem].fFlags = DIALOG_ITEM_VISIBLE | DIALOG_ITEM_FLASH;
	sDialog.dialogItems[iWhichDialogItem].wBackColor = 0;
	sDialog.dialogItems[iWhichDialogItem].wForeColor = wForeColor;
	sDialog.dialogItems[iWhichDialogItem].wFlashColor = wFlashColor;
	strcpy(sDialog.dialogItems[iWhichDialogItem].szCaption, szString);
	sDialog.dialogItems[iWhichDialogItem].pfLButtonProc = pfLButtonProc;
	sDialog.dialogItems[iWhichDialogItem].pfRButtonProc = pfRButtonProc;

	// point to next item
	iWhichDialogItem++;
	sDialog.wNumItems++;
}

void AddDialogStaticString(
	SHORT wId,
	LONG x,
	LONG y,
	SHORT wForeColor,
	char *szString,
	PFVL pfLButtonProc,
	PFVL pfRButtonProc)
{
	sDialog.dialogItems[iWhichDialogItem].wId = wId;
	sDialog.dialogItems[iWhichDialogItem].rDialogItemRect.top = y;
	sDialog.dialogItems[iWhichDialogItem].rDialogItemRect.left = x;
	sDialog.dialogItems[iWhichDialogItem].rDialogItemRect.right = gtext_width(szString);
	sDialog.dialogItems[iWhichDialogItem].rDialogItemRect.bottom = FONT_HEIGHT2+1;
	sDialog.dialogItems[iWhichDialogItem].fFlags = DIALOG_ITEM_VISIBLE;
	sDialog.dialogItems[iWhichDialogItem].wBackColor = 0;
	sDialog.dialogItems[iWhichDialogItem].wForeColor = wForeColor;
	sDialog.dialogItems[iWhichDialogItem].wFlashColor = 0;
	strcpy(sDialog.dialogItems[iWhichDialogItem].szCaption, szString);
	sDialog.dialogItems[iWhichDialogItem].pfLButtonProc = pfLButtonProc;
	sDialog.dialogItems[iWhichDialogItem].pfRButtonProc = pfRButtonProc;

	// point to next item
	iWhichDialogItem++;
	sDialog.wNumItems++;
}

void ShowDialog(void)
{
	sDialog.fActive = TRUE;
}

void HideDialog(void)
{
	if(sDialog.pfOnHide != NULL)
		(*sDialog.pfOnHide)();
		
	sDialog.fActive = FALSE;
}

void MoveDialogTo(LONG x, LONG y)
{
	LONG dx, dy;

	// get the delta move
	dx = sDialog.rDialogRect.left - x;
	dy = sDialog.rDialogRect.top - y;

	// set the new position
	sDialog.rDialogRect.left += dx;
	sDialog.rDialogRect.top += dy;
	sDialog.rDialogRect.right += dx;
	sDialog.rDialogRect.bottom += dy;
}

void MoveDialog(LONG dx, LONG dy)
{
	// set the new position
	sDialog.rDialogRect.left += dx;
	sDialog.rDialogRect.top += dy;
	sDialog.rDialogRect.right += dx;
	sDialog.rDialogRect.bottom += dy;
}

void SetDialogOnHide(PFV pfOnHideFunc)
{
	sDialog.pfOnHide = pfOnHideFunc;
}

void SetDialogItemFlags(SHORT wItemId, BOOL fFlags)
{
	sDialog.dialogItems[wItemId].fFlags = fFlags;
}

BOOL GetDialogItemFlags(SHORT wItemId)
{
	return (sDialog.dialogItems[wItemId].fFlags);
}

BOOL IsDialogItemVisible(SHORT wItemId)
{
	return (sDialog.dialogItems[wItemId].fFlags & DIALOG_ITEM_VISIBLE);
}

BOOL IsDialogItemFlash(SHORT wItemId)
{
	return (sDialog.dialogItems[wItemId].fFlags & DIALOG_ITEM_FLASH);
}

BOOL IsDialogItemButton(SHORT wItemId)
{
	return (sDialog.dialogItems[wItemId].fFlags & DIALOG_ITEM_BUTTON);
}

BOOL IsDialogItemEdit(SHORT wItemId)
{
	return (sDialog.dialogItems[wItemId].fFlags & DIALOG_ITEM_EDIT);
}

BOOL IsDialogItemExitOnSelect(SHORT wItemId)
{
	return (sDialog.dialogItems[wItemId].fFlags & DIALOG_ITEM_EXIT_ON_SELECT);
}

SHORT WhichDialogItemHasFocus(void)
{
	return (sDialog.fWhichHasFocus);
}

void SetDialogItemText(SHORT wItemId, char *szText)
{
	strcpy(sDialog.dialogItems[wItemId].szCaption, szText);
}

void SetDialogItemParam(SHORT wItemId, SHORT wWhichButton, LONG lParam)
{
	if(wWhichButton == DIALOG_LEFT_BUTTON)
		sDialog.dialogItems[wItemId].lLButtonArg = lParam;
	else
		sDialog.dialogItems[wItemId].lRButtonArg = lParam;
}

char *GetDialogItemText(SHORT wItemId)
{
	return((char *)&sDialog.dialogItems[wItemId].szCaption);
}

void MoveDialogItem(SHORT wItemId, LONG dx, LONG dy)
{
	sDialog.dialogItems[wItemId].rDialogItemRect.top += dy;
	sDialog.dialogItems[wItemId].rDialogItemRect.left += dx;
	sDialog.dialogItems[wItemId].rDialogItemRect.right += dx;
	sDialog.dialogItems[wItemId].rDialogItemRect.bottom += dy;
}

void MoveDialogItemTo(SHORT wItemId, LONG x, LONG y)
{
	LONG dx, dy;

	// get the delta move
	dx = sDialog.dialogItems[wItemId].rDialogItemRect.left - x;
	dy = sDialog.dialogItems[wItemId].rDialogItemRect.top - y;

	// set the new position
	sDialog.dialogItems[wItemId].rDialogItemRect.top += dy;
	sDialog.dialogItems[wItemId].rDialogItemRect.left += dx;
	sDialog.dialogItems[wItemId].rDialogItemRect.right += dx;
	sDialog.dialogItems[wItemId].rDialogItemRect.bottom += dy;
}

void DisplayDialog(void)
{
	SHORT i;
	LONG x, y, w, h;
	LONG xOff, yOff;
	LONG lWidth, lHeight;
	char szText[255];

	// check for valid dialog
	if(0 == sDialog.wNumItems)
	{
		// dialog not valid so remove it
		sDialog.fActive = FALSE;
		return;
	}

	if(sDialog.fActive)
	{
		// display the frame
		xOff = sDialog.rDialogRect.left;
		yOff = sDialog.rDialogRect.top;
		lWidth = sDialog.rDialogRect.right - xOff;
		lHeight = sDialog.rDialogRect.bottom - yOff;
//		color_edged_rect(xOff, yOff, lWidth, lHeight, sDialog.wDialogBackColor);
		shade_edged_rect(xOff,yOff,lWidth,lHeight, 33);

		// now show each item
		for(i=0; i<sDialog.wNumItems; i++)
		{
			if(IsDialogItemVisible(i))
			{
				// get the position of the item
				x = xOff + sDialog.dialogItems[i].rDialogItemRect.left;
				y = yOff + sDialog.dialogItems[i].rDialogItemRect.top;
				w = sDialog.dialogItems[i].rDialogItemRect.right - sDialog.dialogItems[i].rDialogItemRect.left;
				h = sDialog.dialogItems[i].rDialogItemRect.bottom - sDialog.dialogItems[i].rDialogItemRect.top;

				// see if there is any text to print
				if(strlen(sDialog.dialogItems[i].szCaption) || IsDialogItemEdit(i))
				{
					// is this a button item
					if(IsDialogItemButton(i) || IsDialogItemEdit(i))
					{
						color_edged_rect(x, y, w, h, sDialog.dialogItems[i].wBackColor);
						x += 4;
						y += 4;
					}

					// get the text to print
					strcpy(szText, sDialog.dialogItems[i].szCaption);

					// are we showing the cursor on an edit field
					if(IsDialogItemEdit(i) && i == WhichDialogItemHasFocus())
					{
						if(numFrames % 2)
							strcat(szText, "_");
					}

					// are we flashing the text after a selection?
					if(IsDialogItemFlash(i) && sDialog.dialogItems[i].wFlashCount)
					{
						// flash the text and decrement the flash count
						print_textf(x, y, (numFrames % 2) == 0 ? sDialog.dialogItems[i].wFlashColor : sDialog.dialogItems[i].wForeColor, szText);
						sDialog.dialogItems[i].wFlashCount--;

						// if the count reaches zero, remove the dialog and execute the button function if any
						if(sDialog.dialogItems[i].wFlashCount == 0)
						{
							if(IsDialogItemExitOnSelect(i))
								HideDialog();

							if(sDialog.dialogItems[i].pfLButtonProc)
							{
								// call the function
								(*sDialog.dialogItems[i].pfLButtonProc)(sDialog.dialogItems[i].lLButtonArg);
							}
						}
					}
					else
						// not flashing this frame, or not selected, or static text, so just print it
						print_textf(x, y, sDialog.dialogItems[i].wForeColor, szText);
				}
			}
		}
	}
}

void AlignDialog(void)
{
	SHORT i;
	int iRight;
	int iBottom;

	iRight = iBottom = 0;
	
	// get the width and height of the dialog
	// also set the right and bottom for strings if they are auto sizing
	for(i=0; i<sDialog.wNumItems; i++)
	{
		if(strlen(sDialog.dialogItems[i].szCaption))
		{
			// check if item rect is auto sizing right
			if(sDialog.dialogItems[i].rDialogItemRect.right == 0)
			{
				sDialog.dialogItems[i].rDialogItemRect.right = sDialog.dialogItems[i].rDialogItemRect.left + gtext_width(sDialog.dialogItems[i].szCaption);

				// make room for auto sizing buttons
				if(IsDialogItemButton(i))
					sDialog.dialogItems[i].rDialogItemRect.right += 8;
			}

			// check if item rect is auto sizing bottom
			if(sDialog.dialogItems[i].rDialogItemRect.bottom == 0)
			{
				sDialog.dialogItems[i].rDialogItemRect.bottom = sDialog.dialogItems[i].rDialogItemRect.top + FONT_HEIGHT2;

				// make room for auto sizing buttons
				if(IsDialogItemButton(i))
					sDialog.dialogItems[i].rDialogItemRect.bottom += 8;
			}
		}

		iRight = MAX(iRight, sDialog.dialogItems[i].rDialogItemRect.right);
		iBottom = MAX(iBottom, sDialog.dialogItems[i].rDialogItemRect.bottom);
	}

	// add space for border
	iRight += 8;
	iBottom += 2;

	if(sDialog.rDialogRect.right == 0)
		sDialog.rDialogRect.right = sDialog.rDialogRect.left + iRight;

	if(sDialog.rDialogRect.bottom == 0)
		sDialog.rDialogRect.bottom = sDialog.rDialogRect.top + iBottom;

	// set the dialog rect to keep in on screen
	if(sDialog.rDialogRect.right >= 640)
	{
		sDialog.rDialogRect.left = 630 - (sDialog.rDialogRect.right - sDialog.rDialogRect.left);
		sDialog.rDialogRect.right = 630;
	}

	if(sDialog.rDialogRect.bottom >= 480)
	{
		sDialog.rDialogRect.top = 470 - (sDialog.rDialogRect.bottom - sDialog.rDialogRect.top);
		sDialog.rDialogRect.bottom = 470;
	}
}

void CenterDialog(void)
{
	LONG dx, dy;
	LONG width, height;

	width = sDialog.rDialogRect.right - sDialog.rDialogRect.left;
	height = sDialog.rDialogRect.bottom - sDialog.rDialogRect.top;

	dx = ((screen_buffer_width / 2) - (width / 2)) - sDialog.rDialogRect.left;
	dy = ((screen_buffer_height / 2) - (height / 2)) - sDialog.rDialogRect.top;

	MoveDialog(dx, dy);
}

// This function is for example only
//GEH void Greet(long lAvatarIndex)
//GEH {
//GEH 	RECT rct;
//GEH 	char szTemp[256];
//GEH 
//GEH 	// put up a greeting dialog
//GEH 	InitDialog(0, 0, 0, 0, 0);
//GEH 	rct.top = 4;
//GEH 	rct.left = 4;
//GEH 	rct.bottom = rct.right = 0;
//GEH 	CAvatar *pAvatar = (CAvatar *) BLKPTR(Avatars[lAvatarIndex]);
//GEH 	
//GEH 	sprintf(szTemp, "My name is %s. %s for your relm.", pAvatar->PlayerStats.Name, pAvatar->PlayerStats.Title);
//GEH 	AddDialogItem(1, &rct, DIALOG_ITEM_VISIBLE, 0, 31, 0, szTemp, NULL, NULL);
//GEH 	rct.top += 8;
//GEH 	AddDialogItem(1, &rct, DIALOG_ITEM_VISIBLE, 0, 31, 0, "How may I help you Sire?", NULL, NULL);
//GEH 	AlignDialog();
//GEH 	CenterDialog();
//GEH 	ShowDialog();
//GEH }
