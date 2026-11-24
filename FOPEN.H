// ===========================================================================
// FOPEN.H
// Prototypes for FOPEN functions.
// ===========================================================================

#ifndef FOPEN_H
#define FOPEN_H

#if defined (__cplusplus)
extern "C" {
#endif

void CheckHandles ();
FILE *FileOpen( CSTRPTR pszFile, CSTRPTR pszAccess );
SHORT DiskOpen( CSTRPTR pszFile );
LONG FileAccess(CSTRPTR);
int DiskClose (SHORT handle);
int FileClose (FILE * fp);
void CloseAllFiles ();
#if defined (__cplusplus)
}
#endif
#endif
