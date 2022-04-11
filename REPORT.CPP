/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: REPORT.cpp  -
   Author:   Alan Clark

   ========================================================================
   Contains the following general functions:

   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#ifdef _WINDOWS
#include <windows.h>
#include "winsys\mulplay.hxx"
#include "winsys\mono_c.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "system.h"
#include "engine.h"

#include "charsel.hxx"
#include "game.h"
#include "menu.h"
#include "realm.hxx"
#include "strenum.h"
#include "units.hxx"
#include "sound.hxx"
#include "vector.hxx"
#include "gmenuenm.h"
#include "flicplay.hxx"
#include "report.hxx"
#include "regents.hxx"
#include "places.hxx"
#include "gamemap.hxx"

extern "C" void RandomLogComment ( char * szString );
extern "C" void RandomLogPrefix ( char * szString );


extern "C" SHORT print_textf_(LONG x, LONG y, LONG color, const char *format, ...);

extern void ReconstructMap(void);
extern void UpdateHoldingFlag (void);


/* -----------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define BLACK						1
#define WHITE						31
#define DKBLUE						60
#define BLUE						64
#define LTBLUE						79
#define DKPURPLE					90
#define LTPURPLE					103
#define DKRED						112
#define MDRED						120
#define LTRED						127
#define DKBROWN					128
#define MDBROWN					136
#define LTBROWN					140
#define ORANGE						142
#define MDYELLOW					168
#define LTYELLOW					175
#define DKGREEN					184
#define MDGREEN					188
#define LTGREEN					192
#define BRIGHTGREEN				199
#define MDTAN						213
#define LTTAN						223

#define INIT_REPORT_MODE		8
#define REPORT_MODE				9
#define REPORT_DONE_MODE		10

#define REPORT_BUFFER_SIZE		4096

#define REPORTTIMEOUT 10000L

#define INTRO_MOVE_AMOUNT 5

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
DEFINE_STATIC_VECTOR_DATA_S(SimpleFLC,advisorFLCs,6);
DEFINE_STATIC_VECTOR_CLASS(SimpleFLC,advisorFLCs);

DEFINE_STATIC_VECTOR_DATA_S(LONG,advisortimer,6) = {0,0,0,0,0,0};
DEFINE_STATIC_VECTOR_CLASS(LONG,advisortimer);

static DECL_VECTOR_DATA(REPORT, report,100);
static SHORT	iNextReport = 0;
static SHORT	iNewReports = 0;
static SHORT	iWhosReport = 0;
static SHORT	iWhichFace = 0;
static SHORT	iWhichReport = 0;
static DECL_VECTOR_DATA(SHORT,sUnits,LAND_REALM_COUNT);
static DECL_VECTOR_DATA(SHORT,sHoldings,LAND_REALM_COUNT);
DECL_VECTOR_DATA(SHORT,sProvinces,LAND_REALM_COUNT);
DECL_VECTOR_DATA(SHORT,sScores,LAND_REALM_COUNT);
static SHORT	newCount;
static SHORT	startCount;
static SHORT	sMyUnits = 0;
SHORT			sMyProvinces = 0;
BOOL			AdvisorReportOn = FALSE;
static SHORT	sMyHoldings = 0;
static SHORT	sMyScore = 0;
static SHORT	sMyVassals = 0;
static SHORT	sMyFull = 0;
static SHORT	sMyPermissive = 0;
static SHORT	sMyOther = 0;

static SHORT	ReportBkgndBitm;
static SHORT	iScrollBitm[5];
static SHORT	iDoneBitm;

static SHORT	oldCountBuilt = FALSE;

//static DECL_VECTOR_DATA(char,n,REPORT_BUFFER_SIZE);
//static DECL_VECTOR_DATA(char,n1,REPORT_BUFFER_SIZE);
//static DECL_VECTOR_DATA(char,szReport,REPORT_BUFFER_SIZE * 2);

static char n[REPORT_BUFFER_SIZE];
static char n1[REPORT_BUFFER_SIZE];
static char szReport[REPORT_BUFFER_SIZE * 2];

static int		iCurSubj;
static BOOL		sndPlayed = FALSE;
static SHORT	introPlaying = -1;
static LONG		DoneTimer = 0;

#define CHAMBER		0
#define GENERAL		100
#define WIZARD		200
#define SPY			300

DEFINE_MATRIX_DATA_S(UBYTE,oldRealmReaction,REALM::REALM_COUNT,LAND_REALM_COUNT) = {

//								  N              B                                   G                                   S           T
//								  O              A     B     C                 F     O                                   P           U
//								                 R  B  R     H  C  D           I     R     M           M  M           S  I        T  A
//								  C  A           U  O  O  C  I  O  H           V     G     A  M  M     O  U  O  R  R  I  D  T  T  H  R  T
//								  O  E  A  A  A  K  E  S  A  M  E  O  D  E  E  E  G  O     R  E  H  M  N  R  S  H  O  E  E  A  A  U  H  U
//								  U  R  L  N  V  A  R  E  R  A  R  E  I  L  N  P  H  N  I  K  D  O  I  S  K  O  U  E  L  R  E  L  R  I  O
//								  N  E  A  U  A  Z  U  N  I  E  A  S  E  I  D  E  O  S  L  A  O  R  E  T  I  E  O  S  W  F  G  I  A  E  R
//								  T  N  M  I  N  H  I  G  E  R  N  O  M  N  I  A  E  C  I  Z  E  I  R  E  L  R  B  O  O  E  H  N  Z  V  N
//								  R  W  I  R  I  I  N  A  L  O  Y  N  E  I  E  K  R  R  E  O  R  E  E  R  E  D  H  N  D  L  A  I  O  E  E
//								  Y  E  E  E  L  K  E  E  E  N  S  E  D  E  R  S  E  N  N  R  E  D  S  S  D  E  E  E  E  L  S  E  R  L  N
//								    NG NE LN LN NG LN NE NE CE CG NN LN LG NE CE LE LE NN LE CG CG    CE LE LE NE NN NN CE NN LG LE NN NN
/* NO_COUNTRY				*/	{99,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50},
/* AERENWE					*/	{50,99,40,50,50,60,50,40,40,40,60,50,50,60,40,40,40,30,50,30,60,60,50,30,30,40,40,70,50,40,50,60,40,50,50},
/* ALAMIE					*/	{50,40,99,50,50,50,50,50,40,50,40,50,50,40,50,50,50,40,50,40,40,40,50,50,40,50,50,50,50,40,50,50,50,50,40},
/* ANUIRE					*/	{50,60,50,99,50,60,50,50,40,60,60,50,50,60,50,50,50,30,50,30,60,60,50,30,30,50,40,50,50,40,50,60,40,50,50},
/* AVANIL					*/	{50,50,40,50,99,50,40,90,40,50,50,50,40,50,50,50,50,40,50,40,50,50,50,30,40,50,40,50,50,40,90,40,50,50,50},
/* BARUKAZHIK				*/	{50,50,50,50,50,99,50,50,40,40,50,60,50,50,50,40,50,30,50,30,50,50,50,30,30,50,40,50,50,40,50,50,50,50,50},
/* BOERUINE					*/	{50,50,70,50,40,50,99,40,40,50,50,50,50,50,50,50,50,40,50,40,50,50,50,30,40,50,40,50,50,40,40,80,50,50,50},
/* BROSENGAE				*/	{50,40,50,50,90,40,50,99,40,50,40,50,50,40,50,50,50,40,50,40,40,40,50,30,40,50,50,50,50,40,50,40,50,50,50},
/* CARIELE					*/	{50,40,40,40,40,40,40,40,99,40,40,40,40,40,40,40,40,30,40,30,40,40,40,30,30,40,40,40,40,40,40,40,40,40,40},
/* CHIMAERON				*/	{50,40,40,40,40,40,40,40,40,99,40,40,40,40,40,40,40,30,40,30,40,40,40,80,30,40,40,40,40,40,40,40,40,40,40},
/* COERANYS					*/	{50,60,40,50,50,70,50,40,40,40,99,50,50,60,40,40,40,30,50,30,60,60,50,30,30,40,40,50,50,40,50,60,40,50,50},
/* DHOESONE					*/	{50,60,40,50,50,60,50,40,40,40,60,99,50,60,40,40,40,30,50,30,60,60,50,30,30,40,40,50,60,40,50,60,70,80,50},
/* DIEMED					*/	{50,50,50,50,40,50,50,50,40,50,50,50,99,50,50,50,50,40,50,40,50,50,50,30,40,50,50,50,50,40,50,50,50,50,50},
/* ELINIE					*/	{50,60,40,50,50,60,50,40,40,40,60,50,50,99,40,40,40,30,50,30,60,60,50,30,30,40,40,50,50,40,50,60,40,50,50},
/* ENDIER					*/	{50,40,50,50,50,40,50,50,40,50,40,50,50,40,99,50,50,40,50,40,40,40,50,30,40,50,50,50,50,40,50,40,50,50,50},
/* FIVEPEAKS				*/	{50,40,40,40,40,40,40,40,40,40,40,40,40,40,40,99,40,30,40,30,40,40,40,80,30,40,40,40,40,40,40,40,40,40,40},
/* GHOERE					*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,99,40,50,40,50,40,50,30,40,50,50,50,50,40,50,50,50,50,50},
/* GORGONSCROWN				*/	{50,30,40,30,30,30,30,40,40,40,30,30,30,30,40,40,40,99,30,90,30,30,30,40,90,40,40,30,30,40,30,30,40,10,30},
/* ILIEN					*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,50,40,99,40,70,50,50,30,40,50,50,70,50,40,50,50,50,50,50},
/* MARKAZOR					*/	{50,20,50,20,40,30,40,50,50,50,30,40,40,30,50,50,50,90,40,99,30,30,40,20,50,50,50,40,40,50,40,30,60,40,40},
/* MEDOERE					*/	{50,60,40,50,50,60,50,40,40,40,60,50,40,60,40,40,40,30,70,50,99,60,50,30,30,40,40,70,50,40,50,60,40,50,50},
/* MHORIED					*/	{50,60,40,60,50,60,50,40,60,40,60,50,50,60,40,40,40,30,50,30,60,99,50,30,30,40,40,50,50,40,50,60,40,50,50},
/* MIERES					*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,50,40,50,40,50,50,99,30,40,50,50,50,50,50,50,50,50,50,50},
/* MONSTERS					*/	{50,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,99,30,30,30,30,30,30,30,30,30,30,30},
/* MURKILAD					*/	{50,30,50,40,40,20,40,50,30,50,30,40,40,30,50,50,50,90,40,50,30,30,40,50,99,50,50,40,10,50,40,30,50,40,40},
/* OSOERDE					*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,50,40,50,40,50,50,50,30,40,99,50,50,50,50,50,50,50,50,50},
/* RHUOBHE					*/	{50,30,30,30,30,40,30,30,30,30,30,30,30,30,30,30,30,40,30,30,30,30,30,30,30,30,99,30,70,50,30,30,70,30,30},
/* ROESONE					*/	{50,60,40,50,50,50,50,40,40,40,60,50,40,60,40,40,40,40,70,30,70,60,50,30,30,40,40,99,50,40,50,60,40,50,50},
/* SIELWODE					*/	{50,50,50,50,50,50,50,50,40,50,50,60,50,50,50,50,50,30,50,30,50,50,50,30,10,50,60,50,99,40,50,50,40,70,50},
/* SPIDERFELL				*/	{50,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,70,30,30,99,30,30,30,30,30},
/* TAEGHAS					*/	{50,50,40,50,90,50,40,40,40,40,50,50,50,50,40,40,40,30,50,30,50,50,50,30,30,40,50,50,50,40,99,50,40,50,50},
/* TALINIE					*/	{50,50,50,50,40,50,80,50,40,50,50,50,50,50,50,50,50,40,50,40,50,50,50,30,40,50,50,50,50,40,50,99,50,50,50},
/* THURAZOR					*/	{50,40,40,40,40,40,40,40,40,40,40,70,40,40,40,40,40,50,50,50,40,40,40,30,30,40,40,40,40,40,40,40,99,40,40},
/* TUARHIEVEL				*/	{50,50,50,50,50,50,50,50,40,50,50,80,50,50,50,50,50,10,50,40,50,50,50,30,40,50,60,50,70,40,50,50,50,99,50},
/* TUORNEN					*/	{50,50,40,50,40,50,40,50,40,50,50,50,50,50,50,50,50,30,50,40,50,50,50,30,40,50,40,50,50,40,50,50,50,50,99},

//							  N              B                                   G                                   S           T
//							  O              A     B     C                 F     O                                   P           U
//							                 R  B  R     H  C  D           I     R     M           M  M           S  I        T  A
//							  C  A           U  O  O  C  I  O  H           V     G     A  M  M     O  U  O  R  R  I  D  T  T  H  R  T
//							  O  E  A  A  A  K  E  S  A  M  E  O  D  E  E  E  G  O     R  E  H  M  N  R  S  H  O  E  E  A  A  U  H  U
//							  U  R  L  N  V  A  R  E  R  A  R  E  I  L  N  P  H  N  I  K  D  O  I  S  K  O  U  E  L  R  E  L  R  I  O
//							  N  E  A  U  A  Z  U  N  I  E  A  S  E  I  D  E  O  S  L  A  O  R  E  T  I  E  O  S  W  F  G  I  A  E  R
//							  T  N  M  I  N  H  I  G  E  R  N  O  M  N  I  A  E  C  I  Z  E  I  R  E  L  R  B  O  O  E  H  N  Z  V  N
//							  R  W  I  R  I  I  N  A  L  O  Y  N  E  I  E  K  R  R  E  O  R  E  E  R  E  D  H  N  D  L  A  I  O  E  E
//							  Y  E  E  E  L  K  E  E  E  N  S  E  D  E  R  S  E  N  N  R  E  D  S  S  D  E  E  E  E  L  S  E  R  L  N
//							    NG NE LN LN NE LN NE NE CE CG NN LN LG NE CE LE LE NN LE CG CG    CE LE LE NE NN NN CE NN LG LE NN NN
/* N_IMPORTS_EXPORTS	*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,50,40,50,40,50,50,50,30,50,50,50,50,50,40,50,50,40,50,50},
/* IMPREGNABLE_HEART_OF_*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,50,40,50,40,50,50,50,30,50,50,50,50,50,40,50,50,40,50,50},
/* S_ANUIRE_SHIPPING_AND*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,50,40,50,40,50,50,50,30,50,50,50,50,50,40,50,50,40,50,50},
/* HIGH_MAGE_AELIES		*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,50,40,50,40,50,50,50,30,50,50,50,50,50,40,50,50,40,50,50},
/* PORT_OF_CALL_EXCHANGE*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,50,40,50,40,50,50,50,30,50,50,50,50,50,40,50,50,40,50,50},
/* ORTHODOX_IMPERIAL_TEM*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,50,40,50,40,50,50,50,30,50,50,50,50,50,40,50,50,40,50,50},
/* WIZARD_HERMEDHIE		*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,50,40,50,40,50,50,50,30,50,50,50,50,50,40,50,50,40,50,50},
/* CELESTIAL_JEWEL_OF_SA*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,50,40,50,40,50,50,50,30,50,50,50,50,50,40,50,50,40,50,50},
/* HIDDEN_TEMPLE_OF_CUIR*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,50,40,50,40,50,50,50,30,50,50,50,50,50,40,50,50,40,50,50},
/* UPPER_ANUIRE_TRADERS	*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,50,40,50,40,50,50,50,30,50,50,50,50,50,40,50,50,40,50,50},
/* HAELYNS_AEGIS	  	*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,50,40,50,40,50,50,50,30,50,50,50,50,50,40,50,50,40,50,50},
/* MILITANT_ORDER_OF_CUI*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,50,40,50,40,50,50,50,30,50,50,50,50,50,40,50,50,40,50,50},
/* POINTS_EAST_TRADING_G*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,50,40,50,40,50,50,50,30,50,50,50,50,50,40,50,50,40,50,50},
/* WIZARD_SECOND_SWAMP_M*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,50,40,50,40,50,50,50,30,50,50,50,50,50,40,50,50,40,50,50},
/* N_REFORMED_CHURCH_OF_*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,50,40,50,40,50,50,50,30,50,50,50,50,50,40,50,50,40,50,50},
/* WIZARD_EYELESS_ONE	*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,50,40,50,40,50,50,50,30,50,50,50,50,50,40,50,50,40,50,50},
/* RUARCH_ROCKHAMMER  	*/	{50,50,50,50,50,50,50,50,40,50,50,50,50,50,50,50,50,40,50,40,50,50,50,30,50,50,50,50,50,40,50,50,40,50,50},
};

DEFINE_MATRIX_CLASS_S(UBYTE, oldRealmReaction, REALM::REALM_COUNT, LAND_REALM_COUNT);

DEFINE_VECTOR_DATA(SUBJECT,Subject) = {
						{ 0, NO_ARRAY,   NO_ARRAY,   NO_ARRAY,   {STR_NULL,STR_NULL,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_INITIAL*/		{CHAMBER +  1, NO_ARRAY,   NO_ARRAY,   NO_ARRAY,   {STR_NULL,STR_NULL,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_DECLARE_WAR*/	{GENERAL +  2, REALM_NAME, NO_ARRAY,   NO_ARRAY,   {STR_RPT_DECLARE_WAR,STR_RPT_DECLARE_WAR,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_REBELLION*/		{GENERAL +  3, PROV_NAME,  NO_ARRAY,   NO_ARRAY,   {STR_RPT_REBELLION1,STR_RPT_REBELLION2, STR_RPT_REBELLION3, STR_RPT_REBELLION4, STR_RPT_PERIOD}},
/*SUBJ_REBEL_SEVERE*/	{GENERAL +  4, PROV_NAME,  NO_ARRAY,   NO_ARRAY,   {STR_RPT_REBEL_SEVERE1, STR_RPT_REBEL_SEVERE2, STR_RPT_REBEL_SEVERE3, STR_RPT_REBEL_SEVERE4, STR_RPT_REBEL_SEVERE5}},
/*SUBJ_REBELLION_ENDS*/	{GENERAL +  5, PROV_NAME,  NO_ARRAY,   NO_ARRAY,   {STR_RPT_REBELLION_ENDS1, STR_RPT_REBELLION_ENDS2, STR_RPT_REBELLION_ENDS3, STR_RPT_REBELLION_ENDS4, STR_RPT_REBELLION_ENDS5}},
/*SUBJ_CONTESTED*/		{CHAMBER +  6, PROV_NAME,  REALM_NAME, NO_ARRAY,   {STR_RPT_CONTESTED1, STR_RPT_CONTESTED2, STR_RPT_CONTESTED3, STR_RPT_CONTESTED4, STR_RPT_CONTESTED5}},
/*SUBJ_FOREIGN_HOLDING*/{CHAMBER +  7, REALM_NAME, HOLDING_TYP,PROV_NAME,  {STR_RPT_FOREIGN_HOLDING1, STR_RPT_FOREIGN_HOLDING2, STR_RPT_FOREIGN_HOLDING3, STR_RPT_FOREIGN_HOLDING4, STR_RPT_FOREIGN_HOLDING5}},
/*SUBJ_FOREIGN_OCCUP*/	{GENERAL +  8, REALM_NAME, PROV_NAME,  NO_ARRAY,   {STR_RPT_FOREIGN_OCCUP1, STR_RPT_FOREIGN_OCCUP2, STR_RPT_FOREIGN_OCCUP3, STR_RPT_FOREIGN_OCCUP4, STR_RPT_FOREIGN_OCCUP5}},
/*SUBJ_SELF_OCCUP*/		{GENERAL +  9, PROV_NAME,  NO_ARRAY,   NO_ARRAY,   {STR_RPT_SELF_OCCUP1, STR_RPT_SELF_OCCUP2, STR_RPT_SELF_OCCUP3, STR_RPT_SELF_OCCUP4, STR_RPT_SELF_OCCUP5}},
/*SUBJ_RULE_PROV*/		{CHAMBER + 10, PROV_NAME,  NUM_DIGITS, NO_ARRAY,   {STR_RPT_RULE_PROV1, STR_RPT_RULE_PROV2, STR_RPT_RULE_PROV3, STR_RPT_RULE_PROV4, STR_RPT_PERIOD}},
/*SUBJ_RULE_HOLD*/		{CHAMBER + 11, HOLDING_TYP,PROV_NAME,  NUM_DIGITS, {STR_RPT_RULE_HOLD1, STR_RPT_RULE_HOLD2, STR_RPT_RULE_HOLD3, STR_RPT_RULE_HOLD4, STR_RPT_PERIOD}},
/*SUBJ_RULE_PR_FAIL*/	{CHAMBER + 12, PROV_NAME,  NO_ARRAY,   NO_ARRAY,   {STR_RPT_RULE_PR_FAIL1, STR_RPT_RULE_PR_FAIL2, STR_RPT_RULE_PR_FAIL3, STR_RPT_RULE_PR_FAIL4, STR_RPT_RULE_PR_FAIL5}},
/*SUBJ_RULE_HLD_FAIL*/	{CHAMBER + 13, HOLDING_TYP,PROV_NAME,  NO_ARRAY,   {STR_RPT_RULE_HLD_FAIL1, STR_RPT_RULE_HLD_FAIL2, STR_RPT_RULE_HLD_FAIL3, STR_RPT_RULE_HLD_FAIL4, STR_RPT_RULE_HLD_FAIL5}},
/*SUBJ_AGITATE*/		{GENERAL + 14, DEC_INC,    PROV_NAME,  LOY_LEVEL,  {STR_RPT_AGITATE1, STR_RPT_AGITATE2, STR_RPT_AGITATE3, STR_RPT_AGITATE4, STR_RPT_PERIOD}},
/*SUBJ_AGITATE_FAIL*/	{GENERAL + 15, PROV_NAME,  NO_ARRAY,   NO_ARRAY,   {STR_RPT_AGITATE_FAIL1, STR_RPT_AGITATE_FAIL2, STR_RPT_AGITATE_FAIL3, STR_RPT_AGITATE_FAIL4, STR_RPT_AGITATE_FAIL5}},
/*SUBJ_DISBAND_UNIT*/	{GENERAL + 16, UNIT_TITLE, PROV_NAME,  NO_ARRAY,   {STR_RPT_DISBAND_UNIT1, STR_RPT_DISBAND_UNIT2, STR_RPT_DISBAND_UNIT3, STR_RPT_DISBAND_UNIT4, STR_RPT_PERIOD}},
/*SUBJ_CREATE_HOLD*/	{CHAMBER + 17, HOLDING_TYP,PROV_NAME,  REALM_NAME, {STR_RPT_CREATE_HOLD1, STR_RPT_CREATE_HOLD2, STR_RPT_CREATE_HOLD3, STR_RPT_CREATE_HOLD4, STR_RPT_PERIOD}},
/*SUBJ_CREATE_HLD_FAIL*/{CHAMBER + 18, HOLDING_TYP,PROV_NAME,  REALM_NAME, {STR_RPT_CREATE_HLD_FAIL1, STR_RPT_CREATE_HLD_FAIL2, STR_RPT_CREATE_HLD_FAIL3, STR_RPT_CREATE_HLD_FAIL4, STR_RPT_PERIOD}},
/*SUBJ_FORTIFY*/		{CHAMBER + 19, NUM_DIGITS, PROV_NAME,  NO_ARRAY,   {STR_RPT_FORTIFY1, STR_RPT_FORTIFY2, STR_RPT_FORTIFY3, STR_RPT_FORTIFY4, STR_RPT_PERIOD}},
/*SUBJ_CONTEST*/		{GENERAL + 20, REALM_NAME, HOLDING_TYP,PROV_NAME,  {STR_RPT_CONTEST1,STR_RPT_CONTEST2,STR_RPT_CONTEST3,STR_RPT_CONTEST4,STR_RPT_PERIOD}},
/*SUBJ_CONTEST_FAIL*/	{GENERAL + 21, PROV_NAME, NO_ARRAY, NO_ARRAY,      {STR_RPT_CONTEST_FAIL1,STR_RPT_CONTEST_FAIL2,STR_RPT_CONTEST_FAIL3,STR_RPT_CONTEST_FAIL4,STR_RPT_CONTEST_FAIL5}},
/*SUBJ_BUILDROAD*/		{CHAMBER + 22, PROV_NAME, PROV_NAME, NO_ARRAY,     {STR_RPT_BUILDROAD1, STR_RPT_BUILDROAD2, STR_RPT_BUILDROAD3, STR_RPT_BUILDROAD4, STR_RPT_BUILDROAD5}},
/*SUBJ_TRADE_RT*/		{CHAMBER + 23, PROV_NAME, PROV_NAME, NO_ARRAY,     {STR_RPT_TRADE_RT1, STR_RPT_TRADE_RT2, STR_RPT_TRADE_RT3, STR_RPT_TRADE_RT4, STR_RPT_PERIOD}},
/*SUBJ_TRD_RT_FAIL*/	{CHAMBER + 24, PROV_NAME, PROV_NAME, NO_ARRAY,     {STR_RPT_TRD_RT_FAIL1, STR_RPT_TRD_RT_FAIL2, STR_RPT_TRD_RT_FAIL3, STR_RPT_TRD_RT_FAIL4, STR_RPT_TRD_RT_FAIL5}},
/*SUBJ_LEY_LINE*/		{WIZARD  + 25, PROV_NAME, PROV_NAME, NO_ARRAY,     {STR_RPT_LEY_LINE1, STR_RPT_LEY_LINE2, STR_RPT_LEY_LINE3, STR_RPT_LEY_LINE4, STR_RPT_PERIOD}},
/*SUBJ_LEYLINE_FAIL*/	{WIZARD  + 26, PROV_NAME, PROV_NAME, NO_ARRAY,     {STR_RPT_LEYLINE_FAIL1, STR_RPT_LEYLINE_FAIL2, STR_RPT_LEYLINE_FAIL3, STR_RPT_LEYLINE_FAIL4, STR_RPT_LEYLINE_FAIL5}},
/*SUBJ_CONTEST_DESTROYED*/ 	{GENERAL + 27, PROV_NAME, NO_ARRAY, NO_ARRAY,  {STR_RPT_CONTEST_DESTROYED1,STR_RPT_CONTEST_DESTROYED2,STR_RPT_CONTEST_DESTROYED3,STR_RPT_CONTEST_DESTROYED4,STR_RPT_PERIOD}},
/*SUBJ_LIEUTENANT*/ 	{GENERAL + 28, REGENT_NAME, NO_ARRAY,   NO_ARRAY,  {STR_RPT_LIEUTENANT,STR_NULL,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_INVESTITURE*/ 	{CHAMBER + 29, PROV_NAME,   NO_ARRAY,   NO_ARRAY,  {STR_RPT_INVESTITURE,STR_NULL,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_INVESTITURE_FAIL*/ 	{CHAMBER + 30, NO_ARRAY,   NO_ARRAY,   NO_ARRAY,  {STR_RPT_INVESTITURE_FAIL,STR_NULL,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_MUSTER*/    	    {GENERAL + 31, NUM_DIGITS,   NO_ARRAY,  NO_ARRAY,  {STR_RPT_MUSTER,STR_NULL,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_SPY*/    	    {SPY + 32,     NUM_DIGITS,   NO_ARRAY,  NO_ARRAY,  {STR_RPT_SPY,STR_NULL,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_SPY_FAIL*/    	{SPY + 33,     NO_ARRAY,     NO_ARRAY,  NO_ARRAY,  {STR_RPT_SPY_FAIL,STR_NULL,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_ASSASSIN*/	    {SPY + 34,     UNIT_TITLE,   NO_ARRAY,  NO_ARRAY,  {STR_RPT_ASSASSIN,STR_NULL,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_ASSASSIN_FAIL*/  {SPY + 35,     NO_ARRAY,     NO_ARRAY,  NO_ARRAY,  {STR_RPT_ASSASSIN_FAIL,STR_NULL,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_DEATH_PLAGUE*/   {WIZARD + 36, DEC_INC,    PROV_NAME,   NUM_DIGITS,  {STR_RPT_DEATH_PLAGUE1,STR_RPT_DEATH_PLAGUE2,STR_RPT_DEATH_PLAGUE3,STR_RPT_DEATH_PLAGUE4,STR_RPT_PERIOD}},
/*SUBJ_DEMAGOGUE*/    	{WIZARD + 37, DEC_INC,    PROV_NAME,   LOY_LEVEL,  {STR_RPT_DEMAGOGUE1,STR_RPT_DEMAGOGUE2,STR_RPT_DEMAGOGUE3,STR_RPT_DEMAGOGUE4,STR_RPT_PERIOD}},
/*SUBJ_LEGION_OF_DEAD*/ {WIZARD + 38, PROV_NAME,    NO_ARRAY,  NO_ARRAY,  {STR_RPT_LEGION_OF_DEAD1,STR_RPT_LEGION_OF_DEAD2,STR_RPT_LEGION_OF_DEAD3,STR_RPT_LEGION_OF_DEAD4,STR_RPT_PERIOD}},
/*SUBJ_MASS_DESTRUCTION*/{WIZARD + 39, NUM_DIGITS,    PROV_NAME,  NO_ARRAY,  {STR_RPT_MASS_DESTRUCTION1,STR_RPT_MASS_DESTRUCTION2,STR_RPT_MASS_DESTRUCTION3,STR_RPT_MASS_DESTRUCTION4,STR_RPT_PERIOD}},
/*SUBJ_RAZE_DESTROY*/   {WIZARD + 40, PROV_NAME,    NO_ARRAY,  NO_ARRAY,  {STR_RPT_RAZE_DESTROY1,STR_RPT_RAZE_DESTROY2,STR_RPT_RAZE_DESTROY3,STR_RPT_RAZE_DESTROY4,STR_RPT_PERIOD}},
/*SUBJ_RAZE*/    	    {WIZARD + 41, PROV_NAME,    NUM_DIGITS,  NO_ARRAY,  {STR_RPT_RAZE1,STR_RPT_RAZE2,STR_RPT_RAZE3,STR_RPT_RAZE4,STR_RPT_PERIOD}},
/*SUBJ_SUMMONING*/    	{WIZARD + 42, NUM_DIGITS,    PROV_NAME,  NO_ARRAY,  {STR_RPT_SUMMONING1,STR_RPT_SUMMONING2,STR_RPT_SUMMONING3,STR_RPT_SUMMONING4,STR_RPT_PERIOD}},
/*SUBJ_TRANSPORT*/    	{WIZARD + 43, PROV_NAME,    PROV_NAME,  NO_ARRAY,  {STR_RPT_TRANSPORT1,STR_RPT_TRANSPORT2,STR_RPT_TRANSPORT3,STR_RPT_TRANSPORT4,STR_RPT_PERIOD}},
/*SUBJ_BLESS_LAND*/     {WIZARD + 44, PROV_NAME,   NO_ARRAY,  NO_ARRAY,  {STR_RPT_BLESS_LAND1,STR_RPT_BLESS_LAND2,STR_RPT_BLESS_LAND3,STR_RPT_BLESS_LAND4,STR_RPT_PERIOD}},
/*SUBJ_BLIGHT*/    	    {WIZARD + 45, PROV_NAME,   NO_ARRAY,  NO_ARRAY,  {STR_RPT_BLIGHT1,STR_RPT_BLIGHT2,STR_RPT_BLIGHT3,STR_RPT_BLIGHT4,STR_RPT_PERIOD}},
/*SUBJ_WARDING*/    	{WIZARD + 46, PROV_NAME,   NO_ARRAY,  NO_ARRAY,  {STR_RPT_WARDING1,STR_RPT_WARDING2,STR_RPT_WARDING3,STR_RPT_WARDING4,STR_RPT_PERIOD}},
/*SUBJ_HONEST*/    	    {WIZARD + 47, PROV_NAME,   NO_ARRAY,  NO_ARRAY,  {STR_RPT_HONEST1,STR_RPT_HONEST2,STR_RPT_HONEST3,STR_RPT_HONEST4,STR_RPT_PERIOD}},
/*SUBJ_BLESS_ARMY*/    	{WIZARD + 48, NUM_DIGITS,   PROV_NAME,  NO_ARRAY,  {STR_RPT_BLESS_ARMY1,STR_RPT_BLESS_ARMY2,STR_RPT_BLESS_ARMY3,STR_RPT_BLESS_ARMY4,STR_RPT_BLESS_ARMY5}},
/*SUBJ_DISPEL_REALM_MAGIC*/ {WIZARD + 49, PROV_NAME,   NO_ARRAY,  NO_ARRAY,  {STR_RPT_DISPEL_REALM_MAGIC1,STR_RPT_DISPEL_REALM_MAGIC2,STR_RPT_DISPEL_REALM_MAGIC3,STR_RPT_DISPEL_REALM_MAGIC4,STR_RPT_PERIOD}},
/*SUBJ_THEFT*/			{CHAMBER + 50, NUM_DIGITS,   NO_ARRAY,  NO_ARRAY,  {STR_RPT_THEFT,STR_NULL,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_EMBEZZLE*/		{CHAMBER + 51, NUM_DIGITS,   NO_ARRAY,  NO_ARRAY,  {STR_RPT_EMBEZZLE,STR_NULL,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_PARTY*/			{CHAMBER + 52, NUM_DIGITS,   NO_ARRAY,  NO_ARRAY,  {STR_RPT_PARTY,STR_NULL,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_STRONGHOLD1*/    	{WIZARD + 53, PROV_NAME,   NO_ARRAY,  NO_ARRAY,  {STR_RPT_STRONGHOLD11,STR_RPT_STRONGHOLD12,STR_RPT_STRONGHOLD13,STR_RPT_STRONGHOLD14,STR_RPT_PERIOD}},
/*SUBJ_STRONGHOLD2*/    	{WIZARD + 54, PROV_NAME,   NUM_DIGITS,  NO_ARRAY,  {STR_RPT_STRONGHOLD21,STR_RPT_STRONGHOLD22,STR_RPT_STRONGHOLD23,STR_RPT_STRONGHOLD24,STR_RPT_PERIOD}},
/*SUBJ_ASSASSIN_AI*/	    {SPY + 55, NUM_DIGITS,   PROV_NAME,  NO_ARRAY,  {STR_RPT_ASSASSIN_AI,STR_NULL,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_ALLIANCE_DEG*/		{GENERAL + 56, REALM_NAME,  NO_ARRAY,   NO_ARRAY,   {STR_RPT_ALLIANCE_DEG, STR_NULL,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_VASSAL_LEAVE*/		{GENERAL + 57, REALM_NAME,  NO_ARRAY,   NO_ARRAY,   {STR_RPT_VASSAL_LEAVE, STR_NULL,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_FORTIFY_COMPLETE*/   {CHAMBER + 58, PROV_NAME, NO_ARRAY, NO_ARRAY, {STR_RPT_FORTIFY_COMPLETE, STR_NULL, STR_NULL,STR_NULL, STR_NULL}},
/*SUBJ_NPC_DECLARE_WAR*/	{GENERAL +  59, REALM_NAME, NO_ARRAY,   NO_ARRAY,   {STR_RPT_NPC_DECLARE_WAR,STR_RPT_NPC_DECLARE_WAR,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_NPC_DECLARE_WAR_ALLY*/	{GENERAL +  60, REALM_NAME, ALLY_LEVEL,   REALM_NAME,   {STR_RPT_NPC_DECLARE_WAR_ALLY,STR_RPT_NPC_DECLARE_WAR_ALLY,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_NPC_MOVE_TROOP*/	{GENERAL +  61, REALM_NAME, PROV_NAME,   NO_ARRAY,   {STR_RPT_NPC_MOVE_TROOP,STR_RPT_NPC_MOVE_TROOP,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_NPC_MOVE_TROOP_ALLY*/	{GENERAL +  62, REALM_NAME, ALLY_LEVEL,   REALM_NAME,   {STR_RPT_NPC_MOVE_TROOP_ALLY,STR_RPT_NPC_MOVE_TROOP,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_NPC_DEATH_PLAGUE*/   {WIZARD + 63, REALM_NAME,    PROV_NAME,   NUM_DIGITS,  {STR_RPT_NPC_DEATH_PLAGUE,STR_RPT_NPC_DEATH_PLAGUE,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_NPC_DEMAGOGUE*/    	{WIZARD + 64, REALM_NAME,    PROV_NAME,   LOY_LEVEL,  {STR_RPT_NPC_DEMAGOGUE,STR_RPT_NPC_DEMAGOGUE, STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_NPC_LEGION_OF_DEAD*/ {WIZARD + 65, REALM_NAME,    PROV_NAME,  NO_ARRAY,  {STR_RPT_NPC_LEGION_OF_DEAD,STR_RPT_NPC_LEGION_OF_DEAD,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_NPC_MASS_DESTRUCTION*/{WIZARD + 66, REALM_NAME,    PROV_NAME,  NO_ARRAY,  {STR_RPT_NPC_MASS_DESTRUCTION,STR_RPT_NPC_MASS_DESTRUCTION,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_NPC_RAZE*/    	    {WIZARD + 67, REALM_NAME,   PROV_NAME,    NUM_DIGITS,  {STR_RPT_NPC_RAZE,STR_RPT_NPC_RAZE,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_NPC_SUMMONING*/    	{WIZARD + 68, REALM_NAME,   PROV_NAME,  NO_ARRAY, {STR_RPT_NPC_SUMMONING,STR_RPT_NPC_SUMMONING,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_NPC_TRANSPORT*/    	{WIZARD + 69, REALM_NAME,  PROV_NAME,    PROV_NAME,  {STR_RPT_NPC_TRANSPORT,STR_RPT_NPC_TRANSPORT,STR_NULL, STR_NULL,STR_NULL}},
/*SUBJ_NPC_BLIGHT*/    	    {WIZARD + 70, REALM_NAME, PROV_NAME,   NO_ARRAY,  {STR_RPT_NPC_BLIGHT,STR_RPT_NPC_BLIGHT,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_NPC_WARDING*/    	{WIZARD + 71, REALM_NAME, PROV_NAME,   NO_ARRAY,  {STR_RPT_NPC_WARDING,STR_RPT_NPC_WARDING,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_NPC_STRONGHOLD*/    	{WIZARD + 72, REALM_NAME, PROV_NAME,   NO_ARRAY,  {STR_RPT_NPC_STRONGHOLD,STR_RPT_NPC_STRONGHOLD,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_NPC_ASSASSIN*/       {GENERAL + 73, REALM_NAME, PROV_NAME, NO_ARRAY, {STR_RPT_NPC_ASSASSIN, STR_RPT_NPC_ASSASSIN, STR_NULL, STR_NULL, STR_NULL}},		
/*SUBJ_NPC_CONTEST_DESTROY*/ {GENERAL + 74, REALM_NAME, HOLDING_TYP,PROV_NAME,  {STR_RPT_NPC_CONTEST_DESTROY, STR_RPT_NPC_CONTEST_DESTROY,STR_NULL, STR_NULL, STR_NULL}},
/*SUBJ_ALLY_DECLARE_WAR*/	{GENERAL +  75, ALLY_LEVEL, REALM_NAME,   REALM_NAME,   {STR_RPT_ALLY_DECLARE_WAR,STR_RPT_ALLY_DECLARE_WAR,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_USED_FREEACTION*/	{CHAMBER +  76, NO_ARRAY, NO_ARRAY,   NO_ARRAY,   {STR_RPT_USED_FREEACTION,STR_NULL,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_LEVELUP*/				{CHAMBER +  77, REGENT_NAME, REALM_NAME,   NO_ARRAY,   {STR_RPT_LEVELUP,STR_NULL,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_NPC_DECLARE_WAR_ALONE*/{GENERAL +  78, REALM_NAME, NO_ARRAY,   NO_ARRAY,   {STR_RPT_NPC_DECLARE_WAR_ALONE,STR_RPT_NPC_DECLARE_WAR_ALONE,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_REDUCEHOLDING*/		{CHAMBER + 79, REALM_NAME, HOLDING_TYP, PROV_NAME,  {STR_RPT_REDUCEHOLD,STR_RPT_REDUCEHOLD, STR_NULL, STR_NULL, STR_NULL}},
/*SUBJ_ALLY_DECLARE_WAR*/	{GENERAL +  80, REALM_NAME,   NO_ARRAY,	NO_ARRAY,	{STR_RPT_ALLIES_JOIN_WAR,STR_RPT_ALLIES_JOIN_WAR,STR_NULL,STR_NULL,STR_NULL}},
/*SUBJ_ALLY_DECLARE_WAR*/	{GENERAL +  81, REALM_NAME,   NO_ARRAY,	NO_ARRAY,	{STR_FMT_DEATHNOTIFY,STR_NULL,STR_NULL,STR_NULL,STR_NULL}},

// lieutenents killed - general
// lieutenents added - general
// actions by other regents - chamberlain
// results of battle - general
// castles completed - chamberlain
// castles not supported - chamberlain
// alliances - chamberlain
// declarations of war - general
// troops mustered and total troops - general
// random events - chamberlain
// espionage copies you on other realm's reports - spy
};
DEFINE_VECTOR_CLASS(SUBJECT,Subject);

//DEFINE_VECTOR_DATA(CSTRPTR,szHoldingType) = {"law","guild","temple","source","castle"};
//DEFINE_VECTOR_CLASS(CSTRPTR, szHoldingType);

DEFINE_VECTOR_DATA(GAME_STRING,gsHoldingType) = {STR_REPORT_HOLDING_TYPE_LAW,STR_REPORT_HOLDING_TYPE_GUILD,STR_REPORT_HOLDING_TYPE_TEMPLE,STR_REPORT_HOLDING_TYPE_SOURCE,STR_REPORT_HOLDING_TYPE_CASTLE,};
DEFINE_VECTOR_CLASS(GAME_STRING,gsHoldingType);

CSTRPTR szDigits[10] = {"0","1","2","3","4","5","6","7","8","9"};

//DEFINE_VECTOR_DATA(CSTRPTR,szNumberWord) = {"zero","one","two","three","four","five","six","seven","eight","nine"};
//DEFINE_VECTOR_CLASS(CSTRPTR, szNumberWord);

DEFINE_VECTOR_DATA (GAME_STRING,gsNumberWord) = {STR_NUM_ZERO,STR_NUM_ONE,STR_NUM_TWO,STR_NUM_THREE,STR_NUM_FOUR,STR_NUM_FIVE,STR_NUM_SIX,STR_NUM_SEVEN,STR_NUM_EIGHT,STR_NUM_NINE};
DEFINE_VECTOR_CLASS(GAME_STRING,gsNumberWord);

//DEFINE_VECTOR_DATA(CSTRPTR,szLoyalty) = {"rebellion","poor","average","high"};
//DEFINE_VECTOR_CLASS(CSTRPTR, szLoyalty);

DEFINE_VECTOR_DATA(GAME_STRING,gsReportLoyalty) = {STR_REPORT_LOYALTY_REBEL,STR_REPORT_LOYALTY_POOR,STR_REPORT_LOYALTY_AVG,STR_REPORT_LOYALTY_HIGH	};
DEFINE_VECTOR_CLASS(GAME_STRING, gsReportLoyalty);

//DEFINE_VECTOR_DATA(CSTRPTR,szAlliance) = {"neutual", "permissive ally","full ally","vassal","liege lord"};
//DEFINE_VECTOR_CLASS(CSTRPTR, szAlliance);

DEFINE_VECTOR_DATA (GAME_STRING,gsAlliance) =
	{STR_REPORT_ALLY_TYPE_NEUT,
	 STR_REPORT_ALLY_TYPE_PALLY_SHORT,
	 STR_REPORT_ALLY_TYPE_FALLY,
	 STR_REPORT_ALLY_TYPE_VASS,
	 STR_REPORT_ALLY_TYPE_LIEGE};
DEFINE_VECTOR_CLASS(GAME_STRING,gsAlliance);

//DEFINE_VECTOR_DATA(CSTRPTR,szSeason) = {"Sarimiere","Haelynir","Erntenir","Keltier"};
//DEFINE_VECTOR_CLASS(CSTRPTR, szSeason);

DEFINE_VECTOR_DATA(GAME_STRING,gsSeason) = {STR_REPORT_SEASON_SAR,STR_REPORT_SEASON_HAE,STR_REPORT_SEASON_ERN,STR_REPORT_SEASON_KEL};
DEFINE_VECTOR_CLASS(GAME_STRING,gsSeason);


DEFINE_VECTOR_DATA_S(BIRTHRT_SND,sndHuman,11) =
{SND_COUNSELLOR_INTRO1,           SND_GENERAL_INTRO1,        SND_WIZARD_INTRO1,            SND_SPY_INTRO1,
 SND_COUNSELLOR_PROVINCE_REPORT1, SND_GENERAL_WAR_REPORT1,   SND_WIZARD_ADVENTURE_REPORT1, SND_SPY_ALLIANCE_REPORT1,
 SND_COUNSELLOR_LOYALTY_REPORT1,  SND_GENERAL_TROOP_REPORT1, SND_WIZARD_INVESTITURE_REPORT1};
DEFINE_VECTOR_CLASS(BIRTHRT_SND, sndHuman);

DEFINE_VECTOR_DATA_S(BIRTHRT_SND,sndElf,11) =
{SND_COUNSELLOR1_INTRO1,           SND_GENERAL1_INTRO1,        SND_WIZARD1_INTRO1,            SND_SPY1_INTRO1,
 SND_COUNSELLOR1_PROVINCE_REPORT1, SND_GENERAL1_WAR_REPORT1,   SND_WIZARD1_ADVENTURE_REPORT1, SND_SPY1_ALLIANCE_REPORT1,
 SND_COUNSELLOR1_LOYALTY_REPORT1,  SND_GENERAL1_TROOP_REPORT1, SND_WIZARD1_INVESTITURE_REPORT1};
DEFINE_VECTOR_CLASS(BIRTHRT_SND, sndElf);

DEFINE_VECTOR_DATA_S(BIRTHRT_SND,sndDwarf,11) =
{SND_COUNSELLOR2_INTRO1,          SND_GENERAL2_INTRO1,        SND_WIZARD2_INTRO1,            SND_SPY2_INTRO1,
 SND_COUNSELLOR2_PROVINCE_REPORT1,SND_GENERAL2_WAR_REPORT1,   SND_WIZARD2_ADVENTURE_REPORT1, SND_SPY2_ALLIANCE_REPORT1,
 SND_COUNSELLOR2_LOYALTY_REPORT1, SND_GENERAL2_TROOP_REPORT1, SND_WIZARD2_INVESTITURE_REPORT1};
DEFINE_VECTOR_CLASS(BIRTHRT_SND, sndDwarf);

extern BOOL 	char_selected;
extern MENU		Menus[];
extern SHORT	fFadedOut;
extern BOOL		fRedrawAll;
extern LONG		report_done_dturn_mode;
extern LONG		iHotSeatMultiplayerIndex;

LONG iOldSite;
static int iAbort = FALSE;

int		SoundTag = fERROR;
BIRTHRT_SND	iWhichSound;

DEFINE_STATIC_VECTOR_DATA_S(SHORT,iUpBitm,6);
DEFINE_STATIC_VECTOR_CLASS(SHORT,iUpBitm);

DEFINE_STATIC_VECTOR_DATA_S(SHORT,iDownBitm,6);
DEFINE_STATIC_VECTOR_CLASS(SHORT,iDownBitm);

class INVEST_COUNT
{
public:
	REALM::REALM_TYPE realm;
	int count;
};

class REPORT_COUNT
{
public:
	REALM::REALM_TYPE realm;
	int count;
	// Unused int score;
};

class ALLIES
{
public:
	REALM::REALM_TYPE realm;
	int total;
	int vassals;
	int full;
	int permisive;
};

DEFINE_STATIC_VECTOR_DATA_S(ALLIES,allies,LAND_REALM_COUNT);
DEFINE_STATIC_VECTOR_CLASS(ALLIES,allies);

#define NUMBER_OF_CT_HOLDINGS	10
static DECL_VECTOR_DATA(REPORT_COUNT,rCountHoldings,NUMBER_OF_CT_HOLDINGS);
#define NUMBER_OF_CT_PROVINCES	10
static DECL_VECTOR_DATA(REPORT_COUNT,rCountProvinces,NUMBER_OF_CT_PROVINCES);
#define NUMBER_OF_CT_UNITS	10
static DECL_VECTOR_DATA(REPORT_COUNT,rCountUnits,NUMBER_OF_CT_UNITS);
#define NUMBER_OF_CT_SCORES	10
static DECL_VECTOR_DATA(REPORT_COUNT,rCountScores,NUMBER_OF_CT_SCORES);

DEFINE_VECTOR_DATA_S(INVEST_COUNT, investProvCount, LAND_REALM_COUNT) = {{REALM::NO_COUNTRY, 0}};
DEFINE_VECTOR_CLASS(INVEST_COUNT, investProvCount);

DEFINE_VECTOR_DATA_S(INVEST_COUNT, investOldProvCount, LAND_REALM_COUNT) = {{REALM::NO_COUNTRY, 0}};
DEFINE_VECTOR_CLASS(INVEST_COUNT, investOldProvCount);

DEFINE_VECTOR_DATA_S(INVEST_COUNT, sortProvCount, LAND_REALM_COUNT) = {{REALM::NO_COUNTRY, 0}};
DEFINE_VECTOR_CLASS(INVEST_COUNT, sortProvCount);

static char const * const ReportString (LONG str_typ, LONG index);
static void PaintImmediateReport (LONG MenuCombo, LONG);
static void PaintProvinceTable (LONG x, LONG y, LONG w, LONG h);
static void PaintUnitsTable (LONG x, LONG y, LONG w, LONG h);
static void PaintWarReport (LONG x, LONG y, LONG w, LONG h);
static void PaintAllianceReport(LONG X, LONG Y, LONG W, LONG H);
static void PaintInvestitureReport(LONG X, LONG Y, LONG W, LONG H);
UBYTE ReactionAndStatusColor(LONG Realm1, LONG Realm2);
void NewTutorial (BOOL);
void SuspendTimeLimit (void);
extern "C" void ResumeTimeLimit (void);

static	long lReportTimeout = 0;


DEFINE_STATIC_VECTOR_DATA_S(int,iAdvButtonStr,13) = {
	STR_RPTBTN_0,
	STR_RPTBTN_1,
	STR_RPTBTN_2,
	STR_RPTBTN_3,
	STR_RPTBTN_4,
	STR_RPTBTN_5,
	STR_RPTBTN_6,
	STR_RPTBTN_7,
	STR_RPTBTN_8,
	STR_RPTBTN_9,
	STR_RPTBTN_10,
	STR_RPTBTN_11,
	STR_HELP1_DONE
};
DEFINE_STATIC_VECTOR_CLASS(int, iAdvButtonStr);

extern "C" {
void SetRedrawMainMapLevel (void);
}

DEFINE_STATIC_VECTOR_DATA_S(CSTRPTR,szAdvisorPort,12) = {
	"h01", "h02", "h03", "h04",
	"e01", "e02", "e03", "e04",
	"d01", "d02", "d03", "d04"};
DEFINE_STATIC_VECTOR_CLASS(CSTRPTR, szAdvisorPort);

void ClearOldInvestitureList(void)
{
	oldCountBuilt = FALSE;
}

/* ========================================================================
   Function    - CopyReactionTable
   Description -
   Returns     - void
   ======================================================================== */
void CopyReactionTable(void)
{
	int i, j;

	for(i=0; i<REALM::REALM_COUNT; i++)
		for(j=0; j<LAND_REALM_COUNT; j++)
			oldRealmReaction[i][j] = RealmReaction[i][j];

	// copy the province count to old
//	for(i=1; i<LAND_REALM_COUNT; i++)
//	{
//		investOldProvCount[i].realm = investProvCount[i].realm;
//		investOldProvCount[i].count = investProvCount[i].count;
//	}
}

/* ========================================================================
   Function    - StartNewReports
   Description -
   Returns     - void
   ======================================================================== */
void StartNewReports (void)
{
	iNewReports = iNextReport;
}

/* ========================================================================
   Function    - ClearOldReports
   Description -
   Returns     - void
   ======================================================================== */
void ClearOldReports (void)
{
	LONG	i;

	for (i=iNewReports; i<iNextReport; i++)
		report[i-iNewReports] = report[i];

	iNextReport -= iNewReports;
	StartNewReports();
}

/* ========================================================================
   Function    - PaintProvinceLose
   Description -
   Returns     - void
   ======================================================================== */
void PaintProvinceLose(LONG MenuCombo, LONG)
{
	LONG	X,Y;
	LONG	mx,my,mw,mh;
	LONG	MenuId, ButtonId;
	CHAR	color = 1;
	CHAR	cpBuffer[256];
	LONG	TextHeight;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	// get position of lose screen
	if( GetButtonPosition( MenuId, 0, &mx, &my ) == fERROR)
		return;
	if( GetButtonSize( MenuId, 0, &mw, &mh ) == fERROR)
		return;

	X = mx + 60;
	Y = my + 20;
	
 	// paint dialog title
	gprint_text(X,Y,STRMGR_GetStr(STR_DEFEAT), 31);
	

	// build some text that says why you lost
	init_gfont(FONT_SANS_16PT);
	sprintf(cpBuffer,"%s",STRMGR_GetStr(STR_PROV_LOSE));

	TextHeight=gtext_height(cpBuffer);
	X = mx + 30;
	Y = my + mh/2 - TextHeight/2;
	
	
	
	
	gprint_text(X,Y,cpBuffer ,color);
	
	// paint the control buttons
	init_gfont(FONT_TITL_10PT);
		
	// get position of Done button
	if( GetButtonPosition( MenuId, 2, &mx, &my ) == fERROR)
		return;
		
	X = mx;
	Y = my;
	
	// get size of Done Button
	if( GetButtonSize( MenuId, 2, &mx, &my ) == fERROR)
		return;
		
	X += mx/2;
	Y += my/2;
	
	strcpy(cpBuffer,STRMGR_GetStr(STR_OK));
	print_text_centered(X,Y,cpBuffer,WHITE);
}

/* ========================================================================
   Function    - ProvinceLooseProc
   Description -
   Returns     - void
   ======================================================================== */
void ProvinceLooseProc(LONG MenuCombo, LONG)
{
	LONG	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	HideMenu(MenuId);
	RunMenus();

	// make 'em start a new game
	char_selected = FALSE;

#ifdef _WINDOWS
	// let multi-player code know
	if ( IsMultiPlayer() )
		AMultiPlayer.Finalize(1);
#endif

	GAMEToggleMainMenu(TRUE, 0);
}

void PaintRegentLose(LONG MenuCombo, LONG)
{
	LONG	X,Y;
	LONG	mx,my,mw,mh;
	LONG	MenuId, ButtonId;
	CHAR	color = 1;
	CHAR	cpBuffer[256];
	LONG	TextHeight;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	// get position of lose screen
	if( GetButtonPosition( MenuId, 0, &mx, &my ) == fERROR)
		return;
	if( GetButtonSize( MenuId, 0, &mw, &mh ) == fERROR)
		return;

	X = mx + 60;
	Y = my + 20;
	
 	// paint dialog title
	gprint_text(X,Y,STRMGR_GetStr(STR_DEFEAT), 31);
	

	// build some text that says why you lost
	init_gfont(FONT_SANS_16PT);
	sprintf(cpBuffer,"%s",STRMGR_GetStr(COMBAT_STR_END_MSG_13));

	TextHeight=gtext_height(cpBuffer);
	X = mx + 30;
	Y = my + mh/2 - TextHeight/2;
	
	
	
	
	gprint_text(X,Y,cpBuffer ,color);
	
	// paint the control buttons
	init_gfont(FONT_TITL_10PT);
		
	// get position of Done button
	if( GetButtonPosition( MenuId, 2, &mx, &my ) == fERROR)
		return;
		
	X = mx;
	Y = my;
	
	// get size of Done Button
	if( GetButtonSize( MenuId, 2, &mx, &my ) == fERROR)
		return;
		
	X += mx/2;
	Y += my/2;
	
	strcpy(cpBuffer,STRMGR_GetStr(STR_OK));
	print_text_centered(X,Y,cpBuffer,WHITE);
}

/* ========================================================================
   Function    - DoneImmediateReport
   Description -
   Returns     - void
   ======================================================================== */
static void DoneImmediateReport(LONG MenuCombo, LONG)
{
   //---- reset the report timeout
   lReportTimeout = 0;

	LONG	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	SetButtonProc (MenuId, 0, NULL, 0, 0);
	
	// click the button
	SetButtonHilight(MenuId, ButtonId, TRUE);
	RunMenus();
	update_screen();
	TickDelay(4);
	SetButtonHilight(MenuId, ButtonId, FALSE);
	RunMenus();
	update_screen();
	TickDelay(4);
	HideMenu(MenuId);
	RunMenus();
	ResumeTimeLimit();		// restart the countdown in timed multiplayer games
}

/* ========================================================================
   Function    - CheckForImmediateReports
   Description -
   Returns     - void
   ======================================================================== */
void CheckForImmediateReports(void)
{
	int i;
	char str0[80];
	char str1[80];
	char str2[80];
	char str3[80];

	for (i=0; i<iNextReport; i++)
	{
		// is this an immediate report
		if(report[i].imm == 1)
		{
			report[i].imm = 2;

			iWhichFace = Subject[report[i].subject].subj_class/100;

			strcpy(str1, ReportString(Subject[report[i].subject].array1,report[i].index1));
			strcpy(str2, ReportString(Subject[report[i].subject].array2,report[i].index2));
			strcpy(str3, ReportString(Subject[report[i].subject].array3,report[i].index3));
			strcpy(str0, STRMGR_GetStr(Subject[report[i].subject].sz[0]));

			sprintf(n, str0, str1, str2, str3);
		
			// setup for generic statement box
			SetButtonProc (D_QUESTION1, 0, PaintImmediateReport, 0, 0);
			SetButtonLabel  (D_QUESTION1, QUESTION_TEXT, -1, BLACK );
		
			// turn on the first button as YES
			SetButtonLabel  (D_QUESTION1, QUESTION_BTN1, STR_OK, BTN_LABEL_COLOR );
			SetButtonProc (D_QUESTION1, QUESTION_BTN1, HideSubMenuWithClick, D_QUESTION1, D_KEY_OK);
			


           // We need to time out this menu in multiplayer

#ifdef _WINDOWS

           if ( IsMultiPlayer() )
           {
               lReportTimeout = GetTickCount() + REPORTTIMEOUT;
           }

#endif
			
			if (iHotSeatMultiplayerIndex != REALM::MIERES)
			{
				SuspendTimeLimit();				// stop the timer in timed multiplayer games
				ShowSubMenu(0,D_QUESTION1);
				RunMenus();
			}
			break;
		}
	}
}

/* ========================================================================
   Function    - AddReport
   Description -
   Returns     - void
   ======================================================================== */
void AddReport (LONG subject, LONG index1, LONG index2, LONG index3)
{
	LONG	i;
	int		iSubImm = FALSE;

	// is this an immediate report?
	if(subject >= IMMEDIATE)
	{
		subject -= IMMEDIATE;
		iSubImm = TRUE;
	}

	for (i=iNewReports; i<iNextReport; i++)
	{
		if (report[i].subject == subject && subject != SUBJ_DISBAND_UNIT
			&& report[i].index1 == index1
			&& (report[i].index2 == index2 || Subject[subject].array2 == NUM_DIGITS)
			&& (report[i].index3 == index3 || Subject[subject].array3 == NUM_DIGITS || Subject[subject].array3 == LOY_LEVEL) )
		{
			report[i].subject	= subject;
			report[i].index1	= index1;
			report[i].index2	= index2;
			report[i].index3	= index3;

			if(iSubImm)
			{
				report[i].imm = 1;
			}
			return;
		}
	}

	report[iNextReport].subject = subject;
	report[iNextReport].index1	 = index1;
	report[iNextReport].index2	 = index2;
	report[iNextReport].index3	 = index3;

	// was this an immediate report?
	if(iSubImm)
	{
		report[iNextReport].imm = 1;
	}

	iNextReport++;
}



/* ========================================================================
   Function    - SendReportTo
   Description -
   Returns     - void
   ======================================================================== */
void SendReportTo (LONG iRealm, LONG subject, LONG index1, LONG index2, LONG index3)
{
	// if we are a multi-player game, and iRealm is a real player
	// send them the message and add an identifier to the subject

	if (iRealm == HomeRealm) // send to ourselves -- shortcut to AddReport
	{
		AddReport(subject, index1, index2, index3);
	}
#ifdef _WINDOWS
	else if ( IsMultiPlayer() &&
        realm[iRealm].mfIsPlayerCtrl() == TRUE )
	{
   	subject += IMMEDIATE;    // bump it up higher this is removed in gamemap.cpp

	#ifdef _WINDOWS
	if ( fLogComment )
   {
		char temp[100];
		sprintf ( temp, "SendReportTo: SendEvent");
		RandomLogComment ( temp );
	}
	#endif
		AMultiPlayer.SendEvent( subject, index1, index2, index3, 0, iRealm );
		AMultiPlayer.InitEvent(0);
	}

#endif

}



/* ========================================================================
   Function    - AddReportTo
   Description -
   Returns     - void
   ======================================================================== */
void AddReportTo (LONG iRealm, LONG subject, LONG index1, LONG index2, LONG index3)
{
	LONG	i;
	int		iSubImm = FALSE;

	// if an AI sent a report to us, add it
	if(iRealm == HomeRealm)
	{
		AddReport(subject, index1, index2, index3);
		return;
	}

}



/* ========================================================================
   Function    - ReportString
   Description -
   Returns     - void
   ======================================================================== */
static char const * const ReportString (LONG str_typ, LONG index)
{
	switch (str_typ)
	{
		case REGENT_NAME:
			if(index >= CHARACTER_COUNT)
				index = 0;
			// [d4-09-97 JPC] Modified the following to use the access
			// function instead of direct access to the .name member,
			// which is going away.
			return regents[index].mfGetname();
		case PLACE_NAME:
			if(index > 63)
				index = 63;
			return place_names[places[index].iName];
		case PROV_NAME:
			if(index >= PROVINCE_COUNT)
				index = 0;
			return province[index].name;
		case REALM_NAME:
			if(index > REALM::REALM_COUNT)
				index = 0;
			return realm[index].mfGetName();
		case HOLDING_TYP:
			if(index > 4)
				index = 0;
			return STRMGR_GetStr(gsHoldingType[index]);
		case NUM_DIGITS:
			if(index > 9)
				index = 0;
			return szDigits[index];
		case NUMBER_WORD:
			if(index > 9)
				index = 0;
			return STRMGR_GetStr(gsNumberWord[index]);
		case DEC_INC:
			return (index) ? STRMGR_GetStr(STR_DECREASED):STRMGR_GetStr(STR_INCREASED);
		case LOY_LEVEL:
			if(index > 3)
				index = 0;
			return STRMGR_GetStr(gsReportLoyalty[index]);
		case UNIT_TITLE:
			if(index > MAX_UNITS)
				index = 0;
			return STRMGR_GetStr(gsUnitTitle[units[index].Icon]);
		case ALLY_LEVEL:
			if(index > 4)
				index = 0;
			return STRMGR_GetStr(gsAlliance[index]);
		default:
			return "";
	}
}

#if 0
/* ========================================================================
   Function    - SetReportString
   Description -
   Returns     -
   ======================================================================== */
static BOOL SetReportString (void)
{
	LONG			i, j, s, p, sz_type, last;
	LONG			X,Y;
	LONG			W,H;
	LONG			startY;
	CSTRPTR			psz = NULL;
	BOOL			rv = TRUE;

	// everyone except the wizard will have a report
	if(iWhosReport == 2) rv = FALSE;

	for (s=iWhosReport * 100; s<(iWhosReport * 100) + 64; s++)		// go through all subjects
	{
		p = 0;						// start at beginning of string
		sz_type = 0;				// start with first format string

		for (i=0; i<iNewReports; i++)
		{
			// don't display immediate reports
			if(report[i].imm)
				continue;

			// save the subject
			iCurSubj = report[i].subject;

			// check for initial report at start of game
			if(iCurSubj == SUBJ_INITIAL)
			{
				if(iNewSite >= 0 && iWhosReport == 2)
					psz = &n1[0];
				else
					psz = NULL;
					
				sprintf(szReport, "%s\n \n     %s", n, psz);
				return TRUE;
			}

			// is this a report for the current subject?
			if (Subject[iCurSubj].subj_class == s)
			{
				// assume this is the only report for this subject
				last = TRUE;

				for (j=i+1; j<iNewReports; j++)		// is this the last?
					if (Subject[report[j].subject].subj_class == s)
						last = FALSE;

				// if last report for subject and we have printed a report for this subject
				// use the fourth format string (sz_type = 3)
//				if (last && (sz_type==1 || sz_type==2))
				if (last && sz_type==2)
					sz_type = 3;

				// put the report in the string
				{
					char str1[80], str2[80], str3[80];
						
					strncpy(str1, ReportString(Subject[iCurSubj].array1,report[i].index1), 80);
					strncpy(str2, ReportString(Subject[iCurSubj].array2,report[i].index2), 80);
					strncpy(str3, ReportString(Subject[iCurSubj].array3,report[i].index3), 80);
					
					sprintf(&szReport[p], STRMGR_GetStr(Subject[report[i].subject].sz[sz_type]),
						str1, str2, str3);
				}

				// set the next position in the string
				p = strlen(szReport);

				// assume another report of the same subject
				// use next format string, but not the last
				if (sz_type<2)
					sz_type++;
			}
		}

		// if a string was produced for the report, print it
		if (p)
		{
			// if there were more than one of this subject
			// add the string epilog
			if (sz_type > 1)
				sprintf(&szReport[p], "%s  ", STRMGR_GetStr(Subject[iCurSubj].sz[4]));

			// if a new adventure site for the wizard, print it
			if(iNewSite >= 0 && iWhosReport == 2)
				psz = &n1[0];
			else
				psz = NULL;
					
			p = strlen(szReport);
			sprintf(&szReport[p], "\n \n     %s", psz);

			rv = TRUE;
		}
	}

	return rv;
}
#endif

/* ========================================================================
   Function    - compare
   Description - comparator for sort routine
   Returns     -
   ======================================================================== */
int compare(void const *op1, void const *op2)
{
	ALLIES *a1, *a2;

	a1 = (ALLIES *)op1;
	a2 = (ALLIES *)op2;

	// < and > are reversed, as I want largest total first
	if(a1->total < a2->total) return 1;
	if(a1->total > a2->total) return -1;
	return 0;
}

/* ========================================================================
   Function    - compare1
   Description - comparator for sort routine
   Returns     -
   ======================================================================== */
int compare1(void const *op1, void const *op2)
{
	INVEST_COUNT *a1, *a2;

	a1 = (INVEST_COUNT *)op1;
	a2 = (INVEST_COUNT *)op2;

	// < and > are reversed, as I want largest count first
	if(a1->count < a2->count) return 1;
	if(a1->count > a2->count) return -1;
	return 0;
}

LONG iWhichVox;
/* ========================================================================
   Function    - GiveReport
   Description -
   Returns     -
   ======================================================================== */
void GiveReport (LONG lArg)
{
	LONG		now;
	LONG		newTop;
	LONG		i, j, r, t;
	char		buff[80];
	FILE		*in;
	REALM::REALM_TYPE		testRealm;

	iWhosReport = 0;
	sndPlayed = FALSE;
	sMyHoldings = 0;
	sMyUnits = 0;
	sMyProvinces = 0;
	sMyVassals = 0;
	sMyFull = 0;
	sMyPermissive = 0;
	sMyOther = 0;

#ifdef _GEH_DEMO
	if (date == 2)
	{
		SHORT	bitm;
		LONG	now;
		
		PlayFinale(REALM::ROESONE,0);
		bitm = GetResourceStd ("finale\\buymore.pcx", FALSE);
 		
		if (bitm != fERROR)
		{
			DrawBitmap (0, 0, bitm, 0, 0, 640, 480);
			update_screen();
			SetPurge(bitm);
		}
		
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
					goto End;
				}
				fAnyKeyChanged = FALSE;
				clear_key_status(0);
				break;
			}
		}
		
End:	
		FadeOut(100);
		
		QuitSys(0,0);
	}
#endif

	push_regions();

	// clear the arrays
	for (i=0; i<LAND_REALM_COUNT; i++)
	{
		sHoldings[i] = 0;
		sUnits[i] = 0;
		sScores[i] = 0;
	}
	
	for (i =0; i < NUMBER_OF_CT_HOLDINGS; ++i)
	{
		rCountHoldings[i].realm = REALM::NO_COUNTRY;
		rCountHoldings[i].count = 0;
	}
	
	for (i=0; i < NUMBER_OF_CT_PROVINCES; ++i)
	{
		rCountProvinces[i].realm = REALM::NO_COUNTRY;
		rCountProvinces[i].count = 0;
	}
	
	for (i= 0; i < NUMBER_OF_CT_UNITS; ++i)
	{
		rCountUnits[i].realm = REALM::NO_COUNTRY;
		rCountUnits[i].count = 0;
	}

	for (i= 0; i < NUMBER_OF_CT_SCORES; ++i)
	{
		rCountScores[i].realm = REALM::NO_COUNTRY;
		rCountScores[i].count = 0;
	}

//@@@@@@@@@@@@@@@@@@@  MULTIPLAYER

	CheckRealmStatus(0,0, FALSE );

	// count the provinces

	// make sure no garbage is in the unused 0 element
	investProvCount[0].count = 0;
	investProvCount[0].realm = REALM::NO_COUNTRY;
	sortProvCount[0].count = 0;
	sortProvCount[0].realm = REALM::NO_COUNTRY;

	// clear the counts first
	for(i=1; i<LAND_REALM_COUNT; i++)
	{
		investProvCount[i].count = 0;
		investProvCount[i].realm = REALM::NO_COUNTRY;

		if(oldCountBuilt == FALSE)
		{
			investOldProvCount[i].count = 0;
			investOldProvCount[i].realm = REALM::NO_COUNTRY;
		}
	}

	// count the provinces for each realm
	for(i=1; i<PROVINCE_COUNT; i++)
	{
		testRealm = (REALM::REALM_TYPE)province[i].Realm;

		// some realms to not include
		if (testRealm==REALM::ANUIRE ||
			testRealm==REALM::MIERES ||
			testRealm==REALM::MONSTERS ||
			testRealm==REALM::NO_COUNTRY ||
			testRealm >= LAND_REALM_COUNT)
			continue;

		investProvCount[testRealm].count++;
		investProvCount[testRealm].realm = testRealm;
	}

	if(oldCountBuilt == FALSE)
	{
		for(i=1; i<LAND_REALM_COUNT; i++)
		{
			investOldProvCount[i].count = investProvCount[i].count;
			investOldProvCount[i].realm = investProvCount[i].realm;
		}

		oldCountBuilt = TRUE;
	}

	// build a difference table for sorting
	for(i=1; i<LAND_REALM_COUNT; i++)
	{
		if(investOldProvCount[i].count != investProvCount[i].count)
		{
			sortProvCount[i].realm = investProvCount[i].realm;
			sortProvCount[i].count = investProvCount[i].count - investOldProvCount[i].count;
		}
		else
		{
			sortProvCount[i].realm = investProvCount[i].realm;
			sortProvCount[i].count = 0;
		}
	}

	// sort the province difference table
	// after sorting the 0th element of sortProvCount is valid
	qsort(&sortProvCount[0], LAND_REALM_COUNT, sizeof(INVEST_COUNT), compare1);


	// count the holdings
	for (i=1; i<MAX_PLACES; i++)
		if ((r = places[i].Realm) < LAND_REALM_COUNT)	// don't count those outside of LAND_REALM_COUNT
			sHoldings[r]++;
	sMyHoldings = sHoldings[HomeRealm];

	// count the units
	for (i=1; i<MAX_UNITS; i++)
		if ((r = units[i].Realm) < LAND_REALM_COUNT)		// don't count those outside of LAND_REALM_COUNT
			sUnits[r]++;
	sMyUnits = sUnits[HomeRealm];

	// count the score
	for (i=1; i<LAND_REALM_COUNT; i++)
		sScores[i] = CalculatePoints((REALM::REALM_TYPE)i);
	sMyScore = sScores[HomeRealm];
	
	// count allies
	for(i=0; i<LAND_REALM_COUNT-1; i++)
	{
		t = i+1;
		allies[i].realm = (REALM::REALM_TYPE)t;
		allies[i].total = 0;

		for(j=1; j<LAND_REALM_COUNT; j++)
		{
			if(realm[t].mfExists() == FALSE)
				continue;
			// vassals
			if (ISVASSAL(j,t) && j != t)
			{
				allies[i].vassals++;
				allies[i].total++;
			}
			else
			{
				// full aliance provinces
				if (AREFULLALLIES(j,t) && j != t)
				{
					allies[i].full++;
					allies[i].total++;
				}
				else
				{	
					// permissive alliances provinces
					if (AREALLIED(j,t) && j != t)
					{
						allies[i].permisive++;
						allies[i].total++;
					}
				}
			}
		}
	}

	// sort the allies
	qsort(&allies[0], LAND_REALM_COUNT-1, sizeof(ALLIES), compare);

	// count allies points
	for (j=1; j<PROVINCE_COUNT; j++)
	{
		PROVINCE ThisProvRealm = (PROVINCE)province[j].Realm;
		
		// my province - 3 points
		if (ThisProvRealm == HomeRealm)
		{
			sMyProvinces += 3;
		}
		
		// my vassals provinces - 3 points
		if (ISVASSAL(ThisProvRealm,HomeRealm) && ThisProvRealm != HomeRealm)
		{
			sMyVassals += 3;
		}
		else
		{
			// full aliance provinces - 2 points
			if (AREFULLALLIES(ThisProvRealm,HomeRealm) && ThisProvRealm != HomeRealm)
			{
				sMyFull += 2;
			}
			else
			{	
				// permissive alliances provinces - 1 point
				if (AREALLIED(ThisProvRealm,HomeRealm) && ThisProvRealm != HomeRealm)
				{
					sMyPermissive += 1;
				}
			}
		}
		
		// Spiderfell provinces - 10 points - add 7, 3 were added above
		if (ThisProvRealm == HomeRealm && province[j].Realm == REALM::SPIDERFELL )
		{
			sMyOther += 7;
		}
		
		// Manslayer provinces - 10 points - add 7, 3 were added above
		if (ThisProvRealm == HomeRealm && province[j].Realm == REALM::RHUOBHE )
		{
			sMyOther += 7;
		}
		
		// Gorgons Crown provinces - 10 points - add 7, 3 were added above
		if ( ThisProvRealm == HomeRealm
			&&
			(	ABATTOIR  == j ||
				ANATHAR  == j ||
				ELFSEYES  == j ||
				JOGH_WARREN  == j ||
				KAL_SAITHARAK  == j ||
				METTLE  == j ||
				MOTILE  == j ||
				MUTIANS_POINT == j ||
				OROGS_HEAD  == j ||
				PELT  == j ||
				PLUMBAGO  == j ||
				SAGES_FEN  == j ||
				SERES_HOLD  == j ||
				SIDEATH == j ||
				STONES_END  == j ||
				SUNDER_FALLS  == j ||
				ZAPTIG == j )
			)
		{
			sMyOther += 7;
		}
		
		// Awnsheghlien allies provinces - 5 points
		if ( ThisProvRealm == HomeRealm
			&&
			(	// markazor
				BRUSHFIRE  == j ||
				DWARFS_HOLD == j ||
				ELFSDEMISE == j ||
				PERILTREES == j ||
				RIVERSPRING == j ||
				SHATTERED_HILLS == j ||
				SUTREN_HILLS == j ||

				// thurazor
				BLOODBAY == j ||
				CRUSHING_HILLS == j ||
				DOOMS_PEAK == j ||
				FALLING_TIMBER == j ||
				MERGARROTE == j ||
				STORMS_RELEASE == j ||

				// chimaeron
				BARNIERE == j ||
				CAREINE == j ||
				HAMEIN == j ||
				LYSSAN == j ||
				MHOWE == j ||
				RUORKHE == j ||
				SALVIENE == j ||

				//mur-kilad
				CRUSHING_ROCK == j ||
				FALLEN_ROCK == j )

			)
		{
			sMyOther += 5;
		}
	}

	// sort the top 10 in each array
	newTop = 9999;
	for(i=0; i<NUMBER_OF_CT_HOLDINGS; i++)
	{
		newCount = 0;
		for(j=1; j<LAND_REALM_COUNT; j++)
		{
			// find the biggest
			if(sHoldings[j] > newCount && sHoldings[j] < newTop && j != HomeRealm)
			{
				rCountHoldings[i].count = sHoldings[j];
				rCountHoldings[i].realm = (REALM::REALM_TYPE)j;
				newCount = sHoldings[j];
			}
		}
		newTop = newCount;
	}

	newTop = 9999;
	for(i=0; i<NUMBER_OF_CT_PROVINCES; i++)
	{
		newCount = 0;
		for(j=1; j<LAND_REALM_COUNT; j++)
		{
			if(sProvinces[j] > newCount && sProvinces[j] < newTop && j != HomeRealm)
			{
				rCountProvinces[i].count = sProvinces[j];
				rCountProvinces[i].realm = (REALM::REALM_TYPE)j;
				newCount = sProvinces[j];
			}
		}
		newTop = newCount;
	}

	newTop = 9999;
	for(i=0; i<NUMBER_OF_CT_UNITS; i++)
	{
		newCount = 0;
		for(j=1; j<LAND_REALM_COUNT; j++)
		{
			if(sUnits[j] > newCount && sUnits[j] < newTop && j != HomeRealm)
			{
				rCountUnits[i].count = sUnits[j];
				rCountUnits[i].realm = (REALM::REALM_TYPE)j;
				newCount = sUnits[j];
			}
		}
		newTop = newCount;
	}

	newTop = 9999;
	for(i=0; i<NUMBER_OF_CT_SCORES; i++)
	{
		newCount = 0;
		for(j=1; j<LAND_REALM_COUNT; j++)
		{
			if(sScores[j] > newCount && sScores[j] < newTop && j != HomeRealm)
			{
				rCountScores[i].count = sScores[j];
				rCountScores[i].realm = (REALM::REALM_TYPE)j;
				newCount = sScores[j];
			}
		}
		newTop = newCount;
	}

	// clear game menu button
	SetButtonFlag(D_GAMEBUTTON, 0, D_INVISIBLE);

	if (HomeRealm == REALM::BARUKAZHIK	// load dwarf advisor art
		|| HomeRealm == REALM::GORGONSCROWN
		|| HomeRealm == REALM::MURKILAD )
	{
		iWhichVox = 2;
		ReportBkgndBitm = GetResourceStd ("UI\\ADVISORS\\ADVISDWF.PCX" , FALSE);
		advisorFLCs[0].mfInit("UI\\ADVISORS\\DCONSL01.FLC",  35, 186);
		advisorFLCs[1].mfInit("UI\\ADVISORS\\DGENRL01.FLC", 174, 142);
		advisorFLCs[2].mfInit("UI\\ADVISORS\\DWIZRD01.FLC", 318, 152);
		advisorFLCs[3].mfInit("UI\\ADVISORS\\DSPYMS01.FLC", 464, 153);
	}
	else if(HomeRealm == REALM::SIELWODE || HomeRealm == REALM::TUARHIEVEL	// load elf advisor art
		|| HomeRealm == REALM::RHUOBHE )
	{
		iWhichVox = 1;
		ReportBkgndBitm = GetResourceStd ("UI\\ADVISORS\\ADVISELF.PCX" , FALSE);
		advisorFLCs[0].mfInit("UI\\ADVISORS\\ECONSL01.FLC",  81, 131);
		advisorFLCs[1].mfInit("UI\\ADVISORS\\EGENRL01.FLC", 204, 101);
		advisorFLCs[2].mfInit("UI\\ADVISORS\\EWIZRD01.FLC", 330, 142);
		advisorFLCs[3].mfInit("UI\\ADVISORS\\ESPYMS01.FLC", 439, 129);
		advisorFLCs[4].mfInit("UI\\ADVISORS\\ELFTORCH.FLC",  34, 202);
		advisorFLCs[5].mfInit("UI\\ADVISORS\\ELFTORCH.FLC", 545, 202);
	}
	else	// human advisor art
	{
		iWhichVox = 0;
		ReportBkgndBitm = GetResourceStd ("UI\\ADVISORS\\ADVISORS.PCX" , FALSE);
		advisorFLCs[0].mfInit("UI\\ADVISORS\\HCONSL01.FLC",  73, 136);
		advisorFLCs[1].mfInit("UI\\ADVISORS\\HGENRL01.FLC", 197, 132);
		advisorFLCs[2].mfInit("UI\\ADVISORS\\HWIZRD01.FLC", 329, 141);
		advisorFLCs[3].mfInit("UI\\ADVISORS\\HSPYMS01.FLC", 459, 123);
	}
	
	SysHideCursor();
	clear_screen_to (1);		// JPC get rid of junk on the edges
									// [d3-14-97 JPC] Change clear_screen to
									// clear_screen_to; solves cursor junk on the
									// edges problem in the DOS version
	DrawBitmap(0, 0, ReportBkgndBitm, 0, 0, 999, 999);
	
	for (i=4; i < 6; i++) // torches
	{
		advisorFLCs[i].mfDrawFrame();
		advisortimer[i] = get_time() + 1;
	}
	for (i=0; i < 4; i++)
	{
		advisorFLCs[i].mfDrawFrame();
		advisortimer[i] = get_time() + ( (random(200)+100) * 182 / 1000);
	}
#if defined(_JUNEDEMO)
	introPlaying = 5;	// don't play their intros
#endif		

	if (introPlaying < 0)
		advisortimer[0] = get_time();

	SysShowCursor();
	update_screen();
	
	// get the button art
	iScrollBitm[0] = GetResourceStd("UI\\SCRLBTN1.PCX", FALSE);
	iScrollBitm[1] = GetResourceStd("UI\\SCRLBTN2.PCX", FALSE);
	iScrollBitm[2] = GetResourceStd("UI\\SCRLBTN3.PCX", FALSE);
	iScrollBitm[3] = GetResourceStd("UI\\SCRLBTN4.PCX", FALSE);
	iScrollBitm[4] = GetResourceStd("UI\\SCRLBTN0.PCX", FALSE);
	iDoneBitm      = GetResourceStd("UI\\SCRLBTN5.PCX", FALSE);

	// get the loyalty bullet art
	iUpBitm[0] = GetResourceStd("UI\\UP_MAR.PCX", FALSE);
	iUpBitm[1] = GetResourceStd("UI\\UP_RED.PCX", FALSE);
	iUpBitm[2] = GetResourceStd("UI\\UP_GRN.PCX", FALSE);
	iUpBitm[3] = GetResourceStd("UI\\UP_BLU.PCX", FALSE);
	iUpBitm[4] = GetResourceStd("UI\\UP_PRP.PCX", FALSE);
	iUpBitm[5] = GetResourceStd("UI\\UP_ORG.PCX", FALSE);

	iDownBitm[0] = GetResourceStd("UI\\DN_MAR.PCX", FALSE);
	iDownBitm[1] = GetResourceStd("UI\\DN_RED.PCX", FALSE);
	iDownBitm[2] = GetResourceStd("UI\\DN_GRN.PCX", FALSE);
	iDownBitm[3] = GetResourceStd("UI\\DN_BLU.PCX", FALSE);
	iDownBitm[4] = GetResourceStd("UI\\DN_PRP.PCX", FALSE);
	iDownBitm[5] = GetResourceStd("UI\\DN_ORG.PCX", FALSE);

	// show the buttons
	ShowMenu(D_ADVISOR);
}

/* ========================================================================
   Function    -
   Description -
   Returns     -
   ======================================================================== */
void AdvisorButton(LONG iWhichButton, LONG)
{
	char	buff[80];
	FILE	*in;
	int		i;

	iCurSubj = 0;
	iWhichReport = iWhichButton;
	
	// stop playing intros
	if (introPlaying >= 0 && introPlaying < 4)
	{
		StopASound(iWhichSound, SoundTag);
		advisorFLCs[introPlaying].mfMove(0, -INTRO_MOVE_AMOUNT);
		introPlaying = 4;
	}
	
	SysHideCursor();
	// do visible click
	{
		LONG x = 40 + ((iWhichReport % 4) * 144);
		LONG y = 20 + ((iWhichReport / 4) * 36);
		LONG w = 128;
		LONG h = 24;
		SHORT bitmap = iScrollBitm[4];
	
		init_gfont(FONT_SANS_8PT);
		
		if (iWhichReport == 12)
		{
			x = 268;
			y = 440;
			w = 104;
			bitmap = iDoneBitm;
		}
		
		DrawBitmap(x-1, y-1, ReportBkgndBitm, x-1, y-1, w+1, h+1);
		DrawBitmap(x-1, y-1, bitmap, 0, 0, w, h);
		print_text_centered(x+(w/2)-1,y+(h/2)-1,STRMGR_GetStr(iAdvButtonStr[iWhichReport]),DKBROWN);
		update_screen();
		TickDelay(2);
		DrawBitmap(x-1, y-1, ReportBkgndBitm, x-1, y-1, w+1, h+1);
		DrawBitmap(x, y, bitmap, 0, 0, w, h);
		print_text_centered(x+(w/2),y+(h/2),STRMGR_GetStr(iAdvButtonStr[iWhichReport]),DKBROWN);
		update_screen();
	}
	SysShowCursor();
	

	HideMenu(D_ADVISOR);
	RunMenus();
	SysHideCursor();
	DrawBitmap(0, 0, ReportBkgndBitm, 0, 0, 999, 999);
	for (i=4; i < 6; i++) // torches
		advisorFLCs[i].mfDrawFrame();
	for (i=0; i < 4; i++)
		advisorFLCs[i].mfDrawFrame();
	SysShowCursor();

	switch(iWhichButton)
	{
		case 0:		// chamberlain intro text
		case 1:		// general intro text
		case 2:		// mage intro text
		case 3:		// spy master intro text
			iWhosReport = iWhichButton;
		 	iCurSubj = SUBJ_INITIAL;

			// open the data file
			sprintf(buff,"%sTEXT\\%s.rp%1d", InstallPath, szRealmTextFile[HomeRealm], iWhosReport);
			memset(&n[0], 0, REPORT_BUFFER_SIZE);	// clear the buffer
			in = FileOpen(buff, "r");
			if(in != NULL)
			{
				fread(n, REPORT_BUFFER_SIZE, 1, in);
				FileClose(in);
			}

			ShowMenu(D_DRPRT);
			RunMenus();
			break;

		case 4:		// province/holdings graph
			iWhosReport = 0;
			ShowMenu(D_DRPRT);
			RunMenus();
			break;

		case 5:		// war report
			iWhosReport = 1;
			ShowMenu(D_DRPRT);
			RunMenus();
			break;

		case 6:		// adventure news
			iWhosReport = 2;

			sprintf(buff,"%sTEXT\\%s.adv", InstallPath, advsite[iOldSite].name);
			memset(&n1[0], 0, REPORT_BUFFER_SIZE);	// clear the buffer
			in = FileOpen(buff, "r");
			if(in != NULL)
			{
				fread(n1, REPORT_BUFFER_SIZE, 1, in);
				FileClose(in);
			}

			ShowMenu(D_DRPRT);
			RunMenus();
			break;

		case 7:		// alliance report
			iWhosReport = 3;
			ShowMenu(D_DRPRT);
			RunMenus();
			break;

		case 8:		// loyalty graph
			iWhosReport = 0;
			ShowMenu(D_LOYALTY);
			RunMenus();
			break;

		case 9:		// troop graph
			iWhosReport = 1;
			ShowMenu(D_DRPRT);
			RunMenus();
			break;

		case 10:	// investiture report
			iWhosReport = 2;
			ShowMenu(D_DRPRT);
			RunMenus();
			break;

		case 11:	// DM Tools
			iWhosReport = 3;
			ShowMenu(D_DRPRT);
			RunMenus();
			break;

		case 12:	// done
			DoneTimer = 0;

			// restore game menu button
			ClearButtonFlag(D_GAMEBUTTON, 0, D_INVISIBLE);

			iWhosReport = 2;
			dturn_mode = report_done_dturn_mode;
			SetRedrawMainMapLevel();
			AdvisorReportOn = FALSE;

			{
				LONG i;
				for (i=0; i <= 4; i++)
				{
					if (iScrollBitm[i]!=fERROR)
					{
						SetPurge(iScrollBitm[i]);
						iScrollBitm[i] = fERROR;
					}
				}
				
				for (i=0; i < 6; i++)
				{
					advisorFLCs[i].mfDispose();
					advisortimer[i] = 0;
				}
			}
			if (iDoneBitm!=fERROR)
			{
				SetPurge(iDoneBitm);
				iDoneBitm = fERROR;
			}
			if(ReportBkgndBitm!=fERROR)
			{
				SetPurge(ReportBkgndBitm);
				ReportBkgndBitm = fERROR;
			}

			for(i=0; i<6; i++)
			{
				if(iUpBitm[i] != fERROR)
				{
					SetPurge(iUpBitm[i]);
					iUpBitm[i] = fERROR;
				}

				if(iDownBitm[i] != fERROR)
				{
					SetPurge(iDownBitm[i]);
					iDownBitm[i] = fERROR;
				}
			}

			pop_regions();
			return;
	}

	if (iWhichButton == 11)		// no voice for DM tools
		return;

	if(iWhichVox == 1)	// elf
		iWhichSound = sndElf[iWhichButton];
	else if(iWhichVox == 2)	// dwarf
		iWhichSound = sndDwarf[iWhichButton];
	else	// human
		iWhichSound = sndHuman[iWhichButton];

	// don't play these sounds here.
	if (iWhichButton <= 3)
		return;
		
#if !defined(_JUNEDEMO)
	SoundTag = AddSndObj(iWhichSound,0,VOLUME_NINETY);
#endif
}

/* ========================================================================
   Function    -
   Description -
   Returns     -
   ======================================================================== */
void PaintReportButtons (LONG MenuCombo, LONG)
{
	LONG	MenuId, ButtonId;
	LONG	x,y;
	LONG	w,h;
	int		i;

	// region for buttons is at 0, 420 for 640 by 60

	// width and height of each button
	w = 128;
	h = 24;

	// delete button regions
	del_region(AdvisorButton, 0);

	SysHideCursor();

	if (fRedrawAll||AdvisorReportOn == FALSE)
	{
		clear_screen_to (1);             // JPC get rid of junk on the edges
		fRedrawAll = FALSE;
	}
	DrawBitmap(0, 0, ReportBkgndBitm, 0, 0, 999, 999);
	AdvisorReportOn = TRUE;

	
	{
		LONG i, now = get_time();
		
		for (i=4; i < 6; i++)
		{
			advisorFLCs[i].mfDrawFrame();

			if (now >= advisortimer[i])
			{
				advisorFLCs[i].mfNextFrame();
				advisortimer[i] = now + 1;
			}
		}
		
		if (introPlaying < 4 &&
			((introPlaying == -1) || (!CheckASound(iWhichSound, SoundTag))))
		{
			
			introPlaying++;
			
			if (introPlaying < 4)
			{
				if(iWhichVox == 1)	// elf
					iWhichSound = sndElf[introPlaying];
				else if(iWhichVox == 2)	// dwarf
					iWhichSound = sndDwarf[introPlaying];
				else	// human
					iWhichSound = sndHuman[introPlaying];
		
#if !defined(_JUNEDEMO)
				SoundTag = AddSndObj(iWhichSound,0,VOLUME_NINETY);
#endif
				advisorFLCs[introPlaying].mfMove(0, INTRO_MOVE_AMOUNT); // move "forward"
			}
			
			if (introPlaying > 0)
			{
				advisorFLCs[introPlaying-1].mfMove(0, -INTRO_MOVE_AMOUNT);  // move back
			}
		}
		
		for (i=0; i < 4; i++)
		{
			advisorFLCs[i].mfDrawFrame();

			if (now >= advisortimer[i])
			{
				// don't fidget if someone else is speaking
				if (introPlaying > 3 || i == introPlaying)
				{
					advisorFLCs[i].mfSetCurFrame(random(advisorFLCs[i].mfTotalFrames()));
				}
				
				if (i == introPlaying)  // make this guy fidget more
					advisortimer[i] = now + 1 + random(3);
				else
					advisortimer[i] = now + ( (random(200)+100) * 182 / 1000);
			}
		}
	}
	NewTutorial(TRUE);
	
	init_gfont(FONT_SANS_8PT);

	// add each button region, paint the art and the text
	for (i=0; i<13; i++)
	{
		BOOL fBtnOn = TRUE;
		if (i != 12)			// not DONE button
		{
			// skip button if no site to describe
			if (i == 6
				&& ( advsite[iOldSite].available != SELECTABLE	// adventure news
	#ifdef _WINDOWS
					|| IsMultiPlayer()
	#endif
				)	)
			{
				fBtnOn = FALSE;
			}

			if (i == 11								// DM toolkit only between turns
					&& (dturn_mode != REPORT_MODE
						#ifdef _WINDOWS
						 || IsMultiPlayer()
						#endif
				)		)
				fBtnOn = FALSE;

			x = 40 + ((i % 4) * 144);
			y = 20 + ((i / 4) * 36);

			if (fBtnOn)
				add_region(x, y, w, h, 0, AdvisorButton, i, 0, 0, -1);
			DrawBitmap(x, (SHORT)y, iScrollBitm[4], 0, 0, 128, 24);
			print_text_centered(x+(w/2),y+(h/2),STRMGR_GetStr(iAdvButtonStr[i]), ((fBtnOn)?DKBROWN:210) );
		}
		else	// done button
		{
			x = 268;
			y = 440;
			add_region(x, y, w, h, 0, AdvisorButton, i, 0, 0, -1);
			DrawBitmap(x, (SHORT)y, iDoneBitm, 0, 0, 104, 24);
			print_text_centered(x+52,y+12,STRMGR_GetStr(iAdvButtonStr[i]),DKBROWN);
		}
	}

	// fully automatic play
	if (DoneTimer==0 && iHotSeatMultiplayerIndex == REALM::MIERES)
		DoneTimer = get_time() + (3L * 18L);
	if (DoneTimer && DoneTimer < get_time())
	{
		DoneTimer = 0;
		AdvisorButton(12,0);
	}


	SysShowCursor();
}

/* ========================================================================
   Function    -
   Description -
   Returns     -
   ======================================================================== */
void WhoControls (LONG i, LONG j)
{
	LONG	k;

	if (j==1) {realm[i].mfSetPlayerCtrl(1); realm[i].mfSetDMCtrl(0);}
	if (j==2) {realm[i].mfSetPlayerCtrl(1); realm[i].mfSetDMCtrl(1);}
	if (j==3) {realm[i].mfSetPlayerCtrl(0); realm[i].mfSetDMCtrl(0);}

	// reset to first player realm
	k = HomeRealm;
	iHotSeatMultiplayerIndex = 1;
	while (!realm[iHotSeatMultiplayerIndex].mfIsPlayerCtrl() && iHotSeatMultiplayerIndex<LAND_REALM_COUNT)
		iHotSeatMultiplayerIndex++;
	if (iHotSeatMultiplayerIndex == LAND_REALM_COUNT)
		iHotSeatMultiplayerIndex = REALM::MIERES;
	HomeRealm = (REALM::REALM_TYPE)(iHotSeatMultiplayerIndex);
	ActiveRegent = realm[HomeRealm].mfGetRegent();
	if (k != HomeRealm)
	{
		UpdateHoldingFlag();
		ReconstructMap();
	}
}

void DoNothing (LONG, LONG)
{
}

/* ========================================================================
   Function    -
   Description -
   Returns     -
   ======================================================================== */
// this routine paints all of the reports except loyalty
void PaintReportMenu (LONG MenuCombo, LONG)
{
	LONG	i, j, s, p, sz_type, last;
	LONG	X,Y;
	LONG	W,H;
	LONG	startY;
	LONG	top;
	LONG	MenuId, ButtonId;
	LONG	iStrPrinted = FALSE;
	CHAR	color = 228;
	CHAR	temp1[80];
	CHAR	temp2[80];

	// fully automatic play
	if (DoneTimer && iHotSeatMultiplayerIndex == REALM::MIERES)
		DoneTimer = get_time() + 1000000L;

	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	// get position of help screen
	if( GetButtonPosition( MenuId, 1, &X, &Y ) == fERROR)
		return;

	// get size of menu
	if( GetButtonSize( MenuId, 1, &W, &H ) == fERROR)
		return;

	top = Y;

	// get the position and size of the parchment scroll menu
	GetButtonPosition( MenuId, 0, &X, &Y );
	GetButtonSize( MenuId, 0, &W, &H );

	if(iWhichReport == 4)	// prov/holding graph
	{
		PaintProvinceTable(X, Y, W, H);
		goto DoTutorial;
	}

	if(iWhichReport == 9)	// troop graph (count of units)
	{
		PaintUnitsTable(X, Y, W, H);
		goto DoTutorial;
	}

	if(iWhichReport == 5)	// war report
	{
		PaintWarReport(X, Y, W, H);
		goto DoTutorial;
	}

	if(iWhichReport == 7)	// alliances
	{
		PaintAllianceReport(X, Y, W, H);
		goto DoTutorial;
	}

	if(iWhichReport == 10)	// investiture report
	{
		PaintInvestitureReport(X, Y, W, H);
		goto DoTutorial;
	}


	if(iWhichReport == 11)	// DM toolkit
	{
		del_region(WhoControls, 0);
		del_region(DoNothing, 0);
		add_region(X, Y, W, H-20, 0, DoNothing, 0, 0, 0, -1);
		Y += 14;
		X += 20;
		Y += print_textf_(X, Y, DKBROWN, "^fsans10.pcx^c60^xm160^jcDungeon Master Toolkit");
		Y += 2;
		for (i=1; i<LAND_REALM_COUNT; i++)
		{
			if (i!=REALM::MONSTERS && i!=REALM::MIERES
				&& (i!=REALM::CHIMAERON		|| realm[REALM::CHIMAERON].mfIsPlayable())
				&& (i!=REALM::FIVEPEAKS		|| realm[REALM::FIVEPEAKS].mfIsPlayable())
				&& (i!=REALM::MARKAZOR		|| realm[REALM::MARKAZOR].mfIsPlayable())
				&& (i!=REALM::MURKILAD		|| realm[REALM::MURKILAD].mfIsPlayable())
				&& (i!=REALM::SPIDERFELL	|| realm[REALM::SPIDERFELL].mfIsPlayable())
				)
			{
				if (realm[i].mfIsPlayable())
					print_textf_(X, Y, DKBROWN, "^fsans8.pcx%s^fsans5.pcx^B110,0,60,5,`,%ld,%ld,1,0,%ldPLAYER|^B180,0,60,5,`,%ld,%ld,2,0,%ldDM CONTROL|^B250,0,60,5,`,%ld,%ld,3,0,%ldCOMPUTER|",
						realm[i].mfGetName(),
						(LONG) WhoControls, i, (realm[i].mfIsPlayerCtrl() && !realm[i].mfIsDMCtrl()),
						(LONG) WhoControls, i, realm[i].mfIsDMCtrl(),
						(LONG) WhoControls, i, (!realm[i].mfIsPlayerCtrl() && !realm[i].mfIsDMCtrl()) );
				else
					print_textf_(X, Y, DKBROWN, "^fsans8.pcx%s^fsans5.pcx^c210^x140^xcNOT AVAIL|^c128^B180,0,60,5,`,%ld,%ld,2,0,%ldDM CONTROL|^B250,0,60,5,`,%ld,%ld,3,0,%ldCOMPUTER|",
						realm[i].mfGetName(),
						(LONG) WhoControls, i, realm[i].mfIsDMCtrl(),
						(LONG) WhoControls, i, (!realm[i].mfIsPlayerCtrl() && !realm[i].mfIsDMCtrl()) );
				Y+=11;
			}
		}
		Y = top + H - 37;
		Y += print_textf_(X, Y, DKBROWN, "^fsans5.pcx^w330SELECT ^c60PLAYER^c0 TO PLAY THAT REALM, ^c60DM CONTROL^c0 FOR UNLIMITED CONTROL OF A REALM, OR ^c60COMPUTER^c0 FOR AUTOMATIC OPERATION.");
		Y = top + H - 18;
		print_textf_(X, Y, DKBROWN, "^fsans8.pcx^B130,0,60,10,`,0,0,0,0,0DONE|");
		goto DoTutorial;
	}


	// ---------------------------------
	// print body of menu here
	// ---------------------------------
	init_gfont(FONT_SANS_12PT);

	Y += 20;
	X += 20;

	// intro text
	switch (iWhichReport)
	{
		case 0:	// chamberlan
			Y += print_textf(X, Y, DKBROWN, STRMGR_GetStr(STR_CHAMBERLAIN));
			break;

		case 1:	// general
			Y += print_textf(X, Y, DKBROWN, STRMGR_GetStr(STR_GENERAL));
			break;

		case 2:	// wizard
			Y += print_textf(X, Y, DKBROWN, STRMGR_GetStr(STR_WIZARD));
			break;

		case 3:	// spy
			Y += print_textf(X, Y, DKBROWN, STRMGR_GetStr(STR_SPY));
			break;
	}

	init_gfont(FONT_SANS_8PT);
	strcpy(temp1, STRMGR_GetStr(STR_YEAR));
	strcpy(temp2, STRMGR_GetStr(gsSeason[date%4]));
	Y += print_textf(X, Y, DKBROWN,
		temp1,
		temp2,
		551+(date/4))+4;
	startY = Y;

	if(iWhichReport < 4)	// intro text
		Y += print_textf(X, Y, DKBROWN, n);

	if(iWhichReport == 6)	// adventure news
		Y += print_textf(X, Y, DKBROWN, n1);

	print_textf(X+200, top+H-20, DKBROWN, STRMGR_GetStr(STR_CLICK));

DoTutorial:
	NewTutorial(TRUE);
}

/* ========================================================================
   Function    -
   Description -
   Returns     -
   ======================================================================== */
void DoneReportMenu (LONG arg, LONG arg1)
{
	// stop voice sound
	StopASound(iWhichSound, SoundTag);

	// get rid of old menu
	if(IsMenuActive(D_DRPRT))
	{
		HideMenu(D_DRPRT);
		RunMenus();
	}

	if(IsMenuActive(D_LOYALTY))
	{
		HideMenu(D_LOYALTY);
		RunMenus();
	}

	// show the buttons
	DrawBitmap(0, 0, ReportBkgndBitm, 0, 0, 999, 999);
	ShowMenu(D_ADVISOR);
	RunMenus();
}

void DoneReportMenu1 (LONG arg, LONG arg1)
{
	DoneReportMenu(arg, arg1);
}

/* ========================================================================
   Function    - PaintImmediateReport
   Description - paint a short report immediatly
   Returns     -
   ======================================================================== */
static void PaintImmediateReport (LONG MenuCombo, LONG)
{
	// set the statement text
	LONG	mx,my;
	LONG	xOff = 0;
	LONG	yOff = 0;
	LONG	MenuId, ButtonId;
	SHORT	bitm;
	char	szBitm[40];

//	LONG X;
//	LONG Y;
//	LONG W;
//	LONG H;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

//	X = X_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].X);
//	Y = Y_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].Y);
//	W = X_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].W);
//	H = Y_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].H);

	xOff = Menus[MenuId].Buttons[0].X;
	yOff = Menus[MenuId].Buttons[0].Y;

	// draw the bitmap
	sprintf(szBitm, "ui\\%s.pcx", szAdvisorPort[(iWhichVox * 4) + iWhichFace]);
	bitm = GetResourceStd (szBitm, FALSE);
 		
	if (bitm != fERROR)
	{
		DrawBitmap (20+xOff, 20+yOff, bitm, 0, 0, 50, 50);
		SetPurge(bitm);
	}
				
	// paint the message
	init_gfont(FONT_SANS_12PT);
	//print_textf(90+xOff, 50+yOff, DKBROWN, "^W220%s",n);
	print_textf(90+xOff, 38+yOff, DKBROWN, "^W220%s",n);

#ifdef _WINDOWS

   //---- Timeout menu in multiplayer

   if ( IsMultiPlayer () )
   {
       if ( lReportTimeout &&
            lReportTimeout < GetTickCount() )
       {
           DoneImmediateReport(0,0);
       }

   }

#endif

}

/* ========================================================================
   Function    - PaintLoyaltyTable
   Description -
   Returns     -
   ======================================================================== */
void PaintLoyaltyTable (LONG MenuCombo, LONG)
{
	SHORT	i, j;
	SHORT	k, l;
	SHORT	color;
	SHORT	iAm;
	LONG	MenuId, ButtonId;
	LONG	curReact;
	LONG	oldReact;
	int		index;

	LONG x;
	LONG y;
	LONG w;
	LONG h;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	x = Menus[MenuId].Buttons[0].X;
	y = Menus[MenuId].Buttons[0].Y;
	w = Menus[MenuId].Buttons[0].W;
	h = Menus[MenuId].Buttons[0].H;

	// print title
	print_textf(x+37, y+58, DKBLUE, STRMGR_GetStr(STR_LOYALTY_RPT));

	// find yourself
	l = 0;
	for (i=1; i<LAND_REALM_COUNT; i++)
	{
		if (i==REALM::ANUIRE || i==REALM::MIERES || i==REALM::MONSTERS)
			continue;

		if(i == HomeRealm)
		{
			iAm = l;
			break;
		}
		l++;
	}

	// draw lines for yourself
	// horizontal
	color_rect(x+98, y+117+(10*iAm), 315, 1, RED);

	// vertical
	color_rect(x+110+(10*iAm), y+105, 1, 315, RED);

	// for each down the side
	l = 0;
	for (i=1; i<LAND_REALM_COUNT; i++)
	{
		if (i==REALM::ANUIRE || i==REALM::MIERES || i==REALM::MONSTERS)
			continue;

		// don't show realms that no longer exist
		// also line out there name
		if(realm[i].mfExists() == 0)
		{
			color_rect(x+30, y+117+(10*l), 70, 1, DKBROWN);
			l++;
			continue;
		}

		if(i == HomeRealm)
			iAm = l;

		// for each across the top
		k = 0;
		for (j=1; j<LAND_REALM_COUNT; j++)
		{
			if (j==REALM::ANUIRE || j==REALM::MIERES || j==REALM::MONSTERS)
				continue;

			if(realm[j].mfExists() == 0)
			{
				color_rect(x+110+(10*k), y+40, 1, 70, DKBROWN);
				k++;
				continue;
			}

			if(j == i)
			{
				k++;
				continue;
			}

			curReact = RealmReaction[i][j];			// must be i, j
			oldReact = oldRealmReaction[i][j];		// must be i, j
			color = ReactionAndStatusColor(j, i);	// must be j, i

			index = 0;
			switch(color)
			{
				case 128: index = 0;	break;		// DKBROWN
				case 120: index = 1;	break;		// MDRED
				case 188: index = 2;	break;		// MDGREEN
				case 64:	 index = 3;	break;		// BLUE
				case 90:  index = 4;	break;		// DKPURPLE
				case 142: index = 5;	break;		// ORANGE
			}

			if (curReact == oldReact && color != DKBROWN)
			{
				// draw box
				color_rect(x+108+(10*k), y+115+(10*l), 4, 4, color);
			}
			else if (curReact < oldReact)
			{
				// draw down arrow
				DrawBitmap (x+107+(10*k), y+114+(10*l), iDownBitm[index], 0, 0, 8, 8);
			}
			else if (curReact > oldReact)
			{
				// draw up arrow
				DrawBitmap (x+107+(10*k), y+114+(10*l), iUpBitm[index], 0, 0, 8, 8);
			}

			// plot RED dot if screwed
			if (RealmStatus[i][j] & 0x80)
				plot(x+107+(10*k), y+114+(10*l)+7, 31);

			k++;
		}
		l++;
	}

	init_gfont(FONT_SANS_8PT);
	print_textf(x+29, y+429, DKBROWN, STRMGR_GetStr(STR_LOYAL_FOOTER));
	print_textf(x+29, y+437, DKBROWN, STRMGR_GetStr(STR_LOYAL_FOOTER1));

	NewTutorial(TRUE);
}

/* ========================================================================
   Function    - DrawPointsBar
   Description -
   Returns     -
   ======================================================================== */
static void DrawPointsBar (REALM::REALM_TYPE iRealm, SHORT scaleWidth, LONG x, LONG y)
{
	SHORT sMyProvinces = 0;
	SHORT sMyVassals = 0;
	SHORT sMyFull = 0;
	SHORT sMyPermissive = 0;
	SHORT sMyOther = 0;
	SHORT points;

	// get the points
	CalculateProvPoints(iRealm);

	sMyProvinces = sProvPoints;
	sMyVassals = sVassalPoints;
	sMyFull = sFullAllyPoints;
	sMyPermissive = sPermissivePoints;
	sMyOther = sOtherPoints;

	// scale it to 100% = scaleWidth points = 185 pixels
	// i.e.	width = (185 * value) / scaleWidth;

	// my provinces
	x += 140;
	points = (185 * sMyProvinces) / scaleWidth;
    color_rect(x, y+30, points, 5, DKBLUE);

	// vassals
	x += points;
	points = (185 * sMyVassals) / scaleWidth;
    color_rect(x, y+30, points, 5, DKPURPLE);

	// full allies
	x += points;
	points = (185 * sMyFull) / scaleWidth;
    color_rect(x, y+30, points, 5, BLUE);

	// permissive allies
	x += points;
	points = (185 * sMyPermissive) / scaleWidth;
    color_rect(x, y+30, points, 5, MDGREEN);

	// other provinces
	x += points;
	points = (185 * sMyOther) / scaleWidth;
    color_rect(x, y+30, points, 5, DKBROWN);
}

/* ========================================================================
   Function    - PaintProvinceTable
   Description -
   Returns     -
   ======================================================================== */
static void PaintProvinceTable (LONG x, LONG y, LONG w, LONG h)
{
	SHORT	i;
	SHORT	j = 0;
	SHORT	startY = y;
	SHORT	startX = x;
	SHORT	scoreWidth;
	SHORT	scaleWidth;
	SHORT	points;
	char	szRealm[40];

	// print title
	y += 20;
	print_textf(x+(w/2), y, DKBLUE, "^F11^c%s", STRMGR_GetStr(iAdvButtonStr[iWhichReport]));
	print_textf(x+w-100, y-8, DKBLUE, STRMGR_GetStr(STR_SCORE_HEADER), date+1);

	y += 10;
	x += 10;
	// print intro text
	print_textf(x+15, y, DKBROWN, "^W310^F02%s", STRMGR_GetStr(STR_SCORE_TEXT));

	// get the scale
	scaleWidth = CalculatePoints(HomeRealm);
	sMyScore = scaleWidth;

	for(i=0; i<NUMBER_OF_CT_SCORES; i++)
	{
		points = CalculatePoints(rCountScores[i].realm);
		if(points > scaleWidth)
			scaleWidth = points;
	}

	// round up to the nearest 10
	scaleWidth += 10 - (scaleWidth % 10);

	// show the scale bar
	print_textf(x+140+170, y+39, DKBROWN, "%d", scaleWidth);
	print_textf(x+140, y+39, DKBROWN, "0");
	print_textf(x+100, y+50, DKBROWN, "^F02%s", STRMGR_GetStr(STR_SCALE));
    color_rect(x+140, y+52, 185, 2, DKBROWN);
	
	y += 50;
	// print "score"
	//GEH print_textf(x+117, y+20, DKBROWN, "^F00%s", STRMGR_GetStr(STR_SCORE));
	print_textf(x+76, y+20, DKBROWN, "^F00%s", STRMGR_GetStr(STR_SCORE));

	// display the results
	// put yourself first
	DrawShield (x+3, y+30, HomeRealm);
	sprintf(szRealm, realm[HomeRealm].mfGetName());
	print_textf(x+15, y+30, DKBROWN, "^F02%s", strupr(szRealm));
	print_textf(x+117, y+30, DKBROWN, "%d", sMyScore);

	DrawPointsBar(HomeRealm, scaleWidth, x, y);

	// cap the holdings bar at 185
    color_rect(x+140, y+35, sMyHoldings > 185 ? 185 : sMyHoldings, 5, 112);

	y += 55;

	for(i=0; i<NUMBER_OF_CT_SCORES; i++)
	{
		// if there are less then 10, leave
		if(!rCountScores[i].count)
			break;

		// print the names down the side with the graph
		DrawShield (x+3, y+(i*20), rCountScores[i].realm);
		sprintf(szRealm, realm[rCountScores[i].realm].mfGetName());
		print_textf(x+15, y+(i*20), DKBROWN, "%s", strupr(szRealm));
		print_textf(x+117, y+(i*20), DKBROWN, "%d", rCountScores[i].count);

		DrawPointsBar(rCountScores[i].realm, scaleWidth, x, y-30+(i*20));

		// cap the bar at 185 holdings
	    color_rect(x+140, y+5+(i*20), sHoldings[rCountScores[i].realm] > 185 ? 185 : sHoldings[rCountScores[i].realm], 5, 112);
	}

	print_textf(x+25, startY+h-40, DKBROWN, STRMGR_GetStr(STR_SCORE_FOOTER1));
	print_textf(x+25, startY+h-30, DKBROWN, STRMGR_GetStr(STR_SCORE_FOOTER2));

}

/* ========================================================================
   Function    - PaintUnitsTable
   Description -
   Returns     -
   ======================================================================== */
static void PaintUnitsTable (LONG x, LONG y, LONG w, LONG h)
{
	SHORT	i;
	SHORT	j = 0;
	SHORT	startY = y;
	SHORT	startX = x;
	char	szRealm[40];

	// print title
	y += 20;
	print_textf(x+(w/2), y, DKBLUE, "^F11^c%s", STRMGR_GetStr(iAdvButtonStr[iWhichReport]));
	print_textf(x+w-100, y-8, DKBLUE, STRMGR_GetStr(STR_SCORE_HEADER), date+1);
//	print_textf(x+(w/2), y, DKBLUE, "^F11^c%s", STRMGR_GetStr(STR_UNITS_HEADER));

	y += 10;
	x += 10;
	// print intro text
	print_textf(x+15, y, DKBROWN, "^W310^F02%s", STRMGR_GetStr(STR_UNITS_TEXT));

	y += 30;

	// print "count"
	print_textf(x+117, y+20, DKBROWN, "^F00%s", STRMGR_GetStr(STR_COUNT));

	// display the results
	// put yourself first
	DrawShield (x+3, y+30, HomeRealm);
	sprintf(szRealm, realm[HomeRealm].mfGetName());
	print_textf(x+15, y+30, DKBROWN, "^F02%s", strupr(szRealm));
	print_textf(x+117, y+30, DKBROWN, "%d", sMyUnits);

	// cap the bar at 200 units
    color_rect(x+140, y+32, sMyUnits > 200 ? 200 : sMyUnits, 5, 112);

	y += 20;

	for(i=0; i<10; i++)
	{
		// if there are less then 10, leave
		if(!rCountUnits[i].count)
			break;

		// print the names down the side with the graph
		DrawShield (x+3, y+50+(i*20), rCountUnits[i].realm);
		sprintf(szRealm, realm[rCountUnits[i].realm].mfGetName());
		print_textf(x+15, y+50+(i*20), DKBROWN, "%s", strupr(szRealm));
		print_textf(x+117, y+50+(i*20), DKBROWN, "%d", rCountUnits[i].count);

		// cap the bar at 200 units
	    color_rect(x+140, y+52+(i*20), rCountUnits[i].count > 200 ? 200 : rCountUnits[i].count, 5, 112);
	}

	print_textf(x+200, startY+h-20, DKBROWN, STRMGR_GetStr(STR_UNITS_FOOTER));
}

#define CWIDTH	190
/* ========================================================================
   Function    - PrintAllies
   Description -
   Returns     - FALSE if at end of screen, else returns next y position
   ======================================================================== */
static int PrintAllies(REALM::REALM_TYPE iRealm, LONG x, LONG y, LONG endy, LONG column)
{
	int		k;
	char	szRealm[40];

	// print any vassals
	if(allies[iRealm].vassals)
	{
		for(k=1; k<LAND_REALM_COUNT; k++)
		{
			// some realms to not include
			if (k==REALM::ANUIRE || k==REALM::MIERES || k==REALM::MONSTERS)
				continue;
			if(realm[k].mfExists() == FALSE)
				continue;

			if (ISVASSAL(k,allies[iRealm].realm) && k != allies[iRealm].realm)
			{
				DrawShield (x+18+(column*CWIDTH), y, (REALM::REALM_TYPE)k);
				sprintf(szRealm, realm[k].mfGetName());
				print_textf(x+30+(column*CWIDTH), y, DKPURPLE, "^F02%s", strupr(szRealm));
				y += 10;

				if(y > endy)
					return FALSE;
			}
		}
	}

	// print any full allies
	if(allies[iRealm].full)
	{
		for(k=1; k<LAND_REALM_COUNT; k++)
		{
			// some realms to not include
			if (k==REALM::ANUIRE || k==REALM::MIERES || k==REALM::MONSTERS)
				continue;
  			if(realm[k].mfExists() == FALSE)
				continue;


			if (AREFULLALLIES(k,allies[iRealm].realm) && k != allies[iRealm].realm && !ISVASSAL(k,allies[iRealm].realm))
			{
				DrawShield (x+18+(column*CWIDTH), y, (REALM::REALM_TYPE)k);
				sprintf(szRealm, realm[k].mfGetName());
				print_textf(x+30+(column*CWIDTH), y, BLUE, "^F02%s", strupr(szRealm));
				y += 10;

				if(y > endy)
					return FALSE;
			}
		}
	}

	// print any permissive allies
	if(allies[iRealm].permisive)
	{
		for(k=1; k<LAND_REALM_COUNT; k++)
		{
			// some realms to not include
			if (k==REALM::ANUIRE || k==REALM::MIERES || k==REALM::MONSTERS)
				continue;
			if(realm[k].mfExists() == FALSE)
				continue;


			if (AREALLIED(k,allies[iRealm].realm) && k != allies[iRealm].realm && !AREFULLALLIES(k,allies[iRealm].realm) && !ISVASSAL(k,allies[iRealm].realm))
			{
				DrawShield (x+18+(column*CWIDTH), y, (REALM::REALM_TYPE)k);
				sprintf(szRealm, realm[k].mfGetName());
				print_textf(x+30+(column*CWIDTH), y, MDGREEN, "^F02%s", strupr(szRealm));
				y += 10;

				if(y > endy)
					return FALSE;
			}
		}
	}
	return y;
}


static SHORT ShowAtWar(REALM::REALM_TYPE me, BOOL hasBeenShown[], LONG x, LONG y)
{
	SHORT count = 0;
	SHORT testRealm;
	char  szRealm[40];
	UBYTE color;
	
	if (me == REALM::MONSTERS || me <= REALM::NO_COUNTRY)
		return 0;
	
	for (testRealm = REALM::FIRST_REALM; testRealm < LAND_REALM_COUNT; testRealm++)
	{
		if (testRealm == REALM::MONSTERS)
			continue;
			
		if (ISATWAR(me,testRealm))
		{
			if (hasBeenShown[testRealm] && hasBeenShown[me])
				continue;
			
			if (!hasBeenShown[me])
			{
				y += 5;
				DrawShield (x+3, y, me);
				sprintf(szRealm, realm[me].mfGetName());
				color = ReactionAndStatusColor(HomeRealm, me);
				print_textf(x+15, y, color, "^F03%s", strupr(szRealm));
				print_textf(x + CWIDTH - 16, y+9, DKBROWN, "^F10^c%s", STRMGR_GetStr(STR_VERSUS));
				
				hasBeenShown[me] = TRUE;
			}
			DrawShield (x+3+CWIDTH, y, (REALM::REALM_TYPE)testRealm);
			sprintf(szRealm, realm[testRealm].mfGetName());
			color = ReactionAndStatusColor(HomeRealm, testRealm);
			print_textf(x+15+CWIDTH, y, color, "^F03%s", strupr(szRealm));
			y += 15;
			count++;
			
			hasBeenShown[testRealm] = TRUE;
		}
	}
	return count;
}


/* ========================================================================
   Function    - PaintWarReport
   Description -
   Returns     -
   ======================================================================== */
static void PaintWarReport (LONG x, LONG y, LONG w, LONG h)
{
	SHORT	startY = y;
	SHORT	startX = x;
	char	szRealm[40];
	BOOL	printed = FALSE;
	int		iSwordDisplay = FALSE;
	BOOL	warShown[LAND_REALM_COUNT];
	LONG	i;
	
	for (i=0; i < LAND_REALM_COUNT; i++)
	{
		warShown[i] = FALSE;
	}

	// print title
	y += 20;
	print_textf(x+(w/2), y, DKBLUE, "^F11^c%s", STRMGR_GetStr(iAdvButtonStr[iWhichReport]));
	print_textf(x+w-100, y-8, DKBLUE, STRMGR_GetStr(STR_SCORE_HEADER), date+1);

	y += 10;
	x += 10;

	// print intro text
	print_textf(x+15, y, DKBROWN, "^W310^F02%s",STRMGR_GetStr(STR_REPORT_MAJOR_WARS_HELP));

	y += 40;

	// first, display the realms you're at war with
	{
		SHORT count = ShowAtWar(HomeRealm, warShown, x, y);
		if (count > 0)
		{
			printed = TRUE;
			y += count * 15 + 5;	
		}
	}
	
	// next, display the realms your allies are at war with
	for (i=REALM::FIRST_REALM; i < LAND_REALM_COUNT; i++)
	{
		if (y > startY + h - 50)
			break;
			
		// some realms to not include
		if (i==HomeRealm || !AREALLIED(HomeRealm, i))
			continue;
		if (i==REALM::ANUIRE || i==REALM::MIERES || i==REALM::MONSTERS)
			continue;
		if (warShown[i])
			continue;
		
		SHORT count = ShowAtWar((REALM::REALM_TYPE)i, warShown, x, y);
		if (count > 0)
		{
			printed = TRUE;
			y += count * 15 + 5;	
		}
	}
	
	// finally, display the rest of the realms
	for (i=REALM::FIRST_REALM; i < LAND_REALM_COUNT; i++)
	{
		if (y > startY + h - 50)
			break;
			
		if (i==REALM::ANUIRE || i==REALM::MIERES || i==REALM::MONSTERS)
			continue;
		if (warShown[i])
			continue;	
		
		SHORT count = ShowAtWar((REALM::REALM_TYPE)i, warShown, x, y);
		if (count > 0)
		{
			printed = TRUE;
			y += count * 15 + 5;	
		}
	}
	
	if(!printed)
		print_textf(x+15, y, DKBROWN, "^W310^F02%s", STRMGR_GetStr(STR_REPORT_PEACE_LATELY));

	print_textf(x+5, startY+h-20, DKBROWN, STRMGR_GetStr(STR_REPORT_COLORS));
}	

/* ========================================================================
   Function    - PaintAllianceReport
   Description -
   Returns     -
   ======================================================================== */
static void PaintAllianceReport(LONG x, LONG y, LONG w, LONG h)
{
	SHORT	i;
	SHORT	j;
	SHORT	k;
	SHORT	startY = y;
	SHORT	startX = x;
	SHORT	topY;
	char	szRealm[40];
	SHORT	column;
	int		lastY;

	// print title
	y += 20;
	print_textf(x+(w/2), y, DKBLUE, "^F11^c%s", STRMGR_GetStr(iAdvButtonStr[iWhichReport]));
	print_textf(x+w-100, y-8, DKBLUE, STRMGR_GetStr(STR_SCORE_HEADER), date+1);
//	print_textf(x+(w/2), y, DKBLUE, "^F11^c%s", STRMGR_GetStr(STR_REPORT_ALLIANCE_REPORT));

	y += 10;
	x += 10;
	// print intro text
	print_textf(x+15, y, DKBROWN, "^W310^F02%s",STRMGR_GetStr(STR_REPORT_ALLY_RPT_HELP));

	y += 40;
	topY = y;

	// display the results
	// put yourself first
	DrawShield (x+3, y, HomeRealm);
	sprintf(szRealm, realm[HomeRealm].mfGetName());
	print_textf(x+15, y, DKBROWN, "^F03%s", strupr(szRealm));

	y += 15;

	// start with first column
	column = 0;

	// now print your allies
	// find yourself
	for(i=0; i<LAND_REALM_COUNT-1; i++)
		if(allies[i].realm == HomeRealm)
			break;

	if((lastY = PrintAllies((REALM::REALM_TYPE)i, x, y, startY+h-30, column)) == FALSE)
	{
		column++;
		y = topY;

		if(column == 2)
			goto done;
	}
	else
		y = lastY;

	// print the other alliances
	for(i=0; i<LAND_REALM_COUNT-1; i++)
	{
		// don't include yourself
		if(allies[i].realm == HomeRealm)
			continue;
		// don't show realm which does not exist anymore
		if(realm[allies[i].realm].mfExists() == FALSE)
			continue;

		// some realms to not include
		if (allies[i].realm==REALM::ANUIRE || allies[i].realm==REALM::MIERES || allies[i].realm==REALM::MONSTERS)
			continue;

		if(allies[i].total)
		{
			// handle widows and orphans
			// see if this realm will fit on the screen
			if(y+20+(10*allies[i].total) > (startY+h-30))
			{
				column++;
				y = topY;

				if(column == 2)
					goto done;
			}

			y += 5;
			if(y > (startY+h-30))
			{
				column++;
				y = topY;

				if(column == 2)
					goto done;
			}

			DrawShield (x+3+(column*CWIDTH), y, allies[i].realm);
			sprintf(szRealm, realm[allies[i].realm].mfGetName());
			print_textf(x+15+(column*CWIDTH), y, DKBROWN, "^F03%s", strupr(szRealm));

			y += 15;
			if(y > (startY+h-30))
			{
				column++;
				y = topY;

				if(column == 2)
					goto done;
			}
		}
		else
			continue;

		if((lastY = PrintAllies((REALM::REALM_TYPE)i, x, y, startY+h-30, column)) == FALSE)
		{
			column++;
			y = topY;
	
			if(column == 2)
				goto done;
		}
		else
			y = lastY;
	}

done:
	print_textf(x+5, startY+h-20, DKBROWN,STRMGR_GetStr(STR_REPORT_COLORS));
}

/* ========================================================================
   Function    - PaintInvestitureReport
   Description -
   Returns     -
   ======================================================================== */
static void PaintInvestitureReport(LONG x, LONG y, LONG w, LONG h)
{
	SHORT	i;
	SHORT	j;
	SHORT	k;
	SHORT	startY = y;
	SHORT	startX = x;
	char	szRealm[40];
	SHORT	column = 0;
	SHORT	topY;
	SHORT	printed = FALSE;

	// print title
	y += 20;
	print_textf(x+(w/2), y, DKBLUE, "^F11^c%s", STRMGR_GetStr(iAdvButtonStr[iWhichReport]));
	print_textf(x+w-100, y-8, DKBLUE, STRMGR_GetStr(STR_SCORE_HEADER), date+1);
//	print_textf(x+(w/2), y, DKBLUE, "^F11^c%s",STRMGR_GetStr(STR_REPORT_INVESTITURE_REPORT));

	y += 10;
	x += 10;
	// print intro text
	print_textf(x+15, y, DKBROWN, "^W310^F02%s", STRMGR_GetStr(STR_REPORT_INVESTITURE_REPORT_HELP));

	y += 40;

	// see if there are any changes
	// after sorting the 0th element is valid
	// if the 0th element has a value (expanded) or the last element has a value (declined)
	// then there is something to print
	if(sortProvCount[0].count != 0 || sortProvCount[LAND_REALM_COUNT-1].count != 0)
	{
		printed = TRUE;

		print_textf(x+10, y, DKBROWN, "^F03%s",STRMGR_GetStr(STR_EXPANDED));
		column = 1;
		print_textf(x-30+(column*CWIDTH), y, DKBROWN, "^F03%s",STRMGR_GetStr(STR_DECLINED));
		y += 20;
		topY = y;

		// do expanded first
		column = 0;

		// after sorting the 0th element is valid
		for(i=0; i<LAND_REALM_COUNT; i++)
		{
			if(sortProvCount[i].count > 0)
			{
				// some realms to not include
				if (sortProvCount[i].realm==REALM::ANUIRE ||
					sortProvCount[i].realm==REALM::MIERES ||
					sortProvCount[i].realm==REALM::MONSTERS ||
					sortProvCount[i].realm==REALM::NO_COUNTRY ||
					sortProvCount[i].realm >= LAND_REALM_COUNT)
					continue;

				DrawShield (x+8+(column*CWIDTH), y, sortProvCount[i].realm);
				sprintf(szRealm, realm[sortProvCount[i].realm].mfGetName());
				print_textf(x+20+(column*CWIDTH), y, DKBROWN, "^F02%s", strupr(szRealm));
				print_textf(x+20+100+(column*CWIDTH), y, DKBROWN, "^F02+ %d", sortProvCount[i].count);
				y += 10;

				if(y > (startY+h-30))
					break;
			}
		}

		// do declined
		column = 1;
		y = topY;

		// print destroyed realms first
		for(i=1; i<LAND_REALM_COUNT; i++)
		{
			if(investProvCount[i].count == 0)
			{
				// some realms to not include
				if (investProvCount[i].realm==REALM::ANUIRE ||
					investProvCount[i].realm==REALM::MIERES ||
					investProvCount[i].realm==REALM::MONSTERS ||
					investProvCount[i].realm==REALM::NO_COUNTRY ||
					investProvCount[i].realm >= LAND_REALM_COUNT)
					continue;

				DrawShield (x-32+(column*CWIDTH), y, investProvCount[i].realm);
				sprintf(szRealm, realm[investProvCount[i].realm].mfGetName());
				print_textf(x-20+(column*CWIDTH), y, DKBROWN, "^F02%s", strupr(szRealm));
				print_textf(x-20+100+(column*CWIDTH), y, DKBROWN, "^F02%s",STRMGR_GetStr(STR_DESTROYED));
				y += 10;

				if(y > (startY+h-30))
					goto done;
			}
		}

		// now print declined
		for(i=LAND_REALM_COUNT-1; i>0; i--)
		{
			// when we reach a difference of 0, leave
			if(sortProvCount[i].count == 0)
				break;

			if(sortProvCount[i].count < 0)
			{
				// some realms to not include
				if (sortProvCount[i].realm==REALM::ANUIRE ||
					sortProvCount[i].realm==REALM::MIERES ||
					sortProvCount[i].realm==REALM::MONSTERS ||
					sortProvCount[i].realm==REALM::NO_COUNTRY ||
					sortProvCount[i].realm >= LAND_REALM_COUNT)
					continue;

				DrawShield (x-32+(column*CWIDTH), y, sortProvCount[i].realm);
				sprintf(szRealm, realm[sortProvCount[i].realm].mfGetName());
				print_textf(x-20+(column*CWIDTH), y, DKBROWN, "^F02%s", strupr(szRealm));
				print_textf(x-20+100+(column*CWIDTH), y, DKBROWN, "^F02- %d", abs(sortProvCount[i].count));
				y += 10;

				if(y > (startY+h-30))
					goto done;
			}
		}
	}

	if(!printed)
		print_textf(x+15, y, DKBROWN, "^W310^F02%s", STRMGR_GetStr(STR_REPORT_NO_NEW_INVESTITURES));

done:
	print_textf(x+200, startY+h-20, DKBROWN, STRMGR_GetStr(STR_UNITS_FOOTER));
}

/* ======================================================================== */
