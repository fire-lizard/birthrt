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

#if defined (_WINDOWS)
#include <Windows.h>
#endif

#include "SYSTEM.H"
#include "ENGINE.H"
#include "MACHINE.H"

#include "HANDLE.HXX"
#include "SCENE.HXX"
#include "SCNKEY.HXX"
#include "SCNMGR.HXX"

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

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
	
/* ========================================================================
   Function    - mfCreateKey
   Description - Make a Key handle and stick it on the keyboard.
   Returns     - HDL to a key
   ======================================================================== */
void KEY::mfCreateKey(char *cpBuffer, HDL_KEY hKey)
{
	char cpKeyIdStr[10];
	
	mfInitVals();
	if ( 1 == sscanf(cpBuffer, "%s", cpKeyIdStr))
	{
		LONG KeyId = NO_KEY;
		
		// Decode the Key Id string.
		// Do this wacky special case code cause no one is going to use all 
		// 128 keys.
		// If later we need lots more keys lets build a lookup table!
		if (strncmp(cpKeyIdStr,"KEY_", 4) == 0 )
		{
			LONG KeyStrLen = strlen(&cpKeyIdStr[4]);
			
			if (KeyStrLen == 1)
			{
				switch(cpKeyIdStr[4])
				{
				case '0':
					KeyId = KEY_0;
					break;
				case '1':
					KeyId = KEY_1;
					break;
				case '2':
					KeyId = KEY_2;
					break;
				case '3':
					KeyId = KEY_3;
					break;
				case '4':
					KeyId = KEY_4;
					break;
				case '5':
					KeyId = KEY_5;
					break;
				case '6':
					KeyId = KEY_6;
					break;
				case '7':
					KeyId = KEY_7;
					break;
				case '8':
					KeyId = KEY_8;
					break;
				case '9':
					KeyId = KEY_9;
					break;
				default:
					printf("Undefined key set. \"%s\"\n", cpKeyIdStr);
					break;
				}
			}
			else
			if (cpKeyIdStr[4] == 'F' && KeyStrLen >= 2 && KeyStrLen < 4)
			{
				switch(cpKeyIdStr[5])
				{
				case '1':
					if (KeyStrLen == 2)
					{
						KeyId = KEY_F1;
					}
					else
					if (KeyStrLen == 3)
					{
						switch (cpKeyIdStr[6])
						{
						case '0':
							KeyId = KEY_F10;
							break;
						
						case '1':
							// GWP KeyId = KEY_F11;
							break;
						
						case '2':
							// GWP KeyId = KEY_F12;
							break;
						}
					}
					break;
				case '2':
					KeyId = KEY_F2;
					break;
				case '3':
					KeyId = KEY_F3;
					break;
				case '4':
					KeyId = KEY_F4;
					break;
				case '5':
					KeyId = KEY_F5;
					break;
				case '6':
					KeyId = KEY_F6;
					break;
				case '7':
					KeyId = KEY_F7;
					break;
				case '8':
					KeyId = KEY_F8;
					break;
				case '9':
					KeyId = KEY_F9;
					break;
				}
			}
			else
			{
				if (strcmp(&cpKeyIdStr[4], "ESCAPE") == 0)
				{
					KeyId = KEY_ESCAPE;
				}
			}
		}
		
		if (KeyId != NO_KEY)
		{
			
			char cpFuncIdStr[20];
			
			// Remove the Keystring.
			cpBuffer = &cpBuffer[strlen(cpKeyIdStr)];
			
			// Strip off leading White space
			int i;
			for(i = 0; isspace(cpBuffer[i]) ;i++)
			{
			}
			
			if (i > 0) // Leading white space found.
			{
				cpBuffer = &cpBuffer[i];
			}
			if (1 == sscanf(cpBuffer, "%s", cpFuncIdStr))
			{
				// strip off the Func Id.
				cpBuffer = &cpBuffer[strlen(cpFuncIdStr)];
			
				if (strcmp(cpFuncIdStr, "SetCamera") == 0)
				{
					if (fCameraData.mfReadData(cpBuffer))
					{
						fKeyId = KeyId;
						replace_key_vals(KeyId, KEY::mfCallBackSetCamera, hKey, 0, &fOldKeyStruct);
					}
					else
					{
						printf("WARNING! Bad arguments for SetCamera \"%s\"\n", cpBuffer);
					}
				}
				else
				if (strcmp(cpFuncIdStr, "MoveCamera") == 0)
				{
					if (fCameraData.mfReadData(cpBuffer))
					{
						fKeyId = KeyId;
						replace_key_vals(KeyId, KEY::mfCallBackMoveCamera, hKey, 0, &fOldKeyStruct);
					}
					else
					{
						printf("WARNING! Bad arguments for MoveCamera \"%s\"\n", cpBuffer);
					}
				}
				else
				if (strcmp(cpFuncIdStr, "LoadScene") == 0)
				{
					// GWP I suspose for debugging we could test for the scene's existance.
					//     and warn the person running the game.
					if (1 == sscanf(cpBuffer, "%s", 
					                 fscInfo.mfGetSceneName()))
					{
						fKeyId = KeyId;
						replace_key_vals(KeyId, KEY::mfCallBackLoadScene, hKey, 0, &fOldKeyStruct);
					}
					else
					{
						printf("WARNING! Bad arguments for LoadScene \"%s\"\n", cpBuffer);
					}
				}
				else
				if (strcmp(cpFuncIdStr, "AttachPlayer") == 0)
				{
					fKeyId = KeyId;
					replace_key_vals(KeyId, KEY::mfCallBackAttachPlayer, hKey, 0, &fOldKeyStruct);
				}
				else
				{
					fKeyId = NO_KEY;
					printf("WARNING! Hot Key function \"%s\" not recognized\n", cpFuncIdStr);
					printf("         Key commands available:\n");
					printf("         SetCamera\n");
					printf("         MoveCamera\n");
					printf("         LoadScene\n");
				}
			
				
				strncpy(fcpKeyIdStr, cpKeyIdStr, sizeof(fcpKeyIdStr) - 1);
				fcpKeyIdStr[sizeof(fcpKeyIdStr) - 1] = 0;
				strncpy(fcpKeyFuncName, cpFuncIdStr, sizeof(fcpKeyFuncName) - 1);
				fcpKeyFuncName[sizeof(fcpKeyFuncName) - 1] = 0;
			}
			else
			{
				fKeyId = NO_KEY;
				printf("WARNING! Incomplete key definition.\n");
			}
		
		}
	}
}
/* ========================================================================
   Function    - mfCreateSetCamera
   Description - Make a SetCamera Key handle and stick it on the keyboard.
   Returns     - HDL to a key
   ======================================================================== */
HDL_KEY KEY::mfCreateSetCamera (char *cpKeyIdStr,
                                              LONG KeyId,
                                              LONG X,
                                              LONG Y,
                                              LONG Z,
                                              LONG Angle,
                                              LONG Pitch,
                                              LONG Roll)
{
	HDL_KEY hKey = fERROR;
	
	hKey = NewBlock(sizeof(KEY));
	
	if (hKey != fERROR)
	{
		DumbAutoLockPtr<KEY> pKey(hKey);
		
		pKey->mfInitVals();
		
		pKey->fKeyId = KeyId;
		pKey->mfChangeCameraLocation(X, Y, Z, Angle, Pitch, Roll);
		
		replace_key_vals(KeyId, KEY::mfCallBackSetCamera, hKey, 0, &pKey->fOldKeyStruct);
		
		strncpy(pKey->fcpKeyIdStr, cpKeyIdStr, sizeof(pKey->fcpKeyIdStr) - 1);
		pKey->fcpKeyIdStr[sizeof(pKey->fcpKeyIdStr) - 1] = 0;
		strncpy(pKey->fcpKeyFuncName, "SetCamera", sizeof(pKey->fcpKeyFuncName) - 1);
		pKey->fcpKeyFuncName[sizeof(pKey->fcpKeyFuncName) - 1] = 0;

	}
	return hKey;
}

/* ========================================================================
   Function    - mfCreateMoveCamera
   Description - Make a SetCamera Key handle and stick it on the keyboard.
   Returns     - HDL to a key
   ======================================================================== */
HDL_KEY KEY::mfCreateMoveCamera (char *cpKeyIdStr,
                                              LONG KeyId,
                                              LONG X,
                                              LONG Y,
                                              LONG Z,
                                              LONG Angle,
                                              LONG Pitch,
                                              LONG Roll)
{
	HDL_KEY hKey = fERROR;
	
	hKey = NewBlock(sizeof(KEY));
	
	if (hKey != fERROR)
	{
		DumbAutoLockPtr<KEY> pKey(hKey);
		
		pKey->mfInitVals();
		
		pKey->fKeyId = KeyId;
		pKey->mfChangeCameraLocation(X, Y, Z, Angle, Pitch, Roll);
		
		replace_key_vals(KeyId, KEY::mfCallBackMoveCamera, hKey, 0, &pKey->fOldKeyStruct);
		
		strncpy(pKey->fcpKeyIdStr, cpKeyIdStr, sizeof(pKey->fcpKeyIdStr) - 1);
		pKey->fcpKeyIdStr[sizeof(pKey->fcpKeyIdStr) - 1 ] = 0;
		strncpy(pKey->fcpKeyFuncName, "MoveCamera", sizeof(pKey->fcpKeyFuncName) - 1);
		pKey->fcpKeyFuncName[sizeof(pKey->fcpKeyFuncName) - 1] = 0;

		
	}
	return hKey;
}


/* ========================================================================
   Function    - mfCallBackSetCamera
   Description - Hot Key fn to set the camera to a new location.
   Returns     - 
   ======================================================================== */

void KEY::mfCallBackSetCamera(LONG hKey, LONG)
{
    fAnyKeyChanged = FALSE;
    if (hKey != fERROR)
    {
		DumbAutoLockPtr<KEY> pKey(hKey);
		
		SCENE_MGR::fgCamera.mfSetCameraState(GAME_CAMERA::WANDER_FREELY);
		pKey->fCameraData.mfSetCamera();
    }
}

/* ========================================================================
   Function    - mfCallBackMoveCamera
   Description - Hot Key fn to move the camera to a new location.
   Returns     - 
   ======================================================================== */

void KEY::mfCallBackMoveCamera(LONG hKey, LONG)
{
    fAnyKeyChanged = FALSE;
    if (hKey != fERROR)
    {
		DumbAutoLockPtr<KEY> pKey(hKey);
		
		SCENE_MGR::fgCamera.mfSetCameraState(GAME_CAMERA::WANDER_FREELY);
		pKey->fCameraData.mfSwoopCamera();
    }
}
/* ========================================================================
   Function    - mfCallBackLoadScene
   Description - Hot Key fn to Change to a new scene file.
   Returns     - 
   ======================================================================== */

void KEY::mfCallBackLoadScene(LONG hKey, LONG)
{
    fAnyKeyChanged = FALSE;
    if (hKey != fERROR)
    {
		PTR_KEY pKey = (PTR_KEY) BLKPTR(hKey);
		
		//GWP Hard code the start location for now. Talk to Greg and decide later
		//    whether to have this driven by the scene file.
		SCENE_MGR::mfRequestNewScene(pKey->fscInfo.mfGetSceneName(), TYPE_PLAYERSTART1, TRUE);
		
		// Note: We can't clear the lock on the key handle here because when we
		//       return from loading the next scene the handle won't be valid 
		//       anymore.
	}
}
/* ========================================================================
   Function    - mfCallBackAttachPlayer
   Description - Hot Key fn to set the camera to a new location.
   Returns     - 
   ======================================================================== */

void KEY::mfCallBackAttachPlayer(LONG, LONG)
{
    fAnyKeyChanged = FALSE;
    SCENE_MGR::fgCamera.mfSetCameraState(GAME_CAMERA::FOLLOW_PLAYER);
}

/* ========================================================================
   Function    - mfWriteKeyData
   Description - Dump the Key data to a scene file
   Returns     - void
   ======================================================================== */

void KEY::mfWriteKeyData(FILE *fp)
{
	fprintf(fp,"%s\t%s\t", fcpKeyIdStr, fcpKeyFuncName);
	if (0 == strcmp("LoadScene", fcpKeyFuncName))
	{
		fscInfo.mfWriteSceneName(fp);
	}
	else
	if ((0 == strcmp("SetCamera", fcpKeyFuncName)) ||
		(0 == strcmp("MoveCamera", fcpKeyFuncName)))
	{
		fCameraData.mfWriteData(fp);
	}
	else
	{
		// AttachPlayer
		fprintf(fp,"\n");
	}
}
