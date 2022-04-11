/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aipthldr.hxx
   Author:   Greg Hightower
   ======================================================================== */
#ifndef _AIPTHLDR_H
#define _AIPTHLDR_H

/* -----------------------------------------------------------------
   class definition
   ----------------------------------------------------------------- */

class PATH_LEADER_DATA
{
public:
	// Write to Scene File
	void mfWriteData(FILE *fp);
	
	// Read from Scene File
	LONG mfReadData(FILE * fp);
	
	FOLLOW_PATH_DATA	FollowPath;
	LEADER_DATA			Leader;
};
	
/* -----------------------------------------------------------------
   inline member functions
   ----------------------------------------------------------------- */

inline void PATH_LEADER_DATA::mfWriteData(FILE *fp)
{
	FollowPath.mfWriteData(fp);
	Leader.mfWriteData(fp);
}

inline LONG PATH_LEADER_DATA::mfReadData(FILE * fp)
{
	LONG retVal = 1;
	if (EOF == FollowPath.mfReadData(fp))
	{ /* do something here on error */ }
	if (EOF == Leader.mfReadData(fp))
	{ /* do something here on error */ }
	return retVal;
}

#endif
