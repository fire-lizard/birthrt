/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aifolid.hxx
   Author:   Greg Hightower
   ======================================================================== */
#ifndef _AIFOLID_H
#define _AIFOLID_H

#if !defined(_FILEUTIL_H)
#include "fileutil.h"
#endif

/* -----------------------------------------------------------------
   class definition
   ----------------------------------------------------------------- */

class FOLLOW_ID_DATA
{
public:
	// Write to Scene File
	void mfWriteData(FILE *fp);
	
	// Read from Scene File
	LONG mfReadData(FILE *fp);
	
	LONG		Id;			// who I follow
	LONG		Rate;		// My speed to keep up with him
	LONG		NewX;		// where I try to move to
	LONG		NewY;
	LONG		NewA;
};
	
/* -----------------------------------------------------------------
   inline member functions
   ----------------------------------------------------------------- */

inline void FOLLOW_ID_DATA::mfWriteData(FILE *fp)
{
	fprintf(fp, "%ld\n", Id);
}

inline LONG FOLLOW_ID_DATA::mfReadData(FILE *fp)
{
	char cpBuffer[80];
	
	LONG Result = GetNextLine(fp, cpBuffer, sizeof (cpBuffer));
	if (Result != EOF)
	{
		Result = ( (1 ==sscanf(cpBuffer, "%ld", &Id) ? Result : EOF));
	}
	return Result;
}

#endif
