/* ========================================================================
   Copyright (c) 1990,1997	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _MAPICON_HXX
#define _MAPICON_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
//----------------------------------
//	Map Icons
//----------------------------------
	typedef enum {
		NO_MAP_ICON = 0,
		ANY_MAP_ICON = 0,
		A_INF_UNIT_ICON,
		A_EIN_UNIT_ICON,
		A_BOW_UNIT_ICON,
		A_IRR_UNIT_ICON,
		A_PIK_UNIT_ICON,
		A_LVY_UNIT_ICON,
		A_SCT_UNIT_ICON,
		A_KNT_UNIT_ICON,
		A_CAV_UNIT_ICON,
		M_INF_UNIT_ICON,
		M_BOW_UNIT_ICON,
		M_IRR_UNIT_ICON,
		M_PIK_UNIT_ICON,
		M_CAV_UNIT_ICON,
		E_INF_UNIT_ICON,
		E_BOW_UNIT_ICON,
		E_CAV_UNIT_ICON,
		D_INF_UNIT_ICON,
		D_BOW_UNIT_ICON,
		G_INF_UNIT_ICON,
		G_BOW_UNIT_ICON,
		G_CAV_UNIT_ICON,
		N_INF_UNIT_ICON,
		N_IRR_UNIT_ICON,
		C_HPY_UNIT_ICON,
		C_SKL_UNIT_ICON,
		C_SPD_UNIT_ICON,
		C_HEL_UNIT_ICON,
		C_WYV_UNIT_ICON,

		REG_LORDM1_UNIT_ICON,
		REG_LORDM2_UNIT_ICON,
		REG_LORDM3_UNIT_ICON,
		REG_LORDM4_UNIT_ICON,
		REG_LORDM5_UNIT_ICON,
		REG_LORDM6_UNIT_ICON,
		REG_LORDM7_UNIT_ICON,
		REG_WIZDM1_UNIT_ICON,
		REG_WIZDM2_UNIT_ICON,
		REG_WIZDM3_UNIT_ICON,
		REG_WIZDM4_UNIT_ICON,
		REG_GILDM1_UNIT_ICON,
		REG_GILDM2_UNIT_ICON,
		REG_PRSTM1_UNIT_ICON,
		REG_PRSTM2_UNIT_ICON,
		REG_LORDF1_UNIT_ICON,
		REG_LORDF2_UNIT_ICON,
		REG_LORDF3_UNIT_ICON,
		REG_WIZDF1_UNIT_ICON,
		REG_WIZDF2_UNIT_ICON,
		REG_GILDF1_UNIT_ICON,
		REG_PRSTF1_UNIT_ICON,
		REG_ELORD1_UNIT_ICON,
		REG_ELORD2_UNIT_ICON,
		REG_ELADY1_UNIT_ICON,
		REG_DLORD1_UNIT_ICON,
		REG_DLORD2_UNIT_ICON,
		REG_DLORD3_UNIT_ICON,
		REG_GLORD1_UNIT_ICON,
		REG_GLADY1_UNIT_ICON,
		GORGON_UNIT_ICON,
		SPIDERKING_UNIT_ICON,
		RHUOBHE_UNIT_ICON,

		LET_LORDM1_UNIT_ICON,
		LET_LORDM2_UNIT_ICON,
		LET_LORDM3_UNIT_ICON,
		LET_LORDM4_UNIT_ICON,
		LET_LORDM5_UNIT_ICON,
		LET_LORDM6_UNIT_ICON,
		LET_LORDM7_UNIT_ICON,
		LET_WIZDM1_UNIT_ICON,
		LET_WIZDM2_UNIT_ICON,
		LET_WIZDM3_UNIT_ICON,
		LET_WIZDM4_UNIT_ICON,
		LET_GILDM1_UNIT_ICON,
		LET_GILDM2_UNIT_ICON,
		LET_PRSTM1_UNIT_ICON,
		LET_PRSTM2_UNIT_ICON,
		LET_LORDF1_UNIT_ICON,
		LET_LORDF2_UNIT_ICON,
		LET_LORDF3_UNIT_ICON,
		LET_WIZDF1_UNIT_ICON,
		LET_WIZDF2_UNIT_ICON,
		LET_GILDF1_UNIT_ICON,
		LET_PRSTF1_UNIT_ICON,
		LET_ELORD1_UNIT_ICON,
		LET_ELORD2_UNIT_ICON,
		LET_ELADY1_UNIT_ICON,
		LET_DLORD1_UNIT_ICON,
		LET_DLORD2_UNIT_ICON,
		LET_DLORD3_UNIT_ICON,
		LET_GLORD1_UNIT_ICON,
		LET_GLADY1_UNIT_ICON,

		VILLAGE_ICON,
		SEAPORT_ICON,
		SEAPORT2_ICON,
		PORT_ICON,
		WIZTOWER_ICON,
		RUINS_ICON,
		LAW1_ICON,
		LAW2_ICON,
		LAW3_ICON,
		LAW4_ICON,
		TEMPLE1_ICON,
		TEMPLE2_ICON,
		TEMPLE3_ICON,
		TEMPLE4_ICON,
		GUILD1_ICON,
		GUILD2_ICON,
		GUILD3_ICON,
		GUILD4_ICON,
		SOURCE1_ICON,
		SOURCE2_ICON,
		SOURCE3_ICON,
		SOURCE4_ICON,
		CASTLE1_ICON,
		CASTLE2_ICON,
		CASTLE3_ICON,
		CASTLE4_ICON,
		SPECIAL1_ICON,
		SPECIAL2_ICON,
		SPECIAL3_ICON,

		MAP_ICON_COUNT
	} MAP_ICON;

#define REG_M_FTR_UNIT_ICON		(REG_LORDM1_UNIT_ICON)
#define REG_M_WIZ_UNIT_ICON		(REG_WIZDM1_UNIT_ICON)
#define REG_M_PRS_UNIT_ICON		(REG_PRSTM1_UNIT_ICON)
#define REG_M_THF_UNIT_ICON		(REG_GILDM1_UNIT_ICON)
#define REG_M_PAL_UNIT_ICON		(REG_LORDM4_UNIT_ICON)
#define REG_M_RNG_UNIT_ICON		(REG_LORDM2_UNIT_ICON)
#define REG_F_FTR_UNIT_ICON		(REG_LORDF1_UNIT_ICON)
#define REG_F_WIZ_UNIT_ICON		(REG_WIZDF1_UNIT_ICON)
#define REG_F_PRS_UNIT_ICON		(REG_PRSTF1_UNIT_ICON)
#define REG_F_THF_UNIT_ICON		(REG_GILDF1_UNIT_ICON)
#define REG_F_PAL_UNIT_ICON		(REG_LORDF1_UNIT_ICON)
#define REG_F_BRD_UNIT_ICON		(REG_ELADY1_UNIT_ICON)
#define REG_ELF_M_FTR_UNIT_ICON	(REG_ELORD1_UNIT_ICON)
#define REG_ELF_F_WIZ_UNIT_ICON	(REG_ELADY1_UNIT_ICON)
#define REG_DWF_M_FTR_UNIT_ICON	(REG_DLORD1_UNIT_ICON)
#define REG_DWF_M_PRS_UNIT_ICON	(REG_DLORD1_UNIT_ICON)
#define REG_GOB_FTR_UNIT_ICON		(REG_GLORD1_UNIT_ICON)
#define M_FIGHTER_UNIT_ICON		(LET_LORDM1_UNIT_ICON)
#define M_WIZARD_UNIT_ICON			(LET_WIZDM1_UNIT_ICON)
#define M_PRIEST_UNIT_ICON			(LET_PRSTM1_UNIT_ICON)
#define M_THIEF_UNIT_ICON			(LET_GILDM1_UNIT_ICON)
#define M_PALADIN_UNIT_ICON		(LET_LORDM4_UNIT_ICON)
#define M_RANGER_UNIT_ICON			(LET_LORDM2_UNIT_ICON)
#define F_FIGHTER_UNIT_ICON		(LET_LORDF1_UNIT_ICON)
#define F_WIZARD_UNIT_ICON			(LET_WIZDF1_UNIT_ICON)
#define F_PRIEST_UNIT_ICON			(LET_PRSTF1_UNIT_ICON)
#define F_THIEF_UNIT_ICON			(LET_GILDF1_UNIT_ICON)
#define F_PALADIN_UNIT_ICON		(LET_LORDF1_UNIT_ICON)
#define F_BARD_UNIT_ICON			(LET_ELADY1_UNIT_ICON)
#define ELF_M_FTR_UNIT_ICON		(LET_ELORD1_UNIT_ICON)
#define ELF_F_WIZ_UNIT_ICON		(LET_ELADY1_UNIT_ICON)
#define DWF_M_FTR_UNIT_ICON		(LET_DLORD1_UNIT_ICON)
#define DWF_M_PRS_UNIT_ICON		(LET_DLORD1_UNIT_ICON)
#define GOB_FTR_UNIT_ICON			(LET_GLORD1_UNIT_ICON)

#define M_OFF_UNIT_ICON				(LET_LORDM3_UNIT_ICON)
#define D_OFF_UNIT_ICON				(LET_DLORD1_UNIT_ICON)
#define E_OFF_UNIT_ICON				(LET_ELORD1_UNIT_ICON)
#define REGENT_UNIT_ICON			(REG_LORDM1_UNIT_ICON)
#define A_OFF_UNIT_ICON				(LET_LORDM3_UNIT_ICON)

#define FIRST_CHARACTER_TYPE		(REG_LORDM1_UNIT_ICON)
#define LAST_CHARACTER_TYPE		(VILLAGE_ICON-1)

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Class Definition
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Inline functions   
   ------------------------------------------------------------------------ */
   
#endif // _MAPICON_HXX
