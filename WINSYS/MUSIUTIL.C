//------------------------------------------------------------------------------
//
//		Copyright 1995/1996 by NW Synergistic Software, Inc.
//
//		MusiUtil.C - Provides interface for playing mds files.
//
//------------------------------------------------------------------------------

#include <windows.h>
#include "mds.h"
#include "mciutil.h"
#include "MusiUtil.h"


#define MDS_MUSIC 1
#define MID_MUSIC 2

static HANDLE hMds;

 // -- Include application specific stuff
#include "MusiUtiS.h"


//---- MDS routines 

DWORD LoadMDSImage(HANDLE *hImage, PBYTE pbImage, DWORD cbImage, DWORD fdw);
DWORD FreeMDSImage(HANDLE hImage);
DWORD PlayMDS(HANDLE hImage, DWORD fdw);
DWORD PauseMDS(HANDLE hImage);
DWORD StopMDS(HANDLE hImage);


static int iMusicType;           //---- Midi Streams or plain midi 




//----------------------------------------------------------------------------
//  ToggleMusic
//
//  Description:
//
//		Toggle music state 
//
//  Arguments:
//
//
//  Return : 
//      
//
//----------------------------------------------------------------------------
void ToggleMusic(void)
{

   if ( fMusic == TRUE )
      {
      PauseMusic();
      }
   else
      {
      StartMusic();
      }         

}   //---- End of ToggleMusic()



//----------------------------------------------------------------------------
//  StopMusic
//
//  Description:
//
//		Stop playing music 
//
//  Arguments:
//
//
//  Return : 
//      
//
//----------------------------------------------------------------------------

void StopMusic (void)
{
    fMusic = FALSE;        //---- Stop Music

    if ( iMusicType == MDS_MUSIC )
    {
        StopMDS ( hMds );
    }
    else
    {
        mci_Stop();
    }
}


//----------------------------------------------------------------------------
//  StartMusic
//
//  Description:
//
//		Start playing music 
//
//  Arguments:
//
//
//  Return : 
//      
//
//----------------------------------------------------------------------------

void StartMusic (void)
{

    fMusic = TRUE;        //---- Start Music


    if ( iMusicType == MDS_MUSIC )
    {
        PlayMDS( hMds, MDS_F_LOOP );
    }
    else
    {
        mci_Play();
    }
}   //---- End of StartMusic()



//----------------------------------------------------------------------------
//  PauseMusic
//
//  Description:
//
//		Pause playing music 
//
//  Arguments:
//
//
//  Return : 
//      
//
//----------------------------------------------------------------------------
void PauseMusic (void)
{
    fMusic = FALSE;        //---- Stop Music

    if ( iMusicType == MDS_MUSIC )
    {
        PauseMDS( hMds );
    }
    else
    {
        mci_Stop();
    }


}   //---- End of PauseMusic()


//----------------------------------------------------------------------------
//  RestoreMusic
//
//  Description:
//
//		Restore the former music stat.
//
//  Arguments:
//
//
//  Return : 
//      
//
//----------------------------------------------------------------------------
void RestoreMusic (void)
{

    if ( fMusic )
    {

        if ( iMusicType == MDS_MUSIC )
        {
	        PlayMDS( hMds, MDS_F_LOOP );
        }
        else
        {
            mci_Play();  
        }

    }


}   //---- End of RestoreMusic()


//----------------------------------------------------------------------------
//  InitMusic
//
//  Description:
//
//		Initialize music either midi stream or just midi
//
//  Arguments:
//
//
//  Return : 
//      
//
//----------------------------------------------------------------------------
void InitMusic ( PSZ pszFilename, PSZ pszDir, BOOL fGeneralMidi ) 
{
    CHAR   szCurDir[MAX_PATH];
    LPSTR  lpszCurDir     = &szCurDir[0];
    CHAR   szTemp[MAX_PATH];
    DWORD  dwResult;
    char   *p;


    iMusicType = MDS_MUSIC;				  

	//---- Create the filename

    //---- Get the current processes directory  

	strcpy ( szCurDir, pszDir );
    
    //---- Only add the \ if its not at the end already

    if ( szCurDir[strlen(szCurDir) - 1] != '\\' )
        strcat ( szCurDir, "\\" );

    //---- add the filename to the current directory

    strcat ( szCurDir, "mds\\" );
    strcat ( szCurDir, pszFilename );

	if ( fGeneralMidi )
	    strcat ( szCurDir, "gm.mds" );
	else
	    strcat ( szCurDir, ".mds" );

    strcpy ( szTemp, szCurDir );


    if ( fWinNT == FALSE )
    {

        //---- Open the MCI device based on file .ext

        dwResult = LoadMDSImage( &hMds,
                                 (PUCHAR) lpszCurDir,
                                 0,
                                 MDS_F_FILENAME );
    }
    else
    {
        dwResult = MDS_ERR_NOMEM;   //---- Force using mciutil because mds doesn't work
    }


    //---- If there was an error try mci plain midi files  

    if ( dwResult != MDS_SUCCESS )
    {
//        debugf ( "LoadMDSImage - %s", szCurDir );

        strcpy ( szCurDir, szTemp );

        p = strstr ( szCurDir, ".mds" );

   	    strcpy ( p, ".mid" );

        //---- Try Midi 

        mci_Init();

        if ( mci_OpenFile( lpszCurDir ) == MCI_NOERROR )
        {
            iMusicType = MID_MUSIC;

            if ( fMusic == TRUE )
            {
                dwResult = mci_Play(); 
            }

        }

        return;

    }
    else
    {

        iMusicType = MDS_MUSIC;

        //---- Start playing the MCI device 
   
        if ( fMusic == TRUE )
        {                                     
            dwResult = PlayMDS( hMds, MDS_F_LOOP );
        }

    }


}   //---- End of InitMusic()




//----------------------------------------------------------------------------
//  FinalMusic
//
//  Description:
//
//		Get rid of music. Cleanup etc. 
//
//  Arguments:
//
//
//  Return : 
//      
//
//----------------------------------------------------------------------------

void FinalMusic( void )
{
    if ( iMusicType == MDS_MUSIC )
    {
        StopMDS( hMds );
        FreeMDSImage( hMds );
    }
    else
    {
        mci_Stop();
        mci_Close();
    }


}   //---- End of FinalMusic()


// MusiUtil