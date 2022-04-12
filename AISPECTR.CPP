/* ========================================================================
   Copyright (c) 1990,1996	  Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aispectr.cpp
   Author:   Lan Zhou
   ======================================================================== */

#include "AVATAR.HXX"	// includes aipong.hxx
#include "PLAYSTAT.HXX"
#include "SOUND.HXX"


// [d11-26-96 JPC] Changed mfMoveToXYZA to mfMoveToXYA for NON-FLYING
// avatars in this file.

/* ========================================================================
   Function	- Spectres
   Description - AI function to bounce off the monster box looking for
   			     the "player" to attack.
   Returns	 - Current state
   ======================================================================== */
void CAvatar::Spectres (CAvatar *pAvatar, CAvatar::AISTATUS Status)
{
	LONG FloorHeight;
	LONG CeilingHeight;
	LONG Special;
	LONG FlyHeight;
	
	//GEH check to see this this live guy is even remotely able to be seen
	// before we bother running any AI
	if( player.ThingIndex != fERROR &&
		!reject( pAvatar->mfGetSector(), mythings[player.ThingIndex].sect))
	{
//		pAvatar->mfStartAnimationLoop(STANDSEQ);
		return;
	}
	
	if(IsFlyingThing(pAvatar->ThingIndex))
	   FlyHeight = 10;
	else
	   FlyHeight = 0;

	switch( Status )
	{
	case AI_INIT:
			
			THINGTYPE type;
			type = pAvatar->mfType();
			
			if(pAvatar->mfType() != SPECTRE
			   &&pAvatar->mfType() != T_SPECTRE
			   &&pAvatar->mfType() != WRAITH
			   &&pAvatar->mfType() != T_WRAITH)
			{
			   if(pAvatar->mfPlayAnimationBackward(EXPIRESEQ)==FALSE)
			   		break;
			}
			
			pAvatar->Status = AI_SEARCH;
			pAvatar->attrib.IssuedChallange = FALSE;
			pAvatar->hEnemy = fERROR;
			pAvatar->lastBump = iNOTHING;
			pAvatar->bumpTimer = 100;
			pAvatar->dAngle = 8;
			pAvatar->fContactDistance = 90;
			pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
			if (FlyHeight == 0)
			{
				pAvatar->mfMoveToXYA(pAvatar->mfX(),
								  pAvatar->mfY(),
			   				  pAvatar->mfAngle());
			}
			else
			{
				pAvatar->mfMoveToXYZA(pAvatar->mfX(),
								  pAvatar->mfY(),
			   					  FloorHeight+FlyHeight,
			   					  pAvatar->mfAngle());
			}

			if (pAvatar->mfStandWaiting())
			{
				pAvatar->mfStartAnimationLoop(STANDSEQ);
			}
			else
			{
				pAvatar->mfStartAnimationLoop(WALKSEQ);
			}

			pAvatar->fSpectre.Movement.dx = 0;
			pAvatar->fSpectre.Movement.dy = pAvatar->mfGetWalkRate();
			
			if (pAvatar->mfTeleportHome())
			{
				// Remember where "Home" is.
				pAvatar->fSpectre.HomeX = pAvatar->mfX();
				pAvatar->fSpectre.HomeY = pAvatar->mfY();
				pAvatar->fSpectre.HomeZ = pAvatar->mfZ();
				pAvatar->fSpectre.HomeA = pAvatar->mfAngle();
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
			if (pAvatar->mfGetArtSequence() != DEADSEQ)
			{
				pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
				if (FlyHeight == 0)
				{
					pAvatar->mfMoveToXYA(pAvatar->mfX(),
									  pAvatar->mfY(),
									  pAvatar->mfAngle());
				}
				else
				{
					pAvatar->mfMoveToXYZA(pAvatar->mfX(),
									  pAvatar->mfY(),
									  FloorHeight,
									  pAvatar->mfAngle());
				}
			}
			
			if (pAvatar->attrib.DiedLastFrame)
			{
//				pAvatar->mfSpewAll();
				pAvatar->attrib.DiedLastFrame=FALSE;
			}

			if(pAvatar->mfTestSequenceDone() &&
			     pAvatar->CurSequence != DEADSEQ)
			{
				pAvatar->mfPlayAnimationOnce(DEADSEQ);
				pAvatar->attrib.DiedLastFrame=TRUE;
				pAvatar->mfTurnHighlightOff();
				
			}
			if (pAvatar->mfTestSequenceDone() &&
				pAvatar->mfGetArtSequence() != DEADSEQ)
				pAvatar->mfPlayAnimationOnce(DEADSEQ);
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
						
						if (pAvatar->mfCanIHitHim(pAvatarTarget, distance, pAvatar->fContactDistance))
						{
							pAvatar->FaceTo(pAvatarTarget->mfX(),
							                pAvatarTarget->mfY());
							
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
						pAvatar->attrib.IssuedChallange = TRUE;
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
				const LONG tx = pAvatar->mfX() + pAvatar->fSpectre.Movement.dx;
				const LONG ty = pAvatar->mfY() + pAvatar->fSpectre.Movement.dy;
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
				    bump == iMONSTER_BOX)
				{
					// We didn't move.
					POINT	tp;
					LONG	NewAngle;
					tp.x = 0;
					tp.y = pAvatar->mfGetWalkRate();
					//GWP move Rate amount in a reflective direction
					pAvatar->mfReflect(Angle, NewAngle);
					
					//if((pAvatar->lastBump == iWALL||
					//	pAvatar->lastBump == iFLOOR ||
					//	pAvatar->lastBump == iHOLE ||
					//	pAvatar->lastBump == iCEILING)
					//   && pAvatar->bumpTimer < 5)
					//{
					//	pAvatar->mfMoveToXYZA(pAvatar->mfX()+pAvatar->fSpectre.Movement.dx;
					//						  pAvatar->mfY()+pAvatar->fSpectre.Movement.dy;
					//						  pAvatar->mfZ(),
					//						  pAvatar->mfAngle());
					//}
					//else
					if((pAvatar->lastBump == iWALL||
						pAvatar->lastBump == iFLOOR ||
						pAvatar->lastBump == iHOLE ||
						pAvatar->lastBump == iCEILING ||
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
					pAvatar->fSpectre.Movement.dx = tp.x;
					pAvatar->fSpectre.Movement.dy = tp.y;
				}
			}
			pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
			if(pAvatar->mfZ() != FloorHeight + FlyHeight)
			{
				if (FlyHeight == 0)
				{
					pAvatar->mfMoveToXYA(pAvatar->mfX(),
								  pAvatar->mfY(),
			   					  pAvatar->mfAngle());
				}
				else
				{
					pAvatar->mfMoveToXYZA(pAvatar->mfX(),
								  pAvatar->mfY(),
			   					  FloorHeight+FlyHeight,
			   					  pAvatar->mfAngle());
				}
			}

			break;
	
	case AI_MOVING:		// Chase the player
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
			
			// First check how far away the player is.
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
					if (FlyHeight == 0)
					{
						pAvatar->mfMoveToXYA(pAvatar->fSpectre.HomeX,
										  pAvatar->fSpectre.HomeY,
										  pAvatar->fSpectre.HomeA);
					}
					else
					{
						pAvatar->mfMoveToXYZA(pAvatar->fSpectre.HomeX,
										  pAvatar->fSpectre.HomeY,
										  pAvatar->fSpectre.HomeZ,
										  pAvatar->fSpectre.HomeA);
					}
				}
			}
			else
			if (pAvatar->mfCanIHitHim(pAvatarTarget, distance, pAvatar->fContactDistance))
			{
				pAvatar->Status = AI_ATTACK;
				pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
				if (FlyHeight == 0)
				{
					pAvatar->mfMoveToXYA(pAvatar->mfX(),
									  pAvatar->mfY(),
									  pAvatar->mfAngle());
				}
				else
				{
					pAvatar->mfMoveToXYZA(pAvatar->mfX(),
									  pAvatar->mfY(),
									  FloorHeight,
									  pAvatar->mfAngle());
				}
				pAvatarTarget->mfEngage();
				// GWP May want to add a Bleech Goto here.
			}
			else
			{
				
				LONG dx = pAvatarTarget->mfX()-pAvatar->mfX();
				LONG dy = pAvatarTarget->mfY()-pAvatar->mfY();
				LONG Rate = pAvatar->mfGetMarchRate();
				FIXED_VECTOR_3D NewVector;
				pAvatar->FaceTo(pAvatarTarget->mfX(), pAvatarTarget->mfY());
				
				if (ABS(distance) < Rate)
				{
					NewVector.dx = dx;
					NewVector.dy = dy;
				}
				else
				{
					NewVector.dx = ((dx * Rate)/distance);
					NewVector.dy = ((dy * Rate)/distance);
				}
				NewVector.dx <<= PLAYER_FIXEDPT;
				NewVector.dy <<= PLAYER_FIXEDPT;
				NewVector.dz = 0;
				pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
				if (FlyHeight == 0)
				{
					pAvatar->mfMoveToXYA(pAvatar->mfX() + PLAYER_INT_VAL(NewVector.dx),
						 pAvatar->mfY() + PLAYER_INT_VAL(NewVector.dy),
						 pAvatar->mfAngle());
				}
				else
				{
					pAvatar->mfMoveToXYZA(pAvatar->mfX() + PLAYER_INT_VAL(NewVector.dx),
						 pAvatar->mfY() + PLAYER_INT_VAL(NewVector.dy),
						 pAvatar->mfZ() + NewVector.dz,
						 pAvatar->mfAngle());
				}

				if(pAvatar->mfZ() != FloorHeight + FlyHeight)
				{
					if (FlyHeight == 0)
					{
						pAvatar->mfMoveToXYA(pAvatar->mfX(),
									  pAvatar->mfY(),
				   					  pAvatar->mfAngle());
					}
					else
					{
						pAvatar->mfMoveToXYZA(pAvatar->mfX(),
									  pAvatar->mfY(),
				   					  FloorHeight+FlyHeight,
				   					  pAvatar->mfAngle());
					}
				}
			}
			break;
	}
}
