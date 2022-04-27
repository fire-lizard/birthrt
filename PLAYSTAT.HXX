/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: PLAYSTAT.H - Runtime Player Information
   Author: 	 Gary Powell
   ======================================================================== */
#ifndef _PLAYSTAT_HXX
#define _PLAYSTAT_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>

#if !defined(_ABILTIY_HXX)
#include "ability.hxx"
#endif

#if !defined(_ALIGNMNT_HXX)
#include "alignmnt.hxx"
#endif

#if !defined(_ARMOR_HXX)
#include "armor.hxx"
#endif

#if !defined(_DEXTERIT_HXX)
#include "dexterit.hxx"
#endif

#if !defined(_DICE_H)
#include "dice.h"
#endif

#if !defined(_FILEUTIL_H)
#include "fileutil.h"
#endif

#if !defined(_REGENTS_HXX)
#include "regents.hxx"
#endif

#if !defined(_GAMETYPE_HXX)
#include "gametype.hxx"
#endif

#if !defined(_INVNTORY_HXX)
#include "invntory.hxx"
#endif

#if !defined(_PLAYID_HXX)
#include "playid.hxx"
#endif

#if !defined(_PLAYRACE_HXX)
#include "playrace.hxx"
#endif

#if !defined(_PLAYCLAS_HXX)
#include "playclas.hxx"
#endif

#if !defined(_SPELL_HXX)
#include "spell.hxx"
#endif

#if !defined(_STATSMOD_HXX)
#include "statsmod.hxx"
#endif

#if !defined(_STRENGTH_HXX)
#include "strength.hxx"
#endif

#if !defined(_STRENUM_H)
#include "strenum.h"
#endif

#if !defined(_VECTOR_HXX)
#include "vector.hxx"
#endif

#if !defined(_WEAPON_HXX)
#include "weapon.hxx"
#endif



/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
//#define TASK_MAX_LEN			39
#define CURRENT_PLAYER_STAT_VERSION		2

#define fNO_STATFILE			-2

#define CANT_CAST_SPELLS		-1

#define MAX_NAME_LEN			31

/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */

typedef SHORT HDL_PLAYER_STATS;

// Note: We may want to split out the Regent character data from the NPC data.
class PLAYER_STATS
{
private:
	friend class PLAYER_HIT;		// This is an additional functionality class
									// for player stats.
	friend void ObjectList::AddPriestSpells(SHORT hPlayerStats);
	friend void StatReadAllFrom(FILE *fp);
	friend BOOL ExpCheck(PLAYER_STATS * pPS);
	friend void GainExp(PLAYER_STATS * pPS, const LONG ExpAmount,
	             const BOOL CanGainLevelNow);
	
	typedef enum { CURRENT = 0
	} VERSION;
	
	// VERSION should be the first data member.
	VERSION 				StructVersion;
	
	SHORT					hThis;			// memory handle of this guy
	THINGTYPE 				ThingType;
	ABILITY					Ability;
	RACE_INFO::TYPE			Race;
	// [d4-09-97, 4-23-97 JPC] Replaced by szNameStat, below:
	// PLAYER_NAME				NameClass;
	PLAYER_TITLE		    TitleClass;
	PLAYER_DESCRIPTION		DesClass;
	ALIGNMENT_INFO::TYPE	Alignment;
public:
	// This is public until I get to all the spell/inventory code.
	STATSMOD				StatsMod;
private:
	SHORT					HitPts;
	SBYTE					sbArmorClass;
	ARMOR_INFO::TYPE		DefaultArmor;
	WEAPON_INFO::TYPE		DefaultWeapon;
	SHORT					sMorale;
	SHORT					FullHitPts;
	SBYTE					Thac0;
	DICE					DamageDice[2];					// for non monsters is probably zero.
	DICE					HitDice;						// To calculate the hit pts.
	LONG					XP;								// Experinence pts gained for killing me.
	PLAYER_CLASS_LIST		PlayerClassList;				// Note: For Multi class players you must traverse this list
public:
	SHORT					hAvatar;						// back pointer to avatar with these stats.
private:
	SBYTE					NumberOfAttacks;				// Number of attacks per round.
	ULONG					Gold;
	char					szNameStat[MAX_NAME_LEN + 1]; // [d4-08-97 JPC] new: contains name
	unsigned int			Blindness : 1;					// TRUE or FALSE.
	unsigned int			Paralyized : 1;					// TRUE or FALSE
	
	// Not used in version 0 of birthrt
	//SHIELD_INFO::TYPE		ShieldType;
	//char					Task[TASK_MAX_LEN + 1];			// Note: The task data structure will change!!!
	//PLAYER_BLOOD_INFO		BloodInfo;
	//PTR_SPELL_LIST		pSpellBook;
	//PTR_SPELL_SCHOOL_LIST	pSpellSchools;					// Schools of magic the player ascribes to.
	//SHORT			 		IdHeir;							// Unimplemented (Player who gets the Regency points if this one dies.)
	//LONG					Age;							// Unimplemented
	//LONG					Height;							// Unimplemented
	//LONG					Weight;							// Unimplemented

public:
	//Inventory
	ObjectList Inventory;
	
	// Default constructor.
	PLAYER_STATS():
		Ability(0,0,0,0,0,0)
	{ mfInitValues(); }
	
	static HDL_PLAYER_STATS mfAllocateSpace(SHORT,LONG);
	void mfInitValues();
	
	void mfDelete();
	
	inline LONG mfReadData(FILE *);
	inline LONG mfWriteData(FILE *);
	LONG mfReadTextData(FILE *);
	LONG mfWriteTextData(FILE *);
	
	inline SHORT	const mfRollDice(DICE const *) const;
	
	inline THINGTYPE const mfGetType() const;
	inline void mfSetType(THINGTYPE const);
	
	inline SHORT const mfGetAvatarHandle() const;
	
	inline SBYTE const mfGetArmorClass() const ;
	inline WEAPON_INFO::TYPE const mfGetFirstWeapon() const ;
	inline ARMOR_INFO::TYPE const mfGetCurrentArmor() const ;
	inline ObjectList & mfGetInventory();
	inline ObjectList const & mfGetInventory() const;
	
	inline BOOL const mfCheckPoorMorale() const;
	
	/*** helper functions to de-mystify the collection of data ***/
	/*** actually needed by the player stats				   ***/
	/* these are for the PLAYER_CLASS_INFO */
	inline CHAR const * const mfGetClass(SHORT const ) const;
	inline PLAYER_CLASS_INFO::TYPE const mfGetClassType(SHORT const ) const;
	inline BOOL const mfIsType(PLAYER_CLASS_INFO::TYPE const /* ClassType */ ) const;
	inline SBYTE const mfGetLevel(SHORT const ) const;
	inline char const * const mfGetName() const;
	inline void mfSetName(CSTRPTR szName);
// [d4-23-97 JPC] mfGetNameIndex is no longer meaningful.
//	inline const GAME_STRING mfGetNameIndex() const;
	inline char const * const mfGetTitle() const;
	inline GAME_STRING const mfGetTitleIndex() const;
	inline char const * const mfGetDescription() const;
	inline GAME_STRING const mfGetDesIndex() const;
	inline RACE_INFO::TYPE mfGetRace() const;	 // [d5-14-97 JPC]
	inline SHORT const mfGetStr() const;
	inline SHORT const mfGetInt() const;
	inline SHORT const mfGetDex() const;
	inline SHORT const mfGetWis() const;
	inline SHORT const mfGetCon() const;
	inline SHORT const mfGetChr() const;
	inline SHORT const mfGetTHAC0() const;
	inline LONG const mfGetExp(SHORT const) const;
	inline SHORT const mfGetMaxHitPoints() const;
	inline SHORT const mfGetCurHitPoints() const;
	SHORT const mfGetSpellCastClass(SPELL_INFO::CASTERTYPE const) const;
	inline SBYTE const mfGetNumberOfAttacks() const;
	
	inline void mfRestoreFullHitPts();
	inline void mfKillem();
	inline void mfTakeDamage(LONG const /* damagepts */);
	inline void mfHeal(LONG const /* Healpts */);
	
	inline LONG const mfDamageVulFactor() const;
	inline BOOL const mfAmFireResistant() const;
	inline BOOL const mfSpeedEnhanced() const;
	
	SHORT const mfCanMemorize(SPELL_INFO::CASTERTYPE , SPELL_INFO::LEVEL) const;
	SHORT const mfMemorized(SPELL_INFO::CASTERTYPE , SPELL_INFO::LEVEL);
	BOOL const mfCanIMemorizeThisSpell( ITEMTYPE const);
	inline void mfMemorizeThisSpell(ITEMTYPE const);
	void mfMemorizeAllInvSpells();
	inline BOOL const mfIsMagicUser() const;
	inline BOOL const PLAYER_STATS::mfIsMagicUser(PLAYER_CLASS_INFO::TYPE * pType,
		LONG * pLevel) const;

	inline ALIGNMENT_INFO::TYPE	const mfGetAlignment() const;
	inline DICE const * const mfGetHitDice() const;
	inline void mfSetBlind();
	inline void mfClearBlind();
	
	inline void mfAddGold(LONG const /* moreGold */);
	inline ULONG const mfGetGold() const;
	inline void mfRemoveAllGold();
protected:
private:
	inline void mfValidateData() const;
};

typedef PLAYER_STATS *PTR_PLAYER_STATS;

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
void StatReadAllFrom(FILE *fp);
void StatWriteAllTo(FILE *fp);
void StatReadAll();
void StatWriteAll();

SHORT LoadStats(LONG ID,SHORT hAvatar);
THINGTYPE const GetThingTypeFromID(LONG const ID);
SHORT LoadAVD (
	CSTRPTR szFileName,
	BOOL /* fSetPal */,
	BOOL /* fLockRes */,
	BOOL /* fRotated */,
	LONG /* iResFileSlot */);

extern "C"{
BOOL IsAvatarDead(THINGTYPE);
}
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

extern DECL_VECTOR_CLASS_S(HDL_PLAYER_STATS,playerstats,CHARACTER_COUNT);
extern BOOL playerStatsInitted;

/* ------------------------------------------------------------------------
   Inline Functions
   ------------------------------------------------------------------------ */
inline BOOL isGoodStats(SHORT hAvatarStats)
{
		if (hAvatarStats==fERROR)
			return FALSE;

		for (LONG i=0;i<CHARACTER_COUNT;i++)
		{
			if (playerstats[i]==hAvatarStats)
				return TRUE;
		}
		return FALSE;
}	



inline void PLAYER_STATS::mfValidateData() const
{
#if defined (DEBUG)
	if (ThingType < 0 || ThingType > LAST_GAME_THING)
	{
		fatal_error ("PlayerStats are toast.\n");
	}
#endif
}

// Roll the dice with any modifiers.
inline SHORT const PLAYER_STATS::mfRollDice( DICE const *diceptr ) const
{
	mfValidateData();
	return(SHORT)(RollDice(diceptr) + StatsMod.mfGetMod(STATSMOD::DIEROLL_MOD));
}

inline LONG PLAYER_STATS::mfReadData(FILE *fp)
{
	LONG Result;
	
	// GWP This may not work in the long run.
	// At some time in the future we may want to check the version numbers.
	Result = fread(this, sizeof(PLAYER_STATS), 1, fp);
	
	return Result;
}

inline LONG PLAYER_STATS::mfWriteData(FILE *fp)
{
	LONG Result;
	
	// GWP This may not work in the long run.
	Result = fwrite(this, sizeof(PLAYER_STATS), 1, fp);
	
	return Result;
}

// Get the thing type of this character
inline THINGTYPE const PLAYER_STATS::mfGetType() const
{
	mfValidateData();
	return ThingType;
}

inline void PLAYER_STATS::mfSetType(THINGTYPE const NewType)
{
	ThingType = NewType;
}

// Get the Thaco value for this character.
inline SHORT const PLAYER_STATS::mfGetTHAC0() const
{
	mfValidateData();
	return Thac0;
}

// Get the current weapon for this character.
inline WEAPON_INFO::TYPE const PLAYER_STATS::mfGetFirstWeapon() const
{
	mfValidateData();
	return DefaultWeapon;
	//return Inventory.mfGetCurrentWeapon();
}

// Get the curernt armor for this character.
inline ARMOR_INFO::TYPE const PLAYER_STATS::mfGetCurrentArmor() const
{
	mfValidateData();
	return DefaultArmor;
	//return Inventory.mfGetCurrentArmor();
}

// Get the const inventory object for this character.
inline ObjectList const & PLAYER_STATS::mfGetInventory() const
{
	mfValidateData();
	return Inventory;
}
// Get the inventory object for this character.
inline ObjectList & PLAYER_STATS::mfGetInventory()
{
	mfValidateData();
	return Inventory;
}

#define ARMORCLASS_WORST 10
#define ARMORCLASS_BEST -10

inline SBYTE const PLAYER_STATS::mfGetArmorClass() const
{
	mfValidateData();
	
	SBYTE baseAC = sbArmorClass;
	SBYTE ACset  = StatsMod.mfGetSet(STATSMOD::ARMORCLASS_SET);
	
	if (ACset != 0 && baseAC > ACset) // can't set AC to 0... slight bug
	    baseAC = ACset;
	
	SBYTE ACmod  = StatsMod.mfGetMod(STATSMOD::ARMORCLASS_MOD);
	SBYTE dexmod = DEXTERITY::mfGetDefensiveAdjust(mfGetDex());
	
	SBYTE AC = baseAC + ACmod + dexmod;
	
	if (AC > ARMORCLASS_WORST)
	    AC = ARMORCLASS_WORST;
	if (AC < ARMORCLASS_BEST)
	    AC = ARMORCLASS_BEST;
	
	return AC;
}

// If True, your Morale is poor.
inline BOOL const PLAYER_STATS::mfCheckPoorMorale() const
{
	mfValidateData();
	DICE const MoraleDie = { 1, 20, 0};
	
	return (sMorale <= mfRollDice(&MoraleDie) ) ? TRUE : FALSE;
}

inline CHAR const * const PLAYER_STATS::mfGetClass(SHORT const sIndex) const
{
	mfValidateData();
	return PlayerClassList.mfGetName(sIndex);
}

inline const PLAYER_CLASS_INFO::TYPE PLAYER_STATS::mfGetClassType(SHORT const sIndex) const
{
	mfValidateData();
	return PlayerClassList.mfGetType(sIndex);
}

inline BOOL const PLAYER_STATS::mfIsType(PLAYER_CLASS_INFO::TYPE const ClassType) const
{
	mfValidateData();
	return PlayerClassList.mfIsType(ClassType);
}

inline SBYTE const PLAYER_STATS::mfGetLevel(SHORT const sIndex) const
{
	mfValidateData();
	SBYTE level = PlayerClassList.mfGetLevel(sIndex);
	SBYTE set = StatsMod.mfGetSet(STATSMOD::LEVEL_SET);
	
	if (set != 0)
		level = set;
		
	if (level != 0)
	{
		level += StatsMod.mfGetMod(STATSMOD::LEVEL_MOD);
		
		PLAYER_CLASS_INFO::TYPE myclass = mfGetClassType(sIndex);
		
		switch(myclass)
		{
		case PLAYER_CLASS_INFO::FIGHTER :
		case PLAYER_CLASS_INFO::PALADIN :
		case PLAYER_CLASS_INFO::RANGER  :
		
			level += StatsMod.mfGetMod(STATSMOD::LEVEL_MOD_WARRIOR);
			break;
			
		case PLAYER_CLASS_INFO::PRIEST  :
		
			level += StatsMod.mfGetMod(STATSMOD::LEVEL_MOD_PRIEST);
			break;
			
		case PLAYER_CLASS_INFO::WIZARD  :
		case PLAYER_CLASS_INFO::MAGICIAN:
		
			level += StatsMod.mfGetMod(STATSMOD::LEVEL_MOD_WIZARD);
			break;
			
		case PLAYER_CLASS_INFO::THIEF   :
		case PLAYER_CLASS_INFO::BARD    :
		
			level += StatsMod.mfGetMod(STATSMOD::LEVEL_MOD_ROGUE);
			break;
		}
	
	    if (level > 20)
		    level = 20;
	    if (level < 1)
		    level = 1;
	}	
	
	return (level);
}

inline char const * const PLAYER_STATS::mfGetName() const
{
	mfValidateData();
// [d4-23-97 JPC] if (NameClass.fName == 0)
	if (szNameStat[0] == 0)
		return GAME_TTYPE::mfGetDescription(ThingType);
	else
		// [d4-23-97 JPC] return (char *)(NameClass.mfGetName(NameClass.fName));
		return szNameStat;
}

inline void PLAYER_STATS::mfSetName (CSTRPTR szName)
{
// [d4-23-97 JPC] New function to set name.
	mfValidateData();
	strncpy (szNameStat, szName, MAX_NAME_LEN);
}

// [d4-09-97, 4-23-97 JPC] Abolished this function because we no longer have this data.
// inline GAME_STRING const PLAYER_STATS::mfGetNameIndex() const
// {
// 	mfValidateData();
// 	return (GAME_STRING)(NameClass.mfGetNameIndex(NameClass.fName));
// }

inline char const * const PLAYER_STATS::mfGetTitle() const
{
	mfValidateData();
	return TitleClass.mfGetTitle(TitleClass.fTitle);
}

inline GAME_STRING const PLAYER_STATS::mfGetTitleIndex() const
{
	mfValidateData();
	return (GAME_STRING)(TitleClass.mfGetTitleIndex(TitleClass.fTitle));
}

inline char const * const PLAYER_STATS::mfGetDescription() const
{
	mfValidateData();
	return (char *)(DesClass.mfGetDescription(DesClass.fDescription));
}

inline GAME_STRING const PLAYER_STATS::mfGetDesIndex() const
{
	mfValidateData();
	return (GAME_STRING)(DesClass.mfGetDesIndex(DesClass.fDescription));
}

inline RACE_INFO::TYPE PLAYER_STATS::mfGetRace() const
{
// [d5-14-97 JPC] For updating regents array from AVA file data.

	mfValidateData();
	return Race;
}

#define MAX_STRENGTH		30  // really 25, because of percentile strength
#define MAX_DEXTERITY		25
#define MAX_CONSTITUTION	25
#define MAX_INTELLIGENCE	25
#define MAX_WISDOM			25
#define MAX_CHARISMA		25

inline SHORT const PLAYER_STATS::mfGetStr() const
{
	mfValidateData();
	SHORT str = Ability.mfGetStrength();
	SBYTE set = StatsMod.mfGetSet(STATSMOD::STRENGTH_SET);
	SBYTE mod = StatsMod.mfGetMod(STATSMOD::STRENGTH_MOD);
	
	if (str != 0)
	{
		str += mod;
		if (str < 1)
		    str = 1;
		if (str > MAX_STRENGTH)
		    str = MAX_STRENGTH;
	}
	// never lower a stat if you're trying to raise it, or vice-versa
	
	if (set != 0)
	{
		BOOL raising    = (set > 10);
		BOOL wouldraise = (str < set);
		
		if (raising == wouldraise)
		    str = set;
	}
	
	return(SHORT)(str);
}

inline SHORT const PLAYER_STATS::mfGetInt() const
{
	mfValidateData();
	SHORT intel = Ability.mfGetIntelligence();
	SBYTE set = StatsMod.mfGetSet(STATSMOD::INTELLIGENCE_SET);
	SBYTE mod = StatsMod.mfGetMod(STATSMOD::INTELLIGENCE_MOD);
	
	if (intel != 0)
	{
		intel += mod;
		if (intel < 1)
		    intel = 1;
		if (intel > MAX_INTELLIGENCE)
		    intel = MAX_INTELLIGENCE;
	}
	
	// never lower a stat if you're trying to raise it, or vice-versa
	
	if (set != 0)
	{
		BOOL raising    = (set > 10);
		BOOL wouldraise = (intel < set);
		
		if (raising == wouldraise)
		    intel = set;
	}
	
	return(SHORT)(intel);
}

inline SHORT const PLAYER_STATS::mfGetDex() const
{
	mfValidateData();
	SHORT dex = Ability.mfGetDexterity();
	SBYTE set = StatsMod.mfGetSet(STATSMOD::DEXTERITY_SET);
	SBYTE mod = StatsMod.mfGetMod(STATSMOD::DEXTERITY_MOD);
	
	if (dex != 0)
	{
		dex += mod;
		if (dex < 1)
		    dex = 1;
		if (dex > MAX_DEXTERITY)
		    dex = MAX_DEXTERITY;
	}
	
	// never lower a stat if you're trying to raise it, or vice-versa
	
	if (set != 0)
	{
		BOOL raising    = (set > 10);
		BOOL wouldraise = (dex < set);
		
		if (raising == wouldraise)
		    dex = set;
	}
		
		
	return(SHORT)(dex);
}

inline SHORT const PLAYER_STATS::mfGetWis() const
{
	mfValidateData();
	SHORT wis = Ability.mfGetWisdom();
	SBYTE set = StatsMod.mfGetSet(STATSMOD::WISDOM_SET);
	SBYTE mod = StatsMod.mfGetMod(STATSMOD::WISDOM_MOD);
	
	if (wis != 0)
	{
		wis += mod;
		if (wis < 1)
		    wis = 1;
		if (wis > MAX_WISDOM)
		    wis = MAX_WISDOM;
	}
	
	// never lower a stat if you're trying to raise it, or vice-versa
	
	if (set != 0)
	{
		BOOL raising    = (set > 10);
		BOOL wouldraise = (wis < set);
		
		if (raising == wouldraise)
		    wis = set;
	}
	
	return(SHORT)(wis);
}

inline SHORT const PLAYER_STATS::mfGetCon() const
{
	mfValidateData();
	SHORT con = Ability.mfGetConstitution();
	SBYTE set = StatsMod.mfGetSet(STATSMOD::CONSTITUTION_SET);
	SBYTE mod = StatsMod.mfGetMod(STATSMOD::CONSTITUTION_MOD);
	
	if (con != 0)
	{
		con += mod;
		if (con < 1)
		    con = 1;
		if (con > MAX_CONSTITUTION)
		    con = MAX_CONSTITUTION;
	}
	
	// never lower a stat if you're trying to raise it, or vice-versa
	
	if (set != 0)
	{
		BOOL raising    = (set > 10);
		BOOL wouldraise = (con < set);
		
		if (raising == wouldraise)
		    con = set;
	}
	
	return(SHORT)(con);
}

inline SHORT const PLAYER_STATS::mfGetChr() const
{
	mfValidateData();
	SHORT chr = Ability.mfGetCharisma();
	SBYTE set = StatsMod.mfGetSet(STATSMOD::CHARISMA_SET);
	SBYTE mod = StatsMod.mfGetMod(STATSMOD::CHARISMA_MOD);
	
	if (chr != 0)
	{
		chr += mod;
		if (chr < 1)
		    chr = 1;
		if (chr > MAX_CHARISMA)
		    chr = MAX_CHARISMA;
	}
	
	// never lower a stat if you're trying to raise it, or vice-versa
	
	if (set != 0)
	{
		BOOL raising    = (set > 10);
		BOOL wouldraise = (chr < set);
		
		if (raising == wouldraise)
		    chr = set;
	}
	
	return(SHORT)(chr);
}

inline LONG const PLAYER_STATS::mfGetExp(SHORT const sIndex) const
{
	mfValidateData();
	return PlayerClassList.mfGetExpPts(sIndex);
}

inline SHORT const PLAYER_STATS::mfGetMaxHitPoints() const
{
	return FullHitPts;
}

inline SHORT const PLAYER_STATS::mfGetCurHitPoints() const
{
	mfValidateData();
	SHORT hp = HitPts;
	
	if (hp && (hp + StatsMod.mfGetMod(STATSMOD::HITPOINTS_MOD)) < 0)
	{
		hp = 1;	
	}
	else
	{
		hp += StatsMod.mfGetMod(STATSMOD::HITPOINTS_MOD);
	}
		
	return hp;
}

inline void PLAYER_STATS::mfRestoreFullHitPts()
{
	mfValidateData();
	HitPts = FullHitPts;
}

inline void PLAYER_STATS::mfKillem()
{
	HitPts = 0;
}

inline void PLAYER_STATS::mfTakeDamage(LONG const damagepts )
{
	HitPts -= damagepts;
	if (HitPts < 0)
		HitPts = 0;
}

inline void PLAYER_STATS::mfHeal(LONG const Healpts)
{
	HitPts += Healpts;
	if (HitPts > FullHitPts)
	{
		HitPts = FullHitPts;
	}
}

// Get Damage Invulnerablity Factor.
inline LONG const PLAYER_STATS::mfDamageVulFactor() const
{
	return( 100 - (LONG) StatsMod.mfGetMod(STATSMOD::DAMAGE_RESIST_MOD));
}

inline BOOL const PLAYER_STATS::mfAmFireResistant() const
{
	return StatsMod.mfGetMod(STATSMOD::FIRE_RESIST_MOD);
}

inline BOOL const PLAYER_STATS::mfSpeedEnhanced() const
{
	return StatsMod.mfGetMod(STATSMOD::SPEED_MOD);
}
inline SBYTE const PLAYER_STATS::mfGetNumberOfAttacks() const
{
	return NumberOfAttacks;
}


// add a charge on this spell. Be sure to test the capablity before
// calling this fn.
inline void PLAYER_STATS::mfMemorizeThisSpell(ITEMTYPE const NewSpell)
{
	InventoryItor itor(Inventory);
	
	if (itor.mfFind(NewSpell, IC_SPELL))
	{
		(*itor)->mfGainOne();
	}
}

inline BOOL const PLAYER_STATS::mfIsMagicUser() const
{
	return ( PlayerClassList.mfIsType(PLAYER_CLASS_INFO::WIZARD) ||
	         PlayerClassList.mfIsType(PLAYER_CLASS_INFO::PRIEST)
	       );
}

inline BOOL const PLAYER_STATS::mfIsMagicUser(PLAYER_CLASS_INFO::TYPE * pType,
	LONG * pLevel) const
{
// [d2-25-97 JPC]
// Use this overloaded function if you want to know the type and level
// of magic user.
//
// Returns:
// 	TRUE  if player is a magic user; type is in *pType and level in *plevel
// 	FALSE if player is not a magic user; *pType and *pLevel are unchanged.
//
// Purpose: improve calculation of magic power in NPC combat.
// See BATTLEUI.CPP: GetMagicUserPower.

	for (LONG iClass = 0; iClass < MAX_PLAYER_CLASSTYPES; iClass++)
	{
      PLAYER_CLASS_INFO::TYPE	iType;
		LONG iLevel;

		iType = PlayerClassList.mfGetType (iClass);
		iLevel = PlayerClassList.mfGetLevel (iClass);

		switch (iType)
		{
			case PLAYER_CLASS_INFO::PRIEST:
			case PLAYER_CLASS_INFO::WIZARD:
				*pType = iType;
				*pLevel = iLevel;
				return TRUE;

			case PLAYER_CLASS_INFO::BARD:
				if (iLevel > 1)
				{
					*pType = iType;
					*pLevel = iLevel;
					return TRUE;
				}
				break;

			// Magicians?  Not used in Birthright 1.

			// High-level Rangers and Paladins can have magic.
			// We don't seem to handle that in this game.  (They may
			// not even be able to reach such high levels in this game.)

			// default: there is no default case
		}
	}

	return FALSE;
}


inline ALIGNMENT_INFO::TYPE	const PLAYER_STATS::mfGetAlignment() const
{
	return Alignment;
}

inline DICE const * const PLAYER_STATS::mfGetHitDice() const
{
	return &HitDice;
}

inline void PLAYER_STATS::mfSetBlind()
{
	Blindness = TRUE;
}

inline void PLAYER_STATS::mfClearBlind()
{
	Blindness = FALSE;
}

inline void PLAYER_STATS::mfAddGold(LONG const moreGold)
{
	Gold += moreGold;
}

inline ULONG const PLAYER_STATS::mfGetGold() const
{
	return Gold;
}
inline void PLAYER_STATS::mfRemoveAllGold()
{
	Gold = 0;
}
#endif // _PLAYSTAT_HXX
