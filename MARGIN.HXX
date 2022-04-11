/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _MARGIN_HXX
#define _MARGIN_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define MARGIN_INCREMENT	20
#define MAX_TIMES_REDUCTION	5

/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Class Definition
   ------------------------------------------------------------------------ */
/*
   --------------------------------------------------------------------------
   	MARGIN_CONTROL is a class to help manage the screen margins for battle 
   	               and adventure/combat AI. It will remember the current state
   	               of the screen and if you call RestoreCurrentSize it will
   	               reset the screen to that previous state.
   --------------------------------------------------------------------------
*/

// This class exists to help the combat and battle menus have a similar look
// and feel for screen size incrementing/decrementing. Also when there is
// background art it will make sure that it is painted. (TBD)

class MARGIN_CONTROL
{
public:
	// This helper class exists, because we may switch from an absolute
	// increase or decrease number to a percentage, and therfore have to
	// retain the previous margins. (To avoid roundoff errors.)
	static void mfIncreaseMargin();
	static void mfDecreaseMargin();
	static BOOL const mfReduceScreenSize(LONG, LONG);
	static BOOL const mfIncreaseScreenSize(LONG, LONG);
	static void const mfClearBorder(void);
	static void mfRestoreCurrentSize();
	static void mfResetCurrentSize();
	static BOOL const mfIsScreenReduced()
	{
		return ((fReductionCount > 0) ? TRUE : FALSE);
	}
	static LONG const mfGetReductionCount()
	{
	    return fReductionCount;
	}
	static void mfSetReductionCount(LONG rc)
	{
		if (rc < 0 || rc > MAX_TIMES_REDUCTION)
			return;
		fReductionCount = rc;
	}
	
private:
	// No margin controls
	MARGIN_CONTROL();
	~MARGIN_CONTROL();
	// No copies or assignments.
	MARGIN_CONTROL(MARGIN_CONTROL const &);
	MARGIN_CONTROL const operator=(MARGIN_CONTROL const &);
	
	static LONG			fReductionCount;
};

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Inline functions   
   ------------------------------------------------------------------------ */
   

#endif // _MARGIN_HXX
