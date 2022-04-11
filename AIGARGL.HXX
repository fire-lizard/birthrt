/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aigargl.hxx
   Author:   Lan Zhou
   ======================================================================== */
#if !defined(_AIGARGL_HXX)
#define _AIGARGL_HXX

#if !defined(_AVATAR_HXX)
#include "avatar.hxx"
#endif

/* -----------------------------------------------------------------
   class definition
   ----------------------------------------------------------------- */

// This AI is to start out inside a MonsterBox.
class GARGOYLE_DATA
{
public:
	void mfInitVals();
	
	// Write to Scene File
	void mfWriteData(FILE *fp) const; 
	
	// Read from Scene File
	const LONG mfReadData(FILE *fp);
	
	const LONG mfFallRate() const
	{
		return 32;	   
	}	

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
inline void GARGOYLE_DATA::mfInitVals()
{
	Movement.dx = 0;
	Movement.dy = 0;
	Movement.dz = 0;
}

// write member data to scene file
inline void GARGOYLE_DATA::mfWriteData(FILE *fp) const
{
	fprintf(fp, "%ld %ld\n", Movement.dx, Movement.dy);
}

// read member data from scene file
inline const LONG GARGOYLE_DATA::mfReadData(FILE *fp)
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