/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: combcntl.hxx
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

#if !defined(_COMBCNTL_HXX)
#define _COMBCNTL_HXX	1
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */


#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_AVATAR_HXX)
#include "avatar.hxx"
#endif

#if !defined(_PANEL_H)
#include "panel.h"
#endif

#if !defined(_REALM_HXX)
#include "realm.hxx"
#endif

#if !defined(_SNDVOX_HXX)
#include "sndvox.hxx"
#endif

#if !defined(_SOUND_HXX)
#include "sound.hxx"
#endif

#if !defined(_VECTOR_HXX)
#include "vector.hxx"
#endif

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define MAX_NUMBER_CONTROLS	4

#define MAX_COMBAT_SPELL_BOXES	6
#define MAX_FIGHTER_COMBAT_SPELL_BOXES	4

#define COMBAT_FIGHTER_PANEL			"UI\\COMBAT1A.PCX"
#define COMBAT_FIGHTER_HIGHLIGHT_PANEL	"UI\\COMBAT1B.PCX"
#define COMBAT_REGULAR_PANEL			"UI\\COMBAT2A.PCX"
#define COMBAT_REGULAR_HIGHLIGHT_PANEL	"UI\\COMBAT2B.PCX"

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
   
/* ------------------------------------------------------------------------
   Classes
   ------------------------------------------------------------------------ */

// Class to hold the static array of the avatars and which menu they
// are attached to.

class CONTROL_PANELS {
public:

	class CURRENT_CONTROL {
	public:
		friend class CONTROL_PANELS;
		
		inline CURRENT_CONTROL();
		~CURRENT_CONTROL() { }
		
		void mfSetControl(SHORT const /* hMenu */,
		                  SHORT const /* hAvatar */,
		                  BOOL	const /* SpeakFlag */,
		                  LONG  const /* MaxSpellBoxes */);
		void mfRelease();
		void mfSetBeginCombat(BOOL const /* AutoAttack */);
		
		// Test whether the avatar has new information to draw on the panel.
		BOOL const mfNeedToUpdatePanel();
		
		// Reset the avatar back to follow the player state if possible.
		void mfSetEndCombat();
		
		// Test whether this menu is hooked up to an avatar or not.
		inline BOOL const mfPanelInUse() const;
		
		void mfPaintPanel();
		void mfPaintFighterPanel();
		
		void mfSwapPanels(CURRENT_CONTROL &);
		
		inline void mfSpeakAcknowledgement() const;
		
		void mfUpdateOptions();
		
		// Data
		SHORT	fhMenu;
		SHORT	fhAvatar;
	protected:
	private:
		LONG				fMaxSpellBoxes;
		BOOL				fSpeakFlag;
		CAvatar::AISTATUS	fOriginalStatus;
		LONG   				fOriginalAngle;
		CThing::PFVC		fOriginalLeftButtonFn;
		
		CAvatar::AISTATUS	fPrevStatus;
		CAvatar::AISTATUS	fPrevPauseStatus;
		LONG				fPrevHitPts;
		SBYTE				fPrevDamage;
		SHORT				fPrevEngaged;
		SHORT				fPrevSpells[MAX_COMBAT_SPELL_BOXES];
		FIGHT_SEQUENCE::ATTACK_MODE		fPrevAttackMode;
		
		static LONG const mfConvertPanel(LONG const);
		static BOOL const mfIsRegularCombatPanel(LONG const /* panel */ );
		static BOOL const mfIsFighterCombatPanel(LONG const /* panel */ );
		static BOOL const mfNeedToConvert(LONG const /* panel1 */,
		                           		  LONG const /* panel2 */ );
	};
	friend class CURRENT_CONTROL;

			static void mfMatchMenuToCurrent(LONG const);
	inline	static BOOL const mfCurrentInUse();
	inline	static BOOL const mfIsCurrentLeader();
			static SHORT const mfGetCurrAvatarHdl();
			static void mfSetBeginCombatAll();
	inline	static void mfMatchAvatarHdlToCurrent(SHORT const hLeader);
			static void mfReleaseAll();
	inline	static void mfSetEndAllCombat();
	inline	static void mfCheckAllPanels();
	inline	static void mfPaintThisMenu(SHORT MenuIndex);
	inline	static void mfCurrentSpeakAcknowledgement();
	inline	static void mfSetCurrentToLeader();
	inline	static void mfSetAllDefend();
			static void mfInitAllPanels();
			static void mfCycleToNextPanel();
			static void mfUpdateOptions();
	
protected:
private:
	static DECL_VECTOR_CLASS_S(CURRENT_CONTROL,fccActive,MAX_NUMBER_CONTROLS);
	static SHORT 			fsccIndex;
	
	static SHORT			fPrevsccIndex;
	
	static void 			mfSetNewCurrent(SHORT const);
	static BOOL				fCurrentlyRPG_Mode;
};

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Local Variables
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

// Constructor
inline CONTROL_PANELS::CURRENT_CONTROL::CURRENT_CONTROL() :
	fhMenu(fERROR),
	fhAvatar(fERROR),
	fMaxSpellBoxes(0),
	fSpeakFlag(FALSE),
	fOriginalStatus(CAvatar::AI_RUNNING),
	fOriginalAngle(0),
	fOriginalLeftButtonFn(0),
	fPrevStatus(CAvatar::AI_INIT),
	fPrevPauseStatus(CAvatar::AI_INIT),
	fPrevHitPts(0),
	fPrevDamage(-1),
	fPrevEngaged(0),
	fPrevAttackMode(FIGHT_SEQUENCE::ATM_NONE)
{
	for (LONG i = 0; i < MAX_COMBAT_SPELL_BOXES; i++)
	{
		fPrevSpells[i] = -1;
	}
}

// Test whether this menu is hooked up to an avatar or not.
inline BOOL const CONTROL_PANELS::CURRENT_CONTROL::mfPanelInUse() const
{
	return ((fhAvatar != fERROR) ? TRUE : FALSE);
}

inline BOOL const CONTROL_PANELS::CURRENT_CONTROL::mfIsRegularCombatPanel(LONG const panel)
{
	return (panel >= D_COMBAT_CONTROL1 && panel <= D_COMBAT_CONTROL4);
}

inline BOOL const CONTROL_PANELS::CURRENT_CONTROL::mfIsFighterCombatPanel(LONG const panel)
{
	return (panel >= D_COMBAT_FIGHTER_CONTROL1 && panel <= D_COMBAT_FIGHTER_CONTROL4);
}

inline BOOL const CONTROL_PANELS::CURRENT_CONTROL::mfNeedToConvert(LONG const panel1,
                                                                   LONG const panel2)
{
	BOOL Result = FALSE;
	
	if ((CONTROL_PANELS::CURRENT_CONTROL::mfIsRegularCombatPanel(panel1) && 
	     CONTROL_PANELS::CURRENT_CONTROL::mfIsFighterCombatPanel(panel2) ) ||
	    (CONTROL_PANELS::CURRENT_CONTROL::mfIsFighterCombatPanel(panel1) && 
	     CONTROL_PANELS::CURRENT_CONTROL::mfIsRegularCombatPanel(panel2) )
	   )
	
	{
		Result = TRUE;
	}
	
	return Result;
}

// This converts the panel from a single combat to a fighter one.
// Note this relies on the panels being in sequence.
inline LONG const CONTROL_PANELS::CURRENT_CONTROL::mfConvertPanel(LONG const panel)
{
	LONG NewPanel;
	if (CONTROL_PANELS::CURRENT_CONTROL::mfIsRegularCombatPanel(panel))
	{
		NewPanel = D_COMBAT_FIGHTER_CONTROL1 + (panel - D_COMBAT_CONTROL1);
	}
	else
	{
		NewPanel = D_COMBAT_CONTROL1 + (panel - D_COMBAT_FIGHTER_CONTROL1);
	}
	
	return NewPanel;
}

			
// Test for speech flag and utter accordlingly.
inline void CONTROL_PANELS::CURRENT_CONTROL::mfSpeakAcknowledgement( ) const
{
	if (fSpeakFlag == TRUE && fhAvatar != fERROR)
	{
		CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(fhAvatar);
		
		switch(GAME_TTYPE::mfGetSoundType(pAvatar->mfType()))
		{
		case DOG_SOUNDS:
			AddSndObj(SND_DOG_BARK1, SND_DOG_BARK_TOTAL, -1);
			break;
		default:
			AddVoxSnd(VOX_CASTLE_INQUIRY, pAvatar->ThingIndex);
			break;
		}
		
	}
}

inline BOOL const CONTROL_PANELS::mfCurrentInUse()
{
	return fccActive[fsccIndex].mfPanelInUse();
}
	
inline BOOL const CONTROL_PANELS::mfIsCurrentLeader()
{
	return 0 == fsccIndex;
}
	
inline SHORT const CONTROL_PANELS::mfGetCurrAvatarHdl()
{
	return fccActive[fsccIndex].fhAvatar;
}

inline void CONTROL_PANELS::mfMatchAvatarHdlToCurrent(SHORT const hLeader) 
{
	for (LONG i = 0; i < MAX_NUMBER_CONTROLS; i++)
	{
		if (fccActive[i].fhAvatar == hLeader)
		{
			mfSetNewCurrent(i);
			break;
		}
	}
}

inline void CONTROL_PANELS::mfSetEndAllCombat()
{
	for (LONG i = 0; i < MAX_NUMBER_CONTROLS; i++)
	{
		fccActive[i].mfSetEndCombat();
	}
}

inline void CONTROL_PANELS::mfCheckAllPanels()
{
	for (LONG i = 0; i < MAX_NUMBER_CONTROLS; i++)
	{
		// Check them all, because the update check also refreshes
		// the previous states.
		if (fccActive[i].mfNeedToUpdatePanel() == TRUE)
		{
			fUpdatePanels = TRUE;
		}
	}
	if (fsccIndex != fPrevsccIndex)
	{
		fPrevsccIndex = fsccIndex;
		fUpdatePanels = TRUE;
	}
}


inline void CONTROL_PANELS::mfPaintThisMenu(SHORT MenuIndex)
    {
    	for (LONG i = 0; i < MAX_NUMBER_CONTROLS; i++)
	{
		if (fccActive[i].fhMenu == MenuIndex)
		{
			fccActive[i].mfPaintPanel();
			break;
		}
	}
}

inline void CONTROL_PANELS::mfCurrentSpeakAcknowledgement()
{
	fccActive[fsccIndex].mfSpeakAcknowledgement();
}

inline void CONTROL_PANELS::mfSetCurrentToLeader()
{
	if (fccActive[0].fhAvatar != fERROR)
	{
		CAvatar * const pAvatar = (CAvatar *const) BLKPTR(fccActive[0].fhAvatar);
		pAvatar->mfTurnHighlightOff();
		
		if (pAvatar->hEnemy != fERROR)
		{
			CAvatar * const pEnemy = (CAvatar * const) BLKPTR(pAvatar->hEnemy);
			pEnemy->mfTurnHighlightOff();
		}
	}
	
	if (fccActive[fsccIndex].fhAvatar != fERROR)
	{
		CAvatar * const pAvatar = (CAvatar *const) BLKPTR(fccActive[fsccIndex].fhAvatar);
		pAvatar->mfTurnHighlightOff();
		if (pAvatar->hEnemy != fERROR)
		{
			CAvatar * const pEnemy = (CAvatar * const) BLKPTR(pAvatar->hEnemy);
			pEnemy->mfTurnHighlightOff();
		}
	}
	
	fccActive[0].mfSwapPanels(fccActive[fsccIndex]);
	mfSetNewCurrent(0);
}

inline void CONTROL_PANELS::mfSetAllDefend()
{
	for (LONG i = 0; i < MAX_NUMBER_CONTROLS; i++)
	{
		if (fccActive[i].fhAvatar != fERROR)
		{
			CAvatar * const pAvatar = (CAvatar * const) BLKPTR(fccActive[i].fhAvatar);
			if (!pAvatar->mfAmIImmoblized())
			{
				pAvatar->mfSetAttackMode(FIGHT_SEQUENCE::ATM_DEFEND);
			}
		}
	}
}


#endif // _COMBCNTL_HXX
