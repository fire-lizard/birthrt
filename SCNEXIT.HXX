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
#include "TYPEDEFS.H"
#endif

#if !defined(_SCNSCTOR_HXX)
#include "SCNSCTOR.HXX"
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

class EXIT_POINT {
public:
	
	void mfCreateExitPoint(char * cpBuffer);
#if defined(SCENE_EDITOR)
	void mfWriteExitPointData(FILE * fp);
#endif
	LONG mfGetId();
	char *mfGetSceneName();
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
	
};

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
   Function    - mfWriteExitPointData
   Description - Write the data to the scene file.
   Returns     - 
   ======================================================================== */
#if defined(SCENE_EDITOR)
inline void EXIT_POINT::mfWriteExitPointData(FILE * fp) 
{
	fExitInfo.mfWriteSectorData(fp);
}

#endif
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
   Function    - mfInitVals
   Description - Initialize the member data.
   Returns     - 
   ======================================================================== */
inline void EXIT_POINT::mfInitVals() 
{
	fExitInfo.mfInitVals();
}

#endif // _SCNEXIT_HXX
