/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aiguard.hxx
   Author:   Greg Hightower
   ======================================================================== */
#ifndef _AIGUARD_H
#define _AIGUARD_H

/* -----------------------------------------------------------------
   class definition
   ----------------------------------------------------------------- */

class GUARDPT_DATA
{
public:
	// Write to Scene File
	void mfWriteData(FILE *fp);
	
	// Read from Scene File
	LONG mfReadData(FILE * fp);
	
	LONG X;
	LONG Y;
	LONG StandAsideX;
	LONG StandAsideY;
	LONG GuardPositionAngle;
	SHORT Wave;

protected:
private:
};
	
/* -----------------------------------------------------------------
   inline member functions
   ----------------------------------------------------------------- */

// write member data to scene file on disk
inline void GUARDPT_DATA::mfWriteData(FILE *fp)
{
	fprintf(fp, "%ld %ld %ld %ld\n", X, Y, StandAsideX, StandAsideY);
}

// read member data from scene file off disk
inline LONG GUARDPT_DATA::mfReadData(FILE * fp)
{
	char cpBuffer[80];
	LONG Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	
	if (Result != EOF)
	{
		Result = ( (4 == sscanf(cpBuffer, "%ld %ld %ld %ld", 
		                                  &X, &Y, &StandAsideX, &StandAsideY) ) ? 
		                                  Result : EOF);
	}
	return Result;
}

#endif
