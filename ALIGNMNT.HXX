
/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _ALIGNMNT_HXX
#define _ALIGNMNT_HXX

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
class ALIGNMENT_INFO {
public:
	typedef enum {
		NON_ALIGNED = 0,
		LAWFUL_GOOD,
		LAWFUL_NEUTRAL,
		LAWFUL_EVIL,
		NEUTRAL_GOOD,
		TRUE_NEUTRAL,
		NEUTRAL_EVIL,
		CHAOTIC_GOOD,
		CHAOTIC_NEUTRAL,
		CHAOTIC_EVIL,
	
		// These are the player alignments,...Don't add more!
		MAX_ALIGNMENTS
	} TYPE;

	static char const * const mfGetName(const TYPE);
	static BOOL const mfIsGood(const TYPE);
	static BOOL const mfIsNeutral(const TYPE);
	static BOOL const mfIsEvil(const TYPE);

private:
	static  GAME_STRING const fpAlignmentInfoTable[];
};

typedef ALIGNMENT_INFO *PTR_ALIGNMENT_INFO;

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Globals
   ------------------------------------------------------------------------ */

inline char const * const ALIGNMENT_INFO::mfGetName(
	const ALIGNMENT_INFO::TYPE type)
{
	return STRMGR_GetStr(fpAlignmentInfoTable[type]);
}

inline BOOL const ALIGNMENT_INFO::mfIsGood( const ALIGNMENT_INFO::TYPE type)
{
	if (type == ALIGNMENT_INFO::LAWFUL_GOOD ||
		type == ALIGNMENT_INFO::NEUTRAL_GOOD ||
		type == ALIGNMENT_INFO::CHAOTIC_GOOD)
		return TRUE;
	
	return FALSE;
}
inline BOOL const ALIGNMENT_INFO::mfIsNeutral( const ALIGNMENT_INFO::TYPE type)
{
	if (type == ALIGNMENT_INFO::LAWFUL_NEUTRAL ||
		type == ALIGNMENT_INFO::TRUE_NEUTRAL ||
		type == ALIGNMENT_INFO::CHAOTIC_NEUTRAL)
		return TRUE;
	
	return FALSE;
}
inline BOOL const ALIGNMENT_INFO::mfIsEvil( const ALIGNMENT_INFO::TYPE type)
{
	if (type == ALIGNMENT_INFO::LAWFUL_EVIL ||
		type == ALIGNMENT_INFO::NEUTRAL_EVIL ||
		type == ALIGNMENT_INFO::CHAOTIC_EVIL)
		return TRUE;
	
	return FALSE;
}

#endif // _ALIGNMNT_HXX
