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
ACTIONS(STR_NULL,0),
ACTIONS(STR_ACTION_ADVENTURE			,ACTION_TEXT0),
ACTIONS(STR_ACTION_AGITATE				,ACTION_TEXT1),
ACTIONS(STR_ACTION_BUILD_RD				,ACTION_TEXT16),
ACTIONS(STR_ACTION_CONTEST				,ACTION_TEXT2),
ACTIONS(STR_ACTION_CREATE_HOLDING	   	,ACTION_TEXT3),
ACTIONS(STR_ACTION_DECLARE_WAR			,ACTION_TEXT4),
ACTIONS(STR_ACTION_DIPLOMACY			,ACTION_TEXT5),
ACTIONS(STR_ACTION_ESPIONAGE			,ACTION_TEXT6),
ACTIONS(STR_ACTION_FORGE_LEY_LINE		,ACTION_TEXT7),
ACTIONS(STR_ACTION_FORTIFY				,ACTION_TEXT8),
ACTIONS(STR_ACTION_HOLD_ACTION			,ACTION_TEXT17),
ACTIONS(STR_ACTION_INVESTITURE			,ACTION_TEXT9),
ACTIONS(STR_ACTION_LIEUTENANT			,ACTION_TEXT10),
ACTIONS(STR_ACTION_LIEUTENANT_ACTION	,ACTION_TEXT11),
ACTIONS(STR_ACTION_MUSTER_ARMIES		,ACTION_TEXT12),
ACTIONS(STR_ACTION_REALM_SPELL			,ACTION_TEXT13),
ACTIONS(STR_ACTION_RULE					,ACTION_TEXT14),
ACTIONS(STR_ACTION_TRADE_ROUTE			,ACTION_TEXT15),
ACTIONS(STR_ACTION_TAXATION				,ACTION_TEXT18),
ACTIONS(STR_ACTION_DOMAIN_MAINT			,ACTION_TEXT19),
ACTIONS(STR_ACTION_PAYROLL				,ACTION_TEXT20),
ACTIONS(STR_ACTION_COURT_VALUE			,ACTION_TEXT21),
ACTIONS(STR_ACTION_DICE_ROLL			,ACTION_TEXT22),
ACTIONS(STR_ACTION_REALM_ACTIONS		,ACTION_TEXT23),
ACTIONS(STR_ACTION_OCCUPATION			,ACTION_TEXT24),
};
DEFINE_VECTOR_CLASS(ACTIONS const, Action);

/* ======================================================================== */
