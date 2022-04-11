/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _LOADSAVE_HXX
#define _LOADSAVE_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
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
typedef struct {
	unsigned int		IsScenario	: 1;
	unsigned int		RealmNames	: 1;
	unsigned int		Unused		: 1;
} SCENARIO_INFO;

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */

#define GAME_NORMAL		0
#define GAME_INTRO		1
#define GAME_ADVENTURE	2
#define GAME_BATTLE		3

extern LONG master_game_type;

/* ------------------------------------------------------------------------
   Class Definition
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)

void LoadGame (LONG, LONG);
void SaveGame (LONG, LONG);
void LoadGameFiles(FILE *);
void SaveGameFiles(FILE *);
void LoadGameMulti(LONG, LONG);

void LoadGameDoMulti(void);


BOOL GetHelpBit(SHORT bitno);	// true if user doesn't need to see this one
void SetHelpBit(SHORT bitno);
void SetAllHelpBits(void);
#endif

extern SCENARIO_INFO	ScenarioInfo;

/* ------------------------------------------------------------------------
   Inline functions   
   ------------------------------------------------------------------------ */
   
#endif // _LOADSAVE_HXX
