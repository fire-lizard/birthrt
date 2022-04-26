#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <ctype.h>
#include <process.h>
#include <dos.h>

#include "TYPEDEFS.H"

#include "STRINT.H"

#define STRINGTEXTFILENAME	"strdat.txt"
#define STRINGENUMFILENAME	"strenum.new"
#define REALSTRINGENUMFILENAME	"strenum.h"
#define BUFSIZE 4096

#define END			1
#define NEWBLOCK	2
#define STRING		3

#define F_OK 0

STRBLOCK stringBlock;
BLOCKHEADER blockHeader;

int GetToken(FILE *fFile);
void WriteNumBlocks(void);

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
#include \"strmgr.h\"\n\
#define GAME_STRING LONG\n\
enum {\n"
};

char szEnumFooter[] = {
"   STR_MAX_STRINGS,\n\
};\n\
\n\
#endif\n"
};

char szTextBuffer[4097];
char szLabel[80];
SHORT sNumBlocks = 0;

FILE *fTextFile;
FILE *fEnumFile;
FILE *fDatFile;
LONG iWhichString = 0;

BOOL	text_differs(char *, char *);

/* ========================================================================
   Function    - main
   Description - Creates a string table
   Returns     - 
   ======================================================================== */
int main(int argc,char * argv[])
{
	int iVal;
	LONG lPosition;
	LONG lStringPos;
	LONG lNewPosition;
	LONG lNewStringPos;
	LONG lCurBlock = 0;
	LONG lDataSize;
	STR_FILE_HEADER fileHeader;
	LONG lBlockLocation;

	// open the string text file
	fTextFile = fopen(STRINGTEXTFILENAME, "rt");
	if(fTextFile == NULL)
	{
		printf("Can't open %d\n", STRINGTEXTFILENAME);
		exit(0);
	}

	// create the output hxx file
	fEnumFile = fopen(STRINGENUMFILENAME, "wt");
	if(fEnumFile == NULL)
	{
		printf("Can't open %d\n", STRINGENUMFILENAME);
		exit(0);
	}

	// create the output dat file
	fDatFile = fopen(STRINGFILENAME, "wb");
	if(fDatFile == NULL)
	{
		printf("Can't open %d\n", STRINGFILENAME);
		exit(0);
	}

	// write the enum header
	fwrite(szEnumHeader, sizeof(szEnumHeader) - 1, 1, fEnumFile);

	memset(&stringBlock, 0, sizeof(stringBlock));
	memset(&blockHeader, 0, sizeof(blockHeader));

	// write the FileHeader;
	fileHeader.version = 0;
	sprintf(fileHeader.cpDate, "%s", __DATE__ " " __TIME__);
	fwrite(&fileHeader, sizeof(fileHeader), 1, fDatFile);
	
	// write data file header
	lBlockLocation = ftell(fDatFile);
	fwrite(&blockHeader, sizeof(blockHeader), 1, fDatFile);
	lPosition = ftell(fDatFile);
	blockHeader.sBlockOffsets[0] = lPosition;

	// set the start of the first string area
	fwrite(&stringBlock, sizeof(stringBlock), 1, fDatFile);
	lStringPos = ftell(fDatFile);

	printf("\n\nProcessing file...");

	// parse the string input file
	while(TRUE)
	{
		iVal = GetToken(fTextFile);
		if(iVal == -1)
			break;

		switch(iVal)
		{
			case END:
				printf(".\n");
				// remember where we are
				lNewPosition = ftell(fDatFile);

				// now go to the start of the current block header
				// and write the size of the string data for this block
				stringBlock.lSize = lNewPosition - lStringPos;
				fseek(fDatFile, lPosition, SEEK_SET);
				fwrite(&stringBlock, sizeof(stringBlock), 1, fDatFile);

				// goto start of file and write the blockheader
				fseek(fDatFile, lBlockLocation, SEEK_SET);
				blockHeader.sBlocks = (SHORT)lCurBlock;
				fwrite(&blockHeader, sizeof(blockHeader), 1, fDatFile);
				break;

			case STRING:
				printf(".");
				lNewStringPos = ftell(fDatFile);
				stringBlock.sOffsets[iWhichString] = lNewStringPos - lStringPos;
				fwrite(szTextBuffer, strlen(szTextBuffer) + 1, 1, fDatFile);
				fprintf(fEnumFile, "\t%s\t= %d,\n", szLabel, (lCurBlock * 100) + iWhichString);
				iWhichString++;

				// force a newblock after 100 strings
				if(iWhichString <= 99)
					break;


			case NEWBLOCK:
				//WRC [14.04.97] this makes a new enum every 
				//newblock to avoid the problem of having
				//too many entries in an enum. 
				//(2047 is the max apparently...)
				//if it starts to mess up because of too
				//many enums or something, we could add
				//another case and do a new enum every
				//2000 strings...
					
				
				fprintf(fEnumFile,"};\n\nenum\n{\n");

				
				// remember where we are
				lNewPosition = ftell(fDatFile);

				// write the current block header
				stringBlock.lSize = lNewPosition - lStringPos;
				fseek(fDatFile, lPosition, SEEK_SET);
				fwrite(&stringBlock, sizeof(stringBlock), 1, fDatFile);

				// clear the block header for the next time around
				memset(&stringBlock, 0, sizeof(stringBlock));

				// increment the block counter
				lCurBlock++;

				// save the disk offset to the new block
				blockHeader.sBlockOffsets[lCurBlock] = lNewPosition;

				// write an empty string block to the file
				fseek(fDatFile, lNewPosition, SEEK_SET);
				fwrite(&stringBlock, sizeof(stringBlock), 1, fDatFile);
				lStringPos = ftell(fDatFile);

				// remember the new position
				lPosition = lNewPosition;
				iWhichString = 0;
				break;
		}
	}

	// close the input file
	fclose(fTextFile);

	// write the enum footer and close the file
	fwrite(szEnumFooter, sizeof(szEnumFooter) - 1, 1, fEnumFile);
	fclose(fEnumFile);

	// Mark the version info into the file.
	fileHeader.version = (lCurBlock * 100) + iWhichString;
	fseek( fDatFile, 0, SEEK_SET);
	fwrite(&fileHeader, sizeof(fileHeader), 1, fDatFile);
	
	// close the data file
	fclose(fDatFile);
	
	if (access(REALSTRINGENUMFILENAME, ( F_OK) ) == 0)  // exists
	{
		if (text_differs(STRINGENUMFILENAME, REALSTRINGENUMFILENAME))
		{
			remove(REALSTRINGENUMFILENAME);
			rename(STRINGENUMFILENAME, REALSTRINGENUMFILENAME);
		}
		else	// files are identical; remove the temporary one
		{
			remove(STRINGENUMFILENAME);
		}
	}
	else
	{
		rename(STRINGENUMFILENAME, REALSTRINGENUMFILENAME);
	}

	return 0;
}

int GetToken(FILE *fFile)
{
	int i;
	char c;
	char *p, *p1;

	// clear the buffer
	memset(szTextBuffer, 0, sizeof(szTextBuffer));

	// read a command from the file
	while(TRUE)
	{
		// check for end of file
		if(fgets(szTextBuffer, 4096, fFile) == NULL)
		{
			return -1;
		}

		p = &szTextBuffer[0];

		// clear leading whitespace in string
		while(isspace(*p)) p++;

		// check for comment
		if(*p == '#' || (*p == '/' && *(p+1) == '/'))
		{
			continue;
		}

		// check for blank line
		if((strlen(p) == 0) || (strcmp(p, "\n") == 0))
		{
			continue;
		}

		// parse the command
		// end of buffer command?
		if(strnicmp(p, "END", 3) == 0)
			return END;

		// newblock command?
		if(strnicmp(p, "NEWBLOCK", 8) == 0)
			return NEWBLOCK;

		// assume a string enumeration label
		memset(szLabel, 0, sizeof(szLabel));
		i = 0;

		while((c = *p++) && !isspace(c))
		{
			szLabel[i++] = c;
		}
		szLabel[i] = 0;

		// now get the string
		if(c == '\n')
		{
			// string is on a seperate line
			if(fgets(szTextBuffer, 4096, fFile) == NULL)
				return -1;

			return STRING;
		}

		// clear leading spaces in string
		if(isspace(c))
			while(isspace(*p)) p++;

		// string was on the same line as the label
		// copy the string to front of buffer
		p1 = &szTextBuffer[0];

		while(*p != '\n')
		{
			// process a newline character
			if(*p == '\\' && *(p+1) == 'n')
			{
				*p1++ = '\n';
				p += 2;
			}
			else
				*p1++ = *p++;
		}
		*p1++ = 0;

		return STRING;
	}
}

BOOL text_differs(char *filename1, char *filename2)
{
	FILE *fp1 = NULL, *fp2 = NULL;
	LONG len1 = 0, len2 = 0;
	char buf1[BUFSIZE+1], buf2[BUFSIZE+1];
	BOOL differs = TRUE;
	LONG i, blocknum = 0;
	
	fp1 = fopen(filename1, "r");
	if (!fp1)
	{
		goto Abort;
	}
		
	fp2 = fopen(filename2, "r");
	if (!fp2)
	{
		goto Abort;
	}
		
	
	while (!feof(fp1))
	{
		blocknum++;
		len1 = fread(buf1, sizeof(char), BUFSIZE, fp1);
		len2 = fread(buf2, sizeof(char), BUFSIZE, fp2);
		
		if (len1 != len2)
		{
			goto Abort;
		}
			
		for (i=0; i < len1; i++)
		{
			if (buf1[i] != buf2[i])
			{
				goto Abort;
			}
		}
	}
	differs = FALSE;
	
Abort:
	if (fp2 != NULL)
		fclose(fp2);
	if (fp1 != NULL)
		fclose(fp1);
	return differs;
}
