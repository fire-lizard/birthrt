/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: CLIP.C      -Handles clipping of lines???
   Author: Chris Phillips
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:
   clip_line             -Clips a line???
   lines_intersect       -Tests to see if two lines intersect

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
// #include <stdio.h>
#include "system.h"
/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
	//REVISIT: Could it be faster??
	//GEH Yes, intersect line is now faster

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define DONT_INTERSECT 0
#define DO_INTERSECT   1
#define COLLINEAR  1

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

/* =======================================================================
   Function    - clip_line
   Description - clips a line defined by the end points a,b.
                 This is the Cohn-Sutherland line clipping Algorithm a
                 description of which can be found in Folly & Van Dam section.
                 3.12.3 p113-117 Second Ed.
                 It does the clip for a line in Quadrant 1 only. So to use
                 this fn, call the rotate fns first on the points.
   Returns     - whether or not it it is visible. (ie intersects.)
   ======================================================================== */
LONG clip_line (POINT * a, POINT * b)
{
 LONG  x1, y1, x2, y2;
 LONG  code1, code2;
 LONG  t;
 LONG  retry = 0;

 x1 = a->x * lens_factor;
 y1 = a->y * NORMAL_LENS;
 x2 = b->x * lens_factor;
 y2 = b->y * NORMAL_LENS;

Retry:									//UGLY UGLY UGLY UGLY

 code1 = ((y1 < x1)<<1) + (y1 < -x1);
 code2 = ((y2 < x2)<<1) + (y2 < -x2);

 if (code1==0 && code2==0)     /* both inside */
  return (TRUE);

 if ((code1 & code2) || (y1<0 && y2<0)) /* both outside on same side */
  return (FALSE);

 if (code1 & 1)        /* push point a toward left edge */
  {
  t = (x1 - x2) - (y2 - y1);
  if (t==0) return (FALSE);
  y1 = ( ((y1 + x1) * (y2 - y1)) / t) + y1;
  x1 = -y1;
  }

 if (code1 & 2)        /* push point a toward right edge */
  {
  t = (x2 - x1) - (y2 - y1);
  if (t==0) return (FALSE);
  y1 = ( ((y1 - x1) * (y2 - y1)) / t) + y1;
  x1 = y1;
  }

 if (code2 & 1)        /* push point b toward left edge */
  {
  t = (x1 - x2) - (y2 - y1);
  if (t==0) return (FALSE);
  y2 = ( ((y1 + x1) * (y2 - y1)) / t) + y1;
  x2 = -y2;
  }

 if (code2 & 2)        /* push point b toward right edge */
  {
  t = (x2 - x1) - (y2 - y1);
  if (t==0) return (FALSE);
  y2 = ( ((y1 - x1) * (y2 - y1)) / t) + y1;
  x2 = y2;
  }

 if (y1 < 0 || y2 < 0)     /* if in negative viewspace, don't accept */
 {
	++retry;
	if (retry > 15)
		return (FALSE);
	goto Retry;
 }

 a->x = x1 / lens_factor;
 a->y = y1 / NORMAL_LENS;
 b->x = x2 / lens_factor;
 b->y = y2 / NORMAL_LENS;

 return (TRUE);
}

/* =======================================================================
   Function    - lines_intersect
   Description - Tests to see if the lines intersect
   Returns     - whether they intersect or not
   ======================================================================== */
#if 0
// JPC used slightly different algorithm from Sedgewick; see below.
LONG lines_intersect(LONG x1,LONG y1,LONG x2,LONG y2,LONG x3,LONG y3,LONG x4,LONG y4)
{

	LONG t1,t2,t3,t4;

   // [d5-02-96 JPC] Special case if line 1 is actually a point:
   if (x1 == x2 && y1 == y2)
   {
      return DONT_INTERSECT;
   }

	// this poly finds which side of a line a point is on.  by
	// testing each end of each line, we can tell if the two line
	// segments cross
	t1 = ((x1 - x3)*(y4 - y3)) - ((x4 - x3)*(y1 - y3));
	
	t2 = ((x2 - x3)*(y4 - y3)) - ((x4 - x3)*(y2 - y3));
	
	t3 = ((x3 - x1)*(y2 - y1)) - ((x2 - x1)*(y3 - y1));
	
	t4 = ((x4 - x1)*(y2 - y1)) - ((x2 - x1)*(y4 - y1));
	
	if (!t1 || !t2 || !t3 || !t4 )
		return DONT_INTERSECT;             // [d5-02-96 JPC] was DONT_INTERSECT, which
                                       // leads to an error if lines are collinear
   // But DO_INTERSECT leads to worse errors. Need a better test for collinearity.

	if (!SAME_SIGNS(t1,t2) && !SAME_SIGNS(t3,t4))
		return DO_INTERSECT;
		
	return( DONT_INTERSECT );
	
//GEH code above replaces code below
// Ax = x2-x1;
// Bx = x3-x4;
//
// if(Ax<0) {						/* X bound box test*/
//   x1lo=(SHORT)x2; x1hi=(SHORT)x1;
//   } else {
//   x1hi=(SHORT)x2; x1lo=(SHORT)x1;
//   }
// if(Bx>0) {
//   if(x1hi < (SHORT)x4 || (SHORT)x3 < x1lo) return DONT_INTERSECT;
//   } else {
//   if(x1hi < (SHORT)x3 || (SHORT)x4 < x1lo) return DONT_INTERSECT;
//   }
//
// Ay = y2-y1;
// By = y3-y4;
//
// if(Ay<0) {						/* Y bound box test*/
//   y1lo=(SHORT)y2; y1hi=(SHORT)y1;
//   } else {
//   y1hi=(SHORT)y2; y1lo=(SHORT)y1;
//   }
// if(By>0) {
//   if(y1hi < (SHORT)y4 || (SHORT)y3 < y1lo) return DONT_INTERSECT;
//   } else {
//   if(y1hi < (SHORT)y3 || (SHORT)y4 < y1lo) return DONT_INTERSECT;
//   }
//
//
// Cx = x1-x3;
// Cy = y1-y3;
// d = By*Cx - Bx*Cy;					/* alpha numerator*/
// f = Ay*Bx - Ax*By;					/* both denominator*/
// if(f>0) {						/* alpha tests*/
//   if(d<0 || d>f) return DONT_INTERSECT;
//   } else {
//   if(d>0 || d<f) return DONT_INTERSECT;
//   }
//
// e = Ax*Cy - Ay*Cx;					/* beta numerator*/
// if(f>0) {						/* beta tests*/
//   if(e<0 || e>f) return DONT_INTERSECT;
//   } else {
//   if(e>0 || e<f) return DONT_INTERSECT;
//   }
//
// if(f==0) return COLLINEAR;
//
// return DO_INTERSECT;
}
#endif







#if 0 // GWP UNUSED
int line_calc_intersect(LINE2D *l1,LINE2D *l2, POINT *p)
{

long Ax,Bx,Cx,Ay,By,Cy,d,e,f,num,offset;
long x1lo,x1hi,y1lo,y1hi;
long x1,y1,x2,y2,x3,y3,x4,y4;

x1=l1->a.x;y1=l1->a.y;
x2=l1->b.x;y2=l1->b.y;
x3=l2->a.x;y3=l2->a.y;
x4=l2->b.x;y4=l2->b.y;

Ax = x2-x1;
Bx = x3-x4;

if(Ax<0) {						/* X bound box test*/
  x1lo=(short)x2; x1hi=(short)x1;
  } else {
  x1hi=(short)x2; x1lo=(short)x1;
  }
if(Bx>0) {
  if(x1hi < (short)x4 || (short)x3 < x1lo) return DONT_INTERSECT;
  } else {
  if(x1hi < (short)x3 || (short)x4 < x1lo) return DONT_INTERSECT;
  }

Ay = y2-y1;
By = y3-y4;

if(Ay<0) {						/* Y bound box test*/
  y1lo=(short)y2; y1hi=(short)y1;
  } else {
  y1hi=(short)y2; y1lo=(short)y1;
  }
if(By>0) {
  if(y1hi < (short)y4 || (short)y3 < y1lo) return DONT_INTERSECT;
  } else {
  if(y1hi < (short)y3 || (short)y4 < y1lo) return DONT_INTERSECT;
  }


Cx = x1-x3;
Cy = y1-y3;
d = By*Cx - Bx*Cy;					/* alpha numerator*/
f = Ay*Bx - Ax*By;					/* both denominator*/
if(f>0) {						/* alpha tests*/
  if(d<0 || d>f) return DONT_INTERSECT;
  } else {
  if(d>0 || d<f) return DONT_INTERSECT;
  }

e = Ax*Cy - Ay*Cx;					/* beta numerator*/
if(f>0) {						/* beta tests*/
  if(e<0 || e>f) return DONT_INTERSECT;
  } else {
  if(e>0 || e<f) return DONT_INTERSECT;
  }

/*compute intersection coordinates*/

if(f==0) return COLLINEAR;
num = d*Ax;						/* numerator */
offset = SAME_SIGNS(num,f) ? f/2 : -f/2;		/* round direction*/
p->x = x1 + (num+offset) / f;				/* intersection x */

num = d*Ay;
offset = SAME_SIGNS(num,f) ? f/2 : -f/2;
p->y = y1 + (num+offset) / f;				/* intersection y */

return DO_INTERSECT;
}
#endif
