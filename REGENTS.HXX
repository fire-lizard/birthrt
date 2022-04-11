/* ========================================================================
   Copyright (c) 1990,1997	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _REGENTS_HXX
#define _REGENTS_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_UNITS_HXX)
#include "units.hxx"
#endif

#if !defined(_REALM_HXX)
#include "realm.hxx"
#endif

#if !defined(_MAPICON_HXX)
#include "mapicon.hxx"
#endif


/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
// there are more characters than just regents...
#define CHARACTER_COUNT 117

/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
//----------------------------------
//	Regents
//----------------------------------
	typedef enum {
			NO_REGENT = 0,
/*01*/	LILIENE_SWORDWAITH,
/*02*/	COLE_ALWIER,
/*03*/	CARILON_ALAM,
/*04*/	DIERDREN_ALAM,
/*05*/	CALIEDHE_DOSIERE,
/*06*/	UNKNOWN1,
/*07*/	DARIEN_AVAN,
/*08*/	DHERAENE_BHAILIE,
/*09*/	GRIMM_GRAYBEARD,
/*10*/	DIIRK_WATERSHOLD,
/*11*/	AERIC_BOERUINE,
/*12*/	INNES,
/*13*/	ERIENE_MIERELEN,
/*14*/	HYDE_TERMONIE,
/*15*/	ENTIER_GLADANIL,
/*16*/	MHEALLIE_BIREON,
/*17*/	DANITA_CHIMAERA,
/*18*/	DENERIK,
/*19*/	ELUVIE_CARIELE,
/*20*/	MEDHLORIE_HAENSEN,
/*21*/	FHIELE_DHOESONE,
/*22*/	CLUMINE_DHOESONE,
/*23*/	HEIRL_DIEM,
/*24*/	LASICA_DIEM,
/*25*/	ASSAN_IBN_DAOUTA,
/*26*/	KALILAH_BINT_DAOUDA,
/*27*/	GUILDER_KALIAN,
/*28*/	CAINE,
/*29*/	THE_WIZARD,
/*30*/	UNKNOWN9,
/*31*/	GAVIN_TAEL,
/*32*/	THE_SWORD_MAGE,
/*33*/	THE_GORGON,
/*34*/	KIRAS_EARTHCORE,
/*35*/	ROGR_AGLONDIER,
/*36*/	ALLIENE_AGLONDIER,
/*37*/	RAZZIK_FANGGRABBER,
/*38*/	UNKNOWN2,
/*39*/	SURIS_ENLIEN,
/*40*/	KOTRIN_SKIRVIN,
/*41*/	DAERIC_MHORIED,
/*42*/	MICHAEL_MHORIED,
/*43*/	UNKNOWN3,
/*44*/	UNKNOWN4,
/*45*/	UNKNOWN5,
/*46*/	UNKNOWN6,
/*47*/	GODAR_THURINSON,
/*48*/	UNKNOWN7,
/*49*/	JAISON_RAENECH,
/*50*/	TERENCE_GRYPHON,
/*51*/	RHUOBHE_THE_ELF,
/*52*/	NHOUN,
/*53*/	MARLAE_ROESONE,
/*54*/	MICHAEL_AGNELIE,
/*55*/	QUEEN_ISELIE,
/*56*/	CORWIN_RHYSDIORDAD,
/*57*/	THE_SPIDER,
/*58*/	UNKNOWN8,
/*59*/	HARALD_KHORIEN,
/*60*/	TREVOR_ONWEN,
/*61*/	THURIENE_DONALS,
/*62*/	TORELE_ANVIRAS,
/*63*/	TIESKAR,
/*64*/	KRAL_TWO_TOES,
/*65*/	FHILERAENE,
/*66*/	LLYTHA_DAMAAN,
/*67*/	LAELA_FLAERTES,
/*68*/	BRAEDONNAL_TUARE,

/*69*/	ADAERE_DONEIM,				// REALM::N_IMPORTS_EXPORTS
/*70*/	AELIES,						// REALM::HIGH_MAGE_AELIES
/*71*/	MARICOERE,					// REALM::HAELYNS_AEGIS
/*72*/	INIERE,						// REALM::AVANIL
/*73*/	BORTHEIN,					// REALM::BOERUINE
/*74*/	EL_HADID,					// REALM::PORT_OF_CALL_EXCHANGE
/*75*/	ELAMIEN_LAMIER,			// REALM::POINTS_EAST_TRADING_GUILD
/*76*/	EYELESS_ONE,				// REALM::WIZARD_EYELESS_ONE
/*77*/	THE_SWORD,					// REALM::MILITANT_ORDER_OF_CUIRAECEN
/*78*/	GAELIN_THURIED,			// REALM::UPPER_ANUIRE_TRADERS
/*79*/	HIRIELE,						// REALM::GHOERE
/*80*/	HERMEDHIE,					// REALM::WIZARD_HERMEDHIE
/*81*/	ARMIENDIN,					// REALM::IMPREGNABLE_HEART_OF_HAELYN
/*82*/	LARRA_NIELEMS,				// REALM::N_REFORMED_CHURCH_OF_SARIMIE
/*83*/	BRIESEN,						// REALM::ORTHODOX_IMPERIAL_TEMPLE_OF_HAELYN
/*84*/	BACCAERE,					// REALM::HIDDEN_TEMPLE_OF_CUIRAECEN
/*85*/	LANNAMAN,					// REALM::MHORIED
/*86*/	ORTHIEN_TANE,				// REALM::S_ANUIRE_SHIPPING_AND_IMPORTS
/*87*/	REGIEN,						// REALM::MHORIED
/*88*/	NICHALEIR,					// REALM::AVANIL
/*89*/	ROCKHAMMER,					// REALM::RUARCH_ROCKHAMMER
/*90*/	THE_SWAMP_MAGE,			// REALM::WIZARD_SECOND_SWAMP_MAGE
/*91*/	TEMIAS_COUMAIN,			// REALM::CELESTIAL_JEWEL_OF_SARIMIE
/*92*/	TORIAS_GRIENE,				// REALM::TALINIE

			REGENT_COUNT
	} REGENT_ENUM;

//----------------------------------
//	Race, Class, and Blood
//----------------------------------
	typedef enum {
		DWARF = 0,
		ELF,
		HALFELF,
		HALFLING,
		ANUIREAN,
		BRECHT,
		KHINASI,
		RJURIK,
		VOS,
		GOBLIN,
		GNOLLS,
		AWNSHEGH,
	} RACE_ENUM;

	typedef enum {
		NO_CLASS = 0,
		FIGHTER,
		PALADIN,
		RANGER,
		PRIEST,
		WIZARD,
		THIEF,
		BARD
	} CLASS_ENUM;

	typedef enum {
		TAINTED = 0,
		MINOR,
		MAJOR,
		GREAT,
		TRUEBL
	} BLOOD_GRADE;

	typedef enum {
		ANDUIRAS = 0,
		REYNIR,
		BRENNA,
		BASAIA,
		MASELA,
		VORYNN,
		AZRAI
	} BLOOD_DERIV;

/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Class Definition
   ------------------------------------------------------------------------ */

//----------------------------------
//	Regents
//----------------------------------
class REGENT {
private:
	// Helper class to hide the data yet let the complier generate a default
	// constructor. (Saves about 1/2 the data space over one I have to specify.)
	class REGENT_DATA {
	public:
		// [d4-23-97 JPC] Removed the following so users can edit the names
		// of characters.  Name is now stored in PLAYER_STATS class, szNameStat.
		// CSTRPTR				name;
		unsigned int		Realm		: 7;
		unsigned int		Race		: 4;
		unsigned int		Class1		: 4;
		unsigned int		Level1		: 6;
		unsigned int		Class2		: 4;
		unsigned int		Level2		: 6;
		unsigned int		BL_grade	: 3;
		unsigned int		BL_deriv	: 3;
		unsigned int		BL_strength	: 8;
		SHORT				unit;
		SHORT               id;
		SHORT				icon;
	};
		
public:
	REGENT_DATA	fRegentData;
	
	// Accessors
	char const * const mfGetname() const;
	inline REALM::REALM_TYPE const mfGetRealm() const;
	inline REALM::REALM_TYPE const mfGetOriginalRealm() const;
	inline RACE_ENUM mfGetRace() const;
	
	inline CLASS_ENUM mfGetClass1() const;
	inline unsigned int mfGetLevel1() const;
	
	inline CLASS_ENUM mfGetClass2() const;
	inline unsigned int mfGetLevel2() const;
	
	inline BLOOD_GRADE mfGetBL_grade() const;
	inline BLOOD_DERIV mfGetBL_deriv() const;
	inline unsigned int mfGetBL_strength() const;
	
	inline SHORT mfGetunit() const;
	inline SHORT mfGetid() const;
	inline MAP_ICON mfGeticon() const;
	
	
	// Modifiers
	inline void mfSetRealm(REALM::REALM_TYPE const);
	inline void mfSetRace(RACE_ENUM const);
	
	inline void mfSetClass1(CLASS_ENUM const);
	inline void mfSetLevel1(unsigned int const);
	
	inline void mfIncLevel1(LONG const);
	
	inline void mfSetClass2(CLASS_ENUM const);
	inline void mfSetLevel2(unsigned int const);
	
	inline void mfIncLevel2(LONG const);
	
	inline void mfSetBL_grade(BLOOD_GRADE const);
	inline void mfSetBL_deriv(BLOOD_DERIV const);
	inline void mfSetBL_strength(unsigned int const);
	
	inline unsigned int mfIncrBL_strength();
	
	inline void mfSetunit(SHORT const);
	inline void mfSetid(SHORT const);
	inline void mfSeticon(MAP_ICON const);
	
protected:
private:
};

typedef REGENT *REGENT_PTR;
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
extern DECL_VECTOR_CLASS(REGENT,regents);
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

void TransferRegentIconsToUnitsArray(void);
SHORT RegentUnitOfRealm(REALM::REALM_TYPE RealmId);
void LinkUnits( BOOL fSend );
BOOL IsRegent(LONG UnitIndex);
BOOL IsLieutenant (LONG UnitIndex);
BOOL IsRegentOrLieutenant (LONG UnitIndex);

/* ------------------------------------------------------------------------
   Inline functions
   ------------------------------------------------------------------------ */


// if there is a unit associated with this regent, use that
// realm info. Otherwise, return the original realm data.
inline REALM::REALM_TYPE const REGENT::mfGetRealm() const
{
	if (fRegentData.unit <= 0)
		return (REALM::REALM_TYPE)fRegentData.Realm;
		
	return (REALM::REALM_TYPE)units[fRegentData.unit].Realm;
}


inline REALM::REALM_TYPE const REGENT::mfGetOriginalRealm() const
{
	return (REALM::REALM_TYPE)fRegentData.Realm;
}

inline RACE_ENUM REGENT::mfGetRace() const
{
	return (RACE_ENUM) fRegentData.Race;
}

inline CLASS_ENUM REGENT::mfGetClass1() const
{
	return (CLASS_ENUM) fRegentData.Class1;
}

inline unsigned int REGENT::mfGetLevel1() const
{
	return fRegentData.Level1;
}

inline CLASS_ENUM REGENT::mfGetClass2() const
{
	return (CLASS_ENUM) fRegentData.Class2;
}

inline unsigned int REGENT::mfGetLevel2() const
{
	return fRegentData.Level2;
}

inline BLOOD_GRADE REGENT::mfGetBL_grade() const
{
	return (BLOOD_GRADE) fRegentData.BL_grade;
}

inline BLOOD_DERIV REGENT::mfGetBL_deriv() const
{
	return (BLOOD_DERIV) fRegentData.BL_deriv;
}

inline unsigned int REGENT::mfGetBL_strength() const
{
	return fRegentData.BL_strength;
}

inline unsigned int REGENT::mfIncrBL_strength()
{
	++fRegentData.BL_strength;
	return fRegentData.BL_strength;
}

inline SHORT REGENT::mfGetunit() const
{
	return fRegentData.unit;
}


inline SHORT REGENT::mfGetid() const
{
	return fRegentData.id;
}

inline MAP_ICON REGENT::mfGeticon() const
{
	return (MAP_ICON) fRegentData.icon;
}

inline void REGENT::mfSetRealm(REALM::REALM_TYPE const NewRealm)
{
	if (fRegentData.unit <= 0)
	{
		fRegentData.Realm = NewRealm;
	}
	else
	{
		if (units[fRegentData.unit].Realm != NewRealm)
		{
			//units[unit].realm = NewRealm;
			SetGameData(MP_UNITS, MPUNITS_REALM, fRegentData.unit, NewRealm, FALSE);
		}
	}
}

inline void REGENT::mfSetRace(RACE_ENUM const NewRace)
{
	fRegentData.Race = NewRace;
}
inline void REGENT::mfSetClass1(CLASS_ENUM const NewClass1)
{
	fRegentData.Class1 = NewClass1;
}
inline void REGENT::mfSetLevel1(unsigned int const NewLevel)
{
	fRegentData.Level1 = NewLevel;
}
inline void REGENT::mfIncLevel1(LONG const Incr)
{
	fRegentData.Level1 += Incr;
}
inline void REGENT::mfSetClass2(CLASS_ENUM const NewClass2)
{
	fRegentData.Class2 = NewClass2;
}
inline void REGENT::mfSetLevel2(unsigned int const NewLevel)
{
	fRegentData.Level2 = NewLevel;
}
inline void REGENT::mfIncLevel2(LONG const Incr)
{
	fRegentData.Level2 += Incr;
}
inline void REGENT::mfSetBL_grade(BLOOD_GRADE const NewGrade)
{
	fRegentData.BL_grade = NewGrade;
}
inline void REGENT::mfSetBL_deriv(BLOOD_DERIV const NewDerivation)
{
	fRegentData.BL_deriv = NewDerivation;
}
inline void REGENT::mfSetBL_strength(unsigned int const NewStrength)
{
	fRegentData.BL_strength = NewStrength;
}
inline void REGENT::mfSetunit(SHORT const NewUnitVal)
{
	fRegentData.unit = NewUnitVal;
}
inline void REGENT::mfSetid(SHORT const NewId)
{
	fRegentData.id = NewId;
}
inline void REGENT::mfSeticon(MAP_ICON const NewIconVal)
{
	fRegentData.icon = NewIconVal;
}
#endif // _REGENTS_HXX
