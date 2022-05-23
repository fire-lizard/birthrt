/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _AIFTSEQ_HXX
#define _AIFTSEQ_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include "typedefs.h"
#if defined (_DEBUG) && !defined(_MACHINE_H)
#include "machine.h"
#endif

#if !defined(_THINGTYP_H)
#include "thingtyp.h"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define MAX_FIGHT_SEQUENCES	6

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
class FIGHT_SEQUENCE {
public:
	typedef enum {
		ATM_NONE	= 0,
		ATM_DEFEND,
		ATM_HIGH,
		ATM_LOW,
		ATM_QUICK_HIGH,
		ATM_QUICK_LOW,
		ATM_MAGIC,
		
		// Add new one modes above this one.
		ATM_LOOP
	} ATTACK_MODE;
	
	class FIGHT_SEQUENCE_DATA {
	public:
		friend class FIGHT_SEQUENCE;
		FIGHT_SEQUENCE_DATA(ATTACK_MODE WeakSeq, ATTACK_MODE* AttackSeq)
		{
			this->WeakSeq = WeakSeq;
			this->AttackSeq = AttackSeq;
		}
	
	protected:
	private:
		ATTACK_MODE WeakSeq;
		ATTACK_MODE *AttackSeq;
	};

	inline ATTACK_MODE const mfGetNextSequence(THINGTYPE const);
	inline ATTACK_MODE const mfGetCurrentSequence(THINGTYPE const) const;
	inline BOOL const mfIsWeakSequence(THINGTYPE const) const;
	inline ATTACK_MODE const mfGetWeakSequence(THINGTYPE const) const;
	
	inline void mfInit();
	
protected:
private:
	SBYTE	fCurrentSequence;
	
	static FIGHT_SEQUENCE_DATA fFightSequenceData[(LAST_GAME_AVATAR - FIRST_GAME_THING) + 1];
};

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Inline functions   
   ------------------------------------------------------------------------ */
   
inline void FIGHT_SEQUENCE::mfInit()
{
	fCurrentSequence = 0;
}

// Advance to the next fight sequence.
inline FIGHT_SEQUENCE::ATTACK_MODE const FIGHT_SEQUENCE::mfGetNextSequence(THINGTYPE const ThingType)
{
#if defined (_DEBUG)
	if (ThingType < FIRST_GAME_THING || ThingType > LAST_GAME_AVATAR)
	{
		fatal_error("FILE: %s[%d] ThingType %ld has no attack sequence.\n", 
					__FILE__, __LINE__, ThingType);
	}
#endif

	fCurrentSequence++;
	if (fFightSequenceData[ThingType - FIRST_GAME_THING].AttackSeq[fCurrentSequence] == ATM_LOOP)
		fCurrentSequence = 0;
	
	return fFightSequenceData[ThingType - FIRST_GAME_THING].AttackSeq[fCurrentSequence];
}

inline FIGHT_SEQUENCE::ATTACK_MODE const FIGHT_SEQUENCE::mfGetCurrentSequence(THINGTYPE const ThingType) const
{
	return fFightSequenceData[ThingType - FIRST_GAME_THING].AttackSeq[fCurrentSequence];
}

// Get the weak sequence for this type of warrior
inline FIGHT_SEQUENCE::ATTACK_MODE const FIGHT_SEQUENCE::mfGetWeakSequence(THINGTYPE const ThingType) const
{
	if (ThingType < FIRST_GAME_THING || ThingType > LAST_GAME_AVATAR)
	{
		return ATM_NONE;
	}

	return fFightSequenceData[ThingType - FIRST_GAME_THING].WeakSeq;
}

// Are we currently on our weak sequence?
inline BOOL const FIGHT_SEQUENCE::mfIsWeakSequence(THINGTYPE const ThingType) const
{
	return ((fFightSequenceData[ThingType - FIRST_GAME_THING].WeakSeq)  ==
	        (fFightSequenceData[ThingType - FIRST_GAME_THING].AttackSeq[fCurrentSequence]));
}
#endif // _AIFTSEQ_HXX
