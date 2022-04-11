/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aifolpth.hxx
   Author:   Greg Hightower
   ======================================================================== */
#ifndef _AIFOLPTH_H
#define _AIFOLPTH_H

/* -----------------------------------------------------------------
   additional includes
   ----------------------------------------------------------------- */


#include <stdio.h>

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_SYSTEM_H)
#include "system.h"
#endif

#if !defined(_AIPATHPT_HXX)
#include "aipathpt.hxx"
#endif

/* -----------------------------------------------------------------
   class definition
   ----------------------------------------------------------------- */

class FOLLOW_PATH_DATA
{
public:
	// Write to Scene File
	void mfWriteData(FILE * fp);
	
	// Read from Scene File
	LONG mfReadData(FILE *fp);
	
	// init member data
	void mfInitVals();
	
	// delete the path infomation
	void mfDelete ();
	
	// get next path point from path array
	BOOL mfGetNextPt(LONG &X, LONG &Y, LONG &Z, LONG &A);
	
	// what is the current point I am looking for
	void mfGetCurrentPt(LONG &X, LONG &Y, LONG &Z, LONG &A);
	
	SHORT			PathCount;		// number of path points. (Each path pt is 4 LONGS)
	SHORT  			PathIndex;		// where in the current path you are.
	HDL_PATH_PTS	hPath;			// Handle to path data.
	SHORT			Wave;
	POINT			oldP;
};

/* -----------------------------------------------------------------
   inline member functions
   ----------------------------------------------------------------- */

inline void FOLLOW_PATH_DATA::mfInitVals()
{
	PathIndex = 0;
	Wave = 0;
	oldP.x = 0;
	oldP.y = 0;
}

inline void FOLLOW_PATH_DATA::mfDelete ()
{
	if (hPath != fERROR)
	{
		DisposBlock(hPath);
		hPath = fERROR;
		PathCount = 0;
	}
}

inline void FOLLOW_PATH_DATA::mfGetCurrentPt(LONG &X, LONG &Y, LONG &Z, LONG &A)
{
	if (hPath != fERROR)
	{
		PTR_PATH_PTS pPathPts = (PTR_PATH_PTS) BLKPTR(hPath);
		X = pPathPts[PathIndex].X;
		Y = pPathPts[PathIndex].Y;
		Z = pPathPts[PathIndex].Z;
		A = pPathPts[PathIndex].A;
	}
}

#endif
