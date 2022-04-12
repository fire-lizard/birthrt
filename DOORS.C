/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: DOORS.C     -Handles Doors AND Lifts AND Crushers, etc.
   Authors: Chris Phillips, John Conley
   ========================================================================
   Contains the following internal functions:
   door_moving           -finds whether or not a door is moving

   Contains the following general functions:
   init_doors            -wipes the door array
   stop_door             -stops a door from moving
   handle_doors          -updates doors
   open_door             -starts a door moving up

	Doors can be activated by:
		-  getting close to them and pressing the spacebar
			-- see SECTORS.C: activate_seg
		-	crossing a line
			-- see BUMP.C: CheckLinesInBlock

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <string.h>
#if defined (_EDIT) || (defined (_WINDOWS) && defined (_DEBUG))
#include <windows.h>
#endif
#if defined (_JPC)
#include "debug.h"
#endif
#include "SYSTEM.H"
#include "ENGINE.H"
#include "ENGINT.H"
#include "MACHINE.H"
#include "SPECIAL.H"
#include "THINGTYP.H"
#include "SOUND.HXX"
#include "SOUNDFX.HXX"
#include "STRMGR.H"
#include "strenum.h"

extern BOOL fSpellKnocking;

extern BOOL DoesPlayerHaveItem(THINGTYPE t);	// in INVNTORY.CPP

typedef struct
{
	LONG	sector;
	LONG  linedef;					// [d7-24-96 JPC] new field
	LONG	state;					// [d7-24-96 JPC] formerly direction
	LONG	goal;						// [d7-24-96 JPC] formerly "desth"
	LONG	change;					// [d7-24-96 JPC] new field
	LONG	speed;
	LONG	special;					// [d7-24-96 JPC] new field
	LONG	iDummyThing;			// [d10-24-96 JPC] for sound
	LONG	iSound;					// [d9-24-96 JPC] new field
	BOOL	fInUse;					// [d7-24-96 JPC] formerly "LONG valid"
	SHORT	hingeVertex;			// [d7-24-96 JPC] new field
	SHORT	movingVertexes[3];   // [d7-24-96 JPC] new field
	LONG	xOriginal[3];
	LONG	yOriginal[3];
} DOOR;

typedef struct
{
	LONG	sector;
	LONG	state;
	LONG	current;
	LONG	goal;
	LONG	speed;
	LONG	special;
	LONG	heightOrig;
	LONG	iDummyThing;			// [d10-24-96 JPC] for sound
   LONG  iSound;              // [d9-24-96 JPC] new field
	short	cFramesWait;
	BOOL	fInUse;
} LIFT;								// for lifts, floors, and ceilings

// List of strings in string manager (STRDAT.DAT).
int giString[] = {
	STR_DOOR_KEY_SILVER,
	STR_DOOR_KEY_TINY,
	STR_DOOR_KEY_RED,
	STR_DOOR_KEY_BLUE,
	STR_DOOR_KEY_JEWELED,
	STR_DOOR_KEY_WHITE,
	STR_DOOR_KEY_SKELETON,
	STR_DOOR_KEY_PURPLE,						// [d1-07-97 JPC] now the "black key"
	STR_DOOR_KEY_BONE,
	STR_DOOR_KEY_STONE,
	STR_DOOR_GEM_BLUE,
	STR_DOOR_GEM_GREEN,
	STR_DOOR_GEM_OF_PASSAGE
};

/* ------------------------------------------------------------------------
   Defines and Compile Flags

	NOTE: I find myself putting more and more of the #defines here because
	putting them in DOORS.H causes practically the entire project to be
	recompiled.
   ------------------------------------------------------------------------ */
#define MAX_DOORS	(10)  // Max number of doors ACTIVE at any one time...
#define MAX_LIFTS	(10)  // Max number of lifts ACTIVE at any one time...
#define MAX_FLOORS (10)	// Max number of floors ACTIVE at any one time
#define MAX_CEILINGS (10)	// Max number of ceilings ACTIVE at any one time

// Defines for sector rule for finding heights of adjacent sectors.
#define CAN_BE_THIS_SECTOR	0
#define NOT_THIS_SECTOR		1

#define LIFT_DELAY			90			// frames
#define CRUSHER_DELAY		50				// frames

// Assume all floors fall within the following range.  Used to find the
// lowest adjacent floor.
#define FLOOR_MIN_HEIGHT	-1024
#define FLOOR_MAX_HEIGHT	1024


// Speeds:
#define SPEED_CRUSHER		(4)
#define SPEED_DOOR_NORMAL	(6)			// [d5-01-97 JPC] was (2)
#define SPEED_DOOR_TURBO	(20)
#define SPEED_LIFT			(2)
#define SPEED_PIT_TRAP		(10)
#define SPEED_SWDOOR			(4)

// Door states:
#define DS_CLOSED			(0)
#define DS_OPENING		(1)
#define DS_OPEN			(2)
#define DS_CLOSING		(3)
#define DS_JIGGLE       (4)

// Lift, floor, and ceiling states:
#define LFCS_GOING_DOWN	(0)
#define LFCS_GOING_UP   (1)
#define LFCS_PAUSED     (2)
#define LFCS_PAUSED_2   (3)

// Pit trap.
#define PIT_TRAP_DROP	128

// Crusher gaps.  Used to be 1, which killed the player if he was caught
// in a crusher.  Reason for change: we don't want crushers to kill, only
// to wound severely.  Note that floor and ceiling crushers can have
// different-sized gaps, if we want.
#define FLOOR_CRUSHER_GAP		30
#define CEILING_CRUSHER_GAP	30

// Sound effects.
// (I got tired of changing the code from AddLoopingSound to AddSndObj
// and back again.  This also makes it easy to change the sound effect
// for all doors, lifts, floors, or ceilings at once. --JPC)
// NOTE: If you call AddSndObj, then a thing index of -1 is full volume,
//			and -2 is 90% volume;
//    BUT if you call AddLoopingSound, a thing index of -1 will prevent
// 		sound from starting; use -2 for full volume.

#define DOOR_SOUND 		SND_MOVING_FLOOR_LOOP1
#define LIFT_SOUND      SND_MOVING_FLOOR_LOOP2
#define FLOOR_SOUND     SND_MOVING_FLOOR_LOOP3
#define CEILING_SOUND   SND_MOVING_FLOOR_LOOP4

#define START_DOOR_SOUND(i) AddLoopingSound (DOOR_SOUND, i)
#define START_LIFT_SOUND(i) AddLoopingSound (LIFT_SOUND, i)
#define START_FLOOR_SOUND(i) AddLoopingSound (FLOOR_SOUND, i)
#define START_CEILING_SOUND(i) AddLoopingSound (CEILING_SOUND, i)
#define STOP_DOOR_SOUND(i) RemoveLoopingSound (i)
#define STOP_LIFT_SOUND(i) RemoveLoopingSound (i)
#define STOP_FLOOR_SOUND(i) RemoveLoopingSound (i)
#define STOP_CEILING_SOUND(i) RemoveLoopingSound (i)


/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
static LONG GetHighestCeilingHeight (LONG iSector);
void TitledMessage (char* t,char* m1); // in TMSGBOX.CPP
static void DoorKeyAdvise (LONG special);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
// The following are calculated in TEXTURES.C: get_texture.  We need to
// know them so we can alter the wall texture when the player throws
// a switch.
LONG	gSwitchTexture[cSWITCH_TEXTURE_PAIRS][2];

extern BOOL JustLoadWad;					// TRUE if program invoked with "w wadname.wad"

/* ------------------------------------------------------------------------
   Module-level Variables
   ------------------------------------------------------------------------ */
static DOOR doors[MAX_DOORS];    // array of all active doors
static LIFT lifts[MAX_LIFTS];		// array of all active lifts
static LIFT floors[MAX_FLOORS];
static LIFT ceilings[MAX_CEILINGS];
static BOOL gfAdvise;				// if TRUE, tell user what he needs to open
											// door (or that room is inaccessible)

#if defined (_EDIT) && defined (_DEBUG)
static char gszTemp[128];		// for debugging messages
#endif


// ---------------------------------------------------------------------------
static LONG DoorCreateThing (LONG x, LONG y)
{
// Create a dummy thing at x,y.
// Assumes create_thing will return -1 if creation failed, but as of
// 10-29-96, create_thing does not do so.
// ---------------------------------------------------------------------------

	LONG			iDummyThing;

	iDummyThing = create_thing (0, x, y, 0);
	if (iDummyThing != -1)
	{
		mythings[iDummyThing].inVisible = TRUE;
	}
	return iDummyThing;
}


// ---------------------------------------------------------------------------
static void DoorRemoveThing (LONG iDummyThing)
{
	if (iDummyThing != -1)
	{
		purge_thing (iDummyThing);
		remove_thing (iDummyThing);
	}
}


// ---------------------------------------------------------------------------
static void DoorCloseSound (LONG iDummyThing)
{
// Make a close door sound when a door closed, a lift reaches its destination,
// and so on.

	if (iDummyThing != -1)
		AddSndObj (SND_DOOR_CLOSE1, 0, iDummyThing);
	else
		AddSndObj (SND_DOOR_CLOSE1, 0, VOLUME_FULL);
}


/* =======================================================================
   Function    - init_doors
   Description - wipes the door array
					- JPC also wipes the lifts, floors, and ceilings arrays
   Returns     - void
   ======================================================================== */
void init_doors(void)
{
	LONG i;

	for(i=0;i<MAX_DOORS;i++)
	{
		if (doors[i].fInUse)
		{
			if (doors[i].iSound != fERROR)
			{
				STOP_DOOR_SOUND (doors[i].iSound);
				doors[i].iSound = fERROR;
			}
			DoorRemoveThing (doors[i].iDummyThing);
			doors[i].fInUse=FALSE;
		}
	}

	for (i=0; i < MAX_LIFTS; i++)
	{
		if (lifts[i].fInUse)
		{
			if (lifts[i].iSound != fERROR)
			{
				STOP_LIFT_SOUND (lifts[i].iSound);
				lifts[i].iSound = fERROR;
			}
			DoorRemoveThing (lifts[i].iDummyThing);
			lifts[i].fInUse = FALSE;
		}
	}

	for (i=0; i < MAX_FLOORS; i++)
	{
		if (floors[i].fInUse)
		{
			if (floors[i].iSound != fERROR)
			{
				STOP_FLOOR_SOUND (floors[i].iSound);
				floors[i].iSound = fERROR;
			}
			DoorRemoveThing (floors[i].iDummyThing);
			floors[i].fInUse = FALSE;
		}
	}

	for (i=0; i < MAX_CEILINGS; i++)
	{
		if (ceilings[i].fInUse)
		{
			if (ceilings[i].iSound != fERROR)
			{
				STOP_FLOOR_SOUND (ceilings[i].iSound);
				ceilings[i].iSound = fERROR;
			}
			DoorRemoveThing (ceilings[i].iDummyThing);
			ceilings[i].fInUse = FALSE;
		}
	}

	gfInCombat = FALSE;						// [d12-07-96 JPC]
}


/* =======================================================================
   Function    - door_moving
   Description - whether or not the door is "in use", which sort of
						tells us whether it is moving, but not quite...
   Returns     - TRUE or FALSE
   ======================================================================== */
static LONG door_moving(LONG iSector)
{
	LONG i;

	for(i=0;i<MAX_DOORS;i++)
	{
		if(doors[i].fInUse && (doors[i].sector==iSector))
			return(TRUE);
	}
	return(FALSE);
}


/* =======================================================================
   Function    - stop_door
   Description - Stops a door from moving by clearing out its record.
					  This also frees the record to be used by other doors.
   Returns     - void
   ======================================================================== */
void stop_door(LONG iSector)
{
	LONG i;

	for (i=0;i<MAX_DOORS;i++)
	{
		if (doors[i].fInUse && (doors[i].sector==iSector))
		{
			DoorRemoveThing (doors[i].iDummyThing);
			if (doors[i].iSound != fERROR)
			{
				STOP_DOOR_SOUND(doors[i].iSound);
				doors[i].iSound = fERROR;
			}
			doors[i].fInUse=FALSE;
		}
	}
}


// ---------------------------------------------------------------------------
static BOOL DoorGetTagSector (LONG * pSector, LONG tag)
{
// [d8-27-96 JPC]
// Use this function to avoid an ambiguity in tag_to_sector: a return of
// 0 from tag_to_sector can mean sector 0 has the tag or it can mean that
// no sector has the tag.
// Here, if the sector actually has the tag, return TRUE and return the
// sector in *pSector; else return FALSE and don't change pSector.
// ---------------------------------------------------------------------------

	LONG			iSector;

	iSector = tag_to_sector (tag);
	
	if (iSector == 0 && sectors[0].tag != tag)
	{
#if defined (_EDIT) && defined (_DEBUG)
		wsprintf (gszTemp, "WARNING: could not find sector with tag %d", tag);
		MessageBox (NULL, gszTemp, "WARNING", MB_OK);
#endif
		return FALSE;
	}

	*pSector = iSector;
	return TRUE;
}


// ---------------------------------------------------------------------------
static void HandleUpDownDoor (LONG iDoor)
{
// Helper routine of handle_doors.
// Standard DOOM-type door that goes up into the ceiling (types 1, 4, 118, ...).
// Unlike DOOM-type door, however, this one does NOT automatically close.
// You activate it again to close it.
// ---------------------------------------------------------------------------

	if (doors[iDoor].state == DS_OPENING)
	{
		if (doors[iDoor].goal > sectors[doors[iDoor].sector].ch)
		{
			// Raise the ceiling height of the affected sector.
			// This raises the upper texture, which is the door.
			sectors[doors[iDoor].sector].ch += (SHORT)doors[iDoor].speed;
		}
		else
		{
			// Ceiling has reached its goal.  Release the door record.
			// (If we want to automatically close the door, we should
			// keep the record active and set the door state to DS_OPEN.
			// Then, after a certain time, we can close it again.)
			sectors[doors[iDoor].sector].ch = (SHORT)doors[iDoor].goal;
			if (doors[iDoor].iSound != fERROR)
			{
				STOP_DOOR_SOUND (doors[iDoor].iSound);
				doors[iDoor].iSound = fERROR;
			}
			DoorCloseSound (doors[iDoor].iDummyThing);
			stop_door (doors[iDoor].sector);
		}
	}
	else if (doors[iDoor].state == DS_JIGGLE)
	{
		if (doors[iDoor].goal > sectors[doors[iDoor].sector].ch)
		{
			// Raise the ceiling height of the affected sector.
			// This raises the upper texture, which is the door.
			sectors[doors[iDoor].sector].ch += (SHORT)doors[iDoor].speed;
		}
		else
		{
			// Ceiling has reached its goal.  Now go back down.
			sectors[doors[iDoor].sector].ch = (SHORT)doors[iDoor].goal;
			doors[iDoor].state = DS_CLOSING;
			doors[iDoor].goal  = sectors[doors[iDoor].sector].fh;
			doors[iDoor].speed = SPEED_DOOR_NORMAL;
			gfAdvise = TRUE;
		}
	}
	else if (doors[iDoor].state == DS_CLOSING)
	{
		if (doors[iDoor].goal < sectors[doors[iDoor].sector].ch)
		{
			// Lower the ceiling height of the affected sector.
			// This lowers the upper texture, which is the door.
			sectors[doors[iDoor].sector].ch -= (SHORT)doors[iDoor].speed;
		}
		else
		{
			// Ceiling height now equals the floor height.
			sectors[doors[iDoor].sector].ch = (SHORT)doors[iDoor].goal;
			if (gfAdvise)
			{
				DoorKeyAdvise (doors[iDoor].special);
				gfAdvise = FALSE;
			}
			if (doors[iDoor].iSound != fERROR)
			{
				STOP_DOOR_SOUND (doors[iDoor].iSound);
				doors[iDoor].iSound = fERROR;
			}
			DoorCloseSound (doors[iDoor].iDummyThing);
			stop_door (doors[iDoor].sector);
		}
	}
}


// ---------------------------------------------------------------------------
static void HandleSwingingDoor (LONG iDoor)
{
// Helper routine of handle_doors.
// Door that swings on hinges (types 26, 27).
// [d7-31-96 JPC] Changed logic to offset from the original vertex values
// every time; the incremental change led to a noticeable error.
// ---------------------------------------------------------------------------

	POINT			p;
	POINT			origin;
	int			i;
	int			iVertex;

	if (doors[iDoor].state == DS_OPENING)
	{
		if (abs (doors[iDoor].change) < abs (doors[iDoor].goal))
		{
			origin.x = vertexs[doors[iDoor].hingeVertex].x;
			origin.y = vertexs[doors[iDoor].hingeVertex].y;
			for (i = 0; i < 3; i++)
			{
				iVertex = doors[iDoor].movingVertexes[i];
				p.x = doors[iDoor].xOriginal[i];
				p.y = doors[iDoor].yOriginal[i];
				RotateAnglePoint (&p, &origin, doors[iDoor].change);
				vertexs[iVertex].x = p.x;
				vertexs[iVertex].y = p.y;
			}
			doors[iDoor].change += doors[iDoor].speed;
		}
		else
		{
			// [d8-30-96 JPC] Once the swinging door opens, it stops
			// being a door.  Also zero out the linedef's special field.
			stop_door (doors[iDoor].sector);
			linedefs[doors[iDoor].linedef].special = 0; // no more door
		}
	}
	else if (doors[iDoor].state == DS_JIGGLE)
	{
		if (abs (doors[iDoor].change) < abs (doors[iDoor].goal))
		{
			origin.x = vertexs[doors[iDoor].hingeVertex].x;
			origin.y = vertexs[doors[iDoor].hingeVertex].y;
			for (i = 0; i < 3; i++)
			{
				iVertex = doors[iDoor].movingVertexes[i];
				p.x = doors[iDoor].xOriginal[i];
				p.y = doors[iDoor].yOriginal[i];
				RotateAnglePoint (&p, &origin, doors[iDoor].change);
				vertexs[iVertex].x = p.x;
				vertexs[iVertex].y = p.y;
			}
			doors[iDoor].change += doors[iDoor].speed;
		}
		else
		{
			doors[iDoor].state = DS_CLOSING;
			gfAdvise = TRUE;
		}
	}
	else if (doors[iDoor].state == DS_CLOSING)
	{
		// if (abs (doors[iDoor].change) > abs (doors[iDoor].speed))
		if (abs (doors[iDoor].change) > 0)
		{
			doors[iDoor].change -= doors[iDoor].speed;
			origin.x = vertexs[doors[iDoor].hingeVertex].x;
			origin.y = vertexs[doors[iDoor].hingeVertex].y;
			for (i = 0; i < 3; i++)
			{
				iVertex = doors[iDoor].movingVertexes[i];
				p.x = doors[iDoor].xOriginal[i];
				p.y = doors[iDoor].yOriginal[i];
				RotateAnglePoint (&p, &origin, doors[iDoor].change);
				vertexs[iVertex].x = p.x;
				vertexs[iVertex].y = p.y;
			}
		}
		else
		{
			// Door is fully closed, so restore old coordinates.
			for (i = 0; i < 3; i++)
			{
				iVertex = doors[iDoor].movingVertexes[i];
				vertexs[iVertex].x = doors[iDoor].xOriginal[i];
				vertexs[iVertex].y = doors[iDoor].yOriginal[i];
			}
			// Release the record so another door can use it.
			if (gfAdvise)
			{
				DoorKeyAdvise (doors[iDoor].special);
				gfAdvise = FALSE;
			}
			stop_door (doors[iDoor].sector);
		}
	}
}


/* =======================================================================
   Function    - handle_doors
   Description - runs through the door array, updating the door's data
   Returns     - void
	Notes			- set up in MAIN: GameMain; called every frame
   ======================================================================== */
#pragma off (unreferenced)
void handle_doors(LONG arg)
{
	LONG i;

#if defined (_EDIT) && !defined (_JPC)
	return;										// do NOT allow changes in EditWad
#endif

	// [d12-07-96 JPC] Decided not to stop doors when in combat.
	// Reason: door can stop halfway up, and combat proceeds through the
	// half-open door.  Looks silly.  Instead, prevent doors from
	// being activated when in combat.
	if (fFreeze || fPause /* || gfInCombat */)
		return;									// [d9-16-96 JPC]

	for(i=0;i<MAX_DOORS;i++)
	{
		if(doors[i].fInUse)
		{
			switch (doors[i].special)
			{
				case LSP_SWDOOR_L:
				case LSP_SWDOOR_R:

				case LSP_SWDOOR_L_SILVER_KEY:
				case LSP_SWDOOR_L_GOLD_KEY:
				case LSP_SWDOOR_L_RED_RUBY_KEY:
				case LSP_SWDOOR_L_BLUE_SAPPHIRE_KEY:
				case LSP_SWDOOR_L_GREEN_EMERALD_KEY:
				case LSP_SWDOOR_L_WHITE_DIAMOND_KEY:
				case LSP_SWDOOR_L_SKELETON_KEY:
				case LSP_SWDOOR_L_KEY_PURPLE:
				case LSP_SWDOOR_L_KEY_BONE:
				case LSP_SWDOOR_L_KEY_STONE:
				case LSP_SWDOOR_L_GEM_BLUE:
				case LSP_SWDOOR_L_GEM_GREEN:
				case LSP_SWDOOR_L_PASSAGE:

				case LSP_SWDOOR_R_SILVER_KEY:
				case LSP_SWDOOR_R_GOLD_KEY:
				case LSP_SWDOOR_R_RED_RUBY_KEY:
				case LSP_SWDOOR_R_BLUE_SAPPHIRE_KEY:
				case LSP_SWDOOR_R_GREEN_EMERALD_KEY:
				case LSP_SWDOOR_R_WHITE_DIAMOND_KEY:
				case LSP_SWDOOR_R_SKELETON_KEY:
				case LSP_SWDOOR_R_KEY_PURPLE:
				case LSP_SWDOOR_R_KEY_BONE:
				case LSP_SWDOOR_R_KEY_STONE:
				case LSP_SWDOOR_R_GEM_BLUE:
				case LSP_SWDOOR_R_GEM_GREEN:
				case LSP_SWDOOR_R_PASSAGE:
					HandleSwingingDoor (i);
					break;

				default:
					HandleUpDownDoor (i);
					break;
			}
		}
	}
}


// ---------------------------------------------------------------------------
static LONG DoorWhichSide (LONG special)
{
// [d8-05-96 JPC]
// Helper function of open_door.
// Returns which side of the linedef we're interested in for this
// type of door.
// ---------------------------------------------------------------------------

	switch (special)
	{
		// Swinging doors.
      case LSP_SWDOOR_L:
		case LSP_SWDOOR_R:

		case LSP_SWDOOR_L_SILVER_KEY:
		case LSP_SWDOOR_L_GOLD_KEY:
		case LSP_SWDOOR_L_RED_RUBY_KEY:
		case LSP_SWDOOR_L_BLUE_SAPPHIRE_KEY:
		case LSP_SWDOOR_L_GREEN_EMERALD_KEY:
		case LSP_SWDOOR_L_WHITE_DIAMOND_KEY:
		case LSP_SWDOOR_L_SKELETON_KEY:
		case LSP_SWDOOR_L_KEY_PURPLE:
		case LSP_SWDOOR_L_KEY_BONE:
		case LSP_SWDOOR_L_KEY_STONE:
		case LSP_SWDOOR_L_GEM_BLUE:
		case LSP_SWDOOR_L_GEM_GREEN:
		case LSP_SWDOOR_L_PASSAGE:

		case LSP_SWDOOR_R_SILVER_KEY:
		case LSP_SWDOOR_R_GOLD_KEY:
		case LSP_SWDOOR_R_RED_RUBY_KEY:
		case LSP_SWDOOR_R_BLUE_SAPPHIRE_KEY:
		case LSP_SWDOOR_R_GREEN_EMERALD_KEY:
		case LSP_SWDOOR_R_WHITE_DIAMOND_KEY:
		case LSP_SWDOOR_R_SKELETON_KEY:
		case LSP_SWDOOR_R_KEY_PURPLE:
		case LSP_SWDOOR_R_KEY_BONE:
		case LSP_SWDOOR_R_KEY_STONE:
		case LSP_SWDOOR_R_GEM_BLUE:
		case LSP_SWDOOR_R_GEM_GREEN:
		case LSP_SWDOOR_R_PASSAGE:
			return FRONT;                    // uses linedefs[x].psdb
		default:
			// Up/down doors.
			return BACK;                     // uses linedefs[x].psdt
	}
}


// ---------------------------------------------------------------------------
static BOOL DoorFindVertex (SHORT startVertex, SHORT notVertex, SHORT *foundVertex)
{
// [d7-24-96 JPC]
// Find the vertex connected to "startVertex" that is NOT "notVertex."
// Return TRUE if found and set foundVertex = found vertex;
// FALSE if not, and set foundVertex = -1.
// ---------------------------------------------------------------------------

	int			iLinedef;

	*foundVertex = -1;
	for (iLinedef = 0; iLinedef < tot_linedefs; iLinedef++)
	{
		if (linedefs[iLinedef].a == startVertex)
		{
			if (linedefs[iLinedef].b != notVertex)
			{
				*foundVertex = linedefs[iLinedef].b;
				break;
			}
		}
		else if (linedefs[iLinedef].b == startVertex)
		{
			if (linedefs[iLinedef].a != notVertex)
			{
				*foundVertex = linedefs[iLinedef].a;
				break;
			}
		}
	}
	if (*foundVertex == -1)
		return FALSE;
	else
		return TRUE;
}


// ---------------------------------------------------------------------------
static void DoorActivateUpDownDoor (LONG seg, LONG special, LONG iDoor, LONG iSector, BOOL fHaveKey)
{
// [d8-05-96 JPC]
// Helper function of open_door.
// Handles door types 1, 118.  LATER: added keyed doors.
// What it does: sets up a "doors" record so that when the system calls
// handle_doors, which it does every frame, the door will move properly.
// ---------------------------------------------------------------------------

	if (fHaveKey)
	{
		if ((sectors[iSector].ch - sectors[iSector].fh) < 10)
		{
			doors[iDoor].state = DS_OPENING;
			doors[iDoor].goal  = sectors[seg_to_sector(seg,FRONT)].ch-10;
		}
		else
		{
			doors[iDoor].state = DS_CLOSING;
			doors[iDoor].goal  = sectors[iSector].fh;
		}
		if (special == LSP_DOOR_DMF)
			// type LSP_DOOR_DMF: turbo
			doors[iDoor].speed=SPEED_DOOR_TURBO;
		else
			// The usual case.
			doors[iDoor].speed = SPEED_DOOR_NORMAL;
	}
	else
	{
		if ((sectors[iSector].ch - sectors[iSector].fh) < 10)
		{
			// Door closed; jiggle the door a bit, then close.
			doors[iDoor].state = DS_JIGGLE;
			doors[iDoor].goal  = sectors[iSector].fh + 2;
			doors[iDoor].speed = SPEED_DOOR_NORMAL;
		}
		else
		{
			// [d10-29-96 JPC]  Door already open.  OK to close it without a key.
			doors[iDoor].state = DS_CLOSING;
			doors[iDoor].goal  = sectors[iSector].fh;
			if (special == LSP_DOOR_DMF)
				doors[iDoor].speed=SPEED_DOOR_TURBO;
			else
				doors[iDoor].speed = SPEED_DOOR_NORMAL;
		}
	}
}


// ---------------------------------------------------------------------------
static BOOL DoorOpensLeft (LONG special)
{
// [d8-27-96 JPC]
// Helper of DoorActivateSwingingDoor, for when we have more than 2
// kinds of swinging doors.
// ---------------------------------------------------------------------------

	switch (special)
	{
		case LSP_SWDOOR_L:
		case LSP_SWDOOR_L_SILVER_KEY:
		case LSP_SWDOOR_L_GOLD_KEY:
		case LSP_SWDOOR_L_RED_RUBY_KEY:
		case LSP_SWDOOR_L_BLUE_SAPPHIRE_KEY:
		case LSP_SWDOOR_L_GREEN_EMERALD_KEY:
		case LSP_SWDOOR_L_WHITE_DIAMOND_KEY:
		case LSP_SWDOOR_L_SKELETON_KEY:
		case LSP_SWDOOR_L_KEY_PURPLE:
		case LSP_SWDOOR_L_KEY_BONE:
		case LSP_SWDOOR_L_KEY_STONE:
		case LSP_SWDOOR_L_GEM_BLUE:
		case LSP_SWDOOR_L_GEM_GREEN:
		case LSP_SWDOOR_L_PASSAGE:
			return TRUE;
		default:
			return FALSE;
	}
}


// ---------------------------------------------------------------------------
static void DoorActivateSwingingDoor (LONG special, LONG iDoor, LONG iSector, LONG iLinedef, BOOL fClose, BOOL fHaveKey)
{
// [d8-05-96 JPC]
// Helper function of open_door.
// Set up all the information for swinging doors.
// The doors are moved by the callback function handle_doors, which
// calls HandleSwingingDoor.
// ---------------------------------------------------------------------------

	SHORT			v0;
	SHORT			v1;
	SHORT			v2;

	if (!fClose)
	{
		doors[iDoor].state  = DS_OPENING;
		doors[iDoor].change = 0;

		if (DoorOpensLeft (special))
		{
			// Swing counterclockwise.
			doors[iDoor].goal        = 72; // 64 = 90 degrees
			doors[iDoor].speed       = SPEED_SWDOOR;
			doors[iDoor].hingeVertex = linedefs[iLinedef].a;
			v0                       = linedefs[iLinedef].b;
		}
		else
		{
			// Swing clockwise.
			doors[iDoor].goal        = -72;
			doors[iDoor].speed       = -SPEED_SWDOOR;
			doors[iDoor].hingeVertex = linedefs[iLinedef].b;
			v0                       = linedefs[iLinedef].a;
		}

		doors[iDoor].movingVertexes[0] = v0;
		doors[iDoor].xOriginal[0]      = vertexs[v0].x;
		doors[iDoor].yOriginal[0]      = vertexs[v0].y;

		if (DoorFindVertex (doors[iDoor].hingeVertex, v0, &v1))
		{
			doors[iDoor].movingVertexes[1] = v1;
			doors[iDoor].xOriginal[1]      = vertexs[v1].x;
			doors[iDoor].yOriginal[1]      = vertexs[v1].y;
		}
		else
		{
#ifdef _DEBUG
			fatal_error ("Could not find vertex v1 in open_door.");
#else
			return;
#endif
		}

		if (DoorFindVertex (v1, doors[iDoor].hingeVertex, &v2))
		{
			doors[iDoor].movingVertexes[2] = v2;
			doors[iDoor].xOriginal[2]      = vertexs[v2].x;
			doors[iDoor].yOriginal[2]      = vertexs[v2].y;
		}
		else
		{
#ifdef _DEBUG
			fatal_error ("Could not find vertex v2 in open_door.");
#else
			return;
#endif
		}

		// Now modify the above if the player didn't have the right key.
		if (!fHaveKey)
		{
			doors[iDoor].state = DS_JIGGLE;
			if (DoorOpensLeft (special))
			{
				doors[iDoor].goal        = 8;
				doors[iDoor].speed       = SPEED_SWDOOR;
			}
			else
			{
				doors[iDoor].goal        = -8;
				doors[iDoor].speed       = -SPEED_SWDOOR;
			}
		}
	}
	// [d9-06-96 JPC] Swinging doors can't close anymore.
	// else
	// {
	// 	doors[iDoor].state = DS_CLOSING;
	// 	// goal is irrelevant at this point.
	// 	// doors[iDoor].goal      = 0;
	// }
}


// ---------------------------------------------------------------------------
static BOOL DoorKeyOK (LONG special)
{
// [d8-28-96 JPC]
// Helper function of open_door.
// Check whether the player has the key for this type door.
// ---------------------------------------------------------------------------

	BOOL ReturnValue=FALSE;	//[WRC] 12/11/96  This is so that we can play a
							//sound easier. (see below)

#ifdef _DEBUG
	// [d10-29-96 JPC] DoesPlayerHaveItem blows up if we invoked the game
	// with the "w wadname.wad" command.
	if (JustLoadWad)
		return TRUE;
#endif

	if (fSpellKnocking)
		return TRUE;


	if (DoesPlayerHaveItem (CHIME_OF_OPENING))
	{
		AddSndObj(SND_HIGH_CHIME_OF_OPENING1_1,NULL,VOLUME_FULL);
		return TRUE;
	}

	
	
	switch (special)
	{
		case LSP_KEY_SILVER:
		case LSP_SWDOOR_L_SILVER_KEY:
		case LSP_SWDOOR_R_SILVER_KEY:
			ReturnValue= (DoesPlayerHaveItem (KEY_SILVER) || DoesPlayerHaveItem (KEY_SKELETON));
			break;
		case LSP_KEY_GOLD:
		case LSP_SWDOOR_L_GOLD_KEY:
		case LSP_SWDOOR_R_GOLD_KEY:
			ReturnValue= (DoesPlayerHaveItem (KEY_GOLD) || DoesPlayerHaveItem (KEY_SKELETON));
			break;
		case LSP_KEY_RED:
		case LSP_SWDOOR_L_RED_RUBY_KEY:
		case LSP_SWDOOR_R_RED_RUBY_KEY:
			ReturnValue= (DoesPlayerHaveItem (KEY_RED) || DoesPlayerHaveItem (KEY_SKELETON));
			break;
		case LSP_KEY_BLUE:
		case LSP_SWDOOR_L_BLUE_SAPPHIRE_KEY:
		case LSP_SWDOOR_R_BLUE_SAPPHIRE_KEY:
			ReturnValue= (DoesPlayerHaveItem (KEY_BLUE) || DoesPlayerHaveItem (KEY_SKELETON));
			break;
		case LSP_KEY_GREEN:
		case LSP_SWDOOR_L_GREEN_EMERALD_KEY:
		case LSP_SWDOOR_R_GREEN_EMERALD_KEY:
			ReturnValue= (DoesPlayerHaveItem (KEY_GREEN) || DoesPlayerHaveItem (KEY_SKELETON));
			break;
		case LSP_KEY_WHITE:
		case LSP_SWDOOR_L_WHITE_DIAMOND_KEY:
		case LSP_SWDOOR_R_WHITE_DIAMOND_KEY:
			ReturnValue= (DoesPlayerHaveItem (KEY_WHITE) || DoesPlayerHaveItem (KEY_SKELETON));
			break;
		case LSP_KEY_SKELETON:
		case LSP_SWDOOR_L_SKELETON_KEY:
		case LSP_SWDOOR_R_SKELETON_KEY:
			ReturnValue= (DoesPlayerHaveItem (KEY_SKELETON));
			break;
		case LSP_KEY_PURPLE:
		case LSP_SWDOOR_L_KEY_PURPLE:
		case LSP_SWDOOR_R_KEY_PURPLE:
			// [d1-07-97 JPC] KEY_PURPLE became the black key at some point.
			ReturnValue= (DoesPlayerHaveItem (KEY_PURPLE) || DoesPlayerHaveItem (KEY_SKELETON));
			break;
		case LSP_KEY_BONE:
		case LSP_SWDOOR_L_KEY_BONE:
		case LSP_SWDOOR_R_KEY_BONE:
			ReturnValue= (DoesPlayerHaveItem (KEY_BONE) || DoesPlayerHaveItem (KEY_SKELETON));
			break;
		case LSP_KEY_STONE:
		case LSP_SWDOOR_L_KEY_STONE:
		case LSP_SWDOOR_R_KEY_STONE:
			ReturnValue= (DoesPlayerHaveItem (KEY_STONE) || DoesPlayerHaveItem (KEY_SKELETON));
			break;
		case LSP_GEM_BLUE:
		case LSP_SWDOOR_L_GEM_BLUE:
		case LSP_SWDOOR_R_GEM_BLUE:
			if (DoesPlayerHaveItem(GEM_BLUE))
				AddSndObj(SND_GEM_STONE1,NULL,VOLUME_FULL);
			return (DoesPlayerHaveItem (GEM_BLUE) ||
				DoesPlayerHaveItem (KEY_SKELETON));
		case LSP_GEM_GREEN:
		case LSP_SWDOOR_L_GEM_GREEN:
		case LSP_SWDOOR_R_GEM_GREEN:
			if (DoesPlayerHaveItem(GEM_GREEN))
				AddSndObj(SND_GEM_STONE1,NULL,VOLUME_FULL);
			return (DoesPlayerHaveItem (GEM_GREEN) ||
				DoesPlayerHaveItem (KEY_SKELETON));
		case LSP_GEM_OF_PASSAGE:
		case LSP_SWDOOR_L_PASSAGE:
		case LSP_SWDOOR_R_PASSAGE:
			if (DoesPlayerHaveItem(GEM_OF_PASSAGE))
				AddSndObj(SND_GEM_STONE1,NULL,VOLUME_FULL);
			// Skeleton key does not work here.
			return (DoesPlayerHaveItem (GEM_OF_PASSAGE));
		default:
			return TRUE;						// (no key required)
	}

	if (ReturnValue)
		AddSndObj(SND_KEY_LOCK1,NULL,VOLUME_FULL);

	return ReturnValue;
}


// ---------------------------------------------------------------------------
static void DoorKeyAdvise (LONG special)
{
// [d8-28-96 JPC]
// Helper function of open_door.
// Intended to be called if player does not have the proper key.
// It advises the player about which key is needed.
// ---------------------------------------------------------------------------

	int			iItemList;
	char			szTemp[128];
	char			szItem[64];

	if (special >= 120)
		iItemList = 10 + (special % 10);
	else
		iItemList = special % 10;

	// strcpy (szItem, gszItemList[iItemList]);
	// sprintf (szTemp, "You need the %s to open this door", szItem);
	// TitledMessage ("Door Locked", szTemp);
	strcpy (szItem, STRMGR_GetStr (giString[iItemList]));
	sprintf (szTemp, STRMGR_GetStr (STR_DOOR_LOCK_ADVISE), szItem);
	TitledMessage (STRMGR_GetStr (STR_DOOR_LOCK_TITLE), szTemp);
}


// ---------------------------------------------------------------------------
void DoorChangeSwitchTexture (LONG iLinedef)
{
// [d9-13-96 JPC]
// Where appropriate, call this function to change the wall texture
// so it looks as if we threw a switch.
// Assumes switch is on sidedef1, middle texture.  Should be a safe assumption.
// [d11-14-96 JPC] NOT a safe assumption! Switch can be in upper texture or
// lower texture as well as middle texture!
// ---------------------------------------------------------------------------

	short iSidedef = linedefs[iLinedef].psdb;
	LONG  iTexture;
	LONG	i;

#if defined (_EDIT) && !defined (_JPC)
	return;										// do NOT allow changes in EditWad
#endif

	iTexture = (LONG) sidedefs[iSidedef].n3;	// n3 is middle
	for (i = 0; i < cSWITCH_TEXTURE_PAIRS; i++)
	{
		if (iTexture == gSwitchTexture[i][0])
		{
			sidedefs[iSidedef].n3 = gSwitchTexture[i][1];
			return;
		}
		else if (iTexture == gSwitchTexture[i][1])
		{
			sidedefs[iSidedef].n3 = gSwitchTexture[i][0];
			return;
		}
	}

	iTexture = (LONG) sidedefs[iSidedef].n1;
	for (i = 0; i < cSWITCH_TEXTURE_PAIRS; i++)
	{
		if (iTexture == gSwitchTexture[i][0])
		{
			sidedefs[iSidedef].n1 = gSwitchTexture[i][1];
			return;
		}
		else if (iTexture == gSwitchTexture[i][1])
		{
			sidedefs[iSidedef].n1 = gSwitchTexture[i][0];
			return;
		}
	}

	iTexture = (LONG) sidedefs[iSidedef].n2;
	for (i = 0; i < cSWITCH_TEXTURE_PAIRS; i++)
	{
		if (iTexture == gSwitchTexture[i][0])
		{
			sidedefs[iSidedef].n2 = gSwitchTexture[i][1];
			return;
		}
		else if (iTexture == gSwitchTexture[i][1])
		{
			sidedefs[iSidedef].n2 = gSwitchTexture[i][0];
			return;
		}
	}
}


// ---------------------------------------------------------------------------

#pragma on (unreferenced)

/* =======================================================================
   Function    - open_door
   Description - adds a door to the "doors" array, and starts it moving
   Returns     - void
	Notes			- called from SECTORS: activate_seg.
					- [d7-24-96 JPC] Added "special" param so we can tell
					  which kind of door it is.
					- Handles only "manual" doors.
                 Remote doors are handled in DoorActivate.
   ======================================================================== */
void open_door (LONG seg, LONG special)
{
// Can also CLOSE a door.
// seg = line segment
// special = special flag of the seg's linedef
// ---------------------------------------------------------------------------

	LONG 			iDoor;
	LONG 			iSector;
	LONG			frontOrBackSide;
	LONG			x;
	LONG			y;
	SHORT			iLinedef;
	BOOL			fClose;
	BOOL			fHaveKey;

#if defined (_EDIT) && !defined (_JPC)
	return;										// do NOT allow changes in EditWad
#endif

	if (gfInCombat)
		return;									// [d12-07-96 JPC]

	if (special == LSP_DOOR_INACCESSIBLE)
	{
		char szTemp[50];
		strcpy(szTemp, STRMGR_GetStr (STR_DOOR_LOCK_NOACCESS));
		TitledMessage (STRMGR_GetStr (STR_DOOR_LOCK_TITLE), szTemp);
		return;
	}

	fHaveKey = DoorKeyOK (special);
	if (!fHaveKey)
	{
		AddSndObj(SND_DOOR_HANDLE_RATTLE1,0,VOLUME_FULL);
	}

	frontOrBackSide = DoorWhichSide (special);
	iSector = seg_to_sector (seg, frontOrBackSide);
	iLinedef = segs[seg].lptr;

	fClose = FALSE;							// default

	// You cannot interrupt a door.  If it is moving, just return.
	if (door_moving(iSector))
		return;

	if (!fClose)
	{
		for (iDoor=0; iDoor < MAX_DOORS; iDoor++)
		{
			if (!doors[iDoor].fInUse)
				break;
		}
	
		if(iDoor>=MAX_DOORS)
		{
#ifdef _DEBUG
			fatal_error("Too many active doors");
#else
			return;
#endif
		}
	
		doors[iDoor].sector  = iSector;
		doors[iDoor].linedef = iLinedef;
		doors[iDoor].special = special;
		doors[iDoor].fInUse  = TRUE;
		doors[iDoor].iDummyThing = -1;	// default
	}

	switch (special)
	{
		case LSP_DOOR_DMS:
		case LSP_DOOR_DMF:
		case LSP_KEY_SILVER:
		case LSP_KEY_GOLD:
		case LSP_KEY_RED:
		case LSP_KEY_BLUE:
		case LSP_KEY_GREEN:
		case LSP_KEY_WHITE:
		case LSP_KEY_SKELETON:
		case LSP_KEY_PURPLE:
		case LSP_KEY_BONE:
		case LSP_KEY_STONE:
		case LSP_GEM_BLUE:
		case LSP_GEM_GREEN:
		case LSP_GEM_OF_PASSAGE:
			x = vertexs[linedefs[iLinedef].a].x;
			y = vertexs[linedefs[iLinedef].a].y;
			if (fHaveKey)
			{
				doors[iDoor].iDummyThing = DoorCreateThing (x, y);
				// doors[iDoor].iDummyThing = -1;
				if (doors[iDoor].iDummyThing != -1)
				{
					// doors[iDoor].iSound = AddSndObj (SND_MOVING_FLOOR_LOOP1, 0, doors[iDoor].iDummyThing);
					doors[iDoor].iSound = START_DOOR_SOUND (doors[iDoor].iDummyThing);
				}
				else
				{
					// doors[iDoor].iSound = AddSndObj (SND_MOVING_FLOOR_LOOP1, 0, -1);
					doors[iDoor].iSound = START_DOOR_SOUND (VOLUME_FULL);
				}
			}
			else
			{
				doors[iDoor].iDummyThing = -1;
			}
			DoorActivateUpDownDoor (seg, special, iDoor, iSector, fHaveKey);
			break;

      case LSP_SWDOOR_L:
		case LSP_SWDOOR_R:

		case LSP_SWDOOR_L_SILVER_KEY:
		case LSP_SWDOOR_L_GOLD_KEY:
		case LSP_SWDOOR_L_RED_RUBY_KEY:
		case LSP_SWDOOR_L_BLUE_SAPPHIRE_KEY:
		case LSP_SWDOOR_L_GREEN_EMERALD_KEY:
		case LSP_SWDOOR_L_WHITE_DIAMOND_KEY:
		case LSP_SWDOOR_L_SKELETON_KEY:
		case LSP_SWDOOR_L_KEY_PURPLE:
		case LSP_SWDOOR_L_KEY_BONE:
		case LSP_SWDOOR_L_KEY_STONE:
		case LSP_SWDOOR_L_GEM_BLUE:
		case LSP_SWDOOR_L_GEM_GREEN:
		case LSP_SWDOOR_L_PASSAGE:

		case LSP_SWDOOR_R_SILVER_KEY:
		case LSP_SWDOOR_R_GOLD_KEY:
		case LSP_SWDOOR_R_RED_RUBY_KEY:
		case LSP_SWDOOR_R_BLUE_SAPPHIRE_KEY:
		case LSP_SWDOOR_R_GREEN_EMERALD_KEY:
		case LSP_SWDOOR_R_WHITE_DIAMOND_KEY:
		case LSP_SWDOOR_R_SKELETON_KEY:
		case LSP_SWDOOR_R_KEY_PURPLE:
		case LSP_SWDOOR_R_KEY_BONE:
		case LSP_SWDOOR_R_KEY_STONE:
		case LSP_SWDOOR_R_GEM_BLUE:
		case LSP_SWDOOR_R_GEM_GREEN:
		case LSP_SWDOOR_R_PASSAGE:
			// doors[iDoor].iDummyThing = DoorCreateThing (x, y);
			doors[iDoor].iDummyThing = -1;
			//  AddSndObj (SND_DOOR_CREAK1, SND_DOOR_CREAK_TOTAL, doors[iDoor].iDummyThing);
			if (fHaveKey)
			{
				AddSndObj (SND_DOOR_CREAK1, SND_DOOR_CREAK_TOTAL, VOLUME_FULL);
			}
			DoorActivateSwingingDoor (special, iDoor, iSector, iLinedef, fClose, fHaveKey);
			break;
	}
}


// ---------------------------------------------------------------------------
static BOOL DoorGetSectorCenter (LONG iSector, LONG * x, LONG * y)
{
// Given a sector number, return the center of its bounding rectangle
// in x and y.
// ---------------------------------------------------------------------------

	LONG			iLinedef;
	LONG			xmin, xmax, ymin, ymax;

	// Start with absurd values that are bound to change.
	xmin = 10000;
	ymin = 10000;
	xmax = -10000;
	ymax = -10000;

	for (iLinedef = 0; iLinedef < tot_linedefs; iLinedef++)
	{
		if (sidedefs[linedefs[iLinedef].psdb].sec_ptr == iSector ||
			sidedefs[linedefs[iLinedef].psdt].sec_ptr == iSector)
		{
			if (xmin > vertexs[linedefs[iLinedef].a].x) xmin = vertexs[linedefs[iLinedef].a].x;
			if (xmin > vertexs[linedefs[iLinedef].b].x) xmin = vertexs[linedefs[iLinedef].b].x;
			if (xmax < vertexs[linedefs[iLinedef].a].x) xmax = vertexs[linedefs[iLinedef].a].x;
			if (xmax < vertexs[linedefs[iLinedef].b].x) xmax = vertexs[linedefs[iLinedef].b].x;
			if (ymin > vertexs[linedefs[iLinedef].a].y) ymin = vertexs[linedefs[iLinedef].a].y;
			if (ymin > vertexs[linedefs[iLinedef].b].y) ymin = vertexs[linedefs[iLinedef].b].y;
			if (ymax < vertexs[linedefs[iLinedef].a].y) ymax = vertexs[linedefs[iLinedef].a].y;
			if (ymax < vertexs[linedefs[iLinedef].b].y) ymax = vertexs[linedefs[iLinedef].b].y;
		}
	}

	// Sanity check.
	if (xmin == 10000 || ymin == 10000 || xmax == -10000 || ymax == -10000)
		return FALSE;

	*x = (xmin + xmax) / 2;
	*y = (ymin + ymax) / 2;
	return TRUE;
}


// ---------------------------------------------------------------------------
void DoorActivate (LONG iLinedef, LONG tag, LONG special)
{
// Activate the sector(s) with tag "tag" as a remote door.
// Called from SECTORS.C: activate_seg (for spacebar click activation)
// and BUMP.C: CheckLinesInBlock (for walkover activation).
// Door types 4 (walkover) and 29 (switch), as of 8-07-96.
// [d9-09-96 JPC] Added types 5 and 30 (fast versions of 4 and 29)
// ---------------------------------------------------------------------------

	LONG			iSector;
	LONG			iDoor;
	LONG			x, y;
	BOOL			fSuccess;
	BOOL			fChangedTexture = FALSE;

#if defined (_EDIT) && !defined (_JPC)
	return;										// do NOT allow changes in EditWad
#endif

	if (gfInCombat)
		return;									// [d12-07-96 JPC]

	if (!DoorGetTagSector (&iSector, tag))
		return;									// no sector has this tag

	while (iSector != 0)
	{
		if (door_moving(iSector))
		{
			iSector = TagToNextSector (iSector + 1, tag);
			continue;							// not return!
		}

		for (iDoor=0; iDoor < MAX_DOORS; iDoor++)
		{
			if (!doors[iDoor].fInUse)
				break;
		}
	
		doors[iDoor].sector  = iSector;
		doors[iDoor].linedef = iLinedef;
		doors[iDoor].special = special;
		doors[iDoor].fInUse  = TRUE;
		doors[iDoor].iDummyThing = -1;	// default

		fSuccess = FALSE;						// default
		if (DoorGetSectorCenter (iSector, &x, &y))
		{
			if ((doors[iDoor].iDummyThing = DoorCreateThing (x, y)) != -1)
			{
				fSuccess = TRUE;
				doors[iDoor].iSound = START_DOOR_SOUND (doors[iDoor].iDummyThing);
			}
		}

		if (!fSuccess)
			doors[iDoor].iSound = START_DOOR_SOUND (VOLUME_FULL);

		if ((sectors[iSector].ch - sectors[iSector].fh) < 10)
		{
			doors[iDoor].state = DS_OPENING;
			doors[iDoor].goal  = GetHighestCeilingHeight (iSector) - 10;
		}
		else
		{
			doors[iDoor].state = DS_CLOSING;
			doors[iDoor].goal  = sectors[iSector].fh;
		}

		if (special == LSP_DOOR_DWS || special == LSP_DOOR_S)
			doors[iDoor].speed = SPEED_DOOR_NORMAL;
		else
			doors[iDoor].speed = SPEED_DOOR_TURBO;

		// For switches, change the wall texture to show the switch
		// in a different state.
		if (special == LSP_DOOR_S || special == LSP_DOOR_SF)
		{
			if (!fChangedTexture)
			{
				// [d11-18-96 JPC] Only change the texture once!
				// Otherwise, if a switch controls an even number of
				// things, the texture will toggle back to where it was.
				DoorChangeSwitchTexture (iLinedef);
				fChangedTexture = TRUE;
			}
		}
	
		iSector = TagToNextSector (iSector + 1, tag);
	}
}


// ===========================================================================
//
//	SECTOR STUFF--Lifts, floors, ceilings.
// These effects are activated from BUMP: CheckLinesInBlock when the
// player crosses a certain linedef.  The special field of the linedef
// tells us what to do and the tag field tells us which sector to do it
// to (i.e., the sector with the same tag number).
//
// ===========================================================================
// LIFTS
//
static LONG LiftFindUnusedRecord ()
{
// [d8-05-96 JPC]
// Return first unused lift record.
// Return MAX_LIFTS if no unused records.
// ---------------------------------------------------------------------------

	LONG			iLift;

	for (iLift = 0; iLift < MAX_LIFTS; iLift++)
	{
		if (!lifts[iLift].fInUse)
			break;
	}
	return iLift;
}


// ---------------------------------------------------------------------------
static LONG GetLowestFloorHeight (LONG iSector, LONG sectorRule)
{
// [d8-05-96 JPC]
// Given a sector "iSector," return the lowest floor height of iSector and
// adjacent sectors.
// Method: go through all the linedefs; for all linedefs that have 2 sides,
// check the sectors of the sidedefs; if one of the sidedefs has iSector
// as its sector, get the floor height of the other sidedef's sector,
// and use it if it is lower than the current lowest.
// [d9-09-96 JPC] Modification: if sectorRule is NOT_THIS_SECTOR, then
// the "lowest" floor MUST be HIGHER than this sector's floor.
// ---------------------------------------------------------------------------

	LONG			iLinedef;
	LONG			otherSector;
	LONG			lowestFloorHeight;
	short			fh;
	short			minimumHeight;

	// If the "adjacent" sector can be this sector, start with the lowest
	// floor height = this sector's floor height.
   // If the adjacent sector cannot be this sector, pick a very high floor
	// height to start with.

	if (sectorRule == CAN_BE_THIS_SECTOR)
	{
		lowestFloorHeight = sectors[iSector].fh;
		// We don't really want a minimum for this type sectorRule, so
		// make it a large negative number.  (This way we don't have to
		// check the rule in the comparison of "fh" below.)
		minimumHeight = FLOOR_MIN_HEIGHT;
	}
	else
	{
		lowestFloorHeight = FLOOR_MAX_HEIGHT;
		minimumHeight = sectors[iSector].fh + 1;
	}

	for (iLinedef = 0; iLinedef < tot_linedefs; iLinedef++)
	{
		if (linedefs[iLinedef].psdt != -1)
		{
			if (sidedefs[linedefs[iLinedef].psdb].sec_ptr == iSector)
			{
				otherSector = sidedefs[linedefs[iLinedef].psdt].sec_ptr;
				fh = sectors[otherSector].fh;
				if (fh < lowestFloorHeight && fh > minimumHeight)
					lowestFloorHeight = fh;
			}
			else if (sidedefs[linedefs[iLinedef].psdt].sec_ptr == iSector)
			{
				otherSector = sidedefs[linedefs[iLinedef].psdb].sec_ptr;
				fh = sectors[otherSector].fh;
				if (fh < lowestFloorHeight && fh > minimumHeight)
					lowestFloorHeight = fh;
			}
		}
	}

	// Put in a special trap for the case where we mistakenly mark a sector
	// as a #12 lift but forget to put a higher sector next to it.
	if (lowestFloorHeight == FLOOR_MAX_HEIGHT)
		lowestFloorHeight = minimumHeight;

	return lowestFloorHeight;
}


// ---------------------------------------------------------------------------
static LONG GetHighestCeilingHeight (LONG iSector)
{
// [d8-06-96 JPC]
// Given a sector "iSector," return the highest ceiling height of iSector and
// adjacent sectors.
// Method: go through all the linedefs; for all linedefs that have 2 sides,
// check the sectors of the sidedefs; if one of the sidedefs has iSector
// as its sector, get the ceiling height of the other sidedef's sector,
// and use it if it is higher than the current winner.
// ---------------------------------------------------------------------------

	LONG			iLinedef;
	LONG			otherSector;
	LONG			highestCeilingHeight;
	short			ch;

	highestCeilingHeight = sectors[iSector].ch;

	for (iLinedef = 0; iLinedef < tot_linedefs; iLinedef++)
	{
		if (linedefs[iLinedef].psdt != -1)
		{
			if (sidedefs[linedefs[iLinedef].psdb].sec_ptr == iSector)
			{
				otherSector = sidedefs[linedefs[iLinedef].psdt].sec_ptr;
				ch = sectors[otherSector].ch;
				if (ch > highestCeilingHeight)
					highestCeilingHeight = ch;
			}
			else if (sidedefs[linedefs[iLinedef].psdt].sec_ptr == iSector)
			{
				otherSector = sidedefs[linedefs[iLinedef].psdb].sec_ptr;
				ch = sectors[otherSector].ch;
				if (ch > highestCeilingHeight)
					highestCeilingHeight = ch;
			}
		}
	}
	return highestCeilingHeight;
}


// ---------------------------------------------------------------------------
static void HandleLiftType10 (LONG iLift)
{
// [d8-05-96 JPC]
// Helper routine of HandleLifts.
// Type 10 goes down to the level of the lowest adjacent floor, including
// itself, pauses, then moves up again.
// Type 13 is like type 10 but is switch-activated.
// Type 15 is like type 10 but is continuous.
// Type 17 is like type 10 but is continuous and is switch-activated.
// ---------------------------------------------------------------------------
	
	switch (lifts[iLift].state)
	{
		case LFCS_GOING_DOWN:
			if (lifts[iLift].current > lifts[iLift].goal)
			{
				lifts[iLift].current -= lifts[iLift].speed;
				sectors[lifts[iLift].sector].fh = lifts[iLift].current;
			}
			else
			{
				lifts[iLift].state = LFCS_PAUSED;
				lifts[iLift].cFramesWait = LIFT_DELAY;
				if (lifts[iLift].iSound != fERROR)
				{
					STOP_LIFT_SOUND (lifts[iLift].iSound);
					lifts[iLift].iSound = fERROR;
				}
				DoorCloseSound (lifts[iLift].iDummyThing);
			}
			break;

		case LFCS_GOING_UP:
			if (lifts[iLift].current < lifts[iLift].heightOrig)
			{
				lifts[iLift].current += lifts[iLift].speed;
				sectors[lifts[iLift].sector].fh = lifts[iLift].current;
			}
			else
			{
				if (lifts[iLift].iSound != fERROR)
				{
					STOP_LIFT_SOUND (lifts[iLift].iSound);
					lifts[iLift].iSound = fERROR;
				}

				DoorCloseSound (lifts[iLift].iDummyThing);

				if (lifts[iLift].special == LSP_LIFT_WI ||
					lifts[iLift].special == LSP_LIFT_SI)
				{
					// Lift does its thing once, then turns off.
					sectors[lifts[iLift].sector].fh = lifts[iLift].heightOrig;
					DoorRemoveThing (lifts[iLift].iDummyThing);
					lifts[iLift].fInUse = FALSE;
				}
				else
				{
					// Lift goes forever.
					lifts[iLift].state = LFCS_PAUSED_2;
					lifts[iLift].cFramesWait = LIFT_DELAY;
				}
			}
			break;

		case LFCS_PAUSED:
			lifts[iLift].cFramesWait--;
			if (lifts[iLift].cFramesWait <= 0)
			{
				lifts[iLift].state = LFCS_GOING_UP;
				if (lifts[iLift].iDummyThing != -1)
					lifts[iLift].iSound = START_LIFT_SOUND (lifts[iLift].iDummyThing);
			}
			break;

		case LFCS_PAUSED_2:
			lifts[iLift].cFramesWait--;
			if (lifts[iLift].cFramesWait <= 0)
			{
				lifts[iLift].state = LFCS_GOING_DOWN;
				if (lifts[iLift].iDummyThing != -1)
					lifts[iLift].iSound = START_LIFT_SOUND (lifts[iLift].iDummyThing);
			}
			break;
	}
	ChangeThingZ (lifts[iLift].sector);
}


// ---------------------------------------------------------------------------
static void HandleLiftType12 (LONG iLift)
{
// [d8-26-96 JPC]
// Helper routine of HandleLifts.
// Type 12 moves UP to the height of the LOWEST adjacent
// floor, NOT including itself, pauses, then moves down.
// ---------------------------------------------------------------------------
	
	switch (lifts[iLift].state)
	{
		case LFCS_GOING_UP:
			if (lifts[iLift].current < lifts[iLift].goal)
			{
				lifts[iLift].current += lifts[iLift].speed;
				sectors[lifts[iLift].sector].fh = lifts[iLift].current;
			}
			else
			{
				lifts[iLift].state = LFCS_PAUSED;
				lifts[iLift].cFramesWait = LIFT_DELAY;
				if (lifts[iLift].iSound != fERROR)
				{
					STOP_LIFT_SOUND (lifts[iLift].iSound);
					lifts[iLift].iSound = fERROR;
				}
				DoorCloseSound (lifts[iLift].iDummyThing);
			}
			break;

		case LFCS_GOING_DOWN:
			if (lifts[iLift].current > lifts[iLift].heightOrig)
			{
				lifts[iLift].current -= lifts[iLift].speed;
				sectors[lifts[iLift].sector].fh = lifts[iLift].current;
			}
			else
			{
				if (lifts[iLift].iSound != fERROR)
				{
					STOP_LIFT_SOUND (lifts[iLift].iSound);
					lifts[iLift].iSound = fERROR;
				}
				DoorCloseSound (lifts[iLift].iDummyThing);

				if (lifts[iLift].special == LSP_LIFT_WA ||
					lifts[iLift].special == LSP_LIFT_SA)
				{
					// Lift does its thing once, then turns off.
					sectors[lifts[iLift].sector].fh = lifts[iLift].heightOrig;
					DoorRemoveThing (lifts[iLift].iDummyThing);
					lifts[iLift].fInUse = FALSE;
				}
				else
				{
					// Lift goes forever.
					lifts[iLift].state = LFCS_PAUSED_2;
					lifts[iLift].cFramesWait = LIFT_DELAY;
				}
			}
			break;

		case LFCS_PAUSED:
			lifts[iLift].cFramesWait--;
			if (lifts[iLift].cFramesWait <= 0)
			{
				lifts[iLift].state = LFCS_GOING_DOWN;
				if (lifts[iLift].iDummyThing != -1)
					lifts[iLift].iSound = START_LIFT_SOUND (lifts[iLift].iDummyThing);
			}
			break;

		case LFCS_PAUSED_2:
			lifts[iLift].cFramesWait--;
			if (lifts[iLift].cFramesWait <= 0)
			{
				lifts[iLift].state = LFCS_GOING_UP;
				if (lifts[iLift].iDummyThing != -1)
					lifts[iLift].iSound = START_LIFT_SOUND (lifts[iLift].iDummyThing);
			}
			break;
	}
	ChangeThingZ (lifts[iLift].sector);
}


// ---------------------------------------------------------------------------
void HandleLifts (LONG arg)
{
// [d8-05-96 JPC]
// ---------------------------------------------------------------------------

	LONG iLift;

#if defined (_EDIT) && !defined (_JPC)
	return;										// do NOT allow changes in EditWad
#endif

	if (fFreeze || fPause || gfInCombat)
		return;									// [d9-16-96 JPC]

	for (iLift = 0; iLift < MAX_LIFTS; iLift++)
	{
		if (lifts[iLift].fInUse)
		{
			switch (lifts[iLift].special)
			{
            case LSP_LIFT_WI:
            case LSP_LIFT_SI:
            case LSP_LIFT_WIC:
            case LSP_LIFT_SIC:
					// In DOOM this is a one-use lift; we can emulate that
					// by setting the sector tag to 0 after the lift is done,
					// but we do NOT do that at this point.
					HandleLiftType10 (iLift);
					break;
				case LSP_LIFT_WA:
				case LSP_LIFT_SA:
				case LSP_LIFT_WAC:
				case LSP_LIFT_SAC:
					// Type 12 moves UP to the height of the LOWEST adjacent
					// floor, NOT including itself, pauses, then moves down.
					HandleLiftType12 (iLift);
					break;
			}
		}
	}
}


// ---------------------------------------------------------------------------
static BOOL LiftIsActive (LONG iSector)
{
// [d9-11-96 JPC]
// Return TRUE if the lift in this sector is already active.
// ---------------------------------------------------------------------------

	LONG			iLift;

	for (iLift = 0; iLift < MAX_LIFTS; iLift++)
	{
		if (lifts[iLift].sector == iSector && lifts[iLift].fInUse)
			return TRUE;
	}
	return FALSE;
}


// ---------------------------------------------------------------------------
void LiftActivate (LONG iLinedef, LONG tag, LONG special)
{
// [d8-05-96 JPC]
// Activate the sector with tag as a lift.
// ---------------------------------------------------------------------------

	LONG			iLift;
	LONG			iSector;
	LONG			x, y;
	BOOL			fSuccess;
	BOOL			fChangedTexture = FALSE;

#if defined (_EDIT) && !defined (_JPC)
	return;										// do NOT allow changes in EditWad
#endif

	if (!DoorGetTagSector (&iSector, tag))
		return;									// no sector has this tag

	while (iSector != 0)
	{
		if (LiftIsActive (iSector))
		{
			iSector = TagToNextSector (iSector + 1, tag);
			continue;					// not return!
		}

		iLift = LiftFindUnusedRecord ();

		if (iLift >= MAX_LIFTS)
		{
#ifdef _DEBUG
			fatal_error ("Too many active lifts.");
#else
			return;
#endif
		}

		lifts[iLift].heightOrig = sectors[iSector].fh;

		if (special == LSP_LIFT_WI || special == LSP_LIFT_SI ||
			special == LSP_LIFT_WIC || special == LSP_LIFT_SIC)
		{
			lifts[iLift].goal = GetLowestFloorHeight (iSector, CAN_BE_THIS_SECTOR);
			if (lifts[iLift].goal < lifts[iLift].heightOrig)
			{
				// If goal is lower than current floor height, activate the
				// lift; else lift is already as low as it will go, so
				// do nothing.
				lifts[iLift].sector  = iSector;
				lifts[iLift].state   = LFCS_GOING_DOWN;
				lifts[iLift].speed   = SPEED_LIFT;	// pixels per frame
				lifts[iLift].fInUse  = TRUE;
				lifts[iLift].special = special;
				lifts[iLift].current = sectors[iSector].fh;
				lifts[iLift].iDummyThing = -1; // default

				fSuccess = FALSE;						// default
				if (DoorGetSectorCenter (iSector, &x, &y))
				{
					if ((lifts[iLift].iDummyThing = DoorCreateThing (x, y)) != -1)
					{
						fSuccess = TRUE;
						lifts[iLift].iSound = START_LIFT_SOUND (lifts[iLift].iDummyThing);
					}
				}
				if (!fSuccess)
					lifts[iLift].iSound  = START_LIFT_SOUND (-2);
			}
		}
		else if (special == LSP_LIFT_WA || special == LSP_LIFT_SA ||
			special == LSP_LIFT_WAC || special == LSP_LIFT_SAC)
		{
			lifts[iLift].goal = GetLowestFloorHeight (iSector, NOT_THIS_SECTOR);
			if (lifts[iLift].goal > lifts[iLift].heightOrig)
			{
				// If goal is higher than current floor height, activate the
				// lift; else lift is already as high as it will go, so
				// do nothing.
				lifts[iLift].sector  = iSector;
				lifts[iLift].state   = LFCS_GOING_UP;
				lifts[iLift].speed   = SPEED_LIFT;	// pixels per frame
				lifts[iLift].fInUse  = TRUE;
				lifts[iLift].special = special;
				lifts[iLift].current = sectors[iSector].fh;
				lifts[iLift].iDummyThing = -1; // default

				fSuccess = FALSE;						// default
				if (DoorGetSectorCenter (iSector, &x, &y))
				{
					if ((lifts[iLift].iDummyThing = DoorCreateThing (x, y)) != -1)
					{
						fSuccess = TRUE;
						lifts[iLift].iSound = START_LIFT_SOUND (lifts[iLift].iDummyThing);
					}
				}
				if (!fSuccess)
					lifts[iLift].iSound  = START_LIFT_SOUND (-2);
			}
		}
		else
		{
#if defined (_WINDOWS) && defined (_DEBUG)
			MessageBox (NULL, "LiftActivate: Invalid Lift Type", "Error", MB_OK);
#endif
			lifts[iLift].fInUse  = FALSE;
		}

		// For switches, change the wall texture to show the switch
		// in a different state.
		if (special == LSP_LIFT_SI || special == LSP_LIFT_SA)
		{
			if (!fChangedTexture)
			{
				// [d11-18-96 JPC] Only change the texture once!
				// Otherwise, if a switch controls an even number of
				// things, the texture will toggle back to where it was.
				DoorChangeSwitchTexture (iLinedef);
				fChangedTexture = TRUE;
			}
		}

		iSector = TagToNextSector (iSector + 1, tag);
	}
}


// ===========================================================================
// FLOOR CRUSHERS, PIT TRAPS, AND ELEVATORS (REVERSE PIT TRAPS).
//
// ---------------------------------------------------------------------------
static LONG FloorFindUnusedRecord ()
{
// [d8-06-96 JPC]
// Return first unused floor record.
// Return MAX_FLOORS if no unused records.
// ---------------------------------------------------------------------------

	LONG			iFloor;

	for (iFloor = 0; iFloor < MAX_FLOORS; iFloor++)
	{
		if (!floors[iFloor].fInUse)
			break;
	}
	return iFloor;
}


// ---------------------------------------------------------------------------
static void HandlePitTrap (LONG iFloor)
{
// [d8-06-96 JPC]
// Helper routine of HandleFloors.  Floor goes down a fixed amount and stays.
// ---------------------------------------------------------------------------
	
	if (floors[iFloor].current > floors[iFloor].goal)
	{
		floors[iFloor].current -= floors[iFloor].speed;
		sectors[floors[iFloor].sector].fh = floors[iFloor].current;
	}
	else
	{
		// Fully dropped.
		sectors[floors[iFloor].sector].fh = floors[iFloor].goal;
		if (floors[iFloor].iSound != fERROR)
		{
			STOP_FLOOR_SOUND (floors[iFloor].iSound);
			floors[iFloor].iSound = fERROR;
		}
		DoorCloseSound (floors[iFloor].iDummyThing);
		DoorRemoveThing (floors[iFloor].iDummyThing);
		floors[iFloor].fInUse = FALSE;
	}
	ChangeThingZ (floors[iFloor].sector);
}


// ---------------------------------------------------------------------------
static void HandleUnPitTrap (LONG iFloor)
{
// [d9-13-96 JPC]
// Helper routine of HandleFloors.  Floor goes up a fixed amount and stays.
// ---------------------------------------------------------------------------
	
	if (floors[iFloor].current < floors[iFloor].goal)
	{
		floors[iFloor].current += floors[iFloor].speed;
		sectors[floors[iFloor].sector].fh = floors[iFloor].current;
	}
	else
	{
		// Fully lifted.
		sectors[floors[iFloor].sector].fh = floors[iFloor].goal;
		if (floors[iFloor].iSound != fERROR)
		{
			STOP_FLOOR_SOUND (floors[iFloor].iSound);
			floors[iFloor].iSound = fERROR;
		}
		DoorCloseSound (floors[iFloor].iDummyThing);
		DoorRemoveThing (floors[iFloor].iDummyThing);
		floors[iFloor].fInUse = FALSE;
	}
	ChangeThingZ (floors[iFloor].sector);
}


// ---------------------------------------------------------------------------
static void HandleFloorType56 (LONG iFloor)
{
// [d8-06-96 JPC]
// Helper routine of HandleFloors.
// Type 56 = crusher.  Stays up.
// ---------------------------------------------------------------------------
	
	if (floors[iFloor].current < floors[iFloor].goal)
	{
		floors[iFloor].current += floors[iFloor].speed;
		sectors[floors[iFloor].sector].fh = floors[iFloor].current;
	}
	else
	{
		// Crushed!
		sectors[floors[iFloor].sector].fh = floors[iFloor].goal;
		if (floors[iFloor].iSound != fERROR)
		{
			STOP_FLOOR_SOUND (floors[iFloor].iSound);
			floors[iFloor].iSound = fERROR;
		}
		DoorCloseSound (floors[iFloor].iDummyThing);
		DoorRemoveThing (floors[iFloor].iDummyThing);
		floors[iFloor].fInUse = FALSE;
	}
	ChangeThingZ (floors[iFloor].sector);
}


// ---------------------------------------------------------------------------
static void HandleFloorType57 (LONG iFloor)
{
// [d8-06-96 JPC]
// Helper routine of HandleFloors.
// Type 57 = crusher.  Pauses, then returns to starting position.
// ---------------------------------------------------------------------------
	
	switch (floors[iFloor].state)
	{
		case LFCS_GOING_UP:
			if (floors[iFloor].current < floors[iFloor].goal)
			{
				floors[iFloor].current += floors[iFloor].speed;
				sectors[floors[iFloor].sector].fh = floors[iFloor].current;
			}
			else
			{
				// Crushed!
				sectors[floors[iFloor].sector].fh = floors[iFloor].goal;
				floors[iFloor].state = LFCS_PAUSED;
				floors[iFloor].cFramesWait = CRUSHER_DELAY;
				if (floors[iFloor].iSound != fERROR)
				{
					STOP_FLOOR_SOUND (floors[iFloor].iSound);
					floors[iFloor].iSound = fERROR;
				}
				DoorCloseSound (floors[iFloor].iDummyThing);
			}
			break;

		case LFCS_GOING_DOWN:
			if (floors[iFloor].current > floors[iFloor].heightOrig)
			{
				floors[iFloor].current -= floors[iFloor].speed;
				sectors[floors[iFloor].sector].fh = floors[iFloor].current;
			}
			else
			{
				sectors[floors[iFloor].sector].fh = floors[iFloor].heightOrig;
				if (floors[iFloor].iSound != fERROR)
				{
					STOP_FLOOR_SOUND (floors[iFloor].iSound);
					floors[iFloor].iSound = fERROR;
				}
				DoorCloseSound (floors[iFloor].iDummyThing);
				DoorRemoveThing (floors[iFloor].iDummyThing);
				floors[iFloor].fInUse = FALSE;
			}
			break;

		case LFCS_PAUSED:
			floors[iFloor].cFramesWait--;
			if (floors[iFloor].cFramesWait <= 0)
			{
				floors[iFloor].state = LFCS_GOING_DOWN;

				// Start the floor sound again if there is a dummy thing
				// attached to the floor.  If there is no dummy thing,
				// run silently.
				if (floors[iFloor].iDummyThing != -1)
					floors[iFloor].iSound = START_FLOOR_SOUND (floors[iFloor].iDummyThing);
			}
			break;
	}


	ChangeThingZ (floors[iFloor].sector);
}


// ---------------------------------------------------------------------------
void HandleFloors (LONG arg)
{
// [d8-06-96 JPC]
// Once a floor is activated, call this routine regularly to move it along.
// [d9-13-96 JPC] Added switch-activated floors.
// ---------------------------------------------------------------------------

	LONG iFloor;

#if defined (_EDIT) && !defined (_JPC)
	return;										// do NOT allow changes in EditWad
#endif

	if (fFreeze || fPause || gfInCombat)
		return;									// [d9-16-96 JPC]

	for (iFloor = 0; iFloor < MAX_FLOORS; iFloor++)
	{
		if (floors[iFloor].fInUse)
		{
			switch (floors[iFloor].special)
			{
            case LSP_PIT:              // pit trap
            case LSP_PIT_S:            // pit trap
					HandlePitTrap (iFloor);
					break;
				case LSP_UNPIT:				// reverse pit trap
				case LSP_UNPIT_S:				// reverse pit trap
					HandleUnPitTrap (iFloor);
					break;
				case LSP_FLOOR_W1:         // floor rises to crush the player--stays up
            case LSP_FLOOR_S1:         // floor rises to crush the player--stays up
					HandleFloorType56 (iFloor);
					break;
            case LSP_FLOOR_WR:         // floor rises to crush the player--returns
            case LSP_FLOOR_SR:         // floor rises to crush the player--returns
					HandleFloorType57 (iFloor);
					break;
			}
		}
	}
}


// ---------------------------------------------------------------------------
BOOL FloorIsActive (LONG iSector)
{
// [d9-11-96 JPC]
// Return TRUE if the floor in this sector is already active.
// ---------------------------------------------------------------------------

	LONG			iFloor;

	for (iFloor = 0; iFloor < MAX_FLOORS; iFloor++)
	{
		if (floors[iFloor].sector == iSector && floors[iFloor].fInUse)
			return TRUE;
	}
	return FALSE;
}


// ---------------------------------------------------------------------------
void FloorActivate (LONG iLinedef, LONG tag, LONG special)
{
// [d8-06-96 JPC]
// Call from BUMP to activate this type floor.
// Implemented as a switch statement to make it E-Z to add more types if
// we need them.
// ---------------------------------------------------------------------------

	LONG			iFloor;
	LONG			iSector;
	LONG			x, y;
	BOOL			fSuccess;
	BOOL			fChangedTexture = FALSE;

	if (!DoorGetTagSector (&iSector, tag))
		return;									// no sector has this tag

	while (iSector != 0)
	{
		if (FloorIsActive (iSector))
		{
			iSector = TagToNextSector (iSector + 1, tag);
			continue;
		}

		switch (special)
		{
         case LSP_PIT:                 // pit trap, walkover
			case LSP_PIT_S:               // pit trap, switch
         case LSP_UNPIT:               // reverse pit trap, walkover
			case LSP_UNPIT_S:             // reverse pit trap, switch
				iFloor = FloorFindUnusedRecord ();
				if (iFloor >= MAX_FLOORS)
				{
#ifdef _DEBUG
					fatal_error ("Too many active floors.");
#else
					return;
#endif
				}
				if (special == LSP_PIT || special == LSP_PIT_S)
				{
					floors[iFloor].goal = sectors[iSector].fh - PIT_TRAP_DROP;
				}
				else
				{
					floors[iFloor].goal = sectors[iSector].fh + PIT_TRAP_DROP;
					if (floors[iFloor].goal > sectors[iSector].ch - FLOOR_CRUSHER_GAP)
						floors[iFloor].goal = sectors[iSector].ch - FLOOR_CRUSHER_GAP;
				}
				floors[iFloor].sector  = iSector;
				floors[iFloor].speed   = SPEED_PIT_TRAP;	// pixels per frame
				floors[iFloor].fInUse  = TRUE;
				floors[iFloor].special = special;
				floors[iFloor].current = sectors[iSector].fh;
				floors[iFloor].iDummyThing = -1;

				fSuccess = FALSE;						// default
				if (DoorGetSectorCenter (iSector, &x, &y))
				{
					if ((floors[iFloor].iDummyThing = DoorCreateThing (x, y)) != -1)
					{
						fSuccess = TRUE;
						floors[iFloor].iSound = START_FLOOR_SOUND (floors[iFloor].iDummyThing);
					}
				}
				if (!fSuccess)
					floors[iFloor].iSound  = START_FLOOR_SOUND (-2);
				break;
	
         case LSP_FLOOR_W1:            // crusher (stays up)
			case LSP_FLOOR_WR:				// crusher (pauses, then comes down)
         case LSP_FLOOR_S1:            // crusher (stays up)
			case LSP_FLOOR_SR:				// crusher (pauses, then comes down)
				for (iFloor = 0; iFloor < MAX_FLOORS; iFloor++)
				{
					if (floors[iFloor].sector == iSector && floors[iFloor].fInUse)
					{
						// Floor is already active, bug out.
						return;
					}
				}
				iFloor = FloorFindUnusedRecord ();
				if (iFloor >= MAX_FLOORS)
				{
#ifdef _DEBUG
					fatal_error ("Too many active floors.");
#else
					return;
#endif
				}
				floors[iFloor].goal    = sectors[iSector].ch - FLOOR_CRUSHER_GAP;
				floors[iFloor].sector  = iSector;
				floors[iFloor].speed   = SPEED_CRUSHER;	// pixels per frame
				floors[iFloor].fInUse  = TRUE;
				floors[iFloor].special = special;
				floors[iFloor].current = sectors[iSector].fh;
				floors[iFloor].iDummyThing = -1;
				fSuccess = FALSE;						// default
				if (DoorGetSectorCenter (iSector, &x, &y))
				{
					if ((floors[iFloor].iDummyThing = DoorCreateThing (x, y)) != -1)
					{
						fSuccess = TRUE;
						floors[iFloor].iSound = START_FLOOR_SOUND (floors[iFloor].iDummyThing);
					}
				}
				if (!fSuccess)
					floors[iFloor].iSound  = START_FLOOR_SOUND (-2);
				if (special == LSP_FLOOR_WR || special == LSP_FLOOR_SR)
				{
					// Repeatable.
					floors[iFloor].state      = LFCS_GOING_UP;
					floors[iFloor].heightOrig = sectors[iSector].fh;
				}
				break;
	
		}

		// For switches, change the wall texture to show the switch
		// in a different state.
		if (special == LSP_PIT_S || special == LSP_UNPIT_S || special == LSP_FLOOR_S1 || special == LSP_FLOOR_SR)
		{
			if (!fChangedTexture)
			{
				// [d11-18-96 JPC] Only change the texture once!
				// Otherwise, if a switch controls an even number of
				// things, the texture will toggle back to where it was.
				DoorChangeSwitchTexture (iLinedef);
				fChangedTexture = TRUE;
			}
		}

		iSector = TagToNextSector (iSector + 1, tag);
	}
}


// ===========================================================================
// CEILING CRUSHERS
//
// ---------------------------------------------------------------------------
static LONG CeilingFindUnusedRecord ()
{
// [d8-06-96 JPC]
// Return first unused ceiling record.
// Return MAX_CEILINGS if no unused records.
// ---------------------------------------------------------------------------

	LONG			iCeiling;

	for (iCeiling = 0; iCeiling < MAX_CEILINGS; iCeiling++)
	{
		if (!ceilings[iCeiling].fInUse)
			break;
	}
	return iCeiling;
}


// ---------------------------------------------------------------------------
static void HandleCeilingType6 (LONG iCeiling)
{
// [d8-06-96 JPC]
// Helper routine of HandleCeilings.
// Type 6 = non-repeatable crusher.
// ---------------------------------------------------------------------------
	
	if (ceilings[iCeiling].current > ceilings[iCeiling].goal)
	{
		ceilings[iCeiling].current -= ceilings[iCeiling].speed;
		sectors[ceilings[iCeiling].sector].ch = ceilings[iCeiling].current;
	}
	else
	{
		// Crushed!
		sectors[ceilings[iCeiling].sector].ch = ceilings[iCeiling].goal;
		if (ceilings[iCeiling].iSound != fERROR)
		{
			STOP_CEILING_SOUND (ceilings[iCeiling].iSound);
			ceilings[iCeiling].iSound = fERROR;
		}
		DoorCloseSound (ceilings[iCeiling].iDummyThing);
		DoorRemoveThing (ceilings[iCeiling].iDummyThing);
		ceilings[iCeiling].fInUse = FALSE; // release the record
	}
}



// ---------------------------------------------------------------------------
static void HandleCeilingType7 (LONG iCeiling)
{
// [d8-06-96 JPC]
// Helper routine of HandleCeilings.
// Handle the repeatable ceiling crusher.
// ---------------------------------------------------------------------------
	
	switch (ceilings[iCeiling].state)
	{
		case LFCS_GOING_DOWN:
			if (ceilings[iCeiling].current > ceilings[iCeiling].goal)
			{
				ceilings[iCeiling].current -= ceilings[iCeiling].speed;
				sectors[ceilings[iCeiling].sector].ch = ceilings[iCeiling].current;
			}
			else
			{
				sectors[ceilings[iCeiling].sector].ch = ceilings[iCeiling].goal;
				ceilings[iCeiling].state = LFCS_PAUSED;
				ceilings[iCeiling].cFramesWait = CRUSHER_DELAY;
				if (ceilings[iCeiling].iSound != fERROR)
				{
					STOP_CEILING_SOUND (ceilings[iCeiling].iSound);
					ceilings[iCeiling].iSound = fERROR;
				}
				DoorCloseSound (ceilings[iCeiling].iDummyThing);
			}
			break;

		case LFCS_GOING_UP:
			if (ceilings[iCeiling].current < ceilings[iCeiling].heightOrig)
			{
				ceilings[iCeiling].current += ceilings[iCeiling].speed;
				sectors[ceilings[iCeiling].sector].ch = ceilings[iCeiling].current;
			}
			else
			{
				sectors[ceilings[iCeiling].sector].ch = ceilings[iCeiling].heightOrig;
				if (ceilings[iCeiling].iSound != fERROR)
				{
					STOP_CEILING_SOUND (ceilings[iCeiling].iSound);
					ceilings[iCeiling].iSound = fERROR;
				}
				DoorCloseSound (ceilings[iCeiling].iDummyThing);
				DoorRemoveThing (ceilings[iCeiling].iDummyThing);
				ceilings[iCeiling].fInUse = FALSE;
			}
			break;

		case LFCS_PAUSED:
			ceilings[iCeiling].cFramesWait--;
			if (ceilings[iCeiling].cFramesWait <= 0)
			{
				ceilings[iCeiling].state = LFCS_GOING_UP;
				if (ceilings[iCeiling].iDummyThing != -1)
					ceilings[iCeiling].iSound = START_CEILING_SOUND (ceilings[iCeiling].iDummyThing);
			}
			break;
	}
}



// ---------------------------------------------------------------------------
void HandleCeilings (LONG arg)
{
// [d8-06-96 JPC]
// Once a ceiling is activated, call this routine regularly to move it along.
// ---------------------------------------------------------------------------

	LONG iCeiling;

#if defined (_EDIT) && !defined (_JPC)
	return;										// do NOT allow changes in EditWad
#endif

	if (fFreeze || fPause || gfInCombat)
		return;									// [d9-16-96 JPC]

	for (iCeiling = 0; iCeiling < MAX_CEILINGS; iCeiling++)
	{
		if (ceilings[iCeiling].fInUse)
		{
			switch (ceilings[iCeiling].special)
			{
            case LSP_CEIL_W1:          // ceiling drops to crush the player, stays down
            case LSP_CEIL_S1:          // ceiling drops to crush the player, stays down
					HandleCeilingType6 (iCeiling);
					break;

				case LSP_CEIL_WR:          // ceiling drops to crush the player, then goes back up
            case LSP_CEIL_SR:          // ceiling drops to crush the player, then goes back up
					HandleCeilingType7 (iCeiling);
					break;
			}
		}
	}
}


// ---------------------------------------------------------------------------
BOOL CeilingIsActive (LONG iSector)
{
// [d9-11-96 JPC]
// Return TRUE if the ceiling in this sector is already active.
// ---------------------------------------------------------------------------

	LONG			iCeiling;

	for (iCeiling = 0; iCeiling < MAX_CEILINGS; iCeiling++)
	{
		if (ceilings[iCeiling].sector == iSector && ceilings[iCeiling].fInUse)
			return TRUE;
	}
	return FALSE;
}


// ---------------------------------------------------------------------------
void CeilingActivate (LONG iLinedef, LONG tag, LONG special)
{
// [d8-06-96 JPC]
// Call from BUMP to activate this type ceiling.
// [d9-13-96 JPC] Added switch activation.
// ---------------------------------------------------------------------------

	LONG			iCeiling;
	LONG			iSector;
	LONG			x, y;
	BOOL			fSuccess;
	BOOL			fChangedTexture = FALSE;

#if defined (_EDIT) && !defined (_JPC)
	return;										// do NOT allow changes in EditWad
#endif

	if (!DoorGetTagSector (&iSector, tag))
		return;									// no sector has this tag

	while (iSector != 0)
	{
		if (CeilingIsActive (iSector))
		{
			iSector = TagToNextSector (iSector + 1, tag);
			continue;
		}

		iCeiling = CeilingFindUnusedRecord ();
		if (iCeiling >= MAX_CEILINGS)
		{
#ifdef _DEBUG
			fatal_error ("Too many active ceilings.");
#else
			return;
#endif
		}
		ceilings[iCeiling].goal    = sectors[iSector].fh + CEILING_CRUSHER_GAP;
		ceilings[iCeiling].sector  = iSector;
		ceilings[iCeiling].speed   = SPEED_CRUSHER;	// pixels per frame
		ceilings[iCeiling].fInUse  = TRUE;
		ceilings[iCeiling].special = special;
		ceilings[iCeiling].current = sectors[iSector].ch;
		ceilings[iCeiling].iDummyThing = -1;

		fSuccess = FALSE;						// default
		if (DoorGetSectorCenter (iSector, &x, &y))
		{
			if ((ceilings[iCeiling].iDummyThing = DoorCreateThing (x, y)) != -1)
			{
				fSuccess = TRUE;
				ceilings[iCeiling].iSound = START_CEILING_SOUND (ceilings[iCeiling].iDummyThing);
			}
		}
		if (!fSuccess)
			ceilings[iCeiling].iSound  = START_CEILING_SOUND (-2);
		
		if (special == LSP_CEIL_WR || special == LSP_CEIL_SR)
		{
			ceilings[iCeiling].state      = LFCS_GOING_DOWN;
			ceilings[iCeiling].heightOrig = sectors[iSector].ch;
		}

		// For switches, change the wall texture to show the switch
		// in a different state.
		if (special == LSP_CEIL_S1 || special == LSP_CEIL_SR)
		{
			if (!fChangedTexture)
			{
				// [d11-18-96 JPC] Only change the texture once!
				// Otherwise, if a switch controls an even number of
				// things, the texture will toggle back to where it was.
				DoorChangeSwitchTexture (iLinedef);
				fChangedTexture = TRUE;
			}
		}

		iSector = TagToNextSector (iSector + 1, tag);
	}
}


// ---------------------------------------------------------------------------
BOOL LinedefIsSwingingDoor(LONG Ldidx)
{
	LINEDEF Ld=linedefs[Ldidx];

	if (Ld.special==LSP_SWDOOR_L || Ld.special==LSP_SWDOOR_R)
		return TRUE;
	else if((Ld.special >= LSP_SWDOOR_L_SILVER_KEY &&
			Ld.special <= LSP_SWDOOR_R_KEY_STONE)
			||
			(Ld.special >= LSP_SWDOOR_L_GEM_BLUE &&
			 	Ld.special <= LSP_SWDOOR_R_PASSAGE))	
		return TRUE;
	return FALSE;
}


// ---------------------------------------------------------------------------
void DoorInfo (LONG special)
{
// [d1-30-97 JPC] Tell users why switch-activated and walkover-activated doors
// won't open.  Called from SECTORS: activate_seg().
// ---------------------------------------------------------------------------

	char			szTemp[128];

	if (special == LSP_DOOR_INFO_WALKOVER)
		sprintf (szTemp, STRMGR_GetStr (STR_DOOR_INFO_WALKOVER));
	else
		sprintf (szTemp, STRMGR_GetStr (STR_DOOR_INFO_SWITCH));
	TitledMessage (STRMGR_GetStr (STR_DOOR_LOCK_TITLE), szTemp);
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
