/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */

#ifndef _DOSHELP_HXX
#define _DOSHELP_HXX

#if !defined (_TYPEDEFS_H)
#include "typedefs.h"
#endif

/* ------------------------------------------------------------------------
   Defines && Macros
   ------------------------------------------------------------------------ */
#define DOSHELP_CONTEXT_FILE	"doscntxt.dat"
#define MAX_CONTEXTS			322

typedef enum
{
	H_TOPIC_ATLAS=0,
	H_TOPIC_RULES,
	H_TOPIC_QUICK_KEYS
} HELP_TOPIC;

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
void InitDOSHelp (HELP_TOPIC which, LONG where);

LONG DOSHELP_ContextOffset(LONG Context);

#endif // _DOSHELP_HXX
