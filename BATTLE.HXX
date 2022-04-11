/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: BATTLE.H - Tables and data for battle
   Author: 	 Greg Hightower
   ======================================================================== */
#ifndef _BATTLE_HXX
#define _BATTLE_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_AVATAR_HXX)
#include "avatar.hxx"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
// HUNGARIAN NOTE: All values are of type long unless specified.  So if
// is doesn't start with a capital letter, the lower case should be a
// prefix.  'u' means Ulong.

#define CARD_SWORDS		0
#define CARD_SHIELD		1
#define CARD_FLAG		2
#define BTL_MAX_MORALE	3

/* ------------------------------------------------------------------------
   Enum
   ------------------------------------------------------------------------ */

// Battle Result Types
enum {
	BTL_N = 0,
	BTL_F,
	BTL_H,
	BTL_R,
	BTL_D,
};

// Battle Bonuses
enum {
	BTL_NOBONUS = 0,
	BTL_VS_CAVALRY,
	BTL_MISSILE_VS_CAVALRY,
	BTL_VS_PIKES_IRREG,
	BTL_MELEE_VS_PIKES_IRREG,
	BTL_PIKEMEN,
	BTL_IGNORE_RF_BY_MAGIC,
	BTL_IGNORE_RF_EXCEPT_MAGIC,
	BTL_IGNORE_TERRAIN,
	BTL_REGENT,
	BTL_OFFICER,
	BTL_AWNSHEGN,
	BTL_LASTBONUS,
};

// table of realm experience points
typedef enum {
	BTL_EXP_ENGAGE = 0,					// just engaging a battle
	BTL_EXP_ONFIELD,						// moving out of the reserves
	BTL_EXP_CAST_MAGIC,					// casing magic bonus per level
	BTL_EXP_WIN,							// winning
	BTL_EXP_WIN_GREATER_STRENGTH,		// winning over a foe of greater strength bonus
	NUM_OF_BTL_EXP_TYPES
} BATTLE_EXPTYPE;

/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
typedef struct _BATTLE_TROOP_INFO {
	SHORT	Class;
	SHORT	Move;
	SHORT	Defend;
	SHORT	Melee;
	SHORT	Charge;
	SHORT	Missile;
	SHORT	Morale;
	SHORT	Magic;
	SHORT	Bonus;
} BATTLE_TROOP_INFO;

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

long ResolveBattleCard( LONG AIcon, LONG Attatck, LONG DIcon, LONG Defense);
BOOL BattleRound( void );

SHORT GetBattleClass( SHORT unit_icon );
SHORT GetBattleMove( SHORT unit_icon );
SHORT GetBattleDefense( SHORT unit_icon );
SHORT GetBattleMelee( SHORT unit_icon );
SHORT GetBattleCharge( SHORT unit_icon );
SHORT GetBattleMissile( SHORT unit_icon );
SHORT GetBattleMorale( SHORT unit_icon );
SHORT GetBattleMagic( SHORT unit_icon );
SHORT GetBattleBonus( SHORT unit_icon );

LONG BattleExpPoints(
	SHORT UnitIndex,
	BATTLE_EXPTYPE ActionType, 
	LONG Modifier,
	BOOL fSend );
	
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

extern BATTLE_TROOP_INFO BattleTroopInfo[];
//extern CHAR BattleCardBonusStrings[BTL_LASTBONUS][128];

#endif // _BATTLE_HXX

