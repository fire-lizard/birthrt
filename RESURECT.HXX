/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: Resurect.hxx -for the Resurrect spell (and targeted healing)
   Author:   Wes Cumberland (and Donald Tsang)
   ======================================================================== */

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_ITEMTYPE_HXX)
#include "itemtype.hxx"
#endif


#define RESURRECT_BUT_ALL	1
#define RESURRECT_BUT_TITLE	2
#define RESURRECT_BUT_NAME1	3
#define RESURRECT_BUT_NAME2	4
#define RESURRECT_BUT_NAME3	5
#define RESURRECT_BUT_NAME4	6
#define RESURRECT_BUT_Q		7
#define RESURRECT_BUT_CANCL	8

class ResurrectionMenu
{
	private:
		static SHORT hCasterStats;
		static ITEMTYPE UsedItemType;
		static SHORT StatsHdls[4];
		
	public:
		static void Paint(LONG,LONG);
		static void Resurrect(LONG,LONG);
		static void Display(SHORT,ITEMTYPE);
};

class TargetMenu
{
	private:
		static SHORT	CharCount;
		static SHORT	TargetType;
		static LONG		TargetArgs;
		static SHORT	StatsHdls[4];
		
	public:
		static void Paint(LONG,LONG);
		static void Activate(LONG,LONG);
		static void Display(LONG, LONG);
};
