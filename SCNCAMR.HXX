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

#endif // SCNCAMR_HXX
