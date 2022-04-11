/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: 
   Author:
   ========================================================================
   Contains the following general functions:
   
   Contains the following internal functions:
   
   ======================================================================== */
#ifndef _MENU_H
#define _MENU_H
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* System menu types */
	// NOTE: this has to be the same as the MENU array in game.c
	// !!!!!! keep it up to date !!!!!!
enum {
	D_HELP,
	D_DACTN,
	D_DRPRT,
	D_MAINMENU,
	D_QUESTION1,
	D_QUESTION2,
	D_QUESTION3,
	D_GAMEBUTTON,
	D_LOAD_MENU,
	D_SAVE_MENU,
	D_OPTION_MENU,
	D_DOSHELP,
	D_CHAT_INPUT,
	D_CHAT_SELECT,

// Insert any new system menus above here. 
// Insert non-system menus in game include file.
	D_LAST_SYSTEM_MENU,
};

#define QUESTION_TEXT	1
#define QUESTION_BTN1	2
#define QUESTION_BTN2	3
#define QUESTION_BTN3	4

#if defined (_GERMANVER)
	#define D_KEY_ESC			KEY_ESC
	#define D_KEY_YES			KEY_J
	#define D_KEY_NO			KEY_N
	#define D_KEY_OK			KEY_O
	#define D_KEY_RETRY		KEY_W
	#define D_KEY_CANCEL		KEY_A
	#define D_KEY_BACK 		KEY_Z
	#define D_KEY_CREDITS	KEY_M
	#define D_KEY_NEVER		KEY_N
	#define D_KEY_UNDO1		KEY_U
	#define D_KEY_ROLL1		KEY_R
	#define D_KEY_DONE1		KEY_F
	#define D_KEY_EXIT		KEY_A
	#define D_KEY_MORE		KEY_M
	#define D_KEY_NEXT		KEY_DOWNRIGHT
	#define D_KEY_PREV		KEY_UPRIGHT
#else
#if defined (_FRENCHVER)
	#define D_KEY_ESC			KEY_ESC
	#define D_KEY_YES			KEY_O
	#define D_KEY_NO			KEY_N
	#define D_KEY_OK			KEY_O
	#define D_KEY_RETRY		KEY_R
	#define D_KEY_CANCEL		KEY_A
	#define D_KEY_BACK 		KEY_R
	#define D_KEY_CREDITS	KEY_I
	#define D_KEY_NEVER		KEY_J
	#define D_KEY_UNDO1		KEY_U
	#define D_KEY_ROLL1		KEY_R
	#define D_KEY_DONE1		KEY_F
	#define D_KEY_EXIT		KEY_Q
	#define D_KEY_MORE		KEY_S
	#define D_KEY_NEXT		KEY_DOWNRIGHT
	#define D_KEY_PREV		KEY_UPRIGHT
#else
	#define D_KEY_ESC			KEY_ESC
	#define D_KEY_YES			KEY_Y
	#define D_KEY_NO			KEY_N
	#define D_KEY_OK			KEY_O
	#define D_KEY_RETRY		KEY_R
	#define D_KEY_CANCEL		KEY_C
	#define D_KEY_BACK 		KEY_B
	#define D_KEY_CREDITS	KEY_R
	#define D_KEY_NEVER		KEY_N
	#define D_KEY_UNDO1		KEY_U
	#define D_KEY_ROLL1		KEY_R
	#define D_KEY_DONE1		KEY_D
	#define D_KEY_EXIT		KEY_E
	#define D_KEY_MORE		KEY_M
	#define D_KEY_NEXT		KEY_DOWNRIGHT
	#define D_KEY_PREV		KEY_UPRIGHT
#endif // french
#endif // english

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

extern BOOL		fPause;
extern SHORT	sMenusUp;
extern BOOL		fMenuClosed;

void ResetMenus(void);

void QuitSys(LONG, LONG);

#if defined (__cplusplus)
}
#endif

#endif 
