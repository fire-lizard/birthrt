/* ========================================================================
   Copyright (c) 1990,1996   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: Itemtype.cpp  -Handles the attributes of gettable items.
   Author:   Wes Cumberland
   ========================================================================
   Contains the following class declarations:
   ItemType -This contains information about a type of gettable
             object.
   
   ItemEffects -This is a wrapper class to keep all the various item effect
                functions out of the global namespace

   
   Contains the following general inline functions:
   ItemType::
    ItemType()    -the constructor, nothing interesting
    mfActivate()  -Runs the effect function of the Item
    mfGameType()  -returns the GameType
    mfGetIcon()   -returns the Handle to the icon resource
    mfPrint()     -debug func that prints the item's name to stdout
    mfInfo()      -returns the pointer to the information about the item
    mfIconFile()  -returns the pointer to the filename of the item's icon
    mfCategory()  -returns the item's category
    mfGetExtendedData() -returns the ExtendedData Index
   ======================================================================== */
#ifndef ITEMTYPE_HXX
#define ITEMTYPE_HXX
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif


#if !defined(_GAMETYPE_HXX)
#include "gametype.hxx"   
#endif

// Not Used #if !defined(_ARMOR_HXX)
// Not Used #include "armor.hxx"
// Not Used #endif
// Not Used 
// Not Used #if !defined(_WEAPON_HXX)
// Not Used #include "weapon.hxx"
// Not Used #endif


#if !defined(_STRENUM_H)
#include "strenum.h"
#endif

#if !defined(_STRMGR_H)
#include "strmgr.h"
#endif

#if !defined(_SCNMGR_HXX)
#include "scnmgr.hxx"
#endif

#if !defined(_STRENUM_H)
#include "strenum.h"   
#endif

#if !defined(_VECTOR_HXX)
#include "vector.hxx"   
#endif

#if !defined(_TASK_H)
#include "task.h"   
#endif

#if !defined(_TMSGBOX_HXX)
#include "tmsgbox.hxx"   
#endif


/* ------------------------------------------------------------------------
   Typedefs and defines
   ------------------------------------------------------------------------ */


typedef enum 
{
/* 000 */	ITEM_NO_THING,

// Mundane Items

/* 001 */	ITEM_BOWL_1,
/* 002 */	ITEM_BUCKET_1,
/* 003 */	ITEM_CANDLE_1,
/* 004 */	ITEM_CUP_1,
/* 005 */	ITEM_FLAGON_1,
/* 006 */	ITEM_HELMET_1,
/* 007 */	ITEM_KNIFE_1,
/* 008 */	ITEM_PLATE_1,
/* 009 */	ITEM_SPEAR_1,
/* 010 */	ITEM_SWORD_1,

/* 011 */	ITEM_KEY_BLACK,
/* 012 */	ITEM_KEY_BLUE,
/* 013 */	ITEM_KEY_BONE,
/* 014 */	ITEM_KEY_JEWELED,
/* 015 */	ITEM_KEY_RED,
/* 016 */	ITEM_KEY_SILVER,
/* 017 */	ITEM_KEY_WHITE,
/* 018 */	ITEM_KEY_SKELETON,
/* 019 */	ITEM_KEY_STONE,
/* 020 */	ITEM_KEY_TINY,

// Magic Items

/* 021 */	ITEM_AMULET_OF_FEATHER_FALL,
/* 022 */	ITEM_AMULET_OF_SPEED,
/* 023 */	ITEM_AMULET_OF_STRIDING_AND_SPRINGING,
/* 024 */	ITEM_AMULET_VERSUS_UNDEAD,
/* 025 */	ITEM_AMULET_OF_WATER_WALKING,
/* 026 */	ITEM_BALM_OF_HEALING,
/* 027 */	ITEM_BOOK_OF_EXALTED_DEEDS, //temp
/* 028 */	ITEM_BOOK_OF_VILE_DARKNESS,
/* 029 */	ITEM_CHIME_OF_OPENING,
/* 030 */	ITEM_CLOAK_OF_PROTECTION_1,
/* 031 */	ITEM_CLOAK_OF_PROTECTION_2,
/* 032 */	ITEM_CLOAK_OF_PROTECTION_3,
/* 033 */	ITEM_GAUNTLETS_OF_DEXTERITY,
/* 034 */	ITEM_GAUNTLETS_OF_OGRE_POWER,
/* 035 */	ITEM_GEM_BLUE,
/* 036 */	ITEM_GEM_GREEN,
/* 037 */	ITEM_GEM_OF_PASSAGE,
/* 038 */	ITEM_GEM_OF_SHIFTING,
/* 039 */	ITEM_IOUN_STONE_PALE_BLUE,
/* 040 */	ITEM_IOUN_STONE_SCARLET_AND_BLUE,
/* 041 */	ITEM_IOUN_STONE_INCANDESCENT_BLUE,
/* 042 */	ITEM_IOUN_STONE_DEEP_RED,
/* 043 */	ITEM_IOUN_STONE_PINK,
/* 044 */	ITEM_IOUN_STONE_PINK_AND_GREEN,
/* 045 */	ITEM_IOUN_STONE_PEARLY_WHITE,
/* 046 */	ITEM_IOUN_STONE_DUSTY_ROSE,
/* 047 */	ITEM_LIBRIUM_OF_GAINFUL_CONJURATION,
/* 048 */	ITEM_LIBRIUM_OF_INEFFABLE_DAMNATION,
/* 049 */	ITEM_LIBRIUM_OF_SILVER_MAGIC,
/* 050 */	ITEM_NECKLACE_OF_MISSILES_8,
/* 051 */	ITEM_OIL_OF_SHARPNESS_1,
/* 052 */	ITEM_OIL_OF_SHARPNESS_2,
/* 053 */	ITEM_OIL_OF_SHARPNESS_3,
/* 054 */	ITEM_OIL_OF_SHARPNESS_4,
/* 055 */	ITEM_OIL_OF_SHARPNESS_5,
/* 056 */	ITEM_POTION_OF_HILL_GIANT_STRENGH,
/* 057 */	ITEM_POTION_OF_STONE_GIANT_STRENGH,
/* 058 */	ITEM_POTION_OF_FROST_GIANT_STRENGH,
/* 059 */	ITEM_POTION_OF_FIRE_GIANT_STRENGH,
/* 060 */	ITEM_POTION_OF_CLOUD_GIANT_STRENGH,
/* 061 */	ITEM_POTION_OF_STORM_GIANT_STRENGH,
/* 062 */	ITEM_POTION_OF_TITAN_STRENGH,
/* 063 */	ITEM_POTION_OF_DIMINUATION,
/* 064 */	ITEM_POTION_OF_INVULNERABILITY,
/* 065 */	ITEM_POTION_OF_LEVITATION,
/* 066 */	ITEM_POTION_OF_FIRE_RESISTANCE,
/* 067 */	ITEM_POTION_OF_FLYING,
/* 068 */	ITEM_POTION_OF_HEALING,
/* 069 */	ITEM_POTION_OF_EXTRA_HEALING,
/* 070 */	ITEM_RING_OF_FEATHER_FALLING,
/* 071 */	ITEM_RING_OF_FIRE_FALLING,
/* 072 */	ITEM_RING_OF_JUMPING,
/* 073 */	ITEM_RING_OF_PROTECTION_1,
/* 074 */	ITEM_RING_OF_PROTECTION_2,
/* 075 */	ITEM_RING_OF_PROTECTION_3,
/* 076 */	ITEM_RING_OF_PROTECTION_4,
/* 077 */	ITEM_RING_OF_REGENERATION,
/* 078 */	ITEM_RING_OF_RESISTANCE_1,
/* 079 */	ITEM_RING_OF_RESISTANCE_2,
/* 080 */	ITEM_RING_OF_RESISTANCE_3,
/* 081 */	ITEM_RING_OF_NIGHTVISION,
/* 082 */	ITEM_ROD_OF_RESURRECTION,
/* 083 */	ITEM_SCROLL_OF_ARMOR_ENHANCEMENT_1,
/* 084 */	ITEM_SCROLL_OF_ARMOR_ENHANCEMENT_2,
/* 085 */	ITEM_SCROLL_OF_ARMOR_ENHANCEMENT_3,
/* 086 */	ITEM_SCROLL_OF_ARMOR_ENHANCEMENT_4,
/* 087 */	ITEM_SCROLL_OF_ARMOR_ENHANCEMENT_5,
/* 088 */	ITEM_STAFF_OF_CURING,
/* 089 */	ITEM_VIAL_OF_HOLY_WATER,
/* 090 */	ITEM_WAND_OF_ENEMY_DETECTION_3,
/* 091 */	ITEM_WAND_OF_FIRE_3,
/* 092 */	ITEM_WAND_OF_FROST_3,
/* 093 */	ITEM_WAND_OF_ILLUMINATION_3,
/* 094 */	ITEM_WAND_OF_LIGHTNING_3,
/* 095 */	ITEM_WAND_OF_MAGIC_DETECTION_3,
/* 096 */	ITEM_WAND_OF_PARALYZATION_3,
/* 097 */	ITEM_WAND_OF_SECRET_DETECTION_3,
/* 098 */	ITEM_CANDLE_OF_INVOCATION,
/* 099 */	ITEM_PARCHMENT_1,
/* 100 */	ITEM_PARCHMENT_2,
/* 101 */	ITEM_PARCHMENT_3,
/* 102 */	ITEM_PARCHMENT_4,
/* 103 */	ITEM_PARCHMENT_5,

// Relics of Power
/* 104 */	ITEM_DIERDRIENS_RING,
/* 105 */	ITEM_FAELES_RING,
/* 106 */	ITEM_GAVELONS_STAFF_OF_PROSPERITY,
/* 107 */	ITEM_AMULET_OF_INSPIRATION,
/* 108 */	ITEM_BANNER_OF_ROELE,
/* 109 */	ITEM_BARAZADS_TOOLS,
/* 110 */	ITEM_BRENNAS_FAVOR,
/* 111 */	ITEM_CHALICE_OF_THE_DEAD,
/* 112 */	ITEM_CROWN_OF_COMMAND,
/* 113 */	ITEM_STATE_CROWN_OF_ANNUIRE,
/* 114 */	ITEM_EMPERORS_CROWN,
/* 115 */	ITEM_FARIDS_COFFER_OF_THE_REALM,
/* 116 */	ITEM_HAMMER_OF_THUNDER,
/* 117 */	ITEM_RING_OF_WIZARDRY,
/* 118 */	ITEM_ROBES_OF_THE_MASES,
/* 119 */	ITEM_REGALIA_OF_THE_EMPIRE,
/* 120 */	ITEM_SIELSHEGH_GEM_LARGE,
/* 121 */	ITEM_SIELSHEGH_GEM_SMALL,
/* 122 */	ITEM_SWORD_OF_ROELE,
/* 123 */	ITEM_SCEPTRE_OF_CUIRAECEN,
/* 124 */	ITEM_CORGANDALS_STAFF,
/* 125 */	ITEM_NAPPOLANS_TOME_OF_WAR,
/* 126 */	ITEM_VAUBENELS_BOOK_OF_FORTIFICATION,
/* 127 */	ITEM_TORC_OF_SPLENDOR,
/* 128 */	ITEM_DANICAS_CRYSTAL_OF_SCRYING,
/* 129 */	ITEM_TOME_OF_MYSTIC_MAGICS,
/* 130 */	ITEM_TOME_OF_SORCERORS_LORE,
/* 131 */	ITEM_TOME_OF_THE_HIGH_WIZARDS,
/* 132 */	ITEM_TOME_OF_THE_MAGE_LORDS,

//wizard spells
/* 133 */	ITEM_SPELL_DETECT_MAGIC,	
/* 134 */	ITEM_SPELL_FEATHER_FALL,
/* 135 */	ITEM_SPELL_JUMP,
/* 136 */	ITEM_SPELL_LIGHT,
/* 137 */	ITEM_SPELL_MAGIC_MISSILE,
/* 138 */	ITEM_SPELL_REDUCE,
/* 139 */	ITEM_SPELL_SHOCKING_GRASP,
/* 140 */	ITEM_SPELL_BLINDNESS,
/* 141 */	ITEM_SPELL_CONTINUAL_LIGHT,
/* 142 */	ITEM_SPELL_KNOCK,
/* 143 */	ITEM_SPELL_LEVITATE,
/* 144 */	ITEM_SPELL_LOCATE_OBJECT,
/* 145 */	ITEM_SPELL_POISON_ARROW,
/* 146 */	ITEM_SPELL_STRENGTH,
/* 147 */	ITEM_SPELL_FIREBALL,
/* 148 */	ITEM_SPELL_FLY,
/* 149 */	ITEM_SPELL_STONE_UNDEAD,
/* 150 */	ITEM_SPELL_INFRAVISION,
/* 151 */	ITEM_SPELL_LIGHTNING_BOLT,
/* 152 */	ITEM_SPELL_CONFUSION,
/* 153 */	ITEM_SPELL_ICE_STORM,
/* 154 */	ITEM_SPELL_MINOR_GLOBE_INVULN,
/* 155 */	ITEM_SPELL_STONESKIN,
/* 156 */	ITEM_SPELL_CONE_OF_COLD,
/* 157 */	ITEM_SPELL_DISINTIGRATE,
/* 158 */	ITEM_SPELL_TELEPORT,
/* 159 */	ITEM_SPELL_CHAIN_LIGHTNING,
/* 160 */	ITEM_SPELL_DEATH,		
/* 161 */	ITEM_SPELL_GLOBE_INVULN,			
/* 162 */	ITEM_SPELL_FLESH_TO_STONE,		
/* 163 */	ITEM_SPELL_IMPROVED_FIREBAL,	
/* 164 */	ITEM_SPELL_POWER_STUN,		
/* 165 */	ITEM_SPELL_POWER_BLIND,			
/* 166 */	ITEM_SPELL_SPELL_IMMUNITY,	
/* 167 */	ITEM_SPELL_METEOR_SWARM,		
/* 168 */	ITEM_SPELL_POWER_KILL,			
/* 169 */	ITEM_SPELL_TIME_STOP,			
     
//priest spells
/* 170 */	ITEM_SPELL_CURE_LIGHT,				
/* 171 */	ITEM_SPELL_DETECT_EVIL,		
/* 172 */	ITEM_SPELL_DETECT_MAGIC_PR,	
/* 173 */	ITEM_SPELL_LIGHT_PR,			
/* 174 */	ITEM_SPELL_SHILLELAGH,			
/* 175 */	ITEM_SPELL_TURN_UNDEAD,		
/* 176 */	ITEM_SPELL_BARKSKIN,			
/* 177 */	ITEM_SPELL_FIND_TRAPS,			
/* 178 */	ITEM_SPELL_FIND_TREASURE,		
/* 179 */	ITEM_SPELL_RESIST_FIRE,		
/* 180 */	ITEM_SPELL_SPIRITUAL_HAMMER,	
/* 181 */	ITEM_SPELL_MAGICAL_VESTAMENT,	
/* 182 */	ITEM_SPELL_WATER_WALK,			
/* 183 */	ITEM_SPELL_CURE_SERIOUS,		
/* 184 */	ITEM_SPELL_PROT_FROM_EVIL,		
/* 185 */	ITEM_SPELL_CURE_CRITICAL,		
/* 186 */	ITEM_SPELL_FLAME_STRIKE,		
/* 187 */	ITEM_SPELL_TRUE_SEEING,		
/* 188 */	ITEM_SPELL_HEAL,			
/* 189 */	ITEM_SPELL_FIRE_STORM,				
/* 190 */	ITEM_SPELL_HOLY_WORD,			
/* 191 */	ITEM_SPELL_RESURRECTION,		



// Wizard Realm Spells

/* 192 */	ITEM_SPELL_ALCHEMY,
/* 193 */	ITEM_SPELL_DEATH_PLAGUE,
/* 194 */	ITEM_SPELL_DEMAGOGUE,
/* 195 */	ITEM_SPELL_DISPEL_REALM_MAGIC,
/* 196 */	ITEM_SPELL_LEGION_OF_DEAD,
/* 197 */	ITEM_SPELL_MASS_DESTRUCTION,
/* 198 */	ITEM_SPELL_RAZE,
/* 199 */	ITEM_SPELL_SCRY,
/* 200 */	ITEM_SPELL_STRONGHOLD,
/* 201 */	ITEM_SPELL_SUMMONING,
/* 202 */	ITEM_SPELL_TRANSPORT,
/* 203 */	ITEM_SPELL_WARDING,
	  

// Priest Realm Spells

/* 204 */	ITEM_SPELL_BLESS_LAND,
/* 205 */	ITEM_SPELL_BLESS_ARMY,
/* 206 */	ITEM_SPELL_BLIGHT,
/* 207 */	ITEM_SPELL_DISPEL_REALM_MAGIC_PRIESTVER,
/* 208 */	ITEM_SPELL_HONEST_DEALING,
/* 209 */	ITEM_SPELL_INVESTITURE,


// Blood Abilities

/* 210 */	ITEM_BATTLEWISE,        
/* 211 */	ITEM_COURAGE,           
/* 212 */	ITEM_DEVINE_WRATH,      
/* 213 */	ITEM_HEIGHTENED_ABILITY,
/* 214 */	ITEM_BLOODMARK,
/* 215 */	ITEM_DETECT_ILLUSION,
/* 216 */	ITEM_DIVINE_AURA,
/* 217 */	ITEM_ENHANCED_SENSE,
/* 218 */	ITEM_FEAR,
/* 219 */	ITEM_HEALING_MINOR,
/* 220 */	ITEM_HEALING_MAJOR,
/* 221 */	ITEM_HEALING_GREAT,
/* 222 */	ITEM_IRON_WILL,
/* 223 */	ITEM_PROTECTION_FROM_EVIL,
/* 224 */	ITEM_REGENERATION,
/* 225 */	ITEM_RESISTANCE,
/* 226 */	ITEM_TRAVEL,
/* 227 */	ITEM_SM_GOLD_COINS,
/* 228 */	ITEM_LG_GOLD_COINS,
/* 229 */	ITEM_SM_GOLD_STACK,
/* 230 */	ITEM_LG_GOLD_STACK,
/* 231 */	ITEM_BAG_GOLD_COINS,
/* 232 */	ITEM_CHEST_GOLD_COINS,
/* 233 */	ITEM_GORGONS_BANNER,
/* 234 */	ITEM_CAPTIVE_1,
/* 235 */	ITEM_CAPTIVE_2,
/* 236 */	ITEM_IOUN_HEART,
/* 237 */	ITEM_IOUN_FIST,
/* 238 */	ITEM_TOME_OF_THE_PRINCE,

/* 239 */	ITEM_NECKLACE_OF_MISSILES_7,
/* 240 */	ITEM_NECKLACE_OF_MISSILES_6,
/* 241 */	ITEM_NECKLACE_OF_MISSILES_5,
/* 242 */	ITEM_NECKLACE_OF_MISSILES_4,
/* 243 */	ITEM_NECKLACE_OF_MISSILES_3,
/* 244 */	ITEM_NECKLACE_OF_MISSILES_2,
/* 245 */	ITEM_NECKLACE_OF_MISSILES_1,

/* 246 */	ITEM_WAND_OF_ENEMY_DETECTION_2,
/* 247 */	ITEM_WAND_OF_ENEMY_DETECTION_1,
/* 248 */	ITEM_WAND_OF_FIRE_2,
/* 249 */	ITEM_WAND_OF_FIRE_1,
/* 250 */	ITEM_WAND_OF_FROST_2,
/* 251 */	ITEM_WAND_OF_FROST_1,
/* 252 */	ITEM_WAND_OF_ILLUMINATION_2,
/* 253 */	ITEM_WAND_OF_ILLUMINATION_1,
/* 254 */	ITEM_WAND_OF_LIGHTNING_2,
/* 255 */	ITEM_WAND_OF_LIGHTNING_1,
/* 256 */	ITEM_WAND_OF_MAGIC_DETECTION_2,
/* 257 */	ITEM_WAND_OF_MAGIC_DETECTION_1,
/* 258 */	ITEM_WAND_OF_PARALYZATION_2,
/* 259 */	ITEM_WAND_OF_PARALYZATION_1,
/* 260 */	ITEM_WAND_OF_SECRET_DETECTION_2,
/* 261 */	ITEM_WAND_OF_SECRET_DETECTION_1,

/* 262 */	ITEM_ABILITY_TURN_UNDEAD,
/* 263 */	ITEM_ABILITY_LAY_ON_HANDS,

/* 264 */	MAX_ITEM_TYPES
} ITEMTYPE;


//action types for ItemEffects
#define ACTIVATE	0x01
#define DEACTIVATE	0x02
#define PICKUP		0x03
#define DROP		0x04
#define EQUIP		0x05
#define UNEQUIP		0x06
#define SERVICE		0x07	//once each frame (for regeneration, mainly)
#define DOUBLEUSE	0x08

//return Types for ItemEffects

//general return codes (used by all actions)
#define OK				0xFF00
#define NOT_SUPPORTED	0xFE00  //no case for action
#define CANT_COMPLETE	0xFD00  //can't drop  etc...
#define NOT_OK			0xFC00	//generally used when the effect function 
								//takes care of whatever it needs to do but 
								//doesn't want the caller to do anything 
								//(such as add to invntory or spell boxes etc...)

//return codes for picking up stuff
#define DONT_ADD		0xFB00	//just run the effect, but don't add item (GoldCoins)
#define DONT_PICKUP		0xFA00	//don't pick the item up, but run effect (Parchment)

//return codes for dropping stuff
#define DONT_DROP		0xFB00	//DROP effect only
#define DECREMENT_CHARGE	0xFA00

//return codes for equipping stuff
#define ALWAYS_IN_EFFECT 0xFB00 //Puts up the "always in effect" message


#define PER_LEVEL     0x8000


#define MAX_ACTIVE_ITEMS 80
#define INDEFINATE_DUR	 0



#define SPLIT_COMBO(l,s1,s2)	{((s1) = (ITEMTYPE)((0xFFFF0000 & (l))>>16));\
								 ((s2) = (short)(0x0000FFFF & (l)));}


//Item categories
typedef enum {
	IC_MUNDANE = 0,
	IC_MAGITEM,
	IC_SPELL,
	IC_REALM_SPELL,
	IC_BLOODAB,
	IC_WEAPON,
	IC_ARMOR,
	IC_ALL,
} ItemCategory;


//Defines for effects...

#define FALL_RATE               6	// GEH HACK ALERT, THESE ARE ALREADY DEFINED
#define TERMINAL_VELOCITY       80	//  IN PLAYER.CPP
extern LONG PlayerFallRate;
extern LONG PlayerTermVelocity;

#define PARTY  0x12345678


#define AT_STR				0x00010000
#define AT_INT				0x00020000
#define AT_WIS				0x00040000
#define AT_DEX				0x00080000
#define AT_CON				0x00100000
#define AT_CHA				0x00200000
#define AT_LEV				0x00400000
#define AT_ALL				0x007F0000

#define AT_HP				0x00800000
#define AT_AC				0x01000000

#define AT_MISSILE			0x01000000
#define AT_FIRE				0x02000000
#define AT_PLASMA			0x04000000
#define AT_LIGHTNING		0x08000000
#define AT_VAPORIZE			0x10000000
#define AT_CRUMBLE			0x20000000
#define USE_LEVEL			0x40000000
#define CLASS_RESTRICTED	0x80000000

#define _RESIST_FIRE		0x00010000
#define _RESIST_COLD		0x00020000
#define _RESIST_EVIL		0x00040000
#define _RESIST_MAGIC		0x00080000
#define _RESIST_FEAR		0x00100000

#define REGEN_TURN			0x01000000
#define REGEN10SECS			0x0A000000


#define MAKE_DICE(sb1,sb2,sb3)	(((sb1)<<16|(sb2)<<8|(sb3))&0x00FFFFFF)
#define DICE_NUMDICE(a)			((a&0x00FF0000) >> 16)
#define DICE_NUMSIDES(a)		((a&0x0000FF00) >> 8)
#define DICE_MODIFIER(a)		((a&0x000000FF) >> 0)
//#define CHARGES(a)			((a&0xF0000000) >>28)


#define HEAL_DATA(a)		a&0x00FFFFFF
#define HEAL_MASK			0x0F000000
#define HEAL_PERCENT		0x01000000
#define HEAL_ABSOLUT		0x02000000
#define HEAL_RANDOM			0x03000000
#define HEAL_FULLY			0x04000000




#define MAKE_ADD_SPELL(num,low,hi) MAKE_DICE(num,low,hi)
#define ADD_REALM 0x01000000

#define PROT_RING 	0x00010000
#define PROT_CLOAK	0x00020000


/* ------------------------------------------------------------------------
   Global Variable
   ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------
   External Functions
   ------------------------------------------------------------------------ */
extern void AddGameKeys(void);

extern "C" extern MapHighlightData MapHighlight;
	
/* ------------------------------------------------------------------------
   Classes
   ------------------------------------------------------------------------ */

#define MAX_CHARGES 10

class ItemChargeHandler
{
	private:
		class ChargedItemChain
		{
			private:
				LONG numCharges;
				ITEMTYPE ItemTypeChain[MAX_CHARGES];
			public:
				ChargedItemChain(LONG,ITEMTYPE=ITEM_NO_THING,ITEMTYPE=ITEM_NO_THING,ITEMTYPE=ITEM_NO_THING,ITEMTYPE=ITEM_NO_THING,ITEMTYPE=ITEM_NO_THING,ITEMTYPE=ITEM_NO_THING,ITEMTYPE=ITEM_NO_THING,ITEMTYPE=ITEM_NO_THING,ITEMTYPE=ITEM_NO_THING,ITEMTYPE=ITEM_NO_THING);
				LONG mfCharges();
				ITEMTYPE operator[](LONG a);
		};
		
		static DECL_VECTOR_CLASS(ChargedItemChain,ChargeDegradeTable);
		static LONG NumChargedItems;	
	public:
		static BOOL mfFind(ITEMTYPE,LONG *,LONG *);
		static void mfDropCharge(ITEMTYPE ItemTypeIdx,SHORT hAvatarStats);
		static LONG mfCharges(ITEMTYPE ItemTypeIdx);
};
		
	

/* ========================================================================
   Class       - ItemType
   Description - This contains information about a type of gettable
                 object, mainly it's effect function and its gametype 
   Size        - 32 bytes
   ======================================================================== */

typedef enum
{
	HANDWAVE	=0x01,
	TARGET		=0x02,
	TARGETWAVE	=0x03,	// both
	CHARGES		=0x10,	// Needs to be charged to use.
} REQUISIT;
		
class ItemType
{
	private:
		ItemCategory Category;
		UBYTE Attribs;
		THINGTYPE GameType;
		PFLLLL  EffectFunc;
		LONG  EffectArg;
		SHORT  Duration;
		char  IconFile[13];
//		const char* MiscInfo; //short description for Description Box
		LONG MiscInfo; //short description for Description Box


		SHORT ExtendedDataIdx; //this is used to index into a table
							   //appropriate to the category.

	public:
		


		ItemType(ItemCategory,UBYTE,SHORT,THINGTYPE,PFLLLL,LONG,SHORT,char*,LONG);
		ItemType();

		LONG mfEffect(LONG,LONG);

		THINGTYPE mfGameType() 		const;
		void mfPrint() 	       		const;
		SHORT mfInfo() 			const;
		const char* mfIconFile()	const;
		SHORT const mfGetIconOn()	const;
		SHORT const mfGetIconOff()	const;
		SHORT mfGetExtendedData() 	const;
		const char* mfGetName()		const;
		SHORT mfDuration(SHORT)		const;

		GAME_STRING const mfGetSTRMgrId() const;
		ItemCategory mfCategory()	const;
		
		BOOL  mfRequires(REQUISIT);
			
		void mfMessage(GAME_STRING) const;
};




/* ========================================================================
   Class       - ItemEffects
   Description - This encapsulates all the effects of all the items, so that
                 they don't intrude on the global namespace
   Size        - 0 bytes
   ======================================================================== */
class ItemEffects
{	
	//friends
	friend class InventoryItemType;
	friend class ObjectList;
	friend void AddGameKeys(void);
	friend class ItemType;

	private: 
		class ActiveItem
		{
			private:
				PFLLLL Func;
				LONG Arg1;
				LONG Arg2;
				LONG TimeToStop;
				BOOL Valid;
				BOOL AutoRemove;
				
			public:
				friend class ItemEffects;
				
				ActiveItem();
				ActiveItem(PFLLLL,LONG,LONG,LONG);
				LONG mfTicksLeft();
				LONG mfSecondsLeft();
		};
		
			
				
				

		static DECL_VECTOR_CLASS(ActiveItem, ActiveItems);
		//static ActiveItem ActiveItems[MAX_ACTIVE_ITEMS];
				
		static void ServiceDurations(LONG);
		static BOOL mfAddActiveItem(PFLLLL,LONG,LONG,LONG,BOOL=FALSE);


		static ActiveItem* mfGetActiveItem(PFLLLL,LONG,LONG);
		static void mfRemoveActiveItem(PFLLLL,LONG,LONG);

		static void mfMessageBox(char const *,char const*);
		static void mfSound( BIRTHRT_SND  , SHORT , LONG);

		

	public:
		static void WipeActiveItems(); 
		static void DeactivateAllItems();
		static void DeactivateAvatarsItems(SHORT hPlayerStats);
		static void InitDurationServicing(){add_task(1,ServiceDurations,0);}
		static void DeinitDurationServicing(){remove_task(ServiceDurations);}
		static void mfPauseDurations(LONG const /* IncrementDuration */);

		static LONG Rescuee				(LONG,LONG,LONG);

		static LONG None				(LONG,LONG,LONG);
		static LONG Template			(LONG,LONG,LONG);
		static LONG Generic				(LONG,LONG,LONG);
		static LONG KeyExplanation		(LONG,LONG,LONG);
		static LONG GoldCoins			(LONG,LONG,LONG);
		static LONG FeatherFall			(LONG,LONG,LONG);
		static LONG RingFeatherFall		(LONG,LONG,LONG);
		static LONG Speed				(LONG,LONG,LONG);
		static LONG StrideAndSpring		(LONG,LONG,LONG);
		static LONG UndeadTurn			(LONG,LONG,LONG);
		static LONG WaterWalk			(LONG,LONG,LONG);
		static LONG Heal				(LONG,LONG,LONG);
		static LONG TouchSpell			(LONG,LONG,LONG);
		static LONG SummonSkeletons		(LONG,LONG,LONG);
		static LONG BookOfExaltedDeeds	(LONG,LONG,LONG);
		static LONG BookOfVileDarkness	(LONG,LONG,LONG);
		static LONG CloakProtect		(LONG,LONG,LONG);
		static LONG GauntletsOfDex		(LONG,LONG,LONG);
		static LONG GuantletsOfOgrePow	(LONG,LONG,LONG);
		static LONG AddToAttribute		(LONG,LONG,LONG);
		static LONG Regenerate			(LONG,LONG,LONG);
		static LONG RegenerateIoun		(LONG,LONG,LONG);
		static LONG LibrOfConjuration	(LONG,LONG,LONG);
		static LONG LibrOfDamnation		(LONG,LONG,LONG);
		static LONG LibrSilver			(LONG,LONG,LONG);
		static LONG ImproveWeapon		(LONG,LONG,LONG);
		static LONG PeriaptVsPoisons	(LONG,LONG,LONG);
		static LONG SetAttrTo			(LONG,LONG,LONG);
		static LONG Grow				(LONG,LONG,LONG);
		static LONG Shrink				(LONG,LONG,LONG);
		static LONG Invulnerable		(LONG,LONG,LONG);
		static LONG Levitate			(LONG,LONG,LONG);
		static LONG Flight				(LONG,LONG,LONG);
		static LONG ResistFire			(LONG,LONG,LONG);
		static LONG Invisibility		(LONG,LONG,LONG);
		static LONG RingJump			(LONG,LONG,LONG);
		static LONG RingProtect			(LONG,LONG,LONG);
		//static LONG Regenerate			(LONG,LONG,LONG);
		static LONG ActResistance		(LONG,LONG,LONG);
		static LONG Resistance			(LONG,LONG,LONG);
		static LONG RingResistance		(LONG,LONG,LONG);
		static LONG Infravision			(LONG,LONG,LONG);
		static LONG RingInfravision		(LONG,LONG,LONG);
		static LONG ActInfravision		(LONG,LONG,LONG);
		static LONG ImproveArmor		(LONG,LONG,LONG);
		static LONG SoulFlute			(LONG,LONG,LONG);
		static LONG MapHighlightTreasure(LONG,LONG,LONG);
		static LONG MapHighlightEvil	(LONG,LONG,LONG);
		static LONG MapHighlightEnemies	(LONG,LONG,LONG);
		static LONG MapHighlightTraps	(LONG,LONG,LONG);
		static LONG MapHighlightSecretDoors(LONG,LONG,LONG);
		static LONG MapHighlightMagic	(LONG,LONG,LONG);
		static LONG MapShowAll			(LONG,LONG,LONG);
		static LONG MapTeleport			(LONG,LONG,LONG);

//		static LONG TomeMysticMagics	(LONG,LONG,LONG);
//		static LONG TomeSorcererLore	(LONG,LONG,LONG);
//		static LONG TomeHighWizards		(LONG,LONG,LONG);
//		static LONG TomeMageLords		(LONG,LONG,LONG);

		static LONG TomeAddRandomSpells	(LONG,LONG,LONG);


		static LONG HurlFireBall		(LONG,LONG,LONG);
		static LONG FrostCone			(LONG,LONG,LONG);
		static LONG IlluminateAreaRad	(LONG,LONG,LONG);
		static LONG ElectricalBolt		(LONG,LONG,LONG);
		static LONG ParalyzeAllInRad	(LONG,LONG,LONG);
		static LONG CandleOfInvocation	(LONG,LONG,LONG);
		static LONG Parchment			(LONG,LONG,LONG);
		static LONG DierdriensRing		(LONG,LONG,LONG);
		static LONG FaelesRing			(LONG,LONG,LONG);
		static LONG GavelonStaff		(LONG,LONG,LONG);
		static LONG AmuletInspiration	(LONG,LONG,LONG);
		static LONG BannerOfRoele		(LONG,LONG,LONG);
		static LONG BarazadsTools		(LONG,LONG,LONG);
		static LONG BrazierOfSummoning	(LONG,LONG,LONG);
		static LONG BrennasFavor		(LONG,LONG,LONG);
		static LONG ChaliceOfDead		(LONG,LONG,LONG);
		static LONG CrownOfCommand		(LONG,LONG,LONG);
		static LONG StateCrownAnnuire	(LONG,LONG,LONG);
		static LONG EmperorsCrown		(LONG,LONG,LONG);
		static LONG FaridsCoffer		(LONG,LONG,LONG);
		static LONG HammerOfThunder		(LONG,LONG,LONG);
		static LONG RingOfWizardry		(LONG,LONG,LONG);
		static LONG RobesOfMasses		(LONG,LONG,LONG);
		static LONG RegaliaEmpire		(LONG,LONG,LONG);
		static LONG SielGemLarge		(LONG,LONG,LONG);
		static LONG SielGemSmall		(LONG,LONG,LONG);
		static LONG SwordOfRoele		(LONG,LONG,LONG);
		static LONG SceptreCuiraeecn	(LONG,LONG,LONG);
		static LONG Corgandal			(LONG,LONG,LONG);
		static LONG NappolansTome		(LONG,LONG,LONG);
		static LONG VaubenelsBook		(LONG,LONG,LONG);
		static LONG TorcSplendor		(LONG,LONG,LONG);
		static LONG SeeInDark			(LONG,LONG,LONG);
		static LONG Danica				(LONG,LONG,LONG);
		static LONG TomePrince			(LONG,LONG,LONG);

		// wizard spells
		
		static LONG Jump				(LONG,LONG,LONG);
		//static LONG Shrink				(LONG,LONG,LONG);
		static LONG ShockingGrasp		(LONG,LONG,LONG);
		static LONG Blindness			(LONG,LONG,LONG);
		static LONG Knock				(LONG,LONG,LONG);
		//static LONG Levitate			(LONG,LONG,LONG);
		static LONG LocateObject		(LONG,LONG,LONG);
		static LONG Strength			(LONG,LONG,LONG);
		static LONG StoneUndead			(LONG,LONG,LONG);
		static LONG Confusion			(LONG,LONG,LONG);
		static LONG StoneSkin			(LONG,LONG,LONG);
		static LONG Disintigrate		(LONG,LONG,LONG);

//these are for Globe of Invulnerability
#define GLOBE_MINOR 1
#define GLOBE_MAJOR 2
		static LONG GlobeInvulnerability(LONG,LONG,LONG);

		static LONG FleshToStone		(LONG,LONG,LONG);
		static LONG ImprovedFireball	(LONG,LONG,LONG);
		static LONG PowerStunSpell		(LONG,LONG,LONG);
		static LONG SpellImmunity		(LONG,LONG,LONG);
		static LONG MagicMissileSpell	(LONG,LONG,LONG);
		static LONG IceStormSpell		(LONG,LONG,LONG);
		static LONG ConeOfColdSpell		(LONG,LONG,LONG);
		static LONG ChainLightningSpell	(LONG,LONG,LONG);
		static LONG DeathSpell			(LONG,LONG,LONG);
		static LONG PowerBlindSpell		(LONG,LONG,LONG);
		static LONG MeteorSwarmSpell	(LONG,LONG,LONG);
		static LONG PowerKillSpell		(LONG,LONG,LONG);
		static LONG TimeStopSpell		(LONG,LONG,LONG);
		//wizard realm
		static LONG RealmGeneric		(LONG,LONG,LONG);
		
		// priest spells

		static LONG Light				(LONG,LONG,LONG);
		static LONG Shillelagh			(LONG,LONG,LONG);
		static LONG TurnUndead			(LONG,LONG,LONG);
		static LONG BarkSkin			(LONG,LONG,LONG);
		//static LONG ResistFire			(LONG,LONG,LONG);
		static LONG SpritualHammer		(LONG,LONG,LONG);
		static LONG MagicalVestement	(LONG,LONG,LONG);
		//static LONG WaterWalk			(LONG,LONG,LONG);
		static LONG ProtectionFromEvil	(LONG,LONG,LONG);
		static LONG FlameStrike			(LONG,LONG,LONG);
		//static LONG MapShowAll			(LONG,LONG,LONG);
		static LONG Resurrection		(LONG,LONG,LONG);
		static LONG FireStormSpell		(LONG,LONG,LONG);
		static LONG HolyWordSpell		(LONG,LONG,LONG);
		
		// blood abilities
		static LONG BABattleWise		(LONG,LONG,LONG);
		static LONG BACourage			(LONG,LONG,LONG);
		static LONG BADetectIllusion		(LONG,LONG,LONG);
		static LONG BADevineWrath		(LONG,LONG,LONG);
		static LONG BAHeightendAbility	(LONG,LONG,LONG);
		static LONG BABloodMark			(LONG,LONG,LONG);
		//static LONG BADetectIllusion	(LONG,LONG,LONG);
		static LONG BADivineAura		(LONG,LONG,LONG);
		static LONG BAEnhancedSense		(LONG,LONG,LONG);
		static LONG BAFear				(LONG,LONG,LONG);

#define BAGREAT 1
#define BAMAJOR 2
#define BAMINOR 3

		static LONG BAHealing			(LONG,LONG,LONG);
		static LONG BAIronWill			(LONG,LONG,LONG);
		static LONG BAProtectionEvil	(LONG,LONG,LONG);
		static LONG BARegeneration		(LONG,LONG,LONG);
		static LONG BAResistance		(LONG,LONG,LONG);
		static LONG BATravel			(LONG,LONG,LONG);
		
};

	


extern DECL_VECTOR_CLASS(ItemType,ItemTypes);
//extern ItemType ItemTypes[];


inline ItemType::ItemType()
{
	Category=IC_ALL;
	ExtendedDataIdx=-1;
	GameType=NO_THING;
	EffectFunc=NULL;
	EffectArg=0;
	MiscInfo=STR_NULL;
	IconFile[0]=0;
}


inline ItemType::ItemType(ItemCategory cat,UBYTE attr,SHORT edi,THINGTYPE Type,PFLLLL func,LONG arg,SHORT dur,char* icon,LONG info)
	:Category(cat),
	Attribs(attr),
	ExtendedDataIdx(edi),
	GameType(Type),
	EffectFunc(func),
	EffectArg(arg),
	MiscInfo(info),
	Duration(dur)
{
	if(icon)
	{
		strncpy(IconFile,icon,12);
		IconFile[12]=0;
		//hIcon=GetResourceStd(icon,FALSE);
		//if (hIcon==-1)
			//printf("warning! can't find Icon for %s\n",GAME_TTYPE::mfGetDescription(Type));
	}
}

inline LONG ItemType::mfEffect(LONG a,LONG action)
{
	if (EffectFunc)
		return (*EffectFunc)(a,EffectArg,action);
	return 0;
}

inline BOOL ItemType::mfRequires(REQUISIT req)
{
	return Attribs & req;
}

// Get the highlighted Icon.
inline SHORT const ItemType::mfGetIconOn() const
{
	SHORT ReturnMe;
	char buf[_MAX_PATH];

	sprintf(buf,"UI\\INVNICON\\%s_B.PCX",IconFile);
	ReturnMe=GetResourceStd(buf,FALSE);
	if (ReturnMe==fERROR)
	{
#ifdef _DEBUG
		fatal_error("no Icon found in GetIconOn! %s\n",buf);
#endif
		ReturnMe=GetResourceStd("UI\\INVNICON\\I_BLANK.PCX",FALSE);
	}

	return ReturnMe;
}

// Get the unhighlighted Icon.
inline SHORT const ItemType::mfGetIconOff() const
{
	SHORT ReturnMe;
	char buf[_MAX_PATH];

	sprintf(buf,"UI\\INVNICON\\%s_A.PCX",IconFile);
	ReturnMe=GetResourceStd(buf,FALSE);
	if (ReturnMe==fERROR)
	{
#ifdef _DEBUG
		fatal_error("no Icon found in GetIconOff! %s \n",buf);
#endif
		ReturnMe=GetResourceStd("UI\\INVNICON\\I_BLANK.PCX",FALSE);
	}

	return ReturnMe;
}

inline THINGTYPE ItemType::mfGameType() const
{
	return GameType;
}

inline void ItemType::mfPrint() const
{
	printf("%s\n",GAME_TTYPE::mfGetDescription(GameType));
}
inline SHORT ItemType::mfInfo() const
{
	return MiscInfo;
}
 

inline const char* ItemType::mfIconFile() const
{
	return IconFile;
}  
inline ItemCategory ItemType::mfCategory() const
{
	return Category;
}
inline SHORT ItemType::mfGetExtendedData() const
{
	return ExtendedDataIdx;
}
inline const char* ItemType::mfGetName() const
{
	return GAME_TTYPE::mfGetDescription(GameType);
}
inline LONG const ItemType::mfGetSTRMgrId() const
{
	return GAME_TTYPE::mfGetSTRMgrId(GameType);
}

extern BOOL fQuietItems;		

inline void ItemType::mfMessage(GAME_STRING message) const
{

	//this does the buffer stuff for you.
	if (fQuietItems)
		return;

	if (message <0 || message >STR_MAX_STRINGS)
		return;		

	char buffer[100]; //100 should be enough...

	strcpy(buffer,mfGetName());
	ItemEffects::mfMessageBox(buffer,STRMGR_GetStr(message));
}




inline void ItemEffects::mfMessageBox(char const * t,char const * m)
{
	if (!fQuietItems)
		TitledMessageBox::Display(t,m);
	else
		printf("message stifled: %s: %s\n",t,m);
}


inline void ItemEffects::mfSound(BIRTHRT_SND s, SHORT r, LONG t)
{
	if (!fQuietItems)
		AddSndObj(s,r,t);
}




inline ItemEffects::ActiveItem::ActiveItem() :
	Func(0),
	Arg1(0),
	Arg2(0),
	TimeToStop(0),
	Valid(FALSE),
	AutoRemove(FALSE)
	
{
}

inline ItemEffects::ActiveItem::ActiveItem(PFLLLL a,LONG b,LONG c,LONG d) :
	Func(a),
	Arg1(b),
	Arg2(c),
	TimeToStop(d),
	Valid(TRUE)
{
}

inline LONG ItemEffects::ActiveItem::mfTicksLeft()
{
	return TimeToStop-SCENE_MGR::gTick;
}
inline LONG ItemEffects::ActiveItem::mfSecondsLeft()
{
	return (TimeToStop-SCENE_MGR::gTick)/(18.2);
}

inline LONG ItemEffects::None(LONG,LONG,LONG action)
{
	if (action == ACTIVATE)
		return DONT_DROP;
		
	if (action== EQUIP)
		return CANT_COMPLETE;
		
	return OK;
}

inline ITEMTYPE ItemChargeHandler::ChargedItemChain::operator[](LONG a)
{
	if (a>=0 && a<MAX_CHARGES) 
		return ItemTypeChain[a]; 
	return ITEM_NO_THING;
}

inline LONG ItemChargeHandler::ChargedItemChain::mfCharges()
{
	return numCharges;
}

#endif
