/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: BATTLE.C - Handles Birthright Battles
   Author: 	 Greg Hightower
   ========================================================================
   Contains the following general functions:
   
   Contains the following internal functions:
   
   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#include "system.h"
#include "machine.h"
#include "thingtyp.h"
//#include "btlstr.h"
#include "strmgr.h"
#include "strenum.h"
#include "avatar.hxx"
#include "battle.hxx"
#include "gamemap.hxx"
#include "regents.hxx"
#include "playstat.hxx"
#include "explevel.hxx"

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

#define MAX_BATTLECARDS		9

typedef struct _BATTLECARD {
	SBYTE	aTable[11][7];
} BATTLECARD, *PBATTLECARD;

/* ------------------------------------------------------------------------
	Macros   
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
extern void GainRealmExp(LONG RegentIndex, const LONG ExpAmount,
				const BOOL CanGainLevelNow, const BOOL fSend);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

// the rows are swapped from the card as follows:
//   3,1,2,0,6,4,5,0,9,7,8
static BATTLECARD aBattleCards[MAX_BATTLECARDS] = 
{
	{ // card #102
	  BTL_N, BTL_H,	BTL_H, BTL_N, BTL_H, BTL_N, BTL_H,	// 00 00
	  BTL_H, BTL_N,	BTL_F, BTL_N, BTL_H, BTL_N, BTL_N,	// 00 01
	  BTL_N, BTL_F,	BTL_N, BTL_N, BTL_H, BTL_H, BTL_D,	// 00 10
	  BTL_N, BTL_N,	BTL_N, BTL_N, BTL_N, BTL_N, BTL_N,	// 00 11 NOT USED
	  BTL_N, BTL_H,	BTL_N, BTL_H, BTL_R, BTL_R, BTL_D,	// 01 00
	  BTL_F, BTL_N,	BTL_N, BTL_R, BTL_D, BTL_H, BTL_R,	// 01 01
	  BTL_H, BTL_F,	BTL_H, BTL_H, BTL_H, BTL_H, BTL_H,	// 01 10
	  BTL_N, BTL_N,	BTL_N, BTL_N, BTL_N, BTL_N, BTL_N,	// 01 11 NOT USED
	  BTL_F, BTL_N,	BTL_H, BTL_F, BTL_N, BTL_H, BTL_H,	// 10 00
	  BTL_N, BTL_F,	BTL_H, BTL_H, BTL_F, BTL_D, BTL_H,	// 10 01
	  BTL_F, BTL_H,	BTL_N, BTL_H, BTL_N, BTL_H, BTL_R	// 10 10
	},
	{ // card #103
	  BTL_F, BTL_N,	BTL_H, BTL_F, BTL_N, BTL_H, BTL_D,
	  BTL_N, BTL_F,	BTL_H, BTL_H, BTL_F, BTL_H, BTL_R,
	  BTL_F, BTL_H,	BTL_N, BTL_H, BTL_N, BTL_R, BTL_H,
	  BTL_N, BTL_H,	BTL_H, BTL_N, BTL_H, BTL_N, BTL_H,
	  BTL_H, BTL_F,	BTL_F, BTL_N, BTL_H, BTL_H, BTL_H,
	  BTL_N, BTL_N,	BTL_N, BTL_N, BTL_H, BTL_N, BTL_R,
	  BTL_N, BTL_F,	BTL_N, BTL_H, BTL_R, BTL_H, BTL_H,
	  BTL_F, BTL_H,	BTL_N, BTL_R, BTL_D, BTL_R, BTL_N,
	  BTL_H, BTL_N,	BTL_H, BTL_H, BTL_H, BTL_D, BTL_D
	},
	{ // card #104
	  BTL_N, BTL_F,	BTL_N, BTL_H, BTL_R, BTL_R, BTL_H,
	  BTL_F, BTL_H,	BTL_N, BTL_R, BTL_D, BTL_N, BTL_H,
	  BTL_H, BTL_N,	BTL_H, BTL_H, BTL_H, BTL_H, BTL_R,
	  BTL_N, BTL_N,	BTL_N, BTL_N, BTL_N, BTL_N, BTL_N,	// NOT USED
	  BTL_F, BTL_F,	BTL_H, BTL_F, BTL_N, BTL_H, BTL_H,
	  BTL_N, BTL_H,	BTL_H, BTL_H, BTL_F, BTL_H, BTL_N,
	  BTL_F, BTL_N,	BTL_N, BTL_H, BTL_N, BTL_R, BTL_D,
	  BTL_N, BTL_N,	BTL_N, BTL_N, BTL_N, BTL_N, BTL_N,	// NOT USED
	  BTL_N, BTL_H,	BTL_H, BTL_N, BTL_H, BTL_R, BTL_D,
	  BTL_N, BTL_N,	BTL_F, BTL_N, BTL_H, BTL_H, BTL_R,
	  BTL_H, BTL_F,	BTL_N, BTL_N, BTL_H, BTL_N, BTL_H
	},
	{ // card #105
	  BTL_F, BTL_H,	BTL_H, BTL_N, BTL_H, BTL_H, BTL_R,
	  BTL_N, BTL_N,	BTL_H, BTL_F, BTL_N, BTL_H, BTL_D,
	  BTL_N, BTL_F,	BTL_N, BTL_N, BTL_H, BTL_H, BTL_H,
	  BTL_N, BTL_N,	BTL_N, BTL_N, BTL_N, BTL_N, BTL_N,	// NOT USED
	  BTL_N, BTL_H,	BTL_N, BTL_H, BTL_N, BTL_N, BTL_H,
	  BTL_F, BTL_N,	BTL_H, BTL_N, BTL_D, BTL_D, BTL_H,
	  BTL_H, BTL_F,	BTL_N, BTL_R, BTL_H, BTL_H, BTL_D,
	  BTL_N, BTL_N,	BTL_N, BTL_N, BTL_N, BTL_N, BTL_N,	// NOT USED
	  BTL_H, BTL_N,	BTL_N, BTL_H, BTL_N, BTL_H, BTL_N,
	  BTL_N, BTL_F,	BTL_H, BTL_H, BTL_R, BTL_R, BTL_R,
	  BTL_F, BTL_H,	BTL_F, BTL_H, BTL_F, BTL_N, BTL_H
	},
	{ // card #106
	  BTL_H, BTL_N,	BTL_N, BTL_H, BTL_N, BTL_H, BTL_H,
	  BTL_N, BTL_F,	BTL_H, BTL_H, BTL_R, BTL_H, BTL_H,
	  BTL_F, BTL_H,	BTL_F, BTL_H, BTL_F, BTL_R, BTL_D,
	  BTL_N, BTL_N,	BTL_N, BTL_N, BTL_N, BTL_N, BTL_N,	// NOT USED
	  BTL_F, BTL_N,	BTL_H, BTL_N, BTL_H, BTL_H, BTL_N,
	  BTL_N, BTL_F,	BTL_H, BTL_F, BTL_N, BTL_R, BTL_R,
	  BTL_N, BTL_H,	BTL_N, BTL_N, BTL_H, BTL_D, BTL_H,
	  BTL_N, BTL_N,	BTL_N, BTL_N, BTL_N, BTL_N, BTL_N,	// NOT USED
	  BTL_N, BTL_F,	BTL_N, BTL_H, BTL_H, BTL_N, BTL_R,
	  BTL_F, BTL_H,	BTL_N, BTL_N, BTL_H, BTL_N, BTL_D,
	  BTL_H, BTL_N,	BTL_H, BTL_R, BTL_D, BTL_R, BTL_H
	},
	{ // card #107
	  BTL_N, BTL_H,	BTL_F, BTL_R, BTL_D, BTL_R, BTL_D,
	  BTL_H, BTL_N,	BTL_N, BTL_N, BTL_H, BTL_R, BTL_H,
	  BTL_N, BTL_N,	BTL_H, BTL_N, BTL_H, BTL_N, BTL_N,
	  BTL_N, BTL_N,	BTL_N, BTL_N, BTL_N, BTL_N, BTL_N,	// NOT USED
	  BTL_H, BTL_H,	BTL_N, BTL_H, BTL_F, BTL_F, BTL_H,
	  BTL_F, BTL_N,	BTL_N, BTL_H, BTL_H, BTL_H, BTL_H,
	  BTL_F, BTL_N,	BTL_H, BTL_H, BTL_R, BTL_R, BTL_R,
	  BTL_N, BTL_N,	BTL_N, BTL_N, BTL_N, BTL_N, BTL_N,	// NOT USED
	  BTL_F, BTL_N,	BTL_N, BTL_N, BTL_H, BTL_H, BTL_R,
	  BTL_N, BTL_N,	BTL_N, BTL_H, BTL_N, BTL_N, BTL_D,
	  BTL_N, BTL_H,	BTL_H, BTL_F, BTL_N, BTL_N, BTL_H
	},
	{ // card #108
	  BTL_N, BTL_F,	BTL_N, BTL_H, BTL_D, BTL_N, BTL_N,
	  BTL_F, BTL_H,	BTL_N, BTL_N, BTL_H, BTL_R, BTL_R,
	  BTL_H, BTL_N,	BTL_H, BTL_R, BTL_H, BTL_H, BTL_H,
	  BTL_N, BTL_N,	BTL_N, BTL_N, BTL_N, BTL_N, BTL_N,	// NOT USED
	  BTL_H, BTL_F,	BTL_N, BTL_H, BTL_F, BTL_H, BTL_R,
	  BTL_N, BTL_H,	BTL_H, BTL_H, BTL_H, BTL_H, BTL_D,
	  BTL_F, BTL_N,	BTL_F, BTL_H, BTL_R, BTL_N, BTL_H,
	  BTL_N, BTL_N,	BTL_N, BTL_N, BTL_N, BTL_N, BTL_N,	// NOT USED
	  BTL_F, BTL_H,	BTL_H, BTL_N, BTL_H, BTL_R, BTL_H,
	  BTL_N, BTL_N,	BTL_N, BTL_F, BTL_N, BTL_H, BTL_H,
	  BTL_N, BTL_F,	BTL_F, BTL_N, BTL_N, BTL_H, BTL_D
	},
	{ // card #109
	  BTL_H, BTL_N,	BTL_N, BTL_H, BTL_F, BTL_D, BTL_R,
	  BTL_F, BTL_H,	BTL_N, BTL_H, BTL_H, BTL_D, BTL_D,
	  BTL_F, BTL_N,	BTL_H, BTL_H, BTL_R, BTL_N, BTL_H,
	  BTL_N, BTL_N,	BTL_N, BTL_N, BTL_N, BTL_N, BTL_N,	// NOT USED
	  BTL_F, BTL_N,	BTL_H, BTL_N, BTL_H, BTL_H, BTL_D,
	  BTL_N, BTL_H,	BTL_H, BTL_H, BTL_N, BTL_N, BTL_H,
	  BTL_N, BTL_N,	BTL_N, BTL_F, BTL_N, BTL_R, BTL_N,
	  BTL_N, BTL_N,	BTL_N, BTL_N, BTL_N, BTL_N, BTL_N,	// NOT USED
	  BTL_N, BTL_H,	BTL_F, BTL_R, BTL_D, BTL_D, BTL_H,
	  BTL_H, BTL_N,	BTL_N, BTL_N, BTL_H, BTL_H, BTL_H,
	  BTL_N, BTL_N,	BTL_H, BTL_N, BTL_H, BTL_R, BTL_R
	},
	{ // card #110
	  BTL_F, BTL_N,	BTL_H, BTL_N, BTL_H, BTL_H, BTL_H,
	  BTL_N, BTL_N,	BTL_H, BTL_F, BTL_N, BTL_H, BTL_H,
	  BTL_N, BTL_H,	BTL_N, BTL_H, BTL_N, BTL_D, BTL_R,
	  BTL_N, BTL_N,	BTL_N, BTL_N, BTL_N, BTL_N, BTL_N,	// NOT USED
	  BTL_N, BTL_N,	BTL_F, BTL_R, BTL_D, BTL_R, BTL_R,
	  BTL_H, BTL_N,	BTL_N, BTL_N, BTL_H, BTL_R, BTL_D,
	  BTL_N, BTL_H,	BTL_H, BTL_N, BTL_H, BTL_N, BTL_H,
	  BTL_N, BTL_N,	BTL_N, BTL_N, BTL_N, BTL_N, BTL_N,	// NOT USED
	  BTL_H, BTL_N,	BTL_N, BTL_H, BTL_F, BTL_H, BTL_D,
	  BTL_F, BTL_H,	BTL_N, BTL_H, BTL_H, BTL_N, BTL_H,
	  BTL_F, BTL_N,	BTL_H, BTL_H, BTL_R, BTL_H, BTL_N
	}
};

	// Class   Move Def Mel ChrgMissMor Mag Bonus
BATTLE_TROOP_INFO	BattleTroopInfo[] = {
{ CARD_FLAG,	0,	0,	0,	0,	0,	0,	0,	BTL_STR_NOBONUS},				// NULL ENTRY
{ CARD_FLAG,	1,	3,	3,	0,	0,	2,	0,	BTL_STR_MELEE_VS_PIKES_IRREG},	// Infantry
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_MELEE_VS_PIKES_IRREG},	// Elite Infantry
{ CARD_FLAG,	2,	2,	2,	0,	4,	2,	0,	BTL_STR_MISSILE_VS_CAVALRY},	// Archer
{ CARD_FLAG,	2,	2,	3,	0,	2,	2,	0,	BTL_STR_NOBONUS},				// Irregular
{ CARD_FLAG,	1,	3,	3,	0,	0,	2,	0,	BTL_STR_PIKEMEN},				// Pikeman
{ CARD_FLAG,	1,	1,	2,	0,	0,	1,	0,	BTL_STR_NOBONUS},				// Levy
{ CARD_FLAG,	3,	2,	2,	0,	3,	2,	0,	BTL_STR_IGNORE_TERRAIN},		// Scout
{ CARD_FLAG,	2,	4,	4,	6,	0,	3,	0,	BTL_STR_NOBONUS},				// Knight/Heavy Cavalry
{ CARD_FLAG,	3,	3,	3,	5,	1,	2,	0,	BTL_STR_NOBONUS},				// Light Cavalry
{ CARD_FLAG,	1,	3,	3,	0,	0,	1,	0,	BTL_STR_VS_PIKES_IRREG},		// Mercenary Infantry
{ CARD_FLAG,	2,	2,	2,	0,	4,	1,	0,	BTL_STR_NOBONUS},				// Mercenary Archer
{ CARD_FLAG,	2,	2,	3,	0,	2,	1,	0,	BTL_STR_NOBONUS},				// Mercenary Irregular
{ CARD_FLAG,	1,	3,	3,	0,	0,	1,	0,	BTL_STR_PIKEMEN},				// Mercenary Pikeman
{ CARD_FLAG,	3,	3,	3,	5,	1,	1,	0,	BTL_STR_NOBONUS},				// Mercenary Cavalry
{ CARD_FLAG,	2,	2,	3,	0,	0,	3,	0,	BTL_STR_IGNORE_TERRAIN},		// Elf Infantry
{ CARD_FLAG,	2,	2,	2,	0,	5,	3,	0,	BTL_STR_IGNORE_TERRAIN},		// Elf Archer
{ CARD_SWORDS,	4,	3,	3,	5,	4,	3,	0,	BTL_STR_IGNORE_TERRAIN},		// Elf Cavalry
{ CARD_FLAG,	1,	5,	3,	0,	0,	3,	0,	BTL_STR_IGNORE_RF_BY_MAGIC},	// Dwarf Infantry
{ CARD_SHIELD,	1,	4,	2,	0,	4,	3,	0,	BTL_STR_IGNORE_RF_BY_MAGIC},	// Dwarf Archer
{ CARD_SHIELD,	1,	2,	2,	0,	1,	1,	0,	BTL_STR_MELEE_VS_PIKES_IRREG},	// Goblin Infantry
{ CARD_FLAG,	1,	2,	1,	0,	3,	1,	0,	BTL_STR_MISSILE_VS_CAVALRY},	// Goblin Archer
{ CARD_SWORDS,	2,	3,	3,	4,	1,	2,	0,	BTL_STR_NOBONUS},				// Goblin Cavalry
{ CARD_SWORDS,	2,	4,	4,	0,	2,	1,	0,	BTL_STR_MELEE_VS_PIKES_IRREG},	// Gnoll Infantry
{ CARD_FLAG,	2,	3,	3,	0,	2,	1,	0,	BTL_STR_NOBONUS},				// Gnoll Irregular
{ CARD_FLAG,	3,	3,	3,	4,	0,	2,	0,	BTL_STR_VS_CAVALRY},			// Troop Harpy
{ CARD_SHIELD,	1,	7,	5,	0,	0,	2,	0,	BTL_STR_IGNORE_RF_EXCEPT_MAGIC},// Troop Skeleton
{ CARD_FLAG,	2,	3,	3,	4,	0,	2,	0,	BTL_STR_IGNORE_TERRAIN},		// Troop Giant Spider
{ CARD_SWORDS,	2,	3,	3,	4,	0,	2,	0,	BTL_STR_NOBONUS},				// Troop Hell Hound
{ CARD_FLAG,	2,	3,	3,	4,	0,	2,	0,	BTL_STR_IGNORE_TERRAIN},		// Troop Wyvern Spider

{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_REGENT},                // REG_LORDM1_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_REGENT},                // REG_LORDM2_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_REGENT},                // REG_LORDM3_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_REGENT},                // REG_LORDM4_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_REGENT},                // REG_LORDM5_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_REGENT},                // REG_LORDM6_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_REGENT},                // REG_LORDM7_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	1,	BTL_STR_REGENT},                // REG_WIZDM1_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	1,	BTL_STR_REGENT},                // REG_WIZDM2_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	1,	BTL_STR_REGENT},                // REG_WIZDM3_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	1,	BTL_STR_REGENT},                // REG_WIZDM4_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_REGENT},                // REG_GILDM1_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_REGENT},                // REG_GILDM2_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	2,	BTL_STR_REGENT},                // REG_PRSTM1_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	2,	BTL_STR_REGENT},                // REG_PRSTM2_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_REGENT},                // REG_LORDF1_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_REGENT},                // REG_LORDF2_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_REGENT},                // REG_LORDF3_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	1,	BTL_STR_REGENT},                // REG_WIZDF1_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	1,	BTL_STR_REGENT},                // REG_WIZDF2_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_REGENT},                // REG_GILDF1_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	2,	BTL_STR_REGENT},                // REG_PRSTF1_UNIT_ICON,
{ CARD_FLAG,	2,	2,	2,	0,	5,	3,	0,	BTL_STR_IGNORE_TERRAIN},        // REG_ELORD1_UNIT_ICON,
{ CARD_FLAG,	2,	2,	2,	0,	5,	3,	0,	BTL_STR_IGNORE_TERRAIN},        // REG_ELORD2_UNIT_ICON,
{ CARD_FLAG,	2,	2,	2,	0,	5,	3,	0,	BTL_STR_IGNORE_TERRAIN},        // REG_ELADY1_UNIT_ICON,
{ CARD_FLAG,	1,	4,	2,	0,	4,	3,	0,	BTL_STR_IGNORE_RF_BY_MAGIC},    // REG_DLORD1_UNIT_ICON,
{ CARD_FLAG,	1,	4,	2,	0,	4,	3,	0,	BTL_STR_IGNORE_RF_BY_MAGIC},    // REG_DLORD2_UNIT_ICON,
{ CARD_FLAG,	1,	4,	2,	0,	4,	3,	0,	BTL_STR_IGNORE_RF_BY_MAGIC},    // REG_DLORD3_UNIT_ICON,
{ CARD_FLAG,	1,	2,	1,	0,	3,	3,	0,	BTL_STR_MISSILE_VS_CAVALRY},    // REG_GLORD1_UNIT_ICON,
{ CARD_FLAG,	1,	2,	1,	0,	3,	3,	0,	BTL_STR_MISSILE_VS_CAVALRY},    // REG_GLADY1_UNIT_ICON,
																		    
{ CARD_FLAG,	1,	9,	7,	9,	0,	3,	1,	BTL_STR_AWNSHEGN},			    // GORGON_UNIT_ICON,
{ CARD_FLAG,	2,	9,	5,	7,	4,	3,	0,	BTL_STR_AWNSHEGN},			    // SPIDERKING_UNIT_ICON,
{ CARD_FLAG,	4,	9,	5,	7,	4,	3,	1,	BTL_STR_AWNSHEGN},			    // RHUOBHE_UNIT_ICON,
																		    
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_OFFICER},               // LET_LORDM1_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_OFFICER},               // LET_LORDM2_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_OFFICER},               // LET_LORDM3_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_OFFICER},               // LET_LORDM4_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_OFFICER},               // LET_LORDM5_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_OFFICER},               // LET_LORDM6_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_OFFICER},               // LET_LORDM7_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	1,	BTL_STR_OFFICER},               // LET_WIZDM1_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	1,	BTL_STR_OFFICER},               // LET_WIZDM2_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	1,	BTL_STR_OFFICER},               // LET_WIZDM3_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	1,	BTL_STR_OFFICER},               // LET_WIZDM4_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_OFFICER},               // LET_GILDM1_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_OFFICER},               // LET_GILDM2_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	2,	BTL_STR_OFFICER},               // LET_PRSTM1_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	2,	BTL_STR_OFFICER},               // LET_PRSTM2_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_OFFICER},               // LET_LORDF1_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_OFFICER},               // LET_LORDF2_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_OFFICER},               // LET_LORDF3_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	1,	BTL_STR_OFFICER},               // LET_WIZDF1_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	1,	BTL_STR_OFFICER},               // LET_WIZDF2_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	0,	BTL_STR_OFFICER},               // LET_GILDF1_UNIT_ICON,
{ CARD_FLAG,	1,	4,	4,	0,	0,	3,	2,	BTL_STR_OFFICER},               // LET_PRSTF1_UNIT_ICON,
{ CARD_FLAG,	2,	2,	2,	0,	5,	3,	0,	BTL_STR_IGNORE_TERRAIN},        // LET_ELORD1_UNIT_ICON,
{ CARD_FLAG,	2,	2,	2,	0,	5,	3,	0,	BTL_STR_IGNORE_TERRAIN},        // LET_ELORD2_UNIT_ICON,
{ CARD_FLAG,	2,	2,	2,	0,	5,	3,	0,	BTL_STR_IGNORE_TERRAIN},        // LET_ELADY1_UNIT_ICON,
{ CARD_FLAG,	1,	4,	2,	0,	4,	3,	0,	BTL_STR_IGNORE_RF_BY_MAGIC},    // LET_DLORD1_UNIT_ICON,
{ CARD_FLAG,	1,	4,	2,	0,	4,	3,	0,	BTL_STR_IGNORE_RF_BY_MAGIC},    // LET_DLORD2_UNIT_ICON,
{ CARD_FLAG,	1,	4,	2,	0,	4,	3,	0,	BTL_STR_IGNORE_RF_BY_MAGIC},    // LET_DLORD3_UNIT_ICON,
{ CARD_FLAG,	1,	2,	1,	0,	3,	3,	0,	BTL_STR_MISSILE_VS_CAVALRY},    // LET_GLORD1_UNIT_ICON,
{ CARD_FLAG,	1,	2,	1,	0,	3,	3,	0,	BTL_STR_MISSILE_VS_CAVALRY},    // LET_GLADY1_UNIT_ICON,
};	

#if defined(_DEBUG)
LONG MaxBattleTroopInfoIndex = sizeof(BattleTroopInfo)/ sizeof(BATTLE_TROOP_INFO);
#endif

#if 0
/* battle card string table */
CHAR BattleCardBonusStrings[BTL_LASTBONUS][128] = {
	BTL_STR_NOBONUS,
	BTL_STR_VS_CAVALRY,
	BTL_STR_MISSILE_VS_CAVALRY,
	BTL_STR_VS_PIKES_IRREG,
	BTL_STR_MELEE_VS_PIKES_IRREG,
	BTL_STR_PIKEMEN,
	BTL_STR_IGNORE_RF_BY_MAGIC,
	BTL_STR_IGNORE_RF_EXCEPT_MAGIC,
	BTL_STR_IGNORE_TERRAIN,
	BTL_STR_REGENT,
	BTL_STR_OFFICER,
	BTL_STR_AWNSHEGN,
};
#endif


// Battle experience points:
#define NUM_OF_BTL_EXP_CLASSES	4
static LONG BattleExpPointsTable[NUM_OF_BTL_EXP_TYPES][NUM_OF_BTL_EXP_CLASSES] = {
/*      Action                       		Fighter,	Priest,	Thief,	Wizard */
/*	BTL_EXP_ENGAGE,                 */	{   500,     500,     300,     500 },
/*	BTL_EXP_ONFIELD,                */	{  1000,     500,     300,     500 },
/*	BTL_EXP_CAST_MAGIC              */	{    50,     200,      50,     200 },
/*	BTL_EXP_WIN,                    */	{  1000,     500,     500,     500 },
/*	BTL_EXP_WIN_GREATER_STRENGTH,   */	{  1000,     500,     500,     500 },
};


/* ========================================================================
   Function    - ResolveBattleCard
   Description - Use Battle Tables to resolve a phase of conflict
   Returns     - battle result (see battle.h)
   ======================================================================== */

LONG ResolveBattleCard( 
	LONG	AIcon, 
	LONG	Attatck, 
	LONG	DIcon, 
	LONG	Defense
)
{
	LONG	IconCombo, AttDef, WhichBtlCard;
	
	WhichBtlCard = random(MAX_BATTLECARDS);
	IconCombo = (AIcon << 2) + DIcon;
	
	// keep with -3 to 3
	AttDef = Attatck - Defense;
	AttDef = (AttDef < -3)? -3 : AttDef;
	AttDef = (AttDef > 3) ?  3 : AttDef;
	
	AttDef += 3;
	return aBattleCards[WhichBtlCard].aTable[IconCombo][AttDef];
}

/* ========================================================================
   Function    - ResolveBattleMagic
   Description - Use Battle Magic Tables to resolve a phase of conflict
   Returns     - battle result (see battle.h)
   ======================================================================== */
LONG ResolveBattleMagic( LONG	Attack )
{
	LONG	ReturnVal;
	
	// GEH This should be looked into later
	if( Attack == 1 )	// wizard spells
	{
		ReturnVal = random(4) + 2;
	}
	else
	if( Attack == 2 )	// priest spells
	{
		ReturnVal = random(4) + 2;
	}
	else	
		ReturnVal = BTL_N;	// no-op
	
	return ReturnVal;
}

/* ========================================================================
   Function    - BattleRound
   Description - make a pass through the battle ai
   Returns     - battle phase must end
   ======================================================================== */
BOOL BattleRound( void )
{
	BOOL bResult = TRUE;
	
	// every 100 frames in Arcade mode
	// or each Phase in Phase mode
	// else just return
	
	//Movement Phase
	//	Attacker moves
	// 	Defender moves
	//Attack Phase
	//	Resolve magical attacks
	//	Resolve stationary missle attacks
	//	Resolve charge attacks
	//	Resolve melee attacks
	//	Resolve moving missle attacks
	//Morale Phase
	//	Routed units flee
	//	Units forced to fall back do so
	//	Routed units attempt morale checks
	//	Surrender or withdrawal
	//Repeat
	
	return bResult;
}

/* ========================================================================
   Function    - GetBattleClass
   Description - returns data out of the battle array
   Returns     - 
   ======================================================================== */
SHORT GetBattleClass( SHORT unit_icon )
{
#if defined (_DEBUG)
	if (unit_icon < 0 || unit_icon >= MaxBattleTroopInfoIndex)
		fatal_error("ERROR! GetBattleClass unit_icon (%hd) out of bounds of %ld.\n",
		            unit_icon, MaxBattleTroopInfoIndex);
#endif

	return (BattleTroopInfo[unit_icon].Class);
}

/* ========================================================================
   Function    - GetBattleMove
   Description - returns data out of the battle array
   Returns     - 
   ======================================================================== */
SHORT GetBattleMove( SHORT unit_icon )
{
#if defined (_DEBUG)
	if (unit_icon < 0 || unit_icon >= MaxBattleTroopInfoIndex)
		fatal_error("ERROR! GetBattleMove unit_icon (%hd) out of bounds of %ld.\n",
		            unit_icon, MaxBattleTroopInfoIndex);
#endif
	return (BattleTroopInfo[unit_icon].Move);
}

/* ========================================================================
   Function    - GetBattleDefense
   Description - returns data out of the battle array
   Returns     - 
   ======================================================================== */
SHORT GetBattleDefense( SHORT unit_icon )
{
#if defined (_DEBUG)
	if (unit_icon < 0 || unit_icon >= MaxBattleTroopInfoIndex)
		fatal_error("ERROR! GetBattleDefense unit_icon (%hd) out of bounds of %ld.\n",
		            unit_icon, MaxBattleTroopInfoIndex);
#endif
	return (BattleTroopInfo[unit_icon].Defend);
}


/* ========================================================================
   Function    - GetBattleMelee
   Description - returns data out of the battle array
   Returns     - 
   ======================================================================== */
SHORT GetBattleMelee( SHORT unit_icon )
{
#if defined (_DEBUG)
	if (unit_icon < 0 || unit_icon >= MaxBattleTroopInfoIndex)
		fatal_error("ERROR! GetBattleMelee unit_icon (%hd) out of bounds of %ld.\n",
		            unit_icon, MaxBattleTroopInfoIndex);
#endif
	return (BattleTroopInfo[unit_icon].Melee);
}


/* ========================================================================
   Function    - GetBattleCharge
   Description - returns data out of the battle array
   Returns     - 
   ======================================================================== */
SHORT GetBattleCharge( SHORT unit_icon )
{
#if defined (_DEBUG)
	if (unit_icon < 0 || unit_icon >= MaxBattleTroopInfoIndex)
		fatal_error("ERROR! GetBattleCharge unit_icon (%hd) out of bounds of %ld.\n",
		            unit_icon, MaxBattleTroopInfoIndex);
#endif
	return (BattleTroopInfo[unit_icon].Charge);
}


/* ========================================================================
   Function    - GetBattleMissile
   Description - returns data out of the battle array
   Returns     - 
   ======================================================================== */
SHORT GetBattleMissile( SHORT unit_icon )
{
#if defined (_DEBUG)
	if (unit_icon < 0 || unit_icon >= MaxBattleTroopInfoIndex)
		fatal_error("ERROR! GetBattleMissile unit_icon (%hd) out of bounds of %ld.\n",
		            unit_icon, MaxBattleTroopInfoIndex);
#endif
	return (BattleTroopInfo[unit_icon].Missile);
}


/* ========================================================================
   Function    - GetBattleMorale
   Description - returns data out of the battle array
   Returns     - 
   ======================================================================== */
SHORT GetBattleMorale( SHORT unit_icon )
{
#if defined (_DEBUG)
	if (unit_icon < 0 || unit_icon >= MaxBattleTroopInfoIndex)
		fatal_error("ERROR! GetBattleMorale unit_icon (%hd) out of bounds of %ld.\n",
		            unit_icon, MaxBattleTroopInfoIndex);
#endif
	return (BattleTroopInfo[unit_icon].Morale);
}


/* ========================================================================
   Function    - GetBattleMagic
   Description - returns data out of the battle array
   Returns     - 
   ======================================================================== */
SHORT GetBattleMagic( SHORT unit_icon )
{
#if defined (_DEBUG)
	if (unit_icon < 0 || unit_icon >= MaxBattleTroopInfoIndex)
		fatal_error("ERROR! GetBattleMagic unit_icon (%hd) out of bounds of %ld.\n",
		            unit_icon, MaxBattleTroopInfoIndex);
#endif
	return (BattleTroopInfo[unit_icon].Magic);
}


/* ========================================================================
   Function    - GetBattleBonus
   Description - returns data out of the battle array
   Returns     - 
   ======================================================================== */
SHORT GetBattleBonus( SHORT unit_icon )
{
#if defined (_DEBUG)
	if (unit_icon < 0 || unit_icon >= MaxBattleTroopInfoIndex)
		fatal_error("ERROR! GetBattleBonus unit_icon (%hd) out of bounds of %ld.\n",
		            unit_icon, MaxBattleTroopInfoIndex);
#endif
	return (BattleTroopInfo[unit_icon].Bonus);
}


/* ========================================================================
   Function    - BattleExpPoints
	Description - Doll out experience points for battle
	Returns     - number of points
	======================================================================== */
LONG BattleExpPoints(
	SHORT UnitIndex,
	BATTLE_EXPTYPE ActionType, 
	LONG Modifier,
	BOOL fSend )
{
	LONG	Amount, A1, A2, Class1, Class2;
	LONG  i, RI = -1;
	
	// if this is a character
	if (units[UnitIndex].Icon >= FIRST_CHARACTER_TYPE)
	{
		for (i=1; i<CHARACTER_COUNT; ++i)			// scan for regent
		{
			if (regents[i].mfGetunit() == UnitIndex)
			{
				RI = i;
				break;
			}
		}
	}
	
	if ( RI == -1 )
		return 0;
		
	// simple default answer
	Amount = 0;
	
	if (playerstats[RI] != fERROR)
	{
		// collect primary/secondary class types
		Class1 = regents[RI].mfGetClass1(); 
		Class2 = regents[RI].mfGetClass2(); 
		
		// convert classes to table lookup values
		switch ( Class1 )
		{
		case FIGHTER:
		case PALADIN:
		case RANGER:
			Class1 = 0;
			break;
		case PRIEST:
			Class1 = 1;
			break;
		case THIEF:
		case BARD:
			Class1 = 2;
			break;
		case WIZARD:
			Class1 = 3;
			break;
		
		case NO_CLASS:
		default:
			// an error, just bail out
			return 0;
		}
		switch ( Class2 )
		{
		case FIGHTER:
		case PALADIN:
		case RANGER:
			Class2 = 0;
			break;
		case PRIEST:
			Class2 = 1;
			break;
		case THIEF:
		case BARD:
			Class2 = 2;
			break;
		case WIZARD:
			Class2 = 3;
			break;
		
		case NO_CLASS:
		default:
			// this character is no second class shmuck
			break;
		}
		
		// give them highest value of their two possible classes
		A1 = BattleExpPointsTable[ActionType][Class1];
		if ( Class2 != NO_CLASS )
		{
			A2 = BattleExpPointsTable[ActionType][Class2];
			Amount = MAX(A1,A2);
		}
		else
		{
			Amount = A1;
		}
			
		// check amount modifiers
		switch ( ActionType )
		{
		case BTL_EXP_CAST_MAGIC:
			Amount *= Modifier;
			break;
		}
		
		GainRealmExp(RI, Amount, TRUE, fSend);
	}

	return Amount;
}
/* ======================================================================== */
