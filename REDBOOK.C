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
#include <i86.h>
#else
#include <windows.h>
#include "WINSYS\MCIUTIL.H"
extern HWND	hwndApp;
#endif
#include <io.h>
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

#if _SYS_DEBUG
void PrintRedError (void);
#endif

SHORT GetCDTrack(void);
int InitRedBook(void);
int ShutdownRedBook(void);
int PlayRedBook (unsigned long start, unsigned long length);
int StopRedBook (void);
int PlayTrack (SHORT track);
unsigned long FindTrackStart (unsigned char track);
int CheckCDBusy (void);
//int PlayCDMusic (unsigned short tune);

/*#######################################################################\
#    Defines
\#######################################################################*/

#define USERINTR		0x69

#define STOPCOMMAND		0x80000001
#define PLAYTRACKCMD	0x80000002
#define FINDTRACKCMD	0x80000003
#define CHECKBUSYCMD	0x80000004
#define THE_SONG_ON_THE_HARD_DRIVE 499
#define TUNES			17
/*#######################################################################\
#    Global variables
\#######################################################################*/
#ifndef _WINDOWS
union REGS inregs;
unsigned long *int6a = (unsigned long *)(0x000001a8);
unsigned long *int6b = (unsigned long *)(0x000001ac);
#endif

BOOL fCDAvailible = FALSE;

#if defined (_WINDOWS)
extern BOOL fMusic;
#else
int fMusic = 1;
#endif

static int CurrentSong = fERROR;
static int CurrentTag = fERROR;
static BOOL MusicSuspended = FALSE;
SHORT UserCDVolume = 100;

SHORT ucWhichTrack = 0;
static  SHORT volume_levels[] = {0, 13, 26, 38, 51, 64, 77, 90, 102, 115, 127};

extern unsigned char Tunes[];
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
//static int FadeMusic = fERROR;
/*#######################################################################\
#    Routines
\#######################################################################*/

#if _SYS_DEBUG
void PrintRedError (void)
{
	if (*int6a == 0xffff0000)
	{
		mprintf ("CD drive error.  No disk found.\n");
	}
	else
	if (*int6a == 0xfffe0000)
	{
		mprintf ("CD drive door open.\n");
	}
	else
	if (*int6a == 0xfffd0000)
	{
		mprintf ("Invalid command.\n");
	}
	else
	if (*int6a == 0xC0000000)
	{
		//mprintf ("Drive busy.  CDbusy = %d\n", CDbusy);
		//mprintf ("Drive busy.\n");
	}
	else
	if (*int6a == 0xA0000000)
	{
		mprintf ("Invalid track requested.\n");
	}
	else
	{
		mprintf ("CD drive error:  int6a = %08lx, int6b = %08lx\n", *int6a, *int6b);
	}
}
#endif
/*#######################################################################\
#    Function
\#######################################################################*/

int InitRedBook(void)
{
#ifdef _WINDOWS
		if(mci_OpenCD() )
			fCDAvailible = TRUE;
		else
			fCDAvailible = FALSE;
#else
	
		if(*int6a == 0xDEADC0DE)
			fCDAvailible = TRUE;
		else
			fCDAvailible = FALSE;
#endif
	//}
	//else
	//	fCDAvailible = TRUE;
	
	if(fCDAvailible)
		fMusic = 1;
	else
		fMusic = 0;
	
	return 1;
}
/*#######################################################################\
#    Function
\#######################################################################*/

int ShutdownRedBook(void)
{  // not using redbook for birthright I
#ifdef UseTheCD
#ifdef _WINDOWS
		mci_CloseCD();
#endif
#endif
	return 0;
}
/*#######################################################################\
#    Function
\#######################################################################*/
#ifdef UseTheCD
int PlayRedBook (unsigned long start, unsigned long length)
{  // not used in birthright I
	if(InstallationType == INSTALL_LARGE)
	{

#ifdef _WINDOWS
		return 0;
#else

		if(!fCDAvailible)
			return 0;
			
		if(!fMusic)
			return 0;

		*int6a = start;
		*int6b = length;
		int386 (USERINTR, &inregs, &inregs);

		if ((*int6a > 0x7fffffff))
		{
#if _SYS_DEBUG
			PrintRedError();
#endif
			return (1);
		}

#endif
	}
	return (0);
}
#endif
/*#######################################################################\
#    Function
\#######################################################################*/


int StopRedBook (void)
{
	// redbook not used in birthright I
#ifdef UseTheCD
	//	if(CurrentTag != fERROR)
//			StopASound(CurrentSong,CurrentTag);
//	FadeMusic = MusicVolume[VolumeIndex];
	if( (InstallationType == INSTALL_LARGE) && GetCDTrack() )
	{

#ifdef _WINDOWS
		mci_StopCD() ;
#else

		*int6a = 0;
		*int6b = STOPCOMMAND;
		int386 (USERINTR, &inregs, &inregs);

		if ((*int6a > 0x7fffffff))
		{
#if _SYS_DEBUG
			PrintRedError();
#endif
		}

#endif
	}
#endif
	return (1);
}

/*#######################################################################\
#    Function
\#######################################################################*/


int PlayTrack (SHORT track)
{
	
	if(!fMusic || MusicSuspended)
		return 0;
		
	ucWhichTrack = track;
	
	if(CurrentTag != fERROR)
			StopASound(CurrentSong,CurrentTag);

#ifdef UseTheCD
	if(InstallationType == INSTALL_LARGE && GetCDTrack() )
	{

#ifdef _WINDOWS
		StopRedBook();

		mci_PlayCD(hwndApp, track, 0, 0, 0, 0, 0, 0);
		return 0;
#else
		StopRedBook();

		*int6a = (unsigned long)track;
		*int6b = PLAYTRACKCMD;
		int386 (USERINTR, &inregs, &inregs);

		if ((*int6a > 0x7fffffff))
		{
#if _SYS_DEBUG
			PrintRedError();
#endif
			return (1);
		}

#endif
	}
	else
#endif
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
			case 2:
				CurrentTag = AddSndObj((BIRTHRT_SND)102,NULL,VOLUME_FULL);
				CurrentSong= 102;
				break;
			case 103:
				CurrentTag = AddSndObj((BIRTHRT_SND)103,NULL,VOLUME_FULL);
				CurrentSong= 103;
				break;
			case 5:
				CurrentTag = AddSndObj((BIRTHRT_SND)105,NULL,VOLUME_FULL);
				CurrentSong= 105;
				break;
			default:
				CurrentTag = AddSndObj((BIRTHRT_SND)THE_SONG_ON_THE_HARD_DRIVE,NULL,VOLUME_FULL);
				CurrentSong= THE_SONG_ON_THE_HARD_DRIVE;
				break;
			}
		
	}
	return (0);
}

/*#######################################################################\
#    Function
\#######################################################################*/

unsigned long FindTrackStart (unsigned char track)
{
	// redbook not used in birthright I
#ifdef UseTheCD
	if(InstallationType == INSTALL_LARGE)
	{
#ifdef _WINDOWS
		return 0;
#else

		*int6a = (unsigned long)track;
		*int6b = FINDTRACKCMD;
		int386 (USERINTR, &inregs, &inregs);

#if _SYS_DEBUG
		if ((*int6a > 0x7fffffff))
		{
			PrintRedError();
		}
#endif

		return (*int6a);					// track starting location (redbook)
#endif
	}
	else
#endif
		return 0;
}
/*#######################################################################\
#    Function
\#######################################################################*/


int CheckCDBusy (void)
{
#ifdef UseTheCD
//	if(MusicSuspended)
//		return(0);

		if(!fCDAvailible)
			return 0;

		if(!fMusic)
			return 0;


	if(0) // InstallationType == INSTALL_LARGE && GetCDTrack() )
	{


#ifdef _WINDOWS
		if((BOOL)mci_CheckCDBusy())
			return(1);
		else
			return(0);
#else
		return((int)CheckCDError());
#endif
	}
	else
#endif
	{
		if(CheckASound(CurrentSong,CurrentTag )  )
			return(1);
		else
		{
			CurrentTag = CurrentSong = fERROR;
			return (0);
		}
	}
//	return (0);
}
/* ========================================================================
   Function    - CheckCDError
   Description - check the CDROM for it's availiblity
   Returns     - return TRUE or FALSE, TRUE if NOT AVAILIBLE
   ======================================================================== */
BOOL CheckCDError(void)
{
	// if cd interrupt not started - return failure
	if(!fCDAvailible)
		return (TRUE);

#ifndef _WINDOWS
#ifdef UseTheCD

	if(!fMusic)
		return (TRUE);

	// check for cd door open

	// check the cd
	*int6a = 0L;
	*int6b = CHECKBUSYCMD;
	int386 (USERINTR, &inregs, &inregs);

	// high bit means error of some kind
	if (*int6a & 0x80000000)
	{
#if _SYS_DEBUG
		PrintRedError();
#endif
		return (TRUE);
	}
#endif
#endif

	return (FALSE);
}

/*#######################################################################\
#    Function
\#######################################################################*/

#ifdef UseTheCD

int PlayCDMusic (unsigned short tune)
{
	if(!fCDAvailible)
		return 0;
		
	if(InstallationType == INSTALL_LARGE)
	{

#ifdef _WINDOWS
		return 0;
#else
		unsigned long starttrack, start, offset, length;

		if (!fMusic)
			return 0;

		if (tune >= TUNES)
		{
#if _SYS_DEBUG
			mprintf ("Audio tune requested (%d) is out of range.  Number in CD tracks:  %d\n", tune, TUNES);
#endif
			return (1);
		}

		// TWI doesn't like "EJECTION"...
		if (tune == 2)
		{
			tune = 13;
		}

		tune *= 7;

		starttrack = FindTrackStart ((unsigned char)Tunes[tune]);
		if (starttrack == 0x80000000)			// invalid track
		{
			return (1);
		}

		offset = (Tunes[tune + 1] * 60L * 75L) +
				 (Tunes[tune + 2] * 75L) +
				 (Tunes[tune + 3]);

		length = (Tunes[tune + 4] * 60L * 75L) +
				 (Tunes[tune + 5] * 75L) +
				 (Tunes[tune + 6]) - offset;

		start = starttrack + offset;

#if _SYS_DEBUG
	//mprintf ("\nCD tune = %d   starttrack = %08lx\noffset = %08lx   length = %08lx   start = %08lx\n",
	//		tune / 7, starttrack, offset, length, start);
#endif

		return (PlayRedBook (start, length));
#endif
	}
	else
		return 0;
}
#endif
/*#######################################################################\
#    Function
\#######################################################################*/
#ifdef UseTheCD

unsigned char Tunes[] =
{
// CDROM TRACK 2				FILE = RBIMUSIC.WAV
// Tunes for Scoreboard Shows
	 2, 0, 0,35, 0, 5,20,		// Bad Call
	 2, 0, 5,60, 0,11, 7,		// Double Play
	 2, 0,11,30, 0,15,40,		// Ejection
	 2, 0,15,40, 0,19,60,		// Hit By Pitch
	 2, 0,19,60, 0,26,60,		// Triple
	 2, 0,26,60, 0,36,25,		// Yank Pitcher
	 2, 0,36,25, 0,44,50,		// Home Run
	 2, 0,44,50, 1, 7,60,		// Take Me Out to the Ball Game
	 2, 1, 7,60, 1,20,45,		// Win Game
	 2, 1,20,45, 2,02,35,		// Star Spangled Banner
	 2, 2,02,35, 2,52,30,		// O Canada

// CDROM TRACK 3				FILE = RBIMUS1.WAV
// Short tunes for atmosphere
	 3, 0, 0, 0, 0, 6,40,		// Charge
	 3, 0, 6,60, 0,33,40,		// Blue Danube
	 3, 0,34, 0, 0,53, 0,		// Hall of the Mountain King
	 3, 0,53,50, 1, 6,40,		// Speedup
	 3, 1, 7, 0, 1,28,40,		// Toccata
	 3, 1,28,50, 1,44,40,		// Turkey in the Straw
};
#endif
/*******************************************************************
	function LoadNewMusic
	purpose copy the new music from the CD to the hard drive
*******************************************************************/
void LoadMusicFromCD(short track)
{
	FILE  *in, *out;
	char n[256], o[256];
	long int length;
	SHORT hBuffer, ItemsRead;
	PTR pt;

	if(!fCDAvailible)
		return;
		
//	if(InstallationType == INSTALL_LARGE && GetCDTrack() )
//		return;

	sprintf(n, "%s%s%d.wav",CDDrive,"music\\",ucWhichTrack + 100);
	in = FileOpen(n,"rb");
	if(in != NULL)
	{
		sprintf(o,"%d.wav",THE_SONG_ON_THE_HARD_DRIVE);
		out = fopen(o, "wb");
		if(out != NULL)
		{
			fseek(in,0L,SEEK_END);
			length = ftell(in);
			hBuffer = NewBlock(16000);
			if(hBuffer != fERROR)
			{
				pt = (PTR)BLKPTR(hBuffer);
				fseek(in,0L,SEEK_SET);
				do
				{
					ItemsRead = fread(pt,1,16000,in);
					fwrite(pt,16000, 1,out);
				}while(ItemsRead == 16000);
				fwrite(pt,ItemsRead,1,out);
				fclose(out);
				DisposBlock(hBuffer);
			}
		}
		FileClose(in);
	}
}

void UISetMusicVolume(int volume)
{
	static int OldVolume = 10;
#ifdef UseTheCD
	if( ( (InstallationType == INSTALL_LARGE ) && !GetCDTrack()) || (InstallationType != INSTALL_LARGE) )
#endif
	{
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
#ifdef UseTheCD
SHORT GetCDTrack()
{
	SHORT TrackSwitch[] = {0,0,1,0,2,0,3,0,4,5,0,0,0};
	if(ucWhichTrack > 11)
		return (0);
	else
		return (TrackSwitch[ucWhichTrack] );
}
#endif

void SuspendMusic(void)
{
//	StopRedbook();
	MusicSuspended = TRUE;
}

void ResumeSuspendedMusic(void)
{
	MusicSuspended = FALSE;
}


int GetMusicVolume(void)
{
	return(VolumeIndex);
}
