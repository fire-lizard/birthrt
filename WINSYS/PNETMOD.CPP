// -------------------------------------------------------------------
//
//		PNetMod.cpp
//
//		PlatformNetworkModule class
//
//		Programmed by:		Cheryl A. Makovsky
//								Synergistic Software
//
//		Modified by John McKinnie	May 1996
//
//		CAM	26 JUL 96	Added optional ReleaseDPlay to SetNetworkProtocol
//		CAM	 2 AUG 96	Added the option to use one of three GUID's
//     DLJ 25 SEP 96   Make it a multiplayer game again
//
//	**** CAM	4 OCT 96		Translated to Sigs	
//
//	    dlj 22 Oct 96   Put Sigs into this also    
//
// -------------------------------------------------------------------

// --- way slow
//GEH #define ACKSIGS

//#define LOGCOMMENTSLEEP Sleep(1);
#define LOGCOMMENTSLEEP {;}

#define iMPT_PING		6
#define iMPT_SYNC		20
#define iMPT_UNIT		8

#ifdef _DEBUG
#define _MULTIMONO    //---- Comment out for no mono
#endif

#define INITGUID
#include "Windows.h"
#include "..\WINSYS\PNETMOD.HXX"
#include "stdio.h"
#include "direct.h"
#include "memory.h"
#include "../SYSTEM.H"
#include "../MACHINE.H"
#include "../MENU.H"


//!!!!!!!!!!!!!!!!!!!!!!!!
#include "../UNITS.HXX"

//!!!!!!!!!!!!!!!!!!!!!!!!

#include "..\WINSYS\MONO_C.H"


#include "..\WINSYS\SNTYPES.H"
#include "..\WINSYS\SIERRANW.H"
#include "..\WINSYS\SNWVALID.H"


extern BOOL fLogComment;

#ifdef _WINDOWS
extern HWND hwndApp;
extern BOOL fQuitting;

#endif

#define NODIRECTSERIAL      // no direct play serial allowed 

extern float	RealmAIVersionNumber;

PNET_ERRCODE_TYPE gErrorVal = kNoError;

 // -- external prototypes

USHORT HashCRC ( PUCHAR lpMsg, USHORT usSize );


//---- Give me the null GUID 

DEFINE_GUID(GUID_NULL, 0L, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);


//---- Birthright GUID #1
//
// {EA7CC2E0-0C7B-11d0-81B8-00AA00470BE8}
DEFINE_GUID( GAME_GUID1, 
             0xea7cc2e0, 0xc7b, 0x11d0, 0x81, 0xb8, 0x0, 0xaa, 0x0, 0x47, 0xb, 0xe8);

//---- Birthright GUID #2
//
// {EA7CC2E1-0C7B-11d0-81B8-00AA00470BE8}
DEFINE_GUID( GAME_GUID2, 
             0xea7cc2e1, 0xc7b, 0x11d0, 0x81, 0xb8, 0x0, 0xaa, 0x0, 0x47, 0xb, 0xe8);

//---- Birthright GUID #3
//
// {EA7CC2E2-0C7B-11d0-81B8-00AA00470BE8}
DEFINE_GUID( GAME_GUID3, 
             0xea7cc2e2, 0xc7b, 0x11d0, 0x81, 0xb8, 0x0, 0xaa, 0x0, 0x47, 0xb, 0xe8);


const int MAX_ENUM_TIMEOUT  = (2500);   //---- Wait for 2 1/2 seconds the first time
const int MAX_ENUM_TIMEOUT2 = (2500);   //---- Wait for another 2 1/2 seconds

const int iMYACTIONINDEX	 = (0);

//---- Pointers to function of SIGS dll's																	   

static lpfnSNWValidateFunc			   lpfnSNWValidate				 = NULL;
static lpfnCheckGameConnectStatusFunc lpfnSNWCheckGameConnectStatus = NULL;
static lpfnGetNumberOfPlayersFunc	   lpfnSNWNumPlayers			 = NULL;
static lpfnSendTCPMessageFunc		   lpfnSNWSendTCP				 = NULL;
static lpfnSendTCPPointMessageFunc	   lpfnSNWSendTCPPoint			 = NULL;
static lpfnRecvTCPMessageFunc		   lpfnSNWRecvTCP	 			 = NULL;
static lpfnWaitForTCPMessageFunc	   lpfnSNWWaitForTCP			 = NULL;
static lpfnPeekForTCPMessageFunc	   lpfnSNWPeekForTCP			 = NULL;
static lpfnCheckForAnyTCPMessageFunc  lpfnSNWCheckForTCP			 = NULL;
static lpfnCheckPlayerConnectStatusFunc	lpfnSNWCheckPlayerConnectStatus = NULL;
static lpfnGetPlayerXNameFunc 		   lpfnSNWGetPlayerXName = NULL;

static lpfnTellMeSpecialChatReceivedFunc	lpfnSNWTellMeSpecialChatReceived 	= NULL;


const int WM_SIGSPCHAT = (WM_USER+5000);


static long GPInPort	= 0;
static long GPOutPort	= 0;

char GPServerIP[]="XXX.XXX.XXX.XXX";

char szGameName[] = "Birthright";

BOOL fSigs = FALSE;   // are we smokin ?



// -- structure for the service provider/session enumeration

typedef struct
{

	GameDisplayStringFunction pDisplayFunc;
	long lContext;
	PlatformNetworkModule *pnmThis;

} ENUMINFO, *LPENUMINFO;



// -- structure for the player enumeration
typedef struct
{

	GameDisplayStringFunction pEnumFunc;
	long lContext;
	PlatformNetworkModule *pnmThis;

} ENUMPINFO, *LPENUMPINFO;



typedef struct
{
   USHORT      usMsgType;	// what is it?
   BOOL        fMaster;    // from master ? 
   DWORD       dwFrom;     // from id ( 0  master ? )
} BRHeader, *PBRHeader;

typedef struct
{
	int	      iSync;                //---- which Sync are we at
} BRSync, *PBRSync;



 // -- local prototypes
void ErrorDPlay (PlatformNetworkModule *pPlatform, HRESULT hResult );


extern void RandomLogComment ( char * szString );


FP_DPENUM	PlatformNetworkModule::fp_DPEnum = NULL;		// DirectEnumProviders()
FP_DPCREATE	PlatformNetworkModule::fp_DPCreate = NULL;	// DirectPlayCreate()



// -----------------------------------------------------------------
//
//  PlatformNetworkModule::PlatformNetworkModule
//
//	Description:
//		Create a PlatformNetworkModule.
//		
//	Arguments:
//		theName	- pointer to the player name
//		NetworkModuleOwnsName	- if TRUE, NetworkModule is responsible
//											for deleting the name upon destruction
//
//	Return: none
//
// -----------------------------------------------------------------
PlatformNetworkModule::PlatformNetworkModule(GamePrintFunction pPrintFunc, ErrCode *err)
{

	*err = kNoError;

	InitVars(); // init vars and buffers

   pPrintFunction = pPrintFunc;


	fSigs = FALSE;

	gGuid = GAME_GUID1;


} // PlatformNetworkModule::PlatformNetworkModule




// -----------------------------------------------------------------
//
//  PlatformNetworkModule::~PlatformNetworkModule
//
//	Description:
//		Destroy a PlatformNetworkModule.
//		
//	Arguments:
//		none
//
//	Return: none
//
// -----------------------------------------------------------------
PlatformNetworkModule::~PlatformNetworkModule( void )
{

   ReleaseDPlay() ;


	if ( !fSigs )
	{
		if ( hDPlay != NULL )
			FreeLibrary( hDPlay );
	}


	if ( lpGenericMsgBuf != NULL )
		HeapFree( GetProcessHeap(), 0, lpGenericMsgBuf );

	if ( lpRecBuff != NULL )
		HeapFree ( GetProcessHeap(), 0, lpRecBuff );

	if ( lpSndBuff != NULL )
		HeapFree ( GetProcessHeap(), 0, lpSndBuff );


} // PlatformNetworkModule::~PlatformNetworkModule

	

// -----------------------------------------------------------------
//
//  PlatformNetworkModule::SetSigs
//
//	Description: Set sigs flag on 
//		
//	Arguments: BOOL  sigs or not 
//
//	Return: none
//
// -----------------------------------------------------------------
void PlatformNetworkModule::SetSigs( BOOL fPassSigs )
{

   fSigs = fPassSigs; 

}



// -----------------------------------------------------------------
//
//  PlatformNetworkModule::GetSigs
//
//	Description: get sigs flag 
//		
//	Arguments: 
//
//	Return: none
//
// -----------------------------------------------------------------
BOOL PlatformNetworkModule::GetSigs( void )
{

   return fSigs; 

}



// -----------------------------------------------------------------
//
//  PlatformNetworkModule::CheckConnectStatus
//
//	Description: Check connection status
//		
//	Arguments:
//
//	Return: none
//
// -----------------------------------------------------------------
int PlatformNetworkModule::CheckConnectStatus( void )
{

	if ( hDPlay != NULL &&
		 lpfnSNWCheckPlayerConnectStatus != NULL )
	{
		int i;
				  
	 	i = lpfnSNWCheckGameConnectStatus();

	}

 	return 1;

}



// -----------------------------------------------------------------
//
//  PlatformNetworkModule::AddPlayer
//
//	Description:
//		Add the given player to our player list for the network game.
//		
//	Arguments:
//		dwNewPlayerId - new player DirectPlay id
//
//	Return: TRUE if added player OK
//
// -----------------------------------------------------------------
BOOL PlatformNetworkModule::AddPlayer( DWORD dwNewPlayerId )
{
	BOOL fFound = FALSE;
	int y;



   //---- Don't add ourself to the list ( we don't want to talk to ourself )

   if ( dwNewPlayerId == dwPlayerId )
   {
       return ( FALSE );
   }


	// -- we cannot add new players once the game has started

	if ( TRUE == fGameInProgress )
   {
		return( FALSE );
   }



	//---- Make sure we don't have this player

	for ( y = 0; y < MAX_PLAYERS; y++ )
	{
		if ( dwPlayers[y] == dwNewPlayerId )
		{
			fFound = TRUE;
			break;
		}
	}



	//---- Don't add them again

	if ( fFound == FALSE )
	{
		 // -- Add the player to our local list

		for ( y = 0; y < MAX_PLAYERS; y++ )
		{
			if ( kNoPlayer == dwPlayers[y] )
			{
				dwPlayers[y] = dwNewPlayerId;
				dwDeleted[y] = 0;
				dwDeletedId[y] = kNoPlayer;
				break;
			}

		} // for each player

	} // if not already added player

	return ( TRUE );


} // PlatformNetworkModule::AddPlayer
	
	

// -----------------------------------------------------------------
//
//  PlatformNetworkModule::CancelNetworkGame
//
//	Description:
//
//		Cancel the network game exit the game
//		
//	Arguments:
//		none
//
//	Return: ErrCode
//
// -----------------------------------------------------------------
ErrCode PlatformNetworkModule::CancelNetworkGame( void )
{

   ReleaseDPlay() ;


	if ( !fSigs )
	{
		if ( hDPlay != NULL )
			FreeLibrary( hDPlay );
	}


	return ( kNoError );

} // PlatformNetworkModule::CancelNetworkGame

	

// -----------------------------------------------------------------
//
//  PlatformNetworkModule::CreateNetworkGame
//
//	Description:
//		Create the designated network game.
//		
//	Arguments:
//		pNetGameInfo	- network information
//
//	Return: ErrCode
//
// -----------------------------------------------------------------
ErrCode PlatformNetworkModule::CreateNetworkGame ( char *playername )
{
	ErrCode ecError = kNoError;
	HRESULT hResult = DP_OK;

	fGameCreator = FALSE;

	if ( fSigs )
	{
	    //---- means IsNetworkingAvailable call failed

		if ( hDPlay == NULL)	
			ecError = kNetworkingNotInstalled;

	}
	else  //---- Direct Play
	{
		if ( NULL == lpDP3A )
			ecError = kNoProtocolSelected;
	}


	if ( TRUE == fGameInProgress )
		ecError = kCannotCreateGame;


	//---- If this is SIGS 

	if ( fSigs )
	{

		if ( kNoError == ecError ) 
		{
			 // --
			 // -- Load the DLL
			 // --
			hDPlay = LoadLibrary ( "SierraNW.dll" );

			if ( hDPlay == NULL)	
			{
				ecError = kNetworkingNotInstalled;
				#ifdef _MULTIMONO
					mono_puts ( 0,1, "SierraNW.dll not found" );
				#endif
			}
			else
			{
				 // --
				 // -- Initialize the DLL
				 // --

				lpfnInitializeDLLFunc lpfnSNWInitialize = (lpfnInitializeDLLFunc)
								GetProcAddress( (HINSTANCE) hDPlay, "InitializeDLL" );

				if ( lpfnSNWInitialize != NULL )
				{
					BOOL fi = lpfnSNWInitialize( &GPServerIP[0], 
												 GPInPort, GPOutPort );

					 // --
					 // -- Wait until the users has left the Gathering Place
					 // --

					if ( fi )
					{
						int i = 0;

						lpfnSNWCheckGameConnectStatus = (lpfnCheckGameConnectStatusFunc)
								GetProcAddress( (HINSTANCE) hDPlay, "CheckGameConnectStatus" );

						do 
						{
							//---- service sound

							run_timers();

							if ( i == _GAME_STAT_PENDING )
							{

                               // -- Give SIGS a change to breath
                               // this allows starting of web page from sigs
                               // main menu 

                               Sleep( 100 );

                               // -- Deal with a message, if any
                               MSG msg;


                               if ( !fQuitting && PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) != 0 )
                               {
                                   TranslateMessage( &msg );
                                   DispatchMessage( &msg );
                               } // end if a message in the queue

							}

							i = lpfnSNWCheckGameConnectStatus();

 						    // -- emergency 'Get out of Dodge' switch

							#ifdef _DEBUG
							if ( GetAsyncKeyState( VK_F4 ) & 0x8000 )
							{
								ecError = kNetworkingNotInstalled;
								break;
							}
							#endif

                       
                           //----- If they quit then exit 
    
							if ( fQuitting )
							{
								ecError = kNetworkingNotInstalled;
								break;
							}

						} while ( i == _GAME_STAT_PENDING );


						// -- Did they just leave the gathering place?

						if ( i == _GAME_STAT_FAILURE )
						{
							ecError = kGameAborted;
						}
						else
						{
							lpfnHideGatheringPlaceFunc lpfnSNWHideGatheringPlace = (lpfnHideGatheringPlaceFunc)
											GetProcAddress( (HINSTANCE) hDPlay, "HideGatheringPlace" );

// dlj 6-10-97 don't hide 	     lpfnSNWHideGatheringPlace();

						}

 					}
					else
					{
						ecError = kNetworkingNotInstalled;
					}

				} // end if InitializeDll
				else
				{
					ecError = kNetworkingNotInstalled;
				}
			}

			if ( ecError != kNoError )
			{
				ecError = kCannotCreateGame;
				ErrorDPlay(hResult );
			}
			else 
			{

			} // if network game was created

		} // if OK to create


		 // --
		 // -- Set information
		 // --

		if ( ecError != kNoError )
		{
			dwPlayerId = kNoPlayer;

			ReleaseDPlay();

		}
		else
		{
			// -- Get my player id

			lpfnGetMyPlayerNumberFunc lpfnSNW = (lpfnGetMyPlayerNumberFunc)
							GetProcAddress( (HINSTANCE) hDPlay, "GetMyPlayerNumber" );

			dwPlayerId = (DWORD) lpfnSNW();


			if ( dwPlayerId == 0 )
			{
				fGameCreator = TRUE;
			}
			else
			{
				fGameCreator = FALSE;
			}

			//---- Get my name ( it might be different than DPlay )

			lpfnSNWGetPlayerXName = (lpfnGetPlayerXNameFunc)
								     GetProcAddress( (HINSTANCE) hDPlay, "GetPlayerXName" );

		    lpfnSNWGetPlayerXName( &szPlayerName[0], dwPlayerId );


           //---- HACK move in the player name received  dlj 6-10-97

           strncpy( playername, szPlayerName, 19 );
           playername[19] = 0;



			lpfnSNWNumPlayers = (lpfnGetNumberOfPlayersFunc)
								GetProcAddress( (HINSTANCE) hDPlay, "GetNumberOfPlayers" );


			// -- Get all the player's ID's

			for ( unsigned short i = 0; i < lpfnSNWNumPlayers( NULL ); i++ )
			{
				AddPlayer( i ); 	//--- add player will skip our player id 
			}


			// -- Get the ProcAddresses for a bunch of items

			lpfnSNWSendTCP	= (lpfnSendTCPMessageFunc) 
								 GetProcAddress( (HINSTANCE) hDPlay, "SendTCPMessage");

			lpfnSNWSendTCPPoint = (lpfnSendTCPPointMessageFunc)
									 GetProcAddress( (HINSTANCE) hDPlay, "SendTCPPointMessage");

			lpfnSNWRecvTCP = (lpfnRecvTCPMessageFunc)
							   	GetProcAddress( (HINSTANCE) hDPlay, "RecvTCPMessage" );

			lpfnSNWWaitForTCP = (lpfnWaitForTCPMessageFunc)
								   GetProcAddress( (HINSTANCE) hDPlay, "WaitForTCPMessage" );

			lpfnSNWPeekForTCP = (lpfnPeekForTCPMessageFunc)
								   GetProcAddress( (HINSTANCE) hDPlay, "PeekForTCPMessage" );

			lpfnSNWCheckForTCP = (lpfnCheckForAnyTCPMessageFunc)
									GetProcAddress( (HINSTANCE) hDPlay, "CheckForAnyTCPMessage" );

			lpfnSNWCheckPlayerConnectStatus = (lpfnCheckPlayerConnectStatusFunc)
												 GetProcAddress( (HINSTANCE) hDPlay, "CheckPlayerConnectStatus" );


			// -- Disable private chat
			lpfnSNWTellMeSpecialChatReceived = (lpfnTellMeSpecialChatReceivedFunc)
										GetProcAddress( (HINSTANCE) hDPlay, "TellMeSpecialChatReceived" );

			lpfnSNWTellMeSpecialChatReceived( SIGS_POST_MESSAGE, NULL, hwndApp, WM_SIGSPCHAT );
		
		
		} // end if we are playing!



	}
	else   //---- Direct Play
	{


		if ( kNoError == ecError ) 
		{

			DPSESSIONDESC2 dpDesc2;
			char   szUserName[PLAYERNAME_LEN + 1];
			DWORD  dwUNSize = PLAYERNAME_LEN + 1;
//			DPCAPS dpCaps;

			memset ( szUserName, 0, PLAYERNAME_LEN + 1 );


			memset( &dpDesc2, 0x00, sizeof(DPSESSIONDESC2) );
			dpDesc2.dwSize = sizeof(dpDesc2);
			dpDesc2.dwMaxPlayers = ngiInfo.usMaxPlayers;
			dpDesc2.dwFlags = 0; 
			
			dpDesc2.guidApplication = GAME_GUID1; // gGuid;							//---- unique GUID

			sprintf ( ngiInfo.szGameNamePlus, "%s%c%3.2f",
                   ngiInfo.szGameName,
                   0x86,  
                   RealmAIVersionNumber );             

			dpDesc2.lpszSessionNameA = ngiInfo.szGameNamePlus;

			dpDesc2.dwUser1 = ngiInfo.dwUser1;
			dpDesc2.dwUser2 = ngiInfo.dwUser2;
			dpDesc2.dwUser3 = ngiInfo.dwUser3;
			dpDesc2.dwUser4 = ngiInfo.dwUser4;

       
			//---- Create a new networked game

			hResult = lpDP3A->Open( &dpDesc2, DPOPEN_CREATE );

			if ( hResult != DP_OK )
			{
				ecError = kCannotCreateGame;
				ErrorDPlay(hResult );
			}
			else 
      	{

			    // -- Make me a player!

				GetUserName( szUserName,  &dwUNSize );	

				DPNAME dpName;
				BuildDPNameA( &dpName, szUserName, playername );

				hResult = lpDP3A->CreatePlayer( &dwPlayerId, 
														  &dpName,
														  NULL,
													     NULL,
														  NULL,
														  NULL );

				if ( hResult != DP_OK)
				{
					ecError = kCannotCreatePlayer;
	                ErrorDPlay( hResult );


	 				hResult = lpDP3A->Close();

					if( hResult != DP_OK )
						ErrorDPlay( hResult );

				}
				else
				{
					//----- Get info about the session we are joining 

					memset(&dpDesc2, 0x00, sizeof(DPSESSIONDESC2));
					dpDesc2.dwSize       = sizeof(dpDesc2);

					DWORD dwTemp = sizeof(DPSESSIONDESC2);
					
					lpDP3A->GetSessionDesc(	(LPVOID) &dpDesc2,
													&dwTemp );

					 //---- Save our session
					guidInstance = dpDesc2.guidInstance;
					strncpy( szPlayerName, playername, 256 ) ;
					fGameCreator = TRUE;
				}

			} // if network game was created

		} // if OK to create

	}	//---- If SIGS or DPLAY


	return ( ecError );



} // PlatformNetworkModule::CreateNetworkGame
	

	
// -----------------------------------------------------------------
//
//  PlatformNetworkModule::GetDeletedPlayer
//
//	Description: Returns deleted player 
//		
//	Arguments:
//		index - index of player 
//
//	Return: none
//
// -----------------------------------------------------------------
int PlatformNetworkModule::GetDeletedPlayer  ( int index )
{

	 return	dwDeleted[index]; 

}


// -----------------------------------------------------------------
//
//  PlatformNetworkModule::GetDeletedPlayerId
//
//	Description: Returns deleted player id
//		
//	Arguments:
//		index - index of player 
//
//	Return: none
//
// -----------------------------------------------------------------
DWORD PlatformNetworkModule::GetDeletedPlayerId  ( int index )
{

	 return	dwDeletedId[index]; 

}


// -----------------------------------------------------------------
//
//  PlatformNetworkModule::ResetDeletedPlayer
//
//	Description: Resets delete player clear the flag and Id
//		
//	Arguments:
//		index - index of player 
//
//	Return: none
//
// -----------------------------------------------------------------
void PlatformNetworkModule::ResetDeletedPlayer  ( int index )
{

	 dwDeletedId[index] = kNoPlayer; 
	 dwDeleted[index] = 0; 

}



// -----------------------------------------------------------------
//
//  PlatformNetworkModule::DeletePlayer
//
//	Description:
//		Delete the designated player from the game.
//		
//	Arguments:
//		dwDelPlayerId	- Id of player to delete from the game
//
//	Return: none
//
// -----------------------------------------------------------------
void PlatformNetworkModule::DeletePlayer( DWORD dwDelPlayerId )
{
	int y;

	for ( y = 0; y < MAX_PLAYERS; y++ )
	{

		if ( dwDelPlayerId == dwPlayers[y] )
		{
			#ifdef _MULTIMONO
       	mono_printf ( 40, 20, "Deleting player %lu",
					  	  dwPlayers[y] );
			#endif

			dwDeleted[y]   = 1;   			//---- Set deleted flag
			dwDeletedId[y] = dwPlayers[y];	//---- Set deleted id
			dwPlayers[y]   = kNoPlayer; 	//---- Set to empty slot


			break;
		}

	} // for each player

} // PlatformNetworkModule::DeletePlayer
	
	


// -----------------------------------------------------------------
//
//  PlatformNetworkModule::EnumerateNetworkGames
//
//	Description:
//		Enumerate the network games available for the previously
//			selected protocol.
//		
//	Arguments:
//		theDisplayStringFunc	- function to display the protocol name string
//		theContext				- the display context; passed to theDisplayStringFunc
//
//	Return: ErrCode
//
// -----------------------------------------------------------------
ErrCode PlatformNetworkModule::EnumerateNetworkGames
                              (GameDisplayStringFunction theDisplayStringFunc,
                               long theContext)
{
	if ( fSigs )
	{
		return ( kGenericError );
	}
	else
	{
		ENUMINFO	eiInfo;
		HRESULT	hResult = DP_OK;
		ErrCode ecError = kNoError;
		DPSESSIONDESC2 dpDesc2;

		 // -- Enumerate the games
		usSessionIdx = 0;
		usNumSessions = 0;

		eiInfo.pDisplayFunc = theDisplayStringFunc;
		eiInfo.lContext = theContext;
		eiInfo.pnmThis = this;

		memset( &dpDesc2, 0x00, sizeof(DPSESSIONDESC2) );
		dpDesc2.dwSize = sizeof(dpDesc2);
		dpDesc2.guidApplication = GAME_GUID1; // gGuid;        //---- Unique GUID

		fTimedout = FALSE;

		 //----- Enumerate all available sessions
		hResult = lpDP3A->EnumSessions( &dpDesc2, 
												  NULL,                 // MAX_ENUM_TIMEOUT,
                                      EnumSession, 
												  (LPVOID) &eiInfo,
	                                   DPENUMSESSIONS_AVAILABLE );

		if ( (hResult != DP_OK) ||
			 (0 == usNumSessions)  )
		{
			ecError = kNoNetworkGamesAvailable;	

      	if ( hResult == DP_OK )
			{
	        	ErrorDPlay(hResult );
   	  	}

		}

		return ( ecError );

	}


} // PlatformNetworkModule::EnumerateNetworkGames
	
	


// -----------------------------------------------------------------
//
//  PlatformNetworkModule::EnumerateNetworkProtocols
//
//	Description:
//		Enumerate the network protocols available.
//		
//	Arguments:
//		theDisplayStringFunc	- function to display the protocol name string
//		theContext				- the display context; passed to theDisplayStringFunc
//
//	Return: ErrCode
//
// -----------------------------------------------------------------
ErrCode PlatformNetworkModule::EnumerateNetworkProtocols
                              ( GameDisplayStringFunction theDisplayStringFunc,
												long theContext )
{
	if ( fSigs )
	{
		return ( kGenericError );
	}
	else
	{

		ENUMINFO	eiInfo;
		HRESULT	hResult = DP_OK;
		ErrCode ecError = kNoError;

		 // -- Enumerate the providers
		usProviderIdx = 0;
		usNumProviders = 0;

		eiInfo.pDisplayFunc = theDisplayStringFunc;
		eiInfo.lContext = theContext;
		eiInfo.pnmThis = this;

		hResult = (( fp_DPEnum ) ( EnumServiceProvider, (LPVOID) &eiInfo ) );

		if ( (hResult != DP_OK) ||
			 (0 == usNumProviders) )
		{
			ecError = kNoNetworkProtocolsAreAvailable;	
		}

		return ( ecError );

	}


} // PlatformNetworkModule::EnumerateNetworkProtocols
	
	


// -----------------------------------------------------------------
//
//  PlatformNetworkModule::EnumeratePlayers
//
//	Description:
//		Enumerate the players available in the current game.
//		
//	Arguments:
//		theEnumeratePlayersFunc	- function to display the protocol name string
//		theContext				- the display context; passed to theEnumeratePlayersFunc
//
//	Return: ErrCode
//
// -----------------------------------------------------------------
ErrCode PlatformNetworkModule::EnumeratePlayers
	                     ( GameDisplayStringFunction theEnumeratePlayersFunc,
					             long theContext )
{

	if ( fSigs )
	{
		char szBuff[MAX_PATH] = "";


		if ( hDPlay == NULL )
			return ( kGenericError );

		if ( lpfnSNWNumPlayers == NULL )
			return ( kGenericError );


		//---- Get all the players names

		for ( short i = 0; i < MAX_PLAYERS; i++ )
		{
			//---- for every player here look them up 

			if ( dwPlayers[i] != kNoPlayer )
			{
			    lpfnSNWGetPlayerXName( &szBuff[0], dwPlayers[i] );
			    theEnumeratePlayersFunc( 0, &szBuff[0], dwPlayers[i], NULL );
			}

		}

		return ( kNoError );
	}
	else
	{
		ErrCode ecError = kNoError;
		HRESULT hResult = DP_OK;
		ENUMPINFO	epiInfo;


		if ( dwPlayerId != kNoPlayer )
		{
			epiInfo.pEnumFunc = theEnumeratePlayersFunc;
			epiInfo.lContext  = theContext;
			epiInfo.pnmThis  = this;

			hResult = lpDP3A->EnumPlayers( NULL, EnumPlayers, 
													(LPVOID) &epiInfo, 0 );

			if ( (hResult != DP_OK) ||
				 (0 == usNumPlayers)   )
			{
				ecError = kNoPlayersInGame;	
			}
		}
		else
			ecError = kNoPlayersInGame;

		return ( ecError );
	}

} // PlatformNetworkModule::EnumeratePlayers


	

// -----------------------------------------------------------------
//
//  PlatformNetworkModule::EnumPlayers
//
//	Description:
//		Enumerate the players available.
//		
//	Arguments:
//
//	Return: BOOL
//
// -----------------------------------------------------------------
BOOL FAR PASCAL EnumPlayers( DPID dpID,
									  DWORD dwPlayerType, 	
   								  LPCDPNAME lpcdpName,
									  DWORD flags, 
									  LPVOID lpContext )
{

	if ( fSigs )
	{
		return ( kGenericError );
	}
	else
	{
		LPENUMPINFO pepiInfo = (LPENUMPINFO) lpContext;


		if( dwPlayerType==DPPLAYERTYPE_PLAYER )
		{

			pepiInfo->pEnumFunc( pepiInfo->lContext, lpcdpName->lpszShortNameA,
										dpID, NULL );

			// -- note this for the calling routine

			pepiInfo->pnmThis->usNumPlayers++;

		}


		return(TRUE);

	}
	

} // PlatformNetworkModule::EnumPlayers


	

// -----------------------------------------------------------------
//
//  PlatformNetworkModule::EnumPlayersInit
//
//	Description:
//		Enumerate the players available, placing them into the dwPlayers
//			array.
//		
//	Arguments:
//
//	Return: BOOL
//
// -----------------------------------------------------------------

BOOL FAR PASCAL EnumPlayersInit( DPID dpID, 
										   DWORD dwPlayerType,
											LPCDPNAME lpcdpName, 
                                 DWORD,
                                 LPVOID lpContext )
{
	if ( fSigs)
	{
		return ( kGenericError );
	}
	else
	{
		PlatformNetworkModule * pnmGame = (PlatformNetworkModule *) lpContext;

		if( dwPlayerType==DPPLAYERTYPE_PLAYER )
			pnmGame->AddPlayer( dpID );

		return(TRUE);
	}

} // PlatformNetworkModule::EnumPlayersInit


	

// -----------------------------------------------------------------
//
//  PlatformNetworkModule::EnumServiceProvider
//
//	Description:
//		Enumerate the network protocols available.
//		
//	Arguments:
//
//	Return: BOOL
//
// -----------------------------------------------------------------
BOOL FAR PASCAL EnumServiceProvider( LPGUID lpSPGuid,
												 LPSTR lpDriverDesc,
                                     DWORD,
												 DWORD,
                                     LPVOID lpContext )
{

	if ( fSigs )
	{
		return ( kGenericError );
	}
	else
	{

#ifdef NODIRECTSERIAL
		if ( *lpSPGuid !=	DPSPGUID_SERIAL )
		{
#endif
			IUnknown *pUnk;

			HRESULT hr = (PlatformNetworkModule::fp_DPCreate) ( lpSPGuid, &pUnk, NULL );

			if( hr != DP_OK )
			{
				// provider isn't supported; don't add to list
				return TRUE;
			}

			pUnk->Release();


		   LPENUMINFO peiInfo = (LPENUMINFO) lpContext;
			PlatformNetworkModule *pNM = (PlatformNetworkModule *)peiInfo->pnmThis;

		    // -- Note this provider's GUID for later

			if ( pNM->usProviderIdx < iMAXPROVIDERS )
			{
				 // -- Send the string to the provided display function
				peiInfo->pDisplayFunc( peiInfo->lContext, 
											  lpDriverDesc,
											  pNM->usProviderIdx,
											  (PNETGAMEINFO) lpSPGuid );

				pNM->guidProviders[pNM->usProviderIdx] = *lpSPGuid;
				pNM->usProviderIdx++;
				pNM->usNumProviders++;
		
			}

#ifdef NODIRECTSERIAL
		}
#endif

		return (TRUE);

	}


} // PlatformNetworkModule::EnumServiceProvider

	


// -----------------------------------------------------------------
//
//  PlatformNetworkModule::EnumSession
//
//	Description:
//		Enumerate the network games available to the current protocol.
//		
//	Arguments:
//
//	Return: BOOL
//
// -----------------------------------------------------------------
BOOL FAR PASCAL EnumSession( LPCDPSESSIONDESC2	lpDPSGameDesc2,
                             LPDWORD				lpdwTimeOut,
                             DWORD					dwFlags,         
                             LPVOID					lpContext )
{

	if ( fSigs )
	{
		return ( kGenericError );
	}
	else
	{

		LPENUMINFO lpInfo = (LPENUMINFO) lpContext;
		PlatformNetworkModule *pNM = (PlatformNetworkModule *)lpInfo->pnmThis;
		char szChar[256];
		

   	 //----- If we timeout something is wrong ? so abort
		if ( dwFlags & DPESC_TIMEDOUT )
		{
			if ( pNM->fTimedout == FALSE )
			{
				(*lpdwTimeOut) = MAX_ENUM_TIMEOUT;     //--- Try alittle longer
				lpInfo->pnmThis->fTimedout = TRUE;
				return ( TRUE );
			}

			return( FALSE );
		}


		// Determine if this sessions GUID is already in our list; if so,
		// return TRUE to continue, but don't add a second copy of it.

		for( int iSession=0; iSession < lpInfo->pnmThis->usSessionIdx; iSession++ )
		{
			if( lpInfo->pnmThis->guidSessions[iSession] == lpDPSGameDesc2->guidInstance )
				return TRUE;
		}


		lpInfo->pnmThis->fModem = FALSE;



      //
      //---- This is a modem game so fake out some values for direct play
      //---- Since DP fills max players with 16 we have to change this to 2
      //

//      if (strncmp(lpDPSGameDesc->szSessionName, "Dial", 4 ) == 0) 
//	   {
//			peiInfo->pnmThis->fModem = TRUE;
//       	peiInfo->pnmThis->ngiInfos[peiInfo->pnmThis->usSessionIdx].usMaxPlayers = 2; // (unsigned short) lpDPSGameDesc->dwMaxPlayers;
//		}
//		else 
//   	{
//       	peiInfo->pnmThis->ngiInfos[peiInfo->pnmThis->usSessionIdx].usMaxPlayers = (unsigned short) lpDPSGameDesc->dwMaxPlayers;
//		}


    	sprintf ( szChar, "%-32.32s", lpDPSGameDesc2->lpszSessionNameA );

		// -- add to the list

		if ( lpInfo->pnmThis->usSessionIdx < iMAXSESSIONS ) 
   	{
			NETGAMEINFO *pNGI = &lpInfo->pnmThis->ngiInfos[lpInfo->pnmThis->usSessionIdx];

			 // -- display the session name
	    	strncpy( pNGI->szGameName, lpDPSGameDesc2->lpszSessionNameA, 32) ;

			pNGI->usMaxPlayers = ( unsigned short ) lpDPSGameDesc2->dwMaxPlayers;

       	pNGI->dwUser1 = lpDPSGameDesc2->dwUser1 ;
       	pNGI->dwUser2 = lpDPSGameDesc2->dwUser2 ;
       	pNGI->dwUser3 = lpDPSGameDesc2->dwUser3 ;
       	pNGI->dwUser4 = lpDPSGameDesc2->dwUser4 ;

       	lpInfo->pDisplayFunc( lpInfo->lContext, szChar, lpInfo->pnmThis->usSessionIdx, pNGI );

			lpInfo->pnmThis->guidSessions[lpInfo->pnmThis->usSessionIdx] = lpDPSGameDesc2->guidInstance;

			lpInfo->pnmThis->usSessionIdx++;
			lpInfo->pnmThis->usNumSessions++;
		}

		return(TRUE);

	}

} // PlatformNetworkModule::EnumSession
	
//----------------------------------------------------------------------------
//	 PlatformNetworkModule::BuildDPNameA
//
//	 Description:
//
//		Build an ASCII DPNAME for DirectPlay3A interface calls.
//
//	 Arguments:
//		pdpName				Pointer to a DPNAME struct
//		pszFormalName		Pointer to a null-terminated formal name string.
//		pszFriendlyName	Pointer to a null-terminated friendly name string.
//
//	 Return:
//		pdpName filled in with correct values.
//
//
//----------------------------------------------------------------------------
void PlatformNetworkModule::BuildDPNameA( LPDPNAME pdpName,
														char *pszFormalName,
														char *pszFriendlyName )
{
	pdpName->dwSize				= sizeof( DPNAME );
	pdpName->lpszLongNameA		= pszFormalName;
	pdpName->lpszShortNameA		= pszFriendlyName;
} /* BuildDPNameA */



//----------------------------------------------------------------------------
//  PlatformNetworkModule::ErrorDPlay
//
//  Description:
//
//		Sends direct play errors to debug window
//
//  Arguments: Result
//
//
//  Return :
//
//
//----------------------------------------------------------------------------
void PlatformNetworkModule::ErrorDPlay (HRESULT hResult ) 
{

#ifdef _MULTIMONO

	if ( fSigs )
	{
		return;
	}
	else
	{

    switch ( hResult ) 
    {
      case DP_OK:
         strncpy(szLastErrorMessage,"DPlay No Error",255) ;
      break;

      case DPERR_ALREADYINITIALIZED:
         strncpy(szLastErrorMessage,"DPlay Already Initialized",255) ;
      break;

      case DPERR_ACCESSDENIED:
         strncpy(szLastErrorMessage, "DPlay Access Denied", 255 );
      break;

      case DPERR_ACTIVEPLAYERS:
         strncpy(szLastErrorMessage, "DPlay Active Players", 255 );
      break;

      case DPERR_BUFFERTOOSMALL:
         strncpy(szLastErrorMessage, "DPlay Buffer too small", 255 );
      break;

      case DPERR_CANTADDPLAYER:
         strncpy(szLastErrorMessage, "DPlay can't add player", 255 );
      break;

      case DPERR_CANTCREATEGROUP:
         strncpy(szLastErrorMessage, "DPlay can't create group", 255 );
      break;

      case DPERR_CANTCREATEPLAYER:
         strncpy(szLastErrorMessage, "DPlay can't create player", 255 );
      break;

      case DPERR_CANTCREATESESSION:
         strncpy(szLastErrorMessage, "DPlay can't create session", 255 );
      break;

      case DPERR_CAPSNOTAVAILABLEYET:
         strncpy(szLastErrorMessage, "DPlay caps not available yet", 255 );
      break;

      case DPERR_EXCEPTION:
         strncpy(szLastErrorMessage, "DPlay exception", 255 );
      break;

      case DPERR_GENERIC:
         strncpy(szLastErrorMessage, "DPlay generic", 255 );
      break;

      case DPERR_INVALIDFLAGS:
         strncpy(szLastErrorMessage, "DPlay invalid flags", 255 );
      break;

      case DPERR_INVALIDOBJECT:
         strncpy(szLastErrorMessage, "DPlay invalid object", 255 );
      break;

      case DPERR_INVALIDPARAM:
         strncpy(szLastErrorMessage, "DPlay invalid param", 255 );
      break;

      case DPERR_INVALIDPLAYER:
         strncpy(szLastErrorMessage, "DPlay invalid player", 255 );
      break;

      case DPERR_NOCAPS:
         strncpy(szLastErrorMessage, "DPlay no caps", 255 );
      break;

      case DPERR_NOCONNECTION:
         strncpy(szLastErrorMessage, "DPlay no connection", 255 );
      break;

      case DPERR_NOMEMORY:
         strncpy(szLastErrorMessage, "DPlay no memory", 255 );
      break;

      case DPERR_NOMESSAGES:
         strncpy(szLastErrorMessage, "DPlay no messages", 255 );
      break;

      case DPERR_NONAMESERVERFOUND:
         strncpy(szLastErrorMessage, "DPlay no name server found", 255 );
      break;

      case DPERR_NOPLAYERS:
         strncpy(szLastErrorMessage, "DPlay no players", 255 );
      break;

      case DPERR_NOSESSIONS:
         strncpy(szLastErrorMessage, "DPlay no sessions", 255 );
      break;

      case DPERR_SENDTOOBIG:
         strncpy(szLastErrorMessage, "DPlay send too big", 255 );
      break;

      case DPERR_TIMEOUT:
         strncpy(szLastErrorMessage, "DPlay timeout", 255 );
      break;

      case DPERR_UNAVAILABLE:
         strncpy(szLastErrorMessage, "DPlay unavailable", 255 );
      break;

      case DPERR_UNSUPPORTED:
         strncpy(szLastErrorMessage, "DPlay unsupported", 255 );
      break;

      case DPERR_BUSY:
         strncpy(szLastErrorMessage, "DPlay Busy", 255 );
      break;

      case DPERR_USERCANCEL:
         strncpy(szLastErrorMessage, "DPlay User Cancel", 255 );
      break;

      default:
         strncpy(szLastErrorMessage, "Unknown DPlay Error", 255 );
      break;
       
    }

	 #ifdef _MULTIMONO
	    mono_printf ( 0, 20, "%s", szLastErrorMessage );
    #endif
      
    return;

	}

#else

	//---- satisfy the compiler

	if (hResult)
	{
	}

#endif

}  //---- End of ErrorDPlay ()



// -----------------------------------------------------------------
//
//  PlatformNetworkModule::GetLocalPlayerId
//
//	Description:
//		Return the local player ID.
//		
//	Arguments:
//		thePlayerId		- pointer to return value
//
//	Return: ErrCode
//
// -----------------------------------------------------------------
ErrCode PlatformNetworkModule::GetLocalPlayerID ( long* thePlayerID )
{
	(*thePlayerID) = dwPlayerId;

	if ( dwPlayerId != kNoPlayer )
		return ( kNoError );
	else
		return ( kNoGameStarted );

} // PlatformNetworkModule::GetLocalPlayerID


// -----------------------------------------------------------------
//
//  PlatformNetworkModule::GetOtherPlayerID
//
//	Description:
//		Return the player ID based on index.
//		
//	Arguments:
//		thePlayerId		- pointer to return value
//
//	Return: ErrCode
//
// -----------------------------------------------------------------
ErrCode PlatformNetworkModule::GetOtherPlayerID ( long* thePlayerID, int index )
{
	(*thePlayerID) = dwPlayers[index] ;

	if ( dwPlayers[index] != kNoPlayer )
		return ( kNoError );
	else
		return ( kNoGameStarted );

} // PlatformNetworkModule::GetOtherPlayerID
	



#if 0

// -----------------------------------------------------------------
//
//  PlatformNetworkModule::GetMessage
//
//	Description:
//
//		Return the last generic message received, if any.
//     Eats any ACKS in message que.
//		
//	Arguments:
//		ptheMessage	- pointer to message area must be MAX_NETBUFF 
//     bytes in size
//		
//		
//
//	Return: ErrCode
//
// -----------------------------------------------------------------
ErrCode PlatformNetworkModule::GetMessage(	LPNETMSG ptheMessage,
															DWORD dwWait,
															const char bRemove, 
															BOOL fAck )
{
	ErrCode ecError = kNoError;
	
	do 
	{
		ecError = ReceiveMessages( dwWait, bRemove, (fAck ? kACK : kDONTACK) );
		if ( kNoError != ecError )
		{
			return ecError ;
		}
	} while (lpGenericMsgBuf->MsgId == ACK) ;
	
	memcpy( ptheMessage, lpGenericMsgBuf, MAX_NETBUFF );
	
	return ( ecError );

} // PlatformNetworkModule::GetMessage
#endif


// -----------------------------------------------------------------
//
//  PlatformNetworkModule::GetNetworkGameInfo
//
//	Description:
//		Return the network game information.
//		
//	Arguments:
//		pNetgameInfo	- pointer to info to return
//
//	Return: ErrCode
//
// -----------------------------------------------------------------
PNETGAMEINFO PlatformNetworkModule::GetNetworkGameInfo  ()
{

	return &ngiInfo ;

} // PlatformNetworkModule::GetNetworkGameInfo
	



// -----------------------------------------------------------------
//
//  PlatformNetworkModule::InitVars
//
//	Description:
//		Initialize various variables.
//		
//	Arguments:
//		none
//
//	Return: none
//
// -----------------------------------------------------------------
void PlatformNetworkModule::InitVars( void )
{
	int x;

	dwPlayerId = kNoPlayer;
	guidInstance  = GUID_NULL;
	fGameInProgress = FALSE;

	// -- clear the game info structure

	memset ( &ngiInfo, 0, sizeof(ngiInfo) );

	for ( x=0; x < MAX_PLAYERS; x++ )
	{
	   dwPlayers[x] = kNoPlayer;
	   dwDeleted[x] = 0;
	   dwDeletedId[x] = kNoPlayer;
	}


	ngiInfo.usMaxPlayers = 2;    //---- Default to 2 players


	hDPlay		= NULL;


	// -- DirectPlay items

	if ( !fSigs )
	{
		fp_DPEnum	= NULL;
		fp_DPCreate = NULL;
		dphEvent		= NULL;
		lpDP  		= NULL;
		lpDP3A		= NULL;

	}

	// -- allocate various buffers

	lpGenericMsgBuf = (LPNETMSG) HeapAlloc( GetProcessHeap(), 0, MAX_NETBUFF );

	lpRecBuff       = (LPNETMSG) HeapAlloc ( GetProcessHeap(), 0, MAX_NETBUFF );

	lpSndBuff		= (LPNETMSG) HeapAlloc ( GetProcessHeap(), 0, MAX_NETBUFF );



} // PlatformNetworkModule::InitVars
	



// -----------------------------------------------------------------
//
//  PlatformNetworkModule::IsModemGame
//
//	Description:
//		Are we a modem game?
//		
//	Arguments:
//		none
//
//	Return: TRUE if a mode game, else FALSE
//
// -----------------------------------------------------------------
BOOL PlatformNetworkModule::IsModemGame( void )
{

	return ( fModem );

} // PlatformNetworkModule::IsModemGame
	



// -----------------------------------------------------------------
//
//  PlatformNetworkModule::IsNetworkingAvailable
//
//	Description:
//		Is networking available?
//		
//	Arguments:
//		none
//
//	Return: ErrCode
//
// -----------------------------------------------------------------
ErrCode PlatformNetworkModule::IsNetworkingAvailable( void )
{

	if ( fSigs )
	{

		ErrCode ecError = kNoError;

		hDPlay = LoadLibrary ( "SNWValid.dll" );

		if ( hDPlay == NULL)	
		{
			ecError = kNetworkingNotInstalled;

			#ifdef _MULTIMONO
				mono_puts( 0, 1, "SNWValid.dll not found" );
			#endif
		}
		else
		{
			lpfnSNWValidate = (lpfnSNWValidateFunc)
							  GetProcAddress( (HINSTANCE) hDPlay, "SNWValidate" );

			if ( lpfnSNWValidate != NULL )
			{
				int bRet = 0;
				char szPath[MAX_PATH], szName[MAX_PATH];

				getcwd( szPath, MAX_PATH );
				if ( szPath[strlen(szPath)-1] != '\\' )
					strcat( szPath, "\\" );

               //---- Pass the application name for auto-update stuff
               //---- BR95.exe

				sprintf( szName, "%sWINBR.exe X", szPath );
			
				bRet = lpfnSNWValidate( szGameName, 
										GPServerIP, 
										&GPInPort, 
										&GPOutPort,
										szPath,
										szName,
										hwndApp );

               FreeLibrary( hDPlay );


				if ( bRet == DLL_EXIT_CONTINUE_TO_CONNECT )
				{
					ecError = kNoError;
				}
				else if ( bRet == DLL_EXIT_FAILED_TO_UPDATE )
				{
					ecError = kNetworkingSetupError;
				}
				else if ( bRet == DLL_EXIT_FOR_SELF_UPDATE )
				{
					ecError = kExitForCodeUpdate;
					QuitSys( 0,0 );
				}
				else
				{
					ecError = kGenericError;
				}

			}
			else
			{
				ecError = kNetworkingSetupError;
               FreeLibrary( hDPlay );

			}




		} // else if found dll


		// -- set the flag

		fNetworkAvailable = (kNoError == ecError) ? TRUE : FALSE;

		return ( ecError );


	}
	else
	{
		ErrCode ecError = kNoError;

		hDPlay = LoadLibrary ( "dplayx.dll" );

		if ( hDPlay == NULL)
		{
			ecError = kNetworkingNotInstalled;

			#ifdef _MULTIMONO
				mono_puts( 0,1, "DPlayx.dll not found" );
			#endif
		}
		else
		{
			 // --
			 // -- Get proc address for enumeration
			 // --

			fp_DPEnum = (FP_DPENUM) GetProcAddress( hDPlay, "DirectPlayEnumerateA");

			if (fp_DPEnum == NULL) 
       	{
				ecError = kNetworkingSetupError;
			}		


       	// --
   	    // -- Get the proc address for direct play creation
	       // --

			fp_DPCreate = (FP_DPCREATE) GetProcAddress( hDPlay, "DirectPlayCreate");

			if ( fp_DPCreate == NULL )
	       {
				ecError = kNetworkingSetupError;
			}

		} // else if found DPlay.dll


	 	// -- set the flag
		fNetworkAvailable = (kNoError == ecError) ? TRUE : FALSE;

		return ( ecError );
	}

} // PlatformNetworkModule::IsNetworkingAvailable
	
	


// -----------------------------------------------------------------
//
//  PlatformNetworkModule::JoinNetworkGame
//
//	Description:
//		Join the designated network game.
//		
//	Arguments:
//		pNetGameInfo	- network information
//
//	Return: ErrCode
//
// -----------------------------------------------------------------
ErrCode PlatformNetworkModule::JoinNetworkGame (long theGameIndex, char *playername)
{

	if ( fSigs )
	{
		return ( CreateNetworkGame( playername ) );
	}
	else
	{
		ErrCode ecError = kNoError;
		HRESULT hResult = DP_OK;

		ecError = IsNetworkingAvailable(); 

		if ( NULL == lpDP3A )
			ecError = kNoProtocolSelected;

		if ( TRUE == fGameInProgress )
			ecError = kCannotCreateGame;

		if ( (theGameIndex < 0) ||
					(theGameIndex >= usNumSessions) )
			ecError = kCannotCreateGame;

		if ( kNoError == ecError )
		{
			DPSESSIONDESC2 dpDesc2;
			char          szUserName[PLAYERNAME_LEN + 1];
			DWORD         dwUNSize = PLAYERNAME_LEN + 1;
	//		DPCAPS        dpCaps;

			memset ( szUserName, 0, PLAYERNAME_LEN + 1 );

	        //---- Setup the join description
			memset(&dpDesc2, 0x00, sizeof(DPSESSIONDESC2));
			dpDesc2.dwSize       = sizeof(dpDesc2);
			dpDesc2.dwFlags      = 0; // DPOPEN_OPENSESSION;

			dpDesc2.guidApplication = GAME_GUID1; // gGuid;
	
	        //---- Which game we are joining
			dpDesc2.guidInstance = guidSessions[ theGameIndex ];

//           sprintf( dpDesc.szPassword, "%3.2f", RealmAIVersionNumber );

			//----- Open the game session
			hResult= lpDP3A->Open( &dpDesc2, DPOPEN_JOIN );

			if ( hResult != DP_OK ) 
			{
				ecError = kCannotJoinGame;
 			}
			else 
			{
   	        //---- Setup the formal name
				GetUserName( szUserName, &dwUNSize );

				DPNAME dpName;
				BuildDPNameA( &dpName, szUserName, playername );


            //---- Create our player
				hResult = lpDP3A->CreatePlayer( &dwPlayerId, 
														  &dpName,
														  NULL, 
														  NULL, 
														  NULL, 
														  NULL );

				if ( hResult != DP_OK )	
           	{
					ErrorDPlay( hResult );

					hResult = lpDP3A->Close();

					if( hResult!=DP_OK )
						ErrorDPlay( hResult );
					
					ecError = kCannotCreatePlayer;
				}
				else 
           	{
					//----- Get info about the session we are joining 

					memset(&dpDesc2, 0x00, sizeof(DPSESSIONDESC2));
					dpDesc2.dwSize       = sizeof(dpDesc2);

					DWORD dwTemp = sizeof(DPSESSIONDESC2);
					
					lpDP3A->GetSessionDesc(	(LPVOID) &dpDesc2,
													&dwTemp );
	
	            //---- Save our session

					guidInstance = dpDesc2.guidInstance;

					// -- save game information
					
					if ( dpDesc2.lpszSessionNameA != NULL )
					{
						strncpy( ngiInfo.szGameName, 
									dpDesc2.lpszSessionNameA,
									sizeof ( ngiInfo.szGameName) );
					}

              	//---- Max players    * dlj

               ngiInfo.usMaxPlayers = ngiInfos[theGameIndex].usMaxPlayers;

		      	ngiInfo.dwUser1 = ngiInfos[theGameIndex].dwUser1;
					ngiInfo.dwUser2 = ngiInfos[theGameIndex].dwUser2;
					ngiInfo.dwUser3 = ngiInfos[theGameIndex].dwUser3;
					ngiInfo.dwUser4 = ngiInfos[theGameIndex].dwUser4;


	//				ResetGameInformation();


					fGameCreator = FALSE;

  	            strncpy(szPlayerName, playername, 256) ;

					//---- Get everyone in our game

					lpDP3A->EnumPlayers( NULL, EnumPlayersInit, this, 0 );
				}
			
			} // end if opened game OK

		} // if OK to join

		return ( ecError );

	}

} // PlatformNetworkModule::JoinNetworkGame


// -----------------------------------------------------------------
//
//  PlatformNetworkModule::PrintString
//
//  Print a string using the pPrintFunction passed to the constructor
//
//	 Arguments:
//		a string to be printed
//
//	Return: the return value of pPrintFunction
//
// -----------------------------------------------------------------

int PlatformNetworkModule::PrintString(char *s)
{

//   if (pPrintFunction == NULL)
//      return 0 ;
//   return (pPrintFunction)(s) ;


     #ifdef _MULTIMONO
		 if ( s )
	        mono_puts ( 40, 21, s );
     #endif

     return 0;

}



// -----------------------------------------------------------------
//
// PlatformNetworkModule::PeekNextMessage
//
// returns the id of the next message in the message que.
// removes extranious acks and checksum errors.
//
// Fills in the buffer lpGenericMsgBuf so that GetMessage will return the
// full message info.
//
// Calls ReceiveMessages which will process all qued system messages.
//
// ReceiveMessages will not ack the message when called by PeekNextMessage.
//
// returns TRUE on Success and FALSE on failure
//
// -----------------------------------------------------------------

int PlatformNetworkModule::PeekNextMessage( LPNETMSG pMsg )
{
	ErrCode	Err ;
	int		iWatchDog = 0;
	
	while (TRUE) 
	{
		//if we have been here way too long, return that
		//there were no more messages
		if(++iWatchDog > 100)
		{
			pMsg->usDataSize = 0;
			return FALSE;
		}
		
		Err = ReceiveMessages( 0, kPEEKONLY );
		
		if ( Err == kCheckSumError ) //---- Remove checksum error messages 
		{
			ReceiveMessages(0, kREMOVEFROMQUE );
			continue ;
		}
		else if ( kMessageNotReceived == Err )
		{
			// don't report this one
			pMsg->usDataSize = 0;
			return FALSE;
		}
		else if ( kNoError != Err ) //---- Error here 
		{
			// -- record this for later use
			SetErrorNum( Err );
			pMsg->usDataSize = 0;
			return FALSE;
		}
		else if ( lpGenericMsgBuf->MsgId == ACK ) //---- Remove ack messages
		{ 
			ReceiveMessages(0, kREMOVEFROMQUE );
			continue ;
		}
		else  //---- a presumably valid and interesting message
		{  
			break ;
		}
	}
	
	memcpy( pMsg, lpGenericMsgBuf, sizeof(NETMSG) );
	
	return TRUE ;

}  //---- End of PeekNextMessage()


// -----------------------------------------------------------------
//
//  PlatformNetworkModule::ReceiveMessages
//
//	Description:
//
//		Receive and process any pertinant messages.
//		
//	Arguments:
//		dwWait	   - how long to wait for the messages
//    bRemove     - bRemove = kPEEKONLY means peek at the first message in the
//                  que. Don't remove it and don't send ACK.
//
//                  bRemove = kREMOVEFROMQUE means get and remove first
//                  message from que, ACK if requested.
//
//    bAck        - to or ack or not to ack 
//
//	Return: ErrCode
//
// -----------------------------------------------------------------
ErrCode PlatformNetworkModule::ReceiveMessages (	DWORD dwWait, 
													const char bRemove, 
													const char bAck )
{

	ErrCode	ecError = kMessageNotReceived;

	HRESULT	hResult = DP_OK;
	int		iResult = 0;

	DWORD	dwExit = FALSE;   //---- exit read loop

	DPID	dpFromId  = 0;
	DPID	dpToId    = dwPlayerId;
	DWORD	dwBytes   = MAX_NETBUFF;
	DWORD	dwCount   = 0;
	DWORD	dwHeaderSize = sizeof(NETMSG);
	char	errstr[500] ;
	DWORD	dwFlag ;
	DWORD	Ticks ;
   static  BOOL fPeekedAlready = FALSE;	// cannot peek twice at the same message
	static	short	sPeekedPlayerId = 0;
	static	short	sPeekedSize = 0;
	static	char  szPeekBuff[MAX_NETBUFF];

	short   iPlayer = 0;
	int		iError = RETURN_ERR;
	int		iReadCount = 0;


	if ( fSigs )
	{
	
		// --
		// -- Make sure we can actually do something
		// --

		if ( (hDPlay == NULL) ||
			(lpfnSNWCheckGameConnectStatus == NULL) )
		{
			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
			char temp[150];
			sprintf ( temp, "PlatformNetworkModule::ReceiveMessages - No Game Started!");
			RandomLogComment ( temp );
			}

			return ( kNoGameStarted );
		}
		else if ( lpfnSNWCheckGameConnectStatus() != _GAME_STAT_SUCCESS )
		{
			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
			char temp[150];
			sprintf ( temp, "PlatformNetworkModule::ReceiveMessages - Connection Lost!");
			RandomLogComment ( temp );
			}

			return( kConnectionLost );
		}


		// --
		// -- Getting a message here
		// --

		dwWait += GetTickCount();

		while ( dwExit == FALSE ) 
		{

		    //---- service sound
			run_timers();

			iError = RETURN_ERR;
			iReadCount = 0;

	
			if ( bRemove == kPEEKONLY ) 
			{
			    // -- Only peeking here

				if ( !fPeekedAlready )
				{
					if ( lpfnSNWCheckForTCP() == RETURN_OK )
					{

						// -- Who has sent us a message?

						for ( iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++ )
						{

							//---- Is this a valid player 
	
							if ( dwPlayers[iPlayer] != kNoPlayer )
							{
								iError = lpfnSNWPeekForTCP( (unsigned char *)(&lpRecBuff[0]), 
															MAX_NETBUFF, dwPlayers[iPlayer] );
												
								if ( iError >= 0 )
								{
									fPeekedAlready = TRUE;
									sPeekedPlayerId = iPlayer;

									sPeekedSize = (short) iError;

									memcpy( &szPeekBuff[0], (char *)&lpRecBuff[0], MAX_NETBUFF );

									break;
								}

							}

						}  //---- For every player 


					} // any messages?

				} // end if not already peeked



				if ( fPeekedAlready )
				{
					// -- pretend like we just peeked this!

					iReadCount = sPeekedSize;

					memcpy( (char *)&lpRecBuff[0], &szPeekBuff[0], MAX_NETBUFF );

					iPlayer = sPeekedPlayerId;

				}

			}
			else if ( bRemove == kREMOVEFROMQUE )
			{
				if ( fPeekedAlready )
				{
					iPlayer = sPeekedPlayerId;
	
					// -- Received the previously peeked item

					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						char temp[150];
						sprintf ( temp, "PlatformNetworkModule::ReceiveMessages - Receiving previously peekded messaged!");
						RandomLogComment ( temp );
					}

					iReadCount = lpfnSNWRecvTCP( (unsigned char *)(&lpRecBuff[0]), 
												MAX_NETBUFF, dwPlayers[iPlayer] );
					fPeekedAlready = FALSE;


				}
				else if ( lpfnSNWCheckForTCP() == RETURN_OK )
				{
	
					// -- Who has sent us a message?

					for ( iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++ )
					{
						//---- Look at valid players only 
	
						if ( dwPlayers[iPlayer] != kNoPlayer )
						{
							iError = lpfnSNWPeekForTCP( (unsigned char *)(&lpRecBuff[0]), 
														MAX_NETBUFF, dwPlayers[iPlayer] );


							if ( iError >= 0 ) 
							{
								LOGCOMMENTSLEEP
								if ( fLogComment )
								{
									char temp[150];
									sprintf ( temp, "PlatformNetworkModule::ReceiveMessages - Receiving message!");
									RandomLogComment ( temp );
								}

								iReadCount = lpfnSNWRecvTCP( (unsigned char *)(&lpRecBuff[0]), 
															 MAX_NETBUFF, dwPlayers[iPlayer] );
								#ifdef _MULTIMONO
									mono_printf ( 40, 20, "TCPRecv message from id %lu",
					   		                      dwPlayers[iPlayer] );
								#endif

								break;
							}
						}
					}
				}

			} // end else get from the queue


			//---- We have received a message 

			if ( 0 != iReadCount )
			{
			    // -- Do the checkSum thing
				USHORT usCheckSum = lpRecBuff->usCheckSum;

				lpRecBuff->usCheckSum = 0;

				if (lpRecBuff->MsgId != iMPT_PING)
				{
					if ( usCheckSum != HashCRC( (PUCHAR) lpRecBuff, (USHORT) iReadCount ) ) 
					{
						#ifdef _MULTIMONO
						PrintString( "*-*-* CheckSum error in message *-*-*" );
						#endif
	
						LOGCOMMENTSLEEP
						if ( fLogComment )
						{
							char temp[150];
							sprintf ( temp, "PlatformNetworkModule::ReceiveMessages - *-*-* CheckSum error in message *-*-*");
							RandomLogComment ( temp );
						}

						
						ecError = kCheckSumError;
						goto receivemessagesendwhileSigs;
					}
				}


				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					if (lpRecBuff->MsgId == iMPT_UNIT)
					{
						typedef struct
						{
						   USHORT      usMsgType;	// what is it?
						   BOOL        fMaster;    // from master ? 
						   DWORD       dwFrom;     // from id ( 0  master ? )
						} BRHeader, *PBRHeader;

						#define SIZEOF_MULTIUNIT 14
						typedef struct
						{
							DWORD		UnitIndex;
							UBYTE		UnitData[SIZEOF_MULTIUNIT];
						}	BRUnitMessage, *PBRUnitMessage;

						char temp[200];
						PCHAR pMsg = (PCHAR)lpRecBuff->cData;
						pMsg += sizeof(BRHeader);
						UNIT_PTR pUnit = (UNIT_PTR) ( ((PBRUnitMessage) pMsg)->UnitData );

						sprintf ( temp, "PlatformNetworkModule::ReceiveMessage - received UNIT message. OurID:%ld sNumBytes:%ld dwBytes:%ld MsgNum:%lx",
								dwPlayerId,lpRecBuff->usDataSize,dwBytes,lpRecBuff->ulSync);
						RandomLogComment ( temp );
						sprintf ( temp, "                                        Unit index:%ld  province:%ld  Realm:%ld",
								((PBRUnitMessage) pMsg)->UnitIndex,
								pUnit->province,
								pUnit->Realm  );
						RandomLogComment ( temp );
					}
				}



				// -- copy it to our generic buffer and set the
				// --  flag alerting the game that it is here!
				memcpy( lpGenericMsgBuf, lpRecBuff, dwBytes);

				lpGenericMsgBuf->dwPlayerId = dwPlayers[iPlayer];

				// -- Don't be sending the ack per Brick
				ecError = kNoError;


	#ifdef ACKSIGS

				if ( (lpRecBuff->bNeedsAck) && 
					 (bRemove == kREMOVEFROMQUE) && 
					 (bAck == kACK)
				   )
   			{
					#ifdef _MULTIMONO
					wsprintf( errstr, "Acking message Id %d Sync %lu from %lu",
						lpRecBuff->MsgId, lpRecBuff->ulSync, dwPlayers[iPlayer] );
					#endif	

					SendMessage( dwPlayers[iPlayer], ACK,
						         NULL, 0, lpRecBuff->ulSync, 0, 0 ) ;

				}
				else if ( bRemove == kREMOVEFROMQUE ) 
				{
					#ifdef _MULTIMONO
					wsprintf( errstr, "Received message Id %d Sync %lu from %lu",
						lpRecBuff->MsgId, lpRecBuff->ulSync, dwPlayers[iPlayer] );
					#endif	
				}
               else
               {
					// -- clear mono message string
					errstr[0] = 0;

               }
	#endif


				#ifdef _MULTIMONO
				if ( strlen(errstr) != 0 )
				{
					PrintString(errstr) ;
				}
				#endif

      	 	    dwExit = TRUE; // got one message so quit

			 	// -- Yes, we have a message

			 	ecError = kNoError;

			}       // end if a message to receive
			else
			{
		    	ecError = kMessageNotReceived ;
			}



receivemessagesendwhileSigs:

			//---- Check to see if we expired

			if ( (dwExit == FALSE) && 
 				(dwWait < (Ticks = GetTickCount())) )
			{
				dwExit = TRUE;
			}


		}   //---- End of while( dwExit )


		return ( ecError );


	}


	//--------------------------------------------------------------
	//---  Direct Play section of code
	//--------------------------------------------------------------
	else
	{

		//---- Make sure we have a Direct play object and we have created a player

		if ( ( NULL == lpDP3A )     ||
		     ( kNoPlayer == dwPlayerId) )
		{
			return ( kNoGameStarted );
		}
		
		dwFlag = DPRECEIVE_ALL ;


		//---- Doing a peek only 

		if ( bRemove == kPEEKONLY ) 
		{
			dwFlag |= DPRECEIVE_PEEK;
		}

		dwWait += GetTickCount();    //---- Setup a wait

		while ( dwExit == FALSE )
		{
			// -- ask for the max

			dwBytes = MAX_NETBUFF;

			//---- service sound
			run_timers();

			hResult = lpDP3A->Receive( &dpFromId, &dpToId, 
								            dwFlag, lpRecBuff, &dwBytes );

			if ( hResult == DP_OK ) 
			{
				//----- System Messages

				if ( dpFromId == kSystemMessage )
				{
					LPDPMSG_GENERIC lpdpMsgGen = (LPDPMSG_GENERIC) lpRecBuff;

					//---- if system message go ahead and process it,
					//---- user may only control non system messages.

					if ( bRemove == kPEEKONLY ) 
					{
						hResult = ReceiveMessages (0, kREMOVEFROMQUE );
						
						if ( hResult == DP_OK )
						{
							// try another message
							continue ;
						}
						else 
						{
							return ecError ;
						}
					}

					switch ( lpdpMsgGen->dwType )
					{

						case DPSYS_CREATEPLAYERORGROUP:
						{
							LPDPMSG_CREATEPLAYERORGROUP lpdpMsgCreate;
							lpdpMsgCreate = (LPDPMSG_CREATEPLAYERORGROUP)lpRecBuff;

							// we don't care about groups
							if( lpdpMsgCreate->dwPlayerType==DPPLAYERTYPE_GROUP )
								break;

							AddPlayer( lpdpMsgCreate->dpId ); 

						}
						break;

						case DPSYS_DESTROYPLAYERORGROUP:
						{
							LPDPMSG_DESTROYPLAYERORGROUP lpdpMsgDestroy;
							lpdpMsgDestroy = (LPDPMSG_DESTROYPLAYERORGROUP)lpRecBuff;

							// we don't care about groups
							if( lpdpMsgDestroy->dwPlayerType==DPPLAYERTYPE_GROUP )
								break;


							// -- Delete the player from our local list
							DeletePlayer( lpdpMsgDestroy->dpId );
							break;
						}


						case DPSYS_SESSIONLOST:
							ecError = kConnectionLost;
							break;

						default:
							break;

					} // end switch msgType

				}
				else 
				{
					//---- It is not a system message; it is one of ours
					//---- Do the checkSum thing

					USHORT usCheckSum = lpRecBuff->usCheckSum;

					lpRecBuff->usCheckSum = 0;
			
					if ( usCheckSum != HashCRC( (PUCHAR) lpRecBuff, (USHORT) dwBytes ) )
					{
						#ifdef _MULTIMONO
						PrintString( "*-*-* CheckSum error in message *-*-*" );
						#endif

						ecError = kCheckSumError ;
						goto receivemessagesendwhile;
					}


					// -- don't save this message if we sent it
			
					if ( dpFromId == dwPlayerId ) 
					{
						if ( bRemove == kPEEKONLY ) 
						{
							ReceiveMessages ( 0, kREMOVEFROMQUE ) ;
						}
						goto receivemessagesendwhile;
					}


					// -- copy it to our generic buffer and set the
					// --  flag alerting the game that it is here!

				   memcpy( lpGenericMsgBuf, lpRecBuff, dwBytes);
	
				   lpGenericMsgBuf->dwPlayerId = dpFromId;

					//HACK JOB 
					//---- This is a special start game message
					
					if ( lpRecBuff->MsgId == NETMSGID_START ) 
					{
						if ( bRemove == kPEEKONLY ) 
						{
							ReceiveMessages ( 0, kREMOVEFROMQUE ) ;
							dwExit = TRUE; // got one message so quit
							goto receivemessagesendwhile;
						}
						
						dwExit = TRUE; // got one message so quit
						
						ngiInfo.usMaxPlayers = lpRecBuff->cData[0]; 
						
						goto receivemessagesendwhile;
	
					}
					
					// record that we recieved a SYNC message
					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						#if 0
						if (lpRecBuff->MsgId == iMPT_SYNC)
						{
							char temp[150];
							PBRHeader pBRHead = (PBRHeader) (&(lpGenericMsgBuf->cData[0]));
							PBRSync pSync = (PBRSync) (&(lpGenericMsgBuf->cData[0]) + sizeof(BRHeader));
	
							sprintf ( temp, "PlatformNetworkModule::ReceiveMessages - received sync %ld message from %ld (%ld).",
									pSync->iSync,
	   							lpGenericMsgBuf->dwPlayerId,         // who send the generic message
									pBRHead->dwFrom);
							RandomLogComment ( temp );
						}
						#endif
					}


					// -- if message needs an ack, we removed it and 
					// --  ack flag says we can ack
					
					if ( (lpRecBuff->bNeedsAck)
							&& (bRemove == kREMOVEFROMQUE ) 
							&& (bAck == kACK) )
					{
						
						#ifdef _MULTIMONO
						wsprintf( errstr, "Acking message Id %d Sync %lu from %lu",
							lpRecBuff->MsgId, lpRecBuff->ulSync, dpFromId );
						#endif	
						
						ErrCode bERR = SendMessage( dpFromId, ACK, 
							NULL, 0, lpRecBuff->ulSync, 0, 0 );
	
						// -- record that we recieved a SYNC message
						#if 0
						LOGCOMMENTSLEEP
						if ( fLogComment )
						{
							if (lpRecBuff->MsgId == iMPT_SYNC)
							{
								char temp[150];
								sprintf ( temp, "PlatformNetworkModule::ReceiveMessages - Ack'd sync message from %ld with %ld result (%ld=no error).",
			   					lpGenericMsgBuf->dwPlayerId, bERR, kNoError);
								RandomLogComment ( temp );
							}
						}
						#endif

					}
					else if ( bRemove == kREMOVEFROMQUE ) 
					{
						#ifdef _MULTIMONO
						wsprintf( errstr, "Received message Id %d Sync %lu from %lu",
							lpRecBuff->MsgId, lpRecBuff->ulSync, dpFromId );
						#endif	
					}
					else
					{
						// -- clear mono message string
						errstr[0] = 0;
					}

					#ifdef _MULTIMONO
					if ( strlen(errstr) != 0 )
					{
						PrintString(errstr) ;
					}
					#endif

					dwExit = TRUE; // -- got one message so quit

				} // -- else a game message

				ecError = kNoError;

			} // -- end if a message to receive

			else if ( hResult == DPERR_NOMESSAGES ) 
			{
				ErrorDPlay(hResult );
				ecError = kMessageNotReceived;
			}

receivemessagesendwhile:

			//---- Check to see if we expired

			if ( (dwExit == FALSE) && (dwWait < (Ticks = GetTickCount()) ) )
			{
				dwExit = TRUE;
			}

		}   //---- End of while( dwExit )

		return ( ecError );

	}

} // PlatformNetworkModule::ReceiveMessages


// -----------------------------------------------------------------
//
//  PlatformNetworkModule::ReleaseDPlay
//
//	Description:
//		Release the DirectPlay object.
//		
//	Arguments:
//		none
//
//	Return: ErrCode
//
// -----------------------------------------------------------------
ErrCode PlatformNetworkModule::ReleaseDPlay( void )
{
	int x;
	ErrCode ecError = kNoError;

	if ( fSigs )
	{


		if ( hDPlay != NULL )
		{

			lpfnSNWValidate				 = NULL;
			lpfnSNWCheckGameConnectStatus = NULL;
			lpfnSNWNumPlayers			 = NULL;
			lpfnSNWSendTCP				 = NULL;
			lpfnSNWSendTCPPoint			 = NULL;
			lpfnSNWRecvTCP	 			 = NULL;
			lpfnSNWWaitForTCP			 = NULL;
			lpfnSNWPeekForTCP			 = NULL;
			lpfnSNWCheckForTCP			 = NULL;
			lpfnSNWCheckPlayerConnectStatus = NULL;
			lpfnSNWGetPlayerXName = NULL;

			lpfnCloseDLLFunc lpfnSNWCloseDll = (lpfnCloseDLLFunc)
							GetProcAddress( (HINSTANCE) hDPlay, "CloseDLL" );
			if ( lpfnSNWCloseDll != 0 )
				lpfnSNWCloseDll();

			FreeLibrary( hDPlay );
			hDPlay = NULL;
		}

//       fSigs = FALSE;


	}
	else
	{

		if( lpDP3A != NULL )
		{
			if ( dwPlayerId != kNoPlayer )
				lpDP3A->DestroyPlayer( dwPlayerId );

			lpDP3A->Close();
			lpDP3A->Release();

			lpDP3A = NULL;
		}

	}

	if( lpDP != NULL )
	{
		// release the old direct play interface
		lpDP->Release();
		lpDP = NULL;
	}


	dwPlayerId = kNoPlayer;
	guidInstance = GUID_NULL;
	fGameInProgress = FALSE;

	// -- clear the game info structure

	memset ( &ngiInfo, 0, sizeof(ngiInfo) );

	for ( x=0; x < MAX_PLAYERS; x++ )
	{
	   dwPlayers[x] = kNoPlayer;
	   dwDeleted[x] = 0;
	   dwDeletedId[x] = kNoPlayer;
	}


	ngiInfo.usMaxPlayers = 2;    //---- Default to 2 players


	return ( ecError );


} // PlatformNetworkModule::ReleaseDPlay




// -----------------------------------------------------------------
//
//  PlatformNetworkModule::SelectGuid
//
//	Description:
//		Select a Guid for this object.
//		
//	Arguments:
//		i		- GUID to use
//
//	Return: none
//
// -----------------------------------------------------------------
void PlatformNetworkModule::SelectGuid( int i )
{
	switch ( i )
	{
	case 0:
		gGuid = GAME_GUID1;
		break;

	case 1:
		gGuid = GAME_GUID2;
		break;

	case 2:
	default:
		gGuid = GAME_GUID3;
		break;
	}
} // PlatformNetworkModule::SelectGuid




// -----------------------------------------------------------------
//
//  PlatformNetworkModule::ResetAcks
//
//	Description:
//		Resets ack table. This is the if the player has acked my 
//     message table
//		
//	Arguments:
//
//	Return: none
//
// -----------------------------------------------------------------
void PlatformNetworkModule::ResetAcks( void )
{
	int x;

	//---- Clear out ack array for a broadcast 

             	
	for ( x = 0; x < MAX_PLAYERS; x++ )
	{
		dwAcks[x] = 0;  //---- haven't received an ack
	}            


}	//---- End of ResetAcks()


// -----------------------------------------------------------------
//
//  PlatformNetworkModule::CheckOneAck
//
//	Description:
//		check a player's ack status
//		
//	Arguments:
//
//	Return: TRUE - if player has ACKed, FALSE - no ACK
//
// -----------------------------------------------------------------
//BOOL PlatformNetworkModule::CheckOneAck( DWORD dwPlayerId )
BOOL PlatformNetworkModule::CheckOneAck( DWORD ArrayPosition )
{
	//int x;
	//
	//for ( x=0; x < MAX_PLAYERS; x++ )
	//{
	//	//---- if we have this player have they acked yet ?
	//	if ( dwPlayers[x] == dwPlayerId )
	//		return dwAcks[x];
	//}
	//
	//return FALSE;

	return !!dwAcks[ArrayPosition];

}


// -----------------------------------------------------------------
//
//  PlatformNetworkModule::UpdateandCheckAcks
//
//	Description:
//		Update ack table and check if every has acked.
//		
//	Arguments:
//
//	Return: TRUE - all have acked, FALSE - missing acks 
//
// -----------------------------------------------------------------
BOOL PlatformNetworkModule::UpdateandCheckAcks( DWORD dwPlayerId )
{
	int x; 
	BOOL fOk = TRUE;   //---- All have acked 


	for ( x=0; x < MAX_PLAYERS; x++ )
	{

		//---- find this player and update the ack status 

		if ( dwPlayerId == dwPlayers[x] ) 
		{
  			dwAcks[x] = 1;
		}

	} 


	// check if everyone has acked yet 
   
	for ( x=0; x < MAX_PLAYERS; x++ )
	{
		//---- if we have this player have they acked yet ?

		if ( dwPlayers[x] != kNoPlayer &&
   	 	!dwAcks[x]                   )
		{
   		fOk = FALSE;
		}

	}



	return fOk;



}	//---- End of UpdateandCheckAcks()



// -----------------------------------------------------------------
//
//  PlatformNetworkModule::SendMessage
//
//	Description:
//		Send the designated message.
//		
//	Arguments:
//	   dpToId	   - receiver of the message
//	   bMsgId	   - message id (type of message)
//	   lpMessage   - message text
//	   sNumBytes   - size of the message text.
//    usMsgNum    - unique identifier ie turn number, packet sequence number.
//    dwWait      - time to wait for ACK on each attempt to send,
//                  0 means don't wait.
//
//    bRetryCount - number of attempts to send.
// 
//
//	Return: errcode
//
// -----------------------------------------------------------------
ErrCode PlatformNetworkModule::SendMessage (	DWORD  dpToId,
															BYTE   bMsgId,
															LPVOID lpMessage,
															short  sNumBytes,
															ULONG  ulMsgNum,
															DWORD  dwWait,
															BYTE   bRetryCount
															)
{


	ErrCode ecError   = kMessageNotReceived ;
	HRESULT hResult   ;
	int	    iResult	  = 0;
	DWORD   dwLenMsg  = sizeof (NETMSG);
	DWORD   dwTimeOut ;
	DWORD   dwFlag ;
	char    errstr[256] ;

	DPID     dpFromId2 = dpToId;
	DPID     dpToId2   = dwPlayerId;
	DWORD    dwBytes   = MAX_NETBUFF;

	ErrCode  err;

	if ( fSigs )
	{
		// -- Are we OK to send?
		if ( (hDPlay == NULL) ||
			 (lpfnSNWCheckGameConnectStatus == NULL) )
		{
			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
				char temp[200];
				sprintf ( temp, "PlatformNetworkModule::SendMessage - SIGS connect status FAILED!!!!!!!!!!!!!");
				RandomLogComment ( temp );
			}

			return ( kNoGameStarted );
		}

		//---- Check our send buffer
		if (lpSndBuff == NULL) 
		{
			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
				char temp[200];
				sprintf ( temp, "PlatformNetworkModule::SendMessage - SendMessage lpSndBuff == NULL!!!!!!!!!!!!!!!!!!!!");
				RandomLogComment ( temp );
			}

			return ( kGenericError );
		}

		//---- Setup message header
		lpSndBuff->MsgId      = bMsgId;
		lpSndBuff->ulSync     = ulMsgNum;
		lpSndBuff->usCheckSum = 0;
		lpSndBuff->usDataSize = sNumBytes;
		lpSndBuff->bNeedsAck  = (BYTE) !!dwWait ; // turn wait time into a BOOL
		lpSndBuff->dwPlayerId = dwPlayerId;

		if ( dwLenMsg > MAX_NETBUFF ) 
		{
			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
				char temp[200];
				sprintf ( temp, "PlatformNetworkModule::SendMessage - SendMessage dwLenMsg(%ld) > MAX_NETBUFF!!!!!!!!!!!!!!!!!!!", dwLenMsg);
				RandomLogComment ( temp );
			}

			return( kMessageBufferOverflow );
		}

		if (sNumBytes > 0) 
		{
			memcpy ( lpSndBuff->cData, lpMessage, sNumBytes );
			dwLenMsg += sNumBytes;
		}

		// -- compute and save the checksum
		lpSndBuff->usCheckSum = HashCRC( (PUCHAR) lpSndBuff, (USHORT) dwLenMsg );

		for (BYTE lcv = 0 ; lcv <= bRetryCount ; lcv++) 
		{
			dwTimeOut = dwWait + GetTickCount();

			//---- service sound
			run_timers();

			//---- Send the message out
 			if ( kBroadcast == dpToId )
 			{

				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					if (bMsgId == iMPT_UNIT)
					{
						typedef struct
						{
						   USHORT      usMsgType;	// what is it?
						   BOOL        fMaster;    // from master ? 
						   DWORD       dwFrom;     // from id ( 0  master ? )
						} BRHeader, *PBRHeader;

						#define SIZEOF_MULTIUNIT 14
						typedef struct
						{
							DWORD		UnitIndex;
							UBYTE		UnitData[SIZEOF_MULTIUNIT];
						}	BRUnitMessage, *PBRUnitMessage;

						char temp[200];
						PCHAR pMsg = (PCHAR)lpMessage;
						pMsg += sizeof(BRHeader);
						UNIT_PTR pUnit = (UNIT_PTR) ( ((PBRUnitMessage) pMsg)->UnitData );

						sprintf ( temp, "PlatformNetworkModule::SendMessage - broadcasting UNIT message. OurID:%ld sNumBytes:%d dwLenMsg:%ld MsgNum:%lx",dwPlayerId,sNumBytes,dwLenMsg,ulMsgNum);
						RandomLogComment ( temp );
						sprintf ( temp, "                                     Unit index:%ld  province:%d  Realm:%d",
								((PBRUnitMessage) pMsg)->UnitIndex,
								pUnit->province,
								pUnit->Realm  );
						RandomLogComment ( temp );
					}
				}

				iResult = lpfnSNWSendTCP( (unsigned char *)&lpSndBuff[0], 
									(HEADER_DATATYPE) dwLenMsg );
 			}
			else
			{
				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					if (bMsgId == iMPT_UNIT)
					{
						char temp[200];
						sprintf ( temp, "PlatformNetworkModule::SendMessage - SendMessage PointCast UNIT message!!!!!!!!!!!!!!!!!!!");
						RandomLogComment ( temp );
					}
				}

				iResult = lpfnSNWSendTCPPoint( dpToId, (unsigned char *)&lpSndBuff[0], 
									(HEADER_DATATYPE) dwLenMsg );
			}

			if ( iResult == -1 )
			{
				continue ;
			}

			if ( dwWait == 0 )
			{
				ecError = kNoError ;
				goto outSigs ;
			}

	#ifndef ACKSIGS
			// -- skip the ack per Brick
			ecError = kNoError ;
			goto outSigs ;

	#else
			//---- Reset ack table
			if ( dpToId == kBroadcast )
			{
				ResetAcks();
			}

			// -- Do ACK receiving loop
			do
			{
				dwBytes = MAX_NETBUFF;

				//---- service sound
				run_timers();

				err = ReceiveMessages( dwWait, kREMOVEFROMQUE, kDONTACK );

				if ( (err != kNoError) && 
					(err != kMessageNotReceived) )
				{
					#ifdef _MULTIMONO
					wsprintf( errstr, "Failed to receive ACK" );
						PrintString(errstr) ;
					#endif

					break ;
				}

				// -- If nothing, try again!

				if ( err == kNoError )
				{
					if ( (lpRecBuff->MsgId  == NETMSGID_ACK) &&
							 (lpRecBuff->ulSync == ulMsgNum)        )
					{
						#ifdef _MULTIMONO
						mono_printf ( 0, 22, "Received ACK for message Id %d Sync %lu from %d retry %d",
							bMsgId, 
							lpRecBuff->ulSync, 
							lpGenericMsgBuf->dwPlayerId,  //--- this is right
							lcv );
						#endif
	
						//----- Broadcast message so we need to get multiple acks
						if ( dpToId == kBroadcast )
						{
							//---- All players have acked
							
							if ( UpdateandCheckAcks( lpGenericMsgBuf->dwPlayerId ) )
							{
								ecError = kNoError;
								goto outSigs;
							}
						}
						else
						{
							ecError = kNoError;
							goto outSigs;
						}
	
					}
					else if ( lpRecBuff->MsgId  == NETMSGID_ACK )
					{
						#ifdef _MULTIMONO
						mono_printf( 0,23,
							"Received undesired ACK for Id %d Sync %lu from %i retry %d",
							bMsgId,
							lpRecBuff->ulSync, 
							lpGenericMsgBuf->dwPlayerId,
							lcv 
							);
						
						#endif
						
					}
	
				} 

				#ifdef _MULTIMONO
				PrintString( "from SendMessage No Ack Yet" ) ;
				#endif
	
			} while ( dwTimeOut > GetTickCount() ) ;
#endif
		} // for bRetryCount

//----- Don't do the connnection status thing here ( maybe thrashing ?? )
#if 0
#ifdef ACKSIGS
		//---- We haven't got all the acks
		if ( dpToId == kBroadcast )
		{
			int x; 
			int iOk = 1;   //---- All have acked 

			//---- Check if a player disconnected
			for ( x=0; x < MAX_PLAYERS; x++ )
			{
				//---- If we have player and they didn't ack and not here anymore 
				//---- Need to delete the player

				if ( dwPlayers[x] != kNoPlayer  &&
					 !dwAcks[x]                 &&
				     lpfnSNWCheckPlayerConnectStatus(dwPlayers[x]) == RETURN_ERR ) 
				{
					DeletePlayer(dwPlayers[x]);  //---- Delete the player
				}
			} 
		}
		else
		{
			int x;

			//---- Check the players connection status 

			for ( x = 0; x < MAX_PLAYERS; x++ )
			{
				if ( dpToId == dwPlayers[x] )
				{
				 	if ( lpfnSNWCheckPlayerConnectStatus(dwPlayers[x]) == RETURN_ERR )
				 	{
						DeletePlayer(dwPlayers[x]);  //---- Delete the player

						ecError = kPlayerDisconnected;
						goto outSigs;

				 	}
				}
			}
		}
#endif
#endif

outSigs:
		return ( ecError );
	}

	//-------------------------------------------------------
	//---- If it is Direct Play
	//-------------------------------------------------------
	else
	{
		//---- Make sure we have a Direct play object
		if ( (NULL == lpDP3A) || (kNoPlayer == dwPlayerId) )
			return ( kNoGameStarted );

   	//---- Check our send buffer
		if (lpSndBuff == NULL)
			return ( kGenericError );

		//---- Setup message header
		lpSndBuff->MsgId		= bMsgId;
		lpSndBuff->ulSync		= ulMsgNum;
		lpSndBuff->usCheckSum	= 0;
		lpSndBuff->usDataSize	= sNumBytes;
		lpSndBuff->bNeedsAck	= (BYTE) !!dwWait;	// turn wait time into a BOOL
		lpSndBuff->dwPlayerId   = dwPlayerId;

		if ( dwLenMsg > MAX_NETBUFF )
			return kMessageBufferOverflow;

		if ( sNumBytes > 0 )
		{
			memcpy ( lpSndBuff->cData, lpMessage, sNumBytes );
			dwLenMsg += sNumBytes;
		}

		// -- compute and save the checksum
		lpSndBuff->usCheckSum = HashCRC( (PUCHAR) lpSndBuff, (USHORT) dwLenMsg );

		for (BYTE lcv = 0 ; lcv <= bRetryCount ; lcv++)
		{
			dwTimeOut = dwWait + GetTickCount();

			//---- service sound
			run_timers();

			//---- Send the message out
			if ( bMsgId == ACK )
				dwFlag = 0; // DPSEND_TRYONCE | DPSEND_HIGHPRIORITY;
			else
				dwFlag = 0; // DPSEND_TRYONCE;

			//---- Send the message 
			if ( dpToId == kBroadcast )
			{
				#if 0
				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					if (bMsgId == iMPT_SYNC)
					{
						char temp[200];
						sprintf ( temp, "PlatformNetworkModule::SendMessage - broadcasting sync message. OurID:%ld  MsgNum:%lx",dwPlayerId,ulMsgNum);
						RandomLogComment ( temp );
					}
				}
				#endif

				hResult = lpDP3A->Send( dwPlayerId, 0,
					dwFlag, lpSndBuff, dwLenMsg );
			}
			else
			{
				#if 0
				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					if (bMsgId == iMPT_SYNC)
					{
						char temp[200];
						sprintf ( temp, "PlatformNetworkModule::SendMessage - pointcasting sync message. OurID:%ld  MsgNum:%lx",dwPlayerId,ulMsgNum);
						RandomLogComment ( temp );
					}
				}
				#endif

				hResult = lpDP3A->Send( dwPlayerId, 
												dpToId, 
												dwFlag, 
												lpSndBuff, 
												dwLenMsg );
			}

			#ifdef _MULTIMONO
			if ( hResult )
			{
				mono_printf ( 40, 15, "Sev %d Fac %d Res %d",
				  HRESULT_SEVERITY(hResult),
				  HRESULT_FACILITY(hResult),
				  HRESULT_CODE(hResult) );
			}
			#endif

			//---- We get this message if no one is connected yet
			//---- and we try to broadcast
			if ( hResult == DPERR_INVALIDPLAYER )
			{
				ErrorDPlay (hResult );
				ecError = kPlayerDisconnected;
				goto out;
			}

			//---- May need to filter more errors here 
			//---- NOTE SEND WILL RETURN # MESSAGES QUEUED NEED TO 
			//---- MAKE SURE IT IS AN ERROR BEFORE WE RETRY
			if ( HRESULT_FACILITY(hResult) == _FACDP
				 && hResult != DP_OK )					//---- Direct Play Error 
			{
				ErrorDPlay (hResult );

				#ifdef _MULTIMONO
				wsprintf(errstr,"%s",szLastErrorMessage);
				PrintString(errstr);
				#endif

				continue;
			}

			//---- Jumps over ACK stuff ( no wait says no ack needed )
			if ( dwWait == 0 )
			{
				ecError = kNoError;
				goto out;
			}

			//---- Clear out ack array for a broadcast 
			if ( dpToId == kBroadcast )
				ResetAcks();

			do
			{
				dwBytes = MAX_NETBUFF;

				run_timers();	//---- service sound

				err = ReceiveMessages( dwWait, kREMOVEFROMQUE, kNOACK );

				if ( err != kNoError
					&& err != kMessageNotReceived)
				{
					ErrorDPlay(hResult);

					#ifdef _MULTIMONO
					wsprintf( errstr, "Failed to receive ACK error is %s",szLastErrorMessage );
					PrintString(errstr) ;
					#endif

					break;
				}

				// don't accept as an ACK any error message
				if ( err == kNoError )
				{
					if ( lpRecBuff->MsgId  == NETMSGID_ACK
							&& lpRecBuff->ulSync == ulMsgNum )
					{
						#ifdef _MULTIMONO
						mono_printf ( 0, 22, "Received ACK for message Id %d Sync %lu from %d retry %d",
							bMsgId,
							lpRecBuff->ulSync,
							lpGenericMsgBuf->dwPlayerId,  //--- this is right
							lcv );
						#endif

						//----- Broadcast message so we need to get multiple acks
						if ( dpToId == kBroadcast )
						{
							#if 0
							LOGCOMMENTSLEEP
							if ( fLogComment )
							{
								if (bMsgId == iMPT_SYNC)
								{
									char temp[200];
									sprintf ( temp, "PlatformNetworkModule::SendMessage - broadcast: received ACK from %ld  MsgNum:%lx", lpGenericMsgBuf->dwPlayerId, lpRecBuff->ulSync);
									RandomLogComment ( temp );
								}
							}
							#endif

							//---- Update and check if all ack have come in
							if ( UpdateandCheckAcks( lpGenericMsgBuf->dwPlayerId ) )
							{
								#if 0
								LOGCOMMENTSLEEP
								if ( fLogComment )
								{
									if (bMsgId == iMPT_SYNC)
									{
										char temp[200];
										sprintf ( temp, "PlatformNetworkModule::SendMessage - received all sync ACKs");
										RandomLogComment ( temp );
									}
								}
								#endif

								ecError = kNoError;
								goto out;
							}
						}

						else		//	no a broadcast, one ACK is enough
						{
							#if 0
							LOGCOMMENTSLEEP
							if ( fLogComment )
							{
								if (bMsgId == iMPT_SYNC)
								{
									char temp[200];
									sprintf ( temp, "PlatformNetworkModule::SendMessage - pointcast: received ACK from %ld  MsgNum:%lx", lpGenericMsgBuf->dwPlayerId, lpRecBuff->ulSync);
									RandomLogComment ( temp );
								}
							}
							#endif

							ecError = kNoError;
							goto out;
						}
					}

					else if ( lpRecBuff->MsgId == NETMSGID_ACK )
					{
						#ifdef _MULTIMONO
						mono_printf( 0,23,
							"Received undesired ACK for Id %d Sync %lu from %i retry %d",
							bMsgId,
							lpRecBuff->ulSync,
							lpGenericMsgBuf->dwPlayerId,
							lcv );
						#endif
					}

				}		//--- if err == kNoError

				#ifdef _MULTIMONO
					PrintString( "from SendMessage No Ack Yet" ) ;
				#endif

			} while ( dwTimeOut > GetTickCount() );

		}	//---- End of For ( retries )
		
out:
		return ( ecError );

	}		//---- End of If it is Direct Play
}			// PlatformNetworkModule::SendMessage


// -----------------------------------------------------------------
//
//  PlatformNetworkModule::SetNetworkProtocol
//
//	Description:
//		Set the desired network protocol.
//		
//	Arguments:
//		theProtocolIndex	- index selected
//
//	Return: none
//
// -----------------------------------------------------------------
ErrCode PlatformNetworkModule::SetNetworkProtocol ( long theProtocolIndex )
{

	if ( fSigs )
	{
		return ( kGenericError );
	}
	else
	{
		ErrCode ecError = kNoError;
		HRESULT hResult = DP_OK;

		if ( (usNumProviders > 0) &&
			 (theProtocolIndex < usNumProviders) )
		{
			// -- Check for a previous game
			if ( lpDP3A != NULL )
				ecError = ReleaseDPlay();

			// -- Create our direct play object
			hResult = (fp_DPCreate )( &guidProviders[theProtocolIndex], &lpDP, NULL );

			if( hResult == DP_OK )
			{
				hResult = lpDP->QueryInterface( IID_IDirectPlay3A, (void **)&lpDP3A );
			}


			if ( hResult != DP_OK )
			{
				ReleaseDPlay();
				ecError = kNetworkingSetupError;

			}
			else
			{
				 // -- set the modem flag accordingly
				if ( (guidProviders[theProtocolIndex] == DPSPGUID_MODEM) ||
					  (guidProviders[theProtocolIndex] == DPSPGUID_SERIAL) )
				{
					fModem = TRUE;
				}
				else
				{
					fModem = FALSE;
				}

			}

		}
		else
   	{
	    	ecError = kNoNetworkProtocolsAreAvailable;
	    }

		return ( ecError );

	}

} // PlatformNetworkModule::SetNetworkProtocol




// -----------------------------------------------------------------
//
// PlatformNetworkModule::WaitForPlayer
//
//	Description: Wait for a single player to join
//		
//		
//	Arguments: dwait - time to wait 
//		
//
//	Return: none
//
// -----------------------------------------------------------------

BOOL PlatformNetworkModule::WaitForPlayer(DWORD dwWait)
{
   int lcv, initialplayercount, finalplayercount ;


	for ( lcv = 0; lcv < MAX_PLAYERS; lcv++ )
	{
		if (dwPlayers[lcv] == kNoPlayer)
		{
			break;
		}
	}

   initialplayercount = lcv ;

   dwWait += GetTickCount() ;

   while ( (dwWait > GetTickCount()) && (dwPlayers[initialplayercount] == kNoPlayer) ) 
   {
	   //---- service sound
	   run_timers();

      ReceiveMessages(0,kPEEKONLY) ;
   }


   for ( lcv = 0; lcv < MAX_PLAYERS; lcv++ )
	{
		if (dwPlayers[lcv] == kNoPlayer)
		{
			break;
		}
	}

   finalplayercount = lcv ;

   return (initialplayercount < finalplayercount) ;


}  //---- End of PlatformNetworkModule::WaitForPlayer() 




//---------------------------------------------------------------------------
//
//  PlatformNetworkModule::WaitForPlayers()  
//
//  Description: peek for add players and return number that has joined 
//
//  Parameters: 
//
//  Returns:    kHaveAll or number of player in game 
//
//---------------------------------------------------------------------------

int PlatformNetworkModule::WaitForPlayers( void )
{
   int    y;
   USHORT usNumPlayers = 0;


	ReceiveMessages( 0, kPEEKONLY, FALSE );

	usNumPlayers = 0;

	//---- Count up the players in the game 
		
   for ( y = 0; y < MAX_PLAYERS; y++ )
   {
		if ( dwPlayers[y] != kNoPlayer )
       {

			usNumPlayers++;

			#ifdef _MULTIMONO
	           	mono_printf ( 0, 24, "Waiting for %d players have %d",
      	                      ngiInfo.usMaxPlayers,
        	                  usNumPlayers+1 );
			#endif
       }

   }


   if ( ( usNumPlayers+1 ) >= ngiInfo.usMaxPlayers )
   {
           return kHaveAll;
   }

   

   return usNumPlayers+1;


}  //---- End of PlatformNetworkModule::WaitForPlayers




//---------------------------------------------------------------------------
//
//  PlatformNetworkModule::ForceStart()  
//
//  Description: Force game start Master can override number of players 
//               at start of game 
//
//  Parameters: 
//
//  Returns: 
//                               HACK JOB 
//
//---------------------------------------------------------------------------

int PlatformNetworkModule::ForceStart( void )
{
   int    y;
   USHORT usNumPlayers = 0;
	HRESULT hResult   ;
	int	    iResult	  = 0;
	DWORD   dwLenMsg  = sizeof (NETMSG);
	DWORD   dwTimeOut ;
	DWORD   dwFlag ;
	char    errstr[256] ;

	//---- Count up the players in the game 
		
   for ( y = 0; y < MAX_PLAYERS; y++ )
   {
		if ( dwPlayers[y] != kNoPlayer )
       {
			usNumPlayers++;
       }
   }


   //---- Reset the max players 

   ngiInfo.usMaxPlayers = usNumPlayers+1;


   //---- Make sure we have a Direct play object

	if ( (NULL == lpDP3A) || (kNoPlayer == dwPlayerId) ) 
   {
		return ( 0 );
	}


	{
		DPSESSIONDESC2 dpDesc2;
		DWORD          dwSize;

		dwSize = sizeof ( DPSESSIONDESC2 );

		lpDP3A->GetSessionDesc( (LPVOID) &dpDesc2,
										&dwSize );

		dpDesc2.dwFlags = DPSESSION_JOINDISABLED;

		lpDP3A->SetSessionDesc( &dpDesc2,
										0 );
	}

   //---- Check our send buffer

	if (lpSndBuff == NULL) 
   {
		return ( 0 );
	}


	//---- Setup message header

	lpSndBuff->MsgId	  = NETMSGID_START;
	lpSndBuff->ulSync	  = 0;
	lpSndBuff->usCheckSum = 0;
	lpSndBuff->usDataSize = 1;
   lpSndBuff->bNeedsAck  = 0; // turn wait time into a BOOL


   lpSndBuff->cData[0] = usNumPlayers+1;
  	dwLenMsg += 1;

	// -- compute and save the checksum

	lpSndBuff->usCheckSum = HashCRC( (PUCHAR) lpSndBuff, (USHORT) dwLenMsg );


	dwFlag = 0; 


   //---- Broadcast message 

   hResult = lpDP3A->Send( dwPlayerId, 0,
									dwFlag, lpSndBuff, dwLenMsg );

   return 1;

}


//---------------------------------------------------------------------------
//
//  PlatformNetworkModule::DisablePlayers()  
//
//  Description: Disable players from joining this game 
//
//  Parameters: 
//
//  Returns: 
//                           
//
//---------------------------------------------------------------------------

void PlatformNetworkModule::DisablePlayers( void )
{

	if ( fSigs )
	{
		return;
	}

	if ( (NULL == lpDP3A) || (kNoPlayer == dwPlayerId) ) 
   {
		return;
	}

	if ( fGameCreator == TRUE )
	{
		DPSESSIONDESC2 dpDesc2;
		DWORD          dwSize;

		dwSize = sizeof ( DPSESSIONDESC2 );

		lpDP3A->GetSessionDesc( (LPVOID) &dpDesc2,
										&dwSize );

		dpDesc2.dwFlags = DPSESSION_JOINDISABLED;

		lpDP3A->SetSessionDesc( &dpDesc2,
										0 );

	}
	
}

/* ========================================================================
   Function    - SetErrorNum
	Description - Record low level errors for later retrievel
	Returns     - 
	======================================================================== */
void SetErrorNum ( PNET_ERRCODE_TYPE ErrorVal )
{
	// -- Save this for later retrieval by higher level routines (silly ain't it)
	gErrorVal = ErrorVal;
}


/* ========================================================================
   Function    - GetErrorNum
	Description - retrieve the global error code
	              NOTE: you only get the last value set, not a stack
	Returns     - 
	======================================================================== */
PNET_ERRCODE_TYPE GetErrorNum ( void )
{
	return gErrorVal;
}


/* ========================================================================
   Function    - ClearErrorNum
	Description - reset the global error code
	Returns     - 
	======================================================================== */
PNET_ERRCODE_TYPE ClearErrorNum ( void )
{
	PNET_ERRCODE_TYPE retVal = gErrorVal;
	gErrorVal = kNoError;
	return retVal;
}

//---- End of PNetMod.cpp
