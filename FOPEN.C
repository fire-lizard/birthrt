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
#include "PLATFORM.H"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "TYPEDEFS.H"
#include "SYSTEM.H"
#include "MACHINE.H"
#include "ENGINE.H"

#define F_OK 0

void SetRedrawMainMapLevel (void);

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

/* ------------------------------------------------------------------------
   Languages (see FOPEN.H). Set once at startup from gameopt.dat: the RES
   files and the string table are read once, so a language cannot change
   under a running game.
   ------------------------------------------------------------------------ */
static char const * const gszLanguageFolder[LANGUAGE_COUNT] = {
	NULL, "FRENCH\\", "GERMAN\\", "RUSSIAN1\\", "RUSSIAN2\\"
};
static char const * gszTextOverlay  = NULL;
static char const * gszSoundOverlay = NULL;
static LONG gTextLanguage = LANGUAGE_ENGLISH;

void SetDataLanguages (LONG TextLang, LONG SoundLang)
{
	gTextLanguage   = (TextLang > 0 && TextLang < LANGUAGE_COUNT) ? TextLang : LANGUAGE_ENGLISH;
	gszTextOverlay  = gszLanguageFolder[gTextLanguage];
	gszSoundOverlay = (SoundLang > 0 && SoundLang < LANGUAGE_RUSSIAN2) ? gszLanguageFolder[SoundLang] : NULL;
}

LONG TextLanguage (void)
{
	return gTextLanguage;
}

LONG LangPick (LONG English, LONG French, LONG German)
{
	return (gTextLanguage == LANGUAGE_FRENCH) ? French : (gTextLanguage == LANGUAGE_GERMAN) ? German : English;
}

char const * SoundOverlay (void)
{
	return gszSoundOverlay;
}

/* PlatResolvePath for reading game data: when the chosen language's overlay
   has the file, read that one instead. RESFILES\WAVE.RES follows the sound
   language, everything else the text language; saved games are the
   player's own and never redirected. (Loose sounds are KERNAUD.CPP's.)
   Nor are starting inventories (SCENES\*.INV): they hold no text, and the
   only overlay that has them, RUSSIAN2, carries an older game's item lists. */
static void ResolveDataPath (char * pszResolved, CSTRPTR pszFile, unsigned long len)
{
	CSTRPTR	pszRel = pszFile;
	char const *	pszOverlay;
	size_t	cch;

	if (pszRel[0] == '.' && (pszRel[1] == '\\' || pszRel[1] == '/'))	// InstallPath
		pszRel += 2;
	cch = strlen(pszRel);

	pszOverlay = (stricmp(pszRel, "RESFILES\\WAVE.RES") == 0) ? gszSoundOverlay : gszTextOverlay;

	if (pszOverlay != NULL && cch >= 2 &&
	    pszRel[0] != '\\' && pszRel[0] != '/' && pszRel[1] != ':' &&	// not absolute
	    strnicmp(pszRel, "savegame", 8) != 0 &&
	    !(cch > 4 && stricmp(pszRel + cch - 4, ".INV") == 0))
	{
		char	chOverlay[_MAX_PATH];

		sprintf(chOverlay, "%s%.*s", pszOverlay, (int)(sizeof(chOverlay) - 16), pszRel);
		PlatResolvePath(pszResolved, chOverlay, len);
		if (access(pszResolved, F_OK) == 0)
			return;
	}

	PlatResolvePath(pszResolved, pszFile, len);
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

// directly replaces the fopen call
// [d4-01-97 JPC] First check whether file is already in our "open" array.
FILE * FileOpen(
	CSTRPTR pszFile,
	CSTRPTR pszAccess
)
{
	FILE	*pFile = NULL;
	char	chResolved[_MAX_PATH];

	/* Every path the game passes is Win32-shaped and of arbitrary case.
	   Resolve it here, before the open-file queue is consulted, so the
	   queue, access() and fopen() all agree on one spelling.
	   No-op on Windows - apart from a language overlay, which only reads see. */
	if (strpbrk(pszAccess, "wa+") == NULL)
		ResolveDataPath(chResolved, pszFile, sizeof(chResolved));
	else
		PlatResolvePath(chResolved, pszFile, sizeof(chResolved));
	pszFile = chResolved;

//top:
	run_timers();
	if ((pFile = FileInQueue (pszFile, pszAccess)) != NULL)
	{
		// To avoid surprising the callers, make sure file pointer is
		// at the beginning of the file.
		fseek (pFile, 0, SEEK_SET);
		return pFile;
	}

	// Returns -1 if file doesn't exist
	if( ( access( pszFile, ( F_OK ) ) ) == 0 )
	{
		// if the fopen fails, NULL is returned
		pFile = fopen( pszFile, pszAccess );

		// If open fails, close a file and try the fopen again.
		// If this fails, we're done for.  Caller needs to handle the error.
		if (pFile == NULL)
		{
			CloseOldestFile ();
			pFile = fopen( pszFile, pszAccess );
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
	SHORT	iFile = -1;
	char	chResolved[_MAX_PATH];

	ResolveDataPath(chResolved, pszFile, sizeof(chResolved));	// read-only open
	pszFile = chResolved;

	if ((iFile = DiskInQueue (pszFile)) != -1)
	{
		// To avoid surprising the callers, make sure file pointer is
		// at the beginning of the file.
		lseek (iFile, 0, SEEK_SET);
		return iFile;
	}

	// Returns -1 if file doesn't exist
	if( ( access( pszFile, ( F_OK ) ) ) == 0 )
	{
		// if the open fails, -1 is returned
		iFile = open( pszFile, O_BINARY|O_RDONLY );

		// If open fails, close a file and try the open again.
		// If this fails, we're done for.  Caller needs to handle the error.
		if (iFile == -1)
		{
			CloseOldestFile ();
			iFile = open( pszFile, O_BINARY|O_RDONLY );
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
	char	chResolved[_MAX_PATH];
	char	chData[_MAX_PATH];

	PlatResolvePath(chResolved, pszFile, sizeof(chResolved));
	ResolveDataPath(chData, pszFile, sizeof(chData));

	// Check resource files, then local drive.
	if (ResourceFileAccess (chResolved) == 0)
	{
		iFile = 0;
	}
	else if( ( access( chData, ( F_OK ) ) ) == 0 )
	{
		iFile = 0;
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

	for (i = 0; i < gcMaxRecords; i++)
	{

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
						return gFileRecord[i].fp;
					}
					else
					{
						gFileRecord[i].fInUse = FALSE;
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
					return NULL;
				}
			}
		}
	}

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
					return gFileRecord[i].handle;
				}
				else
				{
					gFileRecord[i].fInUse = FALSE;
					return -1;
				}
			}
		}
	}

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
