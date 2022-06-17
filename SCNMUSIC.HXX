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
	return Result;
}
#endif // _SCNMUSIC_HXX
