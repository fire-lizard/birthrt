/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: Greg Hightower
   Author:
   ========================================================================
   Contains the following general functions:
   
   
   ======================================================================== */

#ifndef _VOXSND_HXX
#define _VOXSND_HXX   
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_TYINGTYP_H)
#include "thingtyp.h"
#endif


/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
	Macros   
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Enum
   ------------------------------------------------------------------------ */
typedef enum {
	VOX_NO_ENTRY = -1,
	VOX_HUMAN_MALE_YOUNG_1 = 0,		// done	0
	VOX_HUMAN_MALE_YOUNG_2,			// done	1
	VOX_HUMAN_MALE_YOUNG_3,			// done	2
	VOX_HUMAN_MALE_MATURE_1,		// done	3
	VOX_HUMAN_MALE_MATURE_2,		// done	4
	VOX_HUMAN_MALE_OLD_1,			// done	5
	VOX_HUMAN_FEMALE_YOUNG_1,		// done	6
	VOX_HUMAN_FEMALE_YOUNG_2,		// done	7
	VOX_HUMAN_FEMALE_MATURE_1,		// done	8
	VOX_HUMAN_FEMALE_MATURE_2,		// done	9
	VOX_HUMAN_FEMALE_OLD_1,			// done	10
	VOX_ELF_MALE_1,				// done	11
	VOX_ELF_MALE_2,				//	12
	VOX_ELF_FEMALE_1,			// done	13
	VOX_ELF_FEMALE_2,			// done	14
	VOX_HALF_ELF_MALE_1,			//	15
	VOX_HALF_ELF_FEMALE_1,			// done	16
	VOX_DWARF_MALE_1,			// done	17
	VOX_GOBLIN_MALE_1,			// done	18
	VOX_GOBLIN_FEMALE_1,			// done	19
	VOX_SKELETON_1,				// done	20
	VOX_OGRE_1,				// done	21
	VOX_GORGON_1,				// done	22
	VOX_RHUOBHE_1,				// done	23
	VOX_GNOLL_1,				// done	24
	VOX_OTYUGH_1,				// done	25
	VOX_ANKHEG_1,				// done	26
	VOX_SPIDER_KING_1,			// done 27
	VOX_VOICE_SIZE
} VOX_TYPE;

typedef enum {
	VOX_NO_BLOCK = -1,
	VOX_CASTLE_INQUIRY = 0,
	VOX_CASTLE_ACKNOWLEDGE,
	VOX_CASTLE_DECLINE,
	VOX_CHALLENGES,
	VOX_DUEL_TAUNTS,
	VOX_BATTLE_INQUIRY,
	VOX_BATTLE_ACKNOWLEDGE,
	VOX_BATTLE_DECLINE,
	VOX_BUSY,
	VOX_READY,
	VOX_WIZARD_READY,
	VOX_WAITING,
	VOX_UNDER_ATTACK,
	VOX_WE_GET_ROUTED,
	VOX_BACK_TO_RESERVES,
	VOX_RETREAT,
	VOX_ARCHERS_READY,
	VOX_PRIEST_READY,
	VOX_THEY_GET_ROUTED,
	VOX_BLOCK_SIZE
} VOX_BLOCK;

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
SHORT AddVoxSnd( VOX_BLOCK Block, LONG MyThingsIndex);
SHORT AddVoxSndObj( VOX_TYPE Voice, VOX_BLOCK Block, LONG MyThingsIndex);
SHORT AddVoxSndType( VOX_BLOCK Block, THINGTYPE ThingType);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

#endif //_VOXSND_HXX
