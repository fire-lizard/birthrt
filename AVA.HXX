// ===========================================================================
//
// AVA.HXX
//
// Class to read/write AVA file data.
//
// ===========================================================================

#if !defined (_AVA_HXX_)
#define _AVA_HXX_

// #include "avatar.hxx"
#include "alignmnt.hxx"
#include "gamemap.hxx"
#include "main.hxx"
#include "scene.hxx"
#include "playclas.hxx"
#include "appear.hxx"
#include "bloodreq.hxx"
#include "playstat.hxx"
#include "chared.hxx"

class AVA_FILE {
public:
	enum {
		_MAX_REALM = 64
	};
private:
	// Data
	LONG			fFileVersion;
	THINGTYPE	fThingType;
	LONG			fStatsVersion;
	LONG			fStrength;
	LONG			fDexterity;
	LONG			fConstitution;
	LONG			fIntelligence;
	LONG			fWisdom;
	LONG			fCharisma;
	LONG			fRace;
	char			fszName[MAX_NAME_LEN + 1];
	LONG			fTitle;
	LONG			fDescription;
	LONG			fAlignment;
	LONG			fHitPoints;
	LONG			fArmorClass;
	LONG			fArmorType;
	LONG			fWeapon;
	LONG			fMorale;
	LONG			fTotalHitPoints;
	LONG			fTHAC0;
	LONG			fDamageDice[6];
	LONG			fHitDice[3];
	LONG			fExperience;
	LONG			fClass1[3];
	LONG			fClass2[3];
	LONG			fAttacksPerRound;
	LONG			fBloodlineGrade;
	LONG			fBloodlineDerivation;
	LONG			fBloodlineStrength;

	// Store selected realm name; just for reference, not for AVA file.
	char			fszRealmName[AVA_FILE::_MAX_REALM + 1];

public:
	// Not from AVA file; used to determine available spells when we go
	// to the inventory dialog.  (Reflects current selection.)
	LONG			m_class1;
	LONG			m_level1;
	LONG			m_class2;
	LONG			m_level2;

	// Functions
   LONG			mfReadTextData (FILE *fp);
	LONG			mfWriteTextData (FILE *fp);

	// Get member variables.
	/*inline THINGTYPE	mfGetThingType() const;
	inline LONG			mfGetStrength () const;
	inline LONG			mfGetDexterity () const;
	inline LONG			mfGetConstitution () const;
	inline LONG			mfGetIntelligence () const;
	inline LONG			mfGetWisdom () const;
	inline LONG			mfGetCharisma () const;
	inline LONG			mfGetRace () const;
	inline char *		mfGetName ();
	inline LONG			mfGetTitle () const;
	inline LONG			mfGetDescription () const;
	inline LONG			mfGetAlignment () const;
	inline LONG			mfGetHitPoints () const;
	inline LONG			mfGetArmorClass () const;
	inline LONG			mfGetArmorType () const;
	inline LONG			mfGetWeapon () const;
	inline LONG			mfGetMorale () const;
	inline LONG			mfGetTotalHitPoints () const;
	inline LONG			mfGetTHAC0 () const;
	// Damage dice?
	// Hit dice?
	inline LONG			mfGetExperience () const;
	inline LONG			mfGetClass1 () const;
	inline LONG			mfGetClass1Level () const;
	inline LONG			mfGetClass1Experience () const;
	inline LONG			mfGetClass2 () const;
	inline LONG			mfGetClass2Level () const;
	inline LONG			mfGetClass2Experience () const;
	inline LONG			mfGetAttacksPerRound () const;
	inline LONG			mfGetBloodlineGrade () const;
	inline LONG			mfGetBloodlineDerivation () const;
	inline LONG			mfGetBloodlineStrength () const;

	// Set member variables.
	inline void			mfSetThingType (THINGTYPE thingType);
	inline void			mfSetStrength (LONG newVal);
	inline void			mfSetDexterity (LONG newVal);
	inline void			mfSetConstitution (LONG newVal);
	inline void			mfSetIntelligence (LONG newVal);
	inline void			mfSetWisdom (LONG newVal);
	inline void			mfSetCharisma (LONG newVal);
	inline void			mfSetRace (LONG newVal);
	inline void			mfSetName (char * szName);
	inline void			mfSetTitle (LONG newVal);
	inline void			mfSetDescription (LONG newVal);
	inline void			mfSetAlignment (LONG newVal);
	inline void			mfSetHitPoints (LONG newVal);
	inline void			mfSetArmorClass (LONG newVal);
	inline void			mfSetArmorType (LONG newVal);
	inline void			mfSetWeapon (LONG newVal);
	inline void			mfSetMorale (LONG newVal);
	inline void			mfSetTotalHitPoints (LONG newVal);
	inline void			mfSetTHAC0 (LONG newVal);
	// Damage dice?
	// Hit dice?
	inline void			mfSetExperience (LONG newVal);
	inline void			mfSetClass1 (LONG newVal);
	inline void			mfSetClass1Level (LONG newVal);
	inline void			mfSetClass1Experience (LONG newVal);
	inline void			mfSetClass2 (LONG newVal);
	inline void			mfSetClass2Level (LONG newVal);
	inline void			mfSetClass2Experience (LONG newVal);
	inline void			mfSetAttacksPerRound (LONG newVal);
	inline void			mfSetBloodlineGrade (LONG newVal);
	inline void			mfSetBloodlineDerivation (LONG newVal);
	inline void			mfSetBloodlineStrength (LONG newVal);

	// Functions dealing with temporary data (not saved in the AVA file).
	// Get/set character flags.
	inline BOOL			mfIsWizard ();
	inline BOOL			mfIsBard ();
	inline BOOL			mfIsWizardOrBard ();
	inline LONG			mfGetWizardLevel ();
	inline LONG			mfGetBardLevel ();

	// Get/set realm name (just for printing in the dialog for
	// user's convenient reference).
	inline void			mfSetRealmName (char * szName);
	inline char *		mfGetRealmName ();*/


// ---------------------------------------------------------------------------
// "GET" functions.

inline THINGTYPE AVA_FILE::mfGetThingType() const
{
	return fThingType;
}


inline char * AVA_FILE::mfGetName()
{
	return fszName;
}


inline LONG AVA_FILE::mfGetTitle () const
{
	return fTitle;
}


inline LONG AVA_FILE::mfGetRace () const
{
	return fRace;
}


inline LONG AVA_FILE::mfGetClass1 () const
{
	return fClass1[0];
}


inline LONG AVA_FILE::mfGetClass1Level () const
{
	return fClass1[1];
}


inline LONG AVA_FILE::mfGetClass1Experience () const
{
	return fClass1[2];
}


inline LONG AVA_FILE::mfGetClass2 () const
{
// Can return -1, meaning there is no class 2.

	return fClass2[0];
}


inline LONG AVA_FILE::mfGetClass2Level () const
{
	return fClass2[1];
}


inline LONG AVA_FILE::mfGetClass2Experience () const
{
	return fClass2[2];
}


inline LONG AVA_FILE::mfGetStrength () const
{
	return fStrength;
}


inline LONG AVA_FILE::mfGetIntelligence () const
{
	return fIntelligence;
}


inline LONG AVA_FILE::mfGetDexterity () const
{
	return fDexterity;
}


inline LONG AVA_FILE::mfGetWisdom () const
{
	return fWisdom;
}


inline LONG AVA_FILE::mfGetConstitution () const
{
	return fConstitution;
}


inline LONG AVA_FILE::mfGetCharisma () const
{
	return fCharisma;
}


inline LONG AVA_FILE::mfGetTHAC0 () const
{
	return fTHAC0;
}


inline LONG AVA_FILE::mfGetAlignment () const
{
	return fAlignment;
}


inline LONG AVA_FILE::mfGetHitPoints () const
{
	return fHitPoints;
}


inline LONG AVA_FILE::mfGetArmorClass () const
{
	return fArmorClass;
}


inline LONG AVA_FILE::mfGetArmorType () const
{
	return fArmorType;
}


inline LONG AVA_FILE::mfGetWeapon () const
{
	return fWeapon;
}


inline LONG AVA_FILE::mfGetMorale () const
{
	return fMorale;
}


inline LONG AVA_FILE::mfGetTotalHitPoints () const
{
	return fTotalHitPoints;
}


inline LONG AVA_FILE::mfGetExperience () const
{
// TODO: Figure out how fExperience relates to class experience.  I suppose
// they might be the same things, or related somehow?

	return fExperience;
}


inline LONG AVA_FILE::mfGetAttacksPerRound () const
{
	return fAttacksPerRound;
}


inline LONG AVA_FILE::mfGetBloodlineGrade () const
{
	return fBloodlineGrade;
}

inline LONG AVA_FILE::mfGetBloodlineDerivation () const
{
	return fBloodlineDerivation;
}

inline LONG AVA_FILE::mfGetBloodlineStrength () const
{
	return fBloodlineStrength;
}


// ---------------------------------------------------------------------------
// "SET" functions.

inline void	AVA_FILE::mfSetThingType (THINGTYPE thingType)
{
	fThingType = thingType;
}

inline void AVA_FILE::mfSetStrength (LONG newVal)
{
	fStrength = newVal;
}

inline void AVA_FILE::mfSetDexterity (LONG newVal)
{
	fDexterity = newVal;
}

inline void AVA_FILE::mfSetConstitution (LONG newVal)
{
	fConstitution = newVal;
}

inline void AVA_FILE::mfSetIntelligence (LONG newVal)
{
	fIntelligence = newVal;
}

inline void AVA_FILE::mfSetWisdom (LONG newVal)
{
	fWisdom = newVal;
}

inline void AVA_FILE::mfSetCharisma (LONG newVal)
{
	fCharisma = newVal;
}

inline void AVA_FILE::mfSetRace (LONG newVal)
{
	fRace = newVal;
}

inline void AVA_FILE::mfSetName (char * szNewName)
{
	strncpy (fszName, szNewName, MAX_NAME_LEN);
}

inline void AVA_FILE::mfSetTitle (LONG newVal)
{
	fTitle = newVal;
}

inline void AVA_FILE::mfSetDescription (LONG newVal)
{
	fDescription = newVal;
}

inline void AVA_FILE::mfSetAlignment (LONG newVal)
{
	fAlignment = newVal;
}

inline void AVA_FILE::mfSetHitPoints (LONG newVal)
{
	fHitPoints = newVal;
}

inline void AVA_FILE::mfSetArmorClass (LONG newVal)
{
	fArmorClass = newVal;
}

inline void AVA_FILE::mfSetArmorType (LONG newVal)
{
	fArmorType = newVal;
}

inline void AVA_FILE::mfSetWeapon (LONG newVal)
{
	fWeapon = newVal;
}

inline void AVA_FILE::mfSetMorale (LONG newVal)
{
	fMorale = newVal;
}

inline void AVA_FILE::mfSetTotalHitPoints (LONG newVal)
{
	fTotalHitPoints = newVal;
}

inline void AVA_FILE::mfSetTHAC0 (LONG newVal)
{
	fTHAC0 = newVal;
}

inline void AVA_FILE::mfSetExperience (LONG newVal)
{
	fExperience = newVal;
}

inline void AVA_FILE::mfSetClass1 (LONG newVal)
{
	fClass1[0] = newVal;
}

inline void AVA_FILE::mfSetClass1Level (LONG newVal)
{
	fClass1[1] = newVal;
}

inline void AVA_FILE::mfSetClass1Experience (LONG newVal)
{
	fClass1[2] = newVal;
}

inline void AVA_FILE::mfSetClass2 (LONG newVal)
{
	fClass2[0] = newVal;
}

inline void AVA_FILE::mfSetClass2Level (LONG newVal)
{
	fClass2[1] = newVal;
}

inline void AVA_FILE::mfSetClass2Experience (LONG newVal)
{
	fClass2[2] = newVal;
}

inline void AVA_FILE::mfSetAttacksPerRound (LONG newVal)
{
	fAttacksPerRound = newVal;
}

inline void AVA_FILE::mfSetBloodlineGrade (LONG iGrade)
{
	fBloodlineGrade = iGrade;
}

inline void AVA_FILE::mfSetBloodlineDerivation (LONG iDerivation)
{
	fBloodlineDerivation = iDerivation;
}

inline void AVA_FILE::mfSetBloodlineStrength (LONG iStrength)
{
	fBloodlineStrength = iStrength;
}

// ---------------------------------------------------------------------------
// Misc. functions

inline BOOL AVA_FILE::mfIsWizard ()
{
	return m_class1 == CLASS_WIZARD || m_class2 == CLASS_WIZARD;
}

inline BOOL AVA_FILE::mfIsBard ()
{
	return m_class1 == CLASS_BARD || m_class2 == CLASS_BARD;
}

inline BOOL AVA_FILE::mfIsWizardOrBard ()
{
	return mfIsWizard () || mfIsBard ();
}

inline void	AVA_FILE::mfSetRealmName (char * szName)
{
	strncpy (fszRealmName, szName, AVA_FILE::_MAX_REALM);
}

inline char * AVA_FILE::mfGetRealmName ()
{
	return fszRealmName;
}

inline LONG	AVA_FILE::mfGetWizardLevel ()
{
	LONG	level1 = 0;
	LONG	level2 = 0;

	if (m_class1 == CLASS_WIZARD)
		level1 = m_level1;
	if (m_class2 == CLASS_WIZARD)
		level2 = m_level2;

	return MAX (level1, level2);
}

inline LONG	AVA_FILE::mfGetBardLevel ()
{
	LONG	level1 = 0;
	LONG	level2 = 0;

	if (m_class1 == CLASS_BARD)
		level1 = m_level1;
	if (m_class2 == CLASS_BARD)
		level2 = m_level2;

	return MAX (level1, level2);
}

};

#endif


