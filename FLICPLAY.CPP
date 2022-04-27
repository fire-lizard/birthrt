/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: flicplay.cpp
   Author:   Lan Zhou
   ========================================================================
   Contains the following general functions:

   void PlayFlic(char* fn, LONG sec)	-	play a flic

   Contains the following internal functions:

   AddFlicSound(LONG whichFrame, LONG whichSound);
   ClearFlicSound(void);
   LoadFlic (CSTRPTR szFileName, BOOL* ffirst, BOOL* fend);
   decode_one_frame (SHORT iHead, ANIMPTR pAnim);
   OpenFlcAnim (SHORT iAnim, CSTRPTR szFileName, SHORT Type, BOOL *ffirst, BOOL *fend);


   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <dos.h>

#ifdef _WINDOWS
#include <Windows.h>
#endif

#include "SYSTEM.H"
#include "ENGINE.H"
#include "ENGINT.H"
#include "MACHINE.H"
#include "HANDLE.HXX"
#include "SOUND.HXX"
#include "FLICPLAY.HXX"
//#if defined (AUDIO_LIB)
//#include "KERNAUD.HPP"
//#endif
#include "MAIN.HXX"

#ifdef _WINDOWS
	extern BOOL fQuitting;					// [d1-07-97 JPC] defined in WINSYS\MACHINE.C
#endif


#define fFILE_NOT_FOUND   -2

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define MAX_INTRO_BITMS	99
/* ------------------------------------------------------------------------
	Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
SHORT LoadFlic (CSTRPTR szFileName, BOOL* ffirst, BOOL* fend);
ERRCODE decode_one_frame (SHORT iHead, ANIMPTR pAnim);
SHORT OpenFlcAnim (SHORT iAnim, CSTRPTR szFileName, SHORT Type, BOOL *ffirst, BOOL *fend);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
LONG FlcDelay = 0;

typedef struct
{
	LONG lWhichFrame;
	LONG lWhichSound;
} FRAMESOUND;

static FRAMESOUND frameSound[100] = {0, 0};
static int frameSoundIndex = 0;

static FLICHEADPTR	pBlk;
static ULONG           offset;
static SHORT           framesCount=0;
static SHORT           ihead1 = fERROR;
static int				Flicfp	= fERROR;

/* ========================================================================
   Function    - AddFlicSound
   Description - adds a sound to a flic
   Returns     - void
   ======================================================================== */
void AddFlicSound(LONG whichFrame, LONG whichSound)
{
	frameSound[frameSoundIndex].lWhichFrame = whichFrame;
	frameSound[frameSoundIndex].lWhichSound = whichSound;
	frameSoundIndex++;
}

/* ========================================================================
   Function    - ClearFlicSound
   Description - clears the flic sound table
   Returns     - void
   ======================================================================== */
void ClearFlicSound(void)
{
	int i;

	for(i=0; i<100; i++)
	{
		frameSound[frameSoundIndex].lWhichFrame = -1;
		frameSound[frameSoundIndex].lWhichSound = -1;
	}

	frameSoundIndex = 0;
}

/* ========================================================================
   Function    - PlayFlic
   Description - show the intro sequence
   Returns     - TRUE = played complete, FALSE interuppted
   ======================================================================== */
BOOL PlayFlic(char * fn, LONG secondsToWaitAtEnd)
{
	SHORT iAnim;
	// char  fn[40];
	BOOL endflag=FALSE;
	BOOL ffirst=TRUE;
	BOOL fend=FALSE;
	ANIMPTR pAnim;
	iAnim = fERROR;
	LONG timedif;
	LONG lCurFrame = 0;
	char n[256];
	int tag = 33333;
	frameSoundIndex = 0;
	BOOL	fComplete = TRUE;	// assume all works compeletly

	fAnyKeyChanged = FALSE;
	mouse_button = 0;
	Flicfp = fERROR;
		
	while(fend == FALSE)
	{
		timedif = get_time() + FlcDelay;
		
		iAnim= OpenFlcAnim(iAnim, fn, TYPE_FLIC, &ffirst, &fend);
		
		if (iAnim == fERROR)
			goto ReturnTime;
			
		pAnim = (ANIMPTR)BLKPTR(iAnim);

		if(fend)
			break;
			
		
		run_timers();
		if (pAnim->hiData > 0)
		{
			decode_one_frame(pAnim->hiData, pAnim);
		}
		
		run_timers();
		DrawBitmap(80, 60, pAnim->iBuff, 0, 0, pAnim->width, pAnim->height);

		run_timers();
		update_screen();
		DirectDrawPreFrame();
		
		DisposBlock(pAnim->hiData);
		pAnim->hiData = fERROR;
		
		// if this frame has a sound attached, playit
		while(frameSound[frameSoundIndex].lWhichFrame == lCurFrame)
		{
			if(frameSound[frameSoundIndex].lWhichSound < 0)
			{
				// Start a pause
				// Note: GEH big hack attack
				timedif = get_time() + ABS(frameSound[frameSoundIndex].lWhichSound);
				// wait out any delay time
				while(get_time() < timedif)
				{
					// we have to do this to keep the sound going in DOS
					run_timers();
					
			#ifdef _WINDOWS
					ClearMessageQueue();
					// [d1-07-97 JPC] You can quit while intro is playing--check
					// for that here.
					if (fQuitting)
						goto CleanUp;
			#endif
					
					if(mouse_button)
					{
						mouse_button = 0;
						fComplete = FALSE;
						goto CleanUp;
					}
					else
					if(fAnyKeyChanged)
					{
						fAnyKeyChanged = FALSE;
						clear_key_status(0);
						fComplete = FALSE;
						goto CleanUp;
					}
				}
			}
			else
			{
				// play the sound
				AddSndObj((BIRTHRT_SND)frameSound[frameSoundIndex].lWhichSound,0,VOLUME_PRIORITY);
				// GEH you can't do the following.  You loose all Game Volume
				// control.
//				kernel->AudioPlay(	(int) frameSound[frameSoundIndex].lWhichSound,
//								(BOOL) 0, 		// looped
//								(int) 110,    // volume
//								(int) 127,    // volume
//								(int) tag++,  // INSTANCE TAG
//								(long) FALSE); 		// paused
		}
			frameSoundIndex++;
		}

		lCurFrame++;

		#ifdef _WINDOWS
		ClearMessageQueue();
		// [d1-07-97 JPC] You can quit while intro is playing--check
		// for that here.
		if (fQuitting)
			goto CleanUp;
		#endif
					
		update_buttons();
		if(mouse_button)
		{
			mouse_button = 0;
			fComplete = FALSE;
			goto CleanUp;
		}
		else
		if(fAnyKeyChanged)
		{
			fAnyKeyChanged = FALSE;
			clear_key_status(0);
			fComplete = FALSE;
			goto CleanUp;
		}
		
		// wait out any delay time
		while(get_time() < timedif)
		{
			run_timers();
		}
	}

	// [d11-11-96 JPC] Delay for "secondsToWaitAtEnd."  Needed for one
	// of the gorgon flics.  Multiply by 18.2 (91/5) to convert timer
	// ticks to seconds.
	timedif = get_time() + (secondsToWaitAtEnd * 91) / 5;
	while(get_time() < timedif)
	{
		run_timers();
		if(mouse_button)
		{
			mouse_button = 0;
			fComplete = FALSE;
			goto CleanUp;
		}
		else
		if(fAnyKeyChanged)
		{
			fAnyKeyChanged = FALSE;
			clear_key_status(0);
			fComplete = FALSE;
			goto CleanUp;
		}
	}

CleanUp:
	if(iAnim != fERROR)
	{
		if (pAnim->hiData != fERROR)
		{
			DisposBlock(pAnim->hiData);
			pAnim->hiData = fERROR;
		}
		if(pAnim->iBuff != fERROR)
		{
			DisposBlock(pAnim->iBuff);	
			pAnim->iBuff = fERROR;
		}
		DisposBlock(iAnim);
		iAnim = fERROR;
	}
	if(ihead1 != fERROR)
	{
		DisposBlock(ihead1);
		ihead1 = fERROR;
	}
	
	if (Flicfp != fERROR)
	{
		DiskClose(Flicfp);
		Flicfp = fERROR;
	}

ReturnTime:		
	return fComplete;
}

/* =======================================================================
	Load FLC format files
	======================================================================= */
SHORT LoadFlic (CSTRPTR szFileName, BOOL* ffirst, BOOL* fend)
{
	SHORT			iBlk;
	SHORT           ihead2;
	FRAMEHEADPTR	pFramehd;
	SHORT           length;

	if(*ffirst == TRUE)
	{
		Flicfp = DiskOpen(szFileName);	/* try to open the file */
		if (Flicfp == fERROR)								/* Resource not found. Print an error */
			return (SHORT)fFILE_NOT_FOUND;			/* mark file as not found */
		length = sizeof(FLICHEAD);
	
		ihead1 = NewBlock(length);
		if (ihead1 == fERROR)
		{
			return fERROR;
		}
		pBlk = (FLICHEADPTR)BLKPTR(ihead1);

		if (read(Flicfp, pBlk, length) == fERROR)			/* read the first four bytes */
       	{	
			DisposBlock(ihead1);
			ihead1 = fERROR;
			DiskClose(Flicfp);
#if defined (_DEBUG)
			fatal_error("FLICPLAY ERROR - data read failed %s\n",szFileName);
#endif
			return (SHORT)fERROR;
		}
		offset = pBlk->oframe1;
		lseek(Flicfp, -length, SEEK_CUR);
		lseek(Flicfp, offset, SEEK_CUR);
		framesCount = 0;
		*ffirst = FALSE;

	}

	length = sizeof(FRAMEHEAD);
	ihead2 = NewBlock(length);
	if (ihead2 == fERROR)
	{
		DisposBlock(ihead1);
		ihead1 = fERROR;
		return fERROR;
	}

	pFramehd  = (FRAMEHEADPTR)BLKPTR(ihead2);
	framesCount++;
	if (read(Flicfp, pFramehd, length) == fERROR)			/* read the first four bytes */
    {	
		DisposBlock(ihead1);
		ihead1 = fERROR;
		DisposBlock(ihead2);
		ihead2 = fERROR;
		DiskClose(Flicfp);
#if defined (_DEBUG)
		fatal_error("FLICPLAY ERROR - data read failed %s\n",szFileName);
#endif
		return (SHORT)fERROR;
	}
	lseek(Flicfp, -length, SEEK_CUR);
	if (pFramehd->size > 1000000)
	{
		DisposBlock(ihead1);
		ihead1 = fERROR;
		DisposBlock(ihead2);
		ihead2 = fERROR;
		DiskClose(Flicfp);
#if defined (_DEBUG)
		fatal_error("ERROR - pFramehd->size invalid (%d) in decode_frame\n",pFramehd->size);
#endif
	}
	offset += pFramehd->size;

	/* Allocate space for data */
	iBlk = NewBlock(pFramehd->size);

	if (iBlk != fERROR)
	{
		if (read(Flicfp, BLKPTR(iBlk), pFramehd->size) == fERROR)
		{
			DiskClose(Flicfp);
			DisposBlock(ihead1);
			ihead1 = fERROR;
			DisposBlock(ihead2);
			ihead2 = fERROR;
			DisposBlock(iBlk);
			iBlk = fERROR;
#if defined (_DEBUG)
			fatal_error("FLICPLAY ERROR - data read failed %s\n",szFileName);
#endif
			return (SHORT)fERROR;
		}
		else
		{
			if(framesCount == pBlk->frames-1)
			{
				DiskClose(Flicfp);
				DisposBlock(ihead1);
				ihead1 = fERROR;
				*fend=TRUE;
			}
			DisposBlock(ihead2);
			return iBlk;
		}
	}
	else
	{
		DiskClose(Flicfp);
#if defined (_DEBUG)
		fatal_error("FLICPLAY ERROR - memory is unavailable %s\n",szFileName);
#endif
		return (SHORT)fERROR;
	}
}

/* =======================================================================
	decode_frame - Decode a frame that is in memory already into screen.
	Here we just loop through each chunk calling appropriate
	chunk decoder.
	======================================================================= */
ERRCODE decode_one_frame (SHORT iHead, ANIMPTR pAnim)
{
	ULONG				i;
	CHUNKHEADPTR	pChunk;
	PTR				pData;
	FRAMEHEADPTR pFramehd;

	pFramehd = (FRAMEHEADPTR)BLKPTR(iHead);

	if (pFramehd->size - (LONG)sizeof(FRAMEHEAD) > 0L)
	{
		pData = (PTR)(pFramehd+1);

		for (i=0; i<pFramehd->chunks; ++i)
		{
			pChunk = (CHUNKHEADPTR)pData;
			pData += pChunk->size;
			switch (pChunk->type)
			{
//				case COLOR_256:
//					decode_color((PTR)(pChunk+1), pAnim, COLOR_256);
//					break;
  				case DELTA_FLC:
  					decode_delta_flc((PTR)(pChunk+1), pAnim);
  					break;
//				case COLOR_64:
//					decode_color((PTR)(pChunk+1), pAnim, COLOR_64);
//					break;
//				case DELTA_FLI:
//					decode_delta_fli((PTR)(pChunk+1), pAnim);
//					break;
//				case BLACK_FRM:
//					FillRect(pAnim->iBuff, 0, 0, pAnim->width, pAnim->height, coTRANSP);
//					break;
  				case BYTE_RUN:
  					decode_byte_run((PTR)(pChunk+1), pAnim);
  					break;
  				case LITERAL:
  					decode_literal((PTR)(pChunk+1), pAnim);
  					break;
  				default:
//				printf("OTHER CHUNK: %d\n",pChunk->type);
  					break;
			}
		}

	}
	return fNOERR;
}

/* =======================================================================
	OpenAnim - Get a .FLC type resource and create an animation header
	======================================================================= */
SHORT OpenFlcAnim (SHORT iAnim, CSTRPTR szFileName, SHORT Type, BOOL *ffirst, BOOL *fend)
{
	ANIMPTR			pAnim;
	SHORT				iHead;
	SHORT			i;
	BITMPTR         pBuff;
	SHORT                   scale = UNITARY_SCALE;
	if (iAnim <= fNOERR)		/* includes fERROR */
	{
		/* allocate memory for animation header and lock in place */
		if ( (iAnim = NewLockedBlock(sizeof(ANIMHDR))) == fERROR)
			return fERROR;
		pAnim = (ANIMPTR)BLKPTR(iAnim);
		pAnim->type			= Type;
		pAnim->sequence	= 9998;			/* out of range condition */
		pAnim->frame		= 99;			/* out of range condition */
		pAnim->hiData = fERROR;
		pAnim->iBuff		= fERROR;
	}

	iHead = LoadFlic(szFileName, ffirst, fend);
	
	if (iHead == fFILE_NOT_FOUND ||
	    iHead == fERROR)
	{
		*fend = TRUE;
		return iAnim;
	}
	
	if (iHead > 0)
	{
		SetClass2(iHead);				// allow all things to be purged en masse
	}
	
	/* get the anim data into memory */
	pAnim = (ANIMPTR)BLKPTR(iAnim);
	
	if(*fend)
	{
		pAnim->hiData = iHead;
		return iAnim;
	}
		
	pAnim->hiData = iHead;
	pBlk = (FLICHEADPTR)BLKPTR(ihead1);
	pAnim->width	= pBlk->width;
	pAnim->height	= pBlk->height;
	/* if iBuff exists and is not the same size, dispos before making a new one */
	if (pAnim->iBuff != fERROR)
	{
		pBuff = (BITMPTR) BLKPTR(pAnim->iBuff);
		if (pBuff->w == pAnim->width && pBuff->h == pAnim->height)
			return iAnim;		/* same size so no need to make a new one */

		scale = pBuff->scale;	/* save current scale */
		DisposBlock(pAnim->iBuff);
		pAnim->iBuff = fERROR;
	}

	i = OpenBitm(pAnim->width, pAnim->height);
	pAnim = (ANIMPTR)BLKPTR(iAnim);			// recalc pAnim, OpenBitm might screw it up
	pAnim->iBuff = i;
	if (i == fERROR)
	{
#if defined(_DEBUG)
		fatal_error("ERROR - could not allocate buffer in SetAnimSequence\n");
#endif
		return fERROR;
	}
	pBuff = (BITMPTR)BLKPTR(i);
	if (!IsPointerGood(pBuff))
	{
#if defined(_DEBUG)
		fatal_error("ERROR - could not allocate buffer in SetAnimSequence\n");
#endif
		*fend = TRUE;
		return iAnim;
	}

	pBuff->scale = scale;


	return iAnim;
}


//-----------------------------------------------------
// SimpleFLC member functions
//-----------------------------------------------------

SHORT SimpleFLC::mfInit(char *fn, SHORT x, SHORT y)
{
	mfDispose();
	
	if (fn == NULL || strlen(fn) > MAX_FLC_FILENAME_LEN)
		return FLCERR_FILE;
	
	strncpy(filename, fn, MAX_FLC_FILENAME_LEN);	
//	if (FileAccess(fn))
//		return FLCERR_FILE;
	
	if ((iAnim = NewBlock(sizeof(ANIMHDR))) == fERROR)
	{
		return FLCERR_MEM;
	}
	
	run_timers();
    if ((iHead = GetResourceStd(filename, FALSE)) == fERROR)
    {
    	mfDispose();
    	return FLCERR_RSRC;	
    }

	run_timers();
	SHORT width, height;
    // don't lock anything down until we know everything's in
    {
		FLICHEADPTR tmp_pHead = (FLICHEADPTR)BLKPTR(iHead);
		width = tmp_pHead->width;
		height = tmp_pHead->height;
	}
	
	run_timers();
	if ((iBuf = OpenBitm(width, height)) == fERROR)
	{
    	mfDispose();
    	return FLCERR_BITM;
    }

	run_timers();
	DumbAutoLockPtr<ANIMHDR> pAnim(iAnim);
	DumbAutoLockPtr<FLICHEAD> pHead(iHead);
	pAnim->offData = pHead->oframe1;
	pAnim->width = pHead->width;
	pAnim->height = pHead->height;
	pAnim->iBuff = iBuf;
	pAnim->sequence = 0;
	pAnim->rotation = 0;
	pAnim->type = TYPE_FLIC;
	pAnim->totalFrames = pHead->frames;
	pAnim->frame = 0;
	frame_in_buffer = -1;
	
	coord_x = x;
	coord_y = y;
	
	return FLCERR_OK;	
}

void SimpleFLC::mfDispose()
{
	run_timers();
	if (iHead != fERROR)
	{
		SetPurge(iHead);
		iHead = fERROR;
	}
	if (iBuf != fERROR)
	{
		DisposBlock(iBuf);
		iBuf = fERROR;
	}
	if (iAnim != fERROR)
	{
		DisposBlock(iAnim);
		iAnim = fERROR;
	}
	filename[0] = '\0';
	frame_in_buffer = -1;
	coord_x = 0;
	coord_y = 0;
}



void SimpleFLC::mfSetCurFrame(SHORT frameno)
{
	if (iAnim == fERROR || 
	    iHead == fERROR ||
	    frameno < 0 || 
	    frameno >= mfTotalFrames())
		return;
		
	run_timers();
	DumbAutoLockPtr<ANIMHDR> pAnim(iAnim);
	DumbAutoLockPtr<FLICHEAD> pHead(iHead);
	if (frameno < pAnim->frame)
	{
		pAnim->frame = 0;
		pAnim->offData = pHead->oframe1;
	}
	
	// FRAMEHEADPTR pFramehd;
	// FLICHEADPTR nondumb_pHead = pHead;
	
	while (pAnim->frame < frameno)
	{
		run_timers();
		// pFramehd = (FRAMEHEADPTR)((PTR)nondumb_pHead + pAnim->offData);
		// GWP BAD! Decode frame does this. pAnim->offData += pFramehd->size;
		pAnim->frame++;
		decode_frame(pAnim, pHead);
		frame_in_buffer = pAnim->frame;
	}
	
	pAnim->frame = frameno;
}


SHORT SimpleFLC::mfNextFrame()
{
	if (iAnim == fERROR ||
	    iHead == fERROR)
		return fERROR;
		
	run_timers();
	DumbAutoLockPtr<ANIMHDR> const pAnim(iAnim);
	DumbAutoLockPtr<FLICHEAD> const pHead(iHead);
	pAnim->frame++;
	if (pAnim->frame >= pAnim->totalFrames)
	{
		pAnim->frame = 0;
		pAnim->offData = pHead->oframe1;
		decode_frame(pAnim, pHead);
		frame_in_buffer = pAnim->frame;
	}
	
	return pAnim->frame;	
}


void SimpleFLC::mfDrawFrame(SHORT x, SHORT y)
{
	if (iAnim == fERROR || 
	    iHead == fERROR || 
	    iBuf == fERROR)
		return;

	if (x == -1)
		x = coord_x;
	
	if (y == -1)
		y = coord_y;
		
	run_timers();
	DumbAutoLockPtr<ANIMHDR> const pAnim(iAnim);
	DumbAutoLockPtr<FLICHEAD> const pHead(iHead);
	
	if (frame_in_buffer != pAnim->frame)
	{
		decode_frame(pAnim, pHead);
		frame_in_buffer = pAnim->frame;
	}
	DrawBitmap(x, y, pAnim->iBuff, 0, 0, pAnim->width, pAnim->height);
}

