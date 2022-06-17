/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: MAPAI.CPP
   Author:   Alan Clark & David Jackson

   ========================================================================
   Contains the following general functions:

		void DoNPCAdmin (void)
		BOOL DoNPCAction (void)

   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#ifdef _WINDOWS
#include <Windows.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "SYSTEM.H"
#include "MAPAI.HXX"
#include "ACTNMENU.HXX"
#include "CHARSEL.HXX"
#include "GAMEMAP.HXX"
#include "GMENUENM.H"
#include "HANDLE.HXX"
#include "ITEMUTIL.HXX"
#include "MAIN.HXX"
#include "MULTIMAP.HXX"
#include "MULTIUI.HXX"
#include "REALM.HXX"
#include "REPORT.HXX"
#include "SOUND.HXX"
#include "STRMGR.H"
#include "strenum.h"
#include "PLAYSTAT.HXX"
#include "REGENTS.HXX"
#include "REQUEST.H"
#include "PLACES.HXX"

#ifdef _WINDOWS
#include "WINSYS\MULPLAY.HXX"
#include "WINSYS\MONO_C.H"
#endif

extern BOOL fLogComment;

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
//#define  LOGCOMMENTSLEEP	Sleep(1);
#define  LOGCOMMENTSLEEP	{;}
#define FLOGCOMMENT 		// uncomment  & unquote to remove comments "if (0)

#define D20TOPERCENT(a)		((mundane_descriptions)?(105-((a)*5)):(a))
#define PERCENTCHAR			((mundane_descriptions)?'%':0)

#define MUSTERINFO		0

#define MDBROWN			136

typedef enum {
	VIA_ROAD =	1,
	ANY_WAY	= 2
	} PATHTYPE;


#define NOAUTOMP           1

#define PERCENTCHAR			((mundane_descriptions)?'%':0)

#undef PURPLE_GRAD
#define PURPLE_GRAD				9

#undef RED_GRAD
#define RED_GRAD					3

#undef BLUE_GRAD
#define BLUE_GRAD					0


/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
void WarDeclared(REALM::REALM_TYPE who, REALM::REALM_TYPE onWhom, BOOL fSend);	// from actnmenu.cpp
static void RollNPCAction (LONG, LONG );
void IncNum (LONG lpNum, LONG);
void DecNum (LONG lpNum, LONG);
static BOOL DoIHavePlace (PROVINCE prov, REALM::REALM_TYPE placerealm, MAP_ICON icon);
static BOOL FindHolding (REALM::REALM_TYPE provrealm, REALM::REALM_TYPE placerealm, PROVINCE prov, MAP_ICON icon, LONG fMax);
BOOL CheckUnit2ndMoveBlocked(LONG iMovingUnit, PROVINCE iCurProv, PROVINCE iNewProv);
void DrawRoads_ (PROVINCE prov, LONG border);
void DisplayProcess (LONG mode, LONG color, LONG index1, LONG index2);
void UpdateUnitAndPlaceCount (void);
BOOL CheckNotWarded (LONG iMovingUnit, PROVINCE iNewProv, PROVINCE iOldProv);
void PaintOthersDiplomacy (LONG MenuCombo, LONG menuResponse);
void CheckAndFixSourceLevels (PROVINCE iProv);
BOOL CheckVassal ( REALM::REALM_TYPE rlm );
PROVINCE DemandProvince();
LONG FindProvForDemand (PROVINCE prov, LONG base);
void UpdateHoldingFlag (void);

#ifdef _WINDOWS
void BroadcastAllienceTable ( void );
#endif

extern void SysForceCursor (void);

extern void RandomLogComment ( char * szString );
extern "C" void RandomLogPrefix ( char * szString );



/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
// debug strings, don't translate

DEFINE_VECTOR_DATA(CSTRPTR,szAIMode) = {
	"ENTRENCH",
	"ECON_FTR","ECON_THF","ECON_PRST","ECON_WIZ",
	"BUILD_FTR","BUILD_THF","BUILD_PRST","BUILD_WIZ",
	"CTRL_FTR","CTRL_THF","CTRL_PRST","CTRL_WIZ",
	"DEFEND","RAID","WAR_MIN_FTR","WAR_MED_FTR","WAR_MAX_FTR","WAR_WIZ",
	"IRR_ANY","IRR_FTR","IRR_THF","IRR_PRST","IRR_WIZ",
	"ELF_ENTRENCH","BUILD_ELF","DEFEND_ELF","RAID_ELF","WAR_ELF",
	"DWARF_ENTRENCH","BUILD_DWARF","DEFEND_DWARF","RAID_DWARF","WAR_DWARF",
	"GOBBO_ENTRENCH","BUILD_GOBBO","DEFEND_GOBBO","RAID_GOBBO","WAR_GOBBO",
	"BUILD_SPIDER","RAID_SPIDER","WAR_SPIDER",
	"BUILD_GORGON","RAID_GORGON","WAR_GORGON","DEFEND_GORGON",
	"SCREW_BACK_BIGGER","SCREW_BACK_SMALLER",
	"INVEST" };
DEFINE_VECTOR_CLASS(CSTRPTR, szAIMode);


LONG		global_action_turn = 1;
LONG		prev_action_turn[LAND_REALM_COUNT] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

DEFINE_VECTOR_DATA_S(UBYTE,RealmOrder,LAND_REALM_COUNT);
DEFINE_VECTOR_CLASS(UBYTE,RealmOrder);

DEFINE_MATRIX_DATA_S(UBYTE,fAllowed,3,ACTION_COUNT) = {  /*
 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3 3 3 3 3 3 3 3 3 3 4 4 4 4 4 4 4 4 4 4 5 5 5 5 5 5 5 5 5 5 6 6 6 6 6 6 6 6 6 6 7 7 7 7 7 7 7 7 7 7 8 8
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 */
{1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1},
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};
DEFINE_MATRIX_CLASS_S(UBYTE, fAllowed, 3, ACTION_COUNT);

static PROVINCE				UnitProvince = NO_PROVINCE;
LONG				iCurrentRealmIndex = 0;
static REALM::REALM_TYPE	TargetRealm = REALM::NO_COUNTRY;
PROVINCE				TargetProvince = NO_PROVINCE;
static PROVINCE	StartProvince = NO_PROVINCE;
USHORT			TargetPlace = 0;
USHORT 	StartPlace = 0;
USHORT			TargetUnit = 0;
static REALM::REALM_TYPE		TestTargetRealm = REALM::NO_COUNTRY;
static REALM::REALM_TYPE		TriggeredTargetRealm = REALM::NO_COUNTRY;
static USHORT	cGroups;
static LONG		strTargetProvince;
static LONG		timeCount=0;
static LONG		old_dist = 99999999;
static BOOL		fDoneOthersAction;
DEFINE_VECTOR_DATA_S(UBYTE,fProv,PROVINCE_COUNT);
DEFINE_VECTOR_CLASS(UBYTE, fProv);
//extern DECL_VECTOR_CLASS(CSTRPTR,szHoldType);
extern DECL_VECTOR_CLASS(GAME_STRING,gsHoldType);
LONG				BattleSync;
LONG				OldNPCRegency;
LONG				OldNPCRealm;

extern LONG			fControlMode;
extern int			reqSound;
extern DECL_VECTOR_CLASS(SHORT,province_tax);
extern SHORT	    fRoundOver;
extern SHORT		cProvHolds;
extern SHORT		cProvinces;
extern SHORT		cCastles;
extern SHORT		cOccupiedProv;
extern LONG			domain_power;
extern LONG			realm_tax;
extern LONG			realm_reg;
extern LONG			domain_maint;
extern LONG			troop_moves;
extern LONG			payroll;
extern LONG			payregency;
extern LONG			new_treasury;
extern LONG			prov_tax, prov_trade, prov_reg;
extern REALM::REALM_TYPE		CurrentRealm;
extern BOOL			fPlayerNeverDeclaredWar;
extern DIPLOMACY_DATA 	dData;
extern volatile BOOL	fAnyKeyChanged;
extern BOOL			fTutorialSelected;
extern BOOL			mundane_descriptions;
extern LONG			fOtherDipMP;
extern BOOL			fSyncPending;
extern int			iTTimeLimit;
extern LONG			WaitingCount;
extern LONG			WaitingWho;
extern LONG			WaitingWhen;
extern LONG			WaitingBar216ths;
extern SHORT		iDMapBx;

/* ======================================================================== */

void logreroll( SHORT LineNum )
{
	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
   FLOGCOMMENT
   {
		char temp[100];
		sprintf ( temp, "DoNPCAction - Reroll  line:%06d", LineNum);
		if ( fLogComment )
			RandomLogComment ( temp );
		else
			Sleep(0);
	}
	#endif
}

/* ========================================================================
   Function    - 
   Description - do AI for all NPC realms
   Returns     -
   ======================================================================== */
void DoNPCAdmin (void)
{
	LONG	i, j, k, l, rlm, hr, fr, iUnit, roll, surplus;

	hr = HomeRealm;
	fr = fFinalRoll;

	// clear the realm order list
	for (i=REALM::FIRST_REALM; i<LAND_REALM_COUNT; ++i)
		RealmOrder[i]=REALM::REALM_OUT_OF_RANGE;
	RealmOrder[0] = REALM::NO_COUNTRY;

	// order for DM controlled realms
	for (i=rlm=REALM::FIRST_REALM; rlm<LAND_REALM_COUNT; ++rlm)
	 	if (realm[rlm].mfExists() && realm[rlm].mfIsDMCtrl())
			RealmOrder[i++] = rlm;

	// do admin for NPC realms
	for (rlm=REALM::FIRST_REALM; rlm<LAND_REALM_COUNT; ++rlm)
	{
		run_timers();  		// cdb 11/27	// ABC: don't put in inner loops

	#ifdef _WINDOWS
       AMultiPlayer.LookForMessages();   //---- Update not specific messages
	#endif

	 	if (realm[rlm].mfExists())				// don't do destroyed realms
		{
	#ifdef _WINDOWS
			//---- Set next random Seed
			AMultiPlayer.NextSeed(__FILE__,__LINE__);
	#endif

			// don't do player realms in advanced or expert mode
			if (!realm[rlm].mfIsPlayerCtrl() || fControlMode == 0)
			{

				#ifdef _WINDOWS
				LOGCOMMENTSLEEP
				FLOGCOMMENT
				{
					char temp[100];
					sprintf ( temp, "DoNPCAdmin %d - %s  starting regency:%04d  treasury:%06d",
							1,
							realm[rlm].mfGetName(),
							realm[rlm].mfGetRegency(),
							realm[rlm].mfGetTreasury());
					if ( fLogComment )
						RandomLogComment ( temp );
					else
						Sleep(0);
				}
				#endif

				HomeRealm = (REALM::REALM_TYPE)rlm;
				fFinalRoll = 999;
				cOccupiedProv = 0;
				cCastles = 0;
				cProvHolds = 0;
				realm_tax = 0;
				trade_routes = 0;
				domain_power = 0;
				cProvinces = 0;

				// give NPCs a little boost
				if ( !realm[rlm].mfIsPlayerCtrl() )
				{
					i = ( (game_difficulty == DIF_EASY)?5:((game_difficulty == DIF_MEDIUM)?10:15) ) * date;
					realm[rlm].mfAddTreasury(i);

					if (random(5)==0)						// 20% chance of 50GB boost
						realm[rlm].mfAddTreasury(500);
				}

				// get income and expenses for provinces
				for (i=1; i<PROVINCE_COUNT; ++i)
				{
					if (province[i].Realm == rlm) ++cProvHolds;

					province_tax[cProvinces] = -1;
					ProvinceInfo((PROVINCE)i, CASTLE1_ICON);
					if (prov_castle>0) ++cCastles;
					if (prov_law>0)	 ++cProvHolds;
					if (prov_temple>0) ++cProvHolds;
					if (prov_guild>0)	 ++cProvHolds;
					if (province[i].OccupRealm==rlm) ++cOccupiedProv;

					if (province[i].Realm == rlm)
					{
						l = (province[i].Contested) ? 0 : LoyaltyAdj[province[i].TaxLevel];
						l -= (province[i].OccupRealm != 0);
						j = (province[i].OccupRealm==rlm) ? prov_units : prov_law;
						if (j >= province[i].CivLevel)		// all law available
						{
							if (l<0) ++l; if (l<0) ++l;
						}
						else if ((j*2)>=province[i].CivLevel)	// half of all law
						{
							if (l<0) ++l;
						}
						else											// less than half of law
						{
							l = NoLawLoyaltyAdj[province[i].TaxLevel] - (province[i].OccupRealm!=0);
						}

						#ifdef _WINDOWS
						LOGCOMMENTSLEEP
						FLOGCOMMENT
						{
							char temp[100];
							sprintf ( temp, "DoNPCAdmin %d - %s:%s  prov_tax:%04d  prov_trade:%04d  prov_reg:%04d",
								2,
								realm[rlm].mfGetName(),
								province[i].name,
								prov_tax,
								prov_trade,
								prov_reg);
							if ( fLogComment )
								RandomLogComment ( temp );
							else
								Sleep(0);
						}
						#endif
					}

					realm_tax += prov_tax;
					trade_routes += prov_trade;
					domain_power += prov_reg;
				}

				// calc payroll
				payroll = payregency = 0;
				if (rlm != REALM::MONSTERS				// monsters don't have payroll
					&& rlm != REALM::SPIDERFELL		// awnsheigh don't have payroll
					&& rlm != REALM::RHUOBHE			// 
					&& rlm != REALM::MARKAZOR			// 
					&& rlm != REALM::MURKILAD			// 
					&& rlm != REALM::GORGONSCROWN)	// 
				{
					for (i=1; i<MAX_UNITS; ++i)
					{
						// scan through units for leaders
						if (units[i].Realm == rlm && units[i].Joined == 0)
						{
							iUnit = i;
							while (iUnit != -1)		// list followers
							{
								//if (units[iUnit].Paid)
									payroll += UnitMaintCost[units[iUnit].Icon];
								//if (units[iUnit].Use_Regency)
								//	payregency += UnitMaintCost[units[iUnit].Icon] * 5;
								iUnit = units[iUnit].NextUnit;
							}
						}
					}
				}

				if (cProvHolds > 100)
					domain_maint = (cProvHolds/3) + cCastles + cOccupiedProv + CourtCost[realm[rlm].mfGetCourt()];
				else
					domain_maint = MaintCost[cProvHolds] + cCastles + cOccupiedProv + CourtCost[realm[rlm].mfGetCourt()];

				//printf("  Maint - total:%d  MaintCost:%d  Castles:%d  OccProv:%d  Court:%d\n",domain_maint,MaintCost[cProvHolds],cCastles,cOccupiedProv,CourtCost[realm[rlm].mfGetCourt()]);
				realm_reg = MIN(domain_power,regents[realm[rlm].mfGetRegent()].mfGetBL_strength()) + realm[rlm].mfGetRegency() - payregency;
				//printf("  Regency - total:%d  oldRegency:%d  addRegency:%d  payregency:%d\n",realm_reg,realm[rlm].mfGetRegency(),MIN(domain_power,regents[realm[rlm].mfGetRegent()].BL_strength),payregency);

				//new_treasury = realm[rlm].mfGetTreasury() + ((realm_tax + law_holding_claims
				//		+ trade_routes - payroll - domain_maint)*10);
				realm[rlm].mfSetIncome(realm_tax + trade_routes - payroll - domain_maint);
				surplus =  realm[rlm].mfGetTreasury();
				new_treasury = realm[rlm].mfGetTreasury() + (realm[rlm].mfGetIncome() * 10);
				//printf("  Treas - total:%d  oldTreasury:%d  +realmTax:%d  +trade:%d  -payroll:%d  -maint:%d\n",new_treasury,realm[rlm].mfGetTreasury(),realm_tax*10,trade_routes*10,payroll*10,domain_maint*10);

				#ifdef _WINDOWS
				LOGCOMMENTSLEEP
				FLOGCOMMENT
				{
					char temp[100];
					sprintf ( temp, "DoNPCAdmin %d - %s  realm_tax:%04d  trade:%04d  payroll:%04d  maint:%04d  Income:%04",
						3,
						realm[rlm].mfGetName(),
						realm_tax,
						trade_routes,
						payroll,
						domain_maint,
						realm[rlm].mfGetIncome() );
					if ( fLogComment )
						RandomLogComment ( temp );
					else
						Sleep(0);

					sprintf ( temp, "                    HoldingsMaintCost[%ld]:%04d  castles:%04d  OccupPr:%04d  court:%04d",
						cProvHolds,MaintCost[cProvHolds],cCastles,cOccupiedProv,CourtCost[realm[rlm].mfGetCourt()] );
					if ( fLogComment )
						RandomLogComment ( temp );
					else
						Sleep(0);
				}
				#endif

				// if we are low on GB (due most likely to troops)
				if (new_treasury < 15)
				{
					// convert some regency to GB
					if (realm_reg > 25)
					{
						new_treasury += (realm_reg-25) / 5;
						realm_reg -= ((realm_reg-25) / 5) * 5;
					}

					// disband troops troops to get upto zero
					for (i=1; i<MAX_UNITS; ++i)
					{
						if (new_treasury >= 15)
							break;

						// scan through units for units to disband
						if (units[i].Realm == rlm && UnitMaintCost[units[i].Icon])
						{
							new_treasury += UnitMaintCost[units[i].Icon];
							DeleteUnit(i,FALSE);

							#ifdef _WINDOWS
							LOGCOMMENTSLEEP
							FLOGCOMMENT
							{
								char temp[100];
								sprintf ( temp, "NPC Disbanding unit - %s  unit:%ld",
									realm[rlm].mfGetName(), i);
								if ( fLogComment )
									RandomLogComment ( temp );
								else
									Sleep(0);
							}
							#endif

						}
					}

					// make sure we have some money
					if (new_treasury < 5)
						new_treasury = 5 + random(5);

					//printf("NPC ADMIN - %s is FLAT BROKE.\n",realm[rlm].mfGetName());
				}

				// buy treasury to a balanced level
				if (realm_reg > 50 && new_treasury < 50)
				{
					new_treasury += (realm_reg-50) / 5;
					realm_reg -= ((realm_reg-50) / 5) * 5;
				}

				// monsters and five peaks don't collect regency
				if (rlm == REALM::MONSTERS || rlm == REALM::FIVEPEAKS) realm_reg = 0;

				realm[rlm].mfSetRegency(realm_reg);
				realm[rlm].mfSetTreasury(new_treasury);
				realm[rlm].mfSetSurplusIncome( (surplus > 0) ? surplus : 0);

				//printf("NPC ADMIN - %s regency:%d  treasury:%d\n",realm[rlm].name,realm[rlm].regency,realm[rlm].treasury);

				#ifdef _WINDOWS
				LOGCOMMENTSLEEP
				FLOGCOMMENT
				{
					char temp[100];
					sprintf ( temp, "DoNPCAdmin %d - %s  regency:%04d  treasury:%06d",
							4,
							realm[rlm].mfGetName(),
							realm[rlm].mfGetRegency(),
							realm[rlm].mfGetTreasury());
					if ( fLogComment )
						RandomLogComment ( temp );
					else
						Sleep(0);
				}
				#endif

			}

			// roll for this turn's order
		 	if (!realm[rlm].mfIsDMCtrl())
			{
				while (RealmOrder[(roll=random(LAND_REALM_COUNT))] != REALM::REALM_OUT_OF_RANGE) {}
				RealmOrder[roll] = rlm;

				#ifdef _WINDOWS
				LOGCOMMENTSLEEP
				FLOGCOMMENT
				{
					char temp[100];
					sprintf ( temp, "DoNPCAdmin %d - %s is realm #%ld in the realm order\n",
						5,
						realm[rlm].mfGetName(),
						roll);
					if ( fLogComment )
						RandomLogComment ( temp );
					else
						Sleep(0);
				}
				#endif
			}

		}
	}	// for all realms

	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	FLOGCOMMENT
	{
		char temp[100];
		sprintf ( temp, "DoNPCAdmin %d - Order for new turn:  ", 6);
		if ( fLogComment )
			RandomLogComment ( temp );
		else
			Sleep(0);

		for (i=0; i<LAND_REALM_COUNT; ++i)
		{
			j = RealmOrder[i];
			if (j >= 0 && j < LAND_REALM_COUNT)
				sprintf ( temp, "             #%ld - %s",i,realm[RealmOrder[i]].mfGetName() );
			else
				sprintf ( temp, "             #%ld - %s",i,"unused" );
			if ( fLogComment )
				RandomLogComment ( temp );
			else
				Sleep(0);
		}
	}
	#endif

	iCurrentRealmIndex = 0;		// start new turn

	HomeRealm = (REALM::REALM_TYPE)hr;
	fFinalRoll = fr;
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     - 
   ======================================================================== */
LONG SpendNPCRegency (LONG realm_, LONG rp, BOOL fSend)
{
	LONG const oldregency = realm[realm_].mfGetRegency();
	LONG rv;

	if (OldNPCRegency == -1)			// save starting regency
	{
		OldNPCRegency = oldregency;
		OldNPCRealm = realm_;
	}

	rv = MIN(rp, oldregency);
	if (rv < 0)
	{
		rv = 0;
	}
	else
	{
		// see how much is actually successfully spent here
		LONG const newregency = realm[realm_].mfAddRegency(-rv);
		rv = oldregency - newregency;
	}

	SetGameData(MP_REALM, MPRLM_REGENCY, realm_, realm[realm_].mfGetRegency(), fSend);

	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	FLOGCOMMENT
	{
		char temp[100];
		sprintf ( temp, "   SpendNPCRegency - %d %s spent:%d now:%d", 
			7,
			realm[realm_].mfGetName(),
			rp,
			realm[realm_].mfGetRegency());
		if ( fLogComment )
			RandomLogComment ( temp );
		else
			Sleep(0);
	}
	#endif

	return rv;
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
static LONG	RegencePoint;
static LONG	opp_modRegency;
static LONG	StartSuccess;

void DoneNPCAction (LONG, LONG)
{
	fDoneOthersAction = FALSE;

	if (RegencePoint > realm[HomeRealm].mfGetRegency())
		realm[HomeRealm].mfSetRegency(0);
	else
		realm[HomeRealm].mfAddRegency(-RegencePoint);

	//SetGameData(MP_REALM, MPRLM_REGENCY, HomeRealm, realm[HomeRealm].mfGetRegency(), TRUE);

	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	FLOGCOMMENT
	{
		char temp[100];
		sprintf ( temp, "   DoneNPCAction - %d %s   Reg spent:%d   Reg now:%d", 
			8,
			realm[HomeRealm].mfGetName(),
			RegencePoint,
			realm[HomeRealm].mfGetRegency());
		if ( fLogComment )
			RandomLogComment ( temp );
		else
			Sleep(0);
	}
	#endif

	del_region(RollNPCAction, 0);
	del_region(DoneNPCAction, 0);

	//SetButtonProc (D_MOREINFO, 0, PaintMoreInfo, 0, 0);
	//SetButtonProc (D_MOREINFO, 4, HideSubMenu, D_MOREINFO, 0);
	HideRequest(D_MOREINFO);

	RunMenus();
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
static void RollNPCAction (LONG, LONG)
{
	if (RegencePoint+response[curResponse].modRegency > realm[HomeRealm].mfGetRegency())
	{
		AddSndObj((BIRTHRT_SND)SND_UI_NOT_ENOUGH_REGENCY,0,VOLUME_NINETY);
		return;
	}
	fDoneOthersAction = FALSE;
}

/* ========================================================================
   Function    - PaintNPCAction
   Description - 
   Returns     -
   ======================================================================== */
void PaintNPCAction (LONG MenuCombo, LONG)
{
	SHORT	i, j, m=-1;
	LONG	X,Y;
	LONG	W,H;
	LONG	MenuId, ButtonId;
	LONG	result = 0;
	LONG	startY;
	UBYTE	success;
	char FormatString[80];
	char actionBuffer[80];

	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	del_region(RollNPCAction, 0);
	del_region(DoneNPCAction, 0);
	del_region(IncNum,0);
	del_region(DecNum,0);
//	DeleteActnMenuRegions();
//	iThisRoll = -1;

	fAllowRollButton = TRUE;
	fAllowDoneButton = FALSE;

	// get position of text area
	if( GetButtonPosition( MenuId, 0, &X, &Y ) == fERROR)
		return;

	// get size of menu
	if( GetButtonSize( MenuId, 0, &W, &H ) == fERROR)
		return;

	startY = Y;

	init_gfont(FONT_SANS_8PT);

	// ---------------------------------
	// print body of menu here
	// ---------------------------------

	Y += 20;
	X += 20;
	switch ( response[curResponse].lWhichAction )
	{
		case ACTION_CONTEST_LAW:
			m = 0;
			break; 
		case ACTION_CONTEST_GLD:
			m = 1;
			break;
		case ACTION_CONTEST_TMP:
			m = 2;
			break;
		case ACTION_CONTEST_SRC:
			m = 3;
			break;
		default:
	 		break;
	}

	if (m >= 0 && (places[TargetPlace].Level == 0
			   		|| places[TargetPlace].Contested == TRUE) )
	{
		strcpy(FormatString, STRMGR_GetStr(STR_MAPAI_DESTROYING_HOLDING_FMT));
		print_textf(X+5,Y, DKBROWN, FormatString, realm[CurrentRealm].mfGetName(), STRMGR_GetStr(gsHoldType[m]), province[TargetProvince].name);
	}
	else
	{
		strcpy(FormatString,STRMGR_GetStr(STR_ACTNMENU_WHICH_ACTION_FMT));
		strcpy(actionBuffer, szAIAction[response[curResponse].lWhichAction].mfGetStr());
		print_textf(X+5,Y, DKBROWN, FormatString, realm[CurrentRealm].mfGetName(), actionBuffer, province[TargetProvince].name);
	}

	Y += 30;
	print_textf(X+5, Y, DKBROWN, STRMGR_GetStr(STR_ACTNMENU_F02_SUCCESS));
	print_textf(X+71, Y+6, DKBROWN, "^F02^c%d%c", D20TOPERCENT(response[curResponse].lSuccess+response[curResponse].modRegency),PERCENTCHAR);

	print_textf(X+110, Y, DKBROWN,   STRMGR_GetStr(STR_MAPAI_MODIFY_SUCCESS));
	print_textf(X+110, Y+8, DKBROWN, STRMGR_GetStr(STR_MAPAI_USING_REGENCY));
	SetupMod(X+200, Y, &response[curResponse].modRegency, 0, 20*((response[curResponse].lSuccess+response[curResponse].modRegency)<20), 1);

	//---- Only play the sounds once

	if (response[curResponse].lResult == 1)	// success
	{
		AddSndObj((BIRTHRT_SND)SND_UI_SUCCEEDED,0,VOLUME_NINETY);
		response[curResponse].lResult = 3;
	}
	if (response[curResponse].lResult == 2)	// failure
	{
 		AddSndObj((BIRTHRT_SND)SND_UI_DID_NOT_SUCCEED,0,VOLUME_NINETY);
		response[curResponse].lResult = 4;
	}

	//---- Now we can print the data 
	if ( response[curResponse].lResult == 3 )
	{
		fAllowRollButton = FALSE;
		fAllowDoneButton = TRUE;
		print_textf(X+130, Y + 20, RED, STRMGR_GetStr(STR_MAPAI_ACTION_SUCCEEDED));
	}

	if ( response[curResponse].lResult == 4 )
	{
		fAllowRollButton = FALSE;
		fAllowDoneButton = TRUE;
		print_textf(X+130, Y + 20, RED, STRMGR_GetStr(STR_MAPAI_ACTION_FAILED));
	}

	Y = startY+H-80;
	print_textf(X+5, Y, DKBROWN, STRMGR_GetStr(STR_MAPAI_TOTAL_COST_FMT), RegencePoint+response[curResponse].modRegency);

	Y = startY+H-65;
	print_textf(X+5, Y, DKBROWN,STRMGR_GetStr(STR_MAPAI_SPEND_REGENCY_HELP));

	// draw buttons
	init_gfont(FONT_TITL_10PT);
	Y = startY+H-32;

	if(fAllowRollButton)
		add_region(X+138, Y, 68, 28, 0, RollNPCAction, 0, 0, 0, -1);

	SHORT iButton = GetResourceStd ("UI\\REGBTN2A.PCX", FALSE);

	DrawBitmap (X+138-15, (SHORT)Y, iButton, 0, 0, 999, 999);
	print_textf(X+138+34-15, Y+14, (fAllowRollButton)?WHITE:GREY, "^F02^c%s", STRMGR_GetStr(STR_ROLL1));

	if(fAllowDoneButton)
		add_region(X+218, Y, 68, 28, 0, DoneNPCAction, 0, 0, 0, -1);

	DrawBitmap (X+218, (SHORT)Y, iButton, 0, 0, 999, 999);
	print_textf(X+218+34, Y+14, (fAllowDoneButton)?WHITE:GREY, "^F02^c%s", STRMGR_GetStr(STR_DONE1));

	if (iButton!=fERROR)
		SetPurge(iButton);
}


/* ========================================================================
   Function    - 
   Description - 
   Returns     - 
   ======================================================================== */
BOOL RollNPCSuccess (LONG action, BOOL * fSync, BOOL * fSend )
{
	LONG	success, additionalRP, addCur, addTar, tot_addCur, tot_addTar;
	LONG	rv = FALSE;

	*fSync = FALSE;
	*fSend = FALSE;

	success = StartSuccess = 16 - SpendNPCRegency(CurrentRealm, 15, FALSE);

	//printf("CurrentRealm:%s action:%s TargetProv:%s TargetPlace:%d success:%d\n",
	//realm[CurrentRealm].name, szAIAction[action],
	//province[TargetProvince].name, TargetPlace, success);

	// find the realm of the target
	if (TargetPlace)
	{
 		TargetRealm = (REALM::REALM_TYPE)places[TargetPlace].Realm;	// get the realm from the holding..
	}
	else if (TargetProvince)
	{
		TargetRealm = (REALM::REALM_TYPE)province[TargetProvince].Realm;	// ..or province
	}
	else
		return TRUE;	// neither TargetPlace or TargetProvince is set

	// we don't oppose ourselves
	if (TargetRealm == CurrentRealm)
		return TRUE;

	// --------------------------------------------------------
	// Player is the target of the action, allow opposition
	// --------------------------------------------------------
	if (TargetRealm == HomeRealm)
	{
		*fSend = TRUE;
		*fSync = TRUE;

		push_regions();
		response[curResponse].lWhichAction = action;
		response[curResponse].lSuccess = success;
		response[curResponse].modRegency	= 0;
		modRegency = 0;
		RegencePoint = 0;
		opp_modRegency = 0;
		response[curResponse].lResult = 0;

		// do the dialog box
		SetButtonProc (D_MOREINFO, 0, PaintNPCAction, 0, 0);
		SetButtonProc (D_MOREINFO, 4, NULL, 0, 0);
		ShowRequest(D_MOREINFO, curResponse);

DoItAgain:
		response[curResponse].modRegency = modRegency = 0;
		SysForceCursor();
		fDoneOthersAction = TRUE;
		while (fDoneOthersAction)
			MenuLoop();

		RegencePoint += response[curResponse].modRegency;
		response[curResponse].lSuccess = success = StartSuccess - opp_modRegency + RegencePoint;
		//printf("RollNPCSuccess - start suc:%d  our RP:%d  Opp RP:%d  success:%d\n",StartSuccess,RegencePoint,opp_modRegency,success);

		if (response[curResponse].modRegency)
		{
			additionalRP = 0;
			switch (action)
			{
				case ACTION_AGITATE_FOE:
				case ACTION_AGITATE_OWN:
				case ACTION_RULE_LAW:
				case ACTION_RULE_GLD:
				case ACTION_RULE_TMP:
				case ACTION_RULE_SRC:
					if (random(100)<30 && success>10)
						additionalRP = SpendNPCRegency(CurrentRealm, random(10), TRUE);
					break;

				case ACTION_CONTEST_LAW:
				case ACTION_CONTEST_GLD:
				case ACTION_CONTEST_TMP:
				case ACTION_CONTEST_SRC:
				case ACTION_CREATE_LAW:
				case ACTION_CREATE_GLD:
				case ACTION_CREATE_TMP:
				case ACTION_CREATE_SRC:
				case ACTION_LEYLINE:
				case ACTION_RULE_PROV:
				case ACTION_TRADEROUTE:
					if (random(100)<60 && success>10)
						additionalRP = SpendNPCRegency(CurrentRealm, random(success-1), TRUE);
					break;

				case ACTION_INVESTITURE:
					if (success>5
						&&(	  (game_difficulty==DIF_EASY && random(100) < 65)
							||(game_difficulty==DIF_MEDIUM && random(100) < 75)
						    ||(game_difficulty==DIF_HARD && random(100) < 85)))
						additionalRP = SpendNPCRegency(CurrentRealm, success-1, TRUE);
					break;
				
				case ACTION_ESPIONAGE_ASSASSIN:
					success = UnitMusterCost[units[TargetUnit].Icon]+1;
			}

			if (additionalRP)
			{
				//AddSndObj((BIRTHRT_SND)SND_UI_SUCCESS_MODIFIED_BY_OPPONENT,0,VOLUME_NINETY);
				AddSndObj((BIRTHRT_SND)(32362),0,VOLUME_NINETY);
				opp_modRegency += additionalRP;
				response[curResponse].lSuccess = success = StartSuccess - opp_modRegency + RegencePoint;
				response[curResponse].lResult = 0;
				goto DoItAgain;
			}
		}

		response[curResponse].modRegency = modRegency = 0;

		if (random(20)+1 >= success)	// roll 1d20, compare with target #
		{
			rv = TRUE;
			response[curResponse].lResult = 1;
		}
		else
			response[curResponse].lResult = 2;

		SysForceCursor();
		fDoneOthersAction = TRUE;
		while (fDoneOthersAction)
			MenuLoop();

		#ifdef _WINDOWS
		if (IsMultiPlayer())
		{
			// can't xmit data until all have reached this point, put up wait box
			StartSync( iMPS_SYNC8 );
			CheckSync( iMPS_SYNC8, TRUE, TRUE );
		}
		#endif

		SetGameData(MP_REALM, MPRLM_REGENCY, HomeRealm, realm[HomeRealm].mfGetRegency(), TRUE);
		SetGameData(MP_REALM, MPRLM_REGENCY, CurrentRealm, realm[CurrentRealm].mfGetRegency(), TRUE);

		pop_regions();
		clear_key_status(0);
	}

	//------------------------------------------------
	// Some player is the target of the action
	// but on another, remote, machine.
	//------------------------------------------------
	#ifdef _WINDOWS
	else if (IsMultiPlayer() && realm[TargetRealm].mfIsPlayerCtrl())
	{
		// can't go on until all have reached this point, put up wait box
		StartSync( iMPS_SYNC8 );
		CheckSync( iMPS_SYNC8, TRUE, TRUE );

		*fSync = TRUE;
		return FALSE;
	}
	#endif

	//------------------------------------------------
	// spend some more of both players regency here
	//------------------------------------------------
	else
	{
		tot_addCur = tot_addTar = 0;
		addCur = addTar = 1;
		success = StartSuccess;
		do
		{
			addTar = 0;
			if (addCur && random(100)<85 && success<14)
				addTar = SpendNPCRegency(TargetRealm, random(20-success), FALSE);
			tot_addTar += addTar;
			success = StartSuccess - tot_addCur + tot_addTar;

			addCur = 0;
			if (addTar && random(100)<75 && success>7)
				addCur = SpendNPCRegency(CurrentRealm, random(success-1), FALSE);
			tot_addCur += addCur;
			success = StartSuccess - tot_addCur + tot_addTar;
		}
		while (addCur || addTar);

		if (random(20) >= success)
			rv = TRUE;

		//printf("CurrentRealm:%s action:%s TargetRealm:%s TargetProv:%s TargetPlace:%d\n",
		//	realm[CurrentRealm].name, szAIAction[action],
		//	realm[TargetRealm].name, province[TargetProvince].name, TargetPlace);
		//printf("RollNPCSuccess - start suc:%d  TarRP:%d  CurRP:%d  success:%d\n",StartSuccess,tot_addTar,tot_addCur,success);
	}

	return rv;
}

/* ========================================================================
   Function    - IsBorder
   Description - 
   Returns     - returns TRUE if prov borders realm
   ======================================================================== */
#define ADJACENT			1
#define MY_ADJ_ALLYS		2

BOOL IsBorder (PROVINCE prov, REALM::REALM_TYPE realm, SHORT action, LONG purpose)
{
	LONG	i, j, testprov_border, testprov_borderrealm;

	for (i=0; i<7; ++i)
	{
		testprov_border = province[prov].Border[i].Province;
		testprov_borderrealm = province[testprov_border].Realm;

		if ( realm != REALM::ANY_REALM
			&&	(testprov_borderrealm == realm
					|| (AREALLIED(testprov_borderrealm,realm) && purpose==MY_ADJ_ALLYS) )
			&& (action == ACTION_DIPLMCY_DMD_PROV
					|| TerrainCost[province[prov].Border[i].MoveCost] < 100 )
			)
		{
			return TRUE;
		}

		if ( realm == REALM::ANY_REALM
				&& action == ACTION_MOVE_BORDER_OWN
				&& testprov_borderrealm != CurrentRealm
				&& !AREALLIED(testprov_borderrealm, CurrentRealm) )
			return TRUE;

		if (realm == REALM::ANY_REALM && testprov_borderrealm != CurrentRealm)
			return TRUE;
	}
	return FALSE;
}

/* ========================================================================
   Function    - UnitsHere
   Description - set count of groups (cGroups) and strTargetProvince
   Returns     - number of units of realm in prov
   ======================================================================== */
LONG UnitsHere (PROVINCE prov, REALM::REALM_TYPE _realm)
{
	LONG	i, iUnit;
	LONG	cUnits = 0;

	cGroups = 0;
	strTargetProvince = 0;

	for (i=1; i<MAX_UNITS; ++i)
	{
		// scan through units for leaders
		if (units[i].Realm != REALM::NO_COUNTRY
			&& units[i].Joined==0 
			&& (units[i].Realm==_realm || _realm == REALM::ANY_REALM)
			&& UnitFinalProvince(i)==prov)
		{
			++cGroups;
			++cUnits;
			iUnit = i;

			if (units[iUnit].Icon <= C_WYV_UNIT_ICON)
			{
				strTargetProvince += UnitMusterCost[units[iUnit].Icon];
			}

			while (iUnit != -1)		// list followers
			{
				++cUnits;

				// GWP MORE HACKING HERE TOO, see note above.
				if (units[iUnit].Icon <= C_WYV_UNIT_ICON)
				{
					strTargetProvince += UnitMusterCost[units[iUnit].Icon];
				}
				iUnit = units[iUnit].NextUnit;
			}
		}
	}

	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	FLOGCOMMENT
	{
		char temp[100];
		sprintf ( temp, "   UnitsHere - prov:%s  realm:%s  cUnits:%ld  cGroups:%ld  str:%ld",
			province[prov].name,
			realm[_realm].mfGetName(),
			cUnits, cGroups, strTargetProvince);
		if ( fLogComment )
			RandomLogComment ( temp );
		else
			Sleep(0);
	}
	#endif

	return cUnits;
}

/* ========================================================================
   Function    - UnitsHereNotUs
   Description - Are there any units in this province not allied with us.
   Returns     - 
   ======================================================================== */
static BOOL UnitsHereNotUs (REALM::REALM_TYPE OurRealm, PROVINCE prov)
{
	LONG	i, iUnit;
	LONG	cUnits = 0;

	for (i=1; i<MAX_UNITS; ++i)
	{
		// scan through units for leaders
		if (units[i].Realm != REALM::NO_COUNTRY
				&& units[i].Joined==0 
				&& !AREALLIED(units[i].Realm, OurRealm)
				&& UnitFinalProvince(i)==prov )
			return TRUE;
	}

	return FALSE;
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
void Clear_fProv (void)
{
	LONG i;
	for (i=0; i<PROVINCE_COUNT; ++i)
		fProv[i] = FALSE;
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
LONG ProvDist (LONG cur_prov, LONG dest_prov, LONG cProvDist, BOOL fTerrainDontCare)
{
	LONG	i, dx, dy, dist;
	LONG	test_prov;
	LONG	best_prov = 0;
	LONG	best_border = 0;
	LONG	min_dist = 99999999;

//printf("?");

	if (cProvDist==0)
		Clear_fProv();

	fProv[cur_prov] = TRUE;

	for (i=0; i<7; ++i)
	{
		test_prov = province[cur_prov].Border[i].Province;
		dx = province[dest_prov].x - province[test_prov].x;
		dy = province[dest_prov].y - province[test_prov].y;
		dist = (dx * dx) + (dy * dy);
		if (!fProv[test_prov] && dist < min_dist && (fTerrainDontCare || TerrainCost[province[cur_prov].Border[i].MoveCost] < 100))
		{
			min_dist = dist;
			best_prov = test_prov;
			best_border = i;
		}
	}

	// found a good province
	if (best_prov && best_prov != dest_prov)
		cProvDist = ProvDist(best_prov, dest_prov, cProvDist+1, fTerrainDontCare);

	return cProvDist;
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
static BOOL FindProvWithSrcTmp( REALM::REALM_TYPE ourRealm, PROVINCE prov, BOOL flag, LONG rqLevel)
{
	// if flag is true, find province with source, otherwise, find one with temple
	LONG t, c;

	CheckAndFixPlaces(prov);
	t = c = province[prov].FirstPlace;
	while (c != 0 && t>=0 && t<MAX_PLACES)
	{
		if (((flag && places[t].Icon == SOURCE1_ICON&& places[t].Level >= rqLevel)
		   ||(!flag && places[t].Icon == TEMPLE1_ICON && places[t].Level >= rqLevel)) 
		  && places[t].Realm == ourRealm)
			break;
		c = places[t].NextPlace;
		t = province[prov].FirstPlace + c;
	}
	if (c!=0)
	 	return TRUE;
	else 
		return FALSE;
}
/* ========================================================================
   Function    - 
   Description -  sub function to support FindProv for NPC Diplomacy
   Returns     -
   ======================================================================== */
LONG FindProvForDIP (PROVINCE prov, BOOL realm_flag, LONG base)
{
   LONG c, j;
   LONG t = 0;

   if ((realm_flag && province[prov].Realm != CurrentRealm)
   	||(!realm_flag&&province[prov].Realm == CurrentRealm))
   {
		t = base;
		// there is a forein unit (if realm_flag == TRUE)
		// or there is a our unit (if realm_flag == FALSE)
	   for (c = 1; c < MAX_UNITS; ++c)
		{
	   	if (units[c].Realm != REALM::NO_COUNTRY
				&& ((!realm_flag && units[c].Realm == CurrentRealm)
					|| (realm_flag && units[c].Realm != CurrentRealm))
				&& UnitFinalProvince(c) == prov && units[c].Joined == 0)
	   	{
	   		t -= 20;
	   		break;
	   	}
		}

	  	// find a foreign castle or our castle if realm_flag == FALSE
		CheckAndFixPlaces(prov);
	  	j = c = province[prov].FirstPlace;
	  	while (c!=0)
	  	{
	  		if ((	(realm_flag && places[j].Realm != CurrentRealm)
	  				||(!realm_flag && places[j].Realm == CurrentRealm))
				 && places[j].Icon == CASTLE1_ICON)
	  		{
	  			t -= 10;
	  			break;
	  		}
	  		c = places[j].NextPlace;
	  		j = province[prov].FirstPlace+c;
	  	}

   	if ( province[prov].Terrain==FOREST
			|| province[prov].Terrain == SWAMP
			|| province[prov].Terrain == MOUNTN
			|| province[prov].Terrain == HILLS )
		{
   		if (realm_flag)
				t += 20;
			else
				t -= 20;
		}
   
   }

   return t;

}

/* ========================================================================
   Function    - 
   Description -  sub function to support FindProv for NPC Diplomacy
   Returns     -
   ======================================================================== */
BOOL OfferProvince()
{
	LONG i,t;
	LONG maxReaction=0;
	LONG pr=0;
	LONG base=100;
	for(i=1; i<PROVINCE_COUNT; ++i)
	{
		t = FindProvForDIP((PROVINCE)i, FALSE, base);
		if(t==0)
			continue;
		t += random(10);
		if(t > maxReaction)
		{
			maxReaction = t;
			pr = i;
		}
	}
	if (pr)			// if a province was selected
	{
		StartProvince = (PROVINCE)pr;
		return TRUE;
	}
	return FALSE;

}
/* ========================================================================
   Function    - 
   Description -  sub function to support FindProv for NPC Diplomacy
   Returns     -
   ======================================================================== */
LONG FindProvForDemand (PROVINCE prov, LONG base)
{
   LONG c, i, j;
   LONG t = 0;

	// don't bother asking for a couple places
	if (province[prov].Realm == CurrentRealm			// places we own
		|| prov == NO_PROVINCE
		|| prov == ANY_PROVINCE
		|| prov == SPIDERFELL
		|| prov == RHUOBHE
		|| prov == CITY_OF_ANUIRE 
		|| prov == LOST_REALM
		|| prov == NO_PROVINCE2 )
	{
		return 0;
	}

	// province must border me
	for (i=0; i<7; ++i)
	{
		c = province[prov].Border[i].Province;
		if ( province[c].Realm == CurrentRealm )
		{
			t = base;
			break;
		}
	}
	if (t == 0) return 0;

	// foreign units make it harder, our units make it easier
   for (c = 1; c < MAX_UNITS; ++c)
	{
   	if (units[c].Realm != REALM::NO_COUNTRY
			&& UnitFinalProvince(c) == prov && units[c].Joined == 0)
   	{
   		t += (units[c].Realm==CurrentRealm) ? 20 : -10;
   		break;
   	}
	}

  	// castle makes it harder
	CheckAndFixPlaces(prov);
  	j = c = province[prov].FirstPlace;
  	while (c!=0)
  	{
  		if (places[j].Icon == CASTLE1_ICON)
  		{
  			t -= 10;
  			break;
  		}
  		c = places[j].NextPlace;
  		j = province[prov].FirstPlace+c;
  	}

	if ( province[prov].Terrain==FOREST
		|| province[prov].Terrain == SWAMP
		|| province[prov].Terrain == MOUNTN
		|| province[prov].Terrain == HILLS )
	{
		t += 20;
	}

   return t;
}

/* ========================================================================
   Function    - 
   Description -  sub function to support FindProv for NPC Diplomacy
   Returns     -
   ======================================================================== */
PROVINCE DemandProvince()
{
	LONG i, t;
	LONG maxReaction=0;
	LONG pr=0;
	LONG base=100;
	
	for (i=1; i<PROVINCE_COUNT; ++i)
	{
		t = FindProvForDemand((PROVINCE)i, base);
		if (t) t += random(30);
		if (t > maxReaction)
		{
			maxReaction = t;
			pr = i;
		}
	}
	
	return (PROVINCE)pr;
}

/* ========================================================================
   Function    - FindProvToFortify
   Description - Fortify borders with enemies first, basic borders next.
   Returns     - 
   ======================================================================== */
static BOOL FindProvToFortify(REALM::REALM_TYPE ourRealm, PROVINCE *fortifyProv)
{
	BOOL Result = FALSE;
	LONG i;
	PROVINCE provOnBorderOfEnemy = NO_PROVINCE;
	PROVINCE provOnBorder = NO_PROVINCE;
	
	for (i = 0; i < PROVINCE_COUNT; ++i)
	{
		if (province[i].Realm == ourRealm
			&& !DoIHavePlace((PROVINCE) i, ourRealm, CASTLE1_ICON)
			&& !ForeignUnitsHere((PROVINCE)i, ourRealm)
			)
		{
			LONG j;
			for (j = 0; j < 7; ++j)
			{
				PROVINCE BorderProv = (PROVINCE) province[i].Border[j].Province;
				if (BorderProv != NO_PROVINCE 
					&& province[BorderProv].Realm != ourRealm)
				{
					if (ISATWAR(province[BorderProv].Realm, ourRealm))
					{
						provOnBorderOfEnemy = (PROVINCE)i;
						goto StopLooking;
					}
					else
					{
						provOnBorder = (PROVINCE)i;
					}
				}
			}
		}
	}
	
StopLooking:
	if (provOnBorderOfEnemy != NO_PROVINCE)
	{
		*fortifyProv = provOnBorderOfEnemy;
		Result = TRUE;
	}
	else if (provOnBorder != NO_PROVINCE)
	{
		*fortifyProv = provOnBorder;
		Result = TRUE;
	}
	
	return Result;
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
BOOL FindProv (SHORT action)
{
	LONG	i, t, c, s;
	LONG	maxReaction=0;
	LONG	j = 999;
	LONG	f = FALSE;
	LONG	pr = 0;

	for (i=1; i<PROVINCE_COUNT; ++i)
	{
		switch (action)
		{
			case ACTION_DIPLMCY_DMD_PROV:
				pr = DemandProvince();
				f = FALSE;
				i = PROVINCE_COUNT;	// done with for loop
				break;
			case ACTION_AGITATE_FOE:
				if( timeCount == 0 )
					f = (FindHolding(TargetRealm, CurrentRealm, (PROVINCE)i, ANY_MAP_ICON, TRUE) 
									&& province[i].Loyalty == 1);
				else
					f = (FindHolding(TargetRealm, CurrentRealm, (PROVINCE)i, ANY_MAP_ICON, TRUE)
									&& province[i].Loyalty == 2); 
				break;
			case ACTION_AGITATE_OWN:
				if( timeCount == 0 )
					f = (province[i].Realm == CurrentRealm && province[i].Loyalty == 0);
				else 
					f = (province[i].Realm == CurrentRealm && province[i].Loyalty == 1);
				
				break;

			//-----------------------------------------------
			// unit is chosen first. see UnitProvince
			//-----------------------------------------------
			case ACTION_MOVE_BORDER_FOE:
				if ((province[i].Realm == TargetRealm || TargetRealm == REALM::ANY_REALM)
						&& IsBorder((PROVINCE)i,CurrentRealm, action, MY_ADJ_ALLYS)
						&& i != UnitProvince
						&& (!province[i].Warded || (province[i].Warded && province[i].DispelLvl == CurrentRealm))
					)
				{
					c = UnitsHere((PROVINCE)i,CurrentRealm);	// find min units
					if (c < j)	// find min units
					{
						j = c;
						pr = i;
					}
				}
				break;

			case ACTION_MOVE_BORDER_OWN:
			case ACTION_MOVE_RETREAT:
				if ((province[i].Realm == CurrentRealm || AREALLIED(province[i].Realm,CurrentRealm))
						&& IsBorder((PROVINCE)i,TargetRealm, action, ADJACENT)
						&& i != UnitProvince
						&& (!province[i].Warded || (province[i].Warded && province[i].DispelLvl == CurrentRealm))
					)
				{
					c = UnitsHere((PROVINCE)i,CurrentRealm);	// find min units
					if (c < j)	// find min units
					{
						j = c;
						pr = i;
					}
				}
				break;

			case ACTION_MOVE_FILL_FOE:
			case ACTION_MOVE_FILL_OWN:
			case ACTION_MOVE_UNOCCUP_FOE:
			case ACTION_MOVE_UNOCCUP_OWN:
				if ( (province[i].Realm == TargetRealm || TargetRealm == REALM::ANY_REALM)
						&& i != UnitProvince 
						&& (!province[i].Warded || (province[i].Warded && province[i].DispelLvl == CurrentRealm))
					)
				{
					f = (UnitsHere((PROVINCE)i,CurrentRealm) == 0);		// find empty provs
				}
				break;

			case ACTION_MOVE_REINFORCE_FOE:
			case ACTION_MOVE_REINFORCE_OWN:
				if ( (province[i].Realm == TargetRealm || TargetRealm == REALM::ANY_REALM)
						&& i != UnitProvince
						&& (!province[i].Warded || (province[i].Warded && province[i].DispelLvl == CurrentRealm))
					)
				{
					if ((c = UnitsHere((PROVINCE)i,CurrentRealm)) < j)	// find min units
						if (c > 0)										// but not zero
						{
							j = c;
							pr = i;
						}
				}
				break;

			//-----------------------------------------------
			// province is chosen first
			//-----------------------------------------------
			case ACTION_MOVE_ATTACK_FOE:
				if ((province[i].Realm == TargetRealm || TargetRealm == REALM::ANY_REALM)
					&& (!province[i].Warded || (province[i].Warded && province[i].DispelLvl == CurrentRealm))
					)
				{
					f = UnitsHereNotUs(CurrentRealm, (PROVINCE)i);		// find provs with enemy
				}
				break;
			case ACTION_MOVE_ATTACK_OWN:
				if (province[i].Realm == CurrentRealm
					&& (!province[i].Warded || (province[i].Warded && province[i].DispelLvl == CurrentRealm))
					)
				{
					f = UnitsHereNotUs(CurrentRealm, (PROVINCE)i);		// find provs with enemy
				}
				break;

	 		case ACTION_FORTIFY:
				f = (province[i].Realm == CurrentRealm
						&& !DoIHavePlace((PROVINCE) i, CurrentRealm, CASTLE1_ICON)
					 	&& province[i].Realm == CurrentRealm
						&& IsBorder((PROVINCE)i, REALM::ANY_REALM, action, ADJACENT)
						&& !random(3));
				break;

			case ACTION_MUSTER_INF:
			case ACTION_MUSTER_EIN:
			case ACTION_MUSTER_BOW:
			case ACTION_MUSTER_IRR:
			case ACTION_MUSTER_PIK:
			case ACTION_MUSTER_LVY:
			case ACTION_MUSTER_SCT:
			case ACTION_MUSTER_KNT:
			case ACTION_MUSTER_CAV:
			case ACTION_MUSTER_MINF:
			case ACTION_MUSTER_MBOW:
			case ACTION_MUSTER_MIRR:
			case ACTION_MUSTER_MPIK:
			case ACTION_MUSTER_MCAV:
			case ACTION_MUSTER_EINF:
			case ACTION_MUSTER_EBOW:
			case ACTION_MUSTER_ECAV:
			case ACTION_MUSTER_DINF:
			case ACTION_MUSTER_DBOW:
			case ACTION_MUSTER_GINF:
			case ACTION_MUSTER_GBOW:
			case ACTION_MUSTER_GCAV:
			case ACTION_MUSTER_NINF:
			case ACTION_MUSTER_NIRR:
			case ACTION_MUSTER_CSKL:
				t = action - ACTION_MUSTER_INF + 1;
#if MUSTERINFO
if (province[i].LevyMustered >= province[i].CivLevel)
	printf("MUSTER FAILED - CivLevel limit in %s\n",province[i].name);
#endif

				#ifdef _WINDOWS
				LOGCOMMENTSLEEP
				FLOGCOMMENT
				{
					if (province[i].Realm == CurrentRealm)
					{
						char temp[100];
						sprintf ( temp, "   FindProv, Muster - prov:%ld  %d<%d  %d  %d>=%d",
							i, 
							province[i].LevyMustered, province[i].CivLevel,
							UnitsHere((PROVINCE)i,CurrentRealm),
							province[i].CivLevel, LevelUnitReqt[t] );
						if ( fLogComment )
							RandomLogComment ( temp );
						else
							Sleep(0);
					}
				}
				#endif

				if (province[i].Realm == CurrentRealm
					&& province[i].LevyMustered < province[i].CivLevel
					&& (c = UnitsHere((PROVINCE)i,CurrentRealm)) < j		// find min units
					&& province[i].CivLevel >= LevelUnitReqt[t]
//					&& (	(province[i].HumanPop	&& t>=A_INF_UNIT_ICON && t<=A_CAV_UNIT_ICON)
//						||	(province[i].ElfPop		&& t>=E_INF_UNIT_ICON && t<=E_CAV_UNIT_ICON)
//						||	(province[i].DwarfPop	&& t>=D_INF_UNIT_ICON && t<=D_BOW_UNIT_ICON)
//						||	(province[i].GoblinPop	&& t>=G_INF_UNIT_ICON && t<=G_CAV_UNIT_ICON)
//						||	(t>=M_INF_UNIT_ICON && t<=M_CAV_UNIT_ICON) )
					)
				{
					j = c;
					pr = i;
				}
				break;

			case ACTION_RSPELL_BLESSLAND:
				f = (province[i].Realm == CurrentRealm
						&& !province[i].Blessed
						&& !province[i].Dispel
						&& random(3)==0 
						&& FindProvWithSrcTmp(CurrentRealm, (PROVINCE)i, 0, 1));

				break;
			case ACTION_RSPELL_BLIGHT:
				f = ((province[i].Realm == TargetRealm || TargetRealm == REALM::ANY_REALM) 
					 	&& !province[i].Blighted 
						&& !province[i].Dispel
						&& random(2)==0 
						&& FindProvWithSrcTmp(CurrentRealm, (PROVINCE)i, 0, 3));

			case ACTION_RSPELL_DEMAGOGUE:
				f = ((province[i].Realm == TargetRealm || TargetRealm == REALM::ANY_REALM) 
					    && province[i].Loyalty 
						&& !province[i].Dispel
						&& random(2)==0 
						&& FindProvWithSrcTmp(CurrentRealm, (PROVINCE)i, 1, 3));
						
				break;
			case ACTION_RSPELL_WARDING:
				f = ((!province[i].Warded || !AREALLIED((REALM::REALM_TYPE) province[i].DispelLvl, CurrentRealm))
						&& (province[i].Realm == TargetRealm || TargetRealm == REALM::ANY_REALM)
						&& !province[i].Dispel
						&& random(4)==0 
						&& FindProvWithSrcTmp(CurrentRealm, (PROVINCE)i, 1, 5));
					
				break;

			case ACTION_RSPELL_RAZE:
				f = (((!province[i].Warded || province[i].Warded && province[i].DispelLvl == CurrentRealm))
						&& !province[i].Dispel
						&& FindProvWithSrcTmp(CurrentRealm, (PROVINCE)i, 1, 5)
						&& FindHolding(REALM::ANY_REALM,TargetRealm,(PROVINCE)i,CASTLE1_ICON, TRUE));
		
				break;
			case ACTION_RSPELL_STRONGHOLD:
				if (!province[i].Dispel
						&& random(2) == 0
						&& FindProvWithSrcTmp(CurrentRealm, (PROVINCE)i, 1, 7))
				{
					CheckAndFixPlaces((PROVINCE)i);
					t = c = province[i].FirstPlace;
					while (c != 0 && t>=0 && t<MAX_PLACES)
					{
						if (places[t].Icon == WIZTOWER_ICON && places[t].Realm == CurrentRealm)
							break;				 // if there is a wiztower, go to others
						c = places[t].NextPlace;
						t = province[i].FirstPlace + c;
					}
					if ( !c )
						f = TRUE;
				}
				break;
			
			case ACTION_RSPELL_SUMMONING:
			case ACTION_RSPELL_LEGIONOFTHEDEAD:
				f = (!province[i].Dispel
						&& random(2)==0
						&& FindProvWithSrcTmp(CurrentRealm, (PROVINCE)i, 1, 3));
					
				break;

			case ACTION_RSPELL_BLESSARMY:
				f = (!province[i].Dispel
						&& random(2)==0 
						&& FindProvWithSrcTmp(CurrentRealm, (PROVINCE)i, 0, 3));
				break;
					
			case ACTION_RSPELL_DEATHPLAGUE:
				f = (!province[i].Dispel
						&& random(2)==0 
						&& province[i].CivLevel
						&& (province[i].Realm == TargetRealm || TargetRealm == REALM::ANY_REALM)
						&& FindProvWithSrcTmp(CurrentRealm, (PROVINCE)i, 1, 5));
					
				break;
		

			case ACTION_RULE_PROV:
				if (province[i].Realm == CurrentRealm
					&& (c = province[i].CivLevel) < j
					&&  c < 10)	// find min civ level
				{
					j = c;
					pr = i;
				}
				break;

		}

		if (f)		// if current province is selected
		{
			TargetProvince = (PROVINCE)i;
//printf("TPrv:%s ",province[TargetProvince].name);
			return TRUE;
		}
	}

	if (pr)			// if a province was selected
	{
		TargetProvince = (PROVINCE)pr;
//printf("TPrv:%s ",province[TargetProvince].name);
		return TRUE;
	}

	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	FLOGCOMMENT
	{
		char temp[100];
		sprintf ( temp, "   FindProvince %d - Failed", 9 );
		if ( fLogComment )
			RandomLogComment ( temp );
		else
			Sleep(0);
	}
	#endif

	return FALSE;
}

/* ========================================================================
   Function    - CountUnitsInProv
   Description - Count the number of units in a provice of this realm.
   Returns     - 
   ======================================================================== */
static LONG CountUnitsInProv(PROVINCE thisProv, REALM::REALM_TYPE ourRealm)
{
	LONG Result = 0;
	LONG i;
	
	for (i = 1; i < MAX_UNITS; ++i)
	{
		if (units[i].Realm == ourRealm 
			&& units[i].Joined == 0
			&& thisProv == UnitFinalProvince(i)
		   )
		{
			Result += CountUnitStack(i);
		}
	}
	
	return Result;
}

/* ========================================================================
   Function    - FindProvinceToInvest
   Description - Investature with some minor smarts.
   Returns     - 
   ======================================================================== */

static BOOL FindProvinceToInvest(REALM::REALM_TYPE ourRealm, PROVINCE *investProv)
{
	BOOL Result = FALSE;
	LONG i;
	PROVINCE anyProv = NO_PROVINCE;
	PROVINCE anyProvOfOurEnemy = NO_PROVINCE;
	PROVINCE BorderProvOfOurEnemy = NO_PROVINCE;
	PROVINCE BorderProv = NO_PROVINCE;
	
	for (i=1; i<PROVINCE_COUNT; ++i)
	{
		if (province[i].OccupRealm == ourRealm)		// we occupy the prov
		{
			// GWP The ISATWAR Test might not be the right one, we may just
			//     want to check the not liking state. Ask Alan.
			if (ISATWAR(province[i].Realm, ourRealm))
			{
				LONG j;
				for (j = 0; j < 7; ++j)
				{
					PROVINCE BorderProv = (PROVINCE)province[i].Border[j].Province;
					if (BorderProv != NO_PROVINCE
					    && province[BorderProv].Realm == ourRealm)
					{
						BorderProvOfOurEnemy = (PROVINCE)i;
						goto StopLooking;
					}
				}
				anyProvOfOurEnemy = (PROVINCE)i;
			}
			else
			{
				LONG j;
				for (j = 0; j < 7; ++j)
				{
					PROVINCE BorderProv = (PROVINCE)province[i].Border[j].Province;
					if (BorderProv != NO_PROVINCE
					    && province[BorderProv].Realm == ourRealm)
					{
						BorderProv = (PROVINCE) i;
					}
				}
				if (BorderProv == NO_PROVINCE)
				{
					anyProv = (PROVINCE)i;
				}
			}
		}
	}
	
StopLooking:
	if (BorderProvOfOurEnemy != NO_PROVINCE)
	{
		*investProv = BorderProvOfOurEnemy;
		Result = TRUE;
	}
	else if (anyProvOfOurEnemy != NO_PROVINCE)
	{
		*investProv = anyProvOfOurEnemy;
		Result = TRUE;
	}
	else if (anyProv != NO_PROVINCE)
	{
		*investProv = anyProv;
		Result = TRUE;
	}
	
	return Result;
}
/* ========================================================================
   Function    - FindProvToReinforce
   Description - Find a province with a weak stack to send in the reinforcements
   Returns     - Province to go to.
   ======================================================================== */
static BOOL FindProvToReinforce(REALM::REALM_TYPE ourRealm, LONG ExcludeThisUnit, PROVINCE *PlaceToGo)
{
	BOOL Result = FALSE;
	LONG i;
	PROVINCE unitInOurRealmProv = NO_PROVINCE;
	LONG unitInOurRealmCount = 99;
	PROVINCE unitInAllyProv = NO_PROVINCE;
	LONG unitInAllyCount = 99;
	PROVINCE unitInFoeProv = NO_PROVINCE;
	LONG unitInFoeCount = 99;
	PROVINCE unitDefendingProv = NO_PROVINCE;
	LONG unitDefendingCount = 99;
	
	for (i=1; i<MAX_UNITS; ++i)
	{
		// scan through units for leaders
		if (units[i].Joined == 0 
		    && i != ExcludeThisUnit)
		{
			if (units[i].Realm == ourRealm )
			{
				LONG const Count = CountUnitStack(i);
			
				PROVINCE const UnitProv = UnitFinalProvince(i);
				REALM::REALM_TYPE const ProvRealm = (REALM::REALM_TYPE) province[UnitProv].Realm;
				
				if (ISATWAR(ProvRealm, ourRealm))
				{
					LONG const FoeCount = CountUnitsInProv(UnitProv, ProvRealm);
					LONG const UsCount = CountUnitsInProv(UnitProv, ourRealm);
					
					if (FoeCount >= UsCount)
					{
						// We are under attack and out numbered, in enemy territory
						unitInFoeProv = UnitProv;
						unitInFoeCount = 0;
					}
					else if (unitInFoeCount > Count)
					{
						unitInFoeCount = Count;
						unitInFoeProv = UnitProv;
					}
				}
				else if (ProvRealm == ourRealm && Count < gUnitStackSize)
				{
					if (unitInOurRealmCount > Count)
					{
						unitInOurRealmCount = Count;
						unitInOurRealmProv = UnitProv;
					}
				}
				else if (AREALLIED(ProvRealm, ourRealm) && Count < gUnitStackSize)
				{
					if (unitInAllyCount > Count)
					{
						unitInAllyCount = Count;
						unitInAllyProv = UnitProv;
					}
				}
			}
			else
			{
				PROVINCE const UnitProv = UnitFinalProvince(i);
				REALM::REALM_TYPE const ProvRealm = (REALM::REALM_TYPE) province[UnitProv].Realm;
				
				if (ISATWAR(units[i].Realm, ourRealm)
				    && ProvRealm == ourRealm)
				{
					// Enemies are moving into our realm.
					LONG const FoeCount = CountUnitStack(i);
					
					// Count the defenders.
					LONG const UsCount = CountUnitsInProv(UnitProv, ourRealm);
					
					if (FoeCount > UsCount
					    && unitDefendingCount > UsCount)
					{
						unitDefendingProv = UnitProv;
						unitDefendingCount = UsCount;
					}
				}
			}
		}
	}
	
	if (unitDefendingProv != NO_PROVINCE)
	{
		*PlaceToGo = unitDefendingProv;
	}
	else if (unitInFoeProv != NO_PROVINCE)
	{
		*PlaceToGo = unitInFoeProv;
		Result = TRUE;
	}
	else if (unitInOurRealmProv != NO_PROVINCE)
	{
		*PlaceToGo = unitInOurRealmProv;
		Result = TRUE;
	}
	else if (unitInAllyProv != NO_PROVINCE)
	{
		*PlaceToGo = unitInAllyProv;
		Result = TRUE;
	}
	return Result;
}

/* ========================================================================
   Function    - FindUnitToAssinate
   Description - Search for a "best" choice to shoot.
   Returns     - 
   ======================================================================== */

static BOOL FindUnitToAssinate(REALM::REALM_TYPE ourRealm, REALM::REALM_TYPE theirRealm)
{
	BOOL Result = FALSE;
	LONG i;
	LONG unitInOurRealm = 0;
	LONG unitInAlly = 0;
	LONG unitOnBorder = 0;
	LONG anyOldUnit = 0;
	
	for (i=1; i<MAX_UNITS; ++i)
	{
		// scan through units for leaders
		if (units[i].Realm == theirRealm 
			&& units[i].Joined == 0
			)								// Ignore spotted flag as NPC's never spot units.
		{
			PROVINCE const UnitProv = UnitFinalProvince(i);
			
			// Get the troops in our realm first.
			if (province[UnitProv].Realm == ourRealm)
			{
				LONG iUnit;
				LONG u = 0;
				
				for (iUnit = i; iUnit > 0 && !u; iUnit = units[iUnit].NextUnit)
				{
				  	u = (random(3)==0
							&& units[iUnit].Icon < FIRST_CHARACTER_TYPE)?iUnit:0;
					if (u)
					{
						unitInOurRealm = u;
						goto LeaveFn; // That's it he's history.
					}
				}
			}
			// Get troops in our allies realm next.
			else if (unitInAlly == 0 &&
			         AREFULLALLIES(province[UnitProv].Realm, ourRealm))
			{
				LONG iUnit;
				LONG u = 0;
				
				for (iUnit = i; iUnit > 0 && !u; iUnit = units[iUnit].NextUnit)
				{
				  	u = (random(3)==0
							&& units[iUnit].Icon < FIRST_CHARACTER_TYPE)?iUnit:0;
					if (u)
					{
						unitInAlly = u;
					}
				}
			}
			// Get Troops on our border next
			else if (unitOnBorder == 0 &&
			    IsBorder(UnitProv, ourRealm, 0, ADJACENT))
			{
				LONG iUnit;
				LONG u = 0;
				
				for (iUnit = i; iUnit > 0 && !u; iUnit = units[iUnit].NextUnit)
				{
				  	u = (random(3)==0
							&& units[iUnit].Icon < FIRST_CHARACTER_TYPE)?iUnit:0;
					if (u)
					{
						unitOnBorder = u;
					}
				}
			}
			else if (anyOldUnit == 0)
			{
				LONG iUnit;
				LONG u = 0;
				
				for (iUnit = i; iUnit > 0 && !u; iUnit = units[iUnit].NextUnit)
				{
				  	u = (random(3)==0
							&& units[iUnit].Icon < FIRST_CHARACTER_TYPE)?iUnit:0;
					if (u)
					{
						anyOldUnit = u;
					}
				}
			}
		}
	}
LeaveFn:
	if (unitInOurRealm != 0)
	{
		TargetUnit = unitInOurRealm;
		TargetProvince = UnitFinalProvince(TargetUnit);
		Result = TRUE;
	}
	else if (unitInAlly != 0)
	{
		TargetUnit = unitInAlly;
		TargetProvince = UnitFinalProvince(TargetUnit);
		Result = TRUE;
	}
	else if (unitOnBorder != 0)
	{
		TargetUnit = unitOnBorder;
		TargetProvince = UnitFinalProvince(TargetUnit);
		Result = TRUE;
	}
	else if (anyOldUnit != 0)
	{
		TargetUnit = anyOldUnit;
		TargetProvince = UnitFinalProvince(TargetUnit);
		Result = TRUE;
	}
	
	
	return Result;
}
/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
BOOL FindUnit (REALM::REALM_TYPE realm_, LONG action)
{
	LONG	i, c, iUnit;
	LONG	j = 0;
	LONG	cUnits = 0;
	LONG	f = FALSE;
	LONG	u = 0;
	LONG	strTarget;
	LONG	min_dist = 99999999;
	PROVINCE	UnitProv = NO_PROVINCE;
	PROVINCE	up = NO_PROVINCE;

	for (i=1; i<MAX_UNITS; ++i)
	{
		// scan through units for leaders
		if (units[i].Realm == realm_ && units[i].Joined == 0)
		{
			UnitProv = UnitFinalProvince(i);

			switch (action)
			{
				case ACTION_MOVE_BORDER_FOE:
				case ACTION_MOVE_BORDER_OWN:
					UnitsHere(UnitProv, CurrentRealm);	// set cGroups
					if (!IsBorder(UnitProv, TargetRealm, 0, ADJACENT)
						//&& UnitProv != TargetProvince		// dont test vs TargetProvince
						&& cGroups > j)			// find max groups
						{
							j = cGroups;
							u = i;
							up = UnitProv;
						}
					break;

				case ACTION_MOVE_FILL_FOE:
				case ACTION_MOVE_FILL_OWN:
				case ACTION_MOVE_REINFORCE_FOE:
				case ACTION_MOVE_REINFORCE_OWN:
				case ACTION_MOVE_UNOCCUP_FOE:
				case ACTION_MOVE_UNOCCUP_OWN:
					UnitsHere(UnitProv, CurrentRealm);	// set cGroups
					
					if (cGroups > j)			// find max groups
					{
						j = cGroups;
						u = i;
						up = UnitProv;
					}
					break;

			case ACTION_MOVE_RETREAT:
					f = (province[UnitProv].Realm != CurrentRealm);
					up = UnitProv;
					break;

			case ACTION_MOVE_ATTACK_FOE:
			case ACTION_MOVE_ATTACK_OWN:
					strTarget = strTargetProvince;
					UnitsHere(UnitProv, CurrentRealm);
					c = 0;
					if (strTargetProvince > strTarget				// big enough
						&& (c=ProvDist(UnitProv,TargetProvince,0,FALSE)) < min_dist )	// closest
					{
						min_dist = c;
						u = i;
						up = UnitProv;
					}
					break;
				 
	 		case ACTION_RSPELL_BLESSARMY:
					if (UnitProv == TargetProvince)
					{
						for (iUnit = i; iUnit > 0 && !u; iUnit = units[iUnit].NextUnit)
						  	u = (units[iUnit].Blessed == FALSE && random(2)==0)?iUnit:0;
						up = UnitProv;
					}
					break;

			case ACTION_RSPELL_MASSDESTRUCTION:
					if (FindProvWithSrcTmp(CurrentRealm, UnitProv, 1, 5))
					{
						for (iUnit = i; iUnit > 0 && !u; iUnit = units[iUnit].NextUnit)
						{
						  	u = (units[iUnit].Icon != GORGON_UNIT_ICON 
								 	&& units[iUnit].Icon != RHUOBHE_UNIT_ICON 
									&& random(4)==0
									&& iUnit != regents[realm[realm_].mfGetRegent()].mfGetunit()) 
									?iUnit:0;
							if (u)
								up = TargetProvince = UnitProv;
						}
					}
					break;

			case ACTION_ESPIONAGE_ASSASSIN:
					for (iUnit = i; iUnit > 0 && !u; iUnit = units[iUnit].NextUnit)
					{
					  	u = (random(3)==0
								&& units[iUnit].Icon < FIRST_CHARACTER_TYPE)?iUnit:0;
						if(u)
							up = TargetProvince = UnitProv;
					}
				 	break;
			}
		}

		if (f)		// if current unit is selected
		{
			TargetUnit = i;
			UnitProvince = up;
			//printf("TUnt:%d ",TargetUnit);
			return TRUE;
		}
	}

	if (u)			// if a unit was selected
	{
		TargetUnit = u;
		UnitProvince = up;
	//printf("TUnt:%d ",TargetUnit);
		return TRUE;
	}

	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	FLOGCOMMENT
	{
		char temp[100];
		sprintf ( temp, "   FindUnit %d - Failed", 10 );
		if ( fLogComment )
			RandomLogComment ( temp );
		else
			Sleep(0);
	}
	#endif

	return FALSE;
}

/* ========================================================================
   Function    - FindNotHolding
   Description - Find a province to build a new holding in.
   Returns     -
   ======================================================================== */
static BOOL FindNotHolding (
	REALM::REALM_TYPE ourRealm, 
	REALM::REALM_TYPE inRealm, 
	LONG icon,
	PROVINCE *inProv)
{
	LONG	i, j, k, c;

	for (i=1; i<PROVINCE_COUNT; ++i)
	{
		if (province[i].Realm == inRealm)
		{
			CheckAndFixPlaces((PROVINCE)i);
			j = k = province[i].FirstPlace; c=0;
			while (k != 0 && c++<PROVINCE_COUNT && j>=0 && j<MAX_PLACES)
			{
				if (places[j].Realm == ourRealm && places[j].Icon == icon)
					break;
				k = places[j].NextPlace;
				j = province[i].FirstPlace + k;
			}
			if (k == 0)		// province without holding of type icon
			{
				*inProv = (PROVINCE)i;

				#ifdef _WINDOWS
				LOGCOMMENTSLEEP
				FLOGCOMMENT
				{
					char temp[100];
					sprintf ( temp, "   FindNotHolding %d - succeeded; found province(k)=%d, limiter(c)=%d, j=%d, province(i)=%d",
						11,
						k,c,j,i );
					if ( fLogComment )
						RandomLogComment ( temp );
					else
						Sleep(0);
				}
				#endif

				return TRUE;
			}
		}
	}

	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	FLOGCOMMENT
	{
		char temp[100];
		sprintf ( temp, "   FindNotHolding %d - failed; found province(k)=%d, limiter(c)=%d, j=%d, province(i)=%d",
			12,
			k,c,j,i );
		if ( fLogComment )
			RandomLogComment ( temp );
		else
			Sleep(0);
	}
	#endif

	return FALSE;
}

/* ========================================================================
   Function    - DoIHaveHolding
   Description - Does almost the same as FindHolding but doesn't skip level
                 zero castles, and stops at the first one found.
   Returns     - 
   ======================================================================== */

static BOOL DoIHavePlace (PROVINCE prov, REALM::REALM_TYPE placerealm, MAP_ICON icon)
{
	BOOL	FoundOne = FALSE;
	LONG j;
	LONG k;
	LONG cnt = 0;
	
	CheckAndFixPlaces(prov);
	j = k = province[prov].FirstPlace;
	//GEH
	// if k !=0, do I have any places at all
	//    cnt is counting the number of holding we are allowed to have of
	//      any given type, which is one per province
	//    k is is the jump list offset
	//    j is the next place in the places array
	while (k != 0 && cnt++<PROVINCE_COUNT && j>=0 && j<MAX_PLACES)
	{
		if ((placerealm == REALM::ANY_REALM || places[j].Realm == placerealm)
			&& (icon == ANY_MAP_ICON || places[j].Icon == icon) )
		{
			FoundOne = TRUE;
			goto foundone;
		}
		k = places[j].NextPlace;
		j = province[prov].FirstPlace + k;
	}

	FoundOne = FALSE;
	
	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	FLOGCOMMENT
	{
		char temp[100];
		sprintf ( temp, "   DoIHaveHolding %d - Failed",13 );
		if ( fLogComment )
			RandomLogComment ( temp );
		else
			Sleep(0);
	}
	#endif
	
foundone:
	return FoundOne;
}
/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
static BOOL FindHolding (REALM::REALM_TYPE provrealm, REALM::REALM_TYPE placerealm, PROVINCE prov, MAP_ICON icon, LONG fMax)
{
	LONG	i, j, k, c, tr, cnt;
	LONG	p = 0;
	LONG	h = 0;
	LONG	max = 0;
	LONG	min = 999;

	for (i=1; i<PROVINCE_COUNT; ++i)
	{
		if (prov == ANY_PROVINCE || i == prov)
		{
			if (province[i].Realm == provrealm || provrealm == REALM::ANY_REALM)
			{
				CheckAndFixPlaces((PROVINCE)i);
				j = k = province[i].FirstPlace; cnt = 0;
				while (k != 0 && cnt++<PROVINCE_COUNT && j>=0 && j<MAX_PLACES)
				{
					if (	((placerealm != REALM::ANY_REALM && places[j].Realm == placerealm) ||
							 (placerealm == REALM::ANY_REALM && provrealm != REALM::ANY_REALM && places[j].Realm != provrealm))
						&& (!icon || places[j].Icon == icon) )
					{
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// add variation
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						c = places[j].Level;

						if (icon == GUILD1_ICON)
						{
							c = province[i].CivLevel;
							tr = (places[j].iRoute1!=0)+(places[j].iRoute2!=0)+(places[j].iRoute3!=0);
							if (tr >= ((c+2)/3) )		// can't make more trade routes
								c = 0;
						}

						if (fMax && c > max)		// find max
						{
							max = c;
							p = i;
							h = j;
						}
						else if (fMax && icon == CASTLE1_ICON && c == 0)
							continue;	// don't do anything if the level of the castle is zero

						if (!fMax && c < min)		// find min
						{
							min = c;
							p = i;
							h = j;
						}
					}
					k = places[j].NextPlace;
					j = province[i].FirstPlace + k;
				}
			}
		}
	}

	if (h>0 && p>0 && p<PROVINCE_COUNT)
	{
		TargetProvince = (PROVINCE)p;
		TargetPlace = h;
		return TRUE;
	}

	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	FLOGCOMMENT
	{
		char temp[100];
		sprintf ( temp, "   FindHolding %d - Failed",13 );
		if ( fLogComment )
			RandomLogComment ( temp );
		else
			Sleep(0);
	}
	#endif

	return FALSE;
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
BOOL CheckHoldLevels (PROVINCE prov, LONG icon)
{
	LONG	j, k, lvl, maxlvls, c;

	if (icon == SOURCE1_ICON)
		maxlvls = province[prov].MagLevel;
	else
		maxlvls = province[prov].CivLevel;
	lvl = 0;

	CheckAndFixPlaces(prov);
	j = k = province[prov].FirstPlace; c = 0;
	while (k != 0 && c++<PROVINCE_COUNT && j>=0 && j<MAX_PLACES)
	{
		if (places[j].Icon == icon)
			lvl += places[j].Level;
		k = places[j].NextPlace;
		j = province[prov].FirstPlace + k;
	}

	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	FLOGCOMMENT
	{
		if (lvl >= maxlvls);
		{
			char temp[100];
			sprintf ( temp, "   	CheckHoldLevels %d - Failed", 14 );
			if ( fLogComment )
				RandomLogComment ( temp );
			else
				Sleep(0);
		}
	}
	#endif

	return (lvl < maxlvls);
}
/* ========================================================================
   Function    - FindLawToRule
   Description - Search for a Law holding to rule up.
   Returns     - TRUE or FALSE if one is found.
   ======================================================================== */
static BOOL FindLawToRule(REALM::REALM_TYPE ourRealm, PROVINCE *inProv, USHORT *inPlace)
{
	LONG Result = FALSE;
	LONG i;
	LONG maxLawLevel = -1;
	PROVINCE maxLawLevelProv = NO_PROVINCE;
	LONG maxLawPlace = -1;
	
	for (i = 0; i < PROVINCE_COUNT; ++i)
	{
		if (province[i].Realm == ourRealm)
		{
			LONG j;
			LONG k;
			LONG cnt = 0;
			
			j = k = province[i].FirstPlace;
			while( k != 0 && cnt++ < PROVINCE_COUNT && j >=0 && j < MAX_PLACES)
			{
				if (places[j].Icon == LAW1_ICON 
					&& places[j].Realm == ourRealm 
					)
				{
					LONG const PlaceLevel = places[j].Level;
					
					// Only consider law holdings we should rule up.
					if ( (province[i].CivLevel/2) > PlaceLevel)
					{
						if (PlaceLevel > maxLawLevel)
						{
							maxLawLevel = PlaceLevel;
							maxLawLevelProv = (PROVINCE) i;
							maxLawPlace = j;
						}
					}
					break;	// We have only one Law per province.
				}
				k = places[j].NextPlace;
				j = province[i].FirstPlace + k;
			}
		}
	}
	
	// Rule up Law holding to reduce rebellions.
	// In our own provinces only
	if (maxLawLevelProv != NO_PROVINCE)
	{
		*inProv = maxLawLevelProv;
		*inPlace = maxLawPlace;
		Result = TRUE;
	}
	
	return Result;
}

/* ========================================================================
   Function    - FindGuildToRule
   Description - Search for a Guild to rule up.
   Returns     - TRUE or FALSE if one is found.
   ======================================================================== */
static BOOL FindGuildToRule(REALM::REALM_TYPE ourRealm, PROVINCE *inProv, USHORT *inPlace)
{
	LONG Result = FALSE;
	LONG i;
	LONG maxGuildLevel = -1;
	PROVINCE maxGuildLevelProv = NO_PROVINCE;
	LONG maxGuildPlace = -1;
	
	LONG maxGuildWithTradeRtLevel = -1;
	PROVINCE maxGuildWithTradeRtLevelProv = NO_PROVINCE;
	LONG maxGuildWithTradeRtPlace = -1;
	
	LONG maxGuildWithTradeRtLevelInAlly = -1;
	PROVINCE maxGuildWithTradeRtLevelProvInAlly = NO_PROVINCE;
	LONG maxGuildWithTradeRtPlaceInAlly = -1;
	
	LONG maxGuildLevelInAlly = -1;
	PROVINCE maxGuildLevelProvInAlly = NO_PROVINCE;
	LONG maxGuildPlaceInAlly = -1;
	
	for (i = 0; i < PROVINCE_COUNT; ++i)
	{
		LONG j;
		LONG k;
		LONG cnt = 0;
		
		j = k = province[i].FirstPlace;
		while( k != 0 && cnt++ < PROVINCE_COUNT && j >=0 && j < MAX_PLACES)
		{
			if (places[j].Icon == GUILD1_ICON 
				&& places[j].Realm == ourRealm 
				)
			{
				LONG const PlaceLevel = places[j].Level;
				
				// Only consider guilds we can rule up.
				if ( province[i].CivLevel > PlaceLevel)
				{
					if (province[i].Realm == ourRealm)
					{
						if (places[j].iRoute1 || places[j].iRoute2 || places[j].iRoute3)
						{
							if (PlaceLevel > maxGuildWithTradeRtLevel)
							{
								maxGuildWithTradeRtLevel = PlaceLevel;
								maxGuildWithTradeRtLevelProv = (PROVINCE) i;
								maxGuildWithTradeRtPlace = j;
							}
						}
						else
						{
							if (PlaceLevel > maxGuildLevel)
							{
								maxGuildLevel = PlaceLevel;
								maxGuildLevelProv = (PROVINCE) i;
								maxGuildPlace = j;
							}
						}
					}
					else if (AREFULLALLIES(province[i].Realm, ourRealm))
					{
						if (places[j].iRoute1 || places[j].iRoute2 || places[j].iRoute3)
						{
							if (PlaceLevel > maxGuildWithTradeRtLevelInAlly)
							{
								maxGuildWithTradeRtLevelInAlly = PlaceLevel;
								maxGuildWithTradeRtLevelProvInAlly = (PROVINCE) i;
								maxGuildWithTradeRtPlaceInAlly = j;
							}
						}
						else
						{
							if (PlaceLevel > maxGuildLevelInAlly)
							{
								maxGuildLevelInAlly = PlaceLevel;
								maxGuildLevelProvInAlly = (PROVINCE) i;
								maxGuildPlaceInAlly = j;
							}
						}
					}
				}
				break;	// We have only one Guild per province.
			}
			k = places[j].NextPlace;
			j = province[i].FirstPlace + k;
		}
	}
	
	// Rule up guilds without trade routes first, to maximize profits.
	// In our own provinces first, our allies next.
	if (maxGuildLevelProv != NO_PROVINCE)
	{
		*inProv = maxGuildLevelProv;
		*inPlace = maxGuildPlace;
		Result = TRUE;
	}
	else if (maxGuildWithTradeRtLevelProv != NO_PROVINCE)
	{
		*inProv = maxGuildWithTradeRtLevelProv;
		*inPlace = maxGuildWithTradeRtPlace;
		Result = TRUE;
	}
	else if (maxGuildLevelProvInAlly != NO_PROVINCE)
	{
		*inProv = maxGuildLevelProvInAlly;
		*inPlace = maxGuildPlaceInAlly;
		Result = TRUE;
	}
	else if (maxGuildWithTradeRtLevelProvInAlly != NO_PROVINCE)
	{
		*inProv = maxGuildWithTradeRtLevelProvInAlly;
		*inPlace = maxGuildWithTradeRtPlaceInAlly;
		Result = TRUE;					   
	}
	
	return Result;
}

/* ========================================================================
   Function    - FindTempleToRule
   Description - Search for a high temple to get higher. We want to increase our best
   				 and safest sources.
   Returns     - TRUE or FALSE if one is found.
   ======================================================================== */
static BOOL FindTempleToRule(REALM::REALM_TYPE ourRealm, PROVINCE *inProv, USHORT *inPlace)
{
	LONG Result = FALSE;
	LONG i;
	
	LONG maxTempleLevel = -1;
	PROVINCE maxTempleLevelProv = NO_PROVINCE;
	LONG maxTemplePlace = -1;
	
	LONG maxTempleLevelInEnemy = -1;
	PROVINCE maxTempleLevelProvInEnemy = NO_PROVINCE;
	LONG maxTemplePlaceInEnemy;
	
	LONG maxTempleLevelInAlly = -1;
	PROVINCE maxTempleLevelProvInAlly = NO_PROVINCE;
	LONG maxTemplePlaceInAlly = -1;
	
	for (i = 0; i < PROVINCE_COUNT; ++i)
	{
		LONG j;
		LONG k;
		LONG cnt = 0;
		
		j = k = province[i].FirstPlace;
		while( k != 0 && cnt++ < PROVINCE_COUNT && j >=0 && j < MAX_PLACES)
		{
			if (places[j].Icon == TEMPLE1_ICON 
				&& places[j].Realm == ourRealm 
				)
			{
				LONG const PlaceLevel = places[j].Level;
				
				// Only consider Temples we can rule up.
				if (province[i].CivLevel > PlaceLevel
					&& PlaceLevel < 3)					// No need to rule above level 3
				{
					if (province[i].Realm == ourRealm)
					{
						if (PlaceLevel > maxTempleLevel)
						{
							maxTempleLevel = PlaceLevel;
							maxTempleLevelProv = (PROVINCE) i;
							maxTemplePlace = j;
						}
					}
					else if (AREFULLALLIES(province[i].Realm, ourRealm))
					{
						if (PlaceLevel > maxTempleLevelInAlly)
						{
							maxTempleLevelInAlly = PlaceLevel;
							maxTempleLevelProvInAlly = (PROVINCE) i;
							maxTemplePlaceInAlly = j;
						}
					}
					else if (ISATWAR(province[i].Realm, ourRealm))
					{
						if (PlaceLevel > maxTempleLevelInEnemy)
						{
							maxTempleLevelInEnemy = PlaceLevel;
							maxTempleLevelProvInEnemy = (PROVINCE) i;
							maxTemplePlaceInEnemy = j;
						}
					}
				}
				break;	// We have only one Temple per province.
			}
			k = places[j].NextPlace;
			j = province[i].FirstPlace + k;
		}
	}
	
	if (maxTempleLevelProvInEnemy != NO_PROVINCE)
	{
		*inProv = maxTempleLevelProvInEnemy;
		*inPlace = maxTemplePlaceInEnemy;
		Result = TRUE;
	}
	else if (maxTempleLevelProv != NO_PROVINCE)
	{
		*inProv = maxTempleLevelProv;
		*inPlace = maxTemplePlace;
		Result = TRUE;
	}
	else if (maxTempleLevelProvInAlly != NO_PROVINCE)
	{
		*inProv = maxTempleLevelProvInAlly;
		*inPlace = maxTemplePlaceInAlly;
		Result = TRUE;
	}
	
	return Result;
}
/* ========================================================================
   Function    - FindSrcToRule
   Description - Search for a high src to get higher. We want to increase our best
   				 and safest sources. Also level zero src's should be ruled to
   				 level 1 so it takes 2 contests to distroy them.
   Returns     - TRUE or FALSE if one is found.
   ======================================================================== */
static BOOL FindSrcToRule(REALM::REALM_TYPE ourRealm, PROVINCE *inProv, USHORT *inPlace)
{
	LONG Result = FALSE;
	LONG i;
	
	LONG maxSrcLevel = -1;
	PROVINCE maxSrcLevelProv = NO_PROVINCE;
	LONG maxSrcPlace = -1;
	
	LONG maxSrcLevelInEnemy = -1;
	PROVINCE maxSrcLevelProvInEnemy = NO_PROVINCE;
	LONG maxSrcPlaceInEnemy = -1;
	
	PROVINCE levelZeroSrcProv = NO_PROVINCE;
	LONG levelZeroSrcPlace = -1;
	
	BOOL WeHaveAHighLevelSrc = FALSE;
	
	for (i = 0; i < PROVINCE_COUNT; ++i)
	{
		if (province[i].Realm == ourRealm ||
			AREFULLALLIES(province[i].Realm, ourRealm)
			)
		{
			LONG j;
			LONG k;
			LONG cnt = 0;
			
			j = k = province[i].FirstPlace;
			while( k != 0 && cnt++ < PROVINCE_COUNT && j >=0 && j < MAX_PLACES)
			{
				if (places[j].Icon == SOURCE1_ICON 
					&& places[j].Realm == ourRealm 
					)
				{
					LONG const PlaceLevel = places[j].Level;
					if (PlaceLevel >= 7)
					{
						WeHaveAHighLevelSrc = TRUE;
					}
					else if (PlaceLevel == 0)
					{
						levelZeroSrcProv = (PROVINCE) i;
						levelZeroSrcPlace = j;
					}
					
					if (PlaceLevel > maxSrcLevel
						&& PlaceLevel < 8				// No need to rule higher than level 7.
						&& province[i].MagLevel > PlaceLevel)
					{
						maxSrcLevel = PlaceLevel;
						maxSrcLevelProv = (PROVINCE) i;
						maxSrcPlace = j;
					}
					break;	// We have only one Src per province.
				}
				k = places[j].NextPlace;
				j = province[i].FirstPlace + k;
			}
		}
		else
		{
			// Don't bother searching our enemy provinces if we have a src
			// we could be ruleing up already.
			LONG j;
			LONG k;
			LONG cnt = 0;
			
			j = k = province[i].FirstPlace;
			while( k != 0 && cnt++ < PROVINCE_COUNT && j >=0 && j < MAX_PLACES)
			{
				if (places[j].Icon == SOURCE1_ICON 
					&& places[j].Realm == ourRealm 
					)
				{
					LONG const PlaceLevel = places[j].Level;
					
					if (PlaceLevel == 0 && levelZeroSrcProv == NO_PROVINCE)
					{
						levelZeroSrcProv = (PROVINCE) i;
						levelZeroSrcPlace = j;
					}
					
					if (PlaceLevel > maxSrcLevelInEnemy
						&& PlaceLevel < 8				// No need to rule higher than level 7.
						&& province[i].MagLevel > PlaceLevel)
					{
						maxSrcLevelInEnemy = PlaceLevel;
						maxSrcLevelProvInEnemy = (PROVINCE) i;
						maxSrcPlaceInEnemy = j;
					}
					break;	// We have only one Src per province.
				}
				k = places[j].NextPlace;
				j = province[i].FirstPlace + k;
			}
		}
	}
	
	if (!WeHaveAHighLevelSrc && maxSrcLevelProv != NO_PROVINCE)
	{
		*inProv = maxSrcLevelProv;
		*inPlace = maxSrcPlace;
		Result = TRUE;
	}
	else if (!WeHaveAHighLevelSrc && maxSrcLevelProvInEnemy != NO_PROVINCE)
	{
		*inProv = maxSrcLevelProvInEnemy;
		*inPlace = maxSrcPlaceInEnemy;
		Result = TRUE;
	}
	else if (levelZeroSrcProv != NO_PROVINCE)
	{
		*inProv = levelZeroSrcProv;
		*inPlace = levelZeroSrcPlace;
	}
	
	
	return Result;
}
/* ========================================================================
   Function    - FindDiffTerrain
   Description - Look for a connected terrain which is different.
   Returns     - TRUE if found one FALSE otherwise.
   ======================================================================== */
static BOOL FindDiffTerrain (PATHTYPE via, PROVINCE StartingProv, PROVINCE *foundProv)
{
	LONG	i, d, p = 0;
	LONG	terrain = province[StartingProv].Terrain;
	REALM::REALM_TYPE	realm_ = (REALM::REALM_TYPE)province[StartingProv].Realm;
	LONG	min = 999999;

	if (via == ANY_WAY)
	{
		for (i=1; i<PROVINCE_COUNT; ++i)
		{
			if (province[i].Realm == realm_ 
				&& province[i].Terrain != terrain)
			{
				d = ProvDist(StartingProv, i, 0, FALSE);
				if (d < min)
				{
					min = d;
					p = i;
				}
			}
		}

		if (p)
		{
			*foundProv = (PROVINCE)p;
			return TRUE;
		}


		return FALSE;
	}


	// VIA_ROAD
	if (RecursiveRoadFollower(StartingProv) == FALSE)
	{


		return FALSE;
	}

	for (i=1; i<PROVINCE_COUNT; ++i)
	{
		if (iSelectedHolding[i][3]==TRUE
			&& province[i].Terrain != terrain)			// check for existing road
		{
			*foundProv = (PROVINCE)i;
			return TRUE;
		}
	}

	return FALSE;
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     - TRUE or FALSE if we can pay.
   ======================================================================== */
BOOL CanAfford (LONG costGBNow, LONG costRegNow, LONG costGBPerTurn, REALM::REALM_TYPE realm_)
{
	BOOL	rv;

	if (realm_ == REALM::MONSTERS || realm_ == REALM::FIVEPEAKS)
		costRegNow = 0;

	// if more than 500 GB then don't worry about maintenance
	if (realm[realm_].mfGetTreasury() > 5000)
	{
		rv = (costGBNow <= (realm[realm_].mfGetTreasury()/10)
					&& costRegNow <= realm[realm_].mfGetRegency());

		#ifdef _WINDOWS
		LOGCOMMENTSLEEP
		FLOGCOMMENT
		{
			if (!rv)
			{
				char temp[100];
				sprintf ( temp, "   CanAfford %d - Failed1", 18 );
				if ( fLogComment )
					RandomLogComment ( temp );
				else
					Sleep(0);
			}
		}
		#endif

		return rv;
	}

	rv = (costGBNow <= (realm[realm_].mfGetTreasury()/10)
					&& costRegNow <= realm[realm_].mfGetRegency()
					&& costGBPerTurn <= (realm[realm_].mfGetSurplusIncome()/10));

	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	FLOGCOMMENT
	{
		if (!rv)
		{
			char temp[100];
			sprintf ( temp, "   CanAfford %d - Failed2", 19 );
			if ( fLogComment )
				RandomLogComment ( temp );
			else
				Sleep(0);
		}
	}
	#endif

	return rv;
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
static void PayForIt (REALM::REALM_TYPE ThisRealm, LONG payGB, LONG payRegency, LONG AllocateSurplus)
{
	if (realm[ThisRealm].mfGetTreasury() < (payGB*10))
		realm[ThisRealm].mfSetTreasury(0);
	else
		realm[ThisRealm].mfAddTreasury(-(payGB*10));

	if (realm[ThisRealm].mfGetRegency() < payRegency)
		realm[ThisRealm].mfSetRegency(0);
	else
		realm[ThisRealm].mfAddRegency( -payRegency);

	//SetGameData(MP_REALM, MPRLM_REGENCY, ThisRealm, realm[ThisRealm].regency, TRUE);

	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	FLOGCOMMENT
	{
		char temp[100];
		sprintf ( temp, "   PayForIt %d - %s  GB spent:%d  GB now (x10):%d   Reg spent:%d  Reg now:%d", 
					20,
					realm[ThisRealm].mfGetName(),
					payGB,
					realm[ThisRealm].mfGetTreasury(),
					payRegency,
					realm[ThisRealm].mfGetRegency());
		if ( fLogComment )
			RandomLogComment ( temp );
		else
			Sleep(0);
	}
	#endif

	if (realm[ThisRealm].mfGetSurplusIncome() < (AllocateSurplus*10))
		realm[ThisRealm].mfSetSurplusIncome(0);
	else
		realm[ThisRealm].mfAddSurplusIncome(-(AllocateSurplus*10));
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     - TRUE or FALSE if one is found.
   ======================================================================== */
BOOL ForeignUnitsHere (PROVINCE prov, REALM::REALM_TYPE _realm)
{
	LONG	i;

	for (i=1; i<MAX_UNITS; i++)
	{
		if (units[i].Realm != REALM::NO_COUNTRY
			&& units[i].Joined==0								// is leader
			&& units[i].province == prov					// in this prov
			&& !AREALLIED((REALM::REALM_TYPE)units[i].Realm, _realm) ) // not our ally
		{
			return TRUE;
		}
	}
	return FALSE;
}

/* ================================== */
BOOL BorderPermitted (REALM::REALM_TYPE source_realm, REALM::REALM_TYPE dest_realm, PROVINCE dest_prov)
{
	LONG	i;
	BOOL	fOccup = FALSE;

	// we can move into any prov where we already have troops
	for (i=1; i<MAX_UNITS; i++)
	{
		if ( units[i].Joined==0								// is leader
			&& units[i].province == dest_prov 			// unit is there
			&& units[i].Realm == source_realm )			// unit is source_realm's
		{
			fOccup = TRUE;										// allow border crossing
			break;
		}
	}

	return ( dest_realm==source_realm
				|| dest_realm==REALM::MONSTERS
				|| dest_realm==REALM::ANUIRE
				|| source_realm==REALM::MONSTERS
				|| ISATWAR(source_realm, dest_realm)
				|| AREALLIED(source_realm, dest_realm)
				|| province[dest_prov].OccupRealm == source_realm
				|| fOccup );
}

/* ================================== */
BOOL npc_move_ (PROVINCE cur_prov)
{
	LONG	i, dx, dy, dist, cnt, l;
	REALM::REALM_TYPE test_realm;
	PROVINCE	test_prov;
	PROVINCE	best_prov = NO_PROVINCE;
	LONG	best_border = 0;
	LONG	border_cost = 0;
	LONG	min_dist = 99999999;
	BOOL	fTerrainPassable;
	BOOL	fBorderPermitted;
	BOOL	fSend = (CurrentRealm == HomeRealm);
	BOOL	rv;

	if (cur_prov == TargetProvince)			// already there
		return TRUE;

	fProv[cur_prov] = TRUE;

	// scan through borders of current province to find the best way to go
	for (i=0; i<7; ++i)
	{
		test_prov = (PROVINCE) province[cur_prov].Border[i].Province;
		test_realm = (REALM::REALM_TYPE)province[test_prov].Realm;
		dx = province[TargetProvince].x - province[test_prov].x;
		dy = province[TargetProvince].y - province[test_prov].y;
		dist = (dx * dx) + (dy * dy);

		border_cost = TerrainCost[province[cur_prov].Border[i].MoveCost];
		fTerrainPassable = (border_cost < 100);
		fBorderPermitted = BorderPermitted(CurrentRealm,test_realm,test_prov);

		//dist = dist * (border_cost + 2);		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		dist = dist * (border_cost + 1);		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		//printf("%d:%d%d%d%d%d ",i,(dist<min_dist),(!fProv[test_prov]),fTerrainPassable,
		//	fBorderPermitted,(!CheckUnit2ndMoveBlocked(TargetUnit, cur_prov, test_prov)) );

		if (	dist < min_dist
				&& !fProv[test_prov]			// don't backtrack
				&& fTerrainPassable
				&& fBorderPermitted
				&& !CheckUnit2ndMoveBlocked(TargetUnit, cur_prov, test_prov)
				&& CheckNotWarded(TargetUnit, test_prov, cur_prov)
			)
		{
			min_dist = dist;
			best_prov = test_prov;
			best_border = i;
		}
	}

	// if player-controlled and we are walking away from destination then stop
	if (fSend && min_dist > old_dist)
	{
		reqSound = SND_UI_IMPASSABLE;
		return (units[TargetUnit].DestProvince != 0);
	}

	if (best_prov != NO_PROVINCE)		// found a good province
	{
		// check for afford
		cnt = 1;
		l = TargetUnit;
		while (units[l].NextUnit > 0)
		{
			++cnt;
			l = units[l].NextUnit;
		}

		border_cost = TerrainCost[province[cur_prov].Border[best_border].MoveCost];
		if (playerstats[realm[CurrentRealm].mfGetRegent()] != fERROR)
		{
			PTR_PLAYER_STATS pPS = (PTR_PLAYER_STATS)BLKPTR(playerstats[realm[CurrentRealm].mfGetRegent()]);
			border_cost += pPS->StatsMod.mfGetMod(STATSMOD::MOVECOST_MOD);
			if (border_cost < 0) border_cost = 0;
		}

		if ((border_cost * cnt) > realm[CurrentRealm].mfGetTreasury())
		{
			if (fSend) reqSound = SND_UI_NOT_ENOUGH_GOLD;

			#ifdef _WINDOWS
			LOGCOMMENTSLEEP
			FLOGCOMMENT
			{
				char temp[100];
				sprintf ( temp, "   npc_move %d - Failed, could not afford  border:%d cnt:%d treas:%d", 
					21,
					border_cost, 
					cnt, 
					realm[CurrentRealm].mfGetTreasury());
				if ( fLogComment )
					RandomLogComment ( temp );
				else
					Sleep(0);
			}
			#endif

			return FALSE;
		}

		if (units[TargetUnit].DestProvince==0)
			//units[TargetUnit].DestProvince = best_prov;
			SetGameData(MP_UNITS, MPUNITS_DESTPROVINCE, TargetUnit, best_prov, FALSE);
		else if (units[TargetUnit].DestPath1==0)
			//units[TargetUnit].DestPath1 = best_border+1;
			SetGameData(MP_UNITS, MPUNITS_DESTPATH1, TargetUnit, best_border+1, FALSE);
		else if (units[TargetUnit].DestPath2==0)
			//units[TargetUnit].DestPath2 = best_border+1;
			SetGameData(MP_UNITS, MPUNITS_DESTPATH2, TargetUnit, best_border+1, FALSE);
		else if (units[TargetUnit].DestPath3==0)
			//units[TargetUnit].DestPath3 = best_border+1;
			SetGameData(MP_UNITS, MPUNITS_DESTPATH3, TargetUnit, best_border+1, FALSE);
		else
		{
			#ifdef _WINDOWS
			LOGCOMMENTSLEEP
			FLOGCOMMENT
			{
				char temp[100];
				sprintf ( temp, "   npc_move %d - Ran out of path slots", 22 );
				if ( fLogComment )
					RandomLogComment ( temp );
				else
					Sleep(0);
			}
			#endif
			rv = FALSE;		// [abc] 9/4 this may be the proper return value
		}

		// pay for it
		realm[CurrentRealm].mfAddTreasury( -(TerrainCost[province[cur_prov].Border[best_border].MoveCost] * cnt));
		if (fSend)
			SetGameData(MP_REALM, MPRLM_TREASURY, CurrentRealm, realm[CurrentRealm].mfGetTreasury(), TRUE);

		if (best_prov == TargetProvince)		// we're there !!
			return TRUE;

		old_dist = min_dist;
		rv = npc_move_(best_prov);		// do next step

		if (fSend)							// if player-controlled
			return (units[TargetUnit].DestProvince != 0);  // true if any move

		#ifdef _WINDOWS
		LOGCOMMENTSLEEP
		FLOGCOMMENT
		if (rv == FALSE)
		{
			char temp[100];
			sprintf ( temp, "   npc_move - Failed, next step returned FALSE" );
			RandomLogComment ( temp );
		}
		#endif

		return rv;
	}

	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	FLOGCOMMENT
	{
		char temp[100];
		sprintf ( temp, "   npc_move %d - Failed, no acceptable border", 22 );
		if ( fLogComment )
			RandomLogComment ( temp );
		else
			Sleep(0);
	}
	#endif

	return FALSE;
}


/* ======================================================================== */
BOOL npc_move (PROVINCE cur_prov)
{
	LONG	i, dx, dy;
	BOOL	fSend = (CurrentRealm == HomeRealm);
	BOOL	retVal;

	if (cur_prov == NO_PROVINCE)
		cur_prov = (PROVINCE) UnitFinalProvince(TargetUnit);

	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	FLOGCOMMENT
	{
		char temp[120];
		if (fSend)
			sprintf ( temp, "npc_move %d - Player %s Move - TargetUnit:%d cur_prov:%s TargetProvince:%s",
				23,
				realm[CurrentRealm].mfGetName(), 
				TargetUnit, 
				province[cur_prov].name, 
				province[TargetProvince].name);
		else
			sprintf ( temp, "   npc_move %d - TargetUnit:%d cur_prov:%s TargetProvince:%s",
				23,
				TargetUnit, 
				province[cur_prov].name, 
				province[TargetProvince].name);
		if ( fLogComment )
			RandomLogComment ( temp );
		else
			Sleep(0);
	}
	#endif

	// check for able to actually enter final province
	if ( !BorderPermitted(CurrentRealm, (REALM::REALM_TYPE)province[TargetProvince].Realm, TargetProvince) )
	{
		if ( CurrentRealm == HomeRealm )
			reqSound = SND_UI_YOU_CANNOT_CROSS_THAT_BORDER1;
		return FALSE;
	}

	if ( !CheckNotWarded(TargetUnit, TargetProvince, TargetProvince) )
	{
		if ( CurrentRealm == HomeRealm )
			reqSound = SND_UI_IMPASSABLE;
		return FALSE;
	}

	//units[TargetUnit].DestProvince = 0;
	SetGameData(MP_UNITS, MPUNITS_DESTPROVINCE, TargetUnit, 0, FALSE);
	//units[TargetUnit].DestPath1 = 0;
	SetGameData(MP_UNITS, MPUNITS_DESTPATH1, TargetUnit, 0, FALSE);
	//units[TargetUnit].DestPath2 = 0;
	SetGameData(MP_UNITS, MPUNITS_DESTPATH2, TargetUnit, 0, FALSE);
	//units[TargetUnit].DestPath3 = 0;
	SetGameData(MP_UNITS, MPUNITS_DESTPATH3, TargetUnit, 0, FALSE);
	//units[TargetUnit].MovePart = 0;
	SetGameData(MP_UNITS, MPUNITS_MOVEPART, TargetUnit, 0, FALSE);

	// calculate the current distance to the target province
	dx = province[TargetProvince].x - province[cur_prov].x;
	dy = province[TargetProvince].y - province[cur_prov].y;
	old_dist = (dx * dx) + (dy * dy);

	old_dist = old_dist * 4;	// equivalent to forest

	// call the actual lower level move, save the result
	retVal = npc_move_(cur_prov);

	// broadcast the new unit data
	#ifdef _WINDOWS
	if (fSend && IsMultiPlayer())
		AMultiPlayer.UnitMessage(TargetUnit, (UBYTE *)&units[TargetUnit], "MOVED");
	#endif

	return retVal;
}

/* ========================================================================
   Function    - RecursiveRoadBuilder
   Description - build a road from cur_prov to TargetProvince
   Returns     - TRUE if successful
   ======================================================================== */
BOOL RecursiveRoadBuilder (PROVINCE cur_prov)
{
	LONG	i, dx, dy, dist;
	PROVINCE	test_prov;
	PROVINCE	best_prov = NO_PROVINCE;
	LONG	best_border = 0;
	LONG	min_dist = 99999999;

	for (i=0; i<7; ++i)
	{
		test_prov = (PROVINCE) province[cur_prov].Border[i].Province;
		dx = province[TargetProvince].x - province[test_prov].x;
		dy = province[TargetProvince].y - province[test_prov].y;
		dist = (dx * dx) + (dy * dy);
		if (dist < min_dist && TerrainCost[province[cur_prov].Border[i].MoveCost] < 100)
		{
			min_dist = dist;
			best_prov = test_prov;
			best_border = i;
		}
	}

	if (best_prov)		// found a good province
	{
		if (best_prov != TargetProvince)
			if (RecursiveRoadBuilder(best_prov) == FALSE)
				return FALSE;

		//!!!!!!!!!!!!!!!!!!!
		// check for afford
		// pay for it
		//!!!!!!!!!!!!!!!!!!!

		// build a road segment if not already built
		if (province[cur_prov].Border[best_border].MoveCost != PLAINS_ROAD
			&& province[cur_prov].Border[best_border].MoveCost != RIVER_ROAD)
		{
//printf("Î");
			// set border one direction
			province[cur_prov].Border[best_border].MoveCost = PLAINS_ROAD;
			//SetGameData(MP_PROVINCE, MPPROV_BORDER0+best_border, cur_prov, PLAINS_ROAD, TRUE);
			DrawRoads_(cur_prov, best_border);

			// set border other direction
			for (i=0; i<7; ++i)
				if (province[best_prov].Border[i].Province == cur_prov)
				{
					province[best_prov].Border[i].MoveCost = PLAINS_ROAD;
					//SetGameData(MP_PROVINCE, MPPROV_BORDER0+i, best_prov, PLAINS_ROAD, TRUE);
					DrawRoads_(best_prov, i);
				}

			// draw the road the same way it's done in GAMEMAP.CPP
			//if (province[cur_prov].x < province[best_prov].x
			//	|| (province[cur_prov].x==province[best_prov].x
			//		&& province[cur_prov].y < province[best_prov].y))
			//	MapLine(province[cur_prov].x,province[cur_prov].y,province[best_prov].x,province[best_prov].y,MDBROWN,MDBROWN,TRUE);
			//else
			//	MapLine(province[best_prov].x,province[best_prov].y,province[cur_prov].x,province[cur_prov].y,MDBROWN,MDBROWN,TRUE);
		}
		return TRUE;
	}

	return FALSE;
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
BOOL npc_traderoute (LONG place)
{
	if (places[place].Icon != GUILD1_ICON)
	{
		#if defined(_DEBUG)
		fatal_error("Error %s[%d] Wrong place to put a trade route.\n.", __FILE__, __LINE__);
		#else
		return FALSE;
		#endif
	}
	
	if (places[place].iRoute1==0)
	{
		places[place].iRoute1 = TargetProvince;
		//printf("NPC Trade Route - holding:%d iRoute1 from %s to %s\n",place,province[StartProvince].name,province[TargetProvince].name);
		return TRUE;
	}
	else if (places[place].iRoute2==0)
	{
		places[place].iRoute2 = TargetProvince;
		//printf("NPC Trade Route - holding:%d iRoute2 from %s to %s\n",place,province[StartProvince].name,province[TargetProvince].name);
		return TRUE;
	}
	else if (places[place].iRoute3==0)
	{
		places[place].iRoute3 = TargetProvince;
		//printf("NPC Trade Route - holding:%d iRoute3 from %s to %s\n",place,province[StartProvince].name,province[TargetProvince].name);
		return TRUE;
	}

	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	FLOGCOMMENT
	{
		char temp[100];
		sprintf ( temp, "   npc_traderoute %d - Failed", 24 );
		if ( fLogComment )
			RandomLogComment ( temp );
		else
			Sleep(0);
	}
	#endif

	return FALSE;
}

/* ========================================================================
   Function    - 
   Description - fSend - broadcast or not 
   Returns     -
   ======================================================================== */
void SetHelped (REALM::REALM_TYPE Target, REALM::REALM_TYPE Active, BOOL fSend )
{
	LONG	k;

	if (Target != Active)
	{
		if (RealmStatus[Target][Active] & 0x80)
		{
			RealmStatus[Target][Active] &= 0x7F;			// clear SCREWED flag
			SetGameData(MP_REALMSTATUS, Target, Active, RealmStatus[Target][Active], fSend);
		}
		else
		{
			k = RealmReaction[Target][Active] + (random(5)+1);	// 1% to 5% increase
			RealmReaction[Target][Active] = (k<150)?k:150;
			SetGameData(MP_REALMREACTION, Target, Active, RealmReaction[Target][Active], fSend);
			if (Target<LAND_REALM_COUNT)
			{
				k = RealmReaction[Active][Target] + (random(5)+1);	// 1% to 5% increase
				RealmReaction[Active][Target] = (k<150)?k:150;
				SetGameData(MP_REALMREACTION, Active, Target, RealmReaction[Active][Target], fSend);
			}
		}
	}
}

/* ========================================================================
   Function    - 
   Description - fSend - broadcast or not 
   Returns     -
   ======================================================================== */
void SetScrewed (REALM::REALM_TYPE Target, REALM::REALM_TYPE Active, BOOL fSend )
{
	LONG	k;

	if (Target != REALM::ANY_REALM && Target != Active)
	{
		{
			RealmStatus[Target][Active] |= 0x80;				// set SCREWED flag
			SetGameData(MP_REALMSTATUS, Target, Active, RealmStatus[Target][Active], fSend);
			if (Target<LAND_REALM_COUNT)
			{
				RealmStatus[Active][Target] &= 0x7F;			// clear recip. SCREWED flag
				SetGameData(MP_REALMSTATUS, Active, Target, RealmStatus[Active][Target], fSend);
			}
		}
	}
}

/* ======================================================================== */
static BOOL TestInvaded (REALM::REALM_TYPE ourRealm, REALM::REALM_TYPE testRealm)
{
	LONG i, unit_test_prov;

	if (AREALLIED(ourRealm, testRealm))
		return FALSE;

	for (i=1; i<MAX_UNITS; ++i)							// test INVADED
	{
		if (units[i].Joined == 0 &&			// Only look at the leaders of stacks.
		    units[i].Realm == testRealm)
		{
			if (province[UnitFinalProvince(i)].Realm == ourRealm)
			{
				printf("TEST INVADED - %s has invaded %s in prov %s\n",realm[testRealm].mfGetName(),realm[ourRealm].mfGetName(),province[UnitFinalProvince(i)].name);
				TestTargetRealm = testRealm;
				return TRUE;
			}
		}
	}

	return FALSE;
}

/* ======================================================================== */
static BOOL TestRebellion (REALM::REALM_TYPE thisRealm)
{
	LONG	i;
	for (i=1; i<PROVINCE_COUNT; ++i)
		if (province[i].Realm == thisRealm && province[i].Loyalty == 0)
		{
			TestTargetRealm = thisRealm;
			TargetProvince = (PROVINCE)i;
			return TRUE;
		}
	return FALSE;
}

/* ======================================================================== */
static BOOL TestOccupied (REALM::REALM_TYPE thisRealm)
{
	LONG	i;
	for (i=1; i<PROVINCE_COUNT; ++i)
		if (province[i].Realm != thisRealm 
		    && province[i].OccupRealm == thisRealm)
		{
			TestTargetRealm = (REALM::REALM_TYPE)province[i].Realm;
			TargetProvince = (PROVINCE)i;
			return TRUE;
		}
	return FALSE;
}

/* ======================================================================== */
BOOL TestAdjacent (REALM::REALM_TYPE ActiveRealm, REALM::REALM_TYPE testRealm)
{
	LONG	i, j;

	for (i=1; i<PROVINCE_COUNT; ++i)
		//if ((province[i].Realm == ActiveRealm || AREALLIED(province[i].Realm,ActiveRealm))
		if ( province[i].Realm == ActiveRealm
				&& IsBorder((PROVINCE)i, testRealm, 0, ADJACENT) )
			return TRUE;
	return FALSE;
}

/* ======================================================================== */
static BOOL TestBroke (REALM::REALM_TYPE thisRealm)
{
	if (realm[thisRealm].mfGetTreasury() < 5)
	{
		TestTargetRealm = thisRealm;
		return TRUE;
	}
	return FALSE;
}

/* ======================================================================== */
BOOL TestHated (REALM::REALM_TYPE testRealm)
{
	return (RealmReaction[CurrentRealm][testRealm] <= 25);
}

/* ======================================================================== */
static BOOL TestAlly (REALM::REALM_TYPE thisRealm,REALM::REALM_TYPE testRealm)
{
	return (AREALLIED(thisRealm,testRealm));
}

/* ======================================================================== */
static BOOL TestHatedAtwar (REALM::REALM_TYPE testRealm)
{
	LONG	i;
	for (i=1; i<LAND_REALM_COUNT; ++i)
		if (ISATWAR(testRealm,i))
			return TRUE;
	return FALSE;
}

/* ======================================================================== */
static BOOL TestAllyAtwar (REALM::REALM_TYPE thisRealm, REALM::REALM_TYPE testRealm)
{
	LONG	i;
//////////////////////////////////
//	static LONG	counter = 0;
//////////////////////////////////

	if (AREALLIED(thisRealm, testRealm))
	{
		for (i=1; i<LAND_REALM_COUNT; ++i)
			if (ISATWAR(testRealm,i))
			{
				TestTargetRealm = (REALM::REALM_TYPE)i;
	
//////////////////////////////////
//			if (CurrentRealm != REALM::GORGONSCROWN)
//			{
//				++counter;
//			}
/////////////////////////////////
	
				return TRUE;
			}
	}
	return FALSE;
}

/* ======================================================================== */
static BOOL TestScrewed (REALM::REALM_TYPE thisRealm, REALM::REALM_TYPE testRealm)		// agitate, assassinate, contest, create holding, offensive magic
{
	return (RealmStatus[thisRealm][testRealm] & 0x80);
}

/* ======================================================================== */
static BOOL TestBuildup (REALM::REALM_TYPE thisRealm, REALM::REALM_TYPE testRealm)		// > 5 units in any border prov
{
	LONG	i;

	for (i=1; i<PROVINCE_COUNT; ++i)
		if (province[i].Realm == testRealm
				&& IsBorder((PROVINCE)i, thisRealm, 0, ADJACENT)
				&& UnitsHere((PROVINCE)i, testRealm)>5 )
		{
			TargetProvince = (PROVINCE)i;
			return TRUE;
		}
	return FALSE;
}

/* ======================================================================== */
static BOOL TestNoUnits (REALM::REALM_TYPE thisRealm, REALM::REALM_TYPE testRealm)
{
	LONG	i;

	for (i=1; i<PROVINCE_COUNT; ++i)
		if (province[i].Realm == testRealm
				&& IsBorder((PROVINCE)i, thisRealm, 0, ADJACENT)
				&& !UnitsHere((PROVINCE)i, testRealm) )
		{
			TargetProvince = (PROVINCE)i;
			return TRUE;
		}
	return FALSE;
}

/* ======================================================================== */
static BOOL TestEconBigger (REALM::REALM_TYPE thisRealm, REALM::REALM_TYPE testRealm)			// > ours
{
	return (realm[testRealm].mfGetIncome() > realm[thisRealm].mfGetIncome());
}

/* ======================================================================== */
static BOOL TestEconSmaller (REALM::REALM_TYPE thisRealm, REALM::REALM_TYPE testRealm)			// < ours
{
	return (realm[testRealm].mfGetIncome() < realm[thisRealm].mfGetIncome());
}

/* ======================================================================== */
static BOOL TestEconMuchSmaller (REALM::REALM_TYPE thisRealm, REALM::REALM_TYPE testRealm)		// < 70% of ours
{
	return (realm[testRealm].mfGetIncome() < ((realm[thisRealm].mfGetIncome()*7)/10) );
}

// ========================================================================
REALM::REALM_TYPE TestTriggers (USHORT Triggers)
{
	REALM::REALM_TYPE		i;
	BOOL		f = TRUE;
	BOOL		t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16;

	// check for triggers
	for (i=REALM::FIRST_REALM; i<LAND_REALM_COUNT; i = (REALM::REALM_TYPE)(i + 1))
	{
		TestTargetRealm = REALM::NO_COUNTRY;

	#if 01
		t1=t2=t3=t4=t5=t6=t7=t8=t9=t10=t11=t12=t13=t14=t15=t16=FALSE;
		if ((Triggers & TEST_INVADED)		&& !TestInvaded(CurrentRealm, i)		) goto NOT_TRGD; else t1=1;
		if ((Triggers & TEST_SCREWED)		&& !TestScrewed(CurrentRealm,i)		) goto NOT_TRGD; else t2=1;
		if ((Triggers & TEST_ADJACENT)		&& !TestAdjacent(CurrentRealm,i) ) goto NOT_TRGD; else t4=1;
		if ((Triggers & TEST_BUILDUP)		&& !TestBuildup(CurrentRealm, i)		) goto NOT_TRGD; else t3=1;
		if ((Triggers & TEST_HATED)			&& !TestHated(i)			) goto NOT_TRGD; else t5=1;
		if ((Triggers & TEST_ALLY)			&& !TestAlly(CurrentRealm, i)			) goto NOT_TRGD; else t6=1;
		if ((Triggers & TEST_NOT_ALLY)		&& TestAlly(CurrentRealm, i)			) goto NOT_TRGD; else t7=1;
		if ((Triggers & TEST_HATED_ATWAR)	&& !TestHatedAtwar(i)	) goto NOT_TRGD; else t8=1;
		if ((Triggers & TEST_ALLY_ATWAR)	&& !TestAllyAtwar(CurrentRealm, i)	) goto NOT_TRGD; else t9=1;
		if ((Triggers & TEST_NOUNITS)		&& !TestNoUnits(CurrentRealm, i)		) goto NOT_TRGD; else t10=1;
		if ((Triggers & TEST_ECONBIGGER)	&& !TestEconBigger(CurrentRealm, i)	) goto NOT_TRGD; else t11=1;
		if ((Triggers & TEST_ECONSMALLER)	&& !TestEconSmaller(CurrentRealm,i)	) goto NOT_TRGD; else t12=1;
		if ((Triggers & TEST_ECONMUCHSMALLR)&& !TestEconMuchSmaller(CurrentRealm,i)	) goto NOT_TRGD; else t13=1;
		if ((Triggers & TEST_BROKE)			&& !TestBroke(CurrentRealm)	) goto NOT_TRGD; else t14=1;
		if ((Triggers & TEST_REBELLION)		&& !TestRebellion(CurrentRealm)		) goto NOT_TRGD; else t15=1;
		if ((Triggers & TEST_OCCUPIED)		&& !TestOccupied(CurrentRealm)		) goto NOT_TRGD; else t16=1;

	#else
		if ((Triggers & TEST_INVADED)			&& !TestInvaded(i)		) continue;
		if ((Triggers & TEST_SCREWED)			&& !TestScrewed(i)		) continue;
		if ((Triggers & TEST_ADJACENT)		&& !TestAdjacent(CurrentRealm,i) ) continue;
		if ((Triggers & TEST_BUILDUP)			&& !TestBuildup(i)		) continue;
		if ((Triggers & TEST_HATED)			&& !TestHated(i)			) continue;
		if ((Triggers & TEST_ALLY)				&& !TestAlly(i)			) continue;
		if ((Triggers & TEST_NOT_ALLY)		&& TestAlly(i)			) continue;
		if ((Triggers & TEST_HATED_ATWAR)	&& !TestHatedAtwar(i)	) continue;
		if ((Triggers & TEST_ALLY_ATWAR)		&& !TestAllyAtwar(i)		) continue;
		if ((Triggers & TEST_NOUNITS)			&& !TestNoUnits(i)		) continue;
		if ((Triggers & TEST_ECONBIGGER)		&& !TestEconBigger(i)	) continue;
		if ((Triggers & TEST_ECONSMALLER)	&& !TestEconSmaller(i)	) continue;
		if ((Triggers & TEST_ECONMUCHSMALLR)&& !TestEconMuchSmaller(i)	) continue;
		if ((Triggers & TEST_BROKE)			&& !TestBroke()			) continue;
		if ((Triggers & TEST_REBELLION)		&& !TestRebellion()		) continue;
		if ((Triggers & TEST_OCCUPIED)		&& !TestOccupied()		) continue;
	#endif

		if (TestTargetRealm)		// return the third party that's now the target
			return TestTargetRealm;

		return (REALM::REALM_TYPE)i;					// return the triggering realm


NOT_TRGD:

		// ABC  Never change or remove this code. Its here to fix a compiler bug
		{
			char temp[100];
			sprintf ( temp, "NOT TRGD TR I ()");
		}

	}

	return REALM::NO_COUNTRY;						// return NO TRIGGER set
}

/* ========================================================================
   Function    - FindRealm
   Description - Used only by diplomacy. Set TargetRealm 
   Returns     - TRUE if TargetRealm is valid
   ======================================================================== */
BOOL FindRealm ()
{
 	LONG maxReaction=0;
	LONG tmp;
	REALM::REALM_TYPE realm_= REALM::NO_COUNTRY;
	LONG i;


	for (i=REALM::FIRST_REALM; i<REALM::REALM_COUNT; ++i)
	{
		if (RealmStatus[i][CurrentRealm] != STATUS_VASSAL
	   		&& i != CurrentRealm
	   		&& (i >= 35 || (i<35 && RealmStatus[CurrentRealm][i]!=STATUS_VASSAL))
				&& realm[i].mfExists()
				&& i != REALM::MONSTERS
				&& i != REALM::ANUIRE
				&& i != REALM::GORGONSCROWN
				&& i != REALM::MURKILAD
				&& i != REALM::MARKAZOR
				&& i != REALM::FIVEPEAKS
				&& i != REALM::SPIDERFELL
				&& realm[i].mfGetRegent() > 0
				&& (i < LAND_REALM_COUNT || !CheckVassal((REALM::REALM_TYPE)i)) )
		{
			tmp = RealmReaction[i][CurrentRealm];

			tmp += TestAdjacent(CurrentRealm, (REALM::REALM_TYPE) i) ? 15 : 0;

			tmp -= (RealmStatus[i][CurrentRealm] == STATUS_PERMISSIVE_ALLIANCE)?25:0;

			tmp -= (RealmStatus[i][CurrentRealm] == STATUS_FULL_ALLIANCE)?45:0;

			tmp += (i >= LAND_REALM_COUNT) ? 15 : 0;
			tmp += (i >= LAND_REALM_COUNT && date < 5) ? 25 : 0;

			tmp += random(20);

		   if (tmp > maxReaction)
			{
				maxReaction = tmp;
				realm_ = (REALM::REALM_TYPE)i;
			}
		}
	}

	if (realm_ == REALM::NO_COUNTRY)
	{
		TargetRealm = REALM::NO_COUNTRY;

		#ifdef _WINDOWS
		LOGCOMMENTSLEEP
		FLOGCOMMENT
		{
			char temp[100];
	  		sprintf(temp, "FindRealm %d - Failed", 33);
			RandomLogComment(temp);
		}
		#endif

		return FALSE;
	}
	else
	{
		TargetRealm = realm_;
		return TRUE;
	}

}

/* ========================================================================
   Function    - FindFoe
   Description - set TargetRealm 
   Returns     - TRUE if TargetRealm is valid
   ======================================================================== */
BOOL FindFoe (REALM::REALM_TYPE realm_, LONG action )
{
	LONG	i;

	for (i=1; i<LAND_REALM_COUNT; ++i)		// test for invasion
   {
		if (TestInvaded(realm_,(REALM::REALM_TYPE)i))						// stop the war, we've been invaded
		{
			TargetRealm = (REALM::REALM_TYPE)i;
			TriggeredTargetRealm = (REALM::REALM_TYPE)i;

			//---- declare war on whoever invaded us
			if (	( action == ACTION_DECLAREWAR_UNOCCUP
				 	|| action == ACTION_DECLAREWAR_WEAK
				 	|| action == ACTION_DECLAREWAR_DIFTERRAIN
				 	|| action == ACTION_DECLAREWAR_BACKSTAB)
				&& !ISATWAR(realm_,i) )
			{
				return FALSE;		// in ACTION_DECLAREWAR... FALSE means go ahead and declare war
			}

			return TRUE;			//... everywhere else we want to kick the invader
		}
   }

	for (i=1; i<LAND_REALM_COUNT; ++i)		// test for at war
   {
		if (ISATWAR(realm_,i))
		{
			TargetRealm = (REALM::REALM_TYPE)i;
			return TRUE;
		}
   }

	if ( action == ACTION_MOVE_BORDER_OWN )
	{
		TargetRealm = REALM::ANY_REALM;
		return TRUE;
	}

	return FALSE;
}


/* ======================================================================== */
#if 01
CSTRPTR szTriggerTest (USHORT tests)
{
	switch (tests)
	{
		case (TEST_ALLY+TEST_ALLY_ATWAR)									: return "ALYWAR";
		case (TEST_ALLY+TEST_ALLY_ATWAR+TEST_ADJACENT)				: return "ALYWARADJ";
		case (TEST_BUILDUP+TEST_ADJACENT)								: return "BLDADJ";
		case (TEST_BUILDUP+TEST_ADJACENT+TEST_ECONBIGGER)			: return "BLDADJBIG";
		case (TEST_BROKE)														: return "BRK";
		case (TEST_BROKE+TEST_ADJACENT+TEST_ECONSMALLER)			: return "BRKADJSML";
		case (TEST_HATED+TEST_HATED_ATWAR+TEST_ADJACENT)			: return "HATWARADJ";
		case (TEST_INVADED)													: return "INV";
		case (TEST_INVADED+TEST_ECONBIGGER)								: return "INVBIG";
		case (TEST_INVADED+TEST_ECONSMALLER)							: return "INVSML";
		case (TEST_NOT_ALLY+TEST_NOUNITS+TEST_ADJACENT)				: return "NUNADJ";
		case (TEST_NOT_ALLY+TEST_NOUNITS+TEST_ADJACENT+TEST_HATED)	: return "NUNADJHAT";
		case (TEST_NOT_ALLY+TEST_NOUNITS+TEST_ADJACENT+TEST_ECONSMALLER)	: return "NUNADJSML";
		case (TEST_OCCUPIED)													: return "OCC";
		case (TEST_SCREWED+TEST_ADJACENT)								: return "SCRADJ";
		case (TEST_SCREWED+TEST_ADJACENT+TEST_ECONBIGGER)			: return "SCRADJBIG";
		case (TEST_SCREWED+TEST_ADJACENT+TEST_ECONSMALLER)			: return "SCRADJSML";
		case (TEST_SCREWED+TEST_ECONBIGGER)								: return "SCRBIG";
		case (TEST_SCREWED)													: return "SCR";
	}
	return "none";
}
#endif

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
LONG OfferHolding (void)
{
	LONG	i,  c;
	LONG	p = 0;
	LONG	min = 999;
	LONG	regent_;
	LONG	not_icon = GUILD1_ICON;

	TargetPlace = 0;
	regent_ = realm[CurrentRealm].mfGetRegent();

	// regent is wizard, give out guild, temple, or law
	if (regents[regent_].mfGetClass1() == WIZARD || regents[regent_].mfGetClass2() == WIZARD)
		not_icon = SOURCE1_ICON;
	else if (regents[regent_].mfGetClass1() == PRIEST|| regents[regent_].mfGetClass2() == PRIEST)
		not_icon = TEMPLE1_ICON;
	else if (regents[regent_].mfGetClass1() == BARD || regents[regent_].mfGetClass2() == BARD)
		not_icon = SOURCE1_ICON;

	for (i=1; i<MAX_PLACES; ++i)
	{
		if ( places[i].Realm == CurrentRealm
			&& places[i].Icon != not_icon
			&& (places[i].Icon == SOURCE1_ICON || places[i].Icon == TEMPLE1_ICON || places[i].Icon == GUILD1_ICON) )
		{
			c = places[i].Level;

			if ( c < min)		// find min
			{
				min = c;
				p = i;
			}
		}
	}

	if (p)
	{
		TargetPlace = p;
		return HoldingValue(p);
	}

	return 0;
}

/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
LONG DemandHolding (REALM::REALM_TYPE realm_, SHORT oldDemand)
{
	LONG i, j, m, tmp, place, maxReaction;
	LONG regent_ = realm[CurrentRealm].mfGetRegent();
	LONG notProv = 0;

	if (TargetPlace)
		notProv = HoldingToProv(TargetPlace);
	maxReaction = 0;
	StartPlace = 0;
	place = 0;

	for (i = 1; i<PROVINCE_COUNT; ++i)
	{
		if (province[i].Realm != CurrentRealm)
			continue;

		CheckAndFixPlaces((PROVINCE)i);
		j = m = province[i].FirstPlace;

		while (j != 0)
		{
			tmp = 0;
			if ( m != oldDemand && i != notProv
				&& (places[m].Icon == LAW1_ICON || places[m].Icon == SOURCE1_ICON || places[m].Icon == TEMPLE1_ICON || places[m].Icon == GUILD1_ICON) )
			{
				if (places[m].Realm == realm_)
				{
					tmp += 20;
					if (places[m].Icon == LAW1_ICON)
						tmp += 20;
					tmp += random(5);
				}
	 			if (tmp > maxReaction)
				{
					maxReaction = tmp;
					place = m;
				}

				else if (tmp == maxReaction && random(3) == 0 && tmp != 0)
				{
				 	maxReaction = tmp;
					place = m;
				}
			}

			j = places[m].NextPlace;
			m = province[i].FirstPlace + j;
		}
	}


	// found no place within our realm
	if (!place)
	{
		for (i = 1; i<MAX_PLACES; ++i)
		{
			if ( ( (realm_ && places[i].Realm == realm_) || (realm_ == REALM::NO_COUNTRY && places[i].Realm != CurrentRealm) )
				 	&& places[i].Level != 0
					&& i != oldDemand
					&& (places[i].Icon == SOURCE1_ICON || places[i].Icon == TEMPLE1_ICON || places[i].Icon == GUILD1_ICON)
					&& notProv != HoldingToProv(i)
				)
			{
				tmp = RealmReaction[places[i].Realm][CurrentRealm];

				if ( (places[i].Icon == TEMPLE1_ICON
							&& (regents[regent_].mfGetClass1()==PRIEST || regents[regent_].mfGetClass2()==PRIEST) )
		   		|| (places[i].Icon == SOURCE1_ICON
							&& (regents[regent_].mfGetClass1()==WIZARD || regents[regent_].mfGetClass2()==WIZARD
								|| regents[regent_].mfGetClass1()==BARD || regents[regent_].mfGetClass2()==BARD) ) )
					tmp += 20;

				tmp += random(15);

				if (realm_ == REALM::NO_COUNTRY)
					tmp += places[i].Level * 4;

				else 	  // if action is diplomacy, asking for the non_zero place with lowest level
					tmp += (10-places[i].Level) * 3;

				if (tmp > maxReaction)
				{
					maxReaction = tmp;
					place = i;
				}
				else if (tmp == maxReaction && random(3) == 0)
				{
				 	maxReaction = tmp;
					place = i;
				}
			}
		}
	}


	if (place)
	{
		StartPlace = place;
		return HoldingValue(place);
	}

	StartPlace = 0;
	return 0;
}

#define MAX_ACTIONS_PER_MODE	100
/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
BOOL DoNPCAction (void)
{
	LONG				f, i, j, k, type, icon, min, roll, m, done;
	REALM::REALM_TYPE	rlm;
	LONG				cAttemptedRolls, dist, gold, offgold, trib, value;
	LONG				maxReaction = 0;
	LONG				tmp, place, maxloops;
	LONG				minDist = 99;
	AI_ACTION_PTR	p;
	ACTIONS_			action;
	REGENT_ENUM			CurrentRegent;

	BOOL				fSync;
	BOOL 				fSend;
	LONG				holding;
	PROVINCE			tmpProvince;
	LONG				reaction;
	static BOOL		fActionsOver = 0;  //---- action round over
	BOOL				ActionTried[MAX_ACTIONS_PER_MODE];
	BOOL				modeFromTrigger;
	BOOL				PassedOnceAlready;
	ACTION_EXPTYPE		ActionExpType  = RLM_EXP_PASS;
	LONG				ActionExpModifier = 0;

	// ========================================================================
	// 
	// ========================================================================

	if (!fActionsOver)
	{
		#ifdef _WINDOWS
		if (IsMultiPlayer())			// wait until all are done
		{
			// Player's turn. Wait until new action
			if (iCurrentRealmIndex == 0)
			{
				if (action_turn[HomeRealm] == prev_action_turn[HomeRealm]		// return until player takes his turn
					&& action_turn[HomeRealm] < 4 )
				{
					if (CurrentRealm != HomeRealm)
					{
						reqSound = SND_UI_YOUR_TURN1;
						fRedrawAll = TRUE;
						WaitingBar216ths = 0;
						WaitingWhen = get_time();
					}
					CurrentRealm = HomeRealm;

					return FALSE;
				}

				WaitingBar216ths = 0;
				WaitingWhen = 0;

				// wait for all units to reach destinations
				for (i=1; i<MAX_UNITS; ++i)
				{
					if ( units[i].Joined == FALSE
						&& units[i].Realm != REALM::NO_COUNTRY
						&& units[i].province
						&& units[i].iIconBitm != fERROR
						&& realm[units[i].Realm].mfExists()
						&& units[i].DestProvince != NO_PROVINCE )
					{
						fRedrawAll = TRUE;


						SetRemapTable(PURPLE_GRAD);
						DrawBitmap (422, 409, iDMapBx, 22, 0, 216, 21);
						ClearRemapTable();
						init_gfont(FONT_SANS_8PT);
						print_textf(422+108, 420, WHITE, "^c%s", STRMGR_GetStr(STR_GM_TROOPS_MARCHING));
						ScreenCopy( 0, 422, 409, 216, 21, SC_DEFAULT_RES);

						return FALSE;
					}
				}

				if (CurrentRealm == HomeRealm)
				{
					StartSync( iMPS_SYNC3 );
					fRedrawAll = TRUE;
				}
				CurrentRealm = REALM::NO_COUNTRY;

				#ifdef _DEBUG
				if ( GetAsyncKeyState( VK_F10 ) & 0x8000 )		// Break out
					AMultiPlayer.Finalize();
				#endif

				WaitingCount = CheckSync( iMPS_SYNC3, FALSE, FALSE );
				if ( WaitingCount)
				{
					fRedrawAll = TRUE;
					return FALSE;
				}

				// all players are done and synced
				fRedrawAll = TRUE;
				UpdateUnitAndPlaceCount();

				// Share master's version of NPC treasury and regency
				if ( AMultiPlayer.IsMaster() )
				{
					FLOGCOMMENT
					{
						char temp[100];
						sprintf ( temp, "mapai DoNPCAction - Master send all treas and reg");
						if ( fLogComment )
							RandomLogComment ( temp );
						else
							Sleep(0);
					}

					for (i=1; i<LAND_REALM_COUNT; ++i)
					{
			   	   SetGameData( MP_REALM, MPRLM_REGENCY, i,
								realm[i].mfGetRegency(), TRUE );
			     	   SetGameData( MP_REALM, MPRLM_TREASURY, i,
								realm[i].mfGetTreasury(), TRUE );
					}
					//GEH
					//BroadcastAllienceTable();
				}
				else
				{
					char temp[100];
					sprintf ( temp, "mapai DoNPCAction - non-master receive all treas and reg");
					if ( fLogComment )
						RandomLogComment ( temp );
					else
						Sleep(0);
				}

				//---- Sync after data xfer
				StartSync( iMPS_SYNC6 );
				CheckSync( iMPS_SYNC6, TRUE, TRUE );
			}
		}
		else
		#endif

		// Player's turn. Wait until new action
		if (CurrentRealm == HomeRealm && realm[HomeRealm].mfIsPlayerCtrl()
			&& action_turn[HomeRealm] == prev_action_turn[HomeRealm]
			&& action_turn[HomeRealm] < 4 )
		{
			return FALSE;
		}

	}


	//-----------------------------------------
	// make the AI select an action for an NPC
	//-----------------------------------------
	prev_action_turn[HomeRealm] = action_turn[HomeRealm];

GetNextRealm:				// if realm is invalid it loops here until a valid realm is found
	++iCurrentRealmIndex;

	// check for end of action condition
	if (iCurrentRealmIndex >= LAND_REALM_COUNT)
	{

		//---- Sync before troop movement
		#ifdef _WINDOWS
		if (!fActionsOver)
		{
			StartSync( iMPS_SYNC4 );
			CheckSync( iMPS_SYNC4, TRUE, TRUE );
			BattleSync = 0;			// clear sync counter for battles
		}
		#endif

		fActionsOver = 1; 		// The action turn is over so stop doing things

		// wait for all units to reach destinations
		#ifdef _WINDOWS
		if ( IsMultiPlayer() )
		{
			for (i=1; i<MAX_UNITS; ++i)
			{
				if ( units[i].Joined == FALSE
					&& units[i].Realm != REALM::NO_COUNTRY
					&& units[i].province
					&& units[i].iIconBitm != fERROR
					&& realm[units[i].Realm].mfExists()
					&& units[i].DestProvince != NO_PROVINCE )
				{
					iCurrentRealmIndex = LAND_REALM_COUNT - 1;		// make it loop again
					fRedrawAll = TRUE;



					SetRemapTable(BLUE_GRAD);
					DrawBitmap (422, 409, iDMapBx, 22, 0, 216, 21);
					ClearRemapTable();
					init_gfont(FONT_SANS_8PT);
					print_textf(422+108, 420, WHITE, "^c%s", STRMGR_GetStr(STR_GM_TROOPS_MARCHING));
					ScreenCopy( 0, 422, 409, 216, 21, SC_DEFAULT_RES);

					return FALSE;
				}
			}

			//---- Sync before battle
			StartSync( iMPS_SYNC1 );
			CheckSync( iMPS_SYNC1, TRUE, TRUE );
		}
		#endif

		// wait for all battles to be fought
		for (i=1; i<MAX_UNITS; ++i)
		{
			if (units[i].Joined == FALSE
				&& units[i].Realm != REALM::NO_COUNTRY
				&& units[i].province
				&& units[i].iIconBitm != fERROR
				&& realm[units[i].Realm].mfExists())
			{
				SetRemapTable(RED_GRAD);
				DrawBitmap (422, 409, iDMapBx, 22, 0, 216, 21);
				ClearRemapTable();
				init_gfont(FONT_SANS_8PT);
				print_textf(422+108, 420, WHITE, "^c%s", STRMGR_GetStr(STR_GM_TROOPS_MARCHING));
				ScreenCopy( 0, 422, 409, 216, 21, SC_DEFAULT_RES);

				tmp = CheckUnitArrival(i, FALSE);	// force stationary units to battle
		
				if (tmp & fSCENEBATTLE)		// go to Scene level battle
				{
					iCurrentRealmIndex = LAND_REALM_COUNT - 1;		// make it loop again
					ClearRemapTable();
					#ifdef _WINDOWS
					// -- The action isn't really restarted, we just restart the
					// -- action as a way to get a sync 4 to occur to sync up the 
					// -- multi player code
					fActionsOver = FALSE;						// force a sync 4
					#endif
					
					return TRUE;
				}

				if ( (tmp & fAUTOBATTLE)		// units retreated, loop to movement check
					|| units[i].DestProvince != NO_PROVINCE)
				{
					iCurrentRealmIndex = LAND_REALM_COUNT - 1;		// make it loop again
					fRedrawAll = TRUE;
					
					#ifdef _WINDOWS
					if (tmp & fNPCBATTLE)
					{
						StartSync( iMPS_SYNC4 );					// force sync 4
						CheckSync( iMPS_SYNC4, TRUE, TRUE );
					}
					#endif
					
					return FALSE;
				}
			}

		}

		//---- Battle/troop movement is now over 
		fActionsOver = 0;

		#ifdef _WINDOWS
		StartSync( iMPS_SYNC2 );	//---- Sync after battles/troop movement 
		CheckSync( iMPS_SYNC2, TRUE, TRUE );
		#endif

		iCurrentRealmIndex = 0;		// reset
		++global_action_turn;

		if ( global_action_turn >= 4 )
		{
			global_action_turn = 1;
			fRoundOver = TRUE;
			fUpdateDataArea = TRUE;			// prime the pump
			CurrentRealm = REALM::REALM_OUT_OF_RANGE;
			for(i=0; i<LAND_REALM_COUNT; i++)
				prev_action_turn[i] = 1;
			return FALSE;
		}

		#ifdef _WINDOWS
		if (IsMultiPlayer())			// wait until all are done
		{
			CurrentRealm = REALM::NO_COUNTRY;
			fRedrawAll = TRUE;
			return FALSE;
		}
		#endif
	}

	//-----------------------------------------
	// get next realm from the realm order list
	//-----------------------------------------
	rlm = (REALM::REALM_TYPE)RealmOrder[iCurrentRealmIndex];
	CurrentRealm = rlm;

	// handle known invalid conditions
	if (rlm >= LAND_REALM_COUNT || rlm < 1
			|| !realm[rlm].mfExists()
		#ifdef _WINDOWS
			|| ( IsMultiPlayer() && rlm == HomeRealm )	// skip player non-sequential turn in multiplayer
		#endif
		)
		goto GetNextRealm;

	#ifdef _WINDOWS
	// don't do player realms in multiplayer
	if ( IsMultiPlayer() && realm[rlm].mfIsPlayerCtrl())
		goto GetNextRealm;
	#endif

	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	FLOGCOMMENT
	{
		char temp[100];
		sprintf ( temp, "--------    %d #%d: start of %s turn (GB:%d R:%d)    --------",
			34,
			iCurrentRealmIndex,
			realm[rlm].mfGetName(),
			realm[rlm].mfGetTreasury(),
			realm[rlm].mfGetRegency());
		if ( fLogComment )
			RandomLogComment ( temp );
		else
			Sleep(0);
	}
	#endif

	//----- Seed the number generator 
	#ifdef _WINDOWS
	AMultiPlayer.NextSeed(__FILE__,__LINE__);
	#endif

	//-------------------------------
	// give the player his turn
	//-------------------------------
	if (realm[rlm].mfIsPlayerCtrl())		// single player
	{
		//printf("PC ACTION\n");
		i = HomeRealm;
		HomeRealm = rlm;
		ActiveRegent = realm[HomeRealm].mfGetRegent();
		if (i != HomeRealm)
		{
			UpdateHoldingFlag();
			ReconstructMap();
		}
		reqSound = SND_UI_YOUR_TURN1;
		fRedrawAll = TRUE;
		return FALSE;
	}

	npc_ai[rlm].CurrentAction = ACTION_PASS;
	ActionExpType = RLM_EXP_PASS;
	ActionExpModifier = 0;
	modeFromTrigger = FALSE;
	PassedOnceAlready = FALSE;
	for (i=0; i< MAX_ACTIONS_PER_MODE;++i)
		ActionTried[i] = FALSE;

	//-------------------------------------------
	// mode change condition
	//-------------------------------------------
	if (npc_ai[rlm].CurrentMode >= MODE_COUNT || random(100)<npc_ai[rlm].Fickleness)
	{
NewMode:
		//npc_ai[rlm].CurrentMode = npc_ai[rlm].Modes[random(10)];

		for (j=i=0; i<10; ++i)
			j += (LONG)npc_ai[rlm].Chance[i];
		k = random(j);

		//printf("MODE CHANGE - Realm:%s chance_tot:%d choice:%d\n",realm[rlm].name,j,k);
		for (i=0; i<10; ++i)
		{
			k -= npc_ai[rlm].Chance[i];
			if (k < 0)
			{
				npc_ai[rlm].CurrentMode = npc_ai[rlm].Modes[i];
				for (i=0; i< MAX_ACTIONS_PER_MODE;++i)			// Need to reset for being call via a goto
					ActionTried[i] = FALSE;
			

				#ifdef _WINDOWS
	       	LOGCOMMENTSLEEP
				FLOGCOMMENT
				{
					char temp[100];
					sprintf ( temp, "DoNPCAction %d - ModeChange: %s changing to %s", 
						35,
						realm[rlm].mfGetName(), 
						szAIMode[npc_ai[rlm].CurrentMode]);
					if ( fLogComment )
						RandomLogComment ( temp );
					else
						Sleep(0);
				}
				#endif

				break;
			}
		}
		//printf("NPC ACTION - %s has changed modes to %s (%d)\n",realm[rlm].mfGetName(),szAIMode[npc_ai[rlm].CurrentMode],npc_ai[rlm].CurrentMode);
	}

	//-------------------------------------------
	// trigger conditions
	//-------------------------------------------
	TriggeredTargetRealm = REALM::NO_COUNTRY;
	if (modeFromTrigger == FALSE)	// so we don't repeat the test if we've failed.
	{
		for (i=0; 
			 i<8 && TriggeredTargetRealm == REALM::NO_COUNTRY;
			 ++i)
		{
			TriggeredTargetRealm = TestTriggers(npc_ai[rlm].Triggers[i].Tests);
			if (TriggeredTargetRealm != REALM::NO_COUNTRY)
			{
				//printf("TRIGGER - %s trigger %d (%s) set off by %s, mode now %s\n",
				//realm[rlm].mfGetName(),i,szTriggerTest(npc_ai[rlm].Triggers[i].Tests),
				//realm[TriggeredTargetRealm].mfGetName(),szAIMode[npc_ai[rlm].Triggers[i].Action]);
	
				LONG ModeChance = random(100);
				if (ModeChance < npc_ai[rlm].Triggers[i].Chance 
				    || i == 7)	// Last triggered mode possible.
				{
					npc_ai[rlm].CurrentMode = npc_ai[rlm].Triggers[i].Action;
					modeFromTrigger = TRUE;
				}
				else
				{
					TriggeredTargetRealm = REALM::NO_COUNTRY;
				}
	
				#ifdef _WINDOWS
		     	LOGCOMMENTSLEEP
				FLOGCOMMENT
				{
					char temp[100];
					char temp1[100];
					char temp2[100];
					strcpy(temp1, realm[rlm].mfGetName());
					strcpy(temp2, realm[TriggeredTargetRealm].mfGetName());
					sprintf ( temp, "DoNPCAction %d - %s  triggered action:%s  target realm:%s",
						36,
						temp1,
						szAIMode[npc_ai[rlm].CurrentMode],
						temp2);
					if ( fLogComment )
						RandomLogComment ( temp );
					else
						Sleep(0);
				}
				#endif
	
			}
		}
	}

	//-------------------------------------------
	// roll an action
	//-------------------------------------------
	cAttemptedRolls = 0;

ReRoll:

	fSend = FALSE;                          //---- Reset flags
	fSync = FALSE;

	++cAttemptedRolls;
	if (cAttemptedRolls>25)					// only try 25 times then change modes
		goto NewMode;

Another:
	run_timers();  // cdb 11/27	// ABC don't put in inner loops

#ifdef _WINDOWS
	AMultiPlayer.LookForMessages();   //---- Update non specific messages
#endif

	p = Modes[npc_ai[rlm].CurrentMode];

	do
	{
		roll = random(100);
		for (k = 0; roll > p[k].Chance; ++k) {}
	} while (ActionTried[k] == TRUE);
	action = (ACTIONS_) p[k].Action;

	// Because this thing is so confusing let us write out what we are
	// doing. This way we can figure out why this thing is out of sync
	// during a multiplayer game. 

	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	FLOGCOMMENT
	{
		char temp[100];
		char temp2[100];
		char temp3[100];
		char temp4[100];
		if (TriggeredTargetRealm != REALM::NO_COUNTRY 
			&& TriggeredTargetRealm!=rlm
			&& TriggeredTargetRealm < LAND_REALM_COUNT)
		{
			strcpy(temp2,realm[rlm].mfGetName());
			strcpy(temp3,szAIAction[action].mfGetStr() );
			strcpy(temp4,realm[TriggeredTargetRealm].mfGetName());
			
			sprintf ( temp, "DoNPCAction %d - %s  doing action:%ld %s (maybe on %s)", 
				37, temp2, action, temp3, temp4);
		}
		else
		{
			strcpy(temp2, realm[rlm].mfGetName());
			strcpy(temp3, szAIAction[action].mfGetStr());
			sprintf ( temp, "DoNPCAction %d - %s  doing action:%ld %s", 
				37, temp2, action, temp3);
		}
		if ( fLogComment )
			RandomLogComment ( temp );
		else
			Sleep(0);
	}
	#endif

	//-------------------------------------------
	// do an action
	//-------------------------------------------
	if (!fTutorialSelected || date > 7)		// in tutorial, no actions until turn 7
	{
		if (!fAllowed[fControlMode][action])
			{logreroll(__LINE__); goto ReRoll_;}				// can't do the action so try another

		switch (action)
		{
		case ACTION_NONE:
			ActionExpType = RLM_EXP_PASS;
			ActionExpModifier = 0;
			{logreroll(__LINE__); goto ReRoll_;}				// can't do the action so try another

		case ACTION_AGITATE_FOE:
			if (rlm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
			if(!CanAfford (1, 1, 0, rlm)) {logreroll(__LINE__); goto ReRoll_;}
			if (TriggeredTargetRealm != REALM::NO_COUNTRY && TriggeredTargetRealm!=rlm
					&& TriggeredTargetRealm < LAND_REALM_COUNT)
				TargetRealm = TriggeredTargetRealm;
			else if (!FindFoe(rlm,action)) 
				{logreroll(__LINE__); goto ReRoll_;}
			// have a foe in TargetRealm
			// if there isn't a province whose loyalty is poor, 
			// find one whose loyalty is average.
			// timeCount == 0, the first run, timeCount == 1, the second run
			for( timeCount = 0; timeCount < 2; ++timeCount )					  
				if (FindProv(action)) break;
			if(timeCount == 2) {logreroll(__LINE__); goto ReRoll_;}
			TargetPlace=0;

			npc_ai[rlm].CurrentAction = action;
			if (!RollNPCSuccess(action, &fSync, &fSend )) break;

			province[TargetProvince].Loyalty--;
			SetScrewed(TargetRealm, CurrentRealm, fSend );
 			if(TargetRealm == HomeRealm)
				AddReport(SUBJ_AGITATE, 1, TargetProvince, province[TargetProvince].Loyalty);
			SetGameData(MP_PROVINCE, MPPROV_LOYALTY, TargetProvince, province[TargetProvince].Loyalty, fSend );
			if(!CanAfford (1, 1, 0, rlm)) break;
			PayForIt(rlm,1, 1, 0);
			ActionExpType = RLM_EXP_AGITATE;
			ActionExpModifier = 0;
			break;

		case ACTION_AGITATE_OWN:
			if (rlm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
			if(!CanAfford (1, 1, 0, rlm)) {logreroll(__LINE__); goto ReRoll_;}
			for ( timeCount = 0; timeCount < 2; ++timeCount )
				if (FindProv(action)) break;
			if(timeCount == 2) {logreroll(__LINE__); goto ReRoll_;}
			npc_ai[rlm].CurrentAction = action;
			//if(!RollNPCSuccess(action)) break;
			++province[TargetProvince].Loyalty;
			SetGameData(MP_PROVINCE, MPPROV_LOYALTY, TargetProvince, province[TargetProvince].Loyalty, FALSE );
			PayForIt(rlm, 1, 1, 0);
			CurrentRegent = (REGENT_ENUM) realm[CurrentRealm].mfGetRegent();
			if (CurrentRegent != NO_REGENT)
			{
				SHORT const hPlayerStats = LoadStats(regents[CurrentRegent].mfGetid(), fERROR);
				if (hPlayerStats != fERROR)
				{
					DumbAutoLockPtr <PLAYER_STATS const> const pPS(hPlayerStats);
					if (!npcActionData[rlm].mfFreeAgitateUsed() && pPS->mfIsType(PLAYER_CLASS_INFO::PRIEST))
					{
						npcActionData[rlm].mfSetAgitateUsed();
						goto Another;
					}
				}
			}
			ActionExpType = RLM_EXP_AGITATE;
			ActionExpModifier = 0;
			break;

		case ACTION_BUILDROAD:
			if (rlm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
			//printf("BUILD ROAD - start");
			if (!FindHolding(rlm,rlm,ANY_PROVINCE,GUILD1_ICON, TRUE)) {logreroll(__LINE__); goto ReRoll_;} // if no guilds don't bother
			//printf(" found:%s  end",province[TargetProvince].name);
			StartProvince = TargetProvince;
			if (!FindDiffTerrain(ANY_WAY, StartProvince, &TargetProvince)) {logreroll(__LINE__); goto ReRoll_;}	// must be to diff terrain
			//printf(" found:%s ",province[TargetProvince].name);
			if (!RecursiveRoadBuilder(StartProvince)) {logreroll(__LINE__); goto ReRoll_;}
			npc_ai[rlm].CurrentAction = action;
			ActionExpType = RLM_EXP_BUILD_ROAD;
			ActionExpModifier = 0;
			goto Another_;				// action was free so do another

		case ACTION_CONTEST_LAW:
		case ACTION_CONTEST_GLD:
		case ACTION_CONTEST_TMP:
		case ACTION_CONTEST_SRC:
			switch(action)
			{
			case ACTION_CONTEST_LAW:
				ActionExpType = RLM_EXP_CONTEST_LAW;
				ActionExpModifier = 0;
				j = 0;
				break;
			case ACTION_CONTEST_GLD:
				ActionExpType = RLM_EXP_CONTEST_GUILD;
				ActionExpModifier = 0;
				j = 1;
				break;
			case ACTION_CONTEST_TMP:
				ActionExpType = RLM_EXP_CONTEST_TEMPLE;
				ActionExpModifier = 0;
				j = 2;
				break;
			case ACTION_CONTEST_SRC:
				ActionExpType = RLM_EXP_CONTEST_SOURCE;
				ActionExpModifier = 0;
				j = 3;
				break;
			}

			if (rlm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
			if (TriggeredTargetRealm)
			{
				if (FindHolding(rlm, TriggeredTargetRealm, ANY_PROVINCE, (MAP_ICON)InfoReqType[j], TRUE))
					goto FOUND_ONE;
				if (FindHolding(REALM::ANY_REALM, TriggeredTargetRealm, ANY_PROVINCE, (MAP_ICON)InfoReqType[j], TRUE)
						&& FindHolding(REALM::ANY_REALM, rlm, TargetProvince, ANY_MAP_ICON, TRUE) )
							goto FOUND_ONE;
			}
			if (FindHolding(rlm, REALM::ANY_REALM, ANY_PROVINCE, (MAP_ICON)InfoReqType[j], TRUE))
			{
				TargetRealm = (REALM::REALM_TYPE) places[TargetPlace].Realm;
				goto FOUND_ONE;
			}					
			{logreroll(__LINE__); goto ReRoll_;}				// can't do the action so try another

FOUND_ONE:
			npc_ai[rlm].CurrentAction = action;
			if (!RollNPCSuccess(action,&fSync,&fSend)) break;
	 		if (places[TargetPlace].Level == 0
				   || places[TargetPlace].Contested == TRUE)
				DeleteHolding(TargetProvince, TargetPlace, fSend);
			else
				//places[TargetPlace].Contested = TRUE;
				SetGameData(MP_PLACES, MPPLACE_CONTESTED, TargetPlace, TRUE, fSend );
			break;

		case ACTION_CREATE_LAW:
			icon = LAW1_ICON; type = 0;
			ActionExpType = RLM_EXP_CREATE_LAW;
			ActionExpModifier = 0;
			goto CreateHldng;
		case ACTION_CREATE_GLD:
			icon = GUILD1_ICON; type = 1;
			ActionExpType = RLM_EXP_CREATE_GUILD;
			ActionExpModifier = 0;
			goto CreateHldng;
		case ACTION_CREATE_TMP:
			icon = TEMPLE1_ICON; type = 2;
			ActionExpType = RLM_EXP_CREATE_TEMPLE;
			ActionExpModifier = 0;
			goto CreateHldng;
		case ACTION_CREATE_SRC:
			icon = SOURCE1_ICON; type = 3;
			ActionExpType = RLM_EXP_CREATE_SOURCE;
			ActionExpModifier = 0;
CreateHldng:
			if (rlm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//			if (TriggeredTargetRealm)
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			if (cntHolds > (MAX_PLACES-60)) {logreroll(__LINE__); goto ReRoll_;}
			
			
			if (game_difficulty == DIF_EASY)
			{
				if (!FindNotHolding(rlm, rlm, icon, &TargetProvince)) {logreroll(__LINE__); goto ReRoll_;}
			}
			else
			{
				BOOL FoundOneToBuild = FALSE;
				LONG k;
				
				for (k = REALM::FIRST_REALM; k < LAND_REALM_COUNT && !FoundOneToBuild; ++k)
				{
					if (rlm != k 
						&& ISATWAR((REALM::REALM_TYPE) k, rlm))
					{
						if (FindNotHolding(rlm, (REALM::REALM_TYPE)k, icon, &TargetProvince)) {logreroll(__LINE__); goto ReRoll_;}
						{
							FoundOneToBuild = TRUE;
						}
					}
				}
				// No one at war with.
				if (!FoundOneToBuild)
				{
					if (!FindNotHolding(rlm, rlm, icon, &TargetProvince)) {logreroll(__LINE__); goto ReRoll_;}
				}
			}
			TargetPlace=0;
			npc_ai[rlm].CurrentAction = action;
			if (!RollNPCSuccess(action,&fSync, &fSend)) break;
			//--- We can't goto ReRoll after RollNPCSuccess
			//if (!CreateHolding(type, 0, 0, TargetProvince, rlm, fSend ) ) {logreroll(__LINE__); goto ReRoll_;}
			CreateHolding(type, 0, 0, TargetProvince, rlm, fSend);
			break;

		case ACTION_DECLAREWAR_UNOCCUP:
		case ACTION_DECLAREWAR_WEAK:
		case ACTION_DECLAREWAR_DIFTERRAIN:
		case ACTION_DECLAREWAR_BACKSTAB:
			if (rlm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
			//if (action_turn > 2			// don't declare war after 2nd action
			//	||	(fPlayerNeverDeclaredWar
			//		&&	(	(game_difficulty == DIF_EASY && date < 4)
			//			||	(game_difficulty == DIF_MEDIUM && date < 3)
			//			||	(game_difficulty == DIF_HARD && date < 2)
			//	)	)	)
			//	{logreroll(__LINE__); goto ReRoll_;}
			
			ActionExpType = RLM_EXP_DECLARE_WAR;
			ActionExpModifier = 0;
			#ifdef _WINDOWS
			LOGCOMMENTSLEEP
			FLOGCOMMENT
			{
				char temp[200];
				sprintf ( temp, "Declare War - action_turn:%d  fPlayerNeverDelaredWar:%ld game_difficulty:%ld", 
					action_turn[HomeRealm], fPlayerNeverDeclaredWar, game_difficulty );
				if ( fLogComment )
					RandomLogComment ( temp );
				else
					Sleep(0);
			}
			#endif

			#ifdef _WINDOWS
			if ( IsMultiPlayer() )
			{
				if (action_turn[HomeRealm] > 3) {logreroll(__LINE__); goto ReRoll_;}
			}
			else
			{
				if (action_turn[HomeRealm] > 2) {logreroll(__LINE__); goto ReRoll_;}
			}
			#else
			if (action_turn[HomeRealm] > 2) {logreroll(__LINE__); goto ReRoll_;}
			#endif

			if (fPlayerNeverDeclaredWar && (game_difficulty == DIF_EASY && date < 4)) {logreroll(__LINE__); goto ReRoll_;}
			if (fPlayerNeverDeclaredWar && (game_difficulty == DIF_MEDIUM && date < 3)) {logreroll(__LINE__); goto ReRoll_;}
			if (fPlayerNeverDeclaredWar && (game_difficulty == DIF_HARD && date < 2)) {logreroll(__LINE__); goto ReRoll_;}
			
			if (FindFoe(rlm,action)) {logreroll(__LINE__); goto ReRoll_;}	// if already at war don't do again

			// triggered action
			if (TriggeredTargetRealm != REALM::NO_COUNTRY && TriggeredTargetRealm!=rlm
					&& TriggeredTargetRealm < LAND_REALM_COUNT)
			{
				npc_ai[rlm].CurrentAction = action;

				WarDeclared(rlm, TriggeredTargetRealm, FALSE);

				npc_ai[rlm].OldAtWar = TargetRealm;
				break;
			}

			// check for previous at war status
			if (npc_ai[rlm].OldAtWar && realm[npc_ai[rlm].OldAtWar].mfExists())
			{
				min = 39;
				TargetRealm = (REALM::REALM_TYPE) npc_ai[rlm].OldAtWar;		// attack them again
			}

			// search for someone to attack
			else
			{
				min = 9999;
				TargetRealm = REALM::NO_COUNTRY;
				for (i=1; i<LAND_REALM_COUNT; ++i)	// find minimum reaction
				{
					// not self, imp. city of anuire, or full (or better) allies
					if (i==rlm || i==REALM::ANUIRE || AREFULLALLIES(rlm,i))
						continue;

					for (f=0,j=1; j<PROVINCE_COUNT && !f; ++j)	// find adj realm
						if (province[j].Realm==rlm || (rlm==REALM::GORGONSCROWN && AREALLIED(rlm,province[j].Realm)))
							for (k=0; k<7 && !f; ++k)
								if (province[province[j].Border[k].Province].Realm==i)
								{
									roll = RealmReaction[rlm][i]+random(40)-20	// ñ20%
											- (ISATWAR(i,rlm))?50:0				// 50% more if they declared war on us
											+ (AREALLIED(i,rlm))?20:0 ;			// 20% less if they are permissive allies
									if (roll < min)
									{
										min = roll;
										TargetRealm = (REALM::REALM_TYPE)i;
										f = TRUE;
									}
								}
				}
			}

			if (TargetRealm != REALM::NO_COUNTRY && TargetRealm != rlm && min <= 40
				&& TargetRealm < LAND_REALM_COUNT)		// GWP PATCH CODE.
			{
				npc_ai[rlm].CurrentAction = action;
				
				WarDeclared(rlm, TargetRealm, FALSE);
				break;
			}
			{logreroll(__LINE__); goto ReRoll_;}				// can't do the action so try another

		case ACTION_DIPLMCY_ALLY:
			#ifdef _WINDOWS
			if (IsMultiPlayer()) {logreroll(__LINE__); goto ReRoll_;}
			#endif
			if (rlm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}

			if (!FindRealm()) {logreroll(__LINE__); goto ReRoll_;}
			TargetPlace = 0;
			StartPlace = 0;
			TargetProvince = NO_PROVINCE;
			gold = 0;
			offgold = 0;
			trib = 0;

			if (TargetRealm == REALM::ANY_REALM) {logreroll(__LINE__); goto ReRoll_;}
			if (!CanAfford (1, 1, 0, CurrentRealm)) {logreroll(__LINE__); goto ReRoll_;}

			// scan realm for already being a vassal
			if (RealmStatus[TargetRealm][CurrentRealm] == STATUS_FULL_ALLIANCE)
				for (j=REALM::FIRST_REALM; j<LAND_REALM_COUNT; ++j)
					if (ISVASSAL(TargetRealm,j))
						{logreroll(__LINE__); goto ReRoll_;}

			// going for Full or Permissive alliance
			if (RealmStatus[TargetRealm][CurrentRealm] == STATUS_PERMISSIVE_ALLIANCE
				|| RealmStatus[TargetRealm][CurrentRealm] == STATUS_NEUTRAL)
			{
				value = random(25) - 10;						// their perceived value of the alliance

				if (random(3)==0 || value > 0)			// 33% or if needed
				{
					j = OfferHolding();						// TargetPlace is the place
					value -= j;
				}
				if (random(3)==0 || value < 0)			// 33% or if needed
				{
					j = DemandHolding(TargetRealm, 0);	// startPlace is the place we will demand
					value += j;
				}
				if (value < 0)									// make up what's left
				{
					gold = -value;
					if (!CanAfford (gold, 0, 0, TargetRealm))
						gold = realm[TargetRealm].mfGetTreasury()/10-1;
					if (gold < 0)
						gold = 0;
					value += gold;
				}
				else if (value > 0)							// make up what's left
				{
					offgold = value;
					if (!CanAfford(offgold, 0, 0, rlm))
						offgold = realm[rlm].mfGetTreasury()/10-1;
					if (offgold < 0)
						offgold = 0;
					value -= offgold;
				}

				// GWP I can't tell wheter its permissive or Full.
				ActionExpType = RLM_EXP_DIPLOMACY_PERMISSIVE;
				ActionExpModifier = 0;
				//if (value < 10) {logreroll(__LINE__); goto ReRoll_;}				// not good enough
			}

			// going for Vassalage
			else if (RealmStatus[TargetRealm][CurrentRealm] == STATUS_FULL_ALLIANCE)
			{
				value = random(25) + 10;					// their perceived value of the alliance

				if (random(3)==0 || value > 0)			// 33% or if needed
				{
					j = OfferHolding();
					value -= j;
				}
				if (random(3)==0 || value < 0)			// 33% or if needed
				{
					j = DemandHolding(TargetRealm, 0);
					value += j;
				}
				if (value < 0)									// make up what's left
				{
					gold = -value;
					if (!CanAfford (gold, 0, 0, TargetRealm))
						gold = realm[TargetRealm].mfGetTreasury()/10-1;
					if (gold < 0)
						gold = 0;
					value += gold;
				}
				else if (value > 0)							// make up what's left
				{
					offgold = 0;
					trib = value / 3;
					if (!CanAfford(0, 0, trib, rlm))
					{
						trib = 0;
						offgold = value;
						if (!CanAfford(offgold, 0, 0, rlm))
							offgold = realm[rlm].mfGetTreasury()/10-1;
						if (offgold < 0)
							offgold = 0;
						value -= offgold;
					}
					else
						value = 0;
				}

				ActionExpType = RLM_EXP_DIPLOMACY_VASSALAGE;
				ActionExpModifier = 0;
				//if (value < 10) {logreroll(__LINE__); goto ReRoll_;}				// not good enough
			}

			npc_ai[rlm].CurrentAction = action;


			if (!realm[TargetRealm].mfIsPlayerCtrl())  //---- NPC vs. NPC
			{
				// Roll For Success
				if (random(4)==0) break;

				// do the action

				if (RealmStatus[TargetRealm][CurrentRealm] == STATUS_PERMISSIVE_ALLIANCE)
				{
					PayForIt(CurrentRealm, 1, 1, 0);
					SetGameData(MP_REALMSTATUS, TargetRealm, CurrentRealm, STATUS_FULL_ALLIANCE, FALSE );
					if (TargetRealm<LAND_REALM_COUNT)
					{
						//RealmStatus[CurrentRealm][TargetRealm] = STATUS_FULL_ALLIANCE;
						SetGameData(MP_REALMSTATUS, CurrentRealm, TargetRealm, STATUS_FULL_ALLIANCE, FALSE );
						//RealmReaction[CurrentRealm] [TargetRealm]= 95;
						SetGameData(MP_REALMREACTION, CurrentRealm, TargetRealm, 95, FALSE );
					}
					// RealmReaction[TargetRealm][CurrentRealm] = 95;
					SetGameData(MP_REALMREACTION, TargetRealm, CurrentRealm, 95, FALSE);

					realm[CurrentRealm].mfAddTreasury(gold * 10);
					realm[TargetRealm].mfAddTreasury( -(gold * 10));
					// offer holding
					if (TargetPlace)
						//SetGameData(MP_PLACES, MPPLACE_REALM, TargetPlace, TargetRealm, FALSE );
						CheckHolding(TargetPlace, TargetRealm, FALSE);
					if (StartPlace)
						//SetGameData(MP_PLACES, MPPLACE_REALM, StartPlace, CurrentRealm, FALSE );
						CheckHolding(StartPlace, CurrentRealm, FALSE);
				}
				else if (RealmStatus[TargetRealm][CurrentRealm] == STATUS_FULL_ALLIANCE)
				{
					PayForIt(CurrentRealm, gold, 1, 0);
					realm[TargetRealm].mfAddTreasury( gold* 10);
					// offer tribute
					Tribute[TargetRealm][CurrentRealm] = trib;

					// reset realm status and reaction
					SetGameData(MP_REALMSTATUS, TargetRealm, CurrentRealm, STATUS_VASSAL, FALSE );
					SetGameData(MP_REALMREACTION, TargetRealm, CurrentRealm, 150, FALSE );
					if (TargetRealm<LAND_REALM_COUNT)
					{
						SetGameData(MP_REALMSTATUS, CurrentRealm, TargetRealm, STATUS_FULL_ALLIANCE, FALSE );
						SetGameData(MP_REALMREACTION, CurrentRealm, TargetRealm, 95, FALSE );
						// offer province
						//GEH 11/20 SetGameData(MP_PROVINCE, MPPROV_REALM, StartProvince, TargetRealm, FALSE );
						//GEH 11/20 ReconstructMap();
					}
					else		// non-landed vassal becomes lieutenant instead
					{
						TargetProvince = (PROVINCE)units[regents[realm[CurrentRealm].mfGetRegent()].mfGetunit()].province;
						for (j=69; j<CHARACTER_COUNT; ++j)			// scan all regents
						{
							if (regents[j].mfGetRealm() == TargetRealm)
							{
								TargetPlace = j;
							 	LONG const iUnit = CreateUnit(TargetProvince, regents[TargetPlace].mfGeticon(), (LONG) regents[TargetPlace].mfGetid(), CurrentRealm, FALSE);
								if (iUnit != fERROR)
								{
									if (regents[TargetPlace].mfGetRealm() >= LAND_REALM_COUNT
									   	|| regents[TargetPlace].mfGetRealm() == HomeRealm)
										for (k=0; k<MAX_PLACES; ++k)
										{
											if(places[k].Realm == regents[TargetPlace].mfGetRealm())
											{
												CheckHolding(k, CurrentRealm, FALSE);
											}
										}
									SetGameData(MP_REGENT, MPREG_UNIT, TargetPlace, iUnit, FALSE );
									// HACK HACK HACK HACK HACK
									// GWP Don't set this anymore. (It's read from the units array.
									// SetGameData(MP_REGENT, MPREG_REALM, TargetPlace, CurrentRealm, FALSE );
								}
								// even if we can't create a unit we have to stop.
								break;
							}
						}
					}
				}
				else
				{
					PayForIt(CurrentRealm, 1, 1, 0);
					SetGameData(MP_REALMSTATUS, TargetRealm, CurrentRealm, STATUS_PERMISSIVE_ALLIANCE, FALSE );

					if (TargetRealm<LAND_REALM_COUNT)
					{
						//RealmStatus[CurrentRealm][TargetRealm] = STATUS_PERMISSIVE_ALLIANCE;
						SetGameData(MP_REALMSTATUS, CurrentRealm, TargetRealm, STATUS_PERMISSIVE_ALLIANCE, FALSE );
						// RealmReaction[CurrentRealm] [TargetRealm]= 75;
						SetGameData(MP_REALMREACTION, CurrentRealm, TargetRealm, 75, FALSE );
					}

					//RealmReaction[TargetRealm][CurrentRealm] = 75;
					SetGameData(MP_REALMREACTION, TargetRealm, CurrentRealm, 75, FALSE );

					realm[CurrentRealm].mfAddTreasury(gold * 10);
					realm[TargetRealm].mfAddTreasury( -(gold * 10));

					// offer holding
					if (TargetPlace)
						//SetGameData(MP_PLACES, MPPLACE_REALM, TargetPlace, TargetRealm, FALSE );
						CheckHolding(TargetPlace, TargetRealm, FALSE);

					if (StartPlace)
						//SetGameData(MP_PLACES, MPPLACE_REALM, StartPlace, CurrentRealm, FALSE );
						CheckHolding(StartPlace, CurrentRealm, FALSE);
				}
			}

			else if ( TargetRealm == HomeRealm ) //---- Home realm is target
			{
				#ifdef _WINDOWS
				if ( IsMultiPlayer() )
				{
					fOtherDipMP = TRUE;  //---- signal we need to wait
				}
				#endif

				if (RealmStatus[TargetRealm][CurrentRealm] == STATUS_PERMISSIVE_ALLIANCE)
				{
					dData.d1.iAlliance = 2;
					dData.d1.iOffTrib  = 0;
				}
				else if (RealmStatus[TargetRealm][CurrentRealm] == STATUS_FULL_ALLIANCE)
				{
					dData.d1.iAlliance = 3;
					dData.d1.iOffTrib  = trib;
				}
				else
				{
					dData.d1.iAlliance = 1;
					dData.d1.iOffTrib  = 0;
				}

				dData.d1.iReqHold  = StartPlace;
				dData.d1.iOffHold  = TargetPlace;
				dData.d1.iReqGold  = gold;
				dData.d1.iOffGold  = offgold;

				dData.d1.iWhoFrom	= CurrentRealm;
				dData.d1.iReqProv	= 0;
				dData.d1.iOffProv = 0;
				dData.d1.iReqTrib	= 0;
				dData.d1.iOffRegy	= 1;

				if (curResponse < 8)
					++curResponse;
				response[curResponse].lWhichAction	= DIPLOMACY;
				response[curResponse].lWhichProvince= dData.d2.data1;
				response[curResponse].lWhichHold		= dData.d2.data2;
				response[curResponse].lSuccess	 	= dData.d2.data3;
				response[curResponse].lFromId			= 0;
				response[curResponse].lFromRealm		= 0;
				response[curResponse].lResult			= 0;

				ShowRequest(D_OTHERDIPLOMACY, curResponse);	// do Diplomacy style dialog
			}
			else   //----- Need to wait for remote player
			{
				fSync = TRUE;
			}
			break;

		case ACTION_DIPLMCY_DMD_PROV:
			ActionExpType = RLM_EXP_PASS;
			ActionExpModifier = 0;
			#ifdef _WINDOWS
			if (IsMultiPlayer()) {logreroll(__LINE__); goto ReRoll_;}
			#endif
			if (rlm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
			if(CurrentRealm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
			TargetPlace = 0;
			TargetProvince = NO_PROVINCE;
			TargetRealm = REALM::NO_COUNTRY;
			StartPlace = 0;
			
			if(!FindProv(action)) {logreroll(__LINE__); goto ReRoll_;}
			TargetRealm = (REALM::REALM_TYPE)province[TargetProvince].Realm;

			tmpProvince = TargetProvince; //OfferHolding will change targetprovince to the province of the holding found
			// GEH if(!OfferHolding()) {logreroll(__LINE__); goto ReRoll_;}

			if(50 > (RealmReaction[TargetRealm][CurrentRealm]-70+35+15))
				 reaction = 120-50-RealmReaction[TargetRealm][CurrentRealm];
			else 
				reaction = 1;
			if(!CanAfford (reaction/3, 1, 0, rlm)) {logreroll(__LINE__); goto ReRoll_;}
			if(!random(3)) {logreroll(__LINE__); goto ReRoll_;}

			npc_ai[rlm].CurrentAction = action;

			if (!realm[TargetRealm].mfIsPlayerCtrl())	 // NPC vs. NPC
			{
				// take care the demanded province
				CheckAndFixPlaces(tmpProvince);
				i = j = province[tmpProvince].FirstPlace;
				while(i!=0)
				{
					if(places[j].Icon == CASTLE1_ICON && places[j].Realm == province[tmpProvince].Realm)
						places[j].Realm = CurrentRealm;
					i = places[j].NextPlace;
					j = province[tmpProvince].FirstPlace + i;
				}
				province[tmpProvince].Realm = CurrentRealm;


				// take care of the offered holding
				//GEH places[TargetPlace].Realm = TargetRealm;
				//GEH SetGameData(MP_PLACES, MPPLACE_REALM, TargetPlace, TargetRealm, FALSE );
				//GEH CheckHolding(TargetPlace, TargetRealm, FALSE);

				ReconstructMap();
				CheckRealmStatus(rlm,TargetRealm, FALSE );
				Tribute[CurrentRealm][TargetRealm]=reaction/5;
				gold = (reaction-(reaction/5))/3;
				gold = (gold >= 1)?gold:1;
				PayForIt(CurrentRealm, gold, 1, 0);
				realm[TargetRealm].mfAddTreasury(gold* 10);
			}

			else if ( TargetRealm == HomeRealm )   //---- Target is us 
			{

				#ifdef _WINDOWS
				if ( IsMultiPlayer() )
				{
					fOtherDipMP = TRUE;  //---- signal we need to wait
				}
				#endif

				gold = (reaction-reaction/5)/3;
				gold = (gold >= 1)?gold:1;

				dData.d1.iAlliance  = 0;
				dData.d1.iOffGold	= gold;
				dData.d1.iWhoFrom	= CurrentRealm;
				dData.d1.iReqGold	= 0;
				dData.d1.iReqProv	= tmpProvince;
				dData.d1.iReqHold	= 0;
				dData.d1.iReqTrib	= 0;
				dData.d1.iOffProv	= 0;
				dData.d1.iOffHold	= TargetPlace;
				dData.d1.iOffTrib	= reaction/5;
				dData.d1.iOffRegy	= 1;

				if (curResponse < 8)
					++curResponse;
				response[curResponse].lWhichAction	= DIPLOMACY;
				response[curResponse].lWhichProvince= dData.d2.data1;
				response[curResponse].lWhichHold		= dData.d2.data2;
				response[curResponse].lSuccess	 	= dData.d2.data3;
				response[curResponse].lFromId			= 0;
				response[curResponse].lFromRealm		= 0;
				response[curResponse].lResult			= 0;

				ShowRequest(D_OTHERDIPLOMACY, curResponse);	// do Diplomacy style dialog
			}
			else   //----- Need to wait for remote player
			{
				fSync = TRUE;
			}
			
			break;


		case ACTION_DIPLMCY_DMD_HOLD:
			ActionExpType = RLM_EXP_PASS;
			ActionExpModifier = 0;
			#ifdef _WINDOWS
			if (IsMultiPlayer()) {logreroll(__LINE__); goto ReRoll_;}
			#endif
			if (rlm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
			if(CurrentRealm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
			TargetPlace = 0;
			StartPlace = 0;
			
			CurrentRegent = (REGENT_ENUM) realm[CurrentRealm].mfGetRegent();
			
			if(!OfferHolding()) {logreroll(__LINE__); goto ReRoll_;}
			
			if(!DemandHolding(REALM::NO_COUNTRY, 0)) {logreroll(__LINE__); goto ReRoll_;}

			TargetRealm = (REALM::REALM_TYPE)places[StartPlace].Realm;
			if(50 > (RealmReaction[TargetRealm][CurrentRealm]-30+15))
				reaction = 50+30-15 - RealmReaction[TargetRealm][CurrentRealm];
			else
				reaction = 1;
			if(!CanAfford ((reaction-reaction/5)/3+1, reaction/5, 1, rlm)) {logreroll(__LINE__); goto ReRoll_;}

			if(!random(3)) {logreroll(__LINE__); goto ReRoll_;}

			npc_ai[rlm].CurrentAction = action;

			if (!realm[TargetRealm].mfIsPlayerCtrl())	
			{

				// set tribute
				Tribute[CurrentRealm][TargetRealm] = reaction/5;
				gold = (reaction - reaction/5)/3;
				gold = (gold >= 1)?gold:1;
 				PayForIt(CurrentRealm, gold, 1, 0);

				// take care the demanded holding
				// places[StartPlace].Realm = CurrentRealm;
				//SetGameData(MP_PLACES, MPPLACE_REALM, StartPlace, CurrentRealm, FALSE );
				CheckHolding(StartPlace, CurrentRealm, FALSE);
				
				// take care the offered TargetPlace
				//SetGameData(MP_PLACES, MPPLACE_REALM, TargetPlace, TargetRealm, FALSE );
				CheckHolding(TargetPlace, TargetRealm, FALSE);
			}	 	
			else if ( TargetRealm == HomeRealm )   // home realm is target
			{

#ifdef _WINDOWS  

				if ( IsMultiPlayer() )
				{
					fOtherDipMP = TRUE;  //---- signal we need to wait
				}
#endif

				gold = (reaction - reaction/5)/3;
				gold = (gold >= 1)?gold:1;

				dData.d1.iAlliance  = 0;
				dData.d1.iOffGold	= gold;
				dData.d1.iWhoFrom	= CurrentRealm;
				dData.d1.iReqGold	= 0;
				dData.d1.iReqProv	= 0;
				dData.d1.iReqHold	= StartPlace;
				dData.d1.iReqTrib	= 0;
				dData.d1.iOffProv	= 0;
				dData.d1.iOffHold	= TargetPlace;
				dData.d1.iOffTrib	= reaction/5;;
				dData.d1.iOffRegy	= 1;

				if (curResponse < 8)
					++curResponse;
				response[curResponse].lWhichAction	= DIPLOMACY;
				response[curResponse].lWhichProvince= dData.d2.data1;
				response[curResponse].lWhichHold		= dData.d2.data2;
				response[curResponse].lSuccess	 	= dData.d2.data3;
				response[curResponse].lFromId			= 0;
				response[curResponse].lFromRealm		= 0;
				response[curResponse].lResult			= 0;

				ShowRequest(D_OTHERDIPLOMACY, curResponse);	// do Diplomacy style dialog
			}
			else   //----- Need to wait for remote player
			{
				fSync = TRUE;
			}
			break;

		case ACTION_DIPLMCY_DMD_TRIB:
			ActionExpType = RLM_EXP_PASS;
			ActionExpModifier = 0;
			#ifdef _WINDOWS
			if (IsMultiPlayer()) {logreroll(__LINE__); goto ReRoll_;}
			#endif
			if (rlm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
			if(CurrentRealm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
			if(!FindRealm()) {logreroll(__LINE__); goto ReRoll_;}
			if (TargetRealm == REALM::ANY_REALM) {logreroll(__LINE__); goto ReRoll_;}
			if (!realm[TargetRealm].mfIsPlayerCtrl())	{logreroll(__LINE__); goto ReRoll_;}

			npc_ai[rlm].CurrentAction = action;

			if ( TargetRealm == HomeRealm )  // home realm is target
			{

#ifdef _WINDOWS  

				if ( IsMultiPlayer() )
				{
					fOtherDipMP = TRUE;  //---- signal we need to wait
				}
#endif

				trib = random(3) + 1;
				if(50 > (RealmReaction[TargetRealm][CurrentRealm]-(trib*5)))
					gold = (50 + trib*5 - RealmReaction[TargetRealm][CurrentRealm])/3;
				else
					gold = 1;
				if(!CanAfford (gold, 1, 0, rlm)) {logreroll(__LINE__); goto ReRoll_;}
				dData.d1.iAlliance  = 0;
				dData.d1.iOffGold	= gold;
				dData.d1.iWhoFrom	= CurrentRealm;
				dData.d1.iReqGold	= 0;
				dData.d1.iReqProv	= 0;
				dData.d1.iReqHold	= 0;
				dData.d1.iReqTrib	= trib;
				dData.d1.iOffProv	= 0;
				dData.d1.iOffHold	= 0;

				dData.d1.iOffTrib	= 0;
				dData.d1.iOffRegy	= 1;

				if (curResponse < 8)
					++curResponse;
				response[curResponse].lWhichAction	= DIPLOMACY;
				response[curResponse].lWhichProvince= dData.d2.data1;
				response[curResponse].lWhichHold		= dData.d2.data2;
				response[curResponse].lSuccess	 	= dData.d2.data3;
				response[curResponse].lFromId			= 0;
				response[curResponse].lFromRealm		= 0;
				response[curResponse].lResult			= 0;

				ShowRequest(D_OTHERDIPLOMACY, curResponse);	// do Diplomacy style dialog
			}

			else   //----- Need to wait for remote player
			{
				fSync = TRUE;
			}
			break;

		case ACTION_ESPIONAGE_ASSASSIN:
			if (rlm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
			if(!CanAfford (1, 0, 0, rlm)) {logreroll(__LINE__); goto ReRoll_;}
			if (TriggeredTargetRealm != REALM::NO_COUNTRY && TriggeredTargetRealm!=rlm
					&& TriggeredTargetRealm < LAND_REALM_COUNT)
				TargetRealm = TriggeredTargetRealm;
			else if (!FindFoe(rlm,action)) {logreroll(__LINE__); goto ReRoll_;}		// find a foe of ours
			if (TargetRealm == REALM::ANY_REALM) {logreroll(__LINE__); goto ReRoll_;}

			if (game_difficulty == DIF_EASY)
			{
				if (!FindUnit(rlm, TargetRealm)) {logreroll(__LINE__); goto ReRoll_;}
			}
			else
			{
				if (!FindUnitToAssinate(rlm, TargetRealm)) {logreroll(__LINE__); goto ReRoll_;}
			}

			npc_ai[rlm].CurrentAction = action;
			//if (!RollNPCSuccess(action)) break;
			if(units[TargetUnit].Realm == HomeRealm && TargetProvince > NO_PROVINCE && TargetProvince < NO_PROVINCE2)
				AddReport(	IMMEDIATE_NPC_ASSASSIN, CurrentRealm, TargetProvince, 0);

			SetScrewed((REALM::REALM_TYPE)units[TargetUnit].Realm, CurrentRealm, FALSE );
			DeleteUnit(TargetUnit, FALSE );
			PayForIt(CurrentRealm, 1, 0, 0);
			ActionExpType = RLM_EXP_ESPIONAGE_ASSASSINATE;
			ActionExpModifier = 0;
			CurrentRegent = (REGENT_ENUM) realm[CurrentRealm].mfGetRegent();
			if (CurrentRegent != NO_REGENT)
			{
				SHORT const hPlayerStats = LoadStats(regents[CurrentRegent].mfGetid(), fERROR);
				if (hPlayerStats != fERROR)
				{
					DumbAutoLockPtr <PLAYER_STATS const> const pPS(hPlayerStats);
					if (!npcActionData[rlm].mfFreeSpyUsed() && pPS->mfIsType(PLAYER_CLASS_INFO::THIEF))
					{
						npcActionData[rlm].mfSetSpyUsed();
						goto Another;
					}
				}
			}
			break;

		case ACTION_FORTIFY:
			if (rlm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
			if (game_difficulty == DIF_EASY)
			{
				if (!FindProv(action)) {logreroll(__LINE__); goto ReRoll_;}  	// can't do the action so try another
				if (ForeignUnitsHere(TargetProvince, CurrentRealm)) {logreroll(__LINE__); goto ReRoll_;}
			}
			else
			{
				if (!FindProvToFortify(CurrentRealm, &TargetProvince)) {logreroll(__LINE__); goto ReRoll_;}  	// can't do the action so try another
			}
			if(!CreateHolding(4, random(province[TargetProvince].CivLevel) + 1, 0, TargetProvince, CurrentRealm, FALSE )) {logreroll(__LINE__); goto ReRoll_;}
			npc_ai[rlm].CurrentAction = action;
			ActionExpType = RLM_EXP_PASS;
			ActionExpModifier = 0;
			break;

		case ACTION_LEYLINE:
			if (rlm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
			TargetPlace = 0;
			TargetProvince = NO_PROVINCE;
			StartProvince = NO_PROVINCE;
			StartPlace = 0;
			ActionExpType = RLM_EXP_FORGE_LEYLINE;
			ActionExpModifier = 0;
			for(i=1; i<PROVINCE_COUNT; ++i)
			{
				CheckAndFixPlaces((PROVINCE)i);
			 	j = k = province[i].FirstPlace; 
			 	while (k != 0 )
			 	{
					// look for a source with leyline going out but nothing coming in as a TargetPlace
			 	   	if(places[j].Icon == SOURCE1_ICON 
							&& places[j].Realm == CurrentRealm
						 	&& (places[j].iRoute1 || places[j].iRoute2 || places[j].iRoute3))  // Source has a leyline going out                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
					{
						for(m = 0; m< MAX_PLACES; ++m)
							if( places[m].Icon == SOURCE1_ICON 
									&& (places[m].iRoute1 == j || places[m].iRoute2 == j || places[m].iRoute3 == j))
								break;
						if(m == MAX_PLACES)
						{
							TargetProvince = (PROVINCE)i;
							TargetPlace	= j;
							break;
						}
			 		}
			 		k = places[j].NextPlace;
			 		j = province[i].FirstPlace + k;
			 	}
				if(k!=0)
					break;
			} 
		   	// look for a source without any leyline connect to as a StartPlace
		   	// if there is a targetProvince, look for the closet one, otherwise just pick one
			for(i=1; i<PROVINCE_COUNT; ++i)
			{
				CheckAndFixPlaces((PROVINCE)i);
			 	j = k = province[i].FirstPlace; 
			 	while (k != 0 )
			 	{
					if(places[j].Icon == SOURCE1_ICON 
								&& places[j].Realm == CurrentRealm
						 		&& !places[j].iRoute1&& !places[j].iRoute2 && !places[j].iRoute3)  // Source has a leyline going out                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
					{
						for(m = 0; m< MAX_PLACES; ++m)
							if( places[m].Icon == SOURCE1_ICON 
									&& (places[m].iRoute1 == j || places[m].iRoute2 == j || places[m].iRoute3 == j))
								break;
						if(m == MAX_PLACES)
						{
							if(TargetProvince == 0)
							{
								StartPlace = j;
								StartProvince = (PROVINCE)i;
								break;
							}
							dist = ProvDist (TargetProvince, i, 0, TRUE);
							if(dist < minDist)
							{
								minDist = dist;
								StartPlace = j;
							}
						}
					}
			 		k = places[j].NextPlace;
			 		j = province[i].FirstPlace + k;
			 	}
				if(k != 0)
					break;
			}
			minDist = 99;
			// Found a single node to start with, but didn't find any source with only leyline going out
			// look for a closet source 
			if(TargetPlace == 0 && StartPlace != 0)
			{
				for(i=1; i<PROVINCE_COUNT; ++i)
				{
					CheckAndFixPlaces((PROVINCE)i);
				 	j = k = province[i].FirstPlace; 
				 	while (k != 0 )
				 	{
						if(places[j].Icon == SOURCE1_ICON
						   		&& places[j].Realm == CurrentRealm
						   		&& j != StartPlace)
						{
							dist = ProvDist (StartProvince, i, 0, TRUE);
							if(dist < minDist)
							{
								minDist = dist;
								TargetPlace = j;
							}
						}

				 		k = places[j].NextPlace;
				 		j = province[i].FirstPlace + k;
				 	}
				}
			}
			if(!StartPlace || !TargetPlace)
				 {logreroll(__LINE__); goto ReRoll_;}

			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//i = TargetPlace;
			//TargetPlace = 0;
			//j = TargetProvince;
			//TargetProvince = StartProvince;
			//if (!RollNPCSuccess(action)) break;
			//StartProvince = TargetProvince;
			//TargetProvince = j;
			//if (!RollNPCSuccess(action)) break;
			//TargetPlace = i;
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			if(!CanAfford (minDist, minDist, 0, rlm)) {logreroll(__LINE__); goto ReRoll_;}
			npc_ai[rlm].CurrentAction = action;
			SetGameData(MP_PLACES, MPPLACE_IROUTE1, StartPlace, TargetPlace, FALSE );
			PayForIt(CurrentRealm, minDist, minDist, 0);
			CurrentRegent = (REGENT_ENUM) realm[CurrentRealm].mfGetRegent();
			if (CurrentRegent != NO_REGENT)
			{
				SHORT const hPlayerStats = LoadStats(regents[CurrentRegent].mfGetid(), fERROR);
				if (hPlayerStats != fERROR)
				{
					DumbAutoLockPtr <PLAYER_STATS const> const pPS(hPlayerStats);
					if (npcActionData[rlm].mfFreeForgesUsed() < pPS->StatsMod.mfGetMod(STATSMOD::FREEFORGE_MOD))
						{npcActionData[rlm].mfIncFreeForge(); goto Another;}
				}
			}
			break;

		case ACTION_INVESTITURE:
			ActionExpType = RLM_EXP_INVEST;
			ActionExpModifier = 0;
			if (game_difficulty == DIF_EASY)
			{
				for (i=1; i<PROVINCE_COUNT; ++i)
				{
					if (province[i].OccupRealm == rlm)		// we occupy the prov
					{
						TargetProvince = (PROVINCE)i;
						TargetPlace=0;
						npc_ai[rlm].CurrentAction = action;
						if (!RollNPCSuccess(action, &fSync, &fSend)) goto ActionDone;
						j = province[i].Realm;
						SetGameData(MP_PROVINCE, MPPROV_REALM, i, rlm, fSend );
						SetGameData(MP_PROVINCE, MPPROV_OCCUPREALM, i, 0, fSend );
						if (fSend)
							SetGameData(MP_PROVINCE, MPPROV_INVESTED, rlm, j, TRUE );
						else
						{
							ReconstructMap();
							CheckRealmStatus(rlm, j, FALSE );
						}
						goto ActionDone;
					}
				}
			}
			else
			{
				if (FindProvinceToInvest(CurrentRealm, &TargetProvince))
				{
					TargetPlace = 0;
					i = TargetProvince;
					npc_ai[rlm].CurrentAction = action;
					if (!RollNPCSuccess(action, &fSync, &fSend)) goto ActionDone;
					j = province[i].Realm;
					SetGameData(MP_PROVINCE, MPPROV_REALM, i, rlm, fSend );
					SetGameData(MP_PROVINCE, MPPROV_OCCUPREALM, i, 0, fSend );
					if (fSend)
						SetGameData(MP_PROVINCE, MPPROV_INVESTED, rlm, j, TRUE );
					else
					{
						ReconstructMap();
						CheckRealmStatus(rlm, j, FALSE );
					}
					goto ActionDone;
				}
			}
			{logreroll(__LINE__); goto ReRoll_;}				// can't do the action so try another

		case ACTION_LIEUTENANT:
			ActionExpType = RLM_EXP_PASS;
			ActionExpModifier = 0;
/*temp*/	{logreroll(__LINE__); goto ReRoll_;}				// can't do the action so try another
//			effect here
//			break;
		case ACTION_LTACT_:
			ActionExpType = RLM_EXP_PASS;
			ActionExpModifier = 0;
/*temp*/	{logreroll(__LINE__); goto ReRoll_;}				// can't do the action so try another
//			effect here
//			npcActionData[rlm].SetLtUsed();
//			goto Another;				// action was free so do another


		case ACTION_MOVE_ATTACK_FOE:
		case ACTION_MOVE_ATTACK_OWN:
			ActionExpType = RLM_EXP_PASS;
			ActionExpModifier = 0;
			if (!FindFoe(rlm,action)) {logreroll(__LINE__); goto ReRoll_;}
			if (!FindProv(action)) {logreroll(__LINE__); goto ReRoll_;}
			if (!FindUnit(rlm,action)) {logreroll(__LINE__); goto ReRoll_;}
			goto actmovelbl;

		case ACTION_MOVE_BORDER_FOE:
		case ACTION_MOVE_FILL_FOE:
		case ACTION_MOVE_REINFORCE_FOE:
		case ACTION_MOVE_UNOCCUP_FOE:
		case ACTION_MOVE_BORDER_OWN:
			ActionExpType = RLM_EXP_PASS;
			ActionExpModifier = 0;
			if (!FindFoe(rlm,action)) {logreroll(__LINE__); goto ReRoll_;}
			if (!FindUnit(rlm,action)) {logreroll(__LINE__); goto ReRoll_;}
			if (!FindProv(action)) {logreroll(__LINE__); goto ReRoll_;}
actmovelbl:
			Clear_fProv();
			if (!npc_move(NO_PROVINCE)) {logreroll(__LINE__); goto ReRoll_;}
			npc_ai[rlm].CurrentAction = action;
			goto Another;				// action was free so do another

		case ACTION_MOVE_RETREAT:
		case ACTION_MOVE_FILL_OWN:
		case ACTION_MOVE_UNOCCUP_OWN:
			ActionExpType = RLM_EXP_PASS;
			ActionExpModifier = 0;
			TargetRealm = rlm;
			if (!FindUnit(rlm,action)) {logreroll(__LINE__); goto ReRoll_;}
			if (!FindProv(action)) {logreroll(__LINE__); goto ReRoll_;}
			Clear_fProv();
			if (!npc_move(NO_PROVINCE)) {logreroll(__LINE__); goto ReRoll_;}
			npc_ai[rlm].CurrentAction = action;
			goto Another;				// action was free so do another

		case ACTION_MOVE_REINFORCE_OWN:
			ActionExpType = RLM_EXP_PASS;
			ActionExpModifier = 0;
			TargetRealm = rlm;
			if (!FindUnit(rlm,action)) {logreroll(__LINE__); goto ReRoll_;}
			if ( game_difficulty == DIF_EASY)
			{
				if (!FindProv(action)) {logreroll(__LINE__); goto ReRoll_;}
			}
			else
			{
				if (!FindProvToReinforce(rlm, TargetUnit, &TargetProvince)) {logreroll(__LINE__); goto ReRoll_;}
			}
			Clear_fProv();
			if (!npc_move(NO_PROVINCE)) {logreroll(__LINE__); goto ReRoll_;}
			npc_ai[rlm].CurrentAction = action;
			goto Another;				// action was free so do another

		
		case ACTION_MUSTER_INF:
		case ACTION_MUSTER_EIN:
		case ACTION_MUSTER_BOW:
		case ACTION_MUSTER_IRR:
		case ACTION_MUSTER_PIK:
		case ACTION_MUSTER_LVY:
		case ACTION_MUSTER_SCT:
		case ACTION_MUSTER_KNT:
		case ACTION_MUSTER_CAV:
		case ACTION_MUSTER_MINF:
		case ACTION_MUSTER_MBOW:
		case ACTION_MUSTER_MIRR:
		case ACTION_MUSTER_MPIK:
		case ACTION_MUSTER_MCAV:
		case ACTION_MUSTER_EINF:
		case ACTION_MUSTER_EBOW:
		case ACTION_MUSTER_ECAV:
		case ACTION_MUSTER_DINF:
		case ACTION_MUSTER_DBOW:
		case ACTION_MUSTER_GINF:
		case ACTION_MUSTER_GBOW:
		case ACTION_MUSTER_GCAV:
		case ACTION_MUSTER_NINF:
		case ACTION_MUSTER_NIRR:
		case ACTION_MUSTER_CHPY:
		case ACTION_MUSTER_CSKL:
		case ACTION_MUSTER_CSPD:
		{
			ActionExpType = RLM_EXP_PASS;
			ActionExpModifier = 0;
			if (cntUnits_() > maxNPCUnits())
			{
				#ifdef _WINDOWS
				LOGCOMMENTSLEEP
				FLOGCOMMENT
				{
					char temp[100];
					sprintf ( temp, "   muster unit %d - Failed1: cntUnits %ld", 38, cntUnits_() );
					if ( fLogComment )
						RandomLogComment ( temp );
					else
						Sleep(0);
				}
				#endif

#if MUSTERINFO
				printf("MUSTER FAILED - MaxUnits limit, cntUnits:%d\n",cntUnits_());
#endif
				{logreroll(__LINE__); goto ReRoll_;}
			}
			
			LONG SaveForRainyDay = 10 + random(20);
			for (LONG ijkm = 0; ijkm < LAND_REALM_COUNT; ++ijkm)
			{
				if (TestInvaded(rlm, (REALM::REALM_TYPE) ijkm))
				{
					SaveForRainyDay = 0;
					break;
				}
			}
			if (!CanAfford(UnitMusterCost[action-ACTION_MUSTER_INF+1] + SaveForRainyDay,
							0,
							UnitMaintCost[action-ACTION_MUSTER_INF+1] + SaveForRainyDay,
							rlm))
			{
				#ifdef _WINDOWS
				LOGCOMMENTSLEEP
				FLOGCOMMENT
				{
					char temp[100];
					sprintf ( temp, "   muster unit %d - Failed2: CanAfford", 39 );
					if ( fLogComment )
						RandomLogComment ( temp );
					else
						Sleep(0);
				}
				#endif

#if MUSTERINFO
				printf("MUSTER FAILED - Can't afford, realm:%s\n",realm[rlm].mfGetName());
#endif
				{logreroll(__LINE__); goto ReRoll_;}
			}
			if (!FindProv(action))
			{
				#ifdef _WINDOWS
				LOGCOMMENTSLEEP
				FLOGCOMMENT
				{
					char temp[100];
					sprintf ( temp, "   muster unit %d - Failed3: FindProv", 40 );
					if ( fLogComment )
						RandomLogComment ( temp );
					else
						Sleep(0);
				}
				#endif

				{logreroll(__LINE__); goto ReRoll_;}
			}

			// GWP Because the muster actions are in the same order as the unit
			// icons this works fine.
			i = CreateUnit(TargetProvince, (MAP_ICON)(action-ACTION_MUSTER_INF+1), 0, CurrentRealm, FALSE );
			if (i > 0)
			{
				#ifdef _WINDOWS
				LOGCOMMENTSLEEP
				FLOGCOMMENT
				{
					char temp[100];
					sprintf ( temp, "   MusterUnit %d - %s  in %s, index #%ld",
						41,
						realm[rlm].mfGetName(),
						province[TargetProvince].name,
						i);
					if ( fLogComment )
						RandomLogComment ( temp );
					else
						Sleep(0);
				}
				#endif

				PayForIt(CurrentRealm, UnitMusterCost[action-ACTION_MUSTER_INF+1],0,UnitMaintCost[action-ACTION_MUSTER_INF+1]);
				++province[TargetProvince].LevyMustered;
				npc_ai[rlm].CurrentAction = action;
				goto Another;				// action was free so do another
			}

			#ifdef _WINDOWS
			LOGCOMMENTSLEEP
			FLOGCOMMENT
			{
				char temp[100];
				sprintf ( temp, "   muster unit %d - Failed4: CreateUnit", 42 );
				if ( fLogComment )
					RandomLogComment ( temp );
				else
					Sleep(0);
			}
			#endif

#if MUSTERINFO
			printf("MUSTER FAILED - Can't create unit, cntUnits:%d\n",cntUnits_());
#endif
			{logreroll(__LINE__); goto ReRoll_;}					// action failed so reroll

		}
		case ACTION_RSPELL_BLESSLAND:
			ActionExpType = RLM_EXP_REALM_SPELL;
			ActionExpModifier = RlmSpellLevel(ITEM_SPELL_BLESS_LAND);
			if (!FindProv(action)) {logreroll(__LINE__); goto ReRoll_;}
			if(!CanAfford (1, 1, 0, rlm)) {logreroll(__LINE__); goto ReRoll_;}
			npc_ai[rlm].CurrentAction = action;
			PayForIt(rlm, 1, 1, 0);
			if (province[TargetProvince].Blighted==0)
				SetGameData(MP_PROVINCE, MPPROV_BLESSED, TargetProvince, 2, FALSE);
			else
				SetGameData(MP_PROVINCE, MPPROV_BLIGHTED, TargetProvince, 0, FALSE);
			SetHelped((REALM::REALM_TYPE)province[TargetProvince].Realm, CurrentRealm, FALSE );
			CurrentRegent = (REGENT_ENUM) realm[CurrentRealm].mfGetRegent();
			if (CurrentRegent != NO_REGENT)
			{
				SHORT const hPlayerStats = LoadStats(regents[CurrentRegent].mfGetid(), fERROR);
				if (hPlayerStats != fERROR)
				{
					DumbAutoLockPtr <PLAYER_STATS const> const pPS(hPlayerStats);
					if (npcActionData[rlm].mfFreeMagicUsed() < pPS->StatsMod.mfGetMod(STATSMOD::FREEMAGIC_MOD))
					{
						npcActionData[rlm].mfIncMagicUsed();
						goto Another;
					}
				}
			}
			break;
		case ACTION_RSPELL_BLIGHT:
			ActionExpType = RLM_EXP_REALM_SPELL;
			ActionExpModifier = RlmSpellLevel(ITEM_SPELL_BLIGHT);
			if (TriggeredTargetRealm && TriggeredTargetRealm!=rlm
					&& TriggeredTargetRealm < LAND_REALM_COUNT)
				TargetRealm = TriggeredTargetRealm;
			else if (!FindFoe(rlm,action)) {logreroll(__LINE__); goto ReRoll_;}		// find a foe of ours
			// have a foe in TargetRealm
			if (!FindProv(action)) {logreroll(__LINE__); goto ReRoll_;}
			
			if(!CanAfford (2, 1, 0, rlm)) {logreroll(__LINE__); goto ReRoll_;}
			npc_ai[rlm].CurrentAction = action;
			PayForIt(rlm, 2, 1, 0);
			if(province[TargetProvince].Blessed==0)
				SetGameData(MP_PROVINCE, MPPROV_BLIGHTED, TargetProvince, 2, FALSE);
			else
				SetGameData(MP_PROVINCE, MPPROV_BLESSED, TargetProvince, 0, FALSE );
 			if(TargetRealm == HomeRealm)
				AddReport(IMMEDIATE_NPC_BLIGHT, CurrentRealm, TargetProvince, 0);
			SetScrewed((REALM::REALM_TYPE)province[TargetProvince].Realm, CurrentRealm, FALSE );
			CurrentRegent = (REGENT_ENUM) realm[CurrentRealm].mfGetRegent();
			if (CurrentRegent != NO_REGENT)
			{
				SHORT const hPlayerStats = LoadStats(regents[CurrentRegent].mfGetid(), fERROR);
				if (hPlayerStats != fERROR)
				{
					DumbAutoLockPtr <PLAYER_STATS const> const pPS(hPlayerStats);
					if (npcActionData[rlm].mfFreeMagicUsed() < pPS->StatsMod.mfGetMod(STATSMOD::FREEMAGIC_MOD))
					{
						npcActionData[rlm].mfIncMagicUsed();
						goto Another;
					}
				}
			}
			break;
		case ACTION_RSPELL_WARDING:
			ActionExpType = RLM_EXP_REALM_SPELL;
			ActionExpModifier = RlmSpellLevel(ITEM_SPELL_WARDING);
			if (TriggeredTargetRealm && TriggeredTargetRealm!=rlm
					&& TriggeredTargetRealm < LAND_REALM_COUNT)
				TargetRealm = TriggeredTargetRealm;
			else if (!FindFoe(rlm,action)) 
				{logreroll(__LINE__); goto ReRoll_;}
			// have a foe in TargetRealm
			if (!FindProv(action)) {logreroll(__LINE__); goto ReRoll_;}
			if(!CanAfford (5, 5, 0, rlm)) {logreroll(__LINE__); goto ReRoll_;}
			npc_ai[rlm].CurrentAction = action;
			PayForIt(rlm, 5, 5, 0);
			SetGameData(MP_PROVINCE, MPPROV_WARDED, TargetProvince, 2, FALSE );
			SetGameData(MP_PROVINCE, MPPROV_DISPELLVL, TargetProvince, rlm, FALSE );
			SetGameData(MP_PROVINCE, MPPROV_DISPEL, TargetProvince, 0, FALSE );
 			if(TargetRealm == HomeRealm)
 				AddReport(IMMEDIATE_NPC_WARDING, CurrentRealm, TargetProvince, 0);
			SetScrewed((REALM::REALM_TYPE)province[TargetProvince].Realm, CurrentRealm, FALSE );
			CurrentRegent = (REGENT_ENUM) realm[CurrentRealm].mfGetRegent();
			if (CurrentRegent != NO_REGENT)
			{
				SHORT const hPlayerStats = LoadStats(regents[CurrentRegent].mfGetid(), fERROR);
				if (hPlayerStats != fERROR)
				{
					DumbAutoLockPtr <PLAYER_STATS const> const pPS(hPlayerStats);
					if (npcActionData[rlm].mfFreeMagicUsed() < pPS->StatsMod.mfGetMod(STATSMOD::FREEMAGIC_MOD))
					{
						npcActionData[rlm].mfIncMagicUsed();
						goto Another;
					}
				}
			}
			break;
		case ACTION_RSPELL_DEMAGOGUE:
			ActionExpType = RLM_EXP_REALM_SPELL;
			ActionExpModifier = RlmSpellLevel(ITEM_SPELL_DEMAGOGUE);
			// decrease the loyalty
			if (TriggeredTargetRealm && TriggeredTargetRealm!=rlm
					&& TriggeredTargetRealm < LAND_REALM_COUNT)
				TargetRealm = TriggeredTargetRealm;
			else if (!FindFoe(rlm,action)) 
				{logreroll(__LINE__); goto ReRoll_;}
			// have a foe in TargetRealm
			if (!FindProv(action)) {logreroll(__LINE__); goto ReRoll_;}
			if(!CanAfford (1, 5, 0, rlm)) {logreroll(__LINE__); goto ReRoll_;}
			npc_ai[rlm].CurrentAction = action;
			PayForIt(rlm, 1, 5, 0);
			province[TargetProvince].Loyalty--;
			SetGameData(MP_PROVINCE, MPPROV_LOYALTY, TargetProvince, province[TargetProvince].Loyalty, FALSE );
 			if(TargetRealm == HomeRealm)
				AddReport(IMMEDIATE_NPC_DEMAGOGUE, CurrentRealm, TargetProvince, province[TargetProvince].Loyalty);
			SetScrewed((REALM::REALM_TYPE)province[TargetProvince].Realm, CurrentRealm, FALSE );
			CurrentRegent = (REGENT_ENUM) realm[CurrentRealm].mfGetRegent();
			if (CurrentRegent != NO_REGENT)
			{
				SHORT const hPlayerStats = LoadStats(regents[CurrentRegent].mfGetid(), fERROR);
				if (hPlayerStats != fERROR)
				{
					DumbAutoLockPtr <PLAYER_STATS const> const pPS(hPlayerStats);
					if (npcActionData[rlm].mfFreeMagicUsed() < pPS->StatsMod.mfGetMod(STATSMOD::FREEMAGIC_MOD))
					{
						npcActionData[rlm].mfIncMagicUsed();
						goto Another;
					}
				}
			}
			break;
				
		case ACTION_RSPELL_RAZE:
			ActionExpType = RLM_EXP_REALM_SPELL;
			ActionExpModifier = RlmSpellLevel(ITEM_SPELL_RAZE);
			if (TriggeredTargetRealm && TriggeredTargetRealm!=rlm
					&& TriggeredTargetRealm < LAND_REALM_COUNT)
				TargetRealm = TriggeredTargetRealm;
			else if (!FindFoe(rlm,action)) 
				{logreroll(__LINE__); goto ReRoll_;}
			// find the castle StartPlace in a province with source in it
			if (!FindProv(action)) {logreroll(__LINE__); goto ReRoll_;}
			
			if(!CanAfford (2, 10, 0, rlm)) {logreroll(__LINE__); goto ReRoll_;}
			npc_ai[rlm].CurrentAction = action;
			PayForIt(rlm, 2, 10, 0);
			places[TargetPlace].Level--;
			SetGameData(MP_PLACES, MPPLACE_LEVEL, TargetPlace, places[TargetPlace].Level, FALSE );
			SetGameData(MP_PLACES, MPPLACE_IICONBITM, TargetPlace, places[TargetPlace].Icon+(((places[TargetPlace].Level<8)?places[TargetPlace].Level:7)/2), FALSE );
 			if(TargetRealm == HomeRealm)
	 			AddReport(IMMEDIATE_RAZE, CurrentRealm, TargetProvince, places[TargetPlace].Level);
			SetScrewed((REALM::REALM_TYPE)province[TargetProvince].Realm, CurrentRealm, FALSE );
			CurrentRegent = (REGENT_ENUM) realm[CurrentRealm].mfGetRegent();
			if (CurrentRegent != NO_REGENT)
			{
				SHORT const hPlayerStats = LoadStats(regents[CurrentRegent].mfGetid(), fERROR);
				if (hPlayerStats != fERROR)
				{
					DumbAutoLockPtr <PLAYER_STATS const> const pPS(hPlayerStats);
					if (npcActionData[rlm].mfFreeMagicUsed() < pPS->StatsMod.mfGetMod(STATSMOD::FREEMAGIC_MOD))
					{
						npcActionData[rlm].mfIncMagicUsed();
						goto Another;
					}
				}
			}
			break;

		case ACTION_RSPELL_STRONGHOLD:
			ActionExpType = RLM_EXP_REALM_SPELL;
			ActionExpModifier = RlmSpellLevel(ITEM_SPELL_STRONGHOLD);
			if (!FindProv(action)) {logreroll(__LINE__); goto ReRoll_;}
			if(!CanAfford (10, 8, 0, rlm)) {logreroll(__LINE__); goto ReRoll_;}
			npc_ai[rlm].CurrentAction = action;
			PayForIt(rlm, 10, 8, 0);

			CreateHolding(5, 0, 0, TargetProvince, CurrentRealm, FALSE );
			
			CheckAndFixPlaces(TargetProvince);
			i = j = province[TargetProvince].FirstPlace;
			while (j != 0 && i>=0 && i<MAX_PLACES)
			{
				if (places[i].Icon == WIZTOWER_ICON)	// find the new wiztower just created
				{
					++places[i].Level;
					SetGameData(MP_PLACES, MPPLACE_LEVEL, i, places[i].Level, FALSE );
					break;
				}
				j = places[i].NextPlace;
				i = province[TargetProvince].FirstPlace + j;
			}
 			if(province[TargetProvince].Realm == HomeRealm)
 		   		AddReport(IMMEDIATE_NPC_STRONGHOLD, CurrentRealm, TargetProvince, 0);	
			SetScrewed((REALM::REALM_TYPE)province[TargetProvince].Realm, CurrentRealm, FALSE );

			CurrentRegent = (REGENT_ENUM) realm[CurrentRealm].mfGetRegent();
			if (CurrentRegent != NO_REGENT)
			{
				SHORT const hPlayerStats = LoadStats(regents[CurrentRegent].mfGetid(), fERROR);
				if (hPlayerStats != fERROR)
				{
					DumbAutoLockPtr <PLAYER_STATS const> const pPS(hPlayerStats);
					if (npcActionData[rlm].mfFreeMagicUsed() < pPS->StatsMod.mfGetMod(STATSMOD::FREEMAGIC_MOD))
					{
						npcActionData[rlm].mfIncMagicUsed();
						goto Another;
					}
				}
			}
			break;

		case ACTION_RSPELL_SUMMONING:
		{
			ActionExpType = RLM_EXP_REALM_SPELL;
			ActionExpModifier = RlmSpellLevel(ITEM_SPELL_SUMMONING);
			if (!FindProv(action)) {logreroll(__LINE__); goto ReRoll_;}
			MAP_ICON const MonsterIcon = MonsterUnits[random(4)];
			if(!CanAfford (2, 5, UnitMaintCost[MonsterIcon], rlm)) {logreroll(__LINE__); goto ReRoll_;}
			npc_ai[rlm].CurrentAction = action;
			LONG const iUnit = CreateUnit(TargetProvince, MonsterIcon, 0, CurrentRealm, FALSE );
			if ( iUnit > 0)
			{
				PayForIt(rlm, 2, 5, 0);
	 			if(province[TargetProvince].Realm == HomeRealm)
		 			AddReport(IMMEDIATE_NPC_SUMMONING, CurrentRealm, TargetProvince, 0);
				if (!AREALLIED((REALM::REALM_TYPE)province[TargetProvince].Realm, rlm))
				{
					SetScrewed((REALM::REALM_TYPE)province[TargetProvince].Realm, CurrentRealm, FALSE );
				}
			}
			else
			{
				goto ReRoll_;
			}
		}
			CurrentRegent = (REGENT_ENUM) realm[CurrentRealm].mfGetRegent();
			if (CurrentRegent != NO_REGENT)
			{
				SHORT const hPlayerStats = LoadStats(regents[CurrentRegent].mfGetid(), fERROR);
				if (hPlayerStats != fERROR)
				{
					DumbAutoLockPtr <PLAYER_STATS const> const pPS(hPlayerStats);
					if (npcActionData[rlm].mfFreeMagicUsed() < pPS->StatsMod.mfGetMod(STATSMOD::FREEMAGIC_MOD))
					{
						npcActionData[rlm].mfIncMagicUsed();
						goto Another;
					}
				}
			}
			break;
			
		case ACTION_RSPELL_BLESSARMY:
			ActionExpType = RLM_EXP_REALM_SPELL;
			ActionExpModifier = RlmSpellLevel(ITEM_SPELL_BLESS_ARMY);
			if (!FindProv(action)) {logreroll(__LINE__); goto ReRoll_;}
			if (!FindUnit(CurrentRealm, action)) {logreroll(__LINE__); goto ReRoll_;}
			if(!CanAfford (1, 1, 0, rlm)) {logreroll(__LINE__); goto ReRoll_;}
			npc_ai[rlm].CurrentAction = action;
			PayForIt(rlm, 1, 1, 0);
			// units[TargetUnit].Blessed = TRUE;
			SetGameData(MP_UNITS, MPUNITS_BLESSED, TargetUnit, TRUE, FALSE );
			CurrentRegent = (REGENT_ENUM) realm[CurrentRealm].mfGetRegent();
			if (CurrentRegent != NO_REGENT)
			{
				SHORT const hPlayerStats = LoadStats(regents[CurrentRegent].mfGetid(), fERROR);
				if (hPlayerStats != fERROR)
				{
					DumbAutoLockPtr <PLAYER_STATS const> const pPS(hPlayerStats);
					if (npcActionData[rlm].mfFreeMagicUsed() < pPS->StatsMod.mfGetMod(STATSMOD::FREEMAGIC_MOD))
					{
						npcActionData[rlm].mfIncMagicUsed();
						goto Another;
					}
				}
			}
			break;

		case ACTION_RSPELL_DEATHPLAGUE:
			ActionExpType = RLM_EXP_REALM_SPELL;
			ActionExpModifier = RlmSpellLevel(ITEM_SPELL_DEATH_PLAGUE);
			if (TriggeredTargetRealm != REALM::NO_COUNTRY && TriggeredTargetRealm!=rlm
					&& TriggeredTargetRealm < LAND_REALM_COUNT)
				TargetRealm = TriggeredTargetRealm;
			else if (!FindFoe(rlm,action)) 
				{logreroll(__LINE__); goto ReRoll_;}
			if (!FindProv(action)) {logreroll(__LINE__); goto ReRoll_;}
			if(!CanAfford (2, 1, 0, rlm)) {logreroll(__LINE__); goto ReRoll_;}
			npc_ai[rlm].CurrentAction = action;
			PayForIt(rlm, 2, 1, 0);
			SetGameData(MP_PROVINCE, MPPROV_CIVLEVEL, TargetProvince, (province[TargetProvince].CivLevel-1), FALSE );
 			if(province[TargetProvince].Realm == HomeRealm)
 				AddReport(IMMEDIATE_NPC_DEATH_PLAGUE, CurrentRealm, TargetProvince, province[TargetProvince].CivLevel);	
			SetScrewed((REALM::REALM_TYPE)province[TargetProvince].Realm, CurrentRealm, FALSE );
			CurrentRegent = (REGENT_ENUM) realm[CurrentRealm].mfGetRegent();
			if (CurrentRegent != NO_REGENT)
			{
				SHORT const hPlayerStats = LoadStats(regents[CurrentRegent].mfGetid(), fERROR);
				if (hPlayerStats != fERROR)
				{
					DumbAutoLockPtr <PLAYER_STATS const> const pPS(hPlayerStats);
					if (npcActionData[rlm].mfFreeMagicUsed() < pPS->StatsMod.mfGetMod(STATSMOD::FREEMAGIC_MOD))
					{
						npcActionData[rlm].mfIncMagicUsed();
						goto Another;
					}
				}
			}
			break;

		case ACTION_RSPELL_LEGIONOFTHEDEAD:
		{
			ActionExpType = RLM_EXP_REALM_SPELL;
			ActionExpModifier = RlmSpellLevel(ITEM_SPELL_LEGION_OF_DEAD);
			if(!CanAfford (1, 4, UnitMaintCost[C_SKL_UNIT_ICON], rlm)) {logreroll(__LINE__); goto ReRoll_;}
			if (!FindProv(action)) {logreroll(__LINE__); goto ReRoll_;}
			npc_ai[rlm].CurrentAction = action;
			LONG const iUnit = CreateUnit(TargetProvince, C_SKL_UNIT_ICON, 0, CurrentRealm, FALSE );
			if (iUnit > 0)
			{
				PayForIt(rlm, 1, 4, 0);
				#ifdef _WINDOWS
				LOGCOMMENTSLEEP
				FLOGCOMMENT
				{
					char temp[100];
					sprintf ( temp, "   LegionOfDead - %s  in %s, index #%ld",
						realm[rlm].mfGetName(),
						province[TargetProvince].name,
						iUnit);
					if ( fLogComment )
						RandomLogComment ( temp );
					else
						Sleep(0);
				}
				#endif

	 			if(province[TargetProvince].Realm == HomeRealm)
	 				AddReport(IMMEDIATE_NPC_LEGION_OF_DEAD, CurrentRealm, TargetProvince, 0);
				
				if (!AREALLIED((REALM::REALM_TYPE)province[TargetProvince].Realm, CurrentRealm))
				{
					SetScrewed((REALM::REALM_TYPE)province[TargetProvince].Realm, CurrentRealm, FALSE );
				}
				CurrentRegent = (REGENT_ENUM) realm[CurrentRealm].mfGetRegent();
				if (CurrentRegent != NO_REGENT)
				{
					SHORT const hPlayerStats = LoadStats(regents[CurrentRegent].mfGetid(), fERROR);
					if (hPlayerStats != fERROR)
					{
						DumbAutoLockPtr <PLAYER_STATS const> const pPS(hPlayerStats);
						if (npcActionData[rlm].mfFreeMagicUsed() < pPS->StatsMod.mfGetMod(STATSMOD::FREEMAGIC_MOD))
						{
							npcActionData[rlm].mfIncMagicUsed();
							goto Another;
						}
					}
				}
			}
			else
			{
				goto ReRoll_;
			}
		}
			break;

		case ACTION_RSPELL_MASSDESTRUCTION:
			ActionExpType = RLM_EXP_REALM_SPELL;
			ActionExpModifier = RlmSpellLevel(ITEM_SPELL_MASS_DESTRUCTION);
			if (TriggeredTargetRealm != REALM::NO_COUNTRY && TriggeredTargetRealm!=rlm
					&& TriggeredTargetRealm < LAND_REALM_COUNT)
				TargetRealm = TriggeredTargetRealm;
			else if (!FindFoe(rlm,action)) 
				{logreroll(__LINE__); goto ReRoll_;}
			if (!FindUnit(TargetRealm, action)) {logreroll(__LINE__); goto ReRoll_;}
			if(!CanAfford (5, 10, 0, rlm)) {logreroll(__LINE__); goto ReRoll_;}
			npc_ai[rlm].CurrentAction = action;
			PayForIt(rlm, 5, 10, 0);
 			if(TargetRealm == HomeRealm)
 				AddReport(IMMEDIATE_NPC_MASS_DESTRUCTION, CurrentRealm, TargetProvince, 0);
			
			SetScrewed(TargetRealm, CurrentRealm, FALSE );
			DeleteUnit(TargetUnit, FALSE);
			CurrentRegent = (REGENT_ENUM) realm[CurrentRealm].mfGetRegent();
			if (CurrentRegent != NO_REGENT)
			{
				SHORT const hPlayerStats = LoadStats(regents[CurrentRegent].mfGetid(), fERROR);
				if (hPlayerStats != fERROR)
				{
					DumbAutoLockPtr <PLAYER_STATS const> const pPS(hPlayerStats);
					if (npcActionData[rlm].mfFreeMagicUsed() < pPS->StatsMod.mfGetMod(STATSMOD::FREEMAGIC_MOD))
					{
						npcActionData[rlm].mfIncMagicUsed();
						goto Another;
					}
				}
			}
			break;

		case ACTION_RULE_PROV:
			if (rlm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
			if (!FindProv(ACTION_RULE_PROV)) {logreroll(__LINE__); goto ReRoll_;}
			TargetPlace=0;
			ActionExpType = RLM_EXP_RULE_PROV;
			ActionExpModifier = province[TargetProvince].CivLevel;
			if (province[TargetProvince].CivLevel >= 10) {logreroll(__LINE__); goto ReRoll_;}
			npc_ai[rlm].CurrentAction = action;
			if (!RollNPCSuccess(action, &fSync, &fSend)) break;

//			++province[TargetProvince].CivLevel;
			SetGameData(MP_PROVINCE, MPPROV_CIVLEVEL, TargetProvince, (province[TargetProvince].CivLevel+1), fSend );
//			province[TargetProvince].MagLevel--;
			if (province[TargetProvince].MagLevel>0 && !province[TargetProvince].ElfPop)
				SetGameData(MP_PROVINCE, MPPROV_MAGLEVEL, TargetProvince, (province[TargetProvince].MagLevel-1), fSend );
			CheckAndFixSourceLevels(TargetProvince);
			break;

		case ACTION_RULE_LAW:
			if (rlm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
			if (game_difficulty == DIF_EASY)
			{
				if (!FindHolding(REALM::ANY_REALM, rlm, ANY_PROVINCE, LAW1_ICON, FALSE)) {logreroll(__LINE__); goto ReRoll_;}
				if (!CheckHoldLevels(TargetProvince,LAW1_ICON)) {logreroll(__LINE__); goto ReRoll_;}
			}
			else
			{
				if (!FindLawToRule(rlm, &TargetProvince, &TargetPlace)) {logreroll(__LINE__); goto ReRoll_;}
			}
			ActionExpType = RLM_EXP_RULE_LAW;
			ActionExpModifier = places[TargetPlace].Level;
			npc_ai[rlm].CurrentAction = action;
			if (!RollNPCSuccess(action,&fSync, &fSend)) break;
			SetGameData(MP_PLACES, MPPLACE_LEVEL, TargetPlace, (places[TargetPlace].Level+1), fSend );
			SetGameData(MP_PLACES, MPPLACE_IICONBITM, TargetPlace, places[TargetPlace].Icon+(((places[TargetPlace].Level<8)?places[TargetPlace].Level:7)/2), fSend );
//			++places[TargetPlace].Level;
			break;
		case ACTION_RULE_GLD:
			if (rlm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
			if (game_difficulty == DIF_EASY)
			{
				if (!FindHolding(REALM::ANY_REALM, rlm, ANY_PROVINCE, GUILD1_ICON, FALSE)) {logreroll(__LINE__); goto ReRoll_;}
				if (!CheckHoldLevels(TargetProvince,GUILD1_ICON)) {logreroll(__LINE__); goto ReRoll_;}
			}
			else
			{
				if (!FindGuildToRule(rlm, &TargetProvince, &TargetPlace)) {logreroll(__LINE__); goto ReRoll_;}
			}
			ActionExpType = RLM_EXP_RULE_GUILD;
			ActionExpModifier = places[TargetPlace].Level;
			npc_ai[rlm].CurrentAction = action;
			if (!RollNPCSuccess(action, &fSync, &fSend)) break;
			SetGameData(MP_PLACES, MPPLACE_LEVEL, TargetPlace, (places[TargetPlace].Level+1), fSend );
			SetGameData(MP_PLACES, MPPLACE_IICONBITM, TargetPlace, places[TargetPlace].Icon+(((places[TargetPlace].Level<8)?places[TargetPlace].Level:7)/2), fSend );
//			++places[TargetPlace].Level;
			break;
		case ACTION_RULE_TMP:
			if (rlm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
			if (game_difficulty == DIF_EASY)
			{
				if (!FindHolding(REALM::ANY_REALM, rlm, ANY_PROVINCE, TEMPLE1_ICON, FALSE)) {logreroll(__LINE__); goto ReRoll_;}
				if (!CheckHoldLevels(TargetProvince,TEMPLE1_ICON)) {logreroll(__LINE__); goto ReRoll_;}
			}
			else
			{
				if (!FindTempleToRule(rlm, &TargetProvince, &TargetPlace)) {logreroll(__LINE__); goto ReRoll_;}
			}
			ActionExpType = RLM_EXP_RULE_TEMPLE;
			ActionExpModifier = places[TargetPlace].Level;
			npc_ai[rlm].CurrentAction = action;
			if (!RollNPCSuccess(action, &fSync, &fSend)) break;
			SetGameData(MP_PLACES, MPPLACE_LEVEL, TargetPlace, (places[TargetPlace].Level+1), fSend );
			SetGameData(MP_PLACES, MPPLACE_IICONBITM, TargetPlace, places[TargetPlace].Icon+(((places[TargetPlace].Level<8)?places[TargetPlace].Level:7)/2), fSend );
//			++places[TargetPlace].Level;
			break;
		case ACTION_RULE_SRC:
			if (rlm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
			if (game_difficulty == DIF_EASY)
			{
				if (!FindHolding(rlm, rlm, ANY_PROVINCE, SOURCE1_ICON, FALSE)) {logreroll(__LINE__); goto ReRoll_;}
				if (!CheckHoldLevels(TargetProvince,SOURCE1_ICON)) {logreroll(__LINE__); goto ReRoll_;}
			}
			else
			{
				if (!FindSrcToRule(rlm, &TargetProvince, &TargetPlace)) {logreroll(__LINE__); goto ReRoll_;}
			}
			ActionExpType = RLM_EXP_RULE_SOURCE;
			ActionExpModifier = places[TargetPlace].Level;
			npc_ai[rlm].CurrentAction = action;
			if (!RollNPCSuccess(action, &fSync, &fSend )) break;
			SetGameData(MP_PLACES, MPPLACE_LEVEL, TargetPlace, (places[TargetPlace].Level+1), fSend );
			SetGameData(MP_PLACES, MPPLACE_IICONBITM, TargetPlace, places[TargetPlace].Icon+(((places[TargetPlace].Level<8)?places[TargetPlace].Level:7)/2), fSend );
//			++places[TargetPlace].Level;
			break;

		case ACTION_TRADEROUTE:
			ActionExpType = RLM_EXP_BUILD_TRADEROUTE;
			ActionExpModifier = 0;
			if (rlm == REALM::MONSTERS) {logreroll(__LINE__); goto ReRoll_;}
			//printf("TRADE ROUTE - start");
			if (!FindHolding(rlm,rlm,ANY_PROVINCE,GUILD1_ICON,TRUE)) {logreroll(__LINE__); goto ReRoll_;} // if no guilds don't bother
			//printf(" found:%s  end",province[TargetProvince].name);
			StartProvince = TargetProvince;
			if (!FindDiffTerrain(VIA_ROAD, StartProvince, &TargetProvince)) {logreroll(__LINE__); goto ReRoll_;}	// must be to diff terrain
			//printf(" found:%s\n",province[TargetProvince].name);

			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//i = TargetPlace;
			//TargetPlace = 0;
			//j = TargetProvince;
			//TargetProvince = StartProvince;
			//if (!RollNPCSuccess(action)) break;
			//StartProvince = TargetProvince;
			//TargetProvince = j;
			//if (!RollNPCSuccess(action)) break;
			//TargetPlace = i;
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			if (!npc_traderoute(TargetPlace)) {logreroll(__LINE__); goto ReRoll_;}
			npc_ai[rlm].CurrentAction = action;
			break;

		case ACTION_PASS:
			ActionExpType = RLM_EXP_PASS;
			// If difficulty moderate or better make the AI's try more stuff.
			if (PassedOnceAlready == FALSE && game_difficulty != DIF_EASY)
			{
				// Don't waste a action if we could have done something useful.
				PassedOnceAlready = TRUE;
				goto NewMode;
			}
			break;

		default:
			print_textf(320, 240, WHITE, STRMGR_GetStr(STR_MAPAI_UNSUPPORTED_ACTN_ERROR_FMT), realm[rlm].mfGetName(), action);
			{logreroll(__LINE__); goto ReRoll_;}				// can't do the action so try another
		}
		
		
		CurrentRegent = (REGENT_ENUM) realm[CurrentRealm].mfGetRegent();
		if (CurrentRegent != NO_REGENT && action != ACTION_PASS)
		{
			ActionExpPoints(CurrentRegent, ActionExpType, ActionExpModifier, FALSE);
			
			SHORT const hPlayerStats = LoadStats(regents[CurrentRegent].mfGetid(), fERROR);
			if (hPlayerStats != fERROR)
			{
				DumbAutoLockPtr <PLAYER_STATS const> const pPS(hPlayerStats);
				if (npcActionData[rlm].mfFreeAction() < pPS->StatsMod.mfGetMod(STATSMOD::FREEACTION_MOD))
				{
					npcActionData[rlm].mfIncFreeAction();
					goto Another;
				}
			}
		}
	}
ActionDone:

	#ifdef _WINDOWS
	//---- Waiting for RollNPCSuccess() on remote machine
	//---- Yes even more multiplayer ( SYNC POINT #5 )
	if (IsMultiPlayer() && fSync)
	{
		fSync = 0;
		StartSync( iMPS_SYNC_INITIATIVE + iCurrentRealmIndex );
		CheckSync( (iMPS_SYNC_INITIATIVE + iCurrentRealmIndex), TRUE, TRUE );
	}
	#endif

	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	FLOGCOMMENT
	{
		char temp[100];
		sprintf ( temp, "--------    %d end of %s turn (GB:%d R:%d)    --------",
			43,
			realm[rlm].mfGetName(),
			realm[rlm].mfGetTreasury(),
			realm[rlm].mfGetRegency());
		if ( fLogComment )
			RandomLogComment ( temp );
		else
			Sleep(0);
	}
	#endif

	fRedrawAll = TRUE;
	return FALSE;

ReRoll_:
	// ABC  Never change or remove this code. Its here to fix a compiler bug
	{
		char temp[100];
		sprintf ( temp, "DoNPCAction - Reroll" );
	}

	ActionTried[k] = TRUE;
	goto ReRoll;


Another_:
	// ABC  Never change or remove this code. Its here to fix a compiler bug
	{
		char temp[100];
		sprintf ( temp, "DoNPCAction - Another" );
	}

	goto Another;
}

/* ======================================================================== */

/* ======================================================================== */
void MapAIPlayerResponseWaitSync (void)
{
	#ifdef _WINDOWS
	StartSync( iMPS_SYNC_INITIATIVE + iCurrentRealmIndex );
	CheckSync( (iMPS_SYNC_INITIATIVE + iCurrentRealmIndex), TRUE, TRUE );
	#endif
}

/* ========================================================================
   Function    - BroadcastGoldAndRegency
	//NOTE: This routine is example only and is NOT WORKING!
	Description - 
	Returns     - 
	======================================================================== */
#ifdef _WINDOWS
void BroadcastGoldAndRegency( void )
{
	LONG	i;
	SHORT	Gold[LAND_REALM_COUNT];
	SHORT	Regency[LAND_REALM_COUNT];
	
	for ( i = 1; i < LAND_REALM_COUNT; i++ )
	{
		Gold[i] = realm[i].mfGetTreasury();
		Regency[i] = realm[i].mfGetRegency();
	}
	
	AMultiPlayer.AdminMessage((UBYTE *)Gold[0], (UBYTE *)Regency[0], "SEND ADMIN" );
}
#endif
/* ========================================================================
   Function    - BroadcastAllienceTable
	Description - broadcast an allience table 
	Returns     - 
	======================================================================== */
#ifdef _WINDOWS
void BroadcastAllienceTable ( void )
{
	LONG	i;
	
	// send one realms reaction and status
	for ( i = 0; i < REALM::REALM_COUNT; i++)
	{
		AMultiPlayer.AlliedMessage(
			i, 
			(UBYTE *)&RealmReaction[i][0], 
			(UBYTE *)&RealmStatus[i][0], 
			"SEND ALLIENCE" );
	}
	
}
#endif
/* ======================================================================== */
