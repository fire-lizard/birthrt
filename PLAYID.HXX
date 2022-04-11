/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _PLAYID_HXX
#define _PLAYID_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */


//#include "gamestr.h"
#if !defined(_STRENUM_H)
#include "strenum.h"
#endif


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
   Prototypes
   ------------------------------------------------------------------------ */


// [d4-09-97, 4-23-97 JPC] Abolished PLAYER_NAME class.  For the character editor,
// we need to be able to change the names, so we no longer need the enums,
// the access functions, or the name strings in STRDAT.DAT.

class PLAYER_TITLE{
public:
	typedef enum{
		NOT_SET = 0,
		PATRIARCH,
		HIGH_HIEROPHANT,
		DUCHESS,
		BARONESS,
		PRINCE,
		OVERTHANE,
		GUILDER,
		WIZARD,
		COUNT,
		BARON,
		EMERALD_QUEEN,
		QUEEN,
		ARCHPRIEST,
		LORD,
		THANE,
	}TITLE;
	static inline char const * const mfGetTitle(const TITLE title)
	{
	 return(STRMGR_GetStr(player_title_table[title]));
	}
	static inline GAME_STRING mfGetTitleIndex(const TITLE title)
	{
		return(player_title_table[title]);
	}
	TITLE fTitle;
private:
	static GAME_STRING const player_title_table[];
};


class PLAYER_DESCRIPTION{
public:
	typedef enum{
		NOT_SET = 0,

	}DES;
	static inline char const * const mfGetDescription(const DES desp)
	{
	 return(STRMGR_GetStr(player_des_table[desp]));
	}
	static inline GAME_STRING mfGetDesIndex(const DES desp)
	{
		return(player_des_table[desp]);
	}
	DES fDescription;
private:
	static GAME_STRING const player_des_table[];
};


#endif //_PLAYID_HXX_
