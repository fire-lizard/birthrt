#ifndef _HMGR_HXX
#define _HMGR_HXX

#if !defined(_TYEPDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_GMENUENM_H)
#include "gmenuenm.h"
#endif

#define MAX_HIGHLIGHTED_BUTTONS	10

class HighlightMgr
{
	private:
		struct But
		{
			SHORT ID;
			SHORT Duration;
		};

		But HiList[MAX_HIGHLIGHTED_BUTTONS];
		MENU_ID_ENUM MenuId;
	public:
		HighlightMgr(MENU_ID_ENUM /* menuId */ );
		BOOL Highlighted(SHORT) const;
		void HighlightButton(LONG,SHORT);
		void SubdueButton(LONG);
		void Service();
		void Clear();
};



#endif

