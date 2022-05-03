
/*#######################################################################\
#
#	Synergistic Software
#	(c) Copyright 1993-1997  All Rights Reserved.
#	Author:  Greg Hightower
#
#	JPC Added modifications for a FIFO buffer to keep recently-used files open.
#
\#######################################################################*/
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
#include "TYPEDEFS.H"
#include "SYSTEM.H"
#include "MACHINE.H"
#include "ENGINE.H"

#define F_OK 0

//void (__interrupt __far *prev_int_24)();
void SetRedrawMainMapLevel (void);

// this is the drive letter of the cdrom as in "d:"
extern char CDDrive[4];
extern BOOL fCDAvailible;

// [d3-26-97 JPC]
// Keep track of the files we open with an array of FILEOPENREC structures.
#define MAX_FILE_RECORDS   8           // how many files can be open at once
#define MAX_ACCESS         4           // big enough for anything we use
													// ("rb", "wb", "r", "w")
#define FREE_HANDLES			3				// leave at least this many handles free
#define MAX_TEMP_FILES		(MAX_FILE_RECORDS + FREE_HANDLES)

typedef struct STR_FILEOPENREC {
	char		szFilename[_MAX_PATH];		// _MAX_PATH is big (144), but a smaller
													// number is not guaranteed to work.
	char		szAccess[MAX_ACCESS];
	BOOL		fInUse;
	BOOL		fOpened;
	FILE	*	fp;								// one array tracks both kinds of files
	int		handle;
	LONG		time;
} FILEOPENREC;

static FILEOPENREC gFileRecord[MAX_FILE_RECORDS];

static int	gcMaxRecords = MAX_FILE_RECORDS;

static BOOL gfUseFileSystem = TRUE;		// in case of error, turn off our file
													// system (in the release version)

// Prototypes for static functions.
static void RecordOpen (CSTRPTR pszFile, CSTRPTR pszAccess, FILE * fp, int handle);
static void RecordClose (FILE * fp, int handle);
static void CloseOldestFile ();
static FILE * FileInQueue (CSTRPTR pszFile, CSTRPTR pszAccess);
static SHORT DiskInQueue (CSTRPTR pszFile);

// track file access modes
static FILE_MODE fmCurrentFileMode = FILEOPEN_MIXED_MODE;

#ifndef _WINDOWS
int __far crit_error_handler(unsigned deverr, unsigned errcode, unsigned far *devhdr)
{
	return(_HARDERR_IGNORE);
}
#endif

void SetInt24(void)
{
#ifndef _WINDOWS
	prev_int_24 = _dos_getvect( 0x24 );
	_harderr(crit_error_handler);
#endif
}

void ClearInt24(void)
{
#ifndef _WINDOWS
	_dos_setvect( 0x24, prev_int_24 );
#endif
}

void CheckHandles ()
{
// Call at startup to figure out how many file handles we can have.
// Open a temp file until we have all the records we want or run out
// of handles.
// Make sure there are at least FREE_HANDLES free handles.  Don't use more
// than MAX_FILE_RECORDS in any case.
// Note that gcMaxRecords is initialized to MAX_FILE_RECORDS when it
// is defined, above.

	FILE	*		tempFile[MAX_TEMP_FILES];
	int			i;
	int			iLast;

	iLast = MAX_TEMP_FILES;
	for (i = 0; i < iLast; i++)
	{
		tempFile[i] = tmpfile ();			// use handy library function
		if (tempFile[i] == NULL)
		{
			if (errno == EMFILE)
			{
				iLast = i;
				gcMaxRecords = i - FREE_HANDLES;
				if (gcMaxRecords > MAX_FILE_RECORDS)
					gcMaxRecords = MAX_FILE_RECORDS;
				break;
			}
		}
	}

	for (i = 0; i < iLast; i++)
	{
		fclose (tempFile[i]);
	}
}


// directly replaces the fopen call, first tests the hard drive
// then checks the CDROM
// [d4-01-97 JPC] First check whether file is already in our "open" array.
FILE * FileOpen(
	CSTRPTR pszFile,
	CSTRPTR pszAccess
)
{
	FILE	*pFile = NULL;
	char	chFileName[_MAX_PATH];

	// printf ("FileOpen: %s\n", pszFile);
	// fflush (stdout);
//top:
	run_timers();
	if ((pFile = FileInQueue (pszFile, pszAccess)) != NULL)
	{
		// To avoid surprising the callers, make sure file pointer is
		// at the beginning of the file.
		fseek (pFile, 0, SEEK_SET);
		return pFile;
	}

	/* if file not on local drive, check CDROM */
	if( fmCurrentFileMode == FILEOPEN_MIXED_MODE )
	{
		if ( access( pszFile, ( F_OK ) ) != 0 )
		{
			// if CDROM is available
			if(!CheckCDError())
			{
				if( '\0' != CDDrive[0] )
				#if defined(_MULTIPLAY_ONLY)
					sprintf(chFileName, "%sBIRTHRT\\%s", CDDrive, pszFile);
				#else
					sprintf(chFileName, "%s%s", CDDrive, pszFile);
				#endif
			}
			else
			{
				// printf ("  ERROR: Could not open file, no CD, pFile = %P.\n", pFile);
				return( pFile );
			}
		}
		else
		{
			strcpy(chFileName, pszFile);
		}
	}
	else
	if( fmCurrentFileMode == FILEOPEN_CDROM_ONLY)
	{
		// if CDROM is available
		if(!CheckCDError())
		{
			if( '\0' != CDDrive[0] )
				#if defined(_MULTIPLAY_ONLY)
					sprintf(chFileName, "%sBIRTHRT\\%s", CDDrive, pszFile);
				#else
					sprintf(chFileName, "%s%s", CDDrive, pszFile);
				#endif
		}
		else
		{
			// printf ("  ERROR: Could not open file, no CD, pFile = %P.\n", pFile);
			return( pFile );
		}
	}
	else // fmCurrentFileMode == FILEOPEN_HARDDRIVE_ONLY
	{
		strcpy(chFileName, pszFile);
	}
	
	
	// Returns -1 if file doesn't exist
	if( ( access( chFileName, ( F_OK ) ) ) == 0 )
	{
		// if the fopen fails, NULL is returned
		pFile = fopen( chFileName, pszAccess );

		// If open fails, close a file and try the fopen again.
		// If this fails, we're done for.  Caller needs to handle the error.
		if (pFile == NULL)
		{
			CloseOldestFile ();
			pFile = fopen( chFileName, pszAccess );
		}
		if (pFile != NULL)
			RecordOpen (pszFile, pszAccess, pFile, -1);
	}

	return( pFile );
}

// as above, but replaces the open call
SHORT DiskOpen(
	CSTRPTR pszFile
)
{
	SHORT	iFile;
	char	chFileName[_MAX_PATH];

	if ((iFile = DiskInQueue (pszFile)) != -1)
	{
		// To avoid surprising the callers, make sure file pointer is
		// at the beginning of the file.
		lseek (iFile, 0, SEEK_SET);
		return iFile;
	}

	/* if file not on local drive, check CDROM */
	if( ( access( pszFile, ( F_OK ) ) ) != 0 )
	{
		// if CDROM is available
		if(!CheckCDError())
		{
			if( '\0' != CDDrive[0] )
				#if defined(_MULTIPLAY_ONLY)
					sprintf(chFileName, "%sBIRTHRT\\%s", CDDrive, pszFile);
				#else
					sprintf(chFileName, "%s%s", CDDrive, pszFile);
				#endif
		}
		else
		{
			// printf ("  ERROR: Could not open file, no CD, iFile = %d.\n", iFile);
			return( iFile );
		}
	}
	else
	{
		strcpy(chFileName, pszFile);
	}
	
	// Returns -1 if file doesn't exist
	if( ( access( chFileName, ( F_OK ) ) ) == 0 )
	{
		// if the open fails, -1 is returned
		iFile = open( chFileName, O_BINARY|O_RDONLY );

		// If open fails, close a file and try the open again.
		// If this fails, we're done for.  Caller needs to handle the error.
		if (iFile == -1)
		{
			CloseOldestFile ();
			iFile = open( chFileName, O_BINARY|O_RDONLY );
		}
		if (iFile != -1)
			RecordOpen (pszFile, "", NULL, iFile);
	}

		
	return( iFile );
}

// as above, but replaces the access call
LONG FileAccess(
	CSTRPTR pszFile
)
{
	LONG iFile = -1;
	char	chFileName[_MAX_PATH];
	
	// Check resource files, then local drive, then CD-ROM.
	if (ResourceFileAccess (pszFile) == 0)
	{
		iFile = 0;
	}
	else if( ( access( pszFile, ( F_OK ) ) ) == 0 )
	{
		iFile = 0;
	}
	else
	{
		// if CDROM is available
		if(!CheckCDError())
		{
			if( '\0' != CDDrive[0] )
			{
				#if defined(_MULTIPLAY_ONLY)
					sprintf(chFileName, "%sBIRTHRT\\%s", CDDrive, pszFile);
				#else
					sprintf(chFileName, "%s%s", CDDrive, pszFile);
				#endif
				if( ( access( chFileName, ( F_OK ) ) ) == 0 )
					iFile = 0;
			}
		}
	}
	
	// Returns -1 if file doesn't exist

	return( iFile );
}


int DiskClose (short handle)
{
// [d3-21-97 JPC] Call this function to close a file you opened with DiskOpen.
// TODO: Mark the file handle as closed but don't actually close it
// until we run out of handles.
// Compare FileClose.

	int Result;
	
	if (!gfUseFileSystem)
	{
		Result = close (handle);
	}
	else
	{
		RecordClose (NULL, handle);
		Result = 0;
	}
	return Result;
}


int FileClose (FILE * fp)
{
// [d3-21-97 JPC] Call this function to close a file you opened with FileOpen.
// TODO: Mark the file as closed but don't actually close it
// until we run out of handles.
// Compare DiskClose.

	int Result;
	
	if (!gfUseFileSystem)
	{
		Result = fclose (fp);
	}
	else
	{
		RecordClose (fp, -1);
		Result = 0;
	}
	
	return Result;
}


static LONG FindUnusedRecord ()
{
// Helper of FindOpenRecord.

	LONG			i;

	for (i = 0; i < gcMaxRecords; i++)
	{
		if (gFileRecord[i].fInUse == FALSE)
			return i;
	}

	return -1;
}


static LONG FindOpenRecord ()
{
// Helper of RecordOpen.

	LONG			i;

	i = FindUnusedRecord ();

	if (i == -1)
	{
		// We were able to open and leave open more files than we had slots for,
		// so close a file so we can use the slot.
		CloseOldestFile ();
		i = FindUnusedRecord ();
	}

	return i;
}


static void RecordOpen (CSTRPTR pszFile, CSTRPTR pszAccess, FILE * fp, int handle)
{
// Save information about files we opened.

	LONG			i;

	// return;

	if (!gfUseFileSystem)
		return;

	i = FindOpenRecord ();

	if (i == -1)
	{
#if defined (_DEBUG)
		fatal_error ("Too many open files in FOPEN: RecordOpen");
#else
		return;
#endif
	}

	strncpy (gFileRecord[i].szFilename, pszFile, _MAX_PATH);
	strncpy (gFileRecord[i].szAccess, pszAccess, MAX_ACCESS);
	gFileRecord[i].fInUse  = TRUE;
	gFileRecord[i].fOpened = TRUE;
	gFileRecord[i].fp      = fp;
	gFileRecord[i].handle  = handle;
	gFileRecord[i].time    = get_time ();
	// printf ("RecordOpen: record %d is %s\n", i, pszFile);
}


static void RecordClose (FILE * fp, int handle)
{
	LONG			i;

	if (!gfUseFileSystem)
		return;

	for (i = 0; i < gcMaxRecords; i++)
	{
		if (gFileRecord[i].fInUse &&
			gFileRecord[i].fOpened &&
			gFileRecord[i].fp == fp &&
			gFileRecord[i].handle == handle)
		{
			gFileRecord[i].fOpened = FALSE;
			// printf ("RecordClose: marking record %d (%s) as closed\n",
			// 	i, gFileRecord[i].szFilename);
			break;
		}
	}
}


static void CloseOldestFile ()
{
// The smallest time value is the oldest file.

	LONG			i;
	LONG			iOldest;
	LONG			time;

   // [d4-29-97 JPC] Added + 1 to the following to fix the bug where all
	// of the last-opened files were opened in the current time tick.
	time = get_time() + 1;					
	iOldest = -1;

	// printf ("CloseOldestFile:\n");
	for (i = 0; i < gcMaxRecords; i++)
	{
		// printf ("  %2d (%14s) use: %d open: %d\n",
		// 	i,
		// 	gFileRecord[i].szFilename,
		// 	gFileRecord[i].fInUse,
		// 	gFileRecord[i].fOpened);

		if (gFileRecord[i].fInUse && !gFileRecord[i].fOpened)
		{
			if (gFileRecord[i].time < time)
			{
				time = gFileRecord[i].time;
				iOldest = i;
			}
		}
	}

	if (iOldest >= 0)
	{
		// printf ("  Closing oldest = %d (%s)\n", iOldest, gFileRecord[iOldest].szFilename);

		if (gFileRecord[iOldest].fp != NULL)
		{
			fclose (gFileRecord[iOldest].fp);
		}
		else if (gFileRecord[iOldest].handle != -1)
		{
			close (gFileRecord[iOldest].handle);
		}
#if defined (_DEBUG)
		else
		{
			fatal_error ("Invalid record in FOPEN: CloseOldestFile");
		}
#endif
		gFileRecord[iOldest].fp		 = NULL;
		gFileRecord[iOldest].handle = -1;
		gFileRecord[iOldest].fInUse = FALSE;
	}
	else
	{
#if defined (_DEBUG)
		fatal_error ("Could not find a file to close in FOPEN: CloseOldestFile");
#endif
		gfUseFileSystem = FALSE;
	}
}


static FILE * FileInQueue (CSTRPTR pszFile, CSTRPTR pszAccess)
{
// These are the tests a file must pass to be returned by this function:
//		-	the file name matches (case ignored)
//		-	the access type matches (case ignored)
//		-	the file is NOT currently open.  (If we are trying to open a file
//			that is already open, we presumably want two pointers for the
//			same file--see, for example, LEVEL.C.  It would not be correct
//			to return the existing pointer.)

	LONG			i;
	
	for (i = 0; i < gcMaxRecords; i++)
	{
		if (gFileRecord[i].fInUse && gFileRecord[i].fOpened == FALSE)
		{
			if (strnicmp (pszFile, gFileRecord[i].szFilename, _MAX_PATH) == 0)
			{
				if (strnicmp (pszAccess, gFileRecord[i].szAccess, MAX_ACCESS) == 0)
				{
					if (gFileRecord[i].fp != NULL)
					{
						gFileRecord[i].fOpened = TRUE;
						gFileRecord[i].time    = get_time ();
						// printf ("FileInQueue: returning record %d (%s)\n", i, gFileRecord[i].szFilename);
						return gFileRecord[i].fp;
					}
					else
					{
						gFileRecord[i].fInUse = FALSE;
						// printf ("FileInQueue: record %d (%s) had fp = NULL\n", i, gFileRecord[i].szFilename);
						return NULL;
					}
				}
				else
				{
					// File was opened for different access--close it and
					// return NULL.
					if (gFileRecord[i].fp != NULL)
						fclose (gFileRecord[i].fp);
					gFileRecord[i].fInUse = FALSE;
					// printf ("FileInQueue: record %d (%s) had different access\n", i, gFileRecord[i].szFilename);
					return NULL;
				}
			}
		}
	}

	// printf ("FileInQueue: %s not found\n", pszFile);
	return NULL;								// not found
}


static SHORT DiskInQueue (CSTRPTR pszFile)
{
// Does not check access type because DiskOpen does not use access type.
// These are the tests a file must pass to be returned by this function:
//		-	the file name matches (case ignored)
//		-	the file is NOT currently open.  (If we are trying to open a file
//			that is already open, we presumably want two handles for the
//			same file.  It would not be correct to return the existing handle.)

	LONG			i;
	
	for (i = 0; i < gcMaxRecords; i++)
	{
		if (gFileRecord[i].fInUse && gFileRecord[i].fOpened == FALSE)
		{
			if (strnicmp (pszFile, gFileRecord[i].szFilename, _MAX_PATH) == 0)
			{
				if (gFileRecord[i].handle != -1)
				{
					gFileRecord[i].fOpened = TRUE;
					gFileRecord[i].time    = get_time ();
					// printf ("DiskInQueue: returning record %d (%s)\n", i, gFileRecord[i].szFilename);
					return gFileRecord[i].handle;
				}
				else
				{
					gFileRecord[i].fInUse = FALSE;
					// printf ("DiskInQueue: record %d filename (%s) had handle of -1\n", i, gFileRecord[i].szFilename);
					return -1;
				}
			}
		}
	}

	// printf ("DiskInQueue: %s not found\n", pszFile);
	return -1;									// not found
}


void CloseAllFiles ()
{
// Close all files--intended to be called at shutdown.

	LONG			i;

	for (i = 0; i < gcMaxRecords; i++)
	{
		if (gFileRecord[i].fInUse)
		{
#if defined (_DEBUG)
			printf ("Closing record %d, file %s\n", i, gFileRecord[i].szFilename);
#endif
			if (gFileRecord[i].fp != NULL)
			{
				fclose (gFileRecord[i].fp);
			}
			else if (gFileRecord[i].handle != -1)
			{
				close (gFileRecord[i].handle);
			}
			gFileRecord[i].fInUse = FALSE;
			gFileRecord[i].fOpened = FALSE;
		}
	}
}

/* ========================================================================
   Function    - SetFileMode
	Description - define the locations to look for files
	Returns     - old file mode
	======================================================================== */
FILE_MODE SetFileMode ( FILE_MODE NewFileMode )
{
	FILE_MODE OldMode = fmCurrentFileMode;
	fmCurrentFileMode = NewFileMode;
	return OldMode;
}

