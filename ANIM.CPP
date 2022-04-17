/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename:anim.h 
   Author: 
   ========================================================================*/

#include <io.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SYSTEM.H"
#include "ENGINE.H"
#include "ENGINT.H"
#include "MACHINE.H"

#include "GAMETYPE.HXX"
#include "THINGS.H"
//#include "game.h"
#include "ANIM.H"
#include "MENU.H"
#include "STRMGR.H"
#include "strenum.h"
#include "GMENUENM.H"

//#define NUMROTATIONS 8
#define NUMROT 8
#define MAXWINSPEED 50
#define MAXDOSSPEED	1


static LONG index = FIRST_GAME_THING;
static char fn[128];
static char const * n;
static LONG seq;
static LONG rot;
static UBYTE control;
static SHORT flag;
static SHORT play;
static SHORT sequenceEnd;
static char cAction[] = {'S', 'W', 'D', 'A', 'F', 'X', 'C', 'L', 'G', 'B', 'M', 'Z'};
static char cRotation[] = {'0', '3', '5', '7', '9', '7', '5', '3'};
static char cRotationValue[] = {0, 3, 5, 7, 9, -7, -5, -3};
//char cRot[] = {'0', '3', '5', '7', '9'};
static SHORT iAnim = fERROR;
static SHORT playInit = 0;
static USHORT speedRate;
//SHORT flag2;

void init_anim()
{  	GAME_TTYPE::mfRegisterTTypes();
 	// add by lan for animation menu
 	BOOL not_found = TRUE;
	
	index = FIRST_GAME_THING;
	
	while (not_found && index < LAST_GAME_THING)
	{
		n = GAME_TTYPE::mfGetArtFileName((THINGTYPE)index);
	 	seq = 0;
	 	rot = 0;
	 	sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH, n,n, cAction[seq], cRotation[rot]);
	 	
		if( FileAccess(fn) != 0)
			index++;
		else
			not_found = FALSE;

// removed for HD/CD access by cdb
//	 	if (0 != access(fn, F_OK))
//	 	{
//	 		index++;
//	 	}
//	 	else
//	 	{
//	 		not_found = FALSE;
//	 	}
 	}
 	iAnim = AllocateAnim(TYPE_FLIC, index);
	load_FLC_sequence(iAnim, index, seq, rot);
 	SetAnimSequence(iAnim, (seq * NUMROTATIONS) + 0);
 	control = START_FLAG;
 	flag = SAMEANIM;
    play = 0;

	sequenceEnd=0; // set flag to track each sequence in playall 
   	speedRate = 1;
//  	dframe = mframes = 0;  //things.c nextframe()

}	
/* =======================================================================
	nextframe - Advance to next frame of flic
	======================================================================= */
SHORT NextFrame (SHORT iAnim, SHORT sequence, SHORT rotation, UBYTE * pCtrl, SHORT * pflag, SHORT * psequenceEnd, SHORT * flag2)
{
	ANIMPTR			pAnim;
	FLICHEADPTR		pHead;
	SHORT			iHead = fERROR;
	SHORT			seq_and_rot; 
	SHORT     		r;
	SHORT			old_sequence;
	SHORT			old_rotation;
//	SHORT			dframe;
	SHORT			continue_flag = 0;
	SHORT			rv = fERROR;
 	SHORT      		dframe;
	SHORT      		fdecode = 0;
	BOOL        	mirror=FALSE;
	I_DATA_BLK		*piData = 0;
	
	if (iAnim == fERROR)						/* double check */
		return fERROR;

	SetBlockAttr(iAnim,LOCKED,LOCKED);	/* quick SetLock without memory move */
	
	pAnim = (ANIMPTR)BLKPTR(iAnim);
	r = rotation;
	if(r < 0)
	{
		mirror = TRUE;
		r = (SHORT)(-r);
	}
	seq_and_rot = (SHORT)((sequence * NUMROTATIONS ) + r);


	/* not looping and not changing seq or rot and already did one pass */
	if (*pCtrl == 0 && pAnim->sequence == seq_and_rot && *pflag == SAMEANIM )
		goto exit;

	/* check for seq and rot available in this Anim */
	if (pAnim->hiData > 0)
	{
		piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
		if (piData->iData[seq_and_rot] > 0)
		{
			iHead = piData->iData[seq_and_rot];
		}
	}
	if (iHead < 0)
//!!!!!!!!!!!!!!!!!!!!!!!!!! try to reload here !!!!!!!!!!!!!!!!!!!
		goto error_exit;

	
	if (pAnim->type == TYPE_PCX)			/* check for PCX rotation frames */
	{
		pAnim->iBuff = iHead;				/* make iBuff point to correct PCX frame */
		goto exit;							/* skip animation stuff */
	}
	
	
	pHead = (FLICHEADPTR)BLKPTR(iHead);
	
	if (!IsPointerGood(pHead))
		goto error_exit;

	/* prepare for sequence check and frame decoding */
    old_sequence = (SHORT)(pAnim->sequence / NUMROTATIONS);
	old_rotation = (SHORT)(pAnim->sequence % NUMROTATIONS);
	dframe = pAnim->frame;


	/* check for sequence change */
	if ((pAnim->sequence != seq_and_rot)||mirror) 
	{
		MarkOldAnimPurgable(iAnim, old_sequence);
		if (SetAnimSequence(iAnim, seq_and_rot) == fERROR)
			return fERROR;
		pAnim = (ANIMPTR)BLKPTR(iAnim);
		if (pAnim->hiData > 0)
		{
			piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
			iHead = piData->iData[seq_and_rot];
		}
		
		if (iHead <= 0)
			goto error_exit;
			
		pHead = (FLICHEADPTR)BLKPTR(iHead);
		if (!IsPointerGood(pHead))
			goto error_exit;
			

		//decode_frame(pAnim, pHead); 
		fdecode = (SHORT)1;	   				/* decode 1st frame */
 		//detect_scale(pAnim->iBuff, pAnim->width, pAnim->height);	/* scan for scale factor */
  		dframe = 0;
  		continue_flag = START_FLAG;
  		//SetPurge(iHead);
	}
	else 
		dframe++;

		/* check for animation change */
	if(*pflag == CHANGEANIM)
	{
		
		MarkOldAnimPurgable(iAnim, old_sequence);
		if(SetAnimSequence(iAnim, seq_and_rot)==fERROR)
			return fERROR;
		pAnim = (ANIMPTR)BLKPTR(iAnim);
		
		if (pAnim->hiData > 0)
		{
			piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
			iHead = piData->iData[seq_and_rot];
		}
		
		if (iHead <= 0)
			goto error_exit;
			
		pHead = (FLICHEADPTR)BLKPTR(iHead);
		if (pHead == 0)
			goto error_exit;
			
		//decode_frame(pAnim, pHead);
		fdecode = 1;
		//detect_scale(pAnim->iBuff, pAnim->width, pAnim->height);
	    dframe = 0;
	 	continue_flag = START_FLAG;
  		//SetPurge(iHead);
	}


	/* check for end of sequence and looping */
	if ((dframe >= pHead->frames) && (pAnim->frame != 0)) /* after last frame */
	{
		if (!(*pCtrl & REWIND_FLAG))		/* don't rewind to 1st frame if flag not set */
		{	*psequenceEnd = 1;
			
			goto exit;
			
		}
		pAnim->offData = pHead->oframe1;	/* reset to first frame */
		dframe = pAnim->frame = 0;
		decode_frame(pAnim, pHead);		/* decode the 1st frame */
		fdecode = 1;
		detect_scale(pAnim->iBuff, pAnim->width, pAnim->height);	/* scan for scale factor */
		/* added check for loop once being reset */
		if (*pCtrl & (LOOP_FLAG|START_FLAG))
			dframe = 1;				/* loop to 2nd frame */
	}

	/* advance to desired frame */

	
	if(fdecode == 0 && (pAnim->frame < dframe))
	{

	if  (*flag2 == 1 )
	{
		pAnim->frame++;
		decode_frame(pAnim, pHead);

 	 	*flag2 = 0;

 	}
	else 
	{
		goto exit;
	}
	
  
  }	
	
/* set control frame number */
	*pCtrl = (UBYTE)(((*pCtrl) & (LOOP_FLAG|REWIND_FLAG)) + continue_flag + pAnim->frame);
exit:
	rv = pAnim->iBuff;		// Current frame now in iBuff
error_exit:
	ClrLock(iAnim);
	return rv;
}


/* =======================================================================
	get_center_point   
	======================================================================= */
USHORT get_center_point(SHORT iAnim)
{
	if (iAnim != fERROR)
	{
		ANIMPTR const pAnim = (ANIMPTR const)BLKPTR(iAnim);
		if (pAnim->iBuff != fERROR)
		{
			BITMPTR const pBuff = (BITMPTR const)BLKPTR(pAnim->iBuff);
			return pBuff->x_ctr_pt;
		}
	}
	
	return 0;
}

/* ======================================================================
	get_scale
	====================================================================== */
LONG get_scale(SHORT iAnim)
{
	if (iAnim != fERROR)
	{
		ANIMPTR const pAnim = (ANIMPTR const)BLKPTR(iAnim);
		if (pAnim->iBuff != fERROR)
		{
			BITMPTR const pBuff = (BITMPTR const)BLKPTR(pAnim->iBuff);
			return pBuff->scale;
		}
	}
	return 0;
}

void get_widhei(SHORT iAnim, LONG * wid, LONG * hei)
{
	LONG scale;
	LONG w, h;
	ANIMPTR pAnim;
    SHORT iHead;
	FLICHEADPTR pHead;
	I_DATA_BLK	*piData = 0;
	
	*wid = 0;
	*hei = 0;
	if (iAnim < 0)
		return;
		
	pAnim = (ANIMPTR) BLKPTR(iAnim);
	
	if (pAnim->hiData < 0)
		return;
	
	piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
	iHead = piData->iData[seq*NUMROTATIONS + rot];		// get the memory handle for this resource
	
	if (iHead > 0)
	{
		pHead = (FLICHEADPTR)BLKPTR(iHead);
		if (IsPointerGood(pHead))
		{
			h = pHead->width;
			w = pHead->height;
			scale = get_scale(iAnim);
			*hei = h * UNITARY_SCALE / scale;
			*wid = w * UNITARY_SCALE / scale;
			
			SetPurge(iHead);
		}
	}
}
/* =============================================================================================
   Function    - PaintAnimation
   Description - paint the text on the Animation menu
   Returns     -
   ======================================================================== */
void PaintAnimation (LONG MenuCombo, LONG )
{
	SHORT	i;
	LONG	X,Y;
	//LONG	Z;
	LONG	mx,my;
	LONG	MenuId, ButtonId;
	CHAR	color = 200;
	SHORT   iBitm;
  	SHORT   xCenter;
	char    xCntr[40];
	SHORT    scale;
	char    S[40];
	LONG    time;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	static LONG lastTime = 0;
	LONG   timedif;
//	SHORT   iHead;
	LONG    tmp;
	static SHORT flag2 = 0;
	static LONG lastRot=rot; 	
	LONG   wid, hei;

	// get position of text screen
	if( GetButtonPosition( MenuId, 15, &mx, &my ) == fERROR)
		return;

	X = mx;
	Y = my;

	// get size of text screen
	if( GetButtonSize( MenuId, 15, &mx, &my ) == fERROR)
		return;

	// get center of text screen
	X += mx/2;
	Y += 10;


	init_gfont(FONT_TITL_10PT);

  	print_text_centered(X,Y,STRMGR_GetStr(STR_ANIM_1), 60);
	Y += 20;
	print_text_centered(X,Y,STRMGR_GetStr(STR_ANIM_2), 60);
	Y += 20;
	print_text_centered(X,Y,STRMGR_GetStr(STR_ANIM_3), 60);


	// paint the control buttons
	init_gfont(FONT_TITL_10PT);
	for(i=2;i<=13;i++)
	{
		CHAR buffer[128];

		// get position of the control button
		if( GetButtonPosition( MenuId, i, &mx, &my ) == fERROR)
			return;

		X = mx;
		Y = my;

		// get size of the control Button
		if( GetButtonSize( MenuId, i, &mx, &my ) == fERROR)
			return;

		X += mx/2;
		Y += my/2;

		switch(i){
			case 2:
			case 4:
			case 6:
				strcpy(buffer,STRMGR_GetStr(STR_ANIM_PREV));
				print_text_centered(X,Y,buffer,color);
				break;
			case 3:
			case 5:
			case 7:
				strcpy(buffer,STRMGR_GetStr(STR_ANIM_NEXT));
				print_text_centered(X,Y,buffer,color);
				break;
  			case 8:
		   		strcpy(buffer,STRMGR_GetStr(STR_ANIM_RUN));
				print_text_centered(X,Y,buffer,color);
	  			break;
			case 9:
				strcpy(buffer,STRMGR_GetStr(STR_ANIM_STOP));
				print_text_centered(X,Y,buffer,color);
		   		break;
			case 10:
				strcpy(buffer,STRMGR_GetStr(STR_ANIM_QUIT));
				print_text_centered(X,Y,buffer,color);
				break;
			case 11:
				strcpy(buffer,STRMGR_GetStr(STR_ANIM_PLAY));
				print_text_centered(X,Y,buffer,color);
				break;
			case 12:
				strcpy(buffer,STRMGR_GetStr(STR_ANIM_UP));
				print_text_centered(X,Y,buffer,color);
				break;
			case 13:
				strcpy(buffer,STRMGR_GetStr(STR_ANIM_DOWN));
				print_text_centered(X,Y,buffer,color);

		}

	}

	//get position of animation screen
	if(GetButtonPosition( MenuId, 1, &mx, &my )==fERROR)
		return;

	X = mx;
	Y = my;

	//get size of Animation screen
	if( GetButtonSize ( MenuId, 1, &mx, &my )== fERROR)
		return;

	time = get_time();
	
	timedif = time-lastTime;
   
	if(timedif > (speedRate * MAXDOSSPEED))
	{
	   	lastTime = time;
		flag2 = 1;

		if(play && sequenceEnd && playInit == 0)
		{ 		
			GetNextRotation(0, 0);

			if ( ((rot ==1)&&(seq==5))
			   ||((rot ==1)&&(seq==12))
			   ||((rot ==7)&&(seq==11)))
			{	
				rot = 0;
				GetNextSequence(0, 0);
			}
			else if(rot == 0)
			 	GetNextSequence(0, 0);

		  	sequenceEnd = 0;
	   	
		
		}
	 }
	tmp = rot;
	
	switch (rot){
		case 5:
			rot = 3;
			break;
		case 6:
			rot = 2;
			break;
		case 7:
			rot = 1;
			break;
		default:
			;
	}

  
	OpenAnim(iAnim, (SHORT)((seq*5)+rot), fn, TYPE_FLIC);
	
  	if(((tmp == 5)&&(lastRot == 3))
	 ||((tmp == 1)&&(lastRot == 7))
	 ||((tmp == 7)&&(lastRot == 1))
	 ||((tmp == 3)&&(lastRot == 5))) 	
	{
		iBitm = NextFrame(iAnim, (SHORT)seq, (SHORT)-rot, &control, &flag, &sequenceEnd, &flag2); 
	}
	else
	{
		iBitm = NextFrame(iAnim, (SHORT)seq, (SHORT)rot, &control, &flag, &sequenceEnd, &flag2); 
	}

	if (iBitm > 0)
	{
		if(rot != tmp)
		{		
			rot = tmp;
			ScaleBitmap((SHORT)(X+mx/2), (SHORT)Y, iBitm, 0, 0, (SHORT)-mx, (SHORT)-my, FULL_SCALE/2);
	 	}
		else
		{
				
			ScaleBitmap((SHORT)(X+mx/2), (SHORT)Y, iBitm, 0, 0, (SHORT)mx, (SHORT)-my, FULL_SCALE/2);
		}
	}
	
//	SetUpdate(MenuId, 0);
	playInit = 0;
	lastRot = rot;
	
	flag = SAMEANIM;
 
	xCenter = get_center_point(iAnim);
	scale = (SHORT) get_scale(iAnim);
	get_widhei(iAnim, &wid, &hei);

	init_gfont(FONT_TITL_10PT);

	X = X + mx/2 ;
	
  	line(X, Y, X, Y+5, color);
	line(X-5, Y, X, Y+5, color);
	line(X+5, Y, X, Y+5, color);

  	//print out title and center point
   
	if(GetButtonPosition(MenuId, 14, &mx, &my) == fERROR)
		return;

	X = mx;
	Y = my;

	if(GetButtonSize(MenuId, 14, &mx, &my) == fERROR)
		return;

	X += mx/2-10;
	Y += 10;
	
	print_text_centered(X, Y, fn, 60);
	Y += 20;
	
	

	sprintf(xCntr, STRMGR_GetStr(STR_ANIM_STR1),xCenter, speedRate, index);
   	print_text_centered(X, Y, xCntr, 60);  
	Y += 20;
	sprintf(S, STRMGR_GetStr(STR_ANIM_STR2), scale, wid, hei);
	print_text_centered(X, Y, S, 60);

	
}

/*=========================================================================
	Function    -GeNextAnimation
	Description -Get the next animation index
	returns     -index of the next animation
  =======================================================================*/
void GetNextAnimation(LONG, LONG)
{
	  char tmp[128]; 
	  index++;
	  
	  if(index > LAST_GAME_THING)
			index = FIRST_GAME_THING;
	  

	  n = GAME_TTYPE::mfGetArtFileName((THINGTYPE)index);
   	  sprintf(tmp, "%s------\\------%c%c.flc", THING_PATH, cAction[seq], cRotation[rot]);
	  sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH, n,n, cAction[seq], cRotation[rot]);

   
   	  while((0==(strcmp(fn, tmp)))||(0!=FileAccess(fn)) )	  
	  {
		index++;
		if(index > LAST_GAME_THING)
			index = FIRST_GAME_THING;
		n = GAME_TTYPE::mfGetArtFileName((THINGTYPE)index);
   		sprintf(tmp, "%s------\\------%c%c.FLC", THING_PATH, cAction[seq], cRotation[rot]);
		sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH, n,n, cAction[seq], cRotation[rot]);
	  }
	  flag = CHANGEANIM;
	  
	  sprintf(tmp, "%s.IDA", GAME_TTYPE::mfGetArtFileName((THINGTYPE)index));
	  ANIMPTR pAnim = (ANIMPTR) BLKPTR(iAnim);
	  if (pAnim->hiData > 0)
	  {
	  	  SetPurge(pAnim->hiData);
	  }
	  pAnim->hiData = GetResourceStd(tmp, FALSE);
	  
	  load_FLC_sequence(iAnim, index, seq, rot);
	  SetAnimSequence(iAnim, (seq * NUMROTATIONS) + rot);
}
	
void GetPrevAnimation(LONG, LONG)
{
   	char tmp[128];
	sprintf(tmp, "%s------\\------%c%c.FLC", THING_PATH,cAction[seq], cRotation[rot] );
	index--;
	
	if(index < FIRST_GAME_THING)
		index = LAST_GAME_AVATAR;
	
    n = GAME_TTYPE::mfGetArtFileName((THINGTYPE)index);
 	sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH, n,n, cAction[seq], cRotation[rot]);

	
	while((0==(strcmp(fn, tmp)))||(0!=FileAccess(fn) ))	
	{
		index--;
		if(index < FIRST_GAME_THING)
			index = LAST_GAME_AVATAR; 
		n = GAME_TTYPE::mfGetArtFileName((THINGTYPE)index);
   		sprintf(tmp, "%s------\\------%c%c.FLC", THING_PATH, cAction[seq], cRotation[rot]);
		sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH,n, n, cAction[seq],cRotation[rot]);
	}
	flag = CHANGEANIM;
	
	sprintf(tmp, "%s.IDA", GAME_TTYPE::mfGetArtFileName((THINGTYPE)index));
	ANIMPTR pAnim = (ANIMPTR) BLKPTR(iAnim);
	if (pAnim->hiData > 0)
	{
		  SetPurge(pAnim->hiData);
	}
	pAnim->hiData = GetResourceStd(tmp, FALSE);
	
	load_FLC_sequence(iAnim, index, seq, rot);
	SetAnimSequence(iAnim, (seq * NUMROTATIONS) + rot);
}

/* ========================================================================
	Function    -GetNextSequence
	Description -Get the file name of next sequence 
	Returns     -
   ======================================================================== */
void GetNextSequence(LONG, LONG)
{
  
	SHORT r;
	BOOL f=FALSE;
	seq++;
	if(seq >= NUMACTIONS)
		seq = 0;
	sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH, n,n, cAction[seq],cRotation[rot]);  	


	while(0 != FileAccess(fn) )
	{
		for(r=0; r<NUMROT; r++)
		{
			if(cRotationValue[r] < 0)
				sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH, n,n, cAction[seq], cRotation[r]);
			else
				sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH, n,n, cAction[seq], cRotation[r]);
				
			if(0 == FileAccess(fn))
			{
				f=TRUE;
				break;
			}
		}
		if(f)
		{
			rot = r;
			break;
		}
		seq++;
		if(seq >= NUMACTIONS)
			seq = 0;
		sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH, n,n, cAction[seq],cRotation[rot]);  	
	}


}

void GetPrevSequence(LONG, LONG)
{
	SHORT r;
	BOOL f=FALSE;
	seq--;
	if(seq < 0)
		seq = NUMACTIONS-1;
	sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH, n,n, cAction[seq], cRotation[rot]);
	while(0 != FileAccess(fn) )
	{
		for(r=0; r<NUMROT; r++)
		{
			if(cRotationValue[r] < 0)
				sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH, n,n, cAction[seq], cRotation[r]);
			else
				sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH, n,n, cAction[seq], cRotation[r]);
				
			if(0 == FileAccess(fn))
			{
				f=TRUE;
				break;
			}
		}
		if(f)
		{
			rot = r;
			break;
		}

		seq--;
		if(seq < 0)
			seq = NUMACTIONS - 1;
		sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH, n,n, cAction[seq], cRotation[rot]);
	}
	
}


/* ========================================================================
   Function    -GetNextRotation
   Description -
   Returns	   -
   ======================================================================== */
void GetNextRotation (LONG, LONG)
{
	rot++;
	if(rot >= NUMROT )
		rot = 0;
	
	if(cRotationValue[rot] < 0)
		sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH, n,n, cAction[seq],cRotation[rot]);
	else
		sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH, n,n, cAction[seq], cRotation[rot]);

	while(0 != FileAccess(fn))
	{
		rot++;
		if(rot >= NUMROT )
			rot = 0;
	   	if(cRotationValue[rot] < 0)
			sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH, n,n, cAction[seq], cRotation[rot]);
		else
			sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH, n,n, cAction[seq], cRotation[rot]);
	}
}
   
void GetPrevRotation(LONG, LONG)
{
	rot--;
	if(rot < 0)
		rot = NUMROT - 1;
	if(cRotationValue[rot] < 0)
	sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH, n,n, cAction[seq],cRotation[rot]);
	else	
	sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH, n,n, cAction[seq], cRotation[rot]);

	while(0 != FileAccess(fn))
	{
		rot--;
		if(rot < 0)
			rot = NUMROT - 1;
		if(cRotationValue[rot] < 0)
   		sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH, n,n, cAction[seq],cRotation[rot]);
		else	
		sprintf(fn, "%s%s\\%s%c%c.FLC", THING_PATH, n,n, cAction[seq], cRotation[rot]);
	}
}

/* ========================================================================
   Function    -RunLoop
   Description -loop sequence
   Returns     -   
   =======================================================================*/

void RunLoop(LONG, LONG)
{
	control = REWIND_FLAG|LOOP_FLAG|START_FLAG;
}

void StopLoop(LONG, LONG)
{
	control = START_FLAG;
	play = 0;
}
 	
void PlayAll(LONG, LONG)
{  	
	playInit = 1;
  	play = 1;
  	control = START_FLAG;
	rot = NUMROT - 1;
	seq = 0;
 	GetNextRotation(0, 0);
	sequenceEnd = 0; 

} 		

void SpeedUp(LONG, LONG)
{
	if(speedRate/2 >= 1)
		speedRate = (USHORT)(speedRate / 2);
}
void SlowDown(LONG, LONG)
{
	speedRate = (USHORT)(speedRate * 2);
}

void CloseAnimationMenu(LONG , LONG )
{
	if (iAnim > 0)
	{
		RemoveAnim(iAnim);
		iAnim = fERROR;
	}
	SetPurgeClass(CLASS2);	// Purge all the animations and hiData's
	HideMenu(D_SHOW_ANIMATION);
}
