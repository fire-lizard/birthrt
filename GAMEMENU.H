/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename:
   Author: Greg Hightower
   ========================================================================
   Contains the following general functions:

   Contains the following internal functions:

   ======================================================================== */
#ifndef _GAMEMENU_H
#define _GAMEMENU_H

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#if !defined(_SYSTEM_H)
#include "system.h"
#endif

#if !defined(_COLORS_H)
#include "colors.h"
#endif

#if !defined(_MENU_H)
#include "menu.h"
#endif

#if !defined(_GMENUENM_H)
#include "gmenuenm.h"
#endif

#if !defined(_AVATAR_HXX)
#include "avatar.hxx"
#endif

#if !defined(_COMBAT_HXX)
#include "combatui.hxx"
#endif

//#include "btlstr.h"

#if !defined(_BATTLE_HXX)
#include "battle.hxx"
#endif

#if !defined(_BATTLEUI_HXX)
#include "battleui.hxx"
#endif

#if !defined(_INVNGUI_HXX)
#include "invngui.hxx"
#endif

#if !defined(_INVENTORY_HXX)
#include "invntory.hxx"
#endif

#if !defined(_TMSGBOX_HXX)
#include "tmsgbox.hxx"
#endif

#if !defined(_INFOBOX_HXX)
#include "infobox.hxx"
#endif

#if !defined(_GAME_H)
#include "game.h"
#endif

#if !defined(_ANIM_H)
#include "anim.h"
#endif

#if !defined(_STRENUM_H)
#include "strenum.h"
#endif

#if !defined(_MULTIMAP_HXX)
#include "multimap.hxx"
#endif

#if !defined(_MULTIUI_HXX)
#include "multiui.hxx"
#endif

#if !defined(_RESURECT_HXX)
#include "resurect.hxx"
#endif

#if !defined(_GAMEMAP_HXX)
#include "gamemap.hxx"
#endif

#if !defined(_PARCH_HXX)
#include "parch.hxx"
#endif

#if !defined(_REPORT_HXX)
#include "report.hxx"
#endif

#if !defined(_TUTRMENU_HXX)
#include "tutrmenu.hxx"
#endif

#if !defined(_ADVPREP_HXX)
#include "advprep.hxx"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
#ifndef _WINDOWS
void InvokeDebugger(LONG,LONG);
#endif

void ShowStatusContextHelp(LONG MC,LONG HelpStr);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
#if defined (_GERMANVER)

BUTTON AvatarAttributes[] = {
{ 0,BUTTON_BITMAP, D_FREEZE|D_ZOOM, 0,-1, 0,"UI\\STS_PANL.PCX",0,"",112, 20,368,436,NO_KEY,PaintAttributes,0,-1},

{ 1,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                 31,"",0,"", 42, 21,223, 32,NO_KEY,NULL,0,-1},
{ 2,BUTTON_BITMAP, 0,           0,STR_AVA_ATT_ANIM,  BTN_LABEL_COLOR,"UI\\STSBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\STSBTNAB.PCX", 52,400, 84, 28,KEY_M,PlayAnimation,0,-1},
{ 3,BUTTON_BITMAP, 0,           0,-1,                BTN_LABEL_COLOR,"UI\\STSBTNIA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\STSBTNIB.PCX",188,400, 84, 28,KEY_I,InventoryFromStatus,0,-1},
{ 4,BUTTON_BITMAP, 0,           0,STR_AVA_ATT_DONE,  BTN_LABEL_COLOR,"UI\\STSBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\STSBTNXB.PCX",272,400, 84, 28,KEY_F,HideStatus,0,-1},
{ 5,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                 31,"",0,"",184, 81,158, 10,NO_KEY,NULL,0,-1},
{ 6,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_CLASS, 173,"",0,"",184,112,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_CLASS,-1},
{ 7,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_LEVEL, 173,"",0,"",184,128,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_LEVEL,-1},
{ 8,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_BLOOD, 173,"",0,"",184,144,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_BLOOD,-1},
{ 9,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_ALIGN, 173,"",0,"",184,159,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_ALIGN,-1},
{10,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_STR,   173,"",0,"",184,200,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_STR,-1},
{11,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_INT,   173,"",0,"",184,216,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_INT,-1},
{12,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_DEX,   173,"",0,"",184,232,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_DEX,-1},
{13,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_WIS,   173,"",0,"",184,248,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_WIS,-1},
{14,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_CON,   173,"",0,"",184,264,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_CON,-1},
{15,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_CHA,   173,"",0,"",184,280,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_CHA,-1},
{16,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_THA,   173,"",0,"",184,296,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_THAC0,-1},
{17,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_EXP,   173,"",0,"",184,328,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_XP ,-1},
{18,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_ARM,   173,"",0,"",184,344,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_PROT,-1},
{19,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_HIT,   173,"",0,"",184,360,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_HP,-1},
{20,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_HP,    173,"",0,"",184,376,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_CURRHP,-1},
{21,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                  0,"",0,"", 16,108,152,204,NO_KEY,BowToPlayer,0,-1},
{22,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                  0,"",0,"", 20,329,134, 20,NO_KEY,NULL,0,-1},
{23,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                  0,"",0,"",  0,  0,  0,  0,KEY_ESCAPE,HideStatus,0,-1},
};
LONG AvatarAttributesSize = sizeof(AvatarAttributes) / sizeof(BUTTON);


BUTTON AvatarAttributes2[] = {
{ 0,BUTTON_BITMAP, D_FREEZE|D_ZOOM, 0,-1, 0,"UI\\STS_PANL.PCX",0,"",100, 20,368,436,NO_KEY,PaintAttributes,0,-1},

{ 1,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                 31,"",0,"", 42, 21,223, 32,NO_KEY,NULL,0,-1},
{ 2,BUTTON_BITMAP, 0,			0,STR_AVA_ATT_ANIM,  BTN_LABEL_COLOR,"UI\\STSBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\STSBTNAB.PCX", 52,400, 84, 28,KEY_M,PlayAnimation,0,-1},
{ 3,BUTTON_REGION, 0,			0,-1,                0,"",0,"",188,400, 84, 28,NO_KEY,NULL,0,-1},
{ 4,BUTTON_BITMAP, 0,			0,STR_AVA_ATT_DONE,  BTN_LABEL_COLOR,"UI\\STSBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\STSBTNXA.PCX",272,400, 84, 28,KEY_F,HideStatus2,0,-1},
{ 5,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                 31,"",0,"",184, 81,158, 10,NO_KEY,NULL,0,-1},
{ 6,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_CLASS, 173,"",0,"",184,112,158, 10,NO_KEY,NULL,0,-1},
{ 7,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_LEVEL, 173,"",0,"",184,128,158, 10,NO_KEY,NULL,0,-1},
{ 8,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_BLOOD, 173,"",0,"",184,144,158, 10,NO_KEY,NULL,0,-1},
{ 9,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_ALIGN, 173,"",0,"",184,159,158, 10,NO_KEY,NULL,0,-1},
{10,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_STR,   173,"",0,"",184,200,158, 10,NO_KEY,NULL,0,-1},
{11,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_INT,   173,"",0,"",184,216,158, 10,NO_KEY,NULL,0,-1},
{12,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_DEX,   173,"",0,"",184,232,158, 10,NO_KEY,NULL,0,-1},
{13,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_WIS,   173,"",0,"",184,248,158, 10,NO_KEY,NULL,0,-1},
{14,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_CON,   173,"",0,"",184,264,158, 10,NO_KEY,NULL,0,-1},
{15,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_CHA,   173,"",0,"",184,280,158, 10,NO_KEY,NULL,0,-1},
{16,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_THA,   173,"",0,"",184,296,158, 10,NO_KEY,NULL,0,-1},
{17,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_EXP,   173,"",0,"",184,328,158, 10,NO_KEY,NULL,0,-1},
{18,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_ARM,   173,"",0,"",184,344,158, 10,NO_KEY,NULL,0,-1},
{19,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_HIT,   173,"",0,"",184,360,158, 10,NO_KEY,NULL,0,-1},
{20,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_HP,    173,"",0,"",184,376,158, 10,NO_KEY,NULL,0,-1},
{21,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                  0,"",0,"", 16,108,152,204,NO_KEY,BowToPlayer,0,-1},
{22,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                  0,"",0,"", 20,329,134, 20,NO_KEY,NULL,0,-1},
{23,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                  0,"",0,"",  0,  0,  0,  0,KEY_ESCAPE,HideStatus2,0,-1},
};
LONG AvatarAttributes2Size = sizeof(AvatarAttributes2) / sizeof(BUTTON);



BUTTON AvatarInventory[] = {
{INV_BUT_ALL      ,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,0,    "UI\\INV_PANL.PCX",0,"",100, 50,392,348,	NO_KEY,	StaticInventoryGUI::mfPaint,				0,			-1},
{INV_BUT_TITLE    ,BUTTON_REGION,D_TEXT_LEFT,0,STR_AVA_INV_INV,173,   "",0,"",                 42, 21,223, 32,	KEY_I,	NULL,										0,			-1},
{INV_BUT_DONE     ,BUTTON_BITMAP,0,0,STR_AVA_INV_DONE,BTN_LABEL_COLOR,  "UI\\INVBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTNXB.PCX",300,312, 84, 28,	KEY_F,	StaticInventoryGUI::mfDone,					0,			-1},
{INV_BUT_STATUS   ,BUTTON_BITMAP,0,0,STR_AVA_INV_STATUS,BTN_LABEL_COLOR,"UI\\INVBTNSA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTNSB.PCX",216,312, 84, 28,	KEY_S,	StaticInventoryGUI::mfGoStatus,				0,			-1},
{INV_BUT_DUDENAME, BUTTON_REGION,0,0,-1,0,"",0,"",					125,	80, 	230,	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_INFO,     BUTTON_REGION,0,0,-1,0,"",0,"",					120,	240,	240,	45, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_HELPAST,  BUTTON_REGION,0,0,STR_AVA_INV_HELP_ASTERISK,173,"",0,"",120,285,	240,	15, 	NO_KEY,	NULL,										0,			-1}, //context sensitive help here?
{INV_BUT_LISTBOX,  BUTTON_REGION,0,0,-1,0,"",0,"",					120,	108,	239,	119,	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNUMBER1,BUTTON_REGION,0,0,-1,0,"",0,"",					152,	107,	19, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNUMBER2,BUTTON_REGION,0,0,-1,0,"",0,"",					152,	127,	19, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNUMBER3,BUTTON_REGION,0,0,-1,0,"",0,"",					152,	147,	19, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNUMBER4,BUTTON_REGION,0,0,-1,0,"",0,"",					152,	167,	19, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNUMBER5,BUTTON_REGION,0,0,-1,0,"",0,"",					152,	187,	19, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNUMBER6,BUTTON_REGION,0,0,-1,0,"",0,"",					152,	207,	19, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBICON1,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	107,	29, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBICON2,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	127,	29, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBICON3,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	147,	29, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBICON4,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	167,	29, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBICON5,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	187,	29, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBICON6,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	207,	29, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNAME1,  BUTTON_REGION,0,0,-1,0,"",0,"",					171,	107,	188,	20, 	NO_KEY,	StaticInventoryGUI::mfSetHighlightedLine,	0,			-1},
{INV_BUT_LBNAME2,  BUTTON_REGION,0,0,-1,0,"",0,"",					171,	127,	188,	20, 	NO_KEY,	StaticInventoryGUI::mfSetHighlightedLine,	1,			-1},
{INV_BUT_LBNAME3,  BUTTON_REGION,0,0,-1,0,"",0,"",					171,	147,	188,	20, 	NO_KEY,	StaticInventoryGUI::mfSetHighlightedLine,	2,			-1},
{INV_BUT_LBNAME4,  BUTTON_REGION,0,0,-1,0,"",0,"",					171,	167,	188,	20, 	NO_KEY,	StaticInventoryGUI::mfSetHighlightedLine,	3,			-1},
{INV_BUT_LBNAME5,  BUTTON_REGION,0,0,-1,0,"",0,"",					171,	187,	188,	20, 	NO_KEY,	StaticInventoryGUI::mfSetHighlightedLine,	4,			-1},
{INV_BUT_LBNAME6,  BUTTON_REGION,0,0,-1,0,"",0,"",					171,	207,	188,	20, 	NO_KEY,	StaticInventoryGUI::mfSetHighlightedLine,	5,			-1},
{INV_BUT_LBPLUS1,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	107,	15, 	20, 	NO_KEY,	StaticInventoryGUI::mfAddToAvailSpells,		0,			-1},
{INV_BUT_LBPLUS2,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	127,	15, 	20, 	NO_KEY,	StaticInventoryGUI::mfAddToAvailSpells,		1,			-1},
{INV_BUT_LBPLUS3,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	147,	15, 	20, 	NO_KEY,	StaticInventoryGUI::mfAddToAvailSpells,		2,			-1},
{INV_BUT_LBPLUS4,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	167,	15, 	20, 	NO_KEY,	StaticInventoryGUI::mfAddToAvailSpells,		3,			-1},
{INV_BUT_LBPLUS5,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	187,	15, 	20, 	NO_KEY,	StaticInventoryGUI::mfAddToAvailSpells,		4,			-1},
{INV_BUT_LBPLUS6,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	207,	15, 	20, 	NO_KEY,	StaticInventoryGUI::mfAddToAvailSpells,		5,			-1},
{INV_BUT_LBMINUS1, BUTTON_REGION,0,0,-1,0,"",0,"",					138,	107,	14, 	20, 	NO_KEY,	StaticInventoryGUI::mfTakeFromAvailSpells,	0,			-1},
{INV_BUT_LBMINUS2, BUTTON_REGION,0,0,-1,0,"",0,"",					138,	127,	14, 	20, 	NO_KEY,	StaticInventoryGUI::mfTakeFromAvailSpells,	1,			-1},
{INV_BUT_LBMINUS3, BUTTON_REGION,0,0,-1,0,"",0,"",					138,	147,	14, 	20, 	NO_KEY,	StaticInventoryGUI::mfTakeFromAvailSpells,	2,			-1},
{INV_BUT_LBMINUS4, BUTTON_REGION,0,0,-1,0,"",0,"",					138,	167,	14, 	20, 	NO_KEY,	StaticInventoryGUI::mfTakeFromAvailSpells,	3,			-1},
{INV_BUT_LBMINUS5, BUTTON_REGION,0,0,-1,0,"",0,"",					138,	187,	14, 	20, 	NO_KEY,	StaticInventoryGUI::mfTakeFromAvailSpells,	4,			-1},
{INV_BUT_LBMINUS6, BUTTON_REGION,0,0,-1,0,"",0,"",					138,	207,	14, 	20, 	NO_KEY,	StaticInventoryGUI::mfTakeFromAvailSpells,	5,			-1},
{INV_BUT_MUNDANE,  BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN1B.PCX",	12, 	108,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfChangeCurrCat,		IC_MUNDANE,	-1},
{INV_BUT_ARMS,     BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN2A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN2B.PCX",	12, 	132,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfChangeCurrCat,		IC_MAGITEM,	-1},
{INV_BUT_SPELLS,   BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN3A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN3B.PCX",	12, 	156,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfChangeCurrCat,		IC_SPELL,	-1},
{INV_BUT_MAGICITMS,BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN4A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN4B.PCX",	12, 	180,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfChangeCurrCat,		IC_REALM_SPELL,-1},
{INV_BUT_BLOODABLT,BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN5A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN5B.PCX",	12, 	204,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfChangeCurrCat,		IC_BLOODAB,	-1},
{INV_BUT_USE,      BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN6A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN6B.PCX",	12, 	236,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfUseHighlightedObject,	0,			-1},
{INV_BUT_DROP,     BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN7A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN7B.PCX",	12, 	260,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfDropItem,				0,			-1},
{INV_BUT_GIVE,     BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN8A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN8B.PCX",	12, 	284,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfGiveItem,				0,			-1},
{INV_BUT_SCROLLUP, BUTTON_BITMAP,0,0,-1,0,"UI\\INVBTNUA.PCX",0,"UI\\INVBTNUB.PCX",	360,	108,	24, 	60, 	NO_KEY,	StaticInventoryGUI::mfScroll,				-1,			-1},
{INV_BUT_SCROLLDN, BUTTON_BITMAP,0,0,-1,0,"UI\\INVBTNDA.PCX",0,"UI\\INVBTNDB.PCX",	360,	168,	24, 	60, 	NO_KEY,	StaticInventoryGUI::mfScroll,				1,			-1},
{INV_BUT_SPELLBOX1,BUTTON_REGION,0,0,-1,0,"",0,"",					15, 	317,	25, 	18, 	KEY_1,	StaticInventoryGUI::mfClearSpellBox,		0,			-1},
{INV_BUT_SPELLBOX2,BUTTON_REGION,0,0,-1,0,"",0,"",					47, 	317,	25, 	18, 	KEY_2,	StaticInventoryGUI::mfClearSpellBox,		1,			-1},
{INV_BUT_SPELLBOX3,BUTTON_REGION,0,0,-1,0,"",0,"",					79, 	317,	25, 	18, 	KEY_3,	StaticInventoryGUI::mfClearSpellBox,		2,			-1},
{INV_BUT_SPELLBOX4,BUTTON_REGION,0,0,-1,0,"",0,"",					111,	317,	25, 	18, 	KEY_4,	StaticInventoryGUI::mfClearSpellBox,		3,			-1},
{INV_BUT_SPELLBOX5,BUTTON_REGION,0,0,-1,0,"",0,"",					143,	317,	25, 	18, 	KEY_5,	StaticInventoryGUI::mfClearSpellBox,		4,			-1},
{INV_BUT_SPELLBOX6,BUTTON_REGION,0,0,-1,0,"",0,"",					175,	317,	25, 	18, 	KEY_6,	StaticInventoryGUI::mfClearSpellBox,		5,			-1},
{665     ,BUTTON_REGION,0,0,-1,173,  "",0,"",0,0,0,0,KEY_ESCAPE,StaticInventoryGUI::mfDone,0,-1},
};
LONG AvatarInventorySize = sizeof(AvatarInventory) / sizeof(BUTTON);

BUTTON MessageBoxTitled[] = {
{MSGBOX_BUT_ALL   ,BUTTON_BITMAP, D_FREEZE|D_ZOOM, 0,-1,0,"UI\\HELP_SM.PCX",0,"",105,100,356,112,NO_KEY,TitledMessageBox::PrintMessage,0,-1},
{MSGBOX_BUT_TITLE ,BUTTON_REGION, 0, 0,-1,31,"",0,"",4,19,356,31,KEY_SPACE,TitledMessageBox::Hide,0,-1},
{MSGBOX_BUT_MSG   ,BUTTON_REGION, 0, 0,-1,31,"",0,"",4,35,356,55,KEY_ENTER,TitledMessageBox::Hide,0,-1},
{MSGBOX_BUT_OK    ,BUTTON_BITMAP, 0, 0,STR_OK,BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",128,76,100,28,KEY_O,TitledMessageBox::Hide,0,-1},
};
LONG MessageBoxTitledSize = sizeof(MessageBoxTitled) / sizeof(BUTTON);

BUTTON SmallInfoBox[] = {
{INFOBOX_BUT_ALL   ,BUTTON_BITMAP, D_FREEZE|D_ZOOM|D_DONT_SHADE, 0,-1,0,"UI\\INFOBOX.PCX",0,"",160,100,319,41,NO_KEY,InfoBox::Paint,0,-1},
};
LONG InfoBoxSize = sizeof(SmallInfoBox) / sizeof(BUTTON);


BUTTON ResurrectionScreen[] = {
{RESURRECT_BUT_ALL	,BUTTON_BITMAP, D_FREEZE|D_ZOOM, 0,-1,0,"UI\\RESURECT.PCX",0,"",160,100,330,170,NO_KEY,ResurrectionMenu::Paint,0,-1},
{RESURRECT_BUT_TITLE,BUTTON_REGION, 0, 0,STR_ITEM_UI_RESURRECTION,31,"",0,"",4, 4,322, 24,NO_KEY,NULL,0,-1},
{RESURRECT_BUT_Q	,BUTTON_REGION, 0, 0,STR_ITEM_UI_RESURRECTION_QUESTION,31,"",0,"",4,42,322,15,NO_KEY,NULL,0,-1},
{RESURRECT_BUT_NAME1,BUTTON_REGION, 0, 0,-1,31,"",0,"",4,57,322,18,NO_KEY,ResurrectionMenu::Resurrect,0,-1},
{RESURRECT_BUT_NAME2,BUTTON_REGION, 0, 0,-1,31,"",0,"",4,75,322,18,NO_KEY,ResurrectionMenu::Resurrect,1,-1},
{RESURRECT_BUT_NAME3,BUTTON_REGION, 0, 0,-1,31,"",0,"",4,93,322,18,NO_KEY,ResurrectionMenu::Resurrect,2,-1},
{RESURRECT_BUT_NAME4,BUTTON_REGION, 0, 0,-1,31,"",0,"",4,111,322,18,NO_KEY,ResurrectionMenu::Resurrect,3,-1},
{RESURRECT_BUT_CANCL,BUTTON_REGION, 0, 0,STR_CANCEL,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"",126,129,75,19,KEY_A,HideSubMenuWithClick,D_RESURRECT_MENU,-1},
{665,BUTTON_REGION, 0, 0,-1,31,"",0,"",0,0,0,0,KEY_ESCAPE,HideSubMenu,D_RESURRECT_MENU,-1},
};
LONG ResurrectionScreenSize = sizeof(ResurrectionScreen) / sizeof(BUTTON);


BUTTON BtlControl[] = {
{ 0,          BUTTON_BITMAP,0,0,-1,             0,"UI\\BATLPNL1.PCX",0,"",0,316,640,164,NO_KEY,BattleControlPaint,0,-1},
{ 2,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  338, 93, 55, 40,NO_KEY,BattleControlGrid, 0,-1},
{ 3,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  398, 93, 55, 40,NO_KEY,BattleControlGrid, 1,-1},
{ 4,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  458, 93, 55, 40,NO_KEY,BattleControlGrid, 2,-1},
{ 5,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  518, 93, 55, 40,NO_KEY,BattleControlGrid, 3,-1},
{ 6,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  578, 93, 55, 40,NO_KEY,BattleControlGrid, 4,-1},
{ 7,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  338, 49, 55, 40,NO_KEY,BattleControlGrid, 5,-1},
{ 8,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  398, 49, 55, 40,NO_KEY,BattleControlGrid, 6,-1},
{ 9,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  458, 49, 55, 40,NO_KEY,BattleControlGrid, 7,-1},
{10,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  518, 49, 55, 40,NO_KEY,BattleControlGrid, 8,-1},
{11,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  578, 49, 55, 40,NO_KEY,BattleControlGrid, 9,-1},
{12,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  338,  5, 55, 40,NO_KEY,BattleControlGrid,10,-1},
{13,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  398,  5, 55, 40,NO_KEY,BattleControlGrid,11,-1},
{14,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  458,  5, 55, 40,NO_KEY,BattleControlGrid,12,-1},
{15,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  518,  5, 55, 40,NO_KEY,BattleControlGrid,13,-1},
{16,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  578,  5, 55, 40,NO_KEY,BattleControlGrid,14,-1},
{17,          BUTTON_REGION,0,0,-1,             0,"",0,"",                  7,  5,323,100,NO_KEY,BattleReservesGrid,D_BTL_CONTROL,-1},
{BTL_RESV_ID, BUTTON_REGION,0,0,BTLUI_RESV,     BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLBTN1B.PCX", 40,136, 84, 28,KEY_R,BattleReservesMode,0,-1},
{BTL_STAT_ID, BUTTON_REGION,0,0,BTLUI_STATUS,   BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLBTN2B.PCX",124,136, 84, 28,KEY_S,BattleStatusMode,0,-1},
{BTL_EXIT_ID, BUTTON_REGION,0,0,BTLUI_RETREAT,  BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLBTN3B.PCX",208,136, 84, 28,KEY_K,BattleRetreat,0,-1},
{BTL_DONE_ID, BUTTON_REGION,0,0,STR_DONE,       BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLBTN3B.PCX",268,136, 84, 28,KEY_D,BattleNextTurn,0,-1},
{BTL_VIEW_ID, BUTTON_REGION,0,0,BTLUI_COMMAND,  BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLBTN4B.PCX",424,136,124, 28,KEY_A,BattleCameraHighView,0,-1},
{BTL_CANCEL_ID,BUTTON_REGION,0,0,STR_CANCEL1,   BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLCANCE.PCX",150,107, 70, 21,NO_KEY,BattleCancel,0,-1},
{BTL_FALLBACK_ID,BUTTON_REGION,0,0,BTLUI_FALLBACK,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLCANCE.PCX", 70,107, 70, 21,NO_KEY,BattleFallBack,0,-1},
{BTL_LIGHT_ID,BUTTON_REGION,0,0,-1,             0,"",0,"UI\\LIGHT.PCX",     7,  0,129,135,NO_KEY,NULL,0,-1},
};
LONG BtlControlSize = sizeof(BtlControl) / sizeof(BUTTON);


// Note: The next four Button menus are nearly identical. Be sure to duplicate
//       any changes in each of the menus.

BUTTON CombatControl1[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT2A.PCX",0,"",  0,344,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,0,0,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",149, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",177, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
{BUTNO_SPELL5,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,4,-1},
{BUTNO_SPELL6,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,5,-1},
};
LONG CombatControlSize1 = sizeof(CombatControl1) / sizeof(BUTTON);

BUTTON CombatControl2[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT2A.PCX",0,"",320,344,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,0,0,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",149, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",177, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
{BUTNO_SPELL5,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,4,-1},
{BUTNO_SPELL6,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,5,-1},
};
LONG CombatControlSize2 = sizeof(CombatControl2) / sizeof(BUTTON);

BUTTON CombatControl3[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT2A.PCX",0,"",  0,404,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,0,0,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",149, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",177, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
{BUTNO_SPELL5,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,4,-1},
{BUTNO_SPELL6,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,5,-1},
};
LONG CombatControlSize3 = sizeof(CombatControl3) / sizeof(BUTTON);

BUTTON CombatControl4[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT2A.PCX",0,"",320,404,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,0,0,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",149, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",177, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
{BUTNO_SPELL5,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,4,-1},
{BUTNO_SPELL6,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,5,-1},
};
LONG CombatControlSize4 = sizeof(CombatControl4) / sizeof(BUTTON);


BUTTON CombatFighterControl1[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT1A.PCX",0,"",  0,344,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_LOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,CombatLowAttackKey,0,COMBAT_STR_ATTACK2},
{BUTNO_QLOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",143, 37, 25, 18,NO_KEY,CombatLowAttackKey,1,COMBAT_STR_QUICK_ATTACK2},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",171, 37, 25, 18,NO_KEY,0,1,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
};
LONG CombatFighterControlSize1 = sizeof(CombatFighterControl1) / sizeof(BUTTON);

BUTTON CombatFighterControl2[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT1A.PCX",0,"",320,344,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_LOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,CombatLowAttackKey,0,COMBAT_STR_ATTACK2},
{BUTNO_QLOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",143, 37, 25, 18,NO_KEY,CombatLowAttackKey,1,COMBAT_STR_QUICK_ATTACK2},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",171, 37, 25, 18,NO_KEY,0,1,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
};
LONG CombatFighterControlSize2 = sizeof(CombatFighterControl2) / sizeof(BUTTON);

BUTTON CombatFighterControl3[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT1A.PCX",0,"",  0,404,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_LOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,CombatLowAttackKey,0,COMBAT_STR_ATTACK2},
{BUTNO_QLOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",143, 37, 25, 18,NO_KEY,CombatLowAttackKey,1,COMBAT_STR_QUICK_ATTACK2},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",171, 37, 25, 18,NO_KEY,0,1,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
};
LONG CombatFighterControlSize3 = sizeof(CombatFighterControl3) / sizeof(BUTTON);

BUTTON CombatFighterControl4[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT1A.PCX",0,"",320,404,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_LOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,CombatLowAttackKey,0,COMBAT_STR_ATTACK2},
{BUTNO_QLOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",143, 37, 25, 18,NO_KEY,CombatLowAttackKey,1,COMBAT_STR_QUICK_ATTACK2},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",171, 37, 25, 18,NO_KEY,0,1,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
};
LONG CombatFighterControlSize4 = sizeof(CombatFighterControl4) / sizeof(BUTTON);

BUTTON CombatRPGBottomBar[] = {
{ 0,					BUTTON_BITMAP, D_NO_PUSH, 0,-1,		              0,"UI\\EXTRA_16.PCX",0,"",  0,464,640,16,NO_KEY,0,0,-1},
{BUTNO_RPG_LEAVE,		BUTTON_BITMAP,		   0, 0,COMBAT_STR_LEAVE,	BTN_LABEL_COLOR,"UI\\CMBBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN1B.PCX",  0,  0,112,16,NO_KEY,CombatReturnToMap,0,-1},
{BUTNO_RPG_RETREAT,		BUTTON_BITMAP,		   0, 0,COMBAT_STR_RETREAT,	BTN_LABEL_COLOR,"UI\\CMBBTN2A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN2B.PCX",112,  0, 80,16,NO_KEY,CombatRetreatKey,0,-1},
{BUTNO_RPG_LEADER,		BUTTON_BITMAP,		   0, 0,COMBAT_STR_LEADER,	BTN_LABEL_COLOR,"UI\\CMBBTN3A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN3B.PCX",192,  0,112,16,NO_KEY,CombatNewLeaderKey,0,-1},
{BUTNO_RPG_VIEW,		BUTTON_BITMAP,		   0, 0,COMBAT_STR_VIEW,	BTN_LABEL_COLOR,"UI\\CMBBTN4A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN4B.PCX",304,  0,110,16,NO_KEY,CombatToggleViewKey,0,-1},
{BUTNO_RPG_INVENTORY,	BUTTON_BITMAP,		   0, 0,COMBAT_STR_INVENTORY,BTN_LABEL_COLOR,"UI\\CMBBTN5A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN5B.PCX",404,  0, 80,16,NO_KEY,CombatShowInventoryKey,1,-1},
{BUTNO_RPG_STATUS,		BUTTON_BITMAP,		   0, 0,COMBAT_STR_STATUS, 	BTN_LABEL_COLOR,"UI\\CMBBTN6A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN6B.PCX",484,  0, 52,16,NO_KEY,CombatShowStatusKey,1,-1},
{BUTNO_RPG_MAP,			BUTTON_BITMAP,		   0, 0,COMBAT_STR_MAP,		BTN_LABEL_COLOR,"UI\\CMBBTN7A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN7B.PCX",536,  0, 52,16,NO_KEY,CombatToggleMapKey,0,-1},
{BUTNO_RPG_DONE,		BUTTON_BITMAP,		   0, 0,-1,					  0,"UI\\CMBBTN8A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN8B.PCX",588,  0, 52,16,NO_KEY,CombatTurnDoneKey,0,-1}
};
LONG CombatRPGBottomBarSize = sizeof(CombatRPGBottomBar)/ sizeof(BUTTON);


BUTTON CombatBlank1Bar[] = {
{ 0,BUTTON_BITMAP, D_NO_PUSH, 0, -1,0,"UI\\SCMBT_UE.PCX",0,"",320,404,320, 60,NO_KEY,0,0,-1},
};
LONG CombatBlank1BarSize = sizeof(CombatBlank1Bar)/sizeof(BUTTON);

BUTTON BtlChooseField[] = {
{ 0,BUTTON_BITMAP,D_ZOOM, 0,-1, 0,"UI\\CHOOSBTL.PCX",0,"",145, 50,351,371,NO_KEY,PaintChooseBattleField,0,-1},
{ 1,BUTTON_BITMAP,0,      0,-1, 0,"",                0,"", 10, 36,150,160,NO_KEY,ChooseBattleField, 0,-1},
{ 2,BUTTON_BITMAP,0,      0,-1, 0,"",                0,"",172, 36,150,160,NO_KEY,ChooseBattleField, 1,-1},
{ 3,BUTTON_BITMAP,0,      0,-1, 0,"",                0,"", 10,201,150,160,NO_KEY,ChooseBattleField, 2,-1},
{ 4,BUTTON_BITMAP,0,      0,-1, 0,"",                0,"",172,201,150,160,NO_KEY,ChooseBattleField, 3,-1},
{ 6,BUTTON_REGION,0,      0,-1, DKBROWN,"",          0,"",  4,  4,322, 30,NO_KEY,NULL,0,-1},
{91,BUTTON_REGION,0,      0,-1, DKBROWN,"",          0,"", 10,180,150, 20,NO_KEY,NULL,0,-1},
{92,BUTTON_REGION,0,      0,-1, DKBROWN,"",          0,"",172,180,150, 20,NO_KEY,NULL,0,-1},
{93,BUTTON_REGION,0,      0,-1, DKBROWN,"",          0,"", 10,344,150, 20,NO_KEY,NULL,0,-1},
{94,BUTTON_REGION,0,      0,-1, DKBROWN,"",          0,"",172,344,150, 20,NO_KEY,NULL,0,-1},
};
LONG BtlChooseFieldSize = sizeof(BtlChooseField) / sizeof(BUTTON);

BUTTON ShowAnimation[] = {
{0,BUTTON_COLORBEVEL,D_FREEZE|D_ZOOM, 76,-1,0,"",0,"", 40, 10,550,460,NO_KEY,PaintAnimation,0,-1},
{1,BUTTON_REGION,0, 0,-1,                   0,"",0,"", 20, 10,510,360,NO_KEY,0,0,-1},
{2,BUTTON_REGION,0, 0,-1,			        0,"",0,"",113,370, 78, 16,NO_KEY,GetPrevAnimation,0,-1},
{3,BUTTON_REGION,0, 0,-1,                   0,"",0,"",193,370, 78, 16,NO_KEY,GetNextAnimation,0,-1},
{4,BUTTON_REGION,0, 0,-1,                   0,"",0,"",113,390, 78, 16,NO_KEY,GetPrevSequence,0,-1},
{5,BUTTON_REGION,0, 0,-1,                   0,"",0,"",193,390, 78, 16,NO_KEY,GetNextSequence,0,-1},
{6,BUTTON_REGION,0, 0,-1,                   0,"",0,"",113,410, 78, 16,NO_KEY,GetPrevRotation,0,-1},
{7,BUTTON_REGION,0, 0,-1,                   0,"",0,"",193,410, 78, 16,NO_KEY,GetNextRotation,0,-1},
{8,BUTTON_REGION,0, 0,-1,                   0,"",0,"", 22,430, 88, 16,NO_KEY,RunLoop,0,-1},
{9,BUTTON_REGION,0, 0,-1,                   0,"",0,"",113,430, 78, 16,NO_KEY,StopLoop,0,-1},
{10,BUTTON_REGION,0, 0,-1,                  0,"",0,"",453,430, 78, 16,KEY_ESCAPE,CloseAnimationMenu, 0,-1},
{11,BUTTON_REGION,0, 0,-1,                  0,"",0,"",193,430, 78, 16,NO_KEY,PlayAll,0,-1},
{12,BUTTON_REGION,0, 0,-1,                  0,"",0,"",273,430, 88, 16,NO_KEY,SpeedUp,0,-1},
{13,BUTTON_REGION,0, 0,-1,                  0,"",0,"",363,430, 88, 16,NO_KEY,SlowDown,0,-1},
{14,BUTTON_REGION,0, 0,-1,                  0,"",0,"",343,370,150, 60,NO_KEY,0, 0,-1},
{15,BUTTON_REGION,0, 0,-1,                  0,"",0,"",22, 370, 88, 60,NO_KEY,0,0,-1},

};
LONG ShowAnimationSize = sizeof(ShowAnimation)/sizeof(BUTTON);


BUTTON MoreInfo[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,0,"UI\\INFODLOG.PCX",0,"",145, 50,352,175,NO_KEY,PaintMoreInfo,0,-1},
{1,BUTTON_REGION,0,               0,-1,0,"",0,"", 24, 24,305,127,NO_KEY,NULL, 0,-1},
{4,BUTTON_REGION,0,               0,-1,0,"",0,"",152,151,48,15,KEY_ESCAPE,HideSubMenuWithClick, D_MOREINFO,-1},
};
LONG MoreInfoSize = sizeof(MoreInfo) / sizeof(BUTTON);

BUTTON HoldAction[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,0,"UI\\RELMSLCT.PCX",0,"",145,210,356,70,NO_KEY,PaintHoldAction,0,-1},
{1,BUTTON_REGION,0,               0,-1,0,"",0,"", 24, 24,305,127,NO_KEY,CancelHoldAction, 0,-1},
{4,BUTTON_REGION,0,               0,-1,0,"",0,"",161,151,48,15,KEY_ESCAPE,DoneHoldAction, 0,-1},
};
LONG HoldActionSize = sizeof(HoldAction) / sizeof(BUTTON);

BUTTON HelpKeys[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,      0,"UI\\HELP_LG.PCX",0,"",160, 50,356,372,NO_KEY,PaintMoreInfo,0, -1},
{1,BUTTON_REGION,0,               0,-1,      0,"",0,"",                 24, 24,305,320,NO_KEY,0, 0,-1},
{2,BUTTON_REGION,0,     0,STR_OK, BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAA.PCX",128,336,100, 28,KEY_O,DoneHelpKeys, 0,-1},
{3,BUTTON_REGION,0,     0,-1, 0,"",0,"",0,0,0,0,KEY_ESCAPE,DoneHelpKeys, 1,-1},
};
LONG HelpKeysSize = sizeof(HelpKeys) / sizeof(BUTTON);


BUTTON BattleCard[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,0,"UI\\WARCARD.PCX",0,"",160,128,320,192,NO_KEY,PaintBattleCard,0,-1},
{1,BUTTON_REGION,0,               0,-1,0,"",0,"",                 8,  8,304,176,KEY_ESCAPE,BattleCardProc, D_BTL_CARD,-1},
};
LONG BattleCardSize = sizeof(BattleCard) / sizeof(BUTTON);

BUTTON Victory[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,            0,"UI\\VCTPANL1.PCX",0,"",150,30,340,388,NO_KEY,NULL,0,-1},
{1,BUTTON_BITMAP,0,               0,STR_HELP_DONE,BTN_LABEL_COLOR,"UI\\VCTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\VCTBTNXB.PCX",                128,352,84,28,KEY_F,HideSubMenuWithClick, D_VICTORY,-1},
};
LONG VictorySize = sizeof(Victory) / sizeof(BUTTON);

BUTTON Defeat[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,            0,"UI\\DFTPANL1.PCX",0,"",150,30,340,388,NO_KEY,NULL,0,-1},
{1,BUTTON_BITMAP,0,               0,STR_HELP_DONE,BTN_LABEL_COLOR,"UI\\DFTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\DFTBTNXB.PCX",                128,352,84,28,KEY_F,HideSubMenuWithClick, D_DEFEAT,-1},
};
LONG DefeatSize = sizeof(Defeat) / sizeof(BUTTON);

BUTTON ChatDisplayWindow[] =  {
{0,BUTTON_BITMAP,D_FLOATER, 0,-1, 0,"UI\\CHATPNL.PCX",0,"", 0,0,640,64,NO_KEY,PaintChatDisplay,0,-1},
};
LONG ChatDisplayWindowSize = sizeof(ChatDisplayWindow) / sizeof(BUTTON);


BUTTON ChatDisplayWindow1[] =  {
{0,BUTTON_BITMAP,D_FLOATER, 0,-1, 0,"UI\\CHATBOX2.PCX",0,"", 0,0,640,128,NO_KEY, PaintChatDisplay, 1, -1},
};
LONG ChatDisplayWindow1Size = sizeof(ChatDisplayWindow1) / sizeof(BUTTON);

BUTTON ChatSelect[] = {                                           //80
{0,BUTTON_BITMAP,D_FREEZE, 0, -1, 0, "UI\\CHATPNL3.PCX", 0,"", 83, 130, 472, 320,NO_KEY,PaintChatSelect,0, -1},
{1,BUTTON_REGION,0, 0,-1,                0, "",0,"", 25, 100,385,15,NO_KEY,0, 0,-1},
{2,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 132,380,15,NO_KEY,0, 1,-1},
{3,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 148,380,15,NO_KEY,0, 2,-1},
{4,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 164,380,15,NO_KEY,0, 3,-1},
{5,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 180,380,15,NO_KEY,0, 4,-1},
{6,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 196,380,15,NO_KEY,0, 5,-1},
{7,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 212,380,15,NO_KEY,0, 6,-1},
{8,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 228,380,15,NO_KEY,0, 7,-1},
{9,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 244,380,15,NO_KEY,0, 8,-1},
{10,BUTTON_REGION,0, 0,STR_CANCEL1, BTN_LABEL_COLOR,"UI\\CHTBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CHTBTNCB.PCX",252,264,84, 28,KEY_ESCAPE, CancelChatSelect, 0, -1},
{11,BUTTON_REGION,0, 0,STR_DONE1,   BTN_LABEL_COLOR,"UI\\CHTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CHTBTNXB.PCX",336,264,84, 28,NO_KEY,DoneChatSelect, 0, -1},
};

LONG ChatSelectSize = sizeof(ChatSelect) / sizeof(BUTTON);


BUTTON MultiName[] = {
{ 0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\MLT_PNL4.PCX", 0,"", 162, 46, 316, 232,NO_KEY,PaintMultiPlayerName,0, -1},
{ 1,BUTTON_REGION,0,                    0,-1,         0, "",0,"", 61, 100,200,15,NO_KEY,NULL, 0,-1},
{ 2,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,-1,         0, "",0,"", 61, 135,200,15,NO_KEY,SetMultiPlayerProtocol, 0,-1},
{ 3,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,-1,         0, "",0,"", 61, 151,200,15,NO_KEY,SetMultiPlayerProtocol, 1,-1},
{ 4,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,-1,         0, "",0,"", 61, 167,200,15,NO_KEY,SetMultiPlayerProtocol, 2,-1},
{ 9,BUTTON_BITMAP,0, 0,STR_BACK1,  BTN_LABEL_COLOR,"UI\\MLTBTNBA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNBB.PCX", 32,196,84, 28,NO_KEY,BackMultiPlayerName, 0, -1},
{10,BUTTON_BITMAP,0, 0,STR_JOIN,   BTN_LABEL_COLOR,"UI\\MLTBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNCB.PCX",116,196,84, 28,NO_KEY,JoinMultiPlayerName, 16, -1},
{11,BUTTON_BITMAP,0, 0,STR_CREATE, BTN_LABEL_COLOR,"UI\\MLTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNXB.PCX",200,196,84, 28,NO_KEY,HostMultiPlayerName, 0, -1},
{99,BUTTON_REGION,0, 0,-1,0,"",0,"",0,0,0,0,KEY_ESCAPE,BackMultiPlayerName, 16, -1},
};
LONG MultiNameSize = sizeof(MultiName) / sizeof(BUTTON);

BUTTON MultiCreate[] = {
{ 0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\MLT_PNL4.PCX", 0,"", 162, 46, 316, 232,NO_KEY,PaintMultiPlayerCreate,0, -1},
{ 1,BUTTON_REGION,0,        0,-1,         0,              "",0,"", 61, 100,100,15,NO_KEY,NULL, 0,-1},
{ 1,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_MULTIMENU_NP,BTN_LABEL_COLOR,"",0,"", 60, 130,200,14,NO_KEY,NULL, 0,-1},
{ 2,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_2,                 BTN_LABEL_COLOR,"",0,"", 60, 151,50,15,NO_KEY,SetMultiPlayerNumber, 2,-1},
{ 3,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_3,                 BTN_LABEL_COLOR,"",0,"", 60, 167,50,15,NO_KEY,SetMultiPlayerNumber, 3,-1},
{ 4,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_4,                 BTN_LABEL_COLOR,"",0,"",110, 151,50,15,NO_KEY,SetMultiPlayerNumber, 4,-1},
{ 5,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_5,                 BTN_LABEL_COLOR,"",0,"",110, 167,50,15,NO_KEY,SetMultiPlayerNumber, 5,-1},
{ 6,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_6,                 BTN_LABEL_COLOR,"",0,"",160, 151,50,15,NO_KEY,SetMultiPlayerNumber, 6,-1},
{ 7,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_7,                 BTN_LABEL_COLOR,"",0,"",160, 167,50,15,NO_KEY,SetMultiPlayerNumber, 7,-1},
{ 8,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_8,                 BTN_LABEL_COLOR,"",0,"",210, 151,50,15,NO_KEY,SetMultiPlayerNumber, 8,-1},
{ 9,BUTTON_BITMAP,0,        0,STR_BACK1,  BTN_LABEL_COLOR,"UI\\MLTBTNBA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNBB.PCX", 32,196,84, 28,NO_KEY,BackMultiPlayerCreate, 0, -1},
{10,BUTTON_BITMAP,0,        0,STR_CANCEL1,BTN_LABEL_COLOR,"UI\\MLTBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNCB.PCX",116,196,84, 28,NO_KEY,CancelMultiPlayerCreate, 16, -1},
{11,BUTTON_BITMAP,0,        0,STR_HOST,   BTN_LABEL_COLOR,"UI\\MLTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNXB.PCX",200,196,84, 28,NO_KEY,HostMultiPlayerCreate, 0, -1},
{99,BUTTON_REGION,0,        0,-1,0,"",0,"",0,0,0,0,KEY_ESCAPE,BackMultiPlayerCreate, 16, -1},
};
LONG MultiCreateSize = sizeof(MultiCreate) / sizeof(BUTTON);

BUTTON MultiJoin[] = {
{ 0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\MLT_PNL4.PCX", 0,"", 162, 46, 316, 232,NO_KEY,PaintMultiPlayerJoin,0, -1},
{ 1,BUTTON_REGION,0,0,-1,0,"",0,"", 61, 100,200,15,NO_KEY,NULL, 0,-1},
{ 2,BUTTON_REGION,0,0,-1,0,"",0,"", 61, 130,200,15,KEY_ESCAPE,SetMultiPlayerJoin, 0, -1},
{ 3,BUTTON_REGION,0,0,-1,0,"",0,"", 61, 145,200,15,KEY_ESCAPE,SetMultiPlayerJoin, 1, -1},
{ 4,BUTTON_REGION,0,0,-1,0,"",0,"", 61, 160,200,15,KEY_ESCAPE,SetMultiPlayerJoin, 2, -1},
{ 5,BUTTON_REGION,0,0,-1,0,"",0,"", 61, 175,200,15,KEY_ESCAPE,SetMultiPlayerJoin, 3, -1},
{ 9,BUTTON_BITMAP,0,0,STR_BACK1,        BTN_LABEL_COLOR,"UI\\MLTBTNBA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNBB.PCX", 32,196,84, 28,NO_KEY,BackMultiPlayerJoin, 0, -1},
{10,BUTTON_BITMAP,0,0,STR_CANCEL1,      BTN_LABEL_COLOR,"UI\\MLTBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNCB.PCX",116,196,84, 28,NO_KEY,CancelMultiPlayerJoin, 16, -1},
{11,BUTTON_BITMAP,0,0,STR_DONE1,        BTN_LABEL_COLOR,"UI\\MLTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNXB.PCX",200,196,84, 28,NO_KEY,DoneMultiPlayerJoin, 0, -1},
{99,BUTTON_REGION,0,0,-1,0,"",0,"",0,0,0,0,KEY_ESCAPE,CancelMultiPlayerJoin, 16, -1},
};
LONG MultiJoinSize = sizeof(MultiJoin) / sizeof(BUTTON);

BUTTON MultiEnvironment[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\MLT_ENV.PCX", 0,"", 162, 46, 316, 200,NO_KEY,PaintMultiPlayerEnvironment,0, -1},
{1,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,  0,STR_MULTIMENU_INT,BTN_LABEL_COLOR, "",0,"", 63,103,200,15,NO_KEY,SetMultiPlayerEnvironment, 0,-1},
{2,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,  0,STR_MULTIMENU_LA, BTN_LABEL_COLOR, "",0,"", 63,120,200,15,NO_KEY,SetMultiPlayerEnvironment, 1,-1},
{3,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,  0,STR_MULTIMENU_WP, BTN_LABEL_COLOR, "",0,"", 63,137,200,15,NO_KEY,SetMultiPlayerEnvironment, 2,-1},
{10,BUTTON_BITMAP,0, 0,STR_CANCEL,       BTN_LABEL_COLOR,"UI\\MLTBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNCB.PCX", 73,164,84, 28,KEY_A,CancelMultiPlayerEnvironment, 0, -1},
{11,BUTTON_BITMAP,0, 0,STR_DONE,         BTN_LABEL_COLOR,"UI\\MLTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNXB.PCX",157,164,84, 28,KEY_F,DoneMultiPlayerEnvironment, 0, -1},
{12,BUTTON_REGION,0, 0,-1,0,"",0,"",0,0,0,0,KEY_ESCAPE,CancelMultiPlayerEnvironment, 0, -1},
};
LONG MultiEnvironmentSize = sizeof(MultiEnvironment) / sizeof(BUTTON);

BUTTON MultiHost[] = {
{ 0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\HST_PNL1.PCX", 0,"", 208, 100, 224, 264,NO_KEY,PaintMultiPlayerHost,0, -1},
{91,BUTTON_REGION,D_SANS_8,             0,STR_MULTITYPE_SEL, 200,            "",0,"", 20, 70, 82,14,NO_KEY,NULL, 1,-1}, // TITLE
#ifdef _WINDOWS
{ 1,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_MULTITYPE_NEW, BTN_LABEL_COLOR,"",0,"", 25, 93, 74,14,NO_KEY,SetMultiPlayerHostGame, iMPGT_NEW,-1}, // New
{ 2,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_MULTITYPE_LOAD,BTN_LABEL_COLOR,"",0,"", 25,107, 74,14,NO_KEY,SetMultiPlayerHostGame, iMPGT_LOAD,-1}, // Load
#endif
{92,BUTTON_REGION,D_SANS_8,             0,STR_TURNTIME,      200,            "",0,"", 20,130, 82,14,NO_KEY,NULL, 1,-1}, // TITLE
{ 3,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_TURNTIME_1,    BTN_LABEL_COLOR,"",0,"", 25,154, 74,14,NO_KEY,SetMultiPlayerHostTime, iTT_1Min,-1}, // Fast
{ 4,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_TURNTIME_2,    BTN_LABEL_COLOR,"",0,"", 25,168, 74,14,NO_KEY,SetMultiPlayerHostTime, iTT_2Min,-1}, // Med
{ 5,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_TURNTIME_3,    BTN_LABEL_COLOR,"",0,"", 25,182, 74,14,NO_KEY,SetMultiPlayerHostTime, iTT_5Min,-1}, // Slow
{ 6,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_TURNTIME_4,    BTN_LABEL_COLOR,"",0,"", 25,196, 74,14,NO_KEY,SetMultiPlayerHostTime, iTT_None,-1}, // None
{93,BUTTON_REGION,D_SANS_8,             0,STR_OPT_GAME_CTRL, 200,            "",0,"",120, 70, 82,14,NO_KEY,NULL, 1,-1}, // TITLE
{ 7,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_OPT_CTRL_MODE, BTN_LABEL_COLOR,"",0,"",125, 93, 74,14,NO_KEY,SetMultiPlayerHostDetail, 0,-1}, // Basic
{ 8,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_OPT_CTRL_MODE1,BTN_LABEL_COLOR,"",0,"",125,107, 74,14,NO_KEY,SetMultiPlayerHostDetail, 1,-1}, // Advanced
{ 9,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_OPT_CTRL_MODE2,BTN_LABEL_COLOR,"",0,"",125,121, 74,14,NO_KEY,SetMultiPlayerHostDetail, 2,-1}, // Expert
{94,BUTTON_REGION,D_SANS_8,             0,STR_OPT_ADV_DIF,   200,            "",0,"",120,145, 82,14,NO_KEY,NULL, 1,-1}, // TITLE
{10,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_OPT_DIF_EASY,  BTN_LABEL_COLOR,"",0,"",125,168, 74,14,NO_KEY,SetMultiPlayerHostDiff, 0,-1}, // Easy
{11,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_OPT_DIF_MEDIUM,BTN_LABEL_COLOR,"",0,"",125,182, 74,14,NO_KEY,SetMultiPlayerHostDiff, 1,-1}, // Medium
{12,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_OPT_DIF_HARD,  BTN_LABEL_COLOR,"",0,"",125,196, 74,14,NO_KEY,SetMultiPlayerHostDiff, 2,-1}, // Hard
//{13,BUTTON_BITMAP,D_SANS_8, 0,STR_CANCEL,        BTN_LABEL_COLOR,"UI\\HSTBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HSTBTN1B.PCX", 28,226, 84, 28,KEY_C,CancelMultiPlayerHost, 0, -1},
{14,BUTTON_BITMAP,D_SANS_8, 0,STR_DONE,          BTN_LABEL_COLOR,"UI\\HSTBTN2A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HSTBTN2B.PCX",70,226, 84, 28,KEY_F,DoneMultiPlayerHost, 0, -1},
{15,BUTTON_REGION,0, 0,-1,0,"",0,"", 0,0,0,0,KEY_ESCAPE,CancelMultiPlayerHost, 0, -1},
};
LONG MultiHostSize = sizeof(MultiHost) / sizeof(BUTTON);


BUTTON LoyaltyMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE, 0, -1, 0, "UI\\REPORT2.PCX", 0,"", 102, 6, 436, 468,NO_KEY,PaintLoyaltyTable,0, -1},
{1,BUTTON_REGION,0, 0,-1,                0, "",0,"", 0, 0,436,468,NO_KEY,DoneReportMenu, 0,-1},
{2,BUTTON_REGION,0, 0,-1,                0, "",0,"", 0, 0,0,0,KEY_ESCAPE,DoneReportMenu1, 1,-1},
};

LONG LoyaltyMenuSize = sizeof(LoyaltyMenu) / sizeof(BUTTON);

BUTTON DiplomacyAction[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,0,"UI\\HELP_LG.PCX",0,"",160, 50,356,372,NO_KEY,PaintOthersDiplomacy,0, -1},
{1,BUTTON_REGION,0,              0,-1,         0,"",0,"", 24, 24,305,320,NO_KEY,0, 0, -1},
{2,BUTTON_REGION,D_HILIGHTED,0,-1, 0,"",0,"UI\\HLPBTNAA.PCX",  28,340, 100, 28,KEY_W,NeverOthersDiplomacy, 0, -1},
{3,BUTTON_REGION,D_HILIGHTED,0,-1,   0,"",0,"UI\\HLPBTNAA.PCX", 128,340, 100, 28,KEY_A,YesOthersDiplomacy, 0, -1},
{4,BUTTON_REGION,D_HILIGHTED,0,-1,    0,"",0,"UI\\HLPBTNAA.PCX", 228,340, 100, 28,KEY_V,NoOthersDiplomacy, 0, -1},
{5,BUTTON_REGION,0,    0,-1,   0,"",0,"",0,0,0,0,KEY_ESCAPE,YesOthersDiplomacy, 0, -1},
};

LONG DiplomacyActionSize = sizeof(DiplomacyAction) / sizeof(BUTTON);

BUTTON BattleMagicPrep[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,          0,"UI\\BTLSEL.PCX",0,"",   250, 80,140,280,NO_KEY,PaintBattleMagicPrep,0, -1},
{1,BUTTON_BITMAP,0,    0,STR_DONE,  BTN_LABEL_COLOR,"UI\\SELBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\SELBTN1B.PCX",  28,244, 84, 28,KEY_F,BattlePrepProc, 0, -1},
{2,BUTTON_REGION,0,    0,-1,          0,"",0,"",0,0,0,0,KEY_ESCAPE,BattlePrepProc, 0, -1},
};
LONG BattleMagicPrepSize = sizeof(BattleMagicPrep) / sizeof(BUTTON);

BUTTON BattleCommand[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,0,"UI\\HELP_LG.PCX",0,"",160, 50,356,372,NO_KEY,SetCommandBattlePaint, 0, -1},
{1,BUTTON_REGION,  0,0,-1,                 0,              "",                0,                     "", 25,100,290,240,NO_KEY,0, 0, -1},
{333,BUTTON_BITMAP,0,0,STR_YES,            BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"", 28,336,100, 28,KEY_J,SetCommandBattleProc, QUESTION_BTN1, -1},
{334,BUTTON_BITMAP,0,0,COMBAT_STR_RETREAT, BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"",128,336,100, 28,KEY_R,SetCommandBattleProc, QUESTION_BTN2, -1},
{335,BUTTON_BITMAP,0,0,STR_NO,             BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"",228,336,100, 28,KEY_N,SetCommandBattleProc, QUESTION_BTN3, -1},
};
LONG BattleCommandSize = sizeof(BattleCommand) / sizeof(BUTTON);

BUTTON BuildArmys[] = {
{0,BUTTON_BITMAP,D_FREEZE,  0,-1, 0,"UI\\BATLPICK.PCX",0,"",0,0,640,480,NO_KEY,BuildArmyPaint,0,-1},
{9,BUTTON_REGION,D_TEXT_LEFT,0,STR_BB_TITLE,31,"",0,"", 8, 6, 150,55,NO_KEY,NULL, 0, -1},
// my button and list box with controls
{9,           BUTTON_REGION,0,0,STR_BB_MYLIST,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"", 18, 68,130,15,NO_KEY,NULL, 0, -1},
{BB_MYARMY1,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18, 87,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY2,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,103,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY3,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,119,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY4,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,135,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY5,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,151,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY6,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,167,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY7,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,183,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY8,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,199,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY9,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,215,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY10, BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,231,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY11, BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,247,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY12, BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,263,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY13, BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,279,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY14, BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,295,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY15, BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,311,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MY_ADD,   BUTTON_BITMAP,0,0,STR_BB_ADD,   BTN_LABEL_COLOR,"UI\\BTPBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN1B.PCX", 12,332,144,28,NO_KEY,BuildListAdd, 0, -1},
{BB_MY_REMOVE,BUTTON_BITMAP,0,0,STR_BB_REM,   BTN_LABEL_COLOR,"UI\\BTPBTN2A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN2B.PCX", 12,360,144,28,NO_KEY,BuildListRemove,0, -1},
// enemy button and listbox with controls
{9,            BUTTON_REGION,0,0,STR_BB_HISLIST,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"",490, 68,130,15,NO_KEY,NULL, 0, -1},
{BB_HISARMY1,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490, 87,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY2,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,103,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY3,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,119,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY4,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,135,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY5,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,151,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY6,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,167,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY7,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,183,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY8,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,199,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY9,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,215,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY10, BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,231,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY11, BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,247,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY12, BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,263,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY13, BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,279,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY14, BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,295,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY15, BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,311,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HIS_ADD,   BUTTON_BITMAP,0,0,STR_BB_ADD,    BTN_LABEL_COLOR,"UI\\BTPBTN5A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN5B.PCX",484,332,144,28,NO_KEY,BuildListAdd, 0, -1},
{BB_HIS_REMOVE,BUTTON_BITMAP,0,0,STR_BB_REM,    BTN_LABEL_COLOR,"UI\\BTPBTN6A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN6B.PCX",484,360,144,28,NO_KEY,BuildListRemove,0, -1},

{BB_DISPLAY,BUTTON_REGION,0,0,STR_BB_TROOPS,    BTN_LABEL_COLOR,"UI\\BTPBTNZA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTNZB.PCX",248,416,144,28,NO_KEY,BuildDisplayToggle,0, -1},

{BB_PLAINS,    BUTTON_BITMAP,0,0,STR_BB_PLAINS,    BTN_LABEL_COLOR,"UI\\BTPBTNPA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTNPB.PCX",160,448,80, 24,NO_KEY,BuildTerrainSel, 0, -1},
{BB_MOUNTAINS, BUTTON_BITMAP,0,0,STR_BB_MOUNTAINS, BTN_LABEL_COLOR,"UI\\BTPBTNMA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTNMB.PCX",240,448,80, 24,NO_KEY,BuildTerrainSel, 1, -1},
{BB_FOREST,    BUTTON_BITMAP,0,0,STR_BB_FOREST,    BTN_LABEL_COLOR,"UI\\BTPBTNFA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTNFB.PCX",320,448,80, 24,NO_KEY,BuildTerrainSel, 2, -1},
{BB_SWAMP,     BUTTON_BITMAP,0,0,STR_BB_SWAMP,     BTN_LABEL_COLOR,"UI\\BTPBTNSA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTNSB.PCX",400,448,80, 24,NO_KEY,BuildTerrainSel, 3, -1},

{BB_RESTART,   BUTTON_BITMAP,0,0,STR_BB_RESTART,   BTN_LABEL_COLOR,"UI\\BTPBTN3A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN3B.PCX", 12,416,144, 28,NO_KEY,BuildArmyRestart,0, -1},
{BB_RELOAD,    BUTTON_BITMAP,0,0,STR_BB_RELOAD,    BTN_LABEL_COLOR,"UI\\BTPBTN4A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN4B.PCX", 12,444,144, 28,NO_KEY,BuildArmyReload, 0, -1},
{BB_PRACTICE,  BUTTON_BITMAP,0,0,STR_BB_PRACTICE,  BTN_LABEL_COLOR,"UI\\BTPBTN7A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN7B.PCX",484,416,144, 28,NO_KEY,BuildArmyDone,   0, -1},
{BB_FIGHT,     BUTTON_BITMAP,0,0,STR_BB_FIGHT,     BTN_LABEL_COLOR,"UI\\BTPBTN8A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTNBTN8B.PCX",484,444,144, 28,NO_KEY,BuildArmyDone,   0, -1},
};
LONG BuildArmysSize = sizeof(BuildArmys) / sizeof(BUTTON);

BUTTON BuildAdv[] = {
{0,BUTTON_BITMAP,D_FREEZE,   0,-1,           0,"UI\\BUILDADV.PCX",0,"",0,0,640,480,NO_KEY,BuildAdvPaint,0,-1},
{9,BUTTON_REGION,0,                0,STR_BA_TITLE,   31,"",0,"", 15,  2,170,20,NO_KEY,NULL, 0, -1},
// adventure list box 
{9,BUTTON_REGION,D_TEXT_LEFT,    0,STR_CS_SELECT,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"", 16, 55,176,25,NO_KEY,NULL, 0, -1},
{BA_LIST_01,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16, 95,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_02,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,110,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_03,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,125,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_04,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,140,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_05,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,155,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_06,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,170,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_07,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,185,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_08,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,200,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{9,BUTTON_REGION,D_TEXT_LEFT,    0,STR_CS_CHOOSE,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"", 16,230,176,25,NO_KEY,NULL, 0, -1},
{9,BUTTON_REGION,D_TEXT_LEFT,    0,STR_BA_CHOOSE,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"", 16,257,176, 8,NO_KEY,NULL, 0, -1},
{BA_LIST_09,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,270,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_10,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,285,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_11,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,300,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_12,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,315,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_13,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,330,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_14,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,345,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_15,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,360,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_16,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,375,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_RESET,      BUTTON_BITMAP,0,0,STR_BA_RESET,  BTN_LABEL_COLOR,"UI\\ADBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\ADBTN1B.PCX", 32,412,144,28,NO_KEY,BuildAdvRestart, 0, -1},
{BA_DONE,             BUTTON_BITMAP,0,0,STR_BA_DONE,   BTN_LABEL_COLOR,"UI\\ADBTN2A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\ADBTN2B.PCX", 32,444,144,28,NO_KEY,BuildAdvDone, 0, -1},
};
LONG BuildAdvSize = sizeof(BuildAdv) / sizeof(BUTTON);

BUTTON AdvPrep[] = {
{0,BUTTON_BITMAP,D_FREEZE,   0,-1,           0,"UI\\ADVPREP.PCX",0,"",0,0,640,480,NO_KEY,AdvPrepPaint,0,-1},
{9,BUTTON_REGION,0,                0,STR_BA_TITLE,   31,"",0,"", 15,  2,170,20,NO_KEY,NULL, 0, -1},
// adventure list box 
{9,BUTTON_REGION,D_TEXT_LEFT,    0,STR_CS_SELECT,173,"",0,"", 16, 55,176,25,NO_KEY,NULL, 0, -1},
{BA_LIST_01,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16, 95,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_02,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,110,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_03,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,125,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_04,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,140,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_05,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,155,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_06,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,170,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_07,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,185,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_08,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,200,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{9,BUTTON_REGION,D_TEXT_LEFT,    0,STR_CS_CHOOSE,173,"",0,"", 16,230,176,25,NO_KEY,NULL, 0, -1},
{9,BUTTON_REGION,D_TEXT_LEFT,    0,STR_BA_CHOOSE,173,"",0,"", 16,257,176, 8,NO_KEY,NULL, 0, -1},
{BA_LIST_09,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,270,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_10,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,285,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_11,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,300,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_12,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,315,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_13,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,330,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_14,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,345,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_15,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,360,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_16,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,375,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_RESET,      BUTTON_BITMAP,0,0,STR_CANCEL,    BTN_LABEL_COLOR,"UI\\ADBTN1A.PCX", 31,"UI\\ADBTN1B.PCX",  32,412,144,28,KEY_A,AdvPrepCancel, 0, -1},
{BA_DONE,       BUTTON_BITMAP,0,0,STR_BA_DONE,   BTN_LABEL_COLOR,"UI\\ADBTN2A.PCX", 31,"UI\\ADBTN2B.PCX",  32,444,144,28,NO_KEY,AdvPrepDone, 0, -1},
{ADV_LIST1_UP,  BUTTON_BITMAP,0,0,-1,            0,"UI\\ADVTNU1A.PCX",0,"UI\\ADBTNU1B.PCX",192, 76, 24,60,NO_KEY,AdvPrepListScroll, 1, -1},
{ADV_LIST1_DOWN,BUTTON_BITMAP,0,0,-1,            0,"UI\\ADVTND1A.PCX",0,"UI\\ADBTND1B.PCX",192,136, 24,60,NO_KEY,AdvPrepListScroll, 2, -1},
{ADV_LIST2_UP,  BUTTON_BITMAP,0,0,-1,            0,"UI\\ADBTNU1A.PCX",0,"UI\\ADBTNU2B.PCX",192,256, 24,60,NO_KEY,AdvPrepListScroll, 3, -1},
{ADV_LIST2_DOWN,BUTTON_BITMAP,0,0,-1,            0,"UI\\ADBTND1A.PCX",0,"UI\\ADBTND2B.PCX",192,316, 24,60,NO_KEY,AdvPrepListScroll, 4, -1},
};
LONG AdvPrepSize = sizeof(AdvPrep) / sizeof(BUTTON);

BUTTON GameSelMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE,0,-1,             		  0,"UI\\HELPPNL1.PCX",0,"",200, 75,351,371,NO_KEY,PaintGameSelect,0, -1},
{1,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_GAMETYPE,     31,"",0,"", 48, 80,152, 20,NO_KEY,NULL, 0, -1},
{2,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_HELP,        173,"",31,"", 48,114,152, 20,KEY_K,SelectGameType, 0, -1},
{3,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_INTROGAME,   173,"",31,"", 48,134,152, 20,KEY_T,SelectGameType, 1, -1},
{4,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_FULLGAME,    173,"",31,"", 48,154,152, 20,KEY_V,SelectGameType, 2, -1},
{5,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_ADVENTURE,   173,"",31,"", 48,174,152, 20,KEY_B,SelectGameType, 3, -1},
{6,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_BATTLE,      173,"",31,"", 48,194,152, 20,KEY_S,SelectGameType, 4, -1},
{7,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_BACKGROUND,  173,"",31,"", 48,214,152, 20,KEY_H,SelectGameType, 5, -1},
{8,BUTTON_REGION,0,       	   0,-1,             		   0,"",0,"", 44,230,152, 20,NO_KEY,NULL, 0, -1},
{9,BUTTON_BITMAP,0,			   0,STR_CANCEL,			 BTN_LABEL_COLOR,"",31,"UI\\HLPBTNAB.PCX", 76,252, 84, 28,KEY_A ,HideSubMenuWithClick, D_SELECT_GAME, -1},
{10,BUTTON_REGION,0,      0,-1,             		  0,"",0,"",  0,  0,  0,  0,KEY_ESCAPE,HideSubMenu, D_SELECT_GAME, -1},
};
LONG GameSelMenuSize = sizeof(GameSelMenu) / sizeof(BUTTON);

BUTTON GameCmplxMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE,        0,-1,             0,"UI\\DIALPNL1.PCX",0,"", 202,136,236,208,NO_KEY,PaintGameComplexity,0, -1},
{1,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,0,STR_SELECT_COMPLEXITY, 31,"",0,"", 48, 80,152, 20,NO_KEY,NULL, 0, -1},
{2,BUTTON_REGION,0,               0,-1,             173,"",           31,"", 44,109,152, 20,KEY_E,SelectGameType, 10, -1},
{3,BUTTON_REGION,0,               0,-1,             173,"",           31,"", 44,129,152, 20,KEY_F,SelectGameType, 11, -1},
{4,BUTTON_REGION,0,               0,-1,             173,"",           31,"", 44,149,152, 20,KEY_X,SelectGameType, 12, -1},
{9,BUTTON_BITMAP,0,     		  0,STR_CANCEL,BTN_LABEL_COLOR,  "UI\\HLPBTNCA.PCX",31,"UI\\HLPBTNCB.PCX", 76,172, 84, 28,KEY_A ,HideSubMenuWithClick, D_GAME_COMPLEXITY, -1},
{10,BUTTON_REGION,0,              0,-1,              0,"",0,"",  0,  0,  0,  0,KEY_ESCAPE,HideSubMenu, D_GAME_COMPLEXITY, -1},
};
LONG GameCmplxMenuSize = sizeof(GameCmplxMenu) / sizeof(BUTTON);



BUTTON MultiWait[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\CHATPNL2.PCX",0,"", 83, 80, 472, 320,NO_KEY, 0, 0, -1},
{1,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 100,440,15,NO_KEY,0, 0,-1},
{2,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 116,440,15,NO_KEY,0, 1,-1},
{3,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 132,440,15,NO_KEY,0, 2,-1},
{4,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 148,440,15,NO_KEY,0, 3,-1},
{5,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 164,440,15,NO_KEY,0, 4,-1},
{6,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 180,440,15,NO_KEY,0, 5,-1},
{7,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 196,440,15,NO_KEY,0, 6,-1},
//{8,BUTTON_BITMAP,0, 0,STR_CANCEL1, BTN_LABEL_COLOR,"UI\\CHTBTN2A.PCX",31,"UI\\CHTBTN2B.PCX", 196, 220, 84, 28,KEY_ESCAPE,HideSubMenuWithClick, D_MULTIWAIT, -1},
};
LONG MultiWaitSize = sizeof(MultiWait) / sizeof(BUTTON);

BUTTON DescribeMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE,        0,-1,             0,"",0,                "",160,120,320,264, NO_KEY,    PaintDescribe, 0, -1},
{1,BUTTON_REGION,D_HILIGHTED,     0,-1,             0,"",0,"UI\\MARBBTNA.PCX",134,240, 52, 16, KEY_SPACE, IncDescribe,   0, -1},
};
LONG DescribeMenuSize = sizeof(DescribeMenu) / sizeof(BUTTON);

BUTTON SystemLargeHelp[] = {
{SYSHELP_DIALOG,  BUTTON_BITMAP,D_DONT_SHADE|D_FREEZE|D_ZOOM, 0,-1,   0,"UI\\help_lg.PCX",  0,"",145, 50,356,372,NO_KEY,SystemHelpPaintProc,0, -1},
{SYSHELP_TITLE,   BUTTON_REGION,0,               0,-1,                0,"",                 0,"", 15, 10,326, 20,NO_KEY,NULL,0, -1},
{SYSHELP_TEXTBODY,BUTTON_REGION,0,               0,-1,                0,"",                 0,"", 15, 35,326,300,NO_KEY,NULL,0, -1},
{SYSHELP_DONE,    BUTTON_BITMAP,0,               0,STR_HELP_DONE,    BTN_LABEL_COLOR,"UI\\hlpbtnaa.pcx",31,"UI\\hlpbtnab.pcx", 40,339,100, 28,KEY_F ,HideSubMenuWithClick, D_SYSHELP_LARGE, -1},
{SYSHELP_MOREINFO,BUTTON_BITMAP,0,               0,STR_HELP_MOREINFO,BTN_LABEL_COLOR,"UI\\hlpbtnaa.pcx",31,"UI\\hlpbtnab.pcx",224,339,100, 28,KEY_M ,NULL,0, -1},
};
LONG SystemLargeHelpSize = sizeof(SystemLargeHelp) / sizeof(BUTTON);

BUTTON SystemMedHelp[] = {
{SYSHELP_DIALOG,  BUTTON_BITMAP,D_DONT_SHADE|D_FREEZE|D_ZOOM, 0,-1,   0,"UI\\help_md.PCX", 0,"",145,150,356,188,NO_KEY,SystemHelpPaintProc,0, -1},
{SYSHELP_TITLE,   BUTTON_REGION,0,               0,-1,                0,"",                0,"", 15, 10,326, 20,NO_KEY,NULL,0, -1},
{SYSHELP_TEXTBODY,BUTTON_REGION,0,               0,-1,                0,"",                0,"", 15, 35,326,115,NO_KEY,NULL,0, -1},
{SYSHELP_DONE,    BUTTON_BITMAP,0,               0,STR_HELP_DONE,    BTN_LABEL_COLOR,"UI\\hlpbtnaa.pcx",31,"UI\\hlpbtnab.pcx", 40,155,100, 28,KEY_F ,HideSubMenuWithClick, D_SYSHELP_MED, -1},
{SYSHELP_MOREINFO,BUTTON_BITMAP,0,               0,STR_HELP_MOREINFO,BTN_LABEL_COLOR,"UI\\hlpbtnaa.pcx",31,"UI\\hlpbtnab.pcx",224,155,100, 28,KEY_M ,NULL,0, -1},
};
LONG SystemMedHelpSize = sizeof(SystemMedHelp) / sizeof(BUTTON);

BUTTON SystemSmallHelp[] = {
{SYSHELP_DIALOG,  BUTTON_BITMAP,D_DONT_SHADE|D_FREEZE|D_ZOOM, 0,-1,   0,"UI\\help_sm.PCX",  0,"",145,250,356,112,NO_KEY,SystemHelpPaintProc,0, -1},
{SYSHELP_TITLE,   BUTTON_REGION,0,               0,-1,                0,"",                 0,"", 15,  8,326, 20,NO_KEY,NULL,0, -1},
{SYSHELP_TEXTBODY,BUTTON_REGION,0,               0,-1,                0,"",                 0,"", 15, 35,326, 48,NO_KEY,NULL,0, -1},
{SYSHELP_DONE,    BUTTON_BITMAP,0,               0,STR_HELP_DONE,    BTN_LABEL_COLOR,"UI\\hlpbtnaa.pcx",31,"UI\\hlpbtnab.pcx", 40, 79,100, 28,KEY_F ,HideSubMenuWithClick, D_SYSHELP_SMALL, -1},
{SYSHELP_MOREINFO,BUTTON_BITMAP,0,               0,STR_HELP_MOREINFO,BTN_LABEL_COLOR,"UI\\hlpbtnaa.pcx",31,"UI\\hlpbtnab.pcx",224, 79,100, 28,KEY_M ,NULL,0, -1},
};
LONG SystemSmallHelpSize = sizeof(SystemSmallHelp) / sizeof(BUTTON);

BUTTON TutorialMenuWide[] = {
{TUTORIAL_MENU_BACKGROUND,  BUTTON_BITMAP,D_FREEZE, 0,-1,           0,"UI\\MARBLE06.PCX", 0,"",100, 50,321,263,NO_KEY,PaintTutorial,0, -1},
{TUTORIAL_MENU_HEAD,   BUTTON_BITMAP,0,             0,-1,           0,"UI\\HEADCNSL.PCX", 0,"",-24,-50, 64, 96,NO_KEY,NULL,0, -1},
{TUTORIAL_MENU_TEXT,   BUTTON_REGION,0,             0,-1,           0,                "", 0,"", 10, 46,302,170,NO_KEY,NULL,0, -1},
{TUTORIAL_MENU_DONE,   BUTTON_BITMAP,0,             0,STR_HELP_DONE,0,"UI\\MARBBTNA.PCX",31,"UI\\MARBBTNB.PCX",135,231, 51, 15,KEY_F,TutorialDone, 0, -1},
};

LONG TutorialMenuWideSize = sizeof(TutorialMenuWide) / sizeof(BUTTON);

BUTTON TutorialMenuMed[] = {
{TUTORIAL_MENU_BACKGROUND,  BUTTON_BITMAP,D_FREEZE, 0,-1,           0,"UI\\MARBLE03.PCX",  0,"",100, 50,199,161,NO_KEY,PaintTutorial,0, -1},
{TUTORIAL_MENU_HEAD,   BUTTON_BITMAP,0,             0,-1,           0,"UI\\HEADCNSL.PCX",  0,"",-24,-50, 64, 96,NO_KEY,NULL,0, -1},
{TUTORIAL_MENU_TEXT,   BUTTON_REGION,0,             0,-1,           0,                "",  0,"", 10, 46,179, 79,NO_KEY,NULL,0, -1},
{TUTORIAL_MENU_DONE,   BUTTON_BITMAP,0,             0,STR_HELP_DONE,0,"UI\\MARBBTNA.PCX", 31,"UI\\MARBBTNB.PCX", 75,140, 51, 15,KEY_F,TutorialDone, 0, -1},
};

LONG TutorialMenuWideMed = sizeof(TutorialMenuMed) / sizeof(BUTTON);

BUTTON LoadingScene[] = {
{0,BUTTON_BITMAP,D_NO_PUSH,0,-1,0,"UI\\help_sm.PCX",0,"",145,250,356,112,NO_KEY,LoadingSceneProc,0, -1},
};
LONG LoadingSceneSize = sizeof(LoadingScene) / sizeof(BUTTON);

BUTTON Parchment[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0, -1,         0,"UI\\HNTPANL1.PCX",0,"",   154,60,352,370,NO_KEY,NULL,0, -1},
{PARCHMENT_TEXT,  BUTTON_REGION,0,0,-1,         1,"",0,"",38,118,216,116,NO_KEY,NULL,0, -1},
{PARCHMENT_CLICK, BUTTON_REGION,0,0,STR_CLICK,204,"",0,"", 0,330,352, 28,NO_KEY,NULL,0, -1},
{PARCHMENT_OK_BTN,BUTTON_REGION,0,0,-1,         0,"",0,"", 0,  0,352,370,KEY_ENTER,ParchmentDone,0, -1},
};
LONG ParchmentSize = sizeof(Parchment) / sizeof(BUTTON);

#else 
#if defined (_FRENCHVER)

BUTTON AvatarAttributes[] = {
{ 0,BUTTON_BITMAP, D_FREEZE|D_ZOOM, 0,-1, 0,"UI\\STS_PANL.PCX",0,"",112, 20,368,436,NO_KEY,PaintAttributes,0,-1},

{ 1,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                 31,"",0,"", 42, 21,223, 32,NO_KEY,NULL,0,-1},
{ 2,BUTTON_BITMAP, 0,           0,STR_AVA_ATT_ANIM,  BTN_LABEL_COLOR,"UI\\STSBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\STSBTNAB.PCX", 52,400, 84, 28,KEY_A,PlayAnimation,0,-1},
{ 3,BUTTON_BITMAP, 0,           0,-1,                BTN_LABEL_COLOR,"UI\\STSBTNIA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\STSBTNIB.PCX",188,400, 84, 28,KEY_I,InventoryFromStatus,0,-1},
{ 4,BUTTON_BITMAP, 0,           0,STR_AVA_ATT_DONE,  BTN_LABEL_COLOR,"UI\\STSBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\STSBTNXB.PCX",272,400, 84, 28,KEY_O,HideStatus,0,-1},
{ 5,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                 31,"",0,"",184, 81,158, 10,NO_KEY,NULL,0,-1},
{ 6,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_CLASS, 173,"",0,"",184,112,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_CLASS,-1},
{ 7,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_LEVEL, 173,"",0,"",184,128,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_LEVEL,-1},
{ 8,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_BLOOD, 173,"",0,"",184,144,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_BLOOD,-1},
{ 9,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_ALIGN, 173,"",0,"",184,159,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_ALIGN,-1},
{10,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_STR,   173,"",0,"",184,200,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_STR,-1},
{11,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_INT,   173,"",0,"",184,216,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_INT,-1},
{12,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_DEX,   173,"",0,"",184,232,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_DEX,-1},
{13,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_WIS,   173,"",0,"",184,248,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_WIS,-1},
{14,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_CON,   173,"",0,"",184,264,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_CON,-1},
{15,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_CHA,   173,"",0,"",184,280,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_CHA,-1},
{16,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_THA,   173,"",0,"",184,296,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_THAC0,-1},
{17,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_EXP,   173,"",0,"",184,328,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_XP ,-1},
{18,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_ARM,   173,"",0,"",184,344,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_PROT,-1},
{19,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_HIT,   173,"",0,"",184,360,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_HP,-1},
{20,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_HP,    173,"",0,"",184,376,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_CURRHP,-1},
{21,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                  0,"",0,"", 16,108,152,204,NO_KEY,BowToPlayer,0,-1},
{22,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                  0,"",0,"", 20,329,134, 20,NO_KEY,NULL,0,-1},
{23,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                  0,"",0,"",  0,  0,  0,  0,KEY_ESCAPE,HideStatus,0,-1},
};
LONG AvatarAttributesSize = sizeof(AvatarAttributes) / sizeof(BUTTON);


BUTTON AvatarAttributes2[] = {
{ 0,BUTTON_BITMAP, D_FREEZE|D_ZOOM, 0,-1, 0,"UI\\STS_PANL.PCX",0,"",100, 20,368,436,NO_KEY,PaintAttributes,0,-1},

{ 1,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                 31,"",0,"", 42, 21,223, 32,NO_KEY,NULL,0,-1},
{ 2,BUTTON_BITMAP, 0,			0,STR_AVA_ATT_ANIM,  BTN_LABEL_COLOR,"UI\\STSBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\STSBTNAB.PCX", 52,400, 84, 28,KEY_A,PlayAnimation,0,-1},
{ 3,BUTTON_REGION, 0,			0,-1,                0,"",0,"",188,400, 84, 28,NO_KEY,NULL,0,-1},
{ 4,BUTTON_BITMAP, 0,			0,STR_AVA_ATT_DONE,  BTN_LABEL_COLOR,"UI\\STSBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\STSBTNXA.PCX",272,400, 84, 28,KEY_O,HideStatus2,0,-1},
{ 5,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                 31,"",0,"",184, 81,158, 10,NO_KEY,NULL,0,-1},
{ 6,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_CLASS, 173,"",0,"",184,112,158, 10,NO_KEY,NULL,0,-1},
{ 7,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_LEVEL, 173,"",0,"",184,128,158, 10,NO_KEY,NULL,0,-1},
{ 8,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_BLOOD, 173,"",0,"",184,144,158, 10,NO_KEY,NULL,0,-1},
{ 9,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_ALIGN, 173,"",0,"",184,159,158, 10,NO_KEY,NULL,0,-1},
{10,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_STR,   173,"",0,"",184,200,158, 10,NO_KEY,NULL,0,-1},
{11,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_INT,   173,"",0,"",184,216,158, 10,NO_KEY,NULL,0,-1},
{12,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_DEX,   173,"",0,"",184,232,158, 10,NO_KEY,NULL,0,-1},
{13,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_WIS,   173,"",0,"",184,248,158, 10,NO_KEY,NULL,0,-1},
{14,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_CON,   173,"",0,"",184,264,158, 10,NO_KEY,NULL,0,-1},
{15,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_CHA,   173,"",0,"",184,280,158, 10,NO_KEY,NULL,0,-1},
{16,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_THA,   173,"",0,"",184,296,158, 10,NO_KEY,NULL,0,-1},
{17,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_EXP,   173,"",0,"",184,328,158, 10,NO_KEY,NULL,0,-1},
{18,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_ARM,   173,"",0,"",184,344,158, 10,NO_KEY,NULL,0,-1},
{19,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_HIT,   173,"",0,"",184,360,158, 10,NO_KEY,NULL,0,-1},
{20,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_HP,    173,"",0,"",184,376,158, 10,NO_KEY,NULL,0,-1},
{21,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                  0,"",0,"", 16,108,152,204,NO_KEY,BowToPlayer,0,-1},
{22,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                  0,"",0,"", 20,329,134, 20,NO_KEY,NULL,0,-1},
{23,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                  0,"",0,"",  0,  0,  0,  0,KEY_ESCAPE,HideStatus2,0,-1},
};
LONG AvatarAttributes2Size = sizeof(AvatarAttributes2) / sizeof(BUTTON);



BUTTON AvatarInventory[] = {
{INV_BUT_ALL      ,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,0,    "UI\\INV_PANL.PCX",0,"",100, 50,392,348,	NO_KEY,	StaticInventoryGUI::mfPaint,				0,			-1},
{INV_BUT_TITLE    ,BUTTON_REGION,D_TEXT_LEFT,0,STR_AVA_INV_INV,173,   "",0,"",                 42, 21,223, 32,	KEY_I,	NULL,										0,			-1},
{INV_BUT_DONE     ,BUTTON_BITMAP,0,0,STR_AVA_INV_DONE,BTN_LABEL_COLOR,  "UI\\INVBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTNXB.PCX",300,312, 84, 28,	KEY_O,	StaticInventoryGUI::mfDone,					0,			-1},
{INV_BUT_STATUS   ,BUTTON_BITMAP,0,0,STR_AVA_INV_STATUS,BTN_LABEL_COLOR,"UI\\INVBTNSA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTNSB.PCX",216,312, 84, 28,	KEY_S,	StaticInventoryGUI::mfGoStatus,				0,			-1},
{INV_BUT_DUDENAME, BUTTON_REGION,0,0,-1,0,"",0,"",					125,	80, 	230,	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_INFO,     BUTTON_REGION,0,0,-1,0,"",0,"",					120,	240,	240,	45, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_HELPAST,  BUTTON_REGION,0,0,STR_AVA_INV_HELP_ASTERISK,173,"",0,"",120,285,	240,	15, 	NO_KEY,	NULL,										0,			-1}, //context sensitive help here?
{INV_BUT_LISTBOX,  BUTTON_REGION,0,0,-1,0,"",0,"",					120,	108,	239,	119,	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNUMBER1,BUTTON_REGION,0,0,-1,0,"",0,"",					152,	107,	19, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNUMBER2,BUTTON_REGION,0,0,-1,0,"",0,"",					152,	127,	19, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNUMBER3,BUTTON_REGION,0,0,-1,0,"",0,"",					152,	147,	19, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNUMBER4,BUTTON_REGION,0,0,-1,0,"",0,"",					152,	167,	19, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNUMBER5,BUTTON_REGION,0,0,-1,0,"",0,"",					152,	187,	19, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNUMBER6,BUTTON_REGION,0,0,-1,0,"",0,"",					152,	207,	19, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBICON1,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	107,	29, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBICON2,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	127,	29, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBICON3,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	147,	29, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBICON4,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	167,	29, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBICON5,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	187,	29, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBICON6,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	207,	29, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNAME1,  BUTTON_REGION,0,0,-1,0,"",0,"",					171,	107,	188,	20, 	NO_KEY,	StaticInventoryGUI::mfSetHighlightedLine,	0,			-1},
{INV_BUT_LBNAME2,  BUTTON_REGION,0,0,-1,0,"",0,"",					171,	127,	188,	20, 	NO_KEY,	StaticInventoryGUI::mfSetHighlightedLine,	1,			-1},
{INV_BUT_LBNAME3,  BUTTON_REGION,0,0,-1,0,"",0,"",					171,	147,	188,	20, 	NO_KEY,	StaticInventoryGUI::mfSetHighlightedLine,	2,			-1},
{INV_BUT_LBNAME4,  BUTTON_REGION,0,0,-1,0,"",0,"",					171,	167,	188,	20, 	NO_KEY,	StaticInventoryGUI::mfSetHighlightedLine,	3,			-1},
{INV_BUT_LBNAME5,  BUTTON_REGION,0,0,-1,0,"",0,"",					171,	187,	188,	20, 	NO_KEY,	StaticInventoryGUI::mfSetHighlightedLine,	4,			-1},
{INV_BUT_LBNAME6,  BUTTON_REGION,0,0,-1,0,"",0,"",					171,	207,	188,	20, 	NO_KEY,	StaticInventoryGUI::mfSetHighlightedLine,	5,			-1},
{INV_BUT_LBPLUS1,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	107,	15, 	20, 	NO_KEY,	StaticInventoryGUI::mfAddToAvailSpells,		0,			-1},
{INV_BUT_LBPLUS2,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	127,	15, 	20, 	NO_KEY,	StaticInventoryGUI::mfAddToAvailSpells,		1,			-1},
{INV_BUT_LBPLUS3,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	147,	15, 	20, 	NO_KEY,	StaticInventoryGUI::mfAddToAvailSpells,		2,			-1},
{INV_BUT_LBPLUS4,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	167,	15, 	20, 	NO_KEY,	StaticInventoryGUI::mfAddToAvailSpells,		3,			-1},
{INV_BUT_LBPLUS5,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	187,	15, 	20, 	NO_KEY,	StaticInventoryGUI::mfAddToAvailSpells,		4,			-1},
{INV_BUT_LBPLUS6,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	207,	15, 	20, 	NO_KEY,	StaticInventoryGUI::mfAddToAvailSpells,		5,			-1},
{INV_BUT_LBMINUS1, BUTTON_REGION,0,0,-1,0,"",0,"",					138,	107,	14, 	20, 	NO_KEY,	StaticInventoryGUI::mfTakeFromAvailSpells,	0,			-1},
{INV_BUT_LBMINUS2, BUTTON_REGION,0,0,-1,0,"",0,"",					138,	127,	14, 	20, 	NO_KEY,	StaticInventoryGUI::mfTakeFromAvailSpells,	1,			-1},
{INV_BUT_LBMINUS3, BUTTON_REGION,0,0,-1,0,"",0,"",					138,	147,	14, 	20, 	NO_KEY,	StaticInventoryGUI::mfTakeFromAvailSpells,	2,			-1},
{INV_BUT_LBMINUS4, BUTTON_REGION,0,0,-1,0,"",0,"",					138,	167,	14, 	20, 	NO_KEY,	StaticInventoryGUI::mfTakeFromAvailSpells,	3,			-1},
{INV_BUT_LBMINUS5, BUTTON_REGION,0,0,-1,0,"",0,"",					138,	187,	14, 	20, 	NO_KEY,	StaticInventoryGUI::mfTakeFromAvailSpells,	4,			-1},
{INV_BUT_LBMINUS6, BUTTON_REGION,0,0,-1,0,"",0,"",					138,	207,	14, 	20, 	NO_KEY,	StaticInventoryGUI::mfTakeFromAvailSpells,	5,			-1},
{INV_BUT_MUNDANE,  BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN1B.PCX",	12, 	108,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfChangeCurrCat,		IC_MUNDANE,	-1},
{INV_BUT_ARMS,     BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN2A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN2B.PCX",	12, 	132,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfChangeCurrCat,		IC_MAGITEM,	-1},
{INV_BUT_SPELLS,   BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN3A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN3B.PCX",	12, 	156,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfChangeCurrCat,		IC_SPELL,	-1},
{INV_BUT_MAGICITMS,BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN4A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN4B.PCX",	12, 	180,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfChangeCurrCat,		IC_REALM_SPELL,-1},
{INV_BUT_BLOODABLT,BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN5A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN5B.PCX",	12, 	204,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfChangeCurrCat,		IC_BLOODAB,	-1},
{INV_BUT_USE,      BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN6A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN6B.PCX",	12, 	236,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfUseHighlightedObject,	0,			-1},
{INV_BUT_DROP,     BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN7A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN7B.PCX",	12, 	260,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfDropItem,				0,			-1},
{INV_BUT_GIVE,     BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN8A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN8B.PCX",	12, 	284,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfGiveItem,				0,			-1},
{INV_BUT_SCROLLUP, BUTTON_BITMAP,0,0,-1,0,"UI\\INVBTNUA.PCX",0,"UI\\INVBTNUB.PCX",	360,	108,	24, 	60, 	NO_KEY,	StaticInventoryGUI::mfScroll,				-1,			-1},
{INV_BUT_SCROLLDN, BUTTON_BITMAP,0,0,-1,0,"UI\\INVBTNDA.PCX",0,"UI\\INVBTNDB.PCX",	360,	168,	24, 	60, 	NO_KEY,	StaticInventoryGUI::mfScroll,				1,			-1},
{INV_BUT_SPELLBOX1,BUTTON_REGION,0,0,-1,0,"",0,"",					15, 	317,	25, 	18, 	KEY_1,	StaticInventoryGUI::mfClearSpellBox,		0,			-1},
{INV_BUT_SPELLBOX2,BUTTON_REGION,0,0,-1,0,"",0,"",					47, 	317,	25, 	18, 	KEY_2,	StaticInventoryGUI::mfClearSpellBox,		1,			-1},
{INV_BUT_SPELLBOX3,BUTTON_REGION,0,0,-1,0,"",0,"",					79, 	317,	25, 	18, 	KEY_3,	StaticInventoryGUI::mfClearSpellBox,		2,			-1},
{INV_BUT_SPELLBOX4,BUTTON_REGION,0,0,-1,0,"",0,"",					111,	317,	25, 	18, 	KEY_4,	StaticInventoryGUI::mfClearSpellBox,		3,			-1},
{INV_BUT_SPELLBOX5,BUTTON_REGION,0,0,-1,0,"",0,"",					143,	317,	25, 	18, 	KEY_5,	StaticInventoryGUI::mfClearSpellBox,		4,			-1},
{INV_BUT_SPELLBOX6,BUTTON_REGION,0,0,-1,0,"",0,"",					175,	317,	25, 	18, 	KEY_6,	StaticInventoryGUI::mfClearSpellBox,		5,			-1},
{665     ,BUTTON_REGION,0,0,-1,173,  "",0,"",0,0,0,0,KEY_ESCAPE,StaticInventoryGUI::mfDone,0,-1},
};
LONG AvatarInventorySize = sizeof(AvatarInventory) / sizeof(BUTTON);

BUTTON MessageBoxTitled[] = {
{MSGBOX_BUT_ALL   ,BUTTON_BITMAP, D_FREEZE|D_ZOOM, 0,-1,0,"UI\\HELP_SM.PCX",0,"",105,100,356,112,NO_KEY,TitledMessageBox::PrintMessage,0,-1},
{MSGBOX_BUT_TITLE ,BUTTON_REGION, 0, 0,-1,31,"",0,"",4,19,356,31,KEY_SPACE,TitledMessageBox::Hide,0,-1},
{MSGBOX_BUT_MSG   ,BUTTON_REGION, 0, 0,-1,31,"",0,"",4,35,356,55,KEY_ENTER,TitledMessageBox::Hide,0,-1},
{MSGBOX_BUT_OK    ,BUTTON_BITMAP, 0, 0,STR_OK,BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",128,76,100,28,KEY_O,TitledMessageBox::Hide,0,-1},
};
LONG MessageBoxTitledSize = sizeof(MessageBoxTitled) / sizeof(BUTTON);

BUTTON SmallInfoBox[] = {
{INFOBOX_BUT_ALL   ,BUTTON_BITMAP, D_FREEZE|D_ZOOM|D_DONT_SHADE, 0,-1,0,"UI\\INFOBOX.PCX",0,"",160,100,319,41,NO_KEY,InfoBox::Paint,0,-1},
};
LONG InfoBoxSize = sizeof(SmallInfoBox) / sizeof(BUTTON);


BUTTON ResurrectionScreen[] = {
{RESURRECT_BUT_ALL	,BUTTON_BITMAP, D_FREEZE|D_ZOOM, 0,-1,0,"UI\\RESURECT.PCX",0,"",160,100,330,170,NO_KEY,ResurrectionMenu::Paint,0,-1},
{RESURRECT_BUT_TITLE,BUTTON_REGION, 0, 0,STR_ITEM_UI_RESURRECTION,31,"",0,"",4, 4,322, 24,NO_KEY,NULL,0,-1},
{RESURRECT_BUT_Q	,BUTTON_REGION, 0, 0,STR_ITEM_UI_RESURRECTION_QUESTION,31,"",0,"",4,42,322,15,NO_KEY,NULL,0,-1},
{RESURRECT_BUT_NAME1,BUTTON_REGION, 0, 0,-1,31,"",0,"",4,57,322,18,NO_KEY,ResurrectionMenu::Resurrect,0,-1},
{RESURRECT_BUT_NAME2,BUTTON_REGION, 0, 0,-1,31,"",0,"",4,75,322,18,NO_KEY,ResurrectionMenu::Resurrect,1,-1},
{RESURRECT_BUT_NAME3,BUTTON_REGION, 0, 0,-1,31,"",0,"",4,93,322,18,NO_KEY,ResurrectionMenu::Resurrect,2,-1},
{RESURRECT_BUT_NAME4,BUTTON_REGION, 0, 0,-1,31,"",0,"",4,111,322,18,NO_KEY,ResurrectionMenu::Resurrect,3,-1},
{RESURRECT_BUT_CANCL,BUTTON_REGION, 0, 0,STR_CANCEL,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"",126,129,75,19,KEY_A,HideSubMenuWithClick,D_RESURRECT_MENU,-1},
{665,BUTTON_REGION, 0, 0,-1,31,"",0,"",0,0,0,0,KEY_ESCAPE,HideSubMenu,D_RESURRECT_MENU,-1},
};
LONG ResurrectionScreenSize = sizeof(ResurrectionScreen) / sizeof(BUTTON);


BUTTON BtlControl[] = {
{ 0,          BUTTON_BITMAP,0,0,-1,             0,"UI\\BATLPNL1.PCX",0,"",0,316,640,164,NO_KEY,BattleControlPaint,0,-1},
{ 2,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  338, 93, 55, 40,NO_KEY,BattleControlGrid, 0,-1},
{ 3,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  398, 93, 55, 40,NO_KEY,BattleControlGrid, 1,-1},
{ 4,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  458, 93, 55, 40,NO_KEY,BattleControlGrid, 2,-1},
{ 5,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  518, 93, 55, 40,NO_KEY,BattleControlGrid, 3,-1},
{ 6,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  578, 93, 55, 40,NO_KEY,BattleControlGrid, 4,-1},
{ 7,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  338, 49, 55, 40,NO_KEY,BattleControlGrid, 5,-1},
{ 8,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  398, 49, 55, 40,NO_KEY,BattleControlGrid, 6,-1},
{ 9,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  458, 49, 55, 40,NO_KEY,BattleControlGrid, 7,-1},
{10,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  518, 49, 55, 40,NO_KEY,BattleControlGrid, 8,-1},
{11,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  578, 49, 55, 40,NO_KEY,BattleControlGrid, 9,-1},
{12,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  338,  5, 55, 40,NO_KEY,BattleControlGrid,10,-1},
{13,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  398,  5, 55, 40,NO_KEY,BattleControlGrid,11,-1},
{14,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  458,  5, 55, 40,NO_KEY,BattleControlGrid,12,-1},
{15,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  518,  5, 55, 40,NO_KEY,BattleControlGrid,13,-1},
{16,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  578,  5, 55, 40,NO_KEY,BattleControlGrid,14,-1},
{17,          BUTTON_REGION,0,0,-1,             0,"",0,"",                  7,  5,323,100,NO_KEY,BattleReservesGrid,D_BTL_CONTROL,-1},
{BTL_RESV_ID, BUTTON_REGION,0,0,BTLUI_RESV,     BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLBTN1B.PCX", 40,136, 84, 28,KEY_R,BattleReservesMode,0,-1},
{BTL_STAT_ID, BUTTON_REGION,0,0,BTLUI_STATUS,   BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLBTN2B.PCX",124,136, 84, 28,KEY_S,BattleStatusMode,0,-1},
{BTL_DONE_ID, BUTTON_REGION,0,0,STR_DONE,       BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLBTN3B.PCX",268,136, 84, 28,KEY_D,BattleNextTurn,0,-1},
{BTL_EXIT_ID, BUTTON_REGION,0,0,BTLUI_RETREAT,  BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLBTN3B.PCX",208,136, 84, 28,KEY_T,BattleRetreat,0,-1},
{BTL_VIEW_ID, BUTTON_REGION,0,0,BTLUI_COMMAND,  BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLBTN4B.PCX",424,136,124, 28,KEY_V,BattleCameraHighView,0,-1},
{BTL_CANCEL_ID,BUTTON_REGION,0,0,STR_CANCEL1,   BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLCANCE.PCX",150,107, 70, 21,NO_KEY,BattleCancel,0,-1},
{BTL_FALLBACK_ID,BUTTON_REGION,0,0,BTLUI_FALLBACK,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLCANCE.PCX", 70,107, 70, 21,NO_KEY,BattleFallBack,0,-1},
{BTL_LIGHT_ID,BUTTON_REGION,0,0,-1,             0,"",0,"UI\\LIGHT.PCX",     7,  0,129,135,NO_KEY,NULL,0,-1},

};
LONG BtlControlSize = sizeof(BtlControl) / sizeof(BUTTON);


// Note: The next four Button menus are nearly identical. Be sure to duplicate
//       any changes in each of the menus.

BUTTON CombatControl1[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT2A.PCX",0,"",  0,344,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,0,0,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",149, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",177, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
{BUTNO_SPELL5,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,4,-1},
{BUTNO_SPELL6,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,5,-1},
};
LONG CombatControlSize1 = sizeof(CombatControl1) / sizeof(BUTTON);

BUTTON CombatControl2[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT2A.PCX",0,"",320,344,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,0,0,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",149, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",177, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
{BUTNO_SPELL5,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,4,-1},
{BUTNO_SPELL6,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,5,-1},
};
LONG CombatControlSize2 = sizeof(CombatControl2) / sizeof(BUTTON);

BUTTON CombatControl3[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT2A.PCX",0,"",  0,404,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,0,0,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",149, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",177, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
{BUTNO_SPELL5,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,4,-1},
{BUTNO_SPELL6,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,5,-1},
};
LONG CombatControlSize3 = sizeof(CombatControl3) / sizeof(BUTTON);

BUTTON CombatControl4[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT2A.PCX",0,"",320,404,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,0,0,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",149, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",177, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
{BUTNO_SPELL5,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,4,-1},
{BUTNO_SPELL6,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,5,-1},
};
LONG CombatControlSize4 = sizeof(CombatControl4) / sizeof(BUTTON);


BUTTON CombatFighterControl1[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT1A.PCX",0,"",  0,344,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_LOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,CombatLowAttackKey,0,COMBAT_STR_ATTACK2},
{BUTNO_QLOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",143, 37, 25, 18,NO_KEY,CombatLowAttackKey,1,COMBAT_STR_QUICK_ATTACK2},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",171, 37, 25, 18,NO_KEY,0,1,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
};
LONG CombatFighterControlSize1 = sizeof(CombatFighterControl1) / sizeof(BUTTON);

BUTTON CombatFighterControl2[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT1A.PCX",0,"",320,344,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_LOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,CombatLowAttackKey,0,COMBAT_STR_ATTACK2},
{BUTNO_QLOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",143, 37, 25, 18,NO_KEY,CombatLowAttackKey,1,COMBAT_STR_QUICK_ATTACK2},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",171, 37, 25, 18,NO_KEY,0,1,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
};
LONG CombatFighterControlSize2 = sizeof(CombatFighterControl2) / sizeof(BUTTON);

BUTTON CombatFighterControl3[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT1A.PCX",0,"",  0,404,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_LOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,CombatLowAttackKey,0,COMBAT_STR_ATTACK2},
{BUTNO_QLOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",143, 37, 25, 18,NO_KEY,CombatLowAttackKey,1,COMBAT_STR_QUICK_ATTACK2},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",171, 37, 25, 18,NO_KEY,0,1,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
};
LONG CombatFighterControlSize3 = sizeof(CombatFighterControl3) / sizeof(BUTTON);

BUTTON CombatFighterControl4[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT1A.PCX",0,"",320,404,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_LOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,CombatLowAttackKey,0,COMBAT_STR_ATTACK2},
{BUTNO_QLOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",143, 37, 25, 18,NO_KEY,CombatLowAttackKey,1,COMBAT_STR_QUICK_ATTACK2},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",171, 37, 25, 18,NO_KEY,0,1,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
};
LONG CombatFighterControlSize4 = sizeof(CombatFighterControl4) / sizeof(BUTTON);

BUTTON CombatRPGBottomBar[] = {
{ 0,					BUTTON_BITMAP, D_NO_PUSH, 0,-1,		              0,"UI\\EXTRA_16.PCX",0,"",  0,464,640,16,NO_KEY,0,0,-1},
{BUTNO_RPG_LEAVE,		BUTTON_BITMAP,		   0, 0,COMBAT_STR_LEAVE,	BTN_LABEL_COLOR,"UI\\CMBBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN1B.PCX",  0,  0,112,16,NO_KEY,CombatReturnToMap,0,-1},
{BUTNO_RPG_RETREAT,		BUTTON_BITMAP,		   0, 0,COMBAT_STR_RETREAT,	BTN_LABEL_COLOR,"UI\\CMBBTN2A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN2B.PCX",112,  0, 80,16,NO_KEY,CombatRetreatKey,0,-1},
{BUTNO_RPG_LEADER,		BUTTON_BITMAP,		   0, 0,COMBAT_STR_LEADER,	BTN_LABEL_COLOR,"UI\\CMBBTN3A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN3B.PCX",192,  0,112,16,NO_KEY,CombatNewLeaderKey,0,-1},
{BUTNO_RPG_VIEW,		BUTTON_BITMAP,		   0, 0,COMBAT_STR_VIEW,	BTN_LABEL_COLOR,"UI\\CMBBTN4A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN4B.PCX",304,  0,110,16,NO_KEY,CombatToggleViewKey,0,-1},
{BUTNO_RPG_INVENTORY,	BUTTON_BITMAP,		   0, 0,COMBAT_STR_INVENTORY,BTN_LABEL_COLOR,"UI\\CMBBTN5A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN5B.PCX",404,  0, 80,16,NO_KEY,CombatShowInventoryKey,1,-1},
{BUTNO_RPG_STATUS,		BUTTON_BITMAP,		   0, 0,COMBAT_STR_STATUS, 	BTN_LABEL_COLOR,"UI\\CMBBTN6A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN6B.PCX",484,  0, 52,16,NO_KEY,CombatShowStatusKey,1,-1},
{BUTNO_RPG_MAP,			BUTTON_BITMAP,		   0, 0,COMBAT_STR_MAP,		BTN_LABEL_COLOR,"UI\\CMBBTN7A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN7B.PCX",536,  0, 52,16,NO_KEY,CombatToggleMapKey,0,-1},
{BUTNO_RPG_DONE,		BUTTON_BITMAP,		   0, 0,-1,					  0,"UI\\CMBBTN8A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN8B.PCX",588,  0, 52,16,NO_KEY,CombatTurnDoneKey,0,-1}
};
LONG CombatRPGBottomBarSize = sizeof(CombatRPGBottomBar)/ sizeof(BUTTON);


BUTTON CombatBlank1Bar[] = {
{ 0,BUTTON_BITMAP, D_NO_PUSH, 0, -1,0,"UI\\SCMBT_UE.PCX",0,"",320,404,320, 60,NO_KEY,0,0,-1},
};
LONG CombatBlank1BarSize = sizeof(CombatBlank1Bar)/sizeof(BUTTON);

BUTTON BtlChooseField[] = {
{ 0,BUTTON_BITMAP,D_ZOOM, 0,-1, 0,"UI\\CHOOSBTL.PCX",0,"",145, 50,351,371,NO_KEY,PaintChooseBattleField,0,-1},
{ 1,BUTTON_BITMAP,0,      0,-1, 0,"",                0,"", 10, 36,150,160,NO_KEY,ChooseBattleField, 0,-1},
{ 2,BUTTON_BITMAP,0,      0,-1, 0,"",                0,"",172, 36,150,160,NO_KEY,ChooseBattleField, 1,-1},
{ 3,BUTTON_BITMAP,0,      0,-1, 0,"",                0,"", 10,201,150,160,NO_KEY,ChooseBattleField, 2,-1},
{ 4,BUTTON_BITMAP,0,      0,-1, 0,"",                0,"",172,201,150,160,NO_KEY,ChooseBattleField, 3,-1},
{ 6,BUTTON_REGION,0,      0,-1, DKBROWN,"",          0,"",  4,  4,322, 30,NO_KEY,NULL,0,-1},
{91,BUTTON_REGION,0,      0,-1, DKBROWN,"",          0,"", 10,180,150, 20,NO_KEY,NULL,0,-1},
{92,BUTTON_REGION,0,      0,-1, DKBROWN,"",          0,"",172,180,150, 20,NO_KEY,NULL,0,-1},
{93,BUTTON_REGION,0,      0,-1, DKBROWN,"",          0,"", 10,344,150, 20,NO_KEY,NULL,0,-1},
{94,BUTTON_REGION,0,      0,-1, DKBROWN,"",          0,"",172,344,150, 20,NO_KEY,NULL,0,-1},
};
LONG BtlChooseFieldSize = sizeof(BtlChooseField) / sizeof(BUTTON);

BUTTON ShowAnimation[] = {
{0,BUTTON_COLORBEVEL,D_FREEZE|D_ZOOM, 76,-1,0,"",0,"", 40, 10,550,460,NO_KEY,PaintAnimation,0,-1},
{1,BUTTON_REGION,0, 0,-1,                   0,"",0,"", 20, 10,510,360,NO_KEY,0,0,-1},
{2,BUTTON_REGION,0, 0,-1,			        0,"",0,"",113,370, 78, 16,NO_KEY,GetPrevAnimation,0,-1},
{3,BUTTON_REGION,0, 0,-1,                   0,"",0,"",193,370, 78, 16,NO_KEY,GetNextAnimation,0,-1},
{4,BUTTON_REGION,0, 0,-1,                   0,"",0,"",113,390, 78, 16,NO_KEY,GetPrevSequence,0,-1},
{5,BUTTON_REGION,0, 0,-1,                   0,"",0,"",193,390, 78, 16,NO_KEY,GetNextSequence,0,-1},
{6,BUTTON_REGION,0, 0,-1,                   0,"",0,"",113,410, 78, 16,NO_KEY,GetPrevRotation,0,-1},
{7,BUTTON_REGION,0, 0,-1,                   0,"",0,"",193,410, 78, 16,NO_KEY,GetNextRotation,0,-1},
{8,BUTTON_REGION,0, 0,-1,                   0,"",0,"", 22,430, 88, 16,NO_KEY,RunLoop,0,-1},
{9,BUTTON_REGION,0, 0,-1,                   0,"",0,"",113,430, 78, 16,NO_KEY,StopLoop,0,-1},
{10,BUTTON_REGION,0, 0,-1,                  0,"",0,"",453,430, 78, 16,KEY_ESCAPE,CloseAnimationMenu, 0,-1},
{11,BUTTON_REGION,0, 0,-1,                  0,"",0,"",193,430, 78, 16,NO_KEY,PlayAll,0,-1},
{12,BUTTON_REGION,0, 0,-1,                  0,"",0,"",273,430, 88, 16,NO_KEY,SpeedUp,0,-1},
{13,BUTTON_REGION,0, 0,-1,                  0,"",0,"",363,430, 88, 16,NO_KEY,SlowDown,0,-1},
{14,BUTTON_REGION,0, 0,-1,                  0,"",0,"",343,370,150, 60,NO_KEY,0, 0,-1},
{15,BUTTON_REGION,0, 0,-1,                  0,"",0,"",22, 370, 88, 60,NO_KEY,0,0,-1},

};
LONG ShowAnimationSize = sizeof(ShowAnimation)/sizeof(BUTTON);


BUTTON MoreInfo[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,0,"UI\\INFODLOG.PCX",0,"",145, 50,352,175,NO_KEY,PaintMoreInfo,0,-1},
{1,BUTTON_REGION,0,               0,-1,0,"",0,"", 24, 24,305,127,NO_KEY,NULL, 0,-1},
{4,BUTTON_REGION,0,               0,-1,0,"",0,"",152,151,48,15,KEY_ESCAPE,HideSubMenuWithClick, D_MOREINFO,-1},
};
LONG MoreInfoSize = sizeof(MoreInfo) / sizeof(BUTTON);

BUTTON HoldAction[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,0,"UI\\RELMSLCT.PCX",0,"",145,210,356,70,NO_KEY,PaintHoldAction,0,-1},
{1,BUTTON_REGION,0,               0,-1,0,"",0,"", 24, 24,305,127,NO_KEY,CancelHoldAction, 0,-1},
{4,BUTTON_REGION,0,               0,-1,0,"",0,"",161,151,48,15,KEY_ESCAPE,DoneHoldAction, 0,-1},
};
LONG HoldActionSize = sizeof(HoldAction) / sizeof(BUTTON);

BUTTON HelpKeys[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,      0,"UI\\HELP_LG.PCX",0,"",160, 50,356,372,NO_KEY,PaintMoreInfo,0, -1},
{1,BUTTON_REGION,0,               0,-1,      0,"",0,"",                 24, 24,305,320,NO_KEY,0, 0,-1},
{2,BUTTON_REGION,0,     0,STR_OK, BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAA.PCX",128,336,100, 28,KEY_O,DoneHelpKeys, 0,-1},
{3,BUTTON_REGION,0,     0,-1, 0,"",0,"",0,0,0,0,KEY_ESCAPE,DoneHelpKeys, 1,-1},
};
LONG HelpKeysSize = sizeof(HelpKeys) / sizeof(BUTTON);


BUTTON BattleCard[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,0,"UI\\WARCARD.PCX",0,"",160,128,320,192,NO_KEY,PaintBattleCard,0,-1},
{1,BUTTON_REGION,0,               0,-1,0,"",0,"",                 8,  8,304,176,KEY_ESCAPE,BattleCardProc, D_BTL_CARD,-1},
};
LONG BattleCardSize = sizeof(BattleCard) / sizeof(BUTTON);

BUTTON Victory[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,            0,"UI\\VCTPANL1.PCX",0,"",150,30,340,388,NO_KEY,NULL,0,-1},
{1,BUTTON_BITMAP,0,               0,STR_HELP_DONE,BTN_LABEL_COLOR,"UI\\VCTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\VCTBTNXB.PCX",                128,352,84,28,KEY_O,HideSubMenuWithClick, D_VICTORY,-1},
};
LONG VictorySize = sizeof(Victory) / sizeof(BUTTON);

BUTTON Defeat[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,            0,"UI\\DFTPANL1.PCX",0,"",150,30,340,388,NO_KEY,NULL,0,-1},
{1,BUTTON_BITMAP,0,               0,STR_HELP_DONE,BTN_LABEL_COLOR,"UI\\DFTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\DFTBTNXB.PCX",                128,352,84,28,KEY_O,HideSubMenuWithClick, D_DEFEAT,-1},
};
LONG DefeatSize = sizeof(Defeat) / sizeof(BUTTON);

BUTTON ChatDisplayWindow[] =  {
{0,BUTTON_BITMAP,D_FLOATER, 0,-1, 0,"UI\\CHATPNL.PCX",0,"", 0,0,640,64,NO_KEY,PaintChatDisplay,0,-1},
};
LONG ChatDisplayWindowSize = sizeof(ChatDisplayWindow) / sizeof(BUTTON);


BUTTON ChatDisplayWindow1[] =  {
{0,BUTTON_BITMAP,D_FLOATER, 0,-1, 0,"UI\\CHATBOX2.PCX",0,"", 0,0,640,128,NO_KEY, PaintChatDisplay, 1, -1},
};
LONG ChatDisplayWindow1Size = sizeof(ChatDisplayWindow1) / sizeof(BUTTON);

BUTTON ChatSelect[] = {                                           //80
{0,BUTTON_BITMAP,D_FREEZE, 0, -1, 0, "UI\\CHATPNL3.PCX", 0,"", 83, 130, 472, 320,NO_KEY,PaintChatSelect,0, -1},
{1,BUTTON_REGION,0, 0,-1,                0, "",0,"", 25, 100,385,15,NO_KEY,0, 0,-1},
{2,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 132,380,15,NO_KEY,0, 1,-1},
{3,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 148,380,15,NO_KEY,0, 2,-1},
{4,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 164,380,15,NO_KEY,0, 3,-1},
{5,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 180,380,15,NO_KEY,0, 4,-1},
{6,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 196,380,15,NO_KEY,0, 5,-1},
{7,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 212,380,15,NO_KEY,0, 6,-1},
{8,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 228,380,15,NO_KEY,0, 7,-1},
{9,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 244,380,15,NO_KEY,0, 8,-1},
{10,BUTTON_REGION,0, 0,STR_CANCEL1, BTN_LABEL_COLOR,"UI\\CHTBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CHTBTNCB.PCX",252,264,84, 28,KEY_ESCAPE, CancelChatSelect, 0, -1},
{11,BUTTON_REGION,0, 0,STR_DONE1,   BTN_LABEL_COLOR,"UI\\CHTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CHTBTNXB.PCX",336,264,84, 28,NO_KEY,DoneChatSelect, 0, -1},
};

LONG ChatSelectSize = sizeof(ChatSelect) / sizeof(BUTTON);


BUTTON MultiName[] = {
{ 0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\MLT_PNL4.PCX", 0,"", 162, 46, 316, 232,NO_KEY,PaintMultiPlayerName,0, -1},
{ 1,BUTTON_REGION,0,                    0,-1,         0, "",0,"", 61, 100,200,15,NO_KEY,NULL, 0,-1},
{ 2,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,-1,         0, "",0,"", 61, 135,200,15,NO_KEY,SetMultiPlayerProtocol, 0,-1},
{ 3,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,-1,         0, "",0,"", 61, 151,200,15,NO_KEY,SetMultiPlayerProtocol, 1,-1},
{ 4,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,-1,         0, "",0,"", 61, 167,200,15,NO_KEY,SetMultiPlayerProtocol, 2,-1},
{ 9,BUTTON_BITMAP,0, 0,STR_BACK1,  BTN_LABEL_COLOR,"UI\\MLTBTNBA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNBB.PCX", 32,196,84, 28,NO_KEY,BackMultiPlayerName, 0, -1},
{10,BUTTON_BITMAP,0, 0,STR_JOIN,   BTN_LABEL_COLOR,"UI\\MLTBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNCB.PCX",116,196,84, 28,NO_KEY,JoinMultiPlayerName, 16, -1},
{11,BUTTON_BITMAP,0, 0,STR_CREATE, BTN_LABEL_COLOR,"UI\\MLTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNXB.PCX",200,196,84, 28,NO_KEY,HostMultiPlayerName, 0, -1},
{99,BUTTON_REGION,0, 0,-1,0,"",0,"",0,0,0,0,KEY_ESCAPE,BackMultiPlayerName, 16, -1},
};
LONG MultiNameSize = sizeof(MultiName) / sizeof(BUTTON);

BUTTON MultiCreate[] = {
{ 0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\MLT_PNL4.PCX", 0,"", 162, 46, 316, 232,NO_KEY,PaintMultiPlayerCreate,0, -1},
{ 1,BUTTON_REGION,0,        0,-1,         0,              "",0,"", 61, 100,100,15,NO_KEY,NULL, 0,-1},
{ 1,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_MULTIMENU_NP,BTN_LABEL_COLOR,"",0,"", 60, 130,200,14,NO_KEY,NULL, 0,-1},
{ 2,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_2,                 BTN_LABEL_COLOR,"",0,"", 60, 151,50,15,NO_KEY,SetMultiPlayerNumber, 2,-1},
{ 3,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_3,                 BTN_LABEL_COLOR,"",0,"", 60, 167,50,15,NO_KEY,SetMultiPlayerNumber, 3,-1},
{ 4,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_4,                 BTN_LABEL_COLOR,"",0,"",110, 151,50,15,NO_KEY,SetMultiPlayerNumber, 4,-1},
{ 5,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_5,                 BTN_LABEL_COLOR,"",0,"",110, 167,50,15,NO_KEY,SetMultiPlayerNumber, 5,-1},
{ 6,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_6,                 BTN_LABEL_COLOR,"",0,"",160, 151,50,15,NO_KEY,SetMultiPlayerNumber, 6,-1},
{ 7,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_7,                 BTN_LABEL_COLOR,"",0,"",160, 167,50,15,NO_KEY,SetMultiPlayerNumber, 7,-1},
{ 8,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_8,                 BTN_LABEL_COLOR,"",0,"",210, 151,50,15,NO_KEY,SetMultiPlayerNumber, 8,-1},
{ 9,BUTTON_BITMAP,0,        0,STR_BACK1,  BTN_LABEL_COLOR,"UI\\MLTBTNBA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNBB.PCX", 32,196,84, 28,NO_KEY,BackMultiPlayerCreate, 0, -1},
{10,BUTTON_BITMAP,0,        0,STR_CANCEL1,BTN_LABEL_COLOR,"UI\\MLTBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNCB.PCX",116,196,84, 28,NO_KEY,CancelMultiPlayerCreate, 16, -1},
{11,BUTTON_BITMAP,0,        0,STR_HOST,   BTN_LABEL_COLOR,"UI\\MLTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNXB.PCX",200,196,84, 28,NO_KEY,HostMultiPlayerCreate, 0, -1},
{99,BUTTON_REGION,0,        0,-1,0,"",0,"",0,0,0,0,KEY_ESCAPE,BackMultiPlayerCreate, 16, -1},
};
LONG MultiCreateSize = sizeof(MultiCreate) / sizeof(BUTTON);

BUTTON MultiJoin[] = {
{ 0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\MLT_PNL4.PCX", 0,"", 162, 46, 316, 232,NO_KEY,PaintMultiPlayerJoin,0, -1},
{ 1,BUTTON_REGION,0,0,-1,0,"",0,"", 61, 100,200,15,NO_KEY,NULL, 0,-1},
{ 2,BUTTON_REGION,0,0,-1,0,"",0,"", 61, 130,200,15,KEY_ESCAPE,SetMultiPlayerJoin, 0, -1},
{ 3,BUTTON_REGION,0,0,-1,0,"",0,"", 61, 145,200,15,KEY_ESCAPE,SetMultiPlayerJoin, 1, -1},
{ 4,BUTTON_REGION,0,0,-1,0,"",0,"", 61, 160,200,15,KEY_ESCAPE,SetMultiPlayerJoin, 2, -1},
{ 5,BUTTON_REGION,0,0,-1,0,"",0,"", 61, 175,200,15,KEY_ESCAPE,SetMultiPlayerJoin, 3, -1},
{ 9,BUTTON_BITMAP,0,0,STR_BACK1,        BTN_LABEL_COLOR,"UI\\MLTBTNBA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNBB.PCX", 32,196,84, 28,NO_KEY,BackMultiPlayerJoin, 0, -1},
{10,BUTTON_BITMAP,0,0,STR_CANCEL1,      BTN_LABEL_COLOR,"UI\\MLTBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNCB.PCX",116,196,84, 28,NO_KEY,CancelMultiPlayerJoin, 16, -1},
{11,BUTTON_BITMAP,0,0,STR_DONE1,        BTN_LABEL_COLOR,"UI\\MLTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNXB.PCX",200,196,84, 28,NO_KEY,DoneMultiPlayerJoin, 0, -1},
{99,BUTTON_REGION,0,0,-1,0,"",0,"",0,0,0,0,KEY_ESCAPE,CancelMultiPlayerJoin, 16, -1},
};
LONG MultiJoinSize = sizeof(MultiJoin) / sizeof(BUTTON);

BUTTON MultiEnvironment[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\MLT_ENV.PCX", 0,"", 162, 46, 316, 200,NO_KEY,PaintMultiPlayerEnvironment,0, -1},
{1,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,  0,STR_MULTIMENU_INT,BTN_LABEL_COLOR, "",0,"", 63,103,200,15,NO_KEY,SetMultiPlayerEnvironment, 0,-1},
{2,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,  0,STR_MULTIMENU_LA, BTN_LABEL_COLOR, "",0,"", 63,120,200,15,NO_KEY,SetMultiPlayerEnvironment, 1,-1},
{3,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,  0,STR_MULTIMENU_WP, BTN_LABEL_COLOR, "",0,"", 63,137,200,15,NO_KEY,SetMultiPlayerEnvironment, 2,-1},
{10,BUTTON_BITMAP,0, 0,STR_CANCEL,       BTN_LABEL_COLOR,"UI\\MLTBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNCB.PCX", 73,164,84, 28,KEY_A,CancelMultiPlayerEnvironment, 0, -1},
{11,BUTTON_BITMAP,0, 0,STR_DONE,         BTN_LABEL_COLOR,"UI\\MLTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNXB.PCX",157,164,84, 28,KEY_O,DoneMultiPlayerEnvironment, 0, -1},
{12,BUTTON_REGION,0, 0,-1,0,"",0,"",0,0,0,0,KEY_ESCAPE,CancelMultiPlayerEnvironment, 0, -1},
};
LONG MultiEnvironmentSize = sizeof(MultiEnvironment) / sizeof(BUTTON);

BUTTON MultiHost[] = {
{ 0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\HST_PNL1.PCX", 0,"", 208, 100, 224, 264,NO_KEY,PaintMultiPlayerHost,0, -1},
{91,BUTTON_REGION,D_SANS_8,             0,STR_MULTITYPE_SEL, 200,            "",0,"", 20, 70, 82,14,NO_KEY,NULL, 1,-1}, // TITLE
#ifdef _WINDOWS
{ 1,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_MULTITYPE_NEW, BTN_LABEL_COLOR,"",0,"", 25, 93, 74,14,NO_KEY,SetMultiPlayerHostGame, iMPGT_NEW,-1}, // New
{ 2,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_MULTITYPE_LOAD,BTN_LABEL_COLOR,"",0,"", 25,107, 74,14,NO_KEY,SetMultiPlayerHostGame, iMPGT_LOAD,-1}, // Load
#endif
{92,BUTTON_REGION,D_SANS_8,             0,STR_TURNTIME,      200,            "",0,"", 20,130, 82,14,NO_KEY,NULL, 1,-1}, // TITLE
{ 3,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_TURNTIME_1,    BTN_LABEL_COLOR,"",0,"", 25,154, 74,14,NO_KEY,SetMultiPlayerHostTime, iTT_1Min,-1}, // Fast
{ 4,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_TURNTIME_2,    BTN_LABEL_COLOR,"",0,"", 25,168, 74,14,NO_KEY,SetMultiPlayerHostTime, iTT_2Min,-1}, // Med
{ 5,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_TURNTIME_3,    BTN_LABEL_COLOR,"",0,"", 25,182, 74,14,NO_KEY,SetMultiPlayerHostTime, iTT_5Min,-1}, // Slow
{ 6,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_TURNTIME_4,    BTN_LABEL_COLOR,"",0,"", 25,196, 74,14,NO_KEY,SetMultiPlayerHostTime, iTT_None,-1}, // None
{93,BUTTON_REGION,D_SANS_8,             0,STR_OPT_GAME_CTRL, 200,            "",0,"",120, 70, 82,14,NO_KEY,NULL, 1,-1}, // TITLE
{ 7,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_OPT_CTRL_MODE, BTN_LABEL_COLOR,"",0,"",125, 93, 74,14,NO_KEY,SetMultiPlayerHostDetail, 0,-1}, // Basic
{ 8,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_OPT_CTRL_MODE1,BTN_LABEL_COLOR,"",0,"",125,107, 74,14,NO_KEY,SetMultiPlayerHostDetail, 1,-1}, // Advanced
{ 9,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_OPT_CTRL_MODE2,BTN_LABEL_COLOR,"",0,"",125,121, 74,14,NO_KEY,SetMultiPlayerHostDetail, 2,-1}, // Expert
{94,BUTTON_REGION,D_SANS_8,             0,STR_OPT_ADV_DIF,   200,            "",0,"",120,145, 82,14,NO_KEY,NULL, 1,-1}, // TITLE
{10,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_OPT_DIF_EASY,  BTN_LABEL_COLOR,"",0,"",125,168, 74,14,NO_KEY,SetMultiPlayerHostDiff, 0,-1}, // Easy
{11,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_OPT_DIF_MEDIUM,BTN_LABEL_COLOR,"",0,"",125,182, 74,14,NO_KEY,SetMultiPlayerHostDiff, 1,-1}, // Medium
{12,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_OPT_DIF_HARD,  BTN_LABEL_COLOR,"",0,"",125,196, 74,14,NO_KEY,SetMultiPlayerHostDiff, 2,-1}, // Hard
//{13,BUTTON_BITMAP,D_SANS_8, 0,STR_CANCEL,        BTN_LABEL_COLOR,"UI\\HSTBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HSTBTN1B.PCX", 28,226, 84, 28,KEY_A,CancelMultiPlayerHost, 0, -1},
{14,BUTTON_BITMAP,D_SANS_8, 0,STR_DONE,          BTN_LABEL_COLOR,"UI\\HSTBTN2A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HSTBTN2B.PCX",70,226, 84, 28,KEY_O,DoneMultiPlayerHost, 0, -1},
{15,BUTTON_REGION,0, 0,-1,0,"",0,"", 0,0,0,0,KEY_ESCAPE,CancelMultiPlayerHost, 0, -1},
};
LONG MultiHostSize = sizeof(MultiHost) / sizeof(BUTTON);


BUTTON LoyaltyMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE, 0, -1, 0, "UI\\REPORT2.PCX", 0,"", 102, 6, 436, 468,NO_KEY,PaintLoyaltyTable,0, -1},
{1,BUTTON_REGION,0, 0,-1,                0, "",0,"", 0, 0,436,468,NO_KEY,DoneReportMenu, 0,-1},
{2,BUTTON_REGION,0, 0,-1,                0, "",0,"", 0, 0,0,0,KEY_ESCAPE,DoneReportMenu1, 1,-1},
};

LONG LoyaltyMenuSize = sizeof(LoyaltyMenu) / sizeof(BUTTON);

BUTTON DiplomacyAction[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,0,"UI\\HELP_LG.PCX",0,"",160, 50,356,372,NO_KEY,PaintOthersDiplomacy,0, -1},
{1,BUTTON_REGION,0,              0,-1,         0,"",0,"", 24, 24,305,320,NO_KEY,0, 0, -1},
{2,BUTTON_REGION,D_HILIGHTED,0,-1, 0,"",0,"UI\\HLPBTNAA.PCX",  28,340, 100, 28,KEY_R,NeverOthersDiplomacy, 0, -1},
{3,BUTTON_REGION,D_HILIGHTED,0,-1,   0,"",0,"UI\\HLPBTNAA.PCX", 128,340, 100, 28,KEY_A,YesOthersDiplomacy, 0, -1},
{4,BUTTON_REGION,D_HILIGHTED,0,-1,    0,"",0,"UI\\HLPBTNAA.PCX", 228,340, 100, 28,KEY_P,NoOthersDiplomacy, 0, -1},
{5,BUTTON_REGION,0,    0,-1,   0,"",0,"",0,0,0,0,KEY_ESCAPE,YesOthersDiplomacy, 0, -1},
};

LONG DiplomacyActionSize = sizeof(DiplomacyAction) / sizeof(BUTTON);

BUTTON BattleMagicPrep[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,          0,"UI\\BTLSEL.PCX",0,"",   250, 80,140,280,NO_KEY,PaintBattleMagicPrep,0, -1},
{1,BUTTON_BITMAP,0,    0,STR_DONE,  BTN_LABEL_COLOR,"UI\\SELBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\SELBTN1B.PCX",  28,244, 84, 28,KEY_O,BattlePrepProc, 0, -1},
{2,BUTTON_REGION,0,    0,-1,          0,"",0,"",0,0,0,0,KEY_ESCAPE,BattlePrepProc, 0, -1},
};
LONG BattleMagicPrepSize = sizeof(BattleMagicPrep) / sizeof(BUTTON);

BUTTON BattleCommand[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,0,"UI\\HELP_LG.PCX",0,"",160, 50,356,372,NO_KEY,SetCommandBattlePaint, 0, -1},
{1,BUTTON_REGION,  0,0,-1,                 0,              "",                0,                     "", 25,100,290,240,NO_KEY,0, 0, -1},
{333,BUTTON_BITMAP,0,0,STR_YES,            BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"", 28,336,100, 28,KEY_O,SetCommandBattleProc, QUESTION_BTN1, -1},
{334,BUTTON_BITMAP,0,0,COMBAT_STR_RETREAT, BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"",128,336,100, 28,KEY_R,SetCommandBattleProc, QUESTION_BTN2, -1},
{335,BUTTON_BITMAP,0,0,STR_NO,             BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"",228,336,100, 28,KEY_N,SetCommandBattleProc, QUESTION_BTN3, -1},
};
LONG BattleCommandSize = sizeof(BattleCommand) / sizeof(BUTTON);

BUTTON BuildArmys[] = {
{0,BUTTON_BITMAP,D_FREEZE,  0,-1, 0,"UI\\BATLPICK.PCX",0,"",0,0,640,480,NO_KEY,BuildArmyPaint,0,-1},
{9,BUTTON_REGION,D_TEXT_LEFT,0,STR_BB_TITLE,31,"",0,"", 8, 6, 150,55,NO_KEY,NULL, 0, -1},
// my button and list box with controls
{9,           BUTTON_REGION,0,0,STR_BB_MYLIST,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"", 18, 68,130,15,NO_KEY,NULL, 0, -1},
{BB_MYARMY1,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18, 87,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY2,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,103,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY3,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,119,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY4,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,135,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY5,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,151,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY6,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,167,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY7,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,183,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY8,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,199,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY9,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,215,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY10, BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,231,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY11, BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,247,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY12, BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,263,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY13, BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,279,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY14, BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,295,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY15, BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,311,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MY_ADD,   BUTTON_BITMAP,0,0,STR_BB_ADD,   BTN_LABEL_COLOR,"UI\\BTPBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN1B.PCX", 12,332,144,28,NO_KEY,BuildListAdd, 0, -1},
{BB_MY_REMOVE,BUTTON_BITMAP,0,0,STR_BB_REM,   BTN_LABEL_COLOR,"UI\\BTPBTN2A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN2B.PCX", 12,360,144,28,NO_KEY,BuildListRemove,0, -1},
// enemy button and listbox with controls
{9,            BUTTON_REGION,0,0,STR_BB_HISLIST,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"",490, 68,130,15,NO_KEY,NULL, 0, -1},
{BB_HISARMY1,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490, 87,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY2,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,103,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY3,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,119,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY4,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,135,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY5,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,151,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY6,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,167,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY7,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,183,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY8,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,199,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY9,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,215,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY10, BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,231,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY11, BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,247,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY12, BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,263,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY13, BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,279,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY14, BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,295,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY15, BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,311,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HIS_ADD,   BUTTON_BITMAP,0,0,STR_BB_ADD,    BTN_LABEL_COLOR,"UI\\BTPBTN5A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN5B.PCX",484,332,144,28,NO_KEY,BuildListAdd, 0, -1},
{BB_HIS_REMOVE,BUTTON_BITMAP,0,0,STR_BB_REM,    BTN_LABEL_COLOR,"UI\\BTPBTN6A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN6B.PCX",484,360,144,28,NO_KEY,BuildListRemove,0, -1},

{BB_DISPLAY,BUTTON_REGION,0,0,STR_BB_TROOPS,    BTN_LABEL_COLOR,"UI\\BTPBTNZA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTNZB.PCX",248,416,144,28,NO_KEY,BuildDisplayToggle,0, -1},

{BB_PLAINS,    BUTTON_BITMAP,0,0,STR_BB_PLAINS,    BTN_LABEL_COLOR,"UI\\BTPBTNPA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTNPB.PCX",160,448,80, 24,NO_KEY,BuildTerrainSel, 0, -1},
{BB_MOUNTAINS, BUTTON_BITMAP,0,0,STR_BB_MOUNTAINS, BTN_LABEL_COLOR,"UI\\BTPBTNMA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTNMB.PCX",240,448,80, 24,NO_KEY,BuildTerrainSel, 1, -1},
{BB_FOREST,    BUTTON_BITMAP,0,0,STR_BB_FOREST,    BTN_LABEL_COLOR,"UI\\BTPBTNFA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTNFB.PCX",320,448,80, 24,NO_KEY,BuildTerrainSel, 2, -1},
{BB_SWAMP,     BUTTON_BITMAP,0,0,STR_BB_SWAMP,     BTN_LABEL_COLOR,"UI\\BTPBTNSA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTNSB.PCX",400,448,80, 24,NO_KEY,BuildTerrainSel, 3, -1},

{BB_RESTART,   BUTTON_BITMAP,0,0,STR_BB_RESTART,   BTN_LABEL_COLOR,"UI\\BTPBTN3A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN3B.PCX", 12,416,144, 28,NO_KEY,BuildArmyRestart,0, -1},
{BB_RELOAD,    BUTTON_BITMAP,0,0,STR_BB_RELOAD,    BTN_LABEL_COLOR,"UI\\BTPBTN4A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN4B.PCX", 12,444,144, 28,NO_KEY,BuildArmyReload, 0, -1},
{BB_PRACTICE,  BUTTON_BITMAP,0,0,STR_BB_PRACTICE,  BTN_LABEL_COLOR,"UI\\BTPBTN7A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN7B.PCX",484,416,144, 28,NO_KEY,BuildArmyDone,   0, -1},
{BB_FIGHT,     BUTTON_BITMAP,0,0,STR_BB_FIGHT,     BTN_LABEL_COLOR,"UI\\BTPBTN8A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTNBTN8B.PCX",484,444,144, 28,NO_KEY,BuildArmyDone,   0, -1},
};
LONG BuildArmysSize = sizeof(BuildArmys) / sizeof(BUTTON);

BUTTON BuildAdv[] = {
{0,BUTTON_BITMAP,D_FREEZE,   0,-1,           0,"UI\\BUILDADV.PCX",0,"",0,0,640,480,NO_KEY,BuildAdvPaint,0,-1},
{9,BUTTON_REGION,0,                0,STR_BA_TITLE,   31,"",0,"", 15,  2,170,20,NO_KEY,NULL, 0, -1},
// adventure list box 
{9,BUTTON_REGION,D_TEXT_LEFT,    0,STR_CS_SELECT,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"", 16, 55,176,25,NO_KEY,NULL, 0, -1},
{BA_LIST_01,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16, 95,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_02,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,110,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_03,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,125,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_04,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,140,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_05,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,155,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_06,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,170,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_07,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,185,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_08,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,200,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{9,BUTTON_REGION,D_TEXT_LEFT,    0,STR_CS_CHOOSE,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"", 16,230,176,25,NO_KEY,NULL, 0, -1},
{9,BUTTON_REGION,D_TEXT_LEFT,    0,STR_BA_CHOOSE,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"", 16,257,176, 8,NO_KEY,NULL, 0, -1},
{BA_LIST_09,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,270,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_10,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,285,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_11,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,300,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_12,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,315,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_13,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,330,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_14,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,345,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_15,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,360,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_16,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,375,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_RESET,      BUTTON_BITMAP,0,0,STR_BA_RESET,  BTN_LABEL_COLOR,"UI\\ADBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\ADBTN1B.PCX", 32,412,144,28,NO_KEY,BuildAdvRestart, 0, -1},
{BA_DONE,             BUTTON_BITMAP,0,0,STR_BA_DONE,   BTN_LABEL_COLOR,"UI\\ADBTN2A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\ADBTN2B.PCX", 32,444,144,28,NO_KEY,BuildAdvDone, 0, -1},
};
LONG BuildAdvSize = sizeof(BuildAdv) / sizeof(BUTTON);

BUTTON AdvPrep[] = {
{0,BUTTON_BITMAP,D_FREEZE,   0,-1,           0,"UI\\ADVPREP.PCX",0,"",0,0,640,480,NO_KEY,AdvPrepPaint,0,-1},
{9,BUTTON_REGION,0,                0,STR_BA_TITLE,   31,"",0,"", 15,  2,170,20,NO_KEY,NULL, 0, -1},
// adventure list box 
{9,BUTTON_REGION,D_TEXT_LEFT,    0,STR_CS_SELECT,173,"",0,"", 16, 55,176,25,NO_KEY,NULL, 0, -1},
{BA_LIST_01,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16, 95,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_02,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,110,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_03,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,125,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_04,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,140,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_05,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,155,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_06,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,170,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_07,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,185,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_08,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,200,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{9,BUTTON_REGION,D_TEXT_LEFT,    0,STR_CS_CHOOSE,173,"",0,"", 16,230,176,25,NO_KEY,NULL, 0, -1},
{9,BUTTON_REGION,D_TEXT_LEFT,    0,STR_BA_CHOOSE,173,"",0,"", 16,257,176, 8,NO_KEY,NULL, 0, -1},
{BA_LIST_09,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,270,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_10,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,285,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_11,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,300,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_12,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,315,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_13,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,330,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_14,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,345,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_15,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,360,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_16,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,375,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_RESET,      BUTTON_BITMAP,0,0,STR_CANCEL,    BTN_LABEL_COLOR,"UI\\ADBTN1A.PCX", 31,"UI\\ADBTN1B.PCX",  32,412,144,28,KEY_A,AdvPrepCancel, 0, -1},
{BA_DONE,       BUTTON_BITMAP,0,0,STR_BA_DONE,   BTN_LABEL_COLOR,"UI\\ADBTN2A.PCX", 31,"UI\\ADBTN2B.PCX",  32,444,144,28,NO_KEY,AdvPrepDone, 0, -1},
{ADV_LIST1_UP,  BUTTON_BITMAP,0,0,-1,            0,"UI\\ADVTNU1A.PCX",0,"UI\\ADBTNU1B.PCX",192, 76, 24,60,NO_KEY,AdvPrepListScroll, 1, -1},
{ADV_LIST1_DOWN,BUTTON_BITMAP,0,0,-1,            0,"UI\\ADVTND1A.PCX",0,"UI\\ADBTND1B.PCX",192,136, 24,60,NO_KEY,AdvPrepListScroll, 2, -1},
{ADV_LIST2_UP,  BUTTON_BITMAP,0,0,-1,            0,"UI\\ADBTNU1A.PCX",0,"UI\\ADBTNU2B.PCX",192,256, 24,60,NO_KEY,AdvPrepListScroll, 3, -1},
{ADV_LIST2_DOWN,BUTTON_BITMAP,0,0,-1,            0,"UI\\ADBTND1A.PCX",0,"UI\\ADBTND2B.PCX",192,316, 24,60,NO_KEY,AdvPrepListScroll, 4, -1},
};
LONG AdvPrepSize = sizeof(AdvPrep) / sizeof(BUTTON);

BUTTON GameSelMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE,0,-1,             		  0,"UI\\HELPPNL1.PCX",0,"",200, 75,351,371,NO_KEY,PaintGameSelect,0, -1},
{1,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_GAMETYPE,     31,"",0,"", 48, 80,152, 20,NO_KEY,NULL, 0, -1},
{2,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_HELP,        173,"",31,"", 48,114,152, 20,KEY_R,SelectGameType, 0, -1},
{3,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_INTROGAME,   173,"",31,"", 48,134,152, 20,KEY_P,SelectGameType, 1, -1},
{4,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_FULLGAME,    173,"",31,"", 48,154,152, 20,KEY_C,SelectGameType, 2, -1},
{5,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_ADVENTURE,   173,"",31,"", 48,174,152, 20,KEY_V,SelectGameType, 3, -1},
{6,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_BATTLE,      173,"",31,"", 48,194,152, 20,KEY_B,SelectGameType, 4, -1},
{7,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_BACKGROUND,  173,"",31,"", 48,214,152, 20,KEY_H,SelectGameType, 5, -1},
{8,BUTTON_REGION,0,       	   0,-1,             		   0,"",0,"", 44,230,152, 20,NO_KEY,NULL, 0, -1},
{9,BUTTON_BITMAP,0,			   0,STR_CANCEL,			 BTN_LABEL_COLOR,"",31,"UI\\HLPBTNAB.PCX", 76,252, 84, 28,KEY_A ,HideSubMenuWithClick, D_SELECT_GAME, -1},
{10,BUTTON_REGION,0,      0,-1,             		  0,"",0,"",  0,  0,  0,  0,KEY_ESCAPE,HideSubMenu, D_SELECT_GAME, -1},
};
LONG GameSelMenuSize = sizeof(GameSelMenu) / sizeof(BUTTON);

BUTTON GameCmplxMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE,        0,-1,             0,"UI\\DIALPNL1.PCX",0,"", 202,136,236,208,NO_KEY,PaintGameComplexity,0, -1},
{1,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,0,STR_SELECT_COMPLEXITY, 31,"",0,"", 48, 80,152, 20,NO_KEY,NULL, 0, -1},
{2,BUTTON_REGION,0,               0,-1,             173,"",           31,"", 44,109,152, 20,KEY_B,SelectGameType, 10, -1},
{3,BUTTON_REGION,0,               0,-1,             173,"",           31,"", 44,129,152, 20,KEY_V,SelectGameType, 11, -1},
{4,BUTTON_REGION,0,               0,-1,             173,"",           31,"", 44,149,152, 20,KEY_E,SelectGameType, 12, -1},
{9,BUTTON_BITMAP,0,     		  0,STR_CANCEL,BTN_LABEL_COLOR,  "UI\\HLPBTNCA.PCX",31,"UI\\HLPBTNCB.PCX", 76,172, 84, 28,KEY_A ,HideSubMenuWithClick, D_GAME_COMPLEXITY, -1},
{10,BUTTON_REGION,0,              0,-1,              0,"",0,"",  0,  0,  0,  0,KEY_ESCAPE,HideSubMenu, D_GAME_COMPLEXITY, -1},
};
LONG GameCmplxMenuSize = sizeof(GameCmplxMenu) / sizeof(BUTTON);



BUTTON MultiWait[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\CHATPNL2.PCX",0,"", 83, 80, 472, 320,NO_KEY, 0, 0, -1},
{1,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 100,440,15,NO_KEY,0, 0,-1},
{2,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 116,440,15,NO_KEY,0, 1,-1},
{3,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 132,440,15,NO_KEY,0, 2,-1},
{4,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 148,440,15,NO_KEY,0, 3,-1},
{5,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 164,440,15,NO_KEY,0, 4,-1},
{6,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 180,440,15,NO_KEY,0, 5,-1},
{7,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 196,440,15,NO_KEY,0, 6,-1},
//{8,BUTTON_BITMAP,0, 0,STR_CANCEL1, BTN_LABEL_COLOR,"UI\\CHTBTN2A.PCX",31,"UI\\CHTBTN2B.PCX", 196, 220, 84, 28,KEY_ESCAPE,HideSubMenuWithClick, D_MULTIWAIT, -1},
};
LONG MultiWaitSize = sizeof(MultiWait) / sizeof(BUTTON);

BUTTON DescribeMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE,        0,-1,             0,"",0,                "",160,120,320,264, NO_KEY,    PaintDescribe, 0, -1},
{1,BUTTON_REGION,D_HILIGHTED,     0,-1,             0,"",0,"UI\\MARBBTNA.PCX",134,240, 52, 16, KEY_SPACE, IncDescribe,   0, -1},
};
LONG DescribeMenuSize = sizeof(DescribeMenu) / sizeof(BUTTON);

BUTTON SystemLargeHelp[] = {
{SYSHELP_DIALOG,  BUTTON_BITMAP,D_DONT_SHADE|D_FREEZE|D_ZOOM, 0,-1,   0,"UI\\help_lg.PCX",  0,"",145, 50,356,372,NO_KEY,SystemHelpPaintProc,0, -1},
{SYSHELP_TITLE,   BUTTON_REGION,0,               0,-1,                0,"",                 0,"", 15, 10,326, 20,NO_KEY,NULL,0, -1},
{SYSHELP_TEXTBODY,BUTTON_REGION,0,               0,-1,                0,"",                 0,"", 15, 35,326,300,NO_KEY,NULL,0, -1},
{SYSHELP_DONE,    BUTTON_BITMAP,0,               0,STR_HELP_DONE,    BTN_LABEL_COLOR,"UI\\hlpbtnaa.pcx",31,"UI\\hlpbtnab.pcx", 40,339,100, 28,KEY_O ,HideSubMenuWithClick, D_SYSHELP_LARGE, -1},
{SYSHELP_MOREINFO,BUTTON_BITMAP,0,               0,STR_HELP_MOREINFO,BTN_LABEL_COLOR,"UI\\hlpbtnaa.pcx",31,"UI\\hlpbtnab.pcx",224,339,100, 28,KEY_I ,NULL,0, -1},
};
LONG SystemLargeHelpSize = sizeof(SystemLargeHelp) / sizeof(BUTTON);

BUTTON SystemMedHelp[] = {
{SYSHELP_DIALOG,  BUTTON_BITMAP,D_DONT_SHADE|D_FREEZE|D_ZOOM, 0,-1,   0,"UI\\help_md.PCX", 0,"",145,150,356,188,NO_KEY,SystemHelpPaintProc,0, -1},
{SYSHELP_TITLE,   BUTTON_REGION,0,               0,-1,                0,"",                0,"", 15, 10,326, 20,NO_KEY,NULL,0, -1},
{SYSHELP_TEXTBODY,BUTTON_REGION,0,               0,-1,                0,"",                0,"", 15, 35,326,115,NO_KEY,NULL,0, -1},
{SYSHELP_DONE,    BUTTON_BITMAP,0,               0,STR_HELP_DONE,    BTN_LABEL_COLOR,"UI\\hlpbtnaa.pcx",31,"UI\\hlpbtnab.pcx", 40,155,100, 28,KEY_O ,HideSubMenuWithClick, D_SYSHELP_MED, -1},
{SYSHELP_MOREINFO,BUTTON_BITMAP,0,               0,STR_HELP_MOREINFO,BTN_LABEL_COLOR,"UI\\hlpbtnaa.pcx",31,"UI\\hlpbtnab.pcx",224,155,100, 28,KEY_I ,NULL,0, -1},
};
LONG SystemMedHelpSize = sizeof(SystemMedHelp) / sizeof(BUTTON);

BUTTON SystemSmallHelp[] = {
{SYSHELP_DIALOG,  BUTTON_BITMAP,D_DONT_SHADE|D_FREEZE|D_ZOOM, 0,-1,   0,"UI\\help_sm.PCX",  0,"",145,250,356,112,NO_KEY,SystemHelpPaintProc,0, -1},
{SYSHELP_TITLE,   BUTTON_REGION,0,               0,-1,                0,"",                 0,"", 15,  8,326, 20,NO_KEY,NULL,0, -1},
{SYSHELP_TEXTBODY,BUTTON_REGION,0,               0,-1,                0,"",                 0,"", 15, 35,326, 48,NO_KEY,NULL,0, -1},
{SYSHELP_DONE,    BUTTON_BITMAP,0,               0,STR_HELP_DONE,    BTN_LABEL_COLOR,"UI\\hlpbtnaa.pcx",31,"UI\\hlpbtnab.pcx", 40, 79,100, 28,KEY_O ,HideSubMenuWithClick, D_SYSHELP_SMALL, -1},
{SYSHELP_MOREINFO,BUTTON_BITMAP,0,               0,STR_HELP_MOREINFO,BTN_LABEL_COLOR,"UI\\hlpbtnaa.pcx",31,"UI\\hlpbtnab.pcx",224, 79,100, 28,KEY_I ,NULL,0, -1},
};
LONG SystemSmallHelpSize = sizeof(SystemSmallHelp) / sizeof(BUTTON);

BUTTON TutorialMenuWide[] = {
{TUTORIAL_MENU_BACKGROUND,  BUTTON_BITMAP,D_FREEZE, 0,-1,           0,"UI\\MARBLE06.PCX", 0,"",100, 50,321,263,NO_KEY,PaintTutorial,0, -1},
{TUTORIAL_MENU_HEAD,   BUTTON_BITMAP,0,             0,-1,           0,"UI\\HEADCNSL.PCX", 0,"",-24,-50, 64, 96,NO_KEY,NULL,0, -1},
{TUTORIAL_MENU_TEXT,   BUTTON_REGION,0,             0,-1,           0,                "", 0,"", 10, 46,302,170,NO_KEY,NULL,0, -1},
{TUTORIAL_MENU_DONE,   BUTTON_BITMAP,0,             0,STR_HELP_DONE,0,"UI\\MARBBTNA.PCX",31,"UI\\MARBBTNB.PCX",135,231, 51, 15,KEY_O,TutorialDone, 0, -1},
};

LONG TutorialMenuWideSize = sizeof(TutorialMenuWide) / sizeof(BUTTON);

BUTTON TutorialMenuMed[] = {
{TUTORIAL_MENU_BACKGROUND,  BUTTON_BITMAP,D_FREEZE, 0,-1,           0,"UI\\MARBLE03.PCX",  0,"",100, 50,199,161,NO_KEY,PaintTutorial,0, -1},
{TUTORIAL_MENU_HEAD,   BUTTON_BITMAP,0,             0,-1,           0,"UI\\HEADCNSL.PCX",  0,"",-24,-50, 64, 96,NO_KEY,NULL,0, -1},
{TUTORIAL_MENU_TEXT,   BUTTON_REGION,0,             0,-1,           0,                "",  0,"", 10, 46,179, 79,NO_KEY,NULL,0, -1},
{TUTORIAL_MENU_DONE,   BUTTON_BITMAP,0,             0,STR_HELP_DONE,0,"UI\\MARBBTNA.PCX", 31,"UI\\MARBBTNB.PCX", 75,140, 51, 15,KEY_O,TutorialDone, 0, -1},
};

LONG TutorialMenuWideMed = sizeof(TutorialMenuMed) / sizeof(BUTTON);

BUTTON LoadingScene[] = {
{0,BUTTON_BITMAP,D_NO_PUSH,0,-1,0,"UI\\help_sm.PCX",0,"",145,250,356,112,NO_KEY,LoadingSceneProc,0, -1},
};
LONG LoadingSceneSize = sizeof(LoadingScene) / sizeof(BUTTON);

BUTTON Parchment[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0, -1,         0,"UI\\HNTPANL1.PCX",0,"",   154,60,352,370,NO_KEY,NULL,0, -1},
{PARCHMENT_TEXT,  BUTTON_REGION,0,0,-1,         1,"",0,"",38,118,216,116,NO_KEY,NULL,0, -1},
{PARCHMENT_CLICK, BUTTON_REGION,0,0,STR_CLICK,204,"",0,"", 0,330,352, 28,NO_KEY,NULL,0, -1},
{PARCHMENT_OK_BTN,BUTTON_REGION,0,0,-1,         0,"",0,"", 0,  0,352,370,KEY_ENTER,ParchmentDone,0, -1},
};
LONG ParchmentSize = sizeof(Parchment) / sizeof(BUTTON);

#else // english ver

BUTTON AvatarAttributes[] = {
{ 0,BUTTON_BITMAP, D_FREEZE|D_ZOOM, 0,-1, 0,"UI\\STS_PANL.PCX",0,"",112, 20,368,436,NO_KEY,PaintAttributes,0,-1},

{ 1,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                 31,"",0,"", 42, 21,223, 32,NO_KEY,NULL,0,-1},
{ 2,BUTTON_BITMAP, 0,           0,STR_AVA_ATT_ANIM,  BTN_LABEL_COLOR,"UI\\STSBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\STSBTNAB.PCX", 52,400, 84, 28,KEY_M,PlayAnimation,0,-1},
{ 3,BUTTON_BITMAP, 0,           0,-1,                BTN_LABEL_COLOR,"UI\\STSBTNIA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\STSBTNIB.PCX",188,400, 84, 28,KEY_I,InventoryFromStatus,0,-1},
{ 4,BUTTON_BITMAP, 0,           0,STR_AVA_ATT_DONE,  BTN_LABEL_COLOR,"UI\\STSBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\STSBTNXB.PCX",272,400, 84, 28,KEY_D,HideStatus,0,-1},
{ 5,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                 31,"",0,"",184, 81,158, 10,NO_KEY,NULL,0,-1},
{ 6,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_CLASS, 173,"",0,"",184,112,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_CLASS,-1},
{ 7,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_LEVEL, 173,"",0,"",184,128,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_LEVEL,-1},
{ 8,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_BLOOD, 173,"",0,"",184,144,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_BLOOD,-1},
{ 9,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_ALIGN, 173,"",0,"",184,159,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_ALIGN,-1},
{10,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_STR,   173,"",0,"",184,200,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_STR,-1},
{11,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_INT,   173,"",0,"",184,216,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_INT,-1},
{12,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_DEX,   173,"",0,"",184,232,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_DEX,-1},
{13,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_WIS,   173,"",0,"",184,248,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_WIS,-1},
{14,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_CON,   173,"",0,"",184,264,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_CON,-1},
{15,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_CHA,   173,"",0,"",184,280,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_CHA,-1},
{16,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_THA,   173,"",0,"",184,296,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_THAC0,-1},
{17,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_EXP,   173,"",0,"",184,328,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_XP ,-1},
{18,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_ARM,   173,"",0,"",184,344,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_PROT,-1},
{19,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_HIT,   173,"",0,"",184,360,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_HP,-1},
{20,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_HP,    173,"",0,"",184,376,158, 10,NO_KEY,ShowStatusContextHelp,(LONG)STR_STATUS_HELP_CURRHP,-1},
{21,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                  0,"",0,"", 16,108,152,204,NO_KEY,BowToPlayer,0,-1},
{22,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                  0,"",0,"", 20,329,134, 20,NO_KEY,NULL,0,-1},
{23,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                  0,"",0,"",  0,  0,  0,  0,KEY_ESCAPE,HideStatus,0,-1},
};
LONG AvatarAttributesSize = sizeof(AvatarAttributes) / sizeof(BUTTON);


BUTTON AvatarAttributes2[] = {
{ 0,BUTTON_BITMAP, D_FREEZE|D_ZOOM, 0,-1, 0,"UI\\STS_PANL.PCX",0,"",100, 20,368,436,NO_KEY,PaintAttributes,0,-1},

{ 1,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                 31,"",0,"", 42, 21,223, 32,NO_KEY,NULL,0,-1},
{ 2,BUTTON_BITMAP, 0,			0,STR_AVA_ATT_ANIM,  BTN_LABEL_COLOR,"UI\\STSBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\STSBTNAB.PCX", 52,400, 84, 28,KEY_M,PlayAnimation,0,-1},
{ 3,BUTTON_REGION, 0,			0,-1,                0,"",0,"",188,400, 84, 28,NO_KEY,NULL,0,-1},
{ 4,BUTTON_BITMAP, 0,			0,STR_AVA_ATT_DONE,  BTN_LABEL_COLOR,"UI\\STSBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\STSBTNXA.PCX",272,400, 84, 28,KEY_D,HideStatus2,0,-1},
{ 5,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                 31,"",0,"",184, 81,158, 10,NO_KEY,NULL,0,-1},
{ 6,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_CLASS, 173,"",0,"",184,112,158, 10,NO_KEY,NULL,0,-1},
{ 7,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_LEVEL, 173,"",0,"",184,128,158, 10,NO_KEY,NULL,0,-1},
{ 8,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_BLOOD, 173,"",0,"",184,144,158, 10,NO_KEY,NULL,0,-1},
{ 9,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_ALIGN, 173,"",0,"",184,159,158, 10,NO_KEY,NULL,0,-1},
{10,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_STR,   173,"",0,"",184,200,158, 10,NO_KEY,NULL,0,-1},
{11,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_INT,   173,"",0,"",184,216,158, 10,NO_KEY,NULL,0,-1},
{12,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_DEX,   173,"",0,"",184,232,158, 10,NO_KEY,NULL,0,-1},
{13,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_WIS,   173,"",0,"",184,248,158, 10,NO_KEY,NULL,0,-1},
{14,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_CON,   173,"",0,"",184,264,158, 10,NO_KEY,NULL,0,-1},
{15,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_CHA,   173,"",0,"",184,280,158, 10,NO_KEY,NULL,0,-1},
{16,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_THA,   173,"",0,"",184,296,158, 10,NO_KEY,NULL,0,-1},
{17,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_EXP,   173,"",0,"",184,328,158, 10,NO_KEY,NULL,0,-1},
{18,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_ARM,   173,"",0,"",184,344,158, 10,NO_KEY,NULL,0,-1},
{19,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_HIT,   173,"",0,"",184,360,158, 10,NO_KEY,NULL,0,-1},
{20,BUTTON_REGION, D_TEXT_LEFT, 0,STR_AVA_ATT_HP,    173,"",0,"",184,376,158, 10,NO_KEY,NULL,0,-1},
{21,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                  0,"",0,"", 16,108,152,204,NO_KEY,BowToPlayer,0,-1},
{22,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                  0,"",0,"", 20,329,134, 20,NO_KEY,NULL,0,-1},
{23,BUTTON_REGION, D_TEXT_LEFT, 0,-1,                  0,"",0,"",  0,  0,  0,  0,KEY_ESCAPE,HideStatus2,0,-1},
};
LONG AvatarAttributes2Size = sizeof(AvatarAttributes2) / sizeof(BUTTON);



BUTTON AvatarInventory[] = {
{INV_BUT_ALL      ,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,0,    "UI\\INV_PANL.PCX",0,"",100, 50,392,348,	NO_KEY,	StaticInventoryGUI::mfPaint,				0,			-1},
{INV_BUT_TITLE    ,BUTTON_REGION,D_TEXT_LEFT,0,STR_AVA_INV_INV,173,   "",0,"",                 42, 21,223, 32,	KEY_I,	NULL,										0,			-1},
{INV_BUT_DONE     ,BUTTON_BITMAP,0,0,STR_AVA_INV_DONE,BTN_LABEL_COLOR,  "UI\\INVBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTNXB.PCX",300,312, 84, 28,	KEY_D,	StaticInventoryGUI::mfDone,					0,			-1},
{INV_BUT_STATUS   ,BUTTON_BITMAP,0,0,STR_AVA_INV_STATUS,BTN_LABEL_COLOR,"UI\\INVBTNSA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTNSB.PCX",216,312, 84, 28,	KEY_S,	StaticInventoryGUI::mfGoStatus,				0,			-1},
{INV_BUT_DUDENAME, BUTTON_REGION,0,0,-1,0,"",0,"",					125,	80, 	230,	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_INFO,     BUTTON_REGION,0,0,-1,0,"",0,"",					120,	240,	240,	45, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_HELPAST,  BUTTON_REGION,0,0,STR_AVA_INV_HELP_ASTERISK,173,"",0,"",120,285,	240,	15, 	NO_KEY,	NULL,										0,			-1}, //context sensitive help here?
{INV_BUT_LISTBOX,  BUTTON_REGION,0,0,-1,0,"",0,"",					120,	108,	239,	119,	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNUMBER1,BUTTON_REGION,0,0,-1,0,"",0,"",					152,	107,	19, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNUMBER2,BUTTON_REGION,0,0,-1,0,"",0,"",					152,	127,	19, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNUMBER3,BUTTON_REGION,0,0,-1,0,"",0,"",					152,	147,	19, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNUMBER4,BUTTON_REGION,0,0,-1,0,"",0,"",					152,	167,	19, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNUMBER5,BUTTON_REGION,0,0,-1,0,"",0,"",					152,	187,	19, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNUMBER6,BUTTON_REGION,0,0,-1,0,"",0,"",					152,	207,	19, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBICON1,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	107,	29, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBICON2,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	127,	29, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBICON3,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	147,	29, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBICON4,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	167,	29, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBICON5,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	187,	29, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBICON6,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	207,	29, 	20, 	NO_KEY,	NULL,										0,			-1},
{INV_BUT_LBNAME1,  BUTTON_REGION,0,0,-1,0,"",0,"",					171,	107,	188,	20, 	NO_KEY,	StaticInventoryGUI::mfSetHighlightedLine,	0,			-1},
{INV_BUT_LBNAME2,  BUTTON_REGION,0,0,-1,0,"",0,"",					171,	127,	188,	20, 	NO_KEY,	StaticInventoryGUI::mfSetHighlightedLine,	1,			-1},
{INV_BUT_LBNAME3,  BUTTON_REGION,0,0,-1,0,"",0,"",					171,	147,	188,	20, 	NO_KEY,	StaticInventoryGUI::mfSetHighlightedLine,	2,			-1},
{INV_BUT_LBNAME4,  BUTTON_REGION,0,0,-1,0,"",0,"",					171,	167,	188,	20, 	NO_KEY,	StaticInventoryGUI::mfSetHighlightedLine,	3,			-1},
{INV_BUT_LBNAME5,  BUTTON_REGION,0,0,-1,0,"",0,"",					171,	187,	188,	20, 	NO_KEY,	StaticInventoryGUI::mfSetHighlightedLine,	4,			-1},
{INV_BUT_LBNAME6,  BUTTON_REGION,0,0,-1,0,"",0,"",					171,	207,	188,	20, 	NO_KEY,	StaticInventoryGUI::mfSetHighlightedLine,	5,			-1},
{INV_BUT_LBPLUS1,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	107,	15, 	20, 	NO_KEY,	StaticInventoryGUI::mfAddToAvailSpells,		0,			-1},
{INV_BUT_LBPLUS2,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	127,	15, 	20, 	NO_KEY,	StaticInventoryGUI::mfAddToAvailSpells,		1,			-1},
{INV_BUT_LBPLUS3,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	147,	15, 	20, 	NO_KEY,	StaticInventoryGUI::mfAddToAvailSpells,		2,			-1},
{INV_BUT_LBPLUS4,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	167,	15, 	20, 	NO_KEY,	StaticInventoryGUI::mfAddToAvailSpells,		3,			-1},
{INV_BUT_LBPLUS5,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	187,	15, 	20, 	NO_KEY,	StaticInventoryGUI::mfAddToAvailSpells,		4,			-1},
{INV_BUT_LBPLUS6,  BUTTON_REGION,0,0,-1,0,"",0,"",					123,	207,	15, 	20, 	NO_KEY,	StaticInventoryGUI::mfAddToAvailSpells,		5,			-1},
{INV_BUT_LBMINUS1, BUTTON_REGION,0,0,-1,0,"",0,"",					138,	107,	14, 	20, 	NO_KEY,	StaticInventoryGUI::mfTakeFromAvailSpells,	0,			-1},
{INV_BUT_LBMINUS2, BUTTON_REGION,0,0,-1,0,"",0,"",					138,	127,	14, 	20, 	NO_KEY,	StaticInventoryGUI::mfTakeFromAvailSpells,	1,			-1},
{INV_BUT_LBMINUS3, BUTTON_REGION,0,0,-1,0,"",0,"",					138,	147,	14, 	20, 	NO_KEY,	StaticInventoryGUI::mfTakeFromAvailSpells,	2,			-1},
{INV_BUT_LBMINUS4, BUTTON_REGION,0,0,-1,0,"",0,"",					138,	167,	14, 	20, 	NO_KEY,	StaticInventoryGUI::mfTakeFromAvailSpells,	3,			-1},
{INV_BUT_LBMINUS5, BUTTON_REGION,0,0,-1,0,"",0,"",					138,	187,	14, 	20, 	NO_KEY,	StaticInventoryGUI::mfTakeFromAvailSpells,	4,			-1},
{INV_BUT_LBMINUS6, BUTTON_REGION,0,0,-1,0,"",0,"",					138,	207,	14, 	20, 	NO_KEY,	StaticInventoryGUI::mfTakeFromAvailSpells,	5,			-1},
{INV_BUT_MUNDANE,  BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN1B.PCX",	12, 	108,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfChangeCurrCat,		IC_MUNDANE,	-1},
{INV_BUT_ARMS,     BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN2A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN2B.PCX",	12, 	132,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfChangeCurrCat,		IC_MAGITEM,	-1},
{INV_BUT_SPELLS,   BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN3A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN3B.PCX",	12, 	156,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfChangeCurrCat,		IC_SPELL,	-1},
{INV_BUT_MAGICITMS,BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN4A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN4B.PCX",	12, 	180,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfChangeCurrCat,		IC_REALM_SPELL,-1},
{INV_BUT_BLOODABLT,BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN5A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN5B.PCX",	12, 	204,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfChangeCurrCat,		IC_BLOODAB,	-1},
{INV_BUT_USE,      BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN6A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN6B.PCX",	12, 	236,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfUseHighlightedObject,	0,			-1},
{INV_BUT_DROP,     BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN7A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN7B.PCX",	12, 	260,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfDropItem,				0,			-1},
{INV_BUT_GIVE,     BUTTON_BITMAP,0,0,-1,BTN_LABEL_COLOR,"UI\\INVBTN8A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\INVBTN8B.PCX",	12, 	284,	100,	24, 	NO_KEY,	StaticInventoryGUI::mfGiveItem,				0,			-1},
{INV_BUT_SCROLLUP, BUTTON_BITMAP,0,0,-1,0,"UI\\INVBTNUA.PCX",0,"UI\\INVBTNUB.PCX",	360,	108,	24, 	60, 	NO_KEY,	StaticInventoryGUI::mfScroll,				-1,			-1},
{INV_BUT_SCROLLDN, BUTTON_BITMAP,0,0,-1,0,"UI\\INVBTNDA.PCX",0,"UI\\INVBTNDB.PCX",	360,	168,	24, 	60, 	NO_KEY,	StaticInventoryGUI::mfScroll,				1,			-1},
{INV_BUT_SPELLBOX1,BUTTON_REGION,0,0,-1,0,"",0,"",					15, 	317,	25, 	18, 	KEY_1,	StaticInventoryGUI::mfClearSpellBox,		0,			-1},
{INV_BUT_SPELLBOX2,BUTTON_REGION,0,0,-1,0,"",0,"",					47, 	317,	25, 	18, 	KEY_2,	StaticInventoryGUI::mfClearSpellBox,		1,			-1},
{INV_BUT_SPELLBOX3,BUTTON_REGION,0,0,-1,0,"",0,"",					79, 	317,	25, 	18, 	KEY_3,	StaticInventoryGUI::mfClearSpellBox,		2,			-1},
{INV_BUT_SPELLBOX4,BUTTON_REGION,0,0,-1,0,"",0,"",					111,	317,	25, 	18, 	KEY_4,	StaticInventoryGUI::mfClearSpellBox,		3,			-1},
{INV_BUT_SPELLBOX5,BUTTON_REGION,0,0,-1,0,"",0,"",					143,	317,	25, 	18, 	KEY_5,	StaticInventoryGUI::mfClearSpellBox,		4,			-1},
{INV_BUT_SPELLBOX6,BUTTON_REGION,0,0,-1,0,"",0,"",					175,	317,	25, 	18, 	KEY_6,	StaticInventoryGUI::mfClearSpellBox,		5,			-1},
{665     ,BUTTON_REGION,0,0,-1,173,  "",0,"",0,0,0,0,KEY_ESCAPE,StaticInventoryGUI::mfDone,0,-1},
};
LONG AvatarInventorySize = sizeof(AvatarInventory) / sizeof(BUTTON);

BUTTON MessageBoxTitled[] = {
{MSGBOX_BUT_ALL   ,BUTTON_BITMAP, D_FREEZE|D_ZOOM, 0,-1,0,"UI\\HELP_SM.PCX",0,"",105,100,356,112,NO_KEY,TitledMessageBox::PrintMessage,0,-1},
{MSGBOX_BUT_TITLE ,BUTTON_REGION, 0, 0,-1,31,"",0,"",4,19,356,31,KEY_SPACE,TitledMessageBox::Hide,0,-1},
{MSGBOX_BUT_MSG   ,BUTTON_REGION, 0, 0,-1,31,"",0,"",4,35,356,55,KEY_ENTER,TitledMessageBox::Hide,0,-1},
{MSGBOX_BUT_OK    ,BUTTON_BITMAP, 0, 0,STR_OK,BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAB.PCX",128,76,100,28,KEY_O,TitledMessageBox::Hide,0,-1},
};
LONG MessageBoxTitledSize = sizeof(MessageBoxTitled) / sizeof(BUTTON);

BUTTON SmallInfoBox[] = {
{INFOBOX_BUT_ALL   ,BUTTON_BITMAP, D_FREEZE|D_ZOOM|D_DONT_SHADE, 0,-1,0,"UI\\INFOBOX.PCX",0,"",160,100,319,41,NO_KEY,InfoBox::Paint,0,-1},
};
LONG InfoBoxSize = sizeof(SmallInfoBox) / sizeof(BUTTON);


BUTTON ResurrectionScreen[] = {
{RESURRECT_BUT_ALL	,BUTTON_BITMAP, D_FREEZE|D_ZOOM, 0,-1,0,"UI\\RESURECT.PCX",0,"",160,100,330,170,NO_KEY,ResurrectionMenu::Paint,0,-1},
{RESURRECT_BUT_TITLE,BUTTON_REGION, 0, 0,STR_ITEM_UI_RESURRECTION,31,"",0,"",4, 4,322, 24,NO_KEY,NULL,0,-1},
{RESURRECT_BUT_Q	,BUTTON_REGION, 0, 0,STR_ITEM_UI_RESURRECTION_QUESTION,31,"",0,"",4,42,322,15,NO_KEY,NULL,0,-1},
{RESURRECT_BUT_NAME1,BUTTON_REGION, 0, 0,-1,31,"",0,"",4,57,322,18,NO_KEY,ResurrectionMenu::Resurrect,0,-1},
{RESURRECT_BUT_NAME2,BUTTON_REGION, 0, 0,-1,31,"",0,"",4,75,322,18,NO_KEY,ResurrectionMenu::Resurrect,1,-1},
{RESURRECT_BUT_NAME3,BUTTON_REGION, 0, 0,-1,31,"",0,"",4,93,322,18,NO_KEY,ResurrectionMenu::Resurrect,2,-1},
{RESURRECT_BUT_NAME4,BUTTON_REGION, 0, 0,-1,31,"",0,"",4,111,322,18,NO_KEY,ResurrectionMenu::Resurrect,3,-1},
{RESURRECT_BUT_CANCL,BUTTON_REGION, 0, 0,STR_CANCEL,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"",126,129,75,19,KEY_C,HideSubMenuWithClick,D_RESURRECT_MENU,-1},
{665,BUTTON_REGION, 0, 0,-1,31,"",0,"",0,0,0,0,KEY_ESCAPE,HideSubMenu,D_RESURRECT_MENU,-1},
};
LONG ResurrectionScreenSize = sizeof(ResurrectionScreen) / sizeof(BUTTON);


BUTTON BtlControl[] = {
{ 0,          BUTTON_BITMAP,0,0,-1,             0,"UI\\BATLPNL1.PCX",0,"",0,316,640,164,NO_KEY,BattleControlPaint,0,-1},
{ 2,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  338, 93, 55, 40,NO_KEY,BattleControlGrid, 0,-1},
{ 3,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  398, 93, 55, 40,NO_KEY,BattleControlGrid, 1,-1},
{ 4,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  458, 93, 55, 40,NO_KEY,BattleControlGrid, 2,-1},
{ 5,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  518, 93, 55, 40,NO_KEY,BattleControlGrid, 3,-1},
{ 6,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  578, 93, 55, 40,NO_KEY,BattleControlGrid, 4,-1},
{ 7,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  338, 49, 55, 40,NO_KEY,BattleControlGrid, 5,-1},
{ 8,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  398, 49, 55, 40,NO_KEY,BattleControlGrid, 6,-1},
{ 9,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  458, 49, 55, 40,NO_KEY,BattleControlGrid, 7,-1},
{10,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  518, 49, 55, 40,NO_KEY,BattleControlGrid, 8,-1},
{11,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  578, 49, 55, 40,NO_KEY,BattleControlGrid, 9,-1},
{12,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  338,  5, 55, 40,NO_KEY,BattleControlGrid,10,-1},
{13,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  398,  5, 55, 40,NO_KEY,BattleControlGrid,11,-1},
{14,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  458,  5, 55, 40,NO_KEY,BattleControlGrid,12,-1},
{15,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  518,  5, 55, 40,NO_KEY,BattleControlGrid,13,-1},
{16,          BUTTON_REGION,0,0,-1,             0,"",0,"UI\\GRID0H.PCX",  578,  5, 55, 40,NO_KEY,BattleControlGrid,14,-1},
{17,          BUTTON_REGION,0,0,-1,             0,"",0,"",                  7,  5,323,100,NO_KEY,BattleReservesGrid,D_BTL_CONTROL,-1},
{BTL_RESV_ID, BUTTON_REGION,0,0,BTLUI_RESV,     BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLBTN1B.PCX", 40,136, 84, 28,KEY_R,BattleReservesMode,0,-1},
{BTL_STAT_ID, BUTTON_REGION,0,0,BTLUI_STATUS,   BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLBTN2B.PCX",124,136, 84, 28,KEY_S,BattleStatusMode,0,-1},
{BTL_EXIT_ID, BUTTON_REGION,0,0,BTLUI_RETREAT,  BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLBTN3B.PCX",208,136, 84, 28,KEY_T,BattleRetreat,0,-1},
{BTL_DONE_ID, BUTTON_BITMAP,0,0,STR_DONE,       GREY,"UI\\BTLBTN3B.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTLBTN3A.PCX",318,136, 84, 28,KEY_D,BattleNextTurn,0,-1},
{BTL_VIEW_ID, BUTTON_REGION,0,0,BTLUI_COMMAND,  BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLBTN4B.PCX",424,136,124, 28,KEY_V,BattleCameraHighView,0,-1},
{BTL_CANCEL_ID,BUTTON_REGION,0,0,STR_CANCEL1,   BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLCANCE.PCX",150,107, 70, 21,NO_KEY,BattleCancel,0,-1},
{BTL_FALLBACK_ID,BUTTON_REGION,0,0,BTLUI_FALLBACK,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLCANCE.PCX", 70,107, 70, 21,NO_KEY,BattleFallBack,0,-1},
{BTL_LIGHT_ID,BUTTON_REGION,0,0,-1,             0,"",0,"UI\\LIGHT.PCX",     7,  0,129,135,NO_KEY,NULL,0,-1},

//{BTL_SHOOT_ID,BUTTON_REGION,0,0,BTLUI_MISSILE,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLBTN5B.PCX",292,136, 64, 28,KEY_M,BattleSelMissile,0,-1},
//{BTL_MAGIC_ID,BUTTON_REGION,0,0,BTLUI_MAGIC,  BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLBTN6B.PCX",356,136, 64, 28,KEY_G,BattleSelMagic,0,-1},
//{BTL_CHAT_ID, BUTTON_REGION,0,0,BTLUI_CHAT,   BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLBTN3B.PCX",152,136, 64, 28,KEY_C,BattleChatMode,0,-1},
//{BTL_FOLW_ID, BUTTON_REGION,0,0,BTLUI_FOLLOW, BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\BTLBTN8B.PCX",544,136, 92, 28,KEY_F,BattleCameraFollow,0,-1},
};
LONG BtlControlSize = sizeof(BtlControl) / sizeof(BUTTON);


// Note: The next four Button menus are nearly identical. Be sure to duplicate
//       any changes in each of the menus.

BUTTON CombatControl1[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT2A.PCX",0,"",  0,344,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,0,0,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",149, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",177, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
{BUTNO_SPELL5,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,4,-1},
{BUTNO_SPELL6,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,5,-1},
};
LONG CombatControlSize1 = sizeof(CombatControl1) / sizeof(BUTTON);

BUTTON CombatControl2[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT2A.PCX",0,"",320,344,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,0,0,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",149, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",177, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
{BUTNO_SPELL5,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,4,-1},
{BUTNO_SPELL6,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,5,-1},
};
LONG CombatControlSize2 = sizeof(CombatControl2) / sizeof(BUTTON);

BUTTON CombatControl3[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT2A.PCX",0,"",  0,404,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,0,0,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",149, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",177, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
{BUTNO_SPELL5,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,4,-1},
{BUTNO_SPELL6,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,5,-1},
};
LONG CombatControlSize3 = sizeof(CombatControl3) / sizeof(BUTTON);

BUTTON CombatControl4[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT2A.PCX",0,"",320,404,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,0,0,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",149, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",177, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
{BUTNO_SPELL5,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,4,-1},
{BUTNO_SPELL6,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,5,-1},
};
LONG CombatControlSize4 = sizeof(CombatControl4) / sizeof(BUTTON);


BUTTON CombatFighterControl1[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT1A.PCX",0,"",  0,344,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_LOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,CombatLowAttackKey,0,COMBAT_STR_ATTACK2},
{BUTNO_QLOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",143, 37, 25, 18,NO_KEY,CombatLowAttackKey,1,COMBAT_STR_QUICK_ATTACK2},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",171, 37, 25, 18,NO_KEY,0,1,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
};
LONG CombatFighterControlSize1 = sizeof(CombatFighterControl1) / sizeof(BUTTON);

BUTTON CombatFighterControl2[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT1A.PCX",0,"",320,344,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_LOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,CombatLowAttackKey,0,COMBAT_STR_ATTACK2},
{BUTNO_QLOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",143, 37, 25, 18,NO_KEY,CombatLowAttackKey,1,COMBAT_STR_QUICK_ATTACK2},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",171, 37, 25, 18,NO_KEY,0,1,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
};
LONG CombatFighterControlSize2 = sizeof(CombatFighterControl2) / sizeof(BUTTON);

BUTTON CombatFighterControl3[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT1A.PCX",0,"",  0,404,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_LOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,CombatLowAttackKey,0,COMBAT_STR_ATTACK2},
{BUTNO_QLOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",143, 37, 25, 18,NO_KEY,CombatLowAttackKey,1,COMBAT_STR_QUICK_ATTACK2},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",171, 37, 25, 18,NO_KEY,0,1,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
};
LONG CombatFighterControlSize3 = sizeof(CombatFighterControl3) / sizeof(BUTTON);

BUTTON CombatFighterControl4[] = {
{ 0,				BUTTON_BITMAP, D_NO_PUSH, 0,-1,0,"UI\\COMBAT1A.PCX",0,"",320,404,320, 60,NO_KEY,CombatMenuPaint,0,-1},
{BUTNO_AVATAR_ICON,	BUTTON_REGION,         0, 0,-1,0,"",0,"",  6,  5, 32, 50,NO_KEY,CombatSwitchActive,0,COMBAT_STR_TIP_INVENTORY},
{BUTNO_AVATAR_HEALTH,BUTTON_REGION,        0, 0,-1,0,"",0,"", 42,  7,  9, 45,NO_KEY,CombatShowStatusKey,0,COMBAT_STR_TIP_STATUS},
{BUTNO_AVATAR_STATS,BUTTON_REGION,         0, 0,-1,0,"",0,"", 59,  5,254, 14,NO_KEY,0,0,-1},
{BUTNO_AVATAR_INFO,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 20,254, 14,NO_KEY,0,0,-1},
{BUTNO_HIGH_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"", 59, 37, 25, 18,NO_KEY,CombatHighAttackKey,0,COMBAT_STR_ATTACK1},
{BUTNO_QHIGH_ATTACK,BUTTON_REGION,         0, 0,-1,0,"",0,"", 87, 37, 25, 18,NO_KEY,CombatHighAttackKey,1,COMBAT_STR_QUICK_ATTACK1},
{BUTNO_LOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",115, 37, 25, 18,NO_KEY,CombatLowAttackKey,0,COMBAT_STR_ATTACK2},
{BUTNO_QLOW_ATTACK,	BUTTON_REGION,         0, 0,-1,0,"",0,"",143, 37, 25, 18,NO_KEY,CombatLowAttackKey,1,COMBAT_STR_QUICK_ATTACK2},
{BUTNO_DEF_FALL_BTN,BUTTON_REGION,         0, 0,-1,0,"",0,"",171, 37, 25, 18,NO_KEY,0,1,-1},
{BUTNO_SPELL1,		BUTTON_REGION,         0, 0,-1,0,"",0,"",205, 37, 25, 18,NO_KEY,CombatSpellKey,0,-1},
{BUTNO_SPELL2,		BUTTON_REGION,         0, 0,-1,0,"",0,"",233, 37, 25, 18,NO_KEY,CombatSpellKey,1,-1},
{BUTNO_SPELL3,		BUTTON_REGION,         0, 0,-1,0,"",0,"",261, 37, 25, 18,NO_KEY,CombatSpellKey,2,-1},
{BUTNO_SPELL4,		BUTTON_REGION,         0, 0,-1,0,"",0,"",289, 37, 25, 18,NO_KEY,CombatSpellKey,3,-1},
};
LONG CombatFighterControlSize4 = sizeof(CombatFighterControl4) / sizeof(BUTTON);

BUTTON CombatRPGBottomBar[] = {
{ 0,					BUTTON_BITMAP, D_NO_PUSH, 0,-1,		              0,"UI\\EXTRA_16.PCX",0,"",  0,464,640,16,NO_KEY,0,0,-1},
{BUTNO_RPG_LEAVE,		BUTTON_BITMAP,		   0, 0,COMBAT_STR_LEAVE,	BTN_LABEL_COLOR,"UI\\CMBBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN1B.PCX",  0,  0,112,16,NO_KEY,CombatReturnToMap,0,-1},
{BUTNO_RPG_RETREAT,		BUTTON_BITMAP,		   0, 0,COMBAT_STR_RETREAT,	BTN_LABEL_COLOR,"UI\\CMBBTN2A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN2B.PCX",112,  0, 80,16,NO_KEY,CombatRetreatKey,0,-1},
{BUTNO_RPG_LEADER,		BUTTON_BITMAP,		   0, 0,COMBAT_STR_LEADER,	BTN_LABEL_COLOR,"UI\\CMBBTN3A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN3B.PCX",192,  0,112,16,NO_KEY,CombatNewLeaderKey,0,-1},
{BUTNO_RPG_VIEW,		BUTTON_BITMAP,		   0, 0,COMBAT_STR_VIEW,	BTN_LABEL_COLOR,"UI\\CMBBTN4A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN4B.PCX",304,  0,110,16,NO_KEY,CombatToggleViewKey,0,-1},
{BUTNO_RPG_INVENTORY,	BUTTON_BITMAP,		   0, 0,COMBAT_STR_INVENTORY,BTN_LABEL_COLOR,"UI\\CMBBTN5A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN5B.PCX",404,  0, 80,16,NO_KEY,CombatShowInventoryKey,1,-1},
{BUTNO_RPG_STATUS,		BUTTON_BITMAP,		   0, 0,COMBAT_STR_STATUS, 	BTN_LABEL_COLOR,"UI\\CMBBTN6A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN6B.PCX",484,  0, 52,16,NO_KEY,CombatShowStatusKey,1,-1},
{BUTNO_RPG_MAP,			BUTTON_BITMAP,		   0, 0,COMBAT_STR_MAP,		BTN_LABEL_COLOR,"UI\\CMBBTN7A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN7B.PCX",536,  0, 52,16,NO_KEY,CombatToggleMapKey,0,-1},
{BUTNO_RPG_DONE,		BUTTON_BITMAP,		   0, 0,-1,					  0,"UI\\CMBBTN8A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CMBBTN8B.PCX",588,  0, 52,16,NO_KEY,CombatTurnDoneKey,0,-1}
};
LONG CombatRPGBottomBarSize = sizeof(CombatRPGBottomBar)/ sizeof(BUTTON);


BUTTON CombatBlank1Bar[] = {
{ 0,BUTTON_BITMAP, D_NO_PUSH, 0, -1,0,"UI\\SCMBT_UE.PCX",0,"",320,404,320, 60,NO_KEY,0,0,-1},
};
LONG CombatBlank1BarSize = sizeof(CombatBlank1Bar)/sizeof(BUTTON);

BUTTON BtlChooseField[] = {
{ 0,BUTTON_BITMAP,D_ZOOM, 0,-1, 0,"UI\\CHOOSBTL.PCX",0,"",145, 50,351,371,NO_KEY,PaintChooseBattleField,0,-1},
{ 1,BUTTON_BITMAP,0,      0,-1, 0,"",                0,"", 10, 36,150,160,NO_KEY,ChooseBattleField, 0,-1},
{ 2,BUTTON_BITMAP,0,      0,-1, 0,"",                0,"",172, 36,150,160,NO_KEY,ChooseBattleField, 1,-1},
{ 3,BUTTON_BITMAP,0,      0,-1, 0,"",                0,"", 10,201,150,160,NO_KEY,ChooseBattleField, 2,-1},
{ 4,BUTTON_BITMAP,0,      0,-1, 0,"",                0,"",172,201,150,160,NO_KEY,ChooseBattleField, 3,-1},
{ 6,BUTTON_REGION,0,      0,-1, DKBROWN,"",          0,"",  4,  4,322, 30,NO_KEY,NULL,0,-1},
{91,BUTTON_REGION,0,      0,-1, DKBROWN,"",          0,"", 10,180,150, 20,NO_KEY,NULL,0,-1},
{92,BUTTON_REGION,0,      0,-1, DKBROWN,"",          0,"",172,180,150, 20,NO_KEY,NULL,0,-1},
{93,BUTTON_REGION,0,      0,-1, DKBROWN,"",          0,"", 10,344,150, 20,NO_KEY,NULL,0,-1},
{94,BUTTON_REGION,0,      0,-1, DKBROWN,"",          0,"",172,344,150, 20,NO_KEY,NULL,0,-1},
};
LONG BtlChooseFieldSize = sizeof(BtlChooseField) / sizeof(BUTTON);

BUTTON ShowAnimation[] = {
{0,BUTTON_COLORBEVEL,D_FREEZE|D_ZOOM, 76,-1,0,"",0,"", 40, 10,550,460,NO_KEY,PaintAnimation,0,-1},
{1,BUTTON_REGION,0, 0,-1,                   0,"",0,"", 20, 10,510,360,NO_KEY,0,0,-1},
{2,BUTTON_REGION,0, 0,-1,			        0,"",0,"",113,370, 78, 16,NO_KEY,GetPrevAnimation,0,-1},
{3,BUTTON_REGION,0, 0,-1,                   0,"",0,"",193,370, 78, 16,NO_KEY,GetNextAnimation,0,-1},
{4,BUTTON_REGION,0, 0,-1,                   0,"",0,"",113,390, 78, 16,NO_KEY,GetPrevSequence,0,-1},
{5,BUTTON_REGION,0, 0,-1,                   0,"",0,"",193,390, 78, 16,NO_KEY,GetNextSequence,0,-1},
{6,BUTTON_REGION,0, 0,-1,                   0,"",0,"",113,410, 78, 16,NO_KEY,GetPrevRotation,0,-1},
{7,BUTTON_REGION,0, 0,-1,                   0,"",0,"",193,410, 78, 16,NO_KEY,GetNextRotation,0,-1},
{8,BUTTON_REGION,0, 0,-1,                   0,"",0,"", 22,430, 88, 16,NO_KEY,RunLoop,0,-1},
{9,BUTTON_REGION,0, 0,-1,                   0,"",0,"",113,430, 78, 16,NO_KEY,StopLoop,0,-1},
{10,BUTTON_REGION,0, 0,-1,                  0,"",0,"",453,430, 78, 16,KEY_ESCAPE,CloseAnimationMenu, 0,-1},
{11,BUTTON_REGION,0, 0,-1,                  0,"",0,"",193,430, 78, 16,NO_KEY,PlayAll,0,-1},
{12,BUTTON_REGION,0, 0,-1,                  0,"",0,"",273,430, 88, 16,NO_KEY,SpeedUp,0,-1},
{13,BUTTON_REGION,0, 0,-1,                  0,"",0,"",363,430, 88, 16,NO_KEY,SlowDown,0,-1},
{14,BUTTON_REGION,0, 0,-1,                  0,"",0,"",343,370,150, 60,NO_KEY,0, 0,-1},
{15,BUTTON_REGION,0, 0,-1,                  0,"",0,"",22, 370, 88, 60,NO_KEY,0,0,-1},

};
LONG ShowAnimationSize = sizeof(ShowAnimation)/sizeof(BUTTON);


BUTTON MoreInfo[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,0,"UI\\INFODLOG.PCX",0,"",145, 50,352,175,NO_KEY,PaintMoreInfo,0,-1},
{1,BUTTON_REGION,0,               0,-1,0,"",0,"", 24, 24,305,127,NO_KEY,NULL, 0,-1},
{4,BUTTON_REGION,0,               0,-1,0,"",0,"",152,151,48,15,KEY_ESCAPE,HideSubMenuWithClick, D_MOREINFO,-1},
};
LONG MoreInfoSize = sizeof(MoreInfo) / sizeof(BUTTON);

BUTTON HoldAction[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,0,"UI\\RELMSLCT.PCX",0,"",145,210,356,70,NO_KEY,PaintHoldAction,0,-1},
{1,BUTTON_REGION,0,               0,-1,0,"",0,"", 24, 24,305,127,NO_KEY,CancelHoldAction, 0,-1},
{4,BUTTON_REGION,0,               0,-1,0,"",0,"",161,151,48,15,KEY_ESCAPE,DoneHoldAction, 0,-1},
};
LONG HoldActionSize = sizeof(HoldAction) / sizeof(BUTTON);

BUTTON HelpKeys[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,      0,"UI\\HELP_LG.PCX",0,"",160, 50,356,372,NO_KEY,PaintMoreInfo,0, -1},
{1,BUTTON_REGION,0,               0,-1,      0,"",0,"",                 24, 24,305,320,NO_KEY,0, 0,-1},
{2,BUTTON_REGION,0,     0,STR_OK, BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"UI\\HLPBTNAA.PCX",128,336,100, 28,KEY_O,DoneHelpKeys, 0,-1},
{3,BUTTON_REGION,0,     0,-1, 0,"",0,"",0,0,0,0,KEY_ESCAPE,DoneHelpKeys, 1,-1},
};
LONG HelpKeysSize = sizeof(HelpKeys) / sizeof(BUTTON);


BUTTON BattleCard[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,0,"UI\\WARCARD.PCX",0,"",160,128,320,192,NO_KEY,PaintBattleCard,0,-1},
{1,BUTTON_REGION,0,               0,-1,0,"",0,"",                 8,  8,304,176,KEY_ESCAPE,BattleCardProc, D_BTL_CARD,-1},
};
LONG BattleCardSize = sizeof(BattleCard) / sizeof(BUTTON);

BUTTON Victory[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,            0,"UI\\VCTPANL1.PCX",0,"",150,30,340,388,NO_KEY,NULL,0,-1},
{1,BUTTON_BITMAP,0,               0,STR_HELP_DONE,BTN_LABEL_COLOR,"UI\\VCTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\VCTBTNXB.PCX",                128,352,84,28,KEY_D,HideSubMenuWithClick, D_VICTORY,-1},
};
LONG VictorySize = sizeof(Victory) / sizeof(BUTTON);

BUTTON Defeat[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0,-1,            0,"UI\\DFTPANL1.PCX",0,"",150,30,340,388,NO_KEY,NULL,0,-1},
{1,BUTTON_BITMAP,0,               0,STR_HELP_DONE,BTN_LABEL_COLOR,"UI\\DFTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\DFTBTNXB.PCX",                128,352,84,28,KEY_D,HideSubMenuWithClick, D_DEFEAT,-1},
};
LONG DefeatSize = sizeof(Defeat) / sizeof(BUTTON);

BUTTON ChatDisplayWindow[] =  {
{0,BUTTON_BITMAP,D_FLOATER, 0,-1, 0,"UI\\CHATPNL.PCX",0,"", 0,0,640,64,NO_KEY,PaintChatDisplay,0,-1},
};
LONG ChatDisplayWindowSize = sizeof(ChatDisplayWindow) / sizeof(BUTTON);


BUTTON ChatDisplayWindow1[] =  {
{0,BUTTON_BITMAP,D_FLOATER, 0,-1, 0,"UI\\CHATBOX2.PCX",0,"", 0,0,640,128,NO_KEY, PaintChatDisplay, 1, -1},
};
LONG ChatDisplayWindow1Size = sizeof(ChatDisplayWindow1) / sizeof(BUTTON);

BUTTON ChatSelect[] = {                                           //80
{0,BUTTON_BITMAP,D_FREEZE, 0, -1, 0, "UI\\CHATPNL3.PCX", 0,"", 83, 130, 472, 320,NO_KEY,PaintChatSelect,0, -1},
{1,BUTTON_REGION,0, 0,-1,                0, "",0,"", 25, 100,385,15,NO_KEY,0, 0,-1},
{2,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 132,380,15,NO_KEY,0, 1,-1},
{3,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 148,380,15,NO_KEY,0, 2,-1},
{4,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 164,380,15,NO_KEY,0, 3,-1},
{5,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 180,380,15,NO_KEY,0, 4,-1},
{6,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 196,380,15,NO_KEY,0, 5,-1},
{7,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 212,380,15,NO_KEY,0, 6,-1},
{8,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 228,380,15,NO_KEY,0, 7,-1},
{9,BUTTON_REGION,0, 0,-1,                0, "",0,"", 70, 244,380,15,NO_KEY,0, 8,-1},
{10,BUTTON_REGION,0, 0,STR_CANCEL1, BTN_LABEL_COLOR,"UI\\CHTBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CHTBTNCB.PCX",252,264,84, 28,KEY_ESCAPE, CancelChatSelect, 0, -1},
{11,BUTTON_REGION,0, 0,STR_DONE1,   BTN_LABEL_COLOR,"UI\\CHTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\CHTBTNXB.PCX",336,264,84, 28,NO_KEY,DoneChatSelect, 0, -1},
};

LONG ChatSelectSize = sizeof(ChatSelect) / sizeof(BUTTON);


BUTTON MultiName[] = {
{ 0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\MLT_PNL4.PCX", 0,"", 162, 46, 316, 232,NO_KEY,PaintMultiPlayerName,0, -1},
{ 1,BUTTON_REGION,0,                    0,-1,         0, "",0,"", 61, 100,200,15,NO_KEY,NULL, 0,-1},
{ 2,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,-1,         0, "",0,"", 61, 135,200,15,NO_KEY,SetMultiPlayerProtocol, 0,-1},
{ 3,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,-1,         0, "",0,"", 61, 151,200,15,NO_KEY,SetMultiPlayerProtocol, 1,-1},
{ 4,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,-1,         0, "",0,"", 61, 167,200,15,NO_KEY,SetMultiPlayerProtocol, 2,-1},
{ 9,BUTTON_BITMAP,0, 0,STR_BACK1,  BTN_LABEL_COLOR,"UI\\MLTBTNBA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNBB.PCX", 32,196,84, 28,NO_KEY,BackMultiPlayerName, 0, -1},
{10,BUTTON_BITMAP,0, 0,STR_JOIN,   BTN_LABEL_COLOR,"UI\\MLTBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNCB.PCX",116,196,84, 28,NO_KEY,JoinMultiPlayerName, 16, -1},
{11,BUTTON_BITMAP,0, 0,STR_CREATE, BTN_LABEL_COLOR,"UI\\MLTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNXB.PCX",200,196,84, 28,NO_KEY,HostMultiPlayerName, 0, -1},
{99,BUTTON_REGION,0, 0,-1,0,"",0,"",0,0,0,0,KEY_ESCAPE,BackMultiPlayerName, 16, -1},
};
LONG MultiNameSize = sizeof(MultiName) / sizeof(BUTTON);

BUTTON MultiCreate[] = {
{ 0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\MLT_PNL4.PCX", 0,"", 162, 46, 316, 232,NO_KEY,PaintMultiPlayerCreate,0, -1},
{ 1,BUTTON_REGION,0,        0,-1,         0,              "",0,"", 61, 100,100,15,NO_KEY,NULL, 0,-1},
{ 1,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_MULTIMENU_NP,BTN_LABEL_COLOR,"",0,"", 60, 130,200,14,NO_KEY,NULL, 0,-1},
{ 2,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_2,                 BTN_LABEL_COLOR,"",0,"", 60, 151,50,15,NO_KEY,SetMultiPlayerNumber, 2,-1},
{ 3,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_3,                 BTN_LABEL_COLOR,"",0,"", 60, 167,50,15,NO_KEY,SetMultiPlayerNumber, 3,-1},
{ 4,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_4,                 BTN_LABEL_COLOR,"",0,"",110, 151,50,15,NO_KEY,SetMultiPlayerNumber, 4,-1},
{ 5,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_5,                 BTN_LABEL_COLOR,"",0,"",110, 167,50,15,NO_KEY,SetMultiPlayerNumber, 5,-1},
{ 6,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_6,                 BTN_LABEL_COLOR,"",0,"",160, 151,50,15,NO_KEY,SetMultiPlayerNumber, 6,-1},
{ 7,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_7,                 BTN_LABEL_COLOR,"",0,"",160, 167,50,15,NO_KEY,SetMultiPlayerNumber, 7,-1},
{ 8,BUTTON_REGION,D_SANS_8, 0,STR_MULTI_8,                 BTN_LABEL_COLOR,"",0,"",210, 151,50,15,NO_KEY,SetMultiPlayerNumber, 8,-1},
{ 9,BUTTON_BITMAP,0,        0,STR_BACK1,  BTN_LABEL_COLOR,"UI\\MLTBTNBA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNBB.PCX", 32,196,84, 28,NO_KEY,BackMultiPlayerCreate, 0, -1},
{10,BUTTON_BITMAP,0,        0,STR_CANCEL1,BTN_LABEL_COLOR,"UI\\MLTBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNCB.PCX",116,196,84, 28,NO_KEY,CancelMultiPlayerCreate, 16, -1},
{11,BUTTON_BITMAP,0,        0,STR_HOST,   BTN_LABEL_COLOR,"UI\\MLTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNXB.PCX",200,196,84, 28,NO_KEY,HostMultiPlayerCreate, 0, -1},
{99,BUTTON_REGION,0,        0,-1,0,"",0,"",0,0,0,0,KEY_ESCAPE,BackMultiPlayerCreate, 16, -1},
};
LONG MultiCreateSize = sizeof(MultiCreate) / sizeof(BUTTON);

BUTTON MultiJoin[] = {
{ 0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\MLT_PNL4.PCX", 0,"", 162, 46, 316, 232,NO_KEY,PaintMultiPlayerJoin,0, -1},
{ 1,BUTTON_REGION,0,0,-1,0,"",0,"", 61, 100,200,15,NO_KEY,NULL, 0,-1},
{ 2,BUTTON_REGION,0,0,-1,0,"",0,"", 61, 130,200,15,KEY_ESCAPE,SetMultiPlayerJoin, 0, -1},
{ 3,BUTTON_REGION,0,0,-1,0,"",0,"", 61, 145,200,15,KEY_ESCAPE,SetMultiPlayerJoin, 1, -1},
{ 4,BUTTON_REGION,0,0,-1,0,"",0,"", 61, 160,200,15,KEY_ESCAPE,SetMultiPlayerJoin, 2, -1},
{ 5,BUTTON_REGION,0,0,-1,0,"",0,"", 61, 175,200,15,KEY_ESCAPE,SetMultiPlayerJoin, 3, -1},
{ 9,BUTTON_BITMAP,0,0,STR_BACK1,        BTN_LABEL_COLOR,"UI\\MLTBTNBA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNBB.PCX", 32,196,84, 28,NO_KEY,BackMultiPlayerJoin, 0, -1},
{10,BUTTON_BITMAP,0,0,STR_CANCEL1,      BTN_LABEL_COLOR,"UI\\MLTBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNCB.PCX",116,196,84, 28,NO_KEY,CancelMultiPlayerJoin, 16, -1},
{11,BUTTON_BITMAP,0,0,STR_DONE1,        BTN_LABEL_COLOR,"UI\\MLTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNXB.PCX",200,196,84, 28,NO_KEY,DoneMultiPlayerJoin, 0, -1},
{99,BUTTON_REGION,0,0,-1,0,"",0,"",0,0,0,0,KEY_ESCAPE,CancelMultiPlayerJoin, 16, -1},
};
LONG MultiJoinSize = sizeof(MultiJoin) / sizeof(BUTTON);

BUTTON MultiEnvironment[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\MLT_ENV.PCX", 0,"", 162, 46, 316, 200,NO_KEY,PaintMultiPlayerEnvironment,0, -1},
{1,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,  0,STR_MULTIMENU_INT,BTN_LABEL_COLOR, "",0,"", 63,103,200,15,NO_KEY,SetMultiPlayerEnvironment, 0,-1},
{2,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,  0,STR_MULTIMENU_LA, BTN_LABEL_COLOR, "",0,"", 63,120,200,15,NO_KEY,SetMultiPlayerEnvironment, 1,-1},
{3,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,  0,STR_MULTIMENU_WP, BTN_LABEL_COLOR, "",0,"", 63,137,200,15,NO_KEY,SetMultiPlayerEnvironment, 2,-1},
{10,BUTTON_BITMAP,0, 0,STR_CANCEL,       BTN_LABEL_COLOR,"UI\\MLTBTNCA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNCB.PCX", 73,164,84, 28,KEY_C,CancelMultiPlayerEnvironment, 0, -1},
{11,BUTTON_BITMAP,0, 0,STR_DONE,         BTN_LABEL_COLOR,"UI\\MLTBTNXA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\MLTBTNXB.PCX",157,164,84, 28,KEY_D,DoneMultiPlayerEnvironment, 0, -1},
{12,BUTTON_REGION,0, 0,-1,0,"",0,"",0,0,0,0,KEY_ESCAPE,CancelMultiPlayerEnvironment, 0, -1},
};
LONG MultiEnvironmentSize = sizeof(MultiEnvironment) / sizeof(BUTTON);

BUTTON MultiHost[] = {
{ 0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\HST_PNL1.PCX", 0,"", 208, 100, 224, 264,NO_KEY,PaintMultiPlayerHost,0, -1},
{91,BUTTON_REGION,D_SANS_8,             0,STR_MULTITYPE_SEL, 200,            "",0,"", 20, 70, 82,14,NO_KEY,NULL, 1,-1}, // TITLE
#ifdef _WINDOWS
{ 1,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_MULTITYPE_NEW, BTN_LABEL_COLOR,"",0,"", 25, 93, 74,14,NO_KEY,SetMultiPlayerHostGame, iMPGT_NEW,-1}, // New
{ 2,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_MULTITYPE_LOAD,BTN_LABEL_COLOR,"",0,"", 25,107, 74,14,NO_KEY,SetMultiPlayerHostGame, iMPGT_LOAD,-1}, // Load
#endif
{92,BUTTON_REGION,D_SANS_8,             0,STR_TURNTIME,      200,            "",0,"", 20,130, 82,14,NO_KEY,NULL, 1,-1}, // TITLE
{ 3,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_TURNTIME_1,    BTN_LABEL_COLOR,"",0,"", 25,154, 74,14,NO_KEY,SetMultiPlayerHostTime, iTT_1Min,-1}, // Fast
{ 4,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_TURNTIME_2,    BTN_LABEL_COLOR,"",0,"", 25,168, 74,14,NO_KEY,SetMultiPlayerHostTime, iTT_2Min,-1}, // Med
{ 5,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_TURNTIME_3,    BTN_LABEL_COLOR,"",0,"", 25,182, 74,14,NO_KEY,SetMultiPlayerHostTime, iTT_5Min,-1}, // Slow
{ 6,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_TURNTIME_4,    BTN_LABEL_COLOR,"",0,"", 25,196, 74,14,NO_KEY,SetMultiPlayerHostTime, iTT_None,-1}, // None
{93,BUTTON_REGION,D_SANS_8,             0,STR_OPT_GAME_CTRL, 200,            "",0,"",120, 70, 82,14,NO_KEY,NULL, 1,-1}, // TITLE
{ 7,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_OPT_CTRL_MODE, BTN_LABEL_COLOR,"",0,"",125, 93, 74,14,NO_KEY,SetMultiPlayerHostDetail, 0,-1}, // Basic
{ 8,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_OPT_CTRL_MODE1,BTN_LABEL_COLOR,"",0,"",125,107, 74,14,NO_KEY,SetMultiPlayerHostDetail, 1,-1}, // Advanced
{ 9,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_OPT_CTRL_MODE2,BTN_LABEL_COLOR,"",0,"",125,121, 74,14,NO_KEY,SetMultiPlayerHostDetail, 2,-1}, // Expert
{94,BUTTON_REGION,D_SANS_8,             0,STR_OPT_ADV_DIF,   200,            "",0,"",120,145, 82,14,NO_KEY,NULL, 1,-1}, // TITLE
{10,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_OPT_DIF_EASY,  BTN_LABEL_COLOR,"",0,"",125,168, 74,14,NO_KEY,SetMultiPlayerHostDiff, 0,-1}, // Easy
{11,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_OPT_DIF_MEDIUM,BTN_LABEL_COLOR,"",0,"",125,182, 74,14,NO_KEY,SetMultiPlayerHostDiff, 1,-1}, // Medium
{12,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8, 0,STR_OPT_DIF_HARD,  BTN_LABEL_COLOR,"",0,"",125,196, 74,14,NO_KEY,SetMultiPlayerHostDiff, 2,-1}, // Hard
//{13,BUTTON_BITMAP,D_SANS_8, 0,STR_CANCEL,        BTN_LABEL_COLOR,"UI\\HSTBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HSTBTN1B.PCX", 28,226, 84, 28,KEY_C,CancelMultiPlayerHost, 0, -1},
{14,BUTTON_BITMAP,D_SANS_8, 0,STR_DONE,          BTN_LABEL_COLOR,"UI\\HSTBTN2A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\HSTBTN2B.PCX",70,226, 84, 28,KEY_D,DoneMultiPlayerHost, 0, -1},
{15,BUTTON_REGION,0, 0,-1,0,"",0,"", 0,0,0,0,KEY_ESCAPE,CancelMultiPlayerHost, 0, -1},
};
LONG MultiHostSize = sizeof(MultiHost) / sizeof(BUTTON);


BUTTON LoyaltyMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE, 0, -1, 0, "UI\\REPORT2.PCX", 0,"", 102, 6, 436, 468,NO_KEY,PaintLoyaltyTable,0, -1},
{1,BUTTON_REGION,0, 0,-1,                0, "",0,"", 0, 0,436,468,NO_KEY,DoneReportMenu, 0,-1},
{2,BUTTON_REGION,0, 0,-1,                0, "",0,"", 0, 0,0,0,KEY_ESCAPE,DoneReportMenu1, 1,-1},
};

LONG LoyaltyMenuSize = sizeof(LoyaltyMenu) / sizeof(BUTTON);

BUTTON DiplomacyAction[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,0,"UI\\HELP_LG.PCX",0,"",160, 50,356,372,NO_KEY,PaintOthersDiplomacy,0, -1},
{1,BUTTON_REGION,0,              0,-1,         0,"",0,"", 24, 24,305,320,NO_KEY,0, 0, -1},
{2,BUTTON_REGION,D_HILIGHTED,0,-1, 0,"",0,"UI\\HLPBTNAA.PCX",  28,340, 100, 28,KEY_G,NeverOthersDiplomacy, 0, -1},
{3,BUTTON_REGION,D_HILIGHTED,0,-1,   0,"",0,"UI\\HLPBTNAA.PCX", 128,340, 100, 28,KEY_A,YesOthersDiplomacy, 0, -1},
{4,BUTTON_REGION,D_HILIGHTED,0,-1,    0,"",0,"UI\\HLPBTNAA.PCX", 228,340, 100, 28,KEY_I,NoOthersDiplomacy, 0, -1},
{5,BUTTON_REGION,0,    0,-1,   0,"",0,"",0,0,0,0,KEY_ESCAPE,YesOthersDiplomacy, 0, -1},
};

LONG DiplomacyActionSize = sizeof(DiplomacyAction) / sizeof(BUTTON);

BUTTON BattleMagicPrep[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,          0,"UI\\BTLSEL.PCX",0,"",   250, 80,140,280,NO_KEY,PaintBattleMagicPrep,0, -1},
{1,BUTTON_BITMAP,0,    0,STR_DONE,  BTN_LABEL_COLOR,"UI\\SELBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\SELBTN1B.PCX",  28,244, 84, 28,KEY_D,BattlePrepProc, 0, -1},
{2,BUTTON_REGION,0,    0,-1,          0,"",0,"",0,0,0,0,KEY_ESCAPE,BattlePrepProc, 0, -1},
};
LONG BattleMagicPrepSize = sizeof(BattleMagicPrep) / sizeof(BUTTON);

BUTTON BattleCommand[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0,-1,0,"UI\\HELP_LG.PCX",0,"",160, 50,356,372,NO_KEY,SetCommandBattlePaint, 0, -1},
{1,BUTTON_REGION,  0,0,-1,                 0,              "",                0,                     "", 25,100,290,240,NO_KEY,0, 0, -1},
{333,BUTTON_BITMAP,0,0,STR_YES,            BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"", 28,336,100, 28,KEY_Y,SetCommandBattleProc, QUESTION_BTN1, -1},
{334,BUTTON_BITMAP,0,0,COMBAT_STR_RETREAT, BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"",128,336,100, 28,KEY_R,SetCommandBattleProc, QUESTION_BTN2, -1},
{335,BUTTON_BITMAP,0,0,STR_NO,             BTN_LABEL_COLOR,"UI\\HLPBTNAA.PCX",BTN_LABEL_HILITE_COLOR,"",228,336,100, 28,KEY_N,SetCommandBattleProc, QUESTION_BTN3, -1},
};
LONG BattleCommandSize = sizeof(BattleCommand) / sizeof(BUTTON);

BUTTON BuildArmys[] = {
{0,BUTTON_BITMAP,D_FREEZE,  0,-1, 0,"UI\\BATLPICK.PCX",0,"",0,0,640,480,NO_KEY,BuildArmyPaint,0,-1},
{9,BUTTON_REGION,D_TEXT_LEFT,0,STR_BB_TITLE,31,"",0,"", 8, 6, 150,55,NO_KEY,NULL, 0, -1},
// my button and list box with controls
{9,           BUTTON_REGION,0,0,STR_BB_MYLIST,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"", 18, 68,130,15,NO_KEY,NULL, 0, -1},
{BB_MYARMY1,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18, 87,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY2,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,103,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY3,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,119,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY4,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,135,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY5,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,151,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY6,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,167,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY7,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,183,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY8,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,199,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY9,  BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,215,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY10, BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,231,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY11, BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,247,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY12, BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,263,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY13, BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,279,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY14, BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,295,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MYARMY15, BUTTON_REGION,D_TEXT_LEFT,0,-1,             YELLOW,"",WHITE,"", 18,311,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_MY_ADD,   BUTTON_BITMAP,0,0,STR_BB_ADD,   BTN_LABEL_COLOR,"UI\\BTPBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN1B.PCX", 12,332,144,28,NO_KEY,BuildListAdd, 0, -1},
{BB_MY_REMOVE,BUTTON_BITMAP,0,0,STR_BB_REM,   BTN_LABEL_COLOR,"UI\\BTPBTN2A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN2B.PCX", 12,360,144,28,NO_KEY,BuildListRemove,0, -1},
// enemy button and listbox with controls
{9,            BUTTON_REGION,0,0,STR_BB_HISLIST,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"",490, 68,130,15,NO_KEY,NULL, 0, -1},
{BB_HISARMY1,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490, 87,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY2,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,103,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY3,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,119,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY4,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,135,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY5,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,151,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY6,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,167,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY7,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,183,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY8,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,199,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY9,  BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,215,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY10, BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,231,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY11, BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,247,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY12, BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,263,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY13, BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,279,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY14, BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,295,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HISARMY15, BUTTON_REGION,D_TEXT_LEFT,0,-1,              YELLOW,"",WHITE,"",490,311,130,15,NO_KEY,BuildListSel, 0, -1},
{BB_HIS_ADD,   BUTTON_BITMAP,0,0,STR_BB_ADD,    BTN_LABEL_COLOR,"UI\\BTPBTN5A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN5B.PCX",484,332,144,28,NO_KEY,BuildListAdd, 0, -1},
{BB_HIS_REMOVE,BUTTON_BITMAP,0,0,STR_BB_REM,    BTN_LABEL_COLOR,"UI\\BTPBTN6A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN6B.PCX",484,360,144,28,NO_KEY,BuildListRemove,0, -1},

{BB_DISPLAY,BUTTON_REGION,0,0,STR_BB_TROOPS,    BTN_LABEL_COLOR,"UI\\BTPBTNZA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTNZB.PCX",248,416,144,28,NO_KEY,BuildDisplayToggle,0, -1},

{BB_PLAINS,    BUTTON_BITMAP,0,0,STR_BB_PLAINS,    BTN_LABEL_COLOR,"UI\\BTPBTNPA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTNPB.PCX",160,448,80, 24,NO_KEY,BuildTerrainSel, 0, -1},
{BB_MOUNTAINS, BUTTON_BITMAP,0,0,STR_BB_MOUNTAINS, BTN_LABEL_COLOR,"UI\\BTPBTNMA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTNMB.PCX",240,448,80, 24,NO_KEY,BuildTerrainSel, 1, -1},
{BB_FOREST,    BUTTON_BITMAP,0,0,STR_BB_FOREST,    BTN_LABEL_COLOR,"UI\\BTPBTNFA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTNFB.PCX",320,448,80, 24,NO_KEY,BuildTerrainSel, 2, -1},
{BB_SWAMP,     BUTTON_BITMAP,0,0,STR_BB_SWAMP,     BTN_LABEL_COLOR,"UI\\BTPBTNSA.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTNSB.PCX",400,448,80, 24,NO_KEY,BuildTerrainSel, 3, -1},

{BB_RESTART,   BUTTON_BITMAP,0,0,STR_BB_RESTART,   BTN_LABEL_COLOR,"UI\\BTPBTN3A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN3B.PCX", 12,416,144, 28,NO_KEY,BuildArmyRestart,0, -1},
{BB_RELOAD,    BUTTON_BITMAP,0,0,STR_BB_RELOAD,    BTN_LABEL_COLOR,"UI\\BTPBTN4A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN4B.PCX", 12,444,144, 28,NO_KEY,BuildArmyReload, 0, -1},
{BB_PRACTICE,  BUTTON_BITMAP,0,0,STR_BB_PRACTICE,  BTN_LABEL_COLOR,"UI\\BTPBTN7A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTPBTN7B.PCX",484,416,144, 28,NO_KEY,BuildArmyDone,   0, -1},
{BB_FIGHT,     BUTTON_BITMAP,0,0,STR_BB_FIGHT,     BTN_LABEL_COLOR,"UI\\BTPBTN8A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\BTNBTN8B.PCX",484,444,144, 28,NO_KEY,BuildArmyDone,   0, -1},
};
LONG BuildArmysSize = sizeof(BuildArmys) / sizeof(BUTTON);

BUTTON BuildAdv[] = {
{0,BUTTON_BITMAP,D_FREEZE,   0,-1,           0,"UI\\BUILDADV.PCX",0,"",0,0,640,480,NO_KEY,BuildAdvPaint,0,-1},
{9,BUTTON_REGION,0,                0,STR_BA_TITLE,   31,"",0,"", 15,  2,170,20,NO_KEY,NULL, 0, -1},
// adventure list box 
{9,BUTTON_REGION,D_TEXT_LEFT,    0,STR_CS_SELECT,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"", 16, 55,176,25,NO_KEY,NULL, 0, -1},
{BA_LIST_01,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16, 95,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_02,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,110,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_03,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,125,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_04,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,140,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_05,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,155,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_06,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,170,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_07,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,185,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_08,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,200,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{9,BUTTON_REGION,D_TEXT_LEFT,    0,STR_CS_CHOOSE,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"", 16,230,176,25,NO_KEY,NULL, 0, -1},
{9,BUTTON_REGION,D_TEXT_LEFT,    0,STR_BA_CHOOSE,BTN_LABEL_COLOR,"",BTN_LABEL_HILITE_COLOR,"", 16,257,176, 8,NO_KEY,NULL, 0, -1},
{BA_LIST_09,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,270,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_10,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,285,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_11,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,300,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_12,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,315,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_13,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,330,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_14,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,345,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_15,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,360,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_LIST_16,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,375,174,15,NO_KEY,BuildAdvListSel, 0, -1},
{BA_RESET,      BUTTON_BITMAP,0,0,STR_BA_RESET,  BTN_LABEL_COLOR,"UI\\ADBTN1A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\ADBTN1B.PCX", 32,412,144,28,NO_KEY,BuildAdvRestart, 0, -1},
{BA_DONE,             BUTTON_BITMAP,0,0,STR_BA_DONE,   BTN_LABEL_COLOR,"UI\\ADBTN2A.PCX",BTN_LABEL_HILITE_COLOR,"UI\\ADBTN2B.PCX", 32,444,144,28,NO_KEY,BuildAdvDone, 0, -1},
};
LONG BuildAdvSize = sizeof(BuildAdv) / sizeof(BUTTON);

BUTTON AdvPrep[] = {
{0,BUTTON_BITMAP,D_FREEZE,   0,-1,           0,"UI\\ADVPREP.PCX",0,"",0,0,640,480,NO_KEY,AdvPrepPaint,0,-1},
{9,BUTTON_REGION,0,                0,STR_BA_TITLE,   31,"",0,"", 15,  2,170,20,NO_KEY,NULL, 0, -1},
// adventure list box 
{9,BUTTON_REGION,D_TEXT_LEFT,    0,STR_CS_SELECT,173,"",0,"", 16, 55,176,25,NO_KEY,NULL, 0, -1},
{BA_LIST_01,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16, 95,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_02,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,110,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_03,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,125,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_04,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,140,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_05,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,155,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_06,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,170,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_07,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,185,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_08,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,200,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{9,BUTTON_REGION,D_TEXT_LEFT,    0,STR_CS_CHOOSE,173,"",0,"", 16,230,176,25,NO_KEY,NULL, 0, -1},
{9,BUTTON_REGION,D_TEXT_LEFT,    0,STR_BA_CHOOSE,173,"",0,"", 16,257,176, 8,NO_KEY,NULL, 0, -1},
{BA_LIST_09,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,270,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_10,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,285,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_11,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,300,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_12,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,315,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_13,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,330,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_14,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,345,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_15,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,360,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_LIST_16,    BUTTON_REGION,0,69,-1,             0,"",0,"", 16,375,174,15,NO_KEY,AdvPrepListSel, 0, -1},
{BA_RESET,      BUTTON_BITMAP,0,0,STR_CANCEL,    BTN_LABEL_COLOR,"UI\\ADBTN1A.PCX", 31,"UI\\ADBTN1B.PCX",  32,412,144,28,KEY_C,AdvPrepCancel, 0, -1},
{BA_DONE,       BUTTON_BITMAP,0,0,STR_BA_DONE,   BTN_LABEL_COLOR,"UI\\ADBTN2A.PCX", 31,"UI\\ADBTN2B.PCX",  32,444,144,28,NO_KEY,AdvPrepDone, 0, -1},
{ADV_LIST1_UP,  BUTTON_BITMAP,0,0,-1,            0,"UI\\ADVTNU1A.PCX",0,"UI\\ADBTNU1B.PCX",192, 76, 24,60,NO_KEY,AdvPrepListScroll, 1, -1},
{ADV_LIST1_DOWN,BUTTON_BITMAP,0,0,-1,            0,"UI\\ADVTND1A.PCX",0,"UI\\ADBTND1B.PCX",192,136, 24,60,NO_KEY,AdvPrepListScroll, 2, -1},
{ADV_LIST2_UP,  BUTTON_BITMAP,0,0,-1,            0,"UI\\ADBTNU1A.PCX",0,"UI\\ADBTNU2B.PCX",192,256, 24,60,NO_KEY,AdvPrepListScroll, 3, -1},
{ADV_LIST2_DOWN,BUTTON_BITMAP,0,0,-1,            0,"UI\\ADBTND1A.PCX",0,"UI\\ADBTND2B.PCX",192,316, 24,60,NO_KEY,AdvPrepListScroll, 4, -1},
};
LONG AdvPrepSize = sizeof(AdvPrep) / sizeof(BUTTON);

BUTTON GameSelMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE,0,-1,             		  0,"UI\\HELPPNL1.PCX",0,"",200, 75,351,371,NO_KEY,PaintGameSelect,0, -1},
{1,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_GAMETYPE,     31,"",0,"", 48, 80,152, 20,NO_KEY,NULL, 0, -1},
{2,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_HELP,        173,"",31,"", 48,114,152, 20,KEY_Q,SelectGameType, 0, -1},
{3,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_INTROGAME,   173,"",31,"", 48,134,152, 20,KEY_T,SelectGameType, 1, -1},
{4,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_FULLGAME,    173,"",31,"", 48,154,152, 20,KEY_P,SelectGameType, 2, -1},
{5,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_ADVENTURE,   173,"",31,"", 48,174,152, 20,KEY_V,SelectGameType, 3, -1},
{6,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_BATTLE,      173,"",31,"", 48,194,152, 20,KEY_B,SelectGameType, 4, -1},
{7,BUTTON_REGION,D_TEXT_LEFT,  0,STR_SELECT_BACKGROUND,  173,"",31,"", 48,214,152, 20,KEY_H,SelectGameType, 5, -1},
{8,BUTTON_REGION,0,       	   0,-1,             		   0,"",0,"", 44,230,152, 20,NO_KEY,NULL, 0, -1},
{9,BUTTON_BITMAP,0,			   0,STR_CANCEL,			 BTN_LABEL_COLOR,"",31,"UI\\HLPBTNAB.PCX", 76,252, 84, 28,KEY_C ,HideSubMenuWithClick, D_SELECT_GAME, -1},
{10,BUTTON_REGION,0,      0,-1,             		  0,"",0,"",  0,  0,  0,  0,KEY_ESCAPE,HideSubMenu, D_SELECT_GAME, -1},
};
LONG GameSelMenuSize = sizeof(GameSelMenu) / sizeof(BUTTON);

BUTTON GameCmplxMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE,        0,-1,             0,"UI\\DIALPNL1.PCX",0,"", 202,136,236,208,NO_KEY,PaintGameComplexity,0, -1},
{1,BUTTON_REGION,D_TEXT_LEFT|D_SANS_8,0,STR_SELECT_COMPLEXITY, 31,"",0,"", 48, 80,152, 20,NO_KEY,NULL, 0, -1},
{2,BUTTON_REGION,0,               0,-1,             173,"",           31,"", 44,109,152, 20,KEY_B,SelectGameType, 10, -1},
{3,BUTTON_REGION,0,               0,-1,             173,"",           31,"", 44,129,152, 20,KEY_A,SelectGameType, 11, -1},
{4,BUTTON_REGION,0,               0,-1,             173,"",           31,"", 44,149,152, 20,KEY_E,SelectGameType, 12, -1},
{9,BUTTON_BITMAP,0,     		  0,STR_CANCEL,BTN_LABEL_COLOR,  "UI\\HLPBTNCA.PCX",31,"UI\\HLPBTNCB.PCX", 76,172, 84, 28,KEY_C ,HideSubMenuWithClick, D_GAME_COMPLEXITY, -1},
{10,BUTTON_REGION,0,              0,-1,              0,"",0,"",  0,  0,  0,  0,KEY_ESCAPE,HideSubMenu, D_GAME_COMPLEXITY, -1},
};
LONG GameCmplxMenuSize = sizeof(GameCmplxMenu) / sizeof(BUTTON);



BUTTON MultiWait[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM, 0, -1, 0, "UI\\CHATPNL2.PCX",0,"", 83, 80, 472, 320,NO_KEY, 0, 0, -1},
{1,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 100,440,15,NO_KEY,0, 0,-1},
{2,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 116,440,15,NO_KEY,0, 1,-1},
{3,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 132,440,15,NO_KEY,0, 2,-1},
{4,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 148,440,15,NO_KEY,0, 3,-1},
{5,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 164,440,15,NO_KEY,0, 4,-1},
{6,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 180,440,15,NO_KEY,0, 5,-1},
{7,BUTTON_REGION,0, 77,-1,                0, "",0,"", 30, 196,440,15,NO_KEY,0, 6,-1},
//{8,BUTTON_BITMAP,0, 0,STR_CANCEL1, BTN_LABEL_COLOR,"UI\\CHTBTN2A.PCX",31,"UI\\CHTBTN2B.PCX", 196, 220, 84, 28,KEY_ESCAPE,HideSubMenuWithClick, D_MULTIWAIT, -1},
};
LONG MultiWaitSize = sizeof(MultiWait) / sizeof(BUTTON);

BUTTON DescribeMenu[] = {
{0,BUTTON_BITMAP,D_FREEZE,        0,-1,             0,"",0,                "",160,120,320,264, NO_KEY,    PaintDescribe, 0, -1},
{1,BUTTON_REGION,D_HILIGHTED,     0,-1,             0,"",0,"UI\\MARBBTNA.PCX",134,240, 52, 16, KEY_SPACE, IncDescribe,   0, -1},
};
LONG DescribeMenuSize = sizeof(DescribeMenu) / sizeof(BUTTON);

BUTTON SystemLargeHelp[] = {
{SYSHELP_DIALOG,  BUTTON_BITMAP,D_DONT_SHADE|D_FREEZE|D_ZOOM, 0,-1,   0,"UI\\help_lg.PCX",  0,"",145, 50,356,372,NO_KEY,SystemHelpPaintProc,0, -1},
{SYSHELP_TITLE,   BUTTON_REGION,0,               0,-1,                0,"",                 0,"", 15, 10,326, 20,NO_KEY,NULL,0, -1},
{SYSHELP_TEXTBODY,BUTTON_REGION,0,               0,-1,                0,"",                 0,"", 15, 35,326,300,NO_KEY,NULL,0, -1},
{SYSHELP_DONE,    BUTTON_BITMAP,0,               0,STR_HELP_DONE,    BTN_LABEL_COLOR,"UI\\hlpbtnaa.pcx",31,"UI\\hlpbtnab.pcx", 40,339,100, 28,KEY_D ,HideSubMenuWithClick, D_SYSHELP_LARGE, -1},
{SYSHELP_MOREINFO,BUTTON_BITMAP,0,               0,STR_HELP_MOREINFO,BTN_LABEL_COLOR,"UI\\hlpbtnaa.pcx",31,"UI\\hlpbtnab.pcx",224,339,100, 28,KEY_M ,NULL,0, -1},
};
LONG SystemLargeHelpSize = sizeof(SystemLargeHelp) / sizeof(BUTTON);

BUTTON SystemMedHelp[] = {
{SYSHELP_DIALOG,  BUTTON_BITMAP,D_DONT_SHADE|D_FREEZE|D_ZOOM, 0,-1,   0,"UI\\help_md.PCX", 0,"",145,150,356,188,NO_KEY,SystemHelpPaintProc,0, -1},
{SYSHELP_TITLE,   BUTTON_REGION,0,               0,-1,                0,"",                0,"", 15, 10,326, 20,NO_KEY,NULL,0, -1},
{SYSHELP_TEXTBODY,BUTTON_REGION,0,               0,-1,                0,"",                0,"", 15, 35,326,115,NO_KEY,NULL,0, -1},
{SYSHELP_DONE,    BUTTON_BITMAP,0,               0,STR_HELP_DONE,    BTN_LABEL_COLOR,"UI\\hlpbtnaa.pcx",31,"UI\\hlpbtnab.pcx", 40,155,100, 28,KEY_D ,HideSubMenuWithClick, D_SYSHELP_MED, -1},
{SYSHELP_MOREINFO,BUTTON_BITMAP,0,               0,STR_HELP_MOREINFO,BTN_LABEL_COLOR,"UI\\hlpbtnaa.pcx",31,"UI\\hlpbtnab.pcx",224,155,100, 28,KEY_M ,NULL,0, -1},
};
LONG SystemMedHelpSize = sizeof(SystemMedHelp) / sizeof(BUTTON);

BUTTON SystemSmallHelp[] = {
{SYSHELP_DIALOG,  BUTTON_BITMAP,D_DONT_SHADE|D_FREEZE|D_ZOOM, 0,-1,   0,"UI\\help_sm.PCX",  0,"",145,250,356,112,NO_KEY,SystemHelpPaintProc,0, -1},
{SYSHELP_TITLE,   BUTTON_REGION,0,               0,-1,                0,"",                 0,"", 15,  8,326, 20,NO_KEY,NULL,0, -1},
{SYSHELP_TEXTBODY,BUTTON_REGION,0,               0,-1,                0,"",                 0,"", 15, 35,326, 48,NO_KEY,NULL,0, -1},
{SYSHELP_DONE,    BUTTON_BITMAP,0,               0,STR_HELP_DONE,    BTN_LABEL_COLOR,"UI\\hlpbtnaa.pcx",31,"UI\\hlpbtnab.pcx", 40, 79,100, 28,KEY_D ,HideSubMenuWithClick, D_SYSHELP_SMALL, -1},
{SYSHELP_MOREINFO,BUTTON_BITMAP,0,               0,STR_HELP_MOREINFO,BTN_LABEL_COLOR,"UI\\hlpbtnaa.pcx",31,"UI\\hlpbtnab.pcx",224, 79,100, 28,KEY_M ,NULL,0, -1},
};
LONG SystemSmallHelpSize = sizeof(SystemSmallHelp) / sizeof(BUTTON);

BUTTON TutorialMenuWide[] = {
{TUTORIAL_MENU_BACKGROUND,  BUTTON_BITMAP,D_FREEZE, 0,-1,           0,"UI\\MARBLE06.PCX", 0,"",100, 50,321,263,NO_KEY,PaintTutorial,0, -1},
{TUTORIAL_MENU_HEAD,   BUTTON_BITMAP,0,             0,-1,           0,"UI\\HEADCNSL.PCX", 0,"",-24,-50, 64, 96,NO_KEY,NULL,0, -1},
{TUTORIAL_MENU_TEXT,   BUTTON_REGION,0,             0,-1,           0,                "", 0,"", 10, 46,302,170,NO_KEY,NULL,0, -1},
{TUTORIAL_MENU_DONE,   BUTTON_BITMAP,0,             0,STR_HELP_DONE,0,"UI\\MARBBTNA.PCX",31,"UI\\MARBBTNB.PCX",135,231, 51, 15,KEY_D,TutorialDone, 0, -1},
};

LONG TutorialMenuWideSize = sizeof(TutorialMenuWide) / sizeof(BUTTON);

BUTTON TutorialMenuMed[] = {
{TUTORIAL_MENU_BACKGROUND,  BUTTON_BITMAP,D_FREEZE, 0,-1,           0,"UI\\MARBLE03.PCX",  0,"",100, 50,199,161,NO_KEY,PaintTutorial,0, -1},
{TUTORIAL_MENU_HEAD,   BUTTON_BITMAP,0,             0,-1,           0,"UI\\HEADCNSL.PCX",  0,"",-24,-50, 64, 96,NO_KEY,NULL,0, -1},
{TUTORIAL_MENU_TEXT,   BUTTON_REGION,0,             0,-1,           0,                "",  0,"", 10, 46,179, 79,NO_KEY,NULL,0, -1},
{TUTORIAL_MENU_DONE,   BUTTON_BITMAP,0,             0,STR_HELP_DONE,0,"UI\\MARBBTNA.PCX", 31,"UI\\MARBBTNB.PCX", 75,140, 51, 15,KEY_D,TutorialDone, 0, -1},
};

LONG TutorialMenuWideMed = sizeof(TutorialMenuMed) / sizeof(BUTTON);

BUTTON LoadingScene[] = {
{0,BUTTON_BITMAP,D_NO_PUSH,0,-1,0,"UI\\help_sm.PCX",0,"",145,250,356,112,NO_KEY,LoadingSceneProc,0, -1},
};
LONG LoadingSceneSize = sizeof(LoadingScene) / sizeof(BUTTON);

BUTTON Parchment[] = {
{0,BUTTON_BITMAP,D_FREEZE|D_ZOOM,0, -1,         0,"UI\\HNTPANL1.PCX",0,"",   154,60,352,370,NO_KEY,NULL,0, -1},
{PARCHMENT_TEXT,  BUTTON_REGION,0,0,-1,         1,"",0,"",38,118,216,116,NO_KEY,NULL,0, -1},
{PARCHMENT_CLICK, BUTTON_REGION,0,0,STR_CLICK,204,"",0,"", 0,330,352, 28,NO_KEY,NULL,0, -1},
{PARCHMENT_OK_BTN,BUTTON_REGION,0,0,-1,         0,"",0,"", 0,  0,352,370,KEY_ENTER,ParchmentDone,0, -1},
};
LONG ParchmentSize = sizeof(Parchment) / sizeof(BUTTON);

#endif // frenchver
#endif // english ver

#endif
