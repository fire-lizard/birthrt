/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: slfrdemo.cpp  -
   Author:   Gary Powell

   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes

   ------------------------------------------------------------------------ */
#include <sys\types.h>
#include <direct.h>
#include <io.h>
#include <ctype.h>
#if defined(_VC4)
#define W_OK 2
#endif

#include "system.h"
#include "machine.h"

#include "flicplay.hxx"
#include "gamekey.hxx"
#include "scene.hxx"
#include "scnai.hxx"
#include "scnmgr.hxx"
#include "slfrdemo.hxx"

#if defined(OLD_SOUND)
#if defined(WINDOWS)
#include "sndutils.h"
#endif
#endif

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


   	
SRDEMO_DATA::DUMP_STATE		  SRDEMO_DATA::fDmpState=DUMPOFF;
char   		  SRDEMO_DATA::fDmpPath[40]="PCX";

#if defined(_SELF_RUNNING_DEMO)

class COMMAND_TYPES
{
public:
	char						*cpKeyWord;
	SRDEMO_DATA::COMMAND_TYPE	fCommandType;
	SRDEMO_DATA::COMMAND_STATE	(*pfReadBuffer)(SRDEMO_DATA::CMD_DATA *,char *);
	
	static COMMAND_TYPES	pAvailableCommands[];
};

// These are in the order most likely to be seen in
// the demo file.
COMMAND_TYPES COMMAND_TYPES::pAvailableCommands[] = {
	{"MoveCamera",		SRDEMO_DATA::MOVE_CAMERA,		SRDEMO_DATA::MOVE_CAMERA_CMD::mfReadData},
	{"SwoopCamera",		SRDEMO_DATA::SWOOP_CAMERA,		SRDEMO_DATA::SWOOP_CAMERA_CMD::mfReadData},
	{"PlayWave",		SRDEMO_DATA::PLAY_WAVE,			SRDEMO_DATA::PLAYWAVE_CMD::mfReadData},
	{"StopWave",		SRDEMO_DATA::STOP_WAVE,			SRDEMO_DATA::STOPWAVE_CMD::mfReadData},
	{"SetWaveVolume",	SRDEMO_DATA::SET_WAVE_VOLUME,	SRDEMO_DATA::SETWAVEVOLUME_CMD::mfReadData},
	{"PlayMusic",		SRDEMO_DATA::PLAY_MUSIC,		SRDEMO_DATA::PLAYMUSIC_CMD::mfReadData},
	{"Wait",   			SRDEMO_DATA::WAIT,		  		SRDEMO_DATA::WAIT_CMD::mfReadData},
	{"DisplayPCX",   	SRDEMO_DATA::DISPLAY_PCX,  		SRDEMO_DATA::DISPLAY_PCX_CMD::mfReadData},
	{"Sleep",   	  	SRDEMO_DATA::SLEEP,		  		SRDEMO_DATA::SLEEP_CMD::mfReadData},
	{"SetCamera",		SRDEMO_DATA::SET_CAMERA,		SRDEMO_DATA::SET_CAMERA_CMD::mfReadData},
	{"LoadScene",     	SRDEMO_DATA::LOAD_SCENE,		SRDEMO_DATA::LOAD_SCENE_CMD::mfReadData},
	{"Exit",  		   	SRDEMO_DATA::EXIT,				SRDEMO_DATA::EXIT_CMD::mfReadData},
	{"SaveScene",     	SRDEMO_DATA::SAVE_SCENE,		SRDEMO_DATA::SAVE_SCENE_CMD::mfReadData},
	{"RestoreScene",    SRDEMO_DATA::RESTORE_SCENE, 	SRDEMO_DATA::RESTORE_SCENE_CMD::mfReadData},
	{"StartDump",       SRDEMO_DATA::START_DUMP,        SRDEMO_DATA::START_DUMP_CMD::mfReadData},
	{"StopDump",        SRDEMO_DATA::STOP_DUMP,         SRDEMO_DATA::STOP_DUMP_CMD::mfReadData},
	{"LoadPallet",      SRDEMO_DATA::LOAD_PALLET,       SRDEMO_DATA::LOAD_PALLET_CMD::mfReadData},
	{"RunFlic",         SRDEMO_DATA::RUN_FLIC,          SRDEMO_DATA::RUN_FLIC_CMD::mfReadData},
	{"SetHighRes",      SRDEMO_DATA::SET_HIGH_RES,      SRDEMO_DATA::SET_HIGH_RES_CMD::mfReadData},
	{"SetLowRes",       SRDEMO_DATA::SET_LOW_RES,       SRDEMO_DATA::SET_LOW_RES_CMD::mfReadData},
	{"HideCursor",      SRDEMO_DATA::HIDE_CURSOR,       SRDEMO_DATA::HIDE_CURSOR_CMD::mfReadData},
	{"ShowCursor",      SRDEMO_DATA::SHOW_CURSOR,       SRDEMO_DATA::SHOW_CURSOR_CMD::mfReadData},
};

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
// Wait for a given amount of time to pass.
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::SLEEP_CMD::mfDoCommand()
{
	COMMAND_STATE Result = (fSleepTime > (clock() - fStartTime)) ? RUNNING : DONE;
	
	if (Result == DONE)
	{
		KEYSTRUCT	TempKeyStruct;
		
		replace_key_vals(GAME_KEYS::mfGetKey(GAME_KEYS::SKIP_TO_NEXT_FRAME),
		            fOldSpaceKeyStruct.func,
		            fOldSpaceKeyStruct.val,
		            fOldSpaceKeyStruct.val2,
		            &TempKeyStruct);
		
		SCENE_MGR::fbRender = TRUE;
	}
	return Result;
}

// Initialize the member data.
BOOL SRDEMO_DATA::mfInitVals(char *cpFileName)
{
	char cpFullPath[40];
	BOOL Result;
	
	fCmdType = NO_COMMAND;
	fCmdState = DONE;
	fOldCameraState = SCENE_MGR::fgCamera.mfGetCurrentState();
	fDmpState = DUMPOFF;

	sprintf(cpFullPath, "scenes\\%s.srd", cpFileName);
	ffp = FileOpen (cpFullPath, "r");
	if (ffp)
	{
#if defined (_DEBUG)	// GWP Remove for Demo only
		replace_key_vals(GAME_KEYS::mfGetKey(GAME_KEYS::STOP_RUNNING_DEMO),
					mfCallBackStopTheDemo,
					SCENE_MGR::hCurrentScene,
					0,
					&fOldEscapeKeyStruct);
#endif
					
		SCENE_MGR::fgCamera.mfSetCameraState(GAME_CAMERA::PROGRAM_CAMERA_CONTROL);
		Result =  TRUE;
	}
	else
	{
		Result =  FALSE;
	}
	
	return Result;
}

// This is closes the file and resets the Escape key.
void SRDEMO_DATA::mfDelete()
{
	// Reset the game camera back to its initial state.
	SCENE_MGR::fgCamera.mfSetCameraState(fOldCameraState);
	
	if (ffp)
	{
		FileClose(ffp);
		ffp = 0;
	}
#if defined (_DEBUG)	// GWP Remove for Demo only
	KEYSTRUCT ThisKeyStruct;
	
	replace_key_vals(GAME_KEYS::mfGetKey(GAME_KEYS::STOP_RUNNING_DEMO),
					 fOldEscapeKeyStruct.func,
					 fOldEscapeKeyStruct.val,
					 fOldEscapeKeyStruct.val2,
					 &ThisKeyStruct);
#endif
	DisposBlock(fCmdData.fhSRData);
}

// Read the Exit command and cause the game to shut down gracefully.
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::EXIT_CMD::mfReadData(
	CMD_DATA * /* pfCmdData */,
	char * /* cpBuffer */)
{
	quit_sys(0);
	SCENE_MGR::fbRender = FALSE;
	return NO_MORE_COMMANDS;
}

// Read the sleep command & get started sleeping     Yaaaaawn!.
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::SLEEP_CMD::mfReadData(
	CMD_DATA * pfCmdData,
	char * cpBuffer)
{
	COMMAND_STATE Result = DONE;
	
	if (1 == sscanf(cpBuffer, "%ld", &pfCmdData->fSleepCmdData.fSleepTime))
	{
		pfCmdData->fSleepCmdData.fStartTime = clock();
		
;		pfCmdData->fSleepCmdData.fSleepTime *= 10; // Make it tenths of a second. Since clock returns 100ths
		replace_key_vals(GAME_KEYS::mfGetKey(GAME_KEYS::SKIP_TO_NEXT_FRAME),
					mfCallBackSpace,
					pfCmdData->fhSRData,
					0,
					&pfCmdData->fSleepCmdData.fOldSpaceKeyStruct);
		
		SCENE_MGR::fbRender = FALSE;
		Result = RUNNING;
	}
	return Result;
}

void SRDEMO_DATA::SLEEP_CMD::mfCallBackSpace(LONG hSRData, LONG)
{
	// Rather than duplicate all the end of sleep stuff just set the sleep
	// time to Zero. And let the cleanup happen on the next loop thru.
	
	PTR_SRDEMO_DATA pSRData = (PTR_SRDEMO_DATA) BLKPTR(hSRData);
	{
		pSRData->fCmdData.fSleepCmdData.fSleepTime = 0;
	}
}

// Read the Display PCX command & Display it.
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::DISPLAY_PCX_CMD::mfReadData(
	CMD_DATA * /* pfCmdData */,
	char * cpPCXFileName)
{
	// Show a PCX screen
	const SHORT pcxbitm = GetResourceStd (cpPCXFileName, TRUE);
	if (pcxbitm != fERROR)
	{
		DrawBitmap (0, 0, pcxbitm, 0, 0, 999, 999);
		update_screen();
		//GWP should be a release resource call here.
		SetPurge(pcxbitm);
	}
	return DONE;
}

// Read the load pallet command
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::LOAD_PALLET_CMD::mfReadData(
	CMD_DATA * /* pfCmdData */,
	char * cpPalletFileName)
{
	init_pal(cpPalletFileName);
	init_shade_table(cpPalletFileName);
	
	return DONE;
}

// Read the load pallet command
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::RUN_FLIC_CMD::mfReadData(
	CMD_DATA * /* pfCmdData */,
	char * cpFlicFileName)
{
	PlayFlic(cpFlicFileName, 0);
	return DONE;
}

// Read the set high res command
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::SET_HIGH_RES_CMD::mfReadData(
	CMD_DATA *,
	char * )
{
	set_hires(0,0);
	
	return DONE;
}

// Read the set low res command
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::SET_LOW_RES_CMD::mfReadData(
	CMD_DATA *,
	char * )
{
	set_lowres(0,0);
	
	return DONE;
}

// Read the hide cursor command
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::HIDE_CURSOR_CMD::mfReadData(
	CMD_DATA *,
	char * )
{
	SysHideCursor();
	
	return DONE;
}

// Read the hide cursor command
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::SHOW_CURSOR_CMD::mfReadData(
	CMD_DATA *,
	char * )
{
	SysShowCursor();
	
	return DONE;
}

// Read the wait command & get started.
// This wait is in Frame Ticks!
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::WAIT_CMD::mfReadData(
	CMD_DATA *pfCmdData,
	char * cpBuffer)
{
	COMMAND_STATE Result;
	
	pfCmdData->fWaitCmdData.mfInitVals();
	if (1 == sscanf(cpBuffer, "%hd", &pfCmdData->fWaitCmdData.fsWaitTime))
	{
		Result = RUNNING;
	}
	else
	{
		Result = DONE;
	}
	return Result;
}

// Get the Move camera data and start it moving.
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::MOVE_CAMERA_CMD::mfReadData(
	CMD_DATA *pfCmdData,
	char * cpBuffer)
{
	pfCmdData->fMoveCameraCmdData.mfInitVals();
	if (pfCmdData->fMoveCameraCmdData.fCameraData.mfReadData(cpBuffer))
	{
		pfCmdData->fMoveCameraCmdData.fCameraData.mfMoveCamera();
	}
	return RUNNING;
}

// Get the Move camera data and start it moving.
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::SWOOP_CAMERA_CMD::mfReadData(
	CMD_DATA *pfCmdData,
	char * cpBuffer)
{
	pfCmdData->fSwoopCameraCmdData.mfInitVals();
	if (pfCmdData->fSwoopCameraCmdData.fCameraData.mfReadData(cpBuffer))
	{
		pfCmdData->fSwoopCameraCmdData.fCameraData.mfSwoopCamera();
	}
	return RUNNING;
}

// Set the camera to an exact location.
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::SET_CAMERA_CMD::mfReadData(
	CMD_DATA * /* pfCmdData */,
	char * cpBuffer)
{
	CAMERA_DATA	CameraData;
	
	CameraData.mfInitVals();
	
	if (CameraData.mfReadData(cpBuffer))
	{
		CameraData.mfSetCamera();
	}
	return DONE;
}

// Request to Load a new scene next render.
// GWP It may make sense to allow more commands after this one. At the moment,
//     none will be executed.
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::LOAD_SCENE_CMD::mfReadData(
	CMD_DATA * /* pfCmdData */,
	char * cpBuffer)
{
	char cpSceneName[20];
	LONG PlayerStartNumber = TYPE_PLAYERSTART1;
	COMMAND_STATE Result = DONE;
	
	cpSceneName[0] = 0;
	
	if (2 == sscanf(cpBuffer, "%s %ld", cpSceneName, &PlayerStartNumber))
	{
		SCENE_MGR::mfRequestNewScene(cpSceneName, PlayerStartNumber, FALSE);
		Result = RUNNING;
	}
	return Result;
}

// GWP Implement later.
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::SAVE_SCENE_CMD::mfReadData(
	CMD_DATA * /* pfCmdData */,
	char *  /* cpBuffer */)
{
	return DONE;
}

// GWP Implement later.
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::RESTORE_SCENE_CMD::mfReadData(
	CMD_DATA * /* pfCmdData */,
	char * /* cpBuffer */)
{
	return DONE;
}

// Play a wave file. If the optional command "Loop" is at the end, then cause
// the wave to be played in a loop.
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::PLAYWAVE_CMD::mfReadData(
#if defined(_WINDOWS) && defined(OLD_SOUND)
	CMD_DATA * /* pfCmdData */,
	char *  cpBuffer)
#else
	CMD_DATA * /* pfCmdData */,
	char *  /* cpBuffer */)
#endif
{
#if defined(_WINDOWS) && defined(OLD_SOUND)
	char cpFlag[10];
	USHORT WaveNumber;
	const LONG count = sscanf(cpBuffer, "%hd %s", &WaveNumber, cpFlag);
	
	if ( 2 == count)
	{
		if (0 == strcmp(cpFlag, "Loop"))
		{
			PlayWave(WaveNumber, TRUE);
		}
		else
		{
			PlayWave(WaveNumber, FALSE);
		}
	}
	else
	if (1 == count)
	{
		PlayWave(WaveNumber, FALSE);
	}
#endif
	return DONE;
}

// Stop playing this wave file.
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::STOPWAVE_CMD::mfReadData(
#if defined(_WINDOWS) && defined(OLD_SOUND)
	CMD_DATA * /* pfCmdData */,
	char *  cpBuffer)
#else
	CMD_DATA * /* pfCmdData */,
	char *  /* cpBuffer */)
#endif
{
#if defined(_WINDOWS) && defined(OLD_SOUND)
	USHORT WaveNumber;
	const LONG count = sscanf(cpBuffer, "%hd", &WaveNumber);
	
	if (1 == count)
	{
		PauseWave(WaveNumber);
	}
#endif
	return DONE;
}

// Set the volume for a wave file.
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::SETWAVEVOLUME_CMD::mfReadData(
#if defined(_WINDOWS) && defined(OLD_SOUND)
	CMD_DATA * /* pfCmdData */,
	char *  cpBuffer)
#else
	CMD_DATA * /* pfCmdData */,
	char *  /* cpBuffer */)
#endif
{
#if defined(_WINDOWS) && defined(OLD_SOUND)
	USHORT WaveNumber;
	SHORT WaveVolume;
	
	const LONG count = sscanf(cpBuffer, "%hd %hd", &WaveNumber, &WaveVolume);
	
	if (2 == count)
	{
		SetWaveVolume(WaveNumber, WaveVolume);
	}
#endif
 	return DONE;
}

// Read the Play music command and start the music.
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::PLAYMUSIC_CMD::mfReadData(
#if defined(_WINDOWS) && defined(OLD_SOUND)
	CMD_DATA * /* pfCmdData */,
	char *  cpBuffer)
#else
	CMD_DATA * /* pfCmdData */,
	char *  /* cpBuffer */)
#endif
{
#if defined(_WINDOWS) && defined(OLD_SOUND)
	
	StopMusic();
	FinalMusic();
	InitMusic(cBuffer, ".", FALSE);
	StartMusic();
#endif
	return DONE;
}

SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::START_DUMP_CMD::mfReadData(
	CMD_DATA * /* pfCmddata */,
	char * cpDmpPath)
	
{
	strcpy(fDmpPath, cpDmpPath);
	
	if(0 != access(cpDmpPath, W_OK))
	{
		mkdir(cpDmpPath);
	}
	
	fDmpState = DUMPON;
	return DONE;
}

SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::STOP_DUMP_CMD::mfReadData(
 	CMD_DATA * /* pfCmddata */,
	char * /* cpBuffer */)
{
	fDmpState = DUMPOFF;
	return DONE;
}	
	
/* ========================================================================
   Function    - mfGetNextCommand
   Description - Fn to be called to read and execute the next command from
                 the self running demo file.
   Returns     - State of the commands, either RUNNING, DONE or NO_MORE_COMMANDS
   ======================================================================== */
	
SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::mfGetNextCommand()
{
	char cpBuffer[127];
	
	if (ffp == 0 || EOF == GetNextLine(ffp, cpBuffer, sizeof(cpBuffer)))
	{
		fCmdState = NO_MORE_COMMANDS;
		fCmdType = NO_COMMAND;
	}
	else
	{
		char cpCmd[80];
		
		// Get the command keyword.
		// There are quicker ways to get the keyword and the length but this is the
		// easiest to program.
		if (1 == sscanf (cpBuffer, "%s", cpCmd))
		{
			LONG IndexOfNextArg = strlen(cpCmd);
			
			// Advance the pointer past any leading white space.
			while(isspace(cpBuffer[IndexOfNextArg]) && IndexOfNextArg < 127)
			{
				IndexOfNextArg++;
			}
			
			for (int i= 0; i< (sizeof(COMMAND_TYPES::pAvailableCommands) / sizeof(COMMAND_TYPES)) ; i++)
			{
				if (0 == strcmp(COMMAND_TYPES::pAvailableCommands[i].cpKeyWord, cpCmd))
				{
					fCmdType = COMMAND_TYPES::pAvailableCommands[i].fCommandType;
					fCmdState = (*COMMAND_TYPES::pAvailableCommands[i].pfReadBuffer)(&fCmdData, &cpBuffer[IndexOfNextArg]);
				}
			}
		}
	}
	
	return fCmdState;
}
/* ========================================================================
   Function    - mfCallBackStopTheDemo
   Description - Fn to be called when the ESCAPE key is pressed.
   				 called by the check regions code.
   Returns     - void
   ======================================================================== */
	
void SRDEMO_DATA::mfCallBackStopTheDemo(LONG hCurrentScene,LONG)
{
	if (hCurrentScene != fERROR)
	{
		//GEH SetBlockAttr((SHORT)hCurrentScene, LOCKED, LOCKED);
		PTR_SCENE pScene = (PTR_SCENE) BLKPTR((SHORT) hCurrentScene);
		
		// Let go of the Self Running Demo Scene data.
		if (pScene->fhData != fERROR)
		{
			PTR_SRDEMO_DATA pSrdData = (PTR_SRDEMO_DATA) BLKPTR(pScene->fhData);
			
			pSrdData->mfDelete();
			DisposBlock(pScene->fhData);
			pScene->fhData = fERROR;
			
#if defined (_WINDOWS)
#if defined (OLD_SOUND)
			StopAllWave();
#endif
#endif
		}
		
		// Switch to the other basic Scene AI without tossing all the data.
		pScene->mfSetSceneType(SCENE_AI::GENERIC_SCENE);
		pScene->mfSetSceneState(SCENE_AI::PLAY);
		
		//GEH ClrLock((SHORT)hCurrentScene);
	}
}


void SRDEMO_DATA::dmpToPath()
{
	static SHORT	filenumber = 0;
	static CSTRPTR	szFileName = "scrn0000.pcx";
   	char  n[100];

	sprintf(n, "%s\\%s", fDmpPath, szFileName);

	// dump build buffer to a .pcx file
	PutScreen(iDispBuffer, 0, 0, 640, 480, n);
	
	// increment screen number
	
	if ((++filenumber) % 1000 == 0 )
	{
		szFileName[4]++;
		szFileName[5] = '0';
		szFileName[6] = '0';
		szFileName[7] = '0';
	}
	else if(filenumber%100 == 0)
	{
		szFileName[5]++;
		szFileName[6] = '0';
		szFileName[7] = '0';
	}
	else if(filenumber%10 == 0)
	{
		szFileName[6]++;
		szFileName[7] = '0';
	}
	else
	{
		szFileName[7]++;
	}

}
#endif // _SELF_RUNNING_DEMO
