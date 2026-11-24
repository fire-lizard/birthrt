/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aileader.hxx
   Author:   Greg Hightower
   ======================================================================== */
#if !defined(_AILEADER_H)
#define _AILEADER_H

#if defined(SCENE_EDITOR)
#include <stdio.h>
#endif

#if !defined(_SYSTEM_H)
#include "SYSTEM.H"
#endif

/* -----------------------------------------------------------------
   class definition
   ----------------------------------------------------------------- */
class LEADER_DATA 
{
public:
#if defined(SCENE_EDITOR)
	// Write to Scene File
	// (SCENE EDITOR: defined in AVATAR.CPP; the original AILEADER.CPP is gone.)
	void mfWriteData(FILE *fp);
#endif
	void mfDelete();

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
