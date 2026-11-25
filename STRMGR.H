/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _STRMGR_H
#define _STRMGR_H

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "TYPEDEFS.H"
#endif
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

void STRMGR_InitStrMgr(void);
CSTRPTR STRMGR_GetStr(LONG);
char * STRMGR_Upper(char *);	// strupr for displayed game text
void STRMGR_LoadExeText(void);
CSTRPTR STRMGR_ExeText(CSTRPTR);	// text a translation wrote into its exe
SHORT LoadSTR (
	CSTRPTR szFileName,
	BOOL /* fSetPal */,
	BOOL /* fLockRes */,
	BOOL /* fRotated */,
	LONG /* iResFileSlot */);

#if defined (__cplusplus)
}
#endif
	
#endif // _STRMGR_H
