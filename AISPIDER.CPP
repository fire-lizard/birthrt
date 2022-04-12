/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aispider.cpp  -
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "AVATAR.HXX"
#include "PLAYSTAT.HXX"
#include "SOUND.HXX"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
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

/* ------------------------------------------------------------------------
   Local Variables
   ------------------------------------------------------------------------ */
// GWP May need to be stored on a per art type basis in gametypes.
// GWP Hack for Testing purposes.
static LONG DropDistance = 100;

/* ========================================================================
   Function	- Spiders
   Description - AI function to bounce off the monster box looking for 
   			     the "player" to attack.
   Returns	 - Current state
   ======================================================================== */
void CAvatar::Spiders (CAvatar *pAvatar, CAvatar::AISTATUS Status)
{
	LONG FloorHeight;
	LONG CeilingHeight;
	LONG Special;
	
	//GEH check to see this this live guy is even remotely able to be seen
	// before we bother running any AI
	if( player.ThingIndex != fERROR &&
		!reject( pAvatar->mfGetSector(), mythings[player.ThingIndex].sect))
	{
		return;
	}

	if(pAvatar->Status != AI_DEAD)
		pAvatar->mfCheckCrush();

	if(pAvatar->Status == AI_DEAD)
		Status = AI_DEAD;
	
	switch( Status )
	{
	case AI_INIT:
			pAvatar->Status = AI_SEARCH;
			pAvatar->hEnemy = fERROR;
			pAvatar->lastBump = iNOTHING;
			pAvatar->bumpTimer = 100;
			pAvatar->dAngle = 8;
			pAvatar->fContactDistance = 60;
			
			if (pAvatar->mfGetArtSequence() != DEADSEQ)
			{
				pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
				if (HangsFromCeiling(pAvatar->ThingIndex))
				{
					// Stick it on the ceiling!
					pAvatar->mfMoveToXYZA(pAvatar->mfX(),pAvatar->mfY(),
					                      CeilingHeight - pAvatar->mfHeight(),pAvatar->mfAngle());
				}
			}
			
			pAvatar->fSpider.OrigType = pAvatar->mfType(); // for ceiling spiders.
			
			if (pAvatar->mfStandWaiting())
			{
				pAvatar->mfStartAnimationLoop(STANDSEQ);
			}
			else
			{
				pAvatar->mfStartAnimationLoop(WALKSEQ);
			}

			pAvatar->fSpider.Movement.dx = 0;
			pAvatar->fSpider.Movement.dy = pAvatar->mfGetWalkRate();
			
			if (pAvatar->mfTeleportHome())
			{
				// Remember where "Home" is.
				pAvatar->fSpider.HomeX = pAvatar->mfX();
				pAvatar->fSpider.HomeY = pAvatar->mfY();
				pAvatar->fSpider.HomeZ = pAvatar->mfZ();
				pAvatar->fSpider.HomeA = pAvatar->mfAngle();
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
	
	case AI_FALLING:
			{
			pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
			
			if (pAvatar->mfZ() > FloorHeight)
			{
				const LONG DeltaZ = pAvatar->mfZ() - FloorHeight;
				
				if (DeltaZ < pAvatar->fSpider.mfFallRate())
				{
					pAvatar->mfMoveToXYZA(pAvatar->mfX(), pAvatar->mfY(),
					                      FloorHeight, pAvatar->mfAngle());
				}
				else
				{
					pAvatar->mfMoveToXYZA(pAvatar->mfX(), pAvatar->mfY(),
					                      pAvatar->mfZ() - pAvatar->fSpider.mfFallRate(), pAvatar->mfAngle());
				}
			}
			else
			{
				pAvatar->Status = AI_MOVING;
			}
			
			}
			break;
			
	case AI_DEAD:
			// Dead is dead. Don't move.
			pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
			pAvatar->mfMoveToXYZA(pAvatar->mfX(),
								  pAvatar->mfY(),
								  FloorHeight,
								  pAvatar->mfAngle());
			if (pAvatar->mfGetArtSequence() != DEADSEQ)
			{
				
				if (pAvatar->mfType() == CEILING_SPIDER)
				{
					pAvatar->mfSetType(GIANT_SPIDER);
					pAvatar->mfPlayAnimationOnce(EXPIRESEQ);
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
						pAvatarTarget->mfDisEngage();
						pAvatar->hEnemy = fERROR;
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
			
				// If you can be seen you are active no matter what.
				// if (pAvatar->mfIsVisible())
				// {
				// 	pAvatar->Status = AI_MOVING;
				// 	// May want to add a Blech goto here.
				// }
				// else
				
				if (pAvatar->mfCanSeeAvatar(pAvatarTarget) &&
				    pAvatarTarget->Engaged < 4)
				{
					const LONG distance = aprox_dist(
						pAvatar->mfX(), pAvatar->mfY(),
						pAvatarTarget->mfX(), pAvatarTarget->mfY());
						
					if (distance <= CAvatar::ActivationDistance)
					{
						
						pAvatar->Status = AI_MOVING;
						pAvatar->bumpTimer = 100;
						AddSndObj(SND_SPIDER_SPIT1, SND_SPIDER_SPIT_TOTAL, pAvatar->ThingIndex);
					}
				}
			}
		
			if (pAvatar->Status == AI_SEARCH)
			{
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
					const LONG tx = pAvatar->mfX() + pAvatar->fSpider.Movement.dx;
					const LONG ty = pAvatar->mfY() + pAvatar->fSpider.Movement.dy;
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
						
						//GWP move Rate amount in a reflective direction
						tp.x = 0;
						tp.y = pAvatar->mfGetWalkRate();
						pAvatar->mfReflect(Angle, NewAngle);
						if((pAvatar->lastBump == iWALL||
							pAvatar->lastBump == iFLOOR||
							pAvatar->lastBump == iHOLE||
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
						Rotate(&tp, NewAngle);
						pAvatar->lastBump = bump;
						// Give ourselves a new direction vector.
						pAvatar->fSpider.Movement.dx = tp.x;
						pAvatar->fSpider.Movement.dy = tp.y;
					}
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
				pAvatar->bumpTimer = 0;
			else if(pAvatar->bumpTimer < 6)
				pAvatar->bumpTimer++;
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
			// GWP Switch to hEnemy
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
					pAvatar->mfSetType(pAvatar->fSpider.OrigType);
					pAvatar->mfMoveToXYZA(pAvatar->fSpider.HomeX,
										  pAvatar->fSpider.HomeY,
										  pAvatar->fSpider.HomeZ,
										  pAvatar->fSpider.HomeA);
				}
			}
			else
			if (HangsFromCeiling(pAvatar->ThingIndex) == TRUE && 
			    distance < DropDistance)
			{
				if (pAvatar->mfType() == CEILING_SPIDER)
				{
					pAvatar->mfSetType(GIANT_SPIDER);
				}
				pAvatar->Status = AI_FALLING;
			}
			else
			if (pAvatar->mfCanIHitHim(pAvatarTarget, distance, pAvatar->fContactDistance))
			{
				pAvatar->Status = AI_ATTACK;
				pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
				pAvatar->mfMoveToXYZA(pAvatar->mfX(),
									  pAvatar->mfY(),
									  FloorHeight,
									  pAvatar->mfAngle());
				pAvatarTarget->mfEngage();
				// GWP May want to add a Bleech Goto here.
			}
			else
			{
				LONG Angle;
				LONG myThingBumped;
				LONG MoveAngle;
				FIXED_VECTOR_3D tp;
				
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
				    bump == iSLIDE_ON_WALL ||
				    bump == iOBJECT))
				{
					// we're stuck.
					//pAvatar->mfStartAnimationOnce(STANDSEQ);
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
						
						const WadThingType bumpResult = pAvatar->mfCheckNewPosition2(&tp, pAvatar->mfGetMarchRate());
						if(bumpResult == iNOTHING)
						{
							pAvatar->FaceTo(pAvatarTarget->mfX(), pAvatarTarget->mfY());
							pAvatar->mfMoveToXYZA((pAvatar->mfX()+PLAYER_INT_VAL(tp.dx)),
												  (pAvatar->mfY()+PLAYER_INT_VAL(tp.dy)),
												  pAvatar->mfZ()+tp.dz,
												  pAvatar->mfAngle());
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

