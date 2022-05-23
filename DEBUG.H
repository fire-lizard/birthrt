//-----------------------------------------------------------------------------
//
//		Copyright 1994/1995 by NW Synergistic Software, Inc.
//
//		Debug.h - Provides debug console routines.
//
//    Revised 5-9-96 by John Conley for more debugging support.
//    Revised 6-18-96 by JPC to support WATCOM/Visual C/_DEBUG/NDEBUG
//   			  permutations.
//------------------------------------------------------------------------------

#ifndef _DEBUG_H
#define _DEBUG_H

#if defined(_WINDOWS)
int  debugf(const char* format, ...);
#endif

#if defined(__cplusplus)
extern "C" {
#endif

// Prototypes.
#if defined(_WINDOWS)
	void InitDebug ( void );
	void SS_Assert(char *, int);
	void SS_printf (const char *format, ... );
	#if defined (_JPC)
		void	WriteDebug (const char *format, ...);
	#endif
#endif
void	ErrorMessage (const char *format, ...);

// Various macros.
#if defined(_WINDOWS)

	#if defined(_STATUS)
		#include "winsys\Status.h"
		#define StatusLine WriteDebug
		#define TRACE SS_printf
	#endif
	
#else                                  // not Windows

	//GEH #include <assert.h>
	//GEH #define ASSERT assert
	#define InitDebug()
	#define debugf	printf

#endif // _WINDOWS


// Use the following definitions for things that haven't been defined above
// (in which case the macros drop out).
//GEH #if !defined(ASSERT)
	//GEH #define ASSERT(__ignore) ((void)0)
//GEH #endif
#if !defined(StatusLine)
	#define StatusLine 1?0:printf
#endif
#if !defined(TRACE)
	#define TRACE 1?0:printf
#endif

#if defined(__cplusplus)
}
#endif

// Debug.h
#endif // _DEBUG_H