/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: THINGS.H
   Author: Alan Clark
   ======================================================================== */
#if !defined(_THINGS_H)
#define _THINGS_H	1

/* maximum number of mythings in a given wad */
#define MAX_THINGS		800

#define INCR_LOAD     01
#define NUMACTIONS    13
#define NUMROTATIONS   5
#define NUMSEQUENCES   (NUMACTIONS * NUMROTATIONS)
#define THING_PATH    "things\\"

#define MAX_HIRES_THING_SPANS	15000
#define MAX_LOWRES_THING_SPANS	10000

//TTYPE ATTRIBUTE BITS

#define NOATTS				0x00000000
#define NOBUMP				0x00000001
#define HANGS				0x00000002
//see MAP.H for bits (3-7)	0x00000004
//see MAP.H for bits (3-7)	0x00000008
//see MAP.H for bits (3-7)	0x00000010
//see MAP.H for bits (3-7)	0x00000020
//see MAP.H for bits (3-7)	0x00000040
#define FLIES				0x00000080
#define NOSTAND			  	0x00000100		// Like a potted plant.
#define PARTBUMP			0x00000200		// Like a tree you can go thru the branches.
#define HEAT				0x00000400
#define COLD				0x00000800
#define MAGIC				0x00001000
#define EVIL				0x00002000
#define TREASURE			0x00004000
#define VIEWBLKR			0x00008000

#define PCX_ART				0x20000000
#define FLC_ART				0x40000000
#define BITMAP_ART			0x80000000
#define ART_TYPE_MASK		(PCX_ART | FLC_ART | BITMAP_ART)

// Note the next part of the word's bits are set in gametype.hxx




/* animation sequence defines */
typedef enum {
	ANIMATION0SEQ = 0,
	ANIMATION1SEQ,
	ANIMATION2SEQ,
	ANIMATION3SEQ,
	ANIMATION4SEQ,
	ANIMATION5SEQ,
	ANIMATION6SEQ,
	ANIMATION7SEQ,
	ANIMATION8SEQ,
	ANIMATION9SEQ,
	ANIMATION10SEQ,
	ANIMATION11SEQ,
	ANIMATION12SEQ,
	
	// Enter new sequences above this.
	MAX_ANIMATIONSEQ
} ANIM_SEQUENCE_ENUM;
#define INITSEQ	STANDSEQ


/* ------------------------------------------------------------------------
   Typedefs and Structures
   ------------------------------------------------------------------------ */
typedef struct{
	LONG			x;
	LONG			y;
	SHORT			z;
	LONG			dist;					// 24.8 value
//	LONG			xDrawn;
//	LONG			yDrawn;
	unsigned int	OriginalType: 10;
	unsigned int	valid		: 1;
	unsigned int	fDrawn		: 1;
	unsigned int	iSequence	: 6;
	unsigned int	fMapSpotted : 1;	// do we draw this on the map?
	unsigned int	AIbits		: 5;
	unsigned int	inVisible	: 1;
	unsigned int    Frozen   	: 1;
	unsigned int	SkipFrame	: 1;
	unsigned int	fDrawnLastFrame	: 1;
	unsigned int	unused		: 4;
	SHORT			type;
	SHORT			ssect;				// ssector it is in
	SHORT			sect;				// sector it is in
	SHORT			next_thing;
	SHORT			iBitm;
	SHORT			iAnim;
	SHORT			heiScaled;
	SHORT			widScaled;
	UBYTE			angle;			// Byteans 0 -> 255.
	UBYTE			scale_adjust;
	UBYTE			bControl;
	SBYTE			ColorRemap;		// index into the shade table.
} MYTHING;
	
/* Animation Header */
typedef struct
{
	ULONG	offData;			/* offset into Data of current sequence */
#if defined (_DEBUG)
	ULONG	offDataHistory[8];
	USHORT	frameHistory[8];
	UBYTE	SequenceHistory[8];
	UBYTE	RotationHistory[8];
#endif
	SHORT	width;		      	/* width in pixels */
	USHORT	height;		   		/* height in pixels */
//	USHORT	scale;		   		/* current scale discovered */
//	USHORT	speed;		   		/* in milliseconds */
//	SHORT	iData[NUMSEQUENCES];/* Array of flic sequences. */
	SHORT	hiData;				/* Handle to an I_DATABLK resource. */
	SHORT	iBuff;		   		/* handle to delta accumulation buffer */
	SHORT	sequence;	      	/* current sequence */
	SBYTE	rotation;  	      	/* current rotation */
	SBYTE	type;		      	/* set to TYPE_FLIC or TYPE_PCX */
	UBYTE	frame;		   		/* current frame in sequence */
	UBYTE	totalFrames;		/* total frames for this sequence */

} ANIMHDR, *ANIMPTR;

typedef struct _I_DATA_BLK
{
#if defined (_DEBUG)
	char	cpType[12];
#endif
	SHORT	iData[NUMSEQUENCES];
} I_DATA_BLK;

typedef struct{
	char	name[7];			// Dos file name of the art.
	SHORT	LowResIndex;		// the index in the TTYPE array to a lower resolution thing.
	ULONG	attrib;
}TTYPE;

typedef struct
{
	// LONG	t;
	SHORT	t;										// [d6-26-96 JPC]
	// SHORT	sector;								// [d6-26-96 JPC]
	SHORT	light;								// [d6-26-96 JPC]
	LONG	sx;				
	LONG	dx,dy,dye;		
	LONG	clip;			
	LONG	src_inc;		
} THING_SPAN;


/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

extern LONG		MaxThingSpans;

extern THING_SPAN	*thing_spans;
extern LONG			tot_thing_spans;
extern TTYPE		ttypes[];
extern PTR        	TTypePtr;
extern LONG       	G_TTypeSize;
extern LONG       	G_NumberOfTTypes;


//GEH try to get this out of stuff!
extern MYTHING *	mythings;

// GWP Hack variable to reduce the number of compares in CheckBump.
extern LONG MaxThingLoaded;

/* things.c */
ULONG GetMinMemConvert(ULONG type);
void MarkOldAnimPurgable(SHORT iAnim, ULONG seq);
SHORT load_FLC_sequence (ULONG iAnim, ULONG type, ULONG seq, ULONG Rotation);
ERRCODE decode_frame(ANIMPTR pAnim, FLICHEADPTR pHead);
void decode_byte_run (PTR pData, ANIMPTR pAnim);
void decode_literal (PTR pData, ANIMPTR pAnim);
void decode_delta_flc (PTR pData, ANIMPTR pAnim);
void ChangeThingType(LONG ThingType, LONG NewType);
void load_obj_graphic (LONG i,
					   ULONG type,
					   ULONG AnimationSeq,
					   SBYTE rot,
					   UBYTE bFlcControl,
					   UBYTE bPCXControl);
LONG create_thing (ULONG type, LONG x,LONG y,LONG z);
void init_things (LONG * PlayerStart);
void purge_thing (LONG /* ThingIndex */ );
void purge_all_things (void);
void set_thing (LONG index,LONG x,LONG y,LONG z,LONG a);
void move_thing (LONG t,LONG dx,LONG dy);
void move_thing_to (LONG t,LONG nx,LONG ny);
void LoopSequenceContinuously (LONG t, ULONG sequence);
void LoopSequenceOnce (LONG t, ULONG sequence);
void PlaySequenceOnce (LONG t, ULONG sequence);
BOOL TestSequenceDone (LONG t);
LONG SequenceFrameNumber (LONG t);
void remove_thing (LONG t);
void add_thing (LONG t,LONG ss);
void clear_thing_spans (void);
SHORT AllocateAnim(SHORT Type, LONG ttype);
SHORT OpenAnim (SHORT iAnim, SHORT iSeq, CSTRPTR szFileName, SHORT Type);
void RemoveAnim (SHORT iAnim);
SHORT SetAnimSequence (SHORT iAnim, SHORT iSeq);
SHORT NextAnimFrame (SHORT iAnim, USHORT sequence, USHORT rotation, UBYTE * pCtrl, SHORT frame);
void RegisterTTypes (PTR _TTypePtr, LONG _G_TTypeSize, LONG /* NumberOfElements */);
BOOL IsBumpable(LONG /* ThingIndex */);
BOOL HangsFromCeiling(LONG /* ThingIndex */ );
BOOL IsFlyingThing(LONG /* ThingIndex */ );
BOOL IsHalfWidthBumpable(LONG /* ThingIndex */);
BOOL CanStandOn(LONG /* ThingIndex */);
BOOL ThingIsHeatSource (LONG thingIndex);
BOOL ThingIsCold (LONG thingIndex);
BOOL ThingIsMagic (LONG thingIndex);
BOOL ThingIsEvil (LONG thingIndex);
BOOL ThingIsPickupable (LONG thingIndex);
USHORT get_center_point(SHORT iAnim);
LONG get_scale(SHORT iAnim);
void change_scale_adjust(LONG t, LONG newScale);
void ChangeThingZ (LONG iSector);
void SetFrame(LONG t /* ThingIndex */, ULONG iSeq, SHORT iFrame);
void sort_things_from_camera(LONG);
void SetDontDrawCloser(LONG /* ThingIndex */);
void ClearDontDrawCloser();

void SetQuestThing(LONG /* ThingType */);
LONG GetQuestThing();
void InitMagicThings(SHORT * /* pMagicObjects */,
					 LONG /* NumberOfObjects */,
					 LONG /* MagicThingIndex */,
					 LONG /* QuestThingIndex */ );

LONG QuestItemMythingsIndex(void);
SHORT DisposeIDataBlk (SHORT iResBlk, SHORT iMemBlk);
SHORT LoadIData (
	CSTRPTR szFileName,
	BOOL  fSetPal,
	BOOL  fLockRes,
	BOOL  fRotated,
	LONG  iResFileSlot);

#if defined (__cplusplus)
}
#endif

#endif

/* ------------------------------------------------------------------------ */
