/*#######################################################################\
#
#	Synergistic Software
#	(c) Copyright 1993-1996  All Rights Reserved.
#
\#######################################################################*/

/*****************************************************************************
REDBOOK.C

Game specific routines
------------------------------------------------------------------------------
*****************************************************************************/
#ifndef _WINDOWS
#else
#include "PLATFORM.H"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "TYPEDEFS.H"
#include "SOUND.HXX"
#include "REDBOOK.H"
#include "SYSTEM.H"
/*#######################################################################\
#    Prototypes
\#######################################################################*/

int PlayTrack (SHORT track);
int CheckCDBusy (void);

/*#######################################################################\
#    Global variables
\#######################################################################*/
#if defined (_WINDOWS)
extern BOOL fMusic;
#else
int fMusic = 1;
#endif

static int CurrentSong = fERROR;
static int CurrentTag = fERROR;
static BOOL MusicSuspended = FALSE;
static SHORT PendingTrack = fERROR;

SHORT ucWhichTrack = 0;
static  SHORT volume_levels[] = {0, 13, 26, 38, 51, 64, 77, 90, 102, 115, 127};

static int MusicVolume[] = {	
	VOLUME_OFF  ,
	VOLUME_TEN    ,
	VOLUME_TWENTY ,
	VOLUME_THIRTY ,
	VOLUME_FORTY ,
	VOLUME_FIFTY ,
	VOLUME_SIXTY ,
	VOLUME_SEVENTY ,
	VOLUME_EIGHTY ,
	VOLUME_NINETY ,
	VOLUME_FULL
};
static int VolumeIndex = 10;
/*#######################################################################\
#    Routines
\#######################################################################*/

int PlayTrack (SHORT track)
{
	
	if(!fMusic || MusicSuspended)
	{
		// Remember what was asked for while suspended so the track isn't
		// lost - ResumeSuspendedMusic() starts it.
		if(MusicSuspended && fMusic)
		{
			PendingTrack = track;
			ucWhichTrack = track;
		}
		return 0;
	}

	PendingTrack = fERROR;

	ucWhichTrack = track;

	if(CurrentTag != fERROR)
			StopASound(CurrentSong,CurrentTag);

	{
		switch(track)
		{
			case SND_WIN_MUSIC1:
				CurrentSong = (BIRTHRT_SND) (SND_WIN_MUSIC1 + random(SND_WIN_MUSIC_TOTAL) );
				CurrentTag=AddSndObj((BIRTHRT_SND)CurrentSong,NULL,VOLUME_FULL);
				break;
			case SND_LOSE_MUSIC1:
				CurrentSong = (BIRTHRT_SND) (SND_LOSE_MUSIC1 + random(SND_LOSE_MUSIC_TOTAL) );
				CurrentTag=AddSndObj((BIRTHRT_SND)CurrentSong,NULL,VOLUME_FULL);
				break;
			case SND_LOSE_COMBAT_MUSIC1:
				CurrentSong = (BIRTHRT_SND) (SND_LOSE_COMBAT_MUSIC1 + random(SND_LOSE_COMBAT_MUSIC_TOTAL) );
				CurrentTag=AddSndObj((BIRTHRT_SND)CurrentSong,NULL,VOLUME_FULL);
				break;
			case SND_WIN_GAME_MUSIC1:
				CurrentSong = (BIRTHRT_SND) (SND_WIN_GAME_MUSIC1 + random(SND_WIN_GAME_MUSIC_TOTAL) );
				CurrentTag=AddSndObj((BIRTHRT_SND)CurrentSong,NULL,VOLUME_FULL);
				break;
			default:
			{
				// Play tracks straight from the local MUSIC directory.
				// Tracks 102 (battle) and 105 (realm) were Red Book audio
				// on the retail CD and are not in the install's MUSIC set;
				// substitute nearby tracks so those scenes have music.
				char	szSong[32];
				SHORT	id = (track < 100) ? (SHORT)(track + 100) : (SHORT)track;

				sprintf(szSong, "MUSIC\\%d.WAV", id);
				if (access(szSong, 0) != 0)
					id = (id == 102) ? 104 : 101;

				CurrentSong = id;
				CurrentTag = AddSndObj((BIRTHRT_SND)CurrentSong,NULL,VOLUME_FULL);
				break;
			}
			}
		
	}
	return (0);
}

/*#######################################################################\
#    Function
\#######################################################################*/

int CheckCDBusy (void)
{
	if(CheckASound(CurrentSong,CurrentTag )  )
		return(1);
	else
	{
		CurrentTag = CurrentSong = fERROR;
		return (0);
	}
}

void UISetMusicVolume(int volume)
{
	static int OldVolume = 10;
	{
		if(volume < 0)				// stale options file can hold a negative musvol
			volume = 0;
		VolumeIndex = volume;
		if(volume == 0)
		{
			if(CurrentTag != fERROR)
				StopASound(CurrentSong,CurrentTag);
			CurrentTag = fERROR;
			fMusic = 0;
		}
		else
		{
			if(OldVolume == 0)
			{
				PlayTrack(ucWhichTrack);	
			}
			else
			{
				if(CurrentTag != fERROR)
					SetMusicVolume(CurrentSong,volume_levels[VolumeIndex],CurrentTag);
			}
			fMusic = 1;
		}
		OldVolume = volume;
	}
}

void SuspendMusic(void)
{
	MusicSuspended = TRUE;
}

void ResumeSuspendedMusic(void)
{
	MusicSuspended = FALSE;
	if(fMusic && PendingTrack != fERROR)
	{
		SHORT DeferredTrack = PendingTrack;
		PendingTrack = fERROR;
		PlayTrack(DeferredTrack);
	}
}

BOOL IsMusicSuspended(void)
{
	return MusicSuspended;
}

int GetMusicVolume(void)
{
	return(VolumeIndex);
}
