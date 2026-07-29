/* ========================================================================
   Copyright (c) 1990,1997   Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: AIPATH.HXX
   Author:   (birp-dev pathfinding extension)

   Overview:
	 Public interface for sector-graph A* pathfinding.

	 The game uses a Doom-style WAD engine whose map geometry is stored as
	 linedefs (line-segment walls), sidedefs (one per linedef side carrying
	 the sector reference), and sectors (convex-ish floor/ceiling areas).

	 A "portal" is a two-sided, non-IMPASSABLE linedef connecting two
	 sectors.  BuildSectorGraph() scans all linedefs after a WAD is loaded
	 and builds a bidirectional adjacency list of portal edges.

	 FindSectorPath() runs a standard A* search on this graph and returns
	 an ordered list of portal midpoint coordinates that an AI character
	 can follow to navigate from its sector to a target sector, routing
	 cleanly around walls, corners, and furniture.

   Lifecycle:
	 1. After load_new_wad():   call BuildSectorGraph()
	 2. Before PurgeLevel():    call FreeSectorGraph()
	 3. In AI_MOVING:           call FindSectorPath() and follow waypoints

   ======================================================================== */
#ifndef _AIPATH_HXX
#define _AIPATH_HXX

#if !defined(_TYPEDEFS_H)
#include "TYPEDEFS.H"
#endif

/* ========================================================================
   Constants
   ======================================================================== */

/**
 * @brief Maximum number of portal waypoints returned by FindSectorPath().
 *
 * Paths longer than this are truncated at the last stored waypoint;
 * the AI will fall back to direct movement for the remainder.
 * 24 portals is sufficient for all expected combat map layouts.
 */
#define AIPATH_MAX_WAYPOINTS 24

/**
 * @brief Distance threshold (world units) at which an AI character is
 *        considered to have reached a waypoint and may advance to the next.
 *
 * Matches the original WaypointReachDistance constant used in AIFOLPLY.CPP
 * before pathfinding was generalised to all character types.
 */
#define AIPATH_WAYPOINT_REACH_DISTANCE 80

/**
 * @brief Extra spacing (world units) added when a waypoint is routed around a
 *        blocking object so the mover clears the object's cylinder (Option U3).
 */
#define AIPATH_THING_CLEARANCE_MARGIN 16

/* ========================================================================
   AI_MOVER_INFO -- mover geometry for the geometry-aware refinements
   ======================================================================== */

/**
 * @brief Geometry of the character being routed, used by the optional
 *        geometry-aware refinements:
 *          - Option U3: avoid free-standing "things" (furniture/scenery).
 *          - Option U4: reject portals too narrow or too short for the mover
 *                       (large creatures, closed/locked doors, low passages).
 *
 * Pass NULL to FindSectorPath()/ApplySectorPath() to skip U3/U4 entirely and
 * use only the sector A* + funnel (Options U1/U2).
 *
 * @note Fliers set isFlying = TRUE to disable U3/U4: they pass over scenery
 *       (the bump system already lets them go over/under things) and are not
 *       constrained by floor/ceiling clearance the way ground movers are.
 */
struct AI_MOVER_INFO
{
	LONG width;     /**< Mover full width (world units). */
	LONG height;    /**< Mover height (world units); used for vertical clearance. */
	LONG z;         /**< Mover feet Z; used for thing vertical-overlap tests. */
	BOOL isFlying;  /**< TRUE: skip thing-avoidance and clearance/door gating. */
};

/* ========================================================================
   AI_PATH_STATE -- per-character pathfinding state
   ======================================================================== */

/**
 * @brief Sector-graph A* pathfinding state for one AI character.
 *
 * Stored as a non-union member (AI_PATH_STATE fAiPath) on CAvatar so that
 * every AI function -- follow-player, ping-pong, hellhound, spider, spectre,
 * and any future types -- can use the sector-graph path without sharing
 * storage with their own AI-specific union data.
 *
 * Lifecycle:
 *   Initialise to { 0, 0, -1 } at AI_INIT time.  ApplySectorPath() updates
 *   the state each frame during AI_MOVING.  Reset fPathWaypointCount = 0
 *   and fPathTargetSector = -1 whenever the AI gives up steering so that the
 *   next pursuit attempt triggers a fresh A* query.
 */
struct AI_PATH_STATE
{
	LONG fPathWaypointsX[AIPATH_MAX_WAYPOINTS]; /**< X world-coords of portal midpoints, travel order. */
	LONG fPathWaypointsY[AIPATH_MAX_WAYPOINTS]; /**< Y world-coords of portal midpoints, travel order. */
	int  fPathWaypointCount;  /**< Waypoint count: -1=no path found, 0=not computed, >0=count. */
	int  fPathWaypointIndex;  /**< Index of the next waypoint to navigate toward. */
	LONG fPathTargetSector;   /**< Enemy sector when path was computed; -1 = stale/unset. */
};

/* ========================================================================
   Public interface
   ======================================================================== */

/**
 * @brief Build the sector connectivity graph from the currently loaded WAD.
 *
 * Must be called after load_new_wad() completes and before any call to
 * FindSectorPath().  Scans all linedefs for two-sided, non-IMPASSABLE
 * portals and constructs per-sector adjacency lists.  Also computes a
 * representative centre coordinate for every sector (used as the A*
 * heuristic origin).
 *
 * Calling BuildSectorGraph() when a graph already exists silently frees
 * the previous graph first.
 *
 * @note This function must not be called while the AI main loop is running
 *       (i.e., only call it during scene load, not mid-frame).
 */
void BuildSectorGraph();

/**
 * @brief Free all memory associated with the current sector graph.
 *
 * Must be called before PurgeLevel() / load_new_wad() so that stale
 * sector and linedef pointers are not accessed after the WAD is unloaded.
 * Safe to call when no graph has been built (no-op in that case).
 */
void FreeSectorGraph();

/**
 * @brief Find a path from one sector to another via A* on the sector graph.
 *
 * Performs an A* search where nodes are sectors and edges are passable
 * portal linedefs.  Edge costs and the heuristic are measured from the
 * actual point at which each sector was entered (a portal midpoint, or the
 * avatar's real start position for the origin sector) to the next portal
 * midpoint and to the target's real position respectively (Option U2), so
 * the chosen sector sequence reflects where the characters actually stand
 * rather than abstract sector centroids.  Hazardous sectors (lava, acid,
 * deep water) are excluded from the search.
 *
 * Once the sector sequence is found, the portal openings it crosses are
 * smoothed with the funnel ("string-pulling") algorithm anchored at the
 * real start and target positions (Option U1).  The returned waypoints are
 * the funnel corner points -- they hug wall corners instead of stepping
 * through portal centres, and a clear straight line yields zero waypoints.
 *
 * @param fromSector    Index of the starting sector (avatar's current sector).
 * @param toSector      Index of the destination sector (enemy's current sector).
 * @param startX        Avatar's real X world-coordinate (funnel/cost start anchor).
 * @param startY        Avatar's real Y world-coordinate.
 * @param targetX       Target's real X world-coordinate (funnel/cost goal anchor).
 * @param targetY       Target's real Y world-coordinate.
 * @param waypointsX    Output array; receives X world-coordinates of each
 *                      funnel corner along the path, in travel order.
 * @param waypointsY    Output array; receives Y world-coordinates.
 * @param maxWaypoints  Capacity of the output arrays; use AIPATH_MAX_WAYPOINTS.
 * @param pMover        Optional mover geometry.  When non-NULL and not flying,
 *                      portals too narrow or too short for the mover (large
 *                      creatures, closed/locked doors, low passages) are
 *                      excluded from the search (Option U4), and the funnel
 *                      path is nudged around free-standing things (Option U3).
 *                      Pass NULL for sector A* + funnel only (U1/U2).
 *
 * @return  Number of waypoints written (>= 0) on success.
 *          0  means no intermediate corner is needed -- either fromSector ==
 *             toSector, or the funnel found a clear straight line to the
 *             target -- so the caller should move directly toward the target.
 *          -1 means no path exists (impassable geometry or graph not built).
 *
 * @note    The caller should navigate to each waypoint in sequence, advancing
 *          to the next waypoint when within AIPATH_WAYPOINT_REACH_DISTANCE of the
 *          current one.  After all waypoints are consumed, switch to direct movement
 *          toward the enemy.  Prefer calling ApplySectorPath() rather than this
 *          function directly; it wraps the full per-frame update logic.
 */
int FindSectorPath(LONG fromSector, LONG toSector,
				   LONG startX, LONG startY,
				   LONG targetX, LONG targetY,
				   LONG *waypointsX, LONG *waypointsY, int maxWaypoints,
				   const AI_MOVER_INFO *pMover = NULL);

/**
 * @brief Apply sector-graph A* navigation for one AI character on a single frame.
 *
 * Encapsulates the complete per-frame path update used in every AI_MOVING
 * case that needs obstacle-aware navigation:
 *
 *   1. If fromSector == toSector: clear cached path, leave target unchanged.
 *   2. If toSector changed since last call: rerun FindSectorPath() and reset
 *      the waypoint index.
 *   3. If a valid path exists: override the pMoveTargetX and pMoveTargetY outputs with the
 *      next waypoint's coordinates.  Advance the waypoint index automatically
 *      when the character comes within AIPATH_WAYPOINT_REACH_DISTANCE.
 *   4. After all waypoints are consumed or if no path was found: the output
 *      target remains the direct (targetX, targetY) coordinates.
 *
 * Call site pattern:
 * @code
 *   AI_MOVER_INFO mover;
 *   mover.width = pAvatar->mfWidth();  mover.height = pAvatar->mfHeight();
 *   mover.z = pAvatar->mfZ();          mover.isFlying = FALSE;
 *   LONG moveTargetX = pAvatarTarget->mfX();
 *   LONG moveTargetY = pAvatarTarget->mfY();
 *   ApplySectorPath(&pAvatar->fAiPath,
 *                   pAvatar->mfGetSector(), pAvatarTarget->mfGetSector(),
 *                   pAvatar->mfX(), pAvatar->mfY(),
 *                   pAvatarTarget->mfX(), pAvatarTarget->mfY(),
 *                   &moveTargetX, &moveTargetY, &mover);
 *   pAvatar->mfMoveTowardWithBump(moveDist, moveTargetX, moveTargetY, ...);
 * @endcode
 *
 * @param pPath         Pointer to the character's AI_PATH_STATE (pAvatar->fAiPath).
 * @param fromSector    Character's current sector index.
 * @param toSector      Target's current sector index.
 * @param avatarX       Character's current X world-coordinate.
 * @param avatarY       Character's current Y world-coordinate.
 * @param targetX       Target's current X world-coordinate.
 * @param targetY       Target's current Y world-coordinate.
 * @param pMoveTargetX  In/out: set to targetX on entry; overridden with waypoint X if applicable.
 * @param pMoveTargetY  In/out: set to targetY on entry; overridden with waypoint Y if applicable.
 * @param pMover        Optional mover geometry for U3/U4 (see FindSectorPath); NULL skips them.
 */
void ApplySectorPath(AI_PATH_STATE *pPath,
					 LONG fromSector, LONG toSector,
					 LONG avatarX, LONG avatarY,
					 LONG targetX, LONG targetY,
					 LONG *pMoveTargetX, LONG *pMoveTargetY,
					 const AI_MOVER_INFO *pMover = NULL);

#endif /* _AIPATH_HXX */
