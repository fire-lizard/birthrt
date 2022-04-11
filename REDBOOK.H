/* ========================================================================
   Copyright (c) 1990,1997	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _REDBOOK_H
#define _REDBOOK_H

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

extern int				fMusic;					/* redbook.c */
extern SHORT			UserCDVolume;			/* redbook.c */
extern SHORT	ucWhichTrack;			// redbook.c

#if defined (__cplusplus)
}
#endif

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

/* redbook.c */
int PlayRedBook (ULONG start, ULONG length);
int StopRedBook (void);
int PlayTrack (SHORT track);
unsigned long FindTrackStart (unsigned char track);
int CheckCDBusy (void);
int PlayCDMusic (unsigned short tune);
void LoadMusicFromCD(short);


#if defined (__cplusplus)
}
#endif
#endif // _REDBOOK_H
