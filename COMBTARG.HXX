/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _COMBTARG_HXX
#define _COMBTARG_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYEPDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_SYSTEM_H)
#include "system.h"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
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
// class to manage data necesary for targeting spells.

class COMBAT_TARGETTING {
public:
	typedef enum {
			NOT_SET,
			TARGETTING
	} MODE;
	
	inline static void mfInitVals();
	inline static BOOL const mfIsModeTargetting();
	static void mfSetupTargetting(SHORT const /* hAvatar */);
	inline static LONG mfGetTargetIndex();
	inline static SHORT const mfWhoseShooting();
	static void mfStopTargeting();
	inline static void mfSetModeTargetting();

protected:
private:
	static void TargetClick(LONG /* but */ ,
							LONG /* obj */ ,
							LONG /* WadThingtype */);
	
	static void TargetStop(LONG /* but */ );
	
	
	// stores prev mouse callback during targetting
	static PFVLLL	fPrevObjMouseCallback;
	static PFVLLL	fPrevFloorMouseCallback;
	static PFVLLL	fPrevWallMouseCallback;
	static PFVL		fPrevRightMouseCallback;
	
	// holds the resulting target object
	static LONG		fTargetIndex;
	static SHORT	fhAvatar;		// Whose shooting.
	
	static MODE		fMode;
	static BOOL		fRegionsPushed;
};


/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Inline functions   
   ------------------------------------------------------------------------ */
   
inline void COMBAT_TARGETTING::mfInitVals()
{
	fMode = NOT_SET;
	
	fPrevObjMouseCallback = 0;
	fPrevFloorMouseCallback = 0;
	fPrevWallMouseCallback = 0;
	fPrevRightMouseCallback = 0;
	
	fTargetIndex = fERROR;
	fhAvatar = fERROR;
	fRegionsPushed = FALSE;
}

// GWP Hack so that when we start up combat we can keep the camera in place.
inline void COMBAT_TARGETTING::mfSetModeTargetting()
{
	fMode = TARGETTING;
}

inline BOOL const COMBAT_TARGETTING::mfIsModeTargetting()
{
	return (( fMode == TARGETTING)? TRUE : FALSE); 
}

inline LONG COMBAT_TARGETTING::mfGetTargetIndex()
{
	return fTargetIndex;
}

inline SHORT const COMBAT_TARGETTING::mfWhoseShooting()
{
	return fhAvatar;
}
#endif // _COMBTARG_HXX
