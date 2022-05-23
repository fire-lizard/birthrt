/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aifolply.hxx
   Author:   Gary Powell
   ======================================================================== */
#ifndef _AIFOLPLY_H
#define _AIFOLPLY_H

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_ADVENTUR_HXX)
#include "adventur.hxx"
#endif

/* -----------------------------------------------------------------
   class definition
   ----------------------------------------------------------------- */
class FOLLOW_PLAYER_DATA
{
public:
	typedef enum {
		THE_PLAYER,
		THE_ADVENTURE_TEAM
	} FOLLOWING_STATE;
	
	// Write to Scene File
	void mfWriteData(FILE *fp) const;
	
	// Read from Scene File
	LONG mfReadData(FILE *fp);
	
	void mfInitVals(PLAYER *pPlayer, SHORT const /* hAvatar */);
	void mfJoinAdventureTeam();
	void mfDelete();
	
	static void mfSwitchToFollowPlayer(LONG /* hAvatar */ );
	
	inline BOOL const mfCanIWalkThruWall() const;
	inline BOOL const mfAmICrouching() const;
	inline BOOL const mfDidIMove(LONG const /* x */,
                                 LONG const /* y */,
                                 LONG const /* z */,
                                 LONG const /* a */) const;
	inline void mfGetMyPosition(LONG * const /* px */,
					            LONG * const /* py */,
					            LONG * const /* pz */,
					            LONG * const /* pa */) const;
	
	// [d4-25-97 JPC] Added pStartFallZ param.
	inline WadThingType const mfBumpAndFloor(LONG * const /* pThingIndex */,
	                            LONG * const /* pFloor */,
										 LONG * const /* pFallHeight */,
										 LONG * const /* pStartFallZ */) const;
	void mfFollowPlayer();
	inline void mfFollowAdventureTeam();
	inline void mfSwapPlaces(FOLLOW_PLAYER_DATA &);
	
	SBYTE			fCurrentSpellBox;
	
	LONG	fTimeLastBump;
	
private:
	ADVENTURER		fAdventurer;
	PLAYER			*fpPlayer;
	FOLLOWING_STATE	fFollowState;
	SHORT			fhAvatar;
};
	
/* -----------------------------------------------------------------
   inline member functions
   ----------------------------------------------------------------- */
// Cleanup the memory
inline void FOLLOW_PLAYER_DATA::mfDelete()
{
	fAdventurer.mfRelease();
}

// write member data to scene file
inline void FOLLOW_PLAYER_DATA::mfWriteData(FILE *) const
{
	//fprintf(fp, "%ld %ld\n", DistX, DistY);
}

// read member data from scene file
inline LONG FOLLOW_PLAYER_DATA::mfReadData(FILE *)
{
	//char cpBuffer[80];
	//LONG Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	//
	//if (Result != EOF)
	//{
	//	Result = ( (2 == sscanf(cpBuffer, "%ld %ld", &DistX, &DistY) ) ? Result : EOF);
	//}
	//return Result;
	return 1;
}

// Attach this ai to this PLAYER structure.
inline void FOLLOW_PLAYER_DATA::mfInitVals(PLAYER * pPlayer, SHORT const hAvatar)
{
	fpPlayer = pPlayer;
	fAdventurer.mfInitData();
	fFollowState = THE_PLAYER;
	fhAvatar = hAvatar;
}

// Connect up to an adventure team
inline void FOLLOW_PLAYER_DATA::mfJoinAdventureTeam()
{
	fAdventurer.mfJoinAdventureTeam(fhAvatar);
	fFollowState = THE_ADVENTURE_TEAM;
}


inline void FOLLOW_PLAYER_DATA::mfFollowAdventureTeam()
{
	fFollowState = THE_ADVENTURE_TEAM;
}

inline BOOL const FOLLOW_PLAYER_DATA::mfCanIWalkThruWall() const
{
	return (fFollowState == THE_ADVENTURE_TEAM) ?
	        fAdventurer.mfCanIWalkThruWall() : fpPlayer->WalkThruWall;
}

inline BOOL const FOLLOW_PLAYER_DATA::mfAmICrouching() const
{
	return (fFollowState == THE_ADVENTURE_TEAM) ?
	        fAdventurer.mfAmICrouching() : fpPlayer->Crouching;
}

inline void FOLLOW_PLAYER_DATA::mfGetMyPosition(LONG * const px,
					                            LONG * const py,
					                            LONG * const pz,
					                            LONG * const pa) const
{
	if (fFollowState == THE_ADVENTURE_TEAM)
	{
		fAdventurer.mfGetMyPosition(px, py, pz, pa);
	}
	else
	{
		*px = PLAYER_INT_VAL(fpPlayer->x);
		*py = PLAYER_INT_VAL(fpPlayer->y);
		*pz = fpPlayer->z;
		*pa = fpPlayer->a;
	}
}

inline WadThingType const FOLLOW_PLAYER_DATA::mfBumpAndFloor(
	LONG * const  pThingIndex ,
    LONG * const  pFloor ,
	LONG * const  pFallHeight,
	LONG * const  pStartFallZ) const
{
	WadThingType Result;
	
	if (fFollowState == THE_ADVENTURE_TEAM)
	{
		Result = fAdventurer.mfBumpAndFloor(pThingIndex, pFloor, pFallHeight, pStartFallZ);
	}
	else
	{
		*pThingIndex = fpPlayer->BumpIndex;
		*pFloor = fpPlayer->floor;
		*pFallHeight = fpPlayer->fallHeight;
		*pStartFallZ = fpPlayer->startFallZ;
		Result = fpPlayer->bump;
	}
	
	return Result;
}

inline BOOL const FOLLOW_PLAYER_DATA::mfDidIMove(LONG const x,
                                                 LONG const y,
                                                 LONG const z,
                                                 LONG const a) const
{
	BOOL Result;
	
	if (fFollowState == THE_ADVENTURE_TEAM)
	{
		Result = fAdventurer.mfDidIMove(x, y, z, a);
	}
	else
	{
		Result = (x != PLAYER_INT_VAL(fpPlayer->x) ||
				  y != PLAYER_INT_VAL(fpPlayer->y) ||
				  z != fpPlayer->z ||
				  a != fpPlayer->a);
	}
	
	return Result;
}
inline void FOLLOW_PLAYER_DATA::mfSwapPlaces(FOLLOW_PLAYER_DATA & OtherFollower)
{
	fAdventurer.mfSwapPlaces(OtherFollower.fAdventurer);
}
#endif	// _AIFOLPLY_HXX
