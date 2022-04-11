#ifndef _TMSGBOX_H
#define _TMSGBOX_H
/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: TMsgBox.hxx
   Author:  Wes Cumberland
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:
   TitledMessageBox::Display    -Displays a Message
   TitledMessageBox::SetTitle   -Copies a string into the Title buffer
   TitledMessageBox::SetMessage -Copies a string into the Message buffer
   ======================================================================== */

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif


#if !defined (_MENUPROT_H)
#include "menuprot.h"
#endif

#if !defined(_GMENUENM_H)
#include "gmenuenm.h"
#endif

#define MSGBOX_BUT_ALL	  0
#define MSGBOX_BUT_TITLE  1
#define MSGBOX_BUT_MSG	  2
#define MSGBOX_BUT_OK	  3

#define TITLE_LEN	60
#define MESSAGE_LEN	1000


class TitledMessageBox
{
	private:
		static char Title[TITLE_LEN];
		static char Message[MESSAGE_LEN];
		static BOOL fAlreadyUp;
		static long LRMargin;
		static MENU_ID_ENUM ThisMenu;

		static LONG Width(LONG Area=0);
		static LONG Height(LONG Area=0);
	public:
		static void PrintMessage(LONG,LONG);
		static void SetMessage(const char* m);
		static void SetTitle(const char* t);
		static void Display(const char* t,const char* m1);
		static void Display();
		static void Hide(LONG,LONG);
};

inline LONG TitledMessageBox::Width(LONG Area)
{
	LONG W,H;
	GetButtonSize(ThisMenu,Area,&W,&H);
	return W;
}
inline LONG TitledMessageBox::Height(LONG Area)
{
	LONG W,H;
	GetButtonSize(ThisMenu,Area,&W,&H);
	return H;
}
	

#endif
