// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//
//	 GMulPlay.cpp
//		GenericMultiPlayer class
//
//
//	Who	When			Why
// ---	---------	-----------------------------------------------
//	CAM	14 JUN 96	Broke out from MultiPlayer class
// DLJ 25 SEP 96   Try and make it multi player 
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#ifdef _DEBUG
#define _MULTIMONO    //---- Comment out for no mono
#endif

//#define LOGCOMMENTSLEEP Sleep(1);
#define LOGCOMMENTSLEEP {;}

#include <windows.h>
#include "GMulPlay.hxx"
#include "../Resource.h"
#include "../system.h"
#include "../machine.h"

#include "FileXfer.hxx"

#include "StdIO.h"
#include "Limits.h"
#include "Direct.h"
#include "mono_c.h"


#define iMPT_SYNC		20
#define iMPT_UNIT		8
#define iMPT_PING		6


static int		DumpString				( char *s );

enum
{
	iMPM_SEND,
	iMPM_RCV,
	iMPM_CREATE,
	iMPM_RCVFILE,
	iMPM_SENDFILE,
};


// ----------------------------------------------------------
// -- Don't EVER mess with these wait numbers as you will ---
// --  probably break things!!!!  This means you!			 ---
// ----------------------------------------------------------
//
//                                                           DPlay   INN Internet
//GEH DWORD dwMMsgWaitS[GenericMultiPlayer::iMP_NUMTYPES]       = {  700,  700,  700 };
//GEH DWORD dwMMsgTriesS[GenericMultiPlayer::iMP_NUMTYPES]      = {    1,    1,    1 };
//GEH DWORD dwMMsgWaitSR[GenericMultiPlayer::iMP_NUMTYPES]      = { 1400, 1400, 1400 }; 
//GEH DWORD dwMMsgWaitR[GenericMultiPlayer::iMP_NUMTYPES]       = { 1000, 1000, 1000 };	 
//GEH DWORD dwMMsgWaitRS[GenericMultiPlayer::iMP_NUMTYPES]      = { 1000, 1000, 1000 }; 
//GEH DWORD dwMMsgMasterFudge[GenericMultiPlayer::iMP_NUMTYPES] = {  100,  100,  100 };
//GEH DWORD dwMMsgMaxLoop[GenericMultiPlayer::iMP_NUMTYPES]     = {  120,  120,  120 };	  

// -- GEH Just had to try to improve performance in the DPlay version
DWORD dwMMsgWaitS[GenericMultiPlayer::iMP_NUMTYPES]       = {  400,  700,  700 };
DWORD dwMMsgTriesS[GenericMultiPlayer::iMP_NUMTYPES]      = {    1,    1,    1 };
DWORD dwMMsgWaitSR[GenericMultiPlayer::iMP_NUMTYPES]      = { 1400, 1400, 1400 }; 
DWORD dwMMsgWaitR[GenericMultiPlayer::iMP_NUMTYPES]       = { 1000, 1000, 1000 };	 
DWORD dwMMsgWaitRS[GenericMultiPlayer::iMP_NUMTYPES]      = { 1000, 1000, 1000 }; 
DWORD dwMMsgMasterFudge[GenericMultiPlayer::iMP_NUMTYPES] = {  100,  100,  100 };
DWORD dwMMsgMaxLoop[GenericMultiPlayer::iMP_NUMTYPES]     = {   20,  120,  120 };	  

//		iMSG_WAITS		= 700,		// send wait
//		iMSG_TRIESS		= 1, 
//		iMSG_WAITSR		= 1400,		// receive in send loop
//		iMSG_WAITR		= 1000,		// rcv wait
//		iMSG_WAITRS		= 1000, 	// receive in rvc loop 
//		iMSG_MASTERFUDGE= 300,		// master side fudge factor
//		iMAX_LOOP		= 120,		// max loops


typedef FileXfer *(*PNEWFILEXFER) ( GamePrintFunction pPrintFunc, ErrCode *err );
typedef void (*PDELETEFILEXFER) ( FileXfer * );

// static GenericMultiPlayer *ChatObj = NULL;

static FARPROC	fpOldEditWndProc = NULL;


// -- End Local Stuff

static ErrCode err = kNoError;



// -----------------------------------------------------------------
// -- Prototypes
// -----------------------------------------------------------------

void PaintAckLED(int k, DWORD id2, BOOL localAcks);

extern "C" void RandomLogComment ( char * szString );
extern "C" void RandomLogPrefix ( char * szString );



// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::GenericMultiPlayer
//		Default constructor.
//
//	 Inputs:
//		none
//
// -----------------------------------------------------------------
GenericMultiPlayer::GenericMultiPlayer( void ) : 
	fAvailable( FALSE ), pnmGame( NULL, &err ), ulSndMsgNum( 0 )
{
	int x;

	for ( x = 0 ; x < MAX_PLAYERS; x++ )
	{
		ulRcvMsgNum[x] = 0;
	}

	// -- See if Multiplayer play is available

	fAvailable = TRUE;	// default for now!


 	// -- Setup default wait times

 	dwMsgWaitS			= dwMMsgWaitS[iMP_NETMODEM];			
 	dwMsgTriesS			= dwMMsgTriesS[iMP_NETMODEM]; 
 	dwMsgWaitSR			= dwMMsgWaitSR[iMP_NETMODEM]; 
 	dwMsgWaitR			= dwMMsgWaitR[iMP_NETMODEM];	 
 	dwMsgWaitRS			= dwMMsgWaitRS[iMP_NETMODEM]; 
 	dwMsgMasterFudge	= dwMMsgMasterFudge[iMP_NETMODEM];
 	dwMsgMaxLoop		= dwMMsgMaxLoop[iMP_NETMODEM];


} // GenericMultiPlayer::GenericMultiPlayer



// -----------------------------------------------------------------
//
//  GenericMultiPlayer::SetWaitTimes
//     Set the wait times according to the protocol selected
//
//  Inputs:
//     usMultiplier    - additional multiplier for times
//
//  Return: none
//
// -----------------------------------------------------------------
void GenericMultiPlayer::SetWaitTimes( USHORT usMultiplier )
{
    // -- Setup the correct wait times
   dwMsgWaitS          = dwMMsgWaitS[ptType]       * usMultiplier;
   dwMsgTriesS         = dwMMsgTriesS[ptType];
   dwMsgWaitSR         = dwMMsgWaitSR[ptType]      * usMultiplier;;
   dwMsgWaitR          = dwMMsgWaitR[ptType]       * usMultiplier;
   dwMsgWaitRS         = dwMMsgWaitRS[ptType]      * usMultiplier;;
   dwMsgMasterFudge    = dwMMsgMasterFudge[ptType]* usMultiplier;;
   dwMsgMaxLoop        = dwMMsgMaxLoop[ptType];

//     pnmGame.SetXferDelay( dwMsgWaitS * usMultiplier );

       pnmGame.SetXferDelay( dwMsgWaitR );

} // GenericMultiPlayer::SetWaitTimes



// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::~GenericMultiPlayer
//		Default destructor.
//
// -----------------------------------------------------------------
GenericMultiPlayer::~GenericMultiPlayer( void )
{

	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		char temp[200];
		sprintf ( temp, "GenericMultiPlayer::GenericMultiPlayer - finalize");
		RandomLogComment ( temp );
	}

	Finalize();

} // MultiPlayer::~GenericMultiPlayer




// --
// -- Data Retrieval
// --

FileXfer *GenericMultiPlayer::GetNetModule( void ) 
{ 
	return ( &pnmGame ); 
}


// --
// -- Status Inquiries
// --

BOOL GenericMultiPlayer::IsAvailable( void )	
{ 
	return ( fAvailable ); 
}


BOOL GenericMultiPlayer::WasConnectionLost	( void )
{
	return ( !fSent );
}


// --
// -- Data Setting
// --

void GenericMultiPlayer::SetMsgProc( FP_MISCPROC fp ) 
{ 
	fpMsgProc = fp; 
}

void GenericMultiPlayer::SetRunDir( PSZ pszRunDirectory )
{
	pszRunDir = pszRunDirectory;
}


// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::CheckConnectStatus
//
//	 Inputs:
//
//	 Return: 
//
// -----------------------------------------------------------------

int GenericMultiPlayer::CheckConnectStatus( void )
{
   long Id = 0;

   return ( pnmGame.CheckConnectStatus ( ) );


} // GenericMultiPlayer::CheckConnectStatus()


// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::ForceStart
//
//	 Inputs:
//
//	 Return: 
//
// -----------------------------------------------------------------

int GenericMultiPlayer::ForceStart( void )
{

   return ( pnmGame.ForceStart ( ) );


} // GenericMultiPlayer::ForceStart()



// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::DisablePlayers
//
//	 Inputs:
//
//	 Return: 
//
// -----------------------------------------------------------------

void GenericMultiPlayer::DisablePlayers( void )
{

   pnmGame.DisablePlayers( );

} // GenericMultiPlayer::DisablePlayers()




// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::GetPlayerId
//		Get a our player id 
//
//	 Inputs:
//
//	 Return: Player Id
//
// -----------------------------------------------------------------

DWORD GenericMultiPlayer::GetPlayerId( void )
{
   long Id = 0;

   pnmGame.GetLocalPlayerID ( &Id );

   return (DWORD) Id;

} // GenericMultiPlayer::GetPlayerId()


// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::GetAPlayerId
//		Get a player id 
//
//	 Inputs:
//			index 
//
//	 Return: Player Id
//
// -----------------------------------------------------------------

DWORD GenericMultiPlayer::GetAPlayerId( int i )
{
   long Id = 0;

   pnmGame.GetOtherPlayerID ( &Id, i );

   return (DWORD) Id;

} // GenericMultiPlayer::GetAPlayerId()


// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::GetAPlayerIndex
//		Get a player id 
//
//	 Inputs:
//			index 
//
//	 Return: Player Id
//
// -----------------------------------------------------------------

int GenericMultiPlayer::GetAPlayerIndex( DWORD dwWhatId )
{
	int x;
	long Id = 0;

	for ( x = 0; x < MAX_PLAYERS; x++ )
	{
	   	pnmGame.GetOtherPlayerID ( &Id, x );

		if ( Id == dwWhatId )
		{
			return x;						
		}
	}

   return x;

} // GenericMultiPlayer::GetAPlayerIndex()



// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::GetDeletedPlayer
//				Get deleted player flag 
//
//	 Inputs:
//		index 
//
//	 Return: deleted player flag
//
// -----------------------------------------------------------------

int GenericMultiPlayer::GetDeletedPlayer  ( int index )
{
   return (pnmGame.GetDeletedPlayer ( index ));
}



// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::GetDeletedPlayerId
//				Get deleted player id 
//
//	 Inputs:
//		index 
//
//	 Return: delete player id 
//
// -----------------------------------------------------------------

DWORD GenericMultiPlayer::GetDeletedPlayerId  ( int index )
{
   return (pnmGame.GetDeletedPlayerId ( index ));
}


// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::DeletePlayer
//				delete player id 
//
//	 Inputs:
//		index 
//
//	 Return: delete player id 
//
// -----------------------------------------------------------------
void GenericMultiPlayer::DeletePlayer  ( DWORD dwDelPlayerId )
{
   pnmGame.DeletePlayer ( dwDelPlayerId );
}



// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::IsModemGame
//
//	 Inputs:
//
//	 Return: if it is a modem game
//
// -----------------------------------------------------------------

BOOL GenericMultiPlayer::IsModemGame( void )
{
    return ( pnmGame.IsModemGame() );
}



// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::ResetDeletedPlayer
//				reset deleted player  
//
//	 Inputs:
//		index 
//
//	 Return:
//
// -----------------------------------------------------------------

void GenericMultiPlayer::ResetDeletedPlayer  ( int index )
{
   pnmGame.ResetDeletedPlayer ( index );
}




// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::CreateAGame
//		Enumerate the current games, calling the DString function
//			for each.
//
//	 Inputs:
//		DString	- display enumeration string function
//		lContext	- optional additional parameter to DString
//
//	 Return: none
//
// -----------------------------------------------------------------
BOOL GenericMultiPlayer::CreateAGame( PSZ pszPlayerName )
{
	ErrCode err = kNoError;

	err = pnmGame.CreateNetworkGame( pszPlayerName );

	return ( (err == kNoError) ? TRUE : FALSE );

} // GenericMultiPlayer::CreateAGame




// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::EnumerateGames
//		Enumerate the current games, calling the DString function
//			for each.
//
//	 Inputs:
//		DString	- display enumeration string function
//		lContext	- optional additional parameter to DString
//
//	 Return: none
//
// -----------------------------------------------------------------
void GenericMultiPlayer::EnumerateGames( GameDisplayStringFunction DString, long lContext )
{

	pnmGame.EnumerateNetworkGames( DString, lContext );

} // GenericMultiPlayer::EnumerateGames




// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::EnumeratePlayers
//		Enumerate the current players, calling the DString function
//			for each.
//
//	 Inputs:
//		DString	- display enumeration string function
//		lContext	- optional additional parameter to DString
//
//	 Return: none
//
// -----------------------------------------------------------------
void GenericMultiPlayer::EnumeratePlayers( GameDisplayStringFunction DString, long lContext )
{

	pnmGame.EnumeratePlayers( DString, 0 );

} // GenericMultiPlayer::EnumeratePlayers




// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::EnumerateProtocols
//		Enumerate the allowed protocols, calling the DString function
//			for each.
//
//	 Inputs:
//		DString	- display enumeration string function
//		lContext	- optional additional parameter to DString
//
//	 Return: none
//
// -----------------------------------------------------------------
void GenericMultiPlayer::EnumerateProtocols( GameDisplayStringFunction DString, 
														long lContext )
{

	pnmGame.EnumerateNetworkProtocols( DString, lContext );

} // GenericMultiPlayer::EnumerateProtocols




// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::Finalize
//		Close out the multi-player game.
//
//	 Inputs:
//		none
//
//	 Return: results of closing
//
// -----------------------------------------------------------------
BOOL GenericMultiPlayer::Finalize( void )
{

	//---- Logoff etc ???

	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		char temp[200];
		sprintf ( temp, "GenericMultiPlayer::Finalize");
		RandomLogComment ( temp );
	}

	pnmGame.CancelNetworkGame();

	return ( TRUE );

} // GenericMultiPlayer::Finalize




// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::Initialize
//		Initialize the multi-player game.
//
//	 Inputs:
//		usType		- type of play desired
//
//	 Return: results of initialization
//
// -----------------------------------------------------------------
BOOL GenericMultiPlayer::Initialize( PlayType usType )
{
//	BOOL fOK = TRUE;

   ErrCode err = kNoError;
	CHAR szName[MAX_PATH] = "";
	PNEWFILEXFER pNew = NULL;
	int x;


	// -- Save the type

	ptType = usType;



	// -- Setup the correct wait times

	dwMsgWaitS			= dwMMsgWaitS[ptType];			
	dwMsgTriesS			= dwMMsgTriesS[ptType]; 
	dwMsgWaitSR			= dwMMsgWaitSR[ptType]; 
	dwMsgWaitR			= dwMMsgWaitR[ptType];	 
	dwMsgWaitRS			= dwMMsgWaitRS[ptType]; 
	dwMsgMasterFudge	= dwMMsgMasterFudge[ptType];
	dwMsgMaxLoop		= dwMMsgMaxLoop[ptType];	  

	pnmGame.SetXferDelay( dwMsgWaitS );


	// SetupChat();	let MulPlay do this later

	for ( x = 0; x < MAX_PLAYERS; x++ )
	{
		ulRcvMsgNum[x] = 0;
	}

	ulSndMsgNum = 0;



	if ( usType == iMP_SIGS )
	{
		pnmGame.SetSigs( TRUE );
	}
	else
	{
		pnmGame.SetSigs( FALSE );
	}


	err =  pnmGame.IsNetworkingAvailable();


	return( (err == kNoError) ? TRUE : FALSE );


//	return ( fOK );


}  // GenericMultiPlayer::Initialize




// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::IsNetworkingAvailable
//		Look for a misc message, and deal with them.
//
//	 Inputs:
//		none
//
//	 Return: none
//
// -----------------------------------------------------------------
BOOL GenericMultiPlayer::IsNetworkingAvailable( void )
{
	ErrCode err = kNoError;

	err = pnmGame.IsNetworkingAvailable();

	return( (err == kNoError) ? TRUE : FALSE );

} // GenericMultiPlayer::IsNetworkingAvailable




// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::JoinAGame
//		Look for a misc message, and deal with them.
//
//	 Inputs:
//		none
//
//	 Return: none
//
// -----------------------------------------------------------------
BOOL GenericMultiPlayer::JoinAGame( long lIndex, PSZ pszPlayerName )
{
	ErrCode err = kNoError;

	err = pnmGame.JoinNetworkGame( lIndex, pszPlayerName );

	return( (err == kNoError) ? TRUE : FALSE );

} // GenericMultiPlayer::JoinAGame




// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::PeekForNextMessage
//		Look for a message in the queue but don't remove it.
//
//	 Inputs:
//		pMsg	- pointer to message buffer
//
//	 Return: TRUE if a message was found, else FALSE
//
// -----------------------------------------------------------------
BOOL GenericMultiPlayer::PeekForNextMessage( LPNETMSG pMsg )
{

	return ( pnmGame.PeekNextMessage( pMsg ) ) ;

} // GenericMultiPlayer::PeekForNextMessage




// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::SelectAProtocol
//		Select the desired protocol.
//
//	 Inputs:
//		lIndex	- index into the enumerated protocol list
//
//	 Return: result of selection
//
// -----------------------------------------------------------------
BOOL GenericMultiPlayer::SelectAProtocol( long lIndex )
{
	ErrCode err = kNoError;

	err = pnmGame.SetNetworkProtocol( lIndex );

	return ( (err == kNoError) ? TRUE : FALSE );

} // GenericMultiPlayer::SelectAProtocol



// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::WaitForPlayer
//		Wait for the other guy to join.
//
//	 Inputs:
//		none
//
//	 Return: none
//
// -----------------------------------------------------------------
BOOL GenericMultiPlayer::WaitForPlayer( void )
{

	return( pnmGame.WaitForPlayer( dwMsgWaitR * dwMsgMaxLoop ) );

} // GenericMultiPlayer::WaitForPlayer



// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::WaitForPlayers
//		Wait for the other players to join.
//
//	 Inputs:
//		none
//
//	 Return: none
//
// -----------------------------------------------------------------
int GenericMultiPlayer::WaitForPlayers( void )
{

	return( pnmGame.WaitForPlayers( ) );

} // GenericMultiPlayer::WaitForPlayers



 // --
 // -- Protected functions
 // --

// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::ReceiveFile
//		Receive the specified file.
//
//	 Inputs:
//		dwFrom      - Should be master Id 
//		pNew		- new file name of received file
//
//	 Return: TRUE if a file was received, else FALSE
//
// -----------------------------------------------------------------
BOOL GenericMultiPlayer::ReceiveFile( DWORD dwFrom, PSZ pNew )
{
	CHAR szFName[MAX_PATH];
	BOOL fRes = FALSE;

	// -- Wait for the 'Want to send a file' message

	//GEH !!!! fRes = ReceiveMessage( iMPT_FILE, dwMsgWaitR );
	fRes = TRUE;

	if ( fRes )
	{
		// -- delete old temp file
		remove( pNew );

		fRes = pnmGame.ReceiveFile( dwFrom, szFName, dwMsgWaitR );

		if ( fRes )
		{
			// -- rename to correct name
			rename( szFName, pNew );
		}
	}

	return ( fRes );


} // GenericMultiPlayer::ReceiveFile



// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::ReceiveMessage
//		Receive the specified message.
//
//	 Inputs:
//		usType  	- message to receive
//		dwWait	    - how long to wait
//		fOneOnly	- if TRUE, wait for only one message, even if it
//					  is out of synch
//
//	 Return: none
//
// -----------------------------------------------------------------
BOOL GenericMultiPlayer::ReceiveMessage( USHORT usType, DWORD dwWait, 
										  BOOL fOneOnly, BOOL fAck )
{
	LPNETMSG lpnm  = (LPNETMSG) &cNetBuf[0];
	int iNMSize    = MAX_NETBUFF - sizeof(NETMSG) + sizeof(char);
   ErrCode err    = kGenericError;
	int iRcvType   = iMPT_SYSTEM;
	unsigned int i = 0;
	int PlayerInd;

	// GEH while ( (iRcvType != usType) && ( i < dwMsgMaxLoop ) )
	while ( (iRcvType != usType) )
	{
		//---- service sound
		run_timers();

		// -- Go get a regular message
		if ( dwWait != 0 )
			dwWait += dwMsgFudge;

		// -- go get a message
		err = pnmGame.GetMessage( lpnm, dwWait, TRUE, fAck );

		if ( err != kNoError )
		{
			SetErrorNum( err );
		}
		
		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
			#if 0
			if ( err != kNoError && usType == iMPT_SYNC )
			{
				char temp[200];
				sprintf ( temp, "GMulPlay::ReceiveMessage - looking for SYNC, received an Error!");
				RandomLogComment ( temp );
			}
			#endif

			if ( err != kNoError && usType == iMPT_UNIT )
			{
				char temp[200];
				sprintf ( temp, "GMulPlay::ReceiveMessage - looking for UNIT, received an Error!");
				RandomLogComment ( temp );
			}
		}


		// -- Received a message without an error as a return code
		if ( err == kNoError )
		{
			// -- if the message is not a system message
			if ( (lpnm->MsgId > iMPT_SYSTEM) &&
					(lpnm->MsgId < iMPT_FILE) )
			{
				//---- get the player's index from the id
				PlayerInd = GetAPlayerIndex( lpnm->dwPlayerId );

				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					#if 0
					if ( usType == iMPT_SYNC )
					{
						char temp[200];
						sprintf ( temp, "GMulPlay::ReceiveMessage - looking for SYNC, received %s (%ld), FromID:%ld (ix=%ld)  MsgNum:%lx",
							((lpnm->MsgId==iMPT_SYNC)?"SYNC":"NON-SYNC"),lpnm->MsgId,
							lpnm->dwPlayerId,PlayerInd,lpnm->ulSync);
						RandomLogComment ( temp );
					}
					#endif

					if ( usType == iMPT_UNIT )
					{
						char temp[200];
						sprintf ( temp, "GMulPlay::ReceiveMessage - looking for UNIT, received %s (%ld), FromID:%ld (ix=%ld)  MsgNum:%lx",
							((lpnm->MsgId==iMPT_UNIT)?"UNIT":"NON-UNIT"),lpnm->MsgId,
							lpnm->dwPlayerId,PlayerInd,lpnm->ulSync);
						RandomLogComment ( temp );
					}
				}

				// -- handle out of range player indices
				if ( PlayerInd >= MAX_PLAYERS )
				{
					#ifdef _MULTIMONO
					mono_printf(0,16, "Received msg %d Bad Player Id %lu          ", 
											(short) lpnm->MsgId, lpnm->dwPlayerId );
					#endif
				}

				// -- make sure it's not an old message by checking against
				// --  last message number
				else if ( lpnm->ulSync > ulRcvMsgNum[ PlayerInd ] )
				{
					memcpy( &cRcvBuf[0], &lpnm->cData[0], iNMSize );

					iRcvType = (int) (lpnm->MsgId);

					// -- reset this
					ulRcvMsgNum[PlayerInd] = lpnm->ulSync;

					#ifdef _MULTIMONO
					mono_printf( 0,16, "Received msg %d Sync %lu Id %lu             ",
						(short) iRcvType,
						ulRcvMsgNum[PlayerInd],
						lpnm->dwPlayerId );
					#endif

					// -- If we are looking for anything, we found it!
					if ( usType == iMPT_ANY )
						usType = iRcvType;

					if ( iRcvType != usType )
					{
						LOGCOMMENTSLEEP
						if ( fLogComment )
						{
							#if 0
							if ( usType == iMPT_SYNC )
							{
								char temp[200];
								sprintf ( temp, "GMulPlay::ReceiveMessage - looking for sync, got wrong type, call HandleSpecialMessage");
								RandomLogComment ( temp );
							}
							#endif

							if ( usType == iMPT_UNIT )
							{
								char temp[200];
								sprintf ( temp, "GMulPlay::ReceiveMessage - looking for UNIT, got wrong type, call HandleSpecialMessage");
								RandomLogComment ( temp );
							}
						}

						#ifdef _MULTIMONO
							mono_puts ( 40,10, "RECEIVED WRONG MESSAGE" );
						#endif

						if ( !HandleSpecialMessages( (UCHAR) iRcvType, (long)lpnm->ulSync ) )
							break;
					}
				}

				else if ( fOneOnly )
				{
					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						#if 0
						if ( usType == iMPT_SYNC )
						{
							char temp[200];
							sprintf ( temp, "GMulPlay::ReceiveMessage - looking for sync, got old message and OneOnly, return Error!");
							RandomLogComment ( temp );
						}
						#endif

						if ( usType == iMPT_UNIT )
						{
							char temp[200];
							sprintf ( temp, "GMulPlay::ReceiveMessage - looking for UNIT, got old message and OneOnly, return Error!");
							RandomLogComment ( temp );
						}
					}

					#ifdef _MULTIMONO
						mono_printf( 40,15, "SYNC curr %lu got %lu ind %d",
							ulRcvMsgNum[ PlayerInd ],
							lpnm->ulSync,
							PlayerInd );
					#endif

					// -- leave anyway, old or not
					iRcvType	= iMPT_SYSTEM;

					#ifdef _MULTIMONO
						mono_printf(0,15, "fOneOnly old msg %d Sync %lu Id %lu             ", 
						  lpnm->MsgId, lpnm->ulSync, lpnm->dwPlayerId );
					#endif

					break;
				}

				#ifdef _MULTIMONO
				else
				{
					mono_printf( 0,14, "Received an old msg %d Sync %lu Id %lu            ",
						lpnm->MsgId, lpnm->ulSync, lpnm->dwPlayerId );
				}
				#endif

			} // end if my message

			#ifdef _MULTIMONO
			else 
			{
				mono_puts( 0,16, "Received a non-Birthright message" );
			}
			#endif

		}

		else if ( err == kXferRequestReceived )
		{
			#ifdef _MULTIMONO					
				mono_puts( 0,16, "Received a file xfer request" );
			#endif

			// -- If we are expecting a file, leave!

			if ( usType == iMPT_FILE )
			{
				iRcvType = usType;
				break;
			}
		}


		// -- Look for auxilliary messages, if we didn't find
		// --  what we wanted already 

		if ( (dwWait != 0) &&
			 (iRcvType != usType) )
		{
			DWORD dwTime = GetTickCount();
			
			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
				#if 0
				if ( usType == iMPT_SYNC )
				{
					char temp[200];
					sprintf ( temp, "GMulPlay::ReceiveMessage - call LookForMessages from inside");
					RandomLogComment ( temp );
				}
				#endif

				if ( usType == iMPT_UNIT )
				{
					char temp[200];
					sprintf ( temp, "GMulPlay::ReceiveMessage - call LookForMessages from inside");
					RandomLogComment ( temp );
				}
			}

			while ( (GetTickCount() - dwTime) < dwMsgWaitRS )
			{
				//---- service sound
				run_timers();

				if ( !LookForMessages(0) )
				{
					goto LeaveRcv;
				}

//
// This is done in gmulplay.cpp
//
//				Sleep( 50 );
			}

		}

		#if 0
		// GEH says "No quarter given, no quarter expected!"
		i++;

		if ( dwWait == 0 )
		{
			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
				char temp[200];
				sprintf ( temp, "GMulPlay::ReceiveMessage - Timed out in %ld loops!", i);
				RandomLogComment ( temp );
			}

			i = dwMsgMaxLoop;
		}
		#endif
		
		// -- Last ditch escape route!

		#ifdef _DEBUG
		if ( GetAsyncKeyState( VK_F10 ) & 0x8000 )
			break;
		#endif

	}


LeaveRcv:

	#ifdef _MULTIMONO					
	if ( (iRcvType != usType) && 
		 !fOneOnly               )
		mono_printf( 0,16, "Stopped trying to receive msg %d",
                    (short) usType );
	#endif

	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		if ( (usType != iRcvType) || (i == dwMsgMaxLoop) || (dwWait == 0) )
		{
			char temp[200];
			sprintf ( temp, "GMulPlay::ReceiveMessage - Stopped trying to receive msg %ld %ld %ld!", 
				usType, i, dwWait);
			RandomLogComment ( temp );
		}

		#if 0
		//if ( usType == iMPT_SYNC )
		{
			char temp[200];
			sprintf ( temp, "GMulPlay::ReceiveMessage - leaving, return %ld (0=error)", ( iRcvType == usType ));
			RandomLogComment ( temp );
		}
		#endif
	}

	return ( iRcvType == usType );


} // GenericMultiPlayer::ReceiveMessage




// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::SelectGuid
//		Select a game Id
//
//	 Inputs:
//		i		- game id
//
//	 Return: none
//
// -----------------------------------------------------------------
void GenericMultiPlayer::SelectGuid( int i )
{

	pnmGame.SelectGuid( i );

} // GenericMultiPlayer::SelectGuid




// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::SendFile
//		Send the designated file.
//
//	 Inputs:
//		pszFile		- fully-qualified path name of file to send
//
//	 Return: result of send
//
// -----------------------------------------------------------------
BOOL GenericMultiPlayer::SendFile( PSZ pszFile, unsigned int p_index )
{
	//GEH added the index to the arg list and moved loop up to mulplay.cpp
	//    for better sync
	unsigned int i = 0;
	//GEH unsigned int p_index;
	DWORD dwWait = dwMsgWaitS;
	DWORD toWhom = kBroadcast;

	 // -- Fudge factor for debugging!

	#ifdef _MULTIMONO					
	if ( mono_getExist() )
		dwWait *= 3;
	#endif


#if !defined(BROADCAST_SENDFILE)
	//GEH for (p_index=0; p_index < MAX_PLAYERS; p_index++)
	{
		DWORD id = GetAPlayerId( p_index );
		if ( id == GetPlayerId() || id == kNoPlayer)
			//GEH continue;
			return TRUE;
	
		i = 0;

       toWhom = id;

#endif

		fSent = FALSE;
		while ( !fSent && ( i < dwMsgMaxLoop) )
		{

			#ifdef _MULTIMONO					
			if ( mono_getExist() )
			{
			  #if !defined(BROADCAST_SENDFILE)
				mono_printf( 0,17, "Sending file %s to %d", pszFile, id );
			  #else	
				mono_printf( 0,17, "Sending file %s", pszFile );
			  #endif	
			}
			#endif

			//---- service sound
			run_timers();


			fSent = pnmGame.SendFile( toWhom, pszFile, iMPT_FILE, dwWait );

			i++;

			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
				if (fSent)
				{
					char temp[200];
					sprintf(temp, "send to %d(#%d) succeeded try %d", toWhom , p_index, i);
					RandomLogComment(temp);
				}
				else
				{
					char temp[200];
					sprintf(temp, "send to %d(#%d) failed try %d", toWhom , p_index, i);
					RandomLogComment(temp);
				}
			}

			// -- Last ditch escape route!

			#ifdef _DEBUG
			if ( GetAsyncKeyState( VK_F10 ) & 0x8000 )
				break;
			#endif

		
			Sleep(100);
		}

#if !defined(BROADCAST_SENDFILE)
		//GEH if (!fSent)	// any failure is bad; just abort
			//GEH break;
	}
#endif	

	return ( fSent );


} // GenericMultiPlayer::SendFile





// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::SendMessage
//		Send the message.
//
//	 Inputs:
//     dwToWhom    - Id to send message to 
//		usType  	- message type
//		sSize		- actual size of the message
//		fAck		- do we want an Ack?
//
//	 Return: none
//
// -----------------------------------------------------------------
BOOL GenericMultiPlayer::SendMessage( DWORD dwToWhom, USHORT usType, 
										short sSize, BOOL fAck )
{
	ErrCode err = kGenericError;
	unsigned int i = 0;
	DWORD dwWait = dwMsgWaitS+dwMsgFudge;
	int	count = 0;
	BOOL	localAcks[MAX_PLAYERS];
	int	DisconnectCount = 0;
	
	// -- Signal no acking with a wait of zero

	if ( !fAck )
	{
		dwWait = 0;
	}


	// -- Next message
	//ulSndMsgNum++;

	// assure that the message number is unique to this send
	ulSndMsgNum++;
	ulSndMsgNum &= 0x00FFFFFF;
	ulSndMsgNum |= (GetPlayerId() << 24);

	// -- Let us send
   #ifdef _MULTIMONO

	if ( mono_getExist() )
	{
		mono_printf( 0,17, "Sending msg %d Sync %lu          ", 
			(short) usType, ulSndMsgNum );
	}

   #endif



	while ( (err != kNoError) && ( i < dwMsgMaxLoop ) )
	{
		//---- service sound
		run_timers();

		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
			char temp[200];
			sprintf ( temp, "GenericMultiPlayer::SendMessage - usMsgType:%ld %s%s",
				 usType, (usType==iMPT_UNIT)?"iMPT_UNIT":" ", (usType==iMPT_PING)?"iMPT_PING":" " );
			RandomLogComment ( temp );
		}


		err = pnmGame.SendMessage( dwToWhom, 
									(byte) usType,
									(LPVOID) &cSndBuf[0], sSize, ulSndMsgNum,
									dwWait, (UCHAR) dwMsgTriesS );
		i++;

		// make a local copy of the ACK list
		if ( err != kNoError && dwToWhom == kBroadcast )
		{
			int j;
			char temp[200];
			
			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
				sprintf ( temp, " GenericMultiPlayer::SendMessage - initial localAcks : " );
				RandomLogPrefix ( temp );
				memset(&temp[0], 0, 100);
			}
			
			for (j=0; j < MAX_PLAYERS; j++ )
			{
				localAcks[j] = pnmGame.CheckOneAck(j);
				
				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					int id = GetAPlayerId(j); 
					if (id < 16)
						temp[j] = (char)((localAcks[j]) ? ('0'+id) : '0');
					else
						temp[j] = 'x';
				}
			}

			LOGCOMMENTSLEEP
			if ( fLogComment )
				RandomLogComment ( temp );

		}

HandleErrorRetry:

		//---- If there are not any players then fine

//		if ( err == kNoPlayersInGame &&
//		dwToWhom == kBroadcast     )
//		{
//			goto LeaveSend;
//		}

		//---- Player disconnected on send

		if ( err == kPlayerDisconnected )
		{
			goto LeaveSend;
		}

		// -- Last ditch escape route!
		#ifdef _DEBUG
		if ( GetAsyncKeyState( VK_F10 ) & 0x8000 )
			break;
		#endif

		if ( err != kNoError )
		{
			#if (defined _MULTIMONO)
			mono_printf ( 0, 18, "%d SendMessage Error %d   count = %d    ", i, err, ++count );
			#endif

			DWORD dwTime = GetTickCount();
			while ( (GetTickCount() - dwTime) < dwMsgWaitSR )
			{
				run_timers();				//---- service sound
				if (!LookForMessages(0))
					goto LeaveSend;
				//Sleep( 50 * GetAPlayerIndex( GetPlayerId() ));
				Sleep( 50 );
			}

			// if we ever reach here we need to try to point cast the message to
			// individual systems which have not yet responded

			if ( dwToWhom == kBroadcast )
			{

				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					int l;
					char temp[200];
					sprintf ( temp, " GenericMultiPlayer::SendMessage - resend localAcks  : " );
					RandomLogPrefix ( temp );
					memset(&temp[0], 0, 100);
				
					for (l=0; l < MAX_PLAYERS; l++ )
					{
						int id = GetAPlayerId(l); 
						if (id < 16)
							temp[l] = (char)((localAcks[l]) ? ('0'+id) : '0') ;
						else
							temp[l] = 'x';
					}
					RandomLogComment ( temp );
				}
	
				for (int j=0; j < MAX_PLAYERS; j++ )
				{
					DWORD id = GetAPlayerId( j );
					
					//---- find first player which failed to ACK
					if ( id != GetPlayerId()
							&& id != kNoPlayer
							&& !localAcks[j] )
					{
						err = pnmGame.SendMessage( id, 
									(byte) usType,
									(LPVOID) &cSndBuf[0], sSize, ulSndMsgNum,
									dwWait, 1 );
						
						LOGCOMMENTSLEEP
						if ( fLogComment )
						{
							//if (usType == iMPT_SYNC)
							{
								char temp[200];
								if (err == kNoError)
									sprintf ( temp, "GenericMultiPlayer::SendMessage - doing pointcast to %d, no error", id);
								else
									sprintf ( temp, "GenericMultiPlayer::SendMessage - doing pointcast to %d, FAILED", id);
								RandomLogComment ( temp );
							}
						}
						
						if (err == kNoError)				// set current ACK
							localAcks[j] = TRUE;
					}

					//---- check if all ACKs have come in
					int k;
					for (k=0; k < MAX_PLAYERS; k++ )
					{
						DWORD id2 = GetAPlayerId( k );

						// -- paint a green/red spot based on
						// -- whether they have acknowledged
						PaintAckLED(k, id2, localAcks[k]);
						
						if ( id2 != GetPlayerId()
								&& id2 != kNoPlayer
								&& !localAcks[k] )
							break;
					}

					if (k == 0 && ++DisconnectCount > 200 )
					{
						err = kConnectionLost;
						SetErrorNum( err );
						goto LeaveSend;
					}
					else
					//--- k == MAX_PLAYERS if everyone ACKed
					if (k == MAX_PLAYERS)
					{
						LOGCOMMENTSLEEP
						if ( fLogComment )
						{
							//if (usType == iMPT_SYNC)
							{
								char temp[200];
								sprintf ( temp, "GenericMultiPlayer::SendMessage - pointcast %d - received all sync ACKs", id);
								RandomLogComment ( temp );
							}
						}
               
						err = kNoError;
						goto LeaveSend;
					}
					
					err = kGenericError;
				}

				goto HandleErrorRetry;
			}

		}

	}

LeaveSend:


   #ifdef _MULTIMONO					
	if ( mono_getExist() )
	{
		if ( err == kNoError )
			mono_printf( 0, 18, "Sent msg %d Sync %lu                    ", 
	    			   	usType, ulSndMsgNum );
    	else
  			mono_printf( 0, 18, "Stopped trying to send msg %d", 
    					   	usType );
	}
   #endif

	return ( err == kNoError );

} // GenericMultiPlayer::SendMessage





BOOL GenericMultiPlayer::SendBroadcastMessage( USHORT usType, short sSize, BOOL fAck )
{
   return SendMessage( kBroadcast, usType, sSize, fAck );
}


BOOL GenericMultiPlayer::SendDirectMessage( DWORD dwToWhom, USHORT usType, short sSize, BOOL fAck )
{

    return SendMessage( dwToWhom, usType, sSize, fAck );

}


/* ========================================================================
   Function    - PaintAckLED
	Description - paint a green/red for each player - Green ack'd, read waiting
	Returns     - void
	======================================================================== */
#define	ACK_LED_X	632
#define	ACK_LED_Y	10
#define	ACK_LED_W	4
#define	ACK_LED_H	4
#define	ACK_LED_INC	8
#include "..\colors.h"
#include "..\engine.h"
void PaintAckLED(int k, DWORD id2, BOOL localAcks)
{
	LONG	color = (localAcks) ? GREEN : RED ;
	LONG	newY = ACK_LED_Y + (ACK_LED_INC * k);
	
	if (id2 != kNoPlayer)
	{
		color_rect( ACK_LED_X, newY, ACK_LED_W, ACK_LED_H, color );
		ScreenCopy( 0, ACK_LED_X, newY, ACK_LED_W, ACK_LED_H, SC_DEFAULT_RES);
	}
}

//----
//----  Dialog Windows
//----


#if 0
// -----------------------------------------------------------------
//
//	 GenericMultiPlayer::SetupChat
//
//		Setup the chat. Initialize chat stuff 
//
//	 Inputs:
//		none
//
//	 Return: none
//
// -----------------------------------------------------------------
void GenericMultiPlayer::SetupChat( void )
{

   
   memset ( &szChatMessages[0], 0, sizeof ( SZCHAT ) * iMaxChat );   

   sNextMessage = 0;



} // GenericMultiPlayer::SetupChat

#endif




#ifdef _MULTIMONO

// -- Debug statements from NetPlay.dll
static int DumpString( char *s )
{

#ifdef _MULTIMONO
    mono_puts( 0,19, s );
#endif

    return 1;

} // DumpString

#endif





// GMulPlay.cpp
