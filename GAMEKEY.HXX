/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _GAMEKEY_HXX
#define _GAMEKEY_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>

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
/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

class GAME_KEYS {
public:
	typedef enum {
		CURRENT = 0
	} VERSION;
	
	typedef enum {
	
		STOP_RUNNING_DEMO,
		SKIP_TO_NEXT_FRAME,
		
		MAIN_MENU,
		HELP_MENU,
		TOGGLE_CHAT_WINDOW,
		TOGGLE_CHAT_DISPLAY_WINDOW,
		TOGGLE_DIRECT_DRAW,
		TOGGLE_GAME_PAUSE,
		
		MAP_MOVE_UP,
		MAP_MOVE_UPLEFT,
		MAP_MOVE_UPRIGHT,
		MAP_MOVE_DOWN,
		MAP_MOVE_DOWNLEFT,
		MAP_MOVE_DOWNRIGHT,
		MAP_MOVE_LEFT,
		MAP_MOVE_RIGHT,
		
		MAP_SCALE_UP,
		MAP_SCALE_DOWN,
		
		PLAYER_SHOW_STATUS,
		PLAYER_SHOW_INVENTORY,
		
		PLAYER_MOVE_LEFT,
		PLAYER_MOVE_RIGHT,
		PLAYER_MOVE_UP,
		PLAYER_MOVE_DOWN,
		PLAYER_MOVE_FORWARD,
		PLAYER_MOVE_BACKWARD,
		PLAYER_LOOK_UP,
		PLAYER_LOOK_DOWN,
		PLAYER_PITCH_UP,
		PLAYER_PITCH_DOWN,
		PLAYER_SLIDE_LEFT,
		PLAYER_SLIDE_RIGHT,
		PLAYER_MOVE_RUN,
		PLAYER_MOVE_RUN2,
		PLAYER_AUTOCENTER,
		PLAYER_WALKTHRUWALL,
		
		PLAYER_OPENDOOR,
		
		MAP_ZOOMIN,
		MAP_ZOOMOUT,
		MAP_TOGGLEDISPLAY,
		
		CAMERA_FOLLOW_BEHIND,
		
		SHRINK_VIEWING_AREA,
		EXPAND_VIEWING_AREA,
		
		COMBAT_TOGGLEMODE,
		COMBAT_RETURN_TO_MAP,
		COMBAT_RETREAT,
		COMBAT_DONE,
		COMBAT_NEW_LEADER,
		
		COMBAT_HIGH_ATTACK,
		COMBAT_QUICKATTACK_MODIFIER,
		COMBAT_QUICKATTACK_MODIFIER2,
		COMBAT_LOW_ATTACK,
		COMBAT_DUCK,
		COMBAT_DEFEND,
		COMBAT_FALLBACK,
		COMBAT_SPELL1,
		COMBAT_SPELL2,
		COMBAT_SPELL3,
		COMBAT_SPELL4,
		COMBAT_SPELL5,
		COMBAT_SPELL6,
		
		COMBAT_CYCLE_ACTIVE,
		
		COMBAT_CAMERA_FIRSTPERSON,
		COMBAT_CAMERA_BEHINDFRIEND,
		COMBAT_CAMERA_BEHINDFOE,
		COMBAT_CAMERA_RIGHTFRIEND,
		COMBAT_CAMERA_LEFTFRIEND,
		
		
		// Add new keys above here.
		MAX_KEYS
	} KEY;
	
	static VERSION const mfGetVersion();
	
	static LONG const mfGetKey(KEY const /* keyType */ );
	static LONG const mfSetKey(KEY const /* keyType */,
						  LONG const /* keyConst */);
	
	static LONG const mfReadRawData(FILE * const /* fp */ );
	static LONG const mfWriteRawData(FILE * const /* fp */ );
	
protected:
private:
	static VERSION	fVersion;
	static SHORT	fKeyDefinitions[MAX_KEYS];
};

inline GAME_KEYS::VERSION const GAME_KEYS::mfGetVersion()
{
	return fVersion;
}

inline LONG const GAME_KEYS::mfGetKey(GAME_KEYS::KEY const  keyType  )
{
	return fKeyDefinitions[keyType];
}

// returns the previous key value.
inline LONG const GAME_KEYS::mfSetKey(GAME_KEYS::KEY const keyType, LONG const keyConst)
{
	LONG const Result = fKeyDefinitions[keyType];
	
	fKeyDefinitions[keyType] = (SHORT) keyConst;
	return Result;
}

inline LONG const GAME_KEYS::mfReadRawData(FILE * const fp )
{
	LONG Result = fread(&fVersion, sizeof(GAME_KEYS::VERSION), 1, fp);
	Result = fread(fKeyDefinitions, sizeof(SHORT), MAX_KEYS, fp);
	
	return Result;
}

inline LONG const GAME_KEYS::mfWriteRawData(FILE * const fp )
{
	LONG Result = fwrite(&fVersion, sizeof(GAME_KEYS::VERSION), 1, fp);
	Result = fwrite(fKeyDefinitions, sizeof(SHORT), MAX_KEYS, fp);
	
	return Result;
}

#endif // _GAMEKEY_HXX
