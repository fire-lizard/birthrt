/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: AI_UTILS.C
   Author: Greg Hightower
   ========================================================================
   Contains the following general functions:

   AICheckLine - checks for intersection/collision against the blockmap
   AIOpenScript - open/close a script file (toggles)
   AIDropPoint - Place the current Univeral coords in the AIScript file

   Contains the following internal functions:

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SYSTEM.H"
#include "ENGINE.H"
//remove when AICheckLine is moved back to engine
#include "ENGINT.H"

#include "AI_UTILS.H"
#include "ATHING.HXX"
#include "PLAYER.HXX"
#include "SCNMGR.HXX"
#include "SCENE.HXX"

#include "SOUND.HXX"
//#ifdef _WINDOWS
//#ifdef OLD_SOUND
//#include "winsys\sndutil.h"
//#endif
//#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
SHORT TestWorldBit(
	LONG X,
	LONG Y,
	LONG W
);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
FILE	*hAIScriptFile = NULL;

	
	

/* ========================================================================
   Function    - AIOpenScript
   Description - open/close a script file (toggles)
   Returns     - void
   ======================================================================== */
void AIOpenScript( LONG)
{
	static BOOL	open = FALSE;
	
	if(open)
		FileClose( hAIScriptFile );
	else
		hAIScriptFile = FileOpen( "script.txt", "w" );
	
	open = !open;
}

/* ========================================================================
   Function    - AIDropPoint
   Description - Place the current Univeral coords in the AIScript file
   Returns     - void
   ======================================================================== */
void AIDropPoint( LONG )
{
	fprintf( hAIScriptFile, "%ld %ld %ld %ld\n",
		PLAYER_INT_VAL(player.x), PLAYER_INT_VAL(player.y), player.h, player.a );
}


/* ========================================================================
   Function    - pfVoiceInq
   Description - make a voice inquiry
   Returns     -
   ======================================================================== */
void pfVoiceInq( LONG
#ifdef _WINDOWS
#ifdef OLD_SOUND
	type
#endif
#endif
)
{
#ifdef _WINDOWS
#ifdef OLD_SOUND
	SHORT	i = fERROR;
	
	// GEH replace this later
	if (type == DOG)
	{
		i = (SHORT) (43 + random(6));
	}
	else
	if (type == OLD_WOMAN ||
	    type == HIGH_PRIESTESS ||
	    type == PEASANT_WOMAN_1 ||
	    type == PEASANT_WOMAN_2 ||
	    type == SERVANT_WOMAN ||
	    type == WENCH ||
	    type == WIZARD_FEMALE_1 ||
	    type == WIZARD_FEMALE_2 ||
	    type == LORD_FEMALE_1 ||
	    type == LORD_FEMALE_2 ||
	    type == LORD_FEMALE_3
	   )
	{
		i = (SHORT) (SND_AVATAR_FEMALE_INQUIRY_YOUNG + random(SND_AVATAR_FEMALE_INQUIRY_YOUNG_LAST));
	}
	else
	if (type == ARCHER ||
	    type == LIGHT_CAVALRY ||
	    type == ELITE_INFANTRY ||
	    type == INFANTRY ||
	    type == IRREGULAR ||
	    type == KNIGHT ||
	    type == LEVY ||
	    type == OFFICER ||
	    type == PIKEMAN ||
	    type == SCOUT ||
	    type == DWARF_ARCHER ||
	    type == DWARF_INFANTRY ||
	    type == DWARF_OFFICER ||
	    type == ELF_ARCHER ||
	    type == ELF_CAVALRY ||
	    type == ELF_OFFICER ||
	    type == MERC_ARCHER ||
	    type == MERC_CAVALRY ||
	    type == MERC_INFANTRY ||
	    type == MERC_IRREGULAR ||
	    type == MERC_OFFICER ||
	    type == MERC_PIKEMAN ||
	    type == AMBASSADOR ||
	    type == COUNSELOR ||
	    type == CHIEF ||
	    type == GENERAL_1 ||
	    type == GENERAL_2 ||
	    type == GUARD ||
	    type == LIEUTENANT ||
	    type == MERCHANT ||
	    type == TRADER ||
	    type == OLD_MAN ||
	    type == HIGH_PRIEST ||
	    type == MONK ||
	    type == PEASANT_MAN_1 ||
	    type == PEASANT_MAN_2 ||
	    type == RANGER ||
	    type == ROGUE ||
	    type == ROYAL_GUARD ||
	    type == SERVANT_MAN ||
	    type == SHOP_KEEPER_1 ||
	    type == SHOP_KEEPER_2 ||
	    type == SMITH ||
	    type == WARRIOR ||
	    type == WIZARD_MALE_1 ||
	    type == WIZARD_MALE_2 ||
	    type == LORD_MALE_1 ||
	    type == LORD_MALE_2 ||
	    type == LORD_MALE_3 ||
	    type == DWARF_LORD ||
	    type == DWARF_GUARD ||
	    type == ELF_GUARD
	    )
	{
		i = (SHORT) (SND_AVATAR_INQUIRY_MATURE + (random(3)*3) + (type % 3));
	}
		
	if (i != fERROR)
	{
		PlayWave(i, FALSE);
	}
#endif
#endif
}

/* ========================================================================
   Function    - pfVoiceResp
   Description - make a voice acknowledgement
   Returns     -
   ======================================================================== */
void pfVoiceResp( LONG
#ifdef _WINDOWS
#ifdef OLD_SOUND
	type
#endif
#endif
)
{
#ifdef _WINDOWS
#ifdef OLD_SOUND
	SHORT	i = fERROR;
	
	// GEH replace this later
	if (type == DOG)
	{
		i = (SHORT) (43 + random(6));
	}
	else
	if (type == OLD_WOMAN ||
	    type == HIGH_PRIESTESS ||
	    type == PEASANT_WOMAN_1 ||
	    type == PEASANT_WOMAN_2 ||
	    type == SERVANT_WOMAN ||
	    type == WENCH ||
	    type == WIZARD_FEMALE_1 ||
	    type == WIZARD_FEMALE_2 ||
	    type == LORD_FEMALE_1 ||
	    type == LORD_FEMALE_2 ||
	    type == LORD_FEMALE_3
	   )
	{
		i = (SHORT) (64 + random(3));
	}
	else
	if (type == ARCHER ||
	    type == LIGHT_CAVALRY ||
	    type == ELITE_INFANTRY ||
	    type == INFANTRY ||
	    type == IRREGULAR ||
	    type == KNIGHT ||
	    type == LEVY ||
	    type == OFFICER ||
	    type == PIKEMAN ||
	    type == SCOUT ||
	    type == DWARF_ARCHER ||
	    type == DWARF_INFANTRY ||
	    type == DWARF_OFFICER ||
	    type == ELF_ARCHER ||
	    type == ELF_CAVALRY ||
	    type == ELF_OFFICER ||
	    type == MERC_ARCHER ||
	    type == MERC_CAVALRY ||
	    type == MERC_INFANTRY ||
	    type == MERC_IRREGULAR ||
	    type == MERC_OFFICER ||
	    type == MERC_PIKEMAN ||
	    type == AMBASSADOR ||
	    type == COUNSELOR ||
	    type == CHIEF ||
	    type == GENERAL_1 ||
	    type == GENERAL_2 ||
	    type == GUARD ||
	    type == LIEUTENANT ||
	    type == MERCHANT ||
	    type == TRADER ||
	    type == OLD_MAN ||
	    type == HIGH_PRIEST ||
	    type == MONK ||
	    type == PEASANT_MAN_1 ||
	    type == PEASANT_MAN_2 ||
	    type == RANGER ||
	    type == ROGUE ||
	    type == ROYAL_GUARD ||
	    type == SERVANT_MAN ||
	    type == SHOP_KEEPER_1 ||
	    type == SHOP_KEEPER_2 ||
	    type == SMITH ||
	    type == WARRIOR ||
	    type == WIZARD_MALE_1 ||
	    type == WIZARD_MALE_2 ||
	    type == LORD_MALE_1 ||
	    type == LORD_MALE_2 ||
	    type == LORD_MALE_3 ||
	    type == DWARF_LORD ||
	    type == DWARF_GUARD ||
	    type == ELF_GUARD
	    )
	{
		i = (SHORT)( 2 + 9 + (random(3) * 3) + (type % 3));
	}
		
	if (i != fERROR)
	{
//		SndCueWave((BIRTHRT_SND) i, FALSE, NULL,NULL);
//		PlayWave(i, FALSE);
	}
#endif
#endif
}

/* ========================================================================
   Function    - pfCommandOnHide
   Description - on closing of command popup
   Returns     -
   ======================================================================== */
void pfCommandOnHide(void)
{
	PTR_SCENE pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
	pCurrentScene->mfSetSceneState(SCENE_AI::PLAY);
}
