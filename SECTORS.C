/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: SECTORS.C   -Handles Sectors in Nova
   Author: Chris Phillips
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:
   raise_sector_floor    -raises the floor of a Sector
   raise_sector_ceiling  -raises the cieling of a Sector
   point_to_sector_info  - get floor, ceiling, special and tag info
   point_to_sector       -???
   point_to_floor_height -???
   point_to_ceiling_height -???
   point_to_sector_special - return the special value for sector at point
   find_ssector          -???
   point_to_ssector      -???
   tag_to_sector         -finds a sector with the specified tag
   activate_seg          -activates a linedef

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#if !defined(_RELEASE)
#include <stdio.h>
#endif
#if defined (_EDIT)
#include <windows.h>
#endif
#if defined (_JPC)
#include "winsys\status.h"
void WriteDebug (const char *format, ... );
#endif
#include "SYSTEM.H"
#include "ENGINE.H"
#include "ENGINT.H"
#include "MACHINE.H"
#include "SPECIAL.H"
#include "LIGHT.H"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
// MAX_ACTIVATION_DISTANCE is the maximum distance (in map units)
// that a line can be away from the player and still be activated.
#define MAX_ACTIVATION_DISTANCE  100
#define DAMAGE_FRAME					5
#define LAVA_DAMAGE 					1
#define ACID_DAMAGE 					1
#define POISON_DAMAGE 				1

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
static ULONG point_ssector;
static LONG point_ssector_x,point_ssector_y;
// GWP I Don't see the use of this variable.
//     static ULONG ptss_last_front;
static void point_to_ssector(LONG n);

extern LONG first_seg;
extern BOOL	gfWadLoaded;
								
//short sInside = TRUE;

/* =======================================================================
   Function    - raise_sector_floor
   Description - raises the floor of a sector
   Returns     - the new floor height
   ======================================================================== */
LONG raise_sector_floor(LONG s,LONG inc)
{
	sectors[s].fh+=(SHORT)inc;
	return(sectors[s].fh);
}

/* =======================================================================
   Function    - raise_sector_ceiling
   Description - raises the ceiling of a sector
   Returns     - the new ceiling height
   ======================================================================== */
LONG raise_sector_ceiling(LONG s,LONG inc)
{
	sectors[s].ch+=(SHORT)inc;
	return(sectors[s].ch);
}

/* =======================================================================
   Function    - point_to_sector
   Description - ???
   Returns     - a ULONG
   ======================================================================== */
ULONG point_to_sector(LONG x,LONG y)
{
	LONG	sector, floor, ceiling, special, tag;
	point_to_sector_info( x, y, &sector, &floor, &ceiling, &special, &tag);
	return sector;
}

/* =======================================================================
   Function    - point_to_sector_special
   Description - return the special from the sector at point
   Returns     - LONG
   ======================================================================== */
LONG point_to_sector_special(LONG x, LONG y)
{
	LONG	sector, floor, ceiling, special, tag;
	point_to_sector_info( x, y, &sector, &floor, &ceiling, &special, &tag);
	return special;
}

/* =======================================================================
   Function    - point_to_sector_info
   Description - return the floor, ceiling, special, and tag information
                 for the sector at the given point
   Returns     - void
   ======================================================================== */
void point_to_sector_info(
	LONG x,
	LONG y,
	LONG *sector,
	LONG *floor,
	LONG *ceiling,
	LONG *special,
	LONG *tag
)
{
	point_ssector_x=x;
	point_ssector_y=y;
	
	point_to_ssector(tot_nodes-1);
	
	ssector_to_sector_info(x, y, point_ssector, sector, floor, ceiling, special, tag);
}
/* =======================================================================
   Function    - ssector_to_sector_info
   Description - return the floor, ceiling, special, and tag information
                 for the sector at the given ssector
   Returns     - void
   ======================================================================== */
void ssector_to_sector_info(
	LONG x,
	LONG y,
	ULONG PointSsector,
	LONG *sector,
	LONG *floor,
	LONG *ceiling,
	LONG *special,
	LONG *tag
)
{
	LONG sa,sb,sx,sy;
	LONG sided;
	LONG const seg=ssectors[PointSsector].o;
	SEG * pSeg = &segs[seg];				// [d2-28-97 JPC] optimization
	
#if 0	// GWP doesn't seem to be the problem.
#if defined (_DEBUG)
	if (seg < 0 || seg >= tot_segs)
	{
		fatal_error("ERROR - seg %ld\n", seg);
	}
	else
	if (segs[seg].a < 0 || segs[seg].a >= tot_vertexs ||
	    segs[seg].b < 0 || segs[seg].b >= tot_vertexs)
	{
		printf("ERROR segs[seg].a = %ld\n", segs[seg].a);
		fatal_error("ERROR segs[seg].b = %ld\n", segs[seg].b);
	}
#endif
#endif
	
	if(pSeg->flip)
	{
		sx=vertexs[pSeg->b].x;
		sy=vertexs[pSeg->b].y;
		sa=vertexs[pSeg->a].x-sx;
		sb=vertexs[pSeg->a].y-sy;
	}
	else
	{
		sx=vertexs[pSeg->a].x;
		sy=vertexs[pSeg->a].y;
		sa=vertexs[pSeg->b].x-sx;
		sb=vertexs[pSeg->b].y-sy;
	}
	
	{
	LONG const r= ( sa *( y-sy ) ) - ( ( x-sx) * sb );

	//if(r<0)
	//	return(seg_to_height(seg,FRONT));
	//else
	//	return(seg_to_height(seg,BACK));
	
	{
	LONG const l=(LONG)pSeg->lptr;

	if(r<0)
		sided=(LONG)linedefs[l].psdb;
	else
	{
		sided=(LONG)linedefs[l].psdt;
		// We're on the back side of a one sided line.
		if (sided == -1)
		{
			sided = (LONG) linedefs[l].psdb;
		}
	}
	}
	}
	
	{
	LONG const sec=(LONG)sidedefs[sided].sec_ptr;
	
	*sector  = (LONG)sec;
	*floor   = (LONG)sectors[sec].fh;
	*ceiling = (LONG)sectors[sec].ch;
	*special = (LONG)sectors[sec].special;
	*tag     = (LONG)sectors[sec].tag;
	}
}

/* =======================================================================
   Function    - sector_info
   Description - return the floor, ceiling, special, and tag information
                 at the given sector
   Returns     - void
   ======================================================================== */
void sector_info(
	LONG sec,
	LONG *floor,
	LONG *ceiling,
	LONG *special,
	LONG *tag
)
{
	*floor   = (LONG)sectors[sec].fh;
	*ceiling = (LONG)sectors[sec].ch;
	*special = (LONG)sectors[sec].special;
	*tag     = (LONG)sectors[sec].tag;
}
/* =======================================================================
   Function    - point_to_floor_height
   Description - ???
   Returns     - a LONG
   ======================================================================== */
LONG point_to_floor_height(LONG x,LONG y)
{
	LONG	sector, floor, ceiling, special, tag;
	point_to_sector_info( x, y, &sector, &floor, &ceiling, &special, &tag);
	return floor;
}

/* =======================================================================
   Function    - point_to_ceiling_height
   Description - ???
   Returns     - a LONG
   ======================================================================== */
LONG point_to_ceiling_height(LONG x,LONG y)
{
	LONG	sector, floor, ceiling, special, tag;
	point_to_sector_info( x, y, &sector, &floor, &ceiling, &special, &tag);
	return ceiling;
}

/* =======================================================================
   Function    - find_ssector
   Description - ???
   Returns     - a ULONG
   ======================================================================== */
ULONG find_ssector(LONG x,LONG y)
{
	point_ssector_x=x;
	point_ssector_y=y;
	point_to_ssector(tot_nodes-1);
	return(point_ssector);
}

/* =======================================================================
   Function    - point_to_ssector
   Description - from a given x,y coordinate passed in by the static vars
                 point_ssector_x and point_ssector_y find the ssector.

                 This was a very clean recursive fn. I've totally messed it
                 up with these ugly goto's in the name of speed. -GWP-
   Returns     - void
   ======================================================================== */
void point_to_ssector(LONG n)
{

The_Begining:
	if(n&0x8000)       /*if ssector*/
	{
		point_ssector=(n&0xfff);
		return;
	}
	if(point_relation(n,point_ssector_x,point_ssector_y)==FRONT)
	{
		// GWP ptss_last_front doesn't appear to have a use at this time.
		// ptss_last_front=TRUE;
		//point_to_ssector(nodes[n].f);
		
		n = nodes[n].f;
		goto The_Begining;
		
	}
	else
	{
		// ptss_last_front=FALSE;
		// point_to_ssector(nodes[n].r);
		
		n = nodes[n].r;
		goto The_Begining;
		
	}
}

/* =======================================================================
   Function    - tag_to_sector
   Description - finds a sector with the tag specified
   Returns     - LONG
   ======================================================================== */
LONG tag_to_sector(LONG tag)
{
	LONG i;

	for(i=0;i<(LONG)tot_sectors;++i)
		{
		if(sectors[i].tag==tag)
			return(i);
		}
	return(0);
}

/* =======================================================================
   Function    - TagToNextSector
   Description - finds a sector with the tag specified, starting with
						sector "iStart."  This lets us find multiple sectors
						with the same tag, which allows a single linedef to
						trigger multiple sectors for lifts, etc.
   Returns     - Next tagged sector, 0 if no sector found.
					  This means that sector 0, if there is one, CANNOT be
					  a tagged sector.
   ======================================================================== */
LONG TagToNextSector (LONG iStart, LONG tag)
{
	LONG 			iSector;

	for (iSector = iStart; iSector < (LONG)tot_sectors; ++iSector)
	{
		if (sectors[iSector].tag == tag)
			return iSector;
	}
	return 0;
}

/* =======================================================================
   Function    - tag_to_line
   Description - finds a line with the tag specified
   Returns     - LONG
   ======================================================================== */
LONG tag_to_line (LONG tag, LONG notline)
{
	LONG i;

	for(i=0; i<(LONG)tot_linedefs; ++i)
		if (i!=notline && linedefs[i].tag==tag)
			return(i);

	return(0);
}


/* =======================================================================
   Function    - DistanceToSeg
   Description - determines distance squared from a point to a line segment
   Returns     - the distance squared
	Notes       - using the distance squared instead of the actual distance
						lets us avoid taking a square root.  Just make sure all distance
						comparisons that use this function are aware that the return
						value is squared.
   ======================================================================== */
static LONG DistanceToSeg (LONG seg, LONG x, LONG y)
{
	return LinePointDistance (x, y, vertexs[segs[seg].a].x, vertexs[segs[seg].a].y,
		vertexs[segs[seg].b].x, vertexs[segs[seg].b].y);
}


/* =======================================================================
   Function    - activate_seg
   Description - activates a linedef
   Returns     - void
	Notes			- Called from MAIN: ActivateSegment
					- default activation key is the spacebar (see MAIN: AddGameKeys,
					  which hooks the spacebar up to ActivateSegment.
					- [d8-13-96 JPC] Do not activate the segment if it is too
					  far away.
					- walkover activations are handled in BUMP.C: CheckLinesInBlock.
					- switch animations are handled in DOOR.C in various places,
					  as soon as we know that the switch did activate something
   ======================================================================== */
void activate_seg(LONG DistanceSquared)
{
	LONG special;
	LONG iLinedef;
	
#if !defined(_RELEASE)
	printf("trying to activate seg (maxdist squared= %li)\n",DistanceSquared);
#endif

	// JPC Note:
	// first_seg is a global set in RENDER: process_seg.  It's the first
	// segment rendered whose linedef has a non-zero "special" field.
	// If first_seg is 0, then there was no "activatable" segment drawn
	// in the last render.
	// RULE: line segment 0 CANNOT have an activation.
	// It might be tricky to enforce this rule, since the editors generate
	// the segments from the linedefs and don't let us edit them directly.
	if (first_seg == 0)
		return;
	
	if (DistanceToSeg (first_seg, player.x >> PLAYER_FIXEDPT, player.y >> PLAYER_FIXEDPT)
		> DistanceSquared)
		return;

	iLinedef=segs[first_seg].lptr;
	special=linedefs[iLinedef].special & 0x7fff;

	switch(special)
	{
		// Doors.
		// Note: walkover activations are handled in BUMP.C: CheckLinesInBlock ().
		case LSP_DOOR_DMS:
		case LSP_SWDOOR_L:
		case LSP_SWDOOR_R:
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

		// [d9-13-96 JPC] New arrivals:
		case LSP_GEM_BLUE:
		case LSP_GEM_GREEN:
		case LSP_GEM_OF_PASSAGE:
		case LSP_SWDOOR_L_GEM_BLUE:
		case LSP_SWDOOR_L_GEM_GREEN:
		case LSP_SWDOOR_L_PASSAGE:
		case LSP_SWDOOR_R_GEM_BLUE:
		case LSP_SWDOOR_R_GEM_GREEN:
		case LSP_SWDOOR_R_PASSAGE:
		case LSP_DOOR_INACCESSIBLE:
			open_door(first_seg, special);	// [d7-24-96 JPC] added "special" param
			break;
		case LSP_DOOR_S:
		case LSP_DOOR_SF:
			DoorActivate (iLinedef, linedefs[iLinedef].tag, special);
			break;

		// [d1-30-97 JPC] Added these cases so users can get feedback on
		// why a door isn't opening.
		case LSP_DOOR_INFO_WALKOVER:			//	151
		case LSP_DOOR_INFO_SWITCH:				//	152
			DoorInfo (special);
			break;

		// case 51:
		case LSP_TELE_EXIT_2WAY:
			//GEH do not do this in Birthright.  If you do this, the
			// mythings array will change and blow the avatar list
			// right out of the water.  If you want into another wad
			// set the fExitLevel value to the Player start location
			// and let the scene load itself
			//if(sInside)
			//	load_new_wad("new2.wad",1);
			//else
			//	load_new_wad("castle86.wad",1);
			//sInside = !sInside;
			break;
		
		case LSP_TELE_1WAY:
#if !defined(_RELEASE)
			printf("teleporting to sector %li\n",tag_to_sector(linedefs[iLinedef].tag));
#endif
			TeleportPlayer(tag_to_sector(linedefs[iLinedef].tag));
			break;

		case LSP_CEIL_S1:
		case LSP_CEIL_SR:
			CeilingActivate (iLinedef, linedefs[iLinedef].tag, special);
			break;
	
		case LSP_LIFT_SI:       // lift
		case LSP_LIFT_SA:       // lift
		case LSP_LIFT_SIC:      // lift
		case LSP_LIFT_SAC:      // lift
			LiftActivate (iLinedef, linedefs[iLinedef].tag, special);
			break;

		case LSP_PIT_S:         // pit trap
      case LSP_UNPIT_S:       // reverse pit trap
		case LSP_FLOOR_S1:      // floor up to ceiling (crush)
		case LSP_FLOOR_SR:      // floor up to ceiling (crush)
			FloorActivate (iLinedef, linedefs[iLinedef].tag, special);
			break;
	}
}

#if 0
// [d11-14-96 JPC] Removed this function.  Use GetCrusherDamage instead.
/* =======================================================================
   Function    - GetSectorDamage
   Returns     - TRUE if there was damage from this sector, FALSE if not.
						If there was damage, it is returned in the output-only
						parameter damageNumber.
	Notes			- Avatars should call this function every x frames and
						use the return information to assign damage.
   ======================================================================== */
BOOL GetSectorDamage (LONG iSector, LONG *damageNumber)
{
// [d11-13-96 JPC] Relocated this code to GetCrusherDamage.
// No one should call this routine anymore.
	LONG			damage;

	damage = 0;

	switch (sectors[iSector].special)
	{
		case SSP_DAMAGE_2:
			damage = 2;
			break;
		case SSP_DAMAGE_5:
			damage = 5;
			break;
		case SSP_DAMAGE_10:
			damage = 10;
			break;
		case SSP_LAVA:
			damage = LAVA_DAMAGE;
			break;
		case SSP_POISON_GAS:
			damage = POISON_DAMAGE;
			break;
		case SSP_ACID_FLOOR:
			damage = ACID_DAMAGE;
			break;
	}

	*damageNumber = damage;
	if (damage)
		return TRUE;
	else
		return FALSE;
	return FALSE;
}
#endif

#if 0
// [d11-29-96 JPC] Abolished.  Use AVATAR: GetSectorDamage instead.
/* =======================================================================
   Function    - GetCrusherDamage
   Returns     - TRUE if you have been crushed.
						If there was damage, it is returned in the output-only
						parameter damageNumber.
	Notes			- Avatars should call this function every pass through
						the animation loop.
					- 10-21-96 JPC: check to make sure it's a crusher sector
					  before doing damage.
					- 11-21-96 JPC: no damage in lava or acid sectors if avatar
					  is above the floor.  Tried to use parameter avatarZ, but
					  move_thing_to will change the z coordinate of a party member
					  to the floor (move_thing_to is not aware that we are flying).
					  Used player.z instead.  This means that if we are above the
					  floor, nothing in the whole game takes damage from lava or
					  acid.  This should actually be OK, because no monster should
					  ever be in acid or lava.
					  (I couldn't figure out how to get the information about
					  flying to move_thing_to; the IsFlyingThing test only
					  works for things that are intrinsically able to fly.)
   ======================================================================== */
BOOL GetCrusherDamage (LONG iSector, LONG *damageNumber, LONG avatarHeight, LONG avatarZ)
{
	LONG			damage;
	LONG			sectorHeight;

	damage = 0;
	sectorHeight = sectors[iSector].ch - sectors[iSector].fh;

	// Simple rule: once the ceiling touches the avatar's head, the
	// avatar is dead.  We can make this more complicated if we want to.
	if (sectorHeight < avatarHeight)
	{
		// Check for various crushers.
		if (FloorIsActive (iSector) || CeilingIsActive (iSector))
			damage = 200;							// should be enough to kill
	}

	// [d11-13-96 JPC] Move the following code here from GetSectorDamage:
	// Hand out special damage every DAMAGE_FRAME frames.
	if ((gcFrames % DAMAGE_FRAME) == 0)
	{
		switch (sectors[iSector].special)
		{
			case SSP_DAMAGE_2:
				damage += 2;
				break;
			case SSP_DAMAGE_5:
				damage += 5;
				break;
			case SSP_DAMAGE_10:
				damage += 10;
				break;
			case SSP_LAVA:
				// if (avatarZ <= sectors[iSector].fh)
				if (player.z <= sectors[iSector].fh)
					damage += LAVA_DAMAGE;
				break;
			case SSP_POISON_GAS:
				damage += POISON_DAMAGE;
				break;
			case SSP_ACID_FLOOR:
				// if (avatarZ <= sectors[iSector].fh)
				if (player.z <= sectors[iSector].fh)
					damage += ACID_DAMAGE;
				break;
		}
	}

	*damageNumber = damage;

	if (damage)
		return TRUE;
	else
		return FALSE;
}
#endif

// ---------------------------------------------------------------------------
BOOL IsSplashSector (LONG iSector)
{
// [d11-14-96 JPC] Return TRUE if sector is water, deep water, or acid.
// Purpose: let us know whether to make splash noises.
// ---------------------------------------------------------------------------

	SHORT			special;

	special = sectors[iSector].special;
	return (special == SSP_WATER || special == SSP_DEEP_WATER || special == SSP_ACID_FLOOR);
}


// ---------------------------------------------------------------------------
BOOL IsLavaSector (LONG iSector)
{
// [d11-14-96 JPC] Return TRUE if sector is lava.
// Purpose: let us know whether to make lava noise.
// ---------------------------------------------------------------------------

	return (sectors[iSector].special == SSP_LAVA);
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

