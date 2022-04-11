/* ========================================================================
   Copyright (c) 1990,1995	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: APPEAR.CPP
   Author:   John Conley (copied from ARMOR.CPP by Gary Powell)

   ========================================================================


   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <string.h>

#include "appear.hxx"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

#if 01
const APPEARANCE_INFO::APPEARANCE_ITEM APPEARANCE_INFO::fpAppearanceInfoTable[]
 = {
//  THINGTYPE
	LORD_FEMALE_1,
	LORD_FEMALE_2,
	LORD_FEMALE_3,
	LORD_MALE_1,
	LORD_MALE_2,
	LORD_MALE_3,
	LORD_MALE_4,
	LORD_MALE_5,
	LORD_MALE_6,
	LORD_MALE_7,
	GUILDER_MALE_1,
	GUILDER_MALE_2,
	GUILDER_FEMALE,
	PRIEST_MALE_1,
	PRIEST_MALE_2,
	PRIEST_FEMALE_1,
	WIZARD_FEMALE_1,
	WIZARD_FEMALE_2,
	WIZARD_MALE_1,
	WIZARD_MALE_2,
	WIZARD_MALE_4,
	WIZARD_MALE_3,
	DWARF_GUARD,
	DWARF_LORD_1,
	DWARF_LORD_3,
	DWARF_LORD_2,
	ELF_LORD_1,
	ELF_LORD_2,
	ELF_LADY_LORD,
	GOBLIN_LORD_1,
	GOBLIN_QUEEN,
	GNOLL_1,
	GORGON,
	MANSLAYER,
	SPIDER_KING,
};

APPEARANCE_INFO::TYPE APPEARANCE_INFO::IndexFromType (THINGTYPE iType)
{
// Given a THINGTYPE, return the index of the corresponding appearance item.

	LONG			iAppearance;

	for (iAppearance = FIRST_APPEARANCE; iAppearance < APPEARANCE_MAX_INDEX; iAppearance++)
	{
		if (fpAppearanceInfoTable[iAppearance].fGameType == iType)
			return (APPEARANCE_INFO::TYPE) iAppearance;
	}

	// If search fails, return the first item instead of crashing.
#if defined (_DEBUG)
	// Put a debug report here if desired.
#endif
	return FIRST_APPEARANCE;
}
#endif


