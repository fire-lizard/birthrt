/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aifirebl.hxx
   Author:   Lan Zhou
   ======================================================================== */
#if !defined(_AISTUN_HXX)
#define _AISTUN_HXX

#if !defined(_AVATAR_HXX)
#include "avatar.hxx"
#endif

/* -----------------------------------------------------------------
   class definition
   ----------------------------------------------------------------- */


class STUNNED_DATA
{
public:
	void mfInitVals();

	static void mfSwitchToStunned(LONG hAvatar);
	
	// Write to Scene File
	void mfWriteData(FILE *fp) const; 
	
	// Read from Scene File
	const LONG mfReadData(FILE *fp);
	
	VECTOR_3D		Movement;
	POINT_3D  		Home;
	POINT_3D        Target;
	SHORT			hWhoCast;	// To hand back experience pts.
};
	
/* -----------------------------------------------------------------
   inline member functions
   ----------------------------------------------------------------- */
// Initialize the values to something reasonable.
// To be called if created on the fly by the scene file.
inline void STUNNED_DATA::mfInitVals()
{
	Movement.dx = 0;
	Movement.dy = 0;
	Movement.dz = 0;
}

// write member data to scene file
inline void STUNNED_DATA::mfWriteData(FILE *fp) const
{
	fprintf(fp, "%ld %ld\n", Movement.dx, Movement.dy);
}

// read member data from scene file
inline const LONG STUNNED_DATA::mfReadData(FILE *fp)
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
