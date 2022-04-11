/*
 * SNTypes.h
 *
 * Author:        John W. Fawcett
 * Date Created:  08/06/96
 *
 * Purpose:
 *  Public type declarations for SIGS
 *
 * Copyright (c) 1996 by Sierra On-Line, Inc.
 * All Rights Reserved.
 *
 * $Header: /export/home/rcs/sigs2.0//sierralib/snlib/SNTypes.h,v 1.9 1996/09/10 19:02:05 abakken Exp $
 *
 * $Log: SNTypes.h,v $
 * Revision 1.9  1996/09/10  19:02:05  abakken
 * Added the JoinInProgressFlag for games
 *
 * Revision 1.8  1996/09/09  18:28:49  jepson
 * Added const defintions from SNGen.h
 * Changed Invite Permissions options
 *
 * Revision 1.7  1996/09/04  00:01:25  abakken
 * dont use UINT
 *
 * Revision 1.6  1996/09/03  23:07:38  abakken
 * Use LOGICAL instead of BOOL
 *
 * Revision 1.5  1996/08/26  16:35:31  abakken
 * sync with the PC side
 *
 * Revision 1.4  1996/08/07  20:08:50  abakken
 * expanded the game state variables
 *
 * Revision 1.3  1996/08/07  14:53:41  fawcett
 * Added struct for passing PublicInfo back to the application
 *
 * Revision 1.2  1996/08/06  22:14:55  abakken
 * fixed the header
 *
 *
 */

#ifndef _SN_TYPES_H_
# define _SN_TYPES_H_

// this is to allow for one function decleration
// DO NOT!! set the MAKE_DLL parameter in your mak file.
#ifdef MAKE_DLL
	#define SIGSAPI extern "C" __declspec( dllexport )
	#define DllExport extern "C" __declspec( dllexport )
#else
	#define SIGSAPI extern "C" __declspec( dllimport )
#endif

#define HEADER_DATATYPE unsigned short

// Data types
typedef short           LOGICAL;
typedef LOGICAL         SN_FLAG;
// typedef char*           PSTR;
// typedef const char*     PCSTR;
typedef long            CUSTID;
typedef CUSTID          PLAYERID;
typedef CUSTID          USERID;
typedef unsigned char   SN_CODE;
typedef short           GPNUM;
typedef long            CYBERCASH;
typedef long            ROOMNUM;
typedef long            GAMENUM;
typedef GAMENUM         SN_SERVICE;
typedef long            PORTNUM;
typedef short           SERVERSITE;
typedef short           PREQUALNUM;

// String lengths
const short ADDRESS_LEN                 = 60;
const short CITY_LEN                    = 50;
const short COUNTRY_LEN                 = 30;
const short STATE_LEN                   = 4;
const short USER_NAME_LEN               = 25;


// Logical values
#ifndef sun
#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#else
const int TRUE = 1;
const int FALSE = 0;

#endif

#ifdef  min
#undef  min
#endif             /* min */
#define min(a, b)       ((a) < (b) ? (a) : (b))

#ifdef  max
#undef  max
#endif             /* max */
#define max(a, b)       ((a) < (b) ? (b) : (a))

// Status values
const int SN_OK = 0;
const int SN_ERR = -1;
const int SN_TIMEOUT = -2;
const int RETURN_OK = SN_OK;
const int RETURN_ERR = SN_ERR;
const int RETURN_TIMEOUT = SN_TIMEOUT;

// Age Codes
const int AC_LESS_7 = 1;
const int AC_7_12 = 2;
const int AC_13_18 = 3;
const int AC_19_25 = 4;
const int AC_26_40 = 5;
const int AC_41_55 = 6;
const int AC_55_GREATER = 7;
const int AC_NA = 99;

// Gender codes
const int GC_MALE = 1;
const int GC_FEMALE = 2;
const int GC_NA = 9;

// Internet service types
const long IS_OTHER = 0x01;
const long IS_MSN = 0x02;
const long IS_AOL = 0x04;
const long IS_WOW = 0x08;
const long IS_COMPUSERVE = 0x10;
const long IS_PRODIGY = 0x20;
const long IS_GENIE = 0x40;

// Modem speed codes
const int MS_2400 = 1;
const int MS_9600 = 2;
const int MS_14400 = 3;
const int MS_28800 = 4;
const int MS_HIGH = 5;
const int MS_NA = 9;

// Processor codes
const int PC_386 = 1;
const int PC_486 = 2;
const int PC_PENTIUM = 3;
const int PC_PENTIUM_PRO = 4;
const int PC_OTHER = 9;

// RAM amount codes
const int RA_LESS_8 = 1;
const int RA_8_12 = 2;
const int RA_12_GREATER = 3;

// Monitor codes
const int MC_14 = 1;
const int MC_15 = 2;
const int MC_17 = 3;
const int MC_LAPTOP = 4;

// Join and Watch Permissions
const int PM_OFF = 1;
const int PM_NONE = 2;
const int PM_ONE = 3;
const int PM_CAPTAIN = 4;
const int PM_GAME_OWNER = 5;
const int PM_MAJORITY = 6;
const int PM_ALL = 7;

// Invite Permissions
const int IP_NONE = 1;
const int IP_GAME_OWNER = 2;
const int IP_CAPTAIN = 3;
const int IP_PLAYER = 4;
const int IP_ALL = 5;

// Join In Progress
const int JIP_OFF = 0;
const int JIP_ON  = 1;

// Game Groups
const int GG_ADVENTURE = 1;
const int GG_ACTION = 2;
const int GG_SIMULATION = 3;
const int GG_STRATEGY = 4;
const int GG_ROLE_PLAYING = 5;
const int GG_SPORTS = 6;
const int GG_EDUCATION = 7;

typedef struct tPublicInfo {
    char UserName[USER_NAME_LEN];
    char City[CITY_LEN];
    char State[STATE_LEN];
    char Country[COUNTRY_LEN];
    LOGICAL HasCD;
    LOGICAL HasMike;
    SN_CODE AgeGroup;
    SN_CODE GenderGroup;
    SN_CODE ModemSpeedGroup;
    SN_CODE MonitorGroup;
    SN_CODE RamCodeGroup;
    SN_CODE ProcessorGroup;
} sPublicInfo;

// Game state parameters
const int PARAM_JOIN_PERMISSION   = 1;
const int PARAM_INVITE_PERMISSION = 2;
const int PARAM_WATCH_PERMISSION  = 3;
const int PARAM_JOIN_IN_PROGRESS  = 4;

// API callback function type
#define SIGSCALLBACK __cdecl

// Possible values for CheckPlayerConnectStatus()
const int _GAME_STAT_NEWPLAYER = 2;
const int _GAME_STAT_SUCCESS =  1;
const int _GAME_STAT_PENDING =  0;
const int _GAME_STAT_FAILURE = -1;

const int _NO_NEW_PLAYER_FOUND = -1;	//mf 8/2 %%%

// Information for "TellMe" functions:
// Define your callback functions as SIGSCALLBACK and include SNTypes.h

//typedef of enum used to flag notification method in "TellMe" functions
enum NotifyMethod {
	SIGS_DEFAULT_QUERY = 0,
	SIGS_CALLBACK_FUNCTION,
	SIGS_POST_MESSAGE
};
typedef NotifyMethod NOTIFY_METHOD;

//typedef for TellMeGameConnected() callback function
typedef void (*GAME_CONNECT_PROC)(LOGICAL);

//typedef for TellMeUserDisconnected() & TellMePlayerLeftGame() callback functions
typedef void (*PLAYER_STATUS_PROC)(unsigned int);

//typedef for TellMePlayerJoined() callback function
typedef void (*NEW_PLAYER_PROC)(void);

//typedef for TellMeUserPublicInfo callback function
typedef void (*PUBLIC_INFO_PROC)(struct tPublicInfo *PublicInfo);

//a pointer to this structure will be the third parameter when a message is
//posted (WPARAM) and the structure will be the second parameter when a callback
// is used
//for TellMeGameMessReceived and TellMeChatMessReceived functions
typedef struct tMessInfo
{
	int iPlayerIndex;
	int iLength;
} sMessInfo;

typedef void (*MESS_RECEIVED_PROC)(char *, sMessInfo);

enum SpecialChatType {
	SYSOP_AD = 0,
	SYSOP_BROAD,
	PRIVATE
};

typedef SpecialChatType SP_CHAT_TYPE;

typedef struct tSpecialChat
{
	char Sender[USER_NAME_LEN];
	int  MsgColor;
	SP_CHAT_TYPE MsgType;
} sSpecialChat;

typedef void (*SPECIAL_CHAT_PROC)(char *, sSpecialChat);

#endif // _SN_TYPES_H_
