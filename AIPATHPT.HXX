/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aifolpth.hxx
   Author:   Greg Hightower
   ======================================================================== */
#ifndef _AIPATHPT_H
#define _AIPATHPT_H

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_FILEUTIL_H)
#include "fileutil.h"
#endif

/* -----------------------------------------------------------------
   class definition
   ----------------------------------------------------------------- */

class PATH_PT
{
public:
	// Write to Scene File
	void mfWriteData(FILE * fp);
	
	// Read from Scene File
	LONG mfReadData(FILE *fp);
	
	LONG	X;
	LONG	Y;
	LONG	Z;
	LONG	A;
};
	
/* -----------------------------------------------------------------
   inline member functions
   ----------------------------------------------------------------- */

inline void PATH_PT::mfWriteData(FILE * fp)
{
	fprintf(fp, "%ld %ld %ld %ld\n", X, Y, Z, A);
}

inline LONG PATH_PT::mfReadData(FILE *fp)
{
	char cpBuffer[80];
	LONG Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	
	if (Result != EOF)
	{
		Result = (4 == sscanf(cpBuffer, "%ld %ld %ld %ld", &X, &Y, &Z, &A)) ? Result : EOF;
	}
	return Result;
}
	
/* -----------------------------------------------------------------
   typedefs
   ----------------------------------------------------------------- */

typedef SHORT HDL_PATH_PTS;
typedef PATH_PT *PTR_PATH_PTS;

#endif	
