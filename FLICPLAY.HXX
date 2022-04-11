/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: flicplay.hxx
   Author:
   ========================================================================*/

#ifndef _FLICPLAY_HXX
#define _FLICPLAY_HXX

#if !defined(_THINGS_H)
#include "things.h"
#endif

BOOL PlayFlic(char * fn, LONG secondsToWaitAtEnd);


#define MAX_FLC_FILENAME_LEN 128
#define FLCERR_OK		 0
#define FLCERR_FILE		-1
#define FLCERR_MEM		-2
#define FLCERR_RSRC		-3
#define FLCERR_BITM		-4

// Look!  A class to manually handle a flick!

class SimpleFLC
{
public:
	inline SimpleFLC();
	inline ~SimpleFLC();
	SHORT mfInit(char *, SHORT x=0, SHORT y=0); // x and y are optional
	void  mfDispose();
	
	inline SHORT const mfTotalFrames() const;
	inline SHORT const mfGetCurFrame() const;
	void  mfSetCurFrame(SHORT frameno);
	SHORT mfNextFrame();
	void  mfDrawFrame(SHORT x=-1, SHORT y=-1);  // default: use above (x,y)
	inline void  mfMove(SHORT const delta_x, SHORT const delta_y);

protected:
private:
	char  filename[MAX_FLC_FILENAME_LEN];
	SHORT iAnim;
	SHORT iBuf;
	SHORT iHead;
	SHORT frame_in_buffer;
	SHORT coord_x, coord_y;
};

inline SimpleFLC::SimpleFLC() :		// The constructor sets initial (invalid) values
	iAnim(fERROR),
	iBuf(fERROR),
	iHead(fERROR),
	frame_in_buffer(-1),
	coord_x(0),
	coord_y(0)
{
	filename[0] = '\0';
}

inline SimpleFLC::~SimpleFLC()
{
	// Can't call this because the memory manager is already gone.
	// mfDispose();
}

inline SHORT const SimpleFLC::mfTotalFrames() const
{
	if (iAnim == fERROR)
		return fERROR;
		
	ANIMPTR pAnim = (ANIMPTR)BLKPTR(iAnim);
	return pAnim->totalFrames;
}

inline SHORT const SimpleFLC::mfGetCurFrame() const
{
	if (iAnim == fERROR)
		return fERROR;
		
	ANIMPTR pAnim = (ANIMPTR)BLKPTR(iAnim);
	return pAnim->frame;
}

inline void SimpleFLC::mfMove(SHORT const delta_x, SHORT const delta_y)
{
	coord_x += delta_x;
	coord_y += delta_y;
}

#endif

