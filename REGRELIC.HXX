/* ========================================================================
   Copyright (c) 1997	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: REGRELIC.HXX
   Author:   John Conley
   ========================================================================
   Hand out relics to various lieutenants for the multiplayer game.
   ======================================================================= */

#ifndef _REGRELIC_HXX
#define _REGRELIC_HXX

#if !defined(_TYPEDEFS_HXX)
#include "typedefs.h"
#endif

#if !defined(_REGENTS_HXX)
#include "regents.hxx"
#endif

class REGRELIC_STRUCT {
private:
	class REGRELIC_DATA {
	public:
		ITEMTYPE		fItem;
		USHORT		fType;
		CLASS_ENUM	fClass1;
		CLASS_ENUM	fClass2;
		CLASS_ENUM	fClassExcluded;
		BOOL			fAssigned;
	};
public:
	REGRELIC_DATA fRegRelicData;
private:
	static BOOL fAssignedRelics;
public:
	// Access functions.
	static void ClearRelics ();
	static void AssignRelics ();
	static void SetAssigned ();
private:
	static BOOL ItemAvailable (LONG iItem, LONG iRegent);
};

extern DECL_VECTOR_CLASS (REGRELIC_STRUCT, gRelics);

#endif
