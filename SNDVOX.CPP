/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: SNDVOX.CPP - Handles Birthright Voice Sounds
   Author: 	 Greg Hightower
   ========================================================================
   Contains the following general functions:
   
   Contains the following internal functions:
   
   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "system.h"
#include "engine.h"
#include "thingtyp.h"
#include "sound.hxx"
#include "sndvox.hxx"
#include "vector.hxx"

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/*
 * the mask that we use is as follows:
 *
 * BITS: 15VOX_NO_ENTRY4  13-9  8-4  3-0
 *         ^      ^    ^    ^
 *         |      |    |    4 bits - one of 16 random samples as looked up
 *         |      |    |    in the NumBlockSamples array
 *         |      |    |
 *         |      |    5 bits for which of 32 block of content to use
 *         |      |    
 *         |      5 bits for which of 32 voices to use
 *         |          
 *         2 bits for which quad of samples to use
 *
 */
 
#define VOX_FIRST_QUAD		0x0
#define VOX_SECOND_QUAD		0x1
#define VOX_THIRD_QUAD		0x2
#define VOX_FOURTH_QUAD		0x3

#define VOX_QUAD_MASK		14
#define VOX_VOICE_MASK		9
#define VOX_BLOCK_MASK		4

/* ------------------------------------------------------------------------
	Macros   
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
/* the number of samples in each block type */
static DEFINE_STATIC_VECTOR_DATA_S(SHORT,NumBlockSamples,VOX_BLOCK_SIZE) = {
	3,	// VOX_CASTLE_INQUIRY,
	5,	// VOX_CASTLE_ACKNOWLEDGE,
	3,	// VOX_CASTLE_DECLINE,
	3,	// VOX_CHALLENGES,
	3,	// VOX_DUEL_TAUNTS,
	3,	// VOX_BATTLE_INQUIRY,
	4,	// VOX_BATTLE_ACKNOWLEDGE,
	3,	// VOX_BATTLE_DECLINE,
	4,	// VOX_BUSY,
	2,	// VOX_READY,
	1,	// VOX_WIZARD_READY,
	2,	// VOX_WAITING,
	3,	// VOX_UNDER_ATTACK,
	1,	// VOX_WE_GET_ROUTED,
	3,	// VOX_BACK_TO_RESERVES,
	3,	// VOX_RETREAT,
	1,	// VOX_ARCHERS_READY,
	1,  // VOX_PRIEST_READY,
	1	// VOX_THEY_GET_ROUTED,
};
DEFINE_VECTOR_CLASS(SHORT, NumBlockSamples);

static VOX_TYPE VoxThings[] = {
	VOX_NO_ENTRY,				// Gargoyle
	VOX_NO_ENTRY,				// Troop Gargoyle
	VOX_OTYUGH_1,			// Otyugh
	VOX_OTYUGH_1,			// Troop Otyugh
	VOX_NO_ENTRY,				// Unused art slot
	VOX_HUMAN_MALE_YOUNG_2,		// Archer
	VOX_HUMAN_MALE_YOUNG_2,		// Light Cavalry
	VOX_HUMAN_MALE_YOUNG_1,		// Elite Infantry
	VOX_HUMAN_MALE_YOUNG_1,		// Infantry
	VOX_HUMAN_MALE_YOUNG_3,		// Irregular
	VOX_HUMAN_MALE_YOUNG_1,		// Knight/Heavy Cavalry
	VOX_HUMAN_MALE_MATURE_2,	// Troop Lord Male 1
	VOX_HUMAN_MALE_YOUNG_1,		// Officer
	VOX_HUMAN_MALE_YOUNG_1,		// Pikeman
	VOX_NO_ENTRY,		      		// Troop Wyvern	1
	VOX_ANKHEG_1,	      		// Troop Ankheg
	VOX_OGRE_1,	      		// Troop Ogre
	VOX_NO_ENTRY,		      		// Troop Hell Hound
	VOX_NO_ENTRY,		      		// Troop Harpy
	VOX_NO_ENTRY,		      		// Troop Skeleton
	VOX_NO_ENTRY,		      		// Troop Giant Spider
	VOX_NO_ENTRY,		      		// Troop Spectre
	VOX_NO_ENTRY,	        		// Troop Wraith
	VOX_DWARF_MALE_1,	       	// Dwarf Archer
	VOX_DWARF_MALE_1,	       	// Dwarf Infantry
	VOX_DWARF_MALE_1,		// Troop Dwarf Officer 1
	VOX_DWARF_MALE_1,		// Troop Dwarf Officer 2
	VOX_HUMAN_MALE_YOUNG_3,	 	// Elf Archer
 	VOX_HUMAN_MALE_YOUNG_3,	 	// Elf Officer 2
	VOX_HUMAN_MALE_YOUNG_3,	 	// Elf Infantry
	VOX_HUMAN_MALE_YOUNG_3,	 	// Elf Officer 1
	VOX_DWARF_MALE_1,		// Troop Dwarf Officer 3
	VOX_GOBLIN_MALE_1,	  	// Goblin Archer
	VOX_GOBLIN_MALE_1,	  	// Goblin Cavalry
	VOX_GOBLIN_MALE_1,	  	// Goblin Infantry
	VOX_GOBLIN_MALE_1,	  	// Troop Goblin Zombie
	VOX_GOBLIN_MALE_1,	  	// Troop Goblin Officer
	VOX_GOBLIN_FEMALE_1,	   	// Troop Goblin Queen
	VOX_SKELETON_1,	     		// Troop Zombie
	VOX_NO_ENTRY,		      		// Troop Ceiling Spider
	VOX_HUMAN_MALE_MATURE_1,	// Mercenary Infantry
	VOX_HUMAN_FEMALE_MATURE_1,	// Troop Elf Lady Lord
	VOX_HUMAN_MALE_MATURE_2,	// Mercenary Officer
	VOX_HUMAN_MALE_MATURE_1,	// Mercenary Pikeman
	VOX_NO_ENTRY,		   		// Troop Dog
	VOX_HUMAN_MALE_OLD_1,		// Wizard Male 4
	VOX_HUMAN_MALE_OLD_1,		// Troop Wizard Male 4
	VOX_HUMAN_MALE_MATURE_2,	// Troop Lord Male 2
	VOX_HUMAN_MALE_MATURE_1,	// Wizard Male 3 
	VOX_HUMAN_MALE_MATURE_2,	// Troop Lord Male 3
	VOX_HUMAN_MALE_MATURE_1,	// Chief
	VOX_HUMAN_MALE_MATURE_1,	// Lord Male 4
	VOX_HUMAN_MALE_MATURE_2,	// Lord Male 5
	VOX_HUMAN_MALE_YOUNG_1,		// Guard
	VOX_HUMAN_MALE_YOUNG_2,		// Lieutenant
	VOX_HUMAN_MALE_YOUNG_3,		// Guilder Male 1
	VOX_HUMAN_MALE_MATURE_2,	// Guilder Male 2
	VOX_HUMAN_MALE_MATURE_1,	// Troop Lord Male 4
	VOX_HUMAN_MALE_MATURE_2,	// Troop Lord Male 5
	VOX_HUMAN_MALE_MATURE_2,	// Lord Male 6
	VOX_HUMAN_MALE_MATURE_1,	// Priest Male 1
	VOX_HUMAN_MALE_MATURE_2,	// Priest Male 2
	VOX_HUMAN_FEMALE_MATURE_1,	// Priest Female 1
	VOX_HUMAN_MALE_MATURE_2,	// Troop Lord Male 6
	VOX_HUMAN_FEMALE_MATURE_1,	// Elf Lady Lord
	VOX_HUMAN_MALE_MATURE_2,	// Troop Lord Male 7
	VOX_HUMAN_MALE_YOUNG_3,		// Elf Lord 1
	VOX_HUMAN_FEMALE_MATURE_2,	// Troop Lord Female 1
	VOX_HUMAN_MALE_MATURE_1,	// Rogue
	VOX_HUMAN_MALE_YOUNG_1,		// Royal Guard
	VOX_HUMAN_FEMALE_YOUNG_1,	// Troop Guilder Female
	VOX_HUMAN_MALE_MATURE_2,	// Lord Male 7
	VOX_HUMAN_FEMALE_YOUNG_2,	// Guilder Female
	VOX_DWARF_MALE_1,	    	// Dwarf Lord 3
	VOX_DWARF_MALE_1,	    	// Dwarf Lord 2
	VOX_GOBLIN_MALE_1,    		// Goblin Lord 1
	VOX_HUMAN_MALE_MATURE_2,	// Warrior
	VOX_GOBLIN_FEMALE_1,   		// Goblin Queen
	VOX_HUMAN_FEMALE_YOUNG_1,	// Wizard Female 1
	VOX_HUMAN_FEMALE_YOUNG_2,	// Wizard Female 2
	VOX_HUMAN_MALE_OLD_1, 		// Wizard Male 1
	VOX_HUMAN_MALE_OLD_1, 		// Wizard Male 2
	VOX_NO_ENTRY,		   		// Empty Art.
	VOX_HUMAN_FEMALE_MATURE_1,	// Lord Female 1
	VOX_HUMAN_FEMALE_MATURE_2,	// Lord Female 2
	VOX_HUMAN_FEMALE_MATURE_2,	// Lord Female 3
	VOX_HUMAN_MALE_MATURE_2,	// Lord Male 1
	VOX_HUMAN_MALE_MATURE_2,	// Lord Male 2
	VOX_HUMAN_MALE_MATURE_2,	// Lord Male 3
	VOX_HUMAN_FEMALE_MATURE_1,	// Troop Lord Female 2
	VOX_HUMAN_FEMALE_MATURE_2,	// Troop Lord Female 3
	VOX_HUMAN_FEMALE_YOUNG_2,	// Troop Wizard Female 1
	VOX_HUMAN_FEMALE_YOUNG_1,	// Troop Wizard Female 2
	VOX_HUMAN_MALE_OLD_1,		// Troop Wizard Male 1
	VOX_HUMAN_MALE_OLD_1,		// Troop Wizard Male 2
	VOX_NO_ENTRY,		   		// Empty Art.
	VOX_HUMAN_MALE_OLD_1,		// Troop Wizard Male 3
	VOX_HUMAN_MALE_YOUNG_1,		// Troop Guilder Male 1
	VOX_HUMAN_MALE_MATURE_1,	// Troop Guilder Male 2
	VOX_HUMAN_MALE_OLD_1, 		// Troop Priest Male 1
	VOX_HUMAN_MALE_YOUNG_1,		// Troop Priest Male 2
	VOX_DWARF_MALE_1,	      	// Dwarf Lord 1
	VOX_DWARF_MALE_1,	      	// Dwarf Guard
	VOX_HUMAN_FEMALE_MATURE_2,	// Troop Priest Female 1
	VOX_HUMAN_MALE_YOUNG_3,	      	// Elf Guard
	VOX_HUMAN_MALE_YOUNG_3,	      	// Elf Lord 2
	VOX_GNOLL_1,	      		// Gnoll 1
	VOX_GNOLL_1,	      		// Gnoll Irregular
	VOX_NO_ENTRY,		      		// Troop Wyvern 2
	VOX_GOBLIN_MALE_1,	      	// Goblin Guard
	VOX_GOBLIN_MALE_1,	      	// Goblin Zombie
	VOX_ANKHEG_1,	      		// Ankheg
	VOX_NO_ENTRY,		      		// Wyvern 2
	VOX_NO_ENTRY,		      		// Wraith
	VOX_NO_ENTRY,		      		// Dog
	VOX_GORGON_1,	      		// Gorgon
	VOX_NO_ENTRY,		      		// Harpy
	VOX_NO_ENTRY,		      		// Hell Hound
	VOX_OGRE_1,	      		// Ogre
	VOX_RHUOBHE_1,	      		// Rhuobhe Manslayer
	VOX_SKELETON_1,		   	// Skeleton
	VOX_NO_ENTRY,		      		// Spectre
	VOX_SPIDER_KING_1,     		// Spider King
	VOX_NO_ENTRY,		      		// Giant Spider
	VOX_NO_ENTRY,		      		// Wyvern 1
	VOX_SKELETON_1,	      		// Zombie
	VOX_NO_ENTRY,		      		// Giant Spider on ceiling 
	VOX_HUMAN_MALE_YOUNG_3,		      	// Troop Manslayer
	VOX_GORGON_1,	      		// Troop Gorgon
	VOX_SPIDER_KING_1,     		// Troop Spider King
	VOX_HUMAN_MALE_MATURE_2,	// Assassin
	VOX_HUMAN_MALE_YOUNG_3,		// Troop Assassin
	VOX_HUMAN_MALE_MATURE_1,	// Infantry Intro
	VOX_HUMAN_MALE_MATURE_2,	// Royal Guard Intro
	VOX_HUMAN_MALE_MATURE_2,	// Lord Throne
};
	
/* ========================================================================
   Function    - AddVoxSnd
   Description - add a things correct voice sound 
   Returns     - 
   ======================================================================== */
SHORT AddVoxSnd( VOX_BLOCK Block, LONG MyThingsIndex)
{
	VOX_TYPE	Voice = VOX_NO_ENTRY;
	
	if( MyThingsIndex >= 0 && MyThingsIndex < MAX_THINGS)
	{
		LONG const VoxIndex = mythings[MyThingsIndex].OriginalType - FIRST_GAME_THING;
		
		if (VoxIndex >= 0 && VoxIndex < (sizeof(VoxThings)/ sizeof(VOX_TYPE)))
		{
			Voice = VoxThings[VoxIndex];
		}
		
		if( Voice == VOX_NO_ENTRY ) // this thing isn't supported
			return 0;
		
		return(AddVoxSndObj( Voice, Block, MyThingsIndex));
	}
	return 0;
}

/* ========================================================================
   Function    - CalcVoxSndObj
   Description - Calculate the sound objec based on its type.
   Returns     - 
   ======================================================================== */
BIRTHRT_SND CalcVoxSndObj (VOX_TYPE Voice, VOX_BLOCK Block)
{
	USHORT	BitMask = 0;

	BitMask  = VOX_THIRD_QUAD << VOX_QUAD_MASK;	// which quad were using
	BitMask |= Voice << VOX_VOICE_MASK;	// which voice were using
	BitMask |= Block << VOX_BLOCK_MASK;	// which block were using
	BitMask += random(NumBlockSamples[Block]); //plus random sample

	return (BIRTHRT_SND)BitMask;
}

/* ========================================================================
   Function    - AddVoxSndObj
   Description - add a voice sound object
   Returns     - 
   ======================================================================== */
SHORT AddVoxSndObj (VOX_TYPE Voice, VOX_BLOCK Block, LONG MyThingsIndex)
{
	USHORT	BitMask = 0;

	BitMask = (USHORT) CalcVoxSndObj(Voice, Block);

	return AddSndObj((BIRTHRT_SND)BitMask, 0, MyThingsIndex);
}

/* ========================================================================
   Function    - AddVoxSndType
   Description - Add a voice, given a type. (Played at full volume.)
                 This is useful at the map level.
   Returns     - 
   ======================================================================== */

SHORT AddVoxSndType( VOX_BLOCK Block, THINGTYPE ThingType)
{
	VOX_TYPE	Voice = VOX_NO_ENTRY;
	
	if( ThingType >= 0 && ThingType <= LAST_GAME_AVATAR)
	{
		LONG const VoxIndex = ThingType - FIRST_GAME_THING;
		
		if (VoxIndex >= 0 && VoxIndex < (sizeof(VoxThings)/ sizeof(VOX_TYPE)))
		{
			Voice = VoxThings[VoxIndex];
		}
		
		if( Voice == VOX_NO_ENTRY ) // this thing isn't supported
			return 0;
		
		return(AddVoxSndObj( Voice, Block, VOLUME_FULL));
	}
	return 0;
}
/* ======================================================================== */
