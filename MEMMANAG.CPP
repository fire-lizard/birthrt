/* ========================================================================
	MemManag.c - Memory manager routines
   Copyright (c) 1989-1994 by Alan B. Clark
   Right for non-exclusive use by Synergistic Software granted.
	All other rights reserved.
	========================================================================
	Contains the following internal functions:
.		get_free_mem	- returns the available in the far heap
.		CheckValidBlk	- verify block number is acceptable
.		FindFreeHeader	- find a free block header
.		SetBlockAttr	- Used by all the Set and Clr functions below
.		GetBlockSize	- returns the size of a block in bytes
.		FindBlock		- finds a block of memory and returns an index to it
.		CleanUpBlock	- perform housekeeping on a block of memory
.		MergeBlocks		- If current and next blocks are free then concatinate them
.		ReportFreeMem	- return the largest block that could be made by compaction
.		CompactMem		- move blocks around to concentrate free memory
.		PurgeMem			- remove purged blocks to satisfy an allocation request
.		MoveFreeBlock	- swap the positions of an empty and an inuse block

	Contains the following general functions:
+		InitMemManag	- initializes the memory manager
+		QuitMemManag	- Releases the memory allocated to the memory manager
.		ValidateBlockID - Check for a valid block ID: fatal_error or pass-thru
+		ClrLock			- unlock a locked block
.		SetPurge			- allow a block to be purged
.		ClrPurge			- dissallow purging a block
+		SetResource		- mark a block as a resource
+		ClrResource		- remove the resource mark
+		SetClassPerm	- mark a block as perminent
+		SetClass1		- mark a block as class 1
+		SetClass2		- mark a block as class 2
+		SetClassTemp	- mark a block as temporary
.		GetBlockIndex	- find the block index cooresponding to a pointer
+		NewExternalBlock - Make a block header point to a block outside the manager
+		NewBlock			- allocates a new block of memory
+		DisposBlock		- deallocates a block of memory. if a resource, inform the resource manager
+		DisposClass		- deallocates all blocks of memory within a class
.		SetPurgeClass	-	sets all blocks of memory within a class as purgable
.		SetBlockSize	- enlarge or shrink a block while preserving its contents
+		SetLock			- lock a block of memory in place

	======================================================================== */
/* ®RM200¯ */
/* includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <dos.h>
#include <limits.h>
#include <map>

#ifndef _WINDOWS
#include <i86.h>
#include <bios.h>
#else
#include <Windows.h>
#endif

#include <conio.h>
#include "SYSTEM.H"
#include "MACHINE.H"

#include "MEMMANAG.HXX"
#include "RESMANAG.HXX"

using namespace std;

/* ------------------------------------------------------------------------
	Defines for memory manager functions
   ------------------------------------------------------------------------ */

// heap allocations
#ifdef _WINDOWS
	#define malloc(size)	HeapAlloc(hHeap, 0, size)
	#define free(p)			HeapFree(hHeap, 0, p)
	extern HANDLE hHeap;
#endif

/* ------------------------------------------------------------------------ */
/* Defines and Compile flags */

#define RES_MANAGER		1

#define UP_IN_MEM		1
#define DOWN_IN_MEM		2
#define START_BLOCK		0
#define FIRST_BLOCK		1
#define END_BLOCK		2

#define	CLEAN_TO_HIGHER		0
#define	CLEAN_TO_LOWER		1

// Used for debugging by writing a test block at the head & tail of the block.
#define MEMORY_MARKER		0xDEADC0DE
#define UNINITED_MEMORY		0xDC

#define MARKER_SPACE		0
#define HEADER_SPACE		0

/* ------------------------------------------------------------------------ */
/* Macros */

#define TEST_BLK_INUSE(x)	((((x) & BLKTYPEMASK) >= BLKINUSE) && (((x) & BLKTYPEMASK) < STARTBLK))
/* ------------------------------------------------------------------------ */

/* Prototypes */
typedef enum {
	MEM_ERROR_NONE = 0,
	MEM_ERROR_BAD_HANDLE,
	MEM_ERROR_BAD_HEAD,
	MEM_ERROR_BAD_TAIL,
	MEM_ERROR_ZERO_BLOCK,
	MEM_ERROR_UNUSED_BLOCK,
	MEM_ERROR_FREE_BLOCK,
} MEM_ERROR_CODE;

static LONG			CheckValidBlock (LONG, LONG);
static LONG			ValidateBlockID (LONG);
static SHORT		FindFreeHeader (void);
static SHORT		FindBlock (ULONG);
static LONG			CleanUpBlock (LONG, BOOL, ULONG);
static LONG			MergeBlocks (LONG);
static ULONG		CompactMem_ (ULONG, LONG, LONG *);
static ULONG		PurgeMem_ (ULONG, BOOL *, SHORT *);
static BOOL			MoveFreeBlockUp (LONG);
static BOOL			MoveFreeBlockDown (LONG);
static ULONG		FillFreeBlock(LONG);
static LONG 		FindCloseFit(LONG , ULONG , ULONG *);
static LONG			SearchBackFor(LONG cBytes);
static void 		ConcatinateMem();

#ifdef _WINDOWS
ULONG WinCheckMem(void);
#endif

/* ------------------------------------------------------------------------ */
/* Global Variables */

PTR		pHeap;

ULONG	*	apBlocks;
SHORT	*	aiNextBlk;
SHORT	*	aiPrevBlk;
ATTR_BLK_TYPE	*	abBlockAttr;

USHORT	cNumBlkHeads = 0;
USHORT	iMaxBlkHeadUsed = 0;

BOOL	fReport;

static map<int, void*> memoryBlocks;

struct meminfo {
	ULONG		LargestBlockAvail;
	ULONG		MaxUnlockedPage;
	ULONG		LargestLockablePage;
	ULONG		LinAddrSpace;
	ULONG		NumFreePagesAvail;
	ULONG		NumPhysicalPagesFree;
	ULONG		TotalPhysicalPages;
	ULONG		FreeLinAddrSpace;
	ULONG		SizeOfPageFile;
	ULONG		Reserved[3];
} MemInfo;

static ULONG	cbFreemem;
	
/* ========================================================================
	InitMemManager -	initializes the memory manager
	Setup the block header linked list.
	======================================================================== */
ULONG InitMemManag (
	ULONG cbMinMemoryNeeded,
	USHORT cNumBlkHeads_,
	USHORT cNumResHeads_,
	BOOL fReport_)
{
	ULONG		cbMaxFree;
	SHORT		i = 0;
	CHAR		buffer[128];

	fReport = fReport_;
	
	#if defined(MEMORY_CHK)
	pLook = 0;
	#endif

	if (fReport & fREPORT_MEMMGR)
		printf("\n\n\nMemory Manager Initialization\n");

	cNumBlkHeads = cNumBlkHeads_;
	apBlocks	 = (ULONG *)malloc((ULONG)(cNumBlkHeads*sizeof(ULONG *)));
	aiNextBlk	 = (SHORT *)malloc((ULONG)(cNumBlkHeads*sizeof(SHORT)));
	aiPrevBlk	 = (SHORT *)malloc((ULONG)(cNumBlkHeads*sizeof(SHORT)));
	abBlockAttr	 = (ATTR_BLK_TYPE *)malloc((ULONG)cNumBlkHeads *sizeof(ATTR_BLK_TYPE));
	
	for (i=0; i<cNumBlkHeads; ++i)		/* init all blocks to unused */
		abBlockAttr[i] = UNUSED;

#if RES_MANAGER
	// The resource manager also uses malloc memory.
	InitResourceManager(cNumResHeads_);
#endif

#ifdef _WINDOWS
	// WINDOWS VERSION
	cbMaxFree = WinCheckMem();
#else
	// DOS VERSION
	cbMaxFree = get_free_mem();
#endif

	printf("memory available: %ld\n",cbMaxFree);

	cbFreemem = cbMaxFree - 4000;

	// count down til we find the real size of free memory or we can't find enough
	for (pHeap = NULL; pHeap == NULL && cbFreemem > cbMinMemoryNeeded; cbFreemem -= 0x10000)
		pHeap = (PTR)malloc(cbFreemem);

	printf("memory allocated: %ld Starting at 0x%lx\n",cbFreemem, pHeap);

	if (cbFreemem < cbMinMemoryNeeded || pHeap == NULL)
	{
		if (pHeap == NULL)
			puts("Unable to allocate memory in Memmanag. pHeap is NULL\n");
		else
		{
			sprintf(buffer,"Insufficient memory to execute program.\nNeeded at least %ldk bytes free, found %ldk bytes free.\nPlease remove some system resident utilities and try again.\n",
				(cbMinMemoryNeeded/1024L), (cbFreemem/1024L) );
			puts(buffer);
		}
		
		FreeResourceManager();
		
		free(pHeap);
		free(apBlocks);
		free(aiNextBlk);
		free(aiPrevBlk);
		free(abBlockAttr);
		exit(EXIT_SUCCESS);
	}

	apBlocks[START_BLOCK] = (ULONG)pHeap;				/* START-OF-SPACE marker */
	aiNextBlk[START_BLOCK] = FIRST_BLOCK;
	aiPrevBlk[START_BLOCK] = (SHORT)fERROR;						/* means no link */
	abBlockAttr[START_BLOCK] = STARTBLK | LOCKED;

	apBlocks[FIRST_BLOCK] = (ULONG)pHeap;				/* first free space */
	aiNextBlk[FIRST_BLOCK] = END_BLOCK;
	aiPrevBlk[FIRST_BLOCK] = START_BLOCK;
	abBlockAttr[FIRST_BLOCK] = FREEMEM;

	apBlocks[END_BLOCK] = (ULONG)pHeap+cbFreemem;	/* END-OF-CONTIGUOUS-SPACE marker */
	aiNextBlk[END_BLOCK] = (SHORT)fERROR;						/* means no link */
	aiPrevBlk[END_BLOCK] = FIRST_BLOCK;
	abBlockAttr[END_BLOCK] = ENDBLK | LOCKED;

	return cbFreemem;
}

/*	=======================================================================
	QuitMemManag -
	======================================================================= */
void QuitMemManag ()
{
	if (fReport & fREPORT_MEMMGR)
		puts("MemManager removed");
}

/* ====================================================================
	get_free_mem	-
	==================================================================== */
#ifndef _WINDOWS
ULONG get_free_mem()
{
union REGS regs;
struct SREGS sregs;

	regs.x.eax = 0x00000500;
	memset( &sregs, 0, sizeof(sregs) );
	sregs.es = FP_SEG( &MemInfo );
	regs.x.edi = FP_OFF( &MemInfo );

	int386x( DPMI_INT, &regs, &regs, &sregs );
	return( MemInfo.LargestBlockAvail );
}
#endif

/* ========================================================================
	CheckValidBlock -
	======================================================================== */
static LONG CheckValidBlock (LONG i, LONG LineNo)
{
	if (i < 1 || i >= cNumBlkHeads)
	{
		return fERROR;
	}

	if ((abBlockAttr[i] & BLKTYPEMASK) == UNUSED)
	{
		return fERROR;
	}
	return fNOERR;
}

/* ========================================================================
	ValidateBlockID -
	======================================================================== */
static LONG ValidateBlockID (LONG i)
{
	if (i < 1 || i >= cNumBlkHeads)
	{
		return fERROR;
	}
	if ((abBlockAttr[i] & BLKTYPEMASK) == UNUSED)
	{
		return fERROR;
	}
	return i;
}

/* ========================================================================
	FindFreeHeader	- find a free block header
	======================================================================== */
static SHORT FindFreeHeader (void)
{
	static SHORT	j = 0;
	SHORT			jj = j;
	BOOL			fAlreadyTriedAgain = FALSE;

TryAgain:

	++j;
	// if we are already at the end of the list,
	// start back at the first posibile empty block
	if (j >= cNumBlkHeads)
		jj = j = 1;

	while (abBlockAttr[j] != UNUSED)
	{
		++j;

		if (j >= cNumBlkHeads)
			j = 0;

		if (j == jj)				/* search is exhausted, no free block found */
		{
			if (fAlreadyTriedAgain)
			{
				/* no free block header error */
				return (SHORT)fERROR;
			}
			else
			{
				LONG LargestBlock;
				
				// compact all memory
				CompactMem_(ULONG_MAX, aiNextBlk[START_BLOCK], &LargestBlock);
				fAlreadyTriedAgain = TRUE;
				goto TryAgain;
			}
		}
	}

	++iMaxBlkHeadUsed;

	if (fReport & fREPORT_MEMMGR)
		printf("FindFreeHeader: found free header %d\n",j);
	
	return j;
}

/* ========================================================================
	NewExternalBlock - Make a block header point to a block outside the manager
	======================================================================== */
SHORT NewExternalBlock (PTR p)
{
	SHORT		i;

	if ((i = FindFreeHeader()) == fERROR)	/* find an unused header */
		return (SHORT)fERROR;

	abBlockAttr[i] = BLKTYPE4;			/* mark the header as allocated */
	apBlocks[i] = (ULONG)p;
	aiNextBlk[i] = (SHORT)fERROR;
	aiPrevBlk[i] = (SHORT)fERROR;

	if (fReport & fREPORT_MEMMGR)
		printf("NewExternalBlock: assigned block %d\n",i);

	return i;
}

/* ========================================================================
	The routine SetBlockAttr set or clears the attribute bits for the block
	specified by the handle. The following functions are supported as macros:
		ClrLock			-	unlock a locked block
		SetPurge			-	allow a block to be purged
		ClrPurge			-	dissallow purging a block
		SetResource		-	mark a block as a resource
		ClrResource		-	remove the resource mark
	======================================================================== */

SHORT SetBlockAttr (SHORT i, ATTR_BLK_TYPE mask, ATTR_BLK_TYPE value)
{
#if RES_MANAGER
	if ( IsResourceHandle(i))
	{
		SHORT iResBlk = GetResourceHandleBlk(i);
		
		if (iResBlk < 0 || iResBlk >= iMaxResSlots)
		{
	#if defined (MEMORY_CHK)
			fatal_error("MEMMANAG ERROR! SetBlockAttr, Res Handle %d out of range.\n", i);
	#else
			return fERROR;
	#endif
		}
		
#if fUSE_RES_FILES
		if ((mask & CLASS2) == CLASS2)
		{
			if (value)
			{
				gResFlags[iResBlk] |= RM_CLASS2;
			}
			else
			{
				gResFlags[iResBlk] &= (0xFFFF ^ RM_CLASS2);
			}
		}
		if ((mask & PURGABLE) == PURGABLE)
		{
			if (value)
			{
				gResFlags[iResBlk] |= RM_PURGABLE;
			}
			else
			{
				gResFlags[iResBlk] &= (0xFFFF ^ RM_PURGABLE);
			}
		}
		if ((mask & MULTI_USER_BIT) == MULTI_USER_BIT)
		{
			if (value)
			{
				gResFlags[iResBlk] |= RM_MULTI_USER;
			}
			else
			{
				gResFlags[iResBlk] &= (0xFFFF ^ RM_MULTI_USER);
			}
		}
		if ((mask & LOCKED) == LOCKED)
		{
			if (value)
			{
				gResFlags[iResBlk] |= RM_LOCKED;
			}
			else
			{
				gResFlags[iResBlk] &= (0xFFFF ^ RM_LOCKED);
			}
		}
		
		i = iResBlock[iResBlk];
		if (i == 0 && giResFileNames != 0)	// It's not in memory but that's ok.
			return fNOERR;
#endif

		if ( i <= 0)
		{
			return fERROR;	// its not in memory.
		}
	}
#endif

	if (i == fERROR)
		return (SHORT)fERROR;

	if (i < 1 || i >= cNumBlkHeads)
	{
		return (SHORT)fERROR;
	}

	if (!TEST_BLK_INUSE(abBlockAttr[i]))
		return fERROR;
		
	abBlockAttr[i] &= (0xFFFF ^ mask);		/* clear field to zero */
	abBlockAttr[i] |= value;				/* set new value */
	return i;
}

/* ========================================================================
	The routine GetBlockAttr returns the attribute bits for the block
	specified by the handle.  Returns 0 in case of error.  (Returning
	fERROR would indicate all bits set, which we don't want.)
	======================================================================== */

ATTR_BLK_TYPE GetBlockAttr (SHORT i)
{
	ATTR_BLK_TYPE Result = 0;
#if RES_MANAGER
	if ( IsResourceHandle(i))
	{
		SHORT iResBlk = GetResourceHandleBlk(i);
		
		if (iResBlk < 0 || iResBlk >= iMaxResSlots)
		{
			return (ATTR_BLK_TYPE) 0;
		}
		
		i = iResBlock[iResBlk];
	#if fUSE_RES_FILES
		if (i == 0 && giResFileNames[iResBlk] != 0)	// we're currently paged out, but we could come back.
		{
			Result |= RESOURCE;
				
			// Because we don't load resources until we have too, this information
			// is duplicated in the resource manager, with its own set of flags.
			if ((gResFlags[iResBlk] & RM_LOCKED) == RM_LOCKED)
				Result |= LOCKED;
			
			if ((gResFlags[iResBlk] & RM_CLASS2) == RM_CLASS2)
				Result |= CLASS2;
		
			if ((gResFlags[iResBlk] & RM_PURGABLE) == RM_PURGABLE)
				Result |= PURGABLE;
		
			if ((gResFlags[iResBlk] & RM_MULTI_USER) == RM_MULTI_USER)
				Result |= MULTI_USER_BIT;
		}
	#endif
		if ( i <= 0)
		{
			return Result;	// it's not in memory.
		}
	}
#endif

	if (i == fERROR)
		return Result;

	if (i < 1 || i >= cNumBlkHeads)
	{
		return Result;
	}

	return abBlockAttr[i];
}

/* ========================================================================
	GetDataBlkSize	- returns the size of a data block in bytes (Less any
					  debug or header info we choose to keep.  The difference
					  in the addresses of the current block and the next
					  block is the size of the current block.
	======================================================================== */
ULONG GetDataBlkSize (LONG i)
{
#if RES_MANAGER
	if (IsResourceHandle(i))
	{
		i = Query_iResBlock(i);	// Convert a resource handle to a memory handle.
		if ( i <= 0 || i > cNumBlkHeads)
		{
			return 0L;
		}
	}
#endif
	
	if (aiNextBlk[i] == fERROR)		/* handle size request of ENDBLOCK */
		return 0L;

	return apBlocks[aiNextBlk[i]] - apBlocks[i];
}

/* ========================================================================
	GetBlockSize	- returns the size of a block in bytes
	The difference in the addresses of the current block and the next
	block is the size of the current block.
	======================================================================== */
ULONG	GetBlockSize (LONG i)
{
	if (aiNextBlk[i] == fERROR)		/* handle size request of ENDBLOCK */
		return 0L;
	return (apBlocks[aiNextBlk[i]] - apBlocks[i]);
}

/* ========================================================================
	GetBlockIndex	-  Recover a lost pointer back to the handle.
	======================================================================== */
SHORT	GetBlockIndex (PTR p)
{
	SHORT	iHandle;

	for (iHandle = aiNextBlk[START_BLOCK];
	     (abBlockAttr[iHandle] & BLKTYPEMASK) != ENDBLK;
		 iHandle = aiNextBlk[iHandle])			/* move to next block */
	{
		if (apBlocks[iHandle] == (ULONG)p)
			return (iHandle);
	}

	return fERROR;
}

/* ========================================================================
	NewBlock			-	allocates a new block of memory
	Find a free block of at least the desired size then mark it as in use
	then clean up any excess bytes into a new free block.
	======================================================================== */
SHORT _NewBlock (ULONG cBytes)
{
	SHORT		i;
	LONG		alignment;
	
	// return int aligned data pointers, by allocating %sizeof(int);
	alignment = cBytes % sizeof(int);
	if (alignment)
	{
		cBytes += (sizeof(int) - alignment);
	}
	
	/* find a block of the requested size */
	if ((i = FindBlock(cBytes)) == fERROR)
	{
		return (SHORT)fERROR;
	}

	/* mark the block as allocated */
	abBlockAttr[i] = BLKINUSE;

	/* clean up the block to the requested size */
	CleanUpBlock(i, CLEAN_TO_HIGHER, cBytes);

	return i;
}

/* ========================================================================
	FindBlock	- finds a large enough block in low memory and returns an index to it
	======================================================================== */
static SHORT FindBlock (ULONG cBytes)
{
	LONG		cbFound;
	LONG		i;
	LONG		attrMask;
	BOOL		FirstPass = TRUE;

	if (cBytes == 0)					/* can't have zero size blocks */
		return (SHORT)fERROR;
			
Restart:
	
	i = aiNextBlk[START_BLOCK];
	attrMask = abBlockAttr[i] & BLKTYPEMASK;
	
	if (attrMask == FREEMEM && abBlockAttr[aiNextBlk[i]] == FREEMEM)
	{
		MergeBlocks(i);
	}

	/* search for a free block at least as big as we need */
	while (attrMask != FREEMEM || GetBlockSize(i) < cBytes)
	{
		i = aiNextBlk[i];			/* move to next block */

		attrMask = abBlockAttr[i] & BLKTYPEMASK;
		
		// Concatinate adjcent free blocks as we find them.
		if (attrMask == FREEMEM && abBlockAttr[aiNextBlk[i]] == FREEMEM)
		{
			MergeBlocks(i);
		}
			/* search unsucessful */
		if (attrMask == ENDBLK
			|| ( (abBlockAttr[i] & HIGHMEMORY) && FirstPass == TRUE) // Try to stay out of high memory
			)
		{
			
		FirstPass = FALSE;		// Well if you have to use the high memory.
		/* No block has been found which is large enough. Therefore we must
			do garbage collection. We will attempt the following steps:
				if there isn't enough memory available by compaction
				1nd	PurgeMem will be called to free some memory...
				2rd	call CompactMem */
		#if RES_MANAGER		// Only Resources can be marked purgable.
			/* purge enough to insure that compact will succeed */
			{
				ULONG cbCurrentFree = ReportFreeMem(FALSE);
				
				if (cbCurrentFree < cBytes)
				{
					BOOL SingleBlock = FALSE;
					SHORT FoundBlock;
					
					PurgeMem_(cBytes, &SingleBlock, &FoundBlock);
					if (SingleBlock == TRUE)
					{
						// If we were able to get enough memory with one block
						// don't bother compacting, just use it.
						return FoundBlock;
					}
				}
			}
		#endif
			cbFound=CompactMem_(cBytes, aiNextBlk[START_BLOCK], &i);
			
			if (cbFound >= (LONG)cBytes)
				goto LeaveFn;

			if (HandleOutOfMemory())	/* application specific out-of-memory handler */
				goto Restart;				/* returns true if some memory was freed */

			/* Search unsucessful after compact and purge */
			printf("MEMMANAG ERROR - insufficient memory, requested:%ld  available:%ld\n",cBytes,cbFound);
			PrintMemList();
			return (SHORT)fERROR;
		}
	}

LeaveFn:
	if(i < 0 || i > cNumBlkHeads)
		i = fERROR;
		
	/* a free block of an acceptable size has been found */
	return (SHORT)i;
}

/* ========================================================================
	NewBlockHigh - finds a large enough block high in memory, returns index
	======================================================================== */
SHORT _NewBlockHigh (ULONG cBytes)
{
	LONG		cbFound;
	LONG		alignment;
	LONG		i;
	BOOL		FirstPass = TRUE;

	if (cBytes == 0)					/* can't have zero size blocks */
		return (SHORT)fERROR;

	// return int aligned data pointers, by allocating %sizeof(int);
	alignment = cBytes % sizeof(int);
	if (alignment)
	{
		cBytes += (sizeof(int) - alignment);
	}
	
	ConcatinateMem();	// Combine any free blocks into as large as can be.

Restart:
	i = aiPrevBlk[END_BLOCK];

	/* search for a free block at least as big as we need */
	while ((abBlockAttr[i] & BLKTYPEMASK) != FREEMEM || GetBlockSize(i) < cBytes)
	{
		i = aiPrevBlk[i];			/* move to previous block */

		 /* search unsucessful */
		if ((abBlockAttr[i] & BLKTYPEMASK) == STARTBLK
		   || (!(abBlockAttr[i] & HIGHMEMORY) && FirstPass == TRUE)			// Try to keep High memory High
		   )
		{
			LONG iStartBlk;
			
			FirstPass = FALSE;
			
		#if RES_MANAGER		// Only Resources can be marked purgable.
			/* purge enough to insure that compact will succeed */
			{
				ULONG cbCurrentFree = ReportFreeMem(FALSE);
				
				if (cbCurrentFree < cBytes)
				{
					BOOL SingleBlock = FALSE;
					SHORT FoundBlock;
					
					// We're not going to use any found blocks as we want to
					// compact to try and keep high memory blocks high.
					PurgeMem_(cBytes, &SingleBlock, &FoundBlock);
				}
			}
		#endif
			iStartBlk = SearchBackFor(cBytes);
			cbFound=CompactMem_(cBytes, iStartBlk, &i);
			if (cbFound >= (LONG)cBytes)
				goto LeaveFn;

			if (HandleOutOfMemory())	/* application specific out-of-memory handler */
				goto Restart;				/* returns true if some memory was freed */

			/* Search unsucessful after compact and purge */
			return (SHORT)fERROR;
		}
	}

LeaveFn:
	/* mark the block as allocated */
	abBlockAttr[i] = BLKINUSE | HIGHMEMORY;
	
	/* clean up the block to the requested size */
	CleanUpBlock(i, CLEAN_TO_LOWER, cBytes);

	return (SHORT)i;
}

/* ========================================================================
   Function    - SearchBackFor
   Description - Starting at the end block, search backward until you come
   				 to the starting index for which if you compacted, you'd have
   				 contigous cBytes.
   				 Call this after calling purge.
   Returns     - 
   ======================================================================== */

static LONG SearchBackFor(LONG cBytes)
{
	LONG iHandle;
	ULONG MaxBlockSize = 0;
	ULONG BlkBits;
	
	for (iHandle = aiPrevBlk[END_BLOCK], 
		  BlkBits = abBlockAttr[iHandle] & BLKTYPEMASK;
	     
	     BlkBits != STARTBLK && cBytes > MaxBlockSize;
	     
	     iHandle = aiPrevBlk[iHandle], 
	      BlkBits = abBlockAttr[iHandle] & BLKTYPEMASK
	    )
    {
    	if (BlkBits == FREEMEM)
    	{
    		MaxBlockSize += GetBlockSize(iHandle);
    	}
    	else if (abBlockAttr[iHandle] & LOCKED)
    	{
    		MaxBlockSize = 0;
    	}
    }
    
    return iHandle;
}
/* ========================================================================
	_SetPurge	-	Support the extensable resource manager's SetPurge feature
	======================================================================== */
#if RES_MANAGER	// Only resources can be marked puragable.
SHORT _SetPurge (SHORT iHandle)
{

	if (iHandle < 0)
	{
		return fNOERR;
	}
		
	if (IsResourceHandle(iHandle))
	{
		SHORT iMemBlk;
		SHORT iResBlk = GetResourceHandleBlk(iHandle);
		
		if (iResBlk >= iMaxResSlots)
		{
			return fNOERR;
		}
		
		iMemBlk = iResBlock[iResBlk];
		if (iMemBlk < 0)
			return fNOERR;	// Its not available.
		
		/* inform the resource manager */
	#if MOVE_MEMORY_TEST
		return (*ResExtentions[iResExtIndex[iResBlk]].pfDisposeProc)(iResBlk,iMemBlk);
	#else
		return (*ResExtentions[iResExtIndex[iResBlk]].pfSetPurgeProc)(iResBlk,iMemBlk);
	#endif
	}

	/* check for invalid block */
	if (CheckValidBlock(iHandle, __LINE__) == fERROR)
		return (SHORT)fERROR;

	return DisposBlock(iHandle);
}
#endif

/* ========================================================================
	_ClrPurge	-	Support the extensable resource manager's ClrPurge feature
	======================================================================== */
#if RES_MANAGER	// Only resources can be purgable.
SHORT _ClrPurge (SHORT iHandle)
{
	if (iHandle < 0)
	{
		return fERROR;
	}
	
	if (IsResourceHandle(iHandle))
	{
		SHORT iMemBlk;
		SHORT iResBlk = GetResourceHandleBlk(iHandle);
		
		if (iResBlk >= iMaxResSlots)
		{
			return fERROR;
		}
		
		iMemBlk = iResBlock[iResBlk];
		if (iMemBlk < 0)
			return fERROR;	// Its either not available or already gone.
		
		/* inform the resource manager */
		return (*ResExtentions[iResExtIndex[iResBlk]].pfClrPurgeProc)(iResBlk,iMemBlk);
	}

	return fNOERR;
}
#endif

/* ========================================================================
	DisposBlock		-	deallocates a block of memory
	First check for a valid index. If the block is a resource, inform the
	resource manager. Then mark the block as free and try to clean it out
	of existance.
	======================================================================== */
SHORT DisposBlock (SHORT iHandle)
{

#if RES_MANAGER
	
	if (IsResourceHandle(iHandle))
	{
		SHORT iMemBlk;
		SHORT iResBlk = GetResourceHandleBlk(iHandle);
		
		if (iResBlk < 0 || iResBlk >= iMaxResSlots)
		{
			return fERROR;
		}
		
		iMemBlk = iResBlock[iResBlk];
		if (iMemBlk < 0)
			return fERROR;	// Its either not available or already gone.
		
	#if defined (MEMORY_CHK)
		if (iMemBlk != 0)
		{
			if (CheckValidBlock(iMemBlk, __LINE__) == fERROR)
				return fERROR;
		
				ReportBlockError(iMemBlk,CheckBlockOverwrite(iMemBlk));
			}
	#endif
		
		return (*ResExtentions[iResExtIndex[iResBlk]].pfDisposeProc)(iResBlk,iMemBlk);
	}
#endif

	/* check for invalid block */
	if (CheckValidBlock(iHandle, __LINE__) == fERROR)
		return fERROR;

#if RES_MANAGER
	/* test for block being a resource */
	if (abBlockAttr[iHandle] & RESOURCE)
	{
		SHORT iResBlk;
		
		// Note: We got here by being called by PurgeMem_ and do not have the
		//       resource handle.
		/* inform the resource manager */
		for (iResBlk=0; iResBlk<iMaxResSlots; ++iResBlk)
			if (iResBlock[iResBlk] == iHandle)		/* if block has been found */
				return (*ResExtentions[iResExtIndex[iResBlk]].pfDisposeProc)(iResBlk,iHandle);

		/* no resource using this block. print an error */
	#if defined(MEMORY_CHK)
		fatal_error("MEMMANAG ERROR - no resource using this block: %d\n",iHandle);
	#endif
		// Otherwise keep on going and toss this block.
	}
#endif

	if (aiNextBlk[iHandle] == fERROR)		/* check for an external block */
	{
		abBlockAttr[iHandle] = UNUSED;		/* block is external, NOT a managed part of the heap */
		iMaxBlkHeadUsed--;
		return fNOERR;
	}

	abBlockAttr[iHandle] = FREEMEM;			/* mark the block as free */
   	
   	// Concatinate adjacent free blocks.
   	// You can't concatinate the blocks, it disrupts the links
   	// in searches for purging etc. (GWP)
   	
	
	return fNOERR;
}

/* ========================================================================
	DisposClass		-	deallocates all blocks of memory within a class
	======================================================================== */
SHORT DisposClass (UBYTE Class)
{
	SHORT		iHandle;

	/* search for a block of type Class */
	iHandle = aiNextBlk[START_BLOCK];
	while ((abBlockAttr[iHandle] & BLKTYPEMASK) != ENDBLK)
	{
		if ((abBlockAttr[iHandle] & BLKTYPEMASK) > FREEMEM &&
				(abBlockAttr[iHandle] & BLKTYPEMASK) < STARTBLK &&
				(abBlockAttr[iHandle] & CLASSMASK) == Class)
		{
			DisposBlock(iHandle);
			// We don't just move to the next block, because after disposing
			// of this block we could now be the last block. So test again
			// then get the next block handle.
		}
		else
		{
			iHandle = aiNextBlk[iHandle];				/* move to next block */
		}
	}
	
	return fNOERR;
}

/* ========================================================================
	SetPurgeClass	-	sets all blocks of memory within a class as purgable
	======================================================================== */
#if RES_MANAGER		// Only Resources can be marked purgable.
SHORT SetPurgeClass (UBYTE Class)
{
	SHORT		iHandle;

	/* search for blocks of this class type */
	for ( iHandle = aiNextBlk[START_BLOCK];
	      (abBlockAttr[iHandle] & BLKTYPEMASK) != ENDBLK;
		  iHandle = aiNextBlk[iHandle])				/* move to next block */
	{
		if ((abBlockAttr[iHandle] & BLKTYPEMASK) > FREEMEM &&
				(abBlockAttr[iHandle] & BLKTYPEMASK) < STARTBLK &&
				(abBlockAttr[iHandle] & CLASSMASK) == Class)
		{
			SetPurge(iHandle);
		}
	}
	return fNOERR;
}
#endif

/* ========================================================================
	CleanUpBlock		 -	perform housekeeping on a block of memory
	Return - If a new block is created, the handle to it.
	======================================================================== */
static LONG CleanUpBlock (LONG iHandle,
						   BOOL fCleanDirection,
						   ULONG cBytes)
{
	LONG		j;

	/* check for invalid block */
	if (CheckValidBlock(iHandle, __LINE__) == fERROR)
		return (SHORT)fERROR;

	if (cBytes == 0)							/* if request is to eliminate block */
	{
		abBlockAttr[iHandle] = FREEMEM;			/* then mark the block as free */
		/* if new block and prev block are both free then concatinate them */
		if ((abBlockAttr[aiPrevBlk[iHandle]] & BLKTYPEMASK) == FREEMEM)
			MergeBlocks(aiPrevBlk[iHandle]);
		else if ((abBlockAttr[aiNextBlk[iHandle]] & BLKTYPEMASK) == FREEMEM)
			MergeBlocks(iHandle);
	}
	else
	if (GetBlockSize(iHandle) > cBytes)	/* make a new block from extra bytes */
	{
		LONG const iPrev = aiPrevBlk[iHandle];					/* get index of previous link */
		LONG const iNext = aiNextBlk[iHandle];					/* get index of next link */
		
		/* find a free block header */
		j = FindFreeHeader();
		if (j == fERROR)
			return (SHORT)fERROR;

		if (fCleanDirection == CLEAN_TO_HIGHER)
		{
			/* Link new block header in after current block */
			aiNextBlk[iHandle] = j;					   	/* link j in forward */
			aiNextBlk[j] = iNext;
			aiPrevBlk[iNext] = j;						/* link j in back */
			aiPrevBlk[j] = iHandle;
			apBlocks[j] = apBlocks[iHandle] + cBytes;	/* calc address of new block */
			abBlockAttr[j] = FREEMEM;					/* set attribute */
			
			/* if new block and next block are both free then concatinate them */
			if ((abBlockAttr[aiNextBlk[j]] & BLKTYPEMASK) == FREEMEM)
				MergeBlocks(j);
		}
		else
		{
			/* Link new block header in before current block */
			aiPrevBlk[iHandle] = j;						 /* relink j before i */
			aiPrevBlk[j] = iPrev;
			aiNextBlk[j] = iHandle;
			aiNextBlk[iPrev] = j;
			apBlocks[j] = apBlocks[iHandle];			/* calc address of new block */
			apBlocks[iHandle] = apBlocks[iNext]-cBytes;	/* move address of old block */
			abBlockAttr[j] = FREEMEM;					/* set attribute */
		
			/* if new block and prev block are both free then concatinate them */
			if ((abBlockAttr[aiPrevBlk[j]] & BLKTYPEMASK) == FREEMEM)
				MergeBlocks(aiPrevBlk[j]);
		}
	}
	else
		return fERROR;										/* nothing to fix so leave */

	return j;	// Return the new block.
}

/* ========================================================================
	MergeBlocks			- If current and next blocks are free then 
					concatinate them. Mark the appropriate header as unused.
	======================================================================== */
static LONG MergeBlocks (LONG iHandle)
{
	SHORT		iNext;
	SHORT		iNN;
	ULONG		BlkBits;

	while (1)
	{
		iNext = aiNextBlk[iHandle];
		BlkBits = abBlockAttr[iNext] & BLKTYPEMASK;
		
		if (BlkBits != FREEMEM)		// Includes ENDBLK
			break;
	
		iNN = aiNextBlk[iNext];
		aiNextBlk[iHandle] = iNN;			/* unlink iNext forward */
		aiPrevBlk[iNN] = iHandle;			/* unlink iNext back */
		
		aiNextBlk[iNext] = fERROR;
		aiPrevBlk[iNext] = fERROR;
		apBlocks[iNext] = 0;				/* clear pointer */
		abBlockAttr[iNext] = UNUSED;		/* clear attribute */
		
		iMaxBlkHeadUsed--;
	}

	return fNOERR;
}

/* ========================================================================
	ReportFreeMem - return the largest block that could be made by compaction
	======================================================================== */
ULONG ReportFreeMem (ULONG fPurgable)
{
	ULONG		cbMaxBlock = 0;
	ULONG		cbAccum = 0;
	SHORT		i;
	ULONG		abBlockMask;

	/* search for free blocks */
	for (i = aiNextBlk[START_BLOCK], abBlockMask = abBlockAttr[i] & BLKTYPEMASK;
	     abBlockMask != ENDBLK;
		 i = aiNextBlk[i], abBlockMask = abBlockAttr[i] & BLKTYPEMASK)
	{
		if (abBlockAttr[i] & LOCKED)
		{
			cbMaxBlock = MAX(cbMaxBlock, cbAccum);
			cbAccum = 0;
		}
		else
		if ( abBlockMask == FREEMEM || (fPurgable && (abBlockAttr[i] & PURGABLE)) )
			cbAccum += GetBlockSize(i);

	}

	cbMaxBlock = MAX(cbMaxBlock, cbAccum);
	return cbMaxBlock;
}

/* ========================================================================
   Function    - ReportInUse
   Description - return the total of all blocks in use.
   Returns     -
   ======================================================================== */
ULONG ReportInUse(ULONG fPurgable)
{
	SHORT i = 0;
	ULONG Result = cbFreemem;
	
	for (i = aiNextBlk[START_BLOCK];
		 i >= 0 && ((abBlockAttr[i] & BLKTYPEMASK) != ENDBLK);
		 i = aiNextBlk[i])
	{
		if ((fPurgable && (abBlockAttr[i] & PURGABLE)) ||
			(abBlockAttr[i] & BLKTYPEMASK) == FREEMEM
			)
		{
			Result -= GetBlockSize(i);
		}
	}
	
	return Result;
}


/* ========================================================================
	ConcatinateMem		-	Combine all adjcent free blocks. (But don't
	                        move memory.)
	======================================================================== */
static void ConcatinateMem ()
{
	LONG		i;
	ULONG		abBlockMask;

	/* search for a free block */
	for (i = aiNextBlk[START_BLOCK],
			abBlockMask = abBlockAttr[i] & BLKTYPEMASK;
		
		abBlockMask != ENDBLK;
		
		i = aiNextBlk[i],										/* ..move to next block */
			abBlockMask = abBlockAttr[i] & BLKTYPEMASK
		)
	{
		if ( abBlockMask == FREEMEM) /* if not free block.. */
		{
			/* Current block is free, if next blocks are free, concatinate them */
			MergeBlocks(i);
		}

	}
}
/* ========================================================================
	CompactMem		-	move blocks around to concentrate free memory
	======================================================================== */
static ULONG CompactMem_ (ULONG cBytes, LONG iStartBlk, LONG *pLargestBlock)
{
	ULONG		cbMaxBlock;
	LONG		i;
	ULONG		abBlockMask;

	/* search for a free block */
	i = iStartBlk;
	abBlockMask = abBlockAttr[i] & BLKTYPEMASK;
	if (abBlockMask == FREEMEM)
	{
		cbMaxBlock = GetBlockSize(i);	/* initialize largest block found variable */
		*pLargestBlock = i;
	}
	else
	{
		cbMaxBlock = 0;
		*pLargestBlock = fERROR;
	}
	
	while (abBlockMask != ENDBLK && cbMaxBlock < cBytes)
	{
		if ( abBlockMask != FREEMEM) /* if not free block.. */
		{
			i = aiNextBlk[i];										/* ..move to next block */
			abBlockMask = abBlockAttr[i] & BLKTYPEMASK;
			continue;
		}

		/* Current block is free, if next blocks are free, concatinate them */
		MergeBlocks(i);

			
		// current block is free
		if (abBlockMask == FREEMEM)
		{
			// Must do this before back filling because the next block is locked.
			// And the area past the locked block might not have enough room.
			ULONG cbBlockSize = GetBlockSize(i);
			
			if (cbBlockSize > cbMaxBlock)
			{
				cbMaxBlock = cbBlockSize;
				*pLargestBlock = i;
			}
			
			if (cbMaxBlock >= cBytes)
				break;
				
			if ((abBlockAttr[aiNextBlk[i]] & LOCKED) == LOCKED )
			{
				// And next block is locked then fill it
				LONG const iNext = aiNextBlk[i];	// Skip any new blocks created by FillFreeBlock.
				cbMaxBlock = cbBlockSize = FillFreeBlock(i);
				*pLargestBlock = i;
				
				i = iNext;	// move to next block
				abBlockMask = abBlockAttr[i] & BLKTYPEMASK;
			}
			else
			{
				// And next block is not locked then switch places
				MoveFreeBlockUp(i);	// switch positions and try again
			}
		}
		else
		{
			i = aiNextBlk[i];	// move to next block
			abBlockMask = abBlockAttr[i] & BLKTYPEMASK;
		}
	}

	return cbMaxBlock;
}

#if RES_MANAGER	// Only resources get marked PURGABLE.
/* ========================================================================
	PurgeMem_				-	remove purged blocks to satisfy an allocation request
				If we can do it with a single purge, return the block id
				so we don't have to compact or search for it again.
	======================================================================== */
static ULONG PurgeMem_ (ULONG cBytes, BOOL *pSingleBlock, SHORT *pFoundBlock)
{
	ULONG		cbMaxBlock = 0;
	SHORT		i;
	ULONG		attrBits;
	
	*pSingleBlock = FALSE;
	*pFoundBlock = fERROR;

	ULONG const cBytesPlus10Percent = cBytes + cBytes/10;
	
	// Since no one puragable resource is better than another, lets
	// compact the fewest possible blocks by starting at the high end of memory.
	// We may thrash, but at least we won't shift memory and thrash.
	
	// search for one "same size" block to free up first.
	// Then we can skip the compact phase.
	for (i = aiPrevBlk[END_BLOCK], attrBits = abBlockAttr[i];
		 (attrBits & BLKTYPEMASK) != STARTBLK && cbMaxBlock < cBytes;
		  i = aiPrevBlk[i], attrBits = abBlockAttr[i]	/* move to next block */
		 )
	{
		if (attrBits & PURGABLE 
		    && !(attrBits & LOCKED)
		    && !((attrBits & MOLDYBIT) ||
		      (attrBits & MOLDYBITLASTTIME))
		   )
		{
		    ULONG	const BlockSize = GetBlockSize(i);
		
			// find a similar size block to reuse.
			if (cBytes <= BlockSize &&
			    cBytesPlus10Percent >= BlockSize)
			{
				DisposBlock(i);
				cbMaxBlock = ReportFreeMem(FALSE);
				*pSingleBlock = TRUE;
				*pFoundBlock = i;
				goto Leave_fn;
			}
		}
	}
	
	/* search for purgable & Moldy blocks next */
	for ( i = aiPrevBlk[END_BLOCK], attrBits = abBlockAttr[i];
		 (attrBits & BLKTYPEMASK) != STARTBLK && cbMaxBlock < cBytes;
		  i = aiPrevBlk[i], attrBits = abBlockAttr[i]	/* move to next block */
		 )
	{
		if (attrBits & PURGABLE 
		    && !(attrBits & LOCKED)
			&& !((attrBits & MOLDYBIT) || 
			   (attrBits & MOLDYBITLASTTIME))
		   )
		{
			if (GetBlockSize(i) >= cBytes)
			{
				*pSingleBlock = TRUE;
				*pFoundBlock = i;
			}
			DisposBlock(i);
			cbMaxBlock = ReportFreeMem(FALSE);
		}
	}
	
	/* search for any purgable blocks */
	for (i = aiPrevBlk[END_BLOCK], attrBits = abBlockAttr[i];
	     (attrBits & BLKTYPEMASK) != STARTBLK && cbMaxBlock < cBytes;
		 i = aiPrevBlk[i], attrBits = abBlockAttr[i]	/* move to next block */
	     )
	{
		if (attrBits & PURGABLE
		    && !(attrBits & LOCKED))
		{
			if (GetBlockSize(i) >= cBytes)
			{
				*pSingleBlock = TRUE;
				*pFoundBlock = i;
			}
			DisposBlock(i);
			cbMaxBlock = ReportFreeMem(FALSE);
		}
	}

Leave_fn:

	return cbMaxBlock;
}
#endif

/* ========================================================================
	MoveFreeBlockUp		-	Swap the positions of an empty and an inuse block
	i is the index to the free block, dir is the direction to move it.
	======================================================================== */
static BOOL MoveFreeBlockUp (LONG i)
{
	ULONG		Src, Dest, cBytes;
	LONG const iPrev = aiPrevBlk[i];
	LONG const iNext = aiNextBlk[i];
	LONG const iNN	= aiNextBlk[iNext];
	
	/* UP_IN_MEM */
	/* move i up by moving next block down */
	/* before: iPP-iPrev-i-iNext-iNN		After:iPP-iPrev-iNext-i-iNN */

	/* can't move locked block or end block */
	if ((abBlockAttr[iNext] & LOCKED) || (abBlockAttr[iNext] & BLKTYPEMASK)==ENDBLK)
		return FALSE;
	Dest = apBlocks[i];
	Src = apBlocks[iNext];
	cBytes = GetBlockSize(iNext);		/* get the size of the inuse block */

	memmove((PTR)Dest, (PTR)Src, cBytes);

	apBlocks[iNext] = Dest;				/* new position for i is old position of prev */
	apBlocks[i] = Dest + cBytes;
	aiNextBlk[iPrev] = iNext;			/* relink forward */
	aiNextBlk[iNext] = i;
	aiNextBlk[i] = iNN;
	aiPrevBlk[iNN] = i;					/* relink back */
	aiPrevBlk[i] = iNext;
	aiPrevBlk[iNext] = iPrev;


	return TRUE;
}

/* ========================================================================
	MoveFreeBlockDown		-	Swap the positions of an empty and an inuse block
	i is the index to the free block, dir is the direction to move it.
	======================================================================== */
static BOOL MoveFreeBlockDown (LONG i)
{
	ULONG		Src, Dest, cBytes;
	LONG const iPrev = aiPrevBlk[i];
	LONG const iPP	= aiPrevBlk[iPrev];
	LONG const iNext = aiNextBlk[i];

	/* move i down by moving previous block up */
	/* before: iPP-iPrev-i-iNext-iNN		After:iPP-i-iPrev-iNext-iNN */

	/* can't move locked block or start block */
	if ((abBlockAttr[iPrev] & LOCKED) || (abBlockAttr[iPrev] & BLKTYPEMASK)==STARTBLK)
		return FALSE;

	Src = apBlocks[iPrev];
	Dest = Src + GetBlockSize(i);				/* Dest is new position of prev */
	cBytes = GetBlockSize(iPrev);

	memmove((PTR)Dest, (PTR)Src, cBytes);

	apBlocks[i] = Src;					/* new position for i is old position of prev */
	apBlocks[iPrev] = Dest;				/* set position of the moved block */
	aiNextBlk[iPP] = i;					/* relink forward */
	aiNextBlk[i] = iPrev;
	aiNextBlk[iPrev] = iNext;
	aiPrevBlk[iNext] = iPrev;			/* relink back */
	aiPrevBlk[iPrev] = i;
	aiPrevBlk[i] = iPP;

	return TRUE;
}

/* ========================================================================
	SetBlockSize	-	enlarge or shrink a block while preserving its contents
	======================================================================== */
SHORT SetBlockSize (SHORT i, ULONG cBytes)
{
	SHORT		iNew;
	LONG		alignment;

#if RES_MANAGER
	if (IsResourceHandle(i))
	{
		// I'll let the resource come in now if it isn't already. I think these
		// cases are really few. (Where calling this actually the resource is gone.)
		i = Query_iResBlock(i);
		if (i <= 0 || i > cNumBlkHeads)
			return fERROR;
		
	#if defined (CRC_CHK)
		if (!(abBlockAttr[i] & MODIFYABLE_RESOURCE))
		{
			fatal_error("MEMMANAG ERROR! Modify'ing a non-modify'able resource %d.\n",
								i);
		}
	#endif

	}
#endif

	// return int aligned data pointers, by allocating %sizeof(int);
	alignment = cBytes % sizeof(int);
	if (alignment)
	{
		cBytes += (sizeof(int) - alignment);
	}
	

SetHandleSizeLoop:

	/* Attemp to join current and next blocks */
	MergeBlocks(i);

	/* clean up current block to the requested size */
	CleanUpBlock(i, CLEAN_TO_HIGHER, cBytes);

	/* If too small then allocate a block the size of the difference
		and move it to the end of the current block */
	if (GetBlockSize(i) < cBytes)
	{
		/* if FindBlock fails then return from SetHandleSize with error */
		if ((iNew = FindBlock(cBytes - GetBlockSize(i))) == fERROR)
			return (SHORT)fERROR;

		/* While the new block is not adjacent and above the current block,
			move the new block toward the end of the current block. */
		while (aiNextBlk[i] != iNew)
		{
			if (apBlocks[iNew] < apBlocks[i])
			{
				if (MoveFreeBlockUp(iNew) == FALSE)
				{
					DisposBlock(iNew);	/* if the move fails then release the block */
					return (SHORT)fERROR;			/* and return from SetHandleSize with error */
				}
				else if (MoveFreeBlockDown(iNew) == FALSE)
				{
					DisposBlock(iNew);	/* if the move fails then release the block */
					return (SHORT)fERROR;			/* and return from SetHandleSize with error */
				}
			}
		}

		goto SetHandleSizeLoop;
	}

	return fNOERR;
}

/* ========================================================================
	SetLock	- Locks the position of a block in memory, after moving it
	as far up as it can. HEY DONT USE THIS IF MEMORY_CHK IS ON
	======================================================================== */
SHORT SetLock (SHORT iHandle)
{
	SHORT		i, j, ni, pi, nj, pj;
	ULONG		l;

#if RES_MANAGER
	if (IsResourceHandle(iHandle))
	{
	#if fUSE_RES_FILES
		SHORT iResBlk = GetResourceHandleBlk(iHandle);
		
		// Test if purged out.
		if (iResBlock[iResBlk] == 0 && giResFileNames[iResBlk] != 0)
		{
			gResFlags[iResBlk] |= RM_LOCKED;
			return fNOERR;
		}
	#endif
		
		i = Query_iResBlock(iHandle);
		if ( i < 0 || i > cNumBlkHeads)
			return fERROR;		// It's not in memory.
	}
	else
	{
		i = iHandle;
	}
#else
	i = iHandle;
#endif

	if (CheckValidBlock(i, __LINE__) == fERROR)
		return (SHORT)fERROR;

	/* check to see if its already locked */
	if (abBlockAttr[i] & LOCKED)
		return fNOERR;

	l = GetDataBlkSize(i);						/* find the size of the block */
	j = _NewBlockHigh(l);						/* get a new block of the same size */
	if (j == fERROR)							/* if no more room */
	{
		SetBlockAttr(i,LOCKED,LOCKED);	/* let's just lock in place (re. GEH and GWP) */
		return (SHORT)fNOERR;
	}

	while (MoveFreeBlockUp(j)) ;

	l = GetBlockSize(i);
	memcpy((PTR)apBlocks[j], (PTR)apBlocks[i], l);	/* copy the contents */

	/* reset the block number to the old value */
	l = apBlocks[j];
	apBlocks[j] = apBlocks[i];				/* j now points at the old block */
	apBlocks[i] = l;							/* i now points at the new block */

	/* fix-up the linked list */
	pi = aiPrevBlk[i];
	ni = aiNextBlk[i];
	pj = aiPrevBlk[j];
	nj = aiNextBlk[j];

	if (pi==j)									/* blocks are adjacent, pj-j-i-ni */
	{
		aiNextBlk[pj] = i;					/* change to pj-i-j-ni */
		aiNextBlk[i]  = j;
		aiNextBlk[j]  = ni;
		aiPrevBlk[ni] = j;
		aiPrevBlk[j]  = i;
		aiPrevBlk[i]  = pj;
	}
	else if (pj==i)							/* blocks are adjacent, pi-i-j-nj */
	{
		aiNextBlk[pi] = j;					/* change to pi-j-i-nj */
		aiNextBlk[j]  = i;
		aiNextBlk[i]  = nj;
		aiPrevBlk[nj] = i;
		aiPrevBlk[i]  = j;
		aiPrevBlk[j]  = pi;
	}
	else
	{
		aiPrevBlk[i] = pj;					/* blocks are not adjacent */
		aiNextBlk[i] = nj;
		aiPrevBlk[nj] = i;
		aiNextBlk[pj] = i;

		aiPrevBlk[j] = pi;
		aiNextBlk[j] = ni;
		aiPrevBlk[ni] = j;
		aiNextBlk[pi] = j;
	}

	abBlockAttr[i] |= LOCKED;
	
	if (IsClass1(j))
	{
		SetClass1(i);
	}
	
	if (IsClass2(j))
	{
		SetClass2(i);
	}
	
	if (IsClassTemp(j))
	{
		SetClassTemp(i);
	}
	
	if (IsResource(j))
	{
		SHORT iResBlk = GetResourceHandleBlk(iHandle);
		
		if (iResBlk < 0 || iResBlk >= iMaxResSlots)
		{
	#if defined (MEMORY_CHK)
			fatal_error("MEMMANAG ERROR! SetLock, Res Handle %d out of range.\n", iHandle);
	#else
			return fERROR;
	#endif
		}
		
		SetResource(i);
		// Find the resource handle and update it.
		iResBlock[iResBlk] = i;
		
		ClrResource(j);
	}
	
	
	if (IsPurgable(j))
	{
		SetPurge(i);
	}
	
	DisposBlock(j);

	return fNOERR;
}

/* ======================================================================== */
/* Replace the following error reporting code with an application specific
	out-of-memory handler and an application specific out-of-memory reporting
	routine. */
/* ======================================================================== */
SHORT HandleOutOfMemory ()
{
	LONG iHandle;
	ULONG BlkBits;
	SHORT Success = FALSE;
	
	// Memory is hashed, Blow all the purable memory out and try again.
	// Its slow but is better than crashing.
	
	for (iHandle = aiNextBlk[START_BLOCK],
		  BlkBits = abBlockAttr[iHandle];
		 
		 BlkBits != ENDBLK && iHandle > 0;
		 
		 iHandle = aiNextBlk[iHandle],
		  BlkBits = abBlockAttr[iHandle]
		)
	{
		if ((BlkBits & PURGABLE) && !(BlkBits & LOCKED))
		{
			DisposBlock(iHandle);
			Success = TRUE;
		}
	} 
	
	if (Success == TRUE)
	{
		LONG LargestBlock;
		
		CompactMem_(ULONG_MAX, aiNextBlk[START_BLOCK], &LargestBlock);
	}
	     

	return Success;		/* no more memory availabe to release */
}

/* ========================================================================

	======================================================================== */
void PrintMemList ()
{
#if 01
	SHORT		i, i2, col2, cc, type;
	FILE *fp = fopen("memdmp.txt", "w");
		if (!fp)
			return;

	for (col2=i=0; i!=fERROR; i=aiNextBlk[i],++col2) ;
	col2 = (col2+1) / 2;
	for (cc=1,i2=aiNextBlk[0]; cc!=col2; i2=aiNextBlk[i2],++cc) ;
	i = 0;

	fprintf(fp," #  Ptr     Size   CSum Cls Attrib    |  #  Ptr     Size   CSum Cls Attrib\n");
	fprintf(fp,"--- ------- ------ ---- --- --------- | --- ------- ------ ---- --- ---------\n");

	for (cc=0; cc < col2; ++cc)
	{
		fprintf(fp,"%03u %07lx %06lx %04x %03u ",i, apBlocks[i], GetBlockSize(i), CheckSum(i), (abBlockAttr[i]&CLASSMASK)>>3 );
		type = abBlockAttr[i] & BLKTYPEMASK;
		if (type != ENDBLK && aiPrevBlk[aiNextBlk[i]] != i)
			fprintf(fp,"*********");
		else
		{
			if (type == UNUSED)					fprintf(fp,"UNUSD");
			else if (type == FREEMEM)			fprintf(fp,"FREE ");
			else if (type == BLKINUSE)			fprintf(fp,"INUSE");
			else if (type == BLKTYPE3)			fprintf(fp,"TYPE3");
			else if (type == BLKTYPE4)			fprintf(fp,"TYPE4");
			else if (type == STARTBLK)			fprintf(fp,"START");
			else if (type == ENDBLK)			fprintf(fp,"END  ");
			else								fprintf(fp,"ERROR");
			
			if (abBlockAttr[i] & LOCKED)		fprintf(fp," L"); else fprintf(fp," -");
			if (abBlockAttr[i] & RESOURCE)		fprintf(fp,"R");  else fprintf(fp,"-");
			if (abBlockAttr[i] & PURGABLE)		fprintf(fp,"P");  else fprintf(fp,"-");
		}
		i = aiNextBlk[i];

		if (i2 != fERROR)
		{
			fprintf(fp," | %03u %07lx %06lx %04x %03u ",i2, apBlocks[i2], GetBlockSize(i2), CheckSum(i2), (abBlockAttr[i]&CLASSMASK)>>3 );
			type = abBlockAttr[i2] & BLKTYPEMASK;
			if (type != ENDBLK && aiPrevBlk[aiNextBlk[i2]] != i2)
				fprintf(fp,"*********");
			else
			{
				if (type == UNUSED)					fprintf(fp,"UNUSD");
				else if (type == FREEMEM)			fprintf(fp,"FREE ");
				else if (type == BLKINUSE)			fprintf(fp,"INUSE");
				else if (type == BLKTYPE3)			fprintf(fp,"TYPE3");
				else if (type == BLKTYPE4)			fprintf(fp,"TYPE4");
				else if (type == STARTBLK)			fprintf(fp,"START");
				else if (type == ENDBLK)			fprintf(fp,"END  ");
				else							  	fprintf(fp,"ERROR");
				
				if (abBlockAttr[i2] & LOCKED)		fprintf(fp," L"); else fprintf(fp," -");
				if (abBlockAttr[i2] & RESOURCE)		fprintf(fp,"R");  else fprintf(fp,"-");
				if (abBlockAttr[i2] & PURGABLE)		fprintf(fp,"P");  else fprintf(fp,"-");
			}
			i2 = aiNextBlk[i2];
		}

		fprintf(fp,"\n");
	}
	
	fclose(fp);

#endif
}

/* ========================================================================
   Function    - BLKPTR
   Description - Get the block pointer, with memory checking.
   Returns     - void * (You must cast to use.)
   ======================================================================== */

void * BLKPTR(LONG i)
{
	if (i < 0)
	{
		return (void *) i;
	}
	
#if RES_MANAGER
	if (IsResourceHandle(i))
	{
		// Here is where we'll get the resource if we can.
		i = Query_iResBlock(i);	// Convert a resource handle to a memory handle.
		if ( i <= 0)
			return (void *)i;
		
		if ( i > cNumBlkHeads)
			return (void *) fERROR;
	
		abBlockAttr[i] |= MOLDYBIT;
	}
#endif

	if (memoryBlocks.find(i) != memoryBlocks.end())
	{
		return memoryBlocks[i];
	}
	else
	{
		return new ULONG[3072];
		return (void*)((ULONG*)apBlocks[i]);
	}
}

/* ========================================================================
   Function    - BLKPTR2
   Description - Get the block pointer (version of 2022).
   Returns     - void * (You must cast to use.)
   ======================================================================== */

void* BLKPTR2(LONG i, LONG size)
{
	if (memoryBlocks.find(i) != memoryBlocks.end())
	{
		return memoryBlocks[i];
	}
	else
	{
		memoryBlocks[i] = new ULONG[size];
		return (void*)memoryBlocks[i];
	}
}

/* ========================================================================
   Function    - zalloc
   Description - Allocate locked blocks of high memory.
   Returns     - pointer to the memory.
   ======================================================================== */

void *_zalloc(char * FileName, LONG FileLineNo, LONG size)
{				
	SHORT iHandle = _NewBlockHigh(size);
	if (iHandle > 0)
	{
		ValidateBlockID(iHandle);
		SetBlockAttr(iHandle,LOCKED|CLASSMASK,LOCKED|CLASS1);
		SetDebugInfo( FileName, FileLineNo, iHandle);
	}
	else
	{
		return NULL;
	}
	return BLKPTR(iHandle);
}

/* ========================================================================
   Function    - zone_alloc
   Description -
   Returns     - pointer to the memory.
   ======================================================================== */

void *_zone_alloc(LONG size)
{		
	SHORT iHandle = _NewBlockHigh(size);
	if (iHandle > 0)
	{
		ValidateBlockID(iHandle);
		SetBlockAttr(iHandle,LOCKED|CLASSMASK,LOCKED|CLASS1);
	}
	else
	{
		return NULL;
	}
	
	return BLKPTR(iHandle);
}


#ifdef _WINDOWS
/* ========================================================================
   Function    - WinCheckMem
   Description - find the max abount of free mem in a window05 system
   Returns     - size of free mem availible
   ======================================================================== */
ULONG WinCheckMem(void)
{
	MEMORYSTATUS	memStats;
	ULONG			MaxFree;

	memset( &memStats, sizeof(MEMORYSTATUS), 0 );
	memStats.dwLength = sizeof(MEMORYSTATUS);
	
	// ask about memory
	GlobalMemoryStatus(&memStats) ;
	
	MaxFree = (ULONG)(memStats.dwTotalPhys&0xFFFFFFF0);
	
	// on a >32Meg, try to run hires
	if(MaxFree > (34*1024*1024) )
	{
		MaxFree = (24*1024*1024);
	}
	else
	// on a 32Meg or better machine, try to run med res
	if(MaxFree > (29*1024*1024) )
	{
		MaxFree = MIN_MEMORY_32MEG_WINDOWS;
	}
	// or else just give them Min amount
	else
	{
		MaxFree = MIN_MEMORY_16MEG_WINDOWS;
	}
	
	return (MaxFree);
}
#endif // _WINDOWS

/* ========================================================================
   Function    - ClearAllMoldyBits
   Description - At the begining of each render call this.
   				 If the last pass had set the Moldy bit, then set the
   				 Lasttime moldy bit flag.
   Returns     -
   ======================================================================== */

#if RES_MANAGER		// Without resources why bother?
void ClearAllMoldyBits()
{
	LONG iHandle;
	// Start at 1 past the head to use the first real block.
	
	for (iHandle = aiNextBlk[START_BLOCK];
		 iHandle >= 0;
		 iHandle = aiNextBlk[iHandle])
	{
		ATTR_BLK_TYPE * const pabBlockAttr = &abBlockAttr[iHandle];
		ULONG	const abAttr = *pabBlockAttr;
		ULONG	const abMaskedAttr = abAttr & BLKTYPEMASK;
		
		// WARNING!!! This next test counts on FREEMEM being < ENDBLK!!!!!!!!
		if (abMaskedAttr > FREEMEM)
		{
			if (abMaskedAttr == ENDBLK)
				break;
			
			if (abAttr & RESOURCE)		// Only resources get marked purgable.
			{
				/* set the moldy last time bit */
				if (abAttr & MOLDYBIT)
				{
					*pabBlockAttr |= MOLDYBITLASTTIME;
					
					*pabBlockAttr &= (0xFFFF ^ MOLDYBIT);
				}
				else
				{
					*pabBlockAttr &= (0xFFFF ^ (MOLDYBITLASTTIME | MOLDYBIT));
				}
				
			}
		}
	}
}
#endif

/* ========================================================================
   Function    - IsBlockMoldy
   Description - Test if the moldy bit was tripped since last call to
   				 ClearAllMoldyBits
   Returns     - TRUE If it has not been touched yet.
                 FALSE otherwise.
   ======================================================================== */

BOOL IsBlockMoldy(SHORT iBlk)
{
	SHORT iHandle;
	
	if (iBlk < 0)
		return FALSE;
		
#if RES_MANAGER
	if (IsResourceHandle(iBlk))
	{
	#if fUSE_RES_FILES
		SHORT iResBlk = GetResourceHandleBlk(iBlk);
	
		if (iResBlock[iResBlk] == 0 && giResFileNames[iResBlk] != 0)
		{
			return FALSE;
		}
	#endif
		iHandle = Query_iResBlock(iBlk);	// Convert a resource handle to a memory handle.
	}
	else
#endif
	{
		iHandle = iBlk;
	}
	
	if (CheckValidBlock(iHandle, __LINE__) == fERROR)
		return FALSE;

	// If we touched it this time or last time its not moldy.
	if ((abBlockAttr[iHandle] & MOLDYBIT) || (abBlockAttr[iHandle] & MOLDYBITLASTTIME))
		return FALSE;
	
	return TRUE;
}

/* ========================================================================
   Function    - IsBlockPurgable
   Description - Test if the Purgable bit is set
   Returns     - TRUE | FALSE
   ======================================================================== */

BOOL IsBlockPurgable(SHORT iBlk)
{
	SHORT iHandle;
	BOOL Result = FALSE;
	
	if (iBlk < 0)
		return Result;
		
#if RES_MANAGER
	if (IsResourceHandle(iBlk))
	{
	#if fUSE_RES_FILES
		SHORT iResBlk = GetResourceHandleBlk(iBlk);
	
		if (iResBlock[iResBlk] == 0 && giResFileNames[iResBlk] != 0)
		{
			Result = ((gResFlags[iResBlk] & RM_PURGABLE) == RM_PURGABLE);
			return Result;
		}
	#endif
		iHandle = Query_iResBlock(iBlk);	// Convert a resource handle to a memory handle.
	}
	else
#endif
	{
		iHandle = iBlk;
	}
	if (CheckValidBlock(iHandle, __LINE__) == fERROR)
		return Result;

#if defined (MEMORY_CHK)
	ReportBlockError(iHandle,CheckBlockOverwrite(iHandle));
#endif
	
	return ((abBlockAttr[iHandle] & PURGABLE) ? TRUE : FALSE);
}

/* ========================================================================
   Function    - IsBlockMultiUser
   Description - Test if this block is referenced by more than one thing.
   Returns     - TRUE | FALSE
   ======================================================================== */

BOOL IsBlockMultiUser(SHORT iBlk)
{
	BOOL Result = FALSE;
	SHORT iHandle;
	
	if (iBlk < 0)
		return Result;
		
#if RES_MANAGER
	if (IsResourceHandle(iBlk))
	{
	#if fUSE_RES_FILES
		SHORT iResBlk = GetResourceHandleBlk(iHandle);
	
		if (iResBlock[iResBlk] == 0 && giResFileNames[iResBlk] != 0)
		{
			Result = ((gResFlags[iResBlk] & RM_MULTI_USER) == RM_MULTI_USER);
			return Result;
		}
	#endif
		iHandle = Query_iResBlock(iBlk);	// Convert a resource handle to a memory handle.
	}
	else
#endif
	{
		iHandle = iBlk;
	}
	if (CheckValidBlock(iHandle, __LINE__) == fERROR)
		return TRUE;	// Return TRUE so the resource manager won't try to purge it.

#if defined (MEMORY_CHK)
	ReportBlockError(iHandle,CheckBlockOverwrite(iHandle));
#endif
	
	return ((abBlockAttr[iHandle] & MULTI_USER_BIT) ? TRUE : FALSE);
}

/* ========================================================================
   Function    - IsHandleFlushed
   Description - Test whether the resource handle has been flushed.
   Returns     - TRUE || FALSE
   ======================================================================== */

BOOL IsHandleFlushed(LONG iHandle)
{
	void *ptr = BLKPTR(iHandle);
	
	return ( !IsPointerGood(ptr));
}

/* ========================================================================
   Function    - FindCloseFit
   Description - Given a size, find a block which is the same size or 
   				 smaller.
   Returns     - Handle to that block.
   ======================================================================== */

static LONG FindCloseFit(LONG iHandleStart, ULONG cBytes, ULONG *BestBlockSize)
{
	LONG iHandle;
	LONG iBestHandle = fERROR;
	ULONG BlkBits;
	
	*BestBlockSize = 0;
	
	// Start at the end of memory and work backward until we reach our
	// starting location.
	for (iHandle = aiPrevBlk[END_BLOCK], BlkBits = abBlockAttr[iHandle];
		 (BlkBits & BLKTYPEMASK) != STARTBLK
		 && iHandleStart != iHandle;
		 iHandle = aiPrevBlk[iHandle], BlkBits = abBlockAttr[iHandle])
	{
		if ( TEST_BLK_INUSE(BlkBits)
			 && !(BlkBits & LOCKED)		// Can't move locked blocks
			 && !(BlkBits & HIGHMEMORY)	// Don't move the high memory into low memory space.
			)
		{
			LONG const BlkSize = GetBlockSize(iHandle);
			if (BlkSize <= cBytes
			    && BlkSize >= *BestBlockSize)
			{
				iBestHandle = iHandle;
				*BestBlockSize = BlkSize;
			}
		}
	}
	return iBestHandle;
}

/* ========================================================================
   Function    - FillFreeBlock
   Description - Given a handle to a free block, copy into its space as many
   				 good blocks from memory higher than this blocks as will fit.
   				 This fn is used to get moveable memory blocks around
   				 This is recursive, without doing a recursive fn call.
   Returns     - 
   ======================================================================== */

static ULONG	FillFreeBlock(LONG iHandle)
{
	ULONG BlkSize;
	
	while (1)
	{
		LONG iNextHandle = aiNextBlk[iHandle];
		BlkSize = GetBlockSize(iHandle);
		
		if ((abBlockAttr[iNextHandle] & BLKTYPEMASK) != ENDBLK)
		{
			ULONG BestBlockSize;	// Initialized by FindCloseFit
			
			LONG iFillerHandle = FindCloseFit(iHandle, BlkSize, &BestBlockSize);
			if (iFillerHandle != fERROR)
			{
				
				// Copy the good block into the free space.
				
				ULONG const Src = apBlocks[iFillerHandle];
				ULONG const Dest = apBlocks[iHandle];
				LONG  const iNext = aiNextBlk[iHandle];
				LONG  const iPrev = aiPrevBlk[iHandle];
				LONG  const iFillNext = aiNextBlk[iFillerHandle];
				LONG  const iFillPrev = aiPrevBlk[iFillerHandle];
				
				memmove((PTR)Dest, (PTR)Src, BestBlockSize);
				
				apBlocks[iHandle] = Src;
				apBlocks[iFillerHandle] = Dest;
				
				// Now relink it up in ascending memory order!
				aiNextBlk[iPrev] = iFillerHandle;
				aiPrevBlk[iNext] = iFillerHandle;
				aiNextBlk[iFillerHandle] = iNext;
				aiPrevBlk[iFillerHandle] = iPrev;
				
				
				aiNextBlk[iFillPrev] = iHandle;
				aiPrevBlk[iFillNext] = iHandle;
				aiNextBlk[iHandle] = iFillNext;
				aiPrevBlk[iHandle] = iFillPrev;
				
				// Having moved a new free block check whether we can 
				// concatinate it.
				
				if ((abBlockAttr[iFillPrev] & BLKTYPEMASK) == FREEMEM)
				{
					MergeBlocks(iFillPrev);
				}
				else if ((abBlockAttr[iFillNext] & BLKTYPEMASK) == FREEMEM)
				{
				   	MergeBlocks(iHandle);
				}
			
				if (BestBlockSize < BlkSize)
				{
					// Get a handle to the free block of what's left of the space
					// and try again.
					iHandle = CleanUpBlock(iFillerHandle, CLEAN_TO_HIGHER, BestBlockSize);
				}
				else
				{
					break;	// We fit exactly, stop processing.
				}
			}
			else
			{
				break;	// No blocks small enough to fit.
			}
		}
		else
		{
			break;	// At the end of the list of memory blocks.
		}
	}
	
	return BlkSize;	// Free space left.
}
/* ======================================================================== */
