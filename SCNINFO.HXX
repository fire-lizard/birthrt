/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _SCNINFO_HXX
#define _SCNINFO_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <string.h>

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

class SCENE_INFO {
public:
	void mfInitVals ();
	char * mfGetSceneName();
	void mfSetSceneName(char * /* cpName */) ;
	void mfWriteSceneName(FILE *);

	BOOL mfIsSet();
protected:
private:
	void *operator new (size_t stAllocateBlock);	// Can't use new/delete
	void operator delete (void *p);
	
	FILENAME	 fcpSceneName;
};
	
/* ========================================================================
   Function    - mfInitVals
   Description - initialize the member data.
   Returns     - 
   ======================================================================== */
inline void SCENE_INFO::mfInitVals () 
{
	fcpSceneName.mfInitVals();
}

/* ========================================================================
   Function    - mfGetSceneName
   Description - 
   Returns     - 
   ======================================================================== */
inline char * SCENE_INFO::mfGetSceneName() 
{
	return fcpSceneName.mfGetFileName();
}

/* ========================================================================
   Function    - mfSetSceneName
   Description - 
   Returns     - 
   ======================================================================== */
inline void SCENE_INFO::mfSetSceneName(char *cpName) 
{
	fcpSceneName.mfSetFileName(cpName);
}

/* ========================================================================
   Function    - mfIsSet
   Description - 
   Returns     - 
   ======================================================================== */
inline BOOL SCENE_INFO::mfIsSet() 
{
	return fcpSceneName.mfIsSet();
}
/* ========================================================================
   Function    - mfWriteSceneName
   Description - 
   Returns     - 
   ======================================================================== */
inline void SCENE_INFO::mfWriteSceneName(FILE *fp) 
{
	fcpSceneName.mfWriteFileName(fp);
}


#endif // _SCNINFO_HXX
