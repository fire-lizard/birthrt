/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: MAPAI.HXX
   Author:   Alan Clark

   ========================================================================
   Contains the following general functions:

   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_VECTOR_HXX)
#include "vector.hxx"
#endif

#if !defined(_STRMGR_H)
#include "strmgr.h"
#endif

#if !defined(_PROVDATA_HXX)
#include "provdata.hxx"
#endif

#if !defined(_REALM_HXX)
#include "realm.hxx"
#endif


/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
	typedef enum {
		MODE_ENTRENCH = 0,
		MODE_ECON_FTR,
		MODE_ECON_THF,
		MODE_ECON_PRST,
		MODE_ECON_WIZ,
		MODE_BUILD_FTR,
		MODE_BUILD_THF,
		MODE_BUILD_PRST,
		MODE_BUILD_WIZ,
		MODE_CTRL_FTR,
		MODE_CTRL_THF,
		MODE_CTRL_PRST,
		MODE_CTRL_WIZ,
		MODE_DEFEND,
		MODE_RAID,
		MODE_WAR_MIN_FTR,
		MODE_WAR_MED_FTR,
		MODE_WAR_MAX_FTR,
		MODE_WAR_WIZ,
		MODE_IRR_ANY,
		MODE_IRR_FTR,
		MODE_IRR_THF,
		MODE_IRR_PRST,
		MODE_IRR_WIZ,
		MODE_ELF_ENTRENCH,
		MODE_BUILD_ELF,
		MODE_DEFEND_ELF,
		MODE_RAID_ELF,
		MODE_WAR_ELF,
		MODE_DWARF_ENTRENCH,
		MODE_BUILD_DWARF,
		MODE_DEFEND_DWARF,
		MODE_RAID_DWARF,
		MODE_WAR_DWARF,
		MODE_GOBBO_ENTRENCH,
		MODE_BUILD_GOBBO,
		MODE_DEFEND_GOBBO,
		MODE_RAID_GOBBO,
		MODE_WAR_GOBBO,
		MODE_BUILD_SPIDER,
		MODE_RAID_SPIDER,
		MODE_WAR_SPIDER,
		MODE_BUILD_GORGON,
		MODE_RAID_GORGON,
		MODE_WAR_GORGON,
		MODE_DEFEND_GORGON,
		MODE_SCREW_BACK_BIGGER,
		MODE_SCREW_BACK_SMALLER,
		MODE_INVEST,

		MODE_COUNT
	} MODES_;

#define	NT		MODE_ENTRENCH
#define	EF		MODE_ECON_FTR
#define	ET		MODE_ECON_THF
#define	EP		MODE_ECON_PRST
#define	EW		MODE_ECON_WIZ
#define	BF		MODE_BUILD_FTR
#define	BT		MODE_BUILD_THF
#define	BP		MODE_BUILD_PRST
#define	BW		MODE_BUILD_WIZ
#define	CF		MODE_CTRL_FTR
#define	CT		MODE_CTRL_THF
#define	CP		MODE_CTRL_PRST
#define	CW		MODE_CTRL_WIZ
#define	DE		MODE_DEFEND
#define	RA		MODE_RAID
#define	W1		MODE_WAR_MIN_FTR
#define	W2		MODE_WAR_MED_FTR
#define	W3		MODE_WAR_MAX_FTR
#define	WW		MODE_WAR_WIZ
#define	IA		MODE_IRR_ANY
#define	IF		MODE_IRR_FTR
#define	IT		MODE_IRR_THF
#define	IP		MODE_IRR_PRST
#define	IW		MODE_IRR_WIZ
#define	EE		MODE_ELF_ENTRENCH
#define	BE		MODE_BUILD_ELF
#define	DF		MODE_DEFEND_ELF
#define	RE		MODE_RAID_ELF
#define	WE		MODE_WAR_ELF
#define	DT		MODE_DWARF_ENTRENCH
#define	BD		MODE_BUILD_DWARF
#define	DD		MODE_DEFEND_DWARF
#define	RD		MODE_RAID_DWARF
#define	WD		MODE_WAR_DWARF
#define	GT		MODE_GOBBO_ENTRENCH
#define	BG		MODE_BUILD_GOBBO
#define	DG		MODE_DEFEND_GOBBO
#define	RG		MODE_RAID_GOBBO
#define	WG		MODE_WAR_GOBBO
#define	BS		MODE_BUILD_SPIDER
#define	RS		MODE_RAID_SPIDER
#define	WS		MODE_WAR_SPIDER
#define	BN		MODE_BUILD_GORGON
#define	RN		MODE_RAID_GORGON
#define	WN		MODE_WAR_GORGON
#define	DN		MODE_DEFEND_GORGON
#define	SS		MODE_SCREW_BACK_SMALLER
#define	SB		MODE_SCREW_BACK_BIGGER
#if defined(IN)
#undef IN 
#endif
#define	IN		MODE_INVEST

	typedef enum {
/*00*/		ACTION_NONE = 0,
/*01*/		ACTION_MOVE_BORDER_FOE,
/*02*/		ACTION_MOVE_FILL_FOE,
/*03*/		ACTION_MOVE_REINFORCE_FOE,
/*04*/		ACTION_MOVE_UNOCCUP_FOE,
/*05*/		ACTION_MOVE_BORDER_OWN,
/*60*/		ACTION_MOVE_FILL_OWN,
/*07*/		ACTION_MOVE_REINFORCE_OWN,
/*08*/		ACTION_MOVE_UNOCCUP_OWN,
/*09*/		ACTION_MOVE_RETREAT,
/*10*/		ACTION_MOVE_ATTACK_FOE,
/*11*/		ACTION_MOVE_ATTACK_OWN,
/*12*/		ACTION_AGITATE_FOE,
/*13*/		ACTION_AGITATE_OWN,
/*14*/		ACTION_BUILDROAD,
/*15*/		ACTION_CONTEST_LAW,
/*16*/		ACTION_CONTEST_GLD,
/*17*/		ACTION_CONTEST_TMP,
/*18*/		ACTION_CONTEST_SRC,
/*19*/		ACTION_CREATE_LAW,
/*20*/		ACTION_CREATE_GLD,
/*21*/		ACTION_CREATE_TMP,
/*22*/		ACTION_CREATE_SRC,
/*23*/		ACTION_DECLAREWAR_UNOCCUP,
/*24*/		ACTION_DECLAREWAR_WEAK,
/*25*/		ACTION_DECLAREWAR_DIFTERRAIN,
/*26*/		ACTION_DECLAREWAR_BACKSTAB,
/*27*/		ACTION_DIPLMCY_ALLY,
/*28*/		ACTION_DIPLMCY_DMD_PROV,
/*29*/		ACTION_DIPLMCY_DMD_HOLD,
/*30*/		ACTION_DIPLMCY_DMD_TRIB,
/*31*/		ACTION_ESPIONAGE_ASSASSIN,
/*32*/		ACTION_FORTIFY,
/*33*/		ACTION_LEYLINE,
/*34*/		ACTION_INVESTITURE,
/*35*/		ACTION_LIEUTENANT,
/*36*/		ACTION_LTACT_,
/*37*/		ACTION_MUSTER_INF,
/*38*/		ACTION_MUSTER_EIN,
/*39*/		ACTION_MUSTER_BOW,
/*40*/		ACTION_MUSTER_IRR,
/*41*/		ACTION_MUSTER_PIK,
/*42*/		ACTION_MUSTER_LVY,
/*43*/		ACTION_MUSTER_SCT,
/*44*/		ACTION_MUSTER_KNT,
/*45*/		ACTION_MUSTER_CAV,
/*46*/		ACTION_MUSTER_MINF,
/*47*/		ACTION_MUSTER_MBOW,
/*48*/		ACTION_MUSTER_MIRR,
/*49*/		ACTION_MUSTER_MPIK,
/*50*/		ACTION_MUSTER_MCAV,
/*51*/		ACTION_MUSTER_EINF,
/*52*/		ACTION_MUSTER_EBOW,
/*53*/		ACTION_MUSTER_ECAV,
/*54*/		ACTION_MUSTER_DINF,
/*55*/		ACTION_MUSTER_DBOW,
/*56*/		ACTION_MUSTER_GINF,
/*57*/		ACTION_MUSTER_GBOW,
/*58*/		ACTION_MUSTER_GCAV,
/*59*/		ACTION_MUSTER_NINF,
/*60*/		ACTION_MUSTER_NIRR,
/*61*/		ACTION_MUSTER_CHPY,
/*62*/		ACTION_MUSTER_CSKL,
/*63*/		ACTION_MUSTER_CSPD,
/*64*/		ACTION_RSPELL_BLESSLAND,
/*65*/		ACTION_RSPELL_BLIGHT,
/*66*/		ACTION_RSPELL_WARDING,
/*67*/		ACTION_RSPELL_DEMAGOGUE,
/*68*/		ACTION_RSPELL_RAZE,
/*69*/		ACTION_RSPELL_STRONGHOLD,
/*70*/		ACTION_RSPELL_SUMMONING,
/*71*/		ACTION_RSPELL_BLESSARMY,
/*72*/		ACTION_RSPELL_DEATHPLAGUE,
/*73*/		ACTION_RSPELL_LEGIONOFTHEDEAD,
/*74*/		ACTION_RSPELL_MASSDESTRUCTION,
/*75*/		ACTION_RULE_PROV,
/*76*/		ACTION_RULE_LAW,
/*77*/		ACTION_RULE_GLD,
/*78*/		ACTION_RULE_TMP,
/*79*/		ACTION_RULE_SRC,
/*80*/		ACTION_TRADEROUTE,
/*81*/		ACTION_PASS,

				ACTION_COUNT
			} ACTIONS_;

#define	TEST_INVADED			(1)
#define	TEST_SCREWED			(1<<1)
#define	TEST_BUILDUP			(1<<2)
#define	TEST_ADJACENT			(1<<3)
#define	TEST_HATED				(1<<4)
#define	TEST_ALLY				(1<<5)
#define	TEST_NOT_ALLY			(1<<6)
#define	TEST_HATED_ATWAR		(1<<7)
#define	TEST_ALLY_ATWAR		(1<<8)
#define	TEST_NOUNITS			(1<<9)
#define	TEST_ECONBIGGER		(1<<10)
#define	TEST_ECONSMALLER		(1<<11)
#define	TEST_ECONMUCHSMALLR	(1<<12)
#define	TEST_BROKE				(1<<13)
#define	TEST_REBELLION			(1<<14)
#define	TEST_OCCUPIED			(1<<15)

#define	ALYWAR				(TEST_ALLY+TEST_ALLY_ATWAR)
#define	ALYWARADJ			(TEST_ALLY+TEST_ALLY_ATWAR+TEST_ADJACENT)
#define	BLDADJ				(TEST_BUILDUP+TEST_ADJACENT)
#define	BLDADJBIG			(TEST_BUILDUP+TEST_ADJACENT+TEST_ECONBIGGER)
#define	BRK					(TEST_BROKE)
#define	BRKADJSML			(TEST_BROKE+TEST_ADJACENT+TEST_ECONSMALLER)
#define	HATWARADJ			(TEST_HATED+TEST_HATED_ATWAR+TEST_ADJACENT)
#define	INV					(TEST_INVADED)
#define	INVBIG				(TEST_INVADED+TEST_ECONBIGGER)
#define	INVSML				(TEST_INVADED+TEST_ECONSMALLER)
#define	NUNADJ				(TEST_NOT_ALLY+TEST_NOUNITS+TEST_ADJACENT)
#define	NUNADJHAT			(TEST_NOT_ALLY+TEST_NOUNITS+TEST_ADJACENT+TEST_HATED)
#define	NUNADJSML			(TEST_NOT_ALLY+TEST_NOUNITS+TEST_ADJACENT+TEST_ECONSMALLER)
#define	OCC					(TEST_OCCUPIED)
#define	SCRADJ				(TEST_SCREWED+TEST_ADJACENT)
#define	SCRADJBIG			(TEST_SCREWED+TEST_ADJACENT+TEST_ECONBIGGER)
#define	SCRADJSML			(TEST_SCREWED+TEST_ADJACENT+TEST_ECONSMALLER)
#define	SCRBIG				(TEST_SCREWED+TEST_ECONBIGGER)
#define	SCR					(TEST_SCREWED)


/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
typedef struct {
	USHORT		Tests;
	UBYTE			Action;
	UBYTE			Chance;
} TRIGGER;

typedef struct {
	UBYTE			CurrentMode;
	UBYTE			CurrentAction;
	UBYTE			Fickleness;
	UBYTE			OldAtWar;
	UBYTE			Modes[10];
	UBYTE			Chance[10];
	TRIGGER		Triggers[8];
} NPC_AI, *NPC_AI_PTR;

typedef struct {
	UBYTE			Chance;
	UBYTE			Action;
} AI_ACTION;

#if defined(_VECTOR_CHK)
typedef BrVector<AI_ACTION> AI_ACTION_PTR;
#else
typedef AI_ACTION * AI_ACTION_PTR;
#endif

class ACTION_STR {
public:
	char * mfGetStr() const;
	LONG	fActionStrId;
};

inline char * ACTION_STR::mfGetStr() const
{
	return STRMGR_GetStr(fActionStrId);
}

class NPC_ACTION_DATA {
public:
	inline NPC_ACTION_DATA();
	
	inline	BOOL const mfIsLtActionAvailable() const;
	inline	LONG const mfFreeForgesUsed() const;
	inline	LONG const mfFreeMagicUsed() const;
	inline	BOOL const mfFreeAgitateUsed() const;
	inline	BOOL const mfFreeSpyUsed() const;
	inline	LONG const mfFreeAction() const;
	
	inline	void mfSetLtUsed();
	inline	void mfIncFreeForge();
	inline	void mfIncMagicUsed();
	inline	void mfSetAgitateUsed();
	inline	void mfSetSpyUsed();
	inline	void mfIncFreeAction();
	
	inline	void mfReset();
protected:
private:
	UBYTE	fusedLtAction;
	UBYTE	fusedFreeForge;
	UBYTE	fusedFreeMagic;
	UBYTE	fusedFreeAgitate;
	UBYTE	fusedFreeSpy;
	UBYTE	fusedFreeAction;
};

inline NPC_ACTION_DATA::NPC_ACTION_DATA() :
	fusedLtAction(0),
	fusedFreeForge(0),
	fusedFreeMagic(0),
	fusedFreeAgitate(0),
	fusedFreeSpy(0),
	fusedFreeAction(0)
{}

inline BOOL const NPC_ACTION_DATA::mfIsLtActionAvailable() const
{
	return 1 == fusedLtAction;
}

inline LONG const NPC_ACTION_DATA::mfFreeForgesUsed() const
{
	return (LONG const) fusedFreeForge;
}

inline LONG const NPC_ACTION_DATA::mfFreeMagicUsed() const
{
	return (LONG const) fusedFreeMagic;
}

inline BOOL const NPC_ACTION_DATA::mfFreeAgitateUsed() const
{
	return 1 == fusedFreeAgitate;
}

inline BOOL const NPC_ACTION_DATA::mfFreeSpyUsed() const
{
	return 1 == fusedFreeSpy;
}

inline LONG const NPC_ACTION_DATA::mfFreeAction() const
{
	return (LONG const) fusedFreeAction;
}

inline void NPC_ACTION_DATA::mfSetLtUsed()
{
	fusedLtAction = 1;
}

inline void NPC_ACTION_DATA::mfIncFreeForge()
{
	++fusedFreeForge;
}

inline void NPC_ACTION_DATA::mfIncMagicUsed()
{
	++fusedFreeMagic;
}

inline void NPC_ACTION_DATA::mfSetAgitateUsed()
{
	fusedFreeAgitate = 1;
}

inline void NPC_ACTION_DATA::mfSetSpyUsed()
{
	fusedFreeSpy = 1;
}

inline void NPC_ACTION_DATA::mfIncFreeAction()
{
	++fusedFreeAction;
}

inline void NPC_ACTION_DATA::mfReset()
{
	fusedLtAction = 0;
	fusedFreeForge = 0;
	fusedFreeMagic = 0;
	fusedFreeAgitate = 0;
	fusedFreeSpy = 0;
	fusedFreeAction = 0;
}

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
extern DECL_VECTOR_CLASS_S(UBYTE,RealmOrder,LAND_REALM_COUNT);
extern DECL_VECTOR_CLASS(UBYTE,fProv);

extern DECL_VECTOR_CLASS(NPC_ACTION_DATA, npcActionData);
extern DECL_VECTOR_CLASS(NPC_AI,npc_ai);
extern DECL_VECTOR_CLASS(ACTION_STR,szAIAction);

#if defined(_VECTOR_CHK)
extern BrVector < BrVector < AI_ACTION > > Modes;
#else
extern AI_ACTION_PTR Modes[];
#endif

extern LONG		global_action_turn;
extern LONG		iCurrentRealmIndex;

void SetHelped(REALM::REALM_TYPE Target, REALM::REALM_TYPE Active, BOOL fSend ); 
void SetScrewed(REALM::REALM_TYPE Target, REALM::REALM_TYPE Active, BOOL fSend );
LONG ProvDist(LONG cur_prov, LONG dest_prov, LONG cProvDist, BOOL);
BOOL TestAdjacent(REALM::REALM_TYPE ActiveRealm, REALM::REALM_TYPE testRealm);
LONG SpendNPCRegency (LONG who, LONG rp, BOOL fSend);
LONG OfferHolding(void);
BOOL CanAfford (LONG costGBNow, LONG costRegNow, LONG costGBPerTurn, REALM::REALM_TYPE realm_);
LONG DemandHolding(REALM::REALM_TYPE realm_, SHORT oldDemand);
void ResetNPCActionData();
BOOL ForeignUnitsHere (PROVINCE prov, REALM::REALM_TYPE _realm);

/* ======================================================================== */
