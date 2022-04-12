/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: ADVPREP.C
   Author: Greg Hightower
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#include <stdio.h>
#if defined(_WINDOWS)
#include <Windows.h>
#endif

#include "SYSTEM.H"
#include "ENGINE.H"
#include "MACHINE.H"

#include "ACTNMENU.HXX"
#include "AVATAR.HXX"
#include "CHARSEL.HXX"
#include "GAME.H"
#include "GAMETYPE.HXX"
#include "GAMEMAP.HXX"
#include "ANIM.H"
#include "REALM.HXX"
#include "SCNMGR.HXX"
#include "STRMGR.H"
#include "strenum.h"
#include "MULTIMAP.HXX"
#include "MULTIUI.HXX"
#include "SNDVOX.HXX"
#include "LOADSAVE.HXX"
#include "PLAYSTAT.HXX"
#include "INVNGUI.HXX"
#include "VECTOR.HXX"
#include "REGENTS.HXX"
#include "DESCRIBE.HXX"
#include "ADVPREP.HXX"
#include "PLACES.HXX"
#include "SOUNDFX.HXX"
#include "SOUND.HXX"

#ifdef _WINDOWS
#include ".\WINSYS\MULPLAY.HXX"
#endif

#include "MENU.H"
#include "PANEL.H"
#include "GMENUENM.H"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define START_SITE_LIST_X	20
#define START_SITE_LIST_Y	98
#define START_PARTY_LIST_X	20
#define START_PARTY_LIST_Y	273
#define INC_LIST_Y			15
#define LIST_CLASS_X		150
#define LIST_CHECK_X		160
#define LIST_CHECK_REGION_X	140
#define BA_DIFF_COL			170

#define LIST_SIZE 			MAX_ADV_SITES
#define LISTBOX_SIZE 		 8

#define SITE_SIZE	 		29

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
DEFINE_VECTOR_DATA_S(BOOL,inParty,LIST_SIZE);
DEFINE_VECTOR_CLASS(BOOL,inParty);

DEFINE_VECTOR_DATA_S(SHORT,SiteList,LIST_SIZE);
DEFINE_VECTOR_CLASS(SHORT,SiteList);

DEFINE_VECTOR_DATA_S(SHORT,Adventures,LIST_SIZE);
DEFINE_VECTOR_CLASS(SHORT,Adventures);

SHORT			iAdventureSiteIndex;

static SHORT	iLocalCheck[2];

static SHORT	iSite = 0;

static SHORT	AdvSiteListboxFirst = 0;
static SHORT	AdvSiteListboxCount = 0;
static SHORT	AdvSiteListboxSel = 0;
static SHORT	PartyListboxFirst = 0;
static SHORT	PartyListboxCount = 0;
static SHORT	PartyListboxSel = 0;

static LONG		iPartyLead;

int		screennum ;
int		screenend ;
BOOL	fAdvTut = FALSE;

extern LONG		TutorialActionNumber;
extern LONG		TutorialScreenNumber;
extern BOOL		fPreparingSpells;	// from invngui.cpp
extern BOOL		fFreezeDomainUI;	// from gamemap.cpp
extern BOOL		fPractice;			// from practice.cpp
extern SHORT	localActiveRegent;
extern SHORT	sMenusUp;
extern LONG		adv_dif;		// charsel.cpp
extern LONG 	fControlMode;		// gamemap.cpp
extern LONG		iSiteChosen;

extern BOOL fTutorialSelected;	// from gamemap.cpp

extern LONG		GameSpecificGlobal_type;
extern LONG		GameSpecificGlobal_x;
extern LONG		GameSpecificGlobal_y;
extern LONG		GameSpecificGlobal_w;
extern LONG		GameSpecificGlobal_h;

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
// private
static void TogglePartyCheck(LONG i, LONG);
static void SelectAdventures(void);
static void DescribeAdvPrep(void);
static void AdvPrepIncTut(LONG, LONG);
static void MagicPrep(SHORT AdvIndex);

void BuildAdventureList(void);

// imported
void InitDomainTurn(LONG);
void DataAreaMode (LONG type, LONG dummy);

/* -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   ----------------------------------------------------------------- */

/* ========================================================================
   Function    - AdventurePrep
   Description - build an adventure party and go
   Returns     -
   ======================================================================== */
void AdventurePrep(void)
{
	SHORT	i,x,y,me;
	LONG	MenuCombo;
	
	HideMenu(D_GAMEBUTTON);
	push_regions();
	ShowPanel(D_ADVENTURE_PREP);
	ShowMenu(D_GAMEBUTTON);
	init_pal("nova_l");
	init_shade_table("nova_l");
	
	// set adventure values
	localActiveRegent = ActiveRegent;
	
	// shut down the DomainUI
	fPractice = TRUE;
	fFreezeDomainUI = TRUE;
	sMenusUp = 1;
	fControlMode |= 0x100;
	
	// reset all dialog variables
	AdvSiteListboxFirst = 0;
	AdvSiteListboxSel = 0;
	PartyListboxFirst = 0;
	PartyListboxSel = 0;
	
	memset(&inParty[0], 0, sizeof(BOOL)*LIST_SIZE);
	memset(&SiteList[0], -1, sizeof(SHORT)*LIST_SIZE);
	memset(&Adventures[0], -1, sizeof(SHORT)*LIST_SIZE);
	
	// build list of possible adventurers
	BuildAdventureList();
	
	// just for grins, rebuild the advsite[].iPlaces
	// SetupAdvsitePlaces();
	
	// build site list
	iSite = 0;
	for(int j=0; j<MAX_ADV_SITES; ++j)
	{
		// if availible, add to list
		if (advsite[j].available == SELECTABLE)
		{
			SiteList[iSite++] = j;
		}
	}
	// save count and reset current site
	AdvSiteListboxCount = iSite;
	iSite = 0;

	// clear hilight bar
	for(i=0;i<=LISTBOX_SIZE;++i)
	{
		if (i > LIST_SIZE - 1)
			break;
		SetButtonType(
			D_ADVENTURE_PREP,
			BA_LIST_01 + i,
			BUTTON_REGION);
	}
	
	// SetButtonType(D_ADVENTURE_PREP, BA_LIST_01 + AdvSiteListboxSel, BUTTON_COLORBEVEL);
	// SetButtonType(D_ADVENTURE_PREP, BA_LIST_09 + PartyListboxSel, BUTTON_COLORBEVEL);
		
	// add check regions
	del_region(TogglePartyCheck, 0);
	x = START_PARTY_LIST_X + LIST_CHECK_REGION_X;
	y = START_PARTY_LIST_Y;
	for (i = BA_LIST_09; i <= BA_LIST_16; ++i)
	{
		// checkbox region
		add_region(x, y+2, 30,14,0,TogglePartyCheck,i-BA_LIST_09,0,0, -1);
		y += INC_LIST_Y;
	}
		
	// load checkmark art
	iLocalCheck[0] = GetResourceStd ("UI\\DCHECK1.PCX", FALSE);
	iLocalCheck[1] = GetResourceStd ("UI\\DCHECK2.PCX", FALSE);
		
	// set up the adventure site list
	MenuCombo = BUILD_LONG(D_ADVENTURE_PREP,BA_LIST_01);
	AdvPrepListSel (MenuCombo, 0);
	
	// turn on the first member of the party always
	TogglePartyCheck(0, 0);
	
	screennum = STR_BASIC_042;
	screenend = screennum + 5;
	fAdvTut = TRUE;
}

/* ========================================================================
   Function    - AdvPrepPaint
   Description -
   Returns     -
   ======================================================================== */
void AdvPrepPaint (LONG MenuCombo, LONG )
{
	SHORT	i,j;
	LONG    x,y,w,h;
	FILE	*in;
	SHORT	iBitm;
	CHAR    buffer[20];
	char    textbuf[3000];
	char    buff[80];
	CHAR    color = 228;
	LONG    MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// current site title
	init_gfont(FONT_SANS_16PT);
	
	//print_text_centered(420,30,STRMGR_GetStr(SiteTitle[iSite]), BLACK);
	
	init_gfont(FONT_SANS_8PT);
	// select an adventure site
	x = START_SITE_LIST_X;
	y = START_SITE_LIST_Y;
	
	// add the names of the active adventure sites to the screen
	for(i=AdvSiteListboxFirst; i<AdvSiteListboxFirst+LISTBOX_SIZE; ++i)
	{
		SHORT color;
		if (i == iSite)
			color = WHITE;
		else
			color = BEIGE;
		
		if (SiteList[i] == fERROR)
			continue;
			
		SHORT iPlacename = advsite[SiteList[i]].iPlaces;
		if ( iPlacename != -1 )
		{
			gprint_text(x, y, place_names[iPlacename], color);
		}
		else
		{
			gprint_text(x, y, advsite[SiteList[i]].name, color);
		}
		
		// add the name and region
		switch(advsite[SiteList[i]].difficulty)
		{
		case 1:
			sprintf( buffer, "^l%d%s", BA_DIFF_COL, STRMGR_GetStr(STR_BA_EASY));
			gprint_text(x, y, buffer, color);
			break;
		case 2:
			sprintf( buffer, "^l%d%s", BA_DIFF_COL, STRMGR_GetStr(STR_BA_MED));
			gprint_text(x, y, buffer, color);
			break;
		case 3:
			sprintf( buffer, "^l%d%s", BA_DIFF_COL, STRMGR_GetStr(STR_BA_HARD));
			gprint_text(x, y, buffer, color);
			break;
		case 4:
			sprintf( buffer, "^l%d%s", BA_DIFF_COL, STRMGR_GetStr(STR_BA_VERYHARD));
			gprint_text(x, y, buffer, color);
			break;
		case 0:
		default:
			break;
		}
		y += INC_LIST_Y;
	}
	
	// select party members
	x = START_PARTY_LIST_X;
	y = START_PARTY_LIST_Y;
	
	// add the names of the active party members to the screen
	for(i=PartyListboxFirst; i<PartyListboxFirst+LISTBOX_SIZE; ++i)
	{
		SHORT	color;
		CHAR	sClass0[10], sClass1[10];
		CHAR	sLevel0[10], sLevel1[10];
		SHORT	hPlayerStats;
		SHORT	me;
		
		if (fERROR == Adventures[i])
			continue;
			
		me = regents[Adventures[i]].mfGetid();
		
		if (fERROR == me)
			continue;
			
		if (fERROR == (hPlayerStats = LoadStats(me, fERROR)))
			continue;
			
		DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(hPlayerStats);
		
		// get name
		sprintf(buff, "%s", regents[Adventures[i]].mfGetname());
		
		// get classes
		if (pPlayerStats->mfGetClassType(1) >= 0)
		{
			sprintf(sClass0, "%s", pPlayerStats->mfGetClass(0));
			sprintf(sClass1, "%s", pPlayerStats->mfGetClass(1));
			sprintf(sLevel0, "%d", pPlayerStats->mfGetLevel(0));
			sprintf(sLevel1, "%d", pPlayerStats->mfGetLevel(1));
		
			if(sClass0[0] == 'P')
				sClass0[2] = 0;
			else
				sClass0[1] = 0;
		
			if(sClass1[0] == 'P')
				sClass1[2] = 0;
			else
				sClass1[1] = 0;
		
		sprintf(buffer,"%s%s/%s%s",
		sClass0, sLevel0, sClass1, sLevel1 );
		}
		else if (pPlayerStats->mfGetClassType(0) >= 0)
		{
			sprintf(sClass0, "%s", pPlayerStats->mfGetClass(0));
			sprintf(sLevel0, "%d", pPlayerStats->mfGetLevel(0));
		
			if(sClass0[0] == 'P')
				sClass0[2] = 0;
			else
				sClass0[1] = 0;
		
			sprintf(buffer,"%s%s",
			sClass0, sLevel0);
		}
		else
		{
			sprintf(buffer, "Monster");
		}
			
		sprintf(textbuf, "^F02%s ^F01(%s)", buff, buffer);
		
		if (pPlayerStats->mfGetCurHitPoints()<=0)
		{
			color = GREY;
		}
		else
		{
			if (inParty[i])
			{
				color = WHITE;
			}
			else
			{
				color = YELLOW;
			}
		}
			
		gprint_text(x, y, textbuf, color);
		
		// draw the check mark
		if (pPlayerStats->mfGetCurHitPoints()>0)
		{
			DrawBitmap(x+LIST_CHECK_X, y+2, iLocalCheck[inParty[i]], 0, 0, 20, 20);
		}
		y += INC_LIST_Y;
	}
	
	if (SiteList[iSite] != fERROR)
	{
		// clear the buffer
		memset(&textbuf[0], 0, sizeof(textbuf));
		
		sprintf(buffer,"%sTEXT\\%s.txt", InstallPath, advsite[SiteList[iSite]].name);
		in = FileOpen(buffer, "r");
		if(in == NULL)
		{
			sprintf(buffer,"%sTEXT\\usertext.txt", InstallPath );
			in = FileOpen(buffer, "r");
		}
		
		if(in != NULL)
		{
			fread(textbuf, sizeof(textbuf), 1, in);
			FileClose(in);
			if(strlen(textbuf) < 850)
			{
				textbuf[2] = '0';	// change large font
				textbuf[3] = '3';	// 12 point
			}
			else
			{
				textbuf[6] = '0';	// change small font
				textbuf[7] = '2';	// 8 point
			}
			textbuf[6] = '3';	// change format string in local buffer
			textbuf[7] = '9';
			textbuf[8] = '5';
			
			gprint_text(224, 281, textbuf, DKBROWN);
		}
		
		sprintf(textbuf,"UI\\ADVSCN\\%s.pcx", advsite[SiteList[iSite]].name);
		iBitm = GetResourceStd (textbuf, FALSE);
		if (iBitm != fERROR)
		{
			DrawBitmap (224, 52, iBitm, 0, 0, 400, 220 );
			SetPurge(iBitm);
		}
		else
		{
			sprintf(textbuf,"UI\\ADVSCN\\userpic.pcx");
			iBitm = GetResourceStd (textbuf, FALSE);
			if (iBitm != fERROR)
			{
				DrawBitmap (224, 52, iBitm, 0, 0, 400, 220 );
				SetPurge(iBitm);
			}
		}
	}
	else
	{
		strcpy(textbuf,STRMGR_GetStr(STR_BA_NOSITES));
		gprint_text(224, 281, textbuf, DKBROWN);
	}

	DescribeAdvPrep();
}

/* ========================================================================
   Function    - DescribeAdvPrep
   Description -
   Returns     -
   ======================================================================== */
static void DescribeAdvPrep(void)
{
	del_region(AdvPrepIncTut,0);
	if (fAdvTut && fTutorialSelected)
	{
		SysHideCursor();
		init_gfont(FONT_SANS_8PT);
		print_textf(0, 0, DKBROWN, STRMGR_GetStr(screennum));
		if (GameSpecificGlobal_type == 5 || GameSpecificGlobal_type == 6)
		{
			SHORT x = GameSpecificGlobal_x + (GameSpecificGlobal_w/2) - 26;
			SHORT y = GameSpecificGlobal_y + GameSpecificGlobal_h - 16 - 4;
			SHORT iButton = fERROR;
			iButton = GetResourceStd("UI\\MARBBTNA.PCX", FALSE);
			DrawBitmap(x, y, iButton, 0, 0, 99, 99);
			SetPurge(iButton);
			iButton = fERROR;


			char buf[80];
			if (GameSpecificGlobal_type == 5)
				strcpy(buf, STRMGR_GetStr(STR_GM_TUT_BUTS_MORE));
			else	
				strcpy(buf, STRMGR_GetStr(STR_GM_TUT_BUTS_DONE));
			print_textf(x+26, y+9, RED, buf);
			
			add_region(x,y,52,16,(GameSpecificGlobal_type == 5)?D_KEY_MORE:D_KEY_DONE1,AdvPrepIncTut,0,0,0,STR_NULL);
		}
		SysShowCursor();
	}
}

static void AdvPrepIncTut(LONG, LONG)
{
	++screennum;
	if (screennum >= screenend)
	{
		screennum = screenend;
		fAdvTut = FALSE;
	}
	fUpdatePanels = TRUE;
}

/* ========================================================================
   Function    - AdvPrepDone
   Description -
   Returns     -
   ======================================================================== */
void AdvPrepDone (LONG MenuCombo, LONG )
{
	SHORT 	j,count=0;
	SHORT	HomeIndex, AwayIndex;
	LONG	who, me;	  // loop index, unit# of selection
	LONG	unit;		 // index for unit search loop
	LONG	lastGuy = -1; // lockstep link
	LONG    MenuId, ButtonId;
	LONG	index;
	LONG	iPartyLead;
	BOOL	fSend = FALSE;
	
	#if defined(_WINDOWS)
		fSend = IsMultiPlayer();
	#endif
	
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	SetButtonHilight(D_ADVENTURE_PREP, ButtonId, TRUE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	SetButtonHilight(D_ADVENTURE_PREP, ButtonId, FALSE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	
	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
	if(mouse_button == 2)
	{
		//SystemHelp(STR_BA_HELP_DONE_TITLE, STR_BA_HELP_DONE_TITLE, -1, NULL, 0);
	}
	else
	{
		//++action_turn;		[abc 8/18] removed
		if (fLTAction == DURING_LTACTION)
		{
			ActiveRegent = realm[HomeRealm].mfGetRegent();
		  	fLTAction = AFTER_LTACTION;
		}
		else						// [abc 8/18] added
			++action_turn[HomeRealm];		// [abc 8/18] added
	
	
		// first, clean up the linked lists
		
		for (who=0; who<LIST_SIZE; ++who)
		{
			BOOL did_break = FALSE;
			
			if (Adventures[who] != fERROR)
			{
				// find this persons unit index
				// Note: Lieutentents are also in this array.
				me = regents[Adventures[who]].mfGetunit();
				if (me == -1)  /* invalid list */
					continue;
				
				if (inParty[who])  // he's going; take him out of map
				{
					LONG MyLeader = GetUnitLeader(me);
					MakeUnitSingleStack(me, fSend);
					// GWPif (units[me].Joined)
					// GWP{
					// GWP    for (unit = 1; unit < MAX_UNITS; ++unit)
					// GWP    {
					// GWP       if (units[unit].NextUnit == me)
					// GWP       {
					// GWP	       SetGameData(MP_UNITS, MPUNITS_NEXTUNIT,unit,units[me].NextUnit,fSend);
					// GWP	       did_break = TRUE;
					// GWP	       break;
					// GWP       }
					// GWP    }
					// GWP}
					// GWPelse
					// GWP{
					// GWP    if (units[me].NextUnit > 0)
					// GWP	{
					// GWP		units[units[me].NextUnit].Joined = FALSE;
					// GWP		units[units[me].NextUnit].province = units[me].province;
					// GWP	}
					// GWP}
					// GWPunits[me].NextUnit = -1;
					
					if (lastGuy < 0)
					{
						iPartyLead = me;
						/* make sure I don't get lost coming home */
						// GWP Don't change where I was.
						// GWP units[me].DestProvince = units[me].province;
						
						// GWP MakeUnitSingleStack already did this.
						// GWP units[me].Joined = FALSE;
					}
					else
					{
						SetGameData(MP_UNITS, MPUNITS_NEXTUNIT, lastGuy, me, fSend);
						/* we all follow the leader home */
						// GWP Don't change where I was.
						// GWP units[me].DestProvince = units[iPartyLead].DestProvince;
						SetGameData(MP_UNITS, MPUNITS_JOINED, me, TRUE, fSend);
						// Don't reorder the stack here, as then we would lose
						// our original province data.
					}
					
					/* pull me out of the province I'm in */
					// GWP Don't change my province. I'm going back at the end of the adventure.
					// GWP units[me].province = NO_PROVINCE;
						
					// GWP Make sure I return to where I started from.
					// GWP Since only the leader of my stack had valid province data, update mine now.
					units[me].province = units[MyLeader].province;
		
					lastGuy = me;
					++count;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					SHORT	Id = regents[Adventures[who]].mfGetid();
	
					if (Id >= 1000 && Id < 3000)
					{
						SHORT const hPS = LoadStats(Id, fERROR);
						DumbHandlePtr<PLAYER_STATS> const pPS(hPS);
						LONG i;
						LONG pr_spells, wiz_spells;
	
						pr_spells = wiz_spells = 0;
						for (i=1; i<=7; ++i)
							pr_spells += pPS->mfCanMemorize(SPELL_INFO::PRIEST, SPELL_INFO::LEVEL(i))
											-  pPS->mfMemorized(SPELL_INFO::PRIEST, SPELL_INFO::LEVEL(i));
	
						for (i=1; i<=9; ++i)
							wiz_spells += pPS->mfCanMemorize(SPELL_INFO::WIZARD, SPELL_INFO::LEVEL(i))
											-	pPS->mfMemorized(SPELL_INFO::WIZARD, SPELL_INFO::LEVEL(i));
	
						if (pr_spells > 0 || wiz_spells > 0)
						{
							fPreparingSpells = TRUE;
							MagicPrep(who);
							while (fPreparingSpells)
								MenuLoop();
						}
						
						// One charge per adventure per sceptre.
						InventoryItor itor(pPS->Inventory);
						while (itor.mfFind(ITEM_SCEPTRE_OF_CUIRAECEN))
						{
							(*itor)->mfSetCharges(1);
						}
						
						// Setup Blood healing abilites for adventures.
						itor = pPS->Inventory.begin();
						if (itor.mfFind(ITEM_HEALING_MINOR))
						{
							(*itor)->mfSetCharges(1);
						}
						
						itor = pPS->Inventory.begin();
						if (itor.mfFind(ITEM_HEALING_MAJOR))
						{
							(*itor)->mfSetCharges(2);
						}
						
						itor = pPS->Inventory.begin();
						if (itor.mfFind(ITEM_HEALING_GREAT))
						{
							(*itor)->mfSetCharges(3);
						}
						
						itor = pPS->Inventory.begin();
						if (itor.mfFind(ITEM_FEAR))
						{
							(*itor)->mfSetCharges(3);
						}
						
						itor = pPS->Inventory.begin();
						if (itor.mfFind(ITEM_TRAVEL))
						{
							(*itor)->mfSetCharges(1);
						}
						
						itor = pPS->Inventory.begin();
						if (itor.mfFind(ITEM_ENHANCED_SENSE))
						{
							// Anduiras'ns get 3 charges of detect evil.
							if (ANDUIRAS == regents[Adventures[who]].mfGetBL_deriv())
							{
								(*itor)->mfSetCharges(3);
							}
							else
							{
								(*itor)->mfSetCharges(255);	// infinite number. well as big as we can hold.
							}
						}
						
						itor = pPS->Inventory.begin();
						if (itor.mfFind(ITEM_PROTECTION_FROM_EVIL))
						{
							(*itor)->mfSetCharges(1);
						}
					}
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				}
				if (count >= 4)  // That's all we can carry!
					break;
			}
		}	
		fPreparingSpells = FALSE;
		
		iAdventureSiteIndex = SiteList[iSite];
		iSiteChosen = advsite[iAdventureSiteIndex].iPlaces;
		
		// let the scene manager know who's going, and where
		
		SCENE_MGR::Aggressor = (REALM::REALM_TYPE) advsite[iAdventureSiteIndex].realm;
		SCENE_MGR::Visitors = (REALM::REALM_TYPE) units[iPartyLead].Realm;
		SCENE_MGR::HomeIndex =  iPartyLead;	// who is the home unit (w/camera)
		SCENE_MGR::AwayIndex = -1;			// who is the away unit
		SCENE_MGR::PlacesIndex = iSiteChosen;			// where we're going
		SCENE_MGR::SceneType = SCENE_AI::ADVENTURE_SCENE;	   // type for next scene ai
		SCENE_MGR::mfRequestNewScene(advsite[iAdventureSiteIndex].name, TYPE_PLAYERSTART1, FALSE); // load first scene
		adv_dif = advsite[iAdventureSiteIndex].difficulty;
		
		fShowProgressBar = TRUE;
		
		InitDomainTurn(0);
		
		fPractice = FALSE;
		fFreezeDomainUI = FALSE;
		sMenusUp = 0;
		HidePanel(D_ADVENTURE_PREP);
		pop_regions();
		fUpdatePanels = TRUE;
	}
}
	
/* ========================================================================
   Function    - AdvPrepCancel
   Description -
   Returns     -
   ======================================================================== */
void AdvPrepCancel (LONG MenuCombo, LONG )
{
	LONG    MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	SetButtonHilight(D_ADVENTURE_PREP, ButtonId, TRUE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	SetButtonHilight(D_ADVENTURE_PREP, ButtonId, FALSE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	
	/* -----------------------------------------------------------------
	   Handle right clicks help
	   ----------------------------------------------------------------- */
	if(mouse_button == 2)
	{
		//SystemHelp(STR_BA_HELP_DONE_TITLE, STR_BA_HELP_DONE_TITLE, -1, NULL, 0);
	}
	else
	{
		//	  restore the state of the map level
		RESTORE_REGION_STATE(regDOMAIN_TURN_MODE);
		RESTORE_REGION_STATE(regACTION_MODE);
		RESTORE_REGION_STATE(regMINIMIZED);
		RESTORE_REGION_STATE(regMAXIMIZED);
		RESTORE_REGION_STATE(regTAX_ROLLS);
		RESTORE_REGION_STATE(regSWITCH_TO_ACTIONS);
		RESTORE_REGION_STATE(regADJUST_TAXES);

		//AddSndObj((BIRTHRT_SND)SND_UI_STATUS_REPORT,0,VOLUME_NINETY);
		dturn_mode = ACTN_MODE;
		if (fLTAction == DURING_LTACTION)
		{
			ActiveRegent = realm[HomeRealm].mfGetRegent();
  			fLTAction = BEFORE_LTACTION;
		}

		DataAreaMode (ACTN_MODE, 0);
		
		fPractice = FALSE;
		fFreezeDomainUI = FALSE;
		sMenusUp = 0;
		HidePanel(D_ADVENTURE_PREP);
		pop_regions();
		fUpdatePanels = TRUE;
	}
}
	
/* ========================================================================
   Function    - AdvPrepListSel
   Description -
   Returns     -
   ======================================================================== */
void AdvPrepListSel (LONG MenuCombo, LONG )
{
	SHORT	i,index;
	LONG    MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);

	index = ButtonId - BA_LIST_01;
	
	// site selection
	if(index < 8)
	{
		if (SiteList[AdvSiteListboxFirst+index] != fERROR)
		{
			// enable go on adventure button
			SetButtonLabelColor(MenuId, BA_DONE, BTN_LABEL_COLOR);
			activate_region(BA_DONE, TRUE);
		
			for(i=BA_LIST_01;i<=BA_LIST_08;++i)
				SetButtonType(MenuId, i, BUTTON_REGION);
			
			// SetButtonType(MenuId, ButtonId, BUTTON_COLORBEVEL);
			
			iSite = AdvSiteListboxSel = AdvSiteListboxFirst+index;
		}
		else
		{
			// disable go on adventure button
			SetButtonLabelColor(MenuId, BA_DONE, GREY);
			activate_region(BA_DONE, FALSE);
		}
	}	
	// party member selection
	else
	{
		index -= 8;
		if (Adventures[PartyListboxFirst+index] != fERROR)
		{
			for(i=BA_LIST_09;i<=BA_LIST_16;++i)
				SetButtonType(MenuId, i, BUTTON_REGION);
			
			//SetButtonType(MenuId, ButtonId, BUTTON_COLORBEVEL);
		
			PartyListboxSel = PartyListboxFirst+index;
			
			if (mouse_button == 1)
			{
				// show character spells inventory
				MagicPrep(PartyListboxSel);
			}
			else
			if (mouse_button == 2)
			{
				// show character status
				LONG const ThisRegentIndex = Adventures[PartyListboxSel];
				if (ThisRegentIndex > 0  &&
				    ThisRegentIndex <= CHARACTER_COUNT)
				{
					SetStatusRealm((REALM::REALM_TYPE) regents[ThisRegentIndex].mfGetRealm());
					LONG const hPlayerStats = LoadStats(regents[ThisRegentIndex].mfGetid(), fERROR);
					ShowStatus(0, hPlayerStats);
				}
			}
		}
	}
	fUpdatePanels = TRUE;
}

/* ========================================================================
   Function    - AdvPrepListScroll
   Description - scroll button for the two listboxs
   Returns     -
   ======================================================================== */
void AdvPrepListScroll (LONG MenuCombo, LONG ButtonVal)
{
	SHORT	Index;
	LONG    MenuId, ButtonId;
	SPLIT_LONG(MenuCombo, MenuId, ButtonId);
	
	// click the button
	SetButtonHilight(D_ADVENTURE_PREP, ButtonId, TRUE);
	fUpdatePanels = TRUE;
	RunPanels();
	RunMenus();
	update_screen();
	TickDelay(4);
	SetButtonHilight(D_ADVENTURE_PREP, ButtonId, FALSE);
	
	switch (ButtonId)
	{
	case ADV_LIST1_UP:
		// scroll the site list
		if (AdvSiteListboxFirst)
			AdvSiteListboxFirst--;
		// keep the sel on the list
		if (AdvSiteListboxSel > (AdvSiteListboxFirst + LISTBOX_SIZE - 1))
			AdvSiteListboxSel = (AdvSiteListboxFirst + LISTBOX_SIZE - 1);
		Index = AdvSiteListboxSel - AdvSiteListboxFirst + BA_LIST_01;
		break;
	case ADV_LIST1_DOWN:
		// scroll the site list down
		if (AdvSiteListboxFirst < (AdvSiteListboxCount - LISTBOX_SIZE))
			++AdvSiteListboxFirst;
		// keep the sel on the list
		if (AdvSiteListboxSel < AdvSiteListboxFirst)
			AdvSiteListboxSel = AdvSiteListboxFirst;
		Index = AdvSiteListboxSel - AdvSiteListboxFirst + BA_LIST_01;
		break;
	case ADV_LIST2_UP:
		// scroll the party list up
		if (PartyListboxFirst)
			PartyListboxFirst--;
		// keep the sel on the list
		if (PartyListboxSel > (PartyListboxFirst + LISTBOX_SIZE - 1))
			PartyListboxSel = (PartyListboxFirst + LISTBOX_SIZE - 1);
		Index = PartyListboxSel - PartyListboxFirst + BA_LIST_09;
		break;
	case ADV_LIST2_DOWN:
		// scroll the party list down
		if (PartyListboxFirst < (PartyListboxCount - LISTBOX_SIZE))
			++PartyListboxFirst;
		// keep the sel on the list
		if (PartyListboxSel < PartyListboxFirst)
			PartyListboxSel = PartyListboxFirst;
		Index = PartyListboxSel - PartyListboxFirst + BA_LIST_09;
		break;
	default:
		break;
	}
	
	// pass a fake click into the select code to update the selection
	mouse_button = 0;
	MenuCombo = BUILD_LONG(D_ADVENTURE_PREP,Index);
	AdvPrepListSel (MenuCombo, 0);
}

/* ========================================================================
   Function    - TogglePartyCheck
   Description -
   Returns     -
   ======================================================================== */
void TogglePartyCheck(LONG i, LONG)
{
	SHORT	j;
	SHORT	index = i + PartyListboxFirst;
	SHORT	count = 0;
	
	for(j=0;j<LIST_SIZE;++j)
	{
		if (inParty[j])
			++count;
	}
	
	if (i == 0 && inParty[0])	// can't undo first guy in list
	{
		AddSndObj(BIRTHRT_SND::SND_UI_NOT_PERMITTED,0,VOLUME_NINETY);
		return;
	}
		
	if(inParty[index])
	{
		inParty[index] = FALSE;	
	}
	else
	{
		if (count < 4)
		{
			inParty[index] = TRUE;	
			//GEH MagicPrep(index);
		}
		else
			AddSndObj(BIRTHRT_SND::SND_UI_NOT_PERMITTED,0,VOLUME_NINETY);
	}
	fUpdatePanels = TRUE;
}


/* ========================================================================
   Function    - BuildAdventureList
   Description -
   Returns     -
   ======================================================================== */
void BuildAdventureList(void)
{
	SHORT	i,j;
	SHORT	count = 0;
	SHORT	numGoing = 0;

	SHORT activeRegentUnit = regents[localActiveRegent].mfGetunit();
	SHORT realRegentUnit = regents[realm[HomeRealm].mfGetRegent()].mfGetunit();
	
	memset(&Adventures[0], -1, sizeof(SHORT)*LIST_SIZE);
	// if this is a lieut action
	if (activeRegentUnit != realRegentUnit)
	{
	 	// Lieut first, he MUST go
	 	for (j=0;j<CHARACTER_COUNT;++j)
		{
			if (regents[j].mfGetunit() == activeRegentUnit)
				Adventures[count++] = j;
		}
	}
	else
	// normal regent action
	{
	 	// real regent first, he MUST go
	 	for (j=1;j<CHARACTER_COUNT;++j)
		{
			if (regents[j].mfGetunit() == realRegentUnit)
				Adventures[count++] = j;
		}
	}

	// next, find all other eligible characters

	for(i=1; i<MAX_UNITS; ++i)
	{
		if (units[i].Realm == HomeRealm &&
			units[i].id >= 1000 && units[i].id < 3000 &&
			i != activeRegentUnit && i != realRegentUnit)
		{
			for (j=0;j<CHARACTER_COUNT;++j)
			{
				if (regents[j].mfGetunit() == i)
					Adventures[count++] = j;
			}
			if (count >= LIST_SIZE) // as many as will fit
				break;
		}
	}
	
	PartyListboxCount = count;
}

/* ========================================================================
   Function    - MagicPrep
   Description - prep this magic user for spells
   Returns     -
   ======================================================================== */
static void MagicPrep(SHORT AdvIndex)
{
	SHORT	Id = regents[Adventures[AdvIndex]].mfGetid();

	// find eligible characters
    if (Id >= 1000 && Id < 3000)
    {
	    // show 'stuff' for fighter types and spells for magic types
	    SHORT const hPlayerStats = LoadStats(Id, fERROR);
	    DumbHandlePtr<PLAYER_STATS const> const dhPlayerStats(hPlayerStats);
//	    if (dhPlayerStats->mfIsMagicUser())
			fPreparingSpells=TRUE;
//		else
//			fPreparingSpells=FALSE;
		SetStatusRealm((REALM::REALM_TYPE) regents[Adventures[AdvIndex]].mfGetRealm());
		StaticInventoryGUI::mfDisplay(0, hPlayerStats);
    }

	return;
}

void AdvPrepReturn(LONG, LONG)
{
	LONG	who, me;	  // loop index, unit# of selection
    
    for (who=0; who < LIST_SIZE; ++who)
    {
    	if (inParty[who])
    	{
    		if (Adventures[who] == -1)
    			continue;
    			
    		me = regents[Adventures[who]].mfGetunit();
    		if (units[me].province == NO_PROVINCE)
    		{
		    	units[me].province = units[me].DestProvince;
		    	units[me].DestProvince = NO_PROVINCE;
    		}	
    	}
    }

	++TutorialActionNumber;
	TutorialScreenNumber = 0;
}
