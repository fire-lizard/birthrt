/* ========================================================================
   Copyright (c) 1990,1997	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: itemutil.cpp  - Utility fns for Item types.
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "ITEMUTIL.HXX"

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

/* ------------------------------------------------------------------------
   Local Variables
   ------------------------------------------------------------------------ */

/* ========================================================================
   Function    - SpellTypeToItemTypeIdx
   Description - Convert a spell to an Item.
   Returns     - 
   ======================================================================== */

ITEMTYPE SpellTypeToItemTypeIdx(SPELL_INFO::TYPE t)
{
	LONG i=0;

	for (i=0;i<MAX_ITEM_TYPES;++i)
		if (ItemTypes[i].mfCategory()==IC_SPELL)
			if(ItemTypes[i].mfGetExtendedData()==(LONG)t)
				break;
	return (ITEMTYPE)i;
}

/* ========================================================================
   Function    - ItemTypeToRlmSpellType
   Description - Convert a Itemtype to a spell
   Returns     - 
   ======================================================================== */

REALM_SPELL_INFO::TYPE ItemTypeToRlmSpellType(ITEMTYPE iType)
{
	if (ItemTypes[iType].mfCategory() == IC_REALM_SPELL)
	    return (REALM_SPELL_INFO::TYPE) ItemTypes[iType].mfGetExtendedData();
	    
    return REALM_SPELL_INFO::NO_SPELL;

}

LONG RlmSpellLevel(ITEMTYPE iType)
{
	REALM_SPELL_INFO::TYPE const spType = ItemTypeToRlmSpellType(iType);
	
	if (spType != REALM_SPELL_INFO::NO_SPELL)
		return (LONG) REALM_SPELL_INFO::mfGetRealmSpell(spType).mfGetLevel();
		
	return 0;
}
