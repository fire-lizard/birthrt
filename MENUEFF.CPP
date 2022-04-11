#include "system.h"
#include "sysint.h"
#include "engine.h"
#include "engint.h"
#include "machine.h"
#include "machint.h"
#include "Main.hxx"
#include "light.h"
#include "sincos.h"
#include "soundfx.hxx"
#include "sound.hxx"

void SpinOutBitmap (SHORT x, SHORT y, SHORT iBitm, SHORT bx, SHORT by, SHORT w, SHORT h,SHORT hDest, POINT start, POINT bck, SHORT bckw, SHORT bckh)
{
	if (iBitm==fERROR)
		return;

	BITMPTR pBitm=(BITMPTR)BLKPTR(iBitm);
	long CenterX=x+pBitm->w/2;
	long Width=pBitm->w;
	 
	
	AddSndObj(SND_INTRO_VOX1,0,VOLUME_EIGHTY);	//renamed sound.... gotta fix it.

	// spin out a menu
	for (LONG i=0;i<63;i+=6)
	{
		double Scale=((double)1024.0)/cos_table[i];

		SHORT ScaledWidth=Width/Scale;
		SHORT TopLeftX=CenterX-ScaledWidth/2;

		if(hDest != fERROR)
			DrawBitmap (bck.x, bck.y, hDest, 0, 0, bckw, bckh);
		
		ScaleBitmapX(TopLeftX,y,iBitm, bx, by, w, h, (SHORT)(FULL_SCALE*Scale));
  	
		update_screen();
		DirectDrawPreFrame();
		run_timers();
	}
	if(hDest != fERROR)
		DrawBitmap (bck.x, bck.y, hDest, 0, 0, bckw, bckh);

	update_screen();
	DirectDrawPreFrame();
}	


void SpinInBitmap (SHORT x, SHORT y, SHORT iBitm, SHORT bx, SHORT by, SHORT w, SHORT h,SHORT hDest, POINT start, POINT bck, SHORT bckw, SHORT bckh)
{
 
	if (iBitm==fERROR)
		return;

	BITMPTR pBitm=(BITMPTR)BLKPTR(iBitm);
	long CenterX=x+pBitm->w/2;
	long Width=pBitm->w;


	// spin in a menu
	for (LONG i=63;i>=0;i-=6)
	{
		double Scale=((double)1024.0)/cos_table[i];
		SHORT ScaledWidth=Width/Scale;
		SHORT TopLeftX;

		if (ScaledWidth>Width)
		{
			//we don't want it to be bigger than the actual art!
			Scale=1;
			ScaledWidth=Width;
		}

		TopLeftX=CenterX-ScaledWidth/2;

		if(hDest != fERROR)
			DrawBitmap (bck.x, bck.y, hDest, 0, 0, bckw, bckh);
		
		ScaleBitmapX(TopLeftX,y,iBitm, bx, by, w, h, (SHORT)(FULL_SCALE*Scale));
  	
		update_screen();
		DirectDrawPreFrame();
		run_timers();
	}
	KillSoundsNoFade();
	AddSndObj(SND_INTRO_VOX2,0,VOLUME_EIGHTY);	//renamed sound.... gotta fix it.
	
}	


void ScrollOpenBitmap (SHORT x, SHORT y, SHORT iBitm, SHORT bx, SHORT by, SHORT w, SHORT h,SHORT hDest, POINT start, POINT bck, SHORT bckw, SHORT bckh)
{
 
	if (iBitm==fERROR)
		return;

	BITMPTR pBitm=(BITMPTR)BLKPTR(iBitm);
	long Height=pBitm->h;
	long CenterY=y+Height/2;
	
	LONG HalfHeight=Height/2;
	LONG i=HalfHeight;
	
	while(i)
	{
		DrawBitmap (x,y+i,hDest,0,i,1000,HalfHeight-i);
		update_screen();
		i--;
	}
}

void ScrollClosedBitmap (SHORT x, SHORT y, SHORT iBitm, SHORT bx, SHORT by, SHORT w, SHORT h,SHORT hDest, POINT start, POINT bck, SHORT bckw, SHORT bckh)
{
}