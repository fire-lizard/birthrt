/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: gmenuenm.h
   Author: Greg Hightower
   ========================================================================
   Contains the following general functions:
   
   Contains the following internal functions:
   
   ======================================================================== */
#ifndef _GMENUENM_H
#define _GMENUENM_H

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#if !defined(_MENU_H)
#include "menu.h"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
typedef enum {
	D_AVATAR_ATTRIBUTES = D_LAST_SYSTEM_MENU,
	D_AVATAR_INVENTORY,
	D_BTL_CONTROL,
	D_BTL_CHOOSEFIELD,
	D_COMBAT_CONTROL1,
	D_COMBAT_CONTROL2,
	D_COMBAT_CONTROL3,
	D_COMBAT_CONTROL4,
	D_COMBAT_FIGHTER_CONTROL1,
	D_COMBAT_FIGHTER_CONTROL2,
	D_COMBAT_FIGHTER_CONTROL3,
	D_COMBAT_FIGHTER_CONTROL4,
	D_COMBAT_RPGBOTTOM_BAR,
	D_COMBAT_BLANK1_BAR,
	D_SHOW_ANIMATION,
	D_TITLED_MESSAGE_BOX,
	D_INFOBOX,
	D_RESURRECT_MENU,
	D_MOREINFO,
	D_BTL_CARD,
	D_VICTORY,
	D_DEFEAT,
	D_HELPKEYS,
//	D_MULTIMENU,
//	D_MULTIMENUL,
//	D_MULTIMENU1,
//	D_MULTIMENU2,
	D_AVATAR_ATTRIBUTES2,
	D_CHAT_DISPLAY_WINDOW,
	D_CHAT_DISPLAY_WINDOW1,
	D_LOYALTY,
	D_OTHERDIPLOMACY,
	D_BATTLE_PREP,
	D_BTLCOMMAND,
	D_BUILD_ARMY,
	D_BUILD_ADV,
	D_SELECT_GAME,
	D_ADVISOR,
	D_MULTIWAIT,
	D_DESCRIBE_GAME,
	D_SYSHELP_LARGE,
	D_SYSHELP_MED,
	D_SYSHELP_SMALL,
	D_TUTORIAL_HELP_WIDE,
	D_TUTORIAL_HELP_MED,
	D_HOLDACTION,
	D_LOADING_SCENE,
	D_GAME_COMPLEXITY,
	D_ADVENTURE_PREP,
	D_PARCHMENT,
	D_MULTIHOST,
	D_MULTIENV,
	D_MULTINAME,
	D_MULTIJOIN,
	D_MULTICREATE,
} MENU_ID_ENUM;

#endif

