/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aileader.hxx
   Author:   Greg Hightower
   ======================================================================== */
#if !defined(_AILEADER_H)
#define _AILEADER_H

#include <stdio.h>

#if !defined(_SYSTEM_H)
#include "system.h"
#endif

/* -----------------------------------------------------------------
   typedefs
   ----------------------------------------------------------------- */


class CAvatar;

/* -----------------------------------------------------------------
   class definition
   ----------------------------------------------------------------- */
class LEADER_DATA 
{
public:
	// Write to Scene File
	void mfWriteData(FILE *fp);
	
	// Read from Scene File
	LONG mfReadData(FILE * fp);
	
	void mfDelete();
	
	void mfInit(CAvatar *pAvatar);
	void mfRecordTrail(CAvatar *pAvatar);
	void mfMoveFollowers(CAvatar *pAvatar);
	
	// called before scene file is read
	void mfInitVals()
	{
		sMaxFollowers = 0;			// Max followers allowed
		hdlSlotList = fERROR;		// handle to the slot data
		hdlPathDeltas = fERROR;		// handle to path point data
		sHead = 0;					// next avaliable path point
		sCount = 0;					// current step count
	}
	
	SHORT		sMaxFollowers;
	SHORT		hdlSlotList;
	SHORT		hdlPathDeltas;
	SHORT		sHead;
	SHORT		sCount;
	LONG		DeltaX;
	LONG		DeltaY;
};

// Cleanup
inline void LEADER_DATA::mfDelete()
{
	if (hdlSlotList != fERROR && (SHORT)hdlSlotList != (SHORT)0xDCDC)
	{
		DisposBlock(hdlSlotList);
		hdlSlotList = fERROR;
	}
	
	if (hdlPathDeltas != fERROR && (SHORT)hdlPathDeltas != (SHORT)0xDCDC)
	{
		DisposBlock(hdlPathDeltas);
		hdlPathDeltas = fERROR;
	}
}
#endif
