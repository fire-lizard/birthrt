/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _COMBATUI_HXX
#define _COMBATUI_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define BUTNO_AVATAR_ICON	1   
#define BUTNO_AVATAR_HEALTH	2   
#define BUTNO_AVATAR_STATS	3   
#define BUTNO_AVATAR_INFO	4   

// Arcade
#define BUTNO_HIGH_ATTACK	5   
#define BUTNO_QHIGH_ATTACK	6   
#define BUTNO_LOW_ATTACK	7   
#define BUTNO_QLOW_ATTACK	8   
#define BUTNO_DEF_FALL_BTN	9   

#define BUTNO_SPELL1		10   
#define BUTNO_SPELL2		11   
#define BUTNO_SPELL3		12   
#define BUTNO_SPELL4		13   
#define BUTNO_SPELL5		14   
#define BUTNO_SPELL6		15   


// Bottom Bar buttons
#define BUTNO_RPG_RETREAT	1
#define BUTNO_RPG_DONE		2
#define BUTNO_RPG_MAP		3
#define BUTNO_RPG_LEAVE		4
#define BUTNO_RPG_LEADER   	5
#define BUTNO_RPG_VIEW		6
#define BUTNO_RPG_STATUS	7
#define BUTNO_RPG_INVENTORY	8


#define MULTI_EXIT    1


/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
class SCENE;

BOOL const AdventureAllocateData(SCENE & /* rScene */);
void AdventureDeallocateData(SCENE & /* rScene */);
void AdventureEndDialog( int send );

void InitAdventureUI(SCENE & /* rScene */);

void PlayAdventureUI(SCENE & /* rScene */);

void InitCombatUI(SCENE & /* rScene */);
void ReleaseCombatUI(SCENE & /* rScene */);
void PlayCombatUI(SCENE & /* rScene */);
void PauseCombatUI(SCENE & /* rScene */);

void ToggleCombatMode(LONG, LONG);

void DecreaseAdventureScreen(LONG /* unused */, LONG /* unused */ );
void IncreaseAdventureScreen(LONG /* unused */, LONG /* unused */ );

void AdventureShowStatus(LONG /* menuid */, LONG /* unused */ );
void AdventureShowInventory(LONG /* menuid */, LONG /* unused */ );
	
void CombatToggleMapKey(LONG , LONG );
void CombatToggleViewKey(LONG , LONG );
void CombatDefendKey(LONG /* menuid */, LONG );
void CombatLowAttackKey(LONG /* menuid */, LONG );
void CombatHighAttackKey(LONG /* menuid */, LONG );
void CombatFallbackKey(LONG /* menuid */, LONG /* unused */ );
void CombatSpellKey(LONG /* menuid */, LONG /* SpellNumber */ );
void CombatSwitchActive(LONG /* unused */ , LONG /* MenuIndex */);
void CombatCycleActive(LONG, LONG);
	

void CombatCastSpellBox(LONG /* hAvatar */, LONG /* SpellBoxNumber */ );
void CombatMenuPaint(LONG /* menuid */, LONG /* hAvatar */ );

void CombatTurnDoneKey(LONG /* unused */, LONG /* unused */ );
void CombatRetreatKey(LONG /* menuid */, LONG /* unused */);
void CombatNewLeaderKey(LONG /* unused */, LONG /* unused */);
void CombatReturnToMap(LONG /* unused */ , LONG /* unused */ );

void CombatShowStatusKey(LONG  /* menuid */, LONG  /* BottomMenuFlag */  );
void CombatShowInventoryKey(LONG /* unused */ , LONG /* unused */);

BOOL GetCombatMenuUp();
void SetCombatMenuUp(BOOL gCMR);

//WRC [24.3.97]
void ForceHires();
	
void AdventureFixUnitsArray(LONG /* iUnitLeader */);
#endif
