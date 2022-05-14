/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: sound.cpp
   Author:   Chris Barker

   ========================================================================

   Contains the following general functions:


   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#include <io.h>

#ifdef _WINDOWS
#define INCLUDE_MMSYSTEM_H
#include <Windows.h>
#endif

#if defined (WINDOWS_SOUND)
	#include <windows.h>
#else
	//#include <bios.h>
#endif

#include "SYSTEM.H"

#if defined (WINDOWS_SOUND)
#include "winsys\wave.h"
#include "winsys\SndUtil.h"
#include "winsys\SWinUtil.h"
#endif

#if defined (AUDIO_LIB)
#include "KERNAUD.HPP"
#endif

#include "SOUND.HXX"
#include "SCENE.HXX"
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "VECTOR.HXX"
/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
int InitRedBook(void);
extern "C" {
int StopRedBook (void);
}

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

#define MAX_VOLUME 127
#define	MIN_VOLUME 0
#define PAN_RIGHT 100
#define PAN_LEFT   0
#define PAN_CENTER 50
#define FULL_VOLUME_FLAG -1
#define MAX_MIXED_LOOPING_SOUNDS 3
#define ENVIRON_SND -25
#define LOOPING_SOUND_FILES 50000

#define MAX_SOUND_THINGS	100

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
#define REMOVED_FROM_ARRAY -46
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
int CalculatePan(LONG);
LONG TweakVolume(LONG);
static void ModifySndObject(LONG , int, int );
void RemoveLoopingSoundFromArray(LONG);

#ifdef WINDOWS_SOUND
extern "C" {
//BOOL  SetWavePan( USHORT usWave, int Pan );
extern HWND	hwndApp;
extern HINSTANCE hInstApp;
}
#ifdef AUDIO_LIB
//void FAR PASCAL ServiceAudio(UINT, UINT, DWORD DlgPtr, DWORD, DWORD);
#endif
#endif
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
BOOL PlayEnvironmentalSounds = TRUE;
BOOL UIPlayEnvironmentalSounds = TRUE;
SHORT WhichEnvironmentalSound = 0;
#ifdef _WINDOWS
extern "C" {
extern HWND hwndApp;
}
HWND hMyWnd;
#endif
SHORT UserNumSFXPlaying = 4;
extern BOOL SlideShowPlaying;
/* ------------------------------------------------------------------------
   Local Variables
   ------------------------------------------------------------------------ */
//int AdventureMusic[] = {	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,};

static SHORT FullVolumeDistance = 100;
static SHORT HalfVolumeDistance = 200;
static SHORT SFXVolumeIndex = 10;
static BOOL PauseAllSounds_Hits = 0;
static BOOL AudioInitialized = FALSE;
//static SHORT FrameCount;
static LONG TotalEnvironSnds = 0;
static LONG TotalLoopingSounds = 0;

#define FADE_TO_RESET_COUNT 20;
static LONG SoundFade = 0;
static LONG SoundVolume;
static LONG MusicVolume;
static LONG	lTime = 0;
extern SHORT sfxvol;
extern SHORT musvol;

typedef struct _LOOPING_SOUND_THING{
	LONG ThingTypeIndex;
	int InstanceTag;
	BIRTHRT_SND  wave;
}LOOPING_SOUND_THING;

static DECL_VECTOR_DATA(LOOPING_SOUND_THING,LoopingSoundThing,MAX_SOUND_THINGS);

#include "AMBIENCE.INC"

unsigned long SndRandom ( LONG val )
{

#ifdef _WINDOWS

   return (unsigned long) ( GetTickCount() % val );

#else
   return ( unsigned long ) ( clock() % val );

#endif



}


/***********************************************************************************
Function AddSndObj
Purpose  Add a sound to the sound cue
Arguments the wave id, the number of wave files available for randomization, index into mythings
return  the unique tag if the sound is actually played, else fERROR
**************************************************************************************/
extern "C"
SHORT AddSndObj( BIRTHRT_SND  WaveID, SHORT randomCount, LONG MyThingsIndex)
{
	//static SHORT numChallenges = 0;
	LONG volume;
	int pan = 50;

	if(!AudioInitialized || (!SFXVolumeIndex && !fMusic)  )
		return fERROR;

#ifdef AUDIO_LIB
	static SHORT InstanceTag = 1;
#endif
	SHORT Instance = fERROR;
	SHORT looped = FALSE;

	looped = (WaveID >= LOOPING_SOUND_FILES) ? TRUE : FALSE ;
	if( (WaveID == 105) || (WaveID == 102) )
		looped = TRUE;
	if(randomCount != NULL)
	   WaveID  = (BIRTHRT_SND) (WaveID + SndRandom(randomCount) );

	if(MyThingsIndex < 0)
	{
		pan = 50;	// center
		switch(MyThingsIndex)
		{
		case VOLUME_PRIORITY:
				volume = 127;
				break;
			case VOLUME_FULL:
				volume = MAX_VOLUME;
				break;
			case VOLUME_NINETY:
				volume =  114;
				break;
			case VOLUME_EIGHTY:
				volume = 101;
				break;
			case VOLUME_SEVENTY:
				volume = 89;
				break;
			case VOLUME_SIXTY:
				volume = 76;
				break;
			case VOLUME_FIFTY:
				volume = 63;
				break;
			case VOLUME_FORTY:
				volume = 50;
				break;
			case VOLUME_THIRTY:
				volume = 37;
				break;
			case VOLUME_TWENTY:
				volume = 24;
				break;
			case VOLUME_TEN:
				volume = 11;
				break;
			case VOLUME_OFF:
				volume = 0;
				break;
			case ENVIRON_SND:
				volume =  5 + SndRandom(60);
				pan = SndRandom(100);	
				break;
		}
	}
	// calculate distance from camera to thing
	else
	{
		// set the pan
		pan = CalculatePan(MyThingsIndex);
		volume = aprox_dist(PLAYER_INT_VAL(player.x),
							PLAYER_INT_VAL(player.y),
							mythings[MyThingsIndex].x,
							mythings[MyThingsIndex].y);

		volume = TweakVolume(volume);
	}
	if( (volume <= MAX_VOLUME) && (volume > MIN_VOLUME) )
	{
#ifdef AUDIO_LIB
		// adjust volume for user options
		if( (WaveID == 499) || (WaveID == 102) || (WaveID == 105) || ( (WaveID > 111) && (WaveID < 125) )) // this is music xxx
			volume = volume * GetMusicVolume() / 10;  // GetMusicVolume returns an index, 1 - 10, ten is loudest
		else
			volume = volume * SFXVolumeIndex / 10;

		if( (kernel->AudioActiveSamples() < UserNumSFXPlaying ) && (WaveID > 0) )
		{
//#ifndef _WINDOWS
//			kernel->AudioDefaultBits();
//			kernel->AudioSetRate(22050);
//#endif// _windows	
			InstanceTag = (SHORT)((InstanceTag > 30000) ? 1 : ++InstanceTag);
			if(kernel->AudioPlay(	(int) WaveID,
							(BOOL) looped, 		// looped
							(int) volume,    // volume
							(int) InstanceTag,
							(long) FALSE) )		// paused
			{
				Instance = InstanceTag;
				// kernel->AudioPan((int)WaveID, pan,(int)InstanceTag);
			}
		}
		else
			Instance = fERROR;
#endif  // AUDIO_LIB
//#ifdef WINDOWS_SOUND
//		PlayWaveEx( WaveID, looped, pan * 32,-(MAX_VOLUME - volume) * 16,0 );
//#endif //WINDOWS_SOUND
	}
	return(Instance);

}
/******************************************************************
Function	CalculatePan
Purpose		Derive the pan from the mythings angle
arguments	the index into the mythings array
return		the pan
******************************************************************/
#define ANGLE_MULTI (1024)
extern "C" {
    extern LONG sin_table[];
    #include "SYSTEM.H"
}
int CalculatePan(LONG MyThingsIndex)
{
	int angle, pan;
	int objectangle;

	// get the angle from the current view to the object
	if(MyThingsIndex < 0)
		return(0);
	objectangle = AngleFromPoint2(
	    CAMERA_INT_VAL(camera.x), CAMERA_INT_VAL(camera.y),
        mythings[MyThingsIndex].x, mythings[MyThingsIndex].y, 1 );

    angle = RelativeAngle(camera.a, objectangle);

//    printf("Sound System: camera %d, object %d, relative angle %d\n",
  //      camera.a, objectangle, angle);

    // calculate the balance
    pan = PAN_CENTER +
          PAN_CENTER * sin_table[angle] / ANGLE_MULTI;

	if(pan > 100) pan = 100;
	if(pan < 0)	  pan = 0;

	return(pan);

}

/**********************************************************************************
Function	TweakVolume
Purpose 	Set the volume based on FullVolumeDistance and HalfVolumeDistance
Arguments	the volume to adjust
return		the adjusted volume
**********************************************************************************/
LONG TweakVolume(LONG volume)
{

	if(volume <= FullVolumeDistance)
		volume = MAX_VOLUME;

	else if(volume <= HalfVolumeDistance)
		volume = MAX_VOLUME - (LONG)( (float)(volume - FullVolumeDistance) /
								(float)((HalfVolumeDistance - FullVolumeDistance) /
										(float)(MAX_VOLUME / 2)) ) ;
	else
		volume = (MAX_VOLUME / 2) - ( (volume - HalfVolumeDistance) / 2 )  ;

	return (volume);

}
/*********************************************************************************************
Function ModifySndObject
Purpose  change the volume or pan of a sound
arguments  the tag concatenation of the waveID and its tag, the new volume, the new pan %
			pan of 0 is full left, pan of 50 is center, pan of 100 is full right
return    NONE
**********************************************************************************************/
//extern "C"
void ModifySndObject(LONG tag, int volume, int pan)
{							
	
	if(AudioInitialized && (SFXVolumeIndex > 0))
	{
		//SHORT WaveID;
		SHORT i;
		//SHORT k;

		// find this tag
		for(i=0;i<MAX_SOUND_THINGS;i++)
		{
			if(( tag == LoopingSoundThing[i].InstanceTag) && (LoopingSoundThing[i].wave > 0))
			{
				// if volume is 0, turn off the sound and sort the list
				if(0 == volume)
				{
#ifdef AUDIO_LIB
					kernel->AudioStop(LoopingSoundThing[i].wave, LoopingSoundThing[i].InstanceTag);
#endif
#ifdef WINDOWS_SOUND
					PauseWave( (USHORT) LoopingSoundThing[i].wave );
#endif
					LoopingSoundThing[i].InstanceTag = fERROR;
				}
				if(volume != NULL)
				{
#ifdef AUDIO_LIB
					kernel->AudioVolume((int)LoopingSoundThing[i].wave, (int)volume* GetGlobalVolume() / 10, (int)LoopingSoundThing[i].InstanceTag);
#endif
#ifdef WINDOWS_SOUND
//					SetWaveVolume( (USHORT) LoopingSoundThing[i].wave, (int) -(MAX_VOLUME - volume) * 78 ) ;
					SetWaveVolume( (USHORT) LoopingSoundThing[i].wave, (int) -(MAX_VOLUME - volume) * 16 ) ;
#endif
					if(pan != NULL)
					{
#ifdef AUDIO_LIB
						kernel->AudioPan((int)LoopingSoundThing[i].wave, pan,(int)LoopingSoundThing[i].InstanceTag);
#endif
#ifdef WINDOWS_SOUND
						SetWavePan( (USHORT) LoopingSoundThing[i].wave, pan ) ;
#endif
					}
				}
				break;


			}
		}
	}


}
/**************************************************************
Function	SetSoundDecay
Purpose		Set parameters for sound volume of avatars
arguments	full; from 0 to full the sound is played at 100% volume
			half; from full to half the sound decrements to half volume
return		TRUE successful, FALSE bad numbers
**************************************************************/
BOOL SetSoundDecay(SHORT full, SHORT half)
{
	BOOL set = FALSE;
	
	if((full < half) && (full > 0) && (half > 0))
	{	
		FullVolumeDistance = full;
		HalfVolumeDistance = half;
		set = TRUE;
	}
	return(set);
}	
/*********************************************************************************************
FUNCTION 	UpdateLoopingSoundObjects()
PURPOSE		Check all looping sound objects, alter for movement
****************************************************************************************/
extern "C"
void UpdateLoopingSoundObjects(void)
{
	static SHORT LoopingSoundCount = 0;

	if(AudioInitialized && (SFXVolumeIndex > 0))
	{
		LONG i;
		LONG distance;
		// handle non-avatar sounds
		// check the list
		for(i=0;i<MAX_SOUND_THINGS;i++)
		{	
			if(LoopingSoundThing[i].ThingTypeIndex == fERROR)
				break;

			else if(LoopingSoundThing[i].InstanceTag == REMOVED_FROM_ARRAY)
			{
				LoopingSoundCount--;
				RemoveLoopingSoundFromArray(i);
				continue;
			}
			else if(LoopingSoundThing[i].ThingTypeIndex == -2)
				distance = MAX_VOLUME;

			else
			{
				distance= aprox_dist(PLAYER_INT_VAL(player.x),
			               			  PLAYER_INT_VAL(player.y),
			               			  mythings[LoopingSoundThing[i].ThingTypeIndex].x,
			               			  mythings[LoopingSoundThing[i].ThingTypeIndex].y);

				distance = TweakVolume(distance);
			}
			// check each for distance.
			if( (distance <= MAX_VOLUME) && (distance > MIN_VOLUME) )
			{
				// if this thing is already playing, update it
				if(LoopingSoundThing[i].InstanceTag != fERROR)
				{				
					
					ModifySndObject(LoopingSoundThing[i].InstanceTag,distance,
									CalculatePan(LoopingSoundThing[i].ThingTypeIndex) );
				}
				else
				{
					if(LoopingSoundCount <= MAX_MIXED_LOOPING_SOUNDS)
					{
						if( (LoopingSoundThing[i].InstanceTag = AddSndObj(LoopingSoundThing[i].wave, NULL, LoopingSoundThing[i].ThingTypeIndex) ) != fERROR)
							LoopingSoundCount++;
					}
				}
			}
			else if((LoopingSoundThing[i].InstanceTag != fERROR) && (LoopingSoundThing[i].wave > 0))
			{
#ifdef WINDOWS_SOUND
				PauseWave(LoopingSoundThing[i].wave);
#else
#ifdef AUDIO_LIB
				kernel->AudioStop(LoopingSoundThing[i].wave, LoopingSoundThing[i].InstanceTag);
#endif
#endif
				LoopingSoundThing[i].InstanceTag = fERROR;
				LoopingSoundCount--;
				if(i > 	TotalEnvironSnds)
				{
					RemoveLoopingSoundFromArray(i);
				}
			}
		}
	}
}

/**********************************************************************
Function AddLoopingSound
Purpose Add a looping sound so it can be handled by UpdateLoopingSoundObjects and deleted later
arguments   The BIRTHRT_SND (wave file) and the thing index OR -2 for full volume
return index into the LoopingSoundThing array OR fERROR if not added
**********************************************************************/
extern "C"
LONG AddLoopingSound(BIRTHRT_SND WaveID, LONG ThingIndex)
{
	LONG total;

	if(TotalLoopingSounds + 1 <= MAX_SOUND_THINGS)
	{
		LoopingSoundThing[TotalLoopingSounds].wave = WaveID;
		LoopingSoundThing[TotalLoopingSounds].ThingTypeIndex = ThingIndex;
		total = TotalLoopingSounds;
		TotalLoopingSounds++;
	}
	else
		total = fERROR;
	return(total);
}
/*******************************************************************************
Function RemoveLoopingSound
Purpose  Turn off a looping sound and remove it from the LoopingSoundThing array
Arguments  the index into the array returned by AddLoopingSound (above)
*******************************************************************************/
extern "C"
void RemoveLoopingSound(LONG ArrayIndex)
{
	//int i;

	if(LoopingSoundThing[ArrayIndex].InstanceTag != fERROR)
	{
		StopASound((int) LoopingSoundThing[ArrayIndex].wave,LoopingSoundThing[ArrayIndex].InstanceTag);
	}	
		// remove the sound from the array
	LoopingSoundThing[ArrayIndex].InstanceTag = REMOVED_FROM_ARRAY;
}
/********************************************************************************
Function RemoveLoopingSoundFromArray
Purpose duh
Arguments index into LoopingSoundThing array
********************************************************************************/
void RemoveLoopingSoundFromArray(LONG ArrayIndex)
{
	int i;

		for (i=ArrayIndex;i<(MAX_SOUND_THINGS - 1);i++)
		{
			LoopingSoundThing[i] = LoopingSoundThing[i+1];
			if(LoopingSoundThing[i].ThingTypeIndex == fERROR)
				break;
		}
		
		// tail of list case.
		if ( i == (MAX_SOUND_THINGS - 1))
		{
			LoopingSoundThing[i].ThingTypeIndex = fERROR;
			LoopingSoundThing[i].InstanceTag = fERROR;
		}
		TotalLoopingSounds--;
}
/**************************************************************************
Function InitializeSoundSystem
purpose  set up Sol Audio
**************************************************************************/

extern "C"
void InitializeSoundSystem(void)
{
#ifdef AUDIO_LIB

   #ifdef _WINDOWS
       //---- KLUDGE SOLAUDIO doesn't detect that you don't have a sound card
       //---- in the DirectX version
       if ( !waveOutGetNumDevs() )
       {
           return;
       }
   #endif

	if(KernelAudioInitialize() )
	{
		//GEH kernel->AudioSetVolReduction(FALSE);
		// Please leave this as TRUE unless you talk with
		// Greg about why you need to change it.
#ifdef _WINDOWS
		kernel->AudioSetVolReduction(TRUE);
#else
		kernel->AudioSetVolReduction(FALSE);
#endif
		AudioInitialized = TRUE;
		kernel->AudioSetChannels(2);

//		kernel->AudioVolumeAll(127);
#ifndef _WINDOWS
		kernel->AudioDefaultBits();
		kernel->AudioSetRate(22050);
#else
		hMyWnd = hwndApp;
#endif
	}
#endif

	InitSoundThings();

}

/**********************************************************************
Function ShutDownSoundPlayer
Purpose  called from winsys\machine.c pcsys\machine.c
**********************************************************************/
void ShutDownSoundSystem(void)
{
#if defined (AUDIO_LIB)
	if(AudioInitialized)
		KernelAudioTerminate();
#endif

	remove("499.wav");

	AudioInitialized = FALSE;
}

/*****************************************************************************
Function InitSoundThings
purpose	zero out environmental sound array
****************************************************************************/
extern "C"
void InitSoundThings(void)
{
	SHORT i;
	for (i=0;i < MAX_SOUND_THINGS;i++)
	{
		LoopingSoundThing[i].ThingTypeIndex = fERROR;
		LoopingSoundThing[i].InstanceTag = fERROR;						
	}
}

/*************************************************************************
Function ServiceDosAudio
Purpose  prevent breakup up sound in DOS
**************************************************************************/
void ServiceSOLAudio(void)
{
	static SHORT FramesTilNextSound = 300;
	if(!AudioInitialized )
		return;

#ifdef AUDIO_LIB
	if( (SFXVolumeIndex > 0 )|| fMusic)
		KernelAudioService();
#endif

	if(PlayEnvironmentalSounds && UIPlayEnvironmentalSounds && SFXVolumeIndex)
	{
		// play random environmental sounds at random number of frames
		FramesTilNextSound--;
		if(FramesTilNextSound == 0)
		{
			// set the number of frames til the next sound
			FramesTilNextSound = (SHORT)(10 + SndRandom(300));
			// play a sound
			switch(WhichEnvironmentalSound)
			{
				case 1:
					AddSndObj((BIRTHRT_SND)AmbientSoundScene1[SndRandom(AMBIENT_SOUND_SCENE_1_TOTAL)],NULL, ENVIRON_SND);
					break;
				case 2:
					AddSndObj((BIRTHRT_SND)AmbientSoundScene2[SndRandom(AMBIENT_SOUND_SCENE_2_TOTAL)],NULL, ENVIRON_SND);
					break;
				case 3:
					AddSndObj((BIRTHRT_SND)AmbientSoundScene3[SndRandom(AMBIENT_SOUND_SCENE_3_TOTAL)],NULL, ENVIRON_SND);
					break;
				case 4:
					AddSndObj((BIRTHRT_SND)AmbientSoundScene4[SndRandom(AMBIENT_SOUND_SCENE_4_TOTAL)],NULL, ENVIRON_SND);
					break;
				case 5:
					AddSndObj((BIRTHRT_SND)AmbientSoundScene5[SndRandom(AMBIENT_SOUND_SCENE_5_TOTAL)],NULL, ENVIRON_SND);
					break;
			}
		}
	}
#ifdef AUDIO_LIB
	kernel->AudioActiveSamples();
#endif
}

/* ========================================================================
   Function    - FadeToReset
	Description - fades master volume to 0 then re-inits the sound system
	Returns     - remaining count
	======================================================================== */
LONG FadeToReset ( void )
{
	SoundFade = FADE_TO_RESET_COUNT;
	SoundVolume = sfxvol;
	MusicVolume = musvol;
	lTime = 0;
	
	// do what we need to fade the sounds to zero and reset sound system
	while (SoundFade > 0)
	{
		// check each tick count
		if (lTime <= get_time())
		{
			lTime = get_time() + 1;
			
			// -- Decay sound to quiet
			SoundVolume /= 2;
			if (SoundVolume < 1)
				SoundVolume = 1;
			SetGlobalVolume(SoundVolume);
			
			// -- Decay music to quiet
			MusicVolume /= 2;
			if (MusicVolume < 1)
				MusicVolume = 1;
			UISetMusicVolume(MusicVolume);
			
			// -- SoundFade should approach 0
			// -- If Greater then zero, fade down
			// -- If Zero, reset
			SoundFade--;
			
			// -- if this is the fade that result in the
			// -- reset
			if (SoundFade <= 0)
			{
				SoundFade = 0;	// done
				
				// -- shut down
				ShutDownSoundSystem();
				StopRedBook();
				
				// -- restart
				InitializeSoundSystem();
				InitRedBook();
				
				// -- restore volume levels
				SetGlobalVolume(sfxvol);
				UISetMusicVolume(musvol);
			}
		}
		run_timers();
	}
	return 0;
}


/*************************************************************************
Function Check for sound playing
Purpose
**************************************************************************/
BOOL	CheckSoundPlaying (void)
{
	if(!AudioInitialized)
		return FALSE;
#ifdef WINDOWS_SOUND
	return FALSE;
#else
	BOOL	rv = FALSE;
#ifdef AUDIO_LIB
	rv = (BOOL)kernel->AudioActiveSamples();
#endif
	return rv;
#endif
}
/*************************************************************************
Function
Purpose
**************************************************************************/
BOOL CheckASound(int sound, int tag)
{
	int vol;

	if(!AudioInitialized)
		return FALSE;
#ifdef WINDOWS_SOUND
	return FALSE;
#else
	BOOL	rv = FALSE;
#ifdef AUDIO_LIB
	if( (tag > 0) && (sound > 0) )
		rv = (BOOL)kernel->AudioSampleVolume(sound, &vol,tag);
#endif
	return rv;
#endif



}


/*************************************************************************
Function
Purpose
**************************************************************************/
void	StopASound (int reqSound, int SoundTag)
{
	if(!AudioInitialized)
		return;
#ifdef WINDOWS_SOUND
	PauseWave( reqSound );
#else
#ifdef AUDIO_LIB
	if( (SoundTag > 0) &&(reqSound > 0))
		kernel->AudioStop(reqSound,SoundTag);
#endif
#endif
}

/*////////////////////////////////////////////////////////////
	Function TurnOffAllSounds
	purpose  Turn off all the sounds

////////////////////////////////////////////////////////////*/

void TurnOffAllSounds(void)
{
	//SoundFade = 12 * ( (SFXVolumeIndex < GetMusicVolume() ) ? SFXVolumeIndex : GetMusicVolume() );
	KillSoundsNoFade();
}
/*?????????????????????????????????????????????????????????????????
	Funtion KillSoundsNoFade
	purpose Turn off all sounds without fading
????????????????????????????????????????????????????????????????? */

void KillSoundsNoFade(void)
{
	StopRedBook();
	if(!AudioInitialized)
		return;
#ifdef WINDOWS_SOUND
	StopAllWave();
#else
#ifdef AUDIO_LIB
	kernel->AudioStopAll();
#endif
#endif
	SoundFade = fERROR;
//	SuspendMusic();
}
/********************************************************
	Function PauseAllSounds
	Purpose  Pause all sounds, esp if window loses focus
*******************************************************/
void PauseAllSounds(void)
{
 // this is taken care of in SetGlobalVolume, which doesn't set global volume, it just
// sets the volume for soundfx, not for music
}
/**********************************************************
	Function ResumePausedSounds
	Purpose  Resume sounds that were paused with PauseAllSounds
*********************************************************/
void ResumePausedSounds(void)
{
// this doesn't do anything, see PauseAllSounds, above
}
/****************************************************************	
Function SetGlobalVolume(int volume)
purpose  reset the global volume level for wave files
arguments   0 - 127
return none
****************************************************************/	
void SetGlobalVolume(SHORT volume)
{

	if(AudioInitialized)
		SFXVolumeIndex = volume;
}
/******************************************************************
Function	GetGlobalVolume
Purpose		Get the current global volume setting
Arguments	none
return		The global volume	
******************************************************************/	
SHORT GetGlobalVolume(void)
{
	if(!AudioInitialized)
		return (0);

	return (SFXVolumeIndex);
}
/************************************************************************
	Function SetMusicVolume

************************************************************************/	
void SetMusicVolume(int CurrentSong,int volume,int CurrentTag)
{
#ifdef AUDIO_LIB   //xxx
	if( (CurrentTag > 0) && (CurrentSong > 0) )
		kernel->AudioVolume(CurrentSong,volume,CurrentTag);
#endif	
}

/*/////////////////////////////////////////////////////////////////
Function	ServiceMusicWave
Purpose		update the music wave in the demo
argument	TRUE starts a new wave NOW
/////////////////////////////////////////////////////////////////*/
