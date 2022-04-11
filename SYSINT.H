/* =®RM250¯======================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _SYSINT_H
#define _SYSINT_H

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define SIZEOFIFFHEADER		56L

/* Known file and chunk types */
#define codeNONE	0x454E4F4EL
#define codeLZSS	0x53535A4CL
#define codeRLE	0x20454C52L
#define codeFORM	0x4D524F46L
#define codeBMHD	0x44484D42L
#define codeBODY	0x59444F42L
#define codeCMAP	0x50414D43L
#define codeILBM	0x4D424C49L
#define codePBM	0x204D4250L
#define codePCX8	0x0801050AL
#define codeFLC	0xAF12u

/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */

/* ILBM header structure */
typedef struct ilbmhdr
{
	USHORT	w;				/* raster width & height in pixels */
	USHORT	h;
	SHORT		x;				/* position for this image */
	SHORT		y;
	UBYTE		nPlanes;		/* # source bitplanes */
	UBYTE		masking;		/* masking technique */
	UBYTE		fCmpr;		/* compression algorithm. 0=none 1=RLE */
	UBYTE		pad[9];
} ILBMHDR;

/* PCX header structure */
typedef struct pcxhdr
{
	ULONG		code;
	SHORT		xo;
	SHORT		yo;
	SHORT		w;				/* raster width & height in pixels */
	SHORT		h;
	UBYTE		pad1[66L-12L];
	USHORT	cbLine;
} PCXHDR;

/* Information structure for a VESA mode */
typedef struct vesamodeinfo {
	USHORT	flgAttributes;	/* mode attributes */
	UBYTE		flgWindowA;		/* window A attributes */
	UBYTE		flgWindowB;		/* window B attributes */
	USHORT	ckbGranularity;	/* window position granularity in 1K units */
	USHORT	ckbWinSize;		/* window size in 1K units */
	USHORT	segWindowA;		/* window A segment in host address space */
	USHORT	segWindowB;		/* window B segment in host address space */
	PTR		pfBanking;		/* pointer to banking (windowing) procedure */
	USHORT	cbtRowSize;		/* size of each scan line in bytes */
	USHORT	XResolution;
	USHORT	YResolution;
	UBYTE		XCharSize;
	UBYTE		YCharSize;
	UBYTE		NumberOfPlanes;
	UBYTE		BitsPerPixel;
	UBYTE		NumberOfBanks;
	UBYTE		MemoryModel;
	UBYTE		BankSize;
	UBYTE		aReserved[227];	/* reserved */
} VESAMODEINFO, *VESAMODEINFOPTR;

/* VESA information structure for video card */
typedef struct vesacardinfo {
	UBYTE		VESAsig[4];		/* 'VESA', indicating it's a valid struct */
	SHORT		VESAver;			/* VESA BIOS version number */
	ULONG		OEMstr;			/* far ptr to OEM string */
	UBYTE		Capable[4];		/* Capabilities */
	ULONG		VModePtr;		/* far ptr to supported mode list */
	UBYTE		Reserved[238];
} VESAINFOBLOCK, *VESAINFOBLOCKPTR;

/* Information structure for the key handler */
typedef struct _key_slot
{
	UBYTE		Scan;					/* scan code of this key */
	UBYTE		Class;				/* remove class of this key */
	UBYTE		Flags;				/* attribute flags */
	void 	(* pFunct)();		/* function pointer */
	LONG		wArg1;				/* arguments to pass to function */
	LONG		wArg2;				/* arguments to pass to function */
} KSLOT, *KSPTR;

/* Information structure for the timer handler */
typedef struct _timer_slot
{
	ULONG		cTicsPer;			/* count of tics per execution */
	ULONG		cTicsAcc;			/* count of tics since last execution */
	UBYTE		Class;				/* remove class of this event */
	UBYTE		Flags;				/* attribute flags */
	void 	(* pFunct)();		/* function pointer */
	LONG		wArg1;				/* arguments to pass to function */
	LONG		wArg2;				/* arguments to pass to function */
} TSLOT, *TSPTR;

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

#endif // _SYSINT_H
