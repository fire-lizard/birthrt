/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _SCNPLACE_HXX
#define _SCNPLACE_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_ATHING_HXX)
#include "athing.hxx"
#endif

#if !defined(_SCNINFO_HXX)
#include "scninfo.hxx"
#endif


/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

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
   Prototypes
   ------------------------------------------------------------------------ */
typedef SHORT HDL_PLACE;
	
// Place's inherit from ArtThing because it is really an object draw in the 
// scene.

class PLACE : public ArtThing {
public:
	void mfCreatePlace(char *);
	void mfDeletePlace();
#if defined(SCENE_EDITOR)
	void mfWritePlaceData(FILE *fp);
#endif
	void mfInitVals();

	void mfDrawDiamond ();
	char *mfGetSceneName();

protected:
private:
	PLACE();
	~PLACE();
	void *operator new (size_t stAllocateBlock);	// Can't use new/delete
	void operator delete (void *p);
	PLACE(const PLACE &);			// No copies
	const PLACE operator=(const PLACE &);	// No assignments
	
	SCENE_INFO	fscInfo;
};
typedef SHORT HDL_PLACES;
typedef PLACE *PTR_PLACE;
typedef PLACE *PTR_PLACES;
	
/* ========================================================================
   Function    - mfDeletePlace
   Description - Release the place data and remove it from the mythings array
   Returns     - 
   ======================================================================== */
inline void PLACE::mfDeletePlace()
{
}

/* ========================================================================
   Function    - mfInitVals
   Description - Initialize the data since we can't have a constructor
   Returns     - 
   ======================================================================== */
inline void PLACE::mfInitVals() 
{
	ArtThing::mfInitVals();
	fscInfo.mfInitVals();
}

/* ========================================================================
   Function    - mfCreatePlace
   Description - From a scene file line, create a PLACE 
   Returns     - 
   ======================================================================== */
inline void PLACE::mfCreatePlace(char *cpBuffer)
{
	LONG X, Y;
	THINGTYPE ThingType;
	
	mfInitVals();
	
	if( 4 == sscanf(cpBuffer, "%ld %ld %ld %s", &X, 
	                                            &Y, 
	                                            &ThingType, 
	                                            fscInfo.mfGetSceneName()))
	{
		
		mfDefineArtThing(ThingType, X, Y);
	}
}

/* ========================================================================
   Function    - mfWritePlaceData
   Description - Write the scene file data.
   Returns     - 
   ======================================================================== */
#if defined(SCENE_EDITOR)
inline void PLACE::mfWritePlaceData(FILE *fp) 
{
	fprintf(fp, "%ld\t%ld\t%ld\t%s\n",mfX(), 
	                                  mfY(), 
	                                  mfType(), 
	                                  fscInfo.mfGetSceneName());
}
#endif

/* ========================================================================
   Function    - mfDrawDiamond
   Description - Draw the diamond on the map.
   Returns     - 
   ======================================================================== */
inline void PLACE::mfDrawDiamond () 
{
//WRC    **places don't show on map, _for_now_**
//	POINT a;       		
//	
//	a.x = mfX();
//	a.y = mfY();
//	
//	draw_diamond(&a, 254);
}

/* ========================================================================
   Function    - mfGetSceneName
   Description - Get the scene name from the place class.
   Returns     - 
   ======================================================================== */
inline char *PLACE::mfGetSceneName() 
{
	return fscInfo.mfGetSceneName();
}
#endif // _SCNPLACE_HXX
