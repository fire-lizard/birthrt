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
#include "SYSTEM.H"
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
	
#if defined(SCENE_EDITOR)
	void mfInitVals ();
	void mfWriteCameraData(FILE *fp);
	
	LONG mfCountOfChanges();
	void mfSetCameraAccelleration(LONG );
#endif
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
#if defined(SCENE_EDITOR)
inline void CAMERA_SETUP_DATA::mfInitVals () 
{
	fbCameraAccellerationSet = FALSE;
	fCameraAccelleration = SHRT_MAX;
	
	// Reset the global variable too.
}
#endif

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

#if defined(SCENE_EDITOR)
inline void CAMERA_SETUP_DATA::mfSetCameraAccelleration(LONG val ) 
{
	fbCameraAccellerationSet = TRUE;
	fCameraAccelleration = val;
	
	// Set the global variable too.
}
#endif

#endif // SCNCAMR_HXX
