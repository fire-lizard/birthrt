/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _CHARSEL_HXX
#define _CHARSEL_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_REALM_HXX)
#include "realm.hxx"
#endif

#if !defined(_STRENUM_H)
#include "strenum.h"
#endif

#if !defined(_VECTOR_HXX)
#include "vector.hxx"
#endif
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

#define MIN_MODE        0
#define ACTN_MODE       1
#define MAINT_MODE      2
#define TAXLVL_MODE     3
#define LAWCLAIM_MODE   4
#define PAYRL_MODE      5
#define CHARSEL_MODE    6
#define ADVPARTY_MODE   7
#define INIT_REPORT_MODE		8
#define MAX_MODE        99


/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */


typedef enum
{
    DIF_EASY=0,
    DIF_MEDIUM,
    DIF_HARD
} DIFFICULTY;

typedef enum
{
    SPD_SLOW,
    SPD_MEDIUM,
    SPD_FAST,
    SPD_TURNS
} SPEED;

typedef enum
{
    ENV_OFF,
    ENV_ON
} ENV_SETTING;

typedef enum
{
	OPT_ADJ_GAME_DIF,
	OPT_ADJ_BATTLE_DIF,
	OPT_ADJ_BATTLE_SPEED,
	OPT_ADJ_ADVENTURE_DIF,
	OPT_ADJ_ADVENTURE_MODE,
	OPT_ADJ_ADVENTURE_SPEED,
	OPT_ADJ_ADVENTURE_VIEW,
	OPT_ADJ_SOUND_CHANNELS,
	OPT_ADJ_SOUND_MUSVOL,
	OPT_ADJ_SOUND_SFXVOL,
	OPT_ADJ_SOUND_ENV,
	OPT_ADJ_SCREEN_SIZE,
	OPT_ADJ_SCREEN_AUTORES,
	OPT_ADJ_SCREEN_WINDOW,
	OPT_ADJ_DESCRIPTIONS,
	OPT_ADJ_CONTROLMODE,
	OPT_ADJ_COUNT
} OPT_ADJ_TYPE;

/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Class Definition
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)

void DomainNewGame (LONG, LONG);
void DisplayRealmSelection(void);
void SelectRegent(LONG arg1, LONG);
void BeginRule(LONG, LONG);
void BeginRule2(LONG, LONG);
void SelectAdvParty(void);
void EndAdvParty(LONG, LONG);
void DisplayAdvParty(void);
void ReturnFromAdventure(LONG, LONG);
void PaintBattleMagicPrep(LONG, LONG);
void PaintMoreInfo (LONG, LONG);
void ToggleShowParty(LONG i, LONG);
void ShowPartyStatus(LONG i, LONG);
void SelectAdvSite(void);
void SetAdvSite(SHORT);
void SetAllAdvSite(void);
void SelectSite(LONG,LONG);
void SelectARealm(LONG, LONG);
void MultiSetAdvSite(void);
void NewGameConfirmed2(BOOL fMulti);

extern DECL_VECTOR_CLASS_S(LONG,whoIndex,40);
extern DECL_VECTOR_CLASS_S(LONG,lRealmSelectedByPlayer,LAND_REALM_COUNT);

extern DECL_VECTOR_CLASS_S(ADV_SITE,advsite,MAX_ADV_SITES);
extern DECL_VECTOR_CLASS(CSTRPTR,szRealmTextFile);

extern DECL_MATRIX_CLASS_S(GAME_STRING,TypesDiffs,LAND_REALM_COUNT,2);

extern BOOL	fRealmSelected;
extern LONG iDomainCharInfo;
extern int		iNewSite;

extern DIFFICULTY		game_difficulty;
extern DIFFICULTY		battle_difficulty;
extern DIFFICULTY		adventure_difficulty;
extern SPEED			battle_speed;
#endif


/* ------------------------------------------------------------------------
   Inline functions   
   ------------------------------------------------------------------------ */
   
#endif // _CHARSEL_HXX
