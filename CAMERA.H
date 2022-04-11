/* ========================================================================
   Copyright (c) 1990,1997	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _CAMERA_H
#define _CAMERA_H

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define CAMERA_FIXEDPT	8
#define CAMERA_INT_VAL(x)	((x) >> CAMERA_FIXEDPT)
#define CAMERA_FLOAT_VAL(x)	(x)

/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
typedef struct {
	LONG		x;	// Target coordinates
	LONG		y;
	LONG		z;
	LONG		a;
	LONG		p;
} CAMERA_LINEAR_TARGET;

typedef struct {
	LONG		x;     		// face to pt.
	LONG		y;    		// face to pt.
	LONG		z;	   		// New Z
	LONG		p;	   		// New Pitch
	LONG		a;			// destination angle in Polar coords. (a, distance)
	LONG		h;			// New Height
	LONG		radius;		// New radius
	LONG		distance;	// New min distance to pt.
	LONG		max_distance;	// New max distance to pt.
} CAMERA_ROTATIONAL_TARGET;

typedef struct{
	LONG		x,y;		// xy
	LONG		z;			// height above floor
	LONG		a,p;		// angle, pitch
	LONG		TargetRequested; // A flag
	LONG		TargetFactor;
	LONG		Rate[4];	 //Rate[0]: distance rate    Rate[1]: height rate
							 //Rate[2]: angle rate       Rate[3]: pitch rate
	union	{
		CAMERA_LINEAR_TARGET  		LTarget;
		CAMERA_ROTATIONAL_TARGET	RTarget;
	};
	
	CAMERA_ROTATIONAL_TARGET	Current;
	
	LONG		vx;			// Velocity values.
	LONG		vy;
	LONG		vz;
	LONG		va;
} CAMERA;

typedef struct{
	LONG		x,y;		// xy
	LONG		z;			// height above floor
	LONG		a,p;		// angle, pitch
	LONG		attributes; // unused so far
	LONG		width;		// width of buffer
	LONG		height;		// height of buffer
	SHORT		buffer;		// handle to buffer
} CAMERA_DESCRIPTOR;
#define MAX_LEVEL_CAMERAS 12

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

extern CAMERA		camera;

#if defined (__cplusplus)
}
#endif

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

void InitializeCamera(CAMERA *, LONG /* X */,
								LONG /* Y */,
								LONG /* Z */,
								LONG /* A */,
								LONG /* P */);
void SetCameraPosition(CAMERA *,const LONG x,const LONG y,const LONG z, const LONG a, const LONG p);
#define SetCameraXYZA(c,x,y,a)	SetCameraPosition((c),(x),(y),(z),(a),NO_CHANGE)
#define SetCameraXYA(c,x,y,a)	SetCameraPosition((c),(x),(y),NO_CHANGE,(a),NO_CHANGE)
#define SetCameraAngel(c,a)		SetCameraPosition((c),NO_CHANGE,NO_CHANGE,NO_CHANGE,(a),NO_CHANGE)
#define SetCameraPitch(c,p)		SetCameraPosition((c),NO_CHANGE,NO_CHANGE,NO_CHANGE,NO_CHANGE,(p))
void SetCameraCurrentPosition(CAMERA *,const LONG x,const LONG y,const LONG z, const LONG angle, const LONG p, const LONG radius);
void SetCameraMotionVars( CAMERA *pCamera, const LONG	TargetFactor, const LONG	LinearRate);
#define SetCameraTargetFactor(c,tf)	SetCameraMotionVars((c),(tf),NO_CHANGE)
#define SetCameraLinearRate(c,lr)	SetCameraMotionVars((c),NO_CHANGE,(lr))
void SetCameraTarget( CAMERA *, const LONG Targetx, const LONG Targety, const LONG Targetz, const LONG Targeta, const LONG Targetp, const BOOL swoop);
void SetCameraRotateTarget(CAMERA *,
						   const LONG Targetx,
						   const LONG Targety,
						   const LONG NewZ,
						   const LONG NewPitch,
						   const LONG DeltaAngle,
						   const LONG MinRadius,
						   const LONG NewDistance,
						   const BOOL swoop);
void MoveCamera(CAMERA *, const LONG KeyState);
void SwoopCamera(CAMERA *pCamera);
void MoveCameraEvenly(CAMERA *pCamera);
void RotateSwoopCamera(CAMERA *pCamera);
void RotateCameraEvenly(CAMERA *pCamera);
void push_camera(void);
void pull_camera(void);

void RotateCameraAroundPoint(CAMERA * pCamera, LONG KeyState);
void SetCameraRotatePointTarget( CAMERA *pCamera, const LONG , const LONG , const LONG, const LONG , const LONG , const BOOL );
void SetCameraRotatePointRates( CAMERA *pCamera, const LONG , const LONG , const LONG , const LONG, const LONG, const LONG );



#if defined (__cplusplus)
}
#endif
#endif // _CAMERA_H
