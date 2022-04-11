/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: scene.cpp  -
   Author:   Gary Powell

   ========================================================================

   Contains the following general functions:


   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <ctype.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "scene.hxx"

#include "system.h"
#include "engine.h"
#include "machine.h"
#include "map.h"

#include "avatar.hxx"
#include "playstat.hxx"
#include "fileutil.h"
#include "gamemap.hxx"
#include "battleui.hxx"
#include "scndlgs.hxx"
#include "scnexit.hxx"
#include "scnkey.hxx"
#include "scnnotes.hxx"
#include "scnplace.hxx"
#include "scnsctor.hxx"
#include "strenum.h"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

static LONG ComputePlayerStart();

extern BOOL IsRelicFound( SHORT PlacesIndex );

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
	

// GWP char *EXIT_POINT::fcpDefaultScene = "worldmap";
char *EXIT_POINT::fcpDefaultScene = "castle";

/* ========================================================================
   Function    - ComputePlayerStart
   Description - Based on the camera angle figure out which side of a wad
                 to start in.
   Returns     - The player start for this scene.
   ======================================================================== */
static LONG ComputePlayerStart()
{
	LONG Result;
	
	if (player.a < 32 || player.a > 224)
	{
		// If you are facing North, Therefore you are looking at the South entrance.
		Result = TYPE_PLAYERSTART3;
	}
	else
	if (player.a < 96)
	{
		// If you are facing East, Therefore you are looking at the West entrance.
		Result = TYPE_PLAYERSTART4;
	}
	else
	if (player.a < 160)
	{
		// If you are facing South, Therefore you are looking at the North entrance.
		Result = TYPE_PLAYERSTART1;
	}
	else
	if (player.a <= 224)
	{
		// If you are facing West, Therefore you are looking at the East entrance.
		Result = TYPE_PLAYERSTART2;
	}
	
	return Result;
	
}


/* ========================================================================
   Function    - mfWriteSceneFileVersion
   Description - Write Version information to the scene file.
   Returns     -
   ======================================================================== */
#if defined(SCENE_EDITOR)
inline void SCENE::mfWriteSceneFileVersion(FILE * fp)
{
	fprintf(fp,"[Version]\n");
	fVersion.mfWriteSceneFileVersionData(fp);
}
#endif

/* ========================================================================
   Function    - mfWriteWadName
   Description - Write Wad data to scene file.
   Returns     -
   ======================================================================== */
#if defined(SCENE_EDITOR)
inline void SCENE::mfWriteWadName(FILE * fp)
{
	fprintf(fp, "[Wad]\n%s\n", fcpWadName);
}
#endif

/* ========================================================================
   Function    - mfWritePalletName
   Description - Write the pallet information to the scene file.
   Returns     -
   ======================================================================== */
#if defined(SCENE_EDITOR)
inline void SCENE::mfWritePalletName(FILE * fp)
{
	fprintf(fp, "[Pallet]\n%s\n", fcpPalName);
}
#endif

/* ========================================================================
   Function    - mfWriteMap
   Description - Write the Map data to the scene file.
   Returns     -
   ======================================================================== */
#if defined(SCENE_EDITOR)
inline void SCENE::mfWriteMap(FILE *fp)
{
	const LONG count = fMapData.mfCountOfChanges();
	
	if (count > 0)
	{
		fprintf(fp, "[Map]\n%ld\n", count);
		fMapData.mfWriteMapData(fp);
	}
}
#endif

/* ========================================================================
   Function    - mfWritePanoramaData
   Description - Write the Panorama data to the scene file.
   Returns     -
   ======================================================================== */
#if defined(SCENE_EDITOR)
inline void SCENE::mfWritePanoramaData (FILE *fp)
{
	if (fPanoramaData.mfIsSet())
	{
		fprintf(fp,"[Panorama]\n");
		fPanoramaData.mfWriteData(fp);
	}

}
#endif

/* ========================================================================
   Function    - mfWriteCameraData
   Description - Write the camera data to the scene file.
   Returns     -
   ======================================================================== */
#if defined(SCENE_EDITOR)
inline void SCENE::mfWriteCameraData(FILE *fp)
{
	const LONG count = fCameraData.mfCountOfChanges();
	
	if (count > 0)
	{
		fprintf(fp, "[Camera]\n%ld\n", count);
		fCameraData.mfWriteCameraData(fp);
	}

}
#endif

/* ========================================================================
   Function    - mfWriteSelfRunningDemoData
   Description - Write the self running demo data to the scene file.
   Returns     -
   ======================================================================== */
#if defined(SCENE_EDITOR)
inline void SCENE::mfWriteSelfRunningDemoData (FILE *fp)
{
	if (fSelfRunningDemoData.mfIsSet())
	{
		fprintf(fp,"[Demo]\n");
		fSelfRunningDemoData.mfWriteData(fp);
	}

}
#endif

/* ========================================================================
   Function    - mfWriteMusicData
   Description - Write the scene music data to the scene file.
   Returns     -
   ======================================================================== */
#if defined(SCENE_EDITOR)
inline void SCENE::mfWriteMusicData (FILE *fp)
{
	if (fMusicData.mfIsSet())
	{
		fprintf(fp,"[Music]\n");
		fMusicData.mfWriteData(fp);
	}
}
#endif

/* ========================================================================
   Function    - mfInitializeData
   Description - Get everything ready for the first scene load.
   Returns     -
   ======================================================================== */
void SCENE::mfInitializeData()
{
	fSceneAI.mfInitVals();
	
	for (LONG i=0; i < MAX_AVATARS; i++)
	{
		Avatars[i] = fERROR;
	}
	
	InitSoundThings();

	fMaxPlaces = 0;
	fhPlaces = fERROR;
	
	fMaxSectors = 0;
	fhSectors = fERROR;
	
	fMaxKeys = 0;
	fhKeys = fERROR;
	
	fMaxExitPoints = 0;
	fhExitPoints = fERROR;
	
	fMaxNotes = 0;
	fhNotes = fERROR;
	
	fMaxDialogTriplines = 0;
	fhDialogTriplines = fERROR;
	
	fVersion.mfInitVals();
	
	fMapData.mfInitVals();
	
	fCameraData.mfInitVals();
	
	fPanoramaData.mfInitVals();
	
	fSelfRunningDemoData.mfInitVals();
	
	fOpeningDialog = STR_NULL;
	
#if defined(SCENE_EDITOR)
	// Needed for scene editing.
	fcpWadName[0] = 0;
	fcpPalName[0] = 0;
#endif

	cpThisSceneName[0] = 0;
	
	// At the momement these are the same for all scenes.
	SetMouseClicked(iOBJECT, SCENE::mfSceneButtonObject);
#if defined(TEST_MOVEMENT)
	SetMouseClicked(iFLOOR, SCENE::mfSceneButtonFloor);
#endif
}
/* ========================================================================
   Function    - mfLoadScene
   Description - Load everything necessary for a scene.
                 Does not execute the init_state.
   Returns     -
   ======================================================================== */

void SCENE::mfLoadScene(char * cpSceneFile,
                        LONG playerStartLocation)
{
#if !defined (_WINDOWS)
// JPC. In the DOS version, hide the cursor so it won't obscure the text.
// (The cursor can't be moved after this point, except for a short time
// after the load is complete.)
	SysHideCursor ();
#endif
	// Re-initialize the internal variables.
	// fpData = 0;
	StopRedBook();
	TurnOffAllSounds();
	SuspendMusic();
	fVersion.mfInitVals();
	
	fMaxPlaces = 0;
	fhPlaces = fERROR;
	
	fMaxSectors = 0;
	fhSectors = fERROR;
	
	fMapData.mfInitVals();
	
	fCameraData.mfInitVals();
	
	fSelfRunningDemoData.mfInitVals();
	
	fMaxDialogTriplines = 0;
	fhDialogTriplines = fERROR;
	
	fOpeningDialog = STR_NULL;
	
	// Load the .scn file.
	mfReadSceneFile(cpSceneFile, playerStartLocation);

	if (!fVersion.mfIsSet())
	{
		printf("WARNING! Scene File version data missing! File %s\n", cpSceneFile);
	}
	
#if defined(_SELF_RUNNING_DEMO)
	if (fSelfRunningDemoData.mfIsSet())
	{
		fSceneAI.mfSetSceneType(SCENE_AI::SELF_RUNNING_DEMO_SCENE);
	}
#endif
	fSceneAI.mfSetSceneState(SCENE_AI::INIT);
}


/* ========================================================================
   Function    - mfReadSceneFile
   Description - This loads a scene from file.
   Returns     -
   ======================================================================== */
void SCENE::mfReadSceneFile(char *szName,
                            LONG playerStartLocation)
{
	FILE *fp;
	//GEH char cpSceneFileName[20] = {"scenes\\"};
	char cpSceneFileName[20] = { SCENEFILE_PATH };
	char cpBuffer[128];
	LONG fileResult;
	BOOL WadSeen = FALSE;

	printf("Loading scene %s\n", szName);

	strcat(cpSceneFileName, szName);
	//GEH strcat(cpSceneFileName, ".scn");
	strcat(cpSceneFileName, SCENEFILE_EXT);
	
	fp = FileOpen(cpSceneFileName,"r");

	if (fp == NULL)
	{
		fatal_error("SCENE::mfReadSceneFile Unable to open scene file %s\n",
	                                                      cpSceneFileName);
		return;
	}

	strncpy(cpThisSceneName, szName, sizeof(cpThisSceneName) - 1);
	cpThisSceneName[sizeof(cpThisSceneName) - 1] = 0;

	do
	{
		fileResult = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
		
		if (fileResult != EOF)
		{
			if (strcmp(cpBuffer, "[Version]") == 0)
			{
				fileResult = fVersion.mfReadVersion(fp);
			}
			else
			if (strcmp(cpBuffer, "[Panorama]") == 0)
			{
				fileResult = fPanoramaData.mfReadData(fp);
			}
			else
			if (strcmp(cpBuffer, "[Wad]") == 0)
			{
				fileResult = mfReadWad(fp, playerStartLocation);
				WadSeen = TRUE;
			}
			else
			if (strcmp(cpBuffer, "[Pallet]") == 0)
			{
				fileResult = mfReadPallet(fp);
			}
			else
			if (strcmp(cpBuffer, "[Avatars]") == 0)
			{
				fileResult = mfReadAvatars(fp);
			}
			else
			if (strcmp(cpBuffer, "[Places]") == 0)
			{
				fileResult = mfReadPlaces(fp);
			}
			else
			if (strcmp(cpBuffer, "[Sectors]") == 0)
			{
				fileResult = mfReadSectors(fp);
			}
			else
			if (strcmp(cpBuffer, "[Keys]") == 0)
			{
				fileResult = mfReadKeys(fp);
			}
			else
			if (strcmp(cpBuffer, "[Exit]") == 0)
			{
				fileResult = mfReadExits(fp);
			}
			else
			if (strcmp(cpBuffer, "[Map]") == 0)
			{
				fileResult = fMapData.mfReadMap(fp);
			}
			else
			if (strcmp(cpBuffer, "[Camera]") == 0)
			{
				fileResult = fCameraData.mfReadCameraData(fp);
			}
			else
			if (strcmp(cpBuffer, "[Demo]") == 0)
			{
				fileResult = fSelfRunningDemoData.mfReadData(fp);
			}
			else
			if (strcmp(cpBuffer, "[Music]") == 0)
			{
// if defined SCENE_EDITOR				fileResult = fMusicData.mfReadData(fp);
				fileResult = mfReadMusicTrack(fp);
			}
			else
			if (strcmp(cpBuffer, "[Sounds]") == 0)
			{
				fileResult = mfReadAmbientSound(fp);
			}
			else
			if (strcmp(cpBuffer, "[BtlExceptions]") == 0)
			{
				fileResult = mfReadBattleExceptions(fp);
			}
			else
			if (strcmp(cpBuffer, "[Notes]") == 0)
			{
				fileResult = mfReadNotes(fp);
			}
			else
			if (strcmp(cpBuffer, "[QuestThingType]") == 0)
			{
				fileResult = mfReadQuestThingType(fp);
			}
			else
			if (strcmp(cpBuffer, "[DialogTriplines]") == 0)
			{
				fileResult = mfReadDialogTripLines(fp);
			}
			else
			if (strcmp(cpBuffer,"[OpeningDialog]") == 0)
			{
				fileResult = mfReadOpeningDialog(fp);
			}
			
		}
		
	} while (fileResult != EOF);


	FileClose(fp);
	
	if (!WadSeen)
	{
		fatal_error(
			"SCENE::mfReadSceneFile Missing wad data from %s, unable to continue.",
			cpSceneFileName);
	}

	printf("Done loading scene\n");
}

/* ========================================================================
   Function    - mfReadUserSceneData
	Description - read for the user data sections
	Returns     - scene type
	======================================================================== */
SCENE::SCENE_TYPE mfReadUserSceneData( 
	CHAR *cpScenePath, 
	SCENE::SCENE_DATA *SceneData 
)
{
	FILE *fp;
	CHAR cpBuffer[128];
	LONG fileResult;
	SCENE::SCENE_TYPE	SceneType = SCENE::GAME_SCENE;

	fp = FileOpen(cpScenePath,"r");

	if (fp == NULL)
	{
		fatal_error("SCENE::mfReadSceneFile Unable to open scene file %s\n",
	                                                      cpScenePath);
		return SceneType;
	}

	do
	{
		fileResult = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
		
		if (fileResult != EOF)
		{
			if (strcmp(cpBuffer, "[Wad]") == 0)
			{
				fileResult = mfReadUserWad(fp, SceneData);
			}
			else
			if (strcmp(cpBuffer, "[Type]") == 0)
			{
				fileResult = mfReadUserType(fp, SceneData);
				SceneType = SceneData->SceneType;
			}
			else
			if (strcmp(cpBuffer, "[Description]") == 0)
			{
				fileResult = mfReadUserDescription(fp, SceneData);
			}
			else
			if (strcmp(cpBuffer, "[Difficulty]") == 0)
			{
				fileResult = mfReadUserDifficulty(fp, SceneData);
			}
		}
		
	} while (fileResult != EOF);

	FileClose(fp);
	
	printf("Done loading scene\n");
	
	return SceneType;
}

/* ========================================================================
   Function    - mfReadOpeningDialog
   Description - Read the Opening dialog data section of the scene file.
   Returns     - 
   ======================================================================== */
LONG SCENE::mfReadOpeningDialog(FILE * fp)
{
	char cpBuffer[80];
	LONG Result;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (Result != EOF)
	{
		 fOpeningDialog = (LONG) atol(cpBuffer);
		 fOpeningDialog += STR_OPENING_DIALOG_BASE_NUMBER;
	}
	
	return Result;
}

/* ========================================================================
   Function    - mfReadExits
   Description - Read the Exit section of the scene file.
   Returns     -
   ======================================================================== */
LONG SCENE::mfReadExits(FILE *fp)
{
	char cpBuffer[80];
	LONG Result;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (Result != EOF)
	{
		SHORT MaxExitPoints = (SHORT) atol(cpBuffer);
		
		if (MaxExitPoints > fMaxExitPoints)
		{
			if (fhExitPoints != fERROR)
			{
				DisposBlock(fhExitPoints);
			}
			fMaxExitPoints = MaxExitPoints;
			fhExitPoints = NewBlock(sizeof (EXIT_POINT) * fMaxExitPoints);
		}
		
		if (fhExitPoints == fERROR)
		{
			Result = EOF; // Terminate the file read we're out of memory.
		}
		else
		{
			LONG j;
			
			SetBlockAttr(fhExitPoints, LOCKED, LOCKED);
			PTR_EXIT_POINTS pExits = (PTR_EXIT_POINTS) BLKPTR(fhExitPoints);
			
			for (j = 0; j < MaxExitPoints && Result != EOF; j++)
			{
				Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
				if (Result != EOF)
				{
					pExits[j].mfCreateExitPoint(cpBuffer);
				}
			}
			
			ClrLock(fhExitPoints);
		}
	}
	return Result;

}
/* ========================================================================
   Function    - mfReadDialogTripLines
   Description - Read the dialog trip line section of the scene file.
   Returns     -
   ======================================================================== */
LONG SCENE::mfReadDialogTripLines(FILE *fp)
{
	char cpBuffer[128];
	LONG Result;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (Result != EOF)
	{
		if ( 1 == sscanf(cpBuffer, "%ld", &fMaxDialogTriplines))
		{
			fhDialogTriplines = NewBlock(sizeof (DIALOG_TRIPLINES) * fMaxDialogTriplines);
			if (fhDialogTriplines != fERROR)
			{
				DumbHandleArray<DIALOG_TRIPLINES> pDialogTriplines(fhDialogTriplines);
				LONG i;
				
				for(i = 0; i < fMaxDialogTriplines; i++)
				{
					pDialogTriplines[i].mfInitVals();
				}
				
				for(i = 0; i < fMaxDialogTriplines && Result != EOF; i++)
				{
					Result = pDialogTriplines[i].mfReadData(fp);
				}
			}
			else
			{
				fMaxDialogTriplines = 0;
			}
		}
	}
	return Result;
}

/* ========================================================================
   Function    - mfReadQuestThingType
   Description - Read the Quest section of the scene file.
   Returns     -
   ======================================================================== */
LONG SCENE::mfReadQuestThingType(FILE *fp)
{
	char cpBuffer[80];
	LONG Result;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (Result != EOF)
	{
		LONG	ThingNumber;
		if ( 1 == sscanf(cpBuffer, "%ld", &ThingNumber))
		{
//			if(IsRelicFound(SCENE_MGR::PlacesIndex))
//				SetQuestThing(0);		// no quest item
//			else
				SetQuestThing(ThingNumber);
		}
	}
	return Result;
}

/* ========================================================================
   Function    - mfReadNotes
   Description - Read the Notes section of the scene file.
   Returns     -
   ======================================================================== */
LONG SCENE::mfReadNotes(FILE *fp)
{
	char cpBuffer[80];
	LONG Result;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (Result != EOF)
	{
		SHORT MaxNotes = (SHORT) atol(cpBuffer);
		
		if (MaxNotes > fMaxNotes)
		{
			if (fhNotes != fERROR)
			{
				DisposBlock(fhNotes);
			}
			fMaxNotes = MaxNotes;
			fhNotes = NewBlock(sizeof (SCN_NOTE) * fMaxNotes);
		}
		
		if (fhNotes == fERROR)
		{
			Result = EOF; // Terminate the file read we're out of memory.
		}
		else
		{
			LONG j;
			
			SetBlockAttr(fhNotes, LOCKED, LOCKED);
			SCN_NOTE * const pNotes = (SCN_NOTE * const) BLKPTR(fhNotes);
			
			for (j = 0; j < MaxNotes && Result != EOF; j++)
			{
				Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
				if (Result != EOF)
				{
					LONG	NoteNumber;
					LONG	StringId;
					
					if (2 == sscanf(cpBuffer,"%ld %ld", &NoteNumber, &StringId))
					{
						pNotes[j].mfSetNoteData((SBYTE)NoteNumber, (GAME_STRING)StringId);
					}
					else
					{
						pNotes[j].mfInitVals();
					}
				}
			}
			
			ClrLock(fhNotes);
		}
	}
	return Result;

}



/* ========================================================================
   Function    - mfReadWad
   Description - Read the Wad section of the scene file.
   Returns     -
   ======================================================================== */
LONG SCENE::mfReadWad(FILE *fp,
                      LONG playerStartLocation)
{
	char cpBuffer[40] = { "wads\\" };
	
	const LONG Result = GetNextLine(fp, &cpBuffer[5], sizeof(cpBuffer) - 5);
	if (Result != EOF)
	{
		load_new_wad(cpBuffer, playerStartLocation);
		
		// Connect up the AutoAI's (Avatars found in the wad.)
		// This is done here before we add the scene Avatars.
		mfAttachAutoAIsAndThingSnds();
		
#if defined(SCENE_EDITOR)
		strncpy(fcpWadName, cpBuffer, sizeof(fcpWadName) - 1);
		fcpWadName[sizeof(fcpWadName) - 1] = 0;
#endif
	}
		
	return Result;
}

/* ========================================================================
   unction    - mfReadPallet
   Description - Read the Pallet section of the scene file.
   Returns     -
   ======================================================================== */
LONG SCENE::mfReadPallet(FILE *fp)
{
	char cpBuffer[128];
	
	const LONG Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (Result != EOF)
	{
		strncpy(fcpPalName, cpBuffer, sizeof(fcpPalName) - 1);
		fcpPalName[sizeof(fcpPalName) - 1] = 0;
	}
	
	return Result;
}

/* ========================================================================
   Function    - mfReadAvatars
   Description - Read the Avatar section of the scene file.
   Returns     -
   ======================================================================== */
LONG SCENE::mfReadAvatars(FILE *fp)
{
	LONG i = 0;
	LONG j;
	char cpBuffer[128];
	LONG count;
	
	
	LONG Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (Result != EOF)
		count = atol(cpBuffer);
	
	for (j = 0; j < count && Result != EOF; j++)
	{
		LONG tID, tX, tY, tZ, tA;
		REALM::REALM_TYPE RealmType = SCENE_MGR::Aggressor;
		CAvatar::AIFUNC AIIndex;
		Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
		
		// Takes an optional realm type.
		if (Result != EOF && sscanf(cpBuffer, "%ld %ld %ld %ld %ld %ld %ld",
			&tID, &tX, &tY, &tZ, &tA, &AIIndex, &RealmType) >= 6)
		{
			printf("loading Avatar %d\n",tID);
			LONG lMemoryAvail = ReportFreeMem(TRUE);
			
			printf("Free memory before load...%ld\n", lMemoryAvail);
			while( i < MAX_AVATARS && Avatars[i] != fERROR)
			{
				i++;
			}
			
			if (i < MAX_AVATARS)
			{
				Avatars[i] = CAvatar::NewAvatar();
				
				if (Avatars[i] != fERROR)
				{
					//GEH SetBlockAttr(Avatars[i], LOCKED, LOCKED);
					CAvatar *pAvatar = (CAvatar *) BLKPTR(Avatars[i]);
					if (fERROR != (pAvatar->hPlayerStats = LoadStats(tID,Avatars[i])))
					{
						PLAYER_STATS *pPlayerStats;
						pPlayerStats = (PLAYER_STATS*)BLKPTR(pAvatar->hPlayerStats);
						pPlayerStats->mfMemorizeAllInvSpells();
						pAvatar->ThingType = pPlayerStats->mfGetType();
						pAvatar->Id = tID;
						
					}
					else
					{
						printf("WARNING! Unable to load Avatar %d\n", tID);
						// GWP We call this instead of DeleteAvatar for Debug purposes
						DisposBlock(Avatars[i]);
						Avatars[i] = fERROR;
						i--;
						continue;
					}
					
					pAvatar->mfDefineCThing(pAvatar->mfType(), tX, tY, tZ, tA, 0, 0, 0);
					pAvatar->SetAIFuncIndex(AIIndex);
					pAvatar->mfInitVals();
					pAvatar->Id = tID;
					pAvatar->Realm.HomeRealm = RealmType;
			
					Result = pAvatar->mfReadRestOfSceneData(fp);
					//GEH ClrLock(Avatars[i]);
					printf("Memory used after load...%ld\n", (lMemoryAvail - ReportFreeMem(TRUE)));
				}
			}
			else //i > MAX_AVATARS)
			{
				// Keep reading them from the scene file so we can read
				// the other data sections.
				printf("WARNING! Exceeded number of avatars!.");
			}
		}
		else
		{
			printf("WARNING! Bad Avatar data in the scene file. %s\n", cpBuffer);
		}
	}
	return Result;
}

/* ========================================================================
   Function    - mfReadPlaces
   Description - Read the Places section of the scene file.
   Returns     - File state flag.
   ======================================================================== */
LONG SCENE::mfReadPlaces(FILE *fp)
{
	char cpBuffer[80];
	LONG j;
	
	
	LONG Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (Result != EOF)
	{
		fMaxPlaces = (SHORT) atol(cpBuffer);
			
		fhPlaces = NewBlock(sizeof (PLACE) * fMaxPlaces);
		
		if (fhPlaces == fERROR)
		{
			Result = EOF; // Terminate the file read we're out of memory.
		}
		else
		{
			SetBlockAttr(fhPlaces, LOCKED, LOCKED);
			PTR_PLACES pPlaces = (PTR_PLACES) BLKPTR(fhPlaces);
			
			for (j = 0; j < fMaxPlaces && Result != EOF; j++)
			{
				Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
				if (Result != EOF)
				{
					pPlaces[j].mfCreatePlace(cpBuffer);
				}
			}
			
			ClrLock(fhPlaces);
		}
	}
	
	return Result;
}

/* ========================================================================
   Function    - mfReadSectors
   Description - Read the Sectors section of the scene file.
   Returns     -
   ======================================================================== */
LONG SCENE::mfReadSectors(FILE *fp)
{
	char cpBuffer[80];
	
	
	LONG Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (Result != EOF)
	{
		fMaxSectors = (SHORT) atol(cpBuffer);
		
		fhSectors = NewBlock(sizeof (SCN_SECTOR) * fMaxSectors);
		
		if (fhSectors == fERROR)
		{
			Result = EOF; // Terminate the file read we're out of memory.
		}
		else
		{
			LONG j;
			
			SetBlockAttr(fhSectors, LOCKED, LOCKED);
			PTR_SCN_SECTORS pSectors = (PTR_SCN_SECTORS) BLKPTR(fhSectors);
			
			for (j = 0; j < fMaxSectors && Result != EOF; j++)
			{
				Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
				if (Result != EOF)
				{
					pSectors[j].mfCreateSector(cpBuffer);
				}
			}
			
			ClrLock(fhSectors);
		}
		
	}
		
	return Result;
}
/* ========================================================================
   Function    - mfAmbientSound
   Description - Read the AmbientSound section of the scene file.
   Returns     -
   ======================================================================== */
LONG SCENE::mfReadAmbientSound(FILE *fp)
{
	char cpBuffer[10];
	LONG Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer) );
	if(Result != EOF)
	{
		WhichEnvironmentalSound = (SHORT) atol (cpBuffer);
		if(WhichEnvironmentalSound > 0)
			PlayEnvironmentalSounds = TRUE;
		else
			PlayEnvironmentalSounds = FALSE;


	}
 		return Result;

}
/**********************************************************
	Function mfReadMusicTrack(FILE *fp)
	purpose  read a music track from the scene file
	argument  the file
	return succes
**********************************************************/
LONG SCENE::mfReadMusicTrack(FILE *fp)
{
	char cpBuffer[12];
	SHORT test;
//	FILE *in, *out;
	int in, out;
	char n[256], o[256];

	LONG Result = GetNextLine(fp,cpBuffer, sizeof(cpBuffer) );

	if(Result != EOF)
		ucWhichTrack = (SHORT) atol(cpBuffer);

	LoadMusicFromCD(ucWhichTrack);

	return Result;
}
/* ========================================================================
   Function    - mfReadBattleExceptions
   Description - Read the BattleExceptions section of the scene file.
   Returns     -
   ======================================================================== */
LONG SCENE::mfReadBattleExceptions(FILE *fp)
{
	LONG j;
	char cpBuffer[128];
	LONG count;
	LONG Result;
	SHORT r,c,t;
	
	// clear old exceptions out
	memset(BtlExceptions, 0, sizeof(SHORT) * GRID_MAX_ROWS * GRID_MAX_COLS);
	
	// Get the number of exceptions.
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (Result != EOF)
	{
		count = atol(cpBuffer);
	
		for (j = 0; j < count && Result != EOF; j++)
		{
			Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
			if (Result != EOF)
			{
				sscanf(cpBuffer, "%d %d %d", &r, &c, &t);
				BtlExceptions[r][c] = t;
			}
			else
			{
				break;
			}
		}
	}
 	return Result;
}
/* ========================================================================
   Function    - mfReadKeys
   Description - Read the Keys section of the scene file.
   Returns     -
   ======================================================================== */
LONG SCENE::mfReadKeys(FILE *fp)
{
	char cpBuffer[80];
	
	LONG Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (Result != EOF)
	{
		SHORT MaxKeys = (SHORT) atol(cpBuffer);
		LONG j = 0;
		
		if (MaxKeys > fMaxKeys)
		{
			HDL_KEYS hNewKeys = NewBlock(sizeof (HDL_KEY) * MaxKeys);
			if (hNewKeys != fERROR)
			{
				PTR_KEYS pNewKeys = (PTR_KEYS) BLKPTR(hNewKeys);
				if (fhKeys != fERROR)
				{
					PTR_KEYS pOldKeys = (PTR_KEYS) BLKPTR(fhKeys);
					
					// Copy in the old memory handles for reuse.
					for (j = 0; j < fMaxKeys; j++)
					{
						pNewKeys[j] = pOldKeys[j];
					}
					DisposBlock(fhKeys);
				}
				
				// Initialize the rest.
				for (; j < MaxKeys; j++)
				{
					pNewKeys[j] = fERROR;
				}
				fMaxKeys = MaxKeys;
				fhKeys = hNewKeys;
			}
			else
			{
				// We couldn't get more memory, so use what we had.
				MaxKeys = fMaxKeys;
			}
			
		}
		
		if (fhKeys != fERROR)
		{
			SetBlockAttr(fhKeys, LOCKED, LOCKED);
			PTR_KEYS pKeys = (PTR_KEYS) BLKPTR(fhKeys);
			
			for (j = 0; j < MaxKeys && Result != EOF; j++)
			{
				Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
				if (Result != EOF)
				{
					if (pKeys[j] == fERROR)
					{
						pKeys[j] = NewBlock(sizeof(KEY));
						if (pKeys[j] == fERROR)
						{
							Result = EOF;
							break;
						}
					}
					
					SetBlockAttr(pKeys[j], LOCKED, LOCKED);
					PTR_KEY pKey = (PTR_KEY) BLKPTR(pKeys[j]);
					pKey->mfCreateKey(cpBuffer, pKeys[j]);
					ClrLock(pKeys[j]);
				}
			}
			
			ClrLock(fhKeys);
		}
	}
	return Result;
}
/* ========================================================================
   Function    - mfDeletePlaces
   Description - release all the place handles.
   Returns     - void
   ======================================================================== */
void SCENE::mfDeletePlaces()
{
	if (fhPlaces != fERROR && fMaxPlaces > 0)
	{
		SetBlockAttr(fhPlaces, LOCKED, LOCKED);
		PTR_PLACES pPlaces = (PTR_PLACES) BLKPTR(fhPlaces);
		
		for (LONG i = 0; i < fMaxPlaces; i++)
		{
			if (pPlaces[i].ThingIndex != fERROR)
			{
				pPlaces[i].mfDeletePlace();
			}
		}
		ClrLock(fhPlaces);
		DisposBlock(fhPlaces);
	}
	fhPlaces = fERROR;
	fMaxPlaces = 0;
}
/* ========================================================================
   Function    - mfDeleteSectors
   Description - release all the Sector handles.
   Returns     - void
   ======================================================================== */
void SCENE::mfDeleteSectors()
{
	if (fhSectors != fERROR && fMaxSectors > 0)
	{
		DisposBlock(fhSectors);
	}
	fhSectors = fERROR;
	fMaxSectors = 0;
}

/* ========================================================================
   Function    - mfDeleteExits
   Description - Delete all the Exit handles.
   Returns     - void
   ======================================================================== */
void SCENE::mfDeleteExits()
{
	if (fhExitPoints != fERROR && fMaxExitPoints > 0)
	{
		DisposBlock(fhExitPoints);
	}
	fhExitPoints = fERROR;
	fMaxExitPoints = 0;
}
/* ========================================================================
   Function    - mfReleaseExits
   Description - Mark the Exits as unused.
   Returns     - void
   ======================================================================== */
void SCENE::mfReleaseExits()
{
	if (fhExitPoints != fERROR && fMaxExitPoints > 0)
	{
		SetBlockAttr(fhExitPoints, LOCKED, LOCKED);
		PTR_EXIT_POINTS pExits = (PTR_EXIT_POINTS) BLKPTR(fhExitPoints);
		
		for (LONG i = 0; i < fMaxExitPoints; i++)
		{
			pExits[i].mfInitVals();
		}
		ClrLock(fhExitPoints);
	}
}
/* ========================================================================
   Function    - mfGetNote
   Description - Given a note id, find the GAME_STRING offset.
   Returns     - fERROR or a GAME_STRING offset.
   ======================================================================== */
GAME_STRING const SCENE::mfGetNote(LONG const NoteId) const
{
	GAME_STRING Result = STR_NULL;
	
	if (fMaxNotes > 0 && fhNotes != fERROR)
	{
		SCN_NOTE const * const pNotes  = (SCN_NOTE const * const) BLKPTR(fhNotes);
		LONG i;
		
		for (i = 0; i < fMaxNotes; i++)
		{
			if (pNotes[i].mfGetNoteNumber() == NoteId)
			{
				Result = (GAME_STRING) (STR_NOTE_HEADER + pNotes[i].mfGetStrId());
				break;
			}
		}
	}
	
	return Result;
}

/* ========================================================================
   Function    - mfDeleteNotes
   Description - Delete all the Note Data.
   Returns     - void
   ======================================================================== */
void SCENE::mfDeleteNotes()
{
	if (fhNotes != fERROR && fMaxNotes > 0)
	{
		DisposBlock(fhNotes);
	}
	fhNotes = fERROR;
	fMaxNotes = 0;
}
/* ========================================================================
   Function    - mfReleaseNotes
   Description - Mark the Notes as unused.
   Returns     - void
   ======================================================================== */
void SCENE::mfReleaseNotes()
{
	if (fhNotes != fERROR && fMaxNotes > 0)
	{
		SetBlockAttr(fhNotes, LOCKED, LOCKED);
		SCN_NOTE * const pNotes = (SCN_NOTE * const) BLKPTR(fhNotes);
		
		for (LONG i = 0; i < fMaxNotes; i++)
		{
			pNotes[i].mfInitVals();
		}
		ClrLock(fhNotes);
	}
}
/* ========================================================================
   Function    - mfDeleteKeys
   Description - release all the Key handles.
   Returns     - void
   ======================================================================== */
void SCENE::mfDeleteKeys()
{
	if (fhKeys != fERROR && fMaxKeys > 0)
	{
		SetBlockAttr(fhKeys, LOCKED, LOCKED);
		PTR_KEYS pKeys = (PTR_KEYS) BLKPTR(fhKeys);
		
		for (LONG i = 0; i < fMaxKeys; i++)
		{
			if (pKeys[i] != fERROR)
			{
				PTR_KEY pKey = (PTR_KEY) BLKPTR(pKeys[i]);
				
				KEYSTRUCT ThisKeyStruct;
				
				// Reset the previous key;
				if (pKey->mfGetKeyId() != NO_KEY)
				{
					replace_key(pKey->mfGetKeyId(), pKey->mfGetPrevFunc(), pKey->mfGetPrevVal(), &ThisKeyStruct);
				}
				
				DisposBlock(pKeys[i]);
				pKeys[i] = fERROR;
			}
		}
		ClrLock(fhKeys);
		DisposBlock(fhKeys);
	}
	fhKeys = fERROR;
	fMaxKeys = 0;
}

/* ========================================================================
   Function    - mfReleaseKeys
   Description - release all the Key handles. But save the memory for later.
   Returns     - void
   ======================================================================== */
void SCENE::mfReleaseKeys()
{
	if (fhKeys != fERROR && fMaxKeys > 0)
	{
		SetBlockAttr(fhKeys, LOCKED, LOCKED);
		PTR_KEYS pKeys = (PTR_KEYS) BLKPTR(fhKeys);
		
		for (LONG i = 0; i < fMaxKeys; i++)
		{
			if (pKeys[i] != fERROR)
			{
				PTR_KEY pKey = (PTR_KEY) BLKPTR(pKeys[i]);
				
				KEYSTRUCT ThisKeyStruct;
				
				// Reset the previous key;
				if (pKey->mfGetKeyId() != NO_KEY)
				{
					replace_key(pKey->mfGetKeyId(), pKey->mfGetPrevFunc(), pKey->mfGetPrevVal(), &ThisKeyStruct);
					pKey->mfInitVals();
				}
			}
		}
		ClrLock(fhKeys);
	}
}

/* ========================================================================
   Function    - mfDeleteAvatars
   Description - Call every Avatar destructor
   Returns     - void
   ======================================================================== */
void SCENE::mfDeleteAvatars(void)
{
	LONG i;

	for (i=0; i<MAX_AVATARS; i++)
	{
		if (Avatars[i] != fERROR)
		{
			CAvatar::DeleteAvatar(Avatars[i]);
			Avatars[i] = fERROR;
		}
	}
}

/* ========================================================================
   Function    - mfDeleteAvatar
   Description - remove an avatar from the scene
   Returns     - void
   ======================================================================== */
void SCENE::mfDeleteAvatar(SHORT hAvatar)
{
	LONG i;
	PTR_SCENE pScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
	
	for (i=0; i<MAX_AVATARS; i++)
	{
		if (pScene->Avatars[i] == hAvatar)
		{
			CAvatar::DeleteAvatar(pScene->Avatars[i]);
			pScene->Avatars[i] = fERROR;
			return;
		}
	}
}

/* ========================================================================
   Function    - mfReleaseAvatars
   Description - release avatar data
   Returns     - void
   ======================================================================== */
void SCENE::mfReleaseAvatars()
{
	LONG	i;
	CAvatar *pAvatar;
	
	for(i = 0; i < MAX_AVATARS; i++ )
	{
		if( Avatars[i] != fERROR)
		{
			pAvatar = (CAvatar *) BLKPTR(Avatars[i]);
			pAvatar->DoAI(CAvatar::AI_RELEASE);
		}
	}
	mfDeleteAvatars();
}

/* ========================================================================
   Function    - mfAvatarIndex
   Description - Get the array index of a given Id
   Returns     - index in avatar array
   ======================================================================== */
LONG SCENE::mfAvatarIndex( LONG Id )
{
	LONG i;

	for (i=0; i<MAX_AVATARS; i++)
		if (Avatars[i] != fERROR)
		{
			CAvatar *pAvatar = (CAvatar *) BLKPTR(Avatars[i]);
			if (pAvatar->Id == Id)
				return (i);
		}

	return (MAX_AVATARS);
}
/* ========================================================================
   Function    - mfSceneButtonObject
   Description - Look at the mouse click and do the appropriate thing.
   Returns     -
   ======================================================================== */

void SCENE::mfSceneButtonObject(
	LONG butClicked,
	LONG objClicked,
	LONG ) //typeClicked
{
	
	mouse_click = 0;
	if (SCENE_MGR::hCurrentScene != fERROR)
	{
		LONG i;
		BOOL bClickHandled = FALSE;
		BOOL bObjFound = FALSE;
		
		SetBlockAttr(SCENE_MGR::hCurrentScene, LOCKED, LOCKED);
		PTR_SCENE pScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
		
		for (i = 0; i < MAX_AVATARS; i++)
		{
			if (pScene->Avatars[i] != fERROR)
			{
				CAvatar *pAvatar = (CAvatar *) BLKPTR(pScene->Avatars[i]);
				if (pAvatar->ThingIndex == objClicked)
				{
					bObjFound = TRUE;
					// If the avatar dosen't have a button fn that's ok. It
					// may be dead.
					bClickHandled = TRUE;
					
					if (butClicked == 2 && pAvatar->RightButtonFn)
					{
						pAvatar->RightButtonFn(pAvatar);
					}
					else
					if (butClicked == 1 && pAvatar->LeftButtonFn)
					{
						pAvatar->LeftButtonFn(pAvatar);
					}
					
				}
			}
		}
		
		if (bObjFound == FALSE && pScene->fhPlaces != fERROR)
		{
			PTR_PLACES pPlaces = (PTR_PLACES) BLKPTR(pScene->fhPlaces);
			
			for (i = 0; i < pScene->fMaxPlaces; i++)
			{
				if (pPlaces[i].ThingIndex != fERROR)
				{
					if (pPlaces[i].ThingIndex == objClicked)
					{
						bObjFound = TRUE;
						bClickHandled = TRUE;
						
						SCENE_MGR::mfRequestNewScene(pPlaces[i].mfGetSceneName(), ComputePlayerStart(), TRUE);
						break;
					}
				}
			}
		}
		
#if defined(DEMO_ONLY_CODE)
		// Even if we find found it to be an Avatar if we couldn't do any dialog
		// do some default action.
		if (bClickHandled == FALSE && butClicked == 1)
		{
			const LONG type = mythings[objClicked].type;
			
			// GWP Since it wasn't a place and it isn't an avatar, it must be
			//     a wad makers thing. put up a generic info dialog.
			//GWP DEMO Code!
			if (type == ARCHER ||
			    type == ROYAL_GUARD ||
			    type == LIGHT_CAVALRY ||
			    type == DOG ||
			    type == DWARF_OFFICER ||
			    type == HARPY ||
			    type == WYVERN
			    )
			{
				LONG i;
				
				for (i=0; i<MAX_AVATARS && pScene->Avatars[i] != fERROR; i++)
				{}
				
				if (i >= MAX_AVATARS)
				{
					i = fERROR; 			// We have no more slots!
				}
				else
				{
					SHORT hAvatar = CAvatar::NewAvatar();
					if (hAvatar != fERROR)
					{
						//GEH SetBlockAttr(hAvatar, LOCKED, LOCKED);
						CAvatar *pAvatar = (CAvatar *) BLKPTR(hAvatar);
						
						pAvatar->attrib.RuntimeCreated = TRUE;	// Don't save!
						
						// Fix up the CThing Data.
						pAvatar->ThingIndex = objClicked;
						pAvatar->mfSetType((THINGTYPE) type);
						pAvatar->RightButtonFn = 0;
						pAvatar->LeftButtonFn = 0;
						pAvatar->RightButtonFn = 0;
						
						pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_STAND_STILL);
						
						pAvatar->Status = CAvatar::AI_INIT;
						pAvatar->CurSequence = INITSEQ;
						
						if (pAvatar->mfAngle() < 0)
						{
							pAvatar->FaceTo(PLAYER_INT_VAL(player.x), PLAYER_INT_VAL(player.y));
						}
						
						// Now call the buttonhandler.
						pAvatar->LeftButtonFn(pAvatar);
						//GEH ClrLock(hAvatar);
						
						pScene->Avatars[i] = hAvatar;
					}
				}
				
			}
			
		}
		else
		if (bClickHandled == FALSE && butClicked == 2)
		{
			const LONG type = mythings[objClicked].type;
			
			// GWP Since it wasn't a place and it isn't an avatar, it must be
			//     a wad makers thing. put up a generic info dialog.
			//GWP DEMO Code!
			if (type == ARCHER ||
			    type == ROYAL_GUARD ||
			    type == LIGHT_CAVALRY ||
			    type == DOG ||
			    type == DWARF_OFFICER ||
			    type == HARPY ||
			    type == WYVERN
			    )
			{
				LONG i;
				
				for (i=0; i<MAX_AVATARS && pScene->Avatars[i] != fERROR; i++)
				{}
				
				if (i >= MAX_AVATARS)
				{
					i = fERROR; 			// We have no more slots!
				}
				else
				{
					SHORT hAvatar = CAvatar::NewAvatar();
					if (hAvatar != fERROR)
					{
						//GEH SetBlockAttr(hAvatar, LOCKED, LOCKED);
						CAvatar *pAvatar = (CAvatar *) BLKPTR(hAvatar);
						
						pAvatar->attrib.RuntimeCreated = TRUE;	// Don't save!
						
						// Fix up the CThing Data.
						pAvatar->ThingIndex = objClicked;
						pAvatar->mfSetType((THINGTYPE) type);
						pAvatar->LeftButtonFn = 0;
						pAvatar->RightButtonFn = 0;
						
						pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_STAND_STILL);
						
						pAvatar->Status = CAvatar::AI_INIT;
						pAvatar->CurSequence = INITSEQ;
						
						if (pAvatar->mfAngle() < 0)
						{
							pAvatar->FaceTo(PLAYER_INT_VAL(player.x), PLAYER_INT_VAL(player.y));
						}
						
						// Now call the buttonhandler.
						pAvatar->RightButtonFn(pAvatar);
						//GEH ClrLock(hAvatar);
						
						pScene->Avatars[i] = hAvatar;
					}
				}
				
			}
			
		}
#endif // DEMO_ONLY_CODE
		//GEH ClrLock(SCENE_MGR::hCurrentScene);
		
	}
}
/* ========================================================================
   Function    - mfSceneButtonFloor
   Description - Look at the mouse click and do the appropriate thing.
   Returns     -
   ======================================================================== */

#if defined(TEST_MOVEMENT)
void SCENE::mfSceneButtonFloor(
	LONG ,	// butClicked
	LONG objClicked,
	LONG ) // typeClicked
{
	LONG	sx, sy;
	LONG	compa;
	
	// reset this global to say we have handled the mouse event.
	mouse_click = 0;
	
	if (SCENE_MGR::hCurrentScene != fERROR)
	{
		PTR_SCENE pScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
		
		if (pScene->fhSectors != fERROR)
		{
			LONG i;
			PTR_SCN_SECTORS pSectors = (PTR_SCN_SECTORS) BLKPTR(pScene->fhSectors);
			
			for (i = 0; i < pScene->fMaxSectors; i++)
			{
				if (pSectors[i].mfGetId() != fERROR)
				{
					// GWP Later we may want to put an informational dialog up for
					// GWP right button events.
					if (pSectors[i].mfGetId() == objClicked)
					{
						SCENE_MGR::mfRequestNewScene(pSectors[i].mfGetSceneName(), ComputePlayerStart(), TRUE);
						// Note: After loading a new scene nothing in the pSectors, or
						//       or pScene is valid!
						return;
					}
				}
			}
		}
		
		// test code
		ConvertScreenToFloor(cursor_x, cursor_y, &sx, &sy);
		compa = AngleFromPoint ( PLAYER_INT_VAL(player.x), PLAYER_INT_VAL(player.y), sx, sy, RESOLUTION_8 );
	
#if defined(TEST_MOVEMENT)
		printf( "move from %ld, %ld %ld to %ld, %ld %ld\n",
			PLAYER_INT_VAL(player.x), PLAYER_INT_VAL(player.y), player.a,
			sx, sy, compa );
#endif
			
#if defined(TEST_MOVEMENT)
		SetPlayerTarget( sx, sy, player.h, compa, 0);
#endif
	}
}
#endif
/* ========================================================================
   Function    - mfGetExitScene
   Description - Given an Exit ID return a scene name. WorldMap if not found.
   Returns     - pointer to scene name.
   ======================================================================== */

BOOL const SCENE::mfGetExitScene( LONG &Id, char * *const cpNewSceneName)
{
	BOOL bFound = FALSE;
	
	if (fhExitPoints != fERROR)
	{
		LONG i;
		PTR_EXIT_POINTS pExits = (PTR_EXIT_POINTS) BLKPTR(fhExitPoints);
		
		for (i = 0; i < fMaxExitPoints; i++)
		{
			if (pExits[i].mfGetId() == Id)
			{
				*cpNewSceneName = pExits[i].mfGetSceneName();
				bFound = TRUE;
			}
		}
	}
	
	if (!bFound)
	{
#if defined (_DEBUG)
		fatal_error("ERROR: No Exit data in .scene file for Exit line %ld\n", Id);
#endif
		*cpNewSceneName = 0;
	}
	
	return bFound;
}

#if defined (SCENE_EDITOR)
/* ========================================================================
   Function    - mfWriteAvatars
   Description - Dump the Avatar data to a scene file
   Returns     - void
   ======================================================================== */
void SCENE::mfWriteAvatars(FILE *fp)
{
	LONG i;
	LONG count =0;
	// First count'm
	
	for(i=0; i<MAX_AVATARS; i++)
	{
		if (Avatars[i] != fERROR)
		{
			CAvatar *pAvatar = (CAvatar *) BLKPTR(Avatars[i]);
			if (pAvatar && TRUE == pAvatar->mfShouldSaveToSceneFile())
			{
				count++;
			}
		}
	}
	
	if (count > 0)
	{
		fprintf(fp,"[Avatars]\n");
		fprintf(fp,"%ld\n", count);
		
		for(i=0; i<MAX_AVATARS; i++)
		{
			if (Avatars[i] != fERROR)
			{
				CAvatar *pAvatar = (CAvatar *) BLKPTR(Avatars[i]);
				if (IsPointerGood(pAvatar))
				{
					pAvatar->mfWriteSceneData(fp);
				}
			}
		}
	}
}
#endif // SCENE_EDITOR
#if defined (SCENE_EDITOR)
/* ========================================================================
   Function    - mfWriteExitPoints
   Description - Dump the ExitPoint data to a scene file
   Returns     - void
   ======================================================================== */

void SCENE::mfWriteExitPoints(FILE *fp)
{
	if (fMaxExitPoints > 0 && fhExitPoints != fERROR)
	{
		LONG i;
		LONG count = 0;
		PTR_EXIT_POINTS pExits = (PTR_EXIT_POINTS) BLKPTR(fhExitPoints);
		
		for (i = 0; i < fMaxExitPoints; i++)
		{
			if (pExits[i].mfGetId() != fERROR)
			{
				count++;
			}
		}
		if (count > 0)
		{
			fprintf(fp, "[Exit]\n");
			fprintf(fp, "%ld\n", count);
			
			for (i = 0; i < fMaxExitPoints; i++)
			{
				if (pExits[i].mfGetId() != fERROR)
				{
					pExits[i].mfWriteExitPointData(fp);
				}
			}
		}
	
	}
}
#endif // SCENE_EDITOR
#if defined (SCENE_EDITOR)
/* ========================================================================
   Function    - mfWriteSectors
   Description - Dump the Sector data to a scene file
   Returns     - void
   ======================================================================== */

void SCENE::mfWriteSectors(FILE *fp)
{
	if (fMaxSectors > 0 && fhSectors != fERROR)
	{
		LONG i;
		
		fprintf(fp, "[Sectors]\n");
		fprintf(fp, "%ld", fMaxSectors);
		
		PTR_SCN_SECTORS pSectors = (PTR_SCN_SECTORS) BLKPTR(fhSectors);
		
		for (i = 0; i < fMaxSectors; i++)
		{
			if (pSectors[i].mfGetId() != fERROR)
			{
				pSectors[i].mfWriteSectorData(fp);
			}
			
		}
	
	}
}
#endif // SCENE_EDITOR
#if defined (SCENE_EDITOR)
/* ========================================================================
   Function    - mfWritePlaces
   Description - Dump the Place data to a scene file
   Returns     - void
   ======================================================================== */

void SCENE::mfWritePlaces(FILE *fp)
{
	if (fMaxPlaces > 0 && fhPlaces != fERROR)
	{
		LONG i;
		
		fprintf(fp, "[Places]\n");
		fprintf(fp, "%ld", fMaxPlaces);
		
		PTR_PLACES pPlaces = (PTR_PLACES) BLKPTR(fhPlaces);
		
		for (i = 0; i < fMaxPlaces; i++)
		{
			if (pPlaces[i].ThingIndex != fERROR)
			{
				pPlaces[i].mfWritePlaceData(fp);
			}
		}
	}
}
#endif // SCENE_EDITOR
#if defined (SCENE_EDITOR)
/* ========================================================================
   Function    - mfWriteKeys
   Description - Dump the Key data to a scene file
   Returns     - void
   ======================================================================== */

void SCENE::mfWriteKeys(FILE *fp)
{
	if (fMaxKeys > 0 && fhKeys != fERROR)
	{
		LONG i;
		SHORT count = 0;
		PTR_KEYS pKeys = (PTR_KEYS) BLKPTR(fhKeys);
		
		// Count the number in use.
		for (i = 0; i < fMaxKeys; i++)
		{
			if (pKeys[i] != fERROR)
			{
				PTR_KEY pKey = (PTR_KEY) BLKPTR(pKeys[i]);
				if (pKey->mfGetKeyId() != NO_KEY)
				{
					count++;
				}
			}
		}
		
		if (count > 0)
		{
			fprintf(fp, "[Keys]\n");
			fprintf(fp, "%ld\n", count);
			
			for (i = 0; i < fMaxKeys; i++)
			{
				if (pKeys[i] != fERROR)
				{
					PTR_KEY pKey = (PTR_KEY) BLKPTR(pKeys[i]);
					if (pKey->mfGetKeyId() != NO_KEY)
					{
						pKey->mfWriteKeyData(fp);
					}
				}
			}
		}
	}
}
#endif // SCENE_EDITOR
#if defined (SCENE_EDITOR)
/* ========================================================================
   Function    - mfWriteSceneFile
   Description - Dump the scene data to a scene file
   Returns     - BOOL (successful or not.)
   ======================================================================== */

BOOL SCENE::mfWriteSceneFile()
{
	BOOL bResult = FALSE;
	char cpSceneFileName[20] = {"scenes\\"};
	
	strcat(cpSceneFileName, mfGetSceneName());
	strcat(cpSceneFileName, ".scn");
	
	// If this file already exsists, move it to a backup file.
	if (0 == access(cpSceneFileName, F_OK))
	{
		LONG i;
		char cpSceneFileNameBackup[20];
		
		for (i = 0; i < 99; i++)
		{
			sprintf(cpSceneFileNameBackup, "scenes\\%s.b%.2ld", mfGetSceneName(), i);
			
			if (0 != access(cpSceneFileNameBackup, F_OK))
			{
				rename(cpSceneFileName, cpSceneFileNameBackup);
				break;
			}
		}
	}
	
	FILE *fp = FileOpen(cpSceneFileName, "w");
	if (fp)
	{
		mfWriteSceneFileVersion(fp);
		mfWriteMusicData(fp);
		mfWritePanoramaData(fp);
		mfWriteWadName(fp);
		mfWritePalletName(fp);
		mfWritePlaces(fp);
		mfWriteSectors(fp);
		mfWriteKeys(fp);
		mfWriteDialogs(fp);
		mfWriteExitPoints(fp);
		mfWriteMap(fp);
		mfWriteCameraData(fp);
		mfWriteSelfRunningDemoData(fp);
		mfWriteAvatars(fp);
		
		FileClose(fp);
		bResult = TRUE;
	}
	return bResult;
}
#endif // SCENE_EDITOR


/* ========================================================================
   Function    - DrawMapStuff
   Description - draw the avatars & places on the map
   Returns     -
   ======================================================================== */
void SCENE::mfDrawMapStuff ( void )
{
	LONG i;
	
	if (SCENE_MGR::hCurrentScene != fERROR)
	{
		PTR_SCENE pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
	
 		for(i = 0; i < MAX_AVATARS; i++ )
 		{
 			if(pCurrentScene->Avatars[i] != fERROR )
 			{
 				CAvatar *pAvatar = (CAvatar *) BLKPTR(pCurrentScene->Avatars[i]);
				if (pAvatar->Status!=CAvatar::AI_DEAD)
					HandleMapAvatar(pAvatar->ThingIndex,pCurrentScene->Avatars[i],MAP_RED,MAP_SCALED);
 			}
 		}


#if 0		
		if (pCurrentScene->fMaxPlaces > 0 && pCurrentScene->fhPlaces != fERROR)
		{
			PTR_PLACES pPlaces = (PTR_PLACES) BLKPTR(pCurrentScene->fhPlaces);
			
			for (i = 0; i < pCurrentScene->fMaxPlaces; i++)
			{
				if (pPlaces[i].ThingIndex != fERROR)
				{
					pPlaces[i].mfDrawDiamond();
				}
			}
		}
#endif
	}
}
/* ========================================================================
   Function    - mfAttachAutoAIsAndThingSnds
   Description - After the wad is loaded, run thru and grab any art thing
                 which can have an AutoAI and create the avatar for it and
                 set it's ai.
                 Note: This fn must be called after the wad is loaded and
                       before the [Avatars] section of the scene file is
                       read.
   Returns     -
   ======================================================================== */
void SCENE::mfAttachAutoAIsAndThingSnds ( void )
{
	// You know, somedays I think all this avatar stuff doesn't belong
	// in the scene.cpp file. And then I add more....
	
	LONG i = 0;
	LONG ThingIndex;
	
	// Go thru the wad internal data looking for Avatar art.
	for (ThingIndex = 0;
	     ThingIndex < MAX_THINGS &&
	     (mythings[ThingIndex].valid == TRUE);
	     ThingIndex++)
	{
    	const THINGTYPE ttype = (THINGTYPE) mythings[ThingIndex].OriginalType;
    	
    	if (GAME_TTYPE::mfIsGameType(ttype))
    	{
	    	// GWP Testing for AutoAIPossible maybe all that is required here.
	    	if (GAME_TTYPE::mfIsAvatarType(ttype)
	    		) // GWP DEMO Only && GAME_TTYPE::mfIsAutoAIPossible(ttype))
			{
				// Find the first available Avatar slot.
				// Note: Since I don't reset i, we start looking from the
				//       last open spot we found.
				for (; Avatars[i] != fERROR && i < MAX_AVATARS;i++)
				{};
				
				if (i >= MAX_AVATARS)
				{
					printf("WARNING! Out of Avatars slots while loading AutoAI's!");
					return;	// Stop looking thru the mythings array.
				}
				
				Avatars[i] = CAvatar::NewAvatar();
				if (Avatars[i] != fERROR)
				{
					//GEH SetBlockAttr(Avatars[i], LOCKED, LOCKED);
					CAvatar *pAvatar = (CAvatar *) BLKPTR(Avatars[i]);
					
					if (fERROR != (pAvatar->hPlayerStats = LoadStats(ttype, Avatars[i])))
					{
						PLAYER_STATS *pPlayerStats;
						pPlayerStats = (PLAYER_STATS*)BLKPTR(pAvatar->hPlayerStats);
						pPlayerStats->mfMemorizeAllInvSpells();
						pAvatar->ThingType = pPlayerStats->mfGetType();
					}	
					else
					{
						printf("WARNING! Unable to load Avatar %d\n", ttype);
						pAvatar->DeleteAvatar(pAvatar->hThis);
						Avatars[i] = fERROR;
						i--;
						continue;
					}
					pAvatar->attrib.RuntimeCreated = TRUE;	// Don't save!
					
					// Fix up the CThing Data.
					pAvatar->mfAttachCThing(ThingIndex,
											pAvatar->ThingType,
											0,
											0);
											// GWP CAvatar::mfFakeLeftButton,
											// GWP CAvatar::mfFakeRightButton);
					
					// GNOLLS were accidently made too big, so scale'm down here.
					const LONG ScaleAdjust = (pAvatar->ThingType == GNOLL_1 ||
					                          pAvatar->ThingType == GNOLL_IRREGULAR) ?
					                          80 - (random(10) - 5) : 100 - (random(20) - 10);
					pAvatar->mfChangeScale(ScaleAdjust);
					
					switch (pAvatar->ThingType)
					{
					case GIANT_SPIDER:
					case T_GIANT_SPIDER:
					case CEILING_SPIDER:
					case T_CEILING_SPIDER:
						pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_SPIDERS);
						break;
					case HARPY:
					case T_HARPY:
						pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_HARPIES);
						break;
					case WYVERN_1:
					case T_WYVERN_1:
					case WYVERN_2:
					case T_WYVERN_2:
						pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_WYVERNS);
						break;
					case DOG:
					case T_DOG:
					case HELL_HOUND:
					case T_HELL_HOUND:
						pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_HELLHOUND);
						break;
					case SPECTRE:
					case T_SPECTRE:
					case WRAITH:
					case T_WRAITH:
						pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_SPECTRE);
						break;
					case GARGOYLE_1:
					case T_GARGOYLE_1:
						pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_GARGOYLES);
						break;
					default:
						pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_PING_PONG);
						break;
					}
					
					pAvatar->mfInitVals();
					
					// If the Angle data is bad, just set a reasonable value.
					if (pAvatar->mfAngle() < 0 || pAvatar->mfAngle() > 255)
					{
						pAvatar->FaceTo(PLAYER_INT_VAL(player.x), PLAYER_INT_VAL(player.y));
					}
					
					// Otherwise the monsters will attack the other inhabinates of the scene.
					if (GAME_TTYPE::mfIsMonster(ttype))
					{
						pAvatar->Realm.HomeRealm = REALM::MONSTERS;
					}
					else
					if (ttype == ENDAERAL_CATHBIRN ||
					    ttype == TIESKAR_GRAECHER)
					{
						pAvatar->Realm.HomeRealm = SCENE_MGR::Visitors;
					}
					else
					{
						pAvatar->Realm.HomeRealm = SCENE_MGR::Aggressor;
					}
					pAvatar->mfSetOriginalRemapColor(SCENE_MGR::Aggressor);
					// GWP At some future time get the "Scene Senario Realm value"
					// GWP and set the non monsters to that Realm.
					// GWP they now default to REALM::NO_COUNTRY
					
					//GEH ClrLock(Avatars[i]);
				}
			}
#if !defined(_WINDOWS)
			else
			if(GAME_TTYPE::mfIsLoopingSound(ttype ) )
			{
				AddThingToSoundArray(GAME_TTYPE::mfGetSoundType(ttype), ThingIndex);
			
			}
#endif
		}
	}
}

/* ========================================================================
   Function    - mfGetDialog
   Description - Given a trip line number get the string associated.
   Returns     -
   ======================================================================== */

GAME_STRING const SCENE::mfGetDialog(LONG const TripLineNumber  ) const
{
	GAME_STRING Result = STR_NULL;
	
	if (fMaxDialogTriplines > 0)
	{
		LONG i;
		DumbHandleArray<DIALOG_TRIPLINES> const pDialogTriplines(fhDialogTriplines);
		
		for (i= 0; i< fMaxDialogTriplines; i++)
		{
			if (pDialogTriplines[i].mfMatchTripLineNumber(TripLineNumber) &&
			    pDialogTriplines[i].mfTripLineSeen() == FALSE)
			{
				Result = pDialogTriplines[i].mfGetStringId();
				pDialogTriplines[i].mfSetTripLineSeen();
				break;
			}
		}
	}
	
	return Result;
}

/* ========================================================================
   Function    - mfDeleteDialogs
   Description - Delete the dialog trip line data.
   Returns     -
   ======================================================================== */

void SCENE::mfDeleteDialogs()
{
	if (fMaxDialogTriplines > 0)
	{
		if (fhDialogTriplines != fERROR)
		{
			DisposBlock(fhDialogTriplines);
			fhDialogTriplines = fERROR;
		}
		fMaxDialogTriplines = 0;
	}
}

/* ========================================================================
   Function    - mfReadUserWad
   Description - Read the Wad section of the scene file.
   Returns     -
   ======================================================================== */
LONG mfReadUserWad(FILE *fp, SCENE::SCENE_DATA *SceneData )
{
	char cpBuffer[40];
	
	const LONG Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (Result != EOF)
	{
		strncpy(SceneData->szWadName, cpBuffer, sizeof(SceneData->szWadName));
	}
	
	return Result;
}

/* ========================================================================
   Function    - mfReadUserType
   Description - Read the Type section of the scene file.
   Returns     -
   ======================================================================== */
LONG mfReadUserType(FILE *fp, SCENE::SCENE_DATA *SceneData )
{
	char cpBuffer[80];
	LONG Result;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (Result != EOF)
	{
		LONG	SceneType;
		sscanf(cpBuffer, "%ld", &SceneType);
		SceneData->SceneType = (SCENE::SCENE_TYPE) SceneType;
	}
	return Result;
}

/* ========================================================================
   Function    - mfReadUserDescription
   Description - Read the Description section of the scene file.
   Returns     -
   ======================================================================== */
LONG mfReadUserDescription(FILE *fp, SCENE::SCENE_DATA *SceneData )
{
	char cpBuffer[128];
	
	const LONG Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (Result != EOF)
	{
		strncpy(SceneData->szWadName, cpBuffer, sizeof(SceneData->szWadName));
	}
	
	return Result;
}

/* ========================================================================
   Function    - mfReadUserDifficulty
   Description - Read the Difficulty section of the scene file.
   Returns     -
   ======================================================================== */
LONG mfReadUserDifficulty(FILE *fp, SCENE::SCENE_DATA *SceneData )
{
	char cpBuffer[80];
	LONG Result;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (Result != EOF)
	{
		LONG	SceneDiff;
		sscanf(cpBuffer, "%ld", &SceneDiff);
		SceneData->sSceneDiff = (SHORT)SceneDiff;
	}
	
	return Result;
}

