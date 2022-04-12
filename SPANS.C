/* =======================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: SPANS.C     -Handles Spans
   Author: Chris Phillips & Alan Clark
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:
   clear_spans           -clears the spans arrays
   add_line              -???
   draw_lines            -???
   draw_spans            -???
   open_span             -???
   close_span            -???
   add_span_event_start  -???
   add_span_event_end    -???

	JPC Notes:
	- a "span" is a portion of a scanline
	- a "post" is a portion of a column

   =®RM250¯====================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "system.h"
#include "engine.h"
#include "engint.h"
#include "machine.h"
#include "dynamtex.h"
#include "light.h"							// [d7-03-96 JPC]

#if defined( _DEBUG) && defined(_WINDOWS) && defined(_JPC)
static char gszSourceFile[] = __FILE__;
#endif

extern UBYTE				shade_table[];

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define MAX_POSTS_PER_COL	24

#define SPAN_START	(1)
#define SPAN_END		(0)

#define SPAN_OPEN		(1)
#define SPAN_CLOSED	(0)


#define ABSURD_SECTOR (99999)
#define LUDICROUS_SECTOR (-1)

/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */

// [d7-02-96 JPC] Removed xe, wx, and wy from SPAN struct because these
// members aren't used.
typedef struct{
//	LONG x,xe;
//	LONG wx,wy;
	LONG x;			// This is a screen coordinate x value.
	ULONG	sector;
	LONG status;	// Open or closed.
	}SPAN;

// [d7-02-96 JPC] Removed wx and wy from POST struct because these
// members aren't used.
// GEH changed the LONG to SHORT
typedef struct{
	SHORT y;		// y screen coordinate that begins the post. (Wall)
	SHORT ye;		// y screen coordinate that ends this post.
//	SHORT wx,wy;
	USHORT sector;
	SHORT type;
	}POST;

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
static void draw_span( LONG isx, LONG isy, LONG isx_end, UBYTE* tex, LONG cz,
	LONG size, LONG scale, ULONG tex_mask, LONG light, LONG Homogenousflag,
	LONG tex_start);
	
static void draw_cspan(LONG isx, LONG isy, LONG isx_end, UBYTE* tex, LONG cz,
	LONG size, LONG scale, ULONG tex_mask, LONG light);
	
static void close_span (ULONG x,ULONG y,ULONG sector);
static void closeout_span (LONG y);
static void open_span (ULONG x,ULONG y,ULONG sector);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Module-level (more respectable than global--JPC) Variables
   ------------------------------------------------------------------------ */
// GWP took these static vars off data space and put them in the heap.
typedef POST POST_TABLE[MAX_POSTS_PER_COL][MAX_VIEW_WIDTH];
typedef POST_TABLE *POST_TABLE_PTR;

// GWP span_random_start is an array of random numbers used to start
//     drawing a homogenous texture. It's recalculated each time the
//     camera moves.
static LONG			span_random_start[MAX_VIEW_HEIGHT + 10];

// GWP last_post is the number of span endpts at a given x coordinate in the
//	   gPosts matrix. (Both start and end.)
static ULONG		last_post[MAX_VIEW_WIDTH];

// GWP The gPosts matrix is a list. At each x coordinate of every beginning
//     and post there is an entry. There are as many posts at any given time
//     in the y direction as there are wall posts in the view. The number of
//     posts at each x coordinate is kept in the array last_post[x].
//     Each x coordinate can have MAX_POSTS_PER_COL posts (24 as of 4-16-97).
//     This is not enough for extremely complex architecture, where there
//     are numerous small sectors close together--so we try to avoid that
//     kind of architecture.
static POST_TABLE_PTR	gPosts;

// GWP gSpans is an array, one element for each screen coordinate y value.
//     It contains the beginning x coordinate.
//     For each post at an x location there is a span which is concatenated
//     all along the given y coordinate until it runs into the edge of the
//     screen or another post.
static SPAN				*gSpans;

// GWP UNUSED static ULONG			*gLastSpan;

// GWP gJustClosedSpan is an array, one element for each screen coordinate y
//     value. It contains the ending x coordinate and the sector. The sector
//     value doubles as a flag to indicate whether the span is closed or not.
//     the value is set to "ABSURD_SECTOR" to mean we closed and drew the
//     span.
static SPAN				*gJustClosedSpan;

static SHORT 			gSpanLighting[MAX_VIEW_HEIGHT * 2];	// used by init_spans

extern	BOOL	VideoCurrentlyRedirected;

/* ========================================================================
   Function    - allocate_spans
   Description - Allocate the spans arrays.
   Returns     -
   ======================================================================== */

void allocate_spans()
{
	gPosts = (POST_TABLE_PTR) zalloc(sizeof(POST_TABLE));
	memset(gPosts, 0,sizeof(POST_TABLE));
	
	gSpans = (SPAN *) zalloc((MAX_VIEW_HEIGHT + 10) * sizeof(SPAN));
	memset(gSpans, 0,(MAX_VIEW_HEIGHT + 10) * sizeof(SPAN));
	
	// GWP UNUSED gLastSpan = (ULONG *) zalloc(MAX_VIEW_HEIGHT * sizeof(ULONG));
	//memset(gLastSpan, 0,MAX_VIEW_HEIGHT * sizeof(ULONG));
	
	gJustClosedSpan = (SPAN *) zalloc(MAX_VIEW_HEIGHT * sizeof (SPAN));
	memset(gJustClosedSpan, 0 ,MAX_VIEW_HEIGHT * sizeof(SPAN));
	
//	memset(gSpanLighting, 0,MAX_VIEW_HEIGHT * 2 * sizeof(SHORT));
}

/* ========================================================================
   Function    - init_spans
   Description - create a lighting table for current render_center & render_height
   Returns     - void
   ======================================================================== */
void init_spans (void)
{
	LONG	ii;

//  X_RES_ADJ
//  Y_RES_ADJ

	for(ii=0; ii<(MAX_VIEW_HEIGHT*2); ++ii)
	{
		LONG	l;
		LONG const i = ii + render_center_y - MAX_VIEW_HEIGHT;

		if (i < render_center_y)
		{
//			l = render_center_y - i;				/* ceiling expression */
			l = (render_center_y - i)/2;			/* ceiling expression */
		}
		else
		{
			l = i - render_center_y;				/* floor expression */
		}

		if (l > 0)
		{
			gSpanLighting[ii] = (SHORT)(render_height / l);
		}
		else
		{
			gSpanLighting[ii] = 0;
		}
	}
}

/* ========================================================================
   Function    - clear_spans
   Description - clears the gSpans arrays. Must be called before doing a render.
   Returns     - void
   ======================================================================== */
void clear_spans (void)
{
	ULONG i;
	static LONG prev_camera_x = 0;
	static LONG prev_camera_y = 0;
	static LONG prev_camera_a = 0;
	static LONG prev_camera_p = 0;

	// for(i=0; i<MAX_VIEW_WIDTH; ++i)
	// 	last_post[i] = 0;
	memset(&last_post[0], 0, sizeof(last_post));	// replaces above for loop.

	// GWP UNUSED gLastSpan[i] = 0;
	// GWP UNUSED memset(&gLastSpan[0], 0, sizeof(gLastSpan));	// pulled out of lower for loop.
	
	for(i=0; i<MAX_VIEW_HEIGHT; ++i)
	{
		gJustClosedSpan[i].sector = ABSURD_SECTOR;
	}
	
	if (prev_camera_x != camera.x ||
	    prev_camera_y != camera.y ||
	    prev_camera_a != camera.a ||
	    prev_camera_p != camera.p)
	{
		// [d4-15-97 JPC] Need to make sure span_random_start is never larger
		// than width of texture squared, which varies with memory available.
		// Otherwise it's possible to blow up in draw_span when we try to
		// memcpy tex_mask - tex_start elements.  (tex_mask is the width squared
		// minus 1; tex_start is some value from span_random_start.)
		// Assumptions:
		// Low-res textures are 16 x 16 or bigger;
		// Medium-res are 32 x 32 or bigger;
		// High-res are 64 x 64 or bigger.
		if (fLowResTextures)
		{
			for (i = 0; i < (MAX_VIEW_HEIGHT + 10); ++i)
			{
				span_random_start[i] = random(256);
			}
		}
		else if (fMedResTextures)
		{
			for (i = 0; i < (MAX_VIEW_HEIGHT + 10); ++i)
			{
				span_random_start[i] = random(1024);
			}
		}
		else
		{
			for (i = 0; i < (MAX_VIEW_HEIGHT + 10); ++i)
			{
				span_random_start[i] = random(4096);
			}
		}
		prev_camera_x = camera.x;
		prev_camera_y = camera.y;
		prev_camera_a = camera.a;
		prev_camera_p = camera.p;
	}
}

/* ========================================================================
   Function    - add_span_event_start
   Description - ???
   Returns     - void
   ======================================================================== */
void add_span_event_start (ULONG x, ULONG yStart, ULONG yEnd, ULONG sector)
{
	POST *p;

	if (last_post[x] >= MAX_POSTS_PER_COL || x >= MAX_VIEW_WIDTH)
		return;
	
	if (yEnd < yStart)
		SWAPINT(yEnd,yStart);

	// Bound the y values.
	if (yStart >= MAX_VIEW_HEIGHT)
		yStart = MAX_VIEW_HEIGHT - 1;
		
	if (yEnd >= MAX_VIEW_HEIGHT)
		yEnd = MAX_VIEW_HEIGHT - 1;
		
	p = &(*gPosts)[last_post[x]++][x];
	p->y = yStart;
	p->ye = yEnd;
	p->sector = sector;
	p->type = SPAN_START;

}

/* ========================================================================
   Function    - add_span_event_end
   Description - ???
   Returns     - void
   ======================================================================== */
void add_span_event_end (ULONG x, ULONG yStart, ULONG yEnd, ULONG sector)
{
	POST *p;

	if (last_post[x] >= MAX_POSTS_PER_COL || x >= MAX_VIEW_WIDTH)
		return;
	
	if (yEnd < yStart)
		SWAPINT(yEnd,yStart);

	// Bound the y values.
	if (yStart >= MAX_VIEW_HEIGHT)
		yStart = MAX_VIEW_HEIGHT - 1;
		
	if (yEnd >= MAX_VIEW_HEIGHT)
		yEnd = MAX_VIEW_HEIGHT - 1;
		
	p = &(*gPosts)[last_post[x]++][x];
	p->y = yStart;
	p->ye = yEnd;
	p->sector = sector;
	p->type = SPAN_END;

}
/* ========================================================================
   Function    - draw_span
   Description - draw a floor based on data calculated in closeout_span.
					  This function is only called from closeout_span and
					  could be a part of it, but is separated to make the
					  function a more manageable size.
   Returns     - void
   ======================================================================== */
static void draw_span (
	LONG isx,
	LONG isy,
	LONG isx_end,
	UBYTE* tex,
	LONG cz,
	LONG tex_size,
	LONG floor_scale,
	ULONG tex_mask,
	LONG light,
	LONG HomogenousFlag,
	LONG tex_start)
{
	LONG		wx,wy,dx,dy;
	FIXED16		sx,sy;
	LONG		w;
	LONG		camera_offset;
#if WRC_CAMERA_TEST
	LONG		sptr_inc;
#endif
	//ULONG		tex_mask = (tex_size*tex_size)-1;
	PTR	   		sptr;
	POINT		a,b;

// return;

#if 0
//[d7-16-96 JPC] Move shading up into closeout_span so that we can
// adjust for distance BEFORE doing special effects.  This makes
// floors conform to other lighted objects.
	/* shade with distance */
	if (light)		/* if not brightest then darken */
		light += gSpanLighting[isy+MAX_VIEW_HEIGHT-render_center_y];
	if (light>DARKEST)
	{
		light = DARKEST;
	}
	else if (light<0)
	{
		light = 0;
	}
	SetLight(light);
#endif

#if WRC_CAMERA_TEST
	if (VideoCurrentlyRedirected)
	{
		sptr = (PTR)screen+(isx*screen_buffer_height)+isy;	/* pointer to start of span on screen */
		sptr_inc = screen_buffer_height;
	}
	else
#endif
	{
		sptr = (PTR)screen+(isy*screen_buffer_width)+isx;	/* pointer to start of span on screen */
#if WRC_CAMERA_TEST
		sptr_inc = 1;
#endif

	}

	/* we are at camera.x,camera.y. camera_offset is offset into texture */
	camera_offset = (CAMERA_INT_VAL(camera.y)*floor_scale)/UNITARY_SCALE
	   + (CAMERA_INT_VAL(camera.x)*floor_scale*tex_size)/UNITARY_SCALE;

	/* get map x,y from screen x,y */
	sy.lval = isy - render_center_y;			/* translate center to 0,0 */
	sx.lval = isx - render_center_x;
	if (sy.lval == 0)								/* prevent divide by zero */
		sy.lval = 1;
	/* reverse the proj() function, use 28.4bit fixed point math */
	//wy = (cz * render_perspect * floor_scale * 16) / (sy.lval * UNITARY_SCALE);
	wy = cz / (sy.lval * UNITARY_SCALE);
	wx = (sx.lval * wy);							/* wx,wy are map posn of span start */

	a.x = wy;
	a.y = wx / (render_perspect-1);
	rot8dbl(&a);

	b.x = wy;
	//wx += (wy << 16);
	b.y = (wx + (wy << 16))/ (render_perspect-1);
	rot8dbl(&b);

	/* diff is delta. convert 28.4 fixed to integer */
	dx = (a.x - b.x) >> 4;
	dy = (b.y - a.y) >> 4;

	/* switch from 28.4bit fixed point to 16.16bit */
	sx.lval = (camera_offset << 16) - (a.x << 12);
	sy.lval = (a.y << 12);

	w = isx_end - isx + 1;

	// Guarantee that ubCurrShade will be done on the first loop.
	if (light != DARKEST)
	{
		if (light == 0)
		{
			// GWP I noticed that from afar you can't tell the difference in
			//     a Homogenous texture from a scaled homogenous texture.
			//     So lets just memcpy as much of it as possible!
			if (HomogenousFlag)	// Random pattern textures are not scaled.
			{
				// This is done so the textures don't line up and cause
				// vertical strips in the floor.
				//LONG tex_start = ((sx.lval>>16) - ((sy.lval>>16)*tex_size)) & tex_mask;
				
				do {
					LONG const tex_drawn = tex_mask - tex_start;
					
					if (tex_drawn < w)
					{
						memcpy(sptr,&tex[tex_start],tex_drawn);
						w -= tex_drawn;
						sptr += tex_drawn;
						tex_start = 0;	// The next pass start at the beginning of the texture.
					}
					else
					{
						memcpy(sptr,&tex[tex_start],w);
						// GWP MARK END of Span  sptr += w;
						w = 0;
					}
				} while (w > 0);
				
			}
			else
			{
				do {
					// This lookup is 50% of the cost of this fn!
					// GWP I did an optimization that assumed that sy.lval>>16 and sy.lval>>16
					// GWP didn't change every loop. That was worse. (You'll notice it isn't
					// GWP still in the code!
					//*sptr = tex[ ((sx.lval>>16) - ((sy.lval>>16)*tex_size) ) & tex_mask];
					*sptr = tex[ (sx.TwoHalves.sHigh - (sy.TwoHalves.sHigh*tex_size) ) & tex_mask];
					sx.lval += dx;
					sy.lval += dy;
	#if WRC_CAMERA_TEST
					sptr += sptr_inc;
	#else	
					++sptr;
	#endif
				} while (--w);
			}
		}
		else
		{
			// GWP If you want a shaded texture, well you just have to do
			//     the lookup.
			do {
				//*sptr = shade[tex[ ((sx.lval>>16) - ((sy.lval>>16)*tex_size) ) & tex_mask]];
				*sptr = shade[tex[ (sx.TwoHalves.sHigh - (sy.TwoHalves.sHigh*tex_size) ) & tex_mask]];
				sx.lval += dx;
				sy.lval += dy;
#if WRC_CAMERA_TEST
				sptr += sptr_inc;
#else	
				++sptr;
#endif
			} while (--w);
		}
	}
	else
	{
#if WRC_CAMERA_TEST
		do {
			*sptr = BLACK;
			sptr += sptr_inc;
		} while (--w);
#else	
		// GWP If it's total darkness it doesn't matter what the texture
		//     or the shade table say, you are gonna see black!
		memset(sptr, BLACK, w);
#endif
	}
	// GWP This will draw white dots at the end of the floor span. Useful
	//     for debugging. (Be sure and uncomment the line above too.)
	// MARK THE END OF THE SECTOR *(sptr - 1) = WHITE;
}

/* ========================================================================
   Function    - draw_cspan
   Description - draw ceiling span
   Returns     - void
   ======================================================================== */
static void draw_cspan (
	LONG isx,
	LONG isy,
	LONG isx_end,
	UBYTE* tex,
	LONG cz,
	LONG tex_size,
	LONG floor_scale,
	ULONG tex_mask,
	LONG light)
{
	LONG		wx,wy,dx,dy;
	FIXED16		sx,sy;
	LONG		w;
	LONG		camera_offset;
#if WRC_CAMERA_TEST
	LONG		sptr_inc;
#endif
	// ULONG		tex_mask = (tex_size*tex_size)-1;
	PTR		sptr;
	POINT		a,b;
// return;
#if 0
// [d7-16-96 JPC] Move to closeout_span.
// Same reasoning as in draw_span.
	/* shade with distance */
	if (light)		/* if not brightest then darken */
		light += gSpanLighting[isy+MAX_VIEW_HEIGHT-render_center_y];

	// [d7-16-96 JPC] Note that the following wipes out effects such as infravision...
	if (light > DARKEST)
	{
		light = DARKEST;
	}
	else if (light < 0)
	{
		light = 0;
	}
	SetLight(light);
#endif

#if WRC_CAMERA_TEST
	if (VideoCurrentlyRedirected)
	{
		sptr = (PTR)screen+(isx*screen_buffer_height)+isy;	/* pointer to start of span on screen */
		sptr_inc = screen_buffer_height;
	}
	else
#endif
	{
		sptr = (PTR)screen+(isy*screen_buffer_width)+isx;	/* pointer to start of span on screen */
#if WRC_CAMERA_TEST
		sptr_inc = 1;
#endif
	}

	/* we are at camera.x,camera.y. camera_offset is offset into texture */
	camera_offset = (CAMERA_INT_VAL(camera.y)*floor_scale)/UNITARY_SCALE + (CAMERA_INT_VAL(camera.x)*floor_scale*tex_size)/UNITARY_SCALE
					 - 8*tex_size; //this is for a bug fix.. dunno why it has to
					 			   //have this extra substraction, but it works,
								   //so don't mess with it
					

	/* get map x,y from screen x,y */
	sy.lval = isy - render_center_y;			/* translate center to 0,0 */
	sx.lval = isx - render_center_x;
	if (sy.lval == 0)								/* prevent divide by zero */
		sy.lval = 1;
	/* reverse the proj() function, use 28.4bit fixed point math */
	//wy = (cz * render_perspect * floor_scale * 16) / (sy.lval * UNITARY_SCALE);
	wy = cz / (sy.lval * UNITARY_SCALE);
	wx = (sx.lval * wy);							/* wx,wy are map posn of span start */

	a.x = wy;
	a.y = wx / ((render_perspect)-1);
	rot8dbl(&a);

	b.x = wy;
	//wx += (wy << 16);
	b.y = (wx + (wy << 16))/ ((render_perspect)-1);
	rot8dbl(&b);

	/* diff is delta. convert 28.4fixed to integer */
	//dx = (b.x - a.x) / 16;
	dx = (a.x - b.x) / 16;
	dy = (b.y - a.y) / 16;

	/* switch a.x  & a.y from 28.4bit fixed point to 16.16bit */
	sx.lval = (camera_offset << 16) - (a.x << 12);
	//sx = camera_offset - (a.x << 12);
	sy.lval = (a.y << 12);

	w = isx_end - isx + 1;

	if (light != DARKEST)
	{
		if (light == 0)
		{
			do {
				//*sptr = tex[ ((sx.lval>>16) - ((sy.lval>>16)*tex_size) ) & tex_mask];
				*sptr = tex[ (sx.TwoHalves.sHigh - (sy.TwoHalves.sHigh*tex_size) ) & tex_mask];
				sx.lval += dx;
				sy.lval += dy;
#if WRC_CAMERA_TEST
				sptr += sptr_inc;
#else	
				++sptr;
#endif
			} while (--w);
		}
		else
		{
			do {
				//*sptr = shade[tex[ ((sx.lval>>16) - ((sy.lval>>16)*tex_size) ) & tex_mask]];
				*sptr = shade[tex[ (sx.TwoHalves.sHigh - (sy.TwoHalves.sHigh*tex_size) ) & tex_mask]];
				sx.lval += dx;
				sy.lval += dy;
#if WRC_CAMERA_TEST
				sptr += sptr_inc;
#else	
				++sptr;
#endif
			} while (--w);
		}
	}
	else
	{
	#if WRC_CAMERA_TEST
		do {
			*sptr = BLACK;
			sptr += sptr_inc;
		} while (--w);
	#else
		memset(sptr, BLACK, w);
	#endif
	}

}

/* ========================================================================
   Function    - draw_spans
   Description - Go through the entire gPosts matrix and concatenate
                 the spans. If a span is interrupted by a post it's closed. If
                 the next adjacent span is actually a continuation of the same
                 sector one pixel over, the span is re-opened. Otherwise if a
                 new span with new sector is started then the closed one is
                 drawn.
   Returns     - void
   ======================================================================== */
void draw_spans (void)
{
	ULONG x,y;

	for (x=0; x<(ULONG)screen_buffer_width; ++x)
	{
		ULONG i;
		const ULONG LastPostX = last_post[x];

#ifdef _JPC
		if (last_post[x] == 0)
			ASSERT (1);
#endif

		for (i=0; i<LastPostX; ++i)
		{
			POST const * const pPost = &(*gPosts)[i][x];
			ULONG const postYeVal = pPost->ye;
			ULONG const Sector = pPost->sector;
			
			if (pPost->type==SPAN_END)
			{
				for (y=(ULONG)pPost->y; y<=postYeVal; ++y)
				{
					//close_span(x,y,Sector);
					// GWP Unrolled close_span.
					if ( y >= MAX_VIEW_HEIGHT)
						break; // GWP y won't get any smaller! continue;
						
					// This test is in case we are trying to close a
					// span which is already closed but not drawn.
					if(gJustClosedSpan[y].sector != ABSURD_SECTOR)
						closeout_span(y);
				
					gJustClosedSpan[y].x = x;
					gJustClosedSpan[y].sector = Sector;
				}
			}
			else
			{
				// For every y coordinate between these y coordinates,
				// open a span.
				for (y=(ULONG)pPost->y; y<=postYeVal; ++y)
				{
					//open_span(x,y,Sector);
					// GWP Unrolled open_span.
					SPAN *s;
				
					// Validate for outragous data.
					if ( y >= MAX_VIEW_HEIGHT)
						break;	// GWP y won't get any smaller! continue;
						
					// If the span being opened is the same as the one
					// just closed one pixel over, just re-open the previous
					// one.
					if (gJustClosedSpan[y].x+1==(LONG)x  &&
						Sector==gJustClosedSpan[y].sector )
					{
						gJustClosedSpan[y].sector = ABSURD_SECTOR;
						continue;
					}
						
					s = &gSpans[y];
					
					// If the span isn't closed, close it and draw it
					// before re-using this element.
					if (s->status != SPAN_CLOSED)
						closeout_span(y);
				
					s->x = x;
					s->sector = Sector;
					s->status = SPAN_OPEN;
				}
			}
		}
	}
	
	// Now go through and draw any spans still left open.
	for (y=0; y<MAX_VIEW_HEIGHT; ++y)
	{
		if (gSpans[y].status == SPAN_CLOSED)
			continue;
			
		closeout_span(y);
	}
}

#if 0	// GWP Unrolled open_span.
/* ========================================================================
   Function    - open_span
   Description - ???
   Returns     - void
	JPC Note		- open_span can do one of two things:
						(1) nothing (if plot would be redundant?), or
						(2) FIRST call closeout_span and THEN set up the
							 gSpans array.
   ======================================================================== */
static void open_span (ULONG x, ULONG y, ULONG sector)
{
	SPAN *s;

	if ( y >= MAX_VIEW_HEIGHT)
		return;
		
	if ((LONG)sector==gJustClosedSpan[y].sector && gJustClosedSpan[y].x+1==(LONG)x)
	{
		gJustClosedSpan[y].sector = ABSURD_SECTOR;
		return;
	}
	
	// GWP On the first pass this is called without having set gSpans!
	closeout_span(y);

	s = &gSpans[y];
	s->x = x;
	s->sector = sector;
	s->status = SPAN_OPEN;
}
#endif

#if 0 // GWP Unrolled close_span.
/* ========================================================================
   Function    - close_span
   Description - ???
   Returns     - void
   ======================================================================== */
static void close_span (ULONG x, ULONG y, ULONG sector)
{
	if ( y >= MAX_VIEW_HEIGHT)
		return;
		
	if(gJustClosedSpan[y].sector != ABSURD_SECTOR)
		closeout_span(y);

	gJustClosedSpan[y].x = x;
	gJustClosedSpan[y].sector = sector;
}
#endif

/* ========================================================================
   Function    - closeout_span
   Description - ???
   Returns     - void
   ======================================================================== */
static void closeout_span (LONG y)
{
	LONG			i;
	static LONG		sect = ABSURD_SECTOR;
	static LONG		light;
	static UBYTE*	tex;
	static LONG		z;
	static LONG		size;
	static LONG		scale;
	static ULONG 	tex_mask;
	static ULONG 	c_tex_mask;
	static LONG		c_sect = ABSURD_SECTOR;
	static LONG		c_light;
	static UBYTE*	c_tex;
	static LONG		c_z;
	static LONG		c_size;
	static LONG		c_scale;
	static BOOL		c_Homogenous;

// [d8-01-96 JPC] Always do this--solves many lighting problems with the
// floors and ceilings at the cost of slower performance.
   sect   = ABSURD_SECTOR;             // [d7-02-96 JPC] force redraw every time
	c_sect = ABSURD_SECTOR;					// (Otherwise, texture and light may not change.)

	i = gJustClosedSpan[y].sector;
	if (i != ABSURD_SECTOR && i != LUDICROUS_SECTOR)
	{
		LONG	const x = gJustClosedSpan[y].x;
		LONG	xx = gSpans[y].x;

		if (xx > 639)
			xx = 639;

		if (x<640 && y<480 && xx <= x)
		{
			LONG	const newsect = i & 0x7fff;
			LONG	const fCeiling = i & 0x8000;
			
			if (fCeiling)
			{
				if(textures[sectors[newsect].c_flat].type == SKY_TEX)
					hscrape(xx, y, x);
				else
				{
					if (newsect != c_sect)
					{
						SHORT TextureIndex;
						BITMPTR			pTex;
						
						c_sect = newsect;
						c_light = sector_to_light(c_sect);
						// JPC Q: What about the distance factor here?  It seems
						// to work, but where is the code that does it?
						// A: It's handled in draw_cspan, below.
						// LATER: move the code from draw_cspan to here.
						// Adjustment for distance needs to happen before
						// other adjustments.
						// Shade with distance.
						if (c_light)		/* if not brightest then darken */
							c_light += gSpanLighting[y + MAX_VIEW_HEIGHT - render_center_y];
						
						if (c_light < 0)
							c_light = 0;
						else if (c_light > DARKEST)
							c_light = DARKEST;
							
						RenderChangeLight (c_light);
						SetLight (c_light);
						TextureIndex = textures[ sectors[c_sect].c_flat ].t;
						if (TextureIndex < 0)
							goto MemError;
						
						pTex = (BITMPTR) BLKPTR(TextureIndex);
						if (!IsPointerGood(pTex))
							goto MemError;
						
						c_tex = (UBYTE*)pTex + sizeof(BITMHDR);
						//GEH c_z = sector_to_ch(c_sect) + camera.z;
						c_z = sector_to_ch(c_sect) - camera.z;
						c_size = pTex->w;
						c_scale = pTex->scale;
						
						// moved here from draw_cspan.
						c_tex_mask = (c_size*c_size)-1;
						c_z *= render_perspect * c_scale * 16;
						//c_camera_offset = (camera.y*c_scale)/UNITARY_SCALE + (camera.x*c_scale*c_size)/UNITARY_SCALE;
						//c_camera_offset <<= 16;
//						printf("* ");
					}
//					printf("CSPAN - x:%ld->%ld  y:%ld  sect:%ld  tex:%ld\n",xx,x,y,sect,tex);
					draw_cspan(xx, y, x, c_tex, c_z, c_size, c_scale, c_tex_mask, c_light);
				}
			}
			else
			{
				// Floor.
				if (newsect != sect)
				{
					SHORT 	TextureIndex;
					BITMPTR	pTex;
					
					sect = newsect;
					light = sector_to_light(sect);
					// Move here from draw_span: shade with distance.
	                if (light)              // if not brightest then darken
						light += gSpanLighting[y + MAX_VIEW_HEIGHT - render_center_y];
						
					if (light < 0)
						light = 0;
					else if (light > DARKEST)
						light = DARKEST;
						
					RenderChangeLight (light);
					SetLight (light);
					TextureIndex = textures[ sectors[sect].f_flat ].t;
					if (TextureIndex < 0)
						goto MemError;
						
					c_Homogenous = textures[ sectors[sect].f_flat ].type == HOMOGENOUS_TEX;
					pTex = (BITMPTR) BLKPTR(TextureIndex);
					if (!IsPointerGood(pTex))
						goto MemError;
					tex = (UBYTE*)pTex + sizeof(BITMHDR);
					//GEH z = sector_to_fh(sect) + camera.z;
					z = sector_to_fh(sect) - camera.z;
					size = pTex->w;
					scale = pTex->scale;
					
					// moved here from draw_span.
					tex_mask = (size*size)-1;
					z *= render_perspect * scale * 16;
					//camera_offset = (CAMERA_INT_VAL(camera.y)*scale)/UNITARY_SCALE + (CAMERA_INT_VAL(camera.x)*scale*size)/UNITARY_SCALE;
					//camera_offset <<= 16;
//					printf("* ");
				}
//				printf(" SPAN - x:%ld->%ld  y:%ld  sect:%ld  tex:%ld  z:%ld\n",xx,x,y,sect,tex,z, light);
				draw_span(xx, y, x, tex, z, size, scale, tex_mask, light, c_Homogenous, span_random_start[y]);
			}
		}
MemError:
		gJustClosedSpan[y].sector = ABSURD_SECTOR;
		gSpans[y].status = SPAN_CLOSED;
	}
}


/* ======================================================================== */
