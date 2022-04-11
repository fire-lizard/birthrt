/* =======================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   =======================================================================
   Filename: EXPERIEN.H - Collection of Experience Minumums for characters
   Author: 	 Gary Powell
   ======================================================================= */
#ifndef _EXPLEVEL_HXX
#define _EXPLEVEL_HXX

/* -----------------------------------------------------------------------
   Sub Includes
   ----------------------------------------------------------------------- */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_PLAYCLAS_HXX)
#include "playclas.hxx"
#endif

#if !defined(_EXPERIEN_HXX)
#include "experien.hxx"
#endif

/* -----------------------------------------------------------------------
   Typedefs
   ----------------------------------------------------------------------- */
typedef struct {    
    UBYTE save[10];
} SAVECOLUMN;

typedef struct {
    SHORT levels_per_advance;
    SHORT best_save_level;
    SAVECOLUMN type[5];
} SAVETABLE;

typedef enum {
    ST_PARALYZATION  = 0,
    ST_POISON        = 0,
    ST_DEATH_MAGIC   = 0,
    ST_ROD           = 1,
    ST_STAFF         = 1,
    ST_WAND          = 1,
    ST_PETRIFICATION = 2,
    ST_POLYMORPH     = 2,
    ST_BREATH_WEAPON = 3,
    ST_SPELL         = 4,
    ST_MAGIC         = 4
} SAVETYPE;

typedef enum {
    SC_WARRIOR,
    SC_PRIEST,
    SC_ROGUE,
    SC_WIZARD
} SCLASSTYPE;

/* -----------------------------------------------------------------------
   Functions
   ----------------------------------------------------------------------- */
// put these in yourself... trying to avoid #include loops
// extern BOOL ExpCheck(PLAYER_STATS * pPS);
// extern void GainExp(PLAYER_STATS * pPS, const LONG ExpAmount,
// 				const BOOL CanGainLevelNow);
				
extern BOOL SaveVs(const SHORT hPlayerStats,
                   SAVETYPE savetype, SBYTE modifier);

#endif // _EXPLEVEL_HXX
