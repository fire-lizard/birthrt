/* ========================================================================
   Copyright (c) 1990,1996	  Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: Avatar.cpp
   Author:   Greg Hightower & Gary Powell
   ======================================================================== */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SYSTEM.H"
#include "ENGINE.H"

#include "AVATAR.HXX"	// includes aipong.hxx
#include "PLAYSTAT.HXX"
#include "SNDVOX.HXX"
#include "SOUND.HXX"


// [d11-26-96 JPC] Changed all mfMoveToXYZA calls to mfMoveToXYA calls.

/* ========================================================================
   Function	- PingPong
   Description - AI function to bounce off the monster box looking for
   			     the "player" to attack.
   Returns	 - Current state
   ======================================================================== */
void CAvatar::PingPong (CAvatar *pAvatar, CAvatar::AISTATUS Status)
{
	LONG FloorHeight;
	LONG CeilingHeight;
	LONG Special;
	
	//GEH check to see this this live guy is even remotely able to be seen
	// before we bother running any AI
	if( player.ThingIndex != fERROR &&
		!reject( pAvatar->mfGetSector(), mythings[player.ThingIndex].sect))
	{
//		pAvatar->mfStartAnimationLoop(STANDSEQ);
		return;
	}
	
	if(pAvatar->Status != AI_DEAD)
	{
		pAvatar->mfCheckCrush();
	}
	
	if(pAvatar->Status == AI_DEAD)
		Status = AI_DEAD;

	switch( Status )
	{
	case AI_INIT:
			pAvatar->Status = AI_SEARCH;
			pAvatar->attrib.IssuedChallange = FALSE;
			pAvatar->hEnemy = fERROR;
			pAvatar->lastBump = iNOTHING;
			pAvatar->bumpTimer = 100;
			pAvatar->dAngle = 8;
			pAvatar->fPingPong.fCurrentSpell = ITEM_NO_THING;
			pAvatar->fContactDistance = 90;
			
			if (pAvatar->mfStandWaiting())
			{
				pAvatar->mfStartAnimationLoop(STANDSEQ);
			}
			else
			{
				pAvatar->mfStartAnimationLoop(WALKSEQ);
			}

			pAvatar->fPingPong.Movement.dx = 0;
			pAvatar->fPingPong.Movement.dy = pAvatar->mfGetWalkRate();
			
			if (pAvatar->mfTeleportHome())
			{
				// Remember where "Home" is.
				pAvatar->fPingPong.HomeX = pAvatar->mfX();
				pAvatar->fPingPong.HomeY = pAvatar->mfY();
				pAvatar->fPingPong.HomeZ = pAvatar->mfZ();
				pAvatar->fPingPong.HomeA = pAvatar->mfAngle();
			}
			break;
			
	case AI_RELEASE:
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
	
	case AI_DEAD:
			// Dead is dead. Don't move.
			//if (pAvatar->mfGetArtSequence() != DEADSEQ)
			//{
				pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
				pAvatar->mfMoveToXYA(pAvatar->mfX(),
									  pAvatar->mfY(),
									  pAvatar->mfAngle());
			//}
			
			if (pAvatar->attrib.DiedLastFrame)
			{
//				pAvatar->mfSpewAll();
				pAvatar->attrib.DiedLastFrame=FALSE;
			}

			if(pAvatar->mfTestSequenceDone() &&
			     pAvatar->CurSequence != DEADSEQ)
			{
				AddSndObj(SND_BODY_FALLS_DEAD1, SND_BODY_FALLS_DEAD_TOTAL, pAvatar->ThingIndex );
				pAvatar->mfPlayAnimationOnce(DEADSEQ);

				pAvatar->attrib.DiedLastFrame=TRUE;
				pAvatar->mfTurnHighlightOff();

				if(pAvatar->attrib.KilledBySpectre == TRUE)
				{
					pAvatar->mfTurnToSpectre();

				}
				break;
			}
			
			if (pAvatar->mfTestSequenceDone() &&
				pAvatar->mfGetArtSequence() != DEADSEQ)
				pAvatar->mfPlayAnimationOnce(DEADSEQ);
			break;
		
	case AI_CASTSPELL:
			if (pAvatar->hPlayerStats != fERROR)
			{
				const PTR_PLAYER_STATS pPlayerStats = (const PTR_PLAYER_STATS)BLKPTR(pAvatar->hPlayerStats);

				if (pAvatar->hEnemy == fERROR)
				{
					pAvatar->Status = AI_SEARCH;
					if(pAvatar->attrib.IsFollowPath == TRUE)
					{
						//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
						pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
						pAvatar->DoAI(CAvatar::AI_INIT);
						// break;
					}
					break;						// [d11-26-96 JPC] move line per TCW Donald.
				}
				
				
				CAvatar * pAvatarTarget = (CAvatar *) BLKPTR(pAvatar->hEnemy);
				
				if (pAvatarTarget->mfAmIImmoblized())
				{
					pAvatar->Status = AI_SEARCH;
					pAvatarTarget->mfDisEngage();
					pAvatar->hEnemy = fERROR;
					if(pAvatar->attrib.IsFollowPath == TRUE)
					{
						//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
						pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
						pAvatar->DoAI(CAvatar::AI_INIT);
						// break;
					}
					break;						// [d11-26-96 JPC] move this one, too
				}

				if (pAvatarTarget->hPlayerStats != fERROR)
				{
					if (pAvatar->mfCanSeeAvatar(pAvatarTarget))
					{
						if (GAME_TTYPE::mfHasMagicAnimation(pAvatar->mfType()))
						{
							if (pAvatar->mfGetArtSequence() != MAGICSEQ)
							{
								pAvatar->mfStartAnimationOnce(MAGICSEQ);
							}
							else
							{
							    if (pAvatar->mfTestSequence2ndLastFrame())
							    {
									if (pAvatar->hPlayerStats != fERROR)
									{
										PLAYER_STATS * const pPlayerStats = (PLAYER_STATS * const) BLKPTR(pAvatar->hPlayerStats);
										pPlayerStats->Inventory.mfActivateSpell(pAvatar->fPingPong.fCurrentSpell);
									}
									// Now go back looking for the next victim.
									pAvatar->fPingPong.fCurrentSpell = ITEM_NO_THING;
									pAvatar->fDamageFlag = 0;
									pAvatar->Status = AI_SEARCH;
							    }
							}
						}
						else
						{
							pPlayerStats->Inventory.mfActivateSpell(pAvatar->fPingPong.fCurrentSpell);
							// Now go back looking for the next victim.
							pAvatar->fPingPong.fCurrentSpell = ITEM_NO_THING;
							pAvatar->Status = AI_SEARCH;
						}
						pAvatar->FaceTo(pAvatarTarget->mfX(),
						                pAvatarTarget->mfY());
					}
					else
					{
						pAvatar->Status = AI_MOVING;
						pAvatarTarget->mfDisEngage();
						// Maybe Add a Goto here.
					}
				}
			}
			else
			{
				// GWP You have no Player stats....Hit Points????
				pAvatar->Status = AI_SEARCH;
				if (pAvatar->hEnemy != fERROR)
				{
					CAvatar *pTarget = (CAvatar *) BLKPTR(pAvatar->hEnemy);
					pTarget->mfDisEngage();
					pAvatar->hEnemy = fERROR;
				}
				if(pAvatar->attrib.IsFollowPath == TRUE)
				{
					//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
					pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
					pAvatar->DoAI(CAvatar::AI_INIT);
				}
			}
			break;
	
	case AI_DEFEND:
			if (pAvatar->hEnemy == fERROR)
			{
				pAvatar->Status = AI_SEARCH;
				if(pAvatar->attrib.IsFollowPath == TRUE)
				{
					//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
					pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
					pAvatar->DoAI(CAvatar::AI_INIT);
					break;
				}

			}
			else
			{
				CAvatar * pAvatarTarget = (CAvatar *) BLKPTR(pAvatar->hEnemy);
				
				if (pAvatarTarget->mfAmIImmoblized())
				{
					pAvatar->Status = AI_SEARCH;
					pAvatarTarget->mfDisEngage();
					pAvatar->hEnemy = fERROR;
					if(pAvatar->attrib.IsFollowPath == TRUE)
					{
						//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
						pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
						pAvatar->DoAI(CAvatar::AI_INIT);
						break;
					}

				}
				else
				{
					// First check how far away the foe is.
					// GWP change to Target Avatar here.
					const LONG distance = aprox_dist(pAvatar->mfX(), pAvatar->mfY(),
													 pAvatarTarget->mfX(), pAvatarTarget->mfY());
		
					if (pAvatar->mfCanIHitHim(pAvatarTarget, distance, pAvatar->fContactDistance))
					{
						pAvatar->FaceTo(pAvatarTarget->mfX(),
						                pAvatarTarget->mfY());
						
						if ((pAvatar->mfGetArtSequence() != ATTACK1SEQ &&
						     pAvatar->mfGetArtSequence() != ATTACK2SEQ &&
						     pAvatar->mfGetArtSequence() != DEFENDSEQ) ||
						     pAvatar->mfTestSequenceDone())
						{
							
							pAvatar->mfStartNextFightSequence();
						}
					}
					else
					{
						pAvatar->Status = AI_MOVING;
						pAvatarTarget->mfDisEngage();
						// Maybe Add a goto here.
					}
				}
			}
			break;
	case AI_ATTACK:
			if (pAvatar->hPlayerStats != fERROR)
			{
				const PTR_PLAYER_STATS pPlayerStats = (const PTR_PLAYER_STATS)BLKPTR(pAvatar->hPlayerStats);

				if (pAvatar->hEnemy == fERROR)
				{
					pAvatar->Status = AI_SEARCH;
					if(pAvatar->attrib.IsFollowPath == TRUE)
					{
						//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
						pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
						pAvatar->DoAI(CAvatar::AI_INIT);
						break;
					}

				}
				else
				{
					CAvatar * pAvatarTarget = (CAvatar *) BLKPTR(pAvatar->hEnemy);
					
					if (pAvatarTarget->mfAmIImmoblized())
					{
						pAvatar->Status = AI_SEARCH;
						pAvatarTarget->mfDisEngage();
						pAvatar->hEnemy = fERROR;
						if(pAvatar->attrib.IsFollowPath == TRUE)
						{
							//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
							pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
							pAvatar->DoAI(CAvatar::AI_INIT);
							break;
						}

					}
					else
					if(pAvatarTarget->hPlayerStats != fERROR)
					{
						
						// First check how far away the foe is.
						const LONG distance = aprox_dist(pAvatar->mfX(), pAvatar->mfY(),
														 pAvatarTarget->mfX(), pAvatarTarget->mfY());
						
						if (pAvatar->mfCanSeeAvatar(pAvatarTarget))
						{
							pAvatar->FaceTo(pAvatarTarget->mfX(),
							                pAvatarTarget->mfY());
							
							if (pAvatar->attrib.IssuedTaunt == FALSE)
							{
								AddVoxSnd(VOX_DUEL_TAUNTS, pAvatar->ThingIndex);
								pAvatar->attrib.IssuedTaunt = TRUE;
							}
							
							pAvatar->mfTestAndGiveDamage(pAvatar->hEnemy);
								
							if ((pAvatar->mfGetArtSequence() != ATTACK1SEQ &&
							     pAvatar->mfGetArtSequence() != ATTACK2SEQ &&
							     pAvatar->mfGetArtSequence() != DEFENDSEQ)
							      || pAvatar->mfTestSequenceDone()
							      )
							{
								pAvatar->mfStartNextFightSequence();
 							}
						}
						else
						{
							pAvatar->Status = AI_MOVING;
							pAvatarTarget->mfDisEngage();
							// Maybe Add a goto here.
						}
					}
				}
			}
			else
			{
				// GWP You have no Player stats....Hit Points????
				pAvatar->Status = AI_SEARCH;
				if(pAvatar->attrib.IsFollowPath == TRUE)
				{
					//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
					pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
					pAvatar->DoAI(CAvatar::AI_INIT);
				}

			}
			break;
	
	case AI_SEARCH:		// Stay in this mode until activated.

			pAvatar->hEnemy = pAvatar->mfFindClosestFoe();
			
			if (pAvatar->hEnemy != fERROR)
			{
				CAvatar * pAvatarTarget = (CAvatar *) BLKPTR(pAvatar->hEnemy);
			
				if (pAvatar->mfCanSeeAvatar(pAvatarTarget))
				{
					// check to see if this guy is too far to see
					const LONG distance = aprox_dist(
						pAvatar->mfX(), pAvatar->mfY(),
						pAvatarTarget->mfX(), pAvatarTarget->mfY());
						
					if (distance <= CAvatar::ActivationDistance)
					{
					   //	pAvatar->mfStartAnimationOnce(STANDSEQ);
					   //   break;
					
						
						pAvatar->Status = AI_MOVING;
						pAvatar->bumpTimer = 100;
						if(pAvatar->attrib.IssuedChallange == FALSE)
						{
							AddVoxSnd(VOX_CHALLENGES, pAvatar->ThingIndex);
							pAvatar->attrib.IssuedChallange = TRUE;
						}
						break;
					}
				}
			}
		
			pAvatar->hEnemy = fERROR;
			pAvatar->bumpTimer++;
			// No foes seen yet.
			if (pAvatar->mfStandWaiting())
			{
				pAvatar->mfStartAnimationOnce(STANDSEQ);
			}
			else
			{
				// Wander around inside MonsterBox.
				LONG Angle;
				LONG myThingBumped;
				const LONG tx = pAvatar->mfX() + pAvatar->fPingPong.Movement.dx;
				const LONG ty = pAvatar->mfY() + pAvatar->fPingPong.Movement.dy;
				const LONG distance = aprox_dist(
										   pAvatar->mfX(),
				                           pAvatar->mfY(),
				                           tx,
				                           ty);
				
				pAvatar->mfStartAnimationOnce(WALKSEQ);
				const WadThingType bump = pAvatar->mfMoveTowardWithBump(
				                            distance,
											tx,
						                    ty,
						                    0,
						                    pAvatar->mfGetWalkRate(),
						                    CHECKLINE_MONSTER,
						                    &Angle,
						                    &myThingBumped);
				if (bump == iWALL ||
				    bump == iFLOOR ||
				    bump == iHOLE ||
				    bump == iCEILING ||
				    bump == iOBJECT ||
				    bump == iMONSTER_BOX)
				{
					// We didn't move.
					POINT	tp;
					LONG	NewAngle;
					tp.x = 0;
					tp.y = pAvatar->mfGetWalkRate();
					//GWP move Rate amount in a reflective direction
					pAvatar->mfReflect(Angle, NewAngle);
					
					if((pAvatar->lastBump == iWALL||
						pAvatar->lastBump == iFLOOR ||
						pAvatar->lastBump == iHOLE ||
						pAvatar->lastBump == iCEILING ||
						pAvatar->lastBump == iOBJECT ||
						pAvatar->lastBump == iMONSTER_BOX)
					   && pAvatar->bumpTimer < 15)
					{
						if(random(2)==0)
							NewAngle += random(64);
						else
							NewAngle -= random(64);
					}

					pAvatar->bumpTimer = 0;
					//GWP move Rate amount in a reflective direction
					Rotate(&tp, NewAngle);
					pAvatar->lastBump = bump;
					// Give ourselves a new direction vector.
					pAvatar->fPingPong.Movement.dx = tp.x;
					pAvatar->fPingPong.Movement.dy = tp.y;
				}
			}
			break;
	
	case AI_MOVING:		// Chase the enemy
			if (pAvatar->hEnemy == fERROR)
			{
				pAvatar->Status = AI_SEARCH;
				if(pAvatar->attrib.IsFollowPath == TRUE)
				{
					//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
					pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
					pAvatar->DoAI(CAvatar::AI_INIT);
				}
				break;
			}
			
			if (pAvatar->Engaged > 0)
			{
				// We're being attacked from the back. find'm and attack back.
				pAvatar->hEnemy = pAvatar->mfFindClosestFoe();
				if (pAvatar->hEnemy == fERROR)
				{
					// Error condition. Should probably disengage myself.
					break;
				}
			}
			if(pAvatar->bumpTimer > 6)
			{
				pAvatar->bumpTimer = 0;
			}
			else if(pAvatar->bumpTimer < 6)
			{
				pAvatar->bumpTimer++;
			}
			else
			{
				pAvatar->bumpTimer = 100;
				pAvatar->hEnemy = pAvatar->mfFindClosestFoe();
				if (pAvatar->hEnemy == fERROR)
				{
					pAvatar->Status = AI_SEARCH;
					if(pAvatar->attrib.IsFollowPath == TRUE)
					{
						//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
						pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
						pAvatar->DoAI(CAvatar::AI_INIT);
					}
					break;
				}
			}
			
			CAvatar * pAvatarTarget = (CAvatar *) BLKPTR(pAvatar->hEnemy);
			
			// First check how far away the enemy target is.
			const LONG distance = aprox_dist(pAvatar->mfX(), pAvatar->mfY(),
											 pAvatarTarget->mfX(), pAvatarTarget->mfY());
			
			if (distance > CAvatar::ActivationDistance &&
			    !pAvatar->mfCanSeeAvatar(pAvatarTarget)) // Got away
			{
				pAvatar->Status = AI_SEARCH;
				if(pAvatar->attrib.IsFollowPath == TRUE)
				{
					//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
					pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
					pAvatar->DoAI(CAvatar::AI_INIT);
					break;
				}

				if (pAvatar->mfTeleportHome())
				{
					pAvatar->mfMoveToXYA(pAvatar->fPingPong.HomeX,
										  pAvatar->fPingPong.HomeY,
										  pAvatar->fPingPong.HomeA);
				}
				break;
			}
			

			// if (pAvatar->mfCanIHitHim(pAvatarTarget, distance, pAvatar->fContactDistance))
			// [d4-28-97 JPC] Added mfCanSeeAvatar test here to prevent fighting
			// through walls.  (mfCanIHitHim only checks distance and relative
			// Z coordinate, not walls.)
			// Potential problem: this might prevent some fights that should
			// occur.  (GWP brought up the problem of attacking or being
			// attacked from behind.)
			// If some fights that SHOULD happen DON'T happen, suspect this code.
			if (pAvatar->mfCanIHitHim(pAvatarTarget, distance, pAvatar->fContactDistance) &&
				pAvatar->mfCanSeeAvatar(pAvatarTarget)) // [d4-28-97 JPC]
			{
				pAvatar->Status = AI_ATTACK;
				pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
				pAvatar->mfMoveToXYA(pAvatar->mfX(),
									  pAvatar->mfY(),
									  pAvatar->mfAngle());
				pAvatarTarget->mfEngage();
				// GWP May want to add a Bleech Goto here.
				break;
			}
			
			if (GAME_TTYPE::mfHasMagicAnimation(pAvatar->mfType()))
			{
				if (pAvatar->hPlayerStats != fERROR)
				{
					PLAYER_STATS * const pPlayerStats = (PLAYER_STATS * const)BLKPTR(pAvatar->hPlayerStats);
					
					// cast no magic until some damage has been done to you.
					if (pPlayerStats->mfGetCurHitPoints() < pPlayerStats->mfGetMaxHitPoints())
					{
						if (pPlayerStats->Inventory.mfCanICastSpell(ITEM_SPELL_FLESH_TO_STONE))
						{
							pAvatar->Status = AI_CASTSPELL;
							pAvatar->fPingPong.fCurrentSpell = ITEM_SPELL_FLESH_TO_STONE;
							break;
						}
						
						if (pPlayerStats->Inventory.mfCanICastSpell(ITEM_SPELL_FIREBALL))
						{
							pAvatar->Status = AI_CASTSPELL;
							pAvatar->fPingPong.fCurrentSpell = ITEM_SPELL_FIREBALL;
						
						}
						
						if (pPlayerStats->Inventory.mfCanICastSpell(ITEM_SPELL_LIGHTNING_BOLT))
						{
							pAvatar->Status = AI_CASTSPELL;
							pAvatar->fPingPong.fCurrentSpell = ITEM_SPELL_LIGHTNING_BOLT;
							break;
						}
						
						if (pPlayerStats->Inventory.mfCanICastSpell(ITEM_SPELL_MAGIC_MISSILE))
						{
							pAvatar->Status = AI_CASTSPELL;
							pAvatar->fPingPong.fCurrentSpell = ITEM_SPELL_MAGIC_MISSILE;
							break;
						}
					}
				}
			}
			
			{
			LONG Angle;
			LONG myThingBumped;
			LONG MoveAngle;
			FIXED_VECTOR_3D tp;

			//static int iAlreadySaidHalted = FALSE;
			
			// Chase the Target.
			
			// GWP If we hit something do we care?
			// GWP maybe we should try a slightly offset angle from
			// GWP the first try and try again.
			pAvatar->mfStartAnimationOnce(MARCHSEQ);
			const WadThingType bump = pAvatar->mfMoveTowardWithBump(
			                            distance,
										pAvatarTarget->mfX(),
					                    pAvatarTarget->mfY(),
					                    pAvatar->fContactDistance/3,	// GWP HACK VALUE
					                    pAvatar->mfGetMarchRate(),
					                    (SHORT)((TRUE == pAvatar->mfStayInMonsterBox())
								                 ? CHECKLINE_MONSTER : 0),
					                    &Angle,
					                    &myThingBumped);
			if (!(bump == iNOTHING ||
			    bump == iSHALLOW_WATER ||
			    bump == iDEEP_WATER ||
			    bump == iLAVA ||
			    bump == iACID ||
			    bump == iSLIDE_ON_WALL ||
			    bump == iOBJECT))
			{
				// we're stuck.
				tp.dx = 0;
				tp.dy = (pAvatar->mfGetMarchRate())<<PLAYER_FIXEDPT;
				tp.dz = 0;
				if(pAvatar->dAngle > 64)
				{	
					pAvatar->Status = AI_SEARCH;
					if(pAvatar->attrib.IsFollowPath == TRUE)
					{
						//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
						pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
						pAvatar->DoAI(CAvatar::AI_INIT);
						break;
					}
					pAvatar->mfStartAnimationOnce(STANDSEQ);

				}
				else
				{
					MoveAngle=AngleFromPoint2(pAvatar->mfX(), pAvatar->mfY(), pAvatarTarget->mfX(), pAvatarTarget->mfY(), 0);

					Rotate((POINT*)&tp, MoveAngle+pAvatar->dAngle);

					pAvatar->mfStartAnimationOnce(MARCHSEQ);
					
					WadThingType bumpResult = pAvatar->mfCheckNewPosition2(&tp, pAvatar->mfGetMarchRate());
			
					// [d11-30-96 JPC] For a flying thing, treat various nasty sectors as nothing.
					if (pAvatar->attrib.IsFlying)
					{
						if (bumpResult == iLAVA || bumpResult == iDEEP_WATER || bumpResult == iACID)
							bumpResult = iNOTHING;
					}
			
					if(bumpResult == iNOTHING)
					{
						pAvatar->FaceTo(pAvatarTarget->mfX(), pAvatarTarget->mfY());
						// [d11-29-96 JPC] HACK--you CAN step into lava here due
						// to the oddity of the bump return code.  (It returns
						// iLAVA, etc., if the CURRENT sector is lava; it does not
						// check the new sector and we don't really want to do
						// that in the bump code because the call to get_sector_info
						// is so expensive.  Not that the call to mfIsNastySector
						// is cheap, but it does not happen as often.
						if (pAvatar->attrib.IsFlying || !pAvatar->mfIsNastySector (pAvatar->mfX()+PLAYER_INT_VAL(tp.dx),
							pAvatar->mfY()+PLAYER_INT_VAL(tp.dy)))
						{
							pAvatar->mfMoveToXYA((pAvatar->mfX()+PLAYER_INT_VAL(tp.dx)),
											  (pAvatar->mfY()+PLAYER_INT_VAL(tp.dy)),
											  pAvatar->mfAngle());
						}
					}


					if(pAvatar->dAngle>0)
						pAvatar->dAngle = -(pAvatar->dAngle);
					else
						pAvatar->dAngle = -(pAvatar->dAngle) + 8;
				}

			}
			else
			{
				pAvatar->dAngle = 8;
			}
			
			}
			break;
	}
}

/* ========================================================================
   Function	- mfSwitchToPingPong
   Description - Hide the dialog and send the avatar on the way.
   Returns	 - void
   ======================================================================== */

void PINGPONG_DATA::mfSwitchToPingPong(LONG hAvatar)
{
	CAvatar *pAvatar = (CAvatar *) BLKPTR((SHORT) hAvatar);
	// GWP POINT	tp;
	// GWP LONG	angle;
	
	//GEH move Rate amount in a random direction
	// GWP tp.x = 0;
	// GWP tp.y = -(pAvatar->Rate);
	// GWP angle = random(256);
	// GWP rot8_angle(&tp, angle);
	
	// GWP pAvatar->fPingPong.Movement.dx = tp.x;
	// GWP pAvatar->fPingPong.Movement.dy = tp.y;
	
	pAvatar->fPingPong.Movement.dx = 0;
	pAvatar->fPingPong.Movement.dy = -(pAvatar->mfGetWalkRate());
	pAvatar->fPrev.Status = CAvatar::AI_INIT;
	
	pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_PING_PONG);
	
	pAvatar->Status = CAvatar::AI_END_LISTEN;
}

