/* ========================================================================
   Copyright (c) 1990,1995	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: 2DMOUSE.C - Handles the mouse cursor
   Author: 	 Chris Phillips & Wes Cumberland
   ========================================================================
   Contains the following general functions:
   
   mouse_move		-Move the character using the mouse as input.
   handle_2dmouse	-Checks the mouse for buttons, calls appropriate funcs
   
   Contains the following internal functions:
   
   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include "2dmouse.h"
#include "machine.h"
#include "engine.h"

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
	Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

extern LONG mouse_need_seg;

static PFVLLL	pfMouseObjectClicked = NULL;
static PFVLLL	pfMouseFloorClicked = NULL;
static PFVL		pfRightButtonClicked = NULL;
static PFVL		pfLeftButtonClicked = NULL;

/* ========================================================================
   Function    - mouse_move
   Description - Move the character using the mouse as input.
   Returns     - void
   ======================================================================== */

void mouse_move()
{
}

/* ========================================================================
   Function    - SetMouseClicked
   Description - set the mouse click on object routine
   Returns     - Pointer to the previous fn.
   ======================================================================== */
PFVLLL SetMouseClicked (WadThingType type, PFVLLL pfClickFunc )
{
	PFVLLL pfPreviousFunc = NULL;
	
	switch(type)
	{
		case iOBJECT:
			pfPreviousFunc = pfMouseObjectClicked;
			pfMouseObjectClicked = pfClickFunc;
			break;
		case iFLOOR:
			pfPreviousFunc = pfMouseFloorClicked;
			pfMouseFloorClicked = pfClickFunc;
			break;
		case iWALL:
			pfPreviousFunc = NULL;
			break;
	}
	
	return pfPreviousFunc;
}
/* ========================================================================
   Function    - SetLeftButtonClicked
   Description - Set a call back fn to be called whenever the left button is down.
   Returns     - The previous fn.
   ======================================================================== */
PFVL SetLeftButtonClicked(PFVL pfClickFunc )
{
	PFVL pfPreviousFunc = pfLeftButtonClicked;
	
	pfLeftButtonClicked = pfClickFunc;
	
	return pfPreviousFunc;
}

/* ========================================================================
   Function    - SetRightButtonClicked
   Description - Set a call back fn to be called whenever the right button is down.
   Returns     - The previous fn.
   ======================================================================== */
PFVL SetRightButtonClicked(PFVL pfClickFunc )
{
	PFVL pfPreviousFunc = pfRightButtonClicked;
	
	pfRightButtonClicked = pfClickFunc;
	
	return pfPreviousFunc;
}
/* ========================================================================
   Function    - handle_2dmouse 
   Description - checks for button presses and calls appropriate funcs
   Returns     - void
   ======================================================================== */

void handle_2dmouse()
{
	if (mouse_click != 0)
	{
		if (butClicked == 2 &&
		    pfRightButtonClicked != NULL)
		{
			(*pfRightButtonClicked)((LONG)butClicked );
			mouse_click = 0; // event was handled
		}
		else
		if (butClicked == 1 &&
			pfLeftButtonClicked != NULL)
		{
			(*pfLeftButtonClicked)((LONG)butClicked);
			mouse_click = 0; // event was handled
		}
	}
	
	if( objClicked != -1)
	{
		switch(typeClicked)
		{
			case iOBJECT:
				if(pfMouseObjectClicked != NULL)
					(*pfMouseObjectClicked)
						((LONG)butClicked,
						(LONG)objClicked,
						(LONG)typeClicked );
				break;
				
			case iFLOOR:
				if(pfMouseFloorClicked != NULL)
					(*pfMouseFloorClicked)
						((LONG)butClicked,
						(LONG)objClicked,
						(LONG)typeClicked );
				break;
				
			case iWALL:
				break;
		}

		mouse_click = 0; // event was handled
		objClicked = -1;
		typeClicked = (WadThingType) - 1;
	}
}

