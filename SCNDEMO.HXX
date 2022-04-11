/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _SCNDEMO_HXX
#define _SCNDEMO_HXX

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
class SCENE_DEMO_DATA {
public:
	void mfInitVals();
	char * mfGetCommandFileName ();
	LONG mfReadData (FILE *fp);
	
	void mfWriteData (FILE *fp);
	BOOL mfIsSet();

protected:
private:
    SCENE_DEMO_DATA();
    ~SCENE_DEMO_DATA();
    // You can't use new or delete with this memory manager.
    void *operator new( size_t stAllocateBlock);
    void operator delete( void *p );
    SCENE_DEMO_DATA(const SCENE_DEMO_DATA &);                   // No copies
    const SCENE_DEMO_DATA operator=(const SCENE_DEMO_DATA &);   // No assignments.
    
	FILENAME	fCommandFile;
};
	

    
/* ========================================================================
   Function    - mfInitVals
   Description - initialize the member data.
   Returns     - 
   ======================================================================== */
inline void SCENE_DEMO_DATA::mfInitVals() 
{
	fCommandFile.mfInitVals();
}

    
/* ========================================================================
   Function    - mfGetCommandFileName
   Description - Retrieve the comand file name.
   Returns     - 
   ======================================================================== */
inline char * SCENE_DEMO_DATA::mfGetCommandFileName ()
{
	return (fCommandFile.mfGetFileName());
}

    
/* ========================================================================
   Function    - mfReadData
   Description - Read the data from the scene file.
   Returns     - 
   ======================================================================== */
inline LONG SCENE_DEMO_DATA::mfReadData (FILE *fp) 
{
	return fCommandFile.mfReadFileName(fp);
}

    
/* ========================================================================
   Function    - mfWriteData
   Description - Write the data back to the scene file.
   Returns     - 
   ======================================================================== */
inline void SCENE_DEMO_DATA::mfWriteData (FILE *fp) 
{
	fCommandFile.mfWriteFileName(fp);
}

    
/* ========================================================================
   Function    - mfIsSet
   Description - Test for whether this is a demo scene or not.
   Returns     - 
   ======================================================================== */
inline BOOL SCENE_DEMO_DATA::mfIsSet() 
{
	return ( fCommandFile.mfIsSet());
}

#endif // _SCNDEMO_HXX
