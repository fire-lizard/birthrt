// ===========================================================================
//
// AVA.HXX
//
// Class to read/write AVA file data.
//
// ===========================================================================

#if !defined (_AVA_HXX_)
#define _AVA_HXX_

#include "ALIGNMNT.HXX"
#include "GAMEMAP.HXX"
#include "MAIN.HXX"
#include "SCENE.HXX"
#include "PLAYCLAS.HXX"
#include "APPEAR.HXX"
#include "BLOODREQ.HXX"
#include "PLAYSTAT.HXX"
#include "CHARED.HXX"

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

// ---------------------------------------------------------------------------
// "GET" functions.

inline THINGTYPE mfGetThingType() const
{
	return fThingType;
}

inline char * mfGetName()
{
	return fszName;
}

inline LONG mfGetTitle () const
{
	return fTitle;
}

inline LONG mfGetRace () const
{
	return fRace;
}

inline LONG mfGetClass1 () const
{
	return fClass1[0];
}

inline LONG mfGetClass1Level () const
{
	return fClass1[1];
}

inline LONG mfGetClass1Experience () const
{
	return fClass1[2];
}

inline LONG mfGetClass2 () const
{
// Can return -1, meaning there is no class 2.

	return fClass2[0];
}

inline LONG mfGetClass2Level () const
{
	return fClass2[1];
}

inline LONG mfGetClass2Experience () const
{
	return fClass2[2];
}

inline LONG mfGetStrength () const
{
	return fStrength;
}

inline LONG mfGetIntelligence () const
{
	return fIntelligence;
}

inline LONG mfGetDexterity () const
{
	return fDexterity;
}

inline LONG mfGetWisdom () const
{
	return fWisdom;
}

inline LONG mfGetConstitution () const
{
	return fConstitution;
}

inline LONG mfGetCharisma () const
{
	return fCharisma;
}

inline LONG mfGetTHAC0 () const
{
	return fTHAC0;
}

inline LONG mfGetAlignment () const
{
	return fAlignment;
}

inline LONG mfGetHitPoints () const
{
	return fHitPoints;
}

inline LONG mfGetArmorClass () const
{
	return fArmorClass;
}

inline LONG mfGetArmorType () const
{
	return fArmorType;
}

inline LONG mfGetWeapon () const
{
	return fWeapon;
}

inline LONG mfGetMorale () const
{
	return fMorale;
}

inline LONG mfGetTotalHitPoints () const
{
	return fTotalHitPoints;
}

inline LONG mfGetExperience () const
{
// TODO: Figure out how fExperience relates to class experience.  I suppose
// they might be the same things, or related somehow?

	return fExperience;
}

inline LONG mfGetAttacksPerRound () const
{
	return fAttacksPerRound;
}

inline LONG mfGetBloodlineGrade () const
{
	return fBloodlineGrade;
}

inline LONG mfGetBloodlineDerivation () const
{
	return fBloodlineDerivation;
}

inline LONG mfGetBloodlineStrength () const
{
	return fBloodlineStrength;
}

// ---------------------------------------------------------------------------
// "SET" functions.

inline void	mfSetThingType (THINGTYPE thingType)
{
	fThingType = thingType;
}

inline void mfSetStrength (LONG newVal)
{
	fStrength = newVal;
}

inline void mfSetDexterity (LONG newVal)
{
	fDexterity = newVal;
}

inline void mfSetConstitution (LONG newVal)
{
	fConstitution = newVal;
}

inline void mfSetIntelligence (LONG newVal)
{
	fIntelligence = newVal;
}

inline void mfSetWisdom (LONG newVal)
{
	fWisdom = newVal;
}

inline void mfSetCharisma (LONG newVal)
{
	fCharisma = newVal;
}

inline void mfSetRace (LONG newVal)
{
	fRace = newVal;
}

inline void mfSetName (char * szNewName)
{
	strncpy (fszName, szNewName, MAX_NAME_LEN);
}

inline void mfSetTitle (LONG newVal)
{
	fTitle = newVal;
}

inline void mfSetDescription (LONG newVal)
{
	fDescription = newVal;
}

inline void mfSetAlignment (LONG newVal)
{
	fAlignment = newVal;
}

inline void mfSetHitPoints (LONG newVal)
{
	fHitPoints = newVal;
}

inline void mfSetArmorClass (LONG newVal)
{
	fArmorClass = newVal;
}

inline void mfSetArmorType (LONG newVal)
{
	fArmorType = newVal;
}

inline void mfSetWeapon (LONG newVal)
{
	fWeapon = newVal;
}

inline void mfSetMorale (LONG newVal)
{
	fMorale = newVal;
}

inline void mfSetTotalHitPoints (LONG newVal)
{
	fTotalHitPoints = newVal;
}

inline void mfSetTHAC0 (LONG newVal)
{
	fTHAC0 = newVal;
}

inline void mfSetExperience (LONG newVal)
{
	fExperience = newVal;
}

inline void mfSetClass1 (LONG newVal)
{
	fClass1[0] = newVal;
}

inline void mfSetClass1Level (LONG newVal)
{
	fClass1[1] = newVal;
}

inline void mfSetClass1Experience (LONG newVal)
{
	fClass1[2] = newVal;
}

inline void mfSetClass2 (LONG newVal)
{
	fClass2[0] = newVal;
}

inline void mfSetClass2Level (LONG newVal)
{
	fClass2[1] = newVal;
}

inline void mfSetClass2Experience (LONG newVal)
{
	fClass2[2] = newVal;
}

inline void mfSetAttacksPerRound (LONG newVal)
{
	fAttacksPerRound = newVal;
}

inline void mfSetBloodlineGrade (LONG iGrade)
{
	fBloodlineGrade = iGrade;
}

inline void mfSetBloodlineDerivation (LONG iDerivation)
{
	fBloodlineDerivation = iDerivation;
}

inline void mfSetBloodlineStrength (LONG iStrength)
{
	fBloodlineStrength = iStrength;
}

// ---------------------------------------------------------------------------
// Misc. functions

inline BOOL mfIsWizard ()
{
	return m_class1 == CLASS_WIZARD || m_class2 == CLASS_WIZARD;
}

inline BOOL mfIsBard ()
{
	return m_class1 == CLASS_BARD || m_class2 == CLASS_BARD;
}

inline BOOL mfIsWizardOrBard ()
{
	return mfIsWizard () || mfIsBard ();
}

inline void	mfSetRealmName (char * szName)
{
	strncpy (fszRealmName, szName, AVA_FILE::_MAX_REALM);
}

inline char * mfGetRealmName ()
{
	return fszRealmName;
}

inline LONG	mfGetWizardLevel ()
{
	LONG	level1 = 0;
	LONG	level2 = 0;

	if (m_class1 == CLASS_WIZARD)
		level1 = m_level1;
	if (m_class2 == CLASS_WIZARD)
		level2 = m_level2;

	return MAX (level1, level2);
}

inline LONG	mfGetBardLevel ()
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
