/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: GAME.C
   Author: Chris Phillips & Wes Cumberland
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:
   init_game  -initializes game session (mouse, doors, loads level, etc...)

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <io.h>
#include <ctype.h>
#if defined(_WINDOWS)
#include <windows.h>
#else
#include <time.h>
#endif

#if defined(_EDIT)
#include "edit.h"
#endif

#include "SYSTEM.H"
#include "ENGINE.H"
#include "MACHINE.H"

#include "ACTNMENU.HXX"
#include "AI_UTILS.H"
#include "DLG_EDIT.H"
#include "GAME.H"
#include "GAMETYPE.HXX"
#include "ANIM.H"
#include "REALM.HXX"
#include "SCENE.HXX"
#include "SCNMGR.HXX"
#include "REGENTS.HXX"
#include "ENGINT.H"
#include "TASK.H"
//#include "btlstr.h"
#include "MACHINT.H"
#include "MAGIC.H"
#include "STRMGR.H"
#include "strenum.h"
#include "GAMEMAP.HXX"
#include "INVNTORY.HXX"
#include "INVNGUI.HXX"
#include "INVNBUTS.HXX"
#include "PLAYSTAT.HXX"
#include "VERSION.HXX"
#include "MULTIMAP.HXX"
#include "MULTIUI.HXX"
#include "STRENGTH.HXX"
#include "ITEMTYPE.HXX"
#include "SNDVOX.HXX"
#include "LOADSAVE.HXX"
#include "CHARSEL.HXX"
#include "CONTEXT.HXX"
#include "DOSHELP.HXX"
#include "COLORS.H"

#ifdef _WINDOWS
#include ".\WINSYS\DDRAWPRO.H"
#include ".\WINSYS\MULPLAY.HXX"
#endif

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

#define FONT_TITL_40PT  13
#define STR_GAMEICON "UI\\GAMEICON.PCX"
#define STR_SHDWICON "UI\\SHDWICON.PCX"

#define STATUS_FRAMES_PER_SEC	2

typedef enum {
	REALM_KEYS = 2,
	RULES,
	GAMEPLAY,
	ADVENTURE_KEYS,
	BATTLE_KEYS,
	REALM_TUTORIAL,
	ADVENTURE_TUTORIAL,
	BATTLE_TUTORIAL,
} HELP_TOPICS;

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

void InitDomainTurn(LONG);
void BeginRule(LONG, LONG);
void DoneHelpKeys(LONG, LONG);
void PaintQuestItemFound(LONG,LONG);
void QuestItemFoundProc(LONG,LONG);
void Sinners(LONG, LONG);
BOOL BattlePrep(void);


void SpinOutBitmap (SHORT x, SHORT y, SHORT iBitm, SHORT bx, SHORT by, SHORT w, SHORT h,SHORT hDest, POINT start, POINT bck, SHORT bckw, SHORT bckh);
void SpinInBitmap (SHORT x, SHORT y, SHORT iBitm, SHORT bx, SHORT by, SHORT w, SHORT h,SHORT hDest, POINT start, POINT bck, SHORT bckw, SHORT bckh);


void PaintHelpKeys (LONG MenuCombo, LONG arg);
void ToggleDirectDraw(LONG, LONG);

#ifdef _WINDOWS
HWND	hwndApp;
#endif

static void InitTroopDisplay (LONG type, LONG realm);
static void PaintTroopCard();
static LONG AddUnit (SHORT StartIndex, LONG prov, LONG icon, LONG id, LONG realm);
static LONG DelUnit (SHORT StartIndex, SHORT KillIndex);
static void MusterList( SHORT StartIndex );

static void PaintHello( LONG MenuCombo, LONG );

void NextHelpKeys(LONG, LONG);


extern void SetRedrawMainMapLevel (void);

#if defined(_DEBUG)
extern BOOL fWonGame;
void PlayFinale(LONG,LONG);
#endif

//extern "C" { SHORT InitRedirectableVideo(LONG VWidth, LONG VHeight); }

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
extern LONG		dturn_mode;
BOOL 			fMainMenuUp = FALSE;
BOOL			fChatLineOn = FALSE;
static CHAR	ChatLine[CHAT_MAXLINES][CHAT_CHARS];
static SHORT	ChatLineColor[CHAT_MAXLINES];
static SHORT	ChatLineHead = 0;
SHORT   ChatDisplaySize = 0;


static CHAR		SysHelpTitle[80];
static CHAR		SysHelpText[4096];
static PFVLL	SysHelpProc;
CHAR			SysHelpDialogArt[40];
static BOOL     AttribFlag=FALSE;
extern void PlayGorgon(LONG arg,LONG);
// used in gamemap.cpp
BOOL			fAutoMode = FALSE;

BOOL fAIAnimate = TRUE;

extern int iChatKey;
char		pwad[40] = "";
char		pInitScene[9] = "worldmap";
void SetRedrawMainMapLevel (void);

//SHORT hPlayerStats = fERROR;

extern BOOL fVeryFirstTime;
extern LONG new_treasury;
extern BOOL fAIAutoRes;
extern BOOL fFreezeDomainUI;

extern BOOL	mundane_descriptions;

BOOL fStartMultiGame = FALSE;

/* the list of valid menus */
#include "MENU.H"		// NOTE: this has the enum for the array below,
						// !!!!!! keep it up to date !!!!!!
#include "PANEL.H"

#include "MENUDATA.H"
#include "GAMEMENU.H"

MENU Menus[MAX_MENUS] = {
	{Help, HelpSize},
	{DomainAction, DomainActionSize},
	{ReportMenu, ReportMenuSize},
	{MainMenu, MainMenuSize},
	{Question1, Question1Size},
	{Question2, Question2Size},
	{Question3, Question3Size},
	{GameButton, GameButtonSize},
	{LoadOurMenu, LoadMenuSize},
	{SaveMenu, SaveMenuSize},
	{OptionMenu, OptionMenuSize},
	{DOSHelp, DOSHelpSize},
	{ChatInputWindow, ChatInputWindowSize},
	{ChatSelect, ChatSelectSize},
	{AvatarAttributes, AvatarAttributesSize},
	{AvatarInventory, AvatarInventorySize},
	{BtlControl, BtlControlSize},
	{BtlChooseField, BtlChooseFieldSize},
	{CombatControl1, CombatControlSize1},
	{CombatControl2, CombatControlSize2},
	{CombatControl3, CombatControlSize3},
	{CombatControl4, CombatControlSize4},
	{CombatFighterControl1, CombatFighterControlSize1},
	{CombatFighterControl2, CombatFighterControlSize2},
	{CombatFighterControl3, CombatFighterControlSize3},
	{CombatFighterControl4, CombatFighterControlSize4},
	{CombatRPGBottomBar, CombatRPGBottomBarSize},
	{CombatBlank1Bar, CombatBlank1BarSize},
	{ShowAnimation, ShowAnimationSize},			//add by lan for animation
	{MessageBoxTitled,MessageBoxTitledSize},
	{SmallInfoBox,InfoBoxSize},
	{ResurrectionScreen, ResurrectionScreenSize},
	{MoreInfo, MoreInfoSize},
	{BattleCard, BattleCardSize},
	{Victory, VictorySize},
	{Defeat, DefeatSize},
	{HelpKeys, HelpKeysSize},
//	{MultiMenu, MultiMenuSize},
//	{MultiMenuL, MultiMenuLSize},
//	{MultiMenu1, MultiMenu1Size},
//	{MultiMenu2, MultiMenu2Size},
	{AvatarAttributes2, AvatarAttributes2Size},
	{ChatDisplayWindow, ChatDisplayWindowSize},
	{ChatDisplayWindow1, ChatDisplayWindow1Size},
	{LoyaltyMenu, LoyaltyMenuSize},
	{DiplomacyAction, DiplomacyActionSize},
	{BattleMagicPrep, BattleMagicPrepSize},
	{BattleCommand, BattleCommandSize},
	{BuildArmys, BuildArmysSize},
	{BuildAdv, BuildAdvSize},
	{GameSelMenu, GameSelMenuSize},
	{AdvisorMenu, AdvisorMenuSize},
	{MultiWait, MultiWaitSize},
	{DescribeMenu, DescribeMenuSize},
	{SystemLargeHelp, SystemLargeHelpSize},
	{SystemMedHelp, SystemMedHelpSize},
	{SystemSmallHelp, SystemSmallHelpSize},
	{TutorialMenuWide, TutorialMenuWideSize},
	{TutorialMenuMed, TutorialMenuWideMed},
	{HoldAction, HoldActionSize},
	{LoadingScene, LoadingSceneSize},
	{GameCmplxMenu, GameCmplxMenuSize},
	{AdvPrep, AdvPrepSize},
	{Parchment, ParchmentSize},
	{MultiHost, MultiHostSize},
	{MultiEnvironment, MultiEnvironmentSize},
	{MultiName, MultiNameSize},
	{MultiJoin, MultiJoinSize},
	{MultiCreate, MultiCreateSize},
};

SHORT hTroopBitm = fERROR;	// used by battle cards
SHORT iTroopRealm = fERROR;	// used by battle cards
SHORT iTroopType = fERROR;	// used by battle cards
SHORT iTroopScale = FULL_SCALE;	// used by battle cards

SHORT iHelpHighlight = 2;

BOOL fShowProgressBar = FALSE;
static LONG gLoadingPercent = 0;

static REALM::REALM_TYPE Realm = REALM::NO_COUNTRY;

static SHORT iAnim;
static SHORT iAbortSlideShow;

struct{
	char buffer[80];
	USHORT seq;
	}anim;
static char	action[] = {'S','W','D','A','F','X','C','E','L','G','B','M','Z'};


/* status at the start of the game */
BOOL char_selected = FALSE;

void GoToBattleScene(LONG, LONG);


LONG cheated = 0;


// put here for now as JOHNC had game.cpp checked out
extern SHORT iHelpHighlight;
void DoSlideShow(LONG iWhich);


void ShowHelpMenu ( LONG, LONG )
{
   if ( mouse_button == 2 )
   {
   	SystemHelp( STR_MAIN1_HELP_HELP_TITLE,
                   STR_MAIN1_HELP_HELP_TEXT, H_Help, 0, 0 );
   }
   else
   {
#if defined(_JUNEDEMO)
	SetButtonLabelColor(D_HELP, 3, 16);
	SetButtonLabelColor(D_HELP, 4, 16);
#endif
   	ShowSubMenu(0, D_HELP);
   }
}

void DoneHelpMenu(LONG , LONG)
{
	iAbortSlideShow = FALSE;
	CHAR	chFileName[80];

	switch(iHelpHighlight)
	{
		case	REALM_KEYS:
			InitDOSHelp(H_TOPIC_QUICK_KEYS, 0);
			break;


		case	RULES:
#if defined(_JUNEDEMO)
			break;
#endif			

#ifdef _WINDOWS
	#ifdef _FOREIGNVER
			InitDOSHelp(H_TOPIC_ATLAS, 0);
			break;
	#endif
//			if (sDrawMode == iDDRAW)
//				ToggleDirectDraw(0, 0);

           //---- Put us in normal mode  ( well as normal as possible... )
			if (sDrawMode == iDDRAW)
              DDSetCooperative ( FALSE );

			if( '\0' != CDDrive[0] )
				sprintf(chFileName, "%s%s%s", CDDrive,InstallPath, "BIRTHRT.HLP");
			else
				sprintf(chFileName, "%s%s", InstallPath, "BIRTHRT.HLP");
			
			WinHelp(hwndApp, chFileName, HELP_CONTENTS, 0);
			break;
#else
			//HideSubMenu(0, D_HELP);
			//RunMenus();
			InitDOSHelp(H_TOPIC_ATLAS, 0);
			break;
#endif

		case	GAMEPLAY:
#if defined(_JUNEDEMO)
			break;
#endif			
#ifdef _WINDOWS
	#ifdef _FOREIGNVER
			InitDOSHelp(H_TOPIC_RULES, 0);
			break;
	#endif
           //---- Put us in normal mode  ( well as normal as possible... )
			if (sDrawMode == iDDRAW)
              DDSetCooperative ( FALSE );

			if( '\0' != CDDrive[0] )
				sprintf(chFileName, "%s%s%s", CDDrive,InstallPath,  "GAMEPLAY.HLP");
			else
				sprintf(chFileName, "%s%s", InstallPath, "GAMEPLAY.HLP");
			
			WinHelp(hwndApp, chFileName, HELP_CONTENTS, 0);
			break;
#else
//			HideSubMenu(0, D_HELP);
//			RunMenus();
			InitDOSHelp(H_TOPIC_RULES, 0);
			break;
#endif
	}

}

void DoSlideShow(LONG iWhich)
{
	CHAR	filename[40];
	SHORT	i;
	SHORT	bitm;
	SHORT	hBitm;
	LONG	now;
	BITMPTR	p;

	// remove the timer fuction that draws the mouse to stop the
	// mouse from drawing, because the mouse code is so screwed
	// up that SysHideCursor/SysShowCursor doesn't actually
	// stop the mouse from drawing!
	//GEH remove_timer(draw_cursor);

//	TurnOffAllSounds();

	// save the background
	hBitm = SaveBitmap(0, 0, 640, 480);
	
	fAnyKeyChanged = FALSE;
	mouse_button = 0;

	for(i = 1; i <= 15; i++)
	{
	
		// FadeOut(10);
		
		// start zoom sound early
		//AddSndObj(SND_UI_MENU_ZOOM_IN, 0, -1);
		
		// get the file
		switch(iWhich)
		{
			case 1:		// realm
				sprintf(filename, "intro\\rlmtut%02d.pcx", i);
				break;

			case 2:		// adventure
				sprintf(filename, "intro\\advtut%02d.pcx", i);
				break;

			case 3:		// battle
				sprintf(filename, "intro\\btltut%02d.pcx", i);
				break;
		}
		
		bitm = GetResourceStd (filename, FALSE);
  		
		if (bitm != fERROR)
		{
			POINT start,bck;
			
			// set bitmap scale to 1:1
			p = (BITMPTR) BLKPTR(bitm);
			p->scale = UNITARY_SCALE;
			p->x_ctr_pt = 0;

			start.x = 320;
			start.y = 240;
			bck.x = 320;
			bck.y = 240;
			ZoomBitmap (0, 0, bitm, 0, 0, 640, 480, fERROR, start, bck, 640, 480);
			init_gfont(FONT_SANS_12PT);
			print_text_centered(320,12,"Click to continue - ESC to abort", WHITE);
			update_screen();
			SetPurge(bitm);
			bitm = fERROR;
			update_buttons();
		}
		else
		{
			continue; // can't find this one, try the next
		}

		// delay for mouse debounce
		now=get_time() + 40;
		while(get_time()<now)
		{}

		// delay 5 min
		now=get_time() + 6000;

		mouse_button = 0;
		fAnyKeyChanged = FALSE;
		
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
					iAbortSlideShow = TRUE;
					goto End;
				}
				fAnyKeyChanged = FALSE;
				clear_key_status(0);
				break;
			}
		}
	}
End:
	// restore background
	DrawBitmap (0, 0, hBitm, 0, 0, 640, 480);
	DisposBlock(hBitm);

	fAnyKeyChanged = FALSE;
	clear_key_status(0);
	//GEH add_timer(1,draw_cursor);
	return;	
}

void NextHelpKeys(LONG, LONG)
{
	LONG arg;
	arg = GetButtonArg(D_HELPKEYS, 0);
	ChangeButtonProc (D_HELPKEYS, 0, PaintHelpKeys, arg+1, 0);
}

/* ========================================================================
   Function    - PaintHelpKeys
   Description - paint the text on the help menu
   Returns     -
   ======================================================================== */
void PaintHelpKeys (LONG MenuCombo, LONG arg)
{
	LONG    X,Y,W,H;
	LONG    mx,my;
	LONG    MenuId, ButtonId;
	CHAR    color = 228;
	CHAR    buffer[10];
	char    textbuf[4096];
	char    buff[80];
	FILE    *in;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	// get position of help screen
	if( GetButtonPosition( MenuId, 1, &mx, &my ) == fERROR)
		return;

	X = mx;
	Y = my - 5;

	switch(arg)
	{
		case 1:	// realm info
			sprintf(buff,"%sTEXT\\rlmhelp.txt",InstallPath);
			break;

		case 2:	// adventure info
			sprintf(buff,"%sTEXT\\advhelp.txt",InstallPath);
			break;

		case 3:	// battle info
			sprintf(buff,"%sTEXT\\btlhelp.txt",InstallPath);
			break;
	}

	// clear the buffer
	memset(&textbuf[0], 0, sizeof(textbuf));

	in = FileOpen(buff, "r");
	if(in != NULL)
	{
		fread(textbuf, sizeof(textbuf), 1, in);
		FileClose(in);
		gprint_text(X, Y, textbuf, color);
	}
	
	if (arg == 3)
	{
		ChangeButtonProc (D_HELPKEYS, 2, HideSubMenuWithClick, D_HELPKEYS, KEY_N);
	}
	else
	{
		ChangeButtonProc (D_HELPKEYS, 2, NextHelpKeys, 0, KEY_N);
	}
}



/* =======================================================================
   Function    - init_game
   Description - initializes game related stuff, such as the mouse, and doors
   Returns     - void
   ======================================================================== */
void init_game()
{
//	InitRedirectableVideo(256, 128);

	init_doors();

	GAME_TTYPE::mfRegisterTTypes();
	
	InitMagicItems();
	SetQuestThing(RING_OF_NIGHTVISION);

	
	// Show the system menu button first
	InitMenuSys(&Menus[0], sizeof(Menus) / sizeof(MENU));
	
	// setup avatar map draws
	SetMapGameDraw(SCENE::mfDrawMapStuff);
	
    // We wait to scan the resfiles until after the intro has played.
    ScanResFiles ();

	if (pwad[0] == 0)			// check for command line override
	{
		SCENE_MGR::mfRequestNewScene( pInitScene , TYPE_PLAYERSTART1, TRUE); // load first scene
	}
	else
	{
		load_new_wad(pwad, TYPE_PLAYERSTART1);
		add_task(1,handle_doors,0);  	//once every frame see if any doors moving
		add_task(1,HandleLifts,0);  	//once every frame see if any lifts moving
		add_task(1,HandleFloors,0);  	//once every frame see if any floors moving
		add_task(1,HandleCeilings,0); //once every frame see if any ceilings moving
		add_task(1,TextureFrameHandler,0);  //once every frame check lighting effects
#ifdef _EDIT
		EditOpeningMessage ();
#endif
	}
	
//	REALM::mfInitAllyTable();		changed 8-30 ABC
}


/* ========================================================================
   Function    - GameButtonPaint
   Description - paint routine for the game button
   Returns     -
   ======================================================================== */
void GameButtonPaint( LONG MenuCombo, LONG )
{
	LONG	X,Y,W,H;
	LONG	MenuId, ButtonId;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	GetButtonPosition ( MenuId, 1, &X, &Y );
	GetButtonSize ( MenuId, 1, &W, &H );
	
	if(cursor_in_box(X,Y,W,H))
	{
		// change art to color art
		SetButtonArt ( MenuId, 1, STR_GAMEICON );
	}
	else
	{
		// change art to shadow art
		SetButtonArt ( MenuId, 1, STR_SHDWICON );
	}



#ifdef _WINDOWS
	
	AMultiPlayer.DisplayNetStatus ( X, Y );

#endif


	
}

/* ========================================================================
   Function    - GameButtonPressed
   Description - bring up the game menu
   Returns     -
   ======================================================================== */
void GameButtonPressed(LONG, LONG)
{
	GAMEToggleMainMenu(TRUE, 0);
}

/* ========================================================================
   Function    - MainMenuClick
   Description - handle main menu button clicks
   Returns     -
   ======================================================================== */
void MainMenuClick (LONG MenuCombo, LONG ButtonVal)
{
	LONG	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
#if defined(_NETJUNEDEMO)
	if (ButtonVal == 3 || ButtonVal == 4)
		return;
#else
  #if defined(_JUNEDEMO)
	if (ButtonVal == 3 || ButtonVal == 4 || ButtonVal == 6)
		return;
  #endif	
#endif

	// click the button
#if !defined(_WINDOWS)	
	if (ButtonVal != 6)
#endif	
	{
		SetButtonHilight(MenuId, ButtonId, TRUE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
		SetButtonHilight(MenuId, ButtonId, FALSE);
		fUpdatePanels = TRUE;
		RunPanels();
		RunMenus();
		update_screen();
		TickDelay(4);
	}
	
#if defined(_MULTIPLAY_ONLY)
	if (ButtonVal == 3)
	{
		SHORT   bitm;
		LONG    now;
		
		//GEH Splash Screen to tell them to buy the full product
 		bitm = GetResourceStd ("intro\\nosingle.pcx", FALSE);
		if (bitm != fERROR)
		{
			// put up the art
			DrawBitmap (0, 0, bitm, 0, 0, 999, 999);
			update_screen();
			SetPurge(bitm);
			
			// delay 20 seconds
			now=get_time() + (20 * 182 / 10);
		
			#ifdef _WINDOWS
			ClearMessageQueue();
			#endif
						
			fAnyKeyChanged = FALSE;
			clear_key_status(0);
			mouse_button = 0;
	
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
						break;
					}
					fAnyKeyChanged = FALSE;
					clear_key_status(0);
					break;
				}
			}
		}
		
		return;
	}
#endif

	fStartMultiGame = FALSE;
	
	switch(ButtonVal)
	{
	case 1:
		BackToGame(0,0);
		break;
	case 2:
		DomainNewGame(0,0);
		break;
	case 3:
		LoadGame(0,0);
		break;
	case 4:
		SaveGame(0,0);
		break;
	case 5:
		OpenOptions(0,0);
		break;
	case 6:
#ifdef _WINDOWS
		fStartMultiGame = TRUE;
		ShowMultiPlayerMenu(0,0);
#endif
		break;
	case 7:
		ShowHelpMenu(0,D_HELP);
		break;
	case 8:
		CPPQuitSys(0,0);
		break;
	default:
		break;
	}
}

/* ============================================================================
   Function		 - PaintMainMenu
   Description - paint the text on the main menu
   Returns     -
   ======================================================================== */
void PaintMainMenu (LONG MenuCombo, LONG )
{
}

/* ========================================================================
   Function    - ShowStatus
   Description - put up the status dialog
   Returns     -
   ======================================================================== */
void ShowStatus(LONG tID, LONG hPossibleStats)
{
	SHORT hPlayerStats;
	BOOL enemy=FALSE;
	if(tID < 0)
	{
		enemy = TRUE;
		tID = -tID;
	}

	if (hPossibleStats == 0)
	{
		if(fERROR == (hPlayerStats = LoadStats(tID, fERROR)))
			return;
	}
	else
	{
		hPlayerStats = hPossibleStats;
	}
	
	DumbHandlePtr<PLAYER_STATS const> const pPlayerStats(hPlayerStats);
	
	anim.seq = STANDSEQ;
	#if defined(_JUNEDEMO)
	iAnim = AllocateAnim(TYPE_FLIC, GAME_TTYPE::mfGetLowResType(pPlayerStats->mfGetType()));
	#else
	iAnim = AllocateAnim(TYPE_FLIC, pPlayerStats->mfGetType());
	#endif
	if (iAnim > 0)
	{
		#if defined(_JUNEDEMO)
		load_FLC_sequence(iAnim,
						  GAME_TTYPE::mfGetLowResType(pPlayerStats->mfGetType()),
						  anim.seq,
						  0	// rotation Zero.
						  );
		#else
		load_FLC_sequence(iAnim,
						  pPlayerStats->mfGetType(),
						  anim.seq,
						  0	// rotation Zero.
						  );
	    #endif
		
		SetAnimSequence(iAnim, (anim.seq * NUMROTATIONS));
    }
	
	SHORT my_menu = (AttribFlag = (fDomainTurn&&enemy))?
		(D_AVATAR_ATTRIBUTES2):(D_AVATAR_ATTRIBUTES);
		
	SetButtonValue(my_menu, BUTNO_STATUS_HDL, hPlayerStats);
	
	if (mundane_descriptions)
	{
		SetButtonLabel(my_menu, 18, STR_AVA_ATT_ARM2, 173);
		SetButtonLabel(my_menu, 16, STR_AVA_ATT_THA2, 173);
	}
	else
	{
		SetButtonLabel(my_menu, 18, STR_AVA_ATT_ARM, 173);
		SetButtonLabel(my_menu, 16, STR_AVA_ATT_THA, 173);
	}
	
	ShowMenu(my_menu);
}

/* ========================================================================
   Function    - DisposeStatus
   Description - throw out the status info
   Returns     -
   ======================================================================== */
void HideStatus(LONG MenuCombo, LONG)
{
	SHORT	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	if (iAnim > 0)
	{
		MarkOldAnimPurgable(iAnim, anim.seq);
		RemoveAnim(iAnim);
		iAnim = fERROR;
	}
	
	// click the button
	SetButtonHilight(D_AVATAR_ATTRIBUTES, 4, TRUE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	SetButtonHilight(D_AVATAR_ATTRIBUTES, 4, FALSE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	

	HideMenu(D_AVATAR_ATTRIBUTES);
    RunMenus();
}

/* ========================================================================
   Function    - DisposeStatus
   Description - throw out the status info
   Returns     -
   ======================================================================== */
void HideStatus2(LONG MenuCombo, LONG)
{
	SHORT	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	if (iAnim > 0)
	{
		MarkOldAnimPurgable(iAnim, anim.seq);
		RemoveAnim(iAnim);
		iAnim = fERROR;
	}
	
	// click the button
	SetButtonHilight(D_AVATAR_ATTRIBUTES2, 4, TRUE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	SetButtonHilight(D_AVATAR_ATTRIBUTES2, 4, FALSE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);

	HideMenu(D_AVATAR_ATTRIBUTES2);
    RunMenus();
}

/* ========================================================================
   Function    - ShowCharacter
   Description - put up the status dialog
   Returns     -
   ======================================================================== */
#if 0 // UNUSED
void ShowCharacter(LONG tID, LONG hPossibleStats)
{
	SHORT hPlayerStats;
	
	if (hPossibleStats == 0)
	{
		if(fERROR == (hPlayerStats = LoadStats(tID, fERROR)))
			return;
	}
	else
	{
		hPlayerStats = hPossibleStats;
	}
	anim.seq = STANDSEQ;
	DumbHandlePtr<PLAYER_STATS const> const pPlayerStats(hPlayerStats);
	
	iAnim = AllocateAnim(TYPE_FLIC, pPlayerStats->mfGetType());
	
	if (iAnim > 0)
	{
		#if defined(_JUNEDEMO)
		load_FLC_sequence(iAnim,
						  GAME_TTYPE::mfGetLowResType(pPlayerStats->mfGetType()),
						  anim.seq,
						  0	// rotation Zero.
						  );
		#else
		load_FLC_sequence(iAnim,
						  pPlayerStats->mfGetType(),
						  anim.seq,
						  0	// rotation Zero.
						  );
		#endif
		SetAnimSequence(iAnim, (anim.seq * NUMROTATIONS));
    }
	
	SetButtonValue(D_BEGIN_RULE, BUTNO_STATUS_HDL, hPlayerStats);
	
	ShowMenu(D_BEGIN_RULE);
}
#endif

/* ========================================================================
   Function    - HideCharacter
   Description - throw out the status info
   Returns     -
   ======================================================================== */
#if 0 // UNUSED
void HideCharacter(LONG, LONG)
{
	if (iAnim > 0)
	{
		MarkOldAnimPurgable(iAnim, anim.seq);
		RemoveAnim(iAnim);
		iAnim = fERROR;
	}
	HideMenu(D_BEGIN_RULE);
	RunMenus();
}
#endif

/* ========================================================================
   Function    - checksequence
   Description - check if reach the end of the sequence
   Returns     -
   ======================================================================== */
static BOOL CheckSequence (SHORT const iAnim)
{
	BOOL Result = TRUE;		// If there is no art, it must be done!
	SHORT iHead;
	FLICHEADPTR pHead;
	
	if (iAnim > 0)
	{
		ANIMPTR pAnim = (ANIMPTR)BLKPTR(iAnim);
		if (pAnim->hiData > 0)
		{
			I_DATA_BLK *piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
			iHead = piData->iData[anim.seq * NUMROTATIONS];
			if (iHead > 0)
			{
				pHead = (FLICHEADPTR)BLKPTR(iHead);
				if (IsPointerGood(pHead))
				{
					if(pAnim->frame >= (pHead->frames-1))
					{
						Result = TRUE;
					}
					else
					{
						Result = FALSE;
					}
				}
			}
		}
	}
	
	return Result;
}


/* ========================================================================
	Function    - getStatusRealm
	Description	- Set home realm
	Returns     -
   ======================================================================== */
void SetStatusRealm(REALM::REALM_TYPE realm)
{
	Realm = realm;
}




//this table correlates to the ordering of the help strings in the string table.
static LONG StatusContextHelpTopics[]={
H_Class,
H_Level,
H_Bloodline,
H_Alignment,
H_Strength,
H_Intelligence,
H_Dexterity,
H_Wisdom,
H_Constitution,
H_Charisma,
H_THACO,
H_Experience,
H_Armor_Class,
H_Hit_Points,
H_Hit_Points,
H_Armor_Class,
H_Weapon,
H_Gold,
};

void ShowStatusContextHelp(LONG MC,LONG HelpStr)
{
	static GAME_STRING FirstStatusHelpStr=STR_STATUS_HELP_CLASS;
	if (mouse_button==2)
		SystemHelp(STR_PLAIN_STATUS,HelpStr,StatusContextHelpTopics[HelpStr-FirstStatusHelpStr],NULL,0);
}	


/* ========================================================================
   Function    - PaintAttributes
   Description - paint the text on the attributes
   Returns     -
   ======================================================================== */
void PaintAttributes (LONG MenuCombo, LONG )
{
	SHORT	i;
	LONG	X,Y;
	LONG	mx,my;
	LONG	MenuId;
	LONG	ButtonId;
	CHAR	buffer[80];
	SHORT	TabStop = 162;	// tunable tab stop value
	UBYTE   ctrl;
	LONG	timedif;
	SHORT	hBitm;
	LONG	hPlayerStats;
	SHORT	regentNum = 0;
	char path[256];
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	hPlayerStats = GetButtonValue(MenuId, BUTNO_STATUS_HDL);

	// if not set, we have nothing to paint
	if(hPlayerStats == fERROR)
		return;
		
	/* find out which character we are -- shouldn't need to, but blood... */
	if (playerStatsInitted)
	{
	    for (regentNum=1; regentNum < CHARACTER_COUNT; regentNum++)
	        if (playerstats[regentNum] == hPlayerStats)
	            break;

        if (regentNum == CHARACTER_COUNT)
            regentNum = 0;
    }
	
	DumbHandlePtr<PLAYER_STATS const> const pPlayerStats(hPlayerStats);

	// paint the title
	if( GetButtonPosition( MenuId, 1, &mx, &my ) == fERROR)
		return;

	if(iDomainCharInfo != FALSE)
		sprintf(buffer,STRMGR_GetStr(STR_CHARACTER));
	else
		sprintf(buffer,STRMGR_GetStr(STR_STATUS));

	gprint_text(mx,my,buffer,WHITE);

	
	for(i=6;i<21;i++)
	{
		// get position of button
		if( GetButtonPosition( MenuId, i, &mx, &my ) == fERROR)
			return;
		
		if ((regentNum == 33 || regentNum == 51 || regentNum == 57) &&
			(i != 6 && i != 8 && i != 9))
			//sprintf(buffer, "^F02unknown");
			sprintf(buffer, STRMGR_GetStr(STR_UNKNOWN_CLASS));
		else switch(i)
		{
		case 6:
			if (pPlayerStats->mfGetClassType(1) >= 0)
			    sprintf(buffer,"^F02%s/%s", pPlayerStats->mfGetClass(0),
			        pPlayerStats->mfGetClass(1));
			else if (pPlayerStats->mfGetClassType(0) >= 0)
			    sprintf(buffer,"^F02%s", pPlayerStats->mfGetClass(0));
			else
				//sprintf(buffer, "^F02Monster");
				sprintf(buffer, STRMGR_GetStr(STR_MONSTER_RACE));
			break;
		case 7:
			if (pPlayerStats->mfGetClassType(1) >= 0)
			    sprintf(buffer,"^F02%d/%d", pPlayerStats->mfGetLevel(0),
			        pPlayerStats->mfGetLevel(1));
			else
			    sprintf(buffer,"^F02%d", pPlayerStats->mfGetLevel(0));
			break;
		case 8:
			if (regentNum != 0)
			    sprintf(buffer, "^F02%d/%s", regents[regentNum].mfGetBL_strength()+5*pPlayerStats->StatsMod.mfGetMod(STATSMOD::BLOODLINE_MOD),
			    	STRMGR_GetStr(gsBL_deriv[regents[regentNum].mfGetBL_deriv()]));
			else
			    sprintf(buffer,"^F02N/A");
			break;
		case 9:
			sprintf(buffer, "^F02%s", ALIGNMENT_INFO::mfGetName(pPlayerStats->mfGetAlignment()));
			break;
		case 10:
			{
			    SBYTE raw_st = pPlayerStats->mfGetStr();
			    if (raw_st > 30)
			    	raw_st = 30;
			    	
			    if (raw_st < 1)
				{
					sprintf(buffer, "^F02N/A");
					break;
				}
			
			    if (raw_st < 19)
			        sprintf(buffer,"^F02%d", raw_st);
				else if (mundane_descriptions)
					sprintf(buffer, "^F02%s", STRMGR_GetStr(STR_STRENGTH_DESCRIPTIONS_BASE + raw_st - 19));
			    else if (raw_st > 23)
			        sprintf(buffer,"^F02%d", raw_st - 5);
				else
					sprintf(buffer, "^F0218/%02d",
						strength_percentile[raw_st - 19]);
			}
			break;
		case 11:
			if (pPlayerStats->mfGetInt() < 1)
			{
				sprintf(buffer, "^F02N/A");
				break;
			}
			sprintf(buffer,"^F02%d", pPlayerStats->mfGetInt());
			break;
		case 12:
			if (pPlayerStats->mfGetDex() < 1)
			{
				sprintf(buffer, "^F02N/A");
				break;
			}
			sprintf(buffer,"^F02%d", pPlayerStats->mfGetDex());
			break;
		case 13:
			if (pPlayerStats->mfGetWis() < 1)
			{
				sprintf(buffer, "^F02N/A");
				break;
			}
			sprintf(buffer,"^F02%d", pPlayerStats->mfGetWis());
			break;
		case 14:
			if (pPlayerStats->mfGetCon() < 1)
			{
				sprintf(buffer, "^F02N/A");
				break;
			}
			sprintf(buffer,"^F02%d", pPlayerStats->mfGetCon());
			break;
		case 15:
			if (pPlayerStats->mfGetChr() < 1)
			{
				sprintf(buffer, "^F02N/A");
				break;
			}
			sprintf(buffer,"^F02%d", pPlayerStats->mfGetChr());
			break;
		case 16:
			if (mundane_descriptions)
				sprintf(buffer,"^F02%d%%", (20 - pPlayerStats->mfGetTHAC0()) * 5);
			else
				sprintf(buffer,"^F02%d", (SHORT)pPlayerStats->mfGetTHAC0());
			break;
		case 17:
			sprintf(buffer,"^F02%d", pPlayerStats->mfGetExp(0) + pPlayerStats->mfGetExp(1));
			break;
		case 18:
			if (mundane_descriptions)
				sprintf(buffer,"^F02%d%%", (10 - pPlayerStats->mfGetArmorClass()) * 5);
			else
				sprintf(buffer,"^F02%d", (SHORT)pPlayerStats->mfGetArmorClass());
			break;
		case 19:
			sprintf(buffer,"^F02%d", pPlayerStats->mfGetMaxHitPoints());
			break;
		case 20:
			sprintf(buffer,"^F02%d", pPlayerStats->mfGetCurHitPoints());
			break;
		}
	
		X = mx + TabStop - gtext_width(buffer);
		Y = my;
		gprint_text(X,Y,buffer,WHITE);
	}		
	
	// print name
	if( GetButtonPosition( MenuId, 5, &mx, &my ) == fERROR)
		return;
	X = mx;
	Y = my;
	if( GetButtonSize( MenuId, 5, &mx, &my ) == fERROR)
		return;
	X += mx/2;
	Y += my/2;
	sprintf(buffer,"^F06%s",pPlayerStats->mfGetName());
	print_text_centered(X,Y,buffer,WHITE);
	
	// print description
	if( GetButtonPosition( MenuId, 22, &mx, &my ) == fERROR)
		return;

	// print description here
	//gprint_text(mx,my,"^F02Armor: ", 173);
	gprint_text(mx,my,STRMGR_GetStr(STR_ARMOR_WORN), 173);

	my += 12;
	sprintf(buffer,"^F02       %s",ARMOR_INFO::mfGetName(pPlayerStats->mfGetCurrentArmor()) );
	gprint_text(mx,my,buffer,WHITE);
	my += 12;
	//gprint_text(mx,my,"^F02Weapon: ", 173);
	gprint_text(mx,my,STRMGR_GetStr(STR_WEAPON_CARRIED), 173);
	my += 12;
	sprintf(buffer,"^F02       %s",WEAPON_INFO::mfGetName(pPlayerStats->mfGetFirstWeapon()) );
	gprint_text(mx,my,buffer,WHITE);
	my += 12;
	//gprint_text(mx,my,"^F02Gold: ", 173);
	gprint_text(mx,my,STRMGR_GetStr(STR_GOLD_CARRIED), 173);
	sprintf(buffer,"^F02^l100%d", pPlayerStats->mfGetGold());
	gprint_text(mx,my,buffer,WHITE);

	// print inventory/rule button
	if( GetButtonPosition( MenuId, 3, &mx, &my ) == fERROR)
		return;

//WRC  The Begin Rule button will never appear in the status window again
//It's been moved....  Ken Williams strikes again!
//	if(iDomainCharInfo != FALSE)
//	{
//		// if this is a multi-player game, and the realm is already
//		// selected, dim the rule button and kill the function pointer
//		// to the button.
//#ifdef _WINDOWS
//		if(IsMultiPlayer() && lRealmSelectedByPlayer[HomeRealm] != -1)
//		{
//			SetButtonProc ( D_BEGIN_RULE, (LONG) 3, NULL, (LONG) 0, 0);
//			sprintf(buffer,"^T%s",STRMGR_GetStr(STR_RULE));
// 			gprint_text(mx+4,my+4,buffer,128);	// DKBROWN
//		}
//		else
//		{
//#endif
//			SetButtonProc ( D_BEGIN_RULE, (LONG) 3, BeginRule, (LONG) 1, KEY_B);
//			sprintf(buffer,STRMGR_GetStr(STR_RULE));
//			gprint_text(mx+4,my+4,buffer,173);
//#ifdef _WINDOWS
//		}
//#endif
//	}
//	else
	{
		sprintf(buffer,STRMGR_GetStr(STR_INV));
		if(!AttribFlag)
			gprint_text(mx+6,my+8,buffer,BTN_LABEL_COLOR);
		else
			gprint_text(mx+6,my+8,buffer,16);

	}

	// show art
	GetButtonPosition( MenuId, 21, &mx, &my );
	timedif = get_time() + STATUS_FRAMES_PER_SEC;
	
	ctrl = LOOP_FLAG|START_FLAG;

	run_timers();
	hBitm = NextAnimFrame(iAnim, anim.seq, 0, &ctrl, -1);
	// GWP Doesn't work.
	// GWP if(hBitm == fERROR)
	// GWP {
	// GWP 	char buffer[128];
	// GWP 	// Try for the PCX.
	// GWP 	sprintf(buffer, "things\\pcx\\%s.PCX", GAME_TTYPE::mfGetArtFileName(pPlayerStats->mfGetType()));
	// GWP 	hBitm = GetResourceRot(buffer);
	// GWP 	if (hBitm != fERROR)
	// GWP 		SetPurge(hBitm);
	// GWP }
	
	if(hBitm != fERROR)
	{
		SetRemapTable((USHORT)Realm);
		ScaleBitmap((SHORT)(mx+(152/2)), (SHORT)my, hBitm, 0, 0, (SHORT)152, (SHORT)(-204), FULL_SCALE*2/((regentNum == 33)?3:4));
		ClearRemapTable();
	}
	if(anim.seq != STANDSEQ && CheckSequence(iAnim))
	{
		MarkOldAnimPurgable(iAnim, anim.seq);
		
#if !defined(_JUNEDEMO)
		if(anim.seq == ATTACK1SEQ)
			anim.seq = ATTACK2SEQ;
		else
		if (anim.seq == ATTACK2SEQ)
			anim.seq = DEFENDSEQ;
		else
		if (anim.seq == DEFENDSEQ)
			anim.seq = BOWSEQ;
		else
#endif // _JUNEDEMO
			anim.seq = STANDSEQ;
			
		// This is done so we'll load all the sequences even though
		// we won't play them on the adventures.
		LONG const PrevLowMemory = fLowMemory;
		fLowMemory = FALSE;
		#if defined(_JUNEDEMO)
		load_FLC_sequence(iAnim,
						  GAME_TTYPE::mfGetLowResType(pPlayerStats->mfGetType()),
						  anim.seq,
						  0	// rotation Zero.
						  );
		#else
		load_FLC_sequence(iAnim,
						  pPlayerStats->mfGetType(),
						  anim.seq,
						  0	// rotation Zero.
						  );
		#endif
		fLowMemory = PrevLowMemory;
		
		SetAnimSequence(iAnim, (anim.seq * NUMROTATIONS));
	}
	
	while(get_time() < timedif)
	{
		run_timers();
	}

	//hBitm = GetResourceStd (buffer, FALSE);
	//if (hBitm != fERROR)
	//{
	//	DrawBitmap ((SHORT)(mx), (SHORT)(my), hBitm, 0, 0, (SHORT)152, (SHORT)204);
	//	SetPurge(hBitm);
	//}
}

/* ========================================================================
   Function    - PlayAnimation
   Description - play attack and fight animation
   Returns     -
   ======================================================================== */
void PlayAnimation (LONG MenuCombo, LONG)
{
	LONG	MenuId;
	LONG	ButtonId;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	SetButtonHilight(MenuId, 2, TRUE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	SetButtonHilight(MenuId, 2, FALSE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	
	LONG const hPlayerStats = GetButtonValue(MenuId, BUTNO_STATUS_HDL);
	DumbHandlePtr<PLAYER_STATS const> const pPlayerStats(hPlayerStats);
	
	anim.seq = ATTACK1SEQ;
	#if defined(_JUNEDEMO)
	load_FLC_sequence(iAnim,
					  GAME_TTYPE::mfGetLowResType(pPlayerStats->mfGetType()),
					  anim.seq,
					  0	// rotation Zero.
					  );
	#else
	load_FLC_sequence(iAnim,
					  pPlayerStats->mfGetType(),
					  anim.seq,
					  0	// rotation Zero.
					  );
	#endif
	SetAnimSequence(iAnim, (anim.seq * NUMROTATIONS));
}


/* ========================================================================
   Function    - GAMEToggleAnimation
   Description - Set the whole dang game to stop moving.
   Returns     - void
   ======================================================================== */
void GAMEToggleAnimation(LONG, LONG)
{
	fAIAnimate =  (fAIAnimate == TRUE) ? FALSE : TRUE;
}

void BackToGame(LONG, LONG)
{

   if ( mouse_button == 2 )
   {
   	SystemHelp( STR_MAIN1_HELP_RETURN_TITLE,
                   STR_MAIN1_HELP_RETURN_TEXT, H_Return_to_Game, 0, 0 );

   }
   else //---- We need help
   {
      if ( dturn_mode == CHARSEL_MODE ||
	        char_selected ||
		    master_game_type == GAME_ADVENTURE ||
   		master_game_type == GAME_BATTLE )
      {
            GAMEToggleMainMenu(FALSE, 0);
      }

   }


}

/* ========================================================================
   Function    - GAMEToggleMainMenu
   Description - Toggle the menu button on or off
   Returns     - void
   ======================================================================== */
void GAMEToggleMainMenu(LONG fState, LONG)
{
	LONG mx, my;
	extern LONG margin_bottom;				// [d10-28-96 JPC]

	if(fState == fMainMenuUp || fVeryFirstTime) return;

	if((BOOL)fState == TRUE)
	{
		BOOL	fDisplay = FALSE;
		LONG	color;
		
		// we don't want people to be able to bring up the main menu
		// during character selection mode.
		if (dturn_mode == CHARSEL_MODE)
			return;
			
		// We need to be in high res.
		// If rendering in low-res, we need to re-render in high res to
		// keep from seeing the small render screen in the upper left.
		if (!fHighRes)
		{
			set_hires (0, 0);
			if (fRender)
				render_view (FALSE);
		}
		
		GetButtonPosition(D_GAMEBUTTON, 0, &mx, &my);
		zoomStart.x = mx + 16;
		zoomStart.y = my + 16;
		
		// If we were called from an 'unfortunate place', delete all
		// the "regions that get restored each frame" that we can think of
		
		del_region(SelectRegent, 0);
		del_region(SelectARealm, 0);
		del_region(EndAdvParty,0);
		del_region(SelectSite,0);
		del_region(ToggleShowParty,0);
		del_region(ShowPartyStatus,0);
		
		fMainMenuUp = TRUE;
		fFreezeDomainUI = TRUE;
		
		SetButtonFlag(D_GAMEBUTTON, 0, D_INVISIBLE);
	
		if ( dturn_mode == CHARSEL_MODE ||
			master_game_type == GAME_ADVENTURE ||
			master_game_type == GAME_BATTLE )
			fDisplay = TRUE;
		
		// -- color the return to game button
		if (fDisplay || char_selected)
		{
			color = BTN_LABEL_COLOR;
		}
		else
		{
			color = 16;
		}
		SetButtonLabelColor(D_MAINMENU, 1, color);
		
		// -- color the new game button
		if (fDisplay || fDomainTurn)
		{
			color = BTN_LABEL_COLOR;
		}
		else
		{
			color = GREY;
		}
		SetButtonLabelColor(D_MAINMENU, 2, color);
		
		// -- color the load saved game button
#if !defined(_JUNEDEMO)	&& !defined(_MULTIPLAY_ONLY)	
		if (fDomainTurn)
		{
			color = BTN_LABEL_COLOR;
		}
		else
#endif
		{
			color = GREY;
		}
		SetButtonLabelColor(D_MAINMENU, 3, color);
		
		// -- color the save game button
#if !defined(_JUNEDEMO)
		if (char_selected && fDomainTurn && (master_game_type != GAME_INTRO)
//				&& (dturn_mode == MIN_MODE || dturn_mode == TAXLVL_MODE || dturn_mode == MAINT_MODE)
			)
		{
			color = BTN_LABEL_COLOR;
		}
		else
#endif
		{
			color = GREY;
		}
		SetButtonLabelColor(D_MAINMENU, 4, color);
		
		// -- color the multiplayer button
		if (fDomainTurn)
		{
			color = BTN_LABEL_COLOR;
		}
		else
		{
			color = GREY;
		}
		
#if !defined(_NETJUNEDEMO) && defined(_JUNEDEMO)
		SetButtonLabelColor(D_MAINMENU, 6, GREY);
#else
		#if defined(_WINDOWS)
		SetButtonLabelColor(D_MAINMENU, 6, color);
		#else
		SetButtonLabelColor(D_MAINMENU, 6, GREY);
		#endif
#endif
		ShowMenu(D_MAINMENU);
	}
	else
	{
		GetButtonPosition(D_GAMEBUTTON, 0, &mx, &my);
		zoomStart.x = mx + 16;
		zoomStart.y = my + 16;
		
		fMainMenuUp = FALSE;
		fFreezeDomainUI = FALSE;
		HideMenu(D_MAINMENU);
		RunMenus();
		ClearButtonFlag(D_GAMEBUTTON, 0, D_INVISIBLE);
	}
}

/* ========================================================================
   Function    - InitBattleCard_sub
   Description - init the data for the battle card
   Returns     -
   ======================================================================== */
void InitBattleCard_sub (LONG const ID )
{
	CHAR				buffer[80];

	
	sprintf(buffer,"UI\\PORTS_W\\wd%s.PCX", GAME_TTYPE::mfGetArtFileName(GetThingTypeFromID(ID)));
	
	run_timers();
	
	hTroopBitm = GetResourceStd(buffer, FALSE);
	
	ShowMenu(D_BTL_CARD);
}
	
/* ========================================================================
   Function    - InitBattleCard
   Description - init the data for the battle card
   Returns     -
   ======================================================================== */
void InitBattleCard( SHORT UnitIndex )
{
	iTroopType = units[UnitIndex].Icon;
	iTroopRealm = units[UnitIndex].Realm;

	InitBattleCard_sub(units[UnitIndex].id);
}

/* ========================================================================
   Function    - InitBattleCard_alt
   Description - init the data for the battle card
   Returns     -
   ======================================================================== */
void InitBattleCard_alt (LONG type, LONG realm)
{
	SHORT				hPlayerStats;

	iTroopType = type;
	iTroopRealm = realm;

	InitBattleCard_sub(UnitTypeID[type]);
}

/* ========================================================================
   Function    - PaintBattleCard
   Description - paint proc for the battle card menu
   Returns     -
   ======================================================================== */
void PaintBattleCard( LONG MenuCombo, LONG )
{
	SHORT	i;
	LONG	X,Y;
	LONG	mx,my;
	SHORT	tx,ty;
	LONG	MenuId, ButtonId;
	CHAR	buffer[150];
	LONG	timedif;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	// if not set, we have nothing to paint
	if(iTroopType == fERROR)
		return;
		
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	GetButtonPosition( MenuId, 0, &mx, &my );
	
	// paint the stats for this troop type
	
	/* Name     */
	init_gfont(FONT_SANS_16PT);
	sprintf( buffer, "%s", STRMGR_GetStr(gsUnitTitle[iTroopType]));
	gprint_text( mx+10, my+10, buffer, BLACK );
	
	init_gfont(FONT_SANS_12PT);
	tx = (SHORT)mx + 12;
	ty = (SHORT)my + 35;
	
	/* Move		*/
	sprintf( buffer, STRMGR_GetStr(BTLUI_UNITINFO_INFO_MOVE), GetBattleMove(iTroopType));
	gprint_text( tx, ty, buffer, BLACK );
	ty += 16;
	
	/* Defend	*/
	sprintf( buffer, STRMGR_GetStr(BTLUI_UNITINFO_INFO_DEFEND), GetBattleDefense(iTroopType));
	gprint_text( tx, ty, buffer, BLACK );
	ty += 16;
	
	/* Morale   */
	sprintf( buffer, STRMGR_GetStr(BTLUI_UNITINFO_INFO_MORALE), GetBattleMorale(iTroopType));
	gprint_text( tx, ty, buffer, BLACK );
	ty += 16;
	
	tx = (SHORT)mx + 118;
	ty = (SHORT)my + 35;
	
	/* Melee	*/
	sprintf( buffer, STRMGR_GetStr(BTLUI_UNITINFO_INFO_MELEE), GetBattleMelee(iTroopType));
	gprint_text( tx, ty, buffer, BLACK );
	ty += 16;
	
	/* Charge	*/
	sprintf( buffer, STRMGR_GetStr(BTLUI_UNITINFO_INFO_CHARGE), GetBattleCharge(iTroopType));
	gprint_text( tx, ty, buffer, BLACK );
	ty += 16;
	
	/* Missile	*/
	sprintf( buffer, STRMGR_GetStr(BTLUI_UNITINFO_INFO_MISSILE), GetBattleMissile(iTroopType));
	gprint_text( tx, ty, buffer, BLACK );

	/* Bonuses */

	sprintf( buffer, "^W200%s", STRMGR_GetStr(GetBattleBonus(iTroopType)));
	gprint_text( mx+12, my+100, buffer, BLACK );
	
	// paint picture
	if(hTroopBitm != fERROR)
	{
		SetRemapTable(iTroopRealm);
		DrawBitmap(
			(SHORT)(mx+144),(SHORT)(my + 4),
			hTroopBitm, 0, 0, 172, 176);
		ClearRemapTable();
	}
}

/* ========================================================================
   Function    - BattleCardProc
   Description - close the battle card
   Returns     -
   ======================================================================== */
void BattleCardProc( LONG, LONG )
{
	if ( hTroopBitm > 0)
		SetPurge(hTroopBitm);
	HideMenu(D_BTL_CARD);
	RunMenus();
}

/* ========================================================================
   Function    - ShowChatWindow
   Description - Show the chat window and collect input
   Returns     -
   ======================================================================== */
void ShowChatWindow(LONG, LONG)
{
	clear_key_status(0);
	iChatKey = 0;

//	ShowMenu(D_CHAT_INPUT);

   ShowChatSelectWindow(0,0);

	// -- if at the map level, scoot the chat box over to the edge of
	// -- the sliding left hand banner
	if (fDomainTurn)
   	SetChatPosition ( l_map );
	else
		SetChatPosition ( 0 );
}

/* ========================================================================
   Function    - isSpecial
	Description - check for unprintable keys from our fonts
	Returns     - 
	======================================================================== */
int isSpecial ( int key )
{
	// if key is one of the special non-printable keys,
	// return (int)TRUE
	if ( ( key >= 32 && key <= 90 ) 
		|| key == 95        
		|| ( key >=  97 && key <= 122 ) 
		|| ( key >= 128 && key <= 133 ) 
		|| ( key >= 135 && key <= 136 ) 
		|| key == 138        
		|| key == 141        
		|| key == 142        
		|| key == 144        
		|| key == 153        
		|| key == 154        
		|| key == 225        
		|| ( key >= 148 && key <= 151 ) 
		|| ( key >= 160 && key <= 163 ) )
	{
		return 1;
	}
	else
		return 0;
}

void InputHideMenu( void )
{
  	HideMenu(D_CHAT_SELECT);
  	RunMenus();

//  	HidePanel(D_CHAT_SELECT);
//  	RunPanels();

}

/* ========================================================================
   Function    - PaintInputField
   Description - type - 0 normal 
                 type - 1 special
                 type - 2 range 
                 type - 3 file 

   Returns     - 0 - still inputing 1 - done with string 2 - done with cheat code
   ======================================================================== */

int PaintInputField(LONG lButtonId, char *szBuffer, LONG lBufLen, LONG lFont,
    BOOL allowSpace, LONG type, char first, char last )
{
	static int frame = 0;
	LONG	mx,my;
	int 	count;
	SHORT	hPlayerStats;
	
	GetButtonPosition( lButtonId, 0, &mx, &my );
	
	mx += 10;
	my += 10;

	// get the cursor position
	count = strlen(szBuffer);
	if(count) count--;

	// blink the cursor
	frame++;
	if(frame < 5)
		szBuffer[count] = '_';
	else
		szBuffer[count] = ' ';

	if(frame >= 10) frame = 0;

	// clean off unwanted bits
	iChatKey &= 0x000000FF;
	
	if(iChatKey)
	{
   	fAnyKeyChanged = FALSE;
	    clear_key_status(0);

		if((char)iChatKey == 8)
		{
			// backspace key
			if(count)
			{
				szBuffer[count] = '\0';
				count--;
			}
		}
		else if((char)iChatKey == 13)
		{
			// got a return, terminate the string and return done condition
			szBuffer[count] = '\0';
			
			//GEH check for 'CHEAT KEYS'
			
			if(    szBuffer[0] == 'S'
				&& szBuffer[1] == 'Y'
				&& szBuffer[2] == 'N'
				)
			{
				// no cheats in multiplayer mode
				#ifdef _WINDOWS
				if(IsMultiPlayer())
				{
					iChatKey = 0;
					frame = 0;
					return 1;
				}
				#endif
					
				// SYNINV code to add items to your inventory
				if(	   szBuffer[3] == 'I'
					&& szBuffer[4] == 'N'
					&& szBuffer[5] == 'V'
					)
				{
					// only if inventory menu is up
					if(IsMenuActive(D_AVATAR_INVENTORY) && hPlayerStats!=fERROR)
					{
						LONG	Item,Quantity;
						LONG	FieldsRead=1;
						
						hPlayerStats = GetButtonValue(D_AVATAR_INVENTORY, INV_BUT_STATUS);

						FieldsRead=sscanf(&szBuffer[6],"%ld %ld", &Item, &Quantity);
			
						if (FieldsRead>0)
						{
							if (FieldsRead==1)
								Quantity=1;
							

							fQuietItems=TRUE;
							
							AddItemStatus ais=((PLAYER_STATS*)BLKPTR(hPlayerStats))->Inventory.mfAddItem((ITEMTYPE)Item,Quantity, 0, FALSE);
										
							if (ais&ADD_OK)
								AddSndObj(SND_PICKUP_ITEM1,0,VOLUME_FULL);
							else
								AddSndObj(SND_UI_INVALID_BUTTON_CLICK,0,VOLUME_FULL);
							fQuietItems=FALSE;
						}
					}
					InputHideMenu();
					return 2;
				}
				else
				// SYNMEM Flag Bool code to modify system memory flags
				if(	   szBuffer[3] == 'M'
					&& szBuffer[4] == 'E'
					&& szBuffer[5] == 'M'
					)
				{
					LONG	FlagNum;
					BOOL	FlagVal=TRUE;
					
					LONG FieldsRead=sscanf(&szBuffer[6],"%ld %ld", &FlagNum, &FlagVal);
					
					if (FieldsRead==2)
					{	
				   		switch(FlagNum)
				   		{
				   			case 0: fLowMemory      = FlagVal;	break;
				   			case 1: fMinMemory      = FlagVal;	break;
				   			case 2:	fSmallMap       = FlagVal;	break;
				   			case 3:	fMedResTextures = FlagVal;	break;
				   			case 4:	fMedResAni      = FlagVal;	break;
				   			case 5:	fLowResTextures = FlagVal;	break;
				   			case 6:	fLowResAni      = FlagVal;	break;
				   			case 7:	fRestrictAni    = FlagVal;	break;
				   		}
					}
					InputHideMenu();
					return 2;
				}
				else
				// SYNHEAL 0 for kill and 1 for full heal
				if(    szBuffer[3] == 'H'
					&& szBuffer[4] == 'E'
					&& szBuffer[5] == 'A'
					&& szBuffer[6] == 'L'
					)
				{
					LONG	Item;
					
					// only if inventory menu is up
					if(IsMenuActive(D_AVATAR_INVENTORY))
					{
						hPlayerStats = GetButtonValue(D_AVATAR_INVENTORY, INV_BUT_STATUS);

						LONG FieldsRead=sscanf(&szBuffer[7],"%ld", &Item);
	
						if (FieldsRead>0)
						{
							if( Item )
							{
								((PLAYER_STATS*)BLKPTR(hPlayerStats))->mfRestoreFullHitPts();
								AddSndObj(SND_BATTLE_HORN1,0,VOLUME_FULL);
							}
							else
							{
								((PLAYER_STATS*)BLKPTR(hPlayerStats))->mfKillem();
								AddSndObj(SND_BODY_FALLS_DEAD1,0,VOLUME_FULL);
							}
						}
					}
					InputHideMenu();
					return 2;
				}
				else
				// SYNSCN scnname
				if(    szBuffer[3] == 'S'
					&& szBuffer[4] == 'C'
					&& szBuffer[5] == 'N'
					)
				{
					CHAR ScnName[80];
					
					if(fDomainTurn)
					{
						LONG FieldsRead=sscanf(&szBuffer[6],"%s", ScnName);
						if (FieldsRead==1)
						{
							SCENE_MGR::Aggressor = REALM::NO_COUNTRY;
							if (SCENE_MGR::HomeIndex == -1)
								SCENE_MGR::HomeIndex = 245;
							SCENE_MGR::SceneType = SCENE_AI::ADVENTURE_SCENE;
							SCENE_MGR::mfRequestNewScene( ScnName, TYPE_PLAYERSTART1, TRUE);
							TurnOffAllSounds();
							InitDomainTurn(0);
									
							AddSndObj(SND_OWL1,0,VOLUME_FULL);
						}
					}
					InputHideMenu();
					return 2;
				}
				else
				// SYNWTW 0 for off and 1 for on
				if(    szBuffer[3] == 'W'
					&& szBuffer[4] == 'T'
					&& szBuffer[5] == 'W'
					)
				{
					LONG	Item;
					
					LONG FieldsRead=sscanf(&szBuffer[6],"%ld", &Item);
					if (FieldsRead)
					{
						if( Item )
						{
							player.WalkThruWall = TRUE;
							AddSndObj(SND_STONE_DOOR1,0,VOLUME_FULL);
						}
						else
						{
							player.WalkThruWall = FALSE;
							AddSndObj(SND_THUMP1,0,VOLUME_FULL);
						}
					}
					InputHideMenu();
					return 2;
				}
				else
				// SYNFLY 0 for off and 1 for on
				if(    szBuffer[3] == 'F'
					&& szBuffer[4] == 'L'
					&& szBuffer[5] == 'Y'
					)
				{
					LONG	Item;
					
					LONG FieldsRead=sscanf(&szBuffer[6],"%ld", &Item);
					if (FieldsRead)
					{
						if( Item )
						{
							player.Flying = TRUE;
							AddSndObj(SND_WYVERN_WINGFLAP1,0,VOLUME_FULL);
						}
						else
						{
							player.Flying = FALSE;
							AddSndObj(SND_WYVERN_SCREAM1,0,VOLUME_FULL);
						}
					}
					InputHideMenu();
					return 2;
				}
				else
				// SYNSET a b where a is which STATSMOD_SET and b is how much
				// note that the '_' is the minus
				if(    szBuffer[3] == 'S'
					&& szBuffer[4] == 'E'
					&& szBuffer[5] == 'T'
					)
				{
					// only if inventory menu is up
					if(IsMenuActive(D_AVATAR_INVENTORY))
					{
						LONG	Item;
						LONG	Stat;
						
						CHAR *Dash;
						BOOL Minus = FALSE;
						if((Dash = strchr(&szBuffer[6], '_')) != NULL)
						{
							Dash[0] = ' ';
							Minus = TRUE;
						}
						LONG FieldsRead=sscanf(&szBuffer[6],"%ld %ld", &Stat, &Item);
						if (FieldsRead==2)
						{	
							if(Minus)
								Item *= -1;
								
							hPlayerStats = GetButtonValue(D_AVATAR_INVENTORY, INV_BUT_STATUS);
							((PLAYER_STATS*)BLKPTR(hPlayerStats))->StatsMod.mfSetStats((STATSMOD::SET_TYPE)Stat, Item);
							AddSndObj(SND_CROW1,0,VOLUME_FULL);
						}
					}
					InputHideMenu();
					return 2;
				}
				else
				// SYNMOD a b where a is which STATSMOD_MOD and b is how much
				// note that the '_' is the minus
				if(    szBuffer[3] == 'M'
					&& szBuffer[4] == 'O'
					&& szBuffer[5] == 'D'
					)
				{
					// only if inventory menu is up
					if(IsMenuActive(D_AVATAR_INVENTORY))
					{
						LONG	Item;
						LONG	Stat;
						
						CHAR *Dash;
						BOOL Minus = FALSE;
						if((Dash = strchr(&szBuffer[6], '_')) != NULL)
						{
							Dash[0] = ' ';
							Minus = TRUE;
						}
						LONG FieldsRead=sscanf(&szBuffer[6],"%ld %ld", &Stat, &Item);
						if (FieldsRead==2)
						{	
							if(Minus)
								Item *= -1;
								
							hPlayerStats = GetButtonValue(D_AVATAR_INVENTORY, INV_BUT_STATUS);
							((PLAYER_STATS*)BLKPTR(hPlayerStats))->StatsMod.mfModifyStats((STATSMOD::MOD_TYPE)Stat, Item);
							AddSndObj(SND_BEAST_GROWL1,0,VOLUME_FULL);
						}
					}
					InputHideMenu();
					return 2;
				}
				else
				// SYNBTL 0 for hometeam and 1 for awayteam
				if(    szBuffer[3] == 'B'
					&& szBuffer[4] == 'T'
					&& szBuffer[5] == 'L'
					)
				{
					LONG	Item;
					
					if(fDomainTurn)
					{
						LONG FieldsRead=sscanf(&szBuffer[6],"%ld", &Item);
						if (FieldsRead==1)
						{
							if( Item )
							{
								// Away Team
								GoToBattleScene(0, 1);
								AddSndObj(SND_MALE_GROUP_DIE1, 0, VOLUME_NINETY);
							}
							else
							{
								// home team
								GoToBattleScene(0, 0);
								AddSndObj(SND_BATTLE_CHARGE1, 0, VOLUME_NINETY);
							}
						}
					}
					InputHideMenu();
					return 2;
				}
				
				else
				//SYNMAP FULL
				if(    szBuffer[3] == 'M'
					&& szBuffer[4] == 'A'
					&& szBuffer[5] == 'P'
					)
				{
					if(!fDomainTurn && !fBattleCombat)
					{
						if (szBuffer[7]=='F')
						{
							SetMapShowAll(0,TOGGLE);
						}
					}	
					InputHideMenu();
					return 2;
				}
				else
				if(    szBuffer[3] == 'S'
					&& szBuffer[4] == 'N'
					&& szBuffer[5] == 'D'
					)
				{
					LONG	Sound;
					
					LONG FieldsRead=sscanf(&szBuffer[6],"%ld", &Sound);
					if (FieldsRead==1)
						AddSndObj((BIRTHRT_SND)Sound, 0, VOLUME_FULL);
	
					InputHideMenu();
					return 2;
				}
				
				// SYNKILL kill the player
				else
				if(    szBuffer[3] == 'K'
					&& szBuffer[4] == 'I'
					&& szBuffer[5] == 'L'
					&& szBuffer[6] == 'L'
					)
				{
				
					// only if inventory menu is up
					if(IsMenuActive(D_AVATAR_INVENTORY))
					{
						AddSndObj(SND_RAZE1, 0, VOLUME_FULL);
						
						hPlayerStats = GetButtonValue(D_AVATAR_INVENTORY, INV_BUT_STATUS);
	
						if (fERROR != hPlayerStats)
						{
							DumbHandlePtr<PLAYER_STATS> pAvatarStats(hPlayerStats);
							if (pAvatarStats->hAvatar != fERROR)
							{
								DumbHandlePtr<CAvatar> 		pAvatar(pAvatarStats->hAvatar);
								
								pAvatar->mfKillAvatar(pAvatar);
							}
						}
					}
					InputHideMenu();
					return 2;
	
				}
	
				// SYNVER  display version
				else
				if(    szBuffer[3] == 'V'
					&& szBuffer[4] == 'E'
					&& szBuffer[5] == 'R'
					)
				{
					InputHideMenu();
				   	About(0,0);
					return 2;
				}
				else
				if(    szBuffer[3] == 'S'
					&& szBuffer[4] == 'I'
					&& szBuffer[5] == 'N'
					)
				{
					InputHideMenu();
				   	Sinners(0,0);
					return 2;
				}
				// SYNADV show all adventure sites
				else
				if(    szBuffer[3] == 'A'
					&& szBuffer[4] == 'D'
					&& szBuffer[5] == 'V'
					)
				{
					InputHideMenu();
					SetAllAdvSite();
					return 2;
				}
				
				// SYNAUTO 0/1 to set the auto run move for the domain turns
				else
				if(    szBuffer[3] == 'A'
					&& szBuffer[4] == 'U'
					&& szBuffer[5] == 'T'
					&& szBuffer[6] == 'O'
					)
				{
					SHORT	Item;
					
					if(fDomainTurn)
					{
						LONG FieldsRead=sscanf(&szBuffer[7],"%d", &Item);
						if (FieldsRead==1)
							fAutoMode = (BOOL)Item;
					}
					InputHideMenu();
					return 2;
				}
				
				// SYNANI to run the animation display panel
				else
				if(    szBuffer[3] == 'A'
					&& szBuffer[4] == 'N'
					&& szBuffer[5] == 'I'
					)
				{
					SHORT	Item;
					
					if(fDomainTurn)
					{
						ShowAllAnimations(0,0);
					}
					InputHideMenu();
					return 2;
				}

				//shows the Victory screen for the current scene
				else
				if(    szBuffer[3] == 'G'
					&& szBuffer[4] == 'Q'
					&& szBuffer[5] == 'I'
					)
				{
					InputHideMenu();
					VictoryScreen(PaintQuestItemFound,QuestItemFoundProc);
					return 2;
				}
				//activates the specified item
				else
				if(    szBuffer[3] == 'U'
					&& szBuffer[4] == 'S'
					&& szBuffer[5] == 'E'
					)
				{
					// only if inventory menu is up
					if(IsMenuActive(D_AVATAR_INVENTORY))
					{
						LONG	Item;
						
						hPlayerStats = GetButtonValue(D_AVATAR_INVENTORY, INV_BUT_STATUS);
						LONG FieldsRead=sscanf(&szBuffer[6],"%ld", &Item);
						
						if (FieldsRead==1)
						{
							if (fERROR != hPlayerStats)
							{
								LONG Combo=BUILD_LONG((SHORT)Item,hPlayerStats);
								InputHideMenu();
								ItemTypes[Item].mfEffect(Combo,ACTIVATE);
								return 2;
							}
						}
					}
					InputHideMenu();
					return 2;
				}
				// gives the player some money
				else
				if(    szBuffer[3] == 'C'
					&& szBuffer[4] == 'A'
					&& szBuffer[5] == 'S'
					&& szBuffer[6] == 'H'
					)
				{
					InputHideMenu();
				   	if (realm[HomeRealm].mfGetTreasury() < 50000)
				   	{
						AddSndObj(SND_PICKUP_ITEM1,0,VOLUME_FULL);
				   		realm[HomeRealm].mfAddTreasury(5000);
				   		new_treasury += 5000;
				   	}
					else
						AddSndObj(SND_UI_INVALID_BUTTON_CLICK,0,VOLUME_FULL);
					return 2;
				}

				// Plays the gorgon flics!
				if(    szBuffer[3] == 'T'
					&& szBuffer[4] == 'C'
					&& szBuffer[5] == 'S'
					)
				{
					LONG Item;
					InputHideMenu();
					
					
					LONG FieldsRead=sscanf(&szBuffer[6],"%ld", &Item);
					
					if(FieldsRead>0)
						PlayGorgon(Item,0);

					return 2;
				}
				#if defined(_DEBUG)
				if (	szBuffer[3] == 'W'
						&& szBuffer[4] == 'O'
						&& szBuffer[5] == 'N'
					)
				{
					fWonGame = TRUE;
					PlayFinale(0,0);
				}
				#endif

			}
			//else just end the string and return it to the
			// network message code
			else
			{
				iChatKey = 0;
				frame = 0;
				return 1;
			}
		}
		else 
		if ( type == 3 ) // filename entry
		{
			if ( isalnum( iChatKey ) )
			{
				int UpperChatKey = toupper( iChatKey );
				
				// we look at size of buffer - 2 for the blinking cursor
				// and the NULL terminator
				if(count < lBufLen - 2)
				{
					frame = 0;
					szBuffer[count++] = (char)UpperChatKey;
					szBuffer[count] = ' ';
				}
			}
		}
		else 
		if ( type == 1 ) // special 
		{
			if ( isSpecial ( iChatKey ) )
			{
				// we look at size of buffer - 2 for the blinking cursor
				// and the NULL terminator
				if(count < lBufLen - 2)
				{
					frame = 0;
					szBuffer[count++] = (char)iChatKey;
					szBuffer[count] = ' ';
				}
			}
		}
		else 
		if ( type == 0 )  // normal ( old ) 
		{
			if ( (isalnum(iChatKey) &&
			isupper(iChatKey )    )
			|| ((char)iChatKey == ' ' && allowSpace)
			|| (iChatKey == 0x000000BD)	// dash
			|| (iChatKey == 0x000000DC) // slash
			)
			{
				// we look at size of buffer - 2 for the blinking cursor
				// and the NULL terminator
				if(count < lBufLen - 2)
				{
					frame = 0;
					
					if(iChatKey == 0x000000BD)
						szBuffer[count++] = '_';
					else
					if(iChatKey == 0x000000DC)
						szBuffer[count++] = '\\';
					else
						szBuffer[count++] = (char)iChatKey;
					
					szBuffer[count] = ' ';
				}
				
			}
			
		}
		else   //---- range 
		{
			if ( (char)iChatKey >= first &&
			(char)iChatKey <= last     )
			{
				if(count < lBufLen - 2)
				{
					frame = 0;
					szBuffer[count++] = (char)iChatKey;
					szBuffer[count] = ' ';
				}
			}
		}

#if 0
		else if(isalnum(iChatKey)
				|| ((char)iChatKey == ' ' && allowSpace)
				|| (iChatKey == 0x000000BD)	// dash
				|| (iChatKey == 0x000000DC) // slash
			)
		{

			//---- Range of characters

			if ( range  )
			{

			    if ( (char)iChatKey >= first &&
					 (char)iChatKey <= last     )
				{
					// we look at size of buffer - 2 for the blinking cursor
					// and the NULL terminator
					if(count < lBufLen - 2)
					{
						frame = 0;
						if(iChatKey == 0x000000BD)
							szBuffer[count++] = '_';
						else
						if(iChatKey == 0x000000DC)
							szBuffer[count++] = '\\';
						else
							szBuffer[count++] = (char)iChatKey;
						szBuffer[count] = ' ';
					}

				}

			}
			else
			{
				// we look at size of buffer - 2 for the blinking cursor
				// and the NULL terminator
				if(count < lBufLen - 2)
				{
					frame = 0;
					if(iChatKey == 0x000000BD)
						szBuffer[count++] = '_';
					else
					if(iChatKey == 0x000000DC)
						szBuffer[count++] = '\\';
					else
						szBuffer[count++] = (char)iChatKey;
					szBuffer[count] = ' ';
				}
			}

		}
#endif



		iChatKey = 0;
	}
	
	if (lFont >= 0)
	{
	    init_gfont(lFont);
	    gprint_text(mx,my,szBuffer,WHITE);
	}

	return 0;

}

/* ========================================================================
   Function    - PaintChatInput
   Description - take input for the multi player chat
   Returns     -
   ======================================================================== */
void PaintChatInput(LONG, LONG)
{
	static CHAR buffer[40] = {0};
	int done;

	//---- Reset buffer

	done = PaintInputField( D_CHAT_INPUT, &buffer[0], 40, FONT_SANS_16PT, TRUE, TRUE, ' ', 'z' );

	if( done )
	{

#ifdef _WINDOWS
		if ( done == 1 )
			AMultiPlayer.Chat(buffer);
#endif

		// if done, send the string and clear the buffer
		memset(buffer, 0, sizeof(buffer));

		if ( done == 2 )
			cheated++;

// Don't auto hide anymore 
//		if ( done == 1 )
//		{
//			HideMenu(D_CHAT_INPUT);
//		   	RunMenus();
//		}

	}

  	//HideMenu(D_CHAT_INPUT);

}


/* ========================================================================
   Function    - ToggleChatDisplayWindow
   Description - toggles between small and large chat display windows 
   Returns     -
   ======================================================================== */
void ToggleChatDisplayWindow(LONG, LONG)
{
	
	if ( IsPanelUp ( D_CHAT_DISPLAY_WINDOW ) ||
	IsPanelUp ( D_CHAT_DISPLAY_WINDOW1 )   )
	
	{
		HideChatDisplayWindow(0,0);
		RunMenus();
		ChatDisplaySize = !ChatDisplaySize;
		
		// -- if at the map level, scoot the chat box over to the edge of
		// -- the sliding left hand banner
		if (fDomainTurn)
			SetChatPosition ( l_map );
		else
			SetChatPosition ( 0 );
		ShowChatDisplayWindow(0,0);
		RunMenus();
		SetRedrawMainMapLevel();
	}
	
	return;
	
}


/* ========================================================================
   Function    - SetChatPosition
   Description - Set our chat position 
   Returns     -
   ======================================================================== */
void SetChatPosition ( LONG l_map )
{

   if ( !ChatDisplaySize )
   {
       SetButtonPosition(D_CHAT_DISPLAY_WINDOW, 0, l_map, 0);
   }
   else
   {
   	SetButtonPosition(D_CHAT_DISPLAY_WINDOW1, 0, l_map, 0);
   }

}


/* ========================================================================
   Function    - ShowChatDisplayWindow
   Description - Show the chat display window
   Returns     -
   ======================================================================== */
void ShowChatDisplayWindow(LONG, LONG)
{

	fChatLineOn = TRUE;

   if ( !ChatDisplaySize )
   {
   	ShowPanel(D_CHAT_DISPLAY_WINDOW);
   }
   else
   {
   	ShowPanel(D_CHAT_DISPLAY_WINDOW1);
   }


}



/* ========================================================================
   Function    - HideChatDisplayWindow
   Description - Hide the chat display window
   Returns     -
   ======================================================================== */
void HideChatDisplayWindow(LONG, LONG)
{
	fChatLineOn = FALSE;

   if ( !ChatDisplaySize )
   {
   	HidePanel(D_CHAT_DISPLAY_WINDOW);
   }
   else
   {
   	HidePanel(D_CHAT_DISPLAY_WINDOW1);
   }

}

/* ========================================================================
   Function    - MoveChatDisplayWindow
   Description -
   Returns     -
   ======================================================================== */
void MoveChatDisplayWindow(LONG X, LONG Y)
{

   if ( !ChatDisplaySize )
   {
	    SetButtonPosition(D_CHAT_DISPLAY_WINDOW, 0, X, Y);
   }
   else
   {
   	SetButtonPosition(D_CHAT_DISPLAY_WINDOW1, 0, X, Y);
   }


//	SetButtonPosition(D_CHAT_DISPLAY_WINDOW, 0, X, Y);

}

/* ========================================================================
   Function    - ChatLineClear
   Description - clear out old data from chat
   Returns     -
   ======================================================================== */
void ChatLineClear()
{
	ChatLineHead = 0;
	memset(&ChatLine[0][0], 0, CHAT_MAXLINES * CHAT_CHARS);
}

/* ========================================================================
   Function    - ChatLineAdd
   Description - add a line of text to the chat screen
   Returns     -
   ======================================================================== */
void ChatLineAdd(CHAR *Text, SHORT Color)
{
	strncpy(&ChatLine[ChatLineHead][0], Text, CHAT_CHARS-1);
	ChatLineColor[ChatLineHead]=Color;
	ChatLineHead = (ChatLineHead + 1) % CHAT_MAXLINES ;
	fUpdatePanels = TRUE;
}

/* ========================================================================
   Function    - PaintChatDisplay
   Description - paint the contents of the chat list
   Returns     -
   ======================================================================== */
void PaintChatDisplay(LONG MenuCombo, LONG)
{
	LONG	MenuId, ButtonId;
	LONG	mx,my;
	LONG	i,c;

	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	// print the title
	//	GetButtonPosition( D_CHAT_DISPLAY_WINDOW, 0, &mx, &my );

	GetButtonPosition( (ChatDisplaySize ? D_CHAT_DISPLAY_WINDOW1 :  D_CHAT_DISPLAY_WINDOW),
		0, &mx, &my );
	
	mx += 5;
	my += 5;
	
	init_gfont(FONT_SANS_8PT);

	for(i=0;i < (  ChatDisplaySize ? CHAT_MAXLINES : CHAT_LINES );i++)
	{
		c = (ChatLineHead + i + (  ChatDisplaySize ? 0 : CHAT_LINES )) % CHAT_MAXLINES;
		gprint_text( mx, my, &ChatLine[c][0], ChatLineColor[c] );
		my += CHAT_SPACING;
	}

	// -- if at the map level, scoot the chat box over to the edge of
	// -- the sliding left hand banner
	if (fDomainTurn)
		SetChatPosition ( l_map );
	else
		SetChatPosition ( 0 );
}

void InventoryFromStatus(LONG MenuCombo, LONG)
{
	LONG	MenuId, ButtonId;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	//this is so we already have the art in memory
	//so their is no delay when the menus switch
	CSTRPTR pArt;
	GetButtonArt(D_AVATAR_INVENTORY,0,&pArt);
	SHORT DummyLoader=GetResourceStd(pArt,TRUE);

	LONG hPlayerStats = GetButtonValue(D_AVATAR_ATTRIBUTES, BUTNO_STATUS_HDL);

	ClearButtonFlag(D_AVATAR_ATTRIBUTES,0,D_ZOOM);
	ClearButtonFlag(D_AVATAR_INVENTORY,0,D_ZOOM);
	SetButtonFlag(D_AVATAR_ATTRIBUTES,0,D_SPIN);
	SetButtonFlag(D_AVATAR_INVENTORY,0,D_SPIN);
	SetButtonFlag(D_AVATAR_ATTRIBUTES,0,D_DONT_SHADE);
	SetButtonFlag(D_AVATAR_INVENTORY,0,D_DONT_SHADE);

	// click the button
	SetButtonHilight(MenuId, ButtonId, TRUE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	SetButtonHilight(MenuId, ButtonId, FALSE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	
	HideMenu(D_AVATAR_ATTRIBUTES);
	RunMenus();

	SetRedrawMainMapLevel();
	
	if (hPlayerStats != 0 && hPlayerStats != fERROR)
	{
		StaticInventoryGUI::mfDisplay(0, hPlayerStats);
	}
	
	RunMenus();

	ClearButtonFlag(D_AVATAR_ATTRIBUTES,0,D_DONT_SHADE);
	ClearButtonFlag(D_AVATAR_INVENTORY,0,D_DONT_SHADE);
 	ClearButtonFlag(D_AVATAR_ATTRIBUTES,0,D_SPIN);
	ClearButtonFlag(D_AVATAR_INVENTORY,0,D_SPIN);
	SetButtonFlag(D_AVATAR_ATTRIBUTES,0,D_ZOOM);
	SetButtonFlag(D_AVATAR_INVENTORY,0,D_ZOOM);
	
}

void StatusFromInventory(LONG MenuCombo, LONG)
{
	//this is so we already have the art in memory
	//so their is no delay when the menus switch
	CSTRPTR pArt;
	GetButtonArt(D_AVATAR_INVENTORY,0,&pArt);
	SHORT DummyLoader=GetResourceStd(pArt,TRUE);

	LONG hPlayerStats = GetButtonValue(D_AVATAR_INVENTORY, INV_BUT_STATUS);

	ClearButtonFlag(D_AVATAR_ATTRIBUTES,0,D_ZOOM);
	ClearButtonFlag(D_AVATAR_INVENTORY,0,D_ZOOM);
	SetButtonFlag(D_AVATAR_ATTRIBUTES,0,D_SPIN);
	SetButtonFlag(D_AVATAR_INVENTORY,0,D_SPIN);

	// click the button
	SetButtonHilight(D_AVATAR_INVENTORY, INV_BUT_STATUS, TRUE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	SetButtonHilight(D_AVATAR_INVENTORY, INV_BUT_STATUS, FALSE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	
	HideMenu(D_AVATAR_INVENTORY);
  	RunMenus();

	SetRedrawMainMapLevel();
	
	ShowStatus(0, hPlayerStats);
  	RunMenus();

 	ClearButtonFlag(D_AVATAR_ATTRIBUTES,0,D_SPIN);
	ClearButtonFlag(D_AVATAR_INVENTORY,0,D_SPIN);
	SetButtonFlag(D_AVATAR_ATTRIBUTES,0,D_ZOOM);
	SetButtonFlag(D_AVATAR_INVENTORY,0,D_ZOOM);

}

void DoneHelpMenu(LONG, LONG);
void UpdateHighlight(LONG MenuCombo, LONG val)
{
	SHORT	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	SetButtonHilight(MenuId, ButtonId, TRUE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	SetButtonHilight(MenuId, ButtonId, FALSE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	
	iHelpHighlight = val;
	DoneHelpMenu(0, 0);
}

void DoneHelpKeys(LONG arg, LONG arg1)
{
	HideSubMenuWithClick(0, D_HELPKEYS);
	RunMenus();

	if(arg1 == 0)
		ShowSubMenu(0, D_HELP);
}

void PaintHelpScreen(LONG MenuCombo, LONG)
{
	int i;
	LONG mx, my;
	LONG X, Y, W, H;
	char buffer[40];
	LONG	MenuId, ButtonId;

	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	// print the title
	GetButtonPosition( MenuId, 0, &mx, &my );
	init_gfont(FONT_TITL_40PT);
    sprintf(buffer, "^N%s", STRMGR_GetStr(STR_HELP_TITLE));
	print_textf(mx + 25 + 48, my + 23, 0, buffer);
}

/* ========================================================================
   Function    - VictoryScreen
   Description - put up the victory screen
   Returns     -
   ======================================================================== */
void VictoryScreen(PFVLL NewPaintProc, PFVLL NewDoneProc)
{
	SetButtonProc(D_VICTORY, 0, NewPaintProc, 0, NO_KEY);
	SetButtonProc(D_VICTORY, 1, NewDoneProc, 1, D_KEY_DONE1);
	KillSoundsNoFade();
	ResumeSuspendedMusic();
	PlayTrack(SND_WIN_MUSIC1);
	SuspendMusic();
	ucWhichTrack = REDBOOK_REALM;
	ShowMenu(D_VICTORY);
}

/* ========================================================================
   Function    - VictoryScreenProc
   Description -
   Returns     -
   ======================================================================== */
void VictoryScreenProc(LONG MenuCombo, LONG )
{
	LONG	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	SetButtonHilight(MenuId, ButtonId, TRUE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	SetButtonHilight(MenuId, ButtonId, FALSE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	HideMenu(MenuId);
	RunMenus();
	KillSoundsNoFade();
}

/* ========================================================================
   Function    - DefeatScreen
   Description - put up the Defeat screen
   Returns     -
   ======================================================================== */
void DefeatScreen(PFVLL NewPaintProc, PFVLL NewDoneProc)
{
	SetButtonProc(D_DEFEAT, 0, NewPaintProc, 0, NO_KEY);
	SetButtonProc(D_DEFEAT, 1, NewDoneProc, 1, D_KEY_DONE1);
	KillSoundsNoFade();
	ResumeSuspendedMusic();
	PlayTrack(SND_LOSE_MUSIC1);
	SuspendMusic();
	ucWhichTrack = REDBOOK_REALM;
	ShowMenu(D_DEFEAT);
}

/* ========================================================================
   Function    - DefeatScreenProc
   Description -
   Returns     -
   ======================================================================== */
void DefeatScreenProc(LONG MenuCombo, LONG )
{
	LONG	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	SetButtonHilight(MenuId, ButtonId, TRUE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	SetButtonHilight(MenuId, ButtonId, FALSE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	HideMenu(MenuId);
	RunMenus();
	KillSoundsNoFade();
}

/* ========================================================================
   Function    - Sinners
   Description - Vain attemped at humor
   Returns     - No return beyond this point
   ======================================================================== */
void Sinners(LONG, LONG)
{
	CHAR	filename[40];
	SHORT	i;
	SHORT	bitm;
	SHORT	hBitm;
	LONG	now;
	
	StopRedBook();
	TurnOffAllSounds();
	PlayTrack(SND_WIN_GAME_MUSIC1);
	
	SysHideCursor();
	
	hBitm = SaveBitmap(0, 0, 640, 480);
	
	bitm = GetResourceStd ("ui\\FILLPANL.PCX", FALSE);
  		
	if (bitm != fERROR)
	{
		POINT start,bck;
		
		start.x = 320;
		start.y = 240;
		bck.x = 320;
		bck.y = 240;
		ZoomBitmap (0, 0, bitm, 0, 0, 640, 480, fERROR, start, bck, 640, 480);
		update_screen();
		SetPurge(bitm);
	}
	else
	{
		goto End; // can't find this one, try the next
	}
	
	init_gfont(FONT_TITL_16PT);
	
   print_textf(50,25,31,"^W540 Special thanks to the Early Hours Crew."
   "  The place is a mess.  Candles, both ends burnt, lying everywhere."
   "  Coffee rings on every copy of the manual.  Sometimes we were having"
   " a great time, other times we were at each others throats."
   "  In the end we have produced one ^C125K^C000ick ^C125B^C000utt game!.");
	
	init_gfont(FONT_TITL_20PT);
	print_textf(320,162,28,"^c^NMike Branham");
	print_textf(320,190,28,"^c^NMona Branham");
	print_textf(320,218,28,"^c^NRobert Clardy");
	print_textf(320,246,28,"^c^NAlan Clark");
	print_textf(320,274,28,"^c^NJim Edwards");
	print_textf(320,302,28,"^c^NGreg Hightower");
	print_textf(320,330,28,"^c^NDavid Jackson");
	print_textf(320,358,28,"^c^NKirt Lemons");
	print_textf(320,386,28,"^c^NGary Powell");		
	print_textf(320,414,28,"^c^NDonald Tsang");
	print_textf(320,442,28,"^c^NCraig Utterback");
	
	init_gfont(FONT_TITL_8PT);
	print_textf(320,460,31,"^cPress any key or mouse button to continue.");
	
	SysShowCursor();
	
	update_screen();
	
	#ifdef _WINDOWS
	ClearMessageQueue();
	#endif
					
	fAnyKeyChanged = FALSE;
	clear_key_status(0);
	mouse_button = 0;

	while(TRUE)
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

End:
	DrawBitmap (0, 0, hBitm, 0, 0, 640, 480);
	DisposBlock(hBitm);

	SysShowCursor();
	fAnyKeyChanged = FALSE;
	clear_key_status(0);
	//add_timer(1,draw_cursor);
	return;	
}

/* ========================================================================
   Function    - ShowAllAnimations
   Description - Run the whole animation sequence menus.
   Returns     -
   ======================================================================== */

void ShowAllAnimations(LONG, LONG)
{
	if (fDomainTurn)
	{
		init_anim();
		
		ShowSubMenu(0, D_SHOW_ANIMATION);
	}
}

/* ========================================================================
   Function    - BowToPlayer
   Description - When the picture of the person is clicked on, bow and speak.
   Returns     -
   ======================================================================== */

void BowToPlayer(LONG MenuCombo, LONG)
{
	LONG	MenuId;
	LONG	ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	SHORT hPlayerStats = GetButtonValue(MenuId, BUTNO_STATUS_HDL);
	DumbHandlePtr<PLAYER_STATS const> const pPlayerStats(hPlayerStats);
	
	if(iAnim > 0 && anim.seq == STANDSEQ)
	{
		AddVoxSndType(VOX_CASTLE_INQUIRY, pPlayerStats->mfGetType());
		
#if !defined(_DEMO)
		MarkOldAnimPurgable(iAnim, anim.seq);
		anim.seq = BOWSEQ;
		load_FLC_sequence(iAnim,
						  pPlayerStats->mfGetType(),
						  anim.seq,
						  0	// rotation Zero.
						  );
		SetAnimSequence(iAnim, (anim.seq * NUMROTATIONS));
#endif // _DEMO
	}

}


/* ========================================================================
   Function    - GetMoreInfo
   Description - Called when more help is asked for
   Returns     -
   ======================================================================== */
void GetMoreInfo( LONG MenuCombo, LONG Context )
{
	CHAR chFileName[80];

#if defined(_JUNEDEMO)
	return;
#endif			

#ifdef _WINDOWS
	#ifdef _FOREIGNVER
		LONG    MenuId, ButtonId;
		SPLIT_LONG(MenuCombo, MenuId, ButtonId);
		HideSubMenuWithClick(0, MenuId );
		RunMenus();
		InitDOSHelp(H_TOPIC_RULES, Context);
		return;
	#endif

   //---- Put us in normal mode  ( well as normal as possible... )
   if (sDrawMode == iDDRAW)
       DDSetCooperative ( FALSE );

//	if (sDrawMode == iDDRAW)
//		ToggleDirectDraw(0, 0);

	if( '\0' != CDDrive[0] )
		sprintf(chFileName, "%s%s%s", CDDrive,InstallPath,  "GAMEPLAY.HLP");
	else
		sprintf(chFileName, "%s%s", InstallPath, "GAMEPLAY.HLP");
			
	WinHelp( hwndApp, chFileName, HELP_CONTEXT, Context );

#else

	LONG    MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	HideSubMenuWithClick(0, MenuId );
	RunMenus();

	InitDOSHelp(H_TOPIC_RULES, Context);

#endif

}

/* ========================================================================
   Function    - SystemHelp
   Description - system help dialog using a text file
                 NOTE: first line of text in file is the title, next line
                 on is the text body
   Returns     -
   ======================================================================== */
void SystemHelp(
	CHAR *TextFile,
	LONG MoreInfoTag,
	PFVLL PaintProc = NULL,
	LONG MenuControl = 0
)
{
	CHAR	Title[80];
	CHAR	Text[4096];
	SHORT	fileResult;
	FILE    *fp;

	// clear the buffer
	memset(&Title[0], 0, sizeof(Title));
	memset(&Text[0], 0, sizeof(Text));

	// read text in from file
	if (TextFile != 0)
	{
		fp = FileOpen(TextFile, "r");
		if (fp != NULL)
		{
			fileResult = GetNextLine(fp, Title, sizeof(Title));
			if (fileResult == EOF)
				return;
				
			fread(Text, sizeof(Text), 1, fp);
			FileClose(fp);
		}
	}
	
	SystemHelp(&Title[0], &Text[0], MoreInfoTag, PaintProc, MenuControl);
}
	
/* ========================================================================
   Function    - SystemHelp
   Description - system help dialog using a strmgr text index
   Returns     -
   ======================================================================== */
void SystemHelp(
	int TitleIndex,
	int TextIndex,
	LONG MoreInfoTag,
	PFVLL PaintProc = NULL,
	LONG MenuControl = 0
)
{
	CHAR	Title[80];
	CHAR	Text[4096];
	
	// clear the buffer
	memset(&Title[0], 0, sizeof(Title));
	memset(&Text[0], 0, sizeof(Text));

	if (TextIndex != -1)
		strncpy(&Title[0], STRMGR_GetStr(TitleIndex), sizeof(Title));
		
	if (TextIndex != -1)
		strncpy(&Text[0], STRMGR_GetStr(TextIndex), sizeof(Text));
	
	SystemHelp( &Title[0], &Text[0], MoreInfoTag, PaintProc, MenuControl);
}

/* ========================================================================
   Function    - SystemHelp
   Description - system help dialog using buffers
   Returns     -
   ======================================================================== */
void SystemHelp(
	CHAR *Title,
	CHAR *TextBody,
	LONG MoreInfoTag,
	PFVLL PaintProc = 0,
	LONG MenuControl = 0
)
{
	SHORT	DialogName;
	LONG	TextHeight;
	LONG	w,h, bw, bh;
	
	// clear the buffer
	memset(&SysHelpTitle[0], 0, sizeof(SysHelpTitle));
	memset(&SysHelpText[0], 0, sizeof(SysHelpText));

	strcpy(&SysHelpTitle[0],"^F04^C070");
	strcpy(&SysHelpTitle[9], &Title[0]);
	
	strcpy(&SysHelpText[0],"^F03^C001^W322");
	strcpy(&SysHelpText[14], &TextBody[0]);
	
	TextHeight = gtext_height(SysHelpText);
	
	if (TextHeight < 40)		// small box
	{
		DialogName = (SHORT)D_SYSHELP_SMALL;
	}
	else
	if (TextHeight < 115)		// med box
	{
		DialogName = (SHORT)D_SYSHELP_MED;
	}
	else 						// large box
	{
		DialogName = (SHORT)D_SYSHELP_LARGE;
		if (TextHeight > 300)
		{
			// NOTE: a bug in gtext_height causes the column
			// width breaks to be imbedded in the string you
			// are checking.  We must reload the string each
			// time we recheck the text height
			
			// reduce the font size
			strcpy(&SysHelpText[0],"^F02^C001^W322");
			strcpy(&SysHelpText[14], &TextBody[0]);
			if (gtext_height(SysHelpText) > 280)
			{
				// yet again, reduce the font size
				strcpy(&SysHelpText[0],"^F01^C001^W322");
				strcpy(&SysHelpText[14], &TextBody[0]);
			}
		}
	}
	
	if (MenuControl & SYSHELP_LARGE_BOX)
		DialogName = (SHORT)D_SYSHELP_LARGE;
	else
	if (MenuControl & SYSHELP_MED_BOX)
		DialogName = (SHORT)D_SYSHELP_MED;
	else
	if (MenuControl & SYSHELP_SMALL_BOX)
		DialogName = (SHORT)D_SYSHELP_SMALL;
	
	if (MoreInfoTag != -1)
	{
		// make done button visible
		ClearButtonFlag(DialogName, SYSHELP_DONE, D_INVISIBLE);
		activate_region(SYSHELP_DONE, TRUE);
		// move the done button left
		GetButtonSize(DialogName, SYSHELP_DIALOG, &w, &h);
		GetButtonSize(DialogName, SYSHELP_DONE, &bw, &bh);
		w = (w / 3) - (bw / 2);
		h = (h - bh) - 5;
		SetButtonPosition(DialogName, SYSHELP_DONE, w, h);
		
		// hook up moreinfo stuff and make button visible
		ClearButtonFlag(DialogName, SYSHELP_MOREINFO, D_INVISIBLE);
		activate_region(SYSHELP_MOREINFO, TRUE);
		SetButtonValue(DialogName, SYSHELP_MOREINFO, MoreInfoTag);
		SetButtonProc(DialogName, SYSHELP_MOREINFO, GetMoreInfo, MoreInfoTag, D_KEY_MORE );
#if defined(_JUNEDEMO)
		SetButtonLabelColor(DialogName, SYSHELP_MOREINFO, GREY);
#endif
		// move the moreinfo button right
		GetButtonSize(DialogName, SYSHELP_DIALOG, &w, &h);
		GetButtonSize(DialogName, SYSHELP_MOREINFO, &bw, &bh);
		w = (2*(w / 3)) - (bw / 2);
		h = (h - bh) - 5;
		SetButtonPosition(DialogName, SYSHELP_MOREINFO, w, h);
	}
	else
	{
		// make done button visible
		ClearButtonFlag(DialogName, SYSHELP_DONE, D_INVISIBLE);
		activate_region(SYSHELP_DONE, TRUE);
		// center the done button
		GetButtonSize(DialogName, SYSHELP_DIALOG, &w, &h);
		GetButtonSize(DialogName, SYSHELP_DONE, &bw, &bh);
		w = (w / 2) - (bw / 2);
		h = (h - bh) - 5;
		SetButtonPosition(DialogName, SYSHELP_DONE, w, h);
		
		// more info button disabled and invisible
		SetButtonFlag(DialogName, SYSHELP_MOREINFO, D_INVISIBLE);
		SetButtonValue(DialogName, SYSHELP_MOREINFO, -1);
		activate_region(SYSHELP_MOREINFO, FALSE);
	}
	
	if (PaintProc != 0)
		SysHelpProc = PaintProc;
	
	ShowMenu(DialogName);
	
	//GEH clear the mouse click that got us here
	mouse_button = 0;
	mouse_click = 0;
}

/* ========================================================================
   Function    - SystemHelpPaintProc
   Description - paint proc for system help dialog
   Returns     -
   ======================================================================== */
void SystemHelpPaintProc( LONG MenuCombo, LONG )
{
	BOOL	fTitle = FALSE;
	LONG	X,Y,W,H;
	LONG	Y2;
	LONG    MenuId, ButtonId;
	LONG	TextHeight;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	if (SysHelpTitle[0] != 0)
	{
		fTitle = TRUE;
		
		GetButtonPosition( MenuId, SYSHELP_TITLE, &X, &Y );
		GetButtonSize( MenuId, SYSHELP_TITLE, &W, &H );
		X += W / 2;
		Y += H / 2;
		Y2 = Y;			// may be use below
		print_text_centered(X, Y, SysHelpTitle, WHITE);
	}

	if (SysHelpText[0] != 0)
	{
		GetButtonPosition( MenuId, SYSHELP_TEXTBODY, &X, &Y );
		GetButtonSize( MenuId, SYSHELP_TEXTBODY, &W, &H );
		
		// if not the large box, center the text
		if (MenuId != D_SYSHELP_LARGE)
		{
			TextHeight = gtext_height(SysHelpText);
			Y += (H - TextHeight) / 2;
		}
		
		gprint_text(X, Y, SysHelpText, BLACK);
	}
	
	if (SysHelpProc != 0)
		(*SysHelpProc)(MenuCombo, 0);
}

/* ========================================================================
   Function    - PaintHello
   Description -
   Returns     -
   ======================================================================== */
void PaintHello( LONG MenuCombo, LONG )
{
	LONG	X,Y,W,H;
	LONG    MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	if( GetButtonPosition( MenuId, SYSHELP_TITLE, &X, &Y ) == fERROR)
		return;
		
	if( GetButtonSize( MenuId, SYSHELP_TITLE, &W, &H ) == fERROR)
		return;
		
	gprint_text(X,Y,"^F01Hello World!", BLACK);
}

/* ========================================================================
   Function    - LoadingSceneProc
   Description -
   Returns     -
   ======================================================================== */
#define SWORD_X	44
#define SWORD_Y	23
#define SWORD_W	265
#define SWORD_H	73
void LoadingSceneProc( LONG MenuCombo, LONG )
{
	LONG	X,Y,W,H;
	LONG    MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	SHORT	bitm;
	
	if( GetButtonPosition( MenuId, 0, &X, &Y ) == fERROR)
		return;
		
	if( GetButtonSize( MenuId, 0, &W, &H ) == fERROR)
		return;
		
	//print_text_centered(X+(W/2),Y+(H/4),"^F07Loading Scene:",BLACK);
	print_text_centered(X+(W/2),Y+(H/4),STRMGR_GetStr(STR_LOAD_SCENE),BLACK);

	// color_edged_rect( X + (W/6), Y + (H/2), 4*(W/6), H/4, GREY);
	// if (gLoadingPercent == 0)
	// 	gLoadingPercent = 1;
	// color_rect      ( X+(W/6)+1, Y+(H/2)+1, ((4*(W/6))*gLoadingPercent/100)-1, (H/4)-1, RED);
	
	// paint greyed sword
	bitm = GetResourceStd ("ui\\SWORDD.PCX", FALSE);
	DrawBitmap (X+SWORD_X, Y+SWORD_Y, bitm, 0, 0, SWORD_W, SWORD_H);
	SetPurge(bitm);
	
	// paint percentage bright sword
	bitm = GetResourceStd ("ui\\SWORDB.PCX", FALSE);
	DrawBitmap (
		X+SWORD_X,
		Y+SWORD_Y,
		bitm, 0, 0,
		gLoadingPercent*SWORD_W/100,
		SWORD_H);
	SetPurge(bitm);
}

/* ========================================================================
   Function    - SetLoadingProgress
   Description -
   Returns     -
   ======================================================================== */
void SetLoadingProgress( LONG Percent )
{
	gLoadingPercent = Percent;
}

