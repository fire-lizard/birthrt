/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: 
   Author: Greg Hightower
   ========================================================================
   Contains the following general functions:
   
   Contains the following internal functions:
   
   ======================================================================= */
#ifndef _MENUDATA_H
#define _MENUDATA_H

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#if !defined(_SYSTEM_H)
#include "system.h"
#endif

#if !defined(_CHARSEL_HXX)
#include "charsel.hxx"
#endif

#if defined(_MAIN_HXX)
#include "main.hxx"
#endif

#if !defined(_GAME_H)
#include "game.h"
#endif

#if !defined(_MENU_H)
#include "menu.h"
#endif

#if defined(_WINDOWS) && !defined(_MULPLAY_HXX)
#include "winsys\mulplay.hxx"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
void PaintDomainAction( LONG, LONG);
void PaintReportButtons( LONG, LONG);
void PaintReportMenu( LONG, LONG);
void PaintDOSHelp( LONG, LONG);
void CancelDomainAction( LONG, LONG );
void DoneReportMenu1( LONG, LONG );
void DOSHelpControl( LONG, LONG );
void RollDomainAction( LONG, LONG );
void DoneDomainAction( LONG, LONG );

 
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
#if defined(_GERMANVER)
BUTTON Help[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,             0,"UI\\DIALPNL1.PCX",0,"", 200,132,236,208,NO_KEY,PaintHelpScreen,0, -1},
{1,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,0,STR_HELP_1,  31,"",               31, "", 44, 78,152, 20,NO_KEY,0, 1, -1},
{2,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,0,STR_HELP_2, 173,"",               31, "", 44,110,152, 17,NO_KEY,UpdateHighlight, 2, -1},
//{3,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,0,STR_HELP_8,  16,"",               31, "", 44,127,152, 17,NO_KEY,NULL, 3, -1},
{4,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,0,STR_HELP_9A,173,"",               31, "", 44,144,152, 17,NO_KEY,UpdateHighlight, 4, -1},
//{5,BUTTON_REGION,0,             0,-1,             173,"",               31, "", 44,161,152, 17,NO_KEY,UpdateHighlight, 5, -1},
//{6,BUTTON_REGION,0,             0,-1,             173,"",               31, "", 44,178,152, 17,NO_KEY,UpdateHighlight, 6, -1},
//{7,BUTTON_REGION,0,             0,-1,             173,"",               31, "", 44,195,152, 17,NO_KEY,UpdateHighlight, 7, -1},
//{8,BUTTON_REGION,0,             0,-1,             173,"",               31, "", 44,212,152, 17,NO_KEY,UpdateHighlight, 8, -1},
//{9,BUTTON_REGION,0,             0,-1,             173,"",               31, "", 44,229,152, 17,NO_KEY,UpdateHighlight, 9, -1},
{10,BUTTON_REGION,0,    		  0,STR_HELP_DONE, BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNCB.PCX", 76,172, 84, 28,KEY_F ,HideSubMenuWithClick, D_HELP, -1},
{11,BUTTON_REGION,0,              0,-1,             0,"",                0,"",  0,  0,  0,  0,KEY_ESCAPE,HideSubMenu, D_HELP, -1},
};
LONG HelpSize = sizeof(Help) / sizeof(BUTTON);


BUTTON DomainAction[] = {
//{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,0,"UI\\HELP_LG.PCX",0,"",160, 50,356,372,NO_KEY,PaintDomainAction,0, -1},
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,0,"UI\\HELP_LG.PCX",0,"", 15, 90,356,372,NO_KEY,PaintDomainAction,0, -1},
{1,BUTTON_REGION,0,              0,-1,0,"",                0,"", 24, 24,305,320,NO_KEY,0, 0, -1},
{2,BUTTON_REGION,D_HILIGHTED,    0,-1,0,"",0,"UI\\HLPBTNAA.PCX",  28,340, 100, 28,KEY_ESCAPE,CancelDomainAction, 0, -1},
{3,BUTTON_REGION,D_HILIGHTED,    0,-1,0,"",0,"UI\\HLPBTNAA.PCX", 128,340, 100, 28,NO_KEY,RollDomainAction, 0, -1},
{4,BUTTON_REGION,D_HILIGHTED,    0,-1,0,"",0,"UI\\HLPBTNAA.PCX", 228,340, 100, 28,NO_KEY,DoneDomainAction, 0, -1},
};
LONG DomainActionSize = sizeof(DomainAction) / sizeof(BUTTON);

BUTTON AdvisorMenu[] = {
{0,BUTTON_REGION,D_FREEZE, 0,-1,            0, "",0,"",0,420,640,60,NO_KEY,PaintReportButtons,0, -1},
};
LONG AdvisorMenuSize = sizeof(AdvisorMenu) / sizeof(BUTTON);

BUTTON ReportMenu[] = {
//{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,            0,"UI\\HELP_LG.PCX",0,"",160, 50,356,372,NO_KEY,PaintReportMenu,0, -1},
{0,BUTTON_BITMAP,D_FREEZE, 0,-1,            0,"UI\\REPORT1.PCX",0,"",142, 50,356,372,NO_KEY,PaintReportMenu,0, -1},
//{1,BUTTON_REGION,0,               0,-1,            0,"",0,"",                 24, 24,305,320,NO_KEY,0, 0, -1},
{1,BUTTON_REGION,0,               0,-1,            0,"",0,"",  0, 0,356,372,NO_KEY,DoneReportMenu, 0, -1},
{2,BUTTON_REGION,0,               0,-1,            0, "",0,"", 0, 0,0,0,KEY_ESCAPE,DoneReportMenu1, 1,-1},
//{4,BUTTON_REGION,D_HILIGHTED,     0,STR_HELP_DONE,31,"",31,"UI\\HLPBTNAA.PCX", 228,336, 100, 28,NO_KEY,DoneReportMenu, 0, -1},
};
LONG ReportMenuSize = sizeof(ReportMenu) / sizeof(BUTTON);

BUTTON MainMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,0,"UI\\MAINPANL.PCX",0,"",110,  0,419,480,NO_KEY,PaintMainMenu,0, -1},
{1,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_RETURN,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM1.PCX", 64,242,292, 27,KEY_Z, MainMenuClick, 1, -1},
{2,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_NEW,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM2.PCX", 66,269,290, 26,KEY_N, MainMenuClick, 2, -1},
{3,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_LOAD,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM3.PCX", 67,295,288, 26,KEY_L, MainMenuClick, 3, -1},
{4,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_SAVE,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM4.PCX", 71,321,280, 26,KEY_S, MainMenuClick, 4, -1},
{5,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_OPTIONS,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM5.PCX", 79,347,264, 26,KEY_O, MainMenuClick, 5, -1},
#ifdef _WINDOWS		   
{6,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_MULTI,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM6.PCX", 92,373,237, 26,KEY_M, MainMenuClick, 6, -1},
#else				   
{6,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_MULTI,16,"",BTN_LABEL_HILITE_COLOR,"UI\\MM6.PCX", 92,373,237, 26,NO_KEY,MainMenuClick, 6, -1},
#endif				   
{7,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_HELP,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM7.PCX",112,399,197, 26,KEY_F1,MainMenuClick, 7, -1},
{8,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_QUIT,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM8.PCX",141,425,139, 26,KEY_V, MainMenuClick, 8, -1},
};
LONG MainMenuSize = sizeof(MainMenu) / sizeof(BUTTON);

BUTTON Question1[] = {
{0,            BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,          0,"UI\\HELP_SM.PCX",0,"",   142,120,356,112,NO_KEY,NULL,0, -1},
{QUESTION_TEXT,BUTTON_REGION,0,    0,-1,          1,"",0,"",                10, 10,320,70,NO_KEY,NULL,0, -1},
{QUESTION_BTN1,BUTTON_BITMAP,0,    0,STR_OK,     BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",128,76,100, 28,NO_KEY,NULL,1, -1},
};
LONG Question1Size = sizeof(Question1) / sizeof(BUTTON);

BUTTON Question2[] = {
{0,            BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,          0,"UI\\HELP_SM.PCX",0,"",   142,120,356,112,NO_KEY,NULL,0, -1},
{QUESTION_TEXT,BUTTON_REGION,0,    0,-1,          1,"",0,"",                10, 10,320,70,NO_KEY,NULL,0, -1},
{QUESTION_BTN1,BUTTON_BITMAP,0,    0,STR_OK,     BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",28,76,100, 28,NO_KEY,NULL,1, -1},
{QUESTION_BTN2,BUTTON_BITMAP,0,    0,STR_CANCEL, BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",228,76,100, 28,NO_KEY,NULL,3, -1},
};
LONG Question2Size = sizeof(Question2) / sizeof(BUTTON);

BUTTON Question3[] = {
{0,            BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,          0,"UI\\HELP_SM.PCX",0,"",   142,120,356,112,NO_KEY,NULL,0, -1},
{QUESTION_TEXT,BUTTON_REGION,0,    0,-1,          1,"",0,"",                10, 10,320,70,NO_KEY,NULL,0, -1},
{QUESTION_BTN1,BUTTON_BITMAP,0,    0,STR_OK,     BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",28,76,100, 28,NO_KEY,NULL,1, -1},
{QUESTION_BTN2,BUTTON_BITMAP,0,    0,STR_RETRY,  BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",128,76,100, 28,NO_KEY,NULL,2, -1},
{QUESTION_BTN3,BUTTON_BITMAP,0,    0,STR_CANCEL, BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",228,76,100, 28,NO_KEY,NULL,3, -1},
};
LONG Question3Size = sizeof(Question3) / sizeof(BUTTON);

BUTTON GameButton[] = {
{0,BUTTON_REGION,D_NO_PUSH,0,-1,0,"",0,"",599,  5, 36, 42,NO_KEY,GameButtonPaint,0, -1},
{1,BUTTON_BITMAP,0,        0,-1,0,"",0,"",  2,  2, 32, 32,NO_KEY,GameButtonPressed,0, -1},
};
LONG GameButtonSize = sizeof(GameButton) / sizeof(BUTTON);

BUTTON SaveMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\L&S_PANL.PCX",0,"",200,  80,236,296, NO_KEY,PaintLoadSaveMenu,1, -1},
{1,BUTTON_REGION,D_SANS_8,               0, STR_LOADSAVE_CANCEL, BTN_LABEL_COLOR, "UI\\L&SBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\L&SBTNCB.PCX", 36, 260, 84, 28, KEY_ESCAPE, CancelSave, 0, -1},
{2,BUTTON_REGION,D_SANS_8,               0, STR_LOADSAVE_SAVE, BTN_LABEL_COLOR, "UI\\L&SBTN&A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\L&SBTN&B.PCX",120, 260, 84, 28, NO_KEY, SaveGameDo, 0, -1},
{3,BUTTON_REGION,D_SANS_8,               0, STR_LOADSAVE_DELETE, BTN_LABEL_COLOR, "UI\\L&SBTN&A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\L&SBTN&B.PCX",120, 232, 84, 28, NO_KEY, DeleteGame, 0, -1},
};
LONG SaveMenuSize = sizeof(SaveMenu) / sizeof(BUTTON);

BUTTON LoadOurMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\L&S_PANL.PCX",0,"",200,80,236,296,NO_KEY,PaintLoadSaveMenu,2, -1},
{1,BUTTON_REGION,D_SANS_8,               0, STR_LOADSAVE_CANCEL, BTN_LABEL_COLOR, "UI\\L&SBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\L&SBTNCB.PCX", 36, 260, 84, 28, KEY_ESCAPE, CancelLoad, 0, -1},
{2,BUTTON_REGION,D_SANS_8,               0, STR_LOADSAVE_LOAD, BTN_LABEL_COLOR, "UI\\L&SBTN&A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\L&SBTN&B.PCX", 120,260, 84, 28, KEY_RETURN, LoadGameDo, 0, -1},
{3,BUTTON_REGION,D_SANS_8,               0, STR_LOADSAVE_DELETE, BTN_LABEL_COLOR, "UI\\L&SBTN&A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\L&SBTN&B.PCX", 120,232, 84, 28, NO_KEY, DeleteGame, 0, -1},
};
LONG LoadMenuSize = sizeof(LoadOurMenu) / sizeof(BUTTON);

BUTTON OptionMenu[] = {
{0,  BUTTON_BITMAP, D_FREEZE|D_ZOOM, 0, -1, 0,"UI\\OPT_PANL.PCX",0,"",132,80,376,304,NO_KEY,PaintOptionMenu,0, -1},

{1,  BUTTON_REGION,D_SANS_8,0,STR_OPT_CANCEL,BTN_LABEL_COLOR, "UI\\OPTBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTNCB.PCX", 144,264, 84, 28, KEY_ESCAPE, OptionCancel,   0, -1},

{2,  BUTTON_REGION,D_SANS_8,0,STR_OPT_DONE,BTN_LABEL_COLOR, "UI\\OPTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTNXB.PCX", 228,264, 84, 28, KEY_RETURN, OptionDo,       0, -1},
{3,  BUTTON_REGION,D_SANS_8,0,STR_OPT_DEFAULTS,BTN_LABEL_COLOR, "UI\\OPTBTNFA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTNFB.PCX",  60,264, 84, 28, KEY_V,      OptionDefaults, 0, -1},
  
  // menu selection buttons
{4,  BUTTON_REGION,D_SANS_8,0,STR_OPT_GAME,BTN_LABEL_COLOR, "UI\\OPTBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTN1B.PCX",  12,108,140, 24, NO_KEY, OptionRadio1,  0, -1},
{5,  BUTTON_REGION,D_SANS_8,0,STR_OPT_BATTLE,BTN_LABEL_COLOR, "UI\\OPTBTN2A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTN2B.PCX",  12,132,140, 24, NO_KEY, OptionRadio1,  0, -1},
{6,  BUTTON_REGION,D_SANS_8,0,STR_OPT_ADVENTURE,BTN_LABEL_COLOR, "UI\\OPTBTN3A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTN3B.PCX",  12,156,140, 24, NO_KEY, OptionRadio1,  0, -1},
{7,  BUTTON_REGION,D_SANS_8,0,STR_OPT_SOUND,BTN_LABEL_COLOR, "UI\\OPTBTN4A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTN4B.PCX",  12,180,140, 24, NO_KEY, OptionRadio1,  0, -1},
{8,  BUTTON_REGION,D_SANS_8,0,STR_OPT_SCREEN,BTN_LABEL_COLOR, "UI\\OPTBTN5A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTN5B.PCX",  12,204,140, 24, NO_KEY, OptionRadio1,  0, -1},
{9,  BUTTON_REGION,D_SANS_8,0,STR_OPT_SPEED,BTN_LABEL_COLOR, "UI\\OPTBTN6A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTN6B.PCX",  12,228,140, 24, NO_KEY, OptionRadio1,  0, -1},
};
LONG OptionMenuSize = sizeof(OptionMenu) / sizeof(BUTTON);

BUTTON ChatInputWindow[] = {
{0,BUTTON_SHADEBEVEL,D_FREEZE,SMOKE, -1,0,"",0,"",100,200,500, 50,NO_KEY,PaintChatInput,0, -1},
{1,BUTTON_REGION,           0,    0, -1,0,"",0,"",  0,  0,  0,  0,KEY_ESCAPE,HideSubMenu,D_CHAT_INPUT, -1},
};
LONG ChatInputWindowSize = sizeof(ChatInputWindow) / sizeof(BUTTON);

BUTTON DOSHelp[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,0,"UI\\HLPPANL3.PCX",0,"",60, 50,420,370,NO_KEY,PaintDOSHelp,0, -1},
{1,BUTTON_REGION,0,0,-1,0,"",0,"", 20, 20, 380,300,NO_KEY,0, 0, -1}, //text area
{2,BUTTON_REGION,D_HILIGHTED,0,-1,0,"",0,"UI\\HLPBTNAA.PCX",  60,332, 88, 20,KEY_ESCAPE,DOSHelpControl, 0, -1},
{3,BUTTON_REGION,D_HILIGHTED,0,-1,0,"",0,"UI\\HLPBTNAA.PCX", 160,332, 88, 20,KEY_BACKSPACE,DOSHelpControl, -1, -1},
{4,BUTTON_REGION,D_HILIGHTED,0,-1,0,"",0,"UI\\HLPBTNAA.PCX", 260,332, 88, 20,KEY_SPACE,DOSHelpControl, 1, -1},
};
LONG DOSHelpSize = sizeof(DOSHelp) / sizeof(BUTTON);

#else 
#if defined(_FRENCHVER)

BUTTON Help[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,             0,"UI\\DIALPNL1.PCX",0,"", 200,132,236,208,NO_KEY,PaintHelpScreen,0, -1},
{1,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,0,STR_HELP_1,  31,"",               31, "", 44, 78,152, 20,NO_KEY,0, 1, -1},
{2,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,0,STR_HELP_2, 173,"",               31, "", 44,110,152, 17,NO_KEY,UpdateHighlight, 2, -1},
//{3,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,0,STR_HELP_8,  16,"",               31, "", 44,127,152, 17,NO_KEY,NULL, 3, -1},
{4,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,0,STR_HELP_9A,173,"",               31, "", 44,144,152, 17,NO_KEY,UpdateHighlight, 4, -1},
//{5,BUTTON_REGION,0,             0,-1,             173,"",               31, "", 44,161,152, 17,NO_KEY,UpdateHighlight, 5, -1},
//{6,BUTTON_REGION,0,             0,-1,             173,"",               31, "", 44,178,152, 17,NO_KEY,UpdateHighlight, 6, -1},
//{7,BUTTON_REGION,0,             0,-1,             173,"",               31, "", 44,195,152, 17,NO_KEY,UpdateHighlight, 7, -1},
//{8,BUTTON_REGION,0,             0,-1,             173,"",               31, "", 44,212,152, 17,NO_KEY,UpdateHighlight, 8, -1},
//{9,BUTTON_REGION,0,             0,-1,             173,"",               31, "", 44,229,152, 17,NO_KEY,UpdateHighlight, 9, -1},
{10,BUTTON_REGION,0,    		  0,STR_HELP_DONE, BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNCB.PCX", 76,172, 84, 28,KEY_O ,HideSubMenuWithClick, D_HELP, -1},
{11,BUTTON_REGION,0,              0,-1,             0,"",                0,"",  0,  0,  0,  0,KEY_ESCAPE,HideSubMenu, D_HELP, -1},
};
LONG HelpSize = sizeof(Help) / sizeof(BUTTON);


BUTTON DomainAction[] = {
//{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,0,"UI\\HELP_LG.PCX",0,"",160, 50,356,372,NO_KEY,PaintDomainAction,0, -1},
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,0,"UI\\HELP_LG.PCX",0,"", 15, 90,356,372,NO_KEY,PaintDomainAction,0, -1},
{1,BUTTON_REGION,0,              0,-1,0,"",                0,"", 24, 24,305,320,NO_KEY,0, 0, -1},
{2,BUTTON_REGION,D_HILIGHTED,    0,-1,0,"",0,"UI\\HLPBTNAA.PCX",  28,340, 100, 28,KEY_ESCAPE,CancelDomainAction, 0, -1},
{3,BUTTON_REGION,D_HILIGHTED,    0,-1,0,"",0,"UI\\HLPBTNAA.PCX", 128,340, 100, 28,NO_KEY,RollDomainAction, 0, -1},
{4,BUTTON_REGION,D_HILIGHTED,    0,-1,0,"",0,"UI\\HLPBTNAA.PCX", 228,340, 100, 28,NO_KEY,DoneDomainAction, 0, -1},
};
LONG DomainActionSize = sizeof(DomainAction) / sizeof(BUTTON);

BUTTON AdvisorMenu[] = {
{0,BUTTON_REGION,D_FREEZE, 0,-1,            0, "",0,"",0,420,640,60,NO_KEY,PaintReportButtons,0, -1},
};
LONG AdvisorMenuSize = sizeof(AdvisorMenu) / sizeof(BUTTON);

BUTTON ReportMenu[] = {
//{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,            0,"UI\\HELP_LG.PCX",0,"",160, 50,356,372,NO_KEY,PaintReportMenu,0, -1},
{0,BUTTON_BITMAP,D_FREEZE, 0,-1,            0,"UI\\REPORT1.PCX",0,"",142, 50,356,372,NO_KEY,PaintReportMenu,0, -1},
//{1,BUTTON_REGION,0,               0,-1,            0,"",0,"",                 24, 24,305,320,NO_KEY,0, 0, -1},
{1,BUTTON_REGION,0,               0,-1,            0,"",0,"",  0, 0,356,372,NO_KEY,DoneReportMenu, 0, -1},
{2,BUTTON_REGION,0,               0,-1,            0, "",0,"", 0, 0,0,0,KEY_ESCAPE,DoneReportMenu1, 1,-1},
//{4,BUTTON_REGION,D_HILIGHTED,     0,STR_HELP_DONE,31,"",31,"UI\\HLPBTNAA.PCX", 228,336, 100, 28,NO_KEY,DoneReportMenu, 0, -1},
};
LONG ReportMenuSize = sizeof(ReportMenu) / sizeof(BUTTON);

BUTTON MainMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,0,"UI\\MAINPANL.PCX",0,"",110,  0,419,480,NO_KEY,PaintMainMenu,0, -1},
{1,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_RETURN,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM1.PCX", 64,242,292, 27,KEY_P, MainMenuClick, 1, -1},
{2,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_NEW,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM2.PCX", 66,269,290, 26,KEY_N, MainMenuClick, 2, -1},
{3,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_LOAD,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM3.PCX", 67,295,288, 26,KEY_C, MainMenuClick, 3, -1},
{4,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_SAVE,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM4.PCX", 71,321,280, 26,KEY_S, MainMenuClick, 4, -1},
{5,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_OPTIONS,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM5.PCX", 79,347,264, 26,KEY_O, MainMenuClick, 5, -1},
#ifdef _WINDOWS		   
{6,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_MULTI,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM6.PCX", 92,373,237, 26,KEY_M, MainMenuClick, 6, -1},
#else				   
{6,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_MULTI,16,"",BTN_LABEL_HILITE_COLOR,"UI\\MM6.PCX", 92,373,237, 26,NO_KEY,MainMenuClick, 6, -1},
#endif				   
{7,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_HELP,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM7.PCX",112,399,197, 26,KEY_F1,MainMenuClick, 7, -1},
{8,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_QUIT,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM8.PCX",141,425,139, 26,KEY_Q, MainMenuClick, 8, -1},
};
LONG MainMenuSize = sizeof(MainMenu) / sizeof(BUTTON);

BUTTON Question1[] = {
{0,            BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,          0,"UI\\HELP_SM.PCX",0,"",   142,120,356,112,NO_KEY,NULL,0, -1},
{QUESTION_TEXT,BUTTON_REGION,0,    0,-1,          1,"",0,"",                10, 10,320,70,NO_KEY,NULL,0, -1},
{QUESTION_BTN1,BUTTON_BITMAP,0,    0,STR_OK,     BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",128,76,100, 28,NO_KEY,NULL,1, -1},
};
LONG Question1Size = sizeof(Question1) / sizeof(BUTTON);

BUTTON Question2[] = {
{0,            BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,          0,"UI\\HELP_SM.PCX",0,"",   142,120,356,112,NO_KEY,NULL,0, -1},
{QUESTION_TEXT,BUTTON_REGION,0,    0,-1,          1,"",0,"",                10, 10,320,70,NO_KEY,NULL,0, -1},
{QUESTION_BTN1,BUTTON_BITMAP,0,    0,STR_OK,     BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",28,76,100, 28,NO_KEY,NULL,1, -1},
{QUESTION_BTN2,BUTTON_BITMAP,0,    0,STR_CANCEL, BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",228,76,100, 28,NO_KEY,NULL,3, -1},
};
LONG Question2Size = sizeof(Question2) / sizeof(BUTTON);

BUTTON Question3[] = {
{0,            BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,          0,"UI\\HELP_SM.PCX",0,"",   142,120,356,112,NO_KEY,NULL,0, -1},
{QUESTION_TEXT,BUTTON_REGION,0,    0,-1,          1,"",0,"",                10, 10,320,70,NO_KEY,NULL,0, -1},
{QUESTION_BTN1,BUTTON_BITMAP,0,    0,STR_OK,     BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",28,76,100, 28,NO_KEY,NULL,1, -1},
{QUESTION_BTN2,BUTTON_BITMAP,0,    0,STR_RETRY,  BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",128,76,100, 28,NO_KEY,NULL,2, -1},
{QUESTION_BTN3,BUTTON_BITMAP,0,    0,STR_CANCEL, BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",228,76,100, 28,NO_KEY,NULL,3, -1},
};
LONG Question3Size = sizeof(Question3) / sizeof(BUTTON);

BUTTON GameButton[] = {
{0,BUTTON_REGION,D_NO_PUSH,0,-1,0,"",0,"",599,  5, 36, 42,NO_KEY,GameButtonPaint,0, -1},
{1,BUTTON_BITMAP,0,        0,-1,0,"",0,"",  2,  2, 32, 32,NO_KEY,GameButtonPressed,0, -1},
};
LONG GameButtonSize = sizeof(GameButton) / sizeof(BUTTON);

BUTTON SaveMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\L&S_PANL.PCX",0,"",200,  80,236,296, NO_KEY,PaintLoadSaveMenu,1, -1},
{1,BUTTON_REGION,D_SANS_8,               0, STR_LOADSAVE_CANCEL, BTN_LABEL_COLOR, "UI\\L&SBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\L&SBTNCB.PCX", 36, 260, 84, 28, KEY_ESCAPE, CancelSave, 0, -1},
{2,BUTTON_REGION,D_SANS_8,               0, STR_LOADSAVE_SAVE, BTN_LABEL_COLOR, "UI\\L&SBTN&A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\L&SBTN&B.PCX",120, 260, 84, 28, NO_KEY, SaveGameDo, 0, -1},
{3,BUTTON_REGION,D_SANS_8,               0, STR_LOADSAVE_DELETE, BTN_LABEL_COLOR, "UI\\L&SBTN&A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\L&SBTN&B.PCX",120, 232, 84, 28, NO_KEY, DeleteGame, 0, -1},
};
LONG SaveMenuSize = sizeof(SaveMenu) / sizeof(BUTTON);

BUTTON LoadOurMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\L&S_PANL.PCX",0,"",200,80,236,296,NO_KEY,PaintLoadSaveMenu,2, -1},
{1,BUTTON_REGION,D_SANS_8,               0, STR_LOADSAVE_CANCEL, BTN_LABEL_COLOR, "UI\\L&SBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\L&SBTNCB.PCX", 36, 260, 84, 28, KEY_ESCAPE, CancelLoad, 0, -1},
{2,BUTTON_REGION,D_SANS_8,               0, STR_LOADSAVE_LOAD, BTN_LABEL_COLOR, "UI\\L&SBTN&A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\L&SBTN&B.PCX", 120,260, 84, 28, KEY_RETURN, LoadGameDo, 0, -1},
{3,BUTTON_REGION,D_SANS_8,               0, STR_LOADSAVE_DELETE, BTN_LABEL_COLOR, "UI\\L&SBTN&A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\L&SBTN&B.PCX", 120,232, 84, 28, NO_KEY, DeleteGame, 0, -1},
};
LONG LoadMenuSize = sizeof(LoadOurMenu) / sizeof(BUTTON);

BUTTON OptionMenu[] = {
{0,  BUTTON_BITMAP, D_FREEZE|D_ZOOM, 0, -1, 0,"UI\\OPT_PANL.PCX",0,"",132,80,376,304,NO_KEY,PaintOptionMenu,0, -1},

{1,  BUTTON_REGION,D_SANS_8,0,STR_OPT_CANCEL,BTN_LABEL_COLOR, "UI\\OPTBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTNCB.PCX", 144,264, 84, 28, KEY_ESCAPE, OptionCancel,   0, -1},

{2,  BUTTON_REGION,D_SANS_8,0,STR_OPT_DONE,BTN_LABEL_COLOR, "UI\\OPTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTNXB.PCX", 228,264, 84, 28, KEY_RETURN, OptionDo,       0, -1},
{3,  BUTTON_REGION,D_SANS_8,0,STR_OPT_DEFAULTS,BTN_LABEL_COLOR, "UI\\OPTBTNFA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTNFB.PCX",  60,264, 84, 28, KEY_D,      OptionDefaults, 0, -1},
  
  // menu selection buttons
{4,  BUTTON_REGION,D_SANS_8,0,STR_OPT_GAME,BTN_LABEL_COLOR, "UI\\OPTBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTN1B.PCX",  12,108,140, 24, NO_KEY, OptionRadio1,  0, -1},
{5,  BUTTON_REGION,D_SANS_8,0,STR_OPT_BATTLE,BTN_LABEL_COLOR, "UI\\OPTBTN2A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTN2B.PCX",  12,132,140, 24, NO_KEY, OptionRadio1,  0, -1},
{6,  BUTTON_REGION,D_SANS_8,0,STR_OPT_ADVENTURE,BTN_LABEL_COLOR, "UI\\OPTBTN3A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTN3B.PCX",  12,156,140, 24, NO_KEY, OptionRadio1,  0, -1},
{7,  BUTTON_REGION,D_SANS_8,0,STR_OPT_SOUND,BTN_LABEL_COLOR, "UI\\OPTBTN4A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTN4B.PCX",  12,180,140, 24, NO_KEY, OptionRadio1,  0, -1},
{8,  BUTTON_REGION,D_SANS_8,0,STR_OPT_SCREEN,BTN_LABEL_COLOR, "UI\\OPTBTN5A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTN5B.PCX",  12,204,140, 24, NO_KEY, OptionRadio1,  0, -1},
{9,  BUTTON_REGION,D_SANS_8,0,STR_OPT_SPEED,BTN_LABEL_COLOR, "UI\\OPTBTN6A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTN6B.PCX",  12,228,140, 24, NO_KEY, OptionRadio1,  0, -1},
};
LONG OptionMenuSize = sizeof(OptionMenu) / sizeof(BUTTON);

BUTTON ChatInputWindow[] = {
{0,BUTTON_SHADEBEVEL,D_FREEZE,SMOKE, -1,0,"",0,"",100,200,500, 50,NO_KEY,PaintChatInput,0, -1},
{1,BUTTON_REGION,           0,    0, -1,0,"",0,"",  0,  0,  0,  0,KEY_ESCAPE,HideSubMenu,D_CHAT_INPUT, -1},
};
LONG ChatInputWindowSize = sizeof(ChatInputWindow) / sizeof(BUTTON);

BUTTON DOSHelp[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,0,"UI\\HLPPANL3.PCX",0,"",60, 50,420,370,NO_KEY,PaintDOSHelp,0, -1},
{1,BUTTON_REGION,0,0,-1,0,"",0,"", 20, 20, 380,300,NO_KEY,0, 0, -1}, //text area
{2,BUTTON_REGION,D_HILIGHTED,0,-1,0,"",0,"UI\\HLPBTNAA.PCX",  60,332, 88, 20,KEY_ESCAPE,DOSHelpControl, 0, -1},
{3,BUTTON_REGION,D_HILIGHTED,0,-1,0,"",0,"UI\\HLPBTNAA.PCX", 160,332, 88, 20,KEY_BACKSPACE,DOSHelpControl, -1, -1},
{4,BUTTON_REGION,D_HILIGHTED,0,-1,0,"",0,"UI\\HLPBTNAA.PCX", 260,332, 88, 20,KEY_SPACE,DOSHelpControl, 1, -1},
};
LONG DOSHelpSize = sizeof(DOSHelp) / sizeof(BUTTON);

#else // english version

BUTTON Help[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,             0,"UI\\DIALPNL1.PCX",0,"", 200,132,236,208,NO_KEY,PaintHelpScreen,0, -1},
{1,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,0,STR_HELP_1,  31,"",               31, "", 44, 78,152, 20,NO_KEY,0, 1, -1},
{2,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,0,STR_HELP_2, 173,"",               31, "", 44,110,152, 17,NO_KEY,UpdateHighlight, 2, -1},
{3,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,0,STR_HELP_8, 173,"",               31, "", 44,127,152, 17,NO_KEY,UpdateHighlight, 3, -1},
{4,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,0,STR_HELP_9A,173,"",               31, "", 44,144,152, 17,NO_KEY,UpdateHighlight, 4, -1},
//{5,BUTTON_REGION,0,             0,-1,             173,"",               31, "", 44,161,152, 17,NO_KEY,UpdateHighlight, 5, -1},
//{6,BUTTON_REGION,0,             0,-1,             173,"",               31, "", 44,178,152, 17,NO_KEY,UpdateHighlight, 6, -1},
//{7,BUTTON_REGION,0,             0,-1,             173,"",               31, "", 44,195,152, 17,NO_KEY,UpdateHighlight, 7, -1},
//{8,BUTTON_REGION,0,             0,-1,             173,"",               31, "", 44,212,152, 17,NO_KEY,UpdateHighlight, 8, -1},
//{9,BUTTON_REGION,0,             0,-1,             173,"",               31, "", 44,229,152, 17,NO_KEY,UpdateHighlight, 9, -1},
{10,BUTTON_REGION,0,    		  0,STR_HELP_DONE, BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNCB.PCX", 76,172, 84, 28,KEY_D ,HideSubMenuWithClick, D_HELP, -1},
{11,BUTTON_REGION,0,              0,-1,             0,"",                0,"",  0,  0,  0,  0,KEY_ESCAPE,HideSubMenu, D_HELP, -1},
};
LONG HelpSize = sizeof(Help) / sizeof(BUTTON);


BUTTON DomainAction[] = {
//{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,0,"UI\\HELP_LG.PCX",0,"",160, 50,356,372,NO_KEY,PaintDomainAction,0, -1},
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,0,"UI\\HELP_LG.PCX",0,"", 15, 90,356,372,NO_KEY,PaintDomainAction,0, -1},
{1,BUTTON_REGION,0,              0,-1,0,"",                0,"", 24, 24,305,320,NO_KEY,0, 0, -1},
{2,BUTTON_REGION,D_HILIGHTED,    0,-1,0,"",0,"UI\\HLPBTNAA.PCX",  28,340, 100, 28,KEY_ESCAPE,CancelDomainAction, 0, -1},
{3,BUTTON_REGION,D_HILIGHTED,    0,-1,0,"",0,"UI\\HLPBTNAA.PCX", 128,340, 100, 28,NO_KEY,RollDomainAction, 0, -1},
{4,BUTTON_REGION,D_HILIGHTED,    0,-1,0,"",0,"UI\\HLPBTNAA.PCX", 228,340, 100, 28,NO_KEY,DoneDomainAction, 0, -1},
};
LONG DomainActionSize = sizeof(DomainAction) / sizeof(BUTTON);

BUTTON AdvisorMenu[] = {
{0,BUTTON_REGION,D_FREEZE, 0,-1,            0, "",0,"",0,420,640,60,NO_KEY,PaintReportButtons,0, -1},
};
LONG AdvisorMenuSize = sizeof(AdvisorMenu) / sizeof(BUTTON);

BUTTON ReportMenu[] = {
//{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,            0,"UI\\HELP_LG.PCX",0,"",160, 50,356,372,NO_KEY,PaintReportMenu,0, -1},
{0,BUTTON_BITMAP,D_FREEZE, 0,-1,            0,"UI\\REPORT1.PCX",0,"",142, 50,356,372,NO_KEY,PaintReportMenu,0, -1},
//{1,BUTTON_REGION,0,               0,-1,            0,"",0,"",                 24, 24,305,320,NO_KEY,0, 0, -1},
{1,BUTTON_REGION,0,               0,-1,            0,"",0,"",  0, 0,356,372,NO_KEY,DoneReportMenu, 0, -1},
{2,BUTTON_REGION,0,               0,-1,            0, "",0,"", 0, 0,0,0,KEY_ESCAPE,DoneReportMenu1, 1,-1},
//{4,BUTTON_REGION,D_HILIGHTED,     0,STR_HELP_DONE,31,"",31,"UI\\HLPBTNAA.PCX", 228,336, 100, 28,NO_KEY,DoneReportMenu, 0, -1},
};
LONG ReportMenuSize = sizeof(ReportMenu) / sizeof(BUTTON);

BUTTON MainMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,0,"UI\\MAINPANL.PCX",0,"",110,  0,419,480,NO_KEY,PaintMainMenu,0, -1},
{1,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_RETURN,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM1.PCX", 64,242,292, 27,KEY_G, MainMenuClick, 1, -1},
{2,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_NEW,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM2.PCX", 66,269,290, 26,KEY_N, MainMenuClick, 2, -1},
{3,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_LOAD,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM3.PCX", 67,295,288, 26,KEY_L, MainMenuClick, 3, -1},
{4,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_SAVE,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM4.PCX", 71,321,280, 26,KEY_S, MainMenuClick, 4, -1},
{5,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_OPTIONS,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM5.PCX", 79,347,264, 26,KEY_O, MainMenuClick, 5, -1},
#ifdef _WINDOWS		   
{6,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_MULTI,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM6.PCX", 92,373,237, 26,KEY_M, MainMenuClick, 6, -1},
#else				   
{6,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_MULTI,16,"",BTN_LABEL_HILITE_COLOR,"UI\\MM6.PCX", 92,373,237, 26,NO_KEY,MainMenuClick, 6, -1},
#endif				   
{7,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_HELP,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM7.PCX",112,399,197, 26,KEY_F1,MainMenuClick, 7, -1},
{8,BUTTON_REGION,D_TITL_16,0,STR_MAIN1_QUIT,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\MM8.PCX",141,425,139, 26,KEY_Q, MainMenuClick, 8, -1},
};
LONG MainMenuSize = sizeof(MainMenu) / sizeof(BUTTON);

BUTTON Question1[] = {
{0,            BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,          0,"UI\\HELP_SM.PCX",0,"",   142,120,356,112,NO_KEY,NULL,0, -1},
{QUESTION_TEXT,BUTTON_REGION,0,    0,-1,          1,"",0,"",                10, 10,320,70,NO_KEY,NULL,0, -1},
{QUESTION_BTN1,BUTTON_BITMAP,0,    0,STR_OK,     BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",128,76,100, 28,NO_KEY,NULL,1, -1},
};
LONG Question1Size = sizeof(Question1) / sizeof(BUTTON);

BUTTON Question2[] = {
{0,            BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,          0,"UI\\HELP_SM.PCX",0,"",   142,120,356,112,NO_KEY,NULL,0, -1},
{QUESTION_TEXT,BUTTON_REGION,0,    0,-1,          1,"",0,"",                10, 10,320,70,NO_KEY,NULL,0, -1},
{QUESTION_BTN1,BUTTON_BITMAP,0,    0,STR_OK,     BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",28,76,100, 28,NO_KEY,NULL,1, -1},
{QUESTION_BTN2,BUTTON_BITMAP,0,    0,STR_CANCEL, BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",228,76,100, 28,NO_KEY,NULL,3, -1},
};
LONG Question2Size = sizeof(Question2) / sizeof(BUTTON);

BUTTON Question3[] = {
{0,            BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,          0,"UI\\HELP_SM.PCX",0,"",   142,120,356,112,NO_KEY,NULL,0, -1},
{QUESTION_TEXT,BUTTON_REGION,0,    0,-1,          1,"",0,"",                10, 10,320,70,NO_KEY,NULL,0, -1},
{QUESTION_BTN1,BUTTON_BITMAP,0,    0,STR_OK,     BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",28,76,100, 28,NO_KEY,NULL,1, -1},
{QUESTION_BTN2,BUTTON_BITMAP,0,    0,STR_RETRY,  BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",128,76,100, 28,NO_KEY,NULL,2, -1},
{QUESTION_BTN3,BUTTON_BITMAP,0,    0,STR_CANCEL, BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",228,76,100, 28,NO_KEY,NULL,3, -1},
};
LONG Question3Size = sizeof(Question3) / sizeof(BUTTON);

BUTTON GameButton[] = {
{0,BUTTON_REGION,D_NO_PUSH,0,-1,0,"",0,"",599,  5, 36, 42,NO_KEY,GameButtonPaint,0, -1},
{1,BUTTON_BITMAP,0,        0,-1,0,"",0,"",  2,  2, 32, 32,NO_KEY,GameButtonPressed,0, -1},
};
LONG GameButtonSize = sizeof(GameButton) / sizeof(BUTTON);

BUTTON SaveMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\L&S_PANL.PCX",0,"",200,  80,236,296, NO_KEY,PaintLoadSaveMenu,1, -1},
{1,BUTTON_REGION,D_SANS_8,               0, STR_LOADSAVE_CANCEL, BTN_LABEL_COLOR, "UI\\L&SBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\L&SBTNCB.PCX", 36, 260, 84, 28, KEY_ESCAPE, CancelSave, 0, -1},
{2,BUTTON_REGION,D_SANS_8,               0, STR_LOADSAVE_SAVE, BTN_LABEL_COLOR, "UI\\L&SBTN&A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\L&SBTN&B.PCX",120, 260, 84, 28, NO_KEY, SaveGameDo, 0, -1},
{3,BUTTON_REGION,D_SANS_8,               0, STR_LOADSAVE_DELETE, BTN_LABEL_COLOR, "UI\\L&SBTN&A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\L&SBTN&B.PCX",120, 232, 84, 28, NO_KEY, DeleteGame, 0, -1},
};
LONG SaveMenuSize = sizeof(SaveMenu) / sizeof(BUTTON);

BUTTON LoadOurMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\L&S_PANL.PCX",0,"",200,80,236,296,NO_KEY,PaintLoadSaveMenu,2, -1},
{1,BUTTON_REGION,D_SANS_8,               0, STR_LOADSAVE_CANCEL, BTN_LABEL_COLOR, "UI\\L&SBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\L&SBTNCB.PCX", 36, 260, 84, 28, KEY_ESCAPE, CancelLoad, 0, -1},
{2,BUTTON_REGION,D_SANS_8,               0, STR_LOADSAVE_LOAD, BTN_LABEL_COLOR, "UI\\L&SBTN&A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\L&SBTN&B.PCX", 120,260, 84, 28, KEY_RETURN, LoadGameDo, 0, -1},
{3,BUTTON_REGION,D_SANS_8,               0, STR_LOADSAVE_DELETE, BTN_LABEL_COLOR, "UI\\L&SBTN&A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\L&SBTN&B.PCX", 120,232, 84, 28, NO_KEY, DeleteGame, 0, -1},
};
LONG LoadMenuSize = sizeof(LoadOurMenu) / sizeof(BUTTON);

BUTTON OptionMenu[] = {
{0,  BUTTON_BITMAP, D_FREEZE|D_ZOOM, 0, -1, 0,"UI\\OPT_PANL.PCX",0,"",132,80,376,304,NO_KEY,PaintOptionMenu,0, -1},

{1,  BUTTON_REGION,D_SANS_8,0,STR_OPT_CANCEL,BTN_LABEL_COLOR, "UI\\OPTBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTNCB.PCX", 144,264, 84, 28, KEY_ESCAPE, OptionCancel,   0, -1},

{2,  BUTTON_REGION,D_SANS_8,0,STR_OPT_DONE,BTN_LABEL_COLOR, "UI\\OPTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTNXB.PCX", 228,264, 84, 28, KEY_RETURN, OptionDo,       0, -1},
{3,  BUTTON_REGION,D_SANS_8,0,STR_OPT_DEFAULTS,BTN_LABEL_COLOR, "UI\\OPTBTNFA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTNFB.PCX",  60,264, 84, 28, KEY_D,      OptionDefaults, 0, -1},
  
  // menu selection buttons
{4,  BUTTON_REGION,D_SANS_8,0,STR_OPT_GAME,BTN_LABEL_COLOR, "UI\\OPTBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTN1B.PCX",  12,108,140, 24, NO_KEY, OptionRadio1,  0, -1},
{5,  BUTTON_REGION,D_SANS_8,0,STR_OPT_BATTLE,BTN_LABEL_COLOR, "UI\\OPTBTN2A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTN2B.PCX",  12,132,140, 24, NO_KEY, OptionRadio1,  0, -1},
{6,  BUTTON_REGION,D_SANS_8,0,STR_OPT_ADVENTURE,BTN_LABEL_COLOR, "UI\\OPTBTN3A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTN3B.PCX",  12,156,140, 24, NO_KEY, OptionRadio1,  0, -1},
{7,  BUTTON_REGION,D_SANS_8,0,STR_OPT_SOUND,BTN_LABEL_COLOR, "UI\\OPTBTN4A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTN4B.PCX",  12,180,140, 24, NO_KEY, OptionRadio1,  0, -1},
{8,  BUTTON_REGION,D_SANS_8,0,STR_OPT_SCREEN,BTN_LABEL_COLOR, "UI\\OPTBTN5A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTN5B.PCX",  12,204,140, 24, NO_KEY, OptionRadio1,  0, -1},
{9,  BUTTON_REGION,D_SANS_8,0,STR_OPT_SPEED,BTN_LABEL_COLOR, "UI\\OPTBTN6A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\OPTBTN6B.PCX",  12,228,140, 24, NO_KEY, OptionRadio1,  0, -1},
};
LONG OptionMenuSize = sizeof(OptionMenu) / sizeof(BUTTON);

BUTTON ChatInputWindow[] = {
{0,BUTTON_SHADEBEVEL,D_FREEZE,SMOKE, -1,0,"",0,"",100,200,500, 50,NO_KEY,PaintChatInput,0, -1},
{1,BUTTON_REGION,           0,    0, -1,0,"",0,"",  0,  0,  0,  0,KEY_ESCAPE,HideSubMenu,D_CHAT_INPUT, -1},
};
LONG ChatInputWindowSize = sizeof(ChatInputWindow) / sizeof(BUTTON);

BUTTON DOSHelp[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,0,"UI\\HLPPANL3.PCX",0,"",60, 50,420,370,NO_KEY,PaintDOSHelp,0, -1},
{1,BUTTON_REGION,0,0,-1,0,"",0,"", 20, 20, 380,300,NO_KEY,0, 0, -1}, //text area
{2,BUTTON_REGION,D_HILIGHTED,0,-1,0,"",0,"UI\\HLPBTNAA.PCX",  60,332, 88, 20,KEY_ESCAPE,DOSHelpControl, 0, -1},
{3,BUTTON_REGION,D_HILIGHTED,0,-1,0,"",0,"UI\\HLPBTNAA.PCX", 160,332, 88, 20,KEY_BACKSPACE,DOSHelpControl, -1, -1},
{4,BUTTON_REGION,D_HILIGHTED,0,-1,0,"",0,"UI\\HLPBTNAA.PCX", 260,332, 88, 20,KEY_SPACE,DOSHelpControl, 1, -1},
};
LONG DOSHelpSize = sizeof(DOSHelp) / sizeof(BUTTON);

#endif	// french
#endif	// english

#endif
