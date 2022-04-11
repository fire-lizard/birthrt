#ifndef _DEU_H
#define _DEU_H

/* the includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#ifndef _WINDOWS
#include <malloc.h>
#endif

/*
   the directory structure is the structre used by DOOM to order the
   data in it's WAD files
*/

typedef struct Directory *DirPtr;
struct Directory
{
   LONG start;			/* offset to start of data */
   LONG size;			/* byte size of data */
   char name[ 8];		/* name of data block */
};

/*
   The wad file pointer structure is used for holding the information
   on the wad files in a linked list.

   The first wad file is the main wad file. The rest are patches.
*/

typedef struct WadFileInfo *WadPtr;
struct WadFileInfo
{
   WadPtr next;			/* next file in linked list */
   char *filename;		/* name of the wad file */
   FILE *fileinfo;		/* C file stream information */
   char type[ 4];		/* type of wad file (IWAD or PWAD) */
   LONG dirsize;		/* directory size of WAD */
   LONG dirstart;		/* offset to start of directory */
   DirPtr directory;		/* array of directory information */
};


/*
   the master directory structure is used to build a complete directory
   of all the data blocks from all the various wad files
*/

typedef struct MasterDirectory *MDirPtr;
struct MasterDirectory
{
   MDirPtr next;		/* next in list */
   WadPtr wadfile;		/* file of origin */
   struct Directory dir;	/* directory data */
};
/*
   syntactic sugar
*/
typedef int Bool;               /* Boolean data: true or false */


/* from wads.c */
extern WadPtr  WadFileList;	/* list of wad files */
extern MDirPtr MasterDir;	/* the master directory */

/* from wads.c */
#if defined (__cplusplus)
extern "C" {
#endif

void OpenMainWad( char *);
void CloseWadFiles( void);
WadPtr BasicWadOpen( char *);
void BasicWadRead( WadPtr, void *, LONG);
void BasicWadSeek( WadPtr, LONG);
MDirPtr FindMasterDir( MDirPtr, char *);
int Exists( char *);

#if defined (__cplusplus)
}
#endif

#endif // _DEU_H
