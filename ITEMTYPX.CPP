/* ========================================================================
   Copyright (c) 1990,1996   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: ItemType.cpp -Has a Table of All gettable objects and effects
   Author: Wes Cumberland
   ======================================================================== */

#include "blood.hxx"
#include "itemtype.hxx"
#include "spell.hxx"
#include "rlmspell.hxx"

#if defined (_VC4)
#define static
#endif

//static ItemEffects::ActiveItem _ActiveItems[MAX_ACTIVE_ITEMS] = {
DEFINE_VECTOR_MEMBER_DATA_S(ItemEffects::ActiveItem,
                     ItemEffects,
                     ActiveItems,
                     MAX_ACTIVE_ITEMS) = {
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem(),
   ItemEffects::ActiveItem()
};
DEFINE_VECTOR_MEMBER_CLASS(ItemEffects::ActiveItem,
                     ItemEffects,
                     ActiveItems);





BOOL fQuietItems=FALSE;  //whether or not Items display a message box/play a sound
DEFINE_VECTOR_DATA(ItemType,ItemTypes) =
{
//Mundane Items
//                   Extended Data
//Ctor   Category   Atr vvv GameType  Effect               Arg Dur IconFileName     MiscInfo
//------|----------|---|---|---------|--------------------|---|---|----------------|--------|
ItemType(IC_MUNDANE,0,  -1,   NO_THING, NULL/*dont ref me!*/, 0, 0,  "BOWL01",   STR_NULL),
ItemType(IC_MUNDANE,0,  -1,   BOWL_1,    ItemEffects::None,    0, 0,  "BOWL01",     STR_ITEM_DESC_BOWL),
ItemType(IC_MUNDANE,0,  -1,   BUCKET_1, ItemEffects::None,  0, 0,  "BUKT01",  STR_ITEM_DESC_BUCKET),
ItemType(IC_MUNDANE,0,  -1,   CANDLE_1, ItemEffects::None,  0, 0,  "CNDL01",  STR_ITEM_DESC_CANDLE),
ItemType(IC_MUNDANE,0,  -1,   CUP_1,     ItemEffects::None,    0, 0,  "CUP_01",     STR_ITEM_DESC_CUP),
ItemType(IC_MUNDANE,0,  -1,   FLAGON_1, ItemEffects::None,  0, 0,  "FLGN01",  STR_ITEM_DESC_FLAGON),
ItemType(IC_MUNDANE,0,  -1,   HELMET_1, ItemEffects::None,  0, 0,  "HELM01",     STR_ITEM_DESC_HELMET),
ItemType(IC_MUNDANE,0,  -1,   KNIFE_1,  ItemEffects::None,  0, 0,  "KNIF01",     STR_ITEM_DESC_KNIFE),
ItemType(IC_MUNDANE,0,  -1,   PLATE_1,  ItemEffects::None,  0, 0,  "PLAT01",     STR_ITEM_DESC_PLATE),
ItemType(IC_MUNDANE,0,  -1,   SPEAR_1,  ItemEffects::None,  0, 0,  "SPER01",     STR_ITEM_DESC_SPEAR),

ItemType(IC_MUNDANE,0,  -1,   SWORD_1,  ItemEffects::None,  0, 0,  "SWRD01",     STR_ITEM_DESC_SWORD),

//Magical Items
//Ctor   Category   Atr       GameType                Effect                     Arg               Dur Icon     MiscInfo
//------|----------|---|---|-------------------------------|-------------------------------|-------------------|---|--------|--------|
ItemType(IC_MUNDANE,0,  -1,   KEY_BLACK,                 ItemEffects::KeyExplanation,  0,             0, "KEYP01", STR_ITEM_DESC_KEY_GENERICMAGIC),
ItemType(IC_MUNDANE,0,  -1,   KEY_BLUE,                  ItemEffects::KeyExplanation,  0,             0, "KEYB01", STR_ITEM_DESC_KEY_GENERICMAGIC),
ItemType(IC_MUNDANE,0,  -1,   KEY_BONE,                  ItemEffects::KeyExplanation,  0,             0, "KEYT01", STR_ITEM_DESC_KEY_GENERICMAGIC),
ItemType(IC_MUNDANE,0,  -1,   KEY_JEWELED,               ItemEffects::KeyExplanation,  0,             0, "KEYG01", STR_ITEM_DESC_KEY_GENERICMAGIC),
ItemType(IC_MUNDANE,0,  -1,   KEY_RED,                ItemEffects::KeyExplanation,  0,             0, "KEYR01", STR_ITEM_DESC_KEY_GENERICMAGIC),
ItemType(IC_MUNDANE,0,  -1,   KEY_SILVER,                ItemEffects::KeyExplanation,  0,             0, "KEYV01", STR_ITEM_DESC_KEY_GENERICMAGIC),
ItemType(IC_MUNDANE,0,  -1,   KEY_WHITE,                 ItemEffects::KeyExplanation,  0,             0, "KEYW01", STR_ITEM_DESC_KEY_GENERICMAGIC),
ItemType(IC_MUNDANE,0,  -1,   KEY_SKELETON,              ItemEffects::KeyExplanation,  0,             0, "KEYX01", STR_ITEM_DESC_KEY_GENERICMAGIC),
ItemType(IC_MUNDANE,0,  -1,   KEY_STONE,                 ItemEffects::KeyExplanation,  0,             0, "KEYA01", STR_ITEM_DESC_KEY_GENERICMAGIC),
ItemType(IC_MUNDANE,0,  -1,   KEY_TINY,                  ItemEffects::KeyExplanation,  0,             0, "KEYL01", STR_ITEM_DESC_KEY_GENERICMAGIC),
ItemType(IC_MAGITEM,0,  -1,   AMULET_OF_FEATHER_FALL,       ItemEffects::FeatherFall,     0,             60,   "AMUL01", STR_ITEM_DESC_AMULET_OF_FEATHER_FALL),
ItemType(IC_MAGITEM,0,  -1,   AMULET_OF_SPEED,           ItemEffects::Speed,           0,             60,   "AMUL02", STR_ITEM_DESC_AMULET_OF_SPEED),
ItemType(IC_MAGITEM,0,  -1,   AMULET_OF_STRIDING_AND_SPRINGING,ItemEffects::StrideAndSpring, 0,             60,   "AMUL03", STR_ITEM_DESC_AMULET_OF_STRIDING_AND_SPRINGING),
ItemType(IC_MAGITEM,TARGET,   -1,   AMULET_VERSUS_UNDEAD,      ItemEffects::UndeadTurn,      USE_LEVEL|MAKE_DICE(1,6,0),180,"AMUL04", STR_ITEM_DESC_AMULET_VERSUS_UNDEAD),
ItemType(IC_MAGITEM,0,  -1,   AMULET_OF_WATER_WALKING,      ItemEffects::WaterWalk,       0,             60,   "AMUL05", STR_ITEM_DESC_AMULET_OF_WATER_WALKING),
ItemType(IC_MUNDANE,0,  -1,   BALM_OF_HEALING,           ItemEffects::Heal,HEAL_RANDOM|MAKE_DICE(1,3,0),    0, "BALMHL", STR_ITEM_DESC_BALM_OF_HEALING),
ItemType(IC_MAGITEM,0,  -1,   BOOK_OF_EXALTED_DEEDS,        ItemEffects::BookOfExaltedDeeds,0,              0, "BOOK01", STR_ITEM_DESC_BOOK_OF_EXALTED_DEEDS),
ItemType(IC_MAGITEM,0,  -1,   BOOK_OF_VILE_DARKNESS,        ItemEffects::BookOfVileDarkness,0,              0, "BOOK02", STR_ITEM_DESC_BOOK_OF_VILE_DARKNESS),
ItemType(IC_MAGITEM,0,  -1,   CHIME_OF_OPENING,          ItemEffects::KeyExplanation,  0,             0, "CHIMEO", STR_ITEM_DESC_CHIME_OF_OPENING),
ItemType(IC_MAGITEM,0,  -1,   CLOAK_OF_PROTECTION_1,        ItemEffects::CloakProtect,    1|PROT_CLOAK,     180,"CLOK01", STR_ITEM_DESC_CLOAK_OF_PROTECTION_1),
ItemType(IC_MAGITEM,0,  -1,   CLOAK_OF_PROTECTION_2,        ItemEffects::CloakProtect,    2|PROT_CLOAK,     180,"CLOK02", STR_ITEM_DESC_CLOAK_OF_PROTECTION_2),
ItemType(IC_MAGITEM,0,  -1,   CLOAK_OF_PROTECTION_3,        ItemEffects::CloakProtect,    3|PROT_CLOAK,     180,"CLOK03", STR_ITEM_DESC_CLOAK_OF_PROTECTION_3),
ItemType(IC_MAGITEM,0,  -1,   GAUNTLETS_OF_DEXTERITY,       ItemEffects::GauntletsOfDex,  0,             180,"GAUN01", STR_ITEM_DESC_GAUNTLETS_OF_DEXTERITY),
ItemType(IC_MAGITEM,0,  -1,   GAUNTLETS_OF_OGRE_POWER,      ItemEffects::GuantletsOfOgrePow,CLASS_RESTRICTED,  180,"GAUN02", STR_ITEM_DESC_GAUNTLETS_OF_OGRE_POWER),
ItemType(IC_MAGITEM,0,  -1,   GEM_BLUE,                  ItemEffects::Generic,         0,             0, "GEMB01", STR_ITEM_DESC_GEM_BLUE),
ItemType(IC_MAGITEM,0,  -1,   GEM_GREEN,                 ItemEffects::Generic,         0,             0, "GEMG01", STR_ITEM_DESC_GEM_GREEN),
ItemType(IC_MAGITEM,0,  -1,   GEM_OF_PASSAGE,               ItemEffects::KeyExplanation,  0,             0, "GEMX01", STR_ITEM_DESC_GEM_OF_PASSAGE),
ItemType(IC_MAGITEM,0,  -1,   GEM_OF_SHIFTING,           ItemEffects::MapTeleport,     0,             0, "GEMP01", STR_ITEM_DESC_GEM_OF_SHIFTING),
ItemType(IC_MAGITEM,0,  -1,   IOUN_STONE_PALE_BLUE,         ItemEffects::AddToAttribute,  AT_STR|1,         180,"IOUN01", STR_ITEM_DESC_IOUN_STONE_PALE_BLUE),
ItemType(IC_MAGITEM,0,  -1,   IOUN_STONE_SCARLET_AND_BLUE,  ItemEffects::AddToAttribute,  AT_INT|1,         180,"IOUN02", STR_ITEM_DESC_IOUN_STONE_SCARLET_AND_BLUE),
ItemType(IC_MAGITEM,0,  -1,   IOUN_STONE_INCANDESCENT_BLUE, ItemEffects::AddToAttribute,  AT_WIS|1,         180,"IOUN03", STR_ITEM_DESC_IOUN_STONE_INCANDESCENT_BLUE),
ItemType(IC_MAGITEM,0,  -1,   IOUN_STONE_DEEP_RED,       ItemEffects::AddToAttribute,  AT_DEX|1,         180,"IOUN04", STR_ITEM_DESC_IOUN_STONE_DEEP_RED),
ItemType(IC_MAGITEM,0,  -1,   IOUN_STONE_PINK,           ItemEffects::AddToAttribute,  AT_CON|1,         180,"IOUN05", STR_ITEM_DESC_IOUN_STONE_PINK),
ItemType(IC_MAGITEM,0,  -1,   IOUN_STONE_PINK_AND_GREEN,    ItemEffects::AddToAttribute,  AT_CHA|1,         180,"IOUN06", STR_ITEM_DESC_IOUN_STONE_PINK_AND_GREEN),
ItemType(IC_MAGITEM,0,  -1,   IOUN_STONE_PEARLY_WHITE,      ItemEffects::Regenerate,      10,               180,"IOUN08", STR_ITEM_DESC_IOUN_STONE_PEARLY_WHITE),
ItemType(IC_MAGITEM,0,  -1,   IOUN_STONE_DUSTY_ROSE,        ItemEffects::CloakProtect,    1,             180,"IOUN12", STR_ITEM_DESC_IOUN_STONE_DUSTY_ROSE),
ItemType(IC_MAGITEM,0,  -1,   LIBRIUM_OF_GAINFUL_CONJURATION,  ItemEffects::LibrOfConjuration,  CLASS_RESTRICTED, 0, "LIBR01", STR_ITEM_DESC_LIBRIUM_OF_GAINFUL_CONJURATION),
ItemType(IC_MAGITEM,0,  -1,   LIBRIUM_OF_INEFFABLE_DAMNATION,  ItemEffects::LibrOfDamnation, CLASS_RESTRICTED, 0, "LIBR02", STR_ITEM_DESC_LIBRIUM_OF_INEFFABLE_DAMNATION),
ItemType(IC_MAGITEM,0,  -1,   LIBRIUM_OF_SILVER_MAGIC,      ItemEffects::LibrSilver,      CLASS_RESTRICTED, 0, "LIBR03", STR_ITEM_DESC_LIBRIUM_OF_SILVER_MAGIC),
ItemType(IC_MAGITEM,TARGET,   -1,   NECKLACE_OF_MISSILES_8,    ItemEffects::HurlFireBall,    AT_FIRE|MAKE_DICE(2,6,0),0,"NECKMI",STR_ITEM_DESC_NECKLACE_OF_MISSILES),
ItemType(IC_MAGITEM,0,  -1,   OIL_OF_SHARPNESS_1,           ItemEffects::ImproveWeapon,      1,             180,"OILS01", STR_ITEM_DESC_OIL_OF_SHARPNESS_1),
ItemType(IC_MAGITEM,0,  -1,   OIL_OF_SHARPNESS_2,           ItemEffects::ImproveWeapon,      2,             180,"OILS02", STR_ITEM_DESC_OIL_OF_SHARPNESS_2),
ItemType(IC_MAGITEM,0,  -1,   OIL_OF_SHARPNESS_3,           ItemEffects::ImproveWeapon,      3,             180,"OILS03", STR_ITEM_DESC_OIL_OF_SHARPNESS_3),
ItemType(IC_MAGITEM,0,  -1,   OIL_OF_SHARPNESS_4,           ItemEffects::ImproveWeapon,      4,             180,"OILS04", STR_ITEM_DESC_OIL_OF_SHARPNESS_4),
ItemType(IC_MAGITEM,0,  -1,   OIL_OF_SHARPNESS_5,           ItemEffects::ImproveWeapon,      5,             180,"OILS05", STR_ITEM_DESC_OIL_OF_SHARPNESS_5),
ItemType(IC_MAGITEM,0,  -1,   POTION_OF_HILL_GIANT_STRENGTH,   ItemEffects::SetAttrTo,       CLASS_RESTRICTED|AT_STR|24,   180,"POTN06", STR_ITEM_DESC_POTION_OF_HILL_GIANT_STRENGTH),
ItemType(IC_MAGITEM,0,  -1,   POTION_OF_STONE_GIANT_STRENGTH,  ItemEffects::SetAttrTo,       CLASS_RESTRICTED|AT_STR|25,   180,"POTN07", STR_ITEM_DESC_POTION_OF_STONE_GIANT_STRENGTH),
ItemType(IC_MAGITEM,0,  -1,   POTION_OF_FROST_GIANT_STRENGTH,  ItemEffects::SetAttrTo,       CLASS_RESTRICTED|AT_STR|26,   180,"POTN08", STR_ITEM_DESC_POTION_OF_FROST_GIANT_STRENGTH),
ItemType(IC_MAGITEM,0,  -1,   POTION_OF_FIRE_GIANT_STRENGTH,   ItemEffects::SetAttrTo,       CLASS_RESTRICTED|AT_STR|27,   180,"POTN09", STR_ITEM_DESC_POTION_OF_FIRE_GIANT_STRENGTH),
ItemType(IC_MAGITEM,0,  -1,   POTION_OF_CLOUD_GIANT_STRENGTH,  ItemEffects::SetAttrTo,       CLASS_RESTRICTED|AT_STR|28,   180,"POTN10", STR_ITEM_DESC_POTION_OF_CLOUD_GIANT_STRENGTH),
ItemType(IC_MAGITEM,0,  -1,   POTION_OF_STORM_GIANT_STRENGTH,  ItemEffects::SetAttrTo,       CLASS_RESTRICTED|AT_STR|29,   180,"POTN11", STR_ITEM_DESC_POTION_OF_STORM_GIANT_STRENGTH),
ItemType(IC_MAGITEM,0,  -1,   POTION_OF_TITAN_STRENGTH,     ItemEffects::SetAttrTo,       CLASS_RESTRICTED|AT_STR|30,   180,"POTN12", STR_ITEM_DESC_POTION_OF_TITAN_STRENGTH),
ItemType(IC_MAGITEM,0,  -1,   POTION_OF_DIMINUTION,         ItemEffects::Shrink,       25,               60,   "POTN14", STR_ITEM_DESC_POTION_OF_DIMINUTION),
ItemType(IC_MAGITEM,0,  -1,   POTION_OF_INVULNERABILITY,    ItemEffects::Invulnerable,    CLASS_RESTRICTED, 180,"POTN16", STR_ITEM_DESC_POTION_OF_INVULNERABILITY),
ItemType(IC_MAGITEM,0,  -1,   POTION_OF_LEVITATION,         ItemEffects::Levitate,        PARTY,            60,   "POTN17", STR_ITEM_DESC_POTION_OF_LEVITATION),
ItemType(IC_MAGITEM,0,  -1,   POTION_OF_FIRE_RESISTANCE,    ItemEffects::ActResistance,      _RESIST_FIRE,     180,"POTN01", STR_ITEM_DESC_POTION_OF_FIRE_RESISTANCE),
ItemType(IC_MAGITEM,0,  -1,   POTION_OF_FLYING,          ItemEffects::Flight,       0,             90,   "POTN05", STR_ITEM_DESC_POTION_OF_FLYING),
ItemType(IC_MAGITEM,0,  -1,   POTION_OF_HEALING,            ItemEffects::Heal,HEAL_RANDOM|MAKE_DICE(2,4,2),    0, "POTN04", STR_ITEM_DESC_POTION_OF_HEALING),
ItemType(IC_MAGITEM,0,  -1,   POTION_OF_EXTRA_HEALING,      ItemEffects::Heal,HEAL_RANDOM|MAKE_DICE(3,8,3),    0, "POTN02", STR_ITEM_DESC_POTION_OF_EXTRA_HEALING),
ItemType(IC_MAGITEM,0,  -1,   RING_OF_FEATHER_FALLING,      ItemEffects::RingFeatherFall,    0,             60,   "RING01", STR_ITEM_DESC_RING_OF_FEATHER_FALLING),
ItemType(IC_MAGITEM,0,  -1,   RING_OF_FIRE_FALLING,         ItemEffects::RingResistance,  _RESIST_FIRE,     180,"RING02", STR_ITEM_DESC_RING_OF_FIRE_FALLING),
ItemType(IC_MAGITEM,0,  -1,   RING_OF_JUMPING,           ItemEffects::RingJump,           2,             60,   "RING04", STR_ITEM_DESC_RING_OF_JUMPING),
ItemType(IC_MAGITEM,0,  -1,   RING_OF_PROTECTION_1,         ItemEffects::CloakProtect,    1|PROT_RING,      180,"RING05", STR_ITEM_DESC_RING_OF_PROTECTION_1),
ItemType(IC_MAGITEM,0,  -1,   RING_OF_PROTECTION_2,         ItemEffects::CloakProtect,    2|PROT_RING,      180,"RING06", STR_ITEM_DESC_RING_OF_PROTECTION_2),
ItemType(IC_MAGITEM,0,  -1,   RING_OF_PROTECTION_3,         ItemEffects::CloakProtect,    3|PROT_RING,      180,"RING07", STR_ITEM_DESC_RING_OF_PROTECTION_3),
ItemType(IC_MAGITEM,0,  -1,   RING_OF_PROTECTION_4,         ItemEffects::CloakProtect,    4|PROT_RING,      180,"RING08", STR_ITEM_DESC_RING_OF_PROTECTION_4),
ItemType(IC_MAGITEM,0,  -1,   RING_OF_REGENERATION,         ItemEffects::Regenerate,      10,               180,"RING09", STR_ITEM_DESC_RING_OF_REGENERATION),
ItemType(IC_MAGITEM,0,  -1,   RING_OF_RESISTANCE_1,         ItemEffects::Resistance,      _RESIST_MAGIC|25, 180,"RING10", STR_ITEM_DESC_RING_OF_RESISTANCE_1),
ItemType(IC_MAGITEM,0,  -1,   RING_OF_RESISTANCE_2,         ItemEffects::Resistance,      _RESIST_MAGIC|50, 180,"RING11", STR_ITEM_DESC_RING_OF_RESISTANCE_2),
ItemType(IC_MAGITEM,0,  -1,   RING_OF_RESISTANCE_3,         ItemEffects::Resistance,      _RESIST_MAGIC|75, 180,"RING12", STR_ITEM_DESC_RING_OF_RESISTANCE_3),
ItemType(IC_MAGITEM,0,  -1,   RING_OF_NIGHTVISION,       ItemEffects::RingInfravision, 0,             180,"RING14", STR_ITEM_DESC_RING_OF_NIGHTVISION),
ItemType(IC_MAGITEM,0,  -1,   ROD_OF_RESURRECTION,       ItemEffects::Resurrection,    0,             0, "RODRES", STR_ITEM_DESC_ROD_OF_RESURRECTION),
ItemType(IC_MAGITEM,0,  -1,   SCROLL_OF_ARMOR_ENHANCEMENT_1,   ItemEffects::ImproveArmor,    1|(4<<16),        180,"SCRL01", STR_ITEM_DESC_SCROLL_OF_ARMOR_ENHANCEMENT_1),
ItemType(IC_MAGITEM,0,  -1,   SCROLL_OF_ARMOR_ENHANCEMENT_2,   ItemEffects::ImproveArmor,    2|(3<<16),        180,"SCRL02", STR_ITEM_DESC_SCROLL_OF_ARMOR_ENHANCEMENT_2),
ItemType(IC_MAGITEM,0,  -1,   SCROLL_OF_ARMOR_ENHANCEMENT_3,   ItemEffects::ImproveArmor,    3|(2<<16),        180,"SCRL03", STR_ITEM_DESC_SCROLL_OF_ARMOR_ENHANCEMENT_3),
ItemType(IC_MAGITEM,0,  -1,   SCROLL_OF_ARMOR_ENHANCEMENT_4,   ItemEffects::ImproveArmor,    4|(1<<16),        180,"SCRL04", STR_ITEM_DESC_SCROLL_OF_ARMOR_ENHANCEMENT_4),
ItemType(IC_MAGITEM,0,  -1,   SCROLL_OF_ARMOR_ENHANCEMENT_5,   ItemEffects::ImproveArmor,    5|(0<<16),        180,"SCRL05", STR_ITEM_DESC_SCROLL_OF_ARMOR_ENHANCEMENT_5),
ItemType(IC_MAGITEM,0,  -1,   STAFF_OF_CURING,           ItemEffects::TouchSpell,      CLASS_RESTRICTED|HEAL_RANDOM|MAKE_DICE(3,6,3),  0, "STAF01", STR_ITEM_DESC_STAFF_OF_CURING),
ItemType(IC_MUNDANE,0,  -1,   VIAL_OF_HOLY_WATER,           ItemEffects::UndeadTurn,      MAKE_DICE(1,6,0), 0, "VIALHW", STR_ITEM_DESC_VIAL_OF_HOLY_WATER),
ItemType(IC_MAGITEM,0,  -1,   WAND_OF_ENEMY_DETECTION_3,    ItemEffects::MapHighlightEnemies,600,           60,   "WAND01", STR_ITEM_DESC_WAND_OF_ENEMY_DETECTION),
ItemType(IC_MAGITEM,TARGET,   -1, WAND_OF_FIRE_3,           ItemEffects::HurlFireBall,    CLASS_RESTRICTED|AT_FIRE|MAKE_DICE(6,6,0),   0, "WAND03", STR_ITEM_DESC_WAND_OF_FIRE),
ItemType(IC_MAGITEM,TARGET,   -1, WAND_OF_FROST_3,       ItemEffects::FrostCone,       CLASS_RESTRICTED|MAKE_DICE(6,6,0),  0, "WAND05", STR_ITEM_DESC_WAND_OF_FROST),
ItemType(IC_MAGITEM,0,  -1,   WAND_OF_ILLUMINATION_3,       ItemEffects::SeeInDark,       0,             180,"WAND06", STR_ITEM_DESC_WAND_OF_ILLUMINATION),
ItemType(IC_MAGITEM,TARGET,   -1, WAND_OF_LIGHTNING_3,      ItemEffects::ElectricalBolt,  CLASS_RESTRICTED|MAKE_DICE(6,6,0),  0, "WAND07", STR_ITEM_DESC_WAND_OF_LIGHTNING),
ItemType(IC_MAGITEM,0,  -1,   WAND_OF_MAGIC_DETECTION_3,    ItemEffects::MapHighlightMagic,  600,              60,   "WAND08", STR_ITEM_DESC_WAND_OF_MAGIC_DETECTION),
ItemType(IC_MAGITEM,TARGET,   -1, WAND_OF_PARALYZATION_3,      ItemEffects::ParalyzeAllInRad,   CLASS_RESTRICTED|60,             0, "WAND09", STR_ITEM_DESC_WAND_OF_PARALYZATION),
ItemType(IC_MAGITEM,0,  -1,   WAND_OF_SECRET_DETECTION_3,      ItemEffects::MapHighlightSecretDoors,600,       60,   "WAND10", STR_ITEM_DESC_WAND_OF_SECRET_DETECTION),
ItemType(IC_MAGITEM,0,  -1,   CANDLE_OF_INVOCATION,         ItemEffects::CandleOfInvocation,CLASS_RESTRICTED|0,               180,"CNDL02", STR_ITEM_DESC_CANDLE_OF_INVOCATION),
ItemType(IC_MUNDANE,0,  -1,   HINT_1,                    ItemEffects::Parchment,       1,             0, "PRCH01", STR_ITEM_DESC_PARCHMENT_1),
ItemType(IC_MUNDANE,0,  -1,   HINT_2,                    ItemEffects::Parchment,       2,             0, "PRCH02", STR_ITEM_DESC_PARCHMENT_1),
ItemType(IC_MUNDANE,0,  -1,   HINT_3,                    ItemEffects::Parchment,       3,             0, "PRCH03", STR_ITEM_DESC_PARCHMENT_1),
ItemType(IC_MUNDANE,0,  -1,   HINT_4,                    ItemEffects::Parchment,       4,             0, "PRCH02", STR_ITEM_DESC_PARCHMENT_1),
ItemType(IC_MUNDANE,0,  -1,   HINT_5,                    ItemEffects::Parchment,       5,             0, "PRCH01", STR_ITEM_DESC_PARCHMENT_1),



//Relics of Power
//Ctor   Category   Atr       GameType                Effect                     Arg   Dur Icon     MiscInfo
//------|----------|---|---|-------------------------------|-------------------------------|---|---|--------|--------|
ItemType(IC_MAGITEM,0,  -1,   DIERDRIENS_RING,           ItemEffects::DierdriensRing,  0, -1,   "RING15",STR_RELIC_DESC_DIERDRIENS_RING),
ItemType(IC_MAGITEM,0,  -1,   FAELES_RING,               ItemEffects::FaelesRing,      0, -1,   "RING16",STR_RELIC_DESC_FAELES_RING),
ItemType(IC_MAGITEM,0,  -1,   GAVELONS_STAFF_OF_PROSPERITY, ItemEffects::GavelonStaff,    0, -1,   "STAF03",STR_RELIC_DESC_GAVELONS_STAFF_OF_PROSPERITY),
ItemType(IC_MAGITEM,0,  -1,   AMULET_OF_INSPIRATION,        ItemEffects::AmuletInspiration,  0, -1, "AMUL06",STR_RELIC_DESC_AMULET_OF_INSPIRATION),
ItemType(IC_MAGITEM,0,  -1,   BANNER_OF_ROELE,           ItemEffects::BannerOfRoele,      0, -1,   "BANNER",STR_RELIC_DESC_BANNER_OF_ROELE),
ItemType(IC_MAGITEM,0,  -1,   BARAZADS_TOOLS,               ItemEffects::BarazadsTools,      0, -1,   "TOOLSB",STR_RELIC_DESC_BARAZADS_TOOLS),
ItemType(IC_MAGITEM,0,  -1,   BRENNAS_FAVOR,             ItemEffects::BrennasFavor,    0, -1, "BRENNA",STR_RELIC_DESC_BRENNAS_FAVOR),
ItemType(IC_MAGITEM,0,  -1,   CHALICE_OF_THE_DEAD,       ItemEffects::ChaliceOfDead,      0, -1, "CHALIC",STR_RELIC_DESC_CHALICE_OF_THE_DEAD),
ItemType(IC_MAGITEM,0,  -1,   CROWN_OF_COMMAND,          ItemEffects::CrownOfCommand,  0, -1, "CROWNC",STR_RELIC_DESC_CROWN_OF_COMMAND),
ItemType(IC_MAGITEM,0,  -1,   STATE_CROWN_OF_ANUIRE,        ItemEffects::StateCrownAnnuire,  0, -1, "CROWNA",STR_RELIC_DESC_STATE_CROWN_OF_ANUIRE),
ItemType(IC_MAGITEM,0,  -1,   EMPERORS_CROWN,               ItemEffects::EmperorsCrown,      0, -1, "CROWNE",STR_RELIC_DESC_EMPERORS_CROWN),
ItemType(IC_MAGITEM,0,  -1,   FARIDS_COFFER_OF_THE_REALM,      ItemEffects::FaridsCoffer,    0, -1, "COFFER",STR_RELIC_DESC_FARIDS_COFFER_OF_THE_REALM),
ItemType(IC_MAGITEM,TARGET,   -1, HAMMER_OF_THUNDER,           ItemEffects::HammerOfThunder, 0, -1,   "HAMMER",STR_RELIC_DESC_HAMMER_OF_THUNDER),
ItemType(IC_MAGITEM,0,  -1,   RING_OF_WIZARDRY,          ItemEffects::RingOfWizardry,  0, -1,   "RING13",STR_RELIC_DESC_RING_OF_WIZARDRY),
ItemType(IC_MAGITEM,0,  -1,   ROBES_OF_THE_MASES,           ItemEffects::RobesOfMasses,      0, -1,   "ROBESM",STR_RELIC_DESC_ROBES_OF_THE_MASES),
ItemType(IC_MAGITEM,0,  -1,   REGALIA_OF_EMPIRE,            ItemEffects::RegaliaEmpire,      0, -1,   "REGALI",STR_RELIC_DESC_REGALIA_OF_EMPIRE),
ItemType(IC_MAGITEM,0,  -1,   SIELSHEGH_GEM_LARGE,       ItemEffects::SielGemLarge,    0, -1,   "SIELSL",STR_RELIC_DESC_SIELSHEGH_GEM_LARGE),
ItemType(IC_MAGITEM,0,  -1,   SIELSHEGH_GEM_MEDIUM,         ItemEffects::SielGemSmall,    0, -1,   "SIELSM",STR_RELIC_DESC_SIELSHEGH_GEM_MEDIUM),
ItemType(IC_MAGITEM,0,  -1,   SWORD_OF_ROELE,               ItemEffects::SwordOfRoele,    0, -1,   "SWORDR",STR_RELIC_DESC_SWORD_OF_ROELE),
ItemType(IC_MAGITEM,0,  -1,   SCEPTRE_OF_CUIRAECEN,         ItemEffects::SceptreCuiraeecn,   0, -1,   "SCEPTR",STR_RELIC_DESC_SCEPTRE_OF_CUIRAECEN),
ItemType(IC_MAGITEM,0,  -1,   CORGANDALS_STAFF,          ItemEffects::Corgandal,       0, -1,   "STAF04",STR_RELIC_DESC_CORGANDALS_STAFF),
ItemType(IC_MAGITEM,0,  -1,   NAPPOLANS_TOME_OF_WAR,        ItemEffects::NappolansTome,      0, -1,   "TOME02",STR_RELIC_DESC_NAPPOLANS_TOME_OF_WAR),
ItemType(IC_MAGITEM,0,  -1,   VAUBENELS_BOOK_OF_FORTIFICATION,ItemEffects::VaubenelsBook,    0, -1,   "BOOK03",STR_RELIC_DESC_VAUBENELS_BOOK_OF_FORTIFICATION),
ItemType(IC_MAGITEM,0,  -1,   TORC_OF_SPLENDOR,          ItemEffects::TorcSplendor,    0, -1,   "TORCSP",STR_RELIC_DESC_TORC_OF_SPLENDOR),
ItemType(IC_MAGITEM,0,  -1,   DANICAS_CRYSTAL_OF_SCRYING,      ItemEffects::Danica,       0, -1,   "DANICA",STR_RELIC_DESC_DANICAS_CRYSTAL_OF_SCRYING),

ItemType(IC_MAGITEM,0,  -1,   TOME_OF_MYSTIC_MAGICS,        ItemEffects::TomeAddRandomSpells,   MAKE_ADD_SPELL(5,1,3)         ,-1,"TOME03",STR_ITEM_DESC_TOME_OF_MYSTIC_MAGICS),
ItemType(IC_MAGITEM,0,  -1,   TOME_OF_SORCERORS_LORE,       ItemEffects::TomeAddRandomSpells,   MAKE_ADD_SPELL(3,4,6)         ,-1,"TOME04",STR_ITEM_DESC_TOME_OF_SORCERER_LORE),
ItemType(IC_MAGITEM,0,  -1,   TOME_OF_THE_HIGH_WIZARDS,     ItemEffects::TomeAddRandomSpells,   MAKE_ADD_SPELL(1,7,9)         ,-1,"TOME05",STR_ITEM_DESC_TOME_OF_HIGH_WIZARDS),
ItemType(IC_MAGITEM,0,  -1,   TOME_OF_THE_MAGE_LORDS,       ItemEffects::TomeAddRandomSpells,   MAKE_ADD_SPELL(5,0,0)|ADD_REALM  ,-1,"TOME06",STR_ITEM_DESC_TOME_OF_MAGE_LORDS),



//Spells
//Ctor   Category   Atr ExtendedData                        GameType                Effect                        Arg                                 Dur(in seconds) Icon  MiscInfo
//------|----------|---|---------------------------------------|-----------------------|-----------------------------------|---------------------------------------|---------------|-------|-----------------------------|
ItemType(IC_SPELL,   0,       SPELL_INFO::DETECT_MAGIC,     DETECT_MAGIC_1,         ItemEffects::MapHighlightMagic,     600,                          20|PER_LEVEL,  "SCRL06",STR_SPELL_DESC_DETECT_MAGIC),
ItemType(IC_SPELL,   0,       SPELL_INFO::FEATHER_FALL,     FEATHER_FALL_1,         ItemEffects::FeatherFall,        0,                            10|PER_LEVEL,  "SCRL06",STR_ITEM_DESC_AMULET_OF_FEATHER_FALL),
ItemType(IC_SPELL,   0,       SPELL_INFO::JUMP,          JUMP_1,              ItemEffects::Jump,               0,                               10|PER_LEVEL,  "SCRL06",STR_SPELL_DESC_JUMP),
ItemType(IC_SPELL,   0,       SPELL_INFO::LIGHT,            LIGHT_1,          ItemEffects::SeeInDark,          0,                               100|PER_LEVEL, "SCRL06",STR_SPELL_DESC_LIGHT),
ItemType(IC_SPELL,   TARGETWAVE, SPELL_INFO::MAGIC_MISSILE,    MAGIC_MISSILE_1,     ItemEffects::MagicMissileSpell,     AT_MISSILE|USE_LEVEL|MAKE_DICE(1,4,0), -1,            "SCRL06",STR_SPELL_DESC_MAGIC_MISSILE),
ItemType(IC_SPELL,   0,       SPELL_INFO::REDUCE,           REDUCE_1,            ItemEffects::Shrink,          50,                              50|PER_LEVEL,  "SCRL06",STR_SPELL_DESC_REDUCE),
ItemType(IC_SPELL,   TARGETWAVE, SPELL_INFO::SHOCKING_GRASP,      SHOCKING_GRASP_1,    ItemEffects::ShockingGrasp,         0,                               -1,            "SCRL06",STR_SPELL_DESC_SHOCKING_GRASP),
ItemType(IC_SPELL,   0,       SPELL_INFO::BLINDNESS,        BLINDNESS_1,         ItemEffects::Blindness,          0,                               100,        "SCRL06",STR_SPELL_DESC_BLINDNESS),
ItemType(IC_SPELL,   0,       SPELL_INFO::CONTINUAL_LIGHT,  CONTINUAL_LIGHT_1,      ItemEffects::SeeInDark,          0,                               24000,         "SCRL06",STR_SPELL_DESC_CONTINUAL_LIGHT),
ItemType(IC_SPELL,   0,       SPELL_INFO::KNOCK,            KNOCK_1,          ItemEffects::Knock,              0,                               -1,            "SCRL06",STR_SPELL_DESC_KNOCK),
ItemType(IC_SPELL,   0,       SPELL_INFO::LEVITATE,         LEVITATE_1,          ItemEffects::Levitate,           0,                               20|PER_LEVEL,  "SCRL06",STR_SPELL_DESC_LEVITATE),
ItemType(IC_SPELL,   0,       SPELL_INFO::LOCATE_OBJECT,    LOCATE_OBJECT_1,     ItemEffects::LocateObject,       0,                               20|PER_LEVEL,  "SCRL06",STR_SPELL_DESC_LOCATE_OBJECT),
ItemType(IC_SPELL,   TARGETWAVE, SPELL_INFO::POISON_ARROW,     POISON_ARROW_1,         ItemEffects::HurlFireBall,       AT_MISSILE|USE_LEVEL|MAKE_DICE(1,4,0), -1,            "SCRL06",STR_SPELL_DESC_POISON_ARROW),
ItemType(IC_SPELL,   0,       SPELL_INFO::STRENGTH,         STRENGTH_1,          ItemEffects::TouchSpell,         0,                               100|PER_LEVEL, "SCRL06",STR_SPELL_DESC_STRENGTH),
ItemType(IC_SPELL,   TARGETWAVE, SPELL_INFO::FIREBALL,         FIREBALL_1,          ItemEffects::HurlFireBall,       AT_FIRE|USE_LEVEL|MAKE_DICE(1,6,0),    -1,            "SCRL06",STR_SPELL_DESC_FIREBALL),
ItemType(IC_SPELL,   0,       SPELL_INFO::FLY,           FLY_1,               ItemEffects::Flight,          USE_LEVEL,                       100|PER_LEVEL, "SCRL06",STR_SPELL_DESC_FLY),
ItemType(IC_SPELL,   TARGETWAVE, SPELL_INFO::STONE_UNDEAD,     STONE_UNDEAD_1,         ItemEffects::StoneUndead,        0,                               30|PER_LEVEL,  "SCRL06",STR_SPELL_DESC_STONE_UNDEAD),
ItemType(IC_SPELL,   0,       SPELL_INFO::INFRAVISION,      INFRAVISION_1,       ItemEffects::ActInfravision,     0,                               1000|PER_LEVEL,   "SCRL06",STR_SPELL_DESC_INFRAVISION),
ItemType(IC_SPELL,   TARGETWAVE, SPELL_INFO::LIGHTNING_BOLT,      LIGHTNING_BOLT_1,    ItemEffects::HurlFireBall,       AT_LIGHTNING|USE_LEVEL|MAKE_DICE(1,6,0),-1,           "SCRL06",STR_SPELL_DESC_LIGHTNING_BOLT),
ItemType(IC_SPELL,   0,       SPELL_INFO::CONFUSION,        CONFUSION_1,         ItemEffects::Confusion,          0,                               20|PER_LEVEL,  "SCRL06",STR_SPELL_DESC_CONFUSION),
ItemType(IC_SPELL,   TARGETWAVE, SPELL_INFO::ICE_STORM,        ICE_STORM_1,         ItemEffects::HurlFireBall,       AT_PLASMA|MAKE_DICE(1,6,0),            -1,            "SCRL06",STR_SPELL_DESC_ICE_STORM),
ItemType(IC_SPELL,   0,       SPELL_INFO::MINOR_GLOBE_INVULN,  MINOR_GLOBE_INVULN_1,   ItemEffects::GlobeInvulnerability,  GLOBE_MINOR,                     30|PER_LEVEL,  "SCRL06",STR_SPELL_DESC_MINOR_GLOBE_INVULN),
ItemType(IC_SPELL,   0,       SPELL_INFO::STONESKIN,        STONESKIN_1,         ItemEffects::StoneSkin,          0,                               40|PER_LEVEL,  "SCRL06",STR_SPELL_DESC_STONESKIN),
ItemType(IC_SPELL,   TARGETWAVE, SPELL_INFO::CONE_OF_COLD,     CONE_OF_COLD_1,         ItemEffects::ConeOfColdSpell,    AT_PLASMA|USE_LEVEL|MAKE_DICE(1,4,2),  -1,            "SCRL06",STR_SPELL_DESC_CONE_OF_COLD),
ItemType(IC_SPELL,   TARGETWAVE, SPELL_INFO::DISINTIGRATE,     DISINTIGRATE_1,         ItemEffects::Disintigrate,       0,                            -1,            "SCRL06",STR_SPELL_DESC_DISINTIGRATE),
ItemType(IC_SPELL,   0,       SPELL_INFO::TELEPORT,         TELEPORT_1,          ItemEffects::MapTeleport,        0,                            -1,            "SCRL06",STR_SPELL_DESC_MAP_TELEPORT),
ItemType(IC_SPELL,   TARGETWAVE, SPELL_INFO::CHAIN_LIGHTNING,  CHAIN_LIGHTNING_1,      ItemEffects::ChainLightningSpell,   AT_LIGHTNING|USE_LEVEL|MAKE_DICE(1,6,0),-1,           "SCRL06",STR_SPELL_DESC_CHAIN_LIGHTNING),
ItemType(IC_SPELL,   TARGETWAVE, SPELL_INFO::DEATH,            DEATH_1,          ItemEffects::DeathSpell,         0,                            -1,            "SCRL06",STR_SPELL_DESC_DEATH),
ItemType(IC_SPELL,   0,       SPELL_INFO::GLOBE_INVULN,     GLOBE_INVULN_1,         ItemEffects::GlobeInvulnerability,  GLOBE_MAJOR,                     20|PER_LEVEL,  "SCRL06",STR_SPELL_DESC_GLOBE_INVULN),
ItemType(IC_SPELL,   TARGETWAVE, SPELL_INFO::FLESH_TO_STONE,      FLESH_TO_STONE_1,    ItemEffects::FleshToStone,       0,                               -1,            "SCRL06",STR_SPELL_DESC_FLESH_TO_STONE),
ItemType(IC_SPELL,   TARGETWAVE, SPELL_INFO::IMPROVED_FIREBAL, IMPROVED_FIREBAL_1,     ItemEffects::ImprovedFireball,      0,                               -1,            "SCRL06",STR_SPELL_DESC_IMPROVED_FIREBAL),
ItemType(IC_SPELL,   0,       SPELL_INFO::POWER_STUN,       POWER_STUN_1,        ItemEffects::Confusion,          0,                               100,        "SCRL06",STR_SPELL_DESC_POWER_STUN),
ItemType(IC_SPELL,   0,       SPELL_INFO::POWER_BLIND,      POWER_BLIND_1,       ItemEffects::PowerBlindSpell,    MAKE_DICE(1,6,0),                100,        "SCRL06",STR_SPELL_DESC_POWER_BLIND),
ItemType(IC_SPELL,   0,       SPELL_INFO::SPELL_IMMUNITY,      SPELL_IMMUNITY_1,    ItemEffects::SpellImmunity,         0,                               20|PER_LEVEL,  "SCRL06",STR_SPELL_DESC_SPELL_IMMUNITY),
ItemType(IC_SPELL,   TARGETWAVE, SPELL_INFO::METEOR_SWARM,     METEOR_SWARM_1,         ItemEffects::MeteorSwarmSpell,      MAKE_DICE(1,6,0),                -1,            "SCRL06",STR_SPELL_DESC_METEOR_SWARM),
ItemType(IC_SPELL,   TARGETWAVE, SPELL_INFO::POWER_KILL,       POWER_KILL_1,        ItemEffects::PowerKillSpell,     MAKE_DICE(1,6,0),                -1,            "SCRL06",STR_SPELL_DESC_POWER_KILL),
ItemType(IC_SPELL,   0,       SPELL_INFO::TIME_STOP,        TIME_STOP_1,         ItemEffects::TimeStopSpell,         MAKE_DICE(1,6,0),                -1,            "SCRL06",STR_SPELL_DESC_TIME_STOP),

ItemType(IC_SPELL,   0,       SPELL_INFO::CURE_LIGHT,       CURE_LIGHT_1,        ItemEffects::TouchSpell,         HEAL_RANDOM|MAKE_DICE(1,8,0),          -1,            "SCRL07",STR_SPELL_DESC_CURE_LIGHT),
ItemType(IC_SPELL,   0,       SPELL_INFO::DETECT_EVIL,      DETECT_EVIL_1,       ItemEffects::MapHighlightEvil,      600,                          20|PER_LEVEL,  "SCRL07",STR_SPELL_DESC_DETECT_EVIL),
ItemType(IC_SPELL,   0,       SPELL_INFO::DETECT_MAGIC_PR,  DETECT_MAGIC_PR_1,      ItemEffects::MapHighlightMagic,     600,                          20|PER_LEVEL,  "SCRL07",STR_SPELL_DESC_DETECT_MAGIC_PR),
ItemType(IC_SPELL,   0,       SPELL_INFO::LIGHT_PR,         LIGHT_PR_1,          ItemEffects::SeeInDark,          0,                               100|PER_LEVEL, "SCRL07",STR_SPELL_DESC_LIGHT_PR),
ItemType(IC_SPELL,   0,       SPELL_INFO::SHILLELAGH,       SHILLELAGH_1,        ItemEffects::Shillelagh,         0,                               20|PER_LEVEL,  "SCRL07",STR_SPELL_DESC_SHILLELAGH),
ItemType(IC_SPELL,   TARGETWAVE, SPELL_INFO::TURN_UNDEAD,      TURN_UNDEAD_1,       ItemEffects::UndeadTurn,         MAKE_DICE(6,6,0),                   -1,            "SCRL07",STR_SPELL_DESC_TURN_UNDEAD),
ItemType(IC_SPELL,   0,       SPELL_INFO::BARKSKIN,         BARKSKIN_1,          ItemEffects::BarkSkin,           0,                               40|PER_LEVEL,  "SCRL07",STR_SPELL_DESC_BARKSKIN),
ItemType(IC_SPELL,   0,       SPELL_INFO::FIND_TRAPS,       FIND_TRAPS_1,        ItemEffects::MapHighlightTraps,     600,                          20|PER_LEVEL,  "SCRL07",STR_SPELL_DESC_FIND_TRAPS),
ItemType(IC_SPELL,   0,       SPELL_INFO::FIND_TREASURE,    FIND_TREASURE_1,     ItemEffects::MapHighlightTreasure,  600,                          20|PER_LEVEL,  "SCRL07",STR_SPELL_DESC_FIND_TREASURE),
ItemType(IC_SPELL,   0,       SPELL_INFO::RESIST_FIRE,      RESIST_FIRE_1,       ItemEffects::ActResistance,         _RESIST_FIRE,                    20|PER_LEVEL,  "SCRL07",STR_SPELL_DESC_RESIST_FIRE),
ItemType(IC_SPELL,   TARGETWAVE, SPELL_INFO::SPIRITUAL_HAMMER, SPIRITUAL_HAMMER_1,     ItemEffects::SpritualHammer,     0,                               -1,            "SCRL07",STR_SPELL_DESC_SPIRITUAL_HAMMER),
ItemType(IC_SPELL,   0,       SPELL_INFO::MAGICAL_VESTAMENT,   MAGICAL_VESTAMENT_1, ItemEffects::MagicalVestement,      0,                               40|PER_LEVEL,  "SCRL07",STR_SPELL_DESC_MAGICAL_VESTAMENT),
ItemType(IC_SPELL,   0,       SPELL_INFO::WATER_WALK,       WATER_WALK_1,        ItemEffects::SeeInDark,          0,                               24000,         "SCRL07",STR_SPELL_DESC_CONTINUAL_LIGHT),
ItemType(IC_SPELL,   0,       SPELL_INFO::CURE_SERIOUS,     CURE_SERIOUS_1,         ItemEffects::TouchSpell,         HEAL_RANDOM|MAKE_DICE(2,8,1),          -1,            "SCRL07",STR_SPELL_DESC_CURE_SERIOUS),
ItemType(IC_SPELL,   0,       SPELL_INFO::PROT_FROM_EVIL,      PROT_FROM_EVIL_1,    ItemEffects::ProtectionFromEvil, 0,                               20|PER_LEVEL,  "SCRL07",STR_SPELL_DESC_PROT_FROM_EVIL),
ItemType(IC_SPELL,   0,       SPELL_INFO::CURE_CRITICAL,    CURE_CRITICAL_1,     ItemEffects::TouchSpell,         HEAL_RANDOM|MAKE_DICE(3,8,3),          -1,            "SCRL07",STR_SPELL_DESC_CURE_CRITICAL),
ItemType(IC_SPELL,   TARGETWAVE, SPELL_INFO::FLAME_STRIKE,     FLAME_STRIKE_1,         ItemEffects::FlameStrike,        0,                               -1,            "SCRL07",STR_SPELL_DESC_FLAME_STRIKE),
ItemType(IC_SPELL,   0,       SPELL_INFO::TRUE_SEEING,      TRUE_SEEING_1,       ItemEffects::MapShowAll,         0,                            20|PER_LEVEL,  "SCRL07",STR_SPELL_DESC_SHOW_ALL_MAP),
ItemType(IC_SPELL,   0,       SPELL_INFO::HEAL,          HEAL_1,              ItemEffects::TouchSpell,         HEAL_FULLY,                      -1,            "SCRL07",STR_SPELL_DESC_HEAL),
ItemType(IC_SPELL,   TARGETWAVE, SPELL_INFO::FIRE_STORM,       FIRE_STORM_1,        ItemEffects::HurlFireBall,       AT_FIRE|USE_LEVEL|MAKE_DICE(1,8,0),    -1,            "SCRL07",STR_SPELL_DESC_FIRE_STORM),
ItemType(IC_SPELL,   0,       SPELL_INFO::HOLY_WORD,        HOLY_WORD_1,         ItemEffects::HolyWordSpell,         MAKE_DICE(1,6,0),                -1,            "SCRL07",STR_SPELL_DESC_HOLY_WORD),
ItemType(IC_SPELL,   0,       SPELL_INFO::RESURRECTION,     RESURRECTION_1,         ItemEffects::Resurrection,       0,                               -1,            "SCRL07",STR_SPELL_DESC_RESURRECTION),


// Wizard Realm Spells
ItemType(IC_REALM_SPELL,  0,  REALM_SPELL_INFO::ALCHEMY,          REALM_ALCHEMY_1,        ItemEffects::RealmGeneric, 0,        -1 ,"TOME06",STR_REALM_SPELL_DESC_ALCHEMY),
ItemType(IC_REALM_SPELL,  0,  REALM_SPELL_INFO::DEATH_PLAGUE,        REALM_DEATH_PLAGUE_1,      ItemEffects::RealmGeneric, 0,        -1 ,"TOME06",STR_REALM_SPELL_DESC_DEATH_PLAGUE),
ItemType(IC_REALM_SPELL,  0,  REALM_SPELL_INFO::DEMAGOGUE,        REALM_DEMAGOGUE_1,         ItemEffects::RealmGeneric, 0,        -1 ,"TOME06",STR_REALM_SPELL_DESC_DEMAGOGUE),
ItemType(IC_REALM_SPELL,  0,  REALM_SPELL_INFO::DISPEL_REALM_MAGIC,  REALM_DISPEL_REALM_MAGIC_1,   ItemEffects::RealmGeneric, 0,        -1 ,"TOME06",STR_REALM_SPELL_DESC_DISPEL_REALM_MAGIC),
ItemType(IC_REALM_SPELL,  0,  REALM_SPELL_INFO::LEGION_OF_DEAD,      REALM_LEGION_OF_DEAD_1,    ItemEffects::RealmGeneric, 0,        -1 ,"TOME06",STR_REALM_SPELL_DESC_LEGION_OF_DEAD),
ItemType(IC_REALM_SPELL,  0,  REALM_SPELL_INFO::MASS_DESTRUCTION,    REALM_MASS_DESTRUCTION_1,  ItemEffects::RealmGeneric, 0,        -1 ,"TOME06",STR_REALM_SPELL_DESC_MASS_DESTRUCTION),
ItemType(IC_REALM_SPELL,  0,  REALM_SPELL_INFO::RAZE,             REALM_RAZE_1,           ItemEffects::RealmGeneric, 0,        -1 ,"TOME06",STR_REALM_SPELL_DESC_RAZE),
ItemType(IC_REALM_SPELL,  0,  REALM_SPELL_INFO::SCRY,             REALM_SCRY_1,           ItemEffects::RealmGeneric, 0,        -1 ,"TOME06",STR_REALM_SPELL_DESC_SCRY),
ItemType(IC_REALM_SPELL,  0,  REALM_SPELL_INFO::STRONGHOLD,       REALM_STRONGHOLD_1,        ItemEffects::RealmGeneric, 0,        -1 ,"TOME06",STR_REALM_SPELL_DESC_STRONGHOLD),
ItemType(IC_REALM_SPELL,  0,  REALM_SPELL_INFO::SUMMONING,        REALM_SUMMONING_1,         ItemEffects::RealmGeneric, 0,        -1 ,"TOME06",STR_REALM_SPELL_DESC_SUMMONING),
ItemType(IC_REALM_SPELL,  0,  REALM_SPELL_INFO::TRANSPORT,        REALM_TRANSPORT_1,         ItemEffects::RealmGeneric, 0,        -1 ,"TOME06",STR_REALM_SPELL_DESC_TRANSPORT),
ItemType(IC_REALM_SPELL,  0,  REALM_SPELL_INFO::WARDING,          REALM_WARDING_1,        ItemEffects::RealmGeneric, 0,        -1 ,"TOME06",STR_REALM_SPELL_DESC_WARDING),

// Priest Realm Spells
ItemType(IC_REALM_SPELL,  0,  REALM_SPELL_INFO::BLESS_LAND,       REALM_BLESS_LAND_1,        ItemEffects::RealmGeneric, 0,        -1 ,"SCRL08",STR_REALM_SPELL_DESC_BLESS_LAND),
ItemType(IC_REALM_SPELL,  0,  REALM_SPELL_INFO::BLESS_ARMY,       REALM_BLESS_ARMY_1,        ItemEffects::RealmGeneric, 0,        -1 ,"SCRL08",STR_REALM_SPELL_DESC_BLESS_ARMY),
ItemType(IC_REALM_SPELL,  0,  REALM_SPELL_INFO::BLIGHT,           REALM_BLIGHT_1,            ItemEffects::RealmGeneric, 0,        -1 ,"SCRL08",STR_REALM_SPELL_DESC_BLIGHT),
ItemType(IC_REALM_SPELL,  0,  REALM_SPELL_INFO::DISPEL_REALM_MAGIC_PRIESTVER, REALM_DISPEL_REALM_MAGIC_1,   ItemEffects::RealmGeneric, 0, -1 ,"SCRL08",STR_REALM_SPELL_DESC_DISPEL_REALM_MAGIC),
ItemType(IC_REALM_SPELL,  0,  REALM_SPELL_INFO::HONEST_DEALING,      REALM_HONEST_DEALING_1,    ItemEffects::RealmGeneric, 0,        -1 ,"SCRL08",STR_REALM_SPELL_DESC_HONEST_DEALING),
ItemType(IC_REALM_SPELL,  0,  REALM_SPELL_INFO::INVESTITURE,         REALM_INVESTITURE_1,    ItemEffects::RealmGeneric, 0,        -1 ,"SCRL08",STR_REALM_SPELL_DESC_INVESTITURE),


//Blood Abilities
//Ctor   Category   Atr ExtendedData                              GameType                   Effect                           Arg  Dur IconFileName   MiscInfo
//------|----------|---|-------------------------------------------|-------------------------------|--------------------------------|----|---|-------------|--------|
ItemType(IC_BLOODAB,0,  BLOOD_ABILITY_INFO::BATTLEWISE,           BLOOD_BATTLEWISE_1,           ItemEffects::BABattleWise,       0,   -1, "BLOD02",      STR_BLOODAB_DESC_BATTLEWISE),
ItemType(IC_BLOODAB,0,  BLOOD_ABILITY_INFO::COURAGE,           BLOOD_COURAGE_1,           ItemEffects::BACourage,          0,   -1, "BLOD03",      STR_BLOODAB_DESC_COURAGE),
ItemType(IC_BLOODAB,0,  BLOOD_ABILITY_INFO::DEVINE_WRATH,         BLOOD_DIVINE_WRATH_1,         ItemEffects::BADevineWrath,      0,   -1, "BLOD02",      STR_BLOODAB_DESC_DEVINEWRATH),
ItemType(IC_BLOODAB,0,  BLOOD_ABILITY_INFO::HEIGHTENED_ABILITY,      BLOOD_HEIGHTENED_ABILITY_1,      ItemEffects::BAHeightendAbility, 0,   -1, "BLOD03",      STR_BLOODAB_DESC_HEIGHTENDAB),
ItemType(IC_BLOODAB,0,  BLOOD_ABILITY_INFO::BLOODMARK,            BLOOD_BLOODMARK_1,            ItemEffects::BABloodMark,        0,   -1, "BLOD03",      STR_BLOODAB_DESC_BLOODMARK),
ItemType(IC_BLOODAB,0,  BLOOD_ABILITY_INFO::DETECT_ILLUSION,      BLOOD_DETECT_ILLUSION_1,      ItemEffects::BADetectIllusion  ,0,   -1, "BLOD03",    STR_BLOODAB_DESC_DETECT_ILLUSION),
ItemType(IC_BLOODAB,0,  BLOOD_ABILITY_INFO::DIVINE_AURA,       BLOOD_DIVINE_AURA_1,       ItemEffects::BADivineAura,       0,   -1, "BLOD02",      STR_BLOODAB_DESC_DIVINE_AURA),
ItemType(IC_BLOODAB,0,  BLOOD_ABILITY_INFO::ENHANCED_SENSE,       BLOOD_ENHANCED_SENSE_1,       ItemEffects::BAEnhancedSense,    0,   -1, "BLOD03",      STR_BLOODAB_DESC_ENHANCED_SENSE),
ItemType(IC_BLOODAB,0,  BLOOD_ABILITY_INFO::FEAR,              BLOOD_FEAR_1,              ItemEffects::BAFear,             0,   -1, "BLOD02",      STR_BLOODAB_DESC_FEAR),
ItemType(IC_BLOODAB,0,  BLOOD_ABILITY_INFO::HEALING_MINOR,        BLOOD_HEALING_1,           ItemEffects::BAHealing,          BAMINOR,-1,"BLOD02", STR_BLOODAB_DESC_HEALING),
ItemType(IC_BLOODAB,0,  BLOOD_ABILITY_INFO::HEALING_MAJOR,        BLOOD_HEALING_1,           ItemEffects::BAHealing,          BAMAJOR,-1,"BLOD02", STR_BLOODAB_DESC_HEALING),
ItemType(IC_BLOODAB,0,  BLOOD_ABILITY_INFO::HEALING_GREAT,        BLOOD_HEALING_1,           ItemEffects::BAHealing,          BAGREAT,-1,"BLOD02", STR_BLOODAB_DESC_HEALING),
ItemType(IC_BLOODAB,0,  BLOOD_ABILITY_INFO::IRON_WILL,            BLOOD_IRON_WILL_1,            ItemEffects::BAIronWill,         0,   -1, "BLOD03",      STR_BLOODAB_DESC_IRON_WILL),
ItemType(IC_BLOODAB,0,  BLOOD_ABILITY_INFO::PROTECTION_FROM_EVIL, BLOOD_PROTECTION_FROM_EVIL_1, ItemEffects::BAProtectionEvil,   0,   -1, "BLOD02",      STR_BLOODAB_DESC_PROTECTION_FROM_EVIL),
ItemType(IC_BLOODAB,0,  BLOOD_ABILITY_INFO::REGENERATION,         BLOOD_REGENERATION_1,         ItemEffects::BARegeneration,     0,  -1, "BLOD01",    STR_BLOODAB_DESC_REGENERATION),
ItemType(IC_BLOODAB,0,  BLOOD_ABILITY_INFO::RESISTANCE,           BLOOD_RESISTANCE_1,           ItemEffects::BAResistance,       0,   -1, "BLOD02",      STR_BLOODAB_DESC_RESISTANCE),
ItemType(IC_BLOODAB,0,  BLOOD_ABILITY_INFO::TRAVEL,               BLOOD_TRAVEL_1,               ItemEffects::BATravel,           0,   -1, "BLOD01",      STR_BLOODAB_DESC_TRAVEL),

// Stuff that no longer fits above.

ItemType(IC_MUNDANE,0,  -1,   GOLD_COINS_1,  ItemEffects::GoldCoins,  50, 0,  "I_BLANK",     STR_ITEM_DESC_GOLD_1),
ItemType(IC_MUNDANE,0,  -1,   GOLD_COINS_2,  ItemEffects::GoldCoins, 150, 0,  "I_BLANK",     STR_ITEM_DESC_GOLD_1),
ItemType(IC_MUNDANE,0,  -1,   GOLD_COINS_3,  ItemEffects::GoldCoins,  50, 0,  "I_BLANK",     STR_ITEM_DESC_GOLD_1),
ItemType(IC_MUNDANE,0,  -1,   GOLD_COINS_4,  ItemEffects::GoldCoins, 150, 0,  "I_BLANK",     STR_ITEM_DESC_GOLD_1),
ItemType(IC_MUNDANE,0,  -1,   GOLD_COINS_5,  ItemEffects::GoldCoins, 500, 0,  "I_BLANK",     STR_ITEM_DESC_GOLD_1),
ItemType(IC_MUNDANE,0,  -1,   TREASURE_CHEST,ItemEffects::GoldCoins,2000, 0,  "I_BLANK",     STR_ITEM_DESC_TREASURE_CHEST),

// special war bonus
ItemType(IC_MAGITEM,0,  -1,   GORGONS_BANNER,      ItemEffects::None,   0, 0, "gorghd",      STR_ITEM_GORGONS_BANNER),

// rescued captives
ItemType(IC_MUNDANE,0,  -1,   ENDAERAL_CATHBIRN,   ItemEffects::Rescuee,  0, 0,  "LORDM1",      STR_ITEM_DESC_CAPTIVE_1),
ItemType(IC_MUNDANE,0,  -1,   TIESKAR_GRAECHER, ItemEffects::Rescuee,  0, 0,  "GLORD1",      STR_ITEM_DESC_CAPTIVE_2),

// extra relics
ItemType(IC_MAGITEM,0,  -1,   IOUN_HEART,       ItemEffects::AddToAttribute,  AT_ALL|1,         180,"IOUN11", STR_ITEM_DESC_IOUN_HEART),
ItemType(IC_MAGITEM,0,  -1,   IOUN_FIST,        ItemEffects::AddToAttribute,  AT_LEV|2,         180,"IOUN07", STR_ITEM_DESC_IOUN_FIST),
ItemType(IC_MAGITEM,0,  -1,   TOME_OF_THE_PRINCE,  ItemEffects::TomePrince,      0,             0,  "TOME01", STR_RELIC_DESC_TOME_OF_THE_PRINCE),


//fix for charges bug
ItemType(IC_MAGITEM,TARGET,   -1,   NECKLACE_OF_MISSILES_7,    ItemEffects::HurlFireBall,          AT_FIRE|MAKE_DICE(2,6,0),              0, "NECKMI",STR_ITEM_DESC_NECKLACE_OF_MISSILES),
ItemType(IC_MAGITEM,TARGET,   -1,   NECKLACE_OF_MISSILES_6,    ItemEffects::HurlFireBall,          AT_FIRE|MAKE_DICE(2,6,0),              0, "NECKMI",STR_ITEM_DESC_NECKLACE_OF_MISSILES),
ItemType(IC_MAGITEM,TARGET,   -1,   NECKLACE_OF_MISSILES_5,    ItemEffects::HurlFireBall,          AT_FIRE|MAKE_DICE(2,6,0),              0, "NECKMI",STR_ITEM_DESC_NECKLACE_OF_MISSILES),
ItemType(IC_MAGITEM,TARGET,   -1,   NECKLACE_OF_MISSILES_4,    ItemEffects::HurlFireBall,          AT_FIRE|MAKE_DICE(2,6,0),              0, "NECKMI",STR_ITEM_DESC_NECKLACE_OF_MISSILES),
ItemType(IC_MAGITEM,TARGET,   -1,   NECKLACE_OF_MISSILES_3,    ItemEffects::HurlFireBall,          AT_FIRE|MAKE_DICE(2,6,0),              0, "NECKMI",STR_ITEM_DESC_NECKLACE_OF_MISSILES),
ItemType(IC_MAGITEM,TARGET,   -1,   NECKLACE_OF_MISSILES_2,    ItemEffects::HurlFireBall,          AT_FIRE|MAKE_DICE(2,6,0),              0, "NECKMI",STR_ITEM_DESC_NECKLACE_OF_MISSILES),
ItemType(IC_MAGITEM,TARGET,   -1,   NECKLACE_OF_MISSILES_1,    ItemEffects::HurlFireBall,          AT_FIRE|MAKE_DICE(2,6,0),              0, "NECKMI",STR_ITEM_DESC_NECKLACE_OF_MISSILES),
ItemType(IC_MAGITEM,0,     -1,   WAND_OF_ENEMY_DETECTION_2, ItemEffects::MapHighlightEnemies,      600,                             60,   "WAND01",STR_ITEM_DESC_WAND_OF_ENEMY_DETECTION),
ItemType(IC_MAGITEM,0,     -1,   WAND_OF_ENEMY_DETECTION_1, ItemEffects::MapHighlightEnemies,      600,                             60,   "WAND01",STR_ITEM_DESC_WAND_OF_ENEMY_DETECTION),
ItemType(IC_MAGITEM,TARGET,   -1, WAND_OF_FIRE_2,           ItemEffects::HurlFireBall,          CLASS_RESTRICTED|AT_FIRE|MAKE_DICE(6,6,0),   0, "WAND03",STR_ITEM_DESC_WAND_OF_FIRE),
ItemType(IC_MAGITEM,TARGET,   -1, WAND_OF_FIRE_1,           ItemEffects::HurlFireBall,          CLASS_RESTRICTED|AT_FIRE|MAKE_DICE(6,6,0),   0, "WAND03",STR_ITEM_DESC_WAND_OF_FIRE),
ItemType(IC_MAGITEM,TARGET,   -1, WAND_OF_FROST_2,       ItemEffects::FrostCone,             CLASS_RESTRICTED|MAKE_DICE(6,6,0),        0, "WAND05",STR_ITEM_DESC_WAND_OF_FROST),
ItemType(IC_MAGITEM,TARGET,   -1, WAND_OF_FROST_1,       ItemEffects::FrostCone,             CLASS_RESTRICTED|MAKE_DICE(6,6,0),        0, "WAND05",STR_ITEM_DESC_WAND_OF_FROST),
ItemType(IC_MAGITEM,0,     -1,   WAND_OF_ILLUMINATION_2,    ItemEffects::SeeInDark,             0,                               180,"WAND06",STR_ITEM_DESC_WAND_OF_ILLUMINATION),
ItemType(IC_MAGITEM,0,     -1,   WAND_OF_ILLUMINATION_1,    ItemEffects::SeeInDark,             0,                               180,"WAND06",STR_ITEM_DESC_WAND_OF_ILLUMINATION),
ItemType(IC_MAGITEM,TARGET,   -1, WAND_OF_LIGHTNING_2,      ItemEffects::ElectricalBolt,        CLASS_RESTRICTED|MAKE_DICE(6,6,0),        0, "WAND07",STR_ITEM_DESC_WAND_OF_LIGHTNING),
ItemType(IC_MAGITEM,TARGET,   -1, WAND_OF_LIGHTNING_1,      ItemEffects::ElectricalBolt,        CLASS_RESTRICTED|MAKE_DICE(6,6,0),        0, "WAND07",STR_ITEM_DESC_WAND_OF_LIGHTNING),
ItemType(IC_MAGITEM,0,     -1,   WAND_OF_MAGIC_DETECTION_2, ItemEffects::MapHighlightMagic,        600,                             60,   "WAND08",STR_ITEM_DESC_WAND_OF_MAGIC_DETECTION),
ItemType(IC_MAGITEM,0,     -1,   WAND_OF_MAGIC_DETECTION_1, ItemEffects::MapHighlightMagic,        600,                             60,   "WAND08",STR_ITEM_DESC_WAND_OF_MAGIC_DETECTION),
ItemType(IC_MAGITEM,TARGET,   -1, WAND_OF_PARALYZATION_2,      ItemEffects::ParalyzeAllInRad,         CLASS_RESTRICTED|60,                0, "WAND09",STR_ITEM_DESC_WAND_OF_PARALYZATION),
ItemType(IC_MAGITEM,TARGET,   -1, WAND_OF_PARALYZATION_1,      ItemEffects::ParalyzeAllInRad,         CLASS_RESTRICTED|60,                0, "WAND09",STR_ITEM_DESC_WAND_OF_PARALYZATION),
ItemType(IC_MAGITEM,0,     -1,   WAND_OF_SECRET_DETECTION_2,   ItemEffects::MapHighlightSecretDoors,  600,                             60,   "WAND10",STR_ITEM_DESC_WAND_OF_SECRET_DETECTION),
ItemType(IC_MAGITEM,0,     -1,   WAND_OF_SECRET_DETECTION_1,   ItemEffects::MapHighlightSecretDoors,  600,                             60,   "WAND10",STR_ITEM_DESC_WAND_OF_SECRET_DETECTION),

// new abilities
ItemType(IC_BLOODAB,TARGETWAVE,  -1,   TURN_UNDEAD_1,       ItemEffects::TouchSpell,   USE_LEVEL,                    -1,            "SCRL07",STR_SPELL_DESC_TURN_UNDEAD),

ItemType(IC_BLOODAB,    0, -1,   CURE_CRITICAL_1,     ItemEffects::TouchSpell,   HEAL_ABSOLUT|USE_LEVEL|2,        -1,            "SCRL07",STR_SPELL_DESC_CURE_CRITICAL),

};

DEFINE_VECTOR_CLASS(ItemType, ItemTypes);




/* ------------------------------------------------------------------------
   EFFECT FUNCTIONS
   ------------------------------------------------------------------------ */


LONG ItemEffects::RealmGeneric(LONG Combo,LONG Arg,LONG Action)
{Arg=Combo;Combo=Action;Action=Arg;return 0;}
//this function is a helper for a coder trying to implement magic items.


LONG ItemEffects::Template(LONG Combo, LONG Arg, LONG Action)
{
   return OK; //everything went ok
}







LONG ItemEffects::Regenerate(LONG Combo,LONG Arg,LONG Action)
{
   return OK; //everything went ok
}



LONG ItemEffects::Rescuee(LONG,LONG,LONG action)
{
   return OK;
}


LONG ItemEffects::TouchSpell(LONG Combo, LONG arg, LONG action)
{
   return OK;
}


//arg is determined by high byte of arg,
//1=Percentage (low short is what percent)
//2=Absolute (low short is how many hp)
//3=Random (low 3 bytes describe the DICE to roll)
//4=Fully heal (low 3 bytes aren't important)


LONG ItemEffects::Heal(LONG Combo,LONG arg,LONG action)
{
   return OK;
}




/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */

LONG ItemEffects::AddToAttribute(LONG Combo,LONG Arg,LONG Action)
{
   return OK; //everything went ok
}




LONG ItemEffects::ImproveWeapon(LONG Combo,LONG Arg,LONG Action)
{
   return OK; //everything went ok
}

/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */

LONG ItemEffects::SetAttrTo(LONG Combo,LONG Arg,LONG Action)
{
   return OK; //everything went ok
}
/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */
LONG ItemEffects::Invulnerable(LONG Combo,LONG Arg,LONG Action)
{
   return OK; //everything went ok
}
/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */
LONG ItemEffects::ActResistance(LONG Combo,LONG Arg,LONG Action)
{
   return OK; //everything went ok
}
/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */
LONG ItemEffects::Resistance(LONG Combo,LONG Arg,LONG Action)
{
   return OK; //everything went ok
}
/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */
LONG ItemEffects::ImproveArmor(LONG Combo,LONG Arg,LONG Action)
{
   return OK; //everything went ok
}
/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */

LONG ItemEffects::HurlFireBall(LONG Combo,LONG Arg,LONG Action)
{
   return OK; //everything went ok
}
/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */

LONG ItemEffects::FrostCone(LONG Combo,LONG Arg,LONG Action)
{
   return OK; //everything went ok
}
LONG ItemEffects::IlluminateAreaRad(LONG Combo,LONG Arg,LONG Action)
{
   return OK;

}
/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */

LONG ItemEffects::ElectricalBolt(LONG Combo,LONG Arg,LONG Action)
{
   return OK; //everything went ok
}
/* -----------------------------------------------------------------

   ----------------------------------------------------------------- */
LONG ItemEffects::ParalyzeAllInRad(LONG Combo,LONG Arg,LONG Action)
{
   return OK; //everything went ok
}







//effect duration maintenance functions!

BOOL ItemEffects::mfAddActiveItem(PFLLLL f,LONG a1,LONG a2,LONG dur,BOOL fAR)
{
   return TRUE;
}

void ItemEffects::DeactivateAllItems()
{
}


ItemEffects::ActiveItem* ItemEffects::mfGetActiveItem(PFLLLL f,LONG a1,LONG a2)
{
   return NULL;

}




//Service Durations takes a long because it's used as a task.
void ItemEffects::ServiceDurations(LONG)
{
}




void ItemEffects::mfRemoveActiveItem(PFLLLL a,LONG b,LONG c)
{
}



void ItemEffects::WipeActiveItems()
{
}


SHORT ItemType::mfDuration(SHORT AvaStatsHdl) const
{
	return 0;
}
