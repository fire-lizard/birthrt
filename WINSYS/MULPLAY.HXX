// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//
//	 MulPlay.hxx
//		MultiPlayer class	definition.
//
//
//	Who	When			Why
// ---	---------	-----------------------------------------------
//	CAM	6 MAY 96		First Incarnation
//	CAM	27 MAY 96	Added sending/receiving of Substitution file
//	CAM	3 JUN 96		Added paths to Plan/Profile file names
//	CAM	5 JUN 96		Added FrameSync, changes waits to get separate
//							 times for rcv and send
//	CAM	14 JUN 96	Added base class GenericMultiPlayer
// dlj 12 sep 96   convert to birthright / anyone
// MDB  4 Oct 96	Stripped out football specific code
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#ifndef _MULPLAY_H
#define _MULPLAY_H

#include "GMulPlay.hxx"

// about 1/2 minutes in milliseconds
#define MP_PING_FREQ		(30*1000)

extern BOOL fStartPinging;

/* -----------------------------------------------------------------
   Prototypes
   ----------------------------------------------------------------- */

void		EndWait		 	( USHORT usType );
void		StartWait	 	( USHORT usType );

 // -- Multi-player states

enum MultiPlayerType
{
   iMP_NONE,
   iMP_MODEM,
   iMP_NETWORK,
   iMP_INN,
   iMP_SIGS
};


enum MultiGameType
{
   iMPGT_NONE,
   iMPGT_NEW,
   iMPGT_LOAD,
   iMPGT_BATTLE
};



enum 
{
   iMPE_NONE,
   iMPE_ADVENTURE,
   iMPE_AGITATE,
   iMPE_BUILDROAD,
   iMPE_CONTEST,
	iMPE_CREATEHOLDING,
	iMPE_DIPLOMACY,
	iMPE_ESPIONAGE,
	iMPE_RULE,
	iMPE_TRADE_ROUTE,
	iMPE_ADD_ITEM = 4096,		// weird stuff
	iMPE_DROP_ITEM,
	iMPE_GAIN_EXP,

};


enum
{
	iMPS_SYNC0,       //---- Used in gamemap.cpp to sync admin turns 
	iMPS_SYNC1,       //---- Used in mapai.cpp to sync player controlled action turns
	iMPS_SYNC2,
	iMPS_SYNC3,
	iMPS_SYNC4,
	iMPS_SYNC5,
	iMPS_SYNC6,
	iMPS_SYNC7,
	iMPS_SYNC8,
	iMPS_SYNC_INIT01,	//GEH a unique sync point based on the
	iMPS_SYNC_INIT02,	//GEH  player initiative location during actions
	iMPS_SYNC_INIT03,
	iMPS_SYNC_INIT04,
	iMPS_SYNC_INIT05,
	iMPS_SYNC_INIT06,
	iMPS_SYNC_INIT07,
	iMPS_SYNC_INIT08,
	iMPS_SYNC_INIT09,
	iMPS_SYNC_INIT10,
	iMPS_SYNC_INIT11,
	iMPS_SYNC_INIT12,
	iMPS_SYNC_INIT13,
	iMPS_SYNC_INIT14,
	iMPS_SYNC_INIT15,
	iMPS_SYNC_INIT16,
	iMPS_SYNC_INIT17,
	iMPS_SYNC_INIT18,
	iMPS_SYNC_INIT19,
	iMPS_SYNC_INIT20,
	iMPS_SYNC_INIT21,
	iMPS_SYNC_INIT22,
	iMPS_SYNC_INIT23,
	iMPS_SYNC_INIT24,
	iMPS_SYNC_INIT25,
	iMPS_SYNC_INIT26,
	iMPS_SYNC_INIT27,
	iMPS_SYNC_INIT28,
	iMPS_SYNC_INIT29,
	iMPS_SYNC_INIT30,
	iMPS_SYNC_INIT31,
	iMPS_SYNC_INIT32,
	iMPS_SYNC_INIT33,
	iMPS_SYNC_INIT34,
	iMPS_SYNC_GO,
	iMPS_SYNCEND
};

#define iMPS_SYNC_INITIATIVE	iMPS_SYNC_INIT01


#define iCHAT_DIRECT    0
#define iCHAT_BROADCAST 1


 // -- User-PNetMod types

#include "PNetType.h"


#define MAX_GAMES 10

#define kWaitTurn		7777      //---- Waiting for turn update
#define kFromMulti		7777      //---- calling from multiplayer 


#define kNumSeeds      100        //---- number of random seeds



const int iPlayerNameSize	= 19;
const int iGameNameSize	= 23;
const int iChatMsgSize		= 100;  //
const int iPlaySpecSize	= 100;	// overkill number

const char szLoadTemp[] = ".\\SAVEGAME\\MPTRANS~.TMP";


// const int iPlanProfileName = MAX_PATH;

typedef void (*FP_UPDATECALLBACK) ( void );


typedef struct
{
	char	    szOpponentName[iPlayerNameSize+1];	// other player's name

} SZOPPONENT, *PSZOPPONENT;


//---- Used by the Event generator or requester 

typedef struct
{
	int	EventId;				//---- What event are we waiting for replies 0 - none
	int dwFrom;                 //---- Who sent event
	int	replied[MAX_PLAYERS];	//---- Have they replied 1 - yes
	int data1;					//---- eventdata
	int data2;					//---- eventdata
	int data3;					//---- eventdata
	int data4;					//---- eventdata

}	REQEVENT, *PREQEVENT;


//---- Used by event replier

typedef struct
{
	int 	EventId;				//---- Event has come in
	int     replied;            	//---- have we replied
	int		data1;					//---- eventdata
	int 	data2;					//---- eventdata
	int 	data3;					//---- eventdata
	int 	data4;					//---- eventdata

}	REPEVENT, *PREPEVENT;


typedef struct
{
	int	EventId;				//---- What event are we waiting for replies 0 - none
	int data1;					//---- eventdata
	int data2;					//---- eventdata
	int data3;					//---- eventdata
	int data4;					//---- eventdata

}	RESEVENT, *PRESEVENT;



//---- Message header 

typedef struct
{

   USHORT      usMsgType;	// what is it?
   BOOL        fMaster;    // from master ? 
   DWORD       dwFrom;     // from id ( 0  master ? )

} BRHeader, *PBRHeader;


//---- Message master sends to other players 

typedef struct
{
	long	      lMasterId;            //---- Master id 
	unsigned long ulRseeds[kNumSeeds];	//---- random seeds 
	long 		  game_difficulty;
	long		  battle_difficulty;
	long		  battle_speed;
   int           iTTimeLimit;          //---- turn time limit  
   float         realmAIver;           //---- realm AI version
   long         DetailLevel;           //---- basic, adv, expert

} BRSetup, *PBRSetup;



//---- Sync Message

typedef struct
{
	int	      iSync;                //---- which Sync are we at

} BRSync, *PBRSync;


typedef struct
{
	long	      dwId;                //---- which Sync are we at

} BRLeftGame, *PBRLeftGame;


//---- Message won the game 

typedef struct
{
	SHORT	HomeRealm;    //---- winners realm

} BRWonGame, *PBRWonGame;


//---- Message  

typedef struct
{
	int control;    //---- control On or Off were outta control......

} BRControl, *PBRControl;



//---- Event message 

typedef struct
{

	int EventId;				//---- Event has come in
	int data1;					//---- eventdata
	int data2;					//---- eventdata
	int data3;					//---- eventdata
	int data4;					//---- eventdata

} BREvent, *PBREvent;


//---- Reply to Event message 

typedef struct
{
	int EventId; 				//---- Event reply id 
	int data1;					//---- eventdata
	int data2;					//---- eventdata
	int data3;					//---- eventdata
	int data4;					//---- eventdata

} BREventReply, *PBREventReply;



//---- Results to Event message

typedef struct
{
	int EventId; 				//---- Event reply id
	int data1;					//---- eventdata
	int data2;					//---- eventdata
	int data3;					//---- eventdata
	int data4;					//---- eventdata

} BREventResults, *PBREventResults;



//---- Chat message 

typedef struct
{
   char            messtype;
	char			szMsg[iChatMsgSize+iPlayerNameSize+3];

}  BRChat, *PBRChat;




typedef struct
{
	int 	iWhichRealm;
	long	lPlayerId;
	long	lWhichRegent;

} BRRealm, *PBRRealm;



typedef struct
{
	int 	iGameType;

} BRGameType, *PBRGameType;



typedef struct 
{
   long        lArray_Id;        // array id to update       
   long        lArray_Index;     // array index 
   long        lStruct_Id;       // struct element id 
   long        lData;            // value 

} BRIndexS, *PBRIndexS;



typedef struct 
{
   long        lArray_Id;        // array id to update       
   long        lArray_Index;     // array index 
   long        lStruct_Id;       // struct element id 

} BRIndexR, *PBRIndexR;


//---- Test message 

typedef struct
{

   long        lPinger;          // this ain't golf 

}  BRPing, *PBRPing;



//---- unit message
#define SIZEOF_MULTIUNIT 14
typedef struct
{
	DWORD		UnitIndex;
	UBYTE		UnitData[SIZEOF_MULTIUNIT];
}	BRUnitMessage, *PBRUnitMessage;

//---- places message
#define SIZEOF_MULTIPLACES 20
typedef struct
{
	DWORD		PlacesIndex;
	UBYTE		PlacesData[SIZEOF_MULTIPLACES];
}	BRPlacesMessage, *PBRPlacesMessage;

//---- administration message
#define SIZEOF_MULTIGOLD 34
#define SIZEOF_MULTIREGENCY 34
typedef struct
{
	UBYTE		GoldData[SIZEOF_MULTIGOLD];
	UBYTE		RegencyData[SIZEOF_MULTIGOLD];
}	BRAdminMessage, *PBRAdminMessage;

/// -- gain exp message
typedef struct
{
	LONG RegentIndex;
	LONG Amount;
	LONG GainLevel;
} BRGainExpMessage, *PBRGainExpMessage;

//---- allience message
#define SIZEOF_MULTIALLIED 34
typedef struct
{
	LONG		Realm;
	UBYTE		Reaction[SIZEOF_MULTIALLIED];
	UBYTE		Status[SIZEOF_MULTIALLIED];
}	BRAlliedMessage, *PBRAlliedMessage;


//---- Battle messages 

typedef struct
{

   long        lvalue1;          //  Always iMPT_BATTLE 
   long        lvalue2;          //  Subtype 
   long        lvalue3;          //  data
   long        lvalue4;          //  data
   long        lvalue5;          //  data
   long        lvalue6;          //  data

}  BRBattle, *PBRBattle;



class MultiPlayer : public GenericMultiPlayer
{
	// --
	// --- Data Members ---
	// --

public:


	enum MsgType
	{
		iMPT_CHAT	= 1, 		  		// chat messge
       iMPT_INDEXSEND,                 // send index value
       iMPT_INDEXREQUEST,              // request master set index value 
	   	iMPT_DATAREQUEST,				// ask master to return data
	   	iMPT_DATASEND,					// retrieve game data
       iMPT_PING,                      // Ping test message only
       iMPT_BATTLE,                    // Battle messages  
	    iMPT_UNIT,						// unit select messages
	    iMPT_PLACES,					// places select messages
	    iMPT_GAINEXP,					// regent gains exp points
	    iMPT_ADMIN,					// gold and regency
	    iMPT_ALLIED,					// allience data for a realm
	    iMPT_BREALM,					// Realm select messages
	    iMPT_REALM,						// Realm select messages

		iMPT_PLAYERLEFTGAME,      	    // a player has left the game
		iMPT_SETUP,                     // game info
//		iMPT_BACTREALM,                 // broadcast realm
//		iMPT_ACTREALM,                  // done with turn  
		iMPT_EVENT,						// event 
		iMPT_REPEVENT, 					// event reply
		iMPT_RESEVENT, 					// event results
		iMPT_CONTROL, 					// control ai/player
		iMPT_ADVENTOVER,  				// aventure over
		iMPT_WONGAME,	  				// won the game
		iMPT_SYNC,                      // sync point reached
  		iMPT_SYNCACTION,                // sync action turn
  		iMPT_GAMETYPE,                  // type of game
		iMPT_TARGETPLAYERLEFTGAME,      // specified player has left the game
		iMPT_END,                       // place holder 

	};



private:

	enum Status
	{
		iMPS_NONE,		// nothing has happened
		iMPS_INIT,		// we have initialized a game
		iMPS_ENDED,		// game is over
		iMPS_SETUP,		// we have setup the game
	};




	 // -- Game Information

	Status	    eStatus;				// What is the status of the Multi-Player game?
	BOOL	    fMaster;				// Are we the master

	SZOPPONENT  szOpponentName[8];	    // other player's names


	char    	szName[iPlayerNameSize+1];	// my friendly name
	char	    szGame[iGameNameSize+1];  	// my last game name
	short	    sProtocol;			        // my last protocol selected
	short	    sConnSpeed;		       		// my last connection Speed 0 modem 1 network
	short	    sNumPlayers;		        // my last number of players used

public:
	int         lMasterId;                  // Master Id  -1 is not set

	// --
	// --- Member Functions ---
	// --

	MultiPlayer					( void );
	~MultiPlayer				( void );

	 // --
	 // -- Administration and Setup
	 // --

   int             ForceStart( void );

   void            DisablePlayers( void );

	BOOL			ConnectionLost	( void );
	BOOL  			Finalize	  	( int type = 0 );
	PNETGAMEINFO	GetGameInfo  	( void );

	int 			CheckForPlayers( void );

	BOOL			CheckPlayerId( DWORD dwPlayerId );
	LONG			HowManyPlayers( void );

	int 			InitPre 		( int doSigs );
	void 			InitPost 		( void );
	void 			InitPost2 		( void );

	void			InitChat		( void );
	void			InitEvent		( int which = 0 );


	BOOL		   	Setup			( void );


	BOOL 			StartNewGame	( void );
	BOOL 			JoinGame 		( int index );


   void            UpdateDeletedPlayers( void );


	//---- Random functions 

	void 			StartSeed( void );
	void 			InitSeedTable( void );
	void 			NextSeed( char * szFileName, int iLineNumber );


	//---- Sync stuff

	void  			ResetSync( int sync );
	BOOL 			SendSync( int sync );
	BOOL 			SendSyncTo( int sync, DWORD dwToWhom );
	int             GetSync( int sync, int index );


	// -- sync time outs
	void			ResetAllSyncTimes( void );
	long			GetSyncTime( int index );
	void			ResetSyncTime( int index );
	
	void			ResetAllSyncCounts( void );
	void			IncSyncCount( int index );
	long			GetSyncCount( int index );
	
	//---- Sync Action 

	BOOL 			SendSyncAction( void );
	void  			ResetSyncAction( int rlm );
	int             GetSyncAction( int rlm );


	//---- Send Game Type

	BOOL 			SendGameType( void );


   //
   //---- Test
   //

   BOOL    Ping            ( long lData, DWORD dwToWhom = kBroadcast );


   //
   //---- Chat
   //

	BOOL	Chat			( char * pszMsg, int force = 0 );



	//---- Display network status

	void DisplayNetStatus 		( LONG mx, LONG my );



	//---- Realm actions 

//  	int  GetActionRealm		    ( void );

//	BOOL BroadcastActionRealm   ( int realm ); //, int action );	//---- Master only
//	BOOL DoneActionRealm   		( int realm );				//---- Done action


   //
   //---- Select Realm stuff
   //

	BOOL BroadcastRealmInfo( SHORT WhichRealm, DWORD WhichPlayer, LONG lWhichRegent );
	BOOL RequestRealm( SHORT WhichRealm, DWORD WhichPlayer, LONG lWhichRegent);

	BOOL BroadcastSetGameData(LONG lArray_Id, LONG lStruct_Id, LONG lArray_Index, LONG lData, DWORD dwToWhom = kBroadcast);

//	BOOL RequestSetGameData(LONG lArray_Id, LONG lStruct_Id, LONG lArray_Index, LONG lData);
//	BOOL BroadcastGetGameData(LONG lArray_Id, LONG lStruct_Id, LONG lArray_Index);
	BOOL RequestGetGameData(LONG lArray_Id, LONG lStruct_Id, LONG lArray_Index);


	//
	//---- Event messages
	//

	BOOL SendEvent( int EventId, 
					int data1, int data2, int data3, int data4, 
					int rlm );


	BOOL ReplyEvent( int EventId, 
					 int data1, int data2, int data3, int data4 ); 


	BOOL ResultsEvent( int EventId, 
					   int data1, int data2, int data3, int data4, 
					   int rlm );


	BOOL CheckEvent( void );


	BOOL GetAEventReply( int which, 
						 int * data1, int * data2, int * data3, int * data4 );


	int HaveEvent( void );

	int HaveResults( void );


	PCHAR GetRealmNameOfEvent(void);


	//----- Adventure Over Person who setup game sends this 
	
	BOOL BroadcastAdventOver ( void );


	//----- Won the game
	
	BOOL SendWonGame( SHORT HomeRealm );



	//---- Set control on and off

	BOOL MultiPlayer::SendControl( int control );


   //---- Message Loop

	virtual BOOL	LookForMessages	( int fMulPlay = 1 );

   // --
	// -- Game Play
	// --

   BOOL    SendIndex           ( LONG lArrayId,  LONG lArrayIndex,
                                 LONG lStructId, LONG lData );

   BOOL    RequestIndex        ( LONG lArrayId,  LONG lArrayIndex,
                                 LONG lStructId );

	BOOL	StopTheGame		( void );
	BOOL	RemovePlayer 	( DWORD dwId );


	BOOL	UnitMessage		( DWORD Index, UBYTE *Data, char * szWhy = "unspecified", DWORD dwToWhom = kBroadcast );
	
	BOOL	PlacesMessage	( DWORD Index, UBYTE *Data, DWORD dwToWhom = kBroadcast );
	
	BOOL	Battle			( LONG value1, LONG value2, LONG value3,
                                 LONG value4, LONG value5, DWORD dwToWhom = kBroadcast );

	BOOL	AlliedMessage	( DWORD Realm, UBYTE *Reaction, UBYTE *Status, char *szWhy, DWORD dwToWhom = kBroadcast );
	
	BOOL	GainExpMessage	( LONG Regent, LONG Amount, BOOL GainLevel, DWORD dwToWhom  = kBroadcast);
	
	BOOL	AdminMessage	( UBYTE *GoldData, UBYTE *RegencyData, char *szWhy, DWORD dwToWhom  = kBroadcast);

	int     WaitForPlayers		( void );


	// --
	// -- Data retrieval
	// --


	PCHAR	    GetGameName		( void );
	short	    GetNumPlayers	( void );
	short	    GetSpeed		( void );
	PCHAR	    GetPlayerName	( void );
   DWORD		GetPlayerId 	( void );
	DWORD		GetAPlayerId	( int i );

	int 		GetDeletedPlayer( int i );

   short       GetProtocolIndex( void );

	PCHAR 		GetProtocol 	( void );
	PCHAR       GetAProtocol    ( int i );

	PCHAR       GetOpponentName	( int i );


	int         GetChat         ( int index );
	int         GetChatRec      ( int index );

	void		GetGames		(void);

	PCHAR		GetAGames		(int i);

	BOOL 		GetPlayers		( void );

	int 		GetGameType		( void );

   BOOL        IsModemGame     ( void );


	// --
	// -- Object status
	// --

	BOOL		IsInitialized		( void );
	BOOL		IsMaster	  		( void );
	BOOL 		IsRunning			( void );
	BOOL		IsSlave				( void );

   BOOL		IsMultiPlayerandRealm      ( int realm );


   // --
   // -- Setting data
   // --

	void 		SetNumPlayers			( short sNP );
	void 		SetSpeed				( short sCS );
	void 		SetMaster				( BOOL f );

   void	   	SetProtocolIndex		( int sP );

	void        SetChat             	( int index, int set );
	void        SetChatRec             	( int index, int set );

	void		SetControl				( int dwId, int control );

	void 		SetGameType				( int GameType );



	//---- Realm selection stuff 

   int			GetRealmFromId( DWORD dwId  );		


	//---- Exchange saved games 

	BOOL		ExchangeSavedGame		( void );



protected:


	short DoHeader				( MsgType iType, PCHAR *ppMsg );
	virtual BOOL	HandleSpecialMessages( UCHAR usMsgId, long lMsgNum );

	void	InitGoop		  	( void );
	void	ReadCfg				( void );

	BOOL	ReceiveFile			( PSZ pNew );
	BOOL	ReceiveMessage		( USHORT usType, DWORD dwWait, BOOL fOneOnly = FALSE );


	BOOL	SendFile			( PSZ pszPath, PCHAR pszFile );

	BOOL	SendBroadcastMessage ( short sSize, BOOL fAck = TRUE );
	BOOL	SendDirectMessage  	 ( DWORD dwToWhom, short sSize, BOOL fAck = TRUE );

	void	WriteCfg				( void );


	 // -- private copy constructor / assignment operator
	 // --  they are declared but not defined so defaults are not made
	 // --   by the compiler!

	MultiPlayer ( const MultiPlayer& org );
	MultiPlayer operator= (const MultiPlayer& org );


}; // class MultiPlayer



   // -- Default object

   extern MultiPlayer AMultiPlayer;

	extern void MultiPlayerMenu   	( long,
  							  		  long );
	extern BOOL LookMessages(void);

extern "C" {
   extern BOOL IsMultiPlayer ( void );
	extern void PingMyMaster (void);
}

	extern BOOL IsMultiPlayerMaster ( void );

	extern LONG HowManyPlayers ( void );
   
	extern BOOL StartSIGS( void );

	extern REQEVENT EventTable;					//--- Event table
	extern REPEVENT EventReply[MAX_PLAYERS];	//--- Event reply
	extern RESEVENT EventResults;				//--- Event results 

	extern int EventStarter;                    //--- Started event

	extern int iAdventOver;                     //---- Flag for adventure over


	extern BOOL DoMultiRealmStuff(SHORT,int);

   extern float MasterRealmAIVersionNumber;


#endif



//---- End of MulPlay.hxx
