/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _REALM_HXX
#define _REALM_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */

#include <string.h>
#include <limits.h>

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_MACHINE_H)
#include "machine.h"
#endif

#if !defined(_MULTIMAP_HXX)
#include "multimap.hxx"
#endif

#if !defined(_VECTOR_HXX)
#include "vector.hxx"
#endif

#if !defined(_STRMGR_H)
#include "strmgr.h"
#endif

#if !defined(_STRENM_H)
#include "strenum.h"
#endif

#if !defined(_LOADSAVE_HXX)
#include "loadsave.hxx"
#endif


extern BOOL IsMultiPlayer ( void );

/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */

//----------------------------------
//	Realms
//----------------------------------
class REALM_STRUCT {
public:
	inline char	*			mfGetName() const;
	inline SHORT	const 	mfGetNameX() const;
	inline SHORT	const 	mfGetNameY() const;
	inline unsigned int		mfGetRealmWidth() const;
	inline unsigned int		mfGetTitleSize() const;
	inline unsigned int		mfGetTitleSpread() const;
	inline unsigned int		mfGetTitleTrans() const;
	inline SHORT 	const	mfGetRegent() const;
	inline BOOL		const	mfShowUnits() const;
	inline BOOL		const	mfShowHoldings() const;
	inline BOOL		const	mfIsPlayerCtrl() const;
	inline BOOL		const	mfIsDMCtrl() const;
	inline BOOL		const	mfIsPlayable() const;
	inline unsigned int		mfGetCourt() const;
	inline BOOL		const 	mfExists() const;
	inline UBYTE	const	mfGetFirstProvince() const;
	inline LONG		const	mfGetSurplusIncome() const;
	inline LONG		const	mfGetRegency() const;
	inline LONG		const	mfGetTreasury() const;
	inline SHORT	const	mfGetIncome() const;
	
	inline void				mfSetNameX( SHORT const /* NewX */);
	inline void				mfSetNameY( SHORT const /* NewY */);
	inline void				mfSetTitleSize( LONG const /* Newts */);
	inline void				mfSetRealmWidth( unsigned int /* Newrw */);
	inline void				mfSetTitleSpread( LONG const /* Newtsp */);
	inline void				mfSetTitleTrans( LONG const /* Newtt */);
	inline void				mfSetRegent( LONG const /* NewRegent */);
	inline void				mfSetShowUnits( BOOL const /* NewShowUnits */);
	inline void				mfSetShowHoldings( BOOL const /* NewShowHoldings */);
	inline void				mfSetPlayable( BOOL const /* NewPlayable */);
	inline void				mfSetPlayerCtrl( BOOL const /* NewPlayerCtrl */);
	inline void				mfSetDMCtrl( BOOL const /* NewPlayerCtrl */);
	inline void				mfSetCourt( LONG const /* NewCourt */);
	inline void				mfSetExists(BOOL const /* NewExists */);
	inline void				mfSetFirstProvince(UBYTE const /* NewFirstProv */);
	inline void				mfSetSurplusIncome(LONG const /* NewSurplusIncome */);
	inline void				mfSetRegency(LONG const /* NewRegency */);
	inline void				mfSetTreasury(LONG const /* NewTreasury */);
	inline void				mfSetIncome(LONG const /* NewIncome */);
	
	inline LONG const		mfAddRegency(LONG const );
	inline LONG const		mfAddTreasury(LONG const);
	inline LONG const		mfAddCourt(LONG const);
	inline LONG const		mfAddSurplusIncome(LONG const);
protected:
private:
	// Helper class to hide the data yet let the complier generate a default
	// constructor. (Saves about 1/2 the data space over one I have to specify.)
	class REALM_DATA {
	public:
		LONG				name;
		SHORT				x, y;						// x,y on the of the name
		unsigned int		titlesize		: 7;
		unsigned int		realmwidth		: 8;
		unsigned int		titlespread		: 1;
		unsigned int		titletrans		: 1;
		unsigned int		regent			: 7;
		unsigned int		ShowUnits		: 1;
		unsigned int		ShowHoldings	: 1;
		unsigned int		Playable		: 1;
		unsigned int		PlayerCtrl		: 1;
		unsigned int		court			: 3;
		unsigned int		exists			: 1;
		UBYTE				first_province;
		UBYTE				SurplusIncome;
		USHORT				regency;
		USHORT				treasury;
		SHORT				income;
	};
	
public:
	REALM_DATA	fRealmData;
};
typedef REALM_STRUCT *REALM_STRUCT_PTR;

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define	REACT_MAXIMUM		100
#define	REACT_NEUTRAL		50
#define	REACT_MINIMUM		0

// next two sets of defines must match
#define	WR						1
#define	NU						2
#define	PA						3
#define	FA						4
#define	VA						5

#define	STATUS_AT_WAR					1
#define	STATUS_NEUTRAL					2
#define	STATUS_PERMISSIVE_ALLIANCE	3
#define	STATUS_FULL_ALLIANCE			4
#define	STATUS_VASSAL					5
#define	STATUS_ADJACENT				0x40
#define	STATUS_AT_WAR_ADJ				0x41
#define	STATUS_SCREWED					0x80

/* ------------------------------------------------------------------------
   Enums and Classes
   ------------------------------------------------------------------------ */

class REALM {

public:
	typedef enum {
				ANY_REALM = -2,
/* 00 */		NO_COUNTRY = 0,		// For Mercanaries.
				FIRST_REALM = 1,// For loops
/* 01 */		AERENWE = 1,
/* 02 */		ALAMIE,
/* 03 */		ANUIRE,
/* 04 */		AVANIL,
/* 05 */		BARUKAZHIK,
/* 06 */		BOERUINE,
/* 07 */		BROSENGAE,
/* 08 */		CARIELE,
/* 09 */		CHIMAERON,
/* 10 */		COERANYS,
/* 11 */		DHOESONE,
/* 12 */		DIEMED,
/* 13 */		ELINIE,
/* 14 */		ENDIER,
/* 15 */		FIVEPEAKS,
/* 16 */		GHOERE,
/* 17 */		GORGONSCROWN,
/* 18 */		ILIEN,
/* 19 */		MARKAZOR,
/* 20 */		MEDOERE,
/* 21 */		MHORIED,
/* 22 */		MIERES,
/* 23 */		MONSTERS,
/* 24 */		MURKILAD,
/* 25 */		OSOERDE,
/* 26 */		RHUOBHE,
/* 27 */		ROESONE,
/* 28 */		SIELWODE,
/* 29 */		SPIDERFELL,
/* 30 */		TAEGHAS,
/* 31 */		TALINIE,
/* 32 */		THURAZOR,
/* 33 */		TUARHIEVEL,
/* 34 */		TUORNEN,		

/* 35 */		N_IMPORTS_EXPORTS,
/* 36 */		IMPREGNABLE_HEART_OF_HAELYN,
/* 37 */		S_ANUIRE_SHIPPING_AND_IMPORTS,
/* 38 */		HIGH_MAGE_AELIES,
/* 39 */		PORT_OF_CALL_EXCHANGE,
/* 40 */		ORTHODOX_IMPERIAL_TEMPLE_OF_HAELYN,
/* 41 */		WIZARD_HERMEDHIE,
/* 42 */		CELESTIAL_JEWEL_OF_SARIMIE,
/* 43 */		HIDDEN_TEMPLE_OF_CUIRAECEN,
/* 44 */		UPPER_ANUIRE_TRADERS,
/* 45 */		HAELYNS_AEGIS,
/* 46 */		MILITANT_ORDER_OF_CUIRAECEN,
/* 47 */		POINTS_EAST_TRADING_GUILD,
/* 48 */		WIZARD_SECOND_SWAMP_MAGE,
/* 49 */		N_REFORMED_CHURCH_OF_SARIMIE,
/* 50 */		WIZARD_EYELESS_ONE,
/* 51 */		RUARCH_ROCKHAMMER,
		// Add new kingdoms above this line.
		REALM_COUNT,
		REALM_OUT_OF_RANGE = 99,
	} REALM_TYPE;

	// The Country the we started in
	REALM_TYPE	HomeRealm;
	
	// Some basic constuctors used by other classes.
	REALM(REALM_TYPE const & rRt)
	: HomeRealm(rRt) {}
	
	REALM(REALM const &r)
	: HomeRealm(r.HomeRealm) {}
	
	~REALM() {};

	REALM const &operator=(REALM_TYPE const &rt)	// assignments
	{ HomeRealm = rt; return *this; }
	
protected:
private:
	void *operator new (size_t stAllocateBlock);	// Can't use new/delete
	void operator delete (void *p);
};

/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
#define	LAND_REALM_COUNT				REALM::N_IMPORTS_EXPORTS
#define	ENDIER_HEARTLANDS_OUTFITTERS	ENDIER
#define	BROSEN_ROYAL_GUILD				BROSENGAE
#define	RUORNILS_CELESTIAL_SPELL		MEDOERE
#define	COUNCIL_OF_LEADERS				CHIMAERON
#define	N_IMPERIAL_TEMPLE_OF_HAELYN	TALINIE

extern DECL_MATRIX_CLASS_S(UBYTE,RealmReaction,REALM::REALM_COUNT,LAND_REALM_COUNT);
extern DECL_MATRIX_CLASS_S(UBYTE,RealmStatus,REALM::REALM_COUNT,LAND_REALM_COUNT);

extern DECL_VECTOR_CLASS_S(REALM_STRUCT,realm,REALM::REALM_COUNT);


/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
extern char RealmNameBuffer[REALM::REALM_COUNT][22];

/* ======================================================================== */
inline BOOL const ISATWAR(REALM::REALM_TYPE const a,REALM::REALM_TYPE const b)
{
	if (a >= REALM::REALM_COUNT || b >= LAND_REALM_COUNT)
	{
//		fatal_error ("Bad Arguments to ISATWAR\n");
//		return (a == b ) ? FALSE : TRUE;
		return FALSE;
	}
	if (a == REALM::ANY_REALM || b == REALM::ANY_REALM )
		return FALSE;
		
	return (((RealmStatus[a][b])&0x0F)==STATUS_AT_WAR);
}

// GWP Hack converter fns. So Alan won't have to cast his arguments.
inline BOOL const ISATWAR(unsigned long const a, unsigned long const b)
{
	return ISATWAR((REALM::REALM_TYPE const) a, (REALM::REALM_TYPE const) b);
}

/* ======================================================================== */
inline BOOL const AREALLIED(REALM::REALM_TYPE const a,REALM::REALM_TYPE const b)
{
	if (a >= REALM::REALM_COUNT || b >= LAND_REALM_COUNT)
	{
//		fatal_error ("Bad Arguments to AREALLIED\n");
		return (a == b) ? TRUE : FALSE;
	}
	
	// An error in calling but don't stop now.
	if (a == REALM::ANY_REALM || b == REALM::ANY_REALM )
		return FALSE;
		
	// GWP HACK for wad testing.
#if defined(_RELEASE)
	return (((RealmStatus[a][b])&0x0F)>=STATUS_PERMISSIVE_ALLIANCE);
#else
	if (gDontFight == TRUE)
	{
		return TRUE;
	}
	else
	{
		return (((RealmStatus[a][b])&0x0F)>=STATUS_PERMISSIVE_ALLIANCE);
	}
#endif
}

// GWP Hack converter fns. So Alan won't have to cast his arguments.
inline BOOL const AREALLIED(unsigned long const a, unsigned long const b)
{
	return AREALLIED((REALM::REALM_TYPE const) a, (REALM::REALM_TYPE const) b);
}

/* ======================================================================== */
inline BOOL const AREFULLALLIES(REALM::REALM_TYPE const a,REALM::REALM_TYPE const b)
{
	if (a >= REALM::REALM_COUNT || b >= LAND_REALM_COUNT)
	{
//		fatal_error ("Bad Arguments to AREFULLALLIES\n");
		return (a == b) ? TRUE : FALSE;
	}
	// An error in calling but don't stop now.
	if (a == REALM::ANY_REALM || b == REALM::ANY_REALM )
		return FALSE;
		
	return (((RealmStatus[a][b])&0x0F)>=STATUS_FULL_ALLIANCE);
}

// GWP Hack converter fns. So Alan won't have to cast his arguments.
inline BOOL const AREFULLALLIES(unsigned long const a, unsigned long const b)
{
	return AREFULLALLIES((REALM::REALM_TYPE const) a, (REALM::REALM_TYPE const) b);
} 

/* ======================================================================== */
inline BOOL const ISVASSAL(REALM::REALM_TYPE const a,REALM::REALM_TYPE const b)
{
	if (a >= REALM::REALM_COUNT || b >= LAND_REALM_COUNT)
	{
//		fatal_error ("Bad Arguments to ISVASSEAL\n");
		return (a == b) ? FALSE : TRUE;
	}
	// An error in calling but don't stop now.
	if (a == REALM::ANY_REALM || b == REALM::ANY_REALM )
		return FALSE;
		
	return (((RealmStatus[a][b])&0x0F)>=STATUS_VASSAL);
}

// GWP Hack converter fns. So Alan won't have to cast his arguments.
inline BOOL const ISVASSAL(unsigned long const a, unsigned long const b)
{
	return ISVASSAL((REALM::REALM_TYPE const) a, (REALM::REALM_TYPE const) b);
}

// Break any alliance these two realms might have.
inline void SET_NOT_FRIENDLY(REALM::REALM_TYPE const Attacking, REALM::REALM_TYPE const Attacked)
{
	
	// An error in calling but don't stop now.
	if (Attacking == REALM::ANY_REALM || Attacked == REALM::ANY_REALM )
		return;
		
	// GWP Removed at Alan's request.
	//if (Attacking==REALM::FIVEPEAKS || Attacked==REALM::FIVEPEAKS)
	//	return;

	
	if (Attacking == Attacked || Attacking >= REALM::REALM_COUNT || Attacked >= LAND_REALM_COUNT)
		return;


	if (((RealmStatus[Attacking][Attacked]) &0x0F) > STATUS_NEUTRAL)
	{
		RealmStatus[Attacking][Attacked]=STATUS_NEUTRAL;

#ifdef _WINDOWS
		if ( IsMultiPlayer() )
			SetGameData(MP_REALMSTATUS, Attacking, Attacked, STATUS_NEUTRAL, TRUE );
#else
		SetGameData(MP_REALMSTATUS, Attacking, Attacked, STATUS_NEUTRAL, FALSE );
#endif
	}

	if (((RealmStatus[Attacked][Attacking]) &0x0F) > STATUS_NEUTRAL)
	{
		RealmStatus[Attacked][Attacking]=STATUS_NEUTRAL;

#ifdef _WINDOWS
		if ( IsMultiPlayer() )
			SetGameData(MP_REALMSTATUS, Attacked, Attacking, STATUS_NEUTRAL, TRUE );
#else
		SetGameData(MP_REALMSTATUS, Attacked, Attacking, STATUS_NEUTRAL, FALSE );
#endif
	}
}
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

// Warning the string manager buffers are transistory.
inline char * REALM_STRUCT::mfGetName() const
{
	if (ScenarioInfo.RealmNames)
		return (RealmNameBuffer[fRealmData.name-STR_RLM_NAME_CHAOS]);
	else
		return STRMGR_GetStr(fRealmData.name);
}

inline SHORT const REALM_STRUCT::mfGetNameX() const
{
	return (fRealmData.x & 0x2FFF);
}
inline SHORT const REALM_STRUCT::mfGetNameY() const
{
	return fRealmData.y;
}
inline unsigned int REALM_STRUCT::mfGetRealmWidth() const
{
	return fRealmData.realmwidth;
}
inline unsigned int REALM_STRUCT::mfGetTitleSize() const
{
	return fRealmData.titlesize;
}
inline unsigned int REALM_STRUCT::mfGetTitleSpread() const
{
	return fRealmData.titlespread;
}
inline unsigned int REALM_STRUCT::mfGetTitleTrans() const
{
	return fRealmData.titletrans;
}
inline SHORT const REALM_STRUCT::mfGetRegent() const
{
	return fRealmData.regent;
}
inline BOOL const REALM_STRUCT::mfShowUnits() const
{
	return fRealmData.ShowUnits;
}
inline BOOL const REALM_STRUCT::mfShowHoldings() const
{
	return fRealmData.ShowHoldings;
}
inline BOOL const REALM_STRUCT::mfIsPlayable() const
{
	return fRealmData.Playable;
}
inline BOOL const REALM_STRUCT::mfIsPlayerCtrl() const
{
	return fRealmData.PlayerCtrl;
}
inline unsigned int REALM_STRUCT::mfGetCourt() const
{
	return fRealmData.court;
}
inline BOOL const REALM_STRUCT::mfExists() const
{
	return fRealmData.exists;
}
inline UBYTE const REALM_STRUCT::mfGetFirstProvince() const
{
	return fRealmData.first_province;
}
inline LONG const REALM_STRUCT::mfGetSurplusIncome() const
{
	return fRealmData.SurplusIncome;
}
inline LONG const REALM_STRUCT::mfGetRegency() const
{
	return fRealmData.regency;
}
inline LONG const REALM_STRUCT::mfGetTreasury() const
{
	return fRealmData.treasury;
}
inline SHORT const REALM_STRUCT::mfGetIncome() const
{
	return fRealmData.income;
}

inline void	REALM_STRUCT::mfSetTitleSize( LONG const ts )
{
#if defined (_DEBUG)
	if (ts < 0 || ts >= (1 << 7))
	{
		fatal_error("ERROR! REALM_STRUCT titlesize overflow!");
	}
#endif
	fRealmData.titlesize = ts;
}
inline void	REALM_STRUCT::mfSetNameX( SHORT const  NewX)
{
	fRealmData.x = NewX + (fRealmData.x & 0x4000);
}
inline void	REALM_STRUCT::mfSetNameY( SHORT const NewY)
{
	fRealmData.y = NewY;
}
inline void	REALM_STRUCT::mfSetRealmWidth( unsigned int Newrw)
{
	fRealmData.realmwidth = Newrw;
}
inline void	REALM_STRUCT::mfSetTitleSpread( LONG const NewTsp)
{
	fRealmData.titlespread = NewTsp;
}
inline void	REALM_STRUCT::mfSetTitleTrans( LONG const NewTt)
{
	fRealmData.titletrans = NewTt;
}
inline void	REALM_STRUCT::mfSetRegent( LONG const NewRegent)
{
	fRealmData.regent = NewRegent;
}
inline void	REALM_STRUCT::mfSetShowUnits( BOOL const NewShowUnits)
{
	fRealmData.ShowUnits = NewShowUnits;
}
inline void	REALM_STRUCT::mfSetShowHoldings( BOOL const NewShowHoldings)
{
	fRealmData.ShowHoldings = NewShowHoldings;
}
inline void	REALM_STRUCT::mfSetPlayable( BOOL const NewPlayable)
{
	fRealmData.Playable = NewPlayable;
}
inline void	REALM_STRUCT::mfSetPlayerCtrl( BOOL const NewPlayerCtrl)
{
	fRealmData.PlayerCtrl = NewPlayerCtrl;
}
inline void	REALM_STRUCT::mfSetCourt( LONG const NewCourt)
{
	if (NewCourt < 0)
	{
		fRealmData.court = 0;
	}
	else if (NewCourt > 7)
	{
		fRealmData.court = 7;
	}
	else
	{
		fRealmData.court = NewCourt;
	}
}
inline void	REALM_STRUCT::mfSetExists(BOOL const NewExists)
{
	fRealmData.exists = NewExists;
}
inline void	REALM_STRUCT::mfSetFirstProvince(UBYTE const NewFirstProv)
{
	fRealmData.first_province = NewFirstProv;
}
inline void	REALM_STRUCT::mfSetSurplusIncome(LONG const NewSurplusIncome)
{
	if (NewSurplusIncome < 0)
	{
		fRealmData.SurplusIncome = 0;
	}
	else if (NewSurplusIncome > (LONG)(UCHAR_MAX))
	{
		fRealmData.SurplusIncome = UCHAR_MAX;
	}
	else
	{
		fRealmData.SurplusIncome = NewSurplusIncome;
	}
}
inline void	REALM_STRUCT::mfSetRegency(LONG const NewRegency)
{
	if (NewRegency < 0 || NewRegency > (LONG)(SHRT_MAX))
	{
		fRealmData.regency = 0;
	}
//	else if (NewRegency > (LONG)(USHRT_MAX))
//	{
//		fRealmData.regency = USHRT_MAX;
//	}
	else
	{
		fRealmData.regency = NewRegency;
	}
}
inline void	REALM_STRUCT::mfSetTreasury(LONG const NewTreasury)
{
	if (NewTreasury < 0)
	{
		fRealmData.treasury = 0;
	}
	else if (NewTreasury > (LONG)(USHRT_MAX))
	{
		fRealmData.treasury = USHRT_MAX;
	}
	else
	{
		fRealmData.treasury = NewTreasury;
	}
}
inline void	REALM_STRUCT::mfSetIncome(LONG const NewIncome)
{
	if (NewIncome > SHRT_MAX)
	{
		fRealmData.income = SHRT_MAX;
	}
	else if (NewIncome < SHRT_MIN)
	{
		fRealmData.income = SHRT_MIN;
	}
	else
	{
		fRealmData.income = NewIncome;
	}
}
inline LONG const REALM_STRUCT::mfAddRegency(LONG const NewRegency)
{
	LONG const NewTotalRegency = NewRegency + (LONG) fRealmData.regency;
	
	if (NewRegency > (LONG)(SHRT_MAX))
	{
		// someone messed up their calculation with SHORTs and USHORTs
		// do nothing.
	}
	else if (NewTotalRegency < 0)
	{
		fRealmData.regency = 0;
	}
	else if (NewTotalRegency > (LONG)(USHRT_MAX))
	{
		fRealmData.regency = USHRT_MAX;
	}
	else
	{
		fRealmData.regency = NewTotalRegency;
	}
	return fRealmData.regency;
}
inline LONG const REALM_STRUCT::mfAddTreasury(LONG const NewTreasury)
{
	LONG const NewTotalTreasury = NewTreasury + (LONG)fRealmData.treasury;
	if (NewTotalTreasury < 0)
	{
		fRealmData.treasury = 0;
	}
	else if (NewTotalTreasury > (LONG)(USHRT_MAX))
	{
		fRealmData.treasury = USHRT_MAX;
	}
	else
	{
		fRealmData.treasury = NewTotalTreasury;
	}
	return fRealmData.treasury;
}
inline LONG const REALM_STRUCT::mfAddCourt(LONG const NewCourt)
{
	LONG const NewTotalCourt = NewCourt + (LONG)fRealmData.court;
	if (NewTotalCourt < 0)
	{
		fRealmData.court = 0;
	}
	else if (NewTotalCourt > 7)
	{
		fRealmData.court = 7;
	}
	else
	{
		fRealmData.court = NewTotalCourt;
	}
	return fRealmData.court;
}
inline LONG const REALM_STRUCT::mfAddSurplusIncome(LONG const NewSurplusIncome)
{
	LONG const NewTotalIncome = NewSurplusIncome + (LONG)fRealmData.SurplusIncome;
	
	if (NewTotalIncome < 0)
	{
		fRealmData.SurplusIncome = 0;
	}
	else if (NewTotalIncome > (LONG)(UCHAR_MAX))
	{
		fRealmData.SurplusIncome = UCHAR_MAX;
	}
	else
	{
		fRealmData.SurplusIncome = NewTotalIncome;
	}
	return fRealmData.SurplusIncome;
}

inline BOOL const REALM_STRUCT::mfIsDMCtrl() const
{
	return (BOOL)(fRealmData.x & 0x4000);
}
inline void	REALM_STRUCT::mfSetDMCtrl( BOOL const DMCtrl)
{
	fRealmData.x = ((SHORT)DMCtrl << 14) + (fRealmData.x & 0x2FFF);
}


/*	======================================================================== */
#endif // _REALM_HXX


