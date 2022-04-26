/* ========================================================================
   Copyright (c) 1990,1997	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: filefile.cpp  -
   Author:   Greg Hightower

   ========================================================================

   Contains the following general functions:


   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#ifdef _WINDOWS
#include <Windows.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <dos.h>
#include <conio.h>
#include <errno.h>
#include <string>
#include <vector>
#include "TYPEDEFS.H"
#include "SYSTEM.H"
#include "FOPEN.H"
#include "FINDFILE.HXX"
#include "SCENE.HXX"
#include "MULTIMAP.HXX"
#include "VECTOR.HXX"
#include "REALM.HXX"

using namespace std;

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
   External Global Variables
   ------------------------------------------------------------------------ */
extern DECL_VECTOR_CLASS_S(ADV_SITE,advsite,MAX_SITES);

/* ------------------------------------------------------------------------
   Local Global Variables
   ------------------------------------------------------------------------ */

/* ========================================================================
   Function    - FindUserSceneFiles
	Description - Scan SCENES\ for user scene files
	Returns     - TRUE = found any files, FALSE = no files
	======================================================================== */
BOOL FindUserSceneFiles ( char * pPath, char * pExt )
{
	CHAR				ScenePath[128];
	LONG				cFiles = 0;
	FILE_MODE		oldFileMode;
	SCENE::SCENE_TYPE	SceneType;
	SCENE::SCENE_DATA	SceneData;
	char				SceneNames[MAX_SCENE_FILES][13];
	
	// build path to scene directory with wildcard extention
	sprintf(ScenePath, "%s*%s", pPath, pExt);
	
	vector<string> names;
	string search_path = pPath + string("*") + pExt;
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				names.push_back(fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
	else
	// no files found
		return FALSE;

	// clear name array
	memset( &SceneNames[0][0], 0, MAX_SCENE_FILES*13 );
	
	// open directory and read scene file names
	oldFileMode = SetFileMode( FILEOPEN_HARDDRIVE_ONLY );
	// paw through the whole dir for each scene file
	for (int index = 0; index < names.size(); index++)
	{
		strcpy(&SceneNames[cFiles][0], names[index].c_str());
		
		// check for too many files
		if ( ++cFiles >= MAX_SCENE_FILES )
			break;
			
	}

	// read and fixup scene files array
	SHORT UserSceneCount = MAX_SITES;
	for ( cFiles = 0; cFiles < MAX_SCENE_FILES; cFiles++)
	{
		// done if name is null
		if ( SceneNames[cFiles][0] == 0 )
			break;
			
		// open this file
		sprintf(ScenePath, "%s%s", pPath, &SceneNames[cFiles][0] );
		
		// read the user scene file data
		SceneType = mfReadUserSceneData( ScenePath, &SceneData );
		
		// check to see if this is a user scene file
		// and add this scene to the adventure list
		if ( SceneType == SCENE::USER_SCENE )
		{
			CHAR *Dot;
			// remove extention
			if((Dot = strchr(SceneData.szWadName, '.')) != NULL)
				Dot[0] = 0;
			
			strncpy(advsite[UserSceneCount].name, SceneData.szWadName,8);
			advsite[UserSceneCount].difficulty = SceneData.sSceneDiff;
			advsite[UserSceneCount].available = SELECTABLE;
			advsite[UserSceneCount].realm = REALM::MONSTERS;
			advsite[UserSceneCount].iPlaces = -1;
			
			// next slot
			UserSceneCount++;
		}
		
		// only allow MAX_ADV_SITES scene files
		if ( UserSceneCount >= MAX_ADV_SITES)
			break;
	}
	
	// reset file access mode
	SetFileMode( oldFileMode );
	
	return TRUE;
}

