/* ========================================================================
   Copyright (c) 1990,1995	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: blood.cpp  -
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "BLOOD.HXX"
#include "strenum.h"   

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

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
#if defined(_CHARED)			// Character Editor only
BLOODLINE_STRENGTH_INFO::BLOODLINE_STRENGTH_DATA BLOODLINE_STRENGTH_INFO::fpBloodlineStrengthInfo[] = {
	{STR_BLOOD_STRENGTH_TAINTED,	{4, 4, 0}},
	{STR_BLOOD_STRENGTH_MINOR,		{5, 6, 0}},
	{STR_BLOOD_STRENGTH_MAJOR,		{8, 6, 0}},
	{STR_BLOOD_STRENGTH_GREAT,		{8, 8, 0}}
};

GAME_STRING BLOODLINE_DERIVATION_INFO::fpBloodlineDerivationInfo[] = {
	STR_BLOOD_DERIV_ANDUIRAS,
	STR_BLOOD_DERIV_REYNIR,
	STR_BLOOD_DERIV_BRENNA,
	STR_BLOOD_DERIV_BASAIA,
	STR_BLOOD_DERIV_MASELA,
	STR_BLOOD_DERIV_VORYNN,
	STR_BLOOD_DERIV_AZRAI
};
#endif

