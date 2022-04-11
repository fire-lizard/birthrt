/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _SCNMAP_HXX
#define _SCNMAP_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#if !defined(_SYSTEM_H)
#include "system.h"
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
class MAP {
public:
    typedef enum {
    	ON,
    	OFF,
    	NOT_SET
    } TRI_STATE;
    
    LONG mfReadMap(FILE *);
	void mfSetZoomFactor (char * /* cpBuffer */);
	void mfSetMapState (char * /* cpBuffer */);
	
#if defined(SCENE_EDITOR)
	void mfWriteMapData(FILE * /* fp */);
	LONG mfCountOfChanges();
#endif

	void mfInitVals();

protected:
private:
	MAP();
	~MAP();
	void *operator new (size_t stAllocateBlock);	// Can't use new/delete
	void operator delete (void *p);
	MAP(const MAP &);			// No copies
	const MAP operator=(const MAP &);	// No assignments
	
#if defined(SCENE_EDITOR)
	BOOL fbZoomFactorSet;
	LONG fZoomFactor;
	TRI_STATE fMapDisplayed;
#endif
};
	
/* ========================================================================
   Function    - mfSetZoomFactor
   Description - From the buffer set the zoom factor.
   Returns     - 
   ======================================================================== */
inline void MAP::mfSetZoomFactor (char *cpBuffer) 
{
	ZoomMap(0,atol(cpBuffer));
#if defined(SCENE_EDITOR)
	fbZoomFactorSet = TRUE;
	fZoomFactor = MapZoomFactor();
#endif
}

/* ========================================================================
   Function    - mfSetMapState
   Description - From the butter turn the Map on or off.
   Returns     - 
   ======================================================================== */
inline void MAP::mfSetMapState (char *cpBuffer) 
{
	if (strcmp(cpBuffer, "On") == 0)
	{
		SetMapDraw(0,TRUE);
#if defined(SCENE_EDITOR)
		fMapDisplayed = ON;
#endif
	}
	else
	if (strcmp(cpBuffer, "Off") == 0)
	{
		SetMapDraw(0,FALSE);
#if defined(SCENE_EDITOR)
			fMapDisplayed = OFF;
#endif
	}
}

/* ========================================================================
   Function    - mfWriteMapData
   Description - Write the scene file data.
   Returns     - 
   ======================================================================== */
#if defined(SCENE_EDITOR)
inline void MAP::mfWriteMapData(FILE *fp) 
{
	if (fbZoomFactorSet)
	{
		fprintf(fp, "ZoomFactor %ld\n", fZoomFactor);
	}
		
	if (fMapDisplayed != NOT_SET)
	{
		fprintf(fp,"MapState %s\n", (fMapDisplayed == ON) ? "On" : "Off" );
	}
}

/* ========================================================================
   Function    - mfCountOfChanges
   Description - Count the number of lines for the scene file.
   Returns     - 
   ======================================================================== */
inline LONG MAP::mfCountOfChanges() 
{
	return (fbZoomFactorSet + (fMapDisplayed != NOT_SET));
}
#endif

/* ========================================================================
   Function    - mfInitVals
   Description - Initialize the member data.
   Returns     - 
   ======================================================================== */
inline void MAP::mfInitVals() 
{
#if defined(SCENE_EDITOR)
	fbZoomFactorSet = FALSE;
	fZoomFactor = MapZoomFactor();
	fMapDisplayed = NOT_SET;
#endif
}

#endif // _SCNMAP_HXX
