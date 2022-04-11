/* ========================================================================
   Copyright (c) 1997	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: APPEARANCE.HXX - Collection of appearance types
   Author: 	 John Conley (copied from ARMOR.HXX by Gary Powell)
   ======================================================================== */
#ifndef _APPEARANCE_HXX
#define _APPEARANCE_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include "typedefs.h"

// #include "playrace.hxx"
// #include "weapon.hxx"
#include "strenum.h"
#include "thingtyp.h"

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

class APPEARANCE_INFO {
public:
	typedef enum {
		FIRST_APPEARANCE = 0,
		CH_LORD_FEMALE_1,
		CH_LORD_FEMALE_2,
		CH_LORD_FEMALE_3,
		CH_LORD_MALE_1,
		CH_LORD_MALE_2,
		CH_LORD_MALE_3,
		CH_LORD_MALE_4,
		CH_LORD_MALE_5,
		CH_LORD_MALE_6,
		CH_LORD_MALE_7,
		CH_GUILDER_MALE_1,
		CH_GUILDER_MALE_2,
		CH_GUILDER_FEMALE,
		CH_PRIEST_MALE_1,
		CH_PRIEST_MALE_2,
		CH_PRIEST_FEMALE_1,
		CH_WIZARD_FEMALE_1,
		CH_WIZARD_FEMALE_2,
		CH_WIZARD_MALE_1,
		CH_WIZARD_MALE_2,
		CH_WIZARD_MALE_4,
		CH_WIZARD_MALE_3,
		CH_DWARF_GUARD,
		CH_DWARF_LORD_1,
		CH_DWARF_LORD_3,
		CH_DWARF_LORD_2,
		CH_ELF_LORD_1,
		CH_ELF_LORD_2,
		CH_ELF_LADY_LORD,
		CH_GOBLIN_LORD_1,
		CH_GOBLIN_QUEEN,
		CH_GNOLL_1,
		CH_GORGON,
		CH_MANSLAYER,
		CH_SPIDER_KING,
		// Insert new items above this line!
		APPEARANCE_MAX_INDEX
	} TYPE;

	class APPEARANCE_ITEM {
	public:
		friend class APPEARANCE_INFO;
		APPEARANCE_ITEM(THINGTYPE fGameType) { this->fGameType = fGameType; }
	private:
		THINGTYPE	fGameType;				// index to game_ttypes array
	};
	
	static char const * const mfGetName (const TYPE);
	static const THINGTYPE mfGetType (const TYPE);
	static TYPE IndexFromType (THINGTYPE iType);
	
private:
	static const APPEARANCE_ITEM fpAppearanceInfoTable[];
};
typedef APPEARANCE_INFO *PTR_APPEARANCE_INFO;

// Get the index to the game type array for this appearance (so we can
// get the name of the art file).
inline const THINGTYPE APPEARANCE_INFO::mfGetType (
	const APPEARANCE_INFO::TYPE AppearanceType)
{
	if (AppearanceType < FIRST_APPEARANCE || AppearanceType >= APPEARANCE_MAX_INDEX)
		// error--keep running by returning first one
		return fpAppearanceInfoTable[FIRST_APPEARANCE].fGameType;
	else
		return fpAppearanceInfoTable[AppearanceType].fGameType;
}

#endif // _APPEARANCE_HXX


