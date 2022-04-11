#include "system.h"
#include "machine.h"
#include "hmgr.hxx"

HighlightMgr::HighlightMgr(MENU_ID_ENUM a) :
	MenuId(a)
{
#if defined (DEBUG)
	printf ("Constructing the hightlight manager. (%hd)\n", a);
#endif

	for(LONG i=0;i<MAX_HIGHLIGHTED_BUTTONS;i++)
	{
		HiList[i].ID=fERROR;
		HiList[i].Duration=0;
	}
}

BOOL HighlightMgr::Highlighted(SHORT Id) const
{
	if (Id == fERROR)  /* not allowed */
	    return FALSE;
	    
	for (LONG i=0;i<MAX_HIGHLIGHTED_BUTTONS;i++)
	{
		if(HiList[i].ID==Id)
			return TRUE;
	}
	return FALSE;
}
	

void HighlightMgr::HighlightButton(LONG MC,SHORT Dur)
{
	SHORT	Mi = 0;
	SHORT	ID = 0;
	SPLIT_LONG(MC,Mi,ID);
	
//	printf("h mi %li id %li dur %li\n",Mi,ID,Dur);
	

	// only do this if Mi is !0, because it may be a promoted SHORT
	if (Mi && Mi!=MenuId) 
		fatal_error("mismatched menuIDs in %s(%li)",__FILE__,__LINE__);
	
	for (LONG i=0;i<MAX_HIGHLIGHTED_BUTTONS;i++)
	{
		if (HiList[i].ID==fERROR)
		{
			HiList[i].ID=ID;
			HiList[i].Duration=Dur;
			SetButtonHilight(MenuId,ID,TRUE);
			return;
		}
	}
	printf("couldn't highlight button, too many highlighted already\n");
}

void HighlightMgr::SubdueButton(LONG MC)
{
	SHORT	Mi = 0;
	SHORT	ID = 0;
	SPLIT_LONG(MC,Mi,ID);
	
//	printf("s mi id %li %li\n",Mi,ID);

	// only do this if Mi is !0, because it may be a promoted SHORT
	if (Mi && Mi!=MenuId) 
		fatal_error("mismatched menuIDs in %s(%li)",__FILE__,__LINE__);
	
	for (LONG i=0;i<MAX_HIGHLIGHTED_BUTTONS;i++)
	{
		if (HiList[i].ID==ID)
		{	 
			HiList[i].ID=fERROR;
			HiList[i].Duration=0;
			SetButtonHilight(MenuId,ID,FALSE);
			return;
		}
	}
	printf("couldn't subdue button %li, it wasn't present in the list\n",ID); 
}

void HighlightMgr::Service()
{
	for (LONG i=0;i<MAX_HIGHLIGHTED_BUTTONS;i++)
	{
		if (HiList[i].ID!=fERROR && HiList[i].Duration!=-1)
		{
			if (HiList[i].Duration == 0)
			{
					SubdueButton(HiList[i].ID);
					break;
			}
			HiList[i].Duration--;
		}
	}
}


void HighlightMgr::Clear()
{
	for (LONG i=0;i<MAX_HIGHLIGHTED_BUTTONS;i++)
	{
		if (HiList[i].ID!=fERROR)
			SubdueButton(HiList[i].ID);
		HiList[i].ID=fERROR;
		HiList[i].Duration=0;
	}
}
