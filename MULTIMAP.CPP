/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: MULITMAP.cpp  -
   Author:   Michael Branham

   ========================================================================
   Contains the following general functions:

   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#ifdef _WINDOWS
#include <windows.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "SYSTEM.H"

#include "ACTNMENU.HXX"
#include "CHARSEL.HXX"
#include "GMENUENM.H"
#include "GAMEMAP.HXX"
#include "MAPAI.HXX"
#include "MENU.H"
#include "REQUEST.H"
#include "MULTIMAP.HXX"
#include "MULTIUI.HXX"
#include "REALM.HXX"
#include "SOUND.HXX"
#include "strenum.h"
#include "STRMGR.H"
#include "UNITS.HXX"
#include "INVNTORY.HXX"
#include "REPORT.HXX"
#include "REGENTS.HXX"
#include "PLACES.HXX"

//---- Insure we only have mono in debug versions
#ifdef _DEBUG
#define _MULTIMONO
#endif

#ifdef _WINDOWS
#include "WINSYS\MULPLAY.HXX"
#include "WINSYS\MONO_C.H"
#endif

extern BOOL fLogComment;

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
//#define	LOGCOMMENTSLEEP	Sleep(1);
#define  LOGCOMMENTSLEEP	{;}

#define IX_TT_NONE				3

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

void SetRedrawMainMapLevel (void);

extern void RandomLogComment ( char * szString );


// void PaintGenericStatement(LONG MenuCombo, LONG);
void DrawRoads_ (PROVINCE prov, LONG i);
void CancelRoll (void);
void CalcTimerBar(void);
void RedrawProcessPanel(void);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
static LONG iVal0, iVal1, iVal2, iVal3;

LONG iGameData;

LONG lGameSemiphore[MPGAMESEMIPHORE_MAX_DATA];

extern BOOL			fPlayerNeverDeclaredWar;

extern int			iTTimeLimit;
extern LONG			lTimeLimit[];
extern LONG			WaitingCount;
extern LONG			WaitingWho;
extern LONG			WaitingWhen;
extern LONG			WaitingBar216ths;

/* ------------------------------------------------------------------------
   External Variables
   ------------------------------------------------------------------------ */

#if 0
extern "C" {
extern PMENU		Menus;
}
#endif


/* ========================================================================
   Function    - DoSetGameData
   Description -
   Returns     - void
   ======================================================================== */
void DoSetGameData(LONG lArray_Id, LONG lStruct_Id, LONG lArray_Index, LONG lData)
{

	char	n[40];

	switch(lArray_Id)
	{
		case	MP_ADVSITE:
			switch(lStruct_Id)
			{
				case	MPADV_AVAILABLE:
					advsite[lArray_Index].available = (BOOL) lData;
					break;
			}
			break;

		case	MP_REALM:
			switch(lStruct_Id)
			{
				case	MPRLM_NAME:
					// can't set a string, so just break;
					break;

				case	MPRLM_X:
					realm[lArray_Index].mfSetNameX( (SHORT) lData);
					break;

				case	MPRLM_Y:
					realm[lArray_Index].mfSetNameY((SHORT) lData);
					break;

				case	MPRLM_TITLESIZE:
					realm[lArray_Index].mfSetTitleSize((unsigned int) lData);
					break;

				case	MPRLM_TITLESPREAD:
					realm[lArray_Index].mfSetTitleSpread((unsigned int) lData);
					break;

				case	MPRLM_TITLETRANS:
					realm[lArray_Index].mfSetTitleTrans((unsigned int) lData);
					break;

				case	MPRLM_REGENT:
					realm[lArray_Index].mfSetRegent((unsigned int) lData);
					break;

//				case	MPRLM_COLOR1:
//					realm[lArray_Index].color1 = (unsigned int) lData;
//					break;

//				case	MPRLM_COLOR2:
//					realm[lArray_Index].color2 = (unsigned int) lData;
//					break;

				case	MPRLM_SHOWUNITS:
					realm[lArray_Index].mfSetShowUnits((unsigned int) lData);
					break;

				case	MPRLM_SHOWHOLDINGS:
					realm[lArray_Index].mfSetShowHoldings((unsigned int) lData);
					break;

				case	MPRLM_PLAYABLE:
					realm[lArray_Index].mfSetPlayable((unsigned int) lData);
					break;

				case	MPRLM_PLAYERCTRL:
					realm[lArray_Index].mfSetPlayerCtrl((unsigned int) lData);
					break;

				case	MPRLM_REGENCY:
					realm[lArray_Index].mfSetRegency((unsigned int) lData);
					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						char temp[100];
						sprintf ( temp, "   multimap - %s regency:%ld",realm[lArray_Index].mfGetName(),realm[lArray_Index].mfGetRegency());
						RandomLogComment ( temp );
					}
					break;

				case	MPRLM_TREASURY:
					realm[lArray_Index].mfSetTreasury((unsigned int) lData);
					break;

				case	MPRLM_COURT:
					realm[lArray_Index].mfSetCourt( (unsigned int) lData);
					break;

				case	MPRLM_FIRST_PROVINCE:
					realm[lArray_Index].mfSetFirstProvince((unsigned int) lData);
					break;

				case	MPRLM_INCOME:
					realm[lArray_Index].mfSetIncome((SHORT) lData);
					break;

				case	MPRLM_BATTLEQUERY:
					RealmSceneFlags[lArray_Index] = (UBYTE) lData;
					break;
			}
			break;

		case	MP_REGENT:
			switch(lStruct_Id)
			{
				case	MPREG_NAME:
					// can't set a string, so just break;
					break;

				case	MPREG_REALM:
					regents[lArray_Index].mfSetRealm( (REALM::REALM_TYPE) lData);
					break;
				
				case	MPREG_RACE:
					regents[lArray_Index].mfSetRace((RACE_ENUM) lData);
					break;
				
				case	MPREG_CLASS1:
					regents[lArray_Index].mfSetClass1((CLASS_ENUM) lData);
					break;
				
				case	MPREG_LEVEL1:
					regents[lArray_Index].mfSetLevel1((unsigned int) lData);
					break;
				
				case	MPREG_CLASS2:
					regents[lArray_Index].mfSetClass2((CLASS_ENUM) lData);
					break;
				
				case	MPREG_LEVEL2:
					regents[lArray_Index].mfSetLevel2((unsigned int) lData);
					break;
				
				case	MPREG_BL_GRADE:
					regents[lArray_Index].mfSetBL_grade((BLOOD_GRADE) lData);
					break;
				
				case	MPREG_BL_DERIV:
					regents[lArray_Index].mfSetBL_deriv((BLOOD_DERIV) lData);
					break;
				
				case	MPREG_BL_STRENGTH:
					regents[lArray_Index].mfSetBL_strength((unsigned int) lData);
					break;
				
				case	MPREG_UNIT:
				 	regents[lArray_Index].mfSetunit((SHORT) lData);
					break;

				case	MPREG_ID:
				 	regents[lArray_Index].mfSetid((SHORT) lData);
					break;

				case	MPREG_ICON:
				 	regents[lArray_Index].mfSeticon((MAP_ICON) lData);
					break;

			}
			break;

		case	MP_PROVINCE:
			switch(lStruct_Id)
			{
				case	MPPROV_NAME:
					// can't set a string, so just break;
					break;

				case	MPPROV_X:
					province[lArray_Index].x = (SHORT) lData;
					break;

				case	MPPROV_Y:
					province[lArray_Index].y = (SHORT) lData;
					break;

				case	MPPROV_REALM:
					province[lArray_Index].Realm = (unsigned int) lData;
					break;

				case	MPPROV_CIVLEVEL:
					province[lArray_Index].CivLevel = (unsigned int) lData;
					break;

				case	MPPROV_CURCIVLEVEL:
					province[lArray_Index].CurCivLevel = (unsigned int) lData;
					break;

				case	MPPROV_MAGLEVEL:
					province[lArray_Index].MagLevel = (unsigned int) lData;
					break;

				case	MPPROV_TAXLEVEL:
					province[lArray_Index].TaxLevel = (unsigned int) lData;
					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						char temp[100];
						sprintf ( temp, "   multimap - prov:%ld  set TaxLevel:%ld",
							lArray_Index, lData);
						RandomLogComment ( temp );
					}
					break;

				case	MPPROV_LOYALTY:
					province[lArray_Index].Loyalty = (unsigned int) lData;
					break;

				case	MPPROV_CONTESTED:
					province[lArray_Index].Contested = (unsigned int) lData;
					break;

				case	MPPROV_OCCUPREALM:
					province[lArray_Index].OccupRealm = (unsigned int) lData;
					break;

				case	MPPROV_PILLAGED:
					province[lArray_Index].Pillaged = (unsigned int) lData;
					break;

				case	MPPROV_REDUCEHOLDS:
					province[lArray_Index].ReduceHolds = (unsigned int) lData;
					break;

				case	MPPROV_BLESSED:
					province[lArray_Index].Blessed = (unsigned int) lData;
					break;

				case	MPPROV_BLIGHTED:
					province[lArray_Index].Blighted = (unsigned int) lData;
					break;

				case	MPPROV_HONEST:
					province[lArray_Index].Honest = (unsigned int) lData;
					break;

				case	MPPROV_WARDED:
					province[lArray_Index].Warded = (unsigned int) lData;
					break;

				case	MPPROV_DISPEL:
					province[lArray_Index].Dispel = (unsigned int) lData;
					break;

				case	MPPROV_DISPELLVL:
					province[lArray_Index].DispelLvl = (unsigned int) lData;
					break;

				case	MPPROV_LAWCLAIMS:
					province[lArray_Index].LawClaims = (unsigned int) lData;
					break;

				case	MPPROV_CASTLESPTD:
					province[lArray_Index].CastleSptd = (unsigned int) lData;
					break;

				case	MPPROV_HUMANPOP:
					province[lArray_Index].HumanPop = (unsigned int) lData;
					break;

				case	MPPROV_ELFPOP:
					province[lArray_Index].ElfPop = (unsigned int) lData;
					break;

				case	MPPROV_DWARFPOP:
					province[lArray_Index].DwarfPop = (unsigned int) lData;
					break;

				case	MPPROV_GOBLINPOP:
					province[lArray_Index].GoblinPop = (unsigned int) lData;
					break;

				case	MPPROV_LEVYMUSTERED:
					province[lArray_Index].LevyMustered = (unsigned int) lData;
					break;

				case	MPPROV_TERRAIN:
					province[lArray_Index].Terrain = (unsigned int) lData;
					break;

				case	MPPROV_BORDER0:
				case	MPPROV_BORDER1:
				case	MPPROV_BORDER2:
				case	MPPROV_BORDER3:
				case	MPPROV_BORDER4:
				case	MPPROV_BORDER5:
				case	MPPROV_BORDER6:
					province[lArray_Index].Border[lStruct_Id-MPPROV_BORDER0].MoveCost = (unsigned int) lData;
					DrawRoads_((PROVINCE)lArray_Index, lStruct_Id-MPPROV_BORDER0);
					break;

				case	MPPROV_FIRSTPLACE:
					province[lArray_Index].FirstPlace = (SHORT) lData;
					break;

				case	MPPROV_INVESTED:
					#ifdef _WINDOWS
					EndWait( 0 );
					#endif
					ReconstructMap();
					CheckRealmStatus(lArray_Index, lData, TRUE );
					break;
			}
			break;

		case	MP_PLACES:
			switch(lStruct_Id)
			{
				case	MPPLACE_X:
					places[lArray_Index].x		  = (SHORT) lData;
					break;
				
				case	MPPLACE_Y:
					places[lArray_Index].y		  = (SHORT) lData;
					break;
				
				case	MPPLACE_REALM:
					places[lArray_Index].Realm	  = (unsigned int) lData;
					break;
				
				case	MPPLACE_LEVEL:
					places[lArray_Index].Level	  = (unsigned int) lData;
					break;
				
				case	MPPLACE_ICON:
					places[lArray_Index].Icon	  = (unsigned int) lData;
					break;
				
				case	MPPLACE_IROUTE1:
					places[lArray_Index].iRoute1	  = (unsigned int) lData;
					SetRedrawMainMapLevel();
					break;
				
				case	MPPLACE_IROUTE2:
					places[lArray_Index].iRoute2	  = (unsigned int) lData;
					SetRedrawMainMapLevel();
					break;
				
				case	MPPLACE_IROUTE3:
					places[lArray_Index].iRoute3	  = (unsigned int) lData;
					SetRedrawMainMapLevel();
					break;

				case	MPPLACE_BESIEGED:
					places[lArray_Index].Besieged = (SHORT) lData;
					break;

				case	MPPLACE_CONTESTED:
					places[lArray_Index].Contested = (SHORT) lData;
					break;
				
				case	MPPLACE_FSITEDISPLAYED:
					places[lArray_Index].fSiteDisplayed = (SHORT) lData;
					break;

				case	MPPLACE_INAME:
					places[lArray_Index].iName	  = (unsigned int) lData;
					break;
				
				case	MPPLACE_NEXTPLACE:
					places[lArray_Index].NextPlace = (SHORT) lData;
					break;
				
				case	MPPLACE_SCENE:
					places[lArray_Index].scene	  = (CSTRPTR) lData;
					break;
				
				case	MPPLACE_IICONBITM:
					sprintf(n,"UI\\%s.PCX",GameIcons[lData]);
					places[lArray_Index].iIconBitm = GetResourceStd(n, FALSE);
					#if fUSE_RES_FILES
					SetPurge(places[lArray_Index].iIconBitm);
					#endif
					break;

			}
			break;

		case	MP_UNITS:
			switch(lStruct_Id)
			{
				case	MPUNITS_PROVINCE:
					units[lArray_Index].province	 = (unsigned int) lData;
					SetRedrawMainMapLevel();
					break;

				case	MPUNITS_REALM:
					units[lArray_Index].Realm		 = (unsigned int) lData;
					break;

				case	MPUNITS_SALARY:
					units[lArray_Index].Salary		 = (unsigned int) lData;
					break;

				case	MPUNITS_ICON:
					units[lArray_Index].Icon		 = (unsigned int) lData;
					break;

				case	MPUNITS_MOVING:
					units[lArray_Index].Moving		 = (unsigned int) lData;
					break;

				case	MPUNITS_AVOIDMOVE:
					units[lArray_Index].AvoidMove	 = (unsigned int) lData;
					break;

				case	MPUNITS_JOINED:
					{
					units[lArray_Index].Joined		 = (unsigned int) lData;
					
					// This setting of the spotted flag relies on the NextUnit 
					// Value being set first.
					if (units[lArray_Index].Joined == TRUE)
					{
						LONG const iUnitLeader = GetUnitLeader(lArray_Index);
						if (lArray_Index != iUnitLeader)
						{
						    units[iUnitLeader].NotSpotted = units[lArray_Index].NotSpotted = (units[iUnitLeader].NotSpotted && units[lArray_Index].NotSpotted);
						}
					}
					else
					{
						LONG const iNextUnit = units[lArray_Index].NextUnit;
						if (iNextUnit > -1)
						{
						    LONG NotSpottedFlag = (units[iNextUnit].NotSpotted && units[lArray_Index].NotSpotted);
							LONG k;
							for (k = lArray_Index; k > -1; k = units[k].NextUnit)
							{
								units[k].NotSpotted = NotSpottedFlag;
							}
						}
					}
					}
					break;

				case	MPUNITS_NOTSPOTTED:
					units[lArray_Index].NotSpotted	 = (unsigned int) lData;
					break;

				case	MPUNITS_DISGUISED:
					units[lArray_Index].Disguised	 = (unsigned int) lData;
					break;

				case	MPUNITS_HASTENED:
					units[lArray_Index].Hastened	 = (unsigned int) lData;
					break;

				case	MPUNITS_BLESSED:
					units[lArray_Index].Blessed		 = (unsigned int) lData;
					break;

				case	MPUNITS_PAID:
					units[lArray_Index].Paid		 = (unsigned int) lData;
					break;

				case	MPUNITS_USE_REGENCY:
					units[lArray_Index].Use_Regency	 = (unsigned int) lData;
					break;

				case	MPUNITS_MOVEPART:
					units[lArray_Index].MovePart	 = (unsigned int) lData;
					break;

				case	MPUNITS_DESTPROVINCE:
					units[lArray_Index].DestProvince = (unsigned int) lData;
					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						if (lData)
						{
							char temp[100];
							sprintf ( temp, "   multimap - %s unit:%d DestProvince:%d",realm[units[lArray_Index].Realm].mfGetName(),lArray_Index,lData );
							RandomLogComment ( temp );
						}
					}
					SetRedrawMainMapLevel();
					break;

				case	MPUNITS_DESTPATH1:
					units[lArray_Index].DestPath1 = (unsigned int) lData;
					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						if (lData)
						{
							char temp[100];
							sprintf ( temp, "   multimap - DestPath1:%d",lData );
							RandomLogComment ( temp );
						}
					}
					break;

				case	MPUNITS_DESTPATH2:
					units[lArray_Index].DestPath2 = (unsigned int) lData;
					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						if (lData)
						{
							char temp[100];
							sprintf ( temp, "   multimap - DestPath2:%d",lData );
							RandomLogComment ( temp );
						}
					}
					break;

				case	MPUNITS_DESTPATH3:
					units[lArray_Index].DestPath3 = (unsigned int) lData;
					LOGCOMMENTSLEEP
					if ( fLogComment )
					{
						if (lData)
						{
							char temp[100];
							sprintf ( temp, "   multimap - DestPath3:%d",lData );
							RandomLogComment ( temp );
						}
					}
					break;

				case	MPUNITS_ID:
					units[lArray_Index].id			 = (SHORT) lData;
					break;

				case	MPUNITS_NEXTUNIT:
					units[lArray_Index].NextUnit	 = (SHORT) lData;
					break;

				case	MPUNITS_IICONBITM:

					sprintf(n,"UI\\%s.PCX",GameIcons[lData]);
					units[lArray_Index].iIconBitm = GetResourceStd(n, FALSE);
					#if fUSE_RES_FILES
					SetPurge(units[lArray_Index].iIconBitm);
					#endif
					break;

				case	MPUNITS_IHANDLE:
					units[lArray_Index].iHandle		 = (SHORT) lData;
					break;

			}
			break;

		case	MP_REALMREACTION:
			RealmReaction[lStruct_Id][lArray_Index] = (UBYTE) lData;
			break;

		case	MP_REALMSTATUS:
			RealmStatus[lStruct_Id][lArray_Index] = (UBYTE) lData;
			break;

		case	MP_TRIBUTE:
			Tribute[lStruct_Id][lArray_Index] = (UBYTE) lData;
			break;

		case	MP_GAMEDATA:
			switch (lStruct_Id)
			{
				case MPGAMESEMIPHORE_DECLAREDWAR:
					fPlayerNeverDeclaredWar = (BOOL) lData;
					break;

				case MPGAMESEMIPHORE_UNITINDEX:
					//units[lArray_Index].id = (SHORT) lData;
					units[0].id = (SHORT) lData;
					break;

				case MPGAMESEMIPHORE_PLACEINDEX:
					//places[lArray_Index].iIconBitm = (SHORT) lData;
					units[0].id = (SHORT) lData;
					break;
			}

			// -- store the semiphore data
			lGameSemiphore[lStruct_Id] = lData;
	}
}

/* ========================================================================
   Function    - DoGetGameData
   Description -
   Returns     - the data
   ======================================================================== */
LONG DoGetGameData(LONG lArray_Id, LONG lStruct_Id, LONG lArray_Index)
{
	LONG arg;

	switch(lArray_Id)
	{
		case	MP_ADVSITE:
			switch(lStruct_Id)
			{
				case	MPADV_AVAILABLE:
					return advsite[lArray_Index].available;
			}
			break;

		case	MP_REALM:
			switch(lStruct_Id)
			{
				case	MPRLM_NAME:
					// can't set a string, so just break;
					return 0L;

				case	MPRLM_X:
					return (LONG)realm[lArray_Index].mfGetNameX();

				case	MPRLM_Y:
					return (LONG)realm[lArray_Index].mfGetNameY();

				case	MPRLM_TITLESIZE:
					return (LONG)realm[lArray_Index].mfGetTitleSize();

				case	MPRLM_TITLESPREAD:
					return (LONG)realm[lArray_Index].mfGetTitleSpread();

				case	MPRLM_TITLETRANS:
					return (LONG)realm[lArray_Index].mfGetTitleTrans();

				case	MPRLM_REGENT:
					return (LONG)realm[lArray_Index].mfGetRegent();

//				case	MPRLM_COLOR1:
//					return (LONG)realm[lArray_Index].color1;

//				case	MPRLM_COLOR2:
//					return (LONG)realm[lArray_Index].color2;

				case	MPRLM_SHOWUNITS:
					return (LONG)realm[lArray_Index].mfShowUnits();

				case	MPRLM_SHOWHOLDINGS:
					return (LONG)realm[lArray_Index].mfShowHoldings();

				case	MPRLM_PLAYABLE:
					return (LONG)realm[lArray_Index].mfIsPlayable();

				case	MPRLM_PLAYERCTRL:
					return (LONG)realm[lArray_Index].mfIsPlayerCtrl();

				case	MPRLM_REGENCY:
					return (LONG)realm[lArray_Index].mfGetRegency();

				case	MPRLM_TREASURY:
					return (LONG)realm[lArray_Index].mfGetTreasury();

				case	MPRLM_COURT:
					return (LONG)realm[lArray_Index].mfGetCourt();

				case	MPRLM_FIRST_PROVINCE:
					return (LONG)realm[lArray_Index].mfGetFirstProvince();

				case	MPRLM_INCOME:
					return (LONG)realm[lArray_Index].mfGetIncome();

			}
			break;

		case	MP_REGENT:
			switch(lStruct_Id)
			{
				case	MPREG_NAME:
					// can't set a string, so just break;
					return 0L;

				case	MPREG_REALM:
					return (LONG)regents[lArray_Index].mfGetRealm();
				
				case	MPREG_RACE:
					return (LONG)regents[lArray_Index].mfGetRace();
				
				case	MPREG_CLASS1:
					return (LONG)regents[lArray_Index].mfGetClass1();
				
				case	MPREG_LEVEL1:
					return (LONG)regents[lArray_Index].mfGetLevel1();
				
				case	MPREG_CLASS2:
					return (LONG)regents[lArray_Index].mfGetClass2();
				
				case	MPREG_LEVEL2:
					return (LONG)regents[lArray_Index].mfGetLevel2();
				
				case	MPREG_BL_GRADE:
					return (LONG)regents[lArray_Index].mfGetBL_grade();
				
				case	MPREG_BL_DERIV:
					return (LONG)regents[lArray_Index].mfGetBL_deriv();
				
				case	MPREG_BL_STRENGTH:
					return (LONG)regents[lArray_Index].mfGetBL_strength();
				
				case	MPREG_UNIT:
				 	return (LONG)regents[lArray_Index].mfGetunit();

				case	MPREG_ID:
				 	return (LONG)regents[lArray_Index].mfGetid();

				case	MPREG_ICON:
				 	return (LONG)regents[lArray_Index].mfGeticon();

			}
			break;

		case	MP_PROVINCE:
			switch(lStruct_Id)
			{
				case	MPPROV_NAME:
					// can't set a string, so just break;
					return 0L;

				case	MPPROV_X:
					return (LONG)province[lArray_Index].x;

				case	MPPROV_Y:
					return (LONG)province[lArray_Index].y;

				case	MPPROV_REALM:
					return (LONG)province[lArray_Index].Realm;

				case	MPPROV_CIVLEVEL:
					return (LONG)province[lArray_Index].CivLevel;

				case	MPPROV_CURCIVLEVEL:
					return (LONG)province[lArray_Index].CurCivLevel;

				case	MPPROV_MAGLEVEL:
					return (LONG)province[lArray_Index].MagLevel;

				case	MPPROV_TAXLEVEL:
					return (LONG)province[lArray_Index].TaxLevel;

				case	MPPROV_LOYALTY:
					return (LONG)province[lArray_Index].Loyalty;

				case	MPPROV_CONTESTED:
					return (LONG)province[lArray_Index].Contested;

				case	MPPROV_OCCUPREALM:
					return (LONG)province[lArray_Index].OccupRealm;

				case	MPPROV_PILLAGED:
					return (LONG)province[lArray_Index].Pillaged;

				case	MPPROV_REDUCEHOLDS:
					return (LONG)province[lArray_Index].ReduceHolds;

				case	MPPROV_BLESSED:
					return (LONG)province[lArray_Index].Blessed;

				case	MPPROV_BLIGHTED:
					return (LONG)province[lArray_Index].Blighted;

				case	MPPROV_HONEST:
					return (LONG)province[lArray_Index].Honest;

				case	MPPROV_WARDED:
					return (LONG)province[lArray_Index].Warded;

				case	MPPROV_DISPEL:
					return (LONG)province[lArray_Index].Dispel;

				case	MPPROV_DISPELLVL:
					return (LONG)province[lArray_Index].DispelLvl;

				case	MPPROV_LAWCLAIMS:
					return (LONG)province[lArray_Index].LawClaims;

				case	MPPROV_CASTLESPTD:
					return (LONG)province[lArray_Index].CastleSptd;

				case	MPPROV_HUMANPOP:
					return (LONG)province[lArray_Index].HumanPop;

				case	MPPROV_ELFPOP:
					return (LONG)province[lArray_Index].ElfPop;

				case	MPPROV_DWARFPOP:
					return (LONG)province[lArray_Index].DwarfPop;

				case	MPPROV_GOBLINPOP:
					return (LONG)province[lArray_Index].GoblinPop;

				case	MPPROV_LEVYMUSTERED:
					return (LONG)province[lArray_Index].LevyMustered;

				case	MPPROV_TERRAIN:
					return (LONG)province[lArray_Index].Terrain;

				case	MPPROV_BORDER0:
				case	MPPROV_BORDER1:
				case	MPPROV_BORDER2:
				case	MPPROV_BORDER3:
				case	MPPROV_BORDER4:
				case	MPPROV_BORDER5:
				case	MPPROV_BORDER6:
					return 0L;

				case	MPPROV_FIRSTPLACE:
					return (LONG)province[lArray_Index].FirstPlace;
			}
			break;

		case	MP_PLACES:
			switch(lStruct_Id)
			{
				case	MPPLACE_X:
					return (LONG)places[lArray_Index].x;
				
				case	MPPLACE_Y:
					return (LONG)places[lArray_Index].y;
				
				case	MPPLACE_REALM:
					return (LONG)places[lArray_Index].Realm;
				
				case	MPPLACE_LEVEL:
					return (LONG)places[lArray_Index].Level;
				
				case	MPPLACE_ICON:
					return (LONG)places[lArray_Index].Icon;
				
				case	MPPLACE_IROUTE1:
					return (LONG)places[lArray_Index].iRoute1;
				
				case	MPPLACE_IROUTE2:
					return (LONG)places[lArray_Index].iRoute2;
				
				case	MPPLACE_IROUTE3:
					return (LONG)places[lArray_Index].iRoute3;
				
				case	MPPLACE_BESIEGED:
					return (LONG)places[lArray_Index].Besieged;

				case	MPPLACE_CONTESTED:
					return (LONG)places[lArray_Index].Contested;

				case	MPPLACE_FSITEDISPLAYED:
					return places[lArray_Index].fSiteDisplayed;

				case	MPPLACE_INAME:
					return (LONG)places[lArray_Index].iName;
				
				case	MPPLACE_NEXTPLACE:
					return (LONG)places[lArray_Index].NextPlace;
				
				case	MPPLACE_SCENE:
					return (LONG)places[lArray_Index].scene;
				
				case	MPPLACE_IICONBITM:
					return (LONG)places[lArray_Index].iIconBitm;

			}
			break;

		case	MP_UNITS:
			switch(lStruct_Id)
			{
				case	MPUNITS_PROVINCE:
					return (LONG)units[lArray_Index].province;

				case	MPUNITS_REALM:
					return (LONG)units[lArray_Index].Realm;

				case	MPUNITS_SALARY:
					return (LONG)units[lArray_Index].Salary;

				case	MPUNITS_ICON:
					return (LONG)units[lArray_Index].Icon;

				case	MPUNITS_MOVING:
					return (LONG)units[lArray_Index].Moving;

				case	MPUNITS_AVOIDMOVE:
					return (LONG)units[lArray_Index].AvoidMove;

				case	MPUNITS_JOINED:
					return (LONG)units[lArray_Index].Joined;

				case	MPUNITS_NOTSPOTTED:
					return (LONG)units[lArray_Index].NotSpotted;

				case	MPUNITS_DISGUISED:
					return (LONG)units[lArray_Index].Disguised;

				case	MPUNITS_HASTENED:
					return (LONG)units[lArray_Index].Hastened;

				case	MPUNITS_BLESSED:
					return (LONG)units[lArray_Index].Blessed;

				case	MPUNITS_PAID:
					return (LONG)units[lArray_Index].Paid;

				case	MPUNITS_USE_REGENCY:
					return (LONG)units[lArray_Index].Use_Regency;

				case	MPUNITS_MOVEPART:
					return (LONG)units[lArray_Index].MovePart;

				case	MPUNITS_DESTPROVINCE:
					return (LONG)units[lArray_Index].DestProvince;

				case	MPUNITS_DESTPATH1:
					return (LONG)units[lArray_Index].DestPath1;

				case	MPUNITS_DESTPATH2:
					return (LONG)units[lArray_Index].DestPath2;

				case	MPUNITS_DESTPATH3:
					return (LONG)units[lArray_Index].DestPath3;

				case	MPUNITS_ID:
					return (LONG)units[lArray_Index].id;

				case	MPUNITS_NEXTUNIT:
					return (LONG)units[lArray_Index].NextUnit;

				case	MPUNITS_IHANDLE:
					return (LONG)units[lArray_Index].iHandle;
			}
			break;

		case	MP_REALMREACTION:
			return (LONG)RealmReaction[lStruct_Id][lArray_Index];

		case	MP_REALMSTATUS:
			return (LONG)RealmStatus[lStruct_Id][lArray_Index];

		case	MP_TRIBUTE:
			return (LONG)Tribute[lStruct_Id][lArray_Index];

		case	MP_GAMEDATA:
			switch(lStruct_Id)
			{
				case MPGAMESEMIPHORE_DECLAREDWAR:
					return (LONG) fPlayerNeverDeclaredWar;

				case MPGAMESEMIPHORE_UNITINDEX:
#ifdef _WINDOWS
					if ( IsMultiPlayerMaster() )
					{
						RequestUnitsIndex(&arg, FALSE);
						return arg;
					}
#endif
					break;

				case MPGAMESEMIPHORE_PLACEINDEX:
#ifdef _WINDOWS
					if( IsMultiPlayerMaster() )
					{
						RequestPlacesIndex(&arg, FALSE);
						return arg;
					}
#endif
					break;
			}

			return lGameSemiphore[lStruct_Id];

	}

	// couldn't find the array type so return an error

	return 0xBADC0DE;

}



// ------------------------------------------------------------------
//
//   SetGameData()
//		
//                                                                              
//   Inputs:
//
//       fSend - means send to other players 
//
//   Returns: none
// ------------------------------------------------------------------

BOOL SetGameData( LONG lArray_Id, LONG lStruct_Id, LONG lArray_Index, 
				   LONG lData, BOOL fSend )
{
#ifdef _WINDOWS
	int timeNow;

	if( IsMultiPlayer() )
	{

// NEVER PUT A CHECK IN HERE SEEING IF THE DATA IS NOT DIFFERENT AND THEN
// EXIT THE ROUTINE BEFORE YOU PASS THE DATA TO OTHERS !!!!
// !@$!@%^@$#*&!@#

		DoSetGameData(lArray_Id, lStruct_Id, lArray_Index, lData);

		// now tell the other players about it, and leave

		if ( fSend )
		{		
			AMultiPlayer.BroadcastSetGameData(lArray_Id, lStruct_Id, lArray_Index, lData);
		}


		//---- Reset the semiphore after sending 

		if ( lArray_Id == MP_GAMEDATA && 
			 ( lStruct_Id == MPGAMESEMIPHORE_BATTLE_START ||
			   lStruct_Id == MPGAMESEMIPHORE_BATTLE_START1   )  )
		{
			lGameSemiphore[lStruct_Id] = -1;
		}

		return TRUE;

	}
	else
	{

		// single player game, just set the data
		DoSetGameData(lArray_Id, lStruct_Id, lArray_Index, lData);
		return TRUE;
	}

#else
	DoSetGameData(lArray_Id, lStruct_Id, lArray_Index, lData);
	return TRUE;
#endif

}	//---- End of SetGameData()



//@@@@@@  REMOVED by DLJ

#if 01
// ------------------------------------------------------------------
//
//   GetGameData()
//		
//                                                                              
//   Inputs:
//                                                                              
//   Returns: none
// ------------------------------------------------------------------

LONG GetGameData(LONG lArray_Id, LONG lStruct_Id, LONG lArray_Index)
{
#ifdef _WINDOWS
	int timeNow;

	if( IsMultiPlayer() )
	{
		if( AMultiPlayer.IsMaster() )
		{
			// return the requested data

			return DoGetGameData(lArray_Id, lStruct_Id, lArray_Index);
		}
		else
		{
			// clear old data
			//DoSetGameData(lArray_Id, lStruct_Id, lArray_Index, 0xBADC0DE);
			units[0].id = 0;

			// request this data be got by the master
			// send request for data to be retrieved
			AMultiPlayer.RequestGetGameData(lArray_Id, lStruct_Id, lArray_Index);

			// wait for the data
			while ( units[0].id == 0 )
			{
				AMultiPlayer.LookForMessages();
				Sleep(50);
			}

		}
	}
#endif

	return units[0].id;

}	//---- End of GetGameData()


#endif



#if 0   //REMOVED  by DLJ

/* ========================================================================
   Function    - PaintGenericStatement
   Description - Paint routine for generic statments
   Returns     - 
   ======================================================================== */
void PaintGenericStatement(LONG MenuCombo, LONG val)
{
	// set the statement text
	LONG	mx,my;
	LONG	xOff = 0;
	LONG	yOff = 0;
	LONG	MenuId, ButtonId;
	char	textbuf[1024];

	LONG X;
	LONG Y;
	LONG W;
	LONG H;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	X = X_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].X);
	Y = Y_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].Y);
	W = X_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].W);
	H = Y_RES_ADJ(Menus[MenuId].Buttons[QUESTION_TEXT].H);

	xOff = Menus[MenuId].Buttons[0].X;
	yOff = Menus[MenuId].Buttons[0].Y;
			
	// get the message to print
	switch(val)
	{
		case GD_AT_WAR:
			sprintf(textbuf, STRMGR_GetStr(STR_GM_WAR), realm[iVal1].mfGetName());
			break;
	}

	// paint the control buttons
	init_gfont(FONT_SANS_12PT);

	print_text_centered(
		X + xOff + (W/2),
		Y + yOff + (H/2) + 1,
		textbuf,
		Menus[MenuId].Buttons[QUESTION_TEXT].LabelColor
		);

}

#endif




#if 0  // @@@@@ commented this out dlj the real ones are below

/* ========================================================================
   Function    - RequestPlacesCountandIndex
   Description - 
   Returns     - 
   ======================================================================== */
void RequestPlacesIndex(LONG *index, BOOL fSend)
{
	LONG mycount, mytmp, arg;

//#ifdef _WINDOWS
	// if we are the master, or a single player game
	// just return the index, count, and flag
//	if((IsMultiPlayer() && AMultiPlayer.IsMaster()) || !IsMultiPlayer())
//	{
//#endif


		// find the place to put the new holding(the end of places array)
		for (mytmp=0; mytmp<MAX_PLACES && places[mytmp].Realm != REALM::NO_COUNTRY; mytmp++){}


//		WHY IS THIS DONE??? .Realm is unsigned, this just sets it to 127!!!
//		if (mytmp != MAX_PLACES)
//			places[mytmp].Realm = -1;


		*index = mytmp;


//#ifdef _WINDOWS
//	}
//	else
//	{
//		mytmp = GetGameData(MP_GAMEDATA, MPGAMESEMIPHORE_PLACEINDEX, 0);
//		*index = mytmp;
//	}
//#endif

}

#endif





/* ========================================================================
   Function    - RequestPlacesCountandIndex
   Description - find the place to put the new holding(the end of places array)
	Returns     - 
   ======================================================================== */
void RequestPlacesIndex(LONG *index, BOOL fSend)
{
	LONG mycount, mytmp, arg;

#ifdef _WINDOWS
	// if we are the master or a single player game or an NPC
	if ((IsMultiPlayer() && AMultiPlayer.IsMaster()) || !IsMultiPlayer() || fSend==FALSE)
	{
		for (mytmp=0; mytmp < MAX_PLACES && places[mytmp].Realm != REALM::NO_COUNTRY; mytmp++)
		{ ; }		// no body

		if (mytmp < MAX_PLACES)							// mark this as allocated
			places[mytmp].Realm = REALM::MIERES;	// just != REALM::NO_COUNTRY

		*index = mytmp;
	}

	// slave in a multiplayer game
	else
	{
		mytmp = GetGameData(MP_GAMEDATA, MPGAMESEMIPHORE_PLACEINDEX, 0);
		*index = mytmp;
	}

#else
	// single player game
	for (mytmp=0; mytmp < MAX_PLACES && places[mytmp].Realm != REALM::NO_COUNTRY; mytmp++){}
	*index = mytmp;
#endif

}





static LONG iNewResult;


/* ========================================================================
   Function    - SendMyMods
   Description - the remote computer will call this function to send the 
                 new modified chance
   Returns     - 
   ======================================================================== */
void SendMyMods(LONG iWhichAction, LONG iWhichProvince, LONG iWhichHold, LONG modRegency)
{

#ifdef _WINDOWS

	LONG iWhichRealm;

	if ( IsMultiPlayer() )
	{
		// get the realm of the remote player
		iWhichRealm = AMultiPlayer.GetRealmFromId(response[curResponse].lFromId);

		#ifdef _MULTIMONO
			mono_printf(0,10,"SendMods %d to %s              ", modRegency, realm[iWhichRealm].mfGetName());
		#endif

		// send the final result of the roll to the remote computer
	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	if ( fLogComment )
   {
		char temp[100];
		sprintf ( temp, "SendMyMods: SendEvent");
		RandomLogComment ( temp );
	}
	#endif

		AMultiPlayer.SendEvent(iWhichAction, modRegency, 0, 0, FALSE, iWhichRealm);
		AMultiPlayer.InitEvent(0);
	}
#endif
}



/* ========================================================================
   Function    - SendResult
   Description - 
      the computer rolling for success will call this function after the final
      result is computed with success or failure
   Returns     - 
   ======================================================================== */
void SendResult(LONG iWhichAction, LONG iWhichProvince, LONG iWhichHolding, LONG iResult)
{
	LONG iWhichRealm;

	// find the realm of the target
	if(iWhichProvince)
	{
		iWhichRealm = province[iWhichProvince].Realm;
	}
	else	// get the realm from the holding
	{
		iWhichRealm = places[iWhichHolding].Realm;
	}

	// is this target a player, if so send the data, else set global data

	if( realm[iWhichRealm].mfIsPlayerCtrl() && iWhichRealm != HomeRealm )
	{
#ifdef _WINDOWS
		// should not be here if not multi player, but check anyway
		if ( IsMultiPlayer() )
		{
			#ifdef _MULTIMONO
				mono_printf(0,11,"SendResult %d to %s              ", iResult, realm[iWhichRealm].mfGetName());
			#endif

			// send the final result of the roll to the remote computer
	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	if ( fLogComment )
   {
		char temp[100];
		sprintf ( temp, "SendResult: SendEvent");
		RandomLogComment ( temp );
	}
	#endif
			AMultiPlayer.SendEvent(iWhichAction, iResult, 0, 0, FALSE, iWhichRealm);
			AMultiPlayer.InitEvent(0);
		}
#endif
	}
	else
	{
		// ai controlled realm
		// set the result so the wait can return it
		iNewResult = iResult;
	}
}

/* ========================================================================
   Function    - WaitForResult
   Description - 
      the remote computer will call this function to get the final result
      back from the computer who asked for the action to occur
      returning success or failure
   Returns     - 
   ======================================================================== */
LONG WaitForResult(LONG iWhichAction, LONG iWhichProvince, LONG iWhichHolding)
{
	int  iWhichRealm;
	LONG lPlayerId;

#ifdef _WINDOWS
	// get the realm of the remote player
	if ( IsMultiPlayer() )
		iWhichRealm = AMultiPlayer.GetRealmFromId(response[curResponse].lFromId);
	else
#endif
		iWhichRealm = response[curResponse].lFromId;

	// is this target a player, if so send the data, else leave

	if( realm[iWhichRealm].mfIsPlayerCtrl() && iWhichRealm != HomeRealm )
	{
#ifdef _WINDOWS
		if ( IsMultiPlayer() )
		{
			// get the players id

			lPlayerId = lRealmSelectedByPlayer[iWhichRealm];
	
			// wait for a reply
			while(TRUE)
			{
				if ( AMultiPlayer.HaveEvent() == iWhichAction )
				{
					iNewResult = EventTable.data1;
					#ifdef _MULTIMONO
						mono_printf(40,10,"Got Result  %d             ", iNewResult);
					#endif
					AMultiPlayer.InitEvent(0);
					return iNewResult;
				}
	
				// get messages and give time up to the system

				AMultiPlayer.LookForMessages();   //---- Update not specific messages

       		WindowsMessages();

//				Sleep(50);

				Sleep(0);

				#ifdef _DEBUG
				if ( GetAsyncKeyState( VK_F10 ) & 0x8000 )
				{
					AMultiPlayer.Finalize();
					return iNewResult;
				}
				#endif

				//---- Person disconnected 

				if ( !realm[iWhichRealm].mfIsPlayerCtrl() )
				{
					return iNewResult;
				}

			}
		}
#endif
	}
	else
	{
		// ai controlled player, just send back the result sent above
		return iNewResult;
	}
	return iNewResult;
}


LONG iGlobRealm;

/* ========================================================================
   Function    - AskAndWaitForTargetPlayer
   Description - 
      the computer rolling for success will call this function
      it will notify the other computer if controlled by a human
      then return with the new success when the other computer rolls
   Returns     - 
   ======================================================================== */
#define MDYELLOW					168
LONG AskAndWaitForTargetPlayer(LONG iWhichAction, LONG iWhichProvince, LONG iWhichHolding, LONG iSuccess, LONG x, LONG y)
{
	LONG	i, j, iWhichRealm, count;
	LONG	iNewSuccess = iSuccess;
	LONG	timer, timer2;
	BOOL	fFirstTime = FALSE;
	int	iPlayerId;
	static LONG oldSuccess = 999;


	// find the realm of the target
	if(iWhichAction == DIPLOMACY)
	{
		iWhichRealm = iGlobRealm;
	}
	else
	{
		if(iWhichProvince)
		{
			iWhichRealm = province[iWhichProvince].Realm;
		}
		else	// get the realm from the holding
		{
			iWhichRealm = places[iWhichHolding].Realm;
		}
	}

	// is the realm target player controlled?
	if(realm[iWhichRealm].mfIsPlayerCtrl() && iWhichRealm != HomeRealm )
	{
#ifdef _WINDOWS
		if ( IsMultiPlayer() )
		{
			// put up a wait cursor
	
			// get the players id
			iPlayerId = lRealmSelectedByPlayer[iWhichRealm];
	
			// add a sound "Waiting for remote player"
			AddSndObj((BIRTHRT_SND)SND_UI_WAITING_FOR_AN_OPPONENT1,0,VOLUME_NINETY);
	
			#ifdef _MULTIMONO
				mono_printf(0,12,"SendInitial to %s              ", realm[iWhichRealm].mfGetName());
			#endif

			// send a message to the target, asking him to put up a dialog box
			// for rolling a reply
			RealmSceneFlags[HomeRealm] = 99;			// clear response semiphore
			timer = 0;
			timer2 = get_time() + (90 * 18);			// max wait 90 sec.

			while (TRUE)
			{
				// get messages and give time up to the system
				AMultiPlayer.LookForMessages();   //---- Update not specific messages
       		WindowsMessages();
				Sleep(100);
				RunRequests();

				// yell every 7 seconds until heard
				if (timer < get_time() && RealmSceneFlags[HomeRealm] == 99)
				{
					if (timer2 < get_time())			// max wait 90 sec.
						break;
					timer = get_time() + (18*7);		// wait 7 seconds

	#ifdef _WINDOWS
	LOGCOMMENTSLEEP
	if ( fLogComment )
   {
		char temp[100];
		sprintf ( temp, "AskAndWaitForTargetPlayer: SendEvent");
		RandomLogComment ( temp );
	}
	#endif
					AMultiPlayer.SendEvent(iWhichAction, iWhichProvince, iWhichHolding, iSuccess, TRUE, iWhichRealm);
					AMultiPlayer.InitEvent(0);
				}

				// wait for mods to return
				if ( AMultiPlayer.HaveEvent() == iWhichAction )
				{
					//iNewSuccess = EventTable.data1;
					if (iWhichAction == DIPLOMACY)
					{
						iNewSuccess = EventTable.data1;
					}
					else
					{
						iNewSuccess = iSuccess + EventTable.data1;
					}
					#ifdef _MULTIMONO
						mono_printf(40,12,"Got initial reply  %d     ", iNewSuccess);
					#endif
					AMultiPlayer.InitEvent(0);
					break;
				}

				HandleMultiPlayerEvents();

				//if ( iTTimeLimit != IX_TT_NONE )
				//{
				//	LONG time_;
				//	CalcTimerBar();
				//	RedrawProcessPanel();
				//	time_ = (LONG)get_time() - (LONG)WaitingWhen - (LONG)lTimeLimit[iTTimeLimit];
				//	if (time_ > 0 && time_ < (10*18) )
				//		break;
				//}

				#ifdef _DEBUG
            if ( GetAsyncKeyState( VK_F10 ) & 0x8000 )
            {
               AMultiPlayer.Finalize();
					return iNewSuccess;
            }
				#endif

				//---- Person disconnected 
				if ( !realm[iWhichRealm].mfIsPlayerCtrl() )
				{
					return iNewSuccess;	//@@@@ is this right ????
				}
			}
			return iNewSuccess;
		}

		// put back normal cursor
#endif
	}

	// ai success roll modifier
	else if (iWhichRealm != HomeRealm)
	{
		if (modOpponent==0)
		{
			oldSuccess = 999;
			fFirstTime = TRUE;
		}

		i = 0;
		switch (iWhichAction)
		{
			case RULE:
				break;
			case AGITATE:
			case TRADE_ROUTE:
				if (random(3)==0 && iSuccess < 15 && iSuccess!=oldSuccess)
				{
					i = SpendNPCRegency(iWhichRealm, random(10), TRUE);
					modOpponent += i;
					if (i) CancelRoll();
				}
				break;
			case CONTEST:
			case CREATE_HOLDING:
			case DIPLOMACY:
			case FORGE_LEY_LINE:
				if (random(2) && iSuccess < 15 && iSuccess!=oldSuccess)
				{
					i = SpendNPCRegency(iWhichRealm, random(20-iSuccess), TRUE);
					modOpponent += i;
					if (i) CancelRoll();
				}
				break;
			case INVESTITURE:
				count = 0;
				for(i = 1; i<PROVINCE_COUNT; i++)
					if(province[i].Realm == iWhichRealm)
						count++;

				if (iSuccess == oldSuccess)
				{
					i = 0;	// if player didn't spin up, don't spin down
				}
				// try VERY hard to prevent loss of last prov
				else if (count == 1 && realm[iWhichRealm].mfGetRegency() > 0)
				{
					i = SpendNPCRegency(iWhichRealm, 19-iSuccess, TRUE);
				}
				// other than the last prov
				else if ( ( (game_difficulty==DIF_EASY && random(100) < 55)
							|| (game_difficulty==DIF_MEDIUM && random(100) < 70)
							|| (game_difficulty==DIF_HARD && random(100) < 85) )
						&& iSuccess < 16)					// don't fight if chance is < 25%
				{
					i = SpendNPCRegency(iWhichRealm, random(MAX(iSuccess-10,0))+(MAX(10-iSuccess,0)), TRUE);
				}
				else
				{
					i = 0;  // die from boredom
				}

				if (i)
				{
					modOpponent += i;
					CancelRoll();
				}
				break;
		}
		oldSuccess = iSuccess;
		if (i)
		{
			// add a sound "Waiting for opponent"
			if (fFirstTime)
			{
//				print_textf(x+1, y+6, DKBROWN, "^F21^c%s",realm[iWhichRealm].mfGetName());
//				print_textf(x, y+5, MDYELLOW, "^c%s",realm[iWhichRealm].mfGetName());

				AddSndObj((BIRTHRT_SND)SND_UI_WAITING_FOR_AN_OPPONENT1,0,VOLUME_NINETY);
				j = get_time()+20;
				while (j>get_time())
					ServiceSOLAudio();
			}
			AddSndObj((BIRTHRT_SND)SND_UI_CHECK_BOX,0,VOLUME_NINETY);
		}

	}

	return iSuccess;
}



#ifdef _WINDOWS
BOOL IsMulPlayerandRealm(int realm)
{
	return AMultiPlayer.IsMultiPlayerandRealm(realm);
}

/* ========================================================================
   Function    - WaitForSemiphore
   Description - Wait for a specific game semiphore
   Returns     - Value in the semiphore
   ======================================================================== */
LONG WaitForSemiphore(LONG iWhichSemiphore)
{
	LONG val;
	static int count = 0;

	// wait for the semiphore to be set
	while( lGameSemiphore[iWhichSemiphore] < 1 )
	{
		count++;
		#ifdef _MULTIMONO
			mono_printf(0,0,"WaitForSemiphore %d count %d", iWhichSemiphore, count );
		#endif

		AMultiPlayer.LookForMessages();

		WindowsMessages();

		#ifdef _DEBUG
		if ( GetAsyncKeyState( VK_F10 ) & 0x8000 )
		{
 			AMultiPlayer.Finalize();
 			return 0;
		}
		#endif

		Sleep(0);
	}

	val = lGameSemiphore[iWhichSemiphore];

	#ifdef _MULTIMONO
		mono_printf(0,4,"Got Semiphore %d data %d", iWhichSemiphore, val);
	#endif

	// reset the semiphore when done

	lGameSemiphore[iWhichSemiphore] = -1;

	return val;

}
#endif



/* ========================================================================
   Function    - HandleMultiPlayerEvents
   Description - 
   Returns     - 
   ======================================================================== */
void HandleMultiPlayerEvents (void)
{

#ifdef _WINDOWS
	if ( IsMultiPlayer() )
	{
		int iEvent;

		iEvent = AMultiPlayer.HaveEvent();

		//---- If we have a question from someone throw up box
		if ( iEvent == iMPE_ADVENTURE )
		{
			ShowMultiQuestion(0,0);
		}
		else if (iEvent == iMPE_ADD_ITEM)
		{
			MultiAddItem(EventTable.data1, EventTable.data2, EventTable.data3);
		}
		else if (iEvent == iMPE_DROP_ITEM)
		{
			MultiDropItem(EventTable.data1, EventTable.data2);
		}
		else if ( iEvent )
		{
			//@@@@@@ MDB OTHER EVENTS GO HERE 
			if (EventReply[0].EventId == iMPE_ADD_ITEM)
				MultiAddItem(EventTable.data1, EventTable.data2, EventTable.data3);
			else if (EventReply[0].EventId == iMPE_DROP_ITEM)
				MultiDropItem(EventTable.data1, EventTable.data2);
			else if (EventTable.data4)
			{
				// put up dialog box
				DisplayOthersAction(EventTable.dwFrom, EventReply[0].EventId, EventTable.data1, EventTable.data2, EventTable.data3);
				EventReply[0].EventId = 0;		// stop adding displays
			}
			else if( iEvent >= IMMEDIATE)
			{
				AddReport(iEvent-IMMEDIATE, EventTable.data1, EventTable.data2, EventTable.data3);
				AMultiPlayer.InitEvent();
			}
		}
	}
#endif

}

/* ========================================================================
   Function    - CheckSum
   Description - Given a memory block, compute a checksum to see if it has
   				 changed.
   Returns     - a checksum value.
   ======================================================================== */
ULONG CheckSum (char * p, ULONG l)
{
	ULONG *	pl = (ULONG *)p;
	ULONG		ls;
	ULONG		j;
	ULONG		Result = 0;

	// Add four bytes at a time ignoring overflow.
	ls = l / sizeof(ULONG);
	for (j=0; j < ls; j++)
		Result += pl[j];

	// Add in the last 1-3 bytes if necessary.
	ls = l % sizeof(ULONG);
	if (ls)
		for (j=0; j < ls; j++)
			Result += p[j];

	return Result;
}


/* ========================================================================
   Function    - DoMultiplayerDataIntegrityCheck
   Description - 
   Returns     - 
   ======================================================================== */
void DoMultiplayerDataIntegrityCheck ()
{
#if 0

	ULONG		Result = 0;

	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		char temp[200];
		sprintf ( temp, "---------------------------------------------------------------");
		RandomLogComment ( temp );
	}

	//Result += CheckSum ( (char *)units, sizeof(UNIT_TYPE)*MAX_UNITS );
	Result = CheckSum ( (char *)units, sizeof(UNIT_TYPE)*MAX_UNITS );
	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		char temp[200];
		sprintf ( temp, "DoMultiplayerDataIntegrityCheck - player #%ld  units result:%ld ------", AMultiPlayer.GetPlayerId(), Result);
		RandomLogComment ( temp );
	}

	//Result += CheckSum ( (char *)places, sizeof(PLACE_TYPE)*MAX_PLACES );
	Result = CheckSum ( (char *)places, sizeof(PLACE_TYPE)*MAX_PLACES );
	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		char temp[200];
		sprintf ( temp, "DoMultiplayerDataIntegrityCheck - player #%ld  places result:%ld ------", AMultiPlayer.GetPlayerId(), Result);
		RandomLogComment ( temp );
	}

	//Result += CheckSum ( (char *)province, sizeof(PROV_TYPE)*MAX_PROVS );
	Result = CheckSum ( (char *)province, sizeof(PROV_TYPE)*MAX_PROVS );
	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		char temp[200];
		sprintf ( temp, "DoMultiplayerDataIntegrityCheck - player #%ld  province result:%ld ------", AMultiPlayer.GetPlayerId(), Result);
		RandomLogComment ( temp );
	}

	//Result += CheckSum ( (char *)realm, sizeof(REALM_STRUCT)*REALM::REALM_COUNT );
	Result = CheckSum ( (char *)realm, sizeof(REALM_STRUCT)*REALM::REALM_COUNT );
	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		char temp[200];
		sprintf ( temp, "DoMultiplayerDataIntegrityCheck - player #%ld  realm result:%ld ------", AMultiPlayer.GetPlayerId(), Result);
		RandomLogComment ( temp );
	}

	//Result += CheckSum ( (char *)regents, sizeof(REGENT)*REGENT_COUNT );
	Result = CheckSum ( (char *)regents, sizeof(REGENT)*REGENT_COUNT );
	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		char temp[200];
		sprintf ( temp, "DoMultiplayerDataIntegrityCheck - player #%ld  regent result:%ld ------", AMultiPlayer.GetPlayerId(), Result);
		RandomLogComment ( temp );
	}

	LOGCOMMENTSLEEP
	if ( fLogComment )
	{
		char temp[200];
		sprintf ( temp, "---------------------------------------------------------------");
		RandomLogComment ( temp );
	}

#endif
}

/*	======================================================================== */

