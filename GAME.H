
/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _GAME_H
#define _GAME_H

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_SYSTEM_H)
#include "system.h"
#endif

#if !defined(_REALM_HXX)
#include "realm.hxx"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define BUTNO_STATUS_HDL	0

#define	CHAT_CHARS		75
#define  CHAT_MAXLINES  8     
#define	CHAT_LINES		4
#define	CHAT_SPACING	14

#define CHAT_DISPLAY_HEIGHT	64

extern BOOL	fChatLineOn;
extern BOOL fShowProgressBar;

/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* these are for the build-a-battle dialog */
enum {
	BB_MY_ADD = 500,
	BB_MY_REMOVE,
	BB_HIS_ADD,
	BB_HIS_REMOVE,
	
	BB_MYARMY1,
	BB_MYARMY2,
	BB_MYARMY3,
	BB_MYARMY4,
	BB_MYARMY5,
	BB_MYARMY6,
	BB_MYARMY7,
	BB_MYARMY8,
	BB_MYARMY9,
	BB_MYARMY10,
	BB_MYARMY11,
	BB_MYARMY12,
	BB_MYARMY13,
	BB_MYARMY14,
	BB_MYARMY15,
	BB_HISARMY1,
	BB_HISARMY2,
	BB_HISARMY3,
	BB_HISARMY4,
	BB_HISARMY5,
	BB_HISARMY6,
	BB_HISARMY7,
	BB_HISARMY8,
	BB_HISARMY9,
	BB_HISARMY10,
	BB_HISARMY11,
	BB_HISARMY12,
	BB_HISARMY13,
	BB_HISARMY14,
	BB_HISARMY15,
	
	BB_A_INF_UNIT,
	BB_A_EIN_UNIT,
	BB_A_BOW_UNIT,
	BB_A_IRR_UNIT,
	BB_A_PIK_UNIT,
	BB_A_LVY_UNIT,
	BB_A_SCT_UNIT,
	BB_A_KNT_UNIT,
	BB_A_CAV_UNIT,
	BB_M_INF_UNIT,
	BB_M_BOW_UNIT,
	BB_M_IRR_UNIT,
	BB_M_PIK_UNIT,
	BB_M_CAV_UNIT,
	BB_E_INF_UNIT,
	BB_E_BOW_UNIT,
	BB_E_CAV_UNIT,
	BB_D_INF_UNIT,
	BB_D_BOW_UNIT,
	BB_G_INF_UNIT,
	BB_G_BOW_UNIT,
	BB_G_CAV_UNIT,
	BB_N_INF_UNIT,
	BB_N_IRR_UNIT,
	BB_C_SKL_UNIT,
	BB_C_SPD_UNIT,
	
	BB_REGENT1_UNIT,
	BB_REGENT2_UNIT,
	BB_REGENT3_UNIT,
	BB_REGENT4_UNIT,
	BB_REGENT5_UNIT,
	BB_REGENT6_UNIT,
	BB_GORGON_UNIT,
	BB_SPIDER_UNIT,
	BB_RHOUBHE_UNIT,
	
	BB_LIEUTENANT1_UNIT,
	BB_LIEUTENANT2_UNIT,
	BB_LIEUTENANT3_UNIT,
	BB_LIEUTENANT4_UNIT,
	BB_LIEUTENANT5_UNIT,
	BB_LIEUTENANT6_UNIT,
	
	BB_DISPLAY,
	
	BB_PLAINS,
	BB_MOUNTAINS,
	BB_FOREST,
	BB_SWAMP,
	
	BB_RESTART,
	BB_RELOAD,
	BB_PRACTICE,
	BB_FIGHT,
};

/* these enums are for build-an-adv */
enum {
	BA_MODE = 500,
	
	BA_LIST_01,
	BA_LIST_02,
	BA_LIST_03,
	BA_LIST_04,
	BA_LIST_05,
	BA_LIST_06,
	BA_LIST_07,
	BA_LIST_08,
	BA_LIST_09,
	BA_LIST_10,
	BA_LIST_11,
	BA_LIST_12,
	BA_LIST_13,
	BA_LIST_14,
	BA_LIST_15,
	BA_LIST_16,
	BA_LIST_17,
	BA_LIST_18,
	BA_LIST_19,
	BA_LIST_20,
	BA_LIST_21,
	BA_LIST_22,
	BA_LIST_23,
	BA_LIST_24,
	BA_LIST_25,
	BA_LIST_26,
	
	BA_PARTY_LIST,
	BA_RESET,
	BA_DONE,
};

enum {
	SYSHELP_DIALOG = 600,  
	SYSHELP_TITLE,   
	SYSHELP_TEXTBODY,
	SYSHELP_COMMENT, 
	SYSHELP_DONE,    
	SYSHELP_MOREINFO,
};


// moved to doshelp.hxx
//typedef enum
//{
//	H_TOPIC_ATLAS=0,
//	H_TOPIC_RULES,
//	H_TOPIC_QUICK_KEYS
//} HELP_TOPIC;

#define SYSHELP_LARGE_BOX	0x00000001
#define SYSHELP_MED_BOX		0x00000002
#define SYSHELP_SMALL_BOX	0x00000004

/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
int PaintInputField(LONG, char *, LONG, LONG, BOOL, LONG, char, char );
void PaintHelp (LONG MenuCombo, LONG );
void PaintHelpScreen (LONG MenuCombo, LONG );
void DoneHelpMenu (LONG, LONG);
void MainMenuClick (LONG MenuCombo, LONG ButtonVal);
void PaintMainMenu (LONG MenuCombo, LONG );
void PaintAttributes (LONG MenuCombo, LONG );
void PaintLoyaltyTable (LONG MenuCombo, LONG );
void DoneReportMenu (LONG, LONG);
void PaintMoreInfo (LONG MenuCombo, LONG );
void PlayAnimation (LONG MenuCombo, LONG);
void ShowStatus(LONG tID, LONG);
void HideStatus(LONG, LONG);
void HideStatus2(LONG, LONG);
void ShowCharacter(LONG tID, LONG hPossibleStats);
void HideCharacter(LONG, LONG);
void InventoryFromStatus(LONG, LONG);
void StatusFromInventory(LONG, LONG);
void BowToPlayer(LONG, LONG);

void PaintHoldAction(LONG, LONG);
void CancelHoldAction(LONG, LONG);
void DoneHoldAction(LONG, LONG);

void InitBattleCard( SHORT UnitIndex );
void InitBattleCard_alt(LONG, LONG);
void PaintBattleCard( LONG MenuCombo, LONG );
void BattleCardProc( LONG, LONG );

void GameButtonPaint( LONG MenuCombo, LONG );
void GameButtonPressed(LONG, LONG);
void SetStatusRealm(REALM::REALM_TYPE realm);

void NeverOthersDiplomacy(LONG, LONG);
void YesOthersDiplomacy(LONG, LONG);
void NoOthersDiplomacy(LONG, LONG);
void PaintOthersDiplomacy(LONG MenuCombo, LONG);

void OpenGameSelect(LONG, LONG);
void PaintGameSelect(LONG, LONG);
void SelectGameType(LONG, LONG);
void OpenGameComplexity(LONG, LONG);
void PaintGameComplexity(LONG, LONG);

void SelectMPGameType(LONG, LONG);
void PaintMPGameComplexity(LONG, LONG);


void PaintDescribe(LONG, LONG);
void IncDescribe(LONG, LONG);

void ShowHelpMenu( LONG, LONG );

/* battle only interface */
void BuildArmyPaint (LONG , LONG );
void BuildAutoArmy (LONG , LONG );
void BuildListSel (LONG , LONG );
void BuildListAdd (LONG , LONG );
void BuildListRemove (LONG , LONG );
void BuildListClear (LONG , LONG );
void BuildUnitSel (LONG , LONG );
void BuildTerrainSel (LONG , LONG );
void BuildArmyReload (LONG, LONG );
void BuildArmyDone (LONG, LONG );
void BuildArmyRestart (LONG, LONG );
void BuildArmyRestartPaint(LONG , LONG );
void BuildArmyRestartProc (LONG, LONG );
void BuildDisplayToggle(LONG, LONG);
void SetScenario(LONG, LONG);

void BuildAdvPaint (LONG , LONG );
void BuildAdvDone (LONG , LONG );
void BuildAdvRestart (LONG , LONG );
void BuildAdvRestartPaint (LONG , LONG );
void BuildAdvRestartProc (LONG , LONG );
void BuildAdvParty (LONG , LONG );
void BuildAdvListSel (LONG , LONG );

void RunAdvOnly(void);
void RunBattlesOnly(void);


// multiui.cpp 

void ShowMultiPlayerMenu(LONG, LONG);
void PaintMultiPlayerMenu(LONG MenuCombo, LONG);



void BtlHelp( int, int );
void PaintBattleHelp( LONG, LONG );
void BattleHelpProc( LONG, LONG );

void BackToGame(LONG, LONG);

void VictoryScreen(PFVLL, PFVLL );
void VictoryScreenProc(LONG , LONG );

void DefeatScreen(PFVLL NewPaintProc, PFVLL NewDoneProc);

void ShowAllAnimations(LONG, LONG);

void SystemHelp( CHAR *, LONG, PFVLL, LONG );
void SystemHelp( int, int, LONG, PFVLL, LONG );
void SystemHelp( CHAR *, CHAR *, LONG, PFVLL, LONG );


#if defined(__cplusplus)
extern "C" {
#endif
void init_game(void);

void LoadingSceneProc( LONG MenuCombo, LONG );
void SetLoadingProgress( LONG Percent );

void GAMEToggleAnimation(LONG /* unused */, LONG /* unused */ );
extern BOOL fAIAnimate;
extern SHORT iTroopUnit;

/* loadsave.cpp */
void PaintLoadSaveMenu(LONG, LONG);
void CancelSave(LONG, LONG);
void CancelLoad(LONG, LONG);
void LoadGameDo(LONG, LONG);
void SaveGameDo(LONG, LONG);
void DeleteGame(LONG, LONG);
#if !defined(_LOADSAVE_HXX)
void SaveGame(LONG, LONG);
void LoadGame(LONG, LONG);
#endif

/* gameopt.cpp */
void PaintOptionMenu(LONG, LONG);
void OpenOptions   (LONG, LONG);
void OptionCancel  (LONG, LONG);
void OptionDo      (LONG, LONG);
void OptionDefaults(LONG, LONG);
void OptionRadio1  (LONG, LONG);

/* main.cpp */
void CPPQuitSys(LONG, LONG);
void UpdateHighlight(LONG, LONG);

/* chat stuff */
void PaintChatInput(LONG, LONG);
void AddGlobalKeys(void);

void ShowChatWindow(LONG, LONG);
void ShowChatSelectWindow(LONG, LONG);

void SetChatPosition ( LONG l_map );
void ToggleChatDisplayWindow(LONG, LONG);
void ShowChatDisplayWindow(LONG, LONG);
void HideChatDisplayWindow(LONG, LONG);
void MoveChatDisplayWindow(LONG X, LONG Y);
void ChatLineClear();
void ChatLineAdd(CHAR *Text, SHORT Color);
void PaintChatDisplay(LONG, LONG);

void SystemHelpCover1(LONG, LONG);
void SystemHelpCover2(LONG, LONG);
void SystemHelpCover3(LONG, LONG);
void SystemHelpPaintProc( LONG, LONG );

#if defined(__cplusplus)
}
#endif
#endif // _GAME_H


