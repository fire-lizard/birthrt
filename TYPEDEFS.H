/* =======================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: TYPEDEFS.H   -Header file for all core types
   Author: Everyone
   ======================================================================== */
#ifndef _TYPEDEFS_H
#define _TYPEDEFS_H

#if !defined(LONG)
	typedef signed long		LONG;
#endif

//#ifndef _WINDOWS
//ypedef signed long		LONG;
#if defined(_WINDOWS)
	typedef int	BOOL;
#else
	typedef unsigned long	BOOL;
#endif
//#endif

#ifndef TRUE
	#define TRUE	1
#endif

#ifndef FALSE
	#define FALSE	0
#endif

#ifndef NULL
	#define NULL	0L
#endif

// only use for data
typedef signed char		SBYTE;
typedef unsigned char	UBYTE;
typedef signed short	SHORT;
typedef unsigned short	USHORT;
typedef signed short	ERRCODE;
typedef unsigned long	ULONG;
typedef unsigned char *	PTR;
typedef PTR	*			HDL;

// only use for strings
typedef char			CHAR;
typedef unsigned char	UCHAR;
typedef char *			CSTRPTR;

// function prototype types
typedef void	(*PFV)(void);
typedef LONG	(*PFLV)(void);
typedef LONG	(*PFLL)(LONG);
typedef void	(*PFVL)(LONG);
typedef int		(*PFIL)(LONG);
typedef void	(*PFVLLL)(LONG,LONG,LONG);
typedef void	(*PFVLL)(LONG,LONG);
typedef void	(*PFVLLB)(LONG,LONG,BOOL);
typedef LONG	(*PFLLLL)(LONG,LONG,LONG);

// match graphics structs
//#ifndef _WINDOWS
#ifndef _WINDEF_
	typedef struct _RECT
	{
	    LONG    left;
	    LONG    top;
	    LONG    right;
	    LONG    bottom;
	} RECT, *PRECT;
	
	typedef struct {
		LONG		x, y;
	} POINT;

	typedef unsigned long   DWORD;
	typedef unsigned int    UINT;
	typedef unsigned char BYTE;
	typedef BYTE *	    LPBYTE;

#endif

 /* Flic File Header */
 typedef struct
 {
 	LONG		size;				/* Size of flic including this header. */
 	USHORT	type;				/* Either FLI_TYPE or FLC_TYPE below. */
 	USHORT	frames;			/* Number of frames in flic. */
 	USHORT	width;			/* Flic width in pixels. */
 	USHORT	height;			/* Flic height in pixels. */
 	USHORT	depth;			/* Bits per pixel.	(Always 8 now.) */
 	USHORT	flags;			/* FLI_FINISHED | FLI_LOOPED ideally. */
 	LONG		speed;			/* Delay between frames. */
 	USHORT	scale;			/* Set to zero. */
 	SHORT		xCenter;			/* */
 	SHORT		yCenter;			/* */
 //!! fix padding bug in visual c++ !!!!!!!!!!!!!!!!!!!!!
 //	ULONG		creator;			/* Serial # of flic creator. (FLC only.) */
 //	ULONG		updated;			/* Date of flic update. (FLC only.) */
 //	ULONG		updater;			/* Serial # of flic updater. (FLC only.) */
 	USHORT	creator1;		/* Serial # of flic creator. (FLC only.) */
 	USHORT	creator2;		/* Serial # of flic creator. (FLC only.) */
 	USHORT	updated1;		/* Date of flic update. (FLC only.) */
 	USHORT	updated2;		/* Date of flic update. (FLC only.) */
 	USHORT	updater1;		/* Serial # of flic updater. (FLC only.) */
 	USHORT	updater2;		/* Serial # of flic updater. (FLC only.) */
 //!! fix padding bug in visual c++ !!!!!!!!!!!!!!!!!!!!!
 	USHORT	aspect_dx;		/* Width of square rectangle. (FLC only.) */
 	USHORT	aspect_dy;		/* Height of square rectangle. (FLC only.) */
 	UBYTE		reserved2[38];	/* Set to zero. */
 	ULONG		oframe1;			/* Offset to frame 1. (FLC only.) */
 	ULONG		oframe2;			/* Offset to frame 2. (FLC only.) */
 	UBYTE		reserved3[40];	/* Set to zero. */
 } FLICHEAD, *FLICHEADPTR;



// Note: X & Y for the 3D POINT & VECTOR types must be the first
//       two data items so we can call the 2D rotate point fns on them.

typedef struct _POINT_3D
{
	LONG x;
	LONG y;
	LONG z;
} POINT_3D, *PTR_POINT_3D;

typedef struct _VECTOR
{
	LONG dx;
	LONG dy;
} VECTOR, *PTR_VECTOR;

typedef struct _VECTOR_3D
{
	LONG dx;
	LONG dy;
	LONG dz;
} VECTOR_3D, *PTR_VECTOR_3D;


typedef struct
{
	POINT a,b;
} LINE2D;

typedef POINT 		FIXED_POINT;       	// For 24.8 fixed point math points.
typedef POINT_3D	FIXED_POINT_3D;		// x & y are 28.8 and z is LONG
typedef VECTOR		FIXED_VECTOR;   	// a pair of 24.8 delta values.
typedef VECTOR_3D	FIXED_VECTOR_3D;	// x & y are 24.8 and z is a LONG.

// The following two data types are for 16.16 fixed point integers.
// It only works on INTEL machines. And is used instead of >>16 to get
// at the high order bits.
typedef struct {
	SHORT	sFraction;
	SHORT	sHigh;
	} SPLIT_LONG;
typedef union
{
  LONG			lval;
  SPLIT_LONG	TwoHalves;
  
} FIXED16;
/* -----------------------------------------------------------------
   Pragmas
   ----------------------------------------------------------------- */
// init cont problems with shorts
#pragma warning 389 4
// unused variables
#pragma warning  14 4


#endif // _TYPEDEFS_H

