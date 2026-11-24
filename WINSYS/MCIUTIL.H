//-----------------------------------------------------------------------------
//
//        Copyright 1995/1996 by NW Synergistic Software, Inc.
//
//        MciUtil.c - Provides interface for music
//
//------------------------------------------------------------------------------

//---- Defines  

#define MCI_NOERROR 0         //---- No MCI error returned from MCIUTIL

//---- Publics 
BOOL mci_OpenAVI		( void );
void mci_CloseAVI		( void );
BOOL mci_PlayAVI		( char *szFile, HWND hWnd );
void mci_StopAVI		( void );

DWORD mci_Init			( void );
DWORD mci_OpenFile		( LPSTR lpszFileName );
DWORD mci_OpenDevice	( LPSTR lpszDevName );
DWORD mci_Close			( void );
DWORD mci_Stop			( void );
DWORD mci_Rewind		( void );
DWORD mci_Play			( void );
DWORD mci_GetPosition	( void );

// MciUtil.h
