/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: THINGS.C     -Manages things in nova
   Author: Chris Phillips, Alan Clark, Wes Cumberland
   ========================================================================
   Contains the following internal functions:
   no_draw_sort           - sorts things

   Contains the following general functions:
   load_obj_graphic       - loads a graphic for an object
   create_thing           - validates a new entry in the things table
   init_things            - takes wad-loaded things data, and turns it to nova data
   set_thing              - set the xyza of a thing
   move_thing             - moves a thing in nova???
   move_thing_to          - moves a thing in nova???
   remove_thing           - removes a thing from a sector in nova???
   add_thing              - adds a thing to a sector in nova???
   clear_thing_spans      - clears the thing spans variable
   scale_obj              - scales an object according to it's distance from the POV
   draw_thing_spans       - ???
   draw_thing             - draws a thing
   draw_things            - draw all the things in nova sector?
   sort_things            - sorts things

   ======================================================================== */
/* ®RM250¯------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _JPC
#include "debug.h"
#endif
#include "SYSTEM.H"
#include "ENGINE.H"
#include "ENGINT.H"
#include "MACHINE.H"
#include "DYNAMTEX.H"
#include "THINGS.H"
#ifdef INET
#include "inet.h"
#endif
#include "LIGHT.H"
#include "SPECIAL.H"
#include "THINGTYP.H"

extern BOOL fBattleCombat;
extern LONG clip_line(POINT* a, POINT* b);
extern UBYTE				shade_table[];

#define QUIETDEGRADE 1

#if defined (_VC4)
#include <crtdbg.h>
#define ASSERT _ASSERT
#endif

#if defined(_DEBUG) && defined(_WINDOWS) && defined(_JPC)
static char gszSourceFile[] = __FILE__;	// for ASSERT
#endif

#if !defined (ASSERT)
	#if defined(_DEBUG)
		#ifdef _WINDOWS
				#define ASSERT(arg) if(!(arg)) SS_Assert(gszSourceFile, __LINE__)
		#else
				#define ASSERT(arg) {if(!(arg))fatal_error("ASSERTION FAILED %s,%li\n",gszSourceFile, __LINE__);}
		#endif	
	#else
		#define ASSERT(__ignore) ((void)0)
	#endif
#endif

/* ------------------------------------------------------------------------
   Notes
	------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile flags
   ------------------------------------------------------------------------ */
#define HEIGHT_DIFF_OK	32					// for AddThingToAdjacentSubsectors

//GEH this should live somewhere else.  I will try to move it later.
typedef struct _MinMem {
	ULONG base;
	ULONG newbase;
} MINMEMCONVERT;

// additional battle field conversions
MINMEMCONVERT MinMemAdditionalTable[] = {
//  Base Type			New Type
	{T_LORD_MALE_1     ,LORES_A_OFF},			// Troop Lord Male 1
	{T_ELF_LADY_LORD   ,LORES_E_OFF},			// Troop Elf Lady Lord
	{T_WIZARD_MALE_4   ,LORES_A_OFF},			// Troop Wizard Male 4
	{T_LORD_MALE_2     ,LORES_A_OFF},			// Troop Lord Male 2
	{T_LORD_MALE_3     ,LORES_A_OFF},			// Troop Lord Male 3
	{T_LORD_MALE_4     ,LORES_A_OFF},			// Troop Lord Male 4
	{T_LORD_MALE_5     ,LORES_A_OFF},			// Troop Lord Male 5
	{T_LORD_MALE_6     ,LORES_A_OFF},			// Troop Lord Male 6
	{T_LORD_MALE_7     ,LORES_A_OFF},			// Troop Lord Male 7
	{T_LORD_FEMALE_1   ,LORES_A_OFF},			// Troop Lord Female 1
	{T_GUILDER_FEMALE  ,LORES_A_OFF},			// Troop Guilder Female
	{T_LORD_FEMALE_2   ,LORES_A_OFF},			// Troop Lord Female 2
	{T_LORD_FEMALE_3   ,LORES_A_OFF},			// Troop Lord Female 3
	{T_WIZARD_FEMALE_1 ,LORES_A_OFF},			// Troop Wizard Female 1
	{T_WIZARD_FEMALE_2 ,LORES_A_OFF},			// Troop Wizard Female 2
	{T_WIZARD_MALE_1   ,LORES_A_OFF},			// Troop Wizard Male 1
	{T_WIZARD_MALE_2   ,LORES_A_OFF},			// Troop Wizard Male 2
	{T_WIZARD_MALE_3   ,LORES_A_OFF},			// Troop Wizard Male 3
	{T_GUILDER_MALE_1  ,LORES_A_OFF},			// Troop Guilder Male 1
	{T_GUILDER_MALE_2  ,LORES_A_OFF},			// Troop Guilder Male 2
	{T_PRIEST_MALE_1   ,LORES_A_OFF},			// Troop Priest Male 1
	{T_PRIEST_MALE_2   ,LORES_A_OFF},			// Troop Priest Male 2
	{T_PRIEST_FEMALE_1 ,LORES_A_OFF},			// Troop Priest Female 1
	{T_ASSASSIN		   ,LORES_A_OFF}, 			// Troop Assassin
	{T_DWARF_OFFICER_1 ,LORES_D_OFF},			// Troop Dwarf Officer
	{T_DWARF_OFFICER_2 ,LORES_D_OFF},			// Troop Dwarf Officer 2
	{T_DWARF_OFFICER_3 ,LORES_D_OFF},			// Troop Dwarf Officer 3
	{T_GOBLIN_ZOMBIE   ,LORES_G_OFF},			// Troop Goblin Zombie
	{T_GOBLIN_OFFICER  ,LORES_G_OFF},			// Troop Goblin Officer
	{T_GOBLIN_QUEEN    ,LORES_G_OFF},			// Troop Goblin Queen
	{T_ZOMBIE          ,LORES_G_OFF},			// Troop Zombie
};
SHORT MinMemAdditionalTableMax = sizeof(MinMemAdditionalTable) / sizeof(MINMEMCONVERT);	

/*	ננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננ
	Every 10000 thing_spans is another 280 K of RAM in the code! --Wes
	Sorry Wes, I will seriously reduce this when I have an opportunity to
	turn the actinic illumination of my intellect upon it. --Alan
	ננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננ */
#define UNIT_SCALE_ADJUST		100

/* ------------------------------------------------------------------------
   Typedefs and Structures
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
//#define CHECK_THING_INDEX(t,s)	if (t<0 || t>=MAX_THINGS) fatal_error("ERROR: t out of range in %s: %ld",t,s)
#define CHECK_THING_INDEX(t,s)	;
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
static ULONG RestrictAniSeq(ULONG seq);
static void Load_Bitm(SHORT *piBitm, ULONG type);
static SHORT load_PCX_sequence (ULONG iAnim, ULONG type, ULONG seq, ULONG Rotation);
void WriteErrFile(char *n);
void beep(void);
void RemoveAnim (SHORT iAnim);
SHORT NextAnimFrame (SHORT iAnim, USHORT iSeq, USHORT iRot, UBYTE * pCtrl, SHORT frame);
SHORT NextPCXFrame (SHORT iAnim, SHORT iSeq, SHORT iRot);
ERRCODE decode_frame (ANIMPTR pAnim, FLICHEADPTR pHead);
void decode_byte_run (PTR pData, ANIMPTR pAnim);
void decode_literal (PTR pData, ANIMPTR pAnim);
void decode_delta_flc (PTR pData, ANIMPTR pAnim);
SHORT _OpenBitm (SHORT w, USHORT h, BOOL fLocked);
void draw_map_line(POINT a,POINT b,ULONG color);
static void scale_obj (LONG t, LONG sx, LONG dx, LONG dy, LONG dye, LONG clipped, LONG src_inc, LONG light);
void sort_things_in_ssector (LONG ss);
LONG  get_scale(SHORT iAnim);
USHORT get_center_point(SHORT iAnim);
static BOOL IsAViewBlocker(LONG ThingIndex);
static LONG GetArtType(LONG ThingIndex);
static SHORT SetPCXSequence(SHORT iAnim, SHORT iSeq);
static void MarkAllAnimPurgable(SHORT iAnim);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

//char THING_PATH[] = "things\\";

LONG		MaxThingSpans = MAX_HIRES_THING_SPANS;
MYTHING *	mythings = NULL;
LONG	   	*ssector_things;

// JPC: prevent dead things from being clipped at the edge of a subsector.
// [d11-07-96 JPC] Abandon attempt to fix; uses way too many system resources,
// slowing everything down and eventually crashing when we run out of thing spans!
#define EXTRATHINGS 0		// set to 0 to remove this code (for speed testing)
									// NOTE! Conform to #define in AVATAR.CPP.
#if EXTRATHINGS
static LONG	subsectorExtraThings[MAX_SSECTORS];
#endif

THING_SPAN	*thing_spans;
LONG			tot_thing_spans;
LONG			last_obj_graphic=0;
LONG			objClicked = -1;	// item number clicked on
WadThingType	typeClicked = iNOTHING;	// type of thing clicked on
LONG			butClicked = 0;		// which button clicked
LONG			torchh,torchw;
char			cAction[MAX_ANIMATIONSEQ] = {'S','W','D','A','F','X','C','E','L','G','B','M','Z'};
char			cRotation[] = {'0','3','5','7','9'};
//static PTR	TTypePtr = NULL;
//static LONG	G_TTypeSize = 0;
//static LONG	G_NumberOfTTypes = 0;
PTR	TTypePtr = NULL;
LONG	G_TTypeSize = 0;
LONG	G_NumberOfTTypes = 0;
LONG	MaxThingLoaded = 0;

typedef struct {
	BOOL	SetFlag;
	LONG	ThingIndex;
} DONT_DRAW_INFO;

static DONT_DRAW_INFO gDontDrawInfo = { FALSE, fERROR };


typedef struct {
	SHORT	*pMagicItems;
	LONG	NumberOfMagicItems;
	LONG	QuestItem;
	LONG	QuestItemMythingsIndex;
	LONG	MagicThingType;
	LONG	QuestThingType;
} MAGIC_ITEMS_INFO;

static MAGIC_ITEMS_INFO gMagicItems = { 0, 0, 0, -1, -1 };

// for internet
LONG			inetStartX, inetStartY, inetStartA;
BOOL			fDisplayTitles = FALSE;

// Debugging
#ifdef _DEBUG
#ifdef _JPC
static unsigned int giType1 = 23; 		// infantry
static unsigned int giType2 = 163;		// potted plant 1
LONG   gRemapIndex = 0;
#endif
// static SHORT giSector = 1;
static SHORT giDebugThing = 274;
static int gDebug;
#endif
static SHORT debug_cTypes[300];


extern PTR	pTexture;
extern LONG	wTexture;
extern LONG	hTexture;

extern CAMERA_DESCRIPTOR* LevelCameras;
#if WRC_CAMERA_TEST	
extern BOOL	VideoCurrentlyRedirected;
#endif


/* =======================================================================
   Function    - IsAViewBlocker
   Description - Test whether a thing can Block the camera's view.
   Returns     - BOOL
	======================================================================= */

static BOOL IsAViewBlocker(LONG ThingIndex)
{
   return ((((TTYPE *)(TTypePtr + (G_TTypeSize * mythings[ThingIndex].type)))->attrib & VIEWBLKR) ?
		   	TRUE : FALSE);
}

/* ========================================================================
   Function    - GetArtType
   Description - Get whether the art is a PCX or FLC or BITMAP
   Returns     - The art type bit.
   ======================================================================== */

static LONG GetArtType(LONG type)
{
   return (((TTYPE *)(TTypePtr + (G_TTypeSize * type)))->attrib & ART_TYPE_MASK);
}

/* =======================================================================
   Function    - IsBumpable
   Description - Test whether a thing can be bumped.
   Returns     - BOOL
	======================================================================= */

BOOL IsBumpable(LONG ThingIndex)
{
   return ((((TTYPE *)(TTypePtr + (G_TTypeSize * mythings[ThingIndex].type)))->attrib & NOBUMP) ?
		   	FALSE : TRUE);
}
/* =======================================================================
   Function    - HangsFromCeiling
   Description - Test whether a thing belongs on the ceiling.
   Returns     - BOOL
	======================================================================= */

BOOL HangsFromCeiling(LONG ThingIndex)
{
   return ((((TTYPE *)(TTypePtr + (G_TTypeSize * mythings[ThingIndex].type)))->attrib & HANGS) ?
		   	TRUE : FALSE);
}

/* =======================================================================
   Function    - FlyingThing
   Description - Test whether a thing can fly
   Returns     - BOOL
	======================================================================= */

BOOL IsFlyingThing(LONG ThingIndex)
{
   return ((((TTYPE *)(TTypePtr + (G_TTypeSize * mythings[ThingIndex].type)))->attrib & FLIES) ?
		   	TRUE : FALSE);
}
/* =======================================================================
   Function    - CanStandOn
   Description - Test whether a thing can be stood on top of.
   Returns     - BOOL
	======================================================================= */

BOOL CanStandOn(LONG ThingIndex)
{
   return ((((TTYPE *)(TTypePtr + (G_TTypeSize * mythings[ThingIndex].type)))->attrib & NOSTAND) ?
		   	FALSE : TRUE);
}
/* ========================================================================
	Function     -GetThingName
	Description  -Return the thing name in game type table
	Returns      -char*
	========================================================================*/
static char* GetThingName(LONG type)
{
	return (((TTYPE*)(TTypePtr+(type*G_TTypeSize)))->name);
}
	
/* ========================================================================
	Function     -GetLowResIndex
	Description  -Return the thing low res index from game type table
	Returns      -char*
	========================================================================*/
static SHORT GetLowResIndex(LONG type)
{
  return ((TTYPE*)(TTypePtr+(type*G_TTypeSize)))->LowResIndex;
}
/* =======================================================================
   Function    - IsHalfWidthBumpable
   Description - Test whether a thing can be bumped thru on the outer quarter
   Returns     - BOOL
	======================================================================= */

BOOL IsHalfWidthBumpable(LONG ThingIndex)
{
   return ((((TTYPE *)(TTypePtr + (G_TTypeSize * mythings[ThingIndex].type)))->attrib & PARTBUMP) ?
		   	TRUE : FALSE);
}

/* =======================================================================
   Function    - ChangeThingType
   Description - Toss the old art and reset the type to this new type.
   Returns     -
	======================================================================= */
void ChangeThingType(LONG ThingIndex, LONG NewType)
{
	ULONG const iSequence = mythings[ThingIndex].iSequence;
	UBYTE const bControl = mythings[ThingIndex].bControl;
	SHORT const iAnim = mythings[ThingIndex].iAnim;
	SBYTE CurrentRotation = 0;
	
	// Don't do anything if there is no change.
	if (mythings[ThingIndex].type == NewType)
		return;
		
	if (iAnim != fERROR) 	// Is this a FLC or PCX?
	{
		char	cpBuffer[40];
		UBYTE	ubCurrentFrame = 0;
		BOOL iAnimLocked = IsLocked(iAnim);
		ANIMPTR	pAnim;
		
		if (!iAnimLocked)
			SetQuickLock(iAnim);
		
		pAnim = (ANIMPTR)BLKPTR(iAnim);
		
		CurrentRotation = pAnim->rotation;
		ubCurrentFrame = pAnim->frame;
		
		// Release all the old animations.
		MarkAllAnimPurgable(iAnim);
		
		// Switch to the new animation frames.
		sprintf(cpBuffer, "%s.IDA", GetThingName(NewType));
		pAnim->hiData = GetResourceStd(cpBuffer, FALSE);
		
		load_obj_graphic(ThingIndex, NewType, iSequence, CurrentRotation, bControl, bControl);
		
		// Refresh pointer.
		//pAnim = (ANIMPTR) BLKPTR(iAnim);
		
		if (pAnim->type == TYPE_FLIC &&			// PCX's don't have frames.
		    pAnim->hiData > 0)
		{
			I_DATA_BLK *phiData;
			SHORT iHead;
			BOOL hiDataLocked = IsLocked(pAnim->hiData);
			
			if (!hiDataLocked)
				SetQuickLock(pAnim->hiData);
				
			phiData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
			
			iHead = phiData->iData[(iSequence * NUMROTATIONS) + CurrentRotation];
			
			// Make sure that load_flc_sequence() succeeded.
			if (iHead > 0)
			{
				UBYTE i;
				FLICHEADPTR pHead;
				BOOL iHeadLocked = IsLocked(iHead);
				
				if (!iHeadLocked)
					SetQuickLock(iHead);
				
				pHead = (FLICHEADPTR) BLKPTR(iHead);
				
				// Advance to the current frame.
				for (i = pAnim->frame;
				     i < ubCurrentFrame && i < pHead->frames;
				     ++i)
				{
					++pAnim->frame;
					decode_frame(pAnim, pHead);
				}
				
				if (!iHeadLocked)
					ClrLock(iHead);
			}
			
			if (!hiDataLocked)
				ClrLock(pAnim->hiData);
		}
		if (!iAnimLocked)
			ClrLock(iAnim);
	}
	else if (mythings[ThingIndex].iBitm != fERROR)	/* check for valid graphic */
	{
		SetPurge(mythings[ThingIndex].iBitm);	/* purge if resource */
		// Clear the CLASS2 flag so we don't do this again.
		SetClassPerm(mythings[ThingIndex].iBitm);
		mythings[ThingIndex].iBitm = fERROR;
		load_obj_graphic(ThingIndex, NewType, iSequence, CurrentRotation, bControl, bControl);
	}
	
}
/* ========================================================================
   Function    - GetMinMemConvert
   Description - convert things to a ultra low memory configuration
   Returns     - ULONG newthing
   ======================================================================== */
ULONG GetMinMemConvert(ULONG type)
{
//GEH I will try and come back to this later.  I'm not sure where
// this data could live that is better then searching a list for matches
	SHORT i;
	ULONG orig_type = type;
	ULONG lowres_type;
	
	lowres_type = GetLowResIndex(type);
	if(lowres_type != NOLOW)
		return lowres_type;
	
	if(fBattleCombat)
	{
		for(i=0;i<MinMemAdditionalTableMax;++i)
		{
			if(MinMemAdditionalTable[i].base == type)
				return MinMemAdditionalTable[i].newbase;
		}
	}
	
	// not found, use original type
	return orig_type;
}
/* =======================================================================
   Function    - fill_sequence_handles
   Description - handle non-existant rotations by filling in with valid handles
   Returns     - void
	======================================================================= */
void fill_sequence_handles (SHORT iAnim, LONG seq)
{
	BOOL iAnimLocked = IsLocked(iAnim);
	ANIMPTR	pAnim;
	ULONG		iseq = seq * NUMROTATIONS;
	ULONG		rot;
	SHORT		iResHandle;
	I_DATA_BLK *piData;
	BOOL		hiDataLocked;

	if (!iAnimLocked)
		SetQuickLock(iAnim);
		
	pAnim = (ANIMPTR)BLKPTR(iAnim);
	if (pAnim->hiData < 0)
	{
		if (!iAnimLocked)
			ClrLock(iAnim);
		return;
	}
		
	hiDataLocked = IsLocked(pAnim->hiData);
	if (!hiDataLocked)
		SetQuickLock(pAnim->hiData);
		
	piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
	
	// Find the first good rotation.
	for (rot=0; rot<NUMROTATIONS; ++rot)
	{
		iResHandle = piData->iData[iseq+rot];
		if (iResHandle > 0)
		{
			piData->iData[iseq] = iResHandle;			/* fix rot 0 */
			break;
		}
	}

	for (rot=1; rot<NUMROTATIONS; ++rot)
	{
		SHORT iseqrot = iseq+rot;
		
		if (piData->iData[iseqrot] < 0)
		{
			piData->iData[iseqrot] = piData->iData[iseqrot-1];
		}
	}
	
	if (!hiDataLocked)
		ClrLock(pAnim->hiData);
		
	if (!iAnimLocked)
		ClrLock(iAnim);
}

/* =======================================================================
   Function    - load_PCX_sequence
   Description - Attempt to load set of PCX's for a sequence
   Returns     - iAnim handle
	======================================================================= */
static SHORT load_PCX_sequence (ULONG iAnim, ULONG ttype, ULONG seq, ULONG Rotation)
{
	char		n[40];
	char		fn[40];
	ANIMPTR		pAnim = 0;
	ULONG		iseq;
	BOOL		iAnimLocked;
		
	if (iAnim == fERROR)
		return iAnim;
		
	CHECK_THING_INDEX(t,"load_PCX_sequence");
	
	/* ------------------------------------------ */
	/* Attempt to load as a set of PCXs */
	/* ------------------------------------------ */
	// GWP Instead just load one rotation. Let the draw load the rest if/when needed.
	// GWP Also don't fill in the handles, thus forcing a reload when drawn.
	
	iseq = seq * NUMROTATIONS;
	
	sprintf(n,"%s", GetThingName(ttype));
	
	sprintf(fn,"%sPCX\\%s%c%c.pcx",THING_PATH,n,cAction[seq],cRotation[Rotation]);
	iAnim = OpenAnim(iAnim, iseq + Rotation, fn, TYPE_PCX);
		
	iAnimLocked = IsLocked(iAnim);
	if (!iAnimLocked)
		SetQuickLock(iAnim);
		
	pAnim = (ANIMPTR)BLKPTR(iAnim);
	
	if (pAnim->hiData > 0)
	{
		I_DATA_BLK	*piData;
		BOOL		hiDataLocked = IsLocked(pAnim->hiData);
		
		if (!hiDataLocked)
			SetQuickLock(pAnim->hiData);
			
		piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
		if (piData->iData[Rotation] == fERROR)
		{
			ULONG		rot;
			
			// We couldn't get the requested rotation, so load'em all.
			for (rot=0; rot<NUMROTATIONS; ++rot)
			{
				sprintf(fn,"%sPCX\\%s%c%c.pcx",THING_PATH,n,cAction[seq],cRotation[rot]);
				OpenAnim(iAnim, iseq + rot, fn, TYPE_PCX);
			}
			fill_sequence_handles(iAnim, seq);		/* handle non-existant rot */
		}
		
		if (!hiDataLocked)
			ClrLock(pAnim->hiData);
	}
	
	if (!iAnimLocked)
		ClrLock(iAnim);
		
	return iAnim;
}
/* =======================================================================
   Function    - load_FLC_sequence
   Description - Load a flic for a sequence if available.
   Returns     - iAnim handle
	======================================================================= */
SHORT load_FLC_sequence(ULONG iAnim, ULONG ttype, ULONG seq, ULONG Rotation)
{
	ULONG		iseq, rot;
	char		n[40];
	char		fn[40];
	ANIMPTR		pAnim = 0;
	I_DATA_BLK* piData;

	if (iAnim == fERROR)
		return iAnim;

	CHECK_THING_INDEX(t, "load_FLC_sequence");

	if (seq > MAX_ANIMATIONSEQ)
	{
#if defined (_DEBUG)
		fatal_error("ERROR - seq number bad (%ld) in load_FLC_sequence\n", seq);
#else
		return iAnim;
#endif
	}

	//if (iAnim == (SHORT)fERROR)
	//	return;

	/* get base filename from registered TType array */
	/* GEH also check for the ultra low memory config */
	/* and swap out some of the animations for another */
	iseq = seq * NUMROTATIONS;
	if (seq > MAX_ANIMATIONSEQ)
	{
#if defined(_DEBUG)
		fatal_error("ERROR - seq number bad (%ld) in load_FLC_sequence\n", seq);
#else
		return iAnim;
#endif
	}

	sprintf(n, "%s", GetThingName(ttype));

	// First try to get only the rotation requested.
	sprintf(fn, "%s%s\\%s%c%c.flc", THING_PATH, n, n, cAction[seq], cRotation[Rotation]);
	OpenAnim(iAnim, iseq + Rotation, fn, TYPE_FLIC);

	pAnim = (ANIMPTR)BLKPTR(iAnim);	// Refresh the pointer.
	if (pAnim->hiData > 0)
	{
		piData = (I_DATA_BLK*)BLKPTR(pAnim->hiData);
		if (piData->iData[iseq + Rotation] > 0)
		{
			// Got it!
			return iAnim;
		}
	}
	else
		return iAnim;	// Sorry pAnim is missing the hiData.

	// This rotation doesn't exist, we must try for all of them  then fill in.
	for (rot = 0; rot < NUMROTATIONS; ++rot)
	{
		sprintf(fn, "%s%s\\%s%c%c.flc", THING_PATH, n, n, cAction[seq], cRotation[rot]);
		OpenAnim(iAnim, iseq + rot, fn, TYPE_FLIC);
	}


	fill_sequence_handles(iAnim, seq);		/* handle non-existant rot */

	return iAnim;
}

/* =======================================================================
   Function    - load_obj_graphic
   Description - loads a graphic for an object
   Returns     - the index of the new graphic in the obj_graphic table
   ======================================================================== */	
static void load_obj_graphic(LONG i,
					  ULONG ttype,
					  ULONG AnimationSeq,
					  SBYTE rotation,
					  UBYTE bFLCControl,
					  UBYTE bPCXControl)
{
	SHORT		iAnim = fERROR;
	char		n[40];
	char		fn[128];
	char		pn[128];
	
	mythings[i].type = ttype;

	/* check for valid TType array */
	if (TTypePtr == NULL)
	{
		printf("WARNING - TTypes array not registered in load_obj_graphic\n");
		goto LoadGenthing;
	}

	/* check for type out of range */
	if (ttype > G_NumberOfTTypes)
	{
		printf("ERROR - THINGS.C: thing type number greater than %ld: %ld\n",ttype, G_NumberOfTTypes);
		goto LoadGenthing;
	}

	/* get base filename from registered TType array */
	/* GEH also check for the ultra low memory config */
	/* and swap out some of the animations for another */
		LONG ArtType = GetArtType(ttype);
		if (ArtType == FLC_ART)
			goto Art_Is_FLC;
		if (ArtType == BITMAP_ART)
			goto Art_Is_BITMAP;
		if (ArtType == PCX_ART)
			goto Art_Is_PCX;
				
		sprintf(n,"%s", GetThingName(ttype));

	printf("Type:%03ld - %s\n",ttype,n);

	sprintf(fn,"%s%s\\%sS0.flc",THING_PATH,n,n);
	sprintf(pn,"%sPCX\\%sS0.pcx",THING_PATH,n);

	/* ------------------------------------------ */
	/* Attempt to load as an animation (TYPEANIM) */
	/* ------------------------------------------ */
	if (Exists(fn))			/* check for STAND @ 0 rotation */
	{
		ANIMPTR pAnim = 0;
Art_Is_FLC:

		iAnim = mythings[i].iAnim;
		if (iAnim == fERROR)
		{
			iAnim = mythings[i].iAnim = AllocateAnim(TYPE_FLIC, mythings[i].type);
		}
		else
		{
			MarkOldAnimPurgable(iAnim, mythings[i].iSequence);
		}
		
		load_FLC_sequence(iAnim, mythings[i].type, AnimationSeq, rotation);
		SetAnimSequence(iAnim, (AnimationSeq * NUMROTATIONS) + rotation);
		
		// Mark'm purgable in case we don't draw them, we can flush them out.
		MarkOldAnimPurgable(iAnim, AnimationSeq);
		
		mythings[i].iSequence = AnimationSeq;
		mythings[i].bControl  = bFLCControl;
		
		pAnim = (ANIMPTR) BLKPTR(iAnim);
		// pAnim->iBuff could have changed in SetAnimSequence.
		mythings[i].iBitm = pAnim->iBuff;
		if (mythings[i].iBitm != fERROR)
		{
			BITMPTR pBitm = (BITMPTR) BLKPTR(mythings[i].iBitm);
			// Note: We use the bitmap width because the art is rotated 90Deg.
			mythings[i].heiScaled = (pBitm->w*UNITARY_SCALE)/pBitm->scale;
			mythings[i].widScaled = (pBitm->h*UNITARY_SCALE)/pBitm->scale;
		}
	}

	/* ------------------------------------------ */
	/* Attempt to load as a set of PCXs */
	/* ------------------------------------------ */
	else if (Exists(pn))
	{
		ANIMPTR pAnim = 0;
		
Art_Is_PCX:
		iAnim = mythings[i].iAnim;
		if (iAnim == fERROR)
		{
			iAnim = mythings[i].iAnim = AllocateAnim(TYPE_PCX, mythings[i].type);
		}
		else
		{
			MarkOldAnimPurgable(iAnim, mythings[i].iSequence);
		}
		
		load_PCX_sequence(iAnim, mythings[i].type, AnimationSeq, rotation);
		SetPCXSequence(iAnim, (AnimationSeq * NUMROTATIONS) + rotation);
		
		// Mark'm purgable in case we don't draw them, we can flush them out.
		MarkOldAnimPurgable(iAnim, AnimationSeq);
		
		mythings[i].iSequence = AnimationSeq;
		mythings[i].bControl  = bPCXControl;
		
		pAnim = (ANIMPTR) BLKPTR(iAnim);
		mythings[i].iBitm = pAnim->iBuff;
		
		if (mythings[i].iBitm != fERROR)
		{
			BITMPTR pBitm = (BITMPTR) BLKPTR(mythings[i].iBitm);
			// Note: We use the bitmap width because the art is rotated 90Deg.
			mythings[i].heiScaled = (pBitm->h*UNITARY_SCALE)/pBitm->scale;
			mythings[i].widScaled = (pBitm->w*UNITARY_SCALE)/pBitm->scale;
		}
	}

	/* --------------------------------------------- */
	/* Otherwise, try to load as a bitmap (TYPEBITM) */
	/* --------------------------------------------- */
	else
	{
Art_Is_BITMAP:
		Load_Bitm(&mythings[i].iBitm, mythings[i].type);

		if (mythings[i].iBitm == fERROR)
		{
#if defined (_JPC)
			TRACE("ERROR - THINGS.C: Loading GENTHING, could not find: %s (%ld)\n",pn, ttype);
#endif
			printf("WARNING - %s (%ld) not found in load_obj_graphic, loading GENTHING\n",pn,ttype);

LoadGenthing:
			{
			ULONG Gentype;
			
			// GEH substitute valid item
			//GEH type = gMagicItems.pMagicItems[random(gMagicItems.NumberOfMagicItems)];
			Gentype = BALM_OF_HEALING;
			
			/* get base filename from registered TType array */
			Load_Bitm(&mythings[i].iBitm, Gentype);

			}
		}

		mythings[i].iAnim = fERROR;
		if (mythings[i].iBitm == fERROR)
		{
#if defined (_DEBUG)
			fatal_error("ERROR - Unable to load thing file: %s\n",n);
#else
			return;
#endif
		}
		
		
		if (mythings[i].iBitm != fERROR)
		{
			BITMPTR pBitm = (BITMPTR) BLKPTR(mythings[i].iBitm);
			// Note: We use the bitmap width because the art is rotated 90Deg.
			mythings[i].heiScaled = (pBitm->w*UNITARY_SCALE)/pBitm->scale;
			mythings[i].widScaled = (pBitm->h*UNITARY_SCALE)/pBitm->scale;
			
			SetPurge(mythings[i].iBitm);
		}
	}
}

/* ========================================================================
   Function    - RegisterTTypes
   Description -
   Returns     -
	======================================================================== */
void RegisterTTypes (PTR _TTypePtr, LONG _G_TTypeSize, LONG NumberOfTTypes)
{
	TTypePtr = _TTypePtr;
	G_TTypeSize = _G_TTypeSize;
	G_NumberOfTTypes = NumberOfTTypes;
}

/* ========================================================================
   Function    - create_thing
   Description - creates a thing and puts it in the table
   Returns     - the index that it put the thing in
   ======================================================================== */
LONG create_thing(ULONG type, LONG x, LONG y, LONG z)
{
	LONG			i;
	// GWP BITMPTR		pBitm;

	for (i = 0; i < MAX_THINGS; ++i)
		if (!mythings[i].valid)
			break;

	if (i > MaxThingLoaded)
	{
		MaxThingLoaded = i;
	}

	if (i >= MAX_THINGS)
	{
		printf("ERROR - Requested too many things, exceeded MAX_THINGS (%d)\n", MAX_THINGS);
		for (i = 0; i < 300; ++i)
			debug_cTypes[i] = 0;
		for (i = 0; i < MAX_THINGS; ++i)
		{
			if (!mythings[i].valid)
			{
				++debug_cTypes[mythings[i].type];
			}
		}
		for (i = 0; i < 300; ++i)
			if (debug_cTypes[i])
				printf("#%ld - %s: %d", i, GetThingName(type), debug_cTypes[i]);
#if defined(_DEBUG)		
		fatal_error("ERROR - Requested too many things, exceeded MAX_THINGS (%d)\n", MAX_THINGS);
#else
		return fERROR;
#endif
	}

	// Always start with the lowest resolution possible. We'll
	// Res it up if needed.
	SHORT lowResIndex = GetLowResIndex(type);

	if (lowResIndex != -1)
	{
		SHORT UltraLowRes = GetLowResIndex(lowResIndex);
		if (UltraLowRes != -1)
		{
			type = UltraLowRes;
		}
		else
		{
			type = lowResIndex;
		}
	}

	mythings[i].iSequence = 0;	// set to something reasonable.
	mythings[i].iAnim = fERROR;
	if (type == 0)
	{
		// [d11-05-96 JPC] 0 is a dummy type for adding sound effects
		// to doors, lifts, floor crushers, and ceiling crushers.
		mythings[i].type = 0;
		mythings[i].iBitm = fERROR;
		mythings[i].iAnim = fERROR;
		mythings[i].inVisible = TRUE;
	}
	else
	{
		load_obj_graphic(i,
			type,
			ANIMATION0SEQ,
			0,
			LOOP_FLAG | REWIND_FLAG | START_FLAG,
			IDLE_FRAME);
	}

	mythings[i].OriginalType = mythings[i].type; //WRC anything created on the
											   //fly had a garbage value in
											   //the OriginalType field

	mythings[i].x = x;
	mythings[i].y = y;
	mythings[i].z = z;
	mythings[i].valid = TRUE;
	mythings[i].ssect = -1;  //for now, will be changed when add_thing is called
	mythings[i].sect = -1;
	mythings[i].fMapSpotted = FALSE;
	mythings[i].inVisible = FALSE;
	mythings[i].Frozen = FALSE;
	mythings[i].SkipFrame = FALSE;
	mythings[i].dist = LONG_MAX;	// so we won't auto res it up.
	mythings[i].scale_adjust = UNIT_SCALE_ADJUST;
	mythings[i].ColorRemap = 0;

	return(i);
}

/* ========================================================================
   Function    - init_things
   Description - inits the nova things
   Returns     - void
   ======================================================================== */
extern LONG PlayerHeight;

void init_things (LONG * pPlayerStart)  // which player start to begin on
{
	ULONG i, ss;
	LONG t;
	LONG ta;
	BOOL fMoveCamera = FALSE;
	LONG sect;
	LONG FloorHeight;
	LONG CeilingHeight;
	LONG Special;
	LONG Tag;
	LONG UseType;
	//BOOL TorchToggle = FALSE; // I really hate this

	MaxThingSpans = MAX_HIRES_THING_SPANS;
		
	mythings=(MYTHING *)zalloc(MAX_THINGS*sizeof(MYTHING) );
	thing_spans = (THING_SPAN *)zalloc(MaxThingSpans * sizeof(THING_SPAN));
	ssector_things = (LONG *) zalloc(MAX_SSECTORS * sizeof(LONG));

	/*
	MAX_THINGS used to read as "tot_things", I made it this way to stop an
	object drawing bug,  this only takes a MAXIMUM of 22528 more bytes of
	memory, not a big loss
	*/

	if(!mythings)
		fatal_error("Out of memory in init_things");
			
	for(i=0;i<MAX_SSECTORS;++i)
	{
		ssector_things[i] = -1;	//nothing here.
#if EXTRATHINGS
		subsectorExtraThings[i] = -1;		// [d11-05-96 JPC]
#endif
	}

	// Init the mythings array.
	for(i=0;i<MAX_THINGS;++i)
 	{
		mythings[i].next_thing = -1;
		mythings[i].valid = FALSE;
	}

	for(i=0;i<tot_things;++i)
	{
		// this section could take a bit of time so we will
		// call the timer critical routine
		// GWP Too often to be called here. run_timers();
		
		if(things[i].type == 343)
			continue;
			
#ifdef _JPC
		if (i == giDebugThing)
			++gDebug;
#endif
		// in mininum case, colapse the trees and rocks

		if (things[i].type<=4 || things[i].type == TYPE_DEATHMATCH)
		{
			if((*pPlayerStart == TYPE_PLAYERSTART1 && things[i].type == 1)
			|| (*pPlayerStart == TYPE_PLAYERSTART2 && things[i].type == 2)
			|| (*pPlayerStart == TYPE_PLAYERSTART3 && things[i].type == 3)
			|| (*pPlayerStart == TYPE_PLAYERSTART4 && things[i].type == 4)
			  )
			{
				fMoveCamera = TRUE;
				*pPlayerStart = FALSE;			/* resolved, so zero out */
			}

			if (fMoveCamera)
			{
				fMoveCamera = FALSE;
				// must convert the angle from doom style to our style
				ta=((0xffff/360)*things[i].angle)/256;
				ta=(ta-63)&0xff;
				init_player(things[i].x,things[i].y,(65535-ta)&0xFF);
			
				// save the player start data for internet
				inetStartX = things[i].x;
				inetStartY = things[i].y;
				inetStartA = (65535 - ta) & 0xff;
			}

			// don't store player starts in the mythings
			continue;
		}
#if WRC_CAMERA_TEST
		else if (things[i].type == CAMERA_TYPE)
		{
			NewCameraThing(i);
			continue;
		}
#endif
		// GEH these are substitute items
		else if (things[i].type == gMagicItems.MagicThingType) 	// Random Special Item
		{
			things[i].type = BALM_OF_HEALING;
		}
		else if (things[i].type == gMagicItems.QuestThingType)	// Quest Item
		{
			// The game needs to set this variable to some object
			things[i].type = BALM_OF_HEALING;
		}
		else if (things[i].type == 424)	// Potion of Poison cure Item // GEH HACK!
		{
			// The game needs to set this variable to some object
			things[i].type = BALM_OF_HEALING;
		}
		
		UseType = things[i].type;
		// First use a bogus z.
		t = create_thing(UseType, things[i].x, things[i].y, 0);
		mythings[t].OriginalType = things[i].type;
		ss = find_ssector(things[i].x,things[i].y);
		ssector_to_sector_info(mythings[t].x,
		                       mythings[t].y,
		                       ss,
		                       &sect,
		                       &FloorHeight,
		                       &CeilingHeight,
		                       &Special,
		                       &Tag);
		
		// JPC: Trick the things into thinking the floor height
		// is lower than it really is if the sector is a water sector.
		if (sectors[sect].special == SSP_WATER ||
		    sectors[sect].special == SSP_ACID_FLOOR ||
		    sectors[sect].special == SSP_LAVA)
		{
			// Regular water: thing is 1/4 submerged.
			FloorHeight -= mythings[t].heiScaled / 4;
		}
		else if (sectors[sect].special == SSP_DEEP_WATER)
		{
			// Deep water: thing is 3/4 submerged.
			FloorHeight -= (mythings[t].heiScaled * 3) / 4;
		}

		if (HangsFromCeiling(t))
		{
			// now that the graphic is loaded put it on the ceiling.
			mythings[t].z = CeilingHeight - mythings[t].heiScaled;
		}
		else
		{
			mythings[t].z = FloorHeight;
		}

		// mythings[t].angle = ((LONG)(((float)(64*things[i].angle))/90))-64;
		// [d7-30-96 JPC] Per TCW GWP--above calculation is wrong.
		mythings[t].angle = (320 - ((LONG)(((float)(64*things[i].angle))/90))) & 0xFF;
		mythings[t].AIbits = things[i].options;
	
		// Do the following in add_thing:
		// mythings[t].sect = (SHORT)sect;
		add_thing(t, ss);
	}
}
/* ========================================================================
   Function    - purge_thing
   Description - Call this to remove a thing from the mythings array.
   Returns     - void
   ======================================================================== */
void purge_thing(LONG ThingIndex)
{
	if (mythings[ThingIndex].valid == TRUE)
	{
		if (mythings[ThingIndex].iAnim != fERROR)		/* dispose of Anim header */
		{
			// Removing the animation also throws away the iBitm.
			RemoveAnim(mythings[ThingIndex].iAnim);
			mythings[ThingIndex].iAnim = fERROR;
			mythings[ThingIndex].iBitm = fERROR;
		}
		else if (mythings[ThingIndex].iBitm != fERROR)	/* check for valid graphic */
		{
			SetPurge(mythings[ThingIndex].iBitm);	/* purge if resource */
			// Clear the CLASS2 flag so we don't do this again.
			SetClassPerm(mythings[ThingIndex].iBitm);
			mythings[ThingIndex].iBitm = fERROR;
		}
		mythings[ThingIndex].valid = FALSE;
	}
}

/* ========================================================================
   Function    - purge_all_things
   Description -
   Returns     - void
   ======================================================================== */
void purge_all_things (void)
{
	//BITMPTR	pBuff;
	SHORT		i;

	if (mythings != NULL)								/* is there mythings */
	{
		for(i=0; i<MAX_THINGS; ++i)					/* loop thru mythings */
		{
			purge_thing(i);
		}
		mythings = NULL;  // it will be disposed with the class 1 memory.
	}
	last_obj_graphic=0;
}

/* ========================================================================
   Function    - MoveFlyingThingTo
   Description - Moves a thing; keeps z coord from going
					  through floor or ceiling, but does not adjust z
					  coordinate otherwise (unlike move_thing_to).
   Returns     - void
	Notes			- Added 11-22-96 by JPC
					- Derived from move_thing_to; called only from set_thing.
   ======================================================================== */
static void MoveSetThing (LONG ThingIndex,LONG nx,LONG ny)
{
	LONG 			orig_ss;
	LONG 			new_ss;

	if (nx==mythings[ThingIndex].x&& ny==mythings[ThingIndex].y)
		return;						// premature return if thing hasn't moved
	orig_ss = mythings[ThingIndex].ssect;
	mythings[ThingIndex].x=nx;
	mythings[ThingIndex].y=ny;
	new_ss=find_ssector(mythings[ThingIndex].x,mythings[ThingIndex].y);

	if (orig_ss != new_ss)
	{
		LONG sect;
		LONG FloorHeight;
		LONG CeilingHeight;
		LONG Special;
		LONG Tag;
		
		// New subsector, need to get information.
		if(new_ss<0 || new_ss>=MAX_SSECTORS)
		{
			printf("move_thing_to: Bad ssector (%d) for mything[%d]\n",new_ss, ThingIndex);
			return;
		}
		
		remove_thing(ThingIndex);
		add_thing(ThingIndex, new_ss);
		
		ssector_to_sector_info(mythings[ThingIndex].x,
									  mythings[ThingIndex].y,
									  new_ss,
									  &sect,
									  &FloorHeight,
									  &CeilingHeight,
									  &Special,
									  &Tag);
		mythings[ThingIndex].sect = (SHORT)sect;


		// [d12-09-96 JPC] If this is a water sector, the things can
		// go lower than the nominal floor height, so adjust
		// "FloorHeight" here.  (Fixes bug in non-first-person view
		// when the player is walking in water and sometimes hops up
		// into the air when crossing a SUBsector boundary.)
		if (Special == SSP_WATER ||
		    Special == SSP_ACID_FLOOR ||
		    Special == SSP_LAVA)
		{
			FloorHeight -= mythings[ThingIndex].heiScaled / 4;
		}
		else if (Special == SSP_DEEP_WATER)
		{
			FloorHeight -= (mythings[ThingIndex].heiScaled * 3) / 4;
		}


		if (FloorHeight > mythings[ThingIndex].z)
		{
			mythings[ThingIndex].z = FloorHeight;
		}
		else
		{
			LONG ThingHeight = mythings[ThingIndex].heiScaled;

			if (CeilingHeight < (ThingHeight + mythings[ThingIndex].z))
			{
				const LONG NewHeight = CeilingHeight - ThingHeight;

				mythings[ThingIndex].z = (NewHeight > FloorHeight) ? NewHeight : FloorHeight;
			}
		}
	}
#ifdef _STATUS
	if (ThingIndex == player.ThingIndex)
	{
		LONG 			iSector;
		LONG			x, y;
		SHORT			special;

		if (!fGraphInitialized)
			return;
	
		x = mythings[ThingIndex].x;
		y = mythings[ThingIndex].y;
		iSector = point_to_sector (x, y);
		special = sectors[iSector].special;
		if (special == SSP_WATER ||
		    special == SSP_ACID_FLOOR ||
		    special == SSP_LAVA ||
			 special == SSP_DEEP_WATER)
		{
			++gDebug;
			if (mythings[ThingIndex].z == 0)
				++gDebug;
		}
	}
#endif
}


/* ========================================================================
   Function    - set_thing
   Description - sets the thing to a location and angle
   Returns     - void
	======================================================================== */
void set_thing (LONG t, LONG x, LONG y, LONG z, LONG a)
{
	// Set the Z first, then if it is outside reasonable bounds, MoveSetThing
	// will put the object between the floor and the ceiling.
#ifdef _DEBUG
	if (t == giDebugThing && mythings[t].z < 0)
		++gDebug;								// for breakpoint
#endif
	mythings[t].z = z;
	// We used to call move_thing_to, but that function changes the z coordinate
	// too much--it puts our party z on the floor even if we're flying!
	MoveSetThing (t, x, y);
	mythings[t].angle = a;
}

/* ========================================================================
   Function    - move_thing
   Description - moves a thing in nova???
   Returns     - void
   ======================================================================== */
void move_thing (LONG ThingIndex, LONG dx, LONG dy)
{
	LONG orig_ss;
	LONG new_ss;

	CHECK_THING_INDEX(t,"move_thing");

	// GWP orig_ss = find_ssector(mythings[ThingIndex].x,mythings[ThingIndex].y);
	// GWP We already know this.
	orig_ss = mythings[ThingIndex].ssect;

	mythings[ThingIndex].x+=dx;
	mythings[ThingIndex].y+=dy;
	new_ss=find_ssector(mythings[ThingIndex].x,mythings[ThingIndex].y);

	// [d9-04-96 JPC] Added the SSP_WATER and SSP_DEEP_WATER tests to the
	// following; otherwise, objects will sometimes walk on the water.
	// (The Z coordinate does not stay put; various routines like to set
	// it back to the floor.  This seems the best place to intervene.)
	// [d10-09-96 JPC] Added SSP_LAVA.
	if(orig_ss!=new_ss ||
	   sectors[mythings[ThingIndex].sect].special == SSP_WATER ||
       sectors[mythings[ThingIndex].sect].special == SSP_ACID_FLOOR ||
       sectors[mythings[ThingIndex].sect].special == SSP_LAVA ||
	   sectors[mythings[ThingIndex].sect].special == SSP_DEEP_WATER)
	{
		LONG sect;
		LONG FloorHeight;
		LONG CeilingHeight;
		LONG Special;
		LONG Tag;
		
		if (orig_ss == new_ss)
		{
			sect = mythings[ThingIndex].sect;
			FloorHeight = sectors[sect].fh;
			CeilingHeight = sectors[sect].ch;
			Special = sectors[sect].special;
		}
		else
		{
			if(new_ss<0 || new_ss>=MAX_SSECTORS)
			{
				printf("move_thing: Bad ssector (%d) for mything[%d]\n",new_ss, ThingIndex);
				return;
			}
			
			remove_thing(ThingIndex);
			add_thing(ThingIndex, new_ss);
			
			ssector_to_sector_info(mythings[ThingIndex].x,
										  mythings[ThingIndex].y,
										  new_ss,
										  &sect,
										  &FloorHeight,
										  &CeilingHeight,
										  &Special,
										  &Tag);
		}

#if 01
		// [d8-21-96 JPC] Trick the things into thinking the floor height
		// is lower than it really is if the sector is a water sector.
		if (Special == SSP_WATER ||
		    Special == SSP_ACID_FLOOR ||
		    Special == SSP_LAVA)
		{
			FloorHeight -= mythings[ThingIndex].heiScaled / 4;
		}
		else if (Special == SSP_DEEP_WATER)
		{
			FloorHeight -= (mythings[ThingIndex].heiScaled * 3) / 4;
		}
#endif

		if (IsFlyingThing(ThingIndex))
		{
			if (FloorHeight > mythings[ThingIndex].z)
			{
				mythings[ThingIndex].z = FloorHeight;
			}
			else
			{
				LONG ThingHeight = mythings[ThingIndex].heiScaled;

				if (CeilingHeight < (ThingHeight + mythings[ThingIndex].z))
				{
					const LONG NewHeight = CeilingHeight - ThingHeight;
					mythings[ThingIndex].z = (NewHeight > FloorHeight) ? NewHeight : FloorHeight;
				}
			}
		}
		else
		{
			if (HangsFromCeiling(ThingIndex) == TRUE)
			{
				mythings[ThingIndex].z = CeilingHeight - mythings[ThingIndex].heiScaled;
			}
			else
			{
				mythings[ThingIndex].z = FloorHeight;
			}
		}
	}

}

/* ========================================================================
   Function    - move_thing_to
   Description - moves a thing in nova???
   Returns     - void
   ======================================================================== */
void move_thing_to (LONG ThingIndex,LONG nx,LONG ny)
{
	LONG 			orig_ss;
	LONG 			new_ss;
	BOOL			fForceWaterCheck;			// [d9-04-96 JPC]

	CHECK_THING_INDEX(ThingIndex,"move_thing_to");

	// [d9-04-96 JPC] Added the SSP_WATER and SSP_DEEP_WATER tests to the
	// following; otherwise, objects will sometimes walk on the water.
	// (The Z coordinate does not stay put; various routines like to set
	// it back to the floor.  This seems the best place to intervene.)
	fForceWaterCheck = sectors[mythings[ThingIndex].sect].special == SSP_WATER ||
		sectors[mythings[ThingIndex].sect].special == SSP_ACID_FLOOR ||
		sectors[mythings[ThingIndex].sect].special == SSP_LAVA ||
		sectors[mythings[ThingIndex].sect].special == SSP_DEEP_WATER;

	if (!fForceWaterCheck)
		if (nx==mythings[ThingIndex].x&& ny==mythings[ThingIndex].y)
			return;						// premature return if thing hasn't moved

	// GWP orig_ss=find_ssector(mythings[ThingIndex].x,mythings[ThingIndex].y);
	// GWP We already know this.
	orig_ss = mythings[ThingIndex].ssect;
	mythings[ThingIndex].x=nx;
	mythings[ThingIndex].y=ny;
	new_ss=find_ssector(mythings[ThingIndex].x,mythings[ThingIndex].y);

	if (orig_ss != new_ss || fForceWaterCheck)
	{
		LONG sect;
		LONG FloorHeight;
		LONG CeilingHeight;
		LONG Special;
		LONG Tag;
		
		if (orig_ss == new_ss)
		{
			// We should only get here if the sector is a water sector.
			// In that case, z can get set to the wrong value (typically the
			// floor height) by some other routine, so we need to put it back
			// the way it was.
			sect = mythings[ThingIndex].sect;
			FloorHeight = sectors[sect].fh;
			CeilingHeight = sectors[sect].ch;
			Special = sectors[sect].special;
		}
		else
		{
			// New subsector, need to get information.
			if(new_ss<0 || new_ss>=MAX_SSECTORS)
			{
				printf("move_thing_to: Bad ssector (%d) for mything[%d]\n",new_ss, ThingIndex);
				return;
			}
			
			remove_thing(ThingIndex);
			add_thing(ThingIndex, new_ss);
			
			ssector_to_sector_info(mythings[ThingIndex].x,
										  mythings[ThingIndex].y,
										  new_ss,
										  &sect,
										  &FloorHeight,
										  &CeilingHeight,
										  &Special,
										  &Tag);
			mythings[ThingIndex].sect = (SHORT)sect;
		}

#if 01
		// [d8-21-96 JPC] Trick the things into thinking the floor height
		// is lower than it really is if the sector is a water sector.
		if (Special == SSP_WATER ||
		    Special == SSP_ACID_FLOOR ||
		    Special == SSP_LAVA)
		{
			FloorHeight -= mythings[ThingIndex].heiScaled / 4;
		}
		else if (Special == SSP_DEEP_WATER)
		{
			FloorHeight -= (mythings[ThingIndex].heiScaled * 3) / 4;
		}
#endif

		if (IsFlyingThing(ThingIndex) == TRUE)
		{
			if (FloorHeight > mythings[ThingIndex].z)
			{
				mythings[ThingIndex].z = FloorHeight;
			}
			else
			{
				LONG ThingHeight = mythings[ThingIndex].heiScaled;

				if (CeilingHeight < (ThingHeight + mythings[ThingIndex].z))
				{
					LONG const NewHeight = CeilingHeight - ThingHeight;

					mythings[ThingIndex].z = (NewHeight > FloorHeight) ? NewHeight : FloorHeight;
				}
			}
		}
		else
		{
			if (HangsFromCeiling(ThingIndex) == TRUE)
			{
				mythings[ThingIndex].z = CeilingHeight - mythings[ThingIndex].heiScaled;
			}
			else
			{
				mythings[ThingIndex].z = FloorHeight;
			}
		}
	}

#ifdef _STATUS
	if (ThingIndex == player.ThingIndex)
	{
		LONG 			iSector;
		LONG			x, y;
		SHORT			special;

		if (!fGraphInitialized)
			return;
	
		x = mythings[ThingIndex].x;
		y = mythings[ThingIndex].y;
		iSector = point_to_sector (x, y);
		special = sectors[iSector].special;
		if (special == SSP_WATER ||
		    special == SSP_ACID_FLOOR ||
		    special == SSP_LAVA ||
			 special == SSP_DEEP_WATER)
		{
			++gDebug;
			if (mythings[ThingIndex].z == 0)
				++gDebug;
		}
	}
#endif
}

/* =======================================================================
	
	======================================================================= */
void LoopSequenceContinuously (LONG t, ULONG sequence)
{
	CHECK_THING_INDEX(t,"StartSequenceLoop");

	if (mythings[t].valid && 
	    mythings[t].iSequence != (UBYTE) sequence &&
	    mythings[t].iAnim != fERROR)
	{
		MarkOldAnimPurgable(mythings[t].iAnim, mythings[t].iSequence);
	}
	mythings[t].iSequence = (UBYTE)sequence;
	mythings[t].bControl  = LOOP_FLAG | REWIND_FLAG	| START_FLAG;
	mythings[t].Frozen = FALSE;
	mythings[t].SkipFrame = FALSE;
}

/* =======================================================================
	
	======================================================================= */
void LoopSequenceOnce (LONG t, ULONG sequence)
{
	CHECK_THING_INDEX(t,"StartSequenceOnce");

	if (mythings[t].valid && 
	    mythings[t].iSequence != (UBYTE) sequence &&
	    mythings[t].iAnim != fERROR)
	{
		MarkOldAnimPurgable(mythings[t].iAnim, mythings[t].iSequence);
	}
	mythings[t].iSequence = (UBYTE)sequence;
	mythings[t].bControl  = REWIND_FLAG	| START_FLAG;
	mythings[t].Frozen = FALSE;
	mythings[t].SkipFrame = FALSE;
}

/* =======================================================================
	
	======================================================================= */
void PlaySequenceOnce (LONG t, ULONG sequence)
{
	CHECK_THING_INDEX(t,"StartSequenceOnce");

	if (mythings[t].valid && 
	    mythings[t].iSequence != (UBYTE) sequence &&
	    mythings[t].iAnim != fERROR)
	{
		MarkOldAnimPurgable(mythings[t].iAnim, mythings[t].iSequence);
	}
	mythings[t].iSequence = (UBYTE)sequence;
	mythings[t].bControl  = START_FLAG;
	mythings[t].Frozen = FALSE;
	mythings[t].SkipFrame = FALSE;
}

/* =======================================================================
	
	======================================================================= */
BOOL TestSequenceDone (LONG t)
{
	CHECK_THING_INDEX(t,"TestSequenceDone");
	BOOL Result = TRUE;
	
	if (mythings[t].bControl & REWIND_FLAG)
	{
		Result = ((mythings[t].bControl & (FRAME_MASK+START_FLAG)) == IDLE_FRAME);
	}
	else
	{
		if (mythings[t].iAnim > 0)
		{
			ANIMPTR pAnim = (ANIMPTR) BLKPTR(mythings[t].iAnim);
			if (pAnim->totalFrames > 0 &&
			    pAnim->frame >= pAnim->totalFrames - 1)
			{
				Result = TRUE;
			}
			else
			{
				Result = FALSE;
			}
		}
	}

	return Result;
}

/* =======================================================================
	
	======================================================================= */
LONG SequenceFrameNumber (LONG t)
{
	CHECK_THING_INDEX(t,"SequenceFrameNumber");

	return (mythings[t].bControl & FRAME_MASK);
}

/* =======================================================================
	Function : remove_thing
	Description : Remove thing t from the mythings ssector list.
	Returns :
	======================================================================= */
void remove_thing (LONG t)
{
	LONG ss;
	LONG ft;		//first thing
	LONG ct;		//current thing
#if EXTRATHINGS
	LONG iSubsector;							// [d11-05-96 JPC]
#endif

	CHECK_THING_INDEX(t,"remove_thing");

	ss=mythings[t].ssect;
	mythings[t].ssect=-1;
	mythings[t].sect=-1;
	mythings[t].valid = FALSE;


	// Doors, lifts, and so on use a dummy thing for their sound effects.
	// Handle the dummy thing here.  (Also handles cases where things
	// for some pathological reason have ssect = -1).
	if (ss == -1)
	{
		mythings[t].next_thing=-1;
		return;
	}


#if defined(_DEBUG)
	if (ss < 0 || ss >= MAX_SSECTORS)
	{
		fatal_error ("THINGS ERROR! invalid ssector %d, remove_thing.\n", ss);
	}
#endif

	ct = ft = ssector_things[ss];
	if((ULONG)ft==t ) // I am the first thing in a list
	{
		if( mythings[t].next_thing==-1) //this is first and last thing so...
		{							
			ssector_things[ss]=-1;
		}
		else
		{
			ct=mythings[t].next_thing;
			if (mythings[ct].valid == TRUE)
			{
				ssector_things[ss]=ct;
			}
			else
			{
				ssector_things[ss]=-1;
			}
			mythings[t].next_thing=-1;
		}
	}
	else
	{
		while(mythings[ct].next_thing!=(LONG)t) //find previous thing in llist
		{
			ct = mythings[ct].next_thing;
			//GEH THIS NEEDS TO DO MORE, CAUSE YOU'RE BROKEN PAST THIS POINT
			if (ct == -1 || mythings[ct].valid == FALSE || ct >= MAX_THINGS)
			{
#if defined(DEBUG)
				fatal_error("ERROR! things.c mythings array is toast.\n");
#endif
				printf("WARNING: mythings array is inconsistent.\n");
				return;
			}
		}
		mythings[ct].next_thing = mythings[t].next_thing;
		mythings[t].next_thing=-1;
	}

#if EXTRATHINGS
	// [d11-05-96 JPC] Check things that are posted to more than one
	// subsector so they won't clip (mainly for dead bodies).
	for (iSubsector = 0; iSubsector < tot_ssectors; ++iSubsector)
	{
		if (subsectorExtraThings[iSubsector] == t)
		{
			subsectorExtraThings[iSubsector] = -1;
			// No "break" here: multiple subsectors might be affected.
		}
	}
#endif
}

/* =======================================================================
	Function : add_thing
	Description : Put the item t from the mythings array into the sub list by
	              ssector.
	Returns:
	======================================================================= */
void add_thing (LONG t, LONG ss)
{
	LONG ft;		//first thing
	LONG ct;		//current thing
	LONG sect;
	LONG FloorHeight;
	LONG CeilingHeight;
	LONG Special;
	LONG Tag;
		

	CHECK_THING_INDEX(t,"add_thing");
#if defined (_DEBUG)
	if (ss < 0 || ss >= MAX_SSECTORS)
	{
		fatal_error("THINGS ERROR! add_thing Ssector out of bounds %ld.\n", ss);
	}
#endif

//	if (t==14)
//		printf("teleporter being added to ssector %li\n",ss);
//	if (ss==4)
//		printf("adding thing type %li to sector %li\n",mythings[t].type,ss);

	ft = ssector_things[ss];					// get first thing in linked list
	if (ft == -1)									// is there any thing here already?
	{
		ssector_things[ss]=t;
	}
	else
	{
		ct = ft;
		while (mythings[ct].next_thing != -1 && mythings[ct].valid == TRUE)  //find last thing in linked list
		{
			ct = mythings[ct].next_thing;
		}
		mythings[ct].next_thing = t;
	}
	
	mythings[t].next_thing = -1;
	
	mythings[t].ssect = ss;
	mythings[t].valid = TRUE;

	// [d8-13-96 JPC] Put the calculation of sect here because not
	// all callers of add_thing do this calculation.
	ssector_to_sector_info(mythings[t].x,
		                    mythings[t].y,
		                    ss,
		                    &sect,
		                    &FloorHeight,
		                    &CeilingHeight,
		                    &Special,
		                    &Tag);
	mythings[t].sect = (SHORT)sect;
}

/* =======================================================================

	======================================================================= */
/* =======================================================================

	======================================================================= */
void clear_thing_spans(void)
{
	LONG t;

	tot_thing_spans=0;

	for (t=0; t < MaxThingLoaded; ++t)
	{
		if (mythings[t].valid && mythings[t].fDrawn == FALSE)
		{
			if (mythings[t].iAnim != fERROR)
			{
				MarkOldAnimPurgable(mythings[t].iAnim, mythings[t].iSequence);
			}
			else
			{
				if (mythings[t].iBitm != fERROR)
				{
					SetPurge(mythings[t].iBitm);
				}
			}
		}
		mythings[t].fDrawnLastFrame = mythings[t].fDrawn;
		mythings[t].fDrawn = FALSE;
	}
}

/* =======================================================================

	======================================================================= */

static void scale_obj (LONG t, LONG sx, LONG dx, LONG dy, LONG dye, LONG clipped, LONG src_inc, LONG light)
{
	BITMPTR	bptr = (BITMPTR)BLKPTR(t);
	ULONG		tsy;
	//const LONG		hei = bptr->h;		// these are NOT in the original order, they have been swapped (w->h, h->w)
	//const LONG		wid = bptr->w;
	PTR		tptr = ((PTR)BLKPTR(t)) + sizeof(BITMHDR);
	PTR		sptr;
	PTR		sptr_end;
	//LONG		sy;
	ULONG		pix;
	LONG		sptr_inc; //helper for Cameraas

	// GEH a bptr of NULL means the object was purged and we haven't
	// been able to anything about that yet, so just return
	if (bptr == NULL)
//#if defined(_DEBUG)
//		fatal_error("thing.c: scale_obj() attempted to use NULL bitmap pointer.");
//#else
		return;
//#endif
		
#if !defined(_WINDOWS)
	if (fAIMoving)
	{
		if (dx & 1)
			return;
	}
#endif
	
	++dye;
	//if (sx >= hei)
	if (sx < 0 || sx >= bptr->h)
		return;

#if WRC_CAMERA_TEST	
	if (VideoCurrentlyRedirected) 	//must rotate 90deg if rendering to buf
	{
		sptr = (PTR)screen+((dx*screen_buffer_height)+dy);
		sptr_end = sptr + (dye-dy);
		sptr_inc = 1;
	}
	else
#endif
	{
		sptr = (PTR)screen+((dy*screen_buffer_width)+dx);
		sptr_end = sptr + ((dye-dy)*screen_buffer_width);
		sptr_inc = screen_buffer_width;
	}


	// sy = clipped * src_inc;
	tsy = clipped * src_inc;
	// tptr = &tptr[sx * wid]; /*note add of sx here instead of in loop*/
	tptr = &tptr[sx * bptr->w]; /*note add of sx here instead of in loop*/

	// tsy = sy;
	
	if (light != DARKEST)
	{
		if (light == 0)
		{
#if !defined(_WINDOWS)
			if (fAIMoving)
			{
				do {
					pix = tptr[tsy>>15];	/* objects are part of lighting model */
					// Test for transparency
					if (pix)
					{
						*(sptr+1)=*sptr = (UBYTE)pix;		
					}
			
					tsy += src_inc;
					sptr += sptr_inc;
				} while(sptr < sptr_end);
			}
			else
#endif
			{
				do {
					pix = tptr[tsy>>15];	/* objects are part of lighting model */
					// Test for transparency
					if (pix)
					{
						*sptr = (UBYTE)pix;		
					}
			
					tsy += src_inc;
					sptr += sptr_inc;
				} while(sptr < sptr_end);
			}
		}
		else
		{
			
#if !defined(_WINDOWS)
			if (fAIMoving)
			{
				do {
					pix = shade[tptr[tsy>>15]];	/* objects are part of lighting model */
					// Test for transparency
					if (pix)
					{
						*(sptr+1)=*sptr = (UBYTE)pix;		
					}
			
					tsy += src_inc;
					sptr += sptr_inc;
				} while(sptr < sptr_end);
			}
			else
#endif
			{
				do {
					pix = shade[tptr[tsy>>15]];	/* objects are part of lighting model */
					// Test for transparency
					if (pix)
					{
						*sptr = (UBYTE)pix;		
					}
			
					tsy += src_inc;
					sptr += sptr_inc;
				} while(sptr < sptr_end);
			}
		}
	}
	else
	{
		
#if !defined(_WINDOWS)
		if (fAIMoving)
		{
			do {
				*(sptr+1)=*sptr = BLACK;
				sptr += sptr_inc;
			} while (sptr < sptr_end);
		}
		else
#endif
		{
			do {
				*sptr = BLACK;
				sptr += sptr_inc;
			} while (sptr < sptr_end);
		}
	}
}

/* =======================================================================
	
	======================================================================= */
void draw_thing_spans (void)
{
	LONG i;

	/* print text above things */
//	if (fDisplayTitles)
//	{
//		init_gfont(FONT_SANS_6PT);
//		for (i=0; i<(LONG)tot_things; ++i)
//		{
//			if (mythings[i].fDrawn && mythings[i].title != NULL)
//			{
//				print_text_centered(mythings[i].xDrawn, mythings[i].yDrawn-10, mythings[i].title, 31);
//			}
//		}
//	}
#ifdef INET
	DisplayPlayerNames();
#endif
	tot_thing_spans--;

	/* draw all spans */
	for (i=tot_thing_spans; i>-1; i--)
	{
		// [d6-26-96 JPC] Added "light."
		// Light is calculated separately for transparent walls (RENDER: draw_wall)
		// and for things (THINGS: draw_thing).
		// Note that we DO NOT want to check range here, because for color
		// remapping and infravision, "light" can be greater than "DARKEST" (31).
		// "SetLight" is a macro that sets the global pointer "shade" to
		// the appropriate shade_table.
		THING_SPAN *pthing_spans = &thing_spans[i];
		
		SetLight (pthing_spans->light);
		scale_obj(pthing_spans->t,
			pthing_spans->sx,
			pthing_spans->dx,pthing_spans->dy,pthing_spans->dye,
			pthing_spans->clip,
			pthing_spans->src_inc,
			pthing_spans->light
			);
	}
}

/* =======================================================================
	
	======================================================================= */
static void draw_thing (LONG t)
{
// JPC note: Like its caller (draw_things), draw_thing does not actually
// draw anything.  Instead, it fills in elements of an enormous (30,000
// elements) array called "thing_spans."  These elements are later used
// by "draw_thing_spans" to draw the things.
// ---------------------------------------------------------------------------

	POINT			a, b;
	LONG			hei, wid;
	LONG			clip;
	LONG			bottomClip;
	ULONG			src_x;
	ULONG			xsrc_inc;
	LONG			new_w;
	LONG			topY,bottomY;
	LONG			scale_factor;
	LONG			oldHeiScaled;				// [d9-05-96 JPC]
	LONG			floorHeight;				// [d9-05-96 JPC]
	POINT			torig_a;
	//SHORT			iBitm = mythings[t].iBitm;
	BITMPTR			pBitm;
	ULONG			type = mythings[t].type;	// support WARNINGS
	LONG			light;							// [d6-27-96 JPC]
	LONG 			myDeltaY;						// [d7-17-96 JPC]
	SHORT			iAnim;
	LONG			mirror_thing = FALSE;

	CHECK_THING_INDEX(t,"draw_thing");
	
	// GWP HACK to get to get trees out of the way of the camera for fights.
	if (gDontDrawInfo.SetFlag == TRUE &&
		mythings[t].iAnim == (SHORT)fERROR &&		// Not A Flc
		IsAViewBlocker(t) &&
		mythings[t].dist < mythings[gDontDrawInfo.ThingIndex].dist
		)
		return;

#ifdef _JPC
// Debugging breakpoints.
		if (mythings[t].type == giType1 && mythings[t].x == -534)
			++gDebug;					// put breakpoint here
		if (mythings[t].type == giType2)
			++gDebug;					// put breakpoint here
#endif

	/* get some details about the graphic */
	//if (iBitm == (SHORT)fERROR)
	//{
	//	printf("WARNING - iBitm is fERROR in draw_thing (thing type %ld)\n",type);
	//	return;
	//}

	if ( mythings[t].inVisible == TRUE )
		return;

	// CODE FOR ANIMATIONS ONLY
	iAnim = mythings[t].iAnim;
	if (iAnim != fERROR)
	{
		LONG			approach_angle;
		ANIMPTR	   		pAnim = 0;
		SHORT			iHead = fERROR;
		I_DATA_BLK		*piData = 0;
		LONG			rotation;
		SHORT			iSeq;
		POINT 			c;
		POINT 			e;
		
		/* Find angle between camera location and thing */
		approach_angle = (
			RelativeAngle(
				mythings[t].angle,
				AngleFromPoint(	
								mythings[t].x,
								mythings[t].y,
								CAMERA_INT_VAL(camera.x),
								CAMERA_INT_VAL(camera.y),
								RESOLUTION_4
								)
				)
			);

		/* select best angle to represent the approach_angle from avail angles */
		if (approach_angle < 128L)
		{
			approach_angle = 256L - approach_angle;
			mirror_thing = TRUE;
		}
		if (approach_angle <=144L)			//!!!!!!!!!!!!!!!!! needs work
			{rotation = 4; mirror_thing = FALSE;}
		else if (approach_angle <=176L)
			rotation = 3;
		else if (approach_angle <=208L)
			rotation = 2;
		else if (approach_angle <=240L)
			rotation = 1;
		else
			{rotation = 0; mirror_thing = FALSE;}

		pAnim = (ANIMPTR)BLKPTR(iAnim);

		/* check for invalid request and fall back to STAND at 0 deg. */
		iSeq = (mythings[t].iSequence * NUMROTATIONS) + rotation;
		
		if (pAnim->type == TYPE_FLIC)
		{
			FLICHEADPTR		pHead;
			
			if (pAnim->hiData <= 0)
			{
				return; // Sorry no animations for this type.
			}
			// Test whether this sequence has been paged out.
			piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
			iHead = piData->iData[iSeq];
			if (iHead <= 0 ||
			    IsHandleFlushed(iHead))
			{
				iAnim = load_FLC_sequence(iAnim, mythings[t].type, mythings[t].iSequence, rotation);
				
				// Refresh the handles.
				pAnim = (ANIMPTR)BLKPTR(iAnim);
				piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
				iHead = piData->iData[iSeq];
				
				// Were we successful?
				if (iHead <= 0 ||
					IsHandleFlushed(iHead))
				{
#ifndef QUIETDEGRADE				
					printf("WARNING - degrading %s seq:%c to stand\n", GetThingName(type), cAction[mythings[t].iSequence]);
#endif				
					// Couldn't load it.  Degrade to STAND
					
					MarkOldAnimPurgable(iAnim, mythings[t].iSequence);
					mythings[t].iSequence = ANIMATION0SEQ;
					iSeq = rotation + (ANIMATION0SEQ * NUMROTATIONS);
					iHead = piData->iData[iSeq];
					
					// Test whether the stand sequence has ever been loaded.
					if (iHead <= 0 ||
						IsHandleFlushed(iHead))
					{
						// Get the stand sequence.
						load_FLC_sequence(iAnim, mythings[t].type, mythings[t].iSequence, rotation);
						
						pAnim = (ANIMPTR)BLKPTR(iAnim);
						piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
						iHead = piData->iData[iSeq];
						if (iHead <= 0)
						{
							printf("WARNING - Unable to get stand animation %s seq: %c !!\n",
							GetThingName(t), cAction[mythings[t].iSequence]);
							return;	// couldn't load the stand.
						}
					}
					SetAnimSequence(iAnim, iSeq);
					
					// Refresh the handles.
					pAnim = (ANIMPTR)BLKPTR(iAnim);
					piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
					iHead = piData->iData[iSeq];
					
					// We could have changed this Handle in SetAnimSequence so refresh it.
					mythings[t].iBitm = pAnim->iBuff;
					if (mythings[t].iBitm != fERROR)
					{
						BITMPTR pBitm = (BITMPTR) BLKPTR(mythings[t].iBitm);
						// Note: We use the bitmap width because the art is rotated 90Deg.
						mythings[t].heiScaled = (pBitm->w*UNITARY_SCALE)/pBitm->scale;
						mythings[t].widScaled = (pBitm->h*UNITARY_SCALE)/pBitm->scale;
					}
				}
				else
				{
					SetAnimSequence(iAnim, iSeq);
					// Refresh the handles.
					pAnim = (ANIMPTR)BLKPTR(iAnim);
					// We could have changed this Handle in SetAnimSequence so refresh it.
					mythings[t].iBitm = pAnim->iBuff;
					piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
					iHead = piData->iData[iSeq];		// get the memory handle for this resource
				}
			}
			
			
			/* get width and height of selected sequence */
			pHead = (FLICHEADPTR)BLKPTR(iHead);
			// We've been paged out.
			if (pHead == 0)
				return;
				
			hei = pHead->width;
			wid = pHead->height;
	
			/* only decompress flics that can be seen */
			
			if(mythings[t].Frozen == TRUE)
			   // && mythings[t].fDrawnLastFrame == TRUE)		// -GWP- Optimization for Battlefields, causes us to be one frame off...
			{	
				mythings[t].bControl = 0;
				mythings[t].iBitm = NextAnimFrame(iAnim, (SHORT)mythings[t].iSequence, rotation, &(mythings[t].bControl), pAnim->frame);
				if ( mythings[t].iBitm == fERROR)
				{
					printf("WARNING - NextAnimFrame %ld (seq:%ld rot:%ld) returned fERROR\n",type,mythings[t].iSequence,rotation);
					return;
				}
				else
				{
					BITMPTR pBitm = (BITMPTR) BLKPTR(mythings[t].iBitm);
					mythings[t].heiScaled = (pBitm->h*UNITARY_SCALE)/pBitm->scale;
					mythings[t].widScaled = (pBitm->w*UNITARY_SCALE)/pBitm->scale;
				}
					
				if (mythings[t].SkipFrame == TRUE)
				{
					mythings[t].iBitm = NextAnimFrame(iAnim, (SHORT)mythings[t].iSequence, rotation, &(mythings[t].bControl), pAnim->frame);
					if ( mythings[t].iBitm == fERROR)
					{
						printf("WARNING - NextAnimFrame %ld (seq:%ld rot:%ld) returned fERROR\n",type,mythings[t].iSequence,rotation);
						return;
					}
					else
					{
						BITMPTR pBitm = (BITMPTR) BLKPTR(mythings[t].iBitm);
						mythings[t].heiScaled = (pBitm->h*UNITARY_SCALE)/pBitm->scale;
						mythings[t].widScaled = (pBitm->w*UNITARY_SCALE)/pBitm->scale;
					}
					
				}
			}
			else
			{
				//if (mythings[t].fDrawnLastFrame == TRUE)
				{
					mythings[t].iBitm = NextAnimFrame(iAnim, (SHORT)mythings[t].iSequence, rotation, &(mythings[t].bControl), -1);
					if ( mythings[t].iBitm == fERROR)
					{
						printf("WARNING - NextAnimFrame %ld (seq:%ld rot:%ld) returned fERROR\n",type,mythings[t].iSequence,rotation);
						return;
					}
					else
					{
						BITMPTR pBitm = (BITMPTR) BLKPTR(mythings[t].iBitm);
						mythings[t].heiScaled = (pBitm->h*UNITARY_SCALE)/pBitm->scale;
						mythings[t].widScaled = (pBitm->w*UNITARY_SCALE)/pBitm->scale;
					}
					
					if (mythings[t].SkipFrame == TRUE)
					{
						mythings[t].iBitm = NextAnimFrame(iAnim, (SHORT)mythings[t].iSequence, rotation, &(mythings[t].bControl), -1);
						if ( mythings[t].iBitm == fERROR)
						{
							printf("WARNING - NextAnimFrame %ld (seq:%ld rot:%ld) returned fERROR\n",type,mythings[t].iSequence,rotation);
							return;
						}
						else
						{
							BITMPTR pBitm = (BITMPTR) BLKPTR(mythings[t].iBitm);
							mythings[t].heiScaled = (pBitm->h*UNITARY_SCALE)/pBitm->scale;
							mythings[t].widScaled = (pBitm->w*UNITARY_SCALE)/pBitm->scale;
						}
					}
				}
			}
		}
		else
		if (pAnim->type == TYPE_PCX)			/* check for PCX rotation frames */
		{
			if (pAnim->hiData > 0)
			{
				return; // Sorry no animations for this type.
			}
			
			// Test whether this sequence has been paged out.
			piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
			iHead = piData->iData[iSeq];
			if (iHead <= 0 ||
			    IsHandleFlushed(iHead))
			{
				iAnim = load_PCX_sequence(iAnim, mythings[t].type, mythings[t].iSequence, rotation);
				
				// Refresh the handles.
				pAnim = (ANIMPTR)BLKPTR(iAnim);
				piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
				iHead = piData->iData[iSeq];
				
				// Were we successful?
				if (iHead <= 0 ||
				    IsHandleFlushed(iHead))
				{
#ifndef QUIETDEGRADE				
					printf("WARNING - degrading %s seq:%c to stand\n", GetThingName(type), cAction[mythings[t].iSequence]);
#endif				
					// Couldn't load it.  Degrade to STAND
					
					MarkOldAnimPurgable(iAnim, mythings[t].iSequence);
					mythings[t].iSequence = ANIMATION0SEQ;
					iSeq = ANIMATION0SEQ + rotation;
					
					// Test whether the stand sequence has ever been loaded.
					iHead = piData->iData[iSeq];
					
					if (iHead <= 0 ||
					    IsHandleFlushed(iHead))
					{
						load_PCX_sequence(iAnim, mythings[t].type, mythings[t].iSequence, rotation);
						// Refresh the handles.
						pAnim = (ANIMPTR)BLKPTR(iAnim);
						piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
						iHead = piData->iData[iSeq];
					
						if (iHead <= 0)
						{
							// unable to load stand.
							printf("WARNING - Unable to get stand animation %s seq: %c !!\n",
							GetThingName(t), cAction[mythings[t].iSequence]);
							return;
						}
					}
					
					SetPCXSequence(iAnim, iSeq);
					
					// Refresh the handles.
					pAnim = (ANIMPTR)BLKPTR(iAnim);
					piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
					iHead = piData->iData[iSeq];
					
					mythings[t].iBitm = pAnim->iBuff;
					if (mythings[t].iBitm != fERROR)
					{
						BITMPTR pBitm = (BITMPTR) BLKPTR(mythings[t].iBitm);
						// Note: We use the bitmap width because the art is rotated 90Deg.
						mythings[t].heiScaled = (pBitm->h*UNITARY_SCALE)/pBitm->scale;
						mythings[t].widScaled = (pBitm->w*UNITARY_SCALE)/pBitm->scale;
					}
				}
				else
				{
					SetPCXSequence(iAnim, iSeq);
					
					// Refresh the handles.
					pAnim = (ANIMPTR)BLKPTR(iAnim);
					piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
					iHead = piData->iData[iSeq];
				}
			}
			
			/* get width and height of selected sequence */
			pBitm = (BITMPTR)BLKPTR(iHead);
			// We've been paged out.
			if (pBitm == 0)
				return;
			
			hei = pBitm->h;		// these are the original (as drawn) width and height of the graphic
			wid = pBitm->w;
			
			mythings[t].iBitm = NextPCXFrame(iAnim, mythings[t].iSequence, rotation);
			if ( mythings[t].iBitm == fERROR)
			{
				printf("WARNING - NextPCXFrame %ld (seq:%ld rot:%ld) returned fERROR\n",type,mythings[t].iSequence,rotation);
				return;
			}
			
		}
		
		pBitm = (BITMPTR) BLKPTR(mythings[t].iBitm);
		scale_factor = pBitm->scale;
		/* create a line to represent the thing, length is width of thing */
		c.x = 0;
		c.y = mythings[t].widScaled = (wid*UNITARY_SCALE*mythings[t].scale_adjust)/(scale_factor*UNIT_SCALE_ADJUST);
		rot8(&c);									// insure line is perp to viewer
		
		/* create a line to represent the center point */
	 	e.x = 0;
	 	e.y = ((pBitm->x_ctr_pt)*UNITARY_SCALE*mythings[t].scale_adjust)/(scale_factor*UNIT_SCALE_ADJUST);
	 	rot8(&e);
	 	// adjust center point
	 	if (!mirror_thing)
	 	{
	 		a.x=mythings[t].x-e.y;
	 		a.y=mythings[t].y-e.x;
	 		b.x=mythings[t].x+c.y-e.y;
	 		b.y=mythings[t].y+c.x-e.x;
	 	}
	 	else
	 	{
	 		a.x=mythings[t].x-c.y+e.y;
	 		a.y=mythings[t].y-c.x+e.x;
	 		b.x=mythings[t].x+e.y;
	 		b.y=mythings[t].y+e.x;
	 	}
		
 	}
	// CODE FOR STATIC OBJECTS ONLY (not flc & no rotations)
	else
	{
		if (mythings[t].iBitm > 0)
		{
			POINT c;
			
			pBitm = (BITMPTR) BLKPTR(mythings[t].iBitm);
			// We've been paged out.
			if (pBitm == 0)	// We've been paged out.
			{
				Load_Bitm(&mythings[t].iBitm, mythings[t].OriginalType);
				pBitm = (BITMPTR) BLKPTR(mythings[t].iBitm);
			}
			
			if (pBitm == 0)
				return;
			
			hei = pBitm->w;		// these are the original (as drawn) width and height of the graphic
			wid = pBitm->h;
			scale_factor = pBitm->scale;
		 	
			/* create a line to represent the thing, length is width of thing */
			c.x = 0;
			c.y = mythings[t].widScaled = (wid*UNITARY_SCALE*mythings[t].scale_adjust)/(scale_factor*UNIT_SCALE_ADJUST);
			rot8(&c);									// insure line is perp to viewer
			
			// create a line, half on either side
			a.x=mythings[t].x-(c.y>>1);			// a is upper-left
			a.y=mythings[t].y-(c.x>>1);
			b.x=mythings[t].x+(c.y>>1);			// b is lower-right
			b.y=mythings[t].y+(c.x>>1);
		
		}
		else
		{
			// Not set.
			return;
		}
	}

#ifdef _JPC
// Debugging breakpoints.
		if (mythings[t].type == giType1 && mythings[t].x == -534)
			++gDebug;					// put breakpoint here
#endif


	xlate(&a);									// translate to camera
	xlate(&b);
	rot8(&a);									// rotate around viewpoint
	rot8(&b);
	torig_a=a;									// used in calc of source inc
	if (clip_line(&a,&b)==0)				// clip the line, only A can be clipped
		return;

	/* new_w is the width after clipping */
	new_w = dist(a.x, a.y, b.x, b.y);

	/* src_x is how much was clipped off */
	src_x = dist(a.x, a.y, torig_a.x, torig_a.y) << 7;

	/* put upper-left above the floor */
	oldHeiScaled = mythings[t].heiScaled; // [d9-05-96 JPC]
	mythings[t].heiScaled = (hei*UNITARY_SCALE*mythings[t].scale_adjust)/(scale_factor*UNIT_SCALE_ADJUST);
	proj(&a,camera.z - (mythings[t].heiScaled + mythings[t].z));

	// Calculate the clipping necessary by projecting twice, once
	// for the full image and once for the clipped image.
	// The result, "bottomClip," is used below to calculate "bottomY."
	floorHeight = sectors[mythings[t].sect].fh;
	if (!HangsFromCeiling(t) &&
		  mythings[t].z <= floorHeight &&
	     (sectors[mythings[t].sect].special == SSP_WATER ||
		  sectors[mythings[t].sect].special == SSP_ACID_FLOOR ||
		  sectors[mythings[t].sect].special == SSP_LAVA ||
		  sectors[mythings[t].sect].special == SSP_DEEP_WATER))
	{
		POINT	 d;
		SHORT	 thingZ;						// [d8-29-96 JPC]
		
		if (oldHeiScaled != mythings[t].heiScaled)
		{
			// [d9-05-96 JPC]
			// Readjust z based on new height to fix the positioning
			// problem that occurs when something suddenly gets shorter.
			// Example: when something dies in battle.  We have to do this
			// because the thing's z coordinate has already been calculated
			// at this point, using the old, erroneous heiScaled.
			//
			// Check whether we're already adjusting for water.  Test is
			// whether the thing's z coordinate is below the floor.  This
			// should take care of flying and climbing without requiring
			// specific tests for those things.
			if (mythings[t].z < floorHeight)
			{
				if (sectors[mythings[t].sect].special == SSP_WATER ||
					sectors[mythings[t].sect].special == SSP_ACID_FLOOR ||
					sectors[mythings[t].sect].special == SSP_LAVA)
					mythings[t].z = floorHeight - mythings[t].heiScaled / 4;
            else                       // must be SSP_DEEP_WATER
					mythings[t].z = floorHeight - (mythings[t].heiScaled * 3) / 4;
			}
		}

		// Point d is the same as the UNPROJECTED point b.
		d.x = b.x;
		d.y = b.y;

		// if (sectors[mythings[t].sect].special == SSP_WATER ||
		// 	sectors[mythings[t].sect].special == SSP_ACID_FLOOR ||
		// 	sectors[mythings[t].sect].special == SSP_LAVA)
		// 	thingZ = mythings[t].z + mythings[t].heiScaled / 4;
		// else										// must be SSP_DEEP_WATER
		// 	thingZ = mythings[t].z + (mythings[t].heiScaled * 3) / 4;
		thingZ = floorHeight;
		proj(&d,camera.z - thingZ);
		proj(&b,camera.z - mythings[t].z);		// lower-right
		bottomClip = b.y - d.y;						// total clip from standing in water
	}
	else
	{
		proj(&b,camera.z - mythings[t].z);		// lower-right
		bottomClip = 0;								// no clip due to standing in water
	}

	if (!extent_visible(a.x,b.x))
		return;
	if (a.x > b.x)
		SWAPINT(a.x,b.x);

	/* xsrc_inc is number of source columns per screen column */
	xsrc_inc = (new_w << 15) / (b.x - a.x);
	if (xsrc_inc <= 0)
		return;

	/* xsrc_inc and src_x refer to source bitmap so remove the scale factor */
	xsrc_inc = (xsrc_inc * scale_factor*UNIT_SCALE_ADJUST) / (UNITARY_SCALE*mythings[t].scale_adjust);
	src_x = (src_x * scale_factor*UNIT_SCALE_ADJUST) / (UNITARY_SCALE*mythings[t].scale_adjust) ;
	src_x <<= 8;	// Shift the rest to avoid an overflow in the above line. GWP.


	myDeltaY = b.y - a.y;
	// If there's no change in Y, then clip_obj in the draw loop below will return
	// FALSE and nothing will be added to thing_spans[], so we won't need to calculate light.
	if (myDeltaY != 0)
	{
	    BOOL        	fHeatSource;               // [d7-16-96 JPC]
		LONG 			mySrcInc;						// [d7-17-96 JPC]
	
		// y did change, so we do need to calculate light.
		mySrcInc = (hei << 15) / myDeltaY;
		if (mythings[t].ssect != -1)
		{
			LONG			iSector;							// [d6-27-96 JPC]
			
			// [d6-27-96 JPC] Retrieve the light for this subsector.
			// Ignore NSEW lighting, which does not seem relevant for things.
			// The following does not work because sometimes you need
			// the BACK sector:
			iSector = mythings[t].sect;
			light = sector_to_light (iSector);
			if (light)
			{
				// If not brightest then shade with distance.
				// Shift right by 15 instead of 16 because 15 is what we
				// used to construct src_inc, above.
				// Use src_inc here by analogy with the way the
				// parameter src_inc is used in scale_col_ttop
				// to darken walls.
				light += mySrcInc >> 15;
				if (light > DARKEST)
					light = DARKEST;
			}
		}
		else
		{
			light = 0;						// need to pick something
		}
		// At this point the basic lighting value is set and is in the
		// range 0 (brightest) to 31 (DARKEST).
		// We can do color shuffling by adding 32 (for infrared) +
		// 32 * color shuffle index to the .light field.
		// (Do this only for certain things.)
		// If some global vision is set, we can adjust the lighting
		// for that, too.
		// [d7-15-96 JPC] Added color remap as an "else" clause to the
		// infravision clause.  To avoid a ludicrous number of tables,
		// all things in infravision use the same palette.
		// If that isn't what we want, then build tables in CUE.C for
		// infravision versions of all the color remap tables, and make
		// the color remap clause an independent "if" clause.
		// [d7-16-96 JPC] Replace various light adjustments with a
		// call to RenderChangeLight.  Still do color remapping
		// separately, because this function is the ONLY one in which
		// we do color remapping.
		fHeatSource = ThingIsHeatSource (mythings[t].type);
		light = RenderChangeLightHeat (light, fHeatSource);
#ifdef _DEBUG
		if (!gfTestGrayLight)
#endif
		// Color remap--do NOT do color remapping if infravision is on.
		// [d12-13-96 JPC] Infravision does not apply if light is
		// <= INFRAVISION_THRESHHOLD, so we can remap in that case, too.
		if (light != DARKEST)
		{
			if (!gfInfraVision || light <= INFRAVISION_THRESHHOLD)
				light += REMAP_TABLE_OFFSET + 32 * mythings[t].ColorRemap;
		}
	}
	
#ifdef _JPC
// Debugging breakpoints.
		if (mythings[t].type == giType1 && mythings[t].x == -534)
			++gDebug;					// put breakpoint here
#endif

	/* loop for each column in the thing */
	while (a.x < b.x)
	{
		LONG			delta_y;
		
		topY=a.y;
      // [d9-03-96 JPC] Added "bottomClip" factor to the following;
		// bottomClip is 0 in most cases, but is some fraction of the
		// thing's SCALED and PROJECTED height if the thing is in water.
		bottomY=b.y - bottomClip;			
		delta_y = b.y - a.y;		// isn't delta_y always the same within the loop?

		if (clip_obj(a.x,&topY,&bottomY,&clip))
		{
			if (!mythings[t].fDrawn)
			{
				mythings[t].fDrawn = TRUE;			/* part of thing WAS drawn */
//				mythings[t].xDrawn = ((b.x-a.x)/2)+a.x;
//				mythings[t].yDrawn = a.y;
				mythings[t].fMapSpotted = TRUE;
			}
			// Note that if delta_y is 0, clip_obj will return FALSE and
			// we won't get here.  So no division by 0 will occur.
			thing_spans[tot_thing_spans].src_inc = (hei << 15) / delta_y;
#ifdef _JPC
			// GWP ASSERT (thing_spans[tot_thing_spans].src_inc == mySrcInc);
#endif

			// check for click
			if(mouse_button)
			{
				// did we click on this span
				if(cursor_x == a.x && cursor_y < bottomY && cursor_y > topY)
				{
					// set the object clicked on
					objClicked = t;
					typeClicked = iOBJECT;
					butClicked = mouse_button;

#ifdef _WINDOWS
					// play a sound effect
//					beep();
#endif

					// turn off click so we don't process any more
					mouse_button = 0;
				}
			}

			/* handle mirroring - probably need to do more work here !!!!! */
			if (mirror_thing)
				thing_spans[tot_thing_spans].sx=wid-(src_x>>15);
			else
				thing_spans[tot_thing_spans].sx=(src_x>>15);

#if 01
// [d11-07-96 JPC] Somehow .sx is becoming negative in Winterroot adventure.
			if (thing_spans[tot_thing_spans].sx < 0)
				thing_spans[tot_thing_spans].sx = 0; // put breakpoint here
#endif
			
			thing_spans[tot_thing_spans].t=mythings[t].iBitm;

			thing_spans[tot_thing_spans].light = light;
			thing_spans[tot_thing_spans].dx=a.x;
			thing_spans[tot_thing_spans].dy=topY;
			thing_spans[tot_thing_spans].dye=bottomY;
			thing_spans[tot_thing_spans++].clip=clip;

		}

		src_x += xsrc_inc;
		++a.x;
	if(tot_thing_spans>MaxThingSpans)
#if defined(_DEBUG)
		fatal_error("Exceeded MaxThingSpans");
#else
		return;
#endif
	}
}


/* =======================================================================
	Run through list of things while sorting each pair by distance from camera
	======================================================================= */
void draw_things (LONG ss)
{
// Called from render_view.
// ss = subsector.
// JPC note: this function doesn't actually draw anything.
// Instead, it records data about things that will be used when we draw
// them in "draw_thing_spans," which is also called from render_view.
// Need to add lighting information...
// ---------------------------------------------------------------------------

	LONG	i, ni, d, nd;
	LONG	oi = -1;

	if ((i = ssector_things[ss]) == -1)				// get first thing
	{
#if EXTRATHINGS
		// [d11-05-96 JPC]
		// Nothing is "officially" in this subsector, so check the
		// overflow from neighboring subsectors.
		if ((i = subsectorExtraThings[ss]) != -1)
		{
			draw_thing (i);
		}
#endif
		return;
	}

	// calculate the distance to the thing
	d = (aprox_dist(CAMERA_INT_VAL(camera.x),
					CAMERA_INT_VAL(camera.y), mythings[i].x, mythings[i].y)) << PLAYER_FIXEDPT;
#if WRC_CAMERA_TEST	
	if (!VideoCurrentlyRedirected)
	{
#endif
		mythings[i].dist = d;
#if WRC_CAMERA_TEST	
	}
#endif

	for ( ni = mythings[i].next_thing;
		  ni != -1 && mythings[ni].valid == TRUE;
		  ni = mythings[i].next_thing)
	{
		// this section could take a bit of time so we will
		// call the timer critical routine
		// GWP Too often to be called here. run_timers();
		
		nd = (aprox_dist(CAMERA_INT_VAL(camera.x),
						 CAMERA_INT_VAL(camera.y),
						 mythings[ni].x,
						 mythings[ni].y)) << PLAYER_FIXEDPT;
		
#if WRC_CAMERA_TEST	
		if (!VideoCurrentlyRedirected)
		{
#endif
			mythings[ni].dist = nd;
#if WRC_CAMERA_TEST	
		}
#endif

		if (d > nd)
		{
			draw_thing(ni);
			// Resort the sub sector list in the mythings array. (Bubble sort.)
			mythings[i].next_thing = mythings[ni].next_thing;
			if (oi == -1)
				ssector_things[ss] = ni;
			else
				mythings[oi].next_thing = ni;
			mythings[ni].next_thing = i;
			oi = ni;
		}
		else
		{
			draw_thing(i);
			d = nd;
			oi = i;
			i = ni;
		}
	}
	
	if (mythings[i].valid == TRUE)
		draw_thing(i);

#if EXTRATHINGS
	// [d11-05-96 JPC] Implement the kludge to keep dead things from clipping
	// inappropriately.
	if ((i = subsectorExtraThings[ss]) != -1)
	{
		draw_thing(i);
	}
#endif
}

/* ========================================================================
   Function    - AllocateAnim
   Description - Allocate some space for a pAnim
   Returns     - handle to the memory.
   ======================================================================== */
SHORT AllocateAnim(SHORT Type, LONG ttype)
{
	ANIMPTR	pAnim = 0;
	SHORT	iAnim = fERROR;
	char	cpBuffer[40];
	
	/* allocate memory for animation header */
	iAnim = NewBlock(sizeof(ANIMHDR));
	if ( iAnim == fERROR)
		return fERROR;
	
	SetClass2(iAnim);	// GWP There is a leak somewhere and this will guarentee we free this block.
	SetQuickLock(iAnim);
	pAnim = (ANIMPTR)BLKPTR(iAnim);
	pAnim->type			= Type;
	pAnim->sequence		= 9998;			/* out of range condition */
	pAnim->frame		= 99;			/* out of range condition */
	pAnim->totalFrames	= 0;			/* out of range condition */
	pAnim->rotation		= 0;
	pAnim->iBuff		= fERROR;
	sprintf(cpBuffer, "%s.IDA", GetThingName(ttype));
	pAnim->hiData = GetResourceStd(cpBuffer, FALSE);
	
	// Debug information about the iAnim.
#if defined (MEMORY_CHK)
	sprintf(cpBuffer, "%s.IAM", GetThingName(ttype));
	SetBlockName(cpBuffer,iAnim);
#endif
	
	ClrLock(iAnim);
	return iAnim;
}

/* =======================================================================
	OpenAnim - Get a .FLC type resource and create an animation header
	======================================================================= */
SHORT OpenAnim (SHORT iAnim, SHORT iSeq, CSTRPTR szFileName, SHORT Type)
{
	ANIMPTR			pAnim = 0;
	SHORT			iHead = fERROR;
	I_DATA_BLK		*piData = 0;

	/* get the anim data into memory */
	if (Type == TYPE_FLIC)
	{
		pAnim = (ANIMPTR)BLKPTR(iAnim);
		if (pAnim->hiData > 0)
		{
			piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
			if (piData->iData[iSeq] <= 0 ||
			    BLKPTR(piData->iData[iSeq]) == 0)
			{
				iHead = GetResourceStd(szFileName,FALSE);
				// refresh the pointer
				pAnim = (ANIMPTR)BLKPTR(iAnim);
				piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
				
				piData->iData[iSeq] = iHead;
			}
		}
		if (iHead < 0)
			return fERROR;
	}
	else
	if (Type == TYPE_PCX)
	{
		pAnim = (ANIMPTR)BLKPTR(iAnim);
		if (pAnim->hiData > 0)
		{
			piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
			if (piData->iData[iSeq] <= 0 ||
				BLKPTR(piData->iData[iSeq]) == 0)
			{
				iHead = GetResourceRot(szFileName);
				
				// refresh the pointer
				pAnim = (ANIMPTR)BLKPTR(iAnim);
				piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
				
				piData->iData[iSeq] = iHead;
			}
		}
	}

	// Only need to set this if we needed to go get the resource.
	if (iHead > 0)
	{
		SetClass2(iHead);				// allow all things to be purged en masse
		SetMultiUser(iHead);			// set it to be used by multiple users.
#if fUSE_RES_FILES
		SetPurge(iHead);
#endif

		if (BLKPTR(iHead) == NULL )
		{
#if defined(_DEBUG)
			fatal_error("ERROR - iHead references a NULL block in OpenAnim\n");
#endif
		}
	}

	return iAnim;
}

/* =======================================================================
	RemoveAnim - Dispose of everything related to an anim
	======================================================================= */
void RemoveAnim (SHORT iAnim)
{
	ANIMPTR	pAnim;
//	UWORD		i;

	if (iAnim <= fERROR)
		return;

	pAnim = (ANIMPTR)BLKPTR(iAnim);

	/* handled by PurgeClass(CLASS2) in purge_zone */
//	for (i=0; i<NUMSEQUENCES; ++i)
//		if (pAnim->iData[i] != fERROR)
//			SetPurge(pAnim->iData[i]);

	if (pAnim->iBuff != fERROR)
		DisposBlock(pAnim->iBuff);

	pAnim->iBuff = fERROR;	// just in case.
	pAnim->hiData = fERROR;	// The animations will be free'd as class 2 objects.
	DisposBlock(iAnim);

// printf("Disposed of iAnim and iBuff in RemoveAnim\n");

}
	

/* ========================================================================
   Function    - MarkOldAnimPurgable
   Description - Purge out the previous animation.
   Returns     -
   ======================================================================== */
   	
void MarkOldAnimPurgable(SHORT iAnim, ULONG seq)
{
	if (iAnim != fERROR && seq <= NUMACTIONS)
	{
		ANIMPTR		pAnim = (ANIMPTR)BLKPTR(iAnim);
		
		// mark old sequence as purgable if we're changing sequences.
		if (pAnim->hiData > 0)
		{
			ULONG		rot;
			ULONG		iSeq = seq * NUMROTATIONS;
			I_DATA_BLK	*piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
			
			for (rot=0; rot<NUMROTATIONS; ++rot)
			{
				SHORT iSeq_and_rotation = iSeq + rot;
				// test if ever loaded.
				if (piData->iData[iSeq_and_rotation] > 0)
				{
					// set the resource to be purgable
					SetPurge(piData->iData[iSeq_and_rotation]);
					// Clear the CLASS2 flag so we don't do this again.
					SetClassPerm(piData->iData[iSeq_and_rotation]);
				}
			}
		}
	}
}

/* ========================================================================
   Function    - MarkAllAnimPurgable
   Description - Purge all the animations. (For change'ing resolutions.)
   Returns     -
   ======================================================================== */

static void MarkAllAnimPurgable(SHORT iAnim)
{
	ULONG i;
	
	for (i = ANIMATION0SEQ; i < MAX_ANIMATIONSEQ; ++i)
	{
		MarkOldAnimPurgable(iAnim, i);
	}
}

/* ========================================================================
   Function    - SetPCXSequence
   Description - Initialize the pAnim for PCX's
   Returns     - fERROR | fNOERR
   ======================================================================== */
static SHORT SetPCXSequence(SHORT iAnim, SHORT iSeq)
{
	ANIMPTR		pAnim;
	BITMPTR		pBitm = 0;
	SHORT		iHead;
	I_DATA_BLK	*piData = 0;

	if (iAnim == fERROR)
		return fERROR;
		
	pAnim = (ANIMPTR)BLKPTR(iAnim);
	/* set to new sequence, check for invalid request */
	if (pAnim->hiData < 0)
		return fERROR;
		
	piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
	if (piData->iData[iSeq] < 0)
		return fERROR;
	
	iHead = piData->iData[iSeq];
	if (iHead < 0)
		return fERROR;

	pBitm = (BITMPTR) BLKPTR(iHead);
	
	pAnim->sequence		= iSeq;
	pAnim->frame		= 0;
	pAnim->width		= pBitm->h;
	pAnim->height		= pBitm->w;
	pAnim->offData		= 0;
	pAnim->totalFrames	= 0;
	pAnim->iBuff		= iHead;	/* make iBuff point to correct PCX frame */
	
	return fNOERR;
}

/* =======================================================================
	SetAnimSequence - Set the sequence
					  WARNING! OpenBitm moves memory!
	======================================================================= */
SHORT SetAnimSequence (SHORT iAnim, SHORT iSeq)
{
	SHORT		iHead;
	ANIMPTR		pAnim;
	FLICHEADPTR	pHead;
	BITMPTR		pBuff;
	SHORT		scale = UNITARY_SCALE;		// 1:1 scale
	I_DATA_BLK	*piData = 0;
	SHORT		iBitm = fERROR;

	if (iAnim == fERROR)
		return fERROR;
		
	pAnim = (ANIMPTR)BLKPTR(iAnim);
	/* set to new sequence, check for invalid request */
	if (pAnim->hiData < 0)
		return fERROR;
		
	piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
	if (piData->iData[iSeq] < 0)
		return fERROR;
	
	iHead = piData->iData[iSeq];
	if (iHead <= 0)
		return fERROR;

	pHead = (FLICHEADPTR)BLKPTR(iHead);
	if (!IsPointerGood(pHead))
		return fERROR;
	
	ClrPurge(iHead);		// Make sure this animation stays in memory.
	// Need to reset the class 2 flag.
	SetClass2(iHead);		// allow all things to be purged en masse
	
	pAnim->sequence		= iSeq;
	pAnim->frame		= 0;
	pAnim->width		= pHead->width;
	pAnim->height		= pHead->height;
//	pAnim->speed		= (USHORT)pHead->speed;
	pAnim->offData		= pHead->oframe1;
	pAnim->totalFrames	= pHead->frames;
	if (pAnim->offData > 100000)
	{
#if defined (_DEBUG)
		fatal_error("ERROR - pAnim->offData set to invalid value (%ld) in SetAnimSequence\n",pAnim->offData);
#endif
		return fERROR;
	}

	// First time,
	if (pAnim->iBuff == fERROR)
	{
		// Allocate a new Bitm.
		iBitm = OpenBitm(pAnim->width, pAnim->height);
		
		pAnim = (ANIMPTR)BLKPTR(iAnim); // refresh the pointer.
		pHead = (FLICHEADPTR)BLKPTR(iHead);	// refresh the pointer.
		pAnim->iBuff = iBitm;
		if (iBitm == fERROR)
		{
#if defined (_DEBUG)
			fatal_error("ERROR - could not allocate buffer in SetAnimSequence\n");
#endif
			return fERROR;
		}
		SetClass2(iBitm);
		decode_frame(pAnim, pHead);		/* decode the 1st frame */
		detect_scale(pAnim->iBuff, pAnim->width, pAnim->height);	/* scan for scale factor */
	}
	else
	{
		/* if iBuff exists and is not the same size, dispos before making a new one */
		pBuff = (BITMPTR) BLKPTR(pAnim->iBuff);
		if (pBuff->w != pAnim->width || pBuff->h != pAnim->height)
		{
			((BITMPTR)BLKPTR(pAnim->iBuff))->scale = scale;
			scale = pBuff->scale;	/* save current scale */
			
			// Toss the old Bitm back into the heap.
			DisposBlock(pAnim->iBuff);
			
			// Allocate a new Bitm.
			iBitm = OpenBitm(pAnim->width, pAnim->height);
			
			pAnim = (ANIMPTR)BLKPTR(iAnim); // refresh the pointer.
			pHead = (FLICHEADPTR)BLKPTR(iHead);	// refresh the pointer.
			
			pAnim->iBuff = iBitm;
			if (iBitm == fERROR)
			{
#if defined (_DEBUG)
				fatal_error("ERROR - could not allocate buffer in SetAnimSequence\n");
#endif
				return fERROR;
			}
			SetClass2(iBitm);
			
		}
		
		decode_frame(pAnim, pHead);		/* decode the 1st frame */
		detect_scale(pAnim->iBuff, pAnim->width, pAnim->height);	/* scan for scale factor */
	}

	return fNOERR;
}

/* =======================================================================
	NextPCXFrame - Set the buffer for a PCX.
	======================================================================= */
SHORT NextPCXFrame (SHORT iAnim, SHORT sequence, SHORT rotation )
{
	ANIMPTR			pAnim;
	SHORT			iHead = fERROR;
	SHORT			seq_and_rot = (sequence * NUMROTATIONS) + rotation;
	
	if (iAnim == fERROR)						/* double check */
		return fERROR;

	pAnim = (ANIMPTR)BLKPTR(iAnim);

	/* not looping and not changing seq or rot and already did one pass */
	if (pAnim->sequence == seq_and_rot)
		goto exit;

	/* check for seq and rot available in this Anim */
	if (pAnim->hiData > 0)
	{
		I_DATA_BLK		*piData;
		
		piData = (I_DATA_BLK *) BLKPTR(pAnim->hiData);
		iHead = piData->iData[seq_and_rot];
	}
	else
		return fERROR;
	
	if (iHead < 0)
		return fERROR;

	pAnim = (ANIMPTR)BLKPTR(iAnim);
	
	if (iHead == fERROR || IsHandleFlushed(iHead))
	{
#if defined (_DEBUG)
		fatal_error("ERROR - PCX type frame was bogus in NextPCXFrame\n");
#else
		return fERROR;
#endif
	}
	pAnim->iBuff = iHead;				/* make iBuff point to correct PCX frame */

exit:
	return pAnim->iBuff;
}


/* =======================================================================
	NextAnimFrame - Advance to next frame of flic
	======================================================================= */
SHORT NextAnimFrame (SHORT iAnim, USHORT sequence, USHORT rotation, UBYTE * pCtrl, SHORT frame)
{
	ANIMPTR			pAnim = 0;
	FLICHEADPTR		pHead = 0;
	SHORT			iHead = fERROR;
	SHORT			seq_and_rot = (sequence * NUMROTATIONS) + rotation;
	SHORT			old_sequence = 0;
	SHORT			old_rotation = 0;
	SHORT			desired_frame = 0;
	SHORT			continue_flag = ((*pCtrl & START_FLAG) == START_FLAG);
	SHORT			rv = fERROR;
	SHORT      		totalFrames=-1;
	SHORT      		tmpFrame = 0;
	SHORT			i = 0;
	SHORT			ihiData = fERROR;
	I_DATA_BLK		*piData = 0;
	BOOL			iDataLocked = FALSE;
	BOOL			iHeadLocked = FALSE;
	BOOL			iAnimLocked = FALSE;
	
	if (iAnim == fERROR)						/* double check */
		return fERROR;

	iAnimLocked = IsLocked(iAnim);
	
	if (!iAnimLocked)
		SetQuickLock(iAnim);	/* quick SetLock without memory move */
		
	pAnim = (ANIMPTR)BLKPTR(iAnim);

	/* not looping and not changing seq or rot and already did one pass */
	if (*pCtrl == 0 && pAnim->sequence == seq_and_rot)
		goto exit;

	/* check for seq and rot available in this Anim */
	if (pAnim->hiData > 0)
	{
		ihiData = pAnim->hiData;
		iDataLocked = IsLocked(ihiData);
		if (!iDataLocked)
			SetQuickLock(ihiData);
		
		piData = (I_DATA_BLK *) BLKPTR(ihiData);
		iHead = piData->iData[seq_and_rot];
		if (iHead < 0)
			goto error_exit;
			
		iHeadLocked = IsLocked(iHead);
		if (!iHeadLocked)
			SetQuickLock(iHead);
	}
	else
		goto error_exit;
	

	//pAnim = (ANIMPTR)BLKPTR(iAnim);
	
	/* prepare for sequence check and frame decoding */
	pHead = (FLICHEADPTR)BLKPTR(iHead);
	// Paged out or non-existent art.
	if (!IsPointerGood(pHead))
		goto error_exit;
	
	old_sequence = pAnim->sequence / NUMROTATIONS;
	old_rotation = pAnim->sequence % NUMROTATIONS;

	if(old_rotation != -1 && *pCtrl == 0)
	{
		totalFrames = pHead->frames;
	}

	desired_frame = pAnim->frame;

	/* check for sequence & rotation change */
	if (pAnim->sequence != seq_and_rot)
	{
		if (RestrictAniSeq(old_sequence) != RestrictAniSeq(sequence))
		{
			MarkOldAnimPurgable(iAnim, old_sequence);
		}
		if (SetAnimSequence(iAnim, seq_and_rot) == fERROR)
			goto error_exit;
			//return fERROR;
		
		// Refresh the pointers after call to SetAnimSequence.
		//pAnim = (ANIMPTR)BLKPTR(iAnim);
		//piData = (I_DATA_BLK *) BLKPTR(ihiData);
		
		iHead = piData->iData[seq_and_rot];
		if (iHead <= 0)
			goto error_exit;
		
		pHead = (FLICHEADPTR)BLKPTR(iHead);
		if (!IsPointerGood(pHead))
			goto error_exit;
		
		totalFrames = pHead->frames;
		
		if(frame != -1 && *pCtrl == 0)
		{
			// GWP What is this next if statement for????
			if(totalFrames != pHead->frames && totalFrames != -1)
				tmpFrame = frame * pHead->frames / totalFrames;
			else
				tmpFrame = frame;

			for(i=pAnim->frame; i<tmpFrame; ++i)
			{
				++pAnim->frame;
				decode_frame(pAnim, pHead);
			}

		}
		//GEH Not Used
		//if (sequence != old_sequence)		/* not just rotation change */
		//{
		//	desired_frame = 0;
		//	continue_flag = START_FLAG;
		//}
	}
	else
		++desired_frame;

	/* check for end of sequence and looping */
	if (desired_frame >= pHead->frames)			/* after last frame */
	{
		if (pAnim->frame == 0)					/* already at zero */
			desired_frame = 0;
		else											/* otherwise, test for loop */
		{
			if (!(*pCtrl & REWIND_FLAG))		/* don't rewind to 1st frame if flag not set */
				goto exit;
			pAnim->offData = pHead->oframe1;	/* reset to first frame */
			if (pAnim->offData > 100000)
			{
#if defined (_DEBUG)
				fatal_error("ERROR - pAnim->offData set to invalid value (%ld) in NextAnimFrame\n",pAnim->offData);
#else
				// return fERROR;
				goto error_exit;
#endif
			}
			desired_frame = pAnim->frame = 0;
			decode_frame(pAnim, pHead);		/* decode the 1st frame */
			
			detect_scale(pAnim->iBuff, pAnim->width, pAnim->height);	/* scan for scale factor */

			/* added check for loop once being reset */
			if (*pCtrl & (LOOP_FLAG|START_FLAG))
				desired_frame = 1;				/* loop to 2nd frame */
		}
	}

	/* advance to desired frame */
	while (pAnim->frame < desired_frame)
	{
		if ( pAnim->offData >= pHead->size)
		{
#if defined(_DEBUG)
			printf("WARNING! Things.c is missing %ld frames.\n",
			        (LONG)(pAnim->totalFrames - pAnim->frame));
#endif
			pHead->frames = pAnim->frame;
			pAnim->totalFrames = pAnim->frame;
			break;
		}
		
		++pAnim->frame;
		decode_frame(pAnim, pHead);	/* advance to next frame */
	}

	/* set control frame number */
	*pCtrl = ((*pCtrl) & (LOOP_FLAG|REWIND_FLAG)) + continue_flag + pAnim->frame;

exit:
	rv = pAnim->iBuff;		// Current frame now in iBuff
	if (rv == fERROR)
		printf("WARNING - iBuff is fERROR in NextAnimFrame\n");
#if defined(_DEBUG)
	if (IsHandleFlushed(rv))
	{
		fatal_error("ERROR - iBuff indexes NULL memory in NextAnimFrame\n");
	}
#endif

error_exit:
	if (!iHeadLocked && iHead > 0)
		ClrLock(iHead);
	if (!iDataLocked && ihiData > 0)
		ClrLock(pAnim->hiData);
	if (!iAnimLocked)
		ClrLock(iAnim);

	
	return rv;
}


/* =======================================================================
	decode_frame - Decode a frame that is in memory already into screen.
	Here we just loop through each chunk calling appropriate
	chunk decoder.
	======================================================================= */
ERRCODE decode_frame (ANIMPTR pAnim, 	FLICHEADPTR pHead)
{
	ULONG				i;
	FRAMEHEADPTR	pFramehd;
	CHUNKHEADPTR	pChunk;
	PTR				pData;

#if defined(_DEBUG)
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	for (i=7; i>0; i--)
	{
		pAnim->frameHistory[i] = pAnim->frameHistory[i-1];
		pAnim->offDataHistory[i] = pAnim->offDataHistory[i-1];
		pAnim->SequenceHistory[i] = pAnim->SequenceHistory[i-1];
		pAnim->RotationHistory[i] = pAnim->RotationHistory[i-1];
	}
	pAnim->frameHistory[0] = pAnim->frame;
	pAnim->offDataHistory[0] = pAnim->offData;
	pAnim->SequenceHistory[0] = pAnim->sequence;
	pAnim->RotationHistory[0] = pAnim->rotation;
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#endif

	/* decode the next frame of the animation */
	pFramehd = (FRAMEHEADPTR)((ULONG)pHead + pAnim->offData);

	if (pFramehd->size > 100000)
	{
#if defined (_DEBUG)
		fatal_error("ERROR - pFramehd->size invalid (%d) in decode_frame\n",pFramehd->size);
#else
		return fERROR;
#endif
	}

	pAnim->offData += pFramehd->size;

	if (pFramehd->size - (LONG)sizeof(FRAMEHEAD) > 0L)
	{
		pData = (PTR)(pFramehd+1);

		for (i=0; i<pFramehd->chunks; ++i)
		{
			pChunk = (CHUNKHEADPTR)pData;
			pData += pChunk->size;
			switch (pChunk->type)
			{
//				case COLOR_256:
//					decode_color((PTR)(pChunk+1), pAnim, COLOR_256);
//					break;
  				case DELTA_FLC:
  					decode_delta_flc((PTR)(pChunk+1), pAnim);
  					break;
//				case COLOR_64:
//					decode_color((PTR)(pChunk+1), pAnim, COLOR_64);
//					break;
//				case DELTA_FLI:
//					decode_delta_fli((PTR)(pChunk+1), pAnim);
//					break;
//				case BLACK_FRM:
//					FillRect(pAnim->iBuff, 0, 0, pAnim->width, pAnim->height, coTRANSP);
//					break;
  				case BYTE_RUN:
  					decode_byte_run((PTR)(pChunk+1), pAnim);
  					break;
  				case LITERAL:
  					decode_literal((PTR)(pChunk+1), pAnim);
  					break;
  				default:
//				printf("OTHER CHUNK: %d\n",pChunk->type);
  					break;
			}
		}

	}
	return fNOERR;
}

/* =======================================================================
	decode_byte_run - Byte-run-length decompression.
	======================================================================= */
void decode_byte_run (PTR pData, ANIMPTR pAnim)
{
	LONG		width = pAnim->width;
	LONG		height = pAnim->height;
	SBYTE		psize;
	LONG		w;
	PTR		pDest;

	pDest = ((PTR)BLKPTR(pAnim->iBuff)) + sizeof(BITMHDR);

	do
	{
		++pData;						/* skip over obsolete opcount byte */

		psize = w = 0;
		do
		{
			psize = (SBYTE)*pData++;
			if (psize > 0)
			{
				if (psize == 1)		// Optimize for a single byte of data.
				{
					*pDest = *pData;
					++pData;
					++pDest;
					++w;
				}
				else
				{
					memset(pDest, *pData++, psize);
					pDest += psize;
					w += psize;
				}
			}
			else if (psize < 0)
			{
				psize = -psize;
				if (psize == 1)		// Optimize for a single byte of data.
				{
					*pDest = *pData;
					++pDest;
					++pData;
					++w;
				}
				else
				{
					if (psize == 2)		// Optimize for two bytes of data.
					{
						*((short *)(pDest)) = *((short *)(pData));
					}
					else
					{
						memcpy(pDest, pData, psize);
					}
					pDest += psize;
					pData += psize;
					w += psize;
				}
			}
		} while (w < width);

	} while (--height);

}

/* =======================================================================
	decode_literal - Decode a LITERAL chunk.  Just copy data to buffer
	======================================================================= */
void decode_literal (PTR pData, ANIMPTR pAnim)
{
	LONG		width = pAnim->width;
	LONG		height = pAnim->height;
	PTR		pDest = ((PTR)BLKPTR(pAnim->iBuff)) + sizeof(BITMHDR);

	do {
		memcpy(pDest, pData, width);
		pData += width;
		pDest += width;
	} while (--height);
}

/* =======================================================================
	decode_delta_flc - Flc-style delta decompression.
	The data is word oriented though        a lot of the control info (how far to skip,
	how many words to copy) are byte oriented still to save space.
	======================================================================= */
void decode_delta_flc (PTR pData, ANIMPTR pAnim)
{
	LONG		width = pAnim->width;
	SHORT		line_count;
	SHORT		opcount;
	union 	{USHORT *uw; SHORT *w; UBYTE *ub; SBYTE *b;} uDataPtr;
	union 	{USHORT *uw; UBYTE *ub;} uDestTmp;
	PTR		pDest;
	USHORT	pix;
	SBYTE		psize;

// [d1-08-97 JPC] If we're desperate to know where the memory overwrite
// occurs, try checking for the overwrite as we decode--but this will
// be terribly slow.
// #if defined (_DEBUG)
// 	PTR		pEndOfBlock;
// #endif

	pDest = ((PTR)BLKPTR(pAnim->iBuff)) + sizeof(BITMHDR);

// #if defined (_DEBUG)
// 	pEndOfBlock = pDest + GetDataBlkSize (pAnim->iBuff);
// #endif

	uDataPtr.ub = pData;			/* set union of pointers to pData */
	line_count = *uDataPtr.w++;	/* union allows us to treat the ptr as various types */

	do
	{
		/* packets which make up a line */
		if ((opcount = *uDataPtr.w++) >= 0)		/* check for packet count */
		{
			if (opcount)							/* check for zero packets */
			{
				uDestTmp.ub = pDest;					/* recover x coord */
				do
				{
					uDestTmp.ub += *uDataPtr.ub++;	/* column skip */
					/* copy a run */
					if ((psize = *uDataPtr.b++) > 0)	/* copy run of data */
					{
// #if defined (_DEBUG)
// 					if (uDestTmp.uw + psize >= pEndOfBlock)
// 						fatal_error ("Overwrote end of block in decode_delta_flc\n");
// #endif
						memcpy(uDestTmp.uw, uDataPtr.uw, psize<<1);
						uDestTmp.uw += psize;
						uDataPtr.uw += psize;
					}

					/* fill a run */
					else if (psize < 0)				/* rep a word several times */
					{
						psize = -psize;
						pix = *uDataPtr.uw++;
						do {
// #if defined (_DEBUG)
// 							if (uDestTmp.uw >= pEndOfBlock)
// 								fatal_error ("Overwrote end of block in decode_delta_flc\n");
// #endif
							*uDestTmp.uw++ = pix;
						} while (--psize);
					}


				} while(--opcount);
			}
			pDest += width;
			--line_count;
		}

		/* skip lines */
		else if( ((USHORT)opcount) & 0x4000)
		{
			opcount = -opcount;
			pDest += (width * opcount);
		}

		/* last dot in line */
		else
		{
			/* put a dot at the end of the line. pixel is low byte */
			uDestTmp.ub = pDest + width - 1;        /* last pixel */
			*uDestTmp.ub = (UBYTE)opcount;
		}

	} while (line_count);

#if defined (_DEBUG)
	// [d1-08-97 JPC] Simpler check for memory overwrite--do a superfluous
	// call to BLKPTR and let the debugging code check it for us.
	pDest = ((PTR)BLKPTR(pAnim->iBuff)) + sizeof(BITMHDR);
#endif
}

// see below
void sort_things_from_camera(long n)
{
}

#if 0

GEH SAYS - NONE OF THE FOLLOWING WORK, IN FACT THEY ARE VERY BAD!!!!!!!!!!	

/* ======================================================================= */
/* =======================================================================
	
	======================================================================= */
void sort_things_from_camera(long n)
{
	if(n&0x8000)		/*if sector*/
		{
		sort_things_in_ssector(n&0xfff); //sort objects in starting sector
		return;
		}

	if(point_relation(n,CAMERA_INT_VAL(camera.x),CAMERA_INT_VAL(camera.y))==FRONT)
	{
		sort_things_from_camera(nodes[n].f);
		sort_things_from_camera(nodes[n].r);
	}
	else
	{
		sort_things_from_camera(nodes[n].r);
		sort_things_from_camera(nodes[n].f);
	}
}

/* =======================================================================
	======================================================================= */
void sort_things_in_ssector (LONG ss)
 {
	LONG		i, j, c;					//temp-storage for current thing
	//LONG		tt = 0;
	LONG		sort_things[MAX_THINGS];
	// GWP MYTHING	    localthings[MAX_THINGS];

//	LONG		dist[300];

	if ((i = ssector_things[ss]) == -1)
		return;											// get first thing

	// update the the distance's to the thing
	while (i != -1)
		{
		mythings[i].dist= (aprox_dist(CAMERA_INT_VAL(camera.x), CAMERA_INT_VAL(camera.y), mythings[i].x, mythings[i].y)<<8)+tt;
		i = mythings[i].next_thing;				//next thing in list
		}

	// sort the things by distance
	i = ssector_things[ss]; //first thing into i
	while (i != -1)
		{
		j = ssector_things[ss]; //j is to compare against
		c = 0;
		while (j != -1)						//comparing dist of i to each other thing
			{
			if (mythings[i].dist > mythings[j].dist)
				++c;				    //if i is closer then j, increment index
			j = mythings[j].next_thing;	//move j up a thing
			}
		sort_things[c] = i;
		i = mythings[i].next_thing;		//next thing into i
		}

	// GWP for (i = 0; i < tt; ++i)
	// GWP 	localthings[i] = mythings[sort_things[i]];
	// GWP
	// GWP for (j = ssector_things[ss], i = 0; i < tt; ++i)
	// GWP 	{
	// GWP 	c = mythings[j].next_thing;
	// GWP 	mythings[j] = localthings[i];
	// GWP 	mythings[j].next_thing = c;
	// GWP 	j = c;
	// GWP 	}
}
void sort_things_in_sector (LONG ss)
{
	LONG		i, j, c;					//temp-storage for current thing
	LONG		tt = 0;
	LONG		sort_things[300];
	MYTHING	localthings[300];

	LONG		dist[300];

	if ((i = ssector_things[ss]) == -1)
		return;											// get first thing

	// calculate the distance to the thing
	while (i != -1)
		{
		mythings[i].dist= (aprox_dist(CAMERA_INT_VAL(camera.x), CAMERA_INT_VAL(camera.y), mythings[i].x, mythings[i].y)<<8)+tt;
		mythings[i].dist += tt;
		++tt;
		i = mythings[i].next_thing;				//next thing in list
		}

	// sort the things by distance
	i = ssector_things[ss]; //first thing into i
	while (i != -1)
		{
		j = ssector_things[ss]; //j is to compare against
		c = 0;
		while (j != -1)						//comparing dist of i to each other thing
			{
			if (mythings[i].dist < mythings[j].dist)
				++c;								//if i is closer then j, increment index
			j = mythings[j].next_thing;	//move j up a thing
			}
		sort_things[c] = i;
		i = mythings[i].next_thing;		//next thing into i
		}

	for (i = 0; i < tt; ++i)
		localthings[i] = mythings[sort_things[i]];

	for (j = ssector_things[ss], i = 0; i < tt; ++i)
		{
		c = mythings[j].next_thing;
		mythings[j] = localthings[i];
		mythings[j].next_thing = c;
		j = c;
		}
}
#endif

/* ======================================================================= */
BOOL ThingIsHeatSource (LONG thingIndex)
{
// [d7-17-96 JPC]
// Return TRUE if the thing is alive--for infravision.  Living things
// are brighter than nonliving things.
// ---------------------------------------------------------------------------

// Requires editing array in GAMETYPE.CPP to add HEAT flag to selected items.

// GEH fixed thingindex reference
   return ((
	((TTYPE *)(TTypePtr + (G_TTypeSize * thingIndex)))->attrib
	& HEAT) ? TRUE : FALSE);
}

/* ======================================================================= */
BOOL ThingIsCold (LONG thingIndex)
{
// Return TRUE if the thing is artificially cold.
// ---------------------------------------------------------------------------

   return ((
	((TTYPE *)(TTypePtr + (G_TTypeSize * thingIndex)))->attrib
	& COLD) ? TRUE : FALSE);
}

/* ======================================================================= */
BOOL ThingIsMagic (LONG thingIndex)
{
// Return TRUE if the thing is MAGIC
// ---------------------------------------------------------------------------

   return ((
	((TTYPE *)(TTypePtr + (G_TTypeSize * thingIndex)))->attrib
	& MAGIC) ? TRUE : FALSE);
}

/* ======================================================================= */
BOOL ThingIsPickupable (LONG thingIndex)
{
// Return TRUE if the player can pick the thing up.
// ---------------------------------------------------------------------------

   return ((
	((TTYPE *)(TTypePtr + (G_TTypeSize * thingIndex)))->attrib
	& MAPCPICK) ? TRUE : FALSE);
}


/* ======================================================================= */
BOOL ThingIsEvil (LONG thingIndex)
{
// Return TRUE if the thing is EVIL.
// ---------------------------------------------------------------------------

   return ((
	((TTYPE *)(TTypePtr + (G_TTypeSize * thingIndex)))->attrib
	& EVIL) ? TRUE : FALSE);
}

/* ===========================================================================

	=========================================================================== */
void change_scale_adjust(LONG t, LONG newScale)
{
	if(newScale == 0)
	{
#if defined(_DEBUG)
		fatal_error("ERROR - THINGS.C: passed in scale_adjust equals to zero.\n");
#else
		return;
#endif
	}

	mythings[t].scale_adjust = newScale;
	// Now rescale the height and width to approximately the new size
	// It will be recalculated on the next render.
	mythings[t].heiScaled = (mythings[t].heiScaled*mythings[t].scale_adjust)/UNIT_SCALE_ADJUST;
	mythings[t].widScaled = (mythings[t].widScaled*mythings[t].scale_adjust)/UNIT_SCALE_ADJUST;
}


/* ===========================================================================
	ChangeThingZ--when a sector floor height changes, call this function to
	adjust the z coordinate of all things in the sector.  Don't change things
	that hang from the ceiling and do special handling of flying things.
	=========================================================================== */
void ChangeThingZ (LONG iSector)
{
	LONG			iThing;
	LONG			actualFloorHeight;
	LONG			floorHeight;
	BOOL			fWaterSector;
	BOOL			fDeepWaterSector;

	actualFloorHeight = sectors[iSector].fh;

	fWaterSector = sectors[iSector].special == SSP_WATER ||
		sectors[iSector].special == SSP_ACID_FLOOR ||
		sectors[iSector].special == SSP_LAVA;

	fDeepWaterSector = sectors[iSector].special == SSP_DEEP_WATER;

	for (iThing = 0; iThing < MaxThingLoaded; ++iThing)
	{
		if (mythings[iThing].sect == iSector)
		{
			if (!HangsFromCeiling(iThing))
			{
				// [d8-21-96 JPC] Trick the things into thinking the floor height
				// is lower than it really is if the sector is a water sector.
				if (fWaterSector)
				{
					floorHeight = actualFloorHeight - mythings[iThing].heiScaled / 4;
				}
				else if (fDeepWaterSector)
				{
					floorHeight = actualFloorHeight - (mythings[iThing].heiScaled * 3) / 4;
				}
				else
				{
					floorHeight = actualFloorHeight;
				}
				if (IsFlyingThing(iThing))
				{
					if (floorHeight > mythings[iThing].z)
						mythings[iThing].z = floorHeight;
				}
				else								// not a flying thing
				{
					mythings[iThing].z = floorHeight;
				}
			}
		}
	}

// [d3-14-97 JPC] Add the following to keep the player from looking up
// and down on lifts.
// [d3-26-97 JPC] Added check for whether player is actually in the sector. (?!)
	if (player.currentSector == iSector && !player.Flying)
	{
		if (fWaterSector)
		{
			floorHeight = actualFloorHeight - player.h / 4;
		}
		else if (fDeepWaterSector)
		{
			floorHeight = actualFloorHeight - (player.h * 3) / 4;
		}
		else
		{
			floorHeight = actualFloorHeight;
		}
		if (player.z - floorHeight < player.h / 2)
			player.z = floorHeight;
	}
}

/* ===========================================================================
	Function		- SetFrame
	Description - Set sequence and frame to mythings
	Return      -
	=========================================================================== */
void SetFrame(LONG t, ULONG iSeq, SHORT iFrame)
{
	mythings[t].iSequence = (UBYTE)iSeq;
	mythings[t].bControl  = 0;
	mythings[t].Frozen = TRUE;
	mythings[t].SkipFrame = FALSE;
	if (mythings[t].iAnim != fERROR)
	{
		ANIMPTR	pAnim = (ANIMPTR)BLKPTR(mythings[t].iAnim);
		pAnim->frame = iFrame;
		pAnim->sequence = -1;
	}
}

/* ===========================================================================
	Function	- SetDontDrawCloser
	Description - Set the draw objects closer flag
	Return      -
	=========================================================================== */
void SetDontDrawCloser(LONG ThingIndex)
{
	gDontDrawInfo.SetFlag = TRUE;
	gDontDrawInfo.ThingIndex = ThingIndex;
}

/* ===========================================================================
	Function	- ClearDontDrawCloser
	Description - Clear the don't draw close objects flag.
	Return      -
	=========================================================================== */
void ClearDontDrawCloser()
{
	gDontDrawInfo.SetFlag = FALSE;
	gDontDrawInfo.ThingIndex = fERROR;
}

/* ===========================================================================
	Function	- SetQuestThing.
	Description - Set the quest thing for this wad.
	Return      -
	=========================================================================== */
void SetQuestThing(LONG ThingType)
{
	gMagicItems.QuestItem = ThingType;
}

/* ===========================================================================
	Function	- SetMagicThings
	Description - Initialize the magic things array.
	Return      -
	=========================================================================== */
void InitMagicThings(SHORT * pMagicThingTypes,
					 LONG NumberOfThingTypes,
					 LONG MagicThingType,
					 LONG QuestThingType)
{
	gMagicItems.pMagicItems = pMagicThingTypes;
	gMagicItems.NumberOfMagicItems = NumberOfThingTypes;
	gMagicItems.MagicThingType = MagicThingType;
	gMagicItems.QuestThingType = QuestThingType;
	gMagicItems.QuestItemMythingsIndex = 0;   //better than garbage?

}


LONG QuestItemMythingsIndex(void)
{
	return gMagicItems.QuestItemMythingsIndex;
}

LONG GetQuestThing(void)
{
	return gMagicItems.QuestItem;
}




#if EXTRATHINGS
/* ===========================================================================
	Function		- AddThingToAdjacentSubsectors
	Description - Call to add a thing to adjacent subsectors.
					  Example of use: call for a dead thing to keep it
					  from being clipped by a small subsector.
	Note			- This is a slow routine, so only call it when needed
					  (such as initial placement or positioning a dead thing).
	Return      - nothing
	=========================================================================== */
void AddThingToAdjacentSubsectors (LONG ThingIndex)
{
	LONG 			thing_ss;
	LONG 			other_ss;
	LONG			x;
	LONG			y;
	LONG 			sect;
	LONG 			FloorHeight;
	LONG 			CeilingHeight;
	LONG 			Special;
	LONG 			Tag;
	LONG			xAdd;
	LONG			yAdd;
	SHORT 		w;
	BITMPTR		pBitm;
	ANIMPTR		pAnim;

	thing_ss = mythings[ThingIndex].ssect;
	x = mythings[ThingIndex].x;
	y = mythings[ThingIndex].y;

	if (mythings[ThingIndex].iBitm != fERROR)
	{
		pBitm = (BITMPTR) BLKPTR(mythings[ThingIndex].iBitm);
		w = max (pBitm->w, pBitm->h);
	}
	else if (mythings[ThingIndex].iAnim != fERROR)
	{
		pAnim = (ANIMPTR)BLKPTR(mythings[ThingIndex].iAnim);
		w = pAnim->height;
	}
	else
	{
		w = mythings[ThingIndex].widScaled;
	}

	// Check various points the full width of the image in all directions
	// from its x,y.  (Note that a dead thing, which only has one or two frames,
	// will appear to rotate around its feet as you move around it.  So we
	// need to check all subsectors that might be in the circle described
	// by the rotation of the dead body.)
	for (yAdd = -w; yAdd <= w; yAdd += w/8)
	{
		for (xAdd = -w; xAdd <= w; xAdd += w/8)
		{
			if (xAdd != 0 && yAdd != 0)
			{
				other_ss = find_ssector (x + xAdd, y + yAdd);
				if (other_ss != thing_ss &&
					subsectorExtraThings[other_ss] != ThingIndex
					)
				{
					ssector_to_sector_info(x + xAdd, y + yAdd, other_ss, &sect, &FloorHeight,
						&CeilingHeight, &Special, &Tag);
					if (FloorHeight - sectors[mythings[ThingIndex].sect].fh < HEIGHT_DIFF_OK)
					{
						if (subsectorExtraThings[other_ss] == -1)
							subsectorExtraThings[other_ss] = ThingIndex;
					}
				}
			}
		}
	}
}
#else
void AddThingToAdjacentSubsectors (LONG ThingIndex)
{
	// do nothing; leave in for a while so others can compile.
}
#endif


/* ========================================================================
   Function    - LoadIData
   Description - Create a unique I_DATA_BLK. I_DATA_BLK's hold an array
                 of all the Res handles to all the sequences and rotations
                 for this thingtype of art. They are shared by all instances
                 of this art type to reduce the total memory used.
   Returns     - Handle to the memory
   ======================================================================== */
SHORT LoadIData (
	CSTRPTR szFileName,
	BOOL  fSetPal,
	BOOL  fLockRes,
	BOOL  fRotated,
	LONG  iResFileSlot)
{
	SHORT hiData = fERROR;
	
	hiData = NewBlock(sizeof(I_DATA_BLK));
	if (hiData > 0)
	{
		I_DATA_BLK *piData;
		// Just do a lock in place so this can float later.
		if (fLockRes)
			SetQuickLock(hiData);
		
		piData = (I_DATA_BLK *) BLKPTR(hiData);
		
		memset (piData->iData, -1, (NUMSEQUENCES * sizeof (SHORT)));
#if defined (_DEBUG)
		strncpy(piData->cpType, szFileName, sizeof(piData->cpType));
#endif
		SetClass2(hiData);
		SetModifyResource(hiData);
	}
	
	return hiData;
}

/* ========================================================================
   Function    - DisposeIDataBlk
   Description - Call Dispose Resource for all the resource handles.
   				 Note: These are .flc's.
   Returns     -
   ======================================================================== */
SHORT DisposeIDataBlk (SHORT iResBlk, SHORT iMemBlk)
{
	I_DATA_BLK *piData = (I_DATA_BLK *) BLKPTR(iMemBlk);
	LONG i;
	
	for (i = 0; i < NUMSEQUENCES; ++i)
	{
		if (piData->iData[i] > 0)
		{
			SetPurge(piData->iData[i]);
			// Clear the CLASS2 flag so we don't do this again.
			SetClassPerm(piData->iData[i]);
			piData->iData[i] = fERROR;
		}
	}
	
	// Now clear our resource
	return DisposRes(iResBlk, iMemBlk);
}



/* ========================================================================
   Function    - Load_Bitm
   Description - Load a bit map graphic
   Returns     -
   ======================================================================== */

static void Load_Bitm(SHORT *piBitm, ULONG ttype)
{
	char	pn[128];
	char  	n[40];
	BITMPTR pBitm;
	
	*piBitm = fERROR;
	
	/* get base filename from registered TType array */
	/* GEH also check for the ultra low memory config */
	/* and swap out some of the bitmaps for another */
	sprintf(n,"%s", GetThingName(ttype));
	
	sprintf(pn,"%sPCX\\%s.pcx",THING_PATH,n);

//		printf("Loading %s as a bitm\n",pn);
	*piBitm = GetResourceRot(pn);
	if (*piBitm == fERROR)
	{
#if defined (_DEBUG)
		fatal_error("ERROR - Unable to load thing file: %s\n",n);
#else
		return;
#endif
	}
	
	SetClass2(*piBitm);
	SetMultiUser(*piBitm);
#if fUSE_RES_FILES
	SetPurge(*piBitm);
#endif
	
	pBitm = (BITMPTR) BLKPTR(*piBitm);
	
	// If missing the center point dot, put it in the middle.
	if (pBitm->x_ctr_pt == 0)
	{
		pBitm->x_ctr_pt = pBitm->w/2;
	}
}

/* ========================================================================
   Function    - RestrictAniSeq
   Description - In low memory cases, restrict the number of sequences
   Returns     - new sequence
   ======================================================================== */
ULONG RestrictAniSeq(ULONG seq)
{
	ULONG GetSeq = seq;
	
	if( seq == ANIMATION4SEQ )		// ATTACK2
	{								// goes to
		GetSeq = ANIMATION3SEQ;		// attack 1
	}
	else
	if( fBattleCombat && seq == ANIMATION2SEQ )	// DEFEND
	{ 								// goes to
		GetSeq = ANIMATION3SEQ;		// attack 1
	}
	else
	if (seq == ANIMATION1SEQ)		// Walk
	{
		GetSeq = ANIMATION11SEQ;	// March
	}

	return GetSeq;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

