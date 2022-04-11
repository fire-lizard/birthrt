/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: EXTENT.C    -For Handling Extents???
   Author: Chris Phillips
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:
   clear_extents         -resets the extent arrays
   mark_sect_visible     -marks a sector as visible from curr. location
   sect_visible          -is the sector visible?
   extent_visible        -???
   add_extent            -adds an extent to the list???
   fill_extent           -???
   clip_span			 -???
   clip_object			 -???


   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "engine.h"
#include "engint.h"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
		//REVISIT: Malloc or Zalloc arrays.
		//         Good cantidate for speed up.
		// 		  Make some func's macros??

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
LONG h_extent[MAX_VIEW_WIDTH];	// This is a LONG for speed of comparisions.
LONG bot_extent[MAX_VIEW_WIDTH];
LONG top_extent[MAX_VIEW_WIDTH];
LONG sector_visible[MAX_SECTORS];		// This is a LONG for speed of comparisions.


/* ========================================================================
   Function    - clear_extents
   Description - resets the extents array
   Returns     - void
   ======================================================================== */
void clear_extents()
{
	LONG i;
	memset(&h_extent[0],0,sizeof(h_extent));

	memset(&sector_visible[0],0,sizeof(sector_visible));

	for(i=0;i<MAX_VIEW_WIDTH;++i)
	{
		top_extent[i]=render_top;
		bot_extent[i]=render_bot;
	}
}

/* ========================================================================
   Function    - mark_sect_visible
   Description - marks a sector as visible from the current location
   Returns     - void
   ======================================================================== */
void mark_sect_visible(LONG s)
{
	sector_visible[s]=1;
}

/* ========================================================================
   Function    - sect_visible
   Description - is the sector visible?
   Returns     - the value of the sector_visible table (TRUE or FALSE)
   ======================================================================== */
LONG sect_visible(LONG s)
{
	return(sector_visible[s]);
}

/* ========================================================================
   Function    - extent_visible
   Description - ???
   Returns     - TRUE or FALSE
   ======================================================================== */
LONG extent_visible(LONG x1,LONG x2)
{
	if(x1>x2)
		SWAPINT(x1,x2);

	if (x1<0) x1=0;

	while(x1<x2)
		if(h_extent[x1++]==0)
			return(TRUE);
	return(FALSE);
}

/* ========================================================================
   Function    - add_extent
   Description - adds an extent to the list???
   Returns     - void
   ======================================================================== */
void add_extent(LONG x1,LONG x2)
{
	LONG i;
	if(x1>x2)
		SWAPINT(x1,x2);

	// sets it to some number other than zero (0x01010101 is fine.)
	memset(&h_extent[x1],1,(x2-x1)*sizeof(LONG));
	
	memset(&bot_extent[x1], 0, (x2-x1)*sizeof(LONG));
	
	i=x1;
	// while(x1<x2)
	// 	bot_extent[x1++]=0;
	while(i<x2)
		top_extent[i++]=9999;
}

/* ========================================================================
   Function    - fill_extent
   Description - ???
   Returns     - void
   ======================================================================== */
void fill_extent(LONG x)
{
	h_extent[x]=1;
	bot_extent[x]=0;
	top_extent[x]=9999;
}

#if 0
// Moved to RENDER.C
/* =======================================================================
   Function    - clip_span
   Description - ???
   Returns     - TRUE if something was left after clipping, FALSE if not.
   ======================================================================== */
LONG clip_span (LONG x, LONG* y1, LONG* y2, LONG* clipped)
{
#if 0
	LONG* top, * bot;
	LONG topVal, botVal;

	top=&top_extent[x];
	topVal = *top;
	bot=&bot_extent[x];
	botVal = *bot;

	if(*y1<=topVal)
	{
		*clipped=topVal-*y1;    /* amount clipped */
		*y1=topVal;   /*clip*/
	}
	else
	{
		*clipped=0;
	}
	
	if(*y2>=botVal)
		*y2=botVal;   /*clip*/

	if(*y1>=*y2)   /* is ne-thing left? */
		return(FALSE);							// no, span was clipped out of existence

	if(*y1<=topVal)
		*top=*y2+1;   /*abut (update clip spans)*/
		
	if(*y2>=botVal)
		*bot=*y1-1;   /*abut*/

	return(TRUE);								// something was left after clipping
#else
// [d7-02-96 JPC] Rewrote to get rid of the local pointers.

	LONG topVal, botVal;

	topVal = top_extent[x];
	botVal = bot_extent[x];

	if (*y1 <= topVal)
	{
		*clipped = topVal - *y1;			// amount clipped
		*y1 = topVal;   						// clip
	}
	else
	{
		*clipped=0;
	}
	
	if (*y2 >= botVal)
      *y2 = botVal;                    // clip

	if (*y1 >= *y2)   						// is anything left?
      return (FALSE);                  // no, span was clipped out of existence

	if (*y1 <= topVal)
      top_extent[x] = *y2 + 1;         // abut (update clip spans)
		
	if (*y2 >= botVal)
      bot_extent[x] = *y1 - 1;         // abut

	return(TRUE);								// something was left after clipping
#endif
}
#endif

/* =======================================================================
   Function    - clip_obj
   Description - clip_obj is the same as clip_span except top and bot
						extent are not updated.
   Returns     - TRUE or FALSE, depending on success or failure???
   ======================================================================== */
LONG clip_obj (LONG x, LONG* y1, LONG* y2, LONG* clipped)
{
	LONG topVal,botVal;

	topVal=top_extent[x];
	botVal=bot_extent[x];

	if(*y1<=topVal)
	{
		*clipped=topVal-*y1;    /* amount clipped */
		*y1=topVal;   /*clip*/
	}
	else
	{
		*clipped=0;
	}
	if(*y2>=botVal)
		*y2=botVal;   /*clip*/

	if(*y1>=*y2)   /* is anything left? */
		return(FALSE);
	return(TRUE);
}

