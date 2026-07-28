/* ========================================================================
   Copyright (c) 1990,1996	  Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aifolply.cpp
   Author:   Gary Powell
   ======================================================================== */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SYSTEM.H"
#include "ENGINE.H"

#include "AVATAR.HXX"	// includes aifolply.hxx
#include "PLAYSTAT.HXX"
#include "SCNMGR.HXX"
#include "SCENE.HXX"
#include "SOUND.HXX"

// [MDB] HACK for inventory
extern "C" {
int iHackHasKey = FALSE;
}

// GWP hack for Testing purposes.
const LONG SoundDelay = 7;
static const LONG MaxChaseDistance = 1000;
// [Issue 36] A PC gives up chasing a target after this many ticks of pursuing without
// successfully reaching it (15 seconds at the ~10 tick/second combat rate) and returns
// to the await-orders (AI_SEARCH, no enemy) state.
static const ULONG ChaseGiveUpTicks = 150;

/* ========================================================================
   Function    - mfFollowPlayer
   Description - Switch from following the adventure team to following a
                 player.
   Returns     -
   ======================================================================== */

void FOLLOW_PLAYER_DATA::mfFollowPlayer()
{
	fFollowState = THE_PLAYER;
	// Start the player out, where we currently are.
	if (fhAvatar != fERROR)
	{
		CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(fhAvatar);
		SetPlayer(pAvatar->mfX(),
		          pAvatar->mfY(),
		          pAvatar->mfZ(),
		          pAvatar->mfAngle(),
		          0);
	}
}
/* ========================================================================
   Function	- FollowPlayer
   Description - AI Proc to be a "player"
   Returns	 - Current state
   ======================================================================== */
void CAvatar::FollowPlayer (CAvatar *pAvatar, CAvatar::AISTATUS Status)
{
	LONG FloorHeight;
	LONG CeilingHeight;
	LONG Special;
	
	if ((pAvatar->fFollowPlayer.mfCanIWalkThruWall() != TRUE) && (pAvatar->Status != AI_DEAD))
		pAvatar->mfCheckCrush();

	// [Issue 36] A party member's line of sight grants the whole party awareness of EVERY
	// enemy it can currently see (not just its chosen target).  Run this once per frame in
	// the combat states so all spotted enemies become valid targets for every party member.
	switch (Status)
	{
	case AI_SEARCH:
	case AI_MOVING:
	case AI_ATTACK:
	case AI_DEFEND:
	case AI_FALLBACK:
		pAvatar->mfMarkVisibleFoesAware();
		break;
	default:
		break;
	}

	if(pAvatar->Status == AI_DEAD && Status != AI_RESURRECTION)
		Status = AI_DEAD;
	
	switch( Status )
	{
	case AI_INIT:
			pAvatar->Status = AI_RUNNING;
			pAvatar->mfSethEnemy(fERROR);
			pAvatar->fFollowPlayer.fTimeLastBump = 0;
			pAvatar->fFollowPlayer.fCurrentSpellBox = -1;
			pAvatar->fContactDistance = 90;
			
			goto MoveThisGuy;
			
	case AI_RELEASE:
			pAvatar->Status = AI_INIT;
			break;

	case AI_RESURRECTION:
			if(pAvatar->mfPlayAnimationBackward(EXPIRESEQ)==FALSE)
				pAvatar->Status = AI_RESURRECTION;
			else
				pAvatar->Status = AI_INIT;
			break;
			
	case AI_BEGIN_PAUSE:
			pAvatar->mfBeginPause();
			break;
			
	case AI_PAUSED:
			pAvatar->mfStandAndFidget();
			break;

	case AI_END_PAUSE:
			pAvatar->mfEndPause();
			break;
			
	case AI_BEGIN_LISTEN:
			pAvatar->mfBeginListen();
			break;
			
	case AI_LISTEN:
			// Follow the camera around.
			pAvatar->mfListen();
			break;
			
	case AI_END_LISTEN:
			pAvatar->mfEndListen();
			break;
			
	case AI_BEGIN_LISTEN_BOW:
			pAvatar->mfBeginListenBow();
			break;
	
	case AI_END_LISTEN_BOW:
			pAvatar->mfEndListenBow();
			break;
	
	case AI_ROTATE_TO_CAMERA:
			pAvatar->mfRotateToCamera();
			break;
	
	case AI_RETURN_TO_POSITION:
			pAvatar->mfReturnToPosition();
			break;

	case AI_CASTSPELL:
			if (pAvatar->fFollowPlayer.fCurrentSpellBox != -1)
			{
				if (pAvatar->hEnemy != fERROR)
				{
					
					CAvatar * pEnemy = (CAvatar *) BLKPTR (pAvatar->hEnemy);
				
					pAvatar->FaceTo(pEnemy->mfX(), pEnemy->mfY());
					
				}
				
				if (pAvatar->hPlayerStats == fERROR)
				{
					pAvatar->mfSethEnemy(fERROR);
					break;
				}
				
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(pAvatar->hPlayerStats);
				
				if (GAME_TTYPE::mfHasMagicAnimation(pAvatar->mfType()))
				{
					if (pAvatar->mfGetArtSequence() != MAGICSEQ)
					{
						pAvatar->mfStartAnimationOnce(MAGICSEQ);
					}
					else
					{
					    if (pAvatar->mfTestSequencePastHalf())
					    {
							pPlayerStats->Inventory.mfActivateSpellBox(pAvatar->fFollowPlayer.fCurrentSpellBox);
							// Now go back into general defense mode.
							pAvatar->fFollowPlayer.fCurrentSpellBox = -1;
							pAvatar->fDamageFlag = FALSE;
							
					    }
					}
				}
				else
				{
					pPlayerStats->Inventory.mfActivateSpellBox(pAvatar->fFollowPlayer.fCurrentSpellBox);
					// Now go back into general defense mode.
					pAvatar->fFollowPlayer.fCurrentSpellBox = -1;
					pAvatar->mfSetAttackMode(FIGHT_SEQUENCE::ATM_DEFEND);
					pAvatar->fDamageFlag = FALSE;
				}
			}
			if (pAvatar->mfTestSequenceDone())
			{
				pAvatar->mfSetAttackMode(FIGHT_SEQUENCE::ATM_DEFEND);
				pAvatar->mfSetDamageDealt(-1);
			}
			break;
			
	case AI_FALLBACK:
			// GWP DEMO HACK
			// If you don't have a current enemy go away from the nearest one
			if (pAvatar->hEnemy == fERROR)
			{
				// Find nearest un-engaged foe.
				pAvatar->mfSethEnemy( pAvatar->mfFindClosestVisibleFoe());
			}
			
			if (pAvatar->hEnemy == fERROR)
			{
				pAvatar->mfStartAnimationOnce(MARCHSEQ);
			}
			else
			{
				// For now Run Away! Run Away!
				if (pAvatar->hEnemy != fERROR)
				{
					CAvatar *pAvatarEnemy = (CAvatar *) BLKPTR(pAvatar->hEnemy);
					const LONG distance = aprox_dist(pAvatar->mfX(),
					                                 pAvatar->mfY(),
					                                 pAvatarEnemy->mfX(),
					                                 pAvatarEnemy->mfY());
					
					if (pAvatar->mfCanIHitHim(pAvatarEnemy, distance, pAvatar->fContactDistance))
					{
						VECTOR	NewVector;
						LONG	Angle;
						LONG    myThingBumped;
						const LONG	AngleToward = AngleFromPoint(
														    pAvatar->mfX(),
														    pAvatar->mfY(),
															pAvatarEnemy->mfX(),
															pAvatarEnemy->mfY(),
															RESOLUTION_8);
						
						NewVector.dx = pAvatar->mfGetWalkRate(); // GWP Magic number to backway at an angle
						NewVector.dy = -2 * pAvatar->mfGetWalkRate();
						Rotate((POINT *)&NewVector, AngleToward);
						
						if (pAvatar->mfGetArtSequence() != MARCHSEQ)
							pAvatar->mfStartAnimationLoop(MARCHSEQ);
							
						// Move away from your enemy.
						const LONG Bump = pAvatar->mfMoveTowardWithBump(
											distance,
						                    pAvatar->mfX() + NewVector.dx,
											pAvatar->mfY() + NewVector.dy,
											0,
											pAvatar->mfGetWalkRate(),
											0,
											&Angle,
											&myThingBumped
											);
											
						// Now set the player to this location.
						
						// GWP Later play the sound for this ArtThing instead of generic noise.
						switch( Bump)
						{
						case iOBJECT:
							if (SCENE_MGR::gTick > SoundDelay + pAvatar->fFollowPlayer.fTimeLastBump)
							{
								pAvatar->fFollowPlayer.fTimeLastBump = SCENE_MGR::gTick;
								if (GAME_TTYPE::mfIsAvatarType(mythings[myThingBumped].type))
								{
									if (CAvatar::mfDeadFlicSeq(myThingBumped))
									{
										AddSndObj(SND_BUMP_DEAD_BODY1,SND_BUMP_DEAD_BODY_TOTAL, pAvatar->ThingIndex);
									}
									else
									{
										AddSndObj(SND_BUMP_BODY1,SND_BUMP_BODY_TOTAL, pAvatar->ThingIndex);
									}
								}
								else
								{
									AddSndObj(SND_HIT_WALL1,SND_HIT_WALL_TOTAL, pAvatar->ThingIndex);
								}
							}
							break;
						case iWALL:
							pAvatar->mfStartAnimationOnce(STANDSEQ);
							pAvatar->mfSethEnemy(fERROR);
							
							if (SCENE_MGR::gTick > SoundDelay + pAvatar->fFollowPlayer.fTimeLastBump)
							{
								pAvatar->fFollowPlayer.fTimeLastBump = SCENE_MGR::gTick;
								AddSndObj(SND_HIT_WALL1,SND_HIT_WALL_TOTAL, pAvatar->ThingIndex);
							}
							break;
						case iSLIDE_ON_WALL:
							if (SCENE_MGR::gTick > SoundDelay + pAvatar->fFollowPlayer.fTimeLastBump)
							{
								pAvatar->fFollowPlayer.fTimeLastBump = SCENE_MGR::gTick;
								AddSndObj(SND_HIT_WALL1,SND_HIT_WALL_TOTAL, pAvatar->ThingIndex);
							}
							break;
						case iCEILING:
							// You are too tall.
							if (SCENE_MGR::gTick > SoundDelay + pAvatar->fFollowPlayer.fTimeLastBump)
							{
								pAvatar->fFollowPlayer.fTimeLastBump = SCENE_MGR::gTick;
								AddSndObj(SND_HIT_CEILING1,SND_HIT_CEILING_TOTAL, pAvatar->ThingIndex);
							}
							break;
						case iFLOOR:
							// Step too high
							if (SCENE_MGR::gTick > SoundDelay + pAvatar->fFollowPlayer.fTimeLastBump)
							{
								pAvatar->fFollowPlayer.fTimeLastBump = SCENE_MGR::gTick;
								AddSndObj(SND_JUMP_BUMP1,SND_JUMP_BUMP_TOTAL, pAvatar->ThingIndex);
							}
							break;
						}
					}
					else
					{
						// For now keep looking for the next nearest attacker.
						pAvatar->mfStartAnimationOnce(STANDSEQ);
						pAvatar->mfSethEnemy(fERROR);
					}
				}
			}
			
			// GWP END DEMO HACK
			break;
	case AI_MOVING:
			// Go after your enemy then attack.
			if (pAvatar->hEnemy != fERROR)
			{
				CAvatar *pAvatarEnemy = (CAvatar *) BLKPTR(pAvatar->hEnemy);
				
				if (pAvatarEnemy->mfAmIImmoblized())
				{
					pAvatar->Status = AI_SEARCH;
					break;
				}
				
				const LONG distance = aprox_dist(pAvatar->mfX(),
				                                 pAvatar->mfY(),
				                                 pAvatarEnemy->mfX(),
				                                 pAvatarEnemy->mfY());
				
				const BOOL movCanSee = pAvatar->mfCanSeeAvatar(pAvatarEnemy);
				if (movCanSee)
					pAvatarEnemy->mfMarkAware();
				// [Issue 36] Only begin attacking when we currently have LOS to the
				// enemy.  mfCanIHitHim() ignores walls, so without the LOS gate a hero
				// would swing through a wall at a target on the far side; instead we
				// stay in AI_MOVING and pathfind around the obstacle.  Spectres are the
				// exception: they phase through walls, so a hero may strike one once
				// close enough even without LOS.
				const BOOL movTargetIsSpectre =
					(pAvatarEnemy->GetAIFuncIndex() == AI_FUNC_SPECTRE);
				if (pAvatar->mfCanIHitHim(pAvatarEnemy, distance, pAvatar->fContactDistance) &&
					(movCanSee || movTargetIsSpectre))
				{
					// [Issue 36] Reached the target: this pursuit succeeded, so reset the
					// chase give-up timer (it restarts if we later drop back to AI_MOVING).
					pAvatar->fChaseStartTick = 0;
					pAvatar->mfRestartFightSequence();
					// GWP HACK HACK
					goto Start_Attack;
				}
				else
				{
					// [Issue 36] Still pursuing.  Start the give-up clock on the first
					// pursuit frame, and abandon the target after ChaseGiveUpTicks of
					// unsuccessful chasing, returning to await orders (AI_SEARCH, no enemy).
					if (pAvatar->fChaseStartTick == 0)
						pAvatar->fChaseStartTick = SCENE_MGR::gTick;
					else if (SCENE_MGR::gTick - pAvatar->fChaseStartTick > ChaseGiveUpTicks)
					{
						pAvatar->fChaseStartTick = 0;
						pAvatar->fFollowPlayer.dAngle = 8;
						pAvatar->fAiPath.fPathWaypointCount = 0;
						pAvatar->fAiPath.fPathTargetSector  = -1;
						pAvatar->mfSethEnemy(fERROR);
						pAvatar->Status = AI_SEARCH;
						pAvatar->mfStartAnimationOnce(STANDSEQ);
						break;
					}
					LONG Angle;
					LONG myThingBumped;

					pAvatar->mfStartAnimationLoop(MARCHSEQ);

					// Sector-graph A* pathfinding: route through portals toward the enemy.
					// ApplySectorPath() computes a fresh A* path when the enemy changes
					// sectors, then advances through portal waypoints until all are
					// consumed, after which movement reverts to a direct line.
					AI_MOVER_INFO mover;
					mover.width    = pAvatar->mfWidth();
					mover.height   = pAvatar->mfHeight();
					mover.z        = pAvatar->mfZ();
					mover.isFlying = FALSE;
					LONG moveTargetX = pAvatarEnemy->mfX();
					LONG moveTargetY = pAvatarEnemy->mfY();
					ApplySectorPath(&pAvatar->fAiPath,
					                pAvatar->mfGetSector(), pAvatarEnemy->mfGetSector(),
					                pAvatar->mfX(), pAvatar->mfY(),
					                pAvatarEnemy->mfX(), pAvatarEnemy->mfY(),
					                &moveTargetX, &moveTargetY, &mover);

					// Move toward the chosen target (waypoint or enemy).
					const LONG moveDist = aprox_dist(
						pAvatar->mfX(), pAvatar->mfY(), moveTargetX, moveTargetY);
					const LONG Bump = pAvatar->mfMoveTowardWithBump(
										moveDist,
					                    moveTargetX,
										moveTargetY,
										pAvatar->fContactDistance/3,
										pAvatar->mfGetMarchRate(),
										0,
										&Angle,
										&myThingBumped
										);

					// Now set the player to this location.

					// GWP Later play the sound for this ArtThing instead of generic noise.
					switch( Bump)
					{
					case iOBJECT:
						if (SCENE_MGR::gTick > SoundDelay + pAvatar->fFollowPlayer.fTimeLastBump)
						{
							pAvatar->fFollowPlayer.fTimeLastBump = SCENE_MGR::gTick;
							if (myThingBumped < MAX_THINGS && GAME_TTYPE::mfIsAvatarType(mythings[myThingBumped].type))
							{
								if (CAvatar::mfDeadFlicSeq(myThingBumped))
								{
									AddSndObj(SND_BUMP_DEAD_BODY1,SND_BUMP_DEAD_BODY_TOTAL, pAvatar->ThingIndex);
								}
								else
								{
									AddSndObj(SND_BUMP_BODY1,SND_BUMP_BODY_TOTAL, pAvatar->ThingIndex);
								}
							}
						}
						else
						{
							AddSndObj(SND_HIT_WALL1,SND_HIT_WALL_TOTAL, pAvatar->ThingIndex);
						}
						break;
					case iWALL:
						if (SCENE_MGR::gTick > SoundDelay + pAvatar->fFollowPlayer.fTimeLastBump)
						{
							pAvatar->fFollowPlayer.fTimeLastBump = SCENE_MGR::gTick;
							AddSndObj(SND_HIT_WALL1,SND_HIT_WALL_TOTAL, pAvatar->ThingIndex);
						}
						break;
					case iSLIDE_ON_WALL:
						if (SCENE_MGR::gTick > SoundDelay + pAvatar->fFollowPlayer.fTimeLastBump)
						{
							pAvatar->fFollowPlayer.fTimeLastBump = SCENE_MGR::gTick;
							AddSndObj(SND_HIT_WALL1,SND_HIT_WALL_TOTAL, pAvatar->ThingIndex);
						}
						break;
					case iCEILING:
						// You are too tall.
						if (SCENE_MGR::gTick > SoundDelay + pAvatar->fFollowPlayer.fTimeLastBump)
						{
							pAvatar->fFollowPlayer.fTimeLastBump = SCENE_MGR::gTick;
							AddSndObj(SND_HIT_CEILING1,SND_HIT_CEILING_TOTAL, pAvatar->ThingIndex);
						}
						break;
					case iFLOOR:
						if (SCENE_MGR::gTick > SoundDelay + pAvatar->fFollowPlayer.fTimeLastBump)
						{
							pAvatar->fFollowPlayer.fTimeLastBump = SCENE_MGR::gTick;
							AddSndObj(SND_JUMP_BUMP1,SND_JUMP_BUMP_TOTAL, pAvatar->ThingIndex);
						}
						// Step too high
						break;
					}

					// Lightweight wall-steering fallback for residual bumps after waypoint
					// navigation.  With proper sector-graph routing true wall bumps should be
					// uncommon; this handles edge cases such as irregular portal geometry, thin
					// doorways, or locked-door linedefs that A* cannot know about at query time.
					// The give-up threshold of 64 matches all other AI types (~17 steering
					// frames), giving stuck characters the most attempts to free themselves.
					if (Bump == iWALL || Bump == iCEILING || Bump == iFLOOR)
					{
						if (pAvatar->fFollowPlayer.dAngle > 64)
						{
							// Steering exhausted.  Clear the stored path so A* recomputes on
							// the next AI_SEARCH -> AI_MOVING transition, then give up.
							pAvatar->fFollowPlayer.dAngle = 8;
							pAvatar->fAiPath.fPathWaypointCount = 0;
							pAvatar->fAiPath.fPathTargetSector  = -1;
							pAvatar->mfSethEnemy(fERROR);
							pAvatar->Status = AI_SEARCH;
							pAvatar->mfStartAnimationOnce(STANDSEQ);
						}
						else
						{
							const LONG MoveAngle = AngleFromPoint2(
								pAvatar->mfX(), pAvatar->mfY(),
								moveTargetX, moveTargetY, 0);
							FIXED_VECTOR_3D tp;
							tp.dx = 0;
							tp.dy = (LONG)(pAvatar->mfGetMarchRate()) << PLAYER_FIXEDPT;
							tp.dz = 0;
							Rotate((POINT*)&tp, MoveAngle + pAvatar->fFollowPlayer.dAngle);
							if (pAvatar->mfCheckNewPosition2(&tp, pAvatar->mfGetMarchRate()) == iNOTHING
								&& !pAvatar->mfIsNastySector(
										pAvatar->mfX() + PLAYER_INT_VAL(tp.dx),
										pAvatar->mfY() + PLAYER_INT_VAL(tp.dy)))
							{
								pAvatar->FaceTo(pAvatarEnemy->mfX(), pAvatarEnemy->mfY());
								pAvatar->mfMoveToXYA(
									pAvatar->mfX() + PLAYER_INT_VAL(tp.dx),
									pAvatar->mfY() + PLAYER_INT_VAL(tp.dy),
									pAvatar->mfAngle());
							}
							if (pAvatar->fFollowPlayer.dAngle > 0)
								pAvatar->fFollowPlayer.dAngle = -(pAvatar->fFollowPlayer.dAngle);
							else
								pAvatar->fFollowPlayer.dAngle = -(pAvatar->fFollowPlayer.dAngle) + 8;
						}
					}
					else
					{
						pAvatar->fFollowPlayer.dAngle = 8;
					}

				}
			}
			break;
	case AI_SEARCH:		// Stay in this mode until activated.
			pAvatar->hEnemy = pAvatar->mfFindClosestFoe();
			if (pAvatar->hEnemy != fERROR)
			{
				CAvatar * const pAvatarEnemy = (CAvatar * const) BLKPTR(pAvatar->hEnemy);
				const LONG distance = aprox_dist(pAvatar->mfX(),
				                                 pAvatar->mfY(),
				                                 pAvatarEnemy->mfX(),
				                                 pAvatarEnemy->mfY());

				// [Issue 36] Awareness of every visible enemy (including this one) is
				// refreshed once per frame by mfMarkVisibleFoesAware() at the top of
				// FollowPlayer, so a hero spotting an as-yet-unaware enemy has already
				// made the whole party aware of it before this awareness gate runs.

				if (distance > MaxChaseDistance)
				{
					// Enemy found by sector reject but too far to chase -- ignore it.
					pAvatar->mfSethEnemy(fERROR);
					if ((pAvatar->mfGetArtSequence() != ATTACK1SEQ &&
					     pAvatar->mfGetArtSequence() != ATTACK2SEQ &&
					     pAvatar->mfGetArtSequence() != DEFENDSEQ)
					      || pAvatar->mfTestSequenceDone()
					   )
					{
						pAvatar->mfStartAnimationOnce(STANDSEQ);
					}
				}
				else if (!pAvatarEnemy->mfIsAware())
				{
					// [Issue 36] Awareness gate: the party has not seen this enemy
					// recently (or awareness has timed out) -- not a valid target, so
					// the hero awaits orders rather than chasing an unseen enemy.
					pAvatar->mfSethEnemy(fERROR);
					if ((pAvatar->mfGetArtSequence() != ATTACK1SEQ &&
					     pAvatar->mfGetArtSequence() != ATTACK2SEQ &&
					     pAvatar->mfGetArtSequence() != DEFENDSEQ)
					      || pAvatar->mfTestSequenceDone()
					   )
					{
						pAvatar->mfStartAnimationOnce(STANDSEQ);
					}
				}
				else if (pAvatar->mfIsNastySector(pAvatarEnemy->mfX(), pAvatarEnemy->mfY()))
				{
					// [Issue 36] Dangerous-terrain rejection: the candidate is standing
					// in lava/acid/deep water, so it is not a valid target.
					pAvatar->mfSethEnemy(fERROR);
					if ((pAvatar->mfGetArtSequence() != ATTACK1SEQ &&
					     pAvatar->mfGetArtSequence() != ATTACK2SEQ &&
					     pAvatar->mfGetArtSequence() != DEFENDSEQ)
					      || pAvatar->mfTestSequenceDone()
					   )
					{
						pAvatar->mfStartAnimationOnce(STANDSEQ);
					}
				}
				else if (pAvatar->mfCanIHitHim(pAvatarEnemy, distance, pAvatar->fContactDistance))
				{
					pAvatarEnemy->mfEngage();
					if (pAvatar->mfGetAttackMode() == FIGHT_SEQUENCE::ATM_DEFEND)
					{
						// [Issue 36] A defending PC (e.g. a spellcaster, or one halted to
						// defend) that re-acquires a target already in melee range locks
						// on but HOLDS -- it must not swing.
						pAvatar->Status = AI_DEFEND;
						// GWP HACK HACK!
						goto StartDefend;
					}
					else
					{
						// [Issue 36] An ATTACKING PC re-acquiring a target already in melee
						// range (e.g. the next enemy right after it defeated its previous
						// one) must keep fighting with its current fighting style.  The old
						// "GWP HACK HACK" forced AI_DEFEND for every style here, so a fighter
						// using slow attack 1 would stop attacking and just defend the
						// instant it picked up an adjacent foe.
						pAvatar->fChaseStartTick = 0;
						pAvatar->Status = AI_ATTACK;
						goto Start_Attack;
					}
				}
				else if (pAvatar->mfGetAttackMode() == FIGHT_SEQUENCE::ATM_DEFEND)
				{
					// [Issue 36] A defending PC (e.g. a spellcaster sitting in its default
					// `defend` stance) must HOLD position -- it must never auto-pursue into
					// melee.  Previously AI_SEARCH unconditionally went to AI_MOVING,
					// dragging defending casters into melee and out of casting range.
					// Attackers fall through to the AI_MOVING pursuit below.
					pAvatar->mfSethEnemy(fERROR);
					if ((pAvatar->mfGetArtSequence() != ATTACK1SEQ &&
					     pAvatar->mfGetArtSequence() != ATTACK2SEQ &&
					     pAvatar->mfGetArtSequence() != DEFENDSEQ)
					      || pAvatar->mfTestSequenceDone()
					   )
					{
						pAvatar->mfStartAnimationOnce(STANDSEQ);
					}
				}
				else
				{
					// [Issue 36] Begin a fresh pursuit: reset the chase give-up timer so it
					// does not inherit a stale start time from a previous target.
					pAvatar->fChaseStartTick = 0;
					pAvatar->Status = AI_MOVING;
					if ((pAvatar->mfGetArtSequence() != ATTACK1SEQ &&
					     pAvatar->mfGetArtSequence() != ATTACK2SEQ &&
					     pAvatar->mfGetArtSequence() != DEFENDSEQ)
					      || pAvatar->mfTestSequenceDone()
					   )
					{
						pAvatar->mfStartAnimationOnce(STANDSEQ);
					}
				}
			}
			else
			{
				if ((pAvatar->mfGetArtSequence() != ATTACK1SEQ &&
				     pAvatar->mfGetArtSequence() != ATTACK2SEQ &&
				     pAvatar->mfGetArtSequence() != DEFENDSEQ)
				      || pAvatar->mfTestSequenceDone()
				   )
				{
					pAvatar->mfStartAnimationOnce(STANDSEQ);
				}
			}
			break;

	case AI_ATTACK:
Start_Attack:
			// GWP DEMO HACK
			// If you don't have a current enemy go after the nearest one
			if (pAvatar->hEnemy == fERROR)
			{
				// Find nearest un-engaged foe.
				pAvatar->mfSethEnemy (pAvatar->mfFindClosestFoe());
				if (pAvatar->hEnemy != fERROR)
				{
					CAvatar *pAvatarEnemy = (CAvatar *) BLKPTR(pAvatar->hEnemy);
					const LONG acquireDist = aprox_dist(
						pAvatar->mfX(), pAvatar->mfY(),
						pAvatarEnemy->mfX(), pAvatarEnemy->mfY());
					if (acquireDist > MaxChaseDistance)
					{
						// Enemy found by sector reject but too far -- ignore, prefer
						// nearby targets.
						pAvatar->mfSethEnemy(fERROR);
					}
					else
					{
						pAvatarEnemy->mfEngage();
					}
				}

				if (pAvatar->hEnemy == fERROR)
				{
					// No usable target in range (none found or all too far to chase).
					pAvatar->Status = AI_SEARCH;
					pAvatar->mfStartAnimationOnce(STANDSEQ);
				}
			}
			// GWP END DEMO HACK
			
			// Wack your assigned enemy.
			if (pAvatar->hEnemy != fERROR)
			{
				CAvatar *pAvatarEnemy = (CAvatar *) BLKPTR(pAvatar->hEnemy);
				
				if (pAvatarEnemy->mfAmIImmoblized())
				{
					// Got'm
					pAvatar->Status = AI_SEARCH;
					pAvatar->mfSethEnemy(fERROR);
					pAvatarEnemy->mfDisEngage();
				}
				else
				{
					const LONG distance = aprox_dist(pAvatar->mfX(),
					                                 pAvatar->mfY(),
					                                 pAvatarEnemy->mfX(),
					                                 pAvatarEnemy->mfY());
					
					if (distance > MaxChaseDistance)
					{
						// Too far to chase
						pAvatar->Status = AI_SEARCH;
						pAvatarEnemy->mfDisEngage();
					}
					else
					{
						const BOOL atkCanSee = pAvatar->mfCanSeeAvatar(pAvatarEnemy);
						if (atkCanSee)
							pAvatarEnemy->mfMarkAware();
						// [Issue 36] Keep attacking only while we currently have LOS.
						// If the enemy slips behind a wall, drop back to AI_MOVING and
						// pathfind rather than continuing to swing through the wall.
						// Spectres are exempt: a hero keeps attacking a phased spectre.
						const BOOL atkTargetIsSpectre =
							(pAvatarEnemy->GetAIFuncIndex() == AI_FUNC_SPECTRE);
						if (!pAvatar->mfCanIHitHim(pAvatarEnemy, distance, pAvatar->fContactDistance) ||
						    (!atkCanSee && !atkTargetIsSpectre))
						{
							pAvatar->Status = AI_MOVING;
							pAvatarEnemy->mfDisEngage();
						}
						else
						{
						if (pAvatar->hEnemy == fERROR)
						{
							pAvatar->Status = AI_SEARCH;
						}

						pAvatar->mfTestAndGiveDamage(pAvatar->hEnemy);
						if (pAvatar->hEnemy == fERROR)
						{
							pAvatar->Status = AI_SEARCH;
						}
						else
						{
							CAvatar const * const pAvatarEnemy = (CAvatar const * const) BLKPTR(pAvatar->hEnemy);
							pAvatar->FaceTo(pAvatarEnemy->mfX(), pAvatarEnemy->mfY());
						}
						
						if ((pAvatar->mfGetArtSequence() != ATTACK1SEQ &&
						     pAvatar->mfGetArtSequence() != ATTACK2SEQ &&
						     pAvatar->mfGetArtSequence() != DEFENDSEQ)
						      || pAvatar->mfTestSequenceDone()
						      )
						{
							{
								// [d12-10-96 JPC] Chenge mfMoveToXYZA to mfMoveToXYA
								// so that player will sink over water, lava, and acid floors.
								pAvatar->mfMoveToXYA(pAvatar->mfX(),
													  pAvatar->mfY(),
													  pAvatar->mfAngle());
							}
							
							pAvatar->mfDecrementAttackTurnCount();
							if (pAvatar->mfIsMyTurnToAttack())
							{
								pAvatar->mfSetAttackMode(pAvatar->mfGetAttackMode());
								pAvatar->mfSetAttackSequence(pAvatar->mfGetAttackMode());
								pAvatar->mfSetMyTurnToAttack();
							}
							else
							{
								pAvatar->mfSetAttackSequence(FIGHT_SEQUENCE::ATM_DEFEND);
								pAvatarEnemy->mfSetMyTurnToAttack();
							}
							pAvatar->mfStartAnimationOnce(pAvatar->CurSequence);
						}
					}
				}
			}
		}
			break;

	case AI_DEFEND:
StartDefend:
			// Hold your position as best you can.
			if (pAvatar->hEnemy == fERROR)
			{
				// Find nearest un-engaged foe.
				pAvatar->mfSethEnemy(pAvatar->mfFindClosestVisibleFoe());
				if (pAvatar->hEnemy == fERROR)
				{
					// No one found, wait.
					pAvatar->mfFreezeFrame(ATTACK1SEQ, 0);
				}
			}
			
			if (pAvatar->hEnemy != fERROR)
			{
				CAvatar *pAvatarEnemy = (CAvatar *) BLKPTR(pAvatar->hEnemy);
				if (pAvatarEnemy->mfAmIImmoblized())
				{
					// Got'm
					pAvatar->Status = AI_SEARCH;
					pAvatar->mfSethEnemy(fERROR);
					pAvatarEnemy->mfDisEngage();
				}
				else
				{
					const LONG distance = aprox_dist(pAvatar->mfX(),
					                                 pAvatar->mfY(),
					                                 pAvatarEnemy->mfX(),
					                                 pAvatarEnemy->mfY());
					
					pAvatar->FaceTo(pAvatarEnemy->mfX(),
					                pAvatarEnemy->mfY());
					
					if (distance > pAvatar->fContactDistance)
					{
						// [Issue 36] Enemy backed away -- hold position, don't chase.
						// Stay in AI_DEFEND so mfFindClosestVisibleFoe() re-acquires
						// if they return.
						pAvatar->mfSethEnemy(fERROR);
						pAvatarEnemy->mfDisEngage();
						pAvatar->mfFreezeFrame(ATTACK1SEQ, 0);
					}
					else
					{
						if (pAvatar->mfGetArtSequence() != DEFENDSEQ
						    || pAvatar->mfTestSequenceDone()
						      )
						{
							{
								// [d12-10-96 JPC] Chenge mfMoveToXYZA to mfMoveToXYA
								// so that player will sink over water, lava, and acid floors.
								pAvatar->mfMoveToXYA(pAvatar->mfX(),
													  pAvatar->mfY(),
													  pAvatar->mfAngle());
							}
							
							pAvatar->mfSetAttackSequence(FIGHT_SEQUENCE::ATM_DEFEND);
							pAvatar->mfStartAnimationOnce(pAvatar->CurSequence);
						}
					}
				}
			}
			break;
			
	case AI_DEAD:
				// Bodies lie on the ground.
				pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
				pAvatar->mfMoveToXYZA(pAvatar->mfX(),
									  pAvatar->mfY(),
									  FloorHeight,
									  pAvatar->mfAngle());

			if (pAvatar->attrib.DiedLastFrame)
			{
				pAvatar->attrib.DiedLastFrame=FALSE;
			}

			if (pAvatar->CurSequence == EXPIRESEQ &&
			     pAvatar->mfTestSequenceDone())
			{
				AddSndObj(SND_BODY_FALLS_DEAD1, SND_BODY_FALLS_DEAD_TOTAL, -1 /* pAvatar->ThingIndex */);
				pAvatar->mfPlayAnimationOnce(DEADSEQ);
				
			 	pAvatar->attrib.DiedLastFrame=TRUE;	
				if(pAvatar->attrib.KilledBySpectre == TRUE)
				{
					pAvatar->mfTurnToSpectre();
					pAvatar->attrib.KilledBySpectre = FALSE;
				}
			}

			break;
			
MoveThisGuy:		
	case AI_RUNNING:
			if (pAvatar->fFollowPlayer.mfDidIMove(pAvatar->mfX(),
			                                      pAvatar->mfY(),
			                                      pAvatar->mfZ(),
			                                      pAvatar->mfAngle()))
			{
				PTR_SCENE pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
				SCENE_AI::STATE SceneState;
				SCENE_AI::SCENE_TYPE SceneType;
				LONG NewX;
				LONG NewY;
				LONG NewZ;
				LONG NewA;
				WadThingType bump;
				LONG floor, index, fallHeight, startFallZ; // [d4-25-97 JPC] added startFallZ

				if (pAvatar->fFollowPlayer.mfAmICrouching())
				{
					pAvatar->mfStartAnimationLoop(CROUCHSEQ);
				}
				else
				{
					pCurrentScene->mfGetSceneAIState(&SceneState, &SceneType);
					if (SceneType == SCENE_AI::COMBAT_SCENE)
					{
						pAvatar->mfStartAnimationLoop(MARCHSEQ);
					}
					else
					{
						pAvatar->mfStartAnimationLoop(WALKSEQ);
					}
				}
				
				pAvatar->fFollowPlayer.mfGetMyPosition(&NewX, &NewY,
				                                       &NewZ, &NewA);

				// [d4-25-97 JPC] Added new parameter startFallZ.
				bump = pAvatar->fFollowPlayer.mfBumpAndFloor (&index, &floor, &fallHeight, &startFallZ);

				// Now actually do the move.
				pAvatar->mfMoveToXYZA(NewX, NewY, NewZ, NewA);  // calls set_thing, updates mythings[]

				// Get the floor height of the sector we're now in.
				// [d4-25-97 JPC] Added startFallZ field to PLAYER structure so
				// we can calculate how far we really fell.  (Fixes bug where
				// we take falling damage when we jump over a pit, even though
				// we don't fall in.)  The startFallZ field is set in BUMP.C,
				// CheckMove().
				pAvatar->fFollowPlayer.mfBumpAndFloor (&index, &floor, &fallHeight, &startFallZ);
				if (fallHeight && floor == pAvatar->mfZ())
				{
					// [d4-18-97, 4-25-97 JPC] Added Boolean variable fResetFallHeight,
					// which is TRUE by default but will be FALSE if player
					// dies, in which case we will not reset the fall height
					// in the global player structure.  This fixes the bug
					// where the leader dies in a fall but the followers take
					// NO DAMAGE from the fall.
					// (If the leader does not die, then the followers take damage
					// based on the information stored in the breadcrumb trail.)
					LONG actualFall;
					BOOL fResetFallHeight = TRUE;

					actualFall = startFallZ - floor;
					if (actualFall > (pAvatar->mfHeight() * 3) / 2)
					{
						if (!player.Flying && !fClimb)
							fResetFallHeight = pAvatar->mfFallingDamage(bump, floor, actualFall);
					}
					// Do this whenever the leader didn't die, even if we
					// didn't call mfFallingDamage.
					if (fResetFallHeight)
						player.fallHeight = 0;
				}
			}
			else
			{
				if (pAvatar->fFollowPlayer.mfAmICrouching())
				{
					pAvatar->mfStartAnimationLoop(CROUCHSEQ);
					pAvatar->mfSetFrozen(TRUE);
				}
				else
				{
					pAvatar->mfStartAnimationLoop(STANDSEQ);
				}
			}
			
			break;
	}
}
