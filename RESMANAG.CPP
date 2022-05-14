/* ®RM200¯ */
/* ========================================================================
	ResManag.c	- Resource manager routines
   Copyright (c) 1989-1994 by Alan B. Clark
   Right for non-exclusive use by Synergistic Software granted.
	All other rights reserved.
	========================================================================

	Contains the following internal functions:
.		HashCRC			- Uses polynomial division to hash a string into a word
.		ScanIFF			- Scan an IFF file for a mnenonic. Return following long value.

	Contains the following general functions:
.		OpenResFile		- Open a resource type file and create entries in map
.		GetResource		- Load a file, do conversion and decompression

	======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#ifdef _WINDOWS
#include <Windows.h>
#else
#include <i86.h>
#include <bios.h>
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <io.h>
#include <conio.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <string>
#include <algorithm>
#include "SYSTEM.H"
#include "SYSINT.H"
#include "MACHINE.H"

using namespace std;

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

#define	fLOAD_UNKNOWN_TYPE		0
#define	RESFILE_PATH				"RESFILES\\"
#define	RM_IN_MEMORY				0
#define	RM_NOT_IN_MEMORY			1

// #define _CHATTER

/* Define for max number of named resources. */
#define	cMAXRESFILES		32

/* Max number of different extension types handled */
#define	cMAXEXTENTS			10

#define UNSET_RES_EXT		0xFF
// #define UNSET_RES_FILE_EXT	0xFF

#define HASH_COLLSION_FILE	"hashcln.txt"

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
// heap allocations
#ifdef _WINDOWS
	#define malloc(size)	HeapAlloc(hHeap, 0, size)
	#define free(p)			HeapFree(hHeap, 0, p)
	extern HANDLE hHeap;
#endif

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
#define RESUTIL_VERSION		0x00000400
#define NO_COMPRESSION		0				// uncompressed data
#define LZSS_COMPRESSION	2
#define cMAX_RESNAME			13
#define RFF_PCX_UNCOMP		1				// TRUE if PCX uncompressed
// (= converted to BITM format)
#define RFF_ROTATED			2           // TRUE if rotated

// Header for an individual resource.
typedef struct sResourceHeader {
	ULONG		startcode;						// RSRC string for validity check
	ULONG		cbChunk;							// total size of this chunk
	ULONG		cbCompressedData;				// size of compressed data
	ULONG		cbUncompressedData;			// size of uncompressed data
	ULONG		hashValue;						// hash value of file name
	UBYTE		flags;							// [d4-03-97 JPC] new field
	UBYTE		compressionCode;				// 0 = none, 1 = RLE, 2 = LZSS
													// (note: RLE is not currently supported)
	UBYTE		fileExtension;					// index to file extension type
	char		szName[cMAX_RESNAME];		// 8.3 filename (no path)
} RESOURCE_HEADER;

// Header for a resource file.
typedef struct strResfileHeader {
	ULONG    versionResFile;            // resource file version
	ULONG    oDirectory;                // offset to directory from beginning of file
	ULONG    cResources;                // total resources in file
} RESFILE_HEADER;

// Directory consists of an array of DIRENTRY structures.
typedef struct strDirEntry {
	ULONG		hashValue;						// hash value of file name
	ULONG		resOffset;						// offset to resource from beginning of file
	UBYTE		fileExtension;					// index to file extension type
	char		szName[cMAX_RESNAME];		// 8.3 filename (no path)
} DIRENTRY, * DIRENTRY_PTR;

static SHORT LoadUFF (CSTRPTR szFileName, BOOL fNoScale, BOOL fLockRes, BOOL fRotated, LONG iResFileSlot);
SHORT LoadCompressedDataFromResFile (
	RESOURCE_HEADER * pResHeader,
	LONG iResFileSlot,
	BOOL fLockRes);
SHORT LoadPCX (CSTRPTR szFileName, BOOL fNoScale, BOOL fLockRes, BOOL fRotated, LONG iResFileSlot);
SHORT LoadFLC (CSTRPTR szFileName, BOOL fNoScale, BOOL fLockRes, BOOL fRotated, LONG iResFileSlot);
SHORT LoadLump (
	CSTRPTR szFileName,
	BOOL /* fSetPal */,
	BOOL /* fLockRes */,
	BOOL /* fRotated */,
	LONG /* iResFileSlot */);

#if fUSE_RES_FILES
// [d3-06-97 JPC]
LONG FindExtensionIndex (CSTRPTR szResName);
SHORT FindResourceSlot (CSTRPTR szResName, LONG iExtIndex, LONG * piSlot, LONG * piResFileSlot, BOOL fResfileRequired);
SHORT LoadPCXFromResFile (CSTRPTR szFileName, BOOL fNoScale, BOOL fLockRes, BOOL fRotated, LONG iResFileSlot);
SHORT LoadFLCFromResFile (LONG iResFileSlot, BOOL fLockRes);
#endif

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
extern UBYTE antia_table[];


ULONG		*lResNameCRC;	/* hash of name of resource */
SHORT		*iResBlock;		/* index number of the memmanag block for this resource */
UBYTE		*iResExtIndex;	/* index into ResExtentions */

LONG  iMaxResSlotsUsed = 0;
LONG  iMaxResSlots = 0;
LONG  iMaxResSlotsInUse = 0;

#if fUSE_RES_FILES
ULONG    *gcbResOffset;                // offset into file for resource
UBYTE    *giResFileNames;              // index into gszResFileNames
UBYTE		*gResFlags;

// Instructions: plug in the names of the basic RES files here.
// We do not search for .RES files.
char *	gszBasicResFiles[] = {
	"UI.RES",								// everything from UI directory
	"TEXTURES.RES",							// wall and other textures for WADs
	"FONT.RES",								// Fonts
	"MAP.RES",								// The game map
	"LOWRES.RES",							// Ultra low res animations
	"MEDFTRES.RES",							// Medium res Fight animations
	"MEDRTRES.RES",							// Medium res (Rest of)animations
	"HIGHRES.RES",							// High res animations
	"THINGPCX.RES",							// Thing PCX's
	"FINALE.RES",							// Finale PCX's
	""
};
	
char  gszResFileNames[cMAXRESFILES][13];// resource file names
BOOL  gfResFileInUse[cMAXRESFILES];     // flag whether any resource file is
                                        // using this slot

#endif

int		file;
// GWP BAD CODE!
BOOL	fNewRes;

RESTYPE	ResExtentions[cMAXEXTENTS] = {
	{ "", 	 LoadUFF, DisposRes, SetPurgeRes, ClrPurgeRes, HashCRC },
	{ "PCX", LoadPCX, DisposRes, SetPurgeRes, ClrPurgeRes, HashCRC },
	{ "FLC", LoadFLC, DisposRes, SetPurgeRes, ClrPurgeRes, HashCRC },
	{ "WAV", LoadLump, DisposRes, SetPurgeRes, ClrPurgeRes, HashID },
	{  NULL, NULL, NULL, NULL, NULL, NULL },
	{  NULL, NULL, NULL, NULL, NULL, NULL },
	{  NULL, NULL, NULL, NULL, NULL, NULL },
	{  NULL, NULL, NULL, NULL, NULL, NULL },
	{  NULL, NULL, NULL, NULL, NULL, NULL },
	{  NULL, NULL, NULL, NULL, NULL, NULL }
};

#if 01
// Get some idea of the size of a WAD-specific RES file.
FILE *gFile;
#endif

/* ========================================================================
	HashCRC	- Uses polynomial division to hash a string into a word
	======================================================================== */
ULONG HashCRC(register CSTRPTR szIn)
{
	CSTRPTR				sz;
	register ULONG		c;
	register ULONG		accumCRC = 0;
	USHORT				accumXOR = 0;

	// hashing JUST the filename and extension is better
	sz = strrchr(szIn, '\\');
	if (sz == NULL)
		sz = szIn;		// no path character (\)
	else
		sz++;				// next character after path char (\)

	do
	{
		c = *sz++;

		// make uppercase
		if (c >= 'a' && c <= 'z')
		{
			c -= ('a' - 'A');
		}

		// yes, I know this only uses the bottom byte.
		accumXOR = accumXOR ^ c;
		
		accumCRC = (accumCRC << 6) + ((c - ' ') & 63);

		// Unrolled for loop
		if (accumCRC & (1 << 21))
			accumCRC ^= 0x1021 << 5;

		if (accumCRC & (1 << 20))
			accumCRC ^= 0x1021 << 4;

		if (accumCRC & (1 << 19))
			accumCRC ^= 0x1021 << 3;

		if (accumCRC & (1 << 18))
			accumCRC ^= 0x1021 << 2;

		if (accumCRC & (1 << 17))
			accumCRC ^= 0x1021 << 1;

		if (accumCRC & (1 << 16))
			accumCRC ^= 0x1021;

	} while (c);

	return (BUILD_LONG(accumXOR,(USHORT)(accumCRC & 0xFFFF)));
}

/* =======================================================================
	Create or update an entry in the Resource File Extension Handler table
	======================================================================= */
SHORT	RegisterResExtention (
	CSTRPTR sExtension,
	PFRESLOAD LoadProc,
	PFRESDISPOSE DisposeProc,
	PFRESSETPURGE SetPurgeProc,
	PFRESCLRPURGE ClrPurgeProc,
	PFRESHASH HashProc )
{
	LONG	i;

	if (fReport & fREPORT_RESMGR)
		printf("Entering RegisterResExtention\n");

	/* use only upper case names */
	strupr(sExtension);

	/* find a match to over-ride or an empty slot */
	for (i=0; i<cMAXEXTENTS; i++)
	{
		if (NULL == ResExtentions[i].pfLoadProc)
			break;			/* found empty slot first */

		if (0 == stricmp(ResExtentions[i].achExtension, sExtension))
			break;			/* found existing extension, over-write values */
	}

	if (i >= cMAXEXTENTS)
		return fERROR;		/* no available slots */

	if (fReport & fREPORT_RESMGR)
		printf("Res extension registered: %s\n", sExtension);

	strcpy(ResExtentions[i].achExtension, sExtension);
	ResExtentions[i].pfLoadProc = LoadProc;
	ResExtentions[i].pfDisposeProc = DisposeProc;
	ResExtentions[i].pfSetPurgeProc = SetPurgeProc;
	ResExtentions[i].pfClrPurgeProc = ClrPurgeProc;
	ResExtentions[i].pfHashProc = HashProc;

	return fNOERR;
}



#if fUSE_RES_FILES
// ---------------------------------------------------------------------------
// Function		-	ProcessUnknownExtension
//	Description	-	Look for a registered extension that matches this
//                resource's extension.
//	Returns		-	TRUE if we find such an extension.
//                FALSE if this resource has no extension or we don't find
//                a match.  (In that case, the resource file is wrong or else
//                the extension type was not registered soon enough.
//                Extensions should be registered in init_game.)
// Notes:
//	This function is really for future use because we only know how to deal
// with PCX and FLC resources.  As we add more resources to resource files,
// we have to edit the load functions for those sorts of resources.
// ---------------------------------------------------------------------------
static BOOL ProcessUnknownExtension (LONG iSlot, DIRENTRY_PTR pDirentry)
{
	LONG			i;
	char *		szExt;

	szExt = strrchr (pDirentry->szName, '.');
	if (szExt == NULL)
		return FALSE;
	else
		szExt++;

	for (i = 0; i < cMAXEXTENTS; i++)
	{
		if (0 == stricmp(ResExtentions[i].achExtension, szExt))
		{
			iResExtIndex[iSlot] = i;
			return TRUE;
		}
	}
	return FALSE;
}

// ---------------------------------------------------------------------------
// Function		-	GetOpenResFileSlot
//	Description	-	Find an open res file slot.  Do this because resource files
//                will be copied from CD to hard disk for adventures, and
//                then thrown away when no longer needed.
//	Returns		-	Index to open slot.
// ---------------------------------------------------------------------------
LONG GetOpenResFileSlot ()
{
	LONG			i;

	for (i = 1; i < cMAXRESFILES; i++)
	{
		if (!gfResFileInUse[i])
			return i;
	}

	return cMAXRESFILES;						// indicates failure
}


// ---------------------------------------------------------------------------
// Function		-	OpenResFile_
//	Description	-	Opens a specified resource file and scans through it to
//                record what resources are in it.
//	Returns		-	fNOERR if it opened correctly, fERROR if not.
// Notes:
//	The following variables and arrays are altered:
// 		ULONG	lResNameCRC[]    - lResNameCRC is copied to here
// 		ULONG	gcbResOffset[]   - offset into the RES file for the resource
// 		UBYTE	giResFileNames[] - the index into gszResFileNames[] for this res file
// 		WORD	iResBlock[]      - set to fERROR
// ---------------------------------------------------------------------------
SHORT OpenResFile_ (CSTRPTR szFileName)
{
	RESFILE_HEADER	rfHeader;
	LONG		iFileName;
	LONG		i;
	int		file;
	char		szPathname[_MAX_PATH];

	if (fReport & fREPORT_RESMGR)
		printf("Entered OpenResFile - opening file %s\n", szFileName);

	// the wave file depends on language in use
	if(0 == strncmp("wave", szFileName,4) )
		sprintf (szPathname, "%s%s%s", InstallPath, RESFILE_PATH, szFileName);
	else
		sprintf (szPathname, "%s%s", RESFILE_PATH, szFileName);
	file = DiskOpen(szPathname);	/* try to open the file */
	if (file == fERROR)
	{
		return (SHORT)fERROR;
	}

	/* set an array element to the new res file */
	iFileName = GetOpenResFileSlot ();
	if (iFileName >= cMAXRESFILES)
	{
		DiskClose (file);
#if defined (_DEBUG)
		fatal_error("RESMANAG ERROR - no more file slots available\n");
#endif
		return (SHORT)fERROR;
	}

	strncpy (gszResFileNames[iFileName], szFileName, 12);
	gfResFileInUse[iFileName] = TRUE;
	lseek (file, 0, SEEK_SET);
	read (file, &rfHeader, sizeof (rfHeader));
	if (rfHeader.versionResFile != RESUTIL_VERSION)
	{
		DiskClose (file);
#if defined (_DEBUG)
		fatal_error("RESMANAG ERROR - incorrect version\n");
#endif
		return (SHORT)fERROR;
	}

	lseek (file, rfHeader.oDirectory, SEEK_SET);

	for (i = 0; i < rfHeader.cResources; i++)
	{
		LONG	StartLocation;
		LONG	iSlot;
		DIRENTRY direntry;

		read (file, &direntry, sizeof (direntry));
		iSlot = StartLocation = direntry.hashValue % iMaxResSlots;
		do
		{
			if (iResExtIndex[iSlot] == UNSET_RES_EXT)		 // not in use.
			{
				goto UseThisSlot;
			}
#if defined (_DEBUG)
			// [d4-30-97 JPC] Added this to check for and report collisions.
			else
			{
				UBYTE iOldExtension;

				if (direntry.fileExtension == 0)
				{
					int	 i;
					char * szExt;
				
					szExt = strrchr (direntry.szName, '.');
					if (szExt == NULL)
						fatal_error ("Attempted to register %s with no extension",
							direntry.szName);
					else
						szExt++;
				
					for (i = 0; i < cMAXEXTENTS; i++)
					{
						if (0 == stricmp(ResExtentions[i].achExtension, szExt))
						{
							iOldExtension = (UBYTE) i;
							break;
						}
					}
					fatal_error ("Attempted to register %s with invalid extension",
						direntry.szName);
				}
				else
				{
					iOldExtension = direntry.fileExtension;
				}

	            if (iResExtIndex[iSlot] == iOldExtension &&
	                (direntry.hashValue == lResNameCRC[iSlot]))
				{
					FILE *fp;
					int	fileColision;
					// Some other resource with the same extension and hash 
					// already occupies this slot.
					char szOtherPathname[_MAX_PATH];
					RESOURCE_HEADER resHeader;

					iFileName = giResFileNames[iSlot];
					if (iFileName)
					{
						if(0 == strncmp("wave", gszResFileNames[iFileName],4) )
							sprintf (szOtherPathname, "%s%s%s", InstallPath, RESFILE_PATH, gszResFileNames[iFileName]);
						else
							sprintf (szOtherPathname, "%s%s", RESFILE_PATH, gszResFileNames[iFileName]);
						fileColision = DiskOpen (szOtherPathname);		// try to open the file
						if (fileColision == fERROR)					// could not open file
							fatal_error ("Could not open RES file %s", szOtherPathname);
						lseek (fileColision, gcbResOffset[iSlot], SEEK_SET);
						read (fileColision, &resHeader, sizeof (RESOURCE_HEADER));
						DiskClose (fileColision);
	
						fp = fopen(HASH_COLLSION_FILE, "a");
						if (fp)
						{
							fprintf (fp,"Hash collision between resources %s: %s and %s: %s\n",
										szOtherPathname,
										resHeader.szName,
										szPathname,
										direntry.szName);
							fclose(fp);
						}
					}
					else
					{
						fp = fopen("hashcln.txt", "a");
						if (fp)
						{
							fprintf (fp,"Collision of resources. Probably using Resource %s before loading it from resource file %s.\n",
											direntry.szName,
											szPathname);
							fclose(fp);
						}
					}
				}
			}
#endif
			
			iSlot++;
			if (iSlot >= iMaxResSlots)
			{
				iSlot = 0;
			}
			
		} while ( iSlot != StartLocation );
		
#if defined (_DEBUG)
		fatal_error("RESMANAG ERROR! - no more resource slots available\n");
#endif
		return (SHORT)fERROR;

UseThisSlot:

#if 1
		// [d3-25-97 JPC] I suppose we want to record this information:
		iMaxResSlotsUsed++;
		iMaxResSlotsInUse++;
#endif

		giResFileNames[iSlot] = (UBYTE)iFileName;	/* set res file index */
      iResBlock[iSlot] = fERROR;          /* init resblock var */
		lResNameCRC[iSlot] = direntry.hashValue;
      gcbResOffset[iSlot] = direntry.resOffset;

		//	Compression codes: 0 = none, 1 = RLE, 2 = LZSS.
		// NOTE: RLE is not currently supported.

		// NOTE: The only known resource extensions when we build the RES files
		// are 1 (PCX) and 2 (FLC).  The "registered" resource types are added
		// dynamically, so we have to add special code to allow for such types.
		if (direntry.fileExtension == 0)
		{
			if (!ProcessUnknownExtension (iSlot, &direntry))
			// Error out if we encounter an unknown extension type in the resfile.
			// All extensions must be registered BEFORE we open any resfiles.
			{
#if defined (_DEBUG)
				fatal_error ("RESMANAG ERROR! Unregistered extension for resource %s in file %s\n", direntry.szName, szFileName);
#endif		
				giResFileNames[iSlot] = 0;
			}
		}
		else
		{
			// A known extension.
			iResExtIndex[iSlot] = direntry.fileExtension;
		}
	}
	DiskClose (file);
	return fNOERR;
}


// ---------------------------------------------------------------------------
// Function		-	ScanResFiles
//	Description	-	Called from init_game in GAME.CPP to open the basic
//                resource files that should be on hard disk and open
//                during the entire run of the game.
// Note			-	Call this AFTER registering all extensions.
// ---------------------------------------------------------------------------
void ScanResFiles ()
{
	int			i;

	for (i = 0; i < cMAXRESFILES - 1; i++)
	{
		if (strlen (gszBasicResFiles[i]) != 0)
			OpenResFile_ (gszBasicResFiles[i]);
		else
			break;
	}
}

#endif

// ===========================================================================
// Function    -	FindExtensionIndex
// Description -	Searches ResExtentions array for a match to this resource's extension
// Returns     -	Index to correct slot in ResExtentions array or
//						fERROR if not found.
// Notes			-	Slot 0 of ResExtentions (no extension) does not blow up
//                but does not load a resource.  See LoadUFF.
// [d3-25-97 JPC]
// ===========================================================================
static LONG FindExtensionIndex (CSTRPTR szResName)
{
	CSTRPTR szExtension = strrchr(szResName, '.');

	if (szExtension != NULL)
	{
		szExtension++;
		if (*szExtension !=0)
		{
			string ext(szExtension);
			transform(ext.begin(), ext.end(), ext.begin(), toupper);
			for (SHORT iExtIndex = cMAXEXTENTS-1; iExtIndex > 0; iExtIndex--)
				if (0 == ext.compare(ResExtentions[iExtIndex].achExtension))
					return iExtIndex;
		}
		return fERROR;
	}
	else
	{
		return 0;
	}
}


// ===========================================================================
// Function    -	FindResourceSlot
// Description -	Look for a slot in the resmanag arrays for this resource.
//                Check whether the resource is already loaded.
// Returns     -	fERROR if there's an error
//                RM_IN_MEMORY if it finds the resource
//						RM_NOT_IN_MEMORY if DO we need to load the resource from disk,
//                   in which case:
//                   *piSlot = the slot in the resource arrays to use
//                   *piResFileSlot = resource file slot to use, if there is
//                      one, else unchanged.
// Notes			-	Pulled from _GetResource_ so TestAndLoadFromResFile can
//                use it too.
// [d3-25-97 JPC]
// ===========================================================================
static SHORT FindResourceSlot (
	CSTRPTR szResName,
	LONG iExtIndex,
	LONG * piSlot,
	LONG * piResFileSlot,
	BOOL fResfileRequired)
{
	ULONG		lNameHash;
	LONG		iSlot = 0;
	SHORT		iBlk = 0;
   UBYTE		iResIndex = 0;             /* zero the res file index */
	LONG		StartLocation;
	BOOL		FoundPossiblei = FALSE;
	SHORT		Possiblei;

	// GWP BAD CODE!
	fNewRes = FALSE;			/* default to resource already in memory */

	/* use CRC to hash the name into a ULONG */
	lNameHash = (*ResExtentions[iExtIndex].pfHashProc)(szResName);

	/* find the reference to the resource */
	
	// GWP Use the hash as the starting location.
	StartLocation = lNameHash % iMaxResSlots;
	iSlot = StartLocation = ABS(StartLocation);		// Could be a negative number
	Possiblei = StartLocation;
	FoundPossiblei = FALSE;
	
	// Note: We stop looking thru the array at the first UNSET_RES_EXT
	//		 because if the slot we hashed to is not available,
	//     we chain to the first available slot after the one
	//     we hashed to.
	// Also note that if resources are in res files, then the iResExtIndex[iSlot]
	//     value will be an index to the extension type, and will never be
	//     UNSET_RES_EXT.
	do
	{
		if (iResExtIndex[iSlot] == UNSET_RES_EXT)		 // not in use.
		{
			if (FoundPossiblei == FALSE)
			{
				// Remember this hole.
				FoundPossiblei = TRUE;
				Possiblei = iSlot;
			}
			break;
		}
		else
		if (lNameHash == lResNameCRC[iSlot] && iExtIndex == iResExtIndex[iSlot])
		{
			// We now know that the resource name hashed to the same value
			// as an item in the map, AND it's the same type of file (PCX,
			// FLC, etc).  We assume from this that it's the same data,
			// although collisions can occur.  We fix such collisions by
			// renaming source files.

			// See whether the data is now in memory.

			iBlk = iResBlock[iSlot];

			if (iBlk == fFILE_NOT_FOUND)		/* check for previously not found file */
			{
				*piSlot = fERROR;
				return fERROR;
			}

			if (iBlk > 0)	// [ABC] was !=fERROR  // test for resource currently in memory
			{
		#if fUSE_RES_FILES
				if ((gResFlags[iSlot] & RM_PURGABLE) != RM_PURGABLE)
				{
					(*ResExtentions[iExtIndex].pfClrPurgeProc)(iSlot, iBlk);
				}
		#else
				(*ResExtentions[iExtIndex].pfClrPurgeProc)(iSlot, iBlk);
		#endif
            // It is in memory, so just set the resource handle and return TRUE.
				*piSlot = iSlot;   	
				return RM_IN_MEMORY;
			}

			// If we get here, resource was in map but is not currently in memory.
			// See whether it's in a resource file.
	         iResIndex = giResFileNames[iSlot];  /* index into filename array */
	         if (iResIndex != 0)                 /* iResIndex==0 means not in a .RES file */
			{
				*piResFileSlot = iSlot;
			}
			else if (fResfileRequired)
			{
            return RM_NOT_IN_MEMORY;   // note that we don't change iResFileSlot
			}
			goto ExitFromFunction;
		}
		
		iSlot++;
		if (iSlot >= iMaxResSlots)
			iSlot = 0;
			
	} while (iSlot != StartLocation);

	// If we require the resource to be already in a .RES file then just exit.
	if (fResfileRequired)
	{
		return RM_NOT_IN_MEMORY;
	}
	
	// If we get here, the resource was not loaded.
	// Did we find a hole in our search that we can use?
	if (FoundPossiblei == TRUE)
	{
		iSlot = Possiblei;
		goto UseThisSlot;
	}
	
	/* All Handles are in use. Check for slot that we can get rid of. */
	
	// GWP Added disposing of multi user, moldy and purgable resources.
	// Look for the first purgable resource and toss it.
	// Note: You should have more resource blks than resources. Otherwise
	//		 when we purge the memory your resource handle is invalid and we
	//		 can't update it.
	
	iSlot = StartLocation;
	do
	{
		// Look for a not Multi user, moldy and purgable resource and dispose
		// of it.
		SHORT const TestBlk = iResBlock[iSlot];
		
		if (TestBlk > 0)
		{
			if (!IsBlockMultiUser(TestBlk))
			{
				if (IsBlockPurgable(TestBlk))
				{
					if (FoundPossiblei == FALSE)
					{
						// Remember this hole.
						FoundPossiblei = TRUE;
						Possiblei = iSlot;
					}
				
					if (IsBlockMoldy(TestBlk))
					{
						(*ResExtentions[iExtIndex].pfDisposeProc)(iSlot, TestBlk);
						// Use this slot.
						goto UseThisSlot;
					}
				}
			}
		}
		
		iSlot++;
		if (iSlot >= iMaxResSlots)
			iSlot = 0;
			
	} while (iSlot != StartLocation);
	
	// If we got here, all the resources were used this pass, but
	// Hey! the program has to run, so toss a purge'able block.
	// Did we find a hole in our Moldy & Purge'able search that we can use?
	if (FoundPossiblei == TRUE)
	{
		iSlot = Possiblei;
		(*ResExtentions[iExtIndex].pfDisposeProc)(iSlot, iResBlock[iSlot]);
		goto UseThisSlot;
	}
	
	// Well I tried, and you have to increase the number of resource blks.
	// Oh well.
	
#if defined(_DEBUG)
	fatal_error("RESMANAG ERROR! GR - no more resource slots available\n");
#endif
	*piSlot = (SHORT)fERROR;
	return fERROR;
	

UseThisSlot:
	iMaxResSlotsUsed++;
	iMaxResSlotsInUse++;
	lResNameCRC[iSlot] = lNameHash;
	iResBlock[iSlot] = (SHORT)fERROR;
	iResExtIndex[iSlot] = iExtIndex;
#if fUSE_RES_FILES
	// If you get here, resource was not in a .RES file, so update resfile
	// arrays accordingly.
	gcbResOffset[iSlot] = 0;
	giResFileNames[iSlot] = 0;
#endif

	run_timers();  // cdb 11/27

ExitFromFunction:
	*piSlot = iSlot;
	return RM_NOT_IN_MEMORY;
}


// ===========================================================================
// Function    -	TestAndLoadFromResFile
// Description -	Loads a named resource if it is in a resource file.
// Returns     -	Handle to the resource block containing the data.
//              	fERROR if resource is not in a resource file.
// Notes			-	This is for the smaller WAV files.  What you get back
//                is a memory image of the file on disk.  Therefore,
//                this is not an appropriate function to call for PCX
//                or FLC files.
// [d3-25-97 JPC]
// ===========================================================================
SHORT _TestAndLoadFromResFile_ (CSTRPTR szResName, BOOL fLockRes)
{
	LONG				iResFileSlot = -1;
	RESOURCE_HEADER	resHeader;
	LONG				iExtIndex;
	LONG				iSlot;
	SHORT 			iBlk;
	SHORT				retval;

	iExtIndex = FindExtensionIndex (szResName);

	// If an extension was specified we expect to find it in the table.
	if (iExtIndex == fERROR)
	{
#if defined (_DEBUG)
		fatal_error("RESMANAGE ERROR! Resource file extension for %s isn't registered.\n", szResName);
#else
		return fERROR;
#endif
	}

	retval = FindResourceSlot (szResName, iExtIndex, &iSlot, &iResFileSlot, TRUE);

	if (retval == fERROR)
		return fERROR;
	else if (retval == RM_IN_MEMORY)
	{
		iBlk = iResBlock[iSlot];
		
		if (fLockRes)
			SetBlockAttr (iBlk, LOCKED, LOCKED);
		
		return (iSlot | RESOURCE_ID_BIT);
	}
	else if (iResFileSlot != -1)
	{
		iBlk = LoadCompressedDataFromResFile (&resHeader, iResFileSlot, fLockRes);
		if (iBlk >= 0)
		{
			if (iBlk > 0)
			{
				SetResource(iBlk);
			}
		
			iResBlock[iSlot] = iBlk;		/* update the ResBlock array */
			return (iSlot | RESOURCE_ID_BIT);
		}
		return fERROR;
	}
	else
	{
		return fERROR;
	}
}


/* =======================================================================
	GetResource - Returns a handle to a resource. If not already in memory
	then load the resource.
	======================================================================= */
SHORT _GetResource_ (CSTRPTR szResName, BOOL fNoScale, BOOL fLockRes, BOOL fRotated)
{
	SHORT	 iBlk = 0;
    SHORT    iExtIndex;
	LONG	 iSlot = 0;
	LONG	 iResFileSlot = -1;	// [d3-10-97 JPC] < 0 = load from .PCX
										// (or whatever) file; >= 0 means load
										// from resfile; use global parallel
										// arrays to figure out which file
										// and what offset.
	SHORT  retval;

	if (fReport & fREPORT_RESMGR)
		printf("Entered GetResource\n");

	run_timers();  // cdb 11/27

	iExtIndex = FindExtensionIndex (szResName);
	// If an extension was specified we expect to find it in the table.
	// Return fERROR if it is not found.
	// (It is also an error if there was no extension.)
	// The following test was changed from check for <= 0 because 0 is a
	// valid return from FindExtensionIndex.  (It means no extension was
	// specified, and caller will eventually get an fERROR return, but
	// various slots in the global arrays will be filled.)
	if (iExtIndex == fERROR)
	{
#if defined (_DEBUG)
		fatal_error("RESMANAGE ERROR! Resource file extension for %s isn't registered.\n", szResName);
#else
		return fERROR;
#endif
	}

	retval = FindResourceSlot (szResName, iExtIndex, &iSlot, &iResFileSlot, FALSE);

	if (retval == fERROR)
		return fERROR;
	else if (retval == RM_IN_MEMORY)
	{

		iBlk = iResBlock[iSlot];
		if (fLockRes)
		{
			SetBlockAttr(iBlk, LOCKED, LOCKED);
		}
		return (iSlot | RESOURCE_ID_BIT);
	}

	// [d4-13-97 JPC]
	if (iResFileSlot != -1)
	{
		gResFlags[iResFileSlot] = 0;
		if (fNoScale)
			gResFlags[iResFileSlot] |= RM_NOSCALE;
		if (fLockRes)
			gResFlags[iResFileSlot] |= RM_LOCKED;
		if (fRotated)
			gResFlags[iResFileSlot] |= RM_ROTATED;
		// Experiment! (It worked, showing that loading from resfiles
		// via Query_iResBlock works.)
		iResBlock[iResFileSlot] = 0;
	}
	else	// Experiment! Goes with above call to Query_iResBlock.
	{
		iBlk = (*ResExtentions[iExtIndex].pfLoadProc)(szResName, fNoScale, fLockRes, fRotated, iResFileSlot);
		if (iBlk > 0)				/* if not error condition */
		{
			SetResource(iBlk);
			//GWP BAD CODE!
			fNewRes = TRUE;
		}
	}

	if (iBlk == 0 && iResFileSlot == -1)
	{
#if defined (_DEBUG)
		fatal_error("RESMANAG ERROR! pfLoadProc returned iBlk==0, file:%s\n",szResName);
#else
		return fERROR;
#endif
	}
	
	iResBlock[iSlot] = iBlk;		/* update the ResBlock array */

	if (iBlk == fFILE_NOT_FOUND)	/* report back a generic error */
		iBlk = fERROR;

	if (iBlk > 0 || (iBlk == 0 && iResFileSlot != -1))
	{
		return (iSlot | RESOURCE_ID_BIT);
	}
	// Pass error codes back to caller.
	return iBlk;
}


/* =======================================================================
	Various resource query routines to help resource verification
	======================================================================= */

/* ========================================================================
   Function    - Query_iResBlock
   Description - Refresh a resource handle to the memory
   Returns     -
   ======================================================================== */

SHORT Query_iResBlock (SHORT i)
{
// i is an index to a resource slot.

	SHORT iBlk;
	
	if (!(i & RESOURCE_ID_BIT))
	{
		return fERROR;
	}
	
	i &= RESOURCE_ID_MASK;

	
	if (i == fERROR ||
		i > iMaxResSlots
		)
	{
		return fERROR;
	}

	iBlk = iResBlock[i];

#if fUSE_RES_FILES
	// GWP & JPC.
	// "Hot swap" the resource if it was purged out but in a .RES file.
	// (A primitive virtual memory manager.)
	// iBlk is 0 if the memory has been purged.
	// giResFileNames[i] is > 0 if the resource is in a .RES file.

	if (iBlk == 0 && giResFileNames[i] > 0)
	{
		LONG iExtIndex = iResExtIndex[i];
		if (iExtIndex > 0)
		{
			BOOL	fNoScale;
			BOOL	fLocked;
			BOOL	fRotated;

			fNoScale = (gResFlags[i] & RM_NOSCALE) == RM_NOSCALE;
			fLocked  = (gResFlags[i] & RM_LOCKED ) == RM_LOCKED;
			fRotated = (gResFlags[i] & RM_ROTATED) == RM_ROTATED;
			iBlk = (*ResExtentions[iExtIndex].pfLoadProc)("",
				fNoScale, fLocked, fRotated, i);
			
			iResBlock[i] = iBlk;
			if (iBlk > 0)				/* if not error condition */
			{
				SetResource(iBlk);
				
				if ((gResFlags[i] & RM_PURGABLE) == RM_PURGABLE)
				{
					(*ResExtentions[iExtIndex].pfSetPurgeProc)(i, iBlk);
				}
				
				if ((gResFlags[i] & RM_CLASS2) == RM_CLASS2)
				{
					SetClass2(iBlk);
				}
				
				if ((gResFlags[i] & RM_MULTI_USER) == RM_MULTI_USER)
				{
					SetMultiUser(iBlk);
				}
				
				//GWP BAD CODE!
				fNewRes = TRUE;
			}
		}
	}
#endif


	return iBlk;			/* index number of the memmanag block for this resource */
}


/* =======================================================================
	DisposeRes
	
	Pulls off the resource ID bit, gets the memory handle and calls the memory
	dispose. Then zero's out the index in the resource table.

	======================================================================= */
SHORT DisposRes (SHORT iResBlk, SHORT iMemBlk)
{
	iResBlk &= RESOURCE_ID_MASK;

	if (iResBlk < 0 || iResBlk > iMaxResSlots)
	{
		return fERROR;
	}

	// Clear just the memory block, anyone asking whether the data is in memory
	// now can ask using Query_ResBlock.
	iResBlock[iResBlk]		 = 0;		/* set resource map for not-in-memory */

	if (iMemBlk > 0)
	{
		ClrResource(iMemBlk);		// prevent recursion back to DisposRes
		return DisposBlock(iMemBlk);
	}
	return fERROR;
}

/* ========================================================================
   Function    - ReleaseRes
   Description - Marks this resource block as reuseable. Call this fn. when
   				 no one will be re-using this resource again in the game.
   Returns     -
   ======================================================================== */

SHORT ReleaseRes(SHORT iResBlk)
{
	SHORT iMemBlk;
	
	iResBlk &= RESOURCE_ID_MASK;

	if (iResBlk < 0 || iResBlk > iMaxResSlots)
	{
		return fERROR;
	}
	
	iMemBlk = iResBlock[iResBlk];
	
	iResBlock[iResBlk]		= 0;
	lResNameCRC[iResBlk]    = 0;
	iResExtIndex[iResBlk]   = UNSET_RES_EXT;
#if fUSE_RES_FILES
	gcbResOffset[iResBlk]	= 0;
	giResFileNames[iResBlk]	= 0;		// UNSET_RES_FILE_EXT;
	gResFlags[iResBlk]		= 0;
#endif
	
	iMaxResSlotsInUse--;
	
	if (iMemBlk > 0)
	{
		ClrResource(iMemBlk);		// prevent recursion back to DisposRes
		return DisposBlock(iMemBlk);
	}
	return fERROR;
}

/* ========================================================================
   Function    - SetPurgeRes
   Description - Set the purge bit on the memory block
   Returns     -
   ======================================================================== */

SHORT SetPurgeRes (SHORT iResBlk, SHORT iMemBlk)
{
	SHORT Result;
#if fUSE_RES_FILES
	gResFlags[iResBlk] |= RM_PURGABLE;
#endif
	if (iResBlock[iResBlk] > 0)
	{
		Result = SetBlockAttr(iMemBlk,PURGABLE,PURGABLE);	/* set the purge attribute */
	}
	else
	{
		Result = fNOERR;
	}
	return Result;
}

/* ========================================================================
   Function    - ClrPurgeRes
   Description - Clear the purge bit on the memory block
   Returns     -
   ======================================================================== */

SHORT ClrPurgeRes (SHORT iResBlk, SHORT iMemBlk)
{
	SHORT Result;
#if fUSE_RES_FILES
	gResFlags[iResBlk] &= (0xFF ^ RM_PURGABLE);
#endif
	if (iResBlock[iResBlk] > 0)
	{
		Result = SetBlockAttr(iMemBlk,PURGABLE,0);		/* clear the purge attribute */
	}
	else
	{
		Result = fNOERR;
	}
	return Result;
}

/* =======================================================================
	Load Unidentified File Format
	======================================================================= */
SHORT LoadUFF (CSTRPTR szFileName, BOOL fNoScale, BOOL fLockRes, BOOL fRotated, LONG iResFileSlot)
{
	szFileName = 0;
	fNoScale = 0;
	fLockRes = 0;
	fRotated = 0;
	return fERROR;
}


/* =======================================================================
	Load LZSS compressed data from .RES files
	======================================================================= */

#if fUSE_RES_FILES
SHORT LoadCompressedDataFromResFile (
	RESOURCE_HEADER * pResHeader,
	LONG iResFileSlot,
	BOOL	fLockRes)
{
	PTR			pInBuffer;
	SHORT		iTempBlk;
	LONG		iFileName;
	int			result;
	char		szPathname[_MAX_PATH];
	ULONG		iLzssDataOffset = 0;
	ULONG		const iMinExtraBuffer =  4096L;

	iFileName = giResFileNames[iResFileSlot];

	if(0 == strncmp("wave", gszResFileNames[iFileName],4) )
		sprintf (szPathname, "%s%s%s", InstallPath, RESFILE_PATH, gszResFileNames[iFileName]);
	else
		sprintf (szPathname, "%s%s", RESFILE_PATH, gszResFileNames[iFileName]);

	file = DiskOpen (szPathname);		// try to open the file
	if (file == fERROR)					// could not open file
		return (SHORT)fFILE_NOT_FOUND;

	// Load from resfile; result should be identical to the result
	// of loading from PCX file.

	lseek (file, gcbResOffset[iResFileSlot], SEEK_SET);
	read (file, pResHeader, sizeof (RESOURCE_HEADER));
	assert (strncmp ((char *)(&pResHeader->startcode), "RSRC", 4) == 0);

	if (pResHeader->compressionCode == NO_COMPRESSION)
	{
		if (fLockRes)
		{
			iTempBlk = NewLockedBlock (pResHeader->cbUncompressedData);
		}
		else
		{
			iTempBlk = NewBlock (pResHeader->cbUncompressedData);
		}
	}
	else
	{
		// Allocate space in the buffer for inblock decompression.
		iLzssDataOffset = iMinExtraBuffer;
		if (iLzssDataOffset > pResHeader->cbCompressedData)
		{
			iLzssDataOffset = pResHeader->cbCompressedData;
		}
		
		if (fLockRes)
		{
			iTempBlk = NewLockedBlock ((LONG)(pResHeader->cbUncompressedData + iLzssDataOffset));
		}
		else
		{
			iTempBlk = NewBlock ((LONG)(pResHeader->cbUncompressedData + iLzssDataOffset));
		}
	}
	
	if (iTempBlk == fERROR)
	{
		DiskClose (file);
#if defined (_DEBUG)
		fatal_error("RESMANAG ERROR - unable to allocate memory for the resource\n");
#endif
		return (SHORT)fERROR;
	}
	if (!fLockRes)
		SetBlockAttr (iTempBlk, LOCKED, LOCKED);

	pInBuffer = (PTR)BLKPTR(iTempBlk) + ((pResHeader->cbUncompressedData + iLzssDataOffset) - pResHeader->cbCompressedData);

	result = read (file, pInBuffer, pResHeader->cbCompressedData);

	DiskClose (file);

	if (result == fERROR)
	{
		DisposBlock (iTempBlk);
#if defined (_DEBUG)
		fatal_error("RESMANAG ERROR - data read from resfile failed\n");
#endif
		return (SHORT)fERROR;
	}

	if (pResHeader->compressionCode == NO_COMPRESSION)
	{
		if (!fLockRes)
			ClrLock (iTempBlk);
		return iTempBlk;						// all done, return
	}
	
	// If we get here, the resource is compressed.
	// Allocate another buffer to decompress it to.
	// The buffer size of 128L is the next power of 2 greater than the
	// maximum run length in our LZSS compression, which is 67.

	if (pResHeader->compressionCode == LZSS_COMPRESSION)
	{
		// [d3-06-97 JPC] I added code to _LZSSDecode_ to abort if edi passes
		// the last parameter (pMax).
		// (edi should NOT pass the pMax parameter, but if it does and we don't
		// catch it, the program blows up!)

		// Resize the block to the true size of the resource (release the
		// decompression safety buffer).
		SetBlockSize(iTempBlk, pResHeader->cbUncompressedData);
	}
	else
	{
		// LZSS is the only currently supported compression method for RES files.
		DisposBlock (iTempBlk);
		return (SHORT)fERROR;
	}

	if (!fLockRes)
	{
		ClrLock(iTempBlk);
	}
	return iTempBlk;
}

/* ========================================================================
   Function    - DecodeRotatedPCX
   Description - Special Nova rotated pcx's are decoded here.
   				 This is a helper subroutine to LoadPCX
   Returns     -
   ======================================================================== */

static void DecodeRotatedPCX(
	PCXHDR *PCX,
	PTR pSS,	// pSrc
	PTR pDest
)
{
	ULONG ll = PCX->cbLine * PCX->h;
	ULONG w =  PCX->w * PCX->h;
	USHORT j;
	ULONG l = 0;

	for (j=0; j < PCX->h; j++)
	{
		PTR pDD = pDest + j;
		l = 0;
		w = 0;
		do
		{
			UBYTE c = *(pSS++);
			if ((c & 0xC0) == 0xC0)					/* test for run */
			{
				USHORT k = c & 0x3F;							/* run length */
				c = *(pSS++);							/* run byte */
				
				w += k;	// GWP optimization.
				while(k--)
				{
					pDD[l] = c;
					l += PCX->h;
				}
			}
			else											/* unique byte */
			{
				pDD[l] = c;
				l += PCX->h;
				w++;
			}
		} while (l < ll);
		if (PCX->w < PCX->cbLine)
			pDD[l-PCX->h] = 0;
	}
	PCX->w = PCX->cbLine;
	l = PCX->w;				/* reverse the width and height */
	PCX->w = PCX->h;
	PCX->h = (SHORT)l;
}

/* ========================================================================
   Function    - DecodePCX
   Description - Pcx's are decoded here.
   				 This is a helper subroutine to LoadPCX
   Returns     -
   ======================================================================== */

static void DecodePCX(
	PCXHDR *PCX,
	PTR pSS,	// pSrc
	PTR	pDD,	// pDest
	ULONG cbDecompressedSize )
{
	ULONG w = 0;
	ULONG l;
	
	for (l=0; l < cbDecompressedSize; )
	{
		UBYTE c = *(pSS++);
		if ((c & 0xC0) == 0xC0)						/* test for run */
		{
			USHORT const k = c & 0x3F;				 	/* run length */
			c = *(pSS++);							/* run byte */
			
			w += k;	// GWP optimization.
			l += k; // GWP optimization.
			memset(pDD, c, k);	// GWP optimization.
			pDD += k;	// GWP optimization.
		}
		else											/* unique byte */
		{
			*(pDD++) = c;
			l++;
			w++;
		}
		if (w == PCX->w)
			*(pDD-1) = 0;
	}
	PCX->w = PCX->cbLine;
}

/* ========================================================================
   Function    - RotateBitm
   Description - Flip bitm pointed to by iBlk around on the diagonal that
	              runs from upper left to lower right.
   				  This is a helper subroutine of LoadPCXFromResFile.  Call
					  it when a bitm should be rotated but is not rotated in
					  the resource file OR vice versa.
	Returns     - TRUE if it worked, FALSE if not (could not allocate memory)
	======================================================================== */
BOOL RotateBitm (SHORT iBitmBlk, LONG w, LONG h)
{
// If the bitm is a perfect square, we can rotate in place by simple swaps,
// but for the general case of an m X n rectangle, that won't work.
// Copy and rotate the pixels to a temp buffer, then copy the result back
// into the source buffer.

	SHORT			iTempBlk;
	PTR			pData;
	PTR			pRotatedData;
	BITMPTR		pBitmHeader;
	LONG			size;
	LONG			x, y;
	LONG			xth;							// "x times h"
	LONG			ytw;							// "y times w"

	size = w * h;

	iTempBlk = NewBlock (size);
	if (iTempBlk == fERROR)
	{
#if defined (_DEBUG)
		fatal_error("RESMANAG ERROR - unable to allocate memory for rotation, size: %ld\n", size);
#endif
		return FALSE;
	}
	pBitmHeader = (BITMPTR)BLKPTR (iBitmBlk);
	pData = (PTR)pBitmHeader + sizeof(BITMHDR);
	pRotatedData = (PTR)BLKPTR (iTempBlk);

	// Here's the algorithm without the multiplications in the innermost loop:
	for (y = 0, ytw = 0; y < h; y++, ytw += w)
	{
		for (x = 0, xth = 0; x < w; x++, xth += h)
			pRotatedData[xth + y] = pData[ytw + x];
	}
	memcpy (pData, pRotatedData, size);
	DisposBlock (iTempBlk);
	return TRUE;
}


/* =======================================================================
	Load PCX format files from resource file.
	Procedure: take the LZSS-compressed data in the resource file and
	decompress it into a memory image of the original PCX file;  then
	process that memory image in a way similar to the way LoadPCX handles
	actual PCX files.
	======================================================================= */
SHORT LoadPCXFromResFile (CSTRPTR szFileName, BOOL fNoScale, BOOL fLockRes, BOOL fRotated, LONG iResFileSlot)
{
	PCXHDR			PCX;
	PTR				pSrc, pDest;
	USHORT *		pFinal;
	SHORT			iBlk;
	ULONG			cBytes, cbLargest;
	ULONG			cbDecompressed, cbFinal;
	/* cbDestOffset is where the file is decompressed to */
	ULONG const	    cbDestOffset = sizeof(BITMHDR);
	USHORT			scale;
	SHORT			iSrcBlk;
	ULONG	   		cbSafety = 2048L;		/* default to a safety buffer of 2k bytes */
	RESOURCE_HEADER resHeader;


	iSrcBlk = LoadCompressedDataFromResFile (&resHeader, iResFileSlot, fLockRes);

	if (resHeader.flags & RFF_PCX_UNCOMP)
	{
		// If we get here, the PCX resource was already converted to a
		// bitm by RESUTIL.

		BITMPTR	p;
		LONG		w;								// [d4-25-97 JPC] new
		LONG		h;								// [d4-25-97 JPC] new
		BOOL		fResRotated;

		iBlk = iSrcBlk;
		if (!fLockRes)
			SetQuickLock (iBlk);
		p = (BITMPTR)BLKPTR(iBlk);
		PCX.w = w = p->w;
		PCX.h = h = p->h;

		fResRotated = (resHeader.flags & RFF_ROTATED) == RFF_ROTATED;

		// If rotation from RES file does not match desired rotation,
		// rotate the bitm.  Fun fact: two rotations of a bitm puts
		// it back the way it was.  That's because we don't do a
		// simple 90 degree rotation.  Instead, we flip the bitm
		// about a diagonal.

		if (fResRotated != fRotated)
		{
         ClrLock(iBlk);                // unlock to make the allocation
													// in RotateBitm easier
			if (!RotateBitm (iBlk, PCX.w, PCX.h))
			{
				DisposBlock (iBlk);
				return fERROR;					// very bad
			}
			PCX.w = h;
			PCX.h = w;
         SetQuickLock (iBlk);          // lock it back down
		}
	}
	else
	{
		// If we get here, we need to convert the resource from PCX format
		// to BITM format.

		if (iSrcBlk == fERROR || iSrcBlk == fFILE_NOT_FOUND)
			return iSrcBlk;
	
		if (!fLockRes)
			SetBlockAttr (iSrcBlk, LOCKED, LOCKED);
		pSrc = (PTR)BLKPTR(iSrcBlk);
	
		// At this point, pSrc points to an image in memory of the PCX file.
		// Get the PCX header.
		memcpy (&PCX.code, pSrc, sizeof (PCXHDR));
		PCX.w = PCX.w + (SHORT)1 - PCX.xo;			/* calc w and h */
		PCX.h = PCX.h + (SHORT)1 - PCX.yo;
	
		if (PCX.w > 3000 || PCX.h > 0x1000)
		{
			fatal_error ("Resfile failed to decompress properly!\n");
		}
	
		// Note that there are two compressions going on here.
		// The resHeader.cbUncompressedData member is the size of the
		// UNCOMPRESSED LZSS data, which is the size of the COMPRESSED
		// PCX data.
		cBytes = resHeader.cbUncompressedData - 128L;
	
	
		/* cbDecompressed is the size of the decompressed graphic */
		cbDecompressed = (ULONG)PCX.cbLine * (ULONG)PCX.h;
	
		/* cbFinal is the size of the final graphic */
		cbFinal			= cbDecompressed + sizeof(BITMHDR);
	
		// We don't modify cbSafety here, because unlike LoadPCX, we don't
		// have one big buffer, we have two buffers.
	
		/* cbLargest is the size of the buffer to allocate for all opperations */
		cbLargest		= MAX(cbFinal, cBytes) + cbSafety;
	
		/* -------------------------------------------------------- */
		/* Allocate space for data */
		/* -------------------------------------------------------- */
		ClrLock(iSrcBlk);	// unlock to make the next allocation easier
		if (fLockRes)
		{
			iBlk = NewLockedBlock (cbLargest);
		}
		else
		{
			iBlk = NewBlock (cbLargest);
			SetQuickLock(iBlk);
		}
		
		if (iBlk == fERROR)
		{
			DisposBlock (iSrcBlk);
#if defined (_DEBUG)
			fatal_error("RESMANAG ERROR - unable to allocate memory for the file, size: %ld\n",cbLargest);
#endif
			return (SHORT)fERROR;
		}
		pDest = ((PTR)BLKPTR(iBlk)) + cbDestOffset;
		
		SetQuickLock(iSrcBlk);	// Lock it back down for the decoding.
		pSrc = 128L + (PTR) BLKPTR(iSrcBlk);// skip to the PCX compressed data
	
		/* -------------------------------------------------------- */
		/* Decompress the data from PCX-compressed format  */
		/* -------------------------------------------------------- */
		/* This run length decoding routine is unique to NOVA */
		if (fRotated)
		{
			DecodeRotatedPCX(&PCX, pSrc, pDest);
		}
	
		/* This run length decoding routine is unique to PCX */
		else
		{
			DecodePCX(&PCX, pSrc, pDest, cbDecompressed);
		}
	
		/* -------------------------------------------------------- */
		/* The safety buffer can be removed after decompression and optional palette read */
		// We can also release the source block at this point.
		/* -------------------------------------------------------- */
		DisposBlock (iSrcBlk);
		if (cbFinal != cbLargest)
		{
			SetBlockSize(iBlk, cbFinal);	/* Resize the data buffer */
		}
	}

	/* -------------------------------------------------------- */
	/* Do some final cleanup */
	/* -------------------------------------------------------- */
	if (fNoScale)
	{
		BITMPTR	p = (BITMPTR)BLKPTR(iBlk);
		p->scale = scale = UNITARY_SCALE;
		p->x_ctr_pt = 0;
	}
	else
		scale = (USHORT)detect_scale(iBlk, PCX.w, PCX.h);
		
	/* set width, height, x, and y */
	pFinal = (USHORT *)BLKPTR(iBlk);
	pFinal[0] = PCX.w;
	pFinal[1] = PCX.h;
	pFinal[2] = scale;				/* scale */
	pFinal[4] = TYPEBITM;			/* type */

	if (!fLockRes)
	{
		ClrLock(iBlk);
	}
		
	return iBlk;
}
#endif


SHORT LoadPCX (CSTRPTR szFileName, BOOL fNoScale, BOOL fLockRes, BOOL fRotated, LONG iResFileSlot)
{
	PCXHDR			PCX;
	PTR			pSrc, pDest;
	USHORT *		pFinal;
	ULONG			cBytes, cbDecompressed, cbLargest, cbFinal;
	ULONG			cbSrcOffset, cbDestOffset;
	USHORT			 scale;
	SHORT			iBlk;
	ULONG			cbSafety = 2048L;		/* default to a safety buffer of 2k bytes */

#if fUSE_RES_FILES
	if (iResFileSlot >= 0)
	{
		return LoadPCXFromResFile (szFileName, fNoScale, fLockRes, fRotated, iResFileSlot);
	}
#endif


	file = DiskOpen(szFileName);	/* try to open the file */
	if (file == fERROR)								/* Resource not found. Print an error */
		return (SHORT)fFILE_NOT_FOUND;			/* mark file as not found */

	if (read(file, &PCX.code, sizeof(PCXHDR)) == fERROR)	/* read header */
	{
		DiskClose(file);
#if defined (_DEBUG)
		fatal_error("RESMANAG ERROR - data read failed %s\n",szFileName);
#endif
		return (SHORT)fERROR;
	}

	PCX.w = PCX.w + (SHORT)1 - PCX.xo;			/* calc w and h */
	PCX.h = PCX.h + (SHORT)1 - PCX.yo;

	lseek(file, 128L-sizeof(PCXHDR), SEEK_CUR);			/* move to the data */

	/* cBytes is the size of the compressed graphic */
	cBytes = filelength(file) - 128L;		/* get the length of the data */

	/* cbDecompressed is the size of the decompressed graphic */
	cbDecompressed = (ULONG)PCX.cbLine * (ULONG)PCX.h;

	/* cbFinal is the size of the final graphic */
	cbFinal			= cbDecompressed + sizeof(BITMHDR);

	/* if fRotated then cannot have compressed and decompressed overlap */
	cbSafety			= cbSafety + cBytes;

	/* cbLargest is the size of the buffer to allocate for all opperations */
	cbLargest		= MAX(cbFinal, cBytes) + cbSafety;

	/* cbSrcOffset is where the file is loaded */
	cbSrcOffset = cbLargest - cBytes;

	/* cbDestOffset is where the file is decompressed to */
	cbDestOffset = sizeof(BITMHDR);

	/* -------------------------------------------------------- */
	/* Allocate space for data */
	/* -------------------------------------------------------- */
	if (fLockRes)
		iBlk = NewLockedBlock(cbLargest);
	else
		iBlk = NewBlock(cbLargest);

	if (iBlk == fERROR)
	{
		DiskClose(file);
#if defined (_DEBUG)
		fatal_error("RESMANAG ERROR - unable to allocate memory for the file, size: %ld\n",cbLargest);
#endif
		return (SHORT)fERROR;
	}

	/* -------------------------------------------------------- */
	/* Read the data into the block */
	/* -------------------------------------------------------- */
	pSrc = ((PTR)BLKPTR(iBlk)) + cbSrcOffset;
	pDest = ((PTR)BLKPTR(iBlk)) + cbDestOffset;

	if (read(file, pSrc, cBytes) == fERROR)
	{
		DiskClose(file);
		DisposBlock(iBlk);
#if defined (_DEBUG)
		fatal_error("RESMANAG ERROR - data read failed %s\n",szFileName);
#endif
		return (SHORT)fERROR;
	}

	/* -------------------------------------------------------- */
	/* Decompress the data  */
	/* -------------------------------------------------------- */
	/* This run length decoding routine is unique to NOVA */
	if (fRotated)
	{
		DecodeRotatedPCX(&PCX, pSrc, pDest);
	}

	/* This run length decoding routine is unique to PCX */
	else
	{
		DecodePCX(&PCX, pSrc, pDest, cbDecompressed);
	}

	/* -------------------------------------------------------- */
	/* The safety buffer can be removed after decompression */
	/* -------------------------------------------------------- */
	if (cbFinal != cbLargest)
	{
		SetBlockSize(iBlk, cbFinal);	/* remove decompress buffer */
		pDest = ((PTR)BLKPTR(iBlk)) + cbDestOffset;
	}

	/* -------------------------------------------------------- */
	/* Do some final cleanup */
	/* -------------------------------------------------------- */
	if (fNoScale)
	{
		BITMPTR	p = (BITMPTR)BLKPTR(iBlk);
		p->scale = scale = UNITARY_SCALE;
		p->x_ctr_pt = 0;
	}
	else
		scale = (USHORT)detect_scale(iBlk, PCX.w, PCX.h);


	/* set width, height, x, and y */
	pFinal = (USHORT *) BLKPTR (iBlk);
	pFinal[0] = PCX.w;
	pFinal[1] = PCX.h;
	pFinal[2] = scale;				/* scale */
	pFinal[4] = TYPEBITM;			/* type */

	DiskClose(file);

	return iBlk;
}

/* =======================================================================
	Load FLC format files
	======================================================================= */
SHORT LoadFLCFromResFile (LONG iResFileSlot, BOOL fLockRes)
{
	RESOURCE_HEADER resHeader;

	return LoadCompressedDataFromResFile (&resHeader, iResFileSlot, fLockRes );
}


SHORT LoadFLC (CSTRPTR szFileName, BOOL fNoScale, BOOL fLockRes, BOOL fRotated, LONG iResFileSlot)
{
	ULONG			cBytes;
	SHORT			iBlk;

#if fUSE_RES_FILES
	if (iResFileSlot >= 0)
	{
		return LoadFLCFromResFile (iResFileSlot, fLockRes);
	}
#endif

	file = DiskOpen(szFileName);	/* try to open the file */
	if (file == fERROR)								/* Resource not found. Print an error */
		return (SHORT)fFILE_NOT_FOUND;			/* mark file as not found */

	if (read(file, &cBytes, 4) == fERROR)			/* read the first four bytes */
	{
		DiskClose(file);
#if defined (_DEBUG)
		fatal_error("RESMANAG ERROR - data read failed %s\n",szFileName);
#endif
		return (SHORT)fERROR;
	}

	lseek(file, -4L, SEEK_CUR);

	/* Allocate space for data */
	if (fLockRes)
		iBlk = NewLockedBlock(cBytes);
	else
		iBlk = NewBlock(cBytes);

	if (iBlk == fERROR)
	{
		DiskClose(file);
#if defined (_DEBUG)
		fatal_error("RESMANAG ERROR - unable to allocate memory for the file, size: %ld\n",cBytes);
#endif
		return (SHORT)fERROR;
	}

	/* Read the data into the block */
	if (read(file, BLKPTR(iBlk), cBytes) == fERROR)
	{
		DiskClose(file);
		DisposBlock(iBlk);
#if defined (_DEBUG)
		fatal_error("RESMANAG ERROR - data read failed %s\n",szFileName);
#endif
		return (SHORT)fERROR;
	}

	DiskClose(file);

	return iBlk;
}

/* ========================================================================
   Function    - InitResourceManager
   Description - Set all the tables to be initialized.
   				 Generally you want to pick a value which is 2x the number
   				 of resources in the whole game. This is because we do a
   				 chained hash table and holes improve the search.
   Returns     - TRUE | FALSE
   ======================================================================== */

BOOL InitResourceManager(USHORT MaxResRequested)
{
#if defined(_DEBUG)
	remove(HASH_COLLSION_FILE);
#endif

#if defined (_WINDOWS)
	iResBlock	= (SHORT *) malloc (sizeof(SHORT)  * MaxResRequested);
	lResNameCRC = (ULONG *) malloc (sizeof(ULONG) * MaxResRequested);
	iResExtIndex = (UBYTE*) malloc (sizeof(UBYTE)  * MaxResRequested);

#if fUSE_RES_FILES
	/* offset into file for resource */
	gcbResOffset = (ULONG *) malloc(sizeof(LONG) * MaxResRequested);
	
	/* index into apFileNames */
	giResFileNames = (UBYTE *) malloc(sizeof(UBYTE) * MaxResRequested);

	// [d4-13-97 JPC] Flags:
	gResFlags = (UBYTE *) malloc(sizeof(UBYTE) * MaxResRequested);
#endif

#else
	// For DOS use the low memory area for the resource manager memory.
	iResBlock =    (SHORT *) D32DosMemAlloc (sizeof(SHORT)  * MaxResRequested);
	lResNameCRC = (ULONG *) D32DosMemAlloc (sizeof(ULONG) * MaxResRequested);
	iResExtIndex =   (UBYTE*) D32DosMemAlloc (sizeof(UBYTE)  * MaxResRequested);

#if fUSE_RES_FILES
	/* offset into file for resource */
	gcbResOffset = (ULONG *) D32DosMemAlloc(sizeof(LONG) * MaxResRequested);
	
	/* index into apFileNames */
	giResFileNames = (UBYTE *)   D32DosMemAlloc(sizeof(UBYTE) * MaxResRequested);
	// giResFileCode 	= (UBYTE *)   D32DosMemAlloc(sizeof(UBYTE) * MaxResRequested);

	// [d4-13-97 JPC] Flags:
	gResFlags = (UBYTE *) D32DosMemAlloc(sizeof(UBYTE) * MaxResRequested);
#endif

#endif

	
	if (iResBlock == NULL ||
	    lResNameCRC == NULL ||
#if fUSE_RES_FILES
		gcbResOffset == NULL ||
		giResFileNames == NULL ||
#endif
	    iResExtIndex == NULL)
	{
		iMaxResSlots = 0;
		return FALSE;
	}
	
	
	iMaxResSlots = MaxResRequested;
	iMaxResSlotsUsed = 0;
	iMaxResSlotsInUse = 0;
	
	
	// Init the arrays to known values.
	memset(iResBlock, 				 0, (sizeof(SHORT) * MaxResRequested));
	memset(lResNameCRC, 			 0, (sizeof(ULONG) * MaxResRequested));
	memset(iResExtIndex, UNSET_RES_EXT, (sizeof(UBYTE) * MaxResRequested));
#if fUSE_RES_FILES
	memset(gcbResOffset,			 0, (sizeof(ULONG) * MaxResRequested));
	memset(giResFileNames,		 0, (sizeof(UBYTE) * MaxResRequested));
#endif
	
	return TRUE;
}

/* ========================================================================
   Function    - FreeResourceManager
   Description - free up the memory we used.
   Returns     -
   ======================================================================== */

void FreeResourceManager()
{
#if defined (_WINDOWS)
	if (iResBlock)
		free(iResBlock);
	if (lResNameCRC)
		free(lResNameCRC);
	if(iResExtIndex)
		free(iResExtIndex);

	iMaxResSlots = 0;
#if fUSE_RES_FILES
	if (gcbResOffset)
		free(gcbResOffset);
	if (giResFileNames)
		free(giResFileNames);
	if (gResFlags)								// [d4-13-97 JPC]
		free (gResFlags);						// [d4-13-97 JPC]
#endif

#endif
// For DOS we just let the system clean up the memory.

}

/* ========================================================================
   Function    - HashID
   Description - Generate a unique number for each .AVD resource file.
   				 Guess what?!? The ID is guaranteed  to be unique, so just
   				 parse it out.
   Returns     - a USHORT of the id.
   ======================================================================== */

ULONG HashID(CSTRPTR szFileName)
{
	ULONG	Result = 0;
	CHAR	c;
	CSTRPTR	sz = strrchr(szFileName, '\\');
	
	if (sz == NULL)
	{
		sz = szFileName;	// No path character (\)
	}
	else
	{
		sz++;				// Next character after path char. (\)
	}
	
	c = *sz;
	while (!isdigit(c) )
	{
		if (c == 0)
#if defined (_DEBUG)
			fatal_error("HashID ERROR! bad format for ID type file name %s.\n",
															szFileName);
#else
			return 1000;		// Marle Rosone.
#endif
		sz++;
		c = *sz;
	}
	
	while (isdigit(c) && c != '.')
	{
		Result = (10 * Result) + (c - '0');
		
		sz++;
		c = *sz;
	}
	
	return Result;
}

/* ========================================================================
   Function    - CheckSum
   Description - Given a memory block, compute a checksum to see if it has
   				 changed.
   Returns     - a checksum value.
   ======================================================================== */

// Handy union to optimize the calculation of the CRC.
typedef	union {
		ULONG ulcs;
		struct {
			USHORT usUpper;
			USHORT usLower;
			} SplitWord;
	}	CHECK_SUM_NUMBER;
	
USHORT CheckSum (SHORT iHandle)
{
	ULONG	*ps;
	ULONG	l;
	ULONG	ls;
	ULONG	j;
	USHORT	Result = 0;
	SHORT	i;
	CHECK_SUM_NUMBER	cs;
	PTR		p;

	cs.ulcs = 0;
	
	// Strip off the resource bit.
	if (iHandle & RESOURCE_ID_BIT)
	{
		i= iResBlock[iHandle & RESOURCE_ID_MASK];
		// Test to see if the memory block is actually here or has been purged.
		if (i <= 0)
			return 0;
	}
	else
	{
		i = iHandle;
	}
	
	ps = (ULONG *)apBlocks[i]; 	// Use the real pointer because GetBlockSize
						   		// returns the full memory block size.
	l = GetBlockSize(i);
	
	ls = l/sizeof(ULONG);
	// Add four bytes at a time ignoring overflow.
	for (j=0; j<ls; j++, ps++)
		cs.ulcs += *ps;
		
	Result = cs.SplitWord.usUpper + cs.SplitWord.usLower;
		
	p = (PTR) apBlocks[i];
	// Add in the last 3 bytes if necessary.
	for (j = (l % sizeof(ULONG));
		 j > 0;
		 j--)
	{
		Result += p[l - j];
	}
	
	return Result;
}


/* ========================================================================
   Function    - LoadLump
   Description - Load the Lump 'O data from a single binary lump file
   Returns     -
   ======================================================================== */

SHORT LoadLump (
	CSTRPTR szFileName,
	BOOL fhAvatar,
	BOOL fLockRes ,
	BOOL  fRotated ,
	LONG  iResFileSlot )
{
	SHORT hLump = fERROR;
	SHORT pFileRead;
	
#if fUSE_RES_FILES
	if (iResFileSlot >= 0)
	{
		RESOURCE_HEADER	resHeader;
		
		hLump = LoadCompressedDataFromResFile (&resHeader,iResFileSlot, fLockRes);
		
		return hLump;
	}
#endif
	
	pFileRead = DiskOpen(szFileName);
	if (pFileRead != 0)
	{
		LONG size = filelength(pFileRead);
		if (fLockRes)
		{
			hLump = NewLockedBlock(size);
		}
		else
		{
			hLump = NewBlock(size);
		}
		
		if (hLump != fERROR)
		{
			PTR pLump = (PTR) BLKPTR(hLump);
			read(pFileRead, pLump, size);
		}
		DiskClose(pFileRead);
	}
	
	return hLump;
}

/* ========================================================================
   Function    - ResourceFileAccess
   Description - Checks whether a file is in a resource file.
   Returns     - Same as access: 0 if file is present, -1 if not.
   ======================================================================== */
LONG ResourceFileAccess (CSTRPTR pszFile)
{
	LONG			iResFileSlot = -1;
	LONG			iExtIndex;
	LONG			iSlot;
	SHORT			retval;

	iExtIndex = FindExtensionIndex (pszFile);

	// If an extension was specified we expect to find it in the table.
	if (iExtIndex == fERROR)
	{
		return -1;								// not found
	}

	retval = FindResourceSlot (pszFile, iExtIndex, &iSlot, &iResFileSlot, TRUE);

	if (retval == fERROR)
		return -1;

	if (retval == RM_IN_MEMORY)
	{
		return 0;								// found (already in memory)
	}

	// If we get here, resource is not in memory.  Check whether it's in a
	// resource file by checking iResFileSlot.
	if (iResFileSlot != -1)
	{
		return 0;								// found
	}
	else
	{
		return -1;								// not found
	}
}

/* ========================================================================
   Function    - DisposeResExtension
	Description - dispose of all resources of a given extension
	Returns     - TRUE is extension found, FALSE otherwise
	======================================================================== */
BOOL DisposeResExtension ( CHAR * szExtension )
{
	SHORT	i, ExtensionIndex;
	
	// first look for the extension in the table
	for (i = 0; i < cMAXEXTENTS; i++)
	{
		if (0 == stricmp(ResExtentions[i].achExtension, szExtension))
		{
			ExtensionIndex = (UBYTE) i;
			break;
		}
	}
	
	// didn't find this extension, return error (FALSE)
	if ( i == cMAXEXTENTS )
		return FALSE;

	// found a valid extension, dispose of all resource of this type
	for ( i = 0; i < iMaxResSlots; ++i )
	{
		if ( iResExtIndex[i] == ExtensionIndex )
		{
			SHORT const TestBlk = iResBlock[i];
			// if still a valid block, dispose of it
			if ( TestBlk > 0 )
			{
				(*ResExtentions[ExtensionIndex].pfDisposeProc)(i, TestBlk);
				ReleaseRes(i);
			}
		}
	}
	
	return TRUE;
}

// ===========================================================================
