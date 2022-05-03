/* =®RM250¯=======================================================================
   Copyright (c) 1990,1995	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: LEVEL.C   -handles loading for nova
   Author:   Chris Phillips

   ========================================================================
   Contains the following internal functions:
   load_blockmap       -loads the load_blockmap from the wad
   load_rejects        -loads the reject table from the wad
   load_things         -loads the load_things from the wad
   load_linedefs       -loads the load_linedefs from the wad
   load_sidedefs       -loads the load_sidedefs from the wad
   load_vertexes       -loads the load_vertexes from the wad
   load_segs           -loads the load_segs from the wad
   load_ssectors       -loads the load_ssectors from the wad
   load_nodes          -loads the load_nodes from the wad
   load_sectors        -loads the load_sectors from the wad
   load_wall_textures  -loads the load_wall_textures from the wad
   load_flats          -loads the load_flats from the wad

   Contains the following general functions:
   load_level          -handles the loading of a doom-style wad.


   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#if defined (_EDIT)
#include <windows.h>
#include "edit.h"
#endif

#include "DEBUG.H"

#include "SYSTEM.H"
#include "ENGINE.H"
#include "ENGINT.H"
#include "MACHINE.H"
#include "DYNAMTEX.H"
#include "DOORS.H"
#include "LIGHT.H"
#include "TASK.H"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
   //REVISIT:Ug! this is a big time hack. Needs to be simpified,
   //          Add runtime BSP.
   //          ETC.
   //          ETC.
   //          ETC.
   //          ETC.

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
extern void SetLoadingProgress( LONG Percent );

static void load_blockmap(IWAD_ENTRY *ent,FILE *);
static void load_reject(IWAD_ENTRY *ent, FILE *);
static void load_things(IWAD_ENTRY *ent, FILE *);
static void load_linedefs(IWAD_ENTRY *ent, FILE *);
static void load_sidedefs(IWAD_ENTRY *ent, FILE *);
static void load_vertexes(IWAD_ENTRY *ent, FILE *);
static void load_segs(IWAD_ENTRY *ent, FILE *);
static void load_ssectors(IWAD_ENTRY *ent, FILE *);
static void load_nodes(IWAD_ENTRY *ent, FILE *);
static void load_sectors(IWAD_ENTRY *ent, FILE *);
static void load_wall_textures(void);
// static void load_flats(void );

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
FILE		*	fptr;

CSTRPTR	level_name = "E1M1";
CHAR		pwad_name[40] = "castle86.wad";
CHAR		pwad_path[80] = "";

// GWP NOTE: If you add any new variables, be sure
//           and zero them out again in PurgeLevel!
THING *things = 0;
LINEDEF *linedefs = 0;
BR_SIDEDEF *sidedefs = 0;
BR_VERTEX *vertexs = 0;
SEG *segs = 0;
BR_SSECTOR *ssectors = 0;
BR_NODE *nodes = 0;
BR_SECTOR *sectors = 0;
BYTE * gSectorLight = 0;							// [d10-08-96 JPC]
UBYTE *rejects = 0;
BLOCKMAP blockm_header;
USHORT * blockm_offs = 0;
SHORT * blockm = 0;
LONG blockm_size = 0;
// [d10-21-96 JPC] "Mirror" variables to suppress redundant draws of
// duplicate (but mirrored) line segments.
SEGMIRROR * segMirror = 0;
SEGMIRROR_DRAWN * segMirrorDrawn = 0;

ULONG tot_things = 0;
ULONG tot_linedefs = 0;
ULONG tot_sidedefs = 0;
ULONG tot_vertexs = 0;
ULONG tot_segs = 0;
ULONG tot_ssectors = 0;
ULONG tot_nodes = 0;
ULONG tot_sectors = 0;
ULONG tot_rejects = 0;
ULONG tot_blockms = 0;

LONG	cbWad_Uses = 0;
BOOL	gfWadLoaded = FALSE;					// [d8-08-96 JPC] A status variable
													// that may be needed.  Delete if not.

extern char wad_name[];
extern TEXTURE obj_graphics[];
extern BOOL JustLoadWad;					// TRUE if program invoked with "w wadname.wad"

/* ========================================================================
   Function    - load_level
   Description - handles the loading of a doom-style wad.
   Returns     - void
   ======================================================================== */

void load_level(char *name, LONG PlayerStart)
{
FILE	*fit;
FILE	*DataReadFit;
long	num;
long	id;
long	h;
LONG	cbMem_Before;
char	lev[30];
int	iSegment;									// [d11-05-96 JPC]
IWAD_ENTRY ent;

	cbMem_Before = ReportFreeMem(TRUE);		/* mem free before wad loaded */

	// set the pwad_name string
	if (name != NULL)
	{
		if (!JustLoadWad)
			sprintf(pwad_name,"%s%s",pwad_path,name);
		else
		{
			// Program invoked with the "w wadname.wad" command.
			// Do NOT try to locate the wad in the install path, because
			// the w command lets you specify where the wad is.
			sprintf(pwad_name,"%s",name);
		}
	}
//	printf("trying to load %s\n",pwad_name);

	fit=FileOpen(pwad_name,"rb");
	if(fit==NULL)
		fatal_error("Can't open PWAD file \"%s\"",pwad_name);
	
	SetLoadingProgress(20);
	RunMenus();
	RunMenus();
	
	// GWP I open the same file once again for reading the data blocks and
	//     lseeking around without disturbing the header block data.
	DataReadFit=FileOpen(pwad_name,"rb");
	
	allocate_spans();
	fread(&id,sizeof(long),1,fit);	/*id*/
	fread(&num,sizeof(long),1,fit);	/*num items*/
	fread(&h,sizeof(long),1,fit);	/*table offset*/
	fseek(fit,h,SEEK_SET);
	
	SetLoadingProgress(35);
	RunMenus();
	
	strcpy(lev,level_name);
	strupr(lev);
	while(strcmp(lev,ent.name)!=0)
	{
		if(fread(&ent,sizeof(IWAD_ENTRY),1,fit)==0)	/*entry*/
			fatal_error("Cant find level %s",lev);
	}
	SetLoadingProgress(40);
	RunMenus();
	fread(&ent,sizeof(IWAD_ENTRY),1,fit);	/*things*/
	load_things(&ent, DataReadFit);
	SetLoadingProgress(45);
	RunMenus();
	fread(&ent,sizeof(IWAD_ENTRY),1,fit);	/*linedefs*/
	load_linedefs(&ent, DataReadFit);
	SetLoadingProgress(50);
	RunMenus();
	fread(&ent,sizeof(IWAD_ENTRY),1,fit);	/*sidedefs*/
	load_sidedefs(&ent, DataReadFit);
	SetLoadingProgress(55);
	RunMenus();
	fread(&ent,sizeof(IWAD_ENTRY),1,fit);	/*vertexes*/
	load_vertexes(&ent, DataReadFit);
	SetLoadingProgress(60);
	RunMenus();
	fread(&ent,sizeof(IWAD_ENTRY),1,fit);	/*segs*/
	load_segs(&ent, DataReadFit);
	SetLoadingProgress(65);
	RunMenus();
	fread(&ent,sizeof(IWAD_ENTRY),1,fit);	/*ssectors*/
	load_ssectors(&ent, DataReadFit);
	SetLoadingProgress(70);
	RunMenus();
	fread(&ent,sizeof(IWAD_ENTRY),1,fit);	/*nodes*/
	load_nodes(&ent, DataReadFit);
	SetLoadingProgress(75);
	RunMenus();
	fread(&ent,sizeof(IWAD_ENTRY),1,fit);	/*sectors*/
	load_sectors(&ent, DataReadFit);
	SetLoadingProgress(80);
	RunMenus();
	fread(&ent,sizeof(IWAD_ENTRY),1,fit);	/*reject*/
	load_reject(&ent, DataReadFit);
	SetLoadingProgress(85);
	RunMenus();
	//reject( 10, 20 ); //TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST!
	fread(&ent,sizeof(IWAD_ENTRY),1,fit);	/*blockm*/
	load_blockmap(&ent, DataReadFit);
	FileClose(fit);
	FileClose(DataReadFit);

	/* do initialization while checking for player start location */
	init_things(&PlayerStart);

	SetLoadingProgress(90);
	RunMenus();
	
#if WRC_CAMERA_TEST
	DTCheckLinedefs();			//set up the Dynamic Textures
	CrossRefCameras();
#endif
	PlayerArrival(&PlayerStart);


	debugf("Free memory before textures...%ld\n", ReportFreeMem(TRUE));
	load_sky_textures();
	load_wall_textures();
	// UNUSED load_flats();
	debugf("Free memory after textures...%ld\n", ReportFreeMem(TRUE));
	cbWad_Uses = cbMem_Before - ReportFreeMem(TRUE);	/* mem used by wad */

#if 01
	// [d11-05-96 JPC] Now go through all the mirror segments and
	// mark the transparent segments as NOT mirrored.  (This will allow
	// the flip test, which applies to transparent textures only, to work
	// in draw_wall in RENDER.C.)
	for (iSegment = 0; iSegment < tot_segs; ++iSegment)
	{
		extern TEXTURE	textures[MAX_TEXTURES];

		if (segMirror[iSegment] != -1)
		{
			if ((textures[seg_to_texture_num(iSegment,FRONT,MIDDLE_TEXTURE)].type == TRANSP_TEX) ||
				(textures[seg_to_texture_num(iSegment,BACK,MIDDLE_TEXTURE)].type == TRANSP_TEX))
			{
				segMirror[segMirror[iSegment]] = -1;	
				segMirror[iSegment] = -1;	
			}
			
		}
	}
#endif

	gfWadLoaded = TRUE;
}

/* ========================================================================
   Function    - load_blockmap
   Description - loads the blockmap from the wad.
   Returns     - void
   ======================================================================== */
static void load_blockmap(IWAD_ENTRY *ent, FILE *fi)
{
// [d8-15-96 JPC] Adjust size of blockmap elements read to allow for
// the blockmap header.
// ---------------------------------------------------------------------------

	//FILE *fi;
	LONG	size;
	LONG	numRead;

	//fi=FileOpen(pwad_name,"rb");
	fseek(fi,ent->offset,SEEK_SET);
	fread(&blockm_header,sizeof(BLOCKMAP),1,fi);
	size = ent->size - sizeof(BLOCKMAP);
	blockm = (SHORT *)zalloc(ent->size+16);

	tot_blockms = size/2;
	numRead = fread(&blockm[4],sizeof(SHORT),tot_blockms,fi);
	if (numRead < tot_blockms)
	{
		tot_blockms = numRead;
	}
	blockm_offs = (USHORT*) &blockm[4];
	// GWP EXPAND_FACTOR == 1.
	// GWP blockm_header.xo*=EXPAND_FACTOR;
	// GWP blockm_header.yo*=EXPAND_FACTOR;
	//fclose(fi);
}

/* ========================================================================
   Function    - load_reject
   Description - loads the reject table from the wad.
   Returns     - void
   ======================================================================== */
static void load_reject(IWAD_ENTRY *ent, FILE *fi)
{
	//FILE *fi;
	LONG numRead;

	//fi=FileOpen(pwad_name,"rb");
	fseek(fi,ent->offset,SEEK_SET);
	tot_rejects = ent->size;
	rejects = (UBYTE *)zalloc(ent->size);
	numRead = fread(&rejects[0],sizeof(UBYTE),tot_rejects,fi);
	if (numRead < tot_rejects)
	{
		tot_rejects = numRead;
	}
	//fclose(fi);
}

/* ========================================================================
   Function    - load_things
   Description - loads the things from the wad.
   Returns     - void
   ======================================================================== */
static void load_things(IWAD_ENTRY *ent, FILE *fi)
{
// GWP THING t;
//FILE *fi;
//ULONG i=0;
LONG numRead;

	//fi=FileOpen(pwad_name,"rb");
	fseek(fi,ent->offset,SEEK_SET);
	tot_things=ent->size/sizeof(THING);
	if (tot_things==0 || tot_things > 10000)
		fatal_error("LEVEL ERROR - Total things=%ld in wad %s\n",tot_things,pwad_name);
	things=(THING *)zalloc(ent->size+1);

	// GWP while(ftell(fi)<ent->offset+ent->size)
	// GWP 	{
	// GWP 	fread(&t,sizeof(THING),1,fi);
	// GWP 	// GWP EXPAND_FACTOR == 1.
	// GWP 	// GWP t.x*=EXPAND_FACTOR;
	// GWP 	// GWP t.y*=EXPAND_FACTOR;
	// GWP 	things[i++]=t;
	// GWP 	}
	numRead = fread(&things[0],sizeof(THING), tot_things,fi);
	if (numRead < tot_things)
	{
		tot_things = numRead;
	}
	
	//fclose(fi);
}

/* ========================================================================
   Function    - load_linedefs
   Description - loads the linedefs from the wad.
   Returns     - void
   ======================================================================== */

static void load_linedefs(IWAD_ENTRY *ent, FILE * fi)
{
// GWP LINEDEF t;
//FILE *fi;
// GWP ULONG i=0;
LONG numRead;

	//fi=FileOpen(pwad_name,"rb");
	fseek(fi,ent->offset,SEEK_SET);
	tot_linedefs=ent->size/sizeof(LINEDEF);
	if (tot_linedefs==0 || tot_linedefs > 10000)
		fatal_error("LEVEL ERROR - Total linedefs=%ld in wad %s\n",tot_linedefs,pwad_name);
	linedefs=(LINEDEF *)zalloc(ent->size+1);

	// GWP while(ftell(fi)<ent->offset+ent->size)
	// GWP 	{
	// GWP 	fread(&t,sizeof(LINEDEF),1,fi);
	// GWP 	linedefs[i++]=t;
	// GWP 	}
	numRead = fread(&linedefs[0], sizeof(LINEDEF), tot_linedefs, fi);
	if (numRead < tot_linedefs)
	{
		tot_linedefs = numRead;
	}
	//fclose(fi);
}

/* ========================================================================
   Function    - load_sidedefs
   Description - loads the sidedefs from the wad.
   Returns     - void
   ======================================================================== */

static void load_sidedefs(IWAD_ENTRY *ent, FILE * fi)
{
SIDEDEF *t;
SHORT	SidedefHandle;
//FILE *fi;
ULONG i=0;
ULONG status;                          // [d6-06-96 JPC]--ignore status for now
LONG numRead;

	//fi=FileOpen(pwad_name,"rb");
	fseek(fi,ent->offset,SEEK_SET);
	tot_sidedefs=ent->size/sizeof(SIDEDEF);
	if (tot_sidedefs==0 || tot_sidedefs > 10000)
		fatal_error("LEVEL ERROR - Total sidedefs=%ld in wad %s\n",tot_sidedefs,pwad_name);
	
	SidedefHandle = NewBlock(sizeof(SIDEDEF) * tot_sidedefs);
	SetBlockAttr(SidedefHandle, LOCKED, LOCKED);
	t = (SIDEDEF *) BLKPTR(SidedefHandle);

	// GWP while(ftell(fi)<ent->offset+ent->size)
	// GWP 	{
	// GWP 	fread(&t,sizeof(SIDEDEF),1,fi);
	// GWP 	sidedefs[i++]=t;
	// GWP 	}
	numRead = fread(&t[0], sizeof(SIDEDEF), tot_sidedefs, fi);
	if (numRead < tot_sidedefs)
	{
		tot_sidedefs = numRead;
	}
	
	sidedefs=(BR_SIDEDEF *)zalloc(sizeof(BR_SIDEDEF) * tot_sidedefs);
	
	for (i = 0; i < tot_sidedefs; ++i)
	{
		sidedefs[i].xoff = t[i].xoff;
		sidedefs[i].yoff = t[i].yoff;
		sidedefs[i].n1=get_texture((char*) &t[i].n1[0], &status);
		sidedefs[i].n2=get_texture((char*) &t[i].n2[0], &status);
		sidedefs[i].n3=get_texture((char*) &t[i].n3[0], &status);
#if defined(_EDIT)
		sidedefs[i].n1_noTexture = 0;
		sidedefs[i].n2_noTexture = 0;
		sidedefs[i].n3_noTexture = 0;
#endif
		sidedefs[i].sec_ptr = t[i].sec_ptr;
	}
	
	DisposBlock(SidedefHandle);
	//fclose(fi);
}

/* ========================================================================
   Function    - load_vertexes
   Description - loads the vertices from the wad.
   Returns     - void
   ======================================================================== */

static void load_vertexes(IWAD_ENTRY *ent, FILE * fi)
{
SHORT VertexHandle;
VERTEX *t;
//FILE *fi;
ULONG i=0;
ULONG numRead;

	//fi=FileOpen(pwad_name,"rb");
	fseek(fi,ent->offset,SEEK_SET);
	tot_vertexs=ent->size/sizeof(VERTEX);
	if (tot_vertexs==0 || tot_vertexs > 10000)
		fatal_error("LEVEL ERROR - Total vertexs=%ld in wad %s\n",tot_vertexs,pwad_name);
	// GWP vertexs=(SPOINT *)zalloc(ent->size+1);
	VertexHandle = NewBlock(sizeof(VERTEX) * tot_vertexs);
	SetBlockAttr(VertexHandle, LOCKED, LOCKED);
	t = (VERTEX *) BLKPTR(VertexHandle);

	// GWP while(ftell(fi)<ent->offset+ent->size)
	// GWP 	{
	// GWP 	fread(&t,sizeof(SPOINT),1,fi);
	// GWP 	// GWP EXPAND_FACTOR == 1
	// GWP 	// GWP t.x*=EXPAND_FACTOR;
	// GWP 	// GWP t.y*=EXPAND_FACTOR;
	// GWP 	vertexs[i++]=t;
	// GWP 	}
	
	numRead = fread(&t[0],sizeof(VERTEX),tot_vertexs,fi);
	if (numRead < tot_vertexs)
	{
		tot_vertexs = numRead;
	}
	
	vertexs=(BR_VERTEX *)zalloc(tot_vertexs * sizeof(BR_VERTEX));
	
	for(i = 0; i < tot_vertexs; ++i)
	{
		vertexs[i].x=t[i].x;
		vertexs[i].y=t[i].y;
	}
	//fclose(fi);
	DisposBlock(VertexHandle);
}

/* ========================================================================
   Function    - load_segs
   Description - loads the segs from the wad.
   Returns     - void
   ======================================================================== */

static void load_segs(IWAD_ENTRY *ent, FILE * fi)
{
	// GWP SEG t;
	//FILE *fi;
	ULONG i=0;
	ULONG j;
	ULONG numRead;

	//fi=FileOpen(pwad_name,"rb");
	fseek(fi,ent->offset,SEEK_SET);
	tot_segs=ent->size/sizeof(SEG);
	if (tot_segs==0 || tot_segs > 10000)
		fatal_error("LEVEL ERROR - Total segs=%ld in wad %s\n",tot_segs,pwad_name);
	segs=(SEG *)zalloc(ent->size+1);

	// GWP while(ftell(fi)<ent->offset+ent->size)
	// GWP 	{
	// GWP 	fread(&t,sizeof(SEG),1,fi);
	// GWP 	segs[i++]=t;
	// GWP 	}
	numRead = fread(&segs[0], sizeof(SEG), tot_segs, fi);
	if (numRead < tot_segs)
	{
		tot_segs = numRead;
	}
	//fclose(fi);

	// [d10-21-96 JPC] Implement Chris Phillips's idea about avoiding
	// redundant draws of mirrored segs.
	// NOTE that this WILL slow down loading WADs.  The function to
	// compute mirrors does n squared / 2 comparisons where n is the
	// number of line segments.  For a complex WAD with 8000 line
	// segments, this is 32 million times through the loop.
	// If the idea seems worth keeping, we can precalculate the mirror
	// segment table and just load it with the WAD.

	// Define the segMirror arrays.
	segMirror = (SEGMIRROR *) zalloc(tot_segs * sizeof (SEGMIRROR));
	segMirrorDrawn = (SEGMIRROR_DRAWN *) zalloc(tot_segs * sizeof (SEGMIRROR_DRAWN));

	// Init the segMirror array.
	memset (segMirror, 0xFF, tot_segs * sizeof (SEGMIRROR));

#if 1
	// Flag all the mirrored segs.
	for (i = 0; i < tot_segs; ++i)
	{
		if (segMirror[i] == -1)
		{
			LONG const Seg_i_a = segs[i].a;
			LONG const Seg_i_b = segs[i].b;
			
			for (j = i+1; j < tot_segs; ++j)
			{
				LONG const Seg_j_a = segs[j].a;
				LONG const Seg_j_b = segs[j].b;
				
				if (((Seg_i_a == Seg_j_a) && (Seg_i_b == Seg_j_b))
					||
					((Seg_i_a == Seg_j_b) && (Seg_i_b == Seg_j_a)))
				{
					segMirror[i] = j;
					segMirror[j] = i;
					break;
				}
			}
		}
	}
#endif
}

/* ========================================================================
   Function    - load_ssectors
   Description - loads the ssectors from the wad.
   Returns     - void
   ======================================================================== */

static void load_ssectors(IWAD_ENTRY *ent, FILE * fi)
{
SHORT SsectorHandle;
ULONG i=0;
SSECTOR *t;
ULONG numRead;

//FILE *fi;
	//fi=FileOpen(pwad_name,"rb");
	fseek(fi,ent->offset,SEEK_SET);
	tot_ssectors=ent->size/sizeof(SSECTOR);
	if (tot_ssectors==0 || tot_ssectors > 10000)
		fatal_error("LEVEL ERROR - Total ssectors=%ld in wad %s\n",tot_ssectors,pwad_name);
		
	SsectorHandle = NewBlock((1 + tot_ssectors) * sizeof(SSECTOR));
	SetBlockAttr(SsectorHandle, LOCKED, LOCKED);
	t = (SSECTOR *) BLKPTR(SsectorHandle);

	// GWP while(ftell(fi)<ent->offset+ent->size)
	// GWP 	{
	// GWP 	fread(&t,sizeof(SSECTOR),1,fi);
	// GWP 	ssectors[i++]=t;
	// GWP 	}
	numRead = fread(&t[0], sizeof(SSECTOR), tot_ssectors, fi);
	if (numRead < tot_ssectors)
	{
		tot_ssectors = numRead;
	}
	
	ssectors=(BR_SSECTOR *)zalloc(sizeof(BR_SSECTOR) * tot_ssectors);
	
	// Convert the data to our internal structure.
	for (i = 0; i < tot_ssectors; ++i)
	{
		ssectors[i].n = t[i].n;
		ssectors[i].o = t[i].o;
	}
	
	DisposBlock(SsectorHandle);
	//fclose(fi);
}

/* ========================================================================
   Function    - load_nodes
   Description - loads the nodes from the wad.
   Returns     - void
   ======================================================================== */

static void load_nodes(IWAD_ENTRY *ent, FILE * fi)
{
NODE *t;
SHORT NodeHandle;
//FILE *fi;
ULONG i=0;
LONG numRead;

	//fi=FileOpen(pwad_name,"rb");
	fseek(fi,ent->offset,SEEK_SET);
	tot_nodes=ent->size/sizeof(NODE);
	if (tot_nodes==0 || tot_nodes > 10000)
		fatal_error("LEVEL ERROR - Total nodes=%ld in wad %s\n",tot_nodes,pwad_name);
	// GWP nodes=(NODE *)zalloc(ent.size+1);
	
	NodeHandle = NewBlock(sizeof(NODE) * tot_nodes);
	SetBlockAttr(NodeHandle,LOCKED,LOCKED);
	t = (NODE *)BLKPTR(NodeHandle);

	// GWP while(ftell(fi)<ent->offset+ent->size)
	// GWP 	{
	// GWP 	fread(&t,sizeof(NODE),1,fi);
	// GWP 	// GWP EXPAND_FACTOR == 1
	// GWP 	// GWP t.x*=EXPAND_FACTOR;
	// GWP 	// GWP t.y*=EXPAND_FACTOR;
	// GWP 	nodes[i++]=t;
	// GWP 	}
	
	numRead = fread(&t[0], sizeof(NODE), tot_nodes, fi);
	if (numRead < tot_nodes)
	{
		tot_nodes = numRead;
	}
	
	nodes=(BR_NODE *)zalloc((tot_nodes * sizeof(BR_NODE)) + 1);
	
	for (i = 0; i < tot_nodes; ++i)
	{
		nodes[i].x=t[i].x;
		nodes[i].y=t[i].y;
		nodes[i].a=t[i].a;
		nodes[i].b=t[i].b;
		// GWP Unused nodes[i].minyf = t[i].minyf;
		// GWP Unused nodes[i].maxyf = t[i].maxyf;
		// GWP Unused nodes[i].minxf = t[i].minxf;
		// GWP Unused nodes[i].maxxf = t[i].maxxf;
		// GWP Unused nodes[i].minyb = t[i].minyb;
		// GWP Unused nodes[i].maxyb = t[i].maxyb;
		// GWP Unused nodes[i].minxb = t[i].minxb;
		// GWP Unused nodes[i].maxxb = t[i].maxxb;
		nodes[i].f=t[i].f;
		nodes[i].r=t[i].r;
	}
	DisposBlock(NodeHandle);
	//fclose(fi);
}

/* ========================================================================
   Function    - load_sectors
   Description - loads the sectors from the wad.
   Returns     - void
   ======================================================================== */

static void load_sectors(IWAD_ENTRY *ent, FILE * fi)
{
SECTOR *t;
SHORT SectorHandle;
//FILE *fi;
ULONG i=0;
LONG numRead;

	//fi=FileOpen(pwad_name,"rb");
 	fseek(fi,ent->offset,SEEK_SET);
	tot_sectors=ent->size/sizeof(SECTOR);
	if (tot_sectors==0 || tot_sectors > 10000)
		fatal_error("LEVEL ERROR - Total sectors=%ld in wad %s\n",tot_sectors,pwad_name);
		
	SectorHandle=NewBlock(ent->size+1);
	SetBlockAttr(SectorHandle, LOCKED, LOCKED);
	t = (SECTOR *) BLKPTR(SectorHandle);
	
	gSectorLight = (BYTE *)zalloc(tot_sectors); // [d10-08-96 JPC]
	// GWP while(ftell(fi)<ent->offset+ent->size)
	// GWP 	{
	// GWP 	fread(&t,sizeof(SECTOR),1,fi);
	// GWP 	// GWP EXPAND_FACTOR == 1;
	// GWP 	// GWP t.fh*=EXPAND_FACTOR;
	// GWP 	// GWP t.ch*=EXPAND_FACTOR;
	// GWP 	sectors[i++]=t;
	// GWP 	}
	numRead = fread(&t[0], sizeof(SECTOR), tot_sectors, fi);
	if (numRead < tot_sectors)
	{
		tot_sectors = numRead;
	}
	
	sectors=(BR_SECTOR *)zalloc(sizeof(BR_SECTOR) * tot_sectors);
	
	//fclose(fi);
	for (i = 0; i < tot_sectors; ++i)
	{
		sectors[i].fh		= t[i].fh;
		sectors[i].ch 		= t[i].ch;
		sectors[i].f_flat	= get_floor_texture((char*) & t[i].f_flat[0]);
		sectors[i].c_flat	= get_floor_texture((char*)&t[i].c_flat[0]);
		sectors[i].light	= t[i].light;
		sectors[i].special	= t[i].special;
		sectors[i].tag		= t[i].tag;
	}
	
	DisposBlock(SectorHandle);
}

/* ========================================================================
   Function    - load_wall_textures
   Description - loads the wall textures from the wad.
   Returns     - void
   ======================================================================== */


static void load_wall_textures(void)
{
ULONG status;                          // [d6-06-96 JPC]--ignore status for now
                                       // (used in EDIT.C)
#if 01
// [d9-09-96 JPC] Make certain the switch textures are loaded.
	get_texture ("BRIKCSW1", &status);
	get_texture ("BRIKCSW2", &status);
	get_texture ("BRIKESW1", &status);
	get_texture ("BRIKESW2", &status);
	get_texture ("DUNGCSW1", &status);
	get_texture ("DUNGCSW2", &status);
#endif
}


/* ========================================================================
   Function    - PurgeLevel
   Description -
   Returns     - void
   ======================================================================== */
void PurgeLevel(void)
{
	gfWadLoaded = FALSE;

	// [d8-08-96 JPC]
	// We need to remove tasks immediately!
	// Note that this is harmless if function is not a registered task.
	remove_task(handle_doors);
	remove_task(HandleLifts);
	remove_task(HandleFloors);
	remove_task(HandleCeilings);
	remove_task(TextureFrameHandler);
	// no longer used: remove_task(SectorFrameHandler);
#if defined (_EDIT)
	SendMessage(ghwndEditWallDlg, WM_CLOSE, 0, 0);
#endif

	printf("Freemem at end of level: %ld\n", ReportFreeMem(TRUE));
	init_doors();								// [d11-14-96 JPC] removes all sounds
	purge_all_things();
	purge_all_textures();

	SetPurgeClass(CLASS2);			/* purge all things and textures */
	DisposClass(CLASS1);				/* remove all zallocED memory */

	// GWP Zero out the global vars.
	things = 0;
	linedefs = 0;
	sidedefs = 0;
	vertexs = 0;
	segs = 0;
	ssectors = 0;
	nodes = 0;
	sectors = 0;
	gSectorLight = 0;							// [d10-08-96 JPC]
	rejects = 0;
	blockm_offs = 0;
	blockm = 0;
	blockm_size = 0;
	segMirror = 0;
	segMirrorDrawn = 0;

	tot_things = 0;
	tot_linedefs = 0;
	tot_sidedefs = 0;
	tot_vertexs = 0;
	tot_segs = 0;
	tot_ssectors = 0;
	tot_nodes = 0;
	tot_sectors = 0;
	tot_rejects = 0;
	tot_blockms = 0;

	cbWad_Uses = 0;

	printf("Freemem after purge: %ld\n", ReportFreeMem(TRUE));
}

/* ========================================================================
   Function    - load_new_wad
   Description - loads a new wad.
   Returns     - void
   ======================================================================== */

void load_new_wad(char *name, LONG PlayerStart)
{
	// GWP printf("Freemem at end of level: %ld\n", ReportFreeMem(TRUE));

	// GWP Copied from PurgeLevel
	//     Why don't we call PurgeLevel here instead of duplicating 90%
	//     the code??
	PurgeLevel();
	// GWP // [d8-08-96 JPC]
	// GWP // We need to remove tasks immediately!
	// GWP // Note that this is harmless if function is not a registered task.
	// GWP remove_task(handle_doors);
	// GWP remove_task(HandleLifts);
	// GWP remove_task(HandleFloors);
	// GWP remove_task(HandleCeilings);
	// GWP remove_task(TextureFrameHandler);
	// GWP 
	// GWP // no longer used: remove_task(SectorFrameHandler);
	// GWP init_doors ();								// [d11-14-96 JPC] removes all sounds
	// GWP purge_all_things();
	// GWP purge_all_textures();
	// GWP SetPurgeClass(CLASS2);			/* purge all things and textures */
	// GWP DisposClass(CLASS1);				/* remove all zallocED memory */

	printf("Freemem before loading %s: %ld\n", name, ReportFreeMem(TRUE));
	load_level(name,PlayerStart);
}

/* ========================================================================
   Function    - SetWadPath
   Description - Set up the path of where the .WAD file is
   Returns     - void
   ======================================================================== */
void SetWadPath ( CSTRPTR pWadPath )
{
	/* copy the first 79 chars into pwad_path */
	strncpy(pwad_path, pWadPath, 79);
}

// ===========================================================================
#if !defined (_RELEASE)
void CreateResourceScript ()
{
// [d3-20-97 JPC] If user invoked DOS NOVA with "g wadname," generate a
// resource script for the WAD.
// Example:
//		nova g wads\braem_mw.wad
//
// Note that you have to specify both the WAD directory and the .WAD extension.

	FILE *		fit;
	long			num;
	long			id;
	long			h;
	char			lev[30];
	IWAD_ENTRY	ent;
	THING			t;
	int			cThings;
	int			i;
	int			filepos;
	char			szWadName[_MAX_PATH];
	extern char	pwad[];						// name of WAD file, initialized in
													// MACHINE.C: parse_cmdline.

	sprintf (szWadName,"%s", pwad);
	fit = FileOpen(szWadName, "rb");
	if (fit == NULL)
		fatal_error("Can't open PWAD file \"%s\"", szWadName);
	
	fread(&id,sizeof(long),1,fit);	/*id*/
	fread(&num,sizeof(long),1,fit);	/*num items*/
	fread(&h,sizeof(long),1,fit);	/*table offset*/
	fseek(fit,h,SEEK_SET);
	strcpy(lev,level_name);
	strupr(lev);
	while(strcmp(lev,ent.name)!=0)
	{
		if(fread(&ent,sizeof(IWAD_ENTRY),1,fit)==0)	/*entry*/
			fatal_error("Can't find level %s",lev);
	}

	fread (&ent, sizeof(IWAD_ENTRY), 1, fit);	/*things*/
	cThings = ent.size/sizeof(THING);
	fseek (fit, ent.offset, SEEK_SET);
	for (i = 0; i < cThings; ++i)
	{
		filepos = ftell (fit);
		// fseek (fit, filepos, SEEK_SET);
		fread (&t, sizeof(THING), 1, fit);
		printf ("Thing %d type = %d\n", i, t.type);	
	}
	FileClose (fit);
}
#endif

// ===========================================================================


