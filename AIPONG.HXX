/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aipong.hxx
   Author:   Greg Hightower & Gary Powell
   ======================================================================== */
#if !defined (_AIPONG_HXX)
#define _AIPONG_HXX 1

#if !defined(_AVATAR_HXX)
#include "avatar.hxx"
#endif

#if !defined(_FILEUTIL_H)
#include "fileutil.h"
#endif

#if !defined(_ITEMTYPE_HXX)
#include "itemtype.hxx"
#endif

/* -----------------------------------------------------------------
   class definition
   ----------------------------------------------------------------- */

// This AI is to start out inside a MonsterBox.
class PINGPONG_DATA
{
public:
	void mfInitVals();
	
	// Write to Scene File
	void mfWriteData(FILE *fp) const; 
	
	// Read from Scene File
	const LONG mfReadData(FILE *fp);
	
	static void mfSwitchToPingPong(LONG);
	VECTOR		Movement;
	
	LONG		HomeX;
	LONG		HomeY;
	LONG		HomeZ;
	LONG		HomeA;
	ITEMTYPE	fCurrentSpell;
};
	
/* -----------------------------------------------------------------
   inline member functions
   ----------------------------------------------------------------- */
// Initialize the values to something reasonable.
// To be called if created on the fly by the scene file.
inline void PINGPONG_DATA::mfInitVals()
{
	Movement.dx = 0;
	Movement.dy = 0;
}

// write member data to scene file
inline void PINGPONG_DATA::mfWriteData(FILE *fp) const
{
	fprintf(fp, "%ld %ld\n", Movement.dx, Movement.dy);
}

// read member data from scene file
inline const LONG PINGPONG_DATA::mfReadData(FILE *fp)
{
	char cpBuffer[80];
	LONG Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	
	if (Result != EOF)
	{
		Result = ( (2 == sscanf(cpBuffer, "%ld %ld", &Movement.dx, &Movement.dy) ) ? Result : EOF);
	}
	return Result;
}

#endif // AI_PONG_HXX
