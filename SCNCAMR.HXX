/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _SCNCAMR_HXX
#define _SCNCAMR_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#if !defined(_SYSTEM_H)
#include "system.h"
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
class CAMERA_SETUP_DATA {
public:
	CAMERA_SETUP_DATA();
	
	~CAMERA_SETUP_DATA() {};
	
	LONG mfReadCameraData(FILE *);
	
	void mfInitVals ();
	
#if defined(SCENE_EDITOR)
	void mfWriteCameraData(FILE *fp);
	
	LONG mfCountOfChanges();
#endif

	void mfSetCameraAccelleration(LONG );
protected:
private:
	void *operator new (size_t stAllocateBlock);	// Can't use new/delete
	void operator delete (void *p);
	CAMERA_SETUP_DATA(const CAMERA_SETUP_DATA &);			// No copies
	const CAMERA_SETUP_DATA operator=(const CAMERA_SETUP_DATA &);	// No assignments
	
#if defined(SCENE_EDITOR)
	BOOL	fbCameraAccellerationSet;
	LONG	fCameraAccelleration;
#endif
	
};

/* ========================================================================
   Function    - mfInitVals
   Description - Initialize the member data.
   Returns     - 
   ======================================================================== */
inline void CAMERA_SETUP_DATA::mfInitVals () 
{
#if defined(SCENE_EDITOR)
	fbCameraAccellerationSet = FALSE;
	fCameraAccelleration = SHRT_MAX;
	
#endif
	// Reset the global variable too.
}

/* ========================================================================
   Function    - CAMERA_SETUP_DATA
   Description - Constructor
   Returns     - 
   ======================================================================== */
inline CAMERA_SETUP_DATA::CAMERA_SETUP_DATA() 
{
	mfInitVals();
}

/* ========================================================================
   Function    - mfWriteCameraData
   Description - Write the scene camera data.
   Returns     - 
   ======================================================================== */
#if defined(SCENE_EDITOR)
inline void CAMERA_SETUP_DATA::mfWriteCameraData(FILE *fp) 
{
	if (fbCameraAccellerationSet == TRUE)
	{
		fprintf(fp, "Accelleration %ld\n", fCameraAccelleration);
	}
}

/* ========================================================================
   Function    - mfCountOfChanges
   Description - Write the first line of the scene camera data.
   Returns     - 
   ======================================================================== */
inline LONG CAMERA_SETUP_DATA::mfCountOfChanges() 
{
	return (fbCameraAccellerationSet);
}
#endif

/* ========================================================================
   Function    - SCENE_AI
   Description - Constructor
   Returns     - 
   ======================================================================== */
#if defined(SCENE_EDITOR)
inline void CAMERA_SETUP_DATA::mfSetCameraAccelleration(LONG val ) 
{
	fbCameraAccellerationSet = TRUE;
	fCameraAccelleration = val;
	
	// Set the global variable too.
}
#else
inline void CAMERA_SETUP_DATA::mfSetCameraAccelleration(LONG ) 
{
}
#endif

#endif // SCNCAMR_HXX
