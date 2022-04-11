#ifndef _LISTBOX_H
#define _LISTBOX_H

#if !defined(_SYSTEM_H)
#include "system.h"
#endif


#define LISTBOX_REGION_ID 972



class Rect
{
	private:
		SHORT x,y,w,h;
	public:
		enum CornerType 
		{
			TopLeft		=0x0,
			TopRight	=0x1,
			BotLeft		=0x2,
			BotRight	=0x3,
		};
		
		Rect(){x=y=w=h=0;}
		Rect(SHORT X,SHORT Y,SHORT W,SHORT H):x(X),y(Y),w(W),h(H){}
		POINT Corner(CornerType) const;
		SHORT TLx() const {return (SHORT)x;}
		SHORT TLy() const {return (SHORT)y;}
		SHORT TRx() const {return (SHORT)(x+w);}
		SHORT TRy() const {return (SHORT)y;}
		SHORT BLx() const {return (SHORT)x;}
		SHORT BLy() const {return (SHORT)(y+h);}
		SHORT BRx() const {return (SHORT)(x+w);}
		SHORT BRy() const {return (SHORT)(y+h);}

		SHORT Width() const {return w;}
		SHORT Height() const {return h;}
		LONG Area() const {return w*h;}
		
};



class ListBox
{
	private:
		
		//const SHORT ID;
		//const char * const * Entries;
		
		SHORT hStrings;
		UBYTE DispStart;
		UBYTE HighlightedLine;
		UBYTE Lines;
		UBYTE LineH;
		SHORT Linechars;
		SHORT numStrings;
		SHORT Font;
		
		Rect Box;
		Rect ScrUp;
		Rect ScrDn;

	public:
		ListBox();//SHORT a);
		void Init(SHORT,Rect&,Rect&,Rect&,SHORT,SHORT,SHORT,SHORT,PFVL);
		void DeInit();
		
		SHORT Scroll(SHORT); //returns how many spaces it was able to scroll
		const char* GetHighlightedLine() const;
		void SetHighlightedLine(SHORT);
		void Paint() const;
};


SHORT NewListBox(Rect&,Rect&,Rect&,SHORT,SHORT,SHORT,SHORT,PFVL);
void ListBoxScroll(LONG,LONG);
void ListBoxSetHighlight(LONG,LONG);
void ListBoxPaint(LONG,LONG);







inline POINT Rect::Corner(Rect::CornerType c) const
{
	POINT p; 
	
	p.x=x+w*(c&1); 
	p.y=y+h*(c&2); 
	
	return p;
}

//#define SLB_BUT_LB 1
//#define SLB_BUT_SU 2
//#define SLB_BUT_SD 3

inline ListBox::ListBox()//SHORT a)
:DispStart(0),Lines(0),LineH(0),Linechars(0),numStrings(0),Font(fERROR),HighlightedLine(0),hStrings(fERROR)
{}
#endif