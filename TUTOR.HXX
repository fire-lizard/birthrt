#ifndef _TUTOR_HXX
#define _TUTOR_HXX

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_STRENUM_H)
#include "strenum.h"
#endif



#define MAX_THELP_STRINGS_PER_ENTRY 9

class TutorialHelpMgr
{
	private:
		static GAME_STRING const STR_NO_HELP;
		static GAME_STRING const TutorText[][MAX_THELP_STRINGS_PER_ENTRY];
		static long const HelpEntries;

		static BOOL InBounds(long const,long const);

		TutorialHelpMgr();	//no instantiation!
	public:
		static BOOL const HelpExists(long const,long const);
		static GAME_STRING const GetHelp(long const,long const);
};


inline BOOL TutorialHelpMgr::InBounds(long const i,long const j)
{
	if (i>=HelpEntries || i<0)
		return FALSE;
	if (j>=MAX_THELP_STRINGS_PER_ENTRY || j<0)
		return FALSE;
	
	return TRUE;
}


#endif

