/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: REQUEST.H
   Author: Greg Hightower
   ========================================================================
   Contains the following general functions:
   
   Contains the following internal functions:
   
   ======================================================================== */
#ifndef _REQUEST_H
#define _REQUEST_H
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

void ResetRequests(void);
void RunRequests(void);
void ShowRequest(LONG Index, LONG value);
void HideRequest(LONG Index);
int  IsRequestUp ( LONG Index );
LONG MyRequestValue ( LONG Index );

#if defined (__cplusplus)
}

#endif
#endif 
