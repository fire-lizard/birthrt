/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: margin.cpp  -
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>

#include "engine.h"
#include "system.h"
#include "machine.h"

#include "panel.h"
#include "game.h"
#include "margin.hxx"
/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

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

/* ------------------------------------------------------------------------
   Local Variables
   ------------------------------------------------------------------------ */

// static variables for the menu margin control
LONG MARGIN_CONTROL::fReductionCount = 0;

/* ========================================================================
   Function    - 
   Description - 
   Returns     - 
   ======================================================================== */
void MARGIN_CONTROL::mfIncreaseMargin()
{
	LONG CurrMarginRight;
	LONG CurrMarginLeft;
	LONG CurrMarginTop;
	LONG CurrMarginBottom;
	
	get_margin_size(&CurrMarginRight,
	                &CurrMarginLeft,
	                &CurrMarginTop,
	                &CurrMarginBottom);
	                
	if(fChatLineOn)
	{
		CurrMarginTop += MARGIN_INCREMENT;
		if (CurrMarginTop < CHAT_DISPLAY_HEIGHT)
			CurrMarginTop = CHAT_DISPLAY_HEIGHT;
		set_margin_size(CurrMarginRight + MARGIN_INCREMENT, 
						CurrMarginLeft + MARGIN_INCREMENT, 
						CurrMarginTop, 
						CurrMarginBottom);
	}
	else
	{
		set_margin_size(CurrMarginRight + MARGIN_INCREMENT, 
						CurrMarginLeft + MARGIN_INCREMENT, 
						CurrMarginTop + MARGIN_INCREMENT, 
						CurrMarginBottom);
	}
}
		
/* ========================================================================
   Function    - 
   Description - 
   Returns     - 
   ======================================================================== */
void MARGIN_CONTROL::mfDecreaseMargin()
{
	LONG CurrMarginRight;
	LONG CurrMarginLeft;
	LONG CurrMarginTop;
	LONG CurrMarginBottom;
	
	get_margin_size(&CurrMarginRight,
	                &CurrMarginLeft,
	                &CurrMarginTop,
	                &CurrMarginBottom);
	                
	if(fChatLineOn)
	{
		CurrMarginTop -= MARGIN_INCREMENT;
		if (CurrMarginTop < CHAT_DISPLAY_HEIGHT)
			CurrMarginTop = CHAT_DISPLAY_HEIGHT;
		set_margin_size(CurrMarginRight - MARGIN_INCREMENT,
		                CurrMarginLeft - MARGIN_INCREMENT,
		                CurrMarginTop,
		                CurrMarginBottom);
	}
	else
	{
		set_margin_size(CurrMarginRight - MARGIN_INCREMENT,
		                CurrMarginLeft - MARGIN_INCREMENT,
		                CurrMarginTop - MARGIN_INCREMENT,
		                CurrMarginBottom);
	}
}
		
/* ========================================================================
   Function    - mfReduceScreenSize
   Description - Shrink the screen. (Hot key fn.)
   Returns     - Success or not.
   ======================================================================== */
BOOL const MARGIN_CONTROL::mfReduceScreenSize(LONG, LONG)
{
   	BOOL Result = FALSE;
   	
   	if (fReductionCount < MAX_TIMES_REDUCTION)
	{
		mfIncreaseMargin();
		mfClearBorder();
		fReductionCount++;
		Result = TRUE;
	}
	
	return Result;
}

/* ========================================================================
   Function    - mfIncreaseScreenSize
   Description - Restore the screen to the previous margins. (Hot key fn.)
   Returns     - Success or not.
   ======================================================================== */
BOOL const MARGIN_CONTROL::mfIncreaseScreenSize(LONG, LONG)
{
   	BOOL Result = FALSE;
   	
	if (fReductionCount > 0)
	{
		fReductionCount--;
		mfDecreaseMargin();
		mfClearBorder();
		Result = TRUE;
	}
	
	return Result;
}

/* ========================================================================
   Function    - mfClearBorder
   Description - 
   Returns     - 
   ======================================================================== */
void const MARGIN_CONTROL::mfClearBorder(void)
{
	
	SHORT  const hBitm = GetResourceStd("ui\\FILLPANL.PCX", FALSE);
	
	if (hBitm > 0)
	{
		DrawBitmap(0,0, hBitm, 0, 0, (SHORT)window_width, (SHORT)window_height);
		ScreenCopy(0, 0, 0, (SHORT)window_width, (SHORT)window_height, SC_DEFAULT_RES);
		SetPurge(hBitm);
	}
	
	fUpdatePanels = TRUE;
}

/* ========================================================================
   Function    - mfRestoreCurrentSize
   Description - Shrink the screen to the previous screen state.
   Returns     - 
   ======================================================================== */
void MARGIN_CONTROL::mfRestoreCurrentSize()
{
	LONG i;
	for (i = 0; i < fReductionCount; i++ )
	{
		clear_display();
		mfIncreaseMargin();
	}
	
	fUpdatePanels = TRUE;
}

/* ========================================================================
   Function    - mfResetCurrentSize
   Description - set the screen to the previous stored screen state.
   Returns     - 
   ======================================================================== */
void MARGIN_CONTROL::mfResetCurrentSize()
{
	LONG i;
	LONG CurrMarginRight;
	LONG CurrMarginLeft;
	LONG CurrMarginTop;
	LONG CurrMarginBottom;
	
	LONG RC = fReductionCount;
	
	// get the margins that the user has set
	get_margin_size(&CurrMarginRight,
	                &CurrMarginLeft,
	                &CurrMarginTop,
	                &CurrMarginBottom);
	                
	fReductionCount = 0;
	
	// now reduce it to the previous reduction count
	for (i = 0; i < RC; i++ )
	{
		mfReduceScreenSize(0,0);
	}
	
	fUpdatePanels = TRUE;
}



extern "C"
LONG GetMarginReductionCount(void)
{
	return (LONG)MARGIN_CONTROL::mfGetReductionCount();
}