/* =®RM254¯=======================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: UNITS.hxx  -
   Author:   Alan Clark
   
   ======================================================================== */
#if !defined (_UNITS_HXX)
#define _UNITS_HXX 1


#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_VECTOR_HXX)
#include "vector.hxx"
#endif

#if !defined(_MAPICON_HXX)
#include "mapicon.hxx"
#endif

#if !defined(_PROVDATA_HXX)
#include "provdata.hxx"
#endif

#if !defined(_REALM_HXX)
#include "realm.hxx"
#endif

//----------------------------------
//	Defines
//----------------------------------
#ifdef _JUNEDEMO
#define MAX_UNITS			1000					// cause alan said so
#define NPC_MAX_UNITS	 800					// cause alan said so
#define PER_PLAYER_RESERVED_UNITS	125
#else
#define MAX_UNITS			2000					// cause alan said so
#define NPC_MAX_UNITS	1000					// cause alan said so
#define PER_PLAYER_RESERVED_UNITS	125
#endif

#define NO_NAME		0

//----------------------------------
//	Units
//----------------------------------
typedef struct {
	unsigned int		province		: 8;		// current location
	unsigned int		Realm			: 7;
	unsigned int		Salary		: 7;
	unsigned int		Icon			: 7;
	unsigned int		Moving		: 1;		// Unused.
	unsigned int		AvoidMove	: 1;
	unsigned int		Joined		: 1;
	unsigned int		NotSpotted	: 1;
	unsigned int		Disguised	: 1;
	unsigned int		Hastened		: 1;
	unsigned int		Blessed		: 1;
	unsigned int		Paid			: 1;
	unsigned int		Use_Regency	: 1;
	unsigned int		MovePart		: 5;
	unsigned int		DestProvince: 8;		// destination province for movement
	unsigned int		DestPath1	: 4;
	unsigned int		DestPath2	: 4;
	unsigned int		DestPath3	: 4;

	SHORT					id;						// the avatar Id associated with
	SHORT					NextUnit;				// what is my followers index
	union {
	SHORT					iIconBitm;				// Resource handle to the map icon
	SHORT					iHandle;					// Handle to the Avatar memory.
	};
} UNIT_TYPE, *UNIT_PTR;


extern DECL_VECTOR_CLASS(UNIT_TYPE, units);

/* -----------------------------------------------------------------
   Prototypes
   ----------------------------------------------------------------- */
void ValidateUnitsArray(void);
BOOL ValidateUnitList(SHORT UnitIndex);
BOOL ValidateUnit(SHORT UnitIndex);
BOOL CheckUnitStack (LONG iUnit, BOOL fSend );
LONG JoinUnits_sub (LONG iUnit, LONG iUnit2, BOOL fSend );
void JoinUnits (LONG iUnit, LONG iUnit2);
LONG DeleteUnit (SHORT iUnit, BOOL fSend, BOOL ReOrderStack = TRUE );
LONG CheckLeader (LONG iOrgUnitLeader, BOOL fSend );
LONG CalcUnitValues (LONG iUnit);
void MoveSubUnitUpList (LONG iUnit, LONG iStartUnit);
LONG CreateUnit (PROVINCE prov, MAP_ICON icon, LONG id, REALM::REALM_TYPE realm, BOOL fSend );
LONG CreateUnitNJ (PROVINCE prov, MAP_ICON icon, LONG id, REALM::REALM_TYPE realm, BOOL fSend, BOOL fJoin );
void RequestUnitsIndex(LONG *index, BOOL fSend);
PROVINCE UnitFinalProvince (LONG i);
//void TeleportUnits(void);
void TransportUnit(LONG iUnit, PROVINCE prov, BOOL fSend);
LONG MakeUnitSingleStack(LONG iUnit, BOOL fSend);
BOOL JoinUnitToOthersInProvince(LONG iUnit, PROVINCE prov, BOOL fSend);
LONG GetUnitLeader(LONG iUnit);
LONG CountUnitStack(LONG iUnit);
BOOL CanISeeAnyoneInThisProv(REALM::REALM_TYPE /* Realm */, PROVINCE /* prov */);
void MarkUnitsSpotted(PROVINCE prov);

#endif // _UNITS_HXX
/*	======================================================================== */
