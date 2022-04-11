/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename:  playid.cpp
   Author:   Lan Zhou

   ========================================================================

   Contains the following general functions:


   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
//#include "gamestr.h"
#include "playid.hxx"
#include "strenum.h"


/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

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


GAME_STRING const PLAYER_TITLE::player_title_table[] = {
	  STR_NULL,						
	  STR_TITLE_PATRIARCH,					
	  STR_TITLE_HIGH_HIEROPHANT,			
	  STR_TITLE_DUCHESS,					
	  STR_TITLE_BARONESS,					
	  STR_TITLE_PRINCE,						
	  STR_TITLE_OVERTHANE,					
	  STR_TITLE_GUILDER,					
	  STR_TITLE_WIZARD,						
	  STR_TITLE_COUNT,						
	  STR_TITLE_BARON,						
	  STR_TITLE_EMERALD_QUEEN,				
	  STR_TITLE_QUEEN,						
	  STR_TITLE_ARCHPRIEST,					
	  STR_TITLE_LORD,						
	  STR_TITLE_THANE,						
	  };

GAME_STRING const PLAYER_DESCRIPTION::player_des_table[] = {
	  STR_NULL
	  };
				
