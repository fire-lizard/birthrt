/* ========================================================================
   Copyright (c) 1990,1996	  Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: Avatar.cpp
   Author:   Greg Hightower
   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined(_WINDOWS)
#include <Windows.h>
#endif

#include "SYSTEM.H"
#include "ENGINE.H"

#include "AI_UTILS.H"
#include "AVATAR.HXX"
//#include "btlstr.h"
#include "AIBTLCAP.HXX"
#include "GAMEMAP.HXX"
#include "GAMETYPE.HXX"
#include "GMENUENM.H"
#include "HANDLE.HXX"
#include "MAIN.HXX"
#include "GAME.H"
#include "PANEL.H"
#include "BATTLE.HXX"
#include "BATTLEUI.HXX"
#include "SCNMGR.HXX"
#include "SOUND.HXX"
#include "SPECIAL.H"
#include "strenum.h"
#include "STRMGR.H"
#include "SNDVOX.HXX"
#include "DESCRIBE.HXX"
#include "PLAYSTAT.HXX"
#include "REGENTS.HXX"
#include "SCENE.HXX"

#ifdef _WINDOWS
#include "WINSYS\MULPLAY.HXX"
#include "WINSYS\MESSQUE.HXX"
#include "WINSYS\MONO_C.H"
#ifdef OLD_SOUND
#include "winsys\sndutil.h"
#include "winsys\mciutil.h"
#include "winsys\musiutil.h"
#endif
#endif
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define	MAX_TROOP_ROWS	3
#define MAX_TROOP_COLS	7

/* this is the definition of the battle field grid */
/*                             */
/*        His Reserves		   */
/* ÚÄÄÄÄÂÄÄÄÄÂÄÄÄÄÂÄÄÄÄÂÄÄÄÄ¿  */
/* ³Away³    ³    ³    ³    ³  */
/* ³ 2,0³ 2,1³ 2,2³ 2,3³ 2,4³  */
/* ³Home³    ³    ³    ³    ³  */
/* ÀÄÄÄÄÁÄÄÄÄÁÄÄÄÄÁÄÄÄÄÁÄÄÄÄÙ  */
/* ÚÄÄÄÄÂÄÄÄÄÂÄÄÄÄÂÄÄÄÄÂÄÄÄÄ¿  */
/* ³    ³    ³    ³    ³    ³  */
/* ³ 1,0³ 1,1³ 1,2³ 1,3³ 1,4³  */
/* ³    ³    ³    ³    ³    ³  */
/* ÀÄÄÄÄÁÄÄÄÄÁÄÄÄÄÁÄÄÄÄÁÄÄÄÄÙ  */
/* ÚÄÄÄÄÂÄÄÄÄÂÄÄÄÄÂÄÄÄÄÂÄÄÄÄ¿  */
/* ³    ³    ³    ³    ³    ³  */
/* ³ 0,0³ 0,1³ 0,2³ 0,3³ 0,4³  */
/* ³    ³    ³    ³    ³    ³  */
/* ÀÄÄÄÄÁÄÄÄÄÁÄÄÄÄÁÄÄÄÄÁÄÄÄÄÙ  */
/*        My Reserves		   */
/*                             */
/* each grid is broken into 2 x and y sub points */

/* NOTE: look at the array below */

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
#define BTL_KILL_DELAY 1

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
void SendWin ( BOOL IWin );
void SendToReserves ( CAvatar *pAvatar );
void SendFallBack ( CAvatar *pAvatar, SHORT R, SHORT C );
void SendMagic ( CAvatar *pAvatar );

static void DisposeTroopList(CAvatar *pAvatar);
static void CheckDisconnect(void);
static void TransRowCol( SHORT * Row, SHORT * Column );
static void SendMove ( CAvatar *pAvatar );
static void SendArrive ( CAvatar *pAvatar );
static void SendResults ( CAvatar *pAvatar, SHORT points, SHORT shot );
static void SendRout	( CAvatar *pAvatar );
static int CheckArrive ( CAvatar *pAvatar );
static void CheckResults ( CAvatar *pAvatar );
static void CheckRout ( CAvatar *pAvatar );
static void CheckFallBack ( CAvatar *pAvatar );
static void CheckMove(CAvatar *pAvatar);
static void CheckReserves(CAvatar *pAvatar);
static void CheckWin ( void );
static void CheckMagic ( CAvatar *pAvatar );
static void CheckMulti ( CAvatar *pAvatar, CAvatar::AISTATUS Status );

static SHORT ResolveMissiles(CAvatar *pDefender);

static void MultiPrintf(const char *format, ...);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
DEFINE_MATRIX_DATA_S(POINT,BattleGridHome,GRID_MAX_ROWS,GRID_MAX_COLS) = {
{
	{GRID1_X,GRID11_Y},
	{GRID2_X,GRID11_Y},
	{GRID3_X,GRID11_Y},
	{GRID4_X,GRID11_Y},
	{GRID5_X,GRID11_Y},
},
{	
	{GRID1_X,GRID21_Y},
	{GRID2_X,GRID21_Y},
	{GRID3_X,GRID21_Y},
	{GRID4_X,GRID21_Y},
	{GRID5_X,GRID21_Y},
	
},
{	
	{GRID1_X,GRID31_Y},
	{GRID2_X,GRID31_Y},
	{GRID3_X,GRID31_Y},
	{GRID4_X,GRID31_Y},
	{GRID5_X,GRID31_Y},
	
}
};
DEFINE_MATRIX_CLASS_S(POINT, BattleGridHome, GRID_MAX_ROWS, GRID_MAX_COLS);
	
DEFINE_MATRIX_DATA_S(POINT,BattleGridAway,GRID_MAX_ROWS,GRID_MAX_COLS) = {
{	
	{GRID1_X,GRID12_Y},
	{GRID2_X,GRID12_Y},
	{GRID3_X,GRID12_Y},
	{GRID4_X,GRID12_Y},
	{GRID5_X,GRID12_Y},
	
},
{	
	{GRID1_X,GRID22_Y},
	{GRID2_X,GRID22_Y},
	{GRID3_X,GRID22_Y},
	{GRID4_X,GRID22_Y},
	{GRID5_X,GRID22_Y},
	
},
{	
	{GRID1_X,GRID32_Y},
	{GRID2_X,GRID32_Y},
	{GRID3_X,GRID32_Y},
	{GRID4_X,GRID32_Y},
	{GRID5_X,GRID32_Y},
}
};
DEFINE_MATRIX_CLASS_S(POINT, BattleGridAway, GRID_MAX_ROWS, GRID_MAX_COLS);
	
// this is used only for turn undead spell
BOOL fBtlTurnUndead = FALSE;

// list of what avatar is in which grid
//GEH extern DECL_MATRIX_CLASS_S(GRID,WhosWhere,GRID_MAX_ROWS,GRID_MAX_COLS);

extern BOOL fTutorialFirstBattle;		// from gamemap.cpp
extern BOOL	fDoTutReserves ;	// from battleui.cpp
extern BOOL	fDoTutMove ;		// from battleui.cpp
extern BOOL	fDoTutEngage ;		// from battleui.cpp
extern BOOL	fDoTutArchers ;		// from battleui.cpp

//---- If non-zero then we are fighting a remote player
extern DWORD dwBattleMultiId;  //---- If non zero then we have an remote player

extern BOOL fBattleFreezeTime;
extern BOOL fBattleNextTurn;
extern LONG BattleTick;
extern BOOL	fBtlTurnBased;
extern BOOL fUnitsStillMoving;

/* ========================================================================
   Function	- BattleCapt
   Description - AI Proc to find nearest opponent, then release troops to melee
   Returns	 - Current state
   ======================================================================== */
/* The battle capt's job is to move his troops from one point to another
 * and engage the enemy.  He will recieve movement commands from either
 * his mouse click routine or scene commands.  He is in charge of killing
 * off his troops to keep them in line with his hit pts.  He must coordinate
 * with the opposing battle capt to work out whose charging, defending, etc.
 */
void CAvatar::BattleCapt (CAvatar *pAvatar, CAvatar::AISTATUS Status)
{
	LONG		i;
	PTR_SCENE	pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
	SHORT		*pList; 
	SHORT				level;

	switch( Status )
	{
	case AI_INIT:
		{
		pAvatar->fBtlCap.mfInitVals(pAvatar->fBtlCap.TargetType, pAvatar->fBtlCap.TroopType);
		pAvatar->LeftButtonFn = (PFVC)BATTLE_CAPTAIN_DATA::mfLeftButton;
		pAvatar->RightButtonFn = (PFVC)BATTLE_CAPTAIN_DATA::mfRightButton;
		pAvatar->fBtlCap.WavePlaying = FALSE;
		
		switch (pAvatar->fBtlCap.UnitIcon)
		{
		// 3 hit points
		case A_LVY_UNIT_ICON:
		case A_KNT_UNIT_ICON:
		case D_INF_UNIT_ICON:
		case E_INF_UNIT_ICON:
		case C_SKL_UNIT_ICON:
		case A_EIN_UNIT_ICON:
		case N_IRR_UNIT_ICON:
			pAvatar->fBtlCap.TroopRows  = 3;
			pAvatar->fBtlCap.TroopCols  = 7;
			break;
		// 2 hit points
		case A_IRR_UNIT_ICON:
		case A_CAV_UNIT_ICON:
		case G_CAV_UNIT_ICON:
		case G_BOW_UNIT_ICON:
		case E_CAV_UNIT_ICON:
		case E_BOW_UNIT_ICON:
		case D_BOW_UNIT_ICON:
		case A_PIK_UNIT_ICON:
		case A_INF_UNIT_ICON:
		case A_BOW_UNIT_ICON:
		case G_INF_UNIT_ICON:
		case M_CAV_UNIT_ICON:
		case M_INF_UNIT_ICON:
		case M_BOW_UNIT_ICON:
		case M_IRR_UNIT_ICON:
		case M_PIK_UNIT_ICON:
		case N_INF_UNIT_ICON:
			pAvatar->fBtlCap.TroopRows  = 3;
			pAvatar->fBtlCap.TroopCols  = 5;
			break;
		// 1 hit point
		case A_SCT_UNIT_ICON:
			pAvatar->fBtlCap.TroopRows  = 2;
			pAvatar->fBtlCap.TroopCols  = 5;
			break;
		// special cases
		case C_SPD_UNIT_ICON:
		case C_HEL_UNIT_ICON:
			pAvatar->fBtlCap.TroopRows  = 1;
			pAvatar->fBtlCap.TroopCols  = 6;
			break;
		case C_HPY_UNIT_ICON:
		case C_WYV_UNIT_ICON:
			pAvatar->fBtlCap.TroopRows  = 1;
			pAvatar->fBtlCap.TroopCols  = 1;
		default: // 2 hit points
			pAvatar->fBtlCap.TroopRows  = 3;
			pAvatar->fBtlCap.TroopCols  = 5;
			break;
		}
		
		pAvatar->fBtlCap.HitCount = 0;
		pAvatar->fBtlCap.Healthy = 
			pAvatar->fBtlCap.HitPoints = 
			(SHORT)(pAvatar->fBtlCap.TroopRows*pAvatar->fBtlCap.TroopCols);
		
		level = 0;
		if (pAvatar->hPlayerStats != fERROR)
		{
			DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(pAvatar->hPlayerStats);
			SHORT SpellClass = pPlayerStats->mfGetSpellCastClass(SPELL_INFO::WIZARD);
			if (SpellClass != CANT_CAST_SPELLS)
			{
				level = pPlayerStats->mfGetLevel(SpellClass);
				level /= 3;
			}
			else
			{
				SpellClass = pPlayerStats->mfGetSpellCastClass(SPELL_INFO::PRIEST);
				if (SpellClass != CANT_CAST_SPELLS)
				{
					level = pPlayerStats->mfGetLevel(SpellClass);
					level -= 8;
					if (level > 0)
					{
						level /= 2;
					}
					else
					{
						level = 0;
					}
				}
			}
		}
		
		pAvatar->fBtlCap.SpellCount = level;
		pAvatar->fBtlCap.MissileCount = BattleTick + BtlActionRate;
		pAvatar->fBtlCap.MagicCount = BattleTick + BtlActionRate;
		
		// place myself in the reserves
		pAvatar->fBtlCap.Leader.hdlSlotList = fERROR;
		pAvatar->fBtlCap.Leader.hdlPathDeltas = fERROR;
		pAvatar->fBtlCap.FallBackCount = 0;
		BattleMoveToReservesArea(pAvatar->hThis);
		BattlePutInReserves(pAvatar->mfIsHomeTeam(), pAvatar->hThis);
		
		// tally up the players
		if(!pAvatar->mfIsHomeTeam())
			gAwayUnits++;
		
		// speed from reserves
		pAvatar->fBtlCap.Movement = 
			pAvatar->fBtlCap.Rate = (SHORT)(BTLCAP_MARCH_RATE * 8);
		
		// start the game paused
		pAvatar->Status = AI_PAUSED;
		
		if(!pAvatar->mfIsHomeTeam())
		{
#ifdef _WINDOWS
				if ( AMultiPlayer.IsMultiPlayerandRealm( pAvatar->Realm.HomeRealm ) )
				{
					CheckMove ( pAvatar );
				}
				else
#endif
				{
					// run the enemy ai once to get them setup on the field
					pAvatar->fBtlCap.mfRunEnemyBtlCapAI(pAvatar);
				}
		}
		// give all the character expierence points for just being brave
		pAvatar->fBtlCap.EverOnBtlField = FALSE;
		pAvatar->fBtlCap.TurnAction = TRUE;
		pAvatar->fBtlCap.TurnMove = TRUE;
		BattleExpPoints( pAvatar->UnitIndex, BTL_EXP_ENGAGE, 0, FALSE );
		}
		break;
		
	case AI_RELEASE:
		pAvatar->Status = AI_INIT;
		DisposeTroopList(pAvatar);
		break;

	case AI_BEGIN_PAUSE:
		// Tell everyone following or attacking me to stop and go away for now.
		pAvatar->Status = AI_PAUSED;
		pAvatar->mfStartAnimationOnce(STANDSEQ);
		DisposeTroopList(pAvatar);
		// take a powder myself
		pAvatar->mfSetInvisible(TRUE);
		break;
		
	case AI_PAUSED:
		// Just hang out here until called from the reserves
		if(	pAvatar->fBtlCap.Row == -1 ||
			pAvatar->fBtlCap.Row == 3 )
		{
			if(!pAvatar->mfIsHomeTeam())
			{
#ifdef _WINDOWS
				if ( AMultiPlayer.IsMultiPlayerandRealm( pAvatar->Realm.HomeRealm ) )
				{
					CheckMove ( pAvatar );
				}
				else
#endif
				{
					pAvatar->fBtlCap.mfRunEnemyBtlCapAI(pAvatar);
				}
			}
			break;
		}
		// else fall through

	case AI_END_PAUSE:
		// Tell everyone following me to continue on.
		pAvatar->mfSetInvisible(FALSE);
		pAvatar->Status = AI_SEARCH;
		
		// time base stuff
		pAvatar->fTimeLastMoved = SCENE_MGR::gTick;
		pAvatar->fBtlCap.MissileCount = BattleTick + BtlActionRate;
		pAvatar->fBtlCap.MagicCount = BattleTick + BtlActionRate;
		// set up a action count for enemy AI's
		pAvatar->fBtlCap.ActionCount = 
			BattleTick + 							// current time
			2 * BtlActionRate +						// ~10 seconds
			(BtlActionRate * ((SpeedLevel+1)/2)) +	// speed number of turns
			random(BtlActionRate)					// not everyone move at once
			;
			
		pAvatar->mfStartAnimationOnce(STANDSEQ);
		pAvatar->Engaged = 0;
		
		// now create my troops
		pAvatar->CreateBattleTroops();
		pAvatar->MoveBattleTroops();
		
		break;
		
	case AI_ATTACK:
		{
		LONG NewAngle;
		
		if (fBattleFreezeTime)
			return;
			
		// begin the attack
		// unless engaged, lets try always facing the camera 
		// so that our unitinfo box always shows my facing
		if(pAvatar->hEnemy != fERROR)
		{
			CAvatar * const pEnemy = (CAvatar * const)BLKPTR(pAvatar->hEnemy);
			if(!IsPointerGood(pEnemy))
			{
				pAvatar->hEnemy = fERROR;
				pAvatar->Status = AI_SEARCH;
				break;
			}
			else
			{
				NewAngle = AngleFromPoint(
					pAvatar->mfX(),
					pAvatar->mfY(),
					pEnemy->mfX(),
					pEnemy->mfY(),
					RESOLUTION_8);
			}
		}
		else
		{
			NewAngle = AngleFromPoint(
				pAvatar->mfX(),
				pAvatar->mfY(),
				CAMERA_INT_VAL(camera.x), 
				CAMERA_INT_VAL(camera.y), 
				RESOLUTION_8);
		}

		pAvatar->mfRotateToward(pAvatar->mfAngle(), NewAngle);
		pAvatar->mfStartAnimationOnce(STANDSEQ);
			
		pAvatar->fBtlCap.Rate = 
			BTLCAP_MARCH_RATE * 
			BattleTroopInfo[pAvatar->fBtlCap.UnitIcon].Move ;
		
		// try to reengage any of my troops
		pAvatar->fBtlCap.mfMelee(pAvatar);
		
		// attacks are resolved every battle action time
		if(pAvatar->fBtlCap.ActionCount < BattleTick)
		{
			fUpdatePanels = TRUE;
			
			pAvatar->fBtlCap.ActionCount = 
				BattleTick + (BtlActionRate * SpeedLevel);
			
			switch(pAvatar->fBtlCap.UnitIcon)
			{
			case G_CAV_UNIT_ICON:
				AddSndObj(SND_WOLF_SNARL1, SND_WOLF_SNARL_TOTAL, VOLUME_SEVENTY);
				break;
			case A_CAV_UNIT_ICON :
			case A_KNT_UNIT_ICON :
				AddSndObj(SND_HORSE_NOISE1, SND_HORSE_NOISE_TOTAL, VOLUME_SEVENTY);
				break;
			}

			CheckDisconnect();
			
			if ( dwBattleMultiId != kNoMultiId ) //@@@@
			{
			   if ( pAvatar->mfIsHomeTeam() )
			   {
			   		ResolveAttack(pAvatar->hThis, pAvatar->hEnemy, BTLCAP_MELEE);
			   }
			}
			else
			{
				ResolveAttack(pAvatar->hThis, pAvatar->hEnemy, BTLCAP_MELEE);
			}
		}
		}
		break;

	case AI_SEARCH:		
		{
		LONG tx = pAvatar->fBtlCap.TargetX;
		LONG ty = pAvatar->fBtlCap.TargetY;
		LONG oldX = pAvatar->mfX();
		LONG oldY = pAvatar->mfY();
		
		// test for shooting type actions
		if(pAvatar->fBtlCap.Shoot)
		{
			if (fBattleFreezeTime)
				return;
				
			pAvatar->fBtlCap.mfShoot(pAvatar);
			pAvatar->fBtlCap.Shoot = BTLCAP_NOSHOT;
		}
		else
		if (tx != oldX || ty != oldY)
		{

			SendMove( pAvatar );

			// NOTE: we started moving

			fUpdatePanels = TRUE;
			pAvatar->Status = AI_MOVING;
			pAvatar->fTimeLastMoved = SCENE_MGR::gTick;
		}
		else
		if( pAvatar->fBtlCap.TargetRow || pAvatar->fBtlCap.TargetCol )
		{
			BOOL MovedSideways = FALSE;
			// NOTE: we started moving
			fUpdatePanels = TRUE;
			
			if(pAvatar->fBtlCap.TargetRow<0)
			{
				if (pAvatar->fBtlCap.mfMoveBack(0, pAvatar->hThis) == TRUE)
				{
					pAvatar->fBtlCap.TargetRow++;
				}
				else if (pAvatar->fBtlCap.TargetCol<0)
				{
					// Couldn't move back, did we want to try moving left?
					pAvatar->fBtlCap.TargetCol++;
					pAvatar->fBtlCap.mfMoveLeft(0, pAvatar->hThis);
					MovedSideways = TRUE;
				}
				else if (pAvatar->fBtlCap.TargetCol>0)
				{
					// Couldn't move back, did we want to try moving right?
					pAvatar->fBtlCap.TargetCol--;
					pAvatar->fBtlCap.mfMoveRight(0, pAvatar->hThis);
					MovedSideways = TRUE;
				}
				else
				{
					// Can't move, so decrement the requested count.
					pAvatar->fBtlCap.TargetRow++;
				}
			}
			else
			if(pAvatar->fBtlCap.TargetRow>0)
			{
				if (pAvatar->fBtlCap.mfMoveAhead(0, pAvatar->hThis) == TRUE)
				{
					pAvatar->fBtlCap.TargetRow--;
				}
				else if (pAvatar->fBtlCap.TargetCol<0)
				{
					// Couldn't move ahead, did we want to try moving left?
					pAvatar->fBtlCap.TargetCol++;
					pAvatar->fBtlCap.mfMoveLeft(0, pAvatar->hThis);
					MovedSideways = TRUE;
				}
				else if (pAvatar->fBtlCap.TargetCol>0)
				{
					// Couldn't move ahead, did we want to try moving right?
					pAvatar->fBtlCap.TargetCol--;
					pAvatar->fBtlCap.mfMoveRight(0, pAvatar->hThis);
					MovedSideways = TRUE;
				}
				else
				{
					// Can't move, so decrement the requested count.
					pAvatar->fBtlCap.TargetRow--;
				}
			}
			
			// If we didn't movesideways because we were blocked from going forward or back.
			if (MovedSideways == FALSE)
			{
				if(pAvatar->fBtlCap.TargetCol<0)
				{
					// Were we successfull or we aren't trying to move ahead or back.
					if(pAvatar->fBtlCap.mfMoveLeft(0, pAvatar->hThis) == TRUE ||
					   pAvatar->fBtlCap.TargetRow == 0)
					{
						pAvatar->fBtlCap.TargetCol++;
					}
				}
				else
				if(pAvatar->fBtlCap.TargetCol>0)
				{
					// Were we successfull or we aren't trying to move ahead or back.
					if (pAvatar->fBtlCap.mfMoveRight(0, pAvatar->hThis) == TRUE ||
					    pAvatar->fBtlCap.TargetRow == 0)
					{
						pAvatar->fBtlCap.TargetCol--;
					}
				}
			}
			SendMove( pAvatar );
		}
		else
		{
			if(pAvatar->mfIsHomeTeam())
				pAvatar->mfRotateToward(pAvatar->mfAngle(), 0);
			else
				pAvatar->mfRotateToward(pAvatar->mfAngle(), 128);
			pAvatar->mfStartAnimationOnce(STANDSEQ);
			
			if (!fBattleFreezeTime)
				pAvatar->fBtlCap.mfTryToEngageEnemyBtlCap(pAvatar);
			
			if(pAvatar->fBtlCap.OnBtlField)
			{
				pAvatar->fBtlCap.Rate = 
					BTLCAP_MARCH_RATE * 
					BattleTroopInfo[pAvatar->fBtlCap.UnitIcon].Move ;
			}
			else
			{
				pAvatar->fBtlCap.Rate = (SHORT)(BTLCAP_MARCH_RATE * 8);
			}
		
			if(!pAvatar->mfIsHomeTeam() && pAvatar->Status == AI_SEARCH)
			{
#ifdef _WINDOWS
				if ( AMultiPlayer.IsMultiPlayerandRealm( pAvatar->Realm.HomeRealm ) )
				{
					CheckMove ( pAvatar );
				}
				else
#endif
				{
					pAvatar->fBtlCap.mfRunEnemyBtlCapAI(pAvatar);
				}
			}
		}
		
		// make our little duck follow us
		pAvatar->MoveBattleTroops();
		
		}
		break;
	
	case AI_MOVING:
		{
		LONG tx = pAvatar->fBtlCap.TargetX;
		LONG ty = pAvatar->fBtlCap.TargetY;
		LONG oldX = pAvatar->mfX();
		LONG oldY = pAvatar->mfY();
		
		if (tx != oldX || ty != oldY)
		{ 
			// -- signal that units are still on the move
			if ( pAvatar->fBtlCap.OnBtlField )
				fUnitsStillMoving = TRUE;
				
			//---- If this isn't my guy then check for arrive message 
			if ( !pAvatar->mfIsHomeTeam() &&
				 CheckArrive ( pAvatar )     ) 
  	        {  
				// NOTE: we stopped moving 
				
				fUpdatePanels = TRUE;
			
				pAvatar->Status = AI_SEARCH;

			}
			else
			{
			 	pAvatar->fBtlCap.Movement = 
			 		pAvatar->fBtlCap.mfTerrainSpeed(pAvatar, pAvatar->fBtlCap.Rate)
			 		* (SCENE_MGR::gTick - pAvatar->fTimeLastMoved);
			 	pAvatar->FaceTo(tx, ty);
			 	pAvatar->MoveToward( tx, ty, 0, pAvatar->fBtlCap.Movement);
			 	pAvatar->mfStartAnimationOnce(MARCHSEQ);
			}

		}
		else
		{
			if( fBtlTurnBased 
				&& ( pAvatar->fBtlCap.TargetRow || pAvatar->fBtlCap.TargetCol ))
			{
				pAvatar->Status = AI_SEARCH;
				break;
			}
			
			// -- see if it is the next turn
			if ( fBtlTurnBased && !fBattleNextTurn )
				break;
				
			//---- If this is my team tell other machine I made it 

			// GEH this is an attempt to limit the shoot and run tactics that
			// seem too effective
			// reset my shoot timer a little
			if ( !fBtlTurnBased )
				pAvatar->fBtlCap.MissileCount = BattleTick + (BtlActionRate/4);
			
			if ( pAvatar->mfIsHomeTeam() )
			{
				SendArrive ( pAvatar );
 			}

			// NOTE: we stopped moving

			fUpdatePanels = TRUE;
			
			// if I was sent to the reserves, pause myself next turn
			if (pAvatar->fBtlCap.OnBtlField)
			{
				//GEH TUTORIAL
				if (pAvatar->mfIsHomeTeam() &&
					fTutorialFirstBattle && 
					fDoTutMove ) 
				{
					fDoTutMove = FALSE;
					//SystemHelp("TUTORIAL TEST", "Select a unit on the field and move them to one of the blue hilighted squares", -1, NULL, 0);
					DescribeBtlMovement();
				}
				pAvatar->Status = AI_SEARCH;
			}
			else
				pAvatar->Status = AI_BEGIN_PAUSE;
		}
				
		pAvatar->MoveBattleTroops();
		pAvatar->fTimeLastMoved = SCENE_MGR::gTick;	
		
		}
		break;
		
	case AI_DEAD:
		// pretend to be a lump of meat slowly approching air temperature
		if (pAvatar->CurSequence != DEADSEQ &&
			pAvatar->mfTestSequenceDone() )
		{
			// if the death was visible, send my horse off to the sidelines
			if( pAvatar->mfIsVisible() )
			{
				// send a cav horse running off
				if ((pAvatar->mfType() == LIGHT_CAVALRY ) ||
					(pAvatar->mfType() == MERC_CAVALRY ) ||
					(pAvatar->mfType() == LORES_A_CAV ) )
					pAvatar->mfCreateHorse(GetNewId(), LORES_CAV_HORSE);
				
				// send a night horse running off
				if ((pAvatar->mfType() == KNIGHT ) ||
					(pAvatar->mfType() == ELF_CAVALRY ) ||
					(pAvatar->mfType() == LORES_A_KNT ) )
					pAvatar->mfCreateHorse(GetNewId(), LORES_KNIGHT_HORSE);
			}
				
			pAvatar->mfPlayAnimationOnce(DEADSEQ);
			// GEH pAvatar->SetAIFuncIndex(AI_FUNC_NULL);
		}
		
		if (pAvatar->mfGetArtSequence() != DEADSEQ &&
			pAvatar->mfTestSequenceDone() )
			pAvatar->mfPlayAnimationOnce(DEADSEQ);
		
		// dont break!
		return;
	
	default:
		break;
	}


	//---- Check for remote actions 

	CheckMulti ( pAvatar, Status );

	/* -----------------------------------------------------------------
	   exec each frame pass...
	   ----------------------------------------------------------------- */
	// check for damage from a drive-by magic shooting 
	if(pAvatar->KillType != ARROWS && pAvatar->KillType != NO_THING)
	{
		CheckDisconnect();

		if ( dwBattleMultiId != kNoMultiId )
		{
			//if ( pAvatar->mfIsHomeTeam() )
			{
				ResolveBattleMagic(pAvatar);
				while(pAvatar->fBtlCap.HitCount > 0)
					pAvatar->fBtlCap.mfKillTroop(pAvatar,pAvatar->KillType);
				AddSndObj( SND_MALE_GROUP_DIE1, SND_MALE_GROUP_DIE_TOTAL, VOLUME_EIGHTY);
			}
		}
		else
		{
			ResolveBattleMagic(pAvatar);
			while(pAvatar->fBtlCap.HitCount > 0)
				pAvatar->fBtlCap.mfKillTroop(pAvatar,pAvatar->KillType);
			AddSndObj( SND_MALE_GROUP_DIE1, SND_MALE_GROUP_DIE_TOTAL, VOLUME_EIGHTY);
		}
	}
	else
	// check for arrow/missile damage
	if(pAvatar->KillType == ARROWS)
	{
		//if(pAvatar->fBtlCap.Shot > BattleTick)
		{
			ResolveMissiles(pAvatar);
		}
	}

	// clear outside kill types
	pAvatar->KillType = NO_THING;
	
	// do we need to resolve any damage?
	if(pAvatar->fBtlCap.HitCount > 0)
	{
		while(pAvatar->fBtlCap.HitCount > 0)
			pAvatar->fBtlCap.mfKillTroop(pAvatar,NO_THING);
			
		AddSndObj( SND_MALE_GROUP_DIE1, SND_MALE_GROUP_DIE_TOTAL, VOLUME_EIGHTY);
	}

	if(pAvatar->fBtlCap.HitPoints <= 0) // I B DEAD
		KillBattleCaptain(pAvatar);
}

/* ========================================================================
   Function    - BtlCapGenerate
   Description - generate the list of battle captain avatars
   Returns     - TRUE for ok, FALSE or somekind of failure
   ======================================================================== */
BOOL BtlCapGenerate( SHORT UnitIndex )
{
	BOOL	ReturnVal = FALSE;	// assume total failure!
	CAvatar *pAvatar;
	CHAR	buffer[80];
	SHORT	X,Y;
	SHORT	UnitCount = 0;
	
	while(UnitIndex != -1)
	{
		ReturnVal = TRUE;	// got past first step
		
		// no more then gUnitStackSize units may ever go to a single battle
		if(++UnitCount > gUnitStackSize)
			return TRUE;
			
		if (units[UnitIndex].iHandle != fERROR)
		{
			pAvatar = (CAvatar *) BLKPTR(units[UnitIndex].iHandle);
			pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_BATTLE_CAPTAIN);
			pAvatar->mfInitVals();
			
			// GEH now load the icon for this unit, note that we use
			//     the same name as the map level, just add a 'B' to the
			//     beginning of the name
			sprintf(buffer,"%sB%s%s", ICON_DIR, GameIcons[units[UnitIndex].Icon], ICON_EXT);
			pAvatar->fBtlCap.hIcon = GetResourceStd(buffer, FALSE);
			pAvatar->fBtlCap.OfficerIcon = (SHORT) units[UnitIndex].Icon;
			
			// if this is an officer of some type, inherite some
			// appropriate body guards
			if (units[UnitIndex].Icon >= FIRST_CHARACTER_TYPE)
			{
				THINGTYPE	BodyGuardType;
				SHORT		IconType;
				switch(units[UnitIndex].Icon)
				{
				case REG_ELORD1_UNIT_ICON:
				case REG_ELORD2_UNIT_ICON:
				case REG_ELADY1_UNIT_ICON:
				case LET_ELORD1_UNIT_ICON:
				case LET_ELORD2_UNIT_ICON:
				case LET_ELADY1_UNIT_ICON:
					BodyGuardType = ELF_ARCHER;
					IconType = E_BOW_UNIT_ICON;
					break;
					
				case REG_DLORD1_UNIT_ICON:
				case REG_DLORD2_UNIT_ICON:
				case LET_DLORD1_UNIT_ICON:
				case LET_DLORD2_UNIT_ICON:
					BodyGuardType = DWARF_ARCHER;
					IconType = D_BOW_UNIT_ICON;
					break;
				
				case REG_GLORD1_UNIT_ICON:
				case REG_GLADY1_UNIT_ICON:
				case LET_GLORD1_UNIT_ICON:
				case LET_GLADY1_UNIT_ICON:
					BodyGuardType = GOBLIN_ARCHER;
					IconType = G_BOW_UNIT_ICON;
					break;
				
				case GORGON_UNIT_ICON:
					BodyGuardType = T_SKELETON;
					IconType = C_SKL_UNIT_ICON;
					break;
				
				case SPIDERKING_UNIT_ICON:
					BodyGuardType = T_GIANT_SPIDER;
					IconType = C_SPD_UNIT_ICON;
					break;
				
				case RHUOBHE_UNIT_ICON:
					BodyGuardType = ELF_CAVALRY;
					IconType = E_CAV_UNIT_ICON;
					break;
				
				default:
					BodyGuardType = ELITE_INFANTRY;
					IconType = A_EIN_UNIT_ICON;
				}
				pAvatar->fBtlCap.TroopType = BodyGuardType;
				pAvatar->fBtlCap.TargetType = BodyGuardType;
				pAvatar->fBtlCap.UnitIcon = IconType;
			}
			else
			{
				pAvatar->fBtlCap.TroopType = pAvatar->mfType();
				pAvatar->fBtlCap.TargetType = pAvatar->mfType();
				pAvatar->fBtlCap.UnitIcon = (SHORT) units[UnitIndex].Icon;
			}
			
			// start out in the reserves
			if(pAvatar->mfIsHomeTeam())
			{
				Y = MY_RESERVES_Y;
			}
			else
			{
				Y = ENEMY_RESERVES_Y;
			}
			X = (SHORT)(1000 - random(2000));
			
			pAvatar->mfSetInvisible(TRUE);
			pAvatar->mfMoveTo(X, Y);
			pAvatar->fBtlCap.UnitIndex = (SHORT) UnitIndex;
		}
		UnitIndex = (SHORT) units[UnitIndex].NextUnit;
	}
	
	return ReturnVal;
}

/* ========================================================================
   Function	- CreateBattleTroops
   Description - Make some troops for a Battle Captain.
   Returns	 - void
   ======================================================================== */
void CAvatar::CreateBattleTroops(void)
{
	LONG i;
	LONG k;
	// Note : Don't lock the scene here, its already locked.
	PTR_SCENE	pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
	const LONG	X = mfX();
	const LONG	Y = mfY();
	const LONG	Z = mfZ();
	SHORT		*pList;
	
	// init leader data
	fBtlCap.Leader.mfInitVals();
	fBtlCap.Leader.sCount = 0;
	fBtlCap.Leader.sMaxFollowers = (SHORT)((fBtlCap.TroopRows*fBtlCap.TroopCols));
	
	fBtlCap.Leader.hdlSlotList = 
		NewLockedBlock(sizeof(SHORT) * fBtlCap.Leader.sMaxFollowers);
	//GEH SetBlockAttr(fBtlCap.Leader.hdlSlotList, LOCKED, LOCKED);
	
	// No memory for now.
	if (fBtlCap.Leader.hdlSlotList == fERROR)
		return;
	
	pList = (SHORT *)BLKPTR(fBtlCap.Leader.hdlSlotList);
	memset(pList, -1, sizeof(SHORT) * fBtlCap.Leader.sMaxFollowers);
	
	// create my troops and record their handles in my follower list
	//for(k=0; k<fBtlCap.TroopRows; k++)
	//{
		for(i=0; i<fBtlCap.HitPoints; i++)
		{
			LONG index = CAvatar::CreateAvatar( GetNewId(), 
										fBtlCap.TroopType, 
										X, Y, 0, 
										AI_FUNC_FOLLOW_BTLCAP , 
										pCurrentScene, 
										Realm.HomeRealm);
			
			if (index != fERROR  && pCurrentScene->Avatars[index] != fERROR)
			{
				CAvatar *pTroop = (CAvatar *) BLKPTR(pCurrentScene->Avatars[index]);
				pTroop->attrib.RuntimeCreated = TRUE;
				
				pTroop->fFollowBtlCap.hLeader = hThis;	// who to follow
				pTroop->hEnemy = fERROR;
				pTroop->Engaged = 0;
	
				//pList[k*fBtlCap.TroopCols+i] = pTroop->hThis;
				pList[i] = pTroop->hThis;
				
				const LONG ScaleAdjust = 100 - (random(20) - 10);
				pTroop->mfChangeScale(ScaleAdjust);
				
			}
			else
			{
				// No slots left.
				//pList[k*fBtlCap.TroopCols+i] = fERROR;
				pList[i] = fERROR;
				//printf("WARNING! Unable to create requested troops. %d\n", (fBtlCap.TroopRows - k) * (fBtlCap.TroopCols - i));
				return;
			}
		}
	//}
	
}

/* ========================================================================
   Function	- MoveBattleTroops
   Description - move troops for a Battle Captain.
   Returns	 - void
   ======================================================================== */
void CAvatar::MoveBattleTroops(void)
{
	LONG	i,k;
	LONG	r,c;
	const LONG X = mfX();
	const LONG Y = mfY();
	const LONG A = mfAngle();
	SHORT	*pList;
	
	if (fBtlCap.Leader.hdlSlotList == fERROR)
		return;
	
	pList = (SHORT *)BLKPTR(fBtlCap.Leader.hdlSlotList);
	if (pList == NULL)
		return;
	
	// upper left of formation
	r = ((fBtlCap.TroopRows+1)/2)-fBtlCap.TroopRows;
	c = ((fBtlCap.TroopCols+1)/2)-fBtlCap.TroopCols;
	
	// move my troops
	for(k=0; k<fBtlCap.TroopRows; k++)
	{
		for(i=0; i<fBtlCap.TroopCols; i++)
		{
			if(pList[k*fBtlCap.TroopCols+i] != fERROR)
			{
				CAvatar *pTroop = (CAvatar *) BLKPTR(pList[k*fBtlCap.TroopCols+i]);
					
				pTroop->fFollowBtlCap.NewX = X+(80*(c+i));
				pTroop->fFollowBtlCap.NewY = Y+(100*(r+k));
				if(pTroop->mfIsHomeTeam())
					pTroop->fFollowBtlCap.NewA = 0;
				else
					pTroop->fFollowBtlCap.NewA = 127;
			}
		}
	}
}

/* ========================================================================
   Function    - ResolveAttack
   Description - figure out the results of an attatck
   Returns     - hit points delt, positive is defender, negative is attacker
   ======================================================================== */
SHORT ResolveAttack(SHORT hDefender, SHORT hAttacker, LONG AttackType)
{
	CAvatar	*pDefender, *pAttacker;
	SHORT	AttackerClass, AttackerValue;
	SHORT	DefenderClass, DefenderValue;
	SHORT	result;
	SHORT	points = 0;
	SHORT	Bonus;
	SHORT	SpecialAttacker;
	SHORT	SpecialDefender;
	SHORT	type;
	SHORT	i,j;
		
	//GEH just a little protection is fine
	if(hDefender == fERROR || hAttacker == fERROR)
		return 0;
		
	fBattleStarted = TRUE;	// be sure we know fighting has started
	
	pDefender = (CAvatar *)BLKPTR(hDefender);
	pAttacker = (CAvatar *)BLKPTR(hAttacker);

	
	DefenderClass = GetBattleClass(pDefender->fBtlCap.OfficerIcon);
	DefenderValue = GetBattleDefense(pDefender->fBtlCap.OfficerIcon);
	
	switch(AttackType)
	{
	case BTLCAP_MELEE:
		AttackerClass = GetBattleClass(pAttacker->fBtlCap.OfficerIcon);
		// use charge unless we we're down 1/3
		AttackerValue = GetBattleCharge(pAttacker->fBtlCap.OfficerIcon);
		if( AttackerValue == 0 ||
			(pAttacker->fBtlCap.HitPoints*3 < pAttacker->fBtlCap.Healthy*2)
			)
			AttackerValue = GetBattleMelee(pAttacker->fBtlCap.OfficerIcon);
		break;
	case BTLCAP_CHARGE:
		AttackerClass = GetBattleClass(pAttacker->fBtlCap.OfficerIcon);
		AttackerValue = GetBattleCharge(pAttacker->fBtlCap.OfficerIcon);
		if( AttackerValue == 0)
			AttackerValue = GetBattleMelee(pAttacker->fBtlCap.OfficerIcon);
		break;
	case BTLCAP_MISSILE:
		AttackerClass = GetBattleClass(pAttacker->fBtlCap.OfficerIcon);
		AttackerValue = GetBattleMissile(pAttacker->fBtlCap.OfficerIcon);
		break;
	case BTLCAP_MAGIC:
		AttackerClass = GetBattleClass(pAttacker->fBtlCap.OfficerIcon);
		AttackerValue = GetBattleMagic(pAttacker->fBtlCap.OfficerIcon);
		break;
	default:
		// illegal value
		return 0;
	}
	
	// blessed armies get bonus to attack values
	if ( units[pAttacker->UnitIndex].Blessed )
		AttackerValue++;
		
	// now add any Defender bonuses
	if (pDefender->hPlayerStats == fERROR)
		return points;
		
	DumbAutoLockPtr<PLAYER_STATS const> pPlayerStats(pDefender->hPlayerStats);
	DefenderValue += pPlayerStats->StatsMod.mfGetMod(STATSMOD::BTL_ATTACK_MOD);
	
	Bonus = GetBattleBonus(pDefender->fBtlCap.OfficerIcon);
	type = pAttacker->fBtlCap.UnitIcon;
	switch(Bonus)
	{
	case BTL_PIKEMEN:
		// can't be charged
		if( AttackType == BTLCAP_CHARGE)
			AttackerValue = GetBattleMelee(pAttacker->fBtlCap.OfficerIcon);
		
		if( type == A_CAV_UNIT_ICON ||
			type == A_KNT_UNIT_ICON ||
			type == G_CAV_UNIT_ICON
			)
		{
			DefenderValue++;
		}
	}
	
	if(AttackType != BTLCAP_MAGIC)
	{
		// adjust values based on percentage of troops remaining
		AttackerValue = (SHORT)(AttackerValue 
			* pAttacker->fBtlCap.HitPoints 
			/ pAttacker->fBtlCap.Healthy);
			
	}
		
	// now add any Attacker bonuses
	if (pAttacker->hPlayerStats == fERROR)
		return points;
		
	pPlayerStats = pAttacker->hPlayerStats;
	AttackerValue += pPlayerStats->StatsMod.mfGetMod(STATSMOD::BTL_ATTACK_MOD);
	
	Bonus = GetBattleBonus(pAttacker->fBtlCap.OfficerIcon);
	type = pDefender->fBtlCap.UnitIcon;
	switch(Bonus)
	{
	case BTL_VS_CAVALRY:
		if( type == A_CAV_UNIT_ICON ||
			type == A_KNT_UNIT_ICON ||
			type == G_CAV_UNIT_ICON
			)
		{
			AttackerValue++;
		}
		break;
		
	case BTL_MISSILE_VS_CAVALRY:
		if( type == A_CAV_UNIT_ICON ||
			type == A_KNT_UNIT_ICON ||
			type == G_CAV_UNIT_ICON
			)
		{
			if( AttackType == BTLCAP_MISSILE )
				AttackerValue++;
		}
		break;
	case BTL_VS_PIKES_IRREG:
		if( type == A_PIK_UNIT_ICON ||
			type == A_IRR_UNIT_ICON ||
			type == M_IRR_UNIT_ICON ||
			type == M_PIK_UNIT_ICON )
		{
			AttackerValue++;
		}
		break;
		
	case BTL_MELEE_VS_PIKES_IRREG:
		if( type == A_PIK_UNIT_ICON ||
			type == A_IRR_UNIT_ICON ||
			type == M_IRR_UNIT_ICON ||
			type == M_PIK_UNIT_ICON )
		{
			if(AttackType == BTLCAP_MELEE)
				AttackerValue++;
		}
		break;
		
	case BTL_PIKEMEN:
		if( type == A_CAV_UNIT_ICON ||
			type == A_KNT_UNIT_ICON ||
			type == G_CAV_UNIT_ICON
			)
		{
			AttackerValue++;
		}
	}
	
	// attend to any terrain advantage/disadvantages
	SpecialAttacker = sectors[pAttacker->mfGetSector()].special;
	switch(SpecialAttacker)
	{
	case SSP_DEEP_WATER:
		// no fighting in deep water
		//GEH return 0;
		//break;
		
		// actually, let's do a little damage
		AttackerValue/=2;
		//fall through
		
	case SSP_WATER: // also known as Swamp
		if(AttackType == BTLCAP_CHARGE)	// can't charge
			return 0;
		
		// mounted units suffer a -1 penalty to their melee
		type = pAttacker->fBtlCap.UnitIcon;
		if( type == A_CAV_UNIT_ICON ||
			type == A_KNT_UNIT_ICON ||
			type == G_CAV_UNIT_ICON
			)
		{
			if (AttackerValue)
			AttackerValue--;
		}
		break;
		
	case SSP_MOUNTAIN:
		if(AttackType == BTLCAP_CHARGE)	// can't charge
			return 0;
		
		break;
		
	case SSP_FOREST:
		if(AttackType == BTLCAP_CHARGE)	// can't charge
			return 0;
		
		// Elves ignore missile disadvantage, and get an advantage in melee
		type = pAttacker->fBtlCap.UnitIcon;
		if( type == E_INF_UNIT_ICON ||
			type == E_BOW_UNIT_ICON ||
			type == E_CAV_UNIT_ICON
			)
		{
			AttackerValue++;
		}
		break;
		
	case SSP_NORMAL:
	default:
		break;
	}
	
	SpecialDefender = sectors[pDefender->mfGetSector()].special;
	switch(SpecialDefender)
	{
	case SSP_DEEP_WATER:
		// no fighting in deep water
		// return 0;
		
		// Actually, full defence in deep water
		
		// fall through
		
	case SSP_WATER: // also known as Swamp
		if(AttackType == BTLCAP_CHARGE)	// can't charge
			return 0;
		
		// mounted units suffer a -1 penalty to their defense
		type = pDefender->fBtlCap.UnitIcon;
		if( type == A_CAV_UNIT_ICON ||
			type == A_KNT_UNIT_ICON ||
			type == G_CAV_UNIT_ICON
			)
		{
			if(DefenderValue)
				DefenderValue--;
		}
		break;
		
	case SSP_MOUNTAIN:
		if(AttackType == BTLCAP_CHARGE)	// can't charge
			return 0;
		
		// Dwarves ignore defend advantage
		type = pAttacker->fBtlCap.UnitIcon;
		if( type != D_INF_UNIT_ICON ||
			type != D_BOW_UNIT_ICON
			)
		{
			DefenderValue++;
		}
		break;
		
	case SSP_FOREST:
		if(AttackType == BTLCAP_CHARGE)	// can't charge
			return 0;
		
		// Elves ignore missile disadvantage, and get an advantage in melee
		type = pAttacker->fBtlCap.UnitIcon;
		if( type != E_INF_UNIT_ICON ||
			type != E_BOW_UNIT_ICON ||
			type != E_CAV_UNIT_ICON
			)
		{
			if(AttackerValue >= 2 && AttackType == BTLCAP_MISSILE)
				AttackerValue -= 2;
		}
		
		// Elves also get an advantage in defense
		type = pAttacker->fBtlCap.UnitIcon;
		if( type == E_INF_UNIT_ICON ||
			type == E_BOW_UNIT_ICON ||
			type == E_CAV_UNIT_ICON
			)
		{
			DefenderValue++;
		}
		break;
	case SSP_NORMAL:
	default:
		break;
	}
	
	result = (SHORT)ResolveBattleCard(
		AttackerClass, AttackerValue,
		DefenderClass, DefenderValue
		);
		
	// look for resistance type things that over-ride the result
	pPlayerStats = pAttacker->hPlayerStats;
	type = pDefender->fBtlCap.UnitIcon;
	// if I have resist evil and the defender is a evil type, its a rout
	if( pPlayerStats->StatsMod.mfGetMod(STATSMOD::EVIL_RESIST_MOD) )
	{
		if( 
			type == C_HPY_UNIT_ICON ||
			type == C_SKL_UNIT_ICON ||
			type == C_SPD_UNIT_ICON ||
			type == C_HEL_UNIT_ICON ||
			type == C_WYV_UNIT_ICON
			)
			result = BTL_R;
	}
	
	// now look at any defender result bonuses
	Bonus = GetBattleBonus(pDefender->fBtlCap.OfficerIcon);
	switch(Bonus)
	{
	case BTL_IGNORE_RF_BY_MAGIC:
		if( AttackType == BTLCAP_MAGIC && 
			( result == BTL_R || result == BTL_F )
			)
			return 0;
	
	case BTL_IGNORE_RF_EXCEPT_MAGIC:
		if( AttackType != BTLCAP_MAGIC && 
			( result == BTL_R || result == BTL_F )
			)
			return 0;
	}


	// now, look at the results
	
	switch(result)
	{
		case BTL_F:	// attacking unit falls back, we apply 2 dead
			// missile fallback ignored
			if( AttackType != BTLCAP_MISSILE )
			{
				points = -2;
				pAttacker->fBtlCap.FallBackCount++;
				// allow for multipul fall back results before we comply

				if( pAttacker->fBtlCap.FallBackCount++ >= 4 )
				{
					pAttacker->fBtlCap.FallBackCount = 0;
					SendFallBack ( 
						pAttacker, 
						pAttacker->fBtlCap.TargetRow, 
						pAttacker->fBtlCap.TargetCol);
					pAttacker->fBtlCap.mfFallBack(pAttacker->hThis);
				}
			}
			break;

		case BTL_H:	// defending unit takes a hit, we apply damage
			points = 4;
			break;

		case BTL_R:	// defending unit routed, we apply damage and runs away
			points = 4;
			pDefender->fBtlCap.FallBackCount = 0;
			pDefender->fBtlCap.mfRout(pDefender->hThis,TRUE, TRUE);
			break;

		case BTL_D:	// defending unit takes heavy damage
			points = 7;
			break;

		case BTL_N:	// no-op
			break;
	}

	if(points > 0)
	{
		// extra damage for standing missile attacks
		if( AttackType == BTLCAP_MISSILE)
		{
			points = points * 3 / 2;	// missiles deals extra damage
				
			// if damage dealt to enemy by missile attack and
			// they were engaged with my guys, but not me, deal some 
			// damage to my guys
			if( pDefender->Status == CAvatar::AI_ATTACK
				&& pDefender->hEnemy != fERROR
				&& pDefender->hEnemy != hAttacker
				)
			{
				CAvatar *pAvatar = (CAvatar *)BLKPTR(pDefender->hEnemy);
				if(pDefender->hPlayerStats != fERROR)
				{
					DumbAutoLockPtr< PLAYER_STATS const> const pPlayerStats (pDefender->hPlayerStats);
					if( 0 == pPlayerStats->StatsMod.mfGetMod(STATSMOD::DAMAGE_RESIST_MOD))
					{
						pAvatar->fBtlCap.Shot = BTL_KILL_DELAY;	// frame delay to deaths
						pAvatar->fBtlCap.HitCount += points;
						pAvatar->KillType = NO_THING;
						SendResults ( pAvatar, points, ARROWS );
					}
				}
				else
				{
					pAvatar->fBtlCap.Shot = BTL_KILL_DELAY;	// frame delay to deaths
					pAvatar->fBtlCap.HitCount += points;
					pAvatar->KillType = NO_THING;
					SendResults ( pAvatar, points, ARROWS );
				}
			}
		}
		
		if(pDefender->hPlayerStats != fERROR)
		{
			DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(pDefender->hPlayerStats);
			if( 0 == pPlayerStats->StatsMod.mfGetMod(STATSMOD::DAMAGE_RESIST_MOD))
			{
				pDefender->fBtlCap.Shot = BTL_KILL_DELAY;	// frame delay to deaths
				pDefender->fBtlCap.HitCount += points;
				pDefender->KillType = NO_THING;
				SendResults ( pDefender, points, NO_THING );
			}
		}
		else
		{
			pDefender->fBtlCap.Shot = BTL_KILL_DELAY;	// frame delay to deaths
			pDefender->fBtlCap.HitCount += points;
			pDefender->KillType = NO_THING;
			SendResults ( pDefender, points, NO_THING );
		}
	}
	else
	if(points < 0)
	{
		if(pDefender->hPlayerStats != fERROR)
		{
			DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(pDefender->hPlayerStats);
			if( 0 == pPlayerStats->StatsMod.mfGetMod(STATSMOD::DAMAGE_RESIST_MOD))
			{
				points = abs(points);
				pAttacker->fBtlCap.Shot = BTL_KILL_DELAY;	// frame delay to deaths
				pAttacker->fBtlCap.HitCount += points;
				pAttacker->KillType = NO_THING;
				SendResults ( pAttacker, points, NO_THING );
			}
		}
	}
	
	return points;	
}

/* ========================================================================
   Function    - ResolveMissiles
   Description - deal with missile damage
   Returns     - 
   ======================================================================== */
SHORT ResolveMissiles(CAvatar *pDefender)
{
	
	// if damage dealt to enemy by missile attack 
	// deal some damage to my team
	if( pDefender->Status == CAvatar::AI_ATTACK
		&& pDefender->hEnemy != fERROR
		)
	{
		CAvatar *pEnemy = (CAvatar *)BLKPTR(pDefender->hEnemy);
		
		// if engaged with me, do no damage to myself
		if(pEnemy->hEnemy != pDefender->hThis)
		{
			if(pEnemy->hPlayerStats != fERROR)
			{
				DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(pEnemy->hPlayerStats);
				if( 0 == pPlayerStats->StatsMod.mfGetMod(STATSMOD::DAMAGE_RESIST_MOD))
				{
					pEnemy->fBtlCap.Shot = 1;	// frame delay to deaths
					pEnemy->fBtlCap.HitCount += pDefender->fBtlCap.HitCount;
					pEnemy->KillType = NO_THING;
					SendResults ( pEnemy, pDefender->fBtlCap.HitCount, ARROWS );
				}
			}
			else
			{
				pEnemy->fBtlCap.Shot = 1;	// frame delay to deaths
				pEnemy->fBtlCap.HitCount += pDefender->fBtlCap.HitCount;
				pEnemy->KillType = NO_THING;
				SendResults ( pEnemy, pDefender->fBtlCap.HitCount, ARROWS );
			}
		}
	}
	
	if(pDefender->fBtlCap.HitCount > 0)
	{
		SendResults ( pDefender, pDefender->fBtlCap.HitCount, ARROWS );
		while(pDefender->fBtlCap.HitCount > 0)
			pDefender->fBtlCap.mfKillTroop(pDefender,NO_THING);
			
		AddSndObj( SND_MALE_GROUP_DIE1, SND_MALE_GROUP_DIE_TOTAL, VOLUME_EIGHTY);
	}

	return 0;
}

/* ========================================================================
   Function    - DecodeBattleMagic
   Description - Use global set in Inventory Menu to resolve this casting
   Returns     - battle result (see battle.h)
   ======================================================================== */
SHORT DecodeBattleMagic( CAvatar * pAvatar )
{
	CAvatar *pEnemy;
	
	// remember global value
	SHORT		retVal = pAvatar->fBtlCap.fBtlMagicResult;
	
	if(pAvatar->hEnemy != fERROR)
	{
		// this is where we do the magic effects
		switch(pAvatar->fBtlCap.fBtlMagicType)
		{
		case BTLCAP_MAGIC_NO_ATTACK:
			pEnemy = (CAvatar *)BLKPTR(pAvatar->hEnemy);
			// delay the enemy's next decision by 5 rounds
			pEnemy->fBtlCap.ActionCount = 
					5 * BattleTick + (BtlActionRate * SpeedLevel);
			break;
		case BTLCAP_MAGIC_MISSILE :
			pAvatar->mfCreateFireBall( GetNewId(), MAGIC_MISSILE_1, pAvatar->fBtlCap.fBtlMagicResult);
			break;
		case BTLCAP_MAGIC_FIRE:
			pAvatar->mfCreateFireBall( GetNewId(), FIREBALL_1, pAvatar->fBtlCap.fBtlMagicResult);
			break;
		case BTLCAP_MAGIC_PLASMA:
			pAvatar->mfCreateFireBall( GetNewId(), PLASMA_BALL_1, pAvatar->fBtlCap.fBtlMagicResult);
			break;
		case BTLCAP_MAGIC_LIGHTNING:
			pAvatar->mfCreateFireBall( GetNewId(), LIGHTNING_1, pAvatar->fBtlCap.fBtlMagicResult);
			break;
		case BTLCAP_MAGIC_DESOLVE:
			pAvatar->mfCreateFireBall( GetNewId(), VAPORIZE_1, pAvatar->fBtlCap.fBtlMagicResult);
			break;
		}
		
		BattleExpPoints( pAvatar->UnitIndex, BTL_EXP_CAST_MAGIC, 0, FALSE );
	}
	else
		retVal = BTL_N;	// no-op
		
	// clean up globals
	pAvatar->fBtlCap.fBtlMagicResult = BTL_N;
	pAvatar->fBtlCap.fBtlMagicType = -1;
	
	return retVal;
}

/* ========================================================================
   Function    - ResolveBattleMagic
   Description - figure out the results of an magic attack
   Returns     - hit points delt, positive is defender, negative is attacker
   ======================================================================== */
SHORT ResolveBattleMagic(CAvatar *pDefender)
{
	SHORT	DefenderClass, DefenderValue;
	SHORT	result;
	SHORT	points = 0;
	SHORT	Bonus;
	SHORT	Special;
		
	fBattleStarted = TRUE;	// be sure we know fighting has started
	
	result = pDefender->fDamage;
	
	// look at KillType for protection stuff
	//  e.g. protection from fire against fire balls
	if(pDefender->hPlayerStats != fERROR)
	{
		DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(pDefender->hPlayerStats);
		
		// first general magic resistance
		Bonus = pPlayerStats->StatsMod.mfGetMod(STATSMOD::MAGIC_RESIST_MOD);
		if( Bonus )	
		{
			if(random(Bonus + 1))
				result = BTL_N;
		}
		
		if( pDefender->KillType != -1 )
		{
			if( GAME_TTYPE::mfIsHeatSource (pDefender->KillType)
				&& pPlayerStats->StatsMod.mfGetMod(STATSMOD::FIRE_RESIST_MOD)
				)	
				result = BTL_N;
			else
			if( GAME_TTYPE::mfIsCold (pDefender->KillType)
				&& pPlayerStats->StatsMod.mfGetMod(STATSMOD::COLD_RESIST_MOD)
				)	
				result = BTL_N;
			else
			if( GAME_TTYPE::mfIsEvil (pDefender->KillType)
				&& pPlayerStats->StatsMod.mfGetMod(STATSMOD::EVIL_RESIST_MOD)
				)	
				result = BTL_N;
		}
	}
	
	Bonus = GetBattleBonus(pDefender->fBtlCap.OfficerIcon);
	
	// check for turn undead spell on the undead
	// note that only skeletons are undead and only skeletons have RF_EXCEPT...
	if (fBtlTurnUndead && Bonus != BTL_STR_IGNORE_RF_EXCEPT_MAGIC)
	{
		result = BTL_N;
		fBtlTurnUndead = FALSE;
	}
	
	switch(result)
	{
	case BTL_H:
		points = 4;
		break;

	case BTL_F:
		points = 4;
		if( Bonus == BTL_IGNORE_RF_BY_MAGIC)
			break;
		pDefender->fBtlCap.FallBackCount = 0;
		SendFallBack( 
			pDefender,
			pDefender->fBtlCap.TargetRow,
			pDefender->fBtlCap.TargetCol);
		pDefender->fBtlCap.mfFallBack(pDefender->hThis);
		break;

	case BTL_R:
		points = 6;
		if( Bonus == BTL_IGNORE_RF_BY_MAGIC)
			break;
		pDefender->fBtlCap.FallBackCount = 0;
		pDefender->fBtlCap.mfRout(pDefender->hThis,TRUE, TRUE);
		break;

	case BTL_D:
		points = 10;
		break;

	case BTL_N:	// no-op
		break;
	}
	
	// if any damage to be delt, do so
	if(points)
	{
		// check for resistance to all damage
		if(pDefender->hPlayerStats != fERROR)
		{
			DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(pDefender->hPlayerStats);
			
			if( 0 == pPlayerStats->StatsMod.mfGetMod(STATSMOD::DAMAGE_RESIST_MOD))
			{
				// no resistance, take the damage
				pDefender->fBtlCap.HitCount += points;
				//GEH SendResults ( pDefender, points, pDefender->KillType );
			}
		}
		else
		{
			// take the damage
			pDefender->fBtlCap.HitCount += points;
			//GEH SendResults ( pDefender, points, pDefender->KillType );
		}
	
		// if troops were engaged, deal damage to other group
		if( pDefender->Status == CAvatar::AI_ATTACK )
		{
			if (pDefender->hEnemy != fERROR)
			{
				CAvatar *pAttacker = (CAvatar *)BLKPTR(pDefender->hEnemy);
				// check for resistance to all damage
				if(pAttacker->hPlayerStats != fERROR)
				{
					DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(pAttacker->hPlayerStats);
					
					if( 0 != pPlayerStats->StatsMod.mfGetMod(STATSMOD::DAMAGE_RESIST_MOD))
					{
						pAttacker->fDamage = result;
						pAttacker->KillType = pDefender->KillType;
					}
				}
				else
				{
					pAttacker->fDamage = result;
					pAttacker->KillType = pDefender->KillType;
				}
			}
		}
	}
	
	return points;	

}

/* ========================================================================
   Function	   - mfLeftButton
   Description - function to handle a left click of the mouse
   Returns	   - void
   ======================================================================== */
void BATTLE_CAPTAIN_DATA::mfLeftButton(CAvatar *)
{
	/* nothing for now */
}

/* ========================================================================
   Function	   - mfRightButton
   Description - function to handle a Right click of the mouse
   Returns	   - void
   ======================================================================== */
void BATTLE_CAPTAIN_DATA::mfRightButton(CAvatar *pAvatar)
{
	sBattleMode = BTLUI_MODE_UNITINFO;
	hUnitInfo = pAvatar->hThis;
}

/* ========================================================================
   Function    - mfShowInventory
   Description - 
   Returns     - 
   ======================================================================== */
void BATTLE_CAPTAIN_DATA::mfShowInventory(LONG , LONG /* AvatarIndex */)
{
	// ShowMenu(D_AVATAR_INVENTORY);
}

/* ========================================================================
   Function    - mfMelee
   Description - stand and defend
   Returns     - 
   ======================================================================== */
void BATTLE_CAPTAIN_DATA::mfMelee(CAvatar *pAvatar)
{
	LONG	i;
	SHORT	hdlEnemy;
	SHORT	*pList;
	
	if (pAvatar->fBtlCap.Leader.hdlSlotList == fERROR)
		return;
		
	pList = (SHORT *)BLKPTR(pAvatar->fBtlCap.Leader.hdlSlotList);
	if (pList == NULL)
		return;
	for( i = 0; i < (pAvatar->fBtlCap.TroopRows*pAvatar->fBtlCap.TroopCols); i++ )
	{
		CAvatar *pTroop;
		CAvatar *pEnemy;
		
		if(pList[i] != fERROR)
		{
			pTroop = (CAvatar *) BLKPTR(pList[i]);
			// if I find a guy not attacking, try to find someone for him
			if(pTroop->Status != CAvatar::AI_ATTACK)
			{
				hdlEnemy = pTroop->fFollowBtlCap.FindUnengaged( pTroop, pAvatar->hEnemy, pAvatar->fBtlCap.TargetType );
				if(hdlEnemy != fERROR)
				{
					pTroop->Engaged++;
					pTroop->hEnemy = hdlEnemy;
					pTroop->Status = CAvatar::AI_ATTACK;
					
					pEnemy = (CAvatar *) BLKPTR(hdlEnemy);
					pEnemy->Engaged++;
					pEnemy->hEnemy = pList[i];
					pEnemy->Status = CAvatar::AI_ATTACK;
				}
			}
		}
	}
}

/* ========================================================================
   Function    - mfReform
   Description - stand and defend
   Returns     - 
   ======================================================================== */
void BATTLE_CAPTAIN_DATA::mfReform(CAvatar *pAvatar)
{
	SHORT	*pList;
	SHORT	i;
	
	if (pAvatar->fBtlCap.Leader.hdlSlotList == fERROR)
		return;
		
	pList = (SHORT *)BLKPTR(pAvatar->fBtlCap.Leader.hdlSlotList);
	if (pList == NULL)
		return;
	for(i=0; i<pAvatar->fBtlCap.Leader.sMaxFollowers; i++)
	{
		if(pList[i] != fERROR)
		{
			CAvatar *pTroop = (CAvatar *) BLKPTR(pList[i]);
			if( pTroop->Status != CAvatar::AI_DEAD)
			{
				pTroop->Status = CAvatar::AI_MOVING;
			}
			// GWP If you are dead or not, you have no enemy.
			pTroop->Engaged = 0;
			pTroop->hEnemy = fERROR;
		}
	}
}

/* ========================================================================
   Function    - mfShoot
   Description - shoot a volly
   Returns     - 
   ======================================================================== */
void BATTLE_CAPTAIN_DATA::mfShoot(CAvatar *pAvatar)
{
	SHORT	*pList;
	SHORT	i;
	
	// OK to speak my lines again
	pAvatar->fBtlCap.HasSpoken = FALSE;
	
	switch(pAvatar->fBtlCap.Shoot)
	{
	case BTLCAP_MAGIC:
	case BTLCAP_MAGIC_FIRE:
	case BTLCAP_MAGIC_PLASMA:
	case BTLCAP_MAGIC_LIGHTNING:
	case BTLCAP_MAGIC_DESOLVE:
	case BTLCAP_MAGIC_MISSILE:
		DecodeBattleMagic(pAvatar);
		pAvatar->hEnemy = fERROR;				// done with the enemy for now
		pAvatar->Status = CAvatar::AI_SEARCH;	// will find him again if still alive
		break;
	case BTLCAP_MISSILE:
		// now, make my guys shot an arrow
		ResolveAttack( pAvatar->hEnemy, pAvatar->hThis, BTLCAP_MISSILE);
		if (pAvatar->fBtlCap.Leader.hdlSlotList == fERROR)
			return;
		pList = (SHORT *)BLKPTR(pAvatar->fBtlCap.Leader.hdlSlotList);
		if (pList == NULL)
			return;
		for(i=0; i<pAvatar->fBtlCap.Leader.sMaxFollowers; i++)
		{
			if(pList[i] != fERROR)
			{
				CAvatar *pTroop = (CAvatar *) BLKPTR(pList[i]);
				if( pTroop->Status != CAvatar::AI_DEAD)
				{
					pTroop->fFollowBtlCap.Shoot = pAvatar->fBtlCap.Shoot;
				}
			}
		}
		AddSndObj(SND_ARCHERS_VOLLEY1, SND_ARCHERS_VOLLEY_TOTAL, VOLUME_EIGHTY);
		break;
	}
}

/* ========================================================================
   Function    - mfTryToEngageEnemyBtlCap
   Description - have my guys attack his guys
   Returns     - 
   ======================================================================== */
void BATTLE_CAPTAIN_DATA::mfTryToEngageEnemyBtlCap(CAvatar *pAvatar)
{
	SHORT	hAvatar;
	LONG	HisStatus;
	SHORT	TestRow;
	CAvatar *pOtherAvatar;
	
	// if in the reserves, bail
	if(pAvatar->fBtlCap.OnBtlField == FALSE)
		return;
		
	hAvatar = GetWhosWhere(
		pAvatar->fBtlCap.Row,
		pAvatar->fBtlCap.Column,
		!(pAvatar->mfIsHomeTeam())
		);
	
	if(hAvatar != fERROR)
	{
		pOtherAvatar = (CAvatar *) BLKPTR((SHORT) hAvatar);
		HisStatus = pOtherAvatar->Status;
		if(HisStatus == CAvatar::AI_SEARCH)
		{
			CHAR	temp1[80];
			CHAR	temp2[80];
			CHAR	buffer[80];
			
			/* save my info */
			pAvatar->hEnemy = hAvatar;
			pAvatar->Status = CAvatar::AI_ATTACK;
			pAvatar->Engaged = 1;
			pAvatar->fBtlCap.TargetType = pOtherAvatar->fBtlCap.TroopType;
			pAvatar->fBtlCap.mfMelee(pAvatar);
			
			/* make him follow suit */
			pOtherAvatar->hEnemy = pAvatar->hThis;
			pOtherAvatar->Status = CAvatar::AI_ATTACK;
			pOtherAvatar->Engaged = 1;
			
			// get format string
			strcpy(temp1,STRMGR_GetStr(STR_CHAT_TROOPS_ENGAGE)); 
			strcpy(temp2,STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon]));
			
			// fill in data
			sprintf(buffer, 
				temp1, 
				temp2,
				pOtherAvatar->fBtlCap.Row,
				pOtherAvatar->fBtlCap.Column,
				STRMGR_GetStr(gsUnitTitle[pOtherAvatar->fBtlCap.OfficerIcon])
				);
			
			#ifdef BTL_CHAT
			if(pAvatar->mfIsHomeTeam())
				ChatLineAdd(buffer, LT_BLUE);
			else
				ChatLineAdd(buffer, RED);
			#endif
			
			//GEH TUTORIAL
			if (fTutorialFirstBattle && 
				fDoTutEngage )
			{
				fDoTutEngage = FALSE;
				//SystemHelp("TUTORIAL TEST", "Once engaged, the fight insues unless you click on them and fallback to an open square", -1, NULL, 0);
				DescribeBtlEngagement();
			}
		}
	}
}	

/* ========================================================================
   Function    - mfRunEnemyBtlCapAI
   Description - Move an enemy btl cap around the battle grid
   Returns     - 
   ======================================================================== */
void BATTLE_CAPTAIN_DATA::mfRunEnemyBtlCapAI(CAvatar *pAvatar)
{
	SHORT i;
	CHAR temp[80];
	CHAR buffer[80];
	/* if not already engaged in battle, move about */
	if(pAvatar->Engaged == 0)
	{ 
		SHORT MyMorale = GetBattleMorale(pAvatar->fBtlCap.OfficerIcon);
		// if low morale and weaker then 33% or med morale and 25%, just rout back to the reserves
		// high morale fights to the death
		if( ( MyMorale == 1 && pAvatar->fBtlCap.HitPoints <= (pAvatar->fBtlCap.Healthy/3) ) || 
		    ( MyMorale == 2 && pAvatar->fBtlCap.HitPoints <= (pAvatar->fBtlCap.Healthy/4) )
		  )
		{ 
			if( pAvatar->fBtlCap.OnBtlField )
			{
				pAvatar->fBtlCap.mfRout(pAvatar->hThis, FALSE, FALSE);
			}
		}
		else
		if( pAvatar->fBtlCap.ActionCount < BattleTick)
		{
			SHORT r,c,best_r=0, best_c=0;
			SHORT best_case=0;
			
			fUpdatePanels = TRUE;
			
			pAvatar->fBtlCap.ActionCount = 
				BattleTick + (BtlActionRate * SpeedLevel);
			
			// first, if I'm in the enemy reserves, try to find a slot
			if(pAvatar->fBtlCap.Row == GRID_MAX_ROWS)
			{
				best_r = 0;
				if(GetWhosWhere(2,2,WW_AWAY) == fERROR)
				{
					best_r = 2;
					best_c = 2;
				}
				else
				if(GetWhosWhere(2,1,WW_AWAY) == fERROR)
				{
					best_r = 2;
					best_c = 1;
				}
				else
				if(GetWhosWhere(2,3,WW_AWAY) == fERROR)
				{
					best_r = 2;
					best_c = 3;
				}
				else
				if(GetWhosWhere(2,0,WW_AWAY) == fERROR)
				{
					best_r = 2;
					best_c = 0;
				}
				else
				if(GetWhosWhere(2,4,WW_AWAY) == fERROR)
				{
					best_r = 2;
					best_c = 4;
				}
				
				if(best_r != 0)
				{
					// mark new grid slot with my name
					SetWhosWhere(best_r,best_c,WW_AWAY,pAvatar->hThis);
					for(i=0;i<MAX_RESERVES;i++)
					{
						if (Reserves[0][i] == (SHORT)pAvatar->hThis)
							Reserves[0][i] = fERROR;
					}
					// update the avatar
					pAvatar->Status = CAvatar::AI_END_PAUSE;
					pAvatar->fTimeLastMoved = SCENE_MGR::gTick;
					pAvatar->fBtlCap.TargetX = BattleGridAway[best_r][best_c].x; 
					pAvatar->fBtlCap.TargetY = BattleGridAway[best_r][best_c].y;
					pAvatar->fBtlCap.Row = best_r;
					pAvatar->fBtlCap.Column = best_c;
					pAvatar->fBtlCap.OnBtlField = TRUE;
					return;
				}
			}
			else
			if(!random(DifficultyLevel) 
				&& pAvatar->fBtlCap.mfAutoMagic(pAvatar,r,c))
			{
				// fill in data
				//strcpy(temp,STRMGR_GetStr(STR_CHAT_MAGIC_READY)); 
				//sprintf(buffer, temp, 
					//STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon]),
					//pAvatar->fBtlCap.Row,
					//pAvatar->fBtlCap.Column
					//);
				//ChatLineAdd(buffer, RED);
			}
			else
			if(!random(DifficultyLevel) 
				&& pAvatar->fBtlCap.mfAutoShoot(pAvatar,r,c))
			{
				// fill in data
				//strcpy(temp,STRMGR_GetStr(STR_CHAT_ARCHERS_READY)); 
				//sprintf(buffer, temp, 
					//STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon]),
					//pAvatar->fBtlCap.Row,
					//pAvatar->fBtlCap.Column
					//);
				//ChatLineAdd(buffer, RED);
			}
			else
			if(!random(DifficultyLevel+1))
			{
				// do nothing this round
			}
			else
			if(pAvatar->fBtlCap.mfAutoEngage(pAvatar,r,c))
			{
			}
			else
			if(pAvatar->fBtlCap.mfAutoMove(pAvatar,r,c))
			{
			}
			else
			{
				// just move ahead
				best_r = random(2);
				
				// if already in first row, back up
				if(pAvatar->fBtlCap.Row < 1)
					best_r *= -1;
					
				// or maybe sideways
				if(best_r == 0)
					best_c = (SHORT)(random(3) - 1);
				
				if(best_r == -1)
				{
					pAvatar->fBtlCap.mfMoveAhead(0, pAvatar->hThis);
				}
				else
				if(best_r == 1)
				{
					pAvatar->fBtlCap.mfMoveBack(0, pAvatar->hThis);
				}
				else
				if(best_c == 1)
				{
					pAvatar->fBtlCap.mfMoveRight(0, pAvatar->hThis);
				}
				else
				if(best_c == -1)
				{
					pAvatar->fBtlCap.mfMoveLeft(0, pAvatar->hThis);
				}
			}
		}
	}
}

/* ========================================================================
   Function    - mfKillTroop
   Description - kill off one of my guys
   Returns     - TRUE if someone was killed off
   ======================================================================== */
BOOL BATTLE_CAPTAIN_DATA::mfKillTroop(CAvatar *pAvatar, THINGTYPE KillType )
{
	SHORT	i,j;
	SHORT	DoMore = 50;
	SHORT	*pList;
	CAvatar *pTroop;
	
	if (pAvatar->fBtlCap.Leader.hdlSlotList == fERROR)
	{
		pAvatar->fBtlCap.HitCount = 0; // done with any pending death as well
		return FALSE;
	}
		
	pList = (SHORT *)BLKPTR(pAvatar->fBtlCap.Leader.hdlSlotList);
	if (pList == NULL)
	{
		pAvatar->fBtlCap.HitCount = 0; // done with any pending death as well
		return FALSE;
	}
	
	// check how many guys in the list, and set hitpoints to match
	i = 0;
	for(j=0; j<pAvatar->fBtlCap.Leader.sMaxFollowers; j++)
	{
		// count how many guys I have left
		if(pList[j] != fERROR)
			i++;
	}
	pAvatar->fBtlCap.HitPoints = i;
	
	// no one is left to kill (darn!)
	if (pAvatar->fBtlCap.HitPoints <= 0 ||
		pList[0] == fERROR)
	{
		pAvatar->fBtlCap.HitCount = 0; // done with any pending death as well
		return FALSE;
	}
		
	// select a random victim
	i = random(pAvatar->fBtlCap.HitPoints);
	
	if (pList[i] == fERROR)
		return FALSE;
		
	pTroop = (CAvatar *) BLKPTR(pList[i]);
	if (pTroop == NULL)
		return FALSE;
	
	// disengage from this guy
	if(pTroop->hEnemy != fERROR)
	{
		CAvatar *pMyEnemy = (CAvatar *) BLKPTR(pTroop->hEnemy);
		
		if (pMyEnemy->fFollowBtlCap.hLeader != fERROR)
		{
			CAvatar *pEnemyLeader = 
				(CAvatar *) BLKPTR((SHORT) pMyEnemy->fFollowBtlCap.hLeader);
			
			// GWP 5/18/97
			// For all the troops attacking me, dis-enage me from them.
			if (pEnemyLeader->fBtlCap.Leader.hdlSlotList != fERROR)
			{
				SHORT *pOtherList = (SHORT *)BLKPTR(pEnemyLeader->fBtlCap.Leader.hdlSlotList);
				LONG k;
				for (k = 0; k < pEnemyLeader->fBtlCap.Leader.sMaxFollowers; k++)
				{
					if (pOtherList[k] != fERROR)
					{
						CAvatar * pOtherTroop = (CAvatar *)BLKPTR(pOtherList[k]);
						pOtherTroop->hEnemy = fERROR;
					}
				}
			}
		}
		pTroop->hEnemy = fERROR;
		pTroop->Engaged = 0;
	}

	// if my unit hit by some magic (not arrows), do special death
	if(KillType != NO_THING && KillType != ARROWS)
	{
		pTroop->SetAIFuncIndex( CAvatar::AI_FUNC_FIREBALL );
		pTroop->mfPlayAnimationOnce(ATTACK1SEQ);
		pTroop->mfSetType( KillType);
	}
	else
	{
		pTroop->mfPlayAnimationOnce(EXPIRESEQ);
	}
		
	pTroop->Status = CAvatar::AI_DEAD;
	pTroop->LeftButtonFn = 0;
	pTroop->RightButtonFn = 0;
	
	// now clean up and resort the list
	for(j=0; j<pAvatar->fBtlCap.Leader.sMaxFollowers; j++)
	{
		// break when we find the first empty slot
		if(pList[j] == fERROR)
			break;
	}
	// copy this list guy into the slot of the dead guy
	pList[i] = pList[j-1];
	// empty the last slot
	pList[j-1] = fERROR;
	
	// upkeep on the avatar
	pAvatar->fBtlCap.HitPoints--;
	pAvatar->fBtlCap.HitCount--;
	
	fUpdatePanels = TRUE;

	return TRUE;
}	

/* ========================================================================
   Function    - mfEnemyPriority
   Description - Enemy finds the best troops to attack 
   Returns     - priority value
   ======================================================================== */
SHORT BATTLE_CAPTAIN_DATA::mfEnemyPriority(
	CAvatar *pAvatar, 
	SHORT checkRow, 
	SHORT checkCol
	)
{
	SHORT	priority = 0;
	
	checkRow = pAvatar->fBtlCap.Row + checkRow;
	checkCol = pAvatar->fBtlCap.Column + checkCol;
	
	if ((checkRow >= 0 && checkRow < GRID_MAX_ROWS) &&
		(checkCol >= 0 && checkCol < GRID_MAX_COLS) )
	{
		if(GetWhosWhere(checkRow,checkCol,WW_HOME) != fERROR)
		{
			CAvatar *pEnemy = (CAvatar *) BLKPTR(GetWhosWhere(checkRow,checkCol,WW_HOME));
			if(pEnemy->Engaged == 0)
			{
				SHORT MyBonus;
				
				// generate a priority number for this enemy
				priority = (SHORT)(
					((BTL_STR_REGENT == GetBattleBonus(pEnemy->fBtlCap.OfficerIcon))?7:0) +
					(pAvatar->fBtlCap.SpellCount * 2 ) +
					GetBattleMissile(pEnemy->fBtlCap.OfficerIcon) +
					GetBattleCharge(pEnemy->fBtlCap.OfficerIcon) +
					GetBattleMelee(pEnemy->fBtlCap.OfficerIcon) -
					((GetBattleDefense(pEnemy->fBtlCap.OfficerIcon) - GetBattleDefense(pAvatar->fBtlCap.OfficerIcon)) * 3)
					);
			
				// if a fully defensive
				if( priority <= 0 )
					priority = 1;
					
				// if you are a weakend unit, go for the exposed neck
				// subtract a negitive to get a positive
				priority -= pEnemy->fBtlCap.HitPoints - pEnemy->fBtlCap.Healthy;
				
				// if I don't have missiles and he does, bonus him
				if( 0 == GetBattleMissile(pAvatar->fBtlCap.OfficerIcon) &&
					0 != GetBattleMissile(pEnemy->fBtlCap.OfficerIcon) 
				)
					priority += GetBattleMissile(pEnemy->fBtlCap.OfficerIcon);
				
				MyBonus = GetBattleBonus(pAvatar->fBtlCap.OfficerIcon);
				
				// if I have a bonus aginst mounted units
				if( ( MyBonus == BTL_STR_PIKEMEN ||
				      MyBonus == BTL_STR_MISSILE_VS_CAVALRY
				    )
				    &&
					( pEnemy->fBtlCap.OfficerIcon == A_CAV_UNIT_ICON ||
					  pEnemy->fBtlCap.OfficerIcon == A_KNT_UNIT_ICON ||
					  pEnemy->fBtlCap.OfficerIcon == G_CAV_UNIT_ICON
					)
				  )
				{
					priority += 10;
				}
				
				// if I have a bonus aginst pikemen or irreg units
				if( ( MyBonus == BTL_STR_VS_PIKES_IRREG &&
				      MyBonus == BTL_STR_MELEE_VS_PIKES_IRREG
					)
					&&
					( pEnemy->fBtlCap.OfficerIcon == A_PIK_UNIT_ICON ||
					  pEnemy->fBtlCap.OfficerIcon == A_IRR_UNIT_ICON ||
					  pEnemy->fBtlCap.OfficerIcon == M_IRR_UNIT_ICON ||
					  pEnemy->fBtlCap.OfficerIcon == M_PIK_UNIT_ICON ||
					  pEnemy->fBtlCap.OfficerIcon == N_IRR_UNIT_ICON
					)
				  )
				{
					priority += 10;
				}
			}
		}
	}

	return (priority);
}	

/* ========================================================================
   Function    - mfAutoMove
   Description - find the enemy best to move toward
   Returns     - TRUE I moved, FALSE no best case
   ======================================================================== */
BOOL BATTLE_CAPTAIN_DATA::mfAutoMove(
	CAvatar *pAvatar, 
	SHORT , 
	SHORT 
	)
{
	BOOL	Done = FALSE;
	SHORT	i,r,c,best_r=0, best_c=0;
	SHORT	priority, best_case=0;
	SHORT	checkRow =  pAvatar->fBtlCap.Row;
	SHORT	checkCol =  pAvatar->fBtlCap.Column;
	POINT	ScanList[21] = {
			// the grid I'm in
			{ 0, 0},
			// first grids closest to me by r,c deltas
			{-1,1 }, { 1,1 }, {-1,-1}, { 1,-1},
			// second level grids closest to me by r,c deltas
			{ 0, 2}, {-1, 2}, { 1, 2}, {-2, 2}, { 2, 2},
			{ 2, 1}, {-2, 1}, 
			{ 2, 0}, {-2, 0}, 
			{ 2,-1}, {-2,-1}, 
			{ 0, -2}, {-1, -2}, { 1, -2}, {-2, -2}, { 2, -2},
			};
	
	// look at all 4 grids around me
	for(i=0;i<5;i++)
	{
		// in front
		c = ScanList[i].x;
		r = ScanList[i].y;
		priority = pAvatar->fBtlCap.mfEnemyPriority(pAvatar,r,c);
		
		// only concider this move if there is a path is open
		if( priority )
		{
			if ((checkRow+r >= 0 && checkRow+r < GRID_MAX_ROWS) &&
				(checkCol+c >= 0 && checkCol+c < GRID_MAX_COLS)
				)
			{
				if( GetWhosWhere(checkRow,checkCol+c,WW_AWAY) == fERROR &&
					GetWhosWhere(checkRow,checkCol+c,WW_HOME) == fERROR &&
					BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[checkRow][checkCol+c]
					)
				{
					if(priority > best_case)
					{
						best_case = priority;
						best_r = 0;
						best_c = c;
					}
				}
				else
				if( GetWhosWhere(checkRow+r,checkCol,WW_AWAY) == fERROR && 
					GetWhosWhere(checkRow+r,checkCol,WW_HOME) == fERROR &&
					BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[checkRow+r][checkCol]
					)
				{
					if(priority > best_case)
					{
						best_case = priority;
						best_r = r;
						best_c = 0;
					}
				}
				
			}
		}
	}				

	if(best_r || best_c) // found someone
	{
		// try to move toward them
		if(best_c == 1)
		{
			pAvatar->fBtlCap.mfMoveRight(0, pAvatar->hThis);
			Done = TRUE;
		}
		else
		if(best_c == -1)
		{
			pAvatar->fBtlCap.mfMoveLeft(0, pAvatar->hThis);
			Done = TRUE;
		}
		else
		if(best_r == 1)
		{
			pAvatar->fBtlCap.mfMoveAhead(0, pAvatar->hThis);
			Done = TRUE;
		}
		else
		if(best_r == -1)
		{
			pAvatar->fBtlCap.mfMoveBack(0, pAvatar->hThis);
			Done = TRUE;
		}
	}
	else	// look futher out
	{
		for(i=5;i<21;i++)
		{
			// in front
			c = ScanList[i].x;
			r = ScanList[i].y;
			priority = pAvatar->fBtlCap.mfEnemyPriority(pAvatar,r,c);
			
			// only concider this move if there is a path is open
			if( priority )
			{
				SHORT	tr=0,tc=0;
				
				if(r) tr = (r>0)?1:-1;
				if(c) tc = (c>0)?1:-1;
					
				if ((checkRow+tr >= 0 && checkRow+tr < GRID_MAX_ROWS) &&
					(checkCol+tc >= 0 && checkCol+tc < GRID_MAX_COLS) )
				{
					if( GetWhosWhere(checkRow+tr,checkCol+tc,WW_AWAY) == fERROR &&
						GetWhosWhere(checkRow+tr,checkCol+tc,WW_HOME) == fERROR &&
						BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[checkRow+tr][checkCol+tc]
						)
					{
						
						if( GetWhosWhere(checkRow,checkCol+tc,WW_AWAY) == fERROR &&
							GetWhosWhere(checkRow,checkCol+tc,WW_HOME) == fERROR &&
							BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[checkRow][checkCol+tc]
							)
						{
							if(priority > best_case)
							{
								best_case = priority;
								best_r = r;
								best_c = c;
							}
						}
						else
						if( GetWhosWhere(checkRow+tr,checkCol,WW_AWAY) == fERROR && 
							GetWhosWhere(checkRow+tr,checkCol,WW_HOME) == fERROR &&
							BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[checkRow+tr][checkCol]
							)
						{
							if(priority > best_case)
							{
								best_case = priority;
								best_r = r;
								best_c = c;
							}
						}
					}
				}
			}
		}				

		if(best_r || best_c) // found someone
		{
			// try to move toward them
			if(best_r > 0)
			{
				pAvatar->fBtlCap.mfMoveAhead(0, pAvatar->hThis);
				Done = TRUE;
			}
			else
			if(best_r < 0)
			{
				pAvatar->fBtlCap.mfMoveBack(0, pAvatar->hThis);
				Done = TRUE;
			}
			
			if(best_c > 0)
			{
				pAvatar->fBtlCap.mfMoveRight(0, pAvatar->hThis);
				Done = TRUE;
			}
			else
			if(best_c < 0)
			{
				pAvatar->fBtlCap.mfMoveLeft(0, pAvatar->hThis);
				Done = TRUE;
			}
		}
	}
	
	return (Done);
}	

/* ========================================================================
   Function    - mfAutoEngage
   Description - find the enemy best to engage him
   Returns     - TRUE I moved, FALSE no best case
   ======================================================================== */
BOOL BATTLE_CAPTAIN_DATA::mfAutoEngage(
	CAvatar *pAvatar, 
	SHORT , 
	SHORT 
	)
{
	BOOL	Done = FALSE;
	SHORT	i,r,c,best_r=0, best_c=0;
	SHORT	priority, best_case=0;
	POINT	ScanList[5] = {
			// the grid I'm in
			{ 0, 0},
			// first grids closest to me by r,c deltas
			{ 0,1 }, { 1,0 }, {-1,0 }, { 0,-1}
			};
	
	// look at legal grids around me
	for(i=0;i<5;i++)
	{
		// in front
		r = ScanList[i].x;
		c = ScanList[i].y;
		priority = pAvatar->fBtlCap.mfEnemyPriority(pAvatar,r,c);
		if(priority > best_case)
		{
			best_case = priority;
			best_r = r;
			best_c = c;
		}
	}				

	if(best_r || best_c) // found someone
	{
		// try to move toward them
		if(best_c == 1)
		{
			pAvatar->fBtlCap.mfMoveRight(0, pAvatar->hThis);
			Done = TRUE;
		}
		else
		if(best_c == -1)
		{
			pAvatar->fBtlCap.mfMoveLeft(0, pAvatar->hThis);
			Done = TRUE;
		}
		else
		if(best_r == 1)
		{
			pAvatar->fBtlCap.mfMoveAhead(0, pAvatar->hThis);
			Done = TRUE;
		}
		else
		if(best_r == -1)
		{
			pAvatar->fBtlCap.mfMoveBack(0, pAvatar->hThis);
			Done = TRUE;
		}
	}
	
	return (Done);
}	

/* ========================================================================
   Function    - mfAutoMagic
   Description - find the enemy best to shoot at
   Returns     - TRUE I shot, FALSE no target
   ======================================================================== */
BOOL BATTLE_CAPTAIN_DATA::mfAutoMagic(
	CAvatar *pAvatar, 
	SHORT , 
	SHORT 
	)
{
	BOOL	Done = FALSE;
	SHORT	r,c,best_r=0, best_c=0;
	SHORT	priority, best_case=0;
	
	if (pAvatar->hPlayerStats == fERROR)	// GWP
		return FALSE;
	
	DumbAutoLockPtr<PLAYER_STATS const> const dhPlayerStats(pAvatar->hPlayerStats);
	
	//if (dhPlayerStats->mfIsMagicUser() || BattleMagicReady(pAvatar))
	if (BattleMagicReady(pAvatar))
	{
		// only from a standing position
		if(pAvatar->Status != CAvatar::AI_SEARCH)
			return FALSE;
			
		// no more magic left
		// if(pAvatar->fBtlCap.SpellCount == 0)
		//	return FALSE;
			
		// only if it has been long enough
		if(pAvatar->fBtlCap.MagicCount > BattleTick)
			return FALSE;
		
		// look at the five legal shots
		
		// first, my grid
		r=0;c=0;
		priority = pAvatar->fBtlCap.mfEnemyPriority(pAvatar,r,c);
		if(priority > best_case)
		{
			best_case = priority;
			best_r = pAvatar->fBtlCap.Row + r;
			best_c = pAvatar->fBtlCap.Column + c;
		}				
	
		// now grid left
		r=0;c=-1;
		priority = pAvatar->fBtlCap.mfEnemyPriority(pAvatar,r,c);
		if(priority > best_case)
		{
			best_case = priority;
			best_r = pAvatar->fBtlCap.Row + r;
			best_c = pAvatar->fBtlCap.Column + c;
		}				
	
		// now grid right
		r=0;c=1;
		priority = pAvatar->fBtlCap.mfEnemyPriority(pAvatar,r,c);
		if(priority > best_case)
		{
			best_case = priority;
			best_r = pAvatar->fBtlCap.Row + r;
			best_c = pAvatar->fBtlCap.Column + c;
		}				
	
		// now grid north
		r=-1;c=0;
		priority = pAvatar->fBtlCap.mfEnemyPriority(pAvatar,r,c);
		if(priority > best_case)
		{
			best_case = priority;
			best_r = pAvatar->fBtlCap.Row + r;
			best_c = pAvatar->fBtlCap.Column + c;
		}				
	
		// now grid south
		r=1;c=0;
		priority = pAvatar->fBtlCap.mfEnemyPriority(pAvatar,r,c);
		if(priority > best_case)
		{
			best_case = priority;
			best_r = pAvatar->fBtlCap.Row + r;
			best_c = pAvatar->fBtlCap.Column + c;
		}				
	
		if(best_case != 0)
		{
			SHORT				level;
			
			// declare my enemy
			pAvatar->hEnemy = GetWhosWhere(best_r,best_c,WW_HOME);
			
			if(pAvatar->hPlayerStats != fERROR)
			{
				DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(pAvatar->hPlayerStats);
				SHORT SpellClass = pPlayerStats->mfGetSpellCastClass(SPELL_INFO::WIZARD);
				if (SpellClass != CANT_CAST_SPELLS)
				{
					level = pPlayerStats->mfGetLevel(SpellClass);
					level /= 3;
				}
				else
				{
					SpellClass = pPlayerStats->mfGetSpellCastClass(SPELL_INFO::PRIEST);
					if (SpellClass != CANT_CAST_SPELLS)
					{
						level = pPlayerStats->mfGetLevel(SpellClass);
						level -= 8;
						if (level > 0)
						{
							level /= 2;
						}
						else
						{
							// don't know what to do, so make him a wimp
							level = 1;
						}
					}
					else
					{
						// don't know what to do, so make him a wimp
						level = 1;
					}
				}
			}
			else
			{
				// don't know what to do, so make him a wimp
				level = 1;
			}
			
		 	switch(level)
			{
			case 0:
				// GWP Level zero are too wimpy to shoot.
				pAvatar->fBtlCap.SpellCount = 0;
				break;
			case 1:
				pAvatar->fBtlCap.Shoot = BTLCAP_MAGIC_MISSILE;
				pAvatar->mfCreateFireBall( GetNewId(), MAGIC_MISSILE_1, BTL_H);
				break;
			case 2:
			case 3:
				pAvatar->fBtlCap.Shoot = BTLCAP_MAGIC_FIRE;
				pAvatar->mfCreateFireBall( GetNewId(), FIREBALL_1, BTL_R);
				break;
			case 4:
			case 5:
				pAvatar->fBtlCap.Shoot = BTLCAP_MAGIC_LIGHTNING;
				pAvatar->mfCreateFireBall( GetNewId(), LIGHTNING_1, BTL_R);
				break;
				
			// 6 and above
			default:
				pAvatar->fBtlCap.Shoot = BTLCAP_MAGIC_DESOLVE;
				pAvatar->mfCreateFireBall( GetNewId(), VAPORIZE_1, BTL_D);
				break;
			}
			
			// GWP Didn't seem to be resetting the timer.
			pAvatar->fBtlCap.MagicCount = BattleTick + (3 * BtlActionRate);
			pAvatar->fBtlCap.SpellCount--;
			Done = TRUE;
		}
	}
	
	return (Done);
}

/* ========================================================================
   Function    - mfAutoShoot
   Description - find the home unit best to shoot at
   Returns     - TRUE I shot, FALSE no target
   ======================================================================== */
BOOL BATTLE_CAPTAIN_DATA::mfAutoShoot(
	CAvatar *pAvatar, 
	SHORT , 
	SHORT 
	)
{
	BOOL	Done = FALSE;
	SHORT	r=0,c=0,best_r=0, best_c=0;
	SHORT	priority, best_case=0;
	
	// only for people with missiles
	if(0 == GetBattleMissile(pAvatar->fBtlCap.OfficerIcon))
		return FALSE;
		
	// only from a standing position
	if(pAvatar->Status != CAvatar::AI_SEARCH)
		return FALSE;
		
	// only if it has been long enough
	// GEH
	//if(pAvatar->fBtlCap.MissileCount > BattleTick)
	//	return FALSE;
	
	// look at the five legal shots
	
	// first, my grid
	r=0;c=0;
	priority = pAvatar->fBtlCap.mfEnemyPriority(pAvatar,r,c);
	if(priority > best_case)
	{
		best_case = priority;
		best_r = pAvatar->fBtlCap.Row + r;
		best_c = pAvatar->fBtlCap.Column + c;
	}				

	// now grid left
	r=0;c=-1;
	priority = pAvatar->fBtlCap.mfEnemyPriority(pAvatar,r,c);
	if(priority > best_case)
	{
		best_case = priority;
		best_r = pAvatar->fBtlCap.Row + r;
		best_c = pAvatar->fBtlCap.Column + c;
	}				

	// now grid right
	r=0;c=1;
	priority = pAvatar->fBtlCap.mfEnemyPriority(pAvatar,r,c);
	if(priority > best_case)
	{
		best_case = priority;
		best_r = pAvatar->fBtlCap.Row + r;
		best_c = pAvatar->fBtlCap.Column + c;
	}				

	// now grid north
	r=-1;c=0;
	priority = pAvatar->fBtlCap.mfEnemyPriority(pAvatar,r,c);
	if(priority > best_case)
	{
		best_case = priority;
		best_r = pAvatar->fBtlCap.Row + r;
		best_c = pAvatar->fBtlCap.Column + c;
	}				

	// now grid south
	r=1;c=0;
	priority = pAvatar->fBtlCap.mfEnemyPriority(pAvatar,r,c);
	if(priority > best_case)
	{
		best_case = priority;
		best_r = pAvatar->fBtlCap.Row + r;
		best_c = pAvatar->fBtlCap.Column + c;
	}				

	if(best_case != 0)
	{
		pAvatar->hEnemy = GetWhosWhere(best_r,best_c,WW_HOME);
		// GWP Didn't seem to be resetting the timer.
		pAvatar->fBtlCap.MissileCount = BattleTick + BtlActionRate;
		pAvatar->fBtlCap.Shoot = BTLCAP_MISSILE;
		pAvatar->fBtlCap.mfShoot(pAvatar);
		pAvatar->fBtlCap.Shoot = BTLCAP_NOSHOT;
		
		// fake magic shot to get arrow flight
		pAvatar->fBtlCap.fBtlMagicType = BTLCAP_NOSHOT;
		pAvatar->mfCreateFireBall( GetNewId(), ARROWS, ARROW_DAMAGE);
		
		Done = TRUE;
	}
	
	return (Done);
}	

/* -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   Battle Capt Dialog routines
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   ----------------------------------------------------------------- */
   
/* ========================================================================
   Function    - mfMoveAhead
   Description - Move me/my troops ahead one square
   Returns     - 
   ======================================================================== */
BOOL BATTLE_CAPTAIN_DATA::mfMoveAhead(LONG , LONG hAvatar)
{
	SHORT	gridRow;
	SHORT	gridCol;
	BOOL	Result = FALSE;
	
	if (hAvatar == fERROR)
		return FALSE;
		
	CAvatar *pAvatar = (CAvatar *) BLKPTR((SHORT) hAvatar);
	BOOL	HomeTeam = pAvatar->mfIsHomeTeam();
	
	gridRow = pAvatar->fBtlCap.Row;
	gridCol = pAvatar->fBtlCap.Column;
	
	// if in last row, can't advance further north
	if( gridRow > 1)
		return FALSE;
		
	// can't move to a block grid
	if( BTLUI_EXCEPT_IMPASSIBLE == BtlExceptions[gridRow+1][gridCol])
		return FALSE;
		
	pAvatar->Status = CAvatar::AI_MOVING;
	pAvatar->fTimeLastMoved = SCENE_MGR::gTick;
	
	// disengage and move to a new location
	pAvatar->Engaged = 0;
	
	/* check if out way is clear */
	if( HomeTeam  && GetWhosWhere(gridRow+1,gridCol,WW_HOME) == fERROR )
	{
		SetWhosWhere(gridRow,gridCol,WW_HOME,fERROR);
		gridRow++;
		SetWhosWhere(gridRow,gridCol,WW_HOME,hAvatar);
		pAvatar->fBtlCap.TargetX = BattleGridHome[gridRow][gridCol].x; 
		pAvatar->fBtlCap.TargetY = BattleGridHome[gridRow][gridCol].y;
		Result = TRUE;
	}
	else
	if( !HomeTeam && GetWhosWhere(gridRow+1,gridCol,WW_AWAY) == fERROR )
	{
		SetWhosWhere(gridRow,gridCol,WW_AWAY,fERROR);
		gridRow++;
		SetWhosWhere(gridRow,gridCol,WW_AWAY,hAvatar);
		pAvatar->fBtlCap.TargetX = BattleGridAway[gridRow][gridCol].x; 
		pAvatar->fBtlCap.TargetY = BattleGridAway[gridRow][gridCol].y;
		Result = TRUE;
	}
		
	if(gridRow == 3)
		fatal_error("Stupid Grid Movement Error!");
		
	pAvatar->fBtlCap.Row = gridRow;
	pAvatar->fBtlCap.Column = gridCol;
	
	return Result;
}

/* ========================================================================
   Function    - mfMoveBack
   Description - move me/my troops ahead one square
   Returns     - 
   ======================================================================== */
BOOL BATTLE_CAPTAIN_DATA::mfMoveBack(LONG , LONG hAvatar)
{
	if (hAvatar == fERROR)
		return FALSE;
		
	CAvatar *pAvatar = (CAvatar *) BLKPTR((SHORT) hAvatar);
	SHORT	gridRow;
	SHORT	gridCol;
	BOOL	HomeTeam = pAvatar->mfIsHomeTeam();
	BOOL	Result = FALSE;
	
	// not in the reserves
	if(pAvatar->fBtlCap.Row == -1 || pAvatar->fBtlCap.Row == 3)
		return FALSE;
	
	assert(pAvatar->fBtlCap.Column >= 0);
	assert(pAvatar->fBtlCap.Column <= 4);
	
	gridRow = pAvatar->fBtlCap.Row;
	gridCol = pAvatar->fBtlCap.Column;
	
	// if in first row, can't advance further south
	if( gridRow < 1)
		return FALSE;
		
	// can't move to a block grid
	if( BTLUI_EXCEPT_IMPASSIBLE == BtlExceptions[gridRow-1][gridCol])
		return FALSE;
		
	pAvatar->Status = CAvatar::AI_MOVING;
	pAvatar->fTimeLastMoved = SCENE_MGR::gTick;
	
	// disengage and move to a new location
	pAvatar->Engaged = 0;
	
	/* check if out way is clear */
	if( HomeTeam  && GetWhosWhere(gridRow-1,gridCol,WW_HOME) == fERROR )
	{
		SetWhosWhere(gridRow,gridCol,WW_HOME,fERROR);
		gridRow--;
		SetWhosWhere(gridRow,gridCol,WW_HOME,hAvatar);
		pAvatar->fBtlCap.TargetX = BattleGridHome[gridRow][gridCol].x; 
		pAvatar->fBtlCap.TargetY = BattleGridHome[gridRow][gridCol].y;
		Result = TRUE;
	}
	else
	if( !HomeTeam && GetWhosWhere(gridRow-1,gridCol,WW_AWAY) == fERROR )
	{
		SetWhosWhere(gridRow,gridCol,WW_AWAY,fERROR);
		gridRow--;
		SetWhosWhere(gridRow,gridCol,WW_AWAY,hAvatar);
		pAvatar->fBtlCap.TargetX = BattleGridAway[gridRow][gridCol].x; 
		pAvatar->fBtlCap.TargetY = BattleGridAway[gridRow][gridCol].y;
		Result = TRUE;
	}
		
	if(gridRow == 3)
		fatal_error("Stupid Grid Movement Error!");
		
	pAvatar->fBtlCap.Row = gridRow;
	pAvatar->fBtlCap.Column = gridCol;
	
	return Result;
}

/* ========================================================================
   Function    - mfMoveLeft
   Description - move me/my troops left one square
   Returns     - 
   ======================================================================== */
BOOL BATTLE_CAPTAIN_DATA::mfMoveLeft(LONG , LONG hAvatar)
{
	if (hAvatar == fERROR)
		return FALSE;
		
	CAvatar *pAvatar = (CAvatar *) BLKPTR((SHORT) hAvatar);
	SHORT	gridRow;
	SHORT	gridCol;
	BOOL	HomeTeam = pAvatar->mfIsHomeTeam();
	BOOL	Result = FALSE;
	
	// not in the reserves
	if(pAvatar->fBtlCap.Row == -1 || pAvatar->fBtlCap.Row == 3)
		return FALSE;
	
	assert(pAvatar->fBtlCap.Column >= 0);
	assert(pAvatar->fBtlCap.Column <= 4);
	
	gridRow = pAvatar->fBtlCap.Row;
	gridCol = pAvatar->fBtlCap.Column;
	
	// if in first col, can't advance further left
	if( gridCol == 0)
		return FALSE;
		
	// can't move to a block grid
	if( BTLUI_EXCEPT_IMPASSIBLE == BtlExceptions[gridRow][gridCol-1])
		return FALSE;
		
	pAvatar->Status = CAvatar::AI_MOVING;
	pAvatar->fTimeLastMoved = SCENE_MGR::gTick;
	
	// disengage and move to a new location
	pAvatar->Engaged = 0;
	
	/* check if out way is clear */
	if( HomeTeam  && GetWhosWhere(gridRow,gridCol-1,WW_HOME) == fERROR )
	{
		SetWhosWhere(gridRow,gridCol,WW_HOME,fERROR);
		gridCol--;
		SetWhosWhere(gridRow,gridCol,WW_HOME,hAvatar);
		pAvatar->fBtlCap.TargetX = BattleGridHome[gridRow][gridCol].x; 
		pAvatar->fBtlCap.TargetY = BattleGridHome[gridRow][gridCol].y;
		Result = TRUE;
	}
	else
	if( !HomeTeam && GetWhosWhere(gridRow,gridCol-1,WW_AWAY) == fERROR )
	{
		SetWhosWhere(gridRow,gridCol,WW_AWAY,fERROR);
		gridCol--;
		SetWhosWhere(gridRow,gridCol,WW_AWAY,hAvatar);
		pAvatar->fBtlCap.TargetX = BattleGridAway[gridRow][gridCol].x; 
		pAvatar->fBtlCap.TargetY = BattleGridAway[gridRow][gridCol].y;
		Result = TRUE;
	}
		
	if(gridRow == 3)
		fatal_error("Stupid Grid Movement Error!");
		
	pAvatar->fBtlCap.Row = gridRow;
	pAvatar->fBtlCap.Column = gridCol;
	
	return Result;
}

/* ========================================================================
   Function    - mfMoveRight
   Description - move me/my troops left one square
   Returns     - 
   ======================================================================== */
BOOL BATTLE_CAPTAIN_DATA::mfMoveRight(LONG , LONG hAvatar)
{
	if (hAvatar == fERROR)
		return FALSE;
		
	CAvatar *pAvatar = (CAvatar *) BLKPTR((SHORT) hAvatar);
	SHORT	gridRow;
	SHORT	gridCol;
	BOOL	HomeTeam = pAvatar->mfIsHomeTeam();
	BOOL	Result = FALSE;
	
	// not in the reserves
	if(pAvatar->fBtlCap.Row == -1 || pAvatar->fBtlCap.Row == 3)
		return FALSE;
	
	assert(pAvatar->fBtlCap.Column >= 0);
	assert(pAvatar->fBtlCap.Column <= 4);
	
	gridRow = pAvatar->fBtlCap.Row;
	gridCol = pAvatar->fBtlCap.Column;
	
	// if in last col, can't advance further right
	if( gridCol == 4)
		return FALSE;
		
	// can't move to a block grid
	if( BTLUI_EXCEPT_IMPASSIBLE == BtlExceptions[gridRow][gridCol+1])
		return FALSE;
		
	pAvatar->Status = CAvatar::AI_MOVING;
	pAvatar->fTimeLastMoved = SCENE_MGR::gTick;
	
	// disengage and move to a new location
	pAvatar->Engaged = 0;
	
	/* check if out way is clear */
	if( HomeTeam  && GetWhosWhere(gridRow,gridCol+1,WW_HOME) == fERROR )
	{
		SetWhosWhere(gridRow,gridCol,WW_HOME,fERROR);
		gridCol++;
		SetWhosWhere(gridRow,gridCol,WW_HOME,hAvatar);
		pAvatar->fBtlCap.TargetX = BattleGridHome[gridRow][gridCol].x; 
		pAvatar->fBtlCap.TargetY = BattleGridHome[gridRow][gridCol].y;
		Result = TRUE;
	}
	else
	if( !HomeTeam && GetWhosWhere(gridRow,gridCol+1,WW_AWAY) == fERROR )
	{
		SetWhosWhere(gridRow,gridCol,WW_AWAY,fERROR);
		gridCol++;
		SetWhosWhere(gridRow,gridCol,WW_AWAY,hAvatar);
		pAvatar->fBtlCap.TargetX = BattleGridAway[gridRow][gridCol].x; 
		pAvatar->fBtlCap.TargetY = BattleGridAway[gridRow][gridCol].y;
		Result = TRUE;
	}
		
	if(gridRow == 3)
		fatal_error("Stupid Grid Movement Error!");
		
	pAvatar->fBtlCap.Row = gridRow;
	pAvatar->fBtlCap.Column = gridCol;
	
	return Result;
}

/* ========================================================================
   Function    - mfCharge
   Description - set me/my troops charging for the next move
   Returns     - 
   ======================================================================== */
void BATTLE_CAPTAIN_DATA::mfCharge(LONG hAvatar)
{
	if (hAvatar == fERROR)
		return;
		
	CAvatar *pAvatar = (CAvatar *) BLKPTR((SHORT) hAvatar);
	
	pAvatar->Status = CAvatar::AI_END_LISTEN;
}

/* ========================================================================
   Function    - mfEngage
   Description - engage the enemy troops next clicked on
   Returns     - 
   ======================================================================== */
void BATTLE_CAPTAIN_DATA::mfEngage(LONG hAvatar)
{
	if (hAvatar == fERROR)
		return;
		
	CAvatar *pAvatar = (CAvatar *) BLKPTR((SHORT) hAvatar);
	pAvatar->Status = CAvatar::AI_END_LISTEN;
}

/* ========================================================================
   Function    - mfFallBack
   Description - move me/my troops falling back
   Returns     - 
   ======================================================================== */
void BATTLE_CAPTAIN_DATA::mfFallBack(LONG hAvatar)
{
	if (hAvatar == fERROR)
		return;
		
	CAvatar *pAvatar;
	SHORT	gridRow;
	SHORT	gridCol;
	CHAR	temp[80];
	CHAR	buffer[80];
 	
	pAvatar = (CAvatar *) BLKPTR((SHORT) hAvatar);
	
 	// if this is the unit the camera is following, use the command
 	// view camera, can't follow these guys to the reserves
 	if (pAvatar->hThis == hUnitInfo)
 	{
 		BattleCameraHighView(0, 0);
 		hUnitInfo = fERROR;
 		BattlePanelMode(BUILD_LONG(D_BTL_CONTROL,0),BTLUI_MODE_RESERVES);
 	}
 	
	// all ready in reserves
	if( pAvatar->fBtlCap.Row < 0 || pAvatar->fBtlCap.Row > 2 )
		return;

	// if any one needs killing, do so now
	if(pAvatar->fBtlCap.HitCount > 0)
	{
		while(pAvatar->fBtlCap.HitCount > 0)
			pAvatar->fBtlCap.mfKillTroop(pAvatar,pAvatar->KillType);
		
		AddSndObj( SND_MALE_GROUP_DIE1, SND_MALE_GROUP_DIE_TOTAL, VOLUME_EIGHTY);
	}
	
	// if we be dead
	if(pAvatar->fBtlCap.HitPoints <= 0)
		goto FallBackAndDie;
		
	assert(pAvatar->fBtlCap.Column >= 0);
	assert(pAvatar->fBtlCap.Column <= 4);
	
	//assert(pAvatar->fBtlCap.Row >= 0);
	if(pAvatar->fBtlCap.Row < 0)	// my reserves
		return;
		
	//assert(pAvatar->fBtlCap.Row <= 2);
	if(pAvatar->fBtlCap.Row > 2)	// his reserves
		return;
		
	// mark us out of here
	gridRow = pAvatar->fBtlCap.Row;
	gridCol = pAvatar->fBtlCap.Column;
	
	// if I don't already know, decide where I can fall back to
	if(pAvatar->mfIsHomeTeam())
	{
		SetWhosWhere(gridRow,gridCol,WW_HOME,fERROR);
		//look back
		if (gridRow == 0 && !pAvatar->fBtlCap.TargetCol)
		{
			// put myself in the reserves
			BattleMoveToReservesArea(pAvatar->hThis);
			BattlePutInReserves(pAvatar->mfIsHomeTeam(), pAvatar->hThis);
				
			// disengage and move to a new location
			pAvatar->fBtlCap.mfReform(pAvatar);
			pAvatar->Engaged = 0;
			pAvatar->Status = CAvatar::AI_MOVING;
			pAvatar->fTimeLastMoved = SCENE_MGR::gTick;
			
			if(pAvatar->hEnemy != fERROR)
			{
				CAvatar *pOtherAvatar = 
					(CAvatar *) BLKPTR((SHORT) pAvatar->hEnemy);
				pOtherAvatar->hEnemy = fERROR;
				pOtherAvatar->fBtlCap.mfReform(pOtherAvatar);
				pOtherAvatar->Engaged = 0;
				
				pOtherAvatar->Status = CAvatar::AI_SEARCH;
			
				pAvatar->hEnemy = fERROR;
			}
			goto Report;
		}
		else
		if (pAvatar->fBtlCap.TargetRow ||
			pAvatar->fBtlCap.TargetCol )
		{
			gridRow = pAvatar->fBtlCap.Row + pAvatar->fBtlCap.TargetRow;
			gridCol = pAvatar->fBtlCap.Column + pAvatar->fBtlCap.TargetCol;
			pAvatar->fBtlCap.TargetRow = 0;
			pAvatar->fBtlCap.TargetCol = 0;
			pAvatar->Status = CAvatar::AI_MOVING;
		}
		else
		if (gridRow > 0 
			&& BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[gridRow-1][gridCol]
			&& GetWhosWhere(gridRow-1,gridCol,WW_HOME) == fERROR
			&& GetWhosWhere(gridRow-1,gridCol,WW_AWAY) == fERROR
			)
		{
			gridRow--;
			pAvatar->Status = CAvatar::AI_MOVING;
		}
		else
		// look left
		if ( gridCol > 0
			&& BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[gridRow][gridCol-1]
			&& GetWhosWhere(gridRow,gridCol-1,WW_HOME) == fERROR
			&& GetWhosWhere(gridRow,gridCol-1,WW_AWAY) == fERROR
			)
		{
			gridCol--;
			pAvatar->Status = CAvatar::AI_MOVING;
		}
		else
		// look right
		if (gridCol < 4
			&& BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[gridRow][gridCol+1]
			&& GetWhosWhere(gridRow,gridCol+1,WW_HOME) == fERROR
			&& GetWhosWhere(gridRow,gridCol+1,WW_AWAY) == fERROR
			)
		{
			gridCol++;
			pAvatar->Status = CAvatar::AI_MOVING;
		}
		else
		{
			// nowhere to go
			goto FallBackAndDie;
		}
	
		// mark new grid slot with my name
		SetWhosWhere(gridRow,gridCol,WW_HOME,hAvatar);
		pAvatar->fBtlCap.TargetX = BattleGridHome[gridRow][gridCol].x; 
		pAvatar->fBtlCap.TargetY = BattleGridHome[gridRow][gridCol].y;
	}
	else 
	{
		SetWhosWhere(gridRow,gridCol,WW_AWAY,fERROR);
		// told remotely to move
		if (gridRow == 2 && !pAvatar->fBtlCap.TargetCol)
		{
			// put myself in the reserves
			BattleMoveToReservesArea(pAvatar->hThis);
			BattlePutInReserves(pAvatar->mfIsHomeTeam(), pAvatar->hThis);
				
			// disengage and move to a new location
			pAvatar->fBtlCap.mfReform(pAvatar);
			pAvatar->Engaged = 0;
			pAvatar->Status = CAvatar::AI_MOVING;
			pAvatar->fTimeLastMoved = SCENE_MGR::gTick;
			
			if(pAvatar->hEnemy != fERROR)
			{
				CAvatar *pOtherAvatar = 
					(CAvatar *) BLKPTR((SHORT) pAvatar->hEnemy);
				pOtherAvatar->hEnemy = fERROR;
				pOtherAvatar->fBtlCap.mfReform(pOtherAvatar);
				pOtherAvatar->Engaged = 0;
				
				pOtherAvatar->Status = CAvatar::AI_SEARCH;
			
				pAvatar->hEnemy = fERROR;
			}
			goto Report;
		}
		else
		if (pAvatar->fBtlCap.TargetRow ||
			pAvatar->fBtlCap.TargetCol )
		{
			gridRow = pAvatar->fBtlCap.Row + pAvatar->fBtlCap.TargetRow;
			gridCol = pAvatar->fBtlCap.Column + pAvatar->fBtlCap.TargetCol;
			pAvatar->fBtlCap.TargetRow = 0;
			pAvatar->fBtlCap.TargetCol = 0;
			pAvatar->Status = CAvatar::AI_MOVING;
		}
		else
		// look back
		if (gridRow < 2 
			&& BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[gridRow+1][gridCol]
			&& GetWhosWhere(gridRow+1,gridCol,WW_HOME) == fERROR
			&& GetWhosWhere(gridRow+1,gridCol,WW_AWAY) == fERROR
			)
		{
			gridRow++;
			pAvatar->Status = CAvatar::AI_MOVING;
		}
		else
		// look left
		if (gridCol > 0 
			&& BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[gridRow][gridCol-1]
			&& GetWhosWhere(gridRow,gridCol-1,WW_HOME) == fERROR
			&& GetWhosWhere(gridRow,gridCol-1,WW_AWAY) == fERROR
			)
		{
			gridCol--;
			pAvatar->Status = CAvatar::AI_MOVING;
		}
		else
		if (gridCol < 4
			&& BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[gridRow][gridCol+1]
			&& GetWhosWhere(gridRow,gridCol+1,WW_HOME) == fERROR
			&& GetWhosWhere(gridRow,gridCol+1,WW_AWAY) == fERROR
			)
		{
			gridCol++;
			pAvatar->Status = CAvatar::AI_MOVING;
		}
		else
		{
			// nowhere to go
			goto FallBackAndDie;
		}
	
		// mark new grid slot with my name
		SetWhosWhere(gridRow,gridCol,WW_AWAY,hAvatar);
		pAvatar->fBtlCap.TargetX = BattleGridAway[gridRow][gridCol].x; 
		pAvatar->fBtlCap.TargetY = BattleGridAway[gridRow][gridCol].y;
	}
		
	pAvatar->fBtlCap.Row = gridRow;
	pAvatar->fBtlCap.Column = gridCol;

	// if we got here we most likely moved
	pAvatar->fTimeLastMoved = SCENE_MGR::gTick;
	
	// disengage and move to a new location
	pAvatar->fBtlCap.mfReform(pAvatar);
	pAvatar->Engaged = 0;
	
	if(pAvatar->hEnemy != fERROR)
	{
		CAvatar *pOtherAvatar = 
			(CAvatar *) BLKPTR((SHORT) pAvatar->hEnemy);
		pOtherAvatar->fBtlCap.mfReform(pOtherAvatar);
		pOtherAvatar->hEnemy = fERROR;
		pOtherAvatar->Engaged = 0;
		
		pOtherAvatar->Status = CAvatar::AI_SEARCH;
	
		pAvatar->hEnemy = fERROR;
	}

Report:	
	// report the results
	#ifdef BTL_CHAT
	if(pAvatar->mfIsHomeTeam())
	{
		strcpy(temp,STRMGR_GetStr(STR_CHAT_HOME_FALLBACK)); 
		
		// fill in data
		sprintf(buffer, temp, 
			STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.UnitIcon]),
			pAvatar->fBtlCap.Row,
			pAvatar->fBtlCap.Column
			);
		ChatLineAdd(buffer, LT_BLUE);
	}
	else
	{
		strcpy(temp,STRMGR_GetStr(STR_CHAT_ENEMY_FALLBACK)); 
		
		// fill in data
		sprintf(buffer, temp, 
			STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon]),
			pAvatar->fBtlCap.Row,
			pAvatar->fBtlCap.Column
			);
		ChatLineAdd(buffer, RED);
	}
	#endif
	
	// normal return
	return;
	
	// if we died
FallBackAndDie:
	KillBattleCaptain(pAvatar);
	AddSndObj(SND_MALE_GROUP_DIE1, SND_MALE_GROUP_DIE_TOTAL, VOLUME_EIGHTY);
	return;
}

/* ========================================================================
   Function    - mfMoraleCheck
   Description - make a morale check
   Returns     - 
   ======================================================================== */
BOOL BATTLE_CAPTAIN_DATA::mfMoraleCheck(CAvatar *pAvatar)
{
	SHORT				MaxMorale = 0;
	SHORT				MyMorale = 0;
	SHORT				MoraleMods = 0;
	SHORT				TotalMorale;
	
	// the leader of our little army
	if (units[SCENE_MGR::HomeIndex].iHandle == fERROR)
		return FALSE;
		
	CAvatar * pLeaderAvatar = (CAvatar *) BLKPTR(units[SCENE_MGR::HomeIndex].iHandle);
	if (pLeaderAvatar->hPlayerStats == fERROR)
		return FALSE;
		
	DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(pLeaderAvatar->hPlayerStats);
	
	MaxMorale = BTL_MAX_MORALE + 1;
	MyMorale = GetBattleMorale(pAvatar->fBtlCap.OfficerIcon);
	//GEH MoraleMods = pPlayerStats->StatsMod.mfGetMod(STATSMOD::BTL_MORALE_MOD);
	TotalMorale = random(
		MaxMorale - 
		MyMorale - 
		MoraleMods);
		
	// account for a negitive moral (which is very, very good!)
	if(TotalMorale <= 0)
		return TRUE;
	else
		return FALSE;
}					

/* ========================================================================
   Function    - mfRout
   Description - move me/my troops falling back all the way to the reserves
   Returns     - 
   ======================================================================== */
void BATTLE_CAPTAIN_DATA::mfRout(LONG hAvatar, BOOL Broadcast, BOOL Speak)
{
	if (hAvatar == fERROR)
		return;
		
	CAvatar *pAvatar;
	SHORT	gridRow;
	SHORT	gridCol;
	CHAR	temp[80];
	CHAR	buffer[80];
	
	pAvatar = (CAvatar *) BLKPTR((SHORT) hAvatar);
	
 	// if this is the unit the camera is following, use the command
 	// view camera, can't follow these guys to the reserves
 	if (pAvatar->hThis == hUnitInfo)
 	{
 		BattleCameraHighView(0, 0);
 		hUnitInfo = fERROR;
 		BattlePanelMode(BUILD_LONG(D_BTL_CONTROL,0),BTLUI_MODE_RESERVES);
 	}
 	
	// all ready in reserves
	if( pAvatar->fBtlCap.Row < 0 || pAvatar->fBtlCap.Row > 2 )
		return;

	// if any one needs killing, do so now
	while(pAvatar->fBtlCap.HitCount > 0)
	{
		pAvatar->fBtlCap.mfKillTroop(pAvatar,pAvatar->KillType);
	}
	AddSndObj( SND_MALE_GROUP_DIE1, SND_MALE_GROUP_DIE_TOTAL, VOLUME_EIGHTY);
	
	// if we be dead
	if( pAvatar->fBtlCap.HitPoints <= 0)
		goto FallBackAndDie;

	//
	//---- If Broadcast is FALSE then don't check morale it was already done
	//     on the remote machine

	if ( Broadcast == TRUE )
	{

		//---- check as to if succeed a morale check
		if (pAvatar->fBtlCap.mfMoraleCheck(pAvatar))
		{
			AddSndObj(SND_BATTLE_CHARGE1, 0, VOLUME_NINETY);
			return;
		}
	}

	gridRow = pAvatar->fBtlCap.Row;
	gridCol = pAvatar->fBtlCap.Column;
	
	// decide where I can fall back to
	if(pAvatar->mfIsHomeTeam())
	{
		// mark us out of here
		SetWhosWhere(gridRow,gridCol,WW_HOME,fERROR);
		
		if (gridRow == 0)
		{
			// just fall back into the reserves
		}
		else
		if (gridRow > 0 
			&& BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[gridRow-1][gridCol]
			&& GetWhosWhere(gridRow-1,gridCol,WW_HOME) == fERROR
			&& GetWhosWhere(gridRow-1,gridCol,WW_AWAY) == fERROR
			)
		{
			gridRow--;
			pAvatar->Status = CAvatar::AI_MOVING;
		}
		else
		if (gridCol > 0 
			&& BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[gridRow][gridCol-1]
			&& GetWhosWhere(gridRow,gridCol-1,WW_HOME) == fERROR
			&& GetWhosWhere(gridRow,gridCol-1,WW_AWAY) == fERROR
			)
		{
			gridCol--;
			pAvatar->Status = CAvatar::AI_MOVING;
		}
		else
		if (gridCol < 4
			&& BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[gridRow][gridCol+1]
			&& GetWhosWhere(gridRow,gridCol+1,WW_HOME) == fERROR
			&& GetWhosWhere(gridRow,gridCol+1,WW_AWAY) == fERROR
			)
		{
			gridCol++;
			pAvatar->Status = CAvatar::AI_MOVING;
		}
		else
		{
			goto FallBackAndDie;
		}
		
		if(Speak)
		{
			AddVoxSnd(VOX_WE_GET_ROUTED, pAvatar->ThingIndex);
		}
	}
	else 
	{
		// mark us out of here
		SetWhosWhere(gridRow,gridCol,WW_AWAY,fERROR);
		
		if (gridRow == 2)
		{
			// just fall back into the reserves
		}
		else
		if (gridRow < 2
			&& BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[gridRow+1][gridCol]
			&& GetWhosWhere(gridRow+1,gridCol,WW_HOME) == fERROR
			&& GetWhosWhere(gridRow+1,gridCol,WW_AWAY) == fERROR
			)
		{
			gridRow++;
			pAvatar->Status = CAvatar::AI_MOVING;
		}
		else
		if (gridCol > 0 
			&& BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[gridRow][gridCol-1]
			&& GetWhosWhere(gridRow,gridCol-1,WW_HOME) == fERROR
			&& GetWhosWhere(gridRow,gridCol-1,WW_AWAY) == fERROR
			)
		{
			gridCol--;
			pAvatar->Status = CAvatar::AI_MOVING;
		}
		else
		if (gridCol < 4
			&& BTLUI_EXCEPT_IMPASSIBLE != BtlExceptions[gridRow][gridCol+1]
			&& GetWhosWhere(gridRow,gridCol+1,WW_HOME) == fERROR
			&& GetWhosWhere(gridRow,gridCol+1,WW_AWAY) == fERROR
			)
		{
			gridCol++;
			pAvatar->Status = CAvatar::AI_MOVING;
		}
		else
		{
			goto FallBackAndDie;
		}
	}
	
	// troops in the reserves must pass a moral check to come back
	// this is the flag that tells the reserves code
	pAvatar->fBtlCap.FallBackCount = 1;
	
	// put myself in the reserves
	BattleMoveToReservesArea(pAvatar->hThis);
	BattlePutInReserves(pAvatar->mfIsHomeTeam(), pAvatar->hThis);
		
	// disengage and move to a new location
	pAvatar->fBtlCap.mfReform(pAvatar);
	pAvatar->Engaged = 0;
	pAvatar->Status = CAvatar::AI_MOVING;
	pAvatar->fTimeLastMoved = SCENE_MGR::gTick;
	
	if(pAvatar->hEnemy != fERROR)
	{
		CAvatar *pOtherAvatar = 
			(CAvatar *) BLKPTR((SHORT) pAvatar->hEnemy);
		pOtherAvatar->hEnemy = fERROR;
		pOtherAvatar->fBtlCap.mfReform(pOtherAvatar);
		pOtherAvatar->Engaged = 0;
		
		pOtherAvatar->Status = CAvatar::AI_SEARCH;
	
		pAvatar->hEnemy = fERROR;
	}

	//---- Send rout 

	if(Broadcast)
		SendRout ( pAvatar );
	
	// get format string
#ifdef BTL_CHAT
	strcpy(temp,STRMGR_GetStr(STR_CHAT_ROUTED)); 
		
	// fill in data
	sprintf(buffer, temp, 
		STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon]),
		pAvatar->fBtlCap.Row,
		pAvatar->fBtlCap.Column
		);
		
	if(pAvatar->mfIsHomeTeam())
		ChatLineAdd(buffer,LT_BLUE);
	else
		ChatLineAdd(buffer,RED);
	
	// get format string
	strcpy(temp,STRMGR_GetStr(STR_CHAT_ROUTED)); 
		
	// fill in data
	sprintf(buffer, temp, 
		STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon]),
		pAvatar->fBtlCap.Row,
		pAvatar->fBtlCap.Column
		);
		
	if(pAvatar->mfIsHomeTeam())
		ChatLineAdd(buffer,LT_BLUE);
	else
		ChatLineAdd(buffer,RED);
#endif
	
	// normal return
	return;

	// if we died
FallBackAndDie:
	KillBattleCaptain(pAvatar);
	if(Speak)
	{
		// get format string
		AddSndObj( SND_MALE_GROUP_DIE1, SND_MALE_GROUP_DIE_TOTAL, VOLUME_EIGHTY);
	}
	return;
}

/* ========================================================================
   Function    - mfStand
   Description - stand and defend
   Returns     - 
   ======================================================================== */
void BATTLE_CAPTAIN_DATA::mfStand(LONG , LONG hAvatar)
{
	if (hAvatar == fERROR)
		return;
		
	CAvatar *pAvatar = (CAvatar *) BLKPTR((SHORT) hAvatar);
	
	pAvatar->Status = CAvatar::AI_END_LISTEN;
}

/* ========================================================================
   Function    - KillBattleCaptain
   Description - kill of a battle capt and his troops
   Returns     - 
   ======================================================================== */
void KillBattleCaptain(CAvatar *pAvatar)
{
	CHAR	temp[80];
	CHAR	buffer[80];

	fUpdatePanels = TRUE;
	
	// kill off any of my remaining troops
	while(pAvatar->fBtlCap.mfKillTroop(pAvatar,pAvatar->KillType))
	{}
	
	// be sure my icon is off the field
	if(pAvatar->fBtlCap.OnBtlField)
	{
		SetWhosWhere(
			pAvatar->fBtlCap.Row,
			pAvatar->fBtlCap.Column,
			pAvatar->mfIsHomeTeam(),
			fERROR);
	}
		
	// now, if for some reason I am already dead were done
	if(pAvatar->Status == CAvatar::AI_DEAD)
		return;
		
	// release any enemy I'm hooked too
	if(pAvatar->hEnemy != fERROR)
	{
		CAvatar *pOtherAvatar = 
			(CAvatar *) BLKPTR((SHORT) pAvatar->hEnemy);
		pOtherAvatar->hEnemy = fERROR;
		pOtherAvatar->fBtlCap.mfReform(pOtherAvatar);
		pOtherAvatar->Engaged = 0;
		pOtherAvatar->Status = CAvatar::AI_SEARCH;
	
		pAvatar->hEnemy = fERROR;
	}
	
	// kill non-regents
	// regents don't die this easly
	if (!IsRegent(pAvatar->UnitIndex))
	{
		pAvatar->LeftButtonFn = 0;
		pAvatar->RightButtonFn = 0;
		
		// become one with the universe
		pAvatar->Status = CAvatar::AI_DEAD;
		pAvatar->mfPlayAnimationOnce(EXPIRESEQ);
		
		// scream out in agony
		AddSndObj(SND_MALE_SCREAM1, SND_MALE_SCREAM_TOTAL, VOLUME_EIGHTY);
	
		// update the chat information
#ifdef BTL_CHAT
		if(pAvatar->mfIsHomeTeam())
		{
			// get format string
			strcpy(temp,STRMGR_GetStr(STR_CHAT_OUR_TROOPS_DEST)); 
			
			// fill in data
			sprintf(buffer, temp, 
				STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon]),
				pAvatar->fBtlCap.Row,
				pAvatar->fBtlCap.Column
				);
			
			ChatLineAdd(buffer, LT_BLUE);
		}
		else
		{
			// get format string
			strcpy(temp,STRMGR_GetStr(STR_CHAT_ENEMY_TROOPS_DEST)); 
			
			// fill in data
			sprintf(buffer, temp, 
				STRMGR_GetStr(gsUnitTitle[pAvatar->fBtlCap.OfficerIcon]),
				pAvatar->fBtlCap.Row,
				pAvatar->fBtlCap.Column
				);
			
			ChatLineAdd(buffer, RED);
		}
#endif
	}
	// else
	// if(pAvatar->fBtlCap.OnBtlField)
	// {
	// 	// do a kind of mfRout here for regents and officers
	// 	// troops in the reserves must pass a moral check to come back
	// 	// this is the flag that tells the reserves code
	// 	pAvatar->fBtlCap.FallBackCount = 1;
	// 	
	// 	// put myself in the reserves
	// 	BattleMoveToReservesArea(pAvatar->hThis);
	// 	BattlePutInReserves(pAvatar->mfIsHomeTeam(), pAvatar->hThis);
	// }
}

/* -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   BEGIN TROOP AI HERE
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   -----------------------------------------------------------------
   ----------------------------------------------------------------- */

/* ========================================================================
   Function	- FollowBtlCap
   Description - AI Proc to follow a battle captain around
   Returns	 - Current state
   ======================================================================== */

void CAvatar::FollowBtlCap (CAvatar *pAvatar, CAvatar::AISTATUS Status)
{
	LONG	index;
	PTR_SCENE pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
	SHORT rndnum;
	
	switch( Status )
	{
	case AI_INIT:
		pAvatar->Status = AI_MOVING;
		pAvatar->mfStartAnimationLoop(MARCHSEQ);
		pAvatar->LeftButtonFn = (PFVC)FOLLOW_BTLCAP_DATA::mfLeftButton;
		pAvatar->RightButtonFn = (PFVC)FOLLOW_BTLCAP_DATA::mfRightButton;
		pAvatar->fFollowBtlCap.FrameCount = SCENE_MGR::gTick + 20 + random(20);
		pAvatar->fFollowBtlCap.Shoot = BTLCAP_NOSHOT;
		break;
			
	case AI_RELEASE:
		pAvatar->Status = AI_INIT;
		break;
	
	case AI_ATTACK:
		{
		LONG oldX = pAvatar->mfX();
		LONG oldY = pAvatar->mfY();
		LONG tx;
		LONG ty;
		CAvatar *pEnemy; 
		
		// can't attack nobody!!
		if(pAvatar->hEnemy == fERROR)
		{
			// Being this confused, just stand around until further orders
			pAvatar->Engaged--;
			pAvatar->Status = AI_MOVING;
			
			// Added this limiter to prevent us from going major negative.
			if (pAvatar->Engaged < 0)
				pAvatar->Engaged = 0;
				
			pAvatar->mfStartAnimationLoop(STANDSEQ);
				
			break;
		}
		
		pEnemy = (CAvatar *) BLKPTR(pAvatar->hEnemy);
		
		// if my enemy is dead, go back and stand in formation
		if( pEnemy->Status == CAvatar::AI_DEAD )
		{
			pAvatar->hEnemy = fERROR;
			pAvatar->Engaged--;
			pAvatar->Status = AI_MOVING;
			pAvatar->fTimeLastMoved = SCENE_MGR::gTick;
		}
		
		tx = pEnemy->mfX();
		ty = pEnemy->mfY();
		
		// melee
		if( aprox_dist( oldX, oldY, tx, ty) > 50  &&
		    pAvatar->fFollowBtlCap.hLeader != fERROR)
		{
			CAvatar *pLeader = (CAvatar *) BLKPTR(pAvatar->fFollowBtlCap.hLeader);
			pAvatar->mfStartAnimationOnce(MARCHSEQ);
			pAvatar->FaceTo(tx, ty);
			pAvatar->MoveToward(tx, ty, 0, pLeader->fBtlCap.mfTerrainSpeed( pLeader, pLeader->fBtlCap.Rate+4));
		}
			
		if(pAvatar->mfTestSequenceDone())
		{
			rndnum = random(4);
			switch(rndnum)
			{
			case 0:
				pAvatar->mfStartAnimationOnce(ATTACK1SEQ);
				break;
			case 1:
				if (GAME_TTYPE::mfHasAttack2Animation(pAvatar->mfType()))
				{
					pAvatar->mfStartAnimationOnce(ATTACK2SEQ);
				}
				else
				{
					pAvatar->mfStartAnimationOnce(ATTACK1SEQ);
				}
				break;
			case 2:
				pAvatar->mfStartAnimationOnce(DEFENDSEQ);
				break;
			}
			
			// decide how much fight'n noise to make
			//GEH too much rndnum = random(32);	
			rndnum = random(66);	
			if(rndnum==0)
				AddSndObj( SND_BIG_SWORD_HIT1, SND_BIG_SWORD_HIT_TOTAL, VOLUME_EIGHTY);
			else
			if(rndnum==1)
				AddSndObj( SND_SMALL_SWORD_HIT1, SND_SMALL_SWORD_HIT_TOTAL, VOLUME_EIGHTY);
			else
			if(rndnum==3)
				AddSndObj( SND_SWORD_SLASH1, SND_SWORD_SLASH_TOTAL, VOLUME_EIGHTY);
			else
			if(rndnum==4)
				AddSndObj( SND_SHIELD_HIT1, SND_SHIELD_HIT_TOTAL, VOLUME_EIGHTY);
			else
			if(rndnum==5)
				AddSndObj( SND_AX_CHOP_FLESH1, SND_AX_CHOP_FLESH_TOTAL, VOLUME_EIGHTY);
		}
		
		}
		break;
			
	case AI_DEAD:
		// pretend to be a lump of meat slowly approching air temperature
		if (pAvatar->CurSequence != DEADSEQ &&
			pAvatar->mfTestSequenceDone())
		{
			// if the death was visible, send my horse off to the sidelines
			if( pAvatar->mfIsVisible() )
			{
				// send a cav horse running off
				if ((pAvatar->mfType() == LIGHT_CAVALRY ) ||
					(pAvatar->mfType() == MERC_CAVALRY ) ||
					(pAvatar->mfType() == LORES_A_CAV ) )
					pAvatar->mfCreateHorse(GetNewId(), LORES_CAV_HORSE);
				
				// send a night horse running off
				if ((pAvatar->mfType() == KNIGHT ) ||
					(pAvatar->mfType() == ELF_CAVALRY ) ||
					(pAvatar->mfType() == LORES_A_KNT ) )
					pAvatar->mfCreateHorse(GetNewId(), LORES_KNIGHT_HORSE);
			}
				
			pAvatar->mfPlayAnimationOnce(DEADSEQ);
			pAvatar->SetAIFuncIndex(AI_FUNC_NULL);
		}
		if (pAvatar->mfGetArtSequence() != DEADSEQ &&
			pAvatar->mfTestSequenceDone() )
			pAvatar->mfPlayAnimationOnce(DEADSEQ);
		// dont break!
		return;
		
	case AI_FIDGET:
		if(pAvatar->fFollowBtlCap.Shoot != BTLCAP_NOSHOT)
		{
			// stop fidgetting if a shot is requested
			pAvatar->Status = AI_MOVING;
			pAvatar->fTimeLastMoved = SCENE_MGR::gTick;
			break;
		}
			
		if (pAvatar->mfX() != pAvatar->fFollowBtlCap.NewX || 
			pAvatar->mfY() != pAvatar->fFollowBtlCap.NewY )
		{
			if (!random(3))
			{
				pAvatar->Status = AI_MOVING;
				pAvatar->fTimeLastMoved = SCENE_MGR::gTick;
			}
		}
		
		if( pAvatar->fFollowBtlCap.FrameCount < SCENE_MGR::gTick)
		{
			pAvatar->fFollowBtlCap.FrameCount = SCENE_MGR::gTick + 20 + random(20);
			switch( random(4))
			{
			case 0:
				pAvatar->mfStartAnimationOnce(DEFENDSEQ);
				break;
			case 1:	
				pAvatar->mfSetAngle(pAvatar->fFollowBtlCap.NewA + 32);
				break;
			case 2:	
				pAvatar->mfSetAngle(pAvatar->fFollowBtlCap.NewA - 32);
				break;
			case 3: // nothing happens
				break;
			}
			pAvatar->Status = AI_MOVING;
			pAvatar->fTimeLastMoved = SCENE_MGR::gTick;
		}
		
		break;
	
	case AI_MOVING:
		{
		LONG oldX = pAvatar->mfX();
		LONG oldY = pAvatar->mfY();
		LONG tx = pAvatar->fFollowBtlCap.NewX;
		LONG ty = pAvatar->fFollowBtlCap.NewY;
			
		if (tx != oldX || ty != oldY &&
			pAvatar->fFollowBtlCap.hLeader != fERROR)
		{
			CAvatar *pLeader = (CAvatar *) BLKPTR(pAvatar->fFollowBtlCap.hLeader);
			pAvatar->mfStartAnimationOnce(MARCHSEQ);
			pAvatar->mfSetAngle(pAvatar->fFollowBtlCap.NewA);
			pAvatar->MoveToward(tx, ty, 0, pLeader->fBtlCap.Movement + 1);
			// decide how much mov'n noise to make
		//	rndnum = random(8);	
		//	if(rndnum==0)
		//		AddSndObj( SND_HUMAN_MARCHING_LOOP1, NULL, VOLUME_EIGHTY);
		}
		else
		{
			if(pAvatar->fFollowBtlCap.Shoot != BTLCAP_NOSHOT)
			{
				// keep testing until we can shoot, this is so that
				// not all of the archers shoot at exactly the same time
				//if(!random(3))
				if(pAvatar->fFollowBtlCap.hLeader != fERROR) // GEH for now
				{
					CAvatar *pLeader = (CAvatar *) BLKPTR(pAvatar->fFollowBtlCap.hLeader);
					pAvatar->mfSetAngle(pLeader->mfAngle());
					pAvatar->fFollowBtlCap.Shoot = BTLCAP_NOSHOT;
					pAvatar->mfStartAnimationOnce(ATTACK1SEQ);
					//GEH AddSndObj( SND_ARROW_SHOT1, SND_ARROW_SHOT_TOTAL, VOLUME_EIGHTY);
				}
			}
			else
			{
				if (pAvatar->mfTestSequenceDone())
					pAvatar->mfStartAnimationOnce(STANDSEQ);
				
				pAvatar->mfSetAngle(pAvatar->fFollowBtlCap.NewA);
				if ( pAvatar->fFollowBtlCap.FrameCount < SCENE_MGR::gTick )
				{
					pAvatar->fFollowBtlCap.FrameCount = SCENE_MGR::gTick + 20 + random(20);
					pAvatar->Status = AI_FIDGET;
				}
			}
		}
				
		}
		break;
	
	default:
		break;
	}
}

/* ========================================================================
   Function	   - mfLeftButton
   Description - function to handle a left click of the mouse
   Returns	   - void
   ======================================================================== */
void FOLLOW_BTLCAP_DATA::mfLeftButton(CAvatar *pAvatar)
{
	// pass this button click on to my leader
	if (pAvatar->fFollowBtlCap.hLeader != fERROR)
	{
		pAvatar = (CAvatar *)BLKPTR(pAvatar->fFollowBtlCap.hLeader);
		BATTLE_CAPTAIN_DATA::mfLeftButton(pAvatar);
	}
}

/* ========================================================================
   Function	   - mfRightButton
   Description - function to handle a Right click of the mouse
   Returns	   - void
   ======================================================================== */
void FOLLOW_BTLCAP_DATA::mfRightButton(CAvatar *pAvatar)
{
	// just temp
	pAvatar = 0;
}


/* ========================================================================
   Function	- FindUnengaged
   Description - Find the nearest unengaged avatar of Type
   Returns	 - hdl of closest avatar
   ======================================================================== */
SHORT FOLLOW_BTLCAP_DATA::FindUnengaged( CAvatar *pTroop, SHORT EnemyLdr, THINGTYPE /* Type */)
{
	LONG	d, i; 
	SHORT	ReturnVal=fERROR;
	LONG	closestD=9999;
	LONG	leastEngaged=9999;
	CAvatar *pLeader;
	SHORT	*pList; 
	
	if(EnemyLdr == fERROR)
		return fERROR;
		
	pLeader = (CAvatar *)BLKPTR(EnemyLdr);
	if (pLeader->fBtlCap.Leader.hdlSlotList == fERROR)
		return fERROR;
	pList = (SHORT *)BLKPTR(pLeader->fBtlCap.Leader.hdlSlotList);
	if (pList == NULL)
		return fERROR;
	for(i=pLeader->fBtlCap.Leader.sMaxFollowers-1; i >= 0; i--)
	{
		if(pList[i] != fERROR)
		{
			CAvatar *pEnemyTroop = (CAvatar *) BLKPTR(pList[i]);

			// if this guy is least engaged so far
			if(pEnemyTroop->Engaged < leastEngaged)
			{
				leastEngaged = pEnemyTroop->Engaged;
				closestD = 999;
			}	
			
			if(pEnemyTroop->Engaged <= leastEngaged)
			{
				d = aprox_dist(
					pTroop->mfX(), pTroop->mfY(), 
					pEnemyTroop->mfX(), pEnemyTroop->mfY()
					);
				
				if(d < closestD)
				{
					closestD = d;
					ReturnVal = pEnemyTroop->hThis;
				}
			}
		}
	}
	
	return(ReturnVal);
}

/* ========================================================================
   Function    - BattleMoveToReservesArea
   Description - move the avatars out of the battle grid
   Returns     - 
   ======================================================================== */
void BattleMoveToReservesArea( SHORT hAvatar )
{
	if (hAvatar == fERROR)
		return;
		
	CAvatar *pAvatar = (CAvatar *)BLKPTR(hAvatar);
	
	// set a movement target for the reserve area
	if(pAvatar->mfIsHomeTeam())
	{
		pAvatar->fBtlCap.TargetX = 1000 - random(2000);
		pAvatar->fBtlCap.TargetY = MY_RESERVES_Y;
		pAvatar->fBtlCap.Row = -1;
		pAvatar->fBtlCap.Column = 2;
	}
	else
	{
		pAvatar->fBtlCap.TargetX = 1000 - random(2000);
		pAvatar->fBtlCap.TargetY = ENEMY_RESERVES_Y;
		pAvatar->fBtlCap.Row = GRID_MAX_ROWS;
		pAvatar->fBtlCap.Column = 2;
	}


	// take myself off the battle field
	pAvatar->fBtlCap.OnBtlField = FALSE;
	pAvatar->Status = CAvatar::AI_MOVING;
	pAvatar->fTimeLastMoved = SCENE_MGR::gTick;
	pAvatar->fBtlCap.Shoot = BTLCAP_NOSHOT;
	pAvatar->fBtlCap.Shot = 0;
	
}

/* ========================================================================
   Function	   - mfTerrainSpeed
   Description - adjust the speed of this avatar for the terrain we're on
   Returns	   - new speed
   ======================================================================== */
LONG BATTLE_CAPTAIN_DATA::mfTerrainSpeed(CAvatar *pAvatar, SHORT BaseRate)
{
	SHORT	Special;
	LONG	NewRate = BaseRate;
	
	Special = sectors[pAvatar->mfGetSector()].special;
	
	switch(Special)
	{
	case SSP_WATER: // also known as Swamp
		// everyone, excpt Elves, looses any speed advantage
		if( (pAvatar->fBtlCap.UnitIcon == A_SCT_UNIT_ICON) ||
			(pAvatar->fBtlCap.UnitIcon == E_INF_UNIT_ICON) ||
			(pAvatar->fBtlCap.UnitIcon == E_BOW_UNIT_ICON) ||
			(pAvatar->fBtlCap.UnitIcon == E_CAV_UNIT_ICON) ||
			(pAvatar->fBtlCap.UnitIcon == C_SPD_UNIT_ICON) ||
			(pAvatar->fBtlCap.UnitIcon == C_WYV_UNIT_ICON)
			)
		{
			NewRate = BaseRate;
		}
		else
		{
			NewRate = (LONG)(pAvatar->mfGetMarchRate());
		}
		break;
		
	case SSP_DEEP_WATER:
		// everyone looses any speed advantage and slows down
		NewRate = (LONG)(pAvatar->mfGetMarchRate())/2;
		break;
		
	case SSP_MOUNTAIN:
		// except Dwarves, slow down my half
		if( (pAvatar->fBtlCap.UnitIcon == A_SCT_UNIT_ICON) ||
			(pAvatar->fBtlCap.UnitIcon == E_INF_UNIT_ICON) ||
			(pAvatar->fBtlCap.UnitIcon == E_BOW_UNIT_ICON) ||
			(pAvatar->fBtlCap.UnitIcon == E_CAV_UNIT_ICON) ||
			(pAvatar->fBtlCap.UnitIcon == D_INF_UNIT_ICON) ||
			(pAvatar->fBtlCap.UnitIcon == D_BOW_UNIT_ICON) ||
			(pAvatar->fBtlCap.UnitIcon == C_SPD_UNIT_ICON) ||
			(pAvatar->fBtlCap.UnitIcon == C_WYV_UNIT_ICON)
			)
		{
			NewRate = BaseRate;
		}
		else
		{
			NewRate = BaseRate / 2;
		}
		break;
		
	case SSP_FOREST:
		// slow down my half
		if( (pAvatar->fBtlCap.UnitIcon == A_SCT_UNIT_ICON) ||
			(pAvatar->fBtlCap.UnitIcon == E_INF_UNIT_ICON) ||
			(pAvatar->fBtlCap.UnitIcon == E_BOW_UNIT_ICON) ||
			(pAvatar->fBtlCap.UnitIcon == E_CAV_UNIT_ICON) ||
			(pAvatar->fBtlCap.UnitIcon == C_SPD_UNIT_ICON) ||
			(pAvatar->fBtlCap.UnitIcon == C_WYV_UNIT_ICON)
			)
		{
			NewRate = BaseRate;
		}
		else
		{
			NewRate = BaseRate / 2;
		}
		break;
	
	case SSP_NORMAL:
	default:
		//nothing
		break;
	}
	
	return NewRate;
}


/* ========================================================================
   Function    - DisposeTroopList
   Description - 
   Returns     - 
   ======================================================================== */
void DisposeTroopList(CAvatar *pAvatar)
{
	if (pAvatar->fBtlCap.Leader.hdlSlotList != fERROR)
	{
		SHORT *pList = (SHORT *)BLKPTR(pAvatar->fBtlCap.Leader.hdlSlotList);
		if (pList != NULL)
		{
			for(SHORT i=0; i<pAvatar->fBtlCap.Leader.sMaxFollowers; i++)
			{
				if(pList[i] != fERROR)
				{
					LONG const hThisAvatar = pList[i];
					CAvatar * const pTroop = (CAvatar * const ) BLKPTR(hThisAvatar);
					pTroop->DeleteAvatar(pTroop->hThis);
					pList[i] = fERROR;
					
					// GWP I added this bit of code because troops on the grid
					//     were still engaged to disappeared troops. (I suspect
					//     that the code to move them to the reserves was called
					//     because they had been lost on the grid, and now the 
					//     troops they were fighting, are pointing to deleted handles.)
					// Now search the entire set of avatars and detach me from
					// them.
					SCENE * const pCurrentScene = (SCENE * const) BLKPTR(SCENE_MGR::hCurrentScene);
					for (LONG k = 0; k < MAX_AVATARS; ++k)
					{
						if (pCurrentScene->Avatars[k] != fERROR)
						{
							CAvatar *pAvatar = (CAvatar *) BLKPTR(pCurrentScene->Avatars[k]);
							if (pAvatar->hEnemy == hThisAvatar)
							{
								pAvatar->hEnemy = fERROR;
							}
						}
					}
				}
			}
			DisposBlock(pAvatar->fBtlCap.Leader.hdlSlotList);
			pAvatar->fBtlCap.Leader.hdlSlotList = fERROR;
		}
	}
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// MULTI-PLAYER CODE
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/* ========================================================================
   Function	- CheckDisconnect
   Description - Check for multiplayer disconnection
   Returns	 - 
   ======================================================================== */
void CheckDisconnect(void)
{

#ifdef _WINDOWS

	//---- Is the other player disconnected 

	if ( dwBattleMultiId != kNoMultiId )
	{

		if ( !AMultiPlayer.CheckPlayerId( dwBattleMultiId ) )
		{
			dwBattleMultiId = kNoMultiId;
		}
	}

#endif

}



/* ========================================================================
   Function	- TransRowCol
   Description - Mirror row column as if I were the other team
   Returns	 - 
   ======================================================================== */
void TransRowCol( SHORT * Row, SHORT * Column )
{

   if ( *Row == 0 ) 
      *Row = 2;
   else if ( *Row == 2 )
      *Row = 0;

   if ( *Column == 0 )
      *Column = 4;
   else if ( *Column == 1 )
      *Column = 3;
   else if ( *Column == 3 )
      *Column = 1;
   else if ( *Column == 4 )
      *Column = 0;

}  //---- TransRowCol()


/* ========================================================================
   Function	- SendMove
   Description - Send rmove to other player
   Returns	 - 
   ======================================================================== */
void SendMove ( CAvatar *pAvatar )
{
	#ifdef _WINDOWS


		//----  if a remote player then send move message to them

		CheckDisconnect();

		if ( dwBattleMultiId != kNoMultiId  &&
			 pAvatar->mfIsHomeTeam()           )
		{

			MultiPrintf ( "SendMove: unit %d to r:%d,c%d\n", 
				pAvatar->fBtlCap.UnitIndex,
				pAvatar->fBtlCap.Row,        
				pAvatar->fBtlCap.Column );

			AMultiPlayer.Battle (
				iMPST_BATTLE_MOVE,
				pAvatar->fBtlCap.UnitIndex,  // @@@ troopid,
				pAvatar->fBtlCap.Row,        // @@@ row,
				pAvatar->fBtlCap.Column,     // @@@ col,
				0,
				dwBattleMultiId );
		}

	#endif


}	//---- End of SendMove()



/* ========================================================================
   Function	- SendArrive
   Description - Send arrive to other player
   Returns	 - 
   ======================================================================== */
void SendArrive ( CAvatar *pAvatar )
{
	#ifdef _WINDOWS

		CheckDisconnect();

		//----  if a remote player then send arrive message to them

		if ( dwBattleMultiId != kNoMultiId )
		{

			MultiPrintf ( "SendArrive: %d arrived\n", 
				pAvatar->fBtlCap.UnitIndex );

			AMultiPlayer.Battle (
				iMPST_BATTLE_ARRIVE,
				pAvatar->fBtlCap.UnitIndex,  // @@@ troopid,
				pAvatar->fBtlCap.Row,        // @@@ row,
				pAvatar->fBtlCap.Column,     // @@@ col,
  				0,
				dwBattleMultiId );
		}

	#endif
}	//---- End of SendArrive()



/* ========================================================================
   Function	- SendWin
   Description - Send win or loose 
   Returns	 - 
   ======================================================================== */
void SendWin ( BOOL IWin )
{
	#ifdef _WINDOWS

		CheckDisconnect();


		//----  if a remote player then send win/lose message to them

		if ( dwBattleMultiId != kNoMultiId )
		{

			MultiPrintf ( "SendWin: %d Won\n", IWin );

			AMultiPlayer.Battle (
				iMPST_BATTLE_WIN,
				IWin, 
				0,    
				0,    
  				0,
            	dwBattleMultiId );
		}

	#endif

}	//---- End of SendWin()





/* ========================================================================
   Function	- SendResults
   Description - Send attack results to remote 
   Returns	    - 
   ======================================================================== */

void SendResults ( CAvatar *pAvatar, SHORT points, SHORT shot )
{

#ifdef _WINDOWS

	CheckDisconnect();

	//----  if a remote player then send results message to them

	if ( dwBattleMultiId != kNoMultiId )
	{

		MultiPrintf ( "SendResults: unit:%d points:%d shot:%d\n", 
			pAvatar->fBtlCap.UnitIndex,
			points,
			shot );

		AMultiPlayer.Battle ( 
			iMPST_BATTLE_RESULTS,
			pAvatar->fBtlCap.UnitIndex,  // troopid,
			points,        			   // points,
			shot,              		   // shot type
 			0,
            dwBattleMultiId );
	}

#endif

}


/* ========================================================================
   Function	- SendRout
   Description - Send routed 
   Returns	    - 
   ======================================================================== */

void SendRout	( CAvatar *pAvatar )
{

#ifdef _WINDOWS

	CheckDisconnect();

	//----  if a remote player then send rout message to them

	if ( dwBattleMultiId != kNoMultiId )
	{
		MultiPrintf ( "SendRout: unit:%d\n", 
			pAvatar->fBtlCap.UnitIndex );

		AMultiPlayer.Battle ( 
			iMPST_BATTLE_ROUTE,
			pAvatar->fBtlCap.UnitIndex,  // troopid,
			0, 
			0,              
			0,
            dwBattleMultiId );
	}

#endif

}

/* ========================================================================
   Function	- SendFallBack
   Description - Send fall back message 
   Returns	    - 
   ======================================================================== */

void SendFallBack ( CAvatar *pAvatar, SHORT Row, SHORT Column ) 
{

#ifdef _WINDOWS

	CheckDisconnect();

	//----  if a remote player then send fall back message to them

	if ( dwBattleMultiId != kNoMultiId )
	{

		MultiPrintf ( "SendFallBack: unit:%d to %d %d\n", 
			pAvatar->fBtlCap.UnitIndex,
			Row,
			Column
			);

		AMultiPlayer.Battle ( 
			iMPST_BATTLE_FALLBACK,
			pAvatar->fBtlCap.UnitIndex,  // troopid,
			Row,
			Column,              
 			0,
			dwBattleMultiId );


	}

#endif

}


/* ========================================================================
   Function	- SendToReserves
   Description - Send unit to the reserves
   Returns	    - 
   ======================================================================== */

void SendToReserves ( CAvatar *pAvatar ) 
{

#ifdef _WINDOWS

	CheckDisconnect();

	//----  if a remote player then send fall back message to them

	if ( dwBattleMultiId != kNoMultiId )
	{
		MultiPrintf ( "SendToReserves: unit:%d\n", 
			pAvatar->fBtlCap.UnitIndex );

		AMultiPlayer.Battle ( 
			iMPST_BATTLE_RESERVES,
			pAvatar->fBtlCap.UnitIndex,  // troopid,
			0, 
			0,              
 			0,
			dwBattleMultiId );


	}

#endif

}

/* ========================================================================
   Function	- SendMagic
   Description - Send Magic 
   Returns	    - 
   ======================================================================== */

void SendMagic ( CAvatar *pAvatar ) 
{

#ifdef _WINDOWS

	CAvatar *pEnemy;

	CheckDisconnect();

	if (pAvatar->hEnemy == fERROR )
		return;
	
	pEnemy = (CAvatar *)BLKPTR(pAvatar->hEnemy);
	
	//----  if a remote player then send magic message to them

	if ( dwBattleMultiId != kNoMultiId )
	{
		MultiPrintf ( "SendMagic: unit:%d enemy:%d fBtlMagicType:%d fBtlMagicResult:%d\n", 
			pAvatar->fBtlCap.UnitIndex,
			pEnemy->fBtlCap.UnitIndex,
			pAvatar->fBtlCap.fBtlMagicType,
			pAvatar->fBtlCap.fBtlMagicResult
			);

		AMultiPlayer.Battle ( 
			iMPST_BATTLE_MAGIC,
			pAvatar->fBtlCap.UnitIndex,	// troopid,
			pEnemy->fBtlCap.UnitIndex,		// at whom
			pAvatar->fBtlCap.fBtlMagicType,						// what magic
 			pAvatar->fBtlCap.fBtlMagicResult,					// what kind of damage
			dwBattleMultiId );
	}

#endif

}




/* ========================================================================
   Function	- CheckArrive
   Description - Check and see if the unit arrived on the other machine
   Returns	    - 1 - we teleported 0 - we didn't
   ======================================================================== */
int CheckArrive ( CAvatar *pAvatar )
{
	#ifdef _WINDOWS

   MESSQUE MessBuf;

	//---- The unit arrived message

	if ( Get_MessQueTS1( 
			MultiPlayer::iMPT_BATTLE, 
			iMPST_BATTLE_ARRIVE, 
			pAvatar->fBtlCap.UnitIndex,
			&MessBuf ) )
	{

		MultiPrintf ( "CheckArrive: unit:%d arrived\n",
			pAvatar->fBtlCap.UnitIndex );

		pAvatar->mfMoveTo( 
			pAvatar->fBtlCap.TargetX, 
			pAvatar->fBtlCap.TargetY );

		// Remove all move messages from que
		//GEH ? do we really want to throw these away??
		//while ( Get_MessQueTS1( 
		//	MultiPlayer::iMPT_BATTLE, 
		//	iMPST_BATTLE_MOVE, 
		//	pAvatar->fBtlCap.UnitIndex,
		//	&MessBuf ) )
		//{}

		fUpdatePanels = TRUE;

		return 1;
	}

	#endif

	return 0;

}

/* ========================================================================
   Function	- CheckResults
   Description - For any results 
   Returns	    - 
   ======================================================================== */

void CheckResults ( CAvatar *pAvatar )
{
#ifdef _WINDOWS

   MESSQUE MessBuf;


	//---- The check for a results message 

	while ( Get_MessQueTS1( MultiPlayer::iMPT_BATTLE, 
 						    iMPST_BATTLE_RESULTS, 
						    pAvatar->fBtlCap.UnitIndex,
						    &MessBuf ) )
	{

		MultiPrintf ( "CheckResults: unit:%d points:%d shot:%d\n", 
			pAvatar->fBtlCap.UnitIndex,
			MessBuf.data[1],
			MessBuf.data[2] );

		pAvatar->fBtlCap.HitCount += MessBuf.data[1];
		pAvatar->KillType  = (THINGTYPE)MessBuf.data[2];	   

		fUpdatePanels = TRUE;
	}

#endif

}


/* ========================================================================
   Function	- CheckRout
   Description - Check for rout messages 
   Returns	    - 
   ======================================================================== */

void CheckRout ( CAvatar *pAvatar )
{ 

#ifdef _WINDOWS

   MESSQUE MessBuf;

	//---- The check for a rout message 

//	while ( Get_MessQueTS1( MultiPlayer::iMPT_BATTLE, 

	if ( Get_MessQueTS1( MultiPlayer::iMPT_BATTLE, 
 	    			    iMPST_BATTLE_ROUTE, 
					    pAvatar->fBtlCap.UnitIndex,
					    &MessBuf ) )
	{

		MultiPrintf ( "CheckRout: unit:%d routed\n", 
			pAvatar->fBtlCap.UnitIndex );

		pAvatar->fBtlCap.FallBackCount = 0;

		//GEH pAvatar->fBtlCap.mfRoutReal(pAvatar->hThis);

		pAvatar->fBtlCap.mfRout(pAvatar->hThis, FALSE, TRUE);

		fUpdatePanels = TRUE;
	}

#endif

}

/* ========================================================================
   Function	- CheckFallBack
   Description - Check for fall back 
   Returns	    - 
   ======================================================================== */

void CheckFallBack ( CAvatar *pAvatar )
{ 

#ifdef _WINDOWS

	MESSQUE MessBuf;

	//---- The check for a fall back message 

//	while ( Get_MessQueTS1( MultiPlayer::iMPT_BATTLE, 

	if ( Get_MessQueTS1( MultiPlayer::iMPT_BATTLE, 
 						    iMPST_BATTLE_FALLBACK, 
						    pAvatar->fBtlCap.UnitIndex,
						    &MessBuf ) )
	{
		SHORT r = MessBuf.data[1];
		SHORT c = MessBuf.data[2];
	
		//---- Swap them around 
		r *= -1;
		c *= -1;
	
		pAvatar->fBtlCap.FallBackCount = 0;
		pAvatar->fBtlCap.TargetRow = r;
		pAvatar->fBtlCap.TargetCol = c;

		MultiPrintf ( "CheckFallBack: unit:%d fell back %d %d\n", 
			pAvatar->fBtlCap.UnitIndex,
			pAvatar->fBtlCap.TargetRow,
			pAvatar->fBtlCap.TargetCol
			);

		pAvatar->fBtlCap.mfFallBack(pAvatar->hThis);
		
		fUpdatePanels = TRUE;

	}

#endif

}


/* ========================================================================
   Function	- CheckMove
   Description - check for move 
   Returns	    - 
   ======================================================================== */

void CheckMove(CAvatar *pAvatar)
{

#ifdef _WINDOWS

   MESSQUE MessBuf;
  	SHORT i;
	SHORT r,c;


	//---- grab any moves this avatar has made 

	if ( Get_MessQueTS1( MultiPlayer::iMPT_BATTLE, 
		iMPST_BATTLE_MOVE, 
		pAvatar->fBtlCap.UnitIndex,
		&MessBuf ) )
	{
		r = MessBuf.data[1];
		c = MessBuf.data[2];
	
		//---- Swap them around 
		TransRowCol( &r, &c );
	
		//---- We have a move message do something with it
		MultiPrintf ( "CheckMove: unit:%d to r:%d c:%d\n",
			MessBuf.data[0], r, c );  
	
		// if from the reserves
		if (pAvatar->fBtlCap.Row == -1 ||
			pAvatar->fBtlCap.Row == 3 )
		{
			// if from the reserves, clear that array location
			for(i=0;i<MAX_RESERVES;i++)
			{
				if (Reserves[0][i] == (SHORT)pAvatar->hThis)
					Reserves[0][i] = fERROR;
			}
			// end pause state and generate followers
			pAvatar->Status = CAvatar::AI_END_PAUSE;
		}
		// already on the field
		else 
		{
			// clear old grid location
			SetWhosWhere(pAvatar->fBtlCap.Row,pAvatar->fBtlCap.Column,WW_AWAY,fERROR);
			// move out
			pAvatar->Status = CAvatar::AI_MOVING;
		}
			
		// move onto the grid
		SetWhosWhere(r,c,WW_AWAY,pAvatar->hThis);
	
		// update the avatar data
		pAvatar->fTimeLastMoved = SCENE_MGR::gTick;
		pAvatar->fBtlCap.TargetX = BattleGridAway[r][c].x; 
		pAvatar->fBtlCap.TargetY = BattleGridAway[r][c].y;
		pAvatar->fBtlCap.Row = r;
		pAvatar->fBtlCap.Column = c;
		pAvatar->fBtlCap.OnBtlField = TRUE;
	
		fUpdatePanels = TRUE;

	}

#endif

}


/* ========================================================================
   Function	- CheckReserves
   Description - Check for movement to reserves 
   Returns	    - 
   ======================================================================== */

void CheckReserves(CAvatar *pAvatar)
{ 

#ifdef _WINDOWS

	MESSQUE MessBuf;

	//---- The check for a move to reserves message 

	if ( Get_MessQueTS1( MultiPlayer::iMPT_BATTLE, 
		iMPST_BATTLE_RESERVES, 
		pAvatar->fBtlCap.UnitIndex,
		&MessBuf ) )
	{

		MultiPrintf ( "CheckReserves: %d to the reserves\n", 
			MessBuf.data[0] );

		// remove him from the grid
		SetWhosWhere(
			pAvatar->fBtlCap.Row, 
			pAvatar->fBtlCap.Column,
			WW_AWAY, fERROR);
			
		// now place him in the reserves

		BattleMoveToReservesArea(pAvatar->hThis);
		BattlePutInReserves(pAvatar->mfIsHomeTeam(), pAvatar->hThis);

		fUpdatePanels = TRUE;
	}

#endif

}


/* ========================================================================
   Function	- CheckWin
   Description - Check for a win or loose
   Returns	    - 
   ======================================================================== */

void CheckWin ( void )
{ 

#ifdef _WINDOWS

	MESSQUE MessBuf;

	//---- The check for a win/loose message 

	while ( Get_MessQueTS( MultiPlayer::iMPT_BATTLE, 
 						   iMPST_BATTLE_WIN, 
					       &MessBuf ) )
	{

		MultiPrintf ( "CheckWin: %d Wins\n", 
			MessBuf.data[0] );

		fUpdatePanels = TRUE;

		if ( MessBuf.data[0] == TRUE )
		{
			BattleLoose( 0, 0); 
		}
		else
		{
			BattleWin( 0, 0); 
		}
	}

#endif

}



/* ========================================================================
   Function	- CheckMagic
   Description - check for magic 
   Returns	    - 
   ======================================================================== */

void CheckMagic ( CAvatar *pAvatar )
{ 

#ifdef _WINDOWS

	MESSQUE MessBuf;

	//---- The check for a magic message 

	if ( Get_MessQueTS1( MultiPlayer::iMPT_BATTLE, 
 						    iMPST_BATTLE_MAGIC, 
						    pAvatar->fBtlCap.UnitIndex,
						    &MessBuf ) )
	{

		SHORT i,j;
		SHORT Enemy = fERROR;
		SHORT EnemyUnit = (SHORT)MessBuf.data[1];
		SHORT MagicType = (SHORT)MessBuf.data[2];
		SHORT MagicResult = (SHORT)MessBuf.data[3];
		CAvatar *pTarget;
		
		MultiPrintf ( "CheckMagic: unit:%d at enemy unit:%d type:%d result:%d\n", 
			pAvatar->fBtlCap.UnitIndex, 
			EnemyUnit,
			MagicType,
			MagicResult
			);

		// find the person who is my enemy
		for(i=0;i<3;i++)
		{
			for(j=0;j<5;j++)
			{
				if(GetWhosWhere(i,j,WW_HOME) != fERROR)
				{
					pTarget = (CAvatar *) BLKPTR(GetWhosWhere(i,j,WW_HOME));
					if (pTarget->fBtlCap.UnitIndex == EnemyUnit)
						Enemy = pTarget->hThis;
				}
			}
		}
			
		if (Enemy == fERROR)
			return;
			
		MultiPrintf ( "CheckMagic: unit:%d found %d, who is now my enemy\n",
			pAvatar->fBtlCap.UnitIndex, 
			Enemy
			);

		pAvatar->hEnemy = Enemy;
		pAvatar->fBtlCap.fBtlMagicType = MagicType;
		pAvatar->fBtlCap.fBtlMagicResult = MagicResult;
		
		if(pAvatar->fBtlCap.fBtlMagicType == BTLCAP_MAGIC_MISSILE)
		{
			pAvatar->fBtlCap.Shoot = BTLCAP_MISSILE;
		}
		else
		if(pAvatar->fBtlCap.fBtlMagicType == BTLCAP_MAGIC_FIRE)
		{
			pAvatar->fBtlCap.Shoot = BTLCAP_MAGIC_FIRE;
		}
		else
		if(pAvatar->fBtlCap.fBtlMagicType == BTLCAP_MAGIC_PLASMA)
		{
			pAvatar->fBtlCap.Shoot = BTLCAP_MAGIC_FIRE;
		}
		else
		if(pAvatar->fBtlCap.fBtlMagicType == BTLCAP_MAGIC_LIGHTNING)
		{
			pAvatar->fBtlCap.Shoot = BTLCAP_MAGIC_FIRE;
		}
		else
		if(pAvatar->fBtlCap.fBtlMagicType == BTLCAP_MAGIC_DESOLVE)
		{
			pAvatar->fBtlCap.Shoot = BTLCAP_MAGIC_DESOLVE;
		}
		
		pAvatar->fBtlCap.mfShoot( pAvatar );
		
		fUpdatePanels = TRUE;

	}

#endif

}

/* ========================================================================
   Function	- CheckMulti
   Description - Check remote battle captains for actions etc.
   Returns	    - 
   ======================================================================== */

void CheckMulti ( CAvatar *pAvatar, CAvatar::AISTATUS Status )
{
#ifdef _WINDOWS

	CheckDisconnect();

	if ( dwBattleMultiId != kNoMultiId )
	{
		CheckMagic( pAvatar );
		
		CheckRout ( pAvatar );
		
		CheckFallBack ( pAvatar );
		
		CheckReserves ( pAvatar );
		
		//---- Check for remote battle captain AI
		CheckResults( pAvatar );
		
		//---- Check for battle over condition 
		CheckWin ( );
	}

#endif

}	//---- End of CheckMulti()

/* ========================================================================
   Function    - MultiPrintf
   Description - print out the multiplayer debug messages
                 _WINDOWS _DEBUG ONLY
   Returns     - void
   ======================================================================== */
void MultiPrintf(const char *format, ...)
{
#if defined(_WINDOWS) && defined(_DEBUG)
	char textbuffer[200];
	va_list argp;
	FILE *fPtr;
	
	va_start(argp, format);
	vsprintf(textbuffer,format,argp);
	
	fPtr = fopen ("battle.log","at");
	if (fPtr != NULL)
	{
		fprintf(fPtr,"%s",textbuffer);
		fclose(fPtr);
	}
#endif
}

