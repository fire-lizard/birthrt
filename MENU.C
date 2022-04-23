/* =======================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: Menu.c
   Author: 	 Greg Hightower
   ========================================================================
   Contains the following general functions:

   Contains the following internal functions:

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#include <stdio.h>
#if _WINDOWS
#include <Windows.h>
#endif
#include "SYSTEM.H"
#include "SYSINT.H"
#include "ENGINE.H"
#include "MACHINE.H"
#include "PLAYER.HXX"
#include "SOUND.HXX"
#include "STRMGR.H"
#include "strenum.h"
#include "GMENUENM.H"
#include "MAIN.HXX"
#include "PANEL.H"

#if _WINDOWS
#include "MENU.H"
extern HWND	hwndApp;
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
typedef struct
{
	POINT st;
	LONG  w;
	LONG  h;
}BACKGROUND;

/* ------------------------------------------------------------------------
	Macros
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
static void PaintMenus(void);
static LONG AddActiveMenu( LONG Index );
static void RemoveActiveMenu( LONG Index );
void SetRedrawMainMapLevel (void);
void ResumeTimeLimit (void);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

// count of active menus
SHORT	sMenusUp = 0;
BOOL	fMenuClosed = FALSE;

// GWP static BOOL gPrevAIAutoRes;
// GWP static LONG gCountAllMenusUp;

BOOL  fZoom[MAX_ACTIVE_MENUS];
BOOL  fSpin[MAX_ACTIVE_MENUS];
BOOL  fScroll[MAX_ACTIVE_MENUS];
BOOL  fRenderState[MAX_ACTIVE_MENUS];
BOOL  fPauseState[MAX_ACTIVE_MENUS];
BOOL  fRemove[MAX_MENUS];
SHORT hDest[MAX_ACTIVE_MENUS];
BOOL  fPause;
BOOL  fRender=TRUE;
POINT fStart[MAX_ACTIVE_MENUS];
POINT zoomStart;
BACKGROUND backGround[MAX_ACTIVE_MENUS];
// list of active menus
static LONG ActiveMenus[MAX_ACTIVE_MENUS] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

SHORT	hSaveBkgnd = fERROR;
SHORT	cntSaveBkgnd = 0;

// pointers to the data to use (provided by the user)
PMENU Menus = 0;
LONG  MaxMenu = 0;

extern BOOL fIsFadedOut;


/* ========================================================================
   Function    - InitMenuSys
   Description - Attatch user menu data to system
   Returns     - void
   ======================================================================== */
void InitMenuSys(PMENU UserMenus, LONG Count)
{
	SHORT i;
	Menus = UserMenus;
	MaxMenu = Count;
	for(i=0; i<MAX_ACTIVE_MENUS; i++)
	{
		fZoom[i]=FALSE;
		hDest[i] = fERROR;
	}
	for(i=0; i<MaxMenu; i++)
	{
		fRemove[i] = FALSE;
	}

}

/* ========================================================================
   Function    - ResetMenus
	Description - clear out old data
	Returns     - 
	======================================================================== */
void ResetMenus(void)
{
	SHORT i;
	for(i=0; i<MAX_ACTIVE_MENUS; i++)
	{
		ActiveMenus[i] = fERROR;
		fZoom[i]=FALSE;
		hDest[i] = fERROR;
	}
	for(i=0; i<MaxMenu; i++)
	{
		fRemove[i] = FALSE;
	}
}

/* ========================================================================
   Function    -
   Description -
   Returns     -
   ======================================================================== */
void ShowSubMenu(LONG unused, LONG index)
{
	ShowMenu(index);
}

/* ========================================================================
   Function    - QuitSystem
   Description - Coverfn to be used by the menus.
   Returns     -
   ======================================================================== */
void QuitSystem(LONG unused, LONG zero)
{
	quit_sys(zero);
}
/* ========================================================================
   Function    -
   Description -
   Returns     -
   ======================================================================== */

void ShowMenu(LONG Index)
{
	LONG i;
	LONG xOff = 0;
	LONG yOff = 0;
	
	
	// simple error test
	if (!MaxMenu)
		return;
	
	// SysHideCursor();

	// add this guy's index to the list of active menu
	// NOTE: remember you can only have MAX_ACTIVE_MENUS of these
	if (fERROR == AddActiveMenu(Index))
	{
		// SysShowCursor();
		return;
	}
	
	// play a zoom sound
	if(Menus[Index].Buttons[0].Flags & D_ZOOM)
		AddSndObj(SND_UI_MENU_ZOOM_IN, 0, VOLUME_NINETY);

	if (!(Menus[Index].Buttons[0].Flags & D_NO_PUSH))
	{
		sMenusUp++;
		push_regions();
		if (!(Menus[Index].Buttons[0].Flags & D_KEEP_CR))
			AddGlobalKeys();
		fMenuClosed = FALSE;
	}	

	// add regions for all the sub-buttons
	// the first button of a menus describes the background
	// and we use its X and Y as the offsets for all following
	// buttons.
	xOff = Menus[Index].Buttons[0].X;
	yOff = Menus[Index].Buttons[0].Y;
	
	for (i = 1; i < Menus[Index].MenuButtonCount; i++ )
	{
		if (Menus[Index].Buttons[i].pfFunction)
		{
			add_region(
				Menus[Index].Buttons[i].X + xOff,
				Menus[Index].Buttons[i].Y + yOff,
				Menus[Index].Buttons[i].W,
				Menus[Index].Buttons[i].H,
				Menus[Index].Buttons[i].Key,
				Menus[Index].Buttons[i].pfFunction,
				BUILD_LONG(Index, Menus[Index].Buttons[i].Id),
				Menus[Index].Buttons[i].Arg,
				Menus[Index].Buttons[i].Id,
				Menus[Index].Buttons[i].idToolTip
				);
		}
	}
	
	//SysShowCursor();
	
}
/* ========================================================================
   Function    -  HideThisMenu
   Description -  Removes the active menu and resets the hot keys.
   Returns     -  void
   ======================================================================== */

void HideSubMenu(LONG unused, LONG Index)
{
	HideMenu(Index);
	RunMenus();
}

/* ========================================================================
   Function    -  HideThisMenu
   Description -  Removes the active menu and resets the hot keys.
   Returns     -  void
   ======================================================================== */

void HideSubMenuWithClick(LONG MenuCombo, LONG index)
{
	LONG	MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	SetButtonHilight(MenuId, ButtonId, TRUE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	SetButtonHilight(MenuId, ButtonId, FALSE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	
	HideMenu(index);
	RunMenus();
	ResumeTimeLimit();		// restart the countdown in timed multiplayer games
}

/* ========================================================================
   Function    -  Hide Menu
   Description -  Removes the active menu and resets the hot keys.
   Returns     -  void
   ======================================================================== */

void HideMenu(LONG Index)
{
	
	// simple error test
	if (!MaxMenu)
		return;
	
	if (!IsMenuActive(Index))
	{
#ifdef DEBUG	
		// if we wish to debug multiple closes, we *could* put a
		// fatal_error() or something here, but we're just going
		// to allow it.
#endif
		return;
	}
	
	// SysHideCursor();
		
	// play a zoom sound
	if(fRemove[Index] != TRUE)
		if(Menus[Index].Buttons[0].Flags & D_ZOOM)
			AddSndObj(SND_UI_MENU_ZOOM_OUT, 0, VOLUME_NINETY);
	
	if(fRemove[Index] != FALSE)
 	{
		RemoveActiveMenu(Index);
		if(fRemove[Index] == TRUE)
		{
			// SysShowCursor();
			return;		// zoom out
		}
	}
	else
	{
		RemoveActiveMenu(Index);		  // set ActiveMenus[Index] to -1
	}
	
	if (!(Menus[Index].Buttons[0].Flags & D_NO_PUSH))
	{
		sMenusUp--;
		
		// GWP HACK CODE TO KEEP FROM GOING NEGATIVE.
		if (sMenusUp < 0)
			sMenusUp = 0;
		fMenuClosed = TRUE;
		pop_regions();
	}
	
	
	//clear any keys pending that we didn't handle
	clear_key_status(0);
	// SysShowCursor();
}

/* ========================================================================
   Function    - AddActiveMenu
   Description -
   Returns     - The index of this menu (In the active list)
   ======================================================================== */

LONG AddActiveMenu( LONG Index )
{
	LONG i;
	LONG amenu;
#if 0
	LONG TempDest;
	LONG TempZoom;
#endif

	for (i=0;i<MAX_ACTIVE_MENUS;i++)
	{
		amenu = ActiveMenus[i];
	
		if (amenu == Index)
		{
			return fERROR;
		}
			
		if (amenu == -1)
		{

			ActiveMenus[i] = Index;
			Menus[Index].Buttons[0].Flags |= D_UPDATE;
			if(Menus[Index].Buttons[0].Flags & D_ZOOM)
			{
				fZoom[i] = TRUE;
				fStart[i].x = zoomStart.x;
				fStart[i].y = zoomStart.y;
			}
			else if(Menus[Index].Buttons[0].Flags & D_SPIN)
			{
				fSpin[i] = TRUE;
				fStart[i].x = zoomStart.x;
				fStart[i].y = zoomStart.y;
			}
			else if(Menus[Index].Buttons[0].Flags & D_SCROLL)
			{
				fScroll[i] = TRUE;
				fStart[i].x = zoomStart.x;
				fStart[i].y = zoomStart.y;
			}
			
			if (Menus[Index].Buttons[0].Flags & D_FREEZE)
			{
				fFreeze = TRUE;
				fRenderState[i] = fRender;
				fRender = FALSE;
				fPauseState[i] = fPause;
				fPause = TRUE;
			}

			break;
		}
//		if( amenu == -1)
//			continue;
//		else
//			Menus[amenu].Buttons[0].Flags |= D_UPDATE;
	}
	
	if (i == MAX_ACTIVE_MENUS)
		return fERROR;
		
	return i;
}

/* ========================================================================
   Function    -  RemoveActiveMenu
   Description -  The "this" menu and remove it from the active menu array
   Returns     -  void
   ======================================================================== */

void RemoveActiveMenu( LONG Index )
{
	LONG i;
	LONG amenu;
	LONG j;
	LONG menuIndex;
	
	for (i=0;i<MAX_ACTIVE_MENUS;i++)
	{
		amenu = ActiveMenus[i];
		if (amenu == Index)
		{
			if((Menus[Index].Buttons[0].Flags & D_ZOOM) && (!fRemove[Index]))
			{
				fZoom[i] = TRUE;
				fRemove[Index] = TRUE;
			}
			else if((Menus[Index].Buttons[0].Flags & D_SPIN) && (!fRemove[Index]))
			{
				fSpin[i] = TRUE;
				fRemove[Index] = TRUE;
			}
			else if((Menus[Index].Buttons[0].Flags & D_SCROLL) && (!fRemove[Index]))
			{
				fScroll[i] = TRUE;
				fRemove[Index] = TRUE;
			}
			else
			{
				ActiveMenus[i] = -1;
				for(j = 0; j<i; j++)
				{
					menuIndex = ActiveMenus[j];
					if( menuIndex == -1)
						continue;
				}
				
				if (((!(Menus[Index].Buttons[0].Flags & D_ZOOM) ||
					(Menus[Index].Buttons[0].Flags & D_ZOOM))) 
					&&
					(Menus[Index].Buttons[0].Flags & D_FREEZE) )
				{
					fFreeze = FALSE;
					fRender = fRenderState[i];

					fPause = fPauseState[i];
				}
			}
			


			break;

		}
	}
}

/* ========================================================================
   Function    - RunMenus
   Description -
   Returns     -  void
   ======================================================================== */

void RunMenus(void)
{
	// simple error test
	if (!MaxMenu)
		return;

	PaintMenus();
}

/* ========================================================================
   Function    - PaintMenus
   Description -
   Returns     - void
   ======================================================================== */

void PaintMenus(void)
{
	LONG	AMenu;
	
	for (AMenu=0;AMenu < MAX_ACTIVE_MENUS;AMenu++)
	{
		// if this slot not used, skip
		if( ActiveMenus[AMenu] == -1)
			continue;

		PaintActiveMenu(AMenu);
	}
}

/* ========================================================================
   Function    - PaintActiveMenu
   Description - paint an menu out of the active menu list
   Returns     - void
   ======================================================================== */
void PaintActiveMenu(LONG AMenu)
{
	LONG	CurMenu;
	SHORT	hBitm, h;
	LONG	i;
	LONG	xOff;
	LONG	yOff;

	CurMenu = ActiveMenus[AMenu];
	xOff = 0;
	yOff = 0;

	if( Menus[CurMenu].Buttons[0].Flags & D_INVISIBLE)
		return;

	SysHideCursor();
	
	for (i = 0; i < Menus[CurMenu].MenuButtonCount; i++ )
	{
		const LONG X = X_RES_ADJ(Menus[CurMenu].Buttons[i].X);
		const LONG Y = Y_RES_ADJ(Menus[CurMenu].Buttons[i].Y);
		const LONG W = X_RES_ADJ(Menus[CurMenu].Buttons[i].W);
		const LONG H = Y_RES_ADJ(Menus[CurMenu].Buttons[i].H);
	
		run_timers();
	
		if(i== 0)
		{
			xOff = 0;
			yOff = 0;
			// if we need to zoom this menu, do so
			if(fZoom[AMenu])
			{
				switch ( Menus[CurMenu].Buttons[i].btType )
				{
				case BUTTON_SHADEBEVEL:
					// zoom out
					if(fRemove[CurMenu])
					{
		  				zoomout_shade_edged_rect(X + xOff, Y + yOff, W, H, Menus[CurMenu].Buttons[i].Color, hDest[AMenu]);
						fZoom[AMenu] = FALSE;
						HideMenu(CurMenu);
						fRemove[CurMenu] = FALSE;
				  		if (hDest[AMenu] != fERROR)
				  		{
					  		CloseBitm(hDest[AMenu]);
					  		hDest[AMenu] = fERROR;
				  		}
						if (Menus[CurMenu].Buttons[0].Flags & D_FREEZE)
						{
							fFreeze = FALSE;
							fRender = fRenderState[AMenu];
							fPause = fPauseState[AMenu];
						}
				  		goto End; // leave, were done
					}
					else // zoom in
					{
	 					hDest[AMenu] = SaveBitmap(X + xOff-1, Y + yOff-1, W+3, H+2);
	 					zoom_shade_edged_rect(X + xOff, Y + yOff, W, H, Menus[CurMenu].Buttons[i].Color, hDest[AMenu]);
						fZoom[AMenu] = FALSE;
					}
	  				break;
						case BUTTON_ENTRY:
				case BUTTON_LISTBOX:
				case BUTTON_COLORBEVEL:
					// zoom out
					if(fRemove[CurMenu])
					{
						zoomout_color_edged_rect(X + xOff, Y + yOff, W, H, Menus[CurMenu].Buttons[i].Color, hDest[AMenu]);
						fZoom[AMenu] = FALSE;
						HideMenu(CurMenu);
						fRemove[CurMenu] = FALSE;
						if (hDest[AMenu] != fERROR)
						{
							CloseBitm(hDest[AMenu]);
							hDest[AMenu] = fERROR;
						}
						if (Menus[CurMenu].Buttons[0].Flags & D_FREEZE)
						{
							fFreeze=FALSE;
							fRender=fRenderState[AMenu];
							fPause =fPauseState[AMenu];
						}
				  		goto End; // leave, were done
					}
					else	// zoom in
					{
	 					hDest[AMenu] = SaveBitmap(X + xOff-1, Y + yOff-1, W+3, H+2);
	 					zoom_color_edged_rect(X + xOff, Y + yOff, W, H, Menus[CurMenu].Buttons[i].Color, hDest[AMenu]);
						fZoom[AMenu] = FALSE;
					}
					break;
				case BUTTON_BITMAP:
					hBitm = GetResourceStd (Menus[CurMenu].Buttons[0].pArt, TRUE);
					if (hBitm != fERROR)
					{
						// zoom out
						if(fRemove[CurMenu])
						{
							h = (hSaveBkgnd == fERROR) ? fERROR : hDest[AMenu];
							if (h != fERROR)
							{
	//							ZoomOutBitmap(X + xOff, Y + yOff, hBitm, 0, 0, W, H,
	//												hDest[AMenu], fStart[AMenu],
	//												backGround[AMenu].st, backGround[AMenu].w, backGround[AMenu].h);
								ZoomOutBitmap(X + xOff, Y + yOff, hBitm, 0, 0, W, H,
													h, fStart[AMenu],
													backGround[AMenu].st, backGround[AMenu].w, backGround[AMenu].h);
							}
							fZoom[AMenu] = FALSE;
							HideMenu(CurMenu);
							fRemove[CurMenu] = FALSE;
							if (hDest[AMenu] != fERROR)
							{
								CloseBitm(hDest[AMenu]);
								hDest[AMenu] = fERROR;
							}
							SetPurge(hBitm);
							hBitm = fERROR;
							fUpdatePanels = TRUE;
							SetRedrawMainMapLevel();

							if (CurMenu != D_INFOBOX)
							{
								cntSaveBkgnd--;
	printf("MENU.C - ZoomOut - cntSaveBkgnd: %d\n",cntSaveBkgnd);
								if (hSaveBkgnd != fERROR && cntSaveBkgnd < 1)
								{
									SysHideCursor();
									DrawBitm(0, 0, hSaveBkgnd, 0, 0, 640, 480);
									SysShowCursor();
									update_screen();
									DisposBlock(hSaveBkgnd);
									hSaveBkgnd = fERROR;
								}
							}

							if (Menus[CurMenu].Buttons[0].Flags & D_FREEZE)
							{
								fFreeze = FALSE;
								fRender = fRenderState[AMenu];
								fPause = fPauseState[AMenu];
							}
					  		goto End; // leave, we're done
						}
						else // zoom in
						{
							if(fStart[AMenu].x > screen_buffer_width)
								fStart[AMenu].x = screen_buffer_width-1;
							else if(fStart[AMenu].x < 0)
								fStart[AMenu].x = 0;

							if(fStart[AMenu].y > screen_buffer_height)
								fStart[AMenu].y = screen_buffer_height-1;
							else if(fStart[AMenu].y < 0)
								fStart[AMenu].y = 0;

							if(X+xOff > fStart[AMenu].x)
							{
								backGround[AMenu].st.x = fStart[AMenu].x;
								backGround[AMenu].w = X+xOff+W-fStart[AMenu].x;
							}
							else
							{
								backGround[AMenu].st.x = X+xOff;
								if(fStart[AMenu].x < (X+xOff+W))
									backGround[AMenu].w = W;
								else
									backGround[AMenu].w = fStart[AMenu].x - X - xOff;
							}
							
							if(Y+yOff > fStart[AMenu].y)
							{
								backGround[AMenu].st.y = fStart[AMenu].y;
								backGround[AMenu].h = Y+yOff+H-fStart[AMenu].y;
							}
							else
							{
								backGround[AMenu].st.y = Y+yOff;
								if(fStart[AMenu].y < (Y+yOff+H))
									backGround[AMenu].h = H;
								else
									backGround[AMenu].h = fStart[AMenu].y - Y - yOff;
							}


							if (CurMenu != D_INFOBOX)
							{
								if (hSaveBkgnd == fERROR)
								{
									hSaveBkgnd = SaveBitmap(0,0,640,480);
									SysHideCursor();
									shade_rect(0,0,640,480,13);
									SysShowCursor();
								}
								cntSaveBkgnd++;
printf("MENU.C - Zoom - cntSaveBkgnd: %d\n",cntSaveBkgnd);
							}

							hDest[AMenu] = SaveBitmap(backGround[AMenu].st.x, backGround[AMenu].st.y, backGround[AMenu].w, backGround[AMenu].h);
		 					ZoomBitmap(X + xOff, Y + yOff, hBitm, 0, 0,
										  W, H, hDest[AMenu], fStart[AMenu],
										  backGround[AMenu].st, backGround[AMenu].w, backGround[AMenu].h);
							fZoom[AMenu] = FALSE;
							SetPurge(hBitm);
							hBitm = fERROR;
						}
					}
					break;
				}
			}


			if(fScroll[AMenu])
			{
				switch ( Menus[CurMenu].Buttons[i].btType )
				{
					case BUTTON_BITMAP:
						hBitm = GetResourceStd (Menus[CurMenu].Buttons[0].pArt, TRUE);
						if (hBitm != fERROR)
						{
							// spin closed
							if(fRemove[CurMenu])
							{
								if (hSaveBkgnd == fERROR)
									h = fERROR;
								else
									h = hDest[AMenu];

								ScrollClosedBitmap(X + xOff, Y + yOff, hBitm, 0, 0, W, H,
													h, fStart[AMenu],
													backGround[AMenu].st, backGround[AMenu].w, backGround[AMenu].h);

								fScroll[AMenu] = FALSE;
								HideMenu(CurMenu);
								fRemove[CurMenu] = FALSE;
								if (hDest[AMenu] != fERROR)
								{
									CloseBitm(hDest[AMenu]);
									hDest[AMenu] = fERROR;
								}
								SetPurge(hBitm);
								hBitm = fERROR;
								fUpdatePanels = TRUE;
								SetRedrawMainMapLevel();

								
//								if (CurMenu != D_INFOBOX)
//								{
									cntSaveBkgnd--;
printf("MENU.C -	 SpinOut - cntSaveBkgnd: %d\n",cntSaveBkgnd);
									if (hSaveBkgnd != fERROR && cntSaveBkgnd < 1)
									{
										SysHideCursor();
										DrawBitm(0, 0, hSaveBkgnd, 0, 0, 640, 480);
										SysShowCursor();
										update_screen();
										DisposBlock(hSaveBkgnd);
										hSaveBkgnd = fERROR;
									}
//								}
					
								if (Menus[CurMenu].Buttons[0].Flags & D_FREEZE)
								{
									fFreeze = FALSE;
									fRender = fRenderState[AMenu];
									fPause = fPauseState[AMenu];
								}
						  		goto End; // leave, we're done
							}
							else // scroll open
							{
								if(fStart[AMenu].x > screen_buffer_width)
									fStart[AMenu].x = screen_buffer_width-1;
								else if(fStart[AMenu].x < 0)
									fStart[AMenu].x = 0;
								
								if(fStart[AMenu].y > screen_buffer_height)
									fStart[AMenu].y = screen_buffer_height-1;
								else if(fStart[AMenu].y < 0)
									fStart[AMenu].y = 0;
								
								if(X+xOff > fStart[AMenu].x)
								{
									backGround[AMenu].st.x = fStart[AMenu].x;
									backGround[AMenu].w = X+xOff+W-fStart[AMenu].x;
								}
								else
								{
									backGround[AMenu].st.x = X+xOff;
									if(fStart[AMenu].x < (X+xOff+W))
										backGround[AMenu].w = W;
									else
										backGround[AMenu].w = fStart[AMenu].x - X - xOff;
								}
								
								if(Y+yOff > fStart[AMenu].y)
								{
									backGround[AMenu].st.y = fStart[AMenu].y;
									backGround[AMenu].h = Y+yOff+H-fStart[AMenu].y;
								}
								else
								{
									backGround[AMenu].st.y = Y+yOff;
									if(fStart[AMenu].y < (Y+yOff+H))
										backGround[AMenu].h = H;
									else
										backGround[AMenu].h = fStart[AMenu].y - Y - yOff;
								}
						
								backGround[AMenu].w++;//have to do this because of the uncertainty in sin.
								backGround[AMenu].h++;



//									if (CurMenu != D_INFOBOX)
//									{
										if (hSaveBkgnd == fERROR)
										{
											hSaveBkgnd = SaveBitmap(0,0,640,480);
											SysHideCursor();
											if (!Menus[CurMenu].Buttons[0].Flags & D_DONT_SHADE)
											{
												shade_rect(0,0,640,480,13);
											}
											SysShowCursor();
										}
										cntSaveBkgnd++;
//									}
					
								hDest[AMenu] = SaveBitmap(backGround[AMenu].st.x, backGround[AMenu].st.y, backGround[AMenu].w, backGround[AMenu].h);
								ScrollOpenBitmap(X + xOff, Y + yOff, hBitm, 0, 0, W, H,
													hDest[AMenu], fStart[AMenu],
													backGround[AMenu].st, backGround[AMenu].w, backGround[AMenu].h);
								fScroll[AMenu] = FALSE;
								SetPurge(hBitm);
								hBitm = fERROR;
							}
						}
						break;
					case BUTTON_SHADEBEVEL:
					case BUTTON_LISTBOX:
					case BUTTON_COLORBEVEL:
					default:
						break;
				}
			}

			if(fSpin[AMenu])
			{
				switch ( Menus[CurMenu].Buttons[i].btType )
				{
					case BUTTON_BITMAP:
						hBitm = GetResourceStd (Menus[CurMenu].Buttons[0].pArt, TRUE);
						if (hBitm != fERROR)
						{
							// spin closed
							if(fRemove[CurMenu])
							{
								if (hSaveBkgnd == fERROR)
									h = fERROR;
								else
									h = hDest[AMenu];

								SpinOutBitmap(X + xOff, Y + yOff, hBitm, 0, 0, W, H,
													h, fStart[AMenu],
													backGround[AMenu].st, backGround[AMenu].w, backGround[AMenu].h);

								fSpin[AMenu] = FALSE;
								HideMenu(CurMenu);
								fRemove[CurMenu] = FALSE;
								if (hDest[AMenu] != fERROR)
								{
									CloseBitm(hDest[AMenu]);
									hDest[AMenu] = fERROR;
								}
								SetPurge(hBitm);
								hBitm = fERROR;
								fUpdatePanels = TRUE;
								SetRedrawMainMapLevel();

								
//								if (CurMenu != D_INFOBOX)
//								{
									cntSaveBkgnd--;
printf("MENU.C -	 SpinOut - cntSaveBkgnd: %d\n",cntSaveBkgnd);
									if (hSaveBkgnd != fERROR && cntSaveBkgnd < 1)
									{
										SysHideCursor();
										DrawBitm(0, 0, hSaveBkgnd, 0, 0, 640, 480);
										SysShowCursor();
										update_screen();
										DisposBlock(hSaveBkgnd);
										hSaveBkgnd = fERROR;
									}
//								}
					
								if (Menus[CurMenu].Buttons[0].Flags & D_FREEZE)
								{
									fFreeze = FALSE;
									fRender = fRenderState[AMenu];
									fPause = fPauseState[AMenu];
								}
						  		goto End; // leave, we're done
							}
							else // spin in
							{
								if(fStart[AMenu].x > screen_buffer_width)
									fStart[AMenu].x = screen_buffer_width-1;
								else if(fStart[AMenu].x < 0)
									fStart[AMenu].x = 0;
								
								if(fStart[AMenu].y > screen_buffer_height)
									fStart[AMenu].y = screen_buffer_height-1;
								else if(fStart[AMenu].y < 0)
									fStart[AMenu].y = 0;
								
								if(X+xOff > fStart[AMenu].x)
								{
									backGround[AMenu].st.x = fStart[AMenu].x;
									backGround[AMenu].w = X+xOff+W-fStart[AMenu].x;
								}
								else
								{
									backGround[AMenu].st.x = X+xOff;
									if(fStart[AMenu].x < (X+xOff+W))
										backGround[AMenu].w = W;
									else
										backGround[AMenu].w = fStart[AMenu].x - X - xOff;
								}
								
								if(Y+yOff > fStart[AMenu].y)
								{
									backGround[AMenu].st.y = fStart[AMenu].y;
									backGround[AMenu].h = Y+yOff+H-fStart[AMenu].y;
								}
								else
								{
									backGround[AMenu].st.y = Y+yOff;
									if(fStart[AMenu].y < (Y+yOff+H))
										backGround[AMenu].h = H;
									else
										backGround[AMenu].h = fStart[AMenu].y - Y - yOff;
								}
						
								backGround[AMenu].w++;//have to do this because of the uncertainty in sin.
								backGround[AMenu].h++;



//									if (CurMenu != D_INFOBOX)
//									{
										if (hSaveBkgnd == fERROR)
										{
											hSaveBkgnd = SaveBitmap(0,0,640,480);
											SysHideCursor();
											if (!Menus[CurMenu].Buttons[0].Flags & D_DONT_SHADE)
											{
												shade_rect(0,0,640,480,13);
											}
											SysShowCursor();
										}
										cntSaveBkgnd++;
//									}
					
								hDest[AMenu] = SaveBitmap(backGround[AMenu].st.x, backGround[AMenu].st.y, backGround[AMenu].w, backGround[AMenu].h);
								SpinInBitmap(X + xOff, Y + yOff, hBitm, 0, 0, W, H,
													hDest[AMenu], fStart[AMenu],
													backGround[AMenu].st, backGround[AMenu].w, backGround[AMenu].h);
								fSpin[AMenu] = FALSE;
								SetPurge(hBitm);
								hBitm = fERROR;
							}
						}
						break;
					case BUTTON_SHADEBEVEL:
					case BUTTON_LISTBOX:
					case BUTTON_COLORBEVEL:
					default:
						break;
				}
			}


		}
		else
		{
  			// the first button of a Menus describes the background
  			// and we use its X and Y as the offsets for all following
  			// buttons.
  			xOff = X_RES_ADJ(Menus[CurMenu].Buttons[0].X);
  				yOff = Y_RES_ADJ(Menus[CurMenu].Buttons[0].Y);
			
		}

		// if this button invisible, don't paint it (simple, eh?)
		if(	Menus[CurMenu].Buttons[i].Flags & D_INVISIBLE )
			break;
			
		if(	Menus[CurMenu].Buttons[i].Flags & D_HILIGHTED )
		{
			if(Menus[CurMenu].Buttons[i].pHilight != 0)
			{
				hBitm = GetResourceStd (Menus[CurMenu].Buttons[i].pHilight, TRUE);
				if (hBitm != fERROR)
				{
					DrawBitm(X + xOff, Y + yOff, hBitm, 0, 0, W, H);
					SetPurge(hBitm);
					hBitm = fERROR;
				}
			}
		}
		else
		{
			switch ( Menus[CurMenu].Buttons[i].btType )
			{
			case BUTTON_SHADEBEVEL:
				shade_edged_rect( X + xOff, Y + yOff, W, H, Menus[CurMenu].Buttons[i].Color );
  				break;

			case BUTTON_ENTRY:
			case BUTTON_LISTBOX:
			case BUTTON_COLORBEVEL:
				color_edged_rect( X + xOff, Y + yOff, W, H, Menus[CurMenu].Buttons[i].Color );
				break;

			case BUTTON_BITMAP:
				hBitm = GetResourceStd (Menus[CurMenu].Buttons[i].pArt, TRUE);
				if (hBitm != fERROR)
				{
					DrawBitm(X + xOff, Y + yOff, hBitm, 0, 0, W, H);
					SetPurge(hBitm);
					hBitm = fERROR;
				}
				break;
			}
		
		}

		// deal with the labels
		if (Menus[CurMenu].Buttons[i].iLabel != -1 &&
			!(Menus[CurMenu].Buttons[i].Flags & D_LABEL_OFF))
		{
			if (Menus[CurMenu].Buttons[i].Flags & D_SANS_5)
				init_gfont(FONT_SANS_5PT);
			else
			if (Menus[CurMenu].Buttons[i].Flags & D_SANS_6)
				init_gfont(FONT_SANS_6PT);
			else
			if (Menus[CurMenu].Buttons[i].Flags & D_SANS_8)
				init_gfont(FONT_SANS_8PT);
			else
			if (Menus[CurMenu].Buttons[i].Flags & D_SANS_16)
				init_gfont(FONT_SANS_16PT);
			else
			if (Menus[CurMenu].Buttons[i].Flags & D_TITL_8)
				init_gfont(FONT_TITL_8PT);
			else
			if (Menus[CurMenu].Buttons[i].Flags & D_TITL_10)
				init_gfont(FONT_TITL_10PT);
			else
			if (Menus[CurMenu].Buttons[i].Flags & D_TITL_16)
				init_gfont(FONT_TITL_16PT);
			else
				init_gfont(FONT_SANS_12PT);
			
			if (Menus[CurMenu].Buttons[i].Flags & D_TEXT_LEFT)
			{
				LONG labelColor;
				// const LONG tOff = Y_RES_ADJ(4);
				
				if(	Menus[CurMenu].Buttons[i].Flags & D_HILIGHTED )
				{
					labelColor = Menus[CurMenu].Buttons[i].HilightLabelColor;
				}
				else
				{
					labelColor = Menus[CurMenu].Buttons[i].LabelColor;
				}
				print_textf(
					X + xOff,
					Y + yOff,
					labelColor,
					STRMGR_GetStr(Menus[CurMenu].Buttons[i].iLabel)
					);
			}
			else
			{
				LONG labelColor;
				
				if(	Menus[CurMenu].Buttons[i].Flags & D_HILIGHTED )
				{
					labelColor = Menus[CurMenu].Buttons[i].HilightLabelColor;
				}
				else
				{
					labelColor = Menus[CurMenu].Buttons[i].LabelColor;
				}
				print_text_centered(
					X + xOff + (W/2),
					Y + yOff + (H/2) + 1,
					STRMGR_GetStr(Menus[CurMenu].Buttons[i].iLabel),
					labelColor
					);
			}
		}

		//GEH Menus[CurMenu].Buttons[i].Flags &= ~D_UPDATE;	
	}
	
	if( Menus[CurMenu].Buttons[0].pfFunction != NULL )
	{
		(*Menus[CurMenu].Buttons[0].pfFunction)(
			BUILD_LONG(CurMenu, Menus[CurMenu].Buttons[0].Id),
			Menus[CurMenu].Buttons[0].Arg
			);
	}
	
	/* if we are rendering, we need to paint ourselves */
	if(fRender)
	{
		LONG	x, y, w, h;						// [d10-27-96 JPC] temporary
		// if this slot not used, skip
		if( ActiveMenus[AMenu] == -1)
			goto End;

		// don't paint if invisible
		if( Menus[ActiveMenus[AMenu]].Buttons[0].Flags & D_INVISIBLE)
			goto End;
				
		// Restore the following line when done with tests:
		// ScreenCopy(0, Menus[CurMenu].Buttons[0].X, Menus[CurMenu].Buttons[0].Y, Menus[CurMenu].Buttons[0].W, Menus[CurMenu].Buttons[0].H,SC_HIGH_RES);
		x = Menus[CurMenu].Buttons[0].X, y = Menus[CurMenu].Buttons[0].Y, w = Menus[CurMenu].Buttons[0].W, h = Menus[CurMenu].Buttons[0].H;
		ScreenCopy(0, x, y, w, h, SC_HIGH_RES);
	}

	#ifndef _WINDOWS
	//if (fIsFadedOut && CurMenu != 5)
	if (fIsFadedOut)
	{
		//printf("FadedOut in MENU - CurMenu:%d\n",CurMenu);
		fIsFadedOut--;
		if (fIsFadedOut==0)
			FadeIn(100);
	}
	#endif

End:
	SysShowCursor();
}

/* ========================================================================
   Function    - SetButtonValue
   Description - Change the value for a given button, or buttonID == -1 all buttons.
   Returns     - error if button id not found
   ======================================================================== */
LONG SetButtonValue ( LONG MenuIndex, LONG ButtonId, LONG Val )
{
	LONG i;
	
	if (ButtonId == -1)
	{
		for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
		{
			Menus[MenuIndex].Buttons[i].Arg = Val;
		}
	}
	else
	{
		for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
		{
			if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
				break;
		}
		// check for error
		if (i == Menus[MenuIndex].MenuButtonCount)
			return fERROR;
			
		Menus[MenuIndex].Buttons[i].Arg = Val;
	}
	
	return fNOERR;
}

/* ========================================================================
   Function    - GetButtonValue
   Description - retrieve the value for a given button
   Returns     - error if button id not found
   ======================================================================== */
LONG GetButtonValue ( LONG MenuIndex, LONG ButtonId )
{
	LONG i;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
			
	return Menus[MenuIndex].Buttons[i].Arg;
}



/* ========================================================================
   Function    - GetButtonProc
   Description - returns the function pointer of that button
   Returns     - NULL if not found or if button has no proc
   ======================================================================== */
PFVLL GetButtonProc ( LONG MenuIndex, LONG ButtonId)
{
	LONG i;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;

	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return NULL;
		
	return Menus[MenuIndex].Buttons[i].pfFunction;
}









/* ========================================================================
   Function    - SetButtonProc
   Description - assign the function and arg as well as the hot key values
   Returns     - error if button id not found
   ======================================================================== */
LONG SetButtonProc ( LONG MenuIndex, LONG ButtonId, PFVLL pFunc, LONG Val, LONG Key )
{
	LONG i;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	Menus[MenuIndex].Buttons[i].pfFunction = pFunc;
	Menus[MenuIndex].Buttons[i].Arg = Val;
	Menus[MenuIndex].Buttons[i].Key = Key;
	
	return fNOERR;
}

/* ========================================================================
   Function    - ChangeButtonProc
   Description - assign the function and arg as well as the hot key values
   Returns     - error if button id not found
   ======================================================================== */
LONG ChangeButtonProc ( LONG MenuIndex, LONG ButtonId, PFVLL pFunc, LONG Val, LONG Key )
{
	LONG i;
	LONG xOff = 0;
	LONG yOff = 0;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	xOff = Menus[MenuIndex].Buttons[0].X;
	yOff = Menus[MenuIndex].Buttons[0].Y;
	
	// Update the regions code first before we lose the old pfFunction.
	change_function(Menus[MenuIndex].Buttons[i].pfFunction,
	                Menus[MenuIndex].Buttons[i].Key,
	                Menus[MenuIndex].Buttons[i].X + xOff,
	                Menus[MenuIndex].Buttons[i].Y + yOff,
	                pFunc,
	                Key,
					BUILD_LONG(MenuIndex, Menus[MenuIndex].Buttons[i].Id),
	                Val,
	                CHANGE_ALL_REGIONS);
	
	Menus[MenuIndex].Buttons[i].pfFunction = pFunc;
	Menus[MenuIndex].Buttons[i].Arg = Val;
	Menus[MenuIndex].Buttons[i].Key = Key;
	
	
	return fNOERR;
}

/* ========================================================================
   Function    - SetButtonLabel
   Description - Change the button label ptr
   Returns     - error if button id not found
   ======================================================================== */
LONG SetButtonLabel ( LONG MenuIndex, LONG ButtonId, int iLabel, LONG Color )
{
	LONG i;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	Menus[MenuIndex].Buttons[i].iLabel = iLabel;
	if (Color != NO_CHANGE)
	{
		Menus[MenuIndex].Buttons[i].LabelColor = Color;
	}
	return fNOERR;
}

/* ========================================================================
   Function    - SetButtonToolTip
   Description - Change the tool tip string manager index. use fERROR
   				 to remove an existing tooltip.
   Returns     - 
   ======================================================================== */
LONG SetButtonToolTip(LONG MenuIndex, LONG ButtonId, LONG NewToolTip)
{
	LONG i;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	Menus[MenuIndex].Buttons[i].idToolTip = NewToolTip;
	
	return fNOERR;
}

/* ========================================================================
   Function    - ChangeButtonToolTip
   Description - Change the tool tip string manager index. use fERROR
   				 to remove an existing tooltip.
   Returns     - 
   ======================================================================== */
LONG ChangeButtonToolTip(LONG MenuIndex, LONG ButtonId, LONG NewToolTip)
{
	LONG i;
	LONG xOff = 0;
	LONG yOff = 0;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	Menus[MenuIndex].Buttons[i].idToolTip = NewToolTip;
	
	xOff = Menus[MenuIndex].Buttons[0].X;
	yOff = Menus[MenuIndex].Buttons[0].Y;
	
	// Now update the regions code.
	change_tooltip(Menus[MenuIndex].Buttons[i].pfFunction,
	              Menus[MenuIndex].Buttons[i].Key,
	              Menus[MenuIndex].Buttons[i].X + xOff,
	              Menus[MenuIndex].Buttons[i].Y + yOff,
	              NewToolTip,
	              CHANGE_ALL_REGIONS);
	return fNOERR;
}

/* ========================================================================
   Function    - SetButtonLabelColor
   Description - Change the button label ptr
   Returns     - error if button id not found
   ======================================================================== */
LONG SetButtonLabelColor ( LONG MenuIndex, LONG ButtonId, LONG Color )
{
	LONG i;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	Menus[MenuIndex].Buttons[i].LabelColor = Color;
	return fNOERR;
}

/* ========================================================================
   Function    - SetButtonPosition
   Description - Change the location of this button
   Returns     - error if button id not found
   ======================================================================== */
LONG SetButtonPosition ( LONG MenuIndex, LONG ButtonId, LONG X, LONG Y )
{
	LONG i;
	LONG width;
	LONG height = 0;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	// if not a full floating dialog, clip to the screen
	if(!(Menus[MenuIndex].Buttons[i].Flags & D_FLOATER))
	{
		// Try to keep it on the screen.
		width = X_RES_ADJ(Menus[MenuIndex].Buttons[i].W);
		if ((X + width) > window_width)
		{
			X = (window_width - width);
			if (X < 0)
			{
				return fERROR;  // GWP It may still blow up if ShowMenu is called.
			}
		}
		
		// height = Y_RES_ADJ(Menus[MenuIndex].Buttons[i].H);
		if ((Y + height) > window_height)
		{
			Y = (window_height - height);
			if (Y < 0)
			{
				return fERROR;  // GWP It may still blow up if ShowMenu is called.
			}
		}
	}
	
	Menus[MenuIndex].Buttons[i].X = X;
	Menus[MenuIndex].Buttons[i].Y = Y;
	return fNOERR;
}

/* ========================================================================
   Function    - SetButtonSize
   Description - Change the w and h of a button
   Returns     - error if button id not found
   ======================================================================== */
LONG SetButtonSize ( LONG MenuIndex, LONG ButtonId, LONG W, LONG H )
{
	LONG i;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	Menus[MenuIndex].Buttons[i].W = W;
	Menus[MenuIndex].Buttons[i].H = H;
	return fNOERR;
}

/* ========================================================================
   Function    - SetButtonArt
   Description - Change the button art ptr
   Returns     - error if button id not found
   ======================================================================== */
LONG SetButtonArt ( LONG MenuIndex, LONG ButtonId, CSTRPTR pArtPath)
{
	LONG i;

	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	Menus[MenuIndex].Buttons[i].pArt = pArtPath;
	
	return fNOERR;
}

/* ========================================================================
   Function    - SetButtonHilightArt
   Description - Change the button hilight ptr
   Returns     - error if button id not found
   ======================================================================== */
LONG SetButtonHilightArt ( LONG MenuIndex, LONG ButtonId, CSTRPTR pArtPath)
{
	LONG i;

	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	Menus[MenuIndex].Buttons[i].pHilight = pArtPath;
	
	return fNOERR;
}

/* ========================================================================
   Function    - SetButtonHilight
   Description - Change the hilight bit in the flags
   Returns     - error if button id not found
   ======================================================================== */
LONG SetButtonHilight ( LONG MenuIndex, LONG ButtonId, BOOL Hilight )
{
	LONG i;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	if(Hilight)
		Menus[MenuIndex].Buttons[i].Flags |= D_HILIGHTED;
	else
		Menus[MenuIndex].Buttons[i].Flags &= ~D_HILIGHTED;
		
	return fNOERR;
}

/* ========================================================================
   Function    - SetButtonHilight
   Description - Change the hilight bit in the flags
   Returns     - error if button id not found
   ======================================================================== */
BOOL IsButtonHilighted ( LONG MenuIndex, LONG ButtonId)
{
	LONG i;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return FALSE;
		
	return Menus[MenuIndex].Buttons[i].Flags &D_HILIGHTED;
}

/* ========================================================================
   Function    - SetUpdate
   Description -
   Returns     -
   ======================================================================== */
LONG  SetUpdate( LONG MenuIndex, LONG ButtonId)
{
	LONG i;
	for(i = 0; i< Menus[MenuIndex].MenuButtonCount; i++)
	{
		if(Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	if(i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;

	Menus[MenuIndex].Buttons[i].Flags |= D_UPDATE;
	return fNOERR;
}
/* ========================================================================
   Function    - CleanUpdate
   Description -
   Returns     -
   ======================================================================== */
LONG CleanUpdate( LONG MenuIndex, LONG ButtonId)
{
	LONG i;
	for(i = 0; i< Menus[MenuIndex].MenuButtonCount; i++)
	{
		if(Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	if(i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;

	Menus[MenuIndex].Buttons[i].Flags &= ~D_UPDATE;
	return fNOERR;
}
	
/* ========================================================================
   Function    - GetButtonPosition
   Description - return the screen location of a button
   Returns     - error if button id not found
   ======================================================================== */
LONG GetButtonPosition ( LONG MenuIndex, LONG ButtonId, LONG *X, LONG *Y )
{
	LONG i;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	if(ButtonId == 0)
	{
		*X = Menus[MenuIndex].Buttons[0].X;
		*Y = Menus[MenuIndex].Buttons[0].Y;
	}
	else
	{
		*X = Menus[MenuIndex].Buttons[0].X + Menus[MenuIndex].Buttons[i].X;
		*Y = Menus[MenuIndex].Buttons[0].Y + Menus[MenuIndex].Buttons[i].Y;
	}
	
	return fNOERR;
}

/* ========================================================================
   Function    - GetButtonSize
   Description - return the width and height of a button
   Returns     - error if button id not found
   ======================================================================== */
LONG GetButtonSize ( LONG MenuIndex, LONG ButtonId, LONG *W, LONG *H )
{
	LONG i;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	*W = Menus[MenuIndex].Buttons[i].W;
	*H = Menus[MenuIndex].Buttons[i].H;
	
	return fNOERR;
}

/* ========================================================================
   Function    - GetButtonFlags
   Description - return the button flags value
   Returns     - error if button id not found
   ======================================================================== */
LONG GetButtonFlags ( LONG MenuIndex, LONG ButtonId )
{
	LONG i;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	return Menus[MenuIndex].Buttons[i].Flags;
}

/* ========================================================================
   Function    - SetButtonFlag
   Description - add a new flag
   Returns     - error if button id not found
   ======================================================================== */
LONG SetButtonFlag ( LONG MenuIndex, LONG ButtonId, LONG Flag )
{
	LONG i;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	// set new flag bit
	Menus[MenuIndex].Buttons[i].Flags |= Flag;
	
	return Menus[MenuIndex].Buttons[i].Flags;
}

/* ========================================================================
   Function    - ClearButtonFlag
   Description - remove a flag
   Returns     - error if button id not found
   ======================================================================== */
LONG ClearButtonFlag ( LONG MenuIndex, LONG ButtonId, LONG Flag )
{
	LONG i;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	// clear flag bit
	Menus[MenuIndex].Buttons[i].Flags &= ~Flag;
	
	return Menus[MenuIndex].Buttons[i].Flags;
}

/* ========================================================================
   Function    - SetButtonType
   Description - add a new Type
   Returns     - error if button id not found
   ======================================================================== */
LONG SetButtonType ( LONG MenuIndex, LONG ButtonId, LONG Type )
{
	LONG i;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	// set new flag bit
	Menus[MenuIndex].Buttons[i].btType = (BUTTON_TYPE)Type;
	
	return Menus[MenuIndex].Buttons[i].btType;
}

/* ========================================================================
   Function    - GetButtonLabel
   Description - return the button label
   Returns     - error if button id not found
   ======================================================================== */
LONG GetButtonLabel(LONG MenuIndex, LONG ButtonId, CSTRPTR *pLabel)
{
	LONG i;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	if(Menus[MenuIndex].Buttons[i].iLabel == -1)
		*pLabel = NULL;
	else
		*pLabel = STRMGR_GetStr(Menus[MenuIndex].Buttons[i].iLabel);

	return fNOERR;
}
/* ========================================================================
   Function    - GetButtonLabelColor
   Description - return the button label
   Returns     - error if button id not found
   ======================================================================== */
LONG GetButtonLabelColor(LONG MenuIndex, LONG ButtonId, LONG *pLabelColor)
{
	LONG i;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	*pLabelColor = Menus[MenuIndex].Buttons[i].LabelColor;

	return fNOERR;
}

/* ========================================================================
   Function    - GetButtonHLabelColor
   Description - return the button label
   Returns     - error if button id not found
   ======================================================================== */
LONG GetButtonHLabelColor(LONG MenuIndex, LONG ButtonId, LONG *pLabelColor)
{
	LONG i;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	*pLabelColor = Menus[MenuIndex].Buttons[i].HilightLabelColor;

	return fNOERR;
}

/* ========================================================================
   Function    - GetButtonArt
   Description - return the button Art
   Returns     - error if button id not found
   ======================================================================== */
LONG GetButtonArt(LONG MenuIndex, LONG ButtonId, CSTRPTR *pArt)
{
	LONG i;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	*pArt = Menus[MenuIndex].Buttons[i].pArt;

	return fNOERR;
}

/* ========================================================================
   Function    - GetButtonArt
   Description - return the button Arg
   Returns     - error if button id not found
   ======================================================================== */
LONG GetButtonArg(LONG MenuIndex, LONG ButtonId)
{
	LONG i;
	
	for (i = 0; i < Menus[MenuIndex].MenuButtonCount; i++)
	{
		if (Menus[MenuIndex].Buttons[i].Id == ButtonId)
			break;
	}
	// check for error
	if (i == Menus[MenuIndex].MenuButtonCount)
		return fERROR;
		
	return Menus[MenuIndex].Buttons[i].Arg;
}

/* ========================================================================
   Function    - IsMenuActive
   Description - is a given menu currently in the Active list
   Returns     - BOOL
   ======================================================================== */
BOOL IsMenuActive(LONG MenuId)
{
	LONG	AMenu;
	
	for (AMenu=0;AMenu < MAX_ACTIVE_MENUS;AMenu++)
	{
		// if this slot not used, skip
		if( ActiveMenus[AMenu] == MenuId)
			return TRUE;
	}
	
	return FALSE;
}

/* ========================================================================
   Function    - DrawMenuSaveBitmap
   Description - draw the current save background at this menu coord
   Returns     - 
   ======================================================================== */
void DrawMenuSaveBitmap(LONG MenuId)
{
	SHORT amenu;
	SHORT i;
	
	// NOTE: this is a really bad way to zoom and backgrounds.
	// We will surely be dumping this in BR2
	
	// find the active menu
	for (i=0;i<MAX_ACTIVE_MENUS;i++)
	{
		amenu = ActiveMenus[MenuId];
	
		if (amenu == MenuId)
		{
			SHORT	X = Menus[MenuId].Buttons[0].X-1;
			SHORT	Y = Menus[MenuId].Buttons[0].Y-1;
			SHORT	W = Menus[MenuId].Buttons[0].W+3;
			SHORT	H = Menus[MenuId].Buttons[0].H+2;
			DrawBitmap(X,Y,hDest[amenu],0,0,W,H);
		}
	}
}

/* ========================================================================
   Function    - DecrementVal
   Description - Generic routine for add_regions
   Returns     - void
   ======================================================================== */
#pragma unreferenced off
void DecrementVal( LONG unused, LONG pVal )
#pragma unreferenced on
{
	LONG *pLocal = (LONG*)pVal;
	pLocal[0]--;
}

/* ========================================================================
   Function    - IncrementVal
   Description - Generic routine for add_regions
   Returns     - void
   ======================================================================== */
#pragma unreferenced off
void IncrementVal( LONG unused, LONG pVal )
#pragma unreferenced on
{
	LONG *pLocal = (LONG*)pVal;
	pLocal[0]++;
}

/* ========================================================================
   Function    - ToggleVal
   Description - Generic routine for add_regions
   Returns     - void
   ======================================================================== */
#pragma unreferenced off
void ToggleVal( LONG unused, LONG pVal )
#pragma unreferenced on
{
	LONG *pLocal = (LONG*)pVal;
	pLocal[0]=!pLocal[0];
}
/* ========================================================================
   Function    - QuitSys
   Description - Generic routine for Quiting the system from the menus.
   Returns     - void
   ======================================================================== */
#pragma unreferenced off
void QuitSys( LONG unused, LONG unused2 )
#pragma unreferenced on
{
	quit_sys(0);
}

/* ======================================================================== */

void ScaleBitmapX (SHORT x, SHORT y, SHORT iBitm, SHORT bx, SHORT by, SHORT w, SHORT h, SHORT scale);

//this function is still very much a Work in Progress.... it's ugly as butt.
void InvToStats(LONG a,LONG b)
{
	LONG i=256;
 	SHORT hInvBitm=fERROR;
 	SHORT hStaBitm=fERROR;
	FILE* temp=fopen("temp.tmp","rt");
	LONG max;
	double multiplier;
	a=b;

	fscanf(temp,"%li %lf",&max,&multiplier);
	fclose(temp);

	//we'll turn these back on later.
	Menus[D_AVATAR_INVENTORY].Buttons[0].Flags &=~D_ZOOM;
	Menus[D_AVATAR_ATTRIBUTES].Buttons[0].Flags &=~D_ZOOM;
	
	HideMenu(D_AVATAR_INVENTORY);
	hInvBitm = GetResourceStd (Menus[D_AVATAR_INVENTORY].Buttons[0].pArt, TRUE);
	hStaBitm = GetResourceStd (Menus[D_AVATAR_ATTRIBUTES].Buttons[0].pArt, TRUE);
	if (hInvBitm!=fERROR && hStaBitm!=fERROR)
	{
		while (i<max)
		{
			clear_screen();

			ScaleBitmapX(0,0,hInvBitm,0,0,1000,1000,i);
			print_textf(320,0,WHITE,"^F11%li (%li,%li)",i,cursor_x,cursor_y);
			update_screen();
			i*=multiplier;
		}

		while (i>256)
		{
			clear_screen();
			
			ScaleBitmapX(0,0,hStaBitm,0,0,1000,1000,i);
			print_textf(320,0,WHITE,"^F11%li (%li,%li)",i,cursor_x,cursor_y);
			update_screen();
			i/=multiplier;
		}
			
			
	}
	ShowMenu(D_AVATAR_ATTRIBUTES);

	Menus[D_AVATAR_INVENTORY].Buttons[0].Flags |=D_ZOOM;
	Menus[D_AVATAR_ATTRIBUTES].Buttons[0].Flags |=D_ZOOM;


}
