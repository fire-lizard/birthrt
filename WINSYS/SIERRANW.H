/*
 * SierraNWAPI.h
 *
 * Purpose:
 *		Interface declarations for SierraNW.dll.
 *
 * Copyright (c) 1996 by Sierra On-Line, Inc.
 * All Rights Reserved.
 *

  ********* PRELIMINARY **************
*
*	Prototypes and defines for SIGS SierraNW.dll
*	Work on this module is in beta. SIGS reserves the right to make continuing
*	changes to the interface to improve on it's usability and features.
*	
*
******* Important notes ************
*	The calling convention for the DLL is __declspec( dllexport )
*	which means arguments are pushed on the	stack from right to left). 
*	Check your compiler switches under the following
*
*		MSCV4.0:	Build->Settings->C/C++->Calling Convention->__cdecl
*		Borland5.0a:Options->Project->32-Compiler->Calling Convention->C
*		Watcom:		Options->C++ Compiler Switches->Memory Module and Processor switches->80386 Stacked Based Calling
*
******* EXAMPLE CODE ************
*	Function prototypes have been provided for you convenience. 
*	All the SIGSAPI function are used with a GetProcAddress().
*	Use the following as an example of how you call a SIGSAPI.

	HINSTANCE DllHandle = NULL;
	lpfnInitializeDLLFunc lpfnInitializeDLL;
	int ret;

	// load the SierraNW.dll once. Don't need to do this 
	// for every function call
	DllHandle = LoadLibrary("SierraNW.dll");
	if (DllHandle != NULL)
	{
		lpfnInitializeDLL = (lpfnInitializeDLLFunc)GetProcAddress((HINSTANCE)DllHandle, "InitializeDLL");
		if (lpfnInitializeDLL != NULL)
		{
			// make the call to SierraNW to initializee the DLL
			ret = lpfnInitializeDLL("123.456.789.012", 1234, 1235);
		}
		else 
    {
			// if you want to handle the error
    }
	 }
	 else 
   {
		 // if you want to handle the error
   }
*
*	The reason for the use of GetProcAddress() is to avoid loading the DLL 
*	when your EXE starts. We need to keep the DLL available for update
*	when SNWValid.dll is called. Plus it allows the game developers a chance
*	to unload the DLL to free memory.
*/


#ifndef __SIERRANWAPI_H__
#define __SIERRANWAPI_H__

#include "SNTypes.h"

// The following is the evergrowing list of SIGSAPI's.

//************* DirectToGP() **************
// !!This one is for SIGS development only.

SIGSAPI BOOL DirectToGP(LPVOID data);
typedef BOOL (*lpfnDirectToGPFunc)(LPVOID data);


/************* InitializeDLL() **************
* 
*	Initialize the DLL, connect to the GP and rooms, connect to the game server
*	and start a game.  Connect to the opponents and create both the game and chat
*	sockets.
*
*	Input:
*		GpServerIp -	The IP address of the GP to connect to.  Returned from the
*						validation DLL
*		GpInPort, GpOutPort -	The ports the GP is listening on.  Also returned
*								from the validation DLL, SNWValid.dll
*
*	Output:
*		Status	- TRUE if successful, 
*				- FALSE if failure
*
*  Note:  This is an asychronous call.  Use CheckGameConnectStatus to see if
*     the user has completed GP processing and is ready to play a game.
*/

SIGSAPI int InitializeDLL(char *GpServerIp, long GpInPort, long GpOutPort);
typedef int (*lpfnInitializeDLLFunc)(char *GpServerIp, long GpInPort, long GpOutPort);


/************* CloseDLL() **************
*
*	Close the DLL and exit SIGS
*	If you wish to make sure all sockets and threads have been closed off
*	call this API to cleanup the SierraNW.dll
*/

SIGSAPI int CloseDLL(void);
typedef int (*lpfnCloseDLLFunc)(void);


/************* GameClose() **************
*
*	Close the current game connections and return the player to the room.  Does
*	not exit SIGS.  Use to allow players to select a new opponent.
*/

SIGSAPI int GameClose(void);
typedef int (*lpfnGameCloseFunc)(void);


/************* SuppressMsgBox() **************
*
*	Supress SIGS messages
*		***** Not Yet Implemented
*/

SIGSAPI int SuppressMsgBox(void);
typedef int (*lpfnSuppressMsgBoxFunc)(void);


/************* CheckGameConnectStatus() **************
*
*	Check to see if the player has selected an opponent and started a game.
*	Also indicates if new player has joined a game in progress.
*	
*	Output:
*		New Player:				2
*		Game Ready:				1
*		Game Still Pending:		0
*		User exit without game: -1
*/

SIGSAPI int CheckGameConnectStatus(void);
typedef int (*lpfnCheckGameConnectStatusFunc)(void);

/************* WasGameRunning() **************
*
*	Lets you know if the game was running at the time
*   you joined it.
*	
*	Output:
*		TRUE		Game was running
*		FALSE		Game was not running
*/
SIGSAPI int WasGameRunning(void);
typedef int (*lpfnWasGameRunningFunc)(void);


/************* SetGameState() **************
*
*	Changes the state of some game control variables.  
*
*   Input:  Parameter - The parameter you want to set -- one of:
*               PARAM_JOIN_PERMISSION
*               PARAM_INVITE_PERMISSION
*               PARAM_WATCH_PERMISSION (not yet implemented)
*
*           Value - Depends on the Parameter selected.
*               For PARAM_JOIN_PERMISSION:
*					1 - Nobody can join
*		            2 - No approval required
*			        3 - Any one player may allow
*				    4 - Only captain may allow
*					5 - Only game owner may allow
*			        6 - Majority vote required (not yet implemented)
*					7 - Everyone must approve (not yet implemented)
*
*               For PARAM_INVITE_PERMISSION:
*					1 - Only owner may invite (not yet implemented)
*					2 - Only captain may invite (not yet implemented)
*					3 - Any player may invite
*					4 - Anyone in room may invite (not yet implemented)
*	
*	Output:  None
*/
SIGSAPI void SetGameState(int Parameter, int Value);
typedef int (*lpfnSetGameStateFunc)(int Parameter, int Value);

/************* GetMyPlayerNumber() **************
*	Returns your own player number
*/

SIGSAPI int GetMyPlayerNumber(void);
typedef int (*lpfnGetMyPlayerNumberFunc)(void);

/************* GetNumberOfPlayers() **************
*	Returns the number of players in the game
*	Input:
*		Data - returns a pointer to a string with the highest player index 
*				currently in use
*
*	Output:
*		The number of players in the current game
*/

SIGSAPI int GetNumberOfPlayers(LPVOID data);
typedef int (*lpfnGetNumberOfPlayersFunc)(LPVOID data);

/************* GetPlayerXName() **************
*	Returns the SIGS user name of player X
*	Input:
*		playerindex - The player number of the player
*		data		- address to recieve the name of the player
*
*	Output:
*		Status
*/	
SIGSAPI int GetPlayerXName(char *data, HEADER_DATATYPE playerindex);
typedef int (*lpfnGetPlayerXNameFunc)(char *data, HEADER_DATATYPE playerindex);


/************* GetNewPlayerIndex() **************			//mf 8/2 %%%
*	Get the player index of the first inactive player
*	encountered in the player list. This should be passed 
*	to ActivateNewPlayerX() to activate the new player.
*	This function should also be called if a notification function
*	or message indicates that a new player has joined.
*
*	Output:
*		Player index value, or _NO_NEW_PLAYER_FOUND.
*/
SIGSAPI int GetNewPlayersIndex(void);
typedef int (*lpfnGetNewPlayersIndexFunc)(void);


/************* ActivateNewPlayerX() **************			//mf 8/1 %%%
*	If CheckGameConnectStatus() returns value greater than 
*	_GAME_STAT_SUCCESS, activates a player in the list of
*	game/chat connections listened on. Also, decrements 
*	the value returned by CheckGameConnectStatus(). 
*	This function should also be called if a notification function
*	or message indicates that a new player has joined.
*	
*
*	Input:
*		playerindex - The 0-based index of the player.
*
*	Output:
*		The new value of the connection status.
*/
SIGSAPI int ActivateNewPlayerX(int PlayerIndex);
typedef int (*lpfnActivateNewPlayerXFunc)(int PlayerIndex);


/************* CheckPlayerConnectStatus() **************
*	Returns the connection status of the player
*	Input:
*		PlayerIndex - The index of the player
*
*	Output:
*		The connection status (0 = connected, -1 = disconnected or not found)
*/
SIGSAPI int CheckPlayerConnectStatus(int PlayerIndex);
typedef int (*lpfnCheckPlayerConnectStatusFunc)(int PlayerIndex);


/************* CheckPlayersJoiningStatus() **************
*	Checks to see if there are any players in the process
*   of joining the game.
*
*	Output:
*		TRUE if there is currently at least 1 player in the
*			process of joining the game
*		FALSE if there are currently no players joining
*/
SIGSAPI int CheckPlayersJoiningStatus(void);
typedef int (*lpfnCheckPlayersJoiningStatusFunc)(void);



/************* SendTCPMessage() **************
*	Send a TCP message to all other players
*
*	Input:
*		data - the message to send
*		datalengthinbytes - the length of data, in bytes
*
*	Output:
*		1 = success; 0 = failure
*/
SIGSAPI int SendTCPMessage(BYTE *data, HEADER_DATATYPE datalengthinbytes);
typedef int (*lpfnSendTCPMessageFunc)(BYTE *data, HEADER_DATATYPE datalengthinbytes);


/************* WaitForTCPMessage() **************
*	Wait until you receive a game message from a player
*	Input:
*		playerindex - The index of the player who should be sending you a message
*
*	Output:
*		Status - 0 = OK, -1 = error
*
*	Note:  This function will block the entire program until a message is received.  Use
*	CheckForAnyTCPMessage and PeekTCPMessage to see if there are messages pending.
*/
SIGSAPI int WaitForTCPMessage(HEADER_DATATYPE playerindex);
typedef int (*lpfnWaitForTCPMessageFunc)(HEADER_DATATYPE playerindex);


/************* PeekForTCPMessage() **************
*	Peek on the socket to see if there is a message waiting.  If there is a message,
*	the first buflen bytes of data will be returned to you.  The information will not
*	be removed from the essage itself; you will get the entire message when you cann 
*	RecvTCPMessage.  Use the data to allocate memory, switch a CASE statement, etc.
*
*	Input:
*		data - a pointer to a buffer to put the header information.  This is your
*			memory.
*		buflen - The length of data
*		playerindex - the number of the player to peek on
*	
*	Output:
*		Status - >=0 = success (number of bytes received)
*				 -1 = error
*				 -2 = no error, but no data either 
*/
SIGSAPI int PeekForTCPMessage(BYTE *data, HEADER_DATATYPE buflen, int playerindex);
typedef int (*lpfnPeekForTCPMessageFunc)(BYTE *data, HEADER_DATATYPE buflen, int playerindex);


/************* RecvTCPMessage() **************
*	Receive a TCP message from a player
*
*	Input:
*		data - a pointer to a buffer to store the information
*		buflen - the length, in bytes, of data
*		playerindex - The number of the player to read
*
*	Output:
*		returns # of bytes received if successful, FALSE on failure
*/
SIGSAPI int RecvTCPMessage(BYTE *data, HEADER_DATATYPE buflen, int playerindex);
typedef int (*lpfnRecvTCPMessageFunc)(BYTE *data, HEADER_DATATYPE buflen, int playerindex);


/************* SendTCPPointMessage() **************
*	Send a TCP message to a specific user
*
*	Input:
*		playerIndex - the number of the player to send the message to
*		data - the message to send
*		datalengthinbytes - the length of data, in bytes
*
*	Output:
*		1 = success; 0 = failure
*/

SIGSAPI int SendTCPPointMessage(int playerIndex, BYTE *data, HEADER_DATATYPE datalengthinbytes );
typedef int (*lpfnSendTCPPointMessageFunc)(int playerIndex, BYTE *data, HEADER_DATATYPE datalengthinbytes);


/************* CheckForAnyTCPMessage() **************
*	Checks to see if any of the other players have sent a TCP message
*
*	Output:
*		0 = success
*		-1 = error
*		-2 = timeout (no error, but no data)
*/
SIGSAPI int CheckForAnyTCPMessage(void);
typedef int (*lpfnCheckForAnyTCPMessageFunc)(void);


/*
*	UDP functions - unlike TCP, there is NO GUARANTEE of a successful transmission
*	
*/
/************* SendUDPMessage() **************
*	Send a UDP message to all other players (broadcast)
*
*	Input:
*		data - the message to send
*		datalengthinbytes - the length of data, in bytes
*
*	Output:
*		1 = success; 0 = failure
*/
SIGSAPI int SendUDPMessage(BYTE *data, HEADER_DATATYPE datalengthinbytes);
typedef int (*lpfnSendUDPMessageFunc)(BYTE *data, HEADER_DATATYPE datalengthinbytes);


/************* RecvUDPMessage() **************
*	Receive a UDP message.  This function will change the value of playerindex
*   to reflect who the received message came from.
*
*	Input:
*		data - a pointer to a buffer to store the information
*		buflen - the length, in bytes, of data
*		*playerindex - will return the player index of the sender
*
*	Output:
*		returns a 1 for success, a 0 for failure
*/
SIGSAPI int RecvUDPMessage(BYTE *data, HEADER_DATATYPE buflen, int *playerindex);
typedef int (*lpfnRecvUDPMessageFunc)(BYTE *data, HEADER_DATATYPE buflen, int *playerindex);


/************* SendUDPPointMessage() **************
*	Send a UDP message to a specific user
*
*	Input:
*		playerIndex - the number of the player to send the message to
*		data - the message to send
*		datalengthinbytes - the length of data, in bytes
*
*	Output:
*		1 = success; 0 = failure
*/
SIGSAPI int SendUDPPointMessage(int playerIndex, BYTE *data, HEADER_DATATYPE datalengthinbytes );
typedef int (*lpfnSendUDPPointMessageFunc)(int playerIndex, BYTE *data, HEADER_DATATYPE datalengthinbytes );


/************* CheckForAnyUDPMessage() **************
*	Checks to see if any of the other players have sent a UDP message
*
*	Output:
*		TRUE (1) = found a message
*		FALSE (0) = no message found
*/

SIGSAPI int CheckForAnyUDPMessage(void);
typedef int (*lpfnCheckForAnyUDPMessageFunc)(void);



/************* GetGameOption() **************
*	Return the game option value associated with a specific key
*
*	Input:
*		Opt - The option you want to get the value of
*
*	Output:
*		TRUE if option found, FALSE if not, -1 if error
*		val - The value of the option
*/
SIGSAPI int GetGameOption(char *Opt, char *Val);
typedef int (*lpfnGetGameOptionFunc)(char *Opt, char *Val);


/************* InitializeCustomServer() **************
*	Custom server functions
*
*	Initialize the custom server and set the callback window and
*	message to be used when custom server messages are received
*
*	Input:
*		CallBack - The Windows handle of the window to be notified
*			when a custom server message is received
*		MsgNum - The message ID to be posted to the window when a 
*			message is received
*/
SIGSAPI int InitializeCustomServer(HWND CallBack, UINT MsgNum);
typedef int *(lpfnInitializeCustomServer)(HWND CallBack, UINT MsgNum);


/************* SendCustomServerMessage() **************
*	Send a message to the custom server
*	Input:
*		Message - The message to send
*		MsgLen - The length of the message, in bytes
*
*/
SIGSAPI int SendCustomServerMessage(char *Message, long MsgLen);
typedef int (*lpfnSendCustomServerMessageFunc)(char *Message, long MsgLen);


/************* FreeCustomServerMessage() **************
*	When a message is received from the custom server, we post message
*	MsgNum (from InitializeCustomServer) to wimdow CallBack.  One of the
*	data bytes in the PostMessage is a pointer to the memory holding the
*	actual message.  This routine is provided to free that memory, providing
*	symmetry for the system.
*
*	Input:
*		Message - a pointer to the message memory allocated when the message
*			was received
*/

SIGSAPI int FreeCustomServerMessage(char *Message);
typedef int (*lpfnFreeCustomServerMessageFunc)(char *Message);


/************* TellMeGameMessReceived() **************
*	Notify the game every time a game message is received.  Make sure to 
*	call this function with the first parameter when you no longer want
*	to receive message notification.
*   If the game requests a message be posted, the format of the posted message call is
*   PostMessage(HWND my_window, UINT MessageID, WPARAM &sMessInfo, LPARAM char*)
*   where the WPARAM parameter contains the address of an sMessInfo structure, as defined above,
*   which contains the index of the player sending the message
*   and the length of the message, and the message itself is in the LPARAM parameter.
*   If the game requests a callback function, the function is defined as a MESS_RECEIVED_PROC,
*	also defined above, and the parameters are the message buffer and the structure containing
*   the index of the player and the length of the message
*
*
*	Input:
*		notifyType - determines how the calling app will be notified 
*					 when a message is received
*			SIGS_DEFAULT_QUERY		message notification will be no longer occur; 
*								the game must explicitly Peek and Receive messages
*			SIGS_CALLBACK_FUNCTION	specifies that callback function is to be used
*			SIGS_POST_MESSAGE		specifies that windows message is to be used
*
*		my_function -	a pointer to a callback function if notifyType = SIGS_CALLBACK_FUNCTION
*						NULL if notifyType = SIGS_DEFAULT_QUERY or SIGS_POST_MESSAGE
*						
*		my_window -		a handle to a window if notifyType = SIGS_POST_MESSAGE
*						NULL if notifyType = SIGS_DEFAULT_QUERY or SIGS_CALLBACK_FUNCTION
*
*		MessageID -		an unsigned integer specifying the message to be posted if 
*						SIGS_POST_MESSAGE is used as notification method
*						this need not be specified for other notification methods
*	Output:
*		returns a 0 for success, a -1 for failure
*/

SIGSAPI int TellMeGameMessReceived(NOTIFY_METHOD notifyType, MESS_RECEIVED_PROC my_function, HWND my_window, UINT messageID = 0);			  
typedef int (*lpfnTellMeGameMessReceivedFunc)(NOTIFY_METHOD notifyType, MESS_RECEIVED_PROC my_function, HWND my_window, UINT messageID);


/************* FreeGameMessageBuffer() **************
*	When a message is received from the game, we post message
*	WM_MESSAGE_RECEIVED to window my_window, or call my_function.  One of the
*	data bytes in the PostMessage is a pointer to the memory holding the
*	actual message.  This routine is provided to free that memory, providing
*	symmetry for the system. You MUST call this function each time you finish
*	reading the game message, or the memory will not be freed. 
*
*/
SIGSAPI int FreeGameMessageBuffer(void);
typedef int (*lpfnFreeGameMessageBufferFunc)(void);



/************* TellMeChatMessReceived() **************
*	This function has been implemented but has not yet been tested.
*
*	Notify the game every time a chat message is received - so that
*   the game can post its own chats instead of using SIGS chat; game is
*   expected to use the SendChatMessage() function to send these chats
*   If the game requests chat messages be posted, the format of the posted message call is
*   PostMessage(HWND my_window, UINT MessageID, WPARAM &sMessInfo, LPARAM char*)
*   where the WPARAM parameter contains the address of an sMessInfo structure, as defined above,
*   which contains the index of the player sending the message
*   and the length of the message, and the message itself is in the LPARAM parameter.
*   If the game requests a callback function, the function is defined as a MESS_RECEIVED_PROC,
*	also defined above, and the parameters are the message buffer and the structure containing
*   the index of the player and the length of the message

*
*	Input:
*		notifyType - determines how the calling app will be notified
*					 when a chat message is received
*			SIGS_DEFAULT_QUERY		calling app will NO LONGER be notified
*								when chat message is received
*			SIGS_CALLBACK_FUNCTION	specifies that callback function is to be used
*			SIGS_POST_MESSAGE		specifies that windows message is to be used
*
*		my_function -	a pointer to a callback function if notifyType = SIGS_CALLBACK_FUNCTION
*						NULL if notifyType = SIGS_DEFAULT_QUERY or SIGS_POST_MESSAGE
*						
*		my_window -		a handle to a window if notifyType = SIGS_POST_MESSAGE
*						NULL if notifyType = SIGS_DEFAULT_QUERY or SIGS_CALLBACK_FUNCTION
*
*		MessageID -		an unsigned integer specifying the message to be posted if 
*						SIGS_POST_MESSAGE is used as notification method
*						this need not be specified for other notification methods
*	Output:
*		returns a 0 for success, a -1 for failure
*/
SIGSAPI int TellMeChatMessReceived(NOTIFY_METHOD notifyType, MESS_RECEIVED_PROC my_function, HWND my_window, UINT messageID = 0);			  
typedef int (*lpfnTellMeChatMessReceivedFunc)(NOTIFY_METHOD notifyType, MESS_RECEIVED_PROC my_function, HWND my_window, UINT messageID );


/************* SendChatMessage() **************
*
*	Send a chat message to all other players.
*	Only use this function if you have previously called TellMeChatMessReceived
*	with either a SIGS_CALLBACK_FUNCTION or a SIGS_POST_MESSAGE request.
*
*	Input:
*		data - the message to send
*		datalengthinbytes - the length of data, in bytes
*
*	Output:
*		1 = success; 0 = failure
*/
SIGSAPI int SendChatMessage(BYTE *data, HEADER_DATATYPE datalengthinbytes);
typedef int (*lpfnSendChatMessageFunc)(BYTE *data, HEADER_DATATYPE datalengthinbytes);




/************* TellMeGameConnected() **************
*	Notify the game when a user's game starts.  Make sure to 
*	call this function with the first parameter when you no longer want
*	to receive message notification.
*   If the game requests a message be posted, the format of the posted message call is
*   PostMessage(HWND my_window, UINT MessageID, WPARAM BOOL, LPARAM 0)
*   where the WPARAM parameter is set to TRUE if a game has launched successfully
*   or to FALSE if a launch was initiated but failed.  LPARAM has no significance.
*   If the game requests a callback function, the function is defined as a 
*	GAME_CONNECT_PROC, defined above, and the BOOL parameter is the same as that used
*	with the windows message described above.
*
*	Input:
*		notifyType - determines how the calling app will be notified when a game starts
*			SIGS_DEFAULT_QUERY		game start status must be checked via 
*								CheckGameConnectStatus() interface
*			SIGS_CALLBACK_FUNCTION	specifies that callback function is to be used
*			SIGS_POST_MESSAGE		specifies that windows message is to be used
*
*		my_function -	a pointer to a callback function if notifyType = SIGS_CALLBACK_FUNCTION
*						NULL if notifyType = SIGS_DEFAULT_QUERY or SIGS_POST_MESSAGE
*						
*		my_window -		a handle to a window if notifyType = SIGS_POST_MESSAGE
*						NULL if notifyType = SIGS_DEFAULT_QUERY or SIGS_CALLBACK_FUNCTION
*
*		MessageID -		an unsigned integer specifying the message to be posted if 
*						SIGS_POST_MESSAGE is used as notification method
*						this need not be specified for other notification methods
*
*	Output:
*		returns a 0 for success, a -1 for failure
*/
SIGSAPI int TellMeGameConnected(NOTIFY_METHOD notifyType, GAME_CONNECT_PROC my_function, HWND my_window, UINT messageID = 0);
typedef int (*lpfnTellMeGameConnectedFunc)(NOTIFY_METHOD notifyType, GAME_CONNECT_PROC my_function, HWND my_window, UINT messageID );


/************* TellMePlayerLeftGame() **************
*	Notify the game when a player leaves a game in progress.  Make sure to 
*	call this function with the first parameter when you no longer want
*	to receive message notification.
*   If the game requests a message be posted, the format of the posted message call is
*   PostMessage(HWND my_window, UINT MessageID, WPARAM UINT, LPARAM 0)
*   where the WPARAM parameter is equal to the index of the player that dropped out.
*	LPARAM has no significance.
*   If the game requests a callback function, the function is defined as a 
*	PLAYER_STATUS_PROC, defined above, and the UINT parameter is the same as that used
*	with the windows message described above.
*
*	Input:
*		notifyType - determines how the calling app will be notified when a player leaves
*			SIGS_DEFAULT_QUERY		player status must be checked via 
*								CheckPlayerConnectStatus() interface
*			SIGS_CALLBACK_FUNCTION	specifies that callback function is to be used
*			SIGS_POST_MESSAGE		specifies that windows message is to be used
*
*		my_function -	a pointer to a callback function if notifyType = SIGS_CALLBACK_FUNCTION
*						NULL if notifyType = SIGS_DEFAULT_QUERY or SIGS_POST_MESSAGE
*						
*		my_window -		a handle to a window if notifyType = SIGS_POST_MESSAGE
*						NULL if notifyType = SIGS_DEFAULT_QUERY or SIGS_CALLBACK_FUNCTION
*
*		MessageID -		an unsigned integer specifying the message to be posted if 
*						SIGS_POST_MESSAGE is used as notification method
*						this need not be specified for other notification methods
*
*	Output:
*		returns a 0 for success, a -1 for failure
*/
SIGSAPI int TellMePlayerLeftGame(NOTIFY_METHOD notifyType, PLAYER_STATUS_PROC my_function, HWND my_window, UINT messageID = 0);
typedef int (*lpfnTellMePlayerLeftGameFunc)(NOTIFY_METHOD notifyType, PLAYER_STATUS_PROC my_function, HWND my_window, UINT messageID );


/************* TellMeUserDisconnected() **************
*	Notify the game when the local user is disconnected.  Make sure to 
*	call this function with the first parameter when you no longer want
*	to receive message notification.
*   If the game requests a message be posted, the format of the posted message call is
*   PostMessage(HWND my_window, UINT MessageID, WPARAM UINT, LPARAM 0)
*   where the WPARAM parameter is equal to the index of the user.
*	LPARAM has no significance.
*   If the game requests a callback function, the function is defined as a 
*	PLAYER_STATUS_PROC, defined above, and the UINT parameter is the same as that used
*	with the windows message described above.
*
*	Input:
*		notifyType - determines how the calling app will be notified when a player leaves
*			SIGS_DEFAULT_QUERY		user status must be checked via 
*								CheckPlayerConnectStatus() interface
*			SIGS_CALLBACK_FUNCTION	specifies that callback function is to be used
*			SIGS_POST_MESSAGE		specifies that windows message is to be used
*
*		my_function -	a pointer to a callback function if notifyType = SIGS_CALLBACK_FUNCTION
*						NULL if notifyType = SIGS_DEFAULT_QUERY or SIGS_POST_MESSAGE
*						
*		my_window -		a handle to a window if notifyType = SIGS_POST_MESSAGE
*						NULL if notifyType = SIGS_DEFAULT_QUERY or SIGS_CALLBACK_FUNCTION
*
*		MessageID -		an unsigned integer specifying the message to be posted if 
*						SIGS_POST_MESSAGE is used as notification method
*						this need not be specified for other notification methods
*
*	Output:
*		returns a 0 for success, a -1 for failure
*/
SIGSAPI int TellMeUserDisconnected(NOTIFY_METHOD notifyType, PLAYER_STATUS_PROC my_function, HWND my_window, UINT messageID = 0);
typedef int (*lpfnTellMeUserDisconnectedFunc)(NOTIFY_METHOD notifyType, PLAYER_STATUS_PROC my_function, HWND my_window, UINT messageID );


/************* TellMePlayerJoined() **************
*	Notify the game when a player joins a game already in progress.  Make sure to 
*	call this function with the first parameter when you no longer want
*	to receive message notification.
*   If the game requests a message be posted, the format of the posted message call is
*   PostMessage(HWND my_window, UINT MessageID, WPARAM 0, LPARAM 0)  The PARAMs have no
*	significance.  Note that API functions GetNewPlayerIndex(), GetPlayerXName(),
*	ActivateNewPlayerX(), etc. must still be called in order to properly process the
*	new player data.
*   If the game requests a callback function, the function is defined as a 
*	NEW_PLAYER_PROC, defined above, the preceding notes regarding processing of the new
*	player data apply here as well.
*
*	Input:
*		notifyType - determines how the calling app will be notified when a player leaves
*			SIGS_DEFAULT_QUERY		player status must be checked via 
*								CheckGameConnectStatus() interface
*			SIGS_CALLBACK_FUNCTION	specifies that callback function is to be used
*			SIGS_POST_MESSAGE		specifies that windows message is to be used
*
*		my_function -	a pointer to a callback function if notifyType = SIGS_CALLBACK_FUNCTION
*						NULL if notifyType = SIGS_DEFAULT_QUERY or SIGS_POST_MESSAGE
*						
*		my_window -		a handle to a window if notifyType = SIGS_POST_MESSAGE
*						NULL if notifyType = SIGS_DEFAULT_QUERY or SIGS_CALLBACK_FUNCTION
*
*		MessageID -		an unsigned integer specifying the message to be posted if 
*						SIGS_POST_MESSAGE is used as notification method
*						this need not be specified for other notification methods
*	Output:
*		returns a 0 for success, a -1 for failure
*/
SIGSAPI int TellMePlayerJoined(NOTIFY_METHOD notifyType, NEW_PLAYER_PROC my_function, HWND my_window, UINT messageID = 0);
typedef int (*lpfnTellMePlayerJoinedFunc)(NOTIFY_METHOD notifyType, NEW_PLAYER_PROC my_function, HWND my_window, UINT messageID );
						 
/************* TellMeUserPublicInfo() **************
*	Asks the server to provide the public information for the user specified.
*     Registers the callback function or HWND to be notified when the server
*     receives the message.
*
*	Input:
*		notifyType - determines how the calling app will be notified when a player leaves
*			SIGS_DEFAULT_QUERY		player status must be checked via 
*								? interface
*			SIGS_CALLBACK_FUNCTION	specifies that callback function is to be used
*			SIGS_POST_MESSAGE		specifies that windows message is to be used
*
*		my_function -	a pointer to a callback function if notifyType = SIGS_CALLBACK_FUNCTION
*						NULL if notifyType = SIGS_DEFAULT_QUERY or SIGS_POST_MESSAGE
*						
*		my_window -		a handle to a window if notifyType = SIGS_POST_MESSAGE
*						NULL if notifyType = SIGS_DEFAULT_QUERY or SIGS_CALLBACK_FUNCTION
*
*		MessageID -		an unsigned integer specifying the message to be posted if 
*						SIGS_POST_MESSAGE is used as notification method
*						this need not be specified for other notification methods
*
*       UserName -      The name of the user for which you are requesting the
*                       information.  The information will come back from the server and
*                       the callback routine called or a message posted to the HWND.  In
*                       either case, the reply information will be contained in a tPublicInfo
*                       struct (see SNTypes.h).
*	Output:
*		returns a 1 for success, a 0 for failure
*/
SIGSAPI int TellMeUserPublicInfo(NOTIFY_METHOD notifyType, PUBLIC_INFO_PROC my_function, HWND my_window, UINT messageID = 0, char *UserName=NULL);
typedef int (*lpfnTellMeUserPublicInfoFunc)(NOTIFY_METHOD notifyType, PUBLIC_INFO_PROC my_function, HWND my_window, UINT messageID , char *UserName );

/************* SendUserComplaint() **************
*	Send a complaint message to the GP server.  The message is logged.
*	Input:
*       Offender - The user name of the person your user is complaining
*                  about
*		Complaint - The complaint
*
*/
SIGSAPI int SendUserComplaint(char *Offender, char *Complaint);
typedef int (*lpfnSendUserComplaintFunc)(char *Offender, char *Complaint);


/************* TellMeSpecialChatReceived() **************
*	Notify the game every time a special chat message is received
*   from the SYSOP.  If you do not register this function and are
*   not using SIGS game chat, these messages will be displayed in the
*   form of modal dialog boxes.
*
*   If the game requests a message be posted, the format of the posted message call is
*   PostMessage(HWND my_window, UINT MessageID, WPARAM &sMessInfo, LPARAM char*)
*   where the WPARAM parameter contains the address of an sMessInfo structure,
*   as defined above, which contains
*   the length of the message, and the message itself is in the LPARAM parameter.
*   If the game requests a callback function, the function is defined as a MESS_RECEIVED_PROC,
*	also defined above, and the parameters are the message buffer and the structure containing
*   the index of the player and the length of the message
*
*
*	Input:
*		notifyType - determines how the calling app will be notified 
*					 when a message is received
*			SIGS_DEFAULT_QUERY		message notification will be no longer occur; 
*								the game must explicitly Peek and Receive messages
*			SIGS_CALLBACK_FUNCTION	specifies that callback function is to be used
*			SIGS_POST_MESSAGE		specifies that windows message is to be used
*
*		my_function -	a pointer to a callback function if notifyType = SIGS_CALLBACK_FUNCTION
*						NULL if notifyType = SIGS_DEFAULT_QUERY or SIGS_POST_MESSAGE
*						
*		my_window -		a handle to a window if notifyType = SIGS_POST_MESSAGE
*						NULL if notifyType = SIGS_DEFAULT_QUERY or SIGS_CALLBACK_FUNCTION
*
*		MessageID -		an unsigned integer specifying the message to be posted if 
*						SIGS_POST_MESSAGE is used as notification method
*						this need not be specified for other notification methods
*	Output:
*		returns a 0 for success, a -1 for failure
*/

SIGSAPI int TellMeSpecialChatReceived(NOTIFY_METHOD notifyType, SPECIAL_CHAT_PROC my_function, HWND my_window, UINT messageID = 0);			  
typedef int (*lpfnTellMeSpecialChatReceivedFunc)(NOTIFY_METHOD notifyType, SPECIAL_CHAT_PROC my_function, HWND my_window, UINT messageID );



/************* HideDLL() **************
*	Hides the Gathering Place UI
*/
SIGSAPI void HideGatheringPlace(void);
typedef void (*lpfnHideGatheringPlaceFunc)( void );

/************* HideDLL() **************
*	Shows the Gathering Place UI
*/
SIGSAPI void ShowGatheringPlace(void);
typedef void (*lpfnShowGatheringPlaceFunc)( void );

/************* HideDLL() **************
*	Hides the Gathering Place UI
*/
SIGSAPI void ToggleCommExit(int DoIt);
typedef void (*lpfnToggleCommExitFunc)( int );

#endif //__SIERRANWAPI_H__
