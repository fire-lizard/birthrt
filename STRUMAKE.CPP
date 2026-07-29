/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: STRUMAKE.CPP
   Author:
   ========================================================================
   The inverse of STRMAKE: disassembles strdat.dat back into the strdat.txt
   and strenum.h sources it was built from.

   The .dat holds string data only - the enum labels live nowhere but
   strenum.h - so labels are recovered from an existing strenum.h when one
   is readable and synthesised as STR_UNKNOWN_<id> otherwise.

   usage: strumake [strdat.dat] [strenum.h]

   Both inputs are read completely before either output is opened, so it is
   safe to point it at the strenum.h it is about to rewrite.
   ======================================================================== */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "PLATFORM.H"
#include <ctype.h>

#include "TYPEDEFS.H"

#include "STRINT.H"

#define STRINGTEXTFILENAME		"strdat.txt"
#define REALSTRINGENUMFILENAME	"strenum.h"

/* BLOCKHEADER carries 100 block offsets and STRBLOCK 100 string offsets, so
   the on-disk format itself caps the table at 100 * 100 strings. */
#define MAX_BLOCKS		100
#define MAX_PERBLOCK	100
#define MAX_STRINGS		(MAX_BLOCKS * MAX_PERBLOCK)
#define MAX_LABEL		64

/* STRMAKE forces a new block after 100 strings, so a full block must not be
   followed by an explicit NEWBLOCK or it would emit an empty one. */
#define BLOCK_IS_FULL(n)	((n) >= MAX_PERBLOCK)

/* These must stay byte-identical to STRMAKE.CPP's, or a disassemble/rebuild
   round trip will not reproduce the original strenum.h.  One deliberate
   exception: the include below is "STRMGR.H", matching the file's real name
   on disk.  STRMAKE.CPP historically wrote it lowercase, which the POSIX
   build cannot resolve. */
char szEnumHeader[] = {
"/* ========================================================================\n\
   Copyright (c) 1990,1996	Synergistic Software\n\
   All Rights Reserved\n\
   ========================================================================\n\
   Filename: strenum.hxx\n\
   Author: \n\
   ========================================================================\n\
   Contains the following general functions:\n\
\n\
   Contains the following internal functions:\n\
\n\
   ======================================================================== */\n\
#ifndef _STRENM_H\n\
#define _STRENM_H\n\
\n\
/* ------------------------------------------------------------------------\n\
   Includes\n\
   ------------------------------------------------------------------------ */\n\
\n\
/* ------------------------------------------------------------------------\n\
   Defines and Compile Flags\n\
   ------------------------------------------------------------------------ */\n\
\n\
/* ------------------------------------------------------------------------\n\
   Macros\n\
   ------------------------------------------------------------------------ */\n\
\n\
/* ------------------------------------------------------------------------\n\
   Prototypes\n\
   ------------------------------------------------------------------------ */\n\
\n\
/* ------------------------------------------------------------------------\n\
   Global Variables\n\
   ------------------------------------------------------------------------ */\n\
#include \"STRMGR.H\"\n\
#define GAME_STRING LONG\n\
enum {\n"
};

char szEnumFooter[] = {
"   STR_MAX_STRINGS,\n\
};\n\
\n\
#endif\n"
};

BLOCKHEADER blockHeader;
STRBLOCK    stringBlocks[MAX_BLOCKS];
char *      pBlockText[MAX_BLOCKS];
LONG        lStringCount[MAX_BLOCKS];
char        szLabels[MAX_STRINGS][MAX_LABEL];

FILE *fTextFile;
FILE *fEnumFile;
FILE *fDatFile;
LONG  lBlocks = 0;
LONG  lWarnings = 0;

LONG ReadLabels(const char *pszFileName);
LONG ReadStringFile(const char *pszFileName);
LONG CountStrings(STRBLOCK *pBlock);
void WriteEscaped(FILE *fFile, const char *pszText);
void CheckRoundTrip(const char *pszLabel, const char *pszText);

/* ========================================================================
   Function    - main
   Description - Disassembles a string table
   Returns     -
   ======================================================================== */
int main(int argc, char *argv[])
{
	const char *pszDatName  = (argc > 1) ? argv[1] : STRINGFILENAME;
	const char *pszEnumName = (argc > 2) ? argv[2] : REALSTRINGENUMFILENAME;
	LONG lLabels;
	LONG lTotal = 0;
	LONG b, i;

	// recover the labels first - the .dat does not carry them
	lLabels = ReadLabels(pszEnumName);
	if(lLabels < 0)
	{
		printf("No %s to read labels from, synthesising names.\n", pszEnumName);
		lLabels = 0;
	}
	else
	{
		printf("Read %d labels from %s.\n", lLabels, pszEnumName);
	}

	// read the whole string table before opening any output
	if(ReadStringFile(pszDatName) < 0)
		exit(1);

	// create the output text file
	fTextFile = fopen(STRINGTEXTFILENAME, "wt");
	if(fTextFile == NULL)
	{
		printf("Can't open %s\n", STRINGTEXTFILENAME);
		exit(1);
	}

	// create the output enum file
	fEnumFile = fopen(REALSTRINGENUMFILENAME, "wt");
	if(fEnumFile == NULL)
	{
		printf("Can't open %s\n", REALSTRINGENUMFILENAME);
		exit(1);
	}

	// write the enum header
	fwrite(szEnumHeader, sizeof(szEnumHeader) - 1, 1, fEnumFile);

	fprintf(fTextFile, "# Disassembled from %s by STRUMAKE.\n", pszDatName);
	fprintf(fTextFile, "# Rebuild with STRMAKE.\n");

	printf("\n\nProcessing file...");

	for(b = 0; b < lBlocks; b++)
	{
		// a full block auto-advances in STRMAKE, so only short blocks
		// get an explicit NEWBLOCK
		if(b > 0 && !BLOCK_IS_FULL(lStringCount[b - 1]))
			fprintf(fTextFile, "NEWBLOCK\n");

		// STRMAKE emits the enum separator at every block boundary
		if(b > 0)
			fprintf(fEnumFile, "};\n\nenum\n{\n");

		for(i = 0; i < lStringCount[b]; i++)
		{
			LONG lStrNum = (b * MAX_PERBLOCK) + i;
			const char *pszText = pBlockText[b] + stringBlocks[b].sOffsets[i];
			const char *pszLabel = szLabels[lStrNum];

			if(pszLabel[0] == 0)
			{
				sprintf(szLabels[lStrNum], "STR_UNKNOWN_%d", lStrNum);
				pszLabel = szLabels[lStrNum];
			}

			CheckRoundTrip(pszLabel, pszText);

			fprintf(fTextFile, "%s\t", pszLabel);
			WriteEscaped(fTextFile, pszText);
			fprintf(fTextFile, "\n");

			fprintf(fEnumFile, "\t%s\t= %d,\n", pszLabel, lStrNum);
			lTotal++;
		}
		printf(".");
	}

	fprintf(fTextFile, "END\n");
	fclose(fTextFile);

	// write the enum footer and close the file
	fwrite(szEnumFooter, sizeof(szEnumFooter) - 1, 1, fEnumFile);
	fclose(fEnumFile);

	printf("\n%d strings in %d blocks -> %s, %s\n",
	       lTotal, lBlocks, STRINGTEXTFILENAME, REALSTRINGENUMFILENAME);

	if(lWarnings)
		printf("%d string(s) will not survive a rebuild unchanged - see above.\n", lWarnings);

	return 0;
}

/* ========================================================================
   Function    - ReadLabels
   Description - Harvests "LABEL = number," lines out of an existing
                 strenum.h so the disassembly keeps the real names.
   Returns     - the number of labels read, or -1 if the file won't open
   ======================================================================== */
LONG ReadLabels(const char *pszFileName)
{
	FILE *fFile;
	char szLine[1024];
	char szLabel[MAX_LABEL];
	LONG lCount = 0;
	int  iStrNum;

	fFile = fopen(pszFileName, "rt");
	if(fFile == NULL)
		return -1;

	memset(szLabels, 0, sizeof(szLabels));

	while(fgets(szLine, sizeof(szLine), fFile) != NULL)
	{
		// only "<label> = <number>," lines carry a string id
		if(strchr(szLine, '=') == NULL || strchr(szLine, ',') == NULL)
			continue;

		if(sscanf(szLine, " %63[A-Za-z0-9_] = %d", szLabel, &iStrNum) != 2)
			continue;

		if(iStrNum < 0 || iStrNum >= MAX_STRINGS)
			continue;

		strcpy(szLabels[iStrNum], szLabel);
		lCount++;
	}

	fclose(fFile);
	return lCount;
}

/* ========================================================================
   Function    - ReadStringFile
   Description - Reads the file header, block header and every string block
                 out of a strdat.dat.
   Returns     - 0, or -1 on any failure
   ======================================================================== */
LONG ReadStringFile(const char *pszFileName)
{
	STR_FILE_HEADER fileHeader;
	LONG b;

	fDatFile = fopen(pszFileName, "rb");
	if(fDatFile == NULL)
	{
		printf("Can't open %s\n", pszFileName);
		return -1;
	}

	if(fread(&fileHeader, sizeof(fileHeader), 1, fDatFile) != 1)
	{
		printf("%s is too short to hold a file header.\n", pszFileName);
		fclose(fDatFile);
		return -1;
	}

	if(fread(&blockHeader, sizeof(blockHeader), 1, fDatFile) != 1)
	{
		printf("%s is too short to hold a block header.\n", pszFileName);
		fclose(fDatFile);
		return -1;
	}

	// sBlocks is the index of the last block, not a count
	lBlocks = blockHeader.sBlocks + 1;
	if(lBlocks < 1 || lBlocks > MAX_BLOCKS)
	{
		printf("%s claims %d blocks, which is out of range.\n", pszFileName, lBlocks);
		fclose(fDatFile);
		return -1;
	}

	printf("%s: version %d, built %s, %d blocks.\n",
	       pszFileName, fileHeader.version, fileHeader.cpDate, lBlocks);

	for(b = 0; b < lBlocks; b++)
	{
		if(fseek(fDatFile, blockHeader.sBlockOffsets[b], SEEK_SET) != 0 ||
		   fread(&stringBlocks[b], sizeof(STRBLOCK), 1, fDatFile) != 1)
		{
			printf("Can't read the header for block %d.\n", b);
			fclose(fDatFile);
			return -1;
		}

		lStringCount[b] = CountStrings(&stringBlocks[b]);

		if(stringBlocks[b].lSize <= 0)
		{
			pBlockText[b] = NULL;
			continue;
		}

		// one extra byte so a truncated final string still terminates
		pBlockText[b] = (char *) calloc(stringBlocks[b].lSize + 1, 1);
		if(pBlockText[b] == NULL)
		{
			printf("Out of memory reading block %d.\n", b);
			fclose(fDatFile);
			return -1;
		}

		if(fread(pBlockText[b], stringBlocks[b].lSize, 1, fDatFile) != 1)
		{
			printf("Can't read the %d bytes of string data for block %d.\n",
			       stringBlocks[b].lSize, b);
			fclose(fDatFile);
			return -1;
		}
	}

	fclose(fDatFile);

	if(fileHeader.version != (blockHeader.sBlocks * MAX_PERBLOCK) + lStringCount[lBlocks - 1])
	{
		printf("Warning: header version %d disagrees with the %d strings found.\n",
		       fileHeader.version,
		       (blockHeader.sBlocks * MAX_PERBLOCK) + lStringCount[lBlocks - 1]);
		lWarnings++;
	}

	return 0;
}

/* ========================================================================
   Function    - CountStrings
   Description - Works out how many strings a block holds.  STRMAKE zeroes
                 the offset table up front, and offsets climb from zero, so
                 the last non-zero entry marks the end.
   Returns     - the number of strings in the block
   ======================================================================== */
LONG CountStrings(STRBLOCK *pBlock)
{
	LONG lLast = 0;
	LONG i;

	if(pBlock->lSize <= 0)
		return 0;

	for(i = 1; i < MAX_PERBLOCK; i++)
	{
		if(pBlock->sOffsets[i] != 0 && pBlock->sOffsets[i] < pBlock->lSize)
			lLast = i;
	}

	// string 0 always sits at offset 0, so a lone string still counts
	return lLast + 1;
}

/* ========================================================================
   Function    - WriteEscaped
   Description - Writes one string as STRMAKE's GetToken expects to read it
                 back: embedded newlines become a literal backslash-n.
   Returns     -
   ======================================================================== */
void WriteEscaped(FILE *fFile, const char *pszText)
{
	const char *p;

	for(p = pszText; *p; p++)
	{
		if(*p == '\n')
			fputs("\\n", fFile);
		else if(*p != '\r')
			fputc(*p, fFile);
	}
}

/* ========================================================================
   Function    - CheckRoundTrip
   Description - Warns about strings the strdat.txt format cannot carry
                 back through STRMAKE unchanged.
   Returns     -
   ======================================================================== */
void CheckRoundTrip(const char *pszLabel, const char *pszText)
{
	// GetToken skips whitespace between the label and the text
	if(isspace((unsigned char) pszText[0]))
	{
		printf("\nWarning: %s starts with whitespace, which a rebuild will strip.\n", pszLabel);
		lWarnings++;
	}

	// a bare label with no text sends GetToken off the end of its buffer;
	// the game's own convention for an empty string is a single '~'
	if(pszText[0] == 0)
	{
		printf("\nWarning: %s is empty, writing it as-is will not rebuild.\n", pszLabel);
		lWarnings++;
	}

	// GetToken treats these as a comment when they open a line
	if(pszText[0] == '#' || (pszText[0] == '/' && pszText[1] == '/'))
	{
		printf("\nWarning: %s starts a comment, a rebuild will drop it.\n", pszLabel);
		lWarnings++;
	}
}
