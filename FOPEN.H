// ===========================================================================
// FOPEN.H
// Prototypes for FOPEN functions.
// ===========================================================================

#ifndef FOPEN_H
#define FOPEN_H

#if defined (__cplusplus)
extern "C" {
#endif

typedef enum {
	FILEOPEN_MIXED_MODE,
	FILEOPEN_HARDDRIVE_ONLY,
	FILEOPEN_CDROM_ONLY
} FILE_MODE;

FILE_MODE SetFileMode( FILE_MODE NewMode );
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

