//-----------------------------------------------------------------------------
//
//        Copyright 1995/1996 by NW Synergistic Software, Inc.
//
//        SndUtil.c - Provides interface for playing wave files.
//
//------------------------------------------------------------------------------

#include <windows.h>
#include <windowsx.h>

#include <mmreg.h>
#include <msacm.h>
#include <dsound.h>

#include <stdio.h>
#include <string.h>
#include <memory.h>

#include "WAVE.H"
#include "SNDUTIL.H"
#include "SWINUTIL.H"

extern BOOL fSound;

typedef struct
{
	DWORD   dwDelay;                   //---- Delay between sounds
	DWORD   dwLastTick;                //---- Last tickcount played
}   WAVECLASS;


typedef struct
{
    UCHAR                szWaveName[9]; //---- wave name without extension
    DWORD                dwDelay;       //---- time delay before this sound can be replayed
    UINT                 fuSound;       //---- SND_NOSTOP don't interrup a sound ( sndPlaySound )
    BOOL                 fSetPos;       //---- Whether the sound will rewind to position 0 when 
                                        //     already playing, upon another play request
    BOOL                 fSpeech;       //---- This wave is speech 

    //----------------------------------- USED BY CODE ONLY

    BOOL                 fLoaded;       //---- Whether this is a valid wave 
    DWORD                dwLastTick;    //---- Last tickcount played
    LPBYTE               lpRawWave;     //---- pointer to raw wave data  ( sndPlaySound Only )
    LPBYTE               lpDSWave;      //---- pointer to parsed wave data
    LPWAVEFORMATEX       lpwfx;         //---- buffer format use in creating DS buffer
    LPDIRECTSOUNDBUFFER  lpDSBuffer;    //---- Secondary direct sound buffer structure
    DWORD                dwDataSize;    //---- Size of parsed wave data         
}   WAVEINFO;

#define MAX_WAVE_CLASS    3     //---- Number of wave classes + 1 ( o is not used )
WAVECLASS WaveClass [ MAX_WAVE_CLASS ] = { 0,      0,     //---- Never use this
                                           1000,   0,     //---- one second 
                                           2000,   0  };  //---- two seconds 

#include "SNDUTILS.H"


static LPDIRECTSOUND        lpDS        = NULL;        //---- Direct sound object
static LPDIRECTSOUNDBUFFER  lpDSPrimary = NULL;        //---- Primary direct sound buffer 

int			iSoundType		= 0;
static HINSTANCE	hSound			= NULL;
//static HCURSOR		ghWaitCursor	= NULL;
//static BOOL			fSoundEnable	= TRUE;      //---- This varible tells whether sounds should be made
                               //---- This has nothing to do with whether sound is enable on 
                               //---- the menu or not 

typedef HRESULT (__stdcall *FP_DSND)( IID FAR *, LPDIRECTSOUND *, IUnknown FAR * );
static FP_DSND        fp_DSnd = NULL;


 // -- local prototype
void DSError					( MMRESULT mmResult );
BOOL InitializeWaveDevice	( HWND hwndApp );
BOOL SetWavePan				( USHORT usWave, int Pan );
BOOL SetWaveFreq				( USHORT usWave, DWORD dwFreq );




//----------------------------------------------------------------------------
//  ShowWaitCursor
//
//  Description:
//
//        Throw up the wait cursor 
//
//  Arguments:
//
//
//  Return : 
//
//----------------------------------------------------------------------------
HCURSOR ShowWaitCursor ( void ) 
{
//    HCURSOR hPrev;

//    if ( ghWaitCursor == NULL )
//    {
//        ghWaitCursor =  LoadCursor( NULL,
//                                    IDC_WAIT );
//    }

//    hPrev = SetCursor ( ghWaitCursor );

//    return ( hPrev );
	  return 0;

} //---- End of ShowWaitCursor()




//----------------------------------------------------------------------------
//  RestoreCursor
//
//  Description:
//
//        Restore the previous cursor
//
//  Arguments:
//
//
//  Return : 
//      
//
//----------------------------------------------------------------------------
void RestoreCursor ( HCURSOR hPrevCur )
{
//     SetCursor ( hPrevCur );
} // RestoreCursor




//----------------------------------------------------------------------------
//  InitializeWaveDevice
//
//  Description:
//
//        be sure wave method is ready for use 
//
//  Arguments:
//		hwndApp		- application window handle
//
//  Return : 
//      
//
//----------------------------------------------------------------------------
BOOL InitializeWaveDevice ( HWND hwndApp )
{
    MMRESULT        mmResult;
    DSBUFFERDESC    dsbdesc;
    DSBCAPS         dsbcaps;
   
	 // --
    //---- Always try direct sound if it doesn't work use sndPlaySound
	 // --

    hSound = LoadLibrary ( "dsound.dll" );

    if ( hSound )
    {
		 // --
		 //---- Need proc address for create 
		 // --

		if ( !fp_DSnd )
		{
			fp_DSnd = (FP_DSND) GetProcAddress( hSound, "DirectSoundCreate" );
		}

		 //---- Couldn't get proc address so go to sound play sound
		if ( fp_DSnd )
		{
			iSoundType = DIRECT_SOUND;

			 //---- Create direct sound object 
			if (( mmResult = (fp_DSnd)( NULL, &lpDS, NULL ) ) == DS_OK )
			{
				 //---- Set cooperative level
				mmResult = IDirectSound_SetCooperativeLevel( lpDS, hwndApp, DSSCL_NORMAL );
				if( mmResult == DS_OK )
				{
//                    PCMWAVEFORMAT pcmwf;

                    // Set up wave format structure. 11 8 mono

//                    memset( &pcmwf, 0, sizeof(PCMWAVEFORMAT) );
//                    pcmwf.wf.wFormatTag = WAVE_FORMAT_PCM;
//                    pcmwf.wf.nChannels = 1;
//                    pcmwf.wf.nSamplesPerSec = 22050L;
//                    pcmwf.wf.nBlockAlign = 1;
//                    pcmwf.wf.nAvgBytesPerSec = pcmwf.wf.nSamplesPerSec * pcmwf.wf.nBlockAlign;
//                    pcmwf.wBitsPerSample = 8;


					 //---- Set up the primary direct sound buffer. 
					memset( &dsbdesc, 0, sizeof(DSBUFFERDESC) );
					dsbdesc.dwSize = sizeof(DSBUFFERDESC);

					dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
					dsbdesc.dwBufferBytes = 0;                    //---- determined by hardware
					dsbdesc.lpwfxFormat = NULL; // (LPWAVEFORMATEX)&pcmwf; //---- 11 8 mono

					mmResult = IDirectSound_CreateSoundBuffer( lpDS, &dsbdesc, 
																			&lpDSPrimary, NULL );
					if ( mmResult == DS_OK )
					{
						 //---- Device properties  Just for <G>

						mmResult = IDirectSoundBuffer_GetCaps( lpDSPrimary, &dsbcaps );

						 // -- Need to start playing the primary buffer for
						 // --  smooth mixing/starting
						mmResult = IDirectSoundBuffer_Play( lpDSPrimary, 
																		0, 0, DSBPLAY_LOOPING );
						fSound = TRUE;
						return TRUE;

					} // end if created primary buffer

				} // end if can set CooperativeLevel
			} // end if can create DirectSound device


			 //---- Someone is using the wave device
			if ( mmResult == DSERR_ALLOCATED )
			{   
				CHAR szText[256];
#if defined (_FRENCH)
				sprintf ( szText, 
                          "%s%s%s", 
                          "Une autre application utilise le périphérique sonore.\n",
                          "Pour restaurer le son de Thexder, quittez l'application\n",
                          "incriminé puis minimisez et maximisez Thexder." );
#elif defined (_GERMAN)
				sprintf ( szText, 
                          "%s%s%s", 
                          "Die Soundkarte wird von einer anderen Anwendung benutzt.\n",
                          "Um den Sound von Thexder wieder zu hören, schließen Sie die\n",
                          "störende Anwendung. Dann minimieren und maximieren Sie Thexder." );
#else
				sprintf ( szText, 
                          "%s%s%s", 
                          "Another application is using the sound device.\n",
                          "To restore Thexder sound exit the offending application\n",
                          "and then minimize and maximize Thexder." );
#endif
				MessageBox( hwndApp, szText, 
                            NULL, MB_OK | MB_ICONEXCLAMATION );

			}
            
			DSError ( mmResult ); 

		} // end if could get proc address

	} // end if DSound.dll found

	 // -- fall-back position
	iSoundType = SNDPLAY_SOUND;

//	if ( KillSplash() )
	{
#if defined (_FRENCH)
		MessageBox( hwndApp,"Mélange sonore non supporté.", 
                    		"Direct Sound", MB_OK | MB_ICONINFORMATION );
#elif defined (_GERMAN)
		MessageBox( hwndApp,"Sound-Mixing wird nicht unterstützt.", 
                    		"Direct Sound", MB_OK | MB_ICONINFORMATION );
#else
		MessageBox( hwndApp,"Sound mixing not supported.", 
                    		"Direct Sound", MB_OK | MB_ICONINFORMATION );
#endif
	}

    return FALSE;
} // InitializeWaveDevice




BOOL CreateSDS( USHORT   usWave )            //---- Wave number 
{
    DSBUFFERDESC    dsbd;
    DWORD           dwDataSize = 0;
    DWORD           dw;
    MMRESULT        mmResult;
    LPBYTE          lpData = NULL;
    DWORD           dwBSize;
    LPVOID          lpWrapPtr;
    DWORD           dwWrapBSize;
    UINT            cbActualRead = 0;


    dwDataSize = WaveInfo[usWave].dwDataSize;

    //---- Set up a secondary direct sound buffer. 

    memset( &dsbd, 0, sizeof(DSBUFFERDESC) );

    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_CTRLDEFAULT | DSBCAPS_STATIC;  //---- Buffer capabilities
    dsbd.dwBufferBytes = dwDataSize;

            
    //---- Setup buffer format first 

    dw = WaveInfo[usWave].lpwfx->cbSize + sizeof(WAVEFORMATEX);

    if (( dsbd.lpwfxFormat = (LPWAVEFORMATEX)GlobalAllocPtr(GPTR|GMEM_ZEROINIT, dw)) == NULL )
    {
       return FALSE;
    }


    //---- Setup the format, frequency, volume, etc.

    CopyMemory( dsbd.lpwfxFormat, WaveInfo[usWave].lpwfx, dw );

    //---- Create secondary buffer 

    if ( ( mmResult = IDirectSound_CreateSoundBuffer( lpDS, &dsbd,
                                               &WaveInfo[usWave].lpDSBuffer,
                                               NULL ) ) != DS_OK )
    {
        DSError ( mmResult );

        return FALSE;
    }


    //----- Release temp format buffer used in creation of DS Buffer 

    if ( dsbd.lpwfxFormat != NULL )
    {
        GlobalFree( dsbd.lpwfxFormat );
    }


    //---- Need to lock the buffer so that we can write to it!

    if ( ( mmResult = 
				IDirectSoundBuffer_Lock( WaveInfo[usWave].lpDSBuffer, 0,
                                                         dwDataSize,
                                                         (LPLPVOID)&lpData,
                                                         &dwBSize,
                                                         &lpWrapPtr,
                                                         &dwWrapBSize,
                                                         0L ) )       != DS_OK )
    {
        DSError ( mmResult );
            
        return FALSE;
    } 
    else 
    {
         dwDataSize = dwBSize;
    }

    CopyMemory( lpData, WaveInfo[usWave].lpDSWave, dwDataSize );


    //---- Unlock secondary DS buffer 

    IDirectSoundBuffer_Unlock( WaveInfo[usWave].lpDSBuffer,
														&lpData, cbActualRead, NULL, 0 );


    return TRUE;
} // CreateSDS




//----------------------------------------------------------------------------
//  LoadWaveData
//
//  Description:
//
//        Load the wave files into memory.
//
//  Arguments:
//
//
//  Return : 
//      
//
//----------------------------------------------------------------------------
BOOL LoadWaveData( USHORT usWave, PSZ pszFirstDir, PSZ pszSecondDir  )            //---- Wave number 
{
    CHAR   sz1Dir[MAX_PATH];              //--- first directory
    CHAR   sz2Dir[MAX_PATH];              //--- second directory
    //LPSTR  lpsz1Dir     = &sz1Dir[0];
    //LPSTR  lpsz2Dir     = &sz2Dir[0];

    //LPBYTE          lpData    = NULL;
    //LPWAVEHDR       lpWaveHdr = NULL;
    DWORD           dwDataSize = 0;


    //---- If null wave return

    if ( usWave == WAV_NULL ) 
    {
        return TRUE;
    }

    //---- Get the current processes directory
	MakeQualifiedFilename( sz1Dir, pszFirstDir, "Wave", (char *)WaveInfo[usWave ].szWaveName, ".wav" );
	MakeQualifiedFilename( sz2Dir, pszSecondDir, "Wave", (char *)WaveInfo[usWave ].szWaveName, ".wav" );


    //---- For sndPlaySound, we just read the whole file into a buffer
	if ( iSoundType == SNDPLAY_SOUND )
	{
		HANDLE  hFile;

		 //---- Only load it again if we don't have it in memory 
		if ( WaveInfo[usWave].lpRawWave == NULL )
		{
			hFile = CreateFile( sz1Dir, 
                                GENERIC_READ, 
                                FILE_SHARE_READ,
                                NULL,
                                OPEN_EXISTING, 
                                FILE_ATTRIBUTE_NORMAL, 
                                NULL );

			if ( hFile == INVALID_HANDLE_VALUE )
			{
//                debugf ( "LoadWave - %s", sz1Dir );

				hFile = CreateFile( sz2Dir, 
                                    GENERIC_READ, 
                                    FILE_SHARE_READ,
                                    NULL,
                                    OPEN_EXISTING, 
                                    FILE_ATTRIBUTE_NORMAL, 
                                    NULL );

				if ( hFile == INVALID_HANDLE_VALUE )
				{
//                    debugf ( "LoadWave - %s", sz2Dir );
					return( FALSE );
				}
			}
        
			dwDataSize = GetFileSize( hFile, NULL );

			if ( (dwDataSize == 0xFFFFFFFF) || (dwDataSize == 0) )
			{
				CloseHandle(hFile);
				return( FALSE );
			}

			 //---- Allocate memory for the waveform data. 
			WaveInfo[usWave].lpRawWave = (LPBYTE) HeapAlloc( GetProcessHeap(), 0, dwDataSize );
			if ( WaveInfo[usWave].lpRawWave == NULL )
			{
				CloseHandle( hFile );
				return( FALSE );
			}

			//---- Read the whole wave file in 
			ReadFile( hFile, WaveInfo[usWave].lpRawWave, dwDataSize, &dwDataSize, NULL );

			CloseHandle(hFile);
		} // end if we need to load the raw waveform

	} 
	else 
	{
		// --
		//---- DirectSound 
		// --

		HMMIO           hmmioIn;    
		MMCKINFO        ckInRiff;
		MMCKINFO        ckIn;
		UINT            cbSize;
		MMRESULT        mmResult;
		UINT            cbActualRead = 0;

        //---- Don't load it if is already ( just setup the DS Buffers )
		if ( WaveInfo[usWave].lpDSWave == NULL )
		{
			WaveInfo[usWave].lpwfx = NULL;
			cbSize = 0;
        
			 //---- Use routines in WAVE.cpp to open the sound file and
			 //---- parse the data in it.    
			if ( WaveOpenFile( sz1Dir, &hmmioIn, &WaveInfo[usWave].lpwfx, &ckInRiff ) != 0)
			{
//                debugf ( "LoadWave - %s", sz1Dir );

				if ( WaveOpenFile( sz2Dir, &hmmioIn, &WaveInfo[usWave].lpwfx, &ckInRiff ) != 0)
				{
					if ( WaveInfo[usWave].lpwfx != NULL )
						HeapFree( GetProcessHeap(), 0, WaveInfo[usWave].lpwfx );

//                    debugf ( "LoadWave - %s", sz2Dir );
    
					return( FALSE );
				}
			}


			 //---- Position the wave file for reading the wave data
			if ( WaveStartDataRead(&hmmioIn, &ckIn, &ckInRiff) != 0)
			{
				mmioClose(hmmioIn, 0);

				if ( WaveInfo[usWave].lpwfx != NULL )
					HeapFree( GetProcessHeap(), 0, WaveInfo[usWave].lpwfx );

				return( FALSE );
			}


			 //---- Ok, size of wave data is in ckIn, allocate that buffer.

			WaveInfo[usWave].dwDataSize = ckIn.cksize;
			dwDataSize = ckIn.cksize;

			WaveInfo[usWave].lpDSWave = (LPBYTE) HeapAlloc ( GetProcessHeap(), 0, dwDataSize );
			if ( WaveInfo[usWave].lpDSWave == NULL )
			{
				mmioClose(hmmioIn, 0);

				if ( WaveInfo[usWave].lpwfx != NULL )
					HeapFree( GetProcessHeap(), 0, WaveInfo[usWave].lpwfx );

				return( FALSE );

			}


			 //---- Now read the actual wave data into the data buffer.
			mmResult = WaveReadFile( hmmioIn, 
                                     dwDataSize, 
                                     (LPBYTE) WaveInfo[usWave].lpDSWave, 
                                     &ckIn, 
                                     &cbActualRead); 

			mmioClose( hmmioIn, 0 );

			CreateSDS ( usWave );
      
		}
		else        //---- Just create and fill the direct sound buffers 
		{
			CreateSDS ( usWave );
		}

	}   //--- End of if Direct Sound or sndPlaySound



    //---- Last tick count wave was played at
	WaveInfo[usWave].dwLastTick = GetTickCount();
	WaveInfo[usWave].fLoaded = TRUE;

	return (TRUE);
} // LoadWaveDate




//----------------------------------------------------------------------------
//  ToggleWave
//
//  Description:
//
//        toggle sound state 
//
//  Arguments:
//
//
//  Return : 
//      
//
//----------------------------------------------------------------------------
void ToggleWave( void )
{
   if ( fSound == TRUE )
   {
      StopWave();
   }
   else
   {
      StartWave();
   }
}   //---- End of ToggleWave()




//----------------------------------------------------------------------------
//  ToggleSpeech
//
//  Description:
//
//        toggle Speech state 
//
//  Arguments:
//
//
//  Return : 
//      
//
//----------------------------------------------------------------------------
void ToggleSpeech( void )
{
   if ( fSpeech == TRUE )
   {
      fSpeech = FALSE;
   }
   else
   {
      fSpeech = TRUE;
   }
}   //---- End of ToggleSpeech()




//----------------------------------------------------------------------------
//  StartWave
//
//  Description:
//
//        Start playing sounds 
//
//  Arguments:
//
//
//  Return : 
//      
//
//----------------------------------------------------------------------------
void StartWave( void )
{
   fSound = TRUE;

   if ( iSoundType == DIRECT_SOUND )
   {
       IDirectSoundBuffer_Play( lpDSPrimary, 0, 0, DSBPLAY_LOOPING );
   }
}   //---- StartWave() 




//----------------------------------------------------------------------------
//  StopWave
//
//  Description:
//
//        Stop playing sounds 
//
//  Arguments:
//
//
//  Return : 
//      
//
//----------------------------------------------------------------------------

void StopWave( void )
{
   fSound = FALSE;

   if ( iSoundType == DIRECT_SOUND )
   {
       IDirectSoundBuffer_Stop( lpDSPrimary );
   }
}   //---- StopWave()




//----------------------------------------------------------------------------
//  PauseWaves
//
//  Description:
//
//        Pause playing sounds 
//
//  Arguments:
//
//
//  Return : 
//      
//
//----------------------------------------------------------------------------
void PauseWaves( void )
{
    if ( fSound )
    {
        if ( iSoundType == DIRECT_SOUND )
        {
            if ( lpDSPrimary )
            {
                IDirectSoundBuffer_Stop( lpDSPrimary );
            }
        }
    }
}   //---- PauseWaves()




//----------------------------------------------------------------------------
//  RestoreWaves
//
//  Description:
//
//        Resume playing sounds if they are supposed to be on.
//
//  Arguments:
//
//
//  Return : 
//      
//
//----------------------------------------------------------------------------
void RestoreWaves( void )
{
    if ( fSound )
    {
        if ( iSoundType == DIRECT_SOUND )
        {
            if ( lpDSPrimary )
            {
                IDirectSoundBuffer_Play( lpDSPrimary, 0, 0, DSBPLAY_LOOPING );
            }
        }
    }
}   //---- RestoreWaves()




//----------------------------------------------------------------------------
//  BOOL PlayWave
//
//  Description:
//
//        Plays a given wave.
//
//  Arguments:
//
//        LPVOID lpWave  pointer to wave in memory
//             floop   if you loop you should also stop 
//
//  Return ( BOOL ):    TRUE  Sound is played  
//                      FALSE Sound is not played 
//      
//
//--------------------------------------------------------------------------;
BOOL  PlayWave( USHORT usWave, BOOL fLoop )
{
    DWORD dwTempTick;

    if ( fSound == TRUE     &&               //---- Playing waves
         usWave != WAV_NULL &&               //---- Not blank wave 
         WaveInfo[usWave].fLoaded == TRUE )  //---- the wave exists in memory
    {
        //---- Don't play speech if disabled 

        if ( WaveInfo[usWave].fSpeech == TRUE &&
             fSpeech == FALSE )
        {
            return FALSE;
        }


        //---- If a sound delay then only play if ticks are greater than delay

        if ( WaveInfo[usWave].dwDelay > 0 )
        {
            if ( (( dwTempTick = GetTickCount() ) - WaveInfo[usWave].dwLastTick ) < 
                 WaveInfo[usWave].dwDelay                                           ) 
            {
                return TRUE; 
            }

            WaveInfo[usWave].dwLastTick = dwTempTick;

        }


        if ( iSoundType == DIRECT_SOUND )
        {
            if( fLoop == TRUE )
            {
                IDirectSoundBuffer_Play( WaveInfo[usWave].lpDSBuffer,
																0, 0, DSBPLAY_LOOPING );
            }
            else
            {
                if ( WaveInfo[usWave].fSetPos )
                {
                    IDirectSoundBuffer_SetCurrentPosition( WaveInfo[usWave].lpDSBuffer, 0 );
                }
                IDirectSoundBuffer_Play( WaveInfo[usWave].lpDSBuffer, 0, 0, 0 );
            }
        }
        else
        {

            if ( fLoop == TRUE )
            {
                sndPlaySound ( (LPCSTR)WaveInfo[usWave].lpRawWave,
                               SND_ASYNC | SND_NODEFAULT | SND_MEMORY | SND_LOOP | 
                               WaveInfo[usWave].fuSound );
            }
            else
            {
                sndPlaySound ( (LPCSTR)WaveInfo[usWave].lpRawWave,
                               SND_ASYNC | SND_NODEFAULT | SND_MEMORY |
                               WaveInfo[usWave].fuSound );
            }

        }

    }

    return TRUE;
}




//----------------------------------------------------------------------------
//  BOOL PlayWaveClass 
//
//  Description:
//
//        Plays a given wave.
//
//  Arguments:
//
//        LPVOID lpWave  pointer to wave in memory
//               floop   if you loop you should also stop 
//               usClass delay class wave belongs too
//
//  Return ( BOOL ):    TRUE  Sound is played  
//                      FALSE Sound is not played 
//      
//
//--------------------------------------------------------------------------;

BOOL  PlayWaveClass( USHORT usWave, BOOL fLoop, USHORT usClass )
{
    DWORD dwTempTick;

    if ( fSound == TRUE     && 
         usWave != WAV_NULL &&
         WaveInfo[usWave].fLoaded == TRUE )
    {

        //---- Don't play speech if disabled 

        if ( WaveInfo[usWave].fSpeech == TRUE &&
             fSpeech == FALSE )
        {
            return FALSE;
        }

        //---- If a sound delay then only play if ticks are greater than delay

        if ( WaveClass[usClass].dwDelay > 0 )
        {
            if ( (( dwTempTick = GetTickCount() ) - WaveClass[usClass].dwLastTick ) < 
                 WaveClass[usClass].dwDelay                                            ) 
            {
                return TRUE; 
            }

            WaveClass[usClass].dwLastTick = dwTempTick;

        }


        if ( iSoundType == DIRECT_SOUND )
        {
            if( fLoop == TRUE )
            {
                IDirectSoundBuffer_Play( WaveInfo[usWave].lpDSBuffer, 
															0, 0, DSBPLAY_LOOPING );
            }
            else
            {
                //---- Thex Damage Kludgee.. 

                if ( usClass == WAV_CLASS_1 )
                {
                    PlayWaveEx ( usWave, fLoop, 0, -600, 0 );
                    return TRUE;
                }

                if ( WaveInfo[usWave].fSetPos )
                {
                    IDirectSoundBuffer_SetCurrentPosition( WaveInfo[usWave].lpDSBuffer, 0 );
                }
                IDirectSoundBuffer_Play( WaveInfo[usWave].lpDSBuffer, 0, 0, 0 );
            }
        }
        else
        {
            if ( fLoop == TRUE )
            {
                sndPlaySound ( (LPCSTR)WaveInfo[usWave].lpRawWave,
                               SND_ASYNC | SND_NODEFAULT | SND_MEMORY | SND_LOOP |
                               WaveInfo[usWave].fuSound );
            }
            else
            {
                sndPlaySound ( (LPCSTR)WaveInfo[usWave].lpRawWave,
                               SND_ASYNC | SND_NODEFAULT | SND_MEMORY |
                               WaveInfo[usWave].fuSound );
            }

        }

    }

    return TRUE;
}

                       
//----------------------------------------------------------------------------
//  BOOL PlayWaveEx
//
//  Description:
//
//        Plays a given wave with a twist. 
//
//  Arguments:
//
//      LPVOID lpWave    pointer to wave in memory
//      bool   floop     loop da loop
//      long   Pan       -10000 to 10000,  0 - middle in 1/100 of DB attenuation
//      long   Volume    0 to -10000       -10000 is -100 db  silent 
//      dword  Freq      100 - 100000      0 means leave it at current IGNORED RIGHT NOW !!!!!!
//
//  Return ( BOOL ):    TRUE  Sound is played  
//                      FALSE Sound is not played 
//      
//
//--------------------------------------------------------------------------;

BOOL PlayWaveEx( USHORT usWave, 
                 BOOL   fLoop,
                 long   Pan,
                 long   Volume,
                 DWORD  Freq    )
{
    DWORD dwTempTick;

    if ( fSound == TRUE     && 
         usWave != WAV_NULL &&
         WaveInfo[usWave].fLoaded == TRUE )
    {
        //---- Don't play speech if disabled 

        if ( WaveInfo[usWave].fSpeech == TRUE &&
             fSpeech == FALSE )
        {
            return FALSE;
        }

        //---- If a sound delay then only play if ticks are greater than delay

        if ( WaveInfo[usWave].dwDelay > 0 )
        {
            if ( (( dwTempTick = GetTickCount() ) - WaveInfo[usWave].dwLastTick ) < 
                 WaveInfo[usWave].dwDelay                                           ) 
            {
                return TRUE; 
            }

            WaveInfo[usWave].dwLastTick = dwTempTick;

        }



        if ( iSoundType == DIRECT_SOUND )
        {
          //  debugf("Volume %ld Pan %ld\n", Volume, Pan );

            SetWaveVolume ( usWave, Volume );
            SetWavePan ( usWave, Pan );
         //   SetWaveFreq ( usWave, Freq );

            if( fLoop == TRUE)
            {
                IDirectSoundBuffer_Play( WaveInfo[usWave].lpDSBuffer, 0, 0, DSBPLAY_LOOPING );
            }
            else
            {
                if ( WaveInfo[usWave].fSetPos )
                {
                    IDirectSoundBuffer_SetCurrentPosition( WaveInfo[usWave].lpDSBuffer, 0 );
                }
                IDirectSoundBuffer_Play( WaveInfo[usWave].lpDSBuffer, 0, 0, 0 );
            }

        }
        else
        {
            if ( fLoop == TRUE )
            {
                sndPlaySound ( (LPCSTR)WaveInfo[usWave].lpRawWave,
                               SND_ASYNC | SND_NODEFAULT | SND_MEMORY | SND_LOOP |
                               WaveInfo[usWave].fuSound );
            }
            else
            {
                sndPlaySound ( (LPCSTR)WaveInfo[usWave].lpRawWave,
                               SND_ASYNC | SND_NODEFAULT | SND_MEMORY |
                               WaveInfo[usWave].fuSound );
            }

        }

    }

    return TRUE;

}   //---- PlayWaveEx()



//----------------------------------------------------------------------------
//  BOOL PlayWavePos
//
//  Description:
//
//        Plays a given wave with a positional effect from thexder. 
//
//  Arguments:
//
//      USHORT usWave    wave number 
//      int    iObjx     x coord of object making sound
//      int    iObjy     y coord of object making sound 
//      int    iThexx    x coord of Thexder ( where we are )
//      int    iThexy    y coord of Thexder ( where we are ) 
//
//  Return ( BOOL ):    TRUE  Sound is played  
//                      FALSE Sound is not played 
//      
//  
//--------------------------------------------------------------------------;

BOOL PlayWavePos ( USHORT usWave, int iObjx, int iObjy, int iThexx, int iThexy )
{
    int iX;
    int iY;
    long iPan    = MIDPAN_DS;
    long iVolume = MAXVOL_DS;



    if ( iSoundType == DIRECT_SOUND )
    {


        iX = iObjx - iThexx;
        iY = iObjy - iThexy;

        iPan += ( iX * 45 );       //---- Magic number 

        //---- left or right 

        if ( iX > 0 )          //---- right 
        {
            if ( iPan > MAXPAN_DS ) iPan = MAXPAN_DS;
        }
        else                   //---- left 
        {
            if ( iPan < MINPAN_DS ) iPan = MINPAN_DS;
        }
        

        iVolume -= ( (abs(iX) + abs( iY )) * 45  );    //---- Another magic number 


        if ( iVolume < -2000  ) iVolume = -2000;        // minimum is 1/4 normal -40 db

        if ( iVolume > -300 )  iVolume = -300;
    }


    PlayWaveEx( usWave, 
                FALSE,
                iPan,
                iVolume,
                0   );

    return TRUE;


}   //---- PlayWavePos()



//----------------------------------------------------------------------------
//  BOOL StopAllWave
//
//  Description:
//
//         Stop playing all waves ( doesnt stop the main buffer )  
//
//  Arguments:  None 
//
//
//  Return ( BOOL ):    TRUE  Sound is stopped   
//                      FALSE Sound is not stopped
//      
//
//--------------------------------------------------------------------------;

BOOL StopAllWave ( void )
{
    USHORT x;

    for ( x = 0; x < MAX_WAVE; x++ )
    {
        PauseWave ( x );
    }


    return TRUE;
}



//----------------------------------------------------------------------------
//  BOOL PauseWave
//
//  Description:
//
//         Stop playing a wave particular wave  
//
//  Arguments:  None 
//
//
//  Return ( BOOL ):    TRUE  Sound is stopped   
//                      FALSE Sound is not stopped
//      
//
//--------------------------------------------------------------------------;

BOOL  PauseWave( USHORT usWave )
{

    if ( iSoundType == DIRECT_SOUND )
    {
        if ( WaveInfo[usWave].fLoaded == TRUE )
        {
            IDirectSoundBuffer_Stop( WaveInfo[usWave].lpDSBuffer );
        }

    }
    else
    {
        sndPlaySound ( NULL, SND_ASYNC );
    }

    return TRUE;
}




//----------------------------------------------------------------------------
//  BOOL PanWave
//
//  Description:
//
//        Pan a particular wave   
//
//  Arguments:  None 
//
//
//  Return ( BOOL ):    TRUE  Sound is stopped   
//                      FALSE Sound is not stopped
//      
//
//--------------------------------------------------------------------------;

BOOL  SetWavePan( USHORT usWave, int Pan )
{

    if ( iSoundType == DIRECT_SOUND )
    {
        if ( WaveInfo[usWave].fLoaded == TRUE )
        {
            IDirectSoundBuffer_SetPan( WaveInfo[usWave].lpDSBuffer, Pan );
        }
    }

    return TRUE;
}


//----------------------------------------------------------------------------
//  BOOL SetWaveVolume
//
//  Description:
//
//        Set volume on a particular wave   
//
//  Arguments:  None 
//
//
//  Return ( BOOL ):    TRUE  Sound is stopped   
//                      FALSE Sound is not stopped
//      
//
//--------------------------------------------------------------------------;

BOOL  SetWaveVolume( USHORT usWave, int Volume )
{

    if ( iSoundType == DIRECT_SOUND )
    {
        if ( WaveInfo[usWave].fLoaded == TRUE )
        {
           IDirectSoundBuffer_SetVolume( WaveInfo[usWave].lpDSBuffer, Volume );
        }
    }

    return TRUE;
}


//----------------------------------------------------------------------------
//  BOOL SetWaveFreq
//
//  Description:
//
//        Set frequency of wave for a particular wave. If zero leave at current 
//
//  Arguments:  None 
//
//
//  Return ( BOOL ):    TRUE     
//                      FALSE 
//      
//
//--------------------------------------------------------------------------;

BOOL  SetWaveFreq( USHORT usWave, DWORD dwFreq )
{

    if ( iSoundType == DIRECT_SOUND )
    {

        if ( dwFreq != 0 )
        {
            if ( WaveInfo[usWave].fLoaded == TRUE )
            {
                IDirectSoundBuffer_SetFrequency( WaveInfo[usWave].lpDSBuffer, dwFreq );
            }
        }

    }

    return TRUE;

}



//----------------------------------------------------------------------------
//
//  WavesLoad ( ) 
//
//  Description: Load sound effect waves into memory. 
//               Called in appInit 
//
//  Arguments:   
//
//
//  Return :
//      
//--------------------------------------------------------------------------;

void WavesLoad ( PSZ pszFirstDir, PSZ pszSecondDir )
{

    USHORT   x;
    DWORD    dwTemp;
    DWORD    dwTemp2;



    dwTemp2 = GetTickCount();

    //---- Load the waves into memory 

    for ( x=0; x < MAX_WAVE ; x++ )
    {
        if ( LoadWaveData( x, pszFirstDir, pszSecondDir ) == FALSE )
        {
            continue;
        }

    }


//    debugf ( "Sound load time %lu", ( GetTickCount() - dwTemp2 ) );


    //---- Set so sounds don't play over themselves ( for classes )

    dwTemp = GetTickCount();

    //---- Initialize wave classes 

    for ( x = 0; x < MAX_WAVE_CLASS; x ++ )
    {
        WaveClass[x].dwLastTick = dwTemp;
    }



}   //---- End of WavesLoad()


//------------------------------------------------------------------------------------
// Function:  DialogWaveWait() 
//
// Inputs:  
//
// Outputs: 
//
// Abstract: Dialog for waiting to load waves
//
//------------------------------------------------------------------------------------
BOOL CALLBACK DialogWaveWait( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )

{

    switch ( message )
    {   

        case WM_INITDIALOG:
			CenterInWindow( hwnd, (HWND) lParam, TRUE );
			return ( TRUE );
   
        case WM_COMMAND:

            switch ( LOWORD(wParam) ) 
            {
                case IDCANCEL:
                {
                    EndDialog( hwnd, TRUE );
                    return 1;
                }
            }

        default:
            break;
    }

    return 0;

}


//----------------------------------------------------------------------------
//
//  InitSound ( ) 
//
//  Description: Initialize everything required for sound 
//                                  
//
//  Arguments:
//		hInstApp			- application instance   
//		hwndApp			- application window handle
//		pszFirstDir		- first directory to look for waves
//		pszSecondDir	- second directory to look for waves
//
//  Return :
//      
//-------------------------------------------------------------------------;

void InitSound ( HINSTANCE hInstApp, HWND hwndApp, PSZ pszFirstDir, PSZ pszSecondDir )                                      
{

    //---- Only initialize if the direct sound object doesn't exist 

    if ( lpDS == NULL )
    {
//        HCURSOR hPrev;
//        HWND    hwndWave;

//        hwndWave = CreateDialogParam( hInstApp, "WAVEWAIT", hwndApp, 
//													(DLGPROC) DialogWaveWait, (LPARAM) hwndApp );
//
//        ShowWindow( hwndWave,	      // handle of window
//                    SW_SHOWNORMAL ); // show state of window


        //---- Throw up the wait cursor ( it can take awhile loading sounds )

//        hPrev = ShowWaitCursor ();


        InitializeWaveDevice( hwndApp );

        WavesLoad( pszFirstDir, pszSecondDir );


        //---- Destroy WaveWait screen

//	    if( hwndWave )
//		    DestroyWindow( hwndWave );

        //---- Restore the previous cursor 

//        RestoreCursor( hPrev );

    }    
                    
}   //---- End of InitSound()




//----------------------------------------------------------------------------
//
//  FinalSound ( ) 
//
//  Description: Kill maim and destroy the sound maker.
//                                  
//
//  Arguments:   
//
//
//  Return :
//      
//--------------------------------------------------------------------------;

void FinalSound ( void )
{
    int x;


    //---- Release memory for waves 

    for ( x = 0; x < MAX_WAVE; x ++ )
    {
        WaveInfo[x].fLoaded = FALSE;
    
        if ( WaveInfo[x].lpDSBuffer != NULL )
        {
            WaveInfo[x].fLoaded = FALSE;
            IDirectSoundBuffer_Stop(WaveInfo[x].lpDSBuffer);
            IDirectSoundBuffer_Release(WaveInfo[x].lpDSBuffer);
            WaveInfo[x].lpDSBuffer = NULL;

        }

        if ( WaveInfo[x].lpwfx != NULL )
        {
            GlobalFree( WaveInfo[x].lpwfx );
            WaveInfo[x].lpwfx = NULL;
        }

        if ( WaveInfo[x].lpDSWave != NULL )
        {
            GlobalFree( WaveInfo[x].lpDSWave );
            WaveInfo[x].lpDSWave = NULL;
        }

        if ( WaveInfo[x].lpRawWave != NULL )
        {
            WaveInfo[x].fLoaded = FALSE;
            GlobalFree( WaveInfo[x].lpRawWave );
            WaveInfo[x].lpRawWave = NULL;
        }
    
    }


    //---- Get rid of the primary DS Buffer 

    if ( lpDSPrimary != NULL )
    {

        IDirectSoundBuffer_Stop(lpDSPrimary);

        IDirectSoundBuffer_Release(lpDSPrimary);

        lpDSPrimary = NULL;

    }


    //---- Destroy the direct sound object

    if ( lpDS != NULL)
    {
        IDirectSound_Release( lpDS );
        lpDS = NULL;
    }

	 // -- we're through listening now
    FreeLibrary( hSound );

   return;


}   //---- FinalSound()





//----------------------------------------------------------------------------
//
//  SuspendSound () 
//
//  Description: Suspend Sound so someone else can play 
//                                  
//
//  Arguments:   
//
//
//  Return :
//      
//--------------------------------------------------------------------------;

void SuspendSound ( void )
{
    int x;

    //---- Release memory for waves 

    for ( x = 0; x < MAX_WAVE; x ++ )
    {
        WaveInfo[x].fLoaded = FALSE;
    
        if ( WaveInfo[x].lpDSBuffer != NULL )
        {
            IDirectSoundBuffer_Stop(WaveInfo[x].lpDSBuffer);
            IDirectSoundBuffer_Release(WaveInfo[x].lpDSBuffer);
            WaveInfo[x].lpDSBuffer = NULL;

        }

    
    }


    //---- Get rid of the primary DS Buffer 

    if ( lpDSPrimary != NULL )
    {

        IDirectSoundBuffer_Stop(lpDSPrimary);

        IDirectSoundBuffer_Release(lpDSPrimary);

        lpDSPrimary = NULL;

    }


    //---- Destroy the direct sound object

    if ( lpDS != NULL)
    {
        IDirectSound_Release( lpDS );
        lpDS = NULL;
    }


   return;


}   //---- SuspendSound()




//----------------------------------------------------------------------------
//
//  DSError ( ) 
//
//  Description: Debugf the known Direct Sound errors
//                                  
//
//  Arguments:   
//
//
//  Return :
//      
//--------------------------------------------------------------------------;

void DSError ( MMRESULT mmResult )
{
//    #if defined ( _DEBUG )
//
//    switch ( mmResult )
//    {
//        case DS_OK:
//            debugf ( "DS_OK" );
//            break;
//
//        case DSERR_ALLOCATED:
//            debugf ( "DSERR resources (such as a priority level) already being used" );
//            break;
//
//        case DSERR_CONTROLUNAVAIL:
//            debugf( "DSERR The control (vol,pan,etc.) requested is not available." );
//            break;
//
//        case DSERR_INVALIDPARAM:
//            debugf ( "DSERR An invalid parameter was passed to the returning function" );
//            break;
//
//        case DSERR_INVALIDCALL:
//            debugf ( "DSERR This call is not valid for the current state of this object" );
//            break;
//
//        case DSERR_GENERIC:
//            debugf ( "DSERR An undetermined error occured inside the DSound subsystem" );
//            break;          
//
//        case DSERR_PRIOLEVELNEEDED:
//            debugf ( "DSERR The caller does not have the priority level required. " );
//            break;
//
//        case DSERR_OUTOFMEMORY:
//            debugf ( "DSERR Couldn't allocate sufficient memory to complete request" );
//            break;
//
//        case DSERR_BADFORMAT:
//            debugf ( "DSERR Wave format not supported" );
//            break;
//
//        case DSERR_UNSUPPORTED:
//            debugf ( "DSERR Function not support at this time" );
//            break;
//
//        case DSERR_NODRIVER:
//            debugf ( "DSERR No sound driver is available for use" );
//            break;
//
//        case DSERR_ALREADYINITIALIZED:
//            debugf ( "DSERR This object is already initialized" );
//            break; 
//
//        case DSERR_NOAGGREGATION:
//            debugf ( "DSERR This object does not support aggregation" );
//            break;
//
//        case DSERR_BUFFERLOST:
//            debugf ( "DSERR The buffer memory has been lost, and must be Restored." );
//            break;
//
//        case DSERR_OTHERAPPHASPRIO:
//            debugf ( "Another app has a higher priority level, preventing this call from succeeding." );
//            break;
//
//        default:
//            debugf ( "DSERR UnKnown" );
//            break;
//    }
//
//    #endif

    return;

}   //---- End of DSError()



//---- End of SndUtil.cpp


