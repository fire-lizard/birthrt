/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _COMBOPTS_HXX
#define _COMBOPTS_HXX

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
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Class Definition
   ------------------------------------------------------------------------ */

// Communication class to be called by the options menu whenever
// it thinks I should re-check the current options.
class RECHECK_OPTIONS {
public:
	inline static void mfNewOptions();
	inline static BOOL const mfHaveOptionsChanged();
	static void mfUpDateOptions();
	inline static void mfInitOptions();
	
protected:
private:
	static BOOL	fOptionsChanged;
};


// Class to hold the mode of combat play.
class COMBAT_MODE 
{
public:
	typedef enum {
		ARCADE,
		RPG,
		} MODE;
		
	inline static BOOL const mfIsModeRPG();
	inline static MODE const mfGetMode();
	inline static void mfSetMode(MODE const NewMode);
	inline static void mfResetMode();
	
protected:
private:
	// No objects, copies or assignments
	COMBAT_MODE();
	~COMBAT_MODE();
	COMBAT_MODE(COMBAT_MODE const &);
	COMBAT_MODE const operator=(COMBAT_MODE const &);
	
	static MODE		fMode;
};

#define MAX_COMBAT_DELAY	10
// Class to vary the speed at which the Damage occurs.
class COMBAT_ARCADE_SPEED {
public:
	inline static void mfSetNewValue(SBYTE const NewSpeed );
	inline static SBYTE const mfGetCurrValue();
	inline static SHORT const mfGetDelayValue();
	
	
private:
	// No objects, copies or assignments
	COMBAT_ARCADE_SPEED();
	~COMBAT_ARCADE_SPEED();
	COMBAT_ARCADE_SPEED(COMBAT_ARCADE_SPEED const &);
	COMBAT_ARCADE_SPEED const operator=(COMBAT_ARCADE_SPEED const &);

	static SBYTE fSpeed;
	static SHORT const fDelay[MAX_COMBAT_DELAY];
};

class COMBAT_PLAY {
public:
	typedef enum {
		EASY,
		MODERATE,
		HARD
	} LEVEL;
	
	inline static void mfSetNewValue(LEVEL const newLevel);
	inline static LEVEL const mfGetCurrValue();
	
protected:
private:
	static LEVEL	fLevel;
};

class COMBAT_INITIAL_VIEWPT {
public:
	typedef enum {
		FIRST_PERSON,
		AUTO_POSITION
	} MODE;

	inline static void mfSetFirstPersonViewPt();
	inline static void mfSetAutoPositionMode();
	inline static BOOL mfIsModeFirstPerson();

protected:
private:
	static MODE fInitialState;
};

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Inline functions   
   ------------------------------------------------------------------------ */
   
inline void RECHECK_OPTIONS::mfNewOptions() 
{
	fOptionsChanged = TRUE;
}

inline BOOL const RECHECK_OPTIONS::mfHaveOptionsChanged()
{	
	return fOptionsChanged; 
}

inline void RECHECK_OPTIONS::mfInitOptions() 
{
	fOptionsChanged = FALSE;
}


inline BOOL const COMBAT_MODE::mfIsModeRPG()
{
	return (( fMode == RPG)? TRUE : FALSE); 
}

inline COMBAT_MODE::MODE const COMBAT_MODE::mfGetMode()
{ 
	return fMode; 
}

inline void COMBAT_MODE::mfSetMode(MODE const NewMode)
{ 
	fMode = NewMode; 
	RECHECK_OPTIONS::mfNewOptions();
}


inline void COMBAT_ARCADE_SPEED::mfSetNewValue(SBYTE const NewSpeed )
{ 
	if (NewSpeed >= 0 && NewSpeed < MAX_COMBAT_DELAY)
		fSpeed = NewSpeed;
}

inline SBYTE const COMBAT_ARCADE_SPEED::mfGetCurrValue()
{	
	return fSpeed; 
}

inline SHORT const COMBAT_ARCADE_SPEED::mfGetDelayValue()
{	
	return fDelay[fSpeed]; 
}

inline void COMBAT_PLAY::mfSetNewValue(LEVEL const newLevel)
{
	fLevel = newLevel;
}

inline COMBAT_PLAY::LEVEL const COMBAT_PLAY::mfGetCurrValue()
{	
	return fLevel;	
}
	
inline void COMBAT_INITIAL_VIEWPT::mfSetFirstPersonViewPt()
{
	fInitialState = FIRST_PERSON;
}

inline void COMBAT_INITIAL_VIEWPT::mfSetAutoPositionMode()
{
	fInitialState = AUTO_POSITION;
}

inline BOOL COMBAT_INITIAL_VIEWPT::mfIsModeFirstPerson()
{
	return (fInitialState == FIRST_PERSON);
}

#endif // _COMBOPTS_HXX
