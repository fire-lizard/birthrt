/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: PLAYRACE.HXX - Player Race Information
   Author: 	 Gary Powell
   ======================================================================== */
#ifndef _PLAYRACE_HXX
#define _PLAYRACE_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
//#include "gamestr.h"
#if !defined(_STRENUM_H)
#include "strenum.h"   
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
// If you add a new race, you must update the data table in
// Armor.c and RAbility.c

class RACE_INFO {
public:
	typedef enum {
		NOT_SET = -1,
		DWARF = 0,
		ELF,
		HALF_ELF,
		HALFLING,
		HUMAN_ANUIREAN,
		HUMAN_BRECHT,
		HUMAN_KHINASI,
		HUMAN_RJURIK,
		HUMAN_VOS,
		GOBLINS,			// Used for Armor limitations
		GNOLLS,
	
		// Insert new races above this line!
		RACE_MAX_NUMBER
	} TYPE;

	static char const * const mfGetRaceName(const TYPE);
private:
	
	static GAME_STRING const fpRaceInfoTable[];
};

typedef RACE_INFO *PTR_RACE_INFO;

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

// Get the description name of the character's race.
inline char const * const RACE_INFO::mfGetRaceName(
	const RACE_INFO::TYPE PlayerRace)
{
	if (PlayerRace == NOT_SET)
	{
		return 0;
	}
	else
	{
		return STRMGR_GetStr(fpRaceInfoTable[PlayerRace]);
	}
}

#endif // _PLAYRACE_HXX
