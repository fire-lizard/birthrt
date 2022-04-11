/* =®RM250¯=======================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _SCNMGR_HXX
#define _SCNMGR_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <string.h>

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_GAMECAMR_HXX)
#include "gamecamr.hxx"
#endif

#if !defined(_REALM_HXX)
#include "realm.hxx"
#endif

#if !defined(_SCNAI_HXX)
#include "scnai.hxx"
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

LONG const GetNewId();

// Note: All the data and fns are static because I can't see how we will have
//       more than one GAME going at a time. I used a class just to encapsulate
//       the data and fns.

// Note: Please put any GAME related data which travels from SCENE to SCENE
//       in this class. That's what it is for.

class SCENE_MGR {

public:
	typedef enum {
		NO_FADE,
		FADE_OUT,
		FADE_IN
	} FADE_STATE;
	
	static void mfRequestNewScene(char * /* cpSceneFile */,
	                        LONG	/* playerStartLocation */,
	                        BOOL	/* Fade to next scene if TRUE */);

	static void mfDoAI();
	static void mfToggleScenePause(LONG, LONG);
	static void mfSetScenePause(BOOL const bState);
	inline static BOOL const mfIsScenePaused();
	inline static BOOL const mfIsSceneRequested();

	static SHORT		hCurrentScene;
	static ULONG		gTick;
	static ULONG		gFrame;				// Frame count.
	static GAME_CAMERA	fgCamera;
	static BOOL			fbRender;
	static FADE_STATE	fFadeToNextScene;
	
	static SCENE_AI::SCENE_TYPE	SceneType;	// type for next scene ai
	static SHORT				PlacesIndex;// where the adventure is
	static REALM::REALM_TYPE	Aggressor;	// who is the aggressor
	static REALM::REALM_TYPE	Visitors;	// who is the aggressor
	static SHORT				HomeIndex;	// who is the home unit (w/camera)
	static SHORT				AwayIndex;	// who is the away unit
	static SHORT				Victor;		// who won this scene
	static SHORT				Loser;		// who lost this scene
	static SHORT				VictorRealm;// index to realm of winner
	static SHORT				LoserRealm; // index to realm of loser

	static LONG					MachineIds;	// current availible avatar id
	
	static SBYTE				TerrainSubType; // battlefield terrain type
	
	static void mfReleaseSceneToMap(LONG,LONG);
	static void mfToggleFollowCamera(LONG, LONG);
	
protected:
private:
	SCENE_MGR();
	~SCENE_MGR();
	void *operator new (size_t stAllocateBlock);	// Can't use new/delete
	void operator delete (void *p);
	SCENE_MGR(const SCENE_MGR &);			// No copies
	const SCENE_MGR operator=(const SCENE_MGR &);	// No assignments

	static LONG const mfLoadScene(char const * const /* cpSceneFile */,
	                        LONG const  /* playerStartLocation */);

	static BOOL	fScenePaused;

	static BOOL fNewSceneRequested;
	static char fcpNewSceneName[20];
	static LONG fNewScenePlayerStart;
};

/* ------------------------------------------------------------------------
   inline functions
   ------------------------------------------------------------------------ */


/* ========================================================================
   Function    - mfIsScenePaused
   Description -
   Returns     - void
   ======================================================================== */
inline BOOL const SCENE_MGR::mfIsScenePaused()
{
	return fScenePaused;
}

inline BOOL const SCENE_MGR::mfIsSceneRequested()
{
	return fNewSceneRequested;
}
#endif // _SCNMGR_HXX
