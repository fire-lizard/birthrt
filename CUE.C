/* ®RM255¯ */
/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: CUE.C       -???
   Author: Chris Phillips
   ========================================================================
   Contains the following general functions:
   rgb_to_hsl				- converts an RGB value to an HSL value
   hsl_to_rgb				- converts an HSL value to an RGB value
   value						- ???
   remap_pal				- remaps a palette to another
   CreateShadeTable		- Given a HSI change in percent, build a new remap table
   init_shade_table		- initializes the shade table
   shade_rect				- draws a shaded rectangle on the screen
   shade_edged_rect		- draws a 'shadowed' shaded rectangle on the screen
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "engine.h"
#include "machine.h"
#include "light.h"
#include "main.hxx"

#ifdef _JPC
void	ErrorMessage (const char *format, ...);
#endif

#ifdef _DEBUG
BOOL gfTestGrayLight;
#endif

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define MAX_SCALE_RATE     (1<<7)
#define RATE               1
	
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
static void rgb_to_hsl (double r,double g,double b,double *h,double *s,double *l);
static double value (double n1,double n2,double h);
static void hsl_to_rgb (double h,double s,double l,double *r,double *g,double *b);
static void remap_pal (PALETTE src, PALETTE dest, UBYTE *table);
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
// Allocate space for the base tables, the infravision tables, and the
// color remap tables.
// To add more color remap tables, DELETE the old .SHD file from the
// GRAPHICS directory, change the value of cREMAP_GROUPS, and add the
// new colors to the gRemapColors array.
//
// Total size is 256 (bytes per table) * (number of base tables ("regular"
// colors in 32 degrees of shading plus 3 "smoked glass" tables) + infravision
// tables in 32 degrees of shading + grayscale tables in 32 degrees of
// shading + cREMAP_GROUPS tables in 32 degrees of shading).
// Each color table takes 256 * 32 = 8192 bytes to hold all the shaded versions
// of the colors.

#define TOTAL_COLORTABLE_SIZE (256 * (cBASE_TABLES + 32 * cREMAP_GROUPS + 32 * cMISC_TABLES))
UBYTE shade_table[TOTAL_COLORTABLE_SIZE];	// [d7-15-96 JPC] added room for color remap tables
UBYTE antia_table[256*256];

// With 9 color ranges to chose from, there are 45 unique two-color
// combinations (including pure colors).

// [d11-30-96 JPC] Changed remap group 1 from WHITE_GRAD,WHITE_GRAD (grayscale)
// to BLUE_GRAD, PURPLE_GRAD (the original colors) so things with no remaps
// such as treasure chests will look correct.

SHORT gRemapColors[cREMAP_GROUPS][2] = {
// 			Color 1     Color 2
/*   0 */	BLUE_GRAD, PURPLE_GRAD,		// no remap; was WHITE_GRAD,WHITE_GRAD
/*   1 */	ORANGE_GRAD,WHITE_GRAD,
/*   2 */	ORANGE_GRAD,GOLD_GRAD,
/*   3 */	RED_GRAD,GOLD_GRAD,
/*   4 */	GOLD_GRAD,RED_GRAD,
/*   5 */	BLACK_GRAD,TAN_GRAD,
/*   6 */	PURPLE_GRAD,GOLD_GRAD,
/*   7 */	SILVER_GRAD,STEEL_GRAD,
/*   8 */	COPPER_GRAD,GREEN_GRAD,
/*   9 */	PURPLE_GRAD,RED_GRAD,
/*  10 */	PURPLE_GRAD,BLACK_GRAD,
/*  11 */	TAN_GRAD,WHITE_GRAD,
/*  12 */	GOLD_GRAD,WHITE_GRAD,
/*  13 */	PURPLE_GRAD,COPPER_GRAD,
/*  14 */	BLACK_GRAD,GOLD_GRAD,
/*  15 */	BLACK_GRAD,BLACK_GRAD,
/*  16 */	ORANGE_GRAD,RED_GRAD,
/*  17 */	BLACK_GRAD,RED_GRAD,
/*  18 */	GREEN_GRAD,GOLD_GRAD,
/*  19 */	BLACK_GRAD,STEEL_GRAD,
/*  20 */	BLUE_GRAD,WHITE_GRAD,
/*  21 */	STEEL_GRAD,WHITE_GRAD,
/*  22 */	STEEL_GRAD,GOLD_GRAD,
/*  23 */	BLACK_GRAD,BLACK_GRAD,
/*  24 */	SILVER_GRAD,TAN_GRAD,
/*  25 */	GREEN_GRAD,WHITE_GRAD,
/*  26 */	RED_GRAD,SILVER_GRAD,
/*  27 */	BLUE_GRAD,GOLD_GRAD,
/*  28 */	GREEN_GRAD,TAN_GRAD,
/*  29 */	BLACK_GRAD,ORANGE_GRAD,
/*  30 */	PURPLE_GRAD,WHITE_GRAD,
/*  31 */	GREEN_GRAD,ORANGE_GRAD,
/*  32 */	BLACK_GRAD,BLUE_GRAD,
/*  33 */	GREEN_GRAD,SILVER_GRAD,
/*  34 */	BLACK_GRAD,GREEN_GRAD,
};

LONG shade_to_change = 33;
LONG fact_to_change = 0;
LONG percIntensity[3] = {55,80,90};
LONG percTranslucent[3] = {0,0,25};
LONG percRed[3] = {100,100,100};
LONG percGreen[3] = {100,100,100};
LONG percBlue[3] = {100,100,100};
LONG toggle_pannel = FALSE;

CHAR art_path[80] = "graphics\\";

/* =======================================================================
   Function    - rgb_to_hsl
   Description - converts an RGB value to an HSL value
   Returns     - void
   ======================================================================== */
static void rgb_to_hsl (double r,double g,double b,double *h,double *s,double *l)
{
	double v;
	double m;
	double vm;
	double r2, g2, b2;

	v = MAX(r,g);
	v = MAX(v,b);
	m = MIN(r,g);
	m = MIN(m,b);

	*l = (m + v) / (double)2.0;

	if (m == v)
	{
		*h = (double)0;
		*s = (double)0;
		return;
	}

	vm = v - m;

	if (*l < (double)0.5)
		*s = vm / (v + m);
	else
		*s = vm / ((double)2.0 - vm);

	r2 = (v - r) / vm;
	g2 = (v - g) / vm;
	b2 = (v - b) / vm;

	if (r == v)
		*h = b2 - g2;

	else if (g == v)
		*h = 2 + r2 - b2;

	else
		*h = (double)4 + g2 - r2;

	*h *= (double)60;
	if (*h < (double)0.0) *h += (double)360;
}

/* =======================================================================
   Function    - value
   Description - given h on [0..360] & s,l on [0..1], return r,g,b on [0..1]???
   Returns     - the RGB value
   ======================================================================== */
static double value (double n1,double n2,double h)
{
	if (h > (double)360) h -= (double)360;
	if (h < (double)0) h += (double)360;

	if (h < (double)60) return (n1 + (((n2-n1) * h) / (double)60));
	else if (h < (double)180) return n2;
	else if (h < (double)240) return (n1 + (((n2-n1) * ((double)240-h)) / (double)60));
	else return n1;
}

/* =======================================================================
   Function    - hsl_to_rgb
   Description - converts an HSL value to an RGB value
   Returns     - void
   ======================================================================== */
static void hsl_to_rgb (double h,double s,double l,double *r,double *g,double *b)
{
	double m1, m2;

	if (l <= (double)0.5)
		m2 = l * (s + (double)1);
	else
		m2 = l * (s + (double)1);

	m1 = ((double)2 * l) - m2;

	if (s == (double)0)
		*r = *g = *b = l;
	else
	{
		*r = value(m1, m2, h + (double)120);
		*g = value(m1, m2, h);
		*b = value(m1, m2, h - (double)120);
	}
}

/* =======================================================================
   Function    - remap_pal
   Description - remaps a palette to a different one
   Returns     - void
   ======================================================================== */
static void remap_pal (PALETTE src, PALETTE dest, UBYTE *table)
{
	LONG i;
	LONG j;
	LONG k;
	LONG baseRed;
	LONG baseGreen;
	LONG baseBlue;
	LONG newRed;
	LONG newGreen;
	LONG newBlue;
	LONG baseErr;
	LONG newErr;

	/* for each source palette color */
	for(i = 0; i < 256; ++i)
	{
		/* get the source color as integer values */
		baseRed = (LONG)src[i].bRed;
		baseGreen = (LONG)src[i].bGreen;
		baseBlue = (LONG)src[i].bBlue;

		/* set the initial error accumulator to maximum (match first color) */
		baseErr = 0x7FFFFF;

		k = i;

		/* step through the destination colors to locate the closest match */
		// don't include the fire colors that don't dim
		for(j = 0; j < 248; ++j)
		{

			/* get the current destination color components as integer values */
			newRed = (LONG)dest[j].bRed;
			newGreen = (LONG)dest[j].bGreen;
			newBlue = (LONG)dest[j].bBlue;

			/* calculate the error (distance) between the two colors */
			newErr =	(	((LONG)(newRed - baseRed    )*(LONG)(newRed - baseRed    )) +
							  			((LONG)(newGreen - baseGreen)*(LONG)(newGreen - baseGreen)) +
										((LONG)(newBlue - baseBlue  )*(LONG)(newBlue - baseBlue  ))	);

			/* if the current colors more closely match than the previous match */
			if(newErr < baseErr)
			{
				/* save this destination color index */
				k = j;

				/* save the closest match error */
				baseErr = newErr;
			}
		}


		/* set the current source color remap index */
		table[i] = (UBYTE)k;
	}
}

/* =======================================================================
   Function    - CreateShadeTable
   Description - Given a HSI change in percent, build a new remap table
                 eg. percIntensity = 50,
                 	percTranslucent = 100,
                 	percRed = 100,
                 	percGreen = 100
                 	percBlue = 150
                 would return a remap table that is more blue, but
                 half as bright.  If percIntensity was 150, the table
                 would be half again brighter.
   Returns     - void
   ======================================================================= */
void CreateShadeTable (
	UBYTE		*ptrPalette,
	LONG		percIntensity,
	LONG		percTranslucent,
	LONG		percRed,
	LONG		percGreen,
	LONG		percBlue
)
{
	SHORT		c;
	double	trans, red, green, blue;
	double	r,g,b,h,s,l;
	double	factor;
	COLORSPEC	modified_pal[256];

	/* this factor converts 0-63 values to 0-1 values */
	factor = (double)1 / (double)64 ;

	for(c=0; c<256; ++c)
	{
		trans = (double)percTranslucent / (double)100.0;
		red   = (double)percRed   / (double)100.0;
		green = (double)percGreen / (double)100.0;
		blue  = (double)percBlue  / (double)100.0;

		/* retrieve current game color and shift them */
		//------------------------------------------
		r = (double)CurPal[c].bRed * factor ;
		r = (r+trans) * red;
		r = (r > (double)1) ? (double)1 : r ;
		r = (r < (double)0) ? (double)0 : r ;
		
		//------------------------------------------
		g = (double)CurPal[c].bGreen * factor ;
		g = (g+trans) * green;
		g = (g > (double)1) ? (double)1 : g ;
		g = (g < (double)0) ? (double)0 : g ;
		
		//------------------------------------------
		b = (double)CurPal[c].bBlue * factor ;
		b = (b+trans) * blue;
		b = (b > (double)1) ? (double)1 : b ;
		b = (b < (double)0) ? (double)0 : b ;
		//------------------------------------------

		/* convert to hsl */
		rgb_to_hsl(r,g,b,&h,&s,&l);

		/* modify */
		l = ( l * (double)percIntensity ) / (double)100.0;

		/* convert back to rgb value */
		hsl_to_rgb(h,s,l,&r,&g,&b);

		/* save modified value in new palette */
		modified_pal[c].bRed  = (UBYTE)(r * (double)64);
		modified_pal[c].bGreen= (UBYTE)(g * (double)64);
		modified_pal[c].bBlue = (UBYTE)(b * (double)64);
	}
	remap_pal( (PALETTE)&modified_pal[0], (PALETTE)&CurPal[0], (UBYTE *)ptrPalette );
}

/* =======================================================================
   Function    - init_shade_table
   Description - initializes the shading table
   Returns     - void
   ======================================================================== */
void init_shade_table(CHAR *Name)
{
// Called from WINSYS\SYSGRAPH.C: init_graph.
// ---------------------------------------------------------------------------

	FILE *		f;
	LONG			i,j,c;
	LONG			iRemap;
	LONG			size;
	LONG			bytesRead;
	COLORSPEC	dark_pal[256];
	CHAR			pathname[80];

	double factor=(double)1/(double)256;
	sprintf(pathname,"%s%s.shd", art_path, Name );
	f=FileOpen(pathname,"rb");
	if(f!=NULL)
	{
		// If file exists, read it in.
		// fread(&shade_table[0],sizeof(char),256*35,f);
		// fread(&shade_table[0],sizeof(char),256*(35+32),f); // [d7-02-96 JPC]
		size = TOTAL_COLORTABLE_SIZE;
		bytesRead = fread(&shade_table[0],sizeof(char), size, f); // [d7-15-96 JPC]
		if (bytesRead != size)
		{
			char szTemp[512];
			sprintf (szTemp,"init_shade_table: existing file %s is smaller than expected.\n"
				"Possible cause: change in cREMAP_GROUPS without deleting existing .SHD table.\n"
				"Exit the program, delete %s, and run the program again.", pathname, pathname);
#ifdef _JPC
			ErrorMessage (szTemp);
#else
			printf (szTemp);
#endif
		}
		FileClose(f);
	}
	else
	{
		// File does not exist; create it.
		// Create new shade tables by specifying a nonexistent file when
		// you call this routine from WINSYS\SYSGRAPH.C: init_graph.
		// (Either use a new name or delete the existing file.)
		// The new table is created and then written to disk for future use.
		for(c=0;c<256;++c)
			shade_table[c]=(UBYTE)c;
		// Make a shade table with 32 levels.
		// TABLES 1-31 (table 0 is the full-brightness table).
		for(i=1;i<32;++i)
		{
			for(c=0;c<248;++c)		// exclude last eight for flame colors
			{
				double r=(double)(CurPal[c].bRed)*factor;
				double g=(double)(CurPal[c].bGreen)*factor;
				double b=(double)(CurPal[c].bBlue)*factor;
				double h, s, l;
				
				rgb_to_hsl(r,g,b,&h,&s,&l);
				l = l - (i * l) / (double)32;
				hsl_to_rgb(h,s,l,&r,&g,&b);
				dark_pal[c].bRed=(UBYTE)(r*(double)256);
				dark_pal[c].bGreen=(UBYTE)(g*(double)256);
				dark_pal[c].bBlue=(UBYTE)(b*(double)256);
			}
			remap_pal((PALETTE)&dark_pal[0],(PALETTE)&CurPal[0],&shade_table[i*256]);
			for(c=248;c<256;++c)		// fill in remaining 8 color with self references
				shade_table[(i*256)+c] = (UCHAR)c;
		}

		// Create 'smoked glass panel' tables.
		// TABLES 32-34.
		CreateShadeTable((UBYTE *)&shade_table[32*256],percIntensity[0],percTranslucent[0],percRed[0],percGreen[0],percBlue[0]);
		CreateShadeTable((UBYTE *)&shade_table[33*256],percIntensity[1],percTranslucent[1],percRed[1],percGreen[1],percBlue[1]);
		CreateShadeTable((UBYTE *)&shade_table[34*256],percIntensity[2],percTranslucent[2],percRed[2],percGreen[2],percBlue[2]);

		// [d7-02-96 JPC] Do a red-shifted set of palettes for infravision.
		// TABLE 35.
		// At this point, I'm only guessing at the proper values.  They seem OK.
		for(i=0;i<32;++i)
		{
			for(c=0;c<248;++c)		// exclude last eight for flame colors
			{
				double r=(double)(CurPal[c].bRed)*factor;
				double g=(double)(CurPal[c].bGreen)*factor;
				double b=(double)(CurPal[c].bBlue)*factor;
				double h, s, l;
				
				// Do not change red; instead, reduce green and blue.
				// Brightness is the same as for the standard tables.
				g /= 4;
				b /= 4;
				rgb_to_hsl(r,g,b,&h,&s,&l);
				l = l - (i * l) / (double)32;
				hsl_to_rgb(h,s,l,&r,&g,&b);
				dark_pal[c].bRed=(UBYTE)(r*(double)256);
				dark_pal[c].bGreen=(UBYTE)(g*(double)256);
				dark_pal[c].bBlue=(UBYTE)(b*(double)256);
			}
			remap_pal((PALETTE)&dark_pal[0],(PALETTE)&CurPal[0],
				&shade_table[(i+INFRAVISION_OFFSET)*256]);
			for(c=248;c<256;++c)		// fill in remaining 8 color with self references
				shade_table[((i+INFRAVISION_OFFSET)*256)+c] = (UCHAR)c;
		}

		// [d7-15-96 JPC] Now do a set of color remap tables.  Each color
		// remapping requires 32 tables.
		for (iRemap = 0; iRemap < cREMAP_GROUPS; ++iRemap)
		{
			for (i = 0; i < 32; ++i)
			{
				int iBaseTable = i * 256;
				int iTable = (i + REMAP_TABLE_OFFSET + 32 * iRemap) * 256;
	
				for (c = 0; c < 256; ++c)		// copy the corresponding "base" table
					shade_table[iTable + c] = shade_table[iBaseTable + c];
				
				for (c = 0; c < cREMAP_PALETTEENTRIES; ++c)
				{
					j = (LONG)gRemapColors[iRemap][0] + c;
					if (j<1) j = 1;
					shade_table[iTable+c+REMAP_RANGE_1] = shade_table[iBaseTable+j];
				}

				for (c = 0; c < cREMAP_PALETTEENTRIES; ++c)
				{
					j = (LONG)gRemapColors[iRemap][1] + c;
					if (j<1) j = 1;
					shade_table[iTable+c+REMAP_RANGE_2] = shade_table[iBaseTable+j];
				}
			}
		}

		// [d9-05-96 JPC] Do a gray-scale set of tables for items turned to stone.
		for(i=0;i<32;++i)
		{
			for(c=0;c<248;++c)		// exclude last eight for flame colors
			{
				double r=(double)(CurPal[c].bRed)*factor;
				double g=(double)(CurPal[c].bGreen)*factor;
				double b=(double)(CurPal[c].bBlue)*factor;
				double h, s, l;
				double average;
				
				average = (r + g + b) / 3;
				r = average;
				g = average;
				b = average;
				rgb_to_hsl(r,g,b,&h,&s,&l);
				l = l - (i * l) / (double)32;
				hsl_to_rgb(h,s,l,&r,&g,&b);
				dark_pal[c].bRed=(UBYTE)(r*(double)256);
				dark_pal[c].bGreen=(UBYTE)(g*(double)256);
				dark_pal[c].bBlue=(UBYTE)(b*(double)256);
			}
			remap_pal((PALETTE)&dark_pal[0],(PALETTE)&CurPal[0],
				&shade_table[(i+GRAYSCALE_TABLE_OFFSET)*256]);
			for(c=248;c<256;++c)		// fill in remaining 8 color with self references
				shade_table[((i+GRAYSCALE_TABLE_OFFSET)*256)+c] = (UCHAR)c;
		}

		// [d11-06-96 JPC]
		// Prevent any colors (1-247) from mapping to 0 (transparent).
		for (i = 1; i < WRAITH_TABLE_OFFSET; ++i)
		{
			for (c = 1; c < 248; ++c)
				if (shade_table[i * 256 + c] == 0)
					shade_table[i * 256 + c] = 1;
		}

		// [d10-25-96 JPC] Add a "wraith" table, which is the grayscale table
		// with every other pixel 0.
		for(i=0;i<32;++i)				// do 32 tables (brightest to dimmest)
		{
			for(c=0;c<248;++c)		// exclude last eight for flame colors
			{
				if ((c % 2) == 0)
				{
					dark_pal[c].bRed   = 0;
					dark_pal[c].bGreen =	0;
					dark_pal[c].bBlue  =	0;
				}
				else
				{
					double r=(double)(CurPal[c].bRed)*factor;
					double g=(double)(CurPal[c].bGreen)*factor;
					double b=(double)(CurPal[c].bBlue)*factor;
					double h, s, l;
					double average;
					
					average = (r + g + b) / 3;
					r = average;
					g = average;
					b = average;
					rgb_to_hsl(r,g,b,&h,&s,&l);
					l = l - (i * l) / (double)32;
					hsl_to_rgb(h,s,l,&r,&g,&b);
					dark_pal[c].bRed   =(UBYTE)(r*(double)256);
					dark_pal[c].bGreen =(UBYTE)(g*(double)256);
					dark_pal[c].bBlue  =(UBYTE)(b*(double)256);
				}
			}
			remap_pal((PALETTE)&dark_pal[0],(PALETTE)&CurPal[0],
				&shade_table[(i+WRAITH_TABLE_OFFSET)*256]);
			for(c=248;c<256;++c)		// fill in remaining 8 color with self references
				shade_table[((i+WRAITH_TABLE_OFFSET)*256)+c] = (UCHAR)c;
		}

		sprintf(pathname,"%s%s.shd", art_path, Name );
		f=fopen(pathname,"wb");
		if(f != NULL)
		{
			fwrite(&shade_table[0],sizeof(char),TOTAL_COLORTABLE_SIZE,f); // [d7-15-96 JPC]
			fclose(f);
		}
		else
		{
				fatal_error("Unable to write shade table file.");
				return;
		}

	}

	/* create average table */
	sprintf(pathname,"%s%s.ant",art_path, Name );
	f=FileOpen(pathname,"rb");
	if(f!=NULL)
	{
		fread(&antia_table[0],sizeof(char),256*256,f);
		FileClose(f);
	}
	else
	{
		for(i=1; i<256; ++i)
		{
			const UBYTE rr = CurPal[i].bRed;
			const UBYTE gg = CurPal[i].bGreen;
			const UBYTE bb = CurPal[i].bBlue;
			for(c=0; c<256; ++c)
			{
				dark_pal[c].bRed   = (UBYTE)(((CurPal[c].bRed  *40) + (rr*60)) / 100);
				dark_pal[c].bGreen = (UBYTE)(((CurPal[c].bGreen*40) + (gg*60)) / 100);
				dark_pal[c].bBlue  = (UBYTE)(((CurPal[c].bBlue *40) + (bb*60)) / 100);
			}
			remap_pal((PALETTE)&dark_pal[0],(PALETTE)&CurPal[0],&antia_table[i*256]);
		}
		sprintf(pathname,"%s%s.ant", art_path, Name );
		f=fopen(pathname,"wb");
		if(f != NULL)
		{
			fwrite(&antia_table[0],sizeof(char),256*256,f);
			fclose(f);
		}
		else
		{
				fatal_error("Unable to write anti-alias table file.");
				return;
		}
	}
}

/* =======================================================================
   Function    - color_rect
   Description - draws a colored rectangle
   Returns     - void
   ======================================================================== */
void color_rect (LONG x,LONG y,LONG w,LONG h,LONG s)
{
	PTR  sptr;
	// GWP LONG ww;
	
//#if defined(_DEBUG)
//	if (x < 0 || x >= MAX_VIEW_WIDTH ||
//	    y < 0 || y >= MAX_VIEW_HEIGHT ||
//	    (x + w) >= MAX_VIEW_WIDTH ||
//	    (y + h) >= MAX_VIEW_HEIGHT)
//	{
//		fatal_error("CUE ERROR! rectangle off screen. x = %ld, y = %ld, w = %ld, h = %ld,\n",
//													x, y, w, h);
//	}
//#else
	// Clip!
	if (x < 0)
		x = 0;
	else
	if (x >= MAX_VIEW_WIDTH)
		x = MAX_VIEW_WIDTH - 1;
	
	if (y < 0)
		y = 0;
	else
	if (y >= MAX_VIEW_HEIGHT)
		y = MAX_VIEW_HEIGHT - 1;
	
	if ((x + w) >= MAX_VIEW_WIDTH)
		w = (MAX_VIEW_WIDTH - 1) - x;
	
	if ((y + h) >= MAX_VIEW_HEIGHT)
		h = (MAX_VIEW_HEIGHT - 1) - y;
//#endif
	
	sptr = screen + (screen_buffer_width * y) + x;

	/* Note this access screen directly for speed */
	do
	{
		// GWP ww = w;
		// GWP
		// GWP do
		// GWP {
		// GWP 	(*sptr++) = (char)s;
		// GWP } while (ww--);

		// GWP sptr += screen_buffer_width-w-1;
		
		memset(sptr, s, w);
		sptr += screen_buffer_width;
	} while (h--);
}

/* ========================================================================
   Function    - color_box
   Description - draws a box with pixel wide lines
   Returns     - void
   ======================================================================== */
void color_box (LONG x,LONG y,LONG w,LONG h,LONG s)
{
	if (x<0 || y<0 || w<0 || h<0 || s<0)
		return;
	line(x,y,x+w,y,s);		//from UL to UR
	line(x,y,x,y+h,s); //from UR to LL
	line(x+w,y+h,x+w,y,s); //from UL to LL
	line(x+w,y+h,x,y+h,s);   //from LL to LR
	
}

/* =======================================================================
	Function    - zoom_color_edged_rect
	Description - zoom a rectangle
	Returns     - void
	=======================================================================  */

void zoom_color_edged_rect (LONG x, LONG y, LONG w, LONG h, LONG s, SHORT hDest)
{
		LONG scale_rate=MAX_SCALE_RATE ;
		while(scale_rate >= 1)
		{
			DrawBitmap(x-1, y-1, hDest, 0, 0, w+3, h+2);
			color_edged_rect ((x+(w-w/(scale_rate))/2),
									(y+(h-h/(scale_rate))/2),
									w/(scale_rate),
									h/(scale_rate),
									s);
			scale_rate = scale_rate>>RATE;
			update_screen();
			DirectDrawPreFrame();
		}
}	

/* =======================================================================
	Function    - zoomout_color_edged_rect
	Description - zoom a rectangle
	Returns     - void
	=======================================================================  */
	
void zoomout_color_edged_rect (LONG x, LONG y, LONG w, LONG h, LONG s, SHORT hDest)
{
		LONG scale_rate=1;
		while(scale_rate <= MAX_SCALE_RATE)
		{
			DrawBitmap(x-1, y-1, hDest, 0, 0, w+3, h+2);
			color_edged_rect ((x+(w-w/(scale_rate))/2),
									(y+(h-h/(scale_rate))/2),
									w/(scale_rate),
									h/(scale_rate),
									s);
			scale_rate = scale_rate<<RATE;
			update_screen();
			DirectDrawPreFrame();
		}
		DrawBitmap(x-1, y-1, hDest, 0, 0, w+3, h+2);
		update_screen();
		DirectDrawPreFrame();

}	
	



/* =======================================================================
   Function    - color_edged_rect
   Description - draws a 'shadowed' rectangle
   Returns     - void
   ======================================================================== */
void color_edged_rect (LONG x,LONG y,LONG w,LONG h,LONG s)
{
	color_rect (x+1,y+1,w-1,h-1,s);

	color_rect (x,y,w,1,s+2);
	color_rect (x+w,y,1,h+1,s+2);
	color_rect (x,y+1,1,h-1,s-2);
	color_rect (x,y+h,w,1,s-2);
	
	if (fHighRes)
	{
		color_rect (x-1,y-1,w+2,1,s+2);
		color_rect (x+w-1,y+1,1,h-1,s+2);
		color_rect (x-1,y,1,h+1,s-2);
		color_rect (x+1,y+h-1,w-2,1,s-2);
	}
}

/* =======================================================================
   Function    - shade_rect
   Description - draws a shaded rectangle
   Returns     - void
   ======================================================================== */
void shade_rect (LONG x,LONG y,LONG w,LONG h,LONG s)
{
	CHAR *shade;
	LONG sx,sy;
	LONG x_end,y_end;
	PTR screen_y_start;

	if (x<0)
		x=0;
	if (y<0)
		y=0;
	
	/* turn w/h into real screen coords */
	x_end=x+w;
	y_end=y+h;

	if (x_end>MAX_VIEW_WIDTH)
		x_end=MAX_VIEW_WIDTH;
	if (y_end>MAX_VIEW_HEIGHT)
		y_end=MAX_VIEW_HEIGHT;
	

	shade=(CHAR *)&shade_table[256*s];
		
		/*Note this access screen directly for speed*/
	screen_y_start = &screen[x + (y*screen_buffer_width)];
	for(sx=x;sx<x_end;++sx, ++screen_y_start)
	{
		PTR sptr= screen_y_start;
		for(sy=y;sy<y_end;++sy, sptr += screen_buffer_width)
		{
			// GWP screen[sx+(sy*screen_buffer_width)]=shade[ screen[sx+(sy*screen_buffer_width)] ];
			if (*sptr!=254)
				*sptr=shade[ *sptr ];
		}
	}
}

/* =======================================================================
	Function    - zoom_shade_edged_rect
	Description - zoom a rectangle
	Returns     - void
	=======================================================================  */
void zoom_shade_edged_rect (LONG x, LONG y, LONG w, LONG h, LONG s, SHORT hDest)
{
		LONG scale_rate=MAX_SCALE_RATE ;
		while(scale_rate >= 1)
		{
			DrawBitmap(x-1, y-1, hDest, 0, 0, w+3, h+2);
			shade_edged_rect ((x+(w-w/(scale_rate))/2),
									(y+(h-h/(scale_rate))/2),
									w/(scale_rate),
									h/(scale_rate),
									s);
			scale_rate = scale_rate>>RATE;
			update_screen();
			DirectDrawPreFrame();
		}
}	
	
/* =======================================================================
	Function    - zoomout_shade_edged_rect
	Description - zoom a rectangle
	Returns     - void
	=======================================================================  */

void zoomout_shade_edged_rect (LONG x, LONG y, LONG w, LONG h, LONG s, SHORT hDest)
{
		LONG scale_rate=1;
		while(scale_rate <= MAX_SCALE_RATE)
		{
			DrawBitmap(x-1, y-1, hDest, 0, 0, w+3, h+2);
			shade_edged_rect ((x+(w-w/(scale_rate))/2),
									(y+(h-h/(scale_rate))/2),
									w/(scale_rate),
									h/(scale_rate),
									s);
			scale_rate = scale_rate<<RATE;
			update_screen();
			DirectDrawPreFrame();
		}
	  	DrawBitmap(x-1, y-1, hDest, 0, 0, w+3, h+2);
	  	update_screen();
		DirectDrawPreFrame();

}	
	

#if 0
void zoom_shade_edged_rect (LONG x, LONG y, LONG w, LONG h, LONG s, BOOL * f, SHORT * sr, BOOL * fZoom)
{
	LONG scale_rate = *sr;
	if(scale_rate)
	{
			(scale_rate) = w/2;
			if((scale_rate) > h/2) (scale_rate) = h/2;
	 		*f = FALSE;
	}
	
	
		if((scale_rate) <= 0)
		{
			(scale_rate)	= 1;
			*fZoom = FALSE;
			shade_edged_rect(x, y, w, h, s);
		}
  		else
		{
			shade_edged_rect ((x+(w-w/(scale_rate))/2),
									(y+(h-h/(scale_rate))/2),
									w/(scale_rate),
									h/(scale_rate),
									s);
			(scale_rate) = (scale_rate)/ 2;
		}
	
	*sr = (SHORT) scale_rate;
}	
	

#endif

/* =======================================================================
   Function    - shade_edged_rect
   Description - draws a 'shadowed' rectangle
   Returns     - void
   ======================================================================== */
void shade_edged_rect (LONG x,LONG y,LONG w,LONG h,LONG s)
{
	shade_rect (x+1,y+1,w-2,h-2,s);

	shade_rect (x,y,w,1,s+1);
	shade_rect (x+w,y,1,h+1,s+1);
	shade_rect (x,y+1,1,h-1,s-1);
	shade_rect (x,y+h,w,1,s-1);
	
	if (fHighRes)
	{
		shade_rect (x-1,y-1,w+2,1,s+1);
		shade_rect (x+w-1,y+1,1,h-1,s+1);
		shade_rect (x-1,y,1,h+1,s-1);
		shade_rect (x+1,y+h-1,w-2,1,s-1);
	}
}

/* ========================================================================
   Function    - SetArtPath
   Description - Set up the path of where the art and palette files are
   Returns     - void
   ======================================================================== */
void SetArtPath ( CSTRPTR pArtPath )
{
	/* copy the first 79 chars into pwad_path */
	strncpy(art_path, pArtPath, 79);
}

/* ======================================================================= */
