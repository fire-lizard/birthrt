/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aifndtyp.hxx
   Author:   Greg Hightower
   ======================================================================== */
#ifndef _AIFNDTYP_H
#define _AIFNDTYP_H

/* -----------------------------------------------------------------
   additional required include files
   ----------------------------------------------------------------- */

#if !defined(_THINGTYP_H)
#include "thingtyp.h"
#endif

/* -----------------------------------------------------------------
   class definition
   ----------------------------------------------------------------- */

class FIND_TYPE_DATA
{
public:
	// Write to Scene File
	void mfWriteData(FILE *fp);
	
	// Read from Scene File
	LONG mfReadData(FILE *fp);
	
	// init member data
	void mfInitVals(THINGTYPE Target);
	
	THINGTYPE		TargetType;
	LONG			Id;			// The nearest Id of type TargetType.
	LONG			Control;	// State of engagement.
};
	
/* -----------------------------------------------------------------
   inline member functions
   ----------------------------------------------------------------- */

// write member data to scene file on disk
inline void FIND_TYPE_DATA::mfWriteData(FILE *fp)
{
	fprintf(fp, "%ld\n", (LONG) TargetType);
}

// read member data from scene file off disk
inline LONG FIND_TYPE_DATA::mfReadData(FILE *fp)
{
	char cpBuffer[80];
	LONG Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	
	if (Result != EOF)
	{
		Result = ( (1 == sscanf(cpBuffer, "%ld", &TargetType) ) ? Result : EOF);
	}
	return Result;
}
	
inline void FIND_TYPE_DATA::mfInitVals(THINGTYPE Target)
{
	TargetType = Target;
	Id = MAX_AVATARS;
	Control = 0;
}

#endif

