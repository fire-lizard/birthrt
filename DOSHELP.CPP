/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: DOSHelp.cpp
   Author:   Donald Tsang

   ========================================================================
   Contains the following general functions:

   ®RM250¯======================================================================= */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "SYSTEM.H"
#include <stdio.h>
#include "TYPEDEFS.H"
#include <sys\types.h>
#include <direct.h>

#include "GAME.H"
#include "MENU.H"
#include "strenum.h"
#include "DOSHELP.HXX"

/* ------------------------------------------------------------------------
   Defines && Macros
   ------------------------------------------------------------------------ */
#define MAX_HELP_PAGES 1024

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
static LONG		file_offset[MAX_HELP_PAGES];
static UCHAR	last_help_font[MAX_HELP_PAGES];
static SHORT	page_num;
static FILE		*help_fp;
static char		help_fn[40];
static char		help_buf[2048];
static char		help_header[40];

/* ------------------------------------------------------------------------
   Extern Variables
   ------------------------------------------------------------------------ */

extern LONG last_print_offset;		// how far the gprint_text got
extern LONG last_font_used;			// the font when it quit
extern LONG desired_context_number;	// set this; becomes zero when found

/* ------------------------------------------------------------------------
   Internal Prototypes
   ------------------------------------------------------------------------ */
void HelpGetPage();
/* ------------------------------------------------------------------------
   Extern Prototypes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Code
   ------------------------------------------------------------------------ */

void InitDOSHelp (HELP_TOPIC which, LONG where)
{
	switch(which)
	{
		case H_TOPIC_RULES:
			sprintf(help_fn, "%srules.txt",InstallPath);
			break;

		case H_TOPIC_QUICK_KEYS:
			sprintf(help_fn, "%sqkeyshlp.txt",InstallPath);
			break;
			
		
		case H_TOPIC_ATLAS:
		default:
			sprintf(help_fn, "%sdoshelp.txt",InstallPath);
			break;
	}
	
	help_fp = FileOpen(help_fn, "rb");
	if (help_fp == NULL)
	{
		return;
	}
	
	for (LONG page=0; page < MAX_HELP_PAGES; page++)
	{
		file_offset[page] = -1;  // unset
		last_help_font[page] = -1;
	}
	
	desired_context_number = where;
	page_num = -1;  // because we increment first thing
	
	do
	{
		page_num++;
		HelpGetPage();
		run_timers();
	} while (desired_context_number && strlen(help_buf) > strlen(help_header));
	
	if (desired_context_number)  // didn't find it?
	{
		page_num = 0;
		HelpGetPage();
		run_timers();
	}
	
	ShowMenu(D_DOSHELP);
}


void HelpGetPage()
{
	long temp; //used in checking for eof
	LONG menu_w, menu_h, start_font = 2;
	char tmpbuf[2048];	//to compensate for gtext_height's sillyness.
	
		
	GetButtonSize (D_DOSHELP, 1, &menu_w, &menu_h );
	if (page_num > 0)
		start_font = last_help_font[page_num-1];
	
	sprintf(help_header, "^W%03d^H%03d^F%02d", menu_w, menu_h, start_font);
	strcpy(help_buf, help_header);
	char *bufptr = help_buf + strlen(help_buf);
	
	if (file_offset[page_num] == -1)		// going forward from known page
	{
		if (page_num == 0)
			file_offset[page_num] = 0;
		else
			file_offset[page_num] = file_offset[page_num - 1] 
				+ last_print_offset - strlen(help_header);
	}
	
	run_timers();
	if (help_fp == NULL)
	{
		help_fp = FileOpen(help_fn, "rb");
	}
		
	fseek(help_fp, file_offset[page_num], SEEK_SET);
	
	LONG len = fread(bufptr, sizeof(char), 2000, help_fp);
	
	if (len <= 0)
	{
		file_offset[page_num] = -2;
		*bufptr = '\0';
	}
	else
	{
		*(bufptr+len) = '\0';
		
		memcpy(tmpbuf, help_buf, len+strlen(help_header));
		gtext_height(tmpbuf);
		last_help_font[page_num] = last_font_used;
	}
	



	//see if the next page is valid....

	fseek(help_fp,file_offset[page_num] + last_print_offset - strlen(help_header), SEEK_SET);
	
	fread(&temp,1,1,help_fp);
	
	if (feof(help_fp))
		file_offset[page_num+1] = -2;

	
	FileClose(help_fp);
	help_fp = NULL;
}


void PaintDOSHelp (LONG MenuCombo, LONG)
{
	LONG MenuId, ButtonId;
	LONG menu_x, menu_y, menu_w, menu_h;
	LONG x, y, w, h;
	char buffer[2048];
	SHORT button_index;
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	if( GetButtonPosition( MenuId, 1, &menu_x, &menu_y ) == fERROR)
		return;
	
	GetButtonSize ( MenuId, 1, &menu_w, &menu_h );
	
	init_gfont(FONT_SANS_8PT);
	strcpy(buffer, help_buf);
	gprint_text(menu_x, menu_y, buffer, BLACK);
		
	init_gfont(FONT_SANS_12PT);

	for (button_index = 2; button_index <= 4; button_index++)
	{
		GetButtonPosition(MenuId, button_index, &x, &y);
		GetButtonSize(MenuId, button_index, &w, &h);
		
		x += w/2;
		y += h/2 + 3;
		
		switch(button_index)
		{
			case 2:
				strcpy(buffer, STRMGR_GetStr(STR_HELP1_DONE));
				if (GetButtonFlags(MenuId, button_index) & D_HILIGHTED)
					print_text_centered(x-1,y-1,buffer,WHITE);
				else
					print_text_centered(x,y,buffer,173);
				break;
				
			case 3:
				strcpy(buffer, STRMGR_GetStr(STR_HELP1_PREV));
				if (page_num > 0)
				{
					if (GetButtonFlags(MenuId, button_index) & D_HILIGHTED)
						print_text_centered(x-1,y-1,buffer,WHITE);
					else
						print_text_centered(x,y,buffer,173);
				}
				else
					print_text_centered(x,y,buffer,GREY);
				break;
			case 4:
				strcpy(buffer, STRMGR_GetStr(STR_HELP1_NEXT));
				
				if (file_offset[page_num+1] != -2)  // end of file
				{
					if (GetButtonFlags(MenuId, button_index) & D_HILIGHTED)
						print_text_centered(x-1,y-1,buffer,WHITE);
					else
						print_text_centered(x,y,buffer,173);
				}
				else
					print_text_centered(x,y,buffer,GREY);
				
				break;
			default:
				break;
		}
	}
}

void DOSHelpControl (LONG MenuCombo, LONG arg)
{
	LONG MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	switch(arg)
	{
		case 0:
		
			FileClose(help_fp);
			HideMenu(MenuId);
			RunMenus();
			break;
			
		case -1:		/* back a page */
			if (page_num > 0)
			{
				page_num--;
				HelpGetPage();
			}
			break;
			
		case 1:
			if (file_offset[page_num+1] != -2)  // end of file
			{
				page_num++;
				HelpGetPage();
			}
			break;
	}
}

LONG DOSHELP_ContextOffset(LONG Context)
{
	LONG offset = 0;
	
	if (Context < 1 || Context > MAX_CONTEXTS)
		return 0;
	
	FILE *context_file = FileOpen(DOSHELP_CONTEXT_FILE, "rb");
	if (context_file == NULL)
	{
#ifdef _DEBUG
		fatal_error("Can't open DOS Help Context File");
#endif	
		return 0;
	}
	
	fseek(context_file, Context * sizeof(LONG), SEEK_SET);
	fread(&offset, 1, sizeof(LONG), context_file);
	FileClose(context_file);
	
	return offset;
}

