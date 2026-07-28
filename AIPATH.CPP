/* ========================================================================
   Copyright (c) 1990,1997   Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: AIPATH.CPP
   Author:   (birp-dev pathfinding extension)

   Overview:
	 Implements sector-graph A* pathfinding for in-game AI navigation.

   Map representation:
	 The Doom-style WAD engine stores map geometry as:
	   - linedefs  : wall segments, each with two vertex endpoints and two
					 sidedef references (one per side).
	   - sidedefs  : one entry per linedef side, carrying the sector index.
	   - sectors   : convex-ish floor/ceiling areas.

	 A "portal" is a two-sided, non-IMPASSABLE linedef.  Its midpoint
	 (the average of its two vertex endpoints) is a safe waypoint for
	 crossing from one sector to the adjacent one.

   Graph construction (BuildSectorGraph):
	 Scans every linedef.  A linedef qualifies as a portal when:
	   - Both sidedef references are valid (psdt != -1 and psdb != -1), AND
	   - The IMPASSABLE_BIT flag is clear, AND
	   - The two sidedefs reference different sectors.
	 A bidirectional portal edge is stored in both sector's adjacency list.

   A* search (FindSectorPath):
	 Nodes are sectors.  Edges are portal connections.  Costs are measured
	 from the actual point at which each sector was entered -- a portal
	 midpoint, or the avatar's real position for the origin -- to the next
	 portal midpoint, and the heuristic is the straight-line distance from
	 that point to the target's real position (Option U2).  This makes the
	 chosen sector sequence reflect where the characters actually stand
	 instead of abstract sector centroids.  Nasty sectors (lava, acid, deep
	 water) are excluded from traversal.

   Path smoothing (funnel / string-pulling, Option U1):
	 The sector sequence alone would route through portal midpoints, which
	 zig-zags.  After the search, the portal openings the path crosses are
	 fed -- with their two real endpoints, not just the midpoint -- into the
	 simple funnel algorithm anchored at the avatar's and target's real
	 positions.  The output is a list of corner waypoints that hug wall
	 corners; a clear straight line collapses to zero waypoints.

   Object avoidance (Option U3) and clearance/doors (Option U4):
	 When mover geometry is supplied (AI_MOVER_INFO, non-flying), the search
	 additionally rejects any portal too narrow for the mover or whose
	 floor/ceiling opening is too short -- which excludes gaps large creatures
	 cannot fit through and closed/locked doors and low passages (their door
	 sector's ceiling sits at the floor) -- and the funnelled path is nudged
	 around free-standing "things" (furniture, barrels, statues) that the
	 sector graph itself does not model.
   ======================================================================== */

#include <stdlib.h>
#include <string.h>

#include "SYSTEM.H"
#include "ENGINE.H"
#include "ENGINT.H"
#include "SPECIAL.H"
#include "THINGS.H"
#include "THINGTYP.H"
#include "AIPATH.HXX"

/* ========================================================================
   Compile-time capacity limit.
   All known combat WADs have well under 1024 sectors; guard against
   pathological or future maps without dynamic allocation for A* state.
   ======================================================================== */
#define AIPATH_MAX_SECTORS 1024

/* ========================================================================
   Private types
   ======================================================================== */

/**
 * @brief One directed portal edge leading from a sector to an adjacent one.
 */
struct SectorPortal
{
	LONG toSector;  /**< Index of the sector on the other side of the portal. */
	LONG midX;      /**< X world-coordinate of the portal linedef midpoint. */
	LONG midY;      /**< Y world-coordinate of the portal linedef midpoint. */
	LONG ax;        /**< X world-coordinate of portal endpoint A (vertex a). */
	LONG ay;        /**< Y world-coordinate of portal endpoint A. */
	LONG bx;        /**< X world-coordinate of portal endpoint B (vertex b). */
	LONG by;        /**< Y world-coordinate of portal endpoint B. */
};

/**
 * @brief Per-sector node in the sector connectivity graph.
 *
 * @var portals     Heap-allocated array of outbound portal edges.
 * @var numPortals  Number of valid entries in portals[].
 * @var centerX     Approximate sector centre X, derived as the centroid of
 *                  all connected portal midpoints.  Used as the A* heuristic
 *                  origin; not required to be the geometric centroid.
 * @var centerY     Approximate sector centre Y.
 */
struct SectorNode
{
	SectorPortal *portals;
	int           numPortals;
	LONG          centerX;
	LONG          centerY;
};

/* ========================================================================
   Module-level state
   ======================================================================== */

/** @brief Array of SectorNode, one per sector in the current WAD. NULL when no graph exists. */
static SectorNode *gSectorGraph = NULL;

/** @brief Number of sectors in gSectorGraph; 0 when the graph has been freed. */
static int gSectorGraphCount = 0;

/* ========================================================================
   Private helper functions
   ======================================================================== */

/* ========================================================================
   Function    - IsNastySpecial
   Description - Returns TRUE if a sector special marks impassable terrain.
   ======================================================================== */
/**
 * @brief Returns TRUE if a sector special code represents hazardous terrain
 *        that the AI should not route through.
 *
 * Matches the same set of specials checked by CAvatar::mfIsNastySector().
 *
 * @param special  The sector special value from BR_SECTOR::special.
 * @return TRUE if the sector is lava, acid floor, or deep water.
 */
static BOOL IsNastySpecial(LONG special)
{
	return (special == (LONG)SSP_LAVA       ||
			special == (LONG)SSP_ACID_FLOOR ||
			special == (LONG)SSP_DEEP_WATER) ? TRUE : FALSE;
}

/* ========================================================================
   Function    - TriArea2
   Description - Signed area (x2) of triangle ABC; funnel left/right test.
   ======================================================================== */
/**
 * @brief Twice the signed area of triangle (A, B, C).
 *
 * Positive when C is to the left of the directed segment A->B, negative when
 * to the right, zero when collinear.  Used by the funnel algorithm to decide
 * whether a new portal endpoint tightens or crosses the current funnel.
 *
 * @note Computed in double precision: the operands are differences of WAD
 *       world coordinates (SHORT range), whose products can exceed the 32-bit
 *       signed range for map-spanning funnels.  Only the sign is consumed, so
 *       double is exact for this purpose (matches the engine's LinePointDistance).
 */
static double TriArea2(LONG ax, LONG ay, LONG bx, LONG by, LONG cx, LONG cy)
{
	return (double)(bx - ax) * (double)(cy - ay)
		 - (double)(cx - ax) * (double)(by - ay);
}

/* ========================================================================
   Function    - FunnelStringPull
   Description - Simple funnel ("string-pulling") over a portal channel.
   ======================================================================== */
/**
 * @brief Smooth a portal channel into a list of corner waypoints.
 *
 * Implements Mononen's "simple stupid funnel algorithm".  The channel is an
 * ordered list of portals, each given as a left and a right endpoint, where
 * the first and last "portals" are degenerate points equal to the real start
 * and target positions.  The result is the shortest taut path through the
 * openings: a sequence of corner points the character can walk in straight
 * legs.
 *
 * @param leftX/leftY    Left endpoint of each portal,  index 0..nportals-1.
 * @param rightX/rightY  Right endpoint of each portal, index 0..nportals-1.
 * @param nportals       Number of channel entries (>= 2: start + target).
 * @param outX/outY      Output corner points, including the start apex and
 *                       the final goal point.
 * @param maxOut         Capacity of outX/outY.
 * @return Number of corner points written (>= 2 unless clipped by maxOut).
 */
static int FunnelStringPull(const LONG *leftX,  const LONG *leftY,
							const LONG *rightX, const LONG *rightY,
							int nportals,
							LONG *outX, LONG *outY, int maxOut)
{
	int  npts = 0;
	LONG apexX  = leftX[0],  apexY  = leftY[0];
	LONG portLX = leftX[0],  portLY = leftY[0];
	LONG portRX = rightX[0], portRY = rightY[0];
	int  apexIndex = 0, leftIndex = 0, rightIndex = 0;

	if (maxOut <= 0)
		return 0;

	/* Always start with the apex (the real start position). */
	outX[npts] = apexX;  outY[npts] = apexY;  ++npts;

	for (int i = 1; i < nportals && npts < maxOut; ++i)
	{
		const LONG lX = leftX[i],  lY = leftY[i];
		const LONG rX = rightX[i], rY = rightY[i];

		/* --- Update the right side of the funnel. --- */
		if (TriArea2(apexX, apexY, portRX, portRY, rX, rY) <= 0)
		{
			if ((apexX == portRX && apexY == portRY) ||
				TriArea2(apexX, apexY, portLX, portLY, rX, rY) > 0)
			{
				/* Tighten the right side. */
				portRX = rX;  portRY = rY;  rightIndex = i;
			}
			else
			{
				/* Right over left: the left endpoint is a corner.  Emit it
				   and restart the funnel from there. */
				if (npts < maxOut)
				{
					outX[npts] = portLX;  outY[npts] = portLY;  ++npts;
				}
				apexX = portLX;  apexY = portLY;  apexIndex = leftIndex;
				portLX = apexX;  portLY = apexY;
				portRX = apexX;  portRY = apexY;
				leftIndex = apexIndex;  rightIndex = apexIndex;
				i = apexIndex;
				continue;
			}
		}

		/* --- Update the left side of the funnel. --- */
		if (TriArea2(apexX, apexY, portLX, portLY, lX, lY) >= 0)
		{
			if ((apexX == portLX && apexY == portLY) ||
				TriArea2(apexX, apexY, portRX, portRY, lX, lY) < 0)
			{
				/* Tighten the left side. */
				portLX = lX;  portLY = lY;  leftIndex = i;
			}
			else
			{
				/* Left over right: the right endpoint is a corner. */
				if (npts < maxOut)
				{
					outX[npts] = portRX;  outY[npts] = portRY;  ++npts;
				}
				apexX = portRX;  apexY = portRY;  apexIndex = rightIndex;
				portLX = apexX;  portLY = apexY;
				portRX = apexX;  portRY = apexY;
				leftIndex = apexIndex;  rightIndex = apexIndex;
				i = apexIndex;
				continue;
			}
		}
	}

	/* Append the goal point (left == right == target on the last entry). */
	if (npts < maxOut)
	{
		outX[npts] = leftX[nportals - 1];
		outY[npts] = leftY[nportals - 1];
		++npts;
	}

	return npts;
}

/* ========================================================================
   Function    - PortalClearanceOK
   Description - U4 gate: can the mover fit through this portal crossing?
   ======================================================================== */
/**
 * @brief Returns TRUE if a non-flying mover can fit through the portal from
 *        sector @p secA to sector @p secB (Option U4).
 *
 * Two tests, both purely from map geometry:
 *   - Horizontal: the portal opening (distance between its endpoints) must be
 *     at least the mover's width, or a large creature cannot pass.
 *   - Vertical: the headroom across the crossing -- the lower of the two
 *     ceilings minus the higher of the two floors -- must be at least the
 *     mover's height.  A closed/locked door's sector has its ceiling at the
 *     floor, so this rejects it; it also rejects passages too low to enter.
 *
 * @param pt     The portal edge (endpoints carry the opening width).
 * @param secA   One side's sector index.
 * @param secB   The other side's sector index.
 * @param pMover Mover geometry (caller guarantees non-NULL, non-flying).
 * @return TRUE if passable, FALSE if too narrow or too short.
 */
static BOOL PortalClearanceOK(const SectorPortal *pt, int secA, int secB,
							  const AI_MOVER_INFO *pMover)
{
	/* Horizontal opening = portal segment length. */
	const LONG openWidth = aprox_dist(pt->ax, pt->ay, pt->bx, pt->by);
	if (openWidth < pMover->width)
		return FALSE;

	/* Vertical opening = min(ceilings) - max(floors) across the crossing. */
	if (sectors)
	{
		const LONG cA = sectors[secA].ch, fA = sectors[secA].fh;
		const LONG cB = sectors[secB].ch, fB = sectors[secB].fh;
		const LONG ceil  = (cA < cB) ? cA : cB;
		const LONG floor = (fA > fB) ? fA : fB;
		if ((ceil - floor) < pMover->height)
			return FALSE;
	}

	return TRUE;
}

/* ========================================================================
   Function    - SegmentBlockingThing
   Description - U3 helper: find the scenery cylinder a segment runs through.
   ======================================================================== */
/**
 * @brief Find the free-standing "thing" (furniture/scenery) whose cylinder a
 *        path segment passes closest through, if any (Option U3).
 *
 * Considers only valid, bumpable scenery (thing type > LAST_GAME_AVATAR, which
 * excludes all avatars/creatures -- including the mover and its target -- so
 * dynamic combatants are left to the movement-time bump steering).  A thing
 * blocks when the segment passes within (mover half-width + thing half-width)
 * of its centre with the mover's body vertically overlapping it.
 *
 * @param x0/y0,x1/y1  Segment endpoints.
 * @param pMover       Mover geometry.
 * @param pFootX/pFootY  Out: nearest point on the segment to the blocker.
 * @param pClearance     Out: required clearance radius from the blocker centre.
 * @return Thing index of the deepest-intruding blocker, or -1 if the segment
 *         is clear.
 */
static int SegmentBlockingThing(LONG x0, LONG y0, LONG x1, LONG y1,
								const AI_MOVER_INFO *pMover,
								LONG *pFootX, LONG *pFootY, LONG *pClearance)
{
	/* Double precision: segment deltas are WAD-coordinate differences whose
	   squares can exceed 32-bit for long corridors. */
	const double segDX = (double)(x1 - x0);
	const double segDY = (double)(y1 - y0);
	const double lenSq = segDX * segDX + segDY * segDY;
	if (lenSq <= 0.0)
		return -1; /* degenerate segment */

	const LONG moverHalf       = pMover->width / 2;
	const LONG moverHeadHeight = pMover->z + pMover->height;

	int  best        = -1;
	LONG bestIntrude = 0;       /* how far inside the clearance the centre sits */

	for (LONG j = MaxThingLoaded; j > 0; --j)
	{
		if (mythings[j].valid == FALSE)         continue;
		if (mythings[j].iBitm == fERROR)        continue;
		if (mythings[j].type <= LAST_GAME_AVATAR) continue; /* skip avatars/creatures */
		if (!IsBumpable(j))                     continue;

		/* Vertical overlap: mover's body must share height with the thing. */
		const LONG thingHead = (LONG)mythings[j].z + (LONG)mythings[j].heiScaled;
		if (moverHeadHeight <= (LONG)mythings[j].z || pMover->z >= thingHead)
			continue;

		const LONG thingHalf = IsHalfWidthBumpable(j)
							 ? (LONG)mythings[j].widScaled / 4
							 : (LONG)mythings[j].widScaled / 2;
		const LONG clearance = moverHalf + thingHalf;

		/* Project the thing centre onto the segment, clamped to its span. */
		const LONG cx = mythings[j].x;
		const LONG cy = mythings[j].y;
		double tNum = (double)(cx - x0) * segDX + (double)(cy - y0) * segDY;
		if (tNum < 0.0)         tNum = 0.0;
		else if (tNum > lenSq)  tNum = lenSq;

		const LONG footX = x0 + (LONG)((segDX * tNum) / lenSq);
		const LONG footY = y0 + (LONG)((segDY * tNum) / lenSq);

		const LONG d = aprox_dist(cx, cy, footX, footY);
		if (d < clearance)
		{
			const LONG intrude = clearance - d;
			if (intrude > bestIntrude)
			{
				bestIntrude = intrude;
				best        = (int)j;
				*pFootX     = footX;
				*pFootY     = footY;
				*pClearance = clearance;
			}
		}
	}

	return best;
}

/* ========================================================================
   Function    - ThingSweepDetours
   Description - U3: insert detour corners around blocking scenery.
   ======================================================================== */
/**
 * @brief Walk a funnelled polyline and bow each segment around the scenery it
 *        would otherwise run through, producing an expanded polyline.
 *
 * For each input segment a single (deepest-intruding) blocker is detoured: a
 * waypoint is placed on the path side of the blocker, pushed perpendicular to
 * (clearance + margin) from the blocker centre so the mover skirts the
 * cylinder.  One detour per segment keeps the pass O(corners x things) and
 * bounded; residual contact is handled by movement-time bump steering.
 *
 * @param inX/inY    Input polyline (includes start apex and goal).
 * @param nin        Number of input points.
 * @param pMover     Mover geometry.
 * @param outX/outY  Output polyline (start, detours, goal).
 * @param maxOut     Capacity of the output arrays.
 * @return Number of output points written.
 */
static int ThingSweepDetours(const LONG *inX, const LONG *inY, int nin,
							 const AI_MOVER_INFO *pMover,
							 LONG *outX, LONG *outY, int maxOut)
{
	int nout = 0;

	for (int i = 0; i < nin - 1; ++i)
	{
		/* Emit the segment's start point. */
		if (nout < maxOut) { outX[nout] = inX[i]; outY[nout] = inY[i]; ++nout; }

		LONG footX, footY, clearance;
		const int blocker = SegmentBlockingThing(inX[i], inY[i],
			inX[i + 1], inY[i + 1], pMover, &footX, &footY, &clearance);
		if (blocker < 0)
			continue;

		/* Push a detour waypoint away from the blocker centre, through the
		   nearest point on the segment, out to (clearance + margin). */
		const LONG cx = mythings[blocker].x;
		const LONG cy = mythings[blocker].y;
		LONG awayX = footX - cx;
		LONG awayY = footY - cy;
		LONG awayLen = aprox_dist(cx, cy, footX, footY);

		if (awayLen <= 0)
		{
			/* Segment passes through the centre: bow perpendicular to it. */
			awayX = -(inY[i + 1] - inY[i]);
			awayY =  (inX[i + 1] - inX[i]);
			awayLen = aprox_dist(0, 0, awayX, awayY);
			if (awayLen <= 0)
				continue;
		}

		const LONG reach = clearance + AIPATH_THING_CLEARANCE_MARGIN;
		const LONG detourX = cx + (LONG)(((double)awayX * reach) / awayLen);
		const LONG detourY = cy + (LONG)(((double)awayY * reach) / awayLen);

		if (nout < maxOut) { outX[nout] = detourX; outY[nout] = detourY; ++nout; }
	}

	/* Emit the final point. */
	if (nin > 0 && nout < maxOut)
	{
		outX[nout] = inX[nin - 1];
		outY[nout] = inY[nin - 1];
		++nout;
	}

	return nout;
}

/* ========================================================================
   Public interface
   ======================================================================== */

/* ========================================================================
   Function    - BuildSectorGraph
   ======================================================================== */
/**
 * @brief Build the sector connectivity graph from the currently loaded WAD.
 *
 * Two-pass algorithm:
 *
 *   Pass 1 -- Count qualifying portals per sector so that portal arrays can
 *             be allocated exactly to size.  A linedef qualifies when both
 *             sidedef references are valid, IMPASSABLE_BIT is clear, and
 *             the two sides reference distinct sectors.
 *
 *   Pass 2 -- Fill each sector's portal array with the midpoint coordinates
 *             and the adjacent-sector index.  Simultaneously accumulate the
 *             sum of portal midpoints per sector to derive the centre
 *             coordinate used by the A* heuristic.
 *
 * Memory:
 *   gSectorGraph[] is a heap-allocated SectorNode array (one per sector).
 *   Each SectorNode's portals[] is separately heap-allocated and exactly
 *   sized to the number of portals found in pass 1.
 *
 * Error handling:
 *   If any allocation fails the function frees whatever it had already
 *   allocated and leaves the graph in the "not built" state so that
 *   FindSectorPath() falls back to direct movement.
 */
void BuildSectorGraph()
{
	FreeSectorGraph();

	if (tot_sectors == 0 || tot_linedefs == 0)
	{
		return;
	}

	if ((int)tot_sectors > AIPATH_MAX_SECTORS)
	{
		return;
	}

	gSectorGraphCount = (int)tot_sectors;
	gSectorGraph = (SectorNode*)calloc(gSectorGraphCount, sizeof(SectorNode));
	if (!gSectorGraph)
	{
		gSectorGraphCount = 0;
		return;
	}

	/* Temporary scratch arrays -- freed before returning. */
	int  *portalCounts = (int*)calloc(gSectorGraphCount, sizeof(int));
	LONG *sumX         = (LONG*)calloc(gSectorGraphCount, sizeof(LONG));
	LONG *sumY         = (LONG*)calloc(gSectorGraphCount, sizeof(LONG));
	int  *centerCount  = (int*)calloc(gSectorGraphCount, sizeof(int));

	if (!portalCounts || !sumX || !sumY || !centerCount)
	{
		free(portalCounts);
		free(sumX);
		free(sumY);
		free(centerCount);
		FreeSectorGraph();
		return;
	}

	/* ------------------------------------------------------------------
	   Pass 1: count portal edges per sector.
	   ------------------------------------------------------------------ */
	for (ULONG i = 0; i < tot_linedefs; ++i)
	{
		const LINEDEF *ld = &linedefs[i];

		/* One-sided wall -- solid, not a portal. */
		if (ld->psdt == -1 || ld->psdb == -1)
			continue;

		/* Explicitly impassable linedef. */
		if (ld->flags & IMPASSABLE_BIT)
			continue;

		int sA = (int)sidedefs[ld->psdt].sec_ptr;
		int sB = (int)sidedefs[ld->psdb].sec_ptr;

		/* Degenerate self-portal (malformed WAD). */
		if (sA == sB)
			continue;

		/* Out-of-range sector references (malformed WAD). */
		if (sA < 0 || sA >= gSectorGraphCount ||
			sB < 0 || sB >= gSectorGraphCount)
			continue;

		++portalCounts[sA];
		++portalCounts[sB];
	}

	/* Allocate portal arrays.  portalCounts[] is then reused as write cursors
	   in pass 2 (reset to 0 after allocation). */
	for (int s = 0; s < gSectorGraphCount; ++s)
	{
		if (portalCounts[s] > 0)
		{
			gSectorGraph[s].portals = (SectorPortal*)calloc(
				portalCounts[s], sizeof(SectorPortal));
			if (!gSectorGraph[s].portals)
			{
				/* Proceed; sectors with no portal array are treated as dead-ends. */
			}
		}
		portalCounts[s] = 0; /* reset for use as write cursor in pass 2 */
	}

	/* ------------------------------------------------------------------
	   Pass 2: fill portal arrays and accumulate sector centre data.
	   ------------------------------------------------------------------ */
	for (ULONG i = 0; i < tot_linedefs; ++i)
	{
		const LINEDEF *ld = &linedefs[i];
		if (ld->psdt == -1 || ld->psdb == -1)
			continue;
		if (ld->flags & IMPASSABLE_BIT)
			continue;

		int sA = (int)sidedefs[ld->psdt].sec_ptr;
		int sB = (int)sidedefs[ld->psdb].sec_ptr;
		if (sA == sB)
			continue;
		if (sA < 0 || sA >= gSectorGraphCount ||
			sB < 0 || sB >= gSectorGraphCount)
			continue;

		const LONG ax   = (LONG)vertexs[ld->a].x;
		const LONG ay   = (LONG)vertexs[ld->a].y;
		const LONG bx   = (LONG)vertexs[ld->b].x;
		const LONG by   = (LONG)vertexs[ld->b].y;
		const LONG midX = (ax + bx) / 2;
		const LONG midY = (ay + by) / 2;

		/* Add A -> B directed portal. */
		if (gSectorGraph[sA].portals)
		{
			int idx = portalCounts[sA]++;
			gSectorGraph[sA].portals[idx].toSector = (LONG)sB;
			gSectorGraph[sA].portals[idx].midX     = midX;
			gSectorGraph[sA].portals[idx].midY     = midY;
			gSectorGraph[sA].portals[idx].ax       = ax;
			gSectorGraph[sA].portals[idx].ay       = ay;
			gSectorGraph[sA].portals[idx].bx       = bx;
			gSectorGraph[sA].portals[idx].by       = by;
		}

		/* Add B -> A directed portal. */
		if (gSectorGraph[sB].portals)
		{
			int idx = portalCounts[sB]++;
			gSectorGraph[sB].portals[idx].toSector = (LONG)sA;
			gSectorGraph[sB].portals[idx].midX     = midX;
			gSectorGraph[sB].portals[idx].midY     = midY;
			gSectorGraph[sB].portals[idx].ax       = ax;
			gSectorGraph[sB].portals[idx].ay       = ay;
			gSectorGraph[sB].portals[idx].bx       = bx;
			gSectorGraph[sB].portals[idx].by       = by;
		}

		/* Accumulate portal midpoints for sector centre estimation. */
		sumX[sA] += midX;  sumY[sA] += midY;  ++centerCount[sA];
		sumX[sB] += midX;  sumY[sB] += midY;  ++centerCount[sB];
	}

	/* Finalise each sector node. */
	for (int s = 0; s < gSectorGraphCount; ++s)
	{
		gSectorGraph[s].numPortals = portalCounts[s];
		if (centerCount[s] > 0)
		{
			gSectorGraph[s].centerX = sumX[s] / centerCount[s];
			gSectorGraph[s].centerY = sumY[s] / centerCount[s];
		}
		/* Sectors with no portals retain (0,0) as their centre -- they are
		   isolated rooms reachable only by teleport and will never appear
		   in a FindSectorPath result. */
	}

	free(portalCounts);
	free(sumX);
	free(sumY);
	free(centerCount);

}

/* ========================================================================
   Function    - FreeSectorGraph
   ======================================================================== */
/**
 * @brief Free all heap memory owned by the sector graph.
 *
 * Iterates gSectorGraph[], freeing each sector's portal array, then frees
 * gSectorGraph itself and resets the count to zero.
 */
void FreeSectorGraph()
{
	if (!gSectorGraph)
		return;

	for (int s = 0; s < gSectorGraphCount; ++s)
	{
		if (gSectorGraph[s].portals)
		{
			free(gSectorGraph[s].portals);
			gSectorGraph[s].portals = NULL;
		}
	}

	free(gSectorGraph);
	gSectorGraph      = NULL;
	gSectorGraphCount = 0;

}

/* ========================================================================
   Function    - FindSectorPath
   ======================================================================== */
/**
 * @brief A* search on the sector graph from fromSector to toSector, with
 *        funnel-smoothed output anchored at real start/target positions.
 *
 * Algorithm:
 *   1. Initialise g-costs to INT_MAX and mark all sectors as unvisited.
 *      Record the avatar's real (startX, startY) as the origin's arrival point.
 *   2. Add fromSector to the open set with g=0 and h=dist(start, target).
 *   3. Each iteration: extract the lowest-f open sector.
 *      For each outbound portal:
 *        - Skip sectors in the closed set.
 *        - Skip sectors with nasty specials (lava, acid, deep water).
 *        - Compute tentative g = g(current) + dist(arrival[current], portal mid)
 *          where arrival[current] is the actual point this sector was entered.
 *        - If better than known g: record parent portal, store the portal
 *          midpoint as next's arrival point, update g and f (h = dist(portal
 *          mid, real target)), and add to the open set.
 *   4. When toSector is expanded, reconstruct the sequence of portals crossed
 *      and run the funnel algorithm over their real endpoints, anchored at the
 *      real start and target, to produce smoothed corner waypoints (Option U1).
 *
 * Open-set strategy:
 *   Linear scan over the sector array.  With at most AIPATH_MAX_SECTORS (~hundreds)
 *   sectors, this is fast enough to run per-target-acquisition without measurable
 *   frame-time impact.  A heap would be needed only if sector counts grow to
 *   thousands.
 *
 * State arrays:
 *   Declared static so no per-call heap allocation is needed.  The function
 *   is not re-entrant, but re-entrancy is not required -- the game loop
 *   processes one AI character at a time.
 *
 * @param fromSector    Starting sector index.
 * @param toSector      Destination sector index.
 * @param startX        Avatar's real X position (cost/funnel start anchor).
 * @param startY        Avatar's real Y position.
 * @param targetX       Target's real X position (cost/funnel goal anchor).
 * @param targetY       Target's real Y position.
 * @param waypointsX    Output: X world-coordinates of funnel corners, travel order.
 * @param waypointsY    Output: Y world-coordinates of funnel corners, travel order.
 * @param maxWaypoints  Capacity of waypointsX / waypointsY.
 * @param pMover        Optional mover geometry; non-NULL non-flying enables the
 *                      U4 portal clearance/door gate and the U3 thing detours.
 * @return  Number of waypoints written (>= 0) on success, or -1 if no path exists.
 *          0 means no intermediate corner is needed (move directly to target).
 */
int FindSectorPath(LONG fromSector, LONG toSector,
				   LONG startX, LONG startY,
				   LONG targetX, LONG targetY,
				   LONG *waypointsX, LONG *waypointsY, int maxWaypoints,
				   const AI_MOVER_INFO *pMover)
{
	/* U3/U4 apply only to non-flying movers with geometry supplied. */
	const BOOL useGeom = (pMover != NULL && !pMover->isFlying) ? TRUE : FALSE;

	if (fromSector == toSector)
		return 0;

	if (!gSectorGraph || gSectorGraphCount == 0)
	{
		return -1;
	}

	if (fromSector < 0 || fromSector >= (LONG)gSectorGraphCount ||
		toSector   < 0 || toSector   >= (LONG)gSectorGraphCount)
	{
		return -1;
	}

	/* ------------------------------------------------------------------
	   A* state -- static to avoid per-call allocation overhead.
	   ------------------------------------------------------------------ */
	static LONG  gCost[AIPATH_MAX_SECTORS];   /**< Best known cost from fromSector. */
	static LONG  fScore[AIPATH_MAX_SECTORS];  /**< g + h estimate to toSector. */
	static int   parent[AIPATH_MAX_SECTORS];  /**< Sector we arrived from. */
	static int   parentPortal[AIPATH_MAX_SECTORS]; /**< Portal index in parent that leads here. */
	static BOOL  inOpen[AIPATH_MAX_SECTORS];  /**< Sector is in the open set. */
	static BOOL  inClosed[AIPATH_MAX_SECTORS];/**< Sector has been fully expanded. */
	static LONG  arriveX[AIPATH_MAX_SECTORS]; /**< Real point this sector was entered (X). */
	static LONG  arriveY[AIPATH_MAX_SECTORS]; /**< Real point this sector was entered (Y). */

	const int from = (int)fromSector;
	const int to   = (int)toSector;

	for (int i = 0; i < gSectorGraphCount; ++i)
	{
		gCost[i]        = 0x7FFFFFFF;
		fScore[i]       = 0x7FFFFFFF;
		parent[i]       = -1;
		parentPortal[i] = -1;
		inOpen[i]       = FALSE;
		inClosed[i]     = FALSE;
	}

	gCost[from]   = 0;
	arriveX[from] = startX;
	arriveY[from] = startY;
	fScore[from]  = aprox_dist(startX, startY, targetX, targetY);
	inOpen[from]  = TRUE;

	/* ------------------------------------------------------------------
	   Main A* loop.
	   ------------------------------------------------------------------ */
	for (;;)
	{
		/* Find the open sector with the lowest f-score (linear scan). */
		int  current = -1;
		LONG bestF   = 0x7FFFFFFF;
		for (int i = 0; i < gSectorGraphCount; ++i)
		{
			if (inOpen[i] && fScore[i] < bestF)
			{
				bestF   = fScore[i];
				current = i;
			}
		}

		if (current == -1)
		{
			return -1; /* open set exhausted -- no path */
		}

		if (current == to)
			break; /* destination reached */

		inOpen[current]   = FALSE;
		inClosed[current] = TRUE;

		for (int p = 0; p < gSectorGraph[current].numPortals; ++p)
		{
			int next = (int)gSectorGraph[current].portals[p].toSector;
			if (next < 0 || next >= gSectorGraphCount)
				continue;
			if (inClosed[next])
				continue;
			if (sectors && IsNastySpecial(sectors[next].special))
				continue;

			/* U4: reject portals the mover cannot fit through -- too narrow,
			   or too short (closed/locked doors, low passages). */
			if (useGeom &&
				!PortalClearanceOK(&gSectorGraph[current].portals[p],
								   current, next, pMover))
				continue;

			/* Edge cost: distance from the point we actually entered "current"
			   (a portal midpoint, or the avatar's real start position) to this
			   portal's midpoint -- the point at which we would enter "next". */
			const LONG px = gSectorGraph[current].portals[p].midX;
			const LONG py = gSectorGraph[current].portals[p].midY;
			LONG tentG = gCost[current] +
				aprox_dist(arriveX[current], arriveY[current], px, py);
			if (tentG < 0)
				tentG = 0x7FFFFFFE; /* overflow guard */

			if (tentG < gCost[next])
			{
				parent[next]       = current;
				parentPortal[next] = p;
				gCost[next]        = tentG;
				arriveX[next]      = px;
				arriveY[next]      = py;
				/* Heuristic: straight-line from this entry point to the real
				   target position (admissible lower bound on remaining travel). */
				fScore[next]       = tentG + aprox_dist(px, py, targetX, targetY);
				inOpen[next]       = TRUE;
			}
		}
	}

	/* ------------------------------------------------------------------
	   Reconstruct path: walk back from toSector to fromSector, collecting
	   the portals crossed in reverse.  For each portal, resolve which
	   endpoint is on the left vs. right of the travel direction (the line
	   between the two sectors' centroids) so the funnel can string-pull a
	   taut path through the openings.
	   ------------------------------------------------------------------ */
	static LONG revLX[AIPATH_MAX_SECTORS]; /**< Left endpoint of each portal, reverse order. */
	static LONG revLY[AIPATH_MAX_SECTORS];
	static LONG revRX[AIPATH_MAX_SECTORS]; /**< Right endpoint of each portal, reverse order. */
	static LONG revRY[AIPATH_MAX_SECTORS];
	int revLen = 0;
	int cur    = to;

	while (parent[cur] != -1 && revLen < AIPATH_MAX_SECTORS)
	{
		const int prevSector = parent[cur];
		const int portalIdx  = parentPortal[cur];
		const SectorPortal *pt = &gSectorGraph[prevSector].portals[portalIdx];

		/* Travel direction across this portal: prevSector centroid -> cur centroid. */
		const LONG cfx = gSectorGraph[prevSector].centerX;
		const LONG cfy = gSectorGraph[prevSector].centerY;
		const LONG ctx = gSectorGraph[cur].centerX;
		const LONG cty = gSectorGraph[cur].centerY;

		/* Endpoint A on the left of the travel direction? (>= 0 => left) */
		if (TriArea2(cfx, cfy, ctx, cty, pt->ax, pt->ay) >= 0)
		{
			revLX[revLen] = pt->ax;  revLY[revLen] = pt->ay;  /* A left  */
			revRX[revLen] = pt->bx;  revRY[revLen] = pt->by;  /* B right */
		}
		else
		{
			revLX[revLen] = pt->bx;  revLY[revLen] = pt->by;
			revRX[revLen] = pt->ax;  revRY[revLen] = pt->ay;
		}
		++revLen;
		cur = prevSector;
	}

	/* ------------------------------------------------------------------
	   Build the funnel channel in forward (travel) order: real start anchor,
	   each portal's left/right endpoints, then the real target anchor.
	   ------------------------------------------------------------------ */
	static LONG chanLX[AIPATH_MAX_SECTORS + 2];
	static LONG chanLY[AIPATH_MAX_SECTORS + 2];
	static LONG chanRX[AIPATH_MAX_SECTORS + 2];
	static LONG chanRY[AIPATH_MAX_SECTORS + 2];
	int nchan = 0;

	chanLX[nchan] = startX;  chanLY[nchan] = startY;
	chanRX[nchan] = startX;  chanRY[nchan] = startY;
	++nchan;

	for (int i = revLen - 1; i >= 0; --i)
	{
		chanLX[nchan] = revLX[i];  chanLY[nchan] = revLY[i];
		chanRX[nchan] = revRX[i];  chanRY[nchan] = revRY[i];
		++nchan;
	}

	chanLX[nchan] = targetX;  chanLY[nchan] = targetY;
	chanRX[nchan] = targetX;  chanRY[nchan] = targetY;
	++nchan;

	/* ------------------------------------------------------------------
	   String-pull the channel into corner points (start apex .. goal).
	   ------------------------------------------------------------------ */
	static LONG cornerX[AIPATH_MAX_SECTORS + 2];
	static LONG cornerY[AIPATH_MAX_SECTORS + 2];
	int   ncorners = FunnelStringPull(chanLX, chanLY, chanRX, chanRY,
		nchan, cornerX, cornerY, AIPATH_MAX_SECTORS + 2);

	/* ------------------------------------------------------------------
	   U3: bow the funnelled polyline around free-standing scenery the sector
	   graph does not model.  One detour may be inserted per segment, so the
	   expanded polyline can be up to twice as long.
	   ------------------------------------------------------------------ */
	const LONG *pathX = cornerX;
	const LONG *pathY = cornerY;
	int         pathN = ncorners;

	static LONG sweptX[(AIPATH_MAX_SECTORS + 2) * 2];
	static LONG sweptY[(AIPATH_MAX_SECTORS + 2) * 2];
	if (useGeom && ncorners >= 2 && mythings)
	{
		const int nswept = ThingSweepDetours(cornerX, cornerY, ncorners,
			pMover, sweptX, sweptY, (AIPATH_MAX_SECTORS + 2) * 2);
		if (nswept >= 2)
		{
			pathX = sweptX;
			pathY = sweptY;
			pathN = nswept;
		}
	}

	/* Emit only the intermediate points (skip the start apex and the goal);
	   a clear straight line collapses to zero waypoints. */
	int count = 0;
	for (int i = 1; i < pathN - 1 && count < maxWaypoints; ++i)
	{
		waypointsX[count] = pathX[i];
		waypointsY[count] = pathY[i];
		++count;
	}

	return count;
}

/* ========================================================================
   Function    - ApplySectorPath
   ======================================================================== */
/**
 * @brief Per-frame sector-path update for one AI character in AI_MOVING.
 *
 * See AIPATH.HXX for the full description and call-site pattern.
 */
void ApplySectorPath(AI_PATH_STATE *pPath,
					 LONG fromSector, LONG toSector,
					 LONG avatarX, LONG avatarY,
					 LONG targetX, LONG targetY,
					 LONG *pMoveTargetX, LONG *pMoveTargetY,
					 const AI_MOVER_INFO *pMover)
{
	/* Default: aim directly at the target. */
	*pMoveTargetX = targetX;
	*pMoveTargetY = targetY;

	if (fromSector == toSector)
	{
		/* Same sector -- clear any stale cached path and use direct movement. */
		pPath->fPathWaypointCount = 0;
		pPath->fPathTargetSector  = -1;
		return;
	}

	/* Recompute path when the target has moved to a new sector. */
	if (pPath->fPathTargetSector != toSector)
	{
		pPath->fPathWaypointCount = FindSectorPath(fromSector, toSector,
			avatarX, avatarY, targetX, targetY,
			pPath->fPathWaypointsX, pPath->fPathWaypointsY, AIPATH_MAX_WAYPOINTS,
			pMover);
		pPath->fPathWaypointIndex = 0;
		pPath->fPathTargetSector  = toSector;
	}

	/* Follow the current waypoint if the path has remaining entries. */
	if (pPath->fPathWaypointCount > 0 &&
		pPath->fPathWaypointIndex < pPath->fPathWaypointCount)
	{
		*pMoveTargetX = pPath->fPathWaypointsX[pPath->fPathWaypointIndex];
		*pMoveTargetY = pPath->fPathWaypointsY[pPath->fPathWaypointIndex];

		const LONG wpDist = aprox_dist(avatarX, avatarY, *pMoveTargetX, *pMoveTargetY);
		if (wpDist <= (LONG)AIPATH_WAYPOINT_REACH_DISTANCE)
		{
			++pPath->fPathWaypointIndex;
			if (pPath->fPathWaypointIndex < pPath->fPathWaypointCount)
			{
				*pMoveTargetX = pPath->fPathWaypointsX[pPath->fPathWaypointIndex];
				*pMoveTargetY = pPath->fPathWaypointsY[pPath->fPathWaypointIndex];
			}
			else
			{
				/* All waypoints consumed -- switch to direct movement. */
				*pMoveTargetX = targetX;
				*pMoveTargetY = targetY;
			}
		}
	}
	/* else: no path found (count == -1) or not yet computed (count == 0) --
	   *pMoveTargetX/Y already set to the direct target above. */
}
