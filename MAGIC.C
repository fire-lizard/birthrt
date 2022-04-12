/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: magic.c  -
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "ENGINE.H"

#include "THINGTYP.H"
#include "MAGIC.H"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

#define COMMON_MAGIC_COUNT		100

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
   
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
static THINGTYPE	aCommonMagicItems[COMMON_MAGIC_COUNT] = {
	AMULET_OF_FEATHER_FALL  			,						
	AMULET_OF_SPEED         			,						
	AMULET_OF_STRIDING_AND_SPRINGING   	,						
	AMULET_VERSUS_UNDEAD 				,						
	AMULET_OF_WATER_WALKING             ,						
	BALM_OF_HEALING   					,						
//	BONE_WHISTLE   						,						
	BOOK_OF_EXALTED_DEEDS   			,						
	BOOK_OF_VILE_DARKNESS   			,						
//	BROOCH_OF_SHIELDING   				,						
	CHIME_OF_OPENING	 				,						
	CLOAK_OF_PROTECTION_1   			,						
	CLOAK_OF_PROTECTION_2   			,						
	CLOAK_OF_PROTECTION_3   			,						
//	DUST_OF_DISAPPEARANCE   			,						
//	ELIXIR_OF_HEALTH   					,						
	GAUNTLETS_OF_DEXTERITY   			,						
	GAUNTLETS_OF_OGRE_POWER   			,						
//	GEM_OF_ATTRACTION   				,						
	GEM_BLUE   							,						
	GEM_GREEN   						,						
	GEM_OF_PASSAGE   					,						
//	GEM_OF_SPELL_STORING   				,						
	GEM_OF_SHIFTING   					,						
//	HORN_OF_BLASTING   					,						
	IOUN_STONE_PALE_BLUE   				,						
	IOUN_STONE_SCARLET_AND_BLUE   		,						
	IOUN_STONE_INCANDESCENT_BLUE   		,						
	IOUN_STONE_DEEP_RED   				,						
	IOUN_STONE_PINK   					,						
	IOUN_STONE_PINK_AND_GREEN   		,						
//	IOUN_STONE_PALE_GREEN   			,						
	IOUN_STONE_PEARLY_WHITE   			,						
//	IOUN_STONE_PALE_LAVENDER   			,						
//	IOUN_STONE_LAVENDER_AND_GREEN   	,						
//	IOUN_STONE_VIBRANT_PURPLE   		,						
	IOUN_STONE_DUSTY_ROSE   			,						
	KEY_PURPLE   						,						
	KEY_BONE   							,						
	KEY_STONE   						,						
	LIBRIUM_OF_GAINFUL_CONJURATION   	,						
	LIBRIUM_OF_INEFFABLE_DAMNATION   	,						
	LIBRIUM_OF_SILVER_MAGIC   			,						
//	MIRROR_OF_SPELL_REFLECTION   		,						
	NECKLACE_OF_MISSILES_1   				,						
//	OGRE_DRUM   						,						
	OIL_OF_SHARPNESS_1  				,						
	OIL_OF_SHARPNESS_2  				,						
	OIL_OF_SHARPNESS_3  				,						
	OIL_OF_SHARPNESS_4  				,						
	OIL_OF_SHARPNESS_5  				,						
//	PERIAPT_AGAINST_POISONS   			,						
//	PIPES_OF_TRANSFORMATION   			,						
	POTION_OF_HILL_GIANT_STRENGTH   	,						
	POTION_OF_STONE_GIANT_STRENGTH   	,
	POTION_OF_FROST_GIANT_STRENGTH   	,
	POTION_OF_FIRE_GIANT_STRENGTH    	,
	POTION_OF_CLOUD_GIANT_STRENGTH   	,
	POTION_OF_STORM_GIANT_STRENGTH   	,
	POTION_OF_TITAN_STRENGTH   			,
//	POTION_OF_GROWTH       				,
	POTION_OF_DIMINUTION   				,
//	POTION_OF_INVISIBILITY   			,
	POTION_OF_INVULNERABILITY   		,
	POTION_OF_LEVITATION   				,
	RING_OF_FEATHER_FALLING   			,
	RING_OF_FIRE_FALLING   				,
//	RING_OF_INVISIBILITY   				,
	RING_OF_JUMPING   					,
	RING_OF_PROTECTION_1   				,
	RING_OF_PROTECTION_2   				,
	RING_OF_PROTECTION_3   				,
	RING_OF_PROTECTION_4   				,
	RING_OF_REGENERATION   				,
	RING_OF_RESISTANCE_1   				,
	RING_OF_RESISTANCE_2   				,
	RING_OF_RESISTANCE_3   				,
	RING_OF_NIGHTVISION   				,
//	DIERDRIENS_RING   					,
//	FAELES_RING   						,
	ROD_OF_RESURRECTION   				,
	SCROLL_OF_ARMOR_ENHANCEMENT_1   	,
	SCROLL_OF_ARMOR_ENHANCEMENT_2   	,
	SCROLL_OF_ARMOR_ENHANCEMENT_3   	,
	SCROLL_OF_ARMOR_ENHANCEMENT_4   	,
	SCROLL_OF_ARMOR_ENHANCEMENT_5   	,
//	SOUL_FLUTE   						,
	STAFF_OF_CURING   					,
//	STAFF_OF_POWER   					,
//	STONE_OF_LUCK   					,
	VIAL_OF_HOLY_WATER	 				,
	WAND_OF_ENEMY_DETECTION_1 	 		,
//	WAND_OF_FEAR 	 					,
	WAND_OF_FIRE_1 	 					,
//	WAND_OF_FIRE_EXTINGUISHING 	 		,
	WAND_OF_FROST_1 	 					,
	WAND_OF_ILLUMINATION_1 	 			,
	WAND_OF_LIGHTNING_1	 				,
	WAND_OF_MAGIC_DETECTION_1	 			,
	WAND_OF_PARALYZATION_1	 			,
	WAND_OF_SECRET_DETECTION_1	 		,
};
/* ------------------------------------------------------------------------
   Local Variables
   ------------------------------------------------------------------------ */
/* =======================================================================
   Function    - InitMagic
   Description - Set up the magic data for the wads.
   Returns     - 
	======================================================================= */
void InitMagicItems()
{
	// This is a fn, because in the future the data may come from a resource file.
	InitMagicThings((SHORT*)aCommonMagicItems, (sizeof(aCommonMagicItems)/ sizeof(THINGTYPE)), 1000, 1001);
}
