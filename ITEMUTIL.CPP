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
   Function    - ItemTypeToSpellType
   Description - Convert a Itemtype to a spell
   Returns     - 
   ======================================================================== */

SPELL_INFO::TYPE ItemTypeToSpellType(ITEMTYPE iType)
{
	if (ItemTypes[iType].mfCategory() == IC_SPELL)
	    return (SPELL_INFO::TYPE) ItemTypes[iType].mfGetExtendedData();
	    
    return SPELL_INFO::NO_SPELL;

}

/* ========================================================================
   Function    - SpellLevel
   Description - Get the level of a spell given the item type.
   Returns     - The Spell level.
   ======================================================================== */

SPELL_INFO::LEVEL SpellLevel(ITEMTYPE iType)
{
	SPELL_INFO::TYPE const spType = ItemTypeToSpellType(iType);
	
	if (spType != SPELL_INFO::NO_SPELL)
		return SPELL_INFO::mfGetSpell(spType).mfGetLevel();
		
	return SPELL_INFO::LEVEL_0;
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
