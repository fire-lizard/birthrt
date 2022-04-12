/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: MULITUI.cpp  -
   Author:   David L Jackson

   ========================================================================
   Contains the following general functions:

   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#ifdef _WINDOWS
#include <windows.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "system.h"
#include "machint.h"
//#include "loadsave.hxx"
#include "gamemap.hxx"

#include "menu.h"
#include "strenum.h"
#include "gmenuenm.h"
#include "colors.h"
#include "charsel.hxx"
#include "playstat.hxx"
#include "context.hxx"
#include "panel.h"
#include "random.h"
#include "regents.hxx"
#include "scene.hxx"
#include "scnmgr.hxx"
#include "game.h"
#ifdef _WINDOWS
#include "winsys\gotourl.h"
#include "winsys\mulplay.hxx"
#include "winsys\ddrawpro.h"
#endif


#include "multiui.hxx"


void InitDomainTurn(LONG);

extern "C" {
void SetRedrawMainMapLevel (void);
void MainLoop (void);
}

extern SHORT	fFadedOut;


#ifdef _WINDOWS
extern "C" HWND hwndApp;
#endif


/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
//#define	LOGCOMMENTSLEEP	Sleep(1);
#define  LOGCOMMENTSLEEP	{;}

#undef PURPLE_GRAD
#define PURPLE_GRAD				9

#undef RED_GRAD
#define RED_GRAD					3

#define WAITINGFOR				1

// 180 seconds in milliseconds
#define MP_SYNC_TIMEOUT			180000
// 60 seconds in milliseconds
#define MP_LEFT_TIMEOUT			240000

#define WEBTIMEOUT				5000

#define FONT_TITL_40PT  13

#define MENU_INIT      0		//---- Initial menu
#define MENU_NAME      1		//---- Enter player name
#define MENU_PROT      2		//---- Select Protocol
#define MENU_CONNSPD   3       //---- Connection Speed
#define MENU_JOINNEW   4		//---- Join or new game
#define MENU_JOINDONE  5		//---- Join done
#define MENU_NEWNAME   6       //---- Enter new game name
#define MENU_NEWNO     7		//---- Number of players
#define MENU_NEWDONE   8       //---- New done

#define MENU_TYPESEL   9       //---- New/Load 
#define MENU_COMP      10      //---- Complexity


#define NAME_SIZE 16


#define TYPE_NAME		1
#define TYPE_GAME		2
#define TYPE_NUMBER	3


#define MP_SL          3      // scroll lines 
#define MP_SUP         12     // scroll up button
#define MP_SDOWN       13     // scroll down button 


#define CS_TALK_OFF    50
#define CS_LISTEN_OFF  30
   
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

void MultiPlayerStartGame( LONG MenuCombo, LONG arg );
void PaintAckLED(int k, DWORD id2, BOOL localAcks);	// winsys\gmulplay.cpp

//extern "C" {
//void LoadGame(LONG, LONG);
//}

void IncActionTurn(void);

void StartWaitingMenu(LONG, LONG);

void GetMultiPlayerHelp( LONG, LONG );

extern LONG cheated;

// REMOVE LATER ( HACK VERSION )
#if defined (_WINDOWS)
extern CHAR pszGames[MAX_GAMES][24];		// list of game names
#endif

#ifdef _WINDOWS
extern void ToggleDirectDraw(LONG, LONG);
#endif

extern void RandomLogComment ( char * szString );
extern "C" void RandomLogPrefix ( char * szString );

void DisplayProcess (LONG mode, LONG color, LONG index1, LONG index2);

void SetupMultiPlayerHost ( void );
void SetupMultiPlayerName ( void );
void SetupMultiPlayerCreate ( void );
void SetupMultiPlayerJoin ( void );

extern void MultiResetRealmSelect(void);

/* ------------------------------------------------------------------------
   Static Variables
  ------------------------------------------------------------------------ */

static CHAR chatbuff[100] = {0};


SHORT				iMPCheck[2] = {fERROR, fERROR};
SHORT				iMPDCtrlIc = fERROR, iMPDCtr2Ic = fERROR;


//---- Temp of the real chat send in mulplay.cpp

#ifdef _WINDOWS
static int  TempChatSend[MAX_PLAYERS+1];  // Who to send chat messages to
static int  TempChatRec[MAX_PLAYERS+1];  // Who to send chat messages to
static char szTempName[NAME_SIZE+2];
static char szTempGame[NAME_SIZE+2];
static char szTempNumber[1+2];
static int  TempProt;
#else
static int TempChatSend[8+1]; // Who to send chat messages to
static int TempChatRec[8+1];  // Who to send chat messages to
#endif

static int fWeb  = TRUE;
static int fQuestion = FALSE;
static int fWait = FALSE;
static int m_mode;
static int highlighted_choice = -1;  //---- Non-selected

static int fEnumerate = 1;

static int fQuit = iMPQ_BEGINNEW;

static int iGameTop  = 0;
static int iNumGames = 0;

static DWORD dwWebStart = 0;


#if 0 // TEST CODE
char psG[10][8] = {  

"Line 1",
"Line 2",
"Line 3",
"Line 4",
"Line 5",
"Line 6",
"Line 7",
"Line 8",
"Line 9",
"Line 10"

 };
#endif
           

// static int iTransType = iMPFT_REC; //---- Default to receive


/* ------------------------------------------------------------------------
   Global Variables
  ------------------------------------------------------------------------ */
static LONG	gEnvironment = 1;
static LONG	gProtocol = 1;
static LONG	gJoinGame = -1;
static LONG	gNumPlayers = 1;

static LONG	gGameType = 1;
static LONG	gTimeLimit = 3;
static LONG	gDetailLevel = 1;
static LONG	gDifficulty = 1;

BOOL gMPStartGame = FALSE;
static BOOL gMPStartGameLoop = TRUE;

int iTTimeLimit = iTT_None;  //---- Turn time limit default None

BOOL		fSyncPending = FALSE;

LONG       lSaveHighlight;
    
extern BOOL char_selected;		// from game.cpp
extern BOOL fDomainTurn;		// from gamemap.cpp
extern BOOL fInfoOnlyDomainUI;	// from gamemap.cpp

/* ------------------------------------------------------------------------
   External Variables
   ------------------------------------------------------------------------ */

extern BOOL	fPractice;
extern BOOL fTutorialSelected;
extern BOOL fTutorialFirstBattle;

extern float	RealmAIVersionNumber;

extern LONG		WaitingWho;

#ifdef _WINDOWS
extern BOOL fStartGame;   //---- In winsys\mulplay.cpp
extern LONG fControlMode;
#endif

extern SHORT	iDMapBx;

extern "C" {
extern PMENU		Menus;
}


#define TOGGLEMODE

/* ========================================================================
	Function		- SetScreenMode()
	Description - set screen mode to normal based on flags and whether we are
					  in DDraw 

	Returns	- void
	======================================================================== */
#ifdef _WINDOWS
void setScreenMode ( int fExclusive, int fCursor )
{

	if (sDrawMode == iDDRAW)
	{

#ifdef TOGGLEMODE
		ToggleDirectDraw(0, 0);
#else
		if ( fExclusive )
		{
			if ( fCursor )
			{
				ShowCursor(FALSE);	
			}

			DDSetCooperative ( TRUE );

		}
		else
		{

			if ( fCursor )
			{
				ShowCursor(TRUE);
			}

			DDSetCooperative (FALSE);

		}


#endif

	}

}
#endif


/* ========================================================================
	Function	- 
	Description -
	Returns	- void
	======================================================================== */
static void DrawShield (LONG x, LONG y, LONG rlm)
{
	SetRemapTable (rlm%LAND_REALM_COUNT);
	DrawBitmap ((SHORT)x, (SHORT)y, (rlm < LAND_REALM_COUNT)?iMPDCtrlIc:iMPDCtr2Ic, 0, 0, 999, 999);
	ClearRemapTable();
}


/* ========================================================================
   Function    - DoButtonMove
   Description - move a button in a menu
   Returns     -
   ======================================================================== */
void DoButtonMove ( LONG MenuCombo )
{ 

	LONG	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
		
	// click the button
	SetButtonHilight(MenuId, ButtonId, TRUE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	SetButtonHilight(MenuId, ButtonId, FALSE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);


}  // DoButtonMove()



/* ========================================================================
   Function    - SetMPControlMode
   Description - Set control mode for multiplayer
   Returns     -
   ======================================================================== */
void SetMPControlMode ( void )
{

#ifdef _WINDOWS

	if ( IsMultiPlayer() )
	{
       //---- Expert mode for now add dialog later to set this
   }

#endif

}



/* ========================================================================
   Function    - PaintWebStart
   Description -
   Returns     -
   ======================================================================== */

void PaintWebStart( LONG MenuCombo, LONG )
{
	int i;


	// set the statement text
	LONG	mx,my;
	LONG	xOff = 0;
	LONG	yOff = 0;
	LONG	MenuId, ButtonId;
	char	textbuf[1024];

	char *  ptemp;

	LONG X;
	LONG Y;
	LONG W;
	LONG H;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	X = X_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].X);
	Y = Y_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].Y);
	W = X_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].W);
	H = Y_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].H);

	xOff = Menus[MenuId].Buttons[0].X;
	yOff = Menus[MenuId].Buttons[0].Y;



#ifdef _WINDOWS


   sprintf( textbuf, "%s",
  		 	 STRMGR_GetStr(STR_STARTWEB) );


	// paint the control buttons

	init_gfont(FONT_SANS_12PT);

	print_text_centered(
		X + xOff + (W/2),
		Y + yOff + (H/2) + 1,
		textbuf,
		Menus[MenuId].Buttons[QUESTION_TEXT].LabelColor
		);


   //---- Exit if we have been up 5 secs 

   if ( dwWebStart < GetTickCount() )
   {
       dwWebStart = 0;
		HideSubMenu( 0, D_QUESTION1 );
		GAMEToggleMainMenu(TRUE,0);  //---- start up main menu
       
   }

#endif

}



void CancelWebStart(LONG , LONG )
{
   dwWebStart = 0;
   HideSubMenu( 0, D_QUESTION1 );
   GAMEToggleMainMenu(TRUE,0);  //---- start up main menu

}


/* ========================================================================
   Function    - SetupWebStart 
   Description - Setup web start dialog
   Returns     -
   ======================================================================== */

void SetupWebStart ( void )
{

	SetButtonProc (D_QUESTION1, 0, PaintWebStart, 0, 0 );
	SetButtonLabel (D_QUESTION1, QUESTION_TEXT, -1, BLACK );



   // Set the O.K. button

	SetButtonLabel  (D_QUESTION1, QUESTION_BTN1, STR_OK, BTN_LABEL_COLOR );
	//
	// HACK WARNING Key is hard coded! Won't work with international version!
	SetButtonProc   (D_QUESTION1, QUESTION_BTN1, CancelWebStart, 0, D_KEY_OK );
	//
	//
	SetButtonHilight(D_QUESTION1, QUESTION_BTN1, FALSE );


}



/* ========================================================================
   Function    - ShowWebStart
   Description - Show temp menu for starting web page 
   Returns     -
   ======================================================================== */
void ShowWebStart(LONG, LONG)
{

#ifdef _WINDOWS


	//---- Turn on our params

	SetupWebStart();

	dwWebStart = GetTickCount() + WEBTIMEOUT;

	ShowMenu(D_QUESTION1);

	RunMenus();

#endif


}	//---- End of ShowWebStart()





/* ========================================================================
   Function    - ShowWaitMultiType
   Description - Show the waiting for game type window
   Returns     -
   ======================================================================== */
void ShowWaitMultiType(LONG, LONG)
{

#ifdef _WINDOWS


	//---- Turn on our params

	SetupWaitMultiType();

	ShowMenu(D_QUESTION1);

	RunMenus();

#endif



}	//---- End of ShowWaitMultiType()



/* ========================================================================
   Function    - ShowMPOver
   Description - Show the multiplayer game over screen
   Returns     -
   ======================================================================== */
void ShowMPOver(LONG, LONG)
{

#ifdef _WINDOWS


	//---- Turn on our params

	SetupMPOver();

	ShowMenu(D_QUESTION1);

	RunMenus();

#endif



}	//---- End of ShowMPOver()



/* ========================================================================
   Function    - ShowMultiQuestion
   Description - Show multi player question
   Returns     -
   ======================================================================== */
void ShowMultiQuestion(LONG, LONG)
{

#ifdef _WINDOWS

	//---- If question is not already up

	if ( !fQuestion )
	{
		//---- Setup question

		SetupMultiQuestion();

		ShowMenu(D_QUESTION2);

	}


#endif



}	//---- End of ShowMultiQuestion()


/* ========================================================================
   Function    - ShowMultiQuit
   Description - Show multi player quit
   Returns     -
   ======================================================================== */
void ShowMultiQuit(LONG from, LONG)
{

#ifdef _WINDOWS

	fQuit = from;

	//---- Setup question

	SetupMultiQuit();

	ShowMenu(D_QUESTION2);


#endif



}	//---- End of ShowMultiQuit()



/* ========================================================================
   Function    - ShowVoteWaitWindow
   Description - Show the vote wait window
   Returns     -
   ======================================================================== */
void ShowVoteWaitWindow(LONG, LONG)
{

#ifdef _WINDOWS


	//---- Send Event

	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	if ( fLogComment )
   {
		char temp[100];
		sprintf ( temp, "ShowVoteWaitWindow: SendEvent");
		RandomLogComment ( temp );
	}
	#endif
	AMultiPlayer.SendEvent( iMPE_ADVENTURE,	0, 0, 0, 0, 0 );


	//---- Turn on our params

	SetupVoteWait();

	ShowMenu(D_MULTIWAIT);

#endif



}	//---- End of ShowVoteWaitWindow()




/* ========================================================================
   Function    - ShowSyncWait
   Description - Show the sync wait window
   Returns     -
   ======================================================================== */
void ShowSyncWaitWindow( LONG sync, LONG)
{

#ifdef _WINDOWS

	int i;
	BOOL fDone;

	//---- Send Sync ( let others know we are here )

	//GEH ABC and I agree that it is better to hang here
	//GEH then to just Finalize
	while (!AMultiPlayer.SendSync( sync ))
	{
		AMultiPlayer.LookForMessages();

		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
		char temp[200];
		sprintf ( temp, "ShowSyncWaitWindow - SendSync Failed!");
		RandomLogComment ( temp );
		}
	}

	//---- Pre check if all syncs have come in

	for ( i = 0; i < MAX_PLAYERS; i++ )
	{
		DWORD dwId;

		//---- Get Player info ( Name etc )

		dwId = AMultiPlayer.GetAPlayerId ( i );

		if ( dwId != kNoPlayer )
		{
			//---- are they at this sync point also

			if ( !AMultiPlayer.GetSync( sync, i ) )
			{
				fDone = FALSE;
				break;
			}
		}
	}

	//---- Everyone has synced ( they were waiting for us )

	if ( fDone )
	{
		AMultiPlayer.ResetSync( sync );
		fSyncPending = FALSE;
		return;
	}


	//---- Turn on our params

	SetupSyncWait( sync );

	ShowMenu(D_MULTIWAIT);

	RunMenus();

#endif



}	//---- End of ShowSyncWaitWindow()



/* ========================================================================
   Function    - UpdateChatValues
   Description - 
   Returns     -
   ======================================================================== */

void UpdateChatValues(void)
{


#ifdef _WINDOWS


	if ( IsMultiPlayer() )
	{
       int i;

   	//---- Save new player send/receive structure

   	for ( i = 0; i < MAX_PLAYERS+1; i++ )
	    {
		    AMultiPlayer.SetChat( i, TempChatSend[i] );
   		AMultiPlayer.SetChatRec( i, TempChatRec[i] );
	    }

   }

#endif

}

/* ========================================================================
   Function    - ToggleChatSend
   Description - 
   Returns     -
   ======================================================================== */
void ToggleChatSend( LONG index, LONG old )
{


	int i;
	int iOk = 1;

#ifdef _WINDOWS

	if ( IsMultiPlayer() )
	{


   	//---- Selected all

   	if ( index == 0 )
	    {
   		TempChatSend[0] = 1;					//---- Set All Players

	    	for ( i = 1; i < MAX_PLAYERS+1; i++ )
		    {
			    TempChatSend[i] = 1;				//---- Reset single players
   		}	

	    }
   	else  	//----- Select a single / multiplayers
	    {

		    if ( AMultiPlayer.GetAPlayerId ( index - 1 ) != kNoPlayer )
   		{

	    		//---- unset

		    	if ( TempChatSend[index] )
			    {
				    TempChatSend[index] = 0;
                   TempChatSend[0] = 0;    
   			}
	    		else
		    	{
			    	TempChatSend[index] = 1;
   			}

	    	}

   	}


   	//---- If any single player is set then reset the All Players field


	    for ( i=1; i < MAX_PLAYERS+1; i++ )
   	{
		    if ( AMultiPlayer.GetAPlayerId ( index - 1 ) != kNoPlayer )
           {
   	    	if ( !TempChatSend[i] )
	    	    {
                   iOk = 0;
	        		break;
   	    	}
           }
	    }	



   	//---- There are not any single players set so set All Players

	    if ( iOk )
   	{
	    	TempChatSend[0] = 1;
   	}


   }

#endif


   UpdateChatValues();


}


/* ========================================================================
   Function    - ToggleChatRec
   Description - 
   Returns     -
   ======================================================================== */
void ToggleChatRec( LONG index, LONG old )
{

	int i;
	int iOk = 1;


#ifdef _WINDOWS

	if ( IsMultiPlayer() )
	{


   	//---- Selected all

   	if ( index == 0 )
	    {
   		TempChatRec[0] = 1;					//---- Set All Players

	    	for ( i = 1; i < MAX_PLAYERS+1; i++ )
		    {
			    TempChatRec[i] = 1;				//---- Reset single players
   		}	

	    }
   	else  	//----- Select a single / multiplayers
	    {

		    if ( AMultiPlayer.GetAPlayerId ( index - 1 ) != kNoPlayer )
   		{

	    		//---- unset

		    	if ( TempChatRec[index] )
			    {
				    TempChatRec[index] = 0;
                   TempChatRec[0] = 0;    
   			}
	    		else
		    	{
			    	TempChatRec[index] = 1;
   			}

	    	}

   	}


   	//---- If any single player is set then reset the All Players field


	    for ( i=1; i < MAX_PLAYERS+1; i++ )
   	{
		    if ( AMultiPlayer.GetAPlayerId ( index - 1 ) != kNoPlayer )
           {
   	    	if ( !TempChatRec[i] )
	    	    {
                   iOk = 0;
	        		break;
   	    	}
           }
	    }	


   	//---- There are not any single players set so set All Players

	    if ( iOk )
   	{
	    	TempChatRec[0] = 1;
   	}

   
   }

#endif


   UpdateChatValues();


}


#define MAX_INPUT_PIXEL 380
#define MAX_INPUT_CHAR  60


/* ========================================================================
   Function    - CleanupChatSelectWindow
   Description - 
   Returns     -
   ======================================================================== */

void CleanupChatSelectWindow(void)
{

   //---- remove the regions we added


#ifdef _WINDOWS

	if ( IsMultiPlayer() )
	{

   	del_region(ToggleChatSend,0);
	    del_region(ToggleChatRec,0);

   }

#endif


//	HidePanel( D_CHAT_SELECT );
//	RunPanels();


	HideMenu( D_CHAT_SELECT );
	RunMenus();


}



/* ========================================================================
   Function    - ShowChatSelectWindow
   Description - Show the chat select window
   Returns     -
   ======================================================================== */
void ShowChatSelectWindow(LONG, LONG)
{


	//----- Just in case join dialog is goofed up

	SetupChatSelect ();


#ifdef _WINDOWS
	int i;
	

	if ( IsMultiPlayer() )
	{

		//---- Setup our check boxes

		for ( i = 0; i < MAX_PLAYERS+1; i++ )
		{
           //---- Fill in the current send receive options 

		    TempChatSend[i] = AMultiPlayer.GetChat( i );
		    TempChatRec[i]  = AMultiPlayer.GetChatRec( i );
		}
	}

#endif


	if ( IsMenuActive(D_CHAT_SELECT) )
	{
   	CleanupChatSelectWindow();
   	return;
	}
	else
	{
	//	ShowPanel(D_CHAT_SELECT);
	
		ShowMenu(D_CHAT_SELECT);
	   RunMenus();
	}



#ifdef _WINDOWS

	if ( IsMultiPlayer() )
	{

		//---- Setup our check boxes

		for ( i = 0; i < MAX_PLAYERS+1; i++ )
		{
       	LONG mx, my;

           //---- Get some line offsets for the check boxes

   		GetButtonPosition( D_CHAT_SELECT, i+2, &mx, &my );

           add_region( mx-CS_TALK_OFF, my, 
                       10,12,0,ToggleChatSend,i,0,0, STR_NULL);

           add_region( mx-CS_LISTEN_OFF, my, 
                       10,12,0,ToggleChatRec,i,0,0, STR_NULL);

		}
	}

#endif

}



/* ========================================================================
   Function    - DoneChatSelect
   Description - Done with chat select Update structure
   Returns     -
  ======================================================================== */
void DoneChatSelect( LONG MenuCombo, LONG )
{
	int i;

   DoButtonMove ( MenuCombo );


   //---- If something is in the chat box send it

   if ( strlen( chatbuff) > 1 )
   {

       chatbuff[strlen(chatbuff)-1] = 0;
           

#ifdef _WINDOWS
  		AMultiPlayer.Chat(chatbuff);
#endif

 	    memset(chatbuff, 0, sizeof(chatbuff));

   }


   UpdateChatValues();


   CleanupChatSelectWindow();

}



/* ========================================================================
   Function    - CancelChatSelect
   Description - Cancel chat select screen
   Returns     -
   ======================================================================== */
void CancelChatSelect(LONG MenuCombo, LONG )
{

	LONG	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);


#ifdef _WINDOWS

   DoButtonMove ( MenuCombo );

   //---- Reset chat input buffer 

   memset(chatbuff, 0, sizeof(chatbuff));


   CleanupChatSelectWindow();

#endif


}



#define OUR_YELLOW 173

/* ========================================================================
   Function    - PaintChatSelect()
   Description - Paint the chat select window
   Returns     -
   ======================================================================== */

void PaintChatSelect(LONG MenuCombo, LONG)
{

	LONG MenuId, ButtonId;
	LONG mx, my;
   char buffer[100];
	int i;
	int color;
	int x;
	int done;



	SPLIT_LONG(MenuCombo, MenuId, ButtonId);


	//---- Title screen

	GetButtonPosition( MenuId, 0, &mx, &my );
	init_gfont(FONT_TITL_40PT);
   sprintf(buffer, "^N%s", STRMGR_GetStr(STR_CHATSEL_TITLE));
	print_textf(mx + 25 + 48, my + 23, 0, buffer);



   //---- Paint input field

	init_gfont(FONT_SANS_10PT);

	GetButtonPosition( MenuId, 1, &mx, &my );

	gprint_text( mx, my, chatbuff, WHITE );

	//---- Reset buffer

	done = PaintInputField( MenuId, chatbuff, 
                           MAX_INPUT_CHAR+2, 
                           -1, TRUE, 1, 0, 0 );


   //
   //---- if done, send the string and clear the buffer 2 means cheat code
   //

	if ( done )
	{

#ifdef _WINDOWS
		if ( done == 1 )
			AMultiPlayer.Chat(chatbuff);
#endif

	    memset(chatbuff, 0, sizeof(chatbuff));


     	if ( done == 2 )
			cheated++;
   
   }
   else    //---- If pixel is > max or max line length reach the auto send
   {
       if ( ( gtext_width ( chatbuff ) >= MAX_INPUT_PIXEL ) || 
            ( strlen( chatbuff) >=  MAX_INPUT_CHAR )           )
       {
           
           chatbuff[strlen(chatbuff)-1] = 0;

#ifdef _WINDOWS
    		AMultiPlayer.Chat(chatbuff);
#endif

   	    memset(chatbuff, 0, sizeof(chatbuff));

       }

   }



   //
   //---- If this is a multiplayer game then write each player out 
   //

#ifdef _WINDOWS

	if ( IsMultiPlayer() )
	{
       int last = 2;

   	init_gfont(FONT_SANS_8PT);

	    for ( i=2; i < 10; i++ )
   	{
	    	DWORD dwId;

   		buffer[0] = 0;

   		GetButtonPosition( MenuId, i, &mx, &my );
   

          	//---- All Players Line

   		if ( i == 2 )
   		{
   			sprintf(buffer, "%s", STRMGR_GetStr(STR_CHATSEL_2) );
   
               iMPCheck[0]= GetResourceStd("UI\\DCHECK1.PCX", FALSE);
               iMPCheck[1]= GetResourceStd("UI\\DCHECK2.PCX", FALSE);
   
               DrawBitmap( mx-CS_TALK_OFF, my, 
                           iMPCheck[TempChatSend[0]],0,0,20,20 );
   
               DrawBitmap( mx-CS_LISTEN_OFF, my,
                           iMPCheck[TempChatRec[0]] ,0,0,20,20 );

               if (iMPCheck[0]!=fERROR) SetPurge(iMPCheck[0]);	iMPCheck[0]=fERROR;
               if (iMPCheck[1]!=fERROR) SetPurge(iMPCheck[1]);	iMPCheck[1]=fERROR;
   
   		}
   		else  //---- Other players 
   		{
   
   			//---- Get Player info ( Name etc )
   
   			dwId = AMultiPlayer.GetAPlayerId ( i - 3 );
   
   			if ( dwId != kNoPlayer )
   			{
   				int rindex;

   				rindex = AMultiPlayer.GetRealmFromId( dwId  );

                   last = i;
                   

                   //---- Get the check box art 
   
   	            iMPCheck[0]= GetResourceStd("UI\\DCHECK1.PCX", FALSE);
                 	iMPCheck[1]= GetResourceStd("UI\\DCHECK2.PCX", FALSE);
   

                   //---- Draw the check box 

                   DrawBitmap( mx-CS_TALK_OFF, my,
                              iMPCheck[TempChatSend[i-2]],0,0,20,20 );
   
                   DrawBitmap( mx-CS_LISTEN_OFF, my,
                              iMPCheck[TempChatRec[i-2]] ,0,0,20,20 );
   

                   //---- release the box art

                 	if (iMPCheck[0]!=fERROR) SetPurge(iMPCheck[0]);	iMPCheck[0]=fERROR;
                	if (iMPCheck[1]!=fERROR) SetPurge(iMPCheck[1]);	iMPCheck[1]=fERROR;


   
                   //---- Do the realm thing baby

   				if ( rindex < LAND_REALM_COUNT )
   				{
   					int temp;
   					char cpRealmName[80];


                       //---- Shields up

               		iMPDCtrlIc = GetResourceStd("UI\\DCTRL_IC.PCX", FALSE);
	    	            iMPDCtr2Ic = GetResourceStd("UI\\DCTR2_IC.PCX", FALSE);

   	    			DrawShield( mx-15, my, rindex);

              	        if (iMPDCtrlIc!=fERROR) SetPurge(iMPDCtrlIc); iMPDCtrlIc=fERROR;
           	        if (iMPDCtr2Ic!=fERROR) SetPurge(iMPDCtr2Ic); iMPDCtr2Ic=fERROR;
   

                       //---- Add some descriptive text 

   					strcpy(cpRealmName, realm[rindex].mfGetName() );

   					temp = sprintf( buffer,
   							 "%s %s",
   							 AMultiPlayer.GetOpponentName(i - 3),
   							 STRMGR_GetStr(STR_CHATSEL_AS) );
   
   					// [d4-09-97 JPC] Modified the following to use the access
   					// function instead of direct access to the obsolete .name member
   					// of the REGENT class.

   					sprintf( &buffer[temp],					
   							 " %s%s %s",
   	                	     regents[realm[rindex].mfGetRegent()].mfGetname(),
   							 STRMGR_GetStr(STR_CHATSEL_RO),
   							 cpRealmName );
   				}
   				else //---- Not selected yet
   				{
   
   					sprintf( buffer,
   							 "%s %s",
   							 AMultiPlayer.GetOpponentName(i - 3),
   							 STRMGR_GetStr(STR_MULTIWAIT_NOT) );
   				
   				}
   			}
   		}

           //---- Display character info 

   		gprint_text(mx, my, buffer, 200 );

   	}


       //---- print out talk and listen titles  

   	init_gfont(FONT_SANS_6PT);

  		GetButtonPosition( MenuId, last, &mx, &my );

       print_textf( mx-30, 
                    my+25, 200, "^A^L%s", STRMGR_GetStr(STR_TALK) );

       print_textf( mx, 
                    my+25, 200, "^A^L%s", STRMGR_GetStr(STR_LISTEN) );

   }

#endif




}



/* ========================================================================
   Function    - AddInputSpace
   Description - Adds space at end of string that input routine expects
   Returns     -
   ======================================================================== */

void AddInputSpace ( int type )
{
	int i;

#ifdef _WINDOWS

	switch ( type )
	{
		case TYPE_NAME:
			//---- Pre fill name for player entry

			strcpy ( szTempName, AMultiPlayer.GetPlayerName() );

			//---- Last char needs to be diff
			if ( szTempName[(i = strlen(szTempName))-1] != ' ' )
			{
				szTempName[i]   = ' ';
				szTempName[i+1] = 0;
			}
			break;

		case TYPE_GAME:
			//---- Pre fill name for game entry

			strcpy ( szTempGame, AMultiPlayer.GetGameName() );

			//---- Last char needs to be diff
			if ( szTempGame[(i=strlen(szTempGame))-1] != ' ' )
			{
				szTempGame[i]   = ' ';
				szTempGame[i+1] = 0;
			}
		    break;

		case TYPE_NUMBER:
			//---- Pre fill number entry

           if ( AMultiPlayer.GetNumPlayers() < 2 ||
                AMultiPlayer.GetNumPlayers() > 8    )
           {
   			sprintf ( szTempNumber, "%1.1d", 2 );
           }
           else
           {
   			sprintf ( szTempNumber, "%1.1d", AMultiPlayer.GetNumPlayers() );
           }

           
			//---- Last char needs to be diff
			if ( szTempNumber[(i=strlen(szTempNumber))-1] != ' ' )
			{
				szTempNumber[i]   = ' ';
				szTempNumber[i+1] = 0;
			}
			break;

	}
#endif

}


/* ========================================================================
   Function    - RemoveInputSpace
   Description - remove space at end of string that input routine needed
   Returns     -
   ======================================================================== */

void RemoveInputSpace ( int type )
{

#ifdef _WINDOWS

	switch ( type )
	{
		case TYPE_NAME:
			if ( szTempName[strlen(szTempName)-1] == ' ' ||
			     szTempName[strlen(szTempName)-1] == '_'    )
			{
				szTempName[strlen(szTempName)-1] = 0;
			}
			break;

		case TYPE_GAME:
			if ( szTempGame[strlen(szTempGame)-1] == ' ' ||
			     szTempGame[strlen(szTempGame)-1] == '_'    )
			{
				szTempGame[strlen(szTempGame)-1] = 0;
			}
			break;

		case TYPE_NUMBER:
			if ( szTempNumber[strlen(szTempNumber)-1] == ' ' ||
			     szTempNumber[strlen(szTempNumber)-1] == '_'    )
			{
				szTempNumber[strlen(szTempNumber)-1] = 0;
			}
			break;
	}

#endif

}


/* ========================================================================
   Function    - MultiPlayerNewGame
   Description - Setup for a new game
   Returns     -
   ======================================================================== */
void MultiPlayerNewGame (void)
{
	// set the proc to null
	SetButtonProc  (D_QUESTION2, 0, NULL, 0, 0 );
	
	// set the question text
	SetButtonLabel  (D_QUESTION2, QUESTION_TEXT, STR_MAIN1_YN_QUESTION_NEW, BLACK );
	
	// turn on the first button as YES
	SetButtonLabel  (D_QUESTION2, QUESTION_BTN1, STR_YES, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION2, QUESTION_BTN1, MultiPlayerStartGame, QUESTION_BTN1, D_KEY_YES );
	
	// turn on the second button as CANCEL
	SetButtonLabel  (D_QUESTION2, QUESTION_BTN2, STR_CANCEL, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION2, QUESTION_BTN2, MultiPlayerStartGame, QUESTION_BTN2, D_KEY_CANCEL );
	
	ShowMenu(D_QUESTION2);
}

/* ========================================================================
   Function    - MultiPlayerStartGame
	Description - click routine for new game dialog
	Returns     - 
	======================================================================== */
void MultiPlayerStartGame( LONG MenuCombo, LONG arg )
{
	LONG	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	SetButtonHilight(MenuId, ButtonId, TRUE);
	RunMenus();
	update_screen();
	TickDelay(4);
	SetButtonHilight(MenuId, ButtonId, FALSE);
	RunMenus();
	update_screen();
	TickDelay(4);
	HideMenu(MenuId);
	RunMenus();

	if (arg==QUESTION_BTN1)
		gMPStartGame = TRUE;
	else
		gMPStartGame = FALSE;
	
	gMPStartGameLoop = FALSE;
}	

/* ========================================================================
   Function    - ShowMultiplayerMenu
   Description - Show the multiplayer menu
   Returns     -
   ======================================================================== */
void ShowMultiPlayerMenu(LONG, LONG)
{
	if ( mouse_button == 2 )
	{
		SystemHelp( STR_MAIN1_HELP_MULTI_TITLE,
		STR_MAIN1_HELP_MULTI_TEXT, H_Multiplayer, 0, 0 );
	}
	else //---- We need help
	{
		if (!fDomainTurn)		// only allowed to start at the map level
			return;

#ifdef _WINDOWS
		char pszTemp[MAX_PATH];
		
		//GEH
		// if a game already in progress, we must ask if they want
		// to end the game or go back
		if (char_selected)
		{
			gMPStartGame = FALSE;
			gMPStartGameLoop = TRUE;
			MultiPlayerNewGame();
			// call the idle loop until the flag differs
			// NOTE: we won't leave this routine until a field is selected
			while( gMPStartGameLoop )
			{
				MenuLoop();
			}
			
			// user choose to bail out and return to his game
			if (gMPStartGame == FALSE)
				return;
			// no going back now...
			char_selected = FALSE;
		}
		
		// no going back now...
		// MultiResetRealmSelect();
		
		GAMEToggleMainMenu(FALSE,0);	//---- shutdown main menu
		
		AMultiPlayer.Finalize(1);
		
		HidePanel(D_BUILD_ARMY);
		HidePanel(D_BUILD_ADV);
		HidePanel(D_ADVENTURE_PREP);
		
		//---- wait for menu to minimize
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		
		//GEH 8/5/97
		// reload virgin copies of the .ava and .inv files
		//StatReadAll();
		//WriteRealmStartData();
		//GEH 8/25/97
		// from a multiplayer rlmstart.dat file
		
		//GEH 7/78/97
		// release a running scene, calls InitDomainTurn
		if (fDomainTurn == FALSE)
		{
			master_game_type = GAME_NORMAL;
			dturn_mode = START_NEW_OR_LOAD;
			SCENE_MGR::mfReleaseSceneToMap(0,0);
		}
		else
		{
			// exit and release current domain turn
			if (iLgMap != fERROR)		// if domain turn IS active
				InitDomainTurn(0);

			// start a new domain turn UI
			fDomainTurn = FALSE;
			master_game_type = GAME_NORMAL;
			dturn_mode = START_NEW_OR_LOAD;
			InitDomainTurn(0);
		}

		SetRedrawMainMapLevel();
		fFadedOut = -1;
		
		//GEH if (!fDomainTurn)				// you're in adventure or battle?!
		//GEH {
		//GEH 	master_game_type = GAME_NORMAL;
		//GEH 	dturn_mode = START_NEW_OR_LOAD;
		//GEH 	SCENE_MGR::mfReleaseSceneToMap(0,0);	// return to map level
		//GEH }
		
		fPractice = FALSE;
		fTutorialSelected = FALSE;
		fTutorialFirstBattle = FALSE;
		
		//---- Means no web browser for url types
		if ( FindExecutable ( "birth.url", NULL, pszTemp ) <= (HANDLE) 32 )
		{
			fWeb = FALSE;
		}
		
		SetMultiPlayerEnvironment( 0, 0 );
		ShowMenu(D_MULTIENV);
		
#else
		return;    //---- Not in DOS Should never be here in DOS
#endif
	}
}

/* ========================================================================
   Function    - DispMultiOpts
   Description - displays current options ( bottom part of menu )
   Returns     -
   ======================================================================== */
void DispMultiOpts(int fProt, LONG MenuId )
{
	LONG mx, my;
	char buffer[100];
	char buffer1[100];

#ifdef _WINDOWS

	init_gfont(FONT_SANS_8PT);


	//---- Name

	GetButtonPosition( MenuId, 5, &mx, &my );
	sprintf( buffer, "%s %s",
			 STRMGR_GetStr(STR_MULTIMENU_NAME),
			 AMultiPlayer.GetPlayerName() );

	gprint_text(mx, my, buffer, OUR_YELLOW );


	//---- Protocol

	GetButtonPosition( MenuId, 6, &mx, &my );

	sprintf( buffer, "%s %s",
			 STRMGR_GetStr(STR_MULTIMENU_PROT),
			 AMultiPlayer.GetProtocol() );

	gprint_text( mx, my, buffer, OUR_YELLOW );


	//---- Game

	GetButtonPosition( MenuId, 7, &mx, &my );

	sprintf( buffer, "%s %s",
			 STRMGR_GetStr(STR_MULTIMENU_GAME),
			 AMultiPlayer.GetGameName() );

	gprint_text(mx, my, buffer, OUR_YELLOW );


	//---- Players

	GetButtonPosition( MenuId, 8, &mx, &my );

	sprintf( buffer, "%s %d",
			 STRMGR_GetStr(STR_MULTIMENU_PLAYR),
			 AMultiPlayer.GetNumPlayers() );

	gprint_text(mx, my, buffer, OUR_YELLOW );




#if 0
	//---- Speed
	GetButtonPosition( MenuId, 9, &mx, &my );
	sprintf( buffer1, "%s",
			 STRMGR_GetStr(STR_MULTIMENU_SPEED) );
	sprintf( buffer, "%s %s",
			 buffer1,
			 STRMGR_GetStr( STR_MULTIMENU_CSLS + AMultiPlayer.GetSpeed() ) );
	gprint_text( mx, my, buffer, OUR_YELLOW );
#endif




#endif

}



#ifdef _WINDOWS


void SetScrollOff(void )
{

   //---- Blank scroll buttons off, first 

   SetButtonArt ( D_MULTIJOIN, MP_SUP, "UI\\MLTBTNUC.PCX" );
   SetButtonArt ( D_MULTIJOIN, MP_SDOWN, "UI\\MLTBTNDC.PCX" );

}

/* ========================================================================
   Function    - SetScrollButtons
   Description - set scroll buttons for joining games 
   Returns     -
   ======================================================================== */
void SetScrollButtons ( void )
{


  	if ( m_mode == MENU_JOINDONE )
   {

       SetButtonArt ( D_MULTIJOIN, MP_SUP, "UI\\MLTBTNUA.PCX" );
       SetButtonArt ( D_MULTIJOIN, MP_SDOWN, "UI\\MLTBTNDA.PCX" );
   }
   else
   {
      SetScrollOff();
   }

}

#endif


/* ========================================================================
   Function    - GetMultiPlayerHelp
   Description - Get multiplayer help.
   Returns     -
   ======================================================================== */
void GetMultiPlayerHelp( LONG, LONG )
{


#ifdef _WINDOWS

   if ( mouse_button == 2 )
   {

   	//---- clicked on something

	    switch ( m_mode )
   	{
   		case MENU_TYPESEL:
          	    SystemHelp( STR_MULTITYPE_SEL,
                           STR_MULTITYPE_HELP_SEL_TEXT, H_Multiplayer, 0, 0 );
               break;


	    	case MENU_INIT:
           	SystemHelp( STR_MULTIMENU_SEL,
                           STR_MULTIMENU_HELP_SEL_TEXT, H_Select_Game_Environment, 0, 0 );
   			break;


   		case MENU_COMP:
             	SystemHelp( STR_MULTIMENU_YN,
                           STR_MULTIMENU_HELP_YN_TEXT, H_Multiplayer, 0, 0 );
   			break;


   		case MENU_NAME:
             	SystemHelp( STR_MULTIMENU_YN,
                           STR_MULTIMENU_HELP_YN_TEXT, H_Multiplayer, 0, 0 );
   			break;

   		case MENU_PROT:
             	SystemHelp( STR_MULTIMENU_SP,
                           STR_MULTIMENU_HELP_SP_TEXT, H_Select_Protocol, 0, 0 );
   			break;

#if 0
   		case MENU_CONNSPD:
             	SystemHelp( STR_MULTIMENU_CS,
                           STR_MULTIMENU_HELP_CS_TEXT, H_Set_Connection_Speed, 0, 0 );
   			break;
#endif

		    case MENU_JOINNEW:
       	    SystemHelp( STR_MULTIMENU_JN,
                           STR_MULTIMENU_HELP_JN_TEXT, H_Multiplayer, 0, 0 );
   			break;

	    	case MENU_JOINDONE:
           	SystemHelp( STR_MULTIMENU_SJ,
                           STR_MULTIMENU_HELP_SJ_TEXT, H_Multiplayer, 0, 0 );
   			break;

	    	case MENU_NEWNAME:
           	SystemHelp( STR_MULTIMENU_NGP,
                           STR_MULTIMENU_HELP_NGP_TEXT, H_Multiplayer, 0, 0 );
   			break;

	    	case MENU_NEWNO:
           	SystemHelp( STR_MULTIMENU_NP,
                           STR_MULTIMENU_HELP_NP_TEXT, H_Multiplayer, 0, 0 );
   			break;

//	    	case MENU_NEWDONE:
//           	SystemHelp( STR_MAIN1_HELP_MULTI_TITLE,
//                           STR_MAIN1_HELP_MULTI_TEXT, H_Multiplayer, 0, 0 );
//   			break;

	    	default:
		    	break;
   	}

   }

#endif


	return;



}  //---- End of GetMultiPlayerHelp()


/* ========================================================================
   Function    - ScrollMultiPlayerMenu
   Description - Back up one menu
   Returns     - 0 is up 1 is down
   ======================================================================== */
void ScrollMultiPlayerMenu(LONG MenuCombo, LONG down )
{

#ifdef _WINDOWS

   if ( mouse_button == 2 )
   {
       GetMultiPlayerHelp ( 0, 0 );
   }
	else
	{
   	if ( m_mode == MENU_JOINDONE )
	   {


           DoButtonMove ( MenuCombo );


           if ( !down )
           {
              if ( iGameTop )
                  iGameTop--;
           }
           else
           {
               if ( (iNumGames - iGameTop)  > MP_SL )
               {
                   iGameTop++;
               }
           }

           SetScrollButtons();

   	}


	}


#endif

}


/* ========================================================================
   Function    - PaintWaitingMenu
   Description -
   Returns     -
   ======================================================================== */

void PaintWaitingMenu( LONG MenuCombo, LONG )
{
	int i;
	int numplayers;


	// set the statement text
	LONG	mx,my;
	LONG	xOff = 0;
	LONG	yOff = 0;
	LONG	MenuId, ButtonId;
	char	textbuf[1024];

	char *  ptemp;

	LONG X;
	LONG Y;
	LONG W;
	LONG H;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	X = X_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].X);
	Y = Y_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].Y);
	W = X_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].W);
	H = Y_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].H);

	xOff = Menus[MenuId].Buttons[0].X;
	yOff = Menus[MenuId].Buttons[0].Y;



#ifdef _WINDOWS


	numplayers = AMultiPlayer.WaitForPlayers();


	//---- All Players have joined continue on

	if ( kHaveAll == numplayers )
	{

		//---- wait for menu to minimize

  		HideSubMenu( 0, MenuId );
   
		RunMenus();

		//---- Enumerate Player names

		AMultiPlayer.GetPlayers ();

		AMultiPlayer.InitPost();

		if ( AMultiPlayer.IsMaster() )
		{
			SetupMultiPlayerHost();
			ShowMenu(D_MULTIHOST);
		}
		else  //---- put up waiting dialog
		{
			ShowWaitMultiType(0, 0);
			RunMenus();
		}

		return;

	}


   //---- We cant use IsMaster() until after InitPost() so cheat a little

	if ( fStartGame && numplayers > 1 )
   {
   	SetButtonLabel  ( MenuId, QUESTION_BTN2, STR_START1, BTN_LABEL_COLOR );  //---- turn on done button
   	SetButtonHilight( MenuId, QUESTION_BTN2, FALSE );
   }
   else
   {   
       if ( fStartGame )
           SetButtonLabel( MenuId, QUESTION_BTN2, STR_START1, GREY );  //---- greyed button
   }


	{
		int temp;

		temp = sprintf( textbuf, "%s %d ",
   		 			STRMGR_GetStr(STR_MULTIJOIN_1),
			     		AMultiPlayer.GetNumPlayers() - numplayers );

		sprintf( &textbuf[temp],
				 "%s",
 				 STRMGR_GetStr(STR_MULTIJOIN_2) );

	}

	// paint the control buttons

	init_gfont(FONT_SANS_12PT);

	print_text_centered(
		X + xOff + (W/2),
		Y + yOff + (H/2) + 1,
		textbuf,
		Menus[MenuId].Buttons[QUESTION_TEXT].LabelColor
		);



#endif

}


/* ========================================================================
   Function    - CancelWaitingMenu
   Description -
   Returns     -
   ======================================================================== */
void CancelWaitingMenu(LONG MenuCombo, LONG)
{

	LONG	MenuId, ButtonId;

	SPLIT_LONG( MenuCombo, MenuId, ButtonId );


   DoButtonMove ( MenuCombo );



#ifdef _WINDOWS
	AMultiPlayer.Finalize();
#endif


	HideSubMenu(0, MenuId );

	//---- wait for menu to minimize

	RunMenus();


	GAMEToggleMainMenu(TRUE,0);  //---- start up main menu


}



/* ========================================================================
   Function    - StartWaitingMenu
   Description -
   Returns     -
   ======================================================================== */
void StartWaitingMenu(LONG MenuCombo, LONG)
{
#ifdef _WINDOWS
	int numplayers;

	numplayers = AMultiPlayer.WaitForPlayers();

	if ( fStartGame && numplayers > 1 )
	{
		DoButtonMove ( MenuCombo );
		
		//----- Do Force Start game HERE
		AMultiPlayer.ForceStart();
		
		HideMenu( D_QUESTION2 );
		//---- wait for menu to minimize
		RunMenus();
		
		//---- Enumerate Player names
		AMultiPlayer.GetPlayers();
		AMultiPlayer.InitPost();
		ShowMenu(D_MULTIHOST);
		RunMenus();
	}

#endif

}




/* ========================================================================
   Function    - PaintWaitingTypeMenu
   Description - Wait for game type from master
   Returns     -
   ======================================================================== */

void PaintWaitingTypeMenu( LONG MenuCombo, LONG )
{

	// set the statement text
	LONG	mx,my;
	LONG	xOff = 0;
	LONG	yOff = 0;
	LONG	MenuId, ButtonId;
	char	textbuf[1024];

	char *  ptemp;

	LONG X;
	LONG Y;
	LONG W;
	LONG H;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	X = X_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].X);
	Y = Y_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].Y);
	W = X_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].W);
	H = Y_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].H);

	xOff = Menus[MenuId].Buttons[0].X;
	yOff = Menus[MenuId].Buttons[0].Y;



#ifdef _WINDOWS


	AMultiPlayer.LookForMessages();		//---- Check for messages

   //---- Game was cancelled

   if ( !IsMultiPlayer() )
   {

		//---- Lets get out of here

		HideSubMenu( 0, D_QUESTION1 );

		//---- wait for menu to minimize

		RunMenus();

   	GAMEToggleMainMenu(TRUE,0);  //---- start up main menu

       return;

   }

	//---- Game type has been sent

	if ( AMultiPlayer.GetGameType() != iMPGT_NONE )
	{

		//---- Lets get out of here

		HideSubMenu( 0, D_QUESTION1 );

		//---- wait for menu to minimize

		RunMenus();


		//@@@@@ depending on mode we should do something here

		AMultiPlayer.InitPost2();

		return;

	}


	sprintf( textbuf, "%s",
			 STRMGR_GetStr(STR_MULTITYPE_WAIT) );



	// paint the control buttons

	init_gfont(FONT_SANS_12PT);

	print_text_centered(
		X + xOff + (W/2),
		Y + yOff + (H/2) + 1,
		textbuf,
		Menus[MenuId].Buttons[QUESTION_TEXT].LabelColor
		);


#endif


}



/* ========================================================================
   Function    - CancelWaitingTypeMenu
   Description - Cancel wait for master game type message
   Returns     -
   ======================================================================== */
void CancelWaitingTypeMenu(LONG MenuCombo, LONG)
{
   DoButtonMove ( MenuCombo );

#ifdef _WINDOWS

	AMultiPlayer.Finalize();

#endif

	HideSubMenu(0, D_QUESTION1 );

	//---- wait for menu to minimize

	RunMenus();

	GAMEToggleMainMenu(TRUE,0);  //---- start up main menu
}


/* ========================================================================
   Function    - PaintWaitingRealmMenu
   Description -
   Returns     -
   ======================================================================== */
void PaintWaitingRealmMenu( LONG MenuCombo, LONG )
{
	LONG MenuId, ButtonId;
	char buffer[100];
	LONG mx, my;
	int i;
	int numplayers;
	int fDone = TRUE;

	SPLIT_LONG(MenuCombo, MenuId, ButtonId );

	//---- Title screen

	GetButtonPosition( MenuId, 0, &mx, &my );
	init_gfont(FONT_TITL_40PT);
   sprintf(buffer, "^N%s", STRMGR_GetStr(STR_CHATSEL_TITLE));
	print_textf(mx + 25 + 48, my + 23, 0, buffer);



	//--- Select Players Line

	init_gfont(FONT_SANS_8PT);

//	GetButtonPosition( MenuId, 1, &mx, &my );
//	sprintf( buffer, "%s", STRMGR_GetStr(STR_MULTIWAIT_REALM) );
//	gprint_text(mx, my, buffer, WHITE );


#ifdef _WINDOWS


	for ( i = 0; i < MAX_PLAYERS; i++ )
	{
		DWORD dwId;
		int rindex;


		//---- Get Player info ( Name etc )

		dwId = AMultiPlayer.GetAPlayerId ( i );


		if ( dwId != kNoPlayer )
		{

			rindex = AMultiPlayer.GetRealmFromId( dwId  );

			//---- If they have selected a realm

			if ( rindex < LAND_REALM_COUNT )
			{
				int temp;
				char cpRealmName[80];

				temp = sprintf( buffer,
								 "%s %s",
								 AMultiPlayer.GetOpponentName(i),
								 STRMGR_GetStr(STR_CHATSEL_AS) );

				 strcpy(cpRealmName, realm[rindex].mfGetName() );
				// [d4-09-97 JPC] Modified the following to use the access
				// function instead of direct access to the obsolete .name member
				// of the REGENT class.
				sprintf( &buffer[temp],
						 " %s%s %s",
               	     regents[realm[rindex].mfGetRegent()].mfGetname(),
						 STRMGR_GetStr(STR_CHATSEL_RO),
						 cpRealmName);
			}
			else
			{
				sprintf( buffer,
						 "%s %s",
						 AMultiPlayer.GetOpponentName(i),
						 STRMGR_GetStr(STR_MULTIWAIT_NOT) );

				fDone = FALSE;
			}


			GetButtonPosition( MenuId, i+1, &mx, &my );

			gprint_text(mx, my, buffer, WHITE );

		}

	}




	//---- Everyone has selected a realm

	if ( fDone )
	{

		HideSubMenu(0, D_MULTIWAIT );

		//---- wait for menu to minimize

		RunMenus();

		MultiSetAdvSite();

		BeginRule2(0,0);

	}

#endif

}




/* ========================================================================
   Function    - CancelWaitingRealmMenu
   Description -
   Returns     -
   ======================================================================== */
void CancelWaitingRealmMenu(LONG MenuCombo, LONG)
{


   DoButtonMove ( MenuCombo );


#ifdef _WINDOWS

	AMultiPlayer.Finalize();

#endif

	HideSubMenu(0, D_MULTIWAIT );

	//---- wait for menu to minimize

	RunMenus();


	GAMEToggleMainMenu(TRUE,0);  //---- start up main menu


}

/* ========================================================================
   Function    - PaintVoteWait()
   Description - Paint the vote wait screen
   Returns     -
   ======================================================================== */
void PaintVoteWait(LONG MenuCombo, LONG)
{

	LONG MenuId, ButtonId;
	LONG mx, my;
	char buffer[100];
	int i;
	int color;
	int x;
	int fDone = TRUE;
	int data1;
	int data2;
	int data3;
	int data4;

	SPLIT_LONG(MenuCombo, MenuId, ButtonId);



	//---- Title screen

	GetButtonPosition( MenuId, 0, &mx, &my );
	init_gfont(FONT_TITL_40PT);
   sprintf(buffer, "^N%s", STRMGR_GetStr(STR_GM_TT_ACTION1));
	print_textf(mx + 25 + 48, my + 23, 0, buffer);




	//--- Select Players Line

	init_gfont(FONT_SANS_8PT);

//	GetButtonPosition( MenuId, 1, &mx, &my );
//	sprintf( buffer, "%s", STRMGR_GetStr(STR_PLAYER_WAIT) );
//	gprint_text(mx, my, buffer, WHITE );




#ifdef _WINDOWS


	for ( i = 0; i < MAX_PLAYERS; i++ )
	{
		DWORD dwId;


		//---- Get Player info ( Name etc )

		dwId = AMultiPlayer.GetAPlayerId ( i );


		//---- Got one

		if ( dwId != kNoPlayer )
		{

			//---- Have they replied

           if ( AMultiPlayer.GetAEventReply( i, &data1, &data2, &data3, &data4 ) )
			{
				sprintf( buffer,
						 "%s, %s",
						 AMultiPlayer.GetOpponentName(i),
						 data1 ? STRMGR_GetStr(STR_VOTEPOS) : STRMGR_GetStr(STR_VOTENEG));
			}
			else
			{
				sprintf( buffer,
						 "%s %s",
						 AMultiPlayer.GetOpponentName(i),
						 STRMGR_GetStr(STR_VOTEWAIT) );

				fDone = FALSE;
			}

			GetButtonPosition( MenuId, i+1, &mx, &my );

			gprint_text(mx, my, buffer, WHITE );

		}

	}



	//---- Everyone has selected a realm

	if ( fDone )
	{
		int i;
		int tyes = 1;
		int tno = 0;


		for ( i = 0; i < MAX_PLAYERS; i++ )
		{
			DWORD dwId;


			//---- Get Player info ( Name etc )

			dwId = AMultiPlayer.GetAPlayerId ( i );


			//---- Got one

			if ( dwId != kNoPlayer )
			{

           	if ( AMultiPlayer.GetAEventReply( i, &data1, &data2, &data3, &data4 ) )
				{
				
				    if ( data1 )
						tyes++;
					else
						tno++;

				}

			}
		}


		HideSubMenu(0, D_MULTIWAIT );
	
		//---- wait for menu to minimize

		RunMenus();


		//---- has the vote passed

		if ( tyes > tno )
		{
			SelectAdvParty();

// bad do this after if EventStarter	action_turn++;

		}
		else
		{
			//---- Tell other players event was rejected

			AMultiPlayer.ResultsEvent( iMPE_ADVENTURE,
						        	   0, 0, 0, 0, 0 );
			
			IncActionTurn();

			if (fLTAction == DURING_LTACTION)	// return to normal action type
			{
				ActiveRegent = realm[HomeRealm].mfGetRegent();
				fLTAction = AFTER_LTACTION;
			}
//Thanks			
//			action_turn++;		  //---- Will this trigger continue ?
		}

	}


#endif


}




/* ========================================================================
   Function    - PaintWaitingSyncMenu
   Description - Paint waiting for other players to sync
   Returns     -
   ======================================================================== */

void PaintWaitingSyncMenu( LONG MenuCombo, LONG sync )
{
	LONG MenuId, ButtonId;
	char buffer[100];
	LONG mx, my;
	int i;
	int numplayers;
	int fDone = TRUE;

	SPLIT_LONG(MenuCombo, MenuId, ButtonId );


	//---- Title screen

	GetButtonPosition( MenuId, 0, &mx, &my );
	init_gfont(FONT_TITL_40PT);
   sprintf(buffer, "^N%s", STRMGR_GetStr(STR_MULTIWAIT_TITLE));
	print_textf(mx + 25 + 48, my + 23, 0, buffer);



	//--- Select Players Line

	init_gfont(FONT_SANS_8PT);

//	GetButtonPosition( MenuId, 1, &mx, &my );
//	sprintf( buffer, "%s", STRMGR_GetStr(STR_PLAYER_WAIT) );
//	gprint_text(mx, my, buffer, WHITE );


#ifdef _WINDOWS


	for ( i = 0; i < MAX_PLAYERS; i++ )
	{
		DWORD dwId;

		//---- Get Player info ( Name etc )

		dwId = AMultiPlayer.GetAPlayerId ( i );


		if ( dwId != kNoPlayer )
		{

			//---- are they at this sync point also

           if ( AMultiPlayer.GetSync( sync, i ) )
			{
				sprintf( buffer,
						 "%s",
						 AMultiPlayer.GetOpponentName(i) );
			}
			else
			{
				sprintf( buffer,
						 "%s - %s",
						 AMultiPlayer.GetOpponentName(i),
						 STRMGR_GetStr(STR_MULTIWAIT_TITLE) );

				fDone = FALSE;
			}

			GetButtonPosition( MenuId, i+1, &mx, &my );

			gprint_text(mx, my, buffer, WHITE );


		}

	}




	//---- Everyone has synced

	if ( fDone )
	{
		fSyncPending = FALSE;
		HideSubMenu(0, D_MULTIWAIT );
		RunMenus();								//---- wait for menu to minimize
		AMultiPlayer.ResetSync( sync );
	}


   #ifdef _DEBUG
   //---- Break out
	if ( GetAsyncKeyState( VK_F10 ) & 0x8000 )
	{
		AMultiPlayer.Finalize();
	}
	#endif



#endif


}



/* ========================================================================
   Function    - PaintMPOver
   Description -
   Returns     -
   ======================================================================== */

void PaintMPOver( LONG MenuCombo, LONG )
{
	int i;


	// set the statement text
	LONG	mx,my;
	LONG	xOff = 0;
	LONG	yOff = 0;
	LONG	MenuId, ButtonId;
	char	textbuf[1024];

	char *  ptemp;

	LONG X;
	LONG Y;
	LONG W;
	LONG H;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	X = X_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].X);
	Y = Y_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].Y);
	W = X_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].W);
	H = Y_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].H);

	xOff = Menus[MenuId].Buttons[0].X;
	yOff = Menus[MenuId].Buttons[0].Y;



#ifdef _WINDOWS



	// paint the control buttons

	init_gfont(FONT_SANS_12PT);

	LONG XX = X + xOff + (W/2);
	LONG YY = Y + yOff + (H/3) + 1;
	
   sprintf( textbuf, "%s", STRMGR_GetStr(STR_MP_GAMEOVER) );
	print_text_centered(
		XX, 
		YY,
		textbuf,
		Menus[MenuId].Buttons[QUESTION_TEXT].LabelColor
		);
   
   sprintf( textbuf, "%s", STRMGR_GetStr(STR_MP_KEEPPLAYING) );
	print_text_centered(
		XX, 
		YY + 20,
		textbuf,
		Menus[MenuId].Buttons[QUESTION_TEXT].LabelColor
		);

#endif

}



/* ========================================================================
   Function    - SetupVoteWait
   Description - Setup vote wait window
   Returns     -
   ======================================================================== */

void SetupVoteWait ( void )
{

	SetButtonProc (D_MULTIWAIT, 0, PaintVoteWait, 0, 0);

   SetButtonArt    (D_MULTIWAIT, 8, "UI\\CHTBTN2C.PCX" );
	SetButtonLabel  (D_MULTIWAIT, 8, -1, 0 );  //---- turn off cancel button
	SetButtonHilight(D_MULTIWAIT, 8, FALSE );
	SetButtonProc   (D_MULTIWAIT, 8, 0, 0, 0);
}




/* ========================================================================
   Function    - SetupWaitPlayers
   Description - Setup waiting for players to join
   Returns     -
   ======================================================================== */

void SetupWaitPlayers ( LONG MenuId )
{

	SetButtonProc   ( MenuId, 0, PaintWaitingMenu, 0, 0 );
	SetButtonLabel  ( MenuId, QUESTION_TEXT, -1, BLACK );


	SetButtonLabel  ( MenuId, QUESTION_BTN1, STR_CANCEL, BTN_LABEL_COLOR );
	SetButtonProc   ( MenuId, QUESTION_BTN1, CancelWaitingMenu, 0, D_KEY_CANCEL );
	SetButtonHilight( MenuId, QUESTION_BTN1, FALSE );

	if ( MenuId == D_QUESTION2 )
	{
		SetButtonLabel  ( D_QUESTION2, QUESTION_BTN2, -1,0 );
		SetButtonProc   ( D_QUESTION2, QUESTION_BTN2, StartWaitingMenu, 0, 0 );
		SetButtonHilight( D_QUESTION2, QUESTION_BTN2, FALSE );
	}
}




/* ========================================================================
   Function    - SetupWaitMultiType
   Description - Setup waiting for game type
   Returns     -
   ======================================================================== */

void SetupWaitMultiType ( void )
{


	SetButtonProc (D_QUESTION1, 0, PaintWaitingTypeMenu, 0, 0 );
	SetButtonLabel  (D_QUESTION1, QUESTION_TEXT, -1, BLACK );


   // Set Cancel button

	SetButtonLabel  (D_QUESTION1, QUESTION_BTN1, STR_CANCEL, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION1, QUESTION_BTN1, CancelWaitingTypeMenu, 0, D_KEY_CANCEL );
	SetButtonHilight(D_QUESTION1, QUESTION_BTN1, FALSE );


}



/* ========================================================================
   Function    - SetupSyncWait
   Description - Setup waiting for players to sync
   Returns     -
   ======================================================================== */

void SetupSyncWait ( int sync )
{

	SetButtonProc (D_MULTIWAIT, 0, PaintWaitingSyncMenu, sync, 0 );

   SetButtonArt    (D_MULTIWAIT, 8, "UI\\CHTBTN2C.PCX" );
	SetButtonLabel  (D_MULTIWAIT, 8, -1, 0 );  //---- turn off cancel button
	SetButtonHilight(D_MULTIWAIT, 8, FALSE );
	SetButtonProc   (D_MULTIWAIT, 8, 0, 0, 0);
}



/* ========================================================================
   Function    - SetupMPOver 
   Description - Setup Mulitplayer over dialog
   Returns     -
   ======================================================================== */

void SetupMPOver ( void )
{

	SetButtonProc (D_QUESTION1, 0, PaintMPOver, 0, 0 );
	SetButtonLabel  (D_QUESTION1, QUESTION_TEXT, -1, BLACK );


   // Set the O.K. button

	SetButtonLabel  (D_QUESTION1, QUESTION_BTN1, STR_OK, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION1, QUESTION_BTN1, HideSubMenu, D_QUESTION1, D_KEY_OK );
	SetButtonHilight(D_QUESTION1, QUESTION_BTN1, FALSE );

}



/* ========================================================================
   Function    - SetupWaitRealm
   Description - Setup waiting for players to select realms
   Returns     -
   ======================================================================== */

void SetupWaitRealm ( void )
{

	SetButtonProc (D_MULTIWAIT, 0, PaintWaitingRealmMenu, 0, 0);

#if 0 // There is no cancel button on this menu.
	SetButtonArt    (D_MULTIWAIT, 8, "UI\\CHTBTN2A.PCX" );
	SetButtonProc   (D_MULTIWAIT, 8, CancelWaitingRealmMenu, 0, 0);
#endif

}



/* ========================================================================
   Function    - SetupChatSelect
   Description - Back up one menu
   Returns     -
   ======================================================================== */

void SetupChatSelect ( void )
{


	SetButtonProc (D_CHAT_SELECT, 0, PaintChatSelect, 0, 0);

	SetButtonLabel  (D_CHAT_SELECT, 11, STR_DONE1, BTN_LABEL_COLOR );  //---- turn on done button
	SetButtonHilight(D_CHAT_SELECT, 11, FALSE );
	SetButtonProc   (D_CHAT_SELECT, 11, DoneChatSelect, 0, 0 );

	SetButtonProc   (D_CHAT_SELECT, 10, CancelChatSelect, 0, 0);
}



/* ========================================================================
   Function    - MultiAllRealmsSelected()
   Description - All realms selected ??
   Returns     -
   ======================================================================== */

BOOL MultiAllRealmsSelected( void )
{
	BOOL fDone = TRUE;
	int i;

#ifdef _WINDOWS

	//---- Check if all the players have selected a realm

	for ( i = 0; i < MAX_PLAYERS; i++ )
	{
		DWORD dwId;
		int rindex;

		//---- Get Player info ( Name etc )
			
		dwId = AMultiPlayer.GetAPlayerId ( i );


		if ( dwId != kNoPlayer )
		{

			rindex = AMultiPlayer.GetRealmFromId( dwId  );

			//---- If they have selected a realm

			if ( rindex < LAND_REALM_COUNT )
			{
			}
			else
			{
				fDone = FALSE;
			}
		}

	}


#endif

	return fDone;

}



/* ========================================================================
   Function    - CheckQuestion
   Description - Check if result is yes or no
   Returns     -
   ======================================================================== */
void CheckQuestion( LONG MenuCombo, LONG index )
{

#ifdef _WINDOWS

	//---- Send our reply

	AMultiPlayer.ReplyEvent( iMPE_ADVENTURE,
							 !index, 0, 0, 0 );


	fWait = TRUE;

	AMultiPlayer.InitEvent(0);	 //---- Clear event table


	SetButtonLabel  (D_QUESTION2, QUESTION_TEXT, -1, BLACK );

	SetButtonLabel  (D_QUESTION2, QUESTION_BTN1, -1, 0 );
	SetButtonHilight(D_QUESTION2, QUESTION_BTN1, FALSE );
	SetButtonProc   (D_QUESTION2, QUESTION_BTN1, 0, 0, NO_KEY );

	SetButtonLabel  (D_QUESTION2, QUESTION_BTN2, -1, 0 );
	SetButtonHilight(D_QUESTION2, QUESTION_BTN2, FALSE );
	SetButtonProc   (D_QUESTION2, QUESTION_BTN2, 0, 0, NO_KEY );


#endif

}



/* ========================================================================
   Function    - PaintAdventQuestion
   Description - Paint routine for generic statments
   Returns     -
   ======================================================================== */
void PaintAdventQuestion(LONG MenuCombo, LONG val)
{
	// set the statement text
	LONG	mx,my;
	LONG	xOff = 0;
	LONG	yOff = 0;
	LONG	MenuId, ButtonId;
	char	textbuf[1024];

	char *  ptemp;

	LONG X;
	LONG Y;
	LONG W;
	LONG H;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	X = X_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].X);
	Y = Y_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].Y);
	W = X_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].W);
	H = Y_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].H);

	xOff = Menus[MenuId].Buttons[0].X;
	yOff = Menus[MenuId].Buttons[0].Y;



#ifdef _WINDOWS


	if ( fWait )
	{
		sprintf( textbuf, "%s",
				 STRMGR_GetStr(STR_PLAYER_WAIT) );
	}
	else
	{
		if ( (ptemp = AMultiPlayer.GetRealmNameOfEvent()) != NULL )
		{
			sprintf( textbuf, "%s %s",
					 ptemp,
					 STRMGR_GetStr(STR_ADVENT) );
		}				
	}


	//---- Waiting for results

	if ( fWait )
	{

		if ( AMultiPlayer.HaveResults() )
		{
			HideSubMenu(0, D_QUESTION2);
			RunMenus();

			//---- Yes to adventure

			if ( EventResults.data1 )
			{
				SelectAdvParty();      //---- Let go adventuring
			}
			else  //---- Only initialize on failure
			{
	     		AMultiPlayer.InitEvent(0);
			}

			fQuestion = FALSE;
			fWait 	  = FALSE;

		}

	}


	// paint the control buttons

	init_gfont(FONT_SANS_12PT);

	print_text_centered(
		X + xOff + (W/2),
		Y + yOff + (H/2) + 1,
		textbuf,
		Menus[MenuId].Buttons[QUESTION_TEXT].LabelColor
		);

#endif

}



/* ========================================================================
   Function    - CheckQuestion
   Description - Check if result is yes or no
   Returns     -
   ======================================================================== */
void CheckQuit( LONG MenuCombo, LONG index )
{

#ifdef _WINDOWS

	//---- Yes quit multiplayer game

	if ( !index )
	{
		AMultiPlayer.Finalize(1);

		HideSubMenu(0, D_QUESTION2);
		RunMenus();


		//---- Where were they quiting from

		if ( fQuit == iMPQ_BEGINNEW )
		{
			DomainNewGame (0, 0 );
		}
		else
		{
			LoadGame(0, 0);
		}

		return;
	}

#endif

	HideSubMenu(0, D_QUESTION2);
	RunMenus();

}



/* ========================================================================
   Function    - PaintMultiQuit
   Description - Paint multiplayer quit
   Returns     -
   ======================================================================== */
void PaintMultiQuit(LONG MenuCombo, LONG val)
{
	// set the statement text
	LONG	mx,my;
	LONG	xOff = 0;
	LONG	yOff = 0;
	LONG	MenuId, ButtonId;
	char	textbuf[1024];

	char *  ptemp;

	LONG X;
	LONG Y;
	LONG W;
	LONG H;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	X = X_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].X);
	Y = Y_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].Y);
	W = X_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].W);
	H = Y_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].H);

	xOff = Menus[MenuId].Buttons[0].X;
	yOff = Menus[MenuId].Buttons[0].Y;



#ifdef _WINDOWS


	sprintf( textbuf, "%s",
			 STRMGR_GetStr(STR_MULTIQUIT) );


	// paint the control buttons

	init_gfont(FONT_SANS_12PT);

	print_text_centered(
		X + xOff + (W/2),
		Y + yOff + (H/2) + 1,
		textbuf,
		Menus[MenuId].Buttons[QUESTION_TEXT].LabelColor
		);

#endif

}



/* ========================================================================
   Function    - SetupMultiQuestion
   Description -
   Returns     -
   ======================================================================== */
void SetupMultiQuestion ( void )
{

	//@@@@ based on event type set differently

	SetButtonProc (D_QUESTION2, 0, PaintAdventQuestion, 0, 0 );


	SetButtonLabel  (D_QUESTION2, QUESTION_TEXT, -1, BLACK );

	// turn on the first button as YES

	SetButtonLabel  (D_QUESTION2, QUESTION_BTN1, STR_YES, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION2, QUESTION_BTN1, CheckQuestion, 0, D_KEY_YES );
	
	SetButtonLabel  (D_QUESTION2, QUESTION_BTN2, STR_NO, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION2, QUESTION_BTN2, CheckQuestion, 1, D_KEY_NO );

	fWait = FALSE;
				


}



/* ========================================================================
   Function    - SetupMultiQuit
   Description -
   Returns     -
   ======================================================================== */
void SetupMultiQuit ( void )
{


	SetButtonProc (D_QUESTION2, 0, PaintMultiQuit, 0, 0 );


	SetButtonLabel  (D_QUESTION2, QUESTION_TEXT, -1, BLACK );

	// turn on the first button as YES

	SetButtonLabel  (D_QUESTION2, QUESTION_BTN1, STR_YES, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION2, QUESTION_BTN1, CheckQuit, 0, D_KEY_YES );

				
	//---- no
	SetButtonLabel  (D_QUESTION2, QUESTION_BTN2, STR_NO, BTN_LABEL_COLOR );
	SetButtonProc   (D_QUESTION2, QUESTION_BTN2, CheckQuit, 1, D_KEY_NO );

}


/* ========================================================================
   Function    - StartSync
   Description - if player is the master, the sync point is initiated
   Returns     - 
	======================================================================== */
void StartSync ( LONG sync )
{
#ifdef _WINDOWS
	LONG	i;
	LONG	rv;
	BOOL	fDone;

	if ( IsMultiPlayer() )
	{
//	[abc] 9/3
//		if (fInfoOnlyDomainUI)
//			fatal_error("StartSync - fInfoOnlyDomainUI = TRUE, executed 2nd sync (sync:%ld/%ld)", fInfoOnlyDomainUI-1,sync);

		StartWait( sync + 16 );

		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
		char temp[200];
		sprintf ( temp, "StartSync - start of sync %ld -----------------", sync);
		RandomLogComment ( temp );
		}

		AMultiPlayer.LookForMessages();		//---- Check for messages

		// all others except master
		if ( !AMultiPlayer.IsMaster() )
		{
			// -- shut off the pinger
			fStartPinging = FALSE;
			
			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
			char temp[200];
			sprintf ( temp, "   StartSync - Player %ld, send sync %ld", AMultiPlayer.GetPlayerId(), sync);
			RandomLogComment ( temp );
			}

			// clear the syncs we will be using
			AMultiPlayer.ResetSync( iMPS_SYNC_GO );
			AMultiPlayer.ResetSync( sync );

			//---- Send Sync ( let others know we are here )
			while ( !AMultiPlayer.SendSyncTo( sync, AMultiPlayer.lMasterId ) )
			{
				AMultiPlayer.LookForMessages();

				run_timers();
				Sleep(100);
				
				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
				char temp[200];
				sprintf ( temp, "   StartSync - Player %ld, send sync %ld Failed!", AMultiPlayer.GetPlayerId(), sync);
				RandomLogComment ( temp );
				}

				// -- Just couldn't sync up
				AMultiPlayer.Finalize();
				
				// -- if we get informed of someone leaving game, need
				// -- to run the menu code to clear the message box
				RunMenus();
				
				if ( !IsMultiPlayer() )		//---- The game is over somehow
					break;
			}

			#if 0
			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
			char temp[200];
			sprintf ( temp, "   StartSync - Player %ld, sync %ld sent", AMultiPlayer.GetPlayerId(), sync);
			RandomLogComment ( temp );
			}
			#endif
		}
		else			// master
		{
			Sleep(0);

			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
			char temp[200];
			sprintf ( temp, "   StartSync - Master, do nothing, sync %ld", sync);
			RandomLogComment ( temp );
			}
			
			AMultiPlayer.ResetAllSyncTimes();
			AMultiPlayer.ResetAllSyncCounts();
			
		}

		EndWait( sync + 16 );
	}
#endif
}


/* ======================================================================== */
#if 0
#define	NUM_LED_X	629
#define	NUM_LED_Y	10
#define	NUM_LED_W	10
#define	NUM_LED_H	6
#define	NUM_LED_INC	8
#include "engine.h"

void PaintNumberOverLED ( int k, DWORD id2, LONG number )
{
	LONG	newY = NUM_LED_Y + (NUM_LED_INC * k);
	
	if (id2 != kNoPlayer)
	{
		color_rect( NUM_LED_X, newY, NUM_LED_W, NUM_LED_H, RED);
		print_textf( NUM_LED_X, newY, WHITE, "^F00%ld", number);
		ScreenCopy( 0, NUM_LED_X, newY, NUM_LED_W, NUM_LED_H, SC_DEFAULT_RES);
	}
}
#endif

#if 0
void DebugMsgOnScreen ( char * text, LONG arg )
{
	SetRemapTable(RED_GRAD);
	DrawBitmap (422, 409, iDMapBx, 22, 0, 216, 21);
	ClearRemapTable();
	init_gfont(FONT_SANS_8PT);
	print_textf(422+108, 420, WHITE, text, arg);
	ScreenCopy( 0, 422, 409, 216, 21, SC_DEFAULT_RES);
}
#endif

/* ========================================================================
   Function    - CheckSync
   Description - checks for sync status
					if fStay, wait at this sync point until everyone has hit this point
					this locks up the screen ?? Is there anyway we can get
			      around this ( Timeouts??? maybe )

   Returns     - number of players left to sync. 0 == done.
	======================================================================== */
LONG CheckSync ( LONG sync, BOOL fStay, BOOL fDisplayProcess )
{
#ifdef _WINDOWS

	LONG	i, cnt, oldcnt, who;
	LONG  masterindex = 99;

	if ( IsMultiPlayer() )
	{
//	[abc] 9/3
//		if (fInfoOnlyDomainUI)
//			fatal_error("CheckSync - fInfoOnlyDomainUI = TRUE, executed 2nd sync (sync:%ld/%ld)", fInfoOnlyDomainUI-1,sync);

		if (fStay && !fDisplayProcess)
			StartWait( sync + 16 );

		//LOGCOMMENTSLEEP
		//if ( fLogComment )
		//{
		//	char temp[200];
		//	sprintf ( temp, "CheckSync - Start of check sync %ld", sync);
		//	RandomLogComment ( temp );
		//}


		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
			SetRemapTable(RED_GRAD);
			DrawBitmap (422, 409, iDMapBx, 22, 0, 216, 21);
			ClearRemapTable();
			init_gfont(FONT_SANS_8PT);
			print_textf(422+108, 420, WHITE, "^cStart CheckSync %d",sync);
			ScreenCopy( 0, 422, 409, 216, 21, SC_DEFAULT_RES);
		}


		AMultiPlayer.LookForMessages();		//---- Check for messages

		//----------------------------------------------------------
		// master
		//----------------------------------------------------------
		if ( AMultiPlayer.IsMaster() )
		{
			oldcnt = 99;

			// ----  Loop unconditionally. Break when we have received all syncs
			while ( TRUE )
			{
//				DebugMsgOnScreen("^cbefore LookForMsg. sync %ld", sync);
				AMultiPlayer.LookForMessages();	//---- Check for messages

//				DebugMsgOnScreen("^cbefore for loop. sync %ld", sync);
				//---- Check if all syncs have come in
				for ( cnt = i = 0; i < MAX_PLAYERS; i++ )
				{

					DWORD dwId;
					dwId = AMultiPlayer.GetAPlayerId(i);
					if ( dwId != kNoPlayer && dwId != AMultiPlayer.GetPlayerId()
						&& !AMultiPlayer.GetDeletedPlayer(i) )
					{
						// paint ack status
						PaintAckLED(i, dwId, (BOOL)AMultiPlayer.GetSync(sync, i));
						
//						DebugMsgOnScreen("^cbefore .GetSync. sync %ld", sync);
						//---- are they at this sync point
						if ( !AMultiPlayer.GetSync( sync, i ) )
						{
							//PaintNumberOverLED( i, dwId, AMultiPlayer.GetSyncTime(i)/1000 );

							// -- GEH 7/7/97
							// -- If we haven't heard from a player in
							// -- a certain amount of time, assume that
							// -- they are hung in the wrong sync point and 
							// -- try to shake them loose.  If I have seen
							// -- shook them once already, delete them
							// -- GEH 7/17/97
							// -- the extra sync just doesn't help
							// -- ABC 7/21/97
							// -- put extra GO back in to make restore game
							// -- multiplayer work
							
						 	if ( 0 == AMultiPlayer.GetSyncCount(i)	
								&& MP_SYNC_TIMEOUT < AMultiPlayer.GetSyncTime(i))
							{
								AMultiPlayer.ResetSyncTime(i);
								AMultiPlayer.IncSyncCount(i);
								AMultiPlayer.SendSyncTo( iMPS_SYNC_GO, dwId );
								LOGCOMMENTSLEEP
								if ( fLogComment )
								{
									char temp[200];
									sprintf ( temp, "CheckSync - sync %ld for id %ld TIMEDOUT, resending...", sync, dwId);
									RandomLogComment ( temp );
								}
							}
							else
							if ( MP_LEFT_TIMEOUT < AMultiPlayer.GetSyncTime(i) )
							{  
								AMultiPlayer.RemovePlayer( dwId );
								
								#if 0
								LOGCOMMENTSLEEP
								if ( fLogComment )
								{
									char temp[200];
									sprintf ( temp, "CheckSync - sync %ld for id %ld 2nd TIMEDOUT, RemovingPlayer...", sync, dwId);
									RandomLogComment ( temp );
								}
								#endif
							}
							
							cnt++;
							who = AMultiPlayer.GetRealmFromId( dwId );
						}
					}
				}

				//	[abc] 9/3 restored
				if (fDisplayProcess && cnt != oldcnt)
				{
					// keep the player informed
					if (cnt == 1)
						DisplayProcess(WAITINGFOR, RED_GRAD, who, 1);
					else if (cnt > 1)
						DisplayProcess(WAITINGFOR, RED_GRAD, 0, 0);
					MenuLoop();
					oldcnt = cnt;
				}

//				DebugMsgOnScreen("^cbefore cnt test. sync %ld", sync);
				if ( cnt == 0 )		// Everyone has synced
				{
//					DebugMsgOnScreen("^ccnt == 0. sync %ld", sync);
					AMultiPlayer.ResetSync( sync );
					break;
				}

				else if ( !fStay )	// if we don't stick here then return
				{
//					DebugMsgOnScreen("^cfStay == FALSE. sync %ld", sync);
					WaitingWho = who;
					return cnt;
				}

//	[abc] 9/3 removed
#if 0
				else if (fDisplayProcess)	// not all synced but can't just return
				{									// try to let map scroll, etc.
//					DebugMsgOnScreen("^cbefore mainloop. sync %ld", sync);

					fInfoOnlyDomainUI = sync+1;
					fRedrawAll = FALSE;
					fUpdateDataArea = FALSE;
	   			WindowsMessages();
					MainLoop();

//					DebugMsgOnScreen("^cafter mainloop. sync %ld", sync);
					// keep the player informed
					#if 01
					if (cnt != oldcnt)
					{
						if (cnt == 1)
							DisplayProcess(WAITINGFOR, RED_GRAD, who, 1);
						else if (cnt > 1)
							DisplayProcess(WAITINGFOR, RED_GRAD, 0, 0);
						oldcnt = cnt;
					}
					#else
					{
						SetRemapTable(RED_GRAD);
						DrawBitmap (422, 409, iDMapBx, 22, 0, 216, 21);
						ClearRemapTable();
						init_gfont(FONT_SANS_8PT);
						print_textf(422+108, 420, WHITE, "^cCheckSync %d Master Wait",sync);
						ScreenCopy( 0, 422, 409, 216, 21, SC_DEFAULT_RES);
					}
					#endif
				}
#endif

//		[abc] 9/3 removed
//				else
//				{
					// -- if we get informed of someone leaving game, need
					// -- to run the menu code to clear the message box
					RunMenus();
					run_timers();
					Sleep(0);
//		[abc] 9/3 removed
//				}
//				DebugMsgOnScreen("^cafter cnt test. sync %ld", sync);

				if ( !IsMultiPlayer() )		//---- The game is over somehow
					break;

				//---- Break out
				//if ( GetAsyncKeyState( VK_F10 ) & 0x8000 )
				//{
				//    AMultiPlayer.Finalize();
				//    break;
				//}
			}	//----- End of While ( TRUE )

			fRedrawAll = TRUE;		// prime the pump

			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
				char temp[200];
				sprintf ( temp, "   CheckSync - Master, received all sync %ld", sync);
				RandomLogComment ( temp );
				sprintf ( temp, "   CheckSync - Master, send GO for sync %ld", sync);
				RandomLogComment ( temp );
			}

			// send the GO message
			while (!AMultiPlayer.SendSync( iMPS_SYNC_GO ))
			{
				AMultiPlayer.LookForMessages();

				// -- if we get informed of someone leaving game, need
				// -- to run the menu code to clear the message box
				RunMenus();
				
				if ( !IsMultiPlayer() )		//---- The game is over somehow
					break;
					
				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					char temp[200];
					sprintf ( temp, "   CheckSync - Master, send GO for sync %ld Failed!", sync);
					RandomLogComment ( temp );
				}
			}

			run_timers();
			Sleep(100);
			run_timers();
			Sleep(100);
			run_timers();
			Sleep(100);

			// send a second GO message (GO2)
			while (!AMultiPlayer.SendSync( sync ))
			{
				AMultiPlayer.LookForMessages();

				// -- if we get informed of someone leaving game, need
				// -- to run the menu code to clear the message box
				RunMenus();
				
				if ( !IsMultiPlayer() )		//---- The game is over somehow
					break;
					
				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					char temp[200];
					sprintf ( temp, "   CheckSync - Master, send GO2 for sync %ld Failed!", sync);
					RandomLogComment ( temp );
				}
			}

			#if 0
			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
				char temp[200];
				sprintf ( temp, "   CheckSync - Master, GO for sync %ld sent", sync);
				RandomLogComment ( temp );
			}
			#endif
		}

		//----------------------------------------------------------
		// all others except master
		//----------------------------------------------------------
		else
		{
			//LOGCOMMENTSLEEP
			//if ( fLogComment )
			//{
			//	char temp[200];
			//	sprintf ( temp, "   CheckSync - Player %ld, receive GO, sync %ld", AMultiPlayer.GetPlayerId(), sync);
			//	RandomLogComment ( temp );
			//}

			// we can't go on until the master is set
			while (AMultiPlayer.lMasterId == -1 || masterindex == 99)
			{
				AMultiPlayer.LookForMessages();

				// find index of master
				for (i = 0; i < MAX_PLAYERS; i++)
				{
					DWORD dwId;
					dwId = AMultiPlayer.GetAPlayerId ( i );	// Player info
					if (dwId == AMultiPlayer.lMasterId)
					{
						masterindex = i;
						break;
					}
				}

				// -- if we get informed of someone leaving game, need
				// -- to run the menu code to clear the message box
				RunMenus();
				
				if ( !IsMultiPlayer() )		//---- The game is over somehow
					return 0;
			}

			WaitingWho = AMultiPlayer.GetRealmFromId( AMultiPlayer.lMasterId );

//	[abc] 9/3 restored
			if (fDisplayProcess)
			{
				DisplayProcess(WAITINGFOR, RED_GRAD, 0, 2);
				MenuLoop();
			}

			//LOGCOMMENTSLEEP
			//if ( fLogComment )
			//{
			//	char temp[200];
			//	sprintf ( temp, "   CheckSync - Player %ld, master ID %ld, index %ld, realm %s", AMultiPlayer.GetPlayerId(), AMultiPlayer.lMasterId, masterindex, realm[WaitingWho].mfGetName() );
			//	RandomLogComment ( temp );
			//}

			// ----  Loop unconditionally. Break when we have received GO message
			while ( TRUE )
			{
				AMultiPlayer.LookForMessages();		//---- Check for messages

				//---- Check if GO or GO2 message has come in
				if ( AMultiPlayer.GetSync( iMPS_SYNC_GO, masterindex )
					|| AMultiPlayer.GetSync( sync, masterindex ) )
				{
					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						char temp[200];
						sprintf ( temp, "   CheckSync - FOUND iMPS_SYNC_GO for masterindex %d.", masterindex);
						RandomLogComment ( temp );
					}
					
					// -- turn my pinger back on
					fStartPinging = TRUE;
					
					AMultiPlayer.ResetSync( iMPS_SYNC_GO );
					AMultiPlayer.ResetSync( sync );
					break;
				}

				if ( !fStay )	// if we don't stick here then return
				{
//					DebugMsgOnScreen("^cfStay == FALSE. sync %ld", sync);
					return 2;
				}

//	[abc] 9/3 removed
#if 0
				else if (fDisplayProcess)	// not all synced but can't just return
				{									// try to let map scroll, etc.
//					DebugMsgOnScreen("^cbefore mainloop. sync %ld", sync);

					fInfoOnlyDomainUI = sync+1;
					fRedrawAll = FALSE;
					fUpdateDataArea = FALSE;
	   			WindowsMessages();
					MainLoop();

//					DebugMsgOnScreen("^cafter mainloop. sync %ld", sync);
					// keep the player informed
					#if 01
					{
						DisplayProcess(WAITINGFOR, RED_GRAD, 0, 2);
					}
					#else
					{
						SetRemapTable(RED_GRAD);
						DrawBitmap (422, 409, iDMapBx, 22, 0, 216, 21);
						ClearRemapTable();
						init_gfont(FONT_SANS_8PT);
						print_textf(422+108, 420, WHITE, "^cCheckSync %d, Wait for GO",sync);
						ScreenCopy( 0, 422, 409, 216, 21, SC_DEFAULT_RES);
					}
					#endif
				}
#endif

//	[abc] 9/3 removed
//				else
//				{
					// -- if we get informed of someone leaving game, need
					// -- to run the menu code to clear the message box
					RunMenus();
					run_timers();
					Sleep(0);
//	[abc] 9/3 removed
//				}

				if ( !IsMultiPlayer() )		//---- The game is over somehow
					break;

				//---- Break out
				//if ( GetAsyncKeyState( VK_F10 ) & 0x8000 )
				//{
				//    AMultiPlayer.Finalize();
				//    break;
				//}
			}	//----- End of While ( TRUE )

			fRedrawAll = TRUE;		// prime the pump
		}


		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
			char temp[200];
			sprintf ( temp, "CheckSync - End of sync %ld -----------------", sync);
			RandomLogComment ( temp );
		}

		if (fStay && !fDisplayProcess)
			EndWait( sync + 16 );

	}	//---- End of IsMultiPlayer()

#endif

	return 0;

}	//---- End of CheckSync()


/* ========================================================================
   Function    - SetupMultiPlayerHost
	Description - init the hilights
	Returns     - 
	======================================================================== */
void SetupMultiPlayerHost ( void )
{
#ifdef _WINDOWS
	// -- Host type
	SetMultiPlayerHostGame( 0 , iMPGT_NEW );
	
	// -- Time limit
	SetMultiPlayerHostTime( 0 , iTTimeLimit );
	
	// detail level
	//SetMultiPlayerHostDetail( 0 , (fControlMode & 0x0003) );
	SetMultiPlayerHostDetail( 0 , 2 );	// RC default to expert
	
	// -- Game difficulty
	SetMultiPlayerHostDiff( 0 , game_difficulty );
#endif
}

/* ========================================================================
   Function    - SetupMultiPlayerJoin
	Description - init the variables
	Returns     - 
	======================================================================== */
void SetupMultiPlayerJoin ( void )
{
	gJoinGame = -1;
}

/* ========================================================================
   Function    - SetupMultiPlayerName
	Description - init the hilights
	Returns     - 
	======================================================================== */
void SetupMultiPlayerName ( void )
{
#ifdef _WINDOWS
	SHORT i;
	
	// -- Name
	strcpy ( szTempName, AMultiPlayer.GetPlayerName() );
	
	// -- Last char needs to be diff
	if ( szTempName[(i = strlen(szTempName))-1] != ' ' )
	{
		szTempName[i]   = ' ';
		szTempName[i+1] = 0;
	}
	
	if (strlen(szTempName) == 1)
	{
		SetButtonLabelColor(D_MULTINAME, 10, GREY);
		activate_region (10, FALSE);
		SetButtonLabelColor(D_MULTINAME, 11, GREY);
		activate_region (11, FALSE);
	}
	else
	{
		SetButtonLabelColor(D_MULTINAME, 10, BTN_LABEL_COLOR);
		activate_region (10, TRUE);
		SetButtonLabelColor(D_MULTINAME, 11, BTN_LABEL_COLOR);
		activate_region (11, TRUE);
	}
	
	// -- Protocol
   gProtocol = AMultiPlayer.GetProtocolIndex();
	SetMultiPlayerProtocol (0 , gProtocol);
#endif
}

/* ========================================================================
   Function    - SetupMultiPlayerCreate
	Description - init the hilights
	Returns     - 
	======================================================================== */
void SetupMultiPlayerCreate ( void )
{
#ifdef _WINDOWS
	SHORT i;
	
	// -- Name
	memset ( &szTempName[0], 0, NAME_SIZE+2 );
	strcpy ( szTempName, AMultiPlayer.GetGameName() );
	
	// -- Last char needs to be diff
	if ( szTempName[(i = strlen(szTempName))-1] != ' ' )
	{
		szTempName[i]   = ' ';
		szTempName[i+1] = 0;
	}
	
	// -- check to activate buttons
	if (strlen(szTempName) == 1)
	{
		SetButtonLabelColor(D_MULTICREATE, 11, GREY);
		activate_region (11, FALSE);
	}
	else
	{
		SetButtonLabelColor(D_MULTICREATE, 11, BTN_LABEL_COLOR);
		activate_region (11, TRUE);
	}
	
	// -- Number of players	           
	if ( AMultiPlayer.GetNumPlayers() >= 2 &&
		AMultiPlayer.GetNumPlayers() <= 8    )
	{
		gNumPlayers = AMultiPlayer.GetNumPlayers();
	}
	else
	{
		gNumPlayers = 8;
   	AMultiPlayer.SetNumPlayers(gNumPlayers);
	}
	
	// -- hilight the correct button
	for (i = 2; i < 8; i++)
	{
		SetButtonLabelColor ( D_MULTICREATE, i, BTN_LABEL_COLOR );
	}
	SetButtonLabelColor ( D_MULTICREATE, gNumPlayers, BTN_LABEL_HILITE_COLOR );
#endif
}

/* ========================================================================
   Function    - PaintMultiPlayerHost
	Description - 
	Returns     - 
	======================================================================== */
void PaintMultiPlayerHost ( LONG MenuCombo, LONG )
{
#ifdef _WINDOWS
	LONG MenuId, ButtonId;
	LONG mx,my,mw,mh;
	CHAR buffer[256];
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// -- Paint Title
	GetButtonPosition( MenuId, 0, &mx, &my );
	GetButtonSize( MenuId, 0, &mw, &mh );
	init_gfont(FONT_TITL_40PT);
   sprintf(buffer, "^N%s", STRMGR_GetStr(STR_MULTIMENU_NG));
	print_text_centered(mx + (mw/2), my + 40, buffer, WHITE);
#endif
}

/* ========================================================================
   Function    - SetMultiPlayerHostGame
	Description - select field for New or Load
	Returns     - 
	======================================================================== */
void SetMultiPlayerHostGame( LONG , LONG Index)
{
#ifdef _WINDOWS
	// -- clear hilight
	SetButtonLabelColor ( D_MULTIHOST, 1, BTN_LABEL_COLOR );
	SetButtonLabelColor ( D_MULTIHOST, 2, BTN_LABEL_COLOR );
	
	// -- save game type
	gGameType = Index;
	
	// -- hilight the correct button
	switch( Index )
	{
	case 1:
		SetButtonLabelColor ( D_MULTIHOST, 1, BTN_LABEL_HILITE_COLOR );
		break;
	
	case 2:
		SetButtonLabelColor ( D_MULTIHOST, 2, BTN_LABEL_HILITE_COLOR );
		break;
	}
#endif
}

/* ========================================================================
   Function    - 
	Description - 
	Returns     - 
	======================================================================== */
void SetMultiPlayerHostTime( LONG , LONG Index)
{
#ifdef _WINDOWS
	// -- clear hilight
	SetButtonLabelColor ( D_MULTIHOST, 3, BTN_LABEL_COLOR );
	SetButtonLabelColor ( D_MULTIHOST, 4, BTN_LABEL_COLOR );
	SetButtonLabelColor ( D_MULTIHOST, 5, BTN_LABEL_COLOR );
	SetButtonLabelColor ( D_MULTIHOST, 6, BTN_LABEL_COLOR );
	
	// -- set the time limit variable
	gTimeLimit = Index;
	
	// -- hilight the correct button
	switch ( Index )
	{
	case iTT_1Min:
		SetButtonLabelColor ( D_MULTIHOST, 3, BTN_LABEL_HILITE_COLOR );
		break;
	
	case iTT_2Min:
		SetButtonLabelColor ( D_MULTIHOST, 4, BTN_LABEL_HILITE_COLOR );
		break;
	
	case iTT_5Min:
		SetButtonLabelColor ( D_MULTIHOST, 5, BTN_LABEL_HILITE_COLOR );
		break;
	
	case iTT_None:
		SetButtonLabelColor ( D_MULTIHOST, 6, BTN_LABEL_HILITE_COLOR );
		break;
	}
#endif
}

/* ========================================================================
   Function    - 
	Description - 
	Returns     - 
	======================================================================== */
void SetMultiPlayerHostDetail( LONG , LONG Index)
{
#ifdef _WINDOWS
	// -- clear hilight
	SetButtonLabelColor ( D_MULTIHOST, 7, BTN_LABEL_COLOR );
	SetButtonLabelColor ( D_MULTIHOST, 8, BTN_LABEL_COLOR );
	SetButtonLabelColor ( D_MULTIHOST, 9, BTN_LABEL_COLOR );
	
	// -- set the time limit variable
	gDetailLevel = Index;
	
	// -- hilight the correct button
	switch ( Index )
	{
	case 0:
		SetButtonLabelColor ( D_MULTIHOST, 7, BTN_LABEL_HILITE_COLOR );
		break;
	
	case 1:
		SetButtonLabelColor ( D_MULTIHOST, 8, BTN_LABEL_HILITE_COLOR );
		break;
	
	case 2:
		SetButtonLabelColor ( D_MULTIHOST, 9, BTN_LABEL_HILITE_COLOR );
		break;
	}
#endif
}

/* ========================================================================
   Function    - 
	Description - 
	Returns     - 
	======================================================================== */
void SetMultiPlayerHostDiff( LONG , LONG Index)
{
#ifdef _WINDOWS
	// -- clear hilight
	SetButtonLabelColor ( D_MULTIHOST, 10, BTN_LABEL_COLOR );
	SetButtonLabelColor ( D_MULTIHOST, 11, BTN_LABEL_COLOR );
	SetButtonLabelColor ( D_MULTIHOST, 12, BTN_LABEL_COLOR );
	
	// -- set the time limit variable
	gDifficulty = Index;
	
	// -- hilight the correct button
	switch ( Index )
	{
	case 0:
		SetButtonLabelColor ( D_MULTIHOST, 10, BTN_LABEL_HILITE_COLOR );
		break;
	
	case 1:
		SetButtonLabelColor ( D_MULTIHOST, 11, BTN_LABEL_HILITE_COLOR );
		break;
	
	case 2:
		SetButtonLabelColor ( D_MULTIHOST, 12, BTN_LABEL_HILITE_COLOR );
		break;
	}
#endif
}

/* ========================================================================
   Function    - CancelMultiPlayerHost
	Description - 
	Returns     - 
	======================================================================== */
void CancelMultiPlayerHost( LONG MenuCombo, LONG )
{
#ifdef _WINDOWS
	// -- wiggle the button
	DoButtonMove ( MenuCombo );
	AMultiPlayer.Finalize(1);
   HideMenu(D_MULTIHOST);
	RunMenus();
	GAMEToggleMainMenu(TRUE,0);  //---- start up main menu
#endif
}

/* ========================================================================
   Function    - DoneMultiPlayerHost
	Description - 
	Returns     - 
	======================================================================== */
void DoneMultiPlayerHost( LONG MenuCombo, LONG )
{
#ifdef _WINDOWS
	// -- wiggle the button
	DoButtonMove ( MenuCombo );
	
	HideMenu(D_MULTIHOST);
	RunMenus();
	
	// -- Set the Detail level
	fControlMode = gDetailLevel | 0x100;
	
	// -- Set the time limit
	iTTimeLimit = gTimeLimit;
	
	// -- Set difficulty
	game_difficulty = (DIFFICULTY) gDifficulty;
	
	// -- Set the game type
	if ( gGameType == iMPGT_NEW )
	{
		// Set game type selected then broadcast it to others
		AMultiPlayer.SetGameType( iMPGT_NEW );
		AMultiPlayer.SendGameType();
		AMultiPlayer.InitPost2();
	}
	else 
	if ( gGameType == iMPGT_LOAD )
	{
		AMultiPlayer.SetGameType( iMPGT_LOAD );
		LoadGameMulti(0,0);
	}
#endif
}

/* ========================================================================
   Function    - PaintMultiPlayerEnvironment
	Description - 
	Returns     - 
	======================================================================== */
void PaintMultiPlayerEnvironment (LONG MenuCombo, LONG )
{
#ifdef _WINDOWS
	LONG MenuId, ButtonId;
	LONG mx,my,mw,mh;
	CHAR buffer[256];
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// -- Paint Title
	GetButtonPosition( MenuId, 0, &mx, &my );
	GetButtonSize( MenuId, 0, &mw, &mh );
	init_gfont(FONT_TITL_40PT);
   sprintf(buffer, "^N%s", STRMGR_GetStr(STR_MULTIMENU_TITLE));
	print_text_centered(mx + (mw/2), my + 44, buffer, WHITE);
#endif
}

/* ========================================================================
   Function    - SelectMultiPlayerEnvironment
	Description - 
	Returns     - 
	======================================================================== */
void SetMultiPlayerEnvironment (LONG , LONG Index)
{
#ifdef _WINDOWS
	// -- clear hilight
	SetButtonLabelColor ( D_MULTIENV, 1, BTN_LABEL_COLOR );
	SetButtonLabelColor ( D_MULTIENV, 2, BTN_LABEL_COLOR );
	SetButtonLabelColor ( D_MULTIENV, 3, BTN_LABEL_COLOR );
	
	// -- set the time limit variable
	gEnvironment = Index;
	
	// -- hilight the correct button
	switch ( Index )
	{
	case 0:
		SetButtonLabelColor ( D_MULTIENV, 1, BTN_LABEL_HILITE_COLOR );
		break;
	
	case 1:
		SetButtonLabelColor ( D_MULTIENV, 2, BTN_LABEL_HILITE_COLOR );
		break;
	
	case 2:
		SetButtonLabelColor ( D_MULTIENV, 3, BTN_LABEL_HILITE_COLOR );
		break;
	}
#endif
}

/* ========================================================================
   Function    - CancelMultiPlayerEnvironment
	Description - 
	Returns     - 
	======================================================================== */
void CancelMultiPlayerEnvironment (LONG MenuCombo, LONG )
{
#ifdef _WINDOWS
	// -- wiggle the button
	DoButtonMove ( MenuCombo );
	
	AMultiPlayer.Finalize();
	
   HideMenu(D_MULTIENV);
	RunMenus();
	
	GAMEToggleMainMenu(TRUE,0);  //---- start up main menu
#endif
}

/* ========================================================================
   Function    - StartInSIGS
	Description - Entry point for SIGS callback
	Returns     - 
	======================================================================== */
void StartInSIGS ( void )
{
#ifdef _WINDOWS
	LONG	MenuCombo;
	gEnvironment = 0;	// SIGS
	MenuCombo = BUILD_LONG( D_MULTIENV, 11 );
	DoneMultiPlayerEnvironment ( MenuCombo, 0 );
#endif
}

/* ========================================================================
   Function    - DoneMultiPlayerEnvironment
	Description - 
	Returns     - 
	======================================================================== */
void DoneMultiPlayerEnvironment (LONG MenuCombo, LONG )
{
#ifdef _WINDOWS
	// -- wiggle the button
	DoButtonMove ( MenuCombo );

	switch( gEnvironment )
	{
	case 0:	// SIGS
		{
		   HideMenu(D_MULTIENV);
			RunMenus();
			

			setScreenMode ( FALSE, FALSE );

			if ( AMultiPlayer.InitPre(1) )  //---- 1 means sigs
			{
				setScreenMode ( FALSE, TRUE );
				
				if ( StartSIGS() )
				{
					setScreenMode( TRUE, TRUE );
						
					
					//---- Set some connection vars
					
					AMultiPlayer.InitPost();
											
					//---- Let master decide what type of game
					
					if ( AMultiPlayer.IsMaster() )
					{
						highlighted_choice = -1;	
						
						m_mode = MENU_TYPESEL;
						
						SetupMultiPlayerHost();
						ShowMenu(D_MULTIHOST);
						RunMenus();
					}
					else  //---- put up waiting window etc.
					{
						ShowWaitMultiType(0, 0);
					}
				}
				else
				{

					setScreenMode ( TRUE, TRUE );

					ShowMenu(D_MULTIENV);
					RunMenus();
				}
			}
			else
			{	
				setScreenMode ( TRUE, TRUE );
		
				ShowMenu(D_MULTIENV);
				RunMenus();
			}
		}
		break;
		
	case 1:	// Lan/modem
		{
			if ( AMultiPlayer.InitPre(0) )
			{
				m_mode = MENU_PROT;
				highlighted_choice = -1;
				HideMenu( D_MULTIENV );
				RunMenus();
				SetupMultiPlayerName();
				ShowMenu( D_MULTINAME );
				RunMenus();
			}
		}
		break;
		
	case 2:	// Web
		{
			
			//---- Check URL browsing 
			if ( CheckURL ( "birth.url" ) )
			{

				//---- pull them out of Direct draw mode 
				if (sDrawMode == iDDRAW)
					ToggleDirectDraw(0, 0);

				// -- Do the browsing thing
				GotoURL( "birth.url", 0 );

			}



//			ShellExecute( hwndApp, "open", "birth.url", NULL, NULL, 0 );
			
			HideMenu( D_MULTIENV );
			RunMenus();
           
         ShowWebStart(0,0);
			
//			GAMEToggleMainMenu(TRUE,0);  //---- start up main menu
		}
		break;
	}
#endif
}

/* ========================================================================
   Function    - PaintMultiPlayerName
	Description - get the name and protocol of the game
	Returns     - 
	======================================================================== */
void PaintMultiPlayerName (LONG MenuCombo, LONG )
{
#ifdef _WINDOWS
	SHORT i;
	LONG MenuId, ButtonId;
	LONG mx,my,mw,mh;
	CHAR buffer[256];
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// -- Title
	GetButtonPosition( MenuId, 0, &mx, &my );
	GetButtonSize( MenuId, 0, &mw, &mh );
	init_gfont(FONT_TITL_40PT);
   sprintf(buffer, "^N%s", STRMGR_GetStr(STR_MULTIMENU_TITLE));
	//print_textf(mx + 25 + 48, my + 23, 0, buffer);
	print_text_centered(mx + (mw/2), my + 44, buffer, WHITE);

	//---- input / display name and protocol
	GetButtonPosition( MenuId, 1, &mx, &my );

	init_gfont(FONT_SANS_8PT);
	sprintf( buffer, "%s %s ",
			 STRMGR_GetStr(STR_MULTIMENU_YN),
			 szTempName );

	gprint_text(mx, my, buffer, BTN_LABEL_HILITE_COLOR );

	// -- check to activate buttons
	if (strlen(szTempName) == 1)
	{
		SetButtonLabelColor(D_MULTINAME, 10, GREY);
		activate_region (10, FALSE);
		SetButtonLabelColor(D_MULTINAME, 11, GREY);
		activate_region (11, FALSE);
	}
	else
	{
		SetButtonLabelColor(D_MULTINAME, 10, BTN_LABEL_COLOR);
		activate_region (10, TRUE);
		SetButtonLabelColor(D_MULTINAME, 11, BTN_LABEL_COLOR);
		activate_region (11, TRUE);
	}
	
	//---- Ignore Return add space back to end of field    
	if ( PaintInputField(MenuId, szTempName, NAME_SIZE+2, -1, TRUE, 1, 0, 0 ) )
	{
		// ignore return, but put space back at end of szTempName
		char *p;
		if (strlen(szTempName) > 0)
			p = szTempName + strlen(szTempName) - 1;
		else
			p = szTempName;
		
		if (*p != ' ' && *p != '_')
			strcat(szTempName, " ");
	}
	
	//---- max protocols three
	for ( i = 0; i < 3; i ++ )
	{
		//---- Protocol here ?
		if ( *AMultiPlayer.GetAProtocol(i) != 0 )
		{
			GetButtonPosition( MenuId, i+2, &mx, &my );
			sprintf( buffer, "%s", AMultiPlayer.GetAProtocol(i) );
			gprint_text(mx, my, buffer,  gProtocol == i ? BTN_LABEL_HILITE_COLOR : BTN_LABEL_COLOR );
		}
	}
#endif
}

/* ========================================================================
   Function    - SetMultiPlayerProtocol
	Description - 
	Returns     - 
	======================================================================== */
void SetMultiPlayerProtocol (LONG , LONG Index)
{
#ifdef _WINDOWS
	gProtocol = Index;
#endif
}

/* ========================================================================
   Function    - BackMultiPlayerName
	Description - 
	Returns     - 
	======================================================================== */
void BackMultiPlayerName (LONG MenuCombo, LONG )
{
#ifdef _WINDOWS
	// -- Wiggle the button
	DoButtonMove ( MenuCombo );
	
	HideMenu( D_MULTINAME );
	RunMenus();
	ShowMenu( D_MULTIENV );
	RunMenus();
#endif
}

/* ========================================================================
   Function    - JoinMultiPlayerName
	Description - 
	Returns     - 
	======================================================================== */
void JoinMultiPlayerName (LONG MenuCombo, LONG )
{
#ifdef _WINDOWS
	// -- Wiggle the button
	DoButtonMove ( MenuCombo );
	
	RemoveInputSpace( TYPE_NAME );
	strcpy ( AMultiPlayer.GetPlayerName(), szTempName );
	
	AMultiPlayer.SetProtocolIndex( gProtocol );
	
	//---- This initializes DPLAY it Must be done !!!!!!
	AMultiPlayer.SelectAProtocol( gProtocol );
	
	AMultiPlayer.SetSpeed( 1 );
	
	
	HideMenu(D_MULTINAME);
	RunMenus();
	
	fEnumerate = 0;
	SetupMultiPlayerJoin();
	ShowMenu(D_MULTIJOIN);
	RunMenus();
	fEnumerate = 1;
#endif
}

/* ========================================================================
   Function    - HostMultiPlayerName
	Description - 
	Returns     - 
	======================================================================== */
void HostMultiPlayerName (LONG MenuCombo, LONG )
{
#ifdef _WINDOWS
	// -- Wiggle the button
	DoButtonMove ( MenuCombo );
	
	RemoveInputSpace( TYPE_NAME );
	strcpy ( AMultiPlayer.GetPlayerName(), szTempName );
	
	AMultiPlayer.SetProtocolIndex( gProtocol );
	
	//---- This initializes DPLAY it Must be done !!!!!!
	AMultiPlayer.SelectAProtocol( gProtocol );
	
	AMultiPlayer.SetSpeed( 1 );
	
	HideMenu(D_MULTINAME);
	RunMenus();
	SetupMultiPlayerCreate();
	ShowMenu(D_MULTICREATE);
	RunMenus();
#endif
}

/* ========================================================================
   Function    - PaintMultiPlayerJoin
	Description - 
	Returns     - 
	======================================================================== */
void PaintMultiPlayerJoin ( LONG MenuCombo, LONG )
{ 
#ifdef _WINDOWS
	SHORT i;
	LONG MenuId, ButtonId;
	LONG mx,my,mw,mh;
	CHAR buffer[256];
	static LONG WaitTime = 0;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// -- Title
	GetButtonPosition( MenuId, 0, &mx, &my );
	init_gfont(FONT_TITL_40PT);
   sprintf(buffer, "^N%s", STRMGR_GetStr(STR_MULTIMENU_JG));
	print_textf(mx + 25 + 48, my + 23, 0, buffer);

	init_gfont(FONT_SANS_8PT);
	
	// setup
	if ( fEnumerate == 1 )
	{	
		GetButtonPosition( MenuId, 1, &mx, &my );
		sprintf( buffer, "%s", STRMGR_GetStr(STR_MULTIMENU_SJ1) );
		gprint_text(mx, my, buffer,  WHITE );

		if ( !AMultiPlayer.IsModemGame() )
		{
			fEnumerate = 2;
		}
		else
		{

//			GetButtonPosition( MenuId, 2, &mx, &my );
//			strcpy ( buffer, "Select this" );
//			gprint_text( mx, my, buffer,  BTN_LABEL_HILITE_COLOR );

			gJoinGame	= 0;
			fEnumerate	= 3;
			iGameTop		= 0;        //---- Start at the top of the list
			iNumGames	= 1;

		}

	}
	else 
	if ( fEnumerate == 2 )
	{
		// -- ping for availible games
		WaitTime = get_time() + 273;	// 15 seconds


		setScreenMode ( FALSE, TRUE );

		AMultiPlayer.GetGames();   //---- Enumerate the games

		setScreenMode ( TRUE, TRUE );

		fEnumerate = 3;
		iGameTop = 0;        //---- Start at the top of the list
#ifdef _WINDOWS
		for ( i = 0; i < MAX_GAMES; i++ )
		{
			if ( *AMultiPlayer.GetAGames(i) == 0 )
			{
				break;
			}
		}
		iNumGames = i;
#else
		iNumGames = 0;
#endif
		// -- Turn on/off the scroll buttons
		SetScrollButtons();
		mouse_click = 0;
		mouse_button = 0;
		clear_key_status(0);
	}
	else
	if ( fEnumerate == 3 )
	{
		int j;
		
		GetButtonPosition( MenuId, 1, &mx, &my );
		sprintf( buffer, "%s", STRMGR_GetStr(STR_MULTIMENU_SJ));
		gprint_text(mx, my, buffer,  WHITE );
		
		//---- Only print the current MP_SL lines
		for ( i = iGameTop, j=0; i < ( MP_SL + iGameTop ); i++, j++ )
		{
			//---- Game here ?
			if ( *AMultiPlayer.GetAGames(i) != 0 )
			{
				char *ptemp;
				GetButtonPosition( MenuId, j + 2, &mx, &my );
				sprintf( buffer, "%s", AMultiPlayer.GetAGames(i) );
				ptemp = strchr( buffer, 0x86 );
				if ( ptemp )
				{
					float temp;
					char *pver = ptemp+1;
					sscanf( pver, "%f", &temp );
					
					if ( temp != RealmAIVersionNumber )
					{
						sprintf ( ptemp, " - N/A" );
					}
					else
					{
						*ptemp = 0;
					}
				}
				
				gprint_text(mx, my, buffer,  gJoinGame == j ? BTN_LABEL_HILITE_COLOR : BTN_LABEL_COLOR );
			}
		}
		SetScrollButtons();
		
		// -- check for more games to join
		if ( get_time() > WaitTime && 
			  !AMultiPlayer.IsModemGame()	)
		{
			fEnumerate = 1;
		}
	}
	else
	{					
		GetButtonPosition( MenuId, 1, &mx, &my );
		sprintf( buffer, "%s", STRMGR_GetStr(STR_MULTIMENU_SJ1) );
		gprint_text(mx, my, buffer,  WHITE );
	}
#endif
}

/* ========================================================================
   Function    - SetMultiPlayerJoin
	Description - 
	Returns     - 
	======================================================================== */
void SetMultiPlayerJoin ( LONG , LONG Index )
{
#ifdef _WINDOWS
	if ( *AMultiPlayer.GetAGames(Index+iGameTop) != 0 )
		gJoinGame = Index+iGameTop;
#endif
}

/* ========================================================================
   Function    - BackMultiPlayerJoin
	Description - 
	Returns     - 
	======================================================================== */
void BackMultiPlayerJoin ( LONG MenuCombo, LONG )
{
#ifdef _WINDOWS
	HideMenu(D_MULTIJOIN);
	RunMenus();
	SetupMultiPlayerName();
	ShowMenu(D_MULTINAME);
	RunMenus();
#endif
}

/* ========================================================================
   Function    - CancelMultiPlayerJoin
	Description - 
	Returns     - 
	======================================================================== */
void CancelMultiPlayerJoin ( LONG MenuCombo, LONG )
{
#ifdef _WINDOWS
	HideMenu(D_MULTIJOIN);
	RunMenus();
	GAMEToggleMainMenu(TRUE,0);  //---- start up main menu
#endif
}

/* ========================================================================
   Function    - DoneMultiPlayerJoin
	Description - 
	Returns     - 
	======================================================================== */
void DoneMultiPlayerJoin ( LONG MenuCombo, LONG )
{
#ifdef _WINDOWS
	if (gJoinGame == -1)
		return;
		
	HideMenu( D_MULTIJOIN );
	RunMenus();
	
	//---- If modem and direct draw then set normal
	if ( AMultiPlayer.IsModemGame() )
	{
	 	// Modem is special on join enumerate now 

		setScreenMode ( FALSE, TRUE );

	 	AMultiPlayer.GetGames();

	}

	//---- Start the NewGame
	if ( AMultiPlayer.JoinGame(gJoinGame+iGameTop) )
	{
		//---- If modem and direct draw then set exclusive
		if ( AMultiPlayer.IsModemGame() )
		{
			setScreenMode ( TRUE, TRUE );
		}

		//---- Don't put up wait menu if we have enough players
		if ( kHaveAll == AMultiPlayer.WaitForPlayers() )
		{
			//---- Enumerate Player names
			AMultiPlayer.GetPlayers();
			AMultiPlayer.InitPost();
			ShowWaitMultiType(0, 0);
		}
		else
		{
			SetupWaitPlayers(D_QUESTION1);
			ShowMenu(D_QUESTION1);
		}
	}
	else
	{
		//---- If modem and direct draw then set exclusive
		if ( AMultiPlayer.IsModemGame() )
		{
			setScreenMode ( TRUE, TRUE );
		}
		iGameTop = 0;
	}
#endif
}

/* ========================================================================
   Function    - PaintMultiPlayerCreate
	Description - 
	Returns     - 
	======================================================================== */
void PaintMultiPlayerCreate (LONG MenuCombo, LONG )
{
#ifdef _WINDOWS
	SHORT i;
	LONG MenuId, ButtonId;
	LONG mx,my,mw,mh;
	CHAR buffer[256];
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// -- Title
	GetButtonPosition( MenuId, 0, &mx, &my );
	init_gfont(FONT_TITL_40PT);
   sprintf(buffer, "^N%s", STRMGR_GetStr(STR_CREATE));
	print_textf(mx + 25 + 48, my + 23, 0, buffer);

	//---- input / display name and protocol
	GetButtonPosition( MenuId, 1, &mx, &my );

	init_gfont(FONT_SANS_8PT);
	sprintf( buffer, "%s %s ",
			 STRMGR_GetStr(STR_MULTIMENU_NGP),
			 szTempName );

	gprint_text(mx, my, buffer, BTN_LABEL_HILITE_COLOR );

	// -- check to activate buttons
	if (strlen(szTempName) == 1)
	{
		SetButtonLabelColor(D_MULTICREATE, 11, GREY);
		activate_region (11, FALSE);
	}
	else
	{
		SetButtonLabelColor(D_MULTICREATE, 11, BTN_LABEL_COLOR);
		activate_region (11, TRUE);
	}
	
	//---- Ignore Return add space back to end of field    
	if ( PaintInputField(MenuId, szTempName, NAME_SIZE+2, -1, TRUE, 1, 0, 0 ) )
	{
		// ignore return, but put space back at end of szTempName
		char *p;
		if (strlen(szTempName) > 0)
			p = szTempName + strlen(szTempName) - 1;
		else
			p = szTempName;
		
		if (*p != ' ' && *p != '_')
			strcat(szTempName, " ");
	}
#endif
}

/* ========================================================================
   Function    - SetMultiPlayerNumber
	Description - 
	Returns     - 
	======================================================================== */
void SetMultiPlayerNumber (LONG , LONG Index)
{
#ifdef _WINDOWS
	gNumPlayers = Index;
	// -- hilight the correct button
	for (SHORT i = 2; i <= 8; i++)
	{
		SetButtonLabelColor ( D_MULTICREATE, i, BTN_LABEL_COLOR );
	}
	SetButtonLabelColor ( D_MULTICREATE, gNumPlayers, BTN_LABEL_HILITE_COLOR );
#endif
}

/* ========================================================================
   Function    - BackMultiPlayerCreate
	Description - 
	Returns     - 
	======================================================================== */
void BackMultiPlayerCreate (LONG MenuCombo, LONG )
{
#ifdef _WINDOWS
	// -- wiggle the button
	DoButtonMove ( MenuCombo );
	
	HideMenu(D_MULTICREATE);
	RunMenus();
	SetupMultiPlayerName();
	ShowMenu(D_MULTINAME);
	RunMenus();
#endif
}

/* ========================================================================
   Function    - CancelMultiPlayerCreate
	Description - 
	Returns     - 
	======================================================================== */
void CancelMultiPlayerCreate (LONG MenuCombo, LONG )
{
#ifdef _WINDOWS
	// -- wiggle the button
	DoButtonMove ( MenuCombo );
	
	HideMenu(D_MULTICREATE);
	RunMenus();
	GAMEToggleMainMenu(TRUE,0);  //---- start up main menu
#endif
}

/* ========================================================================
   Function    - HostMultiPlayerCreate
	Description - 
	Returns     - 
	======================================================================== */
void HostMultiPlayerCreate (LONG MenuCombo, LONG )
{
#ifdef _WINDOWS
	// -- wiggle the button
	DoButtonMove ( MenuCombo );
	
   // -- set number of players
   AMultiPlayer.SetNumPlayers(gNumPlayers);
   
	// -- set game name
	RemoveInputSpace( TYPE_NAME );
	strcpy ( AMultiPlayer.GetGameName(), szTempName );
	
	HideMenu(D_MULTICREATE);
	RunMenus();
	

	if ( AMultiPlayer.IsModemGame() )
	{
		setScreenMode ( FALSE, TRUE );
	}

	// -- start waiting for players to join
	if ( AMultiPlayer.StartNewGame() )
	{
   	if ( AMultiPlayer.IsModemGame() )
		{
			setScreenMode ( TRUE, TRUE );
		}

		SetupWaitPlayers(D_QUESTION2);
		ShowMenu(D_QUESTION2);
	}
	else
	{
   	if ( AMultiPlayer.IsModemGame() )
   	{
			setScreenMode ( TRUE, TRUE );
	    }

		// -- error
		AMultiPlayer.Finalize();
		RunMenus();

		GAMEToggleMainMenu(TRUE,0);  //---- start up main menu
	// YOU CAN'T DO THIS AFTER AN FINALIZE
//		SetupMultiPlayerName();
//		ShowMenu(D_MULTINAME);
	}
#endif
}


//---- End of multiui.cpp

/* ======================================================================== */

