/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aibtlcap.hxx
   Author:   Greg Hightower
   ======================================================================== */
#ifndef _AIBTLCAP_H
#define _AIBTLCAP_H

/* -----------------------------------------------------------------
   Defines and Enums
   ----------------------------------------------------------------- */

#define BTLCAP_MARCH_RATE	5

#define ARROW_DAMAGE		5

enum {
	BTLCAP_NOSHOT = 0,
	BTLCAP_MELEE,
	BTLCAP_CHARGE,
	BTLCAP_MISSILE,
	BTLCAP_MOVING_MISSILE,
	BTLCAP_MAGIC,	// magic beyond this point
	BTLCAP_MAGIC_MISSILE,
	BTLCAP_MAGIC_FIRE,
	BTLCAP_MAGIC_PLASMA,
	BTLCAP_MAGIC_LIGHTNING,
	BTLCAP_MAGIC_DESOLVE,
	BTLCAP_MAGIC_NO_ATTACK,
};

/* -----------------------------------------------------------------
   additional includes
   ----------------------------------------------------------------- */

#if !defined(_GAMETYPE_HXX)
#include "gametype.hxx"
#endif

#if !defined(_FILEUTIL_H)
#include "fileutil.h"
#endif

/* -----------------------------------------------------------------
   class definition
   ----------------------------------------------------------------- */
class CAvatar;

class BATTLE_CAPTAIN_DATA 
{
public:
	// Write to Scene File
	void mfWriteData(FILE *fp);
	
	// Read from Scene File
	LONG mfReadData(FILE * fp);
	
	// Cleanup
	void mfDelete();
	
	// init my values
	void mfInitVals(THINGTYPE Target, THINGTYPE Troop);
	
	// what to do if clicked on
	static void mfLeftButton(CAvatar *);
	static void mfRightButton(CAvatar *);
	
	// function to show this guys inventory
	static void mfShowInventory(LONG, LONG);
	
	// battle control
	static void mfMelee(CAvatar *);
	static void mfReform(CAvatar *);
	static void mfShoot(CAvatar *);
	static void mfTryToEngageEnemyBtlCap(CAvatar *);
	static void mfRunEnemyBtlCapAI(CAvatar *);
//REMOVED CHANGED to CheckMove()	static void mfRunEnemyBtlCapRemoteAI(CAvatar *);
	static BOOL mfKillTroop(CAvatar *, THINGTYPE);
	static SHORT mfEnemyPriority(CAvatar *, SHORT, SHORT);
	static BOOL mfAutoEngage(CAvatar *, SHORT, SHORT);
	static BOOL mfAutoMove(CAvatar *, SHORT, SHORT);
	static BOOL mfAutoShoot(CAvatar *, SHORT, SHORT);
	static BOOL mfAutoMagic(CAvatar *, SHORT, SHORT);
	
	static BOOL mfMoveAhead(LONG, LONG);
	static BOOL mfMoveBack(LONG, LONG);
	static BOOL mfMoveLeft(LONG, LONG);
	static BOOL mfMoveRight(LONG, LONG);
	static void mfCharge(LONG);
	static void mfEngage(LONG);
	static void mfFallBack(LONG);
	static BOOL mfMoraleCheck(CAvatar *);
	static void mfRout(LONG,BOOL,BOOL);
	static void mfStand(LONG, LONG);
    
	static LONG mfTerrainSpeed(CAvatar *, SHORT);

    // Main AI
	
	LEADER_DATA	Leader;		// List of my followers
	THINGTYPE	TroopType;	// Type of my followers
	THINGTYPE	TargetType;	// What I'm looking for
	
	SHORT		TroopRows;	// formation of my troops
	SHORT		TroopCols;	// 
	SHORT		Row;			// Where on the battle field
	SHORT		Column;		//  we are
	SHORT		TargetRow;	// Which row and col we are going to
	SHORT		TargetCol;	// 
	LONG		TargetX;		// What coord are we moving to
	LONG		TargetY;		//
	SHORT		Rate;			// out movement rate
	SHORT		Movement;	// how far I moved the last time
	
	LONG		ActionCount;// how long until we have to do something
	LONG		KillCount;	// how long till we kill the next trooper
	LONG		MissileCount;// time when ok to fire missiles
	LONG		MagicCount;	// time when ok to fire magic
	LONG		SpellCount;	// how many shots I have
	BOOL		HasSpoken;	// I've said my lines before
	
	SHORT		UnitIndex;	// his index in the units array
	SHORT		UnitIcon;	// What kind of unit icon is he
	SHORT		OfficerIcon;// What kind of Officer is he
	SHORT		hIcon;		// His icon for the control panel
	BOOL		Aggressor;	// I attacked first
	SHORT		Healthy;		// How may hit points I can have
	SHORT		HitPoints;	// How may hit points I currently have
	SHORT		HitCount;	// How many guys die this round
	SHORT		FallBackCount;	// How many times have I needed to fall back
	SHORT		Shoot;		// Type of shooting to do
	SHORT		Shot;			// We were shot
	SHORT		fBtlMagicType;		// Type of Magic used
	SHORT		fBtlMagicResult;	// Result of Magic used
	BOOL		OnBtlField;	// FALSE=Reserves, TRUE=OntheBattleField
	BOOL		EverOnBtlField;	// TRUE=Been on the battle field at least once
	BOOL		TurnAction;	// TRUE=action available for this turn
	BOOL		TurnMove;	// TRUE=move available for this turn
	SHORT		Running;		// default should be about 2, fast about 5
	
	BOOL		WavePlaying;// Flag for whether the wave is running or not.
	SHORT		MarchWave;	// Wave file number for my type.
};

/* -----------------------------------------------------------------
   inline member functions
   ----------------------------------------------------------------- */
// Cleanup
inline void BATTLE_CAPTAIN_DATA::mfDelete()
{
	Leader.mfDelete();
	if (hIcon != fERROR)
	{
		SetPurge(hIcon);		// His icon for the control panel
	}
}

// write my class data to disk
inline void BATTLE_CAPTAIN_DATA::mfWriteData( FILE *fp )
{
	fprintf(fp, "%ld %ld\n", (LONG) TargetType, (LONG) TroopType);
}
	
// read my class data from disk
inline LONG BATTLE_CAPTAIN_DATA::mfReadData( FILE * fp )
{
	char	cpBuffer[80];
	LONG	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	
	if (Result != EOF)
	{
		Result = (2 == sscanf(cpBuffer, "%ld %ld", &TargetType, &TroopType) ) ? Result : EOF;
	}
	return Result;
}
	
// initalize my class data
inline void BATTLE_CAPTAIN_DATA::mfInitVals(
	THINGTYPE , 
	THINGTYPE 
)
{
	TargetRow = 0;
	TargetCol = 0;
	TargetX = 0;
	TargetY = 0;
	Rate = BTLCAP_MARCH_RATE;
	
	HitPoints = 0;
	FallBackCount = 0;
	Shoot = 0;
	Shot = 0;
	OnBtlField = FALSE;
	MissileCount = 0;
	MagicCount = FALSE;
	ActionCount = 0;
	KillCount = 0;
	HasSpoken = FALSE;
	
	WavePlaying = FALSE;
	MarchWave = fERROR;
	
}
	
/* -----------------------------------------------------------------
   Prototype
   ----------------------------------------------------------------- */

void	KillBattleCaptain(CAvatar *pAvatar);
BOOL	BtlCapGenerate(SHORT UnitIndex);
void	BattleMoveToReservesArea( SHORT hAvatar );
SHORT	ResolveAttack(SHORT hDefender, SHORT hAttacker, LONG AttackType);
SHORT	DecodeBattleMagic( CAvatar * );
SHORT	ResolveBattleMagic( CAvatar * );

void	SendWin ( BOOL IWin );

#endif
