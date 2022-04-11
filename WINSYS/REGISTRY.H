//--------------------------------------------------------------------------------
//
//		Copyright 1995/1996 by NW Synergistic Software, Inc.
//
//		Registry.h  - Registry defines. 
//
//--------------------------------------------------------------------------------

 // -- Prototypes

 // -- Main program routines
BOOL	CloseRegistry	( HKEY  hKey, BOOL fSaveOptions );	// does SaveRegistry also
BOOL	OpenRegistry	( PHKEY phKey );	// does Init/LoadRegistry also

 // -- Companion routines
BOOL	DelRegistry		( void );
BOOL	LoadRegistry	( HKEY  hKey );
BOOL	InitRegistry	( HKEY  hKey );
BOOL	SaveRegistry	( HKEY  hKey, BOOL fSaveOptions );

// Registry.h
