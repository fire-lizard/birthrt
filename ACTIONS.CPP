/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: DomText.cpp  -
   Author:   Alan Clark

   Contains the following general functions:

   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "SYSTEM.H"
#include "ACTIONS.HXX"
#include "strenum.h"
#include "CONTEXT.HXX"
/* -----------------------------------------------------------------------
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
// Adventure
//---------------------------------------------

DEFINE_VECTOR_DATA(ACTIONS const,Action) = {
ACTIONS(STR_NULL,0,-1),
ACTIONS(STR_ACTION_ADVENTURE			,ACTION_TEXT0 , H_Adventure_Button),
ACTIONS(STR_ACTION_AGITATE				,ACTION_TEXT1 , H_Agitate_Button),
ACTIONS(STR_ACTION_BUILD_RD				,ACTION_TEXT16, H_Build_Road_Button),
ACTIONS(STR_ACTION_CONTEST				,ACTION_TEXT2 , H_Contest_Button),
ACTIONS(STR_ACTION_CREATE_HOLDING	   	,ACTION_TEXT3 , H_Create_Holding_Button),
ACTIONS(STR_ACTION_DECLARE_WAR			,ACTION_TEXT4 , H_Declare_War_Button ),
ACTIONS(STR_ACTION_DIPLOMACY			,ACTION_TEXT5 , H_Diplomacy_Button),
ACTIONS(STR_ACTION_ESPIONAGE			,ACTION_TEXT6 , H_Espionage_Button ),
ACTIONS(STR_ACTION_FORGE_LEY_LINE		,ACTION_TEXT7 , H_Forge_Ley_Line_Button ),
ACTIONS(STR_ACTION_FORTIFY				,ACTION_TEXT8 , H_Fortify_Button),
ACTIONS(STR_ACTION_HOLD_ACTION			,ACTION_TEXT17, H_Hold_Action_Button ),
ACTIONS(STR_ACTION_INVESTITURE			,ACTION_TEXT9 , H_Investiture_Button),
ACTIONS(STR_ACTION_LIEUTENANT			,ACTION_TEXT10, H_Lieutenant_Button ),
ACTIONS(STR_ACTION_LIEUTENANT_ACTION	,ACTION_TEXT11, H_Lieutenant_Action_Button ),
ACTIONS(STR_ACTION_MUSTER_ARMIES		,ACTION_TEXT12, H_Muster_Button ),
ACTIONS(STR_ACTION_REALM_SPELL			,ACTION_TEXT13, H_Realm_Spell_Button ),
ACTIONS(STR_ACTION_RULE					,ACTION_TEXT14, H_Rule_Button),
ACTIONS(STR_ACTION_TRADE_ROUTE			,ACTION_TEXT15, H_Trade_Routes),
ACTIONS(STR_ACTION_TAXATION				,ACTION_TEXT18, H_Taxation),
ACTIONS(STR_ACTION_DOMAIN_MAINT			,ACTION_TEXT19, H_Maintenance),
ACTIONS(STR_ACTION_PAYROLL				,ACTION_TEXT20, H_Payroll),
ACTIONS(STR_ACTION_COURT_VALUE			,ACTION_TEXT21, -1),
ACTIONS(STR_ACTION_DICE_ROLL			,ACTION_TEXT22,  H_Taxation_Roll),
ACTIONS(STR_ACTION_REALM_ACTIONS		,ACTION_TEXT23,  H_Domain_Actions),
ACTIONS(STR_ACTION_OCCUPATION			,ACTION_TEXT24, H_Occupation),
};
DEFINE_VECTOR_CLASS(ACTIONS const, Action);

/* ======================================================================== */
