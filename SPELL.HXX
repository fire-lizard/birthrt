/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: SPELL.HXX - Collection of Abilities for characters
   Author: 	 Gary Powell
   ======================================================================== */
#ifndef _SPELL_HXX
#define _SPELL_HXX

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

#if !defined(_EXPLEVEL_HXX)
#include "explevel.hxx" // for saving throw types
#endif


#define TRAVELINGSEQ	STANDSEQ
#define IMPACTSEQ		ANIMATION3SEQ
#define EFFECTSEQ		ANIMATION5SEQ

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
class SPELL_INFO 
{
	public:
		// From AD&D Handbook p. 168 (Appendix 2)
		typedef enum {
			SAVE_NONE = 0,	// No saving throw allowed.
			SAVE_HALF, 		// If successfull only 1/2 damage occurs.
			SAVE_NEGATE,	// if successfull, negates the spell.
			SAVE_SPECIAL	// If successfull something still happens.
		} SAVING_THROW;
		
		// From AD&D Handbook p. 168 (Appendix 2)
		typedef enum {
			VERBAL = 0,
			SOMATIC,
			MATERIAL
		} COMPONENTS;
		
		// These are bit fields so that we can store combinations of spheres.
		// Of course, a strongly-typed language wouldn't allow it as "enum"...
		typedef enum 
		{
			NO_SPHERE			= 0x0000,
			SPHERE_ALL			= 0x0001,
			SPHERE_ANIMAL		= 0x0002,
			SPHERE_ASTRAL		= 0x0004,
			SPHERE_CHARM		= 0x0008,
			SPHERE_COMBAT		= 0x0010,
			SPHERE_CREATION		= 0x0020,
			SPHERE_DIVINATION	= 0x0040,
			SPHERE_ELEMENTAL	= 0x0080,
			SPHERE_GUARDIAN		= 0x0100,
			SPHERE_HEALING		= 0x0200,
			SPHERE_NECROMANTIC	= 0x0400,
			SPHERE_PLANT		= 0x0800,
			SPHERE_PROTECTION	= 0x1000,
			SPHERE_SUMMONING	= 0x2000,
			SPHERE_SUN			= 0x4000,
			SPHERE_WEATHER		= 0x8000
		} SPHERE_TYPE;
		
		typedef enum 
		{
			NOT_REVERSABLE = 0,
			REVERSABLE
		} REVERSABILTY;
		
		// AD&D Handbook p.45
		typedef enum 
		{
			SCHOOL_NO_SCHOOL	=-1,
			SCHOOL_ABJURATION = 0,
			SCHOOL_ALTERATION,
			SCHOOL_CONJURATION,			// & SUMMOMING
			SCHOOL_ENCHANTMENT,			// & CHARM
			SCHOOL_DIVINATION,
			SCHOOL_ILLUSION,
			SCHOOL_INVOCATION,			// & EVOCATION
			SCHOOL_NECROMANCY
		} SCHOOL;
		
		typedef enum 
		{
			PRIEST,
			WIZARD
		} CASTERTYPE;

		typedef enum
		{
			REALM=1,
			SCENE=2,
			BATTLE=4,
		} CASTERLOC;
		

		typedef enum
		{
			NO_SPELL = -1,
			DETECT_MAGIC = 0,	/* W1 */
			FIRST_WIZARD_SPELL=DETECT_MAGIC,
			FEATHER_FALL,		/* W1 */
			JUMP,				/* W1 */
			LIGHT,				/* W1 */
			MAGIC_MISSILE,		/* W1 */
			REDUCE,				/* W1 */
			SHOCKING_GRASP,		/* W1 */

			BLINDNESS,			/* W2 */
			CONTINUAL_LIGHT,	/* W2 */
			KNOCK,				/* W2 */
			LEVITATE,			/* W2 */
			LOCATE_OBJECT,		/* W2 */
			POISON_ARROW,		/* W2 */
			STRENGTH,			/* W2 */

			FIREBALL,			/* W3 */
			FLY,				/* W3 */
			STONE_UNDEAD,		/* W3 */
			INFRAVISION,		/* W3 */
			LIGHTNING_BOLT,		/* W3 */

			CONFUSION,			/* W4 */
			ICE_STORM,			/* W4 */
			MINOR_GLOBE_INVULN,	/* W4 */
			STONESKIN,			/* W4 */

			CONE_OF_COLD,		/* W5 */
			DISINTIGRATE,		/* W5 */
			TELEPORT,			/* W5 */

			CHAIN_LIGHTNING,	/* W6 */
			DEATH,				/* W6 */
			GLOBE_INVULN,		/* W6 */
			FLESH_TO_STONE,		/* W6 */

			IMPROVED_FIREBAL,	/* W7 */
			POWER_STUN,			/* W7 */ //power word stun

			POWER_BLIND,		/* W8 */ //power word blind
			SPELL_IMMUNITY,		/* W8 */

			METEOR_SWARM,		/* W9 */
			POWER_KILL,			/* W9 */ //power word kill
			TIME_STOP,			/* W9 */
			LAST_WIZARD_SPELL = TIME_STOP,



			CURE_LIGHT,			/* P1 */ //cure light wounds
			FIRST_PRIEST_SPELL=CURE_LIGHT,
			DETECT_EVIL,		/* P1 */
			DETECT_MAGIC_PR,	/* P1 */
			LIGHT_PR,			/* P1 */
			SHILLELAGH,			/* P1 */
			TURN_UNDEAD,		/* P1 */

			BARKSKIN,			/* P2 */
			FIND_TRAPS,			/* P2 */
			FIND_TREASURE,		/* P2 */
			RESIST_FIRE,		/* P2 */
			SPIRITUAL_HAMMER,	/* P2 */

			MAGICAL_VESTAMENT,	/* P3 */
			WATER_WALK,			/* P3 */

			CURE_SERIOUS,		/* P4 */ //cure serious wounds
			PROT_FROM_EVIL,		/* P4 */

			CURE_CRITICAL,		/* P5 */ //cure critical wounds
			FLAME_STRIKE,		/* P5 */
			TRUE_SEEING,		/* P5 */

			HEAL,				/* P6 */

			FIRE_STORM,			/* P7 */
			HOLY_WORD,			/* P7 */
			RESURRECTION,		/* P7 */
			LAST_PRIEST_SPELL = RESURRECTION, 
			

#if 0
			DETECT_MAGIC = 0,   /* W1 */
			FEATHER_FALL,       /* W1 */
			JUMP,               /* W1 */
			LIGHT,              /* W1 */
			MAGIC_MISSILE,      /* W1 */
			KNOCK,              /* W2 */
			POISON_ARROW,       /* W2 */
			FIREBALL,           /* W3 */
			FLY,                /* W3 */
			ICE_STORM,          /* W4 */
			CONE_OF_COLD,       /* W5 */
			TELEPORT,           /* W5 */
			CHAIN_LIGHTNING,    /* W6 */
			DEATH,              /* W6 */
			IMPROVED_FIREBALL,  /* W7 */
			POWER_BLIND,        /* W8 */
			METEOR_SWARM,       /* W9 */
			POWER_KILL,         /* W9 */
			TIME_STOP,          /* W9 */
			
			CURE_LIGHT,         /* P1 */
			DETECT_EVIL,        /* P1 */
			DETECT_MAGIC_PR,    /* P1 */
			LIGHT_PR,           /* P1 */
			FIND_TRAPS,         /* P2 */
			FIND_TREASURE,      /* P2 */
			BARKSKIN,           /* P2 */
			MAGICAL_VESTAMENT,  /* P3 */
			CURE_SERIOUS,       /* P4 */
			CURE_CRITICAL,      /* P5 */
			TRUE_SEEING,        /* P5 */
			HEAL,               /* P6 */
			FIRE_STORM,         /* P7 */
			HOLY_WORD,          /* P7 */
#endif


			
			
			MAX_SPELL_INDEX,
		}	TYPE;
			

		
		typedef enum {
			LEVEL_0 = 0,  /* dummy */
			LEVEL_1 = 1,
			LEVEL_2 = 2,
			LEVEL_3 = 3,
			LEVEL_4 = 4,
			LEVEL_5 = 5,
			LEVEL_6 = 6,
			LEVEL_7 = 7,
			LEVEL_8 = 8,
			LEVEL_9 = 9,
			
			// (wisdom.hxx depends on this)
			MAX_PR_LEVELS = 8,
			MAX_LEVELS = 10
		} LEVEL;
	


 		class SPELL_ITEM
		{
			public:
		        CASTERTYPE      Type;
				LEVEL			Level;			// Spell Level
				REVERSABILTY	Reversable;
				SAVING_THROW	SavingThrow;			
				SAVETYPE		SaveType;
				SCHOOL			School;
				void			*pSpellObjects;	// Place holder for list of necessary material objects.
				// PLAYER_CLASS_INFO::TYPE		CanCast;
				UBYTE			WhereCast;
				USHORT			Sphere;			// Only used by Priest spells.
				
				void mfSetSphere(const SPHERE_TYPE);
				const BOOL mfCheckSphereType(const SPHERE_TYPE);
				
				inline CASTERTYPE mfGetType() const;
				inline LEVEL mfGetLevel() const;
				inline BOOL mfIsReversable() const;
				inline SAVING_THROW mfGetSavingThrow() const;
				inline SAVETYPE mfGetSaveType() const;
				inline SCHOOL mfGetSchool() const;
				inline CASTERLOC mfGetCastLocation() const;
				inline SPHERE_TYPE mfGetSphere() const;
		};
		static const SPELL_ITEM mfGetSpell(TYPE t);
		
	private:
		static const SPELL_ITEM SpellInfoTable[];
};

typedef SPELL_INFO::SPELL_ITEM *PTR_SPELL;

typedef struct _SPELL_SCHOOL_LIST 
{
	struct _SPELL_SCHOOL_LIST	*next;
	SPELL_INFO::SCHOOL			School;
} SPELL_SCHOOL_LIST, *PTR_SPELL_SCHOOL_LIST;

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
// Set a particular spell to a particular spell sphere.
inline void SPELL_INFO::SPELL_ITEM::mfSetSphere(const SPELL_INFO::SPHERE_TYPE spheretype)
{
	Sphere |= (UBYTE) (1 << spheretype);
}
		
// Test whether a particular spell is in a particular spell sphere
inline const BOOL SPELL_INFO::SPELL_ITEM::mfCheckSphereType(const SPELL_INFO::SPHERE_TYPE spheretype)
{
	return ((Sphere & (1<<spheretype)) == spheretype) ? TRUE : FALSE;
}

inline const SPELL_INFO::SPELL_ITEM SPELL_INFO::mfGetSpell(SPELL_INFO::TYPE t)
{
	return SpellInfoTable[(SHORT)t];
}

inline SPELL_INFO::CASTERTYPE SPELL_INFO::SPELL_ITEM::mfGetType() const
{
	return Type;
}

inline SPELL_INFO::LEVEL SPELL_INFO::SPELL_ITEM::mfGetLevel() const
{
	return Level;
}

inline BOOL SPELL_INFO::SPELL_ITEM::mfIsReversable() const
{
	return (Reversable == REVERSABLE);
}

inline SPELL_INFO::SAVING_THROW SPELL_INFO::SPELL_ITEM::mfGetSavingThrow() const
{
	return SavingThrow;
}

inline SAVETYPE SPELL_INFO::SPELL_ITEM::mfGetSaveType() const
{
	return SaveType;
}

inline SPELL_INFO::SCHOOL SPELL_INFO::SPELL_ITEM::mfGetSchool() const
{
	return School;
}

inline SPELL_INFO::CASTERLOC SPELL_INFO::SPELL_ITEM::mfGetCastLocation() const
{
	return (SPELL_INFO::CASTERLOC)WhereCast;
}

inline SPELL_INFO::SPHERE_TYPE SPELL_INFO::SPELL_ITEM::mfGetSphere() const
{
	return (SPELL_INFO::SPHERE_TYPE)Sphere;
}

#endif // _SPELL_HXX
