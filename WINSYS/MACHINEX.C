/* ========================================================================
   Copyright (c) 1997   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: MACHINEX.C   - Adapted from MACHINE.C for the Birthright I
									 Character Editor by John Conley
   Author: Chris Phillips
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:
X   machine_pre_frame     -does preframe things, such as check the mouse
X   machine_post_frame    -does postframe things, such as updating the screen
   main                  -initializes the system and runs GameMain
   quit_sys              -de-initializes the system, and exits the program

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <conio.h>
#include <windows.h>
#include "../resource.h"
#include "../system.h"
#include "../machine.h"
#include "../machint.h"
#include "../engine.h"
#include "../debug.h"

#include "mciutil.h"

#include "../main.hxx"
#include "../sound.hxx"
#include "../fileutil.h"
//#include "game.h"

// [d11-09-96 JPC] No longer use registry. #include "registry.h"
#include "ddrawpro.h"

// name of first scene in this
extern char pInitScene[];
// extern char pwad[];
extern char chCursorName[32];

#ifdef INET
#define GLOBALS
#include "inet.h"
#endif


/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

static void ParseCmdLine (LPSTR szCmdLine);

void CheckCD(void);

LONG WINAPI MainWndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
void AppExit(void);
void WaitForMessageQueueToEmpty( void );

// void load_new_wad(char *n, LONG);
void OpenAFile(void);

int InitRedBook(void);
int ShutdownRedBook(void);


/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

LONG WindowWidth = MAX_VIEW_WIDTH;
LONG WindowHeight = MAX_VIEW_HEIGHT;

extern LONG tot_thing_spans;
extern LONG need_cursor_draw;
extern HPALETTE hpalApp;
extern char	*GDIscreen;
extern BOOL fMusic;
extern BOOL fHighRes;
//extern BOOL fAIAutoRes;
extern HKEY hKeyBirthright;

BOOL JustLoadWad=FALSE;

BOOL fSound = TRUE;			// [d11-09-96 JPC] moved here from REGISTRS.H
BOOL fMusic = TRUE;        // [d11-09-96 JPC] moved here from REGISTRS.H

#if !defined(_RELEASE)
BOOL gDontFight = FALSE;
#endif
							
extern UINT wCDDeviceID;
extern int iWhichTrack;

#if defined (OLD_SOUND)
extern MCIDEVICEID gwMCIDeviceID;   /* MCI Device ID for the AVI file */
#endif

#ifdef INET
HWND	hwndChat = NULL;
WORD wNetWork;
#endif

HWND	hwndApp;
HDC		gdcScreen;
HCURSOR hcStd;
HINSTANCE hInstApp;

int iChatKey;

HMENU	hMenuInit;

SHORT sDrawMode = iGDI;
BOOL	fDrawing = FALSE;
LONG fps;
BOOL fQuitting = FALSE;
BOOL fAppHasFocus = TRUE;

const DWORD iINITIALHEAPSIZE = (24L * 1024L * 1024L);
// const DWORD iINITIALHEAPSIZE = 0L;
HANDLE hHeap;

LONG cx, cy;
#ifdef INET
static  char  szAppName[]= "Cyberjack";
static char szClassName[] = "Cyberjack";
#else
static  char  szAppName[]= "Birthright Character Editor";
static char szClassName[] = "BRCHARED";
#endif

 // -- moved from AppInit	CAM 20 NOV 96
static LONG lStyle = WS_MINIMIZEBOX|WS_SYSMENU|WS_POPUP|WS_CAPTION;



// -------------------------------------------------------------------
//
//	CombineMouseMoveMessages
//		Peak into the message queue for WM_MOUSEMOVE messages, and
//		 throw away all but the last one.
//
//	Inputs:
//		none
//
//	Returns:	none
//									
// -------------------------------------------------------------------
void CombineMouseMoveMessages( void )
{
	MSG	msg, msg2;

	 // -- marker for if this is filled
	msg2.message = 0;

#if 1
	// Clear the message queue
	while ( !fQuitting &&
					(PeekMessage( &msg, NULL, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE) != 0))
	{
		 // -- save it in case it is the last one!
		msg2 = msg;
	}

	if ( msg2.message != 0 )
	{
		TranslateMessage( &msg2 );
		DispatchMessage( &msg2 );
	}
#else
	while ( !fQuitting &&
					(PeekMessage( &msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE) != 0))
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
#endif

} // CombineMouseMoveMessages




// void AppIdle(void)
// {
// 	if(!fQuitting && fAppHasFocus)
// 		MainLoop();
//
// 	WindowsMessages();
// }

void WindowsMessages(void)
{
	MSG	msg;

	// Clear the message queue
	while(!fQuitting && (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if(msg.message == WM_QUIT || msg.message == WM_CLOSE)
			return;
	}
}

/* ========================================================================
   Function    - ClearMessageQueue
   Description - Clear out the message queue then return
   Returns     -
   ======================================================================== */
void ClearMessageQueue(void)
{
	MSG	msg;

	// Clear the message queue
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if(msg.message == WM_QUIT || msg.message == WM_CLOSE)
			return;
	}
}

/*----------------------------------------------------------------------------*\
|   AppInit( hInst, hPrev)                                                     |
|                                                                              |
|   Description:                                                               |
|       This is called when the application is first loaded into               |
|       memory.  It performs all initialization that doesn't need to be done   |
|       once per instance.                                                     |
|                                                                              |
|   Arguments:                                                                 |
|       hInstance       instance handle of current instance                    |
|       hPrev           instance handle of previous instance                   |
|                                                                              |
|   Returns:                                                                   |
|       TRUE if successful, FALSE if not                                       |
|                                                                              |
\*----------------------------------------------------------------------------*/
#pragma off (unreferenced)
BOOL AppInit( HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw )
{
	WNDCLASS clsex;
	FILE *fp;
	char cpBuffer[256];
	LONG fileResult;
	char path[256];

	int X2, Y2;
    HWND hwndTemp;

#ifdef INET
	WORD wVersionRequested;
	WSADATA wsaData;
#endif

    //---- Is there another instance of BirthRight running
    hwndTemp = FindWindow( szClassName,	    // address of class name
                           szAppName    );	// address of window name

    if ( hwndTemp != NULL )
    {
        if ( MessageBox ( NULL,
                          "You are already running this program.\nDo you want to run another copy?",
                          szAppName,
                         MB_SYSTEMMODAL | MB_YESNO ) == IDNO )
        {
            quit_sys(0L);
    	    return FALSE;
        }
    }

#ifdef INET
	// startup winsock [MDB]
	// assume winsock will load
	wNetWork = TRUE;
	wVersionRequested = MAKEWORD(1, 1);
	if(WSAStartup(wVersionRequested, &wsaData) != 0)
		wNetWork = FALSE;
#endif

	/* Save instance handle for DialogBoxes */
	hInstApp = hInst;

	// create the heap
	hHeap = HeapCreate(HEAP_NO_SERIALIZE, iINITIALHEAPSIZE, 0);

	// Initialize any memory, time, etc statistics
	// These will be calced and printed/dumped in
	// close_statistics <see statistics.c>
	init_statistics();

	// Install a kbrd handler.. check_regions will call key_status
	// on PC this uses key_stat array. On PC keyint will fill in array
	// automaticly so it doesnt hafta be done durring machine_pre_frame
	install_keyint();

	// Install mouse handler.. check_regions will look at cursor_x
	// cursor_y and mouse_button
//	init_mouse();

   	//---- Open/load registry
	// [d11-09-96 JPC] No longer use registry.
   // OpenRegistry( &hKeyBirthright );

	CheckHandles ();							// in FOPEN.C

	if (!hPrev)
	{
		/*
		 *  Register a class for the main application window
		 */

		clsex.hCursor        = NULL;	// set this to NULL; don't assume CAM 24 NOV 96
		clsex.hIcon          = LoadIcon(hInst,"Birthright4");	 // CAM 28 AUG 96
		clsex.lpszMenuName   = NULL;
		clsex.lpszClassName  = szClassName;
		clsex.hbrBackground  = (HBRUSH) COLOR_CAPTIONTEXT;
		clsex.hInstance      = hInst;
		clsex.style          = CS_BYTEALIGNCLIENT | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_OWNDC;
		clsex.lpfnWndProc    = (WNDPROC) MainWndProc;
		clsex.cbWndExtra     = 0;
		clsex.cbClsExtra     = 0;

		if (!RegisterClass(&clsex))
			return FALSE;
	}

#if defined(INET)
	hMenuInit = LoadMenu(hInst, "AppMenu");
#else
	hMenuInit = NULL;
#endif

	 // --
	 // -- Create the windows
	 // --
	cx = 2;
	cy = GetSystemMetrics(SM_CYCAPTION) + 1 ;
	
	X2 = GetSystemMetrics(SM_CXFULLSCREEN) / 2;
	Y2 = GetSystemMetrics(SM_CYFULLSCREEN) / 2;

	if((X2 * 2) < 650)
	{
		lStyle = WS_POPUP;
		cy = 1;
	}

#if defined (_CHARED)
// [d5-07-97 JPC]
		lStyle = WS_POPUP;
		cy = 1;
#endif

	// main window
	hwndApp = CreateWindowEx (0,		// Extended style bits
							szClassName,			// Class name
							szAppName,              // Caption
							lStyle,    // Style bits
							X2 - ((cx + MAX_VIEW_WIDTH) / 2),
							0,	// Position
							MAX_VIEW_WIDTH + cx,
							MAX_VIEW_HEIGHT + cy,		// Size
							(HWND)NULL,             // Parent window (no parent)
							hMenuInit,              // use class menu
							hInst,                  // handle to window instance
							(LPSTR)NULL             // no params to pass on
							);

	DefaultMainWindowSize( 0 );	// CAM 21 NOV 96

	// get the global dc to the window
	gdcScreen = GetDC(hwndApp);

	 // -- Make the world a little more random
	srand(GetTickCount());

	
	ParseCmdLine(szCmdLine);

    // Find the installation type.
	fp=fopen("brsetup.cfg","r");
	if(fp==NULL)
	{
#if defined (_DEBUG)
			fatal_error("Unable to open brsetup.cfg file.  Game not properly installed.");
#endif
			return FALSE;
	}

	do
	{
		fileResult = GetNextLine(fp,cpBuffer,sizeof(cpBuffer));
		if(fileResult != EOF)
		{
			if(strcmp(cpBuffer,"[INSTALL_TYPE]") == 0)
			{
				fileResult = GetNextLine(fp,cpBuffer,sizeof(cpBuffer));
				InstallationType = atoi(cpBuffer);
			}
			else if(strcmp(cpBuffer,"[CD]") == 0)
			{
				fileResult = GetNextLine(fp,cpBuffer,sizeof(cpBuffer));
				strcpy(CDDrive,cpBuffer);
				strcat(CDDrive,":\\");
			}
			else if(strcmp(cpBuffer,"[INSTALL_PATH]") == 0)
				strcat(InstallPath,".\\");
			
		}
	}while(fileResult != EOF);
	fclose(fp);
// 	#ifdef _WINDOWS
// 	#ifdef _DEBUG
// 		RandomLogOpen ();
// 	#endif
// 	#endif


	// start up the redbook player
	InitRedBook();								// finds the CD
	//InitializeSoundSystem();

/*
   GameMain is the name of the MAIN for the game.  Fill it in in order to
   compile and run a application
*/
	
	//GEH GEH GEH!!!!!!!!!!!!!!!!!!!!!!!!!
	//GEH SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	

#if defined (_RELEASE)
	// make sure the cd is in	
	CheckCD();
	if(fQuitting == TRUE)
		return (FALSE);
#endif


	GameMain();

	hcStd = LoadCursor( hInstApp, chCursorName);
	SetCursor(hcStd);


	PostMessage(hwndApp, WM_SIZE, SIZE_RESTORED, MAKELPARAM(640, 480));

	// check the appropriate menu items
	//GEH obsolete CheckMenuItem(hMenuInit, ID_APP_OPTIONS_MUSIC, (fMusic == 0) ? MF_UNCHECKED : MF_CHECKED);
	//GEH obsolete CheckMenuItem(hMenuInit, ID_APP_TEST_ANIMATE, (fAIAnimate == TRUE) ? MF_CHECKED : MF_UNCHECKED);
	
	return ( TRUE );

} // AppInit
#pragma on (unreferenced)



// ===================================================================================================================
//
//	DefaultMainWindowSize
//
//		Set a good default window size
//
// ===================================================================================================================
LONG lSavedX = -1;
LONG lSavedY = -1;
void DefaultMainWindowSize( UINT flags )
{
	RECT rc;
	int XF, YF;

	WaitForMessageQueueToEmpty();

	XF = GetSystemMetrics( SM_CXFULLSCREEN );
	YF = GetSystemMetrics( SM_CYFULLSCREEN );

	rc.top = 0;			
	rc.left = 0;
	rc.right = 640;
	rc.bottom = 480;

	if ( sDrawMode == iGDI )
	{
		SetWindowLong( hwndApp, GWL_STYLE, lStyle );
		AdjustWindowRect( &rc, lStyle, FALSE );
		if ( lSavedX < 0 )
		{
			if ( XF > rc.right )
				lSavedX = (XF - rc.right) / 2;
			else
				lSavedX = 0;
			if ( YF > rc.bottom )
				lSavedY = (YF - rc.bottom) / 2;
			else
				lSavedY = 0;
		}
		SetWindowPos( hwndApp, HWND_NOTOPMOST,
								lSavedX, lSavedY,	
										(rc.right  - rc.left), (rc.bottom - rc.top), flags );
	}
	else
	{
		SetWindowLong( hwndApp, GWL_STYLE, lStyle & (~WS_CAPTION) );
		AdjustWindowRect( &rc, lStyle & (~WS_CAPTION), FALSE );

		SetWindowPos( hwndApp, HWND_NOTOPMOST, 0, 0,	
								(rc.right  - rc.left), (rc.bottom - rc.top), flags );
	}

	WaitForMessageQueueToEmpty();

} // DefaultMainWindowSize




//--------------------------------------------------------------------------;
//
//  void WaitForMessageQueueToEmpty
//
//  Description:
//		Empty the message queue.
//
//  Arguments:
//		fDoExtra		- do we want to do teh extra checks?
//
//  Return (void):
//
//--------------------------------------------------------------------------;
void WaitForMessageQueueToEmpty( void )
{
	MSG     msg;

	 // -- if we are shutting down, don't capture messages; this is so
	 // --  we don't miss the WM_QUIT message and never leave!
	if ( fQuitting )
		return;

	while ( !fQuitting &&
				(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0) )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

} // WaitForMessageQueueToEmpty



/* ========================================================================
   Function    - change the windows cursor
   Description -
   Returns     -
   ======================================================================== */
void WinCursorSet()
{
	//POINT pt;


	//---- Have to do this for the target cursor in adventures

	hcStd = LoadCursor( hInstApp, chCursorName);

	if ( hcStd != GetCursor() )
	{
		SetCursor(hcStd);
	}
	
}
	
/*----------------------------------------------------------------------------*\
|   WinMain( hInst, hPrev, lpszCmdLine, cmdShow )                              |
|                                                                              |
|   Description:                                                               |
|       The main procedure for the App.  After initializing, it just goes      |
|       into a message-processing loop until it gets a WM_QUIT message         |
|       (meaning the app was closed).                                          |
|                                                                              |
|   Arguments:                                                                 |
|       hInst           instance handle of this instance of the app            |
|       hPrev           instance handle of previous instance, NULL if first    |
|       szCmdLine       ->null-terminated command line                         |
|       cmdShow         specifies how the window is initially displayed        |
|                                                                              |
|   Returns:                                                                   |
|       The exit code as specified in the WM_QUIT message.                     |
|                                                                              |
\*----------------------------------------------------------------------------*/
int WINAPI WinMain( HINSTANCE hInstExe, HINSTANCE hInstPrev, LPSTR szCmdLine, int sw)
{
	MSG     msg;

	/* Call initialization procedure */
	if ( !AppInit( hInstExe, hInstPrev, szCmdLine, sw ) )
	{
		MessageBox(NULL, "Can't initialize Birthright Character Editor.\n"
		"Make sure the program is installed on your hard disk\n"
		"in the same directory as the BIRTHRIGHT game.",
		"Birthright", MB_OK | MB_APPLMODAL);
		return FALSE;
	}

    // Polling messages from event queue
	while ( !fQuitting && msg.message != WM_QUIT )
	{

		 // -- Look for messages
		if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} // end if a message in the queue

		// process a frame
		// AppIdle();

	} // while ! WM_QUIT

	 // -- we're done now
	AppExit();

	return msg.wParam;

} // WinMain


/* =======================================================================
   Function    - quit_sys
   Description - prints stats and removes the keyboard int, and shuts down
   Returns     - void
   ======================================================================== */
#pragma off (unreferenced)
void quit_sys(LONG arg)
{
	// WinHelp(hwndApp, "BIRTHRT.HLP", HELP_QUIT, 0);
	// StopRedBook();
	// TurnOffAllSounds();
	// ShutDownSoundSystem();
	CloseAllFiles();							// [d4-09-97 JPC]
	fQuitting = TRUE;
	// [d1-07-97 JPC] Theory: The call to PostQuitMessage already does this,
	// and doing it again leads to an infinite loop.
	// PostMessage(hwndApp, WM_CLOSE, 0L, 0L);
}
#pragma on (unreferenced)

void AppExit(void)
{
#ifdef INET
	DestroyMenu(hMenuInit);
#endif

// #ifdef _WINDOWS
// #ifdef _DEBUG
// 	RandomLogClose ();
// #endif
// #endif

	ReleaseDC(hwndApp, gdcScreen);
	// close_graph();
	// close_statistics();
	// remove_keyint();
	// TurnOffAllSounds();
	// Must be called before HeapDestroy!
	//ShutDownSoundSystem();
	// ShutdownRedBook();
	
	QuitMemManag();	// Call this before you distroy the heap.
	HeapDestroy(hHeap);
	// [d11-09-96 JPC] No longer use registry.
	// CloseRegistry(hKeyBirthright, TRUE);

#ifdef INET
	// delete internet stuff [MDB]
	if(wNetWork)
	{
		closesocket(MySock.sock);
		WSACleanup();
	}

	if(hwndChat)
		DestroyWindow(hwndChat);
#endif
}

LONG WINAPI MainWndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	BOOL fRealize;
	HDC  hdc;
	//RECT rc;
	//WORD i;
	//LONG l;
	int  iKey;

#ifdef INET
	int  iIndex;
	char outtext[256];
#endif

	switch(uMsg)
	{

      //---- If the screen saver tries to kick in don't let it

      case WM_SYSCOMMAND:
         if ( ( wParam & 0xFFF0 ) == SC_SCREENSAVE )
         {
            return 0;
         }
         break;


		case WM_QUIT:
		case WM_CLOSE:
			fQuitting = TRUE;
#ifdef _EDIT
         SendMessage (ghwndEditWallDlg, WM_CLOSE, 0, 0);
#endif
			break;


		//---- DLJ modified this because it was the right thing to do

		case WM_PAINT:
			{
			HDC hdc;
			PAINTSTRUCT ps;

			hdc = BeginPaint ( hwnd, &ps );

			SelectPalette ( hdc, hpalApp, FALSE );
			RealizePalette( hdc );			

			update_screen();

			EndPaint ( hwnd, &ps );


			break;

			}

#ifdef INET
#include "inetproc.inc"
#endif

		case WM_COMMAND:
			switch(wParam)
			{
#ifdef INET
				case ID_APP_FILE_CONNECT:
					if(DialogBox(hInstApp, "Connect", hwnd, (DLGPROC)Connect))
					{
						// we have a connection and have registered our name
						// create the chat window
						hwndChat = CreateDialog(hInstApp, "CHAT", hwndApp, (DLGPROC)Chat);


						// setup the main window to receive network messages
						if(WSAAsyncSelect(MySock.sock, hwndApp, MW_DATAREADY, FD_READ | FD_CLOSE) == SOCKET_ERROR)
						{
							// I hate it when this happens
							wsprintf(outtext, "FAILURE: WSAAsyncSelect returned %u", WSAGetLastError());
							MessageBox(hwnd, outtext, NULL, MB_OK);
							return(0);
						}
					}
					break;

				case ID_APP_FILE_DISCONNECT:
					// get rid of our chat window
					DestroyWindow(hwndChat);
					closesocket(MySock.sock);
					break;
/*
	 The CheckMenuItem function sets the state of the specified menu item's check mark attribute to either checked or unchecked.
DWORD CheckMenuItem(


    HMENU hmenu,	// handle to menu
    UINT uIDCheckItem,	// menu item to check or uncheck
    UINT uCheck	// menu item flags
   );	
Parameters
hmenu
Identifies the menu of interest.
uIDCheckItem	
Specifies the menu item whose check-mark attribute is to be set, as determined by the uCheck parameter.
uCheck
Specifies flags that control the interpretation of the uIDCheckItem parameter and the state of the menu item's check-mark attribute. This parameter can be a combination of either MF_BYCOMMAND or MF_BYPOSITION and MF_CHECKED or MF_UNCHECKED.
*/
				case ID_APP_OPTIONS_SHOWNAMES:
					gbShowPlayerNames = !gbShowPlayerNames;
//					CheckMenuItem(APPMENU, ID_APP_OPTIONS_SHOWNAMES, gbShowPlayerNames);
					break;
#endif

			//GEH Obsolete 	case ID_APP_FILE_LOAD:
			//GEH Obsolete 		OpenAFile();
			//GEH Obsolete 		break;
			//GEH Obsolete 	case ID_APP_FILE_EXIT:
			//GEH Obsolete 		PostMessage(hwndApp, WM_CLOSE, 0L, 0L);
			//GEH Obsolete 		break;
			//GEH Obsolete
			//GEH Obsolete 	case ID_APP_OPTIONS_LOWRES:
			//GEH Obsolete 		fAIAutoRes = FALSE;
			//GEH Obsolete 		CheckMenuItem(hMenuInit, ID_APP_OPTIONS_AUTO, FALSE);
			//GEH Obsolete 		set_lowres(0L);
			//GEH Obsolete 		break;
			//GEH Obsolete
			//GEH Obsolete 	case ID_APP_OPTIONS_HIGHRES:
			//GEH Obsolete 		fAIAutoRes = FALSE;
			//GEH Obsolete 		CheckMenuItem(hMenuInit, ID_APP_OPTIONS_AUTO, FALSE);
			//GEH Obsolete 		set_hires(0L);
			//GEH Obsolete 		break;
			//GEH Obsolete
			//GEH Obsolete 	case ID_APP_OPTIONS_AUTO:
			//GEH Obsolete 		fAIAutoRes = !fAIAutoRes;
			//GEH Obsolete 		CheckMenuItem(hMenuInit, ID_APP_OPTIONS_AUTO, (fAIAutoRes == 0)?MF_UNCHECKED:MF_CHECKED );
			//GEH Obsolete 		set_hires(0L);
			//GEH Obsolete 		break;
			//GEH Obsolete
			//GEH Obsolete 	case ID_APP_OPTIONS_MUSIC:
			//GEH Obsolete 		// ToggleMusic();
			//GEH Obsolete 		CheckMenuItem(hMenuInit, ID_APP_OPTIONS_MUSIC, (fMusic == 0) ? MF_UNCHECKED : MF_CHECKED);
			//GEH Obsolete 		break;
			//GEH Obsolete
			//GEH Obsolete 	case ID_APP_TEST_ANIMATE:
			//GEH Obsolete 		GAMEToggleAnimation(0);
			//GEH Obsolete 		CheckMenuItem(hMenuInit, ID_APP_TEST_ANIMATE, (fAIAnimate == TRUE) ? MF_UNCHECKED : MF_CHECKED);
			//GEH Obsolete 		break;
			}

		case WM_SETCURSOR:
			if ( hcStd != NULL &&
				  hcStd != GetCursor() )
			{
				SetCursor(hcStd);
			}
			return 1;
			


//		case WM_MOUSEMOVE:
//			GetClientRect(hwndApp, &rc);
//			cursor_x = LOWORD(lParam);
//			cursor_y = HIWORD(lParam);

			//GEH scale mouse to draw buffer
//			cursor_x = cursor_x * WindowWidth / rc.right;
//			cursor_y = cursor_y * WindowHeight / rc.bottom;
//			break;

		case WM_LBUTTONDOWN:
//			GetClientRect(hwndApp, &rc);
//			cursor_x = LOWORD(lParam);
//			cursor_y = HIWORD(lParam);

			//GEH scale mouse to draw buffer
//			cursor_x = cursor_x * WindowWidth / rc.right;
//			cursor_y = cursor_y * WindowHeight / rc.bottom;
//			need_cursor_draw = FALSE;
			mouse_button = 1;
			return 0;

		case WM_RBUTTONDOWN:
//			GetClientRect(hwndApp, &rc);
//			cursor_x = LOWORD(lParam);
//			cursor_y = HIWORD(lParam);

			//GEH scale mouse to draw buffer
//			cursor_x = cursor_x * WindowWidth / rc.right;
//			cursor_y = cursor_y * WindowHeight / rc.bottom;
//			need_cursor_draw = FALSE;
			mouse_button = 2;
			return 0;

#ifdef _EDIT
// [d7-08-96 JPC] Restore this function for _EDIT
		case WM_LBUTTONUP:
//			GetClientRect(hwndApp, &rc);
//			cursor_x = LOWORD(lParam);
//			cursor_y = HIWORD(lParam);

			//GEH scale mouse to draw buffer
//			cursor_x = cursor_x * WindowWidth / rc.right;
//			cursor_y = cursor_y * WindowHeight / rc.bottom;
//			need_cursor_draw = FALSE;
//			mouse_button = 0;
// [d6-03-96 JPC]
         EditWall (cursor_x, cursor_y, wParam);
			return 0;
#endif

//		case WM_RBUTTONUP:
//			GetClientRect(hwndApp, &rc);
//			cursor_x = LOWORD(lParam);
//			cursor_y = HIWORD(lParam);

			//GEH scale mouse to draw buffer
//			cursor_x = cursor_x * WindowWidth / rc.right;
//			cursor_y = cursor_y * WindowHeight / rc.bottom;
//			need_cursor_draw = FALSE;
//			mouse_button = 0;
//			return 0;

		case WM_DESTROY:
//			if ( sDrawMode != iGDI )
//				DDCleanup();
			quit_sys(0);
			PostQuitMessage(0);
			return 0;
//			break;

#ifdef _STATUS
      case WM_ACTIVATE:
      {
         WORD fActive = LOWORD (wParam);
         // BOOL fMinimized = (BOOL) HIWORD (wParam);
         HWND hwndAct = (HWND) lParam;

         if (fActive != WA_INACTIVE && hwndAct == hwnd)
         {
            ShowWindow (ghwndStatus, SW_SHOW);
            SetFocus (hwnd);
         }

 			break;
      }
#else
      case WM_ACTIVATE:
      {
         if(LOWORD (wParam) == WA_INACTIVE)
				fAppHasFocus = FALSE;
			else
			{
				fAppHasFocus = TRUE;
				update_screen();
			}

 			break;
      }
#endif

      case WM_ACTIVATEAPP:
         // if ( wParam &&
         //      (sDrawMode == iDDRAW ) )
         // {
         //    DDSetCooperative ( TRUE ); //---- Set us back to exclusive mode DD
         // }
         break;


		case WM_SIZE:
			if ( sDrawMode == iGDI )
			{
				//GEH for now, only allow 640,480
				WindowWidth = MAX_VIEW_WIDTH;
				WindowHeight = MAX_VIEW_HEIGHT;
				
				// WindowWidth = LOWORD(lParam);
				// WindowHeight = HIWORD(lParam);
				//
				// if(WindowWidth > MAX_VIEW_WIDTH)
				// {
				// 	WindowHeight = WindowHeight * MAX_VIEW_WIDTH / WindowWidth;
				// 	WindowWidth = MAX_VIEW_WIDTH;
				// }
				//
				// if(WindowHeight > MAX_VIEW_HEIGHT)
				// 	{
				// 	WindowWidth = WindowWidth * MAX_VIEW_HEIGHT / WindowHeight;
				// 	WindowHeight = MAX_VIEW_HEIGHT;
				// }
				//
				// 	if(!fHighRes)
				// {
				// 	WindowWidth /= 2;
				// 	WindowHeight /= 2;
				// }
			
				// set_screen_size(WindowWidth,WindowHeight);
			}
#ifdef _STATUS
         AdjustStatusBar (hwnd);       // [d5-07-96 10:34 JPC]
         ShowWindow (ghwndStatus, SW_SHOW);
         SetFocus (hwnd);
#endif
			//GEH FALL THROUGH!  break;

		case WM_PALETTECHANGED:
			if ( hwnd == (HWND) wParam )
			{
				break;
			}
			else
			{
				// fall through
			}


		case WM_QUERYNEWPALETTE:
		   hdc = GetDC( hwnd );

    		if ( hpalApp )
				SelectPalette( hdc, hpalApp, FALSE );

		   fRealize = RealizePalette( hdc );
    		ReleaseDC( hwnd,hdc );

		   if ( fRealize )
        		InvalidateRect( hwnd, NULL, TRUE );
			return ( fRealize );

#ifdef _STATUS
		case WM_MOVE:
         AdjustStatusBar (hwnd);       // [d5-07-96 10:34 JPC]
         ShowWindow (ghwndStatus, SW_SHOW);
         SetFocus (hwnd);
			break;
#endif

		case WM_DISPLAYCHANGE:
			// If the window display is changed on the fly adjust the window
			// style.
			if (LOWORD(lParam) < 650)
			{
				lStyle = WS_POPUP;
			}
			else
			{
				lStyle = WS_MINIMIZEBOX|WS_SYSMENU|WS_POPUP|WS_CAPTION;
			}
			SetWindowLong(hwnd, GWL_STYLE, lStyle);
	        ShowWindow (hwnd, SW_SHOW);
	        SetFocus (hwnd);
			break;
		case WM_CHAR:
#ifdef _EDIT
// [d6-13-96 JPC]
			EditHandleChar (wParam);
#else
			// if a dialog is active and an edit field has focus, add char to string
#if 0
			// GEH Replace with new menu code.
			i = WhichDialogItemHasFocus();
			if(sDialog.fActive && IsDialogItemEdit(i))
			{
				l = strlen(sDialog.dialogItems[i].szCaption);

				// check for backspace
				if((char) wParam == 8 && l)
				{
					sDialog.dialogItems[i].szCaption[l-1] = 0;
				}
				else if(isprint(wParam))
				{
					sDialog.dialogItems[i].szCaption[l] = (char) wParam;
					sDialog.dialogItems[i].szCaption[l+1] = 0;
				}
				return 0L;
			}
#endif
#endif
			break;

		case WM_KEYDOWN:
			{
			iKey = (int) wParam;

#ifdef _EDIT
// [d6-13-96 JPC]
         EditHandleKeydown (iKey, hwnd);
#else


#if 0
 			// GEH Fix with new menu code.
 			// if a dialog is active and an edit field has focus, add char to string
			i = WhichDialogItemHasFocus();
			if(sDialog.fActive && IsDialogItemEdit(i))
			{
				//skip
			}
			else
			if(sDialog.fActive)
			{
				HideDialog();
			}
			// put key in status array
			else
#endif
			{
				iKey &= 0xFFL;
				if (!(key_stat[iKey] & 0x80))			// if not prohibit
				{
					fAnyKeyChanged = TRUE;				// keystats is dirty
					key_stat[iKey] = 0x81;				// set key and prohibit
					iChatKey = iKey;
				}
				key_stat[iKey] |= 0x40;					// key down
			}
#endif
			}
			break;

		case WM_KEYUP:
			{
			iKey = (int) wParam;
			key_stat[iKey&0xFFL] &= 0x3F;				// clear permit and keydown
			fAnyKeyChanged = TRUE;				// keystats is dirty
			}
			break;


	}
	return DefWindowProc ( hwnd, uMsg, wParam, lParam );
}

void UpdateFPS(void)
{
} // UpdateFPS

void OpenAFile(void)
{
}

void beep(void)
{
}

static void ParseCmdLine (LPSTR szCmdLine)
{
}
