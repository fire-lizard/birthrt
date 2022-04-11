#include <stdio.h>

#include "infobox.hxx"

#include "system.h"
#include "engine.h"
#include "gametype.hxx"
#include "gmenuenm.h"
#include "tmsgbox.hxx"

LONG InfoBox::Info=STR_NULL;
//static PFVLLL InfoBox::pfPrevObjMouseCallback  =NULL;
//static PFVLLL InfoBox::pfPrevWallMouseCallback =NULL;
//static PFVLLL InfoBox::pfPrevFloorMouseCallback=NULL;
BOOL InfoBox::fShowCalled=FALSE;

void InfoBox::Show(GAME_STRING str)
{
	Info=str;


#if defined (_WINDOWS)
	if (!fHighRes)
	{
		set_hires (0, 0);
		if (fRender)
			render_view (FALSE);				// update the background screen
	}
#endif

	//POINT PrintPt={0,0};
	//VECTOR PrintDim={0,0};
	//
	//GetButtonPosition(MenuId,INFOBOX_BUT_ALL,&PrintPt.x,&PrintPt.y);
	//GetButtonSize(MenuId,INFOBOX_BUT_ALL,&PrintDim.dx,&PrintDim.dy);

	SetButtonLabel (D_INFOBOX,INFOBOX_BUT_ALL,Info,BLACK);

	ShowMenu(D_INFOBOX);
	
	fAnyKeyChanged=FALSE;

//	pfPrevObjMouseCallback =   SetMouseClicked(iOBJECT,InfoBox::Hide);
//	pfPrevWallMouseCallback =  SetMouseClicked(iWALL,  InfoBox::Hide);
//	pfPrevFloorMouseCallback = SetMouseClicked(iFLOOR, InfoBox::Hide);
	
	if (!fShowCalled)
		add_region(0,0,MAX_VIEW_WIDTH-1,MAX_VIEW_HEIGHT-1,NO_KEY,InfoBox::LLHide,0,0,0,-1);

	fShowCalled=TRUE;

}

void InfoBox::Hide()
{

	if (fShowCalled)
	{
		//SetMouseClicked(iOBJECT,pfPrevObjMouseCallback);
		//SetMouseClicked(iWALL,	pfPrevWallMouseCallback);
		//SetMouseClicked(iFLOOR,	pfPrevFloorMouseCallback);
		//
		//pfPrevObjMouseCallback	=NULL;
		//pfPrevWallMouseCallback	=NULL;
		//pfPrevFloorMouseCallback=NULL;			
		fShowCalled=FALSE;
	}
	
	del_region(InfoBox::LLHide,NO_KEY);
	HideMenu(D_INFOBOX);
	RunMenus();
		
	Info=STR_NULL;
	
	
}

void InfoBox::Paint(LONG MenuCombo,LONG)
{
	if (fAnyKeyChanged)
		InfoBox::Hide();
}
