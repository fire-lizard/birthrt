/* ========================================================================
   DPLAYSHIM.H - just enough DirectPlay for the headers to parse.

   WINSYS/PNETMOD.HXX and MULPLAY.HXX declare members and callbacks in
   terms of DirectPlay types. The POSIX build never links a DirectPlay
   implementation (POSIX/MULPLAY_STUB.CPP answers every call with "no
   multiplayer"), but the class declarations still have to compile because
   21 files in the portable core include them.

   Nothing here is ever dereferenced. If a Linux multiplayer transport is
   ever written, it replaces this file rather than extending it.
   ======================================================================== */
#ifndef _DPLAYSHIM_H
#define _DPLAYSHIM_H

#if !defined(_WIN32)

#include "../PLATFORM.H"

typedef ULONG	DPID;

typedef struct _SHIM_GUID
{
	unsigned long	Data1;
	unsigned short	Data2;
	unsigned short	Data3;
	unsigned char	Data4[8];
} GUID;

typedef GUID *	LPGUID;

struct IUnknown;

typedef struct IDirectPlay		*LPDIRECTPLAY;
typedef struct IDirectPlay3A	*LPDIRECTPLAY3A;

typedef struct _DPNAME			*LPDPNAME;
typedef struct _DPSESSIONDESC	*LPDPSESSIONDESC;
typedef struct _DPSESSIONDESC2	*LPDPSESSIONDESC2;
typedef struct _DPCAPS			*LPDPCAPS;

typedef const struct _DPNAME		*LPCDPNAME;
typedef const struct _DPSESSIONDESC2	*LPCDPSESSIONDESC2;

typedef int (*LPDPENUMDPCALLBACKA)(LPGUID, char *, DWORD, DWORD, void *);

#define __stdcall

#endif	/* !_WIN32 */

#endif	/* _DPLAYSHIM_H */
