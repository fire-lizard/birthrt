/* ========================================================================
   Copyright (c) 1997	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: REGRELIC.CPP
   Author:   John Conley, implementing a design by Jim Edwards.
   ========================================================================
   Hand out relics to various lieutenants for the multiplayer game.
	There are 17 eligible lieutenants (those with holdings).
	There is a 50% chance that a given lieutenant will have a relic.
	Relics are assigned randomly, but more or less in keeping with
	the player's class.
	The upshot is that the average multiplayer game will have 8-1/2 relics
	available.  In other words,

	>>> NOT ALL RELICS ARE ASSIGNED!

   ======================================================================= */

#if defined (_WINDOWS)


/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#include "TYPEDEFS.H"
//#include "gamemap.hxx"
//#include "regents.hxx"
//#include "realm.hxx"
#include "ITEMTYPE.HXX"
#include "REGRELIC.HXX"
#include "PLAYSTAT.HXX"


/* ------------------------------------------------------------------------
   Defines
   ------------------------------------------------------------------------ */

#define ANY			0
#define SPECIFIC	1


/* ------------------------------------------------------------------------
   Data
   ------------------------------------------------------------------------ */

// Lieutenants
//
// These are the only lieutenants who can have relics.  These are the
// lieutenants with holdings, who have to be acquired through diplomacy
// rather than direct hire.  It would be bad if you could spend a few
// gold coins and acquire the State Crown of Anuire.

static REGENT_ENUM gLieutenants[] = {
	ADAERE_DONEIM,	
	AELIES,			
	MARICOERE,		
	EL_HADID,		
	ELAMIEN_LAMIER,
	EYELESS_ONE,	
	THE_SWORD,		
	GAELIN_THURIED,
	HERMEDHIE,		
	ARMIENDIN,		
	LARRA_NIELEMS,	
	BRIESEN,			
	BACCAERE,		
	ORTHIEN_TANE,	
	ROCKHAMMER,		
	THE_SWAMP_MAGE,
	TEMIAS_COUMAIN
};


// Relics
//
// Here are the available relics, along with rules for who can possess them.
// ANY means anyone can own this relic.
//	SPECIFIC means we specify the one or two classes that can own this relic.
// Excluded class means this class CANNOT own the relic.
// Assigned becomes TRUE if someone owns the relic (to prevent duplicate
// ownership).

DEFINE_VECTOR_DATA (REGRELIC_STRUCT, gRelics) = {
// Item                                Spec/Any  Class 1   Class 2   Excluded	 Assigned
	{ITEM_DIERDRIENS_RING,              SPECIFIC, WIZARD  , NO_CLASS, NO_CLASS, FALSE},
	{ITEM_FAELES_RING,                  SPECIFIC, PRIEST  , WIZARD  , NO_CLASS, FALSE},
	{ITEM_GAVELONS_STAFF_OF_PROSPERITY, ANY     , NO_CLASS, NO_CLASS, NO_CLASS, FALSE},
	{ITEM_AMULET_OF_INSPIRATION,        SPECIFIC, FIGHTER , NO_CLASS, NO_CLASS, FALSE},
	{ITEM_BANNER_OF_ROELE,              SPECIFIC, FIGHTER , NO_CLASS, NO_CLASS, FALSE},
	{ITEM_BARAZADS_TOOLS,               ANY     , NO_CLASS, NO_CLASS, NO_CLASS, FALSE},
	{ITEM_BRENNAS_FAVOR,                ANY     , NO_CLASS, NO_CLASS, NO_CLASS, FALSE},
	{ITEM_CHALICE_OF_THE_DEAD,          SPECIFIC, PRIEST  , NO_CLASS, NO_CLASS, FALSE},
	{ITEM_CROWN_OF_COMMAND,             ANY     , NO_CLASS, NO_CLASS, NO_CLASS, FALSE},
	{ITEM_STATE_CROWN_OF_ANNUIRE,       ANY     , NO_CLASS, NO_CLASS, NO_CLASS, FALSE},
	{ITEM_EMPERORS_CROWN,               ANY     , NO_CLASS, NO_CLASS, NO_CLASS, FALSE},
	{ITEM_FARIDS_COFFER_OF_THE_REALM,   ANY     , NO_CLASS, NO_CLASS, NO_CLASS, FALSE},
	{ITEM_HAMMER_OF_THUNDER,            SPECIFIC, FIGHTER , NO_CLASS, NO_CLASS, FALSE},
	{ITEM_RING_OF_WIZARDRY,             SPECIFIC, WIZARD  , NO_CLASS, NO_CLASS, FALSE},
	{ITEM_ROBES_OF_THE_MASES,           ANY     , NO_CLASS, NO_CLASS, NO_CLASS, FALSE},
	{ITEM_REGALIA_OF_THE_EMPIRE,        ANY     , NO_CLASS, NO_CLASS, PRIEST  , FALSE},
	{ITEM_SIELSHEGH_GEM_LARGE,          ANY     , NO_CLASS, NO_CLASS, NO_CLASS, FALSE},
	{ITEM_SIELSHEGH_GEM_SMALL,          ANY     , NO_CLASS, NO_CLASS, NO_CLASS, FALSE},
	{ITEM_SCEPTRE_OF_CUIRAECEN,         SPECIFIC, FIGHTER , NO_CLASS, NO_CLASS, FALSE},
	{ITEM_CORGANDALS_STAFF,             SPECIFIC, WIZARD  , NO_CLASS, NO_CLASS, FALSE},
	{ITEM_NAPPOLANS_TOME_OF_WAR,        SPECIFIC, FIGHTER , NO_CLASS, NO_CLASS, FALSE},
   {ITEM_VAUBENELS_BOOK_OF_FORTIFICATION,SPECIFIC, FIGHTER , NO_CLASS, NO_CLASS, FALSE},
	{ITEM_TORC_OF_SPLENDOR,             ANY     , NO_CLASS, NO_CLASS, NO_CLASS, FALSE},
	{ITEM_DANICAS_CRYSTAL_OF_SCRYING,   ANY     , NO_CLASS, NO_CLASS, NO_CLASS, FALSE},
   {ITEM_IOUN_HEART,                   ANY     , NO_CLASS, NO_CLASS, NO_CLASS, FALSE},
   {ITEM_IOUN_FIST,                    ANY     , NO_CLASS, NO_CLASS, NO_CLASS, FALSE},
   {ITEM_TOME_OF_THE_PRINCE,           ANY     , NO_CLASS, NO_CLASS, NO_CLASS, FALSE}
	// Make sure ITEM_TOME_OF_THE_PRINCE is the last item!
};

DEFINE_VECTOR_CLASS (REGRELIC_STRUCT, gRelics);


BOOL REGRELIC_STRUCT::fAssignedRelics = FALSE;



void REGRELIC_STRUCT::ClearRelics ()
{
// Mark all items as unassigned.
// Assumes some other code takes away the items from the lieutenants.
// This should happen in the "start a new game" code.

	LONG			iItem;

	for (iItem = 0; ; iItem++)
	{
		gRelics[iItem].fRegRelicData.fAssigned = FALSE;
		if (gRelics[iItem].fRegRelicData.fItem == ITEM_TOME_OF_THE_PRINCE)
			break;
	}

	fAssignedRelics = FALSE;
}



void REGRELIC_STRUCT::AssignRelics ()
{
// Every lieutenant in our list has a 50% chance of getting an item from
// our item list.

	DICE			dice = {1, 6, 0};
	LONG			cLieutenants;
	LONG			cItems;
	LONG			iRegent;
	LONG			iLieutenant;
	LONG			iItem;

	if (fAssignedRelics)
		return;									// premature return if already assigned

	cLieutenants = sizeof (gLieutenants) / sizeof (REGENT_ENUM);

	// Get count of items.  Note that sizeof does not work with vectors
	// the same way it works with standard arrays.
	cItems = 0;
	while (gRelics[cItems].fRegRelicData.fItem != ITEM_TOME_OF_THE_PRINCE)
		cItems++;
	cItems++;

	for (iLieutenant = 0; iLieutenant < cLieutenants; iLieutenant++)
	{
		if (RollDice (&dice) >= 4)
		{
			// This lieutenant gets a random item.
			// Use the lieutenant's position in the regents array.

			REGENT_ENUM  iRegent = gLieutenants[iLieutenant];
			SHORT hPlayerStats = LoadStats (regents[iRegent].mfGetid(), fERROR);

			if (hPlayerStats != fERROR)
			{
				PTR_PLAYER_STATS pPlrStat = (PTR_PLAYER_STATS)BLKPTR(hPlayerStats);
				if (pPlrStat != (PTR_PLAYER_STATS) fERROR)
				{
					iItem = random (cItems);
					while ( iItem < cItems && !ItemAvailable (iItem, iRegent) )
						iItem++;
					if (iItem >= cItems)
					{
						iItem = 0;
						while ( iItem < cItems && !ItemAvailable (iItem, iRegent) )
							iItem++;
					}
					if (iItem < cItems)
					{
						gRelics[iItem].fRegRelicData.fAssigned = TRUE;
						pPlrStat->Inventory.mfAddItem (gRelics[iItem].fRegRelicData.fItem, 1, 0, FALSE);
					}
				}
			}
		}
	}

	fAssignedRelics = TRUE;
}

void REGRELIC_STRUCT::SetAssigned()
{
	fAssignedRelics = TRUE;
}

BOOL REGRELIC_STRUCT::ItemAvailable (LONG iItem, LONG iRegent)
{
// Returns TRUE if the specified item is unassigned and this lieutenant
// can have it, otherwise FALSE.

	LONG			class1;
	LONG			class2;

	if (gRelics[iItem].fRegRelicData.fAssigned)
		return FALSE;

	class1 = regents[iRegent].mfGetClass1 ();
	class2 = regents[iRegent].mfGetClass2 ();

	if (gRelics[iItem].fRegRelicData.fType == ANY)
	{
		if (gRelics[iItem].fRegRelicData.fClassExcluded == NO_CLASS)
			return TRUE;
		else if (gRelics[iItem].fRegRelicData.fClassExcluded == class1 ||
	      gRelics[iItem].fRegRelicData.fClassExcluded == class2)
			return FALSE;
		else
			return TRUE;
	}
	else
	{
		if (gRelics[iItem].fRegRelicData.fClass1 == class1 ||
	      gRelics[iItem].fRegRelicData.fClass1 == class2 ||
	      gRelics[iItem].fRegRelicData.fClass2 == class1 ||
	      gRelics[iItem].fRegRelicData.fClass2 == class2)
			return TRUE;
	}
	return FALSE;
}


#endif // _WINDOWS
