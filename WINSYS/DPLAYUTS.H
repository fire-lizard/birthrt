//-----------------------------------------------------------------------------
//
//        Copyright 1995/1996 by NW Synergistic Software, Inc.
//
//        DPlayUtS.c - Provides interface for Direct Play networking.
//
//------------------------------------------------------------------------------

#include "DPlayRes.h"

extern BOOL fExitting;
extern HINSTANCE hInstApp;
extern BOOL fSkipExitQuestion;

//---- Unique Id for the game needed by Direct Play
// --   18025c40-2ff3-11cf-b52e-040000092533
// -- computed by running UUIDGEN.EXE
DEFINE_GUID( GAME_GUID,0x18025c40,0x2ff3,0x11cf,0xb5,0x2e,0x04,0x00,0x00,0x09,0x25,0x33 );

 // -- variables needed
BOOL fModem = FALSE;
BOOL fNetworked = FALSE;

NETPLAYERS NetPlayers;


// DPlayUtS.c
