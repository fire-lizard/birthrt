/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _ACTIONS_HXX
#define _ACTIONS_HXX

#if !defined(_TYPEDEFS_HXX)
#include "typedefs.h"
#endif

#if !defined(_VECTOR_HXX)
#include "vector.hxx"
#endif

#if !defined(_STRMGR_H)
#include "strmgr.h"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */

class ACTIONS {
public:
	inline ACTIONS(LONG /* NewTitle */, LONG /* NewVerbose */, LONG /* NewMoreInfo */);
	inline char * mfGetTitle() const;
	inline char * mfGetVerbose() const;
	inline LONG   const mfGetMoreInfo() const;
protected:
private:
	LONG		const title;
	LONG		const verbose;
	LONG		const more_info;
};
typedef ACTIONS *ACTIONS_PTR;
extern DECL_VECTOR_CLASS(ACTIONS const,Action);

inline ACTIONS::ACTIONS(
	LONG NewTitle,
    LONG NewVerbose,
    LONG NewMoreInfo) :
  title(NewTitle),
  verbose(NewVerbose),
  more_info(NewMoreInfo)
{
}

inline char * ACTIONS::mfGetTitle() const
{
	return STRMGR_GetStr(title);
}

inline char *   ACTIONS::mfGetVerbose() const
{
	return STRMGR_GetStr(verbose);
}

inline LONG   const ACTIONS::mfGetMoreInfo() const
{
	return more_info;
}

#endif // _ACTIONS_HXX

/*	======================================================================== */


