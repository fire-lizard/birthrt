/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: GAMETYPE.HXX    -Information about the objects in the game.
   Author: Gary Powell
   ======================================================================== */
#ifndef _GAMETYPE_HXX
#define _GAMETYPE_HXX

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <string.h>
#if defined(_RELEASE)
#include <stdio.h>
#include <io.h>
#endif

#if !defined(_STRENUM_H)
#include "strenum.h"
#endif



#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_MACHINE_H)
#include "machine.h"
#endif

#if !defined(_THINGS_H)
#include "things.h"
#endif

#if !defined(_SOUND_HXX)
#include "sound.hxx"
#endif

#if !defined(_THINGTYP_H)
#include "thingtyp.h"
#endif

#if !defined(_STRMGR_H)
#include "strmgr.h"
#endif

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
//GEH will eventually be in system.h
// JPC it is now in system.h: LONG FileAccess( CSTRPTR pszFile );

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/*
	ATTRIBUTES:
*/
// Note the first SHORT's worth of bits are set in things.h
#define NOATTS				0x00000000
#define PICKABLE			0x00010000
#define MONSTER		 	  	0x00020000
#define HUMANOID			0x00040000
#define ANIMAL		  		0x00080000
#define AUTO_AI				0x00100000
#define LOOPSOUND			0x00200000
#define MAGATTACK			0x00400000
#define NOATTACK2 			0x00800000
#define LG_AVATAR			0x01000000
#define RELICOPOWER			0x02000000


// Note: If you need a new attribute, just change the name "UNUSED" to
//       something we can all understand and remove the "//" from in the front
//       the line.
//       Now in gametypes.cpp use this define for the attrib section of the data
//       structure. To concatinate several attributes for one art object
//       join the defines with a "|" symbol.
//#define UNUSED	 			0x02000000
//#define UNUSED	 			0x04000000
//#define UNUSED	 			0x08000000
//#define UNUSED	 			0x10000000

// Note: the top three bits are used in things.h


typedef UBYTE MOVEMENT_RATE;


class GAME_TTYPE
{
public:
	class GAME_ITEM {
	public:
		friend class GAME_TTYPE;
		GAME_ITEM(TTYPE fttype, UBYTE fwalkRate, UBYTE fmarchRate, SOUND_TYPES fsoundType, GAME_STRING fdescription)
		{
			this->fttype = fttype;
			this->fwalkRate = fwalkRate;
			this->fmarchRate = fmarchRate;
			this->fsoundType = fsoundType;
			this->fdescription = fdescription;
		}
	private:
		// TTYPE Must be the first object in this class.
		TTYPE		  		fttype;
		UBYTE 		fwalkRate;
		UBYTE 		fmarchRate;
		SOUND_TYPES   		fsoundType;
		GAME_STRING  		fdescription;
	};
	
	// Access fns to the game_ttype array.
	
	inline static const BOOL mfCanBePickedUp(const THINGTYPE );
	inline static const BOOL mfIsMonster(const THINGTYPE );
	inline static const BOOL mfIsHuman(const THINGTYPE );
	inline static const BOOL mfIsAnimal(const THINGTYPE );
	inline static const BOOL mfIsAutoAIPossible(const THINGTYPE);
	inline static const BOOL mfIsGameType(const LONG);
	inline static const BOOL mfIsAvatarType(const LONG);
	inline static const BOOL mfCanFly(const THINGTYPE);
	inline static const BOOL mfIsCold(const THINGTYPE);
	inline static const BOOL mfIsHeatSource(const THINGTYPE);
	inline static const BOOL mfIsMagic(const THINGTYPE);
	inline static const BOOL mfIsEvil(const THINGTYPE);
	inline static const BOOL mfIsTreasure(const THINGTYPE);
	inline static const BOOL mfIsRelicOfPower(const THINGTYPE);
	inline static const BOOL mfHasMagicAnimation(const THINGTYPE);
	inline static const BOOL mfHasAttack2Animation(const THINGTYPE);
	inline static const SOUND_TYPES mfGetSoundType(const THINGTYPE );
	inline static const BOOL mfIsLoopingSound(const THINGTYPE );
	inline static char const * const mfGetDescription(const THINGTYPE);
	inline static LONG const mfGetSTRMgrId(const THINGTYPE);
	inline static const UBYTE mfGetWalkRate(const THINGTYPE);
	inline static const UBYTE mfGetMarchRate(const THINGTYPE);
	static const THINGTYPE mfGetThingType(char const * /* cpFileName */ );
	inline static const char *mfGetArtFileName(const THINGTYPE);
	inline static const BOOL mfIsLargeAvatar(const THINGTYPE);
	inline static THINGTYPE const mfGetLowResType(THINGTYPE const);
	static const BOOL mfIsMissileBlocker(const THINGTYPE);
	
	inline static void mfRegisterTTypes();

private:
	static inline void mfCheckValidTypeRange(LONG const /* type */);
	
	static GAME_ITEM const game_ttypes[];
};
typedef GAME_TTYPE *GAME_TTYPE_PTR;

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

// These inline access fns provide encapsualtion of how the data is stored
// Thus, optimizations for speed or size can be done without changing the
// code which calls them for the information.
// ie. The slices of the array may become stored as resources.
inline void GAME_TTYPE::mfCheckValidTypeRange(LONG const type )
{
#if defined (_DEBUG)
	if (type < 0 || type > LAST_GAME_THING)
	{
		fatal_error("ERROR! type %d out of range.", type);
	}
#endif
}

inline const BOOL GAME_TTYPE::mfIsGameType(const LONG type)
{
	return( (type <= LAST_GAME_THING) &&
	        (type >= FIRST_GAME_THING));
}

inline const BOOL GAME_TTYPE::mfCanBePickedUp(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return ( (game_ttypes[type].fttype.attrib & PICKABLE) ? TRUE : FALSE );
}

inline const BOOL GAME_TTYPE::mfIsMonster(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return ( (game_ttypes[type].fttype.attrib & MONSTER) ? TRUE : FALSE );
}

inline const BOOL GAME_TTYPE::mfIsHuman(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return ( (game_ttypes[type].fttype.attrib & HUMANOID) ? TRUE : FALSE );
}

inline const BOOL GAME_TTYPE::mfIsAnimal(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return ( (game_ttypes[type].fttype.attrib & ANIMAL) ? TRUE : FALSE );
}

inline const BOOL GAME_TTYPE::mfIsAutoAIPossible(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return ( (game_ttypes[type].fttype.attrib & AUTO_AI) ? TRUE : FALSE );
}

inline const SOUND_TYPES GAME_TTYPE::mfGetSoundType(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return (game_ttypes[type].fsoundType);
}							

inline const BOOL GAME_TTYPE::mfIsLoopingSound(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return ( (game_ttypes[type].fttype.attrib & LOOPSOUND) ? TRUE : FALSE );
}

inline char const * const GAME_TTYPE::mfGetDescription(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return ( STRMGR_GetStr(game_ttypes[type].fdescription));
}							

inline LONG const GAME_TTYPE::mfGetSTRMgrId(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return game_ttypes[type].fdescription;
}

inline const MOVEMENT_RATE GAME_TTYPE::mfGetWalkRate(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return (game_ttypes[type].fwalkRate);
}							

inline const MOVEMENT_RATE GAME_TTYPE::mfGetMarchRate(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return (game_ttypes[type].fmarchRate);
}							

inline const BOOL GAME_TTYPE::mfIsAvatarType(const LONG type)
{
	return(
	       (
	         (type <= LAST_GAME_AVATAR) &&
	         (type >= FIRST_GAME_THING)
	       )
	       ||
	       (
	         (type >= FIRST_FIEND) &&
	         (type <= LAST_FIEND)
	       )
	      );
}

// Engine level bit also used to deternin motion thru the world.
inline const BOOL GAME_TTYPE::mfCanFly(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return((game_ttypes[type].fttype.attrib & FLIES) ? TRUE : FALSE);
}

// Engine level bit also used to deternin magic issues.
inline const BOOL GAME_TTYPE::mfIsEvil(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return((game_ttypes[type].fttype.attrib & EVIL) ? TRUE : FALSE);
}

// Engine level bit also used to deternin magic issues.
inline const BOOL GAME_TTYPE::mfIsCold(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return((game_ttypes[type].fttype.attrib & COLD) ? TRUE : FALSE);
}

// Engine level bit also used to deternin magic issues.
inline const BOOL GAME_TTYPE::mfIsHeatSource(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return((game_ttypes[type].fttype.attrib & HEAT) ? TRUE : FALSE);
}

// Engine level bit also used to deternin magic issues.
inline const BOOL GAME_TTYPE::mfIsMagic(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return((game_ttypes[type].fttype.attrib & MAGIC) ? TRUE : FALSE);
}

// Engine level bit also used to deternin magic issues.
inline const BOOL GAME_TTYPE::mfIsTreasure(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return((game_ttypes[type].fttype.attrib & TREASURE) ? TRUE : FALSE);
}

// Engine level bit also used to deternin magic issues.
inline const BOOL GAME_TTYPE::mfIsRelicOfPower(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return ( (game_ttypes[type].fttype.attrib & RELICOPOWER) ? TRUE : FALSE );
}

// Used to determin the flics played during the various attack sequences.
inline const BOOL GAME_TTYPE::mfHasMagicAnimation(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return((game_ttypes[type].fttype.attrib & MAGATTACK) ? TRUE : FALSE);
}

// There is either 1 large attack1 sequence, and no attack2, two attacks,
// or one attack and one magic handwaving sequence.
inline const BOOL GAME_TTYPE::mfHasAttack2Animation(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return((game_ttypes[type].fttype.attrib & MAGATTACK ||
	        game_ttypes[type].fttype.attrib & NOATTACK2) ? FALSE : TRUE);
}

// Must be called before the wads are loaded so that the things in the
// wad will become part of the game.
inline void GAME_TTYPE::mfRegisterTTypes()
{
	RegisterTTypes((PTR) &game_ttypes[0], sizeof(GAME_TTYPE::GAME_ITEM), LAST_GAME_THING );
}

// Used to get other art file names like the portraits.
inline const char *GAME_TTYPE::mfGetArtFileName(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return (game_ttypes[type].fttype.name);
}

// Used to determin which damage dice are used against it.
inline const BOOL GAME_TTYPE::mfIsLargeAvatar(const THINGTYPE type)
{
	mfCheckValidTypeRange(type);
	return(game_ttypes[type].fttype.attrib & LG_AVATAR );
}

inline THINGTYPE const GAME_TTYPE::mfGetLowResType(THINGTYPE const type)
{
	mfCheckValidTypeRange(type);
	return ((THINGTYPE) (game_ttypes[type].fttype.LowResIndex));
}

#endif // _GAMETYPE_HXX


