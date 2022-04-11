/* =®RM250¯=======================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: POINT.C
   Author: Chris Phillips
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:
   set_view          -sets various variables dealing with the players view
   round             -rounds a number to the nearest integral value
   point_relation    -???
   back_face_point   -???
   aprox_dist        -approximate distance between two points
   dist              -distance between two points
   xlate             -translates absolute coordinates to the player's position
   rot8              -rotates a vector to player's angle
   rot8_angle        -rotates a vector to a specified angle
   proj              -???
   cross_prod        -gives the cross product of two vectors
   dot_prod          -gives the dot product of two vectors
   unit_normal       -gives the unit_normal of a vector
   AngleFromPoint    -Return the angle of a point relative to another point

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "system.h"
#include "engine.h"
#include "engint.h"
#include "sincos.h"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
	//REVISIT:Dunno. should look at this file for Kludges and
	//        speed issues. Plus add funcionality.. maybe matrix's

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define M_PI				3.14159265358979323846
#define ANGLE_MAX			(255)
#define ANGLE_STEPS		(256)
#define ANGLE_MULTI		(1024)

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
void RotateAnglePoint (POINT *p, POINT *origin, ULONG a);
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

LONG	view_sin, view_cos;
LONG	anti_view_sin, anti_view_cos;

/* ======================================================================
   Function    - set_view
   Description - sets various variables dealing with the players view
   Returns     - void
	UTB CMP 10-28 Now calcs the view frustrum as well.
   ======================================================================== */
LINE2D view_frust_right,view_frust_left;

#define FRUST_LINE_LEN		(100<<4)
void set_view()
{
	ULONG a;
#if 0
	POINT temp;
#endif

	/* insure that the angle is valid */
	a=camera.a;
	a = a % ANGLE_STEPS;

	/*setup rotate values*/
	view_sin=sin_table[a];
	view_cos=cos_table[a];

	render_center_y = render_top + (render_height/2) + ((camera.p * render_height)/480);

#if 0
		//set view frust origin
	view_frust_right.a.x=view_frust_left.a.x=camera.x>>8;
	view_frust_right.a.y=view_frust_left.a.y=camera.y>>8;


		//set right edge
	temp.x=-FRUST_LINE_LEN;
	temp.y=FRUST_LINE_LEN;
	rot8_angle(&temp,-camera.a);
	view_frust_right.b.x=temp.x+(camera.x>>8);
	view_frust_right.b.y=temp.y+(camera.y>>8);

		//set left edge
	temp.x=FRUST_LINE_LEN;
	temp.y=FRUST_LINE_LEN;
	rot8_angle(&temp,-camera.a);
	view_frust_left.b.x=temp.x+(camera.x>>8);
	view_frust_left.b.y=temp.y+(camera.y>>8);
#endif
}

#if defined(UNUSED)
/* =======================================================================
   Function    - round
   Description - rounds a number to the nearest integral value
   Returns     - the integral value
   ======================================================================== */
// TODO: (fire lizard) uncomment
//LONG round (double number)
//{
//	return (LONG)((double)number + (double) 0.5);
//}
#endif // UNUSED

/* =======================================================================
   Function    - point_relation
   Description - given a node n, look at the line (actually a vector,
					  because direction is important) from the node's (x,y) to
					  the node's (x+a,y+b).  Check the slope of that line against
					  the slope of the line from the node's (x,y) to point (x1,y1).
					  If the slope of the node's line is less, then the point
					  (x1,y1) is in BACK of the node line, otherwise it is in FRONT.
					  (We arbitrarily chose left = back and right = front, and
					  everything in the code uses this convention.)

					  We don't handle the case of the point being on the line--
					  we treat it as in BACK in that case.

					  We don't compare the slopes directly, to avoid division by 0.
					  Instead of comparing dy1/dx1 with dy/dx, we use the equivalent
					  comparison dx * dy1 with dy * dx1.
							dx = nodes[n].a
							dy = nodes[n].b
							dx1 = x1 - nodes[n].x
							dy1 = y1 - nodes[n].y

					  ASIDE: a and b are unfortunate names used in the NODE
					  structure for the change in x and the change in y.
					  Elsewhere, a and b are indexes to vertexes.
					
   Returns     - which side (x1,y1) is on, FRONT or BACK.
   ======================================================================== */
LONG point_relation(LONG n,LONG x1,LONG y1)
{
	BR_NODE * const pNode = &nodes[n];	// GWP

	// GWP Added optimzation to remove the array index addition.
	// GWP r= ( nodes[n].a *( y1-nodes[n].y ) ) - ( ( x1-nodes[n].x) * nodes[n].b );
	
	LONG const r= ( pNode->a *( y1-pNode->y ) ) - ( ( x1-pNode->x) * pNode->b );
	
	return ((r<0) ? FRONT : BACK);
}

/* =======================================================================
   Function    - back_face_point
   Description - Determines whether you're in back of a line from point
					  a to point b (the direction is important--we imagine
					  we're moving from point a to point b, and arbitrarily
					  declare points on our left to be in back of the line
					  and points on our right to be in front of the line).
   Returns     - 1 if (x,y) is on the "back" (= left) side of the line
					  from vertex a to vertex b or on the line, 0 otherwise.
	Mods        - [d8-01-96 JPC] Added x and y parameters instead of
					  using globals camera.x and camera.y--fixes a bug
					  detected by Lan and Gary.
					  Also changed return value from TRUE and FALSE to BACK and
					  FRONT to show more clearly what the function is doing.
   ======================================================================== */
LONG back_face_point(POINT *a,POINT *b, LONG x, LONG y)
{
	LONG const dx=(b->x-a->x);
	LONG const dy=(b->y-a->y);
	// Old line:
	// r= ( dx *( CAMERA_INT_VAL(camera.y)-a.y ) ) - ( ( CAMERA_INT_VAL(camera.x)-a.x) * dy );
	LONG const r =  dx * (y - a->y) - (x - a->x) * dy;
			                          // x,y is in front of line from a to b
   return ((r < 0) ? FRONT : BACK);   // x,y is in back of line from a to b
}


/* =======================================================================
   Function    - back_face_vertex
   Description - Determines whether you're in back of a line from vertex
					  a to vertex b (the direction is important--we imagine
					  we're moving from vertex a to vertex b, and arbitrarily
					  declare points on our left to be in back of the line
					  and points on our right to be in front of the line).
   Returns     - BACK if (x,y) is on the "back" (= left) side of the line
					  from vertex a to vertex b or on the line, FRONT otherwise.
	Mods        - [d8-01-96 JPC] Added x and y parameters instead of
					  using globals camera.x and camera.y--fixes a bug
					  detected by Lan and Gary.
					  Also changed return value from TRUE and FALSE to BACK and
					  FRONT to show more clearly what the function is doing.
   ======================================================================== */
LONG back_face_vertex (LONG a, LONG b, LONG x, LONG y)
{
	LONG const dx=(vertexs[b].x-vertexs[a].x);
	LONG const dy=(vertexs[b].y-vertexs[a].y);
	// Old line:
	// r= ( dx *( CAMERA_INT_VAL(camera.y)-vertexs[a].y ) ) - ( ( CAMERA_INT_VAL(camera.x)-vertexs[a].x) * dy );
	LONG const r =  dx * (y - vertexs[a].y)  -  (x - vertexs[a].x) * dy;
	
                       // FRONT = 0; x,y is in front of line a to b
	
                       // BACK = 1; x,y is in back of line a to b
									// or on the line (r = 0)
	 return (( r < 0) ? FRONT : BACK);
}

/* =======================================================================
   Function    - aprox_dist
   Description - finds the approximate distance between two points
   Returns     - the distance, as a LONG
   ======================================================================== */
/*
A Fast Approximation to the Hypotenuse
by Alan Paeth
from "Graphics Gems", Academic Press, 1990
*/

LONG aprox_dist(LONG x1, LONG y1, LONG x2, LONG y2)
{
/*
 * gives approximate distance from (x1,y1) to (x2,y2)
 * with only overestimations, and then never by more
 * than (9/8) + one bit uncertainty.
 */
	if ((x2 -= x1) < 0) x2 = -x2;
	if ((y2 -= y1) < 0) y2 = -y2;
	return (x2 + y2 - (((x2>y2) ? y2 : x2) >> 1) );
}

/* =======================================================================
   Function    - dist
   Description - finds the exact distance between two points
   Returns     - the distance as a LONG
   ======================================================================== */
LONG dist(LONG x1, LONG y1, LONG x2, LONG y2)
{
	LONG aprox,root,number;

	number=((x1-x2)*(x1-x2))+((y1-y2)*(y1-y2));
	aprox=1;
	root=number;
	while(aprox<root)
	{
		root>>=1;
		aprox<<=1;
	}
	while(aprox>root)
	{
		root=number/aprox;
		aprox=(root+aprox)>>1;
	}
	return(root);
}

/* =======================================================================
   Function    - xlate
   Description - translates absolute coordinates to the player's position
   Returns     - void
   ======================================================================== */
void xlate(POINT *p)
{
	p->x=p->x-CAMERA_INT_VAL(camera.x);
	p->y=p->y-CAMERA_INT_VAL(camera.y);
}

/* =======================================================================
   Function    - xlatePoint
   Description - translates absolute coordinates to the player's position
   Returns     - void
   ======================================================================== */
void xlatePoint(POINT *p,POINT a)
{
	p->x=p->x-a.x;
	p->y=p->y-a.y;
}


/* =======================================================================
   Function    - rot8
   Description - rotates a point about the world orgin to the camera's angle
   				 Note: Y is postive in the 0 angle direction.
   				       X is postive in the 64 bytian angle direction
   				       (See "Computer Graphics Second Ed." by Folly & vanDam p.203)
   				       The Angle coordiate system used by this fn is
   				             64
   				              |
   				          128-+-0
   				              |
   				             192
   				       To convert to the nova engine coordinate system use
   				       -a (angle)
   				       Nova:
   				              0
   				              |
   				          192-+-64
   				              |
   				             128
   Returns     - void
   ======================================================================== */
void rot8 (POINT *p)
{
	const LONG ax = p->x;

   /* calculate SIN and COS using a look-up table */
	p->x =(((((p->x) * view_cos) )
		 - (((p->y) * view_sin ) )) / ANGLE_MULTI);

	p->y =(((((ax) * view_sin ) )
		 + (((p->y) * view_cos ) )) / ANGLE_MULTI);
}

/* =======================================================================
   Function    - rot8dbl
   Description - same as rotate but with more digits of precision
   				 Note: Y is postive in the 0 angle direction.
   				       X is postive in the 64 bytian angle direction
   				       (See "Computer Graphics Second Ed." by Folly & vanDam p.203)
   				       The Angle coordiate system used by this fn is
   				             64
   				              |
   				          128-+-0
   				              |
   				             192
   				       To convert to the nova engine coordinate system use
   				       -a (angle)
   				       Nova:
   				              0
   				              |
   				          192-+-64
   				              |
   				             128
   Returns     - void
   ======================================================================== */
void rot8dbl (POINT *p)
{
	LONG	ix, ix_f, iy, iy_f;

	ix = p->x >> 10;
	ix_f = p->x & 0x3ff;
	iy = p->y >> 10;
	iy_f = p->y & 0x3ff;

   /* calculate SIN and COS using a look-up table */
	p->x = (ix * view_cos) + ((ix_f * view_cos) / ANGLE_MULTI)
			-(iy * view_sin) - ((iy_f * view_sin) / ANGLE_MULTI);

	p->y = (ix * view_sin) + ((ix_f * view_sin) / ANGLE_MULTI)
			+(iy * view_cos) + ((iy_f * view_cos) / ANGLE_MULTI);
}

/* =======================================================================
   Function    - rot8_angle
   Description - rotates a point about the world orgin to a specified angle
   				 Note: Y is postive in the 0 angle direction.
   				       X is postive in the 64 bytian angle direction
   				       (See "Computer Graphics Second Ed." by Folly & vanDam p.203)
   				       The Angle coordiate system used by this fn is
   				             64
   				              |
   				          128-+-0
   				              |
   				             192
   				       To convert to the nova engine coordinate system use
   				       -a (angle)
   				       Nova:
   				              0
   				              |
   				          192-+-64
   				              |
   				             128
   Returns     - void
   ======================================================================== */
void rot8_angle (POINT *p,ULONG a)
{
	const LONG ax = p->x;

	a = a % ANGLE_STEPS;

   /* calculate SIN and COS using a look-up table */
	p->x =(((((p->x) * cos_table[a]) )
		 - (((p->y) * sin_table[a] ) )) / ANGLE_MULTI);

	p->y =(((((ax) * sin_table[a] ) )
		 + (((p->y) * cos_table[a] ) )) / ANGLE_MULTI);
}

/* =======================================================================
   Function    - rot8_r_angle
   Description - rotates a point about the world orgin with a radius to a
                 specified angle
   				 Note: Y is postive in the 0 angle direction.
   				       X is postive in the 64 bytian angle direction
   				       (See "Computer Graphics" by Folly & vanDam p.203)
   				       The Angle coordiate system used by this fn is
   				             64
   				              |
   				          128-+-0
   				              |
   				             192
   				       To convert to the nova engine coordinate system use
   				       -a (angles)
   				       Nova:
   				              0
   				              |
   				          192-+-64
   				              |
   				             128
   Returns     - void
   ======================================================================== */
void rot8_r_angle (POINT *p,ULONG OldAngle, ULONG DeltaAngle, LONG radius)
{
	const LONG RadiusxCosOldAngle = (radius * cos_table[OldAngle])/ANGLE_MULTI;
	const LONG RadiusxSinOldAngle = (radius * sin_table[OldAngle])/ANGLE_MULTI;
	
	OldAngle = OldAngle % ANGLE_STEPS;
	DeltaAngle = DeltaAngle % ANGLE_STEPS;

   /* calculate SIN and COS using a look-up table */
	p->x =((RadiusxCosOldAngle * cos_table[DeltaAngle])
		 - (RadiusxSinOldAngle * sin_table[DeltaAngle])) / ANGLE_MULTI;

	p->y =((RadiusxCosOldAngle * sin_table[DeltaAngle])
		 + (RadiusxSinOldAngle * cos_table[DeltaAngle])) / ANGLE_MULTI;
}


/* =======================================================================
   Function    - proj
   Description - Project a 3d point into 2d space.
   Returns     - void
   ======================================================================== */
void proj(POINT *p,LONG z)
{
	/*make sure no div by 0*/
	if(!p->y)
	{
		//p->y++;

		p->x=(p->x*(render_perspect))+render_center_x;
		p->y=(z*(render_perspect))+render_center_y;
	}
	else
	{
		p->x=((p->x*(render_perspect))/p->y)+render_center_x;
		p->y=((z*(render_perspect))/p->y)+render_center_y;
	}

	// p->x cannot be less than zero or greater than the MAX_VIEW_WIDTH
	// because it is used to as an index into the extents array among others.
	if (p->x < 0)
	{
		p->x = 0;
	}
	else if (p->x > (MAX_VIEW_WIDTH - 1))
	{
		p->x = MAX_VIEW_WIDTH-1;
	}

//	if (p->y < 0)
//		p->y = 0;

}
#if defined (UNUSED)
/* =======================================================================
   Function    - projection
   Description - Planar projection of a 3d point into 2d space.
   Returns     - void
   ======================================================================== */
void projection(POINT *p,LONG z, LONG distanceToCamera)
{
	double divisor;
	
	if (!distanceToCamera)	// full scale.
		return;
		
	divisor = ((double) z)/((double)distanceToCamera);

	p->x=(LONG)( ((double)p->x)/divisor);
	p->y=(LONG)( ((double)p->y)/divisor);
}
#endif


#if defined(UNUSED)
/* =======================================================================
   Function    - cross_prod
   Description - returns the cross product of two vectors
   Returns     - the cross product
   ======================================================================== */
POINT3D cross_prod(POINT3D *a, POINT3D *b)
{
	POINT3D c;
	c.x = (a->y*b->z) - (a->z*b->y);
	c.y = (a->z*b->x) - (a->x*b->z);
	c.z = (a->x*b->y) - (a->y*b->x);
	return(c);
}
#endif // UNUSED

#if defined(UNUSED)
/* =======================================================================
   Function    - dot_prod
   Description - returns the dot_product of two vectors
   Returns     - the dot product as a LONG
   ======================================================================== */
LONG dot_prod(POINT3D *a, POINT3D *b)
{
	a->x=a->x<<8;
	a->y=a->y<<8;
	a->z=a->z<<8;
	return(((a->x*b->x)+(a->y*b->y)+(a->z*b->z)));
}
#endif // UNUSED

#if defined(UNUSED)
/* =======================================================================
   Function    - unit_normal
   Description - returns a unit normal to a vector
   Returns     - the unit normal
   ======================================================================== */
POINT3D unit_normal(POINT3D *a,POINT3D *b)
{
	POINT3D c;
	c.x=b->x-a->x;
	c.y=b->y-a->y;
	c.z=b->z-a->z;
	return(c);
}
#endif // UNUSED

/* ========================================================================
   Function    - AngleFromPoint
   Description - Return the angle of a point relative to another point
   Returns     - a value between 0 and 255 in Resolution increments
   ======================================================================== */
LONG AngleFromPoint ( LONG X1, LONG Y1, LONG X2, LONG Y2, LONG Resolution )
{
	LONG	RetVal=0, ShiftMask, TestBit, i;
	LONG	XDelta, YDelta;
	LONG	Quad, Oct, XFact, YFact;

	XDelta = X2 - X1;
	YDelta = Y2 - Y1;

	//  Quads:
	//     x y |
	//   0 -,+ | +,+ 1
	//         |
	//    -----+-----
	//         |
	//   2 -,- | +,- 3
	//         |
	Quad = 0;
	if( XDelta >= 0 )
		Quad = 1;
	if( YDelta < 0 )
		Quad += 2;

	XDelta = abs( XDelta );
	YDelta = abs( YDelta );

	RetVal = 0;
	XFact = YFact = 1;
	Oct = (XDelta > YDelta) ? 1 : 0;

	for( i = 0; i < Resolution; ++i)
	{
		ShiftMask = 0x20 >> i;

		// set bit
		TestBit = ((XDelta * XFact) >= (YDelta * YFact));
		if(TestBit)
			RetVal |= ShiftMask;

		// fix factors for next test
		if( Oct )
		{
 			if( TestBit )
			{
				YFact*=2;
			}
			else
			{
				YFact*=2;
				XFact+=2;
			}
		}
		else
		{
			if( TestBit )
			{
				XFact*=2;
				YFact+=2;
			}
			else
			{
				XFact*=2;
			}
		}
	}

	switch( Quad )
	{
	//  Quads:
	//     x y |
	//   0 -,+ | +,+ 1
	//         |
	//    -----+-----
	//         |
	//   2 -,- | +,- 3
	//         |
	case 0:
		RetVal = 256 - RetVal;
		break;
	case 1:	// already correct
		break;
	case 2:
		RetVal = 128 + RetVal;
		break;
	case 3:
		RetVal = 128 - RetVal;
		break;
	}

	return RetVal;
}

/* ========================================================================
   Function    - AngleFromPoint2
   Description - Return the angle of a point relative to another point
   Returns     - a value between 0 and 255
   ======================================================================== */
LONG AngleFromPoint2 ( LONG X1, LONG Y1, LONG X2, LONG Y2, LONG Resolution)
{
	LONG	XDelta, YDelta, tmp;
	LONG  Quad;
	LONG  RetVal;

	XDelta = X2 - X1;
	YDelta = Y2 - Y1;

	//  Quads:
	//     x y |
	//   0 -,+ | +,+ 1
	//         |
	//    -----+-----
	//         |
	//   2 -,- | +,- 3
	//         |
	Quad = 0;
	if( XDelta >= 0 )
		Quad = 1;
	if( YDelta < 0 )
		Quad += 2;

	XDelta = abs( XDelta );
	YDelta = abs( YDelta );
	
	if(XDelta == 0)
		RetVal = 64;
	else
	{
		for(RetVal=0; RetVal<64; ++RetVal)
		{
			tmp = YDelta * ANGLE_MULTI / XDelta;
			if(tmp <= tan_table[RetVal])
				break;
		}
	}
	
	RetVal = 64 - RetVal;
	switch( Quad )
	{
	//  Quads:
	//     x y |
	//   0 -,+ | +,+ 1
	//         |
	//    -----+-----
	//         |
	//   2 -,- | +,- 3
	//         |
	case 0:
		RetVal = 256 - RetVal;
		break;
	case 1:	// already correct
		break;
	case 2:
		RetVal = 128 + RetVal;
		break;
	case 3:
		RetVal = 128 - RetVal;
		break;
	}

	return RetVal;
}


/* ========================================================================
   Function    - RelativeAngle
   Description - Return the differance between one angle and another
   Returns     - 0 to 255 :   0 means angle coinside,
                             64 means angles facing right
                            127 means angles opposite
                            191 means angles facing left
                            255 means angles almost coinside
                            etc...
   ======================================================================== */
LONG RelativeAngle( LONG Angle1, LONG Angle2 )
{
	LONG delta = Angle2 - Angle1;

	if( delta < 0 )
		delta += 255;

	return delta;
}


// ---------------------------------------------------------------------------
void RotateAnglePoint (POINT *p, POINT *origin, ULONG a)
{
// Rotate a point "p" around another point "origin" by angle "a."
// From Foley & van Dam.
// Uses Cartesian coordinate system.  See notes in POINT: rot8 ().
// If the angle passed to this function is positive, then the point will
// move counterclockwise as viewed from above.
// ---------------------------------------------------------------------------

	LONG			oldx;


	oldx = p->x;

	a = a % ANGLE_STEPS;

   // Use a look-up table for SIN and COS.
	p->x = (p->x * cos_table[a] - p->y * sin_table[a] +
		origin->x * (1024 - cos_table[a]) + origin->y * sin_table[a]) / ANGLE_MULTI;

	p->y = (oldx * sin_table[a] + p->y * cos_table[a] +
		origin->y * (1024 - cos_table[a]) - origin->x * sin_table[a]) / ANGLE_MULTI;
}


/*	======================================================================== */


//sine, cosine and tan tables...

LONG cos_table[]={	1024,
	1024,
	1023,
	1021,
	1019,
	1016,
	1013,
	1009,
	1004,
	999,
	993,
	987,
	980,
	972,
	964,
	955,
	945,
	935,
	925,
	914,
	902,
	890,
	877,
	864,
	850,
	836,
	821,
	806,
	790,
	774,
	757,
	740,
	722,
	704,
	685,
	666,
	647,
	627,
	607,
	586,
	566,
	544,
	523,
	501,
	479,
	456,
	434,
	411,
	387,
	364,
	340,
	316,
	292,
	268,
	244,
	219,
	194,
	170,
	145,
	120,
	94,
	69,
	44,
	19,
	-5,
	-31,
	-56,
	-81,
	-106,
	-131,
	-156,
	-181,
	-206,
	-230,
	-255,
	-279,
	-303,
	-327,
	-351,
	-375,
	-398,
	-421,
	-444,
	-467,
	-489,
	-511,
	-533,
	-554,
	-575,
	-596,
	-616,
	-636,
	-656,
	-675,
	-694,
	-712,
	-730,
	-747,
	-764,
	-781,
	-797,
	-812,
	-827,
	-842,
	-856,
	-870,
	-883,
	-895,
	-907,
	-918,
	-929,
	-940,
	-949,
	-958,
	-967,
	-975,
	-982,
	-989,
	-995,
	-1001,
	-1006,
	-1010,
	-1014,
	-1017,
	-1019,
	-1021,
	-1022,
	-1023,
	-1023,
	-1023,
	-1022,
	-1020,
	-1018,
	-1015,
	-1012,
	-1008,
	-1003,
	-998,
	-992,
	-986,
	-979,
	-971,
	-963,
	-954,
	-944,
	-934,
	-924,
	-913,
	-901,
	-889,
	-876,
	-863,
	-849,
	-835,
	-820,
	-805,
	-789,
	-773,
	-756,
	-739,
	-721,
	-703,
	-684,
	-665,
	-646,
	-626,
	-606,
	-585,
	-565,
	-543,
	-522,
	-500,
	-478,
	-455,
	-433,
	-410,
	-386,
	-363,
	-339,
	-315,
	-291,
	-267,
	-243,
	-218,
	-193,
	-169,
	-144,
	-119,
	-93,
	-68,
	-43,
	-18,
	6,
	32,
	57,
	82,
	107,
	132,
	157,
	182,
	207,
	231,
	256,
	280,
	304,
	328,
	352,
	376,
	399,
	422,
	445,
	468,
	490,
	512,
	534,
	555,
	576,
	597,
	617,
	637,
	657,
	676,
	695,
	713,
	731,
	748,
	765,
	782,
	798,
	813,
	828,
	843,
	857,
	871,
	884,
	896,
	908,
	919,
	930,
	941,
	950,
	959,
	968,
	976,
	983,
	990,
	996,
	1002,
	1007,
	1011,
	1015,
	1018,
	1019,	// 1020,
	1021,	// 1022,
	1023,
	1024,
};

LONG sin_table[]={	0,
	25,
	50,
	75,
	100,	// 101,
	126,
	151,
	176,
	201,
	225,
	250,
	274,
	298,
	322,
	346,
	370,
	393,
	416,
	439,
	462,
	484,
	507,
	528,
	550,
	571,
	592,
	612,
	632,
	652,
	671,
	690,
	708,
	726,
	744,
	761,
	778,
	794,
	810,
	825,
	839,
	854,
	867,
	880,
	893,
	905,
	917,
	928,
	938,
	948,
	957,
	966,
	974,
	981,
	988,
	995,
	1000,
	1005,
	1010,
	1014,
	1017,
	1020,
	1022,
	1023,
	1024,
	1024,
	1024,
	1022,
	1021,
	1018,
	1015,
	1012,
	1008,
	1003,
	998,
	992,
	985,
	978,
	970,
	962,
	953,
	943,
	933,
	922,
	911,
	899,
	887,
	874,
	861,
	847,
	832,
	817,
	802,
	786,
	769,
	752,
	735,
	717,
	699,
	680,
	661,
	642,
	622,
	602,
	581,
	560,
	539,
	517,
	496,
	473,
	451,
	428,
	405,
	382,
	358,
	334,
	310,
	286,
	262,
	238,
	213,
	188,
	163,
	138,
	113,
	88,
	63,
	38,
	13,
	0,
	-24,
	-49,
	-75,
	-100,
	-125,
	-150,
	-175,
	-200,
	-224,
	-249,
	-273,
	-297,
	-321,
	-345,
	-369,
	-392,
	-415,
	-438,
	-461,
	-483,
	-506,
	-527,
	-549,
	-570,
	-591,
	-611,
	-631,
	-651,
	-670,
	-689,
	-707,
	-725,
	-743,
	-760,
	-777,
	-793,
	-809,
	-824,
	-838,
	-853,
	-866,
	-879,
	-892,
	-904,
	-916,
	-927,
	-937,
	-947,
	-956,
	-965,
	-973,
	-980,
	-987,
	-994,
	-999,
	-1004,
	-1009,
	-1013,
	-1016,
	-1019,
	-1021,
	-1022,
	-1023,
	-1023,
	-1023,
	-1021,
	-1020,
	-1017,
	-1014,
	-1011,
	-1007,
	-1002,
	-997,
	-991,
	-984,
	-977,
	-969,
	-961,
	-952,
	-942,
	-932,
	-921,
	-910,
	-898,
	-886,
	-873,
	-860,
	-846,
	-831,
	-816,
	-801,
	-785,
	-768,
	-751,
	-734,
	-716,
	-698,
	-679,
	-660,
	-641,
	-621,
	-601,
	-580,
	-559,
	-538,
	-516,
	-495,
	-472,
	-450,
	-427,
	-404,
	-381,
	-357,
	-333,
	-309,
	-285,
	-261,
	-237,
	-212,
	-187,
	-162,
	-137,
	-112,
	-100,	// -87,
	-75,	// -62,
	-50,	// -37,
	-25	// -12,
};
LONG tan_table[]={
	0,
	25,
	50,
	75,
	100,
	126,
	151,
	177,
	203,
	229,
	256,
	283,
	310,
	338,
	366,
	395,
	424,
	453,
	484,
	515,
	547,
	580,
	613,
	648,
	684,
	721,
	759,
	799,
	840,
	883,
	928,
	974,
	1023,
	1075,
	1129,
	1187,
	1247,
	1312,
	1380,
	1453,
	1532,
	1617,
	1708,
	1807,
	1915,
	2034,
	2165,
	2310,
	2472,
	2654,
	2861,
	3099,
	3375,
	3700,
	4088,
	4560,
	5147,
	5901,
	6903,
	8302,
	10396,
	13882,
	20843,
	41713,
	-2147483647,
	-41713,
	-20844,
	-13882,
	-10396,
	-8302,
	-6903,
	-5901,
	-5147,
	-4560,
	-4088,
	-3700,
	-3375,
	-3099,
	-2861,
	-2654,
	-2472,
	-2310,
	-2165,
	-2034,
	-1915,
	-1807,
	-1708,
	-1617,
	-1532,
	-1453,
	-1380,
	-1312,
	-1247,
	-1187,
	-1129,
	-1075,
	-1024,
	-974,
	-928,
	-883,
	-840,
	-799,
	-759,
	-721,
	-684,
	-648,
	-613,
	-580,
	-547,
	-515,
	-484,
	-453,
	-424,
	-395,
	-366,
	-338,
	-310,
	-283,
	-256,
	-229,
	-203,
	-177,
	-151,
	-126,
	-100,
	-75,
	-50,
	-25,
	0,
	25,
	50,
	75,
	100,
	126,
	151,
	177,
	203,
	229,
	256,
	283,
	310,
	338,
	366,
	395,
	424,
	453,
	484,
	515,
	547,
	580,
	613,
	648,
	684,
	721,
	759,
	799,
	840,
	883,
	928,
	974,
	1023,
	1075,
	1129,
	1187,
	1247,
	1312,
	1380,
	1453,
	1532,
	1617,
	1708,
	1807,
	1915,
	2034,
	2165,
	2310,
	2472,
	2654,
	2861,
	3099,
	3375,
	3700,
	4088,
	4560,
	5147,
	5901,
	6903,
	8302,
	10396,
	13882,
	20843,
	41712,
	-2147483647,
	-41713,
	-20844,
	-13882,
	-10396,
	-8302,
	-6903,
	-5901,
	-5147,
	-4560,
	-4088,
	-3700,
	-3375,
	-3099,
	-2861,
	-2654,
	-2472,
	-2310,
	-2165,
	-2034,
	-1915,
	-1807,
	-1708,
	-1617,
	-1532,
	-1453,
	-1380,
	-1312,
	-1247,
	-1187,
	-1129,
	-1075,
	-1024,
	-974,
	-928,
	-883,
	-840,
	-799,
	-759,
	-721,
	-684,
	-648,
	-613,
	-580,
	-547,
	-515,
	-484,
	-453,
	-424,
	-395,
	-366,
	-338,
	-310,
	-283,
	-256,
	-229,
	-203,
	-177,
	-151,
	-126,
	-100,
	-75,
	-50,
	-25,
};




