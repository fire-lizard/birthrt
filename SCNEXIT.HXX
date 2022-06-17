/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _SCNEXIT_HXX
#define _SCNEXIT_HXX

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

#if !defined(_SCNSCTOR_HXX)
#include "scnsctor.hxx"
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
// Note: I reused the SECTOR class in the EXIT_POINT class because they 
//       both store the exact same data. An Id and a SCENE_INFO.
//       However its an agregate not an inherited model.

typedef SHORT HDL_EXIT_POINT;

class EXIT_POINT {
public:
	
	void mfCreateExitPoint(char * cpBuffer);
	LONG mfGetId();
	char *mfGetSceneName();
	static char * mfGetDefaultScene();
	void mfInitVals();
	
protected:
private:
	EXIT_POINT();
	~EXIT_POINT();
	void *operator new (size_t stAllocateBlock);	// Can't use new/delete
	void operator delete (void *p);
	EXIT_POINT(const EXIT_POINT &);			// No copies
	const EXIT_POINT operator=(const EXIT_POINT &);	// No assignments
	
	SCN_SECTOR fExitInfo;
	
	static char * fcpDefaultScene;
};

typedef SHORT HDL_EXIT_POINTS;
typedef EXIT_POINT *PTR_EXIT_POINT;
typedef EXIT_POINT *PTR_EXIT_POINTS;


/* ========================================================================
   Function    - mfCreateExitPoint
   Description - Make an exit point from the buffer.
   Returns     - 
   ======================================================================== */
inline void EXIT_POINT::mfCreateExitPoint(char * cpBuffer) 
{
	fExitInfo.mfCreateSector(cpBuffer);
}

/* ========================================================================
   Function    - mfGetId
   Description - Get this exit id.
   Returns     - 
   ======================================================================== */
inline LONG EXIT_POINT::mfGetId() 
{
	return fExitInfo.mfGetId();
}

/* ========================================================================
   Function    - mfGetSceneName
   Description - Get the scene name for this exit id.
   Returns     - 
   ======================================================================== */
inline char *EXIT_POINT::mfGetSceneName() 
{
	return fExitInfo.mfGetSceneName();
}
	
/* ========================================================================
   Function    - mfGetDefaultScene
   Description - If an exit point was crossed and no scene specified go here.
   Returns     - 
   ======================================================================== */
inline char * EXIT_POINT::mfGetDefaultScene()
{
	return fcpDefaultScene;
}

/* ========================================================================
   Function    - mfInitVals
   Description - Initialize the member data.
   Returns     - 
   ======================================================================== */
inline void EXIT_POINT::mfInitVals() 
{
	fExitInfo.mfInitVals();
}

#endif // _SCNEXIT_HXX
