/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aiftseq.cpp  -
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "AIFTSEQ.HXX"

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
static FIGHT_SEQUENCE::ATTACK_MODE NoDefense[] = {
	  FIGHT_SEQUENCE::ATM_HIGH,	
	  FIGHT_SEQUENCE::ATM_LOW,	
	  FIGHT_SEQUENCE::ATM_LOOP
};
#define Weak_NoDefense	ATM_NONE

static FIGHT_SEQUENCE::ATTACK_MODE Defensive[] = {
	  FIGHT_SEQUENCE::ATM_DEFEND,	
	  FIGHT_SEQUENCE::ATM_QUICK_LOW,	
	  FIGHT_SEQUENCE::ATM_DEFEND,	
	  FIGHT_SEQUENCE::ATM_DEFEND,	
	  FIGHT_SEQUENCE::ATM_HIGH,	
	  FIGHT_SEQUENCE::ATM_DEFEND, 
	  FIGHT_SEQUENCE::ATM_LOOP
};
#define Weak_Defensive	ATM_QUICK_LOW

static FIGHT_SEQUENCE::ATTACK_MODE ModeratelyDefensive[] = {
	  FIGHT_SEQUENCE::ATM_DEFEND,	
	  FIGHT_SEQUENCE::ATM_DEFEND,	
	  FIGHT_SEQUENCE::ATM_HIGH,	
	  FIGHT_SEQUENCE::ATM_LOW, 
	  FIGHT_SEQUENCE::ATM_QUICK_HIGH,	
	  FIGHT_SEQUENCE::ATM_DEFEND,	
	  FIGHT_SEQUENCE::ATM_LOOP
};
#define Weak_ModeratelyDefensive	ATM_LOW

static FIGHT_SEQUENCE::ATTACK_MODE ModeratelyAggressive[] = {
	  FIGHT_SEQUENCE::ATM_LOW,	
	  FIGHT_SEQUENCE::ATM_QUICK_HIGH,	
	  FIGHT_SEQUENCE::ATM_DEFEND,	
	  FIGHT_SEQUENCE::ATM_HIGH,	
	  FIGHT_SEQUENCE::ATM_QUICK_LOW, 
	  FIGHT_SEQUENCE::ATM_LOOP
};
#define Weak_ModeratelyAggressive	ATM_HIGH

static FIGHT_SEQUENCE::ATTACK_MODE Aggressive[] = {
	  FIGHT_SEQUENCE::ATM_LOW,	
	  FIGHT_SEQUENCE::ATM_LOW,	
	  FIGHT_SEQUENCE::ATM_QUICK_HIGH,	
	  FIGHT_SEQUENCE::ATM_DEFEND,	
	  FIGHT_SEQUENCE::ATM_HIGH,	
	  FIGHT_SEQUENCE::ATM_LOOP
};
#define Weak_Aggressive	ATM_LOW

static FIGHT_SEQUENCE::ATTACK_MODE ExtremelyAggressive[] = {
	  FIGHT_SEQUENCE::ATM_HIGH,	
	  FIGHT_SEQUENCE::ATM_QUICK_HIGH,	
	  FIGHT_SEQUENCE::ATM_QUICK_LOW,	
	  FIGHT_SEQUENCE::ATM_DEFEND,	
	  FIGHT_SEQUENCE::ATM_QUICK_HIGH,	
	  FIGHT_SEQUENCE::ATM_LOOP
};
#define Weak_ExtremelyAggressive	ATM_QUICK_LOW

static FIGHT_SEQUENCE::ATTACK_MODE HellHound[] = {
	  FIGHT_SEQUENCE::ATM_MAGIC,	
	  FIGHT_SEQUENCE::ATM_QUICK_HIGH,	
	  FIGHT_SEQUENCE::ATM_QUICK_HIGH,	
	  FIGHT_SEQUENCE::ATM_MAGIC,	
	  FIGHT_SEQUENCE::ATM_DEFEND, 
	  FIGHT_SEQUENCE::ATM_MAGIC,	
	  FIGHT_SEQUENCE::ATM_HIGH,	
	  FIGHT_SEQUENCE::ATM_LOOP
};

FIGHT_SEQUENCE::FIGHT_SEQUENCE_DATA FIGHT_SEQUENCE::fFightSequenceData[(LAST_GAME_AVATAR - FIRST_GAME_THING) + 1] = {
 // Weak Attack,	Attack Sequence.
	  {Weak_Defensive,   			Defensive},				// 	15 Gargoyle
	  {Weak_Defensive,   			Defensive},				// 	16 Troop Gargoyle
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 	17 Otyugh
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 	18 Troop Otyugh
	  {Weak_Defensive,  			Defensive},				//  No Art
	  {Weak_Defensive,   			Defensive},				//  20 Archer
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  21 Light Cavalry
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  22 Elite Infantry
	  {Weak_Defensive,   			Defensive},				//  23 Infantry
	  {Weak_Defensive,   			Defensive},				//  24 Irregular
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  25 Knight/Heavy Cavalry
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  26 Troop Lord Male 1
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  27 Officer
	  {Weak_Defensive,  			Defensive},				//  28 Pikeman
	  {Weak_Aggressive,  			Aggressive},			//  29 Troop Wyvern	1
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  30 Troop Ankheg
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  31 Troop Ogre
	  {ATM_NONE,      				HellHound},				//  32 Troop Hell Hound
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  33 Troop Harpy
	  {Weak_NoDefense,      	  	NoDefense},				//  34 Troop Skeleton
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  35 Troop Giant Spider
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  36 Troop Spectre
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  37 Troop Wraith
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  38 Dwarf Archer
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  39 Dwarf Infantry
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  40 Troop Dwarf Officer 1
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 	41 Troop Dwarf Officer 2
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  42 Elf Archer
 	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  43 Elf Officer 2
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  44 Elf Infantry
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  45 Elf Officer 1
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  46 Troop Dwarf Officer 3
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  47 Goblin Archer
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  48 Goblin Cavalry
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  49 Goblin Infantry
	  {ATM_NONE,      				NoDefense},				//  50 Troop Goblin Zombie
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  51 Troop Goblin Officer
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  52 Troop Goblin Queen
	  {Weak_NoDefense,   			NoDefense},				//  53 Troop Zombie
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  54 Troop Ceiling Spider
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  55 Mercenary Infantry
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  56 Troop Elf Lady Lord
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  57 Mercenary Officer
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  58 Mercenary Pikeman
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  59 Troop Dog
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 	60 Wizard Male 4
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 	61 Troop Wizard Male 4
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  62 Troop Lord Male 2
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  63 Wizard Male 3 
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  64 Troop Lord Male 3
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  65 Chief
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  66 Lord Male 4
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  67 Lord Male 5
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  68 Guard
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  69 Lieutenant
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  70 Guilder Male 1
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  71 Guilder Male 2
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  72 Troop Lord Male 4
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  73 Troop Lord Male 5
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  74 Lord Male 6
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  75 Priest Male 1
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  76 Priest Male 2
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  77 Priest Female 1
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  78 Troop Lord Male 6
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  79 Elf Lady Lord
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  80 Troop Lord Male 7
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  81 Elf Lord 1
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  82 Troop Lord Female 1
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  83 Rogue
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  84 Royal Guard
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  85 Troop Guilder Female
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	// 	86 Lord Male 7
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  87 Guilder Female
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  88 Dwarf Lord 3
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  89 Dwarf LOrd 2
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  90 Goblin Lord 1
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  91 Warrior
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  92 Goblin Queen
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  93 Wizard Female 1
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  94 Wizard Female 2
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  95 Wizard Male 1
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  96 Wizard Male 2
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  No Art
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  98 Lord Female 1
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	//  99 Lord Female 2
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	// 100 Lord Female 3
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	// 101 Lord Male 1
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	// 102 Lord Male 2
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	// 103 Lord Male 3
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	// 104 Troop Lord Female 2
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	// 105 Troop Lord Female 3
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 106 Troop Wizard Female 1
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 107 Troop Wizard Female 2
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 108 Troop Wizard Male 1
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 109 Troop Wizard Male 2
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	//  No Art
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 111 Troop Wizard Male 3
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 112 Troop Guilder Male 1
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 113 Troop Guilder Male 2
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 114 Troop Priest Male 1
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 115 Troop Priest Male 2
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	// 116 Dwarf Lord 1
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 117 Dwarf Guard
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 118 Troop Priest Female 1
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 119 Elf Guard
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	// 120 Elf Lord 2
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 121 Gnoll 1
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 122 Gnoll Irregular
	  {Weak_Aggressive,  			Aggressive},  			// 123 Troop Wyvern 2
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 124 Goblin Guard
	  {Weak_NoDefense,   			NoDefense},				// 125 Goblin Zombie
	  {Weak_Aggressive,  			Aggressive},			// 126 Ankheg
	  {Weak_Aggressive,  			Aggressive},			// 127 Wyvern 2
	  {Weak_Aggressive,  			Aggressive},			// 128 Wraith
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 129 Dog
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 130 Gorgon
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 131 Harpy
	  {ATM_NONE,     				HellHound},				// 132 Hell Hound
	  {Weak_Aggressive,  			Aggressive},   			// 133 Ogre
	  {Weak_ExtremelyAggressive,	ExtremelyAggressive}, 	// 134 Rhuobhe Manslayer
	  {Weak_NoDefense,  			NoDefense},				// 135 Skeleton
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	// 136 Spectre
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 137 Spider King
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 138 Giant Spider
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 139 Wyvern 1
	  {Weak_NoDefense,  			NoDefense},				// 140 Zombie
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 141 Giant Spider on ceiling 
	  {Weak_ExtremelyAggressive,	ExtremelyAggressive}, 	// 142 Troop Manslayer
	  {Weak_ExtremelyAggressive,	ExtremelyAggressive},  	// 143 Troop Gorgon
	  {Weak_ModeratelyDefensive,	ModeratelyDefensive},	// 144 Troop Spider King
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	// 145 Assassin
	  {Weak_ModeratelyAggressive,	ModeratelyAggressive},	// 146 Troop Assassin
	  {Weak_Defensive,  			Defensive},				// 147 Infantry Intro
	  {Weak_Defensive,  			Defensive},				// 148 Royal Guard Intro
	  {Weak_Defensive,  			Defensive} 				// 149 Lord Throne
};
