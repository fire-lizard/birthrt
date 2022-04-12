/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _ADVENTUR_HXX
#define _ADVENTUR_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_ENGINE_H)
#include "engine.h"
#endif

#if !defined(_SYSTEM_H)
#include "system.h"
#endif

#if !defined(_PLAYER_HXX)
#include "player.hxx"
#endif

#if !defined(_THINGTYP_H)
#include "thingtyp.h"
#endif

#if !defined(_HANDLE_HXX)
#include "handle.hxx"
#endif

#if !defined(_VECTOR_HXX)
#include "vector.hxx"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define MAX_ADVENTURERS		4
#define MAX_TRAIL_POINTS	50
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
   Class Definition
   ------------------------------------------------------------------------ */
class ADVENTURER_TEAM_ITOR;
class CAvatar;

typedef BOOL const (CAvatar::*CAvatar_pmfv)() const;
typedef void (CAvatar::*CAvatar_pmf)();

// Class to hold positional information about the adventure team. Also can
// be called to test whether conditions about the team.

class ADVENTURER {
public:
	friend class ADVENTURER_TEAM_ITOR;
	
	// Helper class to hold the positional data necessary to move the team
	// members through the world.
	class ADVENTURER_POSITION {
	public:
		friend class ADVENTURER;
		
		void		mfUpdatePosition(PLAYER const * const pPlayer);
		void		mfFillReversePlayerInfo ( PLAYER * const pPlayer, LONG const ThingIndex) const;
		BOOL const	mfHasMoved(PLAYER const * const pPlayer) const;
	    BOOL const	mfAmICrouching() const;
	    BOOL const	mfCanIMoveHere(ADVENTURER_POSITION const * const pPosition,
								  LONG const AvatarWidth) const;
	
	protected:
	private:
		PLAYER			fPlayer;
	};
	
	inline void mfInitData();
	inline void mfRelease();
	
	BOOL const mfJoinAdventureTeam(SHORT const hdlAvatar);
	BOOL const mfDidIMove(LONG const /* X */,
	                      LONG const /* Y */,
	                      LONG const /* Z */,
	                      LONG const /* A */) const;
	
	BOOL const mfAmICrouching() const;
	void mfSwapPlaces( ADVENTURER & /* rAdventurer */);
	
	// [d4-25-97 JPC] Added new parameter pStartFallZ.
	inline WadThingType const mfBumpAndFloor(LONG * const /* pThingIndex */,
	                            			 LONG * const /* pFloor */,
										 	 LONG * const /* pFallHeight */,
											 LONG * const /* pStartFallZ */) const;
	void mfGetMyPosition(LONG * const /* pX */,
	                     LONG * const /* pY */,
	                     LONG * const /* pZ */,
	                     LONG * const /* pA */) const;
	
	BOOL const			mfCanIWalkThruWall() const;
	// Call every AI pass to keep the group moving.
	static void			mfUpdateAdventureTeam(PLAYER const * const /* pPlayer */);
	static LONG const	mfNumberOfAdventurers();
	static void			mfInitAdventureTeam(PLAYER const * const /* pPlayer */);
	static void			mfReleaseAdventureTeam();
	static BOOL const	mfIsAdventureTeam(CAvatar_pmfv);
	static SHORT const	mfFirstAdventurerIs(CAvatar_pmfv);
	static void			mfDoToAllAdventurers(CAvatar_pmf);
	static LONG const	mfNumberOfTrailPts();
	static void			mfFillReversePlayerPts(PLAYER * const /* pPlayerPts */,
										LONG const /* NumberOfPts */,
										LONG const /* ThingIndex */ );
	static BOOL const	mfDoesAdventureTeamHave(THINGTYPE const);
	inline static SHORT const	mfWhoGetsObjectsPickedUp();
	static void			mfSetWhoGetsObjectsPickedUp( SHORT const /* hAvatar */);
	
	// First member of the team.  Return only valid handles.
	static ADVENTURER_TEAM_ITOR const begin();
	
	// Past the end of the Team array.
	static ADVENTURER_TEAM_ITOR const end();
	
protected:
private:
	SBYTE			fAdventurerPathIndex;		// Which path follower am I.
	SHORT			fhAvatar;					// Back handle to my avatar data.
	
	static BOOL		fPlayerMoved;
	static SHORT	fhdlPathPositions;
	static DECL_VECTOR_CLASS_S(SHORT,fAdventurersHdl,MAX_ADVENTURERS);
	static SBYTE	fAdventurerCount;
	
	static LONG		fTimeLastBump;
	static LONG		fTimeLastSplash;		// [d11-14-96 JPC]
	static BOOL		fLavaSoundOn;			// [d11-14-96 JPC]
	static SHORT	fLavaSoundID;			// [d11-14-96 JPC]
	static SBYTE const fFirstAdventurerIndex;
	static SBYTE	fNumberOfGoodTrailPts;
	static SHORT	fhWhoGetsObjects;		// When walking around inventory is added to...
};

// Iterator class for the Adventurer team.
// This class is modeled to follow the STL convention for member fns and
// operators.
class ADVENTURER_TEAM_ITOR {
public:
	// Constructor for the adventure team iterator
	ADVENTURER_TEAM_ITOR(SBYTE const val = 0) :
		fCurrentIndex(val) {}
	
	ADVENTURER_TEAM_ITOR(ADVENTURER_TEAM_ITOR const &rhs) :
		fCurrentIndex(rhs.fCurrentIndex) {}
	
	// Advance the iterator to the next Adventurer on the team.
	ADVENTURER_TEAM_ITOR const &operator++(int) {
		for (++fCurrentIndex; ADVENTURER::end() != fCurrentIndex; fCurrentIndex++)
		{
			if (ADVENTURER::fAdventurersHdl[fCurrentIndex] != fERROR)
			{
				break;
			}
		}
		return *this;
	}
	
	// Let the iterator become positioned at this index.
	ADVENTURER_TEAM_ITOR const & operator=(ADVENTURER_TEAM_ITOR const &rhs) {
		fCurrentIndex = rhs.fCurrentIndex;
		return *this;
	}
	
	void mfSet(SHORT const hAvatar)
	{
		ADVENTURER::fAdventurersHdl[fCurrentIndex] = hAvatar;
	}
	
	// Return the handle to the avatar.
	// Note: It doesn't check for out of range values.
	SHORT & operator*() const {
		return ADVENTURER::fAdventurersHdl[fCurrentIndex];
	}
	
	// Comparison operator is a member to avoid an extra copy of the iterator.
	BOOL const operator!=(ADVENTURER_TEAM_ITOR const &rhs) const
	{
		return (fCurrentIndex != rhs.fCurrentIndex) ? TRUE : FALSE;
	}
	
protected:
	// Comparison operator is a member to avoid an extra copy of the iterator.
	BOOL const operator!=(SBYTE const &rhs) const
	{
		return (fCurrentIndex != rhs) ? TRUE : FALSE;
	}
	
private:
	SBYTE				fCurrentIndex;
};
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Inline functions
   ------------------------------------------------------------------------ */
inline void ADVENTURER::ADVENTURER_POSITION::mfUpdatePosition(
	PLAYER const * const pPlayer)
{
	fPlayer.x = PLAYER_INT_VAL(pPlayer->x);
	fPlayer.y = PLAYER_INT_VAL(pPlayer->y);
	fPlayer.z = pPlayer->z;
	fPlayer.a = (SHORT) pPlayer->a;
	fPlayer.h = pPlayer->h;
	fPlayer.Crouching = (SBYTE) pPlayer->Crouching;
	fPlayer.floor = (SHORT) pPlayer->floor;
	fPlayer.ceiling = (SHORT) pPlayer->ceiling;
	fPlayer.bump = pPlayer->bump;
	fPlayer.BumpIndex = (SHORT)pPlayer->BumpIndex;
	fPlayer.WalkThruWall = pPlayer->WalkThruWall;
	fPlayer.currentSector = pPlayer->currentSector;
	fPlayer.fallHeight = pPlayer->fallHeight;
	fPlayer.startFallZ = pPlayer->startFallZ; // [d4-25-97 JPC]
}

inline void ADVENTURER::ADVENTURER_POSITION::mfFillReversePlayerInfo (
	PLAYER * const pPlayer, LONG const ThingIndex) const
{
	pPlayer->x = fPlayer.x << PLAYER_FIXEDPT;
	pPlayer->y = fPlayer.y << PLAYER_FIXEDPT;
	pPlayer->z = fPlayer.z;
	pPlayer->a = (fPlayer.a - 128 ) & 0xFF;
	pPlayer->Crouching = fPlayer.Crouching;
	pPlayer->floor = fPlayer.floor;
	pPlayer->ceiling = fPlayer.ceiling;
	pPlayer->currentSector = fPlayer.currentSector;
	// We won't bump objects while retreating. (We'll still go around.)
	if (fPlayer.bump == iOBJECT)
	{
		pPlayer->bump = iNOTHING;
	}
	else
	{
		pPlayer->bump = fPlayer.bump;
	}
	pPlayer->BumpIndex = fPlayer.BumpIndex;
	pPlayer->WalkThruWall = fPlayer.WalkThruWall;
	
	// Just keep using the default height and width.
	pPlayer->w = player.w;

	// [d11-20-96 JPC]
	// If crouching, height is halved, so we need to know the height.
	// OLD: pPlayer->h = player.h;
	pPlayer->h = fPlayer.h;
	pPlayer->Flying = FALSE;
	pPlayer->ThingIndex = ThingIndex;
	pPlayer->p = 0;	// No pitch.
	pPlayer->fallHeight = fPlayer.fallHeight;
	pPlayer->startFallZ = fPlayer.startFallZ; // [d4-25-97 JPC]
}

inline BOOL const ADVENTURER::ADVENTURER_POSITION::mfHasMoved(
	PLAYER const * const pPlayer) const
{
	return ((fPlayer.x != PLAYER_INT_VAL(pPlayer->x) ||
	         fPlayer.y != PLAYER_INT_VAL(pPlayer->y) ||
	         fPlayer.z != pPlayer->z ||
	         fPlayer.a != pPlayer->a )
	       ? TRUE : FALSE );
}

inline BOOL const ADVENTURER::ADVENTURER_POSITION::mfAmICrouching() const
{
	return ((fPlayer.Crouching == TRUE) ? TRUE : FALSE);
}

inline BOOL const ADVENTURER::ADVENTURER_POSITION::mfCanIMoveHere(
	ADVENTURER_POSITION const * const pPosition,
	LONG const AvatarWidth) const
{
	// [d11-24-96 JPC] Need to take Z into account for falling damage.
	// The team may stack up--the lesser evil.
	if (fPlayer.z != pPosition->fPlayer.z)
		return TRUE;
	return ((AvatarWidth <= aprox_dist(fPlayer.x, fPlayer.y,
	                                   pPosition->fPlayer.x,
	                                   pPosition->fPlayer.y)) ? TRUE : FALSE );
}

inline void ADVENTURER::mfInitData()
{
	fAdventurerPathIndex = -1;
	fhAvatar = -1;
}

inline void ADVENTURER::mfRelease()
{
	LONG i;
	
	for (i = 0; i < MAX_ADVENTURERS; i++)
	{
		if (fhAvatar == fAdventurersHdl[i])
		{
			fAdventurersHdl[i] = fERROR;
			break;
		}
	}
	fAdventurerPathIndex = -1;
	fAdventurerCount--;
	if (fAdventurerCount < 0)
	{
		fAdventurerCount = 0;
	}
	mfInitData();
}

// return the total in the team.
inline LONG const ADVENTURER::mfNumberOfAdventurers()
{
	return fAdventurerCount;
}

// Check whether to use the crouch art or not.
inline BOOL const ADVENTURER::mfAmICrouching() const
{
	BOOL Result = FALSE;
	if (fhdlPathPositions != fERROR && fAdventurerPathIndex != -1)
	{
		DumbHandleArray<ADVENTURER_POSITION const> const pPosition(fhdlPathPositions);
		Result = pPosition[fAdventurerPathIndex].mfAmICrouching();
	}
	
	return Result;
}

// check whether the adventurer can walk through wall or not.
inline BOOL const ADVENTURER::mfCanIWalkThruWall() const
{
	BOOL Result = FALSE;
	if (fhdlPathPositions != fERROR && fAdventurerPathIndex != -1)
	{
		DumbHandleArray<ADVENTURER_POSITION const> const pPosition(fhdlPathPositions);
		Result = (BOOL)(pPosition[fAdventurerPathIndex].fPlayer.WalkThruWall);
	}
	return Result;
}
	
// return the thing we bumped.
inline WadThingType const ADVENTURER::mfBumpAndFloor(LONG * const pThingIndex,
                                                     LONG * const pFloor,
													 LONG * const pFallHeight,
													 LONG * const pStartFallZ) const // [d4-25-97 JPC] new param
{
	WadThingType bump = iNOTHING;
	
	if (fhdlPathPositions != fERROR && fAdventurerPathIndex != -1)
	{
		DumbHandleArray<ADVENTURER_POSITION const> const pPosition(fhdlPathPositions);
		bump = pPosition[fAdventurerPathIndex].fPlayer.bump;
		*pThingIndex = pPosition[fAdventurerPathIndex].fPlayer.BumpIndex;
		*pFloor = pPosition[fAdventurerPathIndex].fPlayer.floor;
		*pFallHeight = pPosition[fAdventurerPathIndex].fPlayer.fallHeight;
		*pStartFallZ = pPosition[fAdventurerPathIndex].fPlayer.startFallZ; // [d4-25-97 JPC]
	}
	return bump;
}

// Get the position data for this bread crumb follower.
inline void ADVENTURER::mfGetMyPosition(
	LONG * const pX,
	LONG * const pY,
	LONG * const pZ,
	LONG * const pA) const
{
	if (fhdlPathPositions != fERROR && fAdventurerPathIndex != -1)
	{
		DumbHandleArray<ADVENTURER_POSITION const> const pPosition(fhdlPathPositions);
		*pX = pPosition[fAdventurerPathIndex].fPlayer.x;
		*pY = pPosition[fAdventurerPathIndex].fPlayer.y;
		*pZ = pPosition[fAdventurerPathIndex].fPlayer.z;
		
		// Only the first person in line follows the player the rest, the direction
		// of travel.
		if (fAdventurerPathIndex == fFirstAdventurerIndex)
		{
			*pA = pPosition[fAdventurerPathIndex].fPlayer.a;
		}
		else
		{
			// Face the followers the way they are traveling.
			*pA = AngleFromPoint(pPosition[fAdventurerPathIndex].fPlayer.x,
			                     pPosition[fAdventurerPathIndex].fPlayer.y,
			                     pPosition[fAdventurerPathIndex + 1].fPlayer.x,
			                     pPosition[fAdventurerPathIndex + 1].fPlayer.y,
			                     RESOLUTION_8);
		}
	}
}

// Test this current position to the next position.
inline BOOL const ADVENTURER::mfDidIMove(
  LONG const X,
  LONG const Y,
  LONG const Z,
  LONG const Angle) const
{
	BOOL Result = FALSE;
	if (fhdlPathPositions != fERROR && fAdventurerPathIndex != -1)
	{
		DumbHandleArray<ADVENTURER_POSITION const> const pPosition(fhdlPathPositions);
		if (X != pPosition[fAdventurerPathIndex].fPlayer.x ||
		    Y != pPosition[fAdventurerPathIndex].fPlayer.y ||
		    Z != pPosition[fAdventurerPathIndex].fPlayer.z ||
		    (fAdventurerPathIndex == fFirstAdventurerIndex &&
		     Angle != pPosition[fAdventurerPathIndex].fPlayer.a))
		 {
		 	Result = TRUE;
		 }
	}
	
	return Result;
}

// Get the number of trail points.
inline LONG const ADVENTURER::mfNumberOfTrailPts()
{	
	return fNumberOfGoodTrailPts;
}

// First member of the team.  Return only valid handles.
inline ADVENTURER_TEAM_ITOR const ADVENTURER::begin()
{
	SBYTE Result = MAX_ADVENTURERS;
	for (SBYTE i = 0; i < MAX_ADVENTURERS; i++)
	{
		if (fAdventurersHdl[i] != fERROR)
		{
			Result = i;
			break;
		}
	}
	return ADVENTURER_TEAM_ITOR(Result);
}

// Past the end of the Team array.
inline ADVENTURER_TEAM_ITOR const ADVENTURER::end()
{
	return ADVENTURER_TEAM_ITOR(MAX_ADVENTURERS);
}

// Return who Gets the objects.
inline SHORT const	ADVENTURER::mfWhoGetsObjectsPickedUp()
{
	return fhWhoGetsObjects;
}

// Change who gets the objects. (Does no error checking...maybe later..)
inline void ADVENTURER::mfSetWhoGetsObjectsPickedUp( SHORT const hAvatar)
{
	fhWhoGetsObjects = hAvatar;
}
#endif // _ADVENTUR_HXX
