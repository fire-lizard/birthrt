/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: anim.h 
   Author: 
   ========================================================================*/

#ifndef _ANIM_H
#define _ANIM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(_SYSTEM_H)
#include "system.h"
#endif





void init_anim();
SHORT NextFrame (SHORT iAnim, SHORT sequence, SHORT rotation, UBYTE * pCtrl, SHORT * pflag, SHORT * psequenceEnd, SHORT * flag2);

USHORT get_center_point(SHORT iAnim);
LONG get_scale(SHORT iAnim);
void PaintAnimation (LONG MenuCombo, LONG );
void GetNextAnimation(LONG, LONG);
void GetPrevAnimation(LONG, LONG);
void GetNextSequence(LONG, LONG);
void GetPrevSequence(LONG, LONG);
void GetNextRotation(LONG, LONG);
void GetPrevRotation(LONG, LONG);
void RunLoop(LONG, LONG);
void StopLoop(LONG, LONG);
void PlayAll(LONG, LONG);
void SpeedUp(LONG, LONG);
void SlowDown(LONG, LONG);
void CloseAnimationMenu(LONG, LONG);


#endif


