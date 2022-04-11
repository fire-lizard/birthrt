/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: DICE.H - Routines and structure for manipulating dice.
   Author: 	 Gary Powell
   ======================================================================== */
#ifndef _DICE_H
#define _DICE_H


/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYEPDEFS_H)
#include "typedefs.h"
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
typedef struct _DICE {
	SBYTE	sbNumberOfDice;
	SBYTE	sbNumberOfSides;	// On the this type of dice
	SBYTE	sbModifier;
} DICE, *PTR_DICE;
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

SHORT RollDice_( DICE const * const, char const*const, LONG const);
#if defined (_WINDOWS) && defined(_DEBUG)
#define RollDice(a) RollDice_(a,__FILE__,__LINE__)
#else
SHORT RollDice( DICE const * const);
#endif

SHORT AveDice( DICE const * const);

#if defined (__cplusplus)
}
#endif

#endif // _DICE_H
