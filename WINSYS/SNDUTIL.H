//-----------------------------------------------------------------------------
//
//        Copyright 1995/1996 by NW Synergistic Software, Inc.
//
//        SndUtil.c - Provides interface for playing wave files.
//
//------------------------------------------------------------------------------

#ifndef _SNDUTIL_H
#define _SNDUTIL_H

#define WAV_NULL         65535   //---- Used to signify no wave is to be used

#define WAV_CLASS_1       1
#define WAV_CLASS_2       2

#define DIRECT_SOUND   1
#define SNDPLAY_SOUND  2

#define EIGHTBIT_DS             1

//---- Direct sound stuff

#define MINFREQ_DS              100L
#define MAXFREQ_DS              100000L
#define MINPAN_DS               -10000L   //---- Max left  
#define MAXPAN_DS               10000L    //---- Max right 
#define MIDPAN_DS               0L        //---- middle 
#define MINVOL_DS               -100L     //---- min volume 
#define MAXVOL_DS               0L        //---- max volume ( no amplification yet )


#if defined (__cplusplus)
extern "C" {
#endif

 // -- Overall Waves/Speech
void InitSound		( HINSTANCE hInstApp, HWND hwndApp, PSZ pszFirstDir, PSZ pszSecondDir );
void FinalSound	( void );

void StartWave		( void );
void StopWave		( void );
void ToggleWave	( void );
void ToggleSpeech	( void );
void PauseWaves	( void );
void RestoreWaves	( void );
BOOL StopAllWave	( void );

 // -- Individual Wave Playing
BOOL PlayWave		( USHORT usWave, BOOL fLoop );
BOOL PlayWaveClass( USHORT usWave, BOOL fLoop, USHORT usClass );
BOOL PlayWaveEx	( USHORT usWave, BOOL fLoop, long Pan, long Volume, DWORD Freq    );
BOOL PlayWavePos	( USHORT usWave, int iObjx, int iObjy, int iMyx, int iMyy );
BOOL SetWaveVolume			( USHORT usWave, int Volume );

BOOL PauseWave( USHORT usWave );

#if defined (__cplusplus)
}
#endif


#endif