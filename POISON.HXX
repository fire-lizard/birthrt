/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _POISON_HXX
#define _POISON_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>

#if !defined(_TYEPDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_FILEUTIL_H)
#include "fileutil.h"
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
class AVATAR_POISON
{
public:
	typedef enum {
		NOT_POISON = 0,
		SLOW_ACTING,
		FAST_ACTING,
		INSTANT_BLINDNESS,
		INSTANT_PARALYSIS,
		
	} TYPE;
	
	inline LONG const mfReadTextData(FILE *);
	inline LONG const mfWriteTextData(FILE *);

	inline TYPE const mfGetType() const;
	inline void mfSetType(TYPE const);
private:
	TYPE	fType;
	SBYTE 	fsbCountTillNextDose;
};
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Inline functions   
   ------------------------------------------------------------------------ */
   
inline const LONG AVATAR_POISON::mfReadTextData(FILE *fp)
{
	char cpBuffer[120];
	const LONG Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	
	if (Result != EOF)
	{
		LONG lTemp1;
		LONG lTemp2;
		
		if( 2 == sscanf(cpBuffer,"%ld %ld", &lTemp1, &lTemp2))
		{
			fType = (TYPE) lTemp1;
			fsbCountTillNextDose = (SBYTE) lTemp2;
		}
	}
	
	return Result;
}

inline const LONG AVATAR_POISON::mfWriteTextData(FILE *fp)
{
	fprintf(fp, "%ld %ld\n", (LONG) fType, (LONG) fsbCountTillNextDose);
}

inline TYPE const AVATAR_POISON::mfGetType() const
{
	return fType;
}

inline void AVATAR_POISON::mfSetType(AVATAR_POISON::TYPE const newType)
{
	fType = newType;
}

#endif // _POISON_HXX
