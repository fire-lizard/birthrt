/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: STRMGR.c  -
   Author:   Michael Branham

   ========================================================================
   Contains the following internal functions:
   Contains the following general functions:
   ======================================================================== */
/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------
	
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "PLATFORM.H"
#include "TYPEDEFS.H"
#include "MACHINE.H"
#include "SYSTEM.H"

#include "STRINT.H"
#include "STRMGR.H"
#include "strenum.h"

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
typedef struct _STRING_DATA_BLOCK
{
	STRBLOCK	stringBlock;
	SHORT		hStrings;
} STRING_DATA_BLOCK;

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
static SHORT DisposeStringBlock (SHORT iResBlk, SHORT iMemBlk);
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
static BLOCKHEADER blockHeader = {fERROR, 0};
static SHORT hPrevStrDataBlock = fERROR;

/* ========================================================================
   Function    - STRMGR_InitStrMgr
   Description - initialize member data, and read in number of blocks.
   Returns     -
   ======================================================================== */
void STRMGR_InitStrMgr (void)
{
	int iStringFile;

	blockHeader.sBlocks = fERROR;
	hPrevStrDataBlock = fERROR;
	
	// open the file
	iStringFile = (int) DiskOpen(STRINGFILENAME);

	if(iStringFile != fERROR)
	{
		STR_FILE_HEADER fileHeader;
		LONG numBytes = 0;
		
		// get the file header and check it matches.
		numBytes = read(iStringFile, &fileHeader, sizeof(fileHeader));
		
		if (fileHeader.version != STR_MAX_STRINGS || numBytes < sizeof(fileHeader))
		{
			DiskClose(iStringFile);
			fatal_error("STRMGR ERROR! String Table built on %s doesn't match executable.\n",
			                   fileHeader.cpDate);
		}
		
		// get the block header from the file
		numBytes = read(iStringFile, &blockHeader, sizeof(blockHeader));
		DiskClose(iStringFile);
		
		if (numBytes < sizeof(blockHeader))
		{
			fatal_error("STRMGR ERROR! Unable to read header for string table.\n");
		}
		
		// Now register this resource.
		RegisterResExtention("STR", LoadSTR, DisposeStringBlock, SetPurgeRes, ClrPurgeRes, HashID);
	}
	else
	{
		fatal_error("ERROR: %s file not found. Please Re-install.", STRINGFILENAME);
	}
}

/* ========================================================================
   Function    - STRMGR_GetStr(int)
   Description - Gets a string from the string pool
   Returns     - pointer to the string
   ======================================================================== */
CSTRPTR STRMGR_GetStr(LONG iStrNum)
{
	LONG lWhichBlock;
	LONG lWhichString;
	LONG lStringOffset;
	CSTRPTR pStrings;
	CSTRPTR pStr;
	char idstr[20];
	SHORT hStrDataBlock;
	STRING_DATA_BLOCK *pStrDataBlock;
	static char NullString[150];

	// make sure there is a valid file
	if(blockHeader.sBlocks == fERROR)
		STRMGR_InitStrMgr();

	// find the block the string is in
	lWhichBlock = iStrNum / 100;
	lWhichString = iStrNum % 100;

	// is this block already in memory?
	sprintf(idstr, "%d.STR", lWhichBlock);
	hStrDataBlock = GetResourceStd(idstr, FALSE);
	
	if (hStrDataBlock < 0)
	{
		NullString[0] = 0;
		return NullString;	// Couldn't load the string for some reason.
	}
	
	if (hStrDataBlock != hPrevStrDataBlock)
	{
		if (hPrevStrDataBlock > 0)
		{
			SetPurge(hPrevStrDataBlock);
		}
		hPrevStrDataBlock = hStrDataBlock;
	}
	
	// Get the memory block.
	pStrDataBlock = (STRING_DATA_BLOCK *) BLKPTR(hStrDataBlock);
	
	if (pStrDataBlock->hStrings == fERROR)
	{
		NullString[0] = 0;
		return NullString;
	}
		
	pStrings = (CSTRPTR)BLKPTR(pStrDataBlock->hStrings);

	// return the string
	lStringOffset = pStrDataBlock->stringBlock.sOffsets[lWhichString];
	pStr = (CSTRPTR)pStrings + lStringOffset;

	// special case for ~ as a null string
	if(*pStr == '~')
	{
		NullString[0] = 0;
		return NullString;
	}
	else
		return pStr;
}

/* ========================================================================
   Function    - LoadStr
   Description - Get a string resource from the string table.
   Returns     - A handle to the block.
   ======================================================================== */

SHORT LoadSTR (
	CSTRPTR szFileName,
	BOOL  fSetPal,
	BOOL  fLockRes,
	BOOL  fRotated,
	LONG  iResFileSlot)
{
	LONG lWhichBlock;
	SHORT hStrings = fERROR;
	int	iStringFile;
	LONG length;
	SHORT hStrDataBlock = fERROR;
	STRING_DATA_BLOCK *pStrDataBlock;
	
	// run the hash fn again to tell us which block to extract.
	lWhichBlock = (LONG) HashID(szFileName);
	
	// open the string file
	iStringFile = (int) DiskOpen(STRINGFILENAME);

	if(iStringFile == fERROR)
		return fFILE_NOT_FOUND;

	hStrDataBlock = NewBlock(sizeof(STRING_DATA_BLOCK));
	if ( hStrDataBlock == fERROR)
		return fERROR;
		
	pStrDataBlock = (STRING_DATA_BLOCK *)BLKPTR(hStrDataBlock);
	pStrDataBlock->hStrings = fERROR;
	
	// read the block
	lseek(iStringFile, blockHeader.sBlockOffsets[lWhichBlock], SEEK_SET);

	// read the block header
	if(read(iStringFile, &(pStrDataBlock->stringBlock), sizeof(STRBLOCK)) < sizeof(STRBLOCK))
	{
		DiskClose(iStringFile);
		DisposBlock(hStrDataBlock);
		return fERROR;
	}

	length = pStrDataBlock->stringBlock.lSize;

	// now get the memory and read the data
	hStrings = NewBlock(length);
	
	// refesh the pointer.
	pStrDataBlock = (STRING_DATA_BLOCK *)BLKPTR(hStrDataBlock);
	
	if(hStrings != fERROR)
	{
		pStrDataBlock->hStrings = hStrings;
		
		if (read(iStringFile, BLKPTR(hStrings), pStrDataBlock->stringBlock.lSize) < pStrDataBlock->stringBlock.lSize)
		{
			DiskClose(iStringFile);
			DisposBlock(hStrings);
			DisposBlock(hStrDataBlock);
			return fFILE_NOT_FOUND;	// So we won't try again for this block.
		}
	}
	else
	{
		DiskClose(iStringFile);
		DisposBlock(hStrDataBlock);
		return fERROR;
	}

	DiskClose(iStringFile);
	
	return hStrDataBlock;
}

/* ========================================================================
   Function    - DisposeStringBlock
   Description - First dispose of our internal block of strings. Then call
  				 DisposRes to get ourselves out of the resource manager.
   Returns     - whatever DisposRes does.
   ======================================================================== */

static SHORT DisposeStringBlock (SHORT iResBlk, SHORT iMemBlk)
{
	STRING_DATA_BLOCK * pStrDataBlock = (STRING_DATA_BLOCK*)BLKPTR(iMemBlk);
	
	// First get rid our our string table.
	if (pStrDataBlock->hStrings != fERROR)
	{
		DisposBlock(pStrDataBlock->hStrings);
	}
	
	// Now clear our resource.
	return DisposRes(iResBlk, iMemBlk);
}

/* ========================================================================
   Function    - STRMGR_Upper
   Description - Upper-cases displayed game text in place, like strupr, but
                 in the text language's own encoding. Both Russian
                 translations draw Cyrillic glyphs on Latin codes. Russkiy
                 Proekt keeps the Latin case pairs as Cyrillic ones and adds
                 a few on punctuation. Fargus's pairs are other letters ('u'
                 is l but 'U' is E-reversed), so strupr garbled its realm
                 names. Fargus's fonts have no capital soft or hard sign:
                 those stay. The capitals it lacks for short i, shch, yo and
                 ze it writes, in its own capitals, as I, Sh, E and the
                 digit 3 - and so does this.
                 Not for file or resource names: those keep strupr.
   Returns     - s
   ======================================================================== */
static const char gszFargusLower[] = "abdfgewrnkuvhoqpctyzx\x95\x82m\x8f\x88\x9b\x85\x83\x91\x8a";
static const char gszFargusUpper[] = "ADBFQE\x9a" "3NKGMHOLPCTYZXRSW@UV\x81" "NWE";
static const char gszRPLower[] = "{|}'_\x89";
static const char gszRPUpper[] = "<=>&`#";

char * STRMGR_Upper (char *s)
{
	LONG const	Lang = TextLanguage();
	char const	*pLower = "";
	char const	*pUpper = "";
	char		*p;

	if (Lang == LANGUAGE_RUSSIAN1)
	{
		pLower = gszFargusLower;
		pUpper = gszFargusUpper;
	}
	else if (Lang == LANGUAGE_RUSSIAN2)
	{
		pLower = gszRPLower;
		pUpper = gszRPUpper;
	}

	for (p = s; *p; ++p)
	{
		char const * const	pHit = strchr(pLower, *p);

		if (pHit != NULL)
			*p = pUpper[pHit - pLower];
		else if (Lang != LANGUAGE_RUSSIAN1 && *p >= 'a' && *p <= 'z')
			*p -= 'a' - 'A';			// strupr in the C locale
	}
	return s;
}

/* ========================================================================
   Function    - STRMGR_LoadExeText / STRMGR_ExeText
   Description - Text the Russian translators wrote straight into their exe
                 over English literals that never went through STRDAT.DAT:
                 the credits, a few labels and messages, and the province and
                 place name tables (PLACES.CPP LoadMapNames). It lives in
                 EXETEXT.TXT in the text language's folder (FOPEN.C), one
                 "key=text" a line. The key is the English literal exactly as
                 in the source, or P<n> / L<n> for province[n] /
                 place_names[n]; the first '=' ends it ('=' is a letter in
                 Russkiy Proekt's text). ';' starts a comment line. The text
                 is in the game font's byte encoding.
                 STRMGR_ExeText returns the text for a key, or the key itself
                 when there is none - always, for English, French and German.
   Returns     - (STRMGR_ExeText) the text to show
   ======================================================================== */
typedef struct {
	char	*pszKey;
	char	*pszText;
} EXE_TEXT;

static char		*gpExeTextFile = NULL;	// ponytail: loaded once, kept for the session
static EXE_TEXT	*gaExeText = NULL;
static LONG		gcExeText = 0;

void STRMGR_LoadExeText (void)
{
	char	szFile[] = "EXETEXT.TXT", szMode[] = "rb";	// FileOpen takes char *
	FILE	*fp = FileOpen(szFile, szMode);
	long	cb;
	LONG	cLines = 1;
	char	*p, *pNext;

	if (fp == NULL)
		return;
	fseek(fp, 0, SEEK_END);
	cb = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	gpExeTextFile = (cb > 0) ? (char *)malloc(cb + 1) : NULL;
	if (gpExeTextFile == NULL || fread(gpExeTextFile, 1, cb, fp) != (size_t)cb)
	{
		free(gpExeTextFile);
		gpExeTextFile = NULL;
		FileClose(fp);
		return;
	}
	FileClose(fp);
	gpExeTextFile[cb] = '\0';

	for (p = gpExeTextFile; *p; ++p)
		if (*p == '\n')
			++cLines;
	gaExeText = (EXE_TEXT *)malloc(cLines * sizeof(EXE_TEXT));
	if (gaExeText == NULL)
		return;

	for (p = gpExeTextFile; p != NULL; p = pNext)
	{
		char	*pEq;

		pNext = strchr(p, '\n');
		if (pNext != NULL)
			*pNext++ = '\0';
		p[strcspn(p, "\r")] = '\0';
		pEq = strchr(p, '=');
		if (*p == ';' || pEq == NULL || pEq == p)
			continue;
		*pEq = '\0';
		gaExeText[gcExeText].pszKey = p;
		gaExeText[gcExeText].pszText = pEq + 1;
		++gcExeText;
	}
}

CSTRPTR STRMGR_ExeText (CSTRPTR pszKey)
{
	LONG	i;

	for (i = 0; i < gcExeText; ++i)		// ponytail: linear, a few hundred keys
		if (strcmp(gaExeText[i].pszKey, pszKey) == 0)
			return gaExeText[i].pszText;
	return pszKey;
}
