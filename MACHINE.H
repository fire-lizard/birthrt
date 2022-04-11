
/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _MACHINE_H
#define _MACHINE_H

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
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

/* machine.c */
void AppIdle(void);
void MenuLoop();
void WindowsMessages(void);
void ClearMessageQueue(void);
int main(int argc,char * argv[]);
void quit_sys(LONG dummy);
void parse_cmdline(int argc,char * argv[]);
LONG key_status(LONG k);
LONG async_key_status(LONG k);
void clear_key_status(LONG unused);
void UpdateFPS(void);
void StartTimerThread(PFV func);
void StopTimerThread(void);
void DefaultMainWindowSize( UINT flags );

/* sysgraph.c */
void init_graph (LONG init_mode);
void set_svga_page (ULONG p);
LONG CalcPage (void);
void set_hires(LONG,LONG);
void set_lowres(LONG,LONG);
void set_pal(char * pal);
void load_pal(char *n);
void init_pal(char *PalName);
void FadeIn(USHORT);
void FadeOut(USHORT);
void close_graph(void);
void update_screen(void);
void clear_screen(void);
void clear_screen_to(int c);				// [d3-14-97 JPC]
void clear_display(void);
void ScreenCopy (
	SHORT	iSrcBitm,
	SHORT	xSrc,
	SHORT	ySrc,
	SHORT	width,
	SHORT	height,
	SHORT	Resolution
);
#define SC_DEFAULT_RES	0					// use default resolution
#define SC_HIGH_RES		1					// force high-res (for drawing panels)


/* statisti.c */
void init_statistics(void);
void close_statistics(void);

/* mouse.c */
#define		CURSOR_HOTSPOT_UL		FALSE
#define		CURSOR_HOTSPOT_CENTER	TRUE
void init_mouse (CSTRPTR name,BOOL hotspot);
void WinCursorSet(void);
void set_mouse_max(void);
void set_mouse_position (LONG x,LONG y);
void read_mouse(void);
void update_buttons(void);
void draw_cursor(void);
LONG cursor_in_box (LONG x1,LONG y1,LONG w,LONG h);

/* utils.c */
#if defined(_WINDOWS)
LONG get_time();
#else
#define get_time()		(*(volatile LONG *)(0x46c) )
#endif
void fatal_error(const char *format, ...);
void pause(LONG t);

/* keyint.c */
LONG keys_down(void);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
/* keyint.c */
//extern volatile char 	key_stat[];
extern volatile BOOL fAnyKeyChanged;

extern LONG			cursor_x,cursor_y,mouse_button;
extern LONG			old_cursor_x,old_cursor_y;
extern LONG			location_x,location_y;
extern LONG			mouse_present;
extern LONG			mouse_need_seg;
extern LONG			mouse_found_seg;
extern LONG			mouse_found_type;
extern LONG			mouse_click;
extern SHORT		iDispBuffer;
extern BOOL			fHighRes;
extern SHORT		cursor_bitmap;		// mouse.c

extern LONG			fps;

#if !defined (_RELEASE)
extern BOOL			gDontFight;
#endif

#if defined (__cplusplus)
}
#endif
#endif // _MACHINE_H
