/* =======================================================================
   Copyright (c) 1990,1995	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename:  BUMP.C--handles bumping into things or walls
   Author:    Chris Phillips & Wes Cumberland (in PLAYER.C)
   			  total rewrite 4-18-96 GEH (in PLAYER.C)
              extensive revision 5-09-96 JPC (created BUMP.C)
   ========================================================================
   Contains the following functions:
   CheckBump        -checks for bump with objects
   CheckLineMove    -checks for linedef collisions
   CheckMove        -checks for collision with objects or lines

   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
	------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "debug.h"

#include "system.h"
#include "engine.h"
#include "engint.h"
#include "player.hxx"
#include "special.h"

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

#if defined (_STATUS)
#include "winsys\status.h"
void WriteDebug (const char *format, ... );
#else
	#define WriteDebug 1?0:printf
#endif

#define UNIT_DISTANCE 	100
#define ANGLE_STEPS 		256
#define ANGLE_MULTI 		1024

// globals from things.c

// Flag that we've teleported.
static BOOL fTeleport = FALSE;
// Line ends of linedef we bumped.
static POINT gbumpV1;
static POINT gbumpV2;
static LONG gFloor;		// if we bump the floor this is set to that floor height.
static LONG gCeiling;
static LONG gBumpDistance;

// GWP HACK To put up dialogs in the game.
LONG	gDialogFlag = 0;

/* ------------------------------------------------------------------------
   Prototypes of static fns.
	------------------------------------------------------------------------ */
static WadThingType CheckLinesInBlock (const PLAYER * const pPlayer,
							   SHORT sSpecial,
							   SHORT * bm,
   							   LONG x1, LONG y1,
   							   LONG x2, LONG y2,
   							   LONG minDist);
static WadThingType CheckLineMove (PLAYER *pPlayer,
						   FIXED_POINT_3D *pPoint,
						   BOOL sSpecial,
						   LONG minDist);

// ---------------------------------------------------------------------------
#if defined (_DEBUG)
int	gfOldTest;								// [d2-26-97 JPC] debug walk through wall error
int	gfOldTestLine;
// #define _BUMPLOG
#endif
#ifdef _BUMPLOG
struct bumpRecordType {
	PLAYER * 		pPlayer;
	LONG				thingIndex;
	LONG				x;
	LONG				y;
	LONG				x1;
	LONG				y1;
	LONG				x2;
	LONG				y2;
	BOOL				fValidBlock;
	LONG				iLinedef;
	WadThingType 	bump;
	LONG				caller;		// 0 = CheckMove, 1 = CheckMoveSimple
} gBumpRecord[100];

LONG giBumpRec = 0;
LONG gPlayerx;
LONG gPlayery;
LONG gDebug = 0;
LONG gCallingFunction = 0;

// [d12-12-96 JPC] Keep track of what we bumped
void UpdateBumpRecord (PLAYER * pPlayer, LONG x1, LONG y1, LONG x2, LONG y2,
	BOOL fValidBlock, LONG iLinedef, WadThingType bump)
{
	gBumpRecord[giBumpRec].pPlayer	  = pPlayer;
	gBumpRecord[giBumpRec].thingIndex  = pPlayer->ThingIndex;
	gBumpRecord[giBumpRec].x			  = gPlayerx;
	gBumpRecord[giBumpRec].y           = gPlayery;
	gBumpRecord[giBumpRec].x1          = x1;
	gBumpRecord[giBumpRec].y1          = y1;
	gBumpRecord[giBumpRec].x2          = x2;
	gBumpRecord[giBumpRec].y2          = y2;
	gBumpRecord[giBumpRec].fValidBlock = fValidBlock;
	gBumpRecord[giBumpRec].iLinedef    = iLinedef;
	gBumpRecord[giBumpRec].bump        = bump;
	gBumpRecord[giBumpRec].caller		  = gCallingFunction;

	if (++giBumpRec >= 100)
		giBumpRec = 0;
}
#endif

/* ========================================================================
   Function    - CheckBump
   Description - checks for bump of an object at point
   				 NOTE: This routine does use 24.8 fixed point dx and dy
   				       and a LONG for dz.
   Returns     - The type for colisions, 0 if no bump.
                 Also the player structure contains the bumped object information.
   ======================================================================== */
LONG CheckBump (																					// )
PLAYER *pPlayer,
FIXED_VECTOR_3D *pPoint,		// This is the delta of the motion not the absolute location.
LONG Rate						// Motion rate for this player.
)
{
	LONG	i;
	LONG	distance;
	const LONG	PlayerHeadHeight = pPlayer->z + pPlayer->h;
	const LONG	PlayerHalfWidth = pPlayer->w/2;
	LONG		ThingHalfWidth;
	LONG		ThingHeadHeight;
	
		
	pPlayer->bump = iNOTHING;
	pPlayer->BumpIndex = fERROR;
	
	for (i = MaxThingLoaded; i > 0; i--)
	{
		if( i == pPlayer->ThingIndex ||		// Not me
		    mythings[i].valid == FALSE  ||  // Valid thing.
			!IsBumpable(i)				||	// Not bumpable.
			mythings[i].iBitm == fERROR	||	// No valid bitmap.
			reject(mythings[i].sect, pPlayer->currentSector) == FALSE
		  )
		{
			continue;
		}

		
		// if this is a dead body, just skip it
		if (mythings[i].iSequence == ANIMATION12SEQ)
			continue;
		
		// if we are (player width/2) + (Thing width/2) from an object, bump with it
		// Model things as cylnders.
		
		distance = aprox_dist(
							PLAYER_INT_VAL(pPlayer->x),
							PLAYER_INT_VAL(pPlayer->y),
							mythings[i].x,
							mythings[i].y
							);
		
		// Note the art is rotated so we use h instead of w to calculate width.
		if (IsHalfWidthBumpable(i)) 	// like trees you can pass thru the outer edges.
		{
			ThingHalfWidth = mythings[i].widScaled/4;
			ThingHeadHeight = mythings[i].z + (mythings[i].heiScaled - mythings[i].heiScaled/4);
		}
		else
		{
			ThingHalfWidth = mythings[i].widScaled/2;
			ThingHeadHeight = mythings[i].z + mythings[i].heiScaled;
		}
		
		
		if( (PlayerHalfWidth + ThingHalfWidth) > distance &&
		     PlayerHeadHeight > mythings[i].z 	&&	// Our head is above their feet.
		     pPlayer->z < ThingHeadHeight			// Our feet are below their head.
		    )
		{
			LONG BumpDistance = ThingHalfWidth;
			const LONG DeltaOverZ = ThingHeadHeight - pPlayer->z;
			const LONG DeltaUnderZ = PlayerHeadHeight - mythings[i].z;
			
			
			// GWP should also check ceiling height here.
			if (DeltaOverZ > 0 &&
			    DeltaOverZ < BumpDistance &&
			    ((CanStandOn(i) && (DeltaOverZ < pPlayer->h/2)) ||
			    pPlayer->Flying  == TRUE ) &&
			    !HangsFromCeiling(i)
			   )
			{
				// Go over
				pPoint->dz += (DeltaOverZ < Rate ) ? DeltaOverZ : Rate;
				
				pPlayer->bump = iOBJECT;
				pPlayer->BumpIndex = i;
				
				return mythings[i].type;
			}
			else
			// GWP should also check floor height here.
			if ( DeltaUnderZ < BumpDistance &&
			   	 pPlayer->Flying == TRUE
			   )
			{
				// Go Under if you are a flying object and can.
				pPoint->dz -= (DeltaUnderZ < Rate ) ? DeltaUnderZ : Rate;
				
				pPlayer->bump = iOBJECT;
				pPlayer->BumpIndex = i;
				
				return mythings[i].type;
			}
			else
			{
				// Go Around
				LONG AngleToObject;
				LONG RelAngle;
				FIXED_VECTOR Delta;
				BOOL GoingForward;
				
				//otherwise return a true bump
				//printf("Bumped into an object!\n");
				
				// check to see if we are moving backwards or forwards
				if ((pPlayer->a > 64 && pPlayer->a < 192 && pPoint->dy < 0) ||
				   ((pPlayer->a < 64 || pPlayer->a > 192) && pPoint->dy > 0))
				{
					GoingForward = TRUE;
				}
				else
				{
					GoingForward = FALSE;
				}
				
				AngleToObject = AngleFromPoint(
								pPlayer->x,
								pPlayer->y,
								(mythings[i].x<<PLAYER_FIXEDPT),
								(mythings[i].y<<PLAYER_FIXEDPT),
								RESOLUTION_1
								);
				
				// GWP RelAngle = RelativeAngle( pPlayer->a, AngleToObject );
				
				// Now normalize the angle to the player's angle.
				RelAngle = AngleToObject - pPlayer->a;
				if (RelAngle < 0)
				{
					RelAngle += 256;
				}
				
				// move either left or right of the bump point whichever
				// is the shortest way around.
				
				if (BumpDistance > Rate)	// Were going to have bump a couple of times,
				{							// but for large things like the Wyvern we have to.
					BumpDistance = Rate;
				}
				
				if (RelAngle > 128)
				{
					Delta.dx = 	(BumpDistance)<<PLAYER_FIXEDPT;
				}
				else
				{
					Delta.dx = 	-(BumpDistance)<<PLAYER_FIXEDPT;
				}
				Delta.dy = 0;
				
				//Rotate((POINT *)&Delta,AngleToObject);
				
				// Rotate the offset in the direction of travel.
				if (GoingForward)
				{
					Rotate((POINT *)&Delta, AngleFromPoint( 0, 0,
				                                       PLAYER_INT_VAL(pPoint->dx),
				                                       PLAYER_INT_VAL(pPoint->dy),
				                                       RESOLUTION_1));
				}
				else
				{
					Rotate((POINT *)&Delta, -AngleFromPoint( 0, 0,
				                                       PLAYER_INT_VAL(pPoint->dx),
				                                       PLAYER_INT_VAL(pPoint->dy),
				                                       RESOLUTION_1));
				}
				
				pPoint->dx += Delta.dx;
				pPoint->dy += Delta.dy;
	
				pPlayer->bump = iOBJECT;
				pPlayer->BumpIndex = i;
	
				return mythings[i].type;
			}
		}
		
	}

	return 0;
}  // CheckBump


// ---------------------------------------------------------------------------
LONG LinePointDistance (LONG x, LONG y, LONG ax, LONG ay, LONG bx, LONG by)
{
// [d5-07-96 14:32 JPC]
// Return distance squared from point (x,y) to the line from (ax,ay) to (bx,by).
// Caller can take the square root of the result, if desired, to get the
// actual distance.
// ax and ay are point A, aka vertex A.
// bx and by are point B, aka vertex B.
// x and y are the player's position.
// TODO: Think about changing from double to LONG; maybe used fixed-point.
// This routine does not seem to slow things down very much.
// [d8-13-96 JPC] Made this function non-static so SECTORS: DistanceToSeg
// could call it.
// ---------------------------------------------------------------------------

	double      x1;
	double      y1;

	const double dx1 = bx - ax;

	//dx2 = x - ax;
	//dy2 = y - ay;

	// Special cases for vertical and horizontal lines AB.
	// Since there are so many of these, this is a useful optimization;
	// but we have to do it anyway to avoid division by zero.
	if (dx1 == 0)
	{
		x1 = ax;
		y1 = y;
		//goto FinalCheck;
	}
	else
	{
		const double dy1 = by - ay;
		if (dy1 == 0)
		{
			x1 = x;
			y1 = ay;
			// goto FinalCheck;
		}
		else
		{
			const double b = (by * dx1 - bx * dy1) / dx1;    // get the constant for the
			// linear equation y = mx + b
			// for the line AB
		
			// Now get the point (x1, y1) at the intersection of AB and the
			// perpendicular line running to (x,y).
			x1 = ((y - by) * (dx1 * dy1) + x * dx1 * dx1 + bx * dy1 * dy1) /
				(dy1 * dy1 + dx1 * dx1);
		
			y1 = (x1 * dy1) / dx1 + b;
		}
	}

//FinalCheck:
	// Now make sure (x1,y1) is actually on the line segment AB.  If not,
	// use the distance from the nearest of point A or point B.
	if (x1 < min (ax, bx))
		x1 = min (ax, bx);
	else if (x1 > max (ax, bx))
		x1 = max (ax, bx);

	if (y1 < min (ay, by))
		y1 = min (ay, by);
	else if (y1 > max (ay, by))
		y1 = max (ay, by);

	// Return the distance squared.
	return (LONG) ((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y));

}  // LinePointDistance


#ifdef _DEBUG
// Global debug variables used in CheckLinesInBlock.
	SHORT  giLine1 = 1930;
	SHORT  giLine2 = 302;
#endif

/* ========================================================================
   Function    - CheckLinesInBlock
   Description - Helper of CheckLineMove.
   Returns     - What type of thing you hit.  iNOTHING if you didn't hit anything.
   ======================================================================== */
static WadThingType CheckLinesInBlock (const PLAYER * const pPlayer,				// )
							   SHORT sSpecial,
							   SHORT * bm,
   							   LONG x1, LONG y1,
   							   LONG x2, LONG y2,
   							   LONG minDist) // Valid only if return == FALSE;
{
// [d5-08-96  9:35 JPC]

	LONG		i;
	LONG		j;
	SHORT		iLineDef;
	LONG		va, vb;
	LONG		special, tag;
	LONG		side1;
	LONG		side2;
	LONG		ceiling1;
	LONG		floor2;
	LONG		ceiling2;
	LONG		distPlayer;
	LONG		distTarget;
	BOOL		fIntersect;             // use Booleans to simplify the code
	BOOL		fTooClose;              // (See McConnell, Sec. 11.5.)
#ifdef _DEBUG
#ifdef _JPC
	ULONG       playerSector;
#endif
#endif

	i=1;                                // blockmap element 0 is always 0,
													// so start at 1
	while (bm[i]!=-1)                   // last element is -1
	{
		iLineDef = bm[i];
		++i;
		special = linedefs[iLineDef].special;
		tag = linedefs[iLineDef].tag;
		va = linedefs[iLineDef].a;
		vb = linedefs[iLineDef].b;

		distPlayer = LinePointDistance (x1, y1, vertexs[va].x, vertexs[va].y, vertexs[vb].x, vertexs[vb].y);
		distTarget = LinePointDistance (x2, y2, vertexs[va].x, vertexs[va].y, vertexs[vb].x, vertexs[vb].y);
		gBumpDistance = distPlayer;
		fIntersect = lines_intersect (x1, y1, x2, y2,
			vertexs[va].x, vertexs[va].y, vertexs[vb].x, vertexs[vb].y);

		fTooClose =  distPlayer > distTarget && distTarget <= minDist;

		if (fIntersect || fTooClose)
		{
			// for the bump code, save this value
			gbumpV1.x = vertexs[va].x;
			gbumpV1.y = vertexs[va].y;
			gbumpV2.x = vertexs[vb].x;
			gbumpV2.y = vertexs[vb].y;
			//gBumpDistance = distPlayer;

			// [d11-15-96 JPC] We are using LSP_TELE_EXIT_2WAY as an
			// edge of the world type.
			if (linedefs[iLineDef].special == LSP_TELE_EXIT_2WAY)
			{
#ifdef _BUMPLOG
		UpdateBumpRecord (pPlayer, x1, y1, x2, y2, TRUE, iLineDef, iEDGE_OF_WORLD);
#endif
				return iEDGE_OF_WORLD;
			}

			/* -----------------------------------------------------------------
			   if linedef has no 2nd-side or it's marked as impassible
			   ----------------------------------------------------------------- */
			// GEH says this may not work if linedefs are "flipped" in
			// the editor; empirically, it seems to work.
			if (linedefs[iLineDef].psdt == -1 || linedefs[iLineDef].psdb == -1)
			{
				// [d2-21-97 JPC] Added test to make sure we're in front of the
				// wall; if we're in back of the wall, we should not bump,
				// because we're not supposed to be able to get there.
				// This should fix the problem with changing direction when we
				// bump a wall.

				// Possible bad side-effect: walking through walls.  If that
				// starts to happen, suspect this code!

				if (back_face_vertex(va,vb,PLAYER_INT_VAL(pPlayer->x),
					PLAYER_INT_VAL(pPlayer->y)) == FRONT)
				{
               return iWALL;           // one-sided line
				}
				// gfOldTest = 1;	// remember that the old test would have treated this as a wall
				// gfOldTestLine = iLineDef;
				continue;						// [d2-26-97 JPC] Do not do other tests
													// on this line.  If you are in back
													// of a single-sided line, you should
													// not be affected by it.
			}

			// [d11-25-96 JPC] Added check for linedef special field to prevent
			// AIs from attacking through closed doors.
			// OLD: if(pPlayer->Flying == FALSE && linedefs[iLineDef].flags & IMPASSABLE_LINE)
			if ((linedefs[iLineDef].flags & IMPASSABLE_LINE) &&
			 	(!pPlayer->Flying || linedefs[iLineDef].special != 0))
			{
				// [d2-26-97 JPC] Note that if we have a 2-sided impassible line,
				// which we shouldn't (but do), then the deflection code in CheckMove
				// might work incorrectly.  The fix is to edit the WADs to make
				// impassible lines 1-sided.
				// [d2-26-97 JPC] Try checking for in front.  Might allow walking
				// through walls.  Note that if we get here, the line must be
				// 2-sided, because the previous test will either return or continue
				// if the line is 1-sided.
				if (back_face_vertex(va,vb,PLAYER_INT_VAL(pPlayer->x),
					PLAYER_INT_VAL(pPlayer->y)) == FRONT)
				{
					return iWALL;					// impassible line
				}
				// gfOldTest = 2;	// remember that the old test would have treated this as a wall
				// gfOldTestLine = iLineDef;
			}

			if (sSpecial & CHECKLINE_MONSTER
				&& linedefs[iLineDef].flags & MONSTER_BLOCK_LINE
				)
			{
#ifdef _BUMPLOG
		UpdateBumpRecord (pPlayer, x1, y1, x2, y2, TRUE, iLineDef, iMONSTER_BOX);
#endif
				return iMONSTER_BOX;
			}
		
		
		
			/* -----------------------------------------------------------------
			   Check for special linedef actions.
				These are walkover activations; for switch activations,
				see SECTORS.C: activate_seg.
			   ----------------------------------------------------------------- */
			if (fIntersect && (sSpecial & CHECKLINE_PLAYER))
			{
				//WRC we've crossed this line so, set this bit (for the map)
				// JPC note: we may not actually have succeeded in crossing
				// the line; floor/ceiling interactions may stop us.  But
				// we have gotten this far (11-24-96) with the existing code,
				// and changing it may break more stuff than it fixes.
				// So don't change it unless we have to.
				linedefs[iLineDef].flags |= HAS_BEEN_CROSSED;
//				printf("crossed linedef %li\n",iLineDef);

				switch (special)	
				{
               case LSP_DOOR_DWS:      // open door
               case LSP_DOOR_DWF:      // open door
						DoorActivate (iLineDef, tag, special);
					   break;

               case LSP_CEIL_W1:       // ceiling crusher
               case LSP_CEIL_WR:       // ceiling crusher
                                       // LSP_CEIL_W1: Ceiling goes down to floor and stays there.
                                       // LSP_CEIL_WR: Ceiling goes down, pauses, then comes up again.
						CeilingActivate (iLineDef, tag, special);
						break;

               case LSP_TELE_EXIT_1WAY: // TYPE 11 TELEPORTS (compare type 52)
                                        /*Test to make sure we're on the front side */
						if (!back_face_vertex(va,vb,PLAYER_INT_VAL(player.x),PLAYER_INT_VAL(player.y)))
						{
							fExitLevel = tag;
							// NOTE: teleport_data is used in PLAYER.CPP: PlayerArrival.
							teleport_data.dx = 0; // [d10-22-96 JPC] special value
							teleport_data.dy = 0; // [d10-22-96 JPC] special value
							teleport_data.a = player.a;
#ifdef _BUMPLOG
		UpdateBumpRecord (pPlayer, x1, y1, x2, y2, TRUE, iLineDef, iEXITLEVEL);
#endif
							return iEXITLEVEL;	// [d10-22-96 JPC]
						}
#ifdef _BUMPLOG
		UpdateBumpRecord (pPlayer, x1, y1, x2, y2, TRUE, iLineDef, iNOTHING);
#endif
						return iNOTHING;		// sb break?

               case LSP_LIFT_WI:       // lift
               case LSP_LIFT_WA:       // lift
               case LSP_LIFT_WIC:      // lift
               case LSP_LIFT_WAC:      // lift
						LiftActivate (iLineDef, tag, special);
						break;

               case LSP_PIT:           // floor down fast and far (pit trap)
               case LSP_UNPIT:         // floor up fast and far (reverse pit trap)
						// Unlike crushers, pit traps move a fixed amount.
						FloorActivate (iLineDef, tag, special);
					   break;

               case LSP_TELE_1WAY:     // "W1 teleport"
                                       /*Test to make sure we're on the front side */
						if (!back_face_vertex(va,vb,PLAYER_INT_VAL(player.x),PLAYER_INT_VAL(player.y)))
						{
							j = tag_to_line(tag, iLineDef);
                     // [d5-28-96 JPC] Attempt to fix reported bug in teleport W1.
                     // Idea: bug is caused by an incorrect x & y when you hit
                     // the teleport line at a glancing blow.  Fix by moving your
                     // x and y to the middle of the linedef of the target,
                     // keeping your angle the same.
                     // [d6-04-96 JPC] The problem with the fix is that if you
                     // are moving straight ahead but are not in the center and
                     // you cross a teleport line, there will be a noticeable
                     // wobble, especially in corridors.  Revisit later.
#if 0
// Old code:
							SetPlayerXYA(
								PLAYER_INT_VAL(player.x)+vertexs[linedefs[j].a].x-vertexs[linedefs[iLineDef].a].x,
								PLAYER_INT_VAL(player.y)+vertexs[linedefs[j].a].y-vertexs[linedefs[iLineDef].a].y,
								player.a
								);
#endif
							x1 = (vertexs[linedefs[j].a].x + vertexs[linedefs[j].b].x) / 2;
							y1 = (vertexs[linedefs[j].a].y + vertexs[linedefs[j].b].y) / 2;
							SetPlayerXYA (x1, y1, player.a);
							fTeleport = TRUE;
						}
#ifdef _BUMPLOG
		UpdateBumpRecord (pPlayer, x1, y1, x2, y2, TRUE, iLineDef, iNOTHING);
#endif
						return iNOTHING;

#if 0
               case LSP_TELE_EXIT_2WAY: // TYPE 52 TELEPORTS--exit (compare type 11)
						// [d11-15-96 JPC] We weren't using this in the game
						// for its intended purpose as an exit, so we have
						// changed its meaning to "Edge of the world."  You get
						// a special message when you hit the edge of the world.
						// But it needs to be handled earlier.
						return iEDGE_OF_WORLD;
#endif

               case LSP_FLOOR_W1:      // floor up to ceiling (crush)
               case LSP_FLOOR_WR:      // floor up to ceiling (crush)
                                       // LSP_FLOOR_W1: Floor stays up.
                                       // LSP_FLOOR_WR: Floor pauses, then goes back down.
						FloorActivate (iLineDef, tag, special);
						break;
					
               case LSP_TELE_2WAY:     // "WR teleport"
						TeleportPlayer(tag_to_sector(linedefs[iLineDef].tag));
						fTeleport = TRUE;
#ifdef _BUMPLOG
		UpdateBumpRecord (pPlayer, x1, y1, x2, y2, TRUE, iLineDef, iNOTHING);
#endif
						return iNOTHING;
				}
				if (special >= LSP_DIALOG_FIRST &&
				    special <= LSP_DIALOG_LAST)
				{
					gDialogFlag = special;
				}
			}

			/* -----------------------------------------------------------------
			   check floor/ceiling interactions
			   ----------------------------------------------------------------- */
			// [d5-02-96 17:22 JPC]
			// OLD CODE: if (!back_face_vertex(va,vb,PLAYER_INT_VAL(player.x), PLAYER_INT_VAL(player.y)))
			//               ^
			// We should swap the normal meaning of psdb and psdt only if we are
			// facing the back side of this linedef.
			// (psdb is always present; psdt is present only for
			// two-sided linedefs.)
			// [d8-01-96 JPC] Added pPlayer x and y; check them instead of
			// the camera's x and y.  (This routine is used by avatars, not
			// just by the player.)
			if (back_face_vertex (va, vb, PLAYER_INT_VAL(pPlayer->x), PLAYER_INT_VAL(pPlayer->y)))
			{
				side1=(LONG)linedefs[iLineDef].psdt;
				side2=(LONG)linedefs[iLineDef].psdb;
			}
			else
			{
				side1=(LONG)linedefs[iLineDef].psdb;
				side2=(LONG)linedefs[iLineDef].psdt;
			}

#ifdef _JPC
			playerSector = point_to_sector (x1, y1);
			// JPC: The following test sometimes fails when the player's
			// X coordinate is ON a line segment.  (Presumably can also
			// fail when the Y coordinate is on a line segment, but I
			// have not seen or looked for this case.)
			// But it doesn't seem to cause a problem.
         if (sidedefs[side1].sec_ptr != (short) playerSector)
         {
            TRACE ("Found discrepancy at %d, %d: sec_ptr = %d, point_to_sector = %d\n\r",
               x1, y1, sidedefs[side1].sec_ptr, playerSector);
         }
#endif
			ceiling1 = sectors[sidedefs[side1].sec_ptr].ch;
			floor2   = sectors[sidedefs[side2].sec_ptr].fh;
			ceiling2 = sectors[sidedefs[side2].sec_ptr].ch;
			
			// can't step up?
			// walks on ceiling.
			if (pPlayer->ThingIndex != fERROR &&
			    HangsFromCeiling(pPlayer->ThingIndex))
			{
				if (pPlayer->z < floor2)
				{
#ifdef _STATUS
					WriteDebug ("Too big a step in line %d", iLineDef);
#endif
					gFloor = floor2;
#ifdef _BUMPLOG
		UpdateBumpRecord (pPlayer, x1, y1, x2, y2, TRUE, iLineDef, iFLOOR);
#endif
					return iFLOOR;
				}
				
				// Can't climb up to the new ceiling, too tall (.5 * height)
				if (ceiling2 > ceiling1 &&
				    (pPlayer->z + pPlayer->h + (pPlayer->h/2)) < ceiling2)
				{
					gCeiling = ceiling2;
#ifdef _BUMPLOG
		UpdateBumpRecord (pPlayer, x1, y1, x2, y2, TRUE, iLineDef, iCEILING);
#endif
					return iCEILING;
				}
				
				// Allow things moving on the ceiling to step over small
				// variations in ceiling height.
				if (ceiling2 < ceiling1 &&
				    (pPlayer->z - (pPlayer->h/2)) > ceiling2)
				{
					gCeiling = ceiling2;
					gFloor = floor2;
#ifdef _BUMPLOG
		UpdateBumpRecord (pPlayer, x1, y1, x2, y2, TRUE, iLineDef, iCEILING);
#endif
					return iCEILING;
				}
			}
			else
			if (pPlayer->Flying)
			{
				LONG	actualFloor;
				LONG	waterOffset;
				SHORT	special;

				special = sectors[sidedefs[side2].sec_ptr].special;
				if (special == SSP_WATER || special == SSP_ACID_FLOOR || special == SSP_LAVA)
					waterOffset = pPlayer->h / 4;
				else if (special == SSP_DEEP_WATER)
					waterOffset = (pPlayer->h * 3) / 4;
				else
					waterOffset = 0;
				actualFloor = floor2 - waterOffset;
				// [d12-12-96 JPC] Added the pPlayer->h/2 term so
				// we can walk up stairs while levitating without
				// having to press the A key.
				if (pPlayer->z + pPlayer->h/2 < actualFloor)
				{
					gFloor = actualFloor;
#ifdef _BUMPLOG
		UpdateBumpRecord (pPlayer, x1, y1, x2, y2, TRUE, iLineDef, iFLOOR);
#endif
					return iFLOOR;
				}
				if (ceiling2 < ceiling1 &&
				    (pPlayer->z + pPlayer->h > ceiling2))
				{
					gCeiling = ceiling2;
#ifdef _BUMPLOG
		UpdateBumpRecord (pPlayer, x1, y1, x2, y2, TRUE, iLineDef, iCEILING);
#endif
					return iCEILING;
				}
				
			}
			else
			{
				const LONG PlayerHalfHeight = pPlayer->h/2;
				
				if ((pPlayer->z + PlayerHalfHeight) < floor2 )
				{
#ifdef _STATUS
					WriteDebug ("Too big a step in line %d", iLineDef);
#endif
					gFloor = floor2;
					gCeiling = ceiling1;
#ifdef _BUMPLOG
		UpdateBumpRecord (pPlayer, x1, y1, x2, y2, TRUE, iLineDef, iFLOOR);
#endif
					return iFLOOR;
				}
				
				// Your head would be in the ceiling.
				if ((pPlayer->z + pPlayer->h) > ceiling2)
				{
					gCeiling = ceiling2;
#ifdef _BUMPLOG
		UpdateBumpRecord (pPlayer, x1, y1, x2, y2, TRUE, iLineDef, iCEILING);
#endif
					return iCEILING;
				}

				// Test for hole.
				// [d11-24-96 JPC] Move test for hole to AFTER test for head
				// in the ceiling.
				// The original test had at least 2 bugs: (1) if you were
				// "too close" to the edge but hadn't actually crossed the
				// line, you fell anyway while staying in the same sector.
				// This could leave you in a wall.  (2) If you moved from
				// sector A to sector C with sector B in the way, and we
				// happened to check the lines for sector B first, and sector
				// B was a hole, then we returned iHOLE even though you should
				// have been able to get from A to C (or C should have returned
				// a benign wall, ceiling, or floor bump).
				if (pPlayer->Flying == FALSE &&
				    (pPlayer->z - (pPlayer->h + PlayerHalfHeight)) > floor2
				   )
				{
					// [d11-24-96 JPC] Add test to see whether target point
					// is actually IN sector 2; if not, you may have jumped
					// over the hole!  In that case, continue checking lines.
					// Note that under this new test, if the target is still
					// in the original sector, then even if it is theoretically
					// "too close" to the edge, you still get to move, you stay
					// in the original sector, and you don't fall.
					//
					// [d3-08-97 JPC] Added check for ladder sector (SSP_CLIMB_OK)
					// so we won't take damage when going to or from ladder
					// sectors.
					SHORT targetSector = (SHORT) point_to_sector (x2, y2);
					if (targetSector == sidedefs[side2].sec_ptr
						&& sectors[targetSector].special != SSP_CLIMB_OK)
					{
						gFloor = floor2;
#ifdef _BUMPLOG
		UpdateBumpRecord (pPlayer, x1, y1, x2, y2, TRUE, iLineDef, iHOLE);
#endif
						return iHOLE;
					}
				}
			}
			
			// Can't step into space too small.
			if ((ceiling2 - floor2) < pPlayer->h)
			{
#ifdef _STATUS
				WriteDebug ("Too small a space in line %d", iLineDef);
#endif
				gCeiling = ceiling2;
#ifdef _BUMPLOG
		UpdateBumpRecord (pPlayer, x1, y1, x2, y2, TRUE, iLineDef, iCEILING);
#endif
				return iCEILING;
			}
		}
	}
#ifdef _BUMPLOG
		UpdateBumpRecord (pPlayer, x1, y1, x2, y2, TRUE, iLineDef, iNOTHING);
#endif
	return iNOTHING;
}  // CheckLinesInBlock


/* ========================================================================
   Function    - CheckLineMove
   Description - checks for intersection/collision against the blockmap
   					NOTE: this routine does not use fixed point x and y
   Returns     - iNOTHING if you didn't hit anything, otherwise what
					  you hit.
   ======================================================================== */
static WadThingType CheckLineMove (PLAYER *pPlayer,
						   FIXED_POINT_3D *pPoint,
						   BOOL sSpecial,
						   LONG minDist)
{
// [d5-08-96  9:54 JPC] Change in logic: instead of looking at the blockmap
// of the target, look at the blockmaps in the direction of travel and
// 45 degrees on either side.
// ---------------------------------------------------------------------------

	SHORT *     bm;                     // current blockmap
	LONG        x1, y1;                 // player

	x1 = PLAYER_INT_VAL(pPlayer->x);
	y1 = PLAYER_INT_VAL(pPlayer->y);

	// Get a pointer to a list of linedefs in this "block," which is a
	// 128 X 128 square in the map.
	bm = get_blockm (x1, y1);

	// gfOldTest = 0;								// [d2-26-97 JPC] debug

	if (bm != FALSE)                      /*inside block area!!!*/
	{
		// SHORT *     bm2;                 // (abandoned)
		SHORT *     bm3;                    // blockmap in dir of travel - 45 degrees
		SHORT *     bm4;                    // blockmap in direction of travel
		SHORT *     bm5;                    // blockmap in dir of travel + 45 degrees
		LONG        x2, y2;                 // target
		LONG        x3, y3;                 // 45 degrees less than x4,y4
		LONG        x4, y4;                 // some multiple of 45 degrees in direction of target
		LONG        x5, y5;                 // 45 degrees more than x4,y4
		LONG        lookAhead;              // distance to look ahead, possibly into other blockmaps
		WadThingType Result = iNOTHING;
		
		x2 = PLAYER_INT_VAL(pPoint->x );
		y2 = PLAYER_INT_VAL(pPoint->y );
		
#ifdef _BUMPLOG
		//UpdateBumpRecord (pPlayer, x1, y1, x2, y2, FALSE, 0, 0);
#endif
	
		Result= CheckLinesInBlock (pPlayer,
								sSpecial,
								bm,
								x1, y1,
								x2, y2,
								minDist);
		if (iNOTHING != Result)
		{
			return Result;
		}
	
		// Now find the blocks in the direction of travel and 45 degrees adjacent
		// to direction of travel.
		// lookAhead = 64;                     // 64 = 1/2 of blockmap width or height
		lookAhead = 128;							// [d12-04-96 JPC] check full width of blockmap
														// (CheckLongMove moves the test point
														// by 100 pixels; sometimes this put
														// the point in a block we didn't check
														// with a lookAhead of only 64!)
		if (x2 < x1)
		{
			if (y2 < y1)
			{
				x3 = x1 - lookAhead;
				x4 = x1 - lookAhead;
				x5 = x1;
				y3 = y1;
				y4 = y1 - lookAhead;
				y5 = y1 - lookAhead;
			}
			else if (y2 > y1)
			{
				x3 = x1;
				x4 = x1 - lookAhead;
				x5 = x1 - lookAhead;
				y3 = y1 + lookAhead;
				y4 = y1 + lookAhead;
				y5 = y1;
			}
			else             // y2 == y1
			{
				x3 = x1 - lookAhead;
				x4 = x1 - lookAhead;
				x5 = x1 - lookAhead;
				y3 = y1 + lookAhead;
				y4 = y1;
				y5 = y1 - lookAhead;
			}
		}
		else if (x2 > x1)
		{
			if (y2 < y1)
			{
				x3 = x1;
				x4 = x1 + lookAhead;
				x5 = x1 + lookAhead;
				y3 = y1 - lookAhead;
				y4 = y1 - lookAhead;
				y5 = y1;
			}
			else if (y2 > y1)
			{
				x3 = x1 + lookAhead;
				x4 = x1 + lookAhead;
				x5 = x1;
				y3 = y1;
				y4 = y1 + lookAhead;
				y5 = y1 + lookAhead;
			}
			else                             // y2 == y1
			{
				x3 = x1 + lookAhead;
				x4 = x1 + lookAhead;
				x5 = x1 + lookAhead;
				y3 = y1 - lookAhead;
				y4 = y1;
				y5 = y1 + lookAhead;
			}
		}
		else                                // x2 == x1
		{
			if (y2 < y1)
			{
				x3 = x1 - lookAhead;
				x4 = x1;
				x5 = x1 + lookAhead;
				y3 = y1 - lookAhead;
				y4 = y1 - lookAhead;
				y5 = y1 - lookAhead;
			}
			else if (y2 > y1)
			{
				x3 = x1 + lookAhead;
				x4 = x1;
				x5 = x1 - lookAhead;
				y3 = y1 + lookAhead;
				y4 = y1 + lookAhead;
				y5 = y1 + lookAhead;
			}
			// else target and starting point are the same--
			else
				return iNOTHING;
		}
	
		bm3 = get_blockm (x3, y3);
		if (bm3 != FALSE)                        /*inside block area!!!*/
		{
#ifdef _BUMPLOG
			//UpdateBumpRecord (pPlayer, x1, y1, x2, y2, FALSE, 0, 0);
#endif
		
			// If bm3 is the same as bm, don't check it again.
			if (bm3 != bm)
			{
				Result = CheckLinesInBlock (pPlayer,
										sSpecial,
										bm3,
										x1, y1,
										x2, y2,
										minDist);
				if (Result != iNOTHING)
					return Result;
			}
		}
	
		bm4 = get_blockm (x4, y4);
		if (bm4 != FALSE)                        /*inside block area!!!*/
		{
#ifdef _BUMPLOG
			//UpdateBumpRecord (pPlayer, x1, y1, x2, y2, FALSE, 0, 0);
#endif
			// If bm4 is the same as one we already checked, don't check it again.
			if (bm4 != bm && bm4 != bm3)
			{
				Result = CheckLinesInBlock (pPlayer,
										sSpecial,
										bm4,
										x1, y1,
										x2, y2,
										minDist);
				if (Result != iNOTHING)
				{
					return Result;
				}
			}
		}
	
		bm5 = get_blockm (x5, y5);
		if (bm5 != FALSE)                        /*inside block area!!!*/
		{
#ifdef _BUMPLOG
			//UpdateBumpRecord (pPlayer, x1, y1, x2, y2, FALSE, 0, 0);
#endif
			// If bm5 is the same as one we already checked, don't check it again.
			if (bm5 != bm && bm5 != bm3 && bm5 != bm4)
			{
				Result = CheckLinesInBlock (pPlayer,
										sSpecial,
										bm5,
										x1, y1,
										x2, y2,
										minDist);
				if (Result != iNOTHING)
				{
					return Result;
				}
			}
		}
	}
	else
	{
		return iWALL;
	}

	// if (gfOldTest > 0)
	// 	printf ("Old test %d would have blocked this move at line %d!\n", gfOldTest, gfOldTestLine);

	return iNOTHING;
}  // CheckLineMove


/* ========================================================================
   Function    - CheckMove
   Description - checks for collisions with line segments.
                 NOTE: check engine.h for macros that set sSpecial
   				 NOTE: this routine does use 24.8 fixed point x and y
   Returns     - iNOTHING if no collisions, otherwise what you collided with
   ======================================================================== */
WadThingType CheckMove (
PLAYER *pPlayer,
FIXED_VECTOR_3D *pPoint, 	// This is the delta of the motion, not the absolute location.
SHORT sSpecial,
LONG *Angle,
LONG *BumpDistance)	// distance to line squared;
{
	//PLAYER      	startp;
	FIXED_POINT_3D 	targetp;
	FIXED_VECTOR	adj;
	BOOL        	TestTwo = FALSE;
 	LONG				special;
 	// GWP LONG				sector;	// Save in currentSector instead.
 	LONG 				tag;
	LONG        	direction;              // direction of movement
	LONG        	minDist;                // minimum distance allowed from wall
	WadThingType 	Result = iNOTHING;
	LONG LineAngle;
	*Angle = fERROR;		// Initialize it to an invalid angle.
	

#ifdef _BUMPLOG
	gCallingFunction = 0;
#endif

	// You cannot get closer to a line than minDist.
	// For now, it is half the player width.  (GEH changed to full width)
	// THINK ABOUT: Perhaps the caller should set minDist.
	//GEH minDist = (pPlayer->w * pPlayer->w) / 4; // square the distance to avoid square roots
#ifdef _EDIT
   minDist = 4;                        // [d6-05-96 JPC] make it easy to move
                                       // when editing
#else
	minDist = (pPlayer->w * pPlayer->w); // square the distance to avoid square roots
#endif

	// This will probably have to come out later, because things might
	// happen even when you're standing still.
	if (pPoint->dx == 0 && pPoint->dy == 0)
	{	
		if(pPlayer->ThingIndex != fERROR
	      && mythings[pPlayer->ThingIndex].valid == TRUE
		 	&& pPlayer->Flying)
		{
		 	sector_info(mythings[pPlayer->ThingIndex].sect, &pPlayer->floor,
		 					&pPlayer->ceiling, &special, &tag);
		}
		else if(pPlayer->Flying)
		{
			point_to_sector_info(PLAYER_INT_VAL(pPlayer->x+pPoint->dx),
										PLAYER_INT_VAL(pPlayer->y+pPoint->dy),
										&pPlayer->currentSector, &pPlayer->floor, &pPlayer->ceiling, &special, &tag);
		}
		
		if(pPlayer->Flying && pPlayer->ceiling < (pPlayer->z+pPoint->dz+pPlayer->h))
				pPoint->dz = pPlayer->ceiling - pPlayer->z - pPlayer->h;

#ifdef _STATUS
		if (Result != iNOTHING)
			WriteDebug ("Result = %d", Result);
#endif
		return Result; // no movement, move ok
	}
	// Check direction of movement from where we were to where we're
	// trying to go.
	direction = AngleFromPoint(
		PLAYER_INT_VAL(pPlayer->x),
		PLAYER_INT_VAL(pPlayer->y),
		PLAYER_INT_VAL(pPlayer->x+pPoint->dx),
		PLAYER_INT_VAL(pPlayer->y+pPoint->dy),
		RESOLUTION_1
		);
	

	// set up the player structure
	// GWP startp.z = pPlayer->z;
	// GWP startp.w = pPlayer->w;
	// GWP startp.h = pPlayer->h;
	// GWP startp.x = pPlayer->x;
	// GWP startp.y = pPlayer->y;

ReTest:
	fTeleport = FALSE;

	// Set the target location.
	targetp.x = pPlayer->x + pPoint->dx;
	targetp.y = pPlayer->y + pPoint->dy;
	targetp.z = pPlayer->z + pPoint->dz;


	Result = CheckLineMove (pPlayer, &targetp, sSpecial, minDist);
	*BumpDistance = gBumpDistance;
	// Always want small x coord to the left.
	if (gbumpV1.x < gbumpV2.x)
	{
		LineAngle = AngleFromPoint ( gbumpV1.x, gbumpV1.y, gbumpV2.x, gbumpV2.y, RESOLUTION_1 );
	}
	else
	{
		LineAngle = AngleFromPoint ( gbumpV2.x, gbumpV2.y, gbumpV1.x, gbumpV1.y, RESOLUTION_1 );
	}
	*Angle = LineAngle;

	
	if (Result == iWALL ||
		 (Result == iFLOOR && (!pPlayer->Flying)))
	{
		// Couldn't get from startp to incTarget because of some
		// impassible line in the way.
		LONG A1;
		//LONG A2;
		//POINT Bump1, Bump2;
		//LONG RotationAngle;
	
	
		// gbumpV* is set whenever you bump a ceiling, floor or wall.
		
		// GWP Old bump code.
		// GWP Bump1.x = gbumpV1.x << PLAYER_FIXEDPT;
		// GWP Bump1.y = gbumpV1.y << PLAYER_FIXEDPT;
		// GWP Bump2.x = gbumpV2.x << PLAYER_FIXEDPT;
		// GWP Bump2.y = gbumpV2.y << PLAYER_FIXEDPT;
	
		// Localize the line to my player angle.
		// GWP Rotate(&Bump1, pPlayer->a);
		// GWP Rotate(&Bump2, pPlayer->a);
		// GWP
		// GWP // Find the angle of this rotated line.
		// GWP // Always want small x coord to the left.
		// GWP if (Bump1.x <= Bump2.x)
		// GWP {
		// GWP 	A1 = AngleFromPoint ( Bump1.x, Bump1.y, Bump2.x, Bump2.y, RESOLUTION_1 );
		// GWP }
		// GWP else
		// GWP {
		// GWP 	A1 = AngleFromPoint ( Bump2.x, Bump2.y, Bump1.x, Bump1.y, RESOLUTION_1 );
		// GWP }
		
		
		// Always want small x coord to the left.
		
		
		// Rotate the line Angle to normalize it to the player angle.
		A1 = direction - LineAngle;
		if (A1 < 0)
		{
			A1 += 256;
		}
		
#if !defined(RELEASE)
		// DEBUG if (sSpecial & CHECKLINE_PLAYER)
		// DEBUG {
		// DEBUG 	printf("Player Angle = %ld, Hit Angle = %ld\n", pPlayer->a, A1);
		// DEBUG 	printf("Line Angle = %ld\n", LineAngle);
		// DEBUG }
#endif
	
		if(TestTwo == TRUE)
		{
// #ifdef _STATUS
// 			WriteDebug ("Failed second bump test!");
// #endif
			*Angle = A1;
			pPlayer->bump = Result;
		 	
		 	if(pPlayer->ThingIndex != fERROR
		 	   && mythings[pPlayer->ThingIndex].valid == TRUE
		 		&& pPlayer->Flying)
		 	{
		 		sector_info(mythings[pPlayer->ThingIndex].sect, &pPlayer->floor,
		 						&pPlayer->ceiling, &special, &tag);
		 	}
		 	else if(pPlayer->Flying)
		 	{
		 		point_to_sector_info(PLAYER_INT_VAL(pPlayer->x+pPoint->dx),
		 									PLAYER_INT_VAL(pPlayer->y+pPoint->dy),
		 									&pPlayer->currentSector, &pPlayer->floor, &pPlayer->ceiling, &special, &tag);
		 	}
		 	if(pPlayer->Flying && pPlayer->ceiling < (pPlayer->z+pPoint->dz+pPlayer->h))
		 	{
		 		pPoint->dz = pPlayer->ceiling - pPlayer->z - pPlayer->h;
		 		
		 		// Make sure we don't push back thru the floor.
		 		if (pPoint->dz < pPlayer->floor)
		 		{
		 			pPoint->dz = 0;
		 		}
	 		}
		 		
		
// #ifdef _STATUS
// 			if (Result != iNOTHING)
// 				WriteDebug ("Result = %d", Result);
// #endif
			return Result;		// only one retest
		}
		
		// Bumped straight on (more or less) with wall.  You're stuck.
		if ((A1 > 54 && A1 < 74 )|| ( A1 < 202 && A1 > 182))
		{
			//printf("Nose square into wall!\n");
#ifdef _STATUS
			WriteDebug ("Wall angle = %d, hit wall, can't slide [dir = %d, LineAngle = %d]",
				A1, direction, LineAngle);
#endif
			*Angle = A1;
			pPlayer->bump = Result;
 			if(pPlayer->ThingIndex != fERROR
 			   && mythings[pPlayer->ThingIndex].valid == TRUE
 				&& pPlayer->Flying)
 			{
 				sector_info(mythings[pPlayer->ThingIndex].sect, &pPlayer->floor,
 								&pPlayer->ceiling, &special, &tag);
 			}
 			else if(pPlayer->Flying)
 			{
 				point_to_sector_info(PLAYER_INT_VAL(pPlayer->x),
 											PLAYER_INT_VAL(pPlayer->y),
 											&pPlayer->currentSector, &pPlayer->floor, &pPlayer->ceiling, &special, &tag);

 			}
 			if(pPlayer->Flying && pPlayer->ceiling < (pPlayer->z+pPoint->dz+pPlayer->h))
 				pPoint->dz = pPlayer->ceiling - pPlayer->z - pPlayer->h;

			return Result;
		}
	
		// Speed is forced to ahead 8 if you bumped into a wall, which you
		// did if you get here.

		// OLD CHANGE: player rotation remains the same.
		// [d1-27-97 JPC] Reinstate very old code that changed player ANGLE
		// in response to bump; this should eventually keep you from bumping
		// whatever you're bumping.
		// Note that in a few cases, the bump code will stop you at a wall
		// that is not the closest wall to you.  (Both walls would have
		// stopped you, but the blockmap happens to list the further wall
		// first.)  In that case, the new angle will be based on the wrong
		// wall, which is why this code doesn't always work.  Fixing the
		// problem would require searching for the nearest wall every time
		// anyone bumps a wall, which is frequently.  This would make the
		// game run more slowly.  The best solution seems to be to put up
		// with an occasional wrong answer.

#ifdef _STATUS
		WriteDebug ("Wall angle = %d, trying to slide [dir = %d, LineAngle = %d]",
				A1, direction, LineAngle);
#endif
		if (A1 > 192)
		{
		 	pPlayer->a += 8;
			// if (pPlayer->a < 0)
			// 	pPlayer->a += 256;
			pPlayer->a %= 256;
			//direction = AngleFromPoint(
			//	pPlayer->x, pPlayer->y,
			//	pPlayer->x+pPoint->dx, pPlayer->y+pPoint->dy,
			//	RESOLUTION_1
			//	);
			//A1 = direction - LineAngle;
			//if(A1 == 0)
			//	pPlayer->a += 1;
			adj.dx = 3<< PLAYER_FIXEDPT;
			adj.dy = 8<< PLAYER_FIXEDPT;
			Rotate((POINT *)&adj,LineAngle);
		}
		else
		if (A1 > 128)
		{
			pPlayer->a -= 8;
			if (pPlayer->a < 0)
				pPlayer->a += 256;
			// pPlayer->a %= 256;
			//direction = AngleFromPoint(
			//	pPlayer->x, pPlayer->y,
			//	pPlayer->x+pPoint->dx, pPlayer->y+pPoint->dy,
			//	RESOLUTION_1
			//	);
			//A1 = direction - LineAngle;
		 	//if(A1 == 128)
			//	pPlayer->a-=1;
			adj.dx = 3<< PLAYER_FIXEDPT;
			adj.dy = -8<< PLAYER_FIXEDPT;
			Rotate((POINT *)&adj,LineAngle);
		}
		else
		if (A1 > 64)
		{
			pPlayer->a += 8;
			//if (pPlayer->a < 0)
			//	pPlayer->a += 255;
			pPlayer->a %= 256;
			//direction = AngleFromPoint(
			//	pPlayer->x, pPlayer->y,
			//	pPlayer->x+pPoint->dx, pPlayer->y+pPoint->dy,
			//	RESOLUTION_1
			//	);
			//A1 = direction - LineAngle;
			//if(A1 == 128)
			//	pPlayer->a +=1;
			adj.dx = -3<< PLAYER_FIXEDPT;
			adj.dy = -8<< PLAYER_FIXEDPT;
			Rotate((POINT *)&adj,LineAngle);
		}
		else
		{
			pPlayer->a -= 8;
			if (pPlayer->a < 0)
				pPlayer->a += 256;
			// pPlayer->a %= 256;
			//direction = AngleFromPoint(
			//	pPlayer->x, pPlayer->y,
			//	pPlayer->x+pPoint->dx, pPlayer->y+pPoint->dy,
			//	RESOLUTION_1
			//	);
			//A1 = direction - LineAngle;
			//if(A1 == 0)
			//	pPlayer->a -=1;
			adj.dx = -3<< PLAYER_FIXEDPT;
			adj.dy = 8<< PLAYER_FIXEDPT;
			Rotate((POINT *)&adj,LineAngle);
		}

		// move in the same direction as the wall.
		pPoint->dx = adj.dx;
		pPoint->dy = adj.dy;
		
		Result = iSLIDE_ON_WALL;
		pPlayer->bump = Result;
	
		TestTwo = TRUE;	// allow second pass
		goto ReTest;
	}
	else if (Result == iEDGE_OF_WORLD)
	{
		pPlayer->bump = Result;
#ifdef _STATUS
		if (Result != iNOTHING)
			WriteDebug ("Result = %d", Result);
#endif
		return Result;
	}
	else
	if (Result == iCEILING
		 && pPlayer->Flying)
	{
		if(TestTwo == TRUE)
		{
#ifdef _STATUS
			WriteDebug ("Failed second bump test!");
#endif
			pPlayer->bump = Result;
			pPlayer->ceiling = gCeiling;
#ifdef _STATUS
		if (Result != iNOTHING)
			WriteDebug ("Result = %d", Result);
#endif
			return Result;		// only one retest
		}
		
		if(pPlayer->ThingIndex == fERROR)
		{
			LONG const NewDz = gCeiling - (pPlayer->z + pPlayer->h);
			LONG const AbsNewDz = ABS(NewDz);
			
			if (AbsNewDz < (pPlayer->h / 2) &&
			    AbsNewDz < (ABS(pPoint->dz)))
				pPoint->dz = NewDz;
			else
				pPoint->dz = 0;
		}
		else
		{
			pPoint->dz = -3;
		}
		
		if ((pPoint->dz + pPlayer->z) < pPlayer->floor)
		{
			pPoint->dz = 0;
		}
		Result = iCEILING;
		pPlayer->bump = Result;
		TestTwo=TRUE;
		goto ReTest;
	}
	else
	if (Result == iFLOOR && pPlayer->Flying)
	{
		if(TestTwo == TRUE)
		{
#ifdef _STATUS
			WriteDebug ("Failed second bump test!");
#endif
			pPlayer->bump = Result;
			pPlayer->floor = gFloor;
			pPlayer->ceiling = gCeiling;
#ifdef _STATUS
			WriteDebug ("Result = %d", Result);
#endif
			return Result;		// only one retest
		}
		if(pPlayer->ThingIndex == fERROR)
		{
			LONG NewDz = gFloor - pPlayer->z;
			LONG const AbsNewDz = ABS(NewDz);
			
			if (AbsNewDz < (pPlayer->h/2) &&
			    AbsNewDz < (ABS(pPoint->dz)))
				pPoint->dz = NewDz;
			else
				pPoint->dz = 0;
				
		}
		else
		{
			pPoint->dz = 3;
		}
		
		if ((pPoint->dz + pPlayer->z + pPlayer->h) > gCeiling)
		{
			pPoint->dz = 0;
		}
		Result = iFLOOR;
		pPlayer->bump = Result;
		TestTwo = TRUE;
		goto ReTest;
	}
	else
	{
		if (fTeleport)
			goto ReTest;
		
		if (Result != iNOTHING)
			pPlayer->bump = Result;
	}
	
	// lan even player couldn't fly, he may still go down into floor??
	if(pPlayer->ThingIndex != fERROR
      && mythings[pPlayer->ThingIndex].valid == TRUE)
	 	//&& pPlayer->Flying)
	{
	 	sector_info(mythings[pPlayer->ThingIndex].sect, &pPlayer->floor,
	 					&pPlayer->ceiling, &special, &tag);
	}
	else //if(pPlayer->Flying)
	{
		point_to_sector_info(PLAYER_INT_VAL(pPlayer->x+pPoint->dx),
									PLAYER_INT_VAL(pPlayer->y+pPoint->dy),
									&pPlayer->currentSector, &pPlayer->floor, &pPlayer->ceiling, &special, &tag);
	}

	if(pPlayer->Flying && pPlayer->ceiling < (pPlayer->z+pPoint->dz+pPlayer->h))
			pPoint->dz = pPlayer->ceiling - (pPlayer->z + pPlayer->h);
	
	// [d9-04-96 JPC] Added tests for water; we don't want the caller to
	// start walking on water, so don't change dz here at all.
	// (In a water sector, the player's z will be less than the floor.)
	// [d10-08-96 JPC] Add test for lava.  It behaves like shallow water
	// for sinking.
	if (special != SSP_WATER &&
		special != SSP_ACID_FLOOR &&
		special != SSP_DEEP_WATER &&
		special != SSP_LAVA)
	{
		if(pPlayer->floor > pPlayer->z + pPoint->dz)
			pPoint->dz = pPlayer->floor-pPlayer->z;		 // for everyone
	}

	// [d11-25-96 JPC] Add iNOTHING test to the following so we can
	// tell the pPlayer whether he's moving into lava, acid, etc.
	if (Result == iHOLE || Result == iNOTHING)
	{
		if (Result == iHOLE)
		{
			// If hole is more than 1-1/2 times player height, record
			// the total fall height for later damage.
			if (pPlayer->fallHeight == 0)
			{
				pPlayer->fallHeight = pPlayer->z - gFloor;
				pPlayer->startFallZ = pPlayer->z; // [d4-25-97 JPC]
			}
			pPlayer->floor = gFloor;
		}
		if (special == SSP_WATER)
			Result = iSHALLOW_WATER;
		else if (special == SSP_DEEP_WATER)
			Result = iDEEP_WATER;
		else if (special == SSP_LAVA)
			Result = iLAVA;
		else if (special == SSP_ACID_FLOOR)
			Result = iACID;
		if (Result != iNOTHING)
			pPlayer->bump = Result;
	}

#ifdef _STATUS
		if (Result != iNOTHING)
			WriteDebug ("Result = %d", Result);
#endif
	return Result;
}  // CheckMove


/* ========================================================================
   Function    - CheckMoveSimple
   Description - checks for collisions with line segments.
   Returns     - iNOTHING if no collisions, otherwise what you collided with
	Notes			- Cloned and trimmed from CheckMove by JPC on 12-11-96.
					  It does not attempt to jiggle the avatar around so it
					  can move, because it is not intended to be used to
					  move avatars.
					  Use this routine for the camera and mfCanSeeAvatar.
   				  This routine does use 24.8 fixed point x and y.
   ======================================================================== */
WadThingType CheckMoveSimple (
	PLAYER *pPlayer,
	FIXED_VECTOR_3D *pPoint, 	// This is the delta of the motion, not the absolute location.
	SHORT sSpecial,
	LONG *BumpDistance)	// distance to line squared;
{
	FIXED_POINT_3D	targetp;
	LONG        	minDist;                // minimum distance allowed from wall
	WadThingType 	Result = iNOTHING;
	
#ifdef _BUMPLOG
	gCallingFunction = 1;
#endif

	minDist = (pPlayer->w * pPlayer->w); // square the distance to avoid square roots

	// Set the target location.
	targetp.x = pPlayer->x + pPoint->dx;
	targetp.y = pPlayer->y + pPoint->dy;
	targetp.z = pPlayer->z + pPoint->dz;

	Result = CheckLineMove (pPlayer, &targetp, sSpecial, minDist);
	*BumpDistance = gBumpDistance;
	pPlayer->bump = Result;
	return Result;
}  // CheckMoveSimple


/* ========================================================================
   Function    - CheckLongMove
   Description - checks for collisions with line segments.
                 NOTE: check engine.h for macros that set sSpecial
   				 NOTE: this routine does use 24.8 fixed point x and y
   Returns     - true if no collisions, false otherwise

   Note: BumpDistance is returned as the real bump distance and not the
         square.
   ======================================================================== */
WadThingType CheckLongMove(
	PLAYER *pPlayer,
	LONG a,
	LONG distance,
	LONG sSpecial,
	LONG z,
	LONG *BumpDistance)	

{
	LONG 					tmpdist;
	FIXED_VECTOR_3D 	pPoint;
	PLAYER 				tempPlayer;

	tmpdist = distance;
	tempPlayer.x = pPlayer->x;
	tempPlayer.y = pPlayer->y;
	tempPlayer.z = pPlayer->z;
	tempPlayer.h = pPlayer->h;
	tempPlayer.w = pPlayer->w;
	tempPlayer.Flying = pPlayer->Flying;
	tempPlayer.a = pPlayer->a;
	tempPlayer.p = pPlayer->p;
	tempPlayer.ThingIndex = pPlayer->ThingIndex;
	tempPlayer.bump = pPlayer->bump;
	tempPlayer.BumpIndex = pPlayer->BumpIndex;
	a = a % ANGLE_STEPS;

#ifdef _BUMPLOG
	gPlayerx = pPlayer->x>>8;
	gPlayery = pPlayer->y>>8;
#endif

	while (tmpdist > UNIT_DISTANCE)
	{
		pPoint.dx = 0;
		pPoint.dy = UNIT_DISTANCE<<PLAYER_FIXEDPT;
		pPoint.dz = (z-tempPlayer.z) * UNIT_DISTANCE / tmpdist;
		Rotate((POINT*)(&pPoint), a);

		if (iNOTHING != CheckMoveSimple (&tempPlayer, &pPoint, sSpecial, BumpDistance))
		{	
			// [d12-12-96 JPC] We always want to know the actual bump distance
			// (= square root).  No need to check whether tmpdist is different
			// from distance.  If they're equal, we just add a zero.
	  		// if(tmpdist != distance)
			// {
				*BumpDistance = (LONG)(sqrt(*BumpDistance)) + (distance-tmpdist);
			// }
			pPlayer->bump = tempPlayer.bump;
			return tempPlayer.bump;
		}
		
		tmpdist = tmpdist - UNIT_DISTANCE;
		tempPlayer.x += pPoint.dx;
		tempPlayer.y += pPoint.dy;
		tempPlayer.z += pPoint.dz;
	}	
	
	pPoint.dx = 0;
	pPoint.dy = tmpdist<<PLAYER_FIXEDPT;
	pPoint.dz = z-tempPlayer.z;
	Rotate((POINT*)(&pPoint), a);
		 	 	
	CheckMoveSimple (&tempPlayer, &pPoint, sSpecial, BumpDistance);

	// [d12-12-96 JPC] We always want to know the actual bump distance
	// (= square root).  No need to check whether tmpdist is different
	// from distance.  If they're equal, we just add a zero.
	// if(tmpdist != distance)
	// {
	 	*BumpDistance = (LONG)(sqrt(*BumpDistance)) + (distance-tmpdist);
	// }
	pPlayer->bump = tempPlayer.bump;
	return tempPlayer.bump;
} // CheckLongMove


