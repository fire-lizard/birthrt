/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _PANORAMA_HXX
#define _PANORAMA_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>

#if !defined(_SYSTEM_H)
#include "system.h"
#endif

#if !defined(_ENGINE_H)
#include "engine.h"
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
	
// Holds the file names of the Panorama or scene scrapes.
class PANORAMA_DATA {
public:
	enum { MAX_PANORAMAS = 10,
	       MAX_FILENAME_LEN = 9};
	
	inline PANORAMA_DATA();
	inline ~PANORAMA_DATA();
	LONG mfReadData(FILE *fp);
	inline void mfInitVals();
	inline void mfReleaseData();
	inline void mfDeleteData();
	
#if defined(SCENE_EDITOR)
	inline BOOL mfIsSet();
	inline void mfWriteData(FILE *fp);
#endif

protected:
private:
	void *operator new (size_t stAllocateBlock);	// Can't use new/delete
	void operator delete (void *p);
	PANORAMA_DATA(const PANORAMA_DATA &);			// No copies
	const PANORAMA_DATA operator=(const PANORAMA_DATA &);	// No assignments
	
	SHORT	fhPanoramaFileNames;
};

/* ========================================================================
   Function    - mfInitVals
   Description - initialize the member data.
   Returns     - 
   ======================================================================== */
inline void PANORAMA_DATA::mfInitVals() {
	// Set the global values to NULL here too!
	hSky_filenames = fhPanoramaFileNames = fERROR;
}

/* ========================================================================
   Function    - PANORAMA_DATA
   Description - Constructor
   Returns     - 
   ======================================================================== */
inline PANORAMA_DATA::PANORAMA_DATA() {
	mfInitVals();
}

/* ========================================================================
   Function    - ~PANORAMA_DATA
   Description - Destructor
   Returns     - 
   ======================================================================== */
inline PANORAMA_DATA::~PANORAMA_DATA() 
{
	mfDeleteData();
}

/* ========================================================================
   Function    - mfDeleteData
   Description - called by the Destructor
   Returns     - 
   ======================================================================== */
inline void PANORAMA_DATA::mfDeleteData() 
{
	if( fhPanoramaFileNames != fERROR)
	{
		SHORT i;
		SHORT *pPanoramaFileNames = (SHORT *) BLKPTR(fhPanoramaFileNames);
		
		for ( i = 0 ; i < MAX_PANORAMAS; i++)
		{
			if (pPanoramaFileNames[i] != fERROR)
			{
				DisposBlock(pPanoramaFileNames[i]);
				pPanoramaFileNames[i] = fERROR;
			}
		}
		DisposBlock(fhPanoramaFileNames);
	}
	hSky_filenames = fhPanoramaFileNames = fERROR;
}
/* ========================================================================
   Function    - mfReleaseData
   Description - just mark the global variable not set.
   Returns     - 
   ======================================================================== */
inline void PANORAMA_DATA::mfReleaseData() {
	// Hang onto our memory, for use later.
	hSky_filenames = fERROR;
}

/* ========================================================================
   Function    - mfIsSet
   Description - 
   Returns     - 
   ======================================================================== */
#if defined(SCENE_EDITOR)
inline BOOL PANORAMA_DATA::mfIsSet() {
	return ( (hSky_filenames == fERROR) ?  FALSE : TRUE);
}

/* ========================================================================
   Function    - mfWriteData
   Description - Write Scene file data.
   Returns     - 
   ======================================================================== */
inline void PANORAMA_DATA::mfWriteData(FILE *fp) {
	SHORT *pPanoramaFileNames = (SHORT *)BLKPTR(fhPanoramaFileNames);
	
	for (int i = 0; i < MAX_PANORAMAS; i++)
	{
		if (pPanoramaFileNames[i] != fERROR)
		{
			char *cpFileName = (char *) BLKPTR(pPanoramaFileNames[i]);
			fprintf(fp, "%s\n", cpFileName);
		}
	}
}
#endif

#endif // _PANORAMA_HXX
