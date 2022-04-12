// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//
//	 MulPlay.cpp
//		MultiPlayer class
//	 
//
//	Who	When			Why
// ---	---------	-----------------------------------------------
// CAM             Fps 97
// dlj 9-12-1996   Make it a Birthright piece of code
// dlj 9-25-1996   make it multiplayer...  
// MDB  4 Oct 96   Stripped out football specific code
//	CAM	1 OCT 96		Added Net/Modem & SIGS selection screen
// DLJ 22 Oct 96   borrowed sigs from fb pro 97 
// dlj 3 Nov 96    finalize our own dialogs version ( not win specific )
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*


#ifdef _DEBUG
#define _MULTIMONO    //---- Comment out for no mono
#endif

#define LOGCOMMENTSLEEP {;}

#include "MulPlay.hxx"

#include "FileXfer.hxx"

#include "StdIO.h"
#include "Limits.h"
#include "Direct.h"
#include "../panel.h"
#include "mono_c.h"
#include "messque.hxx"


#include "../system.h"
#include "../engine.h"

#include "../machine.h"
#include "../machint.h"
#include "../realm.hxx"
#include "../places.hxx"

#include "../charsel.hxx"
#include "../main.hxx"
#include "../menu.h"
#include "../multimap.hxx"
#include "../game.h"
#include "../loadsave.hxx"
#include "../units.hxx"
#include "../gamemap.hxx"
#include "../sound.hxx"
#include "../sndvox.hxx"
#include "../soundfx.hxx"

#include "ddrawpro.h"



// -- Default global object

MultiPlayer AMultiPlayer;


//---- Extern data  

extern LONG lGameSemiphore[MPGAMESEMIPHORE_MAX_DATA];

extern BOOL gMPStartGame;	// from multiui.cpp

extern BOOL  fRedrawAll;

extern BOOL	fWonGame;

extern "C" LONG numrands;

extern "C" unsigned long lastrand;


extern DIFFICULTY game_difficulty;
extern DIFFICULTY battle_difficulty;
extern SPEED      battle_speed;
extern LONG			fControlMode;

extern long SpeedLevel;
extern long DifficultyLevel;
extern SHORT gWinningPoints;

extern LONG l_map;

extern int iTTimeLimit;

//---- Extern proto

extern void GainRealmExp(LONG RegentIndex, const LONG ExpAmount,
				const BOOL CanGainLevelNow, const BOOL fSend);
extern void MultiResetRealmSelect(void);
extern void DomainNewGame (LONG, LONG);
extern void PlayFinale(LONG,LONG);
extern void ToggleDirectDraw(LONG, LONG);

extern void StartSync ( LONG sync );
extern LONG CheckSync ( LONG sync, BOOL fStay, BOOL fDisplayResult );
#define WaitSync(a)	StartSync(a); do { ; } while (CheckSync((a), TRUE, FALSE));

extern BOOL MultiAllRealmsSelected( void );
extern void ShowMPOver(LONG, LONG);


extern float RealmAIVersionNumber;

extern "C" void SetRedrawMainMapLevel (void);

extern "C" void RandomLogComment ( char * szString );
#include "dbstring.cpp"

#ifdef _WINDOWS
extern "C" HWND hwndApp;
#endif


BOOL fStartPinging = FALSE;

void HandleNetworkErrors ( PNET_ERRCODE_TYPE ErrorVal );

enum
{
	iMPM_SEND,
	iMPM_RCV,
	iMPM_CREATE,
	iMPM_RCVFILE,
	iMPM_SENDFILE,
};





//---- Global / static data 


static  char  szAppName[]= "Birthright";


char pszCfg[] = "MP.cfg";


static void		DisplayString	( long lLBId, char* theString, 
									  long theIndex, PNETGAMEINFO pnm );

static void		SaveString	 	( long lLBId, char* theString, 
									  long theIndex, PNETGAMEINFO pnm );



		void		FileProgress	( int iBlock, int iMaxBlock );


       BOOL		fStartGame = FALSE;   //---- Startin a game 

static	BOOL		fSigs      = FALSE;   //---- Using sigs protocol

static	HWND		hwndWait   = NULL;

static	NETMSG	nm;					// junk area for message header


static int			iGameType = iMPGT_NONE;


extern LONG iGameData;


static BOOL fHaveSeedTable = FALSE;


//---- saved protocol name list

const int MAX_PROTOCOL = 10;

static CHAR pszProtocol[MAX_PROTOCOL][81];	// list of protocols 

static CHAR pszGames[MAX_GAMES][24];		// list of game names

static int ChatSend[MAX_PLAYERS+1];        // Who to send chat messages to
static int ChatRec[MAX_PLAYERS+1];         // Who to receive messages from


REQEVENT EventTable;				//--- Event table
REPEVENT EventReply[MAX_PLAYERS];	//--- Event reply
RESEVENT EventResults;				//--- Event results

int EventStarter = FALSE;          //---- Whether this machine started the event
int iAdventOver = FALSE;           //---- If adventure is over


long SyncTimeOuts[MAX_PLAYERS];   				//---- track sync timeouts
long SyncCounts[MAX_PLAYERS];   					//---- track sync timeouts

int SyncTable[iMPS_SYNCEND][MAX_PLAYERS];   //---- our inc table

int SyncAction[MAX_PLAYERS];   //---- action sync

long SyncMsgIDTable[iMPS_SYNCEND][MAX_PLAYERS];   //---- message id table


unsigned long ulRseeds[kNumSeeds];  //---- Random table 


unsigned int fBRealm[MAX_PLAYERS];  //---- Trigger for master to broadcast realm
long    lRegent[MAX_PLAYERS];  //---- regent for master to broadcast with realm

float	MasterRealmAIVersionNumber;

int iOnSeed = 0;


// -----------------------------------------------------------------
//
//	 MultiPlayer::MultiPlayer
//		Default constructor.
//
//	 Inputs:
//		none
//
// -----------------------------------------------------------------
MultiPlayer::MultiPlayer( void ) : eStatus( iMPS_NONE )
{


	InitGoop();

	//---- Read the config stuff

//	ReadCfg();


} // MultiPlayer::MultiPlayer




// -----------------------------------------------------------------
//
//	 MultiPlayer::~MultiPlayer
//		Default destructor.
//
// -----------------------------------------------------------------
MultiPlayer::~MultiPlayer( void )
{

	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		char temp[200];
		sprintf ( temp, "MultiPlayer::MultiPlayer - finalize");
		RandomLogComment ( temp );
	}

	if ( eStatus != iMPS_ENDED )
		Finalize(1);

} // MultiPlayer::~MultiPlayer



// -----------------------------------------------------------------
//
//	 MultiPlayer::GetPlayerName
//	 Inputs:
//	 Return: player friendly name 
//
// -----------------------------------------------------------------

PCHAR MultiPlayer::GetPlayerName( void )
{ 
	return ( &szName[0] ); 
}


// -----------------------------------------------------------------
//
//	 MultiPlayer::GetGameType
//	 Inputs:
//	 Return: game type we are playing
//
// -----------------------------------------------------------------

int MultiPlayer::GetGameType( void )
{ 
	return ( iGameType ); 
}

// -----------------------------------------------------------------
//
//	 MultiPlayer::IsModemGame
//	 Inputs:
//	 Return: are we doing a modem game ?
//
// -----------------------------------------------------------------

BOOL MultiPlayer::IsModemGame( void )
{ 
   return ( GenericMultiPlayer::IsModemGame() );
}


// -----------------------------------------------------------------
//
//	 MultiPlayer::ForceStart
//	 Inputs:
//	 Return:
//
// -----------------------------------------------------------------

BOOL MultiPlayer::ForceStart( void )
{ 
   return ( GenericMultiPlayer::ForceStart() );
}


// -----------------------------------------------------------------
//
//	 MultiPlayer::DisablePlayers
//	 Inputs:
//	 Return:
//
// -----------------------------------------------------------------

void MultiPlayer::DisablePlayers( void )
{ 
   GenericMultiPlayer::DisablePlayers();
}



// -----------------------------------------------------------------
//
//	 MultiPlayer::GetProtocolIndex
//	 Inputs:
//	 Return: return selected protocol index
//
// -----------------------------------------------------------------

short MultiPlayer::GetProtocolIndex( void )
{ 
	return ( sProtocol ); 
}



// -----------------------------------------------------------------
//
//	 MultiPlayer::GetGameName
//	 Inputs:
//	 Return: returns game name
//
// -----------------------------------------------------------------
PCHAR MultiPlayer::GetGameName( void )	
{ 
	return ( &szGame[0] ); 
}



// -----------------------------------------------------------------
//
//	 MultiPlayer::GetNumPlayers
//	 Inputs:
//	 Return: number of players 
//
// -----------------------------------------------------------------
short MultiPlayer::GetNumPlayers( void )	
{ 
	return ( sNumPlayers ); 
}


// -----------------------------------------------------------------
//
//	 MultiPlayer::GetSpeed
//	 Inputs:
//	 Return: connection speed ( 0 - modem 1 - network )
//
// -----------------------------------------------------------------
short MultiPlayer::GetSpeed( void )	
{ 
	return ( sConnSpeed ); 
}



// -----------------------------------------------------------------
//
//	 MultiPlayer::GetPlayerId
//	 Inputs:
//	 Return: our player id
//
// -----------------------------------------------------------------
DWORD MultiPlayer::GetPlayerId( void )
{
   return GenericMultiPlayer::GetPlayerId();

} // MultiPlayer::GetPlayerId()


// -----------------------------------------------------------------
//
//	 MultiPlayer::GetAPlayerId
//	 Inputs:
//	 Return: a player id
//
// -----------------------------------------------------------------
DWORD MultiPlayer::GetAPlayerId( int i )
{

	return GenericMultiPlayer::GetAPlayerId( i );

}

// -----------------------------------------------------------------
//
//	 MultiPlayer::CheckPlayerId
//	 Inputs:
//	 Return: TRUE if playerid was found 
//
// -----------------------------------------------------------------
BOOL MultiPlayer::CheckPlayerId( DWORD dwPlayerId )
{
	int x;


	for ( x = 0; x < MAX_PLAYERS; ++x )
	{
		if ( AMultiPlayer.GetAPlayerId(x) == dwPlayerId )
		{
			return TRUE;
		}

	}

	return FALSE;

}

// -----------------------------------------------------------------
//
//	 MultiPlayer::HowManyPlayers
//	 Inputs:
//	 Return: returns number of active players
//
// -----------------------------------------------------------------
LONG MultiPlayer::HowManyPlayers( void )
{
	int x, count = 0;

	for ( x = 0; x < MAX_PLAYERS; ++x )
	{
		if ( AMultiPlayer.GetAPlayerId(x) != kNoPlayer  )
		{
			++count;
		}
	}

	return count;
}



// -----------------------------------------------------------------
//
//	 MultiPlayer::GetDeletedPlayer
//	 Inputs:
//	 Return: 
//
// -----------------------------------------------------------------
int MultiPlayer::GetDeletedPlayer( int i )
{

	return GenericMultiPlayer::GetDeletedPlayer( i );

}


// -----------------------------------------------------------------
//
//	 MultiPlayer::GetChat
//	 Inputs:
//	 Return: 
//
// -----------------------------------------------------------------
int MultiPlayer::GetChat( int index )		
{
	return ( ChatSend[index] );
 
}


// -----------------------------------------------------------------
//
//	 MultiPlayer::GetChatRec
//	 Inputs:
//	 Return: 
//
// -----------------------------------------------------------------
int MultiPlayer::GetChatRec( int index )		
{
	return ( ChatRec[index] );
 
}



// -----------------------------------------------------------------
//
//	 MultiPlayer::GetRealmFromId
//	 Inputs: player id
//	 Return: realm of player id 
//
// -----------------------------------------------------------------

int MultiPlayer::GetRealmFromId( DWORD dwId  )
{
	int i;

	for ( i = 0; i < LAND_REALM_COUNT; ++i )
	{
		if ( lRealmSelectedByPlayer[i] == dwId )
		{
			return i;
		}
	} 

	return i;

}



// -----------------------------------------------------------------
//
//	 MultiPlayer::GetRealmNameOfEvent
//	 Inputs: player id
//	 Return: realm name player id  otherwise NULL
//
// -----------------------------------------------------------------

PCHAR MultiPlayer::GetRealmNameOfEvent(void)
{
	int i;

	for ( i = 0; i < LAND_REALM_COUNT; ++i )
	{
		if ( lRealmSelectedByPlayer[i] == EventTable.dwFrom )
		{
			return realm[i].mfGetName();
		}
	} 

	return NULL;

}



// -----------------------------------------------------------------
//
//	 MultiPlayer::GetGameInfo
//		Get the game information.
//
//	 Inputs:
//		none
//
//	 Return: pointer to game information
//
// -----------------------------------------------------------------
PNETGAMEINFO MultiPlayer::GetGameInfo( void )
{

	return ( GetNetModule()->GetNetworkGameInfo() );

} // MultiPlayer::GetGameInfo



// -----------------------------------------------------------------
//
//	 MultiPlayer::GetGames
//
//	 Inputs:
//
//	 Return:
//
// -----------------------------------------------------------------

void MultiPlayer::GetGames(void)
{
	int i;

	for (i=0; i < MAX_GAMES; ++i )
	{
		memset ( &pszGames[i][0], 0, 24 );
	}

	AMultiPlayer.EnumerateGames( DisplayString, 1 );

}



// -----------------------------------------------------------------
//
//	 MultiPlayer::GetAGames
//		Get a specific game 
//
//	 Inputs:
//
//	 Return: game name
//
// -----------------------------------------------------------------
PCHAR MultiPlayer::GetAGames(int i)
{
	// -- the n+1 entry is a row of spaces, not 0
	// -- this tries to fix that
	if (pszGames[i][0] == 32
		&& pszGames[i][1] == 32
		&& pszGames[i][2] == 32 )
		pszGames[i][0] = 0;
		
	return pszGames[i];
}


// -----------------------------------------------------------------
//
//	 MultiPlayer::GetProtocol
//	 Inputs:
//	 Return: 
//
// -----------------------------------------------------------------
PCHAR MultiPlayer::GetProtocol ( void )
{
	   return pszProtocol[sProtocol];

}


// -----------------------------------------------------------------
//
//	 MultiPlayer::GetAProtocol
//	 Inputs:
//	 Return: 
//
// -----------------------------------------------------------------
PCHAR MultiPlayer::GetAProtocol ( int i )
{
	   return pszProtocol[i];

}



// -----------------------------------------------------------------
//
//	 MultiPlayer::GetOppenentName
//	 Inputs: which player 
//	 Return: return friendly name
//							   
// -----------------------------------------------------------------

PCHAR MultiPlayer::GetOpponentName( int i )	
{ 
	return ( (PCHAR) &szOpponentName[i] ); 
}


// -----------------------------------------------------------------
//
//	 MultiPlayer::IsInitialized
//	 Inputs:
//	 Return: 1- if initialized
//
// -----------------------------------------------------------------
BOOL MultiPlayer::IsInitialized( void )	
{ 
	return ( eStatus >= iMPS_INIT ); 
}

// -----------------------------------------------------------------
//
//	 MultiPlayer::IsRunning
//	 Inputs:
//	 Return: 1 - if running 
//
// -----------------------------------------------------------------
BOOL MultiPlayer::IsRunning( void )	
{ 
	return ( eStatus >= iMPS_SETUP );
}

// -----------------------------------------------------------------
//
//	 MultiPlayer::IsMaster
//	 Inputs:
//	 Return: if started the game
//
// -----------------------------------------------------------------
BOOL MultiPlayer::IsMaster( void )	
{ 
	return ( fMaster ); 
}

// -----------------------------------------------------------------
//
//	 MultiPlayer::IsSlave
//	 Inputs:
//	 Return: didn't start the game
//
// -----------------------------------------------------------------
BOOL MultiPlayer::IsSlave( void )
{ 
	return ( !fMaster ); 
}


   // --
   // -- Setting data
   // --


// -----------------------------------------------------------------
//
//	 MultiPlayer::ResetSync
//	 Inputs:
//	 Return: 
//
// -----------------------------------------------------------------

void  MultiPlayer::ResetSync( int sync )
{ 
	int i;

	//---- Reset replies for this sync point
	
	for ( i = 0; i < MAX_PLAYERS; ++i )
	{
		SyncTable[sync][i] = 0;
	}

}

// -----------------------------------------------------------------
//
//	 MultiPlayer::GetSync
//	 Inputs:
//	 Return: 
//
// -----------------------------------------------------------------

int MultiPlayer::GetSync( int sync, int index )
{
 	return SyncTable[sync][index];
}



// -----------------------------------------------------------------
//
//	 MultiPlayer::ResetSyncAction
//	 Inputs: rlm - which rlm to resync
//	 Return: 
//
// -----------------------------------------------------------------

void  MultiPlayer::ResetSyncAction( int rlm )
{ 
	int i;

	if ( IsMultiPlayer() )
	{

		//---- Find out where this sync is 

		for ( i=0; i < MAX_PLAYERS; ++i )
		{
			//---- if this is the realm 
			
			if ( lRealmSelectedByPlayer[rlm] == AMultiPlayer.GetAPlayerId(i) )
			{
				SyncAction[i] = 0;
				break;
			}
	
		}
	}

}



// -----------------------------------------------------------------
//
//	 MultiPlayer::GetSyncAction
//	 Inputs: rlm - get this realms sync 
//	 Return: 
//
// -----------------------------------------------------------------

int MultiPlayer::GetSyncAction( int rlm )
{

	int i;


	if ( IsMultiPlayer() )
	{

		//---- Find out where this sync is 

		for ( i=0; i < MAX_PLAYERS; ++i )
		{
			//---- if this is the realm 

			if ( lRealmSelectedByPlayer[rlm] == AMultiPlayer.GetAPlayerId(i) )
			{
				return SyncAction[i];
			}

		}

	}

	//---- We couldn't find this player so just return that the turn is done

	return 1;
		
}



// -----------------------------------------------------------------
//
//	 MultiPlayer::ResetAllSyncTimes
//	 Inputs:
//	 Return: 
//
// -----------------------------------------------------------------

void  MultiPlayer::ResetAllSyncTimes( void )
{ 
	int i;

	//---- Reset replies for this sync point
	
	for ( i = 0; i < MAX_PLAYERS; ++i )
	{
		SyncTimeOuts[i] = (long)GetTickCount();
	}

}

// -----------------------------------------------------------------
//
//	 MultiPlayer::GetSyncTime
//	 Inputs:
//	 Return: 
//
// -----------------------------------------------------------------

long MultiPlayer::GetSyncTime( int index )
{
 	return ((long)GetTickCount() - SyncTimeOuts[index]);
}

// -----------------------------------------------------------------
//
//	 MultiPlayer::ResetSyncTime
//	 Inputs:
//	 Return: 
//
// -----------------------------------------------------------------

void MultiPlayer::ResetSyncTime( int index )
{
	SyncTimeOuts[index] = GetTickCount();
}

// -----------------------------------------------------------------
//
//	 MultiPlayer::ResetAllSyncCounts
//	 Inputs:
//	 Return: 
//
// -----------------------------------------------------------------

void  MultiPlayer::ResetAllSyncCounts( void )
{ 
	int i;

	//---- Reset replies for this sync point
	
	for ( i = 0; i < MAX_PLAYERS; ++i )
	{
		SyncCounts[i] = 0;
	}

}

// -----------------------------------------------------------------
//
//	 MultiPlayer::GetSyncCount
//	 Inputs:
//	 Return: 
//
// -----------------------------------------------------------------

long MultiPlayer::GetSyncCount( int index )
{
 	return SyncCounts[index];
}

// -----------------------------------------------------------------
//
//	 MultiPlayer::IncSyncCount
//	 Inputs:
//	 Return: 
//
// -----------------------------------------------------------------

void MultiPlayer::IncSyncCount( int index )
{
	++SyncCounts[index] ;
}



// -----------------------------------------------------------------
//
//	 MultiPlayer::SetProtocolIndex
//	 Inputs:
//	 Return: current protocol index
//
// -----------------------------------------------------------------

void  MultiPlayer::SetProtocolIndex( int sP )
{ 
	sProtocol = sP;
}



// -----------------------------------------------------------------
//
//	 MultiPlayer::SetNumPlayers
//	 Inputs:
//	 Return: number of players
//
// -----------------------------------------------------------------
void  MultiPlayer::SetNumPlayers( short sNP )
{ 
	sNumPlayers = sNP;
}

// -----------------------------------------------------------------
//
//	 MultiPlayer::SetSpeed
//	 Inputs:
//	 Return: set connection speed
//
// -----------------------------------------------------------------
void  MultiPlayer::SetSpeed( short sCS )
{ 
	sConnSpeed = sCS;
}



// -----------------------------------------------------------------
//
//	 MultiPlayer::SetGameType
//	 Inputs:
//	 Return: set game type we are playing
//
// -----------------------------------------------------------------

void MultiPlayer::SetGameType( int iGT )
{ 
	iGameType = iGT;
}


// -----------------------------------------------------------------
//
//	 MultiPlayer::SetMaster
//	 Inputs: master flag
//	 Return: 
//
// -----------------------------------------------------------------
void  MultiPlayer::SetMaster( BOOL fMstr )		
{ 
	fMaster = fMstr;

	
	if ( fSigs ) //---- Sigs starts at 0
		dwMsgFudge = ( AMultiPlayer.GetPlayerId() % 8 ) * dwMsgMasterFudge;
	else //--- DPlay starts at 1 not zero
		dwMsgFudge = ( ( AMultiPlayer.GetPlayerId() - 1 ) % 8 ) * dwMsgMasterFudge;

	#ifdef _MULTIMONO
		mono_printf ( 0, 6, "My Fudge is %lu", dwMsgFudge );
	#endif


//	if ( fMaster )
//		dwMsgFudge = dwMsgMasterFudge;
//	else
//		dwMsgFudge = 0;


}


// -----------------------------------------------------------------
//
//	 MultiPlayer::SetChat
//	 Inputs:
//	 Return:
//
// -----------------------------------------------------------------
void  MultiPlayer::SetChat( int index, int set )
{ 

	ChatSend[index] = set;

}

// -----------------------------------------------------------------
//
//	 MultiPlayer::SetChatRec
//	 Inputs:
//	 Return:
//
// -----------------------------------------------------------------
void  MultiPlayer::SetChatRec( int index, int set )
{ 

	ChatRec[index] = set;

}


// -----------------------------------------------------------------
//
//	 MultiPlayer::SetControl
//	 Inputs:
//	 Return:
//
// -----------------------------------------------------------------
void  MultiPlayer::SetControl( int dwId, int control )
{ 
	int i;

	for ( i = 0; i < LAND_REALM_COUNT; ++i )
	{
		if ( lRealmSelectedByPlayer[i] == dwId )
		{
 			if ( realm[i].mfIsPlayerCtrl() == control )
			{
				#ifdef _MULTIMONO
			       mono_puts ( 40, 4, "Player Control Already Set!!!!" );
				#endif
								
			}

			realm[i].mfSetPlayerCtrl(control);

		}

	}


}



// -----------------------------------------------------------------
//
//	 MultiPlayer::InitEvent
//	 Inputs: 0 - all 1-table 2-reply 3-results
//	 Return: 
//
// -----------------------------------------------------------------
void  MultiPlayer::InitEvent( int which )
{ 
	int i;


	if ( which == 0 )
	{
		EventStarter = FALSE;
	}


	if ( which == 0 || which == 1 )
	{

		//---- initialize the event table 

		memset ( &EventTable, 0, sizeof ( REQEVENT ) );

	}


	if ( which == 0 || which == 2 )
	{
		for ( i = 0; i < MAX_PLAYERS; ++i )
		{
			//---- Reset reply

			EventReply[i].EventId = iMPE_NONE;			//---- Event has come in
			EventReply[i].replied = 0; 			        //---- 1 we have replied
			EventReply[i].data1   = 0;					//---- data 1
			EventReply[i].data2   = 0;					//---- data 2
			EventReply[i].data3   = 0;					//---- data 3
			EventReply[i].data4   = 0;					//---- data 3
		}
	}


	if ( which == 0 || which == 3 )
	{
		EventResults.EventId = iMPE_NONE;
		EventResults.data1   = 0;
		EventResults.data2   = 0;
		EventResults.data3   = 0;
		EventResults.data4   = 0;
	}


}	//---- MultiPlayer::InitEvent() 



// -----------------------------------------------------------------
//
//	 MultiPlayer::InitChat
//	 Inputs:
//	 Return: 
//
// -----------------------------------------------------------------
void  MultiPlayer::InitChat( void )		
{ 
	int i;

//	ChatSend[0] = 1; //---- All Players
//	ChatRec[0] = 1;  //---- All Players

	for ( i=0; i < MAX_PLAYERS+1; ++i )
	{
		ChatSend[i] = 1;
		ChatRec[i] = 1;
	}



}	//---- MultiPlayer::InitChat()


// -----------------------------------------------------------------
//
//	 MultiPlayer::GetAEventReply
//
//		 get reply status and data of reply
//
//	 Inputs: which - which reply
//
//
//	 Return: TRUE if we have reply
//
// -----------------------------------------------------------------
BOOL MultiPlayer::GetAEventReply( int which, int * data1, int * data2, int * data3, int * data4 )
{
	*data1 = 0;
	*data2 = 0;
	*data3 = 0;
	*data4 = 0;


	//---- is the event set 

	if ( EventTable.EventId != iMPE_NONE )
	{

		//---- They have replied 

		if ( EventTable.replied[which] )
		{

			*data1 = EventReply[which].data1;
			*data2 = EventReply[which].data2;
			*data3 = EventReply[which].data3;
			*data4 = EventReply[which].data4;

			return TRUE;

		}
		else
		{
			return FALSE; 
		}

	}

	return FALSE;



}




// -----------------------------------------------------------------
//
//	 MultiPlayer::HaveEvent
//
//		 Check if an event has come in for us 
//
//	 Inputs: 
//
//	 Return: 0 not >0 we have an event 
//
// -----------------------------------------------------------------
int MultiPlayer::HaveEvent( void )
{

	//---- is the event set

	return EventReply[0].EventId;  //---- filled in when event comes in


}

// -----------------------------------------------------------------
//
//	 MultiPlayer::HaveResults
//
//		 Check if results have come in
//
//	 Inputs: 
//
//	 Return: 0 not >0 we have an event 
//
// -----------------------------------------------------------------
int MultiPlayer::HaveResults( void )
{

	//---- is the event set

	return EventResults.EventId;  //---- filled in when event comes in


}


// -----------------------------------------------------------------
//
//	 MultiPlayer::CheckForPlayers
//
//		 Check if everyone has left
//
//	 Inputs: 
//
//	 Return: 0 not > 0 no other players are in the game
//
// -----------------------------------------------------------------
int MultiPlayer::CheckForPlayers( void )
{
	int x;
	static int dwTime = 0;



#if 0
	if ( fSigs )
	{

		if ( dwTime < GetTickCount() )
		{

		#ifdef _MULTIMONO
			mono_printf ( 0, 14, "Connect Status %d", 	
						  GenericMultiPlayer::CheckConnectStatus() );
		#endif

			dwTime = GetTickCount() + 5000;

		}

	}
#endif


	
	for ( x = 0; x < MAX_PLAYERS; ++x )
	{
		if ( GetAPlayerId(x) != kNoPlayer )
		{
			return FALSE;
		}

	}


	return TRUE;


}



// -----------------------------------------------------------------
//
//	 MultiPlayer::CheckEvent
//
//		 Check status of the event
//
//	 Inputs: 
//
//	 Return: FALSE still waiting TRUE if all replies have come in
//
// -----------------------------------------------------------------
BOOL MultiPlayer::CheckEvent( void )
{
	BOOL fOk = TRUE;

	//---- is the event set 

	if ( EventTable.EventId != iMPE_NONE )
	{
		// ok which one is it 

 		if ( EventTable.EventId == iMPE_ADVENTURE ||
 			 EventTable.EventId == iMPE_ADD_ITEM ||
 			 EventTable.EventId == iMPE_DROP_ITEM)
		{
			int i;

			for ( i = 0; i < MAX_PLAYERS; ++i )
			{
				if ( GetAPlayerId(i) != kNoPlayer )
				{
					if ( !EventTable.replied[i] )
					{
						fOk = FALSE;
					}
				}
			}
			
			return fOk;

		}
		else
		{
			if ( EventTable.replied[0] )
			{
				return TRUE; 
			}
		}

	}

	return FALSE;

}



// -----------------------------------------------------------------
//
//	 MultiPlayer::SendEvent
//
//		 Send an event message 
//
//	 Inputs: realm to send to 
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::SendEvent( int EventId, 
					    	  int data1, int data2, int data3, int data4, int rlm )
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;

	if ( AMultiPlayer.IsRunning() )
	{

		//---- Initialize the event tables 

		AMultiPlayer.InitEvent();


		EventStarter = TRUE;


		// -- Make the message

		sSize = DoHeader( iMPT_EVENT, &pMsg );

		((PBREvent) pMsg)->EventId = EventId;
		((PBREvent) pMsg)->data1   = data1;
		((PBREvent) pMsg)->data2   = data2;
		((PBREvent) pMsg)->data3   = data3;
		((PBREvent) pMsg)->data4   = data4;


		//---- Set the event 

		EventTable.data1   = data1;
		EventTable.data2   = data2;
		EventTable.data3   = data3;
		EventTable.data4   = data4;
		EventTable.EventId = EventId;


		//---- Send the message!

		StartWait( iMPM_SEND );

		//---- Everyone replies on the the adventure messages 

		if ( EventId == iMPE_ADVENTURE ||
			 EventId == iMPE_GAIN_EXP ||
			 EventId == iMPE_ADD_ITEM ||
			 EventId == iMPE_DROP_ITEM)
		{
			fSent = SendBroadcastMessage( sSize, TRUE );
		}
		else
		{

			if ( lRealmSelectedByPlayer[rlm] != -1 )
			{
				fSent = SendDirectMessage( lRealmSelectedByPlayer[rlm],
										   sSize, TRUE );
			}

		}


		#ifdef _MULTIMONO
	       mono_printf ( 40, 16, "Set Event %d", EventId );
		#endif


		EndWait( iMPM_SEND );


	}

	return ( fSent );


}	 // MultiPlayer::SendEvent




// -----------------------------------------------------------------
//
//	 MultiPlayer::SendSync
//
//		 Tell everyone we are at this sync point
//
//	 Inputs:
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::SendSync( int sync )
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{
		// -- Make the message
		sSize = DoHeader( iMPT_SYNC, &pMsg );
		((PBRSync)pMsg)->iSync = sync;

		//---- Send the message!
		StartWait( iMPM_SEND );
		fSent = SendBroadcastMessage( sSize, TRUE );
		EndWait( iMPM_SEND );
	}
	return ( fSent );

} // MultiPlayer::SendSync



BOOL MultiPlayer::SendSyncTo( int sync, DWORD dwToWhom )
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{
		// -- Make the message
		sSize = DoHeader( iMPT_SYNC, &pMsg );
		((PBRSync)pMsg)->iSync = sync;

		//---- Send the message!
		StartWait( iMPM_SEND );
		fSent = SendDirectMessage( dwToWhom, sSize, TRUE );
		EndWait( iMPM_SEND );
	}
	return ( fSent );

} // MultiPlayer::SendSync

// -----------------------------------------------------------------
//
//	 MultiPlayer::SendSyncAction
//
//		 Tell everyone we are done with our turn
//
//	 Inputs:
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::SendSyncAction( void )
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{
		// -- Make the message

		sSize = DoHeader( iMPT_SYNCACTION, &pMsg );

		//---- Send the message!

		StartWait( iMPM_SEND );

		fSent = SendBroadcastMessage( sSize, TRUE );

		EndWait( iMPM_SEND );

		fRedrawAll = TRUE;


	}

	return ( fSent );


} // MultiPlayer::SendSyncAction()





// -----------------------------------------------------------------
//
//	 MultiPlayer::ReplyEvent
//
//		 Reply to an event message 
//
//	 Inputs:
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::ReplyEvent( int EventId,
					     	   int data1, int data2, int data3, int data4 )
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{
		// -- Make the message

		sSize = DoHeader( iMPT_REPEVENT, &pMsg );

		((PBREventReply) pMsg)->EventId = EventId;
		((PBREventReply) pMsg)->data1   = data1;
		((PBREventReply) pMsg)->data2   = data2;
		((PBREventReply) pMsg)->data3   = data3;
		((PBREventReply) pMsg)->data4   = data4;


		//---- Send the message!

		StartWait( iMPM_SEND );


		fSent = SendDirectMessage( EventTable.dwFrom, sSize, TRUE );


		EndWait( iMPM_SEND );
	}

	return ( fSent );


} // MultiPlayer::ReplyEvent



// -----------------------------------------------------------------
//
//	 MultiPlayer::ResultsEvent
//
//		 Results of an event message 
//
//	 Inputs:
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::ResultsEvent( int EventId, 
					        	 int data1, int data2, int data3, int data4, int rlm )
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{
		// -- Make the message

		sSize = DoHeader( iMPT_RESEVENT, &pMsg );

		((PBREventResults) pMsg)->EventId = EventId;
		((PBREventResults) pMsg)->data1   = data1;
		((PBREventResults) pMsg)->data2   = data2;
		((PBREventResults) pMsg)->data3   = data3;
		((PBREventResults) pMsg)->data4   = data4;


		//---- Send the message!

		StartWait( iMPM_SEND );


		//---- Everyone gets the results in adventures 

		if ( EventId == iMPE_ADVENTURE ||
			 EventId == iMPE_GAIN_EXP ||
			 EventId == iMPE_ADD_ITEM ||
			 EventId == iMPE_DROP_ITEM)
		{
			fSent = SendBroadcastMessage( sSize, TRUE );
		}
		else
		{

			if ( lRealmSelectedByPlayer[rlm] != -1 )
			{
				fSent = SendDirectMessage( lRealmSelectedByPlayer[rlm],
										   sSize, TRUE );
			}

		}


		EndWait( iMPM_SEND );
	}

	return ( fSent );


} // MultiPlayer::ResultsEvent



// -----------------------------------------------------------------
//
//	 MultiPlayer::SendCtrl
//
//		 Send player control. Used for telling the master you want 
//      AI control or not.
//
//	 Inputs: TRUE - control FALSE - AI 
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::SendControl( int control )
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;


	if ( AMultiPlayer.IsRunning() &&
	     !IsMaster()                 )
	{


		if ( eStatus == iMPS_SETUP )
		{
			// -- Make the message

			sSize = DoHeader( iMPT_CONTROL, &pMsg );

			((PBRControl) pMsg)->control = control;


			//---- Send the message!

			StartWait( iMPM_SEND );

			fSent = SendDirectMessage( lMasterId, sSize, TRUE );

			EndWait( iMPM_SEND );

		}

	}

	return ( fSent );




} // MultiPlayer::SendControl



// -----------------------------------------------------------------
//
//	 MultiPlayer::SendWonGame
//
//		 Broadcast that this machine won the game. 
//
//	 Inputs: Homerealm - realm of winner 
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::SendWonGame( SHORT HomeRealm )
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;


	if ( AMultiPlayer.IsRunning() ) 
	{

		if ( eStatus == iMPS_SETUP )
		{
			// -- Make the message

			sSize = DoHeader( iMPT_WONGAME, &pMsg );

			((PBRWonGame) pMsg)->HomeRealm = HomeRealm;


			//---- Send the message!

			StartWait( iMPM_SEND );

   	    //---- To broadcast or not to broadcast .... 

		    fSent = SendBroadcastMessage( sSize, TRUE );

			EndWait( iMPM_SEND );

		}

	}

	return ( fSent );




} // MultiPlayer::SendWonGame()



// -----------------------------------------------------------------
//
//	 MultiPlayer::BroadcastAdventOver
//
//		 Broadcasts adventure is over
//
//	 Inputs:
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::BroadcastAdventOver( void )
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{

		// -- Make the message

		sSize = DoHeader( iMPT_ADVENTOVER, &pMsg );


		//---- Send the message!

		StartWait( iMPM_SEND );


       //---- To broadcast or not to broadcast .... 

	    fSent = SendBroadcastMessage( sSize, TRUE );

		EndWait( iMPM_SEND );

	}

	return ( fSent );

} // MultiPlayer::BroadcastAdventOver



// -----------------------------------------------------------------
//
//	 MultiPlayer::BroadcastRealmInfo
//
//		 Broadcasts a realm info message.
//
//	 Inputs:
//		HomeRealm	- which realm the master wants
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::BroadcastRealmInfo( SHORT WhichRealm, DWORD WhichId, LONG lWhichRegent )
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{
		#if 0
		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
			char temp[200];
			sprintf ( temp, "MultiPlayer::BroadcastRealmInfo - Realm:%s Regent:%d ID:%ld",
				debug_realm_names[WhichRealm], lWhichRegent, WhichId );
			RandomLogComment ( temp );
		}
		#endif

		// -- Make the message
		sSize = DoHeader( iMPT_BREALM, &pMsg );

		((PBRRealm) pMsg)->iWhichRealm = WhichRealm;
		((PBRRealm) pMsg)->lPlayerId   = WhichId;
		((PBRRealm) pMsg)->lWhichRegent = lWhichRegent;


		//---- Send the message!

		StartWait( iMPM_SEND );


       //---- To broadcast or not to broadcast .... 

	    fSent = SendBroadcastMessage( sSize, TRUE );

		EndWait( iMPM_SEND );
	}

	return ( fSent );


} // MultiPlayer::BroadcastRealmInfo





// -----------------------------------------------------------------
//
//	 MultiPlayer::RequestRealm
//
//		 Asks master for a realm request
//
//	 Inputs:
//		HomeRealm	- which realm we want
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::RequestRealm( SHORT WhichRealm, DWORD WhichId, LONG lWhichRegent)
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{
		// -- Make the message

		sSize = DoHeader( iMPT_REALM, &pMsg );

		((PBRRealm) pMsg)->iWhichRealm = WhichRealm;
		((PBRRealm) pMsg)->lPlayerId = WhichId;
		((PBRRealm) pMsg)->lWhichRegent = lWhichRegent;


		//---- Send the message!

		StartWait( iMPM_SEND );


       //---- To send direct

		fSent = SendDirectMessage( lMasterId, sSize, TRUE );

//	    fSent = SendBroadcastMessage( sSize, TRUE );

		EndWait( iMPM_SEND );
	}
	return ( fSent );

} // MultiPlayer::RequestRealm




// -----------------------------------------------------------------
//
//	 MultiPlayer::BroadcastSetGameData
//
//		 Broadcasts a game data message
//
//	 Inputs:
//		lArray_Id		- which array type to set
//		lStruct_Id		- which element in struct to set
//		lArray_Index	- which index in the array
//		lData			- data to set
//		dwToWhom		- player id of player to receive message, kBroadcast if all
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::BroadcastSetGameData(LONG lArray_Id, LONG lStruct_Id, LONG lArray_Index, LONG lData, DWORD dwToWhom )
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{
		// -- Make the message

		sSize = DoHeader( iMPT_INDEXSEND, &pMsg );

		((PBRIndexS) pMsg)->lArray_Id	= lArray_Id;
		((PBRIndexS) pMsg)->lStruct_Id	= lStruct_Id;
		((PBRIndexS) pMsg)->lArray_Index	= lArray_Index;
		((PBRIndexS) pMsg)->lData		= lData;


		//---- Send the message!

		StartWait( iMPM_SEND );


       //---- To broadcast or not to broadcast .... 

		if ( dwToWhom != kBroadcast )
		{
			fSent = SendDirectMessage( dwToWhom, sSize, TRUE );
			#ifdef _MULTIMONO
	       	mono_printf ( 0, 3, "-> Game Data index %d      id%lu", 
						  lData,
						  dwToWhom );
			#endif
		}
		else
		{
			#ifdef _MULTIMONO
	    	mono_printf ( 0, 3, "B-> Game Data A%d S%d AI%d D%d", 
						  lArray_Id,
						  lStruct_Id,
						  lArray_Index,	
						  lData );
			#endif

			fSent = SendBroadcastMessage( sSize, TRUE );

		}

		EndWait( iMPM_SEND );
	}
	return ( fSent );

} // MultiPlayer::BroadcastSetGameData



#if 0
// !!!!! DON'T USE THIS

// -----------------------------------------------------------------
//
//	 MultiPlayer::RequestSetGameData
//
//		 Asks master to set game data
//
//	 Inputs:
//		lArray_Id		- which array type to set
//		lStruct_Id		- which element in struct to set
//		lArray_Index	- which index in the array
//		lData			- data to set
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::RequestSetGameData(LONG lArray_Id, LONG lStruct_Id, LONG lArray_Index, LONG lData)
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{
		// -- Make the message

		sSize = DoHeader( iMPT_INDEXREQUEST, &pMsg );

		((PBRIndexS) pMsg)->lArray_Id	= lArray_Id;
		((PBRIndexS) pMsg)->lStruct_Id	= lStruct_Id;
		((PBRIndexS) pMsg)->lArray_Index	= lArray_Index;
		((PBRIndexS) pMsg)->lData		= lData;


		//---- Send the message!

		StartWait( iMPM_SEND );

		#ifdef _MULTIMONO
	       mono_printf ( 0, 2, "RM-> index %d      ", lData );
		#endif

       //---- To broadcast or not to broadcast .... 

  		fSent = SendDirectMessage( lMasterId, sSize, TRUE );

//	    fSent = SendBroadcastMessage( sSize, TRUE );

		EndWait( iMPM_SEND );
	}
	return ( fSent );

} // MultiPlayer::RequestSetGameData

#endif

// DONT USE dlj
#if 0
// -----------------------------------------------------------------
//
//	 MultiPlayer::BroadcastGetGameData
//
//		 Broadcasts a game data message
//
//	 Inputs:
//		lArray_Id		- which array type to set
//		lStruct_Id		- which element in struct to set
//		lArray_Index	- which index in the array
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::BroadcastGetGameData(LONG lArray_Id, LONG lStruct_Id, LONG lArray_Index)
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{
		// -- Make the message

		sSize = DoHeader( iMPT_DATASEND, &pMsg );

		((PBRIndexR) pMsg)->lArray_Id	= lArray_Id;
		((PBRIndexR) pMsg)->lStruct_Id	= lStruct_Id;
		((PBRIndexR) pMsg)->lArray_Index	= lArray_Index;


		//---- Send the message!

		StartWait( iMPM_SEND );


       //---- To broadcast or not to broadcast .... 

	    fSent = SendBroadcastMessage( sSize, TRUE );

		EndWait( iMPM_SEND );
	}
	return ( fSent );

} // MultiPlayer::BroadcastGetGameData
#endif



//!!!!!!!!!!!! DONT USE THIS
#if 01
// -----------------------------------------------------------------
//
//	 MultiPlayer::RequestGetGameData
//
//		 Asks master to set game data
//
//	 Inputs:
//		lArray_Id		- which array type to set
//		lStruct_Id		- which element in struct to set
//		lArray_Index	- which index in the array
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::RequestGetGameData(LONG lArray_Id, LONG lStruct_Id, LONG lArray_Index)
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{
		// -- Make the message

		sSize = DoHeader( iMPT_DATAREQUEST, &pMsg );

		((PBRIndexR) pMsg)->lArray_Id	= lArray_Id;
		((PBRIndexR) pMsg)->lStruct_Id	= lStruct_Id;
		((PBRIndexR) pMsg)->lArray_Index	= lArray_Index;


		//---- Send the message!

		StartWait( iMPM_SEND );


       //---- To broadcast or not to broadcast .... 

		fSent = SendDirectMessage( lMasterId, sSize, TRUE );

//	    fSent = SendBroadcastMessage( sSize, TRUE );

		EndWait( iMPM_SEND );
	}
	return ( fSent );

} // MultiPlayer::RequestGetGameData

#endif


// -----------------------------------------------------------------
//
//	 MultiPlayer::Chat
//
//		 Send a chat message to ChatSend list 
//
//	 Inputs:
//		szMsg		- the message, including the name
//		force       - forces broadcast to everyone 
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::Chat( char * pszMsg, int force )
{
	short sSize = 0;
	PCHAR pMsg = NULL;
 	char  szText2[201] = "";

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{
		// -- Put the our name on the front

	 	sprintf( szText2, "%s: %s", GetPlayerName(), pszMsg );


		// -- Make the message

		sSize = DoHeader( iMPT_CHAT, &pMsg );


		memcpy( ((PBRChat) pMsg)->szMsg, szText2, iChatMsgSize );

		//---- Send the message!

		StartWait( iMPM_SEND );


		//----- If first index is set then send to all

		if ( ChatSend[0] || force )
		{
			#ifdef _MULTIMONO
				mono_puts ( 40,17, "Broadcast Chat" );
			#endif

           ((PBRChat) pMsg)->messtype = iCHAT_BROADCAST;

		    fSent = SendBroadcastMessage( sSize, FALSE );

	    	ChatLineAdd ( &szText2[0], WHITE );

		}
		else
		{
			int i;

			#ifdef _MULTIMONO
				mono_puts ( 40,17, "Direct Chat" );
			#endif


           ((PBRChat) pMsg)->messtype = iCHAT_DIRECT;

			for ( i = 1; i < MAX_PLAYERS+1; ++i )
			{
			    DWORD dwToWhom;

				if ( ChatSend[i] )
				{

			    	dwToWhom = GetAPlayerId( i - 1 );

					//---- If there is actually a player here

			    	if ( dwToWhom != kNoPlayer )
			    	{
	           			fSent = SendDirectMessage( dwToWhom, sSize, FALSE );
					}
				}
			}

           //---- Put this in the chat list 

	    	ChatLineAdd ( &szText2[0], YELLOW );

       }


		EndWait( iMPM_SEND );


	}


	return ( fSent );

} // MultiPlayer::Chat



// -----------------------------------------------------------------
//
//	 MultiPlayer::Ping
//
//		 Sends a test message.
//
//     !! Requires ACK 
//     GEH Not anymore 7/7/97
//
//	 Inputs:
//		lData		- Data to ping with 
//     dwToWhom    - who do you want to send it to default = broadcast 
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::Ping( long lData, DWORD dwToWhom )
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{

		// -- Make the message

		sSize = DoHeader( iMPT_PING, &pMsg );

		((PBRPing) pMsg)->lPinger  = lData;


		//---- Send the message!

		StartWait( iMPM_SEND );


       //---- To broadcast or not to broadcast .... with no ACKing 

       if ( dwToWhom != kBroadcast )
       {
           fSent = SendDirectMessage( dwToWhom, sSize, FALSE );
       }
       else
       {
		    fSent = SendBroadcastMessage( sSize, FALSE );
       }

		EndWait( iMPM_SEND );
	}
	return ( fSent );

} // MultiPlayer::Ping



// -----------------------------------------------------------------
//
//	 MultiPlayer::Setup
//
//		 Sends game info after realms has been selected (master id)
//
//	 Inputs:
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::Setup( void )
{
	fSent		= FALSE;
	short sSize = 0;
	PCHAR pMsg  = NULL;

	int x;


	//---- Master only sends this message

	if ( !IsMaster() )
	{
		return TRUE;	
	}


	if ( eStatus == iMPS_SETUP )
	{

		#ifdef _MULTIMONO
	       mono_printf ( 40, 2, "B-> Master Id%d      ", GetPlayerId() );
		#endif

		// -- Make the message

		sSize = DoHeader( iMPT_SETUP, &pMsg );

		((PBRSetup) pMsg)->lMasterId  = GetPlayerId();


		//---- Setup our random table to be used by everyone 

		for ( x = 0; x < kNumSeeds; ++x )
		{
			((PBRSetup) pMsg)->ulRseeds[x] = GetTickCount () + rand();
		}

		memcpy ( &ulRseeds[0], &((PBRSetup) pMsg)->ulRseeds[0], 
				 sizeof( unsigned long ) * kNumSeeds );


	    ((PBRSetup)pMsg)->battle_speed      = battle_speed;
   	((PBRSetup)pMsg)->battle_difficulty = battle_difficulty;
	    ((PBRSetup)pMsg)->game_difficulty   = game_difficulty;
	    ((PBRSetup)pMsg)->iTTimeLimit       = iTTimeLimit;
       ((PBRSetup)pMsg)->realmAIver        = RealmAIVersionNumber;
       ((PBRSetup)pMsg)->DetailLevel        = fControlMode;

		//---- Signal we have the seed table 

		fHaveSeedTable = TRUE;


		//---- Send the message!

		StartWait( iMPM_SEND );

	    fSent = SendBroadcastMessage( sSize, TRUE );

		EndWait( iMPM_SEND );

	}

	return ( fSent );



} // MultiPlayer::Setup




// -----------------------------------------------------------------
//
//	 MultiPlayer::Battle
//
//     Send a Battle message / either broadcast or directed 
//
//     !! Requires ACK 
//
//	 Inputs:
//     lvalue1-5   - 5 longs used for battle info 
//     dwToWhom    - who do you want to send it to default = broadcast 
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::Battle( LONG lvalue1, LONG lvalue2, LONG lvalue3, 
                          LONG lvalue4, LONG lvalue5, DWORD dwToWhom )
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{
		#ifdef _MULTIMONO
       mono_puts ( 0, 5, "-> Battle Out" );
		#endif

		// -- Make the message

		sSize = DoHeader( iMPT_BATTLE, &pMsg );

		((PBRBattle) pMsg)->lvalue1 = iMPT_BATTLE;
		((PBRBattle) pMsg)->lvalue2 = lvalue1;
		((PBRBattle) pMsg)->lvalue3 = lvalue2;
		((PBRBattle) pMsg)->lvalue4 = lvalue3;
		((PBRBattle) pMsg)->lvalue5 = lvalue4;
		((PBRBattle) pMsg)->lvalue6 = lvalue5;


		//---- Send the message!

		StartWait( iMPM_SEND );


       //---- To broadcast or not to broadcast .... with ACKing 

       if ( dwToWhom != kBroadcast )
       {
           fSent = SendDirectMessage( dwToWhom, sSize, TRUE );
       }
       else
       {
		    fSent = SendBroadcastMessage( sSize, TRUE );
       }

		EndWait( iMPM_SEND );

	}



	return ( fSent );



} // MultiPlayer::Battle


// -----------------------------------------------------------------
//
//	 MultiPlayer::UnitMessage
//
//     Send a unit create / either broadcast or directed 
//
//     !! Requires ACK 
//
//	 Inputs:
//		unit index
//		unit data
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::UnitMessage( DWORD UnitIndex, UBYTE *UnitData, char *szWhy, DWORD dwToWhom )
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{
		// -- Make the message

		sSize = DoHeader( iMPT_UNIT, &pMsg );

		((PBRUnitMessage) pMsg)->UnitIndex = UnitIndex;
		memcpy((UBYTE *)(((PBRUnitMessage) pMsg)->UnitData), (UBYTE *)UnitData, SIZEOF_MULTIUNIT);

		//---- Send the message!

		StartWait( iMPM_SEND );


		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
			if (units[UnitIndex].Realm == REALM::NO_COUNTRY)
			{
				char temp[100];
				sprintf ( temp, "   mulplay.cpp UnitMessage - unit:%d is being deleted, [%s]", UnitIndex, szWhy);
				RandomLogComment ( temp );
			}
			else
			{
				char temp[100];
				sprintf ( temp, "   mulplay.cpp UnitMessage - %s unit:%d is being %s, province:%d DestProvince:%d",
					debug_realm_names[units[UnitIndex].Realm], UnitIndex, szWhy, units[UnitIndex].province, units[UnitIndex].DestProvince );
				RandomLogComment ( temp );
			}
		}


       //---- To broadcast or not to broadcast .... with ACKing 

       if ( dwToWhom != kBroadcast )
       {
           fSent = SendDirectMessage( dwToWhom, sSize, TRUE );
       }
       else
       {
		    fSent = SendBroadcastMessage( sSize, TRUE );
       }

		EndWait( iMPM_SEND );
	}

	return ( fSent );

} // MultiPlayer::UnitMessage


// -----------------------------------------------------------------
//
//	 MultiPlayer::PlacesMessage
//
//     Send a places create / either broadcast or directed 
//
//     !! Requires ACK 
//
//	 Inputs:
//		places index
//		places data
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::PlacesMessage( DWORD PlacesIndex, UBYTE *PlacesData, DWORD dwToWhom )
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{
		// -- Make the message

		sSize = DoHeader( iMPT_PLACES, &pMsg );

		((PBRPlacesMessage) pMsg)->PlacesIndex = PlacesIndex;
		memcpy((UBYTE *)(((PBRPlacesMessage) pMsg)->PlacesData), (UBYTE *)PlacesData, SIZEOF_MULTIPLACES);

		//---- Send the message!

		StartWait( iMPM_SEND );

       //---- To broadcast or not to broadcast .... with ACKing 

       if ( dwToWhom != kBroadcast )
       {
           fSent = SendDirectMessage( dwToWhom, sSize, TRUE );
       }
       else
       {
		    fSent = SendBroadcastMessage( sSize, TRUE );
       }

		EndWait( iMPM_SEND );
	}

	return ( fSent );

} // MultiPlayer::PlacesMessage


/* ========================================================================
   Function    - MultiPlayer::GainExpMessage
	Description - Send a GainExp message
	Returns     - results of send
	======================================================================== */
BOOL MultiPlayer::GainExpMessage( LONG RegentIndex, LONG Amount, BOOL GainLevel, DWORD  dwToWhom )
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{
		// -- Make the message

		sSize = DoHeader( iMPT_GAINEXP, &pMsg );

		((PBRGainExpMessage) pMsg)->RegentIndex = (LONG)RegentIndex;
		((PBRGainExpMessage) pMsg)->Amount = (LONG)Amount;
		((PBRGainExpMessage) pMsg)->GainLevel = (LONG)GainLevel;

		//---- Send the message!

		StartWait( iMPM_SEND );

       //---- To broadcast or not to broadcast .... with ACKing 

       if ( dwToWhom != kBroadcast )
       {
           fSent = SendDirectMessage( dwToWhom, sSize, TRUE );
       }
       else
       {
		    fSent = SendBroadcastMessage( sSize, TRUE );
       }

		EndWait( iMPM_SEND );
	}

	return ( fSent );

}


// -----------------------------------------------------------------
//
//	 MultiPlayer::AdminMessage
//
//     Send a admin create / either broadcast or directed 
//
//     !! Requires ACK 
//
//	 Inputs:
//		realm gold and regency data
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::AdminMessage( UBYTE *GoldData, UBYTE *RegencyData, char *szWhy, DWORD dwToWhom )
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{
		// -- Make the message

		sSize = DoHeader( iMPT_ADMIN, &pMsg );

		memcpy((UBYTE *)(((PBRAdminMessage) pMsg)->GoldData), (UBYTE *)GoldData, SIZEOF_MULTIGOLD);
		memcpy((UBYTE *)(((PBRAdminMessage) pMsg)->RegencyData), (UBYTE *)RegencyData, SIZEOF_MULTIGOLD);

		//---- Send the message!

		StartWait( iMPM_SEND );

       //---- To broadcast or not to broadcast .... with ACKing 

       if ( dwToWhom != kBroadcast )
       {
           fSent = SendDirectMessage( dwToWhom, sSize, TRUE );
       }
       else
       {
		    fSent = SendBroadcastMessage( sSize, TRUE );
       }

		EndWait( iMPM_SEND );
	}

	return ( fSent );

} // MultiPlayer::AdminMessage

// -----------------------------------------------------------------
//
//	 MultiPlayer::AlliedMessage
//
//     Send a admin create / either broadcast or directed 
//
//     !! Requires ACK 
//
//	 Inputs:
//		realm 
//		realm Allience list
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::AlliedMessage( DWORD Realm, UBYTE *Reaction, UBYTE *Status, char *szWhy, DWORD dwToWhom )
{
	short sSize = 0;
	PCHAR pMsg = NULL;

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{
		// -- Make the message

		sSize = DoHeader( iMPT_ALLIED, &pMsg );

		((PBRAlliedMessage) pMsg)->Realm = Realm;
		memcpy((UBYTE *)(((PBRAlliedMessage) pMsg)->Reaction), (UBYTE *)Reaction, SIZEOF_MULTIALLIED);
		memcpy((UBYTE *)(((PBRAlliedMessage) pMsg)->Status), (UBYTE *)Status, SIZEOF_MULTIALLIED);

		//---- Send the message!

		StartWait( iMPM_SEND );

       //---- To broadcast or not to broadcast .... with ACKing 

       if ( dwToWhom != kBroadcast )
       {
           fSent = SendDirectMessage( dwToWhom, sSize, TRUE );
       }
       else
       {
		    fSent = SendBroadcastMessage( sSize, TRUE );
       }

		EndWait( iMPM_SEND );
	}

	return ( fSent );

} // MultiPlayer::AlliedMessage


// -----------------------------------------------------------------
//
//	 MultiPlayer::Finalize
//		Close out the multi-player game.
//
//	 Inputs:
//		none
//
//	 Return: results of closing
//
// -----------------------------------------------------------------
BOOL MultiPlayer::Finalize( int type )
{
	int x;
   int iFlag = 0;
   BOOL fFoundMaster = FALSE;

	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		char temp[200];
		sprintf ( temp, "MultiPlayer::Finalize - player left the game");
		RandomLogComment ( temp );
	}

	if ( IsRunning() )
	{
		// -- Look for the master
		
		// -- If I'm not the master, see if he still exists
		if ( AMultiPlayer.GetPlayerId() != lMasterId )
		{
			for ( x=0; x < MAX_PLAYERS; ++x )
			{
				//---- Only tell everyone else if master is there 
				if ( AMultiPlayer.GetAPlayerId(x) == lMasterId )
				{
					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						char temp[200];
						sprintf ( temp, "MultiPlayer::Finalize - Found master ID %ld (I'm ID %ld)",
							AMultiPlayer.GetAPlayerId(x),
							AMultiPlayer.GetPlayerId() 
							);
						RandomLogComment ( temp );
					}

					fFoundMaster = TRUE;
					break;
				}
			}
		}
		else
			fFoundMaster = FALSE; // because I'm the master
		
		//---- If anyone is out there Tell everyone we left
		// if ( !fFoundMaster || AMultiPlayer.CheckForPlayers() == TRUE )
		{
			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
				char temp[200];
				sprintf ( temp, "MultiPlayer::Finalize - StopTheGame (I'm ID %ld)",
					AMultiPlayer.GetPlayerId() 
					);
				RandomLogComment ( temp );
			}
			
			
		   //---- Reset our status flag 
			eStatus = iMPS_ENDED;
		
			AMultiPlayer.StopTheGame();
			iFlag = 1;   // put up the MP over menu 
		}
	}

	#ifdef _MULTIMONO
   	mono_puts(0, 0, "Birthright - Finalizing a Multiplayer game" );
   #endif

   //---- Reset our status flag 
	eStatus = iMPS_ENDED;

	// -- Reset stuff

   InitGoop();

	// -- Update config file

 	WriteCfg();

   //---- Destroy the Direct Play stuff 

	GenericMultiPlayer::Finalize();

   //---- cleanup the chat stuff 

   ChatLineClear();

   //---- Dont remove memory on destructor

   if (!type)
	   HideChatDisplayWindow(0, 0);

   //---- Display the MP Over menu    

   if ( iFlag && !type )
   {
      ShowMPOver(0,0);
   }

	return ( eStatus == iMPS_ENDED );

}  //---- MultiPlayer::Finalize


 

// -----------------------------------------------------------------
//
//	 MultiPlayer::InitPre
//		Initialize the multi-player game pre menu.
//
//	 Inputs:
//
//	 Return: results of initialization
//
// -----------------------------------------------------------------
int MultiPlayer::InitPre ( int doSigs )
{
	BOOL fOK = TRUE;
   ErrCode err = kNoError;

	if ( (eStatus == iMPS_NONE) || 
		 (eStatus == iMPS_ENDED)   )
	{

       #ifdef _MULTIMONO

       //---- Initialize mono screen and write if applicable

       if( mono_getExist() )
       {
           mono_cls();
           mono_puts( 0, 0,"Initializing Multiplayer" );
       }

       #endif


	    // -- Read config file

   	ReadCfg();


		// -- Save the game type

		if ( doSigs )
		{

           //---- Put us in normal mode  ( well as normal as possible... )

			if (sDrawMode == iDDRAW)
			{
//				ToggleDirectDraw(0, 0);
//               DDSetCooperative ( FALSE ); 
//               ShowCursor(TRUE);
				WindowsMessages();
			}


			fSigs = TRUE;
			fOK = GenericMultiPlayer::Initialize( iMP_SIGS );
		}
		else
		{
			fSigs = FALSE;
			fOK = GenericMultiPlayer::Initialize( iMP_NETMODEM );


			if ( fOK )
			{
				for ( int i = 0; i < MAX_PROTOCOL; ++i )
					pszProtocol[i][0] = 0;

				AMultiPlayer.EnumerateProtocols( SaveString, (long) 0 );

				// -- Select a protocol, if we have before

				short sIdx = AMultiPlayer.GetProtocolIndex();

				if ( sIdx == -1 )
				{
					sIdx = 0;
				}
			}
		}



		if ( !fOK )
		{
			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
				char temp[200];
				sprintf ( temp, "MultiPlayer::InitPre - finalize");
				RandomLogComment ( temp );
			}

			GenericMultiPlayer::Finalize();   //---- Error
	 		return 0;
		}


 		return 1;

	}


	return 0;	



}	//---- End MultiPlayer::InitPre() 



// -----------------------------------------------------------------
//
//	 MultiPlayer::InitPost
//		Initialize the multi-player game post menu.
//
//	 Inputs:
//
//	 Return: results of initialization
//
// -----------------------------------------------------------------
void MultiPlayer::InitPost ( void )
{

	// -- Paranoia write

	WriteCfg();

  	// -- Are we live??


	SelectGuid( 0 );   //---- Always 0 for BRight

	eStatus = iMPS_INIT;


	//---- Do this before we increase the timeouts since it sets the Fudge

	SetMaster( fStartGame );


   //---- Don't allow anyone to join the game 

   DisablePlayers();

	//---- If a modem game, we need to bump the waits quite a bit

   if ( fSigs )
	{
      SetWaitTimes( 15 ) ; // 15 old 
	}
   else if ( GenericMultiPlayer::IsModemGame() ||   //---- Direct Modem
             !AMultiPlayer.GetSpeed()             ) //---- Low speed connection
	{
      SetWaitTimes( 3 );  // 8 old 
	}

	GetNetModule()->SetFileProgressCallback( FileProgress );

	eStatus = iMPS_SETUP;   


	for(int i=0; i<MPGAMESEMIPHORE_MAX_DATA; ++i)
	{
		lGameSemiphore[i] = -1;
	}



   ShowChatDisplayWindow(0, 0);

   //---- Help text for f3 and f4 

   ChatLineAdd( STRMGR_GetStr(STR_F3TEXT), WHITE );
   ChatLineAdd( STRMGR_GetStr(STR_F4TEXT), WHITE );



//	if ( IsMaster() )
//	{
//		Setup();      //---- Send Setup message to everyone not the master 
//	}
   //---- Now that we are connected go out and initialize the game
//   DomainNewGame ( 0, 7777 );




}	//---- MultiPlayer::InitPost()




// -----------------------------------------------------------------
//
//	 MultiPlayer::InitPost2
//		Initialize the multi-player game post 2.
//
//	 Inputs:
//
//	 Return: results of initialization
//
// -----------------------------------------------------------------
void MultiPlayer::InitPost2 ( void )
{


	if ( IsMaster() )
	{
		Setup();      //---- Send Setup message to everyone not the master
	}
	// -- delay everyone else to give the master a chance to get started
	else
	{
		Sleep(5000);	// -- GEH adjust this value???
		
	}

	//---- Lets wait and make sure everyone is with us at this point

	WaitSync ( iMPS_SYNC7 );


	//---- Are we still in a multiplayer game ?

	if ( IsMultiPlayer() )
	{
		dturn_mode = CHARSEL_MODE;

	    //---- Now that we are connected go out and initialize the game

		if ( AMultiPlayer.GetGameType() == iMPGT_NEW )
		{
			// -- if not already in a restarted game
			if (!gMPStartGame)
				DomainNewGame ( 0, 7777 );
			else
				MultiResetRealmSelect();
		}
		else if ( AMultiPlayer.GetGameType() == iMPGT_LOAD )
		{

			//---- Exchange the saved game
 
			if ( AMultiPlayer.ExchangeSavedGame() )
			{
				LoadGameDoMulti();			//---- load in the game 
				
				StartSync( iMPS_SYNC4 );	//---- synchronize everyone
				CheckSync( iMPS_SYNC4, TRUE, FALSE );
				
				NewGameConfirmed2(TRUE);	//---- Char select screen
			}
			else  //---- exchange failed
			{
				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					char temp[200];
					sprintf ( temp, "MultiPlayer::InitPost2 - finalize 1");
					RandomLogComment ( temp );
				}

				AMultiPlayer.Finalize();
			}

		}
		else  //---- Battle ???????? 
		{
			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
				char temp[200];
				sprintf ( temp, "MultiPlayer::InitPost2 - finalize 2");
				RandomLogComment ( temp );
			}

			AMultiPlayer.Finalize();
		}

	}


}	//---- MultiPlayer::InitPost2()



// -----------------------------------------------------------------
//
//	 MultiPlayer::SendGameType
//
//		 Sends game type to other players
//
//	 Inputs:
//
//	 Return: results of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::SendGameType( void )
{
	fSent		= FALSE;
	short sSize = 0;
	PCHAR pMsg  = NULL;

	int x;


	//---- Master only sends this message

	if ( !IsMaster() )
	{
		return TRUE;	
	}


	if ( eStatus == iMPS_SETUP )
	{

		#ifdef _MULTIMONO
	       mono_printf ( 40, 3, "B-> GameType %d      ", GetGameType() );
		#endif

		// -- Make the message

		sSize = DoHeader( iMPT_GAMETYPE, &pMsg );

		((PBRGameType) pMsg)->iGameType = GetGameType();


		//---- Send the message!

		StartWait( iMPM_SEND );

	    fSent = SendBroadcastMessage( sSize, TRUE );

		EndWait( iMPM_SEND );

	}

	return ( fSent );



} //---- MultiPlayer::SendGameType()



// -----------------------------------------------------------------
//
//	 MultiPlayer::UpdateDeletedPlayers()
//
//		Check for deleted players and doing things 
//
//	 Inputs:
//		none
//
//	 Return: 
//
// -----------------------------------------------------------------

void MultiPlayer::UpdateDeletedPlayers( void )
{
	int i;
	int x;

	//---- Check deleted player flags 

	for ( x = 0; x < MAX_PLAYERS; ++x )
	{

		//---- If we have found a deleted player 

		if ( AMultiPlayer.GetDeletedPlayer(x) )
		{

			//---- Find the disconnected realm

			for ( i = 0; i < LAND_REALM_COUNT; ++i )
			{

				//---- Is this a player 

				if ( lRealmSelectedByPlayer[i] != -1 )
				{

					//---- This is the one 

					if ( AMultiPlayer.GetDeletedPlayerId( x ) == lRealmSelectedByPlayer[i] )
					{

						#ifdef _MULTIMONO
							mono_printf ( 40,18, "Player Left id%d realm%d", 
										  lRealmSelectedByPlayer[i],
										  i );
						#endif

                           
                       //---- Display chat that this player has left

       				if ( i < LAND_REALM_COUNT )
       				{
		        			int temp;
                           char buffer[100]; 
				        	char cpRealmName[80];

       					strcpy(cpRealmName, realm[i].mfGetName() );

		            		temp = sprintf( buffer,
                   						"%s %s",
			 		            		    AMultiPlayer.GetOpponentName(x),
             							    STRMGR_GetStr(STR_MP_LEAVING1) );

       					temp = sprintf( &buffer[temp],					
		        				         	" %s %s",
       							        cpRealmName,
                                           STRMGR_GetStr(STR_MP_LEAVING2) );


   						ChatLineAdd( buffer, RED );

       				}




                       // If realms are not selected then dump game if
                       // master leaves otherwise the game should be OK

//                     if ( !MultiAllRealmsSelected() && 
//      					 AMultiPlayer.GetDeletedPlayerId (x) == lMasterId )
//	    				{

       				if ( AMultiPlayer.GetDeletedPlayerId (x) == lMasterId )
	    				{
		    				lRealmSelectedByPlayer[i] = -1;
			    			realm[i].mfSetPlayerCtrl(FALSE);  //---- Not player controlled

				    		AMultiPlayer.ResetDeletedPlayer ( x );


					    	Finalize();
						   	fRedrawAll = TRUE;
   					}
	    				else
		    			{
			    			lRealmSelectedByPlayer[i] = -1;
				    		realm[i].mfSetPlayerCtrl( FALSE);  //---- Not player controlled
						
					    	AMultiPlayer.ResetDeletedPlayer ( x );
                           
						}

					}

				}

			}

		}

	}



}	//---- End of UpdateDeletedPlayers()



// -----------------------------------------------------------------
//
//	 MultiPlayer::LookForMessages
//
//		Look for a misc message, and deal with them.
//
//	 Inputs:
//		none
//
//	 Return: TRUE if OK to go on, else FALSE
//
// -----------------------------------------------------------------
BOOL MultiPlayer::LookForMessages( int fMulPlay )
{
	static int i = 0;
	static long lTick = 0;

	int x;
	long lTemp;

	lTemp = GetTickCount();

	if ( lTick < lTemp )
	{

//		if ( fSigs )
//			lTick = lTemp + 300;
//		else
			lTick = lTemp + 84;    //---- A wierd number to mix it up


		// -- Look for misc. 'anytime' messages

		if ( IsInitialized() )
		{
			UpdateDeletedPlayers();

			//---- Is anyone out there ( if not finalize multiplayer )
			if ( CheckForPlayers() )
			{
				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					char temp[200];
					sprintf ( temp, "MultiPlayer::LookForMessages - finalize 1");
					RandomLogComment ( temp );
				}

			   if ( eStatus != iMPS_ENDED )
					Finalize();
				fRedrawAll = TRUE;
				return FALSE;
			}

			++i;

			#ifdef _MULTIMONO
			    mono_printf ( 30,24, "Look %d i %u # %lu last %lu", 
							  i, 
							  iOnSeed,
							  numrands,
							  lastrand  );
			#endif 


			//---- Do we need to broadcast realm ?? Kludge
			//---- don't do it if this is called from gmulplay otherwise thrash
			if ( IsMaster() && fMulPlay )
			{
				for ( x=0; x < MAX_PLAYERS; ++x )
				{
					if ( fBRealm[x] )
					{
						DWORD dwId;

						//----- Broadcast this realm now 
						dwId = AMultiPlayer.GetAPlayerId(x);
						AMultiPlayer.BroadcastRealmInfo( AMultiPlayer.GetRealmFromId( dwId ),
														 dwId,
                                                        lRegent[x] );

						lRegent[x] = 0;
						fBRealm[x] = 0;

 	 				}
				}
			}

			//---- Get them all
			while (  PeekForNextMessage( &nm ) )
			{
				UpdateDeletedPlayers();

				//---- Is anyone out there ( if not finalize multiplayer )
				if ( CheckForPlayers() )
				{
					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						char temp[200];
						sprintf ( temp, "MultiPlayer::LookForMessages - finalize 2");
						RandomLogComment ( temp );
					}

				   if ( eStatus != iMPS_ENDED )
						Finalize();
					return FALSE;
				}

				++i;

				#ifdef _MULTIMONO
				    mono_printf ( 30,24, "Look %d i %u # %lu last %lu", 
								  i, 
								  iOnSeed,
								  numrands,
								  lastrand  );
				#endif 

				switch ( nm.MsgId )
				{
					case iMPT_SYNC:
					case iMPT_ADVENTOVER:
					case iMPT_CONTROL:
					case iMPT_EVENT:
					case iMPT_REPEVENT:
					case iMPT_RESEVENT:
					case iMPT_REALM:
					case iMPT_BREALM:
					case iMPT_CHAT:
					case iMPT_PING:
					case iMPT_INDEXSEND:
					case iMPT_INDEXREQUEST:
					case iMPT_DATASEND:
					case iMPT_DATAREQUEST:
					case iMPT_BATTLE:
					case iMPT_UNIT:
					case iMPT_PLACES:
					case iMPT_GAINEXP:
					case iMPT_ADMIN:
					case iMPT_ALLIED:
					case iMPT_SETUP:
					case iMPT_WONGAME:
					case iMPT_PLAYERLEFTGAME:
					case iMPT_TARGETPLAYERLEFTGAME:
					case iMPT_SYNCACTION:
					case iMPT_GAMETYPE:

						LOGCOMMENTSLEEP
						if ( fLogComment )
						{
							if ( nm.MsgId == iMPT_SYNC )
							{
								char temp[200];
								sprintf ( temp, "MulPlay::LookForMessages - receiving SYNC message.  OurID:%ld  FromID:%ld  MsgNum:%lx", GetPlayerId(), nm.dwPlayerId, nm.ulSync);
								RandomLogComment ( temp );
							}

							if ( nm.MsgId == iMPT_UNIT )
							{
								char temp[200];
								sprintf ( temp, "MulPlay::LookForMessages - receiving UNIT message.  OurID:%ld  FromID:%ld  MsgNum:%lx", GetPlayerId(), nm.dwPlayerId, nm.ulSync);
								RandomLogComment ( temp );
							}
						}

						if ( ReceiveMessage( nm.MsgId, dwMsgWaitR, TRUE ) )
						{
							LOGCOMMENTSLEEP
							if ( fLogComment )
							{
								if ( nm.MsgId == iMPT_SYNC)
								{
									char temp[200];
										sprintf ( temp, "MulPlay::LookForMessages - callin HandleSpecialMessages with a SYNC message.  FromID:%ld/%ld  MsgNum:%lx",
											nm.dwPlayerId, ((PBRHeader)cRcvBuf)->dwFrom, nm.ulSync);
									RandomLogComment ( temp );
								}

								if ( nm.MsgId == iMPT_UNIT)
								{
									char temp[200];
										sprintf ( temp, "MulPlay::LookForMessages - callin HandleSpecialMessages with a UNIT message.  FromID:%ld/%ld  MsgNum:%lx",
											nm.dwPlayerId, ((PBRHeader)cRcvBuf)->dwFrom, nm.ulSync);
									RandomLogComment ( temp );
								}
							}

							HandleSpecialMessages( nm.MsgId, nm.ulSync );
						}
						else
						{
							LOGCOMMENTSLEEP
							if ( fLogComment )
							{
								char temp[200];
								sprintf ( temp, "MulPlay::LookForMessages - After Peeking Msg, ReceiveMessage %s Failed!",((nm.MsgId==iMPT_SYNC)?"SYNC":((nm.MsgId==iMPT_UNIT)?"UNIT":"non-unit & non-sync")) );
								RandomLogComment ( temp );
							}
						}

						break;

					default:
					{
						int PlayerInd;

						// -- check for an old message and, if found, toss it!
						PlayerInd = GenericMultiPlayer::GetAPlayerIndex ( nm.dwPlayerId );

						if ( PlayerInd >= MAX_PLAYERS )
						{
							if ( nm.ulSync <= ulRcvMsgNum[PlayerInd] )
								ReceiveMessage( (MsgType)nm.MsgId, 0, TRUE );
						}
						break;
					}

				}
				
				// breath...
				Sleep(0);
			}
			
			if ( kNoError != GetErrorNum() )
				HandleNetworkErrors( GetErrorNum() );
			
		}
	}

	return ( TRUE );

}  // MultiPlayer::LookForMessages

/* ========================================================================
   Function    - HandleNetworkErrors
	Description - deal with network error messages
	Returns     - 
	======================================================================== */
void HandleNetworkErrors ( PNET_ERRCODE_TYPE ErrorVal )
{
	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		char temp[200];
		sprintf ( temp, "MulPlay::HandleNetworkErrors - !!!!!!!! ErrorNum:%ld Reported !!!!!!!!", (long)ErrorVal);
		RandomLogComment ( temp );
	}
	
	
	switch ( ErrorVal )
	{
   // -- lost connection, game, player, or mind - punt
   case kConnectionLost:
   case kGameAborted:
   case kNoPlayersInGame:
   case kMessageNotReceived:
   	AMultiPlayer.Finalize();
   	break;
   
	case kPlayerDisconnected:
   case kNetworkingNotInstalled:
   case kNetworkingSetupError:
   case kGenericError:
   case kNoNetworkProtocolsAreAvailable:
   case kCannotCreateGame:
   case kCannotJoinGame:
   case kNoNetworkGamesAvailable:
   case kNoGameStarted:
   case kActionStructNotReady:
   case kIllegalPlayerID:
   case kNoProtocolSelected:
   case kCannotCreatePlayer:
   case kCannotGetGameInfo:
   case kMessageBufferOverflow:
   case kCheckSumError:
	case kExitForCodeUpdate:
	default:
		break;
	}

	SetErrorNum( kNoError );
}


// -----------------------------------------------------------------
//
//	 LookMessages
//
//		C version of above
//
//	 Inputs:
//		none
//
//	 Return: status 
//
// -----------------------------------------------------------------
BOOL LookMessages(void)
{
	 return AMultiPlayer.LookForMessages(1);
}


// -----------------------------------------------------------------
//
//	 MultiPlayer::StopTheGame
//
//		We are quitting the game, tell the other players
//
//	 Inputs:
//		none
//
//	 Return: status of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::StopTheGame( void )
{
	fSent = FALSE;

	if ( eStatus >= iMPS_ENDED )
	{
		short sSize = 0;
		PCHAR pMsg = NULL;


		// -- make the message

		sSize = DoHeader( iMPT_PLAYERLEFTGAME, &pMsg );

		fSent = SendBroadcastMessage( sSize, TRUE );


		#ifdef _MULTIMONO

		if ( fSent )
			mono_puts( 0, 15, "Player left game message" );

		#endif


	}


	return ( fSent );


} // MultiPlayer::StopTheGame

// -----------------------------------------------------------------
//
//	 MultiPlayer::RemovePlayer
//
//     tell the others that a fellow player has left the game
//
//	 Inputs:
//		none
//
//	 Return: status of send
//
// -----------------------------------------------------------------
BOOL MultiPlayer::RemovePlayer( DWORD dwId )
{
	fSent = FALSE;

	if ( eStatus >= iMPS_ENDED )
	{
		short sSize = 0;
		PCHAR pMsg = NULL;

		// tell myself
		GenericMultiPlayer::DeletePlayer( dwId );
		
		if (IsMultiPlayer())
		{
			// -- make the message
	
			sSize = DoHeader( iMPT_TARGETPLAYERLEFTGAME, &pMsg );
	
			((PBRLeftGame)pMsg)->dwId = dwId;
			
			fSent = SendBroadcastMessage( sSize, TRUE );
		}
	
		#ifdef _MULTIMONO
	
		if ( fSent )
			mono_puts( 0, 15, "Remove Player from game message" );
	
		#endif
	
	}
	
	return ( fSent );
	
} // MultiPlayer::StopTheGame





// ********************************************************
// ********** Private/Protected Member Functions **********
// ********************************************************

// -----------------------------------------------------------------
//
//	 MultiPlayer::DoHeader
//		Do the header and pointer stuff for a message.
//
//	 Inputs:
//		usType		- message type
//		ppMsg		- pointer to a PCHAR pointer of the message
//
//	 Return: size of the message
//
// -----------------------------------------------------------------
short MultiPlayer::DoHeader( MsgType usType, PCHAR *ppMsg )
{
	short sSize = 0;


   // -- Compute the size of the entire message

	sSize = sizeof(BRHeader);

	switch ( usType	)
	{
		case iMPT_WONGAME:
			sSize += sizeof(BRWonGame);
			break;

		case iMPT_SYNC:
			sSize += sizeof(BRSync);
			break;

		case iMPT_GAMETYPE:
			sSize += sizeof(BRGameType);
			break;

		case iMPT_SYNCACTION:
		case iMPT_ADVENTOVER:
			break;

		case iMPT_CONTROL:
			sSize += sizeof(BRControl);
			break;

		case iMPT_EVENT:
			sSize += sizeof(BREvent);
			break;

		case iMPT_REPEVENT:
			sSize += sizeof(BREventReply);
			break;

		case iMPT_RESEVENT:
			sSize += sizeof(BREventResults);
			break;

		case iMPT_BREALM:
		case iMPT_REALM:
			sSize += sizeof(BRRealm);
			break;

		case iMPT_CHAT:
			sSize += sizeof(BRChat);
			break;

		case iMPT_PING:
			sSize += sizeof(BRPing);
			break;

		case iMPT_INDEXSEND:
			sSize += sizeof(BRIndexS);
			break;

		case iMPT_INDEXREQUEST:
			sSize += sizeof(BRIndexS);
			break;

		case iMPT_DATAREQUEST:
			sSize += sizeof(BRIndexR);
			break;

		case iMPT_DATASEND:
			sSize += sizeof(BRIndexR);
			break;

		case iMPT_UNIT:
			sSize += sizeof(BRUnitMessage);
			break;

		case iMPT_PLACES:
			sSize += sizeof(BRPlacesMessage);
			break;

		case iMPT_GAINEXP:
			sSize += sizeof(BRGainExpMessage);
			break;

		case iMPT_ADMIN:
			sSize += sizeof(BRAdminMessage);
			break;

		case iMPT_ALLIED:
			sSize += sizeof(BRAlliedMessage);
			break;

		case iMPT_BATTLE:
			sSize += sizeof(BRBattle);
			break;

		case iMPT_PLAYERLEFTGAME:
          break;

		case iMPT_TARGETPLAYERLEFTGAME:
			sSize += sizeof(BRLeftGame);
          break;

		case iMPT_SETUP:
			sSize += sizeof(BRSetup);
          	break;



		default:
			#ifdef _MULTIMONO
				mono_puts( 40, 15, "Invalid switch in DoHeader()" );
			#endif

			break;


	} // end switch usType



	// --
	// -- Prepare the buffer
	// --

	// -- clear it all out first

	PCHAR	pMsg = *ppMsg;

	pMsg = &cSndBuf[0];	

	memset( pMsg, 0, MAX_NETBUFF );


	// -- Set header stuff

	((PBRHeader) pMsg)->usMsgType = usType;
	((PBRHeader) pMsg)->fMaster   = fMaster;

   //---- Our unique (DPLAY) player id 

   ((PBRHeader) pMsg)->dwFrom    = GetPlayerId();


   // -- return the pointer to the rest of the message

	(*ppMsg) = &cSndBuf[0];
	(*ppMsg) += sizeof(BRHeader);


	return ( sSize );


} // MultiPlayer::DoHeader




// -----------------------------------------------------------------
//
//	 MultiPlayer::HandleSpecialMessages
//		Handle special messages.
//
//	 Inputs:
//		ucMsgId		- message in the buffer
//
//	 Return: TRUE if OK to continue, else false
//
// -----------------------------------------------------------------
BOOL MultiPlayer::HandleSpecialMessages( UCHAR ucMsgId, long lMsgNum )
{
	PCHAR	pMsg = NULL;
	DWORD	playerId;
	int     i;


 	switch ( ucMsgId )
	{

		case iMPT_WONGAME:

			pMsg = &cRcvBuf[0] + sizeof(BRHeader);

			fWonGame = TRUE;  //---- Set this so we can't win also

           #ifdef _MULTIMONO
               mono_printf ( 40, 23, "<- Won Game %d", ((PBRWonGame)pMsg)->HomeRealm ); 
           #endif

			PlayFinale(((PBRWonGame)pMsg)->HomeRealm, 0 );

			break;


		case iMPT_ADVENTOVER:

			iAdventOver = TRUE;         //---- Flag for adventure over

           #ifdef _MULTIMONO
               mono_puts ( 40, 23, "<- Adventure Over" ); 
           #endif

			break;



		case iMPT_CONTROL:						// control

			pMsg = &cRcvBuf[0] + sizeof(BRHeader);

			SetControl ( ((PBRHeader)cRcvBuf)->dwFrom,
						 ((PBRControl)pMsg)->control   );


           #ifdef _MULTIMONO

               mono_printf ( 40, 8, "<- Player Control id %d flag %d", 
    						  ((PBRHeader)cRcvBuf)->dwFrom,
						 	  ((PBRControl)pMsg)->control   );
  
           #endif

			break;



		case iMPT_EVENT:						// event

			pMsg = &cRcvBuf[0] + sizeof(BRHeader);

			if ( EventTable.EventId == iMPE_NONE )
			{			
				int i;

				for ( i = 0; i < MAX_PLAYERS; ++i )
				{
					EventTable.replied[i] = 0;
				}

				EventTable.data1      = ((PBREvent)pMsg)->data1;
				EventTable.data2      = ((PBREvent)pMsg)->data2;
				EventTable.data3      = ((PBREvent)pMsg)->data3;
				EventTable.data4      = ((PBREvent)pMsg)->data4;
				EventTable.dwFrom     = ((PBRHeader)cRcvBuf)->dwFrom; //---- Set where event came from

				//---- Signal event has come in

				EventReply[0].EventId = ((PBREvent)pMsg)->EventId;


			}
			else  //---- We already have this event
			{   
				#ifdef _MULTIMONO
   	            mono_puts ( 40, 17, "Already have EVENT !!!!!!" ); 
           	#endif

			}

			break;



		case iMPT_SYNC: 						//---- sync messages
		{
			pMsg = &cRcvBuf[0] + sizeof(BRHeader);

			int i;

			for ( i = 0; i < MAX_PLAYERS; ++i )
			{
				if ( GetAPlayerId(i) == ((PBRHeader)cRcvBuf)->dwFrom )
				{
					//--- Check for and ignore rebroadcast sync messages
					if (lMsgNum == SyncMsgIDTable[((PBRSync)pMsg)->iSync][i])
					{
						LOGCOMMENTSLEEP
						if ( fLogComment )
						{
							char temp[200];
							sprintf ( temp, "MulPlay::HandleSpecialMessage - NOT recording DUPLICATE sync message.  OurID:%ld  FromID:%ld  SyncNum:%ld  MsgNum:%lx", GetPlayerId(), GetAPlayerId(i), ((PBRSync)pMsg)->iSync, lMsgNum);
							RandomLogComment ( temp );
						}
					}
					else
					{
						//---- recognize we are at this sync
	 					SyncTable[((PBRSync)pMsg)->iSync][i] = 1;

						//--- set the message ID for the message which set the sync
						SyncMsgIDTable[((PBRSync)pMsg)->iSync][i] = lMsgNum;
						LOGCOMMENTSLEEP
						if ( fLogComment )
						{
							char temp[200];
							sprintf ( temp, "MulPlay::HandleSpecialMessage - recording sync message.  OurID:%ld  FromID:%ld  SyncNum:%ld  MsgNum:%lx", GetPlayerId(), GetAPlayerId(i), ((PBRSync)pMsg)->iSync, lMsgNum);
							RandomLogComment ( temp );
						}
					}
					break;
				}

			}

			LOGCOMMENTSLEEP
			if ( fLogComment && i == MAX_PLAYERS)
			{
				char temp[200];
				sprintf ( temp, "MulPlay::HandleSpecialMessage - PlayerId %ld not found!", ((PBRHeader)cRcvBuf)->dwFrom );
				RandomLogComment ( temp );
			}

			break;
		}



		case iMPT_SYNCACTION: 		//---- Player is done 
		{
			pMsg = &cRcvBuf[0] + sizeof(BRHeader);

			int i;
			
			for ( i = 0; i < MAX_PLAYERS; ++i )
			{
				if ( GetAPlayerId(i) == ((PBRHeader)cRcvBuf)->dwFrom )
				{
					//---- Set this player is done
 
 					SyncAction[i] = 1;

					fRedrawAll = TRUE;
					break;
				}

			}

			break;
		}




		case iMPT_REPEVENT: 					// reply to event we generated

			pMsg = &cRcvBuf[0] + sizeof(BRHeader);

			
			if ( EventTable.EventId == iMPE_ADVENTURE ||
				 EventTable.EventId == iMPE_ADD_ITEM ||
				 EventTable.EventId == iMPE_DROP_ITEM )
			{
				int i;

				for ( i = 0; i < MAX_PLAYERS; ++i )
				{
					if ( GetAPlayerId(i) == ((PBRHeader)cRcvBuf)->dwFrom )
					{

						EventReply[i].data1   = ((PBREventReply)pMsg)->data1;
						EventReply[i].data2   = ((PBREventReply)pMsg)->data2;
						EventReply[i].data3   = ((PBREventReply)pMsg)->data3;
						EventReply[i].data4   = ((PBREventReply)pMsg)->data4;

						EventTable.replied[i] = 1;  //---- Set that reply has come in

					}
				}
			}
			else
			{
				EventReply[0].data1   = ((PBREventReply)pMsg)->data1;
				EventReply[0].data2   = ((PBREventReply)pMsg)->data2;
				EventReply[0].data3   = ((PBREventReply)pMsg)->data3;
				EventReply[0].data4   = ((PBREventReply)pMsg)->data4;

				EventTable.replied[0] = 1;  //---- Set that reply has come in


			}


			break;


			
		case iMPT_RESEVENT:    	// event results

			pMsg = &cRcvBuf[0] + sizeof(BRHeader);


			EventResults.data1   = ((PBREventResults)pMsg)->data1;
			EventResults.data2   = ((PBREventResults)pMsg)->data2;
			EventResults.data3   = ((PBREventResults)pMsg)->data3;
			EventResults.data4   = ((PBREventResults)pMsg)->data4;
			EventResults.EventId = ((PBREventResults)pMsg)->EventId;

			break;


		case iMPT_BREALM:

			//---- Master sent a realm select message for a player
			//---- Set the data

			#ifdef _MULTIMONO
				mono_puts ( 40,8, "BREALM RECEIVED" );
			#endif


			if ( AMultiPlayer.IsSlave() )
			{
				pMsg = &cRcvBuf[0] + sizeof(BRHeader);

				realm[((PBRRealm)pMsg)->iWhichRealm].mfSetPlayerCtrl(TRUE);
				lRealmSelectedByPlayer[((PBRRealm)pMsg)->iWhichRealm] = ((PBRRealm)pMsg)->lPlayerId;
               realm[((PBRRealm)pMsg)->iWhichRealm].mfSetRegent(((PBRRealm)pMsg)->lWhichRegent); // set the regent also

				#ifdef _MULTIMONO
					mono_printf ( 40,9, "BREALM rlm %d regent %d id %d",
								  ((PBRRealm)pMsg)->iWhichRealm,
                                 ((PBRRealm)pMsg)->lWhichRegent,  
								  ((PBRRealm)pMsg)->lPlayerId	 );
				#endif

				fRedrawAll = TRUE;
			}

			break;


		case iMPT_REALM:

           if ( AMultiPlayer.IsMaster() )
           {
				// -- Someone asked for a specific realm

				pMsg = &cRcvBuf[0] + sizeof(BRHeader);

				//---- if realm is available, set it to who asked, and tell everyone else

				if ( lRealmSelectedByPlayer[((PBRRealm)pMsg)->iWhichRealm] == -1 )
				{
					realm[((PBRRealm)pMsg)->iWhichRealm].mfSetPlayerCtrl(TRUE);	 //---- Player controlled realm

					lRealmSelectedByPlayer[((PBRRealm)pMsg)->iWhichRealm] = ((PBRRealm)pMsg)->lPlayerId;

                   realm[((PBRRealm)pMsg)->iWhichRealm].mfSetRegent(((PBRRealm)pMsg)->lWhichRegent); // set the regent also

					//---- Flag for broadcast later 

					for ( i=0; i < MAX_PLAYERS; ++i )
					{
						//---- Set flag for broadcast later 
				
						if ( ((PBRRealm)pMsg)->lPlayerId == AMultiPlayer.GetAPlayerId(i) )
						{
							fBRealm[i] = 1;
                           lRegent[i] = ((PBRRealm)pMsg)->lWhichRegent;
						}
					}


					//----- Set flag for broadcast later 
//					AMultiPlayer.BroadcastRealmInfo( ((PBRRealm)pMsg)->iWhichRealm, 
//													 ((PBRRealm)pMsg)->lPlayerId );

					fRedrawAll = TRUE;
				}

			}
			break;



		case iMPT_CHAT:
       {
           int color;

			// -- Retrieve our stuff

			pMsg = &cRcvBuf[0] + sizeof(BRHeader);
	

			int i;
			
			for ( i = 0; i < MAX_PLAYERS; ++i )
			{
				if ( GetAPlayerId(i) == ((PBRHeader)cRcvBuf)->dwFrom )
				{
					break;
				}
			}


           
           if ( !AMultiPlayer.GetChatRec(0) &&
                !AMultiPlayer.GetChatRec(i+1)    )
           {
           #ifdef _MULTIMONO
              if ( mono_getExist() )
               {
                   mono_printf ( 0, 14, "<- Chat %s Not Rec", ((PBRChat) pMsg)->szMsg );
               } 
           #endif
               break;
           }           


           //---- Broadcast WHITE

           if ( ((PBRChat) pMsg)->messtype == iCHAT_BROADCAST )
           {
               color = WHITE;
           }
           else    // direct yellow 
           {
               color = YELLOW;
           }

           


			ChatLineAdd( ((PBRChat) pMsg)->szMsg, color );


           #ifdef _MULTIMONO
              if ( mono_getExist() )
               {
                   mono_printf ( 0, 14, "<- Chat %s", ((PBRChat) pMsg)->szMsg );
               } 
           #endif

			break;
       }


		case iMPT_PING:

			// -- Retrieve our stuff
			pMsg = &cRcvBuf[0] + sizeof(BRHeader);
			
			// reset time since last ping
			for ( i = 0; i < MAX_PLAYERS; ++i )
			{
				DWORD dwId;
				dwId = AMultiPlayer.GetAPlayerId ( i );
				if ( dwId != kNoPlayer && dwId == ((PBRPing) pMsg)->lPinger )
				{
					AMultiPlayer.ResetSyncTime(i);
				}
			}
		
			break;



		case iMPT_INDEXSEND:

			//--- Master sent an index update 
			// -- Retrieve our stuff

			pMsg = &cRcvBuf[0] + sizeof(BRHeader);

			DoSetGameData( ((PBRIndexS)pMsg)->lArray_Id, 
							((PBRIndexS)pMsg)->lStruct_Id, 
							((PBRIndexS)pMsg)->lArray_Index, 
							((PBRIndexS)pMsg)->lData );

           #ifdef _MULTIMONO

              if ( mono_getExist() )
               {
                   mono_printf ( 40, 1, "<- Index %d      ",  ((PBRIndexS)pMsg)->lData );
               } 
   
           #endif

			break;



//!!!!! BAD DONT DO THIS OVERWRITES
#if 0
		case iMPT_INDEXREQUEST:

			//--- Someone requested data be set
			// -- Retrieve our stuff

			if ( IsMaster() )
			{
				// set the data, and tell everyone else
				pMsg = &cRcvBuf[0] + sizeof(BRHeader);


				DoSetGameData( ((PBRIndexS)pMsg)->lArray_Id, 
								((PBRIndexS)pMsg)->lStruct_Id,
								 ((PBRIndexS)pMsg)->lArray_Index, 
								  ((PBRIndexS)pMsg)->lData );

				BroadcastSetGameData( ((PBRIndexS)pMsg)->lArray_Id,
									   ((PBRIndexS)pMsg)->lStruct_Id,
									   ((PBRIndexS)pMsg)->lArray_Index, 
									   ((PBRIndexS)pMsg)->lData);

           	#ifdef _MULTIMONO
              		if ( mono_getExist() )
               	{
                   	mono_printf ( 0, 1, " Sending Index %d      ",  ((PBRIndexS)pMsg)->lData);
             		 } 
           	#endif


			}

			break;

		case iMPT_DATASEND:

			//--- Master sent an index update 
			// -- Retrieve our stuff

			pMsg = &cRcvBuf[0] + sizeof(BRHeader);

			iGameData = DoGetGameData( ((PBRIndexR)pMsg)->lArray_Id, 
										((PBRIndexR)pMsg)->lStruct_Id, 
										((PBRIndexR)pMsg)->lArray_Index);
			break;

#endif


#if 01
		case iMPT_DATAREQUEST:

			//--- Someone requested data be retrieved
			// -- Retrieve our stuff

			if ( IsMaster() )
			{
				playerId = ((PBRHeader)cRcvBuf)->dwFrom;

				pMsg = &cRcvBuf[0] + sizeof(BRHeader);

				iGameData = DoGetGameData( ((PBRIndexR)pMsg)->lArray_Id, 
											((PBRIndexR)pMsg)->lStruct_Id,
											 ((PBRIndexR)pMsg)->lArray_Index);

				BroadcastSetGameData( ((PBRIndexR)pMsg)->lArray_Id, 
									  ((PBRIndexR)pMsg)->lStruct_Id, 
									  ((PBRIndexR)pMsg)->lArray_Index,
									  iGameData, playerId );
			}
			break;

#endif


		case iMPT_BATTLE:
			// -- Retrieve our stuff
			pMsg = &cRcvBuf[0] + sizeof(BRHeader);
			//@@@@ Should be some protection from broadcast battle messages ?
			//---- Add to message que
			Add_MessQue( (PMESSQUE) pMsg );
			break;


		case iMPT_UNIT:	// update a unit on our machine from xmitted unit data
			{
				pMsg = &cRcvBuf[0] + sizeof(BRHeader);
				
				LONG index = (LONG)((PBRUnitMessage)pMsg)->UnitIndex;
				UNIT_TYPE *pData = (UNIT_TYPE *)((PBRUnitMessage)pMsg)->UnitData;
				
				// -- Can't memcopy, some data cannot be set to xmitted values
				// memcpy((UBYTE *)&units[index], pData , SIZEOF_MULTIUNIT); 
				
				
				LOGCOMMENTSLEEP
				if ( fLogComment )
				{
					if (pData->Realm == REALM::NO_COUNTRY && units[index].Realm != REALM::NO_COUNTRY)
					{
						char temp[100];
						sprintf ( temp, "   mulplay.cpp UnitReceive - %s unit:%d is being deleted",
							debug_realm_names[units[index].Realm], index);
						RandomLogComment ( temp );
					}
					else if (pData->Realm != REALM::NO_COUNTRY && units[index].Realm == REALM::NO_COUNTRY)
					{
						char temp[100];
						sprintf ( temp, "   mulplay.cpp UnitReceive - %s unit:%d is being created in %d",
							debug_realm_names[pData->Realm], index, pData->province );
						RandomLogComment ( temp );
					}
					else if (pData->province != NO_PROVINCE && units[index].province == NO_PROVINCE)
					{
						char temp[100];
						sprintf ( temp, "   mulplay.cpp UnitReceive - %s unit:%d is being made leader in %d",
							debug_realm_names[pData->Realm], index, pData->province );
						RandomLogComment ( temp );
					}
					else if (units[index].DestProvince != pData->DestProvince
							&& pData->Realm != NO_PROVINCE)
					{
						char temp[100];
						sprintf ( temp, "   mulplay.cpp UnitReceive - %s unit:%d is moving, DestProvince:%d",
							debug_realm_names[units[index].Realm], index, pData->DestProvince );
						RandomLogComment ( temp );
					}
					else
					{
						char temp[100];
						sprintf ( temp, "   mulplay.cpp UnitReceive - %s unit:%d info sent, don't know why. prov:%d",
							debug_realm_names[pData->Realm], index, pData->province );
						RandomLogComment ( temp );
					}
				}
				
				// new units start as NotSpotted = TRUE
				if (pData->Realm != NO_PROVINCE && units[index].province == NO_PROVINCE)
					units[index].NotSpotted = TRUE;
				
				// -- update the unit
				units[index].province		= pData->province;
				units[index].Realm			= pData->Realm;
				units[index].Salary			= pData->Salary;
				units[index].Icon				= pData->Icon;
				units[index].Moving			= pData->Moving;
				units[index].AvoidMove		= pData->AvoidMove;
				units[index].Joined			= pData->Joined;
				units[index].Disguised		= pData->Disguised;
				units[index].Hastened		= pData->Hastened;
				units[index].Blessed			= pData->Blessed;
				units[index].Paid				= pData->Paid;
				units[index].Use_Regency	= pData->Use_Regency;
				units[index].MovePart		= pData->MovePart;
				units[index].DestProvince	= pData->DestProvince;
				units[index].DestPath1		= pData->DestPath1;
				units[index].DestPath2		= pData->DestPath2;
				units[index].DestPath3		= pData->DestPath3;
				units[index].id				= pData->id;
				units[index].NextUnit		= pData->NextUnit;
				
				
				// if we are deleting the unit, don't do the following
				if (units[index].province != NO_PROVINCE)
				{
					//-- Always reattach the icon art to the unit each time.
					//   (No reference counting is used by the resmanager.)
					//   Can't use xmitted piece of data as handles are not the same
					//   across machines.
					{
						char n[80];
						sprintf(n,"UI\\%s.PCX",GameIcons[pData->Icon]);
						units[index].iIconBitm = GetResourceStd(n, FALSE);
					}
					
					//-- Can't use the xmitted NotSpotted flag, different on each machine
					// units[index].NotSpotted		= pData->NotSpotted;
					
					//-- Check whether anyone is already in this province and can see
					//   the new unit.
					if (CanISeeAnyoneInThisProv(HomeRealm, (PROVINCE)pData->province))
					{
						units[index].NotSpotted = FALSE;
					}
					
					//-- We spot any anybody in the prov if a vassal or liege lord's
					//   unit is in the prov.
					if (ISVASSAL(pData->Realm,HomeRealm)
					    || ISVASSAL(HomeRealm, pData->Realm))
					{
						MarkUnitsSpotted((PROVINCE)pData->province);
					}
					
					//-- If we join a group which is spotted, then so are we.
					if (units[index].Joined == TRUE)
					{
						units[index].NotSpotted = units[GetUnitLeader(index)].NotSpotted;
					}
					else
					{
						LONG const iNextUnit = units[index].NextUnit;
						if (iNextUnit > -1)
						{
							// We have followers.
							LONG const SpottedFlag = (units[index].NotSpotted && units[iNextUnit].NotSpotted);
							LONG k;
							for (k = index; k > -1; k = units[k].NextUnit)
							{
								units[k].NotSpotted = SpottedFlag;
							}
						}
					}
				
				}	// if not deleting
				
				SetRedrawMainMapLevel();
			}
			break;


		case iMPT_PLACES:
			// -- Retrieve our stuff
			{
			pMsg = &cRcvBuf[0] + sizeof(BRHeader);
			
			LONG index = (LONG)((PBRPlacesMessage)pMsg)->PlacesIndex;
			UBYTE *pData = (UBYTE *)((PBRPlacesMessage)pMsg)->PlacesData;
			char n[80];
			
			memcpy((UBYTE *)&places[index], pData , SIZEOF_MULTIPLACES); 
			
			LONG lData;
			// Get the correct map resource icon.
			switch (places[index].Icon)
			{
			case CASTLE2_ICON:
				lData = SPECIAL1_ICON;
				break;
			case CASTLE3_ICON:
				lData = SPECIAL2_ICON;
				break;
			case CASTLE4_ICON:
				lData = SPECIAL3_ICON;
				break;
			case VILLAGE_ICON:
			case SEAPORT_ICON:
			case SEAPORT2_ICON:
			case PORT_ICON:
			case WIZTOWER_ICON:
			case RUINS_ICON:
				lData = places[index].Icon;
				break;
			default:
				lData = places[index].Icon+(((places[index].Level<8)?places[index].Level:7)/2);
				break;
			}
			sprintf(n,"UI\\%s.PCX",GameIcons[lData]);
			places[index].iIconBitm = GetResourceStd(n, FALSE);

			}
			break;
		
 		case iMPT_GAINEXP:
 			{
			pMsg = &cRcvBuf[0] + sizeof(BRHeader);
			GainRealmExp(
				(LONG)((PBRGainExpMessage) pMsg)->RegentIndex,
				(LONG)((PBRGainExpMessage) pMsg)->Amount,
				(BOOL)((PBRGainExpMessage) pMsg)->GainLevel,
				FALSE);						// fsend
	 		}
	 		break;
	 		
		case iMPT_ADMIN:
			// -- Retrieve our stuff
			{
			pMsg = &cRcvBuf[0] + sizeof(BRHeader);
			
			// -- get gold pointer
			UBYTE *pGoldData = (UBYTE *)((PBRAdminMessage)pMsg)->GoldData;
			
			// -- get gold regency
			UBYTE *pRegencyData = (UBYTE *)((PBRAdminMessage)pMsg)->RegencyData;
			
			// -- decode data array
			
			}
			break;

		case iMPT_ALLIED:
			// -- Retrieve our stuff
			{
			pMsg = &cRcvBuf[0] + sizeof(BRHeader);
			
			// -- get data pointer
			LONG index = (LONG)((PBRAlliedMessage)pMsg)->Realm;
			UBYTE *pReaction = (UBYTE *)((PBRAlliedMessage)pMsg)->Reaction;
			UBYTE *pStatus = (UBYTE *)((PBRAlliedMessage)pMsg)->Status;
			
			// -- decode data array
			memcpy((UBYTE *)&RealmReaction[index][0], pReaction , SIZEOF_MULTIALLIED); 
			memcpy((UBYTE *)&RealmStatus[index][0], pStatus , SIZEOF_MULTIALLIED); 
			
			}
			break;

		case iMPT_PLAYERLEFTGAME:

			#ifdef _MULTIMONO
			mono_puts ( 40, 1, "PLAYER LEFT GAME!!!!!!!" );
			#endif

			GenericMultiPlayer::DeletePlayer  ( ((PBRHeader)cRcvBuf)->dwFrom );

			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
				char temp[100];
				sprintf ( temp, "Player ID:%d, has left the game.", ((PBRHeader)cRcvBuf)->dwFrom );
				RandomLogComment ( temp );
			}


			//----- Refresh just in case gamemap is running 

			fRedrawAll = TRUE;

			break;

		case iMPT_TARGETPLAYERLEFTGAME:

			#ifdef _MULTIMONO
			mono_puts ( 40, 1, "PLAYER LEFT GAME!!!!!!!" );
			#endif

			pMsg = &cRcvBuf[0] + sizeof(BRHeader);

			GenericMultiPlayer::DeletePlayer( ((PBRLeftGame)pMsg)->dwId );

			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
				char temp[100];
				sprintf ( temp, "Player ID:%d, was told to leave the game.", ((PBRLeftGame)pMsg)->dwId );
				RandomLogComment ( temp );
			}


			//----- Refresh just in case gamemap is running 

			fRedrawAll = TRUE;

			break;


		case iMPT_GAMETYPE:

			pMsg      = &cRcvBuf[0] + sizeof(BRHeader);

			//---- Set Game Type from master 

			SetGameType(((PBRGameType)pMsg)->iGameType);


           #ifdef _MULTIMONO
	           mono_printf ( 40, 1, "<- Game Type %d      ",  GetGameType());
           #endif


			break;



		case iMPT_SETUP:

			pMsg      = &cRcvBuf[0] + sizeof(BRHeader);

			lMasterId = ((PBRSetup)pMsg)->lMasterId;


			memcpy ( &ulRseeds[0], &((PBRSetup) pMsg)->ulRseeds[0], 
					 sizeof( unsigned long ) * kNumSeeds );

			iOnSeed = 0;

	    	battle_speed      = (SPEED)((PBRSetup)pMsg)->battle_speed;
		   	battle_difficulty = (DIFFICULTY)((PBRSetup)pMsg)->battle_difficulty;
	    	game_difficulty   = (DIFFICULTY)((PBRSetup)pMsg)->game_difficulty;
			fControlMode = ((PBRSetup)pMsg)->DetailLevel;

	    	iTTimeLimit   = ((PBRSetup)pMsg)->iTTimeLimit;
           MasterRealmAIVersionNumber = ((PBRSetup)pMsg)->realmAIver;
		    SpeedLevel      = 3 - battle_speed;      /* {3,2,1} from {0,1,2} */
	   		DifficultyLevel = 3 - battle_difficulty; /*        ditto         */
   		gWinningPoints  = 300 + (LONG) game_difficulty * 100;



			//---- Signal we have the seed table 

			fHaveSeedTable = TRUE;


           #ifdef _MULTIMONO
	           mono_printf ( 40, 1, "<- Master id %d      ",  lMasterId );
           #endif


			break;


		default:

			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
				char temp[200];
				sprintf ( temp, "MulPlay::HandleSpecialMessage - Unknown message usType:%ld", ucMsgId );
				RandomLogComment ( temp );
			}
			
			
           #ifdef _MULTIMONO

              if ( mono_getExist() )
               {
                   mono_puts ( 0, 15, "HandleSpecial Unknown message" );
               } 
   
           #endif
			break;

	}


	return ( TRUE );



} // MultiPlayer::HandleSpecialMessages



// -----------------------------------------------------------------
//
//	 MultiPlayer::InitGoop
//
//		Initialize various multiplayer game parameters.
//
//	 Inputs:
//		none
//
//	 Return: none
//
// -----------------------------------------------------------------
void MultiPlayer::InitGoop( void )
{
	int i,j;

	fMaster		= FALSE;
	dwMsgFudge	= 0;


   Init_MessQue ( );      //---- Initialize message que ( clear it )


	//---- Init our chat sending list 

	InitChat();


	//---- Init the event table 

	InitEvent();


	//---- initialize the realm player stuff

	for(i=0; i<LAND_REALM_COUNT; ++i)
	{
		lRealmSelectedByPlayer[i] = -1;
	}

	for(i=0; i<MAX_PLAYERS; ++i)
	{
		fBRealm[i] = 0;
		lRegent[i] = 0;
	}

	
	for(i=0; i<REALM::REALM_COUNT; ++i)
	{

		//---- Reset player control versus non-player controlled

		if ( i != HomeRealm )
		{
			realm[i].mfSetPlayerCtrl(FALSE);	 //---- Player controlled realm
		}

	} 



	//---- Yes there are better ways  Reset sync table 

	for ( i=0; i < iMPS_SYNCEND; ++i )
	{
		for ( j= 0; j < MAX_PLAYERS; ++j )
		{
			SyncTable[i][j] = 0;
			SyncMsgIDTable[i][j] = -99;
		}
	}


	//---- Reset Sync Action table 

	for ( i=0; i < MAX_PLAYERS; ++i )
	{
		SyncAction[i] = 0;
	}


	//---- Reset master id 

	lMasterId = -1;

	//---- Reset game type

	SetGameType ( iMPGT_NONE );

	// -- clear lowlevel error code
	ClearErrorNum();

} // MultiPlayer::InitGoop



// -----------------------------------------------------------------
//
//	 MultiPlayer::ReadCfg
//		Read the configuration file.
//
//	 Inputs:
//		none
//
//	 Return: none
//
// -----------------------------------------------------------------
void MultiPlayer::ReadCfg( void )
{

	CHAR szFile[MAX_PATH] = "";
	short temp;

	sprintf( szFile, "%s", pszCfg );

	std::ifstream ifile( szFile, std::ios::in );

	if ( !ifile.fail() )
	{
		ifile.read( &szName[0], sizeof(szName) );
		ifile.read( &szGame[0], sizeof(szGame) );

		if ( ifile.eof() )
		{
			sProtocol = -1;
		}
		else
		{
			ifile >> sProtocol;
		}

		if ( ifile.eof() )
		{
			sConnSpeed = 0; 	 // default modem
		}
		else
		{
			ifile >> sConnSpeed;
		}

		if ( ifile.eof() )
		{
          sNumPlayers = 8;
       }
       else
       {
			ifile >> sNumPlayers;
       }

	}
	else
	{

		strcpy( szName, STRMGR_GetStr(STR_MP_DEFAULT_NAME) );
		strcpy( szGame, STRMGR_GetStr(STR_MP_DEFAULT_GAME) );
		sProtocol		= 0;
		sConnSpeed   	= 0;
		sNumPlayers 	= 8;

	}

	ifile.close();

} // MultiPlayer::ReadCfg



// -----------------------------------------------------------------
//
//	 MultiPlayer::ReceiveFile
//		Receive the specified file.
//
//	 Inputs:
//		pNew		- new file name of received file
//
//	 Return: TRUE if a file was received, else FALSE
//
// -----------------------------------------------------------------
BOOL MultiPlayer::ReceiveFile( PSZ pNew )
{
	BOOL fRes = FALSE;

	// -- Check for a dead object

	if ( !IsInitialized() )
		return ( FALSE );

	// -- Make sure we are in the default directory

	StartWait( iMPM_RCVFILE );

	// -- We must wait our turn to receive a file transfer
	StartSync( iMPS_SYNC5 );	//---- synchronize everyone
	CheckSync( iMPS_SYNC5, TRUE, FALSE );
	
	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
	    char temp[200];
	    sprintf ( temp, "Start Receive File (lMasterId = %d)", lMasterId );
	    RandomLogComment(temp);
	}


	AddSndObj( SND_BATTLE_HORN1, 0, VOLUME_EIGHTY);
	
	fRes = GenericMultiPlayer::ReceiveFile( lMasterId, pNew );

	EndWait( iMPM_RCVFILE );

	LOGCOMMENTSLEEP
	if ( fLogComment )
		RandomLogComment ( "End Receive File" );

	return ( fRes );


} // MultiPlayer::ReceiveFile



// -----------------------------------------------------------------
//
//	 MultiPlayer::ReceiveMessage
//		Receive the specified message.
//
//	 Inputs:
//		usType		- message to receive
//		dwWait	    - how long to wait
//		fOneOnly	- if TRUE, wait for only one message, even if it
//					  is out of synch
//
//	 Return: none
//
// -----------------------------------------------------------------
BOOL MultiPlayer::ReceiveMessage( USHORT usType, DWORD dwWait, BOOL fOneOnly )
{
	// -- Check for a dead object

	if ( !IsInitialized() )
		return ( FALSE );

	return( GenericMultiPlayer::ReceiveMessage( usType, dwWait, fOneOnly ) );

} // MultiPlayer::ReceiveMessage



// -----------------------------------------------------------------
//
//	 MultiPlayer::SendFile
//		Send the designated file.
//
//	 Inputs:
//		pszPath		- path of file to send (NULL for run directory)
//		pszFile		- file to send
//
//	 Return: none
//
// -----------------------------------------------------------------
BOOL MultiPlayer::SendFile( PSZ pszPath, PCHAR pszFile )
{

	CHAR szFName[MAX_PATH] = "";
	unsigned int p_index;
	DWORD idNext;
	LONG	i,j,c;


	// -- Check for a dead object

	if ( !IsInitialized() )
		return ( FALSE );

	fSent = FALSE;
	
	// -- Make a fully-qualified file name

// for now
//	if ( pszPath != NULL )
//		sprintf( szFName, "%s\\%s", pszPath, pszFile );
//	else
//		sprintf( szFName, "%s%s", szRunDir, pszFile );

// for now

	sprintf( szFName, ".\\%s", pszFile );

	// -- Send it!

	// -- Hourglass wait cursor
	StartWait( iMPM_SENDFILE );
	// -- sync all of us to same point
	StartSync( iMPS_SYNC5 );
	
	// -- clear temp sync arrays
	pnmGame.ResetSync(iMPS_SYNC4);
	
	// -- Loop unconditionally. Break when we have received all syncs
	while ( TRUE )
	{
		LONG	cnt,i;
		
		AMultiPlayer.LookForMessages();	//---- Check for messages

		//---- Check if all syncs have come in
		for ( cnt = i = 0; i < MAX_PLAYERS; ++i )
		{
			DWORD dwId;
			dwId = AMultiPlayer.GetAPlayerId ( i );
			if ( dwId != kNoPlayer && dwId != AMultiPlayer.GetPlayerId())
			{
				// -- are they at this sync point
				if ( !AMultiPlayer.GetSync( iMPS_SYNC5, i ) )
				{
					++cnt;
				}
			}
		}
		
		if ( cnt == 0 )	//---- Everyone has synced
		{
			AMultiPlayer.ResetSync( iMPS_SYNC4 );
			break;
		}
		
		if ( !IsMultiPlayer() )		//---- The game is over somehow
			break;

		Sleep(0);
	}
	
	
	// Set a fake sync 4 for everyone but the last one
	for (p_index=0; p_index < MAX_PLAYERS; ++p_index)
	{
		if ((idNext = GetAPlayerId(p_index)) != kNoPlayer &&
			idNext != AMultiPlayer.GetPlayerId())
		{
			SyncTable[iMPS_SYNC4][p_index] = 1;
			j = p_index;
		}
	}
	SyncTable[iMPS_SYNC4][j] = 0;
	
	
	for (p_index=0; p_index < MAX_PLAYERS; ++p_index)
	{
		sprintf( szFName, ".\\%s", pszFile );
		
		// -- Now, for each possible player, release each one, one
		// -- at a time, and send them the file.
		if ((idNext = GetAPlayerId(p_index)) != kNoPlayer &&
			idNext != AMultiPlayer.GetPlayerId())
		{
			// -- Why 10, you ask?
			i = 10;
			c = 0;
			
			AMultiPlayer.SendSyncTo( iMPS_SYNC_GO, idNext );
			
			// -- if we didn't time/loop out, then send the file
			fSent = (i>0) && GenericMultiPlayer::SendFile( szFName, p_index );
			
			// forget this one and try the next
			if (!fSent)
			{
				RemovePlayer( idNext );

				if ( fLogComment )
				{
					char temp[200];
					sprintf ( temp, "MultiPlayer::SendFile - used to remove player. idNext: %ld", idNext);
					RandomLogComment ( temp );
				}

				continue;
			}
			
			// ABC remove 7/21/97
			// -- take care of any SYNC4's that occured during file xfer
			//for ( j = 0; j < MAX_PLAYERS; ++j )
			//{
			//	if( pnmGame.GetSync( iMPS_SYNC4, j ))
			//	{
			//		//---- recognize we are at this sync
		 	//		SyncTable[iMPS_SYNC4][j] = 1;
			//		//--- set the message ID for the message which set the sync
			//		SyncMsgIDTable[iMPS_SYNC4][j] = pnmGame.GetSyncId(iMPS_SYNC4, j);
			//	}
			//}
			
			//GEH 7/18/97  keep sending
			//GEH if (!fSent)
			//GEH	break;	// -- We're hosed if anyone fails
		}

		// wait until we get a sync 4 from this player
		//while ( !pnmGame.GetSync( iMPS_SYNC4, p_index ) )
		//	AMultiPlayer.LookForMessages();

		run_timers();
		Sleep(0);
	}
	
	// -- clear temp sync arrays
	//pnmGame.ResetSync(iMPS_SYNC4);
	
	// -- end wait
	EndWait( iMPM_SENDFILE );

	return ( fSent );


} // MultiPlayer::SendFile

// -----------------------------------------------------------------
//
//	 MultiPlayer::SendBroadcastMessage
//
//		Send the message.    This is a broadcast !!!!!!!!!!!!!!!
//
//	 Inputs:
//		sSize		- actual size of the message
//		fAck		- do we want acking?
//
//	 Return: none
//
// -----------------------------------------------------------------
BOOL MultiPlayer::SendBroadcastMessage( short sSize, BOOL fAck )
{

	BOOL	RetVal;
	
	// -- Check for a valid object

	if ( !IsInitialized() )
		return ( FALSE );

//@@@@@@@@@@@@@@@@@@@@@@@@@
	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		char temp[200];
		sprintf ( temp, "MultiPlayer::SendBroadcastMessage - usMsgType:%ld %s%s",
			 (LONG)(((PBRHeader)&cSndBuf[0])->usMsgType), (((PBRHeader)&cSndBuf[0])->usMsgType==iMPT_UNIT)?"iMPT_UNIT":" ", (((PBRHeader)&cSndBuf[0])->usMsgType==iMPT_PING)?"iMPT_PING":" " );
		//RandomLogComment ( temp );
	}


	RetVal = ( GenericMultiPlayer::SendBroadcastMessage(
		   (( (PBRHeader) &cSndBuf[0])->usMsgType), 
		    						sSize, fAck ) );

	// -- clean up
	if ( kNoError != GetErrorNum() )
		HandleNetworkErrors( GetErrorNum() );
					
	return RetVal;
	
} // MultiPlayer::SendBroadcastMessage()



// -----------------------------------------------------------------
//
//	 MultiPlayer::SendDirectMessage
//
//		Send the message.   
//
//	 Inputs:
//     dwToWhom    - Id of person to whom the message is going 
//		sSize		- actual size of the message
//		fAck		- do we want acking?
//
//	 Return: none
//
// -----------------------------------------------------------------
BOOL MultiPlayer::SendDirectMessage( DWORD dwToWhom, short sSize, BOOL fAck )
{
	BOOL	RetVal;

	// -- Check for a valid object

	if ( !IsInitialized() )
		return ( FALSE );


	RetVal = ( GenericMultiPlayer::SendDirectMessage(
           dwToWhom,
		   (( (PBRHeader) &cSndBuf[0])->usMsgType), 
		    						sSize, fAck ) );

	// -- clean up
	if ( kNoError != GetErrorNum() )
		HandleNetworkErrors( GetErrorNum() );

	return RetVal;
	
} // MultiPlayer::SendDirectMessage()



// -----------------------------------------------------------------
//
//	 MultiPlayer::WriteCfg
//		Write the configuration file.
//
//	 Inputs:
//		none
//
//	 Return: none
//
// -----------------------------------------------------------------
void MultiPlayer::WriteCfg( void )
{


	CHAR szFile[MAX_PATH] = "";
	short temp;
	sprintf( szFile, "%s", pszCfg );

	std::ofstream ofile( szFile, std::ios::out );

	if ( !ofile.fail() )
	{
		ofile.write( &szName[0], sizeof(szName) );
		ofile.write( &szGame[0], sizeof(szGame) );

		 // -- Write spaces after so read works!

		ofile << sProtocol << " ";
		ofile << sConnSpeed << " ";
		ofile << sNumPlayers << " ";
	}

	ofile.close();


} // MultiPlayer::WriteCfg






// ----------------------------------------------------------------------
//
//   MultiPlayer::StartNewGame ( )
//
//   Start a new direct play game                                                                
//                                                                              
//   Returns: 1 - worked 0 - failure                                                                  
//                                                                             
// ----------------------------------------------------------------------

BOOL MultiPlayer::StartNewGame ( void )
{
	BOOL fOK;
	PNETGAMEINFO pngiInfo = NULL;


	fStartGame = TRUE;

	// -- Setup the game description

	pngiInfo = AMultiPlayer.GetGameInfo();

	strcpy( pngiInfo->szGameName, AMultiPlayer.GetGameName() );

	pngiInfo->usMaxPlayers = AMultiPlayer.GetNumPlayers();


	// -- Create the game

	fOK = AMultiPlayer.CreateAGame( AMultiPlayer.GetPlayerName() );


	return fOK;


}




// ----------------------------------------------------------------------
//
//   MultiPlayer::JoinGame
//
//       Join a direct play game 
//
//   Arguments: index of game                                                                
//                                                                              
//   Returns: 1 - worked 0 - failure                                                                  
//                                                                             
// ----------------------------------------------------------------------

BOOL MultiPlayer::JoinGame ( int index )
{
	BOOL fOK;


	fStartGame = FALSE;


	fOK = AMultiPlayer.JoinAGame( index, 
								  AMultiPlayer.GetPlayerName() );


	PNETGAMEINFO pngiInfo = NULL;


	pngiInfo = AMultiPlayer.GetGameInfo();

	AMultiPlayer.SetNumPlayers( pngiInfo->usMaxPlayers );



	return fOK;


}



// ----------------------------------------------------------------------
//
//   MultiPlayer::GetPlayers()
//
//       Update all the players names
//
//   Arguments:
//                                                                              
//   Returns:
//                                                                             
// ----------------------------------------------------------------------

BOOL MultiPlayer::GetPlayers ( void )
{

	// -- update the other player's names

	AMultiPlayer.EnumeratePlayers( DisplayString, 0 );

	return 1;

}



// ----------------------------------------------------------------------
//
//   MultiPlayer::WaitForPlayers()
//
//       
//
//   Arguments:
//                                                                              
//   Returns: kHaveAll is done waiting else number of players 
//                                                                             
// ----------------------------------------------------------------------

int MultiPlayer::WaitForPlayers ( void )
{
	return( GenericMultiPlayer::WaitForPlayers() );
}


// ----------------------------------------------------------------------
//
//   MultiPlayer::DisplayNetStatus()
//
//   Display network status     
//
//   Arguments:
//                                                                              
//   Returns:  
//                                                                             
// ----------------------------------------------------------------------

void MultiPlayer::DisplayNetStatus ( LONG mx, LONG my )
{
	int x;
	int count = 0;
	char buffer[10];

	if ( IsMultiPlayer() )
	{

		for ( x=0; x < MAX_PLAYERS; ++x )
		{
			if ( GetAPlayerId(x) != kNoPlayer )
			{
				++count;
			}
		}


		if ( count )
		{

			init_gfont(FONT_SANS_8PT);

			sprintf ( buffer, "%c %d", 'M', count+1 );

			gprint_text( mx+7, my+30, buffer, RED );
		}
	}

}	



// -----------------------------------------------------------------
//
//	 MultiPlayer::StartSeed
//
//		Start random seed ( use index 0 in table )
//
//	 Inputs:
//
//	 Return: 
//
// -----------------------------------------------------------------
void MultiPlayer::StartSeed( void )
{

	if ( IsMultiPlayer() )
	{

		if ( fHaveSeedTable )
		{

			iOnSeed = 0;

			LOGCOMMENTSLEEP
			if ( fLogComment )
				RandomLogComment ( "Start Rand Seed" );

			srand ( ulRseeds[0] );

		}
		else
		{
			#ifdef MULTIMONO
				mono_puts ( 40, 15, "StartSeed NO RANDOM TABLE" );
			#endif
		}

	}

}	//---- MultiPlayer::StartSeed()



// -----------------------------------------------------------------
//
//	 MultiPlayer::InitSeedTable
//
//		initialize random seed table
//
//	 Inputs:
//
//	 Return: 
//
// -----------------------------------------------------------------
void MultiPlayer::InitSeedTable( void )
{
	int x;

	if ( IsMultiPlayer() )
	{
		fHaveSeedTable = FALSE;

		iOnSeed = 0;

		for ( x=0; x < kNumSeeds; ++x )
		{
			ulRseeds[x] = 0;
		}

	}

}	//---- MultiPlayer::InitSeedTable()




// -----------------------------------------------------------------
//
//	 MultiPlayer::NextSeed
//
//		advance to next seed  
//
//	 Inputs:
//
//	 Return: 
//
// -----------------------------------------------------------------
void MultiPlayer::NextSeed( char * szFileName, int iLineNumber )
{

	if ( IsMultiPlayer() )
	{


		if ( fHaveSeedTable )
		{
			++iOnSeed;

			if ( iOnSeed >= kNumSeeds )
			{
				iOnSeed = 0;
			}

			LOGCOMMENTSLEEP
			if ( fLogComment )
			{
				char temp[200];
			  	sprintf ( temp, "Next Seed %d in %12.12s line %04d",
							iOnSeed,
							szFileName,
							iLineNumber );
				RandomLogComment ( temp );
			}

			srand ( ulRseeds[iOnSeed] );


		}
		else
		{
			#ifdef MULTIMONO
				mono_puts ( 40, 15, "NextSeed NO RANDOM TABLE" );
			#endif
		}
	}

}



// ------------------------------------------------------------------
//
//   StartSIGS
//
//		Start up a Sierra internet games system game
//                                                                              
//   Inputs:
//		none
//                                                                              
//   Returns: TRUE if the connection was made, else FALSE
//                                                                             
// ------------------------------------------------------------------
BOOL StartSIGS( void )
{
	BOOL fOK = FALSE;

	fOK = AMultiPlayer.CreateAGame( AMultiPlayer.GetPlayerName() );


	if ( fOK )
	{

		// --
		// -- Who is the master?
		// --

		fStartGame = ( 0 == AMultiPlayer.GetPlayerId() );

		// --
		// -- Deal with the game info
		// --


		//---- Master ??

		if ( fStartGame )
		{
			PNETGAMEINFO pngiInfo = NULL;

			// --
			// -- Setup the game description
			// --

			pngiInfo = AMultiPlayer.GetGameInfo();

		}
		else
		{
			 // --
			 // -- Receive the game information
			 // --

		}


		 // --
		 // -- Get the other player's name for the chat box
		 // --

		AMultiPlayer.EnumeratePlayers( DisplayString, 0 );

	}



	//---- Set Focus back to Birthright window

	SetForegroundWindow ( hwndApp );



	return ( fOK );


} //---- StartSIGS()




// ------------------------------------------------------------------
//
//   DisplayString
//		Enumeration callback for lists of protocols and games.
//                                                                              
//   Inputs:
//			lContext	- 0 - emun player 1 - enum games
//			theString	- enumerated name
//			theIndex	- index of the item (used in selecting the item)
//			pnm			- NETGAMINFO structure - only valid when 
//								enumerating games!, otherwise will be NULL
//                                                                              
//   Returns: none
//                                                                             
// ------------------------------------------------------------------
static void DisplayString( long lContext, char* theString, 
											long theIndex, PNETGAMEINFO pnm )
{
	int i;


	//---- Enumerating Players 

	if ( lContext == 0 )
	{

	    // -- Enumerate players; theIndex is DpId ( don't parse ourself )

       if ( strcmp( theString, AMultiPlayer.GetPlayerName() ) != 0 )
       {
			for ( i = 0; i < MAX_PLAYERS; ++i )
			{
				if ( AMultiPlayer.GetAPlayerId( i ) == theIndex )
				{
					strcpy( AMultiPlayer.GetOpponentName(i), theString );

					#ifdef _MULTIMONO
			        mono_printf ( 0, 7, "Have player %s id %d index %d", 
                 			      theString,
								  theIndex,
                        		  i );
					#endif
				}
			}
       }

	}
	else
	{
       //---- Enumerating Games 

		strcpy( &pszGames[theIndex][0], theString );

		//---- Terminate the string 

		pszGames[theIndex][23] = 0;

	}


} //---- End of DisplayString




// ------------------------------------------------------------------
//
//   EndWait
//		Destroy the waiting window.
//                                                                              
//   Inputs:
//			none
//                                                                              
//   Returns: none
// ------------------------------------------------------------------
void EndWait( USHORT usTypeOfWait )
{
	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		if (usTypeOfWait >= 16 && usTypeOfWait < 64)
		{
			char temp[200];
			sprintf ( temp, "MultiPlayer::EndWait - Sync Point %u", usTypeOfWait - 16);
			RandomLogComment ( temp );
		}
		else if (usTypeOfWait == 99)
		{
			char temp[200];
			sprintf ( temp, "MultiPlayer::EndWait - User wait message");
			RandomLogComment ( temp );
		}
	}

   #ifdef _MULTIMONO
       mono_puts ( 0, 0, "                                         " );
   #endif

	init_mouse("DAGGER",CURSOR_HOTSPOT_UL);
	
} //---- EndWait()




// ------------------------------------------------------------------
//
//   SaveString
//		Enumeration callback for lists of protocols.
//                                                                              
//   Inputs:
//			lContext	- extra parameter sent in enumeration
//			theString	- enumerated name
//			theIndex	- index of the item (used in selecting the item)
//			pnm			- NETGAMINFO structure - only valid when 
//								enumerating games!, otherwise will be NULL
//                                                                              
//   Returns: none
//                                                                             
// ------------------------------------------------------------------
static void SaveString( long lContext, char* theString, 
 						 long theIndex, PNETGAMEINFO pnm )
{
	int len;
	LPGUID lpGuid = (LPGUID) pnm;

//	strcpy( &pszProtocol[theIndex][0], theString );

	if ( *lpGuid == DPSPGUID_IPX )
	{
		strcpy ( &pszProtocol[theIndex][0], STRMGR_GetStr(STR_DP_IPX));
	}
	else if ( *lpGuid == DPSPGUID_TCPIP )
	{
		strcpy ( &pszProtocol[theIndex][0], STRMGR_GetStr(STR_DP_TCP));
	}
	else if ( *lpGuid == DPSPGUID_MODEM )
	{
		strcpy ( &pszProtocol[theIndex][0], STRMGR_GetStr(STR_DP_MODEM));
	}
	else // Should never happen in BR
	{
		strcpy ( &pszProtocol[theIndex][0], "Unknown connection" );
	}

	//---- remove ending "Connection for DirectPlay"

//	pszProtocol[theIndex][ strlen ( theString ) - 26] = 0;


} //---- End of SaveString()




// ------------------------------------------------------------------
//
//   StartWait
//		Display the waiting window.
//                                                                              
//   Inputs:
//			usType	- type of wait
//                                                                              
//   Returns: none
// ------------------------------------------------------------------
void StartWait( USHORT usType )
{

	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		CHAR *szMsg[] = {
			"Sending a message",
			"Waiting for a message",
			"Waiting for joiners",
			"Waiting for a file",
			"Sending a file",
		};

		CHAR szText[MAX_PATH] = "";

	   //---- Need Something on the screen to tell that we are waiting

		if (usType <= 4)
		{
			sprintf( szText, "%s - %s", szAppName, szMsg[usType] );
#ifdef _MULTIMONO
			mono_puts ( 0, 0, szText );
#endif
			RandomLogComment ( szText );
		}

		else
		{
			char temp[200];
		
			// sync point value + 16
			if (usType == 99)
			{
				sprintf ( temp, "MultiPlayer::StartWait - User Sync message");
				RandomLogComment ( temp );
			}
			else
			if (usType >= 16 )
			{
				sprintf ( temp, "MultiPlayer::StartWait - Sync Point %d", usType - 16);
				RandomLogComment ( temp );
			}
		}
	}

	init_mouse("WAITCURS",CURSOR_HOTSPOT_UL);

} //---- End of StartWait()



// ------------------------------------------------------------------
//
//   IsMultiPlayer()
//		
//                                                                              
//   Inputs:
//                                                                              
//   Returns: none
// ------------------------------------------------------------------

extern "C" {
BOOL IsMultiPlayer(void)
{
	long i;
	
	// -- GWP 7/10/97
	// -- check to see if this player was deleted
	for ( i = 0; i < MAX_PLAYERS; ++i)
	{
		if ( AMultiPlayer.GetPlayerId() == AMultiPlayer.GetAPlayerId(i)
			&& AMultiPlayer.GetDeletedPlayer( i ) )
			return FALSE;
	}
	
	if ( AMultiPlayer.IsRunning()
		&& AMultiPlayer.CheckForPlayers() == FALSE )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
}

// ------------------------------------------------------------------
//
//   IsMultiPlayerMaster()
//		
//                                                                              
//   Inputs: Multiplayer and master 
//                                                                              
//   Returns: none
// ------------------------------------------------------------------

BOOL IsMultiPlayerMaster(void)
{
	long i;
	
	// -- GEH 7/9/97
	// -- check to see if this player was deleted
	for ( i = 0; i < MAX_PLAYERS; ++i)
	{
		if ( AMultiPlayer.GetPlayerId() == AMultiPlayer.GetAPlayerId(i)
			&& AMultiPlayer.GetDeletedPlayer( i ) )
			return FALSE;
	}
	
	if ( AMultiPlayer.IsRunning() &&
		 AMultiPlayer.IsMaster()     )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

// ------------------------------------------------------------------
//
//   HowManyPlayers()
//		
//                                                                              
//   Inputs: 
//                                                                              
//   Returns: count of players
// ------------------------------------------------------------------

LONG HowManyPlayers( void )
{
	return AMultiPlayer.HowManyPlayers();
}


void MDisplayInfo (CSTRPTR szFrame, int iformat, ...)
{
	char		texbuffer[200];
	va_list argp;
	SHORT		iBkgnd, w, h, x, y;
	char		*format;
	
	if ((iBkgnd = GetResourceStd(szFrame, FALSE)) == fERROR)
		return;

	format = STRMGR_GetStr(iformat);

	w = ((BITMPTR)BLKPTR(iBkgnd))->w;
	h = ((BITMPTR)BLKPTR(iBkgnd))->h;
	x = (SHORT)((((640-l_map)/2)+l_map) - w - 20);

	SysHideCursor();

	DrawBitmap(x, (SHORT)(240-(h/2)), iBkgnd, 0, 0, w, h);

	va_start(argp, iformat);
	vsprintf(texbuffer,format,argp);

	print_textf(x+7, 235, DKBROWN, texbuffer);

	SysShowCursor();

	SetPurge(iBkgnd);
	va_end(argp);


}


// ------------------------------------------------------------------
//
//   FileProgress
//		Do the File progress dialog.
//                                                                              
//   Inputs:
//		iBlock		- currently copying block
//		iMaxBlock	- max blocks to copy
//                                                                              
//   Returns: none
//                                                                      
// ------------------------------------------------------------------
// -- This routine is only used by file transfer, so I
// -- will use better art - GEH
#define SWORD_X	44
#define SWORD_Y	23
#define SWORD_W	265
#define SWORD_H	73

void FileProgress( int iBlock, int iMaxBlock )
{
	int iFormat = STR_FILETRANS_REC;
	int iPercent = (( iBlock * 1000 ) / iMaxBlock) / 10;

	if ( AMultiPlayer.IsMaster() )
	{
		iFormat = STR_FILETRANS_SEND;
	}


//	#ifdef _MULTIMONO
//        mono_printf ( 0, 14, "File transfer %d%% %d %d", 
//      			       (( iBlock * 1000 ) / iMaxBlock) / 10, 
//					   iBlock, iMaxBlock );
//	#endif


	if ( !(iPercent % 2) )
	{
		SHORT	bitm,iBkgnd;
		SHORT w,h,x,y;
		
		if ((iBkgnd = GetResourceStd("UI\\HELP_SM.PCX", FALSE)) == fERROR)
			return;
		
		w = ((BITMPTR)BLKPTR(iBkgnd))->w;
		h = ((BITMPTR)BLKPTR(iBkgnd))->h;
		x = (SHORT)(320-(w/2));
		y = (SHORT)(240-(h/2));
		
		SysHideCursor();
		
		DrawBitmap(x, y, iBkgnd, 0, 0, w, h);
		
		init_gfont(FONT_SANS_12PT);
		print_text_centered(x+10+(w/2),y+10+(h/4),STRMGR_GetStr(iFormat), DKBROWN);
		
		// paint greyed sword
		bitm = GetResourceStd ("ui\\SWORDD.PCX", FALSE);
		DrawBitmap (x+SWORD_X, y+SWORD_Y, bitm, 0, 0, SWORD_W, SWORD_H);
		SetPurge(bitm);
		
		// paint percentage bright sword
		bitm = GetResourceStd ("ui\\SWORDB.PCX", FALSE);
		DrawBitmap (
			x+SWORD_X,
			y+SWORD_Y,
			bitm, 0, 0,
			iPercent*SWORD_W/100,
			SWORD_H);
			
		SetPurge(bitm);
		SetPurge(iBkgnd);
		
		SysShowCursor();
		
		// - old methode
		//MDisplayInfo( "UI\\help_sm.PCX", iFormat, iPercent );
		
		update_screen();
	}


} // FileProgress()



// ------------------------------------------------------------------
//
//   IsMultiPlayerandRealm()
//     If this realm is a remote player and Multiplayer is still 
//     running. 		
//
//     NOTE: Still returns TRUE if our realm 
//
//   Inputs: realm ( not REALM::REALM_TYPE because the *.h are !@#!@ )
//                                                                              
//   Returns: none
// ------------------------------------------------------------------

BOOL MultiPlayer::IsMultiPlayerandRealm( int realm )
{

   if ( lRealmSelectedByPlayer[ realm ] != -1 &&
        AMultiPlayer.IsRunning() )
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }

}   //---- End of IsMultiPlayerandRealm()



// ------------------------------------------------------------------
//
//   DoMultiRealmStuff()
//		
//                                                                              
//   Inputs:
//                                                                              
//   Returns: none
// ------------------------------------------------------------------

BOOL DoMultiRealmStuff(SHORT HomeRealm, int iWhichRegent )
{
	int ok = FALSE;
	int timeNow;
	DWORD dMyId;

	// player has selected a realm
	// put the players id into the selected array
	// and send this data to other players

	// do a final check to make sure this realm is not taken

	if ( lRealmSelectedByPlayer[HomeRealm] != -1 &&
        lRealmSelectedByPlayer[HomeRealm] != AMultiPlayer.GetPlayerId() )
   {
		return ok;
   }

	// if we are the master

	if( AMultiPlayer.IsMaster() )
	{
		// set the data first

		lRealmSelectedByPlayer[HomeRealm] = AMultiPlayer.GetPlayerId();

		realm[HomeRealm].mfSetPlayerCtrl(TRUE);	 //---- Player controlled realm

		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
			char temp[200];
			sprintf ( temp, "MultiPlayer::DoMultiRealmStuff - Master player:%ld  requesting:%s",AMultiPlayer.GetPlayerId(), debug_realm_names[HomeRealm] );
			RandomLogComment ( temp );
		}


		// then tell all other players about it
		AMultiPlayer.BroadcastRealmInfo( HomeRealm, 
                                        AMultiPlayer.GetPlayerId(),
                                        ((HomeRealm * 2) + iWhichRegent - 2) );
		ok = TRUE;

	}
	else
	{
		// request this data be set by the master
		// send request for realm

		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
			char temp[200];
			sprintf ( temp, "MultiPlayer::DoMultiRealmStuff - non-master player:%ld  requesting:%s",AMultiPlayer.GetPlayerId(), debug_realm_names[HomeRealm] );
			RandomLogComment ( temp );
		}


		AMultiPlayer.RequestRealm( HomeRealm, 
                                  AMultiPlayer.GetPlayerId(),
                                  ((HomeRealm * 2) + iWhichRegent - 2) );


		// wait for ack for 5 seconds

       //		timeNow = GetTickCount();

		while(1)  // GetTickCount() < (timeNow + 5000))
		{
			AMultiPlayer.LookForMessages();

			Sleep(0);

			if(lRealmSelectedByPlayer[HomeRealm] != -1)
			{
				if(lRealmSelectedByPlayer[HomeRealm] == AMultiPlayer.GetPlayerId())
				{
					realm[HomeRealm].mfSetPlayerCtrl(TRUE);	 //---- Player controlled realm
					ok = TRUE;
				}
				break;
			}


           #ifdef _DEBUG
           if ( GetAsyncKeyState( VK_F10 ) & 0x8000 )
           {
              AMultiPlayer.Finalize();    
              break; 
           }
           #endif

		}


		//---- We timed out and no one else grabbed this realm set it ourself
		//---- Master must have disappeared 

//		if ( ok == FALSE &&
//			 lRealmSelectedByPlayer[HomeRealm] != -1 )
//		{
//			
//		}

	}

	return ok;

}



// ------------------------------------------------------------------
//
//   ExchangeSavedGame()
//		
//		Exchange saved games 
//                                                                              
//   Inputs:
//                                                                              
//   Returns: none
// ------------------------------------------------------------------

BOOL MultiPlayer::ExchangeSavedGame( void )
{

	fSent = FALSE;

	if ( eStatus == iMPS_SETUP )
	{

		if ( IsMaster() )
		{

			StartWait( iMPM_SEND );

			//---- Send the savegame file	( assumes MPTrans~.tmp )

			fSent = SendFile( NULL, "SaveGame\\MPTrans~.tmp" );

			EndWait( iMPM_SEND );

		}
 		else
		{

			StartWait( iMPM_RCV );

			//---- Receive the savegame file	( assumes MPTrans~.tmp )

			fSent = ReceiveFile( "SaveGame\\MPTrans~.tmp" );

			EndWait( iMPM_RCV );

		}

	}

	return ( fSent );



}	//---- End of ExchangeSavedGame()


/* ========================================================================
   Function    - PingMyMaster
	Description - Send a 'I'm alive' ping to the master
	Returns     - 
	======================================================================== */
extern "C" {
void PingMyMaster (void)
{
#ifdef _WINDOWS
	static LONG lLastPing = 0;
	
	if ( lLastPing > (LONG)GetTickCount() )
	{
		return;
	}
	
	lLastPing = GetTickCount() + MP_PING_FREQ ;
	
	if (fStartPinging && IsMultiPlayer())
	{
			
		LOGCOMMENTSLEEP
		if ( fLogComment )
		{
			char temp[200];
			sprintf ( temp, "Mulplay::PingMyMaster - Sending iMPT_PING.");
			RandomLogComment ( temp );
		}
	
		AMultiPlayer.Ping( AMultiPlayer.GetPlayerId(), AMultiPlayer.lMasterId );
	}
#endif
}
}

//----- End of MulPlay.cpp
