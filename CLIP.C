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
#include "SYSTEM.H"

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
