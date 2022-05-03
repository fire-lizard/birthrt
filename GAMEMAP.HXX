/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _GAMEMAP_HXX
#define _GAMEMAP_HXX

/* -----------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_REALM_HXX)
#include "realm.hxx"
#endif

#if !defined(_VECTOR_HXX)
#include "vector.hxx"
#endif

#if !defined(_SNDVOX_HXX)
#include "sndvox.hxx"
#endif

#if !defined(_PROVDATA_HXX)
#include "provdata.hxx"
#endif

#if !defined(_MAPICON_HXX)
#include "mapicon.hxx"
#endif


/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

#define ICON_DIR			"UI\\"
#define ICON_EXT			".PCX"

#define REBEL				0
#define LO_LOY				1
#define AV_LOY				2
#define HI_LOY				3

#define START_NEW_OR_LOAD		999


#define BEFORE_LTACTION			0
#define DURING_LTACTION			1
#define AFTER_LTACTION			2

#define regFIRST_REGION_STATE   1000
#define regDOMAIN_TURN_MODE     1001
#define regACTION_MODE          1002
#define regMINIMIZED            1003
#define regMAXIMIZED            1004
#define regTAX_ROLLS            1005
#define regSWITCH_TO_ACTIONS    1006
#define regADJUST_TAXES         1007
#define regSHOW_HOLDINGS        1008

#define fSCENEBATTLE					1
#define fAUTOBATTLE					2
#define fNPCBATTLE					4


#define RESTORE_REGION_STATE(a) activate_region((a),region_state[a-regFIRST_REGION_STATE])

/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
	typedef enum {
		NO_COURT,
		MIN_COURT,
		QUAINT_COURT,
		AVE_COURT,
		OPULENT_COURT
	} COURT_TYPE;

	typedef enum {
		NO_ACTION = 0,
		ADVENTURE,
		AGITATE,
		BUILD_ROAD,
		CONTEST,
		CREATE_HOLDING,
		DECLARE_WAR,
		DIPLOMACY,
		ESPIONAGE,
		FORGE_LEY_LINE,
		FORTIFY,
		HOLD_ACTION,
		INVESTITURE,
		LIEUTANANT,
		LIEUTENANT_ACTION,
		MUSTER,
		REALM_SPELL,
		RULE,
		TRADE_ROUTE,
	} ACTION_TYPE;

/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
void BattlePrepProc(LONG, LONG);
void SetCommandBattlePaint(LONG, LONG);
void SetCommandBattleProc(LONG, LONG button);
SHORT UnitPower( SHORT UnitIndex );

void WriteRealmStartData ( void );

// Note: All the data and fns should be static because I can't see how we will have
//       more than one GAME going at a time. I used a class just to encapsulate
//       the data and fns.


class GAME_MAP {

public:
	static REALM::REALM_TYPE HomeRealm;

protected:
private:
	/* NOTE to Alan; leave this alone. This is list of things NOT to do */
	GAME_MAP();
	~GAME_MAP();
	void *operator new (size_t stAllocateBlock);	// Can't use new/delete
	void operator delete (void *p);
	GAME_MAP(const GAME_MAP &);			// No copies
	const GAME_MAP operator=(const GAME_MAP &);	// No assignments

};

SHORT CalculatePoints(REALM::REALM_TYPE CheckRealm);
SHORT CalculateProvPoints(REALM::REALM_TYPE CheckRealm);
SHORT CalculateRelicPoints(REALM::REALM_TYPE CheckRealm);
LONG ProvinceInfo (PROVINCE, MAP_ICON);
void MapLine (LONG, LONG, LONG, LONG, LONG, LONG, BOOL);
BOOL DoNPCAction(void);
char * UnitName (SHORT iUnit);
void DrawShield (LONG x, LONG y, REALM::REALM_TYPE rlm);
void CheckRealmStatus(LONG investor, LONG victim, BOOL fSend);
LONG CheckUnitArrival (LONG iMovingUnit, BOOL fJoinAllowed);
BOOL CheckUnit2ndMovePermitted(LONG iMovingUnit, LONG iNewProv);
void DrawBorders (LONG x, LONG y, LONG map_x, LONG map_y, LONG w, LONG h, LONG /* scale */);
void DrawRoads (PROVINCE prov);
PROVINCE HoldingToProv (LONG h);
LONG cntUnits_(); 
LONG maxNPCUnits();

void SetupAdvsitePlaces(void);	// from charsel.cpp

/* ------------------------------------------------------------------------
	Global Variables
   ------------------------------------------------------------------------ */
extern DECL_VECTOR_CLASS_S(CSTRPTR,GameIcons,MAP_ICON_COUNT);
extern DECL_VECTOR_CLASS(PROV_TYPE,province);
//extern DECL_VECTOR_CLASS(CSTRPTR,UnitTitle);
//extern DECL_VECTOR_CLASS(CSTRPTR,IconAbbriv);
//extern DECL_VECTOR_CLASS(CSTRPTR,szRace);
//extern DECL_VECTOR_CLASS(CSTRPTR, szBL_grade);
//extern DECL_VECTOR_CLASS(CSTRPTR, szClass);
//extern DECL_VECTOR_CLASS(CSTRPTR, szBL_deriv);
extern DECL_VECTOR_CLASS(SHORT,region_state);
//extern DECL_VECTOR_CLASS(CSTRPTR,TaxLevel);
extern DECL_VECTOR_CLASS(SHORT,LoyaltyAdj);
extern DECL_VECTOR_CLASS(SHORT,NoLawLoyaltyAdj);
extern DECL_VECTOR_CLASS(UBYTE,LoyaltyColor);
extern DECL_VECTOR_CLASS(UBYTE,MaintCost);
//extern DECL_VECTOR_CLASS(CSTRPTR, Court);
//extern DECL_VECTOR_CLASS(CSTRPTR, Terrain);
extern DECL_VECTOR_CLASS(SHORT,TerrainCost);
extern DECL_VECTOR_CLASS(SHORT,place_x);
extern DECL_VECTOR_CLASS(SHORT,place_y);
extern DECL_VECTOR_CLASS(SHORT,CourtCost);
extern DECL_VECTOR_CLASS(SHORT,UnitMaintCost);

#include "strenum.h"
extern DECL_VECTOR_CLASS(GAME_STRING,gsIconAbbriv);
extern DECL_VECTOR_CLASS(GAME_STRING,gsUnitTitle);
extern DECL_VECTOR_CLASS(GAME_STRING,gsTerrain);
extern DECL_VECTOR_CLASS(GAME_STRING,gsCourt);
extern DECL_VECTOR_CLASS(GAME_STRING,gsClass);
extern DECL_VECTOR_CLASS(GAME_STRING,gsBL_grade);
extern DECL_VECTOR_CLASS(GAME_STRING,gsBL_deriv);
extern DECL_VECTOR_CLASS(GAME_STRING,gsRace);

extern DECL_MATRIX_CLASS_S(UBYTE,Tribute,REALM::REALM_COUNT,REALM::REALM_COUNT);

extern DECL_VECTOR_CLASS(UBYTE,RealmSceneFlags);
extern VOX_TYPE GameIconVOX[];

extern SHORT			gWinningPoints;
extern SHORT			StatusColor[];
extern SHORT			iDCtrlIc;
extern SHORT			iDCtr2Ic;
extern REALM::REALM_TYPE HomeRealm;
extern SHORT            ActiveRegent;
extern LONG             dturn_mode;
extern LONG				date;
extern SHORT            iDTurnBk;
extern LONG             l_map;
extern SHORT            iLgMap;
extern LONG             req_l_map;
extern LONG             max_actions;
extern LONG             action_turn[];
extern SHORT            iProvSelect;
extern SHORT			fFinalRoll;
extern SHORT			fRoundOver;
extern BOOL				fUpdateDataArea;
extern BOOL				fDoActionIcon;
extern BOOL				fLoadedNewGame;
extern BOOL				fHeldAction;
extern SHORT			fLTAction;
extern SHORT			iCheck[];
extern LONG		   		prov_castle;
extern LONG		   		prov_law;
extern LONG		   		prov_temple;
extern LONG		   		prov_source;
extern LONG			fLgMap;
extern LONG			cntUnits;
extern LONG			cntHolds;
extern LONG			prov_units;
extern LONG			prov_guild;
extern SHORT		sProvPoints;
extern SHORT		sVassalPoints;
extern SHORT		sFullAllyPoints;
extern SHORT		sPermissivePoints;
extern SHORT		sOtherPoints;

extern LONG			trade_routes;
extern BOOL			fRedrawAll;
extern SHORT		iDRollIc;
extern SHORT		fFinalRoll;
extern SHORT		fJoinDialog;
extern SHORT		fCheckUnitArrival;
extern BOOL			fCommandBattleLoop;
extern BOOL			RegentKilled;
extern SHORT		iJoinDialog_to;
extern int			reqSound;
extern REALM::REALM_TYPE		CurrentRealm;
/* ------------------------------------------------------------------------
   inline functions
   ------------------------------------------------------------------------ */



#endif // _GAMEMAP_HXX

/*	======================================================================== */
