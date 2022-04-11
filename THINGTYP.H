/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: THINGTYP.H    -Enumerations of the objects in the game.
   Author: Gary Powell
   ======================================================================== */
#ifndef _THINGTYP_H
#define _THINGTYP_H

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */


//enum table for objects in Birthright
// Note: The numbers assigned to the enumeration are the same as the indices
//       as the values in mythings[objectindex].type.

typedef enum {
	NOLOW			 		=	-1,
	NO_THING         		=    0,	// (nothing)
//	PLAYERSTART1     		=    1,	// Player 1 start
//	PLAYERSTART2     		=    2,	// Player 2 start
//	PLAYERSTART3     		=    3,	// Player 3 start
//	PLAYERSTART4     		=    4,	// Player 4 start
//	DEATHMATCH       		=   11,	// Deathmatch start positions.
//	TELEPORT         		=   14,	// teleport to the SECTOR containing this thing
	FIRST_GAME_THING 		=   15,	// first game thing
	GARGOYLE_1				=	15,	// Gargoyle
	T_GARGOYLE_1			=	16,	// Troop Gargoyle
	OTYUGH_1				=	17,	// Otyugh
	T_OTYUGH_1				=	18,	// Troop Otyugh
		
	ARCHER           		=   20,	// Archer
	LIGHT_CAVALRY    		=   21,	// Light Cavalry
	ELITE_INFANTRY   		=   22,	// Elite Infantry
	INFANTRY         		=   23,	// Infantry
	IRREGULAR        		=   24,	// Irregular
	KNIGHT           		=   25,	// Knight/Heavy Cavalry
	T_LORD_MALE_1    		=   26,	// Troop Lord Male 1
	OFFICER          		=   27,	// Officer
	PIKEMAN          		=   28,	// Pikeman
	T_WYVERN_1         		=   29,	// Troop Wyvern	1
	T_ANKHEG          		=   30,	// Troop Ankheg
	T_OGRE           		=   31,	// Troop Ogre
	T_HELL_HOUND     		=   32,	// Troop Hell Hound
	T_HARPY          		=   33,	// Troop Harpy
	T_SKELETON       		=   34,	// Troop Skeleton
	T_GIANT_SPIDER   		=   35,	// Troop Giant Spider
	T_SPECTRE        		=   36,	// Troop Spectre
	T_WRAITH         		=   37, // Troop Wraith
	DWARF_ARCHER     		=   38,	// Dwarf Archer
	DWARF_INFANTRY   		=   39,	// Dwarf Infantry
	T_DWARF_OFFICER_1    	=   40,	// Troop Dwarf Officer 1
	T_DWARF_OFFICER_2		=	41, // Troop Dwarf Officer 2
	ELF_ARCHER       		=   42,	// Elf Archer
 	ELF_OFFICER_2    		=   43,	// Elf Officer 2
	ELF_INFANTRY     		=   44,	// Elf Infantry
	ELF_OFFICER_1      		=   45,	// Elf Officer 1
	T_DWARF_OFFICER_3		=   46,	// Troop Dwarf Officer 3
	GOBLIN_ARCHER    		=   47,	// Goblin Archer
	GOBLIN_CAVALRY   		=   48,	// Goblin Cavalry
	GOBLIN_INFANTRY  		=   49,	// Goblin Infantry
	T_GOBLIN_ZOMBIE	 		=   50,	// Troop Goblin Zombie
	T_GOBLIN_OFFICER 		=   51,	// Troop Goblin Officer
	T_GOBLIN_QUEEN   		=   52,	// Troop Goblin Queen
	T_ZOMBIE         		=   53,	// Troop Zombie
	T_CEILING_SPIDER 		=   54,	// Troop Ceiling Spider
	MERC_INFANTRY    		=   55,	// Mercenary Infantry
	T_ELF_LADY_LORD  		=   56,	// Troop Elf Lady Lord
	MERC_OFFICER     		=   57,	// Mercenary Officer
	MERC_PIKEMAN     		=   58,	// Mercenary Pikeman
	T_DOG            		=   59, // Troop Dog
	WIZARD_MALE_4			=	60,	// Wizard Male 4
	T_WIZARD_MALE_4			=	61,	// Troop Wizard Male 4
	T_LORD_MALE_2    		=   62, // Troop Lord Male 2
	WIZARD_MALE_3			=   63,	// Wizard Male 3 
	T_LORD_MALE_3    		=   64,	// Troop Lord Male 3
	CHIEF            		=   65,	// Chief
	LORD_MALE_4       		=   66,	// Lord Male 4
	LORD_MALE_5       		=   67,	// Lord Male 5
	GUARD            		=   68,	// Guard
	LIEUTENANT       		=   69,	// Lieutenant
	GUILDER_MALE_1         	=   70,	// Guilder Male 1
	GUILDER_MALE_2          =   71,	// Guilder Male 2
	T_LORD_MALE_4    		=   72,	// Troop Lord Male 4
	T_LORD_MALE_5         	=   73,	// Troop Lord Male 5
	LORD_MALE_6         	=   74,	// Lord Male 6
	PRIEST_MALE_1   		=   75,	// Priest Male 1
	PRIEST_MALE_2   		=   76,	// Priest Male 2
	PRIEST_FEMALE_1   		=   77,	// Priest Female 1
	T_LORD_MALE_6    		=   78,	// Troop Lord Male 6
	ELF_LADY_LORD  			=   79,	// Elf Lady Lord
	T_LORD_MALE_7    		=   80,	// Troop Lord Male 7
	ELF_LORD_1     			=   81,	// Elf Lord 1
	T_LORD_FEMALE_1  		=   82,	// Troop Lord Female 1
	ROGUE            		=   83,	// Rogue
	ROYAL_GUARD      		=   84,	// Royal Guard
	T_GUILDER_FEMALE 		=   85,	// Troop Guilder Female
	LORD_MALE_7      		=	86,	// Lord Male 7
	GUILDER_FEMALE    		=   87,	// Guilder Female
	DWARF_LORD_3    		=   88,	// Dwarf Lord 3
	DWARF_LORD_2    		=   89,	// Dwarf LOrd 2
	GOBLIN_LORD_1    		=   90,	// Goblin Lord 1
	WARRIOR          		=   91,	// Warrior
	GOBLIN_QUEEN            =   92,	// Goblin Queen
	WIZARD_FEMALE_1  		=   93,	// Wizard Female 1
	WIZARD_FEMALE_2  		=   94,	// Wizard Female 2
	WIZARD_MALE_1    		=   95,	// Wizard Male 1
	WIZARD_MALE_2    		=   96,	// Wizard Male 2
	
	LORD_FEMALE_1    		=   98,	// Lord Female 1
	LORD_FEMALE_2    		=   99,	// Lord Female 2
	LORD_FEMALE_3    		=  100,	// Lord Female 3
	LORD_MALE_1      		=  101,	// Lord Male 1
	LORD_MALE_2      		=  102,	// Lord Male 2
	LORD_MALE_3      		=  103,	// Lord Male 3
	T_LORD_FEMALE_2  		=  104,	// Troop Lord Female 2
	T_LORD_FEMALE_3  		=  105,	// Troop Lord Female 3
	T_WIZARD_FEMALE_1		=  106,	// Troop Wizard Female 1
	T_WIZARD_FEMALE_2		=  107,	// Troop Wizard Female 2
	T_WIZARD_MALE_1  		=  108,	// Troop Wizard Male 1
	T_WIZARD_MALE_2  		=  109,	// Troop Wizard Male 2
	
	T_WIZARD_MALE_3  		=  111,	// Troop Wizard Male 3
	T_GUILDER_MALE_1 		=  112,	// Troop Guilder Male 1
	T_GUILDER_MALE_2 		=  113,	// Troop Guilder Male 2
	T_PRIEST_MALE_1  		=  114,	// Troop Priest Male 1
	T_PRIEST_MALE_2  		=  115,	// Troop Priest Male 2
	DWARF_LORD_1     		=  116,	// Dwarf Lord 1
	DWARF_GUARD      		=  117,	// Dwarf Guard
	T_PRIEST_FEMALE_1		=  118,	// Troop Priest Female 1
	ELF_GUARD        		=  119,	// Elf Guard
	ELF_LORD_2       		=  120,	// Elf Lord 2
	GNOLL_1					=  121, // Gnoll 1
	GNOLL_IRREGULAR			=  122, // Gnoll Irregular
	T_WYVERN_2				=  123, // Troop Wyvern 2
	GOBLIN_GUARD     		=  124,	// Goblin Guard
	GOBLIN_ZOMBIE			=  125, // Goblin Zombie
	ANKHEG           		=  126,	// Ankheg
	WYVERN_2				=  127, // Wyvern 2
	WRAITH             	 	=  128,	// Wraith
	DOG              		=  129,	// Dog
	GORGON           		=  130,	// Gorgon
	HARPY            		=  131,	// Harpy
	HELL_HOUND       		=  132,	// Hell Hound
	OGRE             		=  133,	// Ogre
	MANSLAYER        		=  134,	// Rhuobhe Manslayer
	SKELETON         		=  135,	// Skeleton
	SPECTRE          		=  136,	// Spectre
	SPIDER_KING      		=  137,	// Spider King
	GIANT_SPIDER     		=  138,	// Giant Spider
	WYVERN_1           		=  139,	// Wyvern 1
	ZOMBIE			 		=  140,	// Zombie
	CEILING_SPIDER   		=  141,	// Giant Spider on ceiling 
	T_MANSLAYER      		=  142,	// Troop Manslayer
	T_GORGON		 		=  143,	// Troop Gorgon
	T_SPIDER_KING	 		=  144,	// Troop Spider King
	ASSASSIN 				=  145, // Assassin
	T_ASSASSIN				=  146, // Troop Assassin
	T_INFANTRY_X	 		=  147,	// Infantry Intro
	ROYAL_GUARD_X	 		=  148,	// Royal Guard Intro
	LORD_THRONE		 		=  149,	// Lord Throne
	LAST_GAME_AVATAR 		=  149,	// 
	
	
	BARREL_1         		=  150,	// Barrel 1
	BOWL_1           		=  151,	// Bowl 1
	BUCKET_1         		=  152,	// Bucket1
	CHANDELIER_1     		=  153,	// Chandelier 1
	CANDLE_1         		=  154,	// Candle 1
	COLUMN_1         		=  155,	// Column 1
	COLUMN_2         		=  156,	// Column 2
	CUP_1            		=  157,	// Cup 1
	FIRE_1           		=  158,	// Fire 1
	FLAGON_1         		=  159,	// Flagon 1
	
	HELMET_1         		=  160,	// Helmet 1
	KNIFE_1          		=  161,	// Knife 1
	PLATE_1          		=  162,	// Plate 1
	POTTED_PLANT_1   		=  163,	// Potted Plant 1
	POTION_OF_FIRE_RESISTANCE		= 	164,
	SCROLL_OF_ARMOR_ENHANCEMENT_1	=	165,	
	SPEAR_1          		=  166,	// Spear 1
	STATUE_1         		=  167,	// Statue 1
	STATUE_2         		=  168,	// Statue 2
	STOOL_1          		=  169,	// Stool 1
	
	SWORD_1          		=  170,	// Sword 1
	TABLE_1          		=  171,	// Table 1
	TOME_OF_THE_PRINCE		=  172, // Tome of the Prince
	WALL_TORCH_1     		=  173,	// Wall Torch 1
	VASE_1           		=  174,	// Vase 1
	FOUNTAIN_1		 		=  175,	// Fountain 1
	BUSH_1			 		=  176,	// Bush 1
	TREE_1			 		=  177,	// Tree 1
	TREE_2			 		=  178,	// Tree 2
	WALL_TORCH_2     		=  179,	// Wall Torch 2
	
	DUNGEON_COLUMN	 		= 180,
	CAVE_COLUMN_1	 		= 181,
	BIZARRE_COLUMN_1 		= 182,
	MARBLE_COLUMN	 		= 183,
	STALACTITE_1	 		= 184,
	STALACTITE_2	 		= 185,
	STALACTITE_3	 		= 186,
	BIZARRE_STALACTITE_1 	= 187,
	STALAGMITE_1			= 188,
	STALAGMITE_2			= 189,
	
	CAVE_ROCK_1				= 190,
	CAVE_ROCK_2				= 191,
	CAVE_ROCK_3				= 192,
	BIZARRE_ROCK			= 193,
	DUNGEON_RUBBLE			= 194,
	CAVE_RUBBLE				= 195,
	STATUE_3				= 196,
	TORCH_2					= 197,
	HANGING_CHAINS_1 		= 198,
	KEY_SILVER				= 199,
	
	KEY_TINY				= 200,
	KEY_RED					= 201,
	KEY_BLUE				= 202,
	KEY_JEWELED				= 203,
	KEY_WHITE				= 204,
	KEY_SKELETON			= 205,
	SMASHED_BARREL_1 		= 206,
	EMPTY_CHEST				= 207,
	TREASURE_CHEST			= 208,
	CANDELABRA_1			= 209,
	
	CANDELABRA_2			= 210,
	POTION_OF_EXTRA_HEALING	= 211, 
	KEY_OF_OPENING			= 212, 
	POTION_OF_HEALING		= 213, 
	POTION_OF_FLYING		= 214, 
	BROKEN_VASE_1			= 215,
	VASE_2					= 216,
	BROKEN_VASE_2			= 217,
	VASE_3					= 218,
	BROKEN_VASE_3			= 219,
	
	TABLE_2					= 220,
	TABLE_3					= 221,
	STOOL_2					= 222,
	STOOL_3					= 223,
	POTTED_PLANT_2			= 224,
	POTTED_PLANT_3			= 225,
	STATUE_4				= 226,
	TORCH_3					= 227,
	GOLD_COINS_1			= 228,
	GOLD_COINS_2			= 229,
	
	BIZARRE_COLUMN_2 		= 230,
	CAVE_COLUMN_2			= 231,
	BIZARRE_STALACTITE_2 	= 232,
	TREE_3					= 233,
	TREE_4					= 234,
	TREE_5					= 235,
	TREE_6					= 236,
	TREE_7					= 237,
	TREE_8					= 238,
	TREE_9					= 239,
	TREE_10					= 240,
	
	TREE_11					= 241,
	TREE_12					= 242,
	TREE_13					= 243,
	TREE_14					= 244,
	STMP_1					= 245,
	STMP_2					= 246,
	THORN_1					= 247,
	BUSH_2					= 248,
	BUSH_3					= 249,
	
	BUSH_4					= 250,
	BUSH_5					= 251,
	BUSH_6					= 252,
	BUSH_7					= 253,
	FOREST_COLUMN_1			= 254,
	FOREST_COLUMN_2			= 255,
	EVIL_COLUMN_1			= 256,
	TWIG_1					= 257,
	EVIL_COLUMN_2			= 258,
	EVIL_CHANDELIER			= 259,
	
	LEAF_1					= 260,	
	LEAF_2					= 261,	
	LEAF_3					= 262,	
	GOLD_COINS_3			= 263,
	GOLD_COINS_4			= 264,
	GOLD_COINS_5			= 265,
	ROCK_5					= 266,
	ROCK_6					= 267,
	ROCK_7					= 268,
	ROCK_8					= 269,
	
	BARREL_3				= 270,
	STATUE_6				= 271,
	FIRE_2					= 272,
	FIRE_3					= 273,
	FIRE_4					= 274,
	FIRE_5					= 275,
	T0RCH_5					= 276,
	STATUE_5				= 277,
	VASE_7					= 278,
	VASE_8					= 279,
	
	
	AMULET_OF_FEATHER_FALL  			= 280,						
	AMULET_OF_SPEED         			= 281,						
	AMULET_OF_STRIDING_AND_SPRINGING   	= 282,						
	AMULET_VERSUS_UNDEAD 				= 283,						
	AMULET_OF_WATER_WALKING             = 284,						
	AMULET_OF_INSPIRATION               = 285,						
	BALM_OF_HEALING   					= 286,						
	TOME_OF_MYSTIC_MAGICS				= 287,						
	BOOK_OF_EXALTED_DEEDS   			= 288,						
	BOOK_OF_VILE_DARKNESS   			= 289,						
	
	TOME_OF_SORCERORS_LORE				= 290,						
	CHIME_OF_OPENING	 				= 291,						
	CLOAK_OF_PROTECTION_1   			= 292,						
	CLOAK_OF_PROTECTION_2   			= 293,						
	CLOAK_OF_PROTECTION_3   			= 294,						
	TOME_OF_THE_HIGH_WIZARDS			= 295,						
	TOME_OF_THE_MAGE_LORDS				= 296,		
	GAUNTLETS_OF_DEXTERITY   			= 297,						
	GAUNTLETS_OF_OGRE_POWER   			= 298,						
	GEM_OF_ATTRACTION   				= 299,						
	
	GEM_BLUE   							= 300,						
	GEM_GREEN   						= 301,						
	GEM_OF_PASSAGE   					= 302,						
							
	GEM_OF_SHIFTING   					= 304,						
							
	IOUN_STONE_PALE_BLUE   				= 306,						
	IOUN_STONE_SCARLET_AND_BLUE   		= 307,						
	IOUN_STONE_INCANDESCENT_BLUE   		= 308,						
	IOUN_STONE_DEEP_RED   				= 309,						
	
	IOUN_STONE_PINK   					= 310,						
	IOUN_STONE_PINK_AND_GREEN   		= 311,						
							
	IOUN_STONE_PEARLY_WHITE   			= 313,						
	SPELL_SCROLL_1						= 314,
	SPELL_SCROLL_2						= 315,
	SPELL_SCROLL_3						= 316,
	IOUN_STONE_DUSTY_ROSE   			= 317,						
	KEY_BLACK   						= 318,						
	KEY_BONE   							= 319,						
	
	KEY_STONE   						= 320,						
	LIBRIUM_OF_GAINFUL_CONJURATION   	= 321,						
	LIBRIUM_OF_INEFFABLE_DAMNATION   	= 322,						
	LIBRIUM_OF_SILVER_MAGIC   			= 323,						
							
	NECKLACE_OF_MISSILES_8 				= 325,						
							
	OIL_OF_SHARPNESS_1  				= 327,						
	OIL_OF_SHARPNESS_2  				= 328,						
	OIL_OF_SHARPNESS_3  				= 329,						
	
	OIL_OF_SHARPNESS_4  				= 330,						
	OIL_OF_SHARPNESS_5  				= 331,						
							
							
	POTION_OF_HILL_GIANT_STRENGTH   	= 334,						
	POTION_OF_STONE_GIANT_STRENGTH   	= 335,
	POTION_OF_FROST_GIANT_STRENGTH   	= 336,
	POTION_OF_FIRE_GIANT_STRENGTH    	= 337,
	POTION_OF_CLOUD_GIANT_STRENGTH   	= 338,
	POTION_OF_STORM_GIANT_STRENGTH   	= 339,
	
	POTION_OF_TITAN_STRENGTH   			= 340,
	
	POTION_OF_DIMINUTION   				= 342,
	
	POTION_OF_INVULNERABILITY   		= 344,
	POTION_OF_LEVITATION   				= 345,
	RING_OF_FEATHER_FALLING   			= 346,
	RING_OF_FIRE_FALLING   				= 347,
	
	RING_OF_JUMPING   					= 349,
	
	RING_OF_PROTECTION_1   				= 350,
	RING_OF_PROTECTION_2   				= 351,
	RING_OF_PROTECTION_3   				= 352,
	RING_OF_PROTECTION_4   				= 353,
	RING_OF_REGENERATION   				= 354,
	RING_OF_RESISTANCE_1   				= 355,
	RING_OF_RESISTANCE_2   				= 356,
	RING_OF_RESISTANCE_3   				= 357,
	RING_OF_WIZARDRY					= 358,
	RING_OF_NIGHTVISION   				= 359,
	
	DIERDRIENS_RING   					= 360,
	FAELES_RING   						= 361,
	ROD_OF_RESURRECTION   				= 362,
	SCROLL_OF_ARMOR_ENHANCEMENT_2   	= 363,
	SCROLL_OF_ARMOR_ENHANCEMENT_3   	= 364,
	SCROLL_OF_ARMOR_ENHANCEMENT_4   	= 365,
	SCROLL_OF_ARMOR_ENHANCEMENT_5   	= 366,
	
	STAFF_OF_CURING   					= 368,
	
	GAVELONS_STAFF_OF_PROSPERITY   		= 370,
	
	
	VIAL_OF_HOLY_WATER	 				= 372,
	WAND_OF_ENEMY_DETECTION_3			= 373,
	
	WAND_OF_FIRE_3 	 					= 375,
	
	WAND_OF_FROST_3	 					= 377,
	WAND_OF_ILLUMINATION_3	 			= 378,
	WAND_OF_LIGHTNING_3	 				= 379,
	
	WAND_OF_MAGIC_DETECTION_3	 		= 380,
	WAND_OF_PARALYZATION_3	 			= 381,
	WAND_OF_SECRET_DETECTION_3	 		= 382,
	BANNER_OF_ROELE	 					= 383,
	BARAZADS_TOOLS	 					= 384,
	
	BRENNAS_FAVOR	 					= 386,
	CHALICE_OF_THE_DEAD	 				= 387,
	CROWN_OF_COMMAND	 				= 388,
	STATE_CROWN_OF_ANUIRE	 			= 389,
	
	DANICAS_CRYSTAL_OF_SCRYING	 		= 390,
	EMPERORS_CROWN	 					= 391,
	FARIDS_COFFER_OF_THE_REALM	 		= 392,
	HAMMER_OF_THUNDER	 				= 393,
	
	ROBES_OF_THE_MASES	 				= 395,
	REGALIA_OF_EMPIRE	 				= 396,
	SIELSHEGH_GEM_LARGE	 				= 397,
	SIELSHEGH_GEM_MEDIUM	 			= 398,
	SWORD_OF_ROELE	 					= 399,
	
	SCEPTRE_OF_CUIRAECEN	 			= 400,
   	CORGANDALS_STAFF					= 401,
	NAPPOLANS_TOME_OF_WAR	 			= 402,
	TORC_OF_SPLENDOR	 				= 403,
	VAUBENELS_BOOK_OF_FORTIFICATION		= 404,
	CANDLE_OF_INVOCATION	 			= 405,
	
	PRINCE_ROP							= 406,
	GOBLIN_KING_ROP						= 407,
	
// OFFENSIVE WIZARD SPELLS

	ARROWS								= 408,
    FIRST_PROJECTILE_EFFECT				= 409,
	FIREBALL_1							= 409,
	LORES_FIREBALL_1					= 410,
	PLASMA_BALL_1						= 411,
	LORES_PLASMA_BALL_1					= 412,
	PLASMA_STREAK_1                     = 413,
	PLASMA_BALL_2                       = 414,
	LORES_PLASMA_STREAK_1				= 415,
	LORES_PLASMA_BALL_2					= 416,
	LIGHTNING_1							= 417,
	LORES_LIGHTNING_1					= 418,
	VAPORIZE_1							= 419,
	LORES_VAPORIZE_1					= 420,
	CRUMBLE_1							= 421,
	LORES_CRUMBLE_1						= 422,
	CHAIN_LIGHTNING_2					= 423,
    LAST_PROJECTILE_EFFECT				= 423,

// Some other random items

	POTION_OF_POISON_CURE				= 424,
	ROCK_9								= 425,
	ROCK_10								= 426,
	BUSH_8								= 427,
	BUSH_9								= 428,
	BUSH_10								= 429,
	BUSH_11								= 430,
	BUSH_12								= 431,
	TREE_15								= 432,
	TREE_16								= 433,
	TREE_17								= 434,
	TREE_18								= 435,
	TREE_19								= 436,
	TREE_20								= 437,
	CORPSE_1							= 438,
	HAY_PILE_1	  						= 439,
	GARGOYLE_STATUE						= 440,
	REFUSE_PILE							= 441,
	GORGONS_BANNER						= 442,
	GRAVESTONE_1						= 443,
	GRAVESTONE_2						= 444,
	GRAVESTONE_3						= 445,

// REALM SPELLS
	
	REALM_ALCHEMY_1						= 450,
	REALM_DEATH_PLAGUE_1				= 451,
	REALM_DEMAGOGUE_1					= 452,
	REALM_DISPEL_REALM_MAGIC_1			= 453,
	REALM_LEGION_OF_DEAD_1				= 454,
	REALM_MASS_DESTRUCTION_1			= 455,
	REALM_RAZE_1						= 456,
	REALM_SCRY_1						= 457,
	REALM_STRONGHOLD_1					= 458,
	REALM_SUBVERSION_1					= 459,
	REALM_SUMMONING_1					= 460,
	REALM_TRANSPORT_1					= 461,
	REALM_WARDING_1						= 462,
	
// PRIESTLY REALM SPELLS
	
	REALM_BLESS_LAND_1                  = 463,
	REALM_BLESS_ARMY_1                  = 464,
	REALM_BLIGHT_1                      = 465,
	REALM_DISPEL_REALM_MAGIC_PR_1       = 466,
	REALM_HONEST_DEALING_1              = 467,
	REALM_INVESTITURE_1                 = 468,
	
// PRIEST SPELLS														  
    
	CURE_LIGHT_1						= 469,
	DETECT_EVIL_1						= 470,
	DETECT_MAGIC_PR_1					= 471,
	LIGHT_PR_1							= 472,
	SHILLELAGH_1						= 473,
	TURN_UNDEAD_1						= 474,
	BARKSKIN_1							= 475,
	FIND_TRAPS_1						= 476,
	FIND_TREASURE_1						= 477,
	RESIST_FIRE_1						= 478,
	SPIRITUAL_HAMMER_1					= 479,
	MAGICAL_VESTAMENT_1					= 480,
	WATER_WALK_1						= 481,
	CURE_SERIOUS_1						= 482,
	PROT_FROM_EVIL_1					= 483,
	CURE_CRITICAL_1						= 484,
	FLAME_STRIKE_1						= 485,
	TRUE_SEEING_1						= 486,
	HEAL_1								= 487,
	FIRE_STORM_1						= 488,
	HOLY_WORD_1							= 489,
	RESURRECTION_1						= 490,
										
// MORE WIZARD SPELLS									
																		
	DETECT_MAGIC_1						= 497,
	FEATHER_FALL_1						= 498,
	JUMP_1								= 499,
	LIGHT_1								= 500,
	MAGIC_MISSILE_1						= 501,
	REDUCE_1							= 502,
	SHOCKING_GRASP_1					= 503,
	BLINDNESS_1							= 504,
	CONTINUAL_LIGHT_1					= 505,
	KNOCK_1								= 506,
	LEVITATE_1							= 507,
	LOCATE_OBJECT_1						= 508,
	POISON_ARROW_1						= 509,
	STRENGTH_1							= 510,
//	FIREBALL_1							= 511,  //already above
	FLY_1								= 512,
	STONE_UNDEAD_1						= 513,
	INFRAVISION_1						= 514,
	LIGHTNING_BOLT_1					= 515,
	CONFUSION_1							= 516,
	ICE_STORM_1							= 517,
	MINOR_GLOBE_INVULN_1				= 518,
	STONESKIN_1							= 519,
	CONE_OF_COLD_1						= 520,
	DISINTIGRATE_1						= 521,
	TELEPORT_1							= 522,
	CHAIN_LIGHTNING_1					= 523,
	DEATH_1								= 524,
	GLOBE_INVULN_1						= 525,
	FLESH_TO_STONE_1					= 526,
	IMPROVED_FIREBAL_1					= 527,
	POWER_STUN_1						= 528,
	POWER_BLIND_1						= 529,
	SPELL_IMMUNITY_1					= 530,
	METEOR_SWARM_1						= 531,
	POWER_KILL_1						= 532,
	TIME_STOP_1							= 533,

	
// MORE CHARACTERS							  
											  
	MERC_ARCHER							= 551,
	MERC_CAVALRY						= 552,
	ELF_CAVALRY							= 553,
	LEVY								= 554,
	SCOUT								= 555,
	MERC_IRREGULAR						= 556,
	CAV_HORSE							= 557,
	KNIGHT_HORSE						= 558,
											  
											  
// HINTS											  
	
	HINT_1	 							= 561,
	HINT_2								= 562,
	HINT_3								= 563,
	HINT_4								= 564,
	HINT_5								= 565,

// FIENDS

	FIRST_FIEND							= 566,
	MOLTH_KAMBAR						= 566,
	KAST_EKCTRAL						= 567,
	BAUBB_THE_TOYMAKER					= 568,
	ENDAERAL_CATHBIRN					= 569,
	SPAWN_OF_THE_CHIMAERA				= 570,
	NHOUN_THE_ELF						= 571,
	TIESKAR_GRAECHER					= 572,
	
	LAST_FIEND							= 572,
	
	LESSER_SPECTRE						= 573,
	LESSER_WRAITH						= 574,
	
	IOUN_HEART							= 581,
	IOUN_FIST							= 582,

// Blood Abilities

    BLOOD_ALERTNESS_1					= 601,
	BLOOD_ALTER_APPEARANCE_1			= 602,
	BLOOD_ANIMAL_AFFINITY_1				= 603,
	BLOOD_BATTLEWISE_1					= 604,
	BLOOD_BLOOD_HISTORY_1				= 605,
	BLOOD_BLOODMARK_1					= 606,
	BLOOD_CHARACTER_READING_1			= 607,
	BLOOD_COURAGE_1						= 608,
	BLOOD_DETECT_LIE_1					= 609,
	BLOOD_DETECT_ILLUSION_1				= 610,
	BLOOD_DIRECTION_SENSE_1				= 611,
	BLOOD_DIVINE_AURA_1					= 612,
	BLOOD_DIVINE_WRATH_1				= 613,
	BLOOD_ELEMENTAL_CONTROL_1			= 614,
	BLOOD_ENHANCED_SENSE_1				= 615,
	BLOOD_FEAR_1						= 616,
	BLOOD_HEALING_1						= 617,
	BLOOD_HEIGHTENED_ABILITY_1			= 618,
	BLOOD_IRON_WILL_1					= 619,
	BLOOD_PERSUASION_1					= 620,
	BLOOD_POISON_SENSE_1				= 621,
	BLOOD_PROTECTION_FROM_EVIL_1		= 622,
	BLOOD_REGENERATION_1				= 623,
	BLOOD_RESISTANCE_1					= 624,
	BLOOD_SHADOW_FORM_1					= 625,
	BLOOD_TOUCH_OF_DECAY_1				= 626,
	BLOOD_TRAVEL_1						= 627,

// lores art for battle 8Meg version
	LORES_A_BOW							= 628,	
	LORES_A_CAV							= 629,	
	LORES_A_EIN							= 630,	
	LORES_A_INF							= 631,	
	LORES_A_IRR							= 632,	
	LORES_A_KNT							= 633,	
	LORES_A_OFF							= 634,	
	LORES_A_PIK							= 635,	
	LORES_M_INF							= 636,	
	LORES_M_OFF							= 637,	
	LORES_M_PIK							= 638,	
	LORES_D_INF							= 639,	
	LORES_D_BOW							= 640,	
	LORES_D_OFF							= 641,	
	LORES_E_BOW							= 642,	
	LORES_E_INF							= 643,	
	LORES_E_OFF							= 644,	
	LORES_G_BOW							= 645,	
	LORES_G_CAV							= 646,	
	LORES_G_INF							= 647,	
	LORES_G_OFF							= 648,	
	LORES_C_SKL							= 649,	
	LORES_C_SPD							= 650,	
	LORES_N_IRR							= 651,	
	LORES_G_ZOM							= 652,
	LORES_C_HPY							= 653,
	LORES_C_HEL							= 654,
	LORES_C_ZOM							= 655,
	LORES_C_WYV							= 656,
	LORES_C_WYV2						= 657,
	LORES_C_OGR							= 658,
	LORES_C_ANK							= 659,
	LORES_C_WTH							= 660,
	LORES_C_SPC							= 661,
	LORES_C_SPD2						= 662,
	LORES_C_DOG							= 663,
	LORES_C_GRG							= 664,
	LORES_C_OTY							= 665,
	LORES_CAV_HORSE						= 666,
	LORES_KNIGHT_HORSE					= 667,

//WRC
//this is a neat little fix to the problem with charges, each different
//charge level has it's own distinct thingtype and itemtype. 
	NECKLACE_OF_MISSILES_7 				= 668,
	NECKLACE_OF_MISSILES_6 				= 669,
	NECKLACE_OF_MISSILES_5 				= 670,
	NECKLACE_OF_MISSILES_4 				= 671,
	NECKLACE_OF_MISSILES_3 				= 672,
	NECKLACE_OF_MISSILES_2 				= 673,
	NECKLACE_OF_MISSILES_1 				= 674,
	WAND_OF_ENEMY_DETECTION_2			= 675,
	WAND_OF_ENEMY_DETECTION_1			= 676,
	WAND_OF_FIRE_2						= 677,
	WAND_OF_FIRE_1						= 678,
	WAND_OF_FROST_2						= 679,
	WAND_OF_FROST_1						= 680,
	WAND_OF_ILLUMINATION_2				= 681,
	WAND_OF_ILLUMINATION_1				= 682,
	WAND_OF_LIGHTNING_2					= 683,
	WAND_OF_LIGHTNING_1					= 684,
	WAND_OF_MAGIC_DETECTION_2			= 685,
	WAND_OF_MAGIC_DETECTION_1			= 686,
	WAND_OF_PARALYZATION_2				= 687,
	WAND_OF_PARALYZATION_1				= 688,
	WAND_OF_SECRET_DETECTION_2			= 689,
	WAND_OF_SECRET_DETECTION_1			= 690,
	

LAST_GAME_THING  						= 690		// keep this up-to-date!!!
											
} THINGTYPE;								
											
											
// JHC HACK for doors.c						
#define KEY_RED_RUBY		KEY_RED			
#define KEY_GREEN_EMERALD	KEY_GREEN		
#define KEY_BLUE_SAPPHIRE	KEY_BLUE		
#define KEY_WHITE_DIAMOND	KEY_WHITE		

// DKT more hackage 961023
#define KEY_GOLD			KEY_TINY
#define KEY_GREEN			KEY_JEWELED
#define KEY_PURPLE			KEY_BLACK
											
#endif // _THINGTYPE_H						
											
