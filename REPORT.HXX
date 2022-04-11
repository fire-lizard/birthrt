/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _REPORT_HXX
#define _REPORT_HXX

/* -----------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_REALM_HXX)
#include "realm.hxx"
#endif

#if !defined(_UNITS_HXX)
#include "units.hxx"
#endif

#if !defined(_VEcTOR_HXX)
#include "vector.hxx"
#endif


/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define NO_ARRAY		0
#define REGENT_NAME		1
#define PLACE_NAME		2
#define PROV_NAME		3
#define REALM_NAME		4
#define HOLDING_TYP		5
#define NUM_DIGITS		6
#define NUMBER_WORD		7
#define DEC_INC			8
#define LOY_LEVEL		9
#define UNIT_TITLE		10
#define ALLY_LEVEL		11
#define IMMEDIATE	1024


/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */

	typedef enum {
		NO_SUBJECT = 0,
		SUBJ_INITIAL,
		SUBJ_DECLARE_WAR,
		SUBJ_REBELLION,
		SUBJ_REBEL_SEVERE,
		SUBJ_REBELLION_ENDS,
		SUBJ_CONTESTED,
		SUBJ_FOREIGN_HOLDING,
		SUBJ_FOREIGN_OCCUP,
		SUBJ_SELF_OCCUP,
		SUBJ_RULE_PROV,
		SUBJ_RULE_HOLD,
		SUBJ_RULE_PR_FAIL,
		SUBJ_RULE_HLD_FAIL,
		SUBJ_AGITATE,
		SUBJ_AGITATE_FAIL,
		SUBJ_DISBAND_UNIT,
		SUBJ_CREATE_HOLD,
		SUBJ_CREATE_HLD_FAIL,
		SUBJ_FORTIFY,
		SUBJ_CONTEST,
		SUBJ_CONTEST_FAIL,
		SUBJ_BUILDROAD,
		SUBJ_TRADE_RT,
		SUBJ_TRD_RT_FAIL,
		SUBJ_LEY_LINE,
		SUBJ_LEYLINE_FAIL,
		SUBJ_CONTEST_DESTROYED,
		SUBJ_LIEUTENANT,
		SUBJ_INVESTITURE,
		SUBJ_INVESTITURE_FAIL,
		SUBJ_MUSTER,
		SUBJ_SPY,
		SUBJ_SPY_FAIL,
		SUBJ_ASSASSIN,
		SUBJ_ASSASSIN_FAIL,
		SUBJ_DEATH_PLAGUE,
		SUBJ_DEMAGOGUE,
		SUBJ_LEGION_OF_DEAD,
		SUBJ_MASS_DESTRUCTION,
		SUBJ_RAZE_DESTROY,
		SUBJ_RAZE,
		SUBJ_SUMMONING,
		SUBJ_TRANSPORT,
		SUBJ_BLESS_LAND,
		SUBJ_BLIGHT,
		SUBJ_WARDING,
		SUBJ_HONEST,
		SUBJ_BLESS_ARMY,
		SUBJ_DISPEL_REALM_MAGIC,
		SUBJ_THEFT,
		SUBJ_EMBEZZLE,
		SUBJ_PARTY,
		SUBJ_STRONGHOLD1,
		SUBJ_STRONGHOLD2,
		SUBJ_ASSASSIN_AI,
		SUBJ_ALLIANCE_DEG,
		SUBJ_VASSAL_LEAVE,
		SUBJ_FORTIFY_COMPLETE,
		SUBJ_NPC_DECLARE_WAR,
		SUBJ_NPC_DECLARE_WAR_ALLY,
		SUBJ_NPC_MOVE_TROOP,
		SUBJ_NPC_MOVE_TROOP_ALLY,
		SUBJ_NPC_DEATH_PLAGUE,
		SUBJ_NPC_DEMAGOGUE,
		SUBJ_NPC_LEGION_OF_DEAD,
		SUBJ_NPC_MASS_DESTRUCTION,
		SUBJ_NPC_RAZE,
		SUBJ_NPC_SUMMONING,
		SUBJ_NPC_TRANSPORT,
		SUBJ_NPC_BLIGHT,
		SUBJ_NPC_WARDING,
		SUBJ_NPC_STRONGHOLD,
		SUBJ_NPC_ASSASSIN,
		SUBJ_NPC_CONTEST_DESTROY,
		SUBJ_ALLY_DECLARE_WAR,
		SUBJ_USED_FREEACTION,
		SUBJ_LEVELUP,
		SUBJ_NPC_DECLARE_WAR_ALONE,
		SUBJ_OTHER_REDUCEHOLDING,
		SUBJ_ALLIES_JOIN_WAR,
		
		SUBJ_COUNT,

		IMMEDIATE_INITIAL = IMMEDIATE + 1,
		IMMEDIATE_DECLARE_WAR,
		IMMEDIATE_REBELLION,
		IMMEDIATE_REBEL_SEVERE,
		IMMEDIATE_REBELLION_ENDS,
		IMMEDIATE_CONTESTED,
		IMMEDIATE_FOREIGN_HOLDING,
		IMMEDIATE_FOREIGN_OCCUP,
		IMMEDIATE_SELF_OCCUP,
		IMMEDIATE_RULE_PROV,
		IMMEDIATE_RULE_HOLD,
		IMMEDIATE_RULE_PR_FAIL,
		IMMEDIATE_RULE_HLD_FAIL,
		IMMEDIATE_AGITATE,
		IMMEDIATE_AGITATE_FAIL,
		IMMEDIATE_DISBAND_UNIT,
		IMMEDIATE_CREATE_HOLD,
		IMMEDIATE_CREATE_HLD_FAIL,
		IMMEDIATE_FORTIFY,
		IMMEDIATE_CONTEST,
		IMMEDIATE_CONTEST_FAIL,
		IMMEDIATE_BUILDROAD,
		IMMEDIATE_TRADE_RT,
		IMMEDIATE_TRD_RT_FAIL,
		IMMEDIATE_LEY_LINE,
		IMMEDIATE_LEYLINE_FAIL,
		IMMEDIATE_CONTEST_DESTROYED,
		IMMEDIATE_LIEUTENANT,
		IMMEDIATE_INVESTITURE,
		IMMEDIATE_INVESTITURE_FAIL,
		IMMEDIATE_MUSTER,
		IMMEDIATE_SPY,
		IMMEDIATE_SPY_FAIL,
		IMMEDIATE_ASSASSIN,
		IMMEDIATE_ASSASSIN_FAIL,
		IMMEDIATE_DEATH_PLAGUE,
		IMMEDIATE_DEMAGOGUE,
		IMMEDIATE_LEGION_OF_DEAD,
		IMMEDIATE_MASS_DESTRUCTION,
		IMMEDIATE_RAZE_DESTROY,
		IMMEDIATE_RAZE,
		IMMEDIATE_SUMMONING,
		IMMEDIATE_TRANSPORT,
		IMMEDIATE_BLESS_LAND,
		IMMEDIATE_BLIGHT,
		IMMEDIATE_WARDING,
		IMMEDIATE_HONEST,
		IMMEDIATE_BLESS_ARMY,
		IMMEDIATE_DISPEL_REALM_MAGIC,
		IMMEDIATE_THEFT,
		IMMEDIATE_EMBEZZLE,
		IMMEDIATE_PARTY,
		IMMEDIATE_STRONGHOLD1,
		IMMEDIATE_STRONGHOLD2,
		IMMEDIATE_ASSASSIN_AI,
		IMMEDIATE_ALLIANCE_DEG,
		IMMEDIATE_VASSAL_LEAVE,
		IMMEDIATE_FORTIFY_COMPLETE,
		IMMEDIATE_NPC_DECLARE_WAR,
		IMMEDIATE_NPC_DECLARE_WAR_ALLY,
		IMMEDIATE_NPC_MOVE_TROOP,
		IMMEDIATE_NPC_MOVE_TROOP_ALLY,
		IMMEDIATE_NPC_DEATH_PLAGUE,
		IMMEDIATE_NPC_DEMAGOGUE,
		IMMEDIATE_NPC_LEGION_OF_DEAD,
		IMMEDIATE_NPC_MASS_DESTRUCTION,
		IMMEDIATE_NPC_RAZE,
		IMMEDIATE_NPC_SUMMONING,
		IMMEDIATE_NPC_TRANSPORT,
		IMMEDIATE_NPC_BLIGHT,
		IMMEDIATE_NPC_WARDING,
		IMMEDIATE_NPC_STRONGHOLD,
		IMMEDIATE_NPC_ASSASSIN,
		IMMEDIATE_NPC_CONTEST_DESTROY,
		IMMEDIATE_ALLY_DECLARE_WAR,
		IMMEDIATE_USED_FREEACTION,
		IMMEDIATE_LEVELUP,
		IMMEDIATE_NPC_DECLARE_WAR_ALONE,
		IMMEDIATE_REDUCEHOLDING,
		IMMEDIATE_ALLIES_JOIN_WAR,
		IMMEDIATE_DEATHNOTIFY,
	} REPORT_SUBJECTS;

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
typedef struct {
	unsigned int		subj_class	: 8;
	unsigned int		array1		: 8;
	unsigned int		array2		: 8;
	unsigned int		array3		: 8;
//	CSTRPTR				sz[5];
	LONG				sz[5];
} SUBJECT;

typedef struct {
	unsigned int		subject		: 10;
	unsigned int		index1		: 10;
	unsigned int		index2		: 8;
	unsigned int		index3		: 8;
	unsigned int		imm			: 2;
} REPORT;

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
void ClearOldReports(void);
void AddReport(LONG,LONG,LONG,LONG);
void AddReportTo(LONG,LONG,LONG,LONG,LONG);
void SendReportTo(LONG,LONG,LONG,LONG,LONG);
void GiveReport(LONG lArg);
void CheckForImmediateReports(void);
void PaintLoyaltyTable (LONG, LONG);
void PaintProvinceLose(LONG MenuCombo, LONG);
void PaintRegentLose(LONG MenuCombo, LONG);
void ProvinceLooseProc(LONG, LONG);
void PaintReportMenu (LONG MenuCombo, LONG);

/* ------------------------------------------------------------------------
	Global Variables
   ------------------------------------------------------------------------ */
extern DECL_VECTOR_CLASS_S(SHORT,sScores,LAND_REALM_COUNT);
extern DECL_VECTOR_CLASS(SHORT,sProvinces);
extern SHORT	sMyProvinces;
extern LONG		iOldSite;

#endif // _REPORT_HXX

/*	======================================================================== */


