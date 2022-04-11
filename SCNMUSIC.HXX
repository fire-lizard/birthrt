/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _SCNMUSIC_HXX
#define _SCNMUSIC_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_FILEUTIL_H)
#include "fileutil.h"
#endif

#if !defined(_FILENAME_HXX)
#include "filename.hxx"
#endif

#if defined _WINDOWS && defined(OLD_SOUND)
#if !defined(_SNDUTIL_H)
#include "winsys\sndutil.h"
#endif
//#include "winsys\mciutil.h"
#if !defined(_MUSIUTIL_H)
#include "winsys\musiutil.h"
#endif
#endif
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
   Prototypes
   ------------------------------------------------------------------------ */
class SCENE_MUSIC_DATA {
public:
#if defined (SCENE_EDITOR)
	BOOL mfIsSet();
	BOOL mfIsBackgroundSet();
#endif
	char * mfGetBackgroundMusic();
	void mfWriteData(FILE *fp);
	
	LONG mfReadData(FILE *fp);
	
protected:
private:
    void *operator new( size_t stAllocateBlock);
    void operator delete( void *p );
    SCENE_MUSIC_DATA(const SCENE_MUSIC_DATA &);                   // No copies
    const SCENE_MUSIC_DATA operator=(const SCENE_MUSIC_DATA &);   // No assignments.
    
    FILENAME	fBackgroundMusic;
};
/* ------------------------------------------------------------------------
   inline functions   
   ------------------------------------------------------------------------ */
   
/* ========================================================================
   Function    - mfIsSet
   Description - Test for whether this is a demo scene or not.
   Returns     - 
   ======================================================================== */
#if defined (SCENE_EDITOR)
inline BOOL SCENE_MUSIC_DATA::mfIsSet() 
{
	return fBackgroundMusic.mfIsSet();
}
#endif

/* ========================================================================
   Function    - mfIsBackgroundSet
   Description - Test for whether The background music is set
   Returns     - 
   ======================================================================== */
#if defined (SCENE_EDITOR)
inline BOOL SCENE_MUSIC_DATA::mfIsBackgroundSet() 
{
	return fBackgroundMusic.mfIsSet();
}
#endif

/* ========================================================================
   Function    - mfGetBackgroundMusic
   Description - Get the name of the Background Music.
   Returns     - 
   ======================================================================== */
inline char * SCENE_MUSIC_DATA::mfGetBackgroundMusic() 
{
	return fBackgroundMusic.mfGetFileName();
}
/* ========================================================================
   Function    - mfWriteData
   Description - Write the scene file data.
   Returns     - 
   ======================================================================== */
inline void SCENE_MUSIC_DATA::mfWriteData(FILE *fp) 
{
	fBackgroundMusic.mfWriteFileName(fp);
}

/* ========================================================================
   Function    - mfReadData
   Description - Read the scene file data.
   Returns     - 
   ======================================================================== */
inline LONG SCENE_MUSIC_DATA::mfReadData(FILE *fp) 
{
	LONG Result;
	
	fBackgroundMusic.mfInitVals();
	
	Result = fBackgroundMusic.mfReadFileName(fp);
#ifdef _WINDOWS
#ifdef OLD_SOUND
	if (Result != EOF)
	{
		StopMusic();
		FinalMusic();
		InitMusic ( mfGetBackgroundMusic(), ".", FALSE ); 
		StartMusic();
	}
#endif
#endif
		return Result;
}
#endif // _SCNMUSIC_HXX
