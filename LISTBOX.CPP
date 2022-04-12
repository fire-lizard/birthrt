#include "LISTBOX.HXX"
#include "SYSTEM.H"
#include "ENGINE.H"

#include <stdio.h>

#include "STRMGR.H"
#include "strenum.h"

extern "C" {
typedef struct {
	CSTRPTR	filename;
	SHORT		size_points;
	SHORT		next_smaller;
	SHORT		lores_index;
} FONT_DATA;

};

extern FONT_DATA font_info[];

void ListBox::Init(SHORT hThis,Rect& box,Rect& scrUp,Rect& scrDn,SHORT LineHeight,SHORT font,SHORT EntryBlock,SHORT LineChars, PFVL Callback)
{
	//ListBox();
	
	DispStart=0;

	Box=box;
	ScrUp=scrUp;
	ScrDn=scrDn;
	
	hStrings=EntryBlock;

	Lines=box.Height()/LineHeight;

	printf("Lines=%li\n",Lines);

	LineH=(UBYTE)LineHeight;
	Font=font;
	Linechars=LineChars;
	
	numStrings=GetDataBlkSize(hStrings)/LineChars;

	for (SHORT i=0;i<Lines;i++)
	{
		add_region(box.TLx(),box.TLy()+i*LineHeight,box.Width(),LineHeight,NO_KEY,
					ListBoxSetHighlight,hThis,i,LISTBOX_REGION_ID,STR_LB_SET_HIGHLIGHT);
	}

	add_region(scrUp.TLx(),scrUp.TLy(),scrUp.Width(),scrUp.Height(),KEY_U,
				 ListBoxScroll,(LONG)hThis,-1,LISTBOX_REGION_ID,STR_LB_SCROLL_UP);

	add_region(scrDn.TLx(),scrDn.TLy(),scrDn.Width(),scrDn.Height(), KEY_D,
				 ListBoxScroll,(LONG)hThis,1,LISTBOX_REGION_ID,STR_LB_SCROLL_DOWN);
	
}


void ListBox::DeInit()
{
	del_region(ListBoxScroll,0);
	del_region(ListBoxSetHighlight,0);
}


SHORT ListBox::Scroll(SHORT delta)
{
	SHORT i=0;

	SHORT Adder=SGN(delta);
	printf("in scroll with delta %li adder=%li\n",delta,Adder);
	
	delta=ABS(delta);

	for (i=0;i<delta;i++)
	{
		printf("LBDS=%li\n",DispStart);
		if (DispStart+Adder<0)
			break;

		else if (DispStart+Lines+Adder>numStrings)
			break;

		DispStart+=Adder;
	}
	return i;
}


const char *ListBox::GetHighlightedLine() const
{
	if (HighlightedLine>=numStrings || HighlightedLine<0)
		return NULL;

	if(hStrings==fERROR)
		return NULL;

	char* pString=(char*)BLKPTR(hStrings);

	return &pString[HighlightedLine*Linechars];
}


void ListBox::SetHighlightedLine(SHORT delta) 
{
	printf("highlight %li\n",delta);

	HighlightedLine=(SHORT)(DispStart+delta);
}


void ListBox::Paint() const
{
	color_rect(Box.TLx(),Box.TLy(),Box.Width(),Box.Height(),1);
	color_rect(ScrUp.TLx(),ScrUp.TLy(),ScrUp.Width(),ScrUp.Height(),1);
	color_rect(ScrDn.TLx(),ScrDn.TLy(),ScrDn.Width(),ScrDn.Height(),1);

	init_gfont(Font);
	char* pStrings=(char*)BLKPTR(hStrings);

	SHORT padding=(LineH-font_info[Font].size_points)/2;
	
	SHORT LineX,LineY;

	for (LONG i=DispStart;i<DispStart+Lines;i++)
	{
		UBYTE color=173;
		if (i==HighlightedLine)
			color=31;
		LineX=Box.TLx();
		LineY=(i-DispStart)*LineH+padding;
		gprint_text(LineX,LineY,&pStrings[i*Linechars],color);
	}
}


void ListBoxScroll(LONG hlb,LONG delta)
{
	SHORT hListBox=(SHORT)hlb&0xFFFF;
	if(hListBox==fERROR)
	{
		printf("bad hlb in scroll \n");
		return;
	}

	ListBox* pLB=(ListBox*)BLKPTR(hListBox);

	pLB->Scroll((SHORT)(delta&0xFFFF));
}


void ListBoxSetHighlight(LONG hlb,LONG LfromTop)
{
	SHORT hListBox=(SHORT)hlb&0xFFFF;
	ListBox* pLB=(ListBox*)BLKPTR(hListBox);
	
	pLB->SetHighlightedLine(LfromTop);
}


const char* ListBoxGetHighlight(LONG hlb,LONG)
{
	SHORT hListBox=(SHORT)hlb&0xFFFF;
	ListBox* pLB=(ListBox*)BLKPTR(hListBox);
	
	return pLB->GetHighlightedLine();
}


void ListBoxPaint(LONG hlb,LONG)
{
	SHORT hListBox=(SHORT)hlb&0xFFFF;
	ListBox* pLB=(ListBox*)BLKPTR(hListBox);

	pLB->Paint();
	
}


SHORT NewListBox(Rect& box,Rect& scrUp,Rect& scrDn,SHORT EntryBlock,SHORT LineHeight,SHORT font,SHORT LineChars, PFVL Callback)
{
	SHORT hLB=NewBlock(sizeof(ListBox));
	
	ListBox* pLB=(ListBox*)BLKPTR(hLB);
	
	if(hLB==fERROR)
		return fERROR;

	pLB->Init(hLB,box,scrUp,scrDn,EntryBlock,LineHeight,font,LineChars,Callback);
	
	return hLB;

}

