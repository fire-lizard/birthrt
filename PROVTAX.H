/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: ProvTax.H - Collection of Tax rates for Provinces
   Author: 	 Gary Powell
   ======================================================================== */
#ifndef _PROVTAX_H
#define _PROVTAX_H

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

SHORT RollProvinceTaxes (
	SHORT sProvinceRating,
	SHORT TaxRate,
	SHORT flag);

SHORT RollGuildTempleTaxes(
	SHORT sProvinceRating,
	SHORT sHoldingLevel,
	SHORT flag);

SHORT AveProvinceTaxes (
	SHORT sProvinceRating,
	SHORT TaxRate);

SHORT AveGuildTempleTaxes(
	SHORT sProvinceRating,
	SHORT sHoldingLevel);

#if defined (__cplusplus)
}
#endif

#endif // _PROVTAX_H

