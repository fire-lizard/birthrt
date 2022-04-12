/* =®RM250¯======================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _ENGINT_H
#define _ENGINT_H

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_ENGINE_H)
#include "engine.h"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define TEXTURE_PATH			"TEXTURES\\"
#if defined(_EDIT)
#define MAX_TEXTURES			(230)
#else
#define MAX_TEXTURES			(200)
#endif
#define DARKEST				31

/* divide a 32bit fixed pt by 2^n */
#ifdef _WINDOWS
#define DARKEN_FACTOR		(16-fHighRes)
#else
#define DARKEN_FACTOR		(16-graphics_mode)
#endif

#define EXPAND_FACTOR (1)                  // use this to expand the map

#define MAX_SSECTORS	(5000)
#define MAX_SECTORS 	(2500)

#define INVISIBLE		(0)
#define VISIBLE			(1)
#define PARTIAL			(3)

#define FRONT			(0)
#define BACK			(1)

#define LOWER_TEXTURE	(1)
#define MIDDLE_TEXTURE	(2)
#define UPPER_TEXTURE	(3)

#define IMPASSABLE_LINE		0x01
#define MONSTER_BLOCK_LINE	0x02

#define NOT_DRAWN		(0)
#define DRAWN			(1)

#define SECT_VISIBLE(x)	(sector_visible[x])

#define NORMAL_TEX		0
#define SKY_TEX			1
#define TRANSP_TEX		2
#define CAMERA_TEX		4
#define HOMOGENOUS_TEX	5

// Values for status returned from get_texture.
// [d6-06-96 JPC]
#define GT_NO_TEXTURE      0
#define GT_FOUND_TEXTURE   1
#define GT_SKY_TEXTURE     2
#define GT_CAMERA_TEXTURE  3
#define GT_LOADED_TEXTURE  4
#define GT_GENERIC_TEXTURE 5

/* Values for FLICHEAD type */
#define FLC_TYPE			0xAF12u

/* Value for PREFIXHEAD type */
#define PREFIX_TYPE		0xF100

/* Value for FRAMEHEAD type */
#define FRAME_TYPE		0xF1FA

#define COLOR_256			4			/* 256 level color pallette info. (FLC only.) */
#define DELTA_FLC			7			/* Word-oriented delta compression. (FLC only.) */
#define COLOR_64			11			/* 64 level color pallette info. */
#define DELTA_FLI			12			/* Byte-oriented delta compression. */
#define BLACK_FRM			13			/* whole frame is color 0 */
#define BYTE_RUN			15			/* Byte run-length compression. */
#define LITERAL			16			/* Uncompressed pixels. */
#define PSTAMP				18			/* "Postage stamp" chunk. (FLC only.) */


#define TYPE_FLIC				1
#define TYPE_PCX				2


/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
typedef struct {
	SHORT		x, y;
} SPOINT;

typedef struct {
	LONG		x, y, z;
} POINT3D;

typedef struct{
	LONG		offset;
	LONG		size;
	char		name[8];
} IWAD_ENTRY;

typedef struct{
	SHORT		x,y;
	SHORT		angle,type,options;
} THING;

typedef struct{
	SHORT		a;
	SHORT		b;
	SHORT		flags;
	SHORT		special;
	SHORT		tag;
	SHORT		psdb;
	SHORT		psdt;
} LINEDEF;

//flags
#define IMPASSABLE_BIT		0x0001
#define MONSTER_BLOCK_BIT	0x0002
#define TWO_SIDED_BIT		0x0004
#define UPPER_UNPEGGED_BIT	0x0008
#define LOWER_UNPEGGED_BIT	0x0010
#define SECRET_BIT			0x0020
#define BLOCKS_SOUND_BIT	0x0040
#define NOT_ON_MAP_BIT		0x0080
#define DRAW_ON_MAP_BIT		0x0100
#define HAS_BEEN_CROSSED	0x0200




// [d6-05-96 JPC] Changed "char" to "unsigned char" because otherwise
// seg_to_texture_num will return a NEGATIVE number for texture numbers > 127,
// which causes a crash when we try to use the number as an index.
// [d6-13-96 JPC] Changed "unsigned char" to "UBYTE" for consistency.
typedef struct{
	SHORT	xoff, yoff;
	UBYTE	n1[8];		// The name of the texture.
	UBYTE	n2[8];		// The name of the texture.
	UBYTE	n3[8];		// The name of the texture.
	SHORT	sec_ptr;
} SIDEDEF;

// A tune'd structure for Birthright.
// Note: sizeof(BR_SIDEDEF) is < sizeof(SIDEDEF)
typedef struct {
	LONG	xoff;
	LONG	yoff;
	LONG	n1;		// The index into the texture array
	LONG	n2;		// The index into the texture array
	LONG	n3;		// The index into the texture array
#if defined (_EDIT)
	UBYTE	n1_noTexture;
	UBYTE	n2_noTexture;
	UBYTE	n3_noTexture;
#endif
	LONG	sec_ptr;
} BR_SIDEDEF;

typedef struct{
	SHORT		x,y;
} VERTEX;

typedef POINT BR_VERTEX;

typedef struct{
	SHORT		a;
	SHORT		b;
	SHORT		f;
	SHORT		lptr;  		/*f is angle 0-0xffff*/
	SHORT		flip;
	SHORT		txoff;  /*txoff is offset for texture*/
} SEG;

typedef struct{
	SHORT		n,o;
} SSECTOR;

// use LONG's for speed in Birthright -GWP-
typedef struct {
	LONG	n;
	LONG	o;
} BR_SSECTOR;

typedef struct{
	SHORT		x,y;
	SHORT		a,b;	// a is dx, and b is dy
	SHORT		minyf,maxyf,minxf,maxxf; // the bounding box of f
	SHORT		minyb,maxyb,minxb,maxxb; // the bounding box of r  Got it?
	USHORT	f,r;
} NODE;

// Converted to LONG's for Speed.
typedef struct{
	LONG	x,y;
	LONG	a,b;
	// GWP Unused SHORT	minyf,maxyf,minxf,maxxf;	// Left these as shorts for size.
	// GWP Unused SHORT	minyb,maxyb,minxb,maxxb;
	ULONG	f,r;
} BR_NODE;


// [d6-06-96 JPC] Changed "char" to "UBYTE" to fix a crash bug when texture
// index is > 127.
typedef struct{
	SHORT		fh,ch;   					/*floor -n- ceiling*/
	UBYTE		f_flat[8],c_flat[8];  /*names for texures*/
	SHORT		light,special,tag;
} SECTOR;

// Changed fh and ch to LONG's for speed in Birthright -GWP-
// Note: sizeof(SECTOR) == sizeof(BR_SECTOR)
typedef struct{
	LONG		fh;
	LONG		ch;   					/*floor -n- ceiling*/
	LONG		f_flat;
	LONG		c_flat;  /*index's for texures*/
#if defined(_EDIT)
	UBYTE		f_flat_noTexture;
	UBYTE		c_flat_noTexture;
#endif
	LONG		light;
	LONG		special;
	SHORT		tag;
} BR_SECTOR;

//typedef struct{
//	SHORT		a,b,c,d;
//} REJECT;

typedef struct{
	SHORT		xo,yo;
	SHORT		cols,rows;
} BLOCKMAP;

typedef struct{
	LONG		texture;
	LONG		ttexture;
	LONG		ftexture;
	LONG		ctexture;
	LONG		light;
	LONG		fh;
	LONG		ch;
	LONG		fth;
	LONG		cth;
	LONG		sect;
} SIDE;

typedef struct{
	LONG		ox,oy;
	LONG		w,h;
	char *	b;
} BITM;

typedef struct{
	LONG		w,h;
	char		name[10];
	SHORT		t;
	SHORT	   frameHandle;					// [d10-08-96 JPC] for animated textures
	SHORT		iFrame;							// [d10-08-96 JPC] for animated textures
//	LONG		sky;
//	ULONG		scale;
//	LONG		type;
	USHORT	scale;
	USHORT	type;
} TEXTURE;


/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

/* blockm.c */
SHORT* get_blockm (LONG px1,LONG py1);
void show_block_lines (void);

/* convert.c */
LONG segs_match(SHORT a,SHORT b);
LONG single_sided(LONG s);
LONG ssector_to_hei(LONG s);
LONG sector_to_fh(ULONG sec);
LONG sector_to_ch(ULONG sec);
LONG seg_to_ceil(LONG s,LONG side);
LONG seg_to_sector(LONG s,LONG side);
LONG line_to_sector(LONG l,LONG side);
void seg_to_tname(LONG s,LONG side,char *n);
LONG sector_to_light(ULONG sect);
LONG seg_two_sided(LONG s);
LONG seg_to_texture_num(LONG seg,LONG side,LONG pos);
LONG seg_to_txoff(LONG seg,LONG side); // [d6-07-96 JPC]
LONG seg_to_tyoff(LONG seg,LONG side);
void seg_to_txoff_tyoff(LONG seg,LONG side, LONG * xoff, LONG * yoff);

/* extent.c */
void clear_extents(void);
void mark_sect_visible(LONG s);
LONG sect_visible(LONG s);
LONG extent_visible(LONG x1,LONG x2);
void add_extent(LONG x1,LONG x2);
void fill_extent(LONG x);
LONG clip_span(LONG x,LONG * y1,LONG * y2,LONG *clipped);
LONG clip_obj(LONG x,LONG * y1,LONG * y2,LONG *clipped);

/* map.c */
void draw_arrow(void);

/* point.c */
LONG point_relation(LONG n,LONG x,LONG y);
void proj(POINT *p,LONG z);
void projection(POINT *p,LONG z, LONG distanceToCamera);
void rot8dbl (POINT *p);
// [d8-02-96 JPC] Added x and y parameters to the following:
LONG back_face_point(POINT *a,POINT *b, LONG x, LONG y);
LONG back_face_vertex(LONG a, LONG b, LONG x, LONG y);

/* render.c */
LONG clip_obj(LONG x,LONG * y1,LONG * y2,LONG *clipped);
void draw_node(LONG n);
void draw_ssector(LONG sect);
void process_seg(LONG s);
LONG calc_x_texture_info(LONG *xsrc_inc,LONG *src_x);
// [d7-12-96 JPC] draw_wall is now a static function in RENDER.C.
// LONG draw_wall(LONG seg);
// [d6-04-96 JPC] draw_lower_wall and draw_upper_wall functions are
// now static functions in RENDER.C, AND their parameters have changed.
// LONG draw_lower_wall(LONG seg);
// LONG draw_upper_wall(LONG seg);
LONG draw_backface(LONG type);
void add_floor_span(LONG x,LONG y,LONG sect);
void add_ceil_span(LONG x,LONG y,LONG sect);

/* spans.c */
void clear_spans (void);
void draw_spans (void);
void add_span_event_start (ULONG x,ULONG y,ULONG ye,ULONG sector);
void add_span_event_end (ULONG x,ULONG y,ULONG ye,ULONG sector);
void allocate_spans();

/* statisti.c */
void init_statistics(void);
void close_statistics(void);

/* textures.c */
LONG get_texture(char *in, ULONG * o_status); // [d6-06-96 JPC]
LONG get_floor_texture(char *in);
void load_sky_textures (void);
void purge_all_textures (void);
void scale_col_ttop(LONG sx,LONG dx,LONG dy,LONG dye, LONG clipped,LONG src_inc);
void scale_col_tbot(LONG sx,LONG dx,LONG dy,LONG dye, LONG clipped,LONG src_inc);

void scrape(LONG x,	// column to draw in
			LONG y,	// where to end drawing
			LONG ye //ye is where you start drawing
			);
			
void hscrape(LONG x,LONG y,LONG xe);
void WriteErrFile(char *n);
SHORT LoadAnimatedTextureStub (CSTRPTR szFileName,BOOL fSetPal,BOOL fLockRes,BOOL fRotated, LONG iResFileSlot);

/* things.c */
void draw_thing_spans(void);
// JPC void draw_thing (LONG t);
void draw_things (LONG ss);
/* ------------------------------------------------------------------------
   Global Vars
   ------------------------------------------------------------------------ */
extern CSTRPTR				level_name;
extern char					pwad_name[40];

extern THING *				things;
extern LINEDEF *			linedefs;
extern BR_SIDEDEF *			sidedefs;
extern BR_VERTEX *			vertexs;
extern SEG *				segs;
extern BR_SSECTOR *			ssectors;
extern BR_NODE *			nodes;
extern BR_SECTOR *			sectors;
extern UBYTE *			rejects;

extern BLOCKMAP 			blockm_header;
extern USHORT * 			blockm_offs;
extern SHORT * 			blockm;
extern LONG 				blockm_size;

extern ULONG				tot_things;
extern ULONG				tot_linedefs;
extern ULONG				tot_sidedefs;
extern ULONG				tot_vertexs;
extern ULONG				tot_segs;
extern ULONG				tot_ssectors;
extern ULONG				tot_sectors;
extern ULONG				tot_rejects;
extern ULONG				tot_blockms;
extern LONG					bot_extent_sect[MAX_VIEW_WIDTH];
extern LONG					sector_visible[MAX_SECTORS];

extern LONG					base_light;

extern UBYTE * 			shade;
extern LONG					SectLight;
extern LONG					NSEWLight;

extern LONG					render_perspect;
extern LONG					render_center_x, render_center_y;
extern LONG					render_bot, render_top;

extern LONG					frames;
extern LONG					last_texture;
extern LONG					last_obj_graphic;

extern	TEXTURE textures[];

// path information
extern CHAR art_path[];
extern CHAR pwad_path[];

// [d10-21-96 JPC] "Mirror" variables to suppress redundant draws of
// duplicate (but mirrored) line segments.  These variables are
// defined in LEVEL.C.
typedef LONG SEGMIRROR;
typedef ULONG SEGMIRROR_DRAWN;
extern SEGMIRROR * segMirror;
extern SEGMIRROR_DRAWN * segMirrorDrawn;


#if defined (__cplusplus)
}
#endif

#endif // _ENGINT_H
