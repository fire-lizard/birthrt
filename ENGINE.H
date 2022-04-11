/* ®RM250¯ */
/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _ENGINE_H
#define _ENGINE_H

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
//map.h!
#if !defined(_MAP_H)
#include "map.h"
#endif

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#include <limits.h>

#if !defined(_THINGS_H)
#include "things.h"
#endif

#if !defined(_DOORS_H)
#include "doors.h"
#endif

#if !defined(_SECTORS_H)
#include "sectors.h"
#endif

#if !defined(_WADTHING_H)
#include "wadthing.h"
#endif

#if !defined(_2DMOUSE_H)
#include "2dmouse.h"
#endif

#if !defined(_BUMP_H)
#include "bump.h"
#endif

#if !defined(_PLAYER_HXX)
#include "player.hxx"
#endif

#if !defined(_CAMERA_H)
#include "camera.h"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define SVGA 			1
#define MAX_VIEW_WIDTH 	640
#define MAX_VIEW_HEIGHT	480
//#define VIEW_WIDTH 	MAX_VIEW_WIDTH
//#define VIEW_HEIGHT 	MAX_VIEW_HEIGHT

#ifdef _VC4
// Define F_OK for Visual C++ 4.0 compatibility.
// F_OK is used in the "access" function.
// [d7-29-96 JPC]
#define F_OK 0
#endif


/* types for player starts */
#define TYPE_PLAYERSTART1	-1L
#define TYPE_PLAYERSTART2	-2L
#define TYPE_PLAYERSTART3	-3L
#define TYPE_PLAYERSTART4	-4L
#define TYPE_DEATHMATCH		11L

/* types for mouse_found_type */
#define TYPE_WALL		1
#define TYPE_LOWER		2
#define TYPE_UPPER		3
#define TYPE_FLOOR		4
#define TYPE_CEILING	5

/* animation control defines */
#define PLAY_FLAG       0xF0
#define LOOP_FLAG		0x80
#define REWIND_FLAG		0x40
#define START_FLAG		0x20
#define FRAME_MASK		0x1F
#define IDLE_FRAME		0x00

#define CHANGEANIM       1
#define SAMEANIM         0

/* ingore this argument value */
#define NO_CHANGE		LONG_MAX


/* ------------------------------------------------------------------------
	Key defines
   ------------------------------------------------------------------------ */
/* scan codes for key handler */
// -----------------------------------------------------------------
// -- NOTE: foriegn keyboards are different as follows:
// -- 
// -- FRENCH
// -- a -> q
// -- z -> w
// -- q -> a
// -- m -> ;
// -- w -> z
// -- , -> m
// -- ; -> ,
// -- : -> .
// -- = -> /
// -- 
// -- GERMAN
// -- z -> y
// -- y -> z
// -- 
// -----------------------------------------------------------------

#ifdef _WINDOWS
	#define NO_KEY				0
	#define KEY_ESCAPE		VK_ESCAPE
	#define KEY_1				'1'
	#define KEY_2				'2'
	#define KEY_3				'3'
	#define KEY_4				'4'
	#define KEY_5				'5'
	#define KEY_6				'6'
	#define KEY_7				'7'
	#define KEY_8				'8'
	#define KEY_9				'9'
	#define KEY_0				'0'
	#define KEY_MINUS			0xBD	// '-'
	#define KEY_EQUAL			0xBB	// '='
	#define KEY_BACKSPACE	VK_BACK
	#define KEY_TAB 			VK_TAB
	#define KEY_Q				'Q'
	#define KEY_W				'W'
	#define KEY_E				'E'
	#define KEY_R				'R'
	#define KEY_T				'T'
	#define KEY_Y				'Y'
	#define KEY_U				'U'
	#define KEY_I				'I'
	#define KEY_O				'O'
	#define KEY_P				'P'
	#define KEY_LEFTSQUARE		0xDB	// '['
	#define KEY_RIGHTSQUARE 	0xDD	// ']'
	#define KEY_RETURN		VK_RETURN
	#define KEY_ENTER			VK_RETURN
	#define KEY_CTRL			VK_CONTROL
	#define KEY_CONTROL		VK_CONTROL
	#define KEY_A				'A'
	#define KEY_S				'S'
	#define KEY_D				'D'
	#define KEY_F				'F'
	#define KEY_G				'G'
	#define KEY_H				'H'
	#define KEY_J				'J'
	#define KEY_K				'K'
	#define KEY_L				'L'
	#define KEY_SEMICOLON	';'
	#define KEY_QUOTE			'"'
	#define KEY_BACKQUOTE	'`'
	#define KEY_LEFTSHIFT	VK_SHIFT
	#define KEY_BACKSLASH	'\\'
	#define KEY_Z				'Z'
	#define KEY_X				'X'
	#define KEY_C				'C'
	#define KEY_V				'V'
	#define KEY_B				'B'
	#define KEY_N				'N'
	#define KEY_M				'M'
	#define KEY_COMMA			','
	#define KEY_PERIOD		'.'
	#define KEY_SLASH			'/'
	#define KEY_RIGHTSHIFT	VK_SHIFT
	#define KEY_PRINTSCREEN VK_SNAPSHOT
	#define KEY_ALT 			VK_MENU
	#define KEY_SPACEBAR		VK_SPACE
	#define KEY_SPACE			VK_SPACE
	#define KEY_CAPSLOCK		VK_CAPITAL
	#define KEY_F1				VK_F1
	#define KEY_F2				VK_F2
	#define KEY_F3				VK_F3
	#define KEY_F4				VK_F4
	#define KEY_F5				VK_F5
	#define KEY_F6				VK_F6
	#define KEY_F7				VK_F7
	#define KEY_F8				VK_F8
	#define KEY_F9				VK_F9
	#define KEY_F10 			VK_F10
	#define KEY_NUMLOCK		VK_NUMLOCK
	#define KEY_SCROLLLOCK	VK_SCROLL
	#define KEY_UPLEFT		VK_HOME	/* numeric keypad 7, or home */
	#define KEY_UP				VK_UP	/* numeric keypad 8, or up arrow */
	#define KEY_UPRIGHT		VK_PRIOR	/* numeric keypad 9, or pgup */
	#define KEY_OTHERMINUS	VK_SUBTRACT	/* "-" by numeric keypad */
	#define KEY_LEFT			VK_LEFT	/* numeric keypad 4, or left arrow */
	#define KEY_CENTER		VK_CLEAR	/* numeric keypad 5 */
	#define KEY_RIGHT			VK_RIGHT	/* numeric keypad 6, or right arrow */
	#define KEY_OTHERPLUS	VK_ADD	/* "+" by numeric keypad */
	#define KEY_DOWNLEFT		VK_END	/* numeric keypad 1, or end */
	#define KEY_DOWN			VK_DOWN	/* numeric keypad 2, or down arrow */
	#define KEY_DOWNRIGHT	VK_NEXT	/* numeric keypad 3, or pgdn */
	#define KEY_INSERT		VK_INSERT
	#define KEY_DELETE		VK_DELETE
#else
	#define NO_KEY				0
	#define KEY_ESCAPE		1
	#define KEY_1				2
	#define KEY_2				3
	#define KEY_3				4
	#define KEY_4				5
	#define KEY_5				6
	#define KEY_6				7
	#define KEY_7				8
	#define KEY_8				9
	#define KEY_9				10
	#define KEY_0				11
	#define KEY_MINUS			12
#ifdef _FRENCHVER	
	#define KEY_EQUAL			53
#else
	#define KEY_EQUAL			13
#endif
	#define KEY_BACKSPACE	14
	#define KEY_TAB 			15
#ifdef _FRENCHVER	
	#define KEY_Q				30
#else
	#define KEY_Q				16
#endif
#ifdef _FRENCHVER	
	#define KEY_W				44
#else
	#define KEY_W				17
#endif
	#define KEY_E				18
	#define KEY_R				19
	#define KEY_T				20
#ifdef _GERMANVER	
	#define KEY_Y				44
#else
	#define KEY_Y				21
#endif
	#define KEY_U				22
	#define KEY_I				23
	#define KEY_O				24
	#define KEY_P				25
	#define KEY_LEFTSQUARE	26	/* "[" */
	#define KEY_RIGHTSQUARE 27	/* "]" */
	#define KEY_RETURN		28
	#define KEY_ENTER			28
	#define KEY_CTRL			29
	#define KEY_CONTROL		29
#ifdef _FRENCHVER	
	#define KEY_A				16
#else
	#define KEY_A				30
#endif
	#define KEY_S				31
	#define KEY_D				32
	#define KEY_F				33
	#define KEY_G				34
	#define KEY_H				35
	#define KEY_J				36
	#define KEY_K				37
	#define KEY_L				38
#ifdef _FRENCHVER	
	#define KEY_SEMICOLON	51
#else
	#define KEY_SEMICOLON	39
#endif
	#define KEY_QUOTE			40
	#define KEY_BACKQUOTE	41
	#define KEY_LEFTSHIFT	42
	#define KEY_BACKSLASH	43
#ifdef _FRENCHVER	
	#define KEY_Z				17
#else
	#ifdef _GERMANVER	
		#define KEY_Z			21
	#else
		#define KEY_Z			44
	#endif
#endif
	#define KEY_X				45
	#define KEY_C				46
	#define KEY_V				47
	#define KEY_B				48
	#define KEY_N				49
#ifdef _FRENCHVER	
	#define KEY_M			   39
#else
	#define KEY_M				50
#endif
#ifdef _FRENCHVER	
	#define KEY_COMMA			50
#else
	#define KEY_COMMA			51
#endif
	#define KEY_PERIOD		52
	#define KEY_SLASH			53
	#define KEY_RIGHTSHIFT	54
	#define KEY_PRINTSCREEN	55
	#define KEY_ALT 			56
	#define KEY_SPACEBAR		57
	#define KEY_SPACE			57
	#define KEY_CAPSLOCK		58
	#define KEY_F1				59
	#define KEY_F2				60
	#define KEY_F3				61
	#define KEY_F4				62
	#define KEY_F5				63
	#define KEY_F6				64
	#define KEY_F7				65
	#define KEY_F8				66
	#define KEY_F9				67
	#define KEY_F10 			68
	#define KEY_NUMLOCK		69
	#define KEY_SCROLLLOCK	70
	#define KEY_UPLEFT		71	/* numeric keypad 7, or home */
	#define KEY_UP				72	/* numeric keypad 8, or up arrow */
	#define KEY_UPRIGHT		73	/* numeric keypad 9, or pgup */
	#define KEY_OTHERMINUS	74	/* "-" by numeric keypad */
	#define KEY_LEFT			75	/* numeric keypad 4, or left arrow */
	#define KEY_CENTER		76	/* numeric keypad 5 */
	#define KEY_RIGHT			77	/* numeric keypad 6, or right arrow */
	#define KEY_OTHERPLUS	78	/* "+" by numeric keypad */
	#define KEY_DOWNLEFT		79	/* numeric keypad 1, or end */
	#define KEY_DOWN			80	/* numeric keypad 2, or down arrow */
	#define KEY_DOWNRIGHT	81	/* numeric keypad 3, or pgdn */
	#define KEY_INSERT		82
	#define KEY_DELETE		83
#endif

// movement key masks
#define KEY_LEFT_BIT			0x00000001
#define KEY_RIGHT_BIT			0x00000002
#define KEY_FORWARD_BIT			0x00000004
#define KEY_BACKWARD_BIT		0x00000008
#define KEY_UP_BIT				0x00000010
#define KEY_DOWN_BIT			0x00000020
#define KEY_LOOKUP_BIT			0x00000040
#define KEY_LOOKDOWN_BIT		0x00000080
#define KEY_PITCHUP_BIT			0x00000100
#define KEY_PITCHDOWN_BIT		0x00000200
#define KEY_SLIDELEFT_BIT		0x00000400
#define KEY_SLIDERIGHT_BIT		0x00000800
#define KEY_WALKTHRUWALL_BIT	0x00001000
#define KEY_RUN_BIT				0x00002000
#define KEY_AUTOCENTER_BIT		0x00004000

#define KEY_SWOOP_TO_TARGET	0x00010000

/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
// predefined angle resolutions
#define RESOLUTION_1	6		//  1/256 resolution
#define RESOLUTION_2	5		//  2/256
#define RESOLUTION_4	4		//  4/256
#define RESOLUTION_8	3		//  8/256
#define RESOLUTION_16	2		// 16/256
#define RESOLUTION_32	1		// 32/256

/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
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
POINT GetBlockCornerFromXY(LONG x,LONG y);
SHORT* get_blockm (long px1,long py1);

//WRC Fixed Camera functions (mirrors etc...)  these are bad icky things.
//do not use them.
//SHORT NewVideoBuffer(LONG VWidth, LONG VHeight);
//void RedirectVideo(SHORT buffer);
//void RestoreVideo(void);
//void CloseVideoBuffer(SHORT buffer);

/* clip.c */
LONG lines_intersect(LONG x1,LONG y1,LONG x2,LONG y2,LONG x3,LONG y3,LONG x4,LONG y4);

/* convert.c */
LONG seg_to_height(LONG s,LONG side);

/* cue.c */
void SetArtPath ( CSTRPTR pArtPath );
void CreateShadeTable ( UBYTE  *ptrPalette, LONG  percIntensity, LONG  percTranslucent, LONG  percRed, LONG  percGreen, LONG  percBlue );
void init_shade_table(CHAR *Name);
void color_rect (LONG x,LONG y,LONG w,LONG h,LONG s);
void color_box (LONG x,LONG y,LONG w,LONG h,LONG s);
void color_edged_rect (LONG x,LONG y,LONG w,LONG h,LONG s);
void shade_rect (LONG x,LONG y,LONG w,LONG h,LONG s);
void shade_edged_rect (LONG x,LONG y,LONG w,LONG h,LONG s);
void zoom_color_edged_rect (LONG x, LONG y, LONG w, LONG h, LONG s, SHORT hDest);
void zoomout_color_edged_rect (LONG x, LONG y, LONG w, LONG h, LONG s, SHORT hDest);
void zoom_shade_edged_rect (LONG x, LONG y, LONG w, LONG h, LONG s, SHORT hDest);
void zoomout_shade_edged_rect (LONG x, LONG y, LONG w, LONG h, LONG s, SHORT hDest);

/* level.c */
void SetWadPath ( CSTRPTR pWadPath );
void load_level(char *name, LONG PlayerStart);
void load_new_wad(char *name, LONG PlayerStart);

/* point.c */
void set_lens (LONG lens);
void set_view(void);
LONG aprox_dist(LONG x1, LONG y1, LONG x2, LONG y2);
LONG dist(LONG x1, LONG y1, LONG x2, LONG y2);
LONG AngleFromPoint ( LONG X1, LONG Y1, LONG X2, LONG Y2, LONG Resolution );
LONG AngleFromPoint2 ( LONG X1, LONG Y1, LONG X2, LONG Y2, LONG Resolution );
LONG RelativeAngle( LONG Angle1, LONG Angle2 );
void RotateAnglePoint (POINT *p, POINT *origin, ULONG a);
void xlate(POINT *p);
void xlatePoint(POINT *p,POINT a);
//rot8_angle coord-system is on the Std. Cartesian Coord System.
//Nova engine is based on the (0=north,64=east)
#define Rotate(pPoint,angle) rot8_angle((pPoint),-(angle))
void rot8 (POINT *p);
void rot8dbl (POINT *p);
void rot8_angle (POINT *p,ULONG a);
void rot8_r_angle(POINT *p,ULONG OldAngle, ULONG DeltaAngle,LONG radius);

/* reject.c */
BOOL reject(LONG MySector, LONG HisSector);

/* render.c */
void render_view(LONG fskipmap);

/* spans.c */
void init_spans (void);

/* mouse.c */
void SysHideCursor (void);
void SysShowCursor (void);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
extern SHORT		origin_x;
extern SHORT		origin_y;
extern SHORT		clip_x;
extern SHORT		clip_y;

extern LONG			objClicked;
extern WadThingType	typeClicked;
extern LONG			butClicked;

extern LONG			render_width; 		// width and height of rendered data
extern LONG			render_height;
extern LONG			render_perspect;
extern LONG			render_center_x, render_center_y;
extern LONG			window_width;
extern LONG			window_height;


#ifdef _WINDOWS
	extern SHORT 	sDrawMode;
#endif
//extern MAP_INFO		gMapInfo;  //use access functions...



extern CSTRPTR 	sky_filenames[];
extern SHORT	hSky_filenames;

extern BOOL  fPause;
extern BOOL  fRender;
extern BOOL  gfInCombat;

extern ULONG	tot_nodes;

#if defined (__cplusplus)
}
#endif
#endif // _ENGINE_H
