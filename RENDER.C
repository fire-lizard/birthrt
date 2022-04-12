/* ®RM255¯ */
/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: RENDER.C   -Core of the graphics engine
   Author: Chris Phillips
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:
   render_view          -renders players view
   draw_node            -???
   draw_ssector         -???
   process_seg          -???
   calc_x_texture_info  -???
   draw_wall            -draws a wall
   draw_lower_wall      -draws a lower wall
   draw_upper_wall      -draws an upper wall
   draw_backface        -???
   add_floor_span       -???
   add_ceil_span        -???

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _EDIT
	#if !defined(_WINDOWS)
		#error _EDIT must be combined with _WINDOWS
	#endif
	#include <windows.h>
	#include "edit.h"
#endif
#include "DEBUG.H"
#include "SYSTEM.H"
#include "ENGINE.H"
#include "ENGINT.H"
#include "MACHINE.H"
#include "DYNAMTEX.H"
#include "LIGHT.H"
#include "SOUND.HXX"
#include "SPECIAL.H"
#include "COLORS.H"

extern void scan_line(POINT* a, POINT* b, LONG* array);
extern LONG clip_line(POINT* a, POINT* b);
extern UBYTE				shade_table[];

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

	// Linedef Flags:
	//		1	- Impassable
	//		8	- peg to ceiling
	//		16	- peg to floor


/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

#define SEE_THROUGH			01
#define NS_EW_DIFF			5
#define LIGHT_DELTA_MULTIPLY	2			// multiply sidedef lighting delta by this
#define ANIMATED_TEXTURES	1
#define ANGLE_MULTI	(1024)

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
#define SetLightDelta(seg, side)\
   if (side == 0)\
      gLinedefLight = (linedefs[segs[seg].lptr].flags) >> SIDE_1_LIGHT_SHIFT;\
   else\
      gLinedefLight = (linedefs[segs[seg].lptr].flags) >> SIDE_2_LIGHT_SHIFT;\
   gLinedefLight &= LIGHT_VALUE_MASK;\
   if (gLinedefLight > LIGHT_MAX_POSITIVE)\
      gLinedefLight -= LIGHT_ADD_FACTOR;\
	gLinedefLight *= LIGHT_DELTA_MULTIPLY;

extern void Light_Ren(char *, char*, long, long, long, long, char*);
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
static LONG draw_wall(LONG seg, LONG side);
static LONG draw_lower_wall(LONG seg, LONG side);
static LONG draw_upper_wall(LONG seg, LONG side);
extern LONG view_sin,view_cos;
extern LONG h_extent[];	// This is a LONG for speed of comparisions.
extern LONG bot_extent[];
extern LONG top_extent[];

long local_camera_x,local_camera_y;
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
#if defined(_DEBUG) && defined(_WINDOWS) && defined(_JPC)
static char gszSourceFile[] = __FILE__;	// for ASSERT
#endif

LONG first=TRUE;
LONG level_first=TRUE;
LONG frames=0;
BOOL gfFindFirstSeg = TRUE;	// [d7-24-96 JPC]

BOOL mouse_scan = FALSE;	// GEH

// [d7-11-96 JPC] Made wall_scale static.
static LONG wall_scale = 0;

// JPC: The following variables are only used in this file.
// Notes on "front_sect" and "back_sect."
// For one-sided linedefs, front_sect is the one and only sector.
// For two-sided linedefs, the meanings of front_sect and back_sect are
// a bit unintuitive.  In this case, for a given line segment,
// front_sect seems to be the sector that is further away from us, and
// back_sect the sector that is closer to us.  front_sect is not
// necessarily the sector in "front" of the line, although it
// can be, and back_sect is not necessarily the sector in "back"
// of the line, although it can be.
static LONG front_sect;
static LONG back_sect;
static LONG flat_sect;		// sector containing floor or ceiling
static LONG current_seg;	// current line segment (index to the segs array)
// LONG current_light;

POINT orig_a,orig_b;
POINT clipped_a,clipped_b;
LONG proj_ax,proj_bx;
LONG col_hei;
LONG gPeggedValue;							// upper/lower/no pegging value
LONG floor_z;
LONG gtxoff;
LONG tyoff;

LONG print_one_pass=FALSE;

UBYTE * shade;
LONG	SectLight = 0;
LONG	NSEWLight = 0;
LONG  gLinedefLight; // [d6-04-96 JPC] Custom lighting for a linedef.
LONG  gAdjustLight;	// [d9-23-96 JPC] Lighting factor for special effects.
LONG  gMinLight;		// [d9-23-96 JPC] Minimum allowed value for light.
LONG  gMaxLight;		// [d9-23-96 JPC] Maximum allowed value for light.
PTR	pTexture;
LONG	wTexture;
LONG	hTexture;

#if 0
extern BOOL bShowBanner;
extern SHORT testbitm;
#endif

extern TEXTURE textures[];

extern ULONG		tot_nodes;				// [d8-02-96 JPC] used to be in ENGINT.H,
													// but mysteriously disappeared

	/*used by floors and ceils*/
LONG floor_need_start;
LONG last_post_x,last_post_y,last_post_ye;
ULONG floor_lastt_y;
ULONG floor_lastb_y;

	/*used to open doors and such like. this needs a better mechanism*/
LONG first_seg;

#if defined(_DEBUG) && defined(_JPC)
// Comment-out the following to suppress various TRACE statements:
// #define _SHOWLINES
//int giLinedef = 2294;             		// JPC: for breakpoint
int giLinedef = 50;             		// JPC: for breakpoint
int gDebug;                     		// JPC: for breakpoint
#endif

// short gCameraSector;						// [d2-28-97 JPC] Was used for reject
													// table processing in draw_node,
													// which is no longer used.

#if defined (_DEBUGNODE)
// [d3-03-97 JPC] Variables for testing IsNodeVisible.
int gDepth = 0;
int gMax = 0;
#define DEBUG_NODES 2000
char gNodeHit[DEBUG_NODES];
#endif


#define DEBUG_SCALE	(10)
void debug_draw_seg_line(long x1,long y1,long x2,long y2,long c)
{
	x1-=camera.x>>8;
	x2-=camera.x>>8;
	y1-=camera.y>>8;
	y2-=camera.y>>8;
   x1=x1/DEBUG_SCALE;
   y1=y1/DEBUG_SCALE;
   x2=x2/DEBUG_SCALE;
   y2=y2/DEBUG_SCALE;
   line(x1+320,y1+240,x2+320,y2+240,c);

}

void scale_col_ttop(long sx,long dx,long dy,long dye,
				long clipped,long src_inc)
{
	register FIXED16	tsy;
	register PTR		sptr;
	register PTR		tptr;
	register PTR		sptr_end;
	register LONG		sptr_inc;
	LONG				light;

#if defined(BIG_DEBUG)
	// You just sent in bad data. I don't know what to do with it but if I keep
	// going here we will blow up. GWP.
	if (dx < 0 || dy < 0)
		return;
#endif

//dye=dy;

#if !defined(_WINDOWS)
	if (fAIMoving)
	{
		//dx=dx<<1;
	    if (dx & 1)
	 	 	return;									// skip every other column
 	}
#endif
	tsy.lval = (tyoff << 16) + (clipped * src_inc);

	++dye;

	light = NSEWLight + SectLight + gLinedefLight +  // [d6-05-96 JPC] added gLinedefLight
		gAdjustLight;											 // [d9-23-96 JPC] added gAdjustLight
	if (SectLight)			/* if not brightest then darken */
	{
		/* shade with distance */
		light = light + (src_inc >> DARKEN_FACTOR);
	}
	if (light < gMinLight)
		light = gMinLight;
	else if (light > gMaxLight)
		light = gMaxLight;
	SetLight (light);

#if WRC_CAMERA_TEST
	if (VideoCurrentlyRedirected)
	{
		sptr = (PTR)screen+((dx*screen_buffer_height)+dy);
		sptr_end = sptr + (dye-dy);
		sptr_inc = 1;
	}
	else
#endif
	{
		sptr = (PTR)screen+((dy*screen_buffer_width)+dx);
		sptr_end = sptr + ((dye-dy)*screen_buffer_width);
		sptr_inc = screen_buffer_width;
	}

	// JPC notes.
	// "sptr" is a pointer to the screen (or offscreen buffer).
	// "sx" seems to be the x offset into the wall texture graphic.
	// We mod it with hTexture instead of wTexture because the wall textures
	// are rotated 90 degrees for speed.  (We are drawing one column at a
	// time, not one scanline, so we want to get to the next y by a simple
	// increment.  Rotating the texture graphic 90 degrees lets us do this.)
	// Add gtxOff here (this is the x offset specified by the sidedef).
	sx = (sx + gtxoff) % hTexture;		/* insure horiz wrapping */
	tptr = pTexture + (sx * wTexture);

#ifdef _DEBUG
#ifdef _JPC
	if (floor_z % wTexture)
		++gDebug;
#endif
#endif

	// At this point, "tptr" points to the start of the texture row we'll use.
	// Remember that a row in the source texture becomes a column on screen.
	// "tsy" is the y offset into the texture graphic in fixed-point 16:16.


	// OLD: Check pegging.  Note that it's possible to set both upper peg and
	// lower peg in DCK, but they are mutually exclusive in BIRTHRIGHT.
	// If both are set, the result is treated as lower peg.
	//
	// NEW [d6-19-96 JPC]:
	// Upper and lower pegging are now independent, with upper pegging
	// only affecting the upper texture and lower pegging only affecting
	// the lower texture.  This is handled in the caller routines, draw_wall
	// and so on in RENDER.C, which know what kind of texture they're dealing
	// with.
	//
#if 0
|	if (gPeggedValue == 0)	 				// "unpegged" = peg to ground-zero
|	{
|		tsy.lval += ((wTexture - (floor_z % wTexture)) << 16);
|	}
|	else if (gPeggedValue & 16)         // "peg low" = peg to floor
|	{
|		tsy.lval += ((wTexture - (col_hei % wTexture)) << 16);
|	}
|	else if (gPeggedValue & 8)          // "peg high" = peg to ceiling
|	{
|		// in this case, do nothing--leave tsy as is
|	}
#endif
	if (gPeggedValue == 0)	 				// "unpegged" = peg to ground-zero
	{
		tsy.lval += ((wTexture - (floor_z % wTexture)) << 16);
	}
	else if (gPeggedValue & 16)         // "peg low"
	{
		// in this case, do nothing--leave tsy as is
	}
	else if (gPeggedValue & 8)          // "peg high"
	{
		tsy.lval += ((wTexture - (col_hei % wTexture)) << 16);
	}

#if 0
|	if (gPeggedValue != 0)
|	{
|		tsy.lval += ((wTexture - (col_hei % wTexture)) << 16);
|	}
#endif

	// GWP gMaxLight is actually the maximum index into the shade table which
	//     is all BLACK.
	if (light != gMaxLight)
	{
		LONG const wrapMask = (wTexture << 16) - 1;		/* make texture wrap mask */
		
		tsy.lval &= wrapMask;							/* texture wraps at wrapMask */
		
#if !defined(_WINDOWS)
		if (fAIMoving)
		{
			// GWP For DOS Low res draw every other scan line the same for walls.
			if (light == 0)
			{
				#if 0
				// Special optimized assembly routine to do this loop.
				Light_Ren(sptr, tptr, wrapMask, tsy.lval, src_inc, sptr_inc, sptr_end);
				#else
				do {
					// Use just the integer part of tsy to index into the texture
					// tptr.  (tsy is a fixed point integer.)
					// If we are moving only call this fn 1/2 as many times, so we
					// duplicate this pixel in the next pixel over to the right.
					*(sptr+1)=*sptr = tptr[tsy.TwoHalves.sHigh];
					
					// Increment to the next pixel from the texture.
					tsy.lval = ((tsy.lval+src_inc) & wrapMask);
					
					// advance the offscreen buffer to the next location below this pixal.
					sptr += sptr_inc;
				} while (sptr < sptr_end);
				#endif
			}
			else
			{
				do {
					// Sames as above, except look up the color of the pixel through
					// the shade table. (Makes things look dark at a distance.)
					*(sptr+1)=*sptr = shade[ tptr[tsy.TwoHalves.sHigh] ];
					tsy.lval = ((tsy.lval+src_inc) & wrapMask);
					sptr += sptr_inc;
				} while (sptr < sptr_end);
			}
		}
		else
#endif
		{
			if (light == 0)
			{
				do {
					*sptr = tptr[tsy.TwoHalves.sHigh];
					tsy.lval = ((tsy.lval+src_inc) & wrapMask);
					sptr += sptr_inc;
				} while (sptr < sptr_end);
			}
			else
			{
				do {
					*sptr = shade[ tptr[tsy.TwoHalves.sHigh] ];
					tsy.lval = ((tsy.lval+src_inc) & wrapMask);
					sptr += sptr_inc;
				} while (sptr < sptr_end);
			}
		}
	}
	else
	{
#if !defined(_WINDOWS)
		if (fAIMoving)
		{
			do
			{
				*(sptr+1) = *sptr = BLACK;
				sptr += sptr_inc;
			} while (sptr < sptr_end);
		}
		else
#endif
		{
			do
			{
				*sptr = BLACK;
				sptr += sptr_inc;
			} while (sptr < sptr_end);
		}
	}
}





// FKA local_clip_span.  The original clip_span was in EXTENT.C
LONG clip_span (LONG x, LONG* y1, LONG* y2, LONG* clipped)
{
#if 1
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







#if ANIMATED_TEXTURES
/* =======================================================================
   Function    - ChangeTextureFrame
   Description - helper routine of render_view (for animated textures)
   Returns     - void
   ======================================================================== */
void ChangeTextureFrame (int iTexture)
{
// [d10-08-96 JPC]
// Animated textures use the frameHandle field to access 4 animation
// frames, stored as follows:
//		0		1
//		2		3
//
// In this function, we copy one of the frames stored in frameHandle to
// textures[iTexture].t.  The engine then renders the frame normally.
// ---------------------------------------------------------------------------

	int		x, y, w, h;
	UBYTE *	pT;
	UBYTE *	pFrame;
	BITMPTR	pTex;
	TEXTURE	* const pTexture = &textures[iTexture];

	if (pTexture->t < 0)
		return;									// [d12-04-96 JPC] invalid t

	w = pTexture->w;
	h = pTexture->h;

	pTex = (BITMPTR) BLKPTR((SHORT)pTexture->t);
	pT = (UBYTE*)pTex + sizeof(BITMHDR);

	x = w * (pTexture->iFrame % 2);
	y = h * (pTexture->iFrame / 2);
	pTex = (BITMPTR) BLKPTR((SHORT)pTexture->frameHandle);
	pFrame = (UBYTE*)pTex + sizeof(BITMHDR) + x + y * w * 2;

	do
	{
		memcpy (pT, pFrame, w);
		pT += w;
		pFrame += w * 2;
	} while (--h);

	// Update the frame counter.
	++(pTexture->iFrame);
    if (pTexture->iFrame > 3)
		pTexture->iFrame = 0;
}
#endif

/* =======================================================================
   Function    - render_view
   Description - renders the players view
   Returns     - void
   ======================================================================== */
void render_view(LONG fskipmap)
{
	int			iTexture;					// [d10-08-96 JPC] for animated textures


#if ANIMATED_TEXTURES
	for (iTexture = 0; iTexture < last_texture; ++iTexture)
	{
		if (textures[iTexture].frameHandle > 0)
		{
			ChangeTextureFrame (iTexture);
		}
	}
#endif

#if WRC_CAMERA_TEST	
	
	RenderCameras();

#endif
	set_view();  //cmp now also sets frustrum.

    // These local globals are used in draw_node and process_seg.
	local_camera_x=CAMERA_INT_VAL(camera.x);
	local_camera_y=CAMERA_INT_VAL(camera.y);


	clear_extents();
	clear_spans();
	clear_thing_spans();

	//Clear out drawn array
	// [d10-14-96 JPC] Clear out array that keeps track of mirror segments.
	memset (segMirrorDrawn, 0, tot_segs * sizeof(SEGMIRROR_DRAWN));

	run_timers();
	
	first=TRUE;
	gfFindFirstSeg = TRUE;		// [d7-24-96 JPC]


	// draw_node is a recursive procedure that will draw all the normal walls.
	// It also records information about floors, ceilings, transparent
	// walls, and objects, but does not draw them.
	// [d2-28-97 JPC] Added gCameraSector so we can use the reject table.
	// [d3-04-97 JPC] Took gCameraSector out because we aren't using the
	// reject table anymore.
	// gCameraSector = point_to_sector (local_camera_x, local_camera_y);
	draw_node(tot_nodes-1);

#if 0
debug_draw_seg_line(view_frust_left.a.x,view_frust_left.a.y,
		view_frust_left.b.x,view_frust_left.b.y,55);
debug_draw_seg_line(view_frust_right.a.x,view_frust_right.a.y,
		view_frust_right.b.x,view_frust_right.b.y,55);

#endif
	run_timers();
	
	// Draw the floors and ceilings, using information recorded in draw_node.
	draw_spans();

	//run_timers();
	
	// Draw objects and "transparent" walls, using information recorded
	// in draw_node.
	draw_thing_spans();

	run_timers();
	
	if(MapIsActive() && !fskipmap)
	{
		LONG r,l,t,b;
		get_margin_size(&r,&l,&t,&b);
	
		if (fHighRes)
		{
			DrawMap(
				render_width+l-(render_width/2)-12,
		        render_height+t-(render_height/2)-12,
		        (render_width/2)+4,
		        (render_height/2)+4
			   );
		}
		else
		{
			DrawMap(
				render_width+l-(render_width/2)-6,
		        render_height+t-(render_height/2)-6,
		        (render_width/2)+2,
		        (render_height/2)+2
			   );
		}
	}

	++frames;
}




/* =======================================================================
   Function    - LineInFrustrum
   Description - Simplified version of clip_line that only does trivial rejection.
	              Theory: we can get a speed increase by avoiding the iterations
                 of the genuine clip_line.
                 Algorithm seems to be similar to the Cohen-Sutherland algorithm
                 discussed in Foley and van Dam et. al., except we are checking whether the
                 endpoints of a line are within the view frustrum instead of whether
                 the endpoints are within a square viewing window.
	Returns     - TRUE if line is in frustrum, FALSE if not.
	Note        - [d3-03-97 JPC]
   ======================================================================== */
BOOL LineInFrustrum (POINT a, POINT b)
{
	LONG  x1, y1, x2, y2;
	LONG  code1, code2;
	
	x1 = a.x * lens_factor;
	y1 = a.y * NORMAL_LENS;
	x2 = b.x * lens_factor;
	y2 = b.y * NORMAL_LENS;
	
	code1 = ((y1 < x1)<<1) + (y1 < -x1);
	code2 = ((y2 < x2)<<1) + (y2 < -x2);
	
	if ((code1 & code2) || (y1<0 && y2<0)) // both outside frustrum on same side
		return FALSE;

	return TRUE;
}


#if 0	// Not used (The clip is broken.)
/* =======================================================================
   Function    - IsNodeVisible
   Description - Helper of draw_node; uses bounding box and clipping to
	              decide whether a node is visible from our viewpoint.
   Returns     - TRUE if node is visible, FALSE if not.
	Note        - [d3-03-97 JPC]
   ======================================================================== */
BOOL IsNodeVisible (LONG iNode, LONG side)
{
	BR_NODE *	pNode = &nodes[iNode];
	POINT			a, b, c, d;

	// Get bounding box points and check bounding lines:
	//
	//	 c		d
	//
	//	 a		b

	if (side == FRONT)
	{
		// Translate.
		a.x = pNode->minxf - local_camera_x;
		a.y = pNode->minyf - local_camera_y;
		b.x = pNode->maxxf - local_camera_x;
		b.y = pNode->minyf - local_camera_y;
	}
	else
	{
		a.x = pNode->minxb - local_camera_x;
		a.y = pNode->minyb - local_camera_y;
		b.x = pNode->maxxb - local_camera_x;
		b.y = pNode->minyb - local_camera_y;
	}

	// Translate.
	// GWP Moved up to avoid extra assignments.
	// GWP a.x -= local_camera_x;
	// GWP a.y -= local_camera_y;
	// GWP b.x -= local_camera_x;
	// GWP b.y -= local_camera_y;

	// Rotate, using precalculated sine and cosine for camera angle.
	{
	// Use the braces to allocate a temporary variable.
	LONG ax;
	
	ax = a.x;
	a.x =(((((a.x) * view_cos) )
		 - (((a.y) * view_sin ) )) / ANGLE_MULTI);
	a.y =(((((ax) * view_sin ) )
		 + (((a.y) * view_cos ) )) / ANGLE_MULTI);

	ax = b.x;
	b.x =(((((b.x) * view_cos) )
		 - (((b.y) * view_sin ) )) / ANGLE_MULTI);
	b.y =(((((ax) * view_sin ) )
		 + (((b.y) * view_cos ) )) / ANGLE_MULTI);
	}

	// Use special LineInFrustrum routine to see whether this line is visible.
	if (LineInFrustrum (a, b))
		return TRUE;

	// Now we need to check line ac.
	if (side == FRONT)
	{
		c.x = pNode->minxf - local_camera_x;
		c.y = pNode->maxyf - local_camera_y;
	}
	else
	{
		c.x = pNode->minxb - local_camera_x;
		c.y = pNode->maxyb - local_camera_y;
	}
	// GWP c.x -= local_camera_x;
	// GWP c.y -= local_camera_y;
	{
	// Use the braces to allocate a temporary variable.
	LONG ax;
	
	ax = c.x;
	c.x =(((((c.x) * view_cos) )
		 - (((c.y) * view_sin ) )) / ANGLE_MULTI);
	c.y =(((((ax) * view_sin ) )
		 + (((c.y) * view_cos ) )) / ANGLE_MULTI);
	}
	
	if (LineInFrustrum (a, c))
		return TRUE;

	// Now we need to check line bd.
	if (side == FRONT)
	{
		d.x = pNode->maxxf - local_camera_x;
		d.y = pNode->maxyf - local_camera_y;
	}
	else
	{
		d.x = pNode->maxxb - local_camera_x;
		d.y = pNode->maxyb - local_camera_y;
	}
	// GWP d.x -= local_camera_x;
	// GWP d.y -= local_camera_y;
	{
	// Use the braces to allocate a temporary variable.
	LONG ax;
	
	ax = d.x;
	d.x =(((((d.x) * view_cos) )
		 - (((d.y) * view_sin ) )) / ANGLE_MULTI);
	d.y =(((((ax) * view_sin ) )
		 + (((d.y) * view_cos ) )) / ANGLE_MULTI);
	}
	if (LineInFrustrum (b, d))
		return TRUE;

	// Now we need to check line cd.
	if (LineInFrustrum (c, d))
		return TRUE;

	return FALSE;								// no line is in our view
}
#endif


/* =======================================================================
   Function    - draw_node
   Description -
	// draw_node is a recursive procedure that will draw all the normal walls.
	// It also records information about floors, ceilings, transparent
	// walls, and objects, but does not draw them.
   Returns     - void
   ======================================================================== */
#define MAX_NODE_STACK		200
typedef enum {
	EXIT_ROUTINE,
	FRONT_FIRST,
	POP_OUT,
	BACK_FIRST,
} NODE_RETURNS;
typedef struct {
	LONG	NodeId;
	LONG	NodeGoto;
} NODE_STACK;
void draw_node(LONG n)
{
	
	if(n&0x00008000)       /*if sector*/
	{
#if 01
		// Original code, not using reject table.
		draw_ssector(n&0x7fff);
		// GWP return;
#else
		// Reject table enhancement.  Turns out it slows things down if
		// we also do node visibility checking.  Node visibility checking
		// gives slightly better performance, so it will be left in and
		// reject table checking will be left out.
		LONG	HisSector;
		LONG	Index;
		ULONG	ubValue, ubCount, ubMask;

		// This code is from SECTORS.C: ssector_to_sector_info.
		LONG sa,sb,sx,sy;
		LONG sided;
		LONG const seg=ssectors[n&0x7fff].o;
		SEG * pSeg = &segs[seg];
		if(pSeg->flip)
		{
			sx=vertexs[pSeg->b].x;
			sy=vertexs[pSeg->b].y;
			sa=vertexs[pSeg->a].x-sx;
			sb=vertexs[pSeg->a].y-sy;
		}
		else
		{
			sx=vertexs[pSeg->a].x;
			sy=vertexs[pSeg->a].y;
			sa=vertexs[pSeg->b].x-sx;
			sb=vertexs[pSeg->b].y-sy;
		}
		
		{
		LONG const r= ( sa *( local_camera_y-sy ) ) - ( ( local_camera_x-sx) * sb );
		{
		LONG const l=(LONG)pSeg->lptr;
	
		if(r<0)
			sided=(LONG)linedefs[l].psdb;
		else
		{
			sided=(LONG)linedefs[l].psdt;
			// We're on the back side of a one sided line.
			if (sided == -1)
			{
				sided = (LONG) linedefs[l].psdb;
			}
		}
		}
		}
		
		HisSector = (LONG)sidedefs[sided].sec_ptr;
		// End of code from ssector_to_sector_info.

		// Following code taken from the "reject" function in REJECT.C.
		Index = gCameraSector * tot_sectors + HisSector;
		ubValue = rejects[(Index>>3)];
		ubCount = Index & 0x00000007;
		ubMask = 0x01 << ubCount;
		// in the reject table, a 1 means can't see and a 0 means can
		if ( !(ubValue & ubMask) )
		{
		// End of code from "reject" function.
			draw_ssector(n&0x7fff);
		}
		gDepth--;
		// GWP return;
#endif
	}
	else
	{
#if 1 // No Recursion.
		NODE_STACK NodeStack[MAX_NODE_STACK];
		NODE_STACK *pNodeStack;
		LONG nr;
		LONG nf;
		LONG NodeGoto;
		
		
		NodeStack[0].NodeId = 0; // marker for top of the stack.
		NodeStack[0].NodeGoto = EXIT_ROUTINE;
		pNodeStack = NodeStack + 1;
		
reTest:
		// [d3-03-97 JPC] Added code to check whether the node in question
		// is visible.  Don't draw it if not.  This can sidestep up to 95%
		// of the nodes, but only gives a modest increase in performance.
		if(point_relation(n,local_camera_x,local_camera_y)==FRONT)
		{
			//if (IsNodeVisible (n, FRONT))
				// GWP draw_node(nodes[n].f);
			// GWP Replace recursion.
			nf = nodes[n].f;
			if (nf & 0x00008000)
			{
				draw_ssector(nf&0x7fff);
			}	
			else
			{
				pNodeStack->NodeId = n; // push the stack.
				n = nf;
				pNodeStack->NodeGoto = FRONT_FIRST;
				++pNodeStack;
				goto reTest;
			}
			
reStartAtFrontFirst:
			//if (IsNodeVisible (n, BACK))
				// GWP draw_node(nodes[n].r);
			// GWP Replace recursion.
			nr = nodes[n].r;
			if (nr & 0x00008000)
			{
				draw_ssector(nr&0x7fff);
			}	
			else
			{
				pNodeStack->NodeId = n; // push the stack.
				n = nr;
				pNodeStack->NodeGoto = POP_OUT;
				++pNodeStack;
				goto reTest;
			}
		}
		else
		{
			//if (IsNodeVisible (n, BACK))
				//draw_node(nodes[n].r);
			// GWP Replace recursion.
			nr = nodes[n].r;
			if (nr & 0x00008000)
			{
				draw_ssector(nr&0x7fff);
			}	
			else
			{
				pNodeStack->NodeId = n; // push the stack.
				n = nr;
				pNodeStack->NodeGoto = BACK_FIRST;
				++pNodeStack;
				goto reTest;
			}
			
reStartAtBackFirst:
			//if (IsNodeVisible (n, FRONT))
				//draw_node(nodes[n].f);
			// GWP Replace recursion.
			nf = nodes[n].f;
			if (nf & 0x00008000)
			{
				draw_ssector(nf&0x7fff);
			}	
			else
			{
				pNodeStack->NodeId = n; // push the stack.
				n = nf;
				pNodeStack->NodeGoto = POP_OUT;
				++pNodeStack;
				goto reTest;
			}
		}
reStartAtPopOut:
		--pNodeStack;	// Pop the stack.
		NodeGoto = pNodeStack->NodeGoto;
		
		if (NodeGoto == EXIT_ROUTINE)
			return;
		
		n = pNodeStack->NodeId;
		if (NodeGoto == POP_OUT)
			goto reStartAtPopOut;
		if (NodeGoto == FRONT_FIRST)
			goto reStartAtFrontFirst;
		if (NodeGoto == BACK_FIRST)
			goto reStartAtBackFirst;
#else
		if(point_relation(n,local_camera_x,local_camera_y)==FRONT)
		{
			//if (IsNodeVisible (n, FRONT))
				draw_node(nodes[n].f);
			//if (IsNodeVisible (n, BACK))
				draw_node(nodes[n].r);
		}
		else
		{
			//if (IsNodeVisible (n, BACK))
				draw_node(nodes[n].r);
			//if (IsNodeVisible (n, FRONT))
				draw_node(nodes[n].f);
		}
#endif
	}
}

/* =======================================================================
   Function    - draw_ssector
   Description - ???
   Returns     - void
   ======================================================================== */
void draw_ssector(LONG sect)
{
// JPC note: An ssector array element contains the starting line segment
// and the count of segments for a given subsector.
// The path from a subsector to a sector is surprisingly long.
// You can get the linedef for the subsector's .o line segment by looking at
// segs[ssectors[sect].o].lptr.  You can use the linedef to get sidedefs, and
// use one of the sidedefs to get the sector.  We do this in process_seg, below.
// If there is more than one sidedef, figuring out which sidedef to use is
// tricky and I'm not certain we're doing it correctly in every case.
// ---------------------------------------------------------------------------

	ULONG i;
	ULONG LastSsector;

	if(sect>=(LONG)tot_ssectors)
#ifdef _DEBUG
		fatal_error("exceeded ssectors");
#else
		return;
#endif

	draw_things(sect);

	LastSsector = (ULONG) ssectors[sect].n;
	
	for(i=0;i<LastSsector;++i)
	{
		// [d10-21-96 JPC] Add segMirror code.
		// Call process_seg only if the seg has no mirror OR the seg's
		// mirror has not yet been drawn.
#if defined (_JPC)
		ASSERT (ssectors[sect].o + i < tot_segs);
#endif
		// GWP & GEH if (segMirror[ssectors[sect].o+i] == -1 ||
		// GWP & GEH 	segMirrorDrawn[segMirror[ssectors[sect].o+i]] == 0)
		// This test doesn't protect from a -1 array index in segMirrorDrawn.
		
		// GWP added these temp variables for speed optimization.
		LONG const Temp_ssector = ssectors[sect].o+i;
		LONG const Temp_mirror_seg = segMirror[Temp_ssector];
		
		// GWP if (segMirror[ssectors[sect].o+i] == -1)
		// GWP {
		// GWP 	process_seg(ssectors[sect].o+i);
		// GWP }
		// GWP else if (segMirrorDrawn[segMirror[ssectors[sect].o+i]] == 0)
		// GWP {
		// GWP 	process_seg(ssectors[sect].o+i);
		// GWP 	segMirrorDrawn[ssectors[sect].o+i]=TRUE;
		// GWP 	segMirrorDrawn[segMirror[ssectors[sect].o+i]]=TRUE;
		// GWP }
		
		if (Temp_mirror_seg == -1)
		{
			process_seg(Temp_ssector);
		}
		else if (segMirrorDrawn[Temp_mirror_seg] == 0)
		{
			process_seg(Temp_ssector);
			segMirrorDrawn[Temp_ssector]=TRUE;
			segMirrorDrawn[Temp_mirror_seg]=TRUE;
		}
	}
}




// #define ANGLE_MULTI	(1024)

/* =======================================================================
   Function    - process_seg
   Description - ???
   Returns     - void
   ======================================================================== */
void process_seg(LONG iSegment)
{
// Parameter iSegment is an index to the segs (line segments) array.
// ---------------------------------------------------------------------------

	POINT	a,b;
	LONG	side;
	LONG	segFront, segBack;

	LONG const iLinedef = segs[iSegment].lptr;                // [d6-10-96 JPC] move here


	/*If this is the not first sector drawn see if an adj sector was
	  drawn. If it was this sector MIGHT be visible... if not it IS NOT
	  visible*/

	//NOTE: seg_to_sector needs unrolling.  for each frame drawn,
	// process_seg is called ~3500 times! that means seg_to_sector
	// was called about 7000 times per frame!  I have unrolled several
	// such routines, look for GEH
	segFront = sidedefs[linedefs[iLinedef].psdb].sec_ptr;
	// [d10-02-96 JPC] Don't calculate sectBack until we're sure there
   // is a side 2.
	//segBack = sidedefs[linedefs[iLinedef].psdt].sec_ptr;

	if (!first)
	{
		//GEH if(seg_two_sided(iSegment))
		if(linedefs[iLinedef].psdt!=-1)
		{
			// SECT_VISIBLE is a macro defined as follows in ENGINT.H:
			// 	#define SECT_VISIBLE(x)	(sector_visible[x])
			if(!SECT_VISIBLE(segFront)  )
				if(!SECT_VISIBLE(sidedefs[linedefs[iLinedef].psdt].sec_ptr)  )
				{
					return;
				}
		}
		else										// one-sided
			if(!SECT_VISIBLE(segFront)  )
			{
				return;
			}
	}

   // [d7-24-96 JPC] The following test was "if (first)" but that often set
   // first_seg to the wrong segment.
	if (gfFindFirstSeg || first)		
	{
		if (linedefs[segs[iSegment].lptr].special != 0)
		{
			gfFindFirstSeg = FALSE;
			first_seg=iSegment;
		}
	}


   //cmp maybe use use memcpy for this( GWP the structures are now the same size).
	// GWP a.x=vertexs[segs[iSegment].a].x;
	// GWP a.y=vertexs[segs[iSegment].a].y;
	// GWP b.x=vertexs[segs[iSegment].b].x;
	// GWP b.y=vertexs[segs[iSegment].b].y;
	a=vertexs[segs[iSegment].a];
	b=vertexs[segs[iSegment].b];


//	if( line_clip_to_frust(&temp_line) )
//	   debug_draw_seg_line(a.x,a.y,b.x,b.y,85);


   	
      //CMP clips the seg against the view frustrum and
      //(maybe)front clip plane.


//59fps
	if(segs[iSegment].flip)
		side=back_face_point(&b,&a,local_camera_x,local_camera_y);
	else
		side=back_face_point(&a,&b,local_camera_x,local_camera_y);

	/* set north/south walls slightly darker */
	NSEWLight = NS_EW_DIFF * (abs(a.x-b.x)>abs(a.y-b.y));

//52fps


		//xlate.
	a.x=a.x-local_camera_x;
	a.y=a.y-local_camera_y;
	b.x=b.x-local_camera_x;
	b.y=b.y-local_camera_y;
		//rotate
	{
	// Use the braces to allocate a temporary variable.
	LONG ax;
	
	ax = a.x;
	/* calculate SIN and COS using a look-up table */
	a.x =(((((a.x) * view_cos) )
		 - (((a.y) * view_sin ) )) / ANGLE_MULTI);
	a.y =(((((ax) * view_sin ) )
		 + (((a.y) * view_cos ) )) / ANGLE_MULTI);

	ax = b.x;
	/* calculate SIN and COS using a look-up table */
	b.x =(((((b.x) * view_cos) )
		 - (((b.y) * view_sin ) )) / ANGLE_MULTI);
	b.y =(((((ax) * view_sin ) )
		 + (((b.y) * view_cos ) )) / ANGLE_MULTI);
	}

//48 fps	 utb 40fps

	orig_a=a;
	orig_b=b;
	if(clip_line(&a,&b)==0)
	{
		// Clear first_seg if it turns out to be a clipped seg.
		if (first_seg == iSegment)
		{
			gfFindFirstSeg = TRUE;
			first_seg = 0;
		}
		return;
	}
//44 fps	

	clipped_a=a;
	clipped_b=b;

	current_seg=iSegment;

	/* check for single sided walls */
	//GEH if(!seg_two_sided(iSegment))    /*single sided. ie a wall*/


	if(linedefs[iLinedef].psdt==-1)
	{
		if(side==BACK)   /*backface and single sided..CULL!!*/
		{
			// Clear first_seg if it was culled.
			if (first_seg == iSegment)
			{
				gfFindFirstSeg = TRUE;
				first_seg = 0;
			}
			return;
		}
		
		front_sect=segFront;
		flat_sect=front_sect;
		/* don't need to set back sector since there isn't one...*/
		if(draw_wall(iSegment,side)==DRAWN)
		{
			mark_sect_visible(segFront);
			linedefs[iLinedef].flags |= DRAW_ON_MAP_BIT;
			first=FALSE;
		}
		return;
	}
	//ONE SIDED LDs finished, now on to the TWO SIDED ones

	// Now it is safe to calculate segBack.
	segBack = sidedefs[linedefs[iLinedef].psdt].sec_ptr;

	if (side==FRONT)
	{
		front_sect=segBack;
		back_sect=segFront;
	}
	else
	{
		front_sect=segFront;
		back_sect=segBack;
	}

	
	/*first do lower post*/
	//GEH if(sector_to_fh(back_sect) => sector_to_fh(front_sect) )

	if(sectors[back_sect].fh >= sectors[front_sect].fh )
	{
		flat_sect=back_sect;
		if( draw_backface(TYPE_FLOOR)==DRAWN )
		{
			mark_sect_visible(segFront);
			linedefs[iLinedef].flags |= DRAW_ON_MAP_BIT;
			mark_sect_visible(segBack);
			first=FALSE;
		}
	}
	else
	{
		flat_sect=back_sect;
		if(draw_lower_wall(iSegment, side)==DRAWN)
		{
			mark_sect_visible(segFront);
			linedefs[iLinedef].flags |= DRAW_ON_MAP_BIT;
			mark_sect_visible(segBack);
			first=FALSE;
		}
	}

	/*Now do upper post*/
	//GEH if(sector_to_ch(back_sect) <= sector_to_ch(front_sect) )
	if(sectors[back_sect].ch <= sectors[front_sect].ch )
	{
		flat_sect=back_sect;
		if(draw_backface(TYPE_CEILING)==DRAWN)
		{
			mark_sect_visible(segFront);
			mark_sect_visible(segBack);
			first=FALSE;
		}
	}
	else
	{
		flat_sect=back_sect;
		if(draw_upper_wall(iSegment, side)==DRAWN)
		{
			mark_sect_visible(segFront);
			mark_sect_visible(segBack);
			linedefs[iLinedef].flags |= DRAW_ON_MAP_BIT;

			first=FALSE;
		}
	}

#if SEE_THROUGH
	/* Now do middle portion of a two sided wall */
	{
	LONG MiddleTextureIndex = seg_to_texture_num(iSegment, side, MIDDLE_TEXTURE);

	//if (seg_to_texture_num(iSegment, side, MIDDLE_TEXTURE) &&
	//	!(textures[seg_to_texture_num(iSegment,side,MIDDLE_TEXTURE)].type == TRANSP_TEX &&
	if (MiddleTextureIndex &&
		!(textures[MiddleTextureIndex].type == TRANSP_TEX &&
		  segs[iSegment].flip)
	)
	{
		flat_sect = front_sect;

		if(draw_wall(iSegment,side)==DRAWN)
		{
			mark_sect_visible(front_sect);
			linedefs[iLinedef].flags |= DRAW_ON_MAP_BIT;

			/* test for transparent wall */
			// if (textures[seg_to_texture_num(iSegment,FRONT,MIDDLE_TEXTURE)].type == TRANSP_TEX)
			//if (textures[seg_to_texture_num(iSegment,side,MIDDLE_TEXTURE)].type == TRANSP_TEX)
			if (textures[MiddleTextureIndex].type == TRANSP_TEX)
			{
				// printf("found truetranstex %li\n",iLinedef);
				mark_sect_visible(back_sect);
			}
			first=FALSE;
		}
	}
	// [d7-12-96 JPC] Don't use this method. See comment above.
	// else
	// 	linedefs[iLinedef].flags &= 0xFFFD;		/* clear DON'T DRAW */
	}
#endif

}

#if 0
// [d11-05-96 JPC]
// Original function did not take side into account.  Replace with
// CalcXTextureInfo, below.
// Asked Chris Phillips about this: he said OK.
/* =======================================================================
   Function    - calc_x_texture_info
   Description - ???
   Returns     - void
   ======================================================================== */
LONG calc_x_texture_info(LONG *xsrc_inc,LONG *src_x)
{
	LONG new_w;
	LONG clipped_off;


	new_w = dist(clipped_a.x,clipped_a.y,clipped_b.x,clipped_b.y);
	clipped_off = dist(clipped_a.x,clipped_a.y,orig_a.x,orig_a.y);

	if (proj_bx == proj_ax)		// prevent divide by zero
	{
		*xsrc_inc=0;
		*src_x=0;
		return new_w;
	}

	/* xsrc_inc is number of source columns per screen column */
	/* src_x is how much was clipped off + the texture offset from segs */
	if (wall_scale)
	{
		*xsrc_inc = ((new_w * wall_scale)<<16) / ((proj_bx-proj_ax) * UNITARY_SCALE);
		*src_x = (((clipped_off + segs[current_seg].txoff) * wall_scale) << 16) / UNITARY_SCALE;
	}
	else
	{
		*xsrc_inc = (new_w << 16) / (proj_bx-proj_ax);
		*src_x = (clipped_off + segs[current_seg].txoff) << 16;
	}

	return new_w;
}
#endif


// ---------------------------------------------------------------------------
LONG CalcXTextureInfo (LONG *xsrc_inc,LONG *src_x, LONG side, LONG fFlip)
{
// [d11-05-96 JPC] Replaces calc_x_texture_info, above.
// Takes side into account.  (If viewing a texture from the back side,
// we need to subtract the x offset from the width of the line segment
// to get the true offset.  Otherwise, especially with transparent
// textures whose line is broken into multiple line segments, we will see
// the texture at the wrong offset.
// (Solves ARCH problem in ANSIE_TW; does not solve PORTCULLIS problem
// in BRAEME_MW.  The portcullis problem could be solved by changing the
// lines in the WAD so we see their side 1 instead of their side 2.)
// ---------------------------------------------------------------------------

	LONG new_w;
	LONG clipped_off;
	LONG xOffset;
	SEG  *CurrentSegPtr;


	new_w = dist(clipped_a.x,clipped_a.y,clipped_b.x,clipped_b.y);
	clipped_off = dist(clipped_a.x,clipped_a.y,orig_a.x,orig_a.y);

	if (proj_bx == proj_ax)		// prevent divide by zero
	{
		*xsrc_inc=0;
		*src_x=0;
		return new_w;
	}

	CurrentSegPtr = &segs[current_seg];
	
	/* xsrc_inc is number of source columns per screen column */
	/* src_x is how much was clipped off + the texture offset from segs */
	if ((side == FRONT && fFlip) || (side == BACK && !fFlip))
	{
		// [d1-24-97 JPC] Correct equation seems to be:
		// 	xOffset = (total line length - line seg length) - txoff
		// Apparently DCK expects "flipped" lines (and non-flipped "back"
		// lines) to be drawn mirror-image, and calculates the texture
		// offsets accordingly.
		// Note that if the line segment is the same as the line, the
		// equation will produce an offset of -txoff; but in all such
		// cases I have seen, txoff is 0, so it doesn't matter.  But just
		// in case, if xOffset works out to less than 0, it is set to 0.

		LONG	lineLength;
		LONG	segLength;
		LONG	a;
		LONG	b;

		a = linedefs[CurrentSegPtr->lptr].a;
		b = linedefs[CurrentSegPtr->lptr].b;
		
		lineLength = dist (vertexs[a].x, vertexs[a].y, vertexs[b].x, vertexs[b].y);
		
		a = CurrentSegPtr->a;
		b = CurrentSegPtr->b;
		
		segLength = dist (vertexs[a].x, vertexs[a].y, vertexs[b].x, vertexs[b].y);
		
		xOffset = (lineLength - segLength) - CurrentSegPtr->txoff;
		if (xOffset < 0)
			xOffset = 0;						// for safety
	}
	else
	{
		xOffset = CurrentSegPtr->txoff;
	}

	if (wall_scale)
	{
		*xsrc_inc = ((new_w * wall_scale)<<16) / ((proj_bx-proj_ax) * UNITARY_SCALE);
		*src_x = (((clipped_off + (xOffset)) * wall_scale) << 16) / UNITARY_SCALE;
	}
	else
	{
		*xsrc_inc = (new_w << 16) / (proj_bx-proj_ax);
		*src_x = (clipped_off + (xOffset)) << 16;
	}
	return new_w;
}

#if 0
// [d9-23-96 JPC] Make this a macro instead for speed.
/* =======================================================================
   Function    - SetLightDelta
   Description - Change the lighting for a specific linedef
   Returns     - void
   ======================================================================== */
void SetLightDelta (LONG seg, LONG side)
{
// [d6-04-96 JPC] Made this a function--when working, should be posted back
// as in-line code in the 3 places it's called from.
// Side is fairly straightforward when drawing an entire wall, but when
// drawing a lower wall or upper wall, we seem to have flipped the meaning
// of front and back--check it again later.
//
// [d6-18-96 JPC] Added #defines to make it easy to change the number of
// bits (we just changed from 4 bits per sidedef to 3).  Also added
// LIGHT_DELTA_MULTIPLY to make the effect more noticeable.
// The #defines are in LIGHT.H.
// ---------------------------------------------------------------------------

   if (side == 0)                      // "front" side
      gLinedefLight = (linedefs[segs[seg].lptr].flags) >> SIDE_1_LIGHT_SHIFT;
   else
      gLinedefLight = (linedefs[segs[seg].lptr].flags) >> SIDE_2_LIGHT_SHIFT;
   gLinedefLight &= LIGHT_VALUE_MASK;
   if (gLinedefLight > LIGHT_MAX_POSITIVE)	// high bit is sign bit
      gLinedefLight -= LIGHT_ADD_FACTOR;
	gLinedefLight *= LIGHT_DELTA_MULTIPLY;		
}
#endif

/* =======================================================================
   Function    - add_floor_span
   Description - ?????????????
   Returns     - void
   ======================================================================== */
void add_floor_span(LONG x,LONG y,LONG sect)
{
	LONG y2;
	LONG clipped;								// not used
//return;
	/*assume y2 is screen bottom (it will probably be clipped)*/
//	y2 = VIEW_HEIGHT;
	y2 = screen_buffer_height;

	if(!clip_span(x,&y,&y2,&clipped))
	{
		/*if you get here floor span was clipped out of existence*/
		if(last_post_x!=0xffff)
		{
			/*if last_post was valid that means we need an endpost*/
			/*add an end*/
			add_span_event_end(last_post_x,last_post_y,last_post_ye,sect);
			floor_lastt_y=last_post_y;
			floor_lastb_y=last_post_ye;

			/*setup so when we come out of clipped area a new start will be added*/
			floor_need_start=TRUE;

			/*make last_post invalid*/
			last_post_x=last_post_y=last_post_ye=0xffff;
		}
		return; /*nothing more to do (post was clipped)*/
	}

	if(floor_need_start)
	{
		/* need start means that we are at the start of a segment or we emerged */
		/* from a clipped area, add a start */
		add_span_event_start(x,y,y2,sect);
		floor_need_start=0;

		/*setting these results in the next two if statments not being true*/
		floor_lastt_y=y;
		floor_lastb_y=y2;

		 /*+1 and -1 are so no overlap if next colum is full post*/
		last_post_x=x;last_post_y=y+1;last_post_ye=y2-1;
	}

	/*if y has changed then we need a new start point or possibly several points....*/
	if (y!=(LONG)floor_lastt_y)     /*handle top of post*/
	{
		if(y>(LONG)floor_lastt_y)
		{
			/* ends here are unfortuatly a very wierd case.
				I've tweeked with these numbers (-1's) till things
				work right... but there may be some problems once
				textures are added. I had hoped that end points could
				be used to get a texture offset (world x,world y) but
				because of these tweeks that might not work. */
			add_span_event_end(x-1,floor_lastt_y,y-1,sect);
		}
		else
			add_span_event_start(x,y,floor_lastt_y-1,sect);
		floor_lastt_y=y;
	}

	if (y2!=(LONG)floor_lastb_y)
	{
		if (y2>(LONG)floor_lastb_y)
			add_span_event_start(x,floor_lastb_y+1,y2,sect);
		else
			add_span_event_end(x-1,y2+1,floor_lastb_y,sect);
		floor_lastb_y=y2;
	}
	last_post_x=x;last_post_y=y;last_post_ye=y2;

	// check for click
	if(mouse_button || mouse_scan)
	{
		// did we click on this span
		if(cursor_x == x)
		{
			butClicked = mouse_button;
			if(cursor_y > last_post_y && cursor_y < last_post_ye)
			{
				// set the object clicked on
				objClicked = sect;
				typeClicked = iFLOOR;
				butClicked = mouse_button;
	
				// turn off click so we don't process any more
				mouse_button = 0;
			}
		}
	}
#ifdef _EDIT
// [d6-03-96 JPC]
// This is very similar to the "check for click" just above; but it is
// separate code so the editor will be easier to remove.
			if (gfFindSeg)
            EditCheckCoordinates (x, last_post_y, last_post_ye, sect,
               0, 0, typeFLOOR);
#endif
}

/* =======================================================================
   Function    - add_ceil_span
   Description - ???
   Returns     - void
   ======================================================================== */
void add_ceil_span(LONG x,LONG y,LONG sect)
{
	LONG y2;
	LONG clipped;								// not used

//return;
	sect=sect|0x00008000;		/* mark as ceil */

  /*assume y2 is screen bottom (it will probably be clipped)*/
	y2=y;
	y=0;

	if(!clip_span(x,&y,&y2,&clipped))
	{
		/*if you get here floor span was clipped out of existence*/
		if(last_post_x!=0xffff)
		{
			/*if last_post was valid that means we need an endpost*/
			/*add an end*/
			add_span_event_end(last_post_x,last_post_y,last_post_ye,sect);
			floor_lastt_y=last_post_y;
			floor_lastb_y=last_post_ye;

			/*setup so when we come out of clipped area a new start will be added*/
			floor_need_start=TRUE;

			/*make last_post invalid*/
			last_post_x=last_post_y=last_post_ye=0xffff;
		}
		return; /*nothing more to do (post was clipped)*/
	}

	if(floor_need_start)
	{
		/*need start means that we are at the start of a segment */
		/*	or we emerged from a clipped area, add a start*/
		add_span_event_start(x,y,y2,sect);
		floor_need_start=0;

		/*setting these results in the next to if statments not being true*/
		floor_lastt_y=y;
		floor_lastb_y=y2;

		/* +1 and -1 are so no overlap if next colum is full post */
		last_post_x=x;last_post_y=y+1;last_post_ye=y2-1;
	}

	/*if y has changed then we need a new start point or possibly several points....*/
	if(y!=(LONG)floor_lastt_y)     /*handle top of post*/
	{
		if(y>(LONG)floor_lastt_y)
		{
				/*ends here are unfortuatly a very wierd case.
					ive tweeked with these numbers (-1's) till things
					work right... but there may be some problems once
					textures are added. I had hoped that end points could
					be used to get a texture offset (world x,world y) but
					because of these tweeks that might not work.
				*/
			add_span_event_end(x-1,floor_lastt_y,y-1,sect);
		}
		else
			add_span_event_start(x,y,floor_lastt_y-1,sect);
		floor_lastt_y=y;
	}

	if(y2!=(LONG)floor_lastb_y)
	{
		if(y2>(LONG)floor_lastb_y)
			add_span_event_start(x,floor_lastb_y+1,y2,sect);
		else
			add_span_event_end(x-1,y2+1,floor_lastb_y,sect);
		floor_lastb_y=y2;
	}
	last_post_x=x;last_post_y=y;last_post_ye=y2;
#ifdef _EDIT
// [d6-03-96 JPC]
// Similar to floor check in add_floor_span:
			if (gfFindSeg)
            EditCheckCoordinates (x, last_post_y, last_post_ye, sect & 0x7FFF,
               0, 0, typeCEILING);
#endif
}

#if 0
// gAddFactor is a global variable for on-the-fly testing of infravision.
// The higher the value of gAddFactor, the darker non-heat-source things are.
// Make gAddFactor a constant when done debugging and testing various numbers.
LONG gAddFactor = 16;
// ---------------------------------------------------------------------------
void RenderChangeLight (LONG * light, BOOL fHeatSource)
{
// [d7-16-96 JPC] Apply various global lighting effects to the base light.
// Color remapping has to be handled separately.  See THINGS: draw_thing.
// ---------------------------------------------------------------------------

	if (gfTestMagicLightSwitch)
      *light -= 16;                    // brighten by 16 (there are 32 steps)
	if (gcMagicTemporaryLight > 0)
		*light -= gcMagicTemporaryLight/10;
	if (*light < 0)
		*light = 0;

	// Infravision might have to be handled separately.
	if (gfInfraVision)
	{
		if (fHeatSource)
		{
			*light += (INFRAVISION_OFFSET - gAddFactor);
			if (*light < INFRAVISION_OFFSET)
				*light = INFRAVISION_OFFSET;
		}
		else
		{
			*light += INFRAVISION_OFFSET;
			// *light += INFRAVISION_OFFSET + gAddFactor; // non-alive things are darker
			// if (*light > INFRAVISION_MAX)
			// 	*light = INFRAVISION_MAX;
		}
	}
}
#endif

/* =======================================================================
   Function    - draw_wall
   Description - draws a wall in nova
   Returns     - whether or not it was actually drawn
   ======================================================================== */
static LONG draw_wall(LONG seg, LONG side)
{
	POINT ba,bb,ta,tb;
	LONG scaned[MAX_VIEW_WIDTH];
	LONG scanedt[MAX_VIEW_WIDTH];
	FIXED16 src_x;
	LONG new_w;
	LONG x1,x2,save_x1;
	LONG xsrc_inc;
	LONG clipped;
	LONG delta_y;
	SHORT type;
   // In this function, bz = floor height and tz = ceiling height.
	//GEH LONG bz = (-sector_to_fh(front_sect))-camera.z;	/*get hei of floor*/
	//GEH LONG tz = (-sector_to_ch(front_sect))-camera.z;	/*get height of ceil in sector*/
	// JPC LONG bz = (-sector_to_fh(front_sect))+camera.z;	/*get hei of floor*/
	// JPC LONG tz = (-sector_to_ch(front_sect))+camera.z;	/*get height of ceil in sector*/
	LONG	bz;
	LONG	tz;
	LONG	floorHeight;				// JPC intermediate variable for clarity
	LONG	ceilingHeight;          // JPC intermediate variable for clarity
	// const LONG texture=seg_to_texture_num(seg,FRONT,MIDDLE_TEXTURE);
	// [d7-12-96 JPC] Specify "side"--same as in draw_upper_wall and
	// draw_lower_wall.
	LONG texture;



	// In this case, we have a one-sided linedef and front_sect is the only
	// sector for it.
	floorHeight = sector_to_fh (front_sect);
	ceilingHeight = sector_to_ch (front_sect);
	// JPC question: can't we assume the floor height is less than or
	// equal to the ceiling height?  But put the following in anyway to
	// preserve the original calculations.
	if (floorHeight <= ceilingHeight)
	{
		bz = -floorHeight + camera.z;
		tz = -ceilingHeight + camera.z;
		floor_z = ceilingHeight;
	}
	else
	{
		bz = -ceilingHeight + camera.z;
		tz = -floorHeight + camera.z;
		floor_z = floorHeight;
	}

	// GWP Moved this test to nearer the begining of the fn. Seems like we should
	//     exclude the wall before bothering to get the texture information.
	/*project bottom of wall*/
	ba=clipped_a;
	bb=clipped_b;
	proj(&ba,bz);
	proj(&bb,bz);
	
	/* Check wall against extent list */
	if(!extent_visible(ba.x,bb.x))
	{
		return(NOT_DRAWN);
	}
	
	texture=seg_to_texture_num(seg,side,MIDDLE_TEXTURE);
	
	// [d7-11-96 JPC] Don't do this here! Wait until after wall_scale is calculated (below).
	// if (wall_scale)
	//	floor_z = (floor_z * wall_scale) / UNITARY_SCALE;

	//gtxoff = seg_to_txoff(seg, FRONT);  // [d6-07-96 JPC]
	seg_to_txoff_tyoff(seg, FRONT, &gtxoff, &tyoff);	// GWP
   while (gtxoff < 0)
      gtxoff += textures[texture].h;   // not .w because of 90 degree rotation of texture graphic
	//tyoff = seg_to_tyoff(seg, FRONT);

	// Middle texture cannot be "pegged," so set gPeggedValue to 0.
	gPeggedValue = 0;		// line up on ground-zero

	type = textures[texture].type;
	if (type != SKY_TEX)
	{

#if WRC_CAMERA_TEST		
		if (type==CAMERA_TEX)
		{
			DynTexFrameData[texture]=segs[seg].lptr;
			wTexture = textures[texture].h;
			hTexture = textures[texture].w;
		}
		else
		{
			wTexture = textures[texture].w;
			hTexture = textures[texture].h;
		}
#else
		wTexture = textures[texture].w;
		hTexture = textures[texture].h;
#endif
		wall_scale = textures[texture].scale;
		if (wall_scale)
		{
			tyoff = (tyoff * wall_scale) / UNITARY_SCALE;
			gtxoff = (gtxoff * wall_scale) / UNITARY_SCALE; // [d6-20-96 JPC]
			// [d7-11-96 JPC] Move floor_z calculation here.
			floor_z = (floor_z * wall_scale) / UNITARY_SCALE;
		}
		pTexture = ((PTR)BLKPTR((SHORT)textures[texture].t)) + sizeof(BITMHDR);
	}

	/*project bottom of wall*/
   // GWP Moved to higher in the fn.
   // GWP 	ba=clipped_a;
   // GWP 	bb=clipped_b;
   // GWP 	proj(&ba,bz);
   // GWP 	proj(&bb,bz);
   // GWP 
   // GWP 	/* Check wall against extent list */
   // GWP 	if(!extent_visible(ba.x,bb.x))
   // GWP 	{
   // GWP 		return(NOT_DRAWN);
   // GWP 	}

	/* project top of wall */
	ta=clipped_a;
	tb=clipped_b;
	proj(&ta,tz);
	proj(&tb,tz);

	/* get screen start and end points */
	x1=ba.x;
	x2=bb.x;
	if(x1 > x2)				/* force walls to go from left to right */
		SWAPINT(x1,x2);

	save_x1=x1;

	proj_ax=x1;
	proj_bx=x2;

	/* scan convert top and bottom of wall */
	scan_line(&ba,&bb,&scaned[0]);
	scan_line(&ta,&tb,&scanedt[0]);

	/* get texture height.. */
	col_hei=ABS(tz-bz);

	if (wall_scale)
		col_hei = (col_hei * wall_scale) / UNITARY_SCALE;

	/* get x texture constants */
	// [d11-05-96 JPC] Add side parameter.
	// new_w = calc_x_texture_info(&xsrc_inc,&src_x.lval);
	new_w = CalcXTextureInfo (&xsrc_inc,&src_x.lval,side, segs[seg].flip);

	/* set sector lighting */
	SectLight = sector_to_light(front_sect);
	// [d9-23-96 JPC] The following seems to be redundant (duplicated in
	// scale_col_ttop).
	// if (!SectLight) SetLight(0);
   SetLightDelta (seg, 0);                // [d6-04-96 JPC] [draw_wall]

	// [d9-23-96 JPC] Added gAdjustLight to speed up scale_col_ttop.
	gAdjustLight = 0;
	if (gfTestMagicLightSwitch)
      gAdjustLight -= 16;
	if (gcMagicTemporaryLight > 0)
		gAdjustLight -= gcMagicTemporaryLight/10;

	// [d12-13-96 JPC] If light is brighter than a certain threshhold,
	// do not apply infravision.  Note that we do not take gLinedefLight
	// into account in determining sector lighting.  That's because we
	// don't want some walls to be in infravision while others are normal.
	if (gfInfraVision && SectLight + gAdjustLight > INFRAVISION_THRESHHOLD)
	{
		gAdjustLight += INFRAVISION_OFFSET;
		gMinLight = INFRAVISION_OFFSET;
		gMaxLight = INFRAVISION_MAX;
	}
	else {
		gMinLight = 0;
		gMaxLight = DARKEST;
	}

	/* draw ceil code */
	save_x1=x1;
	floor_need_start=TRUE;
	last_post_x=last_post_y=last_post_ye=0xffff;
	while(x1<x2)
	{
		add_ceil_span(x1,scanedt[x1],flat_sect);
		++x1;
	}

	/*close out ceils*/
	if(last_post_x!=0xffff)         /*add an end*/
		add_span_event_end(last_post_x,last_post_y,last_post_ye,flat_sect|0x00008000);
	x1=save_x1;

	/*now do floors*/
	floor_need_start=TRUE;
	last_post_x=last_post_y=last_post_ye=0xffff;
	save_x1=x1;
	while(x1<x2)
	{
		add_floor_span(x1,scaned[x1],flat_sect);
		++x1;
	}

	/*close out floors*/
	if(last_post_x!=0xffff)         /*add an end*/
		add_span_event_end(last_post_x,last_post_y,last_post_ye,flat_sect);

	x1=save_x1;

	if (type == SKY_TEX)
	{
		while(x1<x2)
		{
			if(clip_span(x1,&scanedt[x1],&scaned[x1],&clipped))
				scrape(x1,scaned[x1],scanedt[x1]);
			++x1;
		}
		return(DRAWN);
	}

//	if(segs[seg].lptr!=2)
//GWP no need for this test.	if(TRUE)
	{
		LONG const shift_16_colhei = col_hei << 16;
		LONG const shift_15_colhei = col_hei << 15;
		
		while(x1<x2)
		{
			delta_y = scaned[x1] - scanedt[x1];

#if SEE_THROUGH
			if (textures[texture].type == TRANSP_TEX)		/* check for see-through */
			{
				if ( clip_obj(x1,&scanedt[x1],&scaned[x1],&clipped) )
				{
					LONG	yIncr;
					LONG	light;

					thing_spans[tot_thing_spans].t			= textures[texture].t;
					// Add information about light for transparent wall.
					// yIncr is calculated and used the same way the src_inc
					// parameter is used in scale_col_ttop to darken walls.
					yIncr = (col_hei << 16) / delta_y;
					light = NSEWLight + SectLight + gLinedefLight;
					if (SectLight)
					{
						// If not brightest then shade with distance.
						light += yIncr >> DARKEN_FACTOR;
						if (light > DARKEST)
							light = DARKEST;
					}
					RenderChangeLight (light);
					thing_spans[tot_thing_spans].light		= (SHORT) light;
					thing_spans[tot_thing_spans].sx			= src_x.TwoHalves.sHigh%(textures[texture].h-1);
					thing_spans[tot_thing_spans].dx			= x1;
					thing_spans[tot_thing_spans].dy			= scanedt[x1];
					thing_spans[tot_thing_spans].dye			= scaned[x1];
					thing_spans[tot_thing_spans].clip		= clipped;
					thing_spans[tot_thing_spans++].src_inc	= (shift_15_colhei) / delta_y;
					
					if(tot_thing_spans>MaxThingSpans)
				#if defined(_DEBUG)
						fatal_error("Exceeded MaxThingSpans");
				#else
						return (NOT_DRAWN);
				#endif
				}
			}
			else
			{
				if ( clip_span(x1,&scanedt[x1],&scaned[x1],&clipped) )
				{
					scale_col_ttop(src_x.TwoHalves.sHigh,x1,scanedt[x1],scaned[x1],clipped,(shift_16_colhei)/delta_y);
				}
			}
#else
			if (clip_span(x1,&scanedt[x1],&scaned[x1],&clipped))
				scale_col_ttop(src_x.TwoHalves.sHigh,x1,scanedt[x1],scaned[x1],clipped,(shift_16_colhei)/delta_y);
#endif

#ifdef _EDIT
// [d6-03-96 JPC] Editor needs to know what we clicked on.
			if (gfFindSeg)
            EditCheckCoordinates (x1, scanedt[x1], scaned[x1], seg, MIDDLE_TEXTURE, FRONT, typeWALL);
#endif

			src_x.lval += xsrc_inc;
			++x1;
		}
	}

	/*close out floors*/

	/* If wall was TYPE_WALL and not see-through, fill in extent list */
#if SEE_THROUGH
	if (textures[texture].type != TRANSP_TEX)
#endif
		add_extent(ba.x,bb.x);

	/* Tell caller we drew at least some part of wall */
	return(DRAWN);
}

/* =======================================================================
   Function    - draw_lower_wall
   Description - draws a lower wall
   Returns     - whether or not it was drawn
   Change      - JPC, 6-04-96: added "side" parameter to fix double-sided
                 linedef display bug
   ======================================================================== */
static LONG draw_lower_wall(LONG seg, LONG side)
{
	POINT ba,bb,ta,tb;
	LONG bz,tz;                         // floor heights of front/back sectors
	LONG scaned[MAX_VIEW_WIDTH];
	LONG scanedt[MAX_VIEW_WIDTH];
	LONG x1,x2,save_x1;
	FIXED16 src_x;
	LONG xsrc_inc;
	LONG delta_y;
	LONG clipped;
	LONG texture;
	LONG	closerFloorHeight;
	LONG	furtherFloorHeight;
	SHORT type;

#if defined(_DEBUG) && defined(_JPC)
	if (segs[seg].lptr == giLinedef)
		++gDebug; // put break point here
#endif

	// Remember that front_sect is the sector further away from us and
	// back_sect is the one closer to us.  [Or so it seems--JPC.]
	closerFloorHeight = sector_to_fh (back_sect);
	furtherFloorHeight = sector_to_fh (front_sect);
 	if (closerFloorHeight < furtherFloorHeight)
	{
		// The usual case.
		bz = -closerFloorHeight + camera.z;
		tz = -furtherFloorHeight + camera.z;
		floor_z = furtherFloorHeight;
	}
	else
	{
		// The unusual (maybe nonexistent?) case.
		bz = -furtherFloorHeight + camera.z;
		tz = -closerFloorHeight + camera.z;
		floor_z = closerFloorHeight;
	}

	// Moved here to try and exclude walls not drawn sooner.
	ba=clipped_a;
	bb=clipped_b;
	proj(&ba,bz);
	proj(&bb,bz);
	
	/*Check wall against extent list*/
	if(!extent_visible(ba.x,bb.x))
	{
		return(NOT_DRAWN);
	}
		
	// [d7-11-96 JPC] Don't do this here! Wait until after wall_scale is calculated (below).
	// if (wall_scale)
	// 	floor_z = (floor_z * wall_scale) / UNITARY_SCALE;

	texture=seg_to_texture_num(seg,side,LOWER_TEXTURE);
	//gtxoff = seg_to_txoff(seg, side);   // [d6-07-96 JPC]
	seg_to_txoff_tyoff(seg, side, &gtxoff, &tyoff);	// GWP
   while (gtxoff < 0)
      gtxoff += textures[texture].h;   // not .w because of 90 degree rotation
	//tyoff = seg_to_tyoff(seg,side);

	type = textures[texture].type;
	if (type != SKY_TEX)
	{
		wTexture = textures[texture].w;
		hTexture = textures[texture].h;
		wall_scale = textures[texture].scale;
		if (wall_scale)
		{
			tyoff = (tyoff * wall_scale) / UNITARY_SCALE;
			gtxoff = (gtxoff * wall_scale) / UNITARY_SCALE; // [d6-20-96 JPC]
			// [d7-11-96 JPC] Move floor_z calculation here.
			floor_z = (floor_z * wall_scale) / UNITARY_SCALE;
		}
		pTexture = ((PTR)BLKPTR((SHORT)textures[texture].t)) + sizeof(BITMHDR);
	}

	// [d6-19-96 JPC] We are again treating the upper/lower pegging as
	// independent, but note that we have reversed the meanings of pegged
	// and unpegged from what they are in DOOM and DCK.
 	gPeggedValue = (linedefs[segs[seg].lptr].flags & 16);	// 16 = lower wall pegged
//38fps

	/*project bottom of wall*/
	// GWP Moved to higher in the fn to stop looking for texture info for excluded walls.
	// GWP ba=clipped_a;
	// GWP bb=clipped_b;
	// GWP proj(&ba,bz);
	// GWP proj(&bb,bz);
	// GWP 
	// GWP /*Check wall against extent list*/
	// GWP if(!extent_visible(ba.x,bb.x))
	// GWP 	return(NOT_DRAWN);

	/*project bottom of wall*/
	ta=clipped_a;
	tb=clipped_b;
	proj(&ta,tz);
	proj(&tb,tz);

	/*get screen start and end points*/
	x1=ba.x;x2=bb.x;
	if(x1>x2)     /*I dont think this should ever happen but it does???*/
		SWAPINT(x1,x2);
	save_x1=x1;

	proj_ax=x1;proj_bx=x2;

	/* scan convert top and bottom of wall */
	scan_line(&ba,&bb,&scaned[0]);
	scan_line(&ta,&tb,&scanedt[0]);

	/*get texture height..*/
	col_hei=ABS(tz-bz);
	if (wall_scale)
		col_hei = (col_hei * wall_scale) / UNITARY_SCALE;
//36fps

	/* get x texture constants */
	// [d11-05-96 JPC] Add side parameter.
	// calc_x_texture_info(&xsrc_inc,&src_x.lval);
	CalcXTextureInfo(&xsrc_inc,&src_x.lval,side, segs[seg].flip);

//35fps

	/* set sector lighting */
	SectLight = sector_to_light(back_sect);

	// if (!SectLight) SetLight(0)				// [d11-06-96 JPC] omit

   SetLightDelta (seg, side);                // [d6-04-96 JPC]

	// [d9-23-96 JPC] Added gAdjustLight to speed up scale_col_ttop.
	gAdjustLight = 0;
	if (gfTestMagicLightSwitch)
      gAdjustLight -= 16;
	if (gcMagicTemporaryLight > 0)
		gAdjustLight -= gcMagicTemporaryLight/10;

	// [d12-13-96 JPC] See comment in corresponding section of draw_wall.
	if (gfInfraVision && SectLight + gAdjustLight > INFRAVISION_THRESHHOLD)
	{
		gAdjustLight += INFRAVISION_OFFSET;
		gMinLight = INFRAVISION_OFFSET;
		gMaxLight = INFRAVISION_MAX;
	}
	else {
		gMinLight = 0;
		gMaxLight = DARKEST;
	}

	/*now do floors*/
	floor_need_start=TRUE;
	last_post_x=last_post_y=last_post_ye=0xffff;
	save_x1=x1;
	while(x1<x2)
	{
		add_floor_span(x1,scaned[x1],flat_sect);
		++x1;
	}
//33fps


	/*close out floors*/
	if(last_post_x!=0xffff)         /*add an end*/
		add_span_event_end(last_post_x,last_post_y,last_post_ye,flat_sect);

	x1=save_x1;

	if (type == SKY_TEX)
	{
		while(x1<x2)
		{
			if(clip_span(x1,&scanedt[x1],&scaned[x1],&clipped))
				scrape(x1,scaned[x1],scanedt[x1]);
			++x1;
		}
		return(DRAWN);
	}
//33fps

	{
	LONG const shift_16_colhei = col_hei << 16;
	
	while(x1<x2)
	{
		delta_y = scaned[x1] - scanedt[x1];
		if(clip_span(x1,&scanedt[x1],&scaned[x1],&clipped))
		{
			scale_col_ttop(src_x.TwoHalves.sHigh,x1,scanedt[x1],scaned[x1],clipped,(shift_16_colhei)/delta_y);
		}
#ifdef _EDIT
// [d6-03-96 JPC]
      if (gfFindSeg)
      {
         EditCheckCoordinates (x1, scanedt[x1], scaned[x1], seg, LOWER_TEXTURE, side, typeWALL);
      }
#endif
		src_x.lval+=xsrc_inc;
		++x1;
	}
	}
	/*close out floors*/

	/* Tell caller we drew at least some part of wall */
	return(DRAWN);
}


/* =======================================================================
   Function    - draw_upper_wall
   Description - draws an upper wall
   Returns     - whether or not it was drawn
   Change      - JPC 6-04-96 Added "side" parameter so that "side 2" of
						a 2-sided linedef will use the right texture.
						Note that it's pretty unusual to have a 2-sided linedef.
						An example of one is in a stairway, where the riser of
						the step (going up the stairs) and the ceiling over
						the step (going down) will be two different sidedefs
						of the same linedef.
   ======================================================================== */
static LONG draw_upper_wall(LONG seg, LONG side)
{
	POINT ba,bb,ta,tb;
	LONG bz,tz;                         // ceiling heights of front/back sectors
	LONG	closerCeilingHeight;
	LONG	furtherCeilingHeight;
	LONG scaned[MAX_VIEW_WIDTH];
	LONG scanedt[MAX_VIEW_WIDTH];
	LONG x1,x2,save_x1;
	FIXED16 src_x;
	LONG xsrc_inc;
	LONG delta_y;
	LONG clipped;
	LONG texture;
	SHORT type;

#if defined(_DEBUG) && defined(_JPC)
	if (segs[seg].lptr == giLinedef)
		++gDebug; // put break point here
#endif

	//GEH bz = (-sector_to_ch(front_sect))-camera.z; /*get hei of ceiling*/
	//GEH tz = (-sector_to_ch(back_sect))-camera.z;  /*get height of adj sector*/
	// JPC bz = (-sector_to_ch(front_sect))+camera.z; /*get hei of ceiling*/
	// JCP tz = (-sector_to_ch(back_sect))+camera.z;  /*get height of adj sector*/

	// Remember that front_sect is the sector further away from us and
	// back_sect is the one closer to us.  [I realize this seems odd,
	// but it seems to be true--JPC.]
	closerCeilingHeight = sector_to_ch (back_sect);
	furtherCeilingHeight = sector_to_ch (front_sect);
 	if (closerCeilingHeight > furtherCeilingHeight)
	{
		// The usual case.	
		bz = -closerCeilingHeight + camera.z;
		tz = -furtherCeilingHeight + camera.z;
		floor_z = closerCeilingHeight;
	}
	else
	{
		// JPC: I never saw this case when debugging.  Wouldn't this case be
		// culled because the texture is invisible?
		bz = -furtherCeilingHeight + camera.z;
		tz = -closerCeilingHeight + camera.z;
		floor_z = furtherCeilingHeight;
	}

	/*project bottom of wall*/
	// GWP Moved this here from below to try and exclude walls sooner.
	ba=clipped_a;
	bb=clipped_b;
	proj(&ba,bz);
	proj(&bb,bz);

	/*Check wall against extent list*/
	if(!extent_visible(ba.x,bb.x))
	{
		return(NOT_DRAWN);
	}

	// [d7-11-96 JPC] Don't do this here! Wait until after wall_scale is calculated (below).
	// if (wall_scale)
	// 	floor_z = (floor_z * wall_scale) / UNITARY_SCALE;

	texture=seg_to_texture_num(seg,side,UPPER_TEXTURE);
	//gtxoff = seg_to_txoff(seg, side);   // [d6-07-96 JPC]
	seg_to_txoff_tyoff(seg, side, &gtxoff, &tyoff);	// GWP
   while (gtxoff < 0)
      gtxoff += textures[texture].h;   // not .w because of 90 degree rotation of textures
	//tyoff = seg_to_tyoff(seg,side);

	type = textures[texture].type;
	if (type != SKY_TEX)
	{
		wTexture = textures[texture].w;
		hTexture = textures[texture].h;
		wall_scale = textures[texture].scale;
		if (wall_scale)
		{
			tyoff = (tyoff * wall_scale) / UNITARY_SCALE;
			gtxoff = (gtxoff * wall_scale) / UNITARY_SCALE; // [d6-20-96 JPC]
			// [d7-11-96 JPC] Move floor_z calculation here.
			floor_z = (floor_z * wall_scale) / UNITARY_SCALE;
		}
		pTexture = ((PTR)BLKPTR((SHORT)textures[texture].t)) + sizeof(BITMHDR);
	}


	// [d6-19-96 JPC] We are again treating the upper/lower pegging as
	// independent, but note that we have reversed the meanings of pegged
	// and unpegged from what they are in DOOM and DCK.
	gPeggedValue = linedefs[segs[seg].lptr].flags & 8;	// 8 - Upper wall pegged

	/*project bottom of wall*/
	// GWP Moved this exclusion test higher in the fn.
	// GWP ba=clipped_a;
	// GWP bb=clipped_b;
	// GWP proj(&ba,bz);
	// GWP proj(&bb,bz);
	// GWP 
	// GWP /*Check wall against extent list*/
	// GWP if(!extent_visible(ba.x,bb.x))
	// GWP 	return(NOT_DRAWN);

	/*project bottom of wall*/
	ta=clipped_a;tb=clipped_b;
	proj(&ta,tz);proj(&tb,tz);

	/*get screen start and end points*/
	x1=ba.x;x2=bb.x;
	if(x1>x2)     /*I dont think this should ever happen but it does???*/
		SWAPINT(x1,x2);
	save_x1=x1;

	proj_ax=x1;proj_bx=x2;

	/* scan convert top and bottom of wall */
	/* Note order is different from a regular wall */
	scan_line(&ba,&bb,&scanedt[0]);
	scan_line(&ta,&tb,&scaned[0]);

	/*get texture height..*/
	col_hei=ABS(tz-bz);
	if (wall_scale)
		col_hei = (col_hei * wall_scale) / UNITARY_SCALE;

	/* get x texture constants */
	// [d11-05-96 JPC] Add side parameter.
	// calc_x_texture_info(&xsrc_inc,&src_x.lval);
	CalcXTextureInfo (&xsrc_inc,&src_x.lval, side, segs[seg].flip);

	/* set sector lighting */
//	SectLight = sector_to_light(front_sect);
	SectLight = sector_to_light(back_sect);

	// if (!SectLight) SetLight(0);			// [d11-06-96 JPC] omit

	SetLightDelta (seg, side);          // [d6-04-96 JPC]

	// [d9-23-96 JPC] Added gAdjustLight to speed up scale_col_ttop.
	gAdjustLight = 0;
	if (gfTestMagicLightSwitch)
      gAdjustLight -= 16;
	if (gcMagicTemporaryLight > 0)
		gAdjustLight -= gcMagicTemporaryLight/10;

	// [d12-13-96 JPC] See comment in corresponding section of draw_wall.
	if (gfInfraVision && SectLight + gAdjustLight > INFRAVISION_THRESHHOLD)
	{
		gAdjustLight += INFRAVISION_OFFSET;
		gMinLight = INFRAVISION_OFFSET;
		gMaxLight = INFRAVISION_MAX;
	}
	else {
		gMinLight = 0;
		gMaxLight = DARKEST;
	}

	/* draw ceil code */
	save_x1=x1;
	floor_need_start=TRUE;
	last_post_x=last_post_y=last_post_ye=0xffff;
	while(x1<x2)
	{
		add_ceil_span(x1,scanedt[x1],flat_sect);
		++x1;
	}

	/*close out ceils*/
	if(last_post_x!=0xffff)         /*add an end*/
		add_span_event_end(last_post_x,last_post_y,last_post_ye,flat_sect|0x00008000);
	x1=save_x1;

	if (type == SKY_TEX)
	{
		while(x1<x2)
		{
			if(clip_span(x1,&scanedt[x1],&scaned[x1],&clipped))
				scrape(x1,scaned[x1],scanedt[x1]);
			++x1;
		}
		return(DRAWN);
	}

	{
	LONG const shift_16_colhei = col_hei << 16;
	
	while(x1<x2)
	{
		delta_y = scaned[x1] - scanedt[x1];
		if(clip_span(x1,&scanedt[x1],&scaned[x1],&clipped))
		{
			scale_col_ttop(src_x.TwoHalves.sHigh,x1,scanedt[x1],scaned[x1],clipped,(shift_16_colhei)/delta_y);
		}
#ifdef _EDIT
// [d6-03-96 JPC]
      if (gfFindSeg)
         EditCheckCoordinates (x1, scanedt[x1], scaned[x1], seg, UPPER_TEXTURE, side, typeWALL);
#endif
		src_x.lval+=xsrc_inc;
		++x1;
	}
	}

	/* Tell caller we drew at least some part of wall */
	return(DRAWN);
}


/* =======================================================================
   Function    - draw_backface
   Description - ??????????????????????????????????????????????????????????
   Returns     - DRAWN or NOT_DRAWN
   ======================================================================== */
LONG draw_backface(LONG type)
{
	LONG scaned[MAX_VIEW_WIDTH];
	LONG x1,x2;
	LONG tz,bz;
	POINT a,b;
	a=clipped_a;b=clipped_b;

	if(type==TYPE_FLOOR)
	{
		//GEH bz=(-sector_to_fh(back_sect))-camera.z; /*get hei of floor*/
		bz=(-sector_to_fh(back_sect))+camera.z; /*get hei of floor*/
		proj(&a,bz);
		proj(&b,bz);
	}
	else
	{
		//GEH tz=(-sector_to_ch(back_sect))-camera.z; /*get hei of ceil*/
		tz=(-sector_to_ch(back_sect))+camera.z; /*get hei of ceil*/
		proj(&a,tz);
		proj(&b,tz);
	}
	if(!extent_visible(a.x,b.x))
		return(NOT_DRAWN);

	scan_line(&a,&b,&scaned[0]);
	x1=a.x;
	x2=b.x;
	if(x1>x2)
		SWAPINT(x1,x2);

	if(type==TYPE_FLOOR)
	{
		floor_need_start=TRUE;
		last_post_x=last_post_y=last_post_ye=0xffff;
		while(x1<x2)
		{
			add_floor_span(x1,scaned[x1],flat_sect);
			++x1;
		}
		if(last_post_x!=0xffff)                 /*add an end*/
			add_span_event_end(last_post_x,last_post_y,last_post_ye,flat_sect);
		return(DRAWN);
	}

	floor_need_start=TRUE;
	last_post_x=last_post_y=last_post_ye=0xffff;
	while(x1<x2)
	{
		add_ceil_span(x1,scaned[x1],flat_sect);
		++x1;
	}

	if(last_post_x!=0xffff)                 /*add an end*/
		add_span_event_end(last_post_x,last_post_y,last_post_ye,flat_sect|0x00008000);

	return(DRAWN);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
