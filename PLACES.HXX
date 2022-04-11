/* ========================================================================
   Copyright (c) 1990,1997	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _PLACES_HXX
#define _PLACES_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_VECTOR_HXX)
#include "vector.hxx"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define MAX_PLACES		1000					// cause alan said so
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
//----------------------------------
//	Places within a province
//----------------------------------

typedef struct {
	SHORT					x, y;						// x,y on the map
	unsigned int		Realm			: 7;
	unsigned int		Level			: 4;
	unsigned int		Icon			: 7;
	unsigned int		iRoute1		: 10;
	unsigned int		iRoute2		: 10;
	unsigned int		iRoute3		: 10;
	unsigned int		Besieged		: 1;
	unsigned int		Contested	: 1;
	unsigned int		fSiteDisplayed	: 1;
	unsigned int		fRelicFound	: 1;
	unsigned int		iName			: 8;
	SHORT				NextPlace;
	CSTRPTR				scene;
	SHORT				iIconBitm;
} PLACE_TYPE, *PLACE_PTR;

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Class Definition
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

extern DECL_VECTOR_CLASS(PLACE_TYPE,places);
extern DECL_VECTOR_CLASS(CSTRPTR,place_names);
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
LONG GetCastleName( SHORT PlacesIndex );
void SetRelicFound( SHORT NameIndex );
BOOL IsRelicFound( SHORT NameIndex );
LONG HoldingValue (LONG iHold);

/* ------------------------------------------------------------------------
   Inline functions   
   ------------------------------------------------------------------------ */
   
#endif // _PLACES_HXX
