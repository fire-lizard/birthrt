/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _RESMANAG_HXX
#define _RESMANAG_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_MEMMANAG_HXX)
#include "memmanag.hxx"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define	fUSE_RES_FILES	1
// Resource flags, (bits) ORed together in gResFlags array.
#define RM_LOCKED					0x01
#define	RM_ROTATED					0x02
#define	RM_NOSCALE					0x04
#define RM_CLASS2					0x08
#define RM_PURGABLE	   				0x10
#define RM_MULTI_USER				0x20


/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
typedef SHORT  (*PFRESLOAD)		(CSTRPTR szResName, BOOL fSetPal, BOOL fLockRes, BOOL fRotated, LONG iResFileSlot);
typedef SHORT  (*PFRESDISPOSE)	(SHORT sResHandle, SHORT sMemHandle);
typedef SHORT  (*PFRESSETPURGE)	(SHORT sResHandle, SHORT sMemHandle);
typedef SHORT  (*PFRESCLRPURGE)	(SHORT sResHandle, SHORT sMemHandle);
typedef ULONG  (*PFRESHASH)		(CSTRPTR szResName);

/* RESTYPE structure */
typedef struct _RESTYPE
{
	char			achExtension[4];	// three letter extension to register
	PFRESLOAD		pfLoadProc;			// Load function
	PFRESDISPOSE	pfDisposeProc;		// Dispose function
	PFRESSETPURGE	pfSetPurgeProc;		// Set Purge function
	PFRESCLRPURGE	pfClrPurgeProc;		// Clear Purge function
	PFRESHASH		pfHashProc;			// Hash the szResName.
} RESTYPE, PRESTYPE;

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
#define GetResourceStd(s,f)	_GetResource_((s),(f),FALSE,FALSE)
#define GetResourceRot(s)	_GetResource_((s),FALSE,FALSE,TRUE)
#define GetResourceRotNoScale(s)	_GetResource_((s),TRUE,FALSE,TRUE)

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

//extern SHORT				*iResBlock;		   		/* resmanag.c */
extern SHORT				iResBlock[];		   		/* resmanag.c */
#if fUSE_RES_FILES
//GWP This is really bad but I don't want to write the access fns.
//    And the memory manager needs access to this data.
//extern UBYTE				*gResFlags;
//extern UBYTE				*giResFileNames;
extern UBYTE				gResFlags[];
extern UBYTE				giResFileNames[];
#endif

//extern UBYTE				*iResExtIndex;		/* index into ResExtentions */
extern UBYTE				iResExtIndex[];		/* index into ResExtentions */
extern RESTYPE				ResExtentions[];

extern LONG					iMaxResSlots;

#if defined (__cplusplus)
}
#endif

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

/* resmanag.c */
LONG GetOpenResFileSlot ();
SHORT OpenResFile_ (CSTRPTR szFileName);
void  ScanResFiles ();
SHORT _GetResource_ (CSTRPTR szResName, BOOL fSetPal, BOOL fLockRes, BOOL fRotated);
SHORT Query_iResBlock (SHORT i);
SHORT PutScreen (SHORT iS, USHORT x, USHORT y, USHORT width, USHORT height, CSTRPTR szFileName);
SHORT RegisterResExtention ( CSTRPTR sExtention,
							   PFRESLOAD LoadProc,
							   PFRESDISPOSE DisposeProc,
							   PFRESSETPURGE SetPurgeProc,
							   PFRESCLRPURGE ClrPurgeProc,
							   PFRESHASH HashProc );
BOOL  DisposeResExtension ( CHAR * szExtension );
SHORT DisposRes (SHORT iResBlk, SHORT iMemBlk);
SHORT ReleaseRes (SHORT iResBlk);
SHORT SetPurgeRes (SHORT iResBlk, SHORT iMemBlk);
SHORT ClrPurgeRes (SHORT iResBlk, SHORT iMemBlk);
ULONG HashCRC(CSTRPTR szResName);
ULONG HashID(CSTRPTR szFileName);
USHORT CheckSum(SHORT i);
LONG ResourceFileAccess (CSTRPTR pszFile);

// Call before initializing the memory manager. Or inside the memory
// manager init fn.
BOOL InitResourceManager(USHORT NumResHandles);
void FreeResourceManager();

#if defined (__cplusplus)
}
#endif
#endif // _RESMANAG_HXX
