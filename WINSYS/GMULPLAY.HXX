// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//
//	 GMulPlay.hxx
//		GenericMultiPlayer class definition.
//
//
//	Who	When			Why
// ---	---------	-----------------------------------------------
//	CAM	14 JUN 96	Broke out from MultiPlayer class
//
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#ifndef _GMULPLAY_H
#define _GMULPLAY_H

//#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <wtypes.h>
#include "../typedefs.h"
#include "../Resource.h"

#include "fstream"
#include "StdIO.h"
#include "Limits.h"
#include "Direct.h"


#include "FileXfer.hxx"

// -- This needs to compile w/o the Windows.h stuff, so kludge away!
// #ifndef HWND
// typedef void *HWND;
// #endif
// #ifndef HINSTANCE
// typedef void *HINSTANCE;
// #endif
// #ifndef BOOL
// typedef int BOOL;
// #endif
// #ifndef PCHAR
// typedef char *PCHAR;
// #endif

#ifndef USHORT
typedef unsigned short USHORT;
#endif
#ifndef PSZ
typedef char *PSZ;
#endif
#ifndef DWORD
typedef unsigned long DWORD;
#endif
#ifndef MAX_PATH
#define MAX_PATH          260
#endif
#ifndef UCHAR
typedef unsigned char UCHAR;
#endif
#ifndef BYTE
typedef unsigned char BYTE;
#endif
 // -- End of kludge section


 // -- User-PNetMod types
#include "PNetType.h"

#if 0
const int iMaxChat	= 20;


typedef struct 
{
   char szMessage[120];        //---- could be less

} SZCHAT, *PSZCHAT;
#endif




 // -- Prototype for the Chat Message alert sound routine

typedef void (*FP_MISCPROC)( void );


 // -- Forward reference to imported class
// class FileXfer;

class /* PROCACCESS */ GenericMultiPlayer
{
	// --
	// --- Data Members ---
	// --
public:
	 // -- MultiPlayer types
	enum PlayType
	{
		iMP_NETMODEM,
		iMP_INN,
		iMP_SIGS,
		iMP_NUMTYPES
	};

protected:

	enum
	{
		MAX_MSG_SIZE	= MAX_NETBUFF,
		iMPT_SYSTEM		= 0,			// system file type
		iMPT_ANY		= 190, 			// any message is OK
		iMPT_FILE		= 200,  		// flag to look for start file message
	};

	 // -- Class information

	BOOL		fAvailable;			// Is Multi-Player play available?
	FileXfer    pnmGame;			// multiplayer communication class
	HINSTANCE	hiLib;			    // handle of the desired multiplayer library
	PlayType	ptType;				// type of multiplayer play
	PSZ		    pszRunDir;			// pointer to run directory

//	short		sMaxChatEntries;    // max. chat entries

	FP_MISCPROC fpMsgProc;		    // Message processing procedure

	BOOL		fSent;				// status of the last send/receive

	DWORD		dwMsgFudge;			// message fudge factor (to avoid msg Send/Send locks)
	DWORD		dwMsgWaitS;			// send wait
	DWORD		dwMsgTriesS; 		// tries for a send
	DWORD		dwMsgWaitSR;		// receive in send loop
	DWORD		dwMsgWaitR;			// rcv wait
	DWORD		dwMsgWaitRS; 		// receive in rvc loop 
	DWORD		dwMsgMasterFudge;	// master side fudge factor
	DWORD		dwMsgMaxLoop;		// max loops

	char		cRcvBuf[MAX_NETBUFF]; 	   // my receiving message buffer
	char		cSndBuf[MAX_NETBUFF]; 	   // my sending message buffer
	char		cNetBuf[MAX_NETBUFF]; 	   // network message buffer
	ULONG	    ulSndMsgNum;  			   // synchronization id for sending
	ULONG	    ulRcvMsgNum[MAX_PLAYERS];  // synchronization id for receiving


//   short       sNextMessage;             //---- Next slot to update
//   SZCHAT      szChatMessages[iMaxChat]; //---- Should be allocated


	// --
	// --- Member Functions ---
	// --

public:
	GenericMultiPlayer		        ( void );
	virtual ~GenericMultiPlayer		( void );

	BOOL		CreateAGame			( PSZ pszPlayerName );
	void		EnumerateGames		( GameDisplayStringFunction DString, long lContext );
	void		EnumeratePlayers	( GameDisplayStringFunction DString, long lContext );
	void		EnumerateProtocols  ( GameDisplayStringFunction DString, long lContext );
	BOOL		Finalize			( void );

	BOOL		Initialize			( PlayType ptType = iMP_NETMODEM );

	BOOL		JoinAGame			( long lIndex, PSZ pszPlayerName );
   BOOL		PeekForNextMessage  ( LPNETMSG pMsg );
	BOOL		SelectAProtocol  	( long lIndex );

   BOOL        IsNetworkingAvailable( void );

   int         CheckConnectStatus( void );

   int         ForceStart( void );
   void        DisablePlayers( void );

//	void		SetupChat           ( void );

	virtual BOOL	StopTheGame		( void ) = 0;

//	void		UpdateChatList	    ( PCHAR pszText );

	BOOL		WaitForPlayer		( void );
	int			WaitForPlayers		( void );

	FileXfer    *GetNetModule      	( void );
//	short		GetMaxChatEntries	( void );

	BOOL		IsAvailable			( void );
	BOOL		WasConnectionLost	( void );

//	void		SetMaxChatEntries	( short s );

	void		SetMsgProc			( FP_MISCPROC f );
	void		SetRunDir			( PSZ pszRunDir );

	BOOL		IsModemGame			( void );


protected:


	virtual BOOL	HandleSpecialMessages( UCHAR usMsgId, long lMsgNum ) = 0;

	virtual BOOL	LookForMessages( int fMulPlay = 1 )	= 0;


	BOOL	ReceiveFile		( DWORD dwFrom, PSZ pNew );
	BOOL	SendFile	    ( PSZ pszFile, unsigned int index );

	void	SelectGuid		( int i );

	BOOL	ReceiveMessage	( USHORT usType, DWORD dwWait, BOOL fOneOnly = FALSE,
   							BOOL fAck = TRUE );

	BOOL	SendBroadcastMessage( USHORT usType, short sSize, BOOL fAck = TRUE );
	BOOL	SendDirectMessage	( DWORD dwToWhom, USHORT usType, short sSize, BOOL fAck = TRUE );


   void  SetWaitTimes  ( USHORT usMultiplier = 1 );

   DWORD   GetPlayerId ( void );

   DWORD   GetAPlayerId ( int i );

	int     GetDeletedPlayer( int i );

	DWORD   GetDeletedPlayerId( int i );

	void	ResetDeletedPlayer  ( int index );

	int     GetAPlayerIndex ( DWORD dwPlayerId );

	void    DeletePlayer  ( DWORD dwDelPlayerId );

private:

	BOOL	SendMessage		( DWORD dwToWhom, USHORT usType, short sSize, BOOL fAck = TRUE );

	// -- private copy constructor / assignment operator
	// -- they are declared but not defined so defaults are not made
	// -- by the compiler!

	GenericMultiPlayer ( const GenericMultiPlayer& org );
	GenericMultiPlayer & operator= (const GenericMultiPlayer& org );

}; // class MultiPlayer



#endif



//---- GMulPlay.hxx
