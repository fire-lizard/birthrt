
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
//const UBYTE FOLLOW_PLAYER_DATA::fMaxEntourage = 3;
const LONG SoundDelay = 7;
static const LONG MaxChaseDistance = 1000;

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
			//break;
			
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
					    //if (pAvatar->mfTestSequence2ndLastFrame())
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
						//pAvatar->mfUpdatePositionOfPlayer(*pAvatar->fFollowPlayer.fpPlayer);
						
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
				
				if (pAvatar->mfCanIHitHim(pAvatarEnemy, distance, pAvatar->fContactDistance))
				{
					pAvatar->mfRestartFightSequence();
					// GWP HACK HACK
					goto Start_Attack;
				}
				else
				{
					LONG Angle;
					LONG myThingBumped;
					
					pAvatar->mfStartAnimationLoop(MARCHSEQ);
					
					// Move toward your enemy.
					const LONG Bump = pAvatar->mfMoveTowardWithBump(
										distance,
					                    pAvatarEnemy->mfX(),
										pAvatarEnemy->mfY(),
										pAvatar->fContactDistance/3,
										pAvatar->mfGetMarchRate(),
										0,
										&Angle,
										&myThingBumped
										);
										
					// Now set the player to this location.
					//pAvatar->mfUpdatePositionOfPlayer(*pAvatar->fFollowPlayer.fpPlayer);
					
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
						if (SCENE_MGR::gTick > SoundDelay + pAvatar->fFollowPlayer.fTimeLastBump)
						{
							pAvatar->fFollowPlayer.fTimeLastBump = SCENE_MGR::gTick;
							AddSndObj(SND_JUMP_BUMP1,SND_JUMP_BUMP_TOTAL, pAvatar->ThingIndex);
						}
						// Step too high
						break;
					}
					
				}
			}
			break;
	case AI_SEARCH:		// Stay in this mode until activated.
			pAvatar->hEnemy = pAvatar->mfFindClosestVisibleFoe();
			if (pAvatar->hEnemy != fERROR)
			{
				CAvatar * const pAvatarEnemy = (CAvatar * const) BLKPTR(pAvatar->hEnemy);
				const LONG distance = aprox_dist(pAvatar->mfX(),
				                                 pAvatar->mfY(),
				                                 pAvatarEnemy->mfX(),
				                                 pAvatarEnemy->mfY());
				
				if (pAvatar->mfCanIHitHim(pAvatarEnemy, distance, pAvatar->fContactDistance))
				{
					pAvatar->Status = AI_DEFEND;
					pAvatarEnemy->mfEngage();
					// GWP HACK HACK!
					goto StartDefend;
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
				pAvatar->mfSethEnemy (pAvatar->mfFindClosestVisibleFoe());
				if (pAvatar->hEnemy != fERROR)
				{
					CAvatar *pAvatarEnemy = (CAvatar *) BLKPTR(pAvatar->hEnemy);
					pAvatarEnemy->mfEngage();
				}
				else
				{
					//pAvatar->Status = AI_SEARCH;
					//pAvatar->mfStartAnimationOnce(STANDSEQ);
					pAvatar->mfFreezeFrame(ATTACK1SEQ, 0);
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
					//pAvatar->mfStartAnimationOnce(STANDSEQ);
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
					else if (!pAvatar->mfCanIHitHim(pAvatarEnemy, distance, pAvatar->fContactDistance))
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
							//if (pAvatar->fFollowPlayer.fpPlayer->Flying == FALSE)
							{
								// [d12-10-96 JPC] Chenge mfMoveToXYZA to mfMoveToXYA
								// so that player will sink over water, lava, and acid floors.
								// pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
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
					//pAvatar->mfStartAnimationOnce(STANDSEQ);
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
					//pAvatar->mfStartAnimationOnce(STANDSEQ);
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
						// we've disengaged.
						pAvatar->Status = AI_SEARCH;
						//pAvatar->mfStartAnimationOnce(STANDSEQ);
						pAvatar->mfSethEnemy(fERROR);
						pAvatarEnemy->mfDisEngage();
					}
					else
					{
						if (pAvatar->mfGetArtSequence() != DEFENDSEQ
						    || pAvatar->mfTestSequenceDone()
						      )
						{
							//if (pAvatar->fFollowPlayer.fpPlayer->Flying == FALSE)
							{
								// [d12-10-96 JPC] Chenge mfMoveToXYZA to mfMoveToXYA
								// so that player will sink over water, lava, and acid floors.
								// pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
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
			//if (pAvatar->mfGetArtSequence() != DEADSEQ)
			//{
				// Bodies lie on the ground.
				pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
				pAvatar->mfMoveToXYZA(pAvatar->mfX(),
									  pAvatar->mfY(),
									  FloorHeight,
									  pAvatar->mfAngle());
			//}

			if (pAvatar->attrib.DiedLastFrame)
			{
//				pAvatar->mfSpewAll();
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
				// CHANGE: floor = pAvatar->mfZ();

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


/* ========================================================================
   Function	- mfSwitchToFollowPlayer
   Description - Hide the dialog and send the avatar on the way.
   Returns	 - void
   ======================================================================== */

#if 0
void FOLLOW_PLAYER_DATA::mfSwitchToFollowPlayer(LONG hAvatar)
{
	CAvatar *pAvatar = (CAvatar *) BLKPTR((SHORT) hAvatar);
	
	pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PLAYER);
	//pAvatar->fFollowPlayer.DistX = 30;
	//pAvatar->fFollowPlayer.DistY = 0;
	pAvatar->fPrev.Status = CAvatar::AI_INIT;
	
	pAvatar->Status = CAvatar::AI_END_LISTEN;
}
#endif

