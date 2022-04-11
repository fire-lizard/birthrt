/* ========================================================================
   Copyright (c) 1990,1997	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: nlnclip.c  -
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "nlnclip.h"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
   
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
   
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Local Variables
   ------------------------------------------------------------------------ */

/* ========================================================================
   Function    - LeftSide
   Description - Given a point and a line segment determin whether the point
                 is to the left of the line segment.
   Returns     - TRUE if left, FALSE otherwise
   ======================================================================== */

static BOOL const LeftSide(
	POINT const * const Q,
	POINT const * const P,	// Beginning of line segment.
	LONG  const xMax,
	LONG  const yMax)
{
	BOOL isLeftSide = FALSE;
	
	if ((((Q->y - P->y) * (xMax - P->x)) - (yMax - P->y)* (Q->x - P->x)) > 0)
	{
		// Its positive therefore is to the left.
		isLeftSide = TRUE;
	}
	return isLeftSide;
}
	
/* ========================================================================
   Function    - Intersection
   Description - Given the two endpoints and a line segment,
                 Modify the end point at that line segment.
   Returns     - void
   ======================================================================== */

static void Intersection(
	POINT const P,
	POINT const Q,
	POINT * const NewP,
	LONG  const xMin,
	LONG  const yMin,
	LONG  const xMax,
	LONG  const yMax)
{
	// WARNING needs work.
	NewP->y = P.y + ((Q.y - P.y)*(xMax - P.x))/(Q.x - P.x);
	NewP->x = P.x + ((Q.x - P.x) * (yMax - P.y)) / (Q.y - P.y);
}

/* ========================================================================
   Function    - nlnclipline
   Description - Given a bounding box, and the endpoints of a line.
                 clip the points.
                 From Foley, van Dam et. al. "Computer Graphics Principles
                 and Practice." 2nd Ed. pp. 924-928 fig. 19.5
   Returns     - TRUE if visible. FALSE otherwise.
   ======================================================================== */

// TODO: (fire lizard) uncomment
/*BOOL const nlnclipline(
	POINT * const P,
	POINT * const Q,
	LONG  const xMin,
	LONG  const yMin,
	LONG  const xMax,
	LONG  const yMax)
{
	BOOL bVisible = TRUE;
	
	// First test where both P and Q are in the center region.
	// No clipping necessary.
	if (P->x >= xMin && P->x <= xMax &&
	    P->y >= yMin && P->y <= yMax &&
	    Q->x >= xMin && Q->x <= xMax &&
	    Q->y >= yMin && Q->y <= yMax)
	{
		bVisible = TRUE;
	}
	else if (P->x < xMin) 
	{
		if (P->y < yMin)
		{
			// Where P is in the lower left corner of the region.
			if (Q->y < yMin || 
				Q->x < xMin
			   )
			{
				bVisible = FALSE;
			}
			else if (LeftSide(Q, ray from P to lower left corner.)
			{
				// Region L or LR.
				if (Q->y <= yMax)
				{
					bVisible = TRUE;
					Intersection(*P, *Q, P, Left edge of Clip Region);
					if (Q->x > xMax)
					{
					 	// region LR
					 	Intersection(*P, *Q, Q, RightEdge of Clip region);
					}
				}
				else
				{
					// Above top.
					if (LeftSide(Q, ray from P to upper left corner.)
					{
						bVisible = FALSE;
					}
					else if (Q->x < xMax)
					{
						// First region LT
						bVisible = TRUE;
						Intersection(*P, *Q, P, left edge of clip region);
						Intersection(*P, *Q, Q, top edoge of clip region);
					}
					else if (LeftSide(Q, ray fro mP to upper right corner)
					{
						// region LT
						bVisible = TRUE;
						Intersection(*P, *Q, P, left edge of clip region);
						Intersection(*P, *Q, Q, top edge of clip region);
					}
					else
					{
						// region LR
						bVisible = TRUE;
						Intersection(*P, *Q, P, left edge of clip region);
						Intersection(*P, *Q, Q, right edge of clip region);
					}
				}
			}
			else // Q is to the right of line from P to lower-left corner.
			{
			}
		}
		if (P->y > yMax)
		{
			// Upper left corner
		}
		else
		{
			// center left side.
		}
	}
	else if (P->x < xMax)  // &&P->x > xMin
	{
		// P is in the center column
		if (P->y < yMin)
		{
			// P is center lower box
		}
		else if (p > yMax)
		{
			// P is center top box
		}
		else
		{
			// P is in the center box
			bVisible = TRUE;
		}
	}
	else
	{
		// P is to the right of the view box.
	}

	return bVisible;
}*/
