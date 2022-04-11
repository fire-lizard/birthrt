/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: AThing.hxx
   Author:   Craig Clayton & Gary Powell
   ======================================================================== */
#if !defined(_ATHING_HXX)
#define _ATHING_HXX

#include <stdio.h>

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_SYSTEM_H)
#include "system.h"
#endif

#if !defined(_LIGHT_H)
#include "light.h"
#endif

#if !defined(_GAMETYPE_HXX)
#include "gametype.hxx"
#endif

#if !defined(_SPECIAL_H)
#include "special.h"
#endif

#if !defined(_THINGS_H)
#include "things.h"
#endif


// Base class for all avatar things that exist in Birthright
// Its the interface to the Engine specific data so that should the engine
// change there is only one place that needs fixing for the code to still work.

class ArtThing
{
public:
	// WARNING! Don't modify this value willy nilly. Its the index into
	//          the mythings array for engine specific information.
	LONG    	ThingIndex;         // index of myTHING
	THINGTYPE	ThingType;          // type of THING I am
	
	void mfDefineArtThing(THINGTYPE const, 
						LONG const /* x */, 
						LONG const /* y */, 
						LONG const z=0, 
						LONG const a=0);
	
	void mfTestAndUseLowResArt();
	
	void mfAttachArtThing(LONG const /* ThingIndex */,
	                      THINGTYPE const);
	                      
	LONG const mfX() const;
	LONG const mfY() const;
	LONG const mfZ() const;
	LONG const mfAngle() const;
	LONG const mfHeight() const;//this gets what the height was last time the thing was drawn
	LONG const mfWidth() const; 
	LONG const mfDeathHeight();  //this gets the height of their death frame
	void mfSetToDeathHeightWidth();
	
	THINGTYPE const mfType() const;
	BOOL const mfIsVisible() const;
	void mfGetPoint (POINT &pt) const;
	void mfSetAngle(LONG const NewAngle);
	void mfSetType(THINGTYPE const type);
	void mfSetFrozen(BOOL const flag);
	void mfSetQuickAnimation(BOOL const flag);
	BOOL const mfIsQuickAnimation() const;
	void mfMoveTo(LONG const /* X */, 
	              LONG const /* Y */);
	void mfMoveToXYA(LONG const /* X */, 
	                  LONG const /* Y */, 
	                  LONG const /* A */);
	void mfMoveToXYZA(LONG const /* X */, 
	                  LONG const /* Y */, 
	                  LONG const /* Z */, 
	                  LONG const /* A */);
	void mfGetSectorInfo(LONG * const /* floor height */,
	                     LONG * const /* ceiling height */,
	                     LONG * const /* special */) const;
	LONG const mfGetSector() const;
	void mfInitVals();
    LONG const FaceTo (LONG const XP, LONG const YP);
	UBYTE const mfChangeScale(LONG const newScale);
	char const * const mfGetDescription() const;
	
	void mfRemapColor(LONG const /* Remap index */);
	LONG const mfGetRemapColor() const;
	
	BOOL const mfIsInvisible() const;
	void mfSetInvisible(BOOL const);
	
	BOOL const mfCanBePickedUp() const;
	LONG const mfGetArtSequence() const;
	
	BOOL const mfWading() const;
	BOOL const mfInDeepWater() const;

protected:
private:
	// Defined but not implemented on purpose to generate link errors.
	ArtThing();
	~ArtThing();
	ArtThing operator=(const ArtThing &);
	ArtThing(const ArtThing &);
    void *operator new( size_t stAllocateBlock);
    void operator delete( void *p );
    
    THINGTYPE	LowResThingType;
    THINGTYPE	UltraLowResThingType;
	
};


/* ========================================================================
   Function    - mfAttachArtThing
   Description - inline to attach to an existing thing in the mythings array.
   Returns     - 
   ======================================================================== */
inline void ArtThing::mfAttachArtThing(LONG const lThingIndex,
                                       THINGTYPE const tt)
{
	ThingIndex = lThingIndex;
	ThingType = tt;
	LowResThingType = GAME_TTYPE::mfGetLowResType(tt);
	if (LowResThingType != NOLOW)
	{
		UltraLowResThingType = (THINGTYPE) GetMinMemConvert(LowResThingType);
	}
	else
	{
		UltraLowResThingType = NOLOW;
	}
}

/* ========================================================================
   Function    - mfX
   Description - inline to return the x out of mythings array
   Returns     - x location
   ======================================================================== */
inline LONG const ArtThing::mfX() const
{
	return mythings[ThingIndex].x;
}

/* ========================================================================
   Function    - mfY
   Description - inline to return the y out of the mythings array
   Returns     - 
   ======================================================================== */
inline const LONG ArtThing::mfY() const
{
	return mythings[ThingIndex].y;
}

/* ========================================================================
   Function    - mfZ
   Description - inline to return the z out of the mythings array
   Returns     - 
   ======================================================================== */
inline LONG const ArtThing::mfZ() const
{
	return mythings[ThingIndex].z;
}

/* ========================================================================
   Function    - mfAngle
   Description - inline to return the angle out of mythings array
   Returns     - 
   ======================================================================== */
inline LONG const ArtThing::mfAngle() const
{
	return mythings[ThingIndex].angle;
}

/* ========================================================================
   Function    - mfHeight
   Description - inline to return the height out of mythings array
   Returns     - 
   ======================================================================== */
inline LONG const ArtThing::mfHeight() const
{
	return mythings[ThingIndex].heiScaled;
}



/* ========================================================================
   Function    - mfDeathHeight
   Description - inline to return the height of the death frame
   Returns     - 
   ======================================================================== */
inline LONG const ArtThing::mfDeathHeight()
{
	mfSetToDeathHeightWidth();
	return mythings[ThingIndex].heiScaled;
}

/* ========================================================================
   Function    - mfWidth
   Description - inline to return the Width out of mythings array
   				 Width is the distance from the center of the art to the
   				 edge.
   Returns     - 
   ======================================================================== */
inline LONG const ArtThing::mfWidth() const
{
	return (mythings[ThingIndex].widScaled / 2);
}

/* ========================================================================
   Function    - mfType
   Description - inline to return the thing type
   Returns     - 
   ======================================================================== */
inline THINGTYPE const ArtThing::mfType() const
{
	return ThingType;
}

/* ========================================================================
   Function    - mfIsVisible
   Description - inline to return whether the thing can see the camera.
   Returns     - TRUE | FALSE
   ======================================================================== */
inline BOOL const ArtThing::mfIsVisible() const
{
	return ((mythings[ThingIndex].fDrawn) ? TRUE : FALSE);
}

/* ========================================================================
   Function    - mfGetPoint
   Description - inline to fill a POINT structure with my location
   Returns     - 
   ======================================================================== */
inline void ArtThing::mfGetPoint (POINT &pt) const
{
	pt.x = mythings[ThingIndex].x;
	pt.y = mythings[ThingIndex].y;
}

/* ========================================================================
   Function    - mfSetAngle
   Description - inline to set my new angle
   Returns     - 
   ======================================================================== */
inline void ArtThing::mfSetAngle(LONG const NewAngle)
{
	mythings[ThingIndex].angle = (UBYTE)(NewAngle);
}
	
/* ========================================================================
   Function    - mfSetType
   Description - inline to record the type of this thing
   Returns     - 
   ======================================================================== */
inline void ArtThing::mfSetType(THINGTYPE const type)
{
	if ( type != ThingType)
	{
		ThingType = type;
		ChangeThingType(ThingIndex, type);
		
		LowResThingType = GAME_TTYPE::mfGetLowResType(type);
		if (LowResThingType != NOLOW)
		{
			UltraLowResThingType = (THINGTYPE) GetMinMemConvert(LowResThingType);
		}
		else
		{
			UltraLowResThingType = NOLOW;
		}
	}
}

/* ========================================================================
	Function   - mfSetFrozen
	Description- inline to set the frozen flag
	Returns
   ======================================================================== */
inline void ArtThing::mfSetFrozen(BOOL const flag)
{   
   	mythings[ThingIndex].Frozen = flag;
}
   		
/* ========================================================================
	Function   - mfSetQuickAnimation
	Description- inline to set the Skip every other frame
	Returns
   ======================================================================== */
inline void ArtThing::mfSetQuickAnimation(BOOL const flag)
{   
   	mythings[ThingIndex].SkipFrame = flag;
}
   		
/* ========================================================================
	Function   - mfIsQuickAnimation
	Description- inline to check whether this is a double speed animation
	Returns
   ======================================================================== */
inline BOOL const ArtThing::mfIsQuickAnimation() const
{   
   	return (mythings[ThingIndex].SkipFrame == TRUE);
}
   		
/* ========================================================================
   Function    - mfMoveTo
   Description - inline to move a thing
   Returns     - 
   ======================================================================== */
inline void ArtThing::mfMoveTo(LONG const X, LONG const Y)
{
	move_thing_to( ThingIndex, X, Y );
}

/* ========================================================================
   Function    - mfMoveToXYA
   Description - inline to move a thing
   Returns     - 
   ======================================================================== */
inline void ArtThing::mfMoveToXYA(LONG const X, 
								  LONG const Y, 
								  LONG const A)
{
	move_thing_to( ThingIndex, X, Y );
	mythings[ThingIndex].angle = A;
}
	
/* ========================================================================
   Function    - mfMoveToXYZA
   Description - inline to move a thing (for flying)
   Returns     - 
   ======================================================================== */
inline void ArtThing::mfMoveToXYZA(LONG const X, 
								   LONG const Y, 
								   LONG const Z, 
								   LONG const A)
{
	set_thing( ThingIndex, X, Y, Z, A );
}
	
/* ========================================================================
   Function    - mfInitVals
   Description - inline to init the values in this thing
   Returns     - 
   ======================================================================== */
inline void ArtThing::mfInitVals()
{
	ThingType = NO_THING;
	ThingIndex = fERROR;
	LowResThingType = NOLOW;
	UltraLowResThingType = NOLOW;
}

/* ========================================================================
   Function    - FaceTo
   Description - Rotate my angle to face a X,Y point
   Returns     - new angle
   ======================================================================== */
inline LONG const ArtThing::FaceTo (
        LONG const XP,
        LONG const YP
)
{
        const LONG A = AngleFromPoint( mythings[ThingIndex].x,
                        mythings[ThingIndex].y,
                        XP,
                        YP,
                        RESOLUTION_8 );
        return(mythings[ThingIndex].angle = ((UBYTE)A));
}

/* ========================================================================
   Function    - mfCanBePickedUp
   Description - inline to test if this ArtThing can be picked up.
   Returns     - TRUE | FALSE
   ======================================================================== */
inline const BOOL ArtThing::mfCanBePickedUp() const
{
	return GAME_TTYPE::mfCanBePickedUp(mfType());
}

/* ========================================================================
   Function    - mfGetSectorInfo
   Description - inline to get information about the sector the art is in.
   Returns     - TRUE | FALSE
   ======================================================================== */
inline void ArtThing::mfGetSectorInfo(
					LONG * const pFloorHeight,
                    LONG * const pCeilingHeight,
                    LONG * const pSpecial) const
{
	LONG Tag;
	
	sector_info(mythings[ThingIndex].sect,
	            pFloorHeight,
	            pCeilingHeight,
	            pSpecial,
	            &Tag);
}

/* ========================================================================
   Function    - mfGetSector
   Description - inline to get the sector that the art is in.
   Returns     - the sector
   ======================================================================== */
inline const LONG ArtThing::mfGetSector() const
{
	return mythings[ThingIndex].sect;
}

/* ========================================================================
   Function    - mfChangeScale
   Description - inline to adjust the scale 
   Returns     - 
   ======================================================================== */
inline UBYTE const ArtThing::mfChangeScale(LONG const newScale) 
{
	UBYTE const OldScale = mythings[ThingIndex].scale_adjust;
	change_scale_adjust(ThingIndex, newScale);
	return OldScale;
}

/* ========================================================================
   Function    - mfGetDescription
   Description - inline to Get the art type name
   Returns     - 
   ======================================================================== */
inline char const * const ArtThing::mfGetDescription() const
{
	return GAME_TTYPE::mfGetDescription(ThingType);
}

/* ========================================================================
   Function    - mfRemapColor
   Description - inline to Change the remap colors.
   Returns     - 
   ======================================================================== */
inline void ArtThing::mfRemapColor(LONG const RemapIndex)
{
	mythings[ThingIndex].ColorRemap = (SBYTE) (RemapIndex % MAX_cREMAP_TABLE);
}

/* ========================================================================
   Function    - mfGetRemapColor
   Description - inline to retrieve the remap color.
   Returns     - 
   ======================================================================== */
inline LONG const ArtThing::mfGetRemapColor() const
{
	return mythings[ThingIndex].ColorRemap;
}

/* ========================================================================
   Function    - mfIsInvisible
   Description - inline to Test whether an invisiblity spell is on this thing.
   Returns     - 
   ======================================================================== */
inline const BOOL ArtThing::mfIsInvisible() const
{
	return (mythings[ThingIndex].inVisible == TRUE);
}
/* ========================================================================
   Function    - mfSetInvisible
   Description - inline to set the invisiblity spell is on this thing.
   Returns     - 
   ======================================================================== */
inline void ArtThing::mfSetInvisible(BOOL const flag)
{
	 mythings[ThingIndex].inVisible = (unsigned int) flag;
}


/* ========================================================================
   Function    - mfGetArtSequence
   Description - inline Get the actual art sequence running. (Could be stand.)
   Returns     - 
   ======================================================================== */
inline LONG const ArtThing::mfGetArtSequence() const
{
	return (mythings[ThingIndex].iSequence);
}


/* ========================================================================
   Function    - mfWading
   Description - inline See whether we are wading in any of the various
                 stuff, swamp, acid, lava.
   Returns     - 
   ======================================================================== */
inline BOOL const ArtThing::mfWading() const
{
	LONG FloorHeight;
	LONG CeilingHeight;
	LONG Special;
	
	mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
	
	return ((Special == SSP_WATER ||
		     Special == SSP_ACID_FLOOR ||
		     Special == SSP_LAVA) &&
		     mfZ() < FloorHeight ) ? TRUE : FALSE;
}

/* ========================================================================
   Function    - mfInDeepWater
   Description - inline See whether we are standing in deep water or not.
   Returns     - 
   ======================================================================== */
inline BOOL const ArtThing::mfInDeepWater() const
{
	LONG FloorHeight;
	LONG CeilingHeight;
	LONG Special;
	
	mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
	
	return (Special == SSP_DEEP_WATER &&
		     mfZ() < FloorHeight ) ? TRUE : FALSE;
}

#endif // _ATHING_HXX
