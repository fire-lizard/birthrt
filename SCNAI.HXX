/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _SCNAI_HXX
#define _SCNAI_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

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
   Prototypes
   ------------------------------------------------------------------------ */
// Note: The SCENE_AI class is used to hold the state flag, and scene function
//       pointer which must be kept in synchronization. Rather than have this
//       stuff be littered thru the more general SCENE class.
//       However it may be necessary to move the fn and state flag up to
//       speed up the render loop.

class SCENE;

class SCENE_AI {
public:
    typedef void (*PFVS)(SCENE &);

    typedef enum
    {
        NO_SCENE = -1,
        // GWP Not used. GENERIC_SCENE = 0,
        BATTLE_SCENE,
        COMBAT_SCENE,
        ADVENTURE_SCENE,

        // No entries below here
        MAX_SCENES
    }SCENE_TYPE;

    // status enum
    typedef enum {
        NO_STATE = -1,
        INIT = 0,
        RELEASE,
        PAUSED,
		WAITFORKEY,
		FADEOUT,
        PLAY,

        // No entries below here
        SCENE_MAX_STATES
    } STATE;

	SCENE_AI();
	~SCENE_AI() {}
	
   	void mfSetSceneState(STATE const /* Scene State */);
    void mfSetSceneType(SCENE_TYPE const SceneType);
    void mfGetSceneAIState(STATE *, SCENE_TYPE *) const;
    void mfInitVals();
    void mfDoAI(SCENE &rScene);

	// Needs to be public so that hitting ESCAPE can call it.
	static void mfSelfRunningDemoRelease(SCENE &);
protected:
private:
	void *operator new (size_t stAllocateBlock);	// Can't use new/delete
	void operator delete (void *p);
	SCENE_AI(const SCENE_AI &);			// No copies
	const SCENE_AI operator=(const SCENE_AI &);	// No assignments
	
    // Note: Keep the SceneFunc as the first member so the offset addition
    //       can be optimized away. (+0)
    PFVS      	fAISceneFunc;   // Current scene ai function.
    SCENE_TYPE  fSceneType;     // Current scene type.
    STATE		fSceneState;    // Current scene state.

    static PFVS       sfpSceneTable[MAX_SCENES][SCENE_MAX_STATES];

	// Basic scene AI fns.
	static void mfSceneInit(SCENE &);
	static void mfScenePlay(SCENE &);
	static void mfScenePause(SCENE &);
	static void mfSceneWaitForKey(SCENE &);
	static void mfSceneFadeOut(SCENE &);
	static void mfSceneRelease(SCENE &);
	
	// Battle scene AI fns.
	static void mfBattleInit(SCENE &);
	static void mfBattlePlay(SCENE &);
	static void mfBattlePause(SCENE &);
	static void mfBattleWaitForKey(SCENE &);
	static void mfBattleFadeOut(SCENE &);
	static void mfBattleRelease(SCENE &);
	
	// Combat scene AI Fns.
	static void mfCombatInit(SCENE &);
	static void mfCombatPlay(SCENE &);
	static void mfCombatPause(SCENE &);
	static void mfCombatWaitForKey(SCENE &);
	static void mfCombatFadeOut(SCENE &);
	static void mfCombatRelease(SCENE &);
	
	// Self Running Demo scene AI Fns.
	static void mfSelfRunningDemoInit(SCENE &);
	static void mfSelfRunningDemoPlay(SCENE &);
	
	// Adventure scene AI Fns.
	static void mfAdventureInit(SCENE &);
	static void mfAdventurePlay(SCENE &);
	static void mfAdventureRelease(SCENE &);
	
};

/* ========================================================================
   Function    - mfSetSceneState
   Description - Go directly to this scene state. This way some fn other than
                 the scene AI function can cause the scene to be paused or
                 resume play or unload.
   Returns     -
   ======================================================================== */

inline void SCENE_AI::mfSetSceneState(SCENE_AI::STATE const SceneStateFlag )
{
    fSceneState = SceneStateFlag;
    fAISceneFunc = sfpSceneTable[fSceneType][SceneStateFlag];
}

/* ========================================================================
   Function    - mfInitVals
   Description - Initialize the members of this object.
   Returns     -
   ======================================================================== */
inline void SCENE_AI::mfInitVals()
{
	fAISceneFunc = 0;
	mfSetSceneType(NO_SCENE);
	fSceneState = NO_STATE;
}

/* ========================================================================
   Function    - SCENE_AI
   Description - Constructor
   Returns     -
   ======================================================================== */
inline SCENE_AI::SCENE_AI()
{
	mfInitVals();
}


/* ========================================================================
   Function    - mfSetSceneType
   Description - Set the scene to be Generic or Self running demo.
   Returns     -
   ======================================================================== */
inline void SCENE_AI::mfSetSceneType(SCENE_AI::SCENE_TYPE const SceneType)
{
	fSceneType = SceneType;
}


/* ========================================================================
   Function    - mfDoAI
   Description - Execute the AI fn for this scene if there is one.
   Returns     -
   ======================================================================== */
inline void SCENE_AI::mfDoAI(SCENE &rScene)
{
	if (fAISceneFunc)
	{
		(*fAISceneFunc)(rScene);
	}
}
/* ========================================================================
   Function    - mfGetSceneState
   Description - Return the current scene state.
   Returns     -
   ======================================================================== */
inline void SCENE_AI::mfGetSceneAIState(
SCENE_AI::STATE *pState,
SCENE_AI::SCENE_TYPE *pSceneType
	) const
{
	*pState = fSceneState;
	*pSceneType = fSceneType;
}

#endif // _SCNAI_HXX
