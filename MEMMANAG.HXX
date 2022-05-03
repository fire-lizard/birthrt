/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _MEMMANAG_HXX
#define _MEMMANAG_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define _EXTENDED_ATTRIBUTES	1
#define _AUTO_RESOURCE_ID		1

/* Equates for memory manager block attributes */
// Note: UNUSED, FREEMEM, BLKINUSE, BLKTYPE3, BLKTYPE4, STARTBLK, ENDBLK
//       are all mutually exclusive states, not bits.
#define UNUSED				0x0000
#define FREEMEM		   		0x0001
#define BLKINUSE			0x0002
#define BLKTYPE3			0x0003
#define BLKTYPE4			0x0004
#define STARTBLK			0x0005
#define ENDBLK				0x0006
#define BLKTYPEMASK	   		0x0007

// Note : CLASSPERM, CLASS1, CLASS2, CLASSTEMP are mutually exclusive states,
//		  not bits.
#define CLASSPERM			0x0000
#define CLASS1				0x0008
#define CLASS2				0x0010
#define CLASSTEMP			0x0018
#define CLASSMASK			0x0018

// Note: LOCKED, PURABLE, MODLY, MODIFABLE are all bits, not states.
#define LOCKED				0x0020
#define RESOURCE			0x0040	// Used by Dispose when called from PurgeMem_
#define PURGABLE			0x0080

#define MOLDYBIT			0x0100	// Set this render frame if touched by BLKPTR.
#define MOLDYBITLASTTIME	0x0200	// The previous state of MOLDYBIT, last frame.
#define MULTI_USER_BIT		0x0400
#define HIGHMEMORY			0x1000	// Allocated by NewBlockHigh

// Note: The Resource Id Bit is set on the resource handle itself.
#define RESOURCE_ID_BIT		0x4000
#define RESOURCE_ID_MASK	0xBFFF

/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
typedef USHORT ATTR_BLK_TYPE;

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* These macros set or clear attribute bits in the memory manager */
#define SetQuickLock(i)			SetBlockAttr((i),LOCKED,LOCKED)		// Fragments memory if left on.
#define ClrLock(i)				SetBlockAttr((i),LOCKED,0)
#define SetPurge(i)				_SetPurge(i)
#define ClrPurge(i)				_ClrPurge(i)
#define SetResource(i)			SetBlockAttr((i),RESOURCE, RESOURCE);
#define ClrResource(i)			SetBlockAttr((i),RESOURCE, 0);
#define SetClassPerm(i) 		SetBlockAttr((i),CLASSMASK,CLASSPERM)
#define SetClass1(i)			SetBlockAttr((i),CLASSMASK,CLASS1)
#define SetClass2(i)			SetBlockAttr((i),CLASSMASK,CLASS2)
#define SetClassTemp(i) 		SetBlockAttr((i),CLASSMASK,CLASSTEMP)

#define IsLocked(i)           	(GetBlockAttr(i) & LOCKED)
#define IsResource(i)         	(GetBlockAttr(i) & RESOURCE)
#define IsPurgable(i)         	(GetBlockAttr(i) & PURGABLE)
#define IsMultiUser(i)			(GetBlockAttr(i) & MULTI_USER_BIT)
#define IsClass1(i)				((GetBlockAttr(i) & CLASSMASK) == CLASS1)
#define IsClass2(i)				((GetBlockAttr(i) & CLASSMASK) == CLASS2)
#define IsClassTemp(i)			((GetBlockAttr(i) & CLASSMASK) == CLASSTEMP)

#define SetMultiUser(i)			SetBlockAttr((i),MULTI_USER_BIT,MULTI_USER_BIT)
#define ClrMultiUser(i)			SetBlockAttr((i),MULTI_USER_BIT,0)

#define SetModifyResource(i)
#define ClrModifyResource(i)

#define IsResourceHandle(i)		((i) & RESOURCE_ID_BIT)
#define GetResourceHandleBlk(i)	((i) & RESOURCE_ID_MASK)

	
#define NewLockedBlock(i)		SetBlockAttr(NewBlockHigh(i),LOCKED,LOCKED)

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

extern ULONG *				apBlocks;				/* memmanag.c */
extern SHORT *				aiNextBlk;				/* memmanag.c */
extern SHORT *				aiPrevBlk;				/* memmanag.c */
extern ATTR_BLK_TYPE *		abBlockAttr;			/* memmanag.c */
extern USHORT				cNumBlkHeads;			/* memmanag.c */
extern BOOL					fReport;				/* memmanag.c */

#if defined (__cplusplus)
}
#endif

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

void *BLKPTR(LONG);
void* BLKPTR2(LONG, LONG);
void * _zalloc(char* FileName, LONG FileLineNo, LONG size);
ULONG InitMemManag (ULONG cbMinMemoryNeeded,
					USHORT cNumBlkHeads_,
					USHORT cNumResHeads_,
					BOOL fReport_);
void QuitMemManag (void);
SHORT NewExternalBlock (PTR p);
SHORT SetBlockAttr (SHORT i, ATTR_BLK_TYPE mask, ATTR_BLK_TYPE value);
ATTR_BLK_TYPE GetBlockAttr (SHORT i);
ULONG GetBlockSize (LONG i);
ULONG GetDataBlkSize (LONG i);
SHORT _NewBlock (ULONG cBytes);
SHORT _NewBlockHigh (ULONG cBytes);
SHORT _SetPurge (SHORT );
SHORT _ClrPurge (SHORT );
SHORT DisposBlock (SHORT i);
SHORT DisposClass (UBYTE );
SHORT SetPurgeClass (UBYTE );
ULONG ReportFreeMem (ULONG fPurgable);
ULONG ReportInUse (ULONG fPurgable);
SHORT SetBlockSize (SHORT i, ULONG cBytes);
SHORT HandleOutOfMemory (void);
void PrintMemList (void);
void ClearAllMoldyBits();
BOOL IsBlockMoldy(SHORT i);
BOOL IsBlockPurgable(SHORT i);
BOOL IsBlockMultiUser(SHORT i);

BOOL IsHandleFlushed(LONG i);

#define SetDebugInfo(file,line, hdl)	hdl
#define NewBlock(x)			_NewBlock(x)
#define NewBlockHigh(x)		_NewBlockHigh(x)
#define zalloc(x)			_zalloc("", 0, x)

#if defined (__cplusplus)
}
#endif

#endif // _MEMMANAG_HXX
