/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: INTRO.C
   Author: 	 Michael D. Branham, Lan Zhou
   ========================================================================
   Contains the following general functions:

	void DoIntro(void)	-	shows the intro sequence

   Contains the following internal functions:

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <dos.h>
#include "SYSTEM.H"
#include "ENGINE.H"
#include "ENGINT.H"
#include "MACHINE.H"
#include "SOUND.HXX"
#include "FLICPLAY.HXX"
#if defined (AUDIO_LIB)
#include "KERNAUD.HPP"
#endif

#define NAME "intrbrpl"
#define PATH "intro"
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
//SHORT LoadFLC (CSTRPTR szFileName, BOOL* ffirst, BOOL* fend);
//ERRCODE decode_intro_frame (SHORT iHead, ANIMPTR pAnim);
//SHORT OpenIntro (SHORT iAnim, CSTRPTR szFileName, SHORT Type, BOOL *ffirst, BOOL *fend);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
//int frameFade[33] =  { 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//					   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5 };

//int frameDelay[33] = { 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//					   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 20 };

//int frameSound[33] = { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
//					   0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//FLICHEADPTR	pBlk;
//int             file;
//ULONG           offset;
//SHORT           framesCount=0;
/* ========================================================================
   Function    - DoIntro
   Description - show the intro sequence
   Returns     - void
   ======================================================================== */
#if 0
void DoIntro(void)
{
	SHORT	bitm;
	BITMPTR	p;
	int		iNextBitm = 0;
	int		iCurFrame = 0;
	int		waittime;
	char	szBitmName[40];
	char	szPalName[40];
	fAnyKeyChanged = FALSE;			/* clear dirty bit */

	// start with a black screen	
	FadeOut(0);
	clear_screen();
	// Show a screen
	while(1)
	{
		// leave if done
		if(iNextBitm > MAX_INTRO_BITMS)
			break;

		// create the file name and get the bitmap
		sprintf(szBitmName, "intro%2.2d.pcx", iNextBitm);
		sprintf(szPalName, "intro%2.2d", iNextBitm++);
		bitm = GetResourceStd(szBitmName, TRUE);

		ServiceSOLAudio();

		// display the bitmap
		if (bitm != fERROR)
		{
			// set bitmap scale to 1:1
			p = (BITMPTR) BLKPTR(bitm);
			p->scale = UNITARY_SCALE;
			p->x_ctr_pt = 0;

			// fade out and clear screen
			FadeOut(frameFade[iCurFrame++] * 100);
			clear_screen();
			update_screen();

			// load the palette for the image
			init_pal(szPalName);

			// make sure new palette is off
			FadeOut(0);

			// draw the screen, and fade it in
			DrawBitmap (0, 0, bitm, 0, 0, 999, 999);
			update_screen();
			FadeIn(frameFade[iCurFrame] * 100);

			// if there is a sound for this bitmap, playit
			if(frameSound[iCurFrame])
				kernel->AudioPlay(	(int) frameSound[iCurFrame] + 60000,
								(BOOL) 0, 		// looped
								(int) 127, 			// volume
								(int) 1,
								(long) 0); 		// paused

			// delete the bitmap
			DisposBlock(bitm);

#ifndef _WINDOWS
			waittime = get_time() + (18 * frameDelay[iCurFrame]);
			while(get_time() < waittime)
			{
				ServiceSOLAudio();
//				if(key_status(KEY_ESCAPE))
//				{
//					return;
//				}
			}
#endif
		}
	}
}
#endif
#if 0
/* ========================================================================
   Function    - PlayIntro
   Description - show the intro sequence
   Returns     - void
   ======================================================================== */
void PlayIntro()
{
   char  fn[40];
   sprintf(fn, "%s\\%s.flc", PATH, NAME);	
   PlayFlic(fn, 0);
}
/* =======================================================================
	Load FLC format files
	======================================================================= */
SHORT LoadFLC (CSTRPTR szFileName, BOOL* ffirst, BOOL* fend)
{
	SHORT			iBlk;
	SHORT           ihead1, ihead2;
	FRAMEHEADPTR	pFramehd;
	SHORT           length;

	if(*ffirst == TRUE)
	{
		file = FileOpen(szFileName, MODE_READFILE);	/* try to open the file */
		if (file == fERROR)								/* Resource not found. Print an error */
			return (SHORT)fFILE_NOT_FOUND;			/* mark file as not found */
		length = sizeof(FLICHEAD);
	
		ihead1 = NewBlock(length);
		pBlk = (FLICHEADPTR)BLKPTR(ihead1);

		if (read(file, pBlk, length) == fERROR)			/* read the first four bytes */
       	{	
			close(file);
			fatal_error("RESMANAG ERROR - data read failed %s\n",szFileName);
			return (SHORT)fERROR;
		}
		offset = pBlk->oframe1;
		lseek(file, -length, SEEK_CUR);
		lseek(file, offset, SEEK_CUR);
		*ffirst = FALSE;

	}

	length = sizeof(FRAMEHEAD);
	ihead2 = NewBlock(length);

	pFramehd  = (FRAMEHEADPTR)BLKPTR(ihead2);
	framesCount++;
	if (read(file, pFramehd, length) == fERROR)			/* read the first four bytes */
    {	
		close(file);
		fatal_error("RESMANAG ERROR - data read failed %s\n",szFileName);
		return (SHORT)fERROR;
	}
	lseek(file, -length, SEEK_CUR);
	if (pFramehd->size > 100000)
		fatal_error("ERROR - pFramehd->size invalid (%d) in decode_frame\n",pFramehd->size);
	offset += pFramehd->size;

	/* Allocate space for data */
	iBlk = NewBlock(pFramehd->size);

	
	if (iBlk != fERROR)
	{
		if (read(file, BLKPTR(iBlk), pFramehd->size) == fERROR)
		{
			close(file);
			DisposBlock(iBlk);
			fatal_error("RESMANAG ERROR - data read failed %s\n",szFileName);
			return (SHORT)fERROR;
		}
		else
		{
			if(framesCount == pBlk->frames)
			{
				close(file);
				*fend=TRUE;
			}
			DisposBlock(ihead2);
			return iBlk;
		}
	}
	else
	{
		close(file);
		fatal_error("RESMANAG ERROR - memory is unavailable %s\n",szFileName);
		return (SHORT)fERROR;
	}
}

/* =======================================================================
	decode_frame - Decode a frame that is in memory already into screen.
	Here we just loop through each chunk calling appropriate
	chunk decoder.
	======================================================================= */
ERRCODE decode_intro_frame (SHORT iHead, ANIMPTR pAnim)
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
SHORT OpenIntro (SHORT iAnim, CSTRPTR szFileName, SHORT Type, BOOL *ffirst, BOOL *fend)
{
	ANIMPTR			pAnim;
	SHORT				iHead;
	USHORT			i;
	BITMPTR         pBuff;
	SHORT                   scale = UNITARY_SCALE;
	if (iAnim <= fNOERR)		/* includes fERROR */
	{
		/* allocate memory for animation header and lock in place */
		if ( (iAnim = NewBlockHigh(sizeof(ANIMHDR))) == fERROR)
			return fERROR;
		pAnim = (ANIMPTR)BLKPTR(iAnim);
		pAnim->type			= Type;
		pAnim->sequence	= 9998;			/* out of range condition */
		pAnim->frame		= 9999;			/* out of range condition */
		for (i=0; i<NUMSEQUENCES; i++)
			pAnim->iData[i] = fERROR;
		pAnim->iBuff		= fERROR;
	}

	/* get the anim data into memory */
	SetBlockAttr(iAnim,LOCKED,LOCKED);		/* quick SetLock without memory move */
	pAnim = (ANIMPTR)BLKPTR(iAnim);
	iHead = LoadFLC(szFileName, ffirst, fend);
	pAnim->iData[0] = iHead;
	pAnim->width		= pBlk->width;
	pAnim->height		= pBlk->height;
	/* if iBuff exists and is not the same size, dispos before making a new one */
	if (pAnim->iBuff != fERROR)
	{
		pBuff = (BITMPTR) BLKPTR(pAnim->iBuff);
		if (pBuff->w == pAnim->width && pBuff->h == pAnim->height)
			return iAnim;		/* same size so no need to make a new one */

		scale = pBuff->scale;	/* save current scale */
		DisposBlock(pAnim->iBuff);
	}

	i = OpenBitm(pAnim->width, pAnim->height);
	pAnim = (ANIMPTR)BLKPTR(iAnim);			// recalc pAnim, OpenBitm might screw it up
	pAnim->iBuff = i;
	if (i == fERROR || IsHandleFlushed(i))
	{
//		printf("WARNING - could not allocate buffer in SetAnimSequence\n");
		fatal_error("ERROR - could not allocate buffer in SetAnimSequence\n");
		return fERROR;
	}

	((BITMPTR)BLKPTR(pAnim->iBuff))->scale = scale;


	if (iHead > 0)
	{
		SetClass2(iHead);				// allow all things to be purged en masse

	}

	ClrLock(iAnim);
	return iAnim;
}

#endif
