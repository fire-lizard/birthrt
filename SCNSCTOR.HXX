/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _SCNSCTOR_HXX
#define _SCNSCTOR_HXX

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

#if !defined(_SCNINFO_HXX)
#include "scninfo.hxx"
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

typedef SHORT HDL_SECTOR;

class SCN_SECTOR {
public:
	void mfInitVals();
	void mfCreateSector(char * cpBuffer);
#if defined(SCENE_EDITOR)
	void mfWriteSectorData(FILE * /* fp */);
#endif
	LONG mfGetId();
	char *mfGetSceneName();
	
protected:
private:
	SCN_SECTOR();
	~SCN_SECTOR();
	void *operator new (size_t stAllocateBlock);	// Can't use new/delete
	void operator delete (void *p);
	SCN_SECTOR(const SCN_SECTOR &);			// No copies
	const SCN_SECTOR operator=(const SCN_SECTOR &);	// No assignments
	
	LONG	fId;
	SCENE_INFO	fscInfo;
};
typedef SHORT HDL_SCN_SECTORS;
typedef SCN_SECTOR *PTR_SCN_SECTOR;
typedef SCN_SECTOR *PTR_SCN_SECTORS;
	
/* ========================================================================
   Function    - mfInitVals
   Description - Initialize the member data.
   Returns     - 
   ======================================================================== */
inline void SCN_SECTOR::mfInitVals() 
{
	fId = fERROR;
	fscInfo.mfInitVals();
}

/* ========================================================================
   Function    - mfCreateSector
   Description - Make an Sector datum from the buffer.
   Returns     - 
   ======================================================================== */
inline void SCN_SECTOR::mfCreateSector(char * cpBuffer)
{
	mfInitVals();
	
	const LONG ScanResult = sscanf(cpBuffer, "%ld %s", &fId, fscInfo.mfGetSceneName());
	
	if ( 2 != ScanResult)
	{
		fId = fERROR;
	}
}

/* ========================================================================
   Function    - mfWriteSectorData
   Description - Write the Sector Scene file data.
   Returns     - 
   ======================================================================== */
#if defined(SCENE_EDITOR)
inline void SCN_SECTOR::mfWriteSectorData(FILE * fp) 
{
	fprintf(fp, "%ld\t%s\n", fId, fscInfo.mfGetSceneName());
}
#endif

/* ========================================================================
   Function    - mfGetId
   Description - Get the Id.
   Returns     - 
   ======================================================================== */
inline LONG SCN_SECTOR::mfGetId() 
{
	return fId;
}

/* ========================================================================
   Function    - mfGetSceneName
   Description - Get the scene name.
   Returns     - 
   ======================================================================== */
inline char *SCN_SECTOR::mfGetSceneName() 
{
	return fscInfo.mfGetSceneName();
}
#endif // _SCNSCTOR_HXX
