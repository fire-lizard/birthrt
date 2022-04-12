/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: SYSTEM.C
   Author: Alan Clark
   ========================================================================
   Contains the following internal functions:
   Exists					- Tests to see if a file exists in the current dir
   text_len					- finds the length of a string

   Contains the following general functions:
	SaveVideoSet			-
	RestoreVideoSet		-
   set_view_size			- sets the view size & position
   set_margin_size		-
   set_screen_size		-
	plot						-
	get_pixel				-
	line						-
	detect_scale			-

   gplot_char				- plots a character
   gtext_width				- calculates the width of a string
   gtext_height			- calculates the height of a string
   gprint_text				- prints a line
  	print_text_centered	- centers and prints a line
   print_textf				- allows formatting in gprint_text
   init_gfont				- load a font

	======================================================================== */

/* ®RM255¯ */
/*	------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <dos.h>
#include <io.h>
#include <conio.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <fcntl.h>
#include "system.h"
#include "sysint.h"
#include "engine.h"
#include "engint.h"
#include "machine.h"
#include "machint.h"
#include "Main.hxx"
#include "light.h"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
	prototypes
   ------------------------------------------------------------------------ */
extern void SetRedrawMainMapLevel (void);
extern void ZoomOut (LONG /* type */, LONG);
extern void MoveZoomMap (LONG xx, LONG yy);

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define SHADOW_COLOR			(0)
#define FONT_HEIGHT			5
#define SPACE_WIDTH			3
#define INTER_CHAR_SPACE	1
#define NORM_SCALE			(1<<8)
#define RATE				1
#define MAX_SCALE_RATE		(1<<7)

#define BLACK						1
#define WHITE						31
#define DKBLUE						60
#define BLUE						64
#define LTBLUE						79
#define DKPURPLE					90
#define LTPURPLE					103
#define DKRED						112
#define MDRED						120
#define LTRED						127
#define DKBROWN					128
#define MDBROWN					136
#define LTBROWN					140
#define ORANGE						142
#define MDYELLOW					168
#define LTYELLOW					175
#define DKGREEN					184
#define MDGREEN					188
#define LTGREEN					192
#define BRIGHTGREEN				199
#define MDTAN						213
#define LTTAN						223

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
typedef struct {
	CSTRPTR	filename;
	SHORT		size_points;
	SHORT		next_smaller;
	SHORT		lores_index;
} FONT_DATA;

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
LONG		print_global[5];   // set these for gprint_text "^Gn" directive
LONG		last_print_offset = 0;
LONG		last_font_used = 0;
LONG		desired_context_number = 0;

LONG		screen_buffer_width = MAX_VIEW_WIDTH;
LONG		screen_buffer_height = MAX_VIEW_HEIGHT;
LONG		window_width;
LONG		window_height;

LONG		margin_left = 0;
LONG		margin_right = 0;
LONG		margin_top = 0;
LONG		margin_bottom = 0;

LONG		render_width;
LONG		render_height;
LONG		render_perspect;
LONG		render_center_x;
LONG		render_center_y;
LONG		render_bot;
LONG		render_top;
//LONG		lens_factor = NORMAL_LENS*2;
LONG		lens_factor = NORMAL_LENS;
LONG		lens_view_width;

PTR			VideoStorage_screen;
LONG		VideoStorage_width;
LONG		VideoStorage_height;
LONG		VideoStorage_render_width;
LONG		VideoStorage_render_height;
LONG		VideoStorage_render_perspect;
LONG		VideoStorage_render_center_x;
LONG		VideoStorage_render_center_y;
LONG		VideoStorage_render_bot;
LONG		VideoStorage_render_top;

SHORT		origin_x = 0;
SHORT		origin_y = 0;
SHORT		clip_x = 640;
SHORT		clip_y = 480;

FONT_DATA font_info[] ={
/* 00 */	{"sans5.pcx"	, 5, -1			  ,FONT_SANS_5PT },		// FONT_SANS_5PT
/* 01 */	{"sans6.pcx"	, 6,FONT_SANS_5PT ,FONT_SANS_5PT },		// FONT_SANS_6PT
/* 02 */	{"sans8.pcx"	, 8,FONT_SANS_6PT ,FONT_SANS_8PT },		// FONT_SANS_8PT
/* 03 */	{"sans10.pcx"	,10,FONT_SANS_8PT ,FONT_SANS_6PT },		// FONT_SANS_12PT
/* 04 */	{"sans16.pcx"	,16,FONT_SANS_12PT,FONT_SANS_8PT },		// FONT_SANS_16PT
/* 05 */	{"ign8.pcx"		, 8,FONT_SANS_6PT ,FONT_TITL_8PT },		// FONT_TITL_8PT
/* 06 */	{"ign10.pcx"	,10,FONT_TITL_8PT ,FONT_TITL_10PT},		// FONT_TITL_10PT
/* 07 */	{"ign16.pcx"	,16,FONT_TITL_10PT,FONT_TITL_8PT },		// FONT_TITL_16PT
/* 08 */	{"ign20.pcx"	,20,FONT_TITL_16PT,FONT_TITL_10PT},		// FONT_TITL_20PT
/* 09 */	{"times7.pcx"	, 7,FONT_SANS_6PT ,FONT_SANS_5PT },		// FONT_TIMS_7PT
/* 10 */	{"times8.pcx"	, 8,FONT_TIMS_7PT ,FONT_SANS_5PT },		// FONT_TIMS_8PT
/* 11 */	{"times10.pcx"	,10,FONT_TIMS_8PT ,FONT_SANS_5PT },		// FONT_TIMS_10PT
/* 12 */	{"times30.pcx"	,30,FONT_TIMS_10PT,FONT_SANS_16PT},		// FONT_TIMS_30PT
/* 13 */	{"title40.pcx"	,40,FONT_TITL_20PT,FONT_SANS_16PT},		// FONT_TITL_40PT
/* 14 */	{"outln5.pcx"	, 5,-1,14},		//
/* 15 */	{"outln6.pcx"	, 6,14,14},		//
/* 16 */	{"outln7.pcx"	, 7,15,14},		//
/* 17 */	{"outln8.pcx"	, 8,16,14},		//
/* 18 */	{"outln10.pcx"	,10,17,14},		//
/* 19 */	{"outln30.pcx"	,30,18,18},		//
/* 20 */	{"magic40.pcx"	,40,-1,19},		//
/* 21 */	{"sans7.pcx"	, 8,FONT_SANS_6PT ,FONT_SANS_8PT }};	// FONT_SANS_7PT


SHORT		xCharWidth[256+32];
SHORT		xCharPosn[256+32];
SHORT		iCurFont = fERROR;
SHORT		wCurFont;
SHORT		hCurFont;
SHORT		yFontBase;
LONG		fBold = FALSE;
BOOL		fItalic = FALSE;
BOOL		fNonRemapped  = FALSE;
LONG		wrap_width = 9999;
LONG		wrap_height = 9999;
LONG		tab_width = (640 / 16);	// half-inch tabs, assuming 8" pagewidth
LONG		scale = NORM_SCALE;
LONG		inter_char_space = INTER_CHAR_SPACE;
SHORT		cur_type_and_size = 9999;
LONG		av[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,2};
char		cFraction[] = {0,91,92,93,94,96,123,124,125,126};

BOOL		fDoRemap = FALSE;
UBYTE	*	remap_table;
PTR		screen;

LONG		GameSpecificGlobal_type = -1;
LONG		GameSpecificGlobal_x = 0;
LONG		GameSpecificGlobal_y = 0;
LONG		GameSpecificGlobal_w = 0;
LONG		GameSpecificGlobal_h = 0;
BOOL		GameSpecificArray1[20]={FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE};
BOOL		GameSpecificArray2[20]={FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE};

static LONG ldMarginTop = 0;
static LONG ldMarginBottom = 0;
static LONG ldMarginLeft = 0;
static LONG ldMarginRight = 0;

static UBYTE	lighten[256] = {
/*			  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 */
/*	  4,*/  5,  6,  7,  8,  8,  9, 10, 11, 12, 13, 14, 14, 15, 16, 17, 18, 19, 20, 20, 21, 22, 23, 24, 25, 26, 26, 27, 28, 29, 30, 31, 31,
/*	 36,*/ 37, 38, 39, 40, 40, 41, 42, 43, 44, 44, 45, 46, 47, 48, 48, 49, 50, 51, 52, 52, 53, 54, 55, 55,
/*	 60,*/ 61, 62, 63, 64, 64, 65, 66, 67, 68, 68, 69, 70, 71, 72, 72, 73, 74, 75, 76, 76, 77, 78, 79, 79,
/*	 84,*/ 85, 86, 87, 88, 88, 89, 90, 91, 92, 92, 93, 94, 95, 96, 96, 97, 98, 99,100,100,101,102,103,103,
/*	108,*/109,110,111,112,112,113,114,115,116,116,117,118,119,120,120,121,122,123,124,124,125,126,127,127,
/*	132,*/133,134,135,136,136,137,138,139,140,140,141,142,143,144,144,145,146,147,148,148,149,150,151,151,
/*	156,*/157,158,159,160,160,161,162,163,164,164,165,166,167,168,168,169,170,171,172,172,173,174,175,175,
/*	180,*/181,182,183,184,184,185,186,187,188,188,189,190,191,192,192,193,194,195,196,196,197,198,199,199,
/*	204,*/205,206,207,208,208,209,210,211,212,212,213,214,215,216,216,217,218,219,220,220,221,222,223,223,
/*	228,*/229,230,231,232,232,233,234,235,236,236,237,238,239,240,240,241,242,243,244,244,245,246,247,247,
/*	248,*/249,250,251,252,253,254,255 };

extern UBYTE	antia_table[];

/* =======================================================================
   Function    - SaveVideoSet, RestoreVideoSet
   Description - saves and restores the view size & position
   Returns     - void
   ======================================================================== */
void SaveVideoSet (void)
{
	VideoStorage_screen				= screen;
	VideoStorage_width				= screen_buffer_width;
	VideoStorage_height				= screen_buffer_height;
	VideoStorage_render_width		= render_width;
	VideoStorage_render_height		= render_height;
	VideoStorage_render_perspect	= render_perspect;
	VideoStorage_render_center_x	= render_center_x;
	VideoStorage_render_center_y	= render_center_y;
	VideoStorage_render_bot			= render_bot;
	VideoStorage_render_top			= render_top;
}

void RestoreVideoSet (void)
{
	screen					= VideoStorage_screen;
	screen_buffer_width	= VideoStorage_width;
	screen_buffer_height	= VideoStorage_height;
	render_width			= VideoStorage_render_width;
	render_height			= VideoStorage_render_height;
	render_perspect		= VideoStorage_render_perspect;
	render_center_x		= VideoStorage_render_center_x;
	render_center_y		= VideoStorage_render_center_y;
	render_bot				= VideoStorage_render_bot;
	render_top				= VideoStorage_render_top;
}

/* =======================================================================
   Function    - set_view_size
   Description - sets the view size & position
   Returns     - void
   ======================================================================== */
void set_view_size (LONG vx,LONG vy,LONG vw,LONG vh)
{
	if (fHighRes)
	{
		--vw;
		--vh;
	}
	render_width=lens_view_width=vw;
	render_height=vh;
	render_perspect = (vw * lens_factor) / (2 * NORMAL_LENS);
	render_center_x=vx+(vw/2);
	render_center_y=vy+(vh/2)+((camera.p*vh)/480);
	render_bot=vy+vh;
	render_top=vy;
	init_spans();		// set spans tables for new size and aspect ratio
}

/* ======================================================================
   Function    - set_lens
   Description - sets the lens factor used in clipping and perspective projection
   Returns     - void
   ======================================================================== */
void set_lens (LONG lens)
{
//	if (lens < NORMAL_LENS)
//		lens = NORMAL_LENS;

//printf("set_lens - lens_factor: %d\n",lens);

	lens_factor = lens;
	render_perspect = (lens_view_width * lens_factor) / (2 * NORMAL_LENS);
}

void inc_lens (LONG dummy, LONG unused)
{
	set_lens( lens_factor+1 );
}

void dec_lens (LONG dummy, LONG unused)
{
	set_lens( lens_factor-1 );
}

/* =======================================================================
   Function    - set_margin_size, set_screen_size, get_margin_size
   Description - sets/gets the window size & position
   Returns     - void
   ======================================================================== */
void set_margin_size (LONG l, LONG r, LONG t, LONG b)
{
#if 0
	BOOL			fNeedRender = FALSE;

	if (l > margin_left || r > margin_right || t > margin_top || b > margin_bottom)
	{
		// [d10-27-96 JPC] Screen must have shrunk, get rid of the old
		// adventure screen margins by blanking out the entire width of the
		// screen down to the old margin_bottom.
		//GEH can't do this here. If it needs to be done, it must be
		// done by the calling game code.
		//memset(screen, BLACK, MAX_VIEW_WIDTH * (MAX_VIEW_HEIGHT - margin_bottom));
		//fNeedRender = TRUE;
	}
#endif
	margin_left = l;
	margin_right = r;
	margin_top = t;
	margin_bottom = b;
	set_view_size (margin_left, margin_top, window_width-margin_left-margin_right,	window_height-margin_top-margin_bottom);
	
	//if (fNeedRender)
	//{
	//	render_view (FALSE);
	//	ScreenCopy(0, 0, 0, MAX_VIEW_WIDTH, MAX_VIEW_HEIGHT - margin_bottom, SC_DEFAULT_RES);
	//}
}

void get_margin_size(LONG *Left, LONG *Right, LONG *Top, LONG *Bottom)
{
	*Left = margin_left;
	*Right = margin_right;
	*Top = margin_top;
	*Bottom = margin_bottom;
}

void set_screen_size (LONG w, LONG h)
{
#ifndef _WINDOWS
	screen_buffer_width = w;
	screen_buffer_height = h;
#endif
	window_width = w;
	window_height = h;

	// [d10-27-96 JPC] Auto-res work.  The low-res screen is exactly 1/2 the
	// size of the high-res screen, so we need to divide the margins by 2.
	if (fHighRes)
		set_view_size (margin_left, margin_top,
			window_width-margin_left-margin_right,
			window_height-margin_top-margin_bottom);
	else
		set_view_size (margin_left, margin_top,
			window_width-margin_left/2-margin_right/2,
			window_height-margin_top/2-margin_bottom/2);
}

void dec_view_size(LONG dummy1, LONG dummy2)
{
	if(ldMarginTop < 200)
	{
		ldMarginTop += 20;
		ldMarginBottom += 20;
		ldMarginLeft += 20;
		ldMarginRight += 20;
	}

	//GEH clear_display();
	memset(screen, 0, MAX_VIEW_WIDTH * MAX_VIEW_HEIGHT);
#if 0
|	// [d10-21-96 JPC] Auto-res work.
|	if (fHighRes)
|		set_margin_size(ldMarginTop, ldMarginRight, ldMarginTop, ldMarginBottom);
|	else
|		set_margin_size(ldMarginTop/2, ldMarginRight/2, ldMarginTop/2, ldMarginBottom/2);
#endif
}

void inc_view_size(LONG dummy1, LONG dummy2)
{
	if(ldMarginTop >= 20)
	{
		ldMarginTop -= 20;
		ldMarginBottom -= 20;
		ldMarginLeft -= 20;
		ldMarginRight -= 20;
	}

	clear_display();
	// [d10-21-96 JPC] Auto-res work.
	if (fHighRes)
		set_margin_size(ldMarginTop, ldMarginRight, ldMarginTop, ldMarginBottom);
	else
		set_margin_size(ldMarginTop/2, ldMarginRight/2, ldMarginTop/2, ldMarginBottom/2);
}

/* =======================================================================
   Function    - plot
   Description - plots a pixel
   Returns     - void
   ======================================================================== */
void plot (LONG x,LONG y,LONG c)
{
	if( (x>screen_buffer_width-1) || (x>clip_x-1) || x<0 || x<origin_x)
		return;
	if( (y>screen_buffer_height-1) || (y>clip_y-1) || y<0 || y<origin_y)
		return;
	screen[x+(y*screen_buffer_width)]=(char)c;
}

/* =======================================================================
   Function    - get_pixel
   Description - gets the value of a position on the screen
   Returns     - a long representing the value of the pixel
   ======================================================================== */
LONG get_pixel (LONG x,LONG y)
{
	if ( (x>screen_buffer_width-1) || x<0)
		return(0);
	if ( (y>screen_buffer_height-1) || y<0)
		return(0);
	return (LONG)(screen[x+(y*screen_buffer_width)]);
}

/* =======================================================================
   Function    - line
   Description - draws a line
   Returns     - void
   ======================================================================== */
void line (LONG x0, LONG y0, LONG x1, LONG y1,LONG color)
{
	LONG d,x,y,ax,ay,sx,sy,dx,dy;

	dx = x1 - x0;
	dy = y1 - y0;

	ax = ABS (dx) << 1;
	ay = ABS (dy) << 1;

	sx = SGN (dx);
	sy = SGN (dy);

	x = x0;
	y = y0;

	if (ax>ay)
	{
		/* X dominant */

		d = ay - (ax >> 1);
		while (TRUE)
		{
			plot (x,y,color);
			if (x == x1)
				break;
			if (d>=0)
			{
				y += sy;
				d -= ax;
			}
			x += sx;
			d += ay;
		}
	}
	else
	{
		/* Y dominant */

		d = ax - (ay >> 1);
		while (TRUE)
		{
			plot (x,y,color);
			if (y == y1)
				break;
			if (d>=0)
			{
				x += sx;
				d -= ay;
			}
			y += sy;
			d += ax;
		}
	}
}

/* =======================================================================
   ======================================================================== */
void dashedline (LONG x0, LONG y0, LONG x1, LONG y1, LONG color1 ,LONG color2)
{
	LONG d,x,y,ax,ay,sx,sy,dx,dy,cnt=0,color=color1;

	dx = x1 - x0;
	dy = y1 - y0;

	ax = ABS (dx) << 1;
	ay = ABS (dy) << 1;

	sx = SGN (dx);
	sy = SGN (dy);

	x = x0;
	y = y0;

	if (ax>ay)
	{
		/* X dominant */

		d = ay - (ax >> 1);
		while (TRUE)
		{
			plot (x,y,color); ++cnt; if (cnt>8) {color=(color==color1)?color2:color1;cnt=0;}
			if (x == x1)
				break;
			if (d>=0)
			{
				y += sy;
				d -= ax;
			}
			x += sx;
			d += ay;
		}
	}
	else
	{
		/* Y dominant */

		d = ax - (ay >> 1);
		while (TRUE)
		{
			plot (x,y,color); ++cnt; if (cnt>8) {color=(color==color1)?color2:color1;cnt=0;}
			if (y == y1)
				break;
			if (d>=0)
			{
				x += sx;
				d -= ay;
			}
			y += sy;
			d += ax;
		}
	}
}

/* =======================================================================
   Function    - detect_scale
   Description -
   Returns     - x center point in high 16 bits, scale in low 16 bits
   ======================================================================== */
#if 0
LONG	scale_factors[] = {
	1,								/* 5:1 walls, 500% normal size		(247) */
	3,								/* 5:3 walls, 166% normal size		(248) */
	UNITARY_SCALE,				/* 1:1 walls, doom resolution		(249) */
	UNITARY_SCALE*2,			/* 1:2 walls, half normal size		(250) */
	UNITARY_SCALE*4,			/* 1:4 walls, quarter normal size	(251) */

	UNITARY_PEOP_SCALE,		/* 1:1 people, doom resolution		(252) */
	UNITARY_PEOP_SCALE*2,	/* 1:2 people, half normal size		(253) */
	UNITARY_PEOP_SCALE*4,	/* 1:4 people, quarter normal size	(254) */
	UNITARY_PEOP_SCALE*8		/* 1:8 people, eighth normal size	(255) */
};
#else
LONG	scale_factors[] = {
	1,								/* 5:1 walls, 500% normal size		(247) */
	2,								/* 5:2 walls, 250% normal size		(248) */
	3,								/* 5:3 walls, 166% normal size		(249) */
	UNITARY_SCALE*2,			/* 1:2 walls, half normal size		(250) */
	UNITARY_SCALE*4,			/* 1:4 walls, quarter normal size	(251) */

	UNITARY_PEOP_SCALE,		/* 1:1 people, doom resolution		(252) */
	UNITARY_PEOP_SCALE*2,	/* 1:2 people, half normal size		(253) */
	UNITARY_PEOP_SCALE*4,	/* 1:4 people, quarter normal size	(254) */
	UNITARY_PEOP_SCALE*8		/* 1:8 people, eighth normal size	(255) */
};
#endif

ULONG detect_scale (SHORT t, SHORT h, SHORT w)
{
	BITMPTR	p;
	PTR		tptr;
	ULONG	x;
	ULONG	xx;
	ULONG	pix;

	p = (BITMPTR)BLKPTR(t);
	if (!IsPointerGood(p))
		return UNITARY_SCALE;					/* if error */

	tptr = (PTR)p + sizeof(BITMHDR);

	for (xx=x=0; x<w; ++x, xx+=h)
	{
		pix = tptr[xx];
		if (pix >= 247)
		{
			tptr[xx] = tptr[xx+h];			/* copy next pixel right */
			p->scale = scale_factors[pix-247];
			p->x_ctr_pt = x;
			return scale_factors[pix-247];;
		}
	}
	// if there is no scale bit, then set to neutral values.
	p->scale = UNITARY_SCALE;
	p->x_ctr_pt = 0;
	return UNITARY_SCALE;					/* default if no dot */
}


/* =======================================================================
   Function    - 
   Description - 
   Returns     - 
   ======================================================================== */
static unsigned char ForeignWinToASCII[] =
{	'€','','‚','ƒ','„','…','†','‡', 'ˆ','‰','Š','‹','Œ','','Ž','',
	'','‘','’','“','”','•','–','—', '˜','™','š','›','œ','','ž','Ÿ',
	' ','¡','¢','£','¤','¥','¦','§', '¨','©','ª','«','¬','­','-','-',
	'°','±','²','³','´','µ','¶','·', '¸','¹','º','»','¼','½','¾','¿',
	'À','Á','Â','Ã','Ž','Å','Æ','€', 'È','','Ê','Ë','Ì','Í','Î','Ï',
	'Ð','Ñ','Ò','Ó','Ô','Õ','™','×', 'Ø','Ù','Ú','Û','š','Ý','Þ','ß',
	'…',' ','ƒ','ã','„','å','æ','‡', 'Š','‚','ˆ','ë','','¡','î','ï',
	'ð','ñ','•','¢','ô','õ','”','ù', 'ø','—','£','–','','ý','þ',' ' };

ULONG ConvertWinForeignToASCII (ULONG win_char)
{
	return (ULONG) ForeignWinToASCII [win_char];
}

/* =======================================================================
   Function    - gtext_width_sub
   Description - finds the width of a string in the alt font
   Returns     - the width
   ======================================================================== */
LONG gtext_width_sub (char *txt, BOOL DoWrap)
{
	unsigned char * text = (unsigned char *)txt;
	LONG i, width, max_wid, c;
	LONG lastspace = -1;
	LONG fBoldLocal = fBold;
	SHORT	fDecimal = FALSE;

	if(text == NULL)
		return 0;

	max_wid = width = i = 0;

	while ((c=text[i++]))
	{
		if (c==' ') lastspace = i-1;
		

		if (c=='^')											/* handle commands */
		{
			c=text[i++];

			if (c == 'F')								/* change font */
			{
				LONG j;
				
				if (!isdigit(text[i]) ||
					!isdigit(text[i+1]))
					continue;
				j = (text[i++]-'0') * 10;
				j += (text[i++]-'0');
				init_gfont(j);
			}
			else if (c == 's')								/* inter-char space */
			{
				if (!isdigit(text[i]) ||
					!isdigit(text[i+1]))
					continue;
				inter_char_space = (text[i++]-'0') * 10;
				inter_char_space += text[i++]-'0';
			}
			else if (c == 'G')
			    i += 1;	
			else if (c=='Y')					/* Y## */
				i+=2;
			else if (c=='C' || c=='A' || c=='l' || c=='t' || c=='S' || c=='X' || c=='H' || c=='#')	/* C### or l### or S### */
				i+=3;
			else if (c == 'g' && text[i]=='0' && text[i+1]=='0')
			    i += 18;
			else if (c == 'g')
			    i += 10;
			else if (c=='B')								/* bold  */
				fBoldLocal = !fBoldLocal;
			else if (c == 'D')								/* decimal */
				fDecimal = TRUE;
		}

		else if ((c-32) >= 0)						/* printable characters */
		{
			if (c > 127)
			{
				c = ConvertWinForeignToASCII ( c-128 );
				c += 32;
			}

			// handle fractional numbers
			if (fDecimal == TRUE && c>='0' && c<='9')
			{
				++fDecimal;
				if (c=='0') goto zero;
			}

			if (fDecimal == TRUE+1 && (text[i]<'0' || text[i]>'9'))
			{
				fDecimal = FALSE;
				if (c == '0')
					continue;
				c = cFraction[c-'0'];
			}
zero:
			width = width + (((xCharWidth[c-32] + inter_char_space + fBoldLocal) * scale) / NORM_SCALE);
		}
		else if (c==13)								// carriage-return
		{
		}
		else if (c==10)								// linefeed (newline)
		{
			if (width > max_wid)
				max_wid = width;
			width=0;
			lastspace = -1;		// shouldn't wrap back across \n boundary
		}
		else if (c == 9)
		{
			SHORT const tabstop = width / tab_width;
			width = (tabstop + 1) * tab_width;
			lastspace = i-1;	// tabs are whitespace
		}

		// special code to suborn this function for word wrap
		if (DoWrap && wrap_width > 0 && width > wrap_width && lastspace > 0)
		{
			text[lastspace] = 10;			// change to return (\n)
			
			// GWP This prevents the next line from starting with a space
			// It doesn't really test for \r, just that its a non printable 
			// character.
			if (text[lastspace+1] == ' ')	// two spaces after punctuation
			    text[++lastspace] = '\r';	// a no-op after return
			    
			return (lastspace+1);
		}
	}

	if (width > max_wid)
		max_wid = width;

	if (wrap_width > 0)		// wordwrap terminates with 0
		return 0;

	return max_wid;
}


LONG gtext_width(char *text)
{
	inter_char_space = INTER_CHAR_SPACE;
	return gtext_width_sub(text,FALSE);
}

/* =======================================================================
   Function    - gtext_height
   Description - finds the height of a string in the alt font
   Returns     - the height
   ======================================================================== */
LONG gtext_height(char *txt)
{
	LONG i, height, c;
	char * text = txt;

	if(text == NULL)
		return 0;
	i = 0;
	wrap_height = -1;
	height = hCurFont;
	while ((c=text[i++]))
	{
		if (c==10)  /* handle return (\n) */
		{
			height += hCurFont;
			if (wrap_height	!= -1 && height + hCurFont > wrap_height)
			{
				last_print_offset = i;		// store value away
				last_font_used = cur_type_and_size;
				return height;
			}
		}

		if (c=='^')
		{
			c = text[i++];

				
			if (c == 'F')								/* change font */
			{
				LONG j;
				
				if (!isdigit(text[i]) ||
					!isdigit(text[i+1]))
					continue;
				j = (text[i++]-'0') * 10;
				j += (text[i++]-'0');
				init_gfont(j);
			}
//			else if (c=='s' || c=='Y')					/* Y## */
//				i+=2;
//			else if (c=='C' || c=='A' || c=='l' || c=='t' || c=='S' || c=='X')	/* C### or l### or S### */
//				i+=3;
//			else if (c=='B')								/* bold  */
//				fBoldLocal = !fBoldLocal;
//			else if (c == 'D')								/* decimal */
//				fDecimal = TRUE;

			if (c == 'W')								/* word wrapped */
			{
				LONG ii;
				
				if (!isdigit(text[i]) ||
					!isdigit(text[i+1]) ||
					!isdigit(text[i+2]))
					continue;
				wrap_width = (text[i++]-'0') * 100;
				wrap_width += (text[i++]-'0') * 10;
				wrap_width += (text[i++]-'0');

				for (ii=i; ii != 0; ii = gtext_width_sub(&text[ii],TRUE))
				{
				}
				//ii = i;
				//while ( (ii = gtext_width_sub(&text[ii])) )  // wrap is built into gtext_width
				//	ii = ii;		// dummy
				wrap_width = -1;		// turn it off
			}
			
			if (c == 'H')								/* height limited */
			{
				if (!isdigit(text[i]) ||
					!isdigit(text[i+1]) ||
					!isdigit(text[i+2]))
				{
#if defined(_DEBUG)
					fatal_error("ERROR: Bad height limit specfication %s\n", text);
#else
					return 0;
#endif
				}
				wrap_height = (text[i++]-'0') * 100;
				wrap_height += (text[i++]-'0') * 10;
				wrap_height += (text[i++]-'0');
			}
			if (c == '#')								/* context number */
			{
				SHORT j;
				
				j = (text[i++]-'0') * 100;
				j += (text[i++]-'0') * 10;
				j += (text[i++]-'0');
				
				if (desired_context_number == j)
					desired_context_number = 0;
			}
		}
	}
	last_font_used = cur_type_and_size;
	return height;
}

/* =======================================================================
   Function    - gprint_text
   Description - print a line of text in the alternate font
   Returns     -
   ======================================================================== */
SHORT gprint_text (LONG x, LONG y, char *text, LONG color)
{
	LONG	i=0, tmp=0, tmp2=0;
	ULONG	c;
	LONG	margin_x;
	LONG	angle_x;
	LONG	ascale_x, h_ascale_x, ascale_y, h_ascale_y;
	LONG	yb = 9999;
	LONG	le = 9999;
	LONG	max_h = 0;
	LONG	antialias = color * 256;
	LONG	antialias2 = color * 256;
	LONG	OrigColor = color;
	SHORT	yHeight = 0;
	BOOL	fAngle = FALSE;
	BOOL	fTranslucent = FALSE;
	BOOL	fUnderline = FALSE;
	SHORT	fDecimal = FALSE;
	SHORT	iBitm = fERROR;
	SHORT	iBkgnd= fERROR;
	PTR	pfont;

	if (iCurFont < 1)
		return 0;

	if(text == NULL)
		return 0;

	pfont = ((PTR)BLKPTR(iCurFont)) + sizeof(BITMHDR);
	if (!IsPointerGood(pfont))
		return 0;

	fBold = FALSE;
	fItalic = FALSE;
	fNonRemapped = FALSE;
	wrap_width = -1;
	wrap_height = -1;
	ascale_x = ascale_y = scale = NORM_SCALE;
	h_ascale_x = h_ascale_y = (NORM_SCALE * 2) / 3;
	inter_char_space = INTER_CHAR_SPACE;
	margin_x = x;

	while ((c = (unsigned char)text[i++]))
	{
		if (c == 10)						/* handle linefeed (\n) */
		{
			if (fUnderline) line(le,yb+1,x,yb+1,color);
			x = margin_x;
			y += max_h+1;
			yb += max_h+1;
			yHeight += max_h+1;
//			i++;

			++tmp;
			if (tmp==3 && tmp2) {x -= 5; margin_x = x;}
			if (tmp==4 && tmp2) {x -= 9; margin_x = x;}
			if (tmp==5 && tmp2) {x -= 10; margin_x = x;}

			if (!text[i]) 					/* if nothing after return */
				return yHeight+max_h+1;
			if (wrap_height	!= -1 && yHeight + max_h * 2 > wrap_height)
			{
				last_print_offset = i;		// store value away
				return yHeight+max_h+1;
			}
			max_h = 0;
			continue;
		}
		else if (c == 13)						// carriage-return
		{
			// assume linefeed comes with carriage-return
			continue;
		}
		else if (c == 9)							/* handle tab (\t) */
		{
			SHORT tabstop;
			tabstop = (x - margin_x) / tab_width;
			if (tabstop >= 0)	// don't mess with unlikely values
				x = margin_x + (tabstop + 1) * tab_width;
			continue;	
		}
		else if (c == '^')									/* handle commands */
		{
			c = text[i++];
			if (c == 'c')								/* center text */
			{
				x -= gtext_width_sub(&text[i],FALSE)/2;
				y -= (hCurFont*scale)/(NORM_SCALE*2);
			}
			else if (c == 'F')								/* change font */
			{
				LONG j;
				
				if (!isdigit(text[i]) ||
					!isdigit(text[i+1]))
				{
#if defined(_DEBUG)
					fatal_error("ERROR: Bad Font specfication %s\n", text);
#else
					return 0;
#endif
				}
				j = (text[i++]-'0') * 10;
				j += (text[i++]-'0');
				init_gfont(j);
				pfont = ((PTR)BLKPTR(iCurFont)) + sizeof(BITMHDR);
				if (yb != 9999)
					y = yb - yFontBase;
				last_font_used = j;
			}
			else if (c == 'C')								/* change color */
			{
				if (!isdigit(text[i]) ||
					!isdigit(text[i+1]) ||
					!isdigit(text[i+2]))
				{
#if defined(_DEBUG)
					fatal_error("ERROR: Bad Color specfication %s\n", text);
#else
					return 0;
#endif
				}
				color = (text[i++]-'0') * 100;
				color += (text[i++]-'0') * 10;
				color += (text[i++]-'0');
				if(color == 0)	// trap color zero to return to orig color
					color = OrigColor;
				antialias = color * 256;
			}
			else if (c == 'a')								/* change antialias color */
			{
				LONG j;
				
				if (!isdigit(text[i]) ||
					!isdigit(text[i+1]) ||
					!isdigit(text[i+2]))
				{
#if defined(_DEBUG)
					fatal_error("ERROR: Bad anti-alias specfication %s\n", text);
#else
					return 0;
#endif
				}
				j = (text[i++]-'0') * 100;
				j += (text[i++]-'0') * 10;
				j += (text[i++]-'0');
				antialias2 = j * 256;
			}
			else if (c == 'D')								/* print number as decimal */
			{
				fDecimal = TRUE;
			}
			else if (c == 'L')								/* line-up list */
			{
				LONG j = gtext_width_sub(&text[i],FALSE);
				x -= j;
				if (fAngle) {y -= j/2; angle_x = x;}
			}
			else if (c == 'l')								/* line-up list */
			{
				LONG j;
				
				if (!isdigit(text[i]) ||
					!isdigit(text[i+1]) ||
					!isdigit(text[i+2]))
				{
#if defined(_DEBUG)
					fatal_error("ERROR: Bad line-up specfication %s\n", text);
#else
					return 0;
#endif
				}
				j = (text[i++]-'0') * 100;
				j += (text[i++]-'0') * 10;
				j += (text[i++]-'0');
				x = margin_x + j;
				x -= gtext_width_sub(&text[i],FALSE);
			}
			else if (c == 't')								/* tab */
			{
				LONG j;
				
				if (!isdigit(text[i]) ||
					!isdigit(text[i+1]) ||
					!isdigit(text[i+2]))
				{
#if defined(_DEBUG)
					fatal_error("ERROR: Bad line-up specfication %s\n", text);
#else
					return 0;
#endif
				}
				j = (text[i++]-'0') * 100;
				j += (text[i++]-'0') * 10;
				j += (text[i++]-'0');
				x = margin_x + j;
			}
			else if (c == 'A')								/* print on angle */
			{
				fAngle = !fAngle;
				angle_x = x;
			}
			else if (c == 'T')								/* print translucent */
			{
				fTranslucent = (text[i++]-'0');
			}
			else if (c == 'U')								/* print underline */
			{
				fUnderline = !fUnderline;
				if (fUnderline==FALSE)			// just turned off
					line(le,yb+1,x,yb+1,color);
			}
			else if (c == 'B')								/* bold  */
			{
				fBold = !fBold;
			}
			else if (c == 'I')								/* italic  */
			{
				fItalic = !fItalic;
			}
			else if (c == 'N')								/* non color remapped */
			{
				fNonRemapped = !fNonRemapped;
			}
			else if (c == 'G')								 /* access Global */
			{				/* leave space in the string to fill in over! */
				char tmpbuf[15];
				LONG ii;

				if (text[i] < '1' || text[i] > '5')
				{
#if defined(_DEBUG)
					fatal_error("ERROR: Bad global access %s\n", text);
#else
					return 0;
#endif
				}
				ii = text[i] - '1';		// get global #, then go back to ^
				text[i--] = ' ';		// clear the number out
				text[i--] = ' ';		// clear the 'G' out
				sprintf(tmpbuf, "%d", print_global[ii]);
				memcpy(&text[i], tmpbuf, strlen(tmpbuf));  // no '\0' copy
			}
			else if (c == 'g')								 /* set Game Specific Global */
			{
				LONG ii;
				LONG xx;
				LONG yy;
				
				GameSpecificGlobal_type = -1;
				ii = (text[i++]-'0') * 10;
				ii += (text[i++]-'0');
				if (ii==0)
				{
					LONG j;
					
					++i;
					j = (text[i++]-'0') * 100;
					j += (text[i++]-'0') * 10;
					j += (text[i++]-'0');
					GameSpecificGlobal_x = j;
					++i;
					j = (text[i++]-'0') * 100;
					j += (text[i++]-'0') * 10;
					j += (text[i++]-'0');
					GameSpecificGlobal_y = j;
					++i;
					j = (text[i++]-'0') * 100;
					j += (text[i++]-'0') * 10;
					j += (text[i++]-'0');
					GameSpecificGlobal_w = j;
					++i;
					j = (text[i++]-'0') * 100;
					j += (text[i++]-'0') * 10;
					j += (text[i++]-'0');
					GameSpecificGlobal_h = j;

#if 0
					color_edged_rect(GameSpecificGlobal_x,GameSpecificGlobal_y,GameSpecificGlobal_w,GameSpecificGlobal_h,22);
#else
					if (GameSpecificGlobal_w > 400
						&& GameSpecificGlobal_h < 100)		iBkgnd = GetResourceStd("UI\\MARBLE05.PCX", FALSE);
					else if (GameSpecificGlobal_w > 400
						&& GameSpecificGlobal_h >= 100)		iBkgnd = GetResourceStd("UI\\MARBLE07.PCX", FALSE);
					else if (GameSpecificGlobal_w > 300
						&& GameSpecificGlobal_h <= 300)		iBkgnd = GetResourceStd("UI\\MARBLE06.PCX", FALSE);
					else if (GameSpecificGlobal_w > 300
						&& GameSpecificGlobal_h > 300)		iBkgnd = GetResourceStd("UI\\MARBLE10.PCX", FALSE);
					else if (GameSpecificGlobal_h > 350)	iBkgnd = GetResourceStd("UI\\MARBLE08.PCX", FALSE);
					else if (GameSpecificGlobal_h > 300)	iBkgnd = GetResourceStd("UI\\MARBLE01.PCX", FALSE);
					else if (GameSpecificGlobal_h > 270)	iBkgnd = GetResourceStd("UI\\MARBLE09.PCX", FALSE);
					else if (GameSpecificGlobal_h > 200)	iBkgnd = GetResourceStd("UI\\MARBLE02.PCX", FALSE);
					else if (GameSpecificGlobal_h > 150)	iBkgnd = GetResourceStd("UI\\MARBLE03.PCX", FALSE);
					else if (GameSpecificGlobal_h > 100)	iBkgnd = GetResourceStd("UI\\MARBLE04.PCX", FALSE);
					else 												iBkgnd = GetResourceStd("UI\\MARBLE06.PCX", FALSE);
					DrawBitmap(GameSpecificGlobal_x, GameSpecificGlobal_y, iBkgnd, 0, 0, 999, 999);
					SetPurge(iBkgnd);
					iBkgnd = fERROR;
#endif
					x = GameSpecificGlobal_x+8;		// position text
					y = GameSpecificGlobal_y+8;
					if (iBitm != fERROR)		// leave room for head
						wrap_width = GameSpecificGlobal_w-40;	// set word wrap
					else
						wrap_width = GameSpecificGlobal_w-16;	// set word wrap
					ii = i;
					tmp2 = 0;
					while (tmp = gtext_width_sub(&text[ii],TRUE))
					{
						ii += tmp;
						if (iBitm != fERROR)		// leave room for head
						{
							++tmp2;
							if (tmp2==3)
								wrap_width+=5;
							if (tmp2==4)
								wrap_width+=9;
							if (tmp2==5)
								wrap_width+=10;
						}
					}
					wrap_width = -1;		// turn it off

					if (iBitm != fERROR)		// draw head
					{
						x += 24;
						DrawBitmap(xx, yy, iBitm, 0, 0, 80, 96);
						SetPurge(iBitm);
						iBitm = fERROR;
					}
					margin_x = x;
				}

				if ((ii && ii<5) || (ii >= 30 && ii < 40))
				{
					LONG j;
					
					++i;
					j = (text[i++]-'0') * 100;
					j += (text[i++]-'0') * 10;
					j += (text[i++]-'0');
					xx = j;
					++i;
					j = (text[i++]-'0') * 100;
					j += (text[i++]-'0') * 10;
					j += (text[i++]-'0');
					yy = j;
					if (ii==1)
					{
						iBitm = GetResourceStd("UI\\HANDPTR1.PCX", FALSE);
						yy -= 56;
					}
					if (ii==2)
						iBitm = GetResourceStd("UI\\HEADCNSL.PCX", FALSE);
					if (ii==3)
					{
						iBitm = GetResourceStd("UI\\HANDPTR2.PCX", FALSE);
						yy -= 56;
						xx -= 60;
					}
					if (ii==4)
					{
						iBitm = GetResourceStd("UI\\HANDPTR3.PCX", FALSE);
					}
					
					if (ii==30)
					{
						iBitm = GetResourceStd("UI\\HEADADVN.PCX", FALSE);
						xx -= 12;
					}
					if (ii==31)
					{
						iBitm = GetResourceStd("UI\\HEADBATL.PCX", FALSE);
						xx -= 16;
					}
						
					if (ii!=2 && ii<30 && iBitm != fERROR)
					{
						DrawBitmap(xx, yy, iBitm, 0, 0, 64, 96);
						SetPurge(iBitm);
						iBitm = fERROR;
					}
				}

				if (ii==10)
				{
					LONG j;
					for (j=0; j<20; ++j)
					{
						if (GameSpecificArray1[j] != FALSE)
							SetRedrawMainMapLevel();
						GameSpecificArray1[j] = FALSE;
					}
				}

				if (ii==11)
				{
					LONG j;
					
					++i;
					j = (text[i++]-'0') * 100;
					j += (text[i++]-'0') * 10;
					j += (text[i++]-'0');
					if (GameSpecificArray1[j] != TRUE)
						SetRedrawMainMapLevel();
					GameSpecificArray1[j] = TRUE;
				}

				if (ii==12)
				{
					LONG j;
					for (j=0; j<20; ++j)
					{
						if (GameSpecificArray2[j] != FALSE)
							SetRedrawMainMapLevel();
						GameSpecificArray2[j] = FALSE;
					}
				}

				if (ii==13)
				{
					LONG j;
					
					++i;
					j = (text[i++]-'0') * 100;
					j += (text[i++]-'0') * 10;
					j += (text[i++]-'0');
					if (GameSpecificArray2[j] != TRUE)
						SetRedrawMainMapLevel();
					GameSpecificArray2[j] = TRUE;
				}

				if (ii==20)
					ZoomOut(0,0);

				if (ii==21)
				{
					LONG j;
					LONG xx;
					LONG yy;
					
					++i;
					j = (text[i++]-'0') * 100;
					j += (text[i++]-'0') * 10;
					j += (text[i++]-'0');
					xx = j;
					++i;
					j = (text[i++]-'0') * 100;
					j += (text[i++]-'0') * 10;
					j += (text[i++]-'0');
					yy = j;
					MoveZoomMap(xx,yy);
				}

				GameSpecificGlobal_type = ii;
			}
			else if (c == 'W')								/* word wrapped */
			{
				LONG ii;
				
				if (!isdigit(text[i]) ||
					!isdigit(text[i+1]) ||
					!isdigit(text[i+2]))
				{
#if defined(_DEBUG)
					fatal_error("ERROR: Bad word wrap specfication %s\n", text);
#else
					return 0;
#endif
				}
				wrap_width = (text[i++]-'0') * 100;
				wrap_width += (text[i++]-'0') * 10;
				wrap_width += (text[i++]-'0');
				ii = i;
				while (tmp = gtext_width_sub(&text[ii],TRUE))
				{
					ii += tmp;
				}
				//while ( (ii = gtext_width_sub(&text[ii])) )  // wrap is built into gtext_width
				//	ii = ii;		// dummy
				wrap_width = -1;		// turn it off
			}
			else if (c == 'H')								/* height limited */
			{
				if (!isdigit(text[i]) ||
					!isdigit(text[i+1]) ||
					!isdigit(text[i+2]))
				{
#if defined(_DEBUG)
					fatal_error("ERROR: Bad height limit specfication %s\n", text);
#else
					return 0;
#endif
				}
				wrap_height = (text[i++]-'0') * 100;
				wrap_height += (text[i++]-'0') * 10;
				wrap_height += (text[i++]-'0');
			}
			else if (c == 'n')								/* new page */
			{
				if (wrap_height != -1)  // only useful with ^H???
					yHeight = wrap_height;
			}
			else if (c == 's')								/* inter-char space */
			{
				if (!isdigit(text[i]) ||
					!isdigit(text[i+1]))
				{
#if defined(_DEBUG)
					fatal_error("ERROR: Bad inter char space specfication %s\n", text);
#else
					return 0;
#endif
				}
				inter_char_space = (text[i++]-'0') * 10;
				inter_char_space += text[i++]-'0';
			}
			else if (c == 'Y')								/* decrease Y coord */
			{
				if (!isdigit(text[i]) ||
					!isdigit(text[i+1]))
				{
#if defined(_DEBUG)
					fatal_error("ERROR: Bad decrease y coord. specfication %s\n", text);
#else
					return 0;
#endif
				}
				y -= (text[i++]-'0') * 10;
				y -= text[i++]-'0';
			}
			else if (c == 'S')								/* scale */
			{
				LONG j;
				LONG k;
				LONG l;
				
				if (!isdigit(text[i]) ||
					!isdigit(text[i+1]) ||
					!isdigit(text[i+2]))
				{
#if defined(_DEBUG)
					fatal_error("ERROR: Bad scale specfication %s\n", text);
#else
					return 0;
#endif
				}
				j = (text[i++]-'0') * 100;
				j += (text[i++]-'0') * 10;
				j += (text[i++]-'0');
				if (j==0) j = 1;
				k = font_info[cur_type_and_size].size_points * j;	// desired pt size x100
				l = (inter_char_space * 100) / font_info[cur_type_and_size].size_points;
TRY_SMALLER_FONT:
				scale = (NORM_SCALE * j) / 100;						// calc scale
				if (j < 100)	// scale %
				{
					LONG s = font_info[cur_type_and_size].next_smaller;	// next smaller font
					if (s >= 0 && (font_info[s].size_points*100) >= k) // -1 means none smaller
					{
						inter_char_space = (l * font_info[s].size_points) / 100;
						if (inter_char_space < 0) inter_char_space = 0;
						j = (k + (font_info[s].size_points/2)) / font_info[s].size_points;			// recalc %
						if (s==0) j = 100;								// minimum size
						init_gfont(s);
						pfont = ((PTR)BLKPTR(iCurFont)) + sizeof(BITMHDR);
						if (yb != 9999)
							y = yb - yFontBase;
						goto TRY_SMALLER_FONT;
					}
					if (cur_type_and_size != FONT_TIMS_30PT)	// make full size
						j = 100;
				}
				ascale_x = ascale_y = (NORM_SCALE * 100) / j;
				h_ascale_x = h_ascale_y = (ascale_x * 2) / 3;
			}
			else if (c == 'X')								/* x scale */
			{
				LONG j;
				
				if (!isdigit(text[i]) ||
					!isdigit(text[i+1]) ||
					!isdigit(text[i+2]))
				{
#if defined(_DEBUG)
					fatal_error("ERROR: Bad scale specfication %s\n", text);
#else
					return 0;
#endif
				}
				j = (text[i++]-'0') * 100;
				j += (text[i++]-'0') * 10;
				j += (text[i++]-'0');
				if (j==0) j = 1;

				scale = (NORM_SCALE * j) / 100;						// calc scale
				ascale_x = (NORM_SCALE * 100) / j;
				h_ascale_x = (ascale_x * 2) / 3;
				ascale_y = NORM_SCALE;
				h_ascale_y = (NORM_SCALE * 2) / 3;
			}
			else if (c == '#')								/* context number */
			{
				LONG j;
				
				j = (text[i++]-'0') * 100;
				j += (text[i++]-'0') * 10;
				j += (text[i++]-'0');
				
				if (desired_context_number == j)
					desired_context_number = 0;
			}

			continue;
		}

		// needed to print European characters
		if (c>127)
		{
			c = ConvertWinForeignToASCII(c-128);	// extended characters
			c += 32;											// offset in our fonts
		}

		// needed to print magic characters
		if (c<32) c+=128;									// extended characters

		if ((c>=32) && (c < (256+32)))						// printable character
		{
			LONG xe;
			LONG xh;
			PTR	fptr;
			LONG ye;
			
			// handle fractional numbers
			if (fDecimal == TRUE && c>='0' && c<='9')
			{
				++fDecimal;
				if (c=='0') goto zero;
			}

			if (fDecimal == TRUE+1 && (text[i]<'0' || text[i]>'9'))
			{
				fDecimal = FALSE;
				if (c == '0')
					continue;
				c = cFraction[c-'0'];
			}
zero:
			c = c - 32;
			if (x < le) le = x;
			xe = x + xCharWidth[c];							/* x end */
			xh = x + (xCharWidth[c]/2);
			ye = y + hCurFont - 1;							/* y end */
			if (yb == 9999)
				yb = y + yFontBase;
			if (max_h < (hCurFont-1))
				max_h = (hCurFont-1);

			/* set font pointer to start of char */
			fptr = pfont + ((LONG)xCharPosn[c] * (LONG)hCurFont) + 1L;	/* add start within font to ptr */

			if (scale == NORM_SCALE)
			{
				LONG xx;
				
				/* clip at region edge */
				if ( (xe>screen_buffer_width-1) || (xe>clip_x-1) || x<0 || x<origin_x ||
						(ye>screen_buffer_height-1) || (ye>clip_y-1) || y<0 || y<origin_y )
					goto skip;

				/* loop for x */
				for (xx=x; xx<xe; ++xx, ++fptr)
				{

					// print text at an angle
					if (fAngle)
					{
						LONG yy;
						PTR sptr = screen + ( (y+((xx-angle_x)/2)) * screen_buffer_width) + xx;		/* set screen pointer start */
						/* loop for y */
						for (yy=y; yy<ye; ++yy, ++fptr, sptr+=screen_buffer_width)
						{
							LONG pix = *fptr;
							
							if (pix)							/* check for transparent */
							{
								if (pix==31 && (yy&1)==0)
									*sptr = (char)color;
								else
									*sptr = antia_table[antialias+*sptr];
							}
							if (yy&1)
							{
								--sptr;
								if (pix)
									*sptr = antia_table[antialias+*sptr];
							}
						}
					}

					// print text translucent
					else if (fTranslucent)
					{
						LONG yy;
						PTR sptr = screen + (y * screen_buffer_width) + xx;		/* set screen pointer start */
						
						/* loop for y */
						for (yy=y; yy<ye; ++yy, ++fptr, sptr+=screen_buffer_width)
						{
							LONG pix = *fptr;
							
							if (pix)			/* check for transparent */
							{
								if (fTranslucent==1)
								{
									if (pix == 31)
										*sptr = antia_table[antialias+antia_table[antialias+*sptr]];
									else if (pix==8)
										*sptr = lighten[*sptr];
									else
										*sptr = antia_table[antialias+*sptr];
								}
								else if (fTranslucent==2)
								{
									if (pix == 31)
										*sptr = antia_table[antialias+*sptr];
									else if (pix==8)
										*sptr = lighten[*sptr];
									else
										*sptr = antia_table[(antia_table[antialias+*sptr]*256) + *sptr];
								}
								else if (fTranslucent==3 && pix!=8)
								{
									*sptr = antia_table[(antia_table[antialias+*sptr]*256) + *sptr];
								}
							}
						}
					}

					// print text bold
					else if (fBold)
					{
						LONG yy;
						PTR sptr = screen + (y * screen_buffer_width) + xx;		/* set screen pointer start */
						
						/* loop for y */
						for (yy=y; yy<ye; ++yy, ++fptr, sptr+=screen_buffer_width)
						{
							LONG pix = *fptr;							/* check for transparent */
							if (pix)							/* check for transparent */
							{
								if (pix==31)
									*sptr = *(sptr+1) = (char)color;
								else
								{
									if (xx<xh)
										*sptr = antia_table[antialias+*sptr];
									else
										*(sptr+1) = antia_table[antialias+*(sptr+1)];
								}
							}
						}
					}

					// print text non-color remapped
					else if (fNonRemapped)
					{
						LONG yy;
						PTR sptr = screen + (y * screen_buffer_width) + xx;		/* set screen pointer start */
						/* loop for y */
						for (yy=y; yy<ye; ++yy, ++fptr, sptr+=screen_buffer_width)
						{
							LONG pix = *fptr;
							if (pix)							/* check for transparent */
								*sptr = pix;
						}
					}

					// print text normal
					else
					{
						LONG yy;
						PTR sptr = screen + (y * screen_buffer_width) + xx;		/* set screen pointer start */
						/* loop for y */
						for (yy=y; yy<ye; ++yy, ++fptr, sptr+=screen_buffer_width)
						{
							LONG pix = *fptr;							/* check for transparent */
							if (pix)							/* check for transparent */
							{
								if (pix==31)
									*sptr = (char)color;
								else if (pix==8)
									//*sptr = antia_table[(antia_table[antialias2+*sptr]*256)+*sptr];
									*sptr = lighten[*sptr];
								else
									*sptr = antia_table[antialias+*sptr];
							}
						}
					}

				}		// loop for xx
			}

			/* --------------------- */
			/* draw scaled           */
			/* --------------------- */
			else
			{
				LONG xs;
				LONG xx;
				
				xe = x + (((xCharWidth[c]*NORM_SCALE)+h_ascale_x)/ascale_x);		/* x end */
				ye = y + ((((hCurFont-1)*NORM_SCALE)+h_ascale_y)/ascale_y);		/* y end */

				/* clip at region edge */
				if ( (xe>screen_buffer_width-1) || (xe>clip_x-1) || x<0 || x<origin_x ||
						(ye>screen_buffer_height-1) || (ye>clip_y-1) || y<0 || y<origin_y )
					goto skip;

				/* loop for x */
				for (xx=x,xs=0; xx<xe; ++xx, xs+=ascale_x)
				{
					LONG yy;
					LONG ys;
					PTR sptr = screen + (y * screen_buffer_width) + xx;		/* set screen pointer start */
					PTR fptr = pfont + ( ((LONG)xCharPosn[c]+((LONG)xs>>8)) * (LONG)hCurFont) + 1L;

					/* loop for y */
					for (yy=y,ys=0; yy<ye; ++yy, ys+=ascale_y, sptr+=screen_buffer_width)
					{
						LONG pix;
						
						if (scale > (NORM_SCALE/2))
							pix = fptr[ys>>8];
						else
						{
							LONG yt = ys>>8;
//							pix = av[fptr[yt]] + av[fptr[yt+hCurFont]] + av[fptr[yt+1]] + av[fptr[yt+hCurFont+1]];
//							if (pix < 3) pix = 0;
//							else if (pix > 5) pix = 31;
							pix = fptr[yt] + fptr[yt+hCurFont] + fptr[yt+1] + fptr[yt+hCurFont+1];
							if (pix < 48) pix = 0;
							else if (pix > 61) pix = 31;
						}

						if (pix)				/* check for transparent */
						{
							if (fTranslucent)
							{
								if (fTranslucent==1)
								{
									if (pix == 31)
										*sptr = antia_table[antialias+antia_table[antialias+*sptr]];
									else if (pix==8)
										*sptr = lighten[*sptr];
									else
										*sptr = antia_table[antialias+*sptr];
								}
								else if (fTranslucent==2)
								{
									if (pix == 31)
										*sptr = antia_table[antialias+*sptr];
									else if (pix==8)
										*sptr = lighten[*sptr];
									else
										*sptr = antia_table[(antia_table[antialias+*sptr]*256) + *sptr];
								}
								else if (fTranslucent==3 && pix!=8)
									*sptr = antia_table[(antia_table[antialias+*sptr]*256) + *sptr];
							}
							else if (fNonRemapped)
							{
								*sptr = pix;
							}
							else
							{
								if (pix==31)
									*sptr = (char)color;
								else if (pix==8)
									//*sptr = antia_table[(antia_table[antialias2+*sptr]*256)+*sptr];
									*sptr = lighten[*sptr];
								else
									*sptr = antia_table[antialias+*sptr];
							}
						}
					}
				}
			}

skip:
			/* advance to next avail x pos */
			x += (((xCharWidth[c] + inter_char_space + fBold) * scale)+(NORM_SCALE/2))/NORM_SCALE;
		}
	}

	if (fUnderline) line(le,yb+1,x,yb+1,color);

	return yHeight+max_h+1;
}

/* =======================================================================
   Function    - print_text_centered
   Description - centers and prints a line of text in the alt font
   Returns     - void
   ======================================================================== */
void print_text_centered(LONG x,LONG y, char * text,LONG color)
{
	if (text == NULL)
		return;
	print_textf(x-(gtext_width(text)/2),y-(hCurFont/2),color,text);
}

/* =======================================================================
   Function    - print_textf
   Description - prints a formatted line of text in the alt font
   Returns     - void
   ======================================================================== */
SHORT print_textf(LONG x, LONG y, LONG color, const char *format, ...)
{
	char texbuffer[2000];
	va_list argp;

	LONG	i = (LONG) format;

	if ( i <= 0 )				// prevent NULL or -1 strings from crashing
		return 0;

	va_start(argp, format);
	vsprintf(texbuffer,format,argp);
	va_end(argp);
	return gprint_text(x, y, texbuffer, color);
}

/* =======================================================================
   Function    - init_font
   Description - loads the alt font
   Returns     - void
   ======================================================================== */
void init_gfont (SHORT type_and_size)
{
	LONG		  	x;
	LONG		  	xO;
	LONG		  	y;
	//LONG			yO;
	LONG			ascii_val;
	SHORT			iFont;
	PTR			p;
	char			n[256+32];

// GWP Stop doing this as only fonts drawn on the screen need this not 
// GWP all fonts.

// GWP	if (!fHighRes)											// use stand-in if in lores
// GWP		type_and_size = font_info[type_and_size].lores_index;

	if (cur_type_and_size == type_and_size)		// don't try to reload current font
		return;
		
	sprintf(n,"%s%s",FONT_PATH,font_info[type_and_size].filename);
	iFont = GetResourceRotNoScale(n);							// load .PCX file for font
	if (iFont == fERROR)
		return;

	if (iCurFont != fERROR && iFont != iCurFont) 
	{
		SetPurge(iCurFont);	// purge old font
	}

	cur_type_and_size = type_and_size;				// set current font
	iCurFont = iFont;
	wCurFont = ((BITMPTR)BLKPTR(iCurFont))->h;
	hCurFont = ((BITMPTR)BLKPTR(iCurFont))->w;

//if(type_and_size==13)
//printf("FONT: %d %s - w:%d h:%d\n",iCurFont,n,wCurFont,hCurFont);

	p = ((PTR)BLKPTR(iCurFont)) + sizeof(BITMHDR);

	for (y=0; y<hCurFont; ++y)				/* scan for font base */
		if (p[y])								/* check pixel */
			yFontBase = y;

	for (ascii_val=xO=x=0; x<wCurFont && ascii_val<(256+32); ++x)	/* scan for character positions */
	{
		if (p[x*hCurFont])					/* check pixel */
		{
			if (ascii_val)
				xCharWidth[ascii_val-1] = (SHORT)(x - xO);
			xCharPosn[ascii_val] = (SHORT)x;

//if(type_and_size==13)
//printf("%c:%d ",ascii_val+32,x);

			xO = x;
			++ascii_val;
		}
	}
//if(type_and_size==13)
//printf("\n");

}



/* ========================================================================
   Function    -
   Description -
   Returns     - void
   ======================================================================== */
void ClearRemapTable (void)
{
	remap_table = &shade_table[0];
	fDoRemap = FALSE;
}

/* ====================================================================
	
	==================================================================== */
void SetRemapTable (USHORT table)
{
	remap_table = &shade_table[256 * (REMAP_TABLE_OFFSET + (32 * table))];
	fDoRemap = TRUE;
}

/* ========================================================================
   Function    - BltBitmap
   Description - Just copy the bitmap to the screen buffer the fastest way.
                 NO TRANSPARENCY
   Returns     - 
   ======================================================================== */
void BltBitmap (SHORT x, SHORT y, SHORT iBitm, SHORT bx, SHORT by, SHORT w, SHORT h)
{
	LONG	xx, yy;
	LONG	wid, hei;
	BITMPTR	p;
	PTR		bptr;
	PTR		sptr;
	BOOL	fRotated = FALSE;
	LONG	widMinusW;

	if (iBitm == fERROR)							// check for bad source bitmap
		return;

	p = (BITMPTR) BLKPTR(iBitm);				// get pointer and size
	if (!IsPointerGood(p))
		return;

	bptr = (PTR)p + sizeof(BITMHDR);
	wid = p->w;
	hei = p->h;

	// negative height is used as a flag to say that bitm needs to be rotated
	if (h < 0)
	{
		fRotated = TRUE;
		h = -h;
		y = y + h - wid;		// put feet on bottom of passed area (y+h)
	}

	x -= p->x_ctr_pt;		// handle center point;

	/* clipping */
	if (x>screen_buffer_width-1 || x+w-1<0 || y>screen_buffer_height-1 || y+h-1<0)
		return;

	if (x+w-1 > screen_buffer_width-1)
		w = screen_buffer_width - x - 1;

	if (y+h-1 > screen_buffer_height-1)
		h = screen_buffer_height - y - 1;

	if (fRotated)
	{
		// fix dimensions by swapping
		ULONG const temp = w;
		
		w = h;
		h = temp;
	}

	if (w > wid)
		w = wid;
	if (h > hei)
		h = hei;

	if (x < 0)
	{
		w += x;
		bx -= x;
		x = 0;
	}

	if (y < 0)
	{
		h += y;
		by -= y;
		y = 0;
	}

	bptr += (by * wid) + bx;
	sptr=(PTR)screen+((y*screen_buffer_width)+x);		// set destination pointer

	// Computed for optimization.
	widMinusW = wid - w;
	
	if (fRotated)
	{
		// bitm needs to be rotated
		for (yy=0; yy < h; ++yy)				// copy bitmap
		{
			PTR sptr_ = sptr + yy;
			for (xx=0; xx < w; ++xx)
			{
				*sptr_ = *bptr++;

				sptr_ += screen_buffer_width;				// next pixel in column
			}
			bptr += widMinusW;
		}
	}

	// not rotated (i.e. normal)
	else
	{
		if (w == screen_buffer_width)
		{
			memcpy(sptr, bptr, (w * h));
		}
		else
		{
			for (yy=0; yy < h; ++yy)				// copy bitmap
			{
				memcpy(sptr, bptr, w);
				sptr += screen_buffer_width;			// move to start of next scanline
				bptr += wid;
			}
		}
	}
}

/* ========================================================================
   Function    - DrawBitm
   Description - Does not autoscale the art but does transparency
   Returns     - 
   ======================================================================== */
void DrawBitm (SHORT x, SHORT y, SHORT iBitm, SHORT bx, SHORT by, SHORT w, SHORT h)
{
	LONG		xx, yy;
	LONG		wid, hei;
	BITMPTR	p;
	PTR		bptr;
	PTR		sptr;
	BOOL		fRotated = FALSE;
	BOOL		fMirror = FALSE;
	LONG		widMinusW;

	if (iBitm == fERROR)							// check for bad source bitmap
		return;

	p = (BITMPTR) BLKPTR(iBitm);				// get pointer and size
	if (!IsPointerGood(p))
		return;

	bptr = (PTR)p + sizeof(BITMHDR);
	wid = p->w;
	hei = p->h;

	// negative width is used as a flag to say that bitm needs to be mirrored
	if (w < 0)
	{
		fMirror = TRUE;
		w = -w;
	}

	// negative height is used as a flag to say that bitm needs to be rotated
	if (h < 0)
	{
		fRotated = TRUE;
		h = -h;
		y = y + h - wid;		// put feet on bottom of passed area (y+h)
	}

	/* clipping */
	if (x>screen_buffer_width-1 || x+w-1<0 || y>screen_buffer_height-1 || y+h-1<0)
		return;

	if (x+w-1 > screen_buffer_width-1)
		w = screen_buffer_width - x - 1;

	if (y+h-1 > screen_buffer_height-1)
		h = screen_buffer_height - y - 1;

	if (fRotated)
	{
		// fix dimensions by swapping
		ULONG const temp = w;
		
		w = h;
		h = temp;
	}

	if (w > wid)
		w = wid;
	if (h > hei)
		h = hei;

	if (x < 0)
	{
		w += x;
		bx -= x;
		x = 0;
	}

	if (y < 0)
	{
		h += y;
		by -= y;
		y = 0;
	}

	bptr += (by * wid) + bx;
	sptr=(PTR)screen+((y*screen_buffer_width)+x);		// set destination pointer

	// Computed for optimization.
	widMinusW = wid - w;
	
	if (fRotated)
	{
		if (fDoRemap)
		{
			// bitm needs to be rotated
			for (yy=0; yy < h; ++yy)				// copy bitmap
			{
				PTR sptr_ = sptr + yy;
				for (xx=0; xx < w; ++xx)
				{
					UBYTE const pix = *bptr;
					if (pix)				// check for transparency
						*sptr_ = remap_table[pix];
					++bptr;
					sptr_ += screen_buffer_width;				// next pixel in column
				}
				bptr += widMinusW;
			}
		}
		else
		{
			// bitm needs to be rotated
			for (yy=0; yy < h; ++yy)				// copy bitmap
			{
				PTR sptr_ = sptr + yy;
				for (xx=0; xx < w; ++xx)
				{
					UBYTE const pix = *bptr;
					if (pix)				// check for transparency
						*sptr_ = pix;
					++bptr;
					sptr_ += screen_buffer_width;				// next pixel in column
				}
				bptr += widMinusW;
			}
		}
	}

	// mirrored, not rotated (i.e. normal)
	else if (fMirror)
	{
		LONG const screen_buffer_widthPlusW = screen_buffer_width + w;
		sptr += w-1;

		if (fDoRemap)
		{
			for (yy=0; yy < h; ++yy)				// copy bitmap
			{
				for (xx=0; xx < w; ++xx)
				{
					UBYTE const pix = *bptr;
					if (pix)				// check for transparency
						*sptr = remap_table[pix];
					++bptr;
					--sptr;
				}
				sptr += screen_buffer_widthPlusW;
				bptr += widMinusW;
			}
		}
		else
		{
			for (yy=0; yy < h; ++yy)				// copy bitmap
			{
				for (xx=0; xx < w; ++xx)
				{
					UBYTE const pix = *bptr;
					if (pix)				// check for transparency
						*sptr = pix;
					++bptr;
					--sptr;
				}
				sptr += screen_buffer_widthPlusW;
				bptr += widMinusW;
			}
		}
	}

	// not rotated (i.e. normal)
	else
	{
		LONG const screen_buffer_widthMinusW = screen_buffer_width - w;
		
		if (fDoRemap)
		{
			for (yy=0; yy < h; ++yy)				// copy bitmap
			{
				for (xx=0; xx < w; ++xx)
				{
					UBYTE const pix = *bptr;
					if (pix)				// check for transparency
						*sptr = remap_table[pix];
					++bptr;
					++sptr;
				}
				sptr += screen_buffer_widthMinusW;
				bptr += widMinusW;
			}
		}
		else
		{
			for (yy=0; yy < h; ++yy)				// copy bitmap
			{
				for (xx=0; xx < w; ++xx)
				{
					UBYTE const pix = *bptr;
					if (pix)				// check for transparency
						*sptr = pix;
					++bptr;
					++sptr;
				}
				sptr += screen_buffer_widthMinusW;
				bptr += widMinusW;
			}
		}
	}


}

/* ====================================================================
	
	==================================================================== */
void DrawBitmap (SHORT x, SHORT y, SHORT iBitm, SHORT bx, SHORT by, SHORT w, SHORT h)
{
	BITMPTR	p;

	if (iBitm == fERROR)							// check for bad source bitmap
		return;

	p = (BITMPTR) BLKPTR(iBitm);				// get pointer and size
	if (!IsPointerGood(p))
		return;

	if (p->scale != UNITARY_SCALE)
	{
		ScaleBitmap(x, y, iBitm, bx, by, w, h, FULL_SCALE);
		return;
	}

	x -= p->x_ctr_pt;		// handle center point;

	// draw without scale
	DrawBitm(x, y, iBitm, bx, by, w, h);
}

/* ====================================================================
	
	==================================================================== */
SHORT SaveBitmap (SHORT x, SHORT y, SHORT w, SHORT h)
{
	SHORT    iBitm;
	BITMPTR  pBitm;
	//LONG	xx;
	LONG	yy;
	PTR		bptr;
	PTR		sptr;

//	DirectDrawSaveScreen();

	iBitm = OpenBitm(w, h);
	if (iBitm == fERROR)							// check for bad source bitmap
		return fERROR;

	pBitm = (BITMPTR) BLKPTR(iBitm);				// get pointer and size
	if (!IsPointerGood(pBitm))
		return fERROR;

	bptr = (PTR)pBitm + sizeof(BITMHDR);

	sptr = screen + (y * screen_buffer_width) + x;

	// Be sure that we are within the bounds of the screen.
	if ((y + h) > screen_buffer_height)
	{
		h = screen_buffer_height - y;
	}
	
	if ((x + w) > screen_buffer_width)
	{
		w = screen_buffer_width - x;
	}

	for (yy=0; yy < h; ++yy)				// copy bitmap
	{
		//for (xx=0; xx < w; ++xx)
		//{
		//	*bptr++ = *sptr++;
		//}
		//sptr += (screen_buffer_width - w);			// move to start of next scanline
		
		memcpy(bptr, sptr, w);
		sptr += screen_buffer_width;
		bptr += w;
	}

//	DirectDrawPreFrame();

	return iBitm;
}

/* ====================================================================
	
	==================================================================== */

void ZoomBitmap (SHORT x, SHORT y, SHORT iBitm, SHORT bx, SHORT by, SHORT w, SHORT h,
					  SHORT hDest, POINT start, POINT bck, SHORT bckw, SHORT bckh)
{
	SHORT scale_rate = MAX_SCALE_RATE;
	SHORT tmpX, tmpY;
	while(scale_rate >= 1)
	{

		if(hDest != fERROR)
			DrawBitmap(bck.x, bck.y, hDest, 0, 0, bckw, bckh);
		
		tmpX=(SHORT)start.x + (x-(SHORT)start.x)/scale_rate;
		tmpY=(SHORT)start.y + (y-(SHORT)start.y)/scale_rate;
		ScaleBitmap(tmpX,
						tmpY,
						iBitm, bx, by, w, h, FULL_SCALE*scale_rate);
		scale_rate = scale_rate>>RATE;
		update_screen();
		DirectDrawPreFrame();
		run_timers();
	}
}	

void ZoomOutBitmap (SHORT x, SHORT y, SHORT iBitm, SHORT bx, SHORT by, SHORT w, SHORT h,
						  SHORT hDest, POINT start, POINT bck, SHORT bckw, SHORT bckh)
{
	SHORT scale_rate=1;
	SHORT tmpX, tmpY;
	// zoom out a menu
	while(scale_rate <= MAX_SCALE_RATE)
	{

		if(hDest != fERROR)
			DrawBitmap (bck.x, bck.y, hDest, 0, 0, bckw, bckh);
		
		tmpX=(SHORT)start.x + (x-(SHORT)start.x)/scale_rate;
		tmpY=(SHORT)start.y + (y-(SHORT)start.y)/scale_rate;

	  	ScaleBitmap(tmpX,
	  					tmpY,
	  					iBitm, bx, by, w, h, FULL_SCALE*scale_rate);
 	  	scale_rate = scale_rate<<RATE;
  	
		update_screen();
		DirectDrawPreFrame();
		run_timers();
	}
	if(hDest != fERROR)
		DrawBitmap (bck.x, bck.y, hDest, 0, 0, bckw, bckh);
	update_screen();
	DirectDrawPreFrame();
}	
/* ====================================================================
	
	==================================================================== */
#define FULL_SCALE_FINE		(FULL_SCALE*256)
#define SCALE_SHIFT_FINE	(16)

static void ScaleBitmap_fine (SHORT x, SHORT y, SHORT iBitm, SHORT bx, SHORT by, SHORT w, SHORT h, LONG Xscale, LONG Yscale)
{
	LONG		xx, yy;
	LONG		i;
	LONG 		startX, startY;
	LONG		wid, hei, ww, hh, x_offset;
	FIXED16		tsx, tsy;
	LONG		xinc, yinc;
	LONG		tmpY;
	BOOL		fRotated = FALSE;
	BOOL     fMirror = FALSE;
	BITMPTR	p;
	PTR		tptr, bptr;
	PTR		sptr, sptr_;
	BYTE		pix;

	startX = x;
	startY = y;
	if (iBitm == fERROR)							// check for bad source bitmap
		return;

	p = (BITMPTR) BLKPTR(iBitm);				// get pointer to bitm header
	if (!IsPointerGood(p))
		return;

	tptr = (PTR)p + sizeof(BITMHDR);			// get pointer to data
	wid = p->w;										// get size
	hei = p->h;

	Xscale = (p->scale * Xscale) / UNITARY_SCALE;	// combo scale
	Yscale = (p->scale * Yscale) / UNITARY_SCALE;	// combo scale

	//GEH fix for a divby0 crash
	if(Xscale == 0 || Yscale == 0)
		return;

	ww = (wid * FULL_SCALE_FINE) / ABS(Xscale);			// scaled size
	hh = (hei * FULL_SCALE_FINE) / ABS(Yscale);
	x_offset = (p->x_ctr_pt * FULL_SCALE_FINE) / ABS(Xscale);  // scaled ctr point
	xinc = ABS(Xscale);					// set independant x & y scales
	yinc = ABS(Yscale);					// set independant x & y scales


	// negative width is used as a flag to say that bitm needs to be mirrored
	if (w < 0)
	{
		fMirror = TRUE;
		w = -w;
	}

	// negative height is used as a flag to say that bitm needs to be rotated
	if (h < 0)
	{
		fRotated = TRUE;
		h = -h;
		tmpY = y;
		y = y + h - ww;		// put feet on bottom of passed area (y+h)
		if(y < tmpY)	// bitmap height is greater than the window height
			h += tmpY - y;		// ajust the "window" height to fit the bitmap
	}

	// handle center point;
	if (x_offset && x_offset <= (w/2))
	{
		if (!fMirror)
			x -= x_offset;
		else
			x -= hh - x_offset;			// slightly different if mirrored
	}

	if (x_offset && x_offset > (w/2))			// clipping for center-pointed bitmaps
	{
		x -= w/2;
		i = x_offset - (w/2);
		i = (i * ABS(Xscale)) / FULL_SCALE_FINE;
		if (fRotated)
			by += i;
		else
			bx += i;
	}


	/* clipping */
	if (x>screen_buffer_width-1 || x+w-1<0 || y>screen_buffer_height-1 || y+h-1<0)
		return;

	if (x+w-1 > screen_buffer_width-1)
		w = screen_buffer_width - x - 1;

	if (y+h-1 > screen_buffer_height-1)
		h = screen_buffer_height - y - 1;

	if (fRotated) {tsy.lval = ww; ww = hh; hh = tsy.lval;}			 // fix dimensions

	// clippin for center-pointed bitmap with graphic width less than the window width after previous clipping.		
	if (x_offset && x_offset > (w/2) && ((ww-x_offset+(w/2)) < w))
		w = ww- x_offset+( w/2);	
	else if (w > ww)					 // ww, w are width
		w = ww;					 // hh, h are height
	else if (x_offset && x_offset < (w/2))
		w = w/2 + x_offset;

	if (h > hh)
		h = hh;

	if (x < 0)
	{
		w += x;
		//bx -= x;
		bx -= (x * ABS(Xscale)) / FULL_SCALE_FINE;
		//x = startX;
		x = 0;
	}

	if (y < 0)
	{
		h += y;
		//by -= y;
		by -= (y * ABS(Yscale)) / FULL_SCALE_FINE;
		//y = startY;
		y = 0;
	}

	if (x+w-1 > screen_buffer_width-1)
		w = screen_buffer_width - x - 1;

	if (y+h-1 > screen_buffer_height-1)
		h = screen_buffer_height - y - 1;

	tptr += (by * wid) + bx;					// adj point by start coords
	sptr=(PTR)screen+((y*screen_buffer_width)+x);		// set destination pointer

	if (fRotated)			// check for rotated
	{
		if (!fMirror)
		{
			if (fDoRemap)
			{
				// bitm needs to be rotated
				tsx.lval = 0;
				for (xx=0; xx < w; ++xx)				// copy bitmap
				{
					sptr_ = sptr + xx;
					//bptr = &tptr[(tsx>>SCALE_SHIFT_FINE)*wid];
					bptr = &tptr[tsx.TwoHalves.sHigh*wid];
					tsx.lval += xinc;
					tsy.lval = 0;
					for (yy=0; yy < h; ++yy)
					{
						//pix = bptr[tsy>>SCALE_SHIFT_FINE];
						pix = bptr[tsy.TwoHalves.sHigh];
						tsy.lval += yinc;

						if (pix)				// check for transparency
							*sptr_ = remap_table[pix];

						sptr_ += screen_buffer_width;				// next pixel in column
					}
				}
			}
			else
			{
				// bitm needs to be rotated
				tsx.lval = 0;
				for (xx=0; xx < w; ++xx)				// copy bitmap
				{
					sptr_ = sptr + xx;
					bptr = &tptr[tsx.TwoHalves.sHigh*wid];
					tsx.lval += xinc;
					tsy.lval = 0;
					for (yy=0; yy < h; ++yy)
					{
						pix = bptr[tsy.TwoHalves.sHigh];
						tsy.lval += yinc;

						if (pix)				// check for transparency
							*sptr_ = pix;

						sptr_ += screen_buffer_width;				// next pixel in column
					}
				}
			}
		}

		else		// mirrored and rotated
		{
			if (fDoRemap)
			{
				tsx.lval = xinc * w;
				for(xx = 0; xx < w; ++xx)
				{
					sptr_ = sptr + xx;
					tsx.lval -= xinc;
					bptr = &tptr[tsx.TwoHalves.sHigh*wid];
					tsy.lval = 0;
					for (yy=0; yy < h; ++yy)
					{
				  		pix = bptr[tsy.TwoHalves.sHigh];
				  		tsy.lval += yinc;

				  		if (pix)				// check for transparency
							*sptr_ = remap_table[pix];

						sptr_ += screen_buffer_width;				// next pixel in column
					}
				}
			}
			else
			{
				tsx.lval = xinc * w;
				for(xx = 0; xx < w; ++xx)
				{
					sptr_ = sptr + xx;
					tsx.lval -= xinc;
					bptr = &tptr[tsx.TwoHalves.sHigh*wid];
					tsy.lval = 0;
					for (yy=0; yy < h; ++yy)
					{
				  		pix = bptr[tsy.TwoHalves.sHigh];
				  		tsy.lval += yinc;

				  		if (pix)				// check for transparency
							*sptr_ = pix;

						sptr_ += screen_buffer_width;				// next pixel in column
					}
				}
			}
		}
	}

	else		// not rotated
	{
		if ((Xscale>0 && Yscale>0) || (ABS(Xscale) < (FULL_SCALE_FINE<<1) && ABS(Yscale) < FULL_SCALE_FINE<<1) )
		{
			if (fDoRemap)
			{
				tsy.lval = 0;
				for (yy=0; yy < h; ++yy)			// copy bitmap
				{
					bptr = &tptr[tsy.TwoHalves.sHigh*wid];
					tsy.lval += yinc;
					tsx.lval = 0;
					for (xx=0; xx < w; ++xx)
					{
						pix = bptr[tsx.TwoHalves.sHigh];
						tsx.lval += xinc;

						if (pix)				// check for transparency
							*sptr = remap_table[pix];
						++sptr;
					}
					sptr += (screen_buffer_width - w);			// move to start of next scanline
				}
			}
			else
			{
				tsy.lval = 0;
				for (yy=0; yy < h; ++yy)				// copy bitmap
				{
					bptr = &tptr[tsy.TwoHalves.sHigh*wid];
					tsy.lval += yinc;
					tsx.lval = 0;
					for (xx=0; xx < w; ++xx)
					{
						pix = bptr[tsx.TwoHalves.sHigh];
						tsx.lval += xinc;

						if (pix)				// check for transparency
							*sptr = pix;
						++sptr;
					}
					sptr += (screen_buffer_width - w);			// move to start of next scanline
				}
			}
		}

		// special for 50% or less
		else
		{
			if (fDoRemap)
			{
				tsy.lval = 0;
				for (yy=0; yy < h; ++yy)				// copy bitmap
				{
					bptr = &tptr[tsy.TwoHalves.sHigh*wid];
					tsy.lval += yinc;
					tsx.lval = 0;
					for (xx=0; xx < w; ++xx)
					{
						pix = antia_table[
								(antia_table[(bptr[tsx.TwoHalves.sHigh]*256) + bptr[tsx.TwoHalves.sHigh+1]] *256) +
								antia_table[(bptr[tsx.TwoHalves.sHigh+wid]*256) + bptr[tsx.TwoHalves.sHigh+wid+1]] ];
						tsx.lval += xinc;

						if (pix)				// check for transparency
							*sptr = remap_table[pix];
						++sptr;
					}
					sptr += (screen_buffer_width - w);			// move to start of next scanline
				}
			}
			else
			{
				tsy.lval = 0;
				for (yy=0; yy < h; ++yy)				// copy bitmap
				{
					bptr = &tptr[tsy.TwoHalves.sHigh*wid];
					tsy.lval += yinc;
					tsx.lval = 0;
					for (xx=0; xx < w; ++xx)
					{
						pix = antia_table[
								(antia_table[(bptr[tsx.TwoHalves.sHigh]*256) + bptr[tsx.TwoHalves.sHigh+1]] *256) +
								antia_table[(bptr[tsx.TwoHalves.sHigh+wid]*256) + bptr[tsx.TwoHalves.sHigh+wid+1]] ];
						tsx.lval += xinc;

						if (pix)				// check for transparency
							*sptr = pix;
						++sptr;
					}
					sptr += (screen_buffer_width - w);			// move to start of next scanline
				}
			}
		}
	}		// not rotated

}


void ScaleBitmap (SHORT x, SHORT y, SHORT iBitm, SHORT bx, SHORT by, SHORT w, SHORT h, SHORT scale)
{
	ScaleBitmap_fine (x, y, iBitm, bx, by, w, h, scale*256L,scale*256L);
}

void ScaleBitmapX (SHORT x, SHORT y, SHORT iBitm, SHORT bx, SHORT by, SHORT w, SHORT h, SHORT scale)
{
	ScaleBitmap_fine (x, y, iBitm, bx, by, w, h, scale*256L,256L*256L);
}

void ScaleBitmapY (SHORT x, SHORT y, SHORT iBitm, SHORT bx, SHORT by, SHORT w, SHORT h, SHORT scale)
{
	ScaleBitmap_fine (x, y, iBitm, bx, by, w, h, 256L*256L,scale*256L);
}

/* ====================================================================
	
	==================================================================== */
#if 0
void ScaleMirrorBitmap (SHORT x, SHORT y, SHORT iBitm, SHORT bx, SHORT by, SHORT w, SHORT h, SHORT scale)
{
	LONG		xx;
	LONG		yy;
	LONG		wid, hei, ww, hh, x_offset;
	LONG		tsx, tsy;
	LONG		xinc, yinc;
	BOOL		fRotated = FALSE;
	BOOL     fMirror = FALSE;
	BITMPTR	p;
	PTR		tptr, bptr;
	PTR		sptr, sptr_;
	BYTE		pix;

	if (iBitm == fERROR)							// check for bad source bitmap
		return;

	p = (BITMPTR) BLKPTR(iBitm);				// get pointer to bitm header
	if (!IsPointerGood(p))
		return;

	tptr = (PTR)p + sizeof(BITMHDR);			// get pointer to data
	wid = p->w;										// get size
	hei = p->h;
	scale = (p->scale * scale) / UNITARY_SCALE;	// combo scale
	ww = (wid * FULL_SCALE) / ABS(scale);			// scaled size
	hh = (hei * FULL_SCALE) / ABS(scale);
	x_offset = (p->x_ctr_pt * FULL_SCALE) / ABS(scale);  // scaled ctr point
	tptr += (by * wid) + bx;					// adj point by start coords
	xinc = yinc = ABS(scale);					// set independant x & y scales

	// negative width is used as a flag to say that bitm needs to be rotated
	
	if (w < 0)
	{
		fRotated = TRUE;
		w = -w;
		if(h<0)
		{
			fMirror = TRUE;
			h = -h;
		}
		y = y + h - ww;		// put feet on bottom of passed area (y+h)
	}
	
	
	if(fMirror == FALSE)
		x -= x_offset;		// handle center point;
	else
		x -= hh - x_offset;

	/* clipping */
	if (x>screen_buffer_width-1 || x+w-1<0 || y>screen_buffer_height-1 || y+h-1<0)
		return;

	if (x+w-1 > screen_buffer_width-1)
		w = screen_buffer_width - x - 1;

	if (y+h-1 > screen_buffer_height-1)
		h = screen_buffer_height - y - 1;

	if (x < 0)
	{
		w += x;
		x = 0;
	}

	if (y < 0)
	{
		h += y;
		y = 0;
	}

	if (fRotated) {tsy = w; w = h; h = tsy;}			 // fix dimensions

	if (w > ww)
		w = ww;

	if (h > hh)
		h = hh;

	sptr=(PTR)screen+((y*screen_buffer_width)+x);		// set destination pointer

	if (fRotated)			// check for rotated
	{
		if(fMirror == FALSE)
		{
			// bitm needs to be rotated
			tsy = 0;
			for (yy=0; yy < h; ++yy)				// copy bitmap
			{
				sptr_ = sptr + yy;
				bptr = &tptr[(tsy>>8)*wid];
				tsy += yinc;
				tsx = 0;
				for (xx=0; xx < w; ++xx)
				{
			  		pix = bptr[tsx>>8];
			  		tsx += xinc;

			  		if (pix)				// check for transparency
						*sptr_ = pix;

					sptr_ += screen_buffer_width;				// next pixel in column
				}
			}
		}

		else
		{
			tsy = yinc * h;
			for(yy = 0; yy < h; ++yy)
			{
				sptr_ = sptr + yy;
				tsy -= yinc;
				bptr = &tptr[(tsy>>8)*wid];
				tsx = 0;
				for (xx=0; xx < w; ++xx)
				{
			  		pix = bptr[tsx>>8];
			  		tsx += xinc;

			  		if (pix)				// check for transparency
						*sptr_ = pix;

					sptr_ += screen_buffer_width;				// next pixel in column
				}
			}
		}
	}

	else		// not rotated
	{
		if (scale>0 || ABS(scale) < (FULL_SCALE<<1) )
		{
			tsy = 0;
			for (yy=0; yy < h; ++yy)				// copy bitmap
			{
				bptr = &tptr[(tsy>>8)*wid];
				tsy += yinc;
				tsx = 0;
				for (xx=0; xx < w; ++xx)
				{
					pix = bptr[tsx>>8];
					tsx += xinc;

					if (pix)				// check for transparency
						*sptr = pix;
					++sptr;
				}
				sptr += (screen_buffer_width - w);			// move to start of next scanline
			}
		}

		// special for 50% or less
		else
		{
			tsy = 0;
			for (yy=0; yy < h; ++yy)				// copy bitmap
			{
				bptr = &tptr[(tsy>>8)*wid];
				tsy += yinc;
				tsx = 0;
				for (xx=0; xx < w; ++xx)
				{
					pix = antia_table[
							(antia_table[(bptr[(tsx>>8)]*256) + bptr[(tsx>>8)+1]] *256) +
							antia_table[(bptr[(tsx>>8)+wid]*256) + bptr[(tsx>>8)+wid+1]] ];
					tsx += xinc;

					if (pix)				// check for transparency
						*sptr = pix;
					++sptr;
				}
				sptr += (screen_buffer_width - w);			// move to start of next scanline
			}
		}
	}		// not rotated

}

#endif


/* ====================================================================
	function creates, inits, and allocates memory for a bitmap structure
	==================================================================== */
SHORT _OpenBitm (SHORT w, USHORT h, BOOL fLocked)
{
	BITMPTR	pBlk;
	SHORT		iBlk;

	if (fLocked)
		iBlk = NewLockedBlock((w * h) + sizeof(BITMHDR));
	else
		iBlk = NewBlock((w * h) + sizeof(BITMHDR));

	if (iBlk != fERROR)
	{
		pBlk = (BITMPTR)(BLKPTR(iBlk));
		if (!IsPointerGood(pBlk))
			return fERROR;

		pBlk->w = w;
		pBlk->h = h;
		pBlk->scale = UNITARY_SCALE;				/* 1:1 scale */
		pBlk->x_ctr_pt = 0;
		pBlk->type = TYPEBITM;
	}

	return iBlk;
}

/* =======================================================================
   Function    - Exists
   Description - checks for existence of a file
   Returns     - TRUE or FALSE
   ======================================================================== */

BOOL Exists( char *filename)
{
	// FILE *test;
	//
	// if ((test = fopen( filename, "rb")) == NULL)
	// 	return FALSE;
	// fclose( test);
	// return TRUE;
	return(!FileAccess(filename));
}

/* =======================================================================
	PutScreen - write out the screen in PCX format
	======================================================================= */
UBYTE	aPCXheader[128] = {
0x0A,0x05,0x01,0x08,0x00,0x00,0x00,0x00,0x3E,0x00,0x48,0x00,0x40,0x01,0xC8,0x00,
0x00,0x00,0x00,0x00,0x00,0xAA,0x00,0xAA,0x00,0x00,0xAA,0xAA,0xAA,0x00,0x00,0xAA,
0x00,0xAA,0xAA,0x55,0x00,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,0x55,0xFF,0x55,0xFF,
0x55,0x55,0xFF,0xFF,0xFF,0x55,0x55,0xFF,0x55,0xFF,0xFF,0xFF,0x55,0xFF,0xFF,0xFF,
0x00,0x01,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

SHORT PutScreen (SHORT iS, USHORT x, USHORT y, USHORT width, USHORT height, CSTRPTR szFileName)
{
	USHORT		i;
	USHORT		right;
	USHORT		bottom;
	USHORT		w;
	USHORT		cbSrc;
	COLORSPEC	OutPalette[256];
	PTR			pSrc;
	UBYTE			c;
	UBYTE			buff[1024];
	int			fp;

	width = (width + 1) & 0xFFFE;
	right = width - 1;
	bottom = height - 1;
	cbSrc = 640 & 0xFFFE;
	pSrc = ((PTR)BLKPTR(iS))+(y*cbSrc)+x;

	// ????
	// aPCXheader[8] = (UBYTE)right;
	// aPCXheader[10] = (UBYTE)bottom;
	// aPCXheader[66] = (UBYTE)width;
	
	aPCXheader[8] = (UBYTE)right;
	aPCXheader[9] = (UBYTE)(right >> 8);
	aPCXheader[10] = (UBYTE)bottom;
	aPCXheader[11] = (UBYTE)(bottom >> 8);
	aPCXheader[66] = (UBYTE)width;
	aPCXheader[67] = (UBYTE)(width >> 8);

	/* open the file */
	fp = open(szFileName, MODE_MAKEFILE);	/* try to open the file */
	if (fp == fERROR)		/* if successful then load the resource */
	{
		fatal_error("ERROR in PutPCX - unable to open file: %s\n",szFileName);
		return (SHORT)fERROR;
	}

	/* write out header */
	write(fp, &aPCXheader[0], 128);

	/* write out data */
	do
	{
		w = width;
		i = 0;
		do
		{
			c = *pSrc++;
			if (c > 0xBF)
				buff[i++] = 0xC1;
			buff[i++] = c;
		} while (--w);
		write(fp, &buff, i);
		pSrc += cbSrc - width;
	} while (--height);

	/* write out palette */
	i = 12;
	write(fp, &i, 1);				/* write out extended palette tag */
	for (i=0; i<256; ++i)
	{
		OutPalette[i].bRed   = CurPal[i].bRed << 2;
		OutPalette[i].bGreen = CurPal[i].bGreen << 2;
		OutPalette[i].bBlue  = CurPal[i].bBlue << 2;
	}
	write(fp, &OutPalette[0], 768);
	close (fp);

	return fNOERR;
}

/* ========================================================================
	Function	-
	Description -
	Returns	- void
	======================================================================== */
void crease (LONG x, LONG y, LONG w, LONG h)
{
	LONG	xx, yy, pix;

	for (yy=y; yy<(y+h); ++yy)
		for (xx=x; xx<(x+w); ++xx)
			plot(xx,yy,get_pixel(xx,yy)-3);

	for (yy=y; yy<(y+h); ++yy)
	{
		for (xx=x; xx<(x+w); ++xx)
		{
			pix = get_pixel(xx+1,yy+1);

			if (pix < LTYELLOW && pix > (LTYELLOW-24))
				++pix;
			if (pix < LTTAN && pix > (LTTAN-24))
				++pix;
			if (pix < LTYELLOW && pix > (LTYELLOW-24))
				++pix;
			if (pix < LTTAN && pix > (LTTAN-24))
				++pix;
			if (pix < LTYELLOW && pix > (LTYELLOW-24))
				++pix;
			if (pix < LTTAN && pix > (LTTAN-24))
				++pix;

			plot(xx+1,y+1,pix);
		}
	}
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
void DrawLittleButton (LONG x, LONG y, LONG w, LONG h, LONG pushFlag)
{
	LONG Color1, Color2, Color3;

	if (pushFlag)
	{
		Color1 = COPPER_GRAD;
		Color2 = COPPER_GRAD - 10;
		Color3 = LTTAN - 4;
	}
	else
	{
		Color1 = COPPER_GRAD - 10;
		Color2 = COPPER_GRAD;
		Color3 = LTTAN;
	}

	color_rect(x, y, w+1, h, Color3);

	color_rect(x-2, y-2, 1,   h+4, Color1);
	color_rect(x-1, y-1, 1,   h+2, Color1);
	color_rect(x-1, y-1, w+2, 0, Color1);
	color_rect(x-1, y-2, w+3, 0, Color1);

	color_rect(x+w+2, y-2,   1,   h+4, Color2);
	color_rect(x+w+1, y-1,   1,   h+2, Color2);
	color_rect(x,     y+h+1, w+2, 0,   Color2);
	color_rect(x-1,   y+h+2, w+3, 0,   Color2);
}

/* ======================================================================== */
