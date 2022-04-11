/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aispectr.hxx
   Author:   Lan Zhou
   ======================================================================== */
#if !defined(_AISPECTR_HXX)
#define _AISPECTR_HXX

#if !defined(_AVATAR_HXX)
#include "avatar.hxx"
#endif

/* -----------------------------------------------------------------
   class definition
   ----------------------------------------------------------------- */

// This AI is to start out inside a MonsterBox.
class SPECTRE_DATA
{
public:
	void mfInitVals();
	
	// Write to Scene File
	void mfWriteData(FILE *fp) const; 
	
	// Read from Scene File
	const LONG mfReadData(FILE *fp);

	VECTOR_3D		Movement;
	
	LONG		HomeX;
	LONG		HomeY;
	LONG		HomeZ;
	LONG		HomeA;
	LONG        timer;
	
};
	
/* -----------------------------------------------------------------
   inline member functions
   ----------------------------------------------------------------- */
// Initialize the values to something reasonable.
// To be called if created on the fly by the scene file.
inline void SPECTRE_DATA::mfInitVals()
{
	Movement.dx = 0;
	Movement.dy = 0;
	Movement.dz = 0;
}

// write member data to scene file
inline void SPECTRE_DATA::mfWriteData(FILE *fp) const
{
	fprintf(fp, "%ld %ld\n", Movement.dx, Movement.dy);
}

// read member data from scene file
inline const LONG SPECTRE_DATA::mfReadData(FILE *fp)
{
	char cpBuffer[80];
	LONG Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	
	if (Result != EOF)
	{
		Result = ( (2 == sscanf(cpBuffer, "%ld %ld", &Movement.dx, &Movement.dy) ) ? Result : EOF);
	}
	return Result;
}

#endif
