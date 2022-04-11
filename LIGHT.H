// ===========================================================================
// Copyright (c) 1996 by Synergistic Software. All Rights Reserved.
// LIGHT.H
// [d6-18-96 JPC]
// ===========================================================================

#if !defined(_LIGHT_H_)
#define _LIGHT_H_

#if defined (__cplusplus)
extern "C" {
#endif

#define LIGHT_BITS				3
#define SIDE_1_LIGHT_SHIFT		(16 - LIGHT_BITS)
#define SIDE_2_LIGHT_SHIFT    (16 - LIGHT_BITS * 2)
#define LIGHT_VALUE_MASK		7
#define LIGHT_SIDE_1_MASK		(0xFFFF - (LIGHT_VALUE_MASK << SIDE_1_LIGHT_SHIFT))
#define LIGHT_SIDE_2_MASK		(0xFFFF - (LIGHT_VALUE_MASK << SIDE_2_LIGHT_SHIFT))
#define LIGHT_MAX_POSITIVE		3
#define LIGHT_MIN_NEGATIVE    -4
#define LIGHT_ADD_FACTOR		8
#define HEATSOURCE_ADDFACTOR	16

// GWP trying new infravision values.
#define INFRAVISION_THRESHHOLD	16		// [d12-13-96 JPC] no infravision
													// effect below this level
	  												// (the lower the light number,
													// the brighter it is)

// #define INFRAVISION_THRESHHOLD	-1		// [d5-01-97 JPC] restore more dramatic
													// infravision effect per CW RCC

// Shade table and color remap defines.
// There are 32 degrees of shading for any given color.  That's why there
// are 32 base tables, 32 infravision tables, and 32 tables for every
// color remap.
// There are also 3 "smoked glass" tables.
#define cBASE_TABLES				35		// 32 (regular palette) + 3 ("smoked glass" palettes)

#define cMISC_TABLES				3		// infravision, grayscale, and wraith

// Infravision tables start immediately after cBASE_TABLES.
#define INFRAVISION_OFFSET			cBASE_TABLES		
#define INFRAVISION_MAX				(INFRAVISION_OFFSET + 31)

// Color remap tables start immediately after the infravision tables.
// To use a color remap table, set the .ColorRemap field of the mythings[]
// element to some number between 0 and cREMAP_GROUPS-1.  The code for
// setting the remap is in THINGS.C: draw_thing.
#define REMAP_TABLE_OFFSET		(INFRAVISION_OFFSET + 32)
#define cREMAP_GROUPS 			35		// [9/19 ABC] changed to match cnt of realms
												// (each group has 32 shade tables)

#define MAX_cREMAP_TABLE		(cREMAP_GROUPS + 2)	// plus 2 for the gray scale table
																	// and the wraith table; this works
																	// because we do a MOD rather
																	// than a compare in athing.hxx
																	// (ArtThing::mfRemapColor)

// Defines to help build the remap tables in CUE.C.
#define cREMAP_PALETTEENTRIES	24		// total remapped colors in each range
#define REMAP_RANGE_1			56		// index to first color in remap range 1
#define REMAP_RANGE_2			80    // index to first color in remap range 2
#define CR_GRAY  					8
#define CR_VIOLET					32
#define CR_BLUE					56
#define CR_PURPLE					80
#define CR_RED						104
#define CR_ORANGE					128
#define CR_GOLD					152
#define CR_GREEN					176
#define CR_BROWN					200

#define BLACK_GRAD				-7
#define BLUE_GRAD					56
#define PURPLE_GRAD				80
#define RED_GRAD					104
#define GREEN_GRAD				176
#define ORANGE_GRAD				128
#define COPPER_GRAD				224
#define GOLD_GRAD					152
#define TAN_GRAD					200
#define STEEL_GRAD				32
#define SILVER_GRAD				1
#define WHITE_GRAD				8

// Grayscale tables start immediately after color remap tables.
// Set the .ColorRemap field of the mythings[] element to GRAYSCALE_REMAP
// to get it to go in grayscale.
#define GRAYSCALE_TABLE_OFFSET	(INFRAVISION_OFFSET + 32 + 32 * cREMAP_GROUPS)
#define GRAYSCALE_MAX				(GRAYSCALE_TABLE_OFFSET + 31)
#define GRAYSCALE_REMAP				cREMAP_GROUPS

// Wraith tables start immediately after grayscale tables.
// Set the .ColorRemap field of the mythings[] element to WRAITH_REMAP
// to get it to use the wraith tables.
#define WRAITH_TABLE_OFFSET 		(GRAYSCALE_TABLE_OFFSET + 32)
#define WRAITH_MAX					(WRAITH_TABLE_OFFSET + 31)
#define WRAITH_REMAP					(GRAYSCALE_REMAP + 1)

// Globals for testing lighting effects.
extern BOOL gfTestMagicLightSwitch; // defined in TEXTURES.C
extern BOOL gfInfraVision;          // defined in TEXTURES.C
#ifdef _DEBUG
extern BOOL gfTestGrayLight;				// [d9-05-96 JPC]
#endif
extern LONG gcMagicTemporaryLight;  // defined in TEXTURES.C
extern LONG gcFrames;               // defined in TEXTURES.C


// Prototypes for lighting functions in various files.
void TextureLightTest (LONG arg1, LONG arg2);
void TextureFrameHandler (LONG arg);

// void RenderChangeLight (LONG * light, BOOL fHeatSource);
// Convert function into TWO macros, one that allows for heat sources,
// one that does not.
#define RenderChangeLight(light)							\
	if (gfTestMagicLightSwitch)							\
      light -= 16;											\
	if (gcMagicTemporaryLight > 0)						\
		light -= gcMagicTemporaryLight/10;				\
	if (light < 0)												\
		light = 0;												\
	if (gfInfraVision && light > INFRAVISION_THRESHHOLD)										\
		light += INFRAVISION_OFFSET;

/*#define RenderChangeLightHeat(light, fHeatSource)	\
	if (gfTestMagicLightSwitch)							\
      light -= 16;											\
	if (gcMagicTemporaryLight > 0)						\
		light -= gcMagicTemporaryLight/10;				\
	if (light < 0)												\
		light = 0;												\
	if (gfInfraVision && light > INFRAVISION_THRESHHOLD)			\
	{																\
		if (fHeatSource)										\
		{															\	
			light += (INFRAVISION_OFFSET - HEATSOURCE_ADDFACTOR);	\
			if (light < INFRAVISION_OFFSET)				\
				light = INFRAVISION_OFFSET;				\
		}															\	
		else														\
		{															\
			light += INFRAVISION_OFFSET;					\
		}															\
	}*/

inline LONG RenderChangeLightHeat(LONG light, BOOL fHeatSource)
{
	if (gfTestMagicLightSwitch)
		light -= 16;
	if (gcMagicTemporaryLight > 0)
		light -= gcMagicTemporaryLight / 10;
	if (light < 0)
		light = 0;
	if (gfInfraVision && light > INFRAVISION_THRESHHOLD)
	{
		if (fHeatSource)
		{
			light += (INFRAVISION_OFFSET - HEATSOURCE_ADDFACTOR);
			if (light < INFRAVISION_OFFSET)
				light = INFRAVISION_OFFSET;
		}
		else
		{
			light += INFRAVISION_OFFSET;
		}
	}
	return light;
}

#if defined (__cplusplus)
}
#endif

#endif

