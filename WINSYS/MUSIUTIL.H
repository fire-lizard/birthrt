//------------------------------------------------------------------------------
//
//		Copyright 1995/1996 by NW Synergistic Software, Inc.
//
//		MusiUtil.H - Provides interface for playing mds files.
//
//------------------------------------------------------------------------------


#if defined (__cplusplus)
extern "C" {
#endif

void InitMusic		( PSZ pszFilename, PSZ pszDir, BOOL fGeneralMidi );
void PauseMusic	( void );
void RestoreMusic	( void );
void StartMusic	( void );
void StopMusic		( void );
void ToggleMusic	( void );
void FinalMusic	( void );

#if defined (__cplusplus)
}
#endif

// MusiUtil.h