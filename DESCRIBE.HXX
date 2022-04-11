/* ========================================================================
   Copyright (c) 1990,1997   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: DESCRIBE.HXX
   Author: Donald Tseng
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif


/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
void IncDescribe(LONG, LONG);
void PaintDescribe(LONG, LONG);
void DescribeHistory(void);
void DescribeGame(void);
void DescribeAdventure(void);
void DescribeBattle(void);
void DescribeBtlWelcome(void);
void DescribeBtlReserves(void);
void DescribeBtlMovement(void);
void DescribeBtlArchers(void);
void DescribeBtlEngagement(void);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
