/* ========================================================================
   Copyright (c) 1990,1997	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _SCNDLGS_HXX
#define _SCNDLGS_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_FILEUTIL_H)
#include "fileutil.h"
#endif

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
typedef SHORT HDL_DLG_TRIPLINES;

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Class Definition
   ------------------------------------------------------------------------ */
class DIALOG_TRIPLINES
{
public:
	inline void mfInitVals();
	inline LONG	const mfReadData(FILE *);
	inline LONG const mfGetStringId() const;
	inline BOOL const mfMatchTripLineNumber(LONG const /* TripLine */ ) const;
	inline BOOL const mfTripLineSeen() const;
	inline void mfSetTripLineSeen();

protected:
private:
	LONG	fTripLineNumber;
	LONG	fStringNumber;
	BOOL	fTripLineTripped;
};

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Inline functions   
   ------------------------------------------------------------------------ */
inline void DIALOG_TRIPLINES::mfInitVals()
{
	fTripLineNumber = 0;
	fStringNumber = STR_NULL;
	fTripLineTripped = FALSE;
}

// retrieve the string id.
inline LONG const DIALOG_TRIPLINES::mfGetStringId() const
{
	return fStringNumber;
}

// Read the dialog data in from the scene files.
inline LONG const DIALOG_TRIPLINES::mfReadData(FILE *fp)
{
	char cpBuffer[128];
	LONG const Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	
	if (Result != EOF)
	{
		if ( 2 == sscanf(cpBuffer, "%ld %ld", &fTripLineNumber, &fStringNumber))
		{
			fStringNumber += STR_DIALOG_BASE_NUMBER;
		}
		else
		{
			mfInitVals();
		}
	}
	
	return Result;
}

// Test whether the trip line matches this one.
inline BOOL const DIALOG_TRIPLINES::mfMatchTripLineNumber(LONG const TripLine ) const
{
	return TripLine == fTripLineNumber;
}

// Test whether the trip line has been seen or not.
inline BOOL const DIALOG_TRIPLINES::mfTripLineSeen() const
{
	return (fTripLineTripped == TRUE);
}

// Set the tripline seen flag.
inline void DIALOG_TRIPLINES::mfSetTripLineSeen()
{
	fTripLineTripped = TRUE;
}
#endif // _SCNDLGS_HXX
