/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _SCENE_HXX
#define _SCENE_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#if !defined(_SYSTEM_H)
#include "system.h"
#endif

#if !defined(_MACHINE_H)
#include "machine.h"
#endif

#if !defined(_PANORAMA_HXX)
#include "panorama.hxx"
#endif

#if !defined(_SCNAI_HXX)
#include "scnai.hxx"
#endif

#if !defined(_SCNCAMR_HXX)
#include "scncamr.hxx"
#endif

#if !defined(_SCNDEMO)
#include "scndemo.hxx"
#endif

#if !defined(_SCNMAP_HXX)
#include "scnmap.hxx"
#endif

#if !defined(_SCNMUSIC_HXX)
#include "scnmusic.hxx"
#endif

#if !defined(_SCNVER_HXX)
#include "scnver.hxx"
#endif

#if !defined(_STRENUM_H)
#include "strenum.h"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define MAX_AVATARS			800

#define MAX_SCENE_FILES	100
#define SCENEFILE_PATH	"SCENES\\"
#define SCENEFILE_EXT	".SCN"

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
class SCENE {
public:
	typedef enum {
		GAME_SCENE = 0,
		USER_SCENE,
	} SCENE_TYPE;
	
	typedef struct _scene_data {
		SCENE_TYPE	SceneType;
		SHORT			sSceneDiff;
		CHAR			szSceneDesc[128];
		CHAR			szWadName[13];
	} SCENE_DATA;
	
	void mfInitializeData();
    void mfLoadScene( char * /* cpSceneFile */,
                      LONG   /* player start position */);
    void mfReleaseScene();
    void mfDeleteScene();
    void mfCallSceneAI();
	void mfSetSceneState(SCENE_AI::STATE /* SceneStateFlag */);
	void mfSetSceneType(SCENE_AI::SCENE_TYPE type );
	void mfGetSceneAIState(SCENE_AI::STATE *, SCENE_AI::SCENE_TYPE *) const;
#if defined(SCENE_EDITOR)
	BOOL mfWriteSceneFile();	// Used by scene editor.
	BOOL mfAddHotKey(HDL_KEY hNewKey);
#endif

	BOOL const mfGetExitScene(LONG &/* id */, char ** const /* cpNewSceneName */ ); 
	char *mfGetSceneName();
	char *mfGetCommandFileName ();
    char *mfGetBackGroundMusic();
    void mfSetScenePallet();
    static void mfDrawMapStuff();
	LONG mfAvatarIndex( LONG /* Id */);
    static void mfDeleteAvatar(SHORT);
    
    GAME_STRING const mfGetNote(LONG const /* NoteId */) const;
    
    GAME_STRING const mfGetDialog(LONG const /* TripLine */ ) const;
    
    // Return fERROR if not set.
    inline LONG const mfGetOpeningDialog() const;
    inline void mfClearOpeningDialog();
    
	
    SHORT    	Avatars[MAX_AVATARS];
    
    // Handle to data used by specialized Scene AI functions.
    SHORT        fhData;                        // Private AI scene data.
    
protected:
private:
    // Overloaded fns to give link errors.
    SCENE();
    ~SCENE();
    // You can't use new or delete with this memory manager.
    void *operator new( size_t stAllocateBlock);
    void operator delete( void *p );
    SCENE(const SCENE &);                   // No copies
    const SCENE operator=(const SCENE &);   // No assignments.
    
    SCENE_FILE_VERSION	fVersion;		// Current File version.
    SCENE_AI			fSceneAI;
    
	SHORT				fMaxSectors;
	SHORT				fhSectors;
	
	SHORT				fMaxPlaces;
	SHORT				fhPlaces;
	
	SHORT				fMaxKeys;
	SHORT				fhKeys;
	
	SHORT				fMaxExitPoints;
	SHORT				fhExitPoints;
	
	SHORT				fMaxNotes;
	SHORT				fhNotes;
	
    MAP					fMapData;
    
    CAMERA_SETUP_DATA	fCameraData;
    
    PANORAMA_DATA		fPanoramaData;
    
    SHORT				fMaxDialogTriplines;
    SHORT				fhDialogTriplines;
    
    SCENE_DEMO_DATA		fSelfRunningDemoData;
    
    SCENE_MUSIC_DATA	fMusicData;
    
    LONG				fOpeningDialog;
	
#if defined(SCENE_EDITOR)
	// Needed for scene editing.
	char				fcpWadName[20];
#endif
	char				fcpPalName[20];

	char				cpThisSceneName[20];
    
	// Callbacks for button clicks
	static void mfSceneButtonObject(LONG, LONG, LONG);
	static void mfSceneButtonFloor(LONG, LONG, LONG);
	
    void mfReadSceneFile( char * /* cpSceneFile */,
                          LONG /* playerStartLocation */);

    LONG mfReadWad(FILE *,
                   LONG /* playerStartLocation */);
    LONG mfReadPallet(FILE *);
    LONG mfReadAvatars(FILE *);
    LONG mfReadPlaces(FILE *);
    LONG mfReadSectors(FILE *);
    LONG mfReadKeys(FILE *);
    LONG mfReadExits(FILE *);
	LONG mfReadAmbientSound(FILE *);
	LONG mfReadBattleExceptions(FILE *);
	LONG mfReadMusicTrack(FILE *);
    LONG mfReadNotes(FILE *);
    LONG mfReadQuestThingType(FILE *);
    LONG mfReadDialogTripLines(FILE *);
    LONG mfReadOpeningDialog(FILE *);
    
#if defined(SCENE_EDITOR)
	void mfWriteSceneFileVersion(FILE *);
	void mfWriteWadName(FILE *);
	void mfWritePalletName(FILE *);
	void mfWriteAvatars(FILE *);
	void mfWritePlaces(FILE *);
	void mfWriteSectors(FILE *);
	void mfWriteKeys(FILE *);
	void mfWriteExitPoints(FILE *);
	void mfWriteMap(FILE *fp);
	void mfWritePanoramaData (FILE *);
	void mfWriteCameraData(FILE *) ;
	void mfWriteSelfRunningDemoData (FILE *) ;
	void mfWriteMusicData (FILE *) ;
	void mfWriteDialogs (FILE *) ;
#endif
    
    void mfDeleteSectors();
    void mfDeletePlaces();
    
    void mfDeleteDialogs();
    
    void mfDeleteKeys();
    void mfReleaseKeys();
    
    void mfDeleteExits();
    void mfReleaseExits();
    
    void mfDeleteNotes();
    void mfReleaseNotes();
    
    void mfDeleteAvatars();
    void mfReleaseAvatars();
    void mfReadAvatars(FILE *, 
                       LONG /* count */ );
    
	void mfAttachAutoAIsAndThingSnds();
	
};

typedef SCENE *PTR_SCENE;
typedef SHORT HDL_SCENE;

SCENE::SCENE_TYPE mfReadUserSceneData( 
	CHAR *ScenePath, 
	SCENE::SCENE_DATA *SceneData 
);
	
LONG mfReadUserWad(FILE *fp, SCENE::SCENE_DATA *SceneData );
LONG mfReadUserType(FILE *fp, SCENE::SCENE_DATA *SceneData );
LONG mfReadUserDescription(FILE *fp, SCENE::SCENE_DATA *SceneData );
LONG mfReadUserDifficulty(FILE *fp, SCENE::SCENE_DATA *SceneData );
	
/* ------------------------------------------------------------------------
   inline functions   
   ------------------------------------------------------------------------ */
/* ========================================================================
   Function    - mfSetScenePallet
   Description - Cover fn to Set the scene Pallet
   Returns     - 
   ======================================================================== */
inline void SCENE::mfSetScenePallet()
{
    if (fcpPalName[0] != 0)
    {
    	init_pal(fcpPalName);
    	init_shade_table(fcpPalName);
    }
}
        

/* ========================================================================
   Function    - mfSetSceneState
   Description - Cover fn to get at the SCENE_AI stuff.
   Returns     - 
   ======================================================================== */
inline void SCENE::mfSetSceneState(SCENE_AI::STATE SceneStateFlag )
{
    fSceneAI.mfSetSceneState(SceneStateFlag);
}
        

/* ========================================================================
   Function    - mfCallSceneAI
   Description - Called every render. 
   Returns     - 
   ======================================================================== */
inline void SCENE::mfCallSceneAI() 
{
   fSceneAI.mfDoAI(*this);
}

/* ========================================================================
   Function    - mfReleaseScene
   Description - Free up the non reuse'able memory.
   Returns     - 
   ======================================================================== */
inline void SCENE::mfReleaseScene() 
{
	mfDeleteAvatars();
	mfDeleteSectors();
    mfReleaseKeys();
	mfDeletePlaces();
	mfDeleteDialogs();
	mfReleaseExits();
	mfReleaseNotes();
	fPanoramaData.mfReleaseData();
}

/* ========================================================================
   Function    - mfDeleteScene
   Description - Free up all the memory.
   Returns     - 
   ======================================================================== */
inline void SCENE::mfDeleteScene() 
{
	mfReleaseAvatars();
	mfDeleteSectors();
    mfDeleteKeys();
	mfDeletePlaces();
	mfDeleteExits();
	mfDeleteNotes();
	mfDeleteDialogs();
	fPanoramaData.mfDeleteData();	 // call the destructor directly.
}

/* ========================================================================
   Function    - mfSetSceneType
   Description - Changes the Scene AI from the generic to whatever.
                 See the SCENE_AI class for the options.
   Returns     - 
   ======================================================================== */
inline void SCENE::mfSetSceneType(SCENE_AI::SCENE_TYPE type ) 
{
	fSceneAI.mfSetSceneType( type);
}

/* ========================================================================
   Function    - mfAddHotKey
   Description - Add a new hot key to the scene.
                 To be called while the game is running. For user defined
                 hot keys.
   Returns     - 
   ======================================================================== */
#if defined(SCENE_EDITOR)
inline BOOL SCENE::mfAddHotKey(HDL_KEY hNewKey) 
{
	HDL_KEYS hKeys = NewBlock(sizeof(HDL_KEY) * (fMaxKeys + 1));
	const BOOL bResult = (hKeys != fERROR) ? TRUE : FALSE;
	
	if (bResult)
	{
		PTR_KEYS pNewKeys = (PTR_KEYS) BLKPTR(hKeys);
		
		if (fMaxKeys > 0 && fhKeys != fERROR)
		{
			PTR_KEYS pOldKeys = (PTR_KEYS) BLKPTR(fhKeys);
			memcpy(pNewKeys, pOldKeys, sizeof(HDL_KEY) * fMaxKeys);
			DisposBlock(fhKeys);
		}
		
		pNewKeys[fMaxKeys] = hNewKey;
		fhKeys = hKeys;
		fMaxKeys++;
	}
	
	return bResult;
}
#endif

/* ========================================================================
   Function    - mfGetCommandFileName
   Description - Get the name of the self running demo command file.
   Returns     - 
   ======================================================================== */
inline char *SCENE::mfGetCommandFileName () 
{
	return fSelfRunningDemoData.mfGetCommandFileName();
}

/* ========================================================================
   Function    - mfGetSceneName
   Description - Get the name of this scene.
   Returns     - 
   ======================================================================== */
inline char *SCENE::mfGetSceneName() 
{
	return cpThisSceneName;
}


/* ========================================================================
   Function    - mfGetBackGroundMusic
   Description - Get the name of the current background scene music file.
   Returns     - 
   ======================================================================== */
inline char *SCENE::mfGetBackGroundMusic() 
{
	return fMusicData.mfGetBackgroundMusic();
}

/* ========================================================================
   Function    - mfGetSceneAIState
   Description - Get the current scene AI state flags
   Returns     - 
   ======================================================================== */
inline void SCENE::mfGetSceneAIState(
SCENE_AI::STATE * pState, 
SCENE_AI::SCENE_TYPE *pType
) const
{
	fSceneAI.mfGetSceneAIState(pState, pType);
}

// If the Opening dialog data is set, return it otherwise STR_NULL
inline LONG const SCENE::mfGetOpeningDialog() const
{
	return fOpeningDialog;
}
inline void SCENE::mfClearOpeningDialog()
{
	fOpeningDialog = STR_NULL;
}
#endif // _SCENE_HXX
