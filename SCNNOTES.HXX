/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _SCNNOTES_HXX
#define _SCNNOTES_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
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
/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

typedef SHORT HDL_SCN_NOTE;

class SCN_NOTE {
public:
	void mfSetNoteData(SBYTE const /* NoteNumber */,
	                   GAME_STRING const  /* StringMgrIndex */);
	GAME_STRING const mfGetStrId() const;
	SBYTE const mfGetNoteNumber() const;
	
	void mfInitVals();
	
protected:
private:
	SCN_NOTE();
	~SCN_NOTE();
	void *operator new (size_t stAllocateBlock);	// Can't use new/delete
	void operator delete (void *p);
	SCN_NOTE(const SCN_NOTE &);			// No copies
	const SCN_NOTE operator=(const SCN_NOTE &);	// No assignments
	
	SBYTE		fNoteNumber;
	GAME_STRING fNoteStringId;
};

typedef SHORT HDL_SCN_NOTES;


/* ========================================================================
   Function    - mfSetNoteData
   Description - Fill in the note data fields.
   Returns     - 
   ======================================================================== */
inline void SCN_NOTE::mfSetNoteData(SBYTE const NoteNumber,
                                    GAME_STRING const StringMgrIndex) 
{
	fNoteNumber = NoteNumber;
	fNoteStringId = StringMgrIndex;
}

/* ========================================================================
   Function    - mfGetStrId
   Description - Get this note String Index
   Returns     - 
   ======================================================================== */
inline GAME_STRING const SCN_NOTE::mfGetStrId() const
{
	return fNoteStringId;
}

/* ========================================================================
   Function    - mfGetNoteNumber
   Description - Get this note number
   Returns     - 
   ======================================================================== */
inline SBYTE const SCN_NOTE::mfGetNoteNumber() const
{
	return fNoteNumber;
}


/* ========================================================================
   Function    - mfInitVals
   Description - Initialize the member data.
   Returns     - 
   ======================================================================== */
inline void SCN_NOTE::mfInitVals() 
{
	fNoteNumber = fERROR;
	fNoteStringId = STR_NULL;
}

#endif // _SCNNOTES_HXX
