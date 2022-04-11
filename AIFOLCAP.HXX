/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aifolcap.hxx
   Author:   Greg Hightower
   ======================================================================== */
#ifndef _AIFOLCAP_H
#define _AIFOLCAP_H

/* -----------------------------------------------------------------
   class definition
   ----------------------------------------------------------------- */

class FOLLOW_BTLCAP_DATA
{
public:
	// what to do if clicked on
	static void	mfLeftButton(CAvatar *);
	static void mfRightButton(CAvatar *);
	static SHORT FOLLOW_BTLCAP_DATA::FindUnengaged(CAvatar *, SHORT, THINGTYPE);
	
	SHORT		hLeader;	// Who I follow
	LONG		Rate;		// My speed to keep up with him
	LONG		NewX;		// where I try to move to
	LONG		NewY;
	LONG		NewA;
	LONG		FrameCount;	// How long have I been doing something
	SHORT		Shoot;		// shot type
};
	
#endif
