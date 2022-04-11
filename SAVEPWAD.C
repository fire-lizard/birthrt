// ---------------------------------------------------------------------------
//
//   	This file is part of VirtuaHome by Synergistic Software
//
//    Copyright 1994/1995 by NW Synergistic Software, Inc.
//
//		Programmed by:				Michael Branham and Robert Clardy
//		Other code support by: 	Chris Phillips
//										Alan Clark
//										Steve Coleman
//										Craig Clayton
//
//		UI art by:					Linda Westerfield
//		Home texture art by:		Kirt Lemons
//
//
//		SAVEPWAD.C  - Writes Out A Nova PWAD file from the internal arrays
//								by Wes Cumberland.  Minor mods for Birthright
//                      by John Conley [JPC].
//					
//					
//

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <direct.h>                 // [d5-24-96 JPC]
#include <windows.h>
#include "system.h"
#include "engine.h"
#include "engint.h"
#include "machine.h"
#include "debug.h"
#include "savepwad.h"

// #define NumVertexes 2000			// RCC - this should be read from file and allocated

// extern long    cursector;
// extern BITM    bitms[MAX_BITMS];
// extern OBJECT  object[MAX_OBJS];	
//
// // Globals.
// extern XLAT_TEX *     xlat_tex;
extern IWAD_ENTRY     ent;
TEXTURE textures[MAX_TEXTURES];
// extern THING *        things;
// extern LINEDEF *      linedefs;
// extern SIDEDEF *      sidedefs;
// extern SPOINT *       vertexs;
// extern SPOINT *       vertexUndo[NUM_UNDOS];
// extern SEG *          segs;
// extern SSECTOR *      ssectors;
// extern NODE *         nodes /* JPC = NULL */;
// extern SECTOR *       sectors;
// extern REJECT *       rejects;
// // extern BLOCKM *       blockms;
// extern BLOCKMAP       blockm_header;
// extern USHORT *       blockm_offs;
// extern SHORT *        blockm;
// extern NSEG *         nsegs;
// extern ROOMSPEC *     aRoomSpec;
// extern word *         gaBrightness;
// extern BOOL           VertexUsed[NumVertexes];
// extern ulong          tot_things;
// extern ulong          tot_linedefs;
// extern ulong          tot_sidedefs;
// extern ulong          tot_vertexs;
// extern ulong          tot_segs;
// extern ulong          tot_ssectors;
// extern ulong          tot_nodes;
// extern ulong          tot_sectors;
// extern ulong          tot_rejects;
// extern ulong          tot_blockms;
// extern long           tthings;

// TODO: (fire lizard) uncomment
/*BOOL MakePwad(char* PwadName)
{
	
	//because string literals always have \0 appended, we can't put more
	//than 7 characters in the string, we'll have to just reassign some
	//last-letters later

	IWAD_ENTRY Dir[11]={
						{00,00,"E1M1\0\0\0"},//0
						{-1,-1,"THINGS\0"},	 //1
						{-1,-1,"LINEDEF"},   //2 reassign last letter
						{-1,-1,"SIDEDEF"},   //3 reassign last letter	
						{-1,-1,"VERTEXE"},	 //4 reassign last letter
						{-1,-1,"SEGS\0\0\0"},//5
						{-1,-1,"SSECTOR"},   //6 reassign last letter
						{-1,-1,"NODES\0\0"}, //7
						{-1,-1,"SECTORS"},	 //8
						{-1,-1,"REJECT\0"},	 //9
						{-1,-1,"BLOCKMAP"}	 //10
					   };

	
	FILE*	pwadFile=fopen(PwadName,"wb");
	long writeidx=1;
	long entries=-1;
	long DirStart=-1;

	if (!pwadFile)
	{
		ErrorMessage ("Can't open %s to write to\n",PwadName);
      return FALSE;
	}


	
	//Stroke of luck! they all need the same letter in the "null location" of the string
	Dir[2].name[7]=Dir[3].name[7]=Dir[4].name[7]=Dir[6].name[7]='S';


	fwrite("PWAD",1,4,pwadFile);
	fwrite(&entries,sizeof(long),1,pwadFile);
	fwrite(&DirStart,sizeof(long),1,pwadFile);

	//Start writing data (filling the positions in the Directory as we go)
	//we need to save them in the correct order... (see above)

	//things...
   Dir[writeidx].offset = ftell (pwadFile);
	Dir[writeidx++].size=fwrite(things,sizeof(THING),tot_things,pwadFile) * sizeof(THING);

	
	{
	//linedefs...
	ULONG			iLinedef;
	
	// [d8-15-96 JPC] Need to clear certain flags that are set when you
	// wander around the WAD.
   for (iLinedef = 0; iLinedef < tot_linedefs; iLinedef++)
		linedefs[iLinedef].flags &= ~(DRAW_ON_MAP_BIT | HAS_BEEN_CROSSED);

   Dir[writeidx].offset = ftell (pwadFile);
	Dir[writeidx++].size=fwrite(linedefs,sizeof(LINEDEF),tot_linedefs,pwadFile) * sizeof(LINEDEF);
	}

	
	{
	//sidedefs...
   ULONG       iSidedef;
   SIDEDEF     sidedef;
   
   Dir[writeidx].offset = ftell (pwadFile);
   // JPC: sidedefs are modified at run-time in the Birthright engine.
   // Fix them and write them out one at a time.
   for (iSidedef = 0; iSidedef < tot_sidedefs; iSidedef++)
   {
	   UBYTE       iTexture;
	   
      // Note weird test for "no texture."  We can't just treat 0 as
      // no texture, because 0 is ambiguous.  It can mean "no texture,"
      // but it can also mean "texture 0," which is a valid texture.
      // Determine which it is by looking at the SECOND character as well.
      // Note that this will fail if for some reason texture 0's name is
      // only one character long (a low-probability case).
      memcpy (&sidedef, &sidedefs[iSidedef], sizeof (SIDEDEF));

      iTexture = sidedef.n1[0];
      if (iTexture == 0 && sidedef.n1[1] == 0)
         sidedef.n1[0] = '-';
      else
		{
			// [d8-15-96 JPC] Avoid garbage after the terminal 0 in
			// names that are less than 8 characters long.
			strncpy (sidedef.n1, textures[iTexture].name, 8);
		}

      iTexture = sidedef.n2[0];
      if (iTexture == 0 && sidedef.n2[1] == 0)
         sidedef.n2[0] = '-';
      else
         strncpy (sidedef.n2, textures[iTexture].name, 8);

      iTexture = sidedef.n3[0];
      if (iTexture == 0 && sidedef.n3[1] == 0)
         sidedef.n3[0] = '-';
      else
         strncpy (sidedef.n3, textures[iTexture].name, 8);
      fwrite (&sidedef, sizeof (SIDEDEF), 1, pwadFile);
   }
   Dir[writeidx++].size = sizeof (SIDEDEF) * tot_sidedefs;
   }


	//vertexes...
   Dir[writeidx].offset = ftell (pwadFile);
	// GWP Dir[writeidx++].size=fwrite(vertexs,sizeof(VERTEX),tot_vertexs,pwadFile) * sizeof(VERTEX);
	// GWP our vertex is a POINT, the pwad uses a VERTEX.
	{
	LONG iVertex;
	
	for (iVertex = 0; iVertex < tot_vetexs; iVertex++)
	{
		VERTEX Temp_vert;
		Temp_vert.x = (SHORT)vertexs[iVertex].x;
		Temp_vert.y = (SHORT)vertexs[iVertex].y;
		
		fwrite(&Temp_vert,sizeof(VERTEX),1,pwadFile);
	}
	Dir[writeidx++].size=tot_vertexs * sizeof(VERTEX);
	
	}

	
	//segs...
   Dir[writeidx].offset = ftell (pwadFile);
	Dir[writeidx++].size=fwrite(segs,sizeof(SEG),tot_segs,pwadFile) * sizeof(SEG);

	{
	//ssectors...
	SSECTOR Ssector;
	ULONG 	iSsector;
	
   Dir[writeidx].offset = ftell (pwadFile);
	for (iSsector = 0; iSsector < tot_ssectors; iSsector)
	{
		ssector.n = (SHORT)ssectors[iSsector].n;
		ssector.o = (SHORT)ssectors[iSsector].o;
		
		fwrite(ssector,sizeof(SSECTOR),1,pwadFile);
	}
	Dir[writeidx++].size = tot_ssectors * sizeof(SSECTOR);
	}

	//nodes...
   Dir[writeidx].offset = ftell (pwadFile);
	// GWP Dir[writeidx++].size=fwrite(nodes,sizeof(NODE),tot_nodes,pwadFile) * sizeof(NODE);
	{
	LONG iNode;
	for (iNode =0; iNode < tot_nodes; iNode++)
	{
		NODE Temp_node;
		
		// GWP I'm zero'ing this data out because we don't seem to use it.
		Temp_node.minyf = 0;
		Temp_node.maxyf = 0;
		Temp_node.minxf = 0;
		Temp_node.maxxf = 0;
		Temp_node.minyb = 0;
		Temp_node.maxyb = 0;
		Temp_node.minxb = 0;
		Temp_node.maxxb = 0;
		
		Temp_node.x = nodes[iNode].x;
		Temp_node.y = nodes[iNode].y;
		Temp_node.a = nodes[iNode].a;
		Temp_node.b = nodes[iNode].b;
		Temp_node.f = nodes[iNode].f;
		Temp_node.r = nodes[iNode].r;
		
		fwrite(&Temp_node,sizeof(NODE),1,pwadFile);
	}
	Dir[writeidx++].size=tot_nodes * sizeof(NODE);
	}

	//sectors...
   {
   SECTOR      sector;
   ULONG       iSector;
   
   Dir[writeidx].offset = ftell (pwadFile);
   // Like sidedefs, sectors are modified at run-time.  The first byte of
   // the texture name is converted to an index into the textures array.
   // Put it back to normal for saving.
   for (iSector = 0; iSector < tot_sectors; iSector++)
   {
	   UBYTE       iTexture;
	   
      //memcpy (&sector, &sectors[iSector], sizeof (SECTOR));
      sector.fh = (SHORT)sectors[iSector].fh;
      sector.ch = (SHORT)sectors[iSector].ch;
      sector.light = sectors[iSector].light;
      sector.special = sectors[iSector].special;
      sector.tag = sectors[iSector].tag;

      iTexture = sector.f_flat[0];
      if (iTexture == 0 && sector.f_flat_noTexture == 1)
         sector.f_flat[0] = '-';
      else
         strncpy (sector.f_flat, textures[iTexture].name, 8);

      iTexture = sector.c_flat[0];
      if (iTexture == 0 && sector.c_flat_noTexture == 1)
         sector.c_flat[0] = '-';
      else
         strncpy (sector.c_flat, textures[iTexture].name, 8);

      fwrite (&sector, sizeof (SECTOR), 1, pwadFile);
   }
   Dir[writeidx++].size = sizeof (SECTOR) * tot_sectors;
   }


   //load_level() doesn't read reject, so we don't need to save it...
	//(I don't believe they're needed in DOOM/DCK either.)
   // Dir[writeidx].offset = ftell (pwadFile);
	// Dir[writeidx++].size=fwrite("Not Used",sizeof(char),9,pwadFile)*sizeof(char);
	// [d8-14-96 JPC] We are doing something with the REJECT chunk after all,
	// so save it.
   Dir[writeidx].offset = ftell (pwadFile);
   Dir[writeidx++].size = fwrite (rejects, sizeof(UBYTE), tot_rejects, pwadFile);
	

   // [JPC] blockmaps are needed for the current (Birthright) engine.
   Dir[writeidx].offset = ftell (pwadFile);
   fwrite (&blockm_header, sizeof (BLOCKMAP), 1, pwadFile);
	Dir[writeidx++].size = sizeof (BLOCKMAP) +
      fwrite (&blockm[4], sizeof(SHORT), tot_blockms, pwadFile) * sizeof(SHORT);

	//write Directory...
   DirStart = ftell (pwadFile);
	entries=fwrite(Dir,sizeof(IWAD_ENTRY),11,pwadFile);

	fseek(pwadFile,4,SEEK_SET);
	fwrite(&entries,sizeof(long),1,pwadFile);
	fwrite(&DirStart,sizeof(long),1,pwadFile);

	fclose(pwadFile);
   return TRUE;
}*/


// ---------------------------------------------------------------------------
BOOL GetFileName (HWND i_hwnd, LPCSTR i_szDirName, LPCSTR i_szFilter, LPCSTR i_szExt,
   LPSTR o_szReturnName, LPSTR o_szReturnTitle, DWORD i_dwflags, int openOrSave)
{
// [d5-10-96 17:41 JPC]
// First written for FLIKKA.
// Use the common dialog to get a file name.
// i_ prefix means input-only parameter.
// o_ prefix means output-only parameter.
// io_ prefix means input/output parameter.
// [d5-24-96 JPC] Added code to prevent permanent change of directory.
// Intent is to fix bug in loading new pwads that aren't in the default
// directory (C:\NOVA).
// ---------------------------------------------------------------------------

   OPENFILENAME   ofn;
   BOOL           retval;
   char           szcwd[_MAX_PATH]; // current working directory


   if (getcwd(szcwd, _MAX_PATH ) == NULL)
   {
      MessageBox (NULL, "Could not get current working directory", "Error", MB_OK);
      return FALSE;
   }

   // Set all structure members to zero.
   memset(&ofn, 0, sizeof(OPENFILENAME));
   o_szReturnName[0]  = 0;             // make sure buffer is empty
   o_szReturnTitle[0] = 0;             // ...or GetOpenFileName will fail.

   ofn.lStructSize = sizeof(OPENFILENAME);
   ofn.hwndOwner = i_hwnd;
   ofn.lpstrFilter = i_szFilter;
   ofn.nFilterIndex = 1;
   ofn.lpstrFile= o_szReturnName;
   ofn.nMaxFile = _MAX_PATH;           // not sizeof(o_szReturnName);
   ofn.lpstrFileTitle = o_szReturnTitle;
   ofn.nMaxFileTitle = _MAX_PATH;      // not sizeof(o_szReturnTitle);
   ofn.lpstrInitialDir = i_szDirName;
   ofn.Flags = /* OFN_SHOWHELP | */ OFN_HIDEREADONLY | i_dwflags;
   ofn.lpstrDefExt = i_szExt;

   if (openOrSave == GFN_OPEN)
      retval = GetOpenFileName(&ofn);
   else
      retval = GetSaveFileName(&ofn);

   if (chdir (szcwd))
      fatal_error ("Could not restore old working directory");

   return retval;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
