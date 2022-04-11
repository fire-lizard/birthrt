/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _FILENAME_HXX
#define _FILENAME_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <string.h>

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_FILEUTIL_H)
#include "fileutil.h"
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

class FILENAME {
public:
	void mfInitVals ();
	char * mfGetFileName();
	void mfSetFileName(char * /* cpName */) ;
	LONG mfReadFileName(FILE *);
	void mfWriteFileName(FILE *);
	BOOL mfIsSet();
	
protected:
private:
	void *operator new (size_t stAllocateBlock);	// Can't use new/delete
	void operator delete (void *p);
	
	char	 fcpFileName[9];
};
	
/* ========================================================================
   Function    - mfInitVals
   Description - initialize the member data.
   Returns     - 
   ======================================================================== */
inline void FILENAME::mfInitVals () 
{
	fcpFileName[0] = 0;
}

/* ========================================================================
   Function    - mfGetFileName
   Description - 
   Returns     - 
   ======================================================================== */
inline char * FILENAME::mfGetFileName() 
{
	return fcpFileName;
}

/* ========================================================================
   Function    - mfSetFileName
   Description - 
   Returns     - 
   ======================================================================== */
inline void FILENAME::mfSetFileName(char *cpName) 
{
	strncpy(fcpFileName, cpName, sizeof(fcpFileName) - 1);
	fcpFileName[sizeof(fcpFileName) - 1] = 0;
}

/* ========================================================================
   Function    - mfIsSet
   Description - 
   Returns     - 
   ======================================================================== */
inline BOOL FILENAME::mfIsSet() 
{
	return ((fcpFileName[0] == 0) ? FALSE : TRUE);
}

/* ========================================================================
   Function    - mfReadFileName
   Description - Read the data from the scene file.
   Returns     - 
   ======================================================================== */
inline LONG FILENAME::mfReadFileName (FILE *fp) 
{
	const LONG Result = GetNextLine(fp, fcpFileName, sizeof (fcpFileName));
	if (Result == EOF)
	{
		fcpFileName[0] = 0;
	}
	return Result;
}
/* ========================================================================
   Function    - mfWriteFileName
   Description - Write the data back to the file.
   Returns     - 
   ======================================================================== */
inline void FILENAME::mfWriteFileName (FILE *fp) 
{
	fprintf(fp, "%s\n", fcpFileName);
}

#endif // _FILENAME_HXX
