/* =®RM250¯======================================================================
   Copyright (c) 1990,1995	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: PLAYER.C -Handles player related things, such as movement
   Author:    Chris Phillips & Wes Cumberland
   				total rewrite 4-18-96 GEH
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:
   InitPlayer       -initializes the player to a location, pitch and height
   SetPlayer        -sets the player to a location, pitch and height
   MovePlayer       -checks the keyboard and moves the player according to it

   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
	------------------------------------------------------------------------ */
#include <stdio.h>
#include <math.h>
#ifdef _WINDOWS
#include <Windows.h>

#if defined (_STATUS)
#include "winsys\status.h"
#endif

#if defined (_DEBUG)
#include "winsys\mono_c.h"
#endif

#endif	// _WINDOWS

#include "SYSTEM.H"
#include "ENGINE.H"
#include "ENGINT.H"
#include "MACHINE.H"

#include "GAMEKEY.HXX"
#include "PLAYER.HXX"
#include "SPECIAL.H"


/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
	//REVISIT:Needs to have all player movement turned into
	//          Regions and tasks.
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
// list the default values
#define PLAYER_SPEED			2
#define INIT_FACT				TargetFactor=105;
#define MAX_STEP_HEIGHT         25   // [RCC]
#define PLAYER_WIDTH			12		// [d12-06-96 JPC] reduced from 16
											// so players can get through doors
											// and tunnels more easily
#define PLAYER_HEIGHT			68
#define JUMP_VELOCITY           25   // [GEH]
#define FALL_RATE               6    // [GEH]
#define TERMINAL_VELOCITY       80   // [GEH]

#define MAX_PITCH				200
#define MAX_XY_SPEED			(31 << PLAYER_FIXEDPT)   //prime
#define MAX_A_SPEED				11   //prime
#define MAX_P_SPEED				23   //prime
// #define STAIR_HEIGHT			(2 * FALL_RATE) 		// [d3-16-97 JPC] vz must be
																// >= this before we look
																// up or down
#define LOOK_PITCH			(3 * MAX_PITCH / 4)	// [d3-16-97 JPC] fixed amount
																// to look up or down

#if defined (_EDIT)
// [d7-23-96 JPC] for extra-fine movement control.
// ("maxSpeed" is toggled in EditToggleSpeed.)
extern "C" int maxSpeed = 31 << PLAYER_FIXEDPT;
#undef  MAX_XY_SPEED						
#define MAX_XY_SPEED maxSpeed
#endif

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
extern LONG ssector_things[];


// whether to allow player movement
// whether to fall
// climb up
BOOL	fClimb = FALSE;
// exit

LONG	fExitLevel = 0;
extern "C" {
// for line to line wad teleports
// auto low res for fast movement
#if defined (_WINDOWS)
BOOL fAIAutoRes = TRUE;
#else
BOOL fAIAutoRes = FALSE;					// [d11-12-96 JPC] has no effect in DOS
#endif

#if !defined(_WINDOWS)
BOOL fAIMoving = FALSE;
#endif

BOOL	fFreeze = FALSE;
TELEPORT_DATA teleport_data;
LONG	PlayerLoresSpeed = 3;
LONG	PlayerHiresSpeed = 2;
PLAYER	player;
extern SHORT sMenusUp;
}

LONG	PlayerSpeed = PLAYER_SPEED;
static LONG	PlayerWidth = PLAYER_WIDTH;
static LONG	PlayerHeight = PLAYER_HEIGHT;
LONG	PlayerJumpVelocity = JUMP_VELOCITY;
LONG	PlayerFallRate = FALL_RATE;
LONG	PlayerTermVelocity = TERMINAL_VELOCITY;

// current player movement keys
static int	keyLeft;
static int	keyRight;
static int	keyForward;
static int	keyBackward;
static int	keyUp;
static int	keyDown;
static int	keyLookUp;
static int	keyLookDown;
static int	keyPitchUp;
static int	keyPitchDown;
static int	keySlideLeft;
static int	keySlideRight;
static int	keyWalkThruWall;
static int	keyRun;
static int	keyRun2;
static int	keyAutoCenter;

BOOL fJumpPressed = FALSE;
// Player velocity movement.
static LONG		vx=0, vy=0, vz=0, va=0, vp=0, vh=0;

// where to move the player to
LONG	TargetFactor, TargetFactor2;
LONG	TargetH, TargetA, TargetP, TargetX, TargetY;
PLAYER	Target;

/* ========================================================================
   Function    - InitPlayer
   Description - inits a player to a location and sets his height to default
   Returns     - void
   ======================================================================== */
extern "C" {
void InitPlayer (LONG x, LONG y, LONG a)
{
	LONG	sector, floor, ceiling, special, tag;

	player.x=x<<PLAYER_FIXEDPT;
	player.y=y<<PLAYER_FIXEDPT;
	point_to_sector_info( x, y, &sector, &floor, &ceiling, &special, &tag);
	player.z=floor;
	player.a=a;									// angle in "byteans"
	player.p=0;
	player.w=PlayerWidth;
	player.h=PlayerHeight;
	player.Flying = FALSE;
	player.Crouching = FALSE;
	player.WalkThruWall = FALSE;
	// [d9-04-96 JPC] Adjust z for water, acid, and lava.
	if (special == SSP_WATER || special == SSP_ACID_FLOOR || special == SSP_LAVA)
		player.z -= player.h / 4;
	else if (special == SSP_DEEP_WATER)
		player.z -= (player.h * 3) / 4;

	player.ThingIndex = fERROR;
	player.bump = iNOTHING;
	player.BumpIndex = fERROR;
	player.fallHeight = 0;					// [d11-22-96 JPC]
	player.startFallZ = player.z;			// [d4-25-97 JPC]

	// Reset these values so that whenever the options data
	keyLeft			= GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_MOVE_LEFT);
	keyRight		= GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_MOVE_RIGHT);
	keyForward	 	= GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_MOVE_FORWARD);
	keyBackward		= GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_MOVE_BACKWARD);
	keyUp			= GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_MOVE_UP);
	keyDown			= GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_MOVE_DOWN);
	keyLookUp 		= GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_LOOK_UP);
	keyLookDown		= GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_LOOK_DOWN);
	keyPitchUp 		= GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_PITCH_UP);
	keyPitchDown	= GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_PITCH_DOWN);
	keySlideLeft	= GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_SLIDE_LEFT);
	keySlideRight	= GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_SLIDE_RIGHT);
	keyWalkThruWall	= GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_WALKTHRUWALL);
	keyRun			= GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_MOVE_RUN);
	keyRun2			= GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_MOVE_RUN2);
	keyAutoCenter	= GAME_KEYS::mfGetKey(GAME_KEYS::PLAYER_AUTOCENTER);
}
}

/* ========================================================================
   Function    - SetPlayer
   Description - sets the player to a location, and calculates altitude
   				 Note: Does Automatic crouching for teleports. This trys
   				       to keep the player's head out of the ceiling.
   Returns     - void
	Note        - NO_CHANGE is defined as LONG_MAX as of 8-12-96.
	======================================================================== */
extern "C" {
void SetPlayer (LONG x, LONG y, LONG z, LONG a, LONG p)
{
	LONG	sector, realfloor, effectivefloor, ceiling, special, tag;
	
	if(x != NO_CHANGE)
		player.x=x<<PLAYER_FIXEDPT;
	if(y != NO_CHANGE)
		player.y=y<<PLAYER_FIXEDPT;
		
	point_to_sector_info( x, y, &sector, &realfloor, &ceiling, &special, &tag);
	effectivefloor = realfloor;
	
	// For special's drop the effective floor height.
	if (special == SSP_WATER || special == SSP_ACID_FLOOR || special == SSP_LAVA)
		effectivefloor -= player.h / 4;
	else if (special == SSP_DEEP_WATER)
		effectivefloor -= (player.h * 3) / 4;
			
	if(z != NO_CHANGE)
	{
		if (player.Flying == TRUE)
		{
			if ( (z + player.h) <= ceiling)	// can we fit at this new fly height?
			{
				player.z = z;
			}
			else
			if ((ceiling - player.h) > effectivefloor)
			{
				player.z = ceiling - player.h;
			}
			else	// We don't fit.
			{
				if (player.Crouching == FALSE)
				{
					player.Crouching = TRUE;
					player.h /= 2;
	
					if (special == SSP_WATER || special == SSP_ACID_FLOOR || special == SSP_LAVA)
						effectivefloor = realfloor - (player.h / 4);
					else if (special == SSP_DEEP_WATER)
						effectivefloor = realfloor - ((player.h * 3) / 4);
				}
				
				player.z = effectivefloor;
			}
		}
		else
		{
			if (player.Crouching == FALSE &&
			    (player.h + effectivefloor) > ceiling)
			{
				player.Crouching = TRUE;
				player.h /= 2;

				if (special == SSP_WATER || special == SSP_ACID_FLOOR || special == SSP_LAVA)
					effectivefloor = realfloor - (player.h / 4);
				else if (special == SSP_DEEP_WATER)
					effectivefloor = realfloor - ((player.h * 3) / 4);
			}
			
			player.z=effectivefloor;
		}
	}
	else
	{
		if (player.Flying == TRUE)
		{
			// This is the flying height over the old floor.
			LONG FlyingHeight = player.z - player.floor;
			
			if ( (FlyingHeight + effectivefloor + player.h) <= ceiling)	// can we fit at this new fly height?
			{
				player.z = effectivefloor + FlyingHeight;
			}
			else
			if ( (ceiling - player.h) >= effectivefloor)
			{
				player.z = ceiling - player.h;
			}
			else	// we don't fit.
			{
				if (player.Crouching == FALSE)
				{
					player.Crouching = TRUE;
					player.h /= 2;
	
					if (special == SSP_WATER || special == SSP_ACID_FLOOR || special == SSP_LAVA)
						effectivefloor = realfloor - (player.h / 4);
					else if (special == SSP_DEEP_WATER)
						effectivefloor = realfloor - ((player.h * 3) / 4);
				}
				
				player.z = effectivefloor;	// stick his head in the ceiling.
			}
		}
		else
		{
			if (player.Crouching == FALSE &&
			    (player.h + effectivefloor) > ceiling)
			{
				player.Crouching = TRUE;
				player.h /= 2;
	
				if (special == SSP_WATER || special == SSP_ACID_FLOOR || special == SSP_LAVA)
					effectivefloor = realfloor - (player.h / 4);
				else if (special == SSP_DEEP_WATER)
					effectivefloor = realfloor - ((player.h * 3) / 4);
			}
			player.z=effectivefloor;
		}
	}
	
#if 0
	// GWP [11-29-96] Happens too late to correctly do auto crouching.
	// [d9-03-96 JPC] Allow for water, acid, and lava.
	if (!player.Flying)
	{
		if (special == SSP_WATER || special == SSP_ACID_FLOOR || special == SSP_LAVA)
			player.z -= player.h / 4;
		else if (special == SSP_DEEP_WATER)
			player.z -= (player.h * 3) / 4;
	}
#endif

	if(a != NO_CHANGE)
		player.a= a & 0xFF;
	if(p != NO_CHANGE)
		player.p=p;
	
	player.floor = realfloor;
	player.ceiling = ceiling;
	player.currentSector = sector;
}
}
/* ========================================================================
   Function    - TeleportPlayer
   Description - teleports player to a sector
   Returns     - void
   ======================================================================== */
extern "C" {
void TeleportPlayer (ULONG sect)
{
	ULONG i=0;

//	printf("scanning for exit\n");

	for (i=0;i<tot_things;i++)
	{
//		printf("%li: type: %li sector: %li ssector: %li x:%li y:%li\n",i,mythings[i].type,mythings[i].sect,mythings[i].ssect,mythings[i].x,mythings[i].y);
		if (mythings[i].type==14 &&	mythings[i].sect == sect)
		{
			SetPlayer(
				mythings[i].x,
				mythings[i].y,
				NO_CHANGE,
				(65535-mythings[i].angle) & 0xFF,
				0
				);
			break;
		}
	}

	if (i==tot_things)
	{
//		printf("couldn't find exit\n");
	}

}
}
/* ========================================================================
   Function    - PlayerArrival
   Description - entry point for line to line wad teleport
   Returns     - void
	JPC Note		- if teleport x and y are both 0, this was a wad-to-wad
					  teleport, so just put the player in the middle of
					  the linedef.  (At the time we calculate teleport data,
					  we don't have information about the new wad.)
   ======================================================================== */
extern "C" {
void PlayerArrival (LONG * pPlayerStart)
{
	LONG	j;
	LONG	x1;									// [d9-24-96 JPC]
	LONG	y1;									// [d9-24-96 JPC]

	if (*pPlayerStart > 0)
	{
		j = tag_to_line(*pPlayerStart, 99999);		// notline is dummy

		if (teleport_data.dx == 0 && teleport_data.dy == 0)
		{
			// [d9-24-96 JPC] wad-to-wad teleport; put player in the
			// middle of the line.  Fixes bug where player was stuck
			// in the wall after a wad-to-wad teleport.
			x1 = (vertexs[linedefs[j].a].x + vertexs[linedefs[j].b].x) / 2;
			y1 = (vertexs[linedefs[j].a].y + vertexs[linedefs[j].b].y) / 2;
		}
		else
		{
			// Original values.
			x1 = vertexs[linedefs[j].a].x+teleport_data.dx;
			y1 = vertexs[linedefs[j].a].y+teleport_data.dy;
		}
		SetPlayer(x1, y1, NO_CHANGE, teleport_data.a, NO_CHANGE);
	}
}
}

/* ========================================================================
   Function    - Check_Key
   Description - Checks status of movement keys
   Returns     - key pressed
   ======================================================================== */
LONG CheckMoveKeys(void)
{
	LONG	KeyState = 0;

	if (async_key_status(keyLeft) )
		KeyState |= KEY_LEFT_BIT;
	if (async_key_status(keyRight) )
		KeyState |= KEY_RIGHT_BIT;
	if (async_key_status(keyForward) )
		KeyState |= KEY_FORWARD_BIT;
	if (async_key_status(keyBackward) )
		KeyState |= KEY_BACKWARD_BIT;
	if (async_key_status(keyUp) )
		KeyState |= KEY_UP_BIT;
	if (async_key_status(keyDown) )
		KeyState |= KEY_DOWN_BIT;
	if (async_key_status(keyLookUp) )
		KeyState |= KEY_LOOKUP_BIT;
	if (async_key_status(keyLookDown) )
		KeyState |= KEY_LOOKDOWN_BIT;
	if (async_key_status(keyPitchUp) )
		KeyState |= KEY_PITCHUP_BIT;
	if (async_key_status(keyPitchDown) )
		KeyState |= KEY_PITCHDOWN_BIT;
	if (async_key_status(keySlideLeft) )
		KeyState |= KEY_SLIDELEFT_BIT;
	if (async_key_status(keySlideRight) )
		KeyState |= KEY_SLIDERIGHT_BIT;
// GEH removed from the release game
#ifdef _DEBUG
	if (async_key_status(keyWalkThruWall) )
		KeyState |= KEY_WALKTHRUWALL_BIT;
#else
#if defined (_WADBUILDERS)
	// [d4-07-97 JPC] Allow walk through wall in WAD Builders' version
	// even if not debug.
	if (async_key_status(keyWalkThruWall) )	// WAD builders
		KeyState |= KEY_WALKTHRUWALL_BIT;		// WAD builders
#endif
#endif
	if (async_key_status(keyRun) )
		KeyState |= KEY_RUN_BIT;
	if (async_key_status(keyRun2) )
		KeyState |= KEY_RUN_BIT;
	if (async_key_status(keyAutoCenter) )
		KeyState |= KEY_AUTOCENTER_BIT;
	
	return KeyState;
}

#if 0
// GWP this fn doesn't do what it says. Fix it if you need it.
/* ========================================================================
   Function    - SwoopToTarget
   Description - move along a swoop in path to a new target location
   Returns     - arrived at new location
   ======================================================================== */
BOOL SwoopToTarget ( POSITION *pStart, POSITION *pTarget)
{
	LONG	th, ta, tp, tx, ty;
	
	th = ta = tp = tx = ty = 0;
	
	tx = TargetX - player.x;
	ty = TargetY - player.y;
	th = TargetH - player.h;
	tp = TargetP - player.p;
	ta = TargetA - player.a;
	
	if (ta>128)
		ta=ta-256;
	else
	if (ta<-128)
		ta=ta+256;
	
	TargetFactor2 = TargetFactor - 1;
	player.h += ((th+((th>0)?TargetFactor2:-TargetFactor2)) / TargetFactor);
	player.p += ((tp+((tp>0)?TargetFactor2:-TargetFactor2)) / TargetFactor);
 	 	
	player.x += ((tx+((tx>0)?TargetFactor2:-TargetFactor2)) / TargetFactor);
	player.y += ((ty+((ty>0)?TargetFactor2:-TargetFactor2)) / TargetFactor);
	player.a += ((ta+((ta>0)?TargetFactor2:-TargetFactor2)) / TargetFactor);
	
	TargetFactor = ((TargetFactor - 5) / 2) + 5;

	return TRUE;
}
#endif // 0

/* ========================================================================
   Function    - StopPlayer
   Description - Stop moving the player. (For returning from combat mode.)
   				 Clear the velocity vectors.
   Returns     -
   ======================================================================== */

void StopPlayer()
{
	vx=vy=vz=va=vp=vh=0;
}

#if defined (_DEBUG) && 0
void MonoDump (const char *format, ... )
{
	static short gxMono, gyMono;
	char  strbuf [ 256 ];
   va_list argptr;

	if (gyMono == 0)
		mono_printf (gxMono, 24, "   ");
	else
		mono_printf (gxMono, gyMono - 1, "   ");
	mono_printf (gxMono, gyMono, "-->                                                                             ");

	va_start(argptr,format);
   vsprintf(strbuf,format,argptr);
   va_end(argptr);
	mono_printf (gxMono + 4, gyMono, strbuf);
	gyMono++;
	if (gyMono >= 25)
		gyMono = 0;
}
#else
// [d3-15-97 JPC] The following outlandish expression resolves to nothing
// in the release code but does not generate a compiler warning error.
// So the MonoDump calls can be left in the source or not, as desired.
#define MonoDump  1 ? (void) 0 : (void) printf             // make it nothing
#endif

/* ========================================================================
   Function    - MovePlayer
   Description - moves player according to keyboard input
                 KeyState is returned from CheckMoveKeys
                 NOTE: this routine does use 24.8 fixed point x and y
   Returns     - returns move codes (caller MainLoop ignores the return code)
   ======================================================================== */
LONG MovePlayer( LONG KeyState )
{
	LONG			RetVal = 1;
	FIXED_VECTOR_3D	tp;
	LONG			force_x, force_y, force_z, force_a, force_p;
	LONG			temp;
	LONG			sector, floor,ceiling, special, tag;
	LONG 			waterOffset;				// [d10-21-96 JPC] for water sector jump
	LONG 			actualFloor;				// [d10-21-96 JPC] for water sector jump
	BOOL			localClimb;
#if defined(_STATUS) && defined(_DEBUG)
   char        szTemp[80];
#endif
	static BOOL	fJumping = FALSE;
	static int cLockPitch;					// [d3-14-97 JPC] how long to hold current
													// pitch before auto-leveling

	force_x = force_y = force_z = force_a = force_p = 0;
	
	player.bump = iNOTHING;
	if (KeyState & KEY_WALKTHRUWALL_BIT)
	{
		player.WalkThruWall = TRUE;
	}
	else
	{
		player.WalkThruWall = FALSE;
	}
	/* -----------------------------------------------------------------
	   check for environment changes
	   ----------------------------------------------------------------- */

	//GEH setup freeze
	if (fFreeze==TRUE)
		return RetVal;
		
	//GEH if I'm confused, auto center me
	if (KeyState & KEY_AUTOCENTER_BIT)
	{
		vx=vy=vz=va=vp=vh=0;
	}
	
#if 0
	// GWP There is no "player" target only cameras.
	/* -----------------------------------------------------------------
	   check for target movement
	   ----------------------------------------------------------------- */
	if (KeyState & KEY_SWOOP_TO_TARGET)
	{
		// if not at target, return
		if(!SwoopToTarget((POSITION *)&player, (POSITION *)&Target))
			return RetVal;
	}
#endif

	// get any needed information
	point_to_sector_info(
		(player.x>>PLAYER_FIXEDPT),
		(player.y>>PLAYER_FIXEDPT),
		&sector, &floor, &ceiling, &special, &tag
		);

	player.currentSector = sector;		// [d11-14-96 JPC] remember our sector

	// [d10-21-96 JPC] Water, acid, and lava sector calculations.
	if (special == SSP_WATER || special == SSP_ACID_FLOOR || special == SSP_LAVA)
		waterOffset = player.h / 4;
	else if (special == SSP_DEEP_WATER)
		waterOffset = (player.h * 3) / 4;
	else
		waterOffset = 0;

	actualFloor = floor - waterOffset;
		
	//GEH check to see if this sector allows climbing (ladder sector).
	if (fClimb || special == SSP_CLIMB_OK)
		localClimb = TRUE;
	else
		localClimb = FALSE;
	
	/*
	 * Handle running twisting and looking movement
	 */
	/* rotate left/right */
	if (KeyState & KEY_LEFT_BIT)
	{
		force_a -= MAX_A_SPEED;
	}
	if (KeyState & KEY_RIGHT_BIT)
	{
		force_a += MAX_A_SPEED;
	}

	if (fJumping == FALSE || player.WalkThruWall == TRUE)
	{
		/* forward/backward */
		if (KeyState & KEY_FORWARD_BIT )
		{
			force_y += MAX_XY_SPEED;
		}
		if (KeyState & KEY_BACKWARD_BIT)
		{
			force_y -= MAX_XY_SPEED;
		}

		/* slide left/right */
		if (KeyState & KEY_SLIDELEFT_BIT)
		{
			force_x -= MAX_XY_SPEED;
		}
		if (KeyState & KEY_SLIDERIGHT_BIT)
		{
			force_x += MAX_XY_SPEED;
		}
		
		/* pitch */
		if (KeyState & KEY_PITCHUP_BIT)
		{
			force_p += MAX_P_SPEED;
		}
		if (KeyState & KEY_PITCHDOWN_BIT)
		{
			force_p -= MAX_P_SPEED;
		}
		
	}

#if !defined (_WADBUILDERS)
	// [d1-10-97 JPC] Beta testers demanded modifications to the levitate
	// spell.  In AD&D you aren't supposed to be able to move while levitating;
	// our compromise is to let you move very slowly in the x and y directions.
	// [d1-15-97 JPC] Added floor test; slow movement only applies if player
	// is actually levitating, i.e., above the floor.
	// [d4-07-97 JPC] Don't do this for WAD Builders' version.
   if (fClimb && player.z > floor)     // the global fClimb is TRUE if we're levitating
	{
		force_x /= 6;
		force_y /= 6;
		// Leave force_a alone; dividing it by 6 keeps you from turning at all.
	}
#endif

	/* Look */
	if (KeyState & KEY_LOOKUP_BIT)
	{
		if (player.p < MAX_PITCH)
			player.p += MAX_P_SPEED;
	}
	else if (KeyState & KEY_LOOKDOWN_BIT)
	{
		if (player.p > -MAX_PITCH)
			player.p -= MAX_P_SPEED;
	}
	// else
	else if (vz == 0 && player.p != 0)
	{
		// player.p = (player.p < -(MAX_P_SPEED+1))
		// 					? player.p + MAX_P_SPEED
		// 					: (player.p > (MAX_P_SPEED))
		// 						? player.p - MAX_P_SPEED
		// 						: 0;

		// [d3-13-97 JPC] Reworked the above.
		// This is the code that moves us back to 0 after looking up or down.
		if (cLockPitch > 0)
		{
			// [d3-14-97 JPC] New: lock in the current pitch for a certain
			// number of frames to prevent bobbling.
			cLockPitch--;
			MonoDump ("Auto=change player.p prevented by cLockPitch (%d)", cLockPitch);
		}
		else
		{
			if (player.p < -(MAX_P_SPEED+1))
			{
				player.p += MAX_P_SPEED;
				if (player.p > 0)					// prevent bobbling
					player.p = 0;
			}
			else
			{
				if (player.p > (MAX_P_SPEED))
				{
					player.p -= MAX_P_SPEED;
					if (player.p < 0)				// prevent bobbling
						player.p = 0;
				}
				else
				{
					player.p = 0;
				}
			}
			// MonoDump ("Auto-changed player.p from %d to %d, vz = %d", oldP, player.p, vz);
		}
	}
	
	/*
	 *	Handle up and down movement
	 * [d12-03-96 JPC] Change order of checking player state; used to be
	 * first check flying, then local climb. I switched them so that if
	 * you are levitating, you can go up or down.
	 */
	if (localClimb &&
		!((KeyState & KEY_DOWN_BIT) && player.z <= actualFloor) &&
		!player.Crouching)	
	{
		/* climb/levitate up */
		if (KeyState & KEY_UP_BIT)
		{
			vz = PlayerJumpVelocity;
		}
		else
		/* climb/levitate down */
		if (KeyState & KEY_DOWN_BIT)
		{
			vz = -PlayerJumpVelocity;
		}
		else
			vz = 0;
	}
	else
	if (player.Flying &&
		!((KeyState & KEY_DOWN_BIT) && player.z <= actualFloor) &&
		!player.Crouching)	
	{
		// [d12-03-96 JPC] Added the "&&" so flyers can crouch while on the
		// ground (goes to the following else clause, which handles crouching).
		/* fly up */
		if (KeyState & KEY_UP_BIT)
		{
			// [MDB] if flying adjust velocity by half
			force_z += PlayerJumpVelocity/2;
		}
	}
	else
	{
		/* Jump */
		if (KeyState & KEY_UP_BIT)
		{
			if(KeyState & KEY_RUN_BIT)
				force_z = PlayerJumpVelocity-((PLAYER_HEIGHT-player.h)/5);	// OK, they get to jump
	   		else
				force_z = PlayerJumpVelocity-((PLAYER_HEIGHT-player.h)/5)-5;
			fJumping = TRUE;
		}
		else
		{
			force_z = 0;
		}
			
		/* crouch (autocenters) */
		if (KeyState & KEY_DOWN_BIT)
		{
			if(!player.Crouching)
			{
				player.Crouching = TRUE;
				player.h /= 2;
			}
		}
		else
		{
			if(player.Crouching)
			{
				LONG Space = ceiling - floor;
				Space = ABS(Space);
				
				// Can we stand up?
				if ((2 * player.h) < Space)
				{
					player.Crouching = FALSE;
					player.h *= 2;
				}
			}
		}
	}
	
	/* -----------------------------------------------------------------
	   Calculate movement
	   ----------------------------------------------------------------- */

	//GEH look in the direction you are jumping/falling
	// [d3-14-97 JPC] Per Bob's request, no longer change pitch when jumping.
	// Solves various problems.
	// I tried using a counter so that we would tilt only after the counter
	// reached 3 or 4, but that kept us from looking down when going downstairs
	// because vz kept dropping to 0 when we reached a step, and then the
	// counter got reset.
	if (!player.Flying && !fJumping)
	{
		// if (ABS (vz) >= STAIR_HEIGHT)
		if (vz != 0)
		{
#if 0
			player.p = player.p + (LONG)(vz*4);
			if (player.p < -MAX_PITCH)
				player.p = -MAX_PITCH;
			else if (player.p > MAX_PITCH)
				player.p = MAX_PITCH;
#endif
			player.p = (vz < 0) ? -LOOK_PITCH : LOOK_PITCH;
			cLockPitch = 2;					// do 2 frames without auto-leveling
		}
	}
	
	// if our feet can move us in a new direction
	// [d8-12-96 JPC] Change "player.z == floor" to "player.z <= floor" to fix bug
	// where you keep moving forward when you get on a floor-goes-up-to-ceiling
	// crusher.  In that case, your feet are temporarily UNDER the floor until
	// corrected below.  In general, it makes sense that your vx and vy can be
	// affected when you are on or under the floor.  The intent of the following
	// test (I think) was to see whether you are ABOVE the floor.
	// LATER: add localClimb check as well.  I seem to recall complaints that
	// you couldn't move when climbing.  This should fix that.
	if ((!(fJumping&&player.z>actualFloor))|| player.Flying || localClimb)
	{
		if (ABS(vx) > ABS(force_x))
		{
			// decel in the x
			temp = (vx - force_x) / 3;
			if(temp == 0)
				vx = 0;
			else
				vx -= temp;
		}
		else
		if (ABS(vx) < ABS(force_x))
		{
			// accel in the x
			vx += (force_x - vx) / 3;
			if (ABS(vx) > ABS(force_x))
				vx = force_x;
		}
	
		if (ABS(vy) > ABS(force_y))
		{
			// decel in the y
			temp = (vy - force_y) / 3;
			if(temp == 0)
				vy = 0;
			else
				vy -= temp;
		}
		else
		if (ABS(vy) < ABS(force_y))
		{
			// accel in the y
			vy += (force_y - vy) / 3;
			if (ABS(vy) > ABS(force_y))
				vy = force_y;
		}
		
		vz += force_z;
	}
	//else
	//{
	//	printf("Airborn, no forced movement\n");
	//}
	
	// you can always rotate, even when in the air
	if (ABS(va) > ABS(force_a))
	{
		// decel in the a
		temp = (va - force_a) / 2;
		if(temp == 0)
			va = 0;
		else
			va -= temp;
	}
	else
	if (ABS(va) < ABS(force_a))
	{
		// accel in the a
		va += (force_a - va) / 5;
		if (ABS(va) > ABS(force_a))
			va = force_a;
	}
	player.a += va;
	player.a = (player.a%256);
	if (player.a<0)
		player.a+=256;
	
	/* -----------------------------------------------------------------
	   find my height above the floor
	   ----------------------------------------------------------------- */

	// get floor height where we want to go
	point_to_sector_info(
		(player.x>>PLAYER_FIXEDPT), (player.y>>PLAYER_FIXEDPT),
		&sector, &floor, &ceiling, &special, &tag );
		
	// If I am below the floor, raise up to floor level.
	// Special handling required for water, acid, and lava sectors.
	if (special != SSP_WATER && special != SSP_ACID_FLOOR && special != SSP_LAVA && special != SSP_DEEP_WATER)
	{
		// Normal (not water, acid, or lava) sectors.
		if (player.z < floor)
		{
			// Change the pitch for looking up and down stairs.
			// [d3-14-97 JPC] Won't this only affect looking up stairs?
			// We only get here if player.z is less than the target floor.
			// Comment it all out for now to isolate the falling case.

			// [d3-15-97 JPC] Do not look up when going up. This stops us
			// from looking up when going up stairs, but solves many other
			// bugs with looking up when we aren't supposed to.
// #if 0
// 			player.p += (LONG)((floor - player.z) * 4); // [d8-12-96 JPC] experiment--try to look up
// 			if (player.p < -MAX_PITCH)
// 				player.p = -MAX_PITCH;
// 			else if (player.p > MAX_PITCH)
// 				player.p = MAX_PITCH;
// #else
// 			// If we jump, we can end up below the floor when we come down.
// 			// That is corrected here, but we do NOT want to change the
// 			// camera pitch in that case!
// 			if (!fJumping)
// 			{
// 				player.p = MAX_PITCH;
//   				cLockPitch = 2;
// 			}
// 			MonoDump ("Up stairs: changed player.p from %d to %d, vz = %d", oldP, player.p, vz);
// 			MonoDump ("  floor = %d, player.z = %d", floor, player.z);
// #endif
			player.z = floor;
			vz = 0;
			fJumping = FALSE;
		}
		else
		{
			player.z += vz;		// position player accordingly
			// [d3-15-97 JPC] Added the following to fix the bug where we
			// remain stuck in jump mode for a while after jump is done.
			if (player.z == floor)
				fJumping = FALSE;
		}
	}
	else
	{
		// Water, acid, and lava sectors.  Use previously calculated offset.
		// [d11-21-96 JPC] Restored test for flying.  Without this test,
		// player cannot fly over water, lava, or acid.
		if (!fJumping && !player.Flying && player.z > floor - waterOffset)
		{
			// Should be true only on initial entry into this sector.
			player.z = floor - waterOffset;
		}
		else if (player.z < floor - waterOffset)
		{
			player.z = floor - waterOffset;
			vz = 0;
			fJumping = FALSE;			
		}
		else
		{
         player.z += vz;               // position player accordingly
		}
	}
	
	if (ceiling < (player.z + player.h))
	{
		const LONG NewHeight = ceiling - player.h;
		if (NewHeight > floor)
		{
			player.z =  NewHeight;
		}
		else
		{
			player.z = floor;
			fJumping = FALSE;
		}
		vz = 0;
	}
	
	/* -----------------------------------------------------------------
	   calculate new point in the WAD
	   ----------------------------------------------------------------- */

	if (vx != 0 || vy != 0 || vz != 0)
	{
		tp.dx = vx;
		//tp.dy = -vy;
		tp.dy = vy;
		tp.dz = 0;	// GWP should use vz here.
	
		if (KeyState & KEY_RUN_BIT)
		{
			tp.dx*=2;
			tp.dy*=2;
		}
	
		// rotate current move vector to match player angle
		Rotate((POINT *)&tp, player.a);
	
		// check bump into an object
		if(player.WalkThruWall == FALSE )
		{
			const LONG Rate = aprox_dist(0, 0,
										 PLAYER_INT_VAL(tp.dx),
										 PLAYER_INT_VAL(tp.dy));
			CheckBump(&player, &tp, ABS(Rate)); // tp will be modified if we bump an object.
			switch(player.bump)
			{
			case iOBJECT:
				if (fJumping == TRUE)
				{
					fJumping = FALSE;
					if (player.Flying == FALSE)
					{
						vz = 0;
					}
				}
				break;
			}
		}
	
#ifdef _DEBUG
#ifdef _WINDOWS
//   if (player.WalkThruWall == TRUE)
//      WriteDebug ("Walk through wall is ON, not checking bumps");
#endif
#endif
	
		// check bump into a wall
		if(player.WalkThruWall == FALSE )
		{
			LONG Angle;
			LONG BumpDistance;
			
			CheckMove (&player, &tp, CHECKLINE_PLAYER, &Angle, &BumpDistance);
			
			switch(player.bump)
			{
			// [d1-27-97 JPC] Comment out the iSLIDE_ON_WALL case to permit
			// the bump code to try to keep us moving.
			//case iSLIDE_ON_WALL:
			case iCEILING:
			case iFLOOR:
			case iWALL:
			case iEDGE_OF_WORLD:
				// Even though we hit something we could still be falling from
				// gravity.
				tp.dx = 0;
				tp.dy = 0;
				tp.dz = 0;
				vx = vy = 0;
				break;
			}
	
		}
		
		player.x=player.x+tp.dx; /*Move looks OK!*/
		player.y=player.y+tp.dy;
		player.z=player.z+tp.dz;
	}

	/* Movement is OK, fall through */

	/* -----------------------------------------------------------------
	   calculate falling
	   ----------------------------------------------------------------- */

	// [RCC] If walk through walls key is pressed, allow any size step up
	if(player.WalkThruWall == TRUE)
		player.z = floor;

	if (!localClimb)
	{
      if (player.z > actualFloor)      // [RCC] Currently above the floor?
		{
			// [d1-10-97 JPC] If flying, fall at 1/2 rate.  (Strongly
			// requested by beta testers).
			if (player.Flying)
			{
				LONG	fallRate;				// [d3-08-97 JPC] Make sure fall
													// rate is at least 1 (feather fall
													// reduces it to 1, and dividing
													// by 2 reduces it to 0).
				fallRate = PlayerFallRate/2;
				if (fallRate < 1)
					fallRate = 1;
            vz -= fallRate;            // We're going to fall now
			}
			else
			{
				vz -= PlayerFallRate;			// We're going to fall now

				// The following is to keep us from being embedded in
				// the floor after jumping.
				if (waterOffset == 0 && player.z + vz < actualFloor)
					vz = actualFloor - player.z;
				MonoDump ("vz = %d, player.z (%d) > actualFloor (%d)",
					vz, player.z, actualFloor);
			}
			if (vz >= PlayerTermVelocity)
				vz = PlayerTermVelocity; 	// Never fall faster than terminal
			// if (player.z - vz >= floor)		// Are they going to fall thru the floor?
			//	vz = player.z - floor;		// If so, stop at the floor
		}
		else								// we're not above floor level
		{
			if (vz > (PlayerTermVelocity/2))
				localClimb = FALSE;
			else
			if (vz > (PlayerTermVelocity/4))
				localClimb=FALSE;
			
			if (!fJumping && vz != 0)
			{
				vz = 0;			// so, don't fall anymore
				MonoDump ("vz = %d, not above floor, not jumping", vz);
			}
		}
	}
	
	//GEH speed up frame rate during movement
	// GWP When Auto res works again, uncomment this.
// [d10-26-96 JPC] Restore auto-res for Windows.
//#if defined (_WINDOWS)
// [d11-12-96 JPC] Only do autores in Windows; it's no help in DOS and
// causes more bugs.
	if( fAIAutoRes)
	{
		if( force_x || force_y || force_z || force_a || force_p ||
		   (fJumping && (vx || vy || vz || va || vp))
		   )
		{
#if defined(_WINDOWS)
			set_lowres(0L, 0L);
#else
			fAIMoving = TRUE;
#endif
		}
		else
		{
#if defined(_WINDOWS)
			set_hires(0L, 0L);
#else
			fAIMoving = FALSE;
#endif
		}
	}
//#endif

#if 01
// [d12-04-96 JPC] Can also do this for the CAMERA instead.
#ifdef _DEBUG
#ifdef _WINDOWS
#ifdef _STATUS
// [d5-07-96  9:11 JPC]
// Note that the status window and its child windows (ghwndSBx, ghwndSBy,
// etc.), must be set up for the following to work.
// The status window and child windows are set up in MACHINE.C: AppInit,
// and the support code for the status window is in STATUS.C.
//GEH commented out for now, extern the ghwnd* later
   wsprintf (szTemp, "X %d", player.x >> PLAYER_FIXEDPT);
   SetWindowText (ghwndSBx, szTemp);
   wsprintf (szTemp, "Y %d", player.y >> PLAYER_FIXEDPT);
   SetWindowText (ghwndSBy, szTemp);
   wsprintf (szTemp, "Z %d", player.z);
   SetWindowText (ghwndSBz, szTemp);
   wsprintf (szTemp, "A %d", player.a);
   SetWindowText (ghwndSBa, szTemp);
   wsprintf (szTemp, "P %d", player.p);
   SetWindowText (ghwndSBp, szTemp);
   wsprintf (szTemp, "vz %d", vz);
   SetWindowText (ghwndSBr, szTemp);
   wsprintf (szTemp, "fJ %d", fJumping);
   SetWindowText (ghwndSBs, szTemp);
#endif
#endif
#endif
#endif

	return 1;
}

/* ========================================================================
   Function    - SetPlayerTarget
   Description - Set the player target position
   Returns     - void
   ======================================================================== */
void SetPlayerTarget( LONG	X, LONG	Y, LONG	H, LONG	A, LONG	P )
{
	TargetX=X<<PLAYER_FIXEDPT;	// x and y use 24.8 fixed point
	TargetY=Y<<PLAYER_FIXEDPT;
	TargetH=H;
	TargetP=P;
	TargetA=A;
	
	TargetFactor = 105;
}

/* ========================================================================
   Function    - SetPlayerSpeed
   Description - Adjust the player speed values
   Returns     - old speed
   ======================================================================== */
LONG SetPlayerSpeed(LONG Speed)
{
	LONG oldSpeed = PlayerSpeed;
	PlayerSpeed = Speed;
	PlayerHiresSpeed = PlayerSpeed;
	PlayerLoresSpeed = PlayerSpeed * 3 / 2;
	return oldSpeed;
}

/* ========================================================================
   Function    - SetPlayerSize
   Description - Set the player size in units
   Returns     - 1
   ======================================================================== */
LONG SetPlayerSize(LONG Width, LONG Height)
{
	player.w = Width;
	player.h = Height;
	return 1;
}

/* ========================================================================
   Function    - SetPlayerJump
   Description - Set the player jump rates
   Returns     - 1
   ======================================================================== */
LONG SetPlayerJump(LONG RiseVelocity, LONG FallRate, LONG TerminalVelocity)
{
	PlayerJumpVelocity = RiseVelocity;
	PlayerFallRate = FallRate;
	PlayerTermVelocity = TerminalVelocity;
	return 1;
}

/* ========================================================================
   Function    - PlayerFreeze
   Description - set the freeze flag which halts movement and falling
   Returns     -
   ======================================================================== */
void PlayerFreeze(LONG NewFlag)
{
	if(NewFlag == -1)
		fFreeze = !fFreeze;
	else
		fFreeze = NewFlag;
}

/* ========================================================================
   Function    - PlayerFlying
   Description - set the flying flag to allow players to defy gravity
   Returns     -
   ======================================================================== */
void PlayerFlying(LONG, LONG NewFlag)
{
	if(NewFlag == -1)
		player.Flying = !player.Flying;
	else
		player.Flying = NewFlag;
}

/* ========================================================================
   Function    - PlayerClimbing
   Description - set the climb flag to allow players to defy gravity
   Returns     -
   ======================================================================== */
void PlayerClimbing(LONG, LONG NewFlag)
{
	if(NewFlag == -1)
		fClimb = !fClimb;
	else
		fClimb = NewFlag;
}

/*	======================================================================== */

/* ========================================================================
   Function    - ConvertScreenToFloor
   Description - convert screen point on floor to world coord
   Returns     - void
   ======================================================================== */
void ConvertScreenToFloor(LONG screen_x, LONG screen_y, LONG *sx, LONG *sy)
{
	LONG	cz, tx, ty, wx, wy;
	POINT	tp;
	
	/* get map x,y from screen x,y */
	ty = screen_y - render_center_y;			/* translate center to 0,0 */
	if (ty == 0)								/* prevent divide by zero */
		ty = 1;
	tx = screen_x - render_center_x;
	//cz = point_to_floor_height(CAMERA_INT_VAL(camera.x),
	//						   CAMERA_INT_VAL(camera.y))+camera.h;
	cz = camera.z;
	wy = (cz * render_perspect * 256) / ty;
	wx = (tx * wy) / (render_perspect-1);
	wx >>= 8;
	wy >>= 8;
	
	tp.x = wx;
	tp.y = -(wy);
	rot8dbl(&tp);
	*sx = CAMERA_INT_VAL(camera.x) + tp.x;
	*sy = CAMERA_INT_VAL(camera.y) - tp.y;
}

