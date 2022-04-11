// -------------------------------------------------------------------
//
//      PNetMod.h
//
//      Definition file for PlatformNetworkModule class
//
//      Programmed by:      Cheryl A. Makovsky
//                        Synergistic Software
//
//      David Jackson   make it multiplayer more than 2  
//
// -------------------------------------------------------------------


#ifndef _H_PlatformNetworkModule
#define _H_PlatformNetworkModule

const char kPEEKONLY = 0;              //---- Peek message 
const char kREMOVEFROMQUE = 1;         //---- Remove message 

const char kDONTACK = 0 ;
const char kNOACK = 0;                 //---- Non guarantee message 
const char kACK = 1;                   //---- Ack message

const int MAX_PLAYERS = 8;             //---- 8 for BR

const DWORD kSystemMessage = 0;        //---- DPLAY System message 

const DWORD kBroadcast = 99;           //---- Broadcast message 

const DWORD kNoWait = 0;               //---- Dont wait for receive

const DWORD kNoPlayer = 777777777;		//---- No player here in dwPlayers[]

const DWORD kHaveAll = 123;		 	//---- All players have joined



typedef enum ErrCode
{
    kNoError = 0,
    kNetworkingNotInstalled,
    kNetworkingSetupError,
    kGenericError,
    kNoNetworkProtocolsAreAvailable,
    kCannotCreateGame,
    kCannotJoinGame,
    kNoNetworkGamesAvailable,
    kGameAborted,
    kNoPlayersInGame,
    kNoGameStarted,
    kConnectionLost,
    kActionStructNotReady,
    kIllegalPlayerID,
    kNoProtocolSelected,
    kCannotCreatePlayer,
    kMessageNotReceived,
    kCannotGetGameInfo,
    kMessageBufferOverflow,
    kCheckSumError,
	 kPlayerDisconnected,
	 kExitForCodeUpdate,
	 kSpecial = 65536,
} PNET_ERRCODE_TYPE ;


// -- Message id's

const int NETMSGID_ALL   = 0;
const int NETMSGID_NACK  = 254;
const int NETMSGID_ACK   = 255;

const int NETMSGID_START = 240;    // special message start game early

const int NACK           = 254;
const int ACK            = 255;

const DWORD dwBroadcast  = 999;

typedef char GBoolean;




#include "PNetType.h"

typedef int (*GamePrintFunction)(char* s);


#define	IDIRECTPLAY2_OR_GREATER
#include "DPlay.h"

const int iMAXPROVIDERS	= (15);
const int iMAXSESSIONS		= (20);
const int iMAXGUIDS		= (3);

 // -- Misc. DirectPlay stuff

typedef HRESULT (__stdcall *FP_DPENUM)   ( LPDPENUMDPCALLBACKA, LPVOID );
typedef HRESULT (__stdcall *FP_DPCREATE) ( LPGUID , LPDIRECTPLAY *, IUnknown *);


const int	PLAYERNAME_LEN	= 256;


class PlatformNetworkModule
{
private:
   LPNETMSG      lpRecBuff;            //---- Receive buffer
   LPNETMSG      lpSndBuff;            //---- Send buffer
   BOOL          fTimedout;

   // -- DirectPlay goop

   static FP_DPENUM		fp_DPEnum;		//---- Function pointer to DirectEnumProviders()
   static FP_DPCREATE	fp_DPCreate;	//---- Function pointer to DirectPlayCreate()
   HANDLE					dphEvent;		//---- We aren't using events
   HINSTANCE				hDPlay;        //---- Instance for dplay.dll
   LPDIRECTPLAY			lpDP;          //---- Direct Play Object
   LPDIRECTPLAY3A			lpDP3A;        //---- Direct Play Object

	GUID						gGuid;			//---- this object's GUID



protected:

   ErrCode   ReceiveMessages ( DWORD dwWait, 
                               const char bRemove, const char bAck = kACK );

   GBoolean      fNetworkAvailable;
   char          szPlayerName[PLAYERNAME_LEN];

   GBoolean      fGameInProgress;             // are we currently playing a network game?
   NETGAMEINFO   ngiInfo;                     // information for the current game
   NETGAMEINFO   ngiInfos[iMAXSESSIONS] ;      

   LPNETMSG      lpGenericMsgBuf;             // pointer to generic message buffer
   char          szLastErrorMessage[256] ;


   // -- Internal game information

   BOOL          fGameCreator;                // am I the game creator?

   GUID          guidInstance;                // the current game's guid
   GUID          guidApplication;              // guid of the application 

   DWORD         dwPlayerId;                  // my player Id

   BOOL          fModem;                      // are we a modem game?

   USHORT        usNumPlayers;                // number of players in our game
   DWORD         dwPlayers[MAX_PLAYERS];      // list of my players
   int           dwDeleted[MAX_PLAYERS];      // player was deleted 
   DWORD         dwDeletedId[MAX_PLAYERS];    // player deleted id 
   DWORD         dwAcks[MAX_PLAYERS];         // list of acks from my players

   USHORT        usProviderIdx;               // current index
   USHORT        usNumProviders;              // number of providers in the list
   GUID          guidProviders[iMAXPROVIDERS];// enumerated service providers guids

   USHORT        usSessionIdx;                // current index
   USHORT        usNumSessions;               // number of providers in the list
   GUID          guidSessions[iMAXSESSIONS];  // enumerated sessions	guids

   GamePrintFunction  pPrintFunction ;        // pointer to game specific string display func



   BOOL      AddPlayer					( DWORD dwNewPlayerId );
//   void      DeletePlayer				( DWORD dwDelPlayerId );
   void      InitVars					( void );
   int       PrintString				( char * ) ;
   ErrCode   ReleaseDPlay				( void );
   void      ResetGameInformation		( void );
	void	  ResetAcks					( void );

public:
   PlatformNetworkModule					( GamePrintFunction pPrintFunc, ErrCode *err ) ;
   virtual ~PlatformNetworkModule		( void );

   void SetSigs	( BOOL fPassSigs);
   BOOL GetSigs	( void );

	int       CheckConnectStatus        ( void );

   void      DeletePlayer				( DWORD dwDelPlayerId );

	virtual ErrCode CancelNetworkGame( void );

   virtual ErrCode CreateNetworkGame	(char *playername);

   virtual ErrCode EnumerateNetworkProtocols
                              (GameDisplayStringFunction theDisplayStringFunc,
                              long theContext);

   virtual ErrCode EnumerateNetworkGames
                              (GameDisplayStringFunction theDisplayStringFunc,
                               long theContext);

   virtual ErrCode EnumeratePlayers
                              (GameDisplayStringFunction theEnumeratePlayersFunction,
                               long theContext);

   virtual int PlatformNetworkModule::ForceStart( void );

   virtual void PlatformNetworkModule::DisablePlayers( void );


   virtual ErrCode GetLocalPlayerID    ( long* thePlayerID );
   virtual ErrCode GetOtherPlayerID		( long* thePlayerID, int index ) ;

//   virtual ErrCode GetMessage				( LPNETMSG theMessage, DWORD dwWait, 
//											const char bRemove, BOOL fAck = TRUE );

   virtual PNETGAMEINFO GetNetworkGameInfo( void ) ;

   virtual int     GetDeletedPlayer   ( int index );
   virtual DWORD   GetDeletedPlayerId ( int index );
	virtual void 	ResetDeletedPlayer  ( int index );

	virtual BOOL	IsModemGame			 ( void );
   virtual ErrCode IsNetworkingAvailable( void );

   virtual ErrCode JoinNetworkGame     ( long theGameIndex, char *playername );

   virtual int		PeekNextMessage(LPNETMSG pMsg) ;

	virtual void	SelectGuid				( int i );

   virtual ErrCode SendMessage			( DWORD  dpToId,
                                         BYTE   bMsgId,
										  LPVOID lpMessage,
                                         short  sNumBytes,
                                         ULONG  ulMsgNum,
                                         DWORD  dwWait,
                                         BYTE   bRetryCount
                                       ) ;

   virtual ErrCode SetNetworkProtocol  ( long theProtocolIndex );

   virtual BOOL WaitForPlayer				(DWORD dwWait) ;
   virtual int  WaitForPlayers				(void) ;

   virtual void ErrorDPlay					(HRESULT hResult ) ;

	virtual BOOL UpdateandCheckAcks		( DWORD dwPlayerId );

	virtual BOOL CheckOneAck				( DWORD dwPlayerId );

	// need not be virtual; only called internally 12/20/96 mcqueen

	void BuildDPNameA(	LPDPNAME pdpName,
								char *pszFormalName,
								char *pszFriendlyName );




    // -- These cannot be member functions; DirectPlay hates that!
   friend BOOL FAR PASCAL EnumPlayers   ( DPID dpID,
														DWORD dwPlayerType,
														LPCDPNAME lpcdpName,
														DWORD dwFlags,
														LPVOID lpContext );

   friend BOOL FAR PASCAL EnumPlayersInit( DPID dpID,
		  												 DWORD dwPlayerType,
                                           LPCDPNAME lpcdpName,
														 DWORD dwFlags,
														 LPVOID lpContext );

   friend BOOL FAR PASCAL EnumServiceProvider( LPGUID lpSPGuid, 
															  LPSTR lpFriendlyName,
															  DWORD dwMajorVersion,
															  DWORD dwMinorVersion,
															  LPVOID lpContext );

   friend BOOL FAR PASCAL EnumSession   ( LPCDPSESSIONDESC2 lpDPSGameDesc2,
								  						LPDWORD lpdwTimeOut,
								  		            DWORD dwFlags,
								                  LPVOID lpContext );


}; // PlatformNetworkModule


// -- low level error value setting code
void SetErrorNum ( PNET_ERRCODE_TYPE ErrorVal );
PNET_ERRCODE_TYPE GetErrorNum ( void );
PNET_ERRCODE_TYPE ClearErrorNum ( void );

#endif // _H_PlatformNetworkModule


// -- PNetMod.h
