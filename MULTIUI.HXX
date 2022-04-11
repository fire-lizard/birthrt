/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _MULTIUI_HXX
#define _MULTIUI_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */



/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
enum
{
	iMPFT_REC,
	iMPFT_SEND,
};


enum 
{
   iMPQ_BEGINNEW,
   iMPQ_LOADGAME,
};



enum 
{
   iTT_1Min = 0,
   iTT_2Min,
   iTT_5Min,
   iTT_None,
};



void ShowMPOver(LONG, LONG);
void ShowVoteWaitWindow(LONG, LONG);
void ShowMultiQuestion(LONG, LONG);
void ShowMultiQuit(LONG, LONG);
void ShowWaitMultiType(LONG, LONG);


void ShowSyncWaitWindow( LONG sync, LONG);

void SetChatSelect( LONG, LONG Key );
void DoneChatSelect(LONG, LONG);
void CancelChatSelect(LONG, LONG);


void SetMPControlMode ( void );

void SetupMultiQuestion ( void );
void SetMultiPlayerMenu( LONG, LONG Key );
void GetMultiPlayerHelp( LONG, LONG );
void DoneMultiPlayerMenu(LONG, LONG);
void BackMultiPlayerMenu(LONG, LONG);
void CancelMultiPlayerMenu(LONG, LONG);

void ScrollMultiPlayerMenu(LONG, LONG down );

void PaintMultiPlayerName (LONG MenuCombo, LONG );
void SetMultiPlayerProtocol (LONG , LONG Index);
void BackMultiPlayerName (LONG MenuCombo, LONG );
void JoinMultiPlayerName (LONG MenuCombo, LONG );
void HostMultiPlayerName (LONG MenuCombo, LONG );

void PaintMultiPlayerCreate (LONG MenuCombo, LONG );
void SetMultiPlayerNumber (LONG , LONG Index);
void BackMultiPlayerCreate (LONG MenuCombo, LONG );
void CancelMultiPlayerCreate (LONG MenuCombo, LONG );
void HostMultiPlayerCreate (LONG MenuCombo, LONG );

void PaintMultiPlayerJoin (LONG MenuCombo, LONG );
void SetMultiPlayerJoin (LONG , LONG Index);
void BackMultiPlayerJoin (LONG MenuCombo, LONG );
void CancelMultiPlayerJoin (LONG MenuCombo, LONG );
void DoneMultiPlayerJoin (LONG MenuCombo, LONG );

void PaintMultiPlayerEnvironment (LONG MenuCombo, LONG );
void SetMultiPlayerEnvironment (LONG MenuComb, LONG );
void CancelMultiPlayerEnvironment (LONG MenuCombo, LONG );
void DoneMultiPlayerEnvironment (LONG MenuCombo, LONG );

void SetupMPOver		( void );
void SetupVoteWait		( void );
void SetupWaitPlayers	( LONG MenuId );
void SetupChatSelect	( void );
void SetupWaitRealm	( void );
void SetupMultiQuit 	( void );
void SetupWaitMultiType ( void );

void SetupSyncWait		( int sync );

void StartInSIGS ( void );

void StartSync ( LONG sync );
LONG CheckSync ( LONG sync, BOOL fStay, BOOL fDisplayResult );
#define WaitSync(a)	StartSync(a); do { ; } while (CheckSync((a), TRUE, FALSE));

void PaintMultiPlayerHost( LONG, LONG );
void SetMultiPlayerHostGame( LONG , LONG Index);
void SetMultiPlayerHostTime( LONG , LONG Index);
void SetMultiPlayerHostDetail( LONG , LONG Index);
void SetMultiPlayerHostDiff( LONG , LONG Index);
void CancelMultiPlayerHost( LONG , LONG );
void DoneMultiPlayerHost( LONG , LONG );

BOOL MultiAllRealmsSelected( void );

//#if defined(__cplusplus)
//extern "C" {
//#endif
void PaintChatSelect(LONG MenuCombo, LONG);
void ShowChatSelectWindow(LONG, LONG);
//#if defined(__cplusplus)
//}
//#endif


/* ------------------------------------------------------------------------
   Internal Prototypes - don't call these directly
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */

#endif // _MULTIUI_HXX


