/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: Avatar.hxx
   ======================================================================== */
#if !defined(_AVATAR_HXX)
#define _AVATAR_HXX

#include <stdio.h>
#include <limits.h>

#if !defined(_SYSTEM_H)
#include "system.h"
#endif

#if !defined(_ENGINE_H)
#include "engine.h"
#endif


#if !defined(_AI_UTILS_H)
#include "ai_utils.h"
#endif

#if !defined(_CTHING_HXX)
#include "cthing.hxx"
#endif

#if !defined(_AIFTSEQ_HXX)
#include "aiftseq.hxx"
#endif

#if !defined(_REALM_HXX)
#include "realm.hxx"
#endif

#if !defined(_MACHINE_H)
#include "machine.h"
#endif

#if !defined(_MENU_H)
#include "menu.h"
#endif

#if !defined(_SCNMGR_HXX)
#include "scnmgr.hxx"
#endif

#if !defined(_SOUND_HXX)
#include "sound.hxx"
#endif

#if !defined(_SPECIAL_H)
#include "special.h"
#endif

typedef SHORT HDL_AVATAR;

// ai subclasses
#if !defined(_AI_LEADER_HXX)
#include "aileader.hxx"
#endif

// ai classes
#if !defined(_AIBTLCAP_HXX)
#include "aibtlcap.hxx"
#endif

#if !defined(_AIFOLPLY_HXX)
#include "aifolply.hxx"
#endif

#if !defined(_AIFIREBL_HXX)
#include "aifirebl.hxx"
#endif

#if !defined(_AIFOLID_HXX)
#include "aifolid.hxx"
#endif

#if !defined(_AIFOLPTH_HXX)
#include "aifolpth.hxx"
#endif

#if !defined(_AIGARGL_HXX)
#include "aigargl.hxx"
#endif

#if !defined(_AIGUARD_HXX)
#include "aiguard.hxx"
#endif

#if !defined(_AIPONG_HXX)
#include "aipong.hxx"
#endif

#if !defined(_AIPTHLDR_HXX)
#include "aipthldr.hxx"
#endif

#if !defined(_AIFOLCAP_HXX)
#include "aifolcap.hxx"
#endif

#if !defined(_AISPIDER_HXX)
#include "aispider.hxx"
#endif

#if !defined(_AIHHOUND_HXX)
#include "aihhound.hxx"
#endif

#if !defined(_AIHARPY_HXX)
#include "aiharpy.hxx"
#endif

#if !defined(_AIWYVERN_HXX)
#include "aiwyvern.hxx"
#endif

#if !defined(_AISPECTR_HXX)
#include "aispectr.hxx"
#endif

#if !defined(_AIHORSE_HXX)
#include "aihorse.hxx"
#endif

#if !defined(_AISTUN_HXX)
#include "aistun.hxx"
#endif

/* -----------------------------------------------------------------
   Defines
   ----------------------------------------------------------------- */

#define PLAYER_WIDTH 16
#define PLAYER_HEIGHT 70

#define HIGHLIGHT_TICK_TIME	15

typedef enum {
	STANDSEQ	= ANIMATION0SEQ,
	WALKSEQ	   	= ANIMATION1SEQ,
	DEFENDSEQ	= ANIMATION2SEQ,
	ATTACK1SEQ	= ANIMATION3SEQ,
	ATTACK2SEQ	= ANIMATION4SEQ,
	EXPIRESEQ	= ANIMATION5SEQ,
	CROUCHSEQ	= ANIMATION6SEQ,
	SITGESTSEQ	= ANIMATION7SEQ,
	LISTENSEQ	= ANIMATION8SEQ,
	GEST1SEQ	= ANIMATION9SEQ,
	MARCHSEQ	= ANIMATION11SEQ,
	BOWSEQ		= ANIMATION10SEQ,
	DEADSEQ		= ANIMATION12SEQ,
	MAGICSEQ	= ANIMATION4SEQ,
} ANIMATION_SEQ;

#define MAX_SND_QUEUE	4

#define HIT_FRAME_CNT	0
#define BIG_HIT_FRAME_CNT	1
#define PAIN_FRAME_CNT	2
#define DEATH_SCREAM_FRAME_CNT	2

// avatar control bits
#define	AIC_ENGAGED			0x00000001
#define	AIC_UNUSED			0x00000002

#define MAX_TICK			(ULONG)-1

#define MAX_ATTACK_ROLLS	2

#define ATTACK_TURN_NOT_DECIDED		 -1

// Currently there are Five bits set by DCK which we have decree'd will drive
// the automatic AI's.
#define LARGE_PARTY_ONLY		0x01	// Otherwise remove from wad. (3 or 4 adventurers.)
#define STAND_WAITING			0x02	// Otherwise PingPong inside the Monster box.
#define TELEPORT_HOME_AFTERWARD	0x04	// otherwise do your ai where you lost the player.
#define IGNORE_MONSTER_BOX		0x08	// for chasing the player.
#define HARD_LEVEL_ONLY			0x10	// Unless hard is on.
/* -----------------------------------------------------------------
   Prototypes
   ----------------------------------------------------------------- */

SHORT AvatarIdToHdl ( LONG Id );
inline LONG AvatarHdlToId ( SHORT Hdl );
void mfReadType(FILE *, THINGTYPE *);
void mfWriteType(FILE *, THINGTYPE);
void mfReadTextType(FILE *, THINGTYPE *);
void mfWriteTextType(FILE *, THINGTYPE);

/* -----------------------------------------------------------------
   Globals
   ----------------------------------------------------------------- */

/* -----------------------------------------------------------------
   Avatar class
   ----------------------------------------------------------------- */
class SCENE;
class PLAYER_STATS;


class CAvatar : public CThing
{
public:				// Public for the dialog editor
    typedef enum {
        AI_INIT = 0,
        AI_RELEASE,
        AI_BEGIN_PAUSE,
        AI_PAUSED,
        AI_END_PAUSE,
        AI_MOVING,
        AI_READFILE,
        AI_RUNNING,
        AI_SEARCH,
        AI_ATTACK,
        AI_DEFEND,
        AI_FALLBACK,
        AI_CASTSPELL,
        AI_BEGIN_LISTEN,
        AI_LISTEN,
        AI_END_LISTEN,
        AI_BEGIN_LISTEN_BOW,
        AI_END_LISTEN_BOW,
        AI_ROTATE_TO_CAMERA,
        AI_RETURN_TO_POSITION,
        AI_DEAD,
        AI_FIDGET,
        AI_FALLING,
        AI_STONE,
        AI_CONSUME,
        AI_RESURRECTION,

		// Add above here
		AI_MAX_STATUS
    } AISTATUS;

    typedef enum
    {
        AI_FUNC_NULL = 0,
        AI_FUNC_FOLLOW_PATH,
        AI_FUNC_PING_PONG,
        AI_FUNC_BATTLE_CAPTAIN,
		AI_FUNC_FOLLOW_PLAYER,
        AI_FUNC_FOLLOW_BTLCAP,
        AI_FUNC_SPIDERS,
		AI_FUNC_HELLHOUND,
		AI_FUNC_HARPIES,
		AI_FUNC_WYVERNS,
		AI_FUNC_FIREBALL,
		AI_FUNC_SPECTRE,
		AI_FUNC_GARGOYLES,
		AI_FUNC_STUNNED,
		AI_FUNC_HORSE,
		
        // Add above here
        AI_MAX_NUMBER
    } AIFUNC;

	// Data to be used while Avatar is in Paused mode.
	typedef struct _PREV_STATE {
		AISTATUS 		Status;             // my current status before pausing.
		LONG			A;					// The angle I used to be facing.
		ANIMATION_SEQ	Sequence;			// What was it I used to be doing?
		ULONG			TimeLastRotate;		// Used to slow down a turn.
	} PREV_STATE;
	
	typedef struct _AVATAR_BIT_DATA {
		unsigned	RuntimeCreated	: 	1;	// Created by another avatar.
		unsigned	SceneInstance	:	1;	// Created by the scene. (No special .ava file)
		unsigned	Engaged			:	1;	// unused.
		unsigned	IsFlying		:	1;	// is this guy flying.
		unsigned	IsFeathered		:	1;	// is this guy feather falling.
		unsigned	CanSeeInDark	:	1;	// can this guy see in the dark
		unsigned	CanSeeInfra		:	1;	// Predator!
		unsigned	IssuedChallange :	1;	// Challanged my enemy.
		unsigned	IssuedTaunt		:	1;	// Taunted my enemy.
		unsigned	IsFollowPath    :   1;	// does this guy have a path to follow
		unsigned	KilledBySpectre :   1;
		unsigned	AttackMode		:	4;	// Uses a quick, or slow attack instead of regular slow attack.
		unsigned	PlayedFightSnd	:	1;
		unsigned	IsJumpEnhanced	:	1; // is this guy using any of various jumping/springing items or spells
		unsigned	DiedLastFrame	:	1;  //used by AI to determine when to spit out items
		unsigned	BeingControlled	:	1;	// used by combat to say who is the hot avatar.
		unsigned	KilledByLava	:	1;	// used by resurrection to determine if he can be resurrected
		
	} AVATAR_BIT_DATA;

	typedef struct _AVATAR_SND_DATA {
		BIRTHRT_SND	SndId;				// which sound to play
		SHORT		Range;				// range of the random number count
		LONG		TriggerFrame;		// gFrames count the fires this sound
		SBYTE       Loop;
		SBYTE		TotalFrames;
	} AVATAR_SND_DATA;
	

	LONG    		Version;            // which game version this avatar belongs with
	LONG    		Id;                 // Unique identifier for this avatar.
	LONG			UnitIndex;			// my index in the units array
	ANIMATION_SEQ  	CurSequence;		// Which flick am I running?
	SHORT			hThis;				// The handle from NewBlock that allocated this Avatar.
	AVATAR_BIT_DATA	attrib;				// This has info about how this avatar was created.
	AISTATUS 	    Status;             // my current status
	SHORT		    hPlayerStats;   // AD&D stats (not set for battle troops)
	REALM			Realm;				// what kingdom am I from
	SHORT			Engaged;			// Count of avatars attacking me.
	HDL_AVATAR		hEnemy;				// The handle of your target.
	THINGTYPE		KillType;			// what type of special kill
	SBYTE			fOriginalRemapColor;	// to be reset to when dead.
	SBYTE			fDamageFlag;		// When damage can be done > 0.
	SBYTE			fAttackTurnFlag;	// Number of attack swings I get before letting the enemy have a chance.
	SHORT			fDamage;			// Damage dealt by you. (For report to combat menus.)
	SHORT			fContactDistance;	// Distance in front of avatar where the sword hits.
	SBYTE			fAttackRolls[MAX_ATTACK_ROLLS];	//
	ULONG			fHighlightEndTime;
	
	FIGHT_SEQUENCE	fFightSeq;			// Fight sequence for this avatar.
	
	WadThingType    lastBump;	
	LONG            bumpTimer;			// For movement.
	LONG			dAngle;
	LONG			fTimeLastChanged;
	LONG			fFrameLastChanged;
	LONG			fTimeLastMoved;
	PREV_STATE		fPrev;
	
	AVATAR_SND_DATA	SndData[MAX_SND_QUEUE];			// snds to play on a given seq frame
	

	// Any mutually exclusive data should be stored here.
	union {
		FOLLOW_ID_DATA		fFollowId;
		PINGPONG_DATA 		fPingPong;
		FOLLOW_PLAYER_DATA	fFollowPlayer;
		BATTLE_CAPTAIN_DATA	fBtlCap;
		FOLLOW_BTLCAP_DATA	fFollowBtlCap;
		SPIDER_DATA			fSpider;
		HELLHOUND_DATA		fHellHound;
		HARPY_DATA          fHarpy;
		WYVERN_DATA 		fWyvern;
		FIREBALL_DATA		fFireBall;
		SPECTRE_DATA		fSpectre;
		STUNNED_DATA		fStunned;
		GARGOYLE_DATA		fGargoyle;
		HORSE_DATA			fHorse;
	};
	FOLLOW_PATH_DATA	fFollowPath;
	
	void Save(void);
	void DoAI(void);
	inline AISTATUS DoAI(AISTATUS st);

	void SetAIFuncIndex(AIFUNC);
	inline AIFUNC GetAIFuncIndex() const;
	
	inline void mfGetContactPoint(POINT_3D * const) const;
	
	inline BOOL mfShouldSaveToSceneFile();
	void mfWriteSceneData(FILE * const fp);
	
	inline void mfInitVals();
	inline LONG mfReadRestOfSceneData(FILE * const fp);
	
	static SHORT NewAvatar(void);		// This routine modifies memory, refresh the pointer.
	static void DeleteAvatar(SHORT hAvatar);
	
	// GWP Remove the default Realm value when aibtlcap.cpp is available.
	static LONG CAvatar::CreateAvatar(
			   LONG	Id,
			   THINGTYPE	Type,
			   LONG	X,
			   LONG	Y,
			   LONG	A,
			   CAvatar::AIFUNC	FuncIndex,
			   SCENE	*pScene ,
			   const REALM::REALM_TYPE);
			
	void mfCreateFireBall(
			   LONG Id,
			   THINGTYPE Type,
			   LONG DamageLevel) const;
			
	void mfCreateHorse(
			   LONG Id,
			   THINGTYPE Type) const;
			

	inline void mfSetOriginalRemapColor(LONG const /* remapColor */);
	inline void CAvatar::mfSetBeingControlled(BOOL const /* flag */);
	
	// The Menu Fns.
	static void mfSwitchToStandStill(LONG);
	static void mfSwitchToGuardPt(LONG);

	// start some animation
	inline void mfStartAnimationOnce(ANIMATION_SEQ const /* sequence */ );
	inline void mfPlayAnimationOnce(ANIMATION_SEQ const /* sequence */ );
	inline void mfStartAnimationLoop(ANIMATION_SEQ const  /* sequence */ );
	inline void mfStartAnimationStand(ANIMATION_SEQ const /* sequence */ );
	inline BOOL mfPlayAnimationBackward(ANIMATION_SEQ const /* sequence */ );
	inline void mfFreezeFrame(ANIMATION_SEQ const /* sequence */, SHORT const /* Frame number */);
	// Cover fn until the engine can tell us the real number.
	BOOL const mfTestSequenceDone() const;
	BOOL const mfTestSequencePastHalf() const;
	BOOL const mfTestSequenceAtHit() const;
	BOOL const mfTestSequence2ndLastFrame() const;
	inline BOOL mfTestFrame(LONG const frame);
	
	const UBYTE mfGetWalkRate() const;
	const UBYTE mfGetMarchRate() const;
	
	void mfCheckCrush();
	
	inline void mfSetDamageDealt(LONG const /* damage */ );
	inline LONG const mfGetDamageDealt() const;
	
	// check to see if I generated a sound and need to play it.
	void mfCheckSnds();
	void mfQueueSnd(BIRTHRT_SND const /* SndId */,
	               SHORT const /* range */,
	               LONG const /* inNumbFrames */);
	void mfSetLoopingSnd(BIRTHRT_SND const SndId, SBYTE totalFrames);   // start sound, has to call mfQueueSnd first
	void mfClearLoopingSnd(BIRTHRT_SND const SndId);		 // stop loop sound
	void mfClearAllLoopingSnds();		 // stop all the loop sounds

	// am I on the Home Team
	BOOL const mfIsHomeTeam() const;
	
	inline void mfSetRealm(REALM::REALM_TYPE const);
	// returns TRUE if Him is an ally of Me
	inline const BOOL mfIsAlly(CAvatar  const * const Him) const;
	
	inline void mfConvertPositionToPlayer(PLAYER &) const;
	inline void mfUpdatePositionOfPlayer(PLAYER &) const;
	WadThingType const mfCheckNewPosition(LONG const /* dx */, LONG const /* dx */, LONG const /* Rate */) const;
	WadThingType const mfCheckNewPosition2(FIXED_VECTOR_3D * const /* tp */, LONG const /* Rate */) const;
	inline const LONG mfDistanceToPlayer() const;
	
	const BOOL mfCanSeeAvatar(const CAvatar *) const;
	
	HDL_AVATAR mfFindClosestFoe() const;
	HDL_AVATAR mfFindClosestVisibleFoe() const;
	
	inline BOOL const mfCanIHitHim(CAvatar const * const /* pEnemy */ ,
								 LONG const /* distance */,
								 LONG const /* attackDist */) const;
	BOOL mfCanHarpyHitHim (CAvatar *pAvatarEnemy,
								LONG distance,
								LONG AttackDistance) const;
	
	//AI Modifier Tests. Used by the Auto AI's
	inline BOOL const mfStandWaiting() const;
	inline const BOOL mfStayInMonsterBox() const;	// After I'm active can I leave?
	inline static const BOOL mfLargePartyOnly(LONG const);
	inline static const BOOL mfHardLevelOnly(LONG const);
	inline const BOOL mfTeleportHome() const;		// When I loose the player go home?
	inline static const BOOL mfDeadFlicSeq(const LONG);
	
	inline void mfEngage(); 				// When I am attacked the enemy calls this.
	inline void mfDisEngage(); 			// When I stop being attacked, the enemy calls this.
	BOOL const mfAmIBeingAttacked() const;
	BOOL const mfAmICastingASpell() const;
	BOOL const mfAmIChasingAnEnemy() const;
	
	inline BOOL const mfAmIImmoblized() const;
	BOOL const mfAmIAlive() const;
	void mfIfAliveRestoreHealth();
	
	inline FIGHT_SEQUENCE::ATTACK_MODE const mfGetAttackMode() const;
	void mfSetAttackMode(FIGHT_SEQUENCE::ATTACK_MODE const);
	void mfSetAttackSequence(FIGHT_SEQUENCE::ATTACK_MODE const);
	
	
	inline BOOL const mfIsAllDamageDone() const;
	void mfSetDamageFlag();
	
	void mfSetMyTurnToAttack();
	void mfDecideWhoseTurnToAttack();
	inline void mfClearTurnToAttack();
	BOOL const mfIsMyTurnToAttack() const;
	inline void mfDecrementAttackTurnCount();
	
	BOOL mfWackEm(HDL_AVATAR const hEnemyTarget);
	void mfKillAvatar(CAvatar * const pAvatar);
	BOOL mfInflictMagicDamage(CAvatar * const /* pEnemy */,
							  THINGTYPE const /* KillType */,
							  LONG const /* Damage */);
	LONG const mfToHitEnemy(SHORT const /* hEnemyTarget */ ) const;
	void mfFightSounds(HDL_AVATAR const hEnemyTarget);
	void mfPainSounds();
    void mfSpewAll();  // dead guys drop stuff
	
	void mfTestAndGiveDamage(HDL_AVATAR const hEnemyTarget);
	void mfTestAndGiveMagicDamage(HDL_AVATAR const /* hEnemyTarget */,
	                              THINGTYPE const /* ThingCausingDamage */,
	                              LONG const /* Damage */);
	
	static SHORT PlayerAvatarHandle;	// The avatar that is following the player structure
	static const LONG	ActivationDistance;
	static const LONG	fgTauntDelay;	// How long to wait for the next taunt.
	
	// Which avatar is being controled by the keyboard "player."
	inline static SHORT const mfGetPlayerAvatarHandle();
	inline void  mfSetMeToThePlayer() const;
	
	static BOOL CAvatar::mfSetKillType(CAvatar *, THINGTYPE, LONG DamageLevel );
	
	void mfStartNextFightSequence();
	
	inline BOOL mfIsNastySector (LONG const x, LONG const y); // [d11-30-96 JPC] returns TRUE if
														// sector is lava, deep water, or acid

	// Magic Effects
	void mfTurnToStone();
	void mfTurnToSpectre();
	inline void mfRestartFightSequence();
	
	void mfSethEnemy(SHORT const /* EnemyHdl */);
	inline BOOL const mfAmIBeingControled() const;
	inline void mfSetHighlightAvatar();
	inline void mfSetHighlightEnemy();
	inline void mfTurnHighlightOff();
	inline void mfDoHighlight();
	
	
protected:
private:

    // The AI fns.
    static void FollowPath(CAvatar *, AISTATUS);
    static void BattleCapt(CAvatar *, AISTATUS);
    static void FollowPlayer(CAvatar *, AISTATUS);
    static void PingPong(CAvatar *, AISTATUS);
    static void FollowBtlCap(CAvatar *, AISTATUS);
    static void Spiders(CAvatar *, AISTATUS);
    static void HellHounds(CAvatar *, AISTATUS);
	static void Harpies(CAvatar *, AISTATUS);
	static void Wyverns(CAvatar *, AISTATUS);
	static void FireBall(CAvatar *, AISTATUS);
	static void Spectres(CAvatar *, AISTATUS);
	static void Gargoyles(CAvatar *, AISTATUS);
	static void Stunned(CAvatar *, AISTATUS);
	static void Horse(CAvatar *, AISTATUS);

	LONG const mfCalculateAttackModifier( SHORT const /* hEnemyTarget */) const;
	void CreateBattleTroops();
	void MoveBattleTroops();
    LONG MoveToward(LONG    /* lTargetX */,
                    LONG    /* lTargetY */,
                    LONG    /* Bump */ ,
                    LONG	/* Rate */);


	WadThingType mfMoveTowardWithBump(LONG	 /* distance */,
							LONG	 /* lTargetX */,	// 24.8
							LONG	 /* lTargetY */,	// 24.8
							LONG	 /* Bump */,
							LONG	 /* Rate */,
							SHORT	 /* sSpecial */,
							LONG	 * /* pAngle */,
							LONG     * /* pmyThingBumped */ );
	
	WadThingType mfFlyTowardWithBump(LONG	 /* distance */,
							LONG	 /* lTargetX */,	// 24.8
							LONG	 /* lTargetY */,	// 24.8
							LONG     /* lTargetZ */,
							LONG	 /* Bump */,
							LONG	 /* Rate */,
							SHORT	 /* sSpecial */,
							LONG	 * /* pAngle */,
							LONG	 * /* pmyThingBumped */ );
	
	LONG AvatarNear( THINGTYPE /* type */,
	                 LONG & /* Distance */,
	                 SHORT & /* HandleToAvatar */);


	// Subroutines to help make noise for combat.
	void mfFightMissSounds(PLAYER_STATS const * const /* pAvatarStats */,
								CAvatar * const /* pEnemyTarget */,
								PLAYER_STATS const * const /* pEnemyStats */) const;
	void mfFightStartSounds(PLAYER_STATS const * const /* pAvatarStats */) const;
	void mfDieSounds();
	void mfFightHitSounds(PLAYER_STATS const * const /* pAvatarStats */,
	                           CAvatar * const /* pEnemyTarget */,
	                           PLAYER_STATS const * const /* pEnemyStats */) const;

	void mfBeginPause();
	inline void mfEndPause();
	inline void mfBeginListen();
	void mfRotateToCamera();
	inline void mfReturnToPosition();
	inline void mfBeginListenBow();
	inline void mfEndListenBow();
	inline void mfListen();
	inline void mfEndListen();
	inline void mfStandAndFidget();
	
	LONG mfRotateToward(LONG /* Starting Angle */,
	                    LONG /* Target Angle */ );
	
	inline void mfReflect(const LONG /* Angle */,
	               LONG &/* NewAngle */) const;
	
   // [d9-19-96 JPC]
	// [d4-25-97 JPC] Added return type of BOOL
	BOOL mfFallingDamage (WadThingType bump, LONG floor, LONG fallHeight);	

	AIFUNC  	AIFuncIndex;        	// AI ENUMERATED TYPE
	void (*pAvatarFunc)(CAvatar *, AISTATUS);
	
	// These operators are defined but not implemented they will cause a
	// link error if accidently used
    CAvatar();
    ~CAvatar();
    const CAvatar& operator=( const CAvatar& );
    CAvatar(const CAvatar &);
    void *operator new( size_t stAllocateBlock);
    void operator delete( void *p );
};


// set my kingdom alliance
inline void CAvatar::mfSetRealm(REALM::REALM_TYPE const NewRealm)
{
	Realm.HomeRealm = NewRealm;
}

// returns TRUE if Him is an ally of Me
inline const BOOL CAvatar::mfIsAlly(CAvatar const * const Him) const
{
//	return( Realm.mfIsMyAlly(Him->Realm.HomeRealm) );
	return AREALLIED(Realm.HomeRealm,Him->Realm.HomeRealm);	// changed 9/3 ABC
}

// This routine calls the current AI routine every render frame.
inline void CAvatar::DoAI(void)
{
	if (pAvatarFunc != 0)
	{
		mfTestAndUseLowResArt();
		mfDoHighlight();
		(*pAvatarFunc)(this, Status);
	}
	mfCheckSnds();
}

// This routine forces the AI routine to do a specific command
// returns the new status

inline CAvatar::AISTATUS CAvatar::DoAI(CAvatar::AISTATUS st)
{
	if (pAvatarFunc != 0)
	{
		mfTestAndUseLowResArt();
		mfDoHighlight();
		(*pAvatarFunc)(this, st);
	}
	
	mfCheckSnds();
	return (Status);
}


// This routine provide access for dlg_edit
inline CAvatar::AIFUNC CAvatar::GetAIFuncIndex() const
{
	return AIFuncIndex;
}

// Start a new animation. Cover func sets all our internal vars.
inline void CAvatar::mfStartAnimationOnce(ANIMATION_SEQ const ulNewSeq)
{
	CurSequence = ulNewSeq;
	fTimeLastChanged = SCENE_MGR::gTick;
	fFrameLastChanged = SCENE_MGR::gFrame;
	attrib.PlayedFightSnd = FALSE;
	// GWP Need routine that return error condition.
	LoopSequenceOnce(ThingIndex, ulNewSeq);
	if ((attrib.AttackMode == FIGHT_SEQUENCE::ATM_QUICK_HIGH ||
	    attrib.AttackMode == FIGHT_SEQUENCE::ATM_QUICK_LOW) &&
	    (ulNewSeq == ATTACK1SEQ ||
	     ulNewSeq == ATTACK2SEQ))
	{
		mfSetQuickAnimation(TRUE);
	}
}

// Start a new animation. Cover func sets all our internal vars.
inline void CAvatar::mfPlayAnimationOnce(ANIMATION_SEQ const ulNewSeq)
{
	CurSequence = ulNewSeq;
	fTimeLastChanged = SCENE_MGR::gTick;
	fFrameLastChanged = SCENE_MGR::gFrame;
	attrib.PlayedFightSnd = FALSE;
	// GWP Need routine that return error condition.
	PlaySequenceOnce(ThingIndex, ulNewSeq);
	if ((attrib.AttackMode == FIGHT_SEQUENCE::ATM_QUICK_HIGH ||
	    attrib.AttackMode == FIGHT_SEQUENCE::ATM_QUICK_LOW) &&
	    (ulNewSeq == ATTACK1SEQ ||
	     ulNewSeq == ATTACK2SEQ))
	{
		mfSetQuickAnimation(TRUE);
	}
}

// Switch to a new standing still frame. Cover func sets all our internal vars.
inline void CAvatar::mfStartAnimationStand(ANIMATION_SEQ const ulNewSeq)
{
	CurSequence = ulNewSeq;
	fTimeLastChanged = SCENE_MGR::gTick;
	fFrameLastChanged = SCENE_MGR::gFrame;
	attrib.PlayedFightSnd = FALSE;
	// GWP Need routine that return error condition and sets the frame!.
	LoopSequenceOnce(ThingIndex, ulNewSeq);
	if ((attrib.AttackMode == FIGHT_SEQUENCE::ATM_QUICK_HIGH ||
	    attrib.AttackMode == FIGHT_SEQUENCE::ATM_QUICK_LOW) &&
	    (ulNewSeq == ATTACK1SEQ ||
	     ulNewSeq == ATTACK2SEQ))
	{
		mfSetQuickAnimation(TRUE);
	}
}

// Start a new animation loop. Cover func sets all our internal vars.
inline void CAvatar::mfStartAnimationLoop(ANIMATION_SEQ const ulNewSeq)
{
	CurSequence = ulNewSeq;
	fTimeLastChanged = SCENE_MGR::gTick;
	fFrameLastChanged = SCENE_MGR::gFrame;
	attrib.PlayedFightSnd = FALSE;
	LoopSequenceContinuously(ThingIndex, ulNewSeq);
	if ((attrib.AttackMode == FIGHT_SEQUENCE::ATM_QUICK_HIGH ||
	    attrib.AttackMode == FIGHT_SEQUENCE::ATM_QUICK_LOW) &&
	    (ulNewSeq == ATTACK1SEQ ||
	     ulNewSeq == ATTACK2SEQ))
	{
		mfSetQuickAnimation(TRUE);
	}
}

// Start a new animation and play it backwards
inline BOOL CAvatar::mfPlayAnimationBackward(ANIMATION_SEQ const ulNewSeq)
{
	SHORT Result = FALSE;
	if(bumpTimer > 8)
		bumpTimer = 8;  // use bumpTimer as a frame counter
	CurSequence = ulNewSeq;
	fTimeLastChanged = SCENE_MGR::gTick;
	fFrameLastChanged = SCENE_MGR::gFrame;
	if(bumpTimer > 0)
	{
		SetFrame(ThingIndex, ulNewSeq, (SHORT)bumpTimer);
		bumpTimer--;
	}
	else
	{
		Result = TRUE;
		bumpTimer = 100;
	}

	return Result;
}

// Set the avatar to this frame of this sequence.
inline void CAvatar::mfFreezeFrame(ANIMATION_SEQ const NewSequence , SHORT const FrameNumber)
{
	CurSequence = NewSequence;
	fTimeLastChanged = SCENE_MGR::gFrame;
	fFrameLastChanged = SCENE_MGR::gFrame;
	SetFrame(ThingIndex, NewSequence, FrameNumber);
}

// Check if all Damage has been done.
inline BOOL const CAvatar::mfIsAllDamageDone() const
{
	return (hEnemy == fERROR || 			// If you have no enemy you can't give damage.
	        (!(Status == AI_CONSUME ||
	          ((Status == AI_ATTACK || Status == AI_CASTSPELL) && fDamageFlag > 0))
	        )
	       );
}

inline BOOL CAvatar::mfTestFrame(LONG const frame)
{
	BOOL Result = FALSE;
	
	if((frame + fFrameLastChanged) < SCENE_MGR::gFrame)
		Result = TRUE;

	return Result;
}


// Begin a Listening Sequence
inline void CAvatar::mfBeginListen()
{
	mfPlayAnimationOnce(STANDSEQ);
	Status = AI_BEGIN_LISTEN_BOW;
	FaceTo(CAMERA_INT_VAL(camera.x), CAMERA_INT_VAL(camera.y));
	fPrev.TimeLastRotate = 0; // so that we'll rotate on the first pass.
	//fFreeze = TRUE; // Stop the camera movement.
}

// Wait for Bow to complete.
inline void CAvatar::mfBeginListenBow()
{
	if (mfTestSequenceDone())
	{
		Status = AI_LISTEN;
		pfVoiceInq( mfType() );
	}
}

inline void CAvatar::mfEndListenBow()
{
	if (mfTestSequenceDone())
	{
		Status = AI_RETURN_TO_POSITION;
    	fPrev.TimeLastRotate = SCENE_MGR::gTick;
	}
}

// Listen sequence
inline void CAvatar::mfListen()
{
	// Follow the camera around.
	FaceTo(CAMERA_INT_VAL(camera.x), CAMERA_INT_VAL(camera.y));
}

// Terminate a listening sequence
inline void CAvatar::mfEndListen()
{
    // Since we currently recieve no messages just resume like AI_END_PAUSE
	pfVoiceResp( mfType() );
	Status = AI_END_LISTEN_BOW;
	mfPlayAnimationOnce(BOWSEQ); // Bow once again.
	//	fFreeze = FALSE; // Let go of the camera.
}

// Get the avatar facing back where he was before we clicked on them.
inline void CAvatar::mfReturnToPosition()
{
	if (mfAngle() == fPrev.A)
	{
		Status = AI_END_PAUSE;
	}
	else
	{
		// Rotate at a reasonable speed
		if ((SCENE_MGR::gTick - fPrev.TimeLastRotate) > 1) // GWP Phoney time count.
		{
			fPrev.TimeLastRotate = SCENE_MGR::gTick;
			mfRotateToward(mfAngle(), fPrev.A);
		}
	}
}

inline void CAvatar::mfEndPause()
{
	if (Status == AI_PAUSED ||
	    Status == AI_BEGIN_PAUSE)
	{
		Status = fPrev.Status;
		mfStartAnimationLoop(fPrev.Sequence);
	}
}


// While waiting figet every so often.
inline void CAvatar::mfStandAndFidget()
{
    if (!mfAmIImmoblized() &&
        (SCENE_MGR::gTick - fTimeLastChanged) > (ULONG)(600 + random(100)))	// Time to fidget.
    {
        // Randomly choose a fidget sequence.
        switch (random(2))
        {
        case 0:
            mfPlayAnimationOnce(STANDSEQ);
            break;
        // GWP case 1:
        // GWP     mfStartAnimationStand(WALKSEQ);
        // GWP     break;
        case 1:
            mfPlayAnimationOnce(GEST1SEQ);
            break;
        }
    }
}

// Before reading the rest of the AI data call this routine.
inline void CAvatar::mfInitVals()
{
	attrib.KilledByLava=0;
	switch(GetAIFuncIndex())
	{
	case AI_FUNC_PING_PONG:
		fPingPong.mfInitVals();
		break;
	case AI_FUNC_FOLLOW_PLAYER:
		fFollowPlayer.mfInitVals(&player, hThis);
		break;
	}
}

// Once the Avatar AI is determined get that AI data.
inline LONG CAvatar::mfReadRestOfSceneData(FILE * const fp)
{
	LONG Result;
	
	switch(GetAIFuncIndex())
	{
	case AI_FUNC_FOLLOW_PATH:
		Result = fFollowPath.mfReadData(fp);
		break;
	case AI_FUNC_PING_PONG:
		Result = fPingPong.mfReadData(fp);
		break;
	case AI_FUNC_BATTLE_CAPTAIN:
		Result = fBtlCap.mfReadData(fp);
		break;
	case AI_FUNC_FOLLOW_PLAYER:
		Result = fFollowPlayer.mfReadData(fp);
		break;
	}
	return Result;
}
// Test to see if this avatar will be saved in the scene file.
inline BOOL CAvatar::mfShouldSaveToSceneFile()
{
	return ((attrib.RuntimeCreated == FALSE) ? TRUE : FALSE);
}

/* ========================================================================
   Function    - AvatarHdlToId
   Description - decode the hdl and return the Id
   Returns     -
   ======================================================================== */
inline LONG AvatarHdlToId ( SHORT Hdl )
{
	LONG	hdl = fERROR; // assume id not found
	
	if (Hdl != fERROR)
	{
		const CAvatar *pA = (const CAvatar *) BLKPTR(Hdl);
		hdl = pA->Id;
	}
	
	return hdl;
}

// Convert an Avatars current location to a Player structure.
inline void CAvatar::mfConvertPositionToPlayer(PLAYER &rPlayer) const
{
	rPlayer.x = (mfX()) << PLAYER_FIXEDPT;
	rPlayer.y = (mfY()) << PLAYER_FIXEDPT;
	rPlayer.z = mfZ();
	rPlayer.a = mfAngle();
	rPlayer.p = 0;
	rPlayer.w = mfWidth();
	rPlayer.h = mfHeight();
	rPlayer.Flying = attrib.IsFlying;
	rPlayer.ThingIndex = ThingIndex;
	rPlayer.bump = iNOTHING;
	rPlayer.BumpIndex = fERROR;
	rPlayer.currentSector = mfGetSector();
}

// For a follow player, who is now leading the player from an
// auto AI or keyboard control.
inline void CAvatar::mfUpdatePositionOfPlayer(PLAYER &rPlayer) const
{
	rPlayer.x = (mfX()) << PLAYER_FIXEDPT;
	rPlayer.y = (mfY()) << PLAYER_FIXEDPT;
	rPlayer.z = mfZ();
	rPlayer.a = mfAngle();
	rPlayer.p = 0;
	rPlayer.currentSector = mfGetSector();
}

/* ========================================================================
   Function    - mfDistanceToPlayer
   Description - Get the distance to the player.
   Returns     - Distance to player.
   ======================================================================== */
inline const LONG CAvatar::mfDistanceToPlayer() const
{
	// Note: If the camera is following the player the distance computed
	//       by the engine is correct, otherwise recalculate.
	
	return ((SCENE_MGR::fgCamera.mfGetCurrentState() == GAME_CAMERA::FOLLOW_PLAYER) ?
	         				mythings[ThingIndex].dist >> PLAYER_FIXEDPT:
							aprox_dist(mfX(),
									   mfY(),
									   PLAYER_INT_VAL(player.x),
									   PLAYER_INT_VAL(player.y)));
}


/* ========================================================================
   Function    - mfCanIHitHim
   Description - inline Check whether the Avatar hit the enmey.
   Returns     - TRUE | FALSE
   ======================================================================== */
inline BOOL const CAvatar::mfCanIHitHim (
	CAvatar const * const pAvatarEnemy,
	LONG const distance,
	LONG const AttackDistance) const
{
	BOOL Result;
	
	LONG const ArmHeight= mfZ() + ((4*mfHeight())/5);
	LONG const EnemyArmHeight= pAvatarEnemy->mfZ() + ((4*pAvatarEnemy->mfHeight())/5);
	LONG const KneeHeight = mfZ() + (mfHeight()/5);
	LONG const EnemyKneeHeight = pAvatarEnemy->mfZ() + (pAvatarEnemy->mfHeight()/5);

	// your arms must be above his knees and
	// his arms must be above your knees to attack.
	if (distance < AttackDistance &&
	    ArmHeight > EnemyKneeHeight &&
	    EnemyArmHeight > KneeHeight
	   )
	{
		Result = TRUE;
	}
	else
	{
		Result = FALSE;
	}
	
	return Result;
}
// Do I stand in position or wander in my MonsterBox waiting for the player?
// By default you ping pong around.
inline BOOL const CAvatar::mfStandWaiting() const
{
	// This bit is on by default
	// The default state is OFF
	return (mythings[ThingIndex].AIbits & STAND_WAITING) ? FALSE : TRUE;
}

// Only use these avatars for large parties in the dungeon.
inline const BOOL CAvatar::mfLargePartyOnly(LONG const t)
{
	// This bit is on by default
	// The default state is OFF.
	return (mythings[t].AIbits & LARGE_PARTY_ONLY) ? FALSE : TRUE;
}

// Can I leave my monsterbox to chase the player?
// By default, you can leave.
inline const BOOL CAvatar::mfStayInMonsterBox() const
{
	// This bit is off by default
	// The default state is ON.
	return (mythings[ThingIndex].AIbits & IGNORE_MONSTER_BOX) ? FALSE : TRUE;
}

// Does my AI switch on when you are in my sector? or By activation Distance?
// By default do not do sector activation.
inline const BOOL CAvatar::mfHardLevelOnly(LONG const t)
{
	// This bit is off by default
	// The default state is OFF
	return (mythings[t].AIbits & HARD_LEVEL_ONLY) ? TRUE : FALSE;
}

// After I loose sight of the player (Max distance) do I "go home?"
// or stay where I am doing my AI.
// By default, don't go back home.
inline const BOOL CAvatar::mfTeleportHome() const
{
	// This bit is on by default
	// The default state is ON
	return (mythings[ThingIndex].AIbits & TELEPORT_HOME_AFTERWARD) ? TRUE : FALSE;
}
	
// When I am attacked the enemy calls this.
inline void CAvatar::mfEngage()
{
	Engaged++;
}

// When I stop being attacked, the enemy calls this.
inline void CAvatar::mfDisEngage()
{
	Engaged--;
	if (Engaged < 0)
		Engaged = 0;
}
	
// Make a reflected Angle
inline void CAvatar::mfReflect(const LONG Angle, LONG &NewAngle) const
{
	// At the moment Angle is the angle of the Wall.
	// First normalize so the Wall is running due north.
	LONG A1 = mfAngle() - Angle;
	if (A1 < 0)
	{
		A1 += 256;
	}
	
	if (A1 > 192)
	{
		NewAngle = mfAngle() + (2 * (256 - A1));
		if (NewAngle > 256)
		{
			NewAngle -= 256;
		}
	}
	else
	if  (A1 > 128)
	{
		NewAngle = mfAngle() - (2 * (A1 - 128));
		if (NewAngle < 0)
		{
			NewAngle += 256;
		}
	}
	else
	if (A1 > 64)
	{
		NewAngle = mfAngle() + (2 * ( 128 - A1));
		if (NewAngle > 256)
		{
			NewAngle -= 256;
		}
	}
	else
	{
		NewAngle = mfAngle() - (2 * A1);
		if (NewAngle < 0)
		{
			NewAngle += 256;
		}
	}
}

/* ========================================================================
   Function    - mfDeadFlicSeq
   Description - inline to See whether this mything is playing dead frames.
   Returns     -
   ======================================================================== */
inline const BOOL CAvatar::mfDeadFlicSeq(const LONG ThingIndex)
{
	return ((mythings[ThingIndex].iSequence == EXPIRESEQ ||
			 mythings[ThingIndex].iSequence == DEADSEQ ) ? TRUE : FALSE);
}

// Access fn to get the player handle for inventory
inline SHORT const CAvatar::mfGetPlayerAvatarHandle()
{
	return PlayerAvatarHandle;
}

// Fn to change the player to be this avatar
inline void  CAvatar::mfSetMeToThePlayer() const
{
	PlayerAvatarHandle = hThis;
	player.ThingIndex = ThingIndex;
	player.Flying = attrib.IsFlying;
}


inline BOOL const CAvatar::mfAmIImmoblized() const
{
	return (((Status == AI_DEAD ||
	          Status == AI_STONE ||
	          Status == AI_CONSUME ) ||
	         (Status == AI_PAUSED && (fPrev.Status == AI_DEAD ||
	                                  fPrev.Status == AI_STONE ||
	                                  fPrev.Status == AI_CONSUME) ))? TRUE : FALSE);
}

inline FIGHT_SEQUENCE::ATTACK_MODE const CAvatar::mfGetAttackMode() const
{
	return ((FIGHT_SEQUENCE::ATTACK_MODE) attrib.AttackMode);
}

inline void CAvatar::mfRestartFightSequence()
{
	mfSetAttackMode(mfGetAttackMode());
	mfDecideWhoseTurnToAttack();
	if (!mfIsMyTurnToAttack())
	{
		CurSequence = DEFENDSEQ;
	}
}

/* ========================================================================
   Function		- mfIsNastySector
   Description - A KLUDGE function to prevent avatars from chasing us into
					  lava, deep water, or acid.  Called by mfMoveTowardWithBump.
					  11-26-96 JPC
					  We need this function because the bump code returns iLAVA,
					  etc., only if we're ALREADY IN the lava sector--it does
					  NOT return the type of sector we are about to move into.
   Returns	 	- TRUE if sector is nasty, FALSE if not.
   ======================================================================== */
inline BOOL CAvatar::mfIsNastySector (LONG const x, LONG const y)
{
	ULONG const iSector = point_to_sector (x, y);
	return sectors[iSector].special == SSP_DEEP_WATER ||
		sectors[iSector].special == SSP_LAVA ||
		sectors[iSector].special == SSP_ACID_FLOOR;
}


inline void CAvatar::mfSetOriginalRemapColor(LONG const remapColor )
{
	fOriginalRemapColor = remapColor;
	mfRemapColor(remapColor);
}

inline void CAvatar::mfDoHighlight()
{
	if (fHighlightEndTime < SCENE_MGR::gTick)
	{
		mfTurnHighlightOff();
	}
}

inline void CAvatar::mfSetHighlightAvatar()
{
	mfRemapColor(GRAYSCALE_REMAP);
	fHighlightEndTime = SCENE_MGR::gTick + HIGHLIGHT_TICK_TIME;
}

inline void CAvatar::mfSetHighlightEnemy()
{
	mfRemapColor(-1); // Infra-red for now.
	fHighlightEndTime = SCENE_MGR::gTick + HIGHLIGHT_TICK_TIME;
}

inline void CAvatar::mfTurnHighlightOff()
{
	mfRemapColor(fOriginalRemapColor);
	fHighlightEndTime = ULONG_MAX;
}

inline BOOL const CAvatar::mfAmIBeingControled() const
{
	return (TRUE == attrib.BeingControlled);
}

inline void CAvatar::mfSetBeingControlled(BOOL const flag)
{
	attrib.BeingControlled = flag;
}


// Compute the point at which the sword will hit.
// In BR2 get the distance in front from the art.
inline void CAvatar::mfGetContactPoint(POINT_3D * const pContactPt) const
{
	// Get the position in front of the avatar.
	pContactPt->x = 0;
	pContactPt->y = fContactDistance/2;		// Currently fContact is the distance to the enemy.
	Rotate((POINT *)pContactPt, mfAngle());
	
	// Translate to the avatar's location.
	pContactPt->x += mfX();
	pContactPt->y += mfY();
	pContactPt->z = mfZ() + (mfHeight()/2);
}

// For each time you take one of your turns, decrement the counter.
inline void CAvatar::mfDecrementAttackTurnCount()
{
	if (Status == AI_ATTACK &&
	    CurSequence != DEFENDSEQ)
	{
		fAttackTurnFlag--;
		if (fAttackTurnFlag < ATTACK_TURN_NOT_DECIDED)
		{
			fAttackTurnFlag = ATTACK_TURN_NOT_DECIDED;
		}
	}
}
inline void CAvatar::mfClearTurnToAttack()
{
	fAttackTurnFlag = ATTACK_TURN_NOT_DECIDED;
}

inline void CAvatar::mfSetDamageDealt(LONG const damage )
{
	fDamage = damage;
}
inline LONG const CAvatar::mfGetDamageDealt() const
{
	return fDamage;
}
#endif // _AVATAR_H
