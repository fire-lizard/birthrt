// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//
//	 PNetType.h
//		User types for PNetMod class
//
//
//	Who	When			Why
// ---	---------	-----------------------------------------------
//	CAM	5 JUN 96		First Incarnation
//
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#ifndef _PNETMOD_H_
#define _PNETMOD_H_

const int MAX_NETBUFF       = (512);

 // -- Master network game structure
typedef struct
{
  // -- Session information
   char szGameName[32];      // name of the game
	char szGameNamePlus[50];	 // extra space for realm version for just Birthright
//   GAMEDESC szGameDesc;      // description of game

   USHORT usMaxPlayers;      // maximum number of players for this game

   DWORD   dwUser1 ;
   DWORD   dwUser2 ;
   DWORD   dwUser3 ;
   DWORD   dwUser4 ;
} NETGAMEINFO, *PNETGAMEINFO;

typedef void (*GameDisplayStringFunction)(long theContext,
                                          char* theString,
                                          long theProtocolIndex,
                                          PNETGAMEINFO pGameInfo );

// -- Message structures
typedef struct
{
   BYTE     MsgId;              // message id
   ULONG    ulSync;             // synchronization  number
   USHORT   usCheckSum;         // my checksum
   DWORD    dwPlayerId;         // who send the generic message
   BYTE     bNeedsAck ;
   USHORT   usDataSize ;        // bytes of data
   char     cData[1];           // actual data of the message
} NETMSG, *LPNETMSG;

#endif

 // PNetType.h
