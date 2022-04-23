/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: athing.cpp  -
   Author:   Greg Hightower

   ========================================================================

   Contains the following general functions:


   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "TYPEDEFS.H"
#include "ENGINE.H"
#include "THINGS.H"
#include "MACHINE.H"
#include "ATHING.HXX"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

// Use a distance slightly larger than combat so that we aren't constantly
// swaping animations.
#define BALLOON_HIRES_DISTANCE	(150 << CAMERA_FIXEDPT)
#define BALLOON_LOWRES_DISTANCE	(300 << CAMERA_FIXEDPT)

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
/* ========================================================================
   Function    - mfDefineArtThing
   Description - inline to create a new thing
   Returns     - 
   ======================================================================== */
   // This routine initializes the ArtThing data.
void ArtThing::mfDefineArtThing(THINGTYPE const tt, 
								   LONG const x, 
								   LONG const y, 
								   LONG const z, 
								   LONG const a)
{
	THINGTYPE UseType = tt;
	LowResThingType = GAME_TTYPE::mfGetLowResType(tt);
	
	ThingType = tt;
	
	// If there is low res art
	if (LowResThingType != NOLOW)
	{
		UltraLowResThingType = (THINGTYPE const) GetMinMemConvert(LowResThingType);
		
		LONG const d = (aprox_dist(CAMERA_INT_VAL(camera.x),
					   		       CAMERA_INT_VAL(camera.y), 
					   		       x, y)) << PLAYER_FIXEDPT;
		
		
		if (d > BALLOON_HIRES_DISTANCE)
		{
			if(UltraLowResThingType != NOLOW)
				UseType = UltraLowResThingType;
			else
				UseType = LowResThingType;
		}
		else
		{
			UseType = LowResThingType;
		}
	}
	else
	{
		UltraLowResThingType = NOLOW;
	}
	
	// create the mything
	ThingIndex = create_thing( UseType, x, y, z);
	mythings[ThingIndex].angle = (UBYTE) a;
	mythings[ThingIndex].OriginalType = tt;
	
	const LONG ss=find_ssector(mythings[ThingIndex].x,mythings[ThingIndex].y);
	add_thing(ThingIndex,ss);
}

/* ========================================================================
   Function    - mfTestAndUseLowResArt
   Description - inline to set the art type to low res if we are far from
   				 the camera.
   Returns     - 
   ======================================================================== */
void ArtThing::mfTestAndUseLowResArt()
{
	if (LowResThingType != NOLOW)
	{
		// try to res the art down to min
		if (mythings[ThingIndex].dist > BALLOON_LOWRES_DISTANCE)
		{
			if (mythings[ThingIndex].type != UltraLowResThingType)
			{
				if(UltraLowResThingType != NOLOW)
					ChangeThingType(ThingIndex, UltraLowResThingType);
				else
					ChangeThingType(ThingIndex, LowResThingType);
			}
		}
		else
		// try to res the art down to med
		if (mythings[ThingIndex].dist > BALLOON_HIRES_DISTANCE)
		{
			// try to res the art down to min
			if (mythings[ThingIndex].type != LowResThingType)
			{
				ChangeThingType(ThingIndex, LowResThingType);
			}
		}
		else if (mythings[ThingIndex].type != ThingType)
		// try to res the art up to max
		{
			ChangeThingType(ThingIndex, ThingType);
		}
	}
}

/* ========================================================================
   Function    - mfGetDeathFrameHeightWidth
   Description - updates the height and width fields in the mythings array
   Returns     -
   ======================================================================== */
void ArtThing::mfSetToDeathHeightWidth()
{
	//note: mt is a reference... we're modifying the array, not a copy
	MYTHING& mt=mythings[ThingIndex];

	

	if (mt.iAnim==fERROR)
		return;
	ANIMPTR pAnim=(ANIMPTR)BLKPTR(mt.iAnim);


	//this moves memory, we need to reset pAnim afterwards.
	load_FLC_sequence(mt.iAnim,mt.OriginalType,ANIMATION12SEQ,0);
	SetAnimSequence(mt.iAnim,ANIMATION12SEQ*NUMROTATIONS);

	if (mt.iAnim==fERROR)
		return;
 
	pAnim=(ANIMPTR)BLKPTR(mt.iAnim);
	mt.heiScaled=pAnim->width;
	mt.widScaled=pAnim->height;
}
