/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   Author: G. Powell

   Internal data for the string manager.
   ======================================================================== */
#ifndef _STRINT_H
#define _STRINT_H

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define STRINGFILENAME	"strdat.dat"

/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
typedef struct {
	LONG	version;
	char	cpDate[40];
} STR_FILE_HEADER;

typedef struct {
	LONG	lSize;			// size of the block of strings
	LONG	sOffsets[100];	// offsets to string data for this block
} STRBLOCK;

typedef struct {
	SHORT	sBlocks;			// number of blocks
	LONG	sBlockOffsets[100];	// offsets to STRBLOCK for each block
} BLOCKHEADER;

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif


#if defined (__cplusplus)
}
#endif
#endif // _STRINT_H
