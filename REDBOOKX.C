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
#include "winsys\mciutil.h"
extern HWND	hwndApp;
#endif
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "typedefs.h"
#include "sound.hxx"
#include "redbook.h"
#include "system.h"
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

//static int CurrentSong = fERROR;
//static int CurrentTag = fERROR;
//static BOOL MusicSuspended = FALSE;
SHORT UserCDVolume = 100;

SHORT ucWhichTrack = 0;
//static  SHORT volume_levels[] = {0, 13, 26, 38, 51, 64, 77, 90, 102, 115, 127};

extern unsigned char Tunes[];
//static int MusicVolume[] = {	
// 	VOLUME_OFF  ,
// 	VOLUME_TEN    ,
// 	VOLUME_TWENTY ,
// 	VOLUME_THIRTY ,
// 	VOLUME_FORTY ,
// 	VOLUME_FIFTY ,
// 	VOLUME_SIXTY ,
// 	VOLUME_SEVENTY ,
// 	VOLUME_EIGHTY ,
// 	VOLUME_NINETY ,
// 	VOLUME_FULL
// };
//static int VolumeIndex = 10;
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
	
	// if(fCDAvailible)
	// 	fMusic = 1;
	// else
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
		return (0);
	}
}
/* ========================================================================
   Function    - CheckCDError
   Description - check the CDROM for its availablity
   Returns     - return TRUE or FALSE, TRUE if NOT AVAILABLE
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


