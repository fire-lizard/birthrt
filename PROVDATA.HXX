/* ========================================================================
   Copyright (c) 1990,1997	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _PROVDATA_HXX
#define _PROVDATA_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
//----------------------------------
//	Terrain types
//----------------------------------
	typedef enum {
		NO_TERRAIN = 0,
		FOREST,
		PLAINS,
		MOUNTN,
		HVY_MNT,
		SWAMP,
		HILLS,
		RIVER,
		RIVER_ROAD,
		PLAINS_ROAD,
		TERRAIN_COUNT
	} TERRAIN_ENUM;


/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
//----------------------------------
//	Provinces
//----------------------------------
	typedef enum {
		ANY_PROVINCE = 0,
		NO_PROVINCE = 0,
		FIRST_PROVINCE = 1,

		/*----- ROESONE ----------*/
		ABBATUOR = 1, 
		BELLAM, 
		CAERCAS, 
		DUERLIN, 
		EDLIN, 
		FAIRFIELD, 
		GHORIED, 
		/*----- AERENWE ----------*/
		BANIENS_DEEP, 
		CALRIE, 
		DHOENEL, 
		HALRIED, 
		NORTHVALE, 
		SHADOWGREEN, 
		WESTMARCH, 
		/*----- DIEMED ----------*/
		AERELE, 
		BLIENE, 
		CILIENE, 
		DUENE, 
		MOERE, 
		TIER, 
		/*----- ILIEN ----------*/
		ILIEN, 
		/*----- MEDOERE ----------*/
		ALAMIER, 
		BRAEME, 
		CAERWIL, 
		/*----- SPIDERFELL ----------*/
		SPIDERFELL, 
		/*----- BOERUINE ----------*/
		BACAELE, 
		CALANT, 
		DHALAESE, 
		FHORUILE, 
		NIETIER, 
		REDOUBT, 
		RIVIEN, 
		SEASEDGE, 
		TARIENE, 
		/*----- TALINIE ----------*/
		FREESTEAD, 
		GREENSWARD, 
		ICE_HAVEN, 
		LINDHOLME, 
		SEAPORT, 
		SERIMSET, 
		WINTERS_DEEP, 
		/*----- BROSENGAE ----------*/
		BINDIER, 
		COERE, 
		MARILEN, 
		/*----- RHUOBHE ----------*/
		RHUOBHE, 
		/*----- TAEGHAS ----------*/
		BAYSIDE, 
		BHAINE, 
		BROSIEN, 
		ISLIEN, 
		PORTAGE, 
		SEAMIST, 
		SEASDEEP, 
		WILDERS_GORGE, 
		/*----- AVANIL ----------*/
		ANUIRE, 
		AVARIEN, 
		BHREIN, 
		CAULNOR, 
		DAULTON, 
		DURIENE, 
		NENTRIL, 
		TALIERN, 
		VANILEN, 
		/*----- GHOERE ----------*/
		ACHIESE, 
		BHALAENE, 
		BHELINE, 
		CONALLIER, 
		DANAROENE, 
		GHIERE, 
		RHUMANNEN, 
		THORALINAR, 
		TIRESTE, 
		TORNILEN, 
		/*----- MHORIED ----------*/
		BALTERUINE, 
		BEVALDRUOR, 
		BYRNNOR, 
		CWLLDON, 
		DHALSIEL, 
		MAESILAR, 
		MARLOERS_GAP, 
		TENARIEN, 
		TORIENS_WATCH, 
		WINOENE, 
		/*----- TUORNEN ----------*/
		ALAMSREFT, 
		ELEVESNEMIERE, 
		GHONALLISON, 
		HAESRIEN, 
		MONSEDGE, 
		NABHRIENE, 
		PECHALINN, 
		TUORS_HOLD, 
		/*----- ALAMIE ----------*/
		ALAROINE, 
		DESEIRAIN, 
		HILDON, 
		LARAETH, 
		MAESFORD, 
		NORTMOOR, 
		SORELIES, 
		SOUTMOOR, 
		TRAIWARD, 
		/*----- IMPERIAL CITY OF ANUIRE ----------*/
		CITY_OF_ANUIRE, 
		/*----- ELINIE ----------*/
		ANSIEN, 
		CHALSEDON, 
		HOPES_DEMISE, 
		MHOLIEN, 
		OSOERIENE, 
		SENDOURAS, 
		SOILEITE, 
		/*----- ENDIER ----------*/
		ENDIER, 
		/*----- DHOESONE ----------*/
		BJONDRIG, 
		DHARILEIN, 
		GIANTS_FASTNESS, 
		HIDAELE, 
		NOLIEN, 
		RIVESIDE, 
		ROMIENE, 
		RUIDEWASH, 
		SIDHUIRE, 
		SONIELE, 
		SONNELIND, 
		TRADEBHEIN, 
		/*----- GORGONS CROWN, THE ----------*/
		ABATTOIR, 
		ANATHAR, 
		ELFSEYES, 
		JOGH_WARREN, 
		KAL_SAITHARAK, 
		METTLE, 
		MOTILE, 
		MUTIANS_POINT, 
		OROGS_HEAD, 
		PELT, 
		PLUMBAGO, 
		SAGES_FEN, 
		SERES_HOLD, 
		SIDEATH, 
		STONES_END, 
		SUNDER_FALLS, 
		ZAPTIG, 
		/*----- TUARHIEVEL ----------*/
		AVALLAIGH, 
		BHINDRAITH, 
		BRAETHINDYR, 
		CWMBHEIR, 
		CYMRYR, 
		DHONEAGHMIERE, 
		LLYRANDOR, 
		/*----- CARIELE ----------*/
		MHELLIVIENE, 
		MOUNTAINSEDGE, 
		RIVERFORD, 
		/*----- FIVE PEAKS, THE ----------*/
		GORGE, 
		FLOODSPAETH, 
		HELMSHAVEN, 
		PUINOL, 
		SUFHANIE, 
		THASBYRN, 
		TORAIN, 
		/*----- MARKAZOR ----------*/
		BRUSHFIRE, 
		DWARFS_HOLD, 
		ELFSDEMISE, 
		PERILTREES, 
		RIVERSPRING, 
		SHATTERED_HILLS, 
		SUTREN_HILLS, 
		/*----- MUR-KILAD ----------*/
		CRUSHING_ROCK, 
		FALLEN_ROCK, 
		/*----- THURAZOR ----------*/
		BLOODBAY, 
		CRUSHING_HILLS, 
		DOOMS_PEAK, 
		FALLING_TIMBER, 
		MERGARROTE, 
		STORMS_RELEASE, 
		/*----- BARUK-AZHIK ----------*/
		BRANS_RETREAT, 
		CLIFFS_LAMENT, 
		LANDS_VICTORY, 
		PROMONTORY, 
		RIVENROCK, 
		/*----- CHIMAERON, THE ----------*/
		BARNIERE, 
		CAREINE, 
		HAMEIN, 
		LYSSAN, 
		MHOWE, 
		RUORKHE, 
		SALVIENE, 
		/*----- COERANYS ----------*/
		BOGSEND, 
		CAUDRAIGHT, 
		DEEPSHADOW, 
		DUORNIL, 
		MISTIL, 
		RANIEN, 
		RUORVEN, 
		/*----- OSOERDE ----------*/
		ALGAEL, 
		BROTHENDAR, 
		GHALLIERE, 
		GULFPORT, 
		MOERGEN, 
		MORIEL, 
		SPIRITSEND, 
		SUNKEN_LANDS, 
		/*----- SIELWODE, THE ----------*/
		ANNYDWR, 
		CU_HAELLYRD, 
		FHYLALLIEN, 
		GHYLLWN, 
		HOEHNAEN, 
		ISEARE, 
		LLEWHOELLEN, 
		TUAR_LLYRIEN, 
		YWRNDOR, 

		LOST_REALM,

		NO_PROVINCE2
	} PROVINCE;

#define PROVINCE_COUNT	NO_PROVINCE2
#define MAX_PROVS			PROVINCE_COUNT

//----------------------------------
//	Borders
//----------------------------------
typedef struct {
	UBYTE					Province;
	UBYTE					MoveCost;
} BORDER_TYPE;

//----------------------------------
//	Provinces
//----------------------------------
typedef struct {
	CSTRPTR				name;
	SHORT					x, y;						// x,y on the map
	unsigned int		Realm			: 7;
	unsigned int		CivLevel		: 4;
	unsigned int		CurCivLevel	: 4;
	unsigned int		MagLevel		: 4;
	unsigned int		TaxLevel		: 2;		// 32 bits filled
	unsigned int		Loyalty		: 2;
	unsigned int		Contested	: 1;
	unsigned int		OccupRealm	: 7;
	unsigned int		Pillaged		: 1;

	unsigned int		ReduceHolds	: 1;
	unsigned int		Blessed		: 2;
	unsigned int		Blighted		: 2;
	unsigned int		Honest		: 2;
	unsigned int		Warded		: 2;
	unsigned int		Dispel		: 3;
	unsigned int		DispelLvl	: 6;
	unsigned int		LawClaims	: 1;		// 32 bits filled
	unsigned int		CastleSptd	: 1;
	unsigned int		HumanPop		: 1;
	unsigned int		ElfPop		: 1;
	unsigned int		DwarfPop		: 1;
	unsigned int		GoblinPop	: 1;
	unsigned int		LevyMustered: 4;
	unsigned int		Terrain     : 4;

	BORDER_TYPE			Border[7];
	SHORT					FirstPlace;
} PROV_TYPE, *PROV_PTR;

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
   
#endif // _PROVDATA_HXX
