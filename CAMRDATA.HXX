/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _CAMRDATA_HXX
#define _CAMRDATA_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#if !defined(_CAMERA_H)
#include "camera.h"
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
	
// This class holds Camera Target data for the SCENE Keys and the Self Running
// demo.
enum {
	NO_TARGET_REQUESTED,
	SWOOP_CAMERA,
	MOVE_CAMERA_EVENLY,
	ROTATE_SWOOP_CAMERA,
	ROTATE_CAMERA_EVENLY
};

class CAMERA_DATA {
public:
	void mfInitVals();
	void mfSetCameraData(LONG  /* X */, 
		                 LONG  /* Y */, 
		                 LONG  /* Z */, 
		                 LONG  /* Angle */, 
		                 LONG  /* Pitch */, 
		                 LONG  /* Roll */);
	void mfSetCamera();
	void mfMoveCamera();
	void mfSwoopCamera();
	BOOL mfIsAtCameraPosition();
	
	void mfWriteData (FILE *);
	BOOL mfReadData	(char * /* cpBuffer */ );
	
protected:
private:
	void *operator new (size_t stAllocateBlock);	// Can't use new/delete
	void operator delete (void *p);

	// A CAMERA is overkill here.
	LONG	fx;
	LONG	fy;
	LONG	fz;
	LONG	fa;
	LONG	fp;
	LONG	fr;		// At the moment the engine doesn't support roll.
};

/* ========================================================================
   Function    - mfInitVals
   Description - initialize the member data.
   Returns     - 
   ======================================================================== */
inline void CAMERA_DATA::mfInitVals() 
{
	fx = 0;
	fy = 0;
	fz = 0;
	fa = 0;
	fp = 0;
	fr = 0;
}

/* ========================================================================
   Function    - mfSetCameraData
   Description - 
   Returns     - 
   ======================================================================== */
inline void CAMERA_DATA::mfSetCameraData(
                     LONG  X, 
	                 LONG  Y, 
	                 LONG  Z, 
	                 LONG  Angle, 
	                 LONG  Pitch, 
	                 LONG  /* Roll */)
{
	fx = X;
	fy = Y;
	fz = Z;
	fa = Angle;
	fp = Pitch;
}

/* ========================================================================
   Function    - mfSetCamera
   Description - Move the camera to this absolute position.
   Returns     - 
   ======================================================================== */
inline void CAMERA_DATA::mfSetCamera() 
{
	SetCameraPosition(&camera, fx, fy, fz, fa, fp);
}

/* ========================================================================
   Function    - mfMoveCamera
   Description - Move the camera to this position as if the user pushed the
                 camera movement keys.
   Returns     - 
   ======================================================================== */
inline void CAMERA_DATA::mfMoveCamera() 
{
	// Do the camera swoop.
	SetCameraTarget(&camera, fx, fy, fz, fa, fp, FALSE);
}

/* ========================================================================
   Function    - mfSwoopCamera
   Description - Move the camera to this position with accelleration and
                 decelleration..
   Returns     - 
   ======================================================================== */
inline void CAMERA_DATA::mfSwoopCamera() 
{
	// Do the camera swoop.
	SetCameraTarget(&camera, fx, fy, fz, fa, fp, TRUE);
}

/* ========================================================================
   Function    - mfIsAtCameraPosition.
   Description - For testing to see if you need to keep waiting for the Move 
                 Camera call.
   Returns     - 
   ======================================================================== */
inline BOOL CAMERA_DATA::mfIsAtCameraPosition() 
{
 //	return ( (CAMERA_INT_VAL(camera.x) == fx &&
 //	          CAMERA_INT_VAL(camera.y) == fy &&
 //	          camera.z == fz &&
 //	          camera.a == fa &&
 //	          camera.p == fp 
 //	          ) ? TRUE : FALSE);
   return(camera.TargetRequested==	NO_TARGET_REQUESTED);
}
/* ========================================================================
   Function    - mfReadData
   Description - Read the data from the buffer
   Returns     - TRUE or FALSE if successful
   ======================================================================== */
inline BOOL CAMERA_DATA::mfReadData (char * cpBuffer) 
{
	LONG fr;
	return ((6 == sscanf(cpBuffer, "%ld %ld %ld %ld %ld %ld",
											&fx,
											&fy,
											&fz,
											&fa,
											&fp,
											&fr)) ? TRUE : FALSE);
}

/* ========================================================================
   Function    - mfWriteData
   Description - Write the data back to the scene file.
   Returns     - 
   ======================================================================== */
inline void CAMERA_DATA::mfWriteData (FILE *fp) 
{
	fprintf(fp,"%ld\t%ld\t%ld\t%ld\t%ld\t%ld\n", fx, fy, fz, fa, fp, 0);
}

#endif // _CAMRDATA_HXX
