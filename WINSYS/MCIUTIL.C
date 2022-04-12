//-----------------------------------------------------------------------------
//
//		Copyright 1995/1996 by NW Synergistic Software, Inc.
//
//		MciUtil.C - Provides wrapper around the basic MCI interface 
//
//------------------------------------------------------------------------------

#include <Windows.h>
#include "MCIUTIL.H"

#include <string.h>
#include <Digitalv.h>

extern int fSound;
 
static VOID positionMovie(HWND hWnd);

//---- Globals to this file 
static	FARPROC fpOldAVIWndProc = NULL;
static	HWND hwndApp = NULL;

MCI_OPEN_PARMS mci_OpenParms;    //---- Open parameters for mci 
UINT wCDDeviceID;
UINT wAVIDeviceId = 0;
HWND        ghwndMovie;		/* window handle of the movie */
BOOL fSkipCheck = FALSE;
MCIDEVICEID gwMCIDeviceID = 0;   /* MCI Device ID for the AVI file */

#define AVI_VIDEO "avivideo"
#define GWL_WNDPROC         (-4)


BOOL CALLBACK AviProc ( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg )
	{
        case WM_LBUTTONDOWN:
		{
			PostMessage( hwndApp, WM_LBUTTONDOWN, 0, 0 );
			return ( TRUE );
		}
		break;
	}
                                     	
   	return ( CallWindowProc( (WNDPROC)fpOldAVIWndProc, hwnd, msg, wParam, lParam ) );

} // AviProc


//----------------------------------------------------------------------------
//
//  mci_Init
//   
//  Description: Initialize the mci_OpenParms structure 
//
//
//  Arguments:  void 
//
//
//  Return ( DWORD ):  MCI_NOERROR  - is no error
//                    !MCI_NOERROR  - is error; To get the error text use 
//                                    mciGetErrorString()
//
//--------------------------------------------------------------------------;

DWORD mci_Init ( void )

{

 mci_OpenParms.dwCallback       = 0;
 mci_OpenParms.wDeviceID        = 0;
 mci_OpenParms.lpstrDeviceType  = NULL;
 mci_OpenParms.lpstrElementName = NULL;
 mci_OpenParms.lpstrAlias       = NULL;

 return ( MCI_NOERROR );

}	//---- End of mci_Init()



//----------------------------------------------------------------------------
//
//	mci_OpenFile 
//  
//   
//  Description: Open a MCI device based on the extension of the filename 
//				 *.wav - wave *.mid - midi  *.vid - video etc. Sets 
//               timing format to millseconds 
//
//  Arguments:  lpszFileName - filename that needs a fully qualified path 
//
//
//  Return ( DWORD ):  MCI_NOERROR  - is no error
//                    !MCI_NOERROR  - is error; To get the error text use 
//                                    mciGetErrorString()
//
//--------------------------------------------------------------------------;

DWORD mci_OpenFile ( LPSTR lpszFileName )

{
   DWORD   dwResult = 0;

   //---- If a device is already open then close it 

   if ( mci_OpenParms.wDeviceID != 0 )
      {
      mci_Close();
      }

   //---- Set the parameter list 

   mci_OpenParms.lpstrElementName = lpszFileName;


   //---- Send off the command 

   dwResult = mciSendCommand ( 0,
                               MCI_OPEN,
                               MCI_WAIT | MCI_OPEN_ELEMENT,
                               (DWORD)(LPVOID) &mci_OpenParms );

   //---- Error ? 

   if ( dwResult == MCI_NOERROR )
      {
      //---- Set the time format ( milliseconds )

      MCI_SET_PARMS set;
   
      set.dwTimeFormat = MCI_FORMAT_MILLISECONDS;
    
      //---- Send MCI our time format 

      dwResult = mciSendCommand ( mci_OpenParms.wDeviceID,
                                  MCI_SET,
                                  MCI_WAIT | MCI_SET_TIME_FORMAT,
                                  (DWORD) (LPVOID) &set );
      }

   return ( dwResult );


}   //---- End of mci_OpenFile



//----------------------------------------------------------------------------
//
//  mci_OpenDevice 
//   
//  Description: Open a mci device 
//
//
//  Arguments: lpszDevName - device name  
//
//
//  Return ( DWORD ):  MCI_NOERROR  - is no error
//                    !MCI_NOERROR  - is error; To get the error text use 
//                                    mciGetErrorString()
//
//
//--------------------------------------------------------------------------;

DWORD mci_OpenDevice ( LPSTR lpszDevName )

{
   DWORD   dwResult;

   //---- Close device if it is open already

   if ( mci_OpenParms.wDeviceID != 0)
      {
      mci_Close();
      }

   //---- Set open parameters 

   mci_OpenParms.lpstrDeviceType = lpszDevName;

   //---- Send off the open command

   dwResult = mciSendCommand ( 0,
                               MCI_OPEN,
                               MCI_WAIT | MCI_OPEN_SHAREABLE | MCI_OPEN_TYPE,
                               (DWORD) (LPVOID) &mci_OpenParms );

   //---- Error ?

   if ( dwResult == MCI_NOERROR )
      {

      MCI_SET_PARMS set;

      set.dwTimeFormat = MCI_FORMAT_MILLISECONDS;

      dwResult = mciSendCommand ( mci_OpenParms.wDeviceID,
                                  MCI_SET,
                                  MCI_WAIT | MCI_SET_TIME_FORMAT,
                                  (DWORD)(LPVOID)&set );
      }

   return ( dwResult );

 
}	//---- End of mci_OpenDevice()



//----------------------------------------------------------------------------
//
//  mci_Close 
//   
//  Description: Close the MCI device 
//
//
//  Arguments:  void 
//
//
//  Return ( DWORD ):  MCI_NOERROR  - is no error
//                    !MCI_NOERROR  - is error; To get the error text use 
//                                    mciGetErrorString()
//
//--------------------------------------------------------------------------;

DWORD  mci_Close ( void )

{
   MCI_GENERIC_PARMS    gp;
   DWORD                dwResult;

   //---- See if a device is open 

   if ( mci_OpenParms.wDeviceID == 0 ) return ( MCI_NOERROR );

   //---- Stop the device 

   mci_Stop();

   //---- Send off the command to Close 

   dwResult = mciSendCommand ( mci_OpenParms.wDeviceID,
                               MCI_CLOSE,
                               MCI_WAIT,
                               (DWORD)(LPVOID)&gp);

   mci_OpenParms.wDeviceID = 0;


   return ( dwResult );


}	//---- End of mci_Close()

 

//----------------------------------------------------------------------------
//
//  mci_Play 
//   
//  Description: Play the current mci device 
//
//
//  Arguments: none 
//
//
//  Return ( DWORD ):  MCI_NOERROR  - is no error
//                    !MCI_NOERROR  - is error; To get the error text use 
//                                    mciGetErrorString()
//
//--------------------------------------------------------------------------;

DWORD mci_Play ( void )

{
   MCI_PLAY_PARMS   play;
   DWORD            dwResult;


   play.dwCallback = (DWORD) hwndApp;   //---- Call back handle to signal we are done
     
   //---- Send off the Play command

   dwResult = mciSendCommand ( mci_OpenParms.wDeviceID,
                               MCI_PLAY,
                               MCI_NOTIFY,
                               (DWORD)(LPVOID) &play );


   return ( dwResult );


}   //---- End of mci_Play()

//----------------------------------------------------------------------------
//
//  mci_Rewind 
//   
//  Description: Rewind to beginning of current mci device 
//
//
//  Arguments: none 
//
//
//  Return ( DWORD ):  MCI_NOERROR  - is no error
//                    !MCI_NOERROR  - is error; To get the error text use 
//                                    mciGetErrorString()
//
//--------------------------------------------------------------------------;

DWORD mci_Rewind ( void )

{
   DWORD            dwResult;
   
   dwResult = mciSendCommand ( mci_OpenParms.wDeviceID,
                               MCI_SEEK,
                               MCI_WAIT | MCI_SEEK_TO_START,
                               0 );


   return ( dwResult );


}   //---- End of mci_Rewind()



//----------------------------------------------------------------------------
//
//  mci_Stop
//   
//  Description: Stop playing current MCI device 
//
//
//  Arguments:  void 
//
//
//  Return ( DWORD ):  MCI_NOERROR  - is no error
//                    !MCI_NOERROR  - is error; To get the error text use 
//                                    mciGetErrorString()
//
//--------------------------------------------------------------------------;

DWORD mci_Stop ( void )
{

   DWORD   dwResult;

   dwResult = mciSendCommand ( mci_OpenParms.wDeviceID,
                               MCI_STOP,
                               MCI_WAIT,
                               (DWORD)(LPVOID) NULL );


   return ( dwResult );


}   //---- End of mci_Stop()



//----------------------------------------------------------------------------
//
//  mci_GetPosition
//   
//  Description:  return the current position of the MCI Device 
//
//
//  Arguments:  void 
//
//
//  Return ( DWORD ):  current position      
//      
//
//--------------------------------------------------------------------------;

DWORD mci_GetPosition ( void )

{
   MCI_STATUS_PARMS status;



   status.dwItem = MCI_STATUS_POSITION;

   if ( mciSendCommand ( mci_OpenParms.wDeviceID,
                         MCI_STATUS,
                         MCI_WAIT | MCI_STATUS_ITEM,
                         (DWORD)(LPVOID)&status ) != 0 )
      {

      return ( 0 );

      }



   return ( status.dwReturn );


}   //---- End of mci_GetPosition()

DWORD mci_OpenCD(void)
{
	DWORD dwReturn = 0L;

    MCI_OPEN_PARMS mciOpenParms;

    // Open the CD audio device by specifying the device name.
    mciOpenParms.lpstrDeviceType = "cdaudio";
    if (dwReturn = mciSendCommand( (MCIDEVICEID) NULL, MCI_OPEN,
									MCI_OPEN_TYPE, (DWORD)(LPVOID) &mciOpenParms) )
    {
        // Failed to open device. Don't close it; just return error.
        return (dwReturn);
    }

    // The device opened successfully; get the device ID.
    wCDDeviceID = mciOpenParms.wDeviceID;

	return (1L);
}

DWORD mci_CloseCD(void)
{
	return (mciSendCommand(wCDDeviceID, MCI_CLOSE, 0, (DWORD)NULL));
}


// Plays a given CD audio track using MCI_OPEN, MCI_PLAY. Returns as 
// soon as playback begins. The window procedure function for the given
// window will be notified when playback is complete. Returns 0L on
// success; otherwise, returns an MCI error code.
DWORD mci_PlayCD(HWND hWndNotify, BYTE bTrack, BYTE bStartMin, BYTE bStartSec, BYTE bStartFrame, BYTE bEndMin, BYTE bEndSec, BYTE bEndFrame)
{
    DWORD dwReturn = 0L;
    MCI_SET_PARMS mciSetParms;
    MCI_PLAY_PARMS mciPlayParms;

	// GEH Not So. fails running from CD for now, so just exit
	// return(FALSE);

	// if in network play or sound effects are off, don't use redbook sounds
	if(fSound == FALSE)
		return(FALSE);

    // Set the time format to track/minute/second/frame (TMSF).
    mciSetParms.dwTimeFormat = MCI_FORMAT_TMSF;

    if (dwReturn = mciSendCommand(wCDDeviceID, MCI_SET, 
        MCI_SET_TIME_FORMAT, (DWORD)(LPVOID) &mciSetParms))
    {
        return (dwReturn);
    } 

    // Begin playback from the given track and play until the beginning 
    // of the next track. The window procedure function for the parent 
    // window will be notified with an MM_MCINOTIFY message when 
    // playback is complete. Unless the play command fails, the window 
    // procedure closes the device.
    mciPlayParms.dwFrom = 0L;
    mciPlayParms.dwTo = 0L;
	if(bStartMin == 0 && bEndMin == 0 && bStartSec == 0 && bEndSec == 0)
	{
	    mciPlayParms.dwFrom = MCI_MAKE_TMSF(bTrack, 0, 0, 0);
    	mciPlayParms.dwTo = MCI_MAKE_TMSF(bTrack + 1, 0, 0, 0);
	    mciPlayParms.dwCallback = (DWORD) hWndNotify;

    	dwReturn = mciSendCommand(wCDDeviceID, MCI_PLAY,
        	MCI_FROM | MCI_TO | MCI_NOTIFY, (DWORD)(LPVOID) &mciPlayParms);
	}
	else
	{
	    mciPlayParms.dwFrom = MCI_MAKE_TMSF(bTrack, bStartMin, bStartSec, bStartFrame);
    	mciPlayParms.dwTo = MCI_MAKE_TMSF(bTrack, bEndMin, bEndSec, bEndFrame);
	    mciPlayParms.dwCallback = (DWORD) NULL;

    	dwReturn = mciSendCommand(wCDDeviceID, MCI_PLAY,
        	MCI_FROM | MCI_TO, (DWORD)(LPVOID) &mciPlayParms);
	}

    return (dwReturn);
}	//---- End of mci_PlayCD


DWORD mci_StopCD ( void )
{

   DWORD   dwResult;

   dwResult = mciSendCommand ( wCDDeviceID,
                               MCI_STOP,
                               MCI_WAIT,
                               (DWORD)(LPVOID) NULL );


   return ( dwResult );


}   //---- End of mci_Stop()

DWORD mci_CheckCDBusy ( void )
{

   DWORD   dwResult;
   MCI_STATUS_PARMS	mciStatusParms;

   mciStatusParms.dwItem = MCI_STATUS_MODE;
   
   dwResult = mciSendCommand ( wCDDeviceID,
                               MCI_STATUS,
                               MCI_STATUS_ITEM,
                               (DWORD)(LPVOID) &mciStatusParms );


   // returns 1 if CD is busy
   dwResult = (DWORD)(!(mciStatusParms.dwReturn & 0x00000001));
   
   return ( dwResult );


}   //---- End of mci_CheckCDBusy()

/*--------------------------------------------------------------+
| initAVI - initialize avi libraries				|
|								|
+--------------------------------------------------------------*/
BOOL mci_OpenAVI(void)
{
	MCI_DGV_OPEN_PARMS	mciOpen;
    DWORD dwReturn = 0L;
		
	/* set up the open parameters */
	mciOpen.dwCallback 		= 0L;
	mciOpen.wDeviceID 		= 0;
	mciOpen.lpstrDeviceType 	= AVI_VIDEO;
	mciOpen.lpstrElementName 	= NULL;
	mciOpen.lpstrAlias 		= NULL;
	mciOpen.dwStyle 		= 0;
	mciOpen.hWndParent 		= NULL;
		
	/* try to open the driver */
	dwReturn = mciSendCommand(0, MCI_OPEN, (DWORD)(MCI_OPEN_TYPE),
                          			(DWORD)(LPMCI_DGV_OPEN_PARMS)&mciOpen);

    // The device opened successfully; get the device ID.
    wAVIDeviceId = mciOpen.wDeviceID;

	return ( dwReturn == 0 );

} // mci_OpenAVI

/*--------------------------------------------------------------+
| termAVI - Closes the opened AVI file and the opened device    |
|           type.                                               |
|                                                               |
+--------------------------------------------------------------*/
void mci_CloseAVI(void)
{
	MCIDEVICEID        mciID;
	MCI_GENERIC_PARMS  mciClose;
	//
	// Get the device ID for the opened device type and then close
	// the device type.
//	mciID = mciGetDeviceID(AVI_VIDEO);
	mciID = wAVIDeviceId;

	mciSendCommand(mciID, MCI_CLOSE, 0L,
                   (DWORD)(LPMCI_GENERIC_PARMS)&mciClose);
}

void mci_StopAVI(void)
{
//	MCIDEVICEID        mciID;
	MCI_DGV_WINDOW_PARMS	mciWindow;
//	MCI_GENERIC_PARMS  mciStop;
	//
	// Get the device ID for the opened device type and then stop it

//	mciID = mciGetDeviceID(AVI_VIDEO);

//  gwMCIDeviceID;
//	mciSendCommand(mciID, MCI_STOP, 0L,
//                   (DWORD)(LPMCI_GENERIC_PARMS)&mciStop);

	mciSendCommand( gwMCIDeviceID, MCI_STOP, 0L,
                                (DWORD)(LPVOID) NULL );

	/* hide the playback window */

	mciWindow.dwCallback = 0L;
	mciWindow.hWnd = NULL;
	mciWindow.nCmdShow = SW_HIDE;
	mciWindow.lpstrText = (LPSTR)NULL;

	mciSendCommand( gwMCIDeviceID, 
	                MCI_WINDOW,
	                MCI_DGV_WINDOW_STATE,
		            (DWORD)(LPMCI_DGV_WINDOW_PARMS)&mciWindow);

	 // -- don't forget to close the AVI also!									
	mciSendCommand( gwMCIDeviceID, MCI_CLOSE, 0L,
                   				(DWORD)(LPMCI_GENERIC_PARMS)NULL);


} // mci_StopAVI

BOOL mci_PlayAVI(char *szFile, HWND hWnd)
{
	DWORD			dwFlags, dwError;
	MCI_DGV_PLAY_PARMS	mciPlay;
	MCI_DGV_OPEN_PARMS	mciOpen;
	MCI_DGV_WINDOW_PARMS	mciWindow;
	MCI_DGV_STATUS_PARMS	mciStatus;

	// -- save this for the AVI proc
	hwndApp = hWnd;

	/* set up the open parameters */
	mciOpen.dwCallback = 0L;
	mciOpen.wDeviceID = 0;
	mciOpen.lpstrDeviceType = NULL;
	mciOpen.lpstrElementName = szFile;
	mciOpen.lpstrAlias = NULL;
	mciOpen.dwStyle = WS_CHILD;
	mciOpen.hWndParent = hWnd;

	/* try to open the file */
	dwError = mciSendCommand( 0, MCI_OPEN,
		                (DWORD)(MCI_OPEN_ELEMENT | MCI_DGV_OPEN_PARENT | MCI_DGV_OPEN_WS ),
                		(DWORD)(LPMCI_DGV_OPEN_PARMS)&mciOpen);

	if (dwError == 0)
    {

		/* we opened the file o.k., now set up to */
		/* play it.				   */
		gwMCIDeviceID = mciOpen.wDeviceID;	/* save ID */

		/* show the playback window */
		mciWindow.dwCallback = 0L;
		mciWindow.hWnd = NULL;
		mciWindow.nCmdShow = SW_SHOW;
		mciWindow.lpstrText = (LPSTR)NULL;
		mciSendCommand( gwMCIDeviceID, MCI_WINDOW,
			            MCI_DGV_WINDOW_STATE,
			            (DWORD)(LPMCI_DGV_WINDOW_PARMS)&mciWindow);

		/* get the window handle */
		mciStatus.dwItem = MCI_DGV_STATUS_HWND;
		mciSendCommand(gwMCIDeviceID,
			MCI_STATUS, MCI_STATUS_ITEM,
			(DWORD)(LPMCI_STATUS_PARMS)&mciStatus);
		ghwndMovie = (HWND)mciStatus.dwReturn;

		fpOldAVIWndProc = (FARPROC) SetWindowLong( ghwndMovie, 
						                           GWL_WNDPROC, (DWORD) AviProc );

		/* now get the movie centered */
		positionMovie(hWnd);

		/* init to play all */
		mciPlay.dwCallback = MAKELONG(hWnd,0);
		mciPlay.dwFrom = mciPlay.dwTo = 0;
		dwFlags = MCI_NOTIFY;
		
		mciSendCommand( gwMCIDeviceID, MCI_PLAY, dwFlags,
		                (DWORD)(LPMCI_DGV_PLAY_PARMS)&mciPlay);

		return(TRUE);
	}
#ifdef _DEBUG
	else
	{
		CHAR szText[129];
		mciGetErrorString( dwError, (LPTSTR) &szText[0], sizeof(szText) );
		MessageBox( hWnd, szText, "Error opening Avi", MB_OK | MB_ICONEXCLAMATION);
	}
#endif
	return(FALSE);
}

/*--------------------------------------------------------------+
| positionMovie - sets the movie rectange <rcMovie> to be	|
|		centered within the app's window.		|
|								|
+--------------------------------------------------------------*/
static VOID positionMovie(
HWND hWnd)
{
	RECT rcMovie;		/* the rect where the movie is positioned      */
				/* for QT/W this is the movie rect, for AVI    */
				/* this is the location of the playback window */
	RECT	rcClient, rcMovieBounds;
	MCI_DGV_RECT_PARMS	mciRect;

	GetClientRect(hWnd, &rcClient);	/* get the parent windows rect */
	
	/* get the original size of the movie */
	mciSendCommand(gwMCIDeviceID, MCI_WHERE,
                  (DWORD)(MCI_DGV_WHERE_SOURCE),
                  (DWORD)(LPMCI_DGV_RECT_PARMS)&mciRect);
	CopyRect( &rcMovieBounds, &mciRect.rc );	/* get it in the movie bounds rect */

	rcMovie.left = (rcClient.right/2) - (rcMovieBounds.right / 2);
	rcMovie.top = (rcClient.bottom/2) - (rcMovieBounds.bottom / 2);
	rcMovie.right = rcMovie.left + rcMovieBounds.right;
	rcMovie.bottom = rcMovie.top + rcMovieBounds.bottom;

	/* reposition the playback (child) window */
	MoveWindow(ghwndMovie, rcMovie.left, rcMovie.top,
			rcMovieBounds.right, rcMovieBounds.bottom, TRUE);
}


// MciUtil.c

