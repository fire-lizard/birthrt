/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: AiHHound.hxx
   Author:   Greg Hightower & Gary Powell & Wes Cumberland
   ======================================================================== */
#ifndef _AIPONG_H
#define _AIPONG_H

#if !defined(_ATHING_HXX)
#include "ATHING.HXX"
#endif

#if !defined(_AVATAR_HXX)
#include "AVATAR.HXX"
#endif

typedef SHORT HDL_AVATAR;
/* -----------------------------------------------------------------
   class definition
   ----------------------------------------------------------------- */

// This AI is to start out inside a MonsterBox.
class HELLHOUND_DATA
{
public:
	void mfInitVals();
	
	// Write to Scene File
	void mfWriteData(FILE *fp) const; 
	
	// Read from Scene File
	const LONG mfReadData(FILE *fp);
	
	VECTOR		Movement;
	
	LONG		HomeX;
	LONG		HomeY;
	LONG		HomeZ;
	LONG		HomeA;
};
	
/* -----------------------------------------------------------------
   inline member functions
   ----------------------------------------------------------------- */
// Initialize the values to something reasonable.
// To be called if created on the fly by the scene file.
inline void HELLHOUND_DATA::mfInitVals()
{
	Movement.dx = 0;
	Movement.dy = 0;
}

// write member data to scene file
inline void HELLHOUND_DATA::mfWriteData(FILE *fp) const
{
	fprintf(fp, "%d %d\n", Movement.dx, Movement.dy);
}

// read member data from scene file
inline const LONG HELLHOUND_DATA::mfReadData(FILE *fp)
{
	char cpBuffer[80];
	LONG Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	
	if (Result != EOF)
	{
		Result = ( (2 == sscanf(cpBuffer, "%d %d", &Movement.dx, &Movement.dy) ) ? Result : EOF);
	}
	return Result;
}

#endif
