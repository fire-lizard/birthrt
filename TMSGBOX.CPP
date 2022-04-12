/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: TMsgBox.cpp -Handles a titled message box
   Author:   Wes Cumberland
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:
   TitledMessageBox::PrintMessage() -Displays the box and prints a message
   ======================================================================== */
#include <stdio.h>
#include <string.h>

#include "TMSGBOX.HXX"

#include "MACHINE.H"
#include "SYSTEM.H"
#include "PANEL.H"
#include "STRMGR.H"
#include "strenum.h"
#include "COMBATUI.HXX" //for ForceHires


char TitledMessageBox::Message[]="\0";
char TitledMessageBox::Title[]="\0";
BOOL TitledMessageBox::fAlreadyUp=FALSE;
long TitledMessageBox::LRMargin=4;
MENU_ID_ENUM TitledMessageBox::ThisMenu=D_TITLED_MESSAGE_BOX;

//Display(void) doesn't copy anything into the title buffer or the message 
//buffer. It simply uses the data that was there before...

void TitledMessageBox::Display()
{
	if (fAlreadyUp)
		return;
	
	fAlreadyUp=TRUE;							// [d1-07-97 JPC] moved up

	ForceHires();

	ShowMenu(D_TITLED_MESSAGE_BOX);
	RunMenus();									// [d1-07-97 JPC] added
	fAnyKeyChanged=FALSE;
}

void TitledMessageBox::Display(const char* t,const char* m)
{
	if (!fAlreadyUp)
	{
		SetTitle(t);
		SetMessage(m);
	}

	TitledMessageBox::Display();
}

void TitledMessageBox::Hide(LONG MenuCombo,LONG)
{
	LONG	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	SetButtonHilight(MenuId, MSGBOX_BUT_OK, TRUE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	SetButtonHilight(MenuId, MSGBOX_BUT_OK, FALSE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	HideMenu(D_TITLED_MESSAGE_BOX);
	RunMenus();									// [d1-07-97 JPC] moved up
	fAlreadyUp=FALSE;
}

void TitledMessageBox::SetTitle(const char* t)
{
	memset(Title,0,TITLE_LEN);
	long font=8;

	//auto-font-reduce until it fits!
	do
	{
		sprintf(Title,"^F%02i%s",font,t);
	} while (gtext_width(Title)>Width(MSGBOX_BUT_TITLE)-2*LRMargin && font--!=5);

//	strcpy(Title,"^F08");
//	strncat(Title,t,TITLE_LEN-5);

}

void TitledMessageBox::SetMessage(const char* m)
{
	long BadWrapFunctionConstant=16;	//The text wrap function doesn't 
										//actually wrap where you tell it to.
	memset(Message,0,MESSAGE_LEN);

	sprintf(Message,"^W%03i^F03",Width(MSGBOX_BUT_TITLE)-2*LRMargin-BadWrapFunctionConstant);
	strcat(Message,m);
}



void TitledMessageBox::PrintMessage(LONG mc,LONG)
{
	LONG MenuId,ButtonId;

	SPLIT_LONG(mc, MenuId, ButtonId);

	LONG TitleTLX;
	LONG TitleTLY;
	LONG TitleW;
	LONG TitleH;
	LONG MessageTLX;
	LONG MessageTLY;
	LONG MessageW;
	LONG MessageH;
	LONG MarginY;
	LONG MarginX;

	//get the Text box
	if( GetButtonPosition( MenuId, MSGBOX_BUT_TITLE, &TitleTLX, &TitleTLY ) == fERROR ||
 	    GetButtonSize( MenuId, MSGBOX_BUT_TITLE, &TitleW, &TitleH)==fERROR)
		return;

	if( GetButtonPosition( MenuId, MSGBOX_BUT_MSG, &MessageTLX, &MessageTLY ) == fERROR ||
 	    GetButtonSize( MenuId, MSGBOX_BUT_MSG, &MessageW, &MessageH)==fERROR)
		return;

	MarginX=8;  //don't change.

	MarginY=(MessageH-gtext_height(Message))/2;

	print_text_centered(TitleTLX+TitleW/2,TitleTLY+TitleH/2-5,Title,BLACK);
	

	print_textf(MessageTLX+MarginX,MessageTLY+MarginY,BLACK,Message);
	
#ifdef _DEBUG
//	OutlineAllRegions();
#endif
}

void TitledMessage (char* t,char* m1)
{
// [d9-16-96 JPC] Added for access from C.
	TitledMessageBox::Display (t, m1);
}
