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


SHORT		xCharWidth[128];
SHORT		xCharPosn[128];
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

// static LONG ldMarginTop = 0;
// static LONG ldMarginBottom = 0;
// static LONG ldMarginLeft = 0;
// static LONG ldMarginRight = 0;

// static UBYTE	lighten[256] = {
// /*			  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 */
// /*	  4,*/  5,  6,  7,  8,  8,  9, 10, 11, 12, 13, 14, 14, 15, 16, 17, 18, 19, 20, 20, 21, 22, 23, 24, 25, 26, 26, 27, 28, 29, 30, 31, 31,
// /*	 36,*/ 37, 38, 39, 40, 40, 41, 42, 43, 44, 44, 45, 46, 47, 48, 48, 49, 50, 51, 52, 52, 53, 54, 55, 55,
// /*	 60,*/ 61, 62, 63, 64, 64, 65, 66, 67, 68, 68, 69, 70, 71, 72, 72, 73, 74, 75, 76, 76, 77, 78, 79, 79,
// /*	 84,*/ 85, 86, 87, 88, 88, 89, 90, 91, 92, 92, 93, 94, 95, 96, 96, 97, 98, 99,100,100,101,102,103,103,
// /*	108,*/109,110,111,112,112,113,114,115,116,116,117,118,119,120,120,121,122,123,124,124,125,126,127,127,
// /*	132,*/133,134,135,136,136,137,138,139,140,140,141,142,143,144,144,145,146,147,148,148,149,150,151,151,
// /*	156,*/157,158,159,160,160,161,162,163,164,164,165,166,167,168,168,169,170,171,172,172,173,174,175,175,
// /*	180,*/181,182,183,184,184,185,186,187,188,188,189,190,191,192,192,193,194,195,196,196,197,198,199,199,
// /*	204,*/205,206,207,208,208,209,210,211,212,212,213,214,215,216,216,217,218,219,220,220,221,222,223,223,
// /*	228,*/229,230,231,232,232,233,234,235,236,236,237,238,239,240,240,241,242,243,244,244,245,246,247,247,
// /*	248,*/249,250,251,252,253,254,255 };

extern UBYTE	antia_table[];

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

	for (xx=x=0; x<w; x++, xx+=h)
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


// Stub moved here from TIMER.C.
void run_timers ()
{
}
