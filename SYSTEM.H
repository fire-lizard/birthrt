/* =======================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: SYSTEM.H   -Header file for all source files
   Author: Chris Phillips & Wes Cumberland
   ========================================================================
   Contains the following internal functions:
   Contains the following general functions:
   ======================================================================== */
#ifndef _SYSTEM_H
#define _SYSTEM_H

/* ------------------------------------------------------------------------
	Type defines
   ------------------------------------------------------------------------ */
#include <stdio.h>		// for FILE*.
#include <stdlib.h>		// for rand.

#ifdef _WINDOWS
//#include <windows.h>
#endif

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_COLORS_H)
#include "colors.h"
#endif

#if !defined(_MEMMANAG_H)
#include "memmanag.h"
#endif

#if !defined(_RESMANAG_H)
#include "resmanag.h"
#endif

#if !defined(_REGIONS_H)
#include "regions.h"
#endif

#if !defined(_MENUPROT_H)
#include "menuprot.h"
#endif

#if !defined(_REDBOOK_H)
#include "redbook.h"
#endif

#if !defined(_FOPEN_H)
#include "fopen.h"
#endif

#if defined(_WINDOWS) && !defined(_RANDOM_H)			//---- random override for logging
#include "random.h"
#endif


/* ------------------------------------------------------------------------
	Misc Defines
   ------------------------------------------------------------------------ */
/* Scale defines */
#define UNITARY_SCALE				5
#define UNITARY_PEOP_SCALE			3
#define NORMAL_LENS					4
#define FULL_SCALE					(1<<8)

#define MIN_MEMORY_TO_RUN				(4096000)
#define MIN_MEMORY_TO_FULL_ROTATION		( 6*1024*1024)
#define MIN_MEMORY_TO_MEDRES_ANI		( 7*1024*1024)
#define MIN_MEMORY_TO_MEDRES_TEXTURES	(10*1024*1024)
#define MIN_MEMORY_TO_SMALLMAP			(12*1024*1024)
#define MIN_MEMORY_TO_HIRES_ANI			(15*1024*1024)
#define MIN_MEMORY_TO_HIRES_TEXTURES	(20*1024*1024)

#define MIN_MEMORY_32MEG_WINDOWS	(16*1024*1024)
#define MIN_MEMORY_16MEG_WINDOWS	(8*1024*1024)

/* ------------------------------------------------------------------------
	General Defines
   ------------------------------------------------------------------------ */
#define fNOERR				((SHORT)0)
#define fERROR				((SHORT)-1)

/* mark file as unavailable for future referance */
#define	fFILE_NOT_FOUND	-2

// Macro for testing bad memory pointers.
#define IsPointerGood(x)		((((x) == 0) || ((x) == ((void *)fERROR)) || ((x) == ((void *)fFILE_NOT_FOUND))) ? FALSE : TRUE)

#define fREPORT_ALL		0xFF
#define fREPORT_MEMMGR	0x01
#define fREPORT_RESMGR	0x02
#define fREPORT_SYSTEM	0x04
#define fREPORT_GRAPH	0x08
#define fREPORT_OTHER	0x10

#define DPMI_INT	0x31

#define FONT_HEIGHT2 (8)

/* ------------------------------------------------------------------------
	Font Defines
   ------------------------------------------------------------------------ */
#define FONT_PATH				"fonts\\"
#define FONT_SANS_5PT		(0)
#define FONT_SANS_6PT		(1)
#define FONT_SANS_8PT		(2)
// both SANS_10 and SANS_12 map to the 10pt. font sans10.pcx
#define FONT_SANS_10PT		(3)
#define FONT_SANS_12PT		(3)
#define FONT_SANS_16PT		(4)
#define FONT_TITL_8PT		(5)
#define FONT_TITL_10PT		(6)
#define FONT_TITL_16PT		(7)
#define FONT_TITL_20PT		(8)
#define FONT_TIMS_7PT		(9)
#define FONT_TIMS_8PT		(10)
#define FONT_TIMS_10PT		(11)
#define FONT_TIMS_30PT		(12)


/* ------------------------------------------------------------------------
	Defines for resource manager functions
   ------------------------------------------------------------------------ */
/* file I/O flag defines */
#define MODE_MAKEFILE	O_CREAT|O_RDWR|O_BINARY,S_IREAD|S_IWRITE
#define MODE_READFILE	O_RDONLY|O_BINARY

/* header type defines */
#define TYPEANIM		0x000A
#define TYPEBITM		0x000B

#define FixWord(w)		(w) = ( (((USHORT)(w)) >> 8) | (((USHORT)(w)) << 8) )
#define FixLong(l)		(l) = ((l)>>24)|(((l)&0xFF0000L)>>8)|(((l)&0xFF00L)<<8)|((l)<<24)

#define SWAPINT(a,b)	{LONG x;  x=(a); (a)=(b); (b)=x;}

#define BUILD_LONG(s1, s2)		((0x00000000 | (s1)<<16) | (s2))
#define COMBINE_SHORTS(s1,s2,l)	{((l) = (0x00000000 | (s1)<<16) | (s2));}
#define SPLIT_LONG(l,s1,s2)		{((s1) = (short)((0xFFFF0000 & (l))>>16));\
								 ((s2) = (short)(0x0000FFFF & (l)));}
#define HIGH_LONG(l)			((short)((0xFFFF0000 & (l))>>16))								
#define LOW_LONG(l)				((short)(0x0000FFFF & (l)))

/* ------------------------------------------------------------------------
	Defines for graphic functions
   ------------------------------------------------------------------------ */
/* non-standard mode defines */
#define _WRES256COLOR	0x14

/* draw routine function constants */
#define COPY				0x101
#define DRAW				0x102
#define REVD				0x103
#define COMP				0x104
#define RMAP				0x105
#define RVRM				0x106

/* defines for Print */
#define fLEFTKERN			0x40
#define fRIGHTKERN		0x20


/************************************************************
	Defines for type of installation selected by user, CD-ROM drive letter
************************************************************/
#define INSTALL_SMALL   0
#define INSTALL_MEDIUM 1
#define INSTALL_LARGE   2

/* ------------------------------------------------------------------------
	Structure defines
   ------------------------------------------------------------------------ */
typedef struct colorspec
{
	UBYTE		bRed;
	UBYTE		bGreen;
	UBYTE		bBlue;
} COLORSPEC, *PALETTE;

/* BITM header structure */
typedef struct bitmhdr
{
	SHORT		w;				/* raster width & height in pixels */
	USHORT	h;
	SHORT		scale;		/* current scale discovered */
	SHORT		x_ctr_pt;	/* center point discovered */
	SHORT		type;			/* set to TYPEBITM */
} BITMHDR, *BITMPTR, ARECT, *RECTPTR;


/* Flic Chunk Header */
typedef struct
{
	LONG		size;			/* Size of chunk including header. */
	USHORT	type;			/* Value from ChunkTypes above. */
} CHUNKHEAD, *CHUNKHEADPTR;

/* Flic Frame Header */
typedef struct
{
	LONG		size;				/* Size of frame including header. */
	USHORT	type;				/* Always FRAME_TYPE */
	SHORT		chunks;			/* Number of chunks in frame. */
	UBYTE		reserved[8];	/* Always 0. */
} FRAMEHEAD, *FRAMEHEADPTR;

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
#ifndef ABS
#define ABS(x)			(((x)<0) ? -(x) : (x))
#endif
#ifndef SGN
#define SGN(x)			(((x)<0) ? -1 : (((x)==0) ? 0 : 1))
#endif

#define SAME_SIGNS( a, b ) (((LONG) ((ULONG) a ^ (ULONG) b)) >= 0 )

#define random(num) (SHORT)(((LONG)rand()*(num))/(0x8000U))

#define MAX(a,b)			( ((a) > (b)) ? (a) : (b) )
#define MIN(a,b)			( ((a) < (b)) ? (a) : (b) )

// adjusts a value for screen resolution
//GEH #define X_RES_ADJ(x)	((x) * window_width / MAX_VIEW_WIDTH)
//GEH #define Y_RES_ADJ(y)	((y) * window_height / MAX_VIEW_HEIGHT)
// I fixed the screen to always be MAX_VIEW_WIDTH/HEIGHT
#define X_RES_ADJ(x)	((x))
#define Y_RES_ADJ(y)	((y))


#define ResetSplitScreen()	SetSplitScreen(0x3FF,0)
#define ASetPalette()		SetPalRegs((PTR)CurPal); MakeCompPal()
#define CloseImag(i)			DisposBlock(i)
#define CloseBitm(i)			DisposBlock(i)
#define OpenBitm(w,h)		_OpenBitm((w),(h),FALSE)
#define OpenLockedBitm(w,h) _OpenBitm((w),(h),TRUE)

#define TextHeight(f,s)		((Print(NULL, 0, 0, (f), 0, (s)) >> 10) & 0x3FF)
#define TextWidth(f,s)		(Print(NULL, 0, 0, (f), 0, (s)) & 0x3FF)

#define SetLight(a)							shade=&shade_table[(a)*256]

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

#if defined(_RELEASE)
// over-ride the prototype for the release
int	printf( const char *__format, ... );
#endif

/* assert.c */
// over-ride the assert
#if defined(_RELEASE)
#define assert(exp)
#else
void _assert(BOOL fTest,char *FileName, SHORT LineNum);
#define assert(exp) _assert((exp),__FILE__,__LINE__)
#endif

/* timer.c */
void init_timers();
void close_timers(void);
void add_timer(LONG freq,PFV func);
void remove_timer(PFV func);
void run_timers();
void TickDelay(LONG);


/* system.c */
void SaveVideoSet (void);
void RestoreVideoSet (void);
void set_margin_size (LONG l,LONG r,LONG t,LONG b);
void get_margin_size (LONG *l,LONG *r,LONG *t,LONG *b);
void set_screen_size (LONG vw,LONG vh);
void set_view_size (LONG vx,LONG vy,LONG vw,LONG vh);
void plot (LONG x,LONG y,LONG c);
LONG get_pixel (LONG x,LONG y);
void line (LONG x0, LONG y0, LONG x1, LONG y1,LONG color);
ULONG detect_scale (SHORT t, SHORT h, SHORT w);
LONG gplot_char(LONG c, LONG x, LONG y, LONG color);
LONG gtext_width(char *text);
LONG gtext_height(char *text);
SHORT gprint_text(LONG x,LONG y,char *text,LONG color);
void print_text_centered(LONG x,LONG y,char * text,LONG color);
SHORT print_textf(LONG x, LONG y, LONG color, const char *format, ...);
void init_gfont(SHORT type_and_size);
void ClearRemapTable(void);
void SetRemapTable(USHORT);

void BltBitmap (SHORT x, SHORT y, SHORT iBitm, SHORT bx, SHORT by, SHORT w, SHORT h);
void DrawBitm (SHORT x, SHORT y, SHORT iBitm, SHORT xb, SHORT yb, SHORT w, SHORT h);
void DrawBitmap (SHORT x, SHORT y, SHORT iBitm, SHORT xb, SHORT yb, SHORT w, SHORT h);
void ScaleBitmap (SHORT, SHORT, SHORT, SHORT, SHORT, SHORT, SHORT, SHORT);
void ScaleBitmapX (SHORT, SHORT, SHORT, SHORT, SHORT, SHORT, SHORT, SHORT);
void ScaleBitmapY (SHORT, SHORT, SHORT, SHORT, SHORT, SHORT, SHORT, SHORT);

SHORT SaveBitmap (SHORT x, SHORT y, SHORT w, SHORT h);
void ZoomBitmap(SHORT x, SHORT y, SHORT iBitm, SHORT xb, SHORT yb,
					 SHORT w, SHORT h, SHORT hDest, POINT start,
					 POINT bck, SHORT bckw, SHORT bckh);
void ZoomOutBitmap(SHORT x, SHORT y, SHORT iBitm, SHORT xb, SHORT yb,
					 SHORT w, SHORT h, SHORT hDest, POINT start,
					 POINT bck, SHORT bckw, SHORT bckh);
void SpinInBitmap(SHORT x, SHORT y, SHORT iBitm, SHORT xb, SHORT yb,
					 SHORT w, SHORT h, SHORT hDest, POINT start,
					 POINT bck, SHORT bckw, SHORT bckh);
void SpinOutBitmap(SHORT x, SHORT y, SHORT iBitm, SHORT xb, SHORT yb,
					 SHORT w, SHORT h, SHORT hDest, POINT start,
					 POINT bck, SHORT bckw, SHORT bckh);
void ScrollOpenBitmap(SHORT x, SHORT y, SHORT iBitm, SHORT xb, SHORT yb,
					 SHORT w, SHORT h, SHORT hDest, POINT start,
					 POINT bck, SHORT bckw, SHORT bckh);
void ScrollClosedBitmap(SHORT x, SHORT y, SHORT iBitm, SHORT xb, SHORT yb,
					 SHORT w, SHORT h, SHORT hDest, POINT start,
					 POINT bck, SHORT bckw, SHORT bckh);

SHORT _OpenBitm (SHORT w, USHORT h, BOOL fLocked);
BOOL Exists( char *filename);
void dec_view_size(LONG arg1, LONG arg2);
void inc_view_size(LONG arg1, LONG arg2);
void inc_lens (LONG, LONG);
void dec_lens (LONG, LONG);

void crease (LONG x, LONG y, LONG w, LONG h);
extern void DrawLittleButton(LONG, LONG, LONG, LONG, LONG);

/* game.cpp */
void GAMEToggleMainMenu(LONG fState, LONG);
char* GetSourcePath(void);
BOOL CheckCDError(void);

/* vdpmi.c */
void * D32DosMemAlloc ( ULONG size );

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
// for comment logging
extern BOOL fLogComment;

// for critical errors due to CD removed
extern BOOL fGraphInitialized;	// main.cpp
extern BOOL fIsFadedOut;

extern char CDDrive[4];
extern SHORT InstallationType;   /*main.cpp*/
extern char InstallPath[256];
extern COLORSPEC		CurPal[256];
extern BOOL				fLowMemory;				/* memmanag.c */
extern BOOL				fMinMemory;				/* memmanag.c */
extern BOOL				fSmallMap;				/* memmanag.c */
extern BOOL				fRestrictAni;			/* memmanag.c */
extern BOOL				fLowResAni;				/* memmanag.c */
extern BOOL 			fMedResAni;				/* memmanag.c */
extern BOOL 			fLowResTextures;		/* memmanag.c */
extern BOOL 			fMedResTextures;		/* memmanag.c */
extern BOOL				fHighResCombat;
extern LONG  			screen_buffer_width;	/* system.c */
extern LONG  			screen_buffer_height;
extern LONG				graphics_mode;
extern PTR				screen;
extern LONG				lens_factor;

/*	======================================================================= */


#if defined (__cplusplus)
}
#endif
#endif // _SYSTEM_H
