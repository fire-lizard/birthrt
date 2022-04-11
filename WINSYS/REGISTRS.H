//--------------------------------------------------------------------------------
//
//		Copyright 1995/1996 by NW Synergistic Software, Inc.
//
//		RegistrS.c  - Application-Specific input to the standard Registry
//							routines. 
//
//--------------------------------------------------------------------------------

BOOL fSound;
BOOL fMusic;
extern BOOL fHighRes;
extern BOOL fAIAutoRes;
HKEY hKeyBirthright;

 // -- Registry key
static char szSubKey[] = "Software\\SierraOnLine\\Birthright";

 // -- Number of entries in table 
#define MAX_REGTABLE 4

 // -- Table of variables to register  
REGENTRY RegTable [ MAX_REGTABLE ] = 
{
  // game variables 
//  "PlayerName", REG_SZ,     &GameInfo.szPlayer,           VAR_SZ,    	sizeof ( SZPLAYER ),    0, FALSE,
//  "BirthrightDir",    REG_SZ,     &GameInfo.szScoreDir, 			 VAR_SZ,  		MAX_PATH,  		0, FALSE,
//  "CDDir",	    REG_SZ,     &GameInfo.szLevelDir,			 VAR_SZ,  		MAX_PATH,  		0, FALSE,
//  "DefaultRes", REG_DWORD,	&GameInfo.usArtRes,				 VAR_USHORT,	1,				iLOWRES, TRUE,


  // options flags
  "SoundState",	REG_DWORD,	&fSound,	VAR_BOOL, 1, TRUE, TRUE,
  "MusicState",	REG_DWORD,	&fMusic,	VAR_BOOL, 1, TRUE, TRUE,
  "GraphicMode",REG_DWORD,	&fHighRes,	VAR_BOOL, 1, TRUE, TRUE,
//  "AnimationStatus",REG_DWORD,	&fAIAnimate,	VAR_BOOL, 1, FALSE, TRUE,
  "AutoGraphicsRes",REG_DWORD,	&fAIAutoRes,	VAR_BOOL, 1, TRUE, TRUE,


};


// -- RegistrS.c

