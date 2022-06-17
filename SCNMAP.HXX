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

protected:
private:
	MAP();
	~MAP();
	void *operator new (size_t stAllocateBlock);	// Can't use new/delete
	void operator delete (void *p);
	MAP(const MAP &);			// No copies
	const MAP operator=(const MAP &);	// No assignments
};
	
/* ========================================================================
   Function    - mfSetZoomFactor
   Description - From the buffer set the zoom factor.
   Returns     - 
   ======================================================================== */
inline void MAP::mfSetZoomFactor (char *cpBuffer) 
{
	ZoomMap(0,atol(cpBuffer));
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
	}
	else
	if (strcmp(cpBuffer, "Off") == 0)
	{
		SetMapDraw(0,FALSE);
	}
}

#endif // _SCNMAP_HXX
