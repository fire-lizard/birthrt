/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: REGENTS.cpp (part of GAMEMAP.cpp)
   Author:   Alan Clark
   ========================================================================
   Contains the following general functions:
   ======================================================================= */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "typedefs.h"
#include "system.h"
#include "units.hxx"
#include "regents.hxx"
#include "gamemap.hxx"
#include "vector.hxx"
#include "realm.hxx"
#include "playstat.hxx"
#include "handle.hxx"

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ========================================================================
   Function    - TransferRegentIconsToUnitsArray
   Description - Transfer the regent icons to the units array (duhhh)
   Returns     - void
   ======================================================================== */
void TransferRegentIconsToUnitsArray(void)
{
	SHORT i;
	
	for(i=0;i<CHARACTER_COUNT;i++)
	{
#if 0
		if( regents[i].mfGetRealm() == HomeRealm ||
			realm[regents[i].mfGetRealm()].mfIsPlayerCtrl() )  // or remote player
		{
			REALM_STRUCT_PTR	pRealm  = &realm[HomeRealm];
			SHORT				RegentID = pRealm->mfGetRegent();
			
			// if this is the new PC regent make sure
			// his icon is a regent icon
			if(RegentID == i)
			{
				if (regents[i].mfGeticon() >= REG_LORDM3_UNIT_ICON)
				{
					regents[i].mfSeticon(regents[i].mfGeticon() - 17);
				}
			}
			else
			{
				if (regents[i].mfGeticon() < REG_LORDM3_UNIT_ICON)
				{
					regents[i].mfSeticon(regents[i].mfGeticon() + 17);
				}
			}
		}
#endif
		
		if(regents[i].mfGetunit() > 0)
		{
			if (IsLieutenant(regents[i].mfGetunit()) &&
				regents[i].mfGeticon() >= REG_LORDM1_UNIT_ICON &&	// Using a regent icon
				regents[i].mfGeticon() <= REG_GLADY1_UNIT_ICON)
			{
				// translate to an equivilent lieutenant icon.
				regents[i].mfSeticon((MAP_ICON)(LET_LORDM1_UNIT_ICON + 
				(regents[i].mfGeticon() - REG_LORDM1_UNIT_ICON)));
			}

			units[regents[i].mfGetunit()].Icon = regents[i].mfGeticon();
		}
	}
}


/* ========================================================================
   Function    - GetNextUnitInstance
   Description - Helper function for LinkUnits and ProvinceIsLinked:  This
                 function goes scans the units array, given a starting index,
                 and looks for the next unjoined unit in a given province.
   Returns     - The next units index, MAX_UNITS if no unit was found
   ======================================================================== */
static LONG GetNextUnitInstance(
	REALM::REALM_TYPE MatchingRealm, 
	PROVINCE Prov,
	LONG startIdx
)
{
	LONG i;

	for (i=startIdx;i<MAX_UNITS;i++)
	{
		if (units[i].Realm == MatchingRealm 
			&& units[i].province==Prov 
			&& units[i].Joined==0)
			break;
	}
			return i;
}
	


/* ========================================================================
   Function    - ProvinceIsLinked
   Description - determines whether there are two armies in the units table
                 that are in the same province, yet aren't linked.
   Returns     - TRUE is the province is all linked correctly.
   ======================================================================== */
static BOOL ProvinceIsLinked(REALM::REALM_TYPE MatchingRealm, PROVINCE Prov)
{
	LONG Unit1,Unit2;

	//find first instance of that Province
	Unit1=GetNextUnitInstance(MatchingRealm, Prov,1);
	if (Unit1==MAX_UNITS)
		return TRUE;  //no armies for that prov so it's not un-linked...


	//find next instance
	Unit2=GetNextUnitInstance(MatchingRealm, Prov,Unit1+1);
	if (Unit2==MAX_UNITS)
		return TRUE;  //no second instance, therefore the prov's armies are linked

	return FALSE;
}
/* ========================================================================
   Function    - LinkUnits
   Description - Goes through provinces and links their units
               - Used only at start of game to init stacks
   Returns     - void
   ======================================================================== */
void LinkUnits (BOOL)
{
	REALM::REALM_TYPE	WhichRealm;
	PROVINCE				WhichProv;
	LONG i;

	for (WhichRealm=REALM::FIRST_REALM;WhichRealm<LAND_REALM_COUNT;WhichRealm = (REALM::REALM_TYPE)(WhichRealm + 1))
	{
		for (WhichProv=FIRST_PROVINCE;WhichProv<MAX_PROVS;WhichProv=(PROVINCE)(WhichProv + 1))
		{
			while (!ProvinceIsLinked(WhichRealm, WhichProv))
			{
				LONG Unit1,Unit2;
				
				Unit1 = GetNextUnitInstance(WhichRealm,WhichProv, 1);
				Unit2 = GetNextUnitInstance(WhichRealm,WhichProv, Unit1+1);
				
			   JoinUnits_sub(Unit1, Unit2, FALSE);
			}
		}
	}

	for (i=1; i<MAX_UNITS; i++)
		if (units[i].Joined == FALSE)
			CheckUnitStack(i, FALSE);				// check for oversize stack
}

/* ========================================================================
   Function    - RegentUnitOfRealm
   Description - return the regent of the given realm
   Returns     - unit index number
   ======================================================================== */
SHORT RegentUnitOfRealm(REALM::REALM_TYPE RealmId)
{
	SHORT unitId = 0;
	
	//i = RealmId * 2 - 1;
	
	// if this is not the regent, next guy is

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// NO LONGER WORKS AS A TEST OF WHO THE REGENT IS
//	if (regents[i].mfGeticon() >= REG_LORDM3_UNIT_ICON)
//		i++;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (RealmId <= REALM::REALM_COUNT)
	{
		LONG iRegentIndex = realm[RealmId].mfGetRegent();
	
		if(regents[iRegentIndex].mfGetunit() > 0)
			unitId = regents[iRegentIndex].mfGetunit();
	}

	return unitId;
}

/* ========================================================================
   Function    - IsRegent
   Description - Checks whether a unit is a regent.
   Returns     - TRUE if unit is a regent, FALSE if not.
   ======================================================================== */
BOOL IsRegent (LONG UnitIndex)
{
	// GWP return (units[UnitIndex].Icon >= REG_M_FTR_UNIT_ICON &&
	// GWP  	units[UnitIndex].Icon <= REG_GOB_FTR_UNIT_ICON) ||
	// GWP  	units[UnitIndex].Icon == SPIDERKING_UNIT_ICON;
	
	return (RegentUnitOfRealm((REALM::REALM_TYPE)units[UnitIndex].Realm) == UnitIndex);
}


/* ========================================================================
   Function    - IsLieutenant
   Description - Checks whether a unit is a regent.
   Returns     - TRUE if unit is a regent, FALSE if not.
   ======================================================================== */
BOOL IsLieutenant (LONG UnitIndex)
{
	// GWP return (units[UnitIndex].Icon >= REG_M_FTR_UNIT_ICON &&
	// GWP  	units[UnitIndex].Icon <= REG_GOB_FTR_UNIT_ICON) ||
	// GWP  	units[UnitIndex].Icon == SPIDERKING_UNIT_ICON;
	
	return (RegentUnitOfRealm((REALM::REALM_TYPE)units[UnitIndex].Realm) != UnitIndex &&
	        units[UnitIndex].id > 1000);
}


/* ========================================================================
   Function    - IsRegentOrLieutenant
   Description - Checks whether a unit is a regent or a lieutenant.
   Returns     - TRUE if unit is a regent or lieutenant, FALSE if not.
   ======================================================================== */
BOOL IsRegentOrLieutenant (LONG UnitIndex)
{
	// GWP return (units[UnitIndex].Icon >= REG_M_FTR_UNIT_ICON &&
	// GWP 	units[UnitIndex].Icon <= GOB_FTR_UNIT_ICON) ||
	// GWP  	units[UnitIndex].Icon == SPIDERKING_UNIT_ICON;
	return (IsRegent(UnitIndex) || IsLieutenant(UnitIndex));
}


/* ========================================================================
   Function    - mfGetname
   Description - Get the name of the regent
   Returns     - pointer to the name (NULL if not found)
	Notes:
	[d4-09-97, 4-23-97 JPC] Modified to use mfGetName in player stats.
	Moved from the HXX file to here to avoid recompiling everything when
	we fiddle with it.  Also: it is now more complicated than a typical
	inline function, and it requires #including PLAYSTAT.HXX.
	There's a comment in LoadStats that says anything that knows it's a regent
	should not be calling LoadStats, but I haven't found how else to get hold
	of the stats.
   ======================================================================== */
char const * const REGENT::mfGetname() const
{

	SHORT const hPlayerStats = LoadStats (mfGetid(), fERROR);
	if (hPlayerStats != fERROR)
	{
		DumbAutoLockPtr<PLAYER_STATS> dhPlayerStats(hPlayerStats);
		return dhPlayerStats->mfGetName ();
	}
	else
	{
		return NULL;
	}
}
	
/* ======================================================================== */
