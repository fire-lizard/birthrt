/* =================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: DemoFlic.cpp - flic player wrapper for the demo
   Author: Michael Branham

   ========================================================================
   Contains the following general functions:

   PlayBattleFlc(void) - plays the battle flic
   PlayAdvFlc(void) - plays the adventure flic

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#ifdef _WINDOWS
#include <Windows.h>
#endif

#include "SYSTEM.H"
#include "MACHINE.H"
#include "ENGINE.H"

#include "FLICPLAY.HXX"
#include "GAMEKEY.HXX"
#include "SOUND.HXX"

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define TEN_SECONDS 182
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
void AddFlicSound(LONG whichFrame, LONG whichSound);
void ClearFlicSound(void);
void SlideShow(void);
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
extern LONG FlcDelay;
BOOL SlideShowPlaying = FALSE;

/* ========================================================================
   Function    - PlayBattleFlc
   Description - put up the demo for a battle field
   Returns     -
   ======================================================================== */
void PlayBattleFlc(void)
{
	SHORT	bitm;
	LONG 	now;
	SHORT	CurrentVolume;
	// load new palette
	FadeOut(100);
  	TurnOffAllSounds();
	CurrentVolume = GetGlobalVolume();
	SetGlobalVolume(0);
	clear_screen();
	update_screen();
  	init_pal("nova_l");
  	init_shade_table("nova_l");
	set_hires(0, 0);
	SetGlobalVolume(CurrentVolume);
	FadeIn(100);

#ifdef _NETDEMO	
	SlideShow();
#else
	AddSndObj(SND_BATTLE_LOOP1,0,-1);
	
	// play intro flic	
	bitm = GetResourceStd ("intro\\hbtlscr1.pcx", FALSE);
	if (bitm != fERROR)
	{
		DrawBitmap (0, 0, bitm, 0, 0, 999, 999);
		update_screen();
		SetPurge(bitm);
	}
	
	now=get_time()+TEN_SECONDS;
	fAnyKeyChanged = FALSE;
	mouse_button = 0;
	
	while(get_time()<now)
	{
		run_timers();
		
		#ifdef _WINDOWS
		ClearMessageQueue();
		#endif
					
		if(mouse_button)
		{
			mouse_button = 0;
			break;
		}
		else
		if(fAnyKeyChanged)
		{
			if(key_status(GAME_KEYS::mfGetKey(GAME_KEYS::STOP_RUNNING_DEMO)))
			{
				goto End;
			}
			fAnyKeyChanged = FALSE;
			clear_key_status(0);
			break;
		}
	}
	
	// add list of sound effects
	ClearFlicSound();
	AddFlicSound(6,SND_UI_BUTTON_CLICK);
	//	AddFlicSound(16,SND_TROOP_ACKNOWL2);
	AddFlicSound(17,SND_ARROW_SHOT1);
	AddFlicSound(18,SND_ARROW_SHOT2);
	AddFlicSound(19,SND_ARROW_SHOT3);
	AddFlicSound(20,SND_UI_BUTTON_CLICK);
	//	AddFlicSound(25,SND_MATURE_MAN_ACKNOWL8);
	AddFlicSound(30,SND_UI_BUTTON_CLICK);
	//	AddFlicSound(35,SND_TROOP_ACKNOWL3);
	AddFlicSound(79,SND_BIG_SWORD_HIT1);
	AddFlicSound(81,SND_MALE_SCREAM1);
	AddFlicSound(83,SND_SMALL_SWORD_HIT2);
	AddFlicSound(85,SND_MALE_SCREAM2);
	AddFlicSound(86,SND_ARROW_SHOT1);
	AddFlicSound(87,SND_ARROW_SHOT2);
	AddFlicSound(88,SND_ARROW_SHOT3);
	AddFlicSound(90,SND_METAL_ARMOR_HIT1);
	AddFlicSound(98,SND_UI_BUTTON_CLICK);
	AddFlicSound(100,SND_MALE_SCREAM3);
	//	AddFlicSound(103,SND_MATURE_MAN_ACKNOWL10);
	AddFlicSound(110,SND_UI_BUTTON_CLICK);
	//	AddFlicSound(114,SND_MATURE_MAN_ACKNOWL15);
	AddFlicSound(115,SND_SMALL_SWORD_HIT2);
	AddFlicSound(116,SND_MALE_DIE1);
	AddFlicSound(117,SND_METAL_ARMOR_HIT1);
	AddFlicSound(119,SND_BIG_SWORD_HIT1);
	AddFlicSound(120,SND_MALE_SCREAM3);
	AddFlicSound(122,SND_ARROW_SHOT1);
	AddFlicSound(124,SND_ARROW_SHOT2);
	AddFlicSound(125,SND_ARROW_SHOT3);
	AddFlicSound(127,SND_MALE_DIE1);
	AddFlicSound(130,SND_LEATHER_ARMOR_HIT1);
	AddFlicSound(135,SND_SMALL_SWORD_HIT2);
	AddFlicSound(137,SND_MALE_SCREAM3);
	AddFlicSound(142,SND_METAL_ARMOR_HIT1);
	AddFlicSound(145,SND_BIG_SWORD_HIT1);
	AddFlicSound(147,SND_SMALL_SWORD_HIT2);
	
	FlcDelay = 3;
	PlayFlic("intro\\hbattle.flc", 0);

#endif

End:	
	FadeOut(300);
	clear_screen();
	update_screen();
  	init_pal("nova_d");
  	init_shade_table("nova_d");
	ClearFlicSound();
  	TurnOffAllSounds();
	FadeIn(100);

}

/* ========================================================================
   Function    - PlayAdvFlc
   Description - put up the demo for an adventure
   Returns     -
   ======================================================================== */
void PlayAdvFlc(void)
{
	SHORT	bitm;
	LONG	now;
	SHORT   CurrentVolume;
	
	// load new palette
	FadeOut(100);
  	TurnOffAllSounds();
	clear_screen();
	update_screen();
  	init_pal("nova_l");
  	init_shade_table("nova_l");
	set_hires(0, 0);
  	TurnOffAllSounds();
	FadeIn(100);

#ifdef _DEMO	
	SlideShow();
#else
	// play intro flic	
	bitm = GetResourceStd ("intro\\advscrn1.pcx", FALSE);
	if (bitm != fERROR)
	{
		DrawBitmap (0, 0, bitm, 0, 0, 999, 999);
		update_screen();
		SetPurge(bitm);
	}
	
	// pause 10 seconds
	now=get_time()+TEN_SECONDS;
	fAnyKeyChanged = FALSE;
	mouse_button = 0;
	
	while(get_time()<now)
	{
		run_timers();
		
		#ifdef _WINDOWS
		ClearMessageQueue();
		#endif
					
		
		if(mouse_button)
		{
			mouse_button = 0;
			goto End;
		}
		else
		if(fAnyKeyChanged)
		{
			fAnyKeyChanged = FALSE;
			clear_key_status(0);
			goto End;
		}
	}
	
	// add list of sound effects
	ClearFlicSound();
	AddFlicSound(1,-(TEN_SECONDS/20));
	//	AddFlicSound(2,SND_GUARD_CHALLENGE1);
	//	AddFlicSound(7,SND_DUEL_TAUNT1);
	AddFlicSound(14,SND_UI_MENU_ZOOM_IN);
	AddFlicSound(23,SND_BIG_SWORD_HIT1);
	AddFlicSound(24,SND_LEATHER_ARMOR_HIT2);
	AddFlicSound(33,SND_BIG_SWORD_HIT2);
	AddFlicSound(35,SND_SMALL_SWORD_HIT2);
	AddFlicSound(42,SND_METAL_ARMOR_HIT1);
	AddFlicSound(43,SND_MALE_SCREAM3);
	AddFlicSound(47,SND_LEATHER_ARMOR_HIT1);
	//	AddFlicSound(50,SND_DUEL_TAUNT10);
	AddFlicSound(51,SND_MALE_DIE1);
	AddFlicSound(54,SND_BODY_FALLS_DEAD1);
	AddFlicSound(58,-(18*2));
	
	FlcDelay = 3;
	PlayFlic("intro\\advture.flc", 0);
#endif // _DEMO

End:	
	FadeOut(300);
	clear_screen();
	update_screen();
  	init_pal("nova_d");
  	init_shade_table("nova_d");
	ClearFlicSound();
  	TurnOffAllSounds();
#ifdef _DEMO
  	ServiceMusicWave(TRUE);
#endif
	FadeIn(100);

}

/* ========================================================================
   Function    - SlideShow
   Description - play the .pcx slide show
   Returns     -
   ======================================================================== */
void SlideShow(void)
{
	CHAR	filename[40];
	SHORT	i;
	SHORT	bitm;
	LONG	now;
	
	// remove the timer fuction that draws the mouse tostop the
	// mouse from drawing, because the mouse code is so screwed
	// up that SysHideCursor/SysShowCursor doesn't actually
	// stop the mouse from drawing!
	//GEH remove_timer(draw_cursor);

	TurnOffAllSounds();
#ifdef _DEMO
	SlideShowPlaying = TRUE;
#endif
	AddSndObj(SND_SLIDE_SHOW_MUSIC_LOOP1,0,-1);
	
	fAnyKeyChanged = FALSE;
	mouse_button = 0;
	
	for(i = 1; i <= 15; i++)
	{
	
		// FadeOut(10);
		
		// start zoom sound early
		//AddSndObj(SND_UI_MENU_ZOOM_IN, 0, -1);
		
		// play intro flic	
		sprintf(filename, "intro\\screen%02d.pcx", i);
		
		bitm = GetResourceStd (filename, FALSE);
  		
		if (bitm != fERROR)
		{
			POINT start,bck;
			
			start.x = 320;
			start.y = 240;
			bck.x = 320;
			bck.y = 240;
			ZoomBitmap (0, 0, bitm, 0, 0, 640, 480, fERROR, start, bck, 640, 480);
			//DrawBitmap (0, 0, bitm, 0, 0, 999, 999);
			update_screen();
			SetPurge(bitm);
			//FadeIn(10);
		}
		else
		{
			continue; // can't find this one, try the next
		}
		
		// delay 4 seconds
		now=get_time() + (6 * 182 / 10);
		
		while(get_time()<now)
		{
			run_timers();
			
			#ifdef _WINDOWS
			ClearMessageQueue();
			#endif
						
			update_buttons();
			if(mouse_button)
			{
				mouse_button = 0;
				break;
			}
			else
			if(fAnyKeyChanged)
			{
				if(key_status(KEY_ESCAPE))
				{
					goto End;
				}
				fAnyKeyChanged = FALSE;
				clear_key_status(0);
				break;
			}
		}
	}
End:
	fAnyKeyChanged = FALSE;
	clear_key_status(0);
	SlideShowPlaying = FALSE;
	return;	
}
