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
#include <io.h>
#include <fcntl.h>
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
	SHORT hStrDataBlock = fERROR;
	STRING_DATA_BLOCK *pStrDataBlock;
	static char NullString[150];

	// make sure there is a valid file
	if(blockHeader.sBlocks == fERROR)
		STRMGR_InitStrMgr();

	// find the block the string is in
	lWhichBlock = iStrNum / 100;
	lWhichString = iStrNum % 100;

	// is this block already in memory?
	sprintf(idstr, "%ld.STR", lWhichBlock);
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
#if defined (MEMORY_CHK)
		SetBlockName(szFileName,hStrings);
#endif
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
	STRING_DATA_BLOCK * pStrDataBlock = 0;
	
	// First get rid our our string table.
	pStrDataBlock = (STRING_DATA_BLOCK *) BLKPTR(iMemBlk);
	if (pStrDataBlock->hStrings != fERROR)
	{
		DisposBlock(pStrDataBlock->hStrings);
	}
	
	// Now clear our resource.
	return DisposRes(iResBlk, iMemBlk);
}
