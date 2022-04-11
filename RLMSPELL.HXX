/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: RLMSPELL.HXX - Realm Spell Stuph
   Author: 	 Wes Cumberland
   ======================================================================== */
#ifndef _RLMSPELL_HXX
#define _RLMSPELL_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_THINGS_H)
#include "things.h"
#endif

#if !defined(_PLAYCLAS_HXX)
#include "playclas.hxx"
#endif


/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
class REALM_SPELL_INFO 
{
	public:
		// AD&D Handbook p.45
		typedef enum 
		{
			SCHOOL_NO_SCHOOL	=-1,
			SCHOOL_ABJURATION = 0,
			SCHOOL_ALTERATION = 1,
			SCHOOL_CONJURATION = 2,			// & SUMMOMING
			SCHOOL_ENCHANTMENT = 3,			// & CHARM
			SCHOOL_GREATER_DIVINATION = 4,
			SCHOOL_ILLUSION = 5,
			SCHOOL_INVOCATION = 6,			// & EVOCATION
			SCHOOL_NECROMANCY = 7,
			SCHOOL_LESSER_DIVINATION = 8,
			
			/* priest sphere */
			SCHOOL_ALL = 9,
			SCHOOL_WAR = 10,    // & combat
			SCHOOL_PLANT = 11,
			SCHOOL_PROTECTION = 12,
			SCHOOL_CHARM = 13,   // & law
			SCHOOL_ANY	= 14,

		} SCHOOL;
		
		typedef enum
		{
			NO_SPELL = -1,
			ALCHEMY =0 ,
			DEATH_PLAGUE,
			DEMAGOGUE,
			DISPEL_REALM_MAGIC,
			LEGION_OF_DEAD,
			MASS_DESTRUCTION,
			RAZE,
			SCRY,
			STRONGHOLD,
			SUBVERSION,
			SUMMONING,
			TRANSPORT,
			WARDING,
			BLESS_LAND,
			BLESS_ARMY,
			BLIGHT,
			DISPEL_REALM_MAGIC_PRIESTVER, 
			HONEST_DEALING,
			INVESTITURE,
			MAX_REALM_SPELL_INDEX,
		}	TYPE;
		
		typedef enum
		{
			DURATION_PERM,
			DURATION_INSTANT,
			DURATION_TWO_TURN,
			DURATION_AR,		//?? (see subversion realm_spell)
			DURATION_ONE_TURN,
		}	DURATION;

			
			

 		class REALM_SPELL_ITEM
		{
		public:
			inline REALM_SPELL_ITEM(LONG  /* NewName */,
			                        SCHOOL /* NewSchool */,
			                        UBYTE /* NewRegency */,
			                        UBYTE /* NewSource */,
			                        UBYTE /* NewLevel */,
			                        UBYTE /* NewGold */,
			                        DURATION /* NewDuration */);
			inline char * mfGetName() const;
			inline SCHOOL const mfGetSchool() const;
			inline UBYTE const mfGetRegency() const;
			inline UBYTE const mfGetSource() const;
			inline UBYTE const mfGetLevel() const;
			inline UBYTE const mfGetGold() const;
			inline DURATION const mfGetDuration() const;
		protected:
		private:
			LONG	  const	Name;
			SCHOOL	  const	School;
			UBYTE	  const	Regency;
			UBYTE	  const	Source;
			UBYTE	  const	Level;
			UBYTE	  const	Gold;
			DURATION  const	Duration;
		};
		static REALM_SPELL_ITEM const &mfGetRealmSpell(TYPE t);
		
	private:
		static const REALM_SPELL_ITEM RealmSpellInfoTable[];
};

typedef REALM_SPELL_INFO::REALM_SPELL_ITEM *PTR_REALM_SPELL;

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
inline REALM_SPELL_INFO::REALM_SPELL_ITEM const &REALM_SPELL_INFO::mfGetRealmSpell(REALM_SPELL_INFO::TYPE t)
{
	return RealmSpellInfoTable[(SHORT)t];
}

inline REALM_SPELL_INFO::REALM_SPELL_ITEM::REALM_SPELL_ITEM(
LONG NewName,
REALM_SPELL_INFO::SCHOOL NewSchool,
UBYTE NewRegency,
UBYTE NewSource,
UBYTE NewLevel,
UBYTE NewGold,
REALM_SPELL_INFO::DURATION NewDuration ) :
      Name(NewName),
      School(NewSchool),
      Regency(NewRegency),
      Source(NewSource),
      Level(NewLevel),
      Gold(NewGold),
      Duration(NewDuration)
{
}
inline char * REALM_SPELL_INFO::REALM_SPELL_ITEM::mfGetName() const
{
	return STRMGR_GetStr(Name);
}
inline REALM_SPELL_INFO::SCHOOL const REALM_SPELL_INFO::REALM_SPELL_ITEM::mfGetSchool() const
{
	return School;
}
inline UBYTE const REALM_SPELL_INFO::REALM_SPELL_ITEM::mfGetRegency() const
{
	return Regency;
}
inline UBYTE const REALM_SPELL_INFO::REALM_SPELL_ITEM::mfGetSource() const
{
	return Source;
}
inline UBYTE const REALM_SPELL_INFO::REALM_SPELL_ITEM::mfGetLevel() const
{
	return Level;
}
inline UBYTE const REALM_SPELL_INFO::REALM_SPELL_ITEM::mfGetGold() const
{
	return Gold;
}
inline REALM_SPELL_INFO::DURATION const REALM_SPELL_INFO::REALM_SPELL_ITEM::mfGetDuration() const
{
	return Duration;
}
#endif // _RLMSPELL_HXX
