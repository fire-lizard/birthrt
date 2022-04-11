/* ========================================================================
   Copyright (c) 1996  Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: SPECIAL.H
   Author: John Conley

	Contains #defines for special values for linedefs and sectors.
   ======================================================================== */

#if !defined(_SPECIAL_H)
#define _SPECIAL_H

// Linedef specials.
#define LSP_DOOR_DMS			1		// Door, Doom, Manual, Slow
#define LSP_DOOR_DWS			4		// Door, Doom, Walkover, Slow
#define LSP_DOOR_DWF			5		// Door, Doom, Walkover, Fast
#define LSP_CEIL_W1			6 		// Ceiling Crusher, Walkover, 1-time only
#define LSP_CEIL_WR			7     // Ceiling Crusher, Walkover, repeatable
#define LSP_CEIL_S1			8 		// Ceiling Crusher, Switch, 1-time only
#define LSP_CEIL_SR			9     // Ceiling Crusher, Switch, repeatable
#define LSP_LIFT_WI			10		// Lift, Walkover, goes DOWN to lowest
											// adjacent floor INCLUDING itself
#define LSP_TELE_EXIT_1WAY	11		// Exit Level, 1-way line activation
#define LSP_LIFT_WA			12		// Lift, Walkover, goes UP to lowest
											// adjacent floor HIGHER than itself
#define LSP_LIFT_SI			13		// Lift, Switch, goes down to lowest
											// adjacent floor including itself
#define LSP_LIFT_SA			14		// Lift, Switch, goes up to lowest
											// adjacent floor including itself
#define LSP_LIFT_WIC			15		// like #10 but continuous (walkover)
#define LSP_LIFT_WAC       16		// like #12 but continuous (walkover)
#define LSP_LIFT_SIC       17		// like #10 but continuous (switch)
#define LSP_LIFT_SAC       18    // like #12 but continuous (switch)

#define LSP_SWDOOR_L			26		// Swinging Door, Hinges on Left
#define LSP_SWDOOR_R       27		// Swinging Door, Hinges on Right
#define LSP_DOOR_S			29		// Door, Doom, Switch-activated, Slow
#define LSP_DOOR_SF			30		// Door, Doom, Switch-activated, Fast
#define LSP_PIT				38		// Pit Trap, Walkover
#define LSP_TELE_1WAY		39		// Teleport, 1-way line activation
#define LSP_PIT_S				40		// Pit Trap, Switch
#define LSP_UNPIT				41		// Reverse Pit Trap, Walkover
#define LSP_UNPIT_S			42		// Reverse Pit Trap, Switch
#define LSP_TELE_EXIT_2WAY 52    // Exit Level, 2-way line activation
#define LSP_FLOOR_W1			56		// Floor Crusher, Walkover, 1-time only
#define LSP_FLOOR_WR			57    // Floor Crusher, Walkover, repeatable
#define LSP_FLOOR_S1			58		// Floor Crusher, Switch, 1-time only
#define LSP_FLOOR_SR			59    // Floor Crusher, Switch, repeatable

// Keyed Doors
#define LSP_KEY_SILVER		60
#define LSP_KEY_GOLD			61
#define LSP_KEY_RED			62
#define LSP_KEY_BLUE			63
#define LSP_KEY_GREEN		64
#define LSP_KEY_WHITE		65
#define LSP_KEY_SKELETON	66
// New keys 9-13-96
// [d1-06-97 JPC] The "purple key" became the "black key" at some point.
// Changed the text in STRDAT.TXT; left the other references to PURPLE as is.
#define LSP_KEY_PURPLE		67
#define LSP_KEY_BONE			68
#define LSP_KEY_STONE		69
//
#define LSP_SWDOOR_L_SILVER_KEY			70
#define LSP_SWDOOR_L_GOLD_KEY				71
#define LSP_SWDOOR_L_RED_RUBY_KEY		72
#define LSP_SWDOOR_L_BLUE_SAPPHIRE_KEY	73
#define LSP_SWDOOR_L_GREEN_EMERALD_KEY	74
#define LSP_SWDOOR_L_WHITE_DIAMOND_KEY	75
#define LSP_SWDOOR_L_SKELETON_KEY		76
#define LSP_SWDOOR_L_KEY_PURPLE			77
#define LSP_SWDOOR_L_KEY_BONE				78
#define LSP_SWDOOR_L_KEY_STONE			79
//
#define LSP_SWDOOR_R_SILVER_KEY			80
#define LSP_SWDOOR_R_GOLD_KEY				81
#define LSP_SWDOOR_R_RED_RUBY_KEY		82
#define LSP_SWDOOR_R_BLUE_SAPPHIRE_KEY	83
#define LSP_SWDOOR_R_GREEN_EMERALD_KEY	84
#define LSP_SWDOOR_R_WHITE_DIAMOND_KEY	85
#define LSP_SWDOOR_R_SKELETON_KEY		86
#define LSP_SWDOOR_R_KEY_PURPLE			87
#define LSP_SWDOOR_R_KEY_BONE				88
#define LSP_SWDOOR_R_KEY_STONE			89

#define LSP_TELE_2WAY				97		// Teleport, 2-way line activation
#define LSP_DOOR_DMF					118	// Door, Doom, Manual, Fast

#define LSP_GEM_BLUE					120
#define LSP_GEM_GREEN				121
#define LSP_GEM_OF_PASSAGE			122

#define LSP_SWDOOR_L_GEM_BLUE		130
#define LSP_SWDOOR_L_GEM_GREEN	131
#define LSP_SWDOOR_L_PASSAGE		132

#define LSP_SWDOOR_R_GEM_BLUE 	140
#define LSP_SWDOOR_R_GEM_GREEN	141
#define LSP_SWDOOR_R_PASSAGE		142

#define LSP_DOOR_INACCESSIBLE		150	// no room behind this door

// [d1-30-97 JPC] Added two line types for informational messages.
// Walkover-activated and switch-activated doors did not used to have
// anything special about their line segments.  Their sectors had the
// same tag as the distant line segment that activated the door, but
// their line segments had special = 0.  That meant the lines did not
// get put into the global variable "first_seg," and so activate_seg
// did nothing when you stood in front of them and pressed the spacebar.
// SO: for a walkover-activated or switch-activated door, set up the
// walkover or switch as before and also give the lines on both sides
// of the door one of the following special values.
#define LSP_DOOR_INFO_WALKOVER	151	// produces informational message
#define LSP_DOOR_INFO_SWITCH 		152	// produces informational message

// Defined 20 line types as dialog trip lines.
#define LSP_DIALOG_1			201
#define LSP_DIALOG_2			202
#define LSP_DIALOG_3			203
#define LSP_DIALOG_4			204
#define LSP_DIALOG_5			205
#define LSP_DIALOG_6			206
#define LSP_DIALOG_7			207
#define LSP_DIALOG_8			208
#define LSP_DIALOG_9			209
#define LSP_DIALOG_10			210
#define LSP_DIALOG_11			211
#define LSP_DIALOG_12			212
#define LSP_DIALOG_13			213
#define LSP_DIALOG_14			214
#define LSP_DIALOG_15			215
#define LSP_DIALOG_16			216
#define LSP_DIALOG_17			217
#define LSP_DIALOG_18			218
#define LSP_DIALOG_19			219
#define LSP_DIALOG_20 			220
#define LSP_DIALOG_FIRST		LSP_DIALOG_1
#define LSP_DIALOG_LAST	  		LSP_DIALOG_20

// Note: DCK Seem's to only allow the numbers 1 -> 255 for line tags even
//       though the data is stored as a SHORT.

// Sector specials.
#define SSP_NORMAL			0		// nothing special
#define SSP_LIGHTNING		2 		// Lightning flashes (CONVERT.C: sector_to_light)
#define SSP_BLINK	 			3 		// Blinking light    (CONVERT.C: sector_to_light)
#define SSP_DAMAGE_2			5		// Damage of 2
#define SSP_DAMAGE_5			7     // Damage of 5
#define SSP_DAMAGE_10		9     // Damage of 10
#define SSP_CLIMB_OK			16		// Sector allows climbing
#define SSP_WATER				60		// Water
#define SSP_DEEP_WATER		61		// Deep Water
#define SSP_LAVA           62		// Lava
#define SSP_MOUNTAIN			63		// Mountain
#define SSP_FOREST			64		// Forest
#define SSP_POISON_GAS		65		// Poison Gas
#define SSP_ACID_FLOOR		66		// Acid Floor

#endif


