
/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _DLG_EDIT_H
#define _DLG_EDIT_H

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_SCENE_HXX)
#include "scene.hxx"
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
#ifdef _WINDOWS

typedef struct
{
	char AvatarName[20];
	LONG AvatarId;
} AvatarIdRef;

typedef struct
{
	LONG x;
	LONG y;
	LONG z;
	LONG a;
} AvatarPoint;
#endif

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
void DoMapFunc(long whichMouse, long x, long y);

#ifdef _WINDOWS
void DoEditorDialog(
	LONG index,
	LONG x,
	LONG y
	);
#endif   

void SetEditmode(LONG arg);
void LoadSceneAvatars( char *szSceneName);
void DestroyAvatars(void);
extern BOOL bEditMode;

#endif // _DLG_EDIT_H
