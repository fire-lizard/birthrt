 /* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: Sound.hxx
   Author:   chris barker
   ========================================================================
   Contains the following general functions:
   
   
   ======================================================================== */

#ifndef _SOUND_HXX
#define _SOUND_HXX   
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif


#if defined (__cplusplus)
extern "C" {
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define VOLUME_FULL		-1 
#define VOLUME_NINETY	-2 
#define VOLUME_EIGHTY	-3 
#define VOLUME_SEVENTY	-4 
#define VOLUME_SIXTY	-5 
#define VOLUME_FIFTY	-6 
#define VOLUME_FORTY	-7 
#define VOLUME_THIRTY	-8 
#define VOLUME_TWENTY	-9 
#define VOLUME_TEN	-10
#define VOLUME_OFF	-11
#define VOLUME_PRIORITY -12
	
#define ADVENTURE_MUSIC 7
 
#define INSTALL_SMALL   0
#define INSTALL_MEDIUM 1
#define INSTALL_LARGE   2
   /* ------------------------------------------------------------------------
	Macros   
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
extern BOOL PlayEnvironmentalSounds;
extern BOOL UIPlayEnvironmentalSounds;
extern SHORT WhichEnvironmentalSound;
extern BOOL intro;
extern SHORT UserNumSFXPlaying;  // default 8, which is the maximum, I think.
#if defined(_DEMO)
extern BOOL SlideShowPlaying;
#endif

extern int AdventureMusic[];
/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------
   Enum
   ------------------------------------------------------------------------ */
typedef enum {
	NOSOUND = 0,
	BAT_SOUNDS,
	CROW_SOUNDS,
	DOG_SOUNDS,
	HARPY_SOUNDS,
	HELL_HOUND_SOUNDS,
	HORSE_SOUNDS,
	SPIDER_SOUNDS,
	SKELETON_SOUNDS,
	WYVERN_SOUNDS,
	FIRE_SOUNDS,
	TORCH_SOUNDS,
	FOUNTAIN_SOUNDS,
	MALE_SERVANT_SOUNDS,
	MALE_SOLDIER_SOUNDS,
	MALE_LORD_SOUNDS,
	FEMALE_SERVANT_SOUNDS,
	FEMALE_SOLDIER_SOUNDS,
	FEMALE_LORD_SOUNDS,
	ANKHEG_SOUNDS,
	OGRE_SOUNDS,
	SPECTRE_SOUNDS,
	WRAITH_SOUNDS,
	GOBLIN_SOUNDS,
	ZOMBIE_SOUNDS,
	GORGON_SOUNDS,
	RHUOBHE_SOUNDS,
	SPIDER_KING_SOUNDS,
	GARGOYLE_SOUNDS,
	OTYUGH_SOUNDS,
	GNOLL_SOUNDS,
    FIREBALL_SOUNDS,
    PLASMA_SOUNDS
        
} SOUND_TYPES;


// redbook track selections
enum {
	REDBOOK_INTRO = 6,
	REDBOOK_REALM = 5,
	REDBOOK_BATTLE = 2,
	REDBOOK_PERSCOMBAT =  2,
};

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
#include "soundfx.hxx"

// now build the totals list
#ifdef WINDOWS_SOUND
#include "totals.h"
#endif

/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */

/* -----------------------------------------------------------------
   Prototypes
   ----------------------------------------------------------------- */
void SuspendMusic(void);
void ResumeSuspendedMusic(void);
int GetMusicVolume(void);

void SetMusicVolume(int,int,int);
void UISetMusicVolume(int);

BOOL CheckASound(int , int );

LONG FadeToReset ( void );

void SetGlobalVolume(SHORT);
SHORT GetGlobalVolume(void);

BOOL SetSoundDecay(SHORT , SHORT );

LONG AddLoopingSound(BIRTHRT_SND , LONG );
void RemoveLoopingSound(LONG );
   
void PlayMusicWave(void);
void ServiceMusicWave(BOOL);

void PauseAllSounds(void);
void ResumePausedSounds(void);

void SndCueWave(BIRTHRT_SND, BOOL, LONG,LONG);
SHORT AddSndObj( BIRTHRT_SND  , SHORT , LONG );

void InitializeSoundSystem(void);
void ShutDownSoundSystem(void);
void TurnOffAllSounds(void);
void KillSoundsNoFade(void);
void	StopASound (int reqSound, int SoundTag);

//#ifndef _WINDOWS
void ServiceSOLAudio(void);
//#endif

void UpdateLoopingSoundObjects(void);
void InitSoundThings(void);
void AddThingToSoundArray(SOUND_TYPES, LONG);

unsigned long SndRandom ( LONG val );


/* -----------------------------------------------------------------
   Globals
   ----------------------------------------------------------------- */

// -- Required variables
// GWP Add back later.
// extern BOOL fSound;

// extern BOOL fSpeech;

 // -- provided by application
BOOL KillSplash( void );


#if defined (__cplusplus)
}
#endif

// SndUtilS.c
#endif //_SOUND_HXX
