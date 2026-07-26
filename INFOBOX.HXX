#ifndef INFOBOX_HXX
#define INFOBOX_HXX

#if !defined(_TYPEDEFS_H)
#include "TYPEDEFS.H"
#endif

#if !defined(_STRENUM_H)
#include "strenum.h"
#endif

#define INFOBOX_BUT_ALL 242
#define INFOBOX_BUT_REG 666

class InfoBox
{
	private:
		static GAME_STRING 	Info;
		static BOOL			fShowCalled;

	public:
		static void Paint(LONG,LONG);
		static void Show(GAME_STRING);
		static void LLHide(LONG,LONG){Hide();}
		static void Hide();

};

#endif
