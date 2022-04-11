/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aipthatt.hxx
   Author:   Greg Hightower
   ======================================================================== */
#ifndef _AIPTHATT_H
#define _AIPTHATT_H

/* -----------------------------------------------------------------
   class definition
   ----------------------------------------------------------------- */

class PATH_ATTACK_DATA
{
public:
	// Write to Scene File
	void mfWriteData(FILE *fp);
	
	// Read from Scene File
	LONG mfReadData(FILE * fp);
	
	FOLLOW_PATH_DATA	FollowPath;
	LEADER_DATA			Leader;
	LONG				OffSet;
	LONG				TrailRate;
	
	void mfCreateFollowers(CAvatar *pAvatar);
};

/* -----------------------------------------------------------------
   inline member functions
   ----------------------------------------------------------------- */

inline void PATH_ATTACK_DATA::mfWriteData(FILE *fp)
{
	FollowPath.mfWriteData(fp);
	fprintf(fp, "%ld\n", OffSet);
	fprintf(fp, "%ld\n", TrailRate);
}

inline LONG PATH_ATTACK_DATA::mfReadData(FILE * fp)
{
	LONG retVal = 1;
	char cpBuffer[80];
	if (EOF == FollowPath.mfReadData(fp))
	{ }
	LONG Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	sscanf(cpBuffer, "%ld", &OffSet);
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	sscanf(cpBuffer, "%ld", &TrailRate);
	return retVal;
}

#endif
