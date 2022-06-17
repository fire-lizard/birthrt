/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _SCNVER_HXX
#define _SCNVER_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
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
class SCENE_FILE_VERSION {
public:
	enum { NOT_SET = -1,
	       CURRENT_MAJOR = 1,
	       CURRENT_MINOR = 0 };
	
	LONG mfReadVersion(FILE *);
	void mfInitVals ();
	BOOL mfIsSet();
	
protected:
private:
	// No constructor or destructor.
	SCENE_FILE_VERSION();
	~SCENE_FILE_VERSION();
	void *operator new (size_t stAllocateBlock);	// Can't use new/delete
	void operator delete (void *p);
	SCENE_FILE_VERSION(const SCENE_FILE_VERSION &);			// No copies
	const SCENE_FILE_VERSION operator=(const SCENE_FILE_VERSION &);	// No assignments
	
	SHORT	fsMajor;
	SHORT	fsMinor;
};
	
	
/* ========================================================================
   Function    - mfInitVals
   Description - Initialize the member data.
   Returns     - 
   ======================================================================== */
inline void SCENE_FILE_VERSION::mfInitVals () 
{
	fsMajor = NOT_SET;
	fsMinor = NOT_SET;
}

/* ========================================================================
   Function    - mfIsSet
   Description - Test for whether the version has been set.
   Returns     - 
   ======================================================================== */
inline BOOL SCENE_FILE_VERSION::mfIsSet() 
{
	BOOL Result = TRUE;
	if (fsMajor == SCENE_FILE_VERSION::NOT_SET ||
	    fsMinor == SCENE_FILE_VERSION::NOT_SET)
	{
		Result = FALSE;
	}
	return Result;
}

#endif // _SCNVER_HXX
