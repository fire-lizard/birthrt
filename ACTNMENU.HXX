/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _ACTNMENU_HXX
#define _ACTNMENU_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_GAMEMAP_HXX)
#include "provdata.hxx"
#endif

#if !defined(_REALM_HXX)
#include "realm.hxx"
#endif

#if !defined(_VECTOR_HXX)
#include "vector.hxx"
#endif

#if !defined(_MAPICON_HXX)
#include "mapicon.hxx"
#endif
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
// table of realm experience points
typedef enum {
RLM_EXP_AGITATE = 0,
RLM_EXP_BUILD_ROAD,
RLM_EXP_BUILD_TRADEROUTE,
RLM_EXP_CONTEST_GUILD,
RLM_EXP_CONTEST_LAW,
RLM_EXP_CONTEST_SOURCE,
RLM_EXP_CONTEST_TEMPLE,
RLM_EXP_CREATE_GUILD,
RLM_EXP_CREATE_LAW,
RLM_EXP_CREATE_SOURCE,
RLM_EXP_CREATE_TEMPLE,
RLM_EXP_DECLARE_WAR,
RLM_EXP_DIPLOMACY_PERMISSIVE,
RLM_EXP_DIPLOMACY_FULL,
RLM_EXP_DIPLOMACY_VASSALAGE,
RLM_EXP_ESPIONAGE_SPY,
RLM_EXP_ESPIONAGE_ASSASSINATE,
RLM_EXP_FORGE_LEYLINE,
RLM_EXP_INVEST,
RLM_EXP_REALM_SPELL,
RLM_EXP_RULE_PROV,
RLM_EXP_RULE_GUILD,
RLM_EXP_RULE_LAW,
RLM_EXP_RULE_SOURCE,
RLM_EXP_RULE_TEMPLE,
RLM_EXP_PASS,
NUM_OF_EXP_TYPES
} ACTION_EXPTYPE;

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
   Global Variables
   ------------------------------------------------------------------------ */
// Diplomacy structs
typedef struct
{
	unsigned int	iWhoFrom	: 6;
	unsigned int	iAlliance	: 2;
	unsigned int	iReqGold	: 5;
	unsigned int	iReqProv	: 8;
	unsigned int	iReqHold	: 10;
	unsigned int	iReqTrib	: 5;
	unsigned int	iOffProv	: 8;
	unsigned int	iOffHold	: 10;
	unsigned int	iOffTrib	: 5;
	unsigned int	iOffGold	: 5;
	unsigned int	iOffRegy	: 5;
} DIP_DATA;

typedef struct
{
	LONG	data1;
	LONG	data2;
	LONG	data3;
} DIP_DATA1;

typedef struct
{
	union
	{
		DIP_DATA  d1;
		DIP_DATA1 d2;
	};
} DIPLOMACY_DATA;


typedef struct
{
	LONG lFromId;
	LONG lFromRealm;
	LONG lWhichRealm;
	LONG lWhichAction;
	LONG lWhichProvince;
	LONG lWhichHold;
	LONG lSuccess;
	LONG lResult;
	UBYTE modRegency;
} RESPONSE_DIALOG;


extern DECL_VECTOR_CLASS(SHORT,UnitTypeID);
extern DECL_MATRIX_CLASS_S(UBYTE,iSelectedHolding,PROVINCE_COUNT,4);
extern DECL_VECTOR_CLASS_S(UBYTE,iLieutenants,9);
extern DECL_VECTOR_CLASS(MAP_ICON,InfoReqType);
extern DECL_VECTOR_CLASS(MAP_ICON,MonsterUnits);
extern DECL_VECTOR_CLASS(UBYTE,LevelUnitReqt);
extern DECL_VECTOR_CLASS(UBYTE,UnitMusterCost);

extern LONG fAllowRollButton;
extern LONG fAllowDoneButton;

extern SHORT	usedFreeAction;		/* from actnmenu.cpp */
extern SHORT	usedFreeForge;
extern SHORT	usedFreeMagic;
extern SHORT	usedFreeAgitate;
extern SHORT	usedFreeSpy;

extern UBYTE	modRegency;
extern LONG		modOpponent;

extern RESPONSE_DIALOG response[8];
extern LONG	curResponse;

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
BOOL CreateHolding (LONG type, LONG route1, LONG route2, PROVINCE prov, REALM::REALM_TYPE realm, BOOL fSend);
void DeleteHolding (PROVINCE iProv, LONG iHold, BOOL fSend );
BOOL RecursiveRoadFollower(PROVINCE);
void ReconstructMap (void);
void SetupMod (LONG x, LONG y, UBYTE * pNum, LONG NumMin, LONG Base, LONG dirFactor);
void CheckHolding(LONG place, REALM::REALM_TYPE realm_, BOOL fSend);
void CheckAndFixPlaces (PROVINCE prov);
LONG ActionExpPoints(LONG RegentIndex, ACTION_EXPTYPE ActionType, LONG Modifier, BOOL fSend );

/* ------------------------------------------------------------------------
   Inline functions   
   ------------------------------------------------------------------------ */
   
#endif // _ACTNMENU_HXX
